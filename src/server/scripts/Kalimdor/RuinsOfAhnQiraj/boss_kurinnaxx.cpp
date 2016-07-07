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
#include "CreatureTextMgr.h"

enum Spells
{
    SPELL_MORTAL_WOUND   = 25646,
    SPELL_SAND_TRAP      = 25648,
	SPELL_SAND_TRAP_PROC = 25656,
    SPELL_ENRAGE         = 26527,
    SPELL_SUMMON_PLAYER  = 26446,
    SPELL_TRASH          = 3417,
    SPELL_WIDE_SLASH     = 25814,
};

enum Events
{
    EVENT_MORTAL_WOUND = 1,
    EVENT_SAND_TRAP    = 2,
    EVENT_TRASH        = 3,
    EVENT_WIDE_SLASH   = 4,
};

enum Texts
{
	SAY_KURINAXX_DEATH = 5,
};

class boss_kurinnaxx : public CreatureScript
{
public:
	boss_kurinnaxx() : CreatureScript("boss_kurinnaxx") { }

	struct boss_kurinnaxxAI : public BossAI
	{
		boss_kurinnaxxAI(Creature* creature) : BossAI(creature, DATA_KURINNAXX) {}

		void Reset()
		{
			_Reset();

			events.ScheduleEvent(EVENT_TRASH, 1*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_MORTAL_WOUND, 8*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_SAND_TRAP, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_WIDE_SLASH, 11*IN_MILLISECONDS);

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			_EnterToBattle();

			KaldoreiAttackEvent();
		}

		void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
		{
			if (HealthBelowPct(HEALTH_PERCENT_30))
			{
				if (!me->HasAuraEffect(SPELL_ENRAGE, 0))
				{
					DoCast(me, SPELL_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
            }
		}

		void JustDied(Unit* /*killer*/)
		{
			_JustDied();

			if (Creature* ossirian = me->GetMap()->GetCreature(instance->GetData64(DATA_OSSIRIAN)))
				sCreatureTextMgr->SendChat(ossirian, SAY_KURINAXX_DEATH, 0, CHAT_MSG_ADDON, LANG_ADDON, TEXT_RANGE_ZONE);
		}

		void KaldoreiAttackEvent()
		{
			std::list<Creature*> KaldoreiList;
			me->GetCreatureListWithEntryInGrid(KaldoreiList, NPC_KALDOREI_ELITE, 150.0f);

			if (!KaldoreiList.empty())
			{
				for (std::list<Creature*>::const_iterator itr = KaldoreiList.begin(); itr != KaldoreiList.end(); ++itr)
				{
					if (Creature* kaldorei = *itr)
					{
						kaldorei->AI()->AttackStart(me);
						kaldorei->CastSpell(kaldorei, 58854, true); // need search original visual spell
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
				    case EVENT_MORTAL_WOUND:
						DoCastVictim(SPELL_MORTAL_WOUND);
						events.ScheduleEvent(EVENT_MORTAL_WOUND, 8*IN_MILLISECONDS);
						break;
					case EVENT_SAND_TRAP:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
						{
							if (Creature* trap = me->SummonCreature(NPC_SAND_TRAP_TRIGGER, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 20*IN_MILLISECONDS))
								trap->SetLevel(target->GetCurrentLevel());
						}
						events.ScheduleEvent(EVENT_SAND_TRAP, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
						break;
					case EVENT_WIDE_SLASH:
						DoCast(me, SPELL_WIDE_SLASH);
						events.ScheduleEvent(EVENT_WIDE_SLASH, 11*IN_MILLISECONDS);
						break;
					case EVENT_TRASH:
						DoCast(me, SPELL_TRASH);
						events.ScheduleEvent(EVENT_TRASH, 16*IN_MILLISECONDS);
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
		return new boss_kurinnaxxAI(creature);
	}
};

class npc_sand_trap : public CreatureScript
{
public:
    npc_sand_trap() : CreatureScript("npc_sand_trap") {}

    struct npc_sand_trapAI : public QuantumBasicAI
    {
		npc_sand_trapAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			me->SummonGameObject(GO_SAND_TRAP, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), 0, 0, 0, 0, 20*IN_MILLISECONDS);

			DoCast(me, SPELL_SAND_TRAP_PROC);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sand_trapAI(creature);
    }
};

void AddSC_boss_kurinnaxx()
{
    new boss_kurinnaxx();
	new npc_sand_trap();
}