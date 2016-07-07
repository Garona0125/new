/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "QuantumGossip.h"
#include "QuantumEscortAI.h"
#include "blackfathom_deeps.h"

enum Spells
{
    SPELL_BLESSING_OF_BLACKFATHOM = 8733,
    SPELL_RAVAGE                  = 8391,
    SPELL_FROST_NOVA              = 865,
    SPELL_FROST_BOLT_VOLLEY       = 8398,
    SPELL_TELEPORT_DARNASSUS      = 9268,
};

#define GOSSIP_ITEM_MORRIDUNE "Please teleport me to Darnassus"

const Position HomePosition = { -815.817f, -145.299f, -25.870f, 0 };

class go_blackfathom_altar : public GameObjectScript
{
public:
    go_blackfathom_altar() : GameObjectScript("go_blackfathom_altar") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/)
    {
        if (!player->HasAura(SPELL_BLESSING_OF_BLACKFATHOM))
            player->AddAura(SPELL_BLESSING_OF_BLACKFATHOM, player);
        return true;
    }
};

class go_blackfathom_fire : public GameObjectScript
{
public:
    go_blackfathom_fire() : GameObjectScript("go_blackfathom_fire") { }

    bool OnGossipHello(Player* /*player*/, GameObject* go)
    {
        InstanceScript* instance = go->GetInstanceScript();

        if (instance)
        {
            go->SetGoState(GO_STATE_ACTIVE);
            go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
            instance->SetData(DATA_FIRE, instance->GetData(DATA_FIRE) + 1);
            return true;
        }
        return false;
    }
};

class npc_blackfathom_deeps_event : public CreatureScript
{
public:
    npc_blackfathom_deeps_event() : CreatureScript("npc_blackfathom_deeps_event") { }

    struct npc_blackfathom_deeps_eventAI : public QuantumBasicAI
    {
        npc_blackfathom_deeps_eventAI(Creature* creature) : QuantumBasicAI(creature)
        {
            if (creature->IsSummon())
            {
                creature->SetHomePosition(HomePosition);
                AttackPlayer();
            }

            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 RavageTimer;
        uint32 FrostNovaTimer;
        uint32 FrostBoltVolleyTimer;

        bool bFlee;

        void Reset()
        {
            bFlee = false;

            RavageTimer = urand(5000, 8000);
            FrostNovaTimer = urand(9000, 12000);
            FrostBoltVolleyTimer = urand(2000, 4000);
        }

        void AttackPlayer()
        {
            Map::PlayerList const &PlList = me->GetMap()->GetPlayers();

            if (PlList.isEmpty())
                return;

            for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
            {
                if (Player* player = i->getSource())
                {
                    if (player->IsGameMaster())
                        continue;

                    if (player->IsAlive())
                    {
                        me->SetInCombatWith(player);
                        player->SetInCombatWith(me);
                        me->AddThreat(player, 0.0f);
                    }
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            switch (me->GetEntry())
            {
                case NPC_AKU_MAI_SNAPJAW:
                {
                    if (RavageTimer <= diff)
                    {
                        DoCastVictim(SPELL_RAVAGE);
                        RavageTimer = urand(9000, 14000);
                    }
					else RavageTimer -= diff;
                    break;
                }
                case NPC_MURKSHALLOW_SOFTSHELL:
                case NPC_BARBED_CRUSTACEAN:
                {
                    if (!bFlee && HealthBelowPct(15))
                    {
                        bFlee = true;
                        me->DoFleeToGetAssistance();
                    }
                    break;
                }
                case NPC_AKU_MAI_SERVANT:
                {
                    if (FrostBoltVolleyTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                        {
							DoCast(target, SPELL_FROST_BOLT_VOLLEY);
							FrostBoltVolleyTimer = urand(5000, 8000);
						}
                    } 
                    else FrostBoltVolleyTimer -= diff;
                    
                    if (FrostNovaTimer <= diff)
                    {
                        DoCastAOE(SPELL_FROST_NOVA, false);
                        FrostNovaTimer = urand(25000, 30000);
                    } 
                    else FrostNovaTimer -= diff;
                    break;
                }
            }

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* /*killer*/)
        {
            if (me->IsSummon()) //we are not a normal spawn.
                if (instance)
                    instance->SetData(DATA_EVENT, instance->GetData(DATA_EVENT) + 1);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_blackfathom_deeps_eventAI(creature);
    }
};

enum Morridune
{
    SAY_MORRIDUNE_1 = -1048003,
    SAY_MORRIDUNE_2 = -1048004,
};

class npc_morridune : public CreatureScript
{
public:
    npc_morridune() : CreatureScript("npc_morridune") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_MORRIDUNE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelectMenu(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->TeleportTo(1, 9952.239f, 2284.277f, 1341.394f, 1.595f);
                player->CLOSE_GOSSIP_MENU();
                break;
        }
        return true;
    }

    struct npc_morriduneAI : public npc_escortAI
    {
        npc_morriduneAI(Creature* creature) : npc_escortAI(creature)
        {
            DoSendQuantumText(SAY_MORRIDUNE_1, creature);
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            Start(false, false, 0);
        }

        void WaypointReached(uint32 point)
        {
            switch (point)
            {
                case 4:
					SetEscortPaused(true);
                    me->SetOrientation(1.775791f);
                    me->SendMovementFlagUpdate();
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    DoSendQuantumText(SAY_MORRIDUNE_2, me);
                    break;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_morriduneAI(creature);
    }
};

void AddSC_blackfathom_deeps()
{
    new go_blackfathom_altar();
    new go_blackfathom_fire();
    new npc_blackfathom_deeps_event();
    new npc_morridune();
}