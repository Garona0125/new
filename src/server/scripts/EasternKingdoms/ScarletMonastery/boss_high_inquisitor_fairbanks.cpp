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
    SPELL_CURSE_OF_BLOOD    = 8282,
    SPELL_DISPEL_MAGIC      = 15090,
    SPELL_FEAR              = 12096,
    SPELL_HEAL              = 12039,
    SPELL_POWER_WORD_SHIELD = 11647,
    SPELL_SLEEP             = 8399,
};

class boss_high_inquisitor_fairbanks : public CreatureScript
{
public:
    boss_high_inquisitor_fairbanks() : CreatureScript("boss_high_inquisitor_fairbanks") { }

    struct boss_high_inquisitor_fairbanksAI : public QuantumBasicAI
    {
        boss_high_inquisitor_fairbanksAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CurseOfBloodTimer;
        uint32 DispelMagicTimer;
        uint32 FearTimer;
        uint32 HealTimer;
        uint32 SleepTimer;
        uint32 DispelTimer;

        bool PowerWordShield;

        void Reset()
        {
            CurseOfBloodTimer = 10000;
            DispelMagicTimer = 30000;
            FearTimer = 40000;
            HealTimer = 30000;
            SleepTimer = 30000;
            DispelTimer = 20000;
            PowerWordShield = false;
			me->SetStandState(UNIT_STAND_STATE_DEAD);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_SHOW_HEALTH);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
        {
			me->SetStandState(UNIT_STAND_STATE_STAND);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (!HealthAbovePct(HEALTH_PERCENT_25) && !me->IsNonMeleeSpellCasted(false) && HealTimer <= diff)
            {
                DoCast(me, SPELL_HEAL);
                HealTimer = 30000;
            }
			else HealTimer -= diff;

            if (FearTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
                    DoCast(target, SPELL_FEAR);
					FearTimer = 40000;
				}
            }
			else FearTimer -= diff;

            if (SleepTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_TOP_AGGRO, 0))
				{
                    DoCast(target, SPELL_SLEEP);
					SleepTimer = 30000;
				}
            }
			else SleepTimer -= diff;

            if (!PowerWordShield && !HealthAbovePct(25))
            {
                DoCast(me, SPELL_POWER_WORD_SHIELD);
                PowerWordShield = true;
            }

            if (DispelTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_DISPEL_MAGIC);
					DispelMagicTimer = 30000;
				}
            }
			else DispelMagicTimer -= diff;

            if (CurseOfBloodTimer <= diff)
            {
                DoCastVictim(SPELL_CURSE_OF_BLOOD);
                CurseOfBloodTimer = 25000;
            }
			else CurseOfBloodTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_high_inquisitor_fairbanksAI (creature);
    }
};

void AddSC_boss_high_inquisitor_fairbanks()
{
    new boss_high_inquisitor_fairbanks();
}