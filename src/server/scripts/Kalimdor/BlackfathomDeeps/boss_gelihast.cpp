/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
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
#include "blackfathom_deeps.h"

enum Spells
{
    SPELL_NET = 6533,
};

class boss_gelihast : public CreatureScript
{
public:
    boss_gelihast() : CreatureScript("boss_gelihast") { }

    struct boss_gelihastAI : public QuantumBasicAI
    {
        boss_gelihastAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 NetTimer;

        InstanceScript* instance;

        void Reset()
        {
            NetTimer = urand(2000, 4000);

			instance->SetData(TYPE_GELIHAST, NOT_STARTED);
        }

        void EnterToBattle(Unit* /*who*/)
        {
			instance->SetData(TYPE_GELIHAST, IN_PROGRESS);
        }

        void JustDied(Unit* /*killer*/)
        {
			instance->SetData(TYPE_GELIHAST, DONE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (NetTimer <= diff)
            {
                DoCastVictim(SPELL_NET);
                NetTimer = urand(4000, 7000);
            }
			else NetTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_gelihastAI (creature);
    }
};

void AddSC_boss_gelihast()
{
    new boss_gelihast();
}