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
#include "utgarde_pinnacle.h"

enum Yells
{
    SAY_AGGRO               = -1575015,
    SAY_SLAY_1              = -1575016,
    SAY_SLAY_2              = -1575017,
    SAY_SLAY_3              = -1575018,
    SAY_DEATH               = -1575019,
    SAY_SACRIFICE_PLAYER_1  = -1575020,
    SAY_SACRIFICE_PLAYER_2  = -1575021,
    SAY_SACRIFICE_PLAYER_3  = -1575022,
    SAY_SACRIFICE_PLAYER_4  = -1575023,
    SAY_SACRIFICE_PLAYER_5  = -1575024,
	SAY_SVALA_INTRO_1       = -1575025,
    SAY_SVALA_INTRO_2       = -1575026,
    SAY_SVALA_INTRO_3       = -1575027,
	SAY_ARTHAS_INTRO_1      = -1585028,
	SAY_ARTHAS_INTRO_2      = -1585029,
};

enum Spells
{
    SPELL_SVALA_TRANSFORMING1                     = 54140,
    SPELL_SVALA_TRANSFORMING2                     = 54205,
    SPELL_TRANSFORMING_CHANNEL                    = 54142,
    SPELL_CALL_FLAMES                             = 48258, // caster effect only, triggers event 17841
    SPELL_SINSTER_STRIKE_5N                       = 15667,
    SPELL_SINSTER_STRIKE_5H                       = 59409,
    SPELL_RITUAL_PREPARATION                      = 48267,
    SPELL_RITUAL_OF_THE_SWORD                     = 48276,
    SPELL_RITUAL_STRIKE_TRIGGER                   = 48331, // triggers 48277 & 59930, needs NPC_RITUAL_TARGET as spell_script_target
    SPELL_RITUAL_DISARM                           = 54159,
    SPELL_RITUAL_STRIKE_EFF_1                     = 48277,
    SPELL_RITUAL_STRIKE_EFF_2                     = 59930,
    SPELL_SUMMONED_VIS                            = 64446,
    SPELL_RITUAL_CHANNELER_1                      = 48271,
    SPELL_RITUAL_CHANNELER_2                      = 48274,
    SPELL_RITUAL_CHANNELER_3                      = 48275,
    SPELL_PARALYZE                                = 48278,
    SPELL_SHADOWS_IN_THE_DARK                     = 59407,
	SPELL_ARTHAS_VISUAL                           = 54134,
};

enum SvalaPhase
{
    IDLE,
    INTRO,
    NORMAL,
    SACRIFICING,
    SVALA_DEAD,
};

static const float spectatorWP[2][3] =
{
    {296.95f,-312.76f,86.36f},
    {297.69f,-275.81f,86.36f}
};

static Position ArthasPos = { 295.81f, -366.16f, 92.57f, 1.58f };

class boss_svala_sorrowgrave : public CreatureScript
{
public:
    boss_svala_sorrowgrave() : CreatureScript("boss_svala_sorrowgrave") { }

    struct boss_svala_sorrowgraveAI : public QuantumBasicAI
    {
        boss_svala_sorrowgraveAI(Creature* creature) : QuantumBasicAI(creature), Summons(creature)
        {
            instance = creature->GetInstanceScript();
            Phase = IDLE;

            me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_RITUAL_STRIKE_EFF_1, true);
            me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_RITUAL_STRIKE_EFF_2, true);
        }

        InstanceScript* instance;
        SummonList Summons;
        SvalaPhase Phase;

        Position pos;
        float x, y, z;

        uint32 IntroTimer;
		uint32 SinsterStrikeTimer;
        uint32 CallOfFlamesTimer;
        uint32 SacrificeTimer;

        uint8 IntroPhase;
        uint8 SacrificePhase;

        TempSummon* arthas;
        uint64 arthasGUID;

        bool sacrificed;

        void Reset()
        {
            sacrificed = false;
            SetCombatMovement(true);

            Summons.DespawnAll();

            me->RemoveAllAuras();

            if (Phase > NORMAL)
                Phase = NORMAL;

            me->SetDisableGravity(Phase == NORMAL);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            IntroTimer = 1*IN_MILLISECONDS;
            IntroPhase = 0;
            arthasGUID = 0;

			instance->SetData(DATA_SVALA_SORROWGRAVE_EVENT, NOT_STARTED);
			instance->SetData64(DATA_SACRIFICED_PLAYER, 0);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

            SinsterStrikeTimer = 7*IN_MILLISECONDS;
            CallOfFlamesTimer = urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS);

			instance->SetData(DATA_SVALA_SORROWGRAVE_EVENT, IN_PROGRESS);
        }

        void JustSummoned(Creature* summon)
        {
            if (summon->GetEntry() == NPC_RITUAL_CHANNELER)
                summon->CastSpell(summon, SPELL_SUMMONED_VIS, true);

            Summons.Summon(summon);
        }

        void SummonedCreatureDespawn(Creature* summon)
        {
            Summons.Despawn(summon);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!who)
                return;

            if (Phase == IDLE && me->IsValidAttackTarget(who) && me->IsWithinDistInMap(who, 40))
            {
                Phase = INTRO;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                if (GameObject* mirror = GetClosestGameObjectWithEntry(me, GO_UTGARDE_MIRROR, 100.0f))
                    mirror->SetGoState(GO_STATE_READY);

                if (Creature* arthas = me->SummonCreature(NPC_ARTHAS, ArthasPos, TEMPSUMMON_MANUAL_DESPAWN))
                {
                    arthas->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    arthasGUID = arthas->GetGUID();
                }
            }
        }

		void KilledUnit(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
		}

        void JustDied(Unit* /*killer*/)
        {
            if (Phase == SACRIFICING)
                SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);

            me->HandleEmoteCommand(EMOTE_ONESHOT_FLY_DEATH);

            Summons.DespawnAll();

			instance->SetData(DATA_SVALA_SORROWGRAVE_EVENT, DONE);

            DoSendQuantumText(SAY_DEATH, me);
        }

        void SpellHitTarget(Unit* /*target*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_RITUAL_STRIKE_EFF_1 && Phase != NORMAL && Phase != SVALA_DEAD)
            {
                Phase = NORMAL;
                SetCombatMovement(true);

                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 300.0f, true))
                    me->GetMotionMaster()->MoveChase(target);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (Phase == IDLE)
                return;

            if (Phase == INTRO)
            {
                if (IntroTimer <= diff)
                {
                    Creature* arthas = Unit::GetCreature(*me, arthasGUID);
                    if (!arthas)
                        return;

                    switch (IntroPhase)
                    {
                        case 0:
							arthas->CastSpell(arthas, SPELL_ARTHAS_VISUAL, true);
							DoSendQuantumText(SAY_SVALA_INTRO_1, me);
                            ++IntroPhase;
                            IntroTimer = 8100;
                            break;
                        case 1:
							DoSendQuantumText(SAY_ARTHAS_INTRO_1, arthas);
                            ++IntroPhase;
                            IntroTimer = 10000;
                            break;
						case 2:
                            arthas->CastSpell(me, SPELL_TRANSFORMING_CHANNEL, false);
                            pos.Relocate(me);
                            pos.m_positionZ += 8.0f;
                            me->GetMotionMaster()->MoveTakeoff(0, pos);
                            // spectators flee event
							if (instance)
							{
								std::list<Creature*> lspectatorList;
								GetCreatureListWithEntryInGrid(lspectatorList, me, NPC_SPECTATOR, 100.0f);
								for (std::list<Creature*>::iterator itr = lspectatorList.begin(); itr != lspectatorList.end(); ++itr)
								{
									if ((*itr)->IsAlive())
									{
										(*itr)->SetStandState(UNIT_STAND_STATE_STAND);
										(*itr)->SetWalk(false);
										(*itr)->GetMotionMaster()->MovePoint(1, spectatorWP[0][0], spectatorWP[0][1], spectatorWP[0][2]);
									}
								}
							}
                            ++IntroPhase;
                            IntroTimer = 4200;
                            break;
                        case 3:
                            me->CastSpell(me, SPELL_SVALA_TRANSFORMING1, false);
                            ++IntroPhase;
                            IntroTimer = 6200;
                            break;
                        case 4:
                            me->CastSpell(me, SPELL_SVALA_TRANSFORMING2, false);
                            arthas->InterruptNonMeleeSpells(true);
                            me->RemoveAllAuras();
                            me->UpdateEntry(NPC_SVALA_SORROWGRAVE);
                            me->SetFacingToObject(arthas);
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            ++IntroPhase;
                            IntroTimer = 3200;
                            break;
                        case 5:
							DoSendQuantumText(SAY_SVALA_INTRO_2, me);
                            ++IntroPhase;
                            IntroTimer = 10000;
                            break;
                        case 6:
							DoSendQuantumText(SAY_ARTHAS_INTRO_2, arthas);
                            ++IntroPhase;
                            IntroTimer = 7200;
                            break;
                        case 7:
                            DoSendQuantumText(SAY_SVALA_INTRO_3, me);
                            me->SetOrientation(1.58f);
                            me->SendMovementFlagUpdate();
                            arthas->SetVisible(false);
                            ++IntroPhase;
                            IntroTimer = 13800;
                            break;
                        case 8:
                            pos.Relocate(me);
                            pos.m_positionX = me->GetHomePosition().GetPositionX();
                            pos.m_positionY = me->GetHomePosition().GetPositionY();
                            pos.m_positionZ = 90.6065f;
                            me->GetMotionMaster()->MoveLand(0, pos);
                            me->SetDisableGravity(false, true);
                            me->SetHover(true);
                            ++IntroPhase;
                            IntroTimer = 3000;
                            break;
                        case 9:
                            if (GameObject* mirror = GetClosestGameObjectWithEntry(me, GO_UTGARDE_MIRROR, 100.0f))
                                mirror->SetGoState(GO_STATE_ACTIVE);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            arthas->DespawnAfterAction();
                            arthasGUID = 0;
                            Phase = NORMAL;
                            break;
                    }
                }
                else IntroTimer -= diff;

                return;
            }

            if (Phase == NORMAL)
            {
                if (!UpdateVictim())
                    return;

                if (SinsterStrikeTimer <= diff)
                {
                    DoCastVictim(DUNGEON_MODE(SPELL_SINSTER_STRIKE_5N, SPELL_SINSTER_STRIKE_5H));
                    SinsterStrikeTimer = urand(5*IN_MILLISECONDS, 9*IN_MILLISECONDS);
                }
                else SinsterStrikeTimer -= diff;

                if (CallOfFlamesTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100.0f, true))
                    {
                        DoCast(target, SPELL_CALL_FLAMES);
                        CallOfFlamesTimer = urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS);
                    }
                }
				
				else CallOfFlamesTimer -= diff;

                if (!sacrificed)
                {
                    if (HealthBelowPct(HEALTH_PERCENT_50))
                    {
                        if (Unit* sacrificeTarget = SelectTarget(TARGET_RANDOM, 0, 80.0f, true))
                        {
							instance->SetData64(DATA_SACRIFICED_PLAYER, sacrificeTarget->GetGUID());

                            DoSendQuantumText(RAND(SAY_SACRIFICE_PLAYER_1, SAY_SACRIFICE_PLAYER_2, SAY_SACRIFICE_PLAYER_3, SAY_SACRIFICE_PLAYER_4, SAY_SACRIFICE_PLAYER_5), me);

                            DoCast(sacrificeTarget, SPELL_RITUAL_PREPARATION);

                            SetCombatMovement(false);

                            Phase = SACRIFICING;
                            SacrificePhase = 0;
                            SacrificeTimer = 1*IN_MILLISECONDS;

                            DoCast(me, SPELL_RITUAL_OF_THE_SWORD);
                            sacrificed = true;
                        }
                    }
                }

                DoMeleeAttackIfReady();
            }
            else  //SACRIFICING
            {
                if (SacrificeTimer <= diff)
                {
                    switch (SacrificePhase)
                    {
                        case 0:
                            // spawn ritual channelers
							DoCast(me, SPELL_RITUAL_CHANNELER_1, true);
							DoCast(me, SPELL_RITUAL_CHANNELER_2, true);
							DoCast(me, SPELL_RITUAL_CHANNELER_3, true);
                            ++SacrificePhase;
                            SacrificeTimer = 2*IN_MILLISECONDS;
                            break;
                        case 1:
                            me->StopMoving();
                            me->GetMotionMaster()->MoveIdle();
                            me->InterruptNonMeleeSpells(true);
                            DoCast(me, SPELL_RITUAL_STRIKE_TRIGGER, true);
                            ++SacrificePhase;
                            SacrificeTimer = 200;
                            break;
                        case 2:
                            DoCast(me, SPELL_RITUAL_DISARM);
                            ++SacrificePhase;
                            break;
                        case 3:
                            break;
                    }
                }
                else SacrificeTimer -= diff;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_svala_sorrowgraveAI(creature);
    }
};

class npc_ritual_channeler : public CreatureScript
{
public:
    npc_ritual_channeler() : CreatureScript("npc_ritual_channeler") { }

    struct npc_ritual_channelerAI : public QuantumBasicAI
    {
        npc_ritual_channelerAI(Creature* creature) :QuantumBasicAI(creature)
        {
			SetCombatMovement(false);
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        uint32 ParalyzeTimer;

        void Reset()
        {
            ParalyzeTimer = 1600;

			if (IsHeroic())
				DoCast(me, SPELL_SHADOWS_IN_THE_DARK);
        }

        void UpdateAI(const uint32 diff)
        {
            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (ParalyzeTimer <= diff)
            {
				if (Unit* victim = me->GetUnit(*me, instance->GetData64(DATA_SACRIFICED_PLAYER)))
					DoCast(victim, SPELL_PARALYZE, false);

				ParalyzeTimer = 200;
            }
            else ParalyzeTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ritual_channelerAI(creature);
    }
};

class npc_spectator : public CreatureScript
{
public:
    npc_spectator() : CreatureScript("npc_spectator") { }

    struct npc_spectatorAI : public QuantumBasicAI
    {
        npc_spectatorAI(Creature* creature) : QuantumBasicAI(creature) { }

        void Reset() {}

        void MovementInform(uint32 motionType, uint32 pointId)
        {
            if (motionType == POINT_MOTION_TYPE)
            {
                if (pointId == 1)
                    me->GetMotionMaster()->MovePoint(2, spectatorWP[1][0], spectatorWP[1][1], spectatorWP[1][2]);
                else if (pointId == 2)
                    me->DespawnAfterAction(1000);
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spectatorAI(creature);
    }
};

class checkRitualTarget
{
public:
	explicit checkRitualTarget(Unit* _caster) : caster(_caster) { }

	Unit* caster;

	bool operator() (Unit* unit)
	{
		if (InstanceScript* instance = caster->GetInstanceScript())
			if (instance->GetData64(DATA_SACRIFICED_PLAYER) == unit->GetGUID())
				return false;

		return true;
	}
};

class spell_paralyze_pinnacle : public SpellScriptLoader
{
    public:
        spell_paralyze_pinnacle() : SpellScriptLoader("spell_paralyze_pinnacle") { }

        class spell_paralyze_pinnacle_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_paralyze_pinnacle_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
				unitList.remove_if (checkRitualTarget(GetCaster()));
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_paralyze_pinnacle_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_paralyze_pinnacle_SpellScript();
        }
};

class achievement_incredible_hulk : public AchievementCriteriaScript
{
public:
	achievement_incredible_hulk() : AchievementCriteriaScript("achievement_incredible_hulk") { }

	bool OnCheck(Player* /*player*/, Unit* target)
	{
		return target && target->IsAIEnabled && target->GetAI()->GetData(DATA_INCREDIBLE_HULK);
	}
};

void AddSC_boss_svala_sorrowgrave()
{
    new boss_svala_sorrowgrave();
    new npc_ritual_channeler();
    new npc_spectator();
    new spell_paralyze_pinnacle();
    new achievement_incredible_hulk();
}