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
#include "shattered_halls.h"

class instance_shattered_halls : public InstanceMapScript
{
public:
    instance_shattered_halls() : InstanceMapScript("instance_shattered_halls", 540) { }

    struct instance_shattered_halls_InstanceMapScript : public InstanceScript
    {
		instance_shattered_halls_InstanceMapScript(Map* map) : InstanceScript(map) {}

        uint32 Encounter[MAX_ENCOUNTER];
        uint64 nethekurseGUID;
        uint64 nethekurseDoorGUID;
        uint64 nethekurseDoor2GUID;
        bool nethekurskilled;

        uint32 TimerState;
        uint32 TimerTick;
        uint32 ExecutionPhase; // 0 1 2 3
        uint32 ExecutionTimer;

        void Initialize()
        {
            memset(&Encounter, 0, sizeof(Encounter));

            nethekurseGUID = 0;
            nethekurseDoorGUID = 0;

            nethekurskilled = 1;

            ExecutionPhase = 3;
            TimerTick = 0;
            TimerState = TIMER_NOT_STARTED;
            ExecutionTimer = 0;
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch(go->GetEntry())
            {
                case GO_GRAND_WARLOCK_CHAMBER_DOOR_ENTRANCE: 
                    nethekurseDoorGUID = go->GetGUID();
                    go->SetGoState((GOState)nethekurskilled);
                    break;
                case GO_GRAND_WARLOCK_CHAMBER_DOOR_EXIT: 
                    nethekurseDoor2GUID = go->GetGUID();
                    go->SetGoState((GOState)nethekurskilled);
                    break;
            }
        }

        void OnCreatureCreate(Creature* creature)
        {
            switch (creature->GetEntry())
            {
                case NPC_GRAND_WARLOCK_NETERCURSE: 
                    nethekurseGUID = creature->GetGUID();
                    break;
            }
        }

        void SetData(uint32 type, uint32 data)
        {
            switch(type)
            {
                case TYPE_NETHEKURSE:
                    Encounter[0] = data;
                    if (data == DONE)
                    {
                        nethekurskilled = 0;
                        HandleGameObject(nethekurseDoor2GUID,nethekurskilled == 0 ? true : false);
                        HandleGameObject(nethekurseDoorGUID,nethekurskilled == 0 ? true : false);
                        SetData(ExecutionPhase,3);
                    }
                    break;
                case TYPE_OMROGG:
                    Encounter[1] = data;
                    break;
                //case TimerState:
                //    TimerState = data;
                //    break;
                case EXECUTION_PHASE:
                    if (TimerState == TIMER_ENDED)
                        return;

                    ExecutionPhase = data;

                    switch(ExecutionPhase)
                    {
                    case 3:
                        ExecutionTimer = 55;
                        TimerTick = 60000;
                        TimerState = TIMER_STARTED;
                        break;
                    case 2:
                        ExecutionTimer = 10;
                        TimerTick = 60000;
                        TimerState = TIMER_STARTED;
                        break;
                    case 1:
                        ExecutionTimer = 15;
                        TimerTick = 60000;
                        TimerState = TIMER_STARTED;
                        break;
                    case 0:
                        ExecutionTimer = 0;
                        TimerState = TIMER_ENDED;
                        break;
                    }
                    break;
            }
            SaveToDB();
        }

        uint32 GetData(uint32 type)
        {
            switch(type)
            {
                case TYPE_NETHEKURSE:
                    return Encounter[0];
                case TYPE_OMROGG:
                    return Encounter[1];
                case TIMER_STATE:
                    return TimerState;
                case EXECUTION_PHASE:
                    return ExecutionPhase;
            }
            return 0;
        }

        uint64 GetData64(uint32 data)
        {
            switch(data)
            {
                case DATA_NETHEKURSE:
                    return nethekurseGUID;
                case DATA_NETHEKURSE_DOOR_ENTRANCE:
                    return nethekurseDoorGUID;
                case DATA_NETHEKURSE_DOOR_EXIT:
                    return nethekurseDoor2GUID;
            }
            return 0;
        }

        void Update(uint32 diff)
        {
            if ( TimerState == TIMER_STARTED)
            {
                if (ExecutionTimer)
                {
                    if (TimerTick <= diff)
                    {
                        ExecutionTimer--;
                        TimerTick = 60000;

                        if (ExecutionTimer <= 0)
                        {
                            if (ExecutionPhase > 0)
                                TimerState = TIMER_NEXT_PHASE;
                            else
                                TimerState = TIMER_ENDED;
                        }

                        SaveToDB();
                    }
                    TimerTick -= diff;
                }
            }
        }

        std::string GetSaveData()
        {
            std::ostringstream ss;
            ss << "S H " << nethekurskilled << " " << TimerState << " " << ExecutionTimer << " " << ExecutionPhase;
            std::string data = ss.str();

            return data.c_str();
        }

        void Load(const char* load)
        {
            if (!load) return;
            std::istringstream ss(load);
            char dataHead1, dataHead2; // S H
            uint32 data1,data2,data3,data4;
            ss >> dataHead1 >> dataHead2 >> data1 >> data2 >> data3 >> data4;
            if (dataHead1 == 'S' && dataHead2 == 'H')
            {
                nethekurskilled = data1;
                TimerState = data2;
                ExecutionTimer = data3;
                ExecutionPhase = data4;

            }
			else sLog->OutErrorConsole("Shattered Halls: corrupted save data.");
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_shattered_halls_InstanceMapScript(map);
    }
};

void AddSC_instance_shattered_halls()
{
    new instance_shattered_halls();
}