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
    SPELL_DARK_STRIKE        = 19777,
    SPELL_DEMORALIZING_SHOUT = 19778,
    SPELL_INSPIRE            = 19779,
    SPELL_KNOCKDOWN          = 19780,
    SPELL_FLAME_SPEAR        = 19781,
};

enum Events
{
    EVENT_DARK_STRIKE        = 1,
    EVENT_DEMORALIZING_SHOUT = 2,
    EVENT_INSPIRE            = 3,
    EVENT_KNOCKDOWN          = 4,
    EVENT_FLAME_SPEAR        = 5,
};

class boss_sulfuron : public CreatureScript
{
    public:
        boss_sulfuron() : CreatureScript("boss_sulfuron") { }

        struct boss_sulfuronAI : public BossAI
        {
            boss_sulfuronAI(Creature* creature) : BossAI(creature, BOSS_SULFURON_HARBINGER){}

            void EnterToBattle(Unit* victim)
            {
                BossAI::EnterToBattle(victim);

                events.ScheduleEvent(EVENT_DARK_STRIKE, 10000);
                events.ScheduleEvent(EVENT_DEMORALIZING_SHOUT, 15000);
                events.ScheduleEvent(EVENT_INSPIRE, 13000);
                events.ScheduleEvent(EVENT_KNOCKDOWN, 6000);
                events.ScheduleEvent(EVENT_FLAME_SPEAR, 2000);
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
                        case EVENT_DARK_STRIKE:
                            DoCast(me, SPELL_DARK_STRIKE);
                            events.ScheduleEvent(EVENT_DARK_STRIKE, urand(15000, 18000));
                            break;
                        case EVENT_DEMORALIZING_SHOUT:
                            DoCastVictim(SPELL_DEMORALIZING_SHOUT);
                            events.ScheduleEvent(EVENT_DEMORALIZING_SHOUT, urand(15000, 20000));
                            break;
                        case EVENT_INSPIRE:
                        {
                            std::list<Creature*> healers = DoFindFriendlyMissingBuff(45.0f, SPELL_INSPIRE);
                            if (!healers.empty())
                                DoCast(Quantum::DataPackets::SelectRandomContainerElement(healers), SPELL_INSPIRE);
                            DoCast(me, SPELL_INSPIRE);
                            events.ScheduleEvent(EVENT_INSPIRE, urand(20000, 26000));
                            break;
                        }
                        case EVENT_KNOCKDOWN:
                            DoCastVictim(SPELL_KNOCKDOWN);
                            events.ScheduleEvent(EVENT_KNOCKDOWN, urand(12000, 15000));
                            break;
                        case EVENT_FLAME_SPEAR:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 0.0f, true))
							{
                                DoCast(target, SPELL_FLAME_SPEAR);
							}
                            events.ScheduleEvent(EVENT_FLAME_SPEAR, urand(12000, 16000));
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
            return new boss_sulfuronAI(creature);
        }
};

void AddSC_boss_sulfuron()
{
    new boss_sulfuron();
}