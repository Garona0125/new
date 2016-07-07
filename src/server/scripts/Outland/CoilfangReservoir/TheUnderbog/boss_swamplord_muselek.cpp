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
#include "the_underbog.h"

enum Texts
{
	SAY_MUSELEK_AGGRO_1     = -1420051,
	SAY_MUSELEK_AGGRO_2     = -1420052,
	SAY_MUSELEK_AGGRO_3     = -1420053,
	SAY_MUSELEK_SLAY_1      = -1420054,
	SAY_MUSELEK_SLAY_2      = -1420055,
	SAY_MUSELEK_ENRAGE_CLAW = -1420056,
	SAY_MUSELEK_DEATH       = -1420057,
};

enum Spells
{
	SPELL_BEAR_COMMAND        = 34662, // ??
	SPELL_NOTIFY_OF_DEATH     = 31547, // ??

	SPELL_HUNTER_MASK         = 31615,
	SPELL_DETERRENCE          = 31567,
	SPELL_SHOOT               = 22907,
	SPELL_AIMED_SHOT          = 31623,
	SPELL_MULTI_SHOT          = 34974,
	SPELL_RAPTOR_STRIKE       = 31566,
	SPELL_THROW_FREEZING_TRAP = 31946,
	SPELL_MAUL                = 34298,
	SPELL_ECHOING_ROAR        = 31429,
	SPELL_FRENZY              = 34971,
};

class boss_swamplord_muselek : public CreatureScript
{
public:
    boss_swamplord_muselek() : CreatureScript("boss_swamplord_muselek") {}

    struct boss_swamplord_muselekAI : public QuantumBasicAI
    {
        boss_swamplord_muselekAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;
		uint32 AimedShotTimer;
		uint32 MultiShotTimer;
		uint32 ThrowFreezingTrapTimer;
		uint32 RaptorStrikeTimer;

        void Reset()
        {
			ShootTimer = 1000;
			AimedShotTimer = 2000;
			MultiShotTimer = 4000;
			ThrowFreezingTrapTimer = 6000;
			RaptorStrikeTimer = 8000;

			ClawRespawn();

			me->GetMotionMaster()->MovePath(53893, false);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_MUSELEK_AGGRO_1, SAY_MUSELEK_AGGRO_2, SAY_MUSELEK_AGGRO_3), me);

			DoCastVictim(SPELL_HUNTER_MASK, true);
		}

		void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_MUSELEK_SLAY_1, SAY_MUSELEK_SLAY_2), me);
        }

		void JustDied(Unit* killer)
        {
			if (killer->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(SAY_MUSELEK_DEATH, me);
		}

		void AttackStart(Unit* who)
        {
			AttackStartNoMove(who);
        }

		void ClawRespawn()
		{
			if (Creature* claw = me->FindCreatureWithDistance(NPC_WINDCALLER_CLAW, 500.0f))
			{
				me->Kill(claw);
				claw->Respawn();
				claw->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
			}
		}

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShootTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_SHOOT);
						ShootTimer = urand(2000, 3000);
					}
				}
			}
			else ShootTimer -= diff;

			if (AimedShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(me, SPELL_DETERRENCE, true);
						DoCast(target, SPELL_AIMED_SHOT, true);
						AimedShotTimer = urand(4000, 5000);
					}
				}
			}
			else AimedShotTimer -= diff;

			if (MultiShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_MULTI_SHOT);
						MultiShotTimer = urand(6000, 7000);
					}
				}
			}
			else MultiShotTimer -= diff;

			if (ThrowFreezingTrapTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_THROW_FREEZING_TRAP);
						ThrowFreezingTrapTimer = urand(8000, 9000);
					}
				}
			}
			else ThrowFreezingTrapTimer -= diff;

			if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    DoStartMovement(me->GetVictim());

				if (RaptorStrikeTimer <= diff)
				{
					DoCastVictim(SPELL_RAPTOR_STRIKE);
					RaptorStrikeTimer = urand(5000, 6000);
				}
				else RaptorStrikeTimer -= diff;

                DoMeleeAttackIfReady();
            }
			else
			{
				if (me->GetDistance2d(me->GetVictim()) > 10)
                    DoStartNoMovement(me->GetVictim());

                if (me->GetDistance2d(me->GetVictim()) > 30)
                    DoStartMovement(me->GetVictim());

				if (ShootTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, SPELL_SHOOT);
						ShootTimer = urand(10000, 11000);
					}
				}
				else ShootTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_swamplord_muselekAI(creature);
    }
};

class npc_claw : public CreatureScript
{
public:
    npc_claw() : CreatureScript("npc_claw") {}

    struct npc_clawAI : public QuantumBasicAI
    {
        npc_clawAI(Creature* creature) : QuantumBasicAI(creature){}

		bool Rescued;

		uint32 MaulTimer;
		uint32 EchoingRoarTimer;
		uint32 FrenzyTimer;

        void Reset()
		{
			MaulTimer = 2000;
			EchoingRoarTimer = 3000;
			FrenzyTimer = 5000;

			Rescued = true;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_RANGED_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

		void DamageTaken(Unit* /*DoneBy*/, uint32& damage)
		{
			if (damage > me->GetHealth())
				damage = 0;
		}

		void RescuedClaw()
		{
			if (Rescued)
			{
				me->UpdateEntry(NPC_WINDCALLER_CLAW);

				me->AI()->EnterEvadeMode();
				Rescued = false;
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MaulTimer <= diff)
			{
				DoCastVictim(SPELL_MAUL);
				MaulTimer = 4000;
			}
			else MaulTimer -= diff;

			if (EchoingRoarTimer <= diff)
			{
				DoCastAOE(SPELL_ECHOING_ROAR);
				EchoingRoarTimer = 6000;
			}
			else EchoingRoarTimer -= diff;

			if (FrenzyTimer <= diff)
			{
				DoCast(me, SPELL_FRENZY);

				if (Creature* Muselek = me->FindCreatureWithDistance(NPC_SWAMPLORD_MUSELEK, 500.0f))
					DoSendQuantumText(SAY_MUSELEK_ENRAGE_CLAW, Muselek);

				FrenzyTimer = 17000;
			}
			else FrenzyTimer -= diff;

			if (HealthBelowPct(20))
				RescuedClaw();

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_clawAI(creature);
    }
};

void AddSC_boss_swamplord_muselek()
{
	new boss_swamplord_muselek();
	new npc_claw();
}