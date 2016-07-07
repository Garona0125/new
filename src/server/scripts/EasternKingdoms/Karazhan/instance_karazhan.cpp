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
#include "karazhan.h"

class instance_karazhan : public InstanceMapScript
{
public:
    instance_karazhan() : InstanceMapScript("instance_karazhan", 532) { }

    struct instance_karazhan_InstanceMapScript : public InstanceScript
    {
        instance_karazhan_InstanceMapScript(Map* map) : InstanceScript(map) {}

        uint32 Encounter[MAX_ENCOUNTER];
        std::string strSaveData;
        uint32 OperaEvent;
        uint32 OzDeathCount;
        uint64 CurtainGUID;
        uint64 StageDoorLeftGUID;
        uint64 StageDoorRightGUID;
        uint64 KilrekGUID;
        uint64 TerestianGUID;
        uint64 MoroesGUID;
        uint64 LibraryDoor;                                     // Door at Shade of Aran
        uint64 MassiveDoor;                                     // Door at Netherspite
        uint64 SideEntranceDoor;                                // Side Entrance
        uint64 GamesmansDoor;                                   // Door before Chess
        uint64 GamesmansExitDoor;                               // Door after Chess
        uint64 NetherspaceDoor;                                // Door at Malchezaar
        uint64 MastersTerraceDoor[2];
        uint64 ImageGUID;
        uint64 DustCoveredChest;

        void Initialize()
        {
            memset(&Encounter, 0, sizeof(Encounter));

            OperaEvent  = urand(1, 3);
            OzDeathCount = 0;
            CurtainGUID = 0;
            StageDoorLeftGUID = 0;
            StageDoorRightGUID = 0;
            KilrekGUID = 0;
            TerestianGUID = 0;
            MoroesGUID = 0;
            LibraryDoor = 0;
            MassiveDoor = 0;
            SideEntranceDoor = 0;
            GamesmansDoor = 0;
            GamesmansExitDoor = 0;
            NetherspaceDoor = 0;
            MastersTerraceDoor[0]= 0;
            MastersTerraceDoor[1]= 0;
            ImageGUID = 0;
            DustCoveredChest = 0;
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
                case NPC_MOROES:
					MoroesGUID = creature->GetGUID();
					break;
				case NPC_TERESTIAN:
					TerestianGUID = creature->GetGUID();
					break;
				case NPC_KILREK:
					KilrekGUID = creature->GetGUID();
					break;
            }
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
                case TYPE_ATTUMEN:
					Encounter[0] = data;
					break;
                case TYPE_MOROES:
                    if (Encounter[1] == DONE)
						break;
					Encounter[1] = data;
                    break;
                case TYPE_MAIDEN:
					Encounter[2] = data;
					break;
                case TYPE_OPTIONAL_BOSS:
					Encounter[3] = data;
					break;
                case TYPE_OPERA:
					Encounter[4] = data;
					break;
                case TYPE_CURATOR:
					Encounter[5] = data;
					break;
                case TYPE_ARAN:
					Encounter[6] = data;
					break;
                case TYPE_TERESTIAN:
					Encounter[7] = data;
					break;
                case TYPE_NETHERSPITE:
					Encounter[8] = data;
					break;
                case TYPE_CHESS:
                    if (data == DONE)
                        DoRespawnGameObject(DustCoveredChest, DAY);
                    Encounter[9]  = data;
                    break;
                case TYPE_MALCHEZZAR:
					Encounter[10] = data;
					break;
                case TYPE_NIGHTBANE:
                    if (Encounter[11] != DONE)
                        Encounter[11] = data;
                    break;
                case DATA_OPERA_OZ_DEATHCOUNT:
                    if (data == SPECIAL)
                        ++OzDeathCount;
                    else if (data == IN_PROGRESS)
                        OzDeathCount = 0;
                    break;
            }

            if (data == DONE)
            {
                OUT_SAVE_INST_DATA;
				
				std::ostringstream saveStream;
				saveStream << "K A " << Encounter[0] << ' ' << Encounter[1] << ' ' << Encounter[2] << ' '
					<< Encounter[3] << ' ' << Encounter[4] << ' ' << Encounter[5] << ' ' << Encounter[6] << ' '
                    << Encounter[7] << ' ' << Encounter[8] << ' ' << Encounter[9] << ' ' << Encounter[10] << ' ' << Encounter[11];

                strSaveData = saveStream.str();

                SaveToDB();
                OUT_SAVE_INST_DATA_COMPLETE;
            }
        }

         void SetData64(uint32 identifier, uint64 data)
         {
             switch (identifier)
             {
			     case DATA_IMAGE_OF_MEDIVH:
					 ImageGUID = data;
             }
         }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case 183932:
					CurtainGUID = go->GetGUID();
					break;
                case 184278:
                    StageDoorLeftGUID = go->GetGUID();
                    if (Encounter[4] == DONE)
                        go->SetGoState(GO_STATE_ACTIVE);
                    break;
                case 184279:
                    StageDoorRightGUID = go->GetGUID();
                    if (Encounter[4] == DONE)
                        go->SetGoState(GO_STATE_ACTIVE);
                    break;
                case 184517:
					LibraryDoor = go->GetGUID();
					break;
                case 185521:
					MassiveDoor = go->GetGUID();
					break;
                case 184276:
					GamesmansDoor = go->GetGUID();
					break;
                case 184277:
					GamesmansExitDoor = go->GetGUID();
					break;
                case 185134:
					NetherspaceDoor = go->GetGUID();
					break;
                case 184274:
					MastersTerraceDoor[0] = go->GetGUID();
					break;
                case 184280:
					MastersTerraceDoor[1] = go->GetGUID();
					break;
                case 184275:
                    SideEntranceDoor = go->GetGUID();
                    if (Encounter[4] == DONE)
                        go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
                    else
                        go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
                    break;
                case 185119:
					DustCoveredChest = go->GetGUID();
					break;
            }

            switch (OperaEvent)
            {
                //TODO: Set Object visibilities for Opera based on performance
                case EVENT_OZ:
                    break;
                case EVENT_HOOD:
                    break;
                case EVENT_RAJ:
                    break;
            }
        }

        std::string GetSaveData()
        {
            return strSaveData;
        }

        uint32 GetData(uint32 data)
        {
            switch (data)
            {
                case TYPE_ATTUMEN:
					return Encounter[0];
                case TYPE_MOROES:
					return Encounter[1];
                case TYPE_MAIDEN:
					return Encounter[2];
                case TYPE_OPTIONAL_BOSS:
					return Encounter[3];
                case TYPE_OPERA:
					return Encounter[4];
                case TYPE_CURATOR:
					return Encounter[5];
                case TYPE_ARAN:
					return Encounter[6];
                case TYPE_TERESTIAN:
					return Encounter[7];
                case TYPE_NETHERSPITE:
					return Encounter[8];
                case TYPE_CHESS:
					return Encounter[9];
                case TYPE_MALCHEZZAR:
					return Encounter[10];
                case TYPE_NIGHTBANE:
					return Encounter[11];
                case DATA_OPERA_PERFORMANCE:
					return OperaEvent;
                case DATA_OPERA_OZ_DEATHCOUNT:
					return OzDeathCount;
            }
            return 0;
        }

        uint64 GetData64(uint32 data)
        {
            switch (data)
            {
                case DATA_KILREK:
					return KilrekGUID;
                case DATA_TERESTIAN:
					return TerestianGUID;
                case DATA_MOROES:
					return MoroesGUID;
                case DATA_GO_STAGEDOORLEFT:
					return StageDoorLeftGUID;
                case DATA_GO_STAGEDOORRIGHT:
					return StageDoorRightGUID;
                case DATA_GO_CURTAINS:
					return CurtainGUID;
                case DATA_GO_LIBRARY_DOOR:
					return LibraryDoor;
                case DATA_GO_MASSIVE_DOOR:
					return MassiveDoor;
                case DATA_GO_SIDE_ENTRANCE_DOOR:
					return SideEntranceDoor;
                case DATA_GO_GAME_DOOR:
					return GamesmansDoor;
                case DATA_GO_GAME_EXIT_DOOR:
					return GamesmansExitDoor;
                case DATA_GO_NETHER_DOOR:
					return NetherspaceDoor;
                case DATA_MASTERS_TERRACE_DOOR_1:
					return MastersTerraceDoor[0];
                case DATA_MASTERS_TERRACE_DOOR_2:
					return MastersTerraceDoor[1];
                case DATA_IMAGE_OF_MEDIVH:
					return ImageGUID;
            }
            return 0;
        }

        void Load(const char* chrIn)
        {
            if (!chrIn)
            {
                OUT_LOAD_INST_DATA_FAIL;
                return;
            }

            OUT_LOAD_INST_DATA(chrIn);
            std::istringstream loadStream(chrIn);

			char check1, check2;
			uint32 EncounterLoad[MAX_ENCOUNTER];
			memset(&EncounterLoad, 0, sizeof(EncounterLoad));

			loadStream >> check1 >> check2 >> EncounterLoad[0] >> EncounterLoad[1] >> EncounterLoad[2] >> EncounterLoad[3]
			>> EncounterLoad[4] >> EncounterLoad[5] >> EncounterLoad[6] >> EncounterLoad[7]
			>> EncounterLoad[8] >> EncounterLoad[9] >> EncounterLoad[10] >> EncounterLoad[11];

			if (check1 == 'K' && check2 == 'A')
			{
				for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
					Encounter[i] = EncounterLoad[4];
			}
			else
			{
				OUT_LOAD_INST_DATA_FAIL;
				return;
			}

			for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
			{
				if (Encounter[i] == IN_PROGRESS)
					Encounter[i] = NOT_STARTED;
			}

			OUT_LOAD_INST_DATA_COMPLETE;
		}
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_karazhan_InstanceMapScript(map);
    }
};

void AddSC_instance_karazhan()
{
    new instance_karazhan();
}