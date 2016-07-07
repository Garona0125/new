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

uint32 const PossibleSpawns[32] =
{
	17322, 17661, 17496, 17522,
	17340, 17352, 17333, 17524,
	17654, 17348, 17339, 17345,
	17359, 17353, 17336, 17550,
	17330, 17701, 17321, 17680,
	17325, 17320, 17683, 17342,
	17715, 17334, 17341, 17338,
	17337, 17346, 17344, 17327
};

enum Webbed
{
	NPC_EXPEDITION_RESEARCHER = 17681,
};

class mob_webbed_creature : public CreatureScript
{
public:
    mob_webbed_creature() : CreatureScript("mob_webbed_creature") { }

    struct mob_webbed_creatureAI : public QuantumBasicAI
    {
        mob_webbed_creatureAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset() {}

        void EnterToBattle(Unit* /*who*/) {}

        void JustDied(Unit* killer)
        {
            uint32 SpawnCreatureID = 0;

            switch (urand(0, 2))
            {
                case 0:
                    SpawnCreatureID = 17681;
                    if (Player* player = killer->ToPlayer())
                        player->KilledMonsterCredit(SpawnCreatureID, 0);
                    break;
                case 1:
                case 2:
                    SpawnCreatureID = PossibleSpawns[urand(0, 30)];
                    break;
            }

            if (SpawnCreatureID)
                me->SummonCreature(SpawnCreatureID, 0.0f, 0.0f, 0.0f, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_webbed_creatureAI (creature);
    }
};

#define GOSSIP_HELLO_CSA   "[PH] "
#define GOSSIP_SELECT_CSA1 "[PH] "
#define GOSSIP_SELECT_CSA2 "[PH] "
#define GOSSIP_SELECT_CSA3 "[PH] "
#define GOSSIP_SELECT_CSA4 "[PH] "
#define GOSSIP_SELECT_CSA5 "[PH] "

enum SunhawkAgent
{
	SPELL_EXARCHS_ENCHANTMENT = 31609,

	NPC_SUNHAWK_TRIGGER       = 17974,

	QUEST_WHAT_WE_DONT_KNOW   = 9756,
};

class npc_captured_sunhawk_agent : public CreatureScript
{
public:
    npc_captured_sunhawk_agent() : CreatureScript("npc_captured_sunhawk_agent") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->HasAura(SPELL_EXARCHS_ENCHANTMENT) && player->GetQuestStatus(QUEST_WHAT_WE_DONT_KNOW) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_CSA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(9136, creature->GetGUID());
        }
        else
			player->SEND_GOSSIP_MENU(9134, creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT_CSA1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(9137, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT_CSA2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
                player->SEND_GOSSIP_MENU(9138, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT_CSA3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
                player->SEND_GOSSIP_MENU(9139, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT_CSA4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
                player->SEND_GOSSIP_MENU(9140, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+5:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT_CSA5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
                player->SEND_GOSSIP_MENU(9141, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+6:
                player->CLOSE_GOSSIP_MENU();
                player->TalkedToCreature(NPC_SUNHAWK_TRIGGER, creature->GetGUID());
                break;
        }
        return true;
    }
};

enum Stillpine
{
	SAY_DIRECTION                           = -1800074,
	SPELL_OPENING_PRINCESS_STILLPINE_CREDIT = 31003,
    NPC_PRINCESS_STILLPINE                  = 17682,
    GO_PRINCESS_STILLPINES_CAGE             = 181928,
	QUEST_SAVING_PRINCESS_STILLPINE         = 9667,
};

class go_princess_stillpines_cage : public GameObjectScript
{
public:
    go_princess_stillpines_cage() : GameObjectScript("go_princess_stillpines_cage") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (Creature* stillpine = go->FindCreatureWithDistance(NPC_PRINCESS_STILLPINE, 25, true))
        {
            go->SetGoState(GO_STATE_ACTIVE);
            stillpine->GetMotionMaster()->MovePoint(1, go->GetPositionX(), go->GetPositionY()-15, go->GetPositionZ());
            player->CastedCreatureOrGO(NPC_PRINCESS_STILLPINE, 0, SPELL_OPENING_PRINCESS_STILLPINE_CREDIT);
        }
        return true;
    }
};

class npc_princess_stillpine : public CreatureScript
{
public:
    npc_princess_stillpine() : CreatureScript("npc_princess_stillpine") { }

    struct npc_princess_stillpineAI : public QuantumBasicAI
    {
        npc_princess_stillpineAI(Creature* creature) : QuantumBasicAI(creature) {}

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE && id == 1)
            {
                DoSendQuantumText(SAY_DIRECTION, me);
                me->DespawnAfterAction();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_princess_stillpineAI(creature);
    }
};

enum QuestRedemption
{
	SPELL_SYMBOL_OF_LIFE    = 8593,
	SPELL_FEIGN_DEATH       = 29266,
	NPC_FULBORG_KILL_CREDIT = 17542,
};

class npc_young_furbolg_shaman : public CreatureScript
{
public:
    npc_young_furbolg_shaman() : CreatureScript("npc_young_furbolg_shaman") {}

    struct npc_young_furbolg_shamanAI : public QuantumBasicAI
    {
        npc_young_furbolg_shamanAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_FEIGN_DEATH);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FEIGN_DEATH | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_SYMBOL_OF_LIFE)
            {
				if (Player* player = caster->ToPlayer())
					caster->ToPlayer()->KilledMonsterCredit(NPC_FULBORG_KILL_CREDIT, 0);

				me->DespawnAfterAction(6*IN_MILLISECONDS);
			}
        }

        void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_young_furbolg_shamanAI(creature);
    }
};

enum QuestGalaensFate
{
	QUEST_GALAENS_FATE  = 9579,
	NPC_GALAEN          = 17426,
	ACTION_GALAENS_FATE = 1,

	SAY_GALAENS_FATE_1  = -1420109,
	SAY_GALAENS_FATE_2  = -1420110,
	SAY_GALAENS_FATE_3  = -1420111,

	EVENT_GALAENS_FATE_1 = 1,
	EVENT_GALAENS_FATE_2 = 2,
	EVENT_GALAENS_FATE_3 = 3,
	EVENT_GALAENS_FATE_4 = 4,
	EVENT_GALAENS_FATE_5 = 5,
};

class npc_galaens_corpse : public CreatureScript
{
public:
    npc_galaens_corpse() : CreatureScript("npc_galaens_corpse") { }

	bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
	{
		if (quest->GetQuestId() == QUEST_GALAENS_FATE)
		{
			creature->AI()->DoAction(ACTION_GALAENS_FATE);
			creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUEST_GIVER);
		}
		return true;
	}

	struct npc_galaens_corpseAI : public QuantumBasicAI
    {
		npc_galaens_corpseAI(Creature* creature) : QuantumBasicAI(creature){}

		EventMap events;

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUEST_GIVER);
        }

		void DoAction(int32 const action)
		{
			switch (action)
			{
			    case ACTION_GALAENS_FATE:
					events.ScheduleEvent(EVENT_GALAENS_FATE_1, 1*IN_MILLISECONDS);
					break;
			}
		}

		void UpdateAI(uint32 const diff)
		{
			// Out of Combat Timer
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
				    case EVENT_GALAENS_FATE_1:
						me->SummonCreature(NPC_GALAEN, -2095.47f, -11295.8f, 63.4973f, 4.3274f, TEMPSUMMON_MANUAL_DESPAWN);
						events.ScheduleEvent(EVENT_GALAENS_FATE_2, 2*IN_MILLISECONDS);
						break;
					case EVENT_GALAENS_FATE_2:
						if (Creature* galaen = me->FindCreatureWithDistance(NPC_GALAEN, 35.0f))
							DoSendQuantumText(SAY_GALAENS_FATE_1, galaen);
						events.ScheduleEvent(EVENT_GALAENS_FATE_3, 4*IN_MILLISECONDS);
						break;
					case EVENT_GALAENS_FATE_3:
						if (Creature* galaen = me->FindCreatureWithDistance(NPC_GALAEN, 35.0f))
							DoSendQuantumText(SAY_GALAENS_FATE_2, galaen);
						events.ScheduleEvent(EVENT_GALAENS_FATE_4, 4*IN_MILLISECONDS);
						break;
					case EVENT_GALAENS_FATE_4:
						if (Creature* galaen = me->FindCreatureWithDistance(NPC_GALAEN, 35.0f))
							DoSendQuantumText(SAY_GALAENS_FATE_3, galaen);
						events.ScheduleEvent(EVENT_GALAENS_FATE_5, 4*IN_MILLISECONDS);
						break;
					case EVENT_GALAENS_FATE_5:
						if (Creature* galaen = me->FindCreatureWithDistance(NPC_GALAEN, 35.0f))
							galaen->DespawnAfterAction(1*IN_MILLISECONDS);
						me->AI()->Reset();
						break;
				}
			}
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_galaens_corpseAI(creature);
    }
};

void AddSC_bloodmyst_isle()
{
    new mob_webbed_creature();
    new npc_captured_sunhawk_agent();
    new npc_princess_stillpine();
    new go_princess_stillpines_cage();
	new npc_young_furbolg_shaman();
	new npc_galaens_corpse();
}