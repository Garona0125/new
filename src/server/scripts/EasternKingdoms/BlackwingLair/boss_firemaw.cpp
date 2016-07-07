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
	SPELL_SHADOW_FLAME = 22539,
	SPELL_WING_BUFFET  = 23339,
	SPELL_FLAME_BUFFET = 23341,
};

class boss_firemaw : public CreatureScript
{
public:
    boss_firemaw() : CreatureScript("boss_firemaw") { }

    struct boss_firemawAI : public QuantumBasicAI
    {
        boss_firemawAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ShadowFlameTimer;
        uint32 WingBuffetTimer;
        uint32 FlameBuffetTimer;

        void Reset()
        {
            ShadowFlameTimer = 30000;                          //These times are probably wrong
            WingBuffetTimer = 24000;
            FlameBuffetTimer = 5000;
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoZoneInCombat();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (ShadowFlameTimer <= diff)
            {
                DoCastVictim(SPELL_SHADOW_FLAME);
                ShadowFlameTimer = urand(15000, 18000);
            }
			else ShadowFlameTimer -= diff;

            if (WingBuffetTimer <= diff)
            {
                DoCastVictim(SPELL_WING_BUFFET);
                if (DoGetThreat(me->GetVictim()))
                    DoModifyThreatPercent(me->GetVictim(), -75);

                WingBuffetTimer = 25000;
            }
			else WingBuffetTimer -= diff;

            if (FlameBuffetTimer <= diff)
            {
                DoCastVictim(SPELL_FLAME_BUFFET);
                FlameBuffetTimer = 5000;
            }
			else FlameBuffetTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_firemawAI(creature);
    }
};

void AddSC_boss_firemaw()
{
    new boss_firemaw();
}