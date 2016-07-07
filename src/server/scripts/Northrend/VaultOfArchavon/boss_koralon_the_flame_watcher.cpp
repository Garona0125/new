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
#include "vault_of_archavon.h"

enum Events
{
    // Koralon
    EVENT_BURNING_BREATH    = 1,
    EVENT_BURNING_FURY      = 2,
    EVENT_FLAME_CINDER_A    = 3,
    EVENT_METEOR_FISTS_A    = 4,
    EVENT_METEOR_FISTS_B    = 5,
    // Flame Warder
    EVENT_FW_LAVA_BURST     = 6,
    EVENT_FW_METEOR_FISTS_A = 7,
    EVENT_FW_METEOR_FISTS_B = 8,
};

enum Spells
{
    // Spells Koralon
    SPELL_BURNING_BREATH                  = 66665,
    SPELL_BURNING_FURY                    = 66721,
    SPELL_FLAME_CINDER_A                  = 66684,
    SPELL_FLAME_CINDER_B                  = 66681,
    SPELL_METEOR_FISTS_A                  = 66725,
    SPELL_METEOR_FISTS_B                  = 67333,
    // Spells Flame Warder
    SPELL_FW_LAVA_BURST                   = 66813,
    SPELL_FW_METEOR_FISTS_A               = 66808,
    SPELL_FW_METEOR_FISTS_B               = 67331,
};

class boss_koralon_the_flame_watcher : public CreatureScript
{
    public:
        boss_koralon_the_flame_watcher() : CreatureScript("boss_koralon_the_flame_watcher") { }

        struct boss_koralon_the_flame_watcherAI : public BossAI
        {
            boss_koralon_the_flame_watcherAI(Creature* creature) : BossAI(creature, DATA_KORALON){}

			void Reset()
			{
				_Reset();

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			}

            void EnterToBattle(Unit* /*who*/)
            {
				_EnterToBattle();

                DoCast(me, SPELL_BURNING_FURY);

                events.ScheduleEvent(EVENT_BURNING_FURY, 20000);
                events.ScheduleEvent(EVENT_BURNING_BREATH, 15000);
                events.ScheduleEvent(EVENT_METEOR_FISTS_A, 75000);
                events.ScheduleEvent(EVENT_FLAME_CINDER_A, 30000);
            }

			void JustDied(Unit* /*killer*/)
			{
				_JustDied();
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
                        case EVENT_BURNING_FURY:
                            DoCast(me, SPELL_BURNING_FURY);
                            events.ScheduleEvent(EVENT_BURNING_FURY, 20000);
                            break;
                        case EVENT_BURNING_BREATH:
                            DoCast(me, SPELL_BURNING_BREATH);
                            events.ScheduleEvent(EVENT_BURNING_BREATH, 45000);
                            break;
                        case EVENT_METEOR_FISTS_A:
                            DoCast(me, SPELL_METEOR_FISTS_A);
                            events.ScheduleEvent(EVENT_METEOR_FISTS_B, 1500);
                            break;
                        case EVENT_METEOR_FISTS_B:
                            DoCast(me, SPELL_METEOR_FISTS_B);
                            events.ScheduleEvent(EVENT_METEOR_FISTS_A, 45000);
                            break;
                        case EVENT_FLAME_CINDER_A:
                            DoCast(me, SPELL_FLAME_CINDER_A);
                            events.ScheduleEvent(EVENT_FLAME_CINDER_A, 30000);
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
            return new boss_koralon_the_flame_watcherAI(creature);
        }
};

class npc_flame_warder : public CreatureScript
{
    public:
        npc_flame_warder() : CreatureScript("npc_flame_warder") { }

        struct npc_flame_warderAI : public QuantumBasicAI
        {
            npc_flame_warderAI(Creature* creature) : QuantumBasicAI(creature){}

			EventMap events;

            void Reset()
            {
                events.Reset();

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                DoZoneInCombat();

                events.ScheduleEvent(EVENT_FW_LAVA_BURST, 500);
                events.ScheduleEvent(EVENT_FW_METEOR_FISTS_A, 3000);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FW_LAVA_BURST:
                            DoCastVictim(SPELL_FW_LAVA_BURST);
                            events.ScheduleEvent(EVENT_FW_LAVA_BURST, 4000);
                            break;
                        case EVENT_FW_METEOR_FISTS_A:
                            DoCast(me, SPELL_FW_METEOR_FISTS_A);
                            events.ScheduleEvent(EVENT_FW_METEOR_FISTS_B, 6000);
                            break;
                        case EVENT_FW_METEOR_FISTS_B:
                            DoCast(me, SPELL_FW_METEOR_FISTS_B);
                            events.ScheduleEvent(EVENT_FW_METEOR_FISTS_A, 8000);
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
            return new npc_flame_warderAI(creature);
        }
};

void AddSC_boss_koralon_the_flame_watcher()
{
    new boss_koralon_the_flame_watcher();
    new npc_flame_warder();
}