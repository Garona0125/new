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
    SPELL_WHIRLWIND    = 15589,
    SPELL_MORTALSTRIKE = 24573,
};

class boss_gorosh_the_dervish : public CreatureScript
{
public:
    boss_gorosh_the_dervish() : CreatureScript("boss_gorosh_the_dervish") { }

    struct boss_gorosh_the_dervishAI : public QuantumBasicAI
    {
        boss_gorosh_the_dervishAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 WhirlWindTimer;
        uint32 MortalStrikeTimer;

        void Reset()
        {
            WhirlWindTimer = 12000;
            MortalStrikeTimer = 22000;
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (WhirlWindTimer <= diff)
            {
                DoCast(me, SPELL_WHIRLWIND);
                WhirlWindTimer = 15000;
            }
			else WhirlWindTimer -= diff;

            if (MortalStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_MORTALSTRIKE);
                MortalStrikeTimer = 15000;
            }
			else MortalStrikeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_gorosh_the_dervishAI(creature);
    }
};

void AddSC_boss_gorosh_the_dervish()
{
    new boss_gorosh_the_dervish();
}