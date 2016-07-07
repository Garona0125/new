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
#include "utgarde_keep.h"

class instance_utgarde_keep : public InstanceMapScript
{
public:
    instance_utgarde_keep() : InstanceMapScript("instance_utgarde_keep", 574) { }

    struct instance_utgarde_keep_InstanceMapScript : public InstanceScript
    {
        instance_utgarde_keep_InstanceMapScript(Map* map) : InstanceScript(map) {}

        uint64 KelesethGUID;
        uint64 SkarvaldGUID;
        uint64 DalronnGUID;
        uint64 IngvarGUID;

        uint64 forge_bellow[3];
        uint64 forge_fire[3];
        uint64 forge_anvil[3];
        uint64 portcullis[2];
		uint64 DalronGateGUID;

        uint32 Encounter[MAX_ENCOUNTER];
        uint32 forge_event[3];
        std::string str_data;

       void Initialize()
       {
            memset(&Encounter, 0, sizeof(Encounter));

            KelesethGUID = 0;
            SkarvaldGUID = 0;
            DalronnGUID = 0;
            IngvarGUID = 0;

            for (uint8 i = 0; i < 3; ++i)
            {
                forge_bellow[i] = 0;
                forge_fire[i] = 0;
                forge_anvil[i] = 0;
                forge_event[i] = NOT_STARTED;
            }

            portcullis[0] = 0;
            portcullis[1] = 0;
			DalronGateGUID = 0;
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

        Player* GetPlayerInMap()
        {
            Map::PlayerList const& players = instance->GetPlayers();

            if (!players.isEmpty())
            {
                for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                {
                    if (Player* player = itr->getSource())
						return player;
                }
            };

            return NULL;
        }

        void OnCreatureCreate(Creature* creature)
        {
            switch (creature->GetEntry())
            {
                case NPC_PRINCE_KELESETH:
					KelesethGUID = creature->GetGUID();
					break;
                case NPC_DALRONN_THE_CONTROLLER:
					DalronnGUID = creature->GetGUID();
					break;
                case NPC_SKARVALD_THE_CONSTRUCTOR:
					SkarvaldGUID = creature->GetGUID();
					break;
                case NPC_INGVAR_THE_PLUNDERER:
					IngvarGUID = creature->GetGUID();
					break;
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
            //door and object id
            case GO_BELLOW_1:
				forge_bellow[0] = go->GetGUID();
				if (forge_event[0] != NOT_STARTED)
					HandleGameObject(0, true, go);
				break;
            case GO_BELLOW_2:
				forge_bellow[1] = go->GetGUID();
				if (forge_event[1] != NOT_STARTED)
					HandleGameObject(0, true, go);
				break;
            case GO_BELLOW_3:
				forge_bellow[2] = go->GetGUID();
				if (forge_event[2] != NOT_STARTED)
					HandleGameObject(0, true, go);
				break;
            case GO_FORGE_FIRE_1:
				forge_fire[0] = go->GetGUID();
				if (forge_event[0] != NOT_STARTED)
					HandleGameObject(0, true, go);
				break;
            case GO_FORGE_FIRE_2:
				forge_fire[1] = go->GetGUID();
				if (forge_event[1] != NOT_STARTED)
					HandleGameObject(0, true, go);
				break;
            case GO_FORGE_FIRE_3:
				forge_fire[2] = go->GetGUID();
				if (forge_event[2] != NOT_STARTED)
					HandleGameObject(0, true, go);
				break;
            case GO_GLOWING_ANVIL_1:
				forge_anvil[0] = go->GetGUID();
				if (forge_event[0] != NOT_STARTED)
					HandleGameObject(0, true, go);
				break;
            case GO_GLOWING_ANVIL_2:
				forge_anvil[1] = go->GetGUID();
				if (forge_event[1] != NOT_STARTED)
					HandleGameObject(0, true, go);
				break;
            case GO_GLOWING_ANVIL_3:
				forge_anvil[2] = go->GetGUID();
				if (forge_event[2] != NOT_STARTED)
					HandleGameObject(0, true, go);
				break;
			case GO_GIANT_PORTCULLIS_0:
				DalronGateGUID = go->GetGUID();
				break;
            case GO_GIANT_PORTCULLIS_1:
				portcullis[0] = go->GetGUID();
				if (Encounter[2] == DONE)
					HandleGameObject(0, true, go);
				break;
            case GO_GIANT_PORTCULLIS_2:
				portcullis[1] = go->GetGUID();
				if (Encounter[2] == DONE)
					HandleGameObject(0, true, go);
				break;
            }
        }

        uint64 GetData64(uint32 identifier)
        {
            switch (identifier)
            {
                case DATA_PRINCE_KELESETH:
					return KelesethGUID;
                case DATA_DALRONN:
					return DalronnGUID;
                case DATA_SKARVALD:
					return SkarvaldGUID;
                case DATA_INGVAR:
					return IngvarGUID;
            }

            return 0;
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
            case DATA_PRINCE_KELESETH_EVENT:
                Encounter[0] = data;
                break;
            case DATA_SKARVALD_DALRONN_EVENT:
                Encounter[1] = data;
				if (data == NOT_STARTED)
				{
					HandleGameObject(DalronGateGUID, true);
				}
				else if (data == IN_PROGRESS)
				{
					HandleGameObject(DalronGateGUID, false);
				}
				else if (data == DONE)
				{
					HandleGameObject(DalronGateGUID, true);
				}
                break;
            case DATA_INGVAR_EVENT:
                if (data == DONE)
                {
                    HandleGameObject(portcullis[0], true);
                    HandleGameObject(portcullis[1], true);
                }
                Encounter[2] = data;
                break;
            case EVENT_FORGE_1:
                if (data == NOT_STARTED)
                {
                    HandleGameObject(forge_bellow[0], false);
                    HandleGameObject(forge_fire[0], false);
                    HandleGameObject(forge_anvil[0], false);
                }
				else
                {
                    HandleGameObject(forge_bellow[0], true);
                    HandleGameObject(forge_fire[0], true);
                    HandleGameObject(forge_anvil[0], true);
                }
                forge_event[0] = data;
                break;
            case EVENT_FORGE_2:
                if (data == NOT_STARTED)
                {
                    HandleGameObject(forge_bellow[1], false);
                    HandleGameObject(forge_fire[1], false);
                    HandleGameObject(forge_anvil[1], false);
                }
				else
                {
                    HandleGameObject(forge_bellow[1], true);
                    HandleGameObject(forge_fire[1], true);
                    HandleGameObject(forge_anvil[1], true);
                }
                forge_event[1] = data;
                break;
            case EVENT_FORGE_3:
                if (data == NOT_STARTED)
                {
                    HandleGameObject(forge_bellow[2], false);
                    HandleGameObject(forge_fire[2], false);
                    HandleGameObject(forge_anvil[2], false);
                }
				else
                {
                    HandleGameObject(forge_bellow[2], true);
                    HandleGameObject(forge_fire[2], true);
                    HandleGameObject(forge_anvil[2], true);
                }
                forge_event[2] = data;
                break;
            }

            if (data == DONE)
                SaveToDB();
        }

        uint32 GetData(uint32 type)
        {
            switch (type)
            {
                case DATA_PRINCE_KELESETH_EVENT:
					return Encounter[0];
                case DATA_SKARVALD_DALRONN_EVENT:
					return Encounter[1];
                case DATA_INGVAR_EVENT:
					return Encounter[2];
            }

            return 0;
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << "U K " << Encounter[0] << ' ' << Encounter[1] << ' '
                << Encounter[2] << ' ' << forge_event[0] << ' ' << forge_event[1] << ' ' << forge_event[2];

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
            uint16 data0, data1, data2, data3, data4, data5;

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3 >> data4 >> data5;

            if (dataHead1 == 'U' && dataHead2 == 'K')
            {
                Encounter[0] = data0;
                Encounter[1] = data1;
                Encounter[2] = data2;

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
					if (Encounter[i] == IN_PROGRESS)
						Encounter[i] = NOT_STARTED;

                forge_event[0] = data3;
                forge_event[1] = data4;
                forge_event[2] = data5;

            }
			else OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
       return new instance_utgarde_keep_InstanceMapScript(map);
    }
};

void AddSC_instance_utgarde_keep()
{
    new instance_utgarde_keep();
}