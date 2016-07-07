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
    SPELL_MAGMA_SPLASH   = 13879,
    SPELL_PYROBLAST      = 20228,
    SPELL_EARTHQUAKE     = 19798,
    SPELL_ENRAGE         = 19953,
    SPELL_GOLEMAGG_TRUST = 20553,
};

enum Events
{
    EVENT_PYROBLAST  = 1,
    EVENT_EARTHQUAKE = 2,
};

class boss_golemagg : public CreatureScript
{
    public:
        boss_golemagg() : CreatureScript("boss_golemagg") { }

        struct boss_golemaggAI : public BossAI
        {
            boss_golemaggAI(Creature* creature) : BossAI(creature, BOSS_GOLEMAGG_THE_INCINERATOR){}

            void Reset()
            {
                BossAI::Reset();
                DoCast(me, SPELL_MAGMA_SPLASH, true);
            }

            void EnterToBattle(Unit* who)
            {
                BossAI::EnterToBattle(who);
                events.ScheduleEvent(EVENT_PYROBLAST, 7000);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
            {
				if (!HealthBelowPct(HEALTH_PERCENT_10) || me->HasAura(SPELL_ENRAGE))
                    return;

                DoCast(me, SPELL_ENRAGE, true);
                events.ScheduleEvent(EVENT_EARTHQUAKE, 3000);
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
                        case EVENT_PYROBLAST:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
							{
                                DoCast(target, SPELL_PYROBLAST);
							}
                            events.ScheduleEvent(EVENT_PYROBLAST, 7000);
                            break;
                        case EVENT_EARTHQUAKE:
                            DoCastVictim(SPELL_EARTHQUAKE);
                            events.ScheduleEvent(EVENT_EARTHQUAKE, 3000);
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
            return new boss_golemaggAI(creature);
        }
};

void AddSC_boss_golemagg()
{
    new boss_golemagg();
}