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
    SPELL_FIREBLAST = 15573,
};

class boss_ambassador_flamelash : public CreatureScript
{
public:
    boss_ambassador_flamelash() : CreatureScript("boss_ambassador_flamelash") { }

    struct boss_ambassador_flamelashAI : public QuantumBasicAI
    {
        boss_ambassador_flamelashAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 FireBlastTimer;
        uint32 SpiritTimer;

        void Reset()
        {
            FireBlastTimer = 2000;
            SpiritTimer = 24000;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void SummonSpirits(Unit* victim)
        {
            if (Creature* spirit = DoSpawnCreature(9178, float(irand(-9, 9)), float(irand(-9, 9)), 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60000))
                spirit->AI()->AttackStart(victim);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (FireBlastTimer <= diff)
            {
                DoCastVictim(SPELL_FIREBLAST);
                FireBlastTimer = 7000;
            }
			else FireBlastTimer -= diff;

            if (SpiritTimer <= diff)
            {
                SummonSpirits(me->GetVictim());
                SummonSpirits(me->GetVictim());
                SummonSpirits(me->GetVictim());
                SummonSpirits(me->GetVictim());

                SpiritTimer = 30000;
            }
			else SpiritTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_ambassador_flamelashAI(creature);
    }
};

void AddSC_boss_ambassador_flamelash()
{
    new boss_ambassador_flamelash();
}