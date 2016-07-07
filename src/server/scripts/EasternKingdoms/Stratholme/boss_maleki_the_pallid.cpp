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
	SPELL_FROSTBOLT  = 17503,
	SPELL_DRAIN_LIFE = 20743,
	SPELL_DRAIN_MANA = 17243,
	SPELL_ICE_TOMB   = 16869,
};

class boss_maleki_the_pallid : public CreatureScript
{
public:
    boss_maleki_the_pallid() : CreatureScript("boss_maleki_the_pallid") { }

    struct boss_maleki_the_pallidAI : public QuantumBasicAI
    {
        boss_maleki_the_pallidAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 FrostboltTimer;
        uint32 IceTombTimer;
        uint32 DrainLifeTimer;

        void Reset()
        {
            FrostboltTimer = 1000;
            IceTombTimer = 16000;
            DrainLifeTimer = 31000;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void JustDied(Unit* /*killer*/)
        {
			instance->SetData(TYPE_PALLID, IN_PROGRESS);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (FrostboltTimer <= diff)
            {
				DoCastVictim(SPELL_FROSTBOLT);
                FrostboltTimer = 3500;
            }
			else FrostboltTimer -= diff;

            if (IceTombTimer <= diff)
            {
				DoCastVictim(SPELL_ICE_TOMB);
                IceTombTimer = 28000;
            }
			else IceTombTimer -= diff;

            if (DrainLifeTimer <= diff)
            {
				DoCastVictim(SPELL_DRAIN_LIFE);
                DrainLifeTimer = 31000;
            }
			else DrainLifeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_maleki_the_pallidAI(creature);
    }
};

void AddSC_boss_maleki_the_pallid()
{
    new boss_maleki_the_pallid();
}