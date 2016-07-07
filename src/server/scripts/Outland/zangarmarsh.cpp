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

#define GOSSIP_ITEM_BLESS_ASH "Grant me your mark, wise ancient."
#define GOSSIP_ITEM_BLESS_KEL "Grant me your mark, mighty ancient."

enum AshyenAndkeleth
{
	NPC_ASHYEN             = 17900,
	NPC_KELETH             = 17901,
	GOSSIP_REWARD_BLESSING = -1000359,
};

class npcs_ashyen_and_keleth : public CreatureScript
{
public:
    npcs_ashyen_and_keleth() : CreatureScript("npcs_ashyen_and_keleth") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetReputationRank(942) > REP_NEUTRAL)
        {
            if (creature->GetEntry() == NPC_ASHYEN)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BLESS_ASH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

            if (creature->GetEntry() == NPC_KELETH)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BLESS_KEL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        }
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
			creature->SetPowerType(POWER_MANA);
			creature->SetPower(POWER_MANA, 200);
            creature->SetMaxPower(POWER_MANA, 200);

            if (creature->GetEntry() == NPC_ASHYEN)
            {
                switch (player->GetReputationRank(942))
                {
                    case REP_FRIENDLY:
                        creature->CastSpell(player, 31808, true);
                        DoSendQuantumText(GOSSIP_REWARD_BLESSING, creature);
                        break;
                    case REP_HONORED:
                        creature->CastSpell(player, 31810, true);
                        DoSendQuantumText(GOSSIP_REWARD_BLESSING, creature);
                        break;
                    case REP_REVERED:
                        creature->CastSpell(player, 31811, true);
                        DoSendQuantumText(GOSSIP_REWARD_BLESSING, creature);
                        break;
                    case REP_EXALTED:
                        creature->CastSpell(player, 31815, true);
                        DoSendQuantumText(GOSSIP_REWARD_BLESSING, creature);
                        break;
                    default:
                        break;
                }
            }

            if (creature->GetEntry() == NPC_KELETH)
            {
                switch (player->GetReputationRank(942))
                {
                    case REP_FRIENDLY:
                        creature->CastSpell(player, 31807, true);
                        DoSendQuantumText(GOSSIP_REWARD_BLESSING, creature);
                        break;
                    case REP_HONORED:
                        creature->CastSpell(player, 31812, true);
                        DoSendQuantumText(GOSSIP_REWARD_BLESSING, creature);
                        break;
                    case REP_REVERED:
                        creature->CastSpell(player, 31813, true);
                        DoSendQuantumText(GOSSIP_REWARD_BLESSING, creature);
                        break;
                    case REP_EXALTED:
                        creature->CastSpell(player, 31814, true);
                        DoSendQuantumText(GOSSIP_REWARD_BLESSING, creature);
                        break;
                    default:
                        break;
                }
            }
            player->CLOSE_GOSSIP_MENU();
            player->TalkedToCreature(creature->GetEntry(), creature->GetGUID());
        }
        return true;
    }
};

#define GOSSIP_COOSH "You owe Sim'salabim money. Hand them over or die!"

enum Cooshhooosh
{
    SPELL_LIGHTNING_BOLT = 9532,
    QUEST_CRACK_SKULLS   = 10009,
    FACTION_HOSTILE_CO   = 45,
};

class npc_cooshcoosh : public CreatureScript
{
public:
    npc_cooshcoosh() : CreatureScript("npc_cooshcoosh") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_CRACK_SKULLS) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_COOSH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(9441, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->SetCurrentFaction(FACTION_HOSTILE_CO);
            creature->AI()->AttackStart(player);
        }
        return true;
    }

    struct npc_cooshcooshAI : public QuantumBasicAI
    {
        npc_cooshcooshAI(Creature* creature) : QuantumBasicAI(creature)
        {
            NormFaction = creature->GetFaction();
        }

        uint32 NormFaction;
        uint32 LightningBoltTimer;

        void Reset()
        {
            LightningBoltTimer = 0.5*IN_MILLISECONDS;

            if (me->GetFaction() != NormFaction)
                me->SetCurrentFaction(NormFaction);
        }

        void EnterToBattle(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (LightningBoltTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_LIGHTNING_BOLT);
					LightningBoltTimer = urand(3000, 4000);
				}
            }
			else LightningBoltTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cooshcooshAI(creature);
    }
};

#define GOSSIP_ITEM_KUR1 "Greetings, elder. It is time for your people to end their hostility towards the draenei and their allies."
#define GOSSIP_ITEM_KUR2 "I did not mean to deceive you, elder. The draenei of Telredor thought to approach you in a way that would seem familiar to you."
#define GOSSIP_ITEM_KUR3 "I will tell them. Farewell, elder."

class npc_elder_kuruti : public CreatureScript
{
public:
    npc_elder_kuruti() : CreatureScript("npc_elder_kuruti") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(9803) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KUR1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(9226, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KUR2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(9227, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KUR3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(9229, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
            {
                if (!player->HasItemCount(24573, 1))
                {
                    ItemPosCountVec dest;
                    uint32 itemId = 24573;
                    InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, 1, NULL);
                    if (msg == EQUIP_ERR_OK)
                    {
                        player->StoreNewItem(dest, itemId, true);
                    }
                    else player->SendEquipError(msg, NULL, NULL, itemId);
                }
                player->SEND_GOSSIP_MENU(9231, creature->GetGUID());
                break;
            }
        }
        return true;
    }
};

class npc_mortog_steamhead : public CreatureScript
{
public:
    npc_mortog_steamhead() : CreatureScript("npc_mortog_steamhead") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsVendor() && player->GetReputationRank(942) == REP_EXALTED)
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

enum Kayra
{
    SAY_START          = -1000343,
    SAY_AMBUSH1        = -1000344,
    SAY_PROGRESS       = -1000345,
    SAY_AMBUSH2        = -1000346,
    SAY_NEAR_END       = -1000347,

    QUEST_ESCAPE_FROM  = 9752,
    NPC_SLAVEBINDER    = 18042,
};

class npc_kayra_longmane : public CreatureScript
{
public:
    npc_kayra_longmane() : CreatureScript("npc_kayra_longmane") { }

    struct npc_kayra_longmaneAI : public npc_escortAI
    {
        npc_kayra_longmaneAI(Creature* creature) : npc_escortAI(creature) {}

        void Reset(){}

        void WaypointReached(uint32 i)
        {
            Player* player = GetPlayerForEscort();

            if (!player)
                return;

            switch (i)
            {
                case 4:
                    DoSendQuantumText(SAY_AMBUSH1, me, player);
                    DoSpawnCreature(NPC_SLAVEBINDER, -10.0f, -5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    DoSpawnCreature(NPC_SLAVEBINDER, -8.0f, 5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    break;
                case 5:
                    DoSendQuantumText(SAY_PROGRESS, me, player);
                    SetRun();
                    break;
                case 16:
                    DoSendQuantumText(SAY_AMBUSH2, me, player);
                    DoSpawnCreature(NPC_SLAVEBINDER, -10.0f, -5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    DoSpawnCreature(NPC_SLAVEBINDER, -8.0f, 5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    break;
                case 17:
                    SetRun(false);
                    DoSendQuantumText(SAY_NEAR_END, me, player);
                    break;
                case 25:
                    player->GroupEventHappens(QUEST_ESCAPE_FROM, me);
                    break;
            }
        }
    };

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_ESCAPE_FROM)
        {
            DoSendQuantumText(SAY_START, creature, player);

            if (npc_escortAI* EscortAI = CAST_AI(npc_kayra_longmane::npc_kayra_longmaneAI, creature->AI()))
                EscortAI->Start(false, false, player->GetGUID());
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_kayra_longmaneAI(creature);
    }
};

/*######
## npc_timothy_daniels
######*/

#define GOSSIP_TIMOTHY_DANIELS_ITEM1 "Specialist, eh? Just what kind of specialist are you, anyway?"
#define GOSSIP_TEXT_BROWSE_POISONS   "Let me browse your reagents and poison supplies."

enum TimothyDaniels
{
    GOSSIP_TEXTID_TIMOTHY_DANIELS1 = 9239,
};

class npc_timothy_daniels : public CreatureScript
{
public:
    npc_timothy_daniels() : CreatureScript("npc_timothy_daniels") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->IsVendor())
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_POISONS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TIMOTHY_DANIELS_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_TIMOTHY_DANIELS1, creature->GetGUID());
                break;
            case GOSSIP_ACTION_TRADE:
                player->GetSession()->SendListInventory(creature->GetGUID());
                break;
        }
        return true;
    }
};

enum BalanceMustBePreserved
{
	SPELL_IRONVINE_SEEDS                  = 31736,

	NPC_STEAM_PUMP_OVERSEER               = 18340,
	NPC_KILL_CREDIT_UMBRAFEN_STEAM_PUMP   = 17998,
	NPC_KILL_CREDIT_LAGOON_STEAM_PUMP     = 17999,
	NPC_KILL_CREDIT_SERPENT_STEAM_PUMP    = 18000,
	NPC_KILL_CREDIT_MARSHLIGHT_STEAM_PUMP = 18002,
};

class npc_umbrafen_steam_pump_credit_marker : public CreatureScript
{
public:
    npc_umbrafen_steam_pump_credit_marker() : CreatureScript("npc_umbrafen_steam_pump_credit_marker") {}

    struct npc_umbrafen_steam_pump_credit_markerAI : public QuantumBasicAI
    {
		npc_umbrafen_steam_pump_credit_markerAI(Creature* creature) : QuantumBasicAI(creature) {}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_IRONVINE_SEEDS)
            {
				if (Player* player = caster->ToPlayer())
				{
					caster->ToPlayer()->KilledMonsterCredit(NPC_KILL_CREDIT_UMBRAFEN_STEAM_PUMP, 0);
					me->SummonCreature(NPC_STEAM_PUMP_OVERSEER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000);
				}

				me->DespawnAfterAction(3*IN_MILLISECONDS);
			}
        }

		void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_umbrafen_steam_pump_credit_markerAI(creature);
    }
};

class npc_lagoon_steam_pump_credit_marker : public CreatureScript
{
public:
    npc_lagoon_steam_pump_credit_marker() : CreatureScript("npc_lagoon_steam_pump_credit_marker") {}

    struct npc_lagoon_steam_pump_credit_markerAI : public QuantumBasicAI
    {
		npc_lagoon_steam_pump_credit_markerAI(Creature* creature) : QuantumBasicAI(creature) {}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_IRONVINE_SEEDS)
            {
				if (Player* player = caster->ToPlayer())
				{
					caster->ToPlayer()->KilledMonsterCredit(NPC_KILL_CREDIT_LAGOON_STEAM_PUMP, 0);
					me->SummonCreature(NPC_STEAM_PUMP_OVERSEER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000);
				}

				me->DespawnAfterAction(3*IN_MILLISECONDS);
			}
        }

		void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lagoon_steam_pump_credit_markerAI(creature);
    }
};

class npc_serpent_steam_pump_credit_marker : public CreatureScript
{
public:
    npc_serpent_steam_pump_credit_marker() : CreatureScript("npc_serpent_steam_pump_credit_marker") {}

    struct npc_serpent_steam_pump_credit_markerAI : public QuantumBasicAI
    {
		npc_serpent_steam_pump_credit_markerAI(Creature* creature) : QuantumBasicAI(creature) {}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_IRONVINE_SEEDS)
            {
				if (Player* player = caster->ToPlayer())
				{
					caster->ToPlayer()->KilledMonsterCredit(NPC_KILL_CREDIT_SERPENT_STEAM_PUMP, 0);
					me->SummonCreature(NPC_STEAM_PUMP_OVERSEER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000);
				}

				me->DespawnAfterAction(3*IN_MILLISECONDS);
			}
        }

		void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_serpent_steam_pump_credit_markerAI(creature);
    }
};

class npc_marshlight_steam_pump_credit_marker : public CreatureScript
{
public:
    npc_marshlight_steam_pump_credit_marker() : CreatureScript("npc_marshlight_steam_pump_credit_marker") {}

    struct npc_marshlight_steam_pump_credit_markerAI : public QuantumBasicAI
    {
		npc_marshlight_steam_pump_credit_markerAI(Creature* creature) : QuantumBasicAI(creature) {}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_IRONVINE_SEEDS)
            {
				if (Player* player = caster->ToPlayer())
				{
					caster->ToPlayer()->KilledMonsterCredit(NPC_KILL_CREDIT_MARSHLIGHT_STEAM_PUMP, 0);
					me->SummonCreature(NPC_STEAM_PUMP_OVERSEER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000);
				}

				me->DespawnAfterAction(3*IN_MILLISECONDS);
			}
        }

		void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_marshlight_steam_pump_credit_markerAI(creature);
    }
};

void AddSC_zangarmarsh()
{
    new npcs_ashyen_and_keleth();
    new npc_cooshcoosh();
    new npc_elder_kuruti();
    new npc_mortog_steamhead();
    new npc_kayra_longmane();
    new npc_timothy_daniels();
	new npc_umbrafen_steam_pump_credit_marker();
	new npc_lagoon_steam_pump_credit_marker();
	new npc_serpent_steam_pump_credit_marker();
	new npc_marshlight_steam_pump_credit_marker();
}