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
#include "the_underbog.h"

class instance_the_underbog : public InstanceMapScript
{
public:
    instance_the_underbog() : InstanceMapScript("instance_the_underbog", 546) { }

    struct instance_the_underbog_InstanceMapScript : public InstanceScript
    {
        instance_the_underbog_InstanceMapScript(Map* map) : InstanceScript(map) {}
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_the_underbog_InstanceMapScript(map);
    }
};

void AddSC_instance_the_underbog()
{
    new instance_the_underbog();
}