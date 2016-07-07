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
    SPELL_DUST_FIELD     = 21909,
    SPELL_BOULDER        = 21832,
    SPELL_THRASH         = 3391,
    SPELL_REPULSIVE_GAZE = 21869,
};

class boss_princess_theradras : public CreatureScript
{
public:
    boss_princess_theradras() : CreatureScript("boss_princess_theradras") { }

    struct boss_ptheradrasAI : public QuantumBasicAI
    {
        boss_ptheradrasAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 DustfieldTimer;
        uint32 BoulderTimer;
        uint32 ThrashTimer;
        uint32 RepulsiveGazeTimer;

        void Reset()
        {
            DustfieldTimer = 8000;
            BoulderTimer = 2000;
            ThrashTimer = 5000;
            RepulsiveGazeTimer = 23000;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void JustDied(Unit* /*killer*/)
        {
            me->SummonCreature(12238, 28.067f, 61.875f, -123.405f, 4.67f, TEMPSUMMON_TIMED_DESPAWN, 604800);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (DustfieldTimer <= diff)
            {
                DoCast(me, SPELL_DUST_FIELD);
                DustfieldTimer = 14000;
            } 
            else DustfieldTimer -= diff;

            if (BoulderTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_BOULDER);
					BoulderTimer = 10000;
				}
            } 
            else BoulderTimer -= diff;

            if (RepulsiveGazeTimer <= diff)
            {
                DoCastVictim(SPELL_REPULSIVE_GAZE);
                RepulsiveGazeTimer = 20000;
            } 
            else RepulsiveGazeTimer -= diff;

            if (ThrashTimer <= diff)
            {
                DoCast(me, SPELL_THRASH);
                ThrashTimer = 18000;
            } 
            else ThrashTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_ptheradrasAI(creature);
    }
};

void AddSC_boss_ptheradras()
{
    new boss_princess_theradras();
}