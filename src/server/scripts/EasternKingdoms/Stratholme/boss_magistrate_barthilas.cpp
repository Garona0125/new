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
	SPELL_DRAINING_BLOW = 16793,
	SPELL_CROWD_PUMMEL  = 10887,
	SPELL_MIGHTY_BLOW   = 14099,
	SPELL_FURIOUS_ANGER = 16791,
};

enum Models
{
	MODEL_NORMAL = 10433,
	MODEL_HUMAN  = 3637,
};

class boss_magistrate_barthilas : public CreatureScript
{
public:
    boss_magistrate_barthilas() : CreatureScript("boss_magistrate_barthilas") { }

    struct boss_magistrate_barthilasAI : public QuantumBasicAI
    {
        boss_magistrate_barthilasAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 DrainingBlowTimer;
        uint32 CrowdPummelTimer;
        uint32 MightyBlowTimer;
        uint32 FuriousAngerTimer;
        uint32 AngerCount;

        void Reset()
        {
            DrainingBlowTimer = 20000;
            CrowdPummelTimer = 15000;
            MightyBlowTimer = 10000;
            FuriousAngerTimer = 5000;
            AngerCount = 0;

            if (me->IsAlive())
                me->SetDisplayId(MODEL_NORMAL);
            else
                me->SetDisplayId(MODEL_HUMAN);
        }

        void MoveInLineOfSight(Unit* who)
        {
            //nothing to see here yet

            QuantumBasicAI::MoveInLineOfSight(who);
        }

        void JustDied(Unit* /*Killer*/)
        {
            me->SetDisplayId(MODEL_HUMAN);
        }

        void EnterToBattle(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            //Return since we have no target
            if (!UpdateVictim())
                return;

            if (FuriousAngerTimer <= diff)
            {
                FuriousAngerTimer = 4000;
                if (AngerCount > 25)
                    return;

                ++AngerCount;
                DoCast(me, SPELL_FURIOUS_ANGER, false);
            }
			else FuriousAngerTimer -= diff;

            //Draining Blow
            if (DrainingBlowTimer <= diff)
            {
                DoCastVictim(SPELL_DRAINING_BLOW);
                DrainingBlowTimer = 15000;
            }
			else DrainingBlowTimer -= diff;

            //Crowd Pummel
            if (CrowdPummelTimer <= diff)
            {
                DoCastVictim(SPELL_CROWD_PUMMEL);
                CrowdPummelTimer = 15000;
            }
			else CrowdPummelTimer -= diff;

            //Mighty Blow
            if (MightyBlowTimer <= diff)
            {
                DoCastVictim(SPELL_MIGHTY_BLOW);
                MightyBlowTimer = 20000;
            }
			else MightyBlowTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_magistrate_barthilasAI(creature);
    }
};

void AddSC_boss_magistrate_barthilas()
{
    new boss_magistrate_barthilas();
}