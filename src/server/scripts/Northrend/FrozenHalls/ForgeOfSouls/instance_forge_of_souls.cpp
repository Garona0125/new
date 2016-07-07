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
#include "forge_of_souls.h"

class instance_forge_of_souls : public InstanceMapScript
{
    public:
        instance_forge_of_souls() : InstanceMapScript(FoSScriptName, 632) { }

        struct instance_forge_of_souls_InstanceScript : public InstanceScript
        {
            instance_forge_of_souls_InstanceScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(MAX_ENCOUNTER);
                BronjahmGUID = 0;
                DevourerOfSoulsGUID = 0;
                TeamInInstance = 0;
            }

			uint64 BronjahmGUID;
            uint64 DevourerOfSoulsGUID;
            uint32 TeamInInstance;

            void OnCreatureCreate(Creature* creature)
            {
                Map::PlayerList const &players = instance->GetPlayers();
                if (!players.isEmpty())
                    if (Player* player = players.begin()->getSource())
                        TeamInInstance = player->GetTeam();

                switch (creature->GetEntry())
                {
                    case NPC_BRONJAHM:
                        BronjahmGUID = creature->GetGUID();
                        break;
                    case NPC_DEVOURER_OF_SOULS:
                        DevourerOfSoulsGUID = creature->GetGUID();
                        break;
                    case NPC_SYLVANAS_PART1:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_JAINA_PART1, ALLIANCE);
                        break;
                    case NPC_LORALEN:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_ELANDRA, ALLIANCE);
                        break;
                    case NPC_KALIRA:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_KORELN, ALLIANCE);
                        break;
                }
            }

            uint32 GetData(uint32 type)
            {
                switch (type)
                {
                    case DATA_TEAM_IN_INSTANCE:
                        return TeamInInstance;
                    default:
                        break;
                }
                return 0;
            }

            uint64 GetData64(uint32 type)
            {
                switch (type)
                {
                    case DATA_BRONJAHM:
                        return BronjahmGUID;
                    case DATA_DEVOURER:
                        return DevourerOfSoulsGUID;
                    default:
                        break;
                }
                return 0;
            }

            std::string GetSaveData()
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "F S " << GetBossSaveData();

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            void Load(const char* in)
            {
                if (!in)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(in);

                char dataHead1, dataHead2;

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'F' && dataHead2 == 'S')
                {
                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;
                        SetBossState(i, EncounterState(tmpState));
                    }
                }
				else OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_forge_of_souls_InstanceScript(map);
        }
};

void AddSC_instance_forge_of_souls()
{
    new instance_forge_of_souls();
}