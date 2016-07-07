/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "OutdoorPvPMgr.h"
#include "OutdoorPvPWG.h"
#include "vault_of_archavon.h"

class instance_vault_of_archavon : public InstanceMapScript
{
    public:
        instance_vault_of_archavon() : InstanceMapScript("instance_vault_of_archavon", 624) { }

        struct instance_vault_of_archavon_InstanceMapScript : public InstanceScript
        {
            instance_vault_of_archavon_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(MAX_ENCOUNTER);
            }

			uint64 KoralonGUID;
			uint64 EmalonGUID;
            uint64 ToravonGUID;
			uint64 ArchavonGUID;

            time_t ArchavonDeath;
            time_t EmalonDeath;
            time_t KoralonDeath;

            void Initialize()
            {
				KoralonGUID = 0;
                EmalonGUID = 0;
                ToravonGUID = 0;
				ArchavonGUID = 0;

                ArchavonDeath = 0;
                EmalonDeath = 0;
                KoralonDeath = 0;
            }

			void OnPlayerEnter(Player* player)
			{
				if (sWorld->getBoolConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_ENABLED))
				{
					OutdoorPvPWG* pvpWG = (OutdoorPvPWG*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(4197);

					if (pvpWG && !player->IsGameMaster() && player->GetTeamId() != pvpWG->GetDefenderTeam())
						player->CastSpell(player, SPELL_TELEPORT_FORTRESS, true);
				}
			}

            void OnCreatureCreate(Creature* creature)
            {
                switch (creature->GetEntry())
                {
				    case NPC_KORALON:
						KoralonGUID = creature->GetGUID();
						break;
                    case NPC_EMALON:
                        EmalonGUID = creature->GetGUID();
                        break;
                    case NPC_TORAVON:
                        ToravonGUID = creature->GetGUID();
                        break;
					case NPC_ARCHAVON:
						ArchavonGUID = creature->GetGUID();
						break;
                    default:
                        break;
                }
            }

            uint64 GetData64(uint32 identifier)
            {
                switch (identifier)
                {
				    case DATA_KORALON:
						return KoralonGUID;
                    case DATA_EMALON:
                        return EmalonGUID;
                    case DATA_TORAVON:
                        return ToravonGUID;
					case DATA_ARCHAVON:
						return ArchavonGUID;
                    default:
                        break;
                }
                return 0;
            }

            bool SetBossState(uint32 type, EncounterState state)
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                if (state != DONE)
                   return true;

                switch (type)
                {
                    case DATA_ARCHAVON:
                        ArchavonDeath = time(NULL);
                        break;
                    case DATA_EMALON:
                        EmalonDeath = time(NULL);
                        break;
                    case DATA_KORALON:
                        KoralonDeath = time(NULL);
                        break;
                    default:
                        return true;
                }

                // on every death of Archavon, Emalon and Koralon check our achievement
                DoCastSpellOnPlayers(SPELL_EARTH_WIND_FIRE_ACHIEVEMENT_CHECK);
                return true;
			}

            bool CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* /*source*/, Unit const* /*target*/, uint32 /*miscvalue1*/)
            {
                switch (criteria_id)
                {
                    case CRITERIA_EARTH_WIND_FIRE_10:
                    case CRITERIA_EARTH_WIND_FIRE_25:
                        if (ArchavonDeath && EmalonDeath && KoralonDeath)
                        {
                            // instance difficulty check is already done in db (achievement_criteria_data)
                            // int() for Visual Studio, compile errors with abs(time_t)
                            return (abs(int(ArchavonDeath-EmalonDeath)) < MINUTE && \
                                abs(int(EmalonDeath-KoralonDeath)) < MINUTE && \
                                abs(int(KoralonDeath-ArchavonDeath)) < MINUTE);
                        }
                        break;
                    default:
                        break;
                }
                return false;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_vault_of_archavon_InstanceMapScript(map);
        }
};

void AddSC_instance_vault_of_archavon()
{
    new instance_vault_of_archavon();
}