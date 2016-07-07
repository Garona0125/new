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

#define GOSSIP_HELLO_KW   "Tell me what drives this vengance?"
#define GOSSIP_SELECT_KW1 "Continue please"
#define GOSSIP_SELECT_KW2 "Let me confer with my colleagues"

enum SearingGorge
{
	QUEST_DIVINE_RETRIBUTION       = 3441,
	QUEST_ATTUNEMENT_TO_THE_CORE_1 = 7487,
	QUEST_ATTUNEMENT_TO_THE_CORE_2 = 7848,
	QUEST_PRAYER_TO_ELUNE          = 3377,
};

class npc_kalaran_windblade : public CreatureScript
{
public:
    npc_kalaran_windblade() : CreatureScript("npc_kalaran_windblade") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_DIVINE_RETRIBUTION) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_KW, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT_KW1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(1954, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT_KW2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(1955, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->CLOSE_GOSSIP_MENU();
                player->AreaExploredOrEventHappens(3441);
                break;
        }
        return true;
    }
};

#define GOSSIP_HELLO_LR "Teleport me to the Molten Core"

class npc_lothos_riftwaker : public CreatureScript
{
public:
    npc_lothos_riftwaker() : CreatureScript("npc_lothos_riftwaker") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestRewardStatus(QUEST_ATTUNEMENT_TO_THE_CORE_1) || player->GetQuestRewardStatus(QUEST_ATTUNEMENT_TO_THE_CORE_2))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_LR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(409, 1096, -467, -104.6f, 3.64f);
        }
        return true;
    }
};

#define GOSSIP_HELLO_ZL   "Tell me your story"
#define GOSSIP_SELECT_ZL1 "Please continue..."
#define GOSSIP_SELECT_ZL2 "Goodbye"

class npc_zamael_lunthistle : public CreatureScript
{
public:
    npc_zamael_lunthistle() : CreatureScript("npc_zamael_lunthistle") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_PRAYER_TO_ELUNE) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_ZL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(1920, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT_ZL1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(1921, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT_ZL2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(1922, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->CLOSE_GOSSIP_MENU();
                player->AreaExploredOrEventHappens(3377);
                break;
        }
        return true;
    }
};

void AddSC_searing_gorge()
{
    new npc_kalaran_windblade();
    new npc_lothos_riftwaker();
    new npc_zamael_lunthistle();
}