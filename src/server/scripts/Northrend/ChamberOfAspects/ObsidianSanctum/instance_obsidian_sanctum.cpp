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
#include "obsidian_sanctum.h"

class instance_obsidian_sanctum : public InstanceMapScript
{
public:
    instance_obsidian_sanctum() : InstanceMapScript("instance_obsidian_sanctum", 615) { }

    struct instance_obsidian_sanctum_InstanceMapScript : public InstanceScript
    {
        instance_obsidian_sanctum_InstanceMapScript(Map* map) : InstanceScript(map) {}

        uint32 Encounter[MAX_ENCOUNTER];
        uint64 SartharionGUID;
        uint64 TenebronGUID;
        uint64 ShadronGUID;
        uint64 VesperonGUID;

        std::string str_data;

        void Initialize()
        {
            memset(&Encounter, 0, sizeof(Encounter));

            SartharionGUID = 0;
            TenebronGUID   = 0;
            ShadronGUID    = 0;
            VesperonGUID   = 0;
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;

            saveStream << "O B" << Encounter[0]
                << " " << Encounter[1]
                << " " << Encounter[2]
                << " " << Encounter[3];

            str_data = saveStream.str();

            OUT_SAVE_INST_DATA_COMPLETE;
            return str_data;
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
            uint16 data0, data1, data2, data3;

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3;

            if (dataHead1 == 'O' && dataHead2 == 'B')
            {
                Encounter[0] = data0;
                Encounter[1] = data1;
                Encounter[2] = data2;
                Encounter[3] = data3;

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                {
                    if (Encounter[i] == IN_PROGRESS)
                        Encounter[i] = NOT_STARTED;
                }
            }
			else OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }

        bool IsEncounterInProgress() const
        {
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                if (Encounter[i] == IN_PROGRESS)
                    return true;

            return false;
        }

        void OnCreatureCreate(Creature* creature)
        {
            switch (creature->GetEntry())
            {
                case NPC_SARTHARION:
                    SartharionGUID = creature->GetGUID();
                    break;
                case NPC_TENEBRON:
                    TenebronGUID = creature->GetGUID();
                    creature->SetActive(true);
                    break;
                case NPC_SHADRON:
                    ShadronGUID = creature->GetGUID();
                    creature->SetActive(true);
                    break;
                case NPC_VESPERON:
                    VesperonGUID = creature->GetGUID();
                    creature->SetActive(true);
                    break;
            }
        }

        void SetData(uint32 type, uint32 data)
        {
            if (type == TYPE_SARTHARION_EVENT)
                Encounter[0] = data;
            else if (type == TYPE_TENEBRON_PREKILLED)
                Encounter[1] = data;
            else if (type == TYPE_SHADRON_PREKILLED)
                Encounter[2] = data;
            else if (type == TYPE_VESPERON_PREKILLED)
                Encounter[3] = data;

            if (data == DONE)
                SaveToDB();
        }

        uint32 GetData(uint32 type)
        {
            if (type == TYPE_SARTHARION_EVENT)
                return Encounter[0];
            else if (type == TYPE_TENEBRON_PREKILLED)
                return Encounter[1];
            else if (type == TYPE_SHADRON_PREKILLED)
                return Encounter[2];
            else if (type == TYPE_VESPERON_PREKILLED)
                return Encounter[3];

            return 0;
        }

        uint64 GetData64(uint32 data)
        {
            switch (data)
            {
                case DATA_SARTHARION:
                    return SartharionGUID;
                case DATA_TENEBRON:
                    return TenebronGUID;
                case DATA_SHADRON:
                    return ShadronGUID;
                case DATA_VESPERON:
                    return VesperonGUID;
            }
            return 0;
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_obsidian_sanctum_InstanceMapScript(map);
    }
};

void AddSC_instance_obsidian_sanctum()
{
    new instance_obsidian_sanctum();
}