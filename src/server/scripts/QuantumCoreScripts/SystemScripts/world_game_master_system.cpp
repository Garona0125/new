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

enum InternalItems
{
	GAME_MASTER_HOOD     = 12064,
	GAME_MASTER_ROBE     = 2586,
	GAME_MASTER_SLIPPERS = 11508,
	GAME_MASTER_BAG      = 23162,
	GAME_MASTER_WEAPON   = 192,
	GAME_MASTER_RING     = 12947,
	GAME_MASTER_SWORD_1H = 18582,
};

enum InternalSpells
{
	SPELL_UBER_HEAL     = 1908,
	SPELL_SILENCED      = 1852,
	SPELL_FREEZE        = 9454,
	SPELL_MUR_MUR_TOUCH = 33331,
	SPELL_DIVINE_SHIELD = 40733,
	SPELL_FULL_HEAL     = 25840,
	SPELL_AUTOMATION    = 39258,
	SPELL_BANISH        = 35182,
	SPELL_SNACKLE       = 38505,
	SPELL_INVISIBILITY  = 23452,
	SPELL_MASS_DAMAGE   = 64487,
	SPELL_KILL_ALL      = 74090,
};

class game_master_start_system : public PlayerScript
{
public:
	game_master_start_system() : PlayerScript("game_master_start_system") {}

	uint64 PlayerGUID;
	
	void OnCreate(Player* player)
	{
		if (player && AccountMgr::IsModeratorAccount(player->GetSession()->GetSecurity()
			|| (player && AccountMgr::IsEventMasterAccount(player->GetSession()->GetSecurity()
			|| (player && AccountMgr::IsGMAccount(player->GetSession()->GetSecurity()
			|| (player && AccountMgr::IsAdminAccount(player->GetSession()->GetSecurity()
			|| (player && AccountMgr::IsMainAdministratorAccount(player->GetSession()->GetSecurity()
			|| (player && AccountMgr::IsDeveloperAccount(player->GetSession()->GetSecurity()
			|| (player && AccountMgr::IsTechSupportAccount(player->GetSession()->GetSecurity()))))))))))))))
		{
			PlayerGUID = player->GetGUID();
		}
	}

	void OnLogin(Player* player)
	{
		if (GetConfigSettings::GetBoolState("Gm.Start.Helper.Enable", true))
		{
			if (PlayerGUID == player->GetGUID())
			{
				if (player && AccountMgr::IsModeratorAccount(player->GetSession()->GetSecurity()
					|| (player && AccountMgr::IsEventMasterAccount(player->GetSession()->GetSecurity()
					|| (player && AccountMgr::IsGMAccount(player->GetSession()->GetSecurity()
					|| (player && AccountMgr::IsAdminAccount(player->GetSession()->GetSecurity()
					|| (player && AccountMgr::IsMainAdministratorAccount(player->GetSession()->GetSecurity()
					|| (player && AccountMgr::IsDeveloperAccount(player->GetSession()->GetSecurity()
					|| (player && AccountMgr::IsTechSupportAccount(player->GetSession()->GetSecurity()))))))))))))))
				{
					if (GetConfigSettings::GetBoolState("Gm.Start.Helper.Items.Enable", true))
					{
						player->AddItem(GAME_MASTER_BAG, 4);
						player->AddItem(GAME_MASTER_HOOD, 1);
						player->AddItem(GAME_MASTER_ROBE, 1);
						player->AddItem(GAME_MASTER_SLIPPERS, 1);
						//player->AddItem(GAME_MASTER_WEAPON, 1);
						//player->AddItem(GAME_MASTER_RING, 2);
						//player->AddItem(GAME_MASTER_SWORD_1H, 1);
						PlayerGUID = 0;
					}
				}
				if (player && AccountMgr::IsModeratorAccount(player->GetSession()->GetSecurity()
					|| (player && AccountMgr::IsEventMasterAccount(player->GetSession()->GetSecurity()
					|| (player && AccountMgr::IsGMAccount(player->GetSession()->GetSecurity()
					|| (player && AccountMgr::IsAdminAccount(player->GetSession()->GetSecurity()
					|| (player && AccountMgr::IsMainAdministratorAccount(player->GetSession()->GetSecurity()
					|| (player && AccountMgr::IsDeveloperAccount(player->GetSession()->GetSecurity()
					|| (player && AccountMgr::IsTechSupportAccount(player->GetSession()->GetSecurity()))))))))))))))
				{
					if (GetConfigSettings::GetBoolState("Gm.Start.Helper.Spells.Enable", true))
					{
						player->learnSpell(SPELL_UBER_HEAL, false);
						player->learnSpell(SPELL_SILENCED, false);
						player->learnSpell(SPELL_FREEZE, false);
						player->learnSpell(SPELL_MUR_MUR_TOUCH, false);
						player->learnSpell(SPELL_DIVINE_SHIELD, false);
						player->learnSpell(SPELL_FULL_HEAL, false);
						player->learnSpell(SPELL_AUTOMATION, false);
						player->learnSpell(SPELL_BANISH, false);
						player->learnSpell(SPELL_SNACKLE, false);
						player->learnSpell(SPELL_INVISIBILITY, false);
						player->learnSpell(SPELL_MASS_DAMAGE, false);
						//player->learnSpell(SPELL_KILL_ALL, false);
						PlayerGUID = 0;
					}
				}
			}
		}
	}
};

void AddSC_world_game_master_system()
{
	new game_master_start_system();
}