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
#include "stratholme.h"

//Spell ID to summon this guy is 24627 "Summon Postmaster Malown"
//He should be spawned along with three other elites once the third postbox has been opened

enum Spells
{
	SAY_KILL               = 0,
	SPELL_WAILINGDEAD      = 7713,
	SPELL_BACKHAND         = 6253,
	SPELL_CURSEOFWEAKNESS  = 8552,
	SPELL_CURSEOFTONGUES   = 12889,
	SPELL_CALLOFTHEGRAVE   = 17831,
};

class boss_postmaster_malown : public CreatureScript
{
public:
    boss_postmaster_malown() : CreatureScript("boss_postmaster_malown") { }

    struct boss_postmaster_malownAI : public QuantumBasicAI
    {
        boss_postmaster_malownAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 WailingDeadTimer;
        uint32 BackhandTimer;
        uint32 CurseOfWeaknessTimer;
        uint32 CurseOfTonguesTimer;
        uint32 CallOfTheGraveTimer;

        bool HasYelled;

        void Reset()
        {
            WailingDeadTimer = 19000; //lasts 6 sec
            BackhandTimer = 8000; //2 sec stun
            CurseOfWeaknessTimer = 20000; //lasts 2 mins
            CurseOfTonguesTimer = 22000;
            CallOfTheGraveTimer = 25000;

            HasYelled = false;
        }

        void EnterToBattle(Unit* /*who*/){}

		void KilledUnit(Unit* /*victim*/)
		{
			Talk(SAY_KILL);
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (WailingDeadTimer <= diff)
            {
				DoCastVictim(SPELL_WAILINGDEAD);
                WailingDeadTimer = 19000;
            }
			else WailingDeadTimer -= diff;

            if (BackhandTimer <= diff)
            {
				DoCastVictim(SPELL_BACKHAND);
                BackhandTimer = 8000;
            }
			else BackhandTimer -= diff;

            if (CurseOfWeaknessTimer <= diff)
            {
				DoCastVictim(SPELL_CURSEOFWEAKNESS);
                CurseOfWeaknessTimer = 20000;
            }
			else CurseOfWeaknessTimer -= diff;

            if (CurseOfTonguesTimer <= diff)
            {
				DoCastVictim(SPELL_CURSEOFTONGUES);
                CurseOfTonguesTimer = 22000;
            }
			else CurseOfTonguesTimer -= diff;

            if (CallOfTheGraveTimer <= diff)
            {
				DoCastVictim(SPELL_CALLOFTHEGRAVE);
                CallOfTheGraveTimer = 25000;
            }
			else CallOfTheGraveTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_postmaster_malownAI(creature);
    }
};

void AddSC_boss_postmaster_malown()
{
    new boss_postmaster_malown();
}