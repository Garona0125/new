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

#define GOSSIP_ITEM_BEACON "Please make me a Cenarion Beacon"

enum RiverbreezeAndSilversky
{
    SPELL_CENARION_BEACON       = 15120,
    
    NPC_ARATHANDRIS_SILVERSKY   = 9528,
    NPC_MAYBESS_RIVERBREEZE     = 9529,
    
    QUEST_CLEASING_FELWOOD_A    = 4101,
    QUEST_CLEASING_FELWOOD_H    = 4102,
};

class npcs_riverbreeze_and_silversky : public CreatureScript
{
public:
    npcs_riverbreeze_and_silversky() : CreatureScript("npcs_riverbreeze_and_silversky") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        uint32 creatureId = creature->GetEntry();

        if (creatureId == NPC_ARATHANDRIS_SILVERSKY)
        {
            if (player->GetQuestRewardStatus(QUEST_CLEASING_FELWOOD_A))
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BEACON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                player->SEND_GOSSIP_MENU(2848, creature->GetGUID());
            }
			else if (player->GetTeam() == HORDE)
			{
				player->SEND_GOSSIP_MENU(2845, creature->GetGUID());
			}
            else
                player->SEND_GOSSIP_MENU(2844, creature->GetGUID());
        }

        if (creatureId == NPC_MAYBESS_RIVERBREEZE)
        {
            if (player->GetQuestRewardStatus(QUEST_CLEASING_FELWOOD_H))
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BEACON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                player->SEND_GOSSIP_MENU(2849, creature->GetGUID());
            }
			else if (player->GetTeam() == ALLIANCE)
			{
				player->SEND_GOSSIP_MENU(2843, creature->GetGUID());
			}
            else
                player->SEND_GOSSIP_MENU(2842, creature->GetGUID());
        }
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->CastSpell(player, SPELL_CENARION_BEACON, false);
        }
        return true;
    }
};

enum CorruptedSabersData
{
    NPC_COMMON_KITTEN      = 9937,
    NPC_CORRUPTED_SABER    = 10657,
    NPC_WINNA_HAZZARD      = 9996,

    ZONE_FELWOOD           = 361,
    QUEST_CORRUPTED_SABERS = 4506,
};

class npc_common_kitten : public CreatureScript
{
public:
    npc_common_kitten() : CreatureScript("npc_common_kitten") { }

    struct npc_common_kittenAI : public QuantumBasicAI
    {
        npc_common_kittenAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CorruptionTime;

        bool QuestGiverFound;

        void Reset()
        {
			me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);

            CorruptionTime = 5000;
            QuestGiverFound = false;
        }

        void UpdateAI (const uint32 diff)
        {
            if (CorruptionTime <= diff && me->GetEntry() == NPC_COMMON_KITTEN)
            {
                me->UpdateEntry(NPC_CORRUPTED_SABER);

                if (Unit* owner = me->GetOwner())
                {
                    me->SetLevel(me->GetOwner()->GetCurrentLevel());
                    me->SetFullHealth();
                }
            }
            else CorruptionTime -= diff;

            if (Creature* Winna = me->FindCreatureWithDistance(NPC_WINNA_HAZZARD, 10.0f, true))
                QuestGiverFound = true;

            if (me->GetEntry() == NPC_CORRUPTED_SABER)
            {
                if (Unit* owner = me->GetOwner())
                {
                    if (owner->GetTypeId() != TYPE_ID_PLAYER || !owner->IsAlive() || owner->ToPlayer()->GetZoneId() != ZONE_FELWOOD)
                        me->DespawnAfterAction();
                }
            }

            if (QuestGiverFound)
            {
                if (Player* owner = me->GetOwner()->ToPlayer())
                {
                    owner->ToPlayer()->GroupEventHappens(QUEST_CORRUPTED_SABERS, me);
                    me->DespawnAfterAction(3*IN_MILLISECONDS);
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_common_kittenAI(creature);
    }
};

void AddSC_felwood()
{
    new npcs_riverbreeze_and_silversky();
	new npc_common_kitten();
}