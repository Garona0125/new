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

enum Spells
{
	SPELL_BANSHEEWAIL  = 16565,
	SPELL_BANSHEECURSE = 16867,
	SPELL_SILENCE      = 18327,
	SPELL_POSSESS      = 17244,
};

class boss_baroness_anastari : public CreatureScript
{
public:
    boss_baroness_anastari() : CreatureScript("boss_baroness_anastari") { }

    struct boss_baroness_anastariAI : public QuantumBasicAI
    {
        boss_baroness_anastariAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 BansheeWailTimer;
        uint32 BansheeCurseTimer;
        uint32 SilenceTimer;
        uint32 PossessTimer;

        void Reset()
        {
            BansheeWailTimer = 1000;
            BansheeCurseTimer = 11000;
            SilenceTimer = 13000;
            PossessTimer = 35000;
        }

        void EnterToBattle(Unit* /*who*/) {}

         void JustDied(Unit* /*killer*/)
         {
			 instance->SetData(TYPE_BARONESS, IN_PROGRESS);
         }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (BansheeWailTimer <= diff)
            {
				DoCastVictim(SPELL_BANSHEEWAIL);
                BansheeWailTimer = 4000;
            }
			else BansheeWailTimer -= diff;

            if (BansheeCurseTimer <= diff)
            {
				DoCastVictim(SPELL_BANSHEECURSE);
                BansheeCurseTimer = 18000;
            }
			else BansheeCurseTimer -= diff;

            if (SilenceTimer <= diff)
            {
				DoCastVictim(SPELL_SILENCE);
                SilenceTimer = 13000;
            }
			else SilenceTimer -= diff;

            if (PossessTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
					DoCast(target, SPELL_POSSESS);
					PossessTimer = 50000;
				}
			}
			else PossessTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_baroness_anastariAI(creature);
    }
};

void AddSC_boss_baroness_anastari()
{
    new boss_baroness_anastari();
}