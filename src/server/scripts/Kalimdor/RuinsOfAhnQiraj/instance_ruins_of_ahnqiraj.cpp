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
#include "ruins_of_ahnqiraj.h"

class instance_ruins_of_ahnqiraj : public InstanceMapScript
{
    public:
        instance_ruins_of_ahnqiraj() : InstanceMapScript("instance_ruins_of_ahnqiraj", 509) {}

        struct instance_ruins_of_ahnqiraj_InstanceMapScript : public InstanceScript
        {
            instance_ruins_of_ahnqiraj_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(NUM_ENCOUNTER);

                KurinaxxGUID      = 0;
                GeneralRajaxxGUID = 0;
                MoamGUID          = 0;
                BuruGUID          = 0;
                AyamissGUID       = 0;
                OssirianGUID      = 0;
            }

			uint64 KurinaxxGUID;
            uint64 GeneralRajaxxGUID;
            uint64 MoamGUID;
            uint64 BuruGUID;
            uint64 AyamissGUID;
            uint64 OssirianGUID;

            void OnCreatureCreate(Creature* creature)
            {
                switch (creature->GetEntry())
                {
                    case NPC_KURINAXX:
                        KurinaxxGUID = creature->GetGUID();
                        break;
                    case NPC_GENERAL_RAJAXX:
                        GeneralRajaxxGUID = creature->GetGUID();
                        break;
                    case NPC_MOAM:
                        MoamGUID = creature->GetGUID();
                        break;
                    case NPC_BURU:
                        BuruGUID = creature->GetGUID();
                        break;
                    case NPC_AYAMISS:
                        AyamissGUID = creature->GetGUID();
                        break;
                    case NPC_OSSIRIAN:
                        OssirianGUID = creature->GetGUID();
                        break;
                }
            }

            bool SetBossState(uint32 bossId, EncounterState state)
            {
                if (!InstanceScript::SetBossState(bossId, state))
                    return false;

                return true;
            }

            uint64 GetData64(uint32 type)
            {
                switch (type)
                {
                    case DATA_KURINNAXX:
                        return KurinaxxGUID;
                    case DATA_GENERAL_RAJAXX:
                        return GeneralRajaxxGUID;
                    case DATA_MOAM:
                        return MoamGUID;
                    case DATA_BURU:
                        return BuruGUID;
                    case DATA_AYAMISS:
                        return AyamissGUID;
                    case DATA_OSSIRIAN:
                        return OssirianGUID;
                }

                return 0;
            }

            std::string GetSaveData()
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "R A" << GetBossSaveData();

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            void Load(char const* data)
            {
                if (!data)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(data);

                char dataHead1, dataHead2;

                std::istringstream loadStream(data);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'R' && dataHead2 == 'A')
                {
                    for (uint8 i = 0; i < NUM_ENCOUNTER; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;

                        if (tmpState == IN_PROGRESS || tmpState > TO_BE_DECIDED)
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
            return new instance_ruins_of_ahnqiraj_InstanceMapScript(map);
        }
};

void AddSC_instance_ruins_of_ahnqiraj()
{
    new instance_ruins_of_ahnqiraj();
}