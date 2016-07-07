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
#include "steam_vault.h"

enum Texts
{
	SAY_MECHANICS = -1545007,
	SAY_AGGRO_1   = -1545008,
	SAY_AGGRO_2   = -1545009,
	SAY_AGGRO_3   = -1545010,
	SAY_AGGRO_4   = -1545011,
	SAY_SLAY_1    = -1545012,
	SAY_SLAY_2    = -1545013,
	SAY_SLAY_3    = -1545014,
	SAY_DEATH     = -1545015,
};

enum Spells
{
	SPELL_SUPER_SHRINK_RAY = 31485,
	SPELL_SAW_BLADE        = 31486,
	SPELL_ELECTRIFIED_NET  = 35107,
	SPELL_ENRAGE           = 8599,
	SPELL_DISPEL_MAGIC     = 17201,
	SPELL_REPAIR_5N        = 31532,
	SPELL_REPAIR_5H        = 37936,
};

class boss_mekgineer_steamrigger : public CreatureScript
{
public:
    boss_mekgineer_steamrigger() : CreatureScript("boss_mekgineer_steamrigger") { }

    struct boss_mekgineer_steamriggerAI : public QuantumBasicAI
    {
        boss_mekgineer_steamriggerAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 ShrinkTimer;
        uint32 SawBladeTimer;
        uint32 ElectrifiedNetTimer;
        bool Summon75;
        bool Summon50;
        bool Summon25;

        void Reset()
        {
            ShrinkTimer = 500;
            SawBladeTimer = 3000;
            ElectrifiedNetTimer = 5000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

            Summon75 = false;
            Summon50 = false;
            Summon25 = false;

            if (instance)
                instance->SetData(TYPE_MEKGINEER_STEAMRIGGER, NOT_STARTED);
        }

		void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);

            if (instance)
                instance->SetData(TYPE_MEKGINEER_STEAMRIGGER, IN_PROGRESS);
        }

		void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
        }

        void JustDied(Unit* /*Killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

            if (instance)
                instance->SetData(TYPE_MEKGINEER_STEAMRIGGER, DONE);
        }

        void SummonMechanichs()
        {
            DoSendQuantumText(SAY_MECHANICS, me);

            DoSpawnCreature(NPC_STREAMRIGGER_MECHANIC, 5, 5, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 240000);
            DoSpawnCreature(NPC_STREAMRIGGER_MECHANIC, -5, 5, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 240000);
            DoSpawnCreature(NPC_STREAMRIGGER_MECHANIC, -5, -5, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 240000);

            if (rand()%2)
                DoSpawnCreature(NPC_STREAMRIGGER_MECHANIC, 5, -7, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 240000);
            if (rand()%2)
                DoSpawnCreature(NPC_STREAMRIGGER_MECHANIC, 7, -5, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 240000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (ShrinkTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					me->SetFacingToObject(target);
					DoCast(target, SPELL_SUPER_SHRINK_RAY);
					ShrinkTimer = urand(3000, 4000);
				}
            }
			else ShrinkTimer -= diff;

            if (SawBladeTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
                    DoCast(target, SPELL_SAW_BLADE);
					SawBladeTimer = urand(5000, 6000);
				}
            }
			else SawBladeTimer -= diff;

            if (ElectrifiedNetTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ELECTRIFIED_NET);
					ElectrifiedNetTimer = urand(7000, 8000);
				}
            }
            else ElectrifiedNetTimer -= diff;

            if (!Summon75)
            {
                if (HealthBelowPct(75))
                {
                    SummonMechanichs();
                    Summon75 = true;
                }
            }

            if (!Summon50)
            {
                if (HealthBelowPct(50))
                {
                    SummonMechanichs();
                    Summon50 = true;
                }
            }

            if (!Summon25)
            {
                if (HealthBelowPct(25))
                {
                    SummonMechanichs();
                    Summon25 = true;
                }
            }

			if (HealthBelowPct(50))
			{
				if (!me->HasAuraEffect(SPELL_ENRAGE, 0))
				{
					DoCast(me, SPELL_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_mekgineer_steamriggerAI(creature);
    }
};

#define MAX_REPAIR_RANGE (13.0f) // we should be at least at this range for repair
#define MIN_REPAIR_RANGE (7.0f)  // we can stop movement at this range to repair but not required

class npc_steamrigger_mechanic : public CreatureScript
{
public:
    npc_steamrigger_mechanic() : CreatureScript("npc_steamrigger_mechanic") { }

    struct npc_steamrigger_mechanicAI : public QuantumBasicAI
    {
        npc_steamrigger_mechanicAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 RepairTimer;

        void Reset()
        {
            RepairTimer = 500;
        }

        void MoveInLineOfSight(Unit* /*who*/){}

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (RepairTimer <= diff)
            {
                if (instance && instance->GetData64(DATA_MEKGINEER_STEAMRIGGER) && instance->GetData(TYPE_MEKGINEER_STEAMRIGGER) == IN_PROGRESS)
                {
                    if (Unit* mekgineer = Unit::GetUnit(*me, instance->GetData64(DATA_MEKGINEER_STEAMRIGGER)))
                    {
                        if (me->IsWithinDistInMap(mekgineer, MAX_REPAIR_RANGE))
                        {
                            if (!me->GetUInt32Value(UNIT_CHANNEL_SPELL))
                            {
                                DoCast(me, DUNGEON_MODE(SPELL_REPAIR_5N, SPELL_REPAIR_5H), true);
								RepairTimer = 5000;
							}
                        }
                    }
                }
				else RepairTimer = 5000;
            }
			else RepairTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_steamrigger_mechanicAI(creature);
    }
};

void AddSC_boss_mekgineer_steamrigger()
{
    new boss_mekgineer_steamrigger();
    new npc_steamrigger_mechanic();
}