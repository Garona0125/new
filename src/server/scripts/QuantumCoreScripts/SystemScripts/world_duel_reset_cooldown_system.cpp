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

enum ZoneId
{
	ZONE_ELWYN_FOREST = 12,
	ZONE_DUROTAR      = 14,
};

class duel_reset_cooldown : public PlayerScript
{
public:
    duel_reset_cooldown() : PlayerScript("duel_reset_cooldown") {}

    void OnDuelEnd(Player* winner, Player* looser, DuelCompleteType type)
    {
        if (sWorld->getBoolConfig(CONFIG_DUEL_RESET_COOLDOWN_ON_FINISH))
        {
            if (sWorld->getBoolConfig(CONFIG_DUEL_RESET_COOLDOWN_ONLY_IN_ELWYNN_AND_DUROTAR))
            {
                if (winner->GetZoneId() == ZONE_ELWYN_FOREST || winner->GetZoneId() == ZONE_DUROTAR)
                {
                    if (type == DUEL_WON)
                    {
                        winner->RemoveArenaSpellCooldowns();
                        looser->RemoveArenaSpellCooldowns();
                        winner->SetHealth(winner->GetMaxHealth());
                        looser->SetHealth(looser->GetMaxHealth());

                        if (winner->GetPowerType() == POWER_MANA)
                            winner->SetPower(POWER_MANA, winner->GetMaxPower(POWER_MANA));

                        if (looser->GetPowerType() == POWER_MANA)
							looser->SetPower(POWER_MANA, looser->GetMaxPower(POWER_MANA));
                    }
                    winner->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
                }
            }
            else
				if (type == DUEL_WON)
                {
                    winner->RemoveArenaSpellCooldowns();
                    looser->RemoveArenaSpellCooldowns();
                    winner->SetHealth(winner->GetMaxHealth());
                    looser->SetHealth(looser->GetMaxHealth());

					if (winner->GetPowerType() == POWER_MANA)
                        winner->SetPower(POWER_MANA, winner->GetMaxPower(POWER_MANA));

                    if (looser->GetPowerType() == POWER_MANA)
                        looser->SetPower(POWER_MANA, looser->GetMaxPower(POWER_MANA));
                }
                winner->HandleEmoteCommand(EMOTE_ONESHOT_CHEER);
        }
    }

    void OnDuelStart(Player* player1, Player* player2)
    {
        if (sWorld->getBoolConfig(CONFIG_DUEL_RESET_COOLDOWN_ON_START))
        {
            if (sWorld->getBoolConfig(CONFIG_DUEL_RESET_COOLDOWN_ONLY_IN_ELWYNN_AND_DUROTAR))
            {
                if (player1->GetZoneId() == ZONE_ELWYN_FOREST || player1->GetZoneId() == ZONE_DUROTAR)
                {
                    player1->RemoveArenaSpellCooldowns();
                    player2->RemoveArenaSpellCooldowns();
                    player1->SetHealth(player1->GetMaxHealth());
                    player2->SetHealth(player2->GetMaxHealth());

                    if (sWorld->getBoolConfig(CONFIG_DUEL_RESET_COOLDOWN_RESET_ENERGY_ON_START))
                    {
                        switch (player1->GetPowerType())
                        {
                            case POWER_RAGE:
								player1->SetPower(POWER_RAGE, 0);
								break;
							case POWER_RUNIC_POWER:
								player1->SetPower(POWER_RUNIC_POWER, 0);
								break;
							default:
								break;
						}

                        switch (player2->GetPowerType())
                        {
                            case POWER_RAGE:
								player2->SetPower(POWER_RAGE, 0);
								break;
							case POWER_RUNIC_POWER:
								player2->SetPower(POWER_RUNIC_POWER, 0);
								break;
							default:
								break;
                        }
                    }
                    if (sWorld->getBoolConfig(CONFIG_DUEL_RESET_COOLDOWN_MAX_ENERGY_ON_START))
                    {
                        switch (player1->GetPowerType())
                        {
                            case POWER_MANA:
								player1->SetPower(POWER_MANA, player1->GetMaxPower(POWER_MANA));
								break;
							case POWER_RAGE:
								player1->SetPower(POWER_RAGE, player1->GetMaxPower(POWER_RAGE));
								break;
							case POWER_RUNIC_POWER:
								player1->SetPower(POWER_RUNIC_POWER, player1->GetMaxPower(POWER_RUNIC_POWER));
								break;
							default:
								break;
						}

                        switch (player2->GetPowerType())
                        {
                            case POWER_MANA:
								player2->SetPower(POWER_MANA, player2->GetMaxPower(POWER_MANA));
								break;
							case POWER_RAGE:
								player2->SetPower(POWER_RAGE, player2->GetMaxPower(POWER_RAGE));
								break;
							case POWER_RUNIC_POWER:
								player2->SetPower(POWER_RUNIC_POWER, player2->GetMaxPower(POWER_RUNIC_POWER));
								break;
							default:
								break;
                        }
                    }
                }
            }
            else
            {
                player1->RemoveArenaSpellCooldowns();
                player2->RemoveArenaSpellCooldowns();
                player1->SetHealth(player1->GetMaxHealth());
                player2->SetHealth(player2->GetMaxHealth());

                if (sWorld->getBoolConfig(CONFIG_DUEL_RESET_COOLDOWN_MAX_ENERGY_ON_START))
                {
                    switch (player1->GetPowerType())
                    {
                        case POWER_MANA:
							player1->SetPower(POWER_MANA, player1->GetMaxPower(POWER_MANA));
							break;
						case POWER_RAGE:
							player1->SetPower(POWER_RAGE, player1->GetMaxPower(POWER_RAGE));
							break;
						case POWER_RUNIC_POWER:
							player1->SetPower(POWER_RUNIC_POWER, player1->GetMaxPower(POWER_RUNIC_POWER));
							break;
						default:
							break;
					}

					switch (player2->GetPowerType())
					{
                        case POWER_MANA:
							player2->SetPower(POWER_MANA, player2->GetMaxPower(POWER_MANA));
							break;
						case POWER_RAGE:
							player2->SetPower(POWER_RAGE, player2->GetMaxPower(POWER_RAGE));
							break;
						case POWER_RUNIC_POWER:
							player2->SetPower(POWER_RUNIC_POWER, player2->GetMaxPower(POWER_RUNIC_POWER));
							break;
						default:
							break;
                    }
                }
            }
        }
    }
};

void AddSC_world_duel_reset_cooldown_system()
{
    new duel_reset_cooldown();
}