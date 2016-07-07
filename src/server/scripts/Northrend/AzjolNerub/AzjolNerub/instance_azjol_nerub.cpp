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
#include "azjol_nerub.h"

class instance_azjol_nerub : public InstanceMapScript
{
public:
	instance_azjol_nerub() : InstanceMapScript("instance_azjol_nerub", 601) { }

	struct instance_azjol_nerub_InstanceScript : public InstanceScript
    {
		instance_azjol_nerub_InstanceScript(Map* map) : InstanceScript(map) {}

        uint64 Krikthir;
        uint64 Hadronox;
        uint64 Anubarak;
        uint64 WatcherGashra;
        uint64 WatcherSilthik;
        uint64 WatcherNarjil;
		uint64 KrikthirDoor;
        uint64 AnubarakDoor[3];

        uint32 Encounter[MAX_ENCOUNTER];

       void Initialize()
       {
            memset(&Encounter, 0, sizeof(Encounter));
            memset(&AnubarakDoor, 0, sizeof(AnubarakDoor));

            Krikthir = 0;
            Hadronox = 0;
            Anubarak = 0;
            WatcherGashra = 0;
            WatcherSilthik = 0;
            WatcherNarjil = 0;
            KrikthirDoor = 0;
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
                case NPC_KRIKTHIR_THE_GATEWATCHER:
					Krikthir = creature->GetGUID();
					break;
                case NPC_HADRONOX:
					Hadronox = creature->GetGUID();
					break;
                case NPC_ANUBARAK:
					Anubarak = creature->GetGUID();
					break;
                case NPC_WATCHER_GASHRA:
					WatcherGashra = creature->GetGUID();
					break;
                case NPC_WATCHER_SILTHIK:
					WatcherSilthik = creature->GetGUID();
					break;
                case NPC_WATCHER_NARJIL:
					WatcherNarjil = creature->GetGUID();
					break;
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case GO_KRIKTHIR_DOOR:
                    KrikthirDoor = go->GetGUID();
                    if (Encounter[0] == DONE)
                        HandleGameObject(0, true, go);
                    break;
                case GO_ANUBARAK_DOOR:
                    AnubarakDoor[0] = go->GetGUID();
                    break;
                case GO_ANUBARAK_DOOR_1:
                    AnubarakDoor[1] = go->GetGUID();
                    break;
                case GO_ANUBARAK_DOOR_2:
                    AnubarakDoor[2] = go->GetGUID();
                    break;
            }
        }

        uint64 GetData64(uint32 identifier)
        {
            switch (identifier)
            {
                case DATA_KRIKTHIR_THE_GATEWATCHER:
					return Krikthir;
                case DATA_HADRONOX:
					return Hadronox;
                case DATA_ANUBARAK:
					return Anubarak;
                case DATA_WATCHER_GASHRA:
					return WatcherGashra;
                case DATA_WATCHER_SILTHIK:
					return WatcherSilthik;
                case DATA_WATCHER_NARJIL:
					return WatcherNarjil;
            }

            return 0;
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
            case DATA_KRIKTHIR_THE_GATEWATCHER_EVENT:
                Encounter[0] = data;
                if (data == DONE)
                    HandleGameObject(KrikthirDoor, true);
                break;
            case DATA_HADRONOX_EVENT:
                Encounter[1] = data;
                break;
            case DATA_ANUBARAK_EVENT:
                Encounter[2] = data;
                if (data == IN_PROGRESS)
                    for (uint8 i = 0; i < 3; ++i)
                        HandleGameObject(AnubarakDoor[i], false);
				else if (data == NOT_STARTED || data == DONE)
					for (uint8 i = 0; i < 3; ++i)
                        HandleGameObject(AnubarakDoor[i], true);
                break;
            }

            if (data == DONE)
            {
                SaveToDB();
            }
        }

        uint32 GetData(uint32 type)
        {
            switch (type)
            {
                case DATA_KRIKTHIR_THE_GATEWATCHER_EVENT:
					return Encounter[0];
                case DATA_HADRONOX_EVENT:
					return Encounter[1];
                case DATA_ANUBARAK_EVENT:
					return Encounter[2];
            }
            return 0;
		}

		std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << "A N " << Encounter[0] << ' ' << Encounter[1] << ' '
                << Encounter[2];

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
            uint16 data0, data1, data2;

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2;

            if (dataHead1 == 'A' && dataHead2 == 'N')
            {
                Encounter[0] = data0;
                Encounter[1] = data1;
                Encounter[2] = data2;

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
        return new instance_azjol_nerub_InstanceScript(map);
    }
};

void AddSC_instance_azjol_nerub()
{
   new instance_azjol_nerub;
}