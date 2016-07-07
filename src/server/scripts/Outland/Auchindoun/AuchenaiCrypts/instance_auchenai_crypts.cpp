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
#include "auchenai_crypts.h"

class instance_auchenai_crypts : public InstanceMapScript
{
public:
	instance_auchenai_crypts() : InstanceMapScript("instance_auchenai_crypts", 558) { }

	struct instance_auchenai_crypts_InstanceMapScript : public InstanceScript
	{
		instance_auchenai_crypts_InstanceMapScript(Map* map) : InstanceScript(map){}
	};

	InstanceScript* GetInstanceScript(InstanceMap* map) const
	{
		return new instance_auchenai_crypts_InstanceMapScript(map);
	}
};

void AddSC_instance_auchenai_crypts()
{
    new instance_auchenai_crypts();
}