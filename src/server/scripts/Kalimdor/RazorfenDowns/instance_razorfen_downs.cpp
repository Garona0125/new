/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
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
#include "razorfen_downs.h"

#define MAX_ENCOUNTER  1

class instance_razorfen_downs : public InstanceMapScript
{
public:
    instance_razorfen_downs() : InstanceMapScript("instance_razorfen_downs", 129) { }

    struct instance_razorfen_downs_InstanceMapScript : public InstanceScript
    {
        instance_razorfen_downs_InstanceMapScript(Map* map) : InstanceScript(map){}

        uint64 GongGUID;

        uint32 Encounter[MAX_ENCOUNTER];

        uint16 GongWaves;

        std::string str_data;

        void Initialize()
        {
            GongGUID = 0;

            GongWaves = 0;

            memset(&Encounter, 0, sizeof(Encounter));
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;

            saveStream << "T C " << Encounter[0]
                << ' ' << GongWaves;

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
            uint16 data0, data1;

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> data0 >> data1;

            if (dataHead1 == 'T' && dataHead2 == 'C')
            {
                Encounter[0] = data0;

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (Encounter[i] == IN_PROGRESS)
                        Encounter[i] = NOT_STARTED;

                GongWaves = data1;
            }
			else OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case GO_GONG:
                    GongGUID = go->GetGUID();
                    if (Encounter[0] == DONE)
                        go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    break;
                default:
                    break;
            }
        }

        void SetData(uint32 type, uint32 data)
        {
            if (type == DATA_GONG_WAVES)
            {
                GongWaves = data;

                switch (GongWaves)
                {
                    case 9:
                    case 14:
                        if (GameObject* go = instance->GetGameObject(GongGUID))
                            go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        break;
                    case 1:
                    case 10:
                    case 16:
                    {
                        GameObject* go = instance->GetGameObject(GongGUID);

                        if (!go)
                            return;

                        go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);

                        uint32 uiCreature = 0;
                        uint8 uiSummonTimes = 0;

                        switch (GongWaves)
                        {
                            case 1:
                                uiCreature = CREATURE_TOMB_FIEND;
                                uiSummonTimes = 7;
                                break;
                            case 10:
                                uiCreature = CREATURE_TOMB_REAVER;
                                uiSummonTimes = 3;
                                break;
                            case 16:
                                uiCreature = CREATURE_TUTEN_KASH;
                                break;
                            default:
                                break;
                        }

                        if (Creature* creature = go->SummonCreature(uiCreature, 2502.635f, 844.140f, 46.896f, 0.633f))
                        {
                            if (GongWaves == 10 || GongWaves == 1)
                            {
                                for (uint8 i = 0; i < uiSummonTimes; ++i)
                                {
                                    if (Creature* summon = go->SummonCreature(uiCreature, 2502.635f + float(irand(-5, 5)), 844.140f + float(irand(-5, 5)), 46.896f, 0.633f))
                                        summon->GetMotionMaster()->MovePoint(0, 2533.479f + float(irand(-5, 5)), 870.020f + float(irand(-5, 5)), 47.678f);
                                }
                            }
                            creature->GetMotionMaster()->MovePoint(0, 2533.479f + float(irand(-5, 5)), 870.020f + float(irand(-5, 5)), 47.678f);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            if (type == BOSS_TUTEN_KASH)
            {
                Encounter[0] = data;

                if (data == DONE)
                    SaveToDB();
            }
        }

        uint32 GetData(uint32 type)
        {
            switch (type)
            {
                case DATA_GONG_WAVES:
                    return GongWaves;
            }

            return 0;
        }

        uint64 GetData64(uint32 type)
        {
            switch (type)
            {
                case DATA_GONG:
					return GongGUID;
            }
            return 0;
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_razorfen_downs_InstanceMapScript(map);
    }
};

void AddSC_instance_razorfen_downs()
{
    new instance_razorfen_downs();
}