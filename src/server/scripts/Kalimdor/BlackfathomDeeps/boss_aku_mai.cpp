/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
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
#include "blackfathom_deeps.h"

enum Spells
{
    SPELL_POISON_CLOUD  = 3815,
    SPELL_FRENZIED_RAGE = 3490,
};

class boss_aku_mai : public CreatureScript
{
public:
    boss_aku_mai() : CreatureScript("boss_aku_mai") { }

    struct boss_aku_maiAI : public QuantumBasicAI
    {
        boss_aku_maiAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 PoisonCloudTimer;

        bool IsEnraged;

        InstanceScript* instance;

        void Reset()
        {
            PoisonCloudTimer = urand(5000, 9000);
            IsEnraged = false;

			instance->SetData(TYPE_AKU_MAI, NOT_STARTED);
        }

        void EnterToBattle(Unit* /*who*/)
        {
			instance->SetData(TYPE_AKU_MAI, IN_PROGRESS);
        }

        void JustDied(Unit* /*killer*/)
        {
			instance->SetData(TYPE_AKU_MAI, DONE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (PoisonCloudTimer <= diff)
            {
                DoCastVictim(SPELL_POISON_CLOUD);
                PoisonCloudTimer = urand(25000, 50000);
            }
			else PoisonCloudTimer -= diff;

            if (!IsEnraged && HealthBelowPct(HEALTH_PERCENT_30))
            {
                DoCast(me, SPELL_FRENZIED_RAGE);
                IsEnraged = true;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_aku_maiAI(creature);
    }
};

void AddSC_boss_aku_mai()
{
    new boss_aku_mai();
}