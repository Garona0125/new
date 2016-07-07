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

enum Spells
{
    SPELL_FIERYBURST = 13900,
    SPELL_WARSTOMP   = 24375,
};

enum eEnums
{
    DATA_THRONE_DOOR = 24,
};

class boss_magmus : public CreatureScript
{
public:
    boss_magmus() : CreatureScript("boss_magmus") { }

    struct boss_magmusAI : public QuantumBasicAI
    {
        boss_magmusAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 FieryBurstTimer;
        uint32 WarStompTimer;

        void Reset()
        {
            FieryBurstTimer = 5000;
            WarStompTimer = 0;
        }

        void EnterToBattle(Unit* /*who*/) {}

		void JustDied(Unit* who)
        {
            if (InstanceScript* instance = who->GetInstanceScript())
                instance->HandleGameObject(instance->GetData64(DATA_THRONE_DOOR), true);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (FieryBurstTimer <= diff)
            {
                DoCastVictim(SPELL_FIERYBURST);
                FieryBurstTimer = 6000;
            }
			else FieryBurstTimer -= diff;

            if (HealthBelowPct(51))
            {
                if (WarStompTimer <= diff)
                {
                    DoCastVictim(SPELL_WARSTOMP);
                    WarStompTimer = 8000;
                }
				else WarStompTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_magmusAI(creature);
    }
};

void AddSC_boss_magmus()
{
    new boss_magmus();
}