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

enum Misc
{
	SAY_AGGRO                = 0,
    SPELL_SUMMONSCARLETHOUND = 17164,
    SPELL_BLOODLUST          = 6742,
};

class boss_houndmaster_loksey : public CreatureScript
{
public:
    boss_houndmaster_loksey() : CreatureScript("boss_houndmaster_loksey") { }

    struct boss_houndmaster_lokseyAI : public QuantumBasicAI
    {
        boss_houndmaster_lokseyAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 BloodLustTimer;

        void Reset()
        {
            BloodLustTimer = 20000;
        }

        void EnterToBattle(Unit* /*who*/)
        {
			Talk(SAY_AGGRO);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (BloodLustTimer <= diff)
            {
                DoCast(me, SPELL_BLOODLUST);
                BloodLustTimer = 20000;
            }
			else BloodLustTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_houndmaster_lokseyAI(creature);
    }
};

void AddSC_boss_houndmaster_loksey()
{
    new boss_houndmaster_loksey();
}