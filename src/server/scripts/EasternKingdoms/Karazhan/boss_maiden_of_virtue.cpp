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
#include "SpellScript.h"

enum Texts
{
	SAY_AGGRO               = 0,
	SAY_SLAY                = 1,
	SAY_REPENTANCE          = 2,
	SAY_DEATH               = 3,
};

enum Spells
{
	SPELL_REPENTANCE       = 29511,
	SPELL_HOLY_GROUND      = 29512,
	SPELL_HOLY_FIRE        = 29522,
	SPELL_HOLY_WRATH       = 32445,
	SPELL_BERSERK          = 26662,
};

class boss_maiden_of_virtue : public CreatureScript
{
public:
    boss_maiden_of_virtue() : CreatureScript("boss_maiden_of_virtue") { }

    struct boss_maiden_of_virtueAI : public QuantumBasicAI
    {
        boss_maiden_of_virtueAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 HolygroundTimer;
		uint32 HolywrathTimer;
		uint32 HolyfireTimer;
		uint32 RepentanceTimer;
        uint32 EnrageTimer;

        bool Enraged;

        void Reset()
        {
            HolygroundTimer = 3000;
			HolywrathTimer = 5000;
			HolyfireTimer = 7000;
			RepentanceTimer = 10000;
            EnrageTimer = 600000;
            Enraged = false;
        }

		void EnterToBattle(Unit* /*who*/)
        {
            Talk(SAY_AGGRO);
        }

        void KilledUnit(Unit* /*victim*/)
        {
			Talk(SAY_SLAY);
        }

        void JustDied(Unit* /*killer*/)
        {
			Talk(SAY_DEATH);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HolygroundTimer <= diff)
            {
                DoCast(me, SPELL_HOLY_GROUND, true);
                HolygroundTimer = 3000;
            }
			else HolygroundTimer -= diff;

			if (HolywrathTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
                    DoCast(target, SPELL_HOLY_WRATH);
					HolywrathTimer = urand(5000, 6000);
				}
            }
			else HolywrathTimer -= diff;

			if (HolyfireTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
                    DoCast(target, SPELL_HOLY_FIRE);
                    HolyfireTimer = urand(8000, 9000);
				}
            }
			else HolyfireTimer -= diff;

			if (RepentanceTimer <= diff)
            {
                DoCastVictim(SPELL_REPENTANCE);
                Talk(SAY_REPENTANCE);

                RepentanceTimer = urand(11000, 12000);
            }
			else RepentanceTimer -= diff;

            if (EnrageTimer < diff && !Enraged)
            {
                DoCast(me, SPELL_BERSERK, true);
                Enraged = true;
            }
			else EnrageTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_maiden_of_virtueAI (creature);
    }
};

void AddSC_boss_maiden_of_virtue()
{
    new boss_maiden_of_virtue();
}