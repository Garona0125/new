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
	SPELL_LICH_SLAP        = 28873,
	SPELL_FROSTBOLT_VOLLEY = 8398,
	SPELL_MIND_FLAY        = 17313,
	SPELL_FROST_NOVA       = 15531,
};

class boss_scorn : public CreatureScript
{
public:
    boss_scorn() : CreatureScript("boss_scorn") { }

    struct boss_scornAI : public QuantumBasicAI
    {
        boss_scornAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 LichSlapTimer;
        uint32 FrostboltVolleyTimer;
        uint32 MindFlayTimer;
        uint32 FrostNovaTimer;

        void Reset()
        {
            LichSlapTimer = 45000;
            FrostboltVolleyTimer = 30000;
            MindFlayTimer = 30000;
            FrostNovaTimer = 30000;
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (LichSlapTimer <= diff)
            {
                DoCastVictim(SPELL_LICH_SLAP);
                LichSlapTimer = 45000;
            }
			else LichSlapTimer -= diff;

            if (FrostboltVolleyTimer <= diff)
            {
                DoCastAOE(SPELL_FROSTBOLT_VOLLEY);
                FrostboltVolleyTimer = 20000;
            }
			else FrostboltVolleyTimer -= diff;

            if (MindFlayTimer <= diff)
            {
                DoCastVictim(SPELL_MIND_FLAY);
                MindFlayTimer = 20000;
            }
			else MindFlayTimer -= diff;

            if (FrostNovaTimer <= diff)
            {
                DoCastAOE(SPELL_FROST_NOVA);
                FrostNovaTimer = 15000;
            }
			else FrostNovaTimer -= diff;

            DoMeleeAttackIfReady();
		}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_scornAI(creature);
    }
};

void AddSC_boss_scorn()
{
    new boss_scorn();
}