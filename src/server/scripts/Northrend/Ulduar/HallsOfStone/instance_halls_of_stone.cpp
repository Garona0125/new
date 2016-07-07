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
#include "halls_of_stone.h"

class instance_halls_of_stone : public InstanceMapScript
{
public:
    instance_halls_of_stone() : InstanceMapScript("instance_halls_of_stone", 599) { }

    struct instance_halls_of_stone_InstanceMapScript : public InstanceScript
    {
        instance_halls_of_stone_InstanceMapScript(Map* map) : InstanceScript(map) {}

        uint64 MaidenOfGriefGUID;
        uint64 KrystallusGUID;
        uint64 SjonnirGUID;
        uint64 KaddrakGUID;
        uint64 AbedneumGUID;
        uint64 MarnakGUID;
        uint64 BrannBronzebeardGUID;

        uint64 MaidenOfGriefDoorGUID;
        uint64 SjonnirDoorGUID;
        uint64 BrannDoorGUID;
        uint64 TribunalConsoleGUID;
        uint64 TribunalChest;
        uint64 TribunalSkyFloorGUID;
        uint64 KaddrakGoGUID;
        uint64 AbedneumGoGUID;
        uint64 MarnakGoGUID;

        uint32 Encounter[MAX_ENCOUNTER];

        std::string str_data;

        void Initialize()
        {
            MaidenOfGriefGUID = 0;
            KrystallusGUID = 0;
            SjonnirGUID = 0;

            KaddrakGUID = 0;
            MarnakGUID = 0;
            AbedneumGUID = 0;
            BrannBronzebeardGUID = 0;

            MaidenOfGriefDoorGUID = 0;
            SjonnirDoorGUID = 0;
            BrannDoorGUID = 0;
            KaddrakGoGUID = 0;
            MarnakGoGUID = 0;
            AbedneumGoGUID = 0;
            TribunalConsoleGUID = 0;
            TribunalChest = 0;
            TribunalSkyFloorGUID = 0;

            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                Encounter[i] = NOT_STARTED;
        }

        void OnCreatureCreate(Creature* creature)
        {
            switch (creature->GetEntry())
            {
                case NPC_MAIDEN_OF_GRIEF:
					MaidenOfGriefGUID = creature->GetGUID();
					break;
                case NPC_KRYSTALLUS:
					KrystallusGUID = creature->GetGUID();
					break;
                case NPC_SJONNIR:
					SjonnirGUID = creature->GetGUID();
					break;
                case NPC_MARNAK:
					MarnakGUID = creature->GetGUID();
					break;
                case NPC_KADDRAK:
					KaddrakGUID = creature->GetGUID();
					break;
                case NPC_ABEDNEUM:
					AbedneumGUID = creature->GetGUID();
					break;
                case NPC_BRANN_BRONZEBEARD:
					BrannBronzebeardGUID = creature->GetGUID();
					break;
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case GO_ABEDNEUM:
                    AbedneumGoGUID = go->GetGUID();
                    break;
                case GO_MARNAK:
                    MarnakGoGUID = go->GetGUID();
                    break;
                case GO_KADDRAK:
                    KaddrakGoGUID = go->GetGUID();
                    break;
                case GO_MAIDEN_DOOR:
                    MaidenOfGriefDoorGUID = go->GetGUID();
                    if (Encounter[0] == DONE)
                        go->SetGoState(GO_STATE_ACTIVE);
                    else
                        go->SetGoState(GO_STATE_READY);
                    break;
                case GO_BRANN_DOOR:
                    BrannDoorGUID = go->GetGUID();
                    if (Encounter[1] == DONE)
                        go->SetGoState(GO_STATE_ACTIVE);
                    else
                        go->SetGoState(GO_STATE_READY);
                    break;
                case GO_SJONNIR_DOOR:
                    SjonnirDoorGUID = go->GetGUID();
                    if (Encounter[2] == DONE)
                        go->SetGoState(GO_STATE_ACTIVE);
                    else
                        go->SetGoState(GO_STATE_READY);
                    break;
                case GO_TRIBUNAL_CONSOLE:
                    TribunalConsoleGUID = go->GetGUID();
                    break;
                case GO_TRIBUNAL_CHEST:
                case GO_TRIBUNAL_CHEST_HERO:
                    TribunalChest = go->GetGUID();
                    if (Encounter[2] == DONE)
						go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
                    break;
                case GO_TRIBUNAL_SKYROOM_FLOOR:
                    TribunalSkyFloorGUID = go->GetGUID();
                    break;
            }
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
                case DATA_KRYSTALLUS_EVENT:
                    Encounter[0] = data;
                    if (Encounter[0] == DONE)
                        HandleGameObject(MaidenOfGriefDoorGUID, true);
                    break;
                case DATA_MAIDEN_OF_GRIEF_EVENT:
                    Encounter[1] = data;
                    if (Encounter[1] == DONE)
                        HandleGameObject(BrannDoorGUID, true);
                    break;
                case DATA_BRANN_EVENT:
                    Encounter[2] = data;
                    if (Encounter[2] == DONE)
                    {
                        HandleGameObject(SjonnirDoorGUID, true);

                        if (GameObject* go = instance->GetGameObject(TribunalChest))
							go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
                    }
                    break;
                case DATA_SJONNIR_EVENT:
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
                case DATA_KRYSTALLUS_EVENT:
					return Encounter[0];
                case DATA_MAIDEN_OF_GRIEF_EVENT:
					return Encounter[1];
                case DATA_BRANN_EVENT:
					return Encounter[2];
                case DATA_SJONNIR_EVENT:
					return Encounter[3];
            }
            return 0;
        }

        uint64 GetData64(uint32 identifier)
        {
            switch (identifier)
            {
                case DATA_MAIDEN_OF_GRIEF:
					return MaidenOfGriefGUID;
                case DATA_KRYSTALLUS:
					return KrystallusGUID;
                case DATA_SJONNIR:
					return SjonnirGUID;
                case DATA_KADDRAK:
					return KaddrakGUID;
                case DATA_MARNAK:
					return MarnakGUID;
                case DATA_ABEDNEUM:
					return AbedneumGUID;
                case DATA_GO_TRIBUNAL_CONSOLE:
					return TribunalConsoleGUID;
                case DATA_GO_KADDRAK:
					return KaddrakGoGUID;
                case DATA_GO_ABEDNEUM:
					return AbedneumGoGUID;
                case DATA_GO_MARNAK:
					return MarnakGoGUID;
                case DATA_GO_SKY_FLOOR:
					return TribunalSkyFloorGUID;
                case DATA_SJONNIR_DOOR:
					return SjonnirDoorGUID;
                case DATA_MAIDEN_DOOR:
					return MaidenOfGriefDoorGUID;
            }

            return 0;
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << "H S " << Encounter[0] << ' ' << Encounter[1] << ' ' << Encounter[2] << ' ' << Encounter[3];

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

            if (dataHead1 == 'H' && dataHead2 == 'S')
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
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_halls_of_stone_InstanceMapScript(map);
    }
};

void AddSC_instance_halls_of_stone()
{
    new instance_halls_of_stone();
}