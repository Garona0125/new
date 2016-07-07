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
#include "culling_of_stratholme.h"

enum Spells
{
    SPELL_CURSE_OF_EXERTION  = 52772,
    SPELL_TIME_WARP          = 52766,
    SPELL_TIME_STOP          = 58848, 
    SPELL_WOUNDING_STRIKE_5N = 52771,
    SPELL_WOUNDING_STRIKE_5H = 58830,
    SPELL_TIME_STEP_5N       = 52737,
    SPELL_TIME_STEP_5H       = 58829,
};

enum Texts
{
    SAY_INTRO        = -1595000,
    SAY_AGGRO        = -1595001,
    SAY_TIME_WARP_1  = -1595002,
    SAY_TIME_WARP_2  = -1595003,
    SAY_TIME_WARP_3  = -1595004,
    SAY_SLAY_1       = -1595005,
    SAY_SLAY_2       = -1595006,
    SAY_SLAY_3       = -1595007,
    SAY_DEATH        = -1595008,
};

class boss_chrono_lord_epoch : public CreatureScript
{
public:
    boss_chrono_lord_epoch() : CreatureScript("boss_chrono_lord_epoch") { }

    struct boss_chrono_lord_epochAI : public QuantumBasicAI
    {
        boss_chrono_lord_epochAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
		}

        uint8 StepCount;

        uint32 WoundingStrikeTimer;
        uint32 TimeWarpTimer;
        uint32 TimeStopTimer;
        uint32 CurseOfExertionTimer;

        InstanceScript* instance;

        bool TimeWarping;

        void Reset()
        {
            CurseOfExertionTimer = 2000;
            TimeWarpTimer = 5000;
            TimeStopTimer = 7000;
            WoundingStrikeTimer = 7500;

            TimeWarping = false;

            StepCount = 0;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			instance->SetData(DATA_EPOCH_EVENT, NOT_STARTED);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

			instance->SetData(DATA_EPOCH_EVENT, IN_PROGRESS);
        }

		void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
        }

		void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

			instance->SetData(DATA_EPOCH_EVENT, DONE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (TimeWarping)
            {
                if (TimeWarpTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0, -5, true))
                        DoCast(target, DUNGEON_MODE(SPELL_TIME_STEP_5N, SPELL_TIME_STEP_5H));

                    TimeWarpTimer = 500;
                    ++StepCount;

                    if (StepCount >= 6)
                    {
                        TimeWarping = false;
                        TimeWarpTimer = 15000;
                    }

                }
				else TimeWarpTimer -= diff;

                return;
            }

            if (CurseOfExertionTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
                    DoCast(target, SPELL_CURSE_OF_EXERTION);
					CurseOfExertionTimer = 5000;
				}
            }
			else CurseOfExertionTimer -= diff;

            if (WoundingStrikeTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_WOUNDING_STRIKE_5N, SPELL_WOUNDING_STRIKE_5H));
                WoundingStrikeTimer = 7000;
            }
			else WoundingStrikeTimer -= diff;

            if (IsHeroic())
            {
                if (TimeStopTimer <= diff)
                {
                    DoCastAOE(SPELL_TIME_STOP);
                    TimeStopTimer = 10000;
                }
				else TimeStopTimer -= diff;
            }

            if (TimeWarpTimer <= diff)
            {
                DoSendQuantumText(RAND(SAY_TIME_WARP_1, SAY_TIME_WARP_2, SAY_TIME_WARP_3), me);
                DoCastAOE(SPELL_TIME_WARP);
                TimeWarpTimer = 500;
                TimeWarping = true;
                StepCount = 0;
            }
			else TimeWarpTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_chrono_lord_epochAI(creature);
    }
};

void AddSC_boss_chrono_lord_epoch()
{
    new boss_chrono_lord_epoch();
}