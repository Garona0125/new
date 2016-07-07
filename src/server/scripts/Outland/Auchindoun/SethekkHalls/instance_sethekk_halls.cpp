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
#include "sethekk_halls.h"

class instance_sethekk_halls : public InstanceMapScript
{
public:
    instance_sethekk_halls() : InstanceMapScript("instance_sethekk_halls", 556) { }

    struct instance_sethekk_halls_InstanceMapScript : public InstanceScript
    {
        instance_sethekk_halls_InstanceMapScript(Map* map) : InstanceScript(map) {}

        uint32 AnzuEncounter;
        uint32 IkissEncounter;
        uint64 IkissDoorGUID;

        std::string str_data;

        void Initialize()
        {
            AnzuEncounter = NOT_STARTED;
            IkissEncounter = NOT_STARTED;
            IkissDoorGUID = 0;
        }

        void OnCreatureCreate(Creature* creature)
        {
            if (creature->GetEntry() == NPC_ANZU)
            {
                if (AnzuEncounter >= IN_PROGRESS)
                    creature->DisappearAndDie();
                else
                    AnzuEncounter = IN_PROGRESS;
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
             if (go->GetEntry() == GO_IKISS_DOOR)
                IkissDoorGUID = go->GetGUID();
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
                case DATA_IKISS_DOOR_EVENT:
                    if (data == DONE)
                        DoUseDoorOrButton(IkissDoorGUID, DAY*IN_MILLISECONDS);
                    if (IkissEncounter != DONE)
                        IkissEncounter = data;
                    break;
                case TYPE_ANZU_ENCOUNTER:
                    if (AnzuEncounter != DONE)
                        AnzuEncounter = data;
                    break;
            }

            if (data == DONE)
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "S H " << IkissEncounter << " " << AnzuEncounter;

                str_data = saveStream.str();

                SaveToDB();
                OUT_SAVE_INST_DATA_COMPLETE;
            }
        }

        std::string GetSaveData()
        {
            return str_data.c_str();
        }

        void Load(const char* in)
        {
            if (!in)
            {
                OUT_LOAD_INST_DATA_FAIL;
                return;
            }

            uint32 Temp_IkissEncounter;
            uint32 Temp_AnzuEncounter;
            char dataHead1, dataHead2;

            OUT_LOAD_INST_DATA(in);

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> Temp_IkissEncounter >> Temp_AnzuEncounter;

            if (dataHead1 == 'S' && dataHead2 == 'H')
            {
                IkissEncounter = Temp_IkissEncounter;
                AnzuEncounter = Temp_AnzuEncounter;
            }

            if (IkissEncounter == IN_PROGRESS)
                IkissEncounter = NOT_STARTED;

            if (AnzuEncounter == IN_PROGRESS)
                AnzuEncounter = NOT_STARTED;

            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_sethekk_halls_InstanceMapScript(map);
    }
};

void AddSC_instance_sethekk_halls()
{
    new instance_sethekk_halls();
}