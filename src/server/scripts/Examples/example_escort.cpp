/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
#include "CreatureTextMgr.h"

enum Datas
{
    NPC_FELBOAR                 = 21878,

    SPELL_DEATH_COIL            = 33130,
    SPELL_ELIXIR_OF_FORTITUDE   = 3593,
    SPELL_BLUE_FIREWORK         = 11540,

	SAY_AGGRO1                  = 0,
	SAY_AGGRO2                  = 1,
	SAY_WP_1                    = 2,
	SAY_WP_2                    = 3,
	SAY_WP_3                    = 4,
	SAY_WP_4                    = 5,
	SAY_DEATH_1                 = 6,
	SAY_DEATH_2                 = 7,
	SAY_DEATH_3                 = 8,
	SAY_SPELL                   = 9,
	SAY_RAND_1                  = 10,
	SAY_RAND_2                  = 11
};

#define GOSSIP_ITEM_1 "Click to Test Escort(Attack, Run)"
#define GOSSIP_ITEM_2 "Click to Test Escort(NoAttack, Walk)"
#define GOSSIP_ITEM_3 "Click to Test Escort(NoAttack, Run)"

class example_escort : public CreatureScript
{
    public:
		example_escort() : CreatureScript("example_escort") { }

		bool OnGossipHello(Player* player, Creature* creature)
        {
            player->TalkedToCreature(creature->GetEntry(), creature->GetGUID());
            player->PrepareGossipMenu(creature, 0);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            player->SendPreparedGossip(creature);
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            npc_escortAI* EscortAI = CAST_AI(example_escort::example_escortAI, creature->AI());

            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF+1:
                    player->CLOSE_GOSSIP_MENU();

                    if (EscortAI)
                        EscortAI->Start(true, true, player->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+2:
                    player->CLOSE_GOSSIP_MENU();

                    if (EscortAI)
                        EscortAI->Start(false, false, player->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+3:
                    player->CLOSE_GOSSIP_MENU();

                    if (EscortAI)
                        EscortAI->Start(false, true, player->GetGUID());
                    break;
                default:
                    return false;
            }
            return true;
        }

        struct example_escortAI : public npc_escortAI
        {
            example_escortAI(Creature* creature) : npc_escortAI(creature) { }

            uint32 DeathCoilTimer;
            uint32 ChatTimer;

            void JustSummoned(Creature* summoned)
            {
                summoned->AI()->AttackStart(me);
            }

            void WaypointReached(uint32 id)
            {
                switch (id)
                {
                    case 1:
                        Talk(SAY_WP_1);
                        break;
                    case 3:
                        Talk(SAY_WP_2);
                        me->SummonCreature(NPC_FELBOAR, me->GetPositionX()+5.0f, me->GetPositionY()+7.0f, me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 3000);
                        break;
                    case 4:
                        if (Player* player = GetPlayerForEscort())
                        {
                            //pTmplayer is the target of the text
                            Talk(SAY_WP_3, player->GetGUID());
                            //pTmplayer is the source of the text
							sCreatureTextMgr->SendChat(me, SAY_WP_4, 0, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_NORMAL, 0, TEAM_OTHER, false, player);
                        }
                        break;
                }
            }

            void EnterToBattle(Unit* /*who*/)
            {
                if (HasEscortState(STATE_ESCORT_ESCORTING))
                {
                    if (Player* player = GetPlayerForEscort())
                        Talk(SAY_AGGRO1, player->GetGUID());
                }
                else Talk(SAY_AGGRO2);
            }

            void Reset()
            {
                DeathCoilTimer = 4000;
                ChatTimer = 4000;
            }

            void JustDied(Unit* killer)
            {
                if (HasEscortState(STATE_ESCORT_ESCORTING))
                {
                    if (Player* player = GetPlayerForEscort())
                    {
                        if (killer == me)
                        {
                            Talk(SAY_DEATH_1, player->GetGUID());
                        }
                        else Talk(SAY_DEATH_2, player->GetGUID());
                    }
                }
                else Talk(SAY_DEATH_3);
            }

            void UpdateAI(const uint32 diff)
            {
                npc_escortAI::UpdateAI(diff);

                if (me->GetVictim())
                {
                    if (DeathCoilTimer <= diff)
                    {
                        Talk(SAY_SPELL);
                        DoCastVictim(SPELL_DEATH_COIL, false);
                        DeathCoilTimer = 4000;
                    }
                    else DeathCoilTimer -= diff;
                }
                else
                {
                    if (HasEscortState(STATE_ESCORT_ESCORTING))
                    {
                        if (ChatTimer <= diff)
                        {
                            if (me->HasAura(SPELL_ELIXIR_OF_FORTITUDE, 0))
                            {
                                Talk(SAY_RAND_1);
                                DoCast(me, SPELL_BLUE_FIREWORK, false);
                            }
                            else
                            {
                                Talk(SAY_RAND_2);
                                DoCast(me, SPELL_ELIXIR_OF_FORTITUDE, false);
                            }

                            ChatTimer = 12000;
                        }
                        else ChatTimer -= diff;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new example_escortAI(creature);
        }
};

void AddSC_example_escort()
{
    new example_escort();
}