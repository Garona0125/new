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
#include "../../../scripts/Northrend/Gundrak/gundrak.h"

enum GundrakSpells
{
	SPELL_DETERRENCE           = 31567,
	SPELL_FLARE                = 55798,
	SPELL_SHOOT_5N             = 35946,
	SPELL_SHOOT_5H             = 59146,
	SPELL_ARCANE_SHOT_5N       = 55624,
	SPELL_ARCANE_SHOT_5H       = 58973,
	SPELL_TRANQUILIZING_SHOT   = 55625,
	SPELL_STRIKE_5N            = 12057,
	SPELL_STRIKE_5H            = 15580,
	SPELL_HEAD_CRACK_5N        = 16172,
	SPELL_HEAD_CRACK_5H        = 58969,
	SPELL_SLAM_GROUND          = 55563,
	SPELL_POWERFUL_BLOW        = 55567,
	SPELL_FLAME_SHOCK_5N       = 55613,
	SPELL_FLAME_SHOCK_5H       = 58971,
	SPELL_LAVA_BURST_5N        = 55659,
	SPELL_LAVA_BURST_5H        = 58972,
	SPELL_BLAST_WAVE           = 61362,
	SPELL_MANA_LINK            = 55582, // Need Implement
	SPELL_EARTH_SHIELD_5N      = 55599,
	SPELL_EARTH_SHIELD_5H      = 58981,
	SPELL_HEALING_WAVE_5N      = 55597,
	SPELL_HEALING_WAVE_5H      = 58980,
	SPELL_CLEANSE_MAGIC        = 55598,
	SPELL_MOJO_VOLLEY_5N       = 54849,
	SPELL_MOJO_VOLLEY_5H       = 59453,
	SPELL_MOJO_WAVE_5N         = 55626,
	SPELL_MOJO_WAVE_5H         = 58993,
	SPELL_DISARM               = 6713,
	SPELL_IMPALE_5N            = 55622,
	SPELL_IMPALE_5H            = 58978,
	SPELL_RETAILATION          = 40546,
	SPELL_REGURGITATE          = 55643,
    SPELL_SPRING               = 55652,
	SPELL_THUNDERCLAP_5N       = 55635,
	SPELL_THUNDERCLAP_5H       = 58975,
	SPELL_SHOCKWAVE_5N         = 55636,
	SPELL_SHOCKWAVE_5H         = 58977,
	SPELL_BODY_OF_STONE        = 55633,
	SPELL_VICIOUS_BITE_5N      = 55602,
	SPELL_VICIOUS_BITE_5H      = 59021,
	SPELL_PUNCTURING_STRIKE_5N = 55605,
	SPELL_PUNCTURING_STRIKE_5H = 59023,
	SPELL_COBRA_STRIKE_5N      = 55703,
	SPELL_COBRA_STRIKE_5H      = 59020,
	SPELL_VENOM_SPIT_5N        = 55700,
	SPELL_VENOM_SPIT_5H        = 59019,
	SPELL_THROW_5N             = 55348,
	SPELL_THROW_5H             = 58966,
	SPELL_POISONED_SPEAR_5N    = 55521,
	SPELL_POISONED_SPEAR_5H    = 58967,
	SPELL_CHARGE_5N            = 55530,
	SPELL_CHARGE_5H            = 58991,
	SPELL_DEAFENING_ROAR_5N    = 55663,
	SPELL_DEAFENING_ROAR_5H    = 58992,
};

class npc_drakkari_god_hunter : public CreatureScript
{
public:
    npc_drakkari_god_hunter() : CreatureScript("npc_drakkari_god_hunter") {}

    struct npc_drakkari_god_hunterAI : public QuantumBasicAI
    {
        npc_drakkari_god_hunterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShootTimer;
		uint32 FlareTimer;
		uint32 ArcaneShotTimer;
		uint32 TranquilizingShotTimer;
		uint32 DeterrenceTimer;

        void Reset()
        {
			ShootTimer = 500;
			FlareTimer = 1000;
			ArcaneShotTimer = 2000;
			TranquilizingShotTimer = 3000;
			DeterrenceTimer = 5000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void AttackStart(Unit* who)
        {
			AttackStartNoMove(who);
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
						DoCast(target, DUNGEON_MODE(SPELL_SHOOT_5N, SPELL_SHOOT_5H));
						ShootTimer = urand(2000, 3000);
					}
				}
			}
			else ShootTimer -= diff;

			if (FlareTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_FLARE);
						FlareTimer = urand(4000, 5000);
					}
				}
			}
			else FlareTimer -= diff;

			if (ArcaneShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, DUNGEON_MODE(SPELL_ARCANE_SHOT_5N, SPELL_ARCANE_SHOT_5H));
						ArcaneShotTimer = urand(6000, 7000);
					}
				}
			}
			else ArcaneShotTimer -= diff;

			if (TranquilizingShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_TRANQUILIZING_SHOT);
						TranquilizingShotTimer = urand(8000, 9000);
					}
				}
			}
			else TranquilizingShotTimer -= diff;

			if (DeterrenceTimer <= diff)
			{
				DoCast(SPELL_DETERRENCE);
				DeterrenceTimer = urand(10000, 11000);
			}
			else DeterrenceTimer -= diff;

			if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    DoStartMovement(me->GetVictim());

				DoMeleeAttackIfReady();
            }
			else
			{
				if (me->GetDistance2d(me->GetVictim()) > 10)
                    DoStartNoMovement(me->GetVictim());

                if (me->GetDistance2d(me->GetVictim()) > 30)
                    DoStartMovement(me->GetVictim());
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drakkari_god_hunterAI(creature);
    }
};

class npc_drakkari_inciter : public CreatureScript
{
public:
    npc_drakkari_inciter() : CreatureScript("npc_drakkari_inciter") { }

    struct npc_drakkari_inciterAI : public QuantumBasicAI
    {
        npc_drakkari_inciterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 StrikeTimer;

        void Reset()
        {
            StrikeTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (StrikeTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_STRIKE_5N, SPELL_STRIKE_5H));
                StrikeTimer = urand(3000, 4000);
            }
			else StrikeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drakkari_inciterAI(creature);
    }
};

class npc_drakkari_earthshaker : public CreatureScript
{
public:
    npc_drakkari_earthshaker() : CreatureScript("npc_drakkari_earthshaker") { }

    struct npc_drakkari_earthshakerAI : public QuantumBasicAI
    {
        npc_drakkari_earthshakerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 HeadCrackTimer;
		uint32 PowerfulBlowTimer;
		uint32 SlamGroundTimer;

        void Reset()
        {
			HeadCrackTimer = 2000;
			PowerfulBlowTimer = 3000;
			SlamGroundTimer = 5000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (HeadCrackTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_HEAD_CRACK_5N, SPELL_HEAD_CRACK_5H));
                HeadCrackTimer = urand(3000, 4000);
            }
			else HeadCrackTimer -= diff;

			if (PowerfulBlowTimer <= diff)
            {
                DoCastVictim(SPELL_POWERFUL_BLOW);
                PowerfulBlowTimer = urand(5000, 6000);
            }
			else PowerfulBlowTimer -= diff;

			if (SlamGroundTimer <= diff)
            {
                DoCastAOE(SPELL_SLAM_GROUND);
                SlamGroundTimer = urand(7000, 8000);
            }
			else SlamGroundTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drakkari_earthshakerAI(creature);
    }
};

class npc_drrakkari_fire_weaver : public CreatureScript
{
public:
    npc_drrakkari_fire_weaver() : CreatureScript("npc_drrakkari_fire_weaver") { }

    struct npc_drrakkari_fire_weaverAI : public QuantumBasicAI
    {
        npc_drrakkari_fire_weaverAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 FlameShockTimer;
        uint32 LavaBurstTimer;
		uint32 BlastWaveTimer;

        void Reset()
        {
            FlameShockTimer = 500;
            LavaBurstTimer = 1000;
			BlastWaveTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (FlameShockTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                     DoCast(target, DUNGEON_MODE(SPELL_FLAME_SHOCK_5N, SPELL_FLAME_SHOCK_5H));
					 FlameShockTimer = urand(3000, 4000);
				}
            }
			else FlameShockTimer -= diff;

            if (LavaBurstTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_LAVA_BURST_5N, SPELL_LAVA_BURST_5H));
					LavaBurstTimer = urand(5000, 6000);
				}
            }
			else LavaBurstTimer -= diff;

			if (BlastWaveTimer <= diff)
            {
				DoCastAOE(SPELL_BLAST_WAVE);
				BlastWaveTimer = urand(7000, 8000);
            }
			else BlastWaveTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drrakkari_fire_weaverAI(creature);
    }
};

class npc_drakkari_medicine_man : public CreatureScript
{
public:
    npc_drakkari_medicine_man() : CreatureScript("npc_drakkari_medicine_man") {}

    struct npc_drakkari_medicine_manAI : public QuantumBasicAI
    {
        npc_drakkari_medicine_manAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 EarthShieldTimer;
		uint32 HealingWaveTimer;
		uint32 CleanseMagicTimer;

        void Reset()
        {
			EarthShieldTimer = 500;
			HealingWaveTimer = 2000;
			CleanseMagicTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (EarthShieldTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, DUNGEON_MODE(SPELL_EARTH_SHIELD_5N, SPELL_EARTH_SHIELD_5H));
					EarthShieldTimer = urand(5000, 6000);
				}
			}
			else EarthShieldTimer -= diff;

			if (HealingWaveTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, DUNGEON_MODE(SPELL_HEALING_WAVE_5N, SPELL_HEALING_WAVE_5H));
					HealingWaveTimer = urand(7000, 8000);
				}
			}
			else HealingWaveTimer -= diff;

			if (CleanseMagicTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CLEANSE_MAGIC);
					CleanseMagicTimer = urand(9000, 10000);
				}
			}
			else CleanseMagicTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
				DoCast(me, DUNGEON_MODE(SPELL_HEALING_WAVE_5N, SPELL_HEALING_WAVE_5H), true);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drakkari_medicine_manAI(creature);
    }
};

class npc_drakkari_lancer : public CreatureScript
{
public:
    npc_drakkari_lancer() : CreatureScript("npc_drakkari_lancer") { }

    struct npc_drakkari_lancerAI : public QuantumBasicAI
    {
        npc_drakkari_lancerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ImpaleTimer;
        uint32 DisarmTimer;
		uint32 RetailationTimer;

        void Reset()
        {
            ImpaleTimer = 2000;
            DisarmTimer = 3000;
			RetailationTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ImpaleTimer <= diff)
            {
				DoCastVictim(DUNGEON_MODE(SPELL_IMPALE_5N, SPELL_IMPALE_5H));
				ImpaleTimer = urand(3000, 4000);
            }
			else ImpaleTimer -= diff;

            if (DisarmTimer <= diff)
            {
				DoCastVictim(SPELL_DISARM);
				DisarmTimer = urand(5000, 6000);
            }
			else DisarmTimer -= diff;

			if (RetailationTimer <= diff)
            {
				DoCast(me, SPELL_RETAILATION);
				RetailationTimer = urand(7000, 8000);
            }
			else RetailationTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drakkari_lancerAI(creature);
    }
};

class npc_drakkari_golem : public CreatureScript
{
public:
    npc_drakkari_golem() : CreatureScript("npc_drakkari_golem") { }

    struct npc_drakkari_golemAI : public QuantumBasicAI
    {
        npc_drakkari_golemAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ThunderclapTimer;
        uint32 ShockwaveTimer;

        void Reset()
        {
            ThunderclapTimer = 2000;
            ShockwaveTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ThunderclapTimer <= diff)
            {
				DoCastAOE(DUNGEON_MODE(SPELL_THUNDERCLAP_5N, SPELL_THUNDERCLAP_5H));
				ThunderclapTimer = urand(4000, 5000);
            }
			else ThunderclapTimer -= diff;

            if (ShockwaveTimer <= diff)
            {
				DoCastAOE(DUNGEON_MODE(SPELL_SHOCKWAVE_5N, SPELL_SHOCKWAVE_5H));
				ShockwaveTimer = urand(6000, 7000);
            }
			else ShockwaveTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_BODY_OF_STONE, 0))
					DoCast(me, SPELL_BODY_OF_STONE, true);
			}

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drakkari_golemAI(creature);
    }
};

class npc_drakkari_battle_rider : public CreatureScript
{
public:
	npc_drakkari_battle_rider() : CreatureScript("npc_drakkari_battle_rider") { }

	struct npc_drakkari_battle_riderAI : public QuantumBasicAI
	{
		npc_drakkari_battle_riderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ThrowTimer;
		uint32 PoisonedSpearTimer;

		void Reset()
		{
			ThrowTimer = 500;
			PoisonedSpearTimer = 1000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ThrowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_THROW_5N, SPELL_THROW_5H), true);
					ThrowTimer = urand(3000, 4000);
				}
			}
			else ThrowTimer -= diff;

			if (PoisonedSpearTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_POISONED_SPEAR_5N, SPELL_POISONED_SPEAR_5H), true);
					PoisonedSpearTimer = urand(5000, 6000);
				}
			}
			else PoisonedSpearTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_drakkari_battle_riderAI(creature);
	}
};

class npc_drakkari_rhino : public CreatureScript
{
public:
	npc_drakkari_rhino() : CreatureScript("npc_drakkari_rhino") { }

	struct npc_drakkari_rhinoAI : public QuantumBasicAI
	{
		npc_drakkari_rhinoAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ChargeTimer;
		uint32 DeafeningRoarTimer;

		void Reset()
		{
			ChargeTimer = 100;
			DeafeningRoarTimer = 1500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ChargeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_CHARGE_5N, SPELL_CHARGE_5H), true);
					ChargeTimer = 5000;
				}
			}
			else ChargeTimer -= diff;

			if (DeafeningRoarTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_DEAFENING_ROAR_5N, SPELL_DEAFENING_ROAR_5H));
				DeafeningRoarTimer = 6000;
			}
			else DeafeningRoarTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_drakkari_rhinoAI(creature);
	}
};

class npc_living_mojo : public CreatureScript
{
public:
	npc_living_mojo() : CreatureScript("npc_living_mojo") { }

	struct npc_living_mojoAI : public QuantumBasicAI
	{
		npc_living_mojoAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 MojoWaveTimer;

		void Reset()
		{
			MojoWaveTimer = 1000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, DUNGEON_MODE(SPELL_MOJO_VOLLEY_5N, SPELL_MOJO_VOLLEY_5H));
		}

		void JustDied(Unit* /*killer*/)
		{
			me->RemoveAurasDueToSpell(DUNGEON_MODE(SPELL_MOJO_VOLLEY_5N, SPELL_MOJO_VOLLEY_5H));
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MojoWaveTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_MOJO_WAVE_5N, SPELL_MOJO_WAVE_5H));
				MojoWaveTimer = 4000;
			}
			else MojoWaveTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_living_mojoAI(creature);
	}
};

class npc_ruins_dweller : public CreatureScript
{
public:
	npc_ruins_dweller() : CreatureScript("npc_ruins_dweller") { }

	struct npc_ruins_dwellerAI : public QuantumBasicAI
	{
		npc_ruins_dwellerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();

			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		InstanceScript* instance;

		uint32 SpringTimer;
		uint32 RegurgitateTimer;

		void Reset()
		{
			RegurgitateTimer = 2000;
			SpringTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* /*who*/)
		{
			if (instance)
				instance->SetData(DATA_RUIN_DWELLER_DIED, 1);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RegurgitateTimer <= diff)
			{
				DoCastVictim(SPELL_REGURGITATE, true);
				RegurgitateTimer = urand(4000, 5000);
			}
			else RegurgitateTimer -= diff;

			if (SpringTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SPRING);
					DoResetThreat();
					me->AddThreat(target, 1000);
					SpringTimer = urand(6000, 7000);
				}
			}
			else SpringTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ruins_dwellerAI(creature);
	}
};

class npc_unyielding_constrictor : public CreatureScript
{
public:
	npc_unyielding_constrictor() : CreatureScript("npc_unyielding_constrictor") { }

	struct npc_unyielding_constrictorAI : public QuantumBasicAI
	{
		npc_unyielding_constrictorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ViciousBiteTimer;
		uint32 PuncturingStrikeTimer;

		void Reset()
		{
			ViciousBiteTimer = 2000;
			PuncturingStrikeTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ViciousBiteTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_VICIOUS_BITE_5N, SPELL_VICIOUS_BITE_5H));
				ViciousBiteTimer = urand(4000, 5000);
			}
			else ViciousBiteTimer -= diff;

			if (PuncturingStrikeTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_PUNCTURING_STRIKE_5N, SPELL_PUNCTURING_STRIKE_5H));
				PuncturingStrikeTimer = urand(6000, 7000);
			}
			else PuncturingStrikeTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_unyielding_constrictorAI(creature);
	}
};

class npc_spitting_cobra : public CreatureScript
{
public:
	npc_spitting_cobra() : CreatureScript("npc_spitting_cobra") { }

	struct npc_spitting_cobraAI : public QuantumBasicAI
	{
		npc_spitting_cobraAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CobraStrikeTimer;
		uint32 VenomSpitTimer;

		void Reset()
		{
			VenomSpitTimer = 500;
			CobraStrikeTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (VenomSpitTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_VENOM_SPIT_5N, SPELL_VENOM_SPIT_5H));
					VenomSpitTimer = urand(3000, 4000);
				}
			}
			else VenomSpitTimer -= diff;

			if (CobraStrikeTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_COBRA_STRIKE_5N, SPELL_COBRA_STRIKE_5H));
				CobraStrikeTimer = urand(5000, 6000);
			}
			else CobraStrikeTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_spitting_cobraAI(creature);
	}
};

void AddSC_gundrak_trash()
{
	new npc_drakkari_god_hunter();
	new npc_drakkari_inciter();
	new npc_drakkari_earthshaker();
	new npc_drrakkari_fire_weaver();
	new npc_drakkari_medicine_man();
	new npc_drakkari_lancer();
	new npc_drakkari_golem();
	new npc_drakkari_battle_rider();
	new npc_drakkari_rhino();
	new npc_living_mojo();
	new npc_ruins_dweller();
	new npc_unyielding_constrictor();
	new npc_spitting_cobra();
}