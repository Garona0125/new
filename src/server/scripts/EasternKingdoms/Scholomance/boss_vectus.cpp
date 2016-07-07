/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
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

enum Vectus
{
    EMOTE_FRENZY_KILL  = 0,

    SPELL_FLAMESTRIKE = 18399,
    SPELL_BLAST_WAVE  = 16046,
    SPELL_FIRE_SHIELD = 19626,
    SPELL_FRENZY      = 8269,
};

class boss_vectus : public CreatureScript
{
public:
    boss_vectus() : CreatureScript("boss_vectus") { }

    struct boss_vectusAI : public QuantumBasicAI
    {
        boss_vectusAI(Creature* creature): QuantumBasicAI(creature) {}

        uint32 FireShieldTimer;
        uint32 BlastWaveTimer;
        uint32 FrenzyTimer;

        void Reset()
        {
            FireShieldTimer = 2000;
            BlastWaveTimer = 14000;
            FrenzyTimer = 0;
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            if (FireShieldTimer <= diff)
            {
                DoCast(me, SPELL_FIRE_SHIELD);
                FireShieldTimer = 90000;
            }
            else FireShieldTimer -= diff;

            if (BlastWaveTimer <= diff)
            {
                DoCastAOE(SPELL_BLAST_WAVE);
                BlastWaveTimer = 12000;
            }
            else BlastWaveTimer -= diff;

            if (HealthBelowPct(HEALTH_PERCENT_25))
            {
                if (FrenzyTimer <= diff)
                {
                    DoCast(me, SPELL_FRENZY);
                    Talk(EMOTE_FRENZY_KILL);

                    FrenzyTimer = 24000;
                }
                else FrenzyTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_vectusAI(creature);
    }
};

void AddSC_boss_vectus()
{
    new boss_vectus();
}