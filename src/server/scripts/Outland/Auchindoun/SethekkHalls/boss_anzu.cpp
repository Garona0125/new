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
#include "sethekk_halls.h"

enum Texts
{
    SAY_ANZU_SUMMON_BROOD       = -1420066,
    SAY_ANZU_SPELL_BOMB_1       = -1420067,
	SAY_ANZU_SPELL_BOMB_2       = -1420068,
};

enum Spells
{
	SPELL_MARK_OF_ANZU          = 40366,
    SPELL_PARALYZING_SCREECH    = 40184,
    SPELL_SPELL_BOMB            = 40303,
    SPELL_CYCLONE_OF_FEATHERS   = 40321,
    SPELL_BANISH_SELF           = 42354,
    SPELL_FLESH_RIP             = 40199,
	SPELL_RED_GHOST_VISUAL      = 39205,
	SPELL_FEATHER_CYCLONE       = 43119,
};

enum Events
{
	EVENT_MARK_OF_ANZU          = 1,
    EVENT_PARALYZING_SCREECH    = 2,
    EVENT_CYCLONE_OF_FEATHERS   = 3,
};

Position const PosSummonBrood[7] =
{
    { -118.1717f, 284.5299f, 121.2287f, 2.775074f },
    { -98.15528f, 293.4469f, 109.2385f, 0.174533f },
    { -99.70160f, 270.1699f, 98.27389f, 6.178465f },
    { -69.25543f, 303.0768f, 97.84479f, 5.532694f },
    { -87.59662f, 263.5181f, 92.70478f, 1.658063f },
    { -73.54323f, 276.6267f, 94.25807f, 2.802979f },
    { -81.70527f, 280.8776f, 44.58830f, 0.526849f },
};

class boss_anzu : public CreatureScript
{
    public:
        boss_anzu() : CreatureScript("boss_anzu") { }

        struct boss_anzuAI : public QuantumBasicAI
        {
            boss_anzuAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

			bool Under33Percent;
			bool Under66Percent;

			EventMap events;
			SummonList Summons;

            void Reset()
            {
                events.Reset();

				Under33Percent = false;
				Under66Percent = false;

				Summons.DespawnAll();

				// Delete Claw after summoning Anzu
				if (GameObject* claw = me->FindGameobjectWithDistance(GO_RAVEN_CLAW, 50.0f))
				{
					claw->Delete();
					claw->GetRespawnDelay();
				}

				DoCast(me, SPELL_UNIT_DETECTION_ALL);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

            void EnterToBattle(Unit* /*who*/)
            {
				events.ScheduleEvent(EVENT_MARK_OF_ANZU, 1000);
                events.ScheduleEvent(EVENT_CYCLONE_OF_FEATHERS, 5000);
				events.ScheduleEvent(EVENT_PARALYZING_SCREECH, 14000);
            }

            void JustDied(Unit* /*killer*/)
			{
				Summons.DespawnAll();
			}

			void JustSummoned(Creature* summon)
			{
				if (summon->GetEntry() == NPC_BROOD_OF_ANZU)
				{
					Summons.Summon(summon);
					Summons.DoZoneInCombat();
				}
			}

            void DamageTaken(Unit* /*killer*/, uint32 &damage)
            {
                if (me->HealthBelowPctDamaged(66, damage) && !Under66Percent)
                {
                    Under66Percent = true;
                    DoSendQuantumText(SAY_ANZU_SUMMON_BROOD, me);
                    SummonBroodHawks();
                }

				if (me->HealthBelowPctDamaged(33, damage) && !Under33Percent)
                {
                    Under33Percent = true;
                    DoSendQuantumText(SAY_ANZU_SUMMON_BROOD, me);
                    SummonBroodHawks();
                }
            }

			void SummonBroodHawks()
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_SPELL_BOMB);
						DoSendQuantumText(RAND(SAY_ANZU_SPELL_BOMB_1, SAY_ANZU_SPELL_BOMB_2), me);
					}
				}

				DoCast(me, SPELL_BANISH_SELF, true);

				for (uint8 i = 0; i < 7; i++)
					me->SummonCreature(NPC_BROOD_OF_ANZU, PosSummonBrood[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 46000);
			}

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
					    case EVENT_MARK_OF_ANZU:
							if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
							{
								DoCast(target, SPELL_MARK_OF_ANZU);
							}
							events.ScheduleEvent(EVENT_MARK_OF_ANZU, 4000);
							break;
						case EVENT_CYCLONE_OF_FEATHERS:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
							{
                                DoCast(target, SPELL_CYCLONE_OF_FEATHERS);
							}
                            events.ScheduleEvent(EVENT_CYCLONE_OF_FEATHERS, 21000);
                            break;
                        case EVENT_PARALYZING_SCREECH:
                            DoCastVictim(SPELL_PARALYZING_SCREECH);
                            events.ScheduleEvent(EVENT_PARALYZING_SCREECH, 26000);
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
            return new boss_anzuAI(creature);
        }
};

void AddSC_boss_anzu()
{
    new boss_anzu();
}