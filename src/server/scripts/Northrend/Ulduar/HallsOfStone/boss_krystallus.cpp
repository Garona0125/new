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
#include "halls_of_stone.h"

enum Spells
{
    SPELL_BOULDER_TOSS_5N       = 50843,
	SPELL_BOULDER_TOSS_5H       = 59742,
    SPELL_GROUND_SPIKE          = 59750,
    SPELL_GROUND_SLAM           = 50827,
    SPELL_GROUND_SLAM_TRIGGERED = 50833,
    SPELL_SHATTER_5N            = 50810,
	SPELL_SHATTER_5H            = 61546,
    SPELL_SHATTER_EFFECT_ONE    = 50811,
	SPELL_SHATTER_EFFECT_TWO    = 61547,
    SPELL_STONED                = 50812,
    SPELL_STOMP_5N              = 50868,
    SPELL_STOMP_5H              = 59744,
};

enum Yells
{
    SAY_AGGRO    = -1599007,
    SAY_SLAY     = -1599008,
    SAY_DEATH    = -1599009,
    SAY_SHATTER  = -1599010,
};

enum Events
{
    EVENT_BOULDER_TOSS = 1,
    EVENT_GROUND_SPIKE = 2,
    EVENT_GROUND_SLAM  = 3,
    EVENT_STOMP        = 4,
    EVENT_SHATTER      = 5,
};

class boss_krystallus : public CreatureScript
{
public:
    boss_krystallus() : CreatureScript("boss_krystallus") { }

    struct boss_krystallusAI : public QuantumBasicAI
    {
        boss_krystallusAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

		InstanceScript* instance;
		EventMap events;

        void Reset()
        {
            events.Reset();
			
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			instance->SetData(DATA_KRYSTALLUS_EVENT, NOT_STARTED);
        }

        void EnterToBattle(Unit* who)
        {
			if (!me->IsInEvadeMode() && (who->IsPet() || who->IsHunterPet()))
			{
				EnterEvadeMode();
				return;
			}

            DoSendQuantumText(SAY_AGGRO, me);

            events.ScheduleEvent(EVENT_BOULDER_TOSS, urand(3000, 9000));
            events.ScheduleEvent(EVENT_GROUND_SLAM, urand(20000, 23000));
            events.ScheduleEvent(EVENT_STOMP, urand(15000, 20000));

            if (IsHeroic())
                events.ScheduleEvent(EVENT_GROUND_SPIKE, urand(6000, 11000));

			instance->SetData(DATA_KRYSTALLUS_EVENT, IN_PROGRESS);
        }

		void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(SAY_SLAY, me);
        }

		void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

			instance->SetData(DATA_KRYSTALLUS_EVENT, DONE);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STONED);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_GROUND_SLAM);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_GROUND_SLAM_TRIGGERED);
        }

		void Shattering()
		{
			Map::PlayerList const& Players = me->GetMap()->GetPlayers();
			for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
			{
				if (Player* player = itr->getSource())
				{
					if (player->IsWithinDistInMap(me, 55.0f))
						me->DealDamage(player, 12500, 0, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, 0, false);
					if (IsHeroic())
						me->DealDamage(player, 25000, 0, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, 0, false);
				}
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
                    case EVENT_BOULDER_TOSS:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 50.0f, true))
                            DoCast(target, DUNGEON_MODE(SPELL_BOULDER_TOSS_5N, SPELL_BOULDER_TOSS_5H));
                        events.ScheduleEvent(EVENT_BOULDER_TOSS, urand(9000, 15000));
                        break;
					case EVENT_GROUND_SPIKE:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
                            DoCast(target, SPELL_GROUND_SPIKE);

                        events.ScheduleEvent(EVENT_GROUND_SPIKE, urand(12000, 17000));
                        break;
                    case EVENT_GROUND_SLAM:
                        DoCast(me, SPELL_GROUND_SLAM);
						 events.ScheduleEvent(EVENT_SHATTER, 10000);
						 events.ScheduleEvent(EVENT_GROUND_SLAM, urand(15000, 18000));
                        break;
					case EVENT_STOMP:
                        DoCast(me, DUNGEON_MODE(SPELL_STOMP_5N, SPELL_STOMP_5H));
                        events.ScheduleEvent(EVENT_STOMP, urand(20000, 29000));
                        break;
                    case EVENT_SHATTER:
                        DoCast(me, DUNGEON_MODE(SPELL_SHATTER_5N, SPELL_SHATTER_5H), true);
						Shattering();
                        DoSendQuantumText(SAY_SHATTER, me);
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
        return new boss_krystallusAI(creature);
    }
};

class spell_krystallus_shatter : public SpellScriptLoader
{
    public:
        spell_krystallus_shatter() : SpellScriptLoader("spell_krystallus_shatter") { }

        class spell_krystallus_shatter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_krystallus_shatter_SpellScript);

			bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_STONED))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_SHATTER_EFFECT_ONE))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
				Unit* target = GetHitUnit();

				target->RemoveAurasDueToSpell(SPELL_STONED);
				target->CastSpell(target, SPELL_SHATTER_EFFECT_ONE, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_krystallus_shatter_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_krystallus_shatter_SpellScript();
        }
};

void AddSC_boss_krystallus()
{
    new boss_krystallus();
	new spell_krystallus_shatter();
}