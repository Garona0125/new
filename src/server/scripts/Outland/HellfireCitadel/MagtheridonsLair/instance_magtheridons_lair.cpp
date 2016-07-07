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
#include "magtheridons_lair.h"

#define MAX_ENCOUNTER 1

class instance_magtheridons_lair : public InstanceMapScript
{
public:
    instance_magtheridons_lair() : InstanceMapScript("instance_magtheridons_lair", 544) { }

    struct instance_magtheridons_lair_InstanceMapScript : public InstanceScript
    {
        instance_magtheridons_lair_InstanceMapScript(Map* map) : InstanceScript(map){}

        uint32 Encounter[MAX_ENCOUNTER];

        uint64 MagtheridonGUID;
        uint64 DoorGUID;
        std::set<uint64> ColumnGUID;

        void Initialize()
        {
            memset(&Encounter, 0, sizeof(Encounter));

            MagtheridonGUID = 0;
            DoorGUID = 0;
            ColumnGUID.clear();
        }

        bool IsEncounterInProgress() const
        {
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                if (Encounter[i] == IN_PROGRESS) return true;
            return false;
        }

        void OnCreatureCreate(Creature* creature)
        {
            switch (creature->GetEntry())
            {
                case NPC_MAGTHERIDON:
                    MagtheridonGUID = creature->GetGUID();
                    break;
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case GO_MANTICRON_CUBE:
                    go->SetUInt32Value(GAMEOBJECT_FLAGS, 0);
                    break;
                case GO_MAG_DOOR_1:
                    DoorGUID = go->GetGUID();
                    break;
                case GO_HELLFIRE_GATE:
                case GO_MAGTERIDON_COLUMN:
                case GO_MAGTERIDON_COLUMN_1:
                case GO_MAGTERIDON_COLUMN_2:
                case GO_MAGTERIDON_COLUMN_3:
                case GO_MAGTERIDON_COLUMN_4:
                case GO_MAGTERIDON_COLUMN_5:
                    ColumnGUID.insert(go->GetGUID());
                    break;
            }
        }

        uint64 GetData64(uint32 type)
        {
            switch (type)
            {
                case DATA_MAGTHERIDON:
                    return MagtheridonGUID;
            }
            return 0;
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
                case DATA_MAGTHERIDON_EVENT:
                    if (Encounter[0] == data)
                        return;

                    Encounter[0] = data;
                    HandleGameObject(DoorGUID, Encounter[0] != IN_PROGRESS);
                    break;
                case DATA_COLLAPSE:
                    // true - collapse / false - reset
                    for (std::set<uint64>::const_iterator i = ColumnGUID.begin(); i != ColumnGUID.end(); ++i)
                        DoUseDoorOrButton(*i);
                    break;
                default:
                    break;
            }
        }

        uint32 GetData(uint32 type)
        {
            if (type == DATA_MAGTHERIDON_EVENT)
                return Encounter[0];
            return 0;
        }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_magtheridons_lair_InstanceMapScript(map);
    }
};

void AddSC_instance_magtheridons_lair()
{
    new instance_magtheridons_lair();
}