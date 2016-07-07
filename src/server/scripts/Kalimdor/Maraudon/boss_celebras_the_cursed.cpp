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
    SPELL_WRATH            = 21807,
    SPELL_ENTANGLING_ROOTS = 12747,
    SPELL_CORRUPT_FORCES   = 21968
};

class boss_celebras_the_cursed : public CreatureScript
{
public:
    boss_celebras_the_cursed() : CreatureScript("boss_celebras_the_cursed") { }

    struct boss_celebras_the_cursedAI : public QuantumBasicAI
    {
        boss_celebras_the_cursedAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 WrathTimer;
        uint32 EntanglingRootsTimer;
        uint32 CorruptForcesTimer;

        void Reset()
        {
            WrathTimer = 8000;
            EntanglingRootsTimer = 2000;
            CorruptForcesTimer = 30000;
        }

        void EnterToBattle(Unit* /*who*/) { }

        void JustDied(Unit* /*Killer*/)
        {
            me->SummonCreature(13716, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 600000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (WrathTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_WRATH);
					WrathTimer = 8000;
				}
            } 
            else WrathTimer -= diff;

            if (EntanglingRootsTimer <= diff)
            {
                DoCastVictim(SPELL_ENTANGLING_ROOTS);
                EntanglingRootsTimer = 20000;
            } 
            else EntanglingRootsTimer -= diff;

            if (CorruptForcesTimer <= diff)
            {
                me->InterruptNonMeleeSpells(false);
                DoCast(me, SPELL_CORRUPT_FORCES);
                CorruptForcesTimer = 20000;
            } 
            else CorruptForcesTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_celebras_the_cursedAI(creature);
    }
};

void AddSC_boss_celebras_the_cursed()
{
    new boss_celebras_the_cursed();
}