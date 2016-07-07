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
#include "magisters_terrace.h"

enum Misc
{
    SPELL_TRANSFORM_TO_KAEL     = 44670,
    SPELL_ORB_KILL_CREDIT       = 46307,
    POINT_ID_LAND               = 1,
};

const float afKaelLandPoint[] = {225.045f, -276.236f, -5.434f};

#define GOSSIP_ITEM_KAEL_1 "Who are you?"
#define GOSSIP_ITEM_KAEL_2 "What can we do to assist you?"
#define GOSSIP_ITEM_KAEL_3 "What brings you to the Sunwell?"
#define GOSSIP_ITEM_KAEL_4 "You're not alone here?"
#define GOSSIP_ITEM_KAEL_5 "What would Kil'jaeden want with a mortal woman?"

class npc_kalecgos : public CreatureScript
{
public:
    npc_kalecgos() : CreatureScript("npc_kalecgos") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAEL_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
        player->SEND_GOSSIP_MENU(12498, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAEL_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(12500, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAEL_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(12502, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAEL_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                player->SEND_GOSSIP_MENU(12606, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAEL_5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                player->SEND_GOSSIP_MENU(12607, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                player->SEND_GOSSIP_MENU(12608, creature->GetGUID());
                break;
        }
        return true;
    }

    struct npc_kalecgosAI : public QuantumBasicAI
    {
        npc_kalecgosAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 TransformTimer;

        void Reset()
        {
            TransformTimer = 0;

            if (me->GetEntry() != NPC_KAEL)
			{
                me->GetMotionMaster()->MovePoint(POINT_ID_LAND, afKaelLandPoint[0], afKaelLandPoint[1], afKaelLandPoint[2]);
			}
        }

        void MovementInform(uint32 Type, uint32 PointId)
        {
            if (Type != POINT_MOTION_TYPE)
                return;

            if (PointId == POINT_ID_LAND)
			{
                TransformTimer = MINUTE*IN_MILLISECONDS;
			}
        }

        // some targeting issues with the spell, so use this workaround as temporary solution
        void DoWorkaroundForQuestCredit()
        {
            Map* map = me->GetMap();

            if (!map || map->IsHeroic())
                return;

            Map::PlayerList const &lList = map->GetPlayers();

            if (lList.isEmpty())
                return;

            SpellInfo const* spell = sSpellMgr->GetSpellInfo(SPELL_ORB_KILL_CREDIT);

            for (Map::PlayerList::const_iterator i = lList.begin(); i != lList.end(); ++i)
            {
                if (Player* player = i->getSource())
                {
                    if (spell && spell->Effects[0].MiscValue)
					{
                        player->KilledMonsterCredit(spell->Effects[0].MiscValue, 0);
					}
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (TransformTimer)
            {
                if (TransformTimer <= diff)
                {
                    DoCast(me, SPELL_ORB_KILL_CREDIT, false);
                    DoWorkaroundForQuestCredit();

                    DoCast(me, SPELL_TRANSFORM_TO_KAEL, false);
                    me->UpdateEntry(NPC_KAEL);

                    TransformTimer = 0;
                }
				else TransformTimer -= diff;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_kalecgosAI(creature);
    }
};

void AddSC_magisters_terrace()
{
    new npc_kalecgos();
}