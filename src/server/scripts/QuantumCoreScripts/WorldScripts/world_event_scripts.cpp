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
#include "QuantumEscortAI.h"
#include "QuantumGossip.h"
#include "Group.h"

#define GOSSIP_ITEM_AOF1 "I want to stop the Scourge as much as you do. How can I help?"
#define GOSSIP_ITEM_AOF2 "You can trust me. I am no friend of the Lich King."
#define GOSSIP_ITEM_AOF3 "I will not fail."

Position const FreyaResPos[1] =
{
	{5882.35f, 4104.5f, -87.3312f, 5.34739f},
};

Position const ServantMovePos[1] =
{
	{5876.44f, 4116.12f, -85.5129f, 5.25143f},
};

enum Quests
{
    QUEST_FREYA_PACT         = 12621,
    SPELL_FREYA_CONVERSATION = 52045,
    GOSSIP_TEXTID_AVATAR1    = 13303,
    GOSSIP_TEXTID_AVATAR2    = 13304,
    GOSSIP_TEXTID_AVATAR3    = 13305,
};

enum WildGrowSpells
{
	SPELL_WILD_GROWTH_1           = 52948, // Casts on freya and Servants
	SPELL_WILD_GROWTH_2           = 61750, // Casts on freya and Servants
	SPELL_WILD_GROWTH_3           = 61751, // Casts on freya and Servants
	SPELL_FREYA_DUMMY             = 51318, // OK Prepare to cast
	SPELL_HOLOGRAM_FREYA_CHANNEL  = 64269, // OK
	SPELL_SUMMON_SERVANT_OF_FREYA = 51286, // Summon Npc 28320
	SPELL_FREYA_TRIGGER_DUMMY     = 51335, //??
	SPELL_FREYA_VINES             = 51354, // thorns,wtf? (Need Sniffs)
};

enum FreyaData
{
	FACTION_OF_FREYA              = 2081,
	NPC_WILD_GROW_STALKER         = 29027,
	NPC_SERVANT_OF_FREYA          = 28320,
};

enum FreyaEvents
{
	EVENT_FREYA_MOVE_TO_RES        = 1,
	EVENT_FREYA_PREPARE_TO_RES     = 2,
	EVENT_FREYA_RESSURECT_SERVANTS = 3,
	EVENT_FREYA_REACHED_HOME       = 4,
	EVENT_DESPAWN_SERVANTS         = 5,
};

class npc_avatar_of_freya : public CreatureScript
{
public:
    npc_avatar_of_freya() : CreatureScript("npc_avatar_of_freya") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_FREYA_PACT) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_AOF1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXTID_AVATAR1, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        switch (action)
        {
		    case GOSSIP_ACTION_INFO_DEF+1:
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_AOF2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
				player->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXTID_AVATAR2, creature->GetGUID());
				break;
			case GOSSIP_ACTION_INFO_DEF+2:
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_AOF3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
				player->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXTID_AVATAR3, creature->GetGUID());
				break;
			case GOSSIP_ACTION_INFO_DEF+3:
				player->CastSpell(player, SPELL_FREYA_CONVERSATION, true);
				player->CompleteQuest(QUEST_FREYA_PACT);
				break;
		}
		return true;
	}

	struct npc_avatar_of_freyaAI : public QuantumBasicAI
	{
		npc_avatar_of_freyaAI(Creature* creature) : QuantumBasicAI(creature){}

		EventMap events;

		void Reset()
		{
			events.Reset();

			DoCast(me, SPELL_WILD_GROWTH_1);
			DoCast(me, SPELL_WILD_GROWTH_2);
			DoCast(me, SPELL_WILD_GROWTH_3);

			ServantOfFreyaRespawner();

			events.ScheduleEvent(EVENT_FREYA_MOVE_TO_RES, 2*MINUTE*IN_MILLISECONDS);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void ServantOfFreyaSearcher()
		{
			std::list<Creature*> ServantOfFreyaList;
			me->GetCreatureListWithEntryInGrid(ServantOfFreyaList, NPC_SERVANT_OF_FREYA, 122.0f);

			if (!ServantOfFreyaList.empty())
			{
				for (std::list<Creature*>::const_iterator itr = ServantOfFreyaList.begin(); itr != ServantOfFreyaList.end(); ++itr)
				{
					if (Creature* servant = *itr)
					{
						servant->SetUInt32Value(UNIT_NPC_EMOTE_STATE, 0);
						servant->CastSpell(servant, SPELL_WILD_GROWTH_2, true);
						servant->GetMotionMaster()->MovePoint(0, ServantMovePos[0]);
						servant->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
						servant->SetSpeed(MOVE_WALK, 0.547f, true);
					}
				}
			}
		}

		void ServantOfFreyaDespawner()
		{
			std::list<Creature*> ServantOfFreyaList;
			me->GetCreatureListWithEntryInGrid(ServantOfFreyaList, NPC_SERVANT_OF_FREYA, 122.0f);

			if (!ServantOfFreyaList.empty())
			{
				for (std::list<Creature*>::const_iterator itr = ServantOfFreyaList.begin(); itr != ServantOfFreyaList.end(); ++itr)
				{
					if (Creature* servant = *itr)
						servant->DisappearAndDie();
				}
			}
		}

		void ServantOfFreyaRespawner()
		{
			std::list<Creature*> ServantOfFreyaList;
			me->GetCreatureListWithEntryInGrid(ServantOfFreyaList, NPC_SERVANT_OF_FREYA, 122.0f);

			if (!ServantOfFreyaList.empty())
			{
				for (std::list<Creature*>::const_iterator itr = ServantOfFreyaList.begin(); itr != ServantOfFreyaList.end(); ++itr)
				{
					if (Creature* servant = *itr)
					{
						servant->Respawn();
						servant->CastSpell(servant, SPELL_WILD_GROWTH_1, true);
					}
				}
			}
		}

		void UpdateAI(uint32 const diff)
		{
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
				    case EVENT_FREYA_MOVE_TO_RES:
						me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
						me->GetMotionMaster()->MovePoint(0, FreyaResPos[0]);
						events.ScheduleEvent(EVENT_FREYA_PREPARE_TO_RES, 7*IN_MILLISECONDS);
						break;
				    case EVENT_FREYA_PREPARE_TO_RES:
						DoCast(me, SPELL_FREYA_DUMMY);
						events.ScheduleEvent(EVENT_FREYA_RESSURECT_SERVANTS, 6*IN_MILLISECONDS);
						break;
					 case EVENT_FREYA_RESSURECT_SERVANTS:
						 me->PlayDirectSound(15533);
						 DoCast(me, SPELL_HOLOGRAM_FREYA_CHANNEL);
						 ServantOfFreyaSearcher();
						 events.ScheduleEvent(EVENT_DESPAWN_SERVANTS, 10*IN_MILLISECONDS);
						 break;
					 case EVENT_DESPAWN_SERVANTS:
						 ServantOfFreyaDespawner();
						 events.ScheduleEvent(EVENT_FREYA_REACHED_HOME, 12*IN_MILLISECONDS);
						 break;
					 case EVENT_FREYA_REACHED_HOME:
						 me->RemoveAurasDueToSpell(SPELL_HOLOGRAM_FREYA_CHANNEL);
						 me->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
						 me->GetMotionMaster()->MoveTargetedHome();
						 Reset();
						 break;
					 default:
						 break;
				}
			}

			if (!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_avatar_of_freyaAI(creature);
	}
};

enum IanDrake_EdwardCairn
{
	SPELL_ROTTEN_APPLE_TOSS   = 58509,
	SPELL_ROTTEN_BANANA_TOSS  = 58513,
	SPELL_SPIT                = 58520,
	SPELL_ARGENT_DAWN_TABARD  = 54975,
	SPELL_CREATE_TYRAELS_HILT = 53085,
	SPELL_TYRAEL_DANCE        = 54398,
	SPELL_MINI_TYRAEL         = 53082,
	TABARD_OF_THE_ARGENT_DAWN = 22999,

	MENU_APPLE               = 1,
	MENU_BANANA              = 2,
	MENU_SPIT                = 3,
	MENU_TYRAELS_HILT        = 4,
	MENU_TABARD              = 5,
};

class npc_ian_drake : public CreatureScript
{
public:
	npc_ian_drake() : CreatureScript("npc_ian_drake") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		player->PlayerTalkClass->ClearMenus();
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "You smell pretty bad.", GOSSIP_SENDER_MAIN, MENU_APPLE);
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "The scent is a mix between ghoul foot and a pig rump.", GOSSIP_SENDER_MAIN, MENU_BANANA);
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Nobody likes you.", GOSSIP_SENDER_MAIN, MENU_SPIT);
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_DOT, "Blizzcon 2008 (Mini Tyrael Hilt)", GOSSIP_SENDER_MAIN, MENU_TYRAELS_HILT);
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Lich King Must Fall!", GOSSIP_SENDER_MAIN, MENU_TABARD);
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();

		switch (action)
		{
			case MENU_APPLE:
				player->CLOSE_GOSSIP_MENU();
				player->CastSpell(player, SPELL_ROTTEN_APPLE_TOSS, true);
				creature->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
				player->HandleEmoteCommand(EMOTE_ONESHOT_CRY);
				break;
			case MENU_BANANA:
				player->CLOSE_GOSSIP_MENU();
				player->CastSpell(player, SPELL_ROTTEN_BANANA_TOSS, true);
				creature->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
				player->HandleEmoteCommand(EMOTE_ONESHOT_CRY);
				break;
			case MENU_SPIT:
				player->CLOSE_GOSSIP_MENU();
				player->CastSpell(player, SPELL_SPIT, true);
				creature->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
				player->HandleEmoteCommand(EMOTE_ONESHOT_CRY);
				break;
			case MENU_TYRAELS_HILT:
				if (!player->HasAura(SPELL_MINI_TYRAEL))
				{
					player->CLOSE_GOSSIP_MENU();
				}
				else
				{
					player->CLOSE_GOSSIP_MENU();
					creature->CastSpell(player, SPELL_CREATE_TYRAELS_HILT, true);
				}
				break;
			case MENU_TABARD:
				if (!player->HasItemCount(TABARD_OF_THE_ARGENT_DAWN, 1))
				{
					player->CLOSE_GOSSIP_MENU();
				}
				else
				{
					player->CLOSE_GOSSIP_MENU();
					creature->CastSpell(player, SPELL_ARGENT_DAWN_TABARD, true);
				}
				break;
		}
		return true;
	}
};

class npc_edward_cairn : public CreatureScript
{
public:
	npc_edward_cairn() : CreatureScript("npc_edward_cairn") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		player->PlayerTalkClass->ClearMenus();
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "You smell pretty bad.", GOSSIP_SENDER_MAIN, MENU_APPLE);
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "The scent is a mix between ghoul foot and a pig rump.", GOSSIP_SENDER_MAIN, MENU_BANANA);
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Nobody likes you.", GOSSIP_SENDER_MAIN, MENU_SPIT);
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_DOT, "Blizzcon 2008 (Mini Tyrael Hilt)", GOSSIP_SENDER_MAIN, MENU_TYRAELS_HILT);
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TABARD, "Lich King Must Fall!", GOSSIP_SENDER_MAIN, MENU_TABARD);
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();

		switch (action)
		{
			case MENU_APPLE:
				player->CLOSE_GOSSIP_MENU();
				player->CastSpell(player, SPELL_ROTTEN_APPLE_TOSS, true);
				creature->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
				player->HandleEmoteCommand(EMOTE_ONESHOT_CRY);
				break;
			case MENU_BANANA:
				player->CLOSE_GOSSIP_MENU();
				player->CastSpell(player, SPELL_ROTTEN_BANANA_TOSS, true);
				creature->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
				player->HandleEmoteCommand(EMOTE_ONESHOT_CRY);
				break;
			case MENU_SPIT:
				player->CLOSE_GOSSIP_MENU();
				player->CastSpell(player, SPELL_SPIT, true);
				creature->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
				player->HandleEmoteCommand(EMOTE_ONESHOT_CRY);
				break;
			case MENU_TYRAELS_HILT:
				if (!player->HasAura(SPELL_MINI_TYRAEL))
				{
					player->CLOSE_GOSSIP_MENU();
				}
				else
				{
					player->CLOSE_GOSSIP_MENU();
					creature->CastSpell(player, SPELL_CREATE_TYRAELS_HILT, true);
				}
				break;
			case MENU_TABARD:
				if (!player->HasItemCount(TABARD_OF_THE_ARGENT_DAWN, 1))
				{
					player->CLOSE_GOSSIP_MENU();
				}
				else
				{
					player->CLOSE_GOSSIP_MENU();
					creature->CastSpell(player, SPELL_ARGENT_DAWN_TABARD, true);
				}
				break;
		}
		return true;
	}
};

/*
* Highlord Tirion Fordring is the Supreme Commander of the Argent Crusade and the current bearer of the Ashbringer.
* One of the first five Knights of the Silver Hand selected by Archbishop Alonsus Faol,
* Fordring was one of the heroes of the Second War. He later became Lord of Mardenholde Keep in Hearthglen
* before being stripped of his title and exiled for defending an orc, Eitrigg, with whom he remains good friends to this day.
* Following the death of his son Taelan, Tirion committed himself to refounding the order of the Silver Hand.
* The result was the Argent Crusade, an organization consisting of the old Knights of the Silver Hand and the Argent Dawn.
* Tirion led his new Crusade into battle against the Scourge during the invasion of Northrend and, aided by
* Highlord Darion Mograine and his Knights of the Ebon Blade, directed the successful assault against
* Icecrown Citadel which resulted in the death of the Lich King.
* After the victory in Northrend, Fordring has returned to Hearthglen and directs the Crusade as it works
* to revitalize the Plaguelands.
*
* This article is so full of spoilers you'll think you're in a movie with Paul Walker and Vin Diesel.
* The Argent Coliseum will be so spoiled that its parents bought it a car before it could drive.
* There will be less mystery to the Argent Coliseum than in M. Night Shyamalan's "The Village."
*
* Script Created by Crispi Special For Quantum Core Emulator, All Rights and Reserved © 2009 - 2013 Quantum Core.
*/

enum Npcs
{
	NPC_HERALD_BELUUS     = 35501,
	NPC_TIRION_FORDRING   = 35361,
	NPC_KING_VARIAN_WRYNN = 35321,
	NPC_JAINA_PROUDMOORE  = 35320,
	NPC_THRALL            = 35368,
	NPC_GARROSH           = 35372,
	NPC_KORKRON_ELITE     = 35460,
	NPC_ROYAL_GUARD       = 35322,
};

enum AllianceEvents
{
	EVENT_BELUUS_ALLIANCE      = 1,
	EVENT_VARIAN_STEP_1        = 2,
	EVENT_TIRION_ALLIANCE_1    = 3,
	EVENT_JAINA_STEP_1         = 4,
	EVENT_VARIAN_STEP_2        = 5,
	EVENT_TIRION_ALLIANCE_2    = 6,
	EVENT_JAINA_STEP_2         = 7,
	EVENT_VARIAN_STEP_3        = 8,
	EVENT_TIRION_ALLIANCE_3    = 9,
	EVENT_ALLIANCE_TO_COLISEUM = 10,
	// Horde Events
	EVENT_BELUUS_HORDE         = 11,
	EVENT_THRALL_1             = 12,
	EVENT_GARROSH_1            = 13,
	EVENT_THRALL_2             = 14,
	EVENT_GARROSH_2            = 15,
	EVENT_TIRION_H_1           = 16,
	EVENT_THRALL_3             = 17,
	EVENT_GARROSH_3            = 18,
	EVENT_TIRION_H_2           = 19,
	EVENT_THRALL_4             = 20,
	EVENT_GARROSH_4            = 21,
	EVENT_THRALL_5             = 22,
	EVENT_TIRION_H_3           = 23,
	EVENT_THRALL_6             = 24,
	EVENT_GARROSH_5            = 25,
	EVENT_TIRION_H_4           = 26,
	EVENT_HORDE_TO_COLISEUM    = 27,
};

/*Alliance Dialog*/
#define SAY_BELUUS_ALLIANCE "His Majesty, King Varian Wrynn, and Lady Jaina Proudmoore have touched down upon the tournament grounds! Make way!"
#define SAY_TIRION_ALLIANCE_1 "King Varian, Lady Jaina. Welcome to the tournament. Your place in the coliseum is waiting."
#define SAY_TIRION_ALLIANCE_2 "My people are here to ensure that they honor the rules of engagement. It will do your subjects well to see you in the stands."
#define SAY_TIRION_ALLIANCE_3 "If you will follow me, then."
#define SAY_VARIAN_1 "Tirion."
#define SAY_VARIAN_2 "You ask much of me, Tirion, to sit and watch, trusting the savages to keep themselves in check."
#define SAY_VARIAN_3 "Perhaps. I doubt the effectiveness of this gambit. We would be better served preparing our armies for the final assault, but if my presence is required in these games to gain your support in the battle, then I will remain. For now."
#define SAY_JAINA_1 "Thank you. It's the least I can do to support this; we need to stand together."
#define SAY_JAINA_2 "I'm sure Thrall will have his people under control..."
/*Horde Dialog*/
#define SAY_BELUUS_HORDE "The Warchief of the Horde, Thrall, and Overlord Garrosh Hellscream have arrived upon the tournament grounds! Clear the way to the entrance!"
#define SAY_THRALL_1 "Garrosh, I expect you to control yourself here. I do not want a repeat of the Violet Citadel."
#define SAY_GARROSH_1 "Pah! That was a show of strength, worthy of a leader. I only regret I did not kill that human before the mage interfered."
#define SAY_THRALL_2 "We are guests here, Garrosh, and you will conduct yourself honorably."
#define SAY_GARROSH_2 "What honor is there in thrashing about with blunted sticks? This is a waste of time."
#define SAY_TIRION_H_1 "Welcome, Warchief Thrall. Overlord Hellscream."
#define SAY_THRALL_3 "Thank you for the invitation, Lord Fordring. We look forward to observing these games."
#define SAY_GARROSH_3 "Speak for yourself, Thrall."
#define SAY_TIRION_H_2 "I hope you'll see the merits of these events in time, Garrosh. We cannot win against the Scourge if we continue to war against one another."
#define SAY_THRALL_4 "Wise words."
#define SAY_GARROSH_4 "Words of a fool, you mean. The Horde will destroy the undead without your aid, human, or that of the pompous king."
#define SAY_THRALL_5 "Garrosh, enough!"
#define SAY_TIRION_H_3 "Under my roof, gentlemen, I trust you will behave."
#define SAY_THRALL_6 "Of course, Tirion. I apologize for his outburst. It will not be repeated."
#define SAY_GARROSH_5 "Bah."
#define SAY_TIRION_H_4 "If you will follow me, then."

Position const TirionMovePos[1] =
{
	{8514.91f, 835.369f, 558.566f, 4.77741f}, // Tirion
};

Position const AlliancePos[2] =
{
	{8520.44f, 835.105f, 558.553f, 4.73185f}, // Varian
	{8517.28f, 835.605f, 558.564f, 4.72949f}, // Jaina
};

Position const HordePos[2] =
{
	{8511.37f, 836.046f, 558.558f, 4.71457f}, // Thrall
	{8511.65f, 835.469f, 558.559f, 4.75699f}, // Garrosh
};

class npc_higlord_tirion_fordring_atg : public CreatureScript
{
public:
	npc_higlord_tirion_fordring_atg() : CreatureScript("npc_higlord_tirion_fordring_atg") { }

	struct npc_higlord_tirion_fordring_atgAI  : public QuantumBasicAI
    {
		npc_higlord_tirion_fordring_atgAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		bool EventAlliance;
		bool EventHorde;

		void Reset()
		{
			events.Reset();
			EventAlliance = true;
			EventHorde = true;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void MoveInLineOfSight(Unit* who)
		{
			if (Player* player = who->ToPlayer())
			{
				if (player->IsWithinDist(me, 100.0f))
				{
					if (who->ToPlayer()->GetTeamId() == TEAM_ALLIANCE)
					{
						if (EventAlliance)
						{
							events.ScheduleEvent(EVENT_BELUUS_ALLIANCE, 3000);
							EventAlliance = false;
						}
						else
						{
							if (!EventHorde)
								EventAlliance = false;
						}
					}

					if (who->ToPlayer()->GetTeamId() == TEAM_HORDE)
					{
						if (EventHorde)
						{
							events.ScheduleEvent(EVENT_BELUUS_HORDE, 3000);
							EventHorde = false;
						}
						else
						{
							if (!EventAlliance)
								EventHorde = false;
						}
					}
				}
			}
		}

		void UpdateAI(uint32 const diff)
		{
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
					// Alliance Event
				    case EVENT_BELUUS_ALLIANCE:
						if (Creature* Beluus = me->FindCreatureWithDistance(NPC_HERALD_BELUUS, 150.0f))
						{
							Beluus->MonsterYell(SAY_BELUUS_ALLIANCE, LANG_UNIVERSAL, 0);
						}
						events.ScheduleEvent(EVENT_VARIAN_STEP_1, 7000);
						break;
					case EVENT_VARIAN_STEP_1:
						if (Creature* Varian = me->FindCreatureWithDistance(NPC_KING_VARIAN_WRYNN, 150.0f))
						{
							Varian->MonsterSay(SAY_VARIAN_1, LANG_UNIVERSAL, 0);
							Varian->PlayDirectSound(16070);
						}
						events.ScheduleEvent(EVENT_TIRION_ALLIANCE_1, 2000);
						break;
					case EVENT_TIRION_ALLIANCE_1:
						me->MonsterSay(SAY_TIRION_ALLIANCE_1, LANG_UNIVERSAL, 0);
						me->PlayDirectSound(16054);
						events.ScheduleEvent(EVENT_JAINA_STEP_1, 7000);
						break;
					case EVENT_JAINA_STEP_1:
						if (Creature* Jaina = me->FindCreatureWithDistance(NPC_JAINA_PROUDMOORE, 150.0f))
						{
							Jaina->MonsterSay(SAY_JAINA_1, LANG_UNIVERSAL, 0);
							Jaina->PlayDirectSound(16120);
						}
						events.ScheduleEvent(EVENT_VARIAN_STEP_2, 8000);
						break;
					case EVENT_VARIAN_STEP_2:
						if (Creature* Varian = me->FindCreatureWithDistance(NPC_KING_VARIAN_WRYNN, 150.0f))
						{
							Varian->MonsterSay(SAY_VARIAN_2, LANG_UNIVERSAL, 0);
							Varian->PlayDirectSound(16071);
						}
						events.ScheduleEvent(EVENT_TIRION_ALLIANCE_2, 10000);
						break;
					case EVENT_TIRION_ALLIANCE_2:
						me->MonsterSay(SAY_TIRION_ALLIANCE_2, LANG_UNIVERSAL, 0);
						me->PlayDirectSound(16055);
						events.ScheduleEvent(EVENT_JAINA_STEP_2, 10000);
						break;
					case EVENT_JAINA_STEP_2:
						if (Creature* Jaina = me->FindCreatureWithDistance(NPC_JAINA_PROUDMOORE, 150.0f))
						{
							Jaina->MonsterSay(SAY_JAINA_2, LANG_UNIVERSAL, 0);
							Jaina->PlayDirectSound(16121);
						}
						events.ScheduleEvent(EVENT_VARIAN_STEP_3, 7000);
						break;
					case EVENT_VARIAN_STEP_3:
						if (Creature* Varian = me->FindCreatureWithDistance(NPC_KING_VARIAN_WRYNN, 150.0f))
						{
							Varian->MonsterSay(SAY_VARIAN_3, LANG_UNIVERSAL, 0);
							Varian->PlayDirectSound(16072);
						}
						events.ScheduleEvent(EVENT_TIRION_ALLIANCE_3, 16000);
						break;
					case EVENT_TIRION_ALLIANCE_3:
						me->MonsterSay(SAY_TIRION_ALLIANCE_3, LANG_UNIVERSAL, 0);
						me->PlayDirectSound(16059);
						events.ScheduleEvent(EVENT_ALLIANCE_TO_COLISEUM, 4000);
						break;
					case EVENT_ALLIANCE_TO_COLISEUM:
						me->GetMotionMaster()->MovePoint(0, TirionMovePos[1]);
						me->SetSpeed(MOVE_RUN, 1.0f);
						me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
						me->DespawnAfterAction(11*IN_MILLISECONDS);

						if (Creature* Varian = me->FindCreatureWithDistance(NPC_KING_VARIAN_WRYNN, 150.0f))
						{
							Varian->GetMotionMaster()->MovePoint(0, AlliancePos[0]);
							Varian->SetSpeed(MOVE_RUN, 1.0f);
							Varian->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
							Varian->DespawnAfterAction(11*IN_MILLISECONDS);
						}
						if (Creature* Jaina = me->FindCreatureWithDistance(NPC_JAINA_PROUDMOORE, 150.0f))
						{
							Jaina->GetMotionMaster()->MovePoint(0, AlliancePos[1]);
							Jaina->SetSpeed(MOVE_RUN, 1.0f);
							Jaina->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
							Jaina->DespawnAfterAction(11*IN_MILLISECONDS);
						}
						break;
						// Horde Event
                    case EVENT_BELUUS_HORDE:
						if (Creature* Beluus = me->FindCreatureWithDistance(NPC_HERALD_BELUUS, 150.0f))
						{
							Beluus->MonsterYell(SAY_BELUUS_HORDE, LANG_UNIVERSAL, 0);
						}
						events.ScheduleEvent(EVENT_THRALL_1, 4000);
						break;
					case EVENT_THRALL_1:
						if (Creature* Thrall = me->FindCreatureWithDistance(NPC_THRALL, 150.0f))
						{
							Thrall->MonsterSay(SAY_THRALL_1, LANG_UNIVERSAL, 0);
							Thrall->PlayDirectSound(16181);
						}
						events.ScheduleEvent(EVENT_GARROSH_1, 10000);
						break;
					case EVENT_GARROSH_1:
						if (Creature* Garrosh = me->FindCreatureWithDistance(NPC_GARROSH, 150.0f))
						{
							Garrosh->MonsterSay(SAY_GARROSH_1, LANG_UNIVERSAL, 0);
							Garrosh->PlayDirectSound(16027);
						}
						events.ScheduleEvent(EVENT_THRALL_2, 10000);
						break;
					case EVENT_THRALL_2:
						if (Creature* Thrall = me->FindCreatureWithDistance(NPC_THRALL, 150.0f))
						{
							Thrall->MonsterSay(SAY_THRALL_2, LANG_UNIVERSAL, 0);
							Thrall->PlayDirectSound(16182);
						}
						events.ScheduleEvent(EVENT_GARROSH_2, 6000);
						break;
					case EVENT_GARROSH_2:
						if (Creature* Garrosh = me->FindCreatureWithDistance(NPC_GARROSH, 150.0f))
						{
							Garrosh->MonsterSay(SAY_GARROSH_2, LANG_UNIVERSAL, 0);
							Garrosh->PlayDirectSound(16028);
						}
						events.ScheduleEvent(EVENT_TIRION_H_1, 8000);
						break;
					case EVENT_TIRION_H_1:
						me->MonsterSay(SAY_TIRION_H_1, LANG_UNIVERSAL, 0);
						me->PlayDirectSound(16056);
						events.ScheduleEvent(EVENT_THRALL_3, 5000);
						break;
					case EVENT_THRALL_3:
						if (Creature* Thrall = me->FindCreatureWithDistance(NPC_THRALL, 150.0f))
						{
							Thrall->MonsterSay(SAY_THRALL_3, LANG_UNIVERSAL, 0);
							Thrall->PlayDirectSound(16183);
						}
						events.ScheduleEvent(EVENT_GARROSH_3, 8000);
						break;
					case EVENT_GARROSH_3:
						if (Creature* Garrosh = me->FindCreatureWithDistance(NPC_GARROSH, 150.0f))
						{
							Garrosh->MonsterSay(SAY_GARROSH_3, LANG_UNIVERSAL, 0);
							Garrosh->PlayDirectSound(16029);
						}
						events.ScheduleEvent(EVENT_TIRION_H_2, 4000);
						break;
					case EVENT_TIRION_H_2:
						me->MonsterSay(SAY_TIRION_H_2, LANG_UNIVERSAL, 0);
						me->PlayDirectSound(16057);
						events.ScheduleEvent(EVENT_THRALL_4, 8000);
						break;
					case EVENT_THRALL_4:
						if (Creature* Thrall = me->FindCreatureWithDistance(NPC_THRALL, 150.0f))
						{
							Thrall->MonsterSay(SAY_THRALL_4, LANG_UNIVERSAL, 0);
							Thrall->PlayDirectSound(16184);
						}
						events.ScheduleEvent(EVENT_GARROSH_4, 4000);
						break;
					case EVENT_GARROSH_4:
						if (Creature* Garrosh = me->FindCreatureWithDistance(NPC_GARROSH, 150.0f))
						{
							Garrosh->MonsterSay(SAY_GARROSH_4, LANG_UNIVERSAL, 0);
							Garrosh->PlayDirectSound(16030);
						}
						events.ScheduleEvent(EVENT_THRALL_5, 10000);
						break;
					case EVENT_THRALL_5:
						if (Creature* Thrall = me->FindCreatureWithDistance(NPC_THRALL, 150.0f))
						{
							Thrall->MonsterSay(SAY_THRALL_5, LANG_UNIVERSAL, 0);
							Thrall->PlayDirectSound(16185);
						}
						events.ScheduleEvent(EVENT_TIRION_H_3, 3000);
						break;
					case EVENT_TIRION_H_3:
						me->MonsterSay(SAY_TIRION_H_3, LANG_UNIVERSAL, 0);
						me->PlayDirectSound(16058);
						events.ScheduleEvent(EVENT_THRALL_6, 5000);
						break;
					case EVENT_THRALL_6:
						if (Creature* Thrall = me->FindCreatureWithDistance(NPC_THRALL, 150.0f))
						{
							Thrall->MonsterSay(SAY_THRALL_6, LANG_UNIVERSAL, 0);
							Thrall->PlayDirectSound(16186);
						}
						events.ScheduleEvent(EVENT_GARROSH_5, 8000);
						break;
					case EVENT_GARROSH_5:
						if (Creature* Garrosh = me->FindCreatureWithDistance(NPC_GARROSH, 150.0f))
						{
							Garrosh->MonsterSay(SAY_GARROSH_5, LANG_UNIVERSAL, 0);
							Garrosh->PlayDirectSound(16031);
						}
						events.ScheduleEvent(EVENT_TIRION_H_4, 3000);
						break;
					case EVENT_TIRION_H_4:
						me->MonsterSay(SAY_TIRION_H_4, LANG_UNIVERSAL, 0);
						me->PlayDirectSound(16059);
						events.ScheduleEvent(EVENT_HORDE_TO_COLISEUM, 4000);
						break;
					case EVENT_HORDE_TO_COLISEUM:
						me->GetMotionMaster()->MovePoint(0, TirionMovePos[1]);
						me->SetSpeed(MOVE_RUN, 1.0f);
						me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
						me->DespawnAfterAction(11*IN_MILLISECONDS);

						if (Creature* Thrall = me->FindCreatureWithDistance(NPC_THRALL, 150.0f))
						{
							Thrall->GetMotionMaster()->MovePoint(0, HordePos[0]);
							Thrall->SetSpeed(MOVE_RUN, 1.0f);
							Thrall->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
							Thrall->DespawnAfterAction(11*IN_MILLISECONDS);
						}

						if (Creature* Garrosh = me->FindCreatureWithDistance(NPC_GARROSH, 150.0f))
						{
							Garrosh->GetMotionMaster()->MovePoint(0, HordePos[1]);
							Garrosh->SetSpeed(MOVE_RUN, 1.0f);
							Garrosh->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
							Garrosh->DespawnAfterAction(11*IN_MILLISECONDS);
						}
						break;
				}
			}
		}
	};

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_higlord_tirion_fordring_atgAI(creature);
    }
};

enum Spells
{
	SPELL_MUSIC_START           = 75760,
    SPELL_MUSIC                 = 75765,
    SPELL_MUSIC_END             = 75767,
    SPELL_TRIGGER               = 72400, // 100% wrong. Used for some special cases
    SPELL_HEALTH_REGEN          = 74503,
    SPELL_BRILLIANT_TACTICS     = 74501,
    SPELL_ATTACK                = 74413,
    SPELL_SHOOT                 = 74414,
    SPELL_GRENADE               = 74707,
    SPELL_RAD_GRENADE           = 74764,
    SPELL_MACHINE_GUN           = 74438,
    SPELL_FLAME_SPRAY           = 74440,
    SPELL_EXPLOSION             = 74550,
    SPELL_CANNON_SHOT           = 74307,
    SPELL_CANNON_SHIELD         = 74458,
    SPELL_ROCKET                = 64979, // May be wrong
    SPELL_PARACHUTE             = 79397,
    SPELL_PARACHUTE_AURA        = 79404,
    SPELL_WRENCH_THROW          = 74785, // Seems to have wrong throw model
	SPELL_ATTACH_CAMERA         = 75512, // [Unused]
    SPELL_SPAWN_INVISIBILITY    = 75513,
    SPELL_SEE_INVISIBILITY      = 75514,
    SPELL_BINDSIGHT             = 75517, // Custom spell
	SPELL_RECALL_TRIGGER        = 75553, // Triggers Camera Vehicle summon [Unused]
    SPELL_RECALL                = 75510, // Teleport to camera start position
    SPELL_RECALL_FINAL          = 74412, // Realy final?
    SPELL_RAD_EXPLOSION         = 75545,
};

enum Creatures
{
    NPC_MEKKATORQUE             = 39271,
    NPC_BATTLE_SUIT             = 39902,
    NPC_FASTBLAST               = 39910,
    NPC_INFANTRY                = 39252,
    NPC_COGSPIN                 = 39273,
    NPC_BOMBER                  = 39735,
    NPC_ELGIN                   = 40478,
    NPC_TANK                    = 39860,
    NPC_I_INFANTRY              = 39755,
    NPC_IRRADIATOR              = 39903,
    NPC_I_CAVALRY               = 39836,
    NPC_GASHERIKK               = 39799,
    NPC_BRAG_BOT                = 39901,
    NPC_BOLTCOG                 = 39837,
    NPC_I_TROGG                 = 39826,
    NPC_I_TANK                  = 39819,
    NPC_CANNON                  = 39759, // Tankbuster Cannon
    NPC_RL                      = 39820, // Rocket Launcher
    NPC_EXPLOSION_BUNNY         = 40506,
    NPC_CAMERA_VEHICLE          = 40479,
};

enum Objects
{
    GO_BANNER                  = 194498,
    GO_RAD_CONTROL             = 202767,
    GO_IRRADIATOR              = 202922,
};

enum Misc
{
    QUEST_OPERATION_GNOMEREGAN  = 25393,
    DATA_MOUNT_MEK              = 31692,
    DATA_MOUNT_FAST             = 6569,
    DATA_MOUNT_COG              = 9473,
    FACTION_GNOMEREGAN          = 1770,
};

enum eEvents
{
    EVENT_SUMMON_TROGG          = 0,
};

enum Worldstates
{
    WORLDSTATE_IN_PROCCESS                              = 4981,
    WORLDSTATE_RL_DESTROYED                             = 5027,
    WORLDSTATE_RL_DESTROYED_CTRL                        = 5040,
    WORLDSTATE_BATTLE_NEAR_WORKSHOPS                    = 5018,
    WORLDSTATE_AIRFIELD_ATTACKED                        = 5039,
    WORLDSTATE_AIRFIELD_CAPTURED                        = 5044,
    WORLDSTATE_CANNONS_DESTROYED                        = 5006,
    WORLDSTATE_CANNONS_DESTROYED_CTRL                   = 5018,
    WORLDSTATE_AIRFIELD_AND_COMMAND_CENTER_CAPTURED     = 5043,
    WORLDSTATE_BATTLE_NEAR_ENTRANCE                     = 5019,
    WORLDSTATE_SURFACE_CAPTURED                         = 5045,
    WORLDSTATE_BATLLE_IN_TUNNELS                        = 5021,
    WORLDSTATE_TUNNELS_CAPTURED                         = 5011,
    WORLDSTATE_COUNTDOWN                                = 5037,
    WORLDSTATE_COUNTDOWN_CTRL                           = 5038,
};

const Position iInfantrySpawn[26] =
{
    {-5355.43f, 530.67f, 385.17f, 3.37f},
    {-5356.68f, 535.13f, 385.54f, 3.43f},
    {-5360.16f, 549.49f, 387.26f, 2.87f},
    {-5360.22f, 555.62f, 387.21f, 3.02f},
    {-5359.59f, 561.08f, 387.01f, 3.02f},
    {-5336.43f, 541.66f, 384.97f, 2.67f},
    {-5333.62f, 541.52f, 384.91f, 3.29f},
    {-5345.95f, 532.78f, 384.68f, 2.82f},
    {-5145.95f, 452.20f, 393.56f, 2.32f},
    {-5137.67f, 460.88f, 393.24f, 2.34f},
    {-5115.88f, 434.75f, 397.49f, 1.91f},
    {-5128.01f, 430.56f, 396.55f, 1.70f},
    {-5107.13f, 462.23f, 402.25f, 3.70f},
    {-5103.22f, 455.80f, 402.69f, 3.66f},
    {-5095.57f, 470.80f, 403.48f, 3.96f},
    {-5090.65f, 466.22f, 404.34f, 3.96f},
    {-5081.96f, 453.46f, 409.65f, 2.18f},
    {-5087.25f, 448.15f, 409.30f, 2.17f},
    {-5189.41f, 583.61f, 404.02f, 4.82f},
    {-5178.16f, 583.22f, 403.94f, 4.60f},
    {-5193.70f, 594.34f, 408.54f, 5.20f},
    {-5195.24f, 597.54f, 409.39f, 5.20f},
    {-5191.03f, 598.42f, 408.85f, 4.92f},
    {-5171.88f, 593.29f, 407.82f, 4.17f},
    {-5174.75f, 596.12f, 408.08f, 4.17f},
    {-5171.76f, 597.32f, 408.66f, 4.17f}
};

const Position iTankSpawn[4] =
{
    {-5338.99f, 535.53f, 384.99f, 2.88f},
    {-5349.29f, 556.11f, 385.13f, 3.47f},
    {-5303.43f, 584.31f, 389.94f, 3.79f},
    {-5085.88f, 475.44f, 402.24f, 3.92f}
};

const Position iSoldierSpawn[5] =
{
    {-5040.80f, 739.75f, 256.47f, 6.2f},
    {-5043.66f, 731.75f, 256.47f, 6.2f},
    {-5039.70f, 734.17f, 256.47f, 6.2f},
    {-5044.24f, 737.27f, 256.47f, 6.2f},
    {-5046.30f, 733.86f, 256.47f, 6.2f}
};

const Position InfantrySpawn[19] =
{
    {-5334.11f, 563.10f, 395.79f, 0.71f},
    {-5303.73f, 588.68f, 390.12f, 5.27f},
    {-5308.05f, 587.04f, 389.93f, 5.08f},
    {-5271.25f, 572.90f, 387.12f, 4.93f},
    {-5280.78f, 564.26f, 386.08f, 5.65f},
    {-5085.68f, 480.86f, 401.69f, 4.12f},
    {-5080.83f, 477.69f, 401.76f, 4.12f},
    {-5085.44f, 446.00f, 410.11f, 2.51f},
    {-5081.82f, 451.51f, 410.33f, 2.46f},
    {-5152.82f, 455.32f, 392.82f, 2.49f},
    {-5142.35f, 468.65f, 392.38f, 2.33f},
    {-5191.10f, 574.74f, 400.96f, 1.51f},
    {-5177.70f, 574.41f, 400.40f, 1.51f},
    {-4975.04f, 736.62f, 256.91f, 3.04f},
    {-4976.25f, 723.63f, 256.91f, 3.04f},
    {-5103.44f, 724.77f, 257.77f, 6.24f},
    {-5109.18f, 725.26f, 255.98f, 6.24f},
    {-5104.79f, 717.42f, 257.53f, 6.24f},
    {-5109.95f, 717.81f, 255.92f, 6.24f}
};

const Position RLSpawn[4] =
{
    {-5350.35f, 574.63f, 387.12f, 4.7f},
    {-5331.62f, 545.17f, 385.22f, 5.0f},
    {-5324.26f, 588.66f, 389.01f, 3.7f},
    {-5294.58f, 571.73f, 386.93f, 5.3f}
};

const Position CannonSpawn[6] =
{
    {-5140.57f, 428.39f, 395.89f, 2.1f},
    {-5109.37f, 426.51f, 399.66f, 2.4f},
    {-5109.00f, 481.10f, 398.45f, 3.9f},
    {-5071.21f, 457.83f, 410.94f, 3.0f},
    {-5087.23f, 434.92f, 411.78f, 1.9f},
    {-5092.15f, 450.84f, 406.63f, 1.4f}
};

const Position BattleSuitDriverSpawn[3] =
{
    {-5158.31f, 470.63f, 390.47f, 5.0f},
    {-5156.70f, 473.46f, 390.56f, 5.0f},
    {-5154.23f, 475.71f, 390.82f, 5.0f}
};

const Position BattleSuitSpawn[6] =
{
    {-5073.81f, 481.83f, 401.48f, 5.0f},
    {-5069.55f, 485.22f, 401.49f, 4.8f},
    {-5064.04f, 486.30f, 401.48f, 4.8f},
    {-5086.02f, 713.15f, 260.56f, 6.3f},
    {-5085.04f, 725.92f, 260.56f, 6.3f},
    {-5085.31f, 477.48f, 401.95f, 3.9f}
};

const Position TankSpawn[5] =
{
    {-5402.421387f, 543.972229f, 387.243896f, 6.1f},
    {-5404.228516f, 530.777527f, 387.151581f, 0.2f},
    {-5394.222656f, 519.042908f, 386.308929f, 1.0f},
    {-5280.954102f, 559.265808f, 385.679932f, 5.0f},
    {-5263.007813f, 572.262085f, 388.673157f, 5.0f}
};

const Position BragBotSpawn[2] =
{
    {-5183.73f, 599.90f, 408.96f, 4.6f},
    {-4933.62f, 717.18f, 261.65f, 1.6f}
};

const uint32 WorldStates[15] =
{
    4981,
    5027,
    5040,
    5018,
    5039,
    5044,
    5006,
    5018,
    5043,
    5019,
    5045,
    5021,
    5011,
    5037,
    5038
};

const Position BomberSpawn         = {-5314.53f, 564.82f, 391.43f, 5.7f};
const Position TroggSpawn          = {-5181.74f, 631.21f, 398.54f, 4.7f};
const Position ExplosionBunnySpawn = {-5183.24f, 608.97f, 410.89f, 4.7f};

//#define SOUND_NAME            soundID // Duration
#define MEK_1_1                 "Citizens and Friends of Gnomeregan!"
#define MEK_1_2                 "Today we could wrest from the vile traitor Thermaplugg our great city!"
#define MEK_1_3                 "Let us stand shoulder to shoulder in the battle for their country!"
#define SOUND_MEK_1             17535 // 17150
#define MEK_2_1                 "Our people knew enough to torment and humiliation. We were outcasts, slaves, pulling the yoke of the combat vehicle of a madman!"
#define SOUND_MEK_2             17536 // 12200
#define MEK_3_1                 "Thermaplugg Board comes to an end!"
#define MEK_3_2                 "I declare the general assembly!"
#define SOUND_MEK_3             17537 // 7050
#define MEK_4_1                 "Military tactics and scientific progress, along with our determination to lead us to victory!"
#define MEK_4_2                 "Getting offensive!"
#define SOUND_MEK_4             17538 // 10180
#define MEK_5_1                 "Operation 'Gnomeregan' begins!"
#define MEK_5_2                 "First step: the capture of the airfield."
#define MEK_5_3                 "Destroy the enemy rocket launcher, and we will get air superiority!"
#define SOUND_MEK_5             17539 // 13500
#define MEK_6_1                 "The goal in sight! Guns for battle! Destroy the enemy rocket launcher!"
#define SOUND_MEK_6             17541 // 6730
#define MEK_7_1                 "Haha, this is ready! We continue to advance!"
#define SOUND_MEK_7             17542 // 4970
#define MEK_8_1                 "Haha, another broken! Do not loosen the onslaught!"
#define SOUND_MEK_8             17543 // 6160
#define MEK_9_1                 "Good work! Keep up the great work."
#define SOUND_MEK_9             17544 // 4140
#define MEK_10_1                "The airfield was taken, and the sky above our Gnomeregan!"
#define MEK_10_2                "Now our bombers can destroy protected by anti-tank guns Thermaplugg."
#define MEK_10_3                "The second phase of the operation: the capture of ground command post!"
#define SOUND_MEK_10            17546 // 16970
#define MEK_11_1                "Our bombers destroyed the protection of anti-tank guns! Destroy them and ensure the safety of the ground command center!"
#define SOUND_MEK_11            17547 // 9740
#define MEK_12_1                "The ground part of the city captured. Operation 'Gnomeregan' runs with the efficiency of 93 percent ... All praise!"
#define MEK_12_2                "Disables protective pump radiation. Units grab the remaining combat robots and move to the main entrance!"
#define SOUND_MEK_12            17548 // 16660
#define MEK_13_1                "We are on the verge of victory, my friends! The third step: enter the tunnel!"
#define MEK_13_2                "Once seize the Underground Railroad and the loading platform, we will be able to break into the heart of the city!"
#define MEK_13_3                "The victory is close!"
#define SOUND_MEK_13            17549 // 17600
#define THERM_1_1               "NEVER !!! So ... It's included? This button? One-two, one-two ... Oh!"
#define THERM_1_2               "NEVER !!! I gnomeregan my Mekkatorque!"
#define THERM_1_3               "I think you just will enter in my city?"
#define THERM_1_4               "Activate single nuclear defense system! RELEASE Troggs!"
#define SOUND_THERM_1           17569 // 23190
#define MEK_14_1                "Firs-gears ... Yes, there seems to have gathered all the Troggs city! Soldiers dig in!"
#define MEK_14_2                "Fragmentation grenade and a barrage! HOLD THE LINE! Retreat is impossible!"
#define SOUND_MEK_14            17550 // 14230
#define MEK_15_1                "Thousand snotty Titans! I've never seen such a huge Troggs! Continue shelling ... destroy it !!!"
#define SOUND_MEK_15            17551 // 9560
#define MEK_16_1                "Too quiet. Where troops Thermaplugg?"
#define SOUND_MEK_16            17552 // 5520
#define MEK_17_1                "What is it? It looks like a nuclear bomb, which killed Gnomeregan ... Whoa! The enemy is coming!"
#define SOUND_MEK_17            17553 // 9690
#define BOLTCOG_1               "Our defense will not be overcome. We are true sons of Gnomeregan!"
#define SOUND_BOLTCOG_1         17665 // 6100
#define THERM_2_1               "No! No no no! I will not let you get into your kingdom, the usurper!"
#define THERM_2_2               "Activate nuclear defense system!"
#define THERM_2_3               "Disintegrate them all!"
#define SOUND_THERM_2           17570 // 14640
#define MEK_18_1                "IT'S A TRAP! It employs irradiator!"
#define SOUND_MEK_18            17554 // 3830
#define THERM_3_1               "And there is. And she's twenty-six times more powerful than the first!"
#define THERM_3_2               "Mekkatorque, you lose! I beat you earlier than you had come to my beautiful city."
#define THERM_3_3               "Now die!"
#define THERM_3_4               "Oh, I'd like to see the face of this rogue. I swear it... What? Still it included? Turn off !!!"
#define SOUND_THERM_3           17571 // 22980
#define IRRADIATOR_1_1          "The irradiator 3000 is activated. Attention. Emergency. You've got 10 minutes to be at a minimum safe distance."
#define SOUND_IRRADIATOR_1      17635 // 10830
#define MEK_19_1                "Ten minutes? We have a hundred times to defuse the device."
#define MEK_19_2                "Hinkls, drag-ka Whirring Bronze Gizmo and give fresh water! Gears, toss me a dirty robe Troggs and let a handful of brass screws..."
#define SOUND_MEK_19            17556 // 14930
#define THERM_4_1               "TEN MINUTES ?! You left the factory settings? Idiot quickly give the regulator!"
#define SOUND_THERM_4           17572 // 9970
#define IRRADIATOR_2_1          "Attention. Emergency. You've got 10 seconds to be at a minimum safe distance."
#define SOUND_IRRADIATOR_2      17636 // 7670
#define MEK_20_1                "We must get out of here! LaForge rather Teleport us!"
#define SOUND_MEK_20            17557 // 4790
#define MEK_21_1                "Damn! We had to retreat. But Thermaplugg lost, and he knows it ... He just got a little leeway."
#define MEK_21_2                "We took over the surface, and when the radiation levels drop, back with twice the army and wrest Gnomeregan!"
#define SOUND_MEK_21            17558 // 19280

class npc_og_suit : public CreatureScript
{
    public:
        npc_og_suit() : CreatureScript("npc_og_suit") {}

        struct npc_og_suitAI : public npc_escortAI
        {
            npc_og_suitAI(Creature* creature) : npc_escortAI(creature) {}

            void WaypointReached(uint32 i) {}

            void SetupMovement(uint32 variation)
            {
                switch (variation)
                {
                    case 0:
                        AddWaypoint(0, -5063.989746f, 485.176605f, 401.554596f);
                        AddWaypoint(1, -5063.474121f, 475.547302f, 402.917847f);
                        AddWaypoint(2, -5076.217285f, 469.422577f, 403.661407f);
                        AddWaypoint(3, -5088.637207f, 462.860535f, 405.323853f);
                        AddWaypoint(4, -5095.611816f, 455.524353f, 404.919037f);
                        AddWaypoint(5, -5121.946777f, 444.406799f, 396.678772f);
                        AddWaypoint(6, -5137.889648f, 449.120300f, 394.424469f);
                        AddWaypoint(7, -5149.414063f, 457.053406f, 392.511566f);
                        AddWaypoint(8, -5160.174316f, 468.878296f, 390.560455f);
                        AddWaypoint(9, -5180.711914f, 484.161713f, 388.160278f);
                        AddWaypoint(10, -5189.939941f, 498.311462f, 387.892426f);
                        AddWaypoint(11, -5192.106934f, 510.962097f, 387.746155f);
                        AddWaypoint(12, -5193.543945f, 529.913147f, 388.696655f);
                        AddWaypoint(13, -5191.546387f, 550.148438f, 393.705780f);
                        AddWaypoint(14, -5188.546875f, 586.246826f, 404.783936f);
                        break;
                    case 1:
                        AddWaypoint(0, -5069.393066f, 483.890228f, 401.484985f);
                        AddWaypoint(1, -5068.837402f, 477.813507f, 402.159088f);
                        AddWaypoint(2, -5079.646484f, 472.017700f, 402.333557f);
                        AddWaypoint(3, -5092.053711f, 465.606750f, 404.372650f);
                        AddWaypoint(4, -5100.974609f, 458.543640f, 403.343445f);
                        AddWaypoint(5, -5123.000488f, 449.164886f, 396.634552f);
                        AddWaypoint(6, -5135.452637f, 452.087646f, 394.234161f);
                        AddWaypoint(7, -5145.298340f, 459.457916f, 392.519470f);
                        AddWaypoint(8, -5155.757813f, 471.958282f, 390.643585f);
                        AddWaypoint(9, -5177.949219f, 486.992096f, 388.152344f);
                        AddWaypoint(10, -5185.644043f, 497.995270f, 387.916534f);
                        AddWaypoint(11, -5188.020508f, 510.212585f, 387.776581f);
                        AddWaypoint(12, -5189.084961f, 530.316467f, 388.683136f);
                        AddWaypoint(13, -5187.695313f, 553.590210f, 394.830505f);
                        AddWaypoint(14, -5183.902832f, 589.274170f, 405.504333f);
                        break;
                    case 2:
                        AddWaypoint(0, -5072.857910f, 480.193024f, 401.575989f);
                        AddWaypoint(1, -5081.426758f, 476.311279f, 401.928589f);
                        AddWaypoint(2, -5093.935059f, 470.681549f, 403.441467f);
                        AddWaypoint(3, -5102.093262f, 463.836243f, 403.325378f);
                        AddWaypoint(4, -5124.031250f, 453.702057f, 396.498871f);
                        AddWaypoint(5, -5132.045898f, 454.890564f, 394.364868f);
                        AddWaypoint(6, -5142.617188f, 463.545746f, 392.404541f);
                        AddWaypoint(7, -5152.081055f, 476.321075f, 391.080353f);
                        AddWaypoint(8, -5174.884766f, 489.358124f, 388.310547f);
                        AddWaypoint(9, -5181.639648f, 499.796936f, 387.963043f);
                        AddWaypoint(10, -5183.538574f, 509.857727f, 387.984863f);
                        AddWaypoint(11, -5183.844238f, 528.578613f, 388.656555f);
                        AddWaypoint(12, -5184.622559f, 548.945251f, 393.504517f);
                        AddWaypoint(13, -5179.573730f, 585.523010f, 404.546021f);
                        break;
                    case 3:
                        AddWaypoint(0, -5086.023926f, 713.154724f, 260.556427f);
                        AddWaypoint(1, -5076.176270f, 720.441162f, 260.530670f);
                        AddWaypoint(2, -5061.914063f, 721.010620f, 260.532135f);
                        AddWaypoint(3, -5057.044434f, 728.671509f, 260.554199f);
                        AddWaypoint(4, -5054.104980f, 728.641357f, 261.240845f);
                        AddWaypoint(5, -5046.533691f, 731.763855f, 256.475403f);
                        AddWaypoint(6, -4975.580078f, 725.948120f, 256.266113f);
                        AddWaypoint(7, -4948.541992f, 724.022522f, 260.442596f);
                        AddWaypoint(8, -4947.177246f, 723.911560f, 261.646118f);
                        AddWaypoint(9, -4943.868164f, 723.567993f, 261.646118f);
                        AddWaypoint(10, -4945.804199f, 720.004578f, 261.646118f);
                        AddWaypoint(11, -4946.651367f, 721.550781f, 261.645966f);
                        break;
                    case 4:
                        AddWaypoint(0, -5085.565918f, 725.892761f, 260.554840f);
                        AddWaypoint(1, -5077.067383f, 730.409729f, 260.539093f);
                        AddWaypoint(2, -5064.545410f, 735.317627f, 260.517822f);
                        AddWaypoint(3, -5055.252441f, 737.436584f, 260.556335f);
                        AddWaypoint(4, -5053.518555f, 737.214722f, 261.237610f);
                        AddWaypoint(5, -5046.488281f, 737.037964f, 256.475586f);
                        AddWaypoint(6, -5039.229492f, 739.272888f, 256.475586f);
                        AddWaypoint(7, -4948.179199f, 731.976929f, 260.396484f);
                        AddWaypoint(8, -4946.559570f, 731.800903f, 261.645752f);
                        AddWaypoint(9, -4943.546875f, 731.456848f, 261.645752f);
                        AddWaypoint(10, -4944.289063f, 735.979248f, 261.645752f);
                        AddWaypoint(11, -4945.196289f, 733.968018f, 261.645752f);
                        break;
                    case 5:
                        AddWaypoint(0, -5085.312500f, 477.487366f, 401.958099f);
                        AddWaypoint(1, -5093.171387f, 469.209137f, 403.715790f);
                        AddWaypoint(2, -5100.262695f, 462.922180f, 403.585724f);
                        AddWaypoint(3, -5106.192871f, 458.708801f, 402.284393f);
                        AddWaypoint(4, -5113.296875f, 454.575165f, 400.071075f);
                        AddWaypoint(5, -5129.138184f, 448.644257f, 395.268951f);
                        AddWaypoint(6, -5144.433594f, 458.945282f, 392.658112f);
                        AddWaypoint(7, -5157.008301f, 472.717865f, 390.544586f);
                        AddWaypoint(8, -5167.264160f, 479.733826f, 389.530670f);
                        AddWaypoint(9, -5182.246582f, 490.885254f, 388.022522f);
                        AddWaypoint(10, -5188.400391f, 508.496002f, 387.779266f);
                        AddWaypoint(11, -5189.048828f, 533.551758f, 389.216064f);
                        AddWaypoint(12, -5185.604004f, 572.490662f, 400.257904f);
                        AddWaypoint(13, -5189.048828f, 533.551758f, 389.216064f);
                        AddWaypoint(14, -5188.400391f, 508.496002f, 387.779266f);
                        AddWaypoint(15, -5182.246582f, 490.885254f, 388.022522f);
                        AddWaypoint(16, -5167.264160f, 479.733826f, 389.530670f);
                        AddWaypoint(17, -5157.008301f, 472.717865f, 390.544586f);
                        AddWaypoint(18, -5144.433594f, 458.945282f, 392.658112f);
                        AddWaypoint(19, -5129.138184f, 448.644257f, 395.268951f);
                        AddWaypoint(20, -5113.296875f, 454.575165f, 400.071075f);
                        AddWaypoint(21, -5106.192871f, 458.708801f, 402.284393f);
                        AddWaypoint(22, -5100.262695f, 462.922180f, 403.585724f);
                        AddWaypoint(23, -5093.171387f, 469.209137f, 403.715790f);
                        break;
                    default:
                        sLog->OutErrorConsole("Unexpected movement variation (%i) in npc_og_suitAI::SetupMovement call!", variation);
                        return;
                }
                if (npc_og_suitAI* EscortAI = CAST_AI(npc_og_suitAI, me->AI()))
                {
                    if (variation == 5)
                        EscortAI->Start(true, true);
                    else
                    {
                        EscortAI->Start(true, true);
                        EscortAI->SetDespawnAtEnd(false);
                    }
                    EscortAI->SetDespawnAtFar(false);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                npc_escortAI::UpdateAI(diff);
            }
        };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_og_suitAI(creature);
    }
};

class npc_og_infantry : public CreatureScript
{
    public:
        npc_og_infantry() : CreatureScript("npc_og_infantry") {}

        struct npc_og_infantryAI : public npc_escortAI
        {
            npc_og_infantryAI(Creature* creature) : npc_escortAI(creature) {}

            uint32 CooldownTimer;
            uint32 GrenadeTimer;
            uint32 UVariation;

            void Reset()
            {
                CooldownTimer = 2500;
                GrenadeTimer = urand(10000, 15000);
            }

            void WaypointReached(uint32 id)
            {
                if (id == 9 && UVariation <= 3)
				{
                    if (Creature* suit = me->FindCreatureWithDistance(NPC_BATTLE_SUIT, 50.0f, true))
                    {
                        CAST_AI(npc_og_suit::npc_og_suitAI, suit->AI())->SetupMovement(UVariation);
                        suit->SetCurrentFaction(FACTION_GNOMEREGAN);
                    }
				}
            }

            void SetupMovement(uint32 variation)
            {
                switch (variation)
                {
                    case 0:
                        AddWaypoint(0, -5158.309082f, 470.632172f, 390.470764f);
                        AddWaypoint(1, -5145.815918f, 455.817719f, 392.905914f);
                        AddWaypoint(2, -5133.497070f, 447.780579f, 394.853760f);
                        AddWaypoint(3, -5123.107910f, 447.636963f, 396.523224f);
                        AddWaypoint(4, -5113.999512f, 450.278076f, 399.421631f);
                        AddWaypoint(5, -5099.395020f, 459.180542f, 403.653595f);
                        AddWaypoint(6, -5092.314941f, 464.330475f, 404.561371f);
                        AddWaypoint(7, -5077.594238f, 472.013489f, 402.421844f);
                        AddWaypoint(8, -5061.697754f, 479.314331f, 402.391602f);
                        AddWaypoint(9, -5064.035645f, 486.302704f, 401.484802f);
                        break;
                    case 1:
                        AddWaypoint(0, -5156.698242f, 473.458801f, 390.562836f);
                        AddWaypoint(1, -5143.689453f, 459.033173f, 392.722260f);
                        AddWaypoint(2, -5131.758301f, 450.333496f, 394.751221f);
                        AddWaypoint(3, -5124.032715f, 451.750183f, 396.452881f);
                        AddWaypoint(4, -5115.258789f, 453.386505f, 399.366028f);
                        AddWaypoint(5, -5100.275879f, 462.415070f, 403.588318f);
                        AddWaypoint(6, -5092.391602f, 467.532990f, 404.065125f);
                        AddWaypoint(7, -5077.200684f, 474.754211f, 402.065674f);
                        AddWaypoint(8, -5068.790039f, 478.631042f, 402.016907f);
                        AddWaypoint(9, -5069.553223f, 485.221466f, 401.486023f);
                        break;
                    case 2:
                        AddWaypoint(0, -5154.232910f, 475.711517f, 390.823730f);
                        AddWaypoint(1, -5140.836914f, 461.044586f, 392.818695f);
                        AddWaypoint(2, -5130.810059f, 454.405426f, 394.685913f);
                        AddWaypoint(3, -5124.641113f, 455.657715f, 396.326508f);
                        AddWaypoint(4, -5115.422852f, 456.902405f, 399.631653f);
                        AddWaypoint(5, -5101.960938f, 464.402954f, 403.353455f);
                        AddWaypoint(6, -5093.451172f, 469.957916f, 403.570282f);
                        AddWaypoint(7, -5078.112305f, 477.189697f, 401.767792f);
                        AddWaypoint(8, -5071.622559f, 476.510834f, 402.198334f);
                        AddWaypoint(9, -5073.813477f, 481.830627f, 401.484741f);
                        break;
                    case 15:
                        AddWaypoint(0, -5103.443359f, 724.770813f, 257.777954f);
                        AddWaypoint(1, -5094.822754f, 723.598328f, 260.490723f);
                        AddWaypoint(2, -5086.649902f, 723.079773f, 260.556946f);
                        AddWaypoint(3, -5055.190430f, 736.540833f, 260.557220f);
                        AddWaypoint(4, -5054.147949f, 736.447632f, 261.243988f);
                        AddWaypoint(5, -5047.112305f, 737.144043f, 256.501160f);
                        AddWaypoint(6, -5036.274414f, 739.345154f, 256.475739f);
                        AddWaypoint(7, -4976.154297f, 734.201904f, 256.277985f);
                        AddWaypoint(8, -4947.897461f, 732.244080f, 260.438263f);
                        AddWaypoint(9, -4946.339844f, 732.115906f, 261.646210f);
                        AddWaypoint(10, -4939.273438f, 734.958496f, 261.646210f);
                        AddWaypoint(11, -4938.026855f, 741.879028f, 261.644684f);
                        AddWaypoint(12, -4938.707520f, 740.875122f, 261.644684f);
                        break;
                    case 16:
                        AddWaypoint(0, -5109.180664f, 725.262878f, 255.981613f);
                        AddWaypoint(1, -5095.178711f, 724.099243f, 260.368286f);
                        AddWaypoint(2, -5086.729004f, 723.518494f, 260.557068f);
                        AddWaypoint(3, -5055.729004f, 735.350525f, 260.556061f);
                        AddWaypoint(4, -5054.227051f, 735.254822f, 261.244995f);
                        AddWaypoint(5, -5046.759766f, 735.879211f, 256.475433f);
                        AddWaypoint(6, -5035.186523f, 739.123962f, 256.475433f);
                        AddWaypoint(7, -4974.740234f, 734.052185f, 256.263763f);
                        AddWaypoint(8, -4963.679688f, 732.872192f, 257.911133f);
                        AddWaypoint(9, -4959.834961f, 734.894470f, 259.341797f);
                        AddWaypoint(10, -4963.027832f, 735.367432f, 258.783722f);
                        break;
                    case 17:
                        AddWaypoint(0, -5104.795898f, 717.425720f, 257.537598f);
                        AddWaypoint(1, -5095.783203f, 716.306946f, 260.370605f);
                        AddWaypoint(2, -5085.620117f, 716.721130f, 260.557373f);
                        AddWaypoint(3, -5056.110352f, 727.733398f, 260.561371f);
                        AddWaypoint(4, -5054.630371f, 727.904541f, 261.246185f);
                        AddWaypoint(5, -5047.102051f, 731.708801f, 256.475403f);
                        AddWaypoint(6, -5036.331055f, 730.962219f, 256.475403f);
                        AddWaypoint(7, -4974.770996f, 726.103333f, 256.258118f);
                        AddWaypoint(8, -4948.733398f, 723.634705f, 260.412781f);
                        AddWaypoint(9, -4947.263184f, 723.494263f, 261.646088f);
                        AddWaypoint(10, -4940.096191f, 717.815491f, 261.646088f);
                        AddWaypoint(11, -4939.854980f, 712.329895f, 261.644684f);
                        AddWaypoint(12, -4940.705566f, 713.400269f, 261.644684f);
                        break;
                    case 18:
                        AddWaypoint(0, -5109.956543f, 717.818481f, 255.921844f);
                        AddWaypoint(1, -5095.956055f, 716.377014f, 260.314667f);
                        AddWaypoint(2, -5085.541992f, 716.675903f, 260.557373f);
                        AddWaypoint(3, -5056.114258f, 727.704346f, 260.561310f);
                        AddWaypoint(4, -5054.369629f, 727.836121f, 261.243469f);
                        AddWaypoint(5, -5047.054199f, 731.594299f, 256.475586f);
                        AddWaypoint(6, -5036.313477f, 731.290100f, 256.475586f);
                        AddWaypoint(7, -4975.180664f, 725.747314f, 256.261780f);
                        AddWaypoint(8, -4964.648926f, 725.515686f, 257.849091f);
                        AddWaypoint(9, -4960.842285f, 722.796204f, 259.332458f);
                        AddWaypoint(10, -4964.441895f, 722.924133f, 258.709137f);
                        break;
                    default:
                        sLog->OutErrorConsole("Unexpected movement variation (%i) in npc_og_infantryAI::SetupMovement call!", variation);
                        return;
                }
                UVariation = variation;
                if (npc_og_infantryAI* EscortAI = CAST_AI(npc_og_infantryAI, me->AI()))
                {
                    EscortAI->Start(true, true);
                    EscortAI->SetDespawnAtFar(false);
                    if (variation > 14)
                        EscortAI->SetDespawnAtEnd(false);
                }
            }

            void AttackStart(Unit* who)
            {
                if (!who)
                    return;

                if (me->Attack(who, true))
                {
                    me->AddThreat(who, 10.0f);
                    me->SetInCombatWith(who);
                    who->SetInCombatWith(me);
                    DoStartMovement(who, 20.0f);
                    SetCombatMovement(true);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                npc_escortAI::UpdateAI(diff);

                if (!UpdateVictim())
                    return;

                if (me->IsWithinMeleeRange(me->GetVictim()))
                {
                    if (CooldownTimer <= diff)
                    {
                        if (!urand(0, 3))
                            DoCastVictim(SPELL_ATTACK);

                        CooldownTimer = 5000;
                    }
                    else CooldownTimer -= diff;
                }
                else
                {
                    if (CooldownTimer <= diff)
                    {
                        DoCastVictim(SPELL_SHOOT);
                        CooldownTimer = 3000;
                    }
                    else CooldownTimer -= diff;
                }

                if (GrenadeTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, SPELL_GRENADE);
						GrenadeTimer = urand(10000, 15000);
					}
                }
                else GrenadeTimer -= diff;
            }
        };

		CreatureAI* GetAI(Creature* creature) const
		{
			return new npc_og_infantryAI(creature);
		}
};

class npc_og_tank : public CreatureScript
{
    public:
        npc_og_tank() : CreatureScript("npc_og_tank") {}

        struct npc_og_tankAI : public npc_escortAI
        {
            npc_og_tankAI(Creature* creature) : npc_escortAI(creature) {}

            uint32 CooldownTimer;

            void Reset()
            {
                CooldownTimer = urand(5000, 7000);
            }

            void WaypointReached(uint32 /*i*/){}

            void SetupMovement(uint32 variation)
            {
                switch (variation)
                {
                    case 0:
                        AddWaypoint(0, -5402.410156f, 543.925049f, 387.239044f);
                        AddWaypoint(1, -5395.444824f, 543.309692f, 386.707794f);
                        AddWaypoint(2, -5386.854492f, 546.801086f, 386.116394f);
                        AddWaypoint(3, -5360.019043f, 558.631836f, 387.000153f);
                        AddWaypoint(4, -5335.933594f, 581.771667f, 386.494202f);
                        AddWaypoint(5, -5312.586426f, 572.810486f, 388.351349f);
                        AddWaypoint(6, -5303.000000f, 567.692078f, 386.120575f);
                        break;
                    case 1:
                        AddWaypoint(0, -5404.278809f, 530.734863f, 387.149506f);
                        AddWaypoint(1, -5395.379883f, 532.568787f, 387.045959f);
                        AddWaypoint(2, -5383.813477f, 538.125305f, 386.221008f);
                        AddWaypoint(3, -5358.499023f, 552.581299f, 387.086823f);
                        AddWaypoint(4, -5344.463867f, 553.109802f, 384.685608f);
                        AddWaypoint(5, -5318.844727f, 552.937134f, 385.461639f);
                        break;
                    case 2:
                        AddWaypoint(0, -5394.302734f, 518.734436f, 386.275909f);
                        AddWaypoint(1, -5388.748535f, 524.705505f, 386.650208f);
                        AddWaypoint(2, -5378.070313f, 529.968689f, 386.955872f);
                        AddWaypoint(3, -5353.919434f, 533.967957f, 385.093628f);
                        AddWaypoint(4, -5326.888184f, 539.787048f, 384.828491f);
                        break;
                    case 3:
                        AddWaypoint(0, -5280.954102f, 559.265808f, 385.679932f);
                        AddWaypoint(1, -5267.337402f, 540.788391f, 387.169342f);
                        AddWaypoint(2, -5255.488281f, 521.325256f, 386.325378f);
                        AddWaypoint(3, -5241.595215f, 513.660339f, 387.892212f);
                        AddWaypoint(4, -5229.519531f, 508.052399f, 387.756287f);
                        AddWaypoint(5, -5208.560059f, 495.748932f, 387.916199f);
                        AddWaypoint(6, -5196.718750f, 480.324402f, 387.072052f);
                        AddWaypoint(7, -5164.643555f, 447.614532f, 394.698822f);
                        AddWaypoint(8, -5137.479492f, 428.433716f, 395.987518f);
                        AddWaypoint(9, -5138.148926f, 404.461121f, 396.885284f);
                        AddWaypoint(10, -5139.549805f, 386.402893f, 396.906952f);
                        AddWaypoint(11, -5140.363281f, 375.913422f, 397.931152f);
                        AddWaypoint(12, -5142.502441f, 346.961121f, 398.053497f);
                        AddWaypoint(13, -5141.139160f, 359.352966f, 397.011078f);
                        AddWaypoint(14, -5140.697754f, 378.842957f, 397.872406f);
                        AddWaypoint(15, -5138.448242f, 395.032410f, 396.352783f);
                        AddWaypoint(16, -5133.321289f, 431.758118f, 396.322357f);
                        AddWaypoint(17, -5161.619141f, 452.354553f, 394.115448f);
                        AddWaypoint(18, -5185.990723f, 478.249481f, 387.941895f);
                        AddWaypoint(19, -5201.514160f, 491.001923f, 387.610474f);
                        AddWaypoint(20, -5227.306641f, 500.188507f, 387.142792f);
                        AddWaypoint(21, -5237.200195f, 513.050537f, 387.736145f);
                        AddWaypoint(22, -5251.963379f, 525.391174f, 386.584442f);
                        AddWaypoint(23, -5266.466797f, 537.124451f, 387.494537f);
                        AddWaypoint(24, -5283.755859f, 560.300293f, 385.199860f);
                        break;
                    case 4:
                        AddWaypoint(0, -5263.007813f, 572.262085f, 388.673157f);
                        AddWaypoint(1, -5236.423340f, 542.552368f, 389.753387f);
                        AddWaypoint(2, -5214.283691f, 541.975586f, 389.412659f);
                        AddWaypoint(3, -5197.762207f, 531.495056f, 388.887299f);
                        AddWaypoint(4, -5179.492188f, 519.916443f, 388.667297f);
                        AddWaypoint(5, -5175.582520f, 502.293945f, 388.701996f);
                        AddWaypoint(6, -5167.252441f, 493.800568f, 390.061920f);
                        AddWaypoint(7, -5157.853027f, 480.655945f, 390.738495f);
                        AddWaypoint(8, -5145.563965f, 479.578796f, 393.390839f);
                        AddWaypoint(9, -5131.671875f, 465.390198f, 394.823059f);
                        AddWaypoint(10, -5113.997559f, 476.204285f, 397.972015f);
                        AddWaypoint(11, -5118.809570f, 470.737091f, 397.644592f);
                        AddWaypoint(12, -5136.430664f, 457.685455f, 393.588379f);
                        AddWaypoint(13, -5149.951172f, 475.280426f, 391.100586f);
                        AddWaypoint(14, -5170.628418f, 490.365723f, 388.964539f);
                        AddWaypoint(15, -5177.971191f, 510.636871f, 388.935669f);
                        AddWaypoint(16, -5179.023438f, 522.535889f, 388.936523f);
                        AddWaypoint(17, -5200.432129f, 530.929260f, 388.382721f);
                        AddWaypoint(18, -5214.941895f, 535.946106f, 388.297760f);
                        AddWaypoint(19, -5234.781250f, 535.892456f, 388.333527f);
                        AddWaypoint(20, -5250.526367f, 551.972473f, 388.484741f);
                        AddWaypoint(21, -5262.729492f, 572.853027f, 388.782532f);
                        break;
                    default:
                        sLog->OutErrorConsole("Unexpected movement variation (%i) in npc_og_tankAI::SetupMovement call!", variation);
                        return;
                }
                if (npc_og_tankAI* EscortAI = CAST_AI(npc_og_tankAI, me->AI()))
                {
                    if (variation > 2)
                        EscortAI->Start(true, true);
                    else
                    {
                        EscortAI->Start(true, true);
                        EscortAI->SetDespawnAtEnd(false);
                    }
                    EscortAI->SetDespawnAtFar(false);
                    me->SetActive(true);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                npc_escortAI::UpdateAI(diff);

                if (!UpdateVictim())
                    return;

                if (CooldownTimer <= diff)
                {
                    DoCastVictim(!urand(0, 3) ? SPELL_MACHINE_GUN : SPELL_FLAME_SPRAY);
                    CooldownTimer = urand(5000, 7000);
                }
                else CooldownTimer -= diff;

                if (!me->HasUnitState(UNIT_STATE_CASTING))
                    DoMeleeAttackIfReady();
            }
        };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_og_tankAI(creature);
    }
};

class npc_og_i_tank : public CreatureScript
{
    public:
        npc_og_i_tank() : CreatureScript("npc_og_i_tank") {}

        struct npc_og_i_tankAI : public npc_escortAI
        {
            npc_og_i_tankAI(Creature* creature) : npc_escortAI(creature) {}

            void Reset(){}

            void WaypointReached(uint32 /*id*/){}

            void SetupMovement(uint32 variation)
            {
                switch (variation)
                {
                    case 0:
                        AddWaypoint(0, -5339.479980f, 545.804688f, 384.888336f);
                        AddWaypoint(1, -5348.981934f, 555.181458f, 385.137665f);
                        AddWaypoint(2, -5348.816406f, 567.433472f, 384.981537f);
                        AddWaypoint(3, -5348.981934f, 555.181458f, 385.137665f);
                        break;
                    case 1:
                        AddWaypoint(0, -5318.129395f, 578.573425f, 387.439697f);
                        AddWaypoint(1, -5305.049316f, 584.957153f, 389.928864f);
                        AddWaypoint(2, -5291.333496f, 589.698730f, 387.835785f);
                        AddWaypoint(3, -5283.275391f, 583.998413f, 386.930725f);
                        AddWaypoint(4, -5279.059082f, 571.535461f, 386.423187f);
                        AddWaypoint(5, -5283.275391f, 583.998413f, 386.930725f);
                        AddWaypoint(6, -5291.333496f, 589.698730f, 387.835785f);
                        AddWaypoint(7, -5305.049316f, 584.957153f, 389.928864f);
                        break;
                    case 3:
                        AddWaypoint(0, -5085.889648f, 475.435455f, 402.240814f);
                        AddWaypoint(1, -5092.540527f, 468.895569f, 403.767853f);
                        AddWaypoint(2, -5100.161133f, 463.751984f, 403.568817f);
                        AddWaypoint(3, -5110.015625f, 455.567688f, 401.042847f);
                        AddWaypoint(4, -5116.595215f, 453.564240f, 398.949249f);
                        AddWaypoint(5, -5127.697754f, 447.250763f, 395.562195f);
                        AddWaypoint(6, -5116.622559f, 453.300476f, 398.915619f);
                        AddWaypoint(7, -5111.297363f, 456.783386f, 400.862091f);
                        AddWaypoint(8, -5100.011719f, 463.002197f, 403.610992f);
                        AddWaypoint(9, -5093.187500f, 469.667328f, 403.619598f);
                        AddWaypoint(10, -5088.007324f, 474.113739f, 402.551941f);
                        break;
                    default:
                        sLog->OutErrorConsole("Unexpected movement variation (%i) in npc_og_i_tankAI::SetupMovement call!", variation);
                        return;
                }
                if (npc_og_i_tankAI* EscortAI = CAST_AI(npc_og_i_tankAI, me->AI()))
                {
                    EscortAI->Start(true, true);
                    EscortAI->SetDespawnAtFar(false);
                    me->SetActive(true);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                npc_escortAI::UpdateAI(diff);

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_og_i_tankAI(creature);
    }
};

class npc_og_assistants : public CreatureScript
{
    public:
        npc_og_assistants() : CreatureScript("npc_og_assistants") {}

        struct npc_og_assistantsAI : public npc_escortAI
        {
            npc_og_assistantsAI(Creature* creature) : npc_escortAI(creature) {}

            void Reset(){}

            void WaypointReached(uint32 id)
            {
                switch (id)
                {
                    case 0:
                    case 2:
                    case 20:
                    case 35:
                    case 52:
                    case 57:
                    case 65:
                    case 66:
                    case 67:
                    case 70:
                        SetHoldState(true);
                        break;
                    case 49:
                        SetHoldState(true);
                        me->RemoveMount();
                        if (Creature* mekkatorque = me->FindCreatureWithDistance(NPC_MEKKATORQUE, 100.0f, true))
                            me->CastSpell(mekkatorque, SPELL_TRIGGER, true);
                        break;
                    case 50:
                        me->CastSpell(me, SPELL_PARACHUTE_AURA, true);
                        break;
                    case 51:
                        me->RemoveAurasDueToSpell(SPELL_PARACHUTE_AURA);
                        me->Mount(me->GetEntry() == NPC_FASTBLAST ? DATA_MOUNT_FAST : DATA_MOUNT_COG);
                        break;
                    case 11:
                    case 17:
                        SetHoldState(true);
                        if (Creature* mekkatorque = me->FindCreatureWithDistance(NPC_MEKKATORQUE, 100.0f, true))
                            me->CastSpell(mekkatorque, SPELL_TRIGGER, true);
                        break;
                }
            }

            void SetHoldState(bool b_OnHold)
            {
                SetEscortPaused(b_OnHold);
            }


            void UpdateAI(uint32 const diff)
            {
                npc_escortAI::UpdateAI(diff);

                if (!UpdateVictim())
                    return;
            }
        };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_og_assistantsAI(creature);
    }
};

class npc_og_mekkatorque : public CreatureScript
{
    public:
        npc_og_mekkatorque() : CreatureScript("npc_og_mekkatorque")
        {
            bProcessing = false;
        }

        struct npc_og_mekkatorqueAI : public npc_escortAI
        {
            npc_og_mekkatorqueAI(Creature* creature) : npc_escortAI(creature) { }

            uint32                 uiStep;
            uint32                 StepTimer;
            uint32                 uiRLDestroyed;
            uint32                 uiCannonsDestroyed;
            uint32                 uiTroggs;
            uint32                 uiSoldiers;
            bool                   bCanSummonBomber;
            bool                   bCannonIntro;
            bool                   bBuffs;
            bool                   bControlWP_1;
            bool                   bControlWP_2;
            Creature*              RL[4];
            Creature*              Tank[3];
            Creature*              Cannon[6];
            Creature*              BattleSuit[3];
            Creature*              ExplosionBunny;
            std::list<GameObject*> BannerList_temp;
            std::list<GameObject*> BannerList;
            std::list<uint64>      SummonsGUID;
            Player*                pEscortPlayer;
            Quest const*           pEscortQuest;

            void Reset()
            {
                uiStep               = 0;
                uiTroggs             = 0;
                uiSoldiers           = 0;
                StepTimer         = 0;
                uiRLDestroyed        = 0;
                uiCannonsDestroyed   = 0;
                bCanSummonBomber     = true;
                bCannonIntro         = false;
                bBuffs               = false;
                bControlWP_1         = false;
                bControlWP_2         = false;
            }

            void WaypointReached(uint32 i)
            {
                switch (i)
                {
                    case 0:
                        DoPlayMusic(0);
                        SetHoldState(true);
                        break;
                    case 2:
                        SetHoldState(true);
                        DoTalk(me, MEK_5_1, SOUND_MEK_5, false);
                        JumpToNextStep(3500);
                        break;
                    case 3:
                        SquadSetRun(true);
                        DoUpdateWorldState(WORLDSTATE_RL_DESTROYED, uiRLDestroyed);
                        DoUpdateWorldState(WORLDSTATE_RL_DESTROYED_CTRL, 1);
                        break;
                    case 5:
                        DoTalk(me, MEK_6_1, SOUND_MEK_6, false);
                        if (RL[0]->IsAlive())
                            AttackStart(RL[0]);
                        break;
                    case 7:
                        if (RL[1]->IsAlive())
                            AttackStart(RL[1]);
                        break;
                    case 8:
                        if (RL[2]->IsAlive())
                            AttackStart(RL[2]);
                        break;
                    case 10:
                        if (RL[3]->IsAlive())
                            AttackStart(RL[3]);
                        break;
                    case 12:
                        SetHoldState(true);
                        DoUpdateWorldState(WORLDSTATE_RL_DESTROYED_CTRL, 0);
                        DoUpdateWorldState(WORLDSTATE_AIRFIELD_ATTACKED, 0);
                        DoUpdateWorldState(WORLDSTATE_AIRFIELD_CAPTURED, 1);
                        DoTalk(me, MEK_10_1, SOUND_MEK_10, false);
                        for (int8 n = 0; n < 5; ++n)
                            me->SummonCreature(NPC_INFANTRY, InfantrySpawn[n], TEMPSUMMON_MANUAL_DESPAWN);
                        UpdateBannerState(70.0f);
                        JumpToNextStep(5500);
                        break;
                    case 18:
                        SetHoldState(true);
                        JumpToNextStep(0);
                        break;
                    case 19:
                        SetHoldState(true);
                        me->RemoveMount();
                        JumpToNextStep(2000);
                        break;
                    case 21:
                        SetHoldState(true);
                        me->Mount(DATA_MOUNT_MEK);
                        for (int8 n = 18; n < 26; ++n)
                            me->SummonCreature(NPC_I_INFANTRY, iInfantrySpawn[n], TEMPSUMMON_MANUAL_DESPAWN);
                        JumpToNextStep(7000);
                        break;
                    case 27:
                        DoUpdateWorldState(WORLDSTATE_BATTLE_NEAR_ENTRANCE, 1);
                        for (int8 n = 5; n < 11; ++n)
                            me->SummonCreature(NPC_INFANTRY, InfantrySpawn[n], TEMPSUMMON_MANUAL_DESPAWN);
                        UpdateBannerState(70.0f);
                        break;
                    case 34:
                        if (Creature* suit = me->SummonCreature(NPC_BATTLE_SUIT, BattleSuitSpawn[5], TEMPSUMMON_MANUAL_DESPAWN))
                            CAST_AI(npc_og_suit::npc_og_suitAI, suit->AI())->SetupMovement(5);
                        UpdateBannerState(10.0f);
                        break;
                    case 36:
                        SetHoldState(true);
                        DoTalk(me, MEK_13_1, SOUND_MEK_13, false);
                        JumpToNextStep(6500);
                        break;
                    case 37:
                        UpdateBannerState(10.0f);
                        break;
                    case 47:
                        UpdateBannerState(80.0f);
                        break;
                    case 50:
                        SetHoldState(true);
                        me->RemoveMount();
                        JumpToNextStep(1000);
                        break;
                    case 51:
                        me->CastSpell(me, SPELL_PARACHUTE_AURA, true);
                        break;
                    case 52:
                        me->RemoveAurasDueToSpell(SPELL_PARACHUTE_AURA);
                        me->Mount(DATA_MOUNT_MEK);
                        break;
                    case 53:
                        SetHoldState(true);
                        UpdateBannerState(30.0f);
                        JumpToNextStep(5000);
                        break;
                    case 58:
                        SetHoldState(true);
                        DoTalk(me, MEK_16_1, SOUND_MEK_16, false);
                        DoUpdateWorldState(WORLDSTATE_BATLLE_IN_TUNNELS, 1);
                        JumpToNextStep(3000);
                        break;
                    case 67:
                        SetHoldState(true);
                        JumpToNextStep(3000);
                        break;
                    case 68:
                        SetHoldState(true);
                        JumpToNextStep(3500);
                        break;
                    case 71:
                        me->SetReactState(REACT_AGGRESSIVE);
                        if (Creature* cogspin = me->FindCreatureWithDistance(NPC_COGSPIN, 100.0f))
                            cogspin->SetReactState(REACT_AGGRESSIVE);
                        if (Creature* fastblast = me->FindCreatureWithDistance(NPC_FASTBLAST, 100.0f))
                            fastblast->SetReactState(REACT_AGGRESSIVE);
                        SetHoldState(true);
                        break;
                    case 75:
                        SetHoldState(true);
                        me->SummonCreature(NPC_BRAG_BOT, BragBotSpawn[1], TEMPSUMMON_MANUAL_DESPAWN);
                        JumpToNextStep(2000);
                        break;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                //DoRefreshWorldStates();

                npc_escortAI::UpdateAI(diff);

                if (bBuffs)
                {
                    if (!me->HasAura(SPELL_BRILLIANT_TACTICS))
                        me->AddAura(SPELL_BRILLIANT_TACTICS, me);
                    if (!me->HasAura(SPELL_HEALTH_REGEN))
                        me->AddAura(SPELL_HEALTH_REGEN, me);
                }

                if (StepTimer <= diff)
                {
                    switch (uiStep)
                    {
                        case 1:
                            me->MonsterSay(MEK_1_2, LANG_UNIVERSAL, 0);
                            JumpToNextStep(9000);
                            break;
                        case 2:
                            me->MonsterSay(MEK_1_3, LANG_UNIVERSAL, 0);
                            JumpToNextStep(5000);
                            break;
                        case 3:
                            DoTalk(me, MEK_2_1, SOUND_MEK_2, false);
                            JumpToNextStep(13000);
                            break;
                        case 4:
                            DoTalk(me, MEK_3_1, SOUND_MEK_3, false);
                            JumpToNextStep(4000);
                            break;
                        case 5:
                            me->MonsterYell(MEK_3_2, LANG_UNIVERSAL, 0);
                            DoUpdateWorldState(WORLDSTATE_COUNTDOWN_CTRL, 1);
                            DoUpdateWorldState(WORLDSTATE_COUNTDOWN, 5);
                            JumpToNextStep(1*MINUTE*IN_MILLISECONDS);
                            break;
                        case 6:
                            DoUpdateWorldState(WORLDSTATE_COUNTDOWN, 4);
                            JumpToNextStep(1*MINUTE*IN_MILLISECONDS);
                            break;
                        case 7:
                            DoUpdateWorldState(WORLDSTATE_COUNTDOWN, 3);
                            JumpToNextStep(1*MINUTE*IN_MILLISECONDS);
                            break;
                        case 8:
                            DoUpdateWorldState(WORLDSTATE_COUNTDOWN, 2);
                            JumpToNextStep(1*MINUTE*IN_MILLISECONDS);
                            break;
                        case 9:
                            DoUpdateWorldState(WORLDSTATE_COUNTDOWN, 1);
                            JumpToNextStep(1*MINUTE*IN_MILLISECONDS);
                            break;
                        case 10:
                            DoUpdateWorldState(WORLDSTATE_COUNTDOWN_CTRL, 0);
                            DoUpdateWorldState(WORLDSTATE_IN_PROCCESS, 1);
                            if (npc_og_mekkatorqueAI* EscortAI = CAST_AI(npc_og_mekkatorqueAI, me->AI()))
                            {
                                EscortAI->Start(true, true, pEscortPlayer->GetGUID(), pEscortQuest);
                                EscortAI->SetDespawnAtFar(false);
                                EscortAI->SetDespawnAtEnd(false);
                                me->SetActive(true);
                            }
                            if (Creature* cogspin = me->FindCreatureWithDistance(NPC_COGSPIN, 20))
                            {
                                CAST_AI(npc_og_assistants::npc_og_assistantsAI, cogspin->AI())->Start(true, true, pEscortPlayer->GetGUID(), pEscortQuest);
                                CAST_AI(npc_og_assistants::npc_og_assistantsAI, cogspin->AI())->SetDespawnAtFar(false);
                                CAST_AI(npc_og_assistants::npc_og_assistantsAI, cogspin->AI())->SetDespawnAtEnd(false);
                                cogspin->SetActive(true);
                            }
                            if (Creature* fastblast = me->FindCreatureWithDistance(NPC_FASTBLAST, 20))
                            {
                                CAST_AI(npc_og_assistants::npc_og_assistantsAI, fastblast->AI())->Start(true, true, pEscortPlayer->GetGUID(), pEscortQuest);
                                CAST_AI(npc_og_assistants::npc_og_assistantsAI, fastblast->AI())->SetDespawnAtFar(false);
                                CAST_AI(npc_og_assistants::npc_og_assistantsAI, fastblast->AI())->SetDespawnAtEnd(false);
                                fastblast->SetActive(true);
                            }
                            DoTalk(me, MEK_4_1, SOUND_MEK_4, 0);
                            JumpToNextStep(7700);
                            break;
                        case 11:
                            me->MonsterYell(MEK_4_2, LANG_UNIVERSAL, 0);
                            DoPlayMusic(1);
                            me->CastSpell(me, SPELL_HEALTH_REGEN, true);
                            me->CastSpell(me, SPELL_BRILLIANT_TACTICS, true);
                            bBuffs = true;
                            JumpToNextStep(2000);
                            break;
                        case 12:
                            SetHoldState(false);
                            break;
                        case 14:
                            me->MonsterSay(MEK_5_2, LANG_UNIVERSAL, 0);
                            DoUpdateWorldState(WORLDSTATE_AIRFIELD_ATTACKED, 1);
                            for (int8 n = 0; n < 3; ++n)
                                CAST_AI(npc_og_tank::npc_og_tankAI, Tank[n]->AI())->SetupMovement(n);
                            JumpToNextStep(3300);
                            break;
                        case 15:
                            me->MonsterSay(MEK_5_3, LANG_UNIVERSAL, 0);
                            JumpToNextStep(7000);
                            break;
                        case 16:
                            SetHoldState(false);
                            break;
                        case 18:
                            me->MonsterSay(MEK_10_2, LANG_UNIVERSAL, 0);
                            DoUpdateWorldState(WORLDSTATE_BATTLE_NEAR_WORKSHOPS, 1);
                            DoUpdateWorldState(WORLDSTATE_CANNONS_DESTROYED_CTRL, 1);
                            DoUpdateWorldState(WORLDSTATE_CANNONS_DESTROYED, 0);
                            for (int8 n = 0; n < 6; ++n)
                            {
                                Cannon[n] = me->SummonCreature(NPC_CANNON, CannonSpawn[n], TEMPSUMMON_MANUAL_DESPAWN);
                                Cannon[n]->CastSpell(Cannon[n], SPELL_TRIGGER, true);
                            }

							me->SummonGameObject(GO_RAD_CONTROL, -5072.80f, 441.48f, 410.97f, 2.6f, 0, 0, 0, 0, 300000);

                            for (int8 n = 0; n < 3; ++n)
                                BattleSuit[n] = me->SummonCreature(NPC_BATTLE_SUIT, BattleSuitSpawn[n], TEMPSUMMON_MANUAL_DESPAWN);
                            if (Creature* iTank = me->SummonCreature(NPC_I_TANK, iTankSpawn[3], TEMPSUMMON_MANUAL_DESPAWN))
                                CAST_AI(npc_og_i_tank::npc_og_i_tankAI, iTank->AI())->SetupMovement(3);
                            for (int8 n = 8; n < 18; ++n)
                                me->SummonCreature(NPC_I_INFANTRY, iInfantrySpawn[n], TEMPSUMMON_MANUAL_DESPAWN);
                            JumpToNextStep(7250);
                            break;
                        case 19:
                            me->MonsterSay(MEK_10_3, LANG_UNIVERSAL, 0);
                            if (Creature* tank1 = me->SummonCreature(NPC_TANK, TankSpawn[3], TEMPSUMMON_MANUAL_DESPAWN))
                                if (Creature* tank2 = me->SummonCreature(NPC_TANK, TankSpawn[4], TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    CAST_AI(npc_og_tank::npc_og_tankAI, tank1->AI())->SetupMovement(3);
                                    CAST_AI(npc_og_tank::npc_og_tankAI, tank2->AI())->SetupMovement(4);
                                }
                            DoSummonBomber();
                            JumpToNextStep(3000);
                            break;
                        case 20:
                            if(!ValidateEscortState())
                                StepTimer = 2000;
                            else
                                SetHoldState(false);
                            break;
                        case 22:
                            if (!ValidateEscortState() || me->FindCreatureWithDistance(NPC_CANNON, 100.0f, true))
                            {
                                StepTimer = 2000;
                                return;
                            }
                            DoTalk(me, MEK_12_1, SOUND_MEK_12, false);
                            if (Creature* pDriver1 = me->SummonCreature(NPC_INFANTRY, BattleSuitDriverSpawn[0], TEMPSUMMON_MANUAL_DESPAWN))
                                if (Creature* pDriver2 = me->SummonCreature(NPC_INFANTRY, BattleSuitDriverSpawn[1], TEMPSUMMON_MANUAL_DESPAWN))
                                    if (Creature* pDriver3 = me->SummonCreature(NPC_INFANTRY, BattleSuitDriverSpawn[2], TEMPSUMMON_MANUAL_DESPAWN))
                                    {
                                        CAST_AI(npc_og_infantry::npc_og_infantryAI, pDriver1->AI())->SetupMovement(0);
                                        CAST_AI(npc_og_infantry::npc_og_infantryAI, pDriver2->AI())->SetupMovement(1);
                                        CAST_AI(npc_og_infantry::npc_og_infantryAI, pDriver3->AI())->SetupMovement(2);
                                    }
                            JumpToNextStep(8500);
                            break;
                        case 23:
                            me->MonsterSay(MEK_12_2, LANG_UNIVERSAL, 0);
                            JumpToNextStep(8500);
                            break;
                        case 24:
                            SetHoldState(false);
                            break;
                        case 26:
                            SetEscortPaused(false);
                            JumpToNextStep(0);
                            break;
                        case 28:
                            SetHoldState(false);
                            break;
                        case 30:
                            me->MonsterSay(MEK_13_2, LANG_UNIVERSAL, 0);
                            JumpToNextStep(8000);
                            break;
                        case 31:
                            me->MonsterSay(MEK_13_3, LANG_UNIVERSAL, 0);
                            for (int32 n = 0; n < 3; ++n)
                                BattleSuit[n]->DisappearAndDie();
                            me->SummonCreature(NPC_BRAG_BOT, BragBotSpawn[0], TEMPSUMMON_MANUAL_DESPAWN);
                            JumpToNextStep(5000);
                            break;
                        case 32:
                            if (Creature* bragbot = me->FindCreatureWithDistance(NPC_BRAG_BOT, 20, true))
                                DoTalk(bragbot, THERM_1_1, SOUND_THERM_1, true);
                            JumpToNextStep(7000);
                            break;
                        case 33:
                            if (Creature* bragbot = me->FindCreatureWithDistance(NPC_BRAG_BOT, 20, true))
                                bragbot->MonsterYell(THERM_1_2, LANG_UNIVERSAL, 0);
                            JumpToNextStep(5000);
                            break;
                        case 34:
                            if (Creature* bragbot = me->FindCreatureWithDistance(NPC_BRAG_BOT, 20, true))
                                bragbot->MonsterYell(THERM_1_3, LANG_UNIVERSAL, 0);
                            JumpToNextStep(3000);
                            break;
                        case 35:
                            if (Creature* bragbot = me->FindCreatureWithDistance(NPC_BRAG_BOT, 20, true))
                                bragbot->MonsterYell(THERM_1_4, LANG_UNIVERSAL, 0);
                            JumpToNextStep(4500);
                            break;
                        case 36:
                            PartyCast(SPELL_EXPLOSION);
                            if (Creature* bragbot = me->FindCreatureWithDistance(NPC_BRAG_BOT, 20, true))
                                bragbot->DisappearAndDie();
                            JumpToNextStep(1000);
                            break;
                        case 37:
                            if (uiTroggs <= 20)
                            {
                                if (Creature* pTrogg = me->SummonCreature(NPC_I_TROGG, TroggSpawn, TEMPSUMMON_MANUAL_DESPAWN))
                                    ++uiTroggs;

                                switch (uiTroggs)
                                {
                                    case 2:
                                        DoTalk(me, MEK_14_1, SOUND_MEK_14, false);
                                        break;
                                    case 5:
                                        me->MonsterYell(MEK_14_2, LANG_UNIVERSAL, 0);
                                        break;
                                }

                                StepTimer = uiTroggs % 4 == 0 ? 30000 : 1500;
                            }
                            else
                            {
                                if (Creature* pGasherikk = me->SummonCreature(NPC_GASHERIKK, TroggSpawn, TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    DoTalk(me, MEK_15_1, SOUND_MEK_15, true);
                                    ++uiStep;
                                }
                            }
                            break;
                        case 39:
                            SetHoldState(false);
                            break;
                        case 41:
                            if(!ValidateEscortState())
                                StepTimer = 2000;
                            else
                            {
                                PartyCast(SPELL_PARACHUTE);
                                SetHoldState(false);
                            }
                            break;
                        case 43:
                            SetHoldState(false);
                            break;
                        case 45:
                            if (Creature* suit = me->SummonCreature(NPC_BATTLE_SUIT, BattleSuitSpawn[3], TEMPSUMMON_MANUAL_DESPAWN))
                                CAST_AI(npc_og_suit::npc_og_suitAI, suit->AI())->SetupMovement(3);
                            if (Creature* suit = me->SummonCreature(NPC_BATTLE_SUIT, BattleSuitSpawn[4], TEMPSUMMON_MANUAL_DESPAWN))
                                CAST_AI(npc_og_suit::npc_og_suitAI, suit->AI())->SetupMovement(4);
                            for (int8 n = 15; n < 19; ++n)
                                if (Creature* pInfantry = me->SummonCreature(NPC_INFANTRY, InfantrySpawn[n], TEMPSUMMON_MANUAL_DESPAWN))
                                    CAST_AI(npc_og_infantry::npc_og_infantryAI, pInfantry->AI())->SetupMovement(n);
                            JumpToNextStep(1500);
                            break;
                        case 46:
                            SetHoldState(false);
                            break;
                        case 48:
                            DoTalk(me, MEK_17_1, SOUND_MEK_17, false);
                            for (int8 n = 11; n < 15; ++n)
                                me->SummonCreature(NPC_INFANTRY, InfantrySpawn[n], TEMPSUMMON_MANUAL_DESPAWN);
                            for (int8 n = 0; n < 5; ++n)
                                if (Creature* soldier = me->SummonCreature(urand(0, 1) ? NPC_I_INFANTRY : NPC_I_CAVALRY, iSoldierSpawn[n], TEMPSUMMON_MANUAL_DESPAWN))
                                    soldier->GetMotionMaster()->MovePoint(0, -4955.23f, 728.98f, 259.31f);
                            JumpToNextStep(5000);
                            break;
                        case 49:
                            me->SetReactState(REACT_PASSIVE);
                            if (Creature* cogspin = me->FindCreatureWithDistance(NPC_COGSPIN, 100.0f))
                                cogspin->SetReactState(REACT_PASSIVE);
                            if (Creature* fastblast = me->FindCreatureWithDistance(NPC_FASTBLAST, 100.0f))
                                fastblast->SetReactState(REACT_PASSIVE);
                            SetHoldState(false);
                            break;
                        case 51:
                            SetHoldState(false);
                            JumpToNextStep(5000);
                            break;
                        case 53:
                            if (uiSoldiers <= 30)
                            {
                                if (Creature* soldier = me->SummonCreature(urand(0, 1) ? NPC_I_INFANTRY : NPC_I_CAVALRY, iSoldierSpawn[urand(0,5)], TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    soldier->GetMotionMaster()->MovePoint(0, -4955.23f, 728.98f, 259.31f);
                                    ++uiSoldiers;
                                }
                                StepTimer = 4000;
                            }
                            else
                            {
                                me->SummonCreature(NPC_BOLTCOG, -5035.236816f, 708.675232f, 260.499268f, 0, TEMPSUMMON_MANUAL_DESPAWN);
                                JumpToNextStep(10000);
                            }
                            break;
                        case 54:
                            if (Creature* pBoltcog = me->FindCreatureWithDistance(NPC_BOLTCOG, 1000.0f, true))
                                DoTalk(pBoltcog, BOLTCOG_1, SOUND_BOLTCOG_1, true);
                            ++uiStep;
                            break;
                        case 56:
                            SetHoldState(false);
                            break;
                        case 58:
                            if (Creature* bragbot = me->FindCreatureWithDistance(NPC_BRAG_BOT, 20.0f, true))
                                DoTalk(bragbot, THERM_2_1, SOUND_THERM_2, true);
                            JumpToNextStep(6000);
                            break;
                        case 59:
                            if (Creature* bragbot = me->FindCreatureWithDistance(NPC_BRAG_BOT, 20.0f, true))
                                bragbot->MonsterYell(THERM_2_2, LANG_UNIVERSAL, 0);
                            JumpToNextStep(3000);
                            break;
                        case 60:
                            if (Creature* bragbot = me->FindCreatureWithDistance(NPC_BRAG_BOT, 20.0f, true))
                                bragbot->MonsterYell(THERM_2_3, LANG_UNIVERSAL, 0);
                            JumpToNextStep(5000);
                            break;
                        case 61:
                            DoTalk(me, MEK_18_1, SOUND_MEK_18, false);
                            JumpToNextStep(5000);
                            break;
                        case 62:
                            if (Creature* bragbot = me->FindCreatureWithDistance(NPC_BRAG_BOT, 20.0f, true))
                                DoTalk(bragbot, THERM_3_1, SOUND_THERM_3, true);
                            JumpToNextStep(5000);
                            break;
                        case 63:
                            if (Creature* bragbot = me->FindCreatureWithDistance(NPC_BRAG_BOT, 20.0f, true))
                                bragbot->MonsterYell(THERM_3_2, LANG_UNIVERSAL, 0);
                            JumpToNextStep(7000);
                            break;
                        case 64:
                            if (Creature* bragbot = me->FindCreatureWithDistance(NPC_BRAG_BOT, 20.0f, true))
                                bragbot->MonsterYell(THERM_3_3, LANG_UNIVERSAL, 0);
                            JumpToNextStep(2000);
                            break;
                        case 65:
                            if (Creature* bragbot = me->FindCreatureWithDistance(NPC_BRAG_BOT, 20.0f, true))
                                bragbot->MonsterYell(THERM_3_4, LANG_UNIVERSAL, 0);
                            JumpToNextStep(8000);
                            break;
                        case 66:
                            if (GameObject* go = me->FindGameobjectWithDistance(GO_IRRADIATOR, 20.0f))
                            {
                                go->SetGoState(GO_STATE_ACTIVE);
                                if (Creature* irraiator = me->SummonCreature(NPC_IRRADIATOR, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN))
                                    DoTalk(irraiator, IRRADIATOR_1_1, SOUND_IRRADIATOR_1, true);
                            }
                            JumpToNextStep(8000);
                            break;
                        case 67:
                            DoTalk(me, MEK_19_1, SOUND_MEK_19, false);
                            JumpToNextStep(5000);
                            break;
                        case 68:
                            me->MonsterSay(MEK_19_2, LANG_UNIVERSAL, 0);
                            me->RemoveMount();
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, 25140);
                            me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_WORK_SHEATHED);
                            JumpToNextStep(10000);
                            break;
                        case 69:
                            if (Creature* bragbot = me->FindCreatureWithDistance(NPC_BRAG_BOT, 20.0f, true))
                                DoTalk(bragbot, THERM_4_1, SOUND_THERM_4, true);
                            JumpToNextStep(10000);
                            break;
                        case 70:
                            if (Creature* irraiator = me->FindCreatureWithDistance(NPC_IRRADIATOR, 20.0f, true))
                                DoTalk(irraiator, IRRADIATOR_2_1, SOUND_IRRADIATOR_2, true);
                            if (Creature* bragbot = me->FindCreatureWithDistance(NPC_BRAG_BOT, 20.0f, true))
                                bragbot->DisappearAndDie();
                            JumpToNextStep(8000);
                            break;
                        case 71:
                            if (Creature* irraiator = me->FindCreatureWithDistance(NPC_IRRADIATOR, 20.0f, true))
                                irraiator->DisappearAndDie();
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, 53056);
                            me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_ONESHOT_NONE);
                            DoTalk(me, MEK_20_1, SOUND_MEK_20, false);
                            JumpToNextStep(5000);
                            break;
                        case 72:
                            if (Player* player = GetPlayerForEscort())
                            {
                                if (Group* group = player->GetGroup())
                                {
                                    for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
                                    {
                                        if (Player* member = itr->getSource())
                                        {
                                            Creature* cameraveh = me->SummonCreature(NPC_CAMERA_VEHICLE, -5164.767578f, 556.341125f, 423.753784f, 25.29f, TEMPSUMMON_MANUAL_DESPAWN);
                                            member->CastSpell(member, SPELL_SEE_INVISIBILITY, true);
                                            member->CastSpell(cameraveh, SPELL_BINDSIGHT, true);
                                        }
                                    }
                                }
                                else
                                {
                                    player->GetMap()->LoadGrid(ExplosionBunnySpawn.GetPositionX(), ExplosionBunnySpawn.GetPositionY());
                                    Creature* cameraveh = me->SummonCreature(NPC_CAMERA_VEHICLE, -5164.767578f, 556.341125f, 423.753784f, 25.29f, TEMPSUMMON_MANUAL_DESPAWN);
                                    player->CastSpell(player, SPELL_SEE_INVISIBILITY, true);
                                    player->CastSpell(cameraveh, SPELL_BINDSIGHT, true);
                                }
                            }
                            JumpToNextStep(1300);
                            break;
                        case 73:
                            ExplosionBunny = me->SummonCreature(NPC_EXPLOSION_BUNNY, ExplosionBunnySpawn, TEMPSUMMON_MANUAL_DESPAWN);
                            ExplosionBunny->SetActive(true);
                            ExplosionBunny->CastSpell(ExplosionBunny, SPELL_SPAWN_INVISIBILITY, true);
                            JumpToNextStep(1500);
                            break;
                        case 74:
                            ExplosionBunny->CastSpell(ExplosionBunny, SPELL_RAD_EXPLOSION, true);
                            JumpToNextStep(7000);
                            break;
                        case 75:
                            ExplosionBunny->DespawnAfterAction();
                            bBuffs = false;
                            me->RemoveAurasDueToSpell(SPELL_HEALTH_REGEN);
                            me->RemoveAurasDueToSpell(SPELL_BRILLIANT_TACTICS);
                            me->NearTeleportTo(-4827.0f, -1256.0f, 506.077f, 4.535f);
                            me->SetDisplayId(MODEL_ID_INVISIBLE);
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, 0);
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            DoPlayMusic(2);
                            DoCleanup();
                            JumpToNextStep(10000);
                            break;
                        case 76:
                            DoTalk(me, MEK_21_1, SOUND_MEK_21, false);
                            JumpToNextStep(10000);
                            break;
                        case 77:
                            me->MonsterSay(MEK_21_2, LANG_UNIVERSAL, 0);
                            JumpToNextStep(10000);
                            break;
                        case 78:
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, 53056);
                            me->DisappearAndDie();
                            break;
                    }
                }
                else 
                    StepTimer -= diff;

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*who*/)
            {
                DoCleanup();
            }

            void JustSummoned(Creature* summon)
            {
                SummonsGUID.push_back(summon->GetGUID());
            }

            void JumpToNextStep(uint32 timer)
            {
                StepTimer = timer;
                ++uiStep;
            }

            void SetHoldState(bool b_OnHold)
            {
                SetEscortPaused(b_OnHold);
                if (!b_OnHold)
                {
                    if (Creature* cogspin = me->FindCreatureWithDistance(NPC_COGSPIN, 100.0f))
                        CAST_AI(npc_og_assistants::npc_og_assistantsAI, cogspin->AI())->SetHoldState(b_OnHold);
                    if (Creature* fastblast = me->FindCreatureWithDistance(NPC_FASTBLAST, 100.0f))
                        CAST_AI(npc_og_assistants::npc_og_assistantsAI, fastblast->AI())->SetHoldState(b_OnHold);
                    ++uiStep;
                }
            }

            void EnterToBattle(Unit* who)
            {
                if (who && who->ToCreature())
                    SquadAssist(who->ToCreature());
            }

            void PartyCast(uint32 spell)
            {
                if (Player* player = GetPlayerForEscort())
                {
                    if (Group* group = player->GetGroup())
                    {
                        for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
                        {
                            if (Player* member = itr->getSource())
                                member->CastSpell(member, spell, true);
                        }
                    }
                    else
                        player->CastSpell(player, spell, true);
                }
            }

            void SpecialKill(uint32 variation)
            {
                switch (variation)
                {
                    case 0:
                        ++uiRLDestroyed;
                        DoUpdateWorldState(WORLDSTATE_RL_DESTROYED, uiRLDestroyed);
                        if (uiStep > 14)
                        {
                            switch (uiRLDestroyed)
                            {
                                case 1:
                                    DoTalk(me, MEK_7_1, SOUND_MEK_7, false);
                                    break;
                                case 2:
                                    DoTalk(me, MEK_8_1, SOUND_MEK_8, false);
                                    break;
                                case 3:
                                    DoTalk(me, MEK_9_1, SOUND_MEK_9, false);
                                    break;
                                case 4:
                                    break;
                                default:
                                    sLog->OutErrorConsole("More than 4 Rocket Launchers destroyed!");
                            }
                        }
                        break;
                    case 1:
                        ++uiCannonsDestroyed;
                        DoUpdateWorldState(WORLDSTATE_CANNONS_DESTROYED, uiCannonsDestroyed);
                        if (uiCannonsDestroyed == 6)
                        {
                            bCanSummonBomber = false;
                            DoUpdateWorldState(WORLDSTATE_AIRFIELD_CAPTURED, 0);
                            DoUpdateWorldState(WORLDSTATE_CANNONS_DESTROYED_CTRL, 0);
                            DoUpdateWorldState(WORLDSTATE_AIRFIELD_AND_COMMAND_CENTER_CAPTURED, 1);
                        }
                        if (uiCannonsDestroyed > 6)
                            sLog->OutErrorConsole("More than 6 Tankbuster Cannons destroyed!");
                        break;
                    case 2:
                        DoUpdateWorldState(WORLDSTATE_BATTLE_NEAR_ENTRANCE, 0);
                        DoUpdateWorldState(WORLDSTATE_AIRFIELD_AND_COMMAND_CENTER_CAPTURED, 0);
                        DoUpdateWorldState(WORLDSTATE_SURFACE_CAPTURED, 1);
                        JumpToNextStep(2000);
                        break;
                    case 3:
                        DoUpdateWorldState(WORLDSTATE_BATLLE_IN_TUNNELS, 0);
                        DoUpdateWorldState(WORLDSTATE_TUNNELS_CAPTURED, 1);
                        JumpToNextStep(100);
                        break;
                    default:
                        sLog->OutErrorConsole("Unexpected variation (%i) in npc_og_mekkatorqueAI::SpecialKill call!", variation);
                }
            }

            void DoTalk(Creature* talker, const char* text, uint32 sound, bool yell)
            {
                if (yell)
                    talker->MonsterYell(text, LANG_UNIVERSAL, 0);
                else
                    talker->MonsterSay(text, LANG_UNIVERSAL, 0);

                if (sound)
                    DoPlaySoundToSet(talker, sound);
            }

            void SquadAssist(Creature* target)
            {
                if (!target->IsAlive())
                    return;

                if (Creature* cogspin = me->FindCreatureWithDistance(NPC_COGSPIN, 100))
                    cogspin->AI()->AttackStart(target);

                if (Creature* fastblast = me->FindCreatureWithDistance(NPC_FASTBLAST, 100))
                    fastblast->AI()->AttackStart(target);
            }

            void SquadSetRun(bool b_Run)
            {
                if (Creature* cogspin = me->FindCreatureWithDistance(NPC_COGSPIN, 100))
                    CAST_AI(npc_og_assistants::npc_og_assistantsAI, cogspin->AI())->SetRun(b_Run);
                if (Creature* fastblast = me->FindCreatureWithDistance(NPC_FASTBLAST, 100))
                    CAST_AI(npc_og_assistants::npc_og_assistantsAI, fastblast->AI())->SetRun(b_Run);

                SetRun(b_Run);
            }

            void DoPlayMusic(uint8 musicId)
            {
                switch (musicId)
                {
                    case 0:
                        PartyCast(SPELL_MUSIC_START);
                        break;
                    case 1:
                        PartyCast(SPELL_MUSIC);
                        break;
                    case 2:
                        PartyCast(SPELL_MUSIC_END);
                        break;
                    default:
                        sLog->OutErrorConsole("Unexpected musicId (%i) in npc_og_mekkatorqueAI::DoPlayMusic call!", musicId);
                }
            }

            void DoSummonBomber()
            {
                if (bCanSummonBomber)
                {
                    Creature* bomber = me->SummonCreature(NPC_BOMBER, BomberSpawn, TEMPSUMMON_CORPSE_DESPAWN);
                    bomber->SetActive(true);
                    bomber->SetSpeed(MOVE_FLIGHT, 5.0, true);
                    bomber->GetMotionMaster()->MovePoint(1, -5034.42f, 369.79f, 438.06f);
                }
            }

            void SpellHit(Unit* target, const SpellEntry* spell)
            {
                if (spell->Id == SPELL_TRIGGER)
                {
                    switch (target->GetEntry())
                    {
                        case NPC_FASTBLAST:
                            bControlWP_1 = true;
                            break;
                        case NPC_COGSPIN:
                            bControlWP_2 = true;
                            break;
                    }
                }
            }

            void DoUpdateWorldState(uint32 worldstate, uint32 value)
            {
                Map::PlayerList const &PlList = me->GetMap()->GetPlayers();

                if (PlList.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
                {
                    if (Player* player = i->getSource())
                    {
                        if (player->GetQuestStatus(QUEST_OPERATION_GNOMEREGAN) == QUEST_STATUS_INCOMPLETE)
                            player->SendUpdateWorldState(worldstate, value);
                    }
                }
            }

            /*void DoRefreshWorldStates()
            {
                Map::PlayerList const &PlList = me->GetMap()->GetPlayers();

                if (PlList.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
                {
                    if (Player* player = i->getSource())
                    {
                        if (player->GetQuestStatus(QUEST_OPERATION_GNOMEREGAN) == QUEST_STATUS_NONE || player->GetQuestStatus(QUEST_OPERATION_GNOMEREGAN) == QUEST_STATUS_REWARDED|| player->GetQuestStatus(QUEST_OPERATION_GNOMEREGAN) == QUEST_STATUS_FAILED)
                            for (int8 n = 0; n < 15; ++n)
                                player->SendUpdateWorldState(WorldStates[n], 0);
                    }
                }
            }*/

            bool ValidateEscortState()
            {
                if (!bControlWP_1 || !bControlWP_2)
                    return false;
                else
                {
                    bControlWP_1 = false;
                    bControlWP_2 = false;
                    return true;
                }
            }

            void UpdateBannerState(float radius)
            {
                me->GetGameObjectListWithEntryInGrid(BannerList_temp, GO_BANNER, radius);
                if (!BannerList_temp.empty())
                {
                    for (std::list<GameObject*>::const_iterator itr = BannerList_temp.begin(); itr != BannerList_temp.end(); ++itr)
                    {
                        if ((*itr)->GetRespawnTime() == 0)
                        {
                            (*itr)->SetRespawnTime(1*DAY);
                            BannerList.push_back(*itr);
                        }
                    }
                    BannerList_temp.clear();
                }
                else
                    sLog->OutErrorConsole("TSCR error: BannerList is empty!");
            }

            void DoCleanup()
            {
                if (!SummonsGUID.empty())
                {
                    for (std::list<uint64>::const_iterator itr = SummonsGUID.begin(); itr != SummonsGUID.end(); ++itr)
                        if (Creature* summon = ObjectAccessor::GetCreature(*me, *itr))
                            if (summon->IsAlive())
                                summon->DisappearAndDie();
                            else
                                summon->DespawnAfterAction();
                    SummonsGUID.clear();
                }
                if (!BannerList.empty())
                {
                    for (std::list<GameObject*>::const_iterator itr = BannerList.begin(); itr != BannerList.end(); ++itr)
                            (*itr)->SetRespawnTime(0);
                    BannerList.clear();
                }
                if (Creature* cogspin = me->FindCreatureWithDistance(NPC_COGSPIN, 100.0f))
                    cogspin->DisappearAndDie();
                if (Creature* fastblast = me->FindCreatureWithDistance(NPC_FASTBLAST, 100.0f))
                    fastblast->DisappearAndDie();
                //DoRefreshWorldStates();
            }
        };

typedef npc_og_mekkatorque::npc_og_mekkatorqueAI MekkAI;

        bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
        {
            if (quest->GetQuestId() == QUEST_OPERATION_GNOMEREGAN)
            {
                if (!bProcessing)
                {
                    CAST_AI(MekkAI, creature->AI())->pEscortQuest = quest;
                    CAST_AI(MekkAI, creature->AI())->pEscortPlayer = player;
                    creature->MonsterSay(MEK_1_1, LANG_UNIVERSAL, 0);
                    CAST_AI(MekkAI, creature->AI())->DoPlaySoundToSet(creature, SOUND_MEK_1);
                    for (int8 n = 0; n < 4; ++n)
                        CAST_AI(MekkAI, creature->AI())->RL[n] = creature->SummonCreature(NPC_RL, RLSpawn[n], TEMPSUMMON_MANUAL_DESPAWN);
                    for (int8 n = 0; n < 3; ++n)
                        CAST_AI(MekkAI, creature->AI())->Tank[n] = creature->SummonCreature(NPC_TANK, TankSpawn[n], TEMPSUMMON_MANUAL_DESPAWN);
                    for (int8 n = 0; n < 8; ++n)
                        creature->SummonCreature(NPC_I_INFANTRY, iInfantrySpawn[n], TEMPSUMMON_MANUAL_DESPAWN);
                    creature->SummonCreature(NPC_I_TANK, iTankSpawn[0], TEMPSUMMON_MANUAL_DESPAWN);
                    for (int8 n = 1; n < 3; ++n)
                        if (Creature* p_iTank = creature->SummonCreature(NPC_I_TANK, iTankSpawn[n], TEMPSUMMON_MANUAL_DESPAWN))
                            CAST_AI(npc_og_i_tank::npc_og_i_tankAI, p_iTank->AI())->SetupMovement(n-1);
                    CAST_AI(MekkAI, creature->AI())->JumpToNextStep(4000);
                    bProcessing = true;
                }
            }
            return true;
        }

        private:
            bool bProcessing;

    CreatureAI *GetAI(Creature* creature) const
    {
        return new npc_og_mekkatorqueAI(creature);
    }
};

class npc_og_boltcog : public CreatureScript
{
    public:
        npc_og_boltcog() : CreatureScript("npc_og_boltcog") {}

        struct npc_og_boltcogAI : public npc_escortAI
        {
            npc_og_boltcogAI(Creature* creature) : npc_escortAI(creature) {}
            
            uint32 ThrowTimer;

            void Reset()
            {
                ThrowTimer = urand(10000, 25000);
            }

            void WaypointReached(uint32 i){}

            void JustDied(Unit* /*who*/)
            {
                if (Creature* mekkatorque = me->FindCreatureWithDistance(NPC_MEKKATORQUE, 100.0f, true))
                    CAST_AI(npc_og_mekkatorque::npc_og_mekkatorqueAI, mekkatorque->AI())->SpecialKill(3);
            }

            void IsSummonedBy(Unit* /*who*/)
            {
                AddWaypoint(0, -5035.236816f, 708.675232f, 260.499268f);
                AddWaypoint(1, -5033.954590f, 717.153992f, 260.528778f);
                AddWaypoint(2, -5048.367188f, 718.769409f, 260.534576f);
                AddWaypoint(3, -5048.580078f, 723.327087f, 260.738220f);
                AddWaypoint(4, -5048.201660f, 725.357117f, 261.238556f);
                AddWaypoint(5, -5044.113281f, 735.628906f, 256.475586f);
                AddWaypoint(6, -4974.464355f, 730.595642f, 256.258636f);
                AddWaypoint(7, -4950.033203f, 728.526733f, 260.143768f);
                AddWaypoint(8, -4952.264160f, 728.697937f, 259.785492f);

                if (npc_og_boltcogAI* EscortAI = CAST_AI(npc_og_boltcogAI, me->AI()))
                {
                    EscortAI->Start(true, true);
                    EscortAI->SetDespawnAtFar(false);
                    EscortAI->SetDespawnAtEnd(false);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                npc_escortAI::UpdateAI(diff);

                if (!UpdateVictim())
                    return;

                if (ThrowTimer <= diff)
                {
                    DoCastVictim(SPELL_WRENCH_THROW);
                    ThrowTimer = urand(10000, 25000);
                }
                else
                    ThrowTimer -= diff;
            }
        };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_og_boltcogAI(creature);
    }
};

class npc_og_rl : public CreatureScript
{
public:
	npc_og_rl() : CreatureScript("npc_og_rl") { }

	struct npc_og_rlAI : public QuantumBasicAI
	{
		npc_og_rlAI(Creature* creature) : QuantumBasicAI(creature) {}

		void JustDied(Unit* /*who*/)
		{
			if (Creature* mekkatorque = me->FindCreatureWithDistance(NPC_MEKKATORQUE, 1000, true))
				CAST_AI(npc_og_mekkatorque::npc_og_mekkatorqueAI, mekkatorque->AI())->SpecialKill(0);
		}
	};

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_og_rlAI(creature);
    }
};

class npc_og_cannon : public CreatureScript
{
    public:
        npc_og_cannon() : CreatureScript("npc_og_cannon") { }

        struct npc_og_cannonAI : public QuantumBasicAI
        {
            npc_og_cannonAI(Creature* creature) : QuantumBasicAI(creature) {}

            uint32 uiHits;
            uint32 RocketTimer;

            void Reset()
            {
                if (uiHits < 5 && !me->HasAura(SPELL_CANNON_SHIELD))
                    me->CastSpell(me, SPELL_CANNON_SHIELD, true);

                RocketTimer = urand(1000, 5000);
            }

            void SpellHit(Unit* target, const SpellInfo* spell)
            {
                if (spell->Id == SPELL_ROCKET)
                {
                    ++uiHits;
                    if (uiHits == 5)
                    {
                        if (Creature* mekkatorque = me->FindCreatureWithDistance(NPC_MEKKATORQUE, 1000, true))
                            if (!CAST_AI(npc_og_mekkatorque::npc_og_mekkatorqueAI, mekkatorque->AI())->bCannonIntro)
                            {
                                CAST_AI(npc_og_mekkatorque::npc_og_mekkatorqueAI, mekkatorque->AI())->DoTalk(mekkatorque, MEK_11_1, SOUND_MEK_11, false);
                                CAST_AI(npc_og_mekkatorque::npc_og_mekkatorqueAI, mekkatorque->AI())->bCannonIntro = true;
                            }
                        me->RemoveAurasDueToSpell(SPELL_CANNON_SHIELD);
                    }
                }
                else if (spell->Id == SPELL_TRIGGER)
                {
                    uiHits = 0;
                    me->CastSpell(me, SPELL_CANNON_SHIELD, true);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (RocketTimer <= diff)
                {
                    DoCastVictim(SPELL_CANNON_SHOT);
                    RocketTimer = urand(1000, 5000);
                }
                else
                    RocketTimer -= diff;
            }

            void JustDied(Unit * /*who*/)
            {
                if (Creature* mekkatorque = me->FindCreatureWithDistance(NPC_MEKKATORQUE, 1000, true))
                    CAST_AI(npc_og_mekkatorque::npc_og_mekkatorqueAI, mekkatorque->AI())->SpecialKill(1);
            }
        };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_og_cannonAI(creature);
    }
};

class npc_og_bomber : public CreatureScript
{
    public:
        npc_og_bomber() : CreatureScript("npc_og_bomber") { }

        struct npc_og_bomberlAI : public QuantumBasicAI
        {
            npc_og_bomberlAI(Creature* creature) : QuantumBasicAI(creature) {}

            bool bAction;

            void Reset()
            {
                bAction = true;
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type == POINT_MOTION_TYPE && id == 1)
                    if (Creature* mekkatorque = me->FindCreatureWithDistance(NPC_MEKKATORQUE, 300, true))
                        CAST_AI(npc_og_mekkatorque::npc_og_mekkatorqueAI, mekkatorque->AI())->DoSummonBomber();

                me->DisappearAndDie();
            }

            void MoveInLineOfSight(Unit* who)
            {
                if (who->GetTypeId() != TYPE_ID_UNIT)
                    return;

                if (who->GetEntry() != NPC_CANNON || !bAction || !who->HasAura(SPELL_CANNON_SHIELD))
                    return;

                SpellInfo const* sEntry = sSpellMgr->GetSpellInfo(SPELL_ROCKET);
                me->CastSpell(who, sEntry, true);
                CAST_AI(npc_og_cannon::npc_og_cannonAI, who->ToCreature()->AI())->SpellHit(me, sEntry);
                bAction = false;
            }
        };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_og_bomberlAI(creature);
    }
};

class npc_og_trogg : public CreatureScript
{
    public:
        npc_og_trogg() : CreatureScript("npc_og_trogg") { }

        struct npc_og_troggAI : public npc_escortAI
        {
            npc_og_troggAI(Creature* creature) : npc_escortAI(creature) {}

            void Reset(){}

            void WaypointReached(uint32 /*i*/){}

            void IsSummonedBy(Unit* /*who*/)
            {
                AddWaypoint(0, -5181.290039f, 629.843567f, 398.547211f);
                AddWaypoint(1, -5182.823730f, 612.078735f, 408.880646f);
                AddWaypoint(2, -5183.547852f, 600.796997f, 409.014313f);
                AddWaypoint(3, -5185.562500f, 581.175110f, 403.160065f);

                if (npc_og_troggAI* EscortAI = CAST_AI(npc_og_troggAI, me->AI()))
                {
                    EscortAI->Start(true, true);
                    EscortAI->SetDespawnAtEnd(false);
                }
            }

            void JustDied(Unit * /*who*/)
            {
                if (me->GetEntry() == NPC_GASHERIKK)
                    if (Creature* mekkatorque = me->FindCreatureWithDistance(NPC_MEKKATORQUE, 100, true))
                        CAST_AI(npc_og_mekkatorque::npc_og_mekkatorqueAI, mekkatorque->AI())->SpecialKill(2);
            }
        };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_og_troggAI(creature);
    }
};

class npc_og_i_infantry : public CreatureScript
{
    public:
        npc_og_i_infantry() : CreatureScript("npc_og_i_infantry") {}

        struct npc_og_i_infantryAI : public QuantumBasicAI
        {
            npc_og_i_infantryAI(Creature* creature) : QuantumBasicAI(creature) {}

            uint32 GrenadeTimer;
            uint32 CooldownTimer;

            void Reset()
            {
                GrenadeTimer = urand(10000, 15000);
                CooldownTimer = 3000;
            }

            void AttackStart(Unit* who)
            {
                if (!who)
                    return;

                if (me->Attack(who, true))
                {
                    me->AddThreat(who, 10.0f);
                    me->SetInCombatWith(who);
                    who->SetInCombatWith(me);
                    if (!urand(0,5))
                        DoStartMovement(who, 20.0f);
                    else
                        DoStartMovement(who, 5.0f);
                    SetCombatMovement(true);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (!me->IsWithinMeleeRange(me->GetVictim()))
                {
                    if (CooldownTimer <= diff)
                    {
                        DoCastVictim(SPELL_SHOOT);
                        CooldownTimer = 3000;
                    }
                    else CooldownTimer -= diff;
                }

                if (GrenadeTimer <= diff)
                {
                    if (Unit *target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, SPELL_RAD_GRENADE);
						GrenadeTimer = urand(10000, 15000);
					}
                }
                else GrenadeTimer -= diff;
			}
		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return new npc_og_i_infantryAI(creature);
		}
};

class npc_og_camera_vehicle : public CreatureScript
{
    public:
        npc_og_camera_vehicle() : CreatureScript("npc_og_camera_vehicle") {}

        struct npc_og_camera_vehicleAI : public QuantumBasicAI
        {
            npc_og_camera_vehicleAI(Creature* creature) : QuantumBasicAI(creature) {}

            void SpellHit(Unit* target, const SpellEntry* spell)
            {
                if (spell->Id == SPELL_BINDSIGHT)
                {
                    me->SetSpeed(MOVE_FLIGHT, 0.5f, true);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->GetMotionMaster()->MovePoint(0, -5169.104492f, 575.167786f, 416.563660f);
                }
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (me->HasAura(SPELL_BINDSIGHT))
                {
                    if (Unit* caster = me->GetAura(SPELL_BINDSIGHT)->GetCaster())
                    {
                        Player* player = caster->ToPlayer();
                        player->CastSpell(player, SPELL_RECALL_FINAL, true);
                        player->RemoveAurasDueToSpell(SPELL_SEE_INVISIBILITY);
                        player->RemoveAurasDueToSpell(SPELL_BINDSIGHT);
                        me->RemoveAurasDueToSpell(SPELL_BINDSIGHT);
                    }
                }
                me->DespawnAfterAction();
            }
        };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_og_camera_vehicleAI(creature);
    }
};

enum TrollinForVolunteers
{
	SPELL_PERSUADED              = 75075,
	SPELL_VOLUNTEER              = 75076,
	SPELL_VOLJIN_WAR_DRUMS       = 75102,
	SPELL_QUEST_CREDIT           = 75106,

	QUEST_TROLLIN_FOR_VOLUNTEERS = 25461,

    NPC_TROLL_CITIZEN_1          = 40256,
	NPC_TROLL_CITIZEN_2          = 40257,
	NPC_TROLL_VOLUNTEER_1        = 40260,
	NPC_TROLL_VOLUNTEER_2        = 40264,
};

class npc_champion_uru_zin : public CreatureScript
{
public:
    npc_champion_uru_zin() : CreatureScript("npc_champion_uru_zin") { }

    struct npc_champion_uru_zinAI : public QuantumBasicAI
    {
        npc_champion_uru_zinAI(Creature* creature) : QuantumBasicAI(creature) {}

		void Reset()
		{
			DoCast(me, SPELL_PERSUADED);
		}

        void MoveInLineOfSight(Unit* who)
        {
            QuantumBasicAI::MoveInLineOfSight(who);

            if (who->GetEntry() == NPC_TROLL_VOLUNTEER_1 && me->IsWithinDistInMap(who, 10.0f) || who->GetEntry() == NPC_TROLL_VOLUNTEER_2 && me->IsWithinDistInMap(who, 10.0f))
            {
                if (Unit* owner = who->GetOwner())
                {
					if (Player* player = owner->ToPlayer())
					{
						Creature* creature = who->ToCreature();

						if (player->GetQuestStatus(QUEST_TROLLIN_FOR_VOLUNTEERS) == QUEST_STATUS_INCOMPLETE)
							player->KilledMonsterCredit(NPC_TROLL_VOLUNTEER_1, 0);

						creature->DespawnAfterAction();

						if (player->GetQuestStatus(QUEST_TROLLIN_FOR_VOLUNTEERS) == QUEST_STATUS_COMPLETE)
						{
							owner->RemoveAllMinionsByEntry(NPC_TROLL_VOLUNTEER_1);
							owner->RemoveAllMinionsByEntry(NPC_TROLL_VOLUNTEER_2);
						}
					}
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_champion_uru_zinAI(creature);
    }
};

class npc_troll_citizen : public CreatureScript
{
public:
    npc_troll_citizen() : CreatureScript("npc_troll_citizen") {}

    struct npc_troll_citizenAI : public QuantumBasicAI
    {
		npc_troll_citizenAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 CheckAuraTimer;

		void Reset()
		{
			CheckAuraTimer = 500;
		}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_VOLJIN_WAR_DRUMS)
            {
				if (Player* player = caster->ToPlayer())
					me->DespawnAfterAction();
			}
        }

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_troll_citizenAI(creature);
    }
};

enum ZalazaneFall
{
	SPELL_BOON_OF_THE_LOA_1             = 74865, // Aura For Players
	SPELL_BOON_OF_THE_LOA_2             = 75169, // Aura For Players
	SPELL_ELEMENTAL_BLESSING            = 75341, // Aura For Players
	SPELL_TIKI_YELLOW_COSMETIC          = 75041,
	SPELL_ZALAZANE_FOOL                 = 75018, // Change to target faction 1771
	SPELL_ZALAZANE_FOOL_S               = 75496, // STUN USE AFTER COMPLETING QUEST AND PORTING TO QUESTTAKER
	SPELL_BWONSAMBIS_BOOT               = 75448, // SPELL_EFFECT_KILL_CREDIT 39901
	SPELL_BONES_OF_BWONDSAMDI           = 75426, // SUMMON AFTER VICTORY OF BWONDSAMBI
	SPELL_BONES_FEAR                    = 75374,
	SPELL_VOICE_OF_LEVITATE             = 75333,
	// Friedly creatures
	NPC_VOLJIN                          = 39654,
	NPC_TIKI_WARRIOR_1                  = 40199,
	NPC_TIKI_WARRIOR_2                  = 40263,
	NPC_VANIRA                          = 40192,
	NPC_ZENTABRA                        = 40196,
	NPC_BWONDSAMDI                      = 40182,
	// Trash
	NPC_RESTLESS_ZOMBIE_1               = 39639,
	NPC_RESTLESS_ZOMBIE_2               = 40274,
	NPC_MINDLESS_TROLL                  = 40195,
	NPC_HEXED_TROLL                     = 40231,
	NPC_DARKSPEAR_WARRIOR               = 40241, // CAST SPELL 
	// Bosses
	NPC_JINDO_THE_TRAITOR               = 40189, // 40195
	NPC_HEXED_DIRE_TROLL                = 40225, // 40231 (NPC_HEXED_DIRE_TROLL - SPAWN TWO)
	NPC_ZALAZANE_FRIENDLY               = 40502,
	NPC_ZALAZANE_HATED                  = 39647,
	// Worldstates
	WORLDSTATE_ZALAZANE_READY           = 5052, // Черное Копье готово сражаться за Острова Эха.
	WORLDSTATE_BWONSAMDI_HELP           = 5099, // Попросите Бвонсамди о помощи
	WORLDSTATE_BWONSAMDI_TROLL          = 5100, // Бвонсамди сражается на стороне троллей!
	WORLDSTATE_VILLAGE_WIN              = 5101, // Сожженная деревня была отвоевана!
	WORLDSTATE_ZALAZANE_ESCAPE          = 5102, // Залазан бежал на другой остров!
	WORLDSTATE_RETURN_VILLAGE           = 5056, // Помогите троллям вернуть сожженную деревню.
	WORLDSTATE_VOLJIN_VS_ZALAZANE       = 5090, // Войска Вол'джина сражаются с Залазаном.
	WORLDSTATE_ZALAZANE_IN_BATTLE       = 5060, // Идет битва с Залазаном
	WORLDSTATE_ZALAZANE_WIN             = 5061, // Залазан побежден!
	WORLDSTATE_BATTLE_TIME_TO_ECHO_ISLE = 5070, // Битва за Острова Эха начнется через:

	QUEST_ZALAZANE_FALLS                = 25445,
};

enum Sounds
{
	SOUND_VOLJIN_EVENT_1    = 17573,
	SOUND_VOLJIN_EVENT_2    = 17574,
	SOUND_VOLJIN_EVENT_3    = 17575,
	SOUND_VOLJIN_EVENT_4    = 17576,
	SOUND_VOLJIN_EVENT_5    = 17577,
	SOUND_VOLJIN_EVENT_6    = 17578,

	SOUND_VANIRA_EVENT_1    = 17630,
	SOUND_ZALAZANE_DEATH    = 17602,

	SOUND_ZENTABRA_EVENT_1  = 17621,
	SOUND_ZENTABRA_EVENT_2  = 17622,
	SOUND_ZENTABRA_EVENT_3  = 17623,
	SOUND_ZENTABRA_EVENT_4  = 17624,
	SOUND_ZENTABRA_EVENT_5  = 17625,
};

enum ZalazaneFallsData
{
	// Phases
	PHASE_ONE                   = 1,
	PHASE_TWO                   = 2,
	PHASE_THREE                 = 3,
	// Gossip Action
	GOSSIP_START_BATTLE         = 1,
	// DoActions
	ACTION_START_BATTLE         = 1,
	ACTION_TELEPORT_ALL_PLAYERS = 2,
	// Events
	EVENT_START_BATTLE          = 1,
	EVENT_ZALAZANE_BATTLE_1     = 2,
	EVENT_ZALAZANE_BATTLE_2     = 3,
	EVENT_ZALAZANE_BATTLE_3     = 4,
	EVENT_ZALAZANE_BATTLE_4     = 5,
	EVENT_TELEPORT_PLAYERS      = 6,
};

#define GOSSIP_START_EVENT "Vol'jin, I'm ready to start a battle!"

class npc_voljin_echo_isle : public CreatureScript
{
public:
	npc_voljin_echo_isle() : CreatureScript("npc_voljin_echo_isle") {}

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (creature->IsQuestGiver())
			player->PrepareQuestMenu(creature->GetGUID());

		if (player->GetQuestStatus(QUEST_ZALAZANE_FALLS) == QUEST_STATUS_INCOMPLETE)
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

		if (action == GOSSIP_ACTION_INFO_DEF+1)
		{
			player->CLOSE_GOSSIP_MENU();
			creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
			creature->AI()->DoAction(ACTION_START_BATTLE);
        }
		return true;
	}

	struct npc_voljin_echo_isleAI : public QuantumBasicAI
	{
		npc_voljin_echo_isleAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
		}

		EventMap events;

		void Reset()
		{
			events.Reset();
		}

		void DoAction(int32 const ActionId)
		{
			switch (ActionId)
			{
				case ACTION_START_BATTLE:
					events.ScheduleEvent(EVENT_START_BATTLE, 1*IN_MILLISECONDS);
					break;
				case ACTION_TELEPORT_ALL_PLAYERS:
					events.ScheduleEvent(EVENT_TELEPORT_PLAYERS, 5*IN_MILLISECONDS);
					break;
			}
		}

		void DoUpdateWorldState(uint32 StateId, uint32 StateData)
		{
			Map* map = me->GetMap();

			Map::PlayerList const& lPlayers = map->GetPlayers();

			if (!lPlayers.isEmpty())
			{
				for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				{
					if (Player* player = itr->getSource())
						player->SendUpdateWorldState(StateId, StateData);
				}
			}
		}

		void TeleportAllPlayers()
		{
			Map* map = me->GetMap();

			Map::PlayerList const& lPlayers = map->GetPlayers();

			if (!lPlayers.isEmpty())
			{
				for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
				{
					if (Player* player = itr->getSource())
					{
						if (player->GetQuestStatus(QUEST_ZALAZANE_FALLS) == QUEST_STATUS_COMPLETE)
						{
							player->CastSpell(player, SPELL_ZALAZANE_FOOL_S, true);
							player->TeleportTo(1, -779.679f, -5009.13f, 16.8664f, 0.338296f);
						}
					}
				}
			}
		}

		void UpdateAI(uint32 const diff)
		{
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
				    case EVENT_START_BATTLE:
						events.SetPhase(PHASE_ONE);
						DoUpdateWorldState(WORLDSTATE_ZALAZANE_READY, 1);
						DoUpdateWorldState(WORLDSTATE_BWONSAMDI_HELP, 1);
						me->PlayDirectSound(SOUND_VOLJIN_EVENT_1);
						events.ScheduleEvent(EVENT_ZALAZANE_BATTLE_1, 18*IN_MILLISECONDS, PHASE_ONE);
						break;
					case EVENT_ZALAZANE_BATTLE_1:
						me->PlayDirectSound(SOUND_VOLJIN_EVENT_2);
						events.ScheduleEvent(EVENT_ZALAZANE_BATTLE_2, 10*IN_MILLISECONDS, PHASE_ONE);
						break;
					case EVENT_ZALAZANE_BATTLE_2:
						if (Creature* vanira = me->FindCreatureWithDistance(NPC_VANIRA, 250.0f))
							vanira->PlayDirectSound(SOUND_VANIRA_EVENT_1);
						events.ScheduleEvent(EVENT_ZALAZANE_BATTLE_3, 7*IN_MILLISECONDS, PHASE_ONE);
						break;
					case EVENT_ZALAZANE_BATTLE_3:
						if (Creature* zentabra = me->FindCreatureWithDistance(NPC_ZENTABRA, 250.0f))
							zentabra->PlayDirectSound(SOUND_ZENTABRA_EVENT_1);
						events.ScheduleEvent(EVENT_ZALAZANE_BATTLE_4, 7*IN_MILLISECONDS, PHASE_ONE);
						break;
					case EVENT_ZALAZANE_BATTLE_4:
						DoUpdateWorldState(WORLDSTATE_ZALAZANE_READY, 0);
						DoUpdateWorldState(WORLDSTATE_BWONSAMDI_HELP, 0);
						DoUpdateWorldState(WORLDSTATE_VOLJIN_VS_ZALAZANE, 1);
						DoUpdateWorldState(WORLDSTATE_ZALAZANE_IN_BATTLE, 1);
						me->SummonCreature(NPC_ZALAZANE_HATED, -830.188f, -5388.01f, 0.305789f, 0.679942f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
						break;
					case EVENT_TELEPORT_PLAYERS:
						TeleportAllPlayers();
						break;
					default:
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_voljin_echo_isleAI(creature);
	}
};

enum Zalazane
{
	SPELL_CHAIN_LIGHTNING    = 75362,
	SPELL_HEALING_WAVE       = 75382,
	SPELL_LESER_HEALING_WAVE = 75366,
	SPELL_SHRINK             = 15802,
	SPELL_ZALAZANE_SHIELD    = 75099, // Not implemented
	SPELL_ZALAZANE_REMAINS   = 75105,
};

class boss_zalazane : public CreatureScript
{
public:
    boss_zalazane() : CreatureScript("boss_zalazane") { }

    struct boss_zalazaneAI : public QuantumBasicAI
    {
        boss_zalazaneAI(Creature* creature) : QuantumBasicAI(creature){}

        uint32 ChainLightningTimer;
        uint32 ShrinkTimer;
		uint32 HealingWaveTimer;
		uint32 LeserHealingWaveTimer;

        void Reset()
        {
            ChainLightningTimer = 500;
            ShrinkTimer = 3000;
			HealingWaveTimer = 5000;
			LeserHealingWaveTimer = 8000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void DoUpdateWorldState(uint32 StateId, uint32 StateData)
		{
			Map* map = me->GetMap();

			Map::PlayerList const& lPlayers = map->GetPlayers();

			if (!lPlayers.isEmpty())
			{
				for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
					if (Player* player = itr->getSource())
						player->SendUpdateWorldState(StateId, StateData);
			}
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* /*killer*/)
		{
			DoCast(me, SPELL_ZALAZANE_REMAINS, true);

			me->PlayDirectSound(SOUND_ZALAZANE_DEATH);

			if (Creature* voljin = me->FindCreatureWithDistance(NPC_VOLJIN, 500.0f, true))
			{
				voljin->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
				voljin->AI()->DoAction(ACTION_TELEPORT_ALL_PLAYERS);
			}

			DoUpdateWorldState(WORLDSTATE_VOLJIN_VS_ZALAZANE, 0);
			DoUpdateWorldState(WORLDSTATE_ZALAZANE_IN_BATTLE, 0);
			DoUpdateWorldState(WORLDSTATE_ZALAZANE_WIN, 1);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ChainLightningTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                     DoCast(target, SPELL_CHAIN_LIGHTNING);
					 ChainLightningTimer = urand(3000, 4000);
				}
            }
			else ChainLightningTimer -= diff;

            if (ShrinkTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHRINK);
					ShrinkTimer = urand(5000, 6000);
				}
            }
			else ShrinkTimer -= diff;

			if (HealingWaveTimer <= diff)
            {
				DoCast(me, SPELL_HEALING_WAVE);
				HealingWaveTimer = urand(8000, 9000);
            }
			else HealingWaveTimer -= diff;

			if (LeserHealingWaveTimer <= diff)
            {
				DoCast(me, SPELL_LESER_HEALING_WAVE);
				LeserHealingWaveTimer = urand(10000, 11000);
            }
			else LeserHealingWaveTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_zalazaneAI(creature);
    }
};

enum GatesController
{
	EVENT_CLOSE_DOOR  = 1,
	EVENT_OPEN_DOOR   = 2,

	GO_AHNQIRAJ_GATE  = 176146,
	GO_AHNQIRAJ_ROOTS = 176147,
	GO_AHNQIRAJ_RUNES = 176148,
};

class npc_ahnqiraj_gates_controller : public CreatureScript
{
public:
    npc_ahnqiraj_gates_controller() : CreatureScript("npc_ahnqiraj_gates_controller") { }

	struct npc_ahnqiraj_gates_controllerAI : public QuantumBasicAI
    {
		npc_ahnqiraj_gates_controllerAI(Creature* creature) : QuantumBasicAI(creature){}

		EventMap events;

        void Reset()
        {
			events.Reset();

			events.ScheduleEvent(EVENT_CLOSE_DOOR, 5*IN_MILLISECONDS);
        }

		void CloseAllDoors()
		{
			if (GameObject* Gate = me->FindGameobjectWithDistance(GO_AHNQIRAJ_GATE, 250.0f))
				Gate->SetGoState(GO_STATE_READY);

			if (GameObject* Roots = me->FindGameobjectWithDistance(GO_AHNQIRAJ_ROOTS, 250.0f))
				Roots->SetGoState(GO_STATE_READY);

			if (GameObject* Runes = me->FindGameobjectWithDistance(GO_AHNQIRAJ_RUNES, 250.0f))
				Runes->SetGoState(GO_STATE_READY);
		}

		void OpenAllDoors()
		{
			if (GameObject* Gate = me->FindGameobjectWithDistance(GO_AHNQIRAJ_GATE, 250.0f))
				Gate->SetGoState(GO_STATE_ACTIVE);

			if (GameObject* Roots = me->FindGameobjectWithDistance(GO_AHNQIRAJ_ROOTS, 250.0f))
				Roots->SetGoState(GO_STATE_ACTIVE);

			if (GameObject* Runes = me->FindGameobjectWithDistance(GO_AHNQIRAJ_RUNES, 250.0f))
				Runes->SetGoState(GO_STATE_ACTIVE);
		}

        void UpdateAI(const uint32 diff)
        {
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				    case EVENT_CLOSE_DOOR:
						CloseAllDoors();
						events.ScheduleEvent(EVENT_OPEN_DOOR, 30*IN_MILLISECONDS);
						break;
			        case EVENT_OPEN_DOOR:
						OpenAllDoors();
						events.ScheduleEvent(EVENT_CLOSE_DOOR, 30*IN_MILLISECONDS);
						break;
					default:
						break;
				}
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ahnqiraj_gates_controllerAI(creature);
	}
};

void AddSC_world_event_scripts()
{
	new npc_avatar_of_freya();
	new npc_ian_drake();
	new npc_edward_cairn();
	new npc_higlord_tirion_fordring_atg();
	new npc_og_camera_vehicle();
    new npc_og_mekkatorque();
    new npc_og_assistants();
    new npc_og_i_infantry();
    new npc_og_infantry();
    new npc_og_boltcog();
    new npc_og_cannon();
    new npc_og_i_tank();
    new npc_og_bomber();
    new npc_og_trogg();
    new npc_og_tank();
    new npc_og_suit();
    new npc_og_rl();
	new npc_champion_uru_zin();
	new npc_troll_citizen();
	new npc_voljin_echo_isle();
	new boss_zalazane();
	//new npc_ahnqiraj_gates_controller();
}