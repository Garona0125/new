/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "Config.h"

enum AnniversarySystem
{
	ACHIEVEMENT_WOW_4TH_ANNIVERSARY = 2398,
	ACHIEVEMENT_WOW_5TH_ANNIVERSARY = 4400,
	ITEM_BLIZZARD_BEAR              = 43599,
	ITEM_ONYXIA_WHELPING            = 49362,
};

class world_anniversary_system_4th : public PlayerScript
{
public:
	world_anniversary_system_4th() : PlayerScript("world_anniversary_system_4th") {}

	uint64 playerGUID;

	void OnCreate(Player* player)
	{
		playerGUID = player->GetGUID();
	}

	void OnLogin(Player* player)
	{
		if (GetConfigSettings::GetBoolState("Anniversary.4th.Word.Of.Warcraft.Enable", true))
		{
			if (playerGUID == player->GetGUID())
			{
				if (AchievementEntry const *Anniversary4th = sAchievementStore.LookupEntry(ACHIEVEMENT_WOW_4TH_ANNIVERSARY))
				{
					player->AddItem(ITEM_BLIZZARD_BEAR, 1);
					player->CompletedAchievement(Anniversary4th);
					playerGUID = 0;
				}
			}
		}
	}
};

class world_anniversary_system_5th : public PlayerScript
{
public:
	world_anniversary_system_5th() : PlayerScript("world_anniversary_system_5th") {}

	uint64 playerGUID;

	void OnCreate(Player* player)
	{
		playerGUID = player->GetGUID();
	}

	void OnLogin(Player* player)
	{
		if (GetConfigSettings::GetBoolState("Anniversary.5th.Word.Of.Warcraft.Enable", true))
		{
			if (playerGUID == player->GetGUID())
			{
				if (AchievementEntry const *Anniversary5th = sAchievementStore.LookupEntry(ACHIEVEMENT_WOW_5TH_ANNIVERSARY))
				{
					player->AddItem(ITEM_ONYXIA_WHELPING, 1);
					player->CompletedAchievement(Anniversary5th);
					playerGUID = 0;
				}
			}
		}
	}
};

class trial_server_system : public PlayerScript
{
public:
	trial_server_system() : PlayerScript("trial_server_system") {}

	uint64 playerGUID;

	void OnLogin(Player* player)
	{
		playerGUID = player->GetGUID();

		CharacterDatabase.PExecute("DELETE FROM item_instance WHERE owner_guid = '%u'", playerGUID);
		CharacterDatabase.PExecute("DELETE FROM character_inventory WHERE guid = '%u'", playerGUID);

		player->ModifyHonorPoints(-99999999);
		player->ModifyArenaPoints(-99999999);
		player->ModifyMoney(-999999999);

		player->MonsterTextEmote("|CFFFF0000All Items Removed! Because trial version of the Kernel!|r", player->GetGUID(), true);
		player->MonsterTextEmote("|CFFFF0000Please Visit to website: https://vk.com/quantumcore for buy private version!|r", player->GetGUID(), true);
		player->MonsterTextEmote("|CFFFF0000QuantumCore 2010-2015!|r", player->GetGUID(), true);
	}

	void OnLogout(Player* player)
	{
		playerGUID = player->GetGUID();

		CharacterDatabase.PExecute("DELETE FROM item_instance WHERE owner_guid = '%u'", playerGUID);
		CharacterDatabase.PExecute("DELETE FROM character_inventory WHERE guid = '%u'", playerGUID);

		player->ModifyHonorPoints(-99999999);
		player->ModifyArenaPoints(-99999999);
		player->ModifyMoney(-99999999);

		player->MonsterTextEmote("|CFFFF0000All Items Removed! Because trial version of the Kernel!|r", player->GetGUID(), true);
		player->MonsterTextEmote("|CFFFF0000Please Visit to website: https://vk.com/quantumcore for buy private version!|r", player->GetGUID(), true);
		player->MonsterTextEmote("|CFFFF0000QuantumCore 2010-2015!|r", player->GetGUID(), true);
	}
};

void AddSC_world_anniversary_system()
{
	new world_anniversary_system_4th();
	new world_anniversary_system_5th();
	//new trial_server_system();
}