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

enum Ironaya
{
	SAY_AGGRO         = 0,

	SPELL_ARCING_SMASH = 8374,
	SPELL_KNOCK_AWAY   = 10101,
	SPELL_WAR_STOMP    = 11876,
};

class boss_ironaya : public CreatureScript
{
    public:
        boss_ironaya() : CreatureScript("boss_ironaya") {}

        struct boss_ironayaAI : public QuantumBasicAI
        {
            boss_ironayaAI(Creature* creature) : QuantumBasicAI(creature) {}

            uint32 ArcingTimer;

            bool bHasCastedWstomp;
            bool bHasCastedKnockaway;

            void Reset()
            {
                ArcingTimer = 3000;
                bHasCastedKnockaway = false;
                bHasCastedWstomp = false;
            }

            void EnterToBattle(Unit* /*who*/)
            {
				Talk(SAY_AGGRO);
            }

            void UpdateAI(const uint32 diff)
            {
                //Return since we have no target
                if (!UpdateVictim())
                    return;

                //If we are <50% hp do knockaway ONCE
                if (!bHasCastedKnockaway && HealthBelowPct(HEALTH_PERCENT_50))
                {
                    DoCastVictim(SPELL_KNOCK_AWAY, true);

                    // current aggro target is knocked away pick new target
                    Unit* target = SelectTarget(TARGET_TOP_AGGRO, 0);

                    if (!target || target == me->GetVictim())
                        target = SelectTarget(TARGET_TOP_AGGRO, 1);

                    if (target)
                        me->TauntApply(target);

                    //Shouldn't cast this agian
                    bHasCastedKnockaway = true;
                }

                //ArcingTimer
                if (ArcingTimer <= diff)
                {
                    DoCast(me, SPELL_ARCING_SMASH);
                    ArcingTimer = 13000;
                }
				else ArcingTimer -= diff;

                if (!bHasCastedWstomp && HealthBelowPct(HEALTH_PERCENT_25))
                {
                    DoCast(me, SPELL_WAR_STOMP);
                    bHasCastedWstomp = true;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_ironayaAI(creature);
        }
};

void AddSC_boss_ironaya()
{
    new boss_ironaya();
}