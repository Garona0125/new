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

enum Texts
{
    SAY_AGGRO            = -1608000,
    SAY_SLAY_1           = -1608001,
    SAY_SLAY_2           = -1608002,
    SAY_SLAY_3           = -1608003,
    SAY_DEATH            = -1608004,
    SAY_SPAWN            = -1608005,
    SAY_DISRUPTION       = -1608006,
    SAY_BREATH_ATTACK    = -1608007,
    SAY_SPECIAL_ATTACK_1 = -1608008,
    SAY_SPECIAL_ATTACK_2 = -1608009,
};

enum Spells
{
    SPELL_ARCANE_VACUUM            = 58694,
    SPELL_BLIZZARD_5N              = 58693,
    SPELL_BLIZZARD_5H              = 59369,
    SPELL_MANA_DESTRUCTION         = 59374,
    SPELL_TAIL_SWEEP_5N            = 58690,
    SPELL_TAIL_SWEEP_5H            = 59283,
    SPELL_UNCONTROLLABLE_ENERGY_5N = 58688,
    SPELL_UNCONTROLLABLE_ENERGY_5H = 59281,
    SPELL_TRANSFORM                = 58668,
};

class boss_cyanigosa : public CreatureScript
{
public:
    boss_cyanigosa() : CreatureScript("boss_cyanigosa") { }

    struct boss_cyanigosaAI : public QuantumBasicAI
    {
        boss_cyanigosaAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 ArcaneVacuumTimer;
        uint32 BlizzardTimer;
        uint32 ManaDestructionTimer;
        uint32 TailSweepTimer;
        uint32 UncontrollableEnergyTimer;

        InstanceScript* instance;

        void Reset()
        {
			TailSweepTimer = 2000;
			BlizzardTimer = 3000;
            ArcaneVacuumTimer = 5000;
            ManaDestructionTimer = 7000;
            UncontrollableEnergyTimer = 9000;

			DoSendQuantumText(SAY_SPAWN, me);
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			instance->SetData(DATA_CYANIGOSA_EVENT, NOT_STARTED);
        }

		void MoveInLineOfSight(Unit* /*who*/) {}

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

			instance->SetData(DATA_CYANIGOSA_EVENT, IN_PROGRESS);
        }

		void KilledUnit(Unit* victim)
        {
            if (victim == me)
                return;

            DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
        }

		void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

			instance->SetData(DATA_CYANIGOSA_EVENT, DONE);
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_ARCANE_VACUUM)
            {
                if (target->ToPlayer())
                    target->ToPlayer()->TeleportTo(me->GetMapId(), me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (instance->GetData(DATA_REMOVE_NPC) == 1)
            {
                me->DespawnAfterAction();
                instance->SetData(DATA_REMOVE_NPC, 0);
            }

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TailSweepTimer <= diff)
            {
				DoSendQuantumText(SAY_BREATH_ATTACK, me);
				DoCast(DUNGEON_MODE(SPELL_TAIL_SWEEP_5N, SPELL_TAIL_SWEEP_5H));
				TailSweepTimer = 5000;
            }
			else TailSweepTimer -= diff;

			if (BlizzardTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
					DoCast(target, DUNGEON_MODE(SPELL_BLIZZARD_5N, SPELL_BLIZZARD_5H), true);
					BlizzardTimer = 7000;
                }
            }
			else BlizzardTimer -= diff;

            if (ArcaneVacuumTimer <= diff)
            {
				DoSendQuantumText(SAY_DISRUPTION, me);
				DoCastAOE(SPELL_ARCANE_VACUUM);
				ArcaneVacuumTimer = 9000;
            }
			else ArcaneVacuumTimer -= diff;

			if (ManaDestructionTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoSendQuantumText(SAY_SPECIAL_ATTACK_1, me);
						DoCast(target, SPELL_MANA_DESTRUCTION);
						ManaDestructionTimer = 11000;
					}
				}
			}
			else ManaDestructionTimer -= diff;

            if (UncontrollableEnergyTimer <= diff)
            {
				DoSendQuantumText(SAY_SPECIAL_ATTACK_2, me);
                DoCast(DUNGEON_MODE(SPELL_UNCONTROLLABLE_ENERGY_5N, SPELL_UNCONTROLLABLE_ENERGY_5H));
                UncontrollableEnergyTimer = urand(11000, 12000);
            }
			else UncontrollableEnergyTimer -= diff;

            DoMeleeAttackIfReady();
		}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_cyanigosaAI(creature);
    }
};

class achievement_defenseless : public AchievementCriteriaScript
{
public:
	achievement_defenseless() : AchievementCriteriaScript("achievement_defenseless") { }

	bool OnCheck(Player* /*player*/, Unit* target)
	{
		if (!target)
			return false;

		InstanceScript* instance = target->GetInstanceScript();

		if (!instance)
			return false;

		if (!instance->GetData(DATA_DEFENSELESS))
			return false;

		return true;
	}
};

void AddSC_boss_cyanigosa()
{
    new boss_cyanigosa();
    new achievement_defenseless();
}