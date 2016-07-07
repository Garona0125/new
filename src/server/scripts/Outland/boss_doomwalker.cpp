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

enum Texts
{
	SAY_AGGRO        = -1000159,
	SAY_EARTHQUAKE_1 = -1000160,
	SAY_EARTHQUAKE_2 = -1000161,
	SAY_OVERRUN_1    = -1000162,
	SAY_OVERRUN_2    = -1000163,
	SAY_SLAY_1       = -1000164,
	SAY_SLAY_2       = -1000165,
	SAY_SLAY_3       = -1000166,
	SAY_DEATH        = -1000167,
};

enum Spells
{
    SPELL_EARTHQUAKE        = 32686,
    SPELL_SUNDER_ARMOR      = 33661,
    SPELL_CHAIN_LIGHTNING   = 33665,
    SPELL_OVERRUN           = 32636,
    SPELL_ENRAGE            = 33653,
    SPELL_MARK_DEATH        = 37128,
    SPELL_AURA_DEATH        = 37131,
};

enum Events
{
    EVENT_ENRAGE    = 1,
    EVENT_ARMOR     = 2,
    EVENT_CHAIN     = 3,
    EVENT_QUAKE     = 4,
    EVENT_OVERRUN   = 5,
};

class boss_doomwalker : public CreatureScript
{
    public:
        boss_doomwalker() : CreatureScript("boss_doomwalker") { }

        struct boss_doomwalkerAI : public QuantumBasicAI
        {
			boss_doomwalkerAI(Creature* creature) : QuantumBasicAI(creature) {}

			EventMap events;

			bool Enrage;

            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_ENRAGE, 0);
                events.ScheduleEvent(EVENT_ARMOR, urand(5000, 13000));
                events.ScheduleEvent(EVENT_CHAIN, urand(10000, 30000));
                events.ScheduleEvent(EVENT_QUAKE, urand(25000, 35000));
                events.ScheduleEvent(EVENT_OVERRUN, urand(30000, 45000));
                Enrage = false;
            }

            void KilledUnit(Unit* victim)
            {
                victim->CastSpell(victim, SPELL_MARK_DEATH, 0);

				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
            }

            void JustDied(Unit* /*killer*/)
            {
				DoSendQuantumText(SAY_DEATH, me);
            }

            void EnterToBattle(Unit* /*who*/)
            {
				DoSendQuantumText(SAY_AGGRO, me);
            }

            void MoveInLineOfSight(Unit* who)
            {
                if (who && who->GetTypeId() == TYPE_ID_PLAYER && me->IsValidAttackTarget(who))
				{
                    if (who->HasAura(SPELL_MARK_DEATH, 0))
                        who->CastSpell(who, SPELL_AURA_DEATH, 1);
				}
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
                        case EVENT_ENRAGE:
                            if (!HealthAbovePct(20))
                            {
                                DoCast(me, SPELL_ENRAGE);
                                events.ScheduleEvent(EVENT_ENRAGE, 6000);
                                Enrage = true;
                            }
                            break;
                        case EVENT_OVERRUN:
							DoSendQuantumText(RAND(SAY_OVERRUN_1, SAY_OVERRUN_2), me);
                            DoCastVictim(SPELL_OVERRUN);
                            events.ScheduleEvent(EVENT_OVERRUN, urand(25000, 40000));
                            break;
                        case EVENT_QUAKE:
                            if (urand(0, 1))
                                return;
							DoSendQuantumText(RAND(SAY_EARTHQUAKE_1, SAY_EARTHQUAKE_2), me);
                            if (Enrage)
                                me->RemoveAurasDueToSpell(SPELL_ENRAGE);
                            DoCast(me, SPELL_EARTHQUAKE);
                            events.ScheduleEvent(EVENT_QUAKE, urand(30000, 55000));
                            break;
                        case EVENT_CHAIN:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 0.0f, true))
							{
                                DoCast(target, SPELL_CHAIN_LIGHTNING);
							}
                            events.ScheduleEvent(EVENT_CHAIN, urand(7000, 27000));
                            break;
                        case EVENT_ARMOR:
                            DoCastVictim(SPELL_SUNDER_ARMOR);
                            events.ScheduleEvent(EVENT_ARMOR, urand(10000, 25000));
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
            return new boss_doomwalkerAI(creature);
        }
};

void AddSC_boss_doomwalker()
{
    new boss_doomwalker();
}