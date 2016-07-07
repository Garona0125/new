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
    SPELL_GROUNDTREMOR = 6524,
    SPELL_FRENZY       = 28371,
	EMOTE_FRENZY_KILL  = 0,
};

class boss_grizzle : public CreatureScript
{
public:
    boss_grizzle() : CreatureScript("boss_grizzle") { }

    struct boss_grizzleAI : public QuantumBasicAI
    {
        boss_grizzleAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 GroundTremorTimer;
        uint32 FrenzyTimer;

        void Reset()
        {
            GroundTremorTimer = 12000;
            FrenzyTimer = 0;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (GroundTremorTimer <= diff)
            {
                DoCastVictim(SPELL_GROUNDTREMOR);
                GroundTremorTimer = 8000;
            }
			else GroundTremorTimer -= diff;

            if (HealthBelowPct(HEALTH_PERCENT_50))
            {
                if (FrenzyTimer <= diff)
                {
                    DoCast(me, SPELL_FRENZY);
					Talk(EMOTE_FRENZY_KILL);

                    FrenzyTimer = 15000;
                }
				else FrenzyTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_grizzleAI(creature);
    }
};

void AddSC_boss_grizzle()
{
    new boss_grizzle();
}