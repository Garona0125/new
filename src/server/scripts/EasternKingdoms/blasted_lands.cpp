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

#define GOSSIP_ITEM_USHER "I wish to to visit the Rise of the Defiler."

enum DeathlyUsher
{
	SPELL_TELEPORT_SINGLE          = 12885,
	SPELL_TELEPORT_SINGLE_IN_GROUP = 13142,
	SPELL_TELEPORT_GROUP           = 27686,
	QUEST_YOU_ARE_RAKHLIKH_DEMON   = 3628,
	ITEM_WARD_OF_THE_DEFILER       = 10757,
};

class npc_deathly_usher : public CreatureScript
{
public:
    npc_deathly_usher() : CreatureScript("npc_deathly_usher") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_YOU_ARE_RAKHLIKH_DEMON) == QUEST_STATUS_INCOMPLETE && player->HasItemCount(ITEM_WARD_OF_THE_DEFILER, 1))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_USHER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->CastSpell(player, SPELL_TELEPORT_SINGLE, true);
        }
        return true;
    }
};

#define GOSSIP_H_F1 "Why are you here?"
#define GOSSIP_H_F2 "Continue story..."

#define GOSSIP_ITEM_FALLEN "Continue..."
#define GOSSIP_ITEM_FALLEN1 "What could be worse than death?"
#define GOSSIP_ITEM_FALLEN2 "Subordinates?"
#define GOSSIP_ITEM_FALLEN3 "What are the stones of binding?"
#define GOSSIP_ITEM_FALLEN4 "You can count on me, Hero"
#define GOSSIP_ITEM_FALLEN5 "I shall"

enum HeroesOfOld
{
	NPC_CORPORAL_THUND_SPLITHOOF = 7750,
    QUEST_HEROES_OF_OLD          = 2702,
	QUEST_FALL_FROM_GRACE        = 2784,
	QUEST_A_TALE_OF_SORROW       = 2801,
};

class npc_fallen_hero_of_horde : public CreatureScript
{
public:
    npc_fallen_hero_of_horde() : CreatureScript("npc_fallen_hero_of_horde") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_FALL_FROM_GRACE) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_H_F1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        if (player->GetQuestStatus(QUEST_A_TALE_OF_SORROW) == QUEST_STATUS_INCOMPLETE && player->GetTeam() == HORDE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_H_F2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

        if (player->GetQuestStatus(QUEST_A_TALE_OF_SORROW) == QUEST_STATUS_INCOMPLETE && player->GetTeam() == ALLIANCE)
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_H_F1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
                player->SEND_GOSSIP_MENU(1392, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+11:
                player->SEND_GOSSIP_MENU(1411, creature->GetGUID());

                if (player->GetQuestStatus(QUEST_FALL_FROM_GRACE) == QUEST_STATUS_INCOMPLETE)
                    player->AreaExploredOrEventHappens(QUEST_FALL_FROM_GRACE);

                if (player->GetTeam() == ALLIANCE)
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                    player->SEND_GOSSIP_MENU(1411, creature->GetGUID());
                }
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 21);
                player->SEND_GOSSIP_MENU(1451, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+21:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 22);
                player->SEND_GOSSIP_MENU(1452, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+22:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 23);
                player->SEND_GOSSIP_MENU(1453, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+23:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 24);
                player->SEND_GOSSIP_MENU(1454, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+24:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 25);
                player->SEND_GOSSIP_MENU(1455, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+25:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FALLEN5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 26);
                player->SEND_GOSSIP_MENU(1456, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+26:
                player->CLOSE_GOSSIP_MENU();
                player->AreaExploredOrEventHappens(QUEST_A_TALE_OF_SORROW);
                break;
        }
        return true;
    }

    bool OnQuestAccept(Player* /*player*/, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_HEROES_OF_OLD)
            creature->SummonCreature(NPC_CORPORAL_THUND_SPLITHOOF, -10630.3f, -2987.05f, 28.96f, 4.54f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 9000000);

        return true;
    }
};

enum DoomguardCommander
{
	SPELL_CRYSTAL_IMPISONMENT = 23020,
	SPELL_CRYSTAL_DUMMY       = 23019,
	SPELL_CRIPPLE             = 20812,
	SPELL_DISPEL_MAGIC        = 15090,
	SPELL_RAIN_OF_FIRE        = 16005,
	SPELL_WAR_STOMP           = 16727,

	GO_IMPRISONED_DOOMGUARD   = 179644,
};

class npc_doomguard_commander : public CreatureScript
{
public:
    npc_doomguard_commander() : CreatureScript("npc_doomguard_commander") { }

    struct npc_doomguard_commanderAI : public QuantumBasicAI
    {
        npc_doomguard_commanderAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 CrippleTimer;
		uint32 RainOfFireTimer;
		uint32 DispelMagicTimer;
		uint32 WarStompTimer;

        void Reset()
		{
			CrippleTimer = 2000;
			RainOfFireTimer = 4000;
			DispelMagicTimer = 5000;
			WarStompTimer = 7000;
		}

        void EnterToBattle(Unit* /*who*/) {}

        void SpellHitTarget(Unit* target, SpellInfo* const spell)
        {
			switch (spell->Id)
			{
			    case SPELL_CRYSTAL_DUMMY:
					float x,y,z;
					me->GetPosition(x, y, z);
                    me->SummonGameObject(GO_IMPRISONED_DOOMGUARD, x, y, z, 0, 0, 0, 0, 0, 300000);
                    me->DealDamage(me, me->GetHealth());
                    me->RemoveCorpse();
                    break;
			}
		}

		void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CrippleTimer <= diff)
			{
				DoCastVictim(SPELL_CRIPPLE);
				CrippleTimer = urand(3000, 4000);
			}
			else CrippleTimer -= diff;

			if (RainOfFireTimer <= diff)
			{
				DoCastAOE(SPELL_RAIN_OF_FIRE);
				RainOfFireTimer = urand(5000, 6000);
			}
			else RainOfFireTimer -= diff;

			if (DispelMagicTimer <= diff)
			{
				DoCast(me, SPELL_DISPEL_MAGIC);
				DispelMagicTimer = urand(7000, 8000);
			}
			else DispelMagicTimer -= diff;

			if (WarStompTimer <= diff)
			{
				DoCastAOE(SPELL_WAR_STOMP);
				WarStompTimer = urand(9000, 10000);
			}
			else WarStompTimer -= diff;

			DoMeleeAttackIfReady();
		}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_doomguard_commanderAI(creature);
    }
};

void AddSC_blasted_lands()
{
	new npc_deathly_usher();
	new npc_fallen_hero_of_horde();
	new npc_doomguard_commander();
}