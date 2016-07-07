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
SDName: Boss_Razorgore
SD%Complete: 50
SDComment: Needs additional review. Phase 1 NYI (Grethok the Controller)
SDCategory: Blackwing Lair
EndScriptData */

#include "ScriptMgr.h"
#include "QuantumCreature.h"

//Razorgore Phase 2 Script

enum Say
{
	SAY_EGGS_BROKEN1        = 0,
	SAY_EGGS_BROKEN2        = 1,
	SAY_EGGS_BROKEN3        = 2,
	SAY_DEATH               = 3,
};

enum Spells
{
    SPELL_CLEAVE            = 19632,
    SPELL_WAR_STOMP         = 24375,
    SPELL_FIREBALL_VOLLEY   = 22425,
    SPELL_CONFLAGRATION     = 23023,
};

class boss_razorgore : public CreatureScript
{
public:
    boss_razorgore() : CreatureScript("boss_razorgore") { }

    struct boss_razorgoreAI : public QuantumBasicAI
    {
        boss_razorgoreAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CleaveTimer;
        uint32 WarStompTimer;
        uint32 FireballVolleyTimer;
        uint32 ConflagrationTimer;

        void Reset()
        {
            CleaveTimer = 15000;                               //These times are probably wrong
            WarStompTimer = 35000;
            FireballVolleyTimer = 7000;
            ConflagrationTimer = 12000;
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoZoneInCombat();
        }

        void JustDied(Unit* /*killer*/)
        {
			Talk(SAY_DEATH);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = urand(7000, 10000);
            }
			else CleaveTimer -= diff;

            if (WarStompTimer <= diff)
            {
                DoCastVictim(SPELL_WAR_STOMP);
                WarStompTimer = urand(15000, 25000);
            }
			else WarStompTimer -= diff;

            if (FireballVolleyTimer <= diff)
            {
                DoCastVictim(SPELL_FIREBALL_VOLLEY);
                FireballVolleyTimer = urand(12000, 15000);
            }
			else FireballVolleyTimer -= diff;

            if (ConflagrationTimer <= diff)
            {
                DoCastVictim(SPELL_CONFLAGRATION);

                //if (DoGetThreat(me->GetVictim()))
                //DoModifyThreatPercent(me->GetVictim(), -50);

                ConflagrationTimer = 12000;
            }
			else ConflagrationTimer -= diff;

            // Aura Check. If the gamer is affected by confliguration we attack a random gamer.
            if (me->GetVictim() && me->GetVictim()->HasAura(SPELL_CONFLAGRATION))
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 100, true))
                    me->TauntApply(target);

            DoMeleeAttackIfReady();
        }
    };
	
	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_razorgoreAI(creature);
    }
};

void AddSC_boss_razorgore()
{
    new boss_razorgore();
}