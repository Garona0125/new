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
#include "hellfire_ramparts.h"

class instance_hellfire_ramparts : public InstanceMapScript
{
public:
	instance_hellfire_ramparts(): InstanceMapScript("instance_hellfire_ramparts", 543) { }

	struct instance_hellfire_ramparts_InstanceMapScript : public InstanceScript
	{
		instance_hellfire_ramparts_InstanceMapScript(Map* map) : InstanceScript(map) {}
		
		void Initialize(){}
	};

	InstanceScript* GetInstanceScript(InstanceMap* map) const
	{
		return new instance_hellfire_ramparts_InstanceMapScript(map);
	}
};

void AddSC_instance_ramparts()
{
    new instance_hellfire_ramparts;
}