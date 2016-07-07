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
    SPELL_KNOCKAWAY = 18670,
    SPELL_TRAMPLE   = 5568,
    SPELL_LANDSLIDE = 21808,
};

class boss_landslide : public CreatureScript
{
public:
    boss_landslide() : CreatureScript("boss_landslide") { }

    struct boss_landslideAI : public QuantumBasicAI
    {
        boss_landslideAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 KnockAwayTimer;
        uint32 TrampleTimer;
        uint32 LandslideTimer;

        void Reset()
        {
            KnockAwayTimer = 8000;
            TrampleTimer = 2000;
            LandslideTimer = 0;
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (KnockAwayTimer <= diff)
            {
                DoCastVictim(SPELL_KNOCKAWAY);
                KnockAwayTimer = 15000;
            } 
            else KnockAwayTimer -= diff;

            if (TrampleTimer <= diff)
            {
                DoCast(me, SPELL_TRAMPLE);
                TrampleTimer = 8000;
            } 
            else TrampleTimer -= diff;

            if (HealthBelowPct(50))
            {
                if (LandslideTimer <= diff)
                {
                    me->InterruptNonMeleeSpells(false);
                    DoCast(me, SPELL_LANDSLIDE);
                    LandslideTimer = 60000;
                } 
                else LandslideTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_landslideAI(creature);
    }
};

void AddSC_boss_landslide()
{
    new boss_landslide();
}