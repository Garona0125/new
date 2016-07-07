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
#include "../../../scripts/Outland/TempestKeep/Arcatraz/arcatraz.h"

enum ArcatrazSpells
{
	SPELL_DUAL_WIELD               = 674,
	SPELL_IMMOLATE_5N              = 36638,
	SPELL_IMMOLATE_5H              = 38806,
	SPELL_INFECTED_BLOOD           = 38812,
	SPELL_SHOOT_5N                 = 15620,
	SPELL_SHOOT_5H                 = 22907,
	SPELL_ARCANE_SHOT_5N           = 36609,
	SPELL_ARCANE_SHOT_5H           = 38807,
	SPELL_CHARGED_SHOT_5N          = 36608,
	SPELL_CHARGED_SHOT_5H          = 38808,
	SPELL_WING_CLIP                = 35963,
	SPELL_BLUE_RADIATION           = 52574,
	SPELL_TOOTHY_BITE              = 36612,
	SPELL_INCUBATION               = 36622,
	SPELL_GAPING_MAW_5N            = 36617,
	SPELL_GAPING_MAW_5H            = 38810,
	SPELL_INFECTIOUS_POISON_5N     = 36619,
	SPELL_INFECTIOUS_POISON_5H     = 38811,
	SPELL_ACIDIC_BITE_5N           = 36796,
	SPELL_ACIDIC_BITE_5H           = 64638,
	SPELL_FEIGN_DEATH              = 29266,
	SPELL_CORPSE_BURST             = 36593,
	SPELL_TENTACLE_CLEAVE_5N       = 36664,
	SPELL_TENTACLE_CLEAVE_5H       = 38816,
	SPELL_DRAIN_LIFE_5N            = 36655,
	SPELL_DRAIN_LIFE_5H            = 38817,
	SPELL_DEATH_COUNT_5N           = 36657,
	SPELL_DEATH_COUNT_5H           = 38818,
	SPELL_CHAOS_BREATH             = 36677,
	SPELL_FEL_BREATH_5N            = 36654,
	SPELL_FEL_BREATH_5H            = 38813,
	SPELL_SIGHTLESS_EYE            = 36644,
	SPELL_ENRAGE                   = 33958,
	SPELL_SIGHTLESS_TOUCH_5N       = 36646,
	SPELL_SIGHTLESS_TOUCH_5H       = 38815,
	SPELL_ENERGY_DISCHARGE_5N      = 36716,
	SPELL_ENERGY_DISCHARGE_5H      = 38828,
	SPELL_EXPLODE_5N               = 36719,
	SPELL_EXPLODE_5H               = 38830,
	SPELL_SHADOW_SPAWN             = 46223,
	SPELL_SUMMON_NEGATON_FIELD     = 36813,
	SPELL_NEGATON_FIELD_5N         = 36728,
	SPELL_NEGATON_FIELD_5H         = 38833,
	SPELL_PURPLE_BEAM              = 39123,
	SPELL_PURPLE_ORB_VISUAL        = 57887,
	SPELL_PSYCHIC_SCREAM           = 13704,
	SPELL_REDUCTION_ARCANE         = 34331,
	SPELL_REDUCTION_FIRE           = 34333,
	SPELL_REDUCTION_FROST          = 34334,
	SPELL_REDUCTION_NATURE         = 34335,
	SPELL_REDUCTION_HOLY           = 34336,
	SPELL_REDUCTION_SHADOW         = 34338,
	SPELL_ARCANE_VOLLEY_5N         = 36738,
	SPELL_ARCANE_VOLLEY_5H         = 38835,
	SPELL_FIREBALL_VOLLEY_5N       = 36742,
	SPELL_FIREBALL_VOLLEY_5H       = 38836,
	SPELL_FROSTBOLT_VOLLEY_5N      = 36741,
	SPELL_FROSTBOLT_VOLLEY_5H      = 38837,
	SPELL_HOLY_BOLT_VOLLEY_5N      = 36743,
	SPELL_HOLY_BOLT_VOLLEY_5H      = 38838,
	SPELL_SHADOW_BOLT_VOLLEY_5N    = 36736,
	SPELL_SHADOW_BOLT_VOLLEY_5H    = 38840,
	SPELL_LIGHTNING_BOLT_VOLLEY_5N = 36740,
	SPELL_LIGHTNING_BOLT_VOLLEY_5H = 38839,
	SPELL_UNHOLY_AURA_5N           = 27987,
	SPELL_UNHOLY_AURA_5H           = 38844,
	SPELL_FORCEFUL_CLEAVE_5N       = 36787,
	SPELL_FORCEFUL_CLEAVE_5H       = 38846,
	SPELL_DIMINISH_SOUL_5N         = 36789,
	SPELL_DIMINISH_SOUL_5H         = 38848,
	SPELL_ENTROPIC_AURA            = 36784,
	SPELL_SOUL_STEAL_VICTIM        = 36778,
	SPELL_SOUL_STEAL_SELF          = 36782,
	SPELL_SOUL_CHILL_5N            = 36786,
	SPELL_SOUL_CHILL_5H            = 38843,
	SPELL_GREATER_INVISIBILITY     = 42347,
	SPELL_GOUGE                    = 36862,
	SPELL_LASH_OF_PAIN_5N          = 36864,
	SPELL_LASH_OF_PAIN_5H          = 38852,
	SPELL_CHASTISE_5N              = 36863,
	SPELL_CHASTISE_5H              = 38851,
	SPELL_DEVASTATE_5N             = 36891,
	SPELL_DEVASTATE_5H             = 38849,
	SPELL_DEAFERING_ROAR_5N        = 36887,
	SPELL_DEAFERING_ROAR_5H        = 38850,
	SPELL_SPITEFUL_FURY            = 36886,
	SPELL_DOMINATION               = 36866,
	SPELL_SHADOW_BOLT_5N           = 36868,
	SPELL_SHADOW_BOLT_5H           = 38892,
	SPELL_EVASION                  = 15087,
	SPELL_IMPAIRING_POISON         = 36839,
	SPELL_SLAYING_STRKE_5N         = 36838,
	SPELL_SLAYING_STRKE_5H         = 38894,
	SPELL_ARCANE_HASTE             = 32693,
	SPELL_SONIC_BOOM_5N            = 36841,
	SPELL_SONIC_BOOM_5H            = 38897,
	SPELL_POLYMORPH_5N             = 36840,
	SPELL_POLYMORPH_5H             = 38896,
	SPELL_SHADOW_WORD_PAIN_5N      = 15654,
	SPELL_SHADOW_WORD_PAIN_5H      = 34941,
	SPELL_BIND_5N                  = 37480,
	SPELL_BIND_5H                  = 38900,
	SPELL_SHADOW_MEND_5N           = 37479,
	SPELL_SHADOW_MEND_5H           = 38899,
	SPELL_FIRE_SHIELD_5N           = 38855,
	SPELL_FIRE_SHIELD_5H           = 38901,
	SPELL_METEOR_5N                = 36837,
	SPELL_METEOR_5H                = 38903,
	SPELL_BERSERKER_CHARGE_5N      = 36833,
	SPELL_BERSERKER_CHARGE_5H      = 38907,
	SPELL_AGONIZING_ARMOR          = 36836,
	SPELL_WAR_STOMP_5N             = 36835,
	SPELL_WAR_STOMP_5H             = 38911,
	SPELL_CURSE_OF_THE_ELEMENTS    = 36831,
	SPELL_INCINERATE_5N            = 36832,
	SPELL_INCINERATE_5H            = 38918,
	SPELL_HELL_RAIN_5N             = 36829,
	SPELL_HELL_RAIN_5H             = 38917,
	SPELL_SA_SHOOT_5N              = 22907,
	SPELL_SA_SHOOT_5H              = 38940,
	SPELL_HOOKED_NET_5N            = 36827,
	SPELL_HOOKED_NET_5H            = 38912,
	SPELL_FROST_ARROW_5N           = 35964,
	SPELL_FROST_ARROW_5H           = 38942,
	SPELL_IMMOLATION_ARROW_5N      = 35932,
	SPELL_IMMOLATION_ARROW_5H      = 38943,
	SPELL_SCATTER_SHOT             = 23601,
	SPELL_SERPENT_STING_5N         = 36984,
	SPELL_SERPENT_STING_5H         = 38914,
	SPELL_RAPID_FIRE               = 36828,
};

enum NegatonTexts
{
	SAY_NEGATON_FIRE_ENERGY   = -1000403,
	SAY_NEGATON_ARCANE_ENERGY = -1000404,
	SAY_NEGATON_FROST_ENERGY  = -1000405,
	SAY_NEGATON_HOLY_ENERGY   = -1000406,
	SAY_NEGATON_NATURE_ENERGY = -1000407,
	SAY_NEGATON_SHADOW_ENERGY = -1000408,
};

class npc_arcatraz_defender : public CreatureScript
{
public:
    npc_arcatraz_defender() : CreatureScript("npc_arcatraz_defender") {}

    struct npc_arcatraz_defenderAI : public QuantumBasicAI
    {
        npc_arcatraz_defenderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ImmolateTimer;
		uint32 InfectedBloodTimer;

        void Reset()
		{
			ImmolateTimer = 0.5*IN_MILLISECONDS;
			InfectedBloodTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ImmolateTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_IMMOLATE_5N, SPELL_IMMOLATE_5H));
					ImmolateTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ImmolateTimer -= diff;

			if (InfectedBloodTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					target->CastSpell(target, SPELL_INFECTED_BLOOD, true);
					InfectedBloodTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else InfectedBloodTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_arcatraz_defenderAI(creature);
    }
};

class npc_arcatraz_warder : public CreatureScript
{
public:
    npc_arcatraz_warder() : CreatureScript("npc_arcatraz_warder") {}

    struct npc_arcatraz_warderAI : public QuantumBasicAI
    {
        npc_arcatraz_warderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShootTimer;
		uint32 ArcaneShotTimer;
		uint32 ChargedArcaneShotTimer;
		uint32 WingClipTimer;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;
			ArcaneShotTimer = 1*IN_MILLISECONDS;
			ChargedArcaneShotTimer = 2*IN_MILLISECONDS;
			WingClipTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_RANGED_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_BOW);
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
					DoCast(target, DUNGEON_MODE(SPELL_SHOOT_5N, SPELL_SHOOT_5H));
					ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
				}
			}
			else ShootTimer -= diff;

			if (ArcaneShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_ARCANE_SHOT_5N, SPELL_ARCANE_SHOT_5H));
					ArcaneShotTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else ArcaneShotTimer -= diff;

			if (ChargedArcaneShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_CHARGED_SHOT_5N, SPELL_CHARGED_SHOT_5H));
					ChargedArcaneShotTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else ChargedArcaneShotTimer -= diff;

			if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    DoStartMovement(me->GetVictim());

				if (WingClipTimer <= diff)
				{
					DoCastVictim(SPELL_WING_CLIP);
					WingClipTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
				}
				else WingClipTimer -= diff;

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
						DoCast(target, DUNGEON_MODE(SPELL_SHOOT_5N, SPELL_SHOOT_5H));
						ShootTimer = urand(10*IN_MILLISECONDS, 11*IN_MILLISECONDS);
					}
				}
				else ShootTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_arcatraz_warderAI(creature);
    }
};

class npc_arcatraz_sentinel : public CreatureScript
{
public:
    npc_arcatraz_sentinel() : CreatureScript("npc_arcatraz_sentinel") {}

    struct npc_arcatraz_sentinelAI : public QuantumBasicAI
    {
        npc_arcatraz_sentinelAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		bool Exploded;

        void Reset()
		{
			Exploded = true;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetReactState(REACT_AGGRESSIVE);
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, DUNGEON_MODE(SPELL_ENERGY_DISCHARGE_5N, SPELL_ENERGY_DISCHARGE_5H));
		}

		void JustDied(Unit* /*killer*/)
		{
			me->SummonCreature(NPC_DESTROYED_SENTINEL, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 30*IN_MILLISECONDS);
		}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			if (me->HealthBelowPct(HEALTH_PERCENT_10))
			{
				if (Exploded)
				{
					me->RemoveAllAuras();
					me->RemoveAllAttackers();
					me->AttackStop();
					me->SetReactState(REACT_PASSIVE);
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
					DoCast(me, DUNGEON_MODE(SPELL_EXPLODE_5N, SPELL_EXPLODE_5H), true);
					Exploded = false;
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_arcatraz_sentinelAI(creature);
    }
};

class npc_protean_horror : public CreatureScript
{
public:
    npc_protean_horror() : CreatureScript("npc_protean_horror") {}

    struct npc_protean_horrorAI : public QuantumBasicAI
    {
        npc_protean_horrorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ToothyBiteTimer;

        void Reset()
		{
			ToothyBiteTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_BLUE_RADIATION, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ToothyBiteTimer <= diff)
			{
				DoCastVictim(SPELL_TOOTHY_BITE);
				ToothyBiteTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else ToothyBiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_protean_horrorAI(creature);
    }
};

class npc_protean_nightmare : public CreatureScript
{
public:
    npc_protean_nightmare() : CreatureScript("npc_protean_nightmare") {}

    struct npc_protean_nightmareAI : public QuantumBasicAI
    {
        npc_protean_nightmareAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 IncubationTimer;
		uint32 GapingMawTimer;
		uint32 InfectiousPoisonTimer;

        void Reset()
		{
			IncubationTimer = 0.5*IN_MILLISECONDS;
			GapingMawTimer = 2*IN_MILLISECONDS;
			InfectiousPoisonTimer = 3*IN_MILLISECONDS;

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

			if (IncubationTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_INCUBATION, true);
					IncubationTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else IncubationTimer -= diff;

			if (GapingMawTimer <= diff)
			{
				DoCast(DUNGEON_MODE(SPELL_GAPING_MAW_5N, SPELL_GAPING_MAW_5H));
				GapingMawTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else GapingMawTimer -= diff;

			if (InfectiousPoisonTimer <= diff)
			{
				DoCast(DUNGEON_MODE(SPELL_INFECTIOUS_POISON_5N, SPELL_INFECTIOUS_POISON_5H));
				InfectiousPoisonTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else InfectiousPoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_protean_nightmareAI(creature);
    }
};

class npc_protean_spawn : public CreatureScript
{
public:
    npc_protean_spawn() : CreatureScript("npc_protean_spawn") {}

    struct npc_protean_spawnAI : public QuantumBasicAI
    {
        npc_protean_spawnAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 AcidicBiteTimer;

        void Reset()
		{
			AcidicBiteTimer = 0.5*IN_MILLISECONDS;

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

			if (AcidicBiteTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_ACIDIC_BITE_5N, SPELL_ACIDIC_BITE_5H));
				AcidicBiteTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else AcidicBiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_protean_spawnAI(creature);
    }
};

class npc_warder_corpse : public CreatureScript
{
public:
    npc_warder_corpse() : CreatureScript("npc_warder_corpse") {}

    struct npc_warder_corpseAI : public QuantumBasicAI
    {
        npc_warder_corpseAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			DoCast(me, SPELL_FEIGN_DEATH);
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_warder_corpseAI(creature);
    }
};

class npc_defender_corpse : public CreatureScript
{
public:
    npc_defender_corpse() : CreatureScript("npc_defender_corpse") {}

    struct npc_defender_corpseAI : public QuantumBasicAI
    {
		npc_defender_corpseAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			DoCast(me, SPELL_FEIGN_DEATH);
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_defender_corpseAI(creature);
    }
};

class npc_death_watcher : public CreatureScript
{
public:
    npc_death_watcher() : CreatureScript("npc_death_watcher") {}

    struct npc_death_watcherAI : public QuantumBasicAI
    {
        npc_death_watcherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 TentacleCleaveTimer;
		uint32 DrainLifeTimer;
		uint32 DeathCountTimer;

        void Reset()
		{
			TentacleCleaveTimer = 2*IN_MILLISECONDS;
			DrainLifeTimer = 3*IN_MILLISECONDS;
			DeathCountTimer = 6*IN_MILLISECONDS;

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

			if (TentacleCleaveTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_TENTACLE_CLEAVE_5N, SPELL_TENTACLE_CLEAVE_5H));
				TentacleCleaveTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else TentacleCleaveTimer -= diff;

			if (DrainLifeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_DRAIN_LIFE_5N, SPELL_DRAIN_LIFE_5H));
					DrainLifeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else DrainLifeTimer -= diff;

			if (DeathCountTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_DEATH_COUNT_5N, SPELL_DEATH_COUNT_5H));
				DeathCountTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else DeathCountTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_death_watcherAI(creature);
    }
};

class npc_entropic_eye : public CreatureScript
{
public:
    npc_entropic_eye() : CreatureScript("npc_entropic_eye") {}

    struct npc_entropic_eyeAI : public QuantumBasicAI
    {
        npc_entropic_eyeAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 TentacleCleaveTimer;
		uint32 ChaosBreathTimer;

        void Reset()
		{
			TentacleCleaveTimer = 2*IN_MILLISECONDS;
			ChaosBreathTimer = 3*IN_MILLISECONDS;

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

			if (TentacleCleaveTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_TENTACLE_CLEAVE_5N, SPELL_TENTACLE_CLEAVE_5H));
				TentacleCleaveTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else TentacleCleaveTimer -= diff;

			if (ChaosBreathTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CHAOS_BREATH);
					ChaosBreathTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ChaosBreathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_entropic_eyeAI(creature);
    }
};

class npc_soul_devourer : public CreatureScript
{
public:
    npc_soul_devourer() : CreatureScript("npc_soul_devourer") {}

    struct npc_soul_devourerAI : public QuantumBasicAI
    {
        npc_soul_devourerAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		SummonList Summons;

		uint32 EnrageTimer;
		uint32 SightlessEyeTimer;
		uint32 FelBreathTimer;

        void Reset()
		{
			EnrageTimer = 0.5*IN_MILLISECONDS;
			SightlessEyeTimer = 1*IN_MILLISECONDS;
			FelBreathTimer = 2*IN_MILLISECONDS;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_SIGHTLESS_EYE)
			{
				Summons.Summon(summon);
				Summons.DoZoneInCombat();
			}
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (EnrageTimer <= diff)
			{
				DoCast(me, SPELL_ENRAGE);
				DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				EnrageTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else EnrageTimer -= diff;

			if (SightlessEyeTimer <= diff)
			{
				DoCast(me, SPELL_SIGHTLESS_EYE);
				SightlessEyeTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else SightlessEyeTimer -= diff;

			if (FelBreathTimer <= diff)
			{
				DoCast(DUNGEON_MODE(SPELL_FEL_BREATH_5N, SPELL_FEL_BREATH_5H));
				FelBreathTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else FelBreathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_soul_devourerAI(creature);
    }
};

class npc_sightless_eye : public CreatureScript
{
public:
    npc_sightless_eye() : CreatureScript("npc_sightless_eye") {}

    struct npc_sightless_eyeAI : public QuantumBasicAI
    {
        npc_sightless_eyeAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SightlessTouchTimer;

        void Reset()
		{
			SightlessTouchTimer = 0.5*IN_MILLISECONDS;

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

			if (SightlessTouchTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SIGHTLESS_TOUCH_5N, SPELL_SIGHTLESS_TOUCH_5H));
					SightlessTouchTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else SightlessTouchTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sightless_eyeAI(creature);
    }
};

class npc_destroyed_sentinel : public CreatureScript
{
public:
    npc_destroyed_sentinel() : CreatureScript("npc_destroyed_sentinel") {}

    struct npc_destroyed_sentinelAI : public QuantumBasicAI
    {
        npc_destroyed_sentinelAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, DUNGEON_MODE(SPELL_ENERGY_DISCHARGE_5N, SPELL_ENERGY_DISCHARGE_5H));
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetReactState(REACT_PASSIVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

		void EnterToBattle(Unit* /*who*/){}

		void UpdateAI(uint32 const /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_destroyed_sentinelAI(creature);
    }
};

class npc_negaton_warp_master : public CreatureScript
{
public:
    npc_negaton_warp_master() : CreatureScript("npc_negaton_warp_master") {}

    struct npc_negaton_warp_masterAI : public QuantumBasicAI
    {
        npc_negaton_warp_masterAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		SummonList Summons;

		uint32 PurpleBeamTimer;
		uint32 NegatonFieldTimer;

        void Reset()
		{
			PurpleBeamTimer = 0.1*IN_MILLISECONDS;
			NegatonFieldTimer = 0.5*IN_MILLISECONDS;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->InterruptSpell(CURRENT_CHANNELED_SPELL);

			DoCast(me, SPELL_SHADOW_SPAWN, true);
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_NEGATON_FIELD)
			{
				Summons.Summon(summon);
				Summons.DoZoneInCombat();
			}
		}

		void JustReachedHome()
		{
			Reset();
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (PurpleBeamTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* trigger = me->FindCreatureWithDistance(NPC_WORLD_TRIGGER, 20.0f))
				{
					me->SetFacingToObject(trigger);
					DoCast(trigger, SPELL_PURPLE_BEAM, true);
					PurpleBeamTimer = 2*MINUTE*IN_MILLISECONDS;
				}
			}
			else PurpleBeamTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (NegatonFieldTimer <= diff)
			{
				DoCast(me, SPELL_SUMMON_NEGATON_FIELD);
				NegatonFieldTimer = 8*IN_MILLISECONDS;
			}
			else NegatonFieldTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_negaton_warp_masterAI(creature);
    }
};

class npc_negaton_screamer : public CreatureScript
{
public:
    npc_negaton_screamer() : CreatureScript("npc_negaton_screamer") {}

    struct npc_negaton_screamerAI : public QuantumBasicAI
    {
        npc_negaton_screamerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 PurpleBeamTimer;
		uint32 RandomReductionTimer;
		uint32 PsychicScreamTimer;

        void Reset()
		{
			PurpleBeamTimer = 0.1*IN_MILLISECONDS;
			RandomReductionTimer = 0.5*IN_MILLISECONDS;
			PsychicScreamTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->InterruptSpell(CURRENT_CHANNELED_SPELL);

			DoCast(me, SPELL_SHADOW_SPAWN, true);
		}

		void JustReachedHome()
		{
			Reset();
		}

		void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
		{
			switch (spell->Id)
			{
			    case SPELL_REDUCTION_FIRE:
					DoCastAOE(DUNGEON_MODE(SPELL_FIREBALL_VOLLEY_5N, SPELL_FIREBALL_VOLLEY_5H));
					DoSendQuantumText(SAY_NEGATON_FIRE_ENERGY, me);
					break;
				case SPELL_REDUCTION_ARCANE:
					DoCastAOE(DUNGEON_MODE(SPELL_ARCANE_VOLLEY_5N, SPELL_ARCANE_VOLLEY_5H));
					DoSendQuantumText(SAY_NEGATON_ARCANE_ENERGY, me);
					break;
				case SPELL_REDUCTION_FROST:
					DoCastAOE(DUNGEON_MODE(SPELL_FROSTBOLT_VOLLEY_5N, SPELL_FROSTBOLT_VOLLEY_5H));
					DoSendQuantumText(SAY_NEGATON_FROST_ENERGY, me);
					break;
				case SPELL_REDUCTION_HOLY:
					DoCastAOE(DUNGEON_MODE(SPELL_HOLY_BOLT_VOLLEY_5N, SPELL_HOLY_BOLT_VOLLEY_5H));
					DoSendQuantumText(SAY_NEGATON_HOLY_ENERGY, me);
					break;
				case SPELL_REDUCTION_NATURE:
					DoCastAOE(DUNGEON_MODE(SPELL_LIGHTNING_BOLT_VOLLEY_5N, SPELL_LIGHTNING_BOLT_VOLLEY_5H));
					DoSendQuantumText(SAY_NEGATON_NATURE_ENERGY, me);
					break;
				case SPELL_REDUCTION_SHADOW:
					DoCastAOE(DUNGEON_MODE(SPELL_SHADOW_BOLT_VOLLEY_5N, SPELL_SHADOW_BOLT_VOLLEY_5H));
					DoSendQuantumText(SAY_NEGATON_SHADOW_ENERGY, me);
					break;
			}
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (PurpleBeamTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* trigger = me->FindCreatureWithDistance(NPC_WORLD_TRIGGER, 20.0f))
				{
					me->SetFacingToObject(trigger);
					DoCast(trigger, SPELL_PURPLE_BEAM, true);
					trigger->CastSpell(trigger, SPELL_PURPLE_ORB_VISUAL, true);
					PurpleBeamTimer = 2*MINUTE*IN_MILLISECONDS;
				}
			}
			else PurpleBeamTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RandomReductionTimer <= diff)
			{
				DoCast(me, RAND(SPELL_REDUCTION_FIRE, SPELL_REDUCTION_ARCANE, SPELL_REDUCTION_FROST, SPELL_REDUCTION_HOLY, SPELL_REDUCTION_NATURE, SPELL_REDUCTION_SHADOW));
				RandomReductionTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else RandomReductionTimer -= diff;

			if (PsychicScreamTimer <= diff)
			{
				DoCastAOE(SPELL_PSYCHIC_SCREAM);
				PsychicScreamTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else PsychicScreamTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_negaton_screamerAI(creature);
    }
};

class npc_negaton_field : public CreatureScript
{
public:
    npc_negaton_field() : CreatureScript("npc_negaton_field") {}

    struct npc_negaton_fieldAI : public QuantumBasicAI
    {
        npc_negaton_fieldAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        void Reset()
		{
			DoCast(me, DUNGEON_MODE(SPELL_NEGATON_FIELD_5N, SPELL_NEGATON_FIELD_5H));
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetReactState(REACT_PASSIVE);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        }

		void EnterToBattle(Unit* /*who*/){}

		void UpdateAI(uint32 const /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_negaton_fieldAI(creature);
    }
};

class npc_eredar_deathbringer : public CreatureScript
{
public:
    npc_eredar_deathbringer() : CreatureScript("npc_eredar_deathbringer") {}

    struct npc_eredar_deathbringerAI : public QuantumBasicAI
    {
        npc_eredar_deathbringerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ForcefulCleaveTimer;
		uint32 DiminishSoulTimer;

        void Reset()
		{
			ForcefulCleaveTimer = 2*IN_MILLISECONDS;
			DiminishSoulTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, DUNGEON_MODE(SPELL_UNHOLY_AURA_5N, SPELL_UNHOLY_AURA_5H));
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ForcefulCleaveTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_FORCEFUL_CLEAVE_5N, SPELL_FORCEFUL_CLEAVE_5H));
				ForcefulCleaveTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else ForcefulCleaveTimer -= diff;

			if (DiminishSoulTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_DIMINISH_SOUL_5N, SPELL_DIMINISH_SOUL_5H));
				DiminishSoulTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else DiminishSoulTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_eredar_deathbringerAI(creature);
    }
};

class npc_eredar_soul_eater : public CreatureScript
{
public:
    npc_eredar_soul_eater() : CreatureScript("npc_eredar_soul_eater") {}

    struct npc_eredar_soul_eaterAI : public QuantumBasicAI
    {
        npc_eredar_soul_eaterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 PurpleBeamTimer;
		uint32 SoulStealTimer;
		uint32 SoulChillTimer;

        void Reset()
		{
			PurpleBeamTimer = 0.1*IN_MILLISECONDS;
			SoulStealTimer = 0.5*IN_MILLISECONDS;
			SoulChillTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_ENTROPIC_AURA);
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
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
			if (PurpleBeamTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* trigger = me->FindCreatureWithDistance(NPC_WORLD_TRIGGER, 10.0f))
				{
					me->SetFacingToObject(trigger);
					DoCast(trigger, SPELL_PURPLE_BEAM, true);
					PurpleBeamTimer = 2*MINUTE*IN_MILLISECONDS;
				}
			}
			else PurpleBeamTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SoulStealTimer <= diff)
			{
				DoCastVictim(SPELL_SOUL_STEAL_VICTIM, true);
				DoCast(me, SPELL_SOUL_STEAL_SELF, true);
				SoulStealTimer = 18*IN_MILLISECONDS;
			}
			else SoulStealTimer -= diff;

			if (SoulChillTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_SOUL_CHILL_5N, SPELL_SOUL_CHILL_5H));
				SoulChillTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else SoulChillTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_eredar_soul_eaterAI(creature);
    }
};

class npc_skulking_witch : public CreatureScript
{
public:
    npc_skulking_witch() : CreatureScript("npc_skulking_witch") {}

    struct npc_skulking_witchAI : public QuantumBasicAI
    {
        npc_skulking_witchAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 LashOfPainTimer;
		uint32 ChastiseTimer;
		uint32 GougeTimer;

        void Reset()
		{
			LashOfPainTimer = 0.5*IN_MILLISECONDS;
			ChastiseTimer = 2*IN_MILLISECONDS;
			GougeTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_GREATER_INVISIBILITY);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_GREATER_INVISIBILITY);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (LashOfPainTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_LASH_OF_PAIN_5N, SPELL_LASH_OF_PAIN_5H));
				LashOfPainTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else LashOfPainTimer -= diff;

			if (ChastiseTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_CHASTISE_5N, SPELL_CHASTISE_5H));
				ChastiseTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else ChastiseTimer -= diff;

			if (GougeTimer <= diff)
			{
				DoCastAOE(SPELL_GOUGE);
				GougeTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else GougeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skulking_witchAI(creature);
    }
};

class npc_unbound_devastator : public CreatureScript
{
public:
    npc_unbound_devastator() : CreatureScript("npc_unbound_devastator") {}

    struct npc_unbound_devastatorAI : public QuantumBasicAI
    {
        npc_unbound_devastatorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 DevastateTimer;
		uint32 DeaferingRoarTimer;

        void Reset()
		{
			DevastateTimer = 2*IN_MILLISECONDS;
			DeaferingRoarTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DevastateTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_DEVASTATE_5N, SPELL_DEVASTATE_5H));
				DevastateTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else DevastateTimer -= diff;

			if (DeaferingRoarTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_DEAFERING_ROAR_5N, SPELL_DEAFERING_ROAR_5H));
				DeaferingRoarTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else DeaferingRoarTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unbound_devastatorAI(creature);
    }
};

class npc_spiteful_temptress : public CreatureScript
{
public:
    npc_spiteful_temptress() : CreatureScript("npc_spiteful_temptress") {}

    struct npc_spiteful_temptressAI : public QuantumBasicAI
    {
        npc_spiteful_temptressAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShadowBoltTimer;
		uint32 DominationTimer;

        void Reset()
		{
			ShadowBoltTimer = 0.5*IN_MILLISECONDS;
			DominationTimer = 3.5*IN_MILLISECONDS;

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

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SPITEFUL_FURY, true);
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BOLT_5N, SPELL_SHADOW_BOLT_5H));
					ShadowBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			if (DominationTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
					DoCast(target, SPELL_DOMINATION, true);
					DominationTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else DominationTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spiteful_temptressAI(creature);
    }
};

class npc_ethereum_slayer : public CreatureScript
{
public:
    npc_ethereum_slayer() : CreatureScript("npc_ethereum_slayer") {}

    struct npc_ethereum_slayerAI : public QuantumBasicAI
    {
        npc_ethereum_slayerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SlayingStrikeTimer;
		uint32 ImpairingPoisonTimer;
		uint32 EvasionTimer;

        void Reset()
		{
			SlayingStrikeTimer = 1*IN_MILLISECONDS;
			ImpairingPoisonTimer = 2*IN_MILLISECONDS;
			EvasionTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_DUAL_WIELD);
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SlayingStrikeTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_SLAYING_STRKE_5N, SPELL_SLAYING_STRKE_5H));
				SlayingStrikeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else SlayingStrikeTimer -= diff;

			if (ImpairingPoisonTimer <= diff)
			{
				DoCastVictim(SPELL_IMPAIRING_POISON);
				ImpairingPoisonTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else ImpairingPoisonTimer -= diff;

			if (EvasionTimer <= diff)
			{
				DoCastVictim(SPELL_EVASION);
				EvasionTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else EvasionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ethereum_slayerAI(creature);
    }
};

class npc_ethereum_wave_caster : public CreatureScript
{
public:
    npc_ethereum_wave_caster() : CreatureScript("npc_ethereum_wave_caster") {}

    struct npc_ethereum_wave_casterAI : public QuantumBasicAI
    {
        npc_ethereum_wave_casterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SonicBoomTimer;
		uint32 PolymorphTimer;

        void Reset()
		{
			SonicBoomTimer = 0.5*IN_MILLISECONDS;
			PolymorphTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Arcane Haste Aura in Combat
			if (!me->HasAura(SPELL_ARCANE_HASTE))
				DoCast(me, SPELL_ARCANE_HASTE, true);

			if (SonicBoomTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SONIC_BOOM_5N, SPELL_SONIC_BOOM_5H));
					SonicBoomTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else SonicBoomTimer -= diff;

			if (PolymorphTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
					DoCast(target, DUNGEON_MODE(SPELL_POLYMORPH_5N, SPELL_POLYMORPH_5H), true);
					PolymorphTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else PolymorphTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ethereum_wave_casterAI(creature);
    }
};

class npc_ethereum_life_binder : public CreatureScript
{
public:
    npc_ethereum_life_binder() : CreatureScript("npc_ethereum_life_binder") {}

    struct npc_ethereum_life_binderAI : public QuantumBasicAI
    {
        npc_ethereum_life_binderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShadoWordPainTimer;
		uint32 BindTimer;
		uint32 ShadowMendTimer;

        void Reset()
		{
			ShadoWordPainTimer = 0.5*IN_MILLISECONDS;
			BindTimer = 1*IN_MILLISECONDS;
			ShadowMendTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShadoWordPainTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_WORD_PAIN_5N, SPELL_SHADOW_WORD_PAIN_5H));
					ShadoWordPainTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadoWordPainTimer -= diff;

			if (BindTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_BIND_5N, SPELL_BIND_5H));
					BindTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else BindTimer -= diff;

			if (ShadowMendTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_MEND_5N, SPELL_SHADOW_MEND_5H));
					ShadowMendTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else ShadowMendTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ethereum_life_binderAI(creature);
    }
};

class npc_gargantuan_abyssal : public CreatureScript
{
public:
    npc_gargantuan_abyssal() : CreatureScript("npc_gargantuan_abyssal") {}

    struct npc_gargantuan_abyssalAI : public QuantumBasicAI
    {
        npc_gargantuan_abyssalAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 MeteorTimer;

        void Reset()
		{
			MeteorTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->RemoveAurasDueToSpell(DUNGEON_MODE(SPELL_FIRE_SHIELD_5N, SPELL_FIRE_SHIELD_5H));
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, DUNGEON_MODE(SPELL_FIRE_SHIELD_5N, SPELL_FIRE_SHIELD_5H));

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MeteorTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_METEOR_5N, SPELL_METEOR_5H), true);
				MeteorTimer = 6*IN_MILLISECONDS;
			}
			else MeteorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gargantuan_abyssalAI(creature);
    }
};

class npc_unchained_doombringer : public CreatureScript
{
public:
    npc_unchained_doombringer() : CreatureScript("npc_unchained_doombringer") {}

    struct npc_unchained_doombringerAI : public QuantumBasicAI
    {
        npc_unchained_doombringerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 BerserkerChargeTimer;
		uint32 AgonizingArmorTimer;
		uint32 WarStompTimer;

        void Reset()
		{
			BerserkerChargeTimer = 0.2*IN_MILLISECONDS;
			AgonizingArmorTimer = 1*IN_MILLISECONDS;
			WarStompTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BerserkerChargeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_BERSERKER_CHARGE_5N, SPELL_BERSERKER_CHARGE_5H));
					BerserkerChargeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else BerserkerChargeTimer -= diff;

			if (AgonizingArmorTimer <= diff)
			{
				DoCastVictim(SPELL_AGONIZING_ARMOR);
				AgonizingArmorTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else AgonizingArmorTimer -= diff;

			if (WarStompTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_WAR_STOMP_5N, SPELL_WAR_STOMP_5H));
				WarStompTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else WarStompTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unchained_doombringerAI(creature);
    }
};

class npc_sargeron_hellcaller : public CreatureScript
{
public:
    npc_sargeron_hellcaller() : CreatureScript("npc_sargeron_hellcaller") {}

    struct npc_sargeron_hellcallerAI : public QuantumBasicAI
    {
        npc_sargeron_hellcallerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 CurseOfTheElementsTimer;
		uint32 IncinerateTimer;
		uint32 HellRainTimer;

        void Reset()
		{
			CurseOfTheElementsTimer = 0.5*IN_MILLISECONDS;
			IncinerateTimer = 1*IN_MILLISECONDS;
			HellRainTimer = 3*IN_MILLISECONDS;

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

			if (CurseOfTheElementsTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CURSE_OF_THE_ELEMENTS);
					CurseOfTheElementsTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CurseOfTheElementsTimer -= diff;

			if (IncinerateTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_INCINERATE_5N, SPELL_INCINERATE_5H));
					IncinerateTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else IncinerateTimer -= diff;

			if (HellRainTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_HELL_RAIN_5N, SPELL_HELL_RAIN_5H), true);
				HellRainTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else HellRainTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sargeron_hellcallerAI(creature);
    }
};

class npc_sargeron_archer : public CreatureScript
{
public:
    npc_sargeron_archer() : CreatureScript("npc_sargeron_archer") {}

    struct npc_sargeron_archerAI : public QuantumBasicAI
    {
        npc_sargeron_archerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShootTimer;
		uint32 FrostArrowTimer;
		uint32 ImmolationArrowTimer;
		uint32 SerpentStingTimer;
		uint32 ScatterShotTimer;
		uint32 HookedNetTimer;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;
			FrostArrowTimer = 1*IN_MILLISECONDS;
			ImmolationArrowTimer = 2*IN_MILLISECONDS;
			SerpentStingTimer = 4*IN_MILLISECONDS;
			ScatterShotTimer = 5*IN_MILLISECONDS;
			HookedNetTimer = 7*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_RANGED_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			DoCast(who, SPELL_SCATTER_SHOT, true);
			DoCast(me, SPELL_RAPID_FIRE, true);
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
					DoCast(target, DUNGEON_MODE(SPELL_SA_SHOOT_5N, SPELL_SA_SHOOT_5H));
					ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
				}
			}
			else ShootTimer -= diff;

			if (FrostArrowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FROST_ARROW_5N, SPELL_FROST_ARROW_5H), true);
					FrostArrowTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else FrostArrowTimer -= diff;

			if (ImmolationArrowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_IMMOLATION_ARROW_5N, SPELL_IMMOLATION_ARROW_5H), true);
					ImmolationArrowTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else ImmolationArrowTimer -= diff;

			if (SerpentStingTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SERPENT_STING_5N, SPELL_SERPENT_STING_5H));
					SerpentStingTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
				}
			}
			else SerpentStingTimer -= diff;

			if (ScatterShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SCATTER_SHOT);
					ScatterShotTimer = urand(10*IN_MILLISECONDS, 11*IN_MILLISECONDS);
				}
			}
			else ScatterShotTimer -= diff;

			if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    DoStartMovement(me->GetVictim());

				if (HookedNetTimer <= diff)
				{
					DoCastVictim(DUNGEON_MODE(SPELL_HOOKED_NET_5N, SPELL_HOOKED_NET_5H));
					HookedNetTimer = urand(12*IN_MILLISECONDS, 13*IN_MILLISECONDS);
				}
				else HookedNetTimer -= diff;

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
						DoCast(target, DUNGEON_MODE(SPELL_SA_SHOOT_5N, SPELL_SA_SHOOT_5H));
						ShootTimer = urand(14*IN_MILLISECONDS, 15*IN_MILLISECONDS);
					}
				}
				else ShootTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sargeron_archerAI(creature);
    }
};

void AddSC_arcatraz_trash()
{
	new npc_arcatraz_defender();
	new npc_arcatraz_warder();
	new npc_arcatraz_sentinel();
	new npc_protean_horror();
	new npc_protean_nightmare();
	new npc_protean_spawn();
	new npc_warder_corpse();
	new npc_defender_corpse();
	new npc_death_watcher();
	new npc_entropic_eye();
	new npc_soul_devourer();
	new npc_sightless_eye();
	new npc_destroyed_sentinel();
	new npc_negaton_warp_master();
	new npc_negaton_screamer();
	new npc_negaton_field();
	new npc_eredar_deathbringer();
	new npc_eredar_soul_eater();
	new npc_skulking_witch();
	new npc_unbound_devastator();
	new npc_spiteful_temptress();
	new npc_ethereum_slayer();
	new npc_ethereum_wave_caster();
	new npc_ethereum_life_binder();
	new npc_gargantuan_abyssal();
	new npc_unchained_doombringer();
	new npc_sargeron_hellcaller();
	new npc_sargeron_archer();
}