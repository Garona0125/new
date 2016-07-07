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

#define SQL_HISTORY "INSERT INTO `account_history` (`AccountId`, `RealmId`, `SessionIP`, `HistoryType`, `CharacterGuid`, `CharacterName`) VALUES ('%d', '%u', '%s', '%u', '%u', '%s')"

enum CharacterHistoryType
{
    CHARACTER_HISTORY_UNKNOWN = 0,
    CHARACTER_HISTORY_LOGIN   = 1,
    CHARACTER_HISTORY_LOGOUT  = 2,
    CHARACTER_HISTORY_DELETE  = 3,
    CHARACTER_HISTORY_CREATE  = 4,
};

bool CharacterHistoryEnable = false;
bool CharacterHistoryLogin = false;
bool CharacterHistoryLogout = false;
bool CharacterHistoryCreate = false;
bool CharacterHistoryDelete = false;

void WriteToHistory(Player* player, uint8 historyType)
{
    WorldSession* session = player->GetSession();
    LoginDatabase.PExecute(SQL_HISTORY, session->GetAccountId(), realmID, session->GetRemoteAddress().c_str(), historyType, player->GetGUIDLow(), player->GetName());
}

class mod_CharacterHistory_WorldScript : public WorldScript
{
public:
	mod_CharacterHistory_WorldScript() : WorldScript("mod_CharacterHistory_WorldScript") { }

    void OnConfigLoad(bool /*reload*/)
    {
        CharacterHistoryEnable = GetConfigSettings::GetBoolState("CharacterHistory.Enable", false);

        if (!CharacterHistoryEnable)
            return;

        CharacterHistoryLogin = GetConfigSettings::GetBoolState("CharacterHistory.Login", false);
        CharacterHistoryLogout = GetConfigSettings::GetBoolState("CharacterHistory.Logout", false);
        CharacterHistoryCreate = GetConfigSettings::GetBoolState("CharacterHistory.Create", false);
        CharacterHistoryDelete = GetConfigSettings::GetBoolState("CharacterHistory.Delete", false);
    }
};

class mod_CharacterHistory_PlayerScript : public PlayerScript
{
public:
	mod_CharacterHistory_PlayerScript() : PlayerScript("mod_CharacterHistory_PlayerScript") { }

    void OnCreate(Player* player)
    {
        if (!CharacterHistoryEnable || !CharacterHistoryCreate)
            return;

        WriteToHistory(player, CHARACTER_HISTORY_CREATE);
    }

    void OnLogin(Player* player)
    {
        if (!CharacterHistoryEnable || !CharacterHistoryLogin)
            return;

        WriteToHistory(player, CHARACTER_HISTORY_LOGIN);
    }

    void OnLogout(Player* player)
    {
        if (!CharacterHistoryEnable || !CharacterHistoryLogout)
            return;

        WriteToHistory(player, CHARACTER_HISTORY_LOGOUT);
    }

    void OnDelete(uint64 guid, const char* name, WorldSession* session)
    {
		if (!CharacterHistoryEnable || !CharacterHistoryDelete)
			return;

		LoginDatabase.PExecute(SQL_HISTORY, session->GetAccountId(), realmID, session->GetRemoteAddress().c_str(), CHARACTER_HISTORY_DELETE, GUID_LOPART(guid), name);
    }
};

void AddSC_mod_account_history()
{
    new mod_CharacterHistory_WorldScript();
    new mod_CharacterHistory_PlayerScript();
}