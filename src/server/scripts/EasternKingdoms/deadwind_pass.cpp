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

enum KarazhanQuestRings
{
    QUEST_ARCHMAGE    = 10725,
    QUEST_PROTECTOR   = 10728,
    QUEST_RESTORER    = 10726,
    QUEST_ASSASSIN    = 10727,

    QUEST_W_ARCHMAGE  = 11031,
    QUEST_W_PROTECTOR = 11032,
    QUEST_W_RESTORER  = 11034,
    QUEST_W_ASSASSIN  = 11033,
};

enum KarazhanRings
{
    RING_ARCHMAGE  = 29287,
    RING_PROTECTOR = 29279,
    RING_RESTORER  = 29290,
    RING_ASSASSIN  = 29283,
};

#define GOSSIP_TEXT "I have lost my ring. Can you enchant him again?"

bool AddItemToPlayerRing(Player* player, uint32 ItemEntry)
{
	ItemPosCountVec dest;
	uint8 canStoreNewItem = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ItemEntry, 1);
	if (canStoreNewItem != EQUIP_ERR_OK)
		return false;

	Item *newItem = NULL;
	newItem = player->StoreNewItem(dest, ItemEntry, 1, true);
	player->SendNewItem(newItem, 1, true, false);
	return true;
};

class npc_archmage_leryda : public CreatureScript
{
public:
    npc_archmage_leryda() : CreatureScript("npc_archmage_leryda") { }

    bool OnGoosipHello(Player* player, Creature* creature)
    {
		if (creature->IsQuestGiver())
			player->PrepareQuestMenu( creature->GetGUID());
		
		if (!(player->HasItemCount(RING_PROTECTOR, 1, true)
			|| player->HasItemCount(RING_RESTORER, 1, true)
			|| player->HasItemCount(RING_ARCHMAGE, 1, true)
			|| player->HasItemCount(RING_ASSASSIN, 1, true)))
		{
			if (player->GetQuestStatus(QUEST_ARCHMAGE) == QUEST_STATUS_COMPLETE && player->GetQuestRewardStatus(QUEST_ARCHMAGE))
			{
				player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
			}
			else
				if (player->GetQuestStatus(QUEST_RESTORER) == QUEST_STATUS_COMPLETE && player->GetQuestRewardStatus(QUEST_RESTORER))
				{
					player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
				}
				else if (player->GetQuestStatus(QUEST_PROTECTOR) == QUEST_STATUS_COMPLETE && player->GetQuestRewardStatus(QUEST_PROTECTOR))
				{
					player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
				}
				else if (player->GetQuestStatus(QUEST_ASSASSIN) == QUEST_STATUS_COMPLETE && player->GetQuestRewardStatus(QUEST_ASSASSIN))
				{
					player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
				}
		}
		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
		return true;
	};

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        player->CLOSE_GOSSIP_MENU();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                if (AddItemToPlayerRing(player, RING_ARCHMAGE))
                    player->SetQuestStatus(QUEST_W_ARCHMAGE, QUEST_STATUS_NONE);
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                if (AddItemToPlayerRing(player, RING_RESTORER))
                    player->SetQuestStatus(QUEST_W_RESTORER, QUEST_STATUS_NONE);
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                if (AddItemToPlayerRing(player, RING_PROTECTOR))
                    player->SetQuestStatus(QUEST_W_PROTECTOR, QUEST_STATUS_NONE);
                break;
            case GOSSIP_ACTION_INFO_DEF + 4:
                if (AddItemToPlayerRing(player, RING_ASSASSIN))
                    player->SetQuestStatus(QUEST_W_ASSASSIN, QUEST_STATUS_NONE);
				break;
		}
		return true;
    };
};

enum ArcaneDisturbances
{
	SPELL_VIOLET_SCRYING_CRYSTAL = 32042,
	NPC_UNDERGROUND_WELL_CREDIT  = 18161,
	NPC_UNDERGROUND_POND_CREDIT  = 18162,
};

class npc_underground_well_credit_marker : public CreatureScript
{
public:
    npc_underground_well_credit_marker() : CreatureScript("npc_underground_well_credit_marker") {}

    struct npc_underground_well_credit_markerAI : public QuantumBasicAI
    {
		npc_underground_well_credit_markerAI(Creature* creature) : QuantumBasicAI(creature) {}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_VIOLET_SCRYING_CRYSTAL)
            {
				if (Player* player = caster->ToPlayer())
					caster->ToPlayer()->KilledMonsterCredit(NPC_UNDERGROUND_WELL_CREDIT, 0);
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_underground_well_credit_markerAI(creature);
    }
};

class npc_underground_pond_credit_marker : public CreatureScript
{
public:
    npc_underground_pond_credit_marker() : CreatureScript("npc_underground_pond_credit_marker") {}

    struct npc_underground_pond_credit_markerAI : public QuantumBasicAI
    {
		npc_underground_pond_credit_markerAI(Creature* creature) : QuantumBasicAI(creature) {}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_VIOLET_SCRYING_CRYSTAL)
            {
				if (Player* player = caster->ToPlayer())
					caster->ToPlayer()->KilledMonsterCredit(NPC_UNDERGROUND_POND_CREDIT, 0);
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_underground_pond_credit_markerAI(creature);
    }
};

void AddSC_deadwind_pass()
{
	new npc_archmage_leryda();
	new npc_underground_well_credit_marker();
	new npc_underground_pond_credit_marker();
}