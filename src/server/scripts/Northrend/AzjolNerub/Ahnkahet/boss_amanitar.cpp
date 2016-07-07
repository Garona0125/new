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
#include "ahnkahet.h"

enum Spells
{
    SPELL_BASH                                    = 57094, // Victim
    SPELL_ENTANGLING_ROOTS                        = 57095, // Random Victim 100Y
    SPELL_MINI                                    = 57055, // Self
    SPELL_VENOM_BOLT_VOLLEY                       = 57088, // Random Victim 100Y
    SPELL_HEALTHY_MUSHROOM_POTENT_FUNGUS          = 56648, // Killer 3Y
    SPELL_POISONOUS_MUSHROOM_POISON_CLOUD         = 57061, // Self - Duration 8 Sec
    SPELL_POISONOUS_MUSHROOM_VISUAL_AREA          = 61566, // Self
    SPELL_POISONOUS_MUSHROOM_VISUAL_AURA          = 56741, // Self
    SPELL_PUTRID_MUSHROOM                         = 31690, // To make the mushrooms visible
    SPELL_POWER_MUSHROOM_VISUAL_AURA              = 56740,
};

enum event
{
    EVENT_SPAWN             = 1,
    EVENT_MINI              = 2,
    EVENT_ENTANGLING_ROOTS  = 3,
    EVENT_BASH              = 4,
    EVENT_VENOM_BOLT_VOLLEY = 5,
    EVENT_AURA              = 6,
};

class boss_amanitar : public CreatureScript
{
public:
    boss_amanitar() : CreatureScript("boss_amanitar") {}

    struct boss_amanitarAI : public BossAI
    {
        boss_amanitarAI(Creature* creature) : BossAI(creature, DATA_AMANITAR) {}

        void Reset()
        {
            _Reset();

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            me->SetMeleeDamageSchool(SPELL_SCHOOL_NATURE);
            me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);            
            summons.DespawnAll();

			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MINI);
			instance->SetData(DATA_AMANITAR_EVENT, NOT_STARTED);
        }

        void JustDied(Unit* /*killer*/)
        {
			_JustDied();

			instance->SetData(DATA_AMANITAR_EVENT, DONE);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MINI);

			summons.DespawnAll();
        }

        void EnterToBattle(Unit* /*who*/)
        {
            _EnterToBattle();

            events.ScheduleEvent(EVENT_ENTANGLING_ROOTS, urand(5,9)*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_BASH, urand(10,14)*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_VENOM_BOLT_VOLLEY, urand(15,20)*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_MINI, urand(12,18)*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_SPAWN, 5 *IN_MILLISECONDS);

            me->SetInCombatWithZone();

			instance->SetData(DATA_AMANITAR_EVENT, IN_PROGRESS);
        }

        void SpawnAdds()
        {
            uint8 u = 0;

            for (uint8 i = 0; i < 30; ++i)
            {
                Position pos;
                me->GetPosition(&pos);
                me->GetRandomNearPosition(pos, 30.0f);
                pos.m_positionZ = me->GetMap()->GetHeight(pos.GetPositionX(), pos.GetPositionY(), MAX_HEIGHT) + 2.0f;

                if (Creature* trigger = me->SummonCreature(NPC_TRIGGER, pos))
                {
                    Creature* temp1 = trigger->FindCreatureWithDistance(NPC_HEALTHY_MUSHROOM, 4.0f, true);
                    Creature* temp2 = trigger->FindCreatureWithDistance(NPC_POISONOUS_MUSHROOM, 4.0f, true);
                    if (temp1 || temp2)
                    {
                        trigger->DisappearAndDie();
                    }
                    else
                    {
                        u = 1 - u;
                        trigger->DisappearAndDie();
                        me->SummonCreature(u > 0 ? NPC_POISONOUS_MUSHROOM : NPC_HEALTHY_MUSHROOM, pos, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60*IN_MILLISECONDS);
                    }
                }
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
                    case EVENT_SPAWN:
                        SpawnAdds();
                        events.ScheduleEvent(EVENT_SPAWN, 20*IN_MILLISECONDS);
                        break;
                    case EVENT_MINI:
                        DoCast(SPELL_MINI);
                        events.ScheduleEvent(EVENT_MINI, urand(25,30)*IN_MILLISECONDS);
                        break;
                    case EVENT_ENTANGLING_ROOTS:
                        DoCast(SelectTarget(TARGET_RANDOM,0, 100, true),SPELL_ENTANGLING_ROOTS,true);
                        events.ScheduleEvent(EVENT_ENTANGLING_ROOTS, urand(10,15)*IN_MILLISECONDS);
                        break;
                    case EVENT_BASH:
                        DoCastVictim(SPELL_BASH);
                        events.ScheduleEvent(EVENT_BASH, urand(7,12)*IN_MILLISECONDS);
                        break;
                    case EVENT_VENOM_BOLT_VOLLEY:
                        DoCast(SelectTarget(TARGET_RANDOM,0, 100, true),SPELL_VENOM_BOLT_VOLLEY,true);
                        events.ScheduleEvent(EVENT_VENOM_BOLT_VOLLEY, urand(18,22)*IN_MILLISECONDS);
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
        return new boss_amanitarAI(creature);
    }
};

class npc_poisonous_mushroom : public CreatureScript
{
public:
    npc_poisonous_mushroom() : CreatureScript("npc_poisonous_mushroom") { }

    struct npc_poisonous_mushroomAI : public QuantumBasicAI
    {
        npc_poisonous_mushroomAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        EventMap events;

        void Reset()
        {
            events.Reset();
            events.ScheduleEvent(EVENT_AURA, 1*IN_MILLISECONDS);

            me->SetDisplayId(me->GetCreatureTemplate()->Modelid2);
            DoCast(SPELL_PUTRID_MUSHROOM);

            if (me->GetEntry() == NPC_POISONOUS_MUSHROOM)
                DoCast(SPELL_POISONOUS_MUSHROOM_VISUAL_AURA);
            else
                DoCast(SPELL_POWER_MUSHROOM_VISUAL_AURA);
        }

        void DamageTaken(Unit* /*attacker*/, uint32 &damage)
        {
            if (damage >= me->GetHealth() && me->GetEntry() == NPC_HEALTHY_MUSHROOM)
                DoCast(me, SPELL_HEALTHY_MUSHROOM_POTENT_FUNGUS, true);
        }

        void EnterToBattle(Unit* /*who*/){}

        void AttackStart(Unit* /*victim*/){}

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
                    case EVENT_AURA:
                        if (me->GetEntry() == NPC_POISONOUS_MUSHROOM)
                        {
                            DoCast(me, SPELL_POISONOUS_MUSHROOM_VISUAL_AREA, true);
                            DoCast(me, SPELL_POISONOUS_MUSHROOM_POISON_CLOUD);
                        }
                        events.ScheduleEvent(EVENT_AURA, 7*IN_MILLISECONDS);
                        break;
                    default:
                        break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_poisonous_mushroomAI(creature);
    }
};

void AddSC_boss_amanitar()
{
    new boss_amanitar();
    new npc_poisonous_mushroom();
}