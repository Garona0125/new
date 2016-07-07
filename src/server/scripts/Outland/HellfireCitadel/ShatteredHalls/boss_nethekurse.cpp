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
#include "GameEventMgr.h"
#include "shattered_halls.h"

struct Say
{
    int32 id;
};

static Say PeonAttacked[] =
{
    {-1540001},
    {-1540002},
    {-1540003},
    {-1540004},
};

static Say PeonDies[] =
{
    {-1540005},
    {-1540006},
    {-1540007},
    {-1540008},
};

enum Texts
{
    SAY_INTRO       = -1540000,
    SAY_TAUNT_1     = -1540009,
    SAY_TAUNT_2     = -1540010,
    SAY_TAUNT_3     = -1540011,
    SAY_AGGRO_1     = -1540012,
    SAY_AGGRO_2     = -1540013,
    SAY_AGGRO_3     = -1540014,
    SAY_SLAY_1      = -1540015,
    SAY_SLAY_2      = -1540016,
    SAY_DIE         = -1540017,
};

enum Spells
{
    SPELL_DEATH_COIL           = 30500,
    SPELL_DARK_SPIN            = 30502,
    SPELL_SHADOW_FISSURE       = 30496,
    SPELL_SHADOW_CLEAVE_5N     = 30495,
	SPELL_SHADOW_CLEAVE_5H     = 35953,
    SPELL_SHADOW_SLAM          = 35953,
    SPELL_HEMORRHAGE           = 30478,
    SPELL_CONSUMPTION          = 30497,
    SPELL_TEMPORARY_VISUAL     = 39312,
};

enum SeasonalData
{
	 GAME_EVENT_WINTER_VEIL = 2,
};

class boss_grand_warlock_nethekurse : public CreatureScript
{
    public:
        boss_grand_warlock_nethekurse() : CreatureScript("boss_grand_warlock_nethekurse") { }

        struct boss_grand_warlock_nethekurseAI : public QuantumBasicAI
        {
            boss_grand_warlock_nethekurseAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            bool HeroicMode;

            bool IntroOnce;
            bool IsIntroEvent;
            bool IsMainEvent;
            bool SpinOnce;
            bool Phase;

            uint32 PeonEngagedCount;
            uint32 PeonKilledCount;

            uint32 IntroEventTimer;
            uint32 DeathCoilTimer;
            uint32 ShadowFissureTimer;
            uint32 CleaveTimer;

            void Reset()
            {
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

				DoCast(me, SPELL_UNIT_DETECTION_ALL);

				PeonRespawner();

                IsIntroEvent = false;
                IntroOnce = false;
                IsMainEvent = false;
                SpinOnce = false;
                Phase = false;

                PeonEngagedCount = 0;
                PeonKilledCount = 0;

                IntroEventTimer = 90000;
                DeathCoilTimer = 2000;
                ShadowFissureTimer = 4000;
                CleaveTimer = 6000;

				if (sGameEventMgr->IsActiveEvent(GAME_EVENT_WINTER_VEIL))
					me->SetDisplayId(MODEL_CHRISTMAS_WARLOCK_NETHERKURSE);
            }

            void DoYellForPeonAggro()
            {
                if (PeonEngagedCount >= 4)
                    return;

                DoSendQuantumText(PeonAttacked[PeonEngagedCount].id, me);
                ++PeonEngagedCount;
            }

            void DoYellForPeonDeath()
            {
                if (PeonKilledCount >= 4)
                    return;

                DoSendQuantumText(PeonDies[PeonKilledCount].id, me);
                ++PeonKilledCount;

                if (PeonKilledCount == 4)
                {
                    IsIntroEvent = false;
                    IsMainEvent = true;
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                }
            }

            void DoTauntPeons()
            {
                DoSendQuantumText(RAND(SAY_TAUNT_1, SAY_TAUNT_2, SAY_TAUNT_3), me);

                IsIntroEvent = false;
                PeonEngagedCount = 4;
                PeonKilledCount = 4;
                IsMainEvent = true;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

			void PeonRespawner()
			{
				std::list<Creature*> PeonList;
				me->GetCreatureListWithEntryInGrid(PeonList, NPC_FEL_ORC_CONVERT, 150.0f);

				if (!PeonList.empty())
				{
					for (std::list<Creature*>::const_iterator itr = PeonList.begin(); itr != PeonList.end(); ++itr)
					{
						if (Creature* peon = *itr)
						{
							peon->Respawn();
						}
					}
				}
			}

            void AttackStart(Unit* who)
            {
                if (IsIntroEvent || !IsMainEvent)
                    return;

                if (me->Attack(who, true))
                {
                    if (Phase)
                        DoStartNoMovement(who);
                    else
                        DoStartMovement(who);
                }
            }

            void MoveInLineOfSight(Unit* who)
            {
                if (!IntroOnce && me->IsWithinDistInMap(who, 40.0f))
				{
					if (who->GetTypeId() != TYPE_ID_PLAYER)
						return;

					DoSendQuantumText(SAY_INTRO, me);

					IntroOnce = true;
					IsIntroEvent = true;

					if (instance)
						instance->SetData(TYPE_NETHEKURSE, IN_PROGRESS);
				}

				if (IsIntroEvent || !IsMainEvent)
					return;

				QuantumBasicAI::MoveInLineOfSight(who);
			}

            void EnterToBattle(Unit* /*who*/)
            {
                DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);
            }

            void JustSummoned(Creature* summoned)
            {
                summoned->SetCurrentFaction(16);
                summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                summoned->CastSpell(summoned, SPELL_TEMPORARY_VISUAL, true);
                summoned->CastSpell(summoned, SPELL_CONSUMPTION, false, 0, 0, me->GetGUID());
            }

            void KilledUnit(Unit* /*who*/)
            {
                DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

			void JustReachedHome()
			{
				Reset();
			}

            void JustDied(Unit* /*killer*/)
            {
                DoSendQuantumText(SAY_DIE, me);

                if (!instance)
                    return;

                instance->SetData(TYPE_NETHEKURSE, DONE);
            }

            void UpdateAI(const uint32 diff)
            {
                if (IsIntroEvent)
                {
                    if (!instance)
                        return;

                    if (instance->GetData(TYPE_NETHEKURSE) == IN_PROGRESS)
                    {
                        if (IntroEventTimer <= diff)
                            DoTauntPeons();
                        else
                            IntroEventTimer -= diff;
                    }
                }

                if (!UpdateVictim())
                    return;

                if (!IsMainEvent)
                    return;

                if (Phase)
                {
                    if (!SpinOnce)
                    {
                        DoCastVictim(SPELL_DARK_SPIN);
                        SpinOnce = true;
                    }

                    if (CleaveTimer <= diff)
                    {
                        DoCastVictim(DUNGEON_MODE(SPELL_SHADOW_CLEAVE_5N, SPELL_SHADOW_CLEAVE_5H));
                        CleaveTimer = urand(6000, 7000);
                    }
                    else CleaveTimer -= diff;
                }
                else
                {
                    if (ShadowFissureTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
                            DoCast(target, SPELL_SHADOW_FISSURE);
							ShadowFissureTimer = urand(8000, 9000);
						}
                    }
                    else ShadowFissureTimer -= diff;

                    if (DeathCoilTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
                            DoCast(target, SPELL_DEATH_COIL);
							DeathCoilTimer = urand(4000, 5000);
						}
                    }
                    else DeathCoilTimer -= diff;

                    if (!HealthAbovePct(20))
                        Phase = true;

                    DoMeleeAttackIfReady();
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_grand_warlock_nethekurseAI(creature);
        }
};

class npc_fel_orc_convert : public CreatureScript
{
    public:
        npc_fel_orc_convert() : CreatureScript("npc_fel_orc_convert") { }

        struct npc_fel_orc_convertAI : public QuantumBasicAI
        {
            npc_fel_orc_convertAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
            uint32 HemorrhageTimer;

            void Reset()
            {
                me->SetNoCallAssistance(true);
                HemorrhageTimer = 3000;
            }

            void MoveInLineOfSight(Unit* /*who*/) {}

            void EnterToBattle(Unit* /*who*/)
            {
                if (instance)
                {
                    if (instance->GetData64(DATA_NETHEKURSE))
                    {
                        Creature* pKurse = Unit::GetCreature(*me, instance->GetData64(DATA_NETHEKURSE));
                        if (pKurse && me->IsWithinDist(pKurse, 45.0f))
                        {
                            CAST_AI(boss_grand_warlock_nethekurse::boss_grand_warlock_nethekurseAI, pKurse->AI())->DoYellForPeonAggro();

                            if (instance->GetData(TYPE_NETHEKURSE) == IN_PROGRESS)
                                return;
                            else
                                instance->SetData(TYPE_NETHEKURSE, IN_PROGRESS);
                        }
                    }
                }
            }

            void JustDied(Unit* /*Killer*/)
            {
                if (instance)
                {
                    if (instance->GetData(TYPE_NETHEKURSE) != IN_PROGRESS)
                        return;
                    if (instance->GetData64(DATA_NETHEKURSE))
                        if (Creature* kurse = Unit::GetCreature(*me, instance->GetData64(DATA_NETHEKURSE)))
                            CAST_AI(boss_grand_warlock_nethekurse::boss_grand_warlock_nethekurseAI, kurse->AI())->DoYellForPeonDeath();
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (HemorrhageTimer <= diff)
                {
                    DoCastVictim(SPELL_HEMORRHAGE);
                    HemorrhageTimer = 5000;
                }
				else HemorrhageTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_fel_orc_convertAI(creature);
        }
};

class npc_lesser_shadow_fissure : public CreatureScript
{
    public:
        npc_lesser_shadow_fissure() : CreatureScript("npc_lesser_shadow_fissure") { }

        struct npc_lesser_shadow_fissureAI : public QuantumBasicAI
        {
            npc_lesser_shadow_fissureAI(Creature* creature) : QuantumBasicAI(creature)
			{
				SetCombatMovement(false);
			}

            void Reset() {}

            void MoveInLineOfSight(Unit* /*who*/) {}

            void AttackStart(Unit* /*who*/) {}

            void EnterToBattle(Unit* /*who*/) {}
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_lesser_shadow_fissureAI(creature);
        }
};

void AddSC_boss_grand_warlock_nethekurse()
{
    new boss_grand_warlock_nethekurse();
    new npc_fel_orc_convert();
    new npc_lesser_shadow_fissure();
}