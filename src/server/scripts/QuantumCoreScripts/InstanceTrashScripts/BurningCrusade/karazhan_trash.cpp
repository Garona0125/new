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

enum KarazhanSpells
{
	SPELL_ABSORB_VITALITY             = 29323,
	SPELL_HOOF_STRIKE                 = 29577,
	SPELL_STALLION_CHARGE             = 29320,
	SPELL_HEALING_TOUCH               = 29339,
	SPELL_KNOCKDOWN                   = 18812,
	SPELL_PIERCE_ARMOR                = 6016,
	SPELL_WHIP_RAGE                   = 29340,
	SPELL_BURNING_BRAND               = 29618,
	SPELL_CHILLING_POISON             = 29290,
	SPELL_CURSE_OF_PART_BURDENS       = 29540,
	SPELL_BRITTLE_BONES               = 32441,
	SPELL_FIREBALL_VOLLEY             = 29922,
	SPELL_FROSTBOLT_VOLLEY            = 29923,
	SPELL_SHADOW_BOLT_VOLLEY          = 29924,
	SPELL_CHEF_CLEAVE                 = 29665,
	SPELL_CHEF_HAMSTRING              = 29667,
	SPELL_ROAST                       = 29675,
	SPELL_ROLLING_PIN                 = 29676,
	SPELL_DRUNKEN_SKULL_CRACK         = 29690,
	SPELL_FRENZY                      = 29691,
	SPELL_DANCE_VIBE                  = 29521,
	SPELL_CURSE_OF_AGONY              = 29930,
	SPELL_IMMOLATE                    = 29928,
	SPELL_SEARING_PAIN                = 29492,
	SPELL_HOLY_NOVA                   = 29514,
	SPELL_HEAL                        = 29580,
	SPELL_DEMORALIZING_SHOUT          = 29584,
	SPELL_SHIELD_SLAM                 = 29684,
	SPELL_SUMMON_PHANTOM_HOUND        = 29537,
	SPELL_SUMMON_PHANTOM_HOUND_VISUAL = 29541,
	SPELL_REND                        = 29574,
	SPELL_SHOOT                       = 29575,
	SPELL_MULTI_SHOT                  = 29576,
	SPELL_CONCUBINE_TRANSFORM         = 29489,
	SPELL_SEDUCTION                   = 29490,
	SPELL_TEMPTATION                  = 29494,
	SPELL_JEALOUSY                    = 29497,
	SPELL_TORMENTING_LASH             = 15969,
	SPELL_SHADOW_BOLT                 = 29487,
	SPELL_NM_SEARING_PAIN             = 30358,
	SPELL_IMPENDING_BETRAYAL          = 29491,
	SPELL_NIGHT_MISTRESS_TRANSFORM    = 29488,
	SPELL_WANTON_HOSTESS_TRANSFORM    = 29472,
	SPELL_ALURING_AURA                = 29485,
	SPELL_BEWITCHING_AURA             = 29486,
	SPELL_BANSHEE_WAIL                = 29477,
	SPELL_BANSHEE_SHRIEK              = 29505,
	SPELL_OATH_OF_FEALTY              = 29546,
	SPELL_SR_REND                     = 29578,
	SPELL_MAGNETIC_PULL               = 29661,
	SPELL_FROST_SHOCK                 = 29666,
	SPELL_ICE_TOMB                    = 29670,
	SPELL_SANDBAG                     = 29673,
	SPELL_MALLET_TOSS                 = 29677,
	SPELL_NET                         = 41580,
	SPELL_BAD_POETRY                  = 29679,
	SPELL_CURTAIN_CALL                = 29680,
	SPELL_SUMMON_SPOTLIGHT            = 29683,
	SPELL_SPOTLIGHT_VISUAL            = 25824,
	SPELL_EXHAUSTED                   = 29650,
	SPELL_LEFT_HOOK                   = 29555,
	SPELL_KICK                        = 29560,
	SPELL_ILL_GIFT                    = 29609,
	SPELL_INCITE_RAGE                 = 29612,
	SPELL_CONE_OF_COLD                = 29717,
	SPELL_ELEMENTAL_ARMOR             = 29718,
	SPELL_SHADOW_SHOCK                = 29712,
	SPELL_ETHEREAL_CURSE              = 29716,
	SPELL_CRYSTAL_STRIKE              = 29765,
	SPELL_OVERLOAD                    = 29768,
	SPELL_MANA_SHIELD                 = 29880,
	SPELL_LOOSE_MANA                  = 29882,
	SPELL_BLINK                       = 29883,
	SPELL_ARCANE_VOLLEY               = 29885,
	SPELL_DRAIN_MANA                  = 29881,
	SPELL_UNSTABLE_MAGIC              = 29900,
	SPELL_ASTRAL_BITE                 = 29908,
	SPELL_FIST_OF_STONE               = 29840,
	SPELL_RETURN_FIRE                 = 29788,
	SPELL_MH_ARCANE_VOLLEY            = 37078,
	SPELL_POWER_DISTORTION            = 29911,
	SPELL_WARP_BREATH                 = 29919,
	SPELL_PHASING_INVISIBILITY        = 29920,
	SPELL_FIREBALL                    = 29925,
	SPELL_FROSTBOLT                   = 29926,
	SPELL_SH_SHADOW_BOLT              = 29927,
	SPELL_FIREBOLT                    = 30180,
	SPELL_SPATIAL_DISTORTION_MELEE    = 29982,
	SPELL_DISARM                      = 30013,
	SPELL_ET_CLEAVE                   = 30014,
	SPELL_SPATIAL_DISTORTION_CASTER   = 30007,
	SPELL_STEAL_MAGIC                 = 30036,
	SPELL_TRANSFERENCE                = 30039,
	SPELL_ES_ARCANE_VOLLEY            = 37161,
	SPELL_GAPING_MAW                  = 29935,
	SPELL_INFECTIOUS_POISON           = 29939,
	SPELL_FROST_MIST                  = 29292,
	SPELL_POISON_BOLT_VOLLEY          = 29293,
	SPELL_DARK_SHRIEK                 = 29298,
	SPELL_SONIC_BLAST                 = 29300,
	SPELL_WING_BEAT                   = 29303,
	SPELL_DRAINING_TOUCH              = 32429,
	SPELL_HOWL_OF_THE_BROKEN_HILLS    = 29304,
	SPELL_DB_CLEAVE                   = 29561,
	SPELL_PHASE_SHIFT                 = 29309,
	SPELL_SEAR                        = 29864,
	SPELL_DETONATE                    = 29876,
	SPELL_DETONATE_PROC               = 29870,
	SPELL_ARCANE_DISCHARGE            = 31472,
	SPELL_ARCING_SEAR                 = 30234,
};

enum Texts // Realise be later...
{
	SAY_SPECTRAL_STABLE_AGGRO   = 0,
	SAY_SPECTRAL_STABLE_DEATH   = 1,

	SAY_SRECTRAL_SERVANT_AGGRO  = 0,
	SAY_SRECTRAL_SERVANT_DEATH  = 1,

	SAY_SPECTRAL_SENTRY_AGGRO   = 0,
	SAY_SPECTRAL_SENTRY_DEATH   = 1,

	SAY_PHANTOM_VALET_AGGRO     = 0,
	SAY_PHANTOM_VALET_DEATH     = 1,

	SAY_PHANTOM_GUARDSMAN_AGGRO = 0,
	SAY_PHANTOM_GUARDSMAN_DEATH = 1,

	SAY_CONCUBINE_AGGRO         = 0,
	SAY_CONCUBINE_DEATH         = 1,

	SAY_MISTRESS_AGGRO          = 0,
	SAY_MISTRESS_DEATH          = 1,

	SAY_HOSTESS_AGGRO           = 0,
	SAY_HOSTESS_DEATH           = 1,

	SAY_USHER_AGGRO             = 0,
	SAY_USHER_DEATH             = 1,

	SAY_STAGEHANG_AGGRO         = 0,
	SAY_STAGEHAND_DEATH         = 1,

	SAY_PERFORMER_AGGRO         = 0,
	SAY_PERFORMER_DEATH         = 1,

	SAY_SRECTRAL_PATRON_AGGRO   = 0,
	SAY_SRECTRAL_PATRON_DEATH   = 1,

	SAY_PHILANTHROPIST_AGGRO    = 0,
	SAY_PHILANTHROPIST_DEATH    = 1,

	SAY_TRAPPED_SOUL_AGGRO      = 0,
	SAY_TRAPPED_SOUL_DEATH      = 1,

	SAY_GHASTLY_HAUNT_AGGRO     = 0,
	SAY_GHASTLY_HAUNT_DEATH     = 1,

	SAY_ARCANE_WATCHMAN_AGGRO   = 0,
	SAY_ARCANE_WATCHMAN_DEATH   = 1,

	SAY_ARCANE_PROTECTOR_AGGRO  = 0,
	SAY_ARCANE_PROTECTOR_DEATH  = 1,

	SAY_PILLAGER_AGGRO          = 0,
	SAY_PILLAGER_DEATH          = 1,

	SAY_THIEF_AGGRO             = 0,
	SAY_THIEF_DEATH             = 1,

	SAY_SPELLFILCHER_AGGRO      = 0,
	SAY_SPELLFILCHER_DEATH      = 1,
};

enum Creatures
{
	NPC_PHANTOM_HOUND = 17067,
	NPC_ASTRAL_SPARK  = 17283,
	NPC_SPOTLIGHT     = 18342,
};

class npc_spectral_stallion : public CreatureScript
{
public:
	npc_spectral_stallion() : CreatureScript("npc_spectral_stallion") {}

    struct npc_spectral_stallionAI : public QuantumBasicAI
    {
        npc_spectral_stallionAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 AbsorbVitalityTimer;
		uint32 HoofStrikeTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			AbsorbVitalityTimer = 2000;
			HoofStrikeTimer = 3000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (AbsorbVitalityTimer <= diff)
			{
				DoCast(me, SPELL_ABSORB_VITALITY);
				AbsorbVitalityTimer = urand(3000, 4000);
			}
			else AbsorbVitalityTimer -= diff;

			if (HoofStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_HOOF_STRIKE);
				HoofStrikeTimer = urand(4000, 5000);
			}
			else HoofStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spectral_stallionAI(creature);
    }
};

class npc_spectral_charger : public CreatureScript
{
public:
	npc_spectral_charger() : CreatureScript("npc_spectral_charger") {}

    struct npc_spectral_chargerAI : public QuantumBasicAI
    {
        npc_spectral_chargerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_STALLION_CHARGE);
		}

		void UpdateAI(const uint32 /*diff*/)
		{
			if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spectral_chargerAI(creature);
    }
};

class npc_spectral_stable_hand : public CreatureScript
{
public:
	npc_spectral_stable_hand() : CreatureScript("npc_spectral_stable_hand") {}

    struct npc_spectral_stable_handAI : public QuantumBasicAI
    {
        npc_spectral_stable_handAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 WhipRageTimer;
		uint32 PierceArmorTimer;
		uint32 KnockdownTimer;
		uint32 HealingTouchTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			WhipRageTimer = 2000;
			PierceArmorTimer = 3000;
			KnockdownTimer = 4000;
			HealingTouchTimer = 7000;
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* /*killer*/)
		{
			// Talk(SAY_SPECTRAL_STABLE_DEATH);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WhipRageTimer <= diff)
			{
				DoCastAOE(SPELL_WHIP_RAGE);
				WhipRageTimer = urand(15000, 16000);
			}
			else WhipRageTimer -= diff;

			if (PierceArmorTimer <= diff)
			{
				DoCastVictim(SPELL_PIERCE_ARMOR);
				PierceArmorTimer = urand(4000, 5000);
			}
			else PierceArmorTimer -= diff;

			if (KnockdownTimer <= diff)
			{
				DoCastVictim(SPELL_KNOCKDOWN);
				KnockdownTimer = urand(5000, 6000);
			}
			else KnockdownTimer -= diff;

			if (HealingTouchTimer <= diff)
			{
				DoCast(me, SPELL_HEALING_TOUCH);
				HealingTouchTimer = urand(8000, 9000);
			}
			else HealingTouchTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spectral_stable_handAI(creature);
    }
};

class npc_spectral_apprentice : public CreatureScript
{
public:
	npc_spectral_apprentice() : CreatureScript("npc_spectral_apprentice") {}

    struct npc_spectral_apprenticeAI : public QuantumBasicAI
    {
        npc_spectral_apprenticeAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 BurningBrandTimer;

		void Reset()
        {
			BurningBrandTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_WORK_MINING);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BurningBrandTimer <= diff)
			{
				DoCastVictim(SPELL_BURNING_BRAND);
				BurningBrandTimer = urand(3000, 4000);
			}
			else BurningBrandTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spectral_apprenticeAI(creature);
    }
};

class npc_spectral_sentry : public CreatureScript
{
public:
    npc_spectral_sentry() : CreatureScript("npc_spectral_sentry") {}

    struct npc_spectral_sentryAI : public QuantumBasicAI
    {
		npc_spectral_sentryAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ShootTimer;
		uint32 MultiShotTimer;

        void Reset()
        {
			ShootTimer = 1000;
			MultiShotTimer = 2000;
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* /*killer*/)
		{
			// Talk(SAY_SPECTRAL_SENTRY_DEATH);
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
						DoCast(target, SPELL_SHOOT);
						ShootTimer = urand(5000, 6000);
					}
				}
				else ShootTimer -= diff;
			}

			if (MultiShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_MULTI_SHOT);
						MultiShotTimer = urand(7000, 8000);
					}
				}
			}
			else MultiShotTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spectral_sentryAI(creature);
    }
};

class npc_spectral_servant : public CreatureScript
{
public:
	npc_spectral_servant() : CreatureScript("npc_spectral_servant") {}

    struct npc_spectral_servantAI : public QuantumBasicAI
    {
        npc_spectral_servantAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CurseOfPastBurdensTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			CurseOfPastBurdensTimer = 3000;
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_CURSE_OF_PART_BURDENS);
		}

		void JustDied(Unit* /*killer*/)
		{
			// Talk(SAY_SRECTRAL_SERVANT_DEATH);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CurseOfPastBurdensTimer <= diff)
			{
				DoCastVictim(SPELL_CURSE_OF_PART_BURDENS);
				CurseOfPastBurdensTimer = urand(4000, 5000);
			}
			else CurseOfPastBurdensTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spectral_servantAI(creature);
    }
};

class npc_spectral_chef : public CreatureScript
{
public:
	npc_spectral_chef() : CreatureScript("npc_spectral_chef") {}

    struct npc_spectral_chefAI : public QuantumBasicAI
    {
        npc_spectral_chefAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CleaveTimer;
		uint32 HamstringTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			CleaveTimer = 2000;
			HamstringTimer = 3000;
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
				DoCastVictim(SPELL_CHEF_CLEAVE);
				CleaveTimer = urand(3000, 4000);
			}
			else CleaveTimer -= diff;

			if (HamstringTimer <= diff)
			{
				DoCastVictim(SPELL_CHEF_HAMSTRING);
				HamstringTimer = urand(5000, 6000);
			}
			else HamstringTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spectral_chefAI(creature);
    }
};

class npc_spectral_retainer : public CreatureScript
{
public:
	npc_spectral_retainer() : CreatureScript("npc_spectral_retainer") {}

    struct npc_spectral_retainerAI : public QuantumBasicAI
    {
        npc_spectral_retainerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 RendTimer;
		uint32 OathOfFealtyTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			RendTimer = 2000;
			OathOfFealtyTimer = 4000;
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
				DoCastVictim(SPELL_SR_REND);
				RendTimer = urand(3000, 4000);
			}
			else RendTimer -= diff;

			if (OathOfFealtyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_OATH_OF_FEALTY);
					OathOfFealtyTimer = urand(7000, 8000);
				}
			}
			else OathOfFealtyTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spectral_retainerAI(creature);
    }
};

class npc_spectral_performer : public CreatureScript
{
public:
	npc_spectral_performer() : CreatureScript("npc_spectral_performer") {}

    struct npc_spectral_performerAI : public QuantumBasicAI
    {
        npc_spectral_performerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 BadPoetryTimer;
		uint32 CurtainCallTimer;
		uint32 SpotlightTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			BadPoetryTimer = 1000;
			CurtainCallTimer = 3000;
			SpotlightTimer = 5000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BadPoetryTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BAD_POETRY);
					BadPoetryTimer = urand(3000, 4000);
				}
			}
			else BadPoetryTimer -= diff;

			if (CurtainCallTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CURTAIN_CALL);
					CurtainCallTimer = urand(6000, 7000);
				}
			}
			else CurtainCallTimer -= diff;

			if (SpotlightTimer <= diff)
			{
				DoCast(me, SPELL_SUMMON_SPOTLIGHT);
				SpotlightTimer = urand(9000, 10000);
			}
			else SpotlightTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spectral_performerAI(creature);
    }
};

class npc_spectral_patron : public CreatureScript
{
public:
	npc_spectral_patron() : CreatureScript("npc_spectral_patron") {}

    struct npc_spectral_patronAI : public QuantumBasicAI
    {
        npc_spectral_patronAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 LeftHookTimer;
		uint32 KickTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			LeftHookTimer = 2000;
			KickTimer = 4000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (LeftHookTimer <= diff)
			{
				DoCastVictim(SPELL_LEFT_HOOK);
				LeftHookTimer = urand(3000, 4000);
			}
			else LeftHookTimer -= diff;

			if (KickTimer <= diff)
			{
				DoCastVictim(SPELL_KICK);
				KickTimer = urand(5000, 6000);
			}
			else KickTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_30))
			{
				if (!me->HasAuraEffect(SPELL_EXHAUSTED, 0))
					DoCast(me, SPELL_EXHAUSTED);
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spectral_patronAI(creature);
    }
};

class npc_ghostly_baker : public CreatureScript
{
public:
	npc_ghostly_baker() : CreatureScript("npc_ghostly_baker") {}

    struct npc_ghostly_bakerAI : public QuantumBasicAI
    {
        npc_ghostly_bakerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 RoastTimer;
		uint32 RollingPinTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			RoastTimer = 2000;
			RollingPinTimer = 4000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RoastTimer <= diff)
			{
				DoCastVictim(SPELL_ROAST);
				RoastTimer = urand(3000, 4000);
			}
			else RoastTimer -= diff;

			if (RollingPinTimer <= diff)
			{
				DoCastVictim(SPELL_ROLLING_PIN);
				RollingPinTimer = urand(8000, 9000);
			}
			else RollingPinTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ghostly_bakerAI(creature);
    }
};

class npc_ghostly_steward : public CreatureScript
{
public:
	npc_ghostly_steward() : CreatureScript("npc_ghostly_steward") {}

    struct npc_ghostly_stewardAI : public QuantumBasicAI
    {
        npc_ghostly_stewardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 DrunkenSkullCrackTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			DrunkenSkullCrackTimer = 2000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DrunkenSkullCrackTimer <= diff)
			{
				DoCastVictim(SPELL_DRUNKEN_SKULL_CRACK);
				DrunkenSkullCrackTimer = urand(5000, 6000);
			}
			else DrunkenSkullCrackTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_30))
			{
				if (!me->HasAuraEffect(SPELL_FRENZY, 0))
				{
					DoCast(me, SPELL_FRENZY);
					DoSendQuantumText(SAY_GENERIC_EMOTE_FRENZY, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ghostly_stewardAI(creature);
    }
};

class npc_ghostly_philanthropist : public CreatureScript
{
public:
	npc_ghostly_philanthropist() : CreatureScript("npc_ghostly_philanthropist") {}

    struct npc_ghostly_philanthropistAI : public QuantumBasicAI
    {
        npc_ghostly_philanthropistAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 IllGiftTimer;
		uint32 InciteRageTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			IllGiftTimer = 1000;
			InciteRageTimer = 4000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (IllGiftTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ILL_GIFT);
					IllGiftTimer = urand(3000, 4000);
				}
			}
			else IllGiftTimer -= diff;

			if (InciteRageTimer <= diff)
			{
				DoCast(me, SPELL_INCITE_RAGE);
				InciteRageTimer = urand(8000, 9000);
			}
			else InciteRageTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ghostly_philanthropistAI(creature);
    }
};

class npc_skeletal_waiter : public CreatureScript
{
public:
	npc_skeletal_waiter() : CreatureScript("npc_skeletal_waiter") {}

    struct npc_skeletal_waiterAI : public QuantumBasicAI
    {
        npc_skeletal_waiterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 BrittleBonesTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			BrittleBonesTimer = 3000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BrittleBonesTimer <= diff)
			{
				DoCastVictim(SPELL_BRITTLE_BONES);
				BrittleBonesTimer = urand(4000, 5000);
			}
			else BrittleBonesTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skeletal_waiterAI(creature);
    }
};

class npc_skeletal_usher : public CreatureScript
{
public:
	npc_skeletal_usher() : CreatureScript("npc_skeletal_usher") {}

    struct npc_skeletal_usherAI : public QuantumBasicAI
    {
        npc_skeletal_usherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FrostShockTimer;
		uint32 IceTombTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			FrostShockTimer = 1000;
			IceTombTimer = 3000;
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_MAGNETIC_PULL);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FROST_SHOCK);
					FrostShockTimer = urand(3000, 4000);
				}
			}
			else FrostShockTimer -= diff;

			if (IceTombTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ICE_TOMB);

					if (DoGetThreat(me->GetVictim()))
						DoModifyThreatPercent(me->GetVictim(), -100);

					IceTombTimer = urand(6000, 8000);
				}
			}
			else IceTombTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skeletal_usherAI(creature);
    }
};

class npc_phantom_guest : public CreatureScript
{
public:
	npc_phantom_guest() : CreatureScript("npc_phantom_guest") {}

    struct npc_phantom_guestAI : public QuantumBasicAI
    {
        npc_phantom_guestAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CurseOfAgonyTimer;
		uint32 ImmolateTimer;
		uint32 SearingPainTimer;
		uint32 HolyNovaTimer;

		void Reset()
        {
			DoCast(me, SPELL_DANCE_VIBE);
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			CurseOfAgonyTimer = 1000;
			ImmolateTimer = 2000;
			SearingPainTimer = 4000;
			HolyNovaTimer = 6000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CurseOfAgonyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CURSE_OF_AGONY);
					CurseOfAgonyTimer = urand(4000, 5000);
				}
			}
			else CurseOfAgonyTimer -= diff;

			if (ImmolateTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_IMMOLATE);
					ImmolateTimer = urand(6000, 7000);
				}
			}
			else ImmolateTimer -= diff;

			if (SearingPainTimer <= diff)
			{
				DoCastVictim(SPELL_SEARING_PAIN);
				SearingPainTimer = urand(8000, 9000);
			}
			else SearingPainTimer -= diff;

			if (HolyNovaTimer <= diff)
			{
				DoCastAOE(SPELL_HOLY_NOVA);
				HolyNovaTimer = urand(10000, 11000);
			}
			else HolyNovaTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
				DoCast(me, SPELL_HEAL);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_phantom_guestAI(creature);
    }
};

class npc_phantom_valet : public CreatureScript
{
public:
	npc_phantom_valet() : CreatureScript("npc_phantom_valet") {}

    struct npc_phantom_valetAI : public QuantumBasicAI
    {
        npc_phantom_valetAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 DemoralizingShoutTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			DemoralizingShoutTimer = 2000;
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* /*killer*/)
		{
			// Talk(SAY_PHANTOM_VALET_DEATH);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DemoralizingShoutTimer <= diff)
			{
				DoCastAOE(SPELL_DEMORALIZING_SHOUT);
				DemoralizingShoutTimer = urand(5000, 6000);
			}
			else DemoralizingShoutTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_phantom_valetAI(creature);
    }
};

class npc_phantom_guardsman : public CreatureScript
{
public:
	npc_phantom_guardsman() : CreatureScript("npc_phantom_guardsman") {}

    struct npc_phantom_guardsmanAI : public QuantumBasicAI
    {
        npc_phantom_guardsmanAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShieldSlamTimer;

		SummonList Summons;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			Summons.DespawnAll();

			ShieldSlamTimer = 2000;
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SUMMON_PHANTOM_HOUND);
		}

		void JustDied(Unit* /*killer*/)
		{
			// Talk(SAY_PHANTOM_GUARDSMAN_DEATH);

			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_PHANTOM_HOUND)
			{
				Summons.Summon(summon);
				Summons.DoZoneInCombat();
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShieldSlamTimer <= diff)
			{
				DoCastVictim(SPELL_SHIELD_SLAM);
				ShieldSlamTimer = urand(4000, 5000);
			}
			else ShieldSlamTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_phantom_guardsmanAI(creature);
    }
};

class npc_phantom_hound : public CreatureScript
{
public:
	npc_phantom_hound() : CreatureScript("npc_phantom_hound") {}

    struct npc_phantom_houndAI : public QuantumBasicAI
    {
        npc_phantom_houndAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 RendTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			RendTimer = 2000;
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* /*killer*/)
		{
			DoCast(me, SPELL_SUMMON_PHANTOM_HOUND_VISUAL, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RendTimer <= diff)
			{
				DoCastVictim(SPELL_REND);
				RendTimer = urand(5000, 6000);
			}
			else RendTimer -= diff;

			DoMeleeAttackIfReady();
        }

		Unit* GetRandomTarget()
		{
			return SelectTarget(TARGET_RANDOM);
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_phantom_houndAI(creature);
    }
};

class npc_phantom_stagehand : public CreatureScript
{
public:
	npc_phantom_stagehand() : CreatureScript("npc_phantom_stagehand") {}

    struct npc_phantom_stagehandAI : public QuantumBasicAI
    {
        npc_phantom_stagehandAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 MalletTossTimer;
		uint32 SandbagTimer;
		uint32 NetTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_USE_STANDING);

			MalletTossTimer = 2000;
			SandbagTimer = 4000;
			NetTimer = 6000;
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_MALLET_TOSS);
		}

		void JustDied(Unit* /*killer*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MalletTossTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MALLET_TOSS);
					MalletTossTimer = urand(4000, 5000);
				}
			}
			else MalletTossTimer -= diff;

			if (SandbagTimer <= diff)
			{
				DoCastVictim(SPELL_SANDBAG);
				SandbagTimer = urand(7000, 8000);
			}
			else SandbagTimer -= diff;

			if (NetTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_NET);
					NetTimer = urand(9000, 10000);
				}
			}
			else NetTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_phantom_stagehandAI(creature);
    }
};

class npc_sorcerous_shade : public CreatureScript
{
public:
	npc_sorcerous_shade() : CreatureScript("npc_sorcerous_shade") {}

    struct npc_sorcerous_shadeAI : public QuantumBasicAI
    {
        npc_sorcerous_shadeAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FireballVolleyTimer;
		uint32 FrostboltVolleyTimer;
		uint32 ShadowBoltVolley;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			FireballVolleyTimer = 2000;
			FrostboltVolleyTimer = 3000;
			ShadowBoltVolley = 4000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireballVolleyTimer <= diff)
			{
				DoCastVictim(SPELL_FIREBALL_VOLLEY);
				FireballVolleyTimer = urand(3000, 4000);
			}
			else FireballVolleyTimer -= diff;

			if (FrostboltVolleyTimer <= diff)
			{
				DoCastVictim(SPELL_FROSTBOLT_VOLLEY);
				FrostboltVolleyTimer = urand(5000, 6000);
			}
			else FrostboltVolleyTimer -= diff;

			if (ShadowBoltVolley <= diff)
			{
				DoCastVictim(SPELL_SHADOW_BOLT_VOLLEY);
				ShadowBoltVolley = urand(7000, 8000);
			}
			else ShadowBoltVolley -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sorcerous_shadeAI(creature);
    }
};

class npc_concubine : public CreatureScript
{
public:
	npc_concubine() : CreatureScript("npc_concubine") {}

    struct npc_concubineAI : public QuantumBasicAI
    {
        npc_concubineAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 JealousyTimer;
		uint32 SeductionTimer;
		uint32 TormentingLashTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			JealousyTimer = 2000;
			SeductionTimer = 4000;
			TormentingLashTimer = 6000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (JealousyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_JEALOUSY);
					JealousyTimer = urand(3000, 4000);
				}
			}
			else JealousyTimer -= diff;

			if (TormentingLashTimer <= diff)
			{
				DoCastVictim(SPELL_TORMENTING_LASH);
				TormentingLashTimer = urand(6000, 7000);
			}
			else TormentingLashTimer -= diff;

			if (SeductionTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SEDUCTION);
					SeductionTimer = urand(10000, 11000);
				}
			}
			else SeductionTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_90))
			{
				if (!me->HasAuraEffect(SPELL_CONCUBINE_TRANSFORM, 0))
					DoCast(me, SPELL_CONCUBINE_TRANSFORM);
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_concubineAI(creature);
    }
};

class npc_night_mistress : public CreatureScript
{
public:
	npc_night_mistress() : CreatureScript("npc_night_mistress") {}

    struct npc_night_mistressAI : public QuantumBasicAI
    {
        npc_night_mistressAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShadowBoltTimer;
		uint32 SearingPainTimer;
		uint32 ImpendingBetrayalTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			ShadowBoltTimer = 1000;
			SearingPainTimer = 3000;
			ImpendingBetrayalTimer = 5000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_BOLT);
					ShadowBoltTimer = urand(3000, 4000);
				}
			}
			else ShadowBoltTimer -= diff;

			if (SearingPainTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_NM_SEARING_PAIN);
					SearingPainTimer = urand(5000, 6000);
				}
			}
			else SearingPainTimer -= diff;

			if (ImpendingBetrayalTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCastVictim(SPELL_IMPENDING_BETRAYAL);
					ImpendingBetrayalTimer = urand(7000, 8000);
				}
			}
			else ImpendingBetrayalTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_90))
			{
				if (!me->HasAuraEffect(SPELL_NIGHT_MISTRESS_TRANSFORM, 0))
					DoCast(me, SPELL_NIGHT_MISTRESS_TRANSFORM);
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_night_mistressAI(creature);
    }
};

class npc_wanton_hostess : public CreatureScript
{
public:
	npc_wanton_hostess() : CreatureScript("npc_wanton_hostess") {}

    struct npc_wanton_hostessAI : public QuantumBasicAI
    {
        npc_wanton_hostessAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 BansheeWailTimer;
		uint32 BansheeShriekTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			BansheeWailTimer = 1000;
			BansheeShriekTimer = 3000;
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_ALURING_AURA, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BansheeWailTimer <= diff)
			{
				DoCastAOE(SPELL_BANSHEE_WAIL);
				BansheeWailTimer = urand(3000, 4000);
			}

			else BansheeWailTimer -= diff;

			if (BansheeShriekTimer <= diff)
			{
				DoCastAOE(SPELL_BANSHEE_SHRIEK);
				BansheeShriekTimer = urand(5000, 6000);
			}
			else BansheeShriekTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_90))
			{
				if (!me->HasAuraEffect(SPELL_WANTON_HOSTESS_TRANSFORM, 0))
				{
					me->RemoveAura(SPELL_ALURING_AURA);

					DoCast(me, SPELL_WANTON_HOSTESS_TRANSFORM, true);
					DoCast(me, SPELL_BEWITCHING_AURA, true);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wanton_hostessAI(creature);
    }
};

class npc_theatre_spotlight : public CreatureScript
{
public:
	npc_theatre_spotlight() : CreatureScript("npc_theatre_spotlight") {}

    struct npc_theatre_spotlightAI : public QuantumBasicAI
    {
        npc_theatre_spotlightAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		void Reset()
        {
			DoCast(me, SPELL_SPOTLIGHT_VISUAL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_theatre_spotlightAI(creature);
    }
};

class npc_trapped_soul : public CreatureScript
{
public:
	npc_trapped_soul() : CreatureScript("npc_trapped_soul") {}

    struct npc_trapped_soulAI : public QuantumBasicAI
    {
        npc_trapped_soulAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ConeOfColdTimer;
		uint32 ElementalArmorTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			ConeOfColdTimer = 2000;
			ElementalArmorTimer = 4000;
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_ELEMENTAL_ARMOR);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ConeOfColdTimer <= diff)
			{
				DoCast(SPELL_CONE_OF_COLD);
				ConeOfColdTimer = urand(3000, 4000);
			}
			else ConeOfColdTimer -= diff;

			if (ElementalArmorTimer <= diff)
			{
				DoCast(me, SPELL_ELEMENTAL_ARMOR);
				ElementalArmorTimer = urand(7000, 8000);
			}
			else ElementalArmorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_trapped_soulAI(creature);
    }
};

class npc_ghastly_haunt : public CreatureScript
{
public:
	npc_ghastly_haunt() : CreatureScript("npc_ghastly_haunt") {}

    struct npc_ghastly_hauntAI : public QuantumBasicAI
    {
        npc_ghastly_hauntAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShadowShockTimer;
		uint32 EtherealCurseTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			ShadowShockTimer = 1000;
			EtherealCurseTimer = 2000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShadowShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_SHOCK);
					ShadowShockTimer = 3000;
				}
			}
			else ShadowShockTimer -= diff;

			if (EtherealCurseTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ETHEREAL_CURSE);
					EtherealCurseTimer = 4000;
				}
			}
			else EtherealCurseTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ghastly_hauntAI(creature);
    }
};

class npc_arcane_watchman : public CreatureScript
{
public:
	npc_arcane_watchman() : CreatureScript("npc_arcane_watchman") {}

    struct npc_arcane_watchmanAI : public QuantumBasicAI
    {
        npc_arcane_watchmanAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CrystalStrikeTimer;
		uint32 OverloadTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			CrystalStrikeTimer = 2000;
			OverloadTimer = 4000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CrystalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_CRYSTAL_STRIKE);
				CrystalStrikeTimer = 3000;
			}
			else CrystalStrikeTimer -= diff;

			if (OverloadTimer <= diff)
			{
				DoCastVictim(SPELL_OVERLOAD);
				OverloadTimer = 5000;
			}
			else OverloadTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_arcane_watchmanAI(creature);
    }
};

class npc_arcane_anomaly : public CreatureScript
{
public:
	npc_arcane_anomaly() : CreatureScript("npc_arcane_anomaly") {}

    struct npc_arcane_anomalyAI : public QuantumBasicAI
    {
        npc_arcane_anomalyAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ArcaneVolleyTimer;
		uint32 BlinkTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			ArcaneVolleyTimer = 1000;
			BlinkTimer = 3000;
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_MANA_SHIELD);
		}

		void JustDied(Unit* /*killer*/)
		{
			DoCast(me, SPELL_LOOSE_MANA, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ArcaneVolleyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ARCANE_VOLLEY);
					ArcaneVolleyTimer = urand(3000, 4000);
				}
			}
			else ArcaneVolleyTimer -= diff;

			if (BlinkTimer <= diff)
			{
				DoCast(me, SPELL_BLINK);
				BlinkTimer = urand(5000, 6000);
			}
			else BlinkTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_arcane_anomalyAI(creature);
    }
};

class npc_arcane_protector : public CreatureScript
{
public:
	npc_arcane_protector() : CreatureScript("npc_arcane_protector") {}

    struct npc_arcane_protectorAI : public QuantumBasicAI
    {
        npc_arcane_protectorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FistOfStoneTimer;
		uint32 ReturnFireTimer;
		uint32 SummonSparkTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			ReturnFireTimer = 2000;
			FistOfStoneTimer = 4000;
			SummonSparkTimer = 6000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ReturnFireTimer <= diff)
			{
				DoCast(me, SPELL_RETURN_FIRE);
				ReturnFireTimer = 4000;
			}
			else ReturnFireTimer -= diff;

			if (FistOfStoneTimer <= diff)
			{
				DoCast(me, SPELL_FIST_OF_STONE, true);
				FistOfStoneTimer = 6000;
			}
			else FistOfStoneTimer -= diff;

			if (SummonSparkTimer <= diff)
			{
				me->SummonCreature(NPC_ASTRAL_SPARK, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1*MINUTE*IN_MILLISECONDS);
				me->SummonCreature(NPC_ASTRAL_SPARK, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1*MINUTE*IN_MILLISECONDS);
				SummonSparkTimer = 8000;
			}
			else SummonSparkTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_arcane_protectorAI(creature);
    }
};

class npc_syphoner : public CreatureScript
{
public:
	npc_syphoner() : CreatureScript("npc_syphoner") {}

    struct npc_syphonerAI : public QuantumBasicAI
    {
        npc_syphonerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
			me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);
		}

		uint32 DrainManaTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			DrainManaTimer = 1000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DrainManaTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_DRAIN_MANA);
						DrainManaTimer = urand(3000, 4000);
					}
				}
			}
			else DrainManaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_syphonerAI(creature);
    }
};

class npc_chaotic_sentience : public CreatureScript
{
public:
	npc_chaotic_sentience() : CreatureScript("npc_chaotic_sentience") {}

    struct npc_chaotic_sentienceAI : public QuantumBasicAI
    {
        npc_chaotic_sentienceAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
			me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);
		}

		uint32 UnstableMagicTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			UnstableMagicTimer = 1000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (UnstableMagicTimer <= diff)
			{
				DoCastAOE(SPELL_UNSTABLE_MAGIC);
				UnstableMagicTimer = 5000;
			}
			else UnstableMagicTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_chaotic_sentienceAI(creature);
    }
};

class npc_mana_feeder : public CreatureScript
{
public:
	npc_mana_feeder() : CreatureScript("npc_mana_feeder") {}

    struct npc_mana_feederAI : public QuantumBasicAI
    {
        npc_mana_feederAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
			me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);
		}

		uint32 AstralBiteTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			AstralBiteTimer = 2000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (AstralBiteTimer <= diff)
			{
				DoCast(me, SPELL_ASTRAL_BITE);
				AstralBiteTimer = 10000;
			}
			else AstralBiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mana_feederAI(creature);
    }
};

class npc_mana_warp : public CreatureScript
{
public:
	npc_mana_warp() : CreatureScript("npc_mana_warp") {}

    struct npc_mana_warpAI : public QuantumBasicAI
    {
        npc_mana_warpAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
			me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);
		}

		uint32 WarpBreathTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			WarpBreathTimer = 1000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WarpBreathTimer <= diff)
			{
				DoCastAOE(SPELL_WARP_BREATH);
				WarpBreathTimer = 4000;
			}
			else WarpBreathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mana_warpAI(creature);
    }
};

class npc_magical_horror : public CreatureScript
{
public:
	npc_magical_horror() : CreatureScript("npc_magical_horror") {}

    struct npc_magical_horrorAI : public QuantumBasicAI
    {
        npc_magical_horrorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
			me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);
		}

		uint32 ArcaneVolleyTimer;
		uint32 PowerDistortionTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			ArcaneVolleyTimer = 1000;
			PowerDistortionTimer = 3000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ArcaneVolleyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MH_ARCANE_VOLLEY);
					ArcaneVolleyTimer = urand(3000, 4000);
				}
			}
			else ArcaneVolleyTimer -= diff;

			if (PowerDistortionTimer <= diff)
			{
				DoCastAOE(SPELL_POWER_DISTORTION);
				PowerDistortionTimer = urand(5000, 6000);
			}
			else PowerDistortionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_magical_horrorAI(creature);
    }
};

class npc_spell_shade : public CreatureScript
{
public:
	npc_spell_shade() : CreatureScript("npc_spell_shade") {}

    struct npc_spell_shadeAI : public QuantumBasicAI
    {
        npc_spell_shadeAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FireballTimer;
		uint32 FrostboltTimer;
		uint32 ShadowBoltTimer;
		uint32 PhasingInvisibilityTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			FireballTimer = 1000;
			FrostboltTimer = 2000;
			ShadowBoltTimer = 3000;
			PhasingInvisibilityTimer = 100;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
			if (PhasingInvisibilityTimer <= diff)
			{
				DoCast(me, SPELL_PHASING_INVISIBILITY);
				PhasingInvisibilityTimer = 120000;
			}
			else PhasingInvisibilityTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FIREBALL);
					FireballTimer = urand(2000, 3000);
				}
			}
			else FireballTimer -= diff;

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FROSTBOLT);
					FrostboltTimer = urand(4000, 5000);
				}
			}
			else FrostboltTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SH_SHADOW_BOLT);
					ShadowBoltTimer = urand(6000, 7000);
				}
			}
			else ShadowBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spell_shadeAI(creature);
    }
};

class npc_homunculus : public CreatureScript
{
public:
	npc_homunculus() : CreatureScript("npc_homunculus") {}

    struct npc_homunculusAI : public QuantumBasicAI
    {
        npc_homunculusAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FireboltTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			FireboltTimer = 1000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FIREBOLT);
					FireboltTimer = 3000;
				}
			}
			else FireboltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_homunculusAI(creature);
    }
};

class npc_shadow_pillager : public CreatureScript
{
public:
	npc_shadow_pillager() : CreatureScript("npc_shadow_pillager") {}

    struct npc_shadow_pillagerAI : public QuantumBasicAI
    {
        npc_shadow_pillagerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CurseOfAgonyTimer;
		uint32 ImmolateTimer;
		uint32 SearingPainTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			CurseOfAgonyTimer = 1000;
			ImmolateTimer = 2000;
			SearingPainTimer = 4000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CurseOfAgonyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CURSE_OF_AGONY);
					CurseOfAgonyTimer = urand(4000, 5000);
				}
			}
			else CurseOfAgonyTimer -= diff;

			if (ImmolateTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_IMMOLATE);
					ImmolateTimer = urand(6000, 7000);
				}
			}
			else ImmolateTimer -= diff;

			if (SearingPainTimer <= diff)
			{
				DoCastVictim(SPELL_SEARING_PAIN);
				SearingPainTimer = urand(8000, 9000);
			}
			else SearingPainTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadow_pillagerAI(creature);
    }
};

class npc_ethereal_thief : public CreatureScript
{
public:
	npc_ethereal_thief() : CreatureScript("npc_ethereal_thief") {}

    struct npc_ethereal_thiefAI : public QuantumBasicAI
    {
        npc_ethereal_thiefAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CleaveTimer;
		uint32 DisarmTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			CleaveTimer = 2000;
			DisarmTimer = 4000;
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
				DoCastVictim(SPELL_ET_CLEAVE);
				CleaveTimer = 4000;
			}
			else CleaveTimer -= diff;

			if (DisarmTimer <= diff)
			{
				DoCastVictim(SPELL_DISARM);
				DisarmTimer = 8000;
			}
			else DisarmTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ethereal_thiefAI(creature);
    }
};

class npc_ethereal_spellfilcher : public CreatureScript
{
public:
	npc_ethereal_spellfilcher() : CreatureScript("npc_ethereal_spellfilcher") {}

    struct npc_ethereal_spellfilcherAI : public QuantumBasicAI
    {
        npc_ethereal_spellfilcherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ArcaneVolleyTimer;
		uint32 TransefenceTimer;
		uint32 StealMagicTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			ArcaneVolleyTimer = 1000;
			TransefenceTimer = 3000;
			StealMagicTimer = 5000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ArcaneVolleyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ES_ARCANE_VOLLEY);
					ArcaneVolleyTimer = urand(3000, 4000);
				}
			}
			else ArcaneVolleyTimer -= diff;

			if (TransefenceTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_TRANSFERENCE);
					TransefenceTimer = urand(6000, 7000);
				}
			}
			else TransefenceTimer -= diff;

			if (StealMagicTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_STEAL_MAGIC);
					StealMagicTimer = urand(8000, 9000);
				}
			}
			else StealMagicTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ethereal_spellfilcherAI(creature);
    }
};

class npc_spatial_distortion_melee : public CreatureScript
{
public:
	npc_spatial_distortion_melee() : CreatureScript("npc_spatial_distortion_melee") {}

    struct npc_spatial_distortion_meleeAI : public QuantumBasicAI
    {
        npc_spatial_distortion_meleeAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		void Reset()
        {
			DoCast(me, SPELL_SPATIAL_DISTORTION_MELEE);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spatial_distortion_meleeAI(creature);
    }
};

class npc_spatial_distortion_caster : public CreatureScript
{
public:
	npc_spatial_distortion_caster() : CreatureScript("npc_spatial_distortion_caster") {}

    struct npc_spatial_distortion_casterAI : public QuantumBasicAI
    {
        npc_spatial_distortion_casterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		void Reset()
        {
			DoCast(me, SPELL_SPATIAL_DISTORTION_CASTER);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spatial_distortion_casterAI(creature);
    }
};

class npc_fleshbeast : public CreatureScript
{
public:
	npc_fleshbeast() : CreatureScript("npc_fleshbeast") {}

    struct npc_fleshbeastAI : public QuantumBasicAI
    {
        npc_fleshbeastAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 GapingMawTimer;
		uint32 InfectiousPoisonTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			GapingMawTimer = 1000;
			InfectiousPoisonTimer = 3000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GapingMawTimer <= diff)
			{
				DoCastVictim(SPELL_GAPING_MAW);
				GapingMawTimer = urand(3000, 4000);
			}
			else GapingMawTimer -= diff;

			if (InfectiousPoisonTimer <= diff)
			{
				DoCastAOE(SPELL_INFECTIOUS_POISON);
				InfectiousPoisonTimer = urand(5000, 6000);
			}
			else InfectiousPoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fleshbeastAI(creature);
    }
};

class npc_greater_fleshbeast : public CreatureScript
{
public:
	npc_greater_fleshbeast() : CreatureScript("npc_greater_fleshbeast") {}

    struct npc_greater_fleshbeastAI : public QuantumBasicAI
    {
        npc_greater_fleshbeastAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 GapingMawTimer;
		uint32 InfectiousPoisonTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			GapingMawTimer = 1000;
			InfectiousPoisonTimer = 3000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GapingMawTimer <= diff)
			{
				DoCastVictim(SPELL_GAPING_MAW);
				GapingMawTimer = urand(3000, 4000);
			}
			else GapingMawTimer -= diff;

			if (InfectiousPoisonTimer <= diff)
			{
				DoCastAOE(SPELL_INFECTIOUS_POISON);
				InfectiousPoisonTimer = urand(5000, 6000);
			}
			else InfectiousPoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_greater_fleshbeastAI(creature);
    }
};

class npc_coldmist_stalker : public CreatureScript
{
public:
	npc_coldmist_stalker() : CreatureScript("npc_coldmist_stalker") {}

    struct npc_coldmist_stalkerAI : public QuantumBasicAI
    {
        npc_coldmist_stalkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ChillingPoisonTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			ChillingPoisonTimer = 2000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ChillingPoisonTimer <= diff)
			{
				DoCastVictim(SPELL_CHILLING_POISON);
				ChillingPoisonTimer = urand(3000, 4000);
			}
			else ChillingPoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coldmist_stalkerAI(creature);
    }
};

class npc_coldmist_widow : public CreatureScript
{
public:
	npc_coldmist_widow() : CreatureScript("npc_coldmist_widow") {}

    struct npc_coldmist_widowAI : public QuantumBasicAI
    {
        npc_coldmist_widowAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 PoisonBoltVolleyTimer;
		uint32 FrostMistTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			PoisonBoltVolleyTimer = 1000;
			FrostMistTimer = 3000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PoisonBoltVolleyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_POISON_BOLT_VOLLEY);
					PoisonBoltVolleyTimer = urand(3000, 4000);
				}
			}
			else PoisonBoltVolleyTimer -= diff;

			if (FrostMistTimer <= diff)
			{
				DoCastAOE(SPELL_FROST_MIST);
				FrostMistTimer = urand(5000, 6000);
			}
			else FrostMistTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coldmist_widowAI(creature);
    }
};

class npc_shadowbat : public CreatureScript
{
public:
	npc_shadowbat() : CreatureScript("npc_shadowbat") {}

    struct npc_shadowbatAI : public QuantumBasicAI
    {
        npc_shadowbatAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 DarkShriekTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			DarkShriekTimer = 2000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DarkShriekTimer <= diff)
			{
				DoCastAOE(SPELL_DARK_SHRIEK);
				DarkShriekTimer = urand(3000, 4000);
			}
			else DarkShriekTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadowbatAI(creature);
    }
};

class npc_greater_shadowbat : public CreatureScript
{
public:
	npc_greater_shadowbat() : CreatureScript("npc_greater_shadowbat") {}

    struct npc_greater_shadowbatAI : public QuantumBasicAI
    {
        npc_greater_shadowbatAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 SonicBlastTimer;
		uint32 WingBeatTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			SonicBlastTimer = 1000;
			WingBeatTimer = 2000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SonicBlastTimer <= diff)
			{
				DoCastAOE(SPELL_SONIC_BLAST);
				SonicBlastTimer = urand(3000, 4000);
			}
			else SonicBlastTimer -= diff;

			if (WingBeatTimer <= diff)
			{
				DoCastVictim(SPELL_WING_BEAT);
				WingBeatTimer = urand(5000, 6000);
			}
			else WingBeatTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_greater_shadowbatAI(creature);
    }
};

class npc_vampiric_shadowbat : public CreatureScript
{
public:
	npc_vampiric_shadowbat() : CreatureScript("npc_vampiric_shadowbat") {}

    struct npc_vampiric_shadowbatAI : public QuantumBasicAI
    {
        npc_vampiric_shadowbatAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_DRAINING_TOUCH);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vampiric_shadowbatAI(creature);
    }
};

class npc_shadowbeast : public CreatureScript
{
public:
	npc_shadowbeast() : CreatureScript("npc_shadowbeast") {}

    struct npc_shadowbeastAI : public QuantumBasicAI
    {
        npc_shadowbeastAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 HowlOfTheBrokenHillsTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			HowlOfTheBrokenHillsTimer = 1000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HowlOfTheBrokenHillsTimer <= diff)
			{
				DoCastVictim(SPELL_HOWL_OF_THE_BROKEN_HILLS);
				HowlOfTheBrokenHillsTimer = 4000;
			}
			else HowlOfTheBrokenHillsTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadowbeastAI(creature);
    }
};

class npc_dreadbeast : public CreatureScript
{
public:
	npc_dreadbeast() : CreatureScript("npc_dreadbeast") {}

    struct npc_dreadbeastAI : public QuantumBasicAI
    {
        npc_dreadbeastAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CleaveTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			CleaveTimer = 2000;
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
				DoCastVictim(SPELL_DB_CLEAVE);
				CleaveTimer = 4000;
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dreadbeastAI(creature);
    }
};

class npc_phase_hound : public CreatureScript
{
public:
	npc_phase_hound() : CreatureScript("npc_phase_hound") {}

    struct npc_phase_houndAI : public QuantumBasicAI
    {
        npc_phase_houndAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 PhaseShiftTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			PhaseShiftTimer = 1000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PhaseShiftTimer <= diff)
			{
				DoCast(me, SPELL_PHASE_SHIFT);
				PhaseShiftTimer = 4000;
			}
			else PhaseShiftTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_phase_houndAI(creature);
    }
};

class npc_astral_spark : public CreatureScript
{
public:
	npc_astral_spark() : CreatureScript("npc_astral_spark") {}

    struct npc_astral_sparkAI : public QuantumBasicAI
    {
        npc_astral_sparkAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 SearTimer;
		uint32 ArcaneDischargeTimer;
		uint32 DetonateTimer;
		uint32 DespawnTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			SearTimer = 1000;
			ArcaneDischargeTimer = 2000;
			DetonateTimer = 15000;
			DespawnTimer = 23000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SearTimer <= diff)
			{
				DoCastAOE(SPELL_SEAR);
				SearTimer = 1500;
			}
			else SearTimer -= diff;

			if (ArcaneDischargeTimer <= diff)
			{
				DoCast(me, SPELL_ARCANE_DISCHARGE, true);
				ArcaneDischargeTimer = 3000;
			}
			else ArcaneDischargeTimer -= diff;

			if (DetonateTimer <= diff)
			{
				DoCastAOE(SPELL_DETONATE);
				DetonateTimer = 30000;
			}
			else DetonateTimer -= diff;

			if (DespawnTimer <= diff)
			{
				me->Kill(me);
				DespawnTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else DespawnTimer -= diff;

			DoMeleeAttackIfReady();
        }

		Unit* GetRandomTarget()
		{
			return SelectTarget(TARGET_RANDOM);
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_astral_sparkAI(creature);
    }
};

class npc_astral_flare : public CreatureScript
{
public:
	npc_astral_flare() : CreatureScript("npc_astral_flare") {}

    struct npc_astral_flareAI : public QuantumBasicAI
    {
        npc_astral_flareAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ArcingSearTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			ArcingSearTimer = 1000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ArcingSearTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ARCING_SEAR);
					ArcingSearTimer = 2000;
				}
			}
			else ArcingSearTimer -= diff;

			DoMeleeAttackIfReady();
        }

		Unit* GetRandomTarget()
		{
			return SelectTarget(TARGET_RANDOM);
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_astral_flareAI(creature);
    }
};

void AddSC_karazhan_trash()
{
	new npc_spectral_stallion();
	new npc_spectral_charger();
	new npc_spectral_stable_hand();
	new npc_spectral_apprentice();
	new npc_spectral_sentry();
	new npc_spectral_servant();
	new npc_spectral_chef();
	new npc_spectral_retainer();
	new npc_spectral_performer();
	new npc_spectral_patron();
	new npc_ghostly_baker();
	new npc_ghostly_steward();
	new npc_ghostly_philanthropist();
	new npc_skeletal_waiter();
	new npc_skeletal_usher();
	new npc_phantom_guest();
	new npc_phantom_valet();
	new npc_phantom_guardsman();
	new npc_phantom_hound();
	new npc_phantom_stagehand();
	new npc_sorcerous_shade();
	new npc_concubine();
	new npc_night_mistress();
	new npc_wanton_hostess();
	new npc_theatre_spotlight();
	new npc_trapped_soul();
	new npc_ghastly_haunt();
	new npc_arcane_watchman();
	new npc_arcane_anomaly();
	new npc_arcane_protector();
	new npc_syphoner();
	new npc_chaotic_sentience();
	new npc_mana_feeder();
	new npc_mana_warp();
	new npc_magical_horror();
	new npc_spell_shade();
	new npc_homunculus();
	new npc_shadow_pillager();
	new npc_ethereal_thief();
	new npc_ethereal_spellfilcher();
	new npc_spatial_distortion_melee();
	new npc_spatial_distortion_caster();
	new npc_fleshbeast();
	new npc_greater_fleshbeast();
	new npc_coldmist_stalker();
	new npc_coldmist_widow();
	new npc_shadowbat();
	new npc_greater_shadowbat();
	new npc_vampiric_shadowbat();
	new npc_shadowbeast();
	new npc_dreadbeast();
	new npc_phase_hound();
	new npc_astral_spark();
	new npc_astral_flare();
}

/*
How dare you disturb the master's dinner!
How dare you spill the master's wine!
Impudent outsiders!
Meddling fools! You will pay with your lives!
That was a 200 year old Pupellyverbos Port!
That was wasted on the likes of you!
Wait--no! No!
You'll never make it out alive...
*/