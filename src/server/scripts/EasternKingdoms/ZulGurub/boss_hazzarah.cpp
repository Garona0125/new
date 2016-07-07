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
#include "zulgurub.h"

enum Spells
{
	SPELL_MANA_BURN = 26046,
	SPELL_SLEEP     = 24664,
};

class boss_hazzarah : public CreatureScript
{
    public:
        boss_hazzarah() : CreatureScript("boss_hazzarah") {}

        struct boss_hazzarahAI : public QuantumBasicAI
        {
            boss_hazzarahAI(Creature* creature) : QuantumBasicAI(creature) {}

            uint32 ManaBurnTimer;
            uint32 SleepTimer;
            uint32 IllusionsTimer;

            void Reset()
            {
                ManaBurnTimer = urand(4000, 10000);
                SleepTimer = urand(10000, 18000);
                IllusionsTimer = urand(10000, 18000);
            }

            void EnterToBattle(Unit* /*who*/){}

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                //ManaBurnTimer
                if (ManaBurnTimer <= diff)
                {
                    DoCastVictim(SPELL_MANA_BURN);
                    ManaBurnTimer = urand(8000, 16000);
                }
				else ManaBurnTimer -= diff;

                //SleepTimer
                if (SleepTimer <= diff)
                {
                    DoCastVictim(SPELL_SLEEP);
                    SleepTimer = urand(12000, 20000);
                }
				else SleepTimer -= diff;

                //IllusionsTimer
                if (IllusionsTimer <= diff)
                {
                    Unit* target = NULL;
                    for (uint8 i = 0; i < 3; ++i)
                    {
                        target = SelectTarget(TARGET_RANDOM, 0);
                        if (!target)
                            return;

                        Creature* Illusion = me->SummonCreature(15163, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 30000);
                        if (Illusion)
                            Illusion->AI()->AttackStart(target);
                    }

                    IllusionsTimer = urand(15000, 25000);
                }
				else IllusionsTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_hazzarahAI(creature);
        }
};

void AddSC_boss_hazzarah()
{
    new boss_hazzarah();
}