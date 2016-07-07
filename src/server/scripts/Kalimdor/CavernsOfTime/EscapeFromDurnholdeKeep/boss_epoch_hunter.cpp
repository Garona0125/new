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
SDName: Boss_Epoch_Hunter
SD%Complete: 60
SDComment: Missing spawns pre-event, missing speech to be coordinated with rest of escort event.
SDCategory: Caverns of Time, Old Hillsbrad Foothills
EndScriptData */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "old_hillsbrad.h"

enum EpochHunter
{
    SAY_ENTER                   = 0,
    SAY_AGGRO                   = 1,
    SAY_SLAY                    = 2,
    SAY_BREATH                  = 3,
    SAY_DEATH                   = 4,

    SPELL_SAND_BREATH           = 31914,
    SPELL_IMPENDING_DEATH       = 31916,
    SPELL_MAGIC_DISRUPTION_AURA = 33834,
	SPELL_WING_BUFFET_5N        = 31475,
	SPELL_WING_BUFFET_5H        = 38593,
};

class boss_epoch_hunter : public CreatureScript
{
public:
    boss_epoch_hunter() : CreatureScript("boss_epoch_hunter") { }

    struct boss_epoch_hunterAI : public QuantumBasicAI
    {
        boss_epoch_hunterAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 SandBreathTimer;
        uint32 ImpendingDeathTimer;
        uint32 WingBuffetTimer;
        uint32 MagicDisruptionTimer;

        void Reset()
        {
            SandBreathTimer = 1000;
            ImpendingDeathTimer = 3000;
            WingBuffetTimer = 5000;
            MagicDisruptionTimer = 7000;
        }

        void EnterToBattle(Unit* /*who*/)
        {
			Talk(SAY_AGGRO);
        }

        void KilledUnit(Unit* /*victim*/)
        {
			Talk(SAY_SLAY);
        }

        void JustDied(Unit* /*victim*/)
        {
			Talk(SAY_DEATH);

            if (instance && instance->GetData(TYPE_THRALL_EVENT) == IN_PROGRESS)
                instance->SetData(TYPE_THRALL_PART4, DONE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (SandBreathTimer <= diff)
            {
                if (me->IsNonMeleeSpellCasted(false))
                    me->InterruptNonMeleeSpells(false);

                DoCastVictim(SPELL_SAND_BREATH);

                Talk(SAY_BREATH);

                SandBreathTimer = urand(10000, 20000);
            }
			else SandBreathTimer -= diff;

            if (ImpendingDeathTimer <= diff)
            {
                DoCastVictim(SPELL_IMPENDING_DEATH);
                ImpendingDeathTimer = 25000+rand()%5000;
            }
			else ImpendingDeathTimer -= diff;

			if (WingBuffetTimer < diff)
            {
                DoCastAOE(DUNGEON_MODE(SPELL_WING_BUFFET_5N, SPELL_WING_BUFFET_5H));
                WingBuffetTimer = urand(5000, 6000);
            }
			else WingBuffetTimer -= diff;

            if (MagicDisruptionTimer <= diff)
            {
                DoCast(me, SPELL_MAGIC_DISRUPTION_AURA);
                MagicDisruptionTimer = 15000;
            }
			else MagicDisruptionTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_epoch_hunterAI(creature);
    }
};

void AddSC_boss_epoch_hunter()
{
    new boss_epoch_hunter();
}