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
#include "shadow_labyrinth.h"

class instance_shadow_labyrinth : public InstanceMapScript
{
public:
    instance_shadow_labyrinth() : InstanceMapScript("instance_shadow_labyrinth", 555) { }

    struct instance_shadow_labyrinth_InstanceMapScript : public InstanceScript
    {
        instance_shadow_labyrinth_InstanceMapScript(Map* map) : InstanceScript(map) {}

        uint32 Encounter[MAX_ENCOUNTER];
        std::string str_data;

		uint64 GrandmasterVorpilGUID;

        uint64 RefectoryDoorGUID;
        uint64 ScreamingHallDoorGUID;

        void Initialize()
        {
            memset(&Encounter, 0, sizeof(Encounter));

			GrandmasterVorpilGUID = 0;

            RefectoryDoorGUID     = 0;
            ScreamingHallDoorGUID = 0;
        }

        bool IsEncounterInProgress() const
        {
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
			{
                if (Encounter[i] == IN_PROGRESS)
					return true;
			}

            return false;
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case GO_REFECTORY_DOOR:
                    RefectoryDoorGUID = go->GetGUID();
                    if (Encounter[2] == DONE)
                        go->SetGoState(GO_STATE_ACTIVE);
                    break;
                case GO_SCREAMING_HALL_DOOR:
                    ScreamingHallDoorGUID = go->GetGUID();
                    if (Encounter[3] == DONE)
                        go->SetGoState(GO_STATE_ACTIVE);
                    break;
            }
        }

        void OnCreatureCreate(Creature* creature)
        {
            switch (creature->GetEntry())
            {
                case NPC_GRAND_MASTER_VORPIL:
                    GrandmasterVorpilGUID = creature->GetGUID();
                    break;
            }
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
                case DATA_HELLMAW:
                    Encounter[0] = data;
                    break;
                case DATA_BLACK_HEART_THE_INCITER:
					Encounter[1] = data;
					if (data == NOT_STARTED)
					{
						HandleGameObject(RefectoryDoorGUID, false);
					}
					else if (data == IN_PROGRESS)
					{
						HandleGameObject(RefectoryDoorGUID, false);
					}
					else if (data == DONE)
					{
						HandleGameObject(RefectoryDoorGUID, true);
					}
                    break;
                case DATA_GRAND_MASTER_VORPIL:
					Encounter[2] = data;
					if (data == NOT_STARTED)
					{
						HandleGameObject(ScreamingHallDoorGUID, false);
					}
					else if (data == IN_PROGRESS)
					{
						HandleGameObject(ScreamingHallDoorGUID, false);
					}
					else if (data == DONE)
					{
						HandleGameObject(ScreamingHallDoorGUID, true);
					}
                    break;
                case DATA_MURMUR:
                    Encounter[3] = data;
                    break;
            }

            if (data == DONE)
            {
                OUT_SAVE_INST_DATA;
				
				std::ostringstream saveStream;
				saveStream << "S L " << Encounter[0] << " " << Encounter[1] << " " << Encounter[2] << " " << Encounter[3] << " " << Encounter[4];

                str_data = saveStream.str();

                SaveToDB();
                OUT_SAVE_INST_DATA_COMPLETE;
            }
        }

        uint32 GetData(uint32 type)
        {
            switch (type)
            {
                case DATA_HELLMAW:
					return Encounter[0];
				case DATA_BLACK_HEART_THE_INCITER:
					return Encounter[1];
                case DATA_GRAND_MASTER_VORPIL:
					return Encounter[2];
                case DATA_MURMUR:
					return Encounter[3];
            }
            return false;
        }

        uint64 GetData64(uint32 identifier)
        {
            if (identifier == DATA_GRAND_MASTER_VORPIL)
                return GrandmasterVorpilGUID;

            return 0;
        }

        std::string GetSaveData()
        {
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
			uint32 temp_auiEncounter[MAX_ENCOUNTER];
			memset(&temp_auiEncounter, 0, sizeof(temp_auiEncounter));
			
			std::istringstream loadStream(in);
			loadStream >> dataHead1 >> dataHead2 >> temp_auiEncounter[0] >> temp_auiEncounter[1] >> temp_auiEncounter[2] >> temp_auiEncounter[3] >> temp_auiEncounter[4];
			
			if (dataHead1 == 'S' && dataHead2 == 'L')
			{
				for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
					Encounter[i] = temp_auiEncounter[i];
			}
			else
			{
				OUT_LOAD_INST_DATA_FAIL;
				return;
			}
			
			for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
				
				if (Encounter[i] == IN_PROGRESS)
					Encounter[i] = NOT_STARTED;
			
			OUT_LOAD_INST_DATA_COMPLETE;
		}
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_shadow_labyrinth_InstanceMapScript(map);
    }
};

void AddSC_instance_shadow_labyrinth()
{
    new instance_shadow_labyrinth();
}