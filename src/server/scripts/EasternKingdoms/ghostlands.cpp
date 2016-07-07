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
#include "QuantumSystemText.h"

#define GOSSIP_H_BKD "Take Blood Knight Insignia"

class npc_blood_knight_dawnstar : public CreatureScript
{
public:
    npc_blood_knight_dawnstar() : CreatureScript("npc_blood_knight_dawnstar") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(9692) == QUEST_STATUS_INCOMPLETE && !player->HasItemCount(24226, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_H_BKD, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            ItemPosCountVec dest;
            uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 24226, 1, NULL);
            if (msg == EQUIP_ERR_OK)
            {
                player->StoreNewItem(dest, 24226, 1, true);
                player->PlayerTalkClass->ClearMenus();
            }
        }
        return true;
    }
};

#define GOSSIP_HBN "You gave the crew disguises?"

class npc_budd_nedreck : public CreatureScript
{
public:
    npc_budd_nedreck() : CreatureScript("npc_budd_nedreck") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
		if (creature->IsQuestGiver())
			player->PrepareQuestMenu(creature->GetGUID());
		
		if (player->GetQuestStatus(11132) == QUEST_STATUS_COMPLETE && player->GetQuestStatus(11166) == QUEST_STATUS_INCOMPLETE)
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HBN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->CastSpell(player, 42540, false);
        }
        return true;
    }
};

class npc_rathis_tomber : public CreatureScript
{
public:
    npc_rathis_tomber() : CreatureScript("npc_rathis_tomber") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->IsVendor() && player->GetQuestRewardStatus(9152))
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_VENDOR_BROWSE_YOUR_GOODS), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
            player->SEND_GOSSIP_MENU(8432, creature->GetGUID());
        }
		else
        player->SEND_GOSSIP_MENU(8431, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_TRADE)
            player->GetSession()->SendListInventory(creature->GetGUID());

        return true;
    }
};

enum Ranger
{
	SAY_START                       = 0,
	SAY_PROGRESS1                   = 1,
	SAY_PROGRESS2                   = 2,
	SAY_PROGRESS3                   = 3,
	SAY_END1                        = 4,
	SAY_END2                        = 5,
	SAY_CAPTAIN_ANSWER              = 0,

    QUEST_ESCAPE_FROM_THE_CATACOMBS = 9212,
    GO_CAGE                         = 181152,
    NPC_CAPTAIN_HELIOS              = 16220,
    FACTION_SMOON_E                 = 1603,
};

class npc_ranger_lilatha : public CreatureScript
{
public:
    npc_ranger_lilatha() : CreatureScript("npc_ranger_lilatha") { }

    struct npc_ranger_lilathaAI : public npc_escortAI
    {
        npc_ranger_lilathaAI(Creature* creature) : npc_escortAI(creature) {}

        void WaypointReached(uint32 id)
        {
            Player* player = GetPlayerForEscort();

            if (!player)
                return;

            switch (id)
            {
            case 0:
			{
				me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);

				if (GameObject* cage = me->FindGameobjectWithDistance(GO_CAGE, 20.0f))
					cage->SetGoState(GO_STATE_ACTIVE);

				Talk(SAY_START, player->GetGUID());
				break;
			}
			case 5:
				Talk(SAY_PROGRESS1, player->GetGUID());
            case 11:
                Talk(SAY_PROGRESS2, player->GetGUID());
                me->SetOrientation(4.762841f);
                break;
            case 18:
			{
				Talk(SAY_PROGRESS3, player->GetGUID());
				Creature* Summ1 = me->SummonCreature(16342, 7627.083984f, -7532.538086f, 152.128616f, 1.082733f, TEMPSUMMON_DEAD_DESPAWN, 0);
				Creature* Summ2 = me->SummonCreature(16343, 7620.432129f, -7532.550293f, 152.454865f, 0.827478f, TEMPSUMMON_DEAD_DESPAWN, 0);
				if (Summ1 && Summ2)
				{
					Summ1->Attack(me, true);
					Summ2->Attack(player, true);
				}
				me->AI()->AttackStart(Summ1);
				break;
			}
			case 19:
				me->SetWalk(false);
				break;
            case 25:
				me->SetWalk(true);
				break;
            case 30:
                if (player && player->GetTypeId() == TYPE_ID_PLAYER)
                    CAST_PLR(player)->GroupEventHappens(QUEST_ESCAPE_FROM_THE_CATACOMBS, me);
                break;
            case 32:
                me->SetOrientation(2.978281f);
				Talk(SAY_END1, player->GetGUID());
                break;
            case 33:
                me->SetOrientation(5.858011f);
				Talk(SAY_END2, player->GetGUID());
				Creature* CaptainHelios = me->FindCreatureWithDistance(NPC_CAPTAIN_HELIOS, 50.0f);
                if (CaptainHelios)
					CaptainHelios->AI()->Talk(SAY_CAPTAIN_ANSWER, player->GetGUID());
				break;
            }
        }

        void Reset()
        {
            if (GameObject* Cage = me->FindGameobjectWithDistance(GO_CAGE, 20.0f))
                Cage->SetGoState(GO_STATE_READY);
        }
    };

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_ESCAPE_FROM_THE_CATACOMBS)
        {
            creature->SetCurrentFaction(113);

            if (npc_escortAI* EscortAI = CAST_AI(npc_ranger_lilatha::npc_ranger_lilathaAI, creature->AI()))
                EscortAI->Start(true, false, player->GetGUID());
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ranger_lilathaAI(creature);
    }
};

void AddSC_ghostlands()
{
    new npc_blood_knight_dawnstar();
    new npc_budd_nedreck();
    new npc_rathis_tomber();
    new npc_ranger_lilatha();
}