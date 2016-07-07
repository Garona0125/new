/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "QuantumGossip.h"
#include "InstanceScript.h"
#include "naxxramas.h"

enum TeleportSpells
{
	SPELL_TELEPORT_VISUAL_EFFECT = 64446,
};

enum TeleportData
{
    TELEPORT_PLAGUE_QUARTER    = 100,
    TELEPORT_ARACHNID_QUARTER  = 101,
    TELEPORT_CONSTRUCT_QUARTER = 102,
    TELEPORT_MILITARY_QUARTER  = 103,
};

class go_naxxramas_teleporter : public GameObjectScript
{
public:
	go_naxxramas_teleporter() : GameObjectScript("go_naxxramas_teleporter") {}

	bool OnGossipHello(Player* player, GameObject* go)
	{
		player->RemoveMount();
		player->RemoveAurasByType(SPELL_AURA_MOUNTED);

		if (InstanceScript* instance = go->GetInstanceScript())
		{
			if (instance->GetData(BOSS_NOTH) == DONE
				&& instance->GetData(BOSS_HEIGAN) == DONE
				&& instance->GetData(BOSS_LOATHEB) == DONE)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to The Plague Quarter", GOSSIP_SENDER_MAIN, TELEPORT_PLAGUE_QUARTER);

			if (instance->GetData(BOSS_ANUBREKHAN) == DONE
				&& instance->GetData(BOSS_FAERLINA) == DONE
				&& instance->GetData(BOSS_MAEXXNA) == DONE)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to The Arachnid Quarter", GOSSIP_SENDER_MAIN, TELEPORT_ARACHNID_QUARTER);

			if (instance->GetData(BOSS_PATCHWERK) == DONE
				&& instance->GetData(BOSS_GROBBULUS) == DONE
				&& instance->GetData(BOSS_GLUTH) == DONE
				&& instance->GetData(BOSS_THADDIUS) == DONE)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Construct Quarter", GOSSIP_SENDER_MAIN, TELEPORT_CONSTRUCT_QUARTER);

			if (instance->GetData(BOSS_RAZUVIOUS) == DONE
				&& instance->GetData(BOSS_GOTHIK) == DONE
				&& instance->GetData(BOSS_HORSEMEN) == DONE)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Military Quarter", GOSSIP_SENDER_MAIN, TELEPORT_MILITARY_QUARTER);
		}
		player->SEND_GOSSIP_MENU(go->GetGOInfo()->GetGossipMenuId(), go->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, GameObject* /*go*/, uint32 sender, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();

		if (sender != GOSSIP_SENDER_MAIN)
			return false;

		if (!player->getAttackers().empty())
			return false;

		switch (action)
		{
			case TELEPORT_PLAGUE_QUARTER:
				player->TeleportTo(533, 2976.32f, -3463.77f, 298.161f, 3.92654f);
				player->CastSpell(player, SPELL_TELEPORT_VISUAL_EFFECT, true);
				player->CLOSE_GOSSIP_MENU();
				break;
			case TELEPORT_ARACHNID_QUARTER:
				player->TeleportTo(533, 3036.01f, -3464.52f, 298.221f, 5.5154f);
				player->CastSpell(player, SPELL_TELEPORT_VISUAL_EFFECT, true);
				player->CLOSE_GOSSIP_MENU();
				break;
			case TELEPORT_CONSTRUCT_QUARTER:
				player->TeleportTo(533, 3036.97f, -3403.59f, 298.219f, 0.799428f);
				player->CastSpell(player, SPELL_TELEPORT_VISUAL_EFFECT, true);
				player->CLOSE_GOSSIP_MENU();
				break;
			case TELEPORT_MILITARY_QUARTER:
				player->TeleportTo(533, 2973.52f, -3400.92f, 298.184f, 2.29324f);
				player->CastSpell(player, SPELL_TELEPORT_VISUAL_EFFECT, true);
				player->CLOSE_GOSSIP_MENU();
				break;
		}

		return true;
	}
};

void AddSC_naxxramas_teleporter()
{
    new go_naxxramas_teleporter();
}