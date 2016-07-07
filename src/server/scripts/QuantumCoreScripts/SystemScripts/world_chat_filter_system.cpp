/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "DatabaseEnv.h"

enum ChatFilterAuras
{
    SPELL_AURA_FREEZE = 9454,
    SPELL_AURA_STUN   = 31539,
};

class chat_filter_player_script : public PlayerScript
{
public:
	chat_filter_player_script() : PlayerScript("chat_filter_player_script") { }

	void OnLogin(Player* player)
	{
		if (player->HasAura(SPELL_AURA_FREEZE))
		{
			if (player->LoggedOutWhilePunished())
				player->GetSession()->SendNotification("Your freeze timer was set to the number before you logged out during your freeze.");

			if (QueryResult result = CharacterDatabase.PQuery("SELECT FreezeTimer FROM characters WHERE guid = %u", player->GetGUIDLow()))
			{
				player->SetFreezeStunTimer(true, (*result)[0].GetUInt32());
				CharacterDatabase.PExecute("UPDATE characters SET FreezeTimer = 0 WHERE guid = %u", player->GetGUIDLow());
			}
		}

		if (player->HasAura(SPELL_AURA_STUN))
		{
			if (player->LoggedOutWhilePunished())
				player->GetSession()->SendNotification("Your stun timer was set to the number before you logged out during your stun.");

			if (QueryResult result = CharacterDatabase.PQuery("SELECT StunTimer FROM characters WHERE guid = %u", player->GetGUIDLow()))
			{
				player->SetFreezeStunTimer(false, (*result)[0].GetUInt32());
				CharacterDatabase.PExecute("UPDATE characters SET StunTimer = 0 WHERE guid = %u", player->GetGUIDLow());
			}
		}

		player->SetLoggedOutWhilePunished(false);
	}

	void OnLogout(Player* player)
	{
		if (player->HasAura(SPELL_AURA_FREEZE))
		{
			player->SetLoggedOutWhilePunished(true);
			CharacterDatabase.PExecute("UPDATE characters SET FreezeTimer = %u WHERE guid = %u", player->GetFreezeStunTimer(true), player->GetGUIDLow());
		}

		if (player->HasAura(SPELL_AURA_STUN))
		{
			player->SetLoggedOutWhilePunished(true);
			CharacterDatabase.PExecute("UPDATE characters SET StunTimer = %u WHERE guid = %u", player->GetFreezeStunTimer(false), player->GetGUIDLow());
		}
	}
};

void AddSC_world_chat_filter_system()
{
	new chat_filter_player_script();
}