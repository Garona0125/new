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

#define EMOTE_BERSERK           -1590002

enum Spells
{
	SPELL_ROCK_SHARDS       = 58678,
	SPELL_CRUSHING_LEAP_10N = 58960,
	SPELL_CRUSHING_LEAP_25N = 60894,
	SPELL_STOMP_10N         = 58663,
	SPELL_STOMP_25N         = 60880,
	SPELL_IMPALE_10N        = 58666,
	SPELL_IMPALE_25N        = 60882,
	SPELL_BERSERK           = 47008,
	SPELL_ROCK_SHOWER_10N   = 60919,
	SPELL_ROCK_SHOWER_25N   = 60923,
	SPELL_SHIELD_CRUSH_10N  = 60897,
	SPELL_SHIELD_CRUSH_25N  = 60899,
	SPELL_WHIRL_10N         = 60902,
	SPELL_WHIRL_25N         = 60916,
};

#define SAY_LEAP "Archavon the Stone Watcher lunges for $N!"

enum Events
{
    // Archavon
    EVENT_ROCK_SHARDS   = 1,
    EVENT_LEAP          = 2,
    EVENT_STOMP         = 3,
    EVENT_BERSERK       = 4,
	EVENT_IMPALE_DAMAGE = 5,
    // Warders
    EVENT_ROCK_SHOWER   = 6,
    EVENT_SHIELD_CRUSH  = 7,
    EVENT_WHIRL         = 8,
};

class boss_archavon_the_stone_watcher : public CreatureScript
{
    public:
        boss_archavon_the_stone_watcher() : CreatureScript("boss_archavon_the_stone_watcher") { }

        struct boss_archavon_the_stone_watcherAI : public BossAI
        {
            boss_archavon_the_stone_watcherAI(Creature* creature) : BossAI(creature, DATA_ARCHAVON){}

			uint64 ImpaleTarget;

			void Reset()
			{
				_Reset();
				ImpaleTarget = 0;

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			}

            void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply)
            {
                if (!who->ToPlayer())
                    return;

                if (apply)
                {
                    who->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, true);
                    me->SetInCombatWithZone();
                }
                else
                {
                    me->ResetAttackTimer();
                    who->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_ALL, false);
                    // TODO: move knockback / jump?
					events.DelayEvents(500, 1);
					events.ScheduleEvent(EVENT_IMPALE_DAMAGE, 250);
					ImpaleTarget = who->GetGUID();
                }
            }

            void EnterToBattle(Unit* /*who*/)
            {
				_EnterToBattle();

                events.ScheduleEvent(EVENT_ROCK_SHARDS, 15000, 1);
                events.ScheduleEvent(EVENT_LEAP, 30000);
                events.ScheduleEvent(EVENT_STOMP, 45000);
                events.ScheduleEvent(EVENT_BERSERK, 300000);
            }

			void JustDied(Unit* /*killer*/)
			{
				_JustDied();
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
                        case EVENT_ROCK_SHARDS:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
							{
                                DoCast(target, SPELL_ROCK_SHARDS);
							}
                            events.ScheduleEvent(EVENT_ROCK_SHARDS, 15000, 1);
                            break;
                        case EVENT_LEAP:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, -10, true))
                            {
								DoCast(target, RAID_MODE(SPELL_CRUSHING_LEAP_10N, SPELL_CRUSHING_LEAP_25N));
                                events.ScheduleEvent(EVENT_LEAP, 30000);
                            }
                            else
							{
                                events.ScheduleEvent(EVENT_LEAP, 3000);
							}
                            break;
                        case EVENT_STOMP:
							DoCastAOE(RAID_MODE(SPELL_STOMP_10N, SPELL_STOMP_25N));
                            events.DelayEvents(3000, 1);
                            events.ScheduleEvent(EVENT_STOMP, 45000);
                            break;
						case EVENT_IMPALE_DAMAGE:
						{
							Unit* target = ObjectAccessor::GetUnit(*me, ImpaleTarget);
							if (target && target->IsAlive())
							{
								DoCast(target, RAID_MODE(SPELL_IMPALE_10N, SPELL_IMPALE_25N));
								ImpaleTarget = 0;
							}
							break;
						}
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK, true);
                            DoSendQuantumText(EMOTE_BERSERK, me);
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
            return new boss_archavon_the_stone_watcherAI(creature);
        }
};

class npc_archavon_warder : public CreatureScript
{
    public:
        npc_archavon_warder() : CreatureScript("npc_archavon_warder") { }

        struct npc_archavon_warderAI : public QuantumBasicAI
        {
            npc_archavon_warderAI(Creature* creature) : QuantumBasicAI(creature){}

            EventMap events;

            void Reset()
            {
                events.Reset();

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

                events.ScheduleEvent(EVENT_ROCK_SHOWER, 2000);
                events.ScheduleEvent(EVENT_SHIELD_CRUSH, 4000);
                events.ScheduleEvent(EVENT_WHIRL, 6000);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                DoZoneInCombat();
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
                        case EVENT_ROCK_SHOWER:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
							{
								DoCast(target, RAID_MODE(SPELL_ROCK_SHOWER_10N, SPELL_ROCK_SHOWER_25N));
							}
                            events.ScheduleEvent(EVENT_ROCK_SHARDS, 4000);
                            break;
                        case EVENT_SHIELD_CRUSH:
							DoCastVictim(RAID_MODE(SPELL_SHIELD_CRUSH_10N, SPELL_SHIELD_CRUSH_25N));
                            events.ScheduleEvent(EVENT_SHIELD_CRUSH, 6000);
                            break;
                        case EVENT_WHIRL:
							DoCastVictim(RAID_MODE(SPELL_WHIRL_10N, SPELL_WHIRL_25N));
                            events.ScheduleEvent(EVENT_WHIRL, 8000);
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
            return new npc_archavon_warderAI(creature);
        }
};

void AddSC_boss_archavon_the_stone_watcher()
{
    new boss_archavon_the_stone_watcher();
    new npc_archavon_warder();
}