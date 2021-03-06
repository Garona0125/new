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

#define GOSSIP_HDA1 "What does the Felstone Field Cauldron need?"
#define GOSSIP_HDA2 "What does the Dalson's Tears Cauldron need?"
#define GOSSIP_HDA3 "What does the Writhing Haunt Cauldron need?"
#define GOSSIP_HDA4 "What does the Gahrron's Withering Cauldron need?"
#define GOSSIP_SDA1 "Thanks, i need a Vitreous Focuser"

class npcs_dithers_and_arbington : public CreatureScript
{
public:
    npcs_dithers_and_arbington() : CreatureScript("npcs_dithers_and_arbington") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->IsVendor())
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_VENDOR_BROWSE_YOUR_GOODS), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        if (player->GetQuestRewardStatus(5237) || player->GetQuestRewardStatus(5238))
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HDA1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HDA2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HDA3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HDA4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
            player->SEND_GOSSIP_MENU(3985, creature->GetGUID());
        }
		else
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        switch (action)
        {
            case GOSSIP_ACTION_TRADE:
                player->GetSession()->SendListInventory(creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SDA1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
                player->SEND_GOSSIP_MENU(3980, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SDA1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
                player->SEND_GOSSIP_MENU(3981, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SDA1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
                player->SEND_GOSSIP_MENU(3982, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SDA1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
                player->SEND_GOSSIP_MENU(3983, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+5:
                player->CLOSE_GOSSIP_MENU();
                creature->CastSpell(player, 17529, false);
                break;
        }
        return true;
    }
};

enum Myranda
{
    QUEST_SUBTERFUGE       = 5862,
    QUEST_IN_DREAMS        = 5944,
    SPELL_SCARLET_ILLUSION = 17961,
};

#define GOSSIP_ITEM_ILLUSION "I am ready for the illusion, Myranda."

class npc_myranda_the_hag : public CreatureScript
{
public:
    npc_myranda_the_hag() : CreatureScript("npc_myranda_the_hag") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_SUBTERFUGE) == QUEST_STATUS_COMPLETE &&
			player->GetQuestStatus(QUEST_IN_DREAMS) != QUEST_STATUS_COMPLETE &&
			!player->HasAura(SPELL_SCARLET_ILLUSION))
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ILLUSION, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(4773, creature->GetGUID());
            return true;
        }
        else
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            player->CLOSE_GOSSIP_MENU();
            player->CastSpell(player, SPELL_SCARLET_ILLUSION, false);
        }
        return true;
    }
};

class npc_the_scourge_cauldron : public CreatureScript
{
public:
    npc_the_scourge_cauldron() : CreatureScript("npc_the_scourge_cauldron") { }

    struct npc_the_scourge_cauldronAI : public QuantumBasicAI
    {
        npc_the_scourge_cauldronAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset() {}

        void EnterToBattle(Unit* /*who*/) {}

        void DoDie()
        {
            me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            uint32 RespawnTime = me->GetRespawnDelay();
            if (RespawnTime<600)
                me->SetRespawnDelay(600);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!who || who->GetTypeId() != TYPE_ID_PLAYER)
                return;

            if (who->GetTypeId() == TYPE_ID_PLAYER)
            {
                switch (me->GetAreaId())
                {
                    case 199: // Felstone
                        if (CAST_PLR(who)->GetQuestStatus(5216) == QUEST_STATUS_INCOMPLETE || CAST_PLR(who)->GetQuestStatus(5229) == QUEST_STATUS_INCOMPLETE)
                        {
                            me->SummonCreature(11075, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000);
                            DoDie();
                        }
                        break;
                    case 200: // Dalson
                        if (CAST_PLR(who)->GetQuestStatus(5219) == QUEST_STATUS_INCOMPLETE || CAST_PLR(who)->GetQuestStatus(5231) == QUEST_STATUS_INCOMPLETE)
                        {
                            me->SummonCreature(11077, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000);
                            DoDie();
                        }
                        break;
                    case 201: // Gahrron
                        if (CAST_PLR(who)->GetQuestStatus(5225) == QUEST_STATUS_INCOMPLETE || CAST_PLR(who)->GetQuestStatus(5235) == QUEST_STATUS_INCOMPLETE)
                        {
                            me->SummonCreature(11078, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000);
                            DoDie();
                        }
                        break;
                    case 202: // Writhing
                        if (CAST_PLR(who)->GetQuestStatus(5222) == QUEST_STATUS_INCOMPLETE || CAST_PLR(who)->GetQuestStatus(5233) == QUEST_STATUS_INCOMPLETE)
                        {
                            me->SummonCreature(11076, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000);
                            DoDie();
                        }
                        break;
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_the_scourge_cauldronAI(creature);
	}
};

enum AndorhalTower
{
    GO_BEACON_TORCH = 176093,
};

class npc_andorhal_tower : public CreatureScript
{
public:
    npc_andorhal_tower() : CreatureScript("npc_andorhal_tower") { }

    struct npc_andorhal_towerAI : public QuantumBasicAI
    {
        npc_andorhal_towerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        void MoveInLineOfSight(Unit* who)
        {
            if (!who || who->GetTypeId() != TYPE_ID_PLAYER)
                return;

            if (me->FindGameobjectWithDistance(GO_BEACON_TORCH, 10.0f))
                CAST_PLR(who)->KilledMonsterCredit(me->GetEntry(), me->GetGUID());
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_andorhal_towerAI(creature);
	}
};

enum Truuen
{
    NPC_GHOST_UTHER         = 17233,
    NPC_THEL_DANIS          = 1854,
    NPC_GHOUL               = 1791,

    QUEST_TOMB_LIGHTBRINGER = 9446,

	SAY_WP_0                = 0,
	SAY_WP_1                = 1,
	SAY_WP_2                = 2,
	SAY_WP_3                = 0,
	SAY_WP_4                = 0,
	SAY_WP_5                = 1,
	SAY_WP_6                = 2,
};

class npc_anchorite_truuen : public CreatureScript
{
public:
    npc_anchorite_truuen() : CreatureScript("npc_anchorite_truuen") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        npc_escortAI* EscortAI = CAST_AI(npc_anchorite_truuen::npc_anchorite_truuenAI, creature->AI());

        if (quest->GetQuestId() == QUEST_TOMB_LIGHTBRINGER)
            EscortAI->Start(true, true, player->GetGUID());

        return false;
    }

    struct npc_anchorite_truuenAI : public npc_escortAI
    {
        npc_anchorite_truuenAI(Creature* creature) : npc_escortAI(creature) { }

        uint32 ChatTimer;

        uint64 UghostGUID;
        uint64 TheldanisGUID;

        Creature* Ughost;
        Creature* Theldanis;

        void Reset()
        {
            ChatTimer = 7000;
        }

        void JustSummoned(Creature* summoned)
        {
            if (summoned->GetEntry() == NPC_GHOUL)
                summoned->AI()->AttackStart(me);
        }

        void WaypointReached(uint32 i)
        {
            Player* player = GetPlayerForEscort();
            switch (i)
            {
                case 8:
                    Talk(SAY_WP_0);
                    me->SummonCreature(NPC_GHOUL, me->GetPositionX()+7.0f, me->GetPositionY()+7.0f, me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90000);
                    me->SummonCreature(NPC_GHOUL, me->GetPositionX()+5.0f, me->GetPositionY()+5.0f, me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90000);
                    break;
                case 9:
                    Talk(SAY_WP_1);
                    break;
                case 14:
                    me->SummonCreature(NPC_GHOUL, me->GetPositionX()+7.0f, me->GetPositionY()+7.0f, me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90000);
                    me->SummonCreature(NPC_GHOUL, me->GetPositionX()+5.0f, me->GetPositionY()+5.0f, me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90000);
                    me->SummonCreature(NPC_GHOUL, me->GetPositionX()+10.0f, me->GetPositionY()+10.0f, me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90000);
                    me->SummonCreature(NPC_GHOUL, me->GetPositionX()+8.0f, me->GetPositionY()+8.0f, me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90000);
                    break;
                case 15:
                    Talk(SAY_WP_2);
                case 21:
                    Theldanis = GetClosestCreatureWithEntry(me, NPC_THEL_DANIS, 150);
                    DoSendQuantumText(SAY_WP_3, Theldanis);
                    break;
                case 22:
                    break;
                case 23:
                    Ughost = me->SummonCreature(NPC_GHOST_UTHER, 971.86f, -1825.42f, 81.99f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
					if (Ughost)
					{
						Ughost->SetDisableGravity(true);
						Ughost->AI()->Talk(SAY_WP_4, me->GetGUID());
					}
                    ChatTimer = 4000;
                    break;
                case 24:
                    if (Ughost)
						Ughost->AI()->Talk(SAY_WP_5, me->GetGUID());
                    ChatTimer = 4000;
                    break;
                case 25:
                    if (Ughost)
						Ughost->AI()->Talk(SAY_WP_6, me->GetGUID());
                    ChatTimer = 4000;
                    break;
                case 26:
                    if (player)
                        player->GroupEventHappens(QUEST_TOMB_LIGHTBRINGER, me);
                    break;
            }
        }
		
		void EnterToBattle(Unit* /*who*/){}
		
		void JustDied(Unit* /*killer*/)
		{
			Player* player = GetPlayerForEscort();

			if (player)
				player->FailQuest(QUEST_TOMB_LIGHTBRINGER);
		}
		
		void UpdateAI(const uint32 diff)
		{
			npc_escortAI::UpdateAI(diff);

			DoMeleeAttackIfReady();

			if (HasEscortState(STATE_ESCORT_ESCORTING))
				ChatTimer = 6000;
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_anchorite_truuenAI(creature);
	}
};

void AddSC_western_plaguelands()
{
    new npcs_dithers_and_arbington();
    new npc_myranda_the_hag();
    new npc_the_scourge_cauldron();
    new npc_andorhal_tower();
    new npc_anchorite_truuen();
}