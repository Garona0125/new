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
    SPELL_ANTIMAGIC_PULSE   = 19492,
    SPELL_MAGMA_SHACKLES    = 19496,
    SPELL_ENRAGE            = 19516,

    SPELL_ERUPTION          = 19497,
    SPELL_IMMOLATE          = 20294,
};

enum Events
{
    EVENT_ANTIMAGIC_PULSE    = 1,
    EVENT_MAGMA_SHACKLES     = 2,
};

class boss_garr : public CreatureScript
{
    public:
        boss_garr() : CreatureScript("boss_garr") { }

        struct boss_garrAI : public BossAI
        {
            boss_garrAI(Creature* creature) : BossAI(creature, BOSS_GARR){}

            void EnterToBattle(Unit* victim)
            {
                BossAI::EnterToBattle(victim);
                events.ScheduleEvent(EVENT_ANTIMAGIC_PULSE, 25000);
                events.ScheduleEvent(EVENT_MAGMA_SHACKLES, 15000);
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
                        case EVENT_ANTIMAGIC_PULSE:
                            DoCast(me, SPELL_ANTIMAGIC_PULSE);
                            events.ScheduleEvent(EVENT_ANTIMAGIC_PULSE, urand(10000, 15000));
                            break;
                        case EVENT_MAGMA_SHACKLES:
                            DoCast(me, SPELL_MAGMA_SHACKLES);
                            events.ScheduleEvent(EVENT_MAGMA_SHACKLES, urand(8000, 12000));
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
            return new boss_garrAI(creature);
        }
};

class mob_firesworn : public CreatureScript
{
    public:
        mob_firesworn() : CreatureScript("mob_firesworn") { }

        struct mob_fireswornAI : public QuantumBasicAI
        {
            mob_fireswornAI(Creature* creature) : QuantumBasicAI(creature) {}

            uint32 ImmolateTimer;

            void Reset()
            {
                ImmolateTimer = 4000;                              //These times are probably wrong
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage)
            {
                uint32 const health10pct = me->CountPctFromMaxHealth(HEALTH_PERCENT_10);
                uint32 health = me->GetHealth();
                if (int32(health) - int32(damage) < int32(health10pct))
                {
                    damage = 0;
                    DoCastVictim(SPELL_ERUPTION);
                    me->DespawnAfterAction();
                }
            }

			void JustDied(Unit* /*killer*/)
			{
				if (Creature* garr = me->FindCreatureWithDistance(NPC_GARR, 300.f, true))
					garr->CastSpell(garr, SPELL_ENRAGE, true);
			}

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (ImmolateTimer <= diff)
                {
                     if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					 {
                        DoCast(target, SPELL_IMMOLATE);
						ImmolateTimer = urand(5000, 10000);
					 }
                }
                else ImmolateTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_fireswornAI(creature);
        }
};

void AddSC_boss_garr()
{
    new boss_garr();
    new mob_firesworn();
}