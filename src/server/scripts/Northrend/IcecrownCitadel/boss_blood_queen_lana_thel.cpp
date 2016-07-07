/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"
#include "icecrown_citadel.h"

enum Texts
{
    SAY_AGGRO                   = -1659143,
    SAY_VAMPIRIC_BITE_1         = -1659144,
	SAY_VAMPIRIC_BITE_2         = -1659145,
    SAY_MIND_CONTROL            = -1659146,
    EMOTE_BLOODTHIRST           = -1659147,
    SAY_SWARMING_SHADOWS        = -1659148,
    EMOTE_SWARMING_SHADOWS      = -1659149,
    SAY_PACT_OF_THE_DARKFALLEN  = -1659150,
    SAY_AIR_PHASE               = -1659151,
    SAY_KILL_1                  = -1659152,
	SAY_KILL_2                  = -1659153,
    SAY_WIPE                    = -1659154,
    SAY_BERSERK                 = -1659155,
	SAY_RANDOM                  = -1659156,
    SAY_DEATH                   = -1659157,
};

enum Spells
{
    SPELL_SHROUD_OF_SORROW                  = 70986,
    SPELL_FRENZIED_BLOODTHIRST_VISUAL       = 71949,
	// Vampiric Bite Spell Defines by Crispi
    SPELL_VAMPIRIC_BITE_10N                 = 71726,
	SPELL_VAMPIRIC_BITE_10H                 = 71727,
	SPELL_VAMPIRIC_BITE_25N                 = 71728,
	SPELL_VAMPIRIC_BITE_25H                 = 71729,
	SPELL_VAMPIRIC_BITE_PLAYER_10N          = 70946,
	SPELL_VAMPIRIC_BITE_PLAYER_10H          = 71475,
	SPELL_VAMPIRIC_BITE_PLAYER_25N          = 71476,
	SPELL_VAMPIRIC_BITE_PLAYER_25H          = 71477,
    SPELL_FRENZIED_BLOODTHIRST              = 70877,
	SPELL_FRENZIED_BLOODTHIRST_2            = 71714,
    SPELL_UNCONTROLLABLE_FRENZY             = 70923,
	SPELL_UNCONTROLLABLE_FRENZY_1           = 70924,
    SPELL_UNCONTROLLABLE_FRENZY_2           = 73015,
    SPELL_PRESENCE_OF_THE_DARKFALLEN        = 71952,
	SPELL_PRESENCE_OF_THE_DARKFALLEN_2      = 70995,
    SPELL_BLOOD_MIRROR_DAMAGE               = 70821,
	SPELL_BLOOD_MIRROR_DUMMY                = 70838,
    SPELL_BLOOD_MIRROR_VISUAL               = 71510,
    SPELL_DELIRIOUS_SLASH_10N               = 71623,
	SPELL_DELIRIOUS_SLASH_25N               = 71624,
	SPELL_DELIRIOUS_SLASH_10H               = 71625,
	SPELL_DELIRIOUS_SLASH_25H               = 71626,
    SPELL_PACT_OF_THE_DARKFALLEN_TARGET     = 71336,
    SPELL_PACT_OF_THE_DARKFALLEN            = 71340,
    SPELL_PACT_OF_THE_DARKFALLEN_DAMAGE     = 71341,
    SPELL_SWARMING_SHADOWS                  = 71264,
    SPELL_TWILIGHT_BLOODBOLT_TARGET         = 71445,
    SPELL_TWILIGHT_BLOODBOLT                = 71446,
    SPELL_INCITE_TERROR                     = 73070,
    SPELL_BLOODBOLT_WHIRL                   = 71772,
    SPELL_ANNIHILATE                        = 71322,
	SPELL_BLOOD_INFUSION_CREDIT             = 72934,
	// Shadowmourne
	SPELL_GUSHING_WOUND                     = 72132,
    SPELL_THIRST_QUENCHED                   = 72154,
	// Essence of the Blood Queen
	SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_BASIC  = 70879,
	SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_1      = 70867,
	SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_2      = 71473,
	SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_3      = 71525,
	SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_4      = 71530,
	SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_5      = 71531,
	SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_6      = 71532,
	SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_7      = 71533,
	SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_8      = 70871, // Secondary
	SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_9      = 70949, // Secondary
};

uint32 const VampireAuras[4][MAX_DIFFICULTY] =
{
    {70867, 70879, 71473, 71525}, // Essence of the Blood Queen
    {71530, 71531, 71532, 71533}, // Essence of the Blood Queen
	{70871, 70949, 70871, 70949}, // Essence of the Blood Queen ??
    {70877, 71474, 70877, 71474}, // Frenzied Bloodthirst
};

enum Events
{
    EVENT_BERSERK                   = 1,
    EVENT_VAMPIRIC_BITE             = 2,
    EVENT_BLOOD_MIRROR              = 3,
    EVENT_DELIRIOUS_SLASH           = 4,
    EVENT_PACT_OF_THE_DARKFALLEN    = 5,
    EVENT_SWARMING_SHADOWS          = 6,
    EVENT_TWILIGHT_BLOODBOLT        = 7,
    EVENT_AIR_PHASE                 = 8,
    EVENT_AIR_START_FLYING          = 9,
    EVENT_AIR_FLY_DOWN              = 10,
	EVENT_RANDOM_SAY                = 11,

    EVENT_GROUP_NORMAL              = 1,
    EVENT_GROUP_CANCELLABLE         = 2,
};

enum Guids
{
    GUID_VAMPIRE    = 1,
    GUID_BLOODBOLT  = 2,
};

enum Points
{
    POINT_CENTER    = 1,
    POINT_AIR       = 2,
    POINT_GROUND    = 3,
    POINT_MINCHAR   = 4,
};

Position const centerPos =
{
	4595.7090f, 2769.4190f, 400.6368f, 0.000000f,
};

Position const airPos =
{
	4595.7090f, 2769.4190f, 422.3893f, 0.000000f,
};

Position const mincharPos =
{
	4629.3711f, 2782.6089f, 424.6390f, 0.000000f,
};

bool IsVampire(Unit const* unit)
{
    for (uint8 i = 0; i < 4; ++i)
        if (unit->HasAura(VampireAuras[i][unit->GetMap()->GetSpawnMode()]))
            return true;
    return false;
}

class boss_blood_queen_lana_thel : public CreatureScript
{
    public:
        boss_blood_queen_lana_thel() : CreatureScript("boss_blood_queen_lana_thel") { }

        struct boss_blood_queen_lana_thelAI : public BossAI
        {
            boss_blood_queen_lana_thelAI(Creature* creature) : BossAI(creature, DATA_BLOOD_QUEEN_LANA_THEL){}

			std::set<uint64> Vampires;
            std::set<uint64> _bloodboltedPlayers;
            Player* Offtank;
            bool CreditBloodQuickening;
            bool KillMinchar;

            void Reset()
            {
                _Reset();

				events.ScheduleEvent(EVENT_RANDOM_SAY, 30000);
                events.ScheduleEvent(EVENT_BERSERK, 330000);
                events.ScheduleEvent(EVENT_VAMPIRIC_BITE, 15000);
                events.ScheduleEvent(EVENT_BLOOD_MIRROR, 2500, EVENT_GROUP_CANCELLABLE);
                events.ScheduleEvent(EVENT_DELIRIOUS_SLASH, urand(20000, 24000), EVENT_GROUP_NORMAL);
                events.ScheduleEvent(EVENT_PACT_OF_THE_DARKFALLEN, 15000, EVENT_GROUP_NORMAL);
                events.ScheduleEvent(EVENT_SWARMING_SHADOWS, 30500, EVENT_GROUP_NORMAL);
                events.ScheduleEvent(EVENT_TWILIGHT_BLOODBOLT, urand(20000, 25000), EVENT_GROUP_NORMAL);
                events.ScheduleEvent(EVENT_AIR_PHASE, 124000 + uint32(Is25ManRaid() ? 3000 : 0));

                me->SetSpeed(MOVE_FLIGHT, 0.642857f, true);

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

                Offtank = 0;
                Vampires.clear();

                CreditBloodQuickening = false;
                KillMinchar = false;
            }

			void EnterToBattle(Unit* who)
            {
                if (!instance->CheckRequiredBosses(DATA_BLOOD_QUEEN_LANA_THEL, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    instance->DoCastSpellOnPlayers(SPELL_LIGHTS_HAMMER_TELEPORT);
                    return;
                }

				if (!me->IsInEvadeMode() && (who->IsPet() || who->IsHunterPet()))
				{
					EnterEvadeMode();
					return;
				}

                me->SetActive(true);
                DoZoneInCombat();
                DoSendQuantumText(SAY_AGGRO, me);
                instance->SetBossState(DATA_BLOOD_QUEEN_LANA_THEL, IN_PROGRESS);

                DoCast(me, SPELL_SHROUD_OF_SORROW, true);
                DoCast(me, SPELL_FRENZIED_BLOODTHIRST_VISUAL, true);
                CreditBloodQuickening = instance->GetData(DATA_BLOOD_QUICKENING_STATE) == IN_PROGRESS;
            }

			void KilledUnit(Unit* who)
            {
				if (who->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_KILL_1, SAY_KILL_2), me);
            }

			void RemovePlayerAuras()
			{
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_BASIC);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_1);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_2);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_3);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_4);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_5);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_6);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_7);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_8);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_9);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FRENZIED_BLOODTHIRST);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FRENZIED_BLOODTHIRST_2);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNCONTROLLABLE_FRENZY);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNCONTROLLABLE_FRENZY_1);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNCONTROLLABLE_FRENZY_2);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOOD_MIRROR_DAMAGE);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOOD_MIRROR_DUMMY);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLOOD_MIRROR_VISUAL);
			}

			void EnterEvadeMode()
            {
                _EnterEvadeMode();

				RemovePlayerAuras();

                if (KillMinchar)
                {
                    KillMinchar = false;
                    me->SetDisableGravity(true);
                    me->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND);
                    me->SetCanFly(true);
                    me->GetMotionMaster()->MovePoint(POINT_MINCHAR, mincharPos);
                }
                else
                {
                    me->GetMotionMaster()->MoveTargetedHome();
                    Reset();
                }
            }

            void JustReachedHome()
            {
                me->SetDisableGravity(false);
                me->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND);
                me->SetCanFly(false);
                me->SetReactState(REACT_AGGRESSIVE);
                _JustReachedHome();
                DoSendQuantumText(SAY_WIPE, me);
                instance->SetBossState(DATA_BLOOD_QUEEN_LANA_THEL, FAIL);
            }

            void JustDied(Unit* killer)
            {
                _JustDied();
                DoSendQuantumText(SAY_DEATH, me);

				RemovePlayerAuras();

				if (Is25ManRaid() && me->HasAura(SPELL_SHADOWS_FATE))
					DoCastAOE(SPELL_BLOOD_INFUSION_CREDIT, true);

                // Blah, credit the quest
                if (CreditBloodQuickening)
                {
                    instance->SetData(DATA_BLOOD_QUICKENING_STATE, DONE);

                    if (Player* player = killer->ToPlayer())
                       player->RewardPlayerAndGroupAtEvent(Is25ManRaid() ? NPC_INFILTRATOR_MINCHAR_BQ_25 : NPC_INFILTRATOR_MINCHAR_BQ_10, player);

                    if (Creature* minchar = me->FindCreatureWithDistance(NPC_INFILTRATOR_MINCHAR_BQ_10, 200.0f))
                    {
                        minchar->SetUInt32Value(UNIT_NPC_EMOTE_STATE, 0);
                        minchar->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND);
                        minchar->SetCanFly(false);
                        minchar->SendMovementFlagUpdate();
                        minchar->RemoveAllAuras();
                        minchar->GetMotionMaster()->MoveCharge(4629.3711f, 2782.6089f, 401.5301f, SPEED_CHARGE/3.0f);
                    }
                }

				// Quest credit
				if (Player* player = killer->ToPlayer())
				{
					player->CastSpell(player, SPELL_SOUL_FEAST, true);
					me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
				}
            }

            void DoAction(int32 const action)
            {
                if (action != ACTION_KILL_MINCHAR)
                    return;

                if (instance->GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) == IN_PROGRESS)
                    KillMinchar = true;
                else
                {
                    me->SetDisableGravity(true);
                    me->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND);
                    me->SetCanFly(true);
                    me->SendMovementFlagUpdate();
                    me->GetMotionMaster()->MovePoint(POINT_MINCHAR, mincharPos);
                }
            }

            void SetGUID(uint64 guid, int32 type = 0)
            {
                switch (type)
                {
                    case GUID_VAMPIRE:
                        Vampires.insert(guid);
                        break;
                    case GUID_BLOODBOLT:
                        _bloodboltedPlayers.insert(guid);
                        break;
                    default:
                        break;
                }
            }

            void MovementInform(uint32 type, uint32 id)
            {
				if (type != POINT_MOTION_TYPE && type != EFFECT_MOTION_TYPE)
					return;

                switch (id)
                {
                    case POINT_CENTER:
                        DoCast(me, SPELL_INCITE_TERROR);
                        events.ScheduleEvent(EVENT_AIR_PHASE, 100000 + uint32(Is25ManRaid() ? 0 : 20000));
                        events.RescheduleEvent(EVENT_SWARMING_SHADOWS, 30500, EVENT_GROUP_NORMAL);
                        events.RescheduleEvent(EVENT_PACT_OF_THE_DARKFALLEN, 25500, EVENT_GROUP_NORMAL);
                        events.ScheduleEvent(EVENT_AIR_START_FLYING, 5000);
                        break;
                    case POINT_AIR:
                        _bloodboltedPlayers.clear();
                        DoCast(me, SPELL_BLOODBOLT_WHIRL);
                        DoSendQuantumText(SAY_AIR_PHASE, me);
                        events.ScheduleEvent(EVENT_AIR_FLY_DOWN, 10000);
                        break;
                    case POINT_GROUND:
						me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                        me->SetDisableGravity(false);
                        me->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND);
                        me->SetCanFly(false);
                        me->SendMovementFlagUpdate();
                        me->SetReactState(REACT_AGGRESSIVE);
                        if (Unit* victim = me->SelectVictim())
                            AttackStart(victim);
                        events.ScheduleEvent(EVENT_BLOOD_MIRROR, 2500, EVENT_GROUP_CANCELLABLE);
                        break;
                    case POINT_MINCHAR:
                        DoCast(me, SPELL_ANNIHILATE, true);
                        // already in evade mode
                        me->GetMotionMaster()->MoveTargetedHome();
                        Reset();
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim() || !CheckInRoom())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BERSERK:
                            DoSendQuantumText(EMOTE_GENERIC_BERSERK_RAID, me);
                            DoSendQuantumText(SAY_BERSERK, me);
                            DoCast(me, SPELL_BERSERK);
                            break;
                        case EVENT_VAMPIRIC_BITE:
                        {
                            std::list<Player*> targets;
                            SelectRandomTarget(false, &targets);
                            if (!targets.empty())
                            {
                                Unit* target = targets.front();
                                DoCast(target, RAID_MODE(SPELL_VAMPIRIC_BITE_10N, SPELL_VAMPIRIC_BITE_25N, SPELL_VAMPIRIC_BITE_10H, SPELL_VAMPIRIC_BITE_25H));
                                DoSendQuantumText(RAND(SAY_VAMPIRIC_BITE_1, SAY_VAMPIRIC_BITE_2), me);
                                Vampires.insert(target->GetGUID());
                            }
                            break;
                        }
                        case EVENT_BLOOD_MIRROR:
                        {
                            // victim can be 0 when this is processed in the same update tick as EVENT_AIR_PHASE
                            if (me->GetVictim())
                            {
                                Player* newOfftank = SelectRandomTarget(true);
                                if (Offtank != newOfftank)
                                {
                                    Offtank = newOfftank;
                                    if (Offtank)
                                    {
                                        // both spells have SPELL_ATTR5_SINGLE_TARGET_SPELL, no manual removal needed
                                        Offtank->CastSpell(me->GetVictim(), SPELL_BLOOD_MIRROR_DAMAGE, true);
                                        me->GetVictim()->CastSpell(Offtank, SPELL_BLOOD_MIRROR_DUMMY, true);
                                        DoCastVictim(SPELL_BLOOD_MIRROR_VISUAL);
                                        if (Item* shadowsEdge = Offtank->GetWeaponForAttack(BASE_ATTACK, true))
										{
                                            if (!Offtank->HasAura(SPELL_THIRST_QUENCHED) && shadowsEdge->GetEntry() == ITEM_THE_SHADOW_S_EDGE && !Offtank->HasAura(SPELL_GUSHING_WOUND))
                                                Offtank->CastSpell(Offtank, SPELL_GUSHING_WOUND, true);
										}
                                    }
                                }
                            }
                            events.ScheduleEvent(EVENT_BLOOD_MIRROR, 2500, EVENT_GROUP_CANCELLABLE);
                            break;
                        }
                        case EVENT_DELIRIOUS_SLASH:
                            if (Offtank && !me->HasByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_HOVER))
                                DoCast(Offtank, RAID_MODE(SPELL_DELIRIOUS_SLASH_10N, SPELL_DELIRIOUS_SLASH_25N, SPELL_DELIRIOUS_SLASH_10H, SPELL_DELIRIOUS_SLASH_25H));
                            events.ScheduleEvent(EVENT_DELIRIOUS_SLASH, urand(20000, 24000), EVENT_GROUP_NORMAL);
                            break;
                        case EVENT_PACT_OF_THE_DARKFALLEN:
                        {
                            std::list<Player*> targets;
                            SelectRandomTarget(false, &targets);
                            uint32 targetCount = 2;
                            // do not combine these checks! we want it incremented TWICE when both conditions are met
                            if (IsHeroic())
                                ++targetCount;
                            if (Is25ManRaid())
                                ++targetCount;
                            Quantum::DataPackets::RandomResizeList<Player*>(targets, targetCount);
                            if (targets.size() > 1)
                            {
                                DoSendQuantumText(SAY_PACT_OF_THE_DARKFALLEN, me);
                                for (std::list<Player*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
                                    DoCast(*itr, SPELL_PACT_OF_THE_DARKFALLEN);
                            }
                            events.ScheduleEvent(EVENT_PACT_OF_THE_DARKFALLEN, 30500, EVENT_GROUP_NORMAL);
                            break;
                        }
                        case EVENT_SWARMING_SHADOWS:
                            if (Player* target = SelectRandomTarget(false))
                            {
                                DoSendQuantumText(EMOTE_SWARMING_SHADOWS, target);
                                DoSendQuantumText(SAY_SWARMING_SHADOWS, me);
                                DoCast(target, SPELL_SWARMING_SHADOWS);
                            }
                            events.ScheduleEvent(EVENT_SWARMING_SHADOWS, 30500, EVENT_GROUP_NORMAL);
                            break;
                        case EVENT_TWILIGHT_BLOODBOLT:
                        {
                            std::list<Player*> targets;
                            SelectRandomTarget(false, &targets);
                            Quantum::DataPackets::RandomResizeList<Player*>(targets, uint32(Is25ManRaid() ? 4 : 2));
                            for (std::list<Player*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
                                DoCast(*itr, SPELL_TWILIGHT_BLOODBOLT);
                            DoCast(me, SPELL_TWILIGHT_BLOODBOLT_TARGET);
                            events.ScheduleEvent(EVENT_TWILIGHT_BLOODBOLT, urand(10000, 15000), EVENT_GROUP_NORMAL);
                            break;
                        }
                        case EVENT_AIR_PHASE:
                            DoStopAttack();
                            me->SetReactState(REACT_PASSIVE);
                            events.DelayEvents(10000, EVENT_GROUP_NORMAL);
                            events.CancelEventGroup(EVENT_GROUP_CANCELLABLE);
                            me->GetMotionMaster()->MovePoint(POINT_CENTER, centerPos);
                            break;
                        case EVENT_AIR_START_FLYING:
							me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
                            me->SetDisableGravity(true);
                            me->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND);
                            me->SetCanFly(true);
                            me->SendMovementFlagUpdate();
							me->GetMotionMaster()->MoveTakeoff(POINT_AIR, airPos);
                            break;
                        case EVENT_AIR_FLY_DOWN:
							me->GetMotionMaster()->MoveLand(POINT_GROUND, centerPos);
                            break;
						case EVENT_RANDOM_SAY:
							DoSendQuantumText(SAY_RANDOM, me);
							break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            bool WasVampire(uint64 guid)
            {
                return Vampires.count(guid) != 0;
            }

            bool WasBloodbolted(uint64 guid)
            {
                return _bloodboltedPlayers.count(guid) != 0;
            }

        private:
            // offtank for this encounter is the player standing closest to main tank
            Player* SelectRandomTarget(bool includeOfftank, std::list<Player*>* targetList = 0)
            {
                std::list<HostileReference*> const& threatlist = me->getThreatManager().getThreatList();
                std::list<Player*> tempTargets;

                if (threatlist.empty())
                    return 0;

                for (std::list<HostileReference*>::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
				{
                    if (Unit* refTarget = (*itr)->getTarget())
					{
                        if (refTarget != me->GetVictim() && refTarget->GetTypeId() == TYPE_ID_PLAYER && (includeOfftank ? true : (refTarget != Offtank)))
                            tempTargets.push_back(refTarget->ToPlayer());
					}
				}

                if (tempTargets.empty())
                    return 0;

                if (targetList)
                {
                    *targetList = tempTargets;
                    return 0;
                }

                if (includeOfftank)
                {
                    tempTargets.sort(Trinity::ObjectDistanceOrderPred(me->GetVictim()));
                    return tempTargets.front();
                }

                return Quantum::DataPackets::SelectRandomContainerElement(tempTargets);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<boss_blood_queen_lana_thelAI>(creature);
        }
};

// helper for shortened code
typedef boss_blood_queen_lana_thel::boss_blood_queen_lana_thelAI LanaThelAI;

class spell_blood_queen_vampiric_bite : public SpellScriptLoader
{
    public:
        spell_blood_queen_vampiric_bite() : SpellScriptLoader("spell_blood_queen_vampiric_bite") { }

        class spell_blood_queen_vampiric_bite_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_blood_queen_vampiric_bite_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_BASIC))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_FRENZIED_BLOODTHIRST))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_PRESENCE_OF_THE_DARKFALLEN))
                    return false;
                return true;
            }

            SpellCastResult CheckTarget()
            {
                if (IsVampire(GetExplTargetUnit()))
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_CANT_TARGET_VAMPIRES);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void OnCast()
            {
                if (GetCaster()->GetTypeId() != TYPE_ID_PLAYER)
                    return;

                uint32 spellId = sSpellMgr->GetSpellIdForDifficulty(SPELL_FRENZIED_BLOODTHIRST, GetCaster());
				GetCaster()->RemoveAura(spellId, 0, 0, AURA_REMOVE_BY_ENEMY_SPELL);
                GetCaster()->CastSpell(GetCaster(), SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_BASIC, TRIGGERED_FULL_MASK);

                // Shadowmourne questline
                if (Aura* aura = GetCaster()->GetAura(SPELL_GUSHING_WOUND))
                {
                    if (aura->GetStackAmount() == 3)
                    {
                        GetCaster()->CastSpell(GetCaster(), SPELL_THIRST_QUENCHED, TRIGGERED_FULL_MASK);
                        GetCaster()->RemoveAura(aura);
                    }
                    else
                        GetCaster()->CastSpell(GetCaster(), SPELL_GUSHING_WOUND, TRIGGERED_FULL_MASK);
                }

                if (InstanceScript* instance = GetCaster()->GetInstanceScript())
				{
                    if (Creature* bloodQueen = ObjectAccessor::GetCreature(*GetCaster(), instance->GetData64(DATA_BLOOD_QUEEN_LANA_THEL)))
                        bloodQueen->AI()->SetGUID(GetHitUnit()->GetGUID(), GUID_VAMPIRE);
				}
            }

            void HandlePresence(SpellEffIndex /*effIndex*/)
            {
                GetHitUnit()->CastSpell(GetHitUnit(), SPELL_PRESENCE_OF_THE_DARKFALLEN, TRIGGERED_FULL_MASK);
                GetHitUnit()->CastSpell(GetHitUnit(), SPELL_PRESENCE_OF_THE_DARKFALLEN_2, TRIGGERED_FULL_MASK);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_blood_queen_vampiric_bite_SpellScript::CheckTarget);
                BeforeHit += SpellHitFn(spell_blood_queen_vampiric_bite_SpellScript::OnCast);
                OnEffectHitTarget += SpellEffectFn(spell_blood_queen_vampiric_bite_SpellScript::HandlePresence, EFFECT_1, SPELL_EFFECT_TRIGGER_SPELL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_blood_queen_vampiric_bite_SpellScript();
        }
};

class spell_blood_queen_frenzied_bloodthirst : public SpellScriptLoader
{
    public:
        spell_blood_queen_frenzied_bloodthirst() : SpellScriptLoader("spell_blood_queen_frenzied_bloodthirst") { }

        class spell_blood_queen_frenzied_bloodthirst_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_blood_queen_frenzied_bloodthirst_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (InstanceScript* instance = GetTarget()->GetInstanceScript())
				{
                    if (Creature* bloodQueen = ObjectAccessor::GetCreature(*GetTarget(), instance->GetData64(DATA_BLOOD_QUEEN_LANA_THEL)))
						DoSendQuantumText(EMOTE_BLOODTHIRST, bloodQueen);
				}
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
				Unit* target = GetTarget();

                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
				{
                    if (InstanceScript* instance = target->GetInstanceScript())
					{
                        if (Creature* bloodQueen = ObjectAccessor::GetCreature(*target, instance->GetData64(DATA_BLOOD_QUEEN_LANA_THEL)))
                        {
                            if (target->GetVehicleKit())
                                target->RemoveVehicleKit();

							DoSendQuantumText(SAY_MIND_CONTROL, bloodQueen);
                            bloodQueen->CastSpell(target, SPELL_UNCONTROLLABLE_FRENZY, true);
                        }
					}
				}
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_blood_queen_frenzied_bloodthirst_AuraScript::OnApply, EFFECT_0, SPELL_AURA_OVERRIDE_SPELLS, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_blood_queen_frenzied_bloodthirst_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_OVERRIDE_SPELLS, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_blood_queen_frenzied_bloodthirst_AuraScript();
        }
};

class BloodboltHitCheck
{
    public:
        explicit BloodboltHitCheck(LanaThelAI* ai) : _ai(ai) {}

        bool operator()(Unit* unit)
        {
            return _ai->WasBloodbolted(unit->GetGUID());
        }

    private:
        LanaThelAI* _ai;
};

class spell_blood_queen_bloodbolt : public SpellScriptLoader
{
    public:
        spell_blood_queen_bloodbolt() : SpellScriptLoader("spell_blood_queen_bloodbolt") { }

        class spell_blood_queen_bloodbolt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_blood_queen_bloodbolt_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TWILIGHT_BLOODBOLT))
                    return false;
                return true;
            }

            bool Load()
            {
                return GetCaster()->GetEntry() == NPC_BLOOD_QUEEN_LANA_THEL;
            }

            void FilterTargets(std::list<Unit*>& targets)
            {
                uint32 targetCount = (targets.size() + 2) / 3;
                targets.remove_if (BloodboltHitCheck(static_cast<LanaThelAI*>(GetCaster()->GetAI())));
                Quantum::DataPackets::RandomResizeList(targets, targetCount);
                // mark targets now, effect hook has missile travel time delay (might cast next in that time)
                for (std::list<Unit*>::const_iterator itr = targets.begin(); itr != targets.end(); ++itr)
                    GetCaster()->GetAI()->SetGUID((*itr)->GetGUID(), GUID_BLOODBOLT);
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetCaster()->CastSpell(GetHitUnit(), SPELL_TWILIGHT_BLOODBOLT, true);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_blood_queen_bloodbolt_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_blood_queen_bloodbolt_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_blood_queen_bloodbolt_SpellScript();
        }
};

class spell_blood_queen_pact_of_the_darkfallen : public SpellScriptLoader
{
    public:
        spell_blood_queen_pact_of_the_darkfallen() : SpellScriptLoader("spell_blood_queen_pact_of_the_darkfallen") { }

        class spell_blood_queen_pact_of_the_darkfallen_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_blood_queen_pact_of_the_darkfallen_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if (Trinity::UnitAuraCheck(false, SPELL_PACT_OF_THE_DARKFALLEN));

                bool remove = true;
                std::list<Unit*>::const_iterator itrEnd = unitList.end(), itr, itr2;
                // we can do this, unitList is MAX 4 in size
                for (itr = unitList.begin(); itr != itrEnd && remove; ++itr)
                {
                    if (!GetCaster()->IsWithinDist(*itr, 5.0f, false))
                        remove = false;

                    for (itr2 = unitList.begin(); itr2 != itrEnd && remove; ++itr2)
					{
                        if (itr != itr2 && !(*itr2)->IsWithinDist(*itr, 5.0f, false))
                            remove = false;
					}
                }

                if (remove)
                {
                    if (InstanceScript* instance = GetCaster()->GetInstanceScript())
                    {
                        instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PACT_OF_THE_DARKFALLEN);
                        unitList.clear();
                    }
                }
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_blood_queen_pact_of_the_darkfallen_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_blood_queen_pact_of_the_darkfallen_SpellScript();
        }
};

class spell_blood_queen_pact_of_the_darkfallen_dmg : public SpellScriptLoader
{
    public:
        spell_blood_queen_pact_of_the_darkfallen_dmg() : SpellScriptLoader("spell_blood_queen_pact_of_the_darkfallen_dmg") { }

        class spell_blood_queen_pact_of_the_darkfallen_dmg_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_blood_queen_pact_of_the_darkfallen_dmg_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PACT_OF_THE_DARKFALLEN_DAMAGE))
                    return false;
                return true;
            }

            // this is an additional effect to be executed
            void PeriodicTick(AuraEffect const* aurEff)
            {
                SpellInfo const* damageSpell = sSpellMgr->GetSpellInfo(SPELL_PACT_OF_THE_DARKFALLEN_DAMAGE);
                int32 damage = damageSpell->Effects[EFFECT_0].CalcValue();
                float multiplier = 0.3375f + 0.1f * uint32(aurEff->GetTickNumber()/10); // do not convert to 0.01f - we need tick number/10 as INT (damage increases every 10 ticks)
                damage = int32(damage * multiplier);
                GetTarget()->CastCustomSpell(SPELL_PACT_OF_THE_DARKFALLEN_DAMAGE, SPELLVALUE_BASE_POINT0, damage, GetTarget(), true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_blood_queen_pact_of_the_darkfallen_dmg_AuraScript::PeriodicTick, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_blood_queen_pact_of_the_darkfallen_dmg_AuraScript();
        }
};

class spell_blood_queen_pact_of_the_darkfallen_dmg_target : public SpellScriptLoader
{
    public:
        spell_blood_queen_pact_of_the_darkfallen_dmg_target() : SpellScriptLoader("spell_blood_queen_pact_of_the_darkfallen_dmg_target") { }

        class spell_blood_queen_pact_of_the_darkfallen_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_blood_queen_pact_of_the_darkfallen_dmg_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if (Trinity::UnitAuraCheck(true, SPELL_PACT_OF_THE_DARKFALLEN));
                unitList.push_back(GetCaster());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_blood_queen_pact_of_the_darkfallen_dmg_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_blood_queen_pact_of_the_darkfallen_dmg_SpellScript();
        }
};

class spell_blood_queen_uncontrollable_frenzy : public SpellScriptLoader
{
    public:
        spell_blood_queen_uncontrollable_frenzy() : SpellScriptLoader("spell_blood_queen_uncontrollable_frenzy") { }

        class spell_blood_queen_uncontrollable_frenzy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_blood_queen_uncontrollable_frenzy_AuraScript);

            void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (!GetTarget())
                    return;

                bool valid = false;

                if (InstanceScript* instance = GetTarget()->GetInstanceScript())
				{
                    if (instance->GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) == IN_PROGRESS)
					{
                        if (GetTarget()->GetCharmerGUID() == instance->GetData64(DATA_BLOOD_QUEEN_LANA_THEL))
                            valid = true;
					}
				}

                if (!valid)
                {
                    GetTarget()->RemoveAurasDueToSpell(SPELL_UNCONTROLLABLE_FRENZY);
                    GetTarget()->RemoveAurasDueToSpell(SPELL_UNCONTROLLABLE_FRENZY_1);
                    GetTarget()->RemoveAurasDueToSpell(SPELL_UNCONTROLLABLE_FRENZY_2);
                }
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_blood_queen_uncontrollable_frenzy_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_MECHANIC_IMMUNITY_MASK, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_blood_queen_uncontrollable_frenzy_AuraScript();
        }
};

class achievement_once_bitten_twice_shy_n : public AchievementCriteriaScript
{
public:
	achievement_once_bitten_twice_shy_n() : AchievementCriteriaScript("achievement_once_bitten_twice_shy_n") { }

	bool OnCheck(Player* source, Unit* target)
	{
		if (!target)
			return false;

		if (LanaThelAI* lanaThelAI = CAST_AI(LanaThelAI, target->GetAI()))
		{
			if (lanaThelAI->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL || lanaThelAI->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC)
				return !lanaThelAI->WasVampire(source->GetGUID());
		}
		return false;
	}
};

class achievement_once_bitten_twice_shy_v : public AchievementCriteriaScript
{
public:
	achievement_once_bitten_twice_shy_v() : AchievementCriteriaScript("achievement_once_bitten_twice_shy_v") { }

	bool OnCheck(Player* source, Unit* target)
	{
		if (!target)
			return false;

		if (LanaThelAI* lanaThelAI = CAST_AI(LanaThelAI, target->GetAI()))
		{
			if (lanaThelAI->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL || lanaThelAI->GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC)
				return lanaThelAI->WasVampire(source->GetGUID());
		}
		return false;
	}
};

class achievement_once_bitten_twice_shy_n25 : public AchievementCriteriaScript
{
public:
	achievement_once_bitten_twice_shy_n25() : AchievementCriteriaScript("achievement_once_bitten_twice_shy_n25") { }

	bool OnCheck(Player* source, Unit* target)
	{
		if (!target)
			return false;

		if (LanaThelAI* lanaThelAI = CAST_AI(LanaThelAI, target->GetAI()))
		{
			if (lanaThelAI->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL || lanaThelAI->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
				return !lanaThelAI->WasVampire(source->GetGUID());
		}
		return false;
	}
};

class achievement_once_bitten_twice_shy_v25 : public AchievementCriteriaScript
{
public:
	achievement_once_bitten_twice_shy_v25() : AchievementCriteriaScript("achievement_once_bitten_twice_shy_v25") { }

	bool OnCheck(Player* source, Unit* target)
	{
		if (!target)
			return false;

		if (LanaThelAI* lanaThelAI = CAST_AI(LanaThelAI, target->GetAI()))
		{
			if (lanaThelAI->GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL || lanaThelAI->GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
				return lanaThelAI->WasVampire(source->GetGUID());
		}
		return false;
	}
};

class lana_thel_bite_remover : public PlayerScript
{
public:
	lana_thel_bite_remover() : PlayerScript("lana_thel_bite_remover") {}

	void OnLogin(Player* player)
	{
		player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_BASIC);
		player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_1);
		player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_2);
		player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_3);
		player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_4);
		player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_5);
		player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_6);
		player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_7);
		player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_8);
		player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_9);
		player->RemoveAurasDueToSpell(SPELL_FRENZIED_BLOODTHIRST);
		player->RemoveAurasDueToSpell(SPELL_FRENZIED_BLOODTHIRST_2);
		player->RemoveAurasDueToSpell(SPELL_UNCONTROLLABLE_FRENZY);
		player->RemoveAurasDueToSpell(SPELL_UNCONTROLLABLE_FRENZY_1);
		player->RemoveAurasDueToSpell(SPELL_UNCONTROLLABLE_FRENZY_2);
		player->RemoveAurasDueToSpell(SPELL_BLOOD_MIRROR_DAMAGE);
		player->RemoveAurasDueToSpell(SPELL_BLOOD_MIRROR_DUMMY);
		player->RemoveAurasDueToSpell(SPELL_BLOOD_MIRROR_VISUAL);
	}

	void OnLogout(Player* player)
	{
		player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_BASIC);
		player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_1);
		player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_2);
		player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_3);
		player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_4);
		player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_5);
		player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_6);
		player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_7);
		player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_8);
		player->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_THE_BLOOD_QUEEN_9);
		player->RemoveAurasDueToSpell(SPELL_FRENZIED_BLOODTHIRST);
		player->RemoveAurasDueToSpell(SPELL_FRENZIED_BLOODTHIRST_2);
		player->RemoveAurasDueToSpell(SPELL_UNCONTROLLABLE_FRENZY);
		player->RemoveAurasDueToSpell(SPELL_UNCONTROLLABLE_FRENZY_1);
		player->RemoveAurasDueToSpell(SPELL_UNCONTROLLABLE_FRENZY_2);
		player->RemoveAurasDueToSpell(SPELL_BLOOD_MIRROR_DAMAGE);
		player->RemoveAurasDueToSpell(SPELL_BLOOD_MIRROR_DUMMY);
		player->RemoveAurasDueToSpell(SPELL_BLOOD_MIRROR_VISUAL);
	}
};

void AddSC_boss_blood_queen_lana_thel()
{
    new boss_blood_queen_lana_thel();
    new spell_blood_queen_vampiric_bite();
    new spell_blood_queen_frenzied_bloodthirst();
    new spell_blood_queen_bloodbolt();
    new spell_blood_queen_pact_of_the_darkfallen();
    new spell_blood_queen_pact_of_the_darkfallen_dmg();
    new spell_blood_queen_pact_of_the_darkfallen_dmg_target();
	new spell_blood_queen_uncontrollable_frenzy();
    new achievement_once_bitten_twice_shy_n();
    new achievement_once_bitten_twice_shy_v();
	new achievement_once_bitten_twice_shy_n25();
	new achievement_once_bitten_twice_shy_v25();
	new lana_thel_bite_remover();
}