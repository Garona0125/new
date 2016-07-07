/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "mechanar.h"

enum Texts
{
    SAY_AGGRO       = -1554000,
	SAY_SAW_BLADE_1 = -1554001,
	SAY_SAW_BLADE_2 = -1554002,
	SAY_SLAY_1      = -1554003,
	SAY_SLAY_2      = -1554004,
	SAY_DEATH       = -1554005,
};

enum Spells
{
    SPELL_STREAM_OF_MACHINE_FLUID = 35311,
    SPELL_SAW_BLADE_5N            = 35318,
    SPELL_SAW_BLADE_5H            = 39192,
    SPELL_SHADOW_POWER_5N         = 35322,
    SPELL_SHADOW_POWER_5H         = 39193,
};

enum Events
{
    EVENT_STREAM_OF_MACHINE_FLUID   = 1,
    EVENT_SAW_BLADE                 = 2,
    EVENT_SHADOW_POWER              = 3,
};

class boss_gatewatcher_gyrokill : public CreatureScript
{
    public:
        boss_gatewatcher_gyrokill() : CreatureScript("boss_gatewatcher_gyrokill") {}

        struct boss_gatewatcher_gyrokillAI : public BossAI
        {
            boss_gatewatcher_gyrokillAI(Creature* creature) : BossAI(creature, DATA_GATEWATCHER_GYROKILL) {}

			void Reset()
			{
				DoCast(me, SPELL_UNIT_DETECTION_ALL);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			}

            void EnterToBattle(Unit* /*who*/)
            {
                _EnterToBattle();

                events.ScheduleEvent(EVENT_STREAM_OF_MACHINE_FLUID, 10000);
                events.ScheduleEvent(EVENT_SAW_BLADE, 20000);
                events.ScheduleEvent(EVENT_SHADOW_POWER, 25000);
                DoSendQuantumText(SAY_AGGRO, me);
            }

            void KilledUnit(Unit* victim)
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

			void JustDied(Unit* /*killer*/)
            {
                _JustDied();
                DoSendQuantumText(SAY_DEATH, me);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_STREAM_OF_MACHINE_FLUID:
                            DoCastVictim(SPELL_STREAM_OF_MACHINE_FLUID, true);
                            events.ScheduleEvent(EVENT_STREAM_OF_MACHINE_FLUID, urand(13000, 17000));
                            break;
                        case EVENT_SAW_BLADE:
                            DoCast(me, DUNGEON_MODE(SPELL_SAW_BLADE_5N, SPELL_SAW_BLADE_5H));
							DoSendQuantumText(RAND(SAY_SAW_BLADE_1, SAY_SAW_BLADE_2), me);
                            events.ScheduleEvent(EVENT_SAW_BLADE, urand(20000, 30000));
                            break;
                        case EVENT_SHADOW_POWER:
                            DoCast(me, DUNGEON_MODE(SPELL_SHADOW_POWER_5N, SPELL_SHADOW_POWER_5H));
                            events.ScheduleEvent(EVENT_SAW_BLADE, urand(25000, 35000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_gatewatcher_gyrokillAI (creature);
        }
};

void AddSC_boss_gatewatcher_gyrokill()
{
    new boss_gatewatcher_gyrokill();
}