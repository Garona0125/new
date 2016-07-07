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
#include "mechanar.h"

class instance_mechanar : public InstanceMapScript
{
    public: instance_mechanar() : InstanceMapScript("instance_mechanar", 554) { }

        struct instance_mechanar_InstanceMapScript : public InstanceScript
        {
            instance_mechanar_InstanceMapScript(Map* map) : InstanceScript(map) { }

            uint32 Encounter[MAX_ENCOUNTER];

            void Initialize()
            {
                memset(&Encounter, 0, sizeof(Encounter));
            }

            bool IsEncounterInProgress() const
            {
                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (Encounter[i] == IN_PROGRESS)
                        return true;
				
				return false;
			}

            uint32 GetData(uint32 type)
            {
                switch (type)
                {
                    case DATA_NETHERMANCER_EVENT:
						return Encounter[0];
				}
                return false;
            }

            uint64 GetData64 (uint32 /*identifier*/)
            {
                return 0;
            }

            void SetData(uint32 type, uint32 data)
            {
                switch (type)
                {
                    case DATA_NETHERMANCER_EVENT:
						Encounter[0] = data;
						break;
                }
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_mechanar_InstanceMapScript(map);
        }
};

void AddSC_instance_mechanar()
{
    new instance_mechanar;
}