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
#include "QuantumGossip.h"
#include "QuantumEscortAI.h"
#include "halls_of_stone.h"

enum Texts
{
    SAY_KILL_1                          = -1599016,
    SAY_KILL_2                          = -1599017,
    SAY_KILL_3                          = -1599018,
    SAY_LOW_HEALTH                      = -1599019,
    SAY_DEATH                           = -1599020,
    SAY_PLAYER_DEATH_1                  = -1599021,
    SAY_PLAYER_DEATH_2                  = -1599022,
    SAY_PLAYER_DEATH_3                  = -1599023,
    SAY_ESCORT_START                    = -1599024,

    SAY_EVENT_INTRO_1                   = -1599029,
    SAY_EVENT_INTRO_2                   = -1599030,
    SAY_EVENT_INTRO_3_ABED              = -1599031,

    SAY_EVENT_A_1                       = -1599032,
    SAY_EVENT_A_2_KADD                  = -1599033,
    SAY_EVENT_A_3                       = -1599034,

    SAY_EVENT_B_1                       = -1599035,
    SAY_EVENT_B_2_MARN                  = -1599036,
    SAY_EVENT_B_3                       = -1599037,

    SAY_EVENT_C_1                       = -1599038,
    SAY_EVENT_C_2_ABED                  = -1599039,
    SAY_EVENT_C_3                       = -1599040,

    SAY_EVENT_D_1                       = -1599041,
    SAY_EVENT_D_2_ABED                  = -1599042,
    SAY_EVENT_D_3                       = -1599043,
    SAY_EVENT_D_4_ABED                  = -1599044,

    SAY_EVENT_END_01                    = -1599045,
    SAY_EVENT_END_02                    = -1599046,
    SAY_EVENT_END_03_ABED               = -1599047,
    SAY_EVENT_END_04                    = -1599048,
    SAY_EVENT_END_05_ABED               = -1599049,
    SAY_EVENT_END_06                    = -1599050,
    SAY_EVENT_END_07_ABED               = -1599051,
    SAY_EVENT_END_08                    = -1599052,
    SAY_EVENT_END_09_KADD               = -1599053,
    SAY_EVENT_END_10                    = -1599054,
    SAY_EVENT_END_11_KADD               = -1599055,
    SAY_EVENT_END_12                    = -1599056,
    SAY_EVENT_END_13_KADD               = -1599057,
    SAY_EVENT_END_14                    = -1599058,
    SAY_EVENT_END_15_MARN               = -1599059,
    SAY_EVENT_END_16                    = -1599060,
    SAY_EVENT_END_17_MARN               = -1599061,
    SAY_EVENT_END_18                    = -1599062,
    SAY_EVENT_END_19_MARN               = -1599063,
    SAY_EVENT_END_20                    = -1599064,
    SAY_EVENT_END_21_ABED               = -1599065,

    SAY_ENTRANCE_MEET                   = -1599068,

    TEXT_ID_START                       = 13100,
    TEXT_ID_PROGRESS                    = 13101,
};

enum Spells
{
    SPELL_STEALTH                       = 58506,
    //Kadrak
    SPELL_GLARE_OF_THE_TRIBUNAL         = 50988,
    H_SPELL_GLARE_OF_THE_TRIBUNAL       = 59870,
    //Marnak
    SPELL_DARK_MATTER_VISUAL            = 51000,
    SPELL_DARK_MATTER_DUMMY             = 51001,
    SPELL_DARK_MATTER                   = 51012,
    H_SPELL_DARK_MATTER                 = 59868,
    //Abedneum
    SPELL_SEARING_GAZE_DUMMY            = 51136,
    SPELL_SEARING_GAZE                  = 51125,
    H_SPELL_SEARING_GAZE                = 59866,
};

enum Quests
{
    QUEST_HALLS_OF_STONE                = 13207,
};

enum Achievements
{
    ACHIEVEMENT_BRANN_SPANKIN_NEW       = 2154,
};


#define GOSSIP_ITEM_START    "Brann, it would be our honor!"
#define GOSSIP_ITEM_PROGRESS "Let's move Brann, enough of the history lessons!"

static Position SpawnLocations[] =
{
    {947.85f, 388.953f, 205.994f, 3.97947f},
    {953.346f, 383.628f, 205.994f, 3.88601f},
};

class npc_tribunal_of_the_ages : public CreatureScript
{
public:
    npc_tribunal_of_the_ages() : CreatureScript("npc_tribunal_of_the_ages") { }

    struct npc_tribunal_of_the_agesAI : public QuantumBasicAI
    {
        npc_tribunal_of_the_agesAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            SetCombatMovement(false);
        }

        InstanceScript* instance;

        uint64 DarkMatterGUID;

        uint32 KaddrakEncounterTimer;
        uint32 MarnakEncounterTimer;
        uint32 AbedneumEncounterTimer;
        uint32 LoadingTimer;

        bool bKaddrakActivated;
        bool bMarnakActivated;
        bool bAbedneumActivated;
        bool bLoadingDarkMatter;

        std::list<uint64> KaddrakGUIDList;
        std::list<uint64> AbedneumGUIDList;
        std::list<uint64> MarnakGUIDList;

        void Reset()
        {
            KaddrakEncounterTimer = 1500;
            MarnakEncounterTimer = 10*IN_MILLISECONDS;
            AbedneumEncounterTimer = 10*IN_MILLISECONDS;
            LoadingTimer = 5*IN_MILLISECONDS;

            bKaddrakActivated = false;
            bMarnakActivated = false;
            bAbedneumActivated = false;
            bLoadingDarkMatter = false;

            DarkMatterGUID = 0;

            if (instance)
            {
                instance->HandleGameObject(instance->GetData64(DATA_GO_KADDRAK), false);
                instance->HandleGameObject(instance->GetData64(DATA_GO_MARNAK), false);
                instance->HandleGameObject(instance->GetData64(DATA_GO_ABEDNEUM), false);
                instance->HandleGameObject(instance->GetData64(DATA_GO_SKY_FLOOR), false);
            }

            KaddrakGUIDList.clear();
            AbedneumGUIDList.clear();
            MarnakGUIDList.clear();
        }

        void UpdateAI(uint32 const diff)
        {
            if (bKaddrakActivated)
            {
                if (KaddrakEncounterTimer <= diff)
                {
                    if (!KaddrakGUIDList.empty())
                        for (std::list<uint64>::const_iterator itr = KaddrakGUIDList.begin(); itr != KaddrakGUIDList.end(); ++itr)
                        {
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
							{
                                if (Creature* kaddrak = Unit::GetCreature(*me, *itr))
								{
                                    if (kaddrak->IsAlive())
                                        kaddrak->CastSpell(target, DUNGEON_MODE(SPELL_GLARE_OF_THE_TRIBUNAL, H_SPELL_GLARE_OF_THE_TRIBUNAL), true);
								}
							}
                        }
						KaddrakEncounterTimer = 3*IN_MILLISECONDS;
                }
				else KaddrakEncounterTimer -= diff;
            }

            if (bMarnakActivated)
            {
                if (MarnakEncounterTimer <= diff)
                {
                    if (!MarnakGUIDList.empty())
					{
                        if (Creature* matter = me->SummonCreature(NPC_DARK_MATTER, 904.07f, 351.363f, 214.77f, 0, TEMPSUMMON_TIMED_DESPAWN, 20*IN_MILLISECONDS))
                        {
                            matter->SetDisplayId(17200);
                            matter->SetCurrentFaction(16);
                            DarkMatterGUID = matter->GetGUID();
                            
                            bLoadingDarkMatter = true;

                            for (std::list<uint64>::const_iterator itr = MarnakGUIDList.begin(); itr != MarnakGUIDList.end(); ++itr)
                            {
                                if (Creature* marnak = Unit::GetCreature(*me, *itr))
								{
                                    if (marnak->IsAlive())
                                        marnak->CastSpell(matter, SPELL_DARK_MATTER_DUMMY, true);
								}
                            }
                        }
					}
                    MarnakEncounterTimer = urand(20000, 25000);
                }
				else MarnakEncounterTimer -= diff;

                if (bLoadingDarkMatter && DarkMatterGUID)
                {
                    if (LoadingTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
						{
                            if (Creature* matter = Unit::GetCreature(*me, DarkMatterGUID))
                            {
                                float x, y, z;
                                target->GetPosition(x, y, z);
                
                                me->SummonCreature(NPC_DARK_MATTER_TARGET, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN, 20*IN_MILLISECONDS);
                                matter->GetMotionMaster()->MovePoint(0, x, y, z);
                            }

							bLoadingDarkMatter = false;
							LoadingTimer = 5*IN_MILLISECONDS;
						}
					}
					else LoadingTimer -= diff;
                }
            }

            if (bAbedneumActivated)
            {
                if (AbedneumEncounterTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
					{
                        if (!AbedneumGUIDList.empty())
						{
                            if (Creature* gaze = me->SummonCreature(NPC_SEARING_GAZE_TARGET, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 11*IN_MILLISECONDS))
                            {
                                gaze->SetDisplayId(MODEL_ID_INVISIBLE);
                                gaze->SetCurrentFaction(16);

                                for (std::list<uint64>::const_iterator itr = AbedneumGUIDList.begin(); itr != AbedneumGUIDList.end(); ++itr)
                                {
                                    if (Creature* abedneum = Unit::GetCreature(*me, *itr))
									{
                                        if (abedneum->IsAlive())
                                            abedneum->CastSpell(gaze, SPELL_SEARING_GAZE_DUMMY, true);
									}
                                }
                            }

							AbedneumEncounterTimer = urand(17500, 22500);
						}
					}
                }
				else AbedneumEncounterTimer -= diff;
            }
        }
    };
	
	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tribunal_of_the_agesAI(creature);
    }
};

class npc_brann_hos : public CreatureScript
{
public:
    npc_brann_hos() : CreatureScript("npc_brann_hos") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        InstanceScript* instance = creature->GetInstanceScript();

        uint32 brann_event_data = instance->GetData(DATA_BRANN_EVENT);
        uint32 StepCount = CAST_AI(npc_brann_hos::npc_brann_hosAI, creature->AI())->Step;

        if (brann_event_data == NOT_STARTED && StepCount == 0)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        // skip history lessons
        if (StepCount > 29 && StepCount < 49)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_PROGRESS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        
        player->SEND_GOSSIP_MENU(TEXT_ID_START, creature->GetGUID());
        return true;
    }

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                CAST_AI(npc_brann_hos::npc_brann_hosAI, creature->AI())->StartWP();
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                CAST_AI(npc_brann_hos::npc_brann_hosAI, creature->AI())->Step = 49;
                CAST_AI(npc_brann_hos::npc_brann_hosAI, creature->AI())->PhaseTimer = 1000;
                break;
        }

        player->CLOSE_GOSSIP_MENU();
        return true;
    }

    struct npc_brann_hosAI : public npc_escortAI
    {
        npc_brann_hosAI(Creature* creature) : npc_escortAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 Step;
        uint32 PhaseTimer;

        uint64 ControllerGUID;
        std::list<uint64> lSummonGUIDList;

        InstanceScript* instance;

        bool bIsBattle;
        bool bIsLowHP;
        bool bHasBeenDamaged;

        void Reset()
        {
            if (!HasEscortState(STATE_ESCORT_ESCORTING))
            {
                bIsLowHP = false;
                bIsBattle = false;
                bHasBeenDamaged = false;
                Step = 0;
                PhaseTimer = 0;
                ControllerGUID = 0;
				me->SetCurrentFaction(35);

                DespawnSummon();

                if (instance)
                    if (instance->GetData(DATA_BRANN_EVENT) != DONE) // dont reset if already done
                        instance->SetData(DATA_BRANN_EVENT, NOT_STARTED);
            }
        }

        void DespawnSummon()
        {
            if (lSummonGUIDList.empty() || !instance)
                return;

            for (std::list<uint64>::const_iterator itr = lSummonGUIDList.begin(); itr != lSummonGUIDList.end(); ++itr)
            {
                Creature* summon = Unit::GetCreature(*me, *itr);
                if (summon && summon->IsAlive())
                {
                    if (instance->GetData(DATA_BRANN_EVENT) == DONE && (summon->GetEntry() == NPC_KADDRAK || summon->GetEntry() == NPC_MARNAK
                        || summon->GetEntry() == NPC_ABEDNEUM))
                        continue;

                    summon->DespawnAfterAction();
                }
            }
            lSummonGUIDList.clear();
        }

        void WaypointReached(uint32 PointId)
        {
            switch (PointId)
            {
                case 7:
                    SpawnSummon(4);
                    break;
                case 13:
                    DoSendQuantumText(SAY_EVENT_INTRO_1, me);
                    SetEscortPaused(true);
                    JumpToNextStep(20000);
                    break;
                case 17:
                    DoSendQuantumText(SAY_EVENT_INTRO_2, me);
                    if (instance)
                        instance->HandleGameObject(instance->GetData64(DATA_GO_TRIBUNAL_CONSOLE), true);
                    me->SetStandState(UNIT_STAND_STATE_KNEEL);
					me->SetCurrentFaction(1665);
                    SetEscortPaused(true);
                    JumpToNextStep(8500);
                    break;
                case 18:
                    SetEscortPaused(true);
                    break;
            }
        }

        void SpawnSummon(uint32 type)
        {
            uint32 rnd = urand(0, 1);

            switch (type)
            {
                case 1:
                {
                    uint32 SpawnNumber = DUNGEON_MODE(2,3);

                    for (uint8 i = 0; i < SpawnNumber; ++i)
					{
						me->SummonCreature(NPC_DARK_RUNE_PROTECTOR, SpawnLocations[rnd], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
						me->SummonCreature(NPC_DARK_RUNE_STORMCALLER, SpawnLocations[rnd], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);

					}
					break;
				}
                case 2:
                    for (uint8 i = 0; i < 2; ++i)
					{
                        me->SummonCreature(NPC_DARK_RUNE_STORMCALLER, SpawnLocations[rnd], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
					}
                    break;
                case 3:
                    me->SummonCreature(NPC_IRON_GOLEM_CUSTODIAN, SpawnLocations[rnd], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                    break;
                case 4:
                    if (Creature* creature = me->SummonCreature(NPC_TRIBUNAL_OF_THE_AGES, 910.126f, 345.795f, 237.928f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000))
                    {
                        ControllerGUID = creature->GetGUID();
                        creature->SetVisible(false);
                    }
                    break;
                case 5:
                {
                    if (Creature* controller = Unit::GetCreature(*me, ControllerGUID))
                    {
                        uint32 PositionCounter = 0;
                        for (uint8 i = 0; i < 2; ++i)
                        {
                            Creature* kaddrak = 0;

                            if (PositionCounter == 0)
                                kaddrak = me->SummonCreature(NPC_KADDRAK, 927.265f, 333.200f, 218.780f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                            else
                                kaddrak = me->SummonCreature(NPC_KADDRAK, 921.745f, 328.076f, 218.780f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                            
                            if (kaddrak)
                                CAST_AI(npc_tribunal_of_the_ages::npc_tribunal_of_the_agesAI, controller->AI())->KaddrakGUIDList.push_back(kaddrak->GetGUID());

                            ++PositionCounter;
                        }
                        
                        controller->AI()->DoZoneInCombat(controller, 100.0f);
                        CAST_AI(npc_tribunal_of_the_ages::npc_tribunal_of_the_agesAI, controller->AI())->bKaddrakActivated = true;
                    }
                    break;
                }
                case 6:
                {
                    if (Creature* controller = Unit::GetCreature(*me, ControllerGUID))
                    {
                        uint32 PositionCounter = 0;
                        for (uint8 i = 0; i < 2; ++i)
                        {
                            Creature* marnak = 0;

                            if (PositionCounter == 0)
                                marnak = me->SummonCreature(NPC_MARNAK, 891.309f, 359.382f, 217.422f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                            else
                                marnak = me->SummonCreature(NPC_MARNAK, 895.834f, 363.436f, 217.422f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                            
                            if (marnak)
                                CAST_AI(npc_tribunal_of_the_ages::npc_tribunal_of_the_agesAI, controller->AI())->MarnakGUIDList.push_back(marnak->GetGUID());

                            ++PositionCounter;
                        }
                        
                        controller->AI()->DoZoneInCombat(controller, 100.0f);
                        CAST_AI(npc_tribunal_of_the_ages::npc_tribunal_of_the_agesAI, controller->AI())->bMarnakActivated = true;
                    }
                    break;
                }
                case 7:
                {
                    if (Creature* controller = Unit::GetCreature(*me, ControllerGUID))
                    {
                        uint32 PositionCounter = 0;
                        for (uint8 i = 0; i < 2; ++i)
                        {
                            Creature* abedneum = 0;

                            if (PositionCounter == 0)
                                abedneum = me->SummonCreature(NPC_ABEDNEUM, 897.865f, 328.341f, 223.84f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                            else
                                abedneum = me->SummonCreature(NPC_ABEDNEUM, 893.012f, 332.804f, 223.545f, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
                            
                            if (abedneum)
                                CAST_AI(npc_tribunal_of_the_ages::npc_tribunal_of_the_agesAI, controller->AI())->AbedneumGUIDList.push_back(abedneum->GetGUID());

                            ++PositionCounter;
                        }
                        
                        controller->AI()->DoZoneInCombat(controller, 100.0f);
                        CAST_AI(npc_tribunal_of_the_ages::npc_tribunal_of_the_agesAI, controller->AI())->bAbedneumActivated = true;
                    }
                    break;
                }
            }
        }

        void JustSummoned(Creature* summoned)
        {
            lSummonGUIDList.push_back(summoned->GetGUID());

            switch (summoned->GetEntry())
            {
                case NPC_DARK_RUNE_PROTECTOR:
                case NPC_DARK_RUNE_STORMCALLER:
                case NPC_IRON_GOLEM_CUSTODIAN:
					summoned->AI()->EnterToBattle(me);
					summoned->AddThreat(me, 1.0f);
					summoned->AI()->AttackStart(me);
                    break;
            }
        }

        void JumpToNextStep(uint32 Timer)
        {
            PhaseTimer = Timer;
            ++Step;
        }

        void JustDied(Unit* /*victim*/)
        {
            me->Respawn();
        }

        void StartWP()
        {
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            SetEscortPaused(false);
            Step = 1;
            Start();
        }

        void DamageTaken(Unit* /*doneby*/, uint32 & /*damage*/)
        {
            if (!bHasBeenDamaged)
                bHasBeenDamaged = true;
        }

        void UpdateEscortAI(const uint32 diff)
        {
            if (PhaseTimer <= diff)
            {
                if (!instance)
                    return;

                switch (Step)
                {
                    case 1:
                        if (instance->GetData(DATA_BRANN_EVENT) != NOT_STARTED)
                            return;
                        instance->SetData(DATA_BRANN_EVENT, IN_PROGRESS);
                        bIsBattle = false;
                        DoSendQuantumText(SAY_ESCORT_START, me);
                        SetRun(true);
                        JumpToNextStep(0);
                        break;
                    case 3:
                        SetEscortPaused(false);
                        JumpToNextStep(0);
                        break;
                    case 5:
                        if (Creature* pTemp = (Unit::GetCreature(*me, instance->GetData64(DATA_ABEDNEUM))))
                            DoSendQuantumText(SAY_EVENT_INTRO_3_ABED, pTemp);
                        JumpToNextStep(8500);
                        break;
                    case 6:
                        DoSendQuantumText(SAY_EVENT_A_1, me);
                        JumpToNextStep(6500);
                        break;
                    case 7:
                        if (Creature* pTemp = Unit::GetCreature(*me, instance->GetData64(DATA_KADDRAK)))
                            DoSendQuantumText(SAY_EVENT_A_2_KADD, pTemp);
                        JumpToNextStep(12500);
                        break;
                    case 8:
                        DoSendQuantumText(SAY_EVENT_A_3, me);
                        SpawnSummon(5);
                        JumpToNextStep(5000);
                        break;
                    case 9:
                        me->SetReactState(REACT_PASSIVE);
                        SpawnSummon(1);
                        JumpToNextStep(20000);
                        break;
                    case 10:
                        DoSendQuantumText(SAY_EVENT_B_1, me);
                        JumpToNextStep(6000);
                        break;
                    case 11:
                        if (Creature* pTemp = Unit::GetCreature(*me, instance->GetData64(DATA_MARNAK)))
                            DoSendQuantumText(SAY_EVENT_B_2_MARN, pTemp);
                        SpawnSummon(1);
                        JumpToNextStep(20000);
                        break;
                    case 12:
                        DoSendQuantumText(SAY_EVENT_B_3, me);
                        SpawnSummon(6);
                        JumpToNextStep(10000);
                        break;
                    case 13:
                        SpawnSummon(1);
                        JumpToNextStep(10000);
                        break;
                    case 14:
                        SpawnSummon(2);
                        JumpToNextStep(20000);
                        break;
                    case 15:
                        DoSendQuantumText(SAY_EVENT_C_1, me);
                        SpawnSummon(1);
                        JumpToNextStep(10000);
                        break;
                    case 16:
                        SpawnSummon(2);
                        JumpToNextStep(20000);
                        break;
                    case 17:
                        if (Creature* pTemp = Unit::GetCreature(*me, instance->GetData64(DATA_ABEDNEUM)))
                            DoSendQuantumText(SAY_EVENT_C_2_ABED, pTemp);
                        SpawnSummon(1);
                        JumpToNextStep(20000);
                        break;
                    case 18:
                        DoSendQuantumText(SAY_EVENT_C_3, me);
                        SpawnSummon(7);
                        JumpToNextStep(5000);
                        break;
                    case 19:
                        SpawnSummon(2);
                        JumpToNextStep(10000);
                        break;
                    case 20:
                        SpawnSummon(1);
                        JumpToNextStep(15000);
                        break;
                    case 21:
                        DoSendQuantumText(SAY_EVENT_D_1, me);
                        SpawnSummon(3);
                        JumpToNextStep(20000);
                        break;
                    case 22:
                        if (Creature* pTemp = Unit::GetCreature(*me, instance->GetData64(DATA_ABEDNEUM)))
                            DoSendQuantumText(SAY_EVENT_D_2_ABED, pTemp);
                        SpawnSummon(1);
                        JumpToNextStep(5000);
                        break;
                    case 23:
                        SpawnSummon(2);
                        JumpToNextStep(15000);
                        break;
                    case 24:
                        DoSendQuantumText(SAY_EVENT_D_3, me);
                        SpawnSummon(3);
                        JumpToNextStep(5000);
                        break;
                    case 25:
                        SpawnSummon(1);
                        JumpToNextStep(5000);
                        break;
                    case 26:
                        SpawnSummon(2);
                        JumpToNextStep(10000);
                        break;
                    case 27:
                        if (Creature* pTemp = Unit::GetCreature(*me, instance->GetData64(DATA_ABEDNEUM)))
                            DoSendQuantumText(SAY_EVENT_D_4_ABED, pTemp);
                        SpawnSummon(1);
                        JumpToNextStep(10000);
                        break;
                    case 28:
                        me->SetReactState(REACT_DEFENSIVE);
                        DoSendQuantumText(SAY_EVENT_END_01, me);
                        me->SetStandState(UNIT_STAND_STATE_STAND);
                        instance->HandleGameObject(instance->GetData64(DATA_GO_SKY_FLOOR), true);
                        if (Creature* pTemp = Unit::GetCreature(*me, ControllerGUID))
                            pTemp->DealDamage(pTemp, pTemp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        bIsBattle = true;
                        SetEscortPaused(false);
                        JumpToNextStep(6500);
                        break;
                    case 29:
                        DoSendQuantumText(SAY_EVENT_END_02, me);

                        instance->HandleGameObject(instance->GetData64(DATA_GO_ABEDNEUM), true);
                        instance->SetData(DATA_BRANN_EVENT, DONE);
                        DespawnSummon();
                        instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, 59046);

                        if (!bHasBeenDamaged && IsHeroic())
                            instance->DoCompleteAchievement(ACHIEVEMENT_BRANN_SPANKIN_NEW);
                        JumpToNextStep(5500);
                        break;
                    case 30:
                        if (Creature* pTemp = Unit::GetCreature(*me, instance->GetData64(DATA_ABEDNEUM)))
                            DoSendQuantumText(SAY_EVENT_END_03_ABED, pTemp);
                        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        JumpToNextStep(8500);
                        break;
                    case 31:
                        DoSendQuantumText(SAY_EVENT_END_04, me);
                        JumpToNextStep(11500);
                        break;
                    case 32:
                        if (Creature* pTemp = Unit::GetCreature(*me, instance->GetData64(DATA_ABEDNEUM)))
                            DoSendQuantumText(SAY_EVENT_END_05_ABED, pTemp);
                        JumpToNextStep(11500);
                        break;
                    case 33:
                        DoSendQuantumText(SAY_EVENT_END_06, me);
                        JumpToNextStep(4500);
                        break;
                    case 34:
                        if (Creature* pTemp = Unit::GetCreature(*me, instance->GetData64(DATA_ABEDNEUM)))
                            DoSendQuantumText(SAY_EVENT_END_07_ABED, pTemp);
                        JumpToNextStep(22500);
                        break;
                    case 35:
                        DoSendQuantumText(SAY_EVENT_END_08, me);
                        JumpToNextStep(7500);
                        break;
                    case 36:
                        instance->HandleGameObject(instance->GetData64(DATA_GO_KADDRAK), true);
                        if (Creature* pTemp = Unit::GetCreature(*me, instance->GetData64(DATA_KADDRAK)))
                            DoSendQuantumText(SAY_EVENT_END_09_KADD, pTemp);
                        JumpToNextStep(18500);
                        break;
                    case 37:
                        DoSendQuantumText(SAY_EVENT_END_10, me);
                        JumpToNextStep(5500);
                        break;
                    case 38:
                        if (Creature* pTemp = Unit::GetCreature(*me, instance->GetData64(DATA_KADDRAK)))
                            DoSendQuantumText(SAY_EVENT_END_11_KADD, pTemp);
                        JumpToNextStep(20500);
                        break;
                    case 39:
                        DoSendQuantumText(SAY_EVENT_END_12, me);
                        JumpToNextStep(2500);
                        break;
                    case 40:
                        if (Creature* pTemp = Unit::GetCreature(*me, instance->GetData64(DATA_KADDRAK)))
                            DoSendQuantumText(SAY_EVENT_END_13_KADD, pTemp);
                        JumpToNextStep(19500);
                        break;
                    case 41:
                        DoSendQuantumText(SAY_EVENT_END_14, me);
                        JumpToNextStep(10500);
                        break;
                    case 42:
                        instance->HandleGameObject(instance->GetData64(DATA_GO_MARNAK), true);
                        if (Creature* pTemp = Unit::GetCreature(*me, instance->GetData64(DATA_MARNAK)))
                            DoSendQuantumText(SAY_EVENT_END_15_MARN, pTemp);
                        JumpToNextStep(6500);
                        break;
                    case 43:
                        DoSendQuantumText(SAY_EVENT_END_16, me);
                        JumpToNextStep(6500);
                        break;
                    case 44:
                        if (Creature* pTemp = Unit::GetCreature(*me, instance->GetData64(DATA_MARNAK)))
                            DoSendQuantumText(SAY_EVENT_END_17_MARN, pTemp);
                        JumpToNextStep(25500);
                        break;
                    case 45:
                        DoSendQuantumText(SAY_EVENT_END_18, me);
                        JumpToNextStep(23500);
                        break;
                    case 46:
                        if (Creature* pTemp = Unit::GetCreature(*me, instance->GetData64(DATA_MARNAK)))
                            DoSendQuantumText(SAY_EVENT_END_19_MARN, pTemp);
                        JumpToNextStep(3500);
                        break;
                    case 47:
                        DoSendQuantumText(SAY_EVENT_END_20, me);
                        JumpToNextStep(8500);
                        break;
                    case 48:
                        if (Creature* pTemp = Unit::GetCreature(*me, instance->GetData64(DATA_ABEDNEUM)))
                            DoSendQuantumText(SAY_EVENT_END_21_ABED, pTemp);
                        JumpToNextStep(5500);
                        break;
                    case 49:
                    {
                        instance->HandleGameObject(instance->GetData64(DATA_GO_KADDRAK), false);
                        instance->HandleGameObject(instance->GetData64(DATA_GO_MARNAK), false);
                        instance->HandleGameObject(instance->GetData64(DATA_GO_ABEDNEUM), false);
                        instance->HandleGameObject(instance->GetData64(DATA_GO_SKY_FLOOR), false);

                        Player* player = GetPlayerForEscort();
                        if (player)
                            player->GroupEventHappens(QUEST_HALLS_OF_STONE, me);

						DoSendQuantumText(SAY_EVENT_END_20, me);
                        JumpToNextStep(10000); // (180000);
                        break;
                    }
                    case 50:
                        SetEscortPaused(false);
                        break;
                }
            }
            else PhaseTimer -= diff;

            if (!bIsLowHP && HealthBelowPct(30))
            {
                DoSendQuantumText(SAY_LOW_HEALTH, me);
                bIsLowHP = true;
            }
            else if (bIsLowHP && !HealthBelowPct(30))
                bIsLowHP = false;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_brann_hosAI(creature);
    }
};

class npc_dark_matter : public CreatureScript
{
public:
    npc_dark_matter() : CreatureScript("npc_dark_matter") { }

    struct npc_dark_matterAI : public QuantumBasicAI
    {
        npc_dark_matterAI(Creature* creature) : QuantumBasicAI(creature) { }

        bool Aura;
        bool Casted;
        uint32 CheckTimer;

        void Reset()
        {
            Aura = false;
            Casted = false;
            CheckTimer = 5500;
            me->SetReactState(REACT_PASSIVE);
            me->SetSpeed(MOVE_FLIGHT, 0.7f, true);
        }

        void UpdateAI(const uint32 diff)
        {
            if (CheckTimer <= diff)
            {
                if (!Aura)
                {
                    me->RemoveAllAuras();
                    me->AddAura(SPELL_DARK_MATTER_VISUAL, me);
                    Aura = true;
                }

                if (Casted)
                    me->DisappearAndDie();

                if (Creature* target = me->FindCreatureWithDistance(NPC_DARK_MATTER_TARGET, 1.0f, true))
                {
                    if (!Casted) // prevent double cast
                        DoCast(DUNGEON_MODE(SPELL_DARK_MATTER, H_SPELL_DARK_MATTER));
                    Casted = true;
                }

                CheckTimer = 1*IN_MILLISECONDS;
            } 
            else CheckTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_matterAI(creature);
    }
};

class npc_searing_gaze : public CreatureScript
{
public:
    npc_searing_gaze() : CreatureScript("npc_searing_gaze") { }

    struct npc_searing_gazeAI : public QuantumBasicAI
    {
        npc_searing_gazeAI(Creature* creature) : QuantumBasicAI(creature) { }

        uint32 CheckTimer;

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);
            CheckTimer = 1*IN_MILLISECONDS;
        }

        void UpdateAI(const uint32 diff)
        {
            if (CheckTimer <= diff)
            {
                DoCast(DUNGEON_MODE(SPELL_SEARING_GAZE, H_SPELL_SEARING_GAZE));
                CheckTimer = 1*IN_MILLISECONDS;
            }
			else CheckTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_searing_gazeAI(creature);
    }
};

enum TribunalTrashSpells
{
	SPELL_LIGHTNING_BOLT_5N   = 12167,
	SPELL_LIGHTNING_BOLT_5H   = 59863,
	SPELL_SHADOW_WORD_PAIN_5N = 15654,
	SPELL_SHADOW_WORD_PAIN_5H = 59864,
	SPELL_CHARGE              = 22120,
	SPELL_CLEAVE              = 42724,
	SPELL_CRUSH_ARMOR         = 33661,
	SPELL_GROUND_SMASH_5N     = 12734,
	SPELL_GROUND_SMASH_5H     = 59865,
};

class npc_dark_rune_stormcaller : public CreatureScript
{
public:
    npc_dark_rune_stormcaller() : CreatureScript("npc_dark_rune_stormcaller") {}

    struct npc_dark_rune_stormcallerAI : public QuantumBasicAI
    {
        npc_dark_rune_stormcallerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LightningBoltTimer;
		uint32 ShadowWordPainTimer;

        void Reset()
        {
			LightningBoltTimer = 2000;
			ShadowWordPainTimer = 4000;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (LightningBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_LIGHTNING_BOLT_5N, SPELL_LIGHTNING_BOLT_5H));
					LightningBoltTimer = urand(3000, 4000);
				}
			}
			else LightningBoltTimer -= diff;

			if (ShadowWordPainTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_WORD_PAIN_5N, SPELL_SHADOW_WORD_PAIN_5H));
					ShadowWordPainTimer = urand(7000, 8000);
				}
			}
			else LightningBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_rune_stormcallerAI(creature);
    }
};

class npc_dark_rune_protector : public CreatureScript
{
public:
    npc_dark_rune_protector() : CreatureScript("npc_dark_rune_protector") {}

    struct npc_dark_rune_protectorAI : public QuantumBasicAI
    {
        npc_dark_rune_protectorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CleaveTimer;

        void Reset()
        {
			CleaveTimer = 2000;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_CHARGE);
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastAOE(SPELL_CLEAVE);
				CleaveTimer = urand(3000, 4000);
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_rune_protectorAI(creature);
    }
};

class npc_iron_golem_custodian : public CreatureScript
{
public:
    npc_iron_golem_custodian() : CreatureScript("npc_iron_golem_custodian") {}

    struct npc_iron_golem_custodianAI : public QuantumBasicAI
    {
        npc_iron_golem_custodianAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CrushArmorTimer;
		uint32 GroundSmashTimer;

        void Reset()
        {
			CrushArmorTimer = 2000;
			GroundSmashTimer = 4000;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CrushArmorTimer <= diff)
			{
				DoCastVictim(SPELL_CRUSH_ARMOR);
				CrushArmorTimer = urand(3000, 4000);
			}
			else CrushArmorTimer -= diff;

			if (GroundSmashTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_GROUND_SMASH_5N, SPELL_GROUND_SMASH_5H));
				GroundSmashTimer = urand(7000, 8000);
			}
			else GroundSmashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_iron_golem_custodianAI(creature);
    }
};

void AddSC_halls_of_stone()
{
    new npc_brann_hos();
    new npc_tribunal_of_the_ages();
    new npc_dark_matter();
    new npc_searing_gaze();
	new npc_dark_rune_stormcaller();
	new npc_dark_rune_protector();
	new npc_iron_golem_custodian();
}