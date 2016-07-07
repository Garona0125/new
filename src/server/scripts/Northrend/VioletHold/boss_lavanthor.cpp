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
#include "violet_hold.h"

enum Spells
{
	SPELL_CAUTERIZING_FLAMES = 59466,
	SPELL_FIREBOLT_5N        = 54235,
	SPELL_FIREBOLT_5H        = 59468,
	SPELL_FLAME_BREATH_5N    = 54282,
	SPELL_FLAME_BREATH_5H    = 59469,
	SPELL_LAVA_BURN_5N       = 54249,
	SPELL_LAVA_BURN_5H       = 59594,
};

class boss_lavanthor : public CreatureScript
{
public:
    boss_lavanthor() : CreatureScript("boss_lavanthor") { }

    struct boss_lavanthorAI : public QuantumBasicAI
    {
        boss_lavanthorAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 FireboltTimer;
        uint32 FlameBreathTimer;
        uint32 LavaBurnTimer;
        uint32 CauterizingFlamesTimer;

        InstanceScript* instance;

        void Reset()
        {
            FireboltTimer = 500;
            FlameBreathTimer = 2000;
            LavaBurnTimer = 4000;
            CauterizingFlamesTimer = 5000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			if (instance->GetData(DATA_WAVE_COUNT) == 6)
				instance->SetData(DATA_1ST_BOSS_EVENT, NOT_STARTED);
			else if (instance->GetData(DATA_WAVE_COUNT) == 12)
				instance->SetData(DATA_2ND_BOSS_EVENT, NOT_STARTED);
        }

		void MoveInLineOfSight(Unit* /*who*/) {}

        void AttackStart(Unit* who)
        {
			if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER || UNIT_FLAG_NON_ATTACKABLE))
                return;

            if (me->Attack(who, true))
            {
                me->AddThreat(who, 0.0f);
                me->SetInCombatWith(who);
                who->SetInCombatWith(me);
                DoStartMovement(who);
            }
        }

		void EnterToBattle(Unit* /*who*/)
        {
            if (GameObject* Lcell = instance->instance->GetGameObject(instance->GetData64(DATA_LAVANTHOR_CELL)))
			{
				if (Lcell->GetGoState() == GO_STATE_READY)
				{
					EnterEvadeMode();
					return;
				}

                if (instance->GetData(DATA_WAVE_COUNT) == 6)
                    instance->SetData(DATA_1ST_BOSS_EVENT, IN_PROGRESS);
                else if (instance->GetData(DATA_WAVE_COUNT) == 12)
                    instance->SetData(DATA_2ND_BOSS_EVENT, IN_PROGRESS);
            }
        }

		void JustDied(Unit* /*killer*/)
        {
			if (instance->GetData(DATA_WAVE_COUNT) == 6)
			{
				if (IsHeroic() && instance->GetData(DATA_1ST_BOSS_EVENT) == DONE)
					me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

				instance->SetData(DATA_1ST_BOSS_EVENT, DONE);
				instance->SetData(DATA_WAVE_COUNT, 7);
			}
			else if (instance->GetData(DATA_WAVE_COUNT) == 12)
			{
				if (IsHeroic() && instance->GetData(DATA_2ND_BOSS_EVENT) == DONE)
					me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

				instance->SetData(DATA_2ND_BOSS_EVENT, DONE);
				instance->SetData(DATA_WAVE_COUNT, 13);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (FlameBreathTimer <= diff)
            {
				DoCastVictim(DUNGEON_MODE(SPELL_FLAME_BREATH_5N, SPELL_FLAME_BREATH_5H));
				FlameBreathTimer = urand(3000, 4000);
            }
			else FlameBreathTimer -= diff;

            if (LavaBurnTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_LAVA_BURN_5N, SPELL_LAVA_BURN_5H), true);
					LavaBurnTimer = urand(5000, 6000);
				}
            }
			else LavaBurnTimer -= diff;

			if (CauterizingFlamesTimer <= diff && IsHeroic())
			{
				DoCastAOE(SPELL_CAUTERIZING_FLAMES);
				CauterizingFlamesTimer = urand(7000, 8000);
			}
			else CauterizingFlamesTimer -= diff;

            if (FireboltTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FIREBOLT_5N, SPELL_FIREBOLT_5H), true);
					FireboltTimer = urand(9000, 10000);
				}
            }
			else FireboltTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_lavanthorAI (creature);
    }
};

void AddSC_boss_lavanthor()
{
    new boss_lavanthor();
}