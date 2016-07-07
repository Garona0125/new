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

#define SQL_INFO "SELECT `model_id`, `size` FROM `character_model` WHERE `guid` = '%u'"

bool CharacterModelEnable = false;

class mod_character_model_WorldScript : public WorldScript
{
public:
	mod_character_model_WorldScript() : WorldScript("mod_character_model_WorldScript") { }

    void OnConfigLoad(bool /*reload*/)
    {
		CharacterModelEnable = GetConfigSettings::GetBoolState("Character.Model.Enable", false);
    }
};

class mod_character_model_PlayerScript : public PlayerScript
{
public:
	mod_character_model_PlayerScript() : PlayerScript("mod_character_model_PlayerScript") { }

    void OnLogin(Player* player)
    {
		if (!CharacterModelEnable)
			return;

        QueryResult result = CharacterDatabase.PQuery(SQL_INFO, player->GetGUIDLow());
		if (!result)
            return;

		player->SetNativeDisplayId((*result)[0].GetUInt32());
		player->SetObjectScale((*result)[1].GetFloat());
		player->SetDisplayId(player->GetNativeDisplayId());
    }
};

void AddSC_mod_character_model()
{
    new mod_character_model_WorldScript();
    new mod_character_model_PlayerScript();
}