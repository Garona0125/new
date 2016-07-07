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
#include "halls_of_stone.h"

enum Spells
{
    SPELL_PARTING_SORROW     = 59723,
    SPELL_STORM_OF_GRIEF_5N  = 50752,
    SPELL_STORM_OF_GRIEF_5H  = 59772,
    SPELL_SHOCK_OF_SORROW_5N = 50760,
    SPELL_SHOCK_OF_SORROW_5H = 59726,
    SPELL_PILLAR_OF_WOE_5N   = 50761,
    SPELL_PILLAR_OF_WOE_5H   = 59727,
};

enum Yells
{
    SAY_AGGRO   = -1599000,
    SAY_SLAY_1  = -1599001,
    SAY_SLAY_2  = -1599002,
    SAY_SLAY_3  = -1599003,
    SAY_SLAY_4  = -1599004,
    SAY_DEATH   = -1599005,
    SAY_STUN    = -1599006,
};

enum Achievements
{
    ACHIEV_GOOD_GRIEF_START_EVENT = 20383,
};

class boss_maiden_of_grief : public CreatureScript
{
public:
    boss_maiden_of_grief() : CreatureScript("boss_maiden_of_grief") { }

    struct boss_maiden_of_griefAI : public QuantumBasicAI
    {
        boss_maiden_of_griefAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 PartingSorrowTimer;
        uint32 StormOfGriefTimer;
        uint32 ShockOfSorrowTimer;
        uint32 PillarOfWoeTimer;

        void Reset()
        {
            PartingSorrowTimer = 10000 + rand()%5000;
            StormOfGriefTimer = 10000;
            ShockOfSorrowTimer = 20000+rand()%5000;
            PillarOfWoeTimer = 5000 + rand()%10000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			instance->SetData(DATA_MAIDEN_OF_GRIEF_EVENT, NOT_STARTED);
			instance->DoStopTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_GOOD_GRIEF_START_EVENT);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

			if (GameObject* door = instance->instance->GetGameObject(instance->GetData64(DATA_MAIDEN_DOOR)))
			{
				if (door->GetGoState() == GO_STATE_READY)
				{
					EnterEvadeMode();
					return;
				}
			}

			instance->SetData(DATA_MAIDEN_OF_GRIEF_EVENT, IN_PROGRESS);
			instance->DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_GOOD_GRIEF_START_EVENT);
        }

		void KilledUnit(Unit* victim)
        {
            if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3, SAY_SLAY_4), me);
        }

		void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

			instance->SetData(DATA_MAIDEN_OF_GRIEF_EVENT, DONE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PartingSorrowTimer <= diff && IsHeroic())
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_PARTING_SORROW);
					PartingSorrowTimer = 10000 + rand()%7000;
				}
			}
			else PartingSorrowTimer -= diff;

            if (StormOfGriefTimer <= diff)
            {
				DoCastAOE(DUNGEON_MODE(SPELL_STORM_OF_GRIEF_5N, SPELL_STORM_OF_GRIEF_5H), true);
				StormOfGriefTimer = 15000 + rand()%5000;
            }
			else StormOfGriefTimer -= diff;

            if (ShockOfSorrowTimer <= diff)
            {
				DoSendQuantumText(SAY_STUN, me);
				DoCastAOE(DUNGEON_MODE(SPELL_SHOCK_OF_SORROW_5N, SPELL_SHOCK_OF_SORROW_5H));
				ShockOfSorrowTimer = 20000 + rand()%10000;
            }
			else ShockOfSorrowTimer -= diff;

            if (PillarOfWoeTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
					DoCast(target, DUNGEON_MODE(SPELL_PILLAR_OF_WOE_5N, SPELL_PILLAR_OF_WOE_5H));
					PillarOfWoeTimer = 5000 + rand()%20000;
				}
            }
			else PillarOfWoeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_maiden_of_griefAI(creature);
    }
};

void AddSC_boss_maiden_of_grief()
{
    new boss_maiden_of_grief();
}