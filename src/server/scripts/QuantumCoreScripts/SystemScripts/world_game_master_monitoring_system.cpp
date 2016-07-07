/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "AccountMgr.h"
#include "Config.h"

class game_master_monitoring_system : public PlayerScript
{ 
public:
	game_master_monitoring_system() : PlayerScript("game_master_monitoring_system") { }

	void OnLogin(Player* player)
	{
		if (sWorld->getBoolConfig(CONFIG_GM_ENTER_IN_GAME))
		{
			if (player->GetSession()->GetSecurity() == SEC_MODERATOR)
				sWorld->SendWorldText(LANG_MODERATOR_ENTER_IN_GAME, player->GetSession()->GetPlayerName());

			if (player->GetSession()->GetSecurity() == SEC_EVENT_MASTER)
				sWorld->SendWorldText(LANG_EVENT_MASTER_ENTER_IN_GAME, player->GetSession()->GetPlayerName());

			if (player->GetSession()->GetSecurity() == SEC_GAME_MASTER)
				sWorld->SendWorldText(LANG_GAME_MASTER_ENTER_IN_GAME, player->GetSession()->GetPlayerName());

			if (player->GetSession()->GetSecurity() == SEC_ADMINISTRATOR)
				sWorld->SendWorldText(LANG_ADMINISTRATOR_ENTER_IN_GAME, player->GetSession()->GetPlayerName());

			if (player->GetSession()->GetSecurity() == SEC_MAIN_ADMINISTRATOR)
				sWorld->SendWorldText(LANG_MAIN_ADMINISTRATOR_ENTER_IN_GAME, player->GetSession()->GetPlayerName());

			if (player->GetSession()->GetSecurity() == SEC_DEVELOPER)
				sWorld->SendWorldText(LANG_DEVELOPER_ENTER_IN_GAME, player->GetSession()->GetPlayerName());

			if (player->GetSession()->GetSecurity() == SEC_TECHNICAL_MASTER)
				sWorld->SendWorldText(LANG_TECH_SUPPORT_ENTER_IN_GAME, player->GetSession()->GetPlayerName());
		}
	}

	void OnLogout(Player* player)
	{
		if (sWorld->getBoolConfig(CONFIG_GM_LEAVE_IN_GAME))
		{
			if (player->GetSession()->GetSecurity() == SEC_MODERATOR)
				sWorld->SendWorldText(LANG_MODERATOR_LEAVE_IN_GAME, player->GetSession()->GetPlayerName());

			if (player->GetSession()->GetSecurity() == SEC_EVENT_MASTER)
				sWorld->SendWorldText(LANG_EVENT_MASTER_LEAVE_IN_GAME, player->GetSession()->GetPlayerName());

			if (player->GetSession()->GetSecurity() == SEC_GAME_MASTER)
				sWorld->SendWorldText(LANG_GAME_MASTER_LEAVE_IN_GAME, player->GetSession()->GetPlayerName());

			if (player->GetSession()->GetSecurity() == SEC_ADMINISTRATOR)
				sWorld->SendWorldText(LANG_ADMINISTRATOR_LEAVE_IN_GAME, player->GetSession()->GetPlayerName());

			if (player->GetSession()->GetSecurity() == SEC_MAIN_ADMINISTRATOR)
				sWorld->SendWorldText(LANG_MAIN_ADMINISTRATOR_LEAVE_IN_GAME, player->GetSession()->GetPlayerName());

			if (player->GetSession()->GetSecurity() == SEC_DEVELOPER)
				sWorld->SendWorldText(LANG_DEVELOPER_LEAVE_IN_GAME, player->GetSession()->GetPlayerName());

			if (player->GetSession()->GetSecurity() == SEC_TECHNICAL_MASTER)
				sWorld->SendWorldText(LANG_TECH_SUPPORT_LEAVE_IN_GAME, player->GetSession()->GetPlayerName());
		}
	}
};

void AddSC_world_game_master_monitoring_system()
{ 
	new game_master_monitoring_system();
}