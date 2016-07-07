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

#include "Chat.h"
#include "QuantumSystemText.h"
#include "Player.h"
#include "ScriptMgr.h"

class cheat_commandscript : public CommandScript
{
public:
    cheat_commandscript() : CommandScript("cheat_commandscript") { }

    ChatCommand* GetCommands() const
    {

        static ChatCommand cheatCommandTable[] =
        {
            { "god",            SEC_GAME_MASTER,    false, &HandleGodModeCheatCommand,         "", NULL },
            { "casttime",       SEC_GAME_MASTER,    false, &HandleCasttimeCheatCommand,        "", NULL },
            { "cooldown",       SEC_GAME_MASTER,    false, &HandleCoolDownCheatCommand,        "", NULL },
            { "power",          SEC_GAME_MASTER,    false, &HandlePowerCheatCommand,           "", NULL },
            { "waterwalk",      SEC_GAME_MASTER,    false, &HandleWaterWalkCheatCommand,       "", NULL },
			{ "status",         SEC_GAME_MASTER,    false, &HandleCheatStatusCommand,          "", NULL },
            { "taxi",           SEC_GAME_MASTER,    false, &HandleTaxiCheatCommand,            "", NULL },
            { "explore",        SEC_GAME_MASTER,    false, &HandleExploreCheatCommand,         "", NULL },
            { NULL,             0,                  false, NULL,                               "", NULL }

        };

        static ChatCommand commandTable[] =
        {
            { "cheat",          SEC_GAME_MASTER,    false, NULL,                  "", cheatCommandTable },
            { NULL,             0,                  false, NULL,                               "", NULL }
        };
        return commandTable;
    }

    static bool HandleGodModeCheatCommand(ChatHandler* handler, const char* args)
    {
        if (!handler->GetSession() && !handler->GetSession()->GetPlayer())
            return false;

        std::string argstr = (char*)args;

        if (!*args)
            argstr = (handler->GetSession()->GetPlayer()->GetCommandStatus(CHEAT_GOD)) ? "off" : "on";

        if (argstr == "off")
        {
            handler->GetSession()->GetPlayer()->SetCommandStatusOff(CHEAT_GOD);
            handler->SendSysMessage("Godmode is OFF. You can take damage.");
            return true;
        }
        else if (argstr == "on")
        {
            handler->GetSession()->GetPlayer()->SetCommandStatusOn(CHEAT_GOD);
            handler->SendSysMessage("Godmode is ON. You won't take damage.");
            return true;
        }

        return false;
    }

    static bool HandleCasttimeCheatCommand(ChatHandler* handler, const char* args)
    {
        if (!handler->GetSession() && !handler->GetSession()->GetPlayer())
            return false;

        std::string argstr = (char*)args;

        if (!*args)
            argstr = (handler->GetSession()->GetPlayer()->GetCommandStatus(CHEAT_CASTTIME)) ? "off" : "on";

        if (argstr == "off")
        {
            handler->GetSession()->GetPlayer()->SetCommandStatusOff(CHEAT_CASTTIME);
            handler->SendSysMessage("CastTime Cheat is OFF. Your spells will have a casttime.");
            return true;
        }
        else if (argstr == "on")
        {
            handler->GetSession()->GetPlayer()->SetCommandStatusOn(CHEAT_CASTTIME);
            handler->SendSysMessage("CastTime Cheat is ON. Your spells won't have a casttime.");
            return true;
        }

        return false;
    }

    static bool HandleCoolDownCheatCommand(ChatHandler* handler, const char* args)
    {
        if (!handler->GetSession() && !handler->GetSession()->GetPlayer())
            return false;

        std::string argstr = (char*)args;

        if (!*args)
            argstr = (handler->GetSession()->GetPlayer()->GetCommandStatus(CHEAT_COOLDOWN)) ? "off" : "on";

        if (argstr == "off")
        {
            handler->GetSession()->GetPlayer()->SetCommandStatusOff(CHEAT_COOLDOWN);
            handler->SendSysMessage("Cooldown Cheat is OFF. You are on the global cooldown.");
            return true;
        }
        else if (argstr == "on")
        {
            handler->GetSession()->GetPlayer()->SetCommandStatusOn(CHEAT_COOLDOWN);
            handler->SendSysMessage("Cooldown Cheat is ON. You are not on the global cooldown.");
            return true;
        }

        return false;
    }

    static bool HandlePowerCheatCommand(ChatHandler* handler, const char* args)
    {
        if (!handler->GetSession() && !handler->GetSession()->GetPlayer())
            return false;

        std::string argstr = (char*)args;

        if (!*args)
            argstr = (handler->GetSession()->GetPlayer()->GetCommandStatus(CHEAT_POWER)) ? "off" : "on";

        if (argstr == "off")
        {
            handler->GetSession()->GetPlayer()->SetCommandStatusOff(CHEAT_POWER);
            handler->SendSysMessage("Power Cheat is OFF. You need mana/rage/energy to use spells.");
            return true;
        }
        else if (argstr == "on")
        {
            handler->GetSession()->GetPlayer()->SetCommandStatusOn(CHEAT_POWER);
            handler->SendSysMessage("Power Cheat is ON. You don't need mana/rage/energy to use spells.");
            return true;
        }

        return false;
    }

	static bool HandleCheatStatusCommand(ChatHandler* handler, const char* /*args*/)
	{
		Player* player = handler->GetSession()->GetPlayer();

		const char* enabled = "ON";
		const char* disabled = "OFF";

		handler->SendSysMessage(LANG_COMMAND_CHEAT_STATUS);
		handler->PSendSysMessage(LANG_COMMAND_CHEAT_GOD, player->GetCommandStatus(CHEAT_GOD) ? enabled : disabled);
		handler->PSendSysMessage(LANG_COMMAND_CHEAT_CD, player->GetCommandStatus(CHEAT_COOLDOWN) ? enabled : disabled);
		handler->PSendSysMessage(LANG_COMMAND_CHEAT_CT, player->GetCommandStatus(CHEAT_CASTTIME) ? enabled : disabled);
		handler->PSendSysMessage(LANG_COMMAND_CHEAT_POWER, player->GetCommandStatus(CHEAT_POWER) ? enabled : disabled);
		handler->PSendSysMessage(LANG_COMMAND_CHEAT_WW, player->GetCommandStatus(CHEAT_WATERWALK) ? enabled : disabled);
		handler->PSendSysMessage(LANG_COMMAND_CHEAT_TAXINODES, player->IsTaxiCheater() ? enabled : disabled);
		return true;
	}

    static bool HandleWaterWalkCheatCommand(ChatHandler* handler, const char* args)
    {
        if (!handler->GetSession() && !handler->GetSession()->GetPlayer())
            return false;

        std::string argstr = (char*)args;

        if (!*args)
            argstr = (handler->GetSession()->GetPlayer()->GetCommandStatus(CHEAT_WATERWALK)) ? "off" : "on";

        if (argstr == "off")
        {
            handler->GetSession()->GetPlayer()->SetCommandStatusOff(CHEAT_WATERWALK);
            handler->GetSession()->GetPlayer()->SetMovement(MOVE_LAND_WALK);                // OFF
            handler->SendSysMessage("Waterwalking is OFF. You can't walk on water.");
            return true;
        }
        else if (argstr == "on")
        {
            handler->GetSession()->GetPlayer()->SetCommandStatusOn(CHEAT_WATERWALK);
            handler->GetSession()->GetPlayer()->SetMovement(MOVE_WATER_WALK);               // ON
            handler->SendSysMessage("Waterwalking is ON. You can walk on water.");
            return true;
        }

        return false;
    }

	static bool HandleTaxiCheatCommand(ChatHandler* handler, const char* args)
    {
        std::string argstr = (char*)args;
        Player* target = handler->getSelectedPlayer();

        if (!target)
            target = handler->GetSession()->GetPlayer();
        else if (handler->HasLowerSecurity(target, 0)) // check online security
            return false;

        if (!*args)
            argstr = (target->IsTaxiCheater()) ? "off" : "on";

        if (argstr == "off")
        {
            target->SetTaxiCheater(false);
            handler->PSendSysMessage(LANG_YOU_REMOVE_TAXIS, handler->GetNameLink(target).c_str());
            if (handler->needReportToTarget(target))
                ChatHandler(target->GetSession()).PSendSysMessage(LANG_YOURS_TAXIS_REMOVED, handler->GetNameLink().c_str());
            return true;
        }
        else if (argstr == "on")
        {
            target->SetTaxiCheater(true);
            handler->PSendSysMessage(LANG_YOU_GIVE_TAXIS, handler->GetNameLink(target).c_str());
            if (handler->needReportToTarget(target))
                ChatHandler(target->GetSession()).PSendSysMessage(LANG_YOURS_TAXIS_ADDED, handler->GetNameLink().c_str());
            return true;
        }

        handler->SendSysMessage(LANG_USE_BOL);
        handler->SetSentErrorMessage(true);
        return false;
    }

    static bool HandleExploreCheatCommand(ChatHandler* handler, const char *args)
    {
        if (!*args)
            return false;

		 // std::int flag = (char*)args;
        int flag = atoi((char*)args);

        Player* target = handler->getSelectedPlayer();
		if (!target)
        {
            handler->SendSysMessage(LANG_NO_CHAR_SELECTED);
            handler->SetSentErrorMessage(true);
            return false;
        }

        if (flag != 0)
        {
            handler->PSendSysMessage(LANG_YOU_SET_EXPLORE_ALL, handler->GetNameLink(target).c_str());
            if (handler->needReportToTarget(target))
            ChatHandler(target).PSendSysMessage(LANG_YOURS_EXPLORE_SET_ALL, handler->GetNameLink().c_str());
        }
        else
        {
            handler->PSendSysMessage(LANG_YOU_SET_EXPLORE_NOTHING, handler->GetNameLink(target).c_str());
            if (handler->needReportToTarget(target))
                ChatHandler(target).PSendSysMessage(LANG_YOURS_EXPLORE_SET_NOTHING, handler->GetNameLink().c_str());
        }

        for (uint8 i = 0; i < PLAYER_EXPLORED_ZONES_SIZE; ++i)
        {
            if (flag != 0)
                handler->GetSession()->GetPlayer()->SetFlag(PLAYER_EXPLORED_ZONES_1+i, 0xFFFFFFFF);
            else
                handler->GetSession()->GetPlayer()->SetFlag(PLAYER_EXPLORED_ZONES_1+i, 0);
        }

        return true;
    }
};

void AddSC_cheat_commandscript()
{
    new cheat_commandscript();
}