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

enum Say
{
    SAY_AGGRO               = 0,
    SAY_LEASH               = 1,
};

enum Spells
{
    SPELL_CLEAVE            = 26350,
    SPELL_BLASTWAVE         = 23331,
    SPELL_MORTALSTRIKE      = 24573,
    SPELL_KNOCKBACK         = 25778,
};

class boss_broodlord : public CreatureScript
{
public:
    boss_broodlord() : CreatureScript("boss_broodlord") { }

    struct boss_broodlordAI : public QuantumBasicAI
    {
        boss_broodlordAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CleaveTimer;
        uint32 BlastWaveTimer;
        uint32 MortalStrikeTimer;
        uint32 KnockBackTimer;

        void Reset()
        {
            CleaveTimer = 8000;
            BlastWaveTimer = 12000;
            MortalStrikeTimer = 20000;
            KnockBackTimer = 30000;
        }

        void EnterToBattle(Unit* /*who*/)
        {
            Talk(SAY_AGGRO);
            DoZoneInCombat();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = 7000;
            }
			else CleaveTimer -= diff;

            if (BlastWaveTimer <= diff)
            {
                DoCastVictim(SPELL_BLASTWAVE);
                BlastWaveTimer = urand(8000, 16000);
            }
			else BlastWaveTimer -= diff;

            if (MortalStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_MORTALSTRIKE);
                MortalStrikeTimer = urand(25000, 35000);
            }
			else MortalStrikeTimer -= diff;

            if (KnockBackTimer <= diff)
            {
                DoCastVictim(SPELL_KNOCKBACK);

                if (DoGetThreat(me->GetVictim()))
                    DoModifyThreatPercent(me->GetVictim(), -50);

                KnockBackTimer = urand(15000, 30000);
            }
			else KnockBackTimer -= diff;

            if (EnterEvadeIfOutOfCombatArea(diff))
				Talk(SAY_LEASH);

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_broodlordAI(creature);
    }
};

void AddSC_boss_broodlord()
{
    new boss_broodlord();
}