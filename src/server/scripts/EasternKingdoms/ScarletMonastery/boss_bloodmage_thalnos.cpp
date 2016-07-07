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

enum Texts
{
	SAY_AGGRO               = 0,
	SAY_HEALTH              = 1,
	SAY_KILL                = 2,
};

enum Spells
{
    SPELL_FLAME_SHOCK = 8053,
    SPELL_SHADOW_BOLT = 1106,
    SPELL_FLAME_SPIKE = 8814,
    SPELL_FIRE_NOVA   = 16079,
};

class boss_bloodmage_thalnos : public CreatureScript
{
public:
    boss_bloodmage_thalnos() : CreatureScript("boss_bloodmage_thalnos") { }

    struct boss_bloodmage_thalnosAI : public QuantumBasicAI
    {
        boss_bloodmage_thalnosAI(Creature* creature) : QuantumBasicAI(creature) {}

        bool HpYell;

        uint32 ShadowBoltTimer;
		uint32 FlameShockTimer;
        uint32 FlameSpikeTimer;
        uint32 FireNovaTimer;

        void Reset()
        {
            HpYell = false;

			ShadowBoltTimer = 0.5*IN_MILLISECONDS;
            FlameShockTimer = 4*IN_MILLISECONDS;
            FlameSpikeTimer = 5*IN_MILLISECONDS;
            FireNovaTimer = 7*IN_MILLISECONDS;
        }

        void EnterToBattle(Unit* /*who*/)
        {
			Talk(SAY_AGGRO);
        }

        void KilledUnit(Unit* /*victim*/)
        {
			Talk(SAY_KILL);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (HealthBelowPct(HEALTH_PERCENT_35) && !HpYell)
            {
                Talk(SAY_HEALTH);
                HpYell = true;
            }
			
			if (ShadowBoltTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target , SPELL_SHADOW_BOLT);
					ShadowBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
            }
			else ShadowBoltTimer -= diff;

            if (FlameShockTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FLAME_SHOCK);
					FlameShockTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
            }
			else FlameShockTimer -= diff;

            if (FlameSpikeTimer <= diff)
            {
                DoCastAOE(SPELL_FLAME_SPIKE);
                FlameSpikeTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
            }
			else FlameSpikeTimer -= diff;

            if (FireNovaTimer <= diff)
            {
                DoCastAOE(SPELL_FIRE_NOVA);
                FireNovaTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
            }
			else FireNovaTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_bloodmage_thalnosAI(creature);
    }
};

void AddSC_boss_bloodmage_thalnos()
{
    new boss_bloodmage_thalnos();
}