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
#include "SpellAuraEffects.h"
#include "naxxramas.h"

enum Spells
{
	SPELL_MORTAL_WOUND       = 25646,
	SPELL_ENRAGE_10          = 28371,
	SPELL_ENRAGE_25          = 54427,
	SPELL_DECIMATE_10        = 28374,
	SPELL_DECIMATE_25        = 54426,
	SPELL_BERSERK            = 26662,
	SPELL_INFECTED_WOUND     = 29307,
	SPELL_INFECTED_TRIGGERED = 29306,
};

const Position PosSummon[3] =
{
    {3267.9f, -3172.1f, 297.42f, 0.94f},
    {3253.2f, -3132.3f, 297.42f, 0},
    {3308.3f, -3185.8f, 297.42f, 1.58f},
};

enum Events
{
    EVENT_WOUND    = 1,
    EVENT_ENRAGE   = 2,
    EVENT_DECIMATE = 3,
    EVENT_BERSERK  = 4,
    EVENT_SUMMON   = 5,
};

#define EMOTE_NEARBY   "Spots a nearby zombie to devour!"
#define EMOTE_ENRAGE   "Gluth becomes enraged!"
#define EMOTE_DECIMATE "Gluth decimates all nearby flesh!"

class boss_gluth : public CreatureScript
{
public:
    boss_gluth() : CreatureScript("boss_gluth") { }

    struct boss_gluthAI : public BossAI
    {
        boss_gluthAI(Creature* creature) : BossAI(creature, BOSS_GLUTH)
        {
            me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_INFECTED_TRIGGERED, true);
        }

        void Reset()
        {
            _Reset();

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void AttackGluth(Creature* who)
        {
            who->SetReactState(REACT_PASSIVE);
            who->AddThreat(me, 9999999);
            who->AI()->AttackStart(me);
            who->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, true);
            who->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (who->ToCreature() && who->ToCreature()->GetEntry() == NPC_ZOMBIE && me->IsWithinDistInMap(who, 7.0f))
            {
                AttackStart(who);
                me->SetReactState(REACT_PASSIVE);
                AttackGluth(who->ToCreature());
                me->MonsterTextEmote(EMOTE_NEARBY, 0, true);
            }
            else if (!me->IsInCombatActive() && who->ToPlayer() && who->GetPositionZ() < me->GetPositionZ() + 10.0f && me->IsWithinLOSInMap(who))
                AttackStart(who);
            else
                BossAI::MoveInLineOfSight(who);
        }

		void JustDied(Unit* /*killer*/)
		{
			_JustDied();
		}

        void EnterToBattle(Unit* /*who*/)
        {
            _EnterToBattle();

            events.ScheduleEvent(EVENT_WOUND, 10000);
            events.ScheduleEvent(EVENT_ENRAGE, 15000);
            events.ScheduleEvent(EVENT_DECIMATE, RAID_MODE(120000, 90000));
            events.ScheduleEvent(EVENT_BERSERK, RAID_MODE(8*60000, 7*60000));
            events.ScheduleEvent(EVENT_SUMMON, 15000);
        }

        void JustSummoned(Creature* summon)
        {
            if (summon->GetEntry() == NPC_ZOMBIE)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
                    summon->AI()->AttackStart(target);

                me->SetInCombatWith(summon);
                summon->SetInCombatWith(me);
                me->AddThreat(summon, 0.0f);
            }

            summons.Summon(summon);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictimWithGaze() || !CheckInRoom())
                return;

            if (me->GetDistance(me->GetHomePosition()) > 80.0f)
            {
                EnterEvadeMode();
                return;
            }

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_WOUND:
                        DoCastVictim(SPELL_MORTAL_WOUND);
                        events.ScheduleEvent(EVENT_WOUND, 10000);
                        break;
                    case EVENT_ENRAGE:
                        me->MonsterTextEmote(EMOTE_ENRAGE, 0, true);
						DoCast(RAID_MODE(SPELL_ENRAGE_10, SPELL_ENRAGE_25));
                        events.ScheduleEvent(EVENT_ENRAGE, 15000);
                        break;
                    case EVENT_DECIMATE:
						me->MonsterTextEmote(EMOTE_DECIMATE, 0, true);
						DoCastAOE(RAID_MODE(SPELL_DECIMATE_10, SPELL_DECIMATE_25));
                        events.ScheduleEvent(EVENT_DECIMATE, RAID_MODE(120000, 90000));
                        for (std::list<uint64>::const_iterator itr = summons.begin(); itr != summons.end(); ++itr)
                        {
                            Creature* minion = Unit::GetCreature(*me, *itr);

                            if (minion && minion->IsAlive())
                                AttackGluth(minion);
                        }
                        break;
                    case EVENT_BERSERK:
                        DoCast(me, SPELL_BERSERK, true);
                        break;
                    case EVENT_SUMMON:
                        for (int32 i = 0; i < RAID_MODE(1, 2); ++i)
						{
                            DoSummon(NPC_ZOMBIE, PosSummon[RAID_MODE(0, rand() % 3)], 10*MINUTE*IN_MILLISECONDS, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN);
						}
                        events.ScheduleEvent(EVENT_SUMMON, 10000);
                        break;
                }
            }

            if (me->GetVictim() && me->GetVictim()->GetEntry() == NPC_ZOMBIE)
            {
                if (me->IsWithinMeleeRange(me->GetVictim()))
                {
                    me->Kill(me->GetVictim());
                    me->ModifyHealth(int32(me->CountPctFromMaxHealth(HEALTH_PERCENT_5)));
                }
            }
            else DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_gluthAI(creature);
    }
};

class npc_gluth_zombie : public CreatureScript
{
public:
    npc_gluth_zombie() : CreatureScript("npc_gluth_zombie") { }

    struct npc_gluth_zombieAI : public QuantumBasicAI
    {
        npc_gluth_zombieAI(Creature* creature) : QuantumBasicAI(creature) { }

        void DamageDealt(Unit* victim, uint32 &damage, DamageEffectType /*damagetype*/)
        {
            uint8 stackcount = 0;

            if (victim->HasAura(SPELL_INFECTED_TRIGGERED))
            {
                 if (Aura* infectedAura = victim->GetAura(SPELL_INFECTED_TRIGGERED))
                 {
                     stackcount = infectedAura->GetStackAmount();
                     if (stackcount < 99)
                         infectedAura->SetStackAmount(stackcount + 1);
                     infectedAura->SetDuration(infectedAura->GetMaxDuration());
                 }
            }
            else DoCast(victim, SPELL_INFECTED_TRIGGERED);
        }

		void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
		{
			if (spell->Id == SPELL_DECIMATE_10 || spell->Id == SPELL_DECIMATE_25)
			{
				me->SetHealth(RAID_MODE(25200, 50400));
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gluth_zombieAI(creature);
    }
};

void AddSC_boss_gluth()
{
    new boss_gluth();
    new npc_gluth_zombie();
}