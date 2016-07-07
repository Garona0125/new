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
#include "gundrak.h"

enum Texts
{
    SAY_AGGRO                  = -1604011,
    SAY_QUAKE                  = -1604012,
    SAY_TRANSFORM              = -1604013,
    SAY_SLAY_1                 = -1604014,
    SAY_SLAY_2                 = -1604015,
    SAY_SLAY_3                 = -1604016,
    SAY_DEATH                  = -1604017,
    EMOTE_TRANSFORM            = -1604018,
    EMOTE_TRANSFORMED          = -1604029,
	EMOTE_ACTIVATE_ALTAR       = -1604031,
};

enum Spells
{
	SPELL_DUAL_WIELD           = 674,
    SPELL_DETERMINED_STAB      = 55104,
    SPELL_GROUND_TREMOR        = 55142,
    SPELL_NUMBING_SHOUT        = 55106,
    SPELL_MOJO_FRENZY          = 55163,
    SPELL_MOJO_FRENZY_HASTE    = 55096,
    SPELL_TRANSFORMATION       = 55098,
    SPELL_DETERMINED_GORE_5N   = 55102,
    SPELL_DETERMINED_GORE_5H   = 59444,
    SPELL_QUAKE                = 55101,
    SPELL_NUMBING_ROAR         = 55100,
};

enum Events
{
    EVENT_QUAKE     = 1,
    EVENT_SHOUT     = 2,
    EVENT_STAB      = 3,
    EVENT_TRANSFORM = 4,
    DATA_LESS_RABI  = 5,
};

class boss_moorabi : public CreatureScript
{
    public:
        boss_moorabi() : CreatureScript("boss_moorabi") { }

        struct boss_moorabiAI : public QuantumBasicAI
        {
            boss_moorabiAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;

            EventMap events;

            bool Transformed;

            void Reset()
            {
                events.Reset();

                Transformed = false;

				DoCast(me, SPELL_DUAL_WIELD, true);

				instance->SetData(DATA_MOORABI_EVENT, NOT_STARTED);

				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                DoSendQuantumText(SAY_AGGRO, me);
                DoCast(me, SPELL_MOJO_FRENZY, true);

                events.ScheduleEvent(EVENT_STAB, 10*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_TRANSFORM, 12*IN_MILLISECONDS);
				events.ScheduleEvent(EVENT_SHOUT, 15*IN_MILLISECONDS);
				events.ScheduleEvent(EVENT_QUAKE, 18*IN_MILLISECONDS);

				instance->SetData(DATA_MOORABI_EVENT, IN_PROGRESS);
            }

            uint32 GetData(uint32 type)
            {
                if (type == DATA_LESS_RABI)
                    return !Transformed ? 1 : 0;

                return 0;
            }

            void JustDied(Unit* /*killer*/)
            {
                DoSendQuantumText(SAY_DEATH, me);
				DoSendQuantumText(EMOTE_ACTIVATE_ALTAR, me);

				instance->SetData(DATA_MOORABI_EVENT, DONE);
            }

            void KilledUnit(Unit* victim)
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
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
                        case EVENT_QUAKE:
                            DoSendQuantumText(SAY_QUAKE, me);
                            DoCastVictim(Transformed ? SPELL_GROUND_TREMOR : SPELL_QUAKE);
                            events.ScheduleEvent(EVENT_QUAKE, urand(10, 15) *IN_MILLISECONDS);
                            break;
                        case EVENT_SHOUT:
                            DoCastVictim(Transformed ? SPELL_NUMBING_ROAR : SPELL_NUMBING_SHOUT);
                            events.ScheduleEvent(EVENT_SHOUT, 10*IN_MILLISECONDS);
                            break;
                        case EVENT_STAB:
                            DoCastVictim(Transformed ? DUNGEON_MODE(SPELL_DETERMINED_GORE_5N, SPELL_DETERMINED_GORE_5H) : SPELL_DETERMINED_STAB);
                            events.ScheduleEvent(EVENT_STAB, 7*IN_MILLISECONDS);
                            break;
                        case EVENT_TRANSFORM:
                            DoSendQuantumText(EMOTE_TRANSFORM, me);
                            DoSendQuantumText(SAY_TRANSFORM, me);
                            DoCast(me, SPELL_TRANSFORMATION);
                            events.ScheduleEvent(EVENT_TRANSFORM, 10*IN_MILLISECONDS);
                            break;
                    }
                }

                if (!Transformed && me->HasAura(SPELL_TRANSFORMATION))
                {
                    Transformed = true;
                    events.CancelEvent(EVENT_TRANSFORM);
                    me->RemoveAura(SPELL_MOJO_FRENZY);
                    DoSendQuantumText(EMOTE_TRANSFORMED, me);
                }

                DoMeleeAttackIfReady();
             }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_moorabiAI(creature);
        }
};

class achievement_less_rabi : public AchievementCriteriaScript
{
    public:
        achievement_less_rabi() : AchievementCriteriaScript("achievement_less_rabi"){}

        bool OnCheck(Player* /*player*/, Unit* target)
        {
            if (!target)
                return false;

            if (Creature* moorabi = target->ToCreature())
                if (moorabi->AI()->GetData(DATA_LESS_RABI))
                    return true;

            return false;
        }
};

void AddSC_boss_moorabi()
{
    new boss_moorabi();
    new achievement_less_rabi();
}