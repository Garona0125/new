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
	SPELL_AVARTAR      = 24646,
	SPELL_GROUNDTREMOR = 6524,
};

class boss_grilek : public CreatureScript
{
    public:
        boss_grilek() : CreatureScript("boss_grilek") {}

        struct boss_grilekAI : public QuantumBasicAI
        {
            boss_grilekAI(Creature* creature) : QuantumBasicAI(creature) {}

            uint32 AvartarTimer;
            uint32 GroundTremorTimer;

            void Reset()
            {
                AvartarTimer = urand(15000, 25000);
                GroundTremorTimer = urand(8000, 16000);
            }

            void EnterToBattle(Unit* /*who*/){}

            void UpdateAI(const uint32 diff)
            {
                //Return since we have no target
                if (!UpdateVictim())
                    return;

                //AvartarTimer
                if (AvartarTimer <= diff)
                {
                    DoCast(me, SPELL_AVARTAR);
                    Unit* target = NULL;

                    target = SelectTarget(TARGET_RANDOM, 1);

                    if (DoGetThreat(me->GetVictim()))
                        DoModifyThreatPercent(me->GetVictim(), -50);

                    if (target)
                        AttackStart(target);

                    AvartarTimer = urand(25000, 35000);
                }
				else AvartarTimer -= diff;

                //GroundTremorTimer
                if (GroundTremorTimer <= diff)
                {
                    DoCastVictim(SPELL_GROUNDTREMOR);
                    GroundTremorTimer = urand(12000, 16000);
                }
				else GroundTremorTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_grilekAI(creature);
        }
};

void AddSC_boss_grilek()
{
    new boss_grilek();
}