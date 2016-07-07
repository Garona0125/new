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
#include "QuantumGossip.h"
#include "uldaman.h"

enum Spells
{
    SPELL_CRYSTALLINE_SLUMBER = 3636,
};

class mob_jadespine_basilisk : public CreatureScript
{
    public:
        mob_jadespine_basilisk() : CreatureScript("mob_jadespine_basilisk"){}

        struct mob_jadespine_basiliskAI : public QuantumBasicAI
        {
            mob_jadespine_basiliskAI(Creature* creature) : QuantumBasicAI(creature) {}

            uint32 CslumberTimer;

            void Reset()
            {
                CslumberTimer = 2000;
            }

            void EnterToBattle(Unit* /*who*/){}

            void UpdateAI(const uint32 diff)
            {
                //Return since we have no target
                if (!UpdateVictim())
                    return;

                //CslumberTimer
                if (CslumberTimer <= diff)
                {
                    //Cast
                    DoCastVictim(SPELL_CRYSTALLINE_SLUMBER, true);

                    //Stop attacking target thast asleep and pick new target
                    CslumberTimer = 28000;

                    Unit* target = SelectTarget(TARGET_TOP_AGGRO, 0);

                    if (!target || target == me->GetVictim())
                        target = SelectTarget(TARGET_RANDOM, 0, 100, true);

                    if (target)
                        me->TauntApply(target);

                }
				else CslumberTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_jadespine_basiliskAI(creature);
        }
};

class go_keystone_chamber : public GameObjectScript
{
public:
    go_keystone_chamber() : GameObjectScript("go_keystone_chamber") {}

    bool OnGossipHello(Player* /*player*/, GameObject* go)
    {
        if (InstanceScript* instance = go->GetInstanceScript())
            instance->SetData(DATA_IRONAYA_SEAL, IN_PROGRESS);

        return false;
    }
};

#define QUEST_HIDDEN_CHAMBER 2240

class at_map_chamber : public AreaTriggerScript
{
public:
	at_map_chamber() : AreaTriggerScript("at_map_chamber"){}

	bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
	{
		if (player->GetQuestStatus(QUEST_HIDDEN_CHAMBER) == QUEST_STATUS_INCOMPLETE)
			player->AreaExploredOrEventHappens(QUEST_HIDDEN_CHAMBER);

		return true;
	}
};

void AddSC_uldaman()
{
    new mob_jadespine_basilisk();
    new go_keystone_chamber();
    new at_map_chamber();
}