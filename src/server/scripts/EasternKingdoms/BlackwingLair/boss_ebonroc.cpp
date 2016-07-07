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

/* ScriptData
SDName: Boss_Ebonroc
SD%Complete: 50
SDComment: Shadow of Ebonroc needs core support
SDCategory: Blackwing Lair
EndScriptData */

#include "ScriptMgr.h"
#include "QuantumCreature.h"

enum Spells
{
	SPELL_SHADOW_FLAME      = 22539,
	SPELL_WING_BUFFET       = 23339,
	SPELL_SHADOW_OF_EBONROC = 23340,
	SPELL_HEAL              = 41386,
	SPELL_THRASH            = 3417,
};

class boss_ebonroc : public CreatureScript
{
public:
    boss_ebonroc() : CreatureScript("boss_ebonroc") { }

    struct boss_ebonrocAI : public QuantumBasicAI
    {
        boss_ebonrocAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ShadowFlameTimer;
        uint32 WingBuffetTimer;
        uint32 ShadowOfEbonrocTimer;
        uint32 HealTimer;
		uint32 ThrashTimer;

        void Reset()
        {
            ShadowFlameTimer = 15000;
            WingBuffetTimer = 30000;
            ShadowOfEbonrocTimer = 45000;
            HealTimer = 1000;
			ThrashTimer = 10000;
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoZoneInCombat();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            //Shadowflame Timer
            if (ShadowFlameTimer <= diff)
            {
                DoCastVictim(SPELL_SHADOW_FLAME);
                ShadowFlameTimer = urand(12000, 15000);
            }
			else ShadowFlameTimer -= diff;

			//Thrash Timer
			if (ThrashTimer <= diff)
			{
				DoCast(me, SPELL_THRASH);
				ThrashTimer = urand(10000, 15000);
			}
			else ThrashTimer -= diff;

            //Wing Buffet Timer
            if (WingBuffetTimer <= diff)
            {
                DoCastVictim(SPELL_WING_BUFFET);
                WingBuffetTimer = 25000;
            }
			else WingBuffetTimer -= diff;

            //Shadow of Ebonroc Timer
            if (ShadowOfEbonrocTimer <= diff)
            {
                DoCastVictim(SPELL_SHADOW_OF_EBONROC);
                ShadowOfEbonrocTimer = urand(25000, 350000);
            }
			else ShadowOfEbonrocTimer -= diff;

            if (me->GetVictim()->HasAura(SPELL_SHADOW_OF_EBONROC))
            {
                if (HealTimer <= diff)
                {
                    DoCast(me, SPELL_HEAL);
                    HealTimer = urand(1000, 3000);
                }
				else HealTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_ebonrocAI(creature);
    }
};

void AddSC_boss_ebonroc()
{
    new boss_ebonroc();
}