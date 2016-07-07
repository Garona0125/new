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

enum Totems
{
	ITEM_ID_EARTH_TOTEM  = 5175,
	ITEM_ID_FIRE_TOTEM   = 5176,
	ITEM_ID_WATER_TOTEM  = 5177,
	ITEM_ID_AIR_TOTEM    = 5178,
	ITEM_ID_EARTEN_RELIC = 46978,
};

enum QuestSpells
{
	//Paladin
	SPELL_REDEMPTION           = 7329,
	//Hunter
	SPELL_FEED_AND_REVIVE_PET  = 5300,
	SPELL_TAME_CALL_DIS_PET    = 1579,
	// Druid
	SPELL_BEAR_FORM            = 19179,
	// Warrior
	SPELL_DEFENSIVE_STANCE     = 8121,
	SPELL_BERSERKER_STANCE     = 8616,
	// Warlock
	SPELL_SUMMON_SUCCUBUS      = 11519,
	SPELL_SUMMON_IMP           = 7763,
	SPELL_SUMMON_VOIDWALKER    = 11520,
	SPELL_SUMMON_FELLHUNTER    = 1373,
	// Shaman
	SPELL_STONESKIN_TOTEM      = 8073,
	SPELL_SEARING_TOTEM        = 2075,
	SPELL_HEALING_STREAM_TOTEM = 5396,
	SPELL_CREATE_EARTEN_RELIC  = 66747,
};

class world_starting_player_system : public PlayerScript
{
public:
	world_starting_player_system() : PlayerScript("world_starting_player_system") { }

	uint64 PlayerGUID;

	void OnCreate(Player* player)
	{
		PlayerGUID = player->GetGUID();
	}

	void OnLogin(Player* player)
	{
		if (GetConfigSettings::GetBoolState("Player.Start.Helper.Enable", true))
		{
			if (PlayerGUID == player->GetGUID())
			{
				if (player->GetSession()->GetSecurity() == SEC_PLAYER || SEC_MODERATOR || SEC_EVENT_MASTER || SEC_GAME_MASTER || SEC_ADMINISTRATOR || SEC_MAIN_ADMINISTRATOR || SEC_DEVELOPER || SEC_TECHNICAL_MASTER)
				{
					if (player->GetCurrentClass() == CLASS_PALADIN)
					{
						player->CastSpell(player, SPELL_REDEMPTION, true);
						PlayerGUID = 0;
					}

					if (player->GetCurrentClass() == CLASS_HUNTER)
					{
						player->CastSpell(player, SPELL_FEED_AND_REVIVE_PET, true);
						player->CastSpell(player, SPELL_TAME_CALL_DIS_PET, true);
						PlayerGUID = 0;
					}

					if (player->GetCurrentClass() == CLASS_DRUID)
					{
						player->CastSpell(player, SPELL_BEAR_FORM, true);
						PlayerGUID = 0;
					}

					if (player->GetCurrentClass() == CLASS_WARRIOR)
					{
						player->CastSpell(player, SPELL_DEFENSIVE_STANCE, true);
						player->CastSpell(player, SPELL_BERSERKER_STANCE, true);
						PlayerGUID = 0;
					}

					if (player->GetCurrentClass() == CLASS_WARLOCK)
					{
						player->CastSpell(player, SPELL_SUMMON_SUCCUBUS, true);
						player->CastSpell(player, SPELL_SUMMON_IMP, true);
						player->CastSpell(player, SPELL_SUMMON_VOIDWALKER, true);
						player->CastSpell(player, SPELL_SUMMON_FELLHUNTER, true);
						PlayerGUID = 0;
					}

					if (player->GetCurrentClass() == CLASS_SHAMAN)
					{
						// Totems
						//player->AddItem(ITEM_ID_EARTH_TOTEM, 1);
						//player->AddItem(ITEM_ID_FIRE_TOTEM, 1);
						//player->AddItem(ITEM_ID_WATER_TOTEM, 1);
						//player->AddItem(ITEM_ID_AIR_TOTEM, 1);
						player->AddItem(ITEM_ID_EARTEN_RELIC, 1);
						// Spells
						player->CastSpell(player, SPELL_STONESKIN_TOTEM, true);
						player->CastSpell(player, SPELL_SEARING_TOTEM, true);
						player->CastSpell(player, SPELL_HEALING_STREAM_TOTEM, true);
						PlayerGUID = 0;
					}
				}
			}
		}
	}
};

void AddSC_world_starting_player_system()
{
	new world_starting_player_system();
}