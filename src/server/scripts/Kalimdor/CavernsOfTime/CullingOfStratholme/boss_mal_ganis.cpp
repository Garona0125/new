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
#include "culling_of_stratholme.h"

enum Spells
{
    SPELL_CARRION_SWARM_5N            = 52720,
    SPELL_CARRION_SWARM_5H            = 58852,
    SPELL_MIND_BLAST_5N               = 52722,
    SPELL_MIND_BLAST_5H               = 58850,
    SPELL_SLEEP_5N                    = 52721,
    SPELL_SLEEP_5H                    = 58849,
    SPELL_VAMPIRIC_TOUCH              = 52723,
	SPELL_MAL_GANIS_KILL_CREDIT       = 58124, // Quest credit
    SPELL_KILL_CREDIT                 = 58630, // Serverside
};

enum Yells
{
    SAY_INTRO_1              = -1595009,
    SAY_INTRO_2              = -1595010,
    SAY_AGGRO                = -1595011,
    SAY_KILL_1               = -1595012, // NEED MORE INFO
    SAY_KILL_2               = -1595013,
    SAY_KILL_3               = -1595014,
    SAY_SLAY_1               = -1595015,
    SAY_SLAY_2               = -1595016,
    SAY_SLAY_3               = -1595017,
    SAY_SLAY_4               = -1595018,
    SAY_SLEEP_1              = -1595019,
    SAY_SLEEP_2              = -1595020,
    SAY_30HEALTH             = -1595021,
    SAY_15HEALTH             = -1595022,
    SAY_ESCAPE_SPEECH_1      = -1595023,
    SAY_ESCAPE_SPEECH_2      = -1595024,
    SAY_OUTRO                = -1595025,
};

enum CombatPhases
{
    PHASE_COMBAT,
    PHASE_OUTRO,
};

class boss_mal_ganis : public CreatureScript
{
    public:
        boss_mal_ganis() : CreatureScript("boss_mal_ganis") { }

        struct boss_mal_ganisAI : public QuantumBasicAI
        {
            boss_mal_ganisAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

			uint32 CarrionSwarmTimer;
            uint32 MindBlastTimer;
            uint32 VampiricTouchTimer;
            uint32 SleepTimer;
            uint8 OutroStep;
            uint32 OutroTimer;

            bool IsBelow30PctHealth;
            bool IsBelow15PctHealth;

            CombatPhases Phase;
            InstanceScript* instance;

            void Reset()
            {
				IsBelow30PctHealth = false;
				IsBelow15PctHealth = false;

				Phase = PHASE_COMBAT;

				CarrionSwarmTimer = 6000;
				MindBlastTimer = 12500;
				VampiricTouchTimer = urand(10000, 15000);
				SleepTimer = urand(10000, 15000);
				OutroTimer = 1000;

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
				instance->SetData(DATA_MAL_GANIS_EVENT, NOT_STARTED);
			}

            void EnterToBattle(Unit* /*who*/)
            {
				DoSendQuantumText(SAY_AGGRO, me);

				instance->SetData(DATA_MAL_GANIS_EVENT, IN_PROGRESS);
            }

            void DamageTaken(Unit* /*attacker*/, uint32 &damage)
            {
                if (damage >= me->GetHealth())
                    damage = me->GetHealth() - 1;
            }

            void KilledUnit(Unit* victim)
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3, SAY_SLAY_4), me);
            }

            void UpdateAI(uint32 const diff)
            {
                switch (Phase)
                {
                    case PHASE_COMBAT:

                        if (!UpdateVictim())
                            return;

                        if (me->HasUnitState(UNIT_STATE_CASTING))
                            return;

                        if (!IsBelow30PctHealth && HealthBelowPct(HEALTH_PERCENT_30))
                        {
                            DoSendQuantumText(SAY_30HEALTH, me);
                            IsBelow30PctHealth = true;
                        }

                        if (!IsBelow15PctHealth && HealthBelowPct(HEALTH_PERCENT_15))
                        {
                            DoSendQuantumText(SAY_15HEALTH, me);
                            IsBelow15PctHealth = true;
                        }

                        if (HealthBelowPct(1))
                        {
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                            OutroStep = 1;
                            Phase = PHASE_OUTRO;
                            return;
                        }

                        if (Creature* arthas = me->GetCreature(*me, instance ? instance->GetData64(DATA_ARTHAS) : 0))
						{
                            if (arthas->IsDead())
                            {
                                EnterEvadeMode();
                                me->DespawnAfterAction();

								instance->SetData(DATA_MAL_GANIS_EVENT, FAIL);
                            }
						}

                        if (CarrionSwarmTimer <= diff)
                        {
                            DoCastVictim(DUNGEON_MODE(SPELL_CARRION_SWARM_5N, SPELL_CARRION_SWARM_5H));
                            CarrionSwarmTimer = 7000;
                        }
                        else CarrionSwarmTimer -= diff;

                        if (MindBlastTimer <= diff)
                        {
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
							{
                                DoCast(target, DUNGEON_MODE(SPELL_MIND_BLAST_5N, SPELL_MIND_BLAST_5H));
								MindBlastTimer = 6000;
							}
                        }
                        else MindBlastTimer -= diff;

                        if (VampiricTouchTimer <= diff)
                        {
                            DoCast(me, SPELL_VAMPIRIC_TOUCH);
                            VampiricTouchTimer = 20000;
                        }
                        else VampiricTouchTimer -= diff;

                        if (SleepTimer <= diff)
                        {
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
							{
								DoSendQuantumText(RAND(SAY_SLEEP_1, SAY_SLEEP_2), me);
                                DoCast(target, DUNGEON_MODE(SPELL_SLEEP_5N, SPELL_SLEEP_5H));
								SleepTimer = urand(12500, 17500);
							}
                        }
                        else SleepTimer -= diff;

                        DoMeleeAttackIfReady();
                        break;
                    case PHASE_OUTRO:
                        if (OutroTimer <= diff)
                        {
							switch (OutroStep)
                            {
                                case 1:
                                    DoSendQuantumText(SAY_ESCAPE_SPEECH_1, me);
                                    me->GetMotionMaster()->MoveTargetedHome();
                                    ++OutroStep;
                                    OutroTimer = 8000;
                                    break;
                                case 2:
                                    me->SetTarget(instance ? instance->GetData64(DATA_ARTHAS) : 0);
                                    me->HandleEmoteCommand(29);
                                    DoSendQuantumText(SAY_ESCAPE_SPEECH_2, me);
                                    ++OutroStep;
                                    OutroTimer = 9000;
                                    break;
                                case 3:
                                    DoSendQuantumText(SAY_OUTRO, me);
                                    ++OutroStep;
                                    OutroTimer = 16000;
                                    break;
                                case 4:
                                    me->HandleEmoteCommand(33);
                                    ++OutroStep;
                                    OutroTimer = 500;
                                    break;
                                case 5:
									instance->SetData(DATA_MAL_GANIS_EVENT, DONE);
									DoCastAOE(SPELL_MAL_GANIS_KILL_CREDIT, true);
									DoCastAOE(SPELL_KILL_CREDIT, true);
									instance->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, SPELL_KILL_CREDIT, NULL);
									me->DespawnAfterAction(2*IN_MILLISECONDS);
                                    ++OutroStep;
                                    OutroTimer = 2000;
                                    break;
                            }
                        }
                        else OutroTimer -= diff;
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_mal_ganisAI(creature);
        }
};

void AddSC_boss_mal_ganis()
{
    new boss_mal_ganis();
}