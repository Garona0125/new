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
#include "OutdoorPvPMgr.h"
#include "OutdoorPvPWG.h"
#include "Vehicle.h"

#define GOSSIP_HELLO_DEMO1 "Build catapult."
#define GOSSIP_HELLO_DEMO2 "Build demolisher."
#define GOSSIP_HELLO_DEMO3 "Build siege engine."
#define GOSSIP_HELLO_DEMO4 "I cannot build more!"

#define GOSSIP_HELLO_SPIRIT1 "Guide me to the Fortress Graveyard."
#define GOSSIP_HELLO_SPIRIT2 "Guide me to the Sunken Ring Graveyard."
#define GOSSIP_HELLO_SPIRIT3 "Guide me to the Broken Temple Graveyard."
#define GOSSIP_HELLO_SPIRIT4 "Guide me to the Westspark Graveyard."
#define GOSSIP_HELLO_SPIRIT5 "Guide me to the Eastspark Graveyard."
#define GOSSIP_HELLO_SPIRIT6 "Guide me back to the Horde landing camp."
#define GOSSIP_HELLO_SPIRIT7 "Guide me back to the Alliance landing camp."

enum WinterGrasp
{
	NPC_WINTERGRASP_CONTROL_ARMS = 27852,
};

class npc_demolisher_engineerer : public CreatureScript
{
public:
	npc_demolisher_engineerer() : CreatureScript("npc_demolisher_engineerer") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (creature->IsQuestGiver())
			player->PrepareQuestMenu(creature->GetGUID());

		if (player->IsGameMaster() || creature->GetZoneScript() && creature->GetZoneScript()->GetData(creature->GetDBTableGUIDLow()))
		{
			if (player->HasAura(SPELL_CORPORAL))
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_DEMO1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

			else if (player->HasAura(SPELL_LIEUTENANT))
			{
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_DEMO1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_DEMO2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_DEMO3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
			}
		}
		else
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_DEMO4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+9);

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
	{
		player->CLOSE_GOSSIP_MENU();

		if (player->IsGameMaster() || creature->GetZoneScript() && creature->GetZoneScript()->GetData(creature->GetDBTableGUIDLow()))
		{
			switch (action - GOSSIP_ACTION_INFO_DEF)
			{
			    case 0:
					player->CastSpell(player, 56663, false, 0, 0, creature->GetGUID());
					break;
				case 1:
					player->CastSpell(player, 56575, false, 0, 0, creature->GetGUID());
					break;
				case 2:
					player->CastSpell(player, player->GetTeamId() ? 61408 : 56661, false, 0, 0, creature->GetGUID());
					break;
			}

			if (Creature* controlArms = creature->FindCreatureWithDistance(NPC_WINTERGRASP_CONTROL_ARMS, 30.0f, true))
				creature->CastSpell(controlArms, SPELL_ACTIVATE_CONTROL_ARMS, true);
		}
		return true;
	}

	struct npc_demolisher_engineererAI : public QuantumBasicAI
	{
		npc_demolisher_engineererAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->SetReactState(REACT_PASSIVE);
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_demolisher_engineererAI(creature);
	}
};

enum DalaranBattlemage
{
	SPELL_FROST_ARMOR            = 12544,

	WG_NPC_QUEUE_TEXT_H_NOWAR    = 14775,
	WG_NPC_QUEUE_TEXT_H_QUEUE    = 14790,
	WG_NPC_QUEUE_TEXT_H_WAR      = 14777,
	WG_NPC_QUEUE_TEXT_A_NOWAR    = 14782,
	WG_NPC_QUEUE_TEXT_A_QUEUE    = 14791,
	WG_NPC_QUEUE_TEXT_A_WAR      = 14781,

	WG_NPC_QUEUE_TEXTOPTION_JOIN = -1850507,
};

class npc_wg_dalaran_queue : public CreatureScript
{
public:
   npc_wg_dalaran_queue() : CreatureScript("npc_wg_dalaran_queue") { }

   bool OnGossipHello(Player* player, Creature* creature)
   {
       if (creature->IsQuestGiver())
		   player->PrepareQuestMenu(creature->GetGUID());

	   OutdoorPvPWG* BfWG = (OutdoorPvPWG*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(4197);

	   if (BfWG && player->GetCurrentLevel() > sWorld->getIntConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_MIN_LEVEL))
       {
           if (BfWG->IsWarTime())
           {
               if (!BfWG->IsWarForTeamFull(player))
               {
                   player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(WG_NPC_QUEUE_TEXTOPTION_JOIN), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
                   player->SEND_GOSSIP_MENU(BfWG->GetDefenderTeam()?WG_NPC_QUEUE_TEXT_H_WAR:WG_NPC_QUEUE_TEXT_A_WAR, creature->GetGUID());
               }
               else
				   player->SEND_GOSSIP_MENU(BfWG->GetDefenderTeam()?WG_NPC_QUEUE_TEXT_H_NOWAR:WG_NPC_QUEUE_TEXT_A_NOWAR, creature->GetGUID());
		   }
           else
           {
               uint32 Time = BfWG->GetTimer();

               player->SendUpdateWorldState(4354, time(NULL)+Time);

               if (Time<15*MINUTE)
               {
                   player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(WG_NPC_QUEUE_TEXTOPTION_JOIN), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
                   player->SEND_GOSSIP_MENU(BfWG->GetDefenderTeam()? WG_NPC_QUEUE_TEXT_H_QUEUE:WG_NPC_QUEUE_TEXT_A_QUEUE, creature->GetGUID());
               }
               else
				   player->SEND_GOSSIP_MENU(BfWG->GetDefenderTeam()?WG_NPC_QUEUE_TEXT_H_NOWAR:WG_NPC_QUEUE_TEXT_A_NOWAR, creature->GetGUID());
           }
       }
       return true;
   }

   bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 /*action*/)
   {
	   player->CLOSE_GOSSIP_MENU();

       OutdoorPvPWG* BfWG = (OutdoorPvPWG*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(4197);

       if (BfWG && player->GetCurrentLevel() > sWorld->getIntConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_MIN_LEVEL))
       {
           if (BfWG->IsWarTime())
           {
			   if (!BfWG->IsWarForTeamFull(player))
				   BfWG->InvitePlayerToWar(player);
		   }
           else
           {
               uint32 Time = BfWG->GetTimer();

               if (Time < 15*MINUTE)
                   BfWG->InvitePlayerToQueue(player);
           }
       }
       return true;
   }

   struct npc_wg_dalaran_queueAI : public QuantumBasicAI
   {
	   npc_wg_dalaran_queueAI(Creature* creature) : QuantumBasicAI(creature){}

	   void Reset()
	   {
		   me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
	   }

	   void EnterToBattle(Unit* /*who*/){}

	   void UpdateAI(uint32 const /*diff*/)
	   {
		   if (!me->HasAura(SPELL_FROST_ARMOR) && !me->IsInCombatActive())
			   DoCast(me, SPELL_FROST_ARMOR);

		   if (!UpdateVictim())
			   return;

		   DoMeleeAttackIfReady();
	   }
   };

   CreatureAI* GetAI(Creature* creature) const
   {
	   return new npc_wg_dalaran_queueAI(creature);
   }
};

enum VehicleTeleporter
{
	GO_FORTRESS_GATE = 190375,
};

class go_wg_veh_teleporter : public GameObjectScript
{
public:
   go_wg_veh_teleporter() : GameObjectScript("go_wg_veh_teleporter") { }

   bool OnGossipHello(Player* player, GameObject* go)
   {
       if (GameObject* trigger = go->FindGameobjectWithDistance(GO_FORTRESS_GATE, 500.0f))
       {
           if (Vehicle* vehicle = player->GetVehicle())
           {
               Position triggerPos;
               trigger->GetPosition(&triggerPos);
               triggerPos.m_positionX -= 30;
               vehicle->Relocate(triggerPos);
           }
       }
	   return true;
   }
};

enum WintergraspTeleportSpells
{
	SPELL_TELEPORT_FORTRESS_GRAVEYARD = 59760,
	SPELL_TELEPORT_SUNKEN_RING        = 59762,
	SPELL_TELEPORT_BROKEN_TEMPLE      = 59763,
	SPELL_TELEPORT_WESTSPARK_FACTORY  = 59766,
	SPELL_TELEPORT_EASTSPARK_FACTORY  = 59767,
	SPELL_TELEPORT_HORDE_LANDING      = 59765,
	SPELL_TELEPORT_ALLIANCE_LANDING   = 59769,
};

class npc_wg_spirit_guide : public CreatureScript
{
public:
   npc_wg_spirit_guide() : CreatureScript("npc_wg_spirit_guide") { }

   bool OnGossipHello(Player* player, Creature* creature)
   {
       if (OutdoorPvPWG* pvpWG = (OutdoorPvPWG*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(4197))
	   {
           if (pvpWG->IsWarTime())
           {
			   if (pvpWG->GetDefenderTeam() == TEAM_HORDE)
			   {
				   if (player->GetTeam() == TEAM_ALLIANCE)
				   {
					   player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_SPIRIT4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
					   player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_SPIRIT5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
					   player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_SPIRIT7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
				   }
				   else
				   {
					   player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_SPIRIT1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
					   player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_SPIRIT2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
					   player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_SPIRIT3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
					   player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_SPIRIT6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
				   }
			   }
			   else
			   {
				   if (player->GetTeam() == TEAM_ALLIANCE)
				   {
					   player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_SPIRIT1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
					   player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_SPIRIT2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
					   player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_SPIRIT3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
					   player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_SPIRIT7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
				   }
				   else
				   {
					   player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_SPIRIT4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
					   player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_SPIRIT5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
					   player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_SPIRIT6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
				   }
			   }
           }
       }

       player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
       return true;
   }

   bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
   {
       player->CLOSE_GOSSIP_MENU();

       if (player->IsGameMaster() || creature->GetZoneScript() && creature->GetZoneScript()->GetData(creature->GetDBTableGUIDLow()))
       {
           switch (action - GOSSIP_ACTION_INFO_DEF)
           {
		       case 0:
				   player->CastSpell(player, SPELL_TELEPORT_FORTRESS_GRAVEYARD, false, 0, 0, creature->GetGUID());
				   break;
			   case 1:
				   player->CastSpell(player, SPELL_TELEPORT_SUNKEN_RING, false, 0, 0, creature->GetGUID());
				   break;
			   case 2:
				   player->CastSpell(player, SPELL_TELEPORT_BROKEN_TEMPLE, false, 0, 0, creature->GetGUID());
                   break;
			   case 3:
				   player->CastSpell(player, SPELL_TELEPORT_WESTSPARK_FACTORY, false, 0, 0, creature->GetGUID());
                   break;
			   case 4:
				   player->CastSpell(player, SPELL_TELEPORT_EASTSPARK_FACTORY, false, 0, 0, creature->GetGUID());
                   break;
			   case 5:
				   player->CastSpell(player, SPELL_TELEPORT_HORDE_LANDING, false, 0, 0, creature->GetGUID());
				   break;
			   case 6:
				   player->CastSpell(player, SPELL_TELEPORT_ALLIANCE_LANDING, false, 0, 0, creature->GetGUID());
                   break;	
		   }
	   }
	   return true;
   }

   struct npc_wg_spirit_guideAI : public QuantumBasicAI
   {
	   npc_wg_spirit_guideAI(Creature* creature) : QuantumBasicAI(creature){}

	   void Reset()
	   {
		   me->SetReactState(REACT_PASSIVE);
		   me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
	   }
   };

   CreatureAI* GetAI(Creature* creature) const
   {
       return new npc_wg_spirit_guideAI(creature);
   }
};

class npc_wintergrasp_guard : public CreatureScript
{
public:
    npc_wintergrasp_guard() : CreatureScript("npc_wintergrasp_guard") { }

    struct npc_wintergrasp_guardAI : public CreatureAI
    {
        npc_wintergrasp_guardAI(Creature* creature) : CreatureAI(creature) {}

        uint32 StrikeTimer;

        void Reset()
        {
            StrikeTimer = 2000;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (StrikeTimer <= diff)
            {
                DoCastVictim(SPELL_STRIKE);
                StrikeTimer = urand(4000, 6000);
            }
			else StrikeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wintergrasp_guardAI(creature);
    }
};

enum WintergraspVehicle
{
	EVENT_CHECK_AURA = 1,
};

class npc_wintergrasp_vehicle : public CreatureScript
{
public:
    npc_wintergrasp_vehicle() : CreatureScript("npc_wintergrasp_vehicle") { }

    struct npc_wintergrasp_vehicleAI : public VehicleAI
    {
        npc_wintergrasp_vehicleAI(Creature* creature) : VehicleAI(creature)
		{
			Reset();
		}

        EventMap events;

        void Reset()
        {
            events.ScheduleEvent(EVENT_CHECK_AURA, 0.5*IN_MILLISECONDS);
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

            switch (events.ExecuteEvent())
            {
				if (me->IsInCombatActive())
					return;

				if (me->HasAura(SPELL_ALLIANCE_FLAG_VISUAL) || me->HasAura(SPELL_HORDE_FLAG_VISUAL))
					return;

				case EVENT_CHECK_AURA:
					if (Unit* passenger = me->GetVehicleKit()->GetPassenger(0))
					{
						if (passenger && passenger->GetTypeId() == TYPE_ID_PLAYER)
						{
							// ALLIANCE
							if (passenger->ToPlayer()->GetTeamId() == TEAM_ALLIANCE)
							{
								me->RemoveAura(SPELL_HORDE_FLAG_VISUAL);
								me->AddAura(SPELL_ALLIANCE_FLAG_VISUAL, me);
							}

							// HORDE
							else if (passenger->ToPlayer()->GetTeamId() == TEAM_HORDE)
							{
								me->RemoveAura(SPELL_ALLIANCE_FLAG_VISUAL);
								me->AddAura(SPELL_HORDE_FLAG_VISUAL, me);
							}
						}
					}
					else events.ScheduleEvent(EVENT_CHECK_AURA, 5*IN_MILLISECONDS);
					break;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wintergrasp_vehicleAI(creature);
    }
};

class spell_wintergrasp_grab_passenger : public SpellScriptLoader
{
    public:
        spell_wintergrasp_grab_passenger() : SpellScriptLoader("spell_wintergrasp_grab_passenger") { }

        class spell_wintergrasp_grab_passenger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_wintergrasp_grab_passenger_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Player* caster = GetHitPlayer())
                    caster->CastSpell(GetCaster(), SPELL_RIDE_WG_VEHICLE, false);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_wintergrasp_grab_passenger_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_wintergrasp_grab_passenger_SpellScript();
        }
};

void AddSC_wintergrasp()
{
   new npc_demolisher_engineerer();
   new npc_wg_dalaran_queue();
   new go_wg_veh_teleporter();
   new npc_wg_spirit_guide();
   new npc_wintergrasp_guard();
   new npc_wintergrasp_vehicle();
   new spell_wintergrasp_grab_passenger();
}