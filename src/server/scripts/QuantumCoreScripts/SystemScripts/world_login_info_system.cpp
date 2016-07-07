/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "AccountMgr.h"
#include "ScriptMgr.h"
#include "TicketMgr.h"
#include "QuantumSystemText.h"
#include "Chat.h"
#include "Config.h"

class world_login_info_system : public PlayerScript
{
public:
	world_login_info_system() : PlayerScript("world_login_info_system") { }

	void OnLogin(Player* player)
	{
		if (sWorld->getBoolConfig(CONFIG_CHAR_LOGIN_INFO_ENABLE))
		{
			ChatHandler handler(player->GetSession());
			uint8 level = player->GetCurrentLevel();
			uint16 acclevel = player->GetSession()->GetSecurity();
			uint16 tickets = sTicketMgr->GetOpenTicketCount();
			uint32 online = sWorld->GetPlayerCount();
			std::string name = player->GetName();
			std::string uptime = secsToTimeString(sWorld->GetUptime());
			std::string player_ip = player->GetSession()->GetRemoteAddress();

			handler.PSendSysMessage(LANG_LOGIN_INFO_BORDER);
			handler.PSendSysMessage(LANG_LOGIN_INFO_HELLO, name.c_str());
			handler.PSendSysMessage(LANG_LOGIN_INFO_PLAYER_LEVEL, level);
			handler.PSendSysMessage(LANG_LOGIN_INFO_ACCOUNT_LEVEL, acclevel);
			handler.PSendSysMessage(LANG_LOGIN_INFO_PLAYER_IP, player_ip.c_str());

			if (player->GetSession()->IsPremiumAccount())
				handler.PSendSysMessage(LANG_LOGIN_INFO_PREMIUM_ACTIVE);
			else
				handler.PSendSysMessage(LANG_LOGIN_INFO_PREMIUM_NOT_ACTIVE);

			handler.PSendSysMessage(LANG_LOGIN_INFO_TOTAL_ONLINE , online);
			handler.PSendSysMessage(LANG_LOGIN_INFO_SERVER_UPTIME, uptime.c_str());

			if (player->GetSession()->GetSecurity() >= SEC_MODERATOR)
				handler.PSendSysMessage(LANG_LOGIN_INFO_OPENED_TICKETS, tickets);

			handler.PSendSysMessage(LANG_LOGIN_INFO_GOOD_LUCK);
			handler.PSendSysMessage(LANG_LOGIN_INFO_BORDER);
			return;
		}
	}
};

void AddSC_world_login_info_system()
{
	new world_login_info_system();
}