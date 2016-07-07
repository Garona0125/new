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

enum Emotes
{
    EMOTE_FRENZY           = 0,
};

enum Spells
{
    SPELL_SHADOW_FLAME = 22539,
    SPELL_WING_BUFFET  = 23339,
    SPELL_FRENZY       = 23342,
};

class boss_flamegor : public CreatureScript
{
public:
    boss_flamegor() : CreatureScript("boss_flamegor") { }

    struct boss_flamegorAI : public QuantumBasicAI
    {
        boss_flamegorAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ShadowFlameTimer;
        uint32 WingBuffetTimer;
        uint32 FrenzyTimer;

        void Reset()
        {
            ShadowFlameTimer = 21000;
            WingBuffetTimer = 35000;
            FrenzyTimer = 10000;
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoZoneInCombat();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (ShadowFlameTimer <= diff)
            {
                DoCastVictim(SPELL_SHADOW_FLAME);
                ShadowFlameTimer = urand(15000, 22000);
            }
			else ShadowFlameTimer -= diff;

            if (WingBuffetTimer <= diff)
            {
                DoCastVictim(SPELL_WING_BUFFET);
                if (DoGetThreat(me->GetVictim()))
                    DoModifyThreatPercent(me->GetVictim(), -75);

                WingBuffetTimer = 25000;
            }
			else WingBuffetTimer -= diff;

            if (FrenzyTimer <= diff)
            {
                Talk(EMOTE_FRENZY);
                DoCast(me, SPELL_FRENZY);
                FrenzyTimer = urand(8000, 10000);
            }
			else FrenzyTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_flamegorAI(creature);
    }
};

void AddSC_boss_flamegor()
{
    new boss_flamegor();
}