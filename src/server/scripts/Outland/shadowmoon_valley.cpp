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
#include "SpellScript.h"
#include "Group.h"

enum MatureNetherwing
{
    SAY_JUST_EATEN      = -1000175,

    SPELL_PLACE_CARCASS = 38439,
    SPELL_JUST_EATEN    = 38502,
    SPELL_NETHER_BREATH = 38467,

    GO_CARCASS          = 185155,
    QUEST_KINDNESS      = 10804,
    NPC_EVENT_PINGER    = 22131,
	POINT_ID            = 1,
};

class npc_mature_netherwing_drake : public CreatureScript
{
public:
    npc_mature_netherwing_drake() : CreatureScript("npc_mature_netherwing_drake") { }

    struct npc_mature_netherwing_drakeAI : public QuantumBasicAI
    {
        npc_mature_netherwing_drakeAI(Creature* creature) : QuantumBasicAI(creature) { }

        uint64 uiPlayerGUID;

        bool bCanEat;
        bool bIsEating;

        uint32 EatTimer;
        uint32 CastTimer;

        void Reset()
        {
            uiPlayerGUID = 0;

            bCanEat = false;
            bIsEating = false;

            EatTimer = 5000;
            CastTimer = 5000;
        }

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (bCanEat || bIsEating)
                return;

            if (caster->GetTypeId() == TYPE_ID_PLAYER && spell->Id == SPELL_PLACE_CARCASS && !me->HasAura(SPELL_JUST_EATEN))
            {
                uiPlayerGUID = caster->GetGUID();
                bCanEat = true;
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == POINT_ID)
            {
                bIsEating = true;
                EatTimer = 7000;
                me->HandleEmoteCommand(EMOTE_ONESHOT_ATTACK_UNARMED);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (bCanEat || bIsEating)
            {
                if (EatTimer <= diff)
                {
                    if (bCanEat && !bIsEating)
                    {
                        if (Unit* unit = Unit::GetUnit(*me, uiPlayerGUID))
                        {
                            if (GameObject* go = unit->FindGameobjectWithDistance(GO_CARCASS, 10.0f))
                            {
                                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE)
                                    me->GetMotionMaster()->MovementExpired();

                                me->GetMotionMaster()->MoveIdle();
                                me->StopMoving();

                                me->GetMotionMaster()->MovePoint(POINT_ID, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ());
                            }
                        }
                        bCanEat = false;
                    }
                    else if (bIsEating)
                    {
                        DoCast(me, SPELL_JUST_EATEN);
                        DoSendQuantumText(SAY_JUST_EATEN, me);

                        if (Player* player = Unit::GetPlayer(*me, uiPlayerGUID))
                        {
                            player->KilledMonsterCredit(NPC_EVENT_PINGER, 0);

                            if (GameObject* go = player->FindGameobjectWithDistance(GO_CARCASS, 10.0f))
                                go->Delete();
                        }

                        Reset();
                        me->GetMotionMaster()->Clear();
                    }
                }
                else EatTimer -= diff;

            return;
            }

            if (!UpdateVictim())
                return;

            if (CastTimer <= diff)
            {
                DoCastVictim(SPELL_NETHER_BREATH);
                CastTimer = 5000;
            }
			else CastTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mature_netherwing_drakeAI(creature);
    }
};

enum EnslavedDrake
{
	FACTION_DEFAULT               = 62,
	FACTION_FRIENDLY              = 1840,

	SPELL_HIT_FORCE_OF_NELTHARAKU = 38762,
	SPELL_FORCE_OF_NELTHARAKU     = 38775,

	NPC_DRAGONMAW_SUBJUGATOR      = 21718,
	NPC_ESCAPE_DUMMY              = 22317,
};

class npc_enslaved_netherwing_drake : public CreatureScript
{
public:
    npc_enslaved_netherwing_drake() : CreatureScript("npc_enslaved_netherwing_drake") { }

    struct npc_enslaved_netherwing_drakeAI : public QuantumBasicAI
    {
        npc_enslaved_netherwing_drakeAI(Creature* creature) : QuantumBasicAI(creature)
        {
            PlayerGUID = 0;
            Tapped = false;
            Reset();
        }

        uint64 PlayerGUID;
        uint32 FlyTimer;
        bool Tapped;

        void Reset()
        {
            if (!Tapped)
                me->SetCurrentFaction(FACTION_DEFAULT);

            FlyTimer = 10000;
            me->SetDisableGravity(false);
            me->SetVisible(true);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (!caster)
                return;

            if (caster->GetTypeId() == TYPE_ID_PLAYER && spell->Id == SPELL_HIT_FORCE_OF_NELTHARAKU && !Tapped)
            {
                Tapped = true;
                PlayerGUID = caster->GetGUID();

                me->SetCurrentFaction(FACTION_FRIENDLY);
                DoCast(caster, SPELL_FORCE_OF_NELTHARAKU, true);

                Unit* Dragonmaw = me->FindCreatureWithDistance(NPC_DRAGONMAW_SUBJUGATOR, 50.0f);

                if (Dragonmaw)
                {
                    me->AddThreat(Dragonmaw, 100000.0f);
                    AttackStart(Dragonmaw);
                }

                HostileReference* ref = me->getThreatManager().getOnlineContainer().getReferenceByTarget(caster);
                if (ref)
                    ref->removeReference();
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == 1)
            {
                if (PlayerGUID)
                {
                    Unit* player = Unit::GetUnit(*me, PlayerGUID);
                    if (player)
                        DoCast(player, SPELL_FORCE_OF_NELTHARAKU, true);

                    PlayerGUID = 0;
                }
                me->SetVisible(false);
                me->SetDisableGravity(false);
                me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                me->RemoveCorpse();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
            {
                if (Tapped)
                {
                    if (FlyTimer <= diff)
                    {
                        Tapped = false;
                        if (PlayerGUID)
                        {
                            Player* player = Unit::GetPlayer(*me, PlayerGUID);
                            if (player && player->GetQuestStatus(10854) == QUEST_STATUS_INCOMPLETE)
                            {
                                DoCast(player, SPELL_FORCE_OF_NELTHARAKU, true);
                                /*
                                float x, y, z;
                                me->GetPosition(x, y, z);

                                float dx, dy, dz;
                                me->GetRandomPoint(x, y, z, 20, dx, dy, dz);
                                dz += 20; // so it's in the air, not ground*/

                                Position pos;
                                if (Unit* EscapeDummy = me->FindCreatureWithDistance(NPC_ESCAPE_DUMMY, 30))
                                    EscapeDummy->GetPosition(&pos);
                                else
                                {
                                    me->GetRandomNearPosition(pos, 20);
                                    pos.m_positionZ += 25;
                                }

                                me->SetDisableGravity(true);
                                me->GetMotionMaster()->MovePoint(1, pos);
                            }
                        }
                    }
					else FlyTimer -= diff;
                }
                return;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_enslaved_netherwing_drakeAI(creature);
    }
};

enum DragonmawPeon
{
	SPELL_SUNDER_ARMOR    = 15572,
	SPELL_KICK            = 34802,
	SPELL_POISONED_MUTTON = 40468,

	NPC_PEON_KILL_CREDIT  = 23209,
	QUEST_A_SLOW_DEATH    = 11020,
};

class npc_dragonmaw_peon : public CreatureScript
{
public:
    npc_dragonmaw_peon() : CreatureScript("npc_dragonmaw_peon") { }

    struct npc_dragonmaw_peonAI : public QuantumBasicAI
    {
        npc_dragonmaw_peonAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint64 PlayerGUID;

        bool Tapped;

		uint32 SunderArmorTimer;
		uint32 KickTimer;
        uint32 PoisonTimer;

        void Reset()
        {
            PlayerGUID = 0;
            Tapped = false;

            PoisonTimer = 0;
			SunderArmorTimer = 1000;
			KickTimer = 2000;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_WORK_MINING);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (!caster)
                return;

            if (caster->GetTypeId() == TYPE_ID_PLAYER && spell->Id == SPELL_POISONED_MUTTON && !Tapped)
            {
                PlayerGUID = caster->GetGUID();

                Tapped = true;
                float x, y, z;
                caster->GetClosePoint(x, y, z, me->GetObjectSize());

                me->SetWalk(false);
                me->GetMotionMaster()->MovePoint(1, x, y, z);
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id)
			{
                me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_ONESHOT_EAT);
                PoisonTimer = 15000;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (PoisonTimer)
            {
                if (PoisonTimer <= diff)
                {
                    if (PlayerGUID)
                    {
                        Player* player = Unit::GetPlayer(*me, PlayerGUID);
                        if (player && player->GetQuestStatus(QUEST_A_SLOW_DEATH) == QUEST_STATUS_INCOMPLETE)
                            player->KilledMonsterCredit(NPC_PEON_KILL_CREDIT, 0);
                    }

                    PoisonTimer = 0;
                    me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                }
				else PoisonTimer -= diff;
            }

			if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SunderArmorTimer <= diff)
			{
				DoCastVictim(SPELL_SUNDER_ARMOR);
				SunderArmorTimer = urand(2000, 3000);
			}
			else SunderArmorTimer -= diff;

			if (KickTimer <= diff)
			{
				DoCastVictim(SPELL_KICK);
				KickTimer = urand(4000, 5000);
			}
			else KickTimer -= diff;

			DoMeleeAttackIfReady();
		}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonmaw_peonAI(creature);
    }
};

class npc_drake_dealer_hurlunk : public CreatureScript
{
public:
    npc_drake_dealer_hurlunk() : CreatureScript("npc_drake_dealer_hurlunk") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
		if (creature->IsVendor() && player->GetReputationRank(1015) == REP_EXALTED)
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

enum Swiftwing
{
	ITEM_FLANIS_PACK          = 30658,
	ITEM_KARGOSH_PACK         = 30659,

	QUEST_THE_FATE_OF_FLAINS  = 10583,
	QUEST_THE_FATE_OF_KARGOSH = 10601,
};

#define GOSSIP_HSK1 "Take Flanis's Pack"
#define GOSSIP_HSK2 "Take Kagrosh's Pack"

class npcs_flanis_swiftwing_and_kagrosh : public CreatureScript
{
public:
    npcs_flanis_swiftwing_and_kagrosh() : CreatureScript("npcs_flanis_swiftwing_and_kagrosh") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_THE_FATE_OF_FLAINS) == QUEST_STATUS_INCOMPLETE && !player->HasItemCount(ITEM_FLANIS_PACK, 1, true))
            player->ADD_GOSSIP_ITEM(0, GOSSIP_HSK1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        if (player->GetQuestStatus(QUEST_THE_FATE_OF_KARGOSH) == QUEST_STATUS_INCOMPLETE && !player->HasItemCount(ITEM_KARGOSH_PACK, 1, true))
            player->ADD_GOSSIP_ITEM(0, GOSSIP_HSK2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            ItemPosCountVec dest;
            uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ITEM_FLANIS_PACK, 1, NULL);
            if (msg == EQUIP_ERR_OK)
            {
                player->StoreNewItem(dest, ITEM_FLANIS_PACK, 1, true);
                player->PlayerTalkClass->ClearMenus();
            }
        }
        if (action == GOSSIP_ACTION_INFO_DEF+2)
        {
            ItemPosCountVec dest;
            uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ITEM_KARGOSH_PACK, 1, NULL);
            if (msg == EQUIP_ERR_OK)
            {
                player->StoreNewItem(dest, ITEM_KARGOSH_PACK, 1, true);
                player->PlayerTalkClass->ClearMenus();
            }
        }
        return true;
    }
};

enum MurkbloodOverseer
{
	SPELL_GIVING_A_HAND   = 41121,

	QUEST_SEEKER_OF_TRUTH = 11082,
};

#define GOSSIP_HMO "I am here for you, overseer."
#define GOSSIP_SMO1 "How dare you question an overseer of the Dragonmaw!"
#define GOSSIP_SMO2 "Who speaks of me? What are you talking about, broken?"
#define GOSSIP_SMO3 "Continue please."
#define GOSSIP_SMO4 "Who are these bidders?"
#define GOSSIP_SMO5 "Well... yes."

class npc_murkblood_overseer : public CreatureScript
{
public:
    npc_murkblood_overseer() : CreatureScript("npc_murkblood_overseer") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
		if (player->GetQuestStatus(QUEST_SEEKER_OF_TRUTH) == QUEST_STATUS_INCOMPLETE)
			player->ADD_GOSSIP_ITEM(0, GOSSIP_HMO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
		
		player->SEND_GOSSIP_MENU(10940, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
		player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SMO1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(10940, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SMO2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
                player->SEND_GOSSIP_MENU(10940, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SMO3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
                player->SEND_GOSSIP_MENU(10940, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SMO4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
                player->SEND_GOSSIP_MENU(10940, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+5:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SMO5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
                player->SEND_GOSSIP_MENU(10940, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+6:
				player->SEND_GOSSIP_MENU(10940, creature->GetGUID());
                creature->CastSpell(player, SPELL_GIVING_A_HAND, false);
                player->AreaExploredOrEventHappens(QUEST_SEEKER_OF_TRUTH);
                break;
        }
        return true;
    }
};

enum OronokTornheart
{
	QUEST_THE_CIPHER_OF_DAMNATION = 10519,
};

#define GOSSIP_ORONOK1 "I am ready to hear your story, Oronok."
#define GOSSIP_ORONOK2 "How do I find the cipher?"
#define GOSSIP_ORONOK3 "How do you know all of this?"
#define GOSSIP_ORONOK4 "Yet what? What is it, Oronok?"
#define GOSSIP_ORONOK5 "Continue, please."
#define GOSSIP_ORONOK6 "So what of the cipher now? And your boys?"
#define GOSSIP_ORONOK7 "I will find your boys and the cipher, Oronok."

class npc_oronok_tornheart : public CreatureScript
{
public:
    npc_oronok_tornheart() : CreatureScript("npc_oronok_tornheart") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
		if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->IsVendor())
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_VENDOR_BROWSE_YOUR_GOODS), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        if (player->GetQuestStatus(QUEST_THE_CIPHER_OF_DAMNATION) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ORONOK1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            player->SEND_GOSSIP_MENU(10312, creature->GetGUID());
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
            case GOSSIP_ACTION_INFO_DEF:
                player->ADD_GOSSIP_ITEM(0, GOSSIP_ORONOK2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                player->SEND_GOSSIP_MENU(10313, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(0, GOSSIP_ORONOK3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(10314, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(0, GOSSIP_ORONOK4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
                player->SEND_GOSSIP_MENU(10315, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->ADD_GOSSIP_ITEM(0, GOSSIP_ORONOK5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
                player->SEND_GOSSIP_MENU(10316, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                player->ADD_GOSSIP_ITEM(0, GOSSIP_ORONOK6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
                player->SEND_GOSSIP_MENU(10317, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+5:
                player->ADD_GOSSIP_ITEM(0, GOSSIP_ORONOK7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
                player->SEND_GOSSIP_MENU(10318, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+6:
                player->CLOSE_GOSSIP_MENU();
                player->AreaExploredOrEventHappens(QUEST_THE_CIPHER_OF_DAMNATION);
                break;
        }
        return true;
    }
};

enum Karynaku
{
    QUEST_ALLY_OF_NETHER    = 10870,
    QUEST_ZUHULED_THE_WACK  = 10866,

    NPC_ZUHULED_THE_WACKED  = 11980,
    TAXI_PATH_ID            = 649,
};

class npc_karynaku : public CreatureScript
{
    public:
        npc_karynaku() : CreatureScript("npc_karynaku") { }

        bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
        {
            if (quest->GetQuestId() == QUEST_ALLY_OF_NETHER)
                player->ActivateTaxiPathTo(TAXI_PATH_ID);

            if (quest->GetQuestId() == QUEST_ZUHULED_THE_WACK)
                creature->SummonCreature(NPC_ZUHULED_THE_WACKED, -4204.94f, 316.397f, 122.508f, 1.309f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);

            return true;
        }
};

enum OverlordData
{
    QUEST_LORD_ILLIDAN_STORMRAGE    = 11108,

    C_ILLIDAN                       = 22083,
    C_YARZILL                       = 23141,

    SPELL_ONE                       = 39990, // Red Lightning Bolt
    SPELL_TWO                       = 41528, // Mark of Stormrage
    SPELL_THREE                     = 40216, // Dragonaw Faction
    SPELL_FOUR                      = 42016, // Dragonaw Trasform

    OVERLORD_SAY_1                  = -1000606,
    OVERLORD_SAY_2                  = -1000607,
    OVERLORD_SAY_3                  = -1000608,
    OVERLORD_SAY_4                  = -1000609,
    OVERLORD_SAY_5                  = -1000610,
    OVERLORD_SAY_6                  = -1000611,
    OVERLORD_YELL_1                 = -1000612,
    OVERLORD_YELL_2                 = -1000613,
    LORD_ILLIDAN_SAY_1              = -1000614,
    LORD_ILLIDAN_SAY_2              = -1000615,
    LORD_ILLIDAN_SAY_3              = -1000616,
    LORD_ILLIDAN_SAY_4              = -1000617,
    LORD_ILLIDAN_SAY_5              = -1000618,
    LORD_ILLIDAN_SAY_6              = -1000619,
    LORD_ILLIDAN_SAY_7              = -1000620,
    YARZILL_THE_MERC_SAY            = -1000621,
};

class npc_overlord_morghor : public CreatureScript
{
public:
    npc_overlord_morghor() : CreatureScript("npc_overlord_morghor") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest *quest)
    {
        if (quest->GetQuestId() == QUEST_LORD_ILLIDAN_STORMRAGE)
        {
            CAST_AI(npc_overlord_morghor::npc_overlord_morghorAI, creature->AI())->PlayerGUID = player->GetGUID();
            CAST_AI(npc_overlord_morghor::npc_overlord_morghorAI, creature->AI())->StartEvent();
            return true;
        }
        return false;
    }

    struct npc_overlord_morghorAI : public QuantumBasicAI
    {
        npc_overlord_morghorAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint64 PlayerGUID;
        uint64 IllidanGUID;

        uint32 ConversationTimer;
        uint32 Step;

        bool Event;

        void Reset()
        {
            PlayerGUID = 0;
            IllidanGUID = 0;

            ConversationTimer = 0;
            Step = 0;

            Event = false;
            me->SetUInt32Value(UNIT_NPC_FLAGS, 2);
        }

        void StartEvent()
        {
            me->SetUInt32Value(UNIT_NPC_FLAGS, 0);
            me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
            Unit* Illidan = me->SummonCreature(C_ILLIDAN, -5107.83f, 602.584f, 85.2393f, 4.92598f, TEMPSUMMON_CORPSE_DESPAWN, 0);
            if (Illidan)
            {
                IllidanGUID = Illidan->GetGUID();
                Illidan->SetVisible(false);
            }
            if (PlayerGUID)
            {
                Player* player = Unit::GetPlayer(*me, PlayerGUID);
                if (player)
                    DoSendQuantumText(OVERLORD_SAY_1, me, player);
            }
            ConversationTimer = 4200;
            Step = 0;
            Event = true;
        }

        uint32 NextStep(uint32 Step)
        {
            Unit* player = Unit::GetUnit(*me, PlayerGUID);

            Unit* Illi = Unit::GetUnit(*me, IllidanGUID);

            if (!player || !Illi)
            {
                EnterEvadeMode();
                return 0;
            }

            switch (Step)
            {
            case 0: return 0; 
				break;
            case 1:
				me->GetMotionMaster()->MovePoint(0, -5104.41f, 595.297f, 85.6838f); 
				return 9000;
				break;
            case 2:
				DoSendQuantumText(OVERLORD_YELL_1, me, player);
				return 4500;
				break;
            case 3:
				me->SetInFront(player);
				return 3200;
				break;
            case 4:
				DoSendQuantumText(OVERLORD_SAY_2, me, player);
				return 2000;
				break;
            case 5:
				Illi->SetVisible(true);
				Illi->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				return 350;
				break;
			case 6:
				Illi->CastSpell(Illi, SPELL_ONE, true);
                Illi->SetTarget(me->GetGUID());
                me->SetTarget(IllidanGUID);
                return 2000;
				break;
            case 7:
				DoSendQuantumText(OVERLORD_YELL_2, me);
				return 4500;
				break;
            case 8:
				me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_KNEEL);
				return 2500;
				break;
            case 9:
				DoSendQuantumText(OVERLORD_SAY_3, me);
				return 6500;
				break;
            case 10:
				DoSendQuantumText(LORD_ILLIDAN_SAY_1, Illi);
				return 5000;
				break;
            case 11:
				DoSendQuantumText(OVERLORD_SAY_4, me, player);
				return 6000;
				break;
            case 12:
				DoSendQuantumText(LORD_ILLIDAN_SAY_2, Illi);
				return 5500;
				break;
            case 13:
				DoSendQuantumText(LORD_ILLIDAN_SAY_3, Illi);
				return 4000;
				break;
            case 14: Illi->SetTarget(PlayerGUID);
				return 1500;
				break;
            case 15:
				DoSendQuantumText(LORD_ILLIDAN_SAY_4, Illi);
				return 1500;
				break;
			case 16:
				Illi->CastSpell(player, SPELL_TWO, true);
				player->RemoveAurasDueToSpell(SPELL_THREE);
				player->RemoveAurasDueToSpell(SPELL_FOUR);
				return 5000;
				break;
            case 17:
				DoSendQuantumText(LORD_ILLIDAN_SAY_5, Illi);
				return 5000;
				break;
            case 18:
				DoSendQuantumText(LORD_ILLIDAN_SAY_6, Illi);
				return 5000;
				break;
            case 19:
				DoSendQuantumText(LORD_ILLIDAN_SAY_7, Illi);
				return 5000;
				break;
            case 20:
                Illi->HandleEmoteCommand(EMOTE_ONESHOT_LIFT_OFF);
                Illi->SetDisableGravity(true);
                return 500;
				break;
            case 21:
				DoSendQuantumText(OVERLORD_SAY_5, me);
				return 500;
				break;
            case 22:
                Illi->SetVisible(false);
                Illi->setDeathState(JUST_DIED);
                return 1000;
				break;
            case 23:
				me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
				return 2000;
				break;
            case 24:
				me->SetTarget(PlayerGUID);
				return 5000;
				break;
            case 25: DoSendQuantumText(OVERLORD_SAY_6, me);
				return 2000;
				break;
            case 26:
                if (player)
					CAST_PLR(player)->GroupEventHappens(QUEST_LORD_ILLIDAN_STORMRAGE, me);
				return 6000;
				break;
            case 27:
				{
					Unit* Yarzill = me->FindCreatureWithDistance(C_YARZILL, 50);
					if (Yarzill)
						Yarzill->SetTarget(PlayerGUID);
					return 500; 
				}
				break;
			case 28:
                player->RemoveAurasDueToSpell(SPELL_TWO);
                player->RemoveAurasDueToSpell(41519);
                player->CastSpell(player, SPELL_THREE, true);
                player->CastSpell(player, SPELL_FOUR, true);
                return 1000;
				break;
            case 29:
                {
					Unit* Yarzill = me->FindCreatureWithDistance(C_YARZILL, 50);
					if (Yarzill)
						DoSendQuantumText(YARZILL_THE_MERC_SAY, Yarzill, player);
					return 5000;
				}
				break;
            case 30:
                {
					Unit* Yarzill = me->FindCreatureWithDistance(C_YARZILL, 50);
					if (Yarzill)
						Yarzill->SetTarget(0);
					return 5000;
				}
				break;
            case 31:
                {
					Unit* Yarzill = me->FindCreatureWithDistance(C_YARZILL, 50);
					if (Yarzill)
						Yarzill->CastSpell(player, 41540, true);
					return 1000;
				}
				break;
            case 32: me->GetMotionMaster()->MovePoint(0, -5085.77f, 577.231f, 86.6719f);
				return 5000;
				break;
            case 33: Reset();
				return 100;
				break;
            default :
				return 0;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!ConversationTimer)
                return;

            if (ConversationTimer <= diff)
            {
                if (Event && IllidanGUID && PlayerGUID)
                {
                    ConversationTimer = NextStep(++Step);
                }
            } 
			else ConversationTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_overlord_morghorAI(creature);
    }
};

enum Earthmender
{
    SAY_WIL_START               = -1000381,
    SAY_WIL_AGGRO1              = -1000382,
    SAY_WIL_AGGRO2              = -1000383,
    SAY_WIL_PROGRESS1           = -1000384,
    SAY_WIL_PROGRESS2           = -1000385,
    SAY_WIL_FIND_EXIT           = -1000386,
    SAY_WIL_PROGRESS4           = -1000387,
    SAY_WIL_PROGRESS5           = -1000388,
    SAY_WIL_JUST_AHEAD          = -1000389,
    SAY_WIL_END                 = -1000390,

    SPELL_CHAIN_LIGHTNING       = 16006,
    SPELL_EARTHBIND_TOTEM       = 15786,
    SPELL_FROST_SHOCK           = 12548,
    SPELL_HEALING_WAVE          = 12491,

    QUEST_ESCAPE_COILSCAR       = 10451,
    NPC_COILSKAR_ASSASSIN       = 21044,
    FACTION_EARTHEN             = 1726,
};

class npc_earthmender_wilda : public CreatureScript
{
public:
    npc_earthmender_wilda() : CreatureScript("npc_earthmender_wilda") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_ESCAPE_COILSCAR)
        {
            DoSendQuantumText(SAY_WIL_START, creature, player);
            creature->SetCurrentFaction(FACTION_EARTHEN);

            if (npc_earthmender_wildaAI* EscortAI = CAST_AI(npc_earthmender_wilda::npc_earthmender_wildaAI, creature->AI()))
                EscortAI->Start(false, false, player->GetGUID(), quest);
        }
        return true;
    }

    struct npc_earthmender_wildaAI : public npc_escortAI
    {
        npc_earthmender_wildaAI(Creature* creature) : npc_escortAI(creature) { }

		uint32 ChainLightningTimer;
		uint32 FrostShockTimer;
		uint32 EarthbindTotemTimer;
        uint32 HealingWaveTimer;

        void Reset()
        {
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void WaypointReached(uint32 PointId)
        {
            Player* player = GetPlayerForEscort();

            if (!player)
                return;

            switch (PointId)
            {
                case 13:
                    DoSendQuantumText(SAY_WIL_PROGRESS1, me, player);
                    DoSpawnAssassin();
                    break;
                case 14:
                    DoSpawnAssassin();
                    break;
                case 15:
                    DoSendQuantumText(SAY_WIL_FIND_EXIT, me, player);
                    break;
                case 19:
                    DoRandomSay();
                    break;
                case 20:
                    DoSpawnAssassin();
                    break;
                case 26:
                    DoRandomSay();
                    break;
                case 27:
                    DoSpawnAssassin();
                    break;
                case 33:
                    DoRandomSay();
                    break;
                case 34:
                    DoSpawnAssassin();
                    break;
                case 37:
                    DoRandomSay();
                    break;
                case 38:
                    DoSpawnAssassin();
                    break;
                case 39:
                    DoSendQuantumText(SAY_WIL_JUST_AHEAD, me, player);
                    break;
                case 43:
                    DoRandomSay();
                    break;
                case 44:
                    DoSpawnAssassin();
                    break;
                case 50:
                    DoSendQuantumText(SAY_WIL_END, me, player);
                    player->GroupEventHappens(QUEST_ESCAPE_COILSCAR, me);
                    break;
            }
        }

        void JustSummoned(Creature* summoned)
        {
            if (summoned->GetEntry() == NPC_COILSKAR_ASSASSIN)
                summoned->AI()->AttackStart(me);
        }

        void DoRandomSay()
        {
            DoSendQuantumText(RAND(SAY_WIL_PROGRESS2, SAY_WIL_PROGRESS4, SAY_WIL_PROGRESS5), me);
        }

        void DoSpawnAssassin()
        {
            DoSummon(NPC_COILSKAR_ASSASSIN, me, 15.0f, 5000, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT);
        }

        void EnterToBattle(Unit* who)
        {
            if (rand()%5)
                return;

            if (who->GetTypeId() != TYPE_ID_PLAYER)
            {
                if (urand(0, 1))
					DoSendQuantumText(RAND(SAY_WIL_AGGRO1, SAY_WIL_AGGRO2), who);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

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

			if (FrostShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FROST_SHOCK);
					FrostShockTimer = urand(5000, 6000);
				}
			}
			else FrostShockTimer -= diff;

			if (EarthbindTotemTimer <= diff)
			{
				DoCast(me, SPELL_EARTHBIND_TOTEM, true);
				EarthbindTotemTimer = urand(7000, 8000);
			}
			else EarthbindTotemTimer -= diff;

			if (HealingWaveTimer <= diff)
			{
				DoCast(me, SPELL_HEALING_WAVE);
				HealingWaveTimer = urand(9000, 10000);
			}
			else HealingWaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_earthmender_wildaAI(creature);
    }
};

enum TorlothTheMagnificent
{
	QUEST_BATTLE_OF_THE_CRIMSON_WATCH = 10781,
	END_TEXT                          = -1000366,
	EVENT_AREA_RADIUS                 = 65,
	EVENT_COOLDOWN                    = 30000,
};

struct TorlothCinematic
{
    int32 TextId;
    uint32 creature, Timer;
};

static TorlothCinematic TorlothAnim[] =
{
    {-1000367, 0, 2000},
    {-1000368, 1, 7000},
    {-1000369, 0, 3000},
    {0, 0, 2000},
    {-1000370, 0, 1000},
    {0, 0, 3000},
    {0, 0, 0}
};

struct Location
{
    float x, y, z, o;
};

static Location SpawnLocation[] =
{
    {-4615.8556f, 1342.2532f, 139.9f, 1.612f}, //Illidari Soldier
    {-4598.9365f, 1377.3182f, 139.9f, 3.917f}, //Illidari Soldier
    {-4598.4697f, 1360.8999f, 139.9f, 2.427f}, //Illidari Soldier
    {-4589.3599f, 1369.1061f, 139.9f, 3.165f}, //Illidari Soldier
    {-4608.3477f, 1386.0076f, 139.9f, 4.108f}, //Illidari Soldier
    {-4633.1889f, 1359.8033f, 139.9f, 0.949f}, //Illidari Soldier
    {-4623.5791f, 1351.4574f, 139.9f, 0.971f}, //Illidari Soldier
    {-4607.2988f, 1351.6099f, 139.9f, 2.416f}, //Illidari Soldier
    {-4633.7764f, 1376.0417f, 139.9f, 5.608f}, //Illidari Soldier
    {-4600.2461f, 1369.1240f, 139.9f, 3.056f}, //Illidari Mind Breaker
    {-4631.7808f, 1367.9459f, 139.9f, 0.020f}, //Illidari Mind Breaker
    {-4600.2461f, 1369.1240f, 139.9f, 3.056f}, //Illidari Highlord
    {-4631.7808f, 1367.9459f, 139.9f, 0.020f}, //Illidari Highlord
    {-4615.5586f, 1353.0031f, 139.9f, 1.540f}, //Illidari Highlord
    {-4616.4736f, 1384.2170f, 139.9f, 4.971f}, //Illidari Highlord
    {-4627.1240f, 1378.8752f, 139.9f, 2.544f} //Torloth The Magnificent
};

struct WaveData
{
    uint8 SpawnCount, UsedSpawnPoint;
    uint32 CreatureId, SpawnTimer, YellTimer;
    int32 WaveTextId;
};

static WaveData WavesInfo[] =
{
    {9, 0, 22075, 10000, 7000, -1000371}, // Illidari Soldier
    {2, 9, 22074, 10000, 7000, -1000372}, // Illidari Mind Breaker
    {4, 11, 19797, 10000, 7000, -1000373}, // Illidari Highlord
    {1, 15, 22076, 10000, 7000, -1000374} // Torloth The Magnificent
};

struct SpawnSpells
{
 uint32 Timer1, Timer2, SpellId;
};

static SpawnSpells SpawnCast[] =
{
    {10000, 15000, 35871}, // Illidari Soldier Cast - Spellbreaker
    {10000, 10000, 38985}, // Illidari Mind Breake Cast - Focused Bursts
    {35000, 35000, 22884}, // Illidari Mind Breake Cast - Psychic Scream
    {20000, 20000, 17194}, // Illidari Mind Breake Cast - Mind Blast
    {8000, 15000, 38010}, // Illidari Highlord Cast - Curse of Flames
    {12000, 20000, 16102}, // Illidari Highlord Cast - Flamestrike
    {10000, 15000, 15284}, // Torloth the Magnificent Cast - Cleave
    {18000, 20000, 39082}, // Torloth the Magnificent Cast - Shadowfury
    {25000, 28000, 33961} // Torloth the Magnificent Cast - Spell Reflection
};

class npc_torloth_the_magnificent : public CreatureScript
{
public:
    npc_torloth_the_magnificent() : CreatureScript("npc_torloth_the_magnificent") { }

    struct npc_torloth_the_magnificentAI : public QuantumBasicAI
    {
        npc_torloth_the_magnificentAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 AnimationTimer, SpellTimer1, SpellTimer2, SpellTimer3;

        uint8 AnimationCount;

        uint64 LordIllidanGUID;
        uint64 AggroTargetGUID;

        bool Timers;

        void Reset()
        {
            AnimationTimer = 4000;
            AnimationCount = 0;
            LordIllidanGUID = 0;
            AggroTargetGUID = 0;
            Timers = false;

            me->AddUnitState(UNIT_STATE_ROOT);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetTarget(0);
		}

        void EnterToBattle(Unit* /*who*/){}

        void HandleAnimation()
        {
            Creature* creature = me;

            if (TorlothAnim[AnimationCount].creature == 1)
            {
                creature = (Unit::GetCreature(*me, LordIllidanGUID));

                if (!creature)
                    return;
            }

            if (TorlothAnim[AnimationCount].TextId)
                DoSendQuantumText(TorlothAnim[AnimationCount].TextId, creature);

            AnimationTimer = TorlothAnim[AnimationCount].Timer;

            switch (AnimationCount)
            {
            case 0:
                me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_KNEEL);
                break;
            case 3:
                me->RemoveFlag(UNIT_FIELD_BYTES_1, 8);
                break;
            case 5:
                if (Player* AggroTarget = (Unit::GetPlayer(*me, AggroTargetGUID)))
                {
                    me->SetTarget(AggroTarget->GetGUID());
                    me->AddThreat(AggroTarget, 1);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                }
                break;
            case 6:
                if (Player* AggroTarget = (Unit::GetPlayer(*me, AggroTargetGUID)))
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    me->ClearUnitState(UNIT_STATE_ROOT);

                    float x, y, z;
                    AggroTarget->GetPosition(x, y, z);
                    me->GetMotionMaster()->MovePoint(0, x, y, z);
                }
                break;
            }
            ++AnimationCount;
        }

        void UpdateAI(const uint32 diff)
        {
            if (AnimationTimer)
            {
                if (AnimationTimer <= diff)
                {
                    HandleAnimation();
                }
				else AnimationTimer -= diff;
            }

            if (AnimationCount < 6)
            {
                me->CombatStop();
            } 
			else if (!Timers)
            {
				SpellTimer1 = SpawnCast[6].Timer1;
                SpellTimer2 = SpawnCast[7].Timer1;
                SpellTimer3 = SpawnCast[8].Timer1;
                Timers = true;
            }

            if (Timers)
            {
                if (SpellTimer1 <= diff)
                {
                    DoCastVictim(SpawnCast[6].SpellId); // Cleave
                    SpellTimer1 = SpawnCast[6].Timer2 + (rand()%10 * 1000);
                }
				else SpellTimer1 -= diff;

                if (SpellTimer2 <= diff)
                {
                    DoCastVictim(SpawnCast[7].SpellId); // Shadowfury
                    SpellTimer2 = SpawnCast[7].Timer2 + (rand()%5 * 1000);
                }
				else SpellTimer2 -= diff;

                if (SpellTimer3 <= diff)
                {
                    DoCast(me, SpawnCast[8].SpellId);
                    SpellTimer3 = SpawnCast[8].Timer2 + (rand()%7 * 1000); // Spell Reflection
                }
				else SpellTimer3 -= diff;
            }

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* slayer)
        {
            if (slayer)
                switch (slayer->GetTypeId())
                {
                    case TYPE_ID_UNIT:
						if (Unit* owner = slayer->GetOwner())
							if (owner->GetTypeId() == TYPE_ID_PLAYER)
								CAST_PLR(owner)->GroupEventHappens(QUEST_BATTLE_OF_THE_CRIMSON_WATCH, me);
						break;
                    case TYPE_ID_PLAYER:
                        CAST_PLR(slayer)->GroupEventHappens(QUEST_BATTLE_OF_THE_CRIMSON_WATCH, me);
                        break;
                    default:
                        break;
			}
			
			if (Creature* LordIllidan = (Unit::GetCreature(*me, LordIllidanGUID)))
            {
                DoSendQuantumText(END_TEXT, LordIllidan, slayer);
                LordIllidan->AI()->EnterEvadeMode();
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_torloth_the_magnificentAI(creature);
    }
};

class npc_lord_illidan_stormrage : public CreatureScript
{
public:
    npc_lord_illidan_stormrage() : CreatureScript("npc_lord_illidan_stormrage") { }

    struct npc_lord_illidan_stormrageAI : public QuantumBasicAI
    {
        npc_lord_illidan_stormrageAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint64 PlayerGUID;

        uint32 WaveTimer;
        uint32 AnnounceTimer;

        int8 LiveCount;
        uint8 WaveCount;

        bool EventStarted;
        bool Announced;
        bool Failed;

        void Reset()
        {
            PlayerGUID = 0;

            WaveTimer = 10000;
            AnnounceTimer = 7000;
            LiveCount = 0;
            WaveCount = 0;

            EventStarted = false;
            Announced = false;
            Failed = false;

            me->SetVisible(false);
        }

        void EnterToBattle(Unit* /*who*/){}

        void MoveInLineOfSight(Unit* /*who*/){}

        void AttackStart(Unit* /*who*/){}

        void SummonNextWave();

        void CheckEventFail()
        {
            Player* player = Unit::GetPlayer(*me, PlayerGUID);

            if (!player)
                return;

            if (Group* EventGroup = player->GetGroup())
            {
                Player* GroupMember;

                uint8 GroupMemberCount = 0;
                uint8 DeadMemberCount = 0;
                uint8 FailedMemberCount = 0;

                const Group::MemberSlotList members = EventGroup->GetMemberSlots();

                for (Group::member_citerator itr = members.begin(); itr!= members.end(); ++itr)
                {
                    GroupMember = (Unit::GetPlayer(*me, itr->guid));
                    if (!GroupMember)
                        continue;
                    if (!GroupMember->IsWithinDistInMap(me, EVENT_AREA_RADIUS) && GroupMember->GetQuestStatus(QUEST_BATTLE_OF_THE_CRIMSON_WATCH) == QUEST_STATUS_INCOMPLETE)
                    {
                        GroupMember->FailQuest(QUEST_BATTLE_OF_THE_CRIMSON_WATCH);
                        ++FailedMemberCount;
                    }
                    ++GroupMemberCount;

                    if (GroupMember->IsDead())
                    {
                        ++DeadMemberCount;
                    }
                }

                if (GroupMemberCount == FailedMemberCount)
                {
                    Failed = true;
                }

                if (GroupMemberCount == DeadMemberCount)
                {
                    for (Group::member_citerator itr = members.begin(); itr!= members.end(); ++itr)
                    {
                        GroupMember = Unit::GetPlayer(*me, itr->guid);

                        if (GroupMember && GroupMember->GetQuestStatus(QUEST_BATTLE_OF_THE_CRIMSON_WATCH) == QUEST_STATUS_INCOMPLETE)
                        {
                            GroupMember->FailQuest(QUEST_BATTLE_OF_THE_CRIMSON_WATCH);
                        }
                    }
                    Failed = true;
                }
            }
			else if (player->IsDead() || !player->IsWithinDistInMap(me, EVENT_AREA_RADIUS))
            {
                player->FailQuest(QUEST_BATTLE_OF_THE_CRIMSON_WATCH);
                Failed = true;
            }
        }

        void LiveCounter()
        {
            --LiveCount;
            if (!LiveCount)
                Announced = false;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!PlayerGUID || !EventStarted)
                return;

            if (!LiveCount && WaveCount < 4)
            {
                if (!Announced && AnnounceTimer <= diff)
                {
                    DoSendQuantumText(WavesInfo[WaveCount].WaveTextId, me);
                    Announced = true;
                }
				else AnnounceTimer -= diff;

                if (WaveTimer <= diff)
                {
                    SummonNextWave();
                }
				else WaveTimer -= diff;
            }
            CheckEventFail();

            if (Failed)
                EnterEvadeMode();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lord_illidan_stormrageAI(creature);
    }
};

class npc_illidari_spawn : public CreatureScript
{
public:
    npc_illidari_spawn() : CreatureScript("npc_illidari_spawn") { }

    struct npc_illidari_spawnAI : public QuantumBasicAI
    {
        npc_illidari_spawnAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint64 LordIllidanGUID;
        uint32 SpellTimer1, SpellTimer2, SpellTimer3;
        bool Timers;

        void Reset()
        {
            LordIllidanGUID = 0;
            Timers = false;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void JustDied(Unit* /*killer*/)
        {
            me->RemoveCorpse();
            if (Creature* LordIllidan = (Unit::GetCreature(*me, LordIllidanGUID)))
                if (LordIllidan)
                    CAST_AI(npc_lord_illidan_stormrage::npc_lord_illidan_stormrageAI, LordIllidan->AI())->LiveCounter();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (!Timers)
            {
                if (me->GetEntry() == 22075)// Illidari Soldier
                {
                    SpellTimer1 = SpawnCast[0].Timer1 + (rand()%4 * 1000);
                }
                if (me->GetEntry() == 22074)// Illidari Mind Breaker
                {
                    SpellTimer1 = SpawnCast[1].Timer1 + (rand()%10 * 1000);
                    SpellTimer2 = SpawnCast[2].Timer1 + (rand()%4 * 1000);
                    SpellTimer3 = SpawnCast[3].Timer1 + (rand()%4 * 1000);
                }
                if (me->GetEntry() == 19797)// Illidari Highlord
                {
                    SpellTimer1 = SpawnCast[4].Timer1 + (rand()%4 * 1000);
                    SpellTimer2 = SpawnCast[5].Timer1 + (rand()%4 * 1000);
                }
                Timers = true;
            }
            //Illidari Soldier
            if (me->GetEntry() == 22075)
            {
                if (SpellTimer1 <= diff)
                {
                    DoCastVictim(SpawnCast[0].SpellId); // Spellbreaker
                    SpellTimer1 = SpawnCast[0].Timer2 + (rand()%5 * 1000);
                }
				else SpellTimer1 -= diff;
            }
            // Illidari Mind Breaker
            if (me->GetEntry() == 22074)
            {
                if (SpellTimer1 <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                    {
                        if (target->GetTypeId() == TYPE_ID_PLAYER)
                        {
                            DoCast(target, SpawnCast[1].SpellId); // Focused Bursts
                            SpellTimer1 = SpawnCast[1].Timer2 + (rand()%5 * 1000);
                        }
						else SpellTimer1 = 2000;
                    }
                }
				else SpellTimer1 -= diff;

                if (SpellTimer2 <= diff)
                {
                    DoCastVictim(SpawnCast[2].SpellId); // Psychic Scream
                    SpellTimer2 = SpawnCast[2].Timer2 + (rand()%13 * 1000);
                }
				else SpellTimer2 -= diff;

                if (SpellTimer3 <= diff)
                {
                    DoCastVictim(SpawnCast[3].SpellId);//Mind Blast
                    SpellTimer3 = SpawnCast[3].Timer2 + (rand()%8 * 1000);
                }
				else SpellTimer3 -= diff;
            }
            //Illidari Highlord
            if (me->GetEntry() == 19797)
            {
                if (SpellTimer1 <= diff)
                {
                    DoCastVictim(SpawnCast[4].SpellId);//Curse Of Flames
                    SpellTimer1 = SpawnCast[4].Timer2 + (rand()%10 * 1000);
                }
				else SpellTimer1 -= diff;

                if (SpellTimer2 <= diff)
                {
                    DoCastVictim(SpawnCast[5].SpellId);//Flamestrike
                    SpellTimer2 = SpawnCast[5].Timer2 + (rand()%7 * 13000);
                }
				else SpellTimer2 -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_illidari_spawnAI(creature);
    }
};

void npc_lord_illidan_stormrage::npc_lord_illidan_stormrageAI::SummonNextWave()
{
    uint8 count = WavesInfo[WaveCount].SpawnCount;
    uint8 locIndex = WavesInfo[WaveCount].UsedSpawnPoint;
    uint8 FelguardCount = 0;
    uint8 DreadlordCount = 0;

    for (uint8 i = 0; i < count; ++i)
    {
        Creature* Spawn = NULL;
        float X = SpawnLocation[locIndex + i].x;
        float Y = SpawnLocation[locIndex + i].y;
        float Z = SpawnLocation[locIndex + i].z;
        float O = SpawnLocation[locIndex + i].o;
        Spawn = me->SummonCreature(WavesInfo[WaveCount].CreatureId, X, Y, Z, O, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
        ++LiveCount;

        if (Spawn)
        {
            Spawn->LoadCreaturesAddon();

            if (WaveCount == 0)//1 Wave
            {
                if (rand()%3 == 1 && FelguardCount<2)
                {
                    Spawn->SetDisplayId(18654);
                    ++FelguardCount;
                }
                else if (DreadlordCount < 3)
                {
                    Spawn->SetDisplayId(19991);
                    ++DreadlordCount;
                }
                else if (FelguardCount<2)
                {
                    Spawn->SetDisplayId(18654);
                    ++FelguardCount;
                }
            }

            if (WaveCount < 3)//1-3 Wave
            {
                if (PlayerGUID)
                {
                    if (Player* target = Unit::GetPlayer(*me, PlayerGUID))
                    {
                        float x, y, z;
                        target->GetPosition(x, y, z);
                        Spawn->GetMotionMaster()->MovePoint(0, x, y, z);
                    }
                }
                CAST_AI(npc_illidari_spawn::npc_illidari_spawnAI, Spawn->AI())->LordIllidanGUID = me->GetGUID();
            }

            if (WavesInfo[WaveCount].CreatureId == 22076) // Torloth
            {
                CAST_AI(npc_torloth_the_magnificent::npc_torloth_the_magnificentAI, Spawn->AI())->LordIllidanGUID = me->GetGUID();
                if (PlayerGUID)
                    CAST_AI(npc_torloth_the_magnificent::npc_torloth_the_magnificentAI, Spawn->AI())->AggroTargetGUID = PlayerGUID;
            }
        }
    }
    ++WaveCount;
    WaveTimer = WavesInfo[WaveCount].SpawnTimer;
    AnnounceTimer = WavesInfo[WaveCount].YellTimer;
}

/*#####
# go_crystal_prison
######*/

class go_crystal_prison : public GameObjectScript
{
public:
    go_crystal_prison() : GameObjectScript("go_crystal_prison") { }

    bool OnQuestAccept(Player* player, GameObject* /*go*/, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_BATTLE_OF_THE_CRIMSON_WATCH)
        {
            Creature* Illidan = player->FindCreatureWithDistance(22083, 50);

            if (Illidan && !CAST_AI(npc_lord_illidan_stormrage::npc_lord_illidan_stormrageAI, Illidan->AI())->EventStarted)
            {
                CAST_AI(npc_lord_illidan_stormrage::npc_lord_illidan_stormrageAI, Illidan->AI())->PlayerGUID = player->GetGUID();
                CAST_AI(npc_lord_illidan_stormrage::npc_lord_illidan_stormrageAI, Illidan->AI())->LiveCount = 0;
                CAST_AI(npc_lord_illidan_stormrage::npc_lord_illidan_stormrageAI, Illidan->AI())->EventStarted=true;
            }
        }
		return true;
    }
};

enum EnragedSpirits
{
	QUEST_ENRAGED_SPIRITS_FIRE_EARTH = 10458,
	QUEST_ENRAGED_SPIRITS_WATER      = 10480,
	QUEST_ENRAGED_SPIRITS_AIR        = 10481,

	RADIUS_TOTEM_OF_SPIRITS          = 15,

	NPC_TOTEM_OF_SPIRITS             = 21071,
	NPC_ENRAGED_EARTH_SPIRIT         = 21050,
	NPC_ENRAGED_FIRE_SPIRIT          = 21061,
	NPC_ENRAGED_AIR_SPIRIT           = 21060,
	NPC_ENRAGED_WATER_SPIRIT         = 21059,
	NPC_EARTHEN_SOUL                 = 21073,
	NPC_FIERY_SOUL                   = 21097,
	NPC_ENRAGED_WATERY_SOUL          = 21109,
	NPC_ENRAGED_AIRY_SOUL            = 21116,
	NPC_KILL_CREDIT_FIRE             = 21094,
	NPC_KILL_CREDIT_WATER            = 21095,
	NPC_KILL_CREDIT_AIR              = 21096,
	NPC_KILL_CREDIT_EARTH            = 21092,

	SPELL_EARTH_SPIRIT_DEATH         = 36015,
	SPELL_EARTHEN_SOUL_CREDIT        = 36108,
	SPELL_FIERY_SOUL_CREDIT          = 36117,
	SPELL_AIRY_SOUL_CREDIT           = 36182,
	SPELL_WATERY_SOUL_CREDIT         = 36171,
	SPELL_SOUL_CAPTURED              = 36115,
	SPELL_COSMETIC_STEAM_AURA        = 36151,
	SPELL_FELFIRE_AURA               = 36006,
	SPELL_STORMBOLT                  = 38032,

	FACTION_SOUL_FRIENDLY            = 35,
	FACTION_SOUL_HOSTILE             = 16,
};

class npc_enraged_spirit : public CreatureScript
{
public:
	npc_enraged_spirit() : CreatureScript("npc_enraged_spirit") { }

    struct npc_enraged_spiritAI : public QuantumBasicAI
    {
		npc_enraged_spiritAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 StormboltTimer;

        void Reset()
		{
			StormboltTimer = 1000;

			DoCast(me, SPELL_COSMETIC_STEAM_AURA);
			DoCast(me, SPELL_FELFIRE_AURA);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void EnterToBattle(Unit* /*who*/){}

        void JustDied(Unit* /*killer*/)
        {
			DoCast(me, SPELL_EARTH_SPIRIT_DEATH, true);

			uint32 entry = 0;
            uint32 credit = 0;
			
			switch (me->GetEntry())
			{
                case NPC_ENRAGED_FIRE_SPIRIT:
					entry = NPC_FIERY_SOUL;
					credit = NPC_KILL_CREDIT_FIRE;
					break;
				case NPC_ENRAGED_EARTH_SPIRIT:
					entry = NPC_EARTHEN_SOUL;
					credit = NPC_KILL_CREDIT_EARTH;
					break;
				case NPC_ENRAGED_AIR_SPIRIT:
					entry = NPC_ENRAGED_AIRY_SOUL;
					credit = NPC_KILL_CREDIT_AIR;
					break;
				case NPC_ENRAGED_WATER_SPIRIT:
					entry = NPC_ENRAGED_WATERY_SOUL;
					credit = NPC_KILL_CREDIT_WATER;
					break;
			}

            Creature* Summoned = NULL;
            Unit* totemOspirits = NULL;

			if (entry != 0)
                Summoned = DoSpawnCreature(entry, 0, 0, 1, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 5000);

			if (Summoned)
            {
				totemOspirits = me->FindCreatureWithDistance(NPC_TOTEM_OF_SPIRITS, RADIUS_TOTEM_OF_SPIRITS);
				if (totemOspirits)
				{
					Summoned->SetCurrentFaction(FACTION_SOUL_FRIENDLY);
					Summoned->GetMotionMaster()->MovePoint(0, totemOspirits->GetPositionX(), totemOspirits->GetPositionY(), Summoned->GetPositionZ());

					Unit* Owner = totemOspirits->GetOwner();
					if (Owner && Owner->GetTypeId() == TYPE_ID_PLAYER)
						CAST_PLR(Owner)->KilledMonsterCredit(credit, 0);
					DoCast(totemOspirits, SPELL_SOUL_CAPTURED);
				}
			}
		}

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StormboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_STORMBOLT);
					StormboltTimer = 3000;
				}
			}
			else StormboltTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_enraged_spiritAI(creature);
    }
};

enum SpectreclesMisc
{    
	SPELL_REPLACE_LOST_SPECTRACLES = 37700,
	ITEM_SPECTRECLES               = 30721,
    QUEST_T_G_LORE_AND_LEGEND_A    = 10819,
    QUEST_T_G_LORE_AND_LEGEND_H    = 10633,
    QUEST_T_G_I_AM_A               = 10645,
    QUEST_T_G_I_AM_H               = 10639,
};

#define GOSSIP_ITEM_SPECTRACLES "Please, give me new Spectrecles."

class npc_item_spectrecles : public CreatureScript
{
    public:
        npc_item_spectrecles() : CreatureScript("npc_item_spectrecles") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
			if (creature->IsQuestGiver())
				player->PrepareQuestMenu(creature->GetGUID());

            if (!player->HasItemCount(ITEM_SPECTRECLES, 1, true)
				&& (((player->GetQuestStatus(QUEST_T_G_LORE_AND_LEGEND_A) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_T_G_LORE_AND_LEGEND_A) == QUEST_STATUS_REWARDED) && !player->GetQuestRewardStatus(QUEST_T_G_I_AM_A))
				|| ((player->GetQuestStatus(QUEST_T_G_LORE_AND_LEGEND_H) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_T_G_LORE_AND_LEGEND_H) == QUEST_STATUS_REWARDED) && !player->GetQuestRewardStatus(QUEST_T_G_I_AM_H))))
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_SPECTRACLES, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 0);

			player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
			return true;
		}

        bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF + 0:
                    player->CLOSE_GOSSIP_MENU();
                    player->CastSpell(player, SPELL_REPLACE_LOST_SPECTRACLES);
                    break;
            }
            return true;
		}
};

enum ItemPathOfConquestMisc
{    
	NPC_ORBAROKH                          = 21769,
	NPC_YOREGAR                           = 21773,

	ITEM_WILDHAMMER_FLARE_GUN             = 31310,
	ITEM_KORKRON_FLARE_GUN                = 31108,

	QUEST_PATH_OF_CONQUEST_A              = 10772,
	QUEST_PATH_OF_CONQUEST_H              = 10750,
	QUEST_DISSENSION_AMONGST_THE_RANKS_A  = 10776,
	QUEST_DISSENSION_AMONGST_THE_RANKS_H  = 10769,
 };

#define GOSSIP_ITEM_FLARE_GUN "Please, give me new Flare Gun."

class npc_item_path_of_conquest : public CreatureScript
{
    public:
        npc_item_path_of_conquest() : CreatureScript("npc_item_path_of_conquest") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
			if (creature->IsQuestGiver())
				player->PrepareQuestMenu(creature->GetGUID());

            if ((!player->HasItemCount(ITEM_WILDHAMMER_FLARE_GUN, 1, true) || !player->HasItemCount(ITEM_KORKRON_FLARE_GUN, 1, true)) 
                && (((player->GetQuestStatus(QUEST_PATH_OF_CONQUEST_A) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_PATH_OF_CONQUEST_A) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_PATH_OF_CONQUEST_A) == QUEST_STATUS_REWARDED) && !player->GetQuestRewardStatus(QUEST_DISSENSION_AMONGST_THE_RANKS_A))
                || ((player->GetQuestStatus(QUEST_PATH_OF_CONQUEST_H) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_PATH_OF_CONQUEST_H) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_PATH_OF_CONQUEST_H) == QUEST_STATUS_REWARDED) && !player->GetQuestRewardStatus(QUEST_DISSENSION_AMONGST_THE_RANKS_H))))
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FLARE_GUN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 0);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();

            if (action == GOSSIP_ACTION_INFO_DEF + 0)
            {
                switch (creature->GetEntry())
                {
                    case NPC_YOREGAR:
                        player->AddItem(ITEM_WILDHAMMER_FLARE_GUN, 1);
                        break;
                    case NPC_ORBAROKH:
						player->AddItem(ITEM_KORKRON_FLARE_GUN, 1);
						break;
                }
				player->CLOSE_GOSSIP_MENU();                    
            }
			return true;
		}
};

enum Azaloth
{
	NPC_SUNFURY_WARLOCK       = 21503,
	NPC_AZALOTH_CREDIT        = 21892,

	SPELL_UNBANISH            = 37834,
	SPELL_SPELLBIND_AZALOTH   = 38722,
	SPELL_AZALOTH_CLEAVE      = 40504,
	SPELL_AZALOTH_WARSTOMP    = 38750,
	SPELL_AZALOTH_RAINOFFIRE  = 38741,
	SPELL_AZALOTH_CRIPPLE     = 11443,
};

class npc_azaloth : public CreatureScript
{
public:
    npc_azaloth() : CreatureScript("npc_azaloth") { }

    struct  npc_azalothAI : QuantumBasicAI
    {
        npc_azalothAI(Creature* creature) : QuantumBasicAI(creature)
        {
			isBanished = true;
        }

        bool isBanished;

        uint32 BanishTimer;
        uint32 CleaveTimer;
        uint32 WarstompTimer;
        uint32 RainOfFireTimer;
        uint32 CrippleTimer;

        void Reset()
        {
            BanishTimer = 30000;
            CleaveTimer = 5000;
            WarstompTimer = 10000;
            RainOfFireTimer = 15000;
            CrippleTimer = 15000;
        }

        void EnterToBattle(Unit* /*who*/)
        {
			if (!isBanished)
				me->RemoveAllAuras();
        }

        void DamageTaken(Unit* /*Doneby*/, uint32 &damage)
        {
            if (isBanished)
				damage = 0;
        }

        void SpellHit(Unit* caster, const SpellInfo *spell)
        {
            if (!isBanished)
                return;

            if (spell->Id == SPELL_UNBANISH)
            {
                if (caster->GetTypeId() == TYPE_ID_PLAYER)
					((Player*)caster)->CastedCreatureOrGO(NPC_AZALOTH_CREDIT, me->GetGUID(), SPELL_UNBANISH);
                isBanished = false;
            }
        }

        void AttackStart(Unit* who)
        {
            if (!isBanished)
                QuantumBasicAI::AttackStart(who);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!isBanished)
            {
                if (BanishTimer <= diff)
                {
                    std::list<Creature*> list;
                    GetCreatureListWithEntryInGrid(list, me, NPC_SUNFURY_WARLOCK, 20.0f);

                    for (std::list<Creature*>::iterator iter = list.begin(); iter != list.end(); ++iter)
                    {
                        Creature* creature = *iter;
                        if (creature)
                        {
                            if (creature->IsDead())
                            {
								creature->Respawn(true);
								creature->CastSpell(me, SPELL_SPELLBIND_AZALOTH, true);
                            }
                        }
                    }
                    isBanished = true;
                    BanishTimer = 30000;
                }
				else BanishTimer -= diff;
            }

            if (!UpdateVictim())
                return;

            if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_AZALOTH_CLEAVE);
                CleaveTimer = 10000;
            }
			else CleaveTimer -= diff;

            if (WarstompTimer <= diff)
            {
                DoCast(me, SPELL_AZALOTH_WARSTOMP);
                WarstompTimer = 15000;
            }
			else WarstompTimer -= diff;

            if (RainOfFireTimer <= diff)
            {
                if (Unit *target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_AZALOTH_RAINOFFIRE);
					RainOfFireTimer = 20000;
				}
            }
			else RainOfFireTimer -= diff;

            if (CrippleTimer <= diff)
            {
                if (Unit *target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_AZALOTH_CRIPPLE);
					CrippleTimer = 10000;
				}
            }
			else CrippleTimer -= diff;

            DoMeleeAttackIfReady();

            BanishTimer = 30000;
        }

		CreatureAI* GetAI(Creature* creature) const
		{
			return new npc_azalothAI(creature);
		}
    };
};

enum ZuluhedChains
{
	QUEST_ZULUHED = 10866,
	NPC_KARYNAKU  = 22112,
};

class spell_unlocking_zuluheds_chains : public SpellScriptLoader
{
    public:
        spell_unlocking_zuluheds_chains() : SpellScriptLoader("spell_unlocking_zuluheds_chains") { }

        class spell_unlocking_zuluheds_chains_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_unlocking_zuluheds_chains_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            void HandleAfterHit()
            {
                Player* caster = GetCaster()->ToPlayer();

                if (caster->GetQuestStatus(QUEST_ZULUHED) == QUEST_STATUS_INCOMPLETE)
                    caster->KilledMonsterCredit(NPC_KARYNAKU, 0);
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_unlocking_zuluheds_chains_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_unlocking_zuluheds_chains_SpellScript();
        }
};

enum ShadowMoonTuber
{
    SPELL_WHISTLE              = 36652,
    SPELL_SHADOWMOON_TUBER     = 36462,

    NPC_BOAR                   = 21195,
    GO_SHADOWMOON_TUBER_MOUND  = 184701,

    POINT_TUBER                 = 1,
    TYPE_BOAR                   = 1,
    DATA_BOAR                   = 1,
};

class npc_shadowmoon_tuber_node : public CreatureScript
{
public:
    npc_shadowmoon_tuber_node() : CreatureScript("npc_shadowmoon_tuber_node") {}

    struct npc_shadowmoon_tuber_nodeAI : public QuantumBasicAI
    {
        npc_shadowmoon_tuber_nodeAI(Creature* creature) : QuantumBasicAI(creature) {}

		bool Tapped;
        uint64 TuberGUID;
        uint32 ResetTimer;

        void Reset()
        {
            Tapped = false;
            TuberGUID = 0;
            ResetTimer = 60000;
        }

        void SetData(uint32 id, uint32 data)
        {
            if (id == TYPE_BOAR && data == DATA_BOAR)
            {
                DoCast(SPELL_SHADOWMOON_TUBER);

                if (GameObject* tuber = me->FindGameobjectWithDistance(GO_SHADOWMOON_TUBER_MOUND, 5.0f))
                {
                    TuberGUID = tuber->GetGUID();
                    tuber->SetPhaseMask(2, true);
                }
            }
        }

        void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (!Tapped && spell->Id == SPELL_WHISTLE)
            {
                if (Creature* boar = me->FindCreatureWithDistance(NPC_BOAR, 30.0f))
                {
                    Tapped = true;
                    boar->SetWalk(false);
                    boar->GetMotionMaster()->MovePoint(POINT_TUBER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (Tapped)
            {
                if (ResetTimer <= diff)
                {
                    if (TuberGUID)
                        if (GameObject* tuber = GameObject::GetGameObject(*me, TuberGUID))
                            tuber->SetPhaseMask(1, true);

                    Reset();
                }
                else ResetTimer -= diff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadowmoon_tuber_nodeAI(creature);
    }
};

enum QuestTheSecondCourse
{
	SPELL_FEL_FIREBALL      = 37945,
	SPELL_FLAMING_WOUND     = 37941,
	SPELL_CHANNEL           = 36854,
	SPELL_DIEMETRADON       = 37851,

	QUEST_THE_SECOND_COURSE = 10672,
};

class npc_greater_felfire_diemetradon : public CreatureScript
{
public:
    npc_greater_felfire_diemetradon() : CreatureScript("npc_greater_felfire_diemetradon") {}

    struct npc_greater_felfire_diemetradonAI : public QuantumBasicAI
    {
        npc_greater_felfire_diemetradonAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FelFireballTimer;
		uint32 FlamingWoundTimer;

        void Reset()
        {
			FelFireballTimer = 1000;
			FlamingWoundTimer = 3000;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_DIEMETRADON)
            {
				if (Player* player = caster->ToPlayer())
					caster->CastSpell(caster, SPELL_DIEMETRADON, true);

				me->DespawnAfterAction(1.5*IN_MILLISECONDS);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FelFireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FEL_FIREBALL);
					FelFireballTimer = urand(3000, 4000);
				}
			}
			else FelFireballTimer -= diff;

			if (FlamingWoundTimer <= diff)
			{
				DoCastVictim(SPELL_FLAMING_WOUND);
				FlamingWoundTimer = urand(5000, 6000);
			}
			else FlamingWoundTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_greater_felfire_diemetradonAI(creature);
    }
};

float EventPositions[5][3] =
{
    {-3560.477f, 512.431f, 19.2f},
    {-3572.920f, 662.789f, -1.0f},
    {-3572.825f, 621.806f, 6.3f},
    {-3561.880f, 598.995f, 9.3f},
    {-3604.344f, 343.222f, 39.2f},
};

enum XiriPhases
{
    PHASE_SUMMON_DEMONS = 0,
    PHASE_SUMMON_AKAMA  = 1,
    PHASE_ATTACK        = 2,
    PHASE_KILL_WAIT     = 3,
    PHASE_END           = 4,
    PHASE_WAIT          = 5,
};

enum Entries
{
	NPC_VAGATH                  = 23152,
	NPC_ILLIDARI_SHADOWLORD     = 22988,
	NPC_AKAMA                   = 23191,
	NPC_MAIEV_SHADOWSONG        = 22989,
	NPC_ASHTONGUE_DEATHSWORN    = 21701,
	SPELL_SHADOW_STRIKE         = 40685,
	SPELL_THROW_DAGGERS         = 41152,
	QUEST_A_DISTRACTION_AKAMA   = 10985,
	QUEST_FOR_DISTRACTION_AKAMA = 13429,
};

#define XIRI_OPTION "I am ready"

#define YELL_VAGATH_AGGRO "You've sealed your fate, Akama. The Master will learn of your betrayal!"
#define YELL_AKAMA_INTRO "I will deal with these mongrels! Strike now, friends! Strike at the Betrayer!"

class npc_xiri : public CreatureScript
{
public:
    npc_xiri() : CreatureScript("npc_xiri") { }

    CreatureAI* GetAI(Creature* _Creature) const
    {
        return new npc_xiriAI(_Creature);
    }

struct npc_xiriAI : public QuantumBasicAI
{
    npc_xiriAI(Creature* c) : QuantumBasicAI(c)
    {
        EventStarted = false;
    }

    uint64 EventStarter;
    bool EventStarted;

    XiriPhases Phase;
    XiriPhases NextPhase;

    uint32 Wait_Timer;
    Creature* temp;

    uint64 Akama;
    uint64 Maiev;
    uint64 Vagath;
    uint64 Ashtongue[5];
    uint64 Demons[6];

    void Reset()
    {
        //EventStarter = 0;
        //EventStarted = false;
    }

    void MoveInLineOfSight(Unit* /*who*/){}

    void StartEvent(Player* starter)
    {
        EventStarted = true;
        EventStarter = starter->GetGUID();

        me->SetActive(true);

        Phase = PHASE_WAIT;
        NextPhase = PHASE_SUMMON_DEMONS;
        Wait_Timer = 10000;

        me->SetUInt32Value(UNIT_NPC_FLAGS,0);
    }

    bool AreDemonsDead()
    {
        for (int i = 0; i < 6; i++)
        {
            Creature* c_Demon = Creature::GetCreature((*me),Demons[i]);
            if (c_Demon && c_Demon->IsAlive())
                return false;
        }
        return true;
    }

    void ForceDespawn(Creature* c_creature)
    {
        c_creature->GetMotionMaster()->MoveIdle();
        c_creature->SetVisible(false);
        c_creature->DealDamage(c_creature, c_creature->GetHealth());
        c_creature->RemoveCorpse();

    }

    void EnterToBattle(Unit* /*who*/){}

    void EventHappens(Player* player, uint32 questId)
    {
        if (Group* pGroup = player->GetGroup())
        {
            for (GroupReference* itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player* pGroupGuy = itr->getSource();

                if (pGroupGuy && pGroupGuy->IsWithinDistInMap(me, 500.0f) && !pGroupGuy->GetCorpse())
                    pGroupGuy->AreaExploredOrEventHappens(questId);
            }
        }
        else player->AreaExploredOrEventHappens(questId);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!EventStarted)
            return;
        {
            Creature* c_Vagath;
            Creature* c_Akama;
            Creature* c_Maiev;

            switch(Phase)
            {
            case PHASE_SUMMON_DEMONS:
                temp = me->SummonCreature(NPC_VAGATH, EventPositions[0][0], EventPositions[0][1], EventPositions[0][2], M_PI*0.5, TEMPSUMMON_TIMED_DESPAWN, 300000);
                temp->SetActive(true);
				temp->MonsterYell(YELL_VAGATH_AGGRO, LANG_UNIVERSAL, 0);
                Vagath = temp->GetGUID();

                temp = me->SummonCreature(NPC_ILLIDARI_SHADOWLORD,EventPositions[0][0]-17,EventPositions[0][1]+5, EventPositions[0][2]+1, M_PI*0.5,  TEMPSUMMON_TIMED_DESPAWN, 300000);
                temp->SetActive(true);
                Demons[0] = temp->GetGUID();
                temp = me->SummonCreature(NPC_ILLIDARI_SHADOWLORD,EventPositions[0][0]-12,EventPositions[0][1]+5, EventPositions[0][2]+1, M_PI*0.5, TEMPSUMMON_TIMED_DESPAWN, 300000);
                temp->SetActive(true);
                Demons[1] = temp->GetGUID();
                temp = me->SummonCreature(NPC_ILLIDARI_SHADOWLORD,EventPositions[0][0]-7,EventPositions[0][1]+5, EventPositions[0][2]+1, M_PI*0.5, TEMPSUMMON_TIMED_DESPAWN, 300000);
                temp->SetActive(true);
                Demons[2] = temp->GetGUID();

                temp = me->SummonCreature(NPC_ILLIDARI_SHADOWLORD,EventPositions[0][0]+17,EventPositions[0][1]+5, EventPositions[0][2]+1, M_PI*0.5, TEMPSUMMON_TIMED_DESPAWN, 300000);
                temp->SetActive(true);
                Demons[3] = temp->GetGUID();
                temp = me->SummonCreature(NPC_ILLIDARI_SHADOWLORD,EventPositions[0][0]+12,EventPositions[0][1]+5, EventPositions[0][2]+1, M_PI*0.5, TEMPSUMMON_TIMED_DESPAWN, 300000);
                temp->SetActive(true);
                Demons[4] = temp->GetGUID();
                temp = me->SummonCreature(NPC_ILLIDARI_SHADOWLORD,EventPositions[0][0]+7,EventPositions[0][1]+5, EventPositions[0][2]+1, M_PI*0.5, TEMPSUMMON_TIMED_DESPAWN, 300000);
                temp->SetActive(true);
                Demons[5] = temp->GetGUID();

                Phase = PHASE_WAIT;
                NextPhase = PHASE_SUMMON_AKAMA;
                Wait_Timer = 5000;
                break;
            case PHASE_SUMMON_AKAMA:
                temp = me->SummonCreature(NPC_AKAMA, EventPositions[1][0], EventPositions[1][1], EventPositions[1][2],  M_PI*1.5f, TEMPSUMMON_TIMED_DESPAWN, 300000);
                temp->SetActive(true);
				temp->MonsterYell(YELL_AKAMA_INTRO, LANG_UNIVERSAL, 0);
				temp->SetUInt32Value(UNIT_NPC_FLAGS, 0);
                temp->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                temp->GetMotionMaster()->MovePoint(0,EventPositions[2][0],EventPositions[2][1], EventPositions[2][2]);
                Akama = temp->GetGUID();

                temp = me->SummonCreature(NPC_MAIEV_SHADOWSONG, EventPositions[1][0], EventPositions[1][1], EventPositions[1][2],  M_PI*1.5f, TEMPSUMMON_TIMED_DESPAWN, 300000);
                temp->SetActive(true);
                temp->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                SetEquipmentSlots(false, 44850, EQUIP_UNEQUIP, EQUIP_NO_CHANGE);
                me->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, 45738);
                temp->GetMotionMaster()->MovePoint(0,EventPositions[3][0],EventPositions[3][1], EventPositions[3][2]);
                Maiev = temp->GetGUID();

                temp = me->SummonCreature(NPC_ASHTONGUE_DEATHSWORN, EventPositions[1][0], EventPositions[1][1], EventPositions[1][2],  M_PI*1.5f, TEMPSUMMON_TIMED_DESPAWN, 300000);
                temp->SetActive(true);
                temp->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                temp->GetMotionMaster()->MovePoint(0,EventPositions[3][0],EventPositions[3][1]+5,EventPositions[3][2]);
                Ashtongue[0] = temp->GetGUID();

                temp = me->SummonCreature(NPC_ASHTONGUE_DEATHSWORN, EventPositions[1][0], EventPositions[1][1], EventPositions[1][2],  M_PI*1.5f, TEMPSUMMON_TIMED_DESPAWN, 300000);
                temp->SetActive(true);
                temp->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                temp->GetMotionMaster()->MovePoint(0,EventPositions[3][0]+5,EventPositions[3][1]+10,EventPositions[3][2]);
                Ashtongue[1] = temp->GetGUID();

                temp = me->SummonCreature(NPC_ASHTONGUE_DEATHSWORN, EventPositions[1][0], EventPositions[1][1], EventPositions[1][2],  M_PI*1.5f, TEMPSUMMON_TIMED_DESPAWN, 300000);
                temp->SetActive(true);
                temp->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                temp->GetMotionMaster()->MovePoint(0, EventPositions[3][0]-5, EventPositions[3][1]+10, EventPositions[3][2]);
                Ashtongue[2] = temp->GetGUID();

                temp = me->SummonCreature(NPC_ASHTONGUE_DEATHSWORN, EventPositions[1][0], EventPositions[1][1], EventPositions[1][2],  M_PI*1.5f, TEMPSUMMON_TIMED_DESPAWN, 300000);
                temp->SetActive(true);
                temp->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                temp->GetMotionMaster()->MovePoint(0,EventPositions[3][0]+10,EventPositions[3][1]+15,EventPositions[3][2]);
                Ashtongue[3] = temp->GetGUID();

                temp = me->SummonCreature(NPC_ASHTONGUE_DEATHSWORN, EventPositions[1][0], EventPositions[1][1], EventPositions[1][2],  M_PI*1.5f, TEMPSUMMON_TIMED_DESPAWN, 300000);
                temp->SetActive(true);
                temp->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                temp->GetMotionMaster()->MovePoint(0, EventPositions[3][0]-10, EventPositions[3][1]+15, EventPositions[3][2]);
                Ashtongue[4] = temp->GetGUID();

                Phase = PHASE_WAIT;
                NextPhase = PHASE_ATTACK;
                Wait_Timer = 20000;
                break;
            case PHASE_ATTACK:
                c_Vagath = Creature::GetCreature((*me),Vagath);
                c_Akama = Creature::GetCreature((*me),Akama);
                c_Maiev = Creature::GetCreature((*me),Maiev);
                if (c_Vagath && c_Akama && c_Maiev)
                {
                    c_Akama->GetMotionMaster()->MovePoint(0, EventPositions[0][0], EventPositions[0][1]+10, EventPositions[0][2]);
                    c_Akama->SetHomePosition(EventPositions[0][0], EventPositions[0][1]+10, EventPositions[0][2],M_PI*1.5f);
                    c_Maiev->GetMotionMaster()->MovePoint(0, EventPositions[0][0], EventPositions[0][1], EventPositions[0][2]);
                    c_Maiev->SetHomePosition(EventPositions[0][0], EventPositions[0][1], EventPositions[0][2],M_PI*1.5f);
                
                    for (int i = 0; i < 5; i++)
                    {
                        Creature* c_Ashtongue;
                        c_Ashtongue = Creature::GetCreature((*me), Ashtongue[i]);
                        if (c_Ashtongue)
                        {
                            c_Ashtongue->GetMotionMaster()->MovePoint(0, float(EventPositions[0][0]+(rand()%5-2)), float(EventPositions[0][1]+10), float(EventPositions[0][2]+0.5));
                            c_Ashtongue->SetHomePosition(float(EventPositions[0][0]+(rand()%5-2)), float(EventPositions[0][1]+10), float(EventPositions[0][2]+0.5), M_PI*1.5f);
                        }
                    }

                }
                Phase = PHASE_KILL_WAIT;
                break;
            case PHASE_KILL_WAIT:
                c_Vagath = Creature::GetCreature((*me), Vagath);
                if ((!c_Vagath || c_Vagath->IsDead()) && AreDemonsDead())
                {
                    for (int i = 0; i < 5; i++)
                    {
                        Creature* c_Ashtongue;
                        c_Ashtongue = Creature::GetCreature((*me), Ashtongue[i]);
                        if (c_Ashtongue)
                        {
                            c_Ashtongue->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                            c_Ashtongue->GetMotionMaster()->MovePoint(0, EventPositions[4][0], EventPositions[4][1], EventPositions[4][2]);
                            c_Ashtongue->SetHomePosition(EventPositions[4][0], EventPositions[4][1], EventPositions[4][2],M_PI*1.5f);
                        }
                    }

                    c_Akama = Creature::GetCreature((*me),Akama);
                    c_Maiev = Creature::GetCreature((*me),Maiev);
                    if (c_Akama)
                    {
                        c_Akama->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                        c_Akama->GetMotionMaster()->MovePoint(0,EventPositions[4][0],EventPositions[4][1],EventPositions[4][2]);
                        c_Akama->SetHomePosition(EventPositions[4][0],EventPositions[4][1],EventPositions[4][2],M_PI*1.5f);

                    }
                    if (c_Maiev)
                    {
                        c_Maiev->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING);
                        c_Maiev->GetMotionMaster()->MovePoint(0, EventPositions[4][0], EventPositions[4][1], EventPositions[4][2]);
                        c_Maiev->SetHomePosition(EventPositions[4][0], EventPositions[4][1], EventPositions[4][2],M_PI*1.5f);
                    }

                    Phase = PHASE_WAIT;
                    NextPhase = PHASE_END;
                    Wait_Timer = 15000;
                }
                break;
            case PHASE_END:
                c_Akama = Creature::GetCreature((*me),Akama);
                c_Maiev = Creature::GetCreature((*me),Maiev);

                if (c_Akama) ForceDespawn(c_Akama);
                if (c_Maiev) ForceDespawn(c_Maiev);
                for (int i = 0; i < 5; i++)
                {
                    Creature* c_Ashtongue;
                    c_Ashtongue = Creature::GetCreature((*me), Ashtongue[i]);
                    if (c_Ashtongue) ForceDespawn(c_Ashtongue);
                }

                if (Player* starter = Unit::GetPlayer(*me, EventStarter))
                {
                    EventHappens(starter, 10985);
                    EventHappens(starter, 13429);
                }

                me->SetUInt32Value(UNIT_NPC_FLAGS, 3);
                EventStarted = false;
                break;
            case PHASE_WAIT:
                if (Wait_Timer <= diff)
                {
                    Phase = NextPhase;
                    Wait_Timer = 0;
                }
				else Wait_Timer -= diff;
                break;
            }

        }
    }
};

bool OnGossipHello(Player* player, Creature *_Creature)
{
	if (_Creature->IsQuestGiver())
		player->PrepareQuestMenu( _Creature->GetGUID() );

	if ((player->GetQuestStatus(QUEST_A_DISTRACTION_AKAMA) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_FOR_DISTRACTION_AKAMA) == QUEST_STATUS_INCOMPLETE))
		player->ADD_GOSSIP_ITEM(0, XIRI_OPTION, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

	player->SEND_GOSSIP_MENU(player->GetGossipTextId(_Creature), _Creature->GetGUID());
	return true;
}

bool OnGossipSelect(Player* player, Creature* _Creature, uint32 /*sender*/, uint32 action)
{
	if (action == GOSSIP_ACTION_INFO_DEF+1)
	{
		player->CLOSE_GOSSIP_MENU();
		CAST_AI(npc_xiri::npc_xiriAI,_Creature->AI())->StartEvent(player);
	}
	return true;
  }
};

class npc_preeven_maiev : public CreatureScript
{
public:
    npc_preeven_maiev() : CreatureScript("npc_preeven_maiev") {}

    struct npc_preeven_maievAI : public QuantumBasicAI
    {
        npc_preeven_maievAI(Creature* creature) : QuantumBasicAI(creature)
        {
			Reset();
        }

        uint32 StrikeTimer;
        uint32 ThrowTimer;

        void Reset()
        {
			StrikeTimer = 4000;
			ThrowTimer = 6000;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim() )
                return;

            if (StrikeTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_STRIKE);
					StrikeTimer = 5000;
				}
            }
			else StrikeTimer -= diff;

        
            if (ThrowTimer <= diff)
            {
                DoCastVictim(SPELL_THROW_DAGGERS);
                ThrowTimer = 6000;
            }
			else ThrowTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_preeven_maievAI(creature);
    }
};

void AddSC_shadowmoon_valley()
{
    new npc_mature_netherwing_drake();
    new npc_enslaved_netherwing_drake();
    new npc_dragonmaw_peon();
    new npc_drake_dealer_hurlunk();
    new npcs_flanis_swiftwing_and_kagrosh();
    new npc_murkblood_overseer();
    new npc_karynaku();
    new npc_oronok_tornheart();
    new npc_overlord_morghor();
    new npc_earthmender_wilda();
    new npc_lord_illidan_stormrage();
    new go_crystal_prison();
    new npc_illidari_spawn();
    new npc_torloth_the_magnificent();
    new npc_enraged_spirit();
	new npc_item_spectrecles();
	new npc_item_path_of_conquest();
	new npc_azaloth();
	new spell_unlocking_zuluheds_chains();
	new npc_shadowmoon_tuber_node();
	new npc_greater_felfire_diemetradon();
	new npc_xiri();
	new npc_preeven_maiev();
}