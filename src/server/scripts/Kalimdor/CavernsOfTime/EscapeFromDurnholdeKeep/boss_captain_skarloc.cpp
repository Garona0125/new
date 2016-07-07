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
SDName: Boss_Captain_Skarloc
SD%Complete: 75
SDComment: Missing adds, missing waypoints to move up to Thrall once spawned + speech before enter combat.
SDCategory: Caverns of Time, Old Hillsbrad Foothills
EndScriptData */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "GameEventMgr.h"
#include "old_hillsbrad.h"

enum Texts
{
    SAY_ENTER                   = 0,
    SAY_TAUNT1                  = 1,
    SAY_TAUNT2                  = 2,
    SAY_SLAY                    = 3,
    SAY_DEATH                   = 4,
};

enum Spells
{
    SPELL_HOLY_LIGHT            = 29427,
    SPELL_CLEANSE               = 29380,
    SPELL_HAMMER_OF_JUSTICE     = 13005,
    SPELL_HOLY_SHIELD           = 31904,
    SPELL_DEVOTION_AURA         = 8258,
    SPELL_CONSECRATION          = 38385,
};

enum SeasonalData
{
	 GAME_EVENT_WINTER_VEIL = 2,
};

class boss_captain_skarloc : public CreatureScript
{
public:
    boss_captain_skarloc() : CreatureScript("boss_captain_skarloc") { }

    struct boss_captain_skarlocAI : public QuantumBasicAI
    {
        boss_captain_skarlocAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 HolyLightTimer;
        uint32 CleanseTimer;
        uint32 HammerOfJusticeTimer;
        uint32 HolyShieldTimer;
        uint32 DevotionAuraTimer;
        uint32 ConsecrationTimer;

        void Reset()
        {
            HolyLightTimer = urand(20000, 30000);
            CleanseTimer = 10000;
            HammerOfJusticeTimer = urand(20000, 35000);
            HolyShieldTimer = 240000;
            DevotionAuraTimer = 3000;
            ConsecrationTimer = 8000;

			if (sGameEventMgr->IsActiveEvent(GAME_EVENT_WINTER_VEIL))
				me->SetDisplayId(MODEL_CHRISTMAS_CAPTAIN_SCARLOC);
        }

        void EnterToBattle(Unit* /*who*/)
        {
			Talk(SAY_TAUNT1);
			Talk(SAY_TAUNT2);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            Talk(SAY_SLAY);
        }

        void JustDied(Unit* /*victim*/)
        {
			Talk(SAY_DEATH);

            if (instance && instance->GetData(TYPE_THRALL_EVENT) == IN_PROGRESS)
                instance->SetData(TYPE_THRALL_PART1, DONE);
        }

        void UpdateAI(const uint32 diff)
        {
            //Return since we have no target
            if (!UpdateVictim())
                return;

            //HolyLight
            if (HolyLightTimer <= diff)
            {
                DoCast(me, SPELL_HOLY_LIGHT);
                HolyLightTimer = 30000;
            }
			else HolyLightTimer -= diff;

            //Cleanse
            if (CleanseTimer <= diff)
            {
                DoCast(me, SPELL_CLEANSE);
                CleanseTimer = 10000;
            }
			else CleanseTimer -= diff;

            //Hammer of Justice
            if (HammerOfJusticeTimer <= diff)
            {
                DoCastVictim(SPELL_HAMMER_OF_JUSTICE);
                HammerOfJusticeTimer = 60000;
            }
			else HammerOfJusticeTimer -= diff;

            //Holy Shield
            if (HolyShieldTimer <= diff)
            {
                DoCast(me, SPELL_HOLY_SHIELD);
                HolyShieldTimer = 240000;
            }
			else HolyShieldTimer -= diff;

            //Devotion_Aura
            if (DevotionAuraTimer <= diff)
            {
                DoCast(me, SPELL_DEVOTION_AURA);
                DevotionAuraTimer = urand(45000, 55000);
            }
			else DevotionAuraTimer -= diff;

            //Consecration
            if (ConsecrationTimer <= diff)
            {
                //DoCastVictim(SPELL_CONSECRATION);
                ConsecrationTimer = urand(5000, 10000);
            }
			else ConsecrationTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
	
	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_captain_skarlocAI(creature);
    }
};

void AddSC_boss_captain_skarloc()
{
    new boss_captain_skarloc();
}