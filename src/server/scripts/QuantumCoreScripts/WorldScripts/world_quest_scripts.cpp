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

enum Worlds
{
	// Items
	ITEM_ID_MEDALLION_OF_KARABOR         = 32649,
	ITEM_ID_BLESSED_MEDALLION_OF_KARABOR = 32757,
	ITEM_ID_ARCATRAZ_KEY                 = 31084,
	// Quests
	QUEST_TRIAL_OF_THE_NAARU_MAGTHERIDON = 10888,
	QUEST_HOW_TO_BREAK_INTO_THE_ARCATRAZ = 10704,
	QUEST_THE_FALL_OF_THE_BETRAYER       = 10959,
	QUEST_A_DISTRACTION_FOR_AKAMA_1      = 10985,
	QUEST_A_DISTRACTION_FOR_AKAMA_2      = 13429,
	// Achievements
	ACHIEVEMENT_HAND_OF_ADAL             = 431,
	ACHIEVEMENT_CHAMPION_OF_THE_NAARU    = 432,
};

class npc_seer_kanai : public CreatureScript
{
public:
    npc_seer_kanai() : CreatureScript("npc_seer_kanai") {}

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
		return true;
    }

	bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		return true;
	}

    bool OnQuestReward(Player* player, Creature* creature, Quest const* quest, uint32 /*item*/)
    {
        if (quest->GetQuestId() == QUEST_THE_FALL_OF_THE_BETRAYER)
        {
			if (AchievementEntry const* HandOfAdal = sAchievementStore.LookupEntry(ACHIEVEMENT_HAND_OF_ADAL))
			{
				player->CompletedAchievement(HandOfAdal);
				std::string sText = ("|CFFFF0000Achievement Manager:|r Player |CFFFF3366" + std::string(player->GetSession()->GetPlayerName()) + "|r Has Earned Achievement: [Hand of A'dal]");
				sWorld->SendServerMessage(SERVER_MSG_STRING, sText.c_str());
			}
		}
		return true;
    }
};

#define GOSSIP_ARKATRAZ_KEY      "I have lost the key to the Arkatraz. If you can help me?"
#define GOSSIP_KARABOR_MEDALLION "I have lost the Medallion of Karabor. If you can help me?"

class npc_adal : public CreatureScript
{
public:
    npc_adal() : CreatureScript("npc_adal") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if ((player->GetQuestStatus(QUEST_HOW_TO_BREAK_INTO_THE_ARCATRAZ) == QUEST_STATUS_COMPLETE
			&& player->GetQuestRewardStatus(QUEST_HOW_TO_BREAK_INTO_THE_ARCATRAZ)) && !player->HasItemCount(ITEM_ID_ARCATRAZ_KEY, 1))
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ARKATRAZ_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        if ((player->GetQuestStatus(QUEST_A_DISTRACTION_FOR_AKAMA_1) == QUEST_STATUS_COMPLETE
			&& player->GetQuestRewardStatus(QUEST_A_DISTRACTION_FOR_AKAMA_1))
			|| (player->GetQuestStatus(QUEST_A_DISTRACTION_FOR_AKAMA_2) == QUEST_STATUS_COMPLETE
			&& player->GetQuestRewardStatus(QUEST_A_DISTRACTION_FOR_AKAMA_2)))
        {
            if (player->GetQuestStatus(QUEST_THE_FALL_OF_THE_BETRAYER) == QUEST_STATUS_COMPLETE
				&& player->GetQuestRewardStatus(QUEST_THE_FALL_OF_THE_BETRAYER))
            {
                if (!player->HasItemCount(ITEM_ID_BLESSED_MEDALLION_OF_KARABOR, 1))
                    player->ADD_GOSSIP_ITEM(0, GOSSIP_KARABOR_MEDALLION, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            }
            else
            {
                if (!player->HasItemCount(ITEM_ID_MEDALLION_OF_KARABOR, 1))
                    player->ADD_GOSSIP_ITEM(0, GOSSIP_KARABOR_MEDALLION, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            }
        }

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature),creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        bool canSendItem = false;
        uint32 KeyEntry;
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF+1:
            canSendItem = true;
            KeyEntry = ITEM_ID_ARCATRAZ_KEY;
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            canSendItem = true;
            KeyEntry = ITEM_ID_MEDALLION_OF_KARABOR;
            break;
         case GOSSIP_ACTION_INFO_DEF+3:
            canSendItem = true;
            KeyEntry = ITEM_ID_BLESSED_MEDALLION_OF_KARABOR;
            break;
        }

        player->CLOSE_GOSSIP_MENU();

        if (canSendItem)
        {
            ItemPosCountVec dest;
            uint8 canStoreNewItem = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, KeyEntry, 1);

            if (canStoreNewItem == EQUIP_ERR_OK)
            {
                Item* newItem = NULL;
                newItem = player->StoreNewItem(dest, KeyEntry, 1, true);
                player->SendNewItem(newItem, 1, true, false);
            }
        }
		return true;
    }

	bool OnQuestReward(Player* player, Creature* creature, Quest const* quest, uint32 /*item*/)
    {
        if (quest->GetQuestId() == QUEST_TRIAL_OF_THE_NAARU_MAGTHERIDON)
        {
			if (AchievementEntry const* Naaru = sAchievementStore.LookupEntry(ACHIEVEMENT_CHAMPION_OF_THE_NAARU))
			{
				player->CompletedAchievement(Naaru);
				std::string sText = ("|CFFFF0000Achievement Manager:|r Player |CFFFF3366" + std::string(player->GetSession()->GetPlayerName()) + "|r Has Earned Achievement: [Champion of the Naaru]");
				sWorld->SendServerMessage(SERVER_MSG_STRING, sText.c_str());
			}
		}
		return true;
    }
};

enum QuestWeThereYeti
{
	SPELL_RELEASE_UMIS_YETI = 17166,

	NPC_KILL_CREDIT_SPINKLE = 7583,
	NPC_KILL_CREDIT_QUIXXIL = 10977,
	NPC_KILL_CREDIT_LEGACKI = 10978,
};

class npc_sprinkle : public CreatureScript
{
public:
    npc_sprinkle() : CreatureScript("npc_sprinkle") {}

    struct npc_sprinkleAI : public QuantumBasicAI
    {
		npc_sprinkleAI(Creature* creature) : QuantumBasicAI(creature) {}

		void Reset(){}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_RELEASE_UMIS_YETI)
            {
				if (Player* player = caster->ToPlayer())
					caster->ToPlayer()->KilledMonsterCredit(NPC_KILL_CREDIT_SPINKLE, 0);
			}
        }

		void UpdateAI(const uint32 /*diff*/)
		{
			if (!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sprinkleAI(creature);
    }
};

class npc_quixxil : public CreatureScript
{
public:
    npc_quixxil() : CreatureScript("npc_quixxil") {}

    struct npc_quixxilAI : public QuantumBasicAI
    {
		npc_quixxilAI(Creature* creature) : QuantumBasicAI(creature) {}

		void Reset(){}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_RELEASE_UMIS_YETI)
            {
				if (Player* player = caster->ToPlayer())
					caster->ToPlayer()->KilledMonsterCredit(NPC_KILL_CREDIT_QUIXXIL, 0);
			}
        }

		void UpdateAI(const uint32 /*diff*/)
		{
			if (!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_quixxilAI(creature);
    }
};

class npc_legacki : public CreatureScript
{
public:
    npc_legacki() : CreatureScript("npc_legacki") {}

    struct npc_legackiAI : public QuantumBasicAI
    {
		npc_legackiAI(Creature* creature) : QuantumBasicAI(creature) {}

		void Reset(){}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_RELEASE_UMIS_YETI)
            {
				if (Player* player = caster->ToPlayer())
					caster->ToPlayer()->KilledMonsterCredit(NPC_KILL_CREDIT_LEGACKI, 0);
			}
        }

		void UpdateAI(const uint32 /*diff*/)
		{
			if (!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_legackiAI(creature);
    }
};

enum QuestLostInAction
{
	QUEST_LOST_IN_ACTION  = 9738,

	NPC_CREDIT_RAYGE      = 17885,
	NPC_CREDIT_WEEDER     = 17890,
	NPC_CREDIT_NATURALIST = 17893,
	NPC_CREDIT_WINDCALLER = 17894,

	SPELL_MARK_OF_BITE    = 34906,

	GO_NATURALIST_CAGE    = 182094,
};

class npc_earthbinder_rayge : public CreatureScript
{
public:
    npc_earthbinder_rayge() : CreatureScript("npc_earthbinder_rayge") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_LOST_IN_ACTION) == QUEST_STATUS_INCOMPLETE)
            player->KilledMonsterCredit(NPC_CREDIT_RAYGE, 0);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }
};

class npc_weeder_greenthumb : public CreatureScript
{
public:
    npc_weeder_greenthumb() : CreatureScript("npc_weeder_greenthumb") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_LOST_IN_ACTION) == QUEST_STATUS_INCOMPLETE)
            player->KilledMonsterCredit(NPC_CREDIT_WEEDER, 0);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }
};

class npc_naturalist_bite : public CreatureScript
{
public:
    npc_naturalist_bite() : CreatureScript("npc_naturalist_bite") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_LOST_IN_ACTION) == QUEST_STATUS_INCOMPLETE)
            player->KilledMonsterCredit(NPC_CREDIT_NATURALIST, 0);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }
};

class npc_windcaller_claw : public CreatureScript
{
public:
    npc_windcaller_claw() : CreatureScript("npc_windcaller_claw") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_LOST_IN_ACTION) == QUEST_STATUS_INCOMPLETE)
            player->KilledMonsterCredit(NPC_CREDIT_WINDCALLER, 0);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }
};

enum UlduarQuests
{
	SPELL_FROST_ARMOR      = 18100,
	SPELL_ARCANE_INTELLECT = 36880,
	SPELL_DAMPEN_MAGIC     = 43015,
};

class npc_jaison_and_patrick : public CreatureScript
{
public:
    npc_jaison_and_patrick() : CreatureScript("npc_jaison_and_patrick") {}

    struct npc_jaison_and_patrickAI : public QuantumBasicAI
    {
        npc_jaison_and_patrickAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BuffTimer;

        void Reset()
		{
			BuffTimer = 100;

			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (BuffTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_FROST_ARMOR, true);
				DoCast(me, SPELL_ARCANE_INTELLECT, true);
				DoCast(me, SPELL_DAMPEN_MAGIC, true);
				BuffTimer = 10*MINUTE*IN_MILLISECONDS;
			}
			else BuffTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jaison_and_patrickAI(creature);
    }
};

void AddSC_world_quest_scripts()
{
	new npc_seer_kanai();
	new npc_adal();
	new npc_sprinkle();
	new npc_quixxil();
	new npc_legacki();
	new npc_earthbinder_rayge();
	new npc_weeder_greenthumb();
	new npc_naturalist_bite();
	new npc_windcaller_claw();
	new npc_jaison_and_patrick();
}