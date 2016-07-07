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

enum Emotes
{
	EMOTE_SERVICE       = -1409000,
};

enum Spells
{
    SPELL_INFERNO      = 19695,
    SPELL_IGNITE_MANA  = 19659,
    SPELL_LIVING_BOMB  = 20475,
    SPELL_ARMAGEDDON   = 20479,
};

enum Events
{
    EVENT_INFERNO     = 1,
    EVENT_IGNITE_MANA = 2,
    EVENT_LIVING_BOMB = 3,
};

class boss_baron_geddon : public CreatureScript
{
    public:
        boss_baron_geddon() : CreatureScript("boss_baron_geddon") { }

        struct boss_baron_geddonAI : public BossAI
        {
            boss_baron_geddonAI(Creature* creature) : BossAI(creature, BOSS_BARON_GEDDON){}

            void EnterToBattle(Unit* victim)
            {
                BossAI::EnterToBattle(victim);

                events.ScheduleEvent(EVENT_INFERNO, 45000);
                events.ScheduleEvent(EVENT_IGNITE_MANA, 30000);
                events.ScheduleEvent(EVENT_LIVING_BOMB, 35000);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                // If we are <2% hp cast Armageddon
                if (!HealthAbovePct(2))
                {
                    me->InterruptNonMeleeSpells(true);
                    DoCast(me, SPELL_ARMAGEDDON);
					DoSendQuantumText(EMOTE_SERVICE, me);
                    return;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INFERNO:
                            DoCast(me, SPELL_INFERNO);
                            events.ScheduleEvent(EVENT_INFERNO, 45000);
                            break;
                        case EVENT_IGNITE_MANA:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 0.0f, true, -SPELL_IGNITE_MANA))
							{
                                DoCast(target, SPELL_IGNITE_MANA);
							}
                            events.ScheduleEvent(EVENT_IGNITE_MANA, 30000);
                            break;
                        case EVENT_LIVING_BOMB:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 0.0f, true))
							{
                                DoCast(target, SPELL_LIVING_BOMB);
							}
                            events.ScheduleEvent(EVENT_LIVING_BOMB, 35000);
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
            return new boss_baron_geddonAI (creature);
        }
};

void AddSC_boss_baron_geddon()
{
    new boss_baron_geddon();
}