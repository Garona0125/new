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
#include "scarlet_monastery.h"

enum Spells
{
	SPELL_CALL_OF_THE_GRAVE = 17831,
	SPELL_TERRIFY           = 7399,
	SPELL_SOUL_SIPHON       = 7290,
};

class boss_azshir_the_sleepless : public CreatureScript
{
public:
    boss_azshir_the_sleepless() : CreatureScript("boss_azshir_the_sleepless") { }

    struct boss_azshir_the_sleeplessAI : public QuantumBasicAI
    {
        boss_azshir_the_sleeplessAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 SoulSiphonTimer;
        uint32 CallOftheGraveTimer;
        uint32 TerrifyTimer;

        void Reset()
        {
            SoulSiphonTimer = 1;
            CallOftheGraveTimer = 30000;
            TerrifyTimer = 20000;
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (!HealthAbovePct(HEALTH_PERCENT_50))
            {
                if (SoulSiphonTimer <= diff)
                {
					DoCastVictim(SPELL_SOUL_SIPHON);
					return;

                    //SoulSiphonTimer = 20000;
                }
				else SoulSiphonTimer -= diff;
            }

            if (CallOftheGraveTimer <= diff)
            {
                DoCastVictim(SPELL_CALL_OF_THE_GRAVE);
                CallOftheGraveTimer = 30000;
            }
			else CallOftheGraveTimer -= diff;

            if (TerrifyTimer <= diff)
            {
                DoCastVictim(SPELL_TERRIFY);
                TerrifyTimer = 20000;
            }
			else TerrifyTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_azshir_the_sleeplessAI(creature);
    }
};

void AddSC_boss_azshir_the_sleepless()
{
    new boss_azshir_the_sleepless();
}