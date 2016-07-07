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

enum Texts
{
	SAY_AGGRO     = 0,
	SAY_SPECIALAE = 1,
};

enum Spells
{
	SPELL_POLYMORPH        = 13323,
    SPELL_SILENCE          = 8988,
    SPELL_ARCANE_EXPLOSION = 9433,
    SPELL_DETONATION       = 9435,
    SPELL_ARCANE_BUBBLE    = 9438,
};

class boss_arcanist_doan : public CreatureScript
{
public:
    boss_arcanist_doan() : CreatureScript("boss_arcanist_doan") { }

    struct boss_arcanist_doanAI : public QuantumBasicAI
    {
        boss_arcanist_doanAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 PolymorphTimer;
        uint32 SilenceTimer;
        uint32 ArcaneExplosionTimer;

        bool CanDetonate;
        bool Shielded;

        void Reset()
        {
            PolymorphTimer = 20000;
            SilenceTimer = 15000;
            ArcaneExplosionTimer = 3000;

            CanDetonate = false;
            Shielded = false;
        }

        void EnterToBattle(Unit* /*who*/)
        {
			Talk(SAY_AGGRO);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (Shielded && CanDetonate)
            {
                DoCast(me, SPELL_DETONATION);
                CanDetonate = false;
            }

            if (me->HasAura(SPELL_ARCANE_BUBBLE))
                return;

            if (HealthBelowPct(HEALTH_PERCENT_50) && !Shielded)
            {
                if (me->IsNonMeleeSpellCasted(false))
                    return;

                Talk(SAY_SPECIALAE);
                DoCast(me, SPELL_ARCANE_BUBBLE);

                CanDetonate = true;
                Shielded = true;
            }

            if (PolymorphTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
                    DoCast(target, SPELL_POLYMORPH);
					PolymorphTimer = 20000;
				}
            }
			else PolymorphTimer -= diff;

            if (SilenceTimer <= diff)
            {
                DoCastVictim(SPELL_SILENCE);
                SilenceTimer = urand(15000, 20000);
            }
			else SilenceTimer -= diff;

            if (ArcaneExplosionTimer <= diff)
            {
                DoCastAOE(SPELL_ARCANE_EXPLOSION);
                ArcaneExplosionTimer = 8000;
            }
			else ArcaneExplosionTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_arcanist_doanAI(creature);
    }
};

void AddSC_boss_arcanist_doan()
{
    new boss_arcanist_doan();
}