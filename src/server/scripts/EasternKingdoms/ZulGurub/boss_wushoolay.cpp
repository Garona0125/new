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
	SPELL_LIGHTNING_CLOUD = 25033,
	SPELL_LIGHTNING_WAVE  = 24819,
};

class boss_wushoolay : public CreatureScript
{
    public:
        boss_wushoolay() : CreatureScript("boss_wushoolay") { }

        struct boss_wushoolayAI : public QuantumBasicAI
        {
            boss_wushoolayAI(Creature* creature) : QuantumBasicAI(creature) {}

            uint32 LightningCloudTimer;
            uint32 LightningWaveTimer;

            void Reset()
            {
                LightningCloudTimer = urand(5000, 10000);
                LightningWaveTimer = urand(8000, 16000);
            }

            void EnterToBattle(Unit* /*who*/){}

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (LightningCloudTimer <= diff)
                {
                    DoCastVictim(SPELL_LIGHTNING_CLOUD);
                    LightningCloudTimer = urand(15000, 20000);
                }
				else LightningCloudTimer -= diff;

                if (LightningWaveTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, SPELL_LIGHTNING_WAVE);
						LightningWaveTimer = urand(12000, 16000);
					}
                }
				else LightningWaveTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_wushoolayAI(creature);
        }
};

void AddSC_boss_wushoolay()
{
    new boss_wushoolay();
}