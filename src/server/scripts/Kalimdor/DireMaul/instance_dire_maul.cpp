/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
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

class instance_dire_maul : public InstanceMapScript
{
public:
    instance_dire_maul() : InstanceMapScript("instance_dire_maul", 429) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_dire_maul_InstanceMapScript(map);
    }

    struct instance_dire_maul_InstanceMapScript : public InstanceScript
    {
        instance_dire_maul_InstanceMapScript(Map* map) : InstanceScript(map) {}
    };
};

void AddSC_instance_dire_maul()
{
    new instance_dire_maul();
}