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

enum Timy
{
	SAY_SPAWN           = 0,
	SPELL_RAVENOUSCLAW  = 17470,
};

class boss_timmy_the_cruel : public CreatureScript
{
public:
    boss_timmy_the_cruel() : CreatureScript("boss_timmy_the_cruel") { }

    struct boss_timmy_the_cruelAI : public QuantumBasicAI
    {
        boss_timmy_the_cruelAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 RavenousClawTimer;
        bool HasYelled;

        void Reset()
        {
            RavenousClawTimer = 10000;
            HasYelled = false;
        }

        void EnterToBattle(Unit* /*who*/)
        {
            if (!HasYelled)
            {
                Talk(SAY_SPAWN);
                HasYelled = true;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            //Return since we have no target
            if (!UpdateVictim())
                return;

            //RavenousClaw
            if (RavenousClawTimer <= diff)
            {
                //Cast
                DoCastVictim(SPELL_RAVENOUSCLAW);
                //15 seconds until we should cast this again
                RavenousClawTimer = 15000;
            }
			else RavenousClawTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_timmy_the_cruelAI(creature);
    }
};

void AddSC_boss_timmy_the_cruel()
{
    new boss_timmy_the_cruel();
}