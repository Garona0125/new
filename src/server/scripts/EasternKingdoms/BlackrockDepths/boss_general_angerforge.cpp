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
    SPELL_MIGHTY_BLOW = 14099,
    SPELL_HAMSTRING   = 9080,
    SPELL_CLEAVE      = 20691,
};

class boss_general_angerforge : public CreatureScript
{
public:
    boss_general_angerforge() : CreatureScript("boss_general_angerforge") { }

    struct boss_general_angerforgeAI : public QuantumBasicAI
    {
        boss_general_angerforgeAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 MightyBlowTimer;
        uint32 HamStringTimer;
        uint32 CleaveTimer;
        uint32 AddsTimer;

        bool Medics;

        void Reset()
        {
            MightyBlowTimer = 8000;
            HamStringTimer = 12000;
            CleaveTimer = 16000;
            AddsTimer = 0;

            Medics = false;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void SummonAdds(Unit* victim)
        {
            if (Creature* SummonedAdd = DoSpawnCreature(8901, float(irand(-14, 14)), float(irand(-14, 14)), 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 120000))
                SummonedAdd->AI()->AttackStart(victim);
        }

        void SummonMedics(Unit* victim)
        {
            if (Creature* SummonedMedic = DoSpawnCreature(8894, float(irand(-9, 9)), float(irand(-9, 9)), 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 120000))
                SummonedMedic->AI()->AttackStart(victim);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (MightyBlowTimer <= diff)
            {
                DoCastVictim(SPELL_MIGHTY_BLOW);
                MightyBlowTimer = 18000;
            }
			else MightyBlowTimer -= diff;

            if (HamStringTimer <= diff)
            {
                DoCastVictim(SPELL_HAMSTRING);
                HamStringTimer = 15000;
            }
			else HamStringTimer -= diff;

            if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = 9000;
            }
			else CleaveTimer -= diff;

            if (HealthBelowPct(21))
            {
                if (AddsTimer <= diff)
                {
                    SummonAdds(me->GetVictim());
                    SummonAdds(me->GetVictim());
                    SummonAdds(me->GetVictim());

                    AddsTimer = 25000;
                }
				else AddsTimer -= diff;
            }

            if (!Medics && HealthBelowPct(21))
            {
                SummonMedics(me->GetVictim());
                SummonMedics(me->GetVictim());
                Medics = true;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_general_angerforgeAI(creature);
    }
};

void AddSC_boss_general_angerforge()
{
    new boss_general_angerforge();
}