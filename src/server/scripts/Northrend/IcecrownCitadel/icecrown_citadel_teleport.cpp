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
#include "Spell.h"
#include "QuantumSystemText.h"
#include "icecrown_citadel.h"

#define GOSSIP_SENDER_MAP 631

class icecrown_citadel_teleport : public GameObjectScript
{
    public:
        icecrown_citadel_teleport() : GameObjectScript("icecrown_citadel_teleport") { }

        bool OnGossipHello(Player* player, GameObject* go)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TELEPORT_LIGHTS_HAMMER), GOSSIP_SENDER_MAP, SPELL_LIGHTS_HAMMER_TELEPORT);
            if (InstanceScript* instance = go->GetInstanceScript())
            {
                if (instance->GetBossState(DATA_LORD_MARROWGAR) == DONE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TELEPORT_ORATORY_THE_DAMNED), GOSSIP_SENDER_MAP, SPELL_ORATORY_OF_THE_DAMNED_TELEPORT);
                if (instance->GetBossState(DATA_LADY_DEATHWHISPER) == DONE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TELEPORT_RAMPART_OF_SKULLS), GOSSIP_SENDER_MAP, SPELL_RAMPART_OF_SKULLS_TELEPORT);
                if (instance->GetBossState(DATA_GUNSHIP_EVENT) == DONE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TELEPORT_DEATHBRINGERS_RISE), GOSSIP_SENDER_MAP, SPELL_DEATHBRINGERS_RISE_TELEPORT);
                if (instance->GetData(DATA_COLDFLAME_JETS) == DONE)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TELEPORT_UPPER_SPIRE), GOSSIP_SENDER_MAP, SPELL_UPPER_SPIRE_TELEPORT);
                if (instance->GetBossState(DATA_SINDRAGOSA_GAUNTLET) == DONE)
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TELEPORT_SINDRAGOSAS_LAIR), GOSSIP_SENDER_MAP, SPELL_SINDRAGOSAS_LAIR_TELEPORT);
				if (instance->GetBossState(DATA_PROFESSOR_PUTRICIDE) == DONE && instance->GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) == DONE && instance->GetBossState(DATA_SINDRAGOSA) == DONE)
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TELEPORT_FROZEN_THRONE), GOSSIP_SENDER_MAP, SPELL_FROZEN_THRONE_TELEPORT);

            }

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(go), go->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, GameObject* /*go*/, uint32 sender, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            player->CLOSE_GOSSIP_MENU();

            SpellInfo const* spell = sSpellMgr->GetSpellInfo(action);
            if (!spell)
                return false;

            if (player->IsInCombatActive())
            {
                Spell::SendCastResult(player, spell, 0, SPELL_FAILED_AFFECTING_COMBAT);
                return true;
            }

            if (sender == GOSSIP_SENDER_MAP)
                player->CastSpell(player, spell, true);

            return true;
        }
};

class at_frozen_throne_teleport : public AreaTriggerScript
{
    public:
        at_frozen_throne_teleport() : AreaTriggerScript("at_frozen_throne_teleport") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/)
        {
            if (player->IsInCombatActive())
            {
                if (SpellInfo const* spell = sSpellMgr->GetSpellInfo(SPELL_FROZEN_THRONE_TELEPORT))
                    Spell::SendCastResult(player, spell, 0, SPELL_FAILED_AFFECTING_COMBAT);

                return true;
            }

            if (InstanceScript* instance = player->GetInstanceScript())
                if (instance->GetBossState(DATA_PROFESSOR_PUTRICIDE) == DONE &&
                    instance->GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) == DONE &&
                    instance->GetBossState(DATA_SINDRAGOSA) == DONE &&
                    instance->GetBossState(DATA_THE_LICH_KING) != IN_PROGRESS)
                    player->CastSpell(player, SPELL_FROZEN_THRONE_TELEPORT, true);

            return true;
        }
};

void AddSC_icecrown_citadel_teleport()
{
    new icecrown_citadel_teleport();
    new at_frozen_throne_teleport();
}