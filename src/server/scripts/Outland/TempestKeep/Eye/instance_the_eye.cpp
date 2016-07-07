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
#include "the_eye.h"

class instance_the_eye : public InstanceMapScript
{
    public:
        instance_the_eye() : InstanceMapScript("instance_the_eye", 550) { }

        struct instance_the_eye_InstanceMapScript : public InstanceScript
        {
            instance_the_eye_InstanceMapScript(Map* map) : InstanceScript(map) { }

			uint64 AlarGUID;
			uint64 HighAstromancerSolarianGUID;
			uint64 VoidReaverGUID;
			uint64 KaelthasSunstriderGUID;
            uint64 ThaladredTheDarkenerGUID;
            uint64 LordSanguinarGUID;
            uint64 GrandAstromancerCapernianGUID;
            uint64 MasterEngineerTelonicusGUID;

			uint64 KaelthasLeftDoorGUID;
			uint64 KaelthasRightDoorGUID;
			uint64 KaelthasEntranceRightDoorGUID;
			uint64 KaelthasEntranceLeftDoorGUID;
			uint64 KaelthasPreEntranceRightDoorGUID;
			uint64 KaelthasPreEntranceLeftDoorGUID;

            uint8 KaelthasEventPhase;
            uint8 AlarEventPhase;

            uint32 Encounter[MAX_ENCOUNTER];

            void Initialize()
            {
                memset(&Encounter, 0, sizeof(Encounter));

				AlarGUID                         = 0;
				HighAstromancerSolarianGUID      = 0;
				VoidReaverGUID                   = 0;
				KaelthasSunstriderGUID           = 0;
                ThaladredTheDarkenerGUID         = 0;
                LordSanguinarGUID                = 0;
                GrandAstromancerCapernianGUID    = 0;
                MasterEngineerTelonicusGUID      = 0;

				KaelthasLeftDoorGUID             = 0;
				KaelthasRightDoorGUID            = 0;
				KaelthasEntranceRightDoorGUID    = 0;
				KaelthasEntranceLeftDoorGUID     = 0;
				KaelthasPreEntranceRightDoorGUID = 0;
				KaelthasPreEntranceLeftDoorGUID  = 0;

                KaelthasEventPhase               = 0;
                AlarEventPhase                   = 0;
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

            void OnCreatureCreate(Creature* creature)
            {
                switch (creature->GetEntry())
                {
					case NPC_ALAR:
						AlarGUID = creature->GetGUID();
						break;
					case NPC_HIGH_ASTROMANCER_SOLARIAN:
						HighAstromancerSolarianGUID = creature->GetGUID();
						break;
					case NPC_VOID_REAVER:
						VoidReaverGUID = creature->GetGUID();
						break;
					case NPC_KAELTHAS_SUNSTRIDER:
						KaelthasSunstriderGUID = creature->GetGUID();
						break;
					case NPC_THALADRED_THE_DARKENER:
						ThaladredTheDarkenerGUID = creature->GetGUID();
						break;
					case NPC_LORD_SANGUINAR:
						LordSanguinarGUID = creature->GetGUID();
						break;
					case NPC_GRAND_ASTROMANCER_CAPERNIAN:
						GrandAstromancerCapernianGUID = creature->GetGUID();
						break;
					case NPC_MASTER_ENGINEER_TELONICUS:
						MasterEngineerTelonicusGUID = creature->GetGUID();
						break;
                }
            }

			void OnGameObjectCreate(GameObject* go)
			{
				switch (go->GetEntry())
				{
					case GO_KAETLTHAS_LEFT_DOOR:
						go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
						KaelthasLeftDoorGUID = go->GetGUID();
						break;
					case GO_KAETLTHAS_RIGHT_DOOR:
						go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
						KaelthasRightDoorGUID = go->GetGUID();
						break;
					case GO_KAETLTHAS_ENTRANCE_DOOR_RIGHT:
						go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
						KaelthasEntranceRightDoorGUID = go->GetGUID();
						break;
					case GO_KAETLTHAS_ENTRANCE_DOOR_LEFT:
						go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
						KaelthasEntranceLeftDoorGUID = go->GetGUID();
						break;
					case GO_KAELTHAS_PRE_ENTRANCE_DOOR_RIGHT:
						go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
						KaelthasPreEntranceRightDoorGUID = go->GetGUID();
						break;
					case GO_KAELTHAS_PRE_ENTRANCE_DOOR_LEFT:
						go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
						KaelthasPreEntranceLeftDoorGUID = go->GetGUID();
						break;
				}
			}

            uint64 GetData64(uint32 identifier)
            {
                switch (identifier)
                {
					case DATA_ALAR:
						return AlarGUID;
					case DATA_VOID_REAVER:
						return VoidReaverGUID;
					case DATA_HIGH_ASTROMANCER_SOLARIAN:
						return HighAstromancerSolarianGUID;
					case DATA_KAELTHAS_SUNSTRIDER:
						return KaelthasSunstriderGUID;
					case DATA_THALADRED_THE_DARKENER:
						return ThaladredTheDarkenerGUID;
					case DATA_LORD_SANGUINAR:
						return LordSanguinarGUID;
					case DATA_GRAND_ASTROMANCER_CAPERNIAN:
						return GrandAstromancerCapernianGUID;
					case DATA_MASTER_ENGINEER_TELONICUS:
						return MasterEngineerTelonicusGUID;
					case DATA_KAETLTHAS_LEFT_DOOR:
						return KaelthasLeftDoorGUID;
					case DATA_KAETLTHAS_RIGHT_DOOR:
						return KaelthasRightDoorGUID;
					case DATA_KAETLTHAS_ENTRANCE_DOOR_RIGHT:
						return KaelthasEntranceRightDoorGUID;
					case DATA_KAETLTHAS_ENTRANCE_DOOR_LEFT:
						return KaelthasEntranceLeftDoorGUID;
					case DATA_KAELTHAS_PRE_ENTRANCE_DOOR_RIGHT:
						return KaelthasPreEntranceRightDoorGUID;
					case DATA_KAELTHAS_PRE_ENTRANCE_DOOR_LEFT:
						return KaelthasPreEntranceLeftDoorGUID;
                }
                return 0;
            }

            void SetData(uint32 type, uint32 data)
            {
                switch (type)
                {
                    case DATA_ALAR:
						AlarEventPhase = data;
						Encounter[0] = data;
						break;
					case DATA_HIGH_ASTROMANCER_SOLARIAN:
						Encounter[1] = data;
						break;
					case DATA_VOID_REAVER:
						Encounter[2] = data;
						break;
					case DATA_KAELTHAS_SUNSTRIDER:
						KaelthasEventPhase = data;
						Encounter[3] = data;
						break;
				}

                if (data == DONE)
                    SaveToDB();
			}

            uint32 GetData(uint32 type)
            {
                switch (type)
                {
                    case DATA_ALAR:
						return AlarEventPhase;
					case DATA_HIGH_ASTROMANCER_SOLARIAN:
						return Encounter[1];
					case DATA_VOID_REAVER:
						return Encounter[2];
					case DATA_KAELTHAS_SUNSTRIDER:
						return KaelthasEventPhase;
				}
				return 0;
            }

            std::string GetSaveData()
            {
                OUT_SAVE_INST_DATA;
                std::ostringstream stream;
				stream << "T E " << Encounter[0] << ' ' << Encounter[1] << ' ' << Encounter[2] << ' ' << Encounter[3];

				OUT_SAVE_INST_DATA_COMPLETE;
				return stream.str();
            }

            void Load(const char* in)
            {
				if (!in)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(in);

				std::istringstream stream(in);

				char dataHead1, dataHead2;
				uint32 temp_auiEncounter[MAX_ENCOUNTER];
				memset(&temp_auiEncounter, 0, sizeof(temp_auiEncounter));

				stream >> dataHead1 >> dataHead2 >> temp_auiEncounter[0] >> temp_auiEncounter[1] >> temp_auiEncounter[2] >> temp_auiEncounter[3];

				if (dataHead1 == 'T' && dataHead2 == 'E')
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
            return new instance_the_eye_InstanceMapScript(map);
        }
};

void AddSC_instance_the_eye()
{
    new instance_the_eye;
}