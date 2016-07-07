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
#include "oculus.h"

enum Yells
{
    SAY_AGGRO    = -1578000,
    SAY_SLAY_1   = -1578001,
    SAY_SLAY_2   = -1578002,
    SAY_SLAY_3   = -1578003,
    SAY_DEATH    = -1578004,
    SAY_PULL_1   = -1578005,
    SAY_PULL_2   = -1578006,
    SAY_PULL_3   = -1578007,
    SAY_PULL_4   = -1578008,
    SAY_STOMP_1  = -1578009,
    SAY_STOMP_2  = -1578010,
    SAY_STOMP_3  = -1578011,
};

enum Spells
{
    SPELL_MAGIC_PULL                = 51336,
    SPELL_THUNDERING_STOMP_5N       = 50774,
	SPELL_THUNDERING_STOMP_5H       = 59370,
    SPELL_UNSTABLE_SPHERE_PASSIVE   = 50756,
    SPELL_UNSTABLE_SPHERE_PULSE     = 50757,
    SPELL_UNSTABLE_SPHERE_TIMER     = 50758,
};

enum DrakosAchievement
{
    ACHIEV_TIMED_START_EVENT = 18153
};

enum DrakosEvents
{
    EVENT_MAGIC_PULL = 1,
    EVENT_STOMP,
    EVENT_BOMB_SUMMON
};

class boss_drakos : public CreatureScript
{
    public:
        boss_drakos() : CreatureScript("boss_drakos") { }

        struct boss_drakosAI : public BossAI
        {
            boss_drakosAI(Creature* creature) : BossAI(creature, DATA_DRAKOS_EVENT) {}

			bool PostPull;

            void Reset()
            {
                _Reset();

				events.ScheduleEvent(EVENT_BOMB_SUMMON, 2000);
                events.ScheduleEvent(EVENT_MAGIC_PULL, 15000);
                events.ScheduleEvent(EVENT_STOMP, 17000);

                PostPull = false;
            }

            void EnterToBattle(Unit* /*who*/)
            {
                _EnterToBattle();

                DoSendQuantumText(SAY_AGGRO, me);
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
                        case EVENT_BOMB_SUMMON:
                            Position position;
                            me->GetPosition(&position);
                            for (uint8 i = 0; i <= (PostPull ? 3 : 0); ++i)
                            {
                                me->GetRandomNearPosition(position, float(urand(0, 10)));
                                me->SummonCreature(NPC_UNSTABLE_SPHERE, position);
                            }
                            events.ScheduleEvent(EVENT_BOMB_SUMMON, 3000);
                            break;
                        case EVENT_MAGIC_PULL:
                            DoSendQuantumText(RAND(SAY_PULL_1, SAY_PULL_2, SAY_PULL_3, SAY_PULL_4), me);
                            DoCastAOE(SPELL_MAGIC_PULL);
                            PostPull = true;
                            events.ScheduleEvent(EVENT_MAGIC_PULL, 15000);
                            break;
                        case EVENT_STOMP:
                            DoSendQuantumText(RAND(SAY_STOMP_1, SAY_STOMP_2, SAY_STOMP_3), me);
                            DoCastAOE(DUNGEON_MODE(SPELL_THUNDERING_STOMP_5N, SPELL_THUNDERING_STOMP_5H));
                            events.ScheduleEvent(EVENT_STOMP, 17000);
                            break;
                    }
                }
                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*killer*/)
            {
                _JustDied();
                DoSendQuantumText(SAY_DEATH, me);

                // start achievement timer (kill Eregos within 20 min)
                instance->DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_TIMED_START_EVENT);
            }

            void KilledUnit(Unit* /*victim*/)
            {
                DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_drakosAI(creature);
        }
};

class npc_unstable_sphere : public CreatureScript
{
    public:
        npc_unstable_sphere() : CreatureScript("npc_unstable_sphere") { }

        struct npc_unstable_sphereAI : public QuantumBasicAI
        {
            npc_unstable_sphereAI(Creature* creature) : QuantumBasicAI(creature) {}

			uint32 PulseTimer;
            uint32 DeathTimer;

            void Reset()
            {
				PulseTimer = 1000;
                DeathTimer = 19000;

                me->SetReactState(REACT_PASSIVE);
                me->GetMotionMaster()->MoveRandom(40.0f);

                DoCast(me, SPELL_UNSTABLE_SPHERE_PASSIVE, true);
                DoCast(me, SPELL_UNSTABLE_SPHERE_TIMER, true);
            }

            void UpdateAI(uint32 const diff)
            {
                if (PulseTimer <= diff)
                {
                    DoCast(me, SPELL_UNSTABLE_SPHERE_PULSE, true);
                    PulseTimer = 3000;
                }
                else PulseTimer -= diff;

                if (DeathTimer <= diff)
                    me->DisappearAndDie();
                else
                    DeathTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_unstable_sphereAI(creature);
        }
};

void AddSC_boss_drakos()
{
    new boss_drakos();
    new npc_unstable_sphere();
}