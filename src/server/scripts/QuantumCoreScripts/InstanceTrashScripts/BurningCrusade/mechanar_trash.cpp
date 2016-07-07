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
#include "../../../scripts/Outland/TempestKeep/Mechanar/mechanar.h"

enum MechanarSpells
{
	SPELL_ANESTHETIC                = 36333,
	SPELL_BANDAGE_5N                = 36348,
	SPELL_BANDAGE_5H                = 38919,
	SPELL_HOLY_SHOCK_5N             = 36340,
	SPELL_HOLY_SHOCK_5H             = 38921,
	SPELL_ARCANE_NOVA_5N            = 35261,
	SPELL_ARCANE_NOVA_5H            = 38936,
	SPELL_DISPEL_MAGIC              = 17201,
	SPELL_STARFIRE_5N               = 35243,
	SPELL_STARFIRE_5H               = 38935,
	SPELL_ARCANE_VOLLEY             = 35255,
	SPELL_ARCANE_EXPLOSION          = 35187,
	SPELL_SHIELD_BASH               = 35178,
	SPELL_UNSTABLE_AFFLICTION       = 35184,
	SPELL_MELT_ARMOR_1              = 35186,
	SPELL_CHILLING_TOUCH            = 12531, // Need Implement
	SPELL_KNOCKDOWN                 = 35011,
	SPELL_CHARGED_ARCANE_MISSILE_5N = 35012,
	SPELL_CHARGED_ARCANE_MISSILE_5H = 38941,
	SPELL_MELT_ARMOR_2              = 35192,
	SPELL_MORTAL_STRIKE             = 15708,
	SPELL_SOLAR_STRIKE              = 35189,
	SPELL_WHIRLWIND                 = 13736,
	SPELL_DEATH_RAY_5N              = 36345,
	SPELL_DEATH_RAY_5H              = 39196,
	SPELL_GROWTH_RAY                = 36346,
	SPELL_SUPER_SHRINK_RAY          = 36341,
	SPELL_POUND                     = 35049,
	SPELL_GLOB_OF_MACHINE_FLUID_5N  = 35056,
	SPELL_GLOB_OF_MACHINE_FLUID_5H  = 38923,
	SPELL_NETHER_EXPLOSION          = 35058,
	SPELL_MANIACAL_CHARGE           = 35062,
	SPELL_NETHERBOMB_5N             = 35057,
	SPELL_NETHERBOMB_5H             = 38925,
	SPELL_FIRE_SHIELD_5N            = 35265,
	SPELL_FIRE_SHIELD_5H            = 38933,
	SPELL_SCORCH_5N                 = 17195,
	SPELL_SCORCH_5H                 = 36807,
	SPELL_SOLARBURN_5N              = 35267,
	SPELL_SOLARBURN_5H              = 38930,
	SPELL_TFD_KNOCKDOWN             = 35783,
	SPELL_CHARGED_FIST              = 36582,
	SPELL_DRILL_ARMOR               = 35047,
	SPELL_THE_CLAW                  = 35055,
};

class npc_bloodwarder_physician : public CreatureScript
{
public:
	npc_bloodwarder_physician() : CreatureScript("npc_bloodwarder_physician") {}

    struct npc_bloodwarder_physicianAI : public QuantumBasicAI
    {
        npc_bloodwarder_physicianAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 AnestheticTimer;
		uint32 HolyShockTimer;
		uint32 BandageTimer;

		void Reset()
        {
			AnestheticTimer = 500;
			HolyShockTimer = 2000;
			BandageTimer = 3000;

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

			if (AnestheticTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ANESTHETIC);
					AnestheticTimer = urand(3000, 4000);
				}
			}
			else AnestheticTimer -= diff;

			if (HolyShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_HOLY_SHOCK_5N, SPELL_HOLY_SHOCK_5H));
					HolyShockTimer = urand(5000, 6000);
				}
			}
			else HolyShockTimer -= diff;

			if (BandageTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, DUNGEON_MODE(SPELL_BANDAGE_5N, SPELL_BANDAGE_5H));
					BandageTimer = urand(7000, 8000);
				}
			}
			else BandageTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodwarder_physicianAI(creature);
    }
};

class npc_bloodwarder_centurion : public CreatureScript
{
public:
	npc_bloodwarder_centurion() : CreatureScript("npc_bloodwarder_centurion") {}

    struct npc_bloodwarder_centurionAI : public QuantumBasicAI
    {
        npc_bloodwarder_centurionAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShieldBashTimer;
		uint32 MeltArmorTimer;
		uint32 UnstableAfflictionTimer;

		void Reset()
        {
			ShieldBashTimer = 2000;
			MeltArmorTimer = 3000;
			UnstableAfflictionTimer = 4000;

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
				ShieldBashTimer = urand(3000, 4000);
			}
			else ShieldBashTimer -= diff;

			if (MeltArmorTimer <= diff)
			{
				DoCastVictim(SPELL_MELT_ARMOR_1);
				MeltArmorTimer = urand(5000, 6000);
			}
			else MeltArmorTimer -= diff;

			if (UnstableAfflictionTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_UNSTABLE_AFFLICTION);
					UnstableAfflictionTimer = urand(7000, 8000);
				}
			}
			else UnstableAfflictionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodwarder_centurionAI(creature);
    }
};

class npc_bloodwarder_slayer : public CreatureScript
{
public:
	npc_bloodwarder_slayer() : CreatureScript("npc_bloodwarder_slayer") {}

    struct npc_bloodwarder_slayerAI : public QuantumBasicAI
    {
        npc_bloodwarder_slayerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 MortalStrikeTimer;
		uint32 MeltArmorTimer;
		uint32 SolarStrikeTimer;
		uint32 WhirlwindTimer;

		void Reset()
        {
			MortalStrikeTimer = 2000;
			MeltArmorTimer = 3000;
			SolarStrikeTimer = 4000;
			WhirlwindTimer = 5000;

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

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_MORTAL_STRIKE);
				MortalStrikeTimer = urand(3000, 4000);
			}
			else MortalStrikeTimer -= diff;

			if (MeltArmorTimer <= diff)
			{
				DoCastVictim(SPELL_MELT_ARMOR_2);
				MeltArmorTimer = urand(5000, 6000);
			}
			else MeltArmorTimer -= diff;

			if (SolarStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_SOLAR_STRIKE);
				SolarStrikeTimer = urand(7000, 8000);
			}
			else SolarStrikeTimer -= diff;

			if (WhirlwindTimer <= diff)
			{
				DoCastAOE(SPELL_WHIRLWIND);
				WhirlwindTimer = urand(9000, 10000);
			}
			else WhirlwindTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodwarder_slayerAI(creature);
    }
};

class npc_sunseeker_netherbinder : public CreatureScript
{
public:
	npc_sunseeker_netherbinder() : CreatureScript("npc_sunseeker_netherbinder") {}

    struct npc_sunseeker_netherbinderAI : public QuantumBasicAI
    {
        npc_sunseeker_netherbinderAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ArcaneNovaTimer;
		uint32 DispelMagicTimer;
		uint32 ArcaneServantTimer;

		SummonList Summons;

		void Reset()
        {
			Summons.DespawnAll();

			ArcaneNovaTimer = 2000;
			DispelMagicTimer = 4000;
			ArcaneServantTimer = 6000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SummonCreature(NPC_ARCANE_SERVANT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_ARCANE_SERVANT)
				Summons.Summon(summon);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ArcaneNovaTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_ARCANE_NOVA_5N, SPELL_ARCANE_NOVA_5H));
				ArcaneNovaTimer = urand(4000, 5000);
			}
			else ArcaneNovaTimer -= diff;

			if (DispelMagicTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DISPEL_MAGIC);
					DispelMagicTimer = urand(6000, 7000);
				}
			}
			else DispelMagicTimer -= diff;

			if (ArcaneServantTimer <= diff)
			{
				me->SummonCreature(NPC_ARCANE_SERVANT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
				ArcaneServantTimer = urand(10000, 11000);
			}
			else ArcaneServantTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunseeker_netherbinderAI(creature);
    }
};

class npc_sunseeker_engineer : public CreatureScript
{
public:
	npc_sunseeker_engineer() : CreatureScript("npc_sunseeker_engineer") {}

    struct npc_sunseeker_engineerAI : public QuantumBasicAI
    {
        npc_sunseeker_engineerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 DeathRayTimer;
		uint32 SuperShrinkRayTimer;
		uint32 GrowthRayTimer;

		void Reset()
        {
			DeathRayTimer = 500;
			SuperShrinkRayTimer = 2000;
			GrowthRayTimer = 3000;

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

			if (DeathRayTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_DEATH_RAY_5N, SPELL_DEATH_RAY_5H));
					DeathRayTimer = urand(3000, 4000);
				}
			}
			else DeathRayTimer -= diff;

			if (SuperShrinkRayTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SUPER_SHRINK_RAY);
					SuperShrinkRayTimer = urand(5000, 6000);
				}
			}
			else SuperShrinkRayTimer -= diff;

			if (GrowthRayTimer <= diff)
			{
				DoCast(SPELL_GROWTH_RAY);
				GrowthRayTimer = urand(7000, 8000);
			}
			else GrowthRayTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunseeker_engineerAI(creature);
    }
};

class npc_sunseeker_astromage : public CreatureScript
{
public:
	npc_sunseeker_astromage() : CreatureScript("npc_sunseeker_astromage") {}

    struct npc_sunseeker_astromageAI : public QuantumBasicAI
    {
        npc_sunseeker_astromageAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ScorchTimer;
		uint32 SolarburnTimer;

		void Reset()
        {
			ScorchTimer = 500;
			SolarburnTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, DUNGEON_MODE(SPELL_FIRE_SHIELD_5N, SPELL_FIRE_SHIELD_5H));
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ScorchTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SCORCH_5N, SPELL_SCORCH_5H));
					ScorchTimer = 4000;
				}
			}
			else ScorchTimer -= diff;

			if (SolarburnTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SOLARBURN_5N, SPELL_SOLARBURN_5H));
					SolarburnTimer = 6000;
				}
			}
			else SolarburnTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunseeker_astromageAI(creature);
    }
};

class npc_arcane_servant : public CreatureScript
{
public:
	npc_arcane_servant() : CreatureScript("npc_arcane_servant") {}

    struct npc_arcane_servantAI : public QuantumBasicAI
    {
        npc_arcane_servantAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ArcaneVolleyTimer;
		uint32 ArcaneExplosionTimer;

		void Reset()
        {
			ArcaneVolleyTimer = 500;
			ArcaneExplosionTimer = 2000;

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

			if (ArcaneVolleyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ARCANE_VOLLEY);
					ArcaneVolleyTimer = urand(3000, 4000);
				}
			}
			else ArcaneVolleyTimer -= diff;

			if (ArcaneExplosionTimer <= diff)
			{
				DoCastAOE(SPELL_ARCANE_EXPLOSION);
				ArcaneExplosionTimer = urand(5000, 6000);
			}
			else ArcaneExplosionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_arcane_servantAI(creature);
    }
};

class npc_tempest_forge_patroller : public CreatureScript
{
public:
	npc_tempest_forge_patroller() : CreatureScript("npc_tempest_forge_patroller") {}

    struct npc_tempest_forge_patrollerAI : public QuantumBasicAI
    {
        npc_tempest_forge_patrollerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 KnockdownTimer;
		uint32 ChargedArcaneMissileTimer;

		void Reset()
        {
			ChargedArcaneMissileTimer = 500;
			KnockdownTimer = 2000;

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

			if (ChargedArcaneMissileTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_CHARGED_ARCANE_MISSILE_5N, SPELL_CHARGED_ARCANE_MISSILE_5H));
					ChargedArcaneMissileTimer = urand(3000, 4000);
				}
			}
			else ChargedArcaneMissileTimer -= diff;

			if (KnockdownTimer <= diff)
			{
				DoCastVictim(SPELL_KNOCKDOWN);
				KnockdownTimer = urand(5000, 6000);
			}
			else KnockdownTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tempest_forge_patrollerAI(creature);
    }
};

class npc_tempest_forge_destroyer : public CreatureScript
{
public:
	npc_tempest_forge_destroyer() : CreatureScript("npc_tempest_forge_destroyer") {}

    struct npc_tempest_forge_destroyerAI : public QuantumBasicAI
    {
        npc_tempest_forge_destroyerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ChargedFistTimer;
		uint32 KnockdownTimer;

		void Reset()
        {
			ChargedFistTimer = 1000;
			KnockdownTimer = 3000;

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

			if (ChargedFistTimer <= diff)
			{
				DoCast(me, SPELL_CHARGED_FIST, true);
				ChargedFistTimer = 4000;
			}
			else ChargedFistTimer -= diff;

			if (KnockdownTimer <= diff)
			{
				DoCastVictim(SPELL_TFD_KNOCKDOWN);
				KnockdownTimer = 6000;
			}
			else KnockdownTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tempest_forge_destroyerAI(creature);
    }
};

class npc_mechanar_wrecker : public CreatureScript
{
public:
	npc_mechanar_wrecker() : CreatureScript("npc_mechanar_wrecker") {}

    struct npc_mechanar_wreckerAI : public QuantumBasicAI
    {
        npc_mechanar_wreckerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 PoundTimer;
		uint32 GlobOfMachineFluidTimer;

		void Reset()
        {
			PoundTimer = 2000;
			GlobOfMachineFluidTimer = 3000;

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

			if (PoundTimer <= diff)
			{
				DoCastVictim(SPELL_POUND);
				PoundTimer = urand(4000, 5000);
			}
			else PoundTimer -= diff;

			if (GlobOfMachineFluidTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_GLOB_OF_MACHINE_FLUID_5N, SPELL_GLOB_OF_MACHINE_FLUID_5H));
				GlobOfMachineFluidTimer = urand(6000, 7000);
			}
			else GlobOfMachineFluidTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mechanar_wreckerAI(creature);
    }
};

class npc_mechanar_tinkerer : public CreatureScript
{
public:
	npc_mechanar_tinkerer() : CreatureScript("npc_mechanar_tinkerer") {}

    struct npc_mechanar_tinkererAI : public QuantumBasicAI
    {
        npc_mechanar_tinkererAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 NetherExplosionTimer;
		uint32 NetherbombTimer;
		uint32 ManiacalChargeTimer;

		void Reset()
        {
			NetherExplosionTimer = 500;
			NetherbombTimer = 3000;
			ManiacalChargeTimer = 5000;

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

			if (NetherExplosionTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_NETHER_EXPLOSION);
					 NetherExplosionTimer = urand(4000, 5000);
				}
			}
			else NetherExplosionTimer -= diff;

			if (NetherbombTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_NETHERBOMB_5N, SPELL_NETHERBOMB_5H));
					NetherbombTimer = urand(6000, 7000);
				}
			}
			else NetherbombTimer -= diff;

			if (ManiacalChargeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MANIACAL_CHARGE);
					ManiacalChargeTimer = urand(8000, 9000);
				}
			}
			else ManiacalChargeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mechanar_tinkererAI(creature);
    }
};

class npc_mechanar_driller : public CreatureScript
{
public:
	npc_mechanar_driller() : CreatureScript("npc_mechanar_driller") {}

    struct npc_mechanar_drillerAI : public QuantumBasicAI
    {
        npc_mechanar_drillerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 DrillArmorTimer;
		uint32 GlobOfMachineFluidTimer;

		void Reset()
        {
			DrillArmorTimer = 2000;
			GlobOfMachineFluidTimer = 3000;

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

			if (DrillArmorTimer <= diff)
			{
				DoCastVictim(SPELL_DRILL_ARMOR);
				DrillArmorTimer = urand(4000, 5000);
			}
			else DrillArmorTimer -= diff;

			if (GlobOfMachineFluidTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_GLOB_OF_MACHINE_FLUID_5N, SPELL_GLOB_OF_MACHINE_FLUID_5H));
				GlobOfMachineFluidTimer = urand(6000, 7000);
			}
			else GlobOfMachineFluidTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mechanar_drillerAI(creature);
    }
};

class npc_mechanar_crusher : public CreatureScript
{
public:
	npc_mechanar_crusher() : CreatureScript("npc_mechanar_crusher") {}

    struct npc_mechanar_crusherAI : public QuantumBasicAI
    {
        npc_mechanar_crusherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 TheClawTimer;
		uint32 GlobOfMachineFluidTimer;

		void Reset()
        {
			TheClawTimer = 2000;
			GlobOfMachineFluidTimer = 3000;

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

			if (TheClawTimer <= diff)
			{
				DoCastVictim(SPELL_THE_CLAW);
				TheClawTimer = urand(4000, 5000);
			}
			else TheClawTimer -= diff;

			if (GlobOfMachineFluidTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_GLOB_OF_MACHINE_FLUID_5N, SPELL_GLOB_OF_MACHINE_FLUID_5H));
				GlobOfMachineFluidTimer = urand(6000, 7000);
			}
			else GlobOfMachineFluidTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mechanar_crusherAI(creature);
    }
};

void AddSC_mechanar_trash()
{
	new npc_bloodwarder_physician();
	new npc_bloodwarder_centurion();
	new npc_bloodwarder_slayer();
	new npc_sunseeker_netherbinder();
	new npc_sunseeker_engineer();
	new npc_sunseeker_astromage();
	new npc_arcane_servant();
	new npc_tempest_forge_patroller();
	new npc_tempest_forge_destroyer();
	new npc_mechanar_wrecker();
	new npc_mechanar_tinkerer();
	new npc_mechanar_driller();
	new npc_mechanar_crusher();
}