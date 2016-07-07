/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "ruins_of_ahnqiraj.h"

enum Texts
{
    // The time of our retribution is at hand! Let darkness reign in the hearts of our enemies! Sound: 8645 Emote: 35
    SAY_ANDOROV_INTRO         = -1509003,   // Before for the first wave
    SAY_ANDOROV_ATTACK        = -1509004,   // Beginning the event
    SAY_ANDOROV_WAVE_1        = -1509001,   // When the first wave comes text: Kill first, ask questions later... Incoming! emote: 45 sound: 8653
    SAY_WAVE3                 = -1509005,
    SAY_WAVE4                 = -1509006,
    SAY_WAVE5                 = -1509007,
    SAY_WAVE6                 = -1509008,
    SAY_WAVE7                 = -1509009,
    SAY_INTRO                 = -1509010,
    SAY_UNK1                  = -1509011,
    SAY_UNK2                  = -1509012,
    SAY_UNK3                  = -1509013,
    SAY_DEATH                 = -1509014,
    SAY_CHANGEAGGRO           = -1509015,
    SAY_KILLS_ANDOROV         = -1509016,
    SAY_COMPLETE_QUEST        = -1509017,    // Yell when realm complete quest 8743 for world event
    // Warriors, Captains, continue the fight! Sound: 8640
};

enum Spells
{
    SPELL_DISARM       = 6713,
    SPELL_FRENZY       = 8269,
    SPELL_THUNDERCRASH = 25599,
};

enum Events
{
    EVENT_DISARM        = 1,
    EVENT_THUNDERCRASH  = 2,
    EVENT_CHANGE_AGGRO  = 3,
};

class boss_general_rajaxx : public CreatureScript
{
public:
	boss_general_rajaxx() : CreatureScript("boss_general_rajaxx") { }

	struct boss_general_rajaxxAI : public BossAI
	{
		boss_general_rajaxxAI(Creature* creature) : BossAI(creature, DATA_GENERAL_RAJAXX) {}

		bool Enraged;

		void Reset()
		{
			_Reset();

			Enraged = false;

			events.ScheduleEvent(EVENT_DISARM, 2*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_THUNDERCRASH, 5*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_CHANGE_AGGRO, 18*IN_MILLISECONDS);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*victim*/)
		{
			_EnterToBattle();
		}

		void JustDied(Unit* /*killer*/)
		{
			//SAY_DEATH
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
				    case EVENT_DISARM:
						DoCastVictim(SPELL_DISARM);
						events.ScheduleEvent(EVENT_DISARM, 8*IN_MILLISECONDS);
						break;
					case EVENT_THUNDERCRASH:
						DoCastAOE(SPELL_THUNDERCRASH);
						events.ScheduleEvent(EVENT_THUNDERCRASH, 12*IN_MILLISECONDS);
						break;
					case EVENT_CHANGE_AGGRO:
						if (DoGetThreat(me->GetVictim()))
							DoModifyThreatPercent(me->GetVictim(), -100);
						events.ScheduleEvent(EVENT_CHANGE_AGGRO, 18*IN_MILLISECONDS);
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
		return new boss_general_rajaxxAI(creature);
	}
};

void AddSC_boss_general_rajaxx()
{
    new boss_general_rajaxx();
}