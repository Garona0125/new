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
#include "halls_of_lightning.h"

class instance_halls_of_lightning : public InstanceMapScript
{
public:
    instance_halls_of_lightning() : InstanceMapScript("instance_halls_of_lightning", 602) { }

    struct instance_halls_of_lightning_InstanceMapScript : public InstanceScript
    {
        instance_halls_of_lightning_InstanceMapScript(Map* map) : InstanceScript(map) {}

        uint32 Encounter[MAX_ENCOUNTER];

        uint64 GeneralBjarngrimGUID;
        uint64 IonarGUID;
        uint64 LokenGUID;
        uint64 VolkhanGUID;
		uint64 VolkhanAnvilGUID;

        uint64 BjarngrimDoorGUID;
        uint64 VolkhanDoorGUID;
        uint64 IonarDoorGUID;
        uint64 LokenDoorGUID;
        uint64 LokenGlobeGUID;

        void Initialize()
        {
            memset(&Encounter, 0, sizeof(Encounter));

            GeneralBjarngrimGUID = 0;
            VolkhanGUID          = 0;
            IonarGUID            = 0;
            LokenGUID            = 0;
			VolkhanAnvilGUID	 = 0;

            BjarngrimDoorGUID    = 0;
            VolkhanDoorGUID      = 0;
            IonarDoorGUID        = 0;
            LokenDoorGUID        = 0;
            LokenGlobeGUID       = 0;
        }

        void OnCreatureCreate(Creature* creature)
        {
            switch (creature->GetEntry())
            {
                case NPC_BJARNGRIM:
                    GeneralBjarngrimGUID = creature->GetGUID();
                    break;
                case NPC_VOLKHAN:
                    VolkhanGUID = creature->GetGUID();
                    break;
                case NPC_IONAR:
                    IonarGUID = creature->GetGUID();
                    break;
                case NPC_LOKEN:
                    LokenGUID = creature->GetGUID();
                    break;
                case NPC_VOLKHAN_ANVIL:
                    VolkhanAnvilGUID = creature->GetGUID();
                    break;
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case GO_BJARNGRIM_DOOR:
                    BjarngrimDoorGUID = go->GetGUID();
                    if (Encounter[0] == DONE)
                        go->SetGoState(GO_STATE_ACTIVE);
                    else
                        go->SetGoState(GO_STATE_READY);
                    break;
                case GO_VOLKHAN_DOOR:
                    VolkhanDoorGUID = go->GetGUID();
                    if (Encounter[1] == DONE)
                        go->SetGoState(GO_STATE_ACTIVE);
                    else
                        go->SetGoState(GO_STATE_READY);
                    break;
                case GO_IONAR_DOOR:
                    IonarDoorGUID = go->GetGUID();
                    if (Encounter[2] == DONE)
                        go->SetGoState(GO_STATE_ACTIVE);
                    else
                        go->SetGoState(GO_STATE_READY);
                    break;
                case GO_LOKEN_DOOR:
                    LokenDoorGUID = go->GetGUID();
                    if (Encounter[3] == DONE)
                        go->SetGoState(GO_STATE_ACTIVE);
                    else
                        go->SetGoState(GO_STATE_READY);
                    break;
                case GO_LOKEN_THRONE:
                    LokenGlobeGUID = go->GetGUID();
                    break;
            }
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
                case TYPE_BJARNGRIM:
					if (data == DONE)
					{
						DoUseDoorOrButton(BjarngrimDoorGUID);
						GameObject* go = instance->GetGameObject(BjarngrimDoorGUID);
						go->SetGoState(GO_STATE_ACTIVE);
					}
                    Encounter[0] = data;
                    break;
                case TYPE_VOLKHAN:
                    if (data == DONE)
                        DoUseDoorOrButton(VolkhanDoorGUID);
                    Encounter[1] = data;
                    break;
                case TYPE_IONAR:
                    if (data == DONE)
                        DoUseDoorOrButton(IonarDoorGUID);
                    Encounter[2] = data;
                    break;
                case TYPE_LOKEN:
                    if (data == DONE)
                    {
                        DoUseDoorOrButton(LokenDoorGUID);

                        // Appears to be type 5 GO with animation. Need to figure out how this work, code below only placeholder
                        if (GameObject* globe = instance->GetGameObject(LokenGlobeGUID))
                            globe->SetGoState(GO_STATE_ACTIVE);
                    }
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
                case TYPE_BJARNGRIM:
                    return Encounter[0];
                case TYPE_VOLKHAN:
                    return Encounter[1];
                case TYPE_IONAR:
                    return Encounter[2];
                case TYPE_LOKEN:
                    return Encounter[3];
            }
            return 0;
        }

        uint64 GetData64(uint32 data)
        {
            switch (data)
            {
                case DATA_BJARNGRIM:
                    return GeneralBjarngrimGUID;
                case DATA_VOLKHAN:
                    return VolkhanGUID;
                case DATA_IONAR:
                    return IonarGUID;
                case DATA_LOKEN:
                    return LokenGUID;
			case DATA_VOLKHAN_ANVIL:
				return VolkhanAnvilGUID;
            }
            return 0;
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << "H L " << Encounter[0] << ' ' << Encounter[1] << ' '
            << Encounter[2] << ' ' << Encounter[3];

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
            uint16 data0, data1, data2, data3;

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3;

            if (dataHead1 == 'H' && dataHead2 == 'L')
            {
                Encounter[0] = data0;
                Encounter[1] = data1;
                Encounter[2] = data2;
                Encounter[3] = data3;

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
					if (Encounter[i] == IN_PROGRESS)
						Encounter[i] = NOT_STARTED;
            }
			else OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_halls_of_lightning_InstanceMapScript(map);
    }
};

void AddSC_instance_halls_of_lightning()
{
    new instance_halls_of_lightning();
}