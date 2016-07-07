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
    SPELL_SHADOW_WORD_PAIN = 10894,
    SPELL_MANA_BURN        = 10876,
    SPELL_PSYCHIC_SCREAM   = 8122,
    SPELL_SHADOW_SHIELD    = 22417,
};

class boss_high_interrogator_gerstahn : public CreatureScript
{
public:
    boss_high_interrogator_gerstahn() : CreatureScript("boss_high_interrogator_gerstahn") { }

    struct boss_high_interrogator_gerstahnAI : public QuantumBasicAI
    {
        boss_high_interrogator_gerstahnAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ShadowWordPainTimer;
        uint32 ManaBurnTimer;
        uint32 PsychicScreamTimer;
        uint32 ShadowShieldTimer;

        void Reset()
        {
            ShadowWordPainTimer = 4000;
            ManaBurnTimer = 14000;
            PsychicScreamTimer = 32000;
            ShadowShieldTimer = 8000;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
			if (!UpdateVictim())
				return;

            if (ShadowWordPainTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
                    DoCast(target, SPELL_SHADOW_WORD_PAIN);
					ShadowWordPainTimer = 7000;
				}
            }
			else ShadowWordPainTimer -= diff;

            if (ManaBurnTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_MANA_BURN);
						ManaBurnTimer = 10000;
					}
				}
            }
			else ManaBurnTimer -= diff;

            if (PsychicScreamTimer <= diff)
            {
                DoCastVictim(SPELL_PSYCHIC_SCREAM);
                PsychicScreamTimer = 30000;
            }
			else PsychicScreamTimer -= diff;

            if (ShadowShieldTimer <= diff)
            {
                DoCast(me, SPELL_SHADOW_SHIELD);
                ShadowShieldTimer = 25000;
            }
			else ShadowShieldTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_high_interrogator_gerstahnAI(creature);
    }
};

void AddSC_boss_high_interrogator_gerstahn()
{
    new boss_high_interrogator_gerstahn();
}