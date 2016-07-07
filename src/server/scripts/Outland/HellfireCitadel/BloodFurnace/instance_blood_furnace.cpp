/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "blood_furnace.h"

class instance_blood_furnace : public InstanceMapScript
{
    public:
        instance_blood_furnace() : InstanceMapScript("instance_blood_furnace", 542) { }

        struct instance_blood_furnace_InstanceMapScript : public InstanceScript
        {
			instance_blood_furnace_InstanceMapScript(Map* map) : InstanceScript(map) {}

            uint64 TheMakerGUID;
            uint64 BroggokGUID;
            uint64 KelidanTheBreakerGUID;

            uint64 Door1GUID;
            uint64 Door2GUID;
            uint64 Door3GUID;
            uint64 Door4GUID;
            uint64 Door5GUID;
            uint64 Door6GUID;

            uint64 PrisonCell1GUID;
            uint64 PrisonCell2GUID;
            uint64 PrisonCell3GUID;
            uint64 PrisonCell4GUID;
            uint64 PrisonCell5GUID;
            uint64 PrisonCell6GUID;
            uint64 PrisonCell7GUID;
            uint64 PrisonCell8GUID;

            uint32 Encounter[MAX_ENCOUNTER];
            std::string str_data;

            void Initialize()
            {
                memset(&Encounter, 0, sizeof(Encounter));

                TheMakerGUID = 0;
                BroggokGUID = 0;
                KelidanTheBreakerGUID = 0;

                Door1GUID = 0;
                Door2GUID = 0;
                Door3GUID = 0;
                Door4GUID = 0;
                Door5GUID = 0;
                Door6GUID = 0;

                PrisonCell1GUID = 0;
                PrisonCell2GUID = 0;
                PrisonCell3GUID = 0;
                PrisonCell4GUID = 0;
                PrisonCell5GUID = 0;
                PrisonCell6GUID = 0;
                PrisonCell7GUID = 0;
                PrisonCell8GUID = 0;
            }

            void OnCreatureCreate(Creature* creature)
            {
                switch (creature->GetEntry())
                {
                     case NPC_THE_MAKER:
                         TheMakerGUID = creature->GetGUID();
                         break;
                     case NPC_BROGGOK:
                         BroggokGUID = creature->GetGUID();
                         break;
                     case NPC_KELIDAN:
                         KelidanTheBreakerGUID = creature->GetGUID();
                         break;
                }
            }

            void OnGameObjectCreate(GameObject* go)
            {
                 if (go->GetEntry() == GO_PRISON_DOOR_1)                //Final exit door
                     Door1GUID = go->GetGUID();
                 if (go->GetEntry() == GO_PRISON_DOOR_2)               //The Maker Front door
                     Door2GUID = go->GetGUID();
                 if (go->GetEntry() == GO_PRISON_DOOR_3)                //The Maker Rear door
                     Door3GUID = go->GetGUID();
                 if (go->GetEntry() == GO_PRISON_DOOR_5)               //Broggok Front door
                     Door4GUID = go->GetGUID();
                 if (go->GetEntry() == GO_PRISON_DOOR_4)               //Broggok Rear door
                     Door5GUID = go->GetGUID();
                 if (go->GetEntry() == GO_SUMMON_DOOR_1)               //Kelidan exit door
                     Door6GUID = go->GetGUID();
                 if (go->GetEntry() == GO_PRISON_CELL_DOOR_1)               //The Maker prison cell front right
                     PrisonCell1GUID = go->GetGUID();
                 if (go->GetEntry() == GO_PRISON_CELL_DOOR_2)              //The Maker prison cell back right
                     PrisonCell2GUID = go->GetGUID();
                 if (go->GetEntry() == GO_PRISON_CELL_DOOR_3)             //The Maker prison cell front left
                     PrisonCell3GUID = go->GetGUID();
                 if (go->GetEntry() == GO_PRISON_CELL_DOOR_4)             //The Maker prison cell back left
                     PrisonCell4GUID = go->GetGUID();
                 if (go->GetEntry() == GO_PRISON_CELL_DOOR_5)             //Broggok prison cell front right
                     PrisonCell5GUID = go->GetGUID();
                 if (go->GetEntry() == GO_PRISON_CELL_DOOR_6)             //Broggok prison cell back right
                     PrisonCell6GUID = go->GetGUID();
                 if (go->GetEntry() == GO_PRISON_CELL_DOOR_7)              //Broggok prison cell front left
                     PrisonCell7GUID = go->GetGUID();
                 if (go->GetEntry() == GO_PRISON_CELL_DOOR_8)              //Broggok prison cell back left
                     PrisonCell8GUID = go->GetGUID();
            }

            uint64 GetData64(uint32 data)
            {
                switch (data)
                {
                     case DATA_THE_MAKER:
						 return TheMakerGUID;
                     case DATA_BROGGOK:
						 return BroggokGUID;
                     case DATA_KELIDAN_THE_MAKER:
						 return KelidanTheBreakerGUID;
                     case DATA_DOOR_1:
						 return Door1GUID;
                     case DATA_DOOR_2:
						 return Door2GUID;
                     case DATA_DOOR_3:
						 return Door3GUID;
                     case DATA_DOOR_4:
						 return Door4GUID;
                     case DATA_DOOR_5:
						 return Door5GUID;
                     case DATA_DOOR_6:
						 return Door6GUID;
                     case DATA_PRISON_CELL_1:
						 return PrisonCell1GUID;
                     case DATA_PRISON_CELL_2:
						 return PrisonCell2GUID;
                     case DATA_PRISON_CELL_3:
						 return PrisonCell3GUID;
                     case DATA_PRISON_CELL_4:
						 return PrisonCell4GUID;
                     case DATA_PRISON_CELL_5:
						 return PrisonCell5GUID;
                     case DATA_PRISON_CELL_6:
						 return PrisonCell6GUID;
                     case DATA_PRISON_CELL_7:
						 return PrisonCell7GUID;
                     case DATA_PRISON_CELL_8:
						 return PrisonCell8GUID;
                }
                return 0;
            }

            void SetData(uint32 /*type*/, uint32 data)
            {
                 switch (data)
                 {
                     case TYPE_THE_MAKER_EVENT:
						 Encounter[0] = data;
						 break;
                     case TYPE_BROGGOK_EVENT:
						 Encounter[1] = data;
						 break;
                     case TYPE_KELIDAN_THE_BREAKER_EVENT:
						 Encounter[2] = data;
						 break;
                 }

                if (data == DONE)
                {
                    OUT_SAVE_INST_DATA;

                    std::ostringstream saveStream;
                    saveStream << Encounter[0] << ' ' << Encounter[1] << ' ' << Encounter[2];

                    str_data = saveStream.str();

                    SaveToDB();
                    OUT_SAVE_INST_DATA_COMPLETE;
                }
            }

            uint32 GetData(uint32 data)
            {
                switch (data)
                {
                    case TYPE_THE_MAKER_EVENT:
						return Encounter[0];
                    case TYPE_BROGGOK_EVENT:
						return Encounter[1];
                    case TYPE_KELIDAN_THE_BREAKER_EVENT:
						return Encounter[2];
                }
                return 0;
            }

            const char* Save()
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

                OUT_LOAD_INST_DATA(in);

                std::istringstream loadStream(in);
                loadStream >> Encounter[0] >> Encounter[1] >> Encounter[2];

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (Encounter[i] == IN_PROGRESS || Encounter[i] == FAIL)
                        Encounter[i] = NOT_STARTED;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_blood_furnace_InstanceMapScript(map);
        }
};

void AddSC_instance_blood_furnace()
{
    new instance_blood_furnace();
}