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
    SPELL_CORROSIVE_SALIVA         = 54527,
    SPELL_OPTIC_LINK               = 54396,
    SPELL_RAY_OF_SUFFERING_5N      = 54442,
    SPELL_RAY_OF_SUFFERING_5H      = 59524,
    SPELL_RAY_OF_SUFFERING_TRIGGER = 54417,
    SPELL_RAY_OF_PAIN_5N           = 54438,
    SPELL_RAY_OF_PAIN_5H           = 59523,
    SPELL_RAY_OF_PAIN_TRIGGER      = 54416,
};

class boss_moragg : public CreatureScript
{
public:
    boss_moragg() : CreatureScript("boss_moragg") { }

    struct boss_moraggAI : public QuantumBasicAI
    {
        boss_moraggAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 OpticLinkTimer;
        uint32 CorrosiveSalivaTimer;
        uint32 RaySufferTimer;
        uint32 RayPainTimer;

        InstanceScript* instance;

        void Reset()
        {
            OpticLinkTimer = 10000;
            CorrosiveSalivaTimer = 5000;
            RaySufferTimer = DUNGEON_MODE(5000, 3000);
            RayPainTimer = DUNGEON_MODE(6500, 4500);

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
			if (GameObject* Mcell = instance->instance->GetGameObject(instance->GetData64(DATA_MORAGG_CELL)))
			{
				if (Mcell->GetGoState() == GO_STATE_READY)
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

            if (RaySufferTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
                    DoCast(target, SPELL_RAY_OF_SUFFERING_TRIGGER, true);
					RaySufferTimer = DUNGEON_MODE(5000, 3000);
				}
            }
			else RaySufferTimer -= diff;

            if (RayPainTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
                    DoCast(target, SPELL_RAY_OF_PAIN_TRIGGER, true);
					RayPainTimer = DUNGEON_MODE(6500, 4500);
				}
            }
			else RayPainTimer -= diff;

            if (OpticLinkTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
                    DoCast(target, SPELL_OPTIC_LINK);
					OpticLinkTimer = 15000;
				}
            }
			else OpticLinkTimer -= diff;

            if (CorrosiveSalivaTimer <= diff)
            {
				DoCastVictim(SPELL_CORROSIVE_SALIVA);
				CorrosiveSalivaTimer = 10000;
            }
			else CorrosiveSalivaTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_moraggAI(creature);
    }
};

void AddSC_boss_moragg()
{
    new boss_moragg();
}