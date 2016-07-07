/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Vehicle.h"
#include "ulduar.h"

enum Texts
{
    SAY_AGGRO                       = -1603230,
    SAY_SLAY_1                      = -1603231,
    SAY_SLAY_2                      = -1603232,
    SAY_LEFT_ARM_GONE               = -1603233,
    SAY_RIGHT_ARM_GONE              = -1603234,
    SAY_SHOCKWAVE                   = -1603235,
    SAY_GRAB_PLAYER                 = -1603236,
    SAY_DEATH                       = -1603237,
    SAY_BERSERK                     = -1603238,
	EMOTE_EYEBEAM                   = -1613236,
	EMOTE_RESPAWN_LEFT_HAND         = -1613237,
	EMOTE_RESPAWN_RIGHT_HAND        = -1613238,
	EMOTE_STONE_GRIP                = -1613239,
};

enum Spells
{
    SPELL_ARM_DEAD_DAMAGE_10            = 63629,
    SPELL_ARM_DEAD_DAMAGE_25            = 63979,
    SPELL_TWO_ARM_SMASH_10              = 63356,
    SPELL_TWO_ARM_SMASH_25              = 64003,
    SPELL_ONE_ARM_SMASH_10              = 63573,
    SPELL_ONE_ARM_SMASH_25              = 64006,
    SPELL_ARM_SWEEP_10                  = 63766,
    SPELL_ARM_SWEEP_25                  = 63983,
    SPELL_STONE_SHOUT_10                = 63716,
    SPELL_STONE_SHOUT_25                = 64005,
    SPELL_PETRIFY_BREATH_10             = 62030,
    SPELL_PETRIFY_BREATH_25             = 63980,
    SPELL_STONE_GRIP_10                 = 62166,
    SPELL_STONE_GRIP_25                 = 63981,
    SPELL_STONE_GRIP_DOT_10             = 64290,
    SPELL_STONE_GRIP_DOT_25             = 64292,
    SPELL_STONE_GRIP_CANCEL             = 65594,
    SPELL_SUMMON_RUBBLE                 = 63633,
    SPELL_FALLING_RUBBLE                = 63821,
    SPELL_ARM_ENTER_VEHICLE             = 65343,
    SPELL_ARM_ENTER_VISUAL              = 64753,
    SPELL_FOCUSED_EYEBEAM_PERIODIC_10   = 63347,
    SPELL_FOCUSED_EYEBEAM_PERIODIC_25   = 63977,
    SPELL_SUMMON_FOCUSED_EYEBEAM        = 63342,
    SPELL_FOCUSED_EYEBEAM_VISUAL        = 63369,
    SPELL_FOCUSED_EYEBEAM_VISUAL_LEFT   = 63676,
    SPELL_FOCUSED_EYEBEAM_VISUAL_RIGHT  = 63702,
	SPELL_SQUEEZED_LIFELESS             = 64708,
	SPELL_RUMBLE                        = 63818,
	SPELL_STONE_NOVA                    = 63978,
    SPELL_KOLOGARN_REDUCE_PARRY         = 64651,
    SPELL_KOLOGARN_PACIFY               = 63726,
    SPELL_KOLOGARN_UNK_0                = 65219,
    SPELL_BERSERK                       = 47008,
};

#define SPELL_ARM_DEAD_DAMAGE           RAID_MODE(SPELL_ARM_DEAD_DAMAGE_10, SPELL_ARM_DEAD_DAMAGE_25)
#define SPELL_ARM_SWEEP                 RAID_MODE(SPELL_ARM_SWEEP_10, SPELL_ARM_SWEEP_25)
#define SPELL_FOCUSED_EYEBEAM_PERIODIC  RAID_MODE(SPELL_FOCUSED_EYEBEAM_PERIODIC_10, SPELL_FOCUSED_EYEBEAM_PERIODIC_25)

enum Events
{
	EVENT_INSTALL_ACCESSORIES = 1,
    EVENT_SMASH               = 2,
    EVENT_SWEEP               = 3,
    EVENT_STONE_SHOUT         = 4,
    EVENT_STONE_GRIP          = 5,
    EVENT_FOCUSED_EYEBEAM     = 6,
    EVENT_RESPAWN_LEFT_ARM    = 7,
    EVENT_RESPAWN_RIGHT_ARM   = 8,
    EVENT_ENRAGE              = 9,
};

enum Datas
{
    ACHIEV_DISARMED_START_EVENT = 21687,
    DATA_RUBBLE_AND_ROLL        = 1,
    DATA_DISARMED               = 2,
    DATA_WITH_OPEN_ARMS         = 3,
};

class boss_kologarn : public CreatureScript
{
    public:
        boss_kologarn() : CreatureScript("boss_kologarn") {}

        struct boss_kologarnAI : public BossAI
        {
            boss_kologarnAI(Creature* creature) : BossAI(creature, DATA_KOLOGARN), vehicle(creature->GetVehicleKit()), haveLeftArm(false), haveRightArm(false)
            {
                ASSERT(vehicle);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
                DoCast(SPELL_KOLOGARN_REDUCE_PARRY);
                SetCombatMovement(false);
                Reset();
            }  

			Vehicle* vehicle;
			bool haveLeftArm, haveRightArm;
			bool armDied;
			uint64 eyebeamTarget;
			uint8 rubbleCount;

            void Reset()
            {
                _Reset();
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetStandState(UNIT_STAND_STATE_SUBMERGED);
                me->AddUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY);
                armDied = false;
                rubbleCount = 0;
                eyebeamTarget = 0;

                Creature* leftHand = me->FindCreatureWithDistance(NPC_LEFT_ARM, 100.f);
                if (leftHand)
                    leftHand->AI()->Reset();

                Creature* rightHand = me->FindCreatureWithDistance(NPC_RIGHT_ARM, 100.f);
                if (rightHand)
                    rightHand->AI()->Reset();

                if (instance)
                    instance->DoStopTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_DISARMED_START_EVENT);

				// Respawn Rubble Stalker
				if (Creature* rubbleStalker = me->FindCreatureWithDistance(NPC_RUBBLE_STALKER, 250.0f, true))
					rubbleStalker->Respawn();
            }

			void MoveInLineOfSight(Unit* who)
			{
				if (me->IsWithinDistInMap(who, 55.0f) && who->GetTypeId() == TYPE_ID_PLAYER)
				{
					me->SetStandState(UNIT_STAND_STATE_STAND);
					me->RemoveStandFlags(UNIT_STAND_STATE_SUBMERGED);
				}
			}

            void EnterToBattle(Unit* /*who*/)
            {
                DoSendQuantumText(SAY_AGGRO, me);

				me->RemoveStandFlags(UNIT_STAND_STATE_STAND);

                events.ScheduleEvent(EVENT_SMASH, 5*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SWEEP, 19*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_STONE_GRIP, 25*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_FOCUSED_EYEBEAM, 21*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_ENRAGE, 10*MINUTE*IN_MILLISECONDS);

                for (uint8 i = 0; i < 2; ++i) // 2 -> 2 arms
				{
                    if (Unit* arm = vehicle->GetPassenger(i))
					{
                        if (!arm->IsInCombatActive()) // avoid cyclical activation: His arms are calling Kologarn if they get in combat and he is not.
                            arm->ToCreature()->SetInCombatWithZone();
					}
				}

                _EnterToBattle();
            }

            void JustDied(Unit* /*killer*/)
            {
				DoSendQuantumText(SAY_DEATH, me);
				DoCast(me, SPELL_KOLOGARN_PACIFY, true);
                me->GetMotionMaster()->MoveTargetedHome();
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				me->SetStandState(UNIT_STAND_STATE_DEAD);
                me->SetCorpseDelay(7*DAY);
				me->DespawnAfterAction(10*IN_MILLISECONDS);

                for (uint8 i = 0; i < 2; ++i)
                    if (Unit* arm = vehicle->GetPassenger(i))
                        arm->ExitVehicle();

                summons.DespawnAll();

				// Despawn Rubble Stalker
                if (Creature* rubble = me->FindCreatureWithDistance(NPC_RUBBLE_STALKER, 250.0f, true))
                    rubble->DespawnAfterAction();

                _JustDied();
            }

            void KilledUnit(Unit* victim)
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

            void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply)
            {
                bool isEncounterInProgress = (instance->GetBossState(DATA_KOLOGARN) == IN_PROGRESS);

                if (who->GetEntry() == NPC_LEFT_ARM)
                {
                    haveLeftArm = apply;
                    if (!apply && isEncounterInProgress)
                    {
                        who->ToCreature()->DisappearAndDie();
						DoSendQuantumText(SAY_LEFT_ARM_GONE, me);
                        events.ScheduleEvent(EVENT_RESPAWN_LEFT_ARM, 40*IN_MILLISECONDS);
                    }
                }
                else if (who->GetEntry() == NPC_RIGHT_ARM)
                {
                    haveRightArm = apply;
                    if (!apply && isEncounterInProgress)
                    {
                        who->ToCreature()->DisappearAndDie();
						DoSendQuantumText(SAY_RIGHT_ARM_GONE, me);
                        events.ScheduleEvent(EVENT_RESPAWN_RIGHT_ARM, 40*IN_MILLISECONDS);
                    }
                }

                if (!isEncounterInProgress)
                    return;

                if (!apply)
                {
                    armDied = true;
                    who->CastSpell(me, SPELL_ARM_DEAD_DAMAGE, true);
                    me->LowerPlayerDamageReq(RAID_MODE<uint32>(543855, 2300925));

                    if (Creature* rubbleStalker = who->FindCreatureWithDistance(NPC_RUBBLE_STALKER, 70.0f))
                    {
                        rubbleStalker->CastSpell(rubbleStalker, SPELL_FALLING_RUBBLE, true);
                        rubbleStalker->CastSpell(rubbleStalker, SPELL_SUMMON_RUBBLE, true);
                    }

                    if (!haveRightArm && !haveLeftArm)
                        events.ScheduleEvent(EVENT_STONE_SHOUT, 5*IN_MILLISECONDS);

                    if (instance)
                        instance->DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_DISARMED_START_EVENT);   // !apply = passenger got lost
                }
                else
                {
                    if (instance)
                        instance->DoStopTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_DISARMED_START_EVENT);

                    events.CancelEvent(EVENT_STONE_SHOUT);
                    who->ToCreature()->SetInCombatWithZone();
                }
            }

            // try to get ranged target, not too far away
            Player* GetEyeBeamTarget()
            {
                Map* map = me->GetMap();
                if (map && map->IsDungeon())
                {
                    std::list<Player*> playerList;
                    Map::PlayerList const& players = map->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    {
                        if (Player* player = itr->getSource())
                        {
							if (player->IsDead() || player->HasAura(RAID_MODE(SPELL_STONE_GRIP_DOT_10, SPELL_STONE_GRIP_DOT_25) || player->IsGameMaster()))
								continue;

                            if (me->GetVictim())
                                if (me->GetVictim()->GetGUID() == player->GetGUID())
                                    continue;

                            float Distance = player->GetDistance(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
                            if (Distance < 20.0f || Distance > 60.0f)
                                continue;

                            playerList.push_back(player);
                        }
                    }

                    if (playerList.empty())
                    {
                        Unit* sel = SelectTarget(TARGET_RANDOM, 0, 60.0f, true, -RAID_MODE(SPELL_STONE_GRIP_DOT_10, SPELL_STONE_GRIP_DOT_25));
                        if (sel)
                        {
                            if (Player* player = sel->ToPlayer())
                                playerList.push_back(player);
                            else
                                return 0;
                        }
                        else
                            return 0;    
                    }

                    return Quantum::DataPackets::SelectRandomContainerElement(playerList);
                }
                else
                    return 0;
            }

			void RespawnArm(uint32 entry)
            {
                if (Creature* arm = me->SummonCreature(entry, *me))
                {
                    arm->AddUnitTypeMask(UNIT_MASK_ACCESSORY);
                    int32 seatId = (entry == NPC_LEFT_ARM) ? 0 : 1;
                    arm->CastCustomSpell(SPELL_ARM_ENTER_VEHICLE, SPELLVALUE_BASE_POINT0, seatId+1, me, true);
                    arm->CastSpell(arm, SPELL_ARM_ENTER_VISUAL, true);
                }
            }

            void SummonedCreatureDespawn(Creature* summon)
            {
                if (summon->GetEntry() == NPC_RUBBLE)
                    --rubbleCount;

                summons.Despawn(summon);
            }

            void SummonedCreatureDies(Creature* summon, Unit* /* killer */)
            {
                if (summon->GetEntry() == NPC_RUBBLE)
                    --rubbleCount;

                summons.Despawn(summon);
            }

            void JustSummoned(Creature* summon)
            {
                switch (summon->GetEntry())
                {
                    case NPC_FOCUSED_EYEBEAM:
                        summons.Summon(summon);
                        summon->CastSpell(me, SPELL_FOCUSED_EYEBEAM_VISUAL_LEFT, true);
                        break;
                    case NPC_FOCUSED_EYEBEAM_RIGHT:
                        summons.Summon(summon);
                        summon->CastSpell(me, SPELL_FOCUSED_EYEBEAM_VISUAL_RIGHT, true);
                        break;
                    case NPC_RUBBLE:
                        summons.Summon(summon);
                        summon->SetInCombatWithZone();
                        ++rubbleCount;
                        return;
                    default:
                        return;
                }

                summon->CastSpell(summon, SPELL_FOCUSED_EYEBEAM_PERIODIC, true);
                summon->CastSpell(summon, SPELL_FOCUSED_EYEBEAM_VISUAL, true);
                summon->SetReactState(REACT_PASSIVE);
                summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
                summon->ClearUnitState(UNIT_STATE_CASTING);

                if (eyebeamTarget)
                {
                    if (Unit* target = ObjectAccessor::GetUnit(*summon, eyebeamTarget))
                    {
                        summon->Attack(target, false);
                        summon->GetMotionMaster()->MoveChase(target);
                    }
                }
            }

            uint32 GetData(uint32 type)
            {
                switch (type)
                {
                    case DATA_RUBBLE_AND_ROLL:
                        return (rubbleCount >= 25) ? 1 : 0;
                    case DATA_DISARMED:
                        return (!haveRightArm && !haveLeftArm) ? 1 : 0;
                    case DATA_WITH_OPEN_ARMS:
                        return armDied ? 0 : 1;
                    default:
                        break;
                }

                return 0;
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

				if (events.GetTimer() > 4000 && !me->IsWithinMeleeRange(me->GetVictim()))
					DoCastVictim(RAID_MODE(SPELL_PETRIFY_BREATH_10, SPELL_PETRIFY_BREATH_25));

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SWEEP:
                            if (haveLeftArm)
                            {
                                if (Creature* target = me->FindCreatureWithDistance(NPC_ARM_SWEEP_STALKER, 250.0f, true))
                                {
                                    DoCast(target, SPELL_ARM_SWEEP, true);
									DoSendQuantumText(SAY_SHOCKWAVE, me);
                                }  
                            }
                            events.ScheduleEvent(EVENT_SWEEP, 25*IN_MILLISECONDS);
                            return;
                        case EVENT_STONE_GRIP:
                            if (haveRightArm)
                            {
                                DoCastAOE(RAID_MODE(SPELL_STONE_GRIP_10, SPELL_STONE_GRIP_25));
								DoSendQuantumText(EMOTE_STONE_GRIP, me);
								DoSendQuantumText(SAY_GRAB_PLAYER, me);
                            }
                            events.ScheduleEvent(EVENT_STONE_GRIP, 25*IN_MILLISECONDS);
                            return;
                        case EVENT_SMASH:
                            if (haveLeftArm && haveRightArm)
                                DoCastVictim(RAID_MODE(SPELL_TWO_ARM_SMASH_10, SPELL_TWO_ARM_SMASH_25));
                            else if (haveLeftArm || haveRightArm)
                                DoCastVictim(RAID_MODE(SPELL_ONE_ARM_SMASH_10, SPELL_ONE_ARM_SMASH_25));
                            events.ScheduleEvent(EVENT_SMASH, 15 * IN_MILLISECONDS);
                            return;
                        case EVENT_STONE_SHOUT:
                            DoCast(RAID_MODE(SPELL_STONE_SHOUT_10, SPELL_STONE_SHOUT_25));
                            events.ScheduleEvent(EVENT_STONE_SHOUT, 2*IN_MILLISECONDS);
                            return;
                        case EVENT_ENRAGE:
                            DoCast(SPELL_BERSERK);
							DoSendQuantumText(SAY_BERSERK, me);
                            return;
                        case EVENT_RESPAWN_LEFT_ARM:
                            RespawnArm(NPC_LEFT_ARM);
							DoSendQuantumText(EMOTE_RESPAWN_LEFT_HAND, me);
                            events.CancelEvent(EVENT_RESPAWN_LEFT_ARM);
                            return;
                        case EVENT_RESPAWN_RIGHT_ARM:
                            RespawnArm(NPC_RIGHT_ARM);
							DoSendQuantumText(EMOTE_RESPAWN_RIGHT_HAND, me);
                            events.CancelEvent(EVENT_RESPAWN_RIGHT_ARM);
                            return;
                        case EVENT_FOCUSED_EYEBEAM:
                            if (Player* eyebeamTargetUnit = GetEyeBeamTarget())
                            {
                                eyebeamTarget = eyebeamTargetUnit->GetGUID();
								me->MonsterWhisper(EMOTE_EYEBEAM, eyebeamTarget, true);

                                eyebeamTargetUnit->CastSpell(eyebeamTargetUnit, 63343, true, NULL, NULL, me->GetGUID());
                                eyebeamTargetUnit->CastSpell(eyebeamTargetUnit, 63701, true, NULL, NULL, me->GetGUID());
                            }
                            events.ScheduleEvent(EVENT_FOCUSED_EYEBEAM, urand(15*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        default:
                            return;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_kologarnAI(creature);
        }
};

class npc_kologarn_arm : public CreatureScript
{
    public:
        npc_kologarn_arm() : CreatureScript("npc_kologarn_arm") {}

        struct npc_kologarn_armAI : public QuantumBasicAI
        {
            npc_kologarn_armAI(Creature* creature) : QuantumBasicAI(creature)
			{
				instance = creature->GetInstanceScript();

				me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
				me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
				me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_SQUEEZED_LIFELESS, true);
			}

			int32 ArmDamage;

			InstanceScript* instance;

            void Reset()
            {
                me->SetReactState(REACT_DEFENSIVE);

				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                me->SetReactState(REACT_AGGRESSIVE);

				if (Creature* kologarn = Unit::GetCreature((*me), instance->GetData64(DATA_KOLOGARN)))
				{
					if (!kologarn->IsInCombatActive())
						kologarn->AI()->DoZoneInCombat();
				}
            }

			void KilledUnit(Unit* killed)
			{
				if (Player* player = killed->ToPlayer())
				{
					killed->ExitVehicle();
					killed->GetMotionMaster()->MoveJump(1767.80f, -18.38f, 448.808f, 10.0f, 10.0f);
				}
			}

            void DamageTaken(Unit* /*DoneBy*/, uint32 &damage)
            {
				int32 dmg = RAID_MODE(100000, 480000);

				ArmDamage += damage;

				if (ArmDamage >= dmg || damage >= me->GetHealth())
                {
                    if (me->GetEntry() == NPC_RIGHT_ARM)
                    {
						if (Vehicle* vehicle = me->GetVehicleKit())
                        {
							if (Unit* passenger = vehicle->GetPassenger(0))
                            {
								if (Player* player = passenger->ToPlayer())
								{
									if (player->IsAlive())
									{
										player->RemoveAurasDueToSpell(RAID_MODE(SPELL_STONE_GRIP_DOT_10, SPELL_STONE_GRIP_DOT_25));
										player->ExitVehicle();
										player->GetMotionMaster()->MoveJump(1767.80f, -18.38f, 448.808f, 10.0f, 10.0f);
									}
								}
                            }
                        }
                    }
                }
            }

			void JustDied(Unit* /*victim*/)
            {
                me->DespawnAfterAction();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_kologarn_armAI(creature);
        }
};

class npc_rubble : public CreatureScript
{
public:
	npc_rubble() : CreatureScript("npc_rubble") { }

	struct npc_rubbleAI : public QuantumBasicAI
	{
		npc_rubbleAI(Creature* creature) : QuantumBasicAI(creature) 
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;

		uint32 StoneNovaTimer;
		uint32 DeathTimer;

		bool Die;

		void Reset()
		{
			Die = false;
			StoneNovaTimer = urand(8000, 12000);
			DoZoneInCombat();
		}

		void DamageTaken(Unit* doneBy, uint32& damage)
		{
			if (damage > me->GetHealth())
			{
				if (!Die)
				{
					damage = 0;

					DoCast(me, SPELL_RUMBLE);

					if (Creature* kologarn = Unit::GetCreature((*me), instance->GetData64(DATA_KOLOGARN)))
						kologarn->AI()->DoAction(DATA_RUBBLE_AND_ROLL);

					DeathTimer = 500;

					Die = true;
				}
			}
		}

		void UpdateAI(const uint32 diff)
		{
			if (DeathTimer <= diff && Die)
			{
				me->DealDamage(me, me->GetHealth(), 0, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, 0, false);
			}
			else DeathTimer -= diff;

			if (StoneNovaTimer <= diff && !Die && GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL)
			{
				DoCast(me, SPELL_STONE_NOVA);
				StoneNovaTimer = urand(7000, 9000);
			}
			else StoneNovaTimer -= diff;

			if (!Die)
				DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const    
	{
		return new npc_rubbleAI(creature);
	}
};

class spell_ulduar_rubble_summon : public SpellScriptLoader
{
    public:
        spell_ulduar_rubble_summon() : SpellScriptLoader("spell_ulduar_rubble_summon") {}

        class spell_ulduar_rubble_summonSpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ulduar_rubble_summonSpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                uint64 originalCaster = caster->GetInstanceScript() ? caster->GetInstanceScript()->GetData64(DATA_KOLOGARN) : 0;
                uint32 spellId = GetEffectValue();

                for (uint8 i = 0; i < 5; ++i)
                    caster->CastSpell(caster, spellId, true, 0, 0, originalCaster);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_ulduar_rubble_summonSpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_ulduar_rubble_summonSpellScript();
        }
};

class StoneGripTargetSelector : public std::unary_function<Unit *, bool>
{
    public:
        StoneGripTargetSelector(Creature* me, const Unit* victim) : _me(me), _victim(victim) {}

        bool operator() (Unit* target)
        {
            if (target == _victim && _me->getThreatManager().getThreatList().size() > 1)
                return true;

            if (target->GetTypeId() != TYPE_ID_PLAYER)
                return true;

            return false;
        }

        Creature* _me;
        Unit const* _victim;
};

class spell_ulduar_stone_grip_cast_target : public SpellScriptLoader
{
    public:
        spell_ulduar_stone_grip_cast_target() : SpellScriptLoader("spell_ulduar_stone_grip_cast_target") { }

        class spell_ulduar_stone_grip_cast_target_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ulduar_stone_grip_cast_target_SpellScript);

            bool Load()
            {
                if (GetCaster()->GetTypeId() != TYPE_ID_UNIT)
                    return false;
                return true;
            }

            void FilterTargetsInitial(std::list<Unit*>& unitList)
            {
                unitList.remove_if (StoneGripTargetSelector(GetCaster()->ToCreature(), GetCaster()->GetVictim()));

                uint32 maxTargets = 1;

                if (GetSpellInfo()->Id == SPELL_STONE_GRIP_25)
                    maxTargets = 3;

                while (maxTargets < unitList.size())
                {
                    std::list<Unit*>::iterator itr = unitList.begin();
                    advance(itr, urand(0, unitList.size()-1));
                    unitList.erase(itr);
                }

                m_unitList = unitList;
            }

            void FillTargetsSubsequential(std::list<Unit*>& unitList)
            {
                unitList = m_unitList;
            }

            void HandleForceCast(SpellEffIndex i)
            {
                Player* player = GetHitPlayer();

                if (!player)
                    return;

                player->CastSpell(GetExplTargetUnit(), GetSpellInfo()->Effects[i].TriggerSpell, true);
                PreventHitEffect(i);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_ulduar_stone_grip_cast_target_SpellScript::FilterTargetsInitial, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_ulduar_stone_grip_cast_target_SpellScript::FillTargetsSubsequential, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_ulduar_stone_grip_cast_target_SpellScript::FillTargetsSubsequential, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
				OnEffectHitTarget += SpellEffectFn(spell_ulduar_stone_grip_cast_target_SpellScript::HandleForceCast, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
            }

            // Shared between effects
            std::list<Unit*> m_unitList;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_ulduar_stone_grip_cast_target_SpellScript();
        }
};

class spell_ulduar_cancel_stone_grip : public SpellScriptLoader
{
    public:
        spell_ulduar_cancel_stone_grip() : SpellScriptLoader("spell_ulduar_cancel_stone_grip") {}

        class spell_ulduar_cancel_stone_gripSpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ulduar_cancel_stone_gripSpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* target = GetHitUnit();
                if (!target || !target->GetVehicle() || target->GetTypeId() != TYPE_ID_PLAYER)
                    return;

                switch (target->GetMap()->GetDifficulty())
                {
                    case RAID_DIFFICULTY_10MAN_NORMAL:
                        target->RemoveAura(GetSpellInfo()->Effects[EFFECT_0].CalcValue());
                        break;
                    case RAID_DIFFICULTY_25MAN_NORMAL:
                        target->RemoveAura(GetSpellInfo()->Effects[EFFECT_1].CalcValue());
                        break;
                    default:
                        break;
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_ulduar_cancel_stone_gripSpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_ulduar_cancel_stone_gripSpellScript();
        }
};

class spell_ulduar_squeezed_lifeless : public SpellScriptLoader
{
    public:
        spell_ulduar_squeezed_lifeless() : SpellScriptLoader("spell_ulduar_squeezed_lifeless") {}

        class spell_ulduar_squeezed_lifeless_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ulduar_squeezed_lifeless_SpellScript);

            void HandleInstaKill(SpellEffIndex /*effIndex*/)
            {
                if (!GetHitPlayer() || !GetHitPlayer()->GetVehicle())
                    return;

                Position pos;
                pos.m_positionX = 1756.25f + irand(-3, 3);
                pos.m_positionY = -8.3f + irand(-3, 3);
                pos.m_positionZ = 448.8f;
                pos.m_orientation = M_PI;
                GetHitPlayer()->DestroyForNearbyPlayers();
                GetHitPlayer()->ExitVehicle(&pos);
                GetHitPlayer()->UpdateObjectVisibility(false);
            }

            void MoveCorpse()
            {
                GetHitUnit()->ExitVehicle();
				GetHitUnit()->GetMotionMaster()->MoveJump(1767.80f, -18.38f, 448.808f, 10, 10);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_ulduar_squeezed_lifeless_SpellScript::HandleInstaKill, EFFECT_1, SPELL_EFFECT_INSTAKILL);
                AfterHit += SpellHitFn(spell_ulduar_squeezed_lifeless_SpellScript::MoveCorpse);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_ulduar_squeezed_lifeless_SpellScript();
        }
};

class spell_ulduar_stone_grip_absorb : public SpellScriptLoader
{
    public:
        spell_ulduar_stone_grip_absorb() : SpellScriptLoader("spell_ulduar_stone_grip_absorb") {}

        class spell_ulduar_stone_grip_absorb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ulduar_stone_grip_absorb_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_ENEMY_SPELL)
                    return;

                if (!GetOwner()->ToCreature())
                    return;

                uint32 rubbleStalkerEntry = (GetOwner()->GetMap()->GetDifficulty() == DUNGEON_DIFFICULTY_NORMAL ? NPC_RUBBLE_STALKER : NPC_RUBBLE_STALKER_25);
                Creature* rubbleStalker = GetOwner()->FindCreatureWithDistance(rubbleStalkerEntry, 200.0f, true);

                if (rubbleStalker)
                    rubbleStalker->CastSpell(rubbleStalker, SPELL_STONE_GRIP_CANCEL, true);
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_ulduar_stone_grip_absorb_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_ulduar_stone_grip_absorb_AuraScript();
        }
};

class spell_ulduar_stone_grip : public SpellScriptLoader
{
    public:
        spell_ulduar_stone_grip() : SpellScriptLoader("spell_ulduar_stone_grip") {}

        class spell_ulduar_stone_grip_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ulduar_stone_grip_AuraScript);

            void OnRemoveStun(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Player* owner = GetOwner()->ToPlayer())
                {
                    owner->RemoveAurasDueToSpell(aurEff->GetAmount());
                    owner->RemoveAurasDueToSpell(SPELL_SQUEEZED_LIFELESS);
                }
            }

            void OnEnterVehicle(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Player* caster = GetCaster() ? GetCaster()->ToPlayer() : 0;
                if (caster)
                    caster->ClearUnitState(UNIT_STATE_ON_VEHICLE);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_ulduar_stone_grip_AuraScript::OnEnterVehicle, EFFECT_0, SPELL_AURA_CONTROL_VEHICLE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_ulduar_stone_grip_AuraScript::OnRemoveStun, EFFECT_2, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_ulduar_stone_grip_AuraScript();
        }
};

class spell_kologarn_stone_shout : public SpellScriptLoader
{
    public:
        spell_kologarn_stone_shout() : SpellScriptLoader("spell_kologarn_stone_shout") {}

        class spell_kologarn_stone_shout_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_kologarn_stone_shout_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if (PlayerOrPetCheck());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_kologarn_stone_shout_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_kologarn_stone_shout_SpellScript();
        }
};

class spell_kologarn_summon_focused_eyebeam : public SpellScriptLoader
{
    public:
        spell_kologarn_summon_focused_eyebeam() : SpellScriptLoader("spell_kologarn_summon_focused_eyebeam") {}

        class spell_kologarn_summon_focused_eyebeam_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_kologarn_summon_focused_eyebeam_SpellScript);

            void HandleForceCast(SpellEffIndex eff)
            {
                PreventHitDefaultEffect(eff);
                GetCaster()->CastSpell(GetCaster(), GetSpellInfo()->Effects[eff].TriggerSpell, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_kologarn_summon_focused_eyebeam_SpellScript::HandleForceCast, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
                OnEffectHitTarget += SpellEffectFn(spell_kologarn_summon_focused_eyebeam_SpellScript::HandleForceCast, EFFECT_1, SPELL_EFFECT_FORCE_CAST);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_kologarn_summon_focused_eyebeam_SpellScript();
        }
};

class achievement_rubble_and_roll : public AchievementCriteriaScript
{
public:
	achievement_rubble_and_roll(const char* name) : AchievementCriteriaScript(name) {}

	bool OnCheck(Player* /*source*/, Unit* target)
	{
		if (target && target->IsAIEnabled)
			return target->GetAI()->GetData(DATA_RUBBLE_AND_ROLL);

		return false;
	}
};

class achievement_disarmed : public AchievementCriteriaScript
{
public:
	achievement_disarmed(const char* name) : AchievementCriteriaScript(name) {}

	bool OnCheck(Player* /*source*/, Unit* target)
	{
		if (target && target->IsAIEnabled)
			return target->GetAI()->GetData(DATA_DISARMED);

		return false;
	}
};

class achievement_with_open_arms : public AchievementCriteriaScript
{
public:
	achievement_with_open_arms(const char* name) : AchievementCriteriaScript(name) {}

	bool OnCheck(Player* /*source*/, Unit* target)
	{
		if (target && target->IsAIEnabled)
			return target->GetAI()->GetData(DATA_WITH_OPEN_ARMS);

		return false;
	}
};

void AddSC_boss_kologarn()
{
    new boss_kologarn();
    new npc_kologarn_arm();
	new npc_rubble();
    new spell_ulduar_rubble_summon();
    new spell_ulduar_squeezed_lifeless();
    new spell_ulduar_cancel_stone_grip();
    new spell_ulduar_stone_grip_cast_target();
    new spell_ulduar_stone_grip_absorb();
    new spell_ulduar_stone_grip();
    new spell_kologarn_stone_shout();
    new spell_kologarn_summon_focused_eyebeam();
    new achievement_rubble_and_roll("achievement_rubble_and_roll");
    new achievement_rubble_and_roll("achievement_rubble_and_roll_25");
    new achievement_disarmed("achievement_disarmed");
    new achievement_disarmed("achievement_disarmed_25");
    new achievement_with_open_arms("achievement_with_open_arms");
    new achievement_with_open_arms("achievement_with_open_arms_25");
}