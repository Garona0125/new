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
#include "SpellScript.h"
#include "the_botanica.h"

enum Says
{
    SAY_AGGRO         = -1441018,
    SAY_20_PERCENT_HP = -1441019,
    SAY_SLAY          = -1441020,
    SAY_SACRIFICE     = -1441021,
    SAY_50_PERCENT_HP = -1441022,
    SAY_HELLFIRE_1    = -1441023,
	SAY_HELLFIRE_2    = -1441024,
    SAY_DEATH         = -1441025,
};

enum Spells
{
    SPELL_SACRIFICE             = 34661,
    SPELL_HELLFIRE_NORMAL       = 34659,
    SPELL_HELLFIRE_HEROIC       = 39131,
    SPELL_ENRAGE                = 34670,
};

enum Events
{
    EVENT_SACRIFICE             = 1,
    EVENT_HELLFIRE              = 2,
    EVENT_ENRAGE                = 3,
};

class boss_thorngrin_the_tender : public CreatureScript
{
    public: boss_thorngrin_the_tender() : CreatureScript("boss_thorngrin_the_tender") { }

        struct boss_thorngrin_the_tenderAI : public BossAI
        {
            boss_thorngrin_the_tenderAI(Creature* creature) : BossAI(creature, DATA_THORNGRIN_THE_TENDER) { }

			bool phase1;
            bool phase2;

            void Reset()
            {
                _Reset();

                phase1 = true;
                phase2 = true;

				DoCast(me, SPELL_UNIT_DETECTION_ALL);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                _EnterToBattle();

                DoSendQuantumText(SAY_AGGRO, me);
                events.ScheduleEvent(EVENT_SACRIFICE, 5700);
                events.ScheduleEvent(EVENT_HELLFIRE, IsHeroic() ? urand(17400, 19300) : 18000);
                events.ScheduleEvent(EVENT_ENRAGE, 12000);
            }

            void KilledUnit(Unit* victim)
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(SAY_SLAY, me);
            }

            void JustDied(Unit* /*killer*/)
            {
                _JustDied();
                DoSendQuantumText(SAY_DEATH, me);
            }

            void DamageTaken(Unit* /*killer*/, uint32 &damage)
            {
                if (me->HealthBelowPctDamaged(50, damage) && phase1)
                {
                    phase1 = false;
                    DoSendQuantumText(SAY_50_PERCENT_HP, me);
                }

                if (me->HealthBelowPctDamaged(20, damage) && phase2)
                {
                    phase2 = false;
                    DoSendQuantumText(SAY_20_PERCENT_HP, me);
                }
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
                        case EVENT_SACRIFICE:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 0.0f, true))
                            {
                                DoSendQuantumText(SAY_SACRIFICE, me);
                                DoCast(target, SPELL_SACRIFICE, true);
                            }
                            events.ScheduleEvent(EVENT_SACRIFICE, 29400);
                            break;
                        case EVENT_HELLFIRE:
                            DoSendQuantumText(RAND(SAY_HELLFIRE_1, SAY_HELLFIRE_2), me);
                            DoCastVictim(DUNGEON_MODE(SPELL_HELLFIRE_NORMAL, SPELL_HELLFIRE_HEROIC), true);
                            events.ScheduleEvent(EVENT_HELLFIRE, IsHeroic() ? urand(17400, 19300) : 18000);
                            break;
                        case EVENT_ENRAGE:
                            DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
                            DoCast(me, SPELL_ENRAGE);
                            events.ScheduleEvent(EVENT_ENRAGE, 33000);
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
            return new boss_thorngrin_the_tenderAI(creature);
        }
};

void AddSC_boss_thorngrin_the_tender()
{
    new boss_thorngrin_the_tender();
}