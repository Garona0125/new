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
#include "InstanceScript.h"
#include "QuantumGossip.h"
#include "QuantumEscortAI.h"
#include "ulduar.h"

#define GOSSIP_NORGANNON_1 "Activate secondary defensive systems."
#define GOSSIP_NORGANNON_2 "Confirmed."
#define GOSSIP_ITEM_BRANN  "We are ready"

// Kirin Tor Battle-Mage and Hired Engineer mini event
#define KIRIN_TOR_BATTLE_MAGE_SAY_1 "Any luck?"
#define HIRED_ENGINEER_SAY_1        "Not a bit. For the life of me, I can't figure out how this thing works."
#define KIRIN_TOR_BATTLE_MAGE_SAY_2 "Keep at it. If our suspicions are right and this is a transporter, it could prove extremely useful in our assault."
#define HIRED_ENGINEER_SAY_2        "I know, I know... I haven't given up yet. Don't get your hopes up though, this technology is way beyond me."

enum Actions
{
    ACTION_START = 1,
};

enum Spells
{
	SPELL_SUMMON_IRON_DWARF_WATCHER = 63135,
    SPELL_VISUAL_BEAM               = 48310,
};

enum Texts
{
	// Explorer Dellorah
	SAY_DELLORAH_EVENT_1     = -1613250,
	SAY_DELLORAH_EVENT_2     = -1613251,
	SAY_DELLORAH_EVENT_3     = -1613252,
	SAY_DELLORAH_EVENT_4     = -1613253,
	SAY_DELLORAH_EVENT_5     = -1613254,
	SAY_DELLORAH_EVENT_7     = -1613255,
	SAY_DELLORAH_EVENT_8     = -1613256,
	SAY_DELLORAH_BRANN_1     = -1613257,
	SAY_DELLORAH_BRANN_2     = -1613258,
	// Lore Keeper Norgannon
	SAY_NORGANNON_EVENT_1    = -1613259,
	SAY_NORGANNON_EVENT_2    = -1613260,
	SAY_NORGANNON_EVENT_3    = -1613261,
	SAY_NORGANNON_EVENT_4    = -1613262,
	SAY_NORGANNON_EVENT_8    = -1613263,
	SAY_NORGANNON_DEACTIVATE = -1613264, // Need Implement
	// Archmage Rhydan
	SAY_RHYDIAN_EVENT_6      = -1613265,
	SAY_RHYDIAN_WHISPER      = -1613266,
	// Leviathan Hard Mode + Radio
	SAY_BRANN_1              = -1613267,
	SAY_PENTARUS_1           = -1613268,
	SAY_BRANN_2              = -1613269,
	SAY_PENTARUS_2           = -1613270,
	SAY_BRANN_3              = -1613271,
	SAY_BRANN_4              = -1613272,
	SAY_BRANN_5              = -1613273,
	SAY_BRANN_6              = -1613274,
	SAY_RADIO_TOWER_OF_FLAME = -1613275,
	SAY_RADIO_TOWER_OF_STORM = -1613276,
	SAY_RADIO_TOWER_OF_FROST = -1613277,
	SAY_RADIO_TOWER_OF_LIFE  = -1613278,
	SAY_RADIO_REPAIR_STATION = -1613279,
};

class npc_lore_keeper_of_norgannon : public CreatureScript
{
    public:
        npc_lore_keeper_of_norgannon() : CreatureScript("npc_lore_keeper_of_norgannon") { }

		bool OnGossipHello(Player* player, Creature* creature)
        {
            InstanceScript* instance = creature->GetInstanceScript();
            if (instance->GetData(DATA_FLAME_LEVIATHAN) != DONE && player)
            {
                player->PrepareGossipMenu(creature);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_NORGANNON_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            }
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();

            InstanceScript* instance = creature->GetInstanceScript();

            if (!instance)
                return true;

            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF + 1:
                    if (player)
                    {
                        player->PrepareGossipMenu(creature);
                        instance->instance->LoadGrid(364, -16); // make sure leviathan is loaded

                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_NORGANNON_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                        player->SEND_GOSSIP_MENU(32000, creature->GetGUID());
                    }
                    break;
                case GOSSIP_ACTION_INFO_DEF + 2:
                    if (player)
                        player->SEND_GOSSIP_MENU(32001, creature->GetGUID());

                    creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                    if (Creature* leviathan = instance->instance->GetCreature(instance->GetData64(DATA_FLAME_LEVIATHAN)))
                        leviathan->AI()->DoAction(ACTION_ACTIVATE_HARD_MODE);

                    CAST_AI(npc_lore_keeper_of_norgannonAI, creature->AI())->HardStep = 1;
                    CAST_AI(npc_lore_keeper_of_norgannonAI, creature->AI())->HardTimer = 100;
                    CAST_AI(npc_lore_keeper_of_norgannonAI, creature->AI())->HardMode();
                    break;
            }

            return true;
        }

        struct npc_lore_keeper_of_norgannonAI : public QuantumBasicAI
        {
            npc_lore_keeper_of_norgannonAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = me->GetInstanceScript();
                IntroStep = 0;

                Event = false;
                Greet = false;
            }

			uint32 IntroTimer;
			uint32 IntroStep;
			uint32 HardTimer;
			uint32 HardStep;

			bool Event;
			bool Greet;

			InstanceScript* instance;

			void Reset()
			{
				me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
			}

            void MoveInLineOfSight(Unit* who)
            {
                if (Greet)
                    return;
			
                if (me->IsWithinDistInMap(who, 10.0f) && who->GetTypeId() == TYPE_ID_PLAYER)
                {
                    IntroStep = 1;
                    IntroTimer = 100;
                    Intro();
                    Greet = true;
                }
            }

            void Intro()
            {
                if (!IntroStep)
                    return;

                if (Event)
                    return;

                switch (IntroStep)
                {
                    case 1:
                        if (Creature* dellorah = me->FindCreatureWithDistance(NPC_EXPLORER_DELLORAH, 150.0f))
						{
                            DoSendQuantumText(SAY_DELLORAH_EVENT_1, dellorah);
						}
                        JumpIntro(10000);
                        break;
                    case 2:
                        DoSendQuantumText(SAY_NORGANNON_EVENT_1, me);
                        JumpIntro(10000);
                        break;
                    case 3:
                        if (Creature* dellorah = me->FindCreatureWithDistance(NPC_EXPLORER_DELLORAH, 150.0f))
						{
                            DoSendQuantumText(SAY_DELLORAH_EVENT_2, dellorah);
						}
                        JumpIntro(10000);
                        break;
                    case 4:
                        DoSendQuantumText(SAY_NORGANNON_EVENT_2, me);
                        JumpIntro(10000);
                        break;
                    case 5:
                        if (Creature* dellorah = me->FindCreatureWithDistance(NPC_EXPLORER_DELLORAH, 150.0f))
						{
                            DoSendQuantumText(SAY_DELLORAH_EVENT_3, dellorah);
						}
                        JumpIntro(10000);
                        break;
                    case 6:
                        DoSendQuantumText(SAY_NORGANNON_EVENT_3, me);
                        JumpIntro(10000);
                        break;
                    case 7:
                        DoSendQuantumText(SAY_NORGANNON_EVENT_4, me);
                        JumpIntro(10000);
                        break;
                    case 8:
                        if (Creature* dellorah = me->FindCreatureWithDistance(NPC_EXPLORER_DELLORAH, 150.0f))
						{
                            DoSendQuantumText(SAY_DELLORAH_EVENT_4, dellorah);
						}
                        JumpIntro(10000);
                        break;
                    case 9:
                        if (Creature* rhydian = me->FindCreatureWithDistance(NPC_ARCHMAGE_RHYDIAN, 150.0f))
                        {
                            if (Creature* dellorah = me->FindCreatureWithDistance(NPC_EXPLORER_DELLORAH, 150.0f))
                            {
                                dellorah->SetTarget(rhydian->GetGUID());
                                DoSendQuantumText(SAY_DELLORAH_EVENT_5, dellorah);
                            }
                        }
                        JumpIntro(6000);
                        break;
                    case 10:
                        if (Creature* rhydian = me->FindCreatureWithDistance(NPC_ARCHMAGE_RHYDIAN, 150.0f))
                        {
                            if (Creature* brann = me->FindCreatureWithDistance(NPC_BRANN_BRONZEBEARD, 200.0f, true))
                            {
                                DoSendQuantumText(SAY_RHYDIAN_EVENT_6, rhydian);
                                rhydian->GetMotionMaster()->MovePoint(0, brann->GetPositionX() - 4, brann->GetPositionY(), brann->GetPositionZ());
                            }
                        }
                        JumpIntro(500);
                        break;
                    case 11:
                        if (Creature* nognnanon = me->FindCreatureWithDistance(NPC_LORE_KEEPER_NORGANNON, 150.0f))
                        {
                            if (Creature* dellorah = me->FindCreatureWithDistance(NPC_EXPLORER_DELLORAH, 150.0f))
                            {
                                dellorah->SetTarget(nognnanon->GetGUID());
                                DoSendQuantumText(SAY_DELLORAH_EVENT_7, dellorah);
                            }
                        }
                        JumpIntro(10000);
                        break;
                    case 12:
                        DoSendQuantumText(SAY_NORGANNON_EVENT_8, me);
                        if (Creature* rhydian = me->FindCreatureWithDistance(NPC_ARCHMAGE_RHYDIAN, 200.0f))
                        {
                            if (Creature* brann = me->FindCreatureWithDistance(NPC_BRANN_BRONZEBEARD, 200.0f, true))
                            {
                                rhydian->SetTarget(brann->GetGUID());
                                DoSendQuantumText(SAY_RHYDIAN_WHISPER, rhydian);
                            }
                        }
                        JumpIntro(10000);
                        break;
                    case 13:
                        if (Creature* rhydian = me->FindCreatureWithDistance(NPC_ARCHMAGE_RHYDIAN, 200.0f))
                        {
                            if (Creature* dellorah = me->FindCreatureWithDistance(NPC_EXPLORER_DELLORAH, 200.0f))
                                rhydian->GetMotionMaster()->MovePoint(0, dellorah->GetPositionX() - 2, dellorah->GetPositionY(), dellorah->GetPositionZ());
                        }
                        JumpIntro(10000);
                        break;
                    case 14:
						me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        if (Creature* rhydian = me->FindCreatureWithDistance(NPC_ARCHMAGE_RHYDIAN, 200.0f))
                        {
                            if (Creature* dellorah = me->FindCreatureWithDistance(NPC_EXPLORER_DELLORAH, 200.0f))
                            {
                                rhydian->GetMotionMaster()->MovePoint(0, dellorah->GetPositionX() - 2, dellorah->GetPositionY(), dellorah->GetPositionZ());
                                DoSendQuantumText(SAY_DELLORAH_EVENT_8, dellorah);
                            }
                        }
                        Event = true;
                        break;
                    default:
                        break;
                }
            }

            void HardMode()
            {
				if (!HardStep)
					return;

                Event = true;

                switch (HardStep)
                {
                    case 1:
                        if (Creature* brann = me->FindCreatureWithDistance(NPC_BRANN_BRONZEBEARD, 200.0f, true))
							brann->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                        if (Creature* dellorah = me->FindCreatureWithDistance(NPC_EXPLORER_DELLORAH, 150.0f))
							DoSendQuantumText(SAY_DELLORAH_BRANN_1, dellorah);
                        JumpHard(4000);
                        break;
                    case 2:
                        if (Creature* dellorah = me->FindCreatureWithDistance(NPC_EXPLORER_DELLORAH, 200.0f, true))
						{
                            if (Creature* brann = me->FindCreatureWithDistance(NPC_BRANN_BRONZEBEARD, 200.0f, true))
							{
								dellorah->SetSpeed(MOVE_RUN, 2.0f, true);
                                dellorah->GetMotionMaster()->MovePoint(0, brann->GetPositionX() - 4, brann->GetPositionY(), brann->GetPositionZ());
							}
						}
                        JumpHard(12000);
                        break;
                    case 3:
                        if (Creature* dellorah = me->FindCreatureWithDistance(NPC_EXPLORER_DELLORAH, 200.0f, true))
                        {
                            if (Creature* brann = me->FindCreatureWithDistance(NPC_BRANN_BRONZEBEARD, 200.0f, true))
                            {
                                dellorah->SetTarget(brann->GetGUID());
                                DoSendQuantumText(SAY_DELLORAH_BRANN_2, dellorah);
                            }
                        }
                        JumpHard(5000);
                        break;
                    case 4:
                        if (Creature* brann = me->FindCreatureWithDistance(NPC_BRANN_BRONZEBEARD, 200.0f, true))
                            brann->AI()->DoAction(ACTION_START);

                        if (Creature* dellorah = me->FindCreatureWithDistance(NPC_EXPLORER_DELLORAH, 200.0f, true))
                        {
                            if (Creature* keeper = me->FindCreatureWithDistance(NPC_LORE_KEEPER_NORGANNON, 200.0f, true))
                            {
                                dellorah->GetMotionMaster()->MovePoint(0, keeper->GetPositionX() - 4, keeper->GetPositionY(), keeper->GetPositionZ());
                                dellorah->SetTarget(keeper->GetGUID());
                            }
                        }
                        JumpHard(5000);
                        break;
                    default:
                        break;
                }
            }

            void JumpIntro(uint32 TimeIntro)
            {
                IntroTimer = TimeIntro;
                IntroStep++;
            }

            void JumpHard(uint32 TimeHard)
            {
                HardTimer = TimeHard;
                HardStep++;
            }

            void UpdateAI(uint32 const diff)
            {
                if (IntroTimer < diff)
                   Intro();
                else IntroTimer -= diff;
				   
                if (HardTimer < diff)
                   HardMode();
                else HardTimer -= diff;

                return;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_lore_keeper_of_norgannonAI(creature);
        }
};

class npc_kirin_tor_battle_mage : public CreatureScript
{
public:
    npc_kirin_tor_battle_mage() : CreatureScript("npc_kirin_tor_battle_mage") { }

    struct npc_kirin_tor_battle_mageAI : public QuantumBasicAI
    {
        npc_kirin_tor_battle_mageAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        uint64 TargetGUID;

        void Reset()
        {
            TargetGUID = 0;
        }

        void UpdateAI(const uint32 diff)
        {
            if (me->IsNonMeleeSpellCasted(false))
                return;

            if (me->GetEntry() == NPC_KIRIN_TOR_BATTLE_MAGE)
            {
                if (!TargetGUID)
                {
                    std::list<Creature*> orbList;
                    GetCreatureListWithEntryInGrid(orbList, me, NPC_ULDUAR_SHIELD_BUNNY, 40.0f);
                    if (!orbList.empty())
                    {
                        for (std::list<Creature*>::const_iterator itr = orbList.begin(); itr != orbList.end(); ++itr)
                        {
                            if (Creature* orb = *itr)
                            {
                                if (orb->GetPositionY() < 1000)
                                {
                                    TargetGUID = orb->GetGUID();
                                    orb->SetDisplayId(MODEL_ID_INVISIBLE);
                                    orb->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);                
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                if (!TargetGUID)
				{
                    if (Creature* orb = GetClosestCreatureWithEntry(me, NPC_ULDUAR_SHIELD_BUNNY, 40.0f))
                    {
                        orb->SetDisplayId(MODEL_ID_INVISIBLE);
                        orb->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);                
                        TargetGUID = orb->GetGUID();
                    }
				}
            }

            if (Creature* orb = me->GetCreature(*me, TargetGUID))
			{
                DoCast(orb, SPELL_VISUAL_BEAM);
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_kirin_tor_battle_mageAI(creature);
    }
};

class npc_gauntlet_generator : public CreatureScript
{
public:
    npc_gauntlet_generator() : CreatureScript("npc_gauntlet_generator") { }

    struct npc_gauntlet_generatorAI : public QuantumBasicAI
    {
        npc_gauntlet_generatorAI(Creature* creature) : QuantumBasicAI(creature)
        {
            Step = 0;
        }

		uint32 StepTimer;
        uint32 Step;

        void MoveInLineOfSight(Unit* who)
        {
            if (me->IsWithinDistInMap(who, 100.0f) && who->GetTypeId() == TYPE_ID_PLAYER)
            {
                Step = 1;
                StepTimer = 100;
                Event();
            }
        }

        void Event() // summon 20 dwarfs each beacon
        {
            if (!Step)
                return;

            switch (Step)
            {
                case 1:
                    Spawn();
                    JumpIntro(1000);
                    break;
                case 2:
                    Spawn();
                    JumpIntro(1000);
                    break;
                case 3:
                    Spawn();
                    JumpIntro(1000);
                    break;
                case 4:
                    Spawn();
                    JumpIntro(1000);
                    break;
                case 5:
                    Spawn();
                    JumpIntro(1000);
                    break;
                case 6:
                    Spawn();
                    JumpIntro(1000);
                    break;
                case 7:
                    Spawn();
                    JumpIntro(1000);
                    break;
                case 8:
                    Spawn();
                    JumpIntro(1000);
                    break;
                case 9:
                    Spawn();
                    JumpIntro(1000);
                    break;
                case 10:
                    Spawn();
                    JumpIntro(1000);
                    break;
                case 11:
                    Spawn();
                    JumpIntro(1000);
                    break;
                case 12:
                    Spawn();
                    JumpIntro(1000);
                    break;
                case 13:
                    Spawn();
                    JumpIntro(1000);
                    break;
                case 14:
                    Spawn();
                    JumpIntro(1000);
                    break;
                case 15:
                    Spawn();
                    JumpIntro(1000);
                    break;
                case 16:
                    Spawn();
                    JumpIntro(1000);
                    break;
                case 17:
                    Spawn();
                    JumpIntro(1000);
                    break;
                case 18:
                    Spawn();
                    JumpIntro(1000);
                    break;
                case 19:
                    Spawn();
                    JumpIntro(1000);
                    break;
                case 20:
                    Spawn();
                    JumpIntro(1000);
                    break;
                default:
                    break;
            }
        }

        void Spawn()
        {
            DoCast(me, SPELL_SUMMON_IRON_DWARF_WATCHER);
        }

        void JumpIntro(uint32 Time)
        {
            StepTimer = Time;
            Step++;
        }
			
        void UpdateAI(uint32 const diff)
        {
            if (StepTimer < diff)
               Event();
            else
               StepTimer -= diff;
				
            return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gauntlet_generatorAI(creature);
    }
};

// 15802
// 15804 You've done it! You've broken the defenses of Ulduar. In a few moments, we will be dropping in to...
// 15805 What is that? Be careful! Something's headed your way!
// 15806 Quicly! Evasive action! Evasive act--

class npc_bronzebeard_radio : public CreatureScript
{
public:
    npc_bronzebeard_radio() : CreatureScript("npc_bronzebeard_radio") { }

    struct npc_bronzebeard_radioAI : public QuantumBasicAI
    {
        npc_bronzebeard_radioAI(Creature* creature) : QuantumBasicAI(creature)
        {
			TowerOfFlames = false;
            TowerOfStorm = false;
            TowerOfFrost = false;
            TowerOfLife = false;
            RepairStation = false;
			me->SetDisplayId(MODEL_ID_INVISIBLE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

		bool TowerOfFlames;
        bool TowerOfStorm;
        bool TowerOfFrost;
        bool TowerOfLife;
        bool RepairStation;

        void MoveInLineOfSight(Unit* who)
        {
            // Tower of Flames
            if (!TowerOfFlames && me->IsWithinDistInMap(who, 500.0f) && who->GetTypeId() == TYPE_ID_PLAYER && me->FindGameobjectWithDistance(GO_TOWER_OF_FLAMES, 150.0f))
            {
                TowerOfFlames = true;
				DoSendQuantumText(SAY_RADIO_TOWER_OF_FLAME, me);
            }
            // Tower of Storm
            else if (!TowerOfStorm && me->IsWithinDistInMap(who, 500.0f) && who->GetTypeId() == TYPE_ID_PLAYER && me->FindGameobjectWithDistance(GO_TOWER_OF_STORMS, 150.0f))
            {   
                TowerOfStorm = true;
                DoSendQuantumText(SAY_RADIO_TOWER_OF_STORM, me);
            }
            // Tower of Frost
            else if (!TowerOfFrost && me->IsWithinDistInMap(who, 500.0f) && who->GetTypeId() == TYPE_ID_PLAYER && me->FindGameobjectWithDistance(GO_TOWER_OF_FROST, 150.0f))
            {   
                TowerOfFrost = true;
                DoSendQuantumText(SAY_RADIO_TOWER_OF_FROST, me);
            }
            // Tower of Life
            else if (!TowerOfLife && me->IsWithinDistInMap(who, 500.0f) && who->GetTypeId() == TYPE_ID_PLAYER && me->FindGameobjectWithDistance(GO_TOWER_OF_LIFE, 150.0f))
            {   
                TowerOfLife = true;
                DoSendQuantumText(SAY_RADIO_TOWER_OF_LIFE, me);
            }
            // Repair Station
            else if (!RepairStation && me->IsWithinDistInMap(who, 2.0f) && who->GetTypeId() == TYPE_ID_PLAYER && me->FindGameobjectWithDistance(GO_REPAIR_STATION, 2.0f))
            {   
                RepairStation = true;
                DoSendQuantumText(SAY_RADIO_REPAIR_STATION, me);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bronzebeard_radioAI(creature);
    }
};

class npc_brann_bronzebeard : public CreatureScript
{
public:
    npc_brann_bronzebeard() : CreatureScript("npc_brann_bronzebeard") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BRANN,GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        player->SEND_GOSSIP_MENU(100003, creature->GetGUID());
	    return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF:
			    if (creature->AI())
                    creature->AI()->DoAction(ACTION_START);
                player->CLOSE_GOSSIP_MENU();
                break;
        }
        return true;
	}

    struct npc_brann_bronzebeardAI : public QuantumBasicAI
    {
        npc_brann_bronzebeardAI(Creature* creature) : QuantumBasicAI(creature)
        {
            SteppingBrann = 0;
        }

		uint32 StepTimer;
        uint32 SteppingBrann;

        void EventBrann()
        {
            if (!SteppingBrann)
                return;

            switch (SteppingBrann)
            {
                case 1:
                    DoSendQuantumText(SAY_BRANN_1, me);
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    JumpSteppingBrann(4000);
                    break;
                case 2:
                    if (Unit* pentarus = me->FindCreatureWithDistance(NPC_ARCHMAGE_PENTARUS, 20.0f))
						DoSendQuantumText(SAY_PENTARUS_1, pentarus);

                    JumpSteppingBrann(4000);
                    break;
                case 3:
                    DoSendQuantumText(SAY_BRANN_2, me);
                    JumpSteppingBrann(8000);
                    break;
                case 4:
                    if (Unit* ingener = me->FindCreatureWithDistance(NPC_HIRED_INGENEER, 50.0f))
                        ingener->GetMotionMaster()->MovePoint(0, -777.336f,-45.084f,429.843f);

                    if (Unit* kirintormage = me->FindCreatureWithDistance(NPC_KIRIN_TOR_MAGE, 50.0f))
                    {
                        kirintormage->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                        kirintormage->SetSpeed(MOVE_WALK, 1, true);
                        kirintormage->GetMotionMaster()->MovePoint(0, -682.470520f, -80.405426f, 427.573029f);
                        kirintormage->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
                    }
                    if (Unit* battlemage = me->FindCreatureWithDistance(NPC_KIRIN_TOR_BATTLE_MAGE, 50.0f))
                    {
                        battlemage->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                        battlemage->SetSpeed(MOVE_WALK, 1, true);
                        battlemage->GetMotionMaster()->MovePoint(0, -682.969788f, -75.015648f, 427.604187f);
                        battlemage->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
                    }
                    if (Unit* pentarus = me->FindCreatureWithDistance(NPC_ARCHMAGE_PENTARUS, 20.0f))
                    {
                        pentarus->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                        pentarus->SetSpeed(MOVE_WALK, 1, true);
                        pentarus->GetMotionMaster()->MovePoint(0, -678.194580f, -77.385101f, 426.988464f);
                        pentarus->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
                    }
                    me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                    me->SetSpeed(MOVE_WALK, 1, true);
                    me->GetMotionMaster()->MovePoint(0, -674.219055f, -45.745911f, 426.139160f);
                    JumpSteppingBrann(19000);
                    break;
                case 5:
                    if (Unit* pentarus = me->FindCreatureWithDistance(NPC_ARCHMAGE_PENTARUS, 50.0f))
                        DoSendQuantumText(SAY_PENTARUS_2, pentarus);

                    JumpSteppingBrann(5000);
                    break;
                case 6:
                    DoSendQuantumText(SAY_BRANN_3, me);
                    JumpSteppingBrann(4000);
                    break;
                case 7:
                    if (me->FindGameobjectWithDistance(GO_CUPOLE, 250.0f))
                        me->FindGameobjectWithDistance(GO_CUPOLE, 250.0f)->RemoveFromWorld();
                    
                    if (Creature* dummy = me->FindCreatureWithDistance(NPC_ULDUAR_SHIELD_BUNNY, 250.0f))
                        me->FindCreatureWithDistance(NPC_ULDUAR_SHIELD_BUNNY, 250.0f)->RemoveFromWorld();

                    JumpSteppingBrann(100);
                    break;
                case 8:
                    if (Creature* dummy = me->FindCreatureWithDistance(NPC_ULDUAR_SHIELD_BUNNY, 250.0f))
                        me->FindCreatureWithDistance(NPC_ULDUAR_SHIELD_BUNNY, 250.0f)->RemoveFromWorld();

                    JumpSteppingBrann(7900);
                    break;
                case 9:
                    DoSendQuantumText(SAY_BRANN_4, me);
                    JumpSteppingBrann(10000);
                    break;
                case 10:
                    DoSendQuantumText(SAY_BRANN_5, me);
                    JumpSteppingBrann(8000);
                    break;
                case 11:
                    DoSendQuantumText(SAY_BRANN_6, me);
                    JumpSteppingBrann(8000);
                    break;
                default:
                    break;
            }
        }

        void JumpSteppingBrann(uint32 Time)
        {
            StepTimer = Time;
            SteppingBrann++;
        }

        void DoAction(const int32 id)
        {
            switch(id)
            {
                case ACTION_START:
                    if (Creature* Keeper = me->FindCreatureWithDistance(NPC_LORE_KEEPER_NORGANNON, 200.0f, true))
                        Keeper->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    SteppingBrann = 1;
                    EventBrann();                    
                    break;
                default:
                    break;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (StepTimer < diff)
               EventBrann();
            else
               StepTimer -= diff;
				
            return;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_brann_bronzebeardAI(creature);
	}
};

class npc_ulduar_engineer : public CreatureScript
{
public:
    npc_ulduar_engineer() : CreatureScript("npc_ulduar_engineer") { }

    struct npc_ulduar_engineerAI : public QuantumBasicAI
    {
		npc_ulduar_engineerAI(Creature* creature) : QuantumBasicAI(creature)
        {
			SetCombatMovement(false);
            Step = 0;
            Start = false;
        }

		uint32 StepTimer;
        uint32 Step;
        bool Start;

        void MoveInLineOfSight(Unit* who)
        {
            if (!Start && me->IsWithinDistInMap(who, 10.0f) && who->GetTypeId() == TYPE_ID_PLAYER && me->FindGameobjectWithDistance(GO_TELEPORTER, 15.0f)) // Teleporter check to aviod multiple events in areas
            {
                Step = 1;
                StepTimer = 100;
                Start = true;
                Event();
            }
        }

        void Event()
        {
            if (!Step)
                return;

            switch (Step)
            {
                case 1:
                    if (Unit* mage = me->FindCreatureWithDistance(NPC_KIRIN_TOR_BATTLE_MAGE, 7.0f))
					{
                        mage->MonsterSay(KIRIN_TOR_BATTLE_MAGE_SAY_1, LANG_UNIVERSAL, 0);
					}
                    JumpIntro(3000);
                    break;
                case 2:
                    me->MonsterSay(HIRED_ENGINEER_SAY_1, LANG_UNIVERSAL, 0);
                    JumpIntro(4000);
                    break;
                case 3:
                    if (Unit* mage = me->FindCreatureWithDistance(NPC_KIRIN_TOR_BATTLE_MAGE, 7.0f))
					{
                        mage->MonsterSay(KIRIN_TOR_BATTLE_MAGE_SAY_2, LANG_UNIVERSAL, 0);
					}
                    JumpIntro(7000);
                    break;
                case 4:
                    me->MonsterSay(HIRED_ENGINEER_SAY_2, LANG_UNIVERSAL, 0);
                    JumpIntro(1000);
                    break;
                default:
                    break;
            }
        }

        void JumpIntro(uint32 Time)
        {
            StepTimer = Time;
            Step++;
        }
			
        void UpdateAI(uint32 const diff)
        {
            if (StepTimer <= diff)
			{
               Event();
			}
            else StepTimer -= diff;
				
            return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ulduar_engineerAI(creature);
    }
};

enum KeeperTexts
{
    SAY_MIMIRON_HELP = -1603259,
    SAY_FREYA_HELP   = -1603189,
    SAY_THORIM_HELP  = -1603287,
    SAY_HODIR_HELP   = -1603217,
};

#define GOSSIP_KEEPER_HELP "Lend us your aid, keeper. Together we shall defeat Yogg-Saron."

class npc_keeper_help : public CreatureScript
{
public:
    npc_keeper_help() : CreatureScript("npc_keeper_help") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (InstanceScript* instance = creature->GetInstanceScript())
        {
            uint32 SupportFlag = instance->GetData(DATA_KEEPER_SUPPORT_YOGG);

            switch(creature->GetEntry())
            {
                case NPC_HELP_KEEPER_FREYA:
					if ((SupportFlag & FREYA_SUPPORT) == FREYA_SUPPORT)
						return false;
					break;
				case NPC_HELP_KEEPER_MIMIRON:
					if ((SupportFlag & MIMIRON_SUPPORT) == MIMIRON_SUPPORT)
						return false;
					break;
				case NPC_HELP_KEEPER_THORIM:
					if ((SupportFlag & THORIM_SUPPORT) == THORIM_SUPPORT)
						return false;
					break;
				case NPC_HELP_KEEPER_HODIR:
					if ((SupportFlag & HODIR_SUPPORT) == HODIR_SUPPORT)
						return false;
					break;
			}
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_KEEPER_HELP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
			player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        }
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        player->CLOSE_GOSSIP_MENU();

        InstanceScript* instance = creature->GetInstanceScript();

        if (!instance)
			return false;

        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            switch(creature->GetEntry())
            {
                case NPC_HELP_KEEPER_FREYA:
					DoSendQuantumText(SAY_FREYA_HELP, creature, player);
					creature->CastSpell(creature, SPELL_KEEPER_ACTIVE, true);
					instance->SetData(DATA_ADD_HELP_FLAG, FREYA_SUPPORT);
					break;
				case NPC_HELP_KEEPER_MIMIRON:
					DoSendQuantumText(SAY_MIMIRON_HELP, creature, player);
					creature->CastSpell(creature, SPELL_KEEPER_ACTIVE, true);
					instance->SetData(DATA_ADD_HELP_FLAG, MIMIRON_SUPPORT);
					break;
				case NPC_HELP_KEEPER_THORIM:
					DoSendQuantumText(SAY_THORIM_HELP, creature, player);
					creature->CastSpell(creature, SPELL_KEEPER_ACTIVE, true);
					instance->SetData(DATA_ADD_HELP_FLAG, THORIM_SUPPORT);
					break;
				case NPC_HELP_KEEPER_HODIR:
					DoSendQuantumText(SAY_HODIR_HELP, creature, player);
					creature->CastSpell(creature, SPELL_KEEPER_ACTIVE, true);
					instance->SetData(DATA_ADD_HELP_FLAG, HODIR_SUPPORT);
					break;
            }
        }
        return true;
    }

    struct npc_keeper_helpAI : public QuantumBasicAI
    {
        npc_keeper_helpAI(Creature* creature) : QuantumBasicAI(creature)
        {
			SetCombatMovement(false);

            instance = creature->GetInstanceScript();

            me->SetCurrentFaction(FACTION_FRIENDLY);
        }

        InstanceScript* instance;

        void AttackStart(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
			if (instance->GetBossState(DATA_YOGG_SARON) == IN_PROGRESS)
			{
				me->SetVisible(false);
				return;
			}

			switch(me->GetEntry())
			{
                case NPC_HELP_KEEPER_FREYA:
					me->SetVisible(instance->GetBossState(DATA_FREYA) == DONE);
					break;
                case NPC_HELP_KEEPER_MIMIRON:
					me->SetVisible(instance->GetBossState(DATA_MIMIRON) == DONE);
                    break;
                case NPC_HELP_KEEPER_THORIM:
                    me->SetVisible(instance->GetBossState(DATA_THORIM) == DONE);
                    break;
                case NPC_HELP_KEEPER_HODIR:
                    me->SetVisible(instance->GetBossState(DATA_HODIR) == DONE);
                    break;
			}
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_keeper_helpAI(creature);
    }
};

void AddSC_ulduar()
{
    new npc_gauntlet_generator();
    new npc_lore_keeper_of_norgannon();
    new npc_kirin_tor_battle_mage();
    new npc_bronzebeard_radio();
    new npc_brann_bronzebeard();
    new npc_ulduar_engineer();
	new npc_keeper_help();
}