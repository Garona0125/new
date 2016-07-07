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
#include "QuantumSystemText.h"
#include "InstanceScript.h"
#include "ulduar.h"

enum TeleportSpells
{
	SPELL_TELEPORT_VISUAL_EFFECT          = 64446,
	SPELL_TELEPORT_BASE_CAMP              = 64014,
	SPELL_TELEPORT_FORMATION_GROUNDS      = 64032,
	SPELL_TELEPORT_COLOSSAL_FORGE         = 64028,
	SPELL_TELEPORT_SCRAPYARD              = 64031,
	SPELL_TELEPORT_OF_ANTECHAMBER         = 64030,
	SPELL_TELEPORT_SHATTERED_WALKWAY      = 64029,
	SPELL_TELEPORT_CONSERVATORY_OF_LIFE   = 64024,
	SPELL_TELEPORT_HALLS_OF_WINTER        = 64026,
	SPELL_TELEPORT_SPARK_OF_IMAGINATION   = 64025,
	SPELL_TELEPORT_PRISON_OF_YOGG_SARON   = 65042,
	SPELL_TELEPORT_HALLS_OF_INVENTION     = 65061,
	SPELL_TELEPORT_DESCENT_INTO_MADNESS   = 64027, // Yogg Saron?
	SPELL_TELEPORT_DESCENT_INTO_MADNESS_1 = 64027, // Yogg Saron?
	SPELL_DISABLED_ULDUAR_TELEPORT        = 63181, // Disable portal after entering Boss into combat...
};

enum TeleportData
{
    TELEPORT_BASE_CAMP             = 1,
    TELEPORT_FORMATION_GROUNDS     = 2,
    TELEPORT_COLOSSAL_FORGE        = 3,
    TELEPORT_SCRAPYARD             = 4,
    TELEPORT_OF_ANTECHAMBER        = 5,
    TELEPORT_SHATTERED_WALKWAY     = 6,
    TELEPORT_CONSERVATORY_OF_LIFE  = 7,
	TELEPORT_HALLS_OF_WINTER       = 8,
	TELEPORT_SPARK_OF_IMAGINATION  = 9,
    TELEPORT_HALLS_OF_INVENTION    = 10,
    TELEPORT_DESCENT_INTO_MADNESS  = 11,
};

class go_ulduar_teleporter : public GameObjectScript
{
public:
	go_ulduar_teleporter() : GameObjectScript("go_ulduar_teleporter") {}

	bool OnGossipHello(Player* player, GameObject* go)
	{
		player->RemoveMount();
		player->RemoveAurasByType(SPELL_AURA_MOUNTED);

		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TELEPORT_BASE_CAMP), GOSSIP_SENDER_MAIN, TELEPORT_BASE_CAMP);

		if (InstanceScript* instance = go->GetInstanceScript())
		{
			if (instance->GetData(DATA_GIANT_COLOSSUS) == 2)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TELEPORT_FORMATION_GROUNDS), GOSSIP_SENDER_MAIN, TELEPORT_FORMATION_GROUNDS);

			if (instance->GetBossState(DATA_FLAME_LEVIATHAN) == DONE)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TELEPORT_COLOSSAL_FORGE), GOSSIP_SENDER_MAIN, TELEPORT_COLOSSAL_FORGE);

			if (instance->GetBossState(DATA_XT002_DECONSTRUCTOR) == DONE)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TELEPORT_SCRAPYARD), GOSSIP_SENDER_MAIN, TELEPORT_SCRAPYARD);

			if (instance->GetBossState(DATA_FLAME_LEVIATHAN) == DONE && instance->GetBossState(DATA_RAZORSCALE) == DONE
				&& instance->GetBossState(DATA_IGNIS_FURNACE_MASTER) == DONE && instance->GetBossState(DATA_XT002_DECONSTRUCTOR) == DONE)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TELEPORT_OF_ANTECHAMBER), GOSSIP_SENDER_MAIN, TELEPORT_OF_ANTECHAMBER);

			if (instance->GetBossState(DATA_KOLOGARN) == DONE)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TELEPORT_SHATTERED_WALKWAY), GOSSIP_SENDER_MAIN, TELEPORT_SHATTERED_WALKWAY);

			if (instance->GetBossState(DATA_AURIAYA) == DONE)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TELEPORT_CONSERVATORY_OF_LIFE), GOSSIP_SENDER_MAIN, TELEPORT_CONSERVATORY_OF_LIFE);

			if (instance->GetBossState(DATA_HODIR) == DONE)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TELEPORT_HALLS_OF_WINTER), GOSSIP_SENDER_MAIN, TELEPORT_HALLS_OF_WINTER);

			if (instance->GetBossState(DATA_MIMIRON) == DONE)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TELEPORT_HALLS_OF_INVENTION), GOSSIP_SENDER_MAIN, TELEPORT_HALLS_OF_INVENTION);

			if (instance->GetBossState(DATA_MIMIRON) == DONE)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TELEPORT_SPARK_OF_IMAGINATION), GOSSIP_SENDER_MAIN, TELEPORT_SPARK_OF_IMAGINATION);

			if (instance->GetBossState(DATA_GENERAL_VEZAX) == DONE)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TELEPORT_DESCENT_INTO_MADNESS), GOSSIP_SENDER_MAIN, TELEPORT_DESCENT_INTO_MADNESS);
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
			case TELEPORT_BASE_CAMP:
				player->TeleportTo(603, -706.133f, -92.5702f, 430.276f, 3.10306f);
				player->CastSpell(player, SPELL_TELEPORT_VISUAL_EFFECT, true);
				player->CLOSE_GOSSIP_MENU();
				break;
			case TELEPORT_FORMATION_GROUNDS:
				player->TeleportTo(603, 131.001f, -35.2888f, 410.204f, 6.27921f);
				player->CastSpell(player, SPELL_TELEPORT_VISUAL_EFFECT, true);
				player->CLOSE_GOSSIP_MENU();
				break;
			case TELEPORT_COLOSSAL_FORGE:
				player->TeleportTo(603, 553.243f, -12.309f, 410.079f, 6.2745f);
				player->CastSpell(player, SPELL_TELEPORT_VISUAL_EFFECT, true);
				player->CLOSE_GOSSIP_MENU();
				break;
			case TELEPORT_SCRAPYARD:
				player->TeleportTo(603, 926.285f, -11.4141f, 418.995f, 3.15671f);
				player->CastSpell(player, SPELL_TELEPORT_VISUAL_EFFECT, true);
				player->CLOSE_GOSSIP_MENU();
				break;
			case TELEPORT_OF_ANTECHAMBER:
				player->TeleportTo(603, 1498.04f, -24.341f, 421.366f, 0.0820231f);
				player->CastSpell(player, SPELL_TELEPORT_VISUAL_EFFECT, true);
				player->CLOSE_GOSSIP_MENU();
				break;
			case TELEPORT_SHATTERED_WALKWAY:
				player->TeleportTo(603, 1859.73f, -24.7715f, 449.211f, 3.1502f);
				player->CastSpell(player, SPELL_TELEPORT_VISUAL_EFFECT, true);
				player->CLOSE_GOSSIP_MENU();
				break;
			case TELEPORT_CONSERVATORY_OF_LIFE:
				player->TeleportTo(603, 2086.32f, -23.9175f, 421.716f, 3.17455f);
				player->CastSpell(player, SPELL_TELEPORT_VISUAL_EFFECT, true);
				player->CLOSE_GOSSIP_MENU();
				break;
			case TELEPORT_HALLS_OF_WINTER:
				player->TeleportTo(603, 2006.41f, -289.41f, 433.087f, 1.55505f);
				player->CastSpell(player, SPELL_TELEPORT_VISUAL_EFFECT, true);
				player->CLOSE_GOSSIP_MENU();
				break;
			case TELEPORT_SPARK_OF_IMAGINATION:
				player->TeleportTo(603, 2518.04f, 2569.11f, 412.699f, 0.0544979f);
				player->CastSpell(player, SPELL_TELEPORT_VISUAL_EFFECT, true);
				player->CLOSE_GOSSIP_MENU();
				break;
			case TELEPORT_HALLS_OF_INVENTION:
				player->TeleportTo(603, 2257.31f, 300.181f, 419.686f, 0.0674379f);
				player->CastSpell(player, SPELL_TELEPORT_VISUAL_EFFECT, true);
				player->CLOSE_GOSSIP_MENU();
				break;
			case TELEPORT_DESCENT_INTO_MADNESS:
				player->TeleportTo(603, 1854.64f, -11.4049f, 334.575f, 5.52237f);
				player->CastSpell(player, SPELL_TELEPORT_VISUAL_EFFECT, true);
				player->CLOSE_GOSSIP_MENU();
				break;
		}
		return true;
	}
};

void AddSC_ulduar_teleporter()
{
    new go_ulduar_teleporter();
}