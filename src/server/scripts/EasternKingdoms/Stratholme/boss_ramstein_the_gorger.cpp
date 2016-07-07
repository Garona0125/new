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
#include "stratholme.h"

enum Ramshtein
{
	SPELL_TRAMPLE       = 5568,
	SPELL_KNOCKOUT      = 17307,

	NPC_MINDLESS_UNDEAD = 11030,
};

class boss_ramstein_the_gorger : public CreatureScript
{
public:
    boss_ramstein_the_gorger() : CreatureScript("boss_ramstein_the_gorger") { }

    struct boss_ramstein_the_gorgerAI : public QuantumBasicAI
    {
        boss_ramstein_the_gorgerAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 TrampleTimer;
        uint32 KnockoutTimer;

        void Reset()
        {
            TrampleTimer = 3000;
            KnockoutTimer = 12000;
        }

        void EnterToBattle(Unit* /*who*/){}

        void JustDied(Unit* /*killer*/)
        {
            for (uint8 i = 0; i < 30; ++i)
            {
                if (Creature* mob = me->SummonCreature(NPC_MINDLESS_UNDEAD, 3969.35f+irand(-10, 10), -3391.87f+irand(-10, 10), 119.11f, 5.91f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 1800000))
                    mob->AI()->AttackStart(me->SelectNearestTarget(500));
            }

			instance->SetData(TYPE_RAMSTEIN, DONE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (TrampleTimer <= diff)
            {
                DoCast(me, SPELL_TRAMPLE);
                TrampleTimer = 7000;
            }
			else TrampleTimer -= diff;

            if (KnockoutTimer <= diff)
            {
                DoCastVictim(SPELL_KNOCKOUT);
                KnockoutTimer = 10000;
            }
			else KnockoutTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new boss_ramstein_the_gorgerAI(creature);
    }
};

void AddSC_boss_ramstein_the_gorger()
{
    new boss_ramstein_the_gorger();
}