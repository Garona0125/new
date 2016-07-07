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
#include "Vehicle.h"
#include "ulduar.h"

enum Texts
{
    SAY_AGGRO                                   = -1603240,
    SAY_HARDMODE_ON                             = -1603241,
    SAY_MKII_ACTIVATE                           = -1603242,
    SAY_MKII_SLAY_1                             = -1603243,
    SAY_MKII_SLAY_2                             = -1603244,
    SAY_MKII_DEATH                              = -1603245,
    SAY_VX001_ACTIVATE                          = -1603246,
    SAY_VX001_SLAY_1                            = -1603247,
    SAY_VX001_SLAY_2                            = -1603248,
    SAY_VX001_DEATH                             = -1603249,
    SAY_AERIAL_ACTIVATE                         = -1603250,
    SAY_AERIAL_SLAY_1                           = -1603251,
    SAY_AERIAL_SLAY_2                           = -1603252,
    SAY_AERIAL_DEATH                            = -1603253,
    SAY_V07TRON_ACTIVATE                        = -1603254,
    SAY_V07TRON_SLAY_1                          = -1603255,
    SAY_V07TRON_SLAY_2                          = -1603256,
    SAY_V07TRON_DEATH                           = -1603257,
    SAY_BERSERK                                 = -1603258,
	// Alarm Timer (Hard Mode)
	SAY_ALARM_START                             = -1606241,
	SAY_ALARM_TIMER_10                          = -1606242,
	SAY_ALARM_TIMER_9                           = -1606243,
	SAY_ALARM_TIMER_8                           = -1606244,
	SAY_ALARM_TIMER_7                           = -1606245,
	SAY_ALARM_TIMER_6                           = -1606246,
	SAY_ALARM_TIMER_5                           = -1606247,
	SAY_ALARM_TIMER_4                           = -1606248,
	SAY_ALARM_TIMER_3                           = -1606249,
	SAY_ALARM_TIMER_2                           = -1606250,
	SAY_ALARM_TIMER_1                           = -1606251,
	SAY_ALARM_TIMER_TIME_UP                     = -1606252,
	SAY_ALARM_TIMER_CANCEL                      = -1606253, // Need Implement
	EMOTE_PLASMA_BLAST                          = -1606254,
};

enum Spells
{
    // Leviathan MK II
    SPELL_MINES_SPAWN                           = 65347,
    SPELL_PROXIMITY_MINES                       = 63027,
    SPELL_PLASMA_BLAST                          = 62997,
    SPELL_SHOCK_BLAST                           = 63631,
    SPELL_EXPLOSION                             = 66351,
    SPELL_NAPALM_SHELL                          = 63666,
    // VX-001
    SPELL_RAPID_BURST                           = 63382,
    SPELL_RAPID_BURST_LEFT_10                   = 63387,
    SPELL_RAPID_BURST_RIGHT_10                  = 64019,
    SPELL_RAPID_BURST_LEFT_25                   = 64531,
    SPELL_RAPID_BURST_RIGHT_25                  = 64532,
    SPELL_ROCKET_STRIKE                         = 63036,
    SPELL_ROCKET_STRIKE_AURA                    = 64064,
    SPELL_ROCKET_STRIKE_DAMAGE                  = 63041,
    SPELL_SPINNING_UP                           = 63414,
    SPELL_HEAT_WAVE                             = 63677,
    SPELL_HAND_PULSE                            = 64348,
	SPELL_SELF_STUN                             = 14821,
    // Aerial Command Unit
    SPELL_PLASMA_BALL                           = 63689,
    SPELL_MAGNETIC_CORE                         = 64436,
    SPELL_MAGNETIC_CORE_VISUAL                  = 64438,
	SPELL_BOOM_BOT                              = 63767,
	SPELL_BOOM_BOT_PERIODIC                     = 63801,
    SPELL_MAGNETIC_FIELD                        = 64668,
    SPELL_HOVER                                 = 57764,
    SPELL_BERSERK                               = 47008,
    // Hard Mode
    SPELL_SELF_DESTRUCTION                      = 64610,
    SPELL_SELF_DESTRUCTION_VISUAL               = 64613,
    SPELL_EMERGENCY_MODE                        = 64582,
    SPELL_FLAME_SUPPRESSANT                     = 64570,
    SPELL_FLAME_SUPPRESSANT_VX001               = 65192,
    SPELL_SUMMON_FLAMES_INITIAL                 = 64563,
    SPELL_FLAME                                 = 64561,
    SPELL_FROST_BOMB                            = 64624,
    SPELL_FROST_BOMB_EXPLOSION_10               = 64626,
    SPELL_FROST_BOMB_EXPLOSION_25               = 65333,
    SPELL_WATER_SPRAY                           = 64619,
    SPELL_DEAFENING_SIREN                       = 64616,
    // Creatures
    SPELL_BOMB_BOT                              = 63801,
    SPELL_NOT_SO_FRIENDLY_FIRE                  = 65040,
};

enum Events
{
    // Hardmode announcing
	EVENT_1_MIN,
	EVENT_2_MIN,
	EVENT_3_MIN,
	EVENT_4_MIN,
	EVENT_5_MIN,
	EVENT_6_MIN,
	EVENT_7_MIN,
	EVENT_8_MIN,
	EVENT_9_MIN,
    EVENT_TIME_UP,
    EVENT_CANCEL,
    // Leviathan MK II
    EVENT_PROXIMITY_MINE = 1,
    EVENT_NAPALM_SHELL,
    EVENT_PLASMA_BLAST,
    EVENT_SHOCK_BLAST,
    EVENT_FLAME_SUPPRESSANT,
    // VX-001
    EVENT_RAPID_BURST,
    EVENT_LASER_BARRAGE,
    EVENT_LASER_BARRAGE_END,
    EVENT_ROCKET_STRIKE,
    EVENT_HEAT_WAVE,
    EVENT_HAND_PULSE,
    EVENT_FROST_BOMB,
    EVENT_FLAME_SUPPRESSANT_VX001,
    // Aerial Command Unit
    EVENT_PLASMA_BALL,
    EVENT_REACTIVATE_AERIAL,
    EVENT_SUMMON_BOTS,
    // Temporary
    EVENT_RELOCATE,
};

enum Phases
{
    PHASE_NULL,
    PHASE_INTRO,
    PHASE_COMBAT,
    PHASE_LEVIATHAN_SOLO,
    PHASE_LEVIATHAN_ASSEMBLED,
    PHASE_VX001_ACTIVATION,
    PHASE_VX001_SOLO,
    PHASE_VX001_ASSEMBLED,
    PHASE_AERIAL_ACTIVATION,
    PHASE_AERIAL_SOLO,
    PHASE_AERIAL_ASSEMBLED,
    PHASE_V0L7R0N_ACTIVATION,
};

enum Actions
{
    DO_START_ENCOUNTER = 1,
    DO_ACTIVATE_VX001,
    DO_START_VX001,
    DO_ACTIVATE_AERIAL,
    DO_START_AERIAL,
    DO_DISABLE_AERIAL,
    DO_ACTIVATE_V0L7R0N,
    DO_LEVIATHAN_ASSEMBLED,
    DO_VX001_ASSEMBLED,
    DO_AERIAL_ASSEMBLED,
    DO_ACTIVATE_DEATH_TIMER,
    DO_ENTER_ENRAGE,
    DO_ACTIVATE_HARD_MODE,
    DO_INCREASE_FLAME_COUNT,
    DO_DECREASE_FLAME_COUNT,
    DATA_GET_HARD_MODE,
    DATA_FLAME_COUNT,
	DATA_SET_US_UP_THE_BOMB_BOT,
	DATA_SET_US_UP_THE_BOMB_ROCKET,
	DATA_SET_US_UP_THE_BOMB_MINE,
};

Position const SummonPos[9] =
{
    {2703.93f, 2569.32f, 364.397f, 0},
    {2715.33f, 2569.23f, 364.397f, 0},
    {2726.85f, 2569.28f, 364.397f, 0},
    {2765.24f, 2534.38f, 364.397f, 0},
    {2759.54f, 2544.30f, 364.397f, 0},
    {2753.82f, 2554.22f, 364.397f, 0},
    {2764.95f, 2604.11f, 364.397f, 0},
    {2759.19f, 2594.26f, 364.397f, 0},
    {2753.56f, 2584.30f, 364.397f, 0},
};

enum Cap
{
	FLAME_CAP = 154,
};

class boss_mimiron : public CreatureScript
{
    public:
        boss_mimiron() : CreatureScript("boss_mimiron") { }

        struct boss_mimironAI : public BossAI
        {
            boss_mimironAI(Creature* creature) : BossAI(creature, DATA_MIMIRON)
            {
                me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_ROCKET_STRIKE_DAMAGE, true);
                me->SetReactState(REACT_PASSIVE);
            }

			Phases Phase;
            uint32 PhaseTimer;
            uint32 Step;
            uint32 EnrageTimer;
            uint32 FlameTimer;
            uint32 FlameCount;
            uint32 BotTimer;
            uint32 CheckTargetTimer;
            bool MimironHardMode;
            bool CheckBotAlive;
            bool Enraged;
			bool RocketCriteria;
			bool MineCriteria;
			bool BotCriteria;

            void DespawnCreatures(uint32 entry, float distance)
            {
                std::list<Creature*> creatures;
                GetCreatureListWithEntryInGrid(creatures, me, entry, distance);

                if (creatures.empty())
                    return;

                for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
                    (*iter)->DespawnAfterAction();
            }

            void Reset()
            {
                if (me->GetFaction() == FACTION_FRIENDLY)
                    return;

                _Reset();
				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_ONESHOT_USE_STANDING);
                me->SetVisible(true);
                me->ExitVehicle();
                me->GetMotionMaster()->MoveTargetedHome();

                instance->SetData(DATA_MIMIRON_ELEVATOR, GO_STATE_ACTIVE);

                Phase = PHASE_NULL;
                Step = 0;
                PhaseTimer = -1;
                BotTimer = 0;
                FlameCount = 0;
                MimironHardMode = false;
                CheckBotAlive = true;
                Enraged = false;
                CheckTargetTimer = 7000;
				RocketCriteria = true;
				MineCriteria = true;
				BotCriteria = true;

                DespawnCreatures(NPC_FLAMES_INITIAL, 100.0f);
                DespawnCreatures(NPC_PROXIMITY_MINE, 100.0f);
                DespawnCreatures(NPC_ROCKET, 100.0f);
                DespawnCreatures(NPC_JUNK_BOT, 100.0f);
                DespawnCreatures(NPC_ASSAULT_BOT, 100.0f);
                DespawnCreatures(NPC_BOOM_BOT, 100.0f);
                DespawnCreatures(NPC_EMERGENCY_BOT, 100.0f);

                for (uint8 data = DATA_LEVIATHAN_MK_II; data <= DATA_AERIAL_UNIT; ++data)
				{
					if (Creature* creature = me->GetCreature(*me, instance->GetData64(data)))
					{
						if (creature->IsAlive())
                        {
                            creature->ExitVehicle();
                            creature->AI()->EnterEvadeMode();
						}
					}
				}

                if (GameObject* go = me->FindGameobjectWithDistance(GO_BIG_RED_BUTTON, 200.0f))
                {
                    go->SetGoState(GO_STATE_READY);
                    go->SetLootState(GO_JUST_DEACTIVATED);
                    go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                }
            }

			void QuestReward()
			{
				Map::PlayerList const& Players = me->GetMap()->GetPlayers();
				for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
				{
					if (Player* player = itr->getSource())
					{
						if (Is25ManRaid())
						{
							if (player->GetQuestStatus(QUEST_CONQUEROR_OF_ULDUAR_A) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_CONQUEROR_OF_ULDUAR_H) == QUEST_STATUS_INCOMPLETE)
								player->AddItem(ITEM_ID_CORE_OF_MIMIRON, 1);
						}
					}
				}
			}

            void EndEncounter()
            {
                DoSendQuantumText(SAY_V07TRON_DEATH, me);
                me->SetCurrentFaction(FACTION_FRIENDLY);
				me->SummonCreature(NPC_UNFLUENCE_TENTACLE, 2720.34f, 2569.11f, 364.314f, 6.27798f, TEMPSUMMON_TIMED_DESPAWN, 35000);

				instance->SetBossState(DATA_MIMIRON, DONE);

				if (MimironHardMode)
					me->SummonGameObject(RAID_MODE<uint32>(GO_CACHE_OF_INNOVATION_H_10, GO_CACHE_OF_INNOVATION_H_25), 2744.65f, 2569.46f, 364.314f, 3.14159f, 0, 0, 0.7f, 0.7f, 604800);
				else
					me->SummonGameObject(RAID_MODE<uint32>(GO_CACHE_OF_INNOVATION_N_10, GO_CACHE_OF_INNOVATION_N_25), 2744.65f, 2569.46f, 364.314f, 3.14159f, 0, 0, 0.7f, 0.7f, 604800);
				instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, NPC_LEVIATHAN_MKII, 1);

				//QuestReward();
                EnterEvadeMode();
                me->DespawnAfterAction(5*IN_MILLISECONDS);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                _EnterToBattle();

				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                Phase = PHASE_INTRO;
                FlameTimer = 5000;
                EnrageTimer = MimironHardMode ? 10*60*1000 : 15*60*1000; // Enrage in 10 (hard mode) or 15 min
                JumpToNextStep(100);

                if (GameObject* go = me->FindGameobjectWithDistance(GO_BIG_RED_BUTTON, 200))
                    go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
            }

            void JumpToNextStep(uint32 timer)
            {
                PhaseTimer = timer;
                ++Step;
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

            events.Update(diff);

            if (MimironHardMode)
            {
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_9_MIN:
							if (Creature* computer = me->FindCreatureWithDistance(NPC_COMPUTER, 500.0f))
							{
								DoSendQuantumText(SAY_ALARM_TIMER_9, computer);
							}
                            break;
                        case EVENT_8_MIN:
							if (Creature* computer = me->FindCreatureWithDistance(NPC_COMPUTER, 500.0f))
							{
								DoSendQuantumText(SAY_ALARM_TIMER_8, computer);
							}
                            break;
                        case EVENT_7_MIN:
							if (Creature* computer = me->FindCreatureWithDistance(NPC_COMPUTER, 500.0f))
							{
								DoSendQuantumText(SAY_ALARM_TIMER_7, computer);
							}
                            break;
                        case EVENT_6_MIN:
							if (Creature* computer = me->FindCreatureWithDistance(NPC_COMPUTER, 500.0f))
							{
								DoSendQuantumText(SAY_ALARM_TIMER_6, computer);
							}
                            break;
                        case EVENT_5_MIN:
							if (Creature* computer = me->FindCreatureWithDistance(NPC_COMPUTER, 500.0f))
							{
								DoSendQuantumText(SAY_ALARM_TIMER_5, computer);
							}
                            break;
                        case EVENT_4_MIN:
							if (Creature* computer = me->FindCreatureWithDistance(NPC_COMPUTER, 500.0f))
							{
								DoSendQuantumText(SAY_ALARM_TIMER_4, computer);
							}
                            break;
                        case EVENT_3_MIN:
							if (Creature* computer = me->FindCreatureWithDistance(NPC_COMPUTER, 500.0f))
							{
								DoSendQuantumText(SAY_ALARM_TIMER_3, computer);
							}
                            break;
                        case EVENT_2_MIN:
							if (Creature* computer = me->FindCreatureWithDistance(NPC_COMPUTER, 500.0f))
							{
								DoSendQuantumText(SAY_ALARM_TIMER_2, computer);
							}
                            break;
                        case EVENT_1_MIN:
							if (Creature* computer = me->FindCreatureWithDistance(NPC_COMPUTER, 500.0f))
							{
								DoSendQuantumText(SAY_ALARM_TIMER_1, computer);
							}
                            break;
                        case EVENT_TIME_UP:
							if (Creature* computer = me->FindCreatureWithDistance(NPC_COMPUTER, 500.0f))
							{
								DoSendQuantumText(SAY_ALARM_TIMER_TIME_UP, computer);
							}
                            break;
					}
				}
			}

                if (CheckTargetTimer < diff)
                {
                    if (!SelectTarget(TARGET_RANDOM, 0, 200.0f, true))
                    {
                        EnterEvadeMode();
                        return;
                    }
                    CheckTargetTimer = 7000;
                }
                else CheckTargetTimer -= diff;

                if (EnrageTimer <= diff && !Enraged)
                {
                    DoSendQuantumText(SAY_BERSERK, me);
                    for (uint8 data = DATA_LEVIATHAN_MK_II; data <= DATA_AERIAL_UNIT; ++data)
                        if (Creature* creature = me->GetCreature(*me, instance->GetData64(data)))
                            creature->AI()->DoAction(DO_ENTER_ENRAGE);

                    Enraged = true;

                    if (MimironHardMode)
                    {
                        DoCast(me, SPELL_SELF_DESTRUCTION, true);
                        DoCast(me, SPELL_SELF_DESTRUCTION_VISUAL, true);
                    }
                }
                else EnrageTimer -= diff;

                if (MimironHardMode)
				{
                    if (FlameTimer <= diff)
                    {
                        for (uint8 i = 0; i < 3; ++i)
						{
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
								DoCast(target, SPELL_SUMMON_FLAMES_INITIAL, true);
						}
						FlameTimer = 30000;
                    }
					else FlameTimer -= diff;
				}

                // All sections need to die within 15 seconds, else they respawn
                if (CheckBotAlive)
                    BotTimer = 0;
                else
                {
                    BotTimer += diff;
                    if (BotTimer > 15000) // spell 64383
                    {
                        if (Creature* Leviathan = me->GetCreature(*me, instance->GetData64(DATA_LEVIATHAN_MK_II)))
                            Leviathan->AI()->DoAction(DO_LEVIATHAN_ASSEMBLED);
                        if (Creature* VX_001 = me->GetCreature(*me, instance->GetData64(DATA_VX_001)))
                            VX_001->AI()->DoAction(DO_VX001_ASSEMBLED);
                        if (Creature* AerialUnit = me->GetCreature(*me, instance->GetData64(DATA_AERIAL_UNIT)))
                            AerialUnit->AI()->DoAction(DO_AERIAL_ASSEMBLED);

                        CheckBotAlive = true;
                    }
                    else
                    {
                        Creature* Leviathan = me->GetCreature(*me, instance->GetData64(DATA_LEVIATHAN_MK_II));
                        Creature* VX_001 = me->GetCreature(*me, instance->GetData64(DATA_VX_001));
                        Creature* AerialUnit = me->GetCreature(*me, instance->GetData64(DATA_AERIAL_UNIT));
                        if (Leviathan && VX_001 && AerialUnit)
                        {
                            if (Leviathan->getStandState() == UNIT_STAND_STATE_DEAD &&
                                VX_001->getStandState() == UNIT_STAND_STATE_DEAD &&
                                AerialUnit->getStandState() == UNIT_STAND_STATE_DEAD)
                            {
                                Leviathan->DespawnAfterAction(5*MINUTE*IN_MILLISECONDS);
                                VX_001->DisappearAndDie();
                                AerialUnit->DisappearAndDie();
                                me->Kill(Leviathan);
                                DespawnCreatures(NPC_FLAMES_INITIAL, 100.0f);
                                DespawnCreatures(NPC_PROXIMITY_MINE, 100.0f);
                                DespawnCreatures(NPC_ROCKET, 100);
                                me->ExitVehicle();
                                EndEncounter();
                                CheckBotAlive = true;
                            }
                        }
                    }
                }

                if (Phase == PHASE_INTRO)
                {
                    if (PhaseTimer <= diff)
                    {
                        switch (Step)
                        {
                            case 1:
                                if (MimironHardMode)
                                {
									if (Creature* computer = me->FindCreatureWithDistance(NPC_COMPUTER, 500.0f))
									{
										DoSendQuantumText(SAY_ALARM_START, computer);
									}
                                    JumpToNextStep(3000); //2000
                                }
                                else
                                {
                                    JumpToNextStep(1);
                                }
                                break;
                            case 2:
                                if (MimironHardMode)
                                {
                                    DoSendQuantumText(SAY_HARDMODE_ON, me);
                                    JumpToNextStep(15000);
                                }
                                else
                                {
                                    DoSendQuantumText(SAY_AGGRO, me);
                                    JumpToNextStep(10000);
                                }
                                break;
                            case 3:
								if (Creature* Leviathan = me->GetCreature(*me, instance->GetData64(DATA_LEVIATHAN_MK_II)))
								{
									me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
									me->EnterVehicle(Leviathan, 4);
                                }
                                JumpToNextStep(2000);
                                break;
                            case 4:
                                if (MimironHardMode)
                                {
									if (Creature* computer = me->FindCreatureWithDistance(NPC_COMPUTER, 500.0f))
										DoSendQuantumText(SAY_ALARM_TIMER_10, computer);

									me->ChangeSeat(2);
									JumpToNextStep(3000);
                                }
                                else
                                {
                                    me->ChangeSeat(2);
                                    JumpToNextStep(2000);
                                }
                                break;
                            case 5:
                                me->ChangeSeat(5);
                                me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_STAND);
                                JumpToNextStep(3000);
                                break;
                            case 6:
                                DoSendQuantumText(SAY_MKII_ACTIVATE, me);
                                me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_TALK);
                                JumpToNextStep(6000);
                                break;
                            case 7:
                                me->ChangeSeat(6);
                                JumpToNextStep(2000);
                                break;
                            case 8:
								if (Creature* Leviathan = me->GetCreature(*me, instance->GetData64(DATA_LEVIATHAN_MK_II)))
								{
									me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_STAND);
									Leviathan->AI()->DoAction(DO_START_ENCOUNTER);
									Phase = PHASE_COMBAT;
									events.ScheduleEvent(EVENT_9_MIN, 60000);
									events.ScheduleEvent(EVENT_8_MIN, 120000);
									events.ScheduleEvent(EVENT_7_MIN, 180000);
									events.ScheduleEvent(EVENT_6_MIN, 240000);
									events.ScheduleEvent(EVENT_5_MIN, 300000);
									events.ScheduleEvent(EVENT_4_MIN, 360000);
									events.ScheduleEvent(EVENT_3_MIN, 420000);
									events.ScheduleEvent(EVENT_2_MIN, 480000);
									events.ScheduleEvent(EVENT_1_MIN, 540000);
									events.ScheduleEvent(EVENT_TIME_UP, 600000);
								}
                                break;
                            default:
                                break;
                        }
                    }
                    else PhaseTimer -= diff;
                }

                if (Phase == PHASE_VX001_ACTIVATION)
                {
                    if (PhaseTimer <= diff)
                    {
                        switch (Step)
                        {
                            case 1:
                                DoSendQuantumText(SAY_MKII_DEATH, me);
                                JumpToNextStep(10000);
                                break;
                            case 2:
                                me->ChangeSeat(1);
                                JumpToNextStep(2000);
                                break;
                            case 3:
								instance->SetData(DATA_MIMIRON_ELEVATOR, GO_STATE_READY);
                                JumpToNextStep(5000);
                                break;
                            case 4:
								if (Creature* VX_001 = me->SummonCreature(NPC_VX_001, 2744.65f, 2569.46f, 364.397f, 3.14159f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
								{
									instance->SetData(DATA_MIMIRON_ELEVATOR, GO_STATE_ACTIVE_ALTERNATIVE);
									VX_001->SetVisible(true);
									for (uint8 n = 5; n < 7; ++n)
									{
										if (Creature* Rocket = me->SummonCreature(NPC_ROCKET, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
										{
											Rocket->SetCurrentFaction(FACTION_HOSTILE);
											Rocket->SetReactState(REACT_PASSIVE);
											Rocket->EnterVehicle(VX_001, n);
										}
                                    }
                                }
                                JumpToNextStep(8000);
                                break;
                            case 5:
								if (Creature* VX_001 = me->GetCreature(*me, instance->GetData64(DATA_VX_001)))
									me->EnterVehicle(VX_001, 0);

                                JumpToNextStep(3500);
                                break;
                            case 6:
                                me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_TALK);
                                DoSendQuantumText(SAY_VX001_ACTIVATE, me);
                                JumpToNextStep(10000);
                                break;
                            case 7:
                                me->ChangeSeat(1);
                                me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SIT);
                                JumpToNextStep(2000);
                                break;
                            case 8:
								if (Creature* VX_001 = me->GetCreature(*me, instance->GetData64(DATA_VX_001)))
									VX_001->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);

                                JumpToNextStep(3500);
                                break;
                            case 9:
								if (Creature* VX_001 = me->GetCreature(*me, instance->GetData64(DATA_VX_001)))
								{
									VX_001->AddAura(SPELL_HOVER, VX_001); // Hover
									VX_001->AI()->DoAction(DO_START_VX001);
									Phase = PHASE_COMBAT;
								}
                                break;
                            default:
                                break;
                        }
                    }
                    else PhaseTimer -= diff;
                }

                if (Phase == PHASE_AERIAL_ACTIVATION)
                {
                    if (PhaseTimer <= diff)
                    {
                        switch (Step)
                        {
                            case 1:
                                me->ChangeSeat(4);
                                me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_STAND);
                                JumpToNextStep(2500);
                                break;
                            case 2:
                                DoSendQuantumText(SAY_VX001_DEATH, me);
                                me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_TALK);
                                JumpToNextStep(5000);
                                break;
                            case 3:
                                me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_STAND);

								if (Creature* AerialUnit = me->SummonCreature(NPC_AERIAL_COMMAND_UNIT, 2744.65f, 2569.46f, 380, 3.14159f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000))
									AerialUnit->SetVisible(true);

                                JumpToNextStep(5000);
                                break;
                            case 4:
                                me->ExitVehicle();
                                if (Creature* AerialUnit = me->GetCreature(*me, instance->GetData64(DATA_AERIAL_UNIT)))
                                     me->EnterVehicle(AerialUnit, 0);
                                JumpToNextStep(2000);
                                break;
                            case 5:
                                me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_TALK);
                                DoSendQuantumText(SAY_AERIAL_ACTIVATE, me);
                                JumpToNextStep(8000);
                                break;
                            case 6:
                                me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_STAND);

								if (Creature* AerialUnit = me->GetCreature(*me, instance->GetData64(DATA_AERIAL_UNIT)))
								{
									AerialUnit->AI()->DoAction(DO_START_AERIAL);
									Phase = PHASE_COMBAT;
                                }
                                break;
                            default:
                                break;
                        }
                    }
                    else PhaseTimer -= diff;
                }

                if (Phase == PHASE_V0L7R0N_ACTIVATION)
                {
                    if (PhaseTimer <= diff)
                    {
                        switch (Step)
                        {
                            case 1:
								if (Creature* Leviathan = me->GetCreature(*me, instance->GetData64(DATA_LEVIATHAN_MK_II)))
									Leviathan->GetMotionMaster()->MovePoint(0, 2744.65f, 2569.46f, 364.397f);

								if (Creature* VX_001 = me->GetCreature(*me, instance->GetData64(DATA_VX_001)))
								{
									me->EnterVehicle(VX_001, 1);
									me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_STAND);
									DoSendQuantumText(SAY_AERIAL_DEATH, me);
                                }
                                JumpToNextStep(5000);
                                break;
                            case 2:
								if (Creature* VX_001 = me->GetCreature(*me, instance->GetData64(DATA_VX_001)))
								{
									if (Creature* Leviathan = me->GetCreature(*me, instance->GetData64(DATA_LEVIATHAN_MK_II)))
									{
										VX_001->SetStandState(UNIT_STAND_STATE_STAND);
										VX_001->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_CUSTOM_SPELL_01);
										VX_001->EnterVehicle(Leviathan, 7);
									}
								}
                                JumpToNextStep(2000);
                                break;
                            case 3:
								if (Creature* VX_001 = me->GetCreature(*me, instance->GetData64(DATA_VX_001)))
								{
									if (Creature* AerialUnit = me->GetCreature(*me, instance->GetData64(DATA_AERIAL_UNIT)))
									{
										AerialUnit->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
										AerialUnit->SetCanFly(false);
										AerialUnit->EnterVehicle(VX_001, 3);
										DoSendQuantumText(SAY_V07TRON_ACTIVATE, me);
									}
								}
                                JumpToNextStep(10000);
                                break;
                            case 4:
								if (Creature* Leviathan = me->GetCreature(*me, instance->GetData64(DATA_LEVIATHAN_MK_II)))
									Leviathan->AI()->DoAction(DO_LEVIATHAN_ASSEMBLED);

								if (Creature* VX_001 = me->GetCreature(*me, instance->GetData64(DATA_VX_001)))
									VX_001->AI()->DoAction(DO_VX001_ASSEMBLED);

								if (Creature* AerialUnit = me->GetCreature(*me, instance->GetData64(DATA_AERIAL_UNIT)))
									AerialUnit->AI()->DoAction(DO_AERIAL_ASSEMBLED);
								Phase = PHASE_COMBAT;
								break;
                            default:
                                break;
                        }
                    }
                    else PhaseTimer -= diff;
                }
            }

            uint32 GetData(uint32 type)
            {
                switch (type)
                {
                    case DATA_GET_HARD_MODE:
                        return MimironHardMode ? 1 : 0;
                    case DATA_FLAME_COUNT:
                        return FlameCount;
					case DATA_SET_US_UP_THE_BOMB_MINE:
						return MineCriteria;
					case DATA_SET_US_UP_THE_BOMB_ROCKET:
						return RocketCriteria;
					case DATA_SET_US_UP_THE_BOMB_BOT:
						return BotCriteria;
                    default:
                        return 0;
                }
            }

			void SetData(uint32 type, uint32 data)
            {
                switch (type)
                {
                    case DATA_SET_US_UP_THE_BOMB_MINE:
                        MineCriteria = data;
                        break;
                    case DATA_SET_US_UP_THE_BOMB_ROCKET:
                        RocketCriteria = data;
                        break;
                    case DATA_SET_US_UP_THE_BOMB_BOT:
                        BotCriteria = data;
                        break;
                    default:
                        break;
                }
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case DO_ACTIVATE_VX001:
                        Phase = PHASE_VX001_ACTIVATION;
                        Step = 0;
                        PhaseTimer = -1;
                        JumpToNextStep(100);
                        break;
                    case DO_ACTIVATE_AERIAL:
                        Phase = PHASE_AERIAL_ACTIVATION;
                        Step = 0;
                        PhaseTimer = -1;
                        JumpToNextStep(5000);
                        break;
                    case DO_ACTIVATE_V0L7R0N:
                        me->SetVisible(true);
                        Phase = PHASE_V0L7R0N_ACTIVATION;
                        Step = 0;
                        PhaseTimer = -1;
                        JumpToNextStep(1000);
                        break;
                    case DO_ACTIVATE_DEATH_TIMER:
                        CheckBotAlive = false;
                        break;
                    case DO_ACTIVATE_HARD_MODE:
                        MimironHardMode = true;
                        DoZoneInCombat();
                        break;
                    case DO_INCREASE_FLAME_COUNT:
                        ++FlameCount;
                        break;
                    case DO_DECREASE_FLAME_COUNT:
                        if (FlameCount)
                            --FlameCount;
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_mimironAI(creature);
        }
};

class boss_leviathan_mk : public CreatureScript
{
public:
    boss_leviathan_mk() : CreatureScript("boss_leviathan_mk") { }

    struct boss_leviathan_mkAI : public BossAI
    {
        boss_leviathan_mkAI(Creature* creature) : BossAI(creature, DATA_MIMIRON), phase(PHASE_NULL), vehicle(creature->GetVehicleKit())
        {
            me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_ROCKET_STRIKE_DAMAGE, true);
        }

        Vehicle* vehicle;
        Phases phase;
        bool MimironHardMode;

        void Reset()
        {
            events.Reset();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            me->SetStandState(UNIT_STAND_STATE_STAND);
            me->SetReactState(REACT_PASSIVE);
            me->RemoveAllAurasExceptType(SPELL_AURA_CONTROL_VEHICLE);
            phase = PHASE_NULL;
            events.SetPhase(PHASE_NULL);
            MimironHardMode = false;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

            if (Creature* turret = CAST_CRE(me->GetVehicleKit()->GetPassenger(3)))
            {
                turret->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                turret->SetReactState(REACT_PASSIVE);
            }
        }

        void KilledUnit(Unit* victim)
        {
			if (Creature* Mimiron = me->GetCreature(*me, instance->GetData64(DATA_MIMIRON)))
			{
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
				{
					if (phase == PHASE_LEVIATHAN_SOLO)
						DoSendQuantumText(RAND(SAY_MKII_SLAY_1, SAY_MKII_SLAY_2), Mimiron);
					else
						DoSendQuantumText(RAND(SAY_V07TRON_SLAY_1, SAY_V07TRON_SLAY_2), Mimiron);
				}
			}
		}

        void DamageTaken(Unit* /*who*/, uint32 &damage)
        {
            if (phase == PHASE_LEVIATHAN_SOLO)
			{
                if (damage >= me->GetHealth())
                {
                    damage = 0;
                    me->InterruptNonMeleeSpells(true);
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    me->AttackStop();
                    me->SetReactState(REACT_PASSIVE);
                    me->RemoveAllAurasExceptType(SPELL_AURA_CONTROL_VEHICLE);
                    me->SetHealth(me->GetMaxHealth());
                    events.SetPhase(PHASE_NULL);
                    phase = PHASE_NULL;

                    if (Creature* Mimiron = me->GetCreature(*me, instance->GetData64(DATA_MIMIRON)))
                        Mimiron->AI()->DoAction(DO_ACTIVATE_VX001);

                    if (Creature* turret = CAST_CRE(me->GetVehicleKit()->GetPassenger(3)))
						turret->DespawnAfterAction();

                    me->SetSpeed(MOVE_RUN, 1.5f, true);
                    me->GetMotionMaster()->MovePoint(0, 2790.11f, 2595.83f, 364.32f);
                }
			}

            if (phase == PHASE_LEVIATHAN_ASSEMBLED)
			{
                if (damage >= me->GetHealth())
                {
                    damage = 0;
                    me->InterruptNonMeleeSpells(true);
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    me->AttackStop();
                    me->SetReactState(REACT_PASSIVE);
                    me->RemoveAllAurasExceptType(SPELL_AURA_CONTROL_VEHICLE);
                    me->SetHealth(me->GetMaxHealth());
                    me->SetStandState(UNIT_STAND_STATE_DEAD);
                    events.SetPhase(PHASE_NULL);
                    phase = PHASE_NULL;
                    if (Creature* Mimiron = me->GetCreature(*me, instance->GetData64(DATA_MIMIRON)))
                        Mimiron->AI()->DoAction(DO_ACTIVATE_DEATH_TIMER);
                }
			}
        }

        void EnterToBattle(Unit* /*who*/)
        {
            if (Creature* Mimiron = me->GetCreature(*me, instance ? instance->GetData64(DATA_MIMIRON) : 0))
                MimironHardMode = Mimiron->AI()->GetData(DATA_GET_HARD_MODE);

            if (MimironHardMode)
            {
                DoCast(me, SPELL_EMERGENCY_MODE, true);
                events.ScheduleEvent(EVENT_FLAME_SUPPRESSANT, 60000, 0, PHASE_LEVIATHAN_SOLO);
            }

            if (Creature* turret = CAST_CRE(me->GetVehicleKit()->GetPassenger(3)))
            {
                turret->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                turret->SetReactState(REACT_AGGRESSIVE);
                turret->AI()->DoZoneInCombat();
            }

            events.ScheduleEvent(EVENT_PROXIMITY_MINE, 1000);
            events.ScheduleEvent(EVENT_PLASMA_BLAST, 10000, 0, PHASE_LEVIATHAN_SOLO);
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
                case DO_START_ENCOUNTER:
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_CREATURE | UNIT_FLAG_NOT_SELECTABLE);
                    me->SetReactState(REACT_AGGRESSIVE);
                    phase = PHASE_LEVIATHAN_SOLO;
                    events.SetPhase(PHASE_LEVIATHAN_SOLO);
                    DoZoneInCombat();
                    break;
                case DO_LEVIATHAN_ASSEMBLED:
                    if (MimironHardMode)
                        DoCast(me, SPELL_EMERGENCY_MODE, true);
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->SetHealth(int32(me->GetMaxHealth() / 2));
                    me->SetSpeed(MOVE_RUN, 1.0f, true);
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    phase = PHASE_LEVIATHAN_ASSEMBLED;
                    events.SetPhase(PHASE_LEVIATHAN_ASSEMBLED);
                    events.RescheduleEvent(EVENT_PROXIMITY_MINE, 1000);
                    events.RescheduleEvent(EVENT_SHOCK_BLAST, 30000);
                    events.ScheduleEvent(EVENT_RELOCATE, 1000);
                    break;
                case DO_ENTER_ENRAGE:
                    DoCast(me, SPELL_BERSERK, true);
                    break;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING) || me->HasUnitState(UNIT_STATE_STUNNED))
                return;

            if (phase == PHASE_LEVIATHAN_SOLO || phase == PHASE_LEVIATHAN_ASSEMBLED)
            {
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_RELOCATE:
							me->GetVehicleKit()->RelocatePassengers();
                            events.ScheduleEvent(EVENT_RELOCATE, 1000);
                            break;
                        case EVENT_PROXIMITY_MINE:
                            for (int i = 0; i < 10; ++i)
							{
                                DoCast(SPELL_MINES_SPAWN);
							}
                            DoCast(SPELL_PROXIMITY_MINES);
                            events.RescheduleEvent(EVENT_PROXIMITY_MINE, 35000);
                            break;
                        case EVENT_PLASMA_BLAST:
                            DoSendQuantumText(EMOTE_PLASMA_BLAST, me);
                            DoCast(SPELL_PLASMA_BLAST);
                            events.RescheduleEvent(EVENT_PLASMA_BLAST, urand(30000, 35000), 0, PHASE_LEVIATHAN_SOLO);
                            events.RescheduleEvent(EVENT_SHOCK_BLAST, urand(6000, 10000));
                            return;
                        case EVENT_SHOCK_BLAST:
                            DoCastAOE(SPELL_SHOCK_BLAST);
                            events.RescheduleEvent(EVENT_SHOCK_BLAST, 35000);
                            return;
                        case EVENT_FLAME_SUPPRESSANT:
                            DoCastAOE(SPELL_FLAME_SUPPRESSANT);
                            events.CancelEvent(EVENT_FLAME_SUPPRESSANT);
                            break;
                    }
                }
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_leviathan_mkAI(creature);
    }
};

class boss_leviathan_mk_turret : public CreatureScript
{
    public:
        boss_leviathan_mk_turret() : CreatureScript("boss_leviathan_mk_turret") { }

        struct boss_leviathan_mk_turretAI : public QuantumBasicAI
        {
            boss_leviathan_mk_turretAI(Creature* creature) : QuantumBasicAI(creature)
            {
                SetImmuneToPushPullEffects(true);
                me->SetReactState(REACT_PASSIVE);
                NapalmShell = urand(8000, 12000);
            }

            // try to prefer ranged targets
            Unit* GetNapalmShellTarget()
            {
                Map* map = me->GetMap();
                if (map && map->IsDungeon())
                {
                    std::list<Player*> playerList;
                    Map::PlayerList const& Players = map->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
                    {
                        if (Player* player = itr->getSource())
                        {
                            if (player->IsDead() || player->IsGameMaster())
                                continue;

                            float Distance = player->GetDistance(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
							if (Distance < 15.0f || Distance > 100.0f)
                                continue;

                            playerList.push_back(player);
                        }
                    }

                    if (playerList.empty())
                        return NULL;

					return Quantum::DataPackets::SelectRandomContainerElement(playerList);
                }
                else return NULL;
            }

			uint32 NapalmShell;

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (NapalmShell <= diff)
                {
                    Unit* shellTarget = GetNapalmShellTarget();
                    if (!shellTarget)
                        shellTarget = SelectTarget(TARGET_RANDOM, 0, 100.0f, true);
                    if (shellTarget)
                        DoCast(shellTarget, SPELL_NAPALM_SHELL);
                    NapalmShell = urand(8000, 12000);
                }
                else NapalmShell -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_leviathan_mk_turretAI(creature);
        }
};

class npc_proximity_mine : public CreatureScript
{
public:
    npc_proximity_mine() : CreatureScript("npc_proximity_mine") { }

    struct npc_proximity_mineAI : public QuantumBasicAI
    {
        npc_proximity_mineAI(Creature* creature) : QuantumBasicAI(creature)
        {
			SetCombatMovement(false);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
			BoomTimer = 35000;
			Boom = false;
        }

        uint32 BoomTimer;
        bool Boom;

        void MoveInLineOfSight(Unit* who)
        {
            if (!Boom && me->IsWithinDistInMap(who, 0.5f) && who->ToPlayer() && !who->ToPlayer()->IsGameMaster())
            {
                DoCast(SPELL_EXPLOSION);
                me->DespawnAfterAction(1*IN_MILLISECONDS);
                Boom = true;
            }
        }

		void SpellHitTarget(Unit* target, SpellInfo const* spell)
		{
			if (target->GetTypeId() == TYPE_ID_PLAYER && spell->Id == SPELL_EXPLOSION)
			{
				if (Creature* Mimiron = me->FindCreatureWithDistance(NPC_MIMIRON, 200.0f))
					Mimiron->AI()->SetData(DATA_SET_US_UP_THE_BOMB_MINE, false);
			}
		}

        void UpdateAI(const uint32 diff)
        {
            if (BoomTimer <= diff && !Boom)
            {
				DoCast(SPELL_EXPLOSION);
				me->DespawnAfterAction(1*IN_MILLISECONDS);
				Boom = true;
            }
            else BoomTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_proximity_mineAI(creature);
    }
};

class boss_vx_001 : public CreatureScript
{
public:
    boss_vx_001() : CreatureScript("boss_vx_001") { }

    struct boss_vx_001AI : public BossAI
    {
        boss_vx_001AI(Creature* creature) : BossAI(creature, DATA_MIMIRON), phase(PHASE_NULL)
        {
            me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_ROCKET_STRIKE_DAMAGE, true);
        }

        Phases phase;

        bool MimironHardMode;
        bool Spinning;
        bool Direction;

        uint32 SpinTimer;

        void Reset()
        {
            Spinning = false;
            Direction = false;
            SpinTimer = 250;

            events.Reset();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_DISABLE_MOVE);
            me->SetStandState(UNIT_STAND_STATE_STAND);
            me->SetVisible(false);
            me->RemoveAllAurasExceptType(SPELL_AURA_CONTROL_VEHICLE);
            phase = PHASE_NULL;
            events.SetPhase(PHASE_NULL);
            MimironHardMode = false;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

            me->ApplySpellImmune(0, IMMUNITY_ID, 48181, true);
            me->ApplySpellImmune(0, IMMUNITY_ID, 59161, true);
            me->ApplySpellImmune(0, IMMUNITY_ID, 59163, true);
            me->ApplySpellImmune(0, IMMUNITY_ID, 59164, true);
        }

        void KilledUnit(Unit* victim)
        {
			if (Creature* Mimiron = me->GetCreature(*me, instance->GetData64(DATA_MIMIRON)))
			{
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
				{
					if (phase == PHASE_VX001_SOLO)
						DoSendQuantumText(RAND(SAY_VX001_SLAY_1, SAY_VX001_SLAY_2), Mimiron);
					else
						DoSendQuantumText(RAND(SAY_V07TRON_SLAY_1, SAY_V07TRON_SLAY_2), Mimiron);
				}
			}
		}

        void EnterToBattle(Unit* /*who*/)
        {
            if (Creature* Mimiron = me->GetCreature(*me, instance ? instance->GetData64(DATA_MIMIRON) : 0))
                MimironHardMode = Mimiron->AI()->GetData(DATA_GET_HARD_MODE);

            if (MimironHardMode)
            {
                DoCast(me, SPELL_EMERGENCY_MODE, true);
                events.ScheduleEvent(EVENT_FROST_BOMB, 15000);
                events.ScheduleEvent(EVENT_FLAME_SUPPRESSANT_VX001, 1000, 0, PHASE_VX001_SOLO);
            }

            events.ScheduleEvent(EVENT_RAPID_BURST, 2500, 0, PHASE_VX001_SOLO);
            events.ScheduleEvent(EVENT_LASER_BARRAGE, urand(35000, 40000));
            events.ScheduleEvent(EVENT_ROCKET_STRIKE, 20000);
            events.ScheduleEvent(EVENT_HEAT_WAVE, urand(8000, 10000), 0, PHASE_VX001_SOLO);
            events.ScheduleEvent(EVENT_HAND_PULSE, 10000, 0, PHASE_VX001_ASSEMBLED);
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
                case DO_START_VX001:
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                    phase = PHASE_VX001_SOLO;
                    events.SetPhase(PHASE_VX001_SOLO);
                    DoZoneInCombat();
                    break;
                case DO_VX001_ASSEMBLED:
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    me->SetHealth(int32(me->GetMaxHealth() / 2));
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    phase = PHASE_VX001_ASSEMBLED;
                    events.SetPhase(PHASE_VX001_ASSEMBLED);

					if (Creature* Rocket = me->SummonCreature(NPC_ROCKET, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN))
					{
						Rocket->SetCurrentFaction(FACTION_HOSTILE);
						Rocket->SetReactState(REACT_PASSIVE);
						Rocket->EnterVehicle(me, 6);
					}

                    events.RescheduleEvent(EVENT_LASER_BARRAGE, urand(35000, 40000));
                    events.RescheduleEvent(EVENT_ROCKET_STRIKE, 20000);
                    events.RescheduleEvent(EVENT_HAND_PULSE, 15000, 0, PHASE_VX001_ASSEMBLED);
                    events.ScheduleEvent(EVENT_RELOCATE, 1000);
                    if (MimironHardMode)
                    {
                        DoCast(me, SPELL_EMERGENCY_MODE, true);
                        events.RescheduleEvent(EVENT_FROST_BOMB, 15000);
                    }
                    break;
                case DO_ENTER_ENRAGE:
                    DoCast(me, SPELL_BERSERK, true);
                    break;
            }
        }

        void DamageTaken(Unit* /*who*/, uint32 &damage)
        {
            if (phase == PHASE_VX001_SOLO)
			{
                if (damage >= me->GetHealth())
                {
                    damage = 0;
                    Spinning = false;
                    me->InterruptNonMeleeSpells(true);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    me->AttackStop();
                    me->GetMotionMaster()->Initialize();
                    me->RemoveAllAurasExceptType(SPELL_AURA_CONTROL_VEHICLE);
                    me->SetHealth(me->GetMaxHealth());
                    me->SetStandState(UNIT_STAND_STATE_DEAD);
                    phase = PHASE_NULL;
                    events.SetPhase(PHASE_NULL);

                    if (Creature* Mimiron = me->GetCreature(*me, instance->GetData64(DATA_MIMIRON)))
                        Mimiron->AI()->DoAction(DO_ACTIVATE_AERIAL);
                }
			}

            if (phase == PHASE_VX001_ASSEMBLED)
			{
                if (damage >= me->GetHealth())
                {
                    damage = 0;
                    Spinning = false;
                    me->InterruptNonMeleeSpells(true);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    me->AttackStop();
                    me->RemoveAllAurasExceptType(SPELL_AURA_CONTROL_VEHICLE);
                    me->SetHealth(me->GetMaxHealth());
                    me->SetStandState(UNIT_STAND_STATE_DEAD);
                    events.SetPhase(PHASE_NULL);
                    phase = PHASE_NULL;

                    if (Creature* Mimiron = me->GetCreature(*me, instance->GetData64(DATA_MIMIRON)))
						Mimiron->AI()->DoAction(DO_ACTIVATE_DEATH_TIMER);
                }
			}
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            if (Spinning)
            {
                if (SpinTimer <= diff)
                {
                    float orient = me->GetOrientation();
                    orient += Direction ? M_PI/60 : -M_PI/60;

                    if (Creature* leviathan = me->GetVehicleCreatureBase())
                    {
                        leviathan->SetFacingTo(orient);
                        me->SetOrientation(orient);
                    }
                    else
                        me->SetFacingTo(orient);

                    float x, y;
                    me->GetNearPoint2D(x, y, 10.0f, me->GetOrientation());
                    if (Creature* creature = me->SummonCreature(NPC_BURST_TARGET, x, y, me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 500))
                        me->SetTarget(creature->GetGUID());

                    SpinTimer = 250;
                }
                else SpinTimer -= diff;
            }

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (phase == PHASE_VX001_SOLO || phase == PHASE_VX001_ASSEMBLED)
            {
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_RELOCATE:
							me->GetVehicleKit()->RelocatePassengers();
                            events.ScheduleEvent(EVENT_RELOCATE, 1000);
                            break;
                        case EVENT_RAPID_BURST:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
							{
                                if (Creature* BurstTarget = me->SummonCreature(NPC_BURST_TARGET, *target, TEMPSUMMON_TIMED_DESPAWN, 3100))
                                    DoCast(BurstTarget, SPELL_RAPID_BURST);
							}
                            events.RescheduleEvent(EVENT_RAPID_BURST, 5000, 0, PHASE_VX001_SOLO);
                            break;
                        case EVENT_LASER_BARRAGE:
                            me->SetReactState(REACT_PASSIVE);
                            if (Creature* leviathan = me->GetVehicleCreatureBase())
                            {
                                float orient = float(2*M_PI * rand_norm());
                                leviathan->CastSpell(leviathan, SPELL_SELF_STUN, true); // temporary
                                leviathan->SetFacingTo(orient);
                                leviathan->SetOrientation(orient);
                                me->SetOrientation(orient);
                            }
                            Direction = urand(0, 1);
                            Spinning = true;
                            DoCast(SPELL_SPINNING_UP);
                            events.DelayEvents(14500);
                            events.RescheduleEvent(EVENT_LASER_BARRAGE, 60000);
                            events.RescheduleEvent(EVENT_LASER_BARRAGE_END, 14000);
                            break;
                        case EVENT_LASER_BARRAGE_END:
                            me->SetReactState(REACT_AGGRESSIVE);
                            if (me->GetVictim())
                                AttackStart(me->GetVictim());
                            Spinning = false;
                            break;
                        case EVENT_ROCKET_STRIKE:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
							{
                                if (Unit* missile = me->GetVehicleKit()->GetPassenger(5))
                                    missile->CastSpell(target, SPELL_ROCKET_STRIKE, true);
							}

                            if (phase == PHASE_VX001_ASSEMBLED)
							{
                                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
								{
                                    if (Unit* missile = me->GetVehicleKit()->GetPassenger(6))
                                        missile->CastSpell(target, SPELL_ROCKET_STRIKE, true);
								}
							}
                            events.RescheduleEvent(EVENT_ROCKET_STRIKE, urand(20000, 25000));
                            break;
                        case EVENT_HEAT_WAVE:
                            DoCastAOE(SPELL_HEAT_WAVE);
                            events.RescheduleEvent(EVENT_HEAT_WAVE, 10000, 0, PHASE_VX001_SOLO);
                            break;
                        case EVENT_HAND_PULSE:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
							{
                                DoCast(target, SPELL_HAND_PULSE);
							}
                            events.RescheduleEvent(EVENT_HAND_PULSE, urand(3000, 4000));
                            break;
                        case EVENT_FROST_BOMB:
                        {
                            std::list<Creature*> flames;
                            me->GetCreatureListWithEntryInGrid(flames, NPC_FLAME_SPREAD, 150.0f);
                            if (!flames.empty())
                            {
                                if (Creature* flame = Quantum::DataPackets::SelectRandomContainerElement(flames))
                                    me->SummonCreature(NPC_FROST_BOMB, *flame, TEMPSUMMON_TIMED_DESPAWN, 11000);
                            }
                            else
								me->SummonCreature(NPC_FROST_BOMB, SummonPos[rand()%9], TEMPSUMMON_TIMED_DESPAWN, 11000);

                            events.RescheduleEvent(EVENT_FROST_BOMB, 45000);
                            break;
                        }
                        case EVENT_FLAME_SUPPRESSANT_VX001:
                            DoCastAOE(SPELL_FLAME_SUPPRESSANT_VX001);
                            events.RescheduleEvent(EVENT_FLAME_SUPPRESSANT_VX001, 10000, 0, PHASE_VX001_SOLO);
                            break;
                    }
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_vx_001AI(creature);
    }
};

class npc_rocket_strike : public CreatureScript
{
public:
    npc_rocket_strike() : CreatureScript("npc_rocket_strike") { }

    struct npc_rocket_strikeAI : public QuantumBasicAI
    {
        npc_rocket_strikeAI(Creature* creature) : QuantumBasicAI(creature)
        {
			SetCombatMovement(false);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
            me->DespawnAfterAction(10*IN_MILLISECONDS);
            DoCast(me, SPELL_ROCKET_STRIKE_AURA);
        }

		void SpellHitTarget(Unit* target, SpellInfo const* spell)
		{
			if (target->GetTypeId() == TYPE_ID_PLAYER && spell->Id == SPELL_ROCKET_STRIKE_DAMAGE)
			{
				if (Creature* Mimiron = me->FindCreatureWithDistance(NPC_MIMIRON, 200.0f))
					Mimiron->AI()->SetData(DATA_SET_US_UP_THE_BOMB_ROCKET, false);
			}
		}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_rocket_strikeAI(creature);
    }
};

class spell_rapid_burst : public SpellScriptLoader
{
    public:
        spell_rapid_burst() : SpellScriptLoader("spell_rapid_burst") { }

        class spell_rapid_burst_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rapid_burst_AuraScript);

            void HandleDummyTick(AuraEffect const* aurEff)
            {
                if (!GetTarget())
                    return;

                if (Unit* caster = GetCaster())
                {
                    switch (caster->GetMap()->GetDifficulty())
                    {
                        case RAID_DIFFICULTY_10MAN_NORMAL:
                            caster->CastSpell(GetTarget(), RAND(SPELL_RAPID_BURST_LEFT_10, SPELL_RAPID_BURST_RIGHT_10), true, NULL, aurEff);
                            break;
                        case RAID_DIFFICULTY_25MAN_NORMAL:
                            caster->CastSpell(GetTarget(), RAND(SPELL_RAPID_BURST_LEFT_25, SPELL_RAPID_BURST_RIGHT_25), true, NULL, aurEff);
                            break;
                        default:
                            break;
                    }
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_rapid_burst_AuraScript::HandleDummyTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_rapid_burst_AuraScript();
        }
};

class boss_aerial_command_unit : public CreatureScript
{
public:
    boss_aerial_command_unit() : CreatureScript("boss_aerial_command_unit") { }

    struct boss_aerial_command_unitAI : public BossAI
    {
        boss_aerial_command_unitAI(Creature* creature) : BossAI(creature, DATA_MIMIRON), phase(PHASE_NULL)
        {
            me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_ROCKET_STRIKE_DAMAGE, true);
			me->SetUnitMovementFlags(MOVEMENTFLAG_FLYING);
        }

        Phases phase;
        bool MimironHardMode;
        uint8 spawnedAdds;

        void Reset()
        {
            events.Reset();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            me->SetReactState(REACT_PASSIVE);
            me->SetStandState(UNIT_STAND_STATE_STAND);
            me->SetVisible(false);
            me->RemoveAllAurasExceptType(SPELL_AURA_CONTROL_VEHICLE);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
            me->SetCanFly(true);
            phase = PHASE_NULL;
            events.SetPhase(PHASE_NULL);
            summons.DespawnAll();
            spawnedAdds = 0;
            MimironHardMode = false;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
        }

        void KilledUnit(Unit* victim)
        {
			if (Creature* Mimiron = me->GetCreature(*me, instance->GetData64(DATA_MIMIRON)))
			{
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
				{
					if (phase == PHASE_AERIAL_SOLO)
						DoSendQuantumText(RAND(SAY_AERIAL_SLAY_1, SAY_AERIAL_SLAY_2), Mimiron);
					else
						DoSendQuantumText(RAND(SAY_V07TRON_SLAY_1, SAY_V07TRON_SLAY_2), Mimiron);
				}
			}
		}

        void EnterToBattle(Unit* /*who*/)
        {
            if (Creature* Mimiron = me->GetCreature(*me, instance ? instance->GetData64(DATA_MIMIRON) : 0))
                MimironHardMode = Mimiron->AI()->GetData(DATA_GET_HARD_MODE);

            if (MimironHardMode)
                DoCast(me, SPELL_EMERGENCY_MODE, true);

            events.ScheduleEvent(EVENT_PLASMA_BALL, 1000);
            events.ScheduleEvent(EVENT_SUMMON_BOTS, 10000, 0, PHASE_AERIAL_SOLO);
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
                case DO_START_AERIAL:
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                    me->SetReactState(REACT_AGGRESSIVE);
                    phase = PHASE_AERIAL_SOLO;
                    events.SetPhase(PHASE_AERIAL_SOLO);
                    DoZoneInCombat();
                    break;
                case DO_DISABLE_AERIAL:
                    if (phase == PHASE_AERIAL_SOLO)
                    {
                        me->CastStop();
                        me->SetReactState(REACT_PASSIVE);
                        me->GetMotionMaster()->Clear(true);
                        DoCast(me, SPELL_MAGNETIC_CORE);
                        DoCast(me, SPELL_MAGNETIC_CORE_VISUAL);
                        me->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), 368.965f, 0.0f);
                        events.RescheduleEvent(EVENT_PLASMA_BALL, 22000, 0, PHASE_AERIAL_SOLO);
                        events.RescheduleEvent(EVENT_SUMMON_BOTS, 24000, 0, PHASE_AERIAL_SOLO);
                        events.ScheduleEvent(EVENT_REACTIVATE_AERIAL, 20000, 0, PHASE_AERIAL_SOLO);
                    }
                    break;
                case DO_AERIAL_ASSEMBLED:
                    if (MimironHardMode)
                        DoCast(me, SPELL_EMERGENCY_MODE, true);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->SetHealth(int32(me->GetMaxHealth() / 2));
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    phase = PHASE_AERIAL_ASSEMBLED;
                    events.SetPhase(PHASE_AERIAL_ASSEMBLED);
                    events.RescheduleEvent(EVENT_PLASMA_BALL, 2000);
                    break;
                case DO_ENTER_ENRAGE:
                    DoCast(me, SPELL_BERSERK, true);
                    break;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (phase == PHASE_AERIAL_SOLO || phase == PHASE_AERIAL_ASSEMBLED)
            {
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PLASMA_BALL:
                            if (phase == PHASE_AERIAL_SOLO && me->GetVictim())
                            {
                                float x = me->GetVictim()->GetPositionX();
                                float y = me->GetVictim()->GetPositionY();
                                float z = me->GetVictim()->GetPositionZ();
                                if (me->IsWithinDist3d(x, y, z, 30))
                                {
                                    me->GetMotionMaster()->Initialize();
                                    DoCastVictim(SPELL_PLASMA_BALL);
                                }
                                else me->GetMotionMaster()->MovePoint(0, x, y, 380.04f);
                            }
                            else if (phase == PHASE_AERIAL_ASSEMBLED && me->GetVictim())
                            {
                                if (me->GetVictim()->IsWithinDist3d(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 30))
                                    DoCastVictim(SPELL_PLASMA_BALL);

                                else if (Unit* target = SelectTarget(TARGET_NEAREST, 0))
                                    DoCast(target, SPELL_PLASMA_BALL);
                            }
                            events.RescheduleEvent(EVENT_PLASMA_BALL, 2000);
                            break;
                        case EVENT_REACTIVATE_AERIAL:
                            me->RemoveAurasDueToSpell(SPELL_MAGNETIC_CORE_VISUAL);
                            me->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), 380.04f, 3.14159f, false);
                            me->SetReactState(REACT_AGGRESSIVE);
                            events.CancelEvent(EVENT_REACTIVATE_AERIAL);
                            break;
                        case EVENT_SUMMON_BOTS:
                            if (phase == PHASE_AERIAL_SOLO)
                            {
                                spawnAdd();
                                events.RescheduleEvent(EVENT_SUMMON_BOTS, 10000, 0, PHASE_AERIAL_SOLO);
                            }
                            break;
                    }
                }
            }
        }

        void spawnAdd()
        {
            switch (spawnedAdds)
            {
                case 0:
                    for (uint8 n = 0; n < 2; n++)
                        me->SummonCreature(NPC_JUNK_BOT, SummonPos[rand()%9], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);
                    break;
                case 1:
                    me->SummonCreature(NPC_ASSAULT_BOT, SummonPos[rand()%9], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                    if (MimironHardMode)
                        for (uint8 i = 0; i < 2; ++i)
                            me->SummonCreature(NPC_EMERGENCY_BOT, SummonPos[rand()%9], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);
                    break;
                case 2:
                    me->SummonCreature(NPC_BOOM_BOT, 2744.65f, 2569.46f, 364.397f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);
                    break;
            }

            ++spawnedAdds;
            if (spawnedAdds > 2)
                spawnedAdds = 0;
        }

        void JustSummoned(Creature* summon)
        {
            summons.Summon(summon);

            if (summon->GetEntry() == NPC_EMERGENCY_BOT)
                return;

            summon->AI()->DoZoneInCombat();

            if (MimironHardMode)
                summon->CastSpell(summon, SPELL_EMERGENCY_MODE, true);
        }

        void DamageTaken(Unit* /*who*/, uint32 &damage)
        {
            if (phase == PHASE_AERIAL_SOLO)
			{
                if (damage >= me->GetHealth())
                {
                    damage = 0;
                    me->InterruptNonMeleeSpells(true);
                    me->GetMotionMaster()->Clear(true);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    me->SetReactState(REACT_PASSIVE);
                    me->AttackStop();
                    me->RemoveAllAurasExceptType(SPELL_AURA_CONTROL_VEHICLE);
                    me->SetHealth(me->GetMaxHealth());
                    events.CancelEvent(EVENT_SUMMON_BOTS);
                    phase = PHASE_NULL;
                    events.SetPhase(PHASE_NULL);

                    if (Creature* Mimiron = me->GetCreature(*me, instance->GetData64(DATA_MIMIRON)))
                        Mimiron->AI()->DoAction(DO_ACTIVATE_V0L7R0N);
                }
			}

            if (phase == PHASE_AERIAL_ASSEMBLED)
			{
                if (damage >= me->GetHealth())
                {
                    damage = 0;
                    me->InterruptNonMeleeSpells(true);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    me->AttackStop();
                    me->SetReactState(REACT_PASSIVE);
                    me->RemoveAllAurasExceptType(SPELL_AURA_CONTROL_VEHICLE);
                    me->SetHealth(me->GetMaxHealth());
                    me->SetStandState(UNIT_STAND_STATE_DEAD);
                    events.SetPhase(PHASE_NULL);
                    phase = PHASE_NULL;

                    if (Creature* Mimiron = me->GetCreature(*me, instance->GetData64(DATA_MIMIRON)))
                        Mimiron->AI()->DoAction(DO_ACTIVATE_DEATH_TIMER);
                }
			}
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_aerial_command_unitAI(creature);
    }
};

class npc_magnetic_core : public CreatureScript
{
public:
	npc_magnetic_core() : CreatureScript("npc_magnetic_core") { }

	struct npc_magnetic_coreAI : public QuantumBasicAI
	{
		npc_magnetic_coreAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
			me->DespawnAfterAction(21*IN_MILLISECONDS);

			if (Creature* AerialUnit = me->FindCreatureWithDistance(NPC_AERIAL_COMMAND_UNIT, 100.0f, true))
			{
				AerialUnit->AI()->DoAction(DO_DISABLE_AERIAL);
				me->GetMotionMaster()->MoveFall();
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_magnetic_coreAI(creature);
	}
};

class npc_assault_bot : public CreatureScript
{
public:
	npc_assault_bot() : CreatureScript("npc_assault_bot") { }

	struct npc_assault_botAI : public QuantumBasicAI
	{
		npc_assault_botAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 FieldTimer;

		void Reset()
		{
			FieldTimer = urand(4000, 6000);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (FieldTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MAGNETIC_FIELD);
					FieldTimer = urand(15000, 20000);
				}
			}
			else FieldTimer -= diff;

			DoMeleeAttackIfReady();
		}

		void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_ROCKET_STRIKE_DAMAGE)
			{
				Map::PlayerList const& Players = me->GetMap()->GetPlayers();
				for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
				{
					if (Player* player = itr->getSource())
						player->CastSpell(player, SPELL_NOT_SO_FRIENDLY_FIRE, true);
				}
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_assault_botAI(creature);
	}
};

class npc_emergency_bot : public CreatureScript
{
public:
	npc_emergency_bot() : CreatureScript("npc_emergency_bot") { }

	struct npc_emergency_botAI : public QuantumBasicAI
	{
		npc_emergency_botAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->SetCurrentFaction(FACTION_HOSTILE);
			SetImmuneToPushPullEffects(true);
			me->SetReactState(REACT_PASSIVE);
			me->GetMotionMaster()->MoveRandom(15);
			SprayTimer = 5000;

			if (Is25ManRaid())
				DoCast(me, SPELL_DEAFENING_SIREN, true);
		}

		uint32 SprayTimer;

		void UpdateAI(uint32 const diff)
		{
			if (SprayTimer <= diff)
			{
				DoCast(SPELL_WATER_SPRAY);
				SprayTimer = 10000;
			}
			else SprayTimer -= diff;
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_emergency_botAI(creature);
	}
};

class npc_mimiron_bomb_bot : public CreatureScript
{
    public:
        npc_mimiron_bomb_bot() : CreatureScript("npc_mimiron_bomb_bot") { }

        struct npc_mimiron_bomb_botAI : public QuantumBasicAI
        {
            npc_mimiron_bomb_botAI(Creature* creature) : QuantumBasicAI(creature) { }

            Unit* SelectPlayerTargetInRange(float range)
            {
                Player* target = NULL;
                Trinity::AnyPlayerInObjectRangeCheck check(me, range, true);
                Trinity::PlayerSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(me, target, check);
                me->VisitNearbyObject(range, searcher);
                return target;
            }

			bool Despawn;

            void Reset()
            {
				Despawn = false;
                if (Unit* target = SelectPlayerTargetInRange(100.0f))
                {
                    me->AddThreat(target, 999999.0f);
                    AttackStart(target);
                }
            }

			void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
			{
				if (spell->Id == SPELL_BOOM_BOT_PERIODIC)
					me->DespawnAfterAction(1*IN_MILLISECONDS);
			}

            void JustDied(Unit* /*killer*/)
            {
                DoCast(me, SPELL_BOMB_BOT, true);
            }

			void SpellHitTarget(Unit* target, SpellInfo const* spell)
			{
				if (target->GetTypeId() == TYPE_ID_PLAYER && spell->Id == SPELL_BOMB_BOT)
				{
					if (Creature* Mimiron = me->FindCreatureWithDistance(NPC_MIMIRON, 200.0f))
						Mimiron->AI()->SetData(DATA_SET_US_UP_THE_BOMB_BOT, false);
				}
			}

            void UpdateAI(uint32 const /*diff*/)
            {
                if (!UpdateVictim())
                    return;

                if (!Despawn && me->IsWithinMeleeRange(me->GetVictim()))
                {
                    Despawn = true;
                    DoCast(me, SPELL_BOMB_BOT, true);
                }
				DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_mimiron_bomb_botAI(creature);
        }
};

class npc_mimiron_flame_trigger : public CreatureScript
{
    public:
        npc_mimiron_flame_trigger() : CreatureScript("npc_mimiron_flame_trigger") { }

        struct npc_mimiron_flame_triggerAI : public QuantumBasicAI
        {
            npc_mimiron_flame_triggerAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
                FlameTimer = 2000;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
                me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                me->SetInCombatWithZone();
            }

			InstanceScript* instance;
            uint32 FlameTimer;

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                switch (spell->Id)
                {
                    case SPELL_FLAME_SUPPRESSANT:
                    case SPELL_FLAME_SUPPRESSANT_VX001:
                    case SPELL_FROST_BOMB_EXPLOSION_10:
                    case SPELL_FROST_BOMB_EXPLOSION_25:
                    case SPELL_WATER_SPRAY:
                        FlameTimer = 1000;
                        me->DespawnAfterAction(0.5*IN_MILLISECONDS);
                        break;
                    default:
                        break;
                }
            }

            void JustSummoned(Creature* /*summon*/)
            {
                if (Creature* mimiron = ObjectAccessor::GetCreature(*me, instance ? instance->GetData64(DATA_MIMIRON) : 0))
                    mimiron->AI()->DoAction(DO_INCREASE_FLAME_COUNT);
            }

            void UpdateAI(uint32 const diff)
            {
                if (FlameTimer <= diff)
                {
                    if (Creature* mimiron = ObjectAccessor::GetCreature(*me, instance ? instance->GetData64(DATA_MIMIRON) : 0))
					{
                        if (mimiron->AI()->GetData(DATA_FLAME_COUNT) >= FLAME_CAP)
                        {
                            me->DespawnAfterAction();
                            return;
                        }
					}

                    DoAttackerAreaInCombat(me, 100.0f);

                    if (Player* nearest = me->SelectNearestPlayer(100.0f))
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveFollow(nearest, 0.0f, 0.0f);
                    }

                    me->SummonCreature(NPC_FLAME_SPREAD, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                    FlameTimer = 4000;
                }
                else FlameTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_mimiron_flame_triggerAI(creature);
        }
};

class npc_mimiron_flame_spread : public CreatureScript
{
    public:
        npc_mimiron_flame_spread() : CreatureScript("npc_mimiron_flame_spread") { }

        struct npc_mimiron_flame_spreadAI : public QuantumBasicAI
        {
            npc_mimiron_flame_spreadAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED | UNIT_FLAG_DISABLE_MOVE);
                me->SetReactState(REACT_PASSIVE);
                DoCast(me, SPELL_FLAME, true);
            }

			InstanceScript* instance;

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                switch (spell->Id)
                {
                    case SPELL_FLAME_SUPPRESSANT:
                    case SPELL_FLAME_SUPPRESSANT_VX001:
                    case SPELL_FROST_BOMB_EXPLOSION_10:
                    case SPELL_FROST_BOMB_EXPLOSION_25:
                    case SPELL_WATER_SPRAY:
                        if (Creature* mimiron = ObjectAccessor::GetCreature(*me, instance ? instance->GetData64(DATA_MIMIRON) : 0))
                            mimiron->AI()->DoAction(DO_DECREASE_FLAME_COUNT);

                        me->DespawnAfterAction(0.5*IN_MILLISECONDS);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const /*diff*/)
            {
                if (instance->GetBossState(DATA_MIMIRON) != IN_PROGRESS)
                    me->DespawnAfterAction();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_mimiron_flame_spreadAI(creature);
        }
};

class npc_frost_bomb : public CreatureScript
{
public:
	npc_frost_bomb() : CreatureScript("npc_frost_bomb") { }

	struct npc_frost_bombAI : public QuantumBasicAI
	{
		npc_frost_bombAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
			me->SetReactState(REACT_PASSIVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
			DoCast(me, SPELL_FROST_BOMB, true);
			FrostTimer = 10000;
		}

		uint32 FrostTimer;

		void UpdateAI(uint32 const diff)
		{
			if (FrostTimer <= diff)
			{
				DoCast(me, RAID_MODE(SPELL_FROST_BOMB_EXPLOSION_10, SPELL_FROST_BOMB_EXPLOSION_25), true);
				FrostTimer = 10000;
			}
			else FrostTimer -= diff;
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_frost_bombAI(creature);
	}
};

class go_not_push_button : public GameObjectScript
{
public:
    go_not_push_button() : GameObjectScript("go_not_push_button") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        InstanceScript* instance = go->GetInstanceScript();

        if (!instance)
            return false;

        if ((instance->GetBossState(DATA_MIMIRON) != IN_PROGRESS || instance->GetBossState(DATA_MIMIRON) != DONE) && player)
		{
            if (Creature* mimiron = player->GetCreature((*player), instance->GetData64(DATA_MIMIRON)))
                mimiron->AI()->DoAction(DO_ACTIVATE_HARD_MODE);
		}

        go->UseDoorOrButton();

        return true;
    }
};

class go_call_tram : public GameObjectScript
{
public:
    go_call_tram() : GameObjectScript("go_call_tram") { }

    bool OnGossipHello(Player* /*player*/, GameObject* go)
    {
        InstanceScript* instance = go->GetInstanceScript();

        if (!instance)
            return false;

        switch (go->GetEntry())
        {
            case GO_MIMIRON_CALL_TRAM_1:
            case GO_MIMIRON_ACTIVATE_TRAM_1:
				go->UseDoorOrButton();
                instance->SetData(DATA_CALL_TRAM, 0);
                break;
            case GO_MIMIRON_CALL_TRAM_2:
            case GO_MIMIRON_ACTIVATE_TRAM_2:
				go->UseDoorOrButton();
                instance->SetData(DATA_CALL_TRAM, 1);
                break;
        }
        return true;
    }
};

class achievement_firefighter : public AchievementCriteriaScript
{
public:
	achievement_firefighter() : AchievementCriteriaScript("achievement_firefighter") { }

	bool OnCheck(Player* player, Unit* /*target*/)
	{
		if (!player)
			return false;

		if (InstanceScript* instance = player->GetInstanceScript())
		{
			if (Creature* mimiron = ObjectAccessor::GetCreature(*player, instance->GetData64(DATA_MIMIRON)))
			{
				if (mimiron->AI()->GetData(DATA_GET_HARD_MODE))
					return true;
			}
		}

		return false;
	}
};

class achievement_set_us_up_the_bomb : public AchievementCriteriaScript
{
public:
	uint32 spell_id;

	achievement_set_us_up_the_bomb(const char* name, uint32 spell_entry) : AchievementCriteriaScript(name)
	{
		spell_id = spell_entry;
	}

	bool OnCheck(Player* player, Unit* /*target*/)
	{
		if (!player)
			return false;

		if (InstanceScript* instance = player->GetInstanceScript())
		{
			if (Creature* mimiron = ObjectAccessor::GetCreature(*player, instance->GetData64(DATA_MIMIRON)))
			{
				if (spell_id == SPELL_BOMB_BOT && mimiron->AI()->GetData(DATA_SET_US_UP_THE_BOMB_BOT))
					return true;

				if (spell_id == SPELL_ROCKET_STRIKE_DAMAGE && mimiron->AI()->GetData(DATA_SET_US_UP_THE_BOMB_ROCKET))
					return true;

				if (spell_id == SPELL_EXPLOSION && mimiron->AI()->GetData(DATA_SET_US_UP_THE_BOMB_MINE))
					return true;
			}
		}
		return false;
	}
};

void AddSC_boss_mimiron()
{
    new boss_mimiron();
    new boss_leviathan_mk();
    new boss_leviathan_mk_turret();
    new npc_proximity_mine();
    new boss_vx_001();
    new npc_rocket_strike();
    new spell_rapid_burst();
    new boss_aerial_command_unit();
    new npc_magnetic_core();
    new npc_assault_bot();
    new npc_emergency_bot();
    new npc_mimiron_bomb_bot();
    new npc_mimiron_flame_trigger();
    new npc_mimiron_flame_spread();
    new npc_frost_bomb();
	new go_not_push_button();
	new go_call_tram();
    new achievement_firefighter();
	new achievement_set_us_up_the_bomb("achievement_set_us_up_the_bomb_bot", SPELL_BOMB_BOT);
	new achievement_set_us_up_the_bomb("achievement_set_us_up_the_bomb_rocket", SPELL_ROCKET_STRIKE_DAMAGE);
	new achievement_set_us_up_the_bomb("achievement_set_us_up_the_bomb_mine", SPELL_EXPLOSION);
}