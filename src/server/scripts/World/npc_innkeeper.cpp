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
#include "QuantumSystemText.h"
#include "GameEventMgr.h"

enum Inkeeper
{
	HALLOWEEN_EVENT_ID     = 12,

	SPELL_TRICK_OR_TREATED = 24755,
	SPELL_TREAT            = 24715,
};

class npc_innkeeper : public CreatureScript
{
public:
    npc_innkeeper() : CreatureScript("npc_innkeeper") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
		if (sGameEventMgr->IsActiveEvent(HALLOWEEN_EVENT_ID) && !player->HasAura(SPELL_TRICK_OR_TREATED))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_INNKEEPER_TRICK_OR_TREAT), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+HALLOWEEN_EVENT_ID);

        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->IsVendor())
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_VENDOR_BROWSE_YOUR_GOODS), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        if (creature->IsInnkeeper())
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_INNKEEPER_CREATE_HOME), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INN);

        player->TalkedToCreature(creature->GetEntry(), creature->GetGUID());
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF+ HALLOWEEN_EVENT_ID && IsEventActive(HALLOWEEN_EVENT_ID) && !player->HasAura(SPELL_TRICK_OR_TREATED))
        {
            player->CastSpell(player, SPELL_TRICK_OR_TREATED, true);

            if (urand(0, 1))
                player->CastSpell(player, SPELL_TREAT, true);
            else
            {
                uint32 TrickSpell = 0;

                switch (urand(0, 13))
                {
                    case 0:
						TrickSpell = 24753;
						break; // cannot cast, random 30sec
                    case 1:
						TrickSpell = 24713;
						break; // lepper gnome costume
                    case 2:
						TrickSpell = 24735;
						break; // male ghost costume
                    case 3: 
						TrickSpell = 24736;
						break; // female ghostcostume
                    case 4:
						TrickSpell = 24710;
						break; // male ninja costume
                    case 5:
						TrickSpell = 24711;
						break; // female ninja costume
                    case 6:
						TrickSpell = 24708;
						break; // male pirate costume
                    case 7:
						TrickSpell = 24709;
						break; // female pirate costume
                    case 8:
						TrickSpell = 24723;
						break; // skeleton costume
                    case 9:
						TrickSpell = 24753;
						break; // Trick
                    case 10:
						TrickSpell = 24924;
						break; // Hallow's End Candy
                    case 11:
						TrickSpell = 24925;
						break; // Hallow's End Candy
                    case 12:
						TrickSpell = 24926;
						break; // Hallow's End Candy
                    case 13:
						TrickSpell = 24927;
						break; // Hallow's End Candy
                }
                player->CastSpell(player, TrickSpell, true);
            }
            player->CLOSE_GOSSIP_MENU();
            return true;
        }

        player->CLOSE_GOSSIP_MENU();

        switch (action)
        {
            case GOSSIP_ACTION_TRADE:
				player->GetSession()->SendListInventory(creature->GetGUID());
				break;
            case GOSSIP_ACTION_INN:
				player->SetBindPoint(creature->GetGUID());
				break;
        }
        return true;
    }
};

void AddSC_npc_innkeeper()
{
    new npc_innkeeper;
}