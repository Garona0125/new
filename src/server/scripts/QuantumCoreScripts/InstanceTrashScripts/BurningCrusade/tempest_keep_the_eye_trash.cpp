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
#include "../../../scripts/Outland/TempestKeep/Eye/the_eye.h"

#define EMOTE_POWERED_DOWN "%s Begins into repair!"

enum TempestKeepSpells
{
    SPELL_COUNTERCHARGE_1          = 35035,
	SPELL_COUNTERCHARGE_2          = 35039,
    SPELL_KNOCK_AWAY               = 22893,
	SPELL_POWERED_DOWN             = 34937,
	SPELL_CHARGED_ARCANE_EXPLOSION = 37106,
	SPELL_OVERCHARGE               = 37104,
	SPELL_DOMINATION               = 37122,
	SPELL_STARFALL                 = 37124,
	SPELL_ARCANE_BLAST             = 37126,
	SPELL_DUAL_WIELD               = 42459,
	SPELL_WHIRLWIND                = 33500,
	SPELL_CLEAVE                   = 15284,
	SPELL_BLOODTHIRST              = 35949,
	SPELL_CLEANSE                  = 39078,
	SPELL_FLASH_OF_LIGHT           = 37249,
	SPELL_HAMMER_OF_JUSTICE        = 13005,
	SPELL_HAMMER_OF_WRATH          = 37251,
	SPELL_MOLTEN_ARMOR             = 35915,
	SPELL_FIREBALL_VOLLEY          = 37109,
	SPELL_FIRE_BLAST               = 37110,
	SPELL_BL_WHIRLWIND             = 36132,
	SPELL_UPPERCUT                 = 34996,
	SPELL_BS_FLASH_OF_LIGHT        = 37254,
	SPELL_BS_HAMMER_OF_JUSTICE     = 39077,
	SPELL_BS_HAMMER_OF_WRATH       = 37255,
	SPELL_FIRE_SHIELD              = 37318,
	SPELL_IMMOLATION_ARROW         = 37154,
	SPELL_SHOOT                    = 39079,
	SPELL_KNOCKBACK                = 37317,
	SPELL_SILENCE                  = 37160,
	SPELL_WING_BUFFET              = 37319,
	SPELL_DIVE                     = 37156,
	SPELL_MANA_BURN                = 37159,
	SPELL_FRAGMENTATION_BOMB       = 37120,
	SPELL_SHELL_SHOCK              = 37118,
	SPELL_GOLEM_REPAIR             = 34946,
	SPELL_POWER_UP                 = 37112,
	SPELL_ARCANE_BUFFET            = 37133,
	SPELL_ARCANE_EXPLOSION         = 38725,
	SPELL_ARCANE_SHOCK             = 37132,
	SPELL_ARCANE_VOLLEY            = 37129,
	SPELL_RECHARGE                 = 37121,
	SPELL_SAW_BLADE                = 37123,
	SPELL_NA_FIRE_SHIELD           = 37282,
	SPELL_FIREBALL                 = 37111,
	SPELL_FIRE_NOVA                = 38728,
	SPELL_RAIN_OF_FIRE             = 37279,
	SPELL_DRAGONS_BREATH           = 37289,
	SPELL_NS_DOMINATION            = 37135,
	SPELL_NL_SHOOT                 = 36980,
	SPELL_MULTI_SHOT               = 36979,
	SPELL_BLINK                    = 36994,
	SPELL_DEVASTATION_WHIRLWIND    = 36981,
	SPELL_HEAL                     = 36983,
	SPELL_HOLY_NOVA                = 36985,
	SPELL_THRASH                   = 3417,
	SPELL_REND                     = 36991,
	SPELL_SHIELD_BASH              = 36988,
	SPELL_FROSTBOLT                = 36990,
	SPELL_FROST_NOVA               = 36989,
};

class npc_crystalcore_devastator : public CreatureScript
{
public:
	npc_crystalcore_devastator() : CreatureScript("npc_crystalcore_devastator") {}

	struct npc_crystalcore_devastatorAI : public QuantumBasicAI
	{
		npc_crystalcore_devastatorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
		}

		uint32 KnockAwayTimer;
		uint32 SelfCounterchargeTimer;
		uint32 CounterchargeTimer;

		void Reset()
		{
			SelfCounterchargeTimer = 2000;
			CounterchargeTimer = 4000;
			KnockAwayTimer = 6000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SelfCounterchargeTimer <= diff)
			{
				DoCast(me, SPELL_COUNTERCHARGE_1);
				SelfCounterchargeTimer = urand(4000, 5000);
			}
			else SelfCounterchargeTimer -= diff;

			if (CounterchargeTimer <= diff)
			{
				DoCastVictim(SPELL_COUNTERCHARGE_2);
				CounterchargeTimer = urand(6000, 7000);
			}
			else CounterchargeTimer -= diff;

			if (KnockAwayTimer <= diff)
			{
				DoCastVictim(SPELL_KNOCK_AWAY, true);

				Unit* target = SelectTarget(TARGET_TOP_AGGRO, 0);

				if (!target || target == me->GetVictim())
					target = SelectTarget(TARGET_TOP_AGGRO, 1);

				if (target)
					me->TauntApply(target);
				
				KnockAwayTimer = urand(9000, 10000);
			}
			else KnockAwayTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_20))
			{
				if (!me->HasAuraEffect(SPELL_POWERED_DOWN, 0))
				{
					me->MonsterTextEmote(EMOTE_POWERED_DOWN, LANG_UNIVERSAL, false);
					DoCast(me, SPELL_POWERED_DOWN);
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_crystalcore_devastatorAI(creature);
	}
};

class npc_crystalcore_sentinel : public CreatureScript
{
public:
	npc_crystalcore_sentinel() : CreatureScript("npc_crystalcore_sentinel") {}

	struct npc_crystalcore_sentinelAI : public QuantumBasicAI
	{
		npc_crystalcore_sentinelAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
		}

		uint32 ChargedArcaneExplosionTimer;
		uint32 OverchargeTimer;

		void Reset()
		{
			ChargedArcaneExplosionTimer = 2000;
			OverchargeTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ChargedArcaneExplosionTimer <= diff)
			{
				DoCastAOE(SPELL_CHARGED_ARCANE_EXPLOSION);
				ChargedArcaneExplosionTimer = urand(4000, 5000);
			}
			else ChargedArcaneExplosionTimer -= diff;

			if (OverchargeTimer <= diff)
			{
				DoCastVictim(SPELL_OVERCHARGE);
				OverchargeTimer = urand(6000, 7000);
			}
			else OverchargeTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_20))
			{
				if (!me->HasAuraEffect(SPELL_POWERED_DOWN, 0))
				{
					me->MonsterTextEmote(EMOTE_POWERED_DOWN, LANG_UNIVERSAL, false);
					DoCast(me, SPELL_POWERED_DOWN);
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_crystalcore_sentinelAI(creature);
	}
};

class npc_star_scryer : public CreatureScript
{
public:
    npc_star_scryer() : CreatureScript("npc_star_scryer") {}

    struct npc_star_scryerAI : public QuantumBasicAI
    {
        npc_star_scryerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 ArcaneBlastTimer;
		uint32 StarfallTimer;
		uint32 DominationTimer;

        void Reset()
		{
			ArcaneBlastTimer = 1000;
			StarfallTimer = 3000;
			DominationTimer = 5000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ArcaneBlastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ARCANE_BLAST);
					ArcaneBlastTimer = urand(3000, 4000);
				}
			}
			else ArcaneBlastTimer -= diff;

			if (StarfallTimer <= diff)
			{
				DoCastAOE(SPELL_STARFALL);
				StarfallTimer = urand(6000, 7000);
			}
			else StarfallTimer -= diff;

			if (DominationTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
					DoCast(target, SPELL_DOMINATION);
					DominationTimer = urand(10000, 12000);
				}
			}
			else DominationTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_star_scryerAI(creature);
    }
};

class npc_astromancer : public CreatureScript
{
public:
    npc_astromancer() : CreatureScript("npc_astromancer") {}

    struct npc_astromancerAI : public QuantumBasicAI
    {
        npc_astromancerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 FireballVolleyTimer;
		uint32 FireBlastTimer;

        void Reset()
		{
			FireballVolleyTimer = 1000;
			FireBlastTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_MOLTEN_ARMOR);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireballVolleyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FIREBALL_VOLLEY);
					FireballVolleyTimer = urand(2000, 3000);
				}
			}
			else FireballVolleyTimer -= diff;

			if (FireBlastTimer <= diff)
			{
				DoCastVictim(SPELL_FIRE_BLAST);
				FireBlastTimer = urand(4000, 5000);
			}
			else FireBlastTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_astromancerAI(creature);
    }
};

class npc_bloodwarder_legionnaire : public CreatureScript
{
public:
    npc_bloodwarder_legionnaire() : CreatureScript("npc_bloodwarder_legionnaire") {}

    struct npc_bloodwarder_legionnaireAI : public QuantumBasicAI
    {
        npc_bloodwarder_legionnaireAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 CleaveTimer;
		uint32 WhirlwindTimer;
		uint32 BloodthirstTimer;

        void Reset()
		{
			CleaveTimer = 2000;
			WhirlwindTimer = 4000;
			BloodthirstTimer = 6000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_DUAL_WIELD, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastAOE(SPELL_CLEAVE);
				CleaveTimer = urand(3000, 4000);
			}
			else CleaveTimer -= diff;

			if (WhirlwindTimer <= diff)
			{
				DoCastAOE(SPELL_WHIRLWIND);
				WhirlwindTimer = urand(6000, 7000);
			}
			else WhirlwindTimer -= diff;

			if (BloodthirstTimer <= diff)
			{
				DoCastVictim(SPELL_BLOODTHIRST);
				BloodthirstTimer = urand(9000, 10000);
			}
			else BloodthirstTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodwarder_legionnaireAI(creature);
    }
};

class npc_bloodwarder_vindicator : public CreatureScript
{
public:
    npc_bloodwarder_vindicator() : CreatureScript("npc_bloodwarder_vindicator") {}

    struct npc_bloodwarder_vindicatorAI : public QuantumBasicAI
    {
        npc_bloodwarder_vindicatorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 CleanseTimer;
		uint32 FlashOfLightTimer;
		uint32 HammerOfJusticeTimer;
		uint32 HammerOfWrathTimer;

        void Reset()
		{
			CleanseTimer = 2000;
			FlashOfLightTimer = 3000;
			HammerOfJusticeTimer = 5000;
			HammerOfWrathTimer = 7000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleanseTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, SPELL_CLEANSE);
					CleanseTimer = urand(3000, 4000);
				}
			}
			else CleanseTimer -= diff;

			if (FlashOfLightTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, SPELL_FLASH_OF_LIGHT);
					FlashOfLightTimer = urand(5000, 6000);
				}
			}
			else FlashOfLightTimer -= diff;

			if (HammerOfJusticeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_HAMMER_OF_JUSTICE);
					HammerOfJusticeTimer = urand(7000, 8000);
				}
			}
			else HammerOfJusticeTimer -= diff;

			if (HammerOfWrathTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && HealthBelowPct(HEALTH_PERCENT_20))
					{
						DoCast(target, SPELL_HAMMER_OF_WRATH);
						HammerOfWrathTimer = urand(9000, 10000);
					}
				}
			}
			else HammerOfWrathTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_60))
				DoCast(me, SPELL_FLASH_OF_LIGHT);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodwarder_vindicatorAI(creature);
    }
};

class npc_bloodwarder_marshal : public CreatureScript
{
public:
    npc_bloodwarder_marshal() : CreatureScript("npc_bloodwarder_marshal") {}

    struct npc_bloodwarder_marshalAI : public QuantumBasicAI
    {
        npc_bloodwarder_marshalAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 WhirlwindTimer;
		uint32 BloodthirstTimer;
		uint32 UppercutTimer;

        void Reset()
		{
			WhirlwindTimer = 2000;
			BloodthirstTimer = 4000;
			UppercutTimer = 6000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_DUAL_WIELD, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WhirlwindTimer <= diff)
			{
				DoCastAOE(SPELL_BL_WHIRLWIND);
				WhirlwindTimer = urand(3000, 4000);
			}
			else WhirlwindTimer -= diff;

			if (BloodthirstTimer <= diff)
			{
				DoCastVictim(SPELL_BLOODTHIRST);
				BloodthirstTimer = urand(6000, 7000);
			}
			else BloodthirstTimer -= diff;

			if (UppercutTimer <= diff)
			{
				DoCastVictim(SPELL_UPPERCUT);
				UppercutTimer = urand(8000, 9000);
			}
			else UppercutTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodwarder_marshalAI(creature);
    }
};

class npc_bloodwarder_squire : public CreatureScript
{
public:
    npc_bloodwarder_squire() : CreatureScript("npc_bloodwarder_squire") {}

    struct npc_bloodwarder_squireAI : public QuantumBasicAI
    {
        npc_bloodwarder_squireAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 CleanseTimer;
		uint32 FlashOfLightTimer;
		uint32 HammerOfJusticeTimer;
		uint32 HammerOfWrathTimer;

        void Reset()
		{
			CleanseTimer = 2000;
			FlashOfLightTimer = 3000;
			HammerOfJusticeTimer = 5000;
			HammerOfWrathTimer = 7000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleanseTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, SPELL_CLEANSE);
					CleanseTimer = urand(3000, 4000);
				}
			}
			else CleanseTimer -= diff;

			if (FlashOfLightTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, SPELL_BS_FLASH_OF_LIGHT);
					FlashOfLightTimer = urand(5000, 6000);
				}
			}
			else FlashOfLightTimer -= diff;

			if (HammerOfJusticeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BS_HAMMER_OF_JUSTICE);
					HammerOfJusticeTimer = urand(7000, 8000);
				}
			}
			else HammerOfJusticeTimer -= diff;

			if (HammerOfWrathTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && HealthBelowPct(HEALTH_PERCENT_20))
					{
						DoCast(target, SPELL_BS_HAMMER_OF_WRATH);
						HammerOfWrathTimer = urand(9000, 10000);
					}
				}
			}
			else HammerOfWrathTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_60))
				DoCast(me, SPELL_BS_FLASH_OF_LIGHT);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodwarder_squireAI(creature);
    }
};

class npc_tempest_falconer : public CreatureScript
{
public:
    npc_tempest_falconer() : CreatureScript("npc_tempest_falconer") {}

    struct npc_tempest_falconerAI : public QuantumBasicAI
    {
        npc_tempest_falconerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 ShootTimer;
		uint32 ImmolationArrowTimer;
		uint32 KnockbackTimer;

        void Reset()
        {
			ShootTimer = 2000;
			ImmolationArrowTimer = 4000;
			KnockbackTimer = 6000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_FIRE_SHIELD);
		}

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
						DoCast(target, SPELL_SHOOT);
						ShootTimer = urand(3000, 4000);
					}
				}
			}
			else ShootTimer -= diff;

			if (ImmolationArrowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_IMMOLATION_ARROW);
						ImmolationArrowTimer = urand(5000, 6000);
					}
				}
			}
			else ImmolationArrowTimer -= diff;

			if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    DoStartMovement(me->GetVictim());

				if (KnockbackTimer <= diff)
				{
					DoCastVictim(SPELL_KNOCKBACK);
					KnockbackTimer = urand(7000, 8000);
				}
				else KnockbackTimer -= diff;

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
						ShootTimer = urand(9000, 10000);
					}
				}
				else ShootTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tempest_falconerAI(creature);
    }
};

class npc_phoenix_hawk_hatchling : public CreatureScript
{
public:
    npc_phoenix_hawk_hatchling() : CreatureScript("npc_phoenix_hawk_hatchling") {}

    struct npc_phoenix_hawk_hatchlingAI : public QuantumBasicAI
    {
        npc_phoenix_hawk_hatchlingAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 SilenceTimer;
		uint32 WingBuffetTimer;

        void Reset()
		{
			SilenceTimer = 2000;
			WingBuffetTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SilenceTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SILENCE);
					SilenceTimer = urand(3000, 4000);
				}
			}
			else SilenceTimer -= diff;

			if (WingBuffetTimer <= diff)
			{
				DoCastAOE(SPELL_WING_BUFFET);
				WingBuffetTimer = urand(5000, 6000);
			}
			else WingBuffetTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_phoenix_hawk_hatchlingAI(creature);
    }
};

class npc_phoenix_hawk : public CreatureScript
{
public:
    npc_phoenix_hawk() : CreatureScript("npc_phoenix_hawk") {}

    struct npc_phoenix_hawkAI : public QuantumBasicAI
    {
        npc_phoenix_hawkAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 DiveTimer;
		uint32 ManaBurnTimer;

        void Reset()
		{
			DiveTimer = 2000;
			ManaBurnTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DiveTimer <= diff)
			{
				DoCastVictim(SPELL_DIVE);
				DiveTimer = urand(3000, 4000);
			}
			else DiveTimer -= diff;

			if (ManaBurnTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_MANA_BURN);
						ManaBurnTimer = urand(5000, 6000);
					}
				}
			}
			else ManaBurnTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_phoenix_hawkAI(creature);
    }
};

class npc_tempest_smith : public CreatureScript
{
public:
    npc_tempest_smith() : CreatureScript("npc_tempest_smith") {}

    struct npc_tempest_smithAI : public QuantumBasicAI
    {
        npc_tempest_smithAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 GolemRepairTimer;
		uint32 FragmentationBombTimer;
		uint32 ShellShockTimer;
		uint32 SpellPowerUpTimer;

        void Reset()
		{
			GolemRepairTimer = 100;
			FragmentationBombTimer = 500;
			ShellShockTimer = 2000;
			SpellPowerUpTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->InterruptSpell(CURRENT_CHANNELED_SPELL);
		}

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (GolemRepairTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* devastator = me->FindCreatureWithDistance(NPC_CRYSTALCORE_DEVASTATOR, 25.0f))
				{
					me->SetFacingToObject(devastator);
					DoCast(devastator, SPELL_GOLEM_REPAIR, true);
					GolemRepairTimer = 120000; // 2 Minutes
				}
			}
			else GolemRepairTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FragmentationBombTimer <= diff)
			{
				DoCastVictim(SPELL_FRAGMENTATION_BOMB);
				FragmentationBombTimer = urand(4000, 5000);
			}
			else FragmentationBombTimer -= diff;

			if (ShellShockTimer <= diff)
			{
				DoCastVictim(SPELL_SHELL_SHOCK);
				ShellShockTimer = urand(6000, 8000);
			}
			else ShellShockTimer -= diff;

			if (SpellPowerUpTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, SPELL_POWER_UP, true);
					SpellPowerUpTimer = urand(10000, 12000);
				}
			}
			else SpellPowerUpTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tempest_smithAI(creature);
    }
};

class npc_crystalcore_mechanic : public CreatureScript
{
public:
    npc_crystalcore_mechanic() : CreatureScript("npc_crystalcore_mechanic") {}

    struct npc_crystalcore_mechanicAI : public QuantumBasicAI
    {
        npc_crystalcore_mechanicAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 SawBladeTimer;
		uint32 RechargeTimer;

        void Reset()
		{
			SawBladeTimer = 1000;
			RechargeTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SawBladeTimer <= diff)
			{
				DoCastVictim(SPELL_SAW_BLADE);
				SawBladeTimer = urand(3000, 4000);
			}
			else SawBladeTimer -= diff;

			if (RechargeTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, SPELL_RECHARGE);
					RechargeTimer = urand(7000, 8000);
				}
			}
			else RechargeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crystalcore_mechanicAI(creature);
    }
};

class npc_apprentice_star_scryer : public CreatureScript
{
public:
    npc_apprentice_star_scryer() : CreatureScript("npc_apprentice_star_scryer") {}

    struct npc_apprentice_star_scryerAI : public QuantumBasicAI
    {
        npc_apprentice_star_scryerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 ArcaneBuffetTimer;
		uint32 ArcaneShockTimer;
		uint32 ArcaneVolleyTimer;
		uint32 ArcaneExplosionTimer;

        void Reset()
		{
			ArcaneBuffetTimer = 1000;
			ArcaneShockTimer = 3000;
			ArcaneVolleyTimer = 4000;
			ArcaneExplosionTimer = 5000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ArcaneBuffetTimer <= diff)
			{
				DoCastVictim(SPELL_ARCANE_BUFFET);
				ArcaneBuffetTimer = urand(2000, 3000);
			}
			else ArcaneBuffetTimer -= diff;

			if (ArcaneShockTimer <= diff)
			{
				DoCastVictim(SPELL_ARCANE_SHOCK);
				ArcaneShockTimer = urand(4000, 5000);
			}
			else ArcaneShockTimer -= diff;

			if (ArcaneVolleyTimer <= diff)
			{
				DoCastVictim(SPELL_ARCANE_VOLLEY);
				ArcaneVolleyTimer = urand(6000, 7000);
			}
			else ArcaneVolleyTimer -= diff;

			if (ArcaneExplosionTimer <= diff)
			{
				DoCastAOE(SPELL_ARCANE_EXPLOSION);
				ArcaneExplosionTimer = urand(8000, 9000);
			}
			else ArcaneExplosionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_apprentice_star_scryerAI(creature);
    }
};

class npc_novice_astromancer : public CreatureScript
{
public:
    npc_novice_astromancer() : CreatureScript("npc_novice_astromancer") {}

    struct npc_novice_astromancerAI : public QuantumBasicAI
    {
        npc_novice_astromancerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 FireballTimer;
		uint32 FireNovaTimer;
		uint32 RainOfFireTimer;

        void Reset()
		{
			FireballTimer = 2000;
			FireNovaTimer = 4000;
			RainOfFireTimer = 5000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_NA_FIRE_SHIELD);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireballTimer <= diff)
			{
				DoCastVictim(SPELL_FIREBALL);
				FireballTimer = urand(3000, 4000);
			}
			else FireballTimer -= diff;

			if (FireNovaTimer <= diff)
			{
				DoCastAOE(SPELL_FIRE_NOVA);
				FireNovaTimer = urand(5000, 6000);
			}
			else FireNovaTimer -= diff;

			if (RainOfFireTimer <= diff)
			{
				DoCastAOE(SPELL_RAIN_OF_FIRE);
				RainOfFireTimer = urand(7000, 8000);
			}
			else RainOfFireTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_novice_astromancerAI(creature);
    }
};

class npc_astromancer_lord : public CreatureScript
{
public:
    npc_astromancer_lord() : CreatureScript("npc_astromancer_lord") {}

    struct npc_astromancer_lordAI : public QuantumBasicAI
    {
        npc_astromancer_lordAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 FireballVolleyTimer;
		uint32 FireBlastTimer;
		uint32 DragonsBreathTimer;

        void Reset()
		{
			FireballVolleyTimer = 1000;
			FireBlastTimer = 2000;
			DragonsBreathTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_NA_FIRE_SHIELD);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireballVolleyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FIREBALL_VOLLEY);
					FireballVolleyTimer = urand(2000, 3000);
				}
			}
			else FireballVolleyTimer -= diff;

			if (FireBlastTimer <= diff)
			{
				DoCastVictim(SPELL_FIRE_BLAST);
				FireBlastTimer = urand(4000, 5000);
			}
			else FireBlastTimer -= diff;

			if (DragonsBreathTimer <= diff)
			{
				DoCastAOE(SPELL_DRAGONS_BREATH);
				DragonsBreathTimer = urand(6000, 7000);
			}
			else DragonsBreathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_astromancer_lordAI(creature);
    }
};

class npc_nether_scryer : public CreatureScript
{
public:
    npc_nether_scryer() : CreatureScript("npc_nether_scryer") {}

    struct npc_nether_scryerAI : public QuantumBasicAI
    {
        npc_nether_scryerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 ArcaneBlastTimer;
		uint32 DominationTimer;

        void Reset()
		{
			ArcaneBlastTimer = 1000;
			DominationTimer = 5000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ArcaneBlastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ARCANE_BLAST);
					ArcaneBlastTimer = urand(3000, 4000);
				}
			}
			else ArcaneBlastTimer -= diff;

			if (DominationTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_NS_DOMINATION);
					DominationTimer = urand(10000, 12000);
				}
			}
			else DominationTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nether_scryerAI(creature);
    }
};

class npc_netherstrand_longbow : public CreatureScript
{
public:
    npc_netherstrand_longbow() : CreatureScript("npc_netherstrand_longbow") {}

    struct npc_netherstrand_longbowAI : public QuantumBasicAI
    {
        npc_netherstrand_longbowAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISARM, true);
		}

		bool Blink;

		uint32 ShootTimer;
		uint32 MultiShotTimer;

        void Reset()
        {
			Blink = true;

			ShootTimer = 500;
			MultiShotTimer = 1000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_RANGED_WEAPON);
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
						DoCast(target, SPELL_NL_SHOOT);
						ShootTimer = urand(2000, 3000);
					}
				}
			}
			else ShootTimer -= diff;

			if (MultiShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_MULTI_SHOT);
						MultiShotTimer = urand(4000, 5000);
					}
				}
			}
			else MultiShotTimer -= diff;

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

				if (ShootTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, SPELL_NL_SHOOT);
						ShootTimer = urand(6000, 7000);
					}
				}
				else ShootTimer -= diff;

				if (HealthBelowPct(HEALTH_PERCENT_50))
				{
					if (Blink)
					{
						DoCast(me, SPELL_BLINK);
						Blink = false;
					}
				}
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_netherstrand_longbowAI(creature);
    }
};

class npc_devastation : public CreatureScript
{
public:
    npc_devastation() : CreatureScript("npc_devastation") {}

    struct npc_devastationAI : public QuantumBasicAI
    {
        npc_devastationAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISARM, true);
		}

		uint32 WhirlwindTimer;

        void Reset()
		{
			WhirlwindTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WhirlwindTimer <= diff)
			{
				DoCastAOE(SPELL_DEVASTATION_WHIRLWIND);
				WhirlwindTimer = urand(8000, 9000);
			}
			else WhirlwindTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_devastationAI(creature);
    }
};

class npc_cosmic_infuser : public CreatureScript
{
public:
    npc_cosmic_infuser() : CreatureScript("npc_cosmic_infuser") {}

    struct npc_cosmic_infuserAI : public QuantumBasicAI
    {
        npc_cosmic_infuserAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISARM, true);
		}

		uint32 HealTimer;
		uint32 HolyNovaTimer;

        void Reset()
		{
			HealTimer = 2000;
			HolyNovaTimer = 5000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, SPELL_HEAL);
					HealTimer = urand(5000, 6000);
				}
			}
			else HealTimer -= diff;

			if (HolyNovaTimer <= diff)
			{
				DoCastAOE(SPELL_HOLY_NOVA);
				HolyNovaTimer = urand(7000, 8000);
			}
			else HolyNovaTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
				DoCast(me, SPELL_HEAL);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cosmic_infuserAI(creature);
    }
};

class npc_infinity_blades : public CreatureScript
{
public:
    npc_infinity_blades() : CreatureScript("npc_infinity_blades") {}

    struct npc_infinity_bladesAI : public QuantumBasicAI
    {
        npc_infinity_bladesAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISARM, true);
		}

        void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_THRASH);
		}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_infinity_bladesAI(creature);
    }
};

class npc_warp_slicer : public CreatureScript
{
public:
    npc_warp_slicer() : CreatureScript("npc_warp_slicer") {}

    struct npc_warp_slicerAI : public QuantumBasicAI
    {
        npc_warp_slicerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISARM, true);
		}

		uint32 RendTimer;

        void Reset()
		{
			RendTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RendTimer <= diff)
			{
				DoCastVictim(SPELL_REND);
				RendTimer = urand(4000, 5000);
			}
			else RendTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_warp_slicerAI(creature);
    }
};

class npc_phaseshift_bulwark : public CreatureScript
{
public:
    npc_phaseshift_bulwark() : CreatureScript("npc_phaseshift_bulwark") {}

    struct npc_phaseshift_bulwarkAI : public QuantumBasicAI
    {
        npc_phaseshift_bulwarkAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISARM, true);
		}

		uint32 ShieldBashTimer;

        void Reset()
		{
			ShieldBashTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShieldBashTimer <= diff)
			{
				DoCastVictim(SPELL_SHIELD_BASH);
				ShieldBashTimer = urand(5000, 6000);
			}
			else ShieldBashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_phaseshift_bulwarkAI(creature);
    }
};

class npc_staff_of_disintegration : public CreatureScript
{
public:
    npc_staff_of_disintegration() : CreatureScript("npc_staff_of_disintegration") {}

    struct npc_staff_of_disintegrationAI : public QuantumBasicAI
    {
        npc_staff_of_disintegrationAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISARM, true);
		}

		uint32 FrostboltTimer;
		uint32 FrostNovaTimer;

        void Reset()
		{
			FrostboltTimer = 2000;
			FrostNovaTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FROSTBOLT);
					FrostboltTimer = urand(3000, 4000);
				}
			}
			else FrostboltTimer -= diff;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(SPELL_FROST_NOVA);
				FrostNovaTimer = urand(5000, 6000);
			}
			else FrostNovaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_staff_of_disintegrationAI(creature);
    }
};

void AddSC_tempest_keep_the_eye_trash()
{
	new npc_crystalcore_devastator();
	new npc_crystalcore_sentinel();
	new npc_star_scryer();
	new npc_astromancer();
	new npc_bloodwarder_legionnaire();
	new npc_bloodwarder_vindicator();
	new npc_bloodwarder_marshal();
	new npc_bloodwarder_squire();
	new npc_tempest_falconer();
	new npc_phoenix_hawk_hatchling();
	new npc_phoenix_hawk();
	new npc_tempest_smith();
	new npc_crystalcore_mechanic();
	new npc_apprentice_star_scryer();
	new npc_novice_astromancer();
	new npc_astromancer_lord();
	new npc_nether_scryer();
	new npc_netherstrand_longbow();
	new npc_devastation();
	new npc_cosmic_infuser();
	new npc_infinity_blades();
	new npc_warp_slicer();
	new npc_phaseshift_bulwark();
	new npc_staff_of_disintegration();
}