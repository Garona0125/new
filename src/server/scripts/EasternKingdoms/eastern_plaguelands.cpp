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
#include "QuantumSystemText.h"

class npc_augustus_the_touched : public CreatureScript
{
public:
    npc_augustus_the_touched() : CreatureScript("npc_augustus_the_touched") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->IsVendor() && player->GetQuestRewardStatus(6164))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_VENDOR_BROWSE_YOUR_GOODS), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
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

enum SpiritSpells
{
	SPELL_SPIRIT_SPAWNIN = 17321,
};

class npc_darrowshire_spirit : public CreatureScript
{
public:
    npc_darrowshire_spirit() : CreatureScript("npc_darrowshire_spirit") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        player->SEND_GOSSIP_MENU(3873, creature->GetGUID());
        player->TalkedToCreature(creature->GetEntry(), creature->GetGUID());
        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        return true;
    }

    struct npc_darrowshire_spiritAI : public QuantumBasicAI
    {
        npc_darrowshire_spiritAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset()
        {
            DoCast(me, SPELL_SPIRIT_SPAWNIN);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        void EnterToBattle(Unit* /*who*/) {}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_darrowshire_spiritAI (creature);
    }
};

#define GOSSIP_HELLO   "I am ready to hear your tale, Tirion."
#define GOSSIP_SELECT1 "Thank you, Tirion.  What of your identity?"
#define GOSSIP_SELECT2 "That is terrible."
#define GOSSIP_SELECT3 "I will, Tirion."

enum TirionFordring
{
	QUEST_REDEMPTION = 5742,
};

class npc_tirion_fordring : public CreatureScript
{
public:
    npc_tirion_fordring() : CreatureScript("npc_tirion_fordring") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_REDEMPTION) == QUEST_STATUS_INCOMPLETE && player->getStandState() == UNIT_STAND_STATE_SIT)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(4493, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                player->SEND_GOSSIP_MENU(4494, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                player->SEND_GOSSIP_MENU(4495, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                player->CLOSE_GOSSIP_MENU();
                player->AreaExploredOrEventHappens(QUEST_REDEMPTION);
                break;
        }
        return true;
    }
};

enum WhenSmokeySingsIGetViolent
{
	SPELL_PLACING_SMOKEY_EXPLOSIVES = 19250,
	SPELL_MARK_OF_DETONATION        = 22438,

	NPC_SCOURGE_KILL_CREDIT         = 12247,
};

class npc_mark_of_detonation : public CreatureScript
{
public:
    npc_mark_of_detonation() : CreatureScript("npc_mark_of_detonation") {}

    struct npc_mark_of_detonationAI : public QuantumBasicAI
    {
        npc_mark_of_detonationAI(Creature* creature) : QuantumBasicAI(creature){}

		bool Detonated;

        void Reset()
		{
			Detonated = false;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void SpellHit(Unit* caster, SpellInfo* const spell)
        {
            if (spell->Id == SPELL_PLACING_SMOKEY_EXPLOSIVES && !Detonated)
			{
				Detonated = true;
				DoCast(me, SPELL_MARK_OF_DETONATION, true);
				caster->ToPlayer()->KilledMonsterCredit(NPC_SCOURGE_KILL_CREDIT, 0);
				me->DespawnAfterAction();
			}
        }

        void UpdateAI(uint32 const /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mark_of_detonationAI(creature);
    }
};

void AddSC_eastern_plaguelands()
{
    new npc_augustus_the_touched();
    new npc_darrowshire_spirit();
    new npc_tirion_fordring();
	new npc_mark_of_detonation();
}