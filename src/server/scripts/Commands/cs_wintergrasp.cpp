/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "Chat.h"
#include "QuantumSystemText.h"
#include "ScriptMgr.h"
#include "OutdoorPvPMgr.h"
#include "OutdoorPvPWG.h"
#include "Config.h"

class wintergrasp_commandscript : public CommandScript
{
public:
    wintergrasp_commandscript() : CommandScript("wintergrasp_commandscript") { }

    ChatCommand* GetCommands() const
    {
        static ChatCommand wintergraspCommandTable[] =
        {
			{ "status",         SEC_ADMINISTRATOR,  false, &HandleWintergraspStatusCommand,     "", NULL },
			{ "enable",         SEC_ADMINISTRATOR,  false, &HandleWintergraspEnableCommand,     "", NULL },
			{ "start",          SEC_ADMINISTRATOR,  false, &HandleWintergraspStartCommand,      "", NULL },
			{ "stop",           SEC_ADMINISTRATOR,  false, &HandleWintergraspStopCommand,       "", NULL },
			{ "switch",         SEC_ADMINISTRATOR,  false, &HandleWintergraspSwitchTeamCommand, "", NULL },
			{ "timer",          SEC_ADMINISTRATOR,  false, &HandleWintergraspTimerCommand,      "", NULL },
            { NULL,             0,                  false, NULL,                                "", NULL }
        };

        static ChatCommand commandTable[] =
        {
            { "wg",             SEC_ADMINISTRATOR,  false, NULL,            "", wintergraspCommandTable },
            { NULL,             0,                  false, NULL,                               "", NULL }
        };

        return commandTable;
    }

	static bool HandleWintergraspStatusCommand(ChatHandler* handler, char const* args)
	{
		OutdoorPvPWG* pvpWG = (OutdoorPvPWG*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(4197);

		if (!pvpWG || !sWorld->getBoolConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_ENABLED))
		{
			handler->SendSysMessage(LANG_BG_WG_DISABLE);
			handler->SetSentErrorMessage(true);
			return false;
		}

		handler->PSendSysMessage(LANG_BG_WG_STATUS, sObjectMgr->GetQuantumSystemTextForDBCLocale(
			pvpWG->GetDefenderTeam() == TEAM_ALLIANCE ? LANG_BG_AB_ALLY : LANG_BG_AB_HORDE),
			secsToTimeString(pvpWG->GetTimer(), true).c_str(),
			pvpWG->IsWarTime() ? "Yes" : "No",
			pvpWG->GetNumPlayersH(),
			pvpWG->GetNumPlayersA());
		return true;
	}

	static bool HandleWintergraspStartCommand(ChatHandler* handler, char const* args)
	{
		OutdoorPvPWG* pvpWG = (OutdoorPvPWG*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(4197);

		if (!pvpWG || !sWorld->getBoolConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_ENABLED))
		{
			handler->SendSysMessage(LANG_BG_WG_DISABLE);
			handler->SetSentErrorMessage(true);
			return false;
		}
		pvpWG->ForceStartBattle();
		handler->PSendSysMessage(LANG_BG_WG_BATTLE_FORCE_START);
		return true;
	}

	static bool HandleWintergraspStopCommand(ChatHandler* handler, char const* args)
	{
		OutdoorPvPWG* pvpWG = (OutdoorPvPWG*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(4197);

		if (!pvpWG || !sWorld->getBoolConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_ENABLED))
		{
			handler->SendSysMessage(LANG_BG_WG_DISABLE);
			handler->SetSentErrorMessage(true);
			return false;
		}
		pvpWG->ForceStopBattle();
		handler->PSendSysMessage(LANG_BG_WG_BATTLE_FORCE_STOP);
		return true;
	}

	static bool HandleWintergraspEnableCommand(ChatHandler* handler, char const* args)
	{
		if (!*args)
			return false;

		OutdoorPvPWG* pvpWG = (OutdoorPvPWG*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(4197);

		if (!pvpWG || !sWorld->getBoolConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_ENABLED))
		{
			handler->SendSysMessage(LANG_BG_WG_DISABLE);
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (!strncmp(args, "on", 3))
		{
			if (!sWorld->getBoolConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_ENABLED))
			{
				pvpWG->ForceStopBattle();
				sWorld->setBoolConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_ENABLED, true);
			}
			handler->PSendSysMessage(LANG_BG_WG_ENABLE);
			return true;
		}
		else if (!strncmp(args, "off", 4))
		{
			if (sWorld->getBoolConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_ENABLED))
			{
				pvpWG->ForceStopBattle();
				sWorld->setBoolConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_ENABLED, false);
			}
			handler->PSendSysMessage(LANG_BG_WG_DISABLE);
			return true;
		}
		else
		{
			handler->SendSysMessage(LANG_USE_BOL);
			handler->SetSentErrorMessage(true);
			return false;
		}
	}

	static bool HandleWintergraspTimerCommand(ChatHandler* handler, char const* args)
	{
		if (!*args)
			return false;

		OutdoorPvPWG* pvpWG = (OutdoorPvPWG*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(4197);

		if (!pvpWG)
		{
			handler->SendSysMessage(LANG_BG_WG_DISABLE);
			handler->SetSentErrorMessage(true);
			return false;
		}

		int32 time = atoi (args);

		// Min value 1 min
		if (1 > time)
			time = 1;
		// Max value during wartime = 60. No wartime = 1440 (day)
		if (pvpWG->IsWarTime())
		{
			if (60 < time)
				return false;
		}
		else
			if (1440 < time)
				return false;

		time *= MINUTE *IN_MILLISECONDS;

		pvpWG->setTimer((uint32)time);
		sWorld->SendWintergraspState(); //Update WG time at bg tab
		handler->PSendSysMessage(LANG_BG_WG_CHANGE_TIMER, secsToTimeString(pvpWG->GetTimer(), true).c_str());
		return true;
	}

	static bool HandleWintergraspSwitchTeamCommand(ChatHandler* handler, char const* args)
	{
		OutdoorPvPWG* pvpWG = (OutdoorPvPWG*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(4197);

		if (!pvpWG)
		{
			handler->SendSysMessage(LANG_BG_WG_DISABLE);
			handler->SetSentErrorMessage(true);
			return false;
		}
		uint32 timer = pvpWG->GetTimer();
		pvpWG->ForceChangeTeam();
		pvpWG->setTimer(timer);
		handler->PSendSysMessage(LANG_BG_WG_SWITCH_FACTION, handler->GetQuantumSystemText(pvpWG->GetDefenderTeam() == TEAM_ALLIANCE ? LANG_BG_AB_ALLY : LANG_BG_AB_HORDE));
		return true;
	}
};

void AddSC_wintergrasp_commandscript()
{
    new wintergrasp_commandscript();
}