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
#include "QuantumCreature.h"
#include "sunken_temple.h"

class at_malfurion_stormrage : public AreaTriggerScript
{
public:
    at_malfurion_stormrage() : AreaTriggerScript("at_malfurion_stormrage") { }

    bool OnTrigger(Player* player, const AreaTriggerEntry* /*at*/)
    {
        if (player->GetInstanceScript() && !player->FindCreatureWithDistance(15362, 15))
            player->SummonCreature(15362, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), -1.52f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 100000);
        return false;
    }
};

class go_atalai_statue : public GameObjectScript
{
public:
    go_atalai_statue() : GameObjectScript("go_atalai_statue") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (InstanceScript* instance = player->GetInstanceScript())
            instance->SetData(EVENT_STATE, go->GetEntry());
        return false;
    }
};

void AddSC_sunken_temple()
{
    new at_malfurion_stormrage();
    new go_atalai_statue();
}