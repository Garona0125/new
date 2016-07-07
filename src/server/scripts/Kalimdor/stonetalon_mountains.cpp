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

#define GOSSIP_HBD1 "Ysera"
#define GOSSIP_HBD2 "Neltharion"
#define GOSSIP_HBD3 "Nozdormu"
#define GOSSIP_HBD4 "Alexstrasza"
#define GOSSIP_HBD5 "Malygos"

enum BraugDimspirit
{
	SPELL_TEST_OF_LORE = 6766,

	QUEST_TEST_OF_LORE = 6627,
};

class npc_braug_dimspirit : public CreatureScript
{
public:
    npc_braug_dimspirit() : CreatureScript("npc_braug_dimspirit") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_TEST_OF_LORE) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HBD1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HBD2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HBD3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HBD4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HBD5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

            player->SEND_GOSSIP_MENU(5820, creature->GetGUID());
        }
        else
            player->SEND_GOSSIP_MENU(5819, creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->CastSpell(player, SPELL_TEST_OF_LORE, false);

        }

        if (action == GOSSIP_ACTION_INFO_DEF+2)
        {
            player->CLOSE_GOSSIP_MENU();
            player->AreaExploredOrEventHappens(QUEST_TEST_OF_LORE);
        }
        return true;
    }
};

enum Kaya
{
    FACTION_ESCORTEE_H      = 775,
    NPC_GRIMTOTEM_RUFFIAN   = 11910,
    NPC_GRIMTOTEM_BRUTE     = 11912,
    NPC_GRIMTOTEM_SORCERER  = 11913,
    SAY_START               = -1000357,
    SAY_AMBUSH              = -1000358,
    SAY_END                 = -1000359,
    QUEST_PROTECT_KAYA      = 6523,
};

class npc_kaya_flathoof : public CreatureScript
{
public:
    npc_kaya_flathoof() : CreatureScript("npc_kaya_flathoof") { }

    struct npc_kaya_flathoofAI : public npc_escortAI
    {
        npc_kaya_flathoofAI(Creature* creature) : npc_escortAI(creature) {}

        void WaypointReached(uint32 i)
        {
            Player* player = GetPlayerForEscort();

            if (!player)
                return;

            switch (i)
            {
            case 16:
                DoSendQuantumText(SAY_AMBUSH, me);
                me->SummonCreature(NPC_GRIMTOTEM_BRUTE, -48.53f, -503.34f, -46.31f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                me->SummonCreature(NPC_GRIMTOTEM_RUFFIAN, -38.85f, -503.77f, -45.90f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                me->SummonCreature(NPC_GRIMTOTEM_SORCERER, -36.37f, -496.23f, -45.71f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                break;
            case 18: me->SetInFront(player);
                DoSendQuantumText(SAY_END, me, player);
                if (player)
                    player->GroupEventHappens(QUEST_PROTECT_KAYA, me);
                break;
            }
        }

        void JustSummoned(Creature* summoned)
        {
            summoned->AI()->AttackStart(me);
        }

        void Reset(){}
    };

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_PROTECT_KAYA)
        {
            if (npc_escortAI* EscortAI = CAST_AI(npc_kaya_flathoof::npc_kaya_flathoofAI, creature->AI()))
                EscortAI->Start(true, false, player->GetGUID());

            DoSendQuantumText(SAY_START, creature);
            creature->SetCurrentFaction(113);
            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_kaya_flathoofAI(creature);
    }
};

void AddSC_stonetalon_mountains()
{
    new npc_braug_dimspirit();
    new npc_kaya_flathoof();
}