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

enum ItemCreator
{
	ITEM_ENCHANTED_JADE        = 46894,
	ITEM_ENCHANTED_PURPLE_JADE = 49664,
	ITEM_ENCHANTED_ONYX        = 48527,
	ACHIEVEMENT_JADE_TIGER     = 3636,
};

class enchanted_jade_creator : public PlayerScript
{
public:
	enchanted_jade_creator() : PlayerScript("enchanted_jade_creator") {}

	uint64 PlayerGUID;

	void OnCreate(Player* player)
	{
		PlayerGUID = player->GetGUID();
	}

	void OnLogin(Player* player)
	{
		if (GetConfigSettings::GetBoolState("Enchanted.Jade.Tiger.Give.Away.Enable", true))
		{
			if (PlayerGUID == player->GetGUID())
			{
				if (AchievementEntry const *JadeTiger = sAchievementStore.LookupEntry(ACHIEVEMENT_JADE_TIGER))
				{
					player->AddItem(ITEM_ENCHANTED_JADE, 1);
					player->CompletedAchievement(JadeTiger);
					PlayerGUID = 0;
				}
			}
		}
	}
};

class purple_jade_creator : public PlayerScript
{
public:
	purple_jade_creator() : PlayerScript("purple_jade_creator") {}

	uint64 PlayerGUID;

	void OnCreate(Player* player)
	{
		PlayerGUID = player->GetGUID();
	}

	void OnLogin(Player* player)
	{

		if (GetConfigSettings::GetBoolState("Purple.Jade.Tiger.Give.Away.Enable", true))
		{
			if (PlayerGUID == player->GetGUID())
			{
				player->AddItem(ITEM_ENCHANTED_PURPLE_JADE, 1);
				PlayerGUID = 0;
			}
		}
	}
};

class onyx_panter_creator : public PlayerScript
{
public:
	onyx_panter_creator() : PlayerScript("onyx_panter_creator") {}

	uint64 PlayerGUID;

	void OnCreate(Player* player)
	{
		PlayerGUID = player->GetGUID();
	}

	void OnLogin(Player* player)
	{
		if (GetConfigSettings::GetBoolState("Onyx.Panther.Give.Away.Enable", true))
		{
			if (PlayerGUID == player->GetGUID())
			{
				player->AddItem(ITEM_ENCHANTED_ONYX, 1);
				PlayerGUID = 0;
			}
		}
	}
};

void AddSC_world_rewarding_system()
{
	new enchanted_jade_creator();
	new purple_jade_creator();
	new onyx_panter_creator();
}