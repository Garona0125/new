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
#include "QuantumFollowerAI.h"
#include "Vehicle.h"

#define GOSSIP_ITEM_1 "<Reach down and pull the injured Rainspeaker Oracle to it's feet.>"
#define GOSSIP_ITEM_2 "I am ready to travel to your village now."

enum Rainspeaker
{
	SPELL_R_RAINSPEAKER_ORACLE_STATE   = 50503,
    SAY_START_WAYPOINT                 = -1571000,
    SAY_QUEST_ACCEPT                   = -1571001,
    SAY_REACHED_HOME                   = -1571002,

    QUEST_FORTUNATE_MISUNDER_STANDINGS = 12570,
    QUEST_JUST_FOLLOWING_ORDERS        = 12540,
	NPC_INJURED_RAINSPEAKER_ORACLE     = 28217,
    NPC_RAVENOUS_MANGAL_CROCOLISK      = 28325,

	FACTION_FRENZYHEART                = 1104,
	FACTION_ORACLES                    = 1105,

	EVENT_REACHED_HOME                 = 1,
};

class npc_injured_rainspeaker_oracle : public CreatureScript
{
public:
    npc_injured_rainspeaker_oracle() : CreatureScript("npc_injured_rainspeaker_oracle") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

		if (player->GetQuestStatus(QUEST_JUST_FOLLOWING_ORDERS) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        if (player->GetQuestStatus(QUEST_FORTUNATE_MISUNDER_STANDINGS) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
		if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            player->SummonCreature(NPC_RAVENOUS_MANGAL_CROCOLISK, 5379.35f, 4507.82f, -129.595f, 0.740071f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
            player->CLOSE_GOSSIP_MENU();
        }

        else if (action == GOSSIP_ACTION_INFO_DEF + 2)
		{
            DoSendQuantumText(SAY_START_WAYPOINT, creature);
			creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
			creature->SetWalk(true);
			creature->SetSpeed(MOVE_WALK, 1.5f, true);
			creature->SetSpeed(MOVE_RUN, 1.5f, true);
			creature->GetMotionMaster()->MovePath(creature->GetEntry(), false);
		}

		return true;
    }

	bool OnQuestAccept(Player* /*player*/, Creature* creature, Quest const* /*quest*/)
    {
        DoSendQuantumText(SAY_QUEST_ACCEPT, creature);
        return false;
    }

    bool OnQuestReward(Player* player, Creature* creature, Quest const* quest, uint32 /*item*/)
    {
		if (quest->GetQuestId() == QUEST_JUST_FOLLOWING_ORDERS)
			player->GetReputationMgr().SetReputation(sFactionStore.LookupEntry(FACTION_ORACLES), 3000);

		return true;
    }

    struct npc_injured_rainspeaker_oracleAI : public QuantumBasicAI
    {
        npc_injured_rainspeaker_oracleAI(Creature* creature) : QuantumBasicAI(creature){}

        EventMap events;

        void Reset()
		{
			DoCast(me, SPELL_R_RAINSPEAKER_ORACLE_STATE);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != WAYPOINT_MOTION_TYPE)
                return;

            if (id == 15)
			{
				DoSendQuantumText(SAY_REACHED_HOME, me);
				CompleteQuest();

				me->DespawnAfterAction(5*IN_MILLISECONDS);
			}
        }

		void CompleteQuest()
		{
			Map::PlayerList const& Players = me->GetMap()->GetPlayers();
			for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
			{
				if (Player* player = itr->getSource())
				{
					if (player->IsWithinDist(me, 25.0f))
					{
						if (player->GetQuestStatus(QUEST_FORTUNATE_MISUNDER_STANDINGS) == QUEST_STATUS_INCOMPLETE)
							player->GroupEventHappens(QUEST_FORTUNATE_MISUNDER_STANDINGS, me);
					}
				}
			}
		}

		void UpdateAI(uint32 const /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_injured_rainspeaker_oracleAI(creature);
    }
};

#define GOSSIP_VEKJIK_ITEM1 "Shaman Vekjik, I have spoken with the big-tongues and they desire peace. I have brought this offering on their behalf."
#define GOSSIP_VEKJIK_ITEM2 "No no... I had no intentions of betraying your people. I was only defending myself. it was all a misunderstanding."

enum Vekjik
{
    GOSSIP_TEXTID_VEKJIK1    = 13137,
    GOSSIP_TEXTID_VEKJIK2    = 13138,
    SAY_TEXTID_VEKJIK1       = -1000208,
    SPELL_FREANZYHEARTS_FURY = 51469,
    QUEST_MAKING_PEACE       = 12573,
};

class npc_vekjik : public CreatureScript
{
public:
    npc_vekjik() : CreatureScript("npc_vekjik") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_MAKING_PEACE) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_VEKJIK_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_VEKJIK1, creature->GetGUID());
            return true;
        }

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_VEKJIK_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_VEKJIK2, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->CLOSE_GOSSIP_MENU();
                DoSendQuantumText(SAY_TEXTID_VEKJIK1, creature, player);
                player->AreaExploredOrEventHappens(QUEST_MAKING_PEACE);
                creature->ApplySpellImmune(0,IMMUNITY_ID, SPELL_FREANZYHEARTS_FURY, true);
				creature->CastSpell(player, SPELL_FREANZYHEARTS_FURY, false);
                break;
        }
        return true;
    }
};

class npc_bushwhacker : public CreatureScript
{
public:
	npc_bushwhacker() : CreatureScript("npc_bushwhacker") { }

    struct npc_bushwhackerAI : public QuantumBasicAI
    {
        npc_bushwhackerAI(Creature* creature) : QuantumBasicAI(creature){}

        void InitializeAI()
        {
            if (me->IsDead())
                return;

            if (me->IsSummon())
			{
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
				{
                    if (summoner)
                        me->GetMotionMaster()->MovePoint(0, summoner->GetPositionX(), summoner->GetPositionY(), summoner->GetPositionZ());
				}
			}
			
			Reset();
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
        return new npc_bushwhackerAI(creature);
    }
};

enum Helice
{
    SAY_WP_7              = -1800047,
    SAY_WP_6              = -1800048,
    SAY_WP_5              = -1800049,
    SAY_WP_4              = -1800050,
    SAY_WP_3              = -1800051,
    SAY_WP_2              = -1800052,
    SAY_WP_1              = -1800053,

	SPELL_EXPLODE_CRYSTAL = 62487,
    SPELL_FLAMES          = 64561,
    QUEST_DISASTER        = 12688,
	GO_CONSUMING_FLAMES   = 184743,
};

class npc_engineer_helice : public CreatureScript
{
public:
    npc_engineer_helice() : CreatureScript("npc_engineer_helice") { }

    struct npc_engineer_heliceAI : public npc_escortAI
    {
        npc_engineer_heliceAI(Creature* creature) : npc_escortAI(creature) { }

        uint32 ChatTimer;

        void WaypointReached(uint32 id)
        {
            Player* player = GetPlayerForEscort();
            switch (id)
            {
                case 0:
                    DoSendQuantumText(SAY_WP_2, me);
                    break;
                case 1:
                    DoSendQuantumText(SAY_WP_3, me);
                    me->CastSpell(5918.33f, 5372.91f, -98.770f, SPELL_EXPLODE_CRYSTAL, true);
                    me->SummonGameObject(GO_CONSUMING_FLAMES, 5918.33f, 5372.91f, -98.770f, 0, 0, 0, 0, 0, TEMPSUMMON_MANUAL_DESPAWN);     //approx 3 to 4 seconds
                    me->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
                    break;
                case 2:
                    DoSendQuantumText(SAY_WP_4, me);
                    break;
                case 7:
                    DoSendQuantumText(SAY_WP_5, me);
                    break;
                case 8:
                    me->CastSpell(5887.37f, 5379.39f, -91.289f, SPELL_EXPLODE_CRYSTAL, true);
                    me->SummonGameObject(GO_CONSUMING_FLAMES, 5887.37f, 5379.39f, -91.289f, 0, 0, 0, 0, 0, TEMPSUMMON_MANUAL_DESPAWN);      //approx 3 to 4 seconds
                    me->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
                    break;
                case 9:
                    DoSendQuantumText(SAY_WP_6, me);
                    break;
                case 13:
                    if (player)
                    {
                        player->GroupEventHappens(QUEST_DISASTER, me);
                        DoSendQuantumText(SAY_WP_7, me);
                    }
                    break;
            }
        }

        void Reset()
        {
            ChatTimer = 4000;
        }

        void JustDied(Unit* /*killer*/)
        {
            Player* player = GetPlayerForEscort();
            if (HasEscortState(STATE_ESCORT_ESCORTING))
            {
                if (player)
                    player->FailQuest(QUEST_DISASTER);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (HasEscortState(STATE_ESCORT_ESCORTING))
            {
                if (ChatTimer <= diff)
                {
                    ChatTimer = 12000;
                }
                else ChatTimer -= diff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_engineer_heliceAI(creature);
    }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_DISASTER)
        {
            if (npc_engineer_heliceAI* EscortAI = CAST_AI(npc_engineer_helice::npc_engineer_heliceAI, creature->AI()))
            {
                creature->GetMotionMaster()->MoveJumpTo(0, 0.4f, 0.4f);
                creature->SetCurrentFaction(113);

                EscortAI->Start(false, false, player->GetGUID());
                DoSendQuantumText(SAY_WP_1, creature);
            }
        }
        return true;
    }
};

enum HighOracleSooSay
{
	SPELL_RAINSPEAKER_ORACLE_STATE       = 50503,
	SPELL_THUNDERSTORM                   = 53071,

    ITEM_JALOOTS_FAVORITE_CRYSTAL        = 38623,
    ITEM_LAFOOS_BUG_BAG                  = 38622,
    ITEM_MOODLES_STRESS_BALL             = 38624,

    QUEST_APPEASING_THE_GREAT_RAIN_STONE = 12704, //Alle
    QUEST_GODS_LIKE_SHINEY_THINGS        = 12572, //Lafoo
    QUEST_MAKE_THE_BAD_SNAKE_GO_AWAY     = 12571, //Lafoo
    //QUEST_MAKING_PEACE = 12573, //Lafoo
    QUEST_BACK_SO_SOON                   = 12574, //Jaloo
    QUEST_FORCED_HAND                    = 12576, //Jaloo
    QUEST_THE_LOST_MISTWHISPER_TRESURE   = 12575, //Jaloo
    QUEST_THE_ANGRY_GORLOC               = 12578, //Moodle
    QUEST_A_HEROS_BURDEN                 = 12581, //Moodle
};

#define GOSSIP_LAFOO_ITEM  "I need to find Lafoo, do you have his bug bag?"
#define GOSSIP_JALOOT_ITEM "I need to find Jaloot, do you have his favorite crystal?"
#define GOSSIP_MOODLE_ITEM "I need to find Moodle, do you have his stressball?"

class npc_high_oracle_soo_say : public CreatureScript
{
public:
    npc_high_oracle_soo_say() : CreatureScript("npc_high_oracle_soo_say") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_APPEASING_THE_GREAT_RAIN_STONE) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LAFOO_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_JALOOT_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_MOODLE_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature),creature->GetGUID());
            return true;
        }

        if (player->GetQuestStatus(QUEST_MAKE_THE_BAD_SNAKE_GO_AWAY) == QUEST_STATUS_INCOMPLETE
            || (player->GetQuestStatus(QUEST_GODS_LIKE_SHINEY_THINGS) == QUEST_STATUS_INCOMPLETE
            || player->GetQuestStatus(QUEST_GODS_LIKE_SHINEY_THINGS) == QUEST_STATUS_NONE)
            || player->GetQuestStatus(QUEST_MAKING_PEACE) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LAFOO_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        if (player->GetQuestStatus(QUEST_BACK_SO_SOON) == QUEST_STATUS_INCOMPLETE
            || player->GetQuestStatus(QUEST_FORCED_HAND) == QUEST_STATUS_INCOMPLETE
            || player->GetQuestStatus(QUEST_THE_LOST_MISTWHISPER_TRESURE) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_JALOOT_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

        if (player->GetQuestStatus(QUEST_THE_ANGRY_GORLOC) == QUEST_STATUS_INCOMPLETE
            || (player->GetQuestStatus(QUEST_THE_ANGRY_GORLOC) == QUEST_STATUS_COMPLETE
            && player->GetQuestStatus(QUEST_A_HEROS_BURDEN) == QUEST_STATUS_NONE))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_MOODLE_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature),creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        ItemPosCountVec dest;
        uint32 ToStoreItem;

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                ToStoreItem = ITEM_LAFOOS_BUG_BAG;
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                ToStoreItem = ITEM_JALOOTS_FAVORITE_CRYSTAL;
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                ToStoreItem = ITEM_MOODLES_STRESS_BALL;
                break;
        }

		if (ToStoreItem > 0)
        {
            uint8 CanStoreNewItem = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ToStoreItem, 1);
            if (CanStoreNewItem == EQUIP_ERR_OK)
			{
                Item *newItem = 0;
                newItem = player->StoreNewItem(dest, ToStoreItem, 1, true);
                player->SendNewItem(newItem, 1, true, false);
            }
            player->CLOSE_GOSSIP_MENU();
        }
        return true;
	}

	struct npc_high_oracle_soo_sayAI : public QuantumBasicAI
	{
		npc_high_oracle_soo_sayAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ThunderstormTimer;

		void Reset()
		{
			ThunderstormTimer = 2000;

			DoCast(me, SPELL_RAINSPEAKER_ORACLE_STATE);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ThunderstormTimer <= diff)
			{
				DoCastAOE(SPELL_THUNDERSTORM);
				ThunderstormTimer = urand(5000, 6000);
			}
			else ThunderstormTimer -= diff;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_high_oracle_soo_sayAI(creature);
    }
};

enum TreasureTrigger
{
    SPELL_GLINTING_BUG_SPARKLE    = 51444,
    NPC_ORACLE_MOODLE_ID          = 28122,
    NPC_ORACLE_JALOOT_ID          = 28121,
    NPC_ORACLE_LAFOO_ID           = 28120,

    TRIGGER_SIGHT_DIST            = 15,
    DATA_CHANGE_FORCE_AURA_REMOVE = 666,
    DATA_CHANGE_FORCE_DESPAWN     = 667,
};

class npc_generic_oracle_treasure_trigger : public CreatureScript
{
public:
    npc_generic_oracle_treasure_trigger() : CreatureScript("npc_generic_oracle_treasure_trigger") { }

    struct npc_generic_oracle_treasure_triggerAI: public QuantumBasicAI
    {
        npc_generic_oracle_treasure_triggerAI(Creature* creature) : QuantumBasicAI(creature)
        {
            InitialCast = true;
            AuraTimer = 3000;
            ForceAuraRemove = false;
            ForceDespawn = false;
        }

        bool AuraRemoved;
        bool InitialCast;
        bool ForceAuraRemove;
        bool ForceDespawn;
        uint32 AuraTimer;

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);
            AuraRemoved = false;
            ForceAuraRemove = false;
            ForceDespawn = false;
        }

        void ChangeForceAuraRemove(bool condition)
        {
            ForceAuraRemove = condition;
        }

        void ChangeForceDespawn(bool despawn)
        {
            ForceDespawn = despawn;
        }

        void SetData(uint32 data, uint32 value)
        {
            if (data == DATA_CHANGE_FORCE_AURA_REMOVE)
                ChangeForceAuraRemove(value == 1);

            if (data == DATA_CHANGE_FORCE_DESPAWN)
                ChangeForceDespawn(value == 1);
        }

        Player* GetNearPlayer()
        {
            float radius = TRIGGER_SIGHT_DIST + 10;

            if (me->FindPlayerWithDistance(radius) && (me->FindPlayerWithDistance(radius)->GetQuestStatus(QUEST_APPEASING_THE_GREAT_RAIN_STONE) == QUEST_STATUS_INCOMPLETE || me->FindPlayerWithDistance(radius)->GetQuestStatus(QUEST_GODS_LIKE_SHINEY_THINGS) == QUEST_STATUS_INCOMPLETE))
                return me->FindPlayerWithDistance(radius);
            return 0;
        }

        void UpdateAI(const uint32 diff)
        {
            if (InitialCast)
            {
                me->CastSpell(me, SPELL_GLINTING_BUG_SPARKLE, false);
                InitialCast = false;
            }

            if (ForceDespawn)
                me->DespawnAfterAction(0);

            if (!ForceAuraRemove)
            {
                if (AuraTimer <= diff)
                {
                    if (GetNearPlayer())
                    {
                        if (!AuraRemoved)
                        {
                            if (!me->FindCreatureWithDistance(NPC_ORACLE_MOODLE_ID, TRIGGER_SIGHT_DIST) && !me->FindCreatureWithDistance(NPC_ORACLE_JALOOT_ID, TRIGGER_SIGHT_DIST) && !me->FindCreatureWithDistance(NPC_ORACLE_LAFOO_ID, TRIGGER_SIGHT_DIST)) {
                                me->RemoveAllAuras();
                                AuraRemoved = true;
                            }
                        }

                        if (AuraRemoved)
                        {
                            if (me->FindCreatureWithDistance(NPC_ORACLE_MOODLE_ID, TRIGGER_SIGHT_DIST + 8) || me->FindCreatureWithDistance(NPC_ORACLE_JALOOT_ID, TRIGGER_SIGHT_DIST + 8) || me->FindCreatureWithDistance(NPC_ORACLE_LAFOO_ID, TRIGGER_SIGHT_DIST + 8)) {
                                DoCast(me, SPELL_GLINTING_BUG_SPARKLE);
                                Reset();
                            }
                        }
                    }
					else
                    {
                        me->RemoveAllAuras();
                        AuraRemoved = true;
                    }
                    AuraTimer = 3000;
                }
				else AuraTimer -= diff;
            }
			else
            {
                me->RemoveAllAuras();
                AuraRemoved = true;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_generic_oracle_treasure_triggerAI(creature);
    }
};

static const char* GenericSayings[] =
{
	"That weird. Someone took our bugs. You have maybe?",
	"You have pie? Someone gave us pie year ago. Really yummy.",
	"One day maybe you can be Soo-dryskin. You like?",
	"You got funny looking eyeballs.",
	"Never meet Great Ones. Nope, just met their shrines.",
	"Chase the hatchlings! Chase the hatchlings! So fast!",
	"Mistcaller really good at controlling weather with shrine. They know just which crystals to use! Really good!",
	"You like that? Bet not! Bet not cause you dead! Hahah!",
	"We know you have dragon thing! We not scared! We going to get you and dragon thing!",
	"Teach you to steal our treasures! Our treasures are... ours, okay?",
	"Want us to help with something? What we help with? We good at helping.",
	"Hello! You're pretty strong and nice and stuff. I like you. Lets stay friends, okay?",
	"Misty. Very misty. Seem sort of misty to you? Maybe not so much.",
};

static const char* SpecialSayings[] =
{
	"Home good, but not stay long! More adventures!",
	"Good home. Had it long time, still stay good.",
	"There some reason why we're still here? I'd love to hear it",
	"Planning to sit on your butt some more, or are we going to go do something that's actually useful?",
	"Yeah, let's go now.",
	"No come back this time!",
	"Crunchy bugs so delicious. Want?",
	"Ever trip over own feet? We have. Kind of hurt. No do it, not fun. Nope.",
	"What the heck are shiny rabbits doing here anyway?",
};

static const char* SeekerSayings[] =
{
	"Ooh! Shinies!",
	"Maybe more shiny bugs! We like bugs!",
	"Treasure for me! Treasure for shrine!",
	"Dig, dig, dig...",
	"Shiny things! Shiny things! Not as shiny as my crystal, but still shiny!",
	"More treasures! Treasures of all kinds! Yay treasures!",
	"Can never have too many shiny crystals! Nope!",
	"We are fast digger! Really fast! See... all done! That was fast, huh?",
	"Yay... more dirt.",
	"Here I am, digging up some useless bauble on the will of someone I just met...does that seem right to you?", "I'm done here",
};

enum OracleData
{
    NPC_GLINTING_DIRT       = 28362,
	NPC_HIGH_ORACLE_SOO_SAY = 28027,
	NPC_GLINTING_BUG        = 28372,
	NPC_MOODLE              = 28122,

    TRIGGER_SEARCH_DIST     = 10,
    FACTION_ID              = 35,
    HOME_DIST               = 50,
    AREA_MOSSWALKER_VILLAGE = 4297,
};

enum SeekingSpells
{
    SPELL_DIG_UP_GLINTING_BUG          = 51443,
    SPELL_DIG_UP_SPARKLING_HARE        = 51441,
    SPELL_DIG_UP_DISGRUNTLED_BUG_COVER = 52928,
    SPELL_DIG_UP_GLINTING_ARMOR        = 51398,
    SPELL_DIG_UP_GLOWING_GEM           = 51400,
    SPELL_DIG_UP_POLISHED_PLATTER      = 51401,
    SPELL_DIG_UP_SHINING_CRYSTAL       = 51397,
    SPELL_DIG_UP_SPARKLING_TREASURE    = 51402,
};

class npc_generic_oracle_treasure_seeker : public CreatureScript
{
public:
    npc_generic_oracle_treasure_seeker() : CreatureScript("npc_generic_oracle_treasure_seeker") { }
	
	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    struct npc_generic_oracle_treasure_seekerAI: public FollowerAI
    {
        npc_generic_oracle_treasure_seekerAI(Creature* creature) : FollowerAI(creature) {}

		uint32 CheckTimer;
		uint32 SearchTimer;
		uint32 GeneralSayTimer;
		uint32 TreasureSpell;
		uint32 AreaCheckTimer;
		uint8 SearchCount;

		bool AlreadyMoved;
		bool SearchAnim;
		bool SayHome;
		bool IsAway;

		void Reset()
		{
			if (HasFollowState(STATE_FOLLOW_PAUSED | STATE_FOLLOW_INPROGRESS))
				SetFollowPaused(false);

			if (me->GetOwner() && Unit::GetPlayer(*me, me->GetOwner()->GetGUID()))
				StartFollow(Unit::GetPlayer(*me, me->GetOwner()->GetGUID()), FACTION_ID, 0);

                me->SetReactState(REACT_DEFENSIVE);

                AlreadyMoved = false;
                CheckTimer = 2000;
                SearchTimer = 0;
                SearchCount = 1;
                SearchAnim = false;
                SayHome = false;
                GeneralSayTimer = 60000;
                IsAway = false;
                AreaCheckTimer = 5000;
            }

            void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
            {
                switch(spell->Id)
                {
				    case SPELL_DIG_UP_GLINTING_BUG:
					case SPELL_DIG_UP_SPARKLING_HARE:
					case SPELL_DIG_UP_GLINTING_ARMOR:
					case SPELL_DIG_UP_GLOWING_GEM:
					case SPELL_DIG_UP_POLISHED_PLATTER:
					case SPELL_DIG_UP_SHINING_CRYSTAL:
					case SPELL_DIG_UP_SPARKLING_TREASURE:
						break;
					default:
						if (HasFollowState(STATE_FOLLOW_INPROGRESS) && rand() % 10 == 1)
						{
							me->MonsterSay(SpecialSayings[7], LANG_UNIVERSAL, 0);
						}
						break;
				}
            }

            void UpdateFollowerAI(const uint32 diff)
            {
                if (me->GetEntry() == NPC_MOODLE)
                {
                    if (AreaCheckTimer <= diff)
                    {
                        if (me->GetAreaId() == AREA_MOSSWALKER_VILLAGE)
						{
                            if (me->GetOwner() && me->GetOwner()->ToPlayer())
							{
                                if (me->GetOwner()->ToPlayer()->GetQuestStatus(QUEST_THE_ANGRY_GORLOC) == QUEST_STATUS_INCOMPLETE)
                                    me->GetOwner()->ToPlayer()->CompleteQuest(QUEST_THE_ANGRY_GORLOC);
							}
						}
                        AreaCheckTimer = 5000;
                    }
					else AreaCheckTimer -= diff;
                }

                if (CheckTimer <= diff)
                {
                    if (me->GetOwner() && Unit::GetPlayer(*me, me->GetOwner()->GetGUID()))
                        IsAway = Unit::GetPlayer(*me, me->GetOwner()->GetGUID())->IsAFK();

                    if (!IsAway)
                    {
                        if (!SayHome && me->FindCreatureWithDistance(NPC_HIGH_ORACLE_SOO_SAY, HOME_DIST))
                        {
                            me->MonsterSay(SpecialSayings[urand(0, 1)], LANG_UNIVERSAL, 0);
                            SayHome = true;
                        }
                        if (SayHome && !me->FindCreatureWithDistance(NPC_HIGH_ORACLE_SOO_SAY, HOME_DIST))
                            SayHome = false;

                        if (me->GetOwner() && Unit::GetPlayer(*me,me->GetOwner()->GetGUID()) && Unit::GetPlayer(*me,me->GetOwner()->GetGUID())->GetQuestStatus(QUEST_APPEASING_THE_GREAT_RAIN_STONE) == QUEST_STATUS_INCOMPLETE
                                || Unit::GetPlayer(*me,me->GetOwner()->GetGUID())->GetQuestStatus(QUEST_GODS_LIKE_SHINEY_THINGS) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (Creature* OracleTrigger = me->FindCreatureWithDistance(NPC_GLINTING_DIRT, TRIGGER_SEARCH_DIST))
                            {
                                OracleTrigger->AI()->SetData(DATA_CHANGE_FORCE_AURA_REMOVE, 1);
                                SetFollowPaused(true);
                                if (HasFollowState(STATE_FOLLOW_PAUSED))
                                {
                                    if (!AlreadyMoved)
									{
                                        me->GetMotionMaster()->MovePoint(0, OracleTrigger->GetPositionX(), OracleTrigger->GetPositionY(), OracleTrigger->GetPositionZ());
                                        AlreadyMoved = true;
                                    }

                                    if (me->IsWithinDistInMap(OracleTrigger, 1))
                                        SearchAnim = true;
                                }
                                CheckTimer = 2000;
                            }
                        }
                    }
					else
                    {
                        me->MonsterSay(SpecialSayings[urand(2, 4)], LANG_UNIVERSAL, 0);
                        CheckTimer = 120000;
                    }
                }
				else CheckTimer -= diff;

                if (GeneralSayTimer <= diff)
                {
                    if (!SearchAnim && !IsAway)
                        me->MonsterSay(GenericSayings[urand(0, 12)], LANG_UNIVERSAL, 0);

                    GeneralSayTimer = 30000 + urand(300000, 600000);
                }
				else GeneralSayTimer -= diff;

                if (SearchAnim)
                {
                    if (SearchTimer <= diff)
                    {
                        if (SearchCount < 4)
                            DoCast(me, SPELL_DIG_UP_DISGRUNTLED_BUG_COVER);

                        if (SearchCount == 2)
                        {
                            TreasureSpell = RAND(SPELL_DIG_UP_GLINTING_ARMOR, SPELL_DIG_UP_SPARKLING_HARE, SPELL_DIG_UP_GLOWING_GEM, SPELL_DIG_UP_POLISHED_PLATTER, SPELL_DIG_UP_SPARKLING_TREASURE, SPELL_DIG_UP_GLINTING_BUG);
                            if (TreasureSpell == SPELL_DIG_UP_GLINTING_BUG)
                                me->MonsterSay(SpecialSayings[6], LANG_UNIVERSAL, 0);
                            else
                                if (TreasureSpell == SPELL_DIG_UP_SPARKLING_HARE)
                                    me->MonsterSay(SpecialSayings[8], LANG_UNIVERSAL, 0);
                                else
                                    me->MonsterSay(SeekerSayings[urand(0, 10)], LANG_UNIVERSAL, 0);
                        }

                        if (SearchCount == 4)
                            DoCast(me, TreasureSpell);

                        if (SearchCount > 4)
                        {
                            if (Creature* OracleTrigger = me->FindCreatureWithDistance(NPC_GLINTING_DIRT, TRIGGER_SEARCH_DIST-8))
							{
                                OracleTrigger->AI()->SetData(DATA_CHANGE_FORCE_DESPAWN, 1);
								Reset();
							}
                        }

                        SearchTimer = 1000;
                        SearchCount++;
                    }
					else SearchTimer -= diff;
				}
          }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_generic_oracle_treasure_seekerAI(creature);
    }
};

#define QUEST_A_ROUGH_RIDE              12536
#define AREA_MISTWHISPER_REFUGE         4306

class npc_frenzyheart_zepik : public CreatureScript
{
public:
    npc_frenzyheart_zepik() : CreatureScript("npc_frenzyheart_zepik") { }

    struct npc_frenzyheart_zepikAI: public QuantumBasicAI
    {
        npc_frenzyheart_zepikAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CheckTimer;

        void Reset()
        {
            CheckTimer = 5000;
            if (me->GetOwner() && me->GetOwner()->ToPlayer())
                me->GetMotionMaster()->MoveFollow(me->GetOwner()->ToPlayer(), PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
        }

        void Aggro(){}

        void UpdateAI(const uint32 diff)
        {
            if (CheckTimer <= diff)
            {
                if (me->GetAreaId() == AREA_MISTWHISPER_REFUGE)
                    if (me->GetOwner() && me->GetOwner()->ToPlayer())
                    {
                        if (me->GetOwner()->ToPlayer()->GetQuestStatus(QUEST_A_ROUGH_RIDE) == QUEST_STATUS_INCOMPLETE)
                            me->GetOwner()->ToPlayer()->CompleteQuest(QUEST_A_ROUGH_RIDE);
					}
					CheckTimer = 5000;
			}
			else CheckTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frenzyheart_zepikAI(creature);
    }
};

enum MosswalkerVictim
{
	SPELL_MOSSWALKER_QUEST_CREDIT = 52157,
	QUEST_THE_MOSSWALKER_SAVIOR   = 12580,
};

static const char* dead_sayings[5] =
{
    "Please take... my shinies. All done...",
    "We not do anything... to them... I no understand.",
    "Use my shinies... make weather good again... make undead things go away.",
    "We gave shinies to shrine... we not greedy... why this happen?",
    "I do something bad? I sorry..."
};

static const char* survive_sayings[4] =
{
    "We saved. You nice, dryskin.",
    "Maybe you make weather better too?",
    "You save us! Yay for you!",
    "Thank you! You good!"
};

#define GOSSIP_EVENT "<Check for pulse.>"

class npc_mosswalker_victim : public CreatureScript
{
public:
    npc_mosswalker_victim() : CreatureScript("npc_mosswalker_victim") { }

    struct npc_mosswalker_victimAI: public QuantumBasicAI
    {
        npc_mosswalker_victimAI(Creature* creature) : QuantumBasicAI(creature) {}

        bool PulseChecked;
        bool HasSurvived;
        bool DoReset;
        uint32 ActionTimer;

        void Reset()
        {
            PulseChecked = false;
            ActionTimer = 5000;
            DoReset = false;
        }

        void CheckPulse(Player* player)
        {
            PulseChecked = true;
            HasSurvived = (urand(0,1) == 0);
            uint32 CheckRand;
            if (HasSurvived)
            {
                CheckRand = urand(0,3);
                me->CastSpell(player,SPELL_MOSSWALKER_QUEST_CREDIT,true);
                me->MonsterSay(survive_sayings[CheckRand],LANG_UNIVERSAL, 0);
                ActionTimer = 15000;
            }
            else
            {
                CheckRand = urand(0,4);
                me->MonsterSay(dead_sayings[CheckRand],LANG_UNIVERSAL, 0);
                ActionTimer = 2000;
            }
        }

        void Aggro() { }
        void MoveInLineOfSight(Unit* /*who*/) {}
        void AttackStart(Unit* /*who*/) {}
        void UpdateAI(const uint32 diff)
        {
            if (PulseChecked)
            {
                if (ActionTimer <= diff)
                {
                    if (HasSurvived)
                    {
                        me->DealDamage(me, me->GetHealth());
                        me->RemoveCorpse();
                    }
					else
                    {
                        me->DealDamage(me,me->GetHealth());
                    }
                    ActionTimer = 9999999;
                }
				else ActionTimer -= diff;
            }
        }
    };

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action )
    {
        switch(action)
        {
        case GOSSIP_ACTION_INFO_DEF+1:
            CAST_AI(npc_mosswalker_victim::npc_mosswalker_victimAI,creature->AI())->CheckPulse(player);
            break;
        }
        player->CLOSE_GOSSIP_MENU();
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_THE_MOSSWALKER_SAVIOR) == QUEST_STATUS_INCOMPLETE && !CAST_AI(npc_mosswalker_victim::npc_mosswalker_victimAI,creature->AI())->PulseChecked)
           player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_EVENT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mosswalker_victimAI(creature);
    }
};

enum Artruis
{
    SPELL_FROSTBOLT         = 15530,
    SPELL_ICE_LANCE         = 54261,
    SPELL_ICY_VEINS         = 54792,
    SPELL_FROST_NOVA        = 11831,
    SPELL_BINDINGS          = 52185,
    SPELL_SUMMON_PHYLACTERY = 52518,

    GO_ARTRUIS_PHYLACTERY   = 190777,

    NPC_ZEPIK               = 28668,
    NPC_JALOOT              = 28667,

    EVENT_FROSTBOLT         = 1,
    EVENT_ICE_LANCE         = 2,
    EVENT_ICY_VEINS         = 3,
    EVENT_FROST_NOVA        = 4,

    ACTION_UNBIND           = 0,
    ACTION_BIND             = 1,
    ACTION_SET_QUESTGIVER   = 2,
    ACTION_RESET            = 3,

    SAY_AGGRO               = 0,
    SAY_80PERC              = 1,
    SAY_60PERC              = 2,
    SAY_30PERC              = 3,
    EMOTE_SHIELDED          = 4,
    SAY_5PERC               = 5,
};

class npc_artruis_the_heartless : public CreatureScript
{
    public:
        npc_artruis_the_heartless() : CreatureScript("npc_artruis_the_heartless") {}

        struct npc_artruis_the_heartlessAI : public QuantumBasicAI
        {
            npc_artruis_the_heartlessAI(Creature* creature) : QuantumBasicAI(creature) {}

            EventMap events;

            void Reset()
            {
                events.Reset();

                WasImmune = false;
                IsBound = false;

                TalkPhase = 0;

                me->SetReactState(REACT_DEFENSIVE);

                // set event into a sane state, cannot start without zepik&jaloot, despawn phylactery
                if (Creature* zepik = me->FindCreatureWithDistance(NPC_ZEPIK, 60.0f, false))
                    zepik->Respawn();
                else if (Creature* zepik = me->FindCreatureWithDistance(NPC_ZEPIK, 60.0f, true))
                    zepik->GetAI()->Reset();

                if (Creature* jaloot = me->FindCreatureWithDistance(NPC_JALOOT, 60.0f, false))
                    jaloot->Respawn();
                else if (Creature* jaloot = me->FindCreatureWithDistance(NPC_JALOOT, 60.0f, true))
                    jaloot->GetAI()->Reset();

                //if (GameObject* phylactery = me->FindGameobjectWithDistance(GO_ARTRUIS_PHYLACTERY, 60.0f))
                //    phylactery->Delete();
            }

            void EnterToBattle(Unit* /*who*/)
            {
                events.ScheduleEvent(EVENT_FROSTBOLT, 1000);
                events.ScheduleEvent(EVENT_ICE_LANCE, 200);
                events.ScheduleEvent(EVENT_FROST_NOVA, 100);
                events.ScheduleEvent(EVENT_ICY_VEINS, 4500);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (me->GetVictim()->GetTypeId() != TYPE_ID_PLAYER)
				{
                    EnterEvadeMode();
					return;
				}

                // dialog
                if (TalkPhase == SAY_AGGRO)
                {
                    Talk(SAY_AGGRO);
                    TalkPhase++;
                }
                else if (me->GetHealthPct() <= 80.0f && TalkPhase++ == SAY_80PERC)
                    Talk(SAY_80PERC);
                else if (me->GetHealthPct() <= 60.0f && TalkPhase++ == SAY_60PERC)
                    Talk(SAY_60PERC);
                else if (me->GetHealthPct() <= 5.0f && TalkPhase++ == SAY_5PERC)
                    Talk(SAY_5PERC);

                // Immune-Choosing-Phase
                if (me->GetHealthPct() <= 30 && !WasImmune)
                {
                    me->InterruptNonMeleeSpells(true);
                    DoCastVictim(SPELL_BINDINGS); // has implicit targeting

                    Talk(SAY_30PERC);
                    Talk(EMOTE_SHIELDED);
                    TalkPhase += 2;

                    // workaround for SpellHit not being called for implicit targeting
                    if (Creature* zepik = me->FindCreatureWithDistance(NPC_ZEPIK, 60.0f, true))
                        zepik->GetAI()->DoAction(ACTION_BIND);

                    if (Creature* jaloot = me->FindCreatureWithDistance(NPC_JALOOT, 60.0f, true))
                        jaloot->GetAI()->DoAction(ACTION_BIND);

                    WasImmune = true;
                    IsBound = true;
                }

                events.Update(diff);

                if (IsBound || me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                    switch (eventId)
                    {
                        case EVENT_FROSTBOLT:
                            DoCastVictim(SPELL_FROSTBOLT);
                            events.ScheduleEvent(EVENT_FROSTBOLT, 4000);
                            break;
                        case EVENT_ICE_LANCE:
                            DoCastVictim(SPELL_ICE_LANCE);
                            events.ScheduleEvent(EVENT_ICE_LANCE, 7000);
                            break;
                        case EVENT_FROST_NOVA:
                            DoCastVictim(SPELL_FROST_NOVA);
                            events.ScheduleEvent(EVENT_FROST_NOVA, 14000);
                            break;
                        case EVENT_ICY_VEINS:
                            DoCast(me, SPELL_ICY_VEINS);
                            events.ScheduleEvent(EVENT_ICY_VEINS, 25000);
                            break;
                    }
            }

            void JustDied(Unit* /*killer*/)
            {
                // DoCastVictim(SPELL_SUMMON_PHYLACTERY);
                // stupid spell, manual spawn for new, screw you!

                // enable quests for the chosen npc
                if (Creature* zepik = me->FindCreatureWithDistance(NPC_ZEPIK, 120.0f, true))
                    if (zepik->IsAlive())
                        zepik->GetAI()->DoAction(ACTION_SET_QUESTGIVER);

                if (Creature* jaloot = me->FindCreatureWithDistance(NPC_JALOOT, 120.0f, true))
                    if (jaloot->IsAlive())
                        jaloot->GetAI()->DoAction(ACTION_SET_QUESTGIVER);
            }

            void DoAction(const int32 actionId)
            {
                if (actionId == ACTION_UNBIND)
                {
                    me->RemoveAurasDueToSpell(SPELL_BINDINGS);
                    IsBound = false;
                }
            }

        private:
            bool WasImmune;
            bool IsBound;
            uint8 TalkPhase;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_artruis_the_heartlessAI(creature);
        }
};

enum ZepikJaloot
{
    SPELL_TOMB_OF_HEARTLESS = 52182,
    // Zepik
    SPELL_OPEN_WOUND        = 52873,
    SPELL_SPIKE_TRAP        = 52886,
    // Jaloot
    SPELL_LIGHTNING_WHIRL   = 52943,
    SPELL_SPARK_FRENZY      = 52964,

    NPC_ARTRUIS             = 28659,

    EVENT_OPEN_WOUND        = 1,
    EVENT_SPIKE_TRAP        = 2,
    EVENT_LIGHTNING_WHIRL   = 3,
    EVENT_SPARK_FRENZY      = 4,

    FACTION_FRIENDLY        = 250,
    FACTION_MONSTER         = 14,

    SAY_FREED               = 0,
};

class npc_zepik_jaloot : public CreatureScript
{
    public:
    npc_zepik_jaloot() : CreatureScript("npc_zepik_jaloot") {}

        struct npc_zepik_jalootAI : public QuantumBasicAI
        {
            npc_zepik_jalootAI(Creature* creature) : QuantumBasicAI(creature) {}

            EventMap events;

            void Reset()
            {
                events.Reset();
                me->SetCurrentFaction(FACTION_FRIENDLY);

                switch (me->GetEntry())
                {
                    case NPC_ZEPIK:
                        events.ScheduleEvent(EVENT_OPEN_WOUND, 2000);
                        events.ScheduleEvent(EVENT_SPIKE_TRAP, 4000);
                        break;
                    case NPC_JALOOT:
                        events.ScheduleEvent(EVENT_LIGHTNING_WHIRL, 2000);
                        events.ScheduleEvent(EVENT_SPARK_FRENZY, 4000);
                        break;
                }

                me->RemoveAurasDueToSpell(SPELL_BINDINGS);

                me->GetMotionMaster()->MoveTargetedHome();
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (!me->HasAura(SPELL_BINDINGS))
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                    switch (eventId)
                    {
                        case EVENT_OPEN_WOUND:
                            DoCastVictim(SPELL_OPEN_WOUND);
                            events.ScheduleEvent(EVENT_OPEN_WOUND, 4000);
                            break;
                        case EVENT_SPIKE_TRAP:
                            DoCastVictim(SPELL_SPIKE_TRAP);
                            events.ScheduleEvent(EVENT_SPIKE_TRAP, 4000);
                            break;
                        case EVENT_LIGHTNING_WHIRL:
                            DoCastVictim(SPELL_LIGHTNING_WHIRL);
                            events.ScheduleEvent(EVENT_LIGHTNING_WHIRL, 4000);
                            break;
                        case EVENT_SPARK_FRENZY:
                            DoCastVictim(SPELL_SPARK_FRENZY);
                            events.ScheduleEvent(EVENT_SPARK_FRENZY, 14000);
                            break;
                    }
            }

            // Spell: Bindings of Submission target Zepik&Jaloot implicitly, but don't turn up in SpellHit(), makes developer sad ;-(
            void SpellHit(Unit* who, const SpellEntry* spell)
            {
                // Artruis chains Zepik&Jaloot, removes Tomb of Heartless, both attack player
                if (spell->Id == SPELL_BINDINGS)
                {
                    me->RemoveAurasDueToSpell(SPELL_TOMB_OF_HEARTLESS);
                    if (Creature* artruis = me->FindCreatureWithDistance(NPC_ARTRUIS, 120.0f, true))
                    {
                        me->SetCurrentFaction(FACTION_MONSTER);
                        me->Attack(artruis->GetVictim(), true);
                        me->GetMotionMaster()->MoveChase(artruis->GetVictim());
                    }
                }
            }

            void JustDied(Unit* /*killer*/)
            {
                if (Creature* artruis = me->FindCreatureWithDistance(NPC_ARTRUIS, 120.0f, true))
                {
                    // determine players choice
                    uint32 assistantEntry = (me->GetEntry() == NPC_ZEPIK) ? NPC_JALOOT : NPC_ZEPIK;

                    // and start assisting
                    if (Creature* assistant = me->FindCreatureWithDistance(assistantEntry, 120.0f, true))
                    {
                        artruis->GetAI()->DoAction(ACTION_UNBIND); // do this first, because otherwise he is immune
                        assistant->RemoveAurasDueToSpell(SPELL_BINDINGS);

                        assistant->AddThreat(artruis, 25000.0f);
                        assistant->GetMotionMaster()->MoveChase(artruis);
                        assistant->SetCurrentFaction(FACTION_FRIENDLY);
                        assistant->Attack(artruis, true);

                        Talk(SAY_FREED);
                    }
                }
            }

            void JustReachedHome()
            {
                me->DeleteThreatList();

                if (Creature* artruis = me->FindCreatureWithDistance(NPC_ARTRUIS, 120.0f, false))
                    return; // this is really important, if reset is not blocked they will ice block and neither give quest nor assist

                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUEST_GIVER);

                //DoCast(me, SPELL_TOMB_OF_HEARTLESS, false);
                me->AddAura(SPELL_TOMB_OF_HEARTLESS, me);
            }

            void DoAction(const int32 actionId)
            {
                if (actionId == ACTION_SET_QUESTGIVER)
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUEST_GIVER);

                // replacement for SpellHit-Method
                else if (actionId == ACTION_BIND)
                {
                    if (Creature* artruis = me->FindCreatureWithDistance(NPC_ARTRUIS, 120.0f, true))
                    {
                        me->SetCurrentFaction(FACTION_MONSTER);
                        me->Attack(artruis->GetVictim(), true);
                        me->GetMotionMaster()->MoveChase(artruis->GetVictim());
                    }
                    me->RemoveAurasDueToSpell(SPELL_TOMB_OF_HEARTLESS);
                }

                else if (actionId == ACTION_RESET)
					Reset();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_zepik_jalootAI(creature);
        }
};

/*#####
## npc_jungle_punch_target
#####*/

#define SAY_OFFER     "Care to try Grimbooze Thunderbrew's new jungle punch?"
#define SAY_HEMET_1   "Aye, I'll try it."
#define SAY_HEMET_2   "That's exactly what I needed!"
#define SAY_HEMET_3   "It's got my vote! That'll put hair on your chest like nothing else will."
#define SAY_HADRIUS_1 "I'm always up for something of Grimbooze's."
#define SAY_HADRIUS_2 "Well, so far, it tastes like something my wife would drink..."
#define SAY_HADRIUS_3 "Now, there's the kick I've come to expect from Grimbooze's drinks! I like it!"
#define SAY_TAMARA_1  "Sure!"
#define SAY_TAMARA_2  "Oh my..."
#define SAY_TAMARA_3  "Tastes like I'm drinking... engine degreaser!"

enum utils
{
    NPC_HEMET       = 27986,
    NPC_HADRIUS     = 28047,
    NPC_TAMARA      = 28568,
    SPELL_OFFER     = 51962,
    QUEST_ENTRY     = 12645,
};

enum NesingwaryChildrensWeek
{
    SPELL_ORPHAN_OUT                  = 58818,
    QUEST_THE_MIGHTY_HEMET_NESINGWARY = 13957,
    ORPHAN_WOLVAR                     = 33532,
    TEXT_WOLVAR_ORPHAN_6                = 6,
    TEXT_WOLVAR_ORPHAN_7                = 7,
    TEXT_WOLVAR_ORPHAN_8                = 8,
    TEXT_WOLVAR_ORPHAN_9                = 9,
    TEXT_NESINGWARY_1                   = 1,
};

class npc_jungle_punch_target : public CreatureScript
{
public:
    npc_jungle_punch_target() : CreatureScript("npc_jungle_punch_target") { }

    struct npc_jungle_punch_targetAI : public QuantumBasicAI
    {
        npc_jungle_punch_targetAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint16 sayTimer;
		uint8 sayStep;
		uint32 timer;
		int8 phase;
		uint64 playerGUID;
		uint64 orphanGUID;

        void Reset()
        {
            sayTimer = 3500;
            sayStep = 0;
            timer = 0;
            phase = 0;
            playerGUID = 0;
            orphanGUID = 0;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!phase && who && who->GetDistance2d(me) < 10.0f)
			{
                if (Player* player = who->ToPlayer())
				{
                    if (player->GetQuestStatus(QUEST_THE_MIGHTY_HEMET_NESINGWARY) == QUEST_STATUS_INCOMPLETE)
                    {
                        playerGUID = player->GetGUID();
                        if (Aura* orphanOut = player->GetAura(SPELL_ORPHAN_OUT))
						{
                            if (orphanOut->GetCaster() && orphanOut->GetCaster()->GetEntry() == ORPHAN_WOLVAR)
                            {
                                orphanGUID = orphanOut->GetCaster()->GetGUID();
                                phase = 1;
                            }
						}
                    }
				}
			}
        }

        void proceedCwEvent(const uint32 diff)
        {
            if (timer <= diff)
            {
                Player* player = Player::GetPlayer(*me, playerGUID);
                Creature* orphan = Creature::GetCreature(*me, orphanGUID);

                if (!orphan || !player)
                {
                    Reset();
                    return;
                }

                switch(phase)
                {
                    case 1:
                        orphan->GetMotionMaster()->MovePoint(0, me->GetPositionX() + cos(me->GetOrientation()) * 5, me->GetPositionY() + sin(me->GetOrientation()) * 5, me->GetPositionZ());
                        orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_6);
                        timer = 5000;
                        break;
                    case 2:
                        orphan->SetFacingToObject(me);
                        orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_7);
                        timer = 5000;
                        break;
                    case 3:
                        Talk(TEXT_NESINGWARY_1);
                        timer = 5000;
                        break;
                    case 4:
                        orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_8);
                        timer = 5000;
                        break;
                    case 5:
                        orphan->AI()->Talk(TEXT_WOLVAR_ORPHAN_9);
                        timer = 5000;
                        break;
                    case 6:
                        orphan->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                        player->GroupEventHappens(QUEST_THE_MIGHTY_HEMET_NESINGWARY, me);
                        Reset();
                        return;
                }
                ++phase;
            }
            else
                timer -= diff;
        }

        void UpdateAI(const uint32 diff)
        {
            if (phase)
                proceedCwEvent(diff);

            if (!sayStep)
                return;

            if (sayTimer < diff)
            {
                switch (sayStep)
                {
                    case 0:
                    {
                        switch (me->GetEntry())
                        {
                            case NPC_HEMET:
								me->MonsterSay(SAY_HEMET_1, LANG_UNIVERSAL, 0);
								break;
                            case NPC_HADRIUS:
								me->MonsterSay(SAY_HADRIUS_1, LANG_UNIVERSAL, 0);
								break;
                            case NPC_TAMARA:
								me->MonsterSay(SAY_TAMARA_1, LANG_UNIVERSAL, 0);
								break;
                        }
                        sayTimer = 3000;
                        sayStep++;
                        break;
                    }
                    case 1:
                    {
                        switch (me->GetEntry())
                        {
                            case NPC_HEMET:
								me->MonsterSay(SAY_HEMET_2, LANG_UNIVERSAL, 0);
								break;
                            case NPC_HADRIUS:
								me->MonsterSay(SAY_HADRIUS_2, LANG_UNIVERSAL, 0);
								break;
                            case NPC_TAMARA:
								me->MonsterSay(SAY_TAMARA_2, LANG_UNIVERSAL, 0);
								break;
                        }
                        sayTimer = 3000;
                        sayStep++;
                        break;
                    }
                    case 2:
                    {
                        switch (me->GetEntry())
                        {
                            case NPC_HEMET:
								me->MonsterSay(SAY_HEMET_3, LANG_UNIVERSAL, 0);
								break;
                            case NPC_HADRIUS:
								me->MonsterSay(SAY_HADRIUS_3, LANG_UNIVERSAL, 0);
								break;
                            case NPC_TAMARA:
								me->MonsterSay(SAY_TAMARA_3, LANG_UNIVERSAL, 0);
								break;
                        }
                        sayTimer = 3000;
                        sayStep = 0;
                        break;
                    }
                }
            }
            else
                sayTimer -= diff;
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (!spell || spell->Id != SPELL_OFFER)
                return;

            if (!caster->ToPlayer())
                return;

            QuestStatusMap::const_iterator itr = caster->ToPlayer()->getQuestStatusMap().find(QUEST_ENTRY);
            if (itr->second.Status != QUEST_STATUS_INCOMPLETE)
                return;

            for (uint8 i=0; i<3; i++)
            {
                switch (i)
                {
                   case 0:
                       if (NPC_HEMET != me->GetEntry())
                           continue;
                       else
                           break;
                   case 1:
                       if (NPC_HADRIUS != me->GetEntry())
                           continue;
                       else
                           break;
                   case 2:
                       if (NPC_TAMARA != me->GetEntry())
                           continue;
                       else
                           break;
                }

                if (itr->second.CreatureOrGOCount[i] != 0)
                    continue;

                caster->ToPlayer()->KilledMonsterCredit(me->GetEntry(), 0);
                caster->ToPlayer()->Say(SAY_OFFER, LANG_UNIVERSAL);
                sayStep = 0;
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jungle_punch_targetAI(creature);
    }
};

/*######
## npc_adventurous_dwarf
######*/

#define GOSSIP_OPTION_ORANGE  "Can you spare an orange?"
#define GOSSIP_OPTION_BANANAS "Have a spare bunch of bananas?"
#define GOSSIP_OPTION_PAPAYA  "I could really use a papaya."

enum AdventurousDwarf
{
    QUEST_12634         = 12634,

    ITEM_BANANAS        = 38653,
    ITEM_PAPAYA         = 38655,
    ITEM_ORANGE         = 38656,

    SPELL_ADD_ORANGE    = 52073,
    SPELL_ADD_BANANAS   = 52074,
    SPELL_ADD_PAPAYA    = 52076,

    GOSSIP_MENU_DWARF   = 13307,

    SAY_DWARF_OUCH      = -1571042,
    SAY_DWARF_HELP      = -1571043
};

class npc_adventurous_dwarf : public CreatureScript
{
public:
    npc_adventurous_dwarf() : CreatureScript("npc_adventurous_dwarf") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        DoSendQuantumText(SAY_DWARF_OUCH, creature);
        return 0;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_12634) != QUEST_STATUS_INCOMPLETE)
            return false;

        if (player->GetItemCount(ITEM_ORANGE) < 1)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_OPTION_ORANGE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        if (player->GetItemCount(ITEM_BANANAS) < 2)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_OPTION_BANANAS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

        if (player->GetItemCount(ITEM_PAPAYA) < 1)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_OPTION_PAPAYA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

        player->PlayerTalkClass->SendGossipMenu(GOSSIP_MENU_DWARF, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        uint32 spellId = 0;
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
				spellId = SPELL_ADD_ORANGE;
				break;
            case GOSSIP_ACTION_INFO_DEF + 2:
				spellId = SPELL_ADD_BANANAS;
				break;
            case GOSSIP_ACTION_INFO_DEF + 3:
				spellId = SPELL_ADD_PAPAYA;
				break;
        }
        if (spellId)
            player->CastSpell(player, spellId, true);
        DoSendQuantumText(SAY_DWARF_HELP, creature);
        creature->DespawnAfterAction();
        return true;
    }
};

enum TipsyMcManus
{
    DATA_EVENT                 = 1,
    DATA_OBJECT_ENTRY          = 2,
    QUEST_STILL_AT_IT          = 12644,
    GOSSIP_TIPSY_MCMANUS_TEXT  = 13288,
    GO_JUNGLE_PUNCH            = 190643
};

static uint32 const goEntry[5] =
{
    190635,
    190636,
    190637,
    190638,
    190639
};

static char const* Instructions[6] =
{
    "Use the pressure control valve!",
    "Heat up the coal frying pan!",
    "Throw in one more orange, fast!",
    "Mix a few bananas there!",
    "Quick, a papaya!",
    "No, this was wrong! We must begin once again."
};

#define GOSSIP_ITEM_TIPSY  "I am ready, lets us start."

class npc_tipsy_mcmanus : public CreatureScript
{
    public:
        npc_tipsy_mcmanus() : CreatureScript("npc_tipsy_mcmanus") { }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            if (action == GOSSIP_ACTION_INFO_DEF + 1)
            {
                player->CLOSE_GOSSIP_MENU();
                creature->AI()->SetData(DATA_EVENT, 1);
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }
            return true;
        }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (player->GetQuestStatus(QUEST_STILL_AT_IT) == QUEST_STATUS_INCOMPLETE)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TIPSY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

            player->SEND_GOSSIP_MENU(GOSSIP_TIPSY_MCMANUS_TEXT, creature->GetGUID());
            return true;
        }

        struct npc_tipsy_mcmanusAI : public QuantumBasicAI
        {
            npc_tipsy_mcmanusAI(Creature* creature) : QuantumBasicAI(creature) {}

			bool _event;
            bool _choice;
            uint8 _count;
            uint8 _rnd;
            uint32 ReactTimer;

            void Reset()
            {
                _event = false;
                _choice = true;
                _rnd = 0;
                _count = 0;
                ReactTimer = 10000;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void SetData(uint32 id, uint32 data)
            {
                switch (id)
                {
                    case DATA_EVENT:
                        _event = data ? true : false;
                        break;
                    case DATA_OBJECT_ENTRY:
                        if (!_choice && data == goEntry[_rnd])
                        {
                            me->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
                            _choice = true;
                            ReactTimer = urand(5000, 7000);
                        }
                        break;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (UpdateVictim())
                {
                    DoMeleeAttackIfReady();
                    return;
                }

                if (_event) // active
                {
                    if (ReactTimer <= diff)
                    {
                        if (_choice) // used correct object
                        {
                            ++_count;

                            if (_count > 10) // spawn quest reward and reset
                            {
                                float x, y, z;
                                me->GetPosition(x, y, z);
                                me->SummonGameObject(GO_JUNGLE_PUNCH, x + 1.2f, y + 0.8f, z - 0.23f, 0, 0, 0, 0, 0, 60);
                                Reset();
                                return;
                            }

                            _rnd = urand(0, 4);
                            me->MonsterSay(Instructions[_rnd], LANG_UNIVERSAL, 0); // give new instructions
                            me->HandleEmoteCommand(RAND(EMOTE_ONESHOT_EXCLAMATION, EMOTE_ONESHOT_POINT));
                            _choice = false; // reset choice
                        }
                        else // failed -> reset and try again
                        {
                            me->MonsterSay(Instructions[5], LANG_UNIVERSAL, 0);
                            me->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
                            Reset();
                            return;
                        }

                        ReactTimer = 10000;
                    }
                    else ReactTimer -= diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_tipsy_mcmanusAI(creature);
        }
};

enum BrewEvent
{
    NPC_TIPSY_MCMANUS = 28566,
};

class go_brew_event : public GameObjectScript
{
public:
	go_brew_event() : GameObjectScript("go_brew_event") { }

	bool OnGossipHello(Player* /*player*/, GameObject* go)
	{
		if (Creature* Tipsy = go->FindCreatureWithDistance(NPC_TIPSY_MCMANUS, 30.0f, true))
			Tipsy->AI()->SetData(DATA_OBJECT_ENTRY, go->GetEntry());

		return false;
	}
};

enum Stormwatcher
{
    SPELL_CALL_LIGHTNING                = 32018,
    SPELL_THROW_VENTURE_CO_EXPLOSIVES   = 53145,
    SPELL_SUMMON_STORMWATCHERS_HEAD     = 53162,
};

class npc_stormwatcher : public CreatureScript
{
public:
	npc_stormwatcher() : CreatureScript("npc_stormwatcher"){ }

	struct npc_stormwatcherAI : public QuantumBasicAI
	{
		npc_stormwatcherAI(Creature* creature) : QuantumBasicAI (creature){ }

		uint32 CallLightningTimer;

		void Reset()
		{
			CallLightningTimer = 3000;
		}

		void SpellHit (Unit* /*caster*/, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_THROW_VENTURE_CO_EXPLOSIVES)
			{
				DoCast(me, SPELL_SUMMON_STORMWATCHERS_HEAD, true);
				me->DespawnAfterAction();
			}
		}

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;

			if (CallLightningTimer <= diff)
			{
				DoCastVictim(SPELL_CALL_LIGHTNING);
				CallLightningTimer = urand(3000, 5000);
			}
			else CallLightningTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_stormwatcherAI(creature);
	}
};

enum FirstBlood
{
    NPC_SAPPHIRE_HIVE_WASP       = 28086,
    NPC_HARDKNUCKLE_CHARGER      = 28096,
    NPC_MISTWHISPER_ORACLE       = 28110,
    NPC_MISTWHISPER_WARRIOR      = 28109,
    // Kill Credit Entries from quest_template
    NPC_CREDIT_1                 = 28040,
    NPC_CREDIT_2                 = 36189,
    NPC_CREDIT_3                 = 29043,

    SPELL_BLOOD_REJEKS_SWORD     = 52992,
    SPELL_WASP_STINGER_RAGE      = 34392,
    SPELL_CHARGER_CHARGE         = 49758,
    SPELL_ORACLE_LIGHTNING_CLOUD = 54921,
    SPELL_WARRIOR_FLIP_ATTACK    = 50533,
};

class npc_rejek_first_blood : public CreatureScript
{
    public:
        npc_rejek_first_blood() : CreatureScript("npc_rejek_first_blood"){ }

        struct npc_rejek_first_bloodAI : public QuantumBasicAI
        {
            npc_rejek_first_bloodAI(Creature* creature) : QuantumBasicAI (creature){ }

            uint32 FlipAttackTimer;
            uint32 ChargeTimer;
            
            bool Frenzied;

            void Reset()
            {
                FlipAttackTimer = urand (2000, 6000);
                ChargeTimer = 0;
            }

            void EnterToBattle (Unit* /*who*/)
            {
                Frenzied = false;

                if (me->GetEntry() == NPC_MISTWHISPER_ORACLE)
                    DoCast(me, SPELL_ORACLE_LIGHTNING_CLOUD, true);
            }

            void SpellHit(Unit* caster, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_BLOOD_REJEKS_SWORD)
                {
                    if (caster->ToPlayer())
                    {
                        switch(me->GetEntry())
                        {
                            case NPC_SAPPHIRE_HIVE_WASP:
                                caster->ToPlayer()->KilledMonsterCredit(NPC_CREDIT_1, 0);
                                break;
                            case NPC_HARDKNUCKLE_CHARGER:
                                caster->ToPlayer()->KilledMonsterCredit(NPC_CREDIT_2, 0);
                                break;
                            case NPC_MISTWHISPER_ORACLE:
                            case NPC_MISTWHISPER_WARRIOR:
                                caster->ToPlayer()->KilledMonsterCredit(NPC_CREDIT_3, 0);
                                break;
                        }
                    }
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (me->GetEntry() == NPC_SAPPHIRE_HIVE_WASP)
                {
                    if (!Frenzied && HealthBelowPct(30))
                    {
                        DoCast(me, SPELL_WASP_STINGER_RAGE, true);
                        Frenzied = true;
                    }
                }

                if (me->GetEntry() == NPC_HARDKNUCKLE_CHARGER)
                {
                    if (ChargeTimer <= diff)
                    {
                        DoCastVictim(SPELL_CHARGER_CHARGE);
                        ChargeTimer = 5000;
                    }
                    else ChargeTimer -= diff;
				}

                if (me->GetEntry() == NPC_MISTWHISPER_WARRIOR)
                {
                    if (FlipAttackTimer <= diff)
                    {
                        DoCastVictim(SPELL_WARRIOR_FLIP_ATTACK);
                        FlipAttackTimer = urand(4000, 7000);
                    }
                    else FlipAttackTimer -= diff;
                }

                DoMeleeAttackIfReady();
			}
		};

		CreatureAI* GetAI(Creature* creature) const
        {
			return new npc_rejek_first_bloodAI(creature);
		}
};

enum Haiphoon
{
    SPELL_DEVOUR_WIND   = 52862,
    SPELL_DEVOUR_WATER  = 52864,
    NPC_HAIPHOON_WATER  = 28999,
    NPC_HAIPHOON_AIR    = 28985,
	NPC_REVENANT_CREDIT = 29008,
};

class vehicle_haiphoon : public CreatureScript
{
public:
    vehicle_haiphoon() : CreatureScript("vehicle_haiphoon") { }

    struct vehicle_haiphoonAI : public VehicleAI
    {
        vehicle_haiphoonAI(Creature* creature) : VehicleAI(creature) { }

        void SpellHitTarget(Unit* /*caster*/, SpellInfo const* spell)
        {
			if (spell->Id == SPELL_DEVOUR_WIND)
            {
                if (Player* player = me->GetCharmerOrOwnerPlayerOrPlayerItself())
                {
                    player->KilledMonsterCredit(NPC_REVENANT_CREDIT, 0);
                    me->UpdateEntry(NPC_HAIPHOON_AIR);                    
                    player->VehicleSpellInitialize();
                    me->SetCurrentFaction(player->GetFaction());
                }
            }

            if (spell->Id == SPELL_DEVOUR_WATER)
            {
                if (Player* player = me->GetCharmerOrOwnerPlayerOrPlayerItself())
                {
                    player->KilledMonsterCredit(NPC_REVENANT_CREDIT, 0);                    
                    me->UpdateEntry(NPC_HAIPHOON_WATER);
                    player->VehicleSpellInitialize();
                    me->SetCurrentFaction(player->GetFaction());
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new vehicle_haiphoonAI(creature);
    }
};

enum ElderHarkek_Misc
{
    ITEM_GOREGEKS_SHACKLES              = 38619,
    ITEM_DAJIKS_WORN_CHALK              = 38621,
    ITEM_ZEPIKS_HUNTING_HORN            = 38512,
        
    SPELL_FORCEITEM_GOREGEK             = 52542,
    SPELL_FORCEITEM_DAJIK               = 52544,
    SPELL_FORCEITEM_ZEPIK               = 52545,

    QUEST_PLAYING_ALONG                 = 12528,
    QUEST_THE_WHASP_HUNTERS_APPRENTICE  = 12533,
    QUEST_ROUGH_RIDE                    = 12536,
};

#define GOSSIP_GOREGEK_ITEM "I need to find Goregek, do you have his shackles?"
#define GOSSIP_DAJIK_ITEM   "I need to find Dajik, do you have his chalk?"
#define GOSSIP_ZEPIK_ITEM   "I need to find Zepik, do you have his horn?"

class npc_elder_harkek : public CreatureScript
{
    public:
        npc_elder_harkek() : CreatureScript("npc_elder_harkek") { }

        bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
        {
            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF+0:
                    player->CastSpell(player, SPELL_FORCEITEM_GOREGEK, true);
                    player->CLOSE_GOSSIP_MENU();
                    break;
                case GOSSIP_ACTION_INFO_DEF+1:
                    player->CastSpell(player, SPELL_FORCEITEM_DAJIK, true);
                    player->CLOSE_GOSSIP_MENU();
                    break;
                case GOSSIP_ACTION_INFO_DEF+2:
                    player->CastSpell(player, SPELL_FORCEITEM_ZEPIK, true);
                    player->CLOSE_GOSSIP_MENU();
                    break;
            }
			return true;
        }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            if (player->GetQuestStatus(QUEST_PLAYING_ALONG) == QUEST_STATUS_REWARDED
                && !player->HasItemCount(ITEM_GOREGEKS_SHACKLES, 1, true))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_GOREGEK_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+0);

            if ((player->GetQuestStatus(QUEST_THE_WHASP_HUNTERS_APPRENTICE) == QUEST_STATUS_INCOMPLETE 
                || player->GetQuestStatus(QUEST_THE_WHASP_HUNTERS_APPRENTICE) == QUEST_STATUS_REWARDED)
                && !player->HasItemCount(ITEM_DAJIKS_WORN_CHALK, 1, true))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_DAJIK_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

            if ((player->GetQuestStatus(QUEST_ROUGH_RIDE) == QUEST_STATUS_INCOMPLETE 
                || player->GetQuestStatus(QUEST_ROUGH_RIDE) == QUEST_STATUS_COMPLETE 
                || player->GetQuestStatus(QUEST_ROUGH_RIDE) == QUEST_STATUS_REWARDED)
                && !player->HasItemCount(ITEM_ZEPIKS_HUNTING_HORN, 1, true))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ZEPIK_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

			player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature),creature->GetGUID());
            return true;
        }
};

enum MiscLifewarden
{
    NPC_PRESENCE              = 28563, // Freya's Presence
    NPC_SABOTEUR              = 28538, // Cultist Saboteur
    NPC_SERVANT               = 28320, // Servant of Freya
    WHISPER_ACTIVATE          = 0,
    SPELL_FREYA_DUMMY         = 51318,
    SPELL_LIFEFORCE           = 51395,
    SPELL_FREYA_DUMMY_TRIGGER = 51335,
    SPELL_LASHER_EMERGE       = 48195,
    SPELL_WILD_GROWTH         = 52948,
};

class spell_q12620_the_lifewarden_wrath : public SpellScriptLoader
{
public:
    spell_q12620_the_lifewarden_wrath() : SpellScriptLoader("spell_q12620_the_lifewarden_wrath") { }

    class spell_q12620_the_lifewarden_wrath_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q12620_the_lifewarden_wrath_SpellScript);

        void HandleSendEvent(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);

            if (Unit* caster = GetCaster())
            {
                if (Creature* presence = caster->FindCreatureWithDistance(NPC_PRESENCE, 50.0f))
                {
                    presence->AI()->Talk(WHISPER_ACTIVATE, caster->GetGUID());
                    presence->CastSpell(presence, SPELL_FREYA_DUMMY, true); // will target plants
                    // Freya Dummy could be scripted with the following code

                    // Revive plants
                    std::list<Creature*> servants;
                    GetCaster()->GetCreatureListWithEntryInGrid(servants, NPC_SERVANT, 200.0f);
                    for (std::list<Creature*>::iterator itr = servants.begin(); itr != servants.end(); ++itr)
                    {
                        // Couldn't find a spell that does this
                        if ((*itr)->IsDead())
                            (*itr)->Respawn(true);

                        (*itr)->CastSpell(*itr, SPELL_FREYA_DUMMY_TRIGGER, true);
                        (*itr)->CastSpell(*itr, SPELL_LASHER_EMERGE, false);
                        (*itr)->CastSpell(*itr, SPELL_WILD_GROWTH, false);

                        if (Unit* target = (*itr)->SelectNearestTarget(150.0f))
                            (*itr)->AI()->AttackStart(target);
                    }

                    // Kill nearby enemies
                    std::list<Creature*> saboteurs;
                    caster->GetCreatureListWithEntryInGrid(saboteurs, NPC_SABOTEUR, 200.0f);
                    for (std::list<Creature*>::iterator itr = saboteurs.begin(); itr != saboteurs.end(); ++itr)
                        if ((*itr)->IsAlive())
                            // Lifeforce has a cast duration, it should be cast at all saboteurs one by one
                            presence->CastSpell((*itr), SPELL_LIFEFORCE, false);
                }
            }
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_q12620_the_lifewarden_wrath_SpellScript::HandleSendEvent, EFFECT_0, SPELL_EFFECT_SEND_EVENT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q12620_the_lifewarden_wrath_SpellScript();
    }
};

enum KickWhatKick
{
    NPC_LUCKY_WILHELM      = 28054,
    NPC_APPLE              = 28053,
    NPC_DROSTAN            = 28328,
    NPC_CRUNCHY            = 28346,
    NPC_THICKBIRD          = 28093,

    SPELL_HIT_APPLE        = 51331,
    SPELL_MISS_APPLE       = 51332,
    SPELL_MISS_BIRD_APPLE  = 51366,
    SPELL_APPLE_FALL       = 51371,
    SPELL_BIRD_FALL        = 51369,

    EVENT_MISS             = 0,
    EVENT_HIT              = 1,
    EVENT_MISS_BIRD        = 2,

    SAY_WILHELM_MISS       = 0,
    SAY_WILHELM_HIT        = 1,
    SAY_DROSTAN_REPLY_MISS = 0,
};

class spell_q12589_shoot_rjr : public SpellScriptLoader
{
public:
    spell_q12589_shoot_rjr() : SpellScriptLoader("spell_q12589_shoot_rjr") { }

    class spell_q12589_shoot_rjr_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q12589_shoot_rjr_SpellScript);

        SpellCastResult CheckCast()
        {
            if (Unit* target = GetExplTargetUnit())
                if (target->GetEntry() == NPC_LUCKY_WILHELM)
                    return SPELL_CAST_OK;

            SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_TARGET_WILHELM);
            return SPELL_FAILED_CUSTOM_ERROR;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            uint32 roll = urand(1, 100);

            uint8 ev;
            if (roll <= 50)
                ev = EVENT_MISS;
            else if (roll <= 83)
                ev = EVENT_HIT;
            else
                ev = EVENT_MISS_BIRD;

            Unit* shooter = GetCaster();
            Creature* wilhelm = GetHitUnit()->ToCreature();
            Creature* apple = shooter->FindCreatureWithDistance(NPC_APPLE, 30);
            Creature* drostan = shooter->FindCreatureWithDistance(NPC_DROSTAN, 30);

            if (!wilhelm || !apple || !drostan)
                return;

            switch (ev)
            {
                case EVENT_MISS_BIRD:
                {
                    Creature* crunchy = shooter->FindCreatureWithDistance(NPC_CRUNCHY, 30);
                    Creature* bird = shooter->FindCreatureWithDistance(NPC_THICKBIRD, 30);

                    if (!bird || !crunchy); // fall to EVENT_MISS
                    else
                    {
                        shooter->CastSpell(bird, SPELL_MISS_BIRD_APPLE);
                        bird->CastSpell(bird, SPELL_BIRD_FALL);
                        wilhelm->AI()->Talk(SAY_WILHELM_MISS);
                        drostan->AI()->Talk(SAY_DROSTAN_REPLY_MISS);

                        bird->Kill(bird);
                        crunchy->GetMotionMaster()->MovePoint(0, bird->GetPositionX(), bird->GetPositionY(),
                            bird->GetMap()->GetWaterOrGroundLevel(bird->GetPositionX(), bird->GetPositionY(), bird->GetPositionZ()));
                        // TODO: Make crunchy perform emote eat when he reaches the bird

                        break;
                    }
                }
                case EVENT_MISS:
                {
                    shooter->CastSpell(wilhelm, SPELL_MISS_APPLE);
                    wilhelm->AI()->Talk(SAY_WILHELM_MISS);
                    drostan->AI()->Talk(SAY_DROSTAN_REPLY_MISS);
                    break;
                }
                case EVENT_HIT:
                {
                    shooter->CastSpell(apple, SPELL_HIT_APPLE);
                    apple->CastSpell(apple, SPELL_APPLE_FALL);
                    wilhelm->AI()->Talk(SAY_WILHELM_HIT);
                    if (Player* player = shooter->ToPlayer())
                        player->KilledMonsterCredit(NPC_APPLE, 0);
                    apple->DespawnAfterAction();

                    break;
                }
            }
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_q12589_shoot_rjr_SpellScript::CheckCast);
            OnEffectHitTarget += SpellEffectFn(spell_q12589_shoot_rjr_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q12589_shoot_rjr_SpellScript();
    }
};

enum ReconnaissanceFlight
{
	SPELL_VICS_PLANE_DUMMY   = 52199,
	SPELL_FLIGHT             = 52211,
	SPELL_SUMMON_VIC_MACHINE = 52218,
	SPELL_AURA_ENGINE        = 52255,
    SPELL_LAND               = 52226,
	SPELL_VICS_MACHINE_AGGRO = 52260,
    SPELL_CREDIT             = 53328,

    NPC_PLANE                = 28710,
    NPC_PILOT                = 28646,
	QUEST_RECON_FLIGHT       = 12671,

    VIC_SAY_0                = 0,
    VIC_SAY_1                = 1,
    VIC_SAY_2                = 2,
    VIC_SAY_3                = 3,
    VIC_SAY_4                = 4,
    VIC_SAY_5                = 5,
    VIC_SAY_6                = 6,
    PLANE_EMOTE              = 0,
};

class npc_vics_flying_machine : public CreatureScript
{
public:
    npc_vics_flying_machine() : CreatureScript("npc_vics_flying_machine") { }

    struct npc_vics_flying_machineAI : public VehicleAI
    {
        npc_vics_flying_machineAI(Creature* creature) : VehicleAI(creature)
		{
			DoCast(me, SPELL_FLIGHT, true);
			DoCast(me, SPELL_VICS_MACHINE_AGGRO, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}
		
		void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply)
        {
            if (apply && passenger->ToPlayer())
                me->GetMotionMaster()->MovePath(NPC_PLANE, false);
        }

        void MovementInform(uint32 type, uint32 id)
        {
			if (type != WAYPOINT_MOTION_TYPE)
				return;

            if (Creature* pilot = GetClosestCreatureWithEntry(me, NPC_PILOT, 10))
			{
                switch (id)
                {
                    case 5:
                        pilot->AI()->Talk(VIC_SAY_0);
                        break;
                    case 11:
                        pilot->AI()->Talk(VIC_SAY_1);
                        break;
                    case 12:
                        pilot->AI()->Talk(VIC_SAY_2);
                        break;
                    case 14:
                        pilot->AI()->Talk(VIC_SAY_3);
                        break;
                    case 15:
                        pilot->AI()->Talk(VIC_SAY_4);
                        break;
                    case 17:
                        pilot->AI()->Talk(VIC_SAY_5);
                        break;
                    case 21:
                        pilot->AI()->Talk(VIC_SAY_6);
                        break;
                    case 25:
						Talk(PLANE_EMOTE);
						DoCast(SPELL_AURA_ENGINE);
						if (Player* player = me->FindPlayerWithDistance(30.0f))
							player->CastSpell(player, SPELL_CREDIT, true);
                        break;
                }
			}
        }

        void UpdateAI(const uint32 /*diff*/) {}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vics_flying_machineAI(creature);
    }
};

class npc_pilot_vic : public CreatureScript
{
public:
    npc_pilot_vic() : CreatureScript("npc_pilot_vic") {}

	bool OnQuestAccept(Player* player, Creature* /*creature*/, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_RECON_FLIGHT)
			player->CastSpell(player, SPELL_SUMMON_VIC_MACHINE, true);

		return true;
	}

    struct npc_pilot_vicAI : public QuantumBasicAI
    {
        npc_pilot_vicAI(Creature* creature) : QuantumBasicAI(creature){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_pilot_vicAI(creature);
    }
};

enum ElementalSpells
{
	NPC_LIFEBLOOD_KILL_CREDIT = 29303,

	SPELL_LIFEBLOOD_SIPHON    = 54191,
	SPELL_LIFEBLOOD_DUMMY     = 54190,
	SPELL_RED_RADIATION       = 54189,
	SPELL_VERTEX_COLOR_PINK   = 53213,
	SPELL_BLOOD_TAP           = 54790,
};

class npc_lifeblood_elemental : public CreatureScript
{
public:
    npc_lifeblood_elemental() : CreatureScript("npc_lifeblood_elemental") {}

    struct npc_lifeblood_elementalAI : public QuantumBasicAI
    {
        npc_lifeblood_elementalAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BloodTapTimer;

        void Reset()
		{
			BloodTapTimer = 1000;

			DoCast(me, SPELL_VERTEX_COLOR_PINK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_RED_RADIATION)
			{
				if (Player* player = caster->ToPlayer())
					player->KilledMonsterCredit(NPC_LIFEBLOOD_KILL_CREDIT, 0);

				me->DespawnAfterAction(5*IN_MILLISECONDS);
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BloodTapTimer <= diff)
			{
				DoCastVictim(SPELL_BLOOD_TAP);
				BloodTapTimer = urand(4000, 5000);
			}
			else BloodTapTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lifeblood_elementalAI(creature);
    }
};

enum TormentingTheSoftknuckles
{
	SPELL_SCARED_SOFTKNUCKLE      = 50979,

	QUEST_TORMENTING_SOFTKNUCKLES = 12530,
	NPC_HARDKNUCKLE_MATRIARCH     = 28213,
};

class npc_softknuckle : public CreatureScript
{
public:
    npc_softknuckle() : CreatureScript("npc_softknuckle") {}

    struct npc_softknuckleAI : public QuantumBasicAI
    {
        npc_softknuckleAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ResetTimer;

		bool AlreadySummoned;

        void Reset()
		{
			ResetTimer = 60000;

			AlreadySummoned = false;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_SCARED_SOFTKNUCKLE && !AlreadySummoned)
			{
				if (Player* player = caster->ToPlayer())
				{
					if (player->GetQuestStatus(QUEST_TORMENTING_SOFTKNUCKLES) == QUEST_STATUS_COMPLETE)
						return;

					me->SummonCreature(NPC_HARDKNUCKLE_MATRIARCH, 5131.69f, 3995.46f, -61.7222f, 0.843439f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);

					AlreadySummoned = true;
				}
			}
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of combat timer
			if (ResetTimer <= diff && !me->IsInCombatActive())
			{
				Reset();
				ResetTimer = 60000;
			}
			else ResetTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_softknuckleAI(creature);
    }
};

enum SongOfReflection
{
	SPELL_DIDGERIDOO         = 53038,
	QUEST_SONG_OF_REFLECTION = 12736,
};

class npc_glimmering_pillar_credit : public CreatureScript
{
public:
    npc_glimmering_pillar_credit() : CreatureScript("npc_glimmering_pillar_credit") {}

    struct npc_glimmering_pillar_creditAI : public QuantumBasicAI
    {
        npc_glimmering_pillar_creditAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_DIDGERIDOO)
			{
				if (Player* player = caster->ToPlayer())
				{
					if (player->GetQuestStatus(QUEST_SONG_OF_REFLECTION) == QUEST_STATUS_COMPLETE)
						return;

					player->KilledMonsterCredit(me->GetEntry(), 0);
				}
			}
		}

        void UpdateAI(uint32 const /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_glimmering_pillar_creditAI(creature);
    }
};

class npc_suntouched_pillar_credit : public CreatureScript
{
public:
    npc_suntouched_pillar_credit() : CreatureScript("npc_suntouched_pillar_credit") {}

    struct npc_suntouched_pillar_creditAI : public QuantumBasicAI
    {
        npc_suntouched_pillar_creditAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_DIDGERIDOO)
			{
				if (Player* player = caster->ToPlayer())
				{
					if (player->GetQuestStatus(QUEST_SONG_OF_REFLECTION) == QUEST_STATUS_COMPLETE)
						return;

					player->KilledMonsterCredit(me->GetEntry(), 0);
				}
			}
		}

        void UpdateAI(uint32 const /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_suntouched_pillar_creditAI(creature);
    }
};

class npc_mosslight_pillar_credit : public CreatureScript
{
public:
    npc_mosslight_pillar_credit() : CreatureScript("npc_mosslight_pillar_credit") {}

    struct npc_mosslight_pillar_creditAI : public QuantumBasicAI
    {
        npc_mosslight_pillar_creditAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_DIDGERIDOO)
			{
				if (Player* player = caster->ToPlayer())
				{
					if (player->GetQuestStatus(QUEST_SONG_OF_REFLECTION) == QUEST_STATUS_COMPLETE)
						return;

					player->KilledMonsterCredit(me->GetEntry(), 0);
				}
			}
		}

        void UpdateAI(uint32 const /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mosslight_pillar_creditAI(creature);
    }
};

class npc_skyreach_pillar_credit : public CreatureScript
{
public:
    npc_skyreach_pillar_credit() : CreatureScript("npc_skyreach_pillar_credit") {}

    struct npc_skyreach_pillar_creditAI : public QuantumBasicAI
    {
        npc_skyreach_pillar_creditAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_DIDGERIDOO)
			{
				if (Player* player = caster->ToPlayer())
				{
					if (player->GetQuestStatus(QUEST_SONG_OF_REFLECTION) == QUEST_STATUS_COMPLETE)
						return;

					player->KilledMonsterCredit(me->GetEntry(), 0);
				}
			}
		}

        void UpdateAI(uint32 const /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skyreach_pillar_creditAI(creature);
    }
};

void AddSC_sholazar_basin()
{
    new npc_injured_rainspeaker_oracle();
    new npc_vekjik();
    new npc_bushwhacker();
    new npc_engineer_helice();
    new npc_jungle_punch_target();
    new npc_high_oracle_soo_say();
    new npc_generic_oracle_treasure_trigger();
    new npc_generic_oracle_treasure_seeker();
    new npc_frenzyheart_zepik();
    new npc_mosswalker_victim();
    new npc_artruis_the_heartless();
	new npc_zepik_jaloot();
    new npc_adventurous_dwarf();
    new npc_tipsy_mcmanus();
    new go_brew_event();
    new npc_stormwatcher();
    new npc_rejek_first_blood();
    new vehicle_haiphoon();
	new npc_elder_harkek();
	new spell_q12620_the_lifewarden_wrath();
	new spell_q12589_shoot_rjr();
	new npc_vics_flying_machine();
	new npc_pilot_vic();
	new npc_lifeblood_elemental();
	new npc_softknuckle();
	new npc_glimmering_pillar_credit();
	new npc_suntouched_pillar_credit();
	new npc_mosslight_pillar_credit();
	new npc_skyreach_pillar_credit();
}