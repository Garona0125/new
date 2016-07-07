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
#include "Vehicle.h"
#include "CombatAI.h"

#define SAY_AGGRO      -1571003
#define GOSSIP_AGNETTA "Skip the warmup, sister... or are you too scared to face soemeone your own size?"

enum Agnetta
{
    QUEST_ITS_THAT_YOUR_GOBLIN = 12969,
    FACTION_HOSTILE_AT1        = 45,
};

class npc_agnetta_tyrsdottar : public CreatureScript
{
public:
    npc_agnetta_tyrsdottar() : CreatureScript("npc_agnetta_tyrsdottar") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_ITS_THAT_YOUR_GOBLIN) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_AGNETTA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(13691, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            DoSendQuantumText(SAY_AGGRO, creature);
            player->CLOSE_GOSSIP_MENU();
            creature->SetCurrentFaction(FACTION_HOSTILE_AT1);
            creature->AI()->AttackStart(player);
        }
        return true;
    }

    struct npc_agnetta_tyrsdottarAI : public QuantumBasicAI
    {
        npc_agnetta_tyrsdottarAI(Creature* creature) : QuantumBasicAI(creature) { }

        void Reset()
        {
            me->RestoreFaction();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_agnetta_tyrsdottarAI(creature);
    }
};

#define GOSSIP_ITEM1 "Are you okay? I've come to take you back to Frosthold if you can stand."
#define GOSSIP_ITEM2 "I'm sorry that I didn't get here sooner. What happened?"
#define GOSSIP_ITEM3 "I'll go get some help. Hang in there."

enum FrostbornScout
{
    QUEST_MISSING_SCOUTS  =  12864,
};

class npc_frostborn_scout : public CreatureScript
{
public:
    npc_frostborn_scout() : CreatureScript("npc_frostborn_scout") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_MISSING_SCOUTS) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->PlayerTalkClass->SendGossipMenu(13611, creature->GetGUID());
        }

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF+1:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->PlayerTalkClass->SendGossipMenu(13612, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            player->PlayerTalkClass->SendGossipMenu(13613, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            player->PlayerTalkClass->SendGossipMenu(13614, creature->GetGUID());
            player->AreaExploredOrEventHappens(QUEST_MISSING_SCOUTS);
            break;
        }
        return true;
    }
};

#define GOSSIP_HN  "Thorim?"
#define GOSSIP_SN1 "Can you tell me what became of Sif?"
#define GOSSIP_SN2 "He did more than that, Thorim. He controls Ulduar now."
#define GOSSIP_SN3 "It needn't end this way."

enum Thorim
{
    QUEST_SIBLING_RIVALRY = 13064,

    NPC_THORIM            = 29445,

    GOSSIP_TEXTID_THORIM1 = 13799,
    GOSSIP_TEXTID_THORIM2 = 13801,
    GOSSIP_TEXTID_THORIM3 = 13802,
    GOSSIP_TEXTID_THORIM4 = 13803,
};

class npc_thorim : public CreatureScript
{
public:
    npc_thorim() : CreatureScript("npc_thorim") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_SIBLING_RIVALRY) == QUEST_STATUS_INCOMPLETE)
		{
            player->ADD_GOSSIP_ITEM(0, GOSSIP_HN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_THORIM1, creature->GetGUID());
            return true;
        }
        return false;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(0, GOSSIP_SN1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_THORIM2, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(0, GOSSIP_SN2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_THORIM3, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->ADD_GOSSIP_ITEM(0, GOSSIP_SN3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_THORIM4, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                player->CLOSE_GOSSIP_MENU();
                player->CompleteQuest(QUEST_SIBLING_RIVALRY);
                break;
        }
        return true;
    }
};

#define GOSSIP_CHALLENGER "Let's do this, sister."

enum VictoriousChallenger
{
    QUEST_TAKING_ALL_CHALLENGERS = 12971,
    QUEST_DEFENDING_YOUR_TITLE   = 13423,

    SPELL_SUNDER_ARMOR           = 11971,
    SPELL_REND_VC                = 11977,
};

class npc_victorious_challenger : public CreatureScript
{
public:
    npc_victorious_challenger() : CreatureScript("npc_victorious_challenger") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_TAKING_ALL_CHALLENGERS) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_DEFENDING_YOUR_TITLE) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_CHALLENGER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        return false;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->SetCurrentFaction(16);
            creature->AI()->AttackStart(player);
        }
        return true;
    }

    struct npc_victorious_challengerAI : public QuantumBasicAI
    {
        npc_victorious_challengerAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 SunderArmorTimer;
        uint32 RendTimer;

        void Reset()
        {
            me->RestoreFaction();

            SunderArmorTimer = 500;
            RendTimer = 1000;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (SunderArmorTimer < diff)
            {
                DoCastVictim(SPELL_SUNDER_ARMOR, true);
                SunderArmorTimer = 2000;
            }
			else SunderArmorTimer -= diff;

            if (RendTimer < diff)
            {
                DoCastVictim(SPELL_REND_VC, true);
                RendTimer = 4000;
            }
			else RendTimer -= diff;

            DoMeleeAttackIfReady();
        }

        void KilledUnit(Unit* /*victim*/)
        {
            me->RestoreFaction();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_victorious_challengerAI(creature);
    }
};

#define GOSSIP_LOKLIRACRONE  "Tell me about this proposal"
#define GOSSIP_LOKLIRACRONE1 "What happened then?"
#define GOSSIP_LOKLIRACRONE2 "You want me to take part in the Hyldsmeet to end the war?"
#define GOSSIP_LOKLIRACRONE3 "Very well. I'll take part in this competition."

enum LokliraCrone
{
    QUEST_HYLDSMEET    = 12970,

    GOSSIP_TEXTID_LOK1 = 13778,
    GOSSIP_TEXTID_LOK2 = 13779,
    GOSSIP_TEXTID_LOK3 = 13780,
};

class npc_loklira_crone : public CreatureScript
{
public:
    npc_loklira_crone() : CreatureScript("npc_loklira_crone") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_HYLDSMEET) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOKLIRACRONE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }
        return false;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOKLIRACRONE1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_LOK1, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOKLIRACRONE2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_LOK2, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOKLIRACRONE3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_LOK3, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                player->CLOSE_GOSSIP_MENU();
                player->CompleteQuest(QUEST_HYLDSMEET);
                break;
        }
        return true;
    }
};

enum InjuredGoblin
{
    QUEST_BITTER_DEPARTURE = 12832,
    SAY_QUEST_ACCEPT       =  -1800042,
    SAY_END_WP_REACHED     =  -1800043,
};

#define GOSSIP_ITEM_1 "I am ready, lets get you out of here"

class npc_injured_goblin : public CreatureScript
{
public:
    npc_injured_goblin() : CreatureScript("npc_injured_goblin") { }

	    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_BITTER_DEPARTURE) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(0, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->PlayerTalkClass->SendGossipMenu(9999999, creature->GetGUID());
        }
        else
            player->SEND_GOSSIP_MENU(999999, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        npc_escortAI* EscortAI = CAST_AI(npc_injured_goblin::npc_injured_goblinAI, creature->AI());

        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            EscortAI->Start(true, true, player->GetGUID());
            creature->SetCurrentFaction(113);
        }
        return true;
    }

	bool OnQuestAccept(Player* /*player*/, Creature* creature, Quest const *quest)
    {
        if (quest->GetQuestId() == QUEST_BITTER_DEPARTURE)
            DoSendQuantumText(SAY_QUEST_ACCEPT, creature);

        return false;
    }

    struct npc_injured_goblinAI : public npc_escortAI
    {
        npc_injured_goblinAI(Creature* creature) : npc_escortAI(creature) { }

        void WaypointReached(uint32 id)
        {
            Player* player = GetPlayerForEscort();
            switch (id)
            {
            case 26:
                DoSendQuantumText(SAY_END_WP_REACHED, me, player);
                break;
            case 27:
                if (player)
                    player->GroupEventHappens(QUEST_BITTER_DEPARTURE, me);
                break;
            }
        }

		void Reset(){}

        void EnterToBattle(Unit* /*who*/){}

        void JustDied(Unit* /*killer*/)
        {
            Player* player = GetPlayerForEscort();

            if (HasEscortState(STATE_ESCORT_ESCORTING) && player)
                player->FailQuest(QUEST_BITTER_DEPARTURE);
		}
		
		void UpdateAI(const uint32 diff)
		{
			npc_escortAI::UpdateAI(diff);

			if (!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_injured_goblinAI(creature);
    }
};

enum RoxiRamrocket
{
	SPELL_MECHANO_HOG        = 60866,
	SPELL_MEKGINEERS_CHOPPER = 60867,
};

class npc_roxi_ramrocket : public CreatureScript
{
public:
    npc_roxi_ramrocket() : CreatureScript("npc_roxi_ramrocket") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->IsTrainer())
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TRAINER_TRAIN), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRAIN);

        if (creature->IsVendor())
            if (player->HasSpell(SPELL_MECHANO_HOG) || player->HasSpell(SPELL_MEKGINEERS_CHOPPER))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_VENDOR_BROWSE_YOUR_GOODS), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
        case GOSSIP_ACTION_TRAIN:
            player->GetSession()->SendTrainerList(creature->GetGUID());
            break;
        case GOSSIP_ACTION_TRADE:
            player->GetSession()->SendListInventory(creature->GetGUID());
            break;
        }
        return true;
    }
};

enum GoingBearback
{
	SPELL_DEBILITATING_STRIKE = 38621,
	SPELL_FROSTWORG_CREDIT    = 58183,
	SPELL_FROST_GIANT_CREDIT  = 58184,
	SPELL_MAIM_FLESH          = 50075,
	SPELL_FROSTBITE           = 61572,
    SPELL_ABLAZE              = 54683,
};

class npc_niffelem_frost_giant : public CreatureScript
{
public:
    npc_niffelem_frost_giant() : CreatureScript("npc_niffelem_frost_giant") {}

    struct npc_niffelem_frost_giantAI : public QuantumBasicAI
    {
        npc_niffelem_frost_giantAI(Creature* creature) : QuantumBasicAI(creature) {}

		bool Burned;

		uint32 DebilitatingStrikeTimer;

        void Reset()
        {
			DebilitatingStrikeTimer = 1000;

			Burned = false;

            me->SetCorpseDelay(5);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_ABLAZE && !Burned)
			{
				Burned = true;
				caster->CastSpell(caster, SPELL_FROST_GIANT_CREDIT, true);
				me->DespawnAfterAction(5*IN_MILLISECONDS);
			}
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (DebilitatingStrikeTimer < diff)
			{
				DoCastVictim(SPELL_DEBILITATING_STRIKE);
				DebilitatingStrikeTimer = 4000;
			}
			else DebilitatingStrikeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_niffelem_frost_giantAI(creature);
    }
};

class npc_frostworg : public CreatureScript
{
public:
    npc_frostworg() : CreatureScript("npc_frostworg") {}

    struct npc_frostworgAI : public QuantumBasicAI
    {
        npc_frostworgAI(Creature* creature) : QuantumBasicAI(creature) {}

		bool Burned;

		uint32 MaimFleshTimer;
		uint32 FrostbiteTimer;

        void Reset()
        {
			MaimFleshTimer = 1000;
            FrostbiteTimer = 2000;

			Burned = false;

            me->SetCorpseDelay(5);
            me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_ABLAZE && !Burned)
			{
				Burned = true;
				caster->CastSpell(caster, SPELL_FROSTWORG_CREDIT, true);
				me->DespawnAfterAction(5*IN_MILLISECONDS);
			}
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (MaimFleshTimer < diff)
			{
				DoCastVictim(SPELL_MAIM_FLESH);
				MaimFleshTimer = 3000;
			}
			else MaimFleshTimer -= diff;

			if (FrostbiteTimer < diff)
			{
				DoCastVictim(SPELL_FROSTBITE);
				FrostbiteTimer = 5000;
			}
			else FrostbiteTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frostworgAI(creature);
    }
};

enum CaptiveProtoDrake
{
	SPELL_SUMMON_FREED_PROTO_DRAKE = 55028,

	QUEST_COLD_HEARTED             = 12856,
};

class npc_captive_proto_drake : public CreatureScript
{
public:
    npc_captive_proto_drake() : CreatureScript("npc_captive_proto_drake") { }

    struct npc_captive_proto_drakeAI : public QuantumBasicAI
    {
        npc_captive_proto_drakeAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void Reset(){}

		void MoveInLineOfSight(Unit* who)
		{
			if (Player* player = who->ToPlayer())
			{
				if (player->IsWithinDist(me, 15.0f) && player->GetQuestStatus(QUEST_COLD_HEARTED) == QUEST_STATUS_INCOMPLETE)
				{
					player->RemoveAurasByType(SPELL_AURA_MOUNTED);
					player->CastSpell(player, SPELL_SUMMON_FREED_PROTO_DRAKE, true);
				}
			}
		}

		void UpdateAI(uint32 const /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_captive_proto_drakeAI(creature);
    }
};

enum FreedProtoDrake
{
    NPC_DRAKE                      = 29709,
    AREA_VALLEY_OF_ANCIENT_WINTERS = 4437,

	FREED_PROTO_DRAKE_EMOTE_1      = -1300029,
	FREED_PROTO_DRAKE_EMOTE_2      = -1300030,

	SPELL_PARACHUTE                = 45472,
	SPELL_FLIGHT                   = 55034,
	SPELL_BURST_OF_SPEED           = 61183,
    SPELL_KILL_CREDIT_PRISONER     = 55144,
    SPELL_SUMMON_LIBERATED         = 55073,
    SPELL_KILL_CREDIT_DRAKE        = 55143,

    EVENT_CHECK_AREA               = 1,
    EVENT_REACHED_HOME             = 2,
};

class npc_freed_proto_drake : public CreatureScript
{
public:
    npc_freed_proto_drake() : CreatureScript("npc_freed_proto_drake") { }

    struct npc_freed_proto_drakeAI : public VehicleAI
    {
        npc_freed_proto_drakeAI(Creature* creature) : VehicleAI(creature)
		{
			DoCast(me, SPELL_FLIGHT, true);
			DoCast(me, SPELL_BURST_OF_SPEED, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        EventMap events;

        void Reset()
        {
            events.ScheduleEvent(EVENT_CHECK_AREA, 0.5*IN_MILLISECONDS);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != WAYPOINT_MOTION_TYPE)
                return;

            if (id == 15)
				events.ScheduleEvent(EVENT_REACHED_HOME, 2*IN_MILLISECONDS);
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

            switch (events.ExecuteEvent())
            {
                case EVENT_CHECK_AREA:
                    if (me->GetAreaId() == AREA_VALLEY_OF_ANCIENT_WINTERS)
                    {
                        if (Vehicle* vehicle = me->GetVehicleKit())
						{
                            if (Unit* passenger = vehicle->GetPassenger(0))
                            {
								DoSendQuantumText(RAND(FREED_PROTO_DRAKE_EMOTE_1, FREED_PROTO_DRAKE_EMOTE_2), me, passenger);
                                me->GetMotionMaster()->MovePath(NPC_DRAKE, false);
                            }
						}
					}
					else events.ScheduleEvent(EVENT_CHECK_AREA, 0.5*IN_MILLISECONDS);
                    break;
				case EVENT_REACHED_HOME:
                    if (Vehicle* vehicle = me->GetVehicleKit())
					{
						if (Unit* player = vehicle->GetPassenger(0))
						{
							if (player->GetTypeId() == TYPE_ID_PLAYER)
							{
								player->CastSpell(player, SPELL_KILL_CREDIT_PRISONER, true);
								player->CastSpell(player, SPELL_KILL_CREDIT_PRISONER, true);
								player->CastSpell(player, SPELL_KILL_CREDIT_PRISONER, true);
								player->CastSpell(player, SPELL_SUMMON_LIBERATED, true);
								player->ExitVehicle();
							}
							me->CastSpell(me, SPELL_KILL_CREDIT_DRAKE, true);
						}
					}
					break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_freed_proto_drakeAI(creature);
    }
};

enum BrunnhildarPrisoner
{
    SPELL_ICE_BLOCK        = 54894,
    SPELL_ICE_SHARD        = 55046,
    SPELL_ICE_SHARD_IMPACT = 55047,
};

class npc_brunnhildar_prisoner : public CreatureScript
{
public:
    npc_brunnhildar_prisoner() : CreatureScript("npc_brunnhildar_prisoner") {}

    struct npc_brunnhildar_prisonerAI : public QuantumBasicAI
    {
        npc_brunnhildar_prisonerAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CheckTimer;

        void Reset()
        {
            CheckTimer = 10*IN_MILLISECONDS;
            DoCast(me, SPELL_ICE_BLOCK, true);
        }

        void DoAction(int32 const /*action*/)
        {
            me->Kill(me);
            me->Respawn();
        }

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_ICE_SHARD)
            {
                DoCast(me, SPELL_ICE_SHARD_IMPACT, true);

                if (caster->IsVehicle())
                {
                    uint8 seat = caster->GetVehicleKit()->GetNextEmptySeat(0, true);
                    if (seat <= 0)
                        return;

                    me->EnterVehicle(caster);
                    me->RemoveAurasDueToSpell(SPELL_ICE_BLOCK);
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (CheckTimer < diff)
            {
                if (!me->HasUnitState(UNIT_STATE_ON_VEHICLE))
                {
                    if (me->GetDistance(me->GetHomePosition()) > 30.0f)
                        DoAction(0);
                }
                else
                {
                    if (me->GetPositionY() > -2595.0f)
                    {
                        if (Unit* base = me->GetVehicleBase())
						{
                            if (base->IsCharmed())
                                base->RemoveCharmedBy(base->GetCharmer());
						}
                    }
                }

                CheckTimer = 10*IN_MILLISECONDS;
            }
            else CheckTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_brunnhildar_prisonerAI(creature);
    }
};

enum IcemawMatriarch
{
    QUEST_LAST_OF_HER_KIND = 12983,

    NPC_INJURED_ICEMAW     = 29563,

    SPELL_HARNESSED_ICEMAW = 56795,
};

class npc_injured_icemaw : public CreatureScript
{
public:
    npc_injured_icemaw() : CreatureScript("npc_injured_icemaw") { }

    struct npc_injured_icemawAI : public QuantumBasicAI
    {
        npc_injured_icemawAI(Creature* creature) : QuantumBasicAI(creature) { }

        void MoveInLineOfSight(Unit* who)
        {
            if (who->GetTypeId() != TYPE_ID_PLAYER)
                return;

            if (who->ToPlayer()->GetQuestStatus(QUEST_LAST_OF_HER_KIND) == QUEST_STATUS_INCOMPLETE && !who->HasUnitState(UNIT_STATE_ON_VEHICLE) && who->GetDistance(me) < 5.0f)
            {
                who->CastSpell(who, SPELL_HARNESSED_ICEMAW, true);

                if (Unit* base = who->GetVehicleBase())
				{
                    if (base->IsCharmed())
                        base->RemoveCharmedBy(base->GetCharmer());
				}
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_injured_icemawAI(creature);
    }
};

const Position HarnessedIcemawWaypoints[17] =
{
    {7332.80f, -2065.69f, 765.29f, 0.0f},
    {7327.32f, -2101.70f, 774.22f, 0.0f},
    {7254.51f, -2117.08f, 778.98f, 0.0f},
    {7224.31f, -2117.58f, 777.44f, 0.0f},
    {7194.28f, -2114.08f, 765.97f, 0.0f},
    {7155.83f, -2134.19f, 762.16f, 0.0f},
    {7117.62f, -2113.06f, 760.57f, 0.0f},
    {7074.25f, -1956.43f, 769.82f, 0.0f},
    {7065.34f, -1917.58f, 781.57f, 0.0f},
    {7094.17f, -1884.47f, 787.00f, 0.0f},
    {7033.13f, -1883.46f, 799.88f, 0.0f},
    {7021.64f, -1844.55f, 818.59f, 0.0f},
    {7015.42f, -1745.49f, 819.72f, 0.0f},
    {7003.12f, -1721.36f, 820.06f, 0.0f},
    {6947.09f, -1724.14f, 820.61f, 0.0f},
    {6877.17f, -1684.31f, 820.03f, 0.0f},
    {6825.53f, -1702.27f, 820.55f, 0.0f}
};

class npc_harnessed_icemaw : public CreatureScript
{
public:
    npc_harnessed_icemaw() : CreatureScript("npc_harnessed_icemaw") { }

    struct npc_harnessed_icemawAI : public QuantumBasicAI
    {
        npc_harnessed_icemawAI(Creature* creature) : QuantumBasicAI(creature) { }

        uint8 count;
        bool WpReached;
        bool MovementStarted;

        void Reset()
        {
            count = 0;
            WpReached = false;
            MovementStarted = false;
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

			me->SetSpeed(MOVE_RUN, 2.0f, true);

            if (id < 16)
            {
                ++count;
                WpReached = true;
            }
            else
            {
                if (Unit* player = me->GetVehicleKit()->GetPassenger(0))
				{
					if (player->GetTypeId() == TYPE_ID_PLAYER)
					{
						player->ToPlayer()->KilledMonsterCredit(NPC_INJURED_ICEMAW, 0);
						player->ExitVehicle();
					}
                }
            }
        }

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!me->IsCharmed() && !MovementStarted)
            {
                MovementStarted = true;
                WpReached = true;
            }

            if (WpReached)
            {
                WpReached = false;
                me->GetMotionMaster()->MovePoint(count, HarnessedIcemawWaypoints[count]);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_harnessed_icemawAI(creature);
    }
};

enum Drakerider
{
	NPC_HYLDSMEET_DRAKERIDER = 29694,
	SPELL_DESPAWN_DRAKERIDER = 57809,
};

class npc_hyldsmeet_protodrake : public CreatureScript
{
public:
	npc_hyldsmeet_protodrake() : CreatureScript("npc_hyldsmeet_protodrake"){}

	struct npc_hyldsmeet_protodrakeAI : public CreatureAI
	{
		npc_hyldsmeet_protodrakeAI(Creature* creature) : CreatureAI(creature), AccessoryRespawnTimer(0), VehicleKit(creature->GetVehicleKit()){}

		Vehicle* VehicleKit;

		uint32 AccessoryRespawnTimer;

		void Reset()
		{
			me->SetSpeed(MOVE_FLIGHT, 3.5f);
			me->SetCanFly(true);
			me->SetUnitMovementFlags(MOVEMENTFLAG_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
		}

		void PassengerBoarded(Unit* passenger, int8 /*seat*/, bool apply)
		{
			if (apply)
				return;

			if (passenger->GetEntry() == NPC_HYLDSMEET_DRAKERIDER)
				AccessoryRespawnTimer = 1*MINUTE *IN_MILLISECONDS;
		}

		void UpdateAI(uint32 const diff)
		{
			if (AccessoryRespawnTimer <= diff && VehicleKit)
			{
				VehicleKit->InstallAllAccessories(true);
				AccessoryRespawnTimer = 0;
			}
			else AccessoryRespawnTimer -= diff;
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_hyldsmeet_protodrakeAI(creature);
	}
};

enum SnowblindFollower
{
    NPC_SNOWBLIND_CREDIT     = 34899,
    SPELL_THROW_NET          = 66474,

	SAY_SNOWBLIND_FOLLOWER_1 = -1200300,
	SAY_SNOWBLIND_FOLLOWER_2 = -1200301,
	SAY_SNOWBLIND_FOLLOWER_3 = -1200302,
	SAY_SNOWBLIND_FOLLOWER_4 = -1200303,
	SAY_SNOWBLIND_FOLLOWER_5 = -1200304,
	SAY_SNOWBLIND_FOLLOWER_6 = -1200305,
};

class npc_snowblind_follower : public CreatureScript
{
public:
    npc_snowblind_follower() : CreatureScript("npc_snowblind_follower") {}

    struct npc_snowblind_followerAI : public QuantumBasicAI
    {
        npc_snowblind_followerAI(Creature* creature) : QuantumBasicAI(creature) { }

        bool Hitbynet;

        void Reset()
        {
            Hitbynet = false;
        }

        void MoveInLineOfSight(Unit* /*who*/) {}

        void AttackStart(Unit* /*who*/) {}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_THROW_NET)
            {
                if (!caster || !caster->ToPlayer())
                    return;

                if (Hitbynet)
                    return;

                Hitbynet = true;

				DoSendQuantumText(RAND(SAY_SNOWBLIND_FOLLOWER_1, SAY_SNOWBLIND_FOLLOWER_2, SAY_SNOWBLIND_FOLLOWER_3,
				SAY_SNOWBLIND_FOLLOWER_4, SAY_SNOWBLIND_FOLLOWER_5, SAY_SNOWBLIND_FOLLOWER_6), caster);

				caster->ToPlayer()->KilledMonsterCredit(NPC_SNOWBLIND_CREDIT, 0);

                me->DespawnAfterAction(3*IN_MILLISECONDS);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_snowblind_followerAI(creature);
    }
};

enum Mechaton
{
    NPC_ARCHIVIST_MECHATON  = 29775,

    SPELL_ARCHIVISTS_SCAN   = 55224,
    SPELL_CHARGED_DISK      = 55197,

    GO_FLOOR_GLYPH          = 191762,
    GO_ENERGY_COLUMN        = 191763,

    SAY_1                   = 1,
    SAY_2                   = 2,
    SAY_3                   = 3,
    SAY_4                   = 4,
    EMOTE_1                 = 5,
    SAY_5                   = 6,
    SAY_6                   = 7,
    SAY_7                   = 8,
};

class npc_archivist_mechaton : public CreatureScript
{
public:
    npc_archivist_mechaton() : CreatureScript("npc_archivist_mechaton") {}

    struct npc_archivist_mechatonAI : public QuantumBasicAI
    {
        npc_archivist_mechatonAI(Creature* creature) : QuantumBasicAI(creature)
        {
            if (me->IsSummon())
            {
                me->SummonGameObject(GO_FLOOR_GLYPH, 7991.89f, -827.66f, 968.156f, -2.33874f, 0, 0, 0, 0, 27);
                me->SummonGameObject(GO_ENERGY_COLUMN, 7991.8f, -827.639f, 968.16f, 0.90757f, 0, 0, 0, 0, 27);
            }

            FirstTime = true;
        }

        uint8 Saycount;
        uint32 SayTimer;
        bool FirstTime;

        void Reset()
        {
            SayTimer = 0;

            if (FirstTime)
                Saycount = 1;
            else
            {
                Saycount = 0;
                me->DespawnAfterAction();
            }
        }

        void DoNextText(uint32 timer)
        {
            SayTimer = timer;
            ++Saycount;
        }

        void UpdateAI(uint32 const diff)
        {
            if (SayTimer <= diff)
            {
                Unit* summoner = me->ToTempSummon()->GetSummoner();

                switch (Saycount)
                {
                    case 1:
                        Talk(SAY_1);
                        DoNextText(4000);
                        break;
                    case 2:
                        Talk(SAY_2);
                        DoNextText(3000);
                        break;
                    case 3:
                        Talk(SAY_3);
                        DoNextText(4000);
                        break;
                    case 4:
                        Talk(SAY_4);
                        me->CastSpell(summoner, SPELL_ARCHIVISTS_SCAN, true);
                        DoNextText(1100);
                        break;
                    case 5:
                        Talk(EMOTE_1);
                        DoNextText(8000);
                        break;
                    case 6:
                        Talk(SAY_5);
                        me->CombatStop();
                        DoNextText(4000);
                        break;
                    case 7:
                        Talk(SAY_6);
                        DoNextText(5000);
                        break;
                    case 8:
                        Talk(SAY_7);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->SetReactState(REACT_AGGRESSIVE);
                        AttackStart(summoner);
                        DoNextText(0);
                        FirstTime = false;
                        break;
                }
            }
            else SayTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_archivist_mechatonAI(creature);
    }
};

enum Ornament
{
	SPELL_HYLDNIR_HARPOON = 54933,
};

class npc_column_ornament : public CreatureScript
{
public:
    npc_column_ornament() : CreatureScript("npc_column_ornament") { }

    struct npc_column_ornamentAI : public QuantumBasicAI
    {
		npc_column_ornamentAI(Creature* creature) : QuantumBasicAI(creature){}

		void Reset(){}

		void SpellHit(Unit* caster, SpellInfo const* spell)
        {
			if (spell->Id == SPELL_HYLDNIR_HARPOON)
            {
				if (Player* player = caster->ToPlayer())
				{
					player->ExitVehicle();
					player->TeleportTo(571, 7413.26f, -523.398f, 1896.85f, 2.6318f);
				}
			}
		}
	};

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_column_ornamentAI(creature);
    }
};

enum BrannsCommunicator_Misc
{
    ITEM_ID_BRANNS_COMMUNICATOR        = 40971,

    QUEST_CATCHING_UP_WITH_BRANN       = 12920,
    QUEST_SNIFFING_OUT_THE_PERPETRATOR = 12855,
};

#define GOSSIP_COMMUNICATOR_ITEM "Please, give me a new Kommunicator."

class npc_item_branns_communicator : public CreatureScript
{
    public:
        npc_item_branns_communicator() : CreatureScript("npc_item_branns_communicator") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            if (!player->HasItemCount(ITEM_ID_BRANNS_COMMUNICATOR, 1, true) 
                && (player->GetQuestStatus(QUEST_SNIFFING_OUT_THE_PERPETRATOR) == QUEST_STATUS_REWARDED
                || player->GetQuestStatus(QUEST_CATCHING_UP_WITH_BRANN) == QUEST_STATUS_INCOMPLETE
                || player->GetQuestStatus(QUEST_CATCHING_UP_WITH_BRANN) == QUEST_STATUS_COMPLETE
                || player->GetQuestStatus(QUEST_CATCHING_UP_WITH_BRANN) == QUEST_STATUS_REWARDED))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_COMMUNICATOR_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 0);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
			switch (action)
            {
				case GOSSIP_ACTION_INFO_DEF + 0:
					player->AddItem(ITEM_ID_BRANNS_COMMUNICATOR, 1);
                    player->CLOSE_GOSSIP_MENU();
                    break;
			}
			return true;
		}
};

enum Warbear
{
	NPC_WARBEAR_MATRIARCH    = 29918,
	NPC_HYLDSMEET_BEAR_RIDER = 30175,

	QUEST_INTO_THE_PIT       = 12997,
	QUEST_BACK_TO_THE_PIT    = 13424,

	SPELL_SMASH              = 54458,
	SPELL_CHARGE             = 54460,
	SPELL_DEMORALIZING_ROAR  = 15971,
};

class npc_hyldsmeet_warbear : public CreatureScript
{
public:
    npc_hyldsmeet_warbear() : CreatureScript("npc_hyldsmeet_warbear") { }

    struct npc_hyldsmeet_warbearAI : public QuantumBasicAI
    {
        npc_hyldsmeet_warbearAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 SmashTimer;
		uint32 DemoralizingRoarTimer;

		void Reset()
		{
			SmashTimer = 2000;
			DemoralizingRoarTimer = 4000;

			me->RestoreFaction();
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
		}

		void MoveInLineOfSight(Unit* who)
		{
			if (Creature* bear = me->FindCreatureWithDistance(NPC_WARBEAR_MATRIARCH, 5.0f))
			{
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
				me->SetCurrentFaction(16);
				me->SetInCombatWith(bear);
			}
		}

		void EnterToBattle(Unit* /*who*/)
		{
			if (Creature* bear = me->FindCreatureWithDistance(NPC_WARBEAR_MATRIARCH, 5.0f))
			{
				me->AI()->AttackStart(bear);
				DoCast(bear, SPELL_CHARGE, true);
			}
		}

		void JustDied(Unit* /*killer*/)
		{
			if (Creature* rider = me->FindCreatureWithDistance(NPC_HYLDSMEET_BEAR_RIDER, 2.0f, true))
				rider->DespawnAfterAction();
		}

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;

			if (SmashTimer <= diff)
			{
				DoCastVictim(SPELL_SMASH);
				SmashTimer = 10000;
			}
			else SmashTimer -= diff;

			if (DemoralizingRoarTimer <= diff)
			{
				DoCastAOE(SPELL_DEMORALIZING_ROAR);
				DemoralizingRoarTimer = 20000;
			}
			else DemoralizingRoarTimer -= diff;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hyldsmeet_warbearAI(creature);
    }
};

enum MendingFencesQuestChain
{
	SPELL_HURL_BOULDER       = 55818,
	SPELL_SUMMON_EARTHEN     = 55528,
	SPELL_SHOCKWAVE          = 57741,
	SPELL_CALL_OF_EARTH      = 55512,
	SPELL_FLAME_BREATH       = 57801,

	QUEST_MENDING_FENCES     = 12915,
	QUEST_TH_SIBLING_RIVALRY = 13064,

	NPC_EARTHEN_IRONBANE     = 29927,
};

#define EMOTE_COMBAT_IRON_GIANT "Stormforged Iron Giant is coming to you!"

class npc_stormforged_iron_giant : public CreatureScript
{
public:
    npc_stormforged_iron_giant() : CreatureScript("npc_stormforged_iron_giant"){}

    struct npc_stormforged_iron_giantAI : public QuantumBasicAI
    {
        npc_stormforged_iron_giantAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 ShockWaveTimer;

		void Reset()
		{
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

			ShockWaveTimer = 5000;
		}

		void MoveInLineOfSight(Unit* who)
        {
			if (Player* player = who->ToPlayer())
			{
				if (player->IsWithinDist(me, 20.0f))
				{
					if (player->GetQuestStatus(QUEST_MENDING_FENCES) == QUEST_STATUS_INCOMPLETE && player->GetQuestStatus(QUEST_TH_SIBLING_RIVALRY) == QUEST_STATUS_REWARDED)
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				}
			}
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->MonsterTextEmote(EMOTE_COMBAT_IRON_GIANT, LANG_UNIVERSAL, true);
		}

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShockWaveTimer <= diff)
			{
				DoCastAOE(SPELL_SHOCKWAVE);
				ShockWaveTimer = urand(6000, 7000);
			}
			else ShockWaveTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_30))
				DoCast(me, SPELL_CALL_OF_EARTH);

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stormforged_iron_giantAI(creature);
    }
};

class npc_fjorn : public CreatureScript
{
public:
    npc_fjorn() : CreatureScript("npc_fjorn"){}

    struct npc_fjornAI : public QuantumBasicAI
    {
        npc_fjornAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 FlameBreathTimer;

		void Reset()
		{
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_ONESHOT_ATTACK_1H);

			FlameBreathTimer = 2000;
		}

		void MoveInLineOfSight(Unit* who)
        {
			if (Player* player = who->ToPlayer())
			{
				if (player->IsWithinDist(me, 20.0f))
				{
					if (player->GetQuestStatus(QUEST_MENDING_FENCES) == QUEST_STATUS_INCOMPLETE && player->GetQuestStatus(QUEST_TH_SIBLING_RIVALRY) == QUEST_STATUS_REWARDED)
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				}
			}
        }

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FlameBreathTimer <= diff)
			{
				DoCastVictim(SPELL_FLAME_BREATH);
				FlameBreathTimer = urand(6000, 7000);
			}
			else FlameBreathTimer -= diff;

			if (HealthBelowPct(30))
				DoCast(me, SPELL_CALL_OF_EARTH);

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fjornAI(creature);
    }
};

enum SeethingRevenants
{
	SPELL_SEETHING_FLAME       = 56620,
	SPELL_THROW_SNOWBALL       = 56753,
	SPELL_KILL_CREDIT_REVENANT = 56754,
};

class npc_seething_revenant : public CreatureScript
{
public:
    npc_seething_revenant() : CreatureScript("npc_seething_revenant") {}

    struct npc_seething_revenantAI : public QuantumBasicAI
    {
        npc_seething_revenantAI(Creature* creature) : QuantumBasicAI(creature) {}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SEETHING_FLAME);
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
        {
			if (spell->Id == SPELL_THROW_SNOWBALL)
			{
				if (Player* player = me->FindPlayerWithDistance(200.0f, true))
					caster->CastSpell(player, SPELL_KILL_CREDIT_REVENANT, true);
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
        return new npc_seething_revenantAI(creature);
    }
};

class npc_seething_revenant_phase_two : public CreatureScript
{
public:
    npc_seething_revenant_phase_two() : CreatureScript("npc_seething_revenant_phase_two") {}

    struct npc_seething_revenant_phase_twoAI : public QuantumBasicAI
    {
        npc_seething_revenant_phase_twoAI(Creature* creature) : QuantumBasicAI(creature) {}

		void Reset(){}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SEETHING_FLAME);
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
        return new npc_seething_revenant_phase_twoAI(creature);
    }
};

enum DeadIrongiant
{
	SPELL_REMANENT_DEATH     = 29266,
	SPELL_QUEST_INVIS_TIER_1 = 56779,
    SPELL_SALVAGE_CORPSE     = 56227,
    SPELL_CREATE_EYES        = 56230,
	NPC_AMBUSHER             = 30208,
};

class npc_dead_iron_giant : public CreatureScript
{
public:
    npc_dead_iron_giant() : CreatureScript("npc_dead_iron_giant") {}

    struct npc_dead_iron_giantAI : public QuantumBasicAI
    {
        npc_dead_iron_giantAI(Creature* creature) : QuantumBasicAI(creature) {}

		void Reset()
		{
			DoCast(me, SPELL_REMANENT_DEATH);
			DoCast(me, SPELL_QUEST_INVIS_TIER_1);
		}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_SALVAGE_CORPSE)
            {
                if (!urand(0, 2))
                {
                    for (uint8 i = 0; i < 3; ++i)
					{
                        if (Creature* ambusher = me->SummonCreature(NPC_AMBUSHER, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60*IN_MILLISECONDS))
                            ambusher->AI()->AttackStart(caster);
					}
                }
                else me->CastSpell(caster, SPELL_CREATE_EYES, true);

                me->DespawnAfterAction(0.5*IN_MILLISECONDS);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dead_iron_giantAI(creature);
    }
};

enum ScrapBunny
{
	SPELL_SUMMON_SMOLDERING_SCRAP  = 56073,
	SPELL_SUMMON_FROZEN_IRON_SCRAP = 56101,
	SPELL_THROW_ESSENCE_OF_ICE     = 56099,
	SPELL_SMOLDER_SMOKE            = 56118,
	GO_SMOLDERING_SCRAP            = 192124,
};

class npc_smoldering_scrap_bunny : public CreatureScript
{
public:
    npc_smoldering_scrap_bunny() : CreatureScript("npc_smoldering_scrap_bunny") {}

    struct npc_smoldering_scrap_bunnyAI : public QuantumBasicAI
    {
        npc_smoldering_scrap_bunnyAI(Creature* creature) : QuantumBasicAI(creature) {}

		void Reset()
		{
			DoCast(SPELL_SMOLDER_SMOKE);
			DoCast(SPELL_SUMMON_SMOLDERING_SCRAP);
		}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_THROW_ESSENCE_OF_ICE)
            {
				if (GameObject* scrap = me->FindGameobjectWithDistance(GO_SMOLDERING_SCRAP, 5.0f))
					scrap->Delete();

				DoCast(SPELL_SUMMON_FROZEN_IRON_SCRAP);
				me->DespawnAfterAction();
			}
        }

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_smoldering_scrap_bunnyAI(creature);
    }
};

enum BlowingHodirsHorn
{
	SPELL_BLOWS_HODIR_HORN     = 55983,
	SPELL_ICE_SPIRE            = 57454,
	NPC_KILL_CREDIT_DWARF      = 30139,
	NPC_KILL_CREDIT_FOREFATHER = 30138,
};

class npc_restless_frostborn_warrior : public CreatureScript
{
public:
    npc_restless_frostborn_warrior() : CreatureScript("npc_restless_frostborn_warrior") {}

    struct npc_restless_frostborn_warriorAI : public QuantumBasicAI
    {
		npc_restless_frostborn_warriorAI(Creature* creature) : QuantumBasicAI(creature) {}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_BLOWS_HODIR_HORN)
            {
				if (Player* player = caster->ToPlayer())
					caster->ToPlayer()->KilledMonsterCredit(NPC_KILL_CREDIT_DWARF, 0);

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
        return new npc_restless_frostborn_warriorAI(creature);
    }
};

class npc_niffelem_forefather : public CreatureScript
{
public:
    npc_niffelem_forefather() : CreatureScript("npc_niffelem_forefather") {}

    struct npc_niffelem_forefatherAI : public QuantumBasicAI
    {
		npc_niffelem_forefatherAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 IceSpireTimer;

		void Reset()
		{
			IceSpireTimer = 3000;
		}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_BLOWS_HODIR_HORN)
            {
				if (Player* player = caster->ToPlayer())
					caster->ToPlayer()->KilledMonsterCredit(NPC_KILL_CREDIT_FOREFATHER, 0);

				me->DespawnAfterAction();
			}
        }

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (IceSpireTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ICE_SPIRE);
					IceSpireTimer = urand(10000, 18000);
				}
			}
			else IceSpireTimer -= diff;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_niffelem_forefatherAI(creature);
    }
};

enum RoamingJormungar
{
	NPC_JORMUNGAR_KILL_CREDIT = 30421,
	SPELL_ARNGRIMS_TOOTH      = 56727,
	SPELL_SUMMON_ARNGRIM      = 56732,
};

class npc_roaming_jormungar : public CreatureScript
{
public:
    npc_roaming_jormungar() : CreatureScript("npc_roaming_jormungar") {}

    struct npc_roaming_jormungarAI : public QuantumBasicAI
    {
        npc_roaming_jormungarAI(Creature* creature) : QuantumBasicAI(creature) {}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_ARNGRIMS_TOOTH)
            {
				if (me->HealthBelowPct(20))
				{
					DoCast(SPELL_SUMMON_ARNGRIM);

					if (Player* player = caster->ToPlayer())
						caster->ToPlayer()->KilledMonsterCredit(NPC_JORMUNGAR_KILL_CREDIT, 0);

					caster->Kill(me);
				}
			}
        }

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_roaming_jormungarAI(creature);
    }
};

enum JormuttarIsSooFat
{
	SPELL_CARVE_BEAR_FLANK  = 56562,
	SPELL_CREATE_BEAR_FLANK = 56566,
	SPELL_LURE_JORMUNTAR    = 56573,
	NPC_JORMUTTAR           = 30340,
};

class npc_dead_icemaw_bear : public CreatureScript
{
public:
    npc_dead_icemaw_bear() : CreatureScript("npc_dead_icemaw_bear") {}

    struct npc_dead_icemaw_bearAI : public QuantumBasicAI
    {
        npc_dead_icemaw_bearAI(Creature* creature) : QuantumBasicAI(creature) {}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_CARVE_BEAR_FLANK)
			{
				if (Player* player = caster->ToPlayer())
					player->CastSpell(player, SPELL_CREATE_BEAR_FLANK, true);

				me->DisappearAndDie();
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dead_icemaw_bearAI(creature);
    }
};

class npc_lure_jormuttar_bunny : public CreatureScript
{
public:
    npc_lure_jormuttar_bunny() : CreatureScript("npc_lure_jormuttar_bunny") {}

    struct npc_lure_jormuttar_bunnyAI : public QuantumBasicAI
    {
        npc_lure_jormuttar_bunnyAI(Creature* creature) : QuantumBasicAI(creature) {}

		void Reset()
        {
			me->SummonCreature(NPC_JORMUTTAR, 7316.6f, -2051.19f, 761.354f, 0.547671f, TEMPSUMMON_CORPSE_DESPAWN, 500);
			me->DespawnAfterAction(0.3*IN_MILLISECONDS);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lure_jormuttar_bunnyAI(creature);
    }
};

enum WildWyrm
{
	SPELL_HODIR_SPEAR    = 56671,
	NPC_KILL_CREDIT_WYRM = 30415,
};

class npc_wild_wyrm : public CreatureScript
{
public:
    npc_wild_wyrm() : CreatureScript("npc_wild_wyrm") {}

    struct npc_wild_wyrmAI : public QuantumBasicAI
    {
        npc_wild_wyrmAI(Creature* creature) : QuantumBasicAI(creature) {}

		void Reset()
		{
			me->SetCanFly(true);
			me->SetUnitMovementFlags(MOVEMENTFLAG_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_HODIR_SPEAR)
            {
				if (Player* player = caster->ToPlayer())
					caster->ToPlayer()->KilledMonsterCredit(NPC_KILL_CREDIT_WYRM, 0);

				caster->Kill(me);
			}
		}
	};

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wild_wyrmAI(creature);
    }
};

enum Kirgaraak
{
	QUEST_THE_WARM_UP         = 12996,
	NPC_KIRGARAAK_KILL_CREDIT = 30221,
	FACTION_FRIENDLY          = 35,
};

class npc_kirgaraak : public CreatureScript
{
public:
    npc_kirgaraak() : CreatureScript("npc_kirgaraak") {}

    struct npc_kirgaraakAI : public QuantumBasicAI
    {
        npc_kirgaraakAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->RestoreFaction();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (Player* player = who->ToPlayer())
			{
				if (player->IsWithinDist(me, 15.0f) && player->GetQuestStatus(QUEST_THE_WARM_UP) == QUEST_STATUS_INCOMPLETE)
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
			}
		}

		void DamageTaken(Unit* attacker, uint32 &amount)
        {
            if (amount >= me->GetHealth())
            {
				if (Player* player = attacker->ToPlayer())
				{
					amount = 0;

					player->KilledMonsterCredit(NPC_KIRGARAAK_KILL_CREDIT, 0);
					me->SetCurrentFaction(FACTION_FRIENDLY);
					EnterEvadeMode();
				}
            }
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_kirgaraakAI(creature);
    }
};

enum ForgingAnAlliance
{
	QUEST_FORGING_AN_ALLIANCE = 12924,
};

class npc_king_jokkum : public CreatureScript
{
public:
    npc_king_jokkum() : CreatureScript("npc_king_jokkum") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_FORGING_AN_ALLIANCE) == QUEST_STATUS_INCOMPLETE)
		{
			player->CompleteQuest(QUEST_FORGING_AN_ALLIANCE);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }
        return false;
    }

    struct npc_king_jokkumAI : public QuantumBasicAI
    {
        npc_king_jokkumAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_king_jokkumAI(creature);
    }
};

enum AmpleInspiration
{
	SPELL_IRONWOOL_COAT          = 56356,
	SPELL_STAMPEDE               = 55644,
	SPELL_THROW_UDED             = 54577,
	SPELL_SUMMON_MEAT_OBJECT     = 54625,
	SPELL_SUMMON_MEAT_SPAWNER    = 54581,
	SPELL_TRIGGER_SUMMON_MEAT_1  = 54623,
	SPELL_TRIGGER_SUMMON_MEAT_2  = 54627,
	SPELL_TRIGGER_SUMMON_MEAT_3  = 54628,
	SPELL_SUMMON_MAMMOTH_CARCASS = 57444,

	NPC_WORLD_TRIGGER            = 22515,

	QUEST_AMPLE_INSPIRATION      = 12828,
};

class npc_ironwool_mammoth : public CreatureScript
{
public:
    npc_ironwool_mammoth() : CreatureScript("npc_ironwool_mammoth") {}

    struct npc_ironwool_mammothAI : public QuantumBasicAI
    {
        npc_ironwool_mammothAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 IronwoolCoatTimer;

        void Reset()
        {
			IronwoolCoatTimer = 1*IN_MILLISECONDS;

			me->SetVisible(true);

			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_THROW_UDED && !me->IsInCombatActive())
			{
				if (Player* player = caster->ToPlayer())
				{
					if (player->GetQuestStatus(QUEST_AMPLE_INSPIRATION) == QUEST_STATUS_INCOMPLETE)
					{
						DoCast(me, SPELL_TRIGGER_SUMMON_MEAT_1, true);
						DoCast(me, SPELL_TRIGGER_SUMMON_MEAT_2, true);
						DoCast(me, SPELL_TRIGGER_SUMMON_MEAT_3, true);
						DoCast(me, SPELL_SUMMON_MAMMOTH_CARCASS, true);
						me->SetVisible(false);

						if (Creature* trigger = me->SummonCreature(NPC_WORLD_TRIGGER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(),  me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 4*IN_MILLISECONDS))
						{
							trigger->SetObjectScale(2.0f);
							trigger->CastSpell(trigger, SPELL_SUMMON_MEAT_SPAWNER, true);
						}

						me->DespawnAfterAction(1*MINUTE*IN_MILLISECONDS);
					}
				}
			}
        }

		void JustDied(Unit* /*killer*/)
		{
			DoCast(me, SPELL_STAMPEDE, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (IronwoolCoatTimer <= diff)
			{
				DoCast(me, SPELL_IRONWOOL_COAT);
				IronwoolCoatTimer = 4*IN_MILLISECONDS;
			}
			else IronwoolCoatTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ironwool_mammothAI(creature);
    }
};

class npc_mammoth_meat_bunny : public CreatureScript
{
public:
    npc_mammoth_meat_bunny() : CreatureScript("npc_mammoth_meat_bunny") {}

    struct npc_mammoth_meat_bunnyAI : public QuantumBasicAI
    {
        npc_mammoth_meat_bunnyAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_SUMMON_MEAT_OBJECT);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mammoth_meat_bunnyAI(creature);
    }
};

enum QuestSlavesOfTheStormforged
{
	NPC_CREDIT_MECHAGNOME = 29962,

	GOSSIP_MECHAGNOME     = 9871,

	SAY_MECHAGNOME_RESQ_1 = -1420114,
	SAY_MECHAGNOME_RESQ_2 = -1420115,
	SAY_MECHAGNOME_RESQ_3 = -1420116,
	SAY_MECHAGNOME_RESQ_4 = -1420117,
	SAY_MECHAGNOME_RESQ_5 = -1420118,
};

class npc_captive_mechagnome : public CreatureScript
{
public:
    npc_captive_mechagnome() : CreatureScript("npc_captive_mechagnome") {}

    struct npc_captive_mechagnomeAI : public QuantumBasicAI
    {
        npc_captive_mechagnomeAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			me->SetCorpseDelay(0);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_WORK_MINING);
		}

		void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
		{
			if (menuId == GOSSIP_MECHAGNOME && gossipListId == 0)
			{
				player->KilledMonsterCredit(NPC_CREDIT_MECHAGNOME, 0);

				me->SetWalk(true);
				me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
				me->HandleEmoteCommand(EMOTE_ONESHOT_TALK);
				me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_NONE);

				DoSendQuantumText(RAND(SAY_MECHAGNOME_RESQ_1, SAY_MECHAGNOME_RESQ_2, SAY_MECHAGNOME_RESQ_3, SAY_MECHAGNOME_RESQ_4, SAY_MECHAGNOME_RESQ_5), me, player);

				me->GetMotionMaster()->MovePoint(0, 7818.4f, -86.48f, 880.63f);
				me->DespawnAfterAction(2*IN_MILLISECONDS);

				player->PlayerTalkClass->SendCloseGossip();
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
        return new npc_captive_mechagnomeAI(creature);
    }
};

void AddSC_storm_peaks()
{
    new npc_agnetta_tyrsdottar();
    new npc_frostborn_scout();
    new npc_thorim();
    new npc_victorious_challenger();
    new npc_loklira_crone();
    new npc_injured_goblin();
    new npc_roxi_ramrocket();
    new npc_niffelem_frost_giant();
	new npc_frostworg();
	new npc_captive_proto_drake();
	new npc_freed_proto_drake();
	new npc_brunnhildar_prisoner();
    new npc_injured_icemaw();
    new npc_harnessed_icemaw();
    new npc_hyldsmeet_protodrake();
    new npc_snowblind_follower();
	new npc_archivist_mechaton();
	new npc_column_ornament();
	new npc_item_branns_communicator();
	new npc_hyldsmeet_warbear();
	new npc_stormforged_iron_giant();
	new npc_fjorn();
	new npc_seething_revenant();
	new npc_seething_revenant_phase_two();
	new npc_dead_iron_giant();
	new npc_smoldering_scrap_bunny();
	new npc_restless_frostborn_warrior();
	new npc_niffelem_forefather();
	new npc_roaming_jormungar();
	new npc_dead_icemaw_bear();
	new npc_lure_jormuttar_bunny();
	new npc_wild_wyrm();
	new npc_kirgaraak();
	//new npc_king_jokkum();
	new npc_ironwool_mammoth();
	new npc_mammoth_meat_bunny();
	new npc_captive_mechagnome();
}