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
#include "molten_core.h"

enum Spells
{
    SPELL_GEHENNAS_CURSE     = 19716,
    SPELL_RAIN_OF_FIRE       = 19717,
    SPELL_SHADOW_BOLT_RANDOM = 19728,
	SPELL_SHADOW_BOLT_TARGET = 19729,
};

enum Events
{
    EVENT_GEHENNAS_CURSE = 1,
    EVENT_RAIN_OF_FIRE   = 2,
    EVENT_SHADOW_BOLT    = 3,
};

class boss_gehennas : public CreatureScript
{
    public:
        boss_gehennas() : CreatureScript("boss_gehennas") { }

        struct boss_gehennasAI : public BossAI
        {
            boss_gehennasAI(Creature* creature) : BossAI(creature, BOSS_GEHENNAS){}

            void EnterToBattle(Unit* victim)
            {
                BossAI::EnterToBattle(victim);
                events.ScheduleEvent(EVENT_GEHENNAS_CURSE, 12000);
                events.ScheduleEvent(EVENT_RAIN_OF_FIRE, 10000);
                events.ScheduleEvent(EVENT_SHADOW_BOLT, 6000);
            }

            void UpdateAI(const uint32 diff)
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
                        case EVENT_GEHENNAS_CURSE:
                            DoCastVictim(SPELL_GEHENNAS_CURSE);
                            events.ScheduleEvent(EVENT_GEHENNAS_CURSE, urand(22000, 30000));
                            break;
                        case EVENT_RAIN_OF_FIRE:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
							{
                                DoCast(target, SPELL_RAIN_OF_FIRE);
							}
                            events.ScheduleEvent(EVENT_RAIN_OF_FIRE, urand(4000, 12000));
                            break;
                        case EVENT_SHADOW_BOLT:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
                                DoCast(target, RAND(SPELL_SHADOW_BOLT_RANDOM, SPELL_SHADOW_BOLT_TARGET));

                            events.ScheduleEvent(EVENT_SHADOW_BOLT, 7000);
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
            return new boss_gehennasAI(creature);
        }
};

void AddSC_boss_gehennas()
{
    new boss_gehennas();
}