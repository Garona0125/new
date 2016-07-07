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
#include "sunwell_plateau.h"

enum ProphetSpeeches
{
    PROPHET_SAY1 = -1580099,
    PROPHET_SAY2 = -1580100,
    PROPHET_SAY3 = -1580101,
    PROPHET_SAY4 = -1580102,
    PROPHET_SAY5 = -1580103,
    PROPHET_SAY6 = -1580104,
    PROPHET_SAY7 = -1580105,
    PROPHET_SAY8 = -1580106
};

enum LiadrinnSpeeches
{
    LIADRIN_SAY1 = -1580107,
    LIADRIN_SAY2 = -1580108,
    LIADRIN_SAY3 = -1580109
};

#define CS_GOSSIP1 "Give me a situation report, Captain."
#define CS_GOSSIP2 "What went wrong?"
#define CS_GOSSIP3 "Why did they stop?"
#define CS_GOSSIP4 "Your insight is appreciated."

enum Says
{
	SAY_QUELDELAR_1  = -1380561, 
    SAY_QUELDELAR_2  = -1380562,
    SAY_QUELDELAR_3  = -1380563,
    SAY_QUELDELAR_4  = -1380564,
    SAY_QUELDELAR_5  = -1380565,
    SAY_QUELDELAR_6  = -1380566,
    SAY_QUELDELAR_7  = -1380567,
    SAY_QUELDELAR_8  = -1380568,
    SAY_QUELDELAR_9  = -1380569,
    SAY_QUELDELAR_10 = -1380570,
    SAY_QUELDELAR_11 = -1380571,
    SAY_QUELDELAR_12 = -1380572,
};

class npc_chamberlain_galiros : public CreatureScript
{
public:
    npc_chamberlain_galiros() : CreatureScript("npc_chamberlain_galiros") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		player->PrepareGossipMenu(creature, 0);

		if (player->HasItemCount(ITEM_TAINTED_QUELDELAR_1, 1) || player->HasItemCount(ITEM_TAINTED_QUELDELAR_2, 1))
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "He brought Quel'delar.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

		player->SendPreparedGossip(creature);
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		switch(action)
		{
            case GOSSIP_ACTION_INFO_DEF+1:
				player->CLOSE_GOSSIP_MENU();
				creature->AI()->SetGUID(player->GetGUID());
				creature->AI()->DoAction(1);
				break;
			default:
				return false;                                  
		}
		return true;                                          
	}

    struct npc_chamberlain_galirosAI : public QuantumBasicAI
    {
        npc_chamberlain_galirosAI(Creature* creature) : QuantumBasicAI(creature) {}

        EventMap events;

        uint64 Rommath;
        uint64 Theron;
        uint64 Auric;
        uint64 QuelDelar;
        uint64 uiPlayer;
		uint64 PlayerGUID;

        bool StartQuest;
        
        void Reset()
        {
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            StartQuest = true;
            events.Reset();
        }

		void SetGUID(const uint64 &guid, int32 /*iId*/)
        {
            uiPlayer = guid;
        }

        void DoAction(const int32 actionId)
        {
            switch(actionId)
            {
                case 1:
					if (StartQuest)
                    {
						me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
						events.ScheduleEvent(1, 0);
						StartQuest = false;
					}
					break;
			}
        }

        void UpdateAI(const uint32 diff)
        {
			events.Update(diff);

            switch(events.ExecuteEvent())
            {
                case 1:
                    if (Creature* rommath = me->FindCreatureWithDistance(NPC_GRAND_MASTER_ROMMATH, 50.0f, true))  
                    {
                        rommath->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
                        Rommath = rommath->GetGUID();
                    }
                    if (Creature* theron = me->FindCreatureWithDistance(NPC_LORTHEMAR_THERON, 50.0f, true))
                    {
                        theron->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
                        Theron = theron->GetGUID();
                    }
                    if (Creature* auric = me->FindCreatureWithDistance(NPC_CAPTAIN_AURIC_SUNCHASER, 50.0f, true))
                    {
                        auric->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
                        Auric = auric->GetGUID();
                    }
                    if (GameObject* gQuelDelar = me->SummonGameObject(GO_QUEL_DELAR, 1683.99f, 620.231f, 29.3599f, 0.410932f, 0, 0, 0, 0, 0))
                    {
                        QuelDelar = gQuelDelar->GetGUID();
                        gQuelDelar->SetFlag(GAMEOBJECT_FLAGS, 5);
                    }
                    if (Player* player = me->GetPlayer(*me, uiPlayer))
                    {
                        player->DestroyItemCount(ITEM_TAINTED_QUELDELAR_1, 1, true);
                        player->DestroyItemCount(ITEM_TAINTED_QUELDELAR_2, 1, true);
                        DoSendQuantumText(SAY_QUELDELAR_1, me, player);
                    }
                    events.ScheduleEvent(2, 2000);
                    break;
                case 2:
                    if (Creature* guard = me->FindCreatureWithDistance(NPC_SUNWELL_HONOR_GUARD, 20.0f, true))
                        DoSendQuantumText(SAY_QUELDELAR_2, guard);
                    events.ScheduleEvent(3, 1000);
                    break;
                case 3:
                    if (Creature* theron = me->GetCreature(*me, Theron))
                        DoSendQuantumText(SAY_QUELDELAR_3, theron);
                    events.ScheduleEvent(4, 4000);
                    break;
                case 4:
                    if (Creature* rommath = me->GetCreature(*me, Rommath))
                        rommath->GetMotionMaster()->MovePoint(1, 1675.8f, 617.19f, 28.0504f);
                    if (Creature* auric = me->GetCreature(*me, Auric))
                        auric->GetMotionMaster()->MovePoint(1, 1681.77f, 612.084f, 28.4409f);
                    events.ScheduleEvent(5, 6000);
                    break;
                case 5:
                    if (Creature* rommath = me->GetCreature(*me, Rommath))
                    {
                        rommath->SetOrientation(0.3308f);
                        DoSendQuantumText(SAY_QUELDELAR_4, rommath);
                    }
					if (Creature* auric = me->GetCreature(*me, Auric))
                        auric->SetOrientation(1.29057f);
                    if (Creature* theron = me->GetCreature(*me, Theron))
                        theron->GetMotionMaster()->MovePoint(1, 1677.07f, 613.122f, 28.0504f);
                    events.ScheduleEvent(6, 10000);
                    break;
                case 6:
                    if (Creature* theron = me->GetCreature(*me, Theron))
                    {
                        if (Player* player = me->GetPlayer(*me, uiPlayer))
                            DoSendQuantumText(SAY_QUELDELAR_5, theron, player);
                        theron->GetMotionMaster()->MovePoint(1, 1682.3f, 618.459f, 27.9581f);
                    }
                    events.ScheduleEvent(7, 4000);
                    break;
                case 7:
                    if (Creature* theron = me->GetCreature(*me, Theron))
                        theron->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                    events.ScheduleEvent(8, 800);
                    break;
                case 8:
                    if (Creature* theron = me->GetCreature(*me, Theron))
                        theron->CastSpell(theron, SPELL_QUEL_DELAR_WRATH, true);
                    events.ScheduleEvent(9, 1000);
                    break;
                case 9:
                    if (Creature* rommath = me->GetCreature(*me, Rommath))
                    {
                        if (Player* player = me->GetPlayer(*me, uiPlayer))
                            rommath->CastSpell(player, SPELL_ICY_PRISON, true);

                        DoSendQuantumText(SAY_QUELDELAR_6, rommath);
                    }
                    if (Creature* guard = me->FindCreatureWithDistance(NPC_SUNWELL_HONOR_GUARD, 20.0f))
                    {
                        guard->GetMotionMaster()->MovePoint(0, 1681.1f, 614.955f, 28.4983f);
                        guard->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
                    }
                    events.ScheduleEvent(10, 3000);
                    break;
                case 10:
                    if (Creature* guard = me->FindCreatureWithDistance(NPC_SUNWELL_HONOR_GUARD, 20.0f))
                        DoSendQuantumText(SAY_QUELDELAR_7, guard);
                    events.ScheduleEvent(11, 2000);
                    break;
                case 11:
                    if (Creature* auric = me->GetCreature(*me, Auric))
                        DoSendQuantumText(SAY_QUELDELAR_8, auric);
                    events.ScheduleEvent(12, 6000);
                    break;
                case 12:
                    if (Creature* auric = me->GetCreature(*me, Auric))
                        DoSendQuantumText(SAY_QUELDELAR_9, auric);
                    events.ScheduleEvent(13, 5000);
                    break;
                case 13:
                    if (Creature* rommath = me->GetCreature(*me, Rommath))
                        DoSendQuantumText(SAY_QUELDELAR_10, rommath);
                    events.ScheduleEvent(14, 2000);
                    break;
                case 14:
                    if (Creature* guard = me->FindCreatureWithDistance(NPC_SUNWELL_HONOR_GUARD, 20.0f))
                    {
						guard->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_STAND);
                        guard->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
                        guard->GetMotionMaster()->MovePoint(0, guard->GetHomePosition());
                    }
                    if (Creature* rommath = me->GetCreature(*me, Rommath))
                    {
                        rommath->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                        if (Player* player = me->GetPlayer(*me, uiPlayer))
                            DoSendQuantumText(SAY_QUELDELAR_11, rommath, player);
                    }
                    events.ScheduleEvent(15, 7000);
                    break;
                case 15:
                    if (Creature* auric = me->GetCreature(*me, Auric))
                    {
                        auric->HandleEmoteCommand(EMOTE_ONESHOT_POINT);

                        if (Player* player = me->GetPlayer(*me, uiPlayer))
                            DoSendQuantumText(SAY_QUELDELAR_12, auric, player);

                        if (GameObject* gQuelDelar = me->FindGameobjectWithDistance(GO_QUEL_DELAR, 20.0f))
                            gQuelDelar->RemoveFlag(GAMEOBJECT_FLAGS, 5);
                    }
                    events.ScheduleEvent(16, 2000);
                    break;
                case 16:
					if (Creature* auric = me->GetCreature(*me, Auric))
                         auric->GetMotionMaster()->MovePoint(0, auric->GetHomePosition());

					if (Creature* rommath = me->GetCreature(*me, Rommath))
						rommath->GetMotionMaster()->MovePoint(0, rommath->GetHomePosition());

					if (Creature* theron = me->GetCreature(*me, Theron))
						theron->GetMotionMaster()->MovePoint(0, theron->GetHomePosition());
					break;
              }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_chamberlain_galirosAI(creature);
    }
};

class item_tainted_queldelar : public ItemScript
{
public:
	item_tainted_queldelar() : ItemScript("item_tainted_queldelar") { }

	bool OnUse(Player* player, Item* item, SpellCastTargets const& /*targets*/)
	{
		InstanceScript* instance = player->GetInstanceScript();

		if (instance && player->FindCreatureWithDistance(NPC_SUNWELL_CASTER_BUNNY, 20.0f, true))
		{
			Creature* Introducer = NULL;
			Introducer = Unit::GetCreature((*player), instance->GetData64(DATA_QUELDELAR_INTRODUCER));
			Introducer->AI()->SetGUID(player->GetGUID());
			Introducer->AI()->DoAction(1);
			return true;
		}
		else
			return false;
	}
};

void AddSC_sunwell_plateau()
{
    new npc_chamberlain_galiros();
    new item_tainted_queldelar();
}