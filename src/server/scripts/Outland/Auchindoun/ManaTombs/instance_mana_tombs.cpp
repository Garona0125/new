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
#include "mana_tombs.h"

class instance_mana_tombs : public InstanceMapScript
{
    public:
        instance_mana_tombs() : InstanceMapScript("instance_mana_tombs", 557) { }

        struct instance_mana_tombs_InstanceMapScript : public InstanceScript
        {
            instance_mana_tombs_InstanceMapScript(Map* map) : InstanceScript(map){}
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_mana_tombs_InstanceMapScript(map);
        }
};

void AddSC_instance_mana_tombs()
{
    new instance_mana_tombs();
}