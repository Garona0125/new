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
#include "QuantumSystemText.h"

enum Aquementas
{
    AGGRO_YELL_AQUE     = -1000350,

    SPELL_AQUA_JET      = 13586,
    SPELL_FROST_SHOCK   = 15089,
};

class mob_aquementas : public CreatureScript
{
public:
    mob_aquementas() : CreatureScript("mob_aquementas") { }

    struct mob_aquementasAI : public QuantumBasicAI
    {
        mob_aquementasAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 SendItemTimer;
        uint32 SwitchFactionTimer;
        bool isFriendly;

        uint32 FrostShockTimer;
        uint32 AquaJetTimer;

        void Reset()
        {
            SendItemTimer = 0;
            SwitchFactionTimer = 10000;
            me->SetCurrentFaction(35);
            isFriendly = true;

            AquaJetTimer = 5000;
            FrostShockTimer = 1000;
        }

        void SendItem(Unit* receiver)
        {
            if (CAST_PLR(receiver)->HasItemCount(11169, 1, false) &&
                CAST_PLR(receiver)->HasItemCount(11172, 11, false) &&
                CAST_PLR(receiver)->HasItemCount(11173, 1, false) &&
                !CAST_PLR(receiver)->HasItemCount(11522, 1, true))
            {
                ItemPosCountVec dest;
                uint8 msg = CAST_PLR(receiver)->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, 11522, 1, NULL);
                if (msg == EQUIP_ERR_OK)
				{
                    CAST_PLR(receiver)->StoreNewItem(dest, 11522, 1, true);
				}
            }
        }

        void EnterToBattle(Unit* who)
        {
            DoSendQuantumText(AGGRO_YELL_AQUE, me, who);
        }

        void UpdateAI(const uint32 diff)
        {
            if (isFriendly)
            {
                if (SwitchFactionTimer <= diff)
                {
                    me->SetCurrentFaction(91);
                    isFriendly = false;
                }
				else SwitchFactionTimer -= diff;
            }

            if (!UpdateVictim())
                return;

            if (!isFriendly)
            {
                if (SendItemTimer <= diff)
                {
                    if (me->GetVictim()->GetTypeId() == TYPE_ID_PLAYER)
					{
                        SendItem(me->GetVictim());
						SendItemTimer = 5000;
					}
                }
				else SendItemTimer -= diff;
            }

            if (FrostShockTimer <= diff)
            {
                DoCastVictim(SPELL_FROST_SHOCK);
                FrostShockTimer = 15000;
            }
			else FrostShockTimer -= diff;

            if (AquaJetTimer <= diff)
            {
                DoCast(me, SPELL_AQUA_JET);
                AquaJetTimer = 15000;
            }
			else AquaJetTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_aquementasAI (creature);
    }
};

enum CustodianOfTime
{
    WHISPER_CUSTODIAN_1     = 0,
    WHISPER_CUSTODIAN_2     = 1,
    WHISPER_CUSTODIAN_3     = 2,
    WHISPER_CUSTODIAN_4     = 3,
    WHISPER_CUSTODIAN_5     = 4,
    WHISPER_CUSTODIAN_6     = 5,
    WHISPER_CUSTODIAN_7     = 6,
    WHISPER_CUSTODIAN_8     = 7,
    WHISPER_CUSTODIAN_9     = 8,
    WHISPER_CUSTODIAN_10    = 9,
    WHISPER_CUSTODIAN_11    = 10,
    WHISPER_CUSTODIAN_12    = 11,
    WHISPER_CUSTODIAN_13    = 12,
    WHISPER_CUSTODIAN_14    = 13,
};

class npc_custodian_of_time : public CreatureScript
{
public:
    npc_custodian_of_time() : CreatureScript("npc_custodian_of_time") { }

    struct npc_custodian_of_timeAI : public npc_escortAI
    {
        npc_custodian_of_timeAI(Creature* creature) : npc_escortAI(creature) {}

		void Reset() {}

        void EnterToBattle(Unit* /*who*/) {}

        void WaypointReached(uint32 point)
        {
            if (Player* player = GetPlayerForEscort())
            {
                switch (point)
                {
                    case 0:
                        Talk(WHISPER_CUSTODIAN_1, player->GetGUID());
                        break;
                    case 1:
                        Talk(WHISPER_CUSTODIAN_2, player->GetGUID());
                        break;
                    case 2:
                        Talk(WHISPER_CUSTODIAN_3, player->GetGUID());
                        break;
                    case 3:
                        Talk(WHISPER_CUSTODIAN_4, player->GetGUID());
                        break;
                    case 5:
                        Talk(WHISPER_CUSTODIAN_5, player->GetGUID());
                        break;
                    case 6:
                        Talk(WHISPER_CUSTODIAN_6, player->GetGUID());
                        break;
                    case 7:
                        Talk(WHISPER_CUSTODIAN_7, player->GetGUID());
                        break;
                    case 8:
                        Talk(WHISPER_CUSTODIAN_8, player->GetGUID());
                        break;
                    case 9:
                        Talk(WHISPER_CUSTODIAN_9, player->GetGUID());
                        break;
                    case 10:
                        Talk(WHISPER_CUSTODIAN_4, player->GetGUID());
                        break;
                    case 13:
                        Talk(WHISPER_CUSTODIAN_10, player->GetGUID());
                        break;
                    case 14:
                        Talk(WHISPER_CUSTODIAN_4, player->GetGUID());
                        break;
                    case 16:
                        Talk(WHISPER_CUSTODIAN_11, player->GetGUID());
                        break;
                    case 17:
                        Talk(WHISPER_CUSTODIAN_12, player->GetGUID());
                        break;
                    case 18:
                        Talk(WHISPER_CUSTODIAN_4, player->GetGUID());
                        break;
                    case 22:
                        Talk(WHISPER_CUSTODIAN_13, player->GetGUID());
                        break;
                    case 23:
                        Talk(WHISPER_CUSTODIAN_4, player->GetGUID());
                        break;
                    case 24:
                        Talk(WHISPER_CUSTODIAN_14, player->GetGUID());
                        DoCast(player, 34883);
                        player->AreaExploredOrEventHappens(10277);
                        break;
                }
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (HasEscortState(STATE_ESCORT_ESCORTING))
                return;

            if (who->GetTypeId() == TYPE_ID_PLAYER)
            {
                if (who->HasAura(34877) && CAST_PLR(who)->GetQuestStatus(10277) == QUEST_STATUS_INCOMPLETE)
                {
                    float Radius = 10.0f;

                    if (me->IsWithinDistInMap(who, Radius))
						Start(false, false, who->GetGUID());
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_custodian_of_timeAI(creature);
    }
};

class npc_marin_noggenfogger : public CreatureScript
{
public:
    npc_marin_noggenfogger() : CreatureScript("npc_marin_noggenfogger") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->IsVendor() && player->GetQuestRewardStatus(2662))
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

#define GOSSIP_ITEM_FLIGHT  "Please take me to the master's lair."

class npc_steward_of_time : public CreatureScript
{
public:
    npc_steward_of_time() : CreatureScript("npc_steward_of_time") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(10279) == QUEST_STATUS_INCOMPLETE || player->GetQuestRewardStatus(10279))
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FLIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(9978, creature->GetGUID());
        }
        else
            player->SEND_GOSSIP_MENU(9977, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF + 1)
            player->CastSpell(player, 34891, true); // (Flight through Caverns)

		return true;
    }

	bool OnQuestAccept(Player* player, Creature* /*creature*/, Quest const* quest)
    {
        if (quest->GetQuestId() == 10279) // Quest: To The Master's Lair
            player->CastSpell(player, 34891, true); // (Flight through Caverns)

        return false;
    }
};

#define GOSSIP_ITEM_NORGANNON_1 "What function do you serve?"
#define GOSSIP_ITEM_NORGANNON_2 "What are the Plates of Uldum?"
#define GOSSIP_ITEM_NORGANNON_3 "Where are the Plates of Uldum?"
#define GOSSIP_ITEM_NORGANNON_4 "Excuse me? We've been \"reschedueled for visitations\"? What does that mean?!"
#define GOSSIP_ITEM_NORGANNON_5 "So, what's inside Uldum?"
#define GOSSIP_ITEM_NORGANNON_6 "I will return when i have the Plates of Uldum."

class npc_stone_watcher_of_norgannon : public CreatureScript
{
public:
    npc_stone_watcher_of_norgannon() : CreatureScript("npc_stone_watcher_of_norgannon") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(2954) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(1674, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                player->SEND_GOSSIP_MENU(1675, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(1676, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
                player->SEND_GOSSIP_MENU(1677, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
                player->SEND_GOSSIP_MENU(1678, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_NORGANNON_6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
                player->SEND_GOSSIP_MENU(1679, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+5:
                player->CLOSE_GOSSIP_MENU();
                player->AreaExploredOrEventHappens(2954);
                break;
        }
        return true;
    }
};

enum Npc00X17
{
    SAY_OOX_START           = -1000287,
    SAY_OOX_AGGRO1          = -1000288,
    SAY_OOX_AGGRO2          = -1000289,
    SAY_OOX_AMBUSH          = -1000290,
    SAY_OOX17_AMBUSH_REPLY  = -1000291,
    SAY_OOX_END             = -1000292,

    Q_OOX17                 = 648,
    SPAWN_FIRST             = 7803,
    SPAWN_SECOND_1          = 5617,
    SPAWN_SECOND_2          = 7805,
};

class npc_OOX17 : public CreatureScript
{
public:
    npc_OOX17() : CreatureScript("npc_OOX17") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == Q_OOX17)
        {
            creature->SetCurrentFaction(113);
            creature->SetFullHealth();
            creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            DoSendQuantumText(SAY_OOX_START, creature);

            if (npc_escortAI* EscortAI = CAST_AI(npc_OOX17::npc_OOX17AI, creature->AI()))
                EscortAI->Start(true, false, player->GetGUID());
        }
        return true;
    }

    struct npc_OOX17AI : public npc_escortAI
    {
        npc_OOX17AI(Creature* creature) : npc_escortAI(creature) {}

        void WaypointReached(uint32 waypointId)
        {
            if (Player* player = GetPlayerForEscort())
            {
                switch (waypointId)
                {
                    case 23:
                        me->SummonCreature(SPAWN_FIRST, -8350.96f, -4445.79f, 10.10f, 6.20f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                        me->SummonCreature(SPAWN_FIRST, -8355.96f, -4447.79f, 10.10f, 6.27f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                        me->SummonCreature(SPAWN_FIRST, -8353.96f, -4442.79f, 10.10f, 6.08f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                        DoSendQuantumText(SAY_OOX_AMBUSH, me);
                        break;
                    case 56:
                        me->SummonCreature(SPAWN_SECOND_1, -7510.07f, -4795.50f, 9.35f, 6.06f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                        me->SummonCreature(SPAWN_SECOND_2, -7515.07f, -4797.50f, 9.35f, 6.22f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                        me->SummonCreature(SPAWN_SECOND_2, -7518.07f, -4792.50f, 9.35f, 6.22f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                        DoSendQuantumText(SAY_OOX_AMBUSH, me);
                        if (Unit* scoff = me->FindCreatureWithDistance(SPAWN_SECOND_2, 30))
                            DoSendQuantumText(SAY_OOX17_AMBUSH_REPLY, scoff);
                        break;
                    case 86:
                        DoSendQuantumText(SAY_OOX_END, me);
                        player->GroupEventHappens(Q_OOX17, me);
                        break;
                }
            }
        }

        void Reset(){}

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(RAND(SAY_OOX_AGGRO1, SAY_OOX_AGGRO2), me);
        }

        void JustSummoned(Creature* summoned)
        {
			summoned->AI()->AttackStart(me);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_OOX17AI(creature);
    }
};

enum Tooga
{
    SAY_TOOGA_THIRST         = -1000391,
    SAY_TOOGA_WORRIED        = -1000392,
    SAY_TOOGA_POST_1         = -1000393,
    SAY_TORT_POST_2          = -1000394,
    SAY_TOOGA_POST_3         = -1000395,
    SAY_TORT_POST_4          = -1000396,
    SAY_TOOGA_POST_5         = -1000397,
    SAY_TORTA_POST_6         = -1000398,

    QUEST_TOOGA              = 1560,
    NPC_TORTA                = 6015,

    POINT_ID_TO_WATER        = 1,
    FACTION_TOOG_ESCORTEE    = 113,
};

Position const ToWaterLoc = {-7032.664551f, -4906.199219f, -1.606446f, 0.0f};

class npc_tooga : public CreatureScript
{
public:
    npc_tooga() : CreatureScript("npc_tooga") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_TOOGA)
        {
            if (npc_toogaAI* ToogaAI = CAST_AI(npc_tooga::npc_toogaAI, creature->AI()))
			{
                ToogaAI->StartFollow(player, FACTION_TOOG_ESCORTEE, quest);
			}
        }
        return true;
    }

    struct npc_toogaAI : public FollowerAI
    {
        npc_toogaAI(Creature* creature) : FollowerAI(creature) { }

        uint32 CheckSpeechTimer;
        uint32 PostEventTimer;
        uint32 PhasePostEvent;

        uint64 TortaGUID;

        void Reset()
        {
            CheckSpeechTimer = 2500;
            PostEventTimer = 1000;
            PhasePostEvent = 0;

            TortaGUID = 0;
        }

        void MoveInLineOfSight(Unit* who)
        {
            FollowerAI::MoveInLineOfSight(who);

            if (!me->GetVictim() && !HasFollowState(STATE_FOLLOW_COMPLETE | STATE_FOLLOW_POSTEVENT) && who->GetEntry() == NPC_TORTA)
            {
                if (me->IsWithinDistInMap(who, INTERACTION_DISTANCE))
                {
                    Player* player = GetLeaderForFollower();

                    if (player && player->GetQuestStatus(QUEST_TOOGA) == QUEST_STATUS_INCOMPLETE)
                        player->GroupEventHappens(QUEST_TOOGA, me);

                    TortaGUID = who->GetGUID();
                    SetFollowComplete(true);
                }
            }
        }

        void MovementInform(uint32 MotionType, uint32 PointId)
        {
            FollowerAI::MovementInform(MotionType, PointId);

            if (MotionType != POINT_MOTION_TYPE)
                return;

            if (PointId == POINT_ID_TO_WATER)
                SetFollowComplete();
        }

        void UpdateFollowerAI(const uint32 diff)
        {
            if (!UpdateVictim())
            {
                //we are doing the post-event, or...
                if (HasFollowState(STATE_FOLLOW_POSTEVENT))
                {
                    if (PostEventTimer <= diff)
                    {
                        PostEventTimer = 5000;

                        Unit* Torta = Unit::GetUnit(*me, TortaGUID);
                        if (!Torta || !Torta->IsAlive())
                        {
                            //something happened, so just complete
                            SetFollowComplete();
                            return;
                        }

                        switch (PhasePostEvent)
                        {
                            case 1:
                                DoSendQuantumText(SAY_TOOGA_POST_1, me);
                                break;
                            case 2:
                                DoSendQuantumText(SAY_TORT_POST_2, Torta);
                                break;
                            case 3:
                                DoSendQuantumText(SAY_TOOGA_POST_3, me);
                                break;
                            case 4:
                                DoSendQuantumText(SAY_TORT_POST_4, Torta);
                                break;
                            case 5:
                                DoSendQuantumText(SAY_TOOGA_POST_5, me);
                                break;
                            case 6:
                                DoSendQuantumText(SAY_TORTA_POST_6, Torta);
                                me->GetMotionMaster()->MovePoint(POINT_ID_TO_WATER, ToWaterLoc);
                                break;
                        }

                        ++PhasePostEvent;
                    }
                    else PostEventTimer -= diff;
                }
                //...we are doing regular speech check
                else if (HasFollowState(STATE_FOLLOW_INPROGRESS))
                {
                    if (CheckSpeechTimer <= diff)
                    {
                        CheckSpeechTimer = 5000;

                        if (urand(0, 9) > 8)
                            DoSendQuantumText(RAND(SAY_TOOGA_THIRST, SAY_TOOGA_WORRIED), me);
                    }
                    else CheckSpeechTimer -= diff;
                }
                return;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_toogaAI(creature);
    }
};

/*######
## npc_soridormi HyjalRing
######*/

enum HyjalRingQuests
{
    QUEST_SAGE     = 10472,
    QUEST_DEFENDER = 10475,
    QUEST_ETERNAL  = 10473,
    QUEST_CHAMPION = 10474,

    QUEST_W_SAGE     = 11103,
    QUEST_W_DEFENDER = 11106,
    QUEST_W_ETERNAL  = 11104,
    QUEST_W_CHAMPION = 11105,
};

enum HyjalRings
{
    RING_SAGE     = 29305,
    RING_DEFENDER = 29297,
    RING_ETERNAL  = 29309,
    RING_CHAMPION = 29301,
};

#define GOSSIP_TEXT "I have lost my ring. Can you enchant him again?"

bool AddItemToPlayer(Player* player, uint32 ItemEntry)
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

class npc_soridormi : public CreatureScript
{
public:
	npc_soridormi() : CreatureScript("npc_soridormi") { }

    bool OnGoosipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu( creature->GetGUID());

        if (!(player->HasItemCount(RING_SAGE, 1, true) || player->HasItemCount(RING_DEFENDER, 1, true) ||
            player->HasItemCount(RING_ETERNAL, 1, true) || player->HasItemCount(RING_CHAMPION, 1, true)))
        {
            if (player->GetQuestStatus(QUEST_SAGE) == QUEST_STATUS_COMPLETE && player->GetQuestRewardStatus(QUEST_SAGE))
            {
                player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF +1);
            }
			else if (player->GetQuestStatus(QUEST_ETERNAL) == QUEST_STATUS_COMPLETE && player->GetQuestRewardStatus(QUEST_ETERNAL))
            {
                player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            }
			else if (player->GetQuestStatus(QUEST_DEFENDER) == QUEST_STATUS_COMPLETE && player->GetQuestRewardStatus(QUEST_DEFENDER))
            {
                player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            }
			else if (player->GetQuestStatus(QUEST_CHAMPION) == QUEST_STATUS_COMPLETE && player->GetQuestRewardStatus(QUEST_CHAMPION))
            {
                player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            }
        }
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        player->CLOSE_GOSSIP_MENU();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                if (AddItemToPlayer(player, RING_SAGE))
                {
                    player->SetQuestStatus(QUEST_W_SAGE, QUEST_STATUS_NONE);
                    //player->getQuestStatusMap()[QUEST_W_SAGE].m_rewarded = false;
                }
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                if (AddItemToPlayer(player,RING_ETERNAL))
                {
                    player->SetQuestStatus(QUEST_W_ETERNAL,QUEST_STATUS_NONE);
                    //player->getQuestStatusMap()[QUEST_W_ETERNAL].m_rewarded = false;
                }
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                if (AddItemToPlayer(player,RING_DEFENDER))
                {
                    player->SetQuestStatus(QUEST_W_DEFENDER,QUEST_STATUS_NONE);
                    //player->getQuestStatusMap()[QUEST_W_DEFENDER].m_rewarded = false;
                }
                break;
            case GOSSIP_ACTION_INFO_DEF + 4:
                if (AddItemToPlayer(player,RING_CHAMPION))
                {
                    player->SetQuestStatus(QUEST_W_CHAMPION,QUEST_STATUS_NONE);
                    //player->getQuestStatusMap()[QUEST_W_CHAMPION].m_rewarded = false;
                }
				break;
		}
        return true;
    };
};

/*######
## npc_anachronos  AQ40Ring
######*/

enum AQ40RingQuests
{
    QUEST_AQ40_PROTECTOR  = 8751,
    QUEST_AQ4_CONQUEROR   = 8756,
    QUEST_AQ4_INVOKER     = 8761,

    QUEST_AQ4_W_PROTECTOR = 8764,
    QUEST_AQ4_W_CONQUEROR = 8766,
    QUEST_AQ4_W_INVOKER   = 8765,
};

enum AQ40Rings
{
    I_AQ4_PROTECTOR = 21200,
    I_AQ4_CONQUEROR = 21205,
    I_AQ4_INVOKER   = 21210,
};

class npc_anachronos : public CreatureScript
{
public:
    npc_anachronos() : CreatureScript("npc_anachronos") { }

    bool OnGoosipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu( creature->GetGUID());

        if (!(player->HasItemCount(I_AQ4_PROTECTOR,1,true) || player->HasItemCount(I_AQ4_CONQUEROR,1,true) ||
            player->HasItemCount(I_AQ4_INVOKER,1,true)))
		{
			if (player->GetQuestStatus(QUEST_AQ40_PROTECTOR) == QUEST_STATUS_COMPLETE && player->GetQuestRewardStatus(QUEST_AQ40_PROTECTOR))
			{
                player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            }
			else if (player->GetQuestStatus(QUEST_AQ4_CONQUEROR) == QUEST_STATUS_COMPLETE && player->GetQuestRewardStatus(QUEST_AQ4_CONQUEROR))
            {
                player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            }
			else if (player->GetQuestStatus(QUEST_AQ4_INVOKER) == QUEST_STATUS_COMPLETE && player->GetQuestRewardStatus(QUEST_AQ4_INVOKER))
            {
                player->ADD_GOSSIP_ITEM(0, GOSSIP_TEXT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            }
        }
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        player->CLOSE_GOSSIP_MENU();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                if (AddItemToPlayer(player, I_AQ4_PROTECTOR))
                {
                    player->SetQuestStatus(QUEST_AQ4_W_PROTECTOR, QUEST_STATUS_NONE);
                    //player->getQuestStatusMap()[QUEST_AQ4_W_PROTECTOR].m_rewarded  = false;
                }
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                if (AddItemToPlayer(player, I_AQ4_CONQUEROR))
                {
                    player->SetQuestStatus(QUEST_AQ4_W_CONQUEROR, QUEST_STATUS_NONE);
                    //player->getQuestStatusMap()[QUEST_AQ4_W_CONQUEROR].m_rewarded = false;
                }
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                if (AddItemToPlayer(player, I_AQ4_INVOKER))
                {
                    player->SetQuestStatus(QUEST_AQ4_W_INVOKER,QUEST_STATUS_NONE);
                    //player->getQuestStatusMap()[QUEST_AQ4_W_INVOKER].m_rewarded = false;
                }
				break;
		}
		return true;
    };
};

void AddSC_tanaris()
{
    new mob_aquementas();
    new npc_custodian_of_time();
    new npc_marin_noggenfogger();
    new npc_steward_of_time();
    new npc_stone_watcher_of_norgannon();
    new npc_OOX17();
    new npc_tooga();
	new npc_soridormi();
	new npc_anachronos();
}