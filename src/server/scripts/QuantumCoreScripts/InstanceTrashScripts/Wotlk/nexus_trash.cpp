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
#include "../../../scripts/Northrend/Nexus/Nexus/nexus.h"

#define EMOTE_REGENERATE "%s begins to regenerate"

enum NexusSpells
{
	SPELL_FROSTBOLT_5N         = 15530,
	SPELL_FROSTBOLT_5H         = 56775,
	SPELL_ARCANE_SHOCK_5N      = 37132,
	SPELL_ARCANE_SHOCK_5H      = 56776,
	SPELL_MANA_SHIELD_5N       = 46151,
	SPELL_MANA_SHIELD_5H       = 56778,
	SPELL_SPELL_LOCK           = 30849,
	SPELL_DRAW_MAGIC           = 50131,
	SPELL_ARCANE_EXPLOSION_5N  = 34933,
	SPELL_ARCANE_EXPLOSION_5H  = 56825,
	SPELL_AURA_ARCANE_HASTE_5N = 50182,
	SPELL_AURA_ARCANE_HASTE_5H = 56827,
	SPELL_ARCANE_TRAP          = 47789,
	SPELL_CONE_OF_COLD_5N      = 15244,
	SPELL_CONE_OF_COLD_5H      = 38384,
	SPELL_FIREBALL_5N          = 12466,
	SPELL_FIREBALL_5H          = 17290,
	SPELL_MHA_FROSTBOLT_5N     = 12737,
	SPELL_MHA_FROSTBOLT_5H     = 56837,
	SPELL_RAIN_OF_FIRE_5N      = 36808,
	SPELL_RAIN_OF_FIRE_5H      = 39376,
	SPELL_POLYMORPH            = 13323,
	SPELL_FREEZING_TRAP        = 55040,
	SPELL_IMMOLATION_TRAP      = 47784,
	SPELL_FROZEN_PRISON        = 47543,
	SPELL_FLASH_HEAL_5N        = 17843,
	SPELL_FLASH_HEAL_5H        = 56919,
	SPELL_SHADOW_WORD_DEATH_5N = 47697,
	SPELL_SHADOW_WORD_DEATH_5H = 56920,
	SPELL_POWER_WORD_SHIELD_5N = 17139,
	SPELL_POWER_WORD_SHIELD_5H = 35944,
	SPELL_BLOODLUST            = 21049,
	SPELL_WAR_STOMP            = 38682,
	SPELL_FRENZY               = 47774,
	SPELL_SHOOT_5N             = 22907,
	SPELL_SHOOT_5H             = 48854,
	SPELL_INCENDIARY_SHOT_5N   = 47777,
	SPELL_INCENDIARY_SHOT_5H   = 56933,
	SPELL_RAPID_SHOT_5N        = 48191,
	SPELL_RAPID_SHOT_5H        = 49474,
	SPELL_ARCANE_CHANNELING    = 54219,
	SPELL_DRAIN_MANA           = 17682,
	SPELL_MAGIC_BURN_5N        = 50198,
	SPELL_MAGIC_BURN_5H        = 56860,
	SPELL_RENEW                = 25058,
	SPELL_ARCANE_TORRENT       = 47779,
	SPELL_SPELLBREAKER         = 47780,
	SPELL_MALEVOLENT_STRIKE    = 39171,
	SPELL_BRUTAL_STRIKE        = 58460,
	SPELL_CLEAVE               = 15284,
	SPELL_ARCANE_BLAST_5N      = 38881,
	SPELL_ARCANE_BLAST_5H      = 56969,
	SPELL_HEAL_5N              = 15586,
	SPELL_HEAL_5H              = 61326,
	SPELL_ARCANE_MISSILES      = 33832,
	SPELL_ARCANE_NOVA_5N       = 47696,
	SPELL_ARCANE_NOVA_5H       = 57046,
	SPELL_MANA_BURN_5N         = 48054,
	SPELL_MANA_BURN_5H         = 57047,
	SPELL_LOOSE_MANA           = 29882,
	SPELL_AURA_OF_REGEN_5N     = 52067,
	SPELL_AURA_OF_REGEN_5H     = 57056,
	SPELL_ENSNARE              = 48053,
	SPELL_CRYSTAL_BLOOM        = 48058,
	SPELL_SEED_POD             = 48082,
	SPELL_CRYSTAL_SPIKE_5N     = 33688,
	SPELL_CRYSTAL_SPIKE_5H     = 57052,
	SPELL_CRYSTAL_BARK         = 47699,
	SPELL_THUNDERCLAP          = 30633,
	SPELL_CRYSTAL_CHAINS_5N    = 47698,
	SPELL_CRYSTAL_CHAINS_5H    = 57050,
	SPELL_SHARD_SPRAY_5N       = 50302,
	SPELL_SHARD_SPRAY_5H       = 57051,
	SPELL_TOUGHEN_HIDE         = 50994,
	SPELL_TRANQUILITY_5N       = 51972,
	SPELL_TRANQUILITY_5H       = 57054,
	SPELL_ENTANGLING_ROOTS     = 28858,
	SPELL_GHOST_VISUAL         = 35838,
};

class npc_azure_magus : public CreatureScript
{
public:
    npc_azure_magus() : CreatureScript("npc_azure_magus") {}

    struct npc_azure_magusAI : public QuantumBasicAI
    {
        npc_azure_magusAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FrostboltTimer;
		uint32 ArcaneShockTimer;
		uint32 ManaShieldTimer;

        void Reset()
        {
			FrostboltTimer = 500;
			ArcaneShockTimer = 2000;
			ManaShieldTimer = 4000;

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

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FROSTBOLT_5N, SPELL_FROSTBOLT_5H));
					FrostboltTimer = urand(3000, 4000);
				}
			}
			else FrostboltTimer -= diff;

			if (ArcaneShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_ARCANE_SHOCK_5N, SPELL_ARCANE_SHOCK_5H));
					ArcaneShockTimer = urand(5000, 6000);
				}
			}
			else ArcaneShockTimer -= diff;

			if (ManaShieldTimer <= diff)
			{
				DoCast(me, (DUNGEON_MODE(SPELL_MANA_SHIELD_5N, SPELL_MANA_SHIELD_5H)));
				ManaShieldTimer = urand(7000, 8000);
			}
			else ManaShieldTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_azure_magusAI(creature);
    }
};

class npc_mage_slayer : public CreatureScript
{
public:
    npc_mage_slayer() : CreatureScript("npc_mage_slayer") {}

    struct npc_mage_slayerAI : public QuantumBasicAI
    {
        npc_mage_slayerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 DrawMagicTimer;
		uint32 SpellLockTimer;

        void Reset()
        {
			DrawMagicTimer = 2000;
			SpellLockTimer = 4000;

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

			if (DrawMagicTimer <= diff)
			{
				DoCastAOE(SPELL_DRAW_MAGIC);
				DrawMagicTimer = urand(5000, 6000);
			}
			else DrawMagicTimer -= diff;

			if (SpellLockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA && target->HasUnitState(UNIT_STATE_CASTING))
					{
						DoCast(target, SPELL_SPELL_LOCK);
						SpellLockTimer = urand(7000, 8000);
					}
				}
			}
			else SpellLockTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mage_slayerAI(creature);
    }
};

class npc_mage_hunter_ascendant : public CreatureScript
{
public:
    npc_mage_hunter_ascendant() : CreatureScript("npc_mage_hunter_ascendant") {}

    struct npc_mage_hunter_ascendantAI : public QuantumBasicAI
    {
        npc_mage_hunter_ascendantAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ArcaneHasteTimer;
		uint32 FireballTimer;
		uint32 FrostboltTimer;
		uint32 ArcaneExplosionTimer;
		uint32 ConeOfColdTimer;
		uint32 RainOfFireTimer;
		uint32 PolymorphTimer;

        void Reset()
        {
			ArcaneHasteTimer = 500;
			FireballTimer = 2000;
			FrostboltTimer = 3000;
			ArcaneExplosionTimer = 4000;
			ConeOfColdTimer = 5000;
			RainOfFireTimer = 6000;
			PolymorphTimer = 7000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastAOE(SPELL_IMMOLATION_TRAP);
		}

		void JustDied(Unit* /*killer*/)
		{
			DoCastAOE(SPELL_FREEZING_TRAP);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ArcaneHasteTimer <= diff)
			{
				DoCast(me, DUNGEON_MODE(SPELL_AURA_ARCANE_HASTE_5N, SPELL_AURA_ARCANE_HASTE_5H));
				ArcaneHasteTimer = urand(2000, 3000);
			}
			else ArcaneHasteTimer -= diff;

			if (FireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FIREBALL_5N, SPELL_FIREBALL_5H));
					FireballTimer = urand(4000, 5000);
				}
			}
			else FireballTimer -= diff;

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FROSTBOLT_5N, SPELL_FROSTBOLT_5H));
					FrostboltTimer = urand(6000, 7000);
				}
			}
			else FrostboltTimer -= diff;

			if (ArcaneExplosionTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_ARCANE_EXPLOSION_5N, SPELL_ARCANE_EXPLOSION_5H));
				ArcaneExplosionTimer = urand(8000, 9000);
			}
			else ArcaneExplosionTimer -= diff;

			if (ConeOfColdTimer <= diff)
			{
				DoCast(DUNGEON_MODE(SPELL_CONE_OF_COLD_5N, SPELL_CONE_OF_COLD_5H));
				ConeOfColdTimer = urand(10000, 11000);
			}
			else ConeOfColdTimer -= diff;

			if (RainOfFireTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_RAIN_OF_FIRE_5N, SPELL_RAIN_OF_FIRE_5H));
					RainOfFireTimer = urand(12000, 13000);
				}
			}
			else RainOfFireTimer -= diff;

			if (PolymorphTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_POLYMORPH);
						PolymorphTimer = urand(14000, 15000);
					}
				}
			}
			else PolymorphTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mage_hunter_ascendantAI(creature);
    }
};

class npc_alliance_cleric : public CreatureScript
{
public:
    npc_alliance_cleric() : CreatureScript("npc_alliance_cleric") {}

    struct npc_alliance_clericAI : public QuantumBasicAI
    {
        npc_alliance_clericAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FlashHealTimer;
		uint32 ShadowWorldDeathTimer;
		uint32 PowerWordShieldTimer;

        void Reset()
        {
			FlashHealTimer = 500;
			ShadowWorldDeathTimer = 2000;
			PowerWordShieldTimer = 4000;

			DoCast(me, SPELL_FROZEN_PRISON);
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_FROZEN_PRISON);
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

			if (FlashHealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FLASH_HEAL_5N, SPELL_FLASH_HEAL_5H));
					FlashHealTimer = urand(3000, 4000);
				}
			}
			else FlashHealTimer -= diff;

			if (ShadowWorldDeathTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_WORD_DEATH_5N, SPELL_SHADOW_WORD_DEATH_5H));
					ShadowWorldDeathTimer = urand(5000, 6000);
				}
			}
			else ShadowWorldDeathTimer -= diff;

			if (PowerWordShieldTimer <= diff)
			{
				DoCast(me, DUNGEON_MODE(SPELL_POWER_WORD_SHIELD_5N, SPELL_POWER_WORD_SHIELD_5H));
				PowerWordShieldTimer = urand(7000, 8000);
			}
			else PowerWordShieldTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_70))
				DoCast(me, DUNGEON_MODE(SPELL_FLASH_HEAL_5N, SPELL_FLASH_HEAL_5H));

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_alliance_clericAI(creature);
    }
};

class npc_alliance_berserker : public CreatureScript
{
public:
    npc_alliance_berserker() : CreatureScript("npc_alliance_berserker") {}

    struct npc_alliance_berserkerAI : public QuantumBasicAI
    {
        npc_alliance_berserkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 BloodlustTimer;
		uint32 WarStompTimer;

        void Reset()
        {
			BloodlustTimer = 2000;
			WarStompTimer = 3000;

			DoCast(me, SPELL_FROZEN_PRISON);
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_FROZEN_PRISON);
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

			if (BloodlustTimer <= diff)
			{
				DoCast(me, SPELL_BLOODLUST);
				BloodlustTimer = urand(3000, 4000);
			}
			else BloodlustTimer -= diff;

			if (WarStompTimer <= diff)
			{
				DoCastAOE(SPELL_WAR_STOMP);
				WarStompTimer = urand(5000, 6000);
			}
			else WarStompTimer -= diff;

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
        return new npc_alliance_berserkerAI(creature);
    }
};

class npc_alliance_ranger : public CreatureScript
{
public:
    npc_alliance_ranger() : CreatureScript("npc_alliance_ranger") {}

    struct npc_alliance_rangerAI : public QuantumBasicAI
    {
        npc_alliance_rangerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShootTimer;
		uint32 IncendiaryShotTimer;
		uint32 RapidShotTimer;

        void Reset()
        {
			ShootTimer = 500;
			IncendiaryShotTimer = 4000;
			RapidShotTimer = 6000;

			DoCast(me, SPELL_FROZEN_PRISON);
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_FROZEN_PRISON);
			DoCastVictim(DUNGEON_MODE(SPELL_SHOOT_5N, SPELL_SHOOT_5H));
		}

		void JustReachedHome()
		{
			Reset();
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
						DoCast(target, DUNGEON_MODE(SPELL_SHOOT_5N, SPELL_SHOOT_5H));
						ShootTimer = urand(2000, 3000);
					}
				}
			}
			else ShootTimer -= diff;

			if (IncendiaryShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, DUNGEON_MODE(SPELL_INCENDIARY_SHOT_5N, SPELL_INCENDIARY_SHOT_5H));
						IncendiaryShotTimer = urand(4000, 5000);
					}
				}
			}
			else IncendiaryShotTimer -= diff;

			if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
				{
                    DoStartMovement(me->GetVictim());
				}

                DoMeleeAttackIfReady();
            }
			else
			{
				if (me->GetDistance2d(me->GetVictim()) > 10)
                    DoStartNoMovement(me->GetVictim());

                if (me->GetDistance2d(me->GetVictim()) > 30)
                    DoStartMovement(me->GetVictim());

				if (RapidShotTimer <= diff)
				{
					DoCastVictim(DUNGEON_MODE(SPELL_RAPID_SHOT_5N, SPELL_RAPID_SHOT_5H));
					RapidShotTimer = urand(6000, 7000);
				}
				else RapidShotTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_alliance_rangerAI(creature);
    }
};

class npc_horde_cleric : public CreatureScript
{
public:
    npc_horde_cleric() : CreatureScript("npc_horde_cleric") {}

    struct npc_horde_clericAI : public QuantumBasicAI
    {
        npc_horde_clericAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FlashHealTimer;
		uint32 ShadowWorldDeathTimer;
		uint32 PowerWordShieldTimer;

        void Reset()
        {
			FlashHealTimer = 500;
			ShadowWorldDeathTimer = 2000;
			PowerWordShieldTimer = 4000;

			DoCast(me, SPELL_FROZEN_PRISON);
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_FROZEN_PRISON);
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

			if (FlashHealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FLASH_HEAL_5N, SPELL_FLASH_HEAL_5H));
					FlashHealTimer = urand(3000, 4000);
				}
			}
			else FlashHealTimer -= diff;

			if (ShadowWorldDeathTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_WORD_DEATH_5N, SPELL_SHADOW_WORD_DEATH_5H));
					ShadowWorldDeathTimer = urand(5000, 6000);
				}
			}
			else ShadowWorldDeathTimer -= diff;

			if (PowerWordShieldTimer <= diff)
			{
				DoCast(me, DUNGEON_MODE(SPELL_POWER_WORD_SHIELD_5N, SPELL_POWER_WORD_SHIELD_5H));
				PowerWordShieldTimer = urand(7000, 8000);
			}
			else PowerWordShieldTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_70))
				DoCast(me, DUNGEON_MODE(SPELL_FLASH_HEAL_5N, SPELL_FLASH_HEAL_5H));

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_horde_clericAI(creature);
    }
};

class npc_horde_berserker : public CreatureScript
{
public:
    npc_horde_berserker() : CreatureScript("npc_horde_berserker") {}

    struct npc_horde_berserkerAI : public QuantumBasicAI
    {
        npc_horde_berserkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 BloodlustTimer;
		uint32 WarStompTimer;

        void Reset()
        {
			BloodlustTimer = 2000;
			WarStompTimer = 3000;

			DoCast(me, SPELL_FROZEN_PRISON);
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_FROZEN_PRISON);
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

			if (BloodlustTimer <= diff)
			{
				DoCast(me, SPELL_BLOODLUST);
				BloodlustTimer = urand(3000, 4000);
			}
			else BloodlustTimer -= diff;

			if (WarStompTimer <= diff)
			{
				DoCastAOE(SPELL_WAR_STOMP);
				WarStompTimer = urand(5000, 6000);
			}
			else WarStompTimer -= diff;

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
        return new npc_horde_berserkerAI(creature);
    }
};

class npc_horde_ranger : public CreatureScript
{
public:
    npc_horde_ranger() : CreatureScript("npc_horde_ranger") {}

    struct npc_horde_rangerAI : public QuantumBasicAI
    {
        npc_horde_rangerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShootTimer;
		uint32 IncendiaryShotTimer;
		uint32 RapidShotTimer;

        void Reset()
        {
			ShootTimer = 500;
			IncendiaryShotTimer = 4000;
			RapidShotTimer = 6000;

			DoCast(me, SPELL_FROZEN_PRISON);
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_FROZEN_PRISON);
			DoCastVictim(DUNGEON_MODE(SPELL_SHOOT_5N, SPELL_SHOOT_5H));
		}

		void JustReachedHome()
		{
			Reset();
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
						DoCast(target, DUNGEON_MODE(SPELL_SHOOT_5N, SPELL_SHOOT_5H));
						ShootTimer = urand(2000, 3000);
					}
				}
			}
			else ShootTimer -= diff;

			if (IncendiaryShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, DUNGEON_MODE(SPELL_INCENDIARY_SHOT_5N, SPELL_INCENDIARY_SHOT_5H));
						IncendiaryShotTimer = urand(4000, 5000);
					}
				}
			}
			else IncendiaryShotTimer -= diff;

			if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
				{
                    DoStartMovement(me->GetVictim());
				}

                DoMeleeAttackIfReady();
            }
			else
			{
				if (me->GetDistance2d(me->GetVictim()) > 10)
                    DoStartNoMovement(me->GetVictim());

                if (me->GetDistance2d(me->GetVictim()) > 30)
                    DoStartMovement(me->GetVictim());

				if (RapidShotTimer <= diff)
				{
					DoCastVictim(DUNGEON_MODE(SPELL_RAPID_SHOT_5N, SPELL_RAPID_SHOT_5H));
					RapidShotTimer = urand(6000, 7000);
				}
				else RapidShotTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_horde_rangerAI(creature);
    }
};

class npc_mage_hunter_initiate : public CreatureScript
{
public:
    npc_mage_hunter_initiate() : CreatureScript("npc_mage_hunter_initiate") {}

    struct npc_mage_hunter_initiateAI : public QuantumBasicAI
    {
        npc_mage_hunter_initiateAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 MagicBurnTimer;
		uint32 RenewTimer;
		uint32 DrainManaTimer;

        void Reset()
        {
			MagicBurnTimer = 500;
			RenewTimer = 2000;
			DrainManaTimer = 4000;

			DoCast(me, SPELL_ARCANE_CHANNELING);
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_ARCANE_CHANNELING);
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

			if (MagicBurnTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_MAGIC_BURN_5N, SPELL_MAGIC_BURN_5H));
				MagicBurnTimer = urand(3000, 4000);
			}
			else MagicBurnTimer -= diff;

			if (RenewTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, SPELL_RENEW);
					RenewTimer = urand(5000, 6000);
				}
			}
			else RenewTimer -= diff;

			if (DrainManaTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_DRAIN_MANA);
						DrainManaTimer = urand(7000, 8000);
					}
				}
			}
			else DrainManaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mage_hunter_initiateAI(creature);
    }
};

class npc_steward : public CreatureScript
{
public:
    npc_steward() : CreatureScript("npc_steward") {}

    struct npc_stewardAI : public QuantumBasicAI
    {
        npc_stewardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ArcaneTorrentTimer;
		uint32 SpellBreakerTimer;

        void Reset()
        {
			ArcaneTorrentTimer = 1000;
			SpellBreakerTimer = 3000;

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

			if (ArcaneTorrentTimer <= diff)
			{
				DoCastAOE(SPELL_ARCANE_TORRENT);
				ArcaneTorrentTimer = urand(4000, 5000);
			}
			else ArcaneTorrentTimer -= diff;

			if (SpellBreakerTimer <= diff)
			{
				DoCastVictim(SPELL_SPELLBREAKER);
				SpellBreakerTimer = urand(6000, 7000);
			}
			else SpellBreakerTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stewardAI(creature);
    }
};

class npc_azure_enforcer : public CreatureScript
{
public:
    npc_azure_enforcer() : CreatureScript("npc_azure_enforcer") {}

    struct npc_azure_enforcerAI : public QuantumBasicAI
    {
        npc_azure_enforcerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 MalevolentStrikeTimer;
		uint32 BrutalStrikeTimer;
		uint32 CleaveTimer;

        void Reset()
        {
			MalevolentStrikeTimer = 2000;
			BrutalStrikeTimer = 4000;
			CleaveTimer = 6000;

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

			if (MalevolentStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_MALEVOLENT_STRIKE);
				MalevolentStrikeTimer = urand(3000, 4000);
			}
			else MalevolentStrikeTimer -= diff;

			if (BrutalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_BRUTAL_STRIKE);
				BrutalStrikeTimer = urand(5000, 6000);
			}
			else BrutalStrikeTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(7000, 8000);
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_azure_enforcerAI(creature);
    }
};

class npc_azure_scale_binder : public CreatureScript
{
public:
    npc_azure_scale_binder() : CreatureScript("npc_azure_scale_binder") {}

    struct npc_azure_scale_binderAI : public QuantumBasicAI
    {
        npc_azure_scale_binderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ArcaneBlastTimer;
		uint32 HealTimer;

        void Reset()
        {
			ArcaneBlastTimer = 500;
			HealTimer = 2000;

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

			if (ArcaneBlastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_ARCANE_BLAST_5N, SPELL_ARCANE_BLAST_5H));
					ArcaneBlastTimer = urand(3000, 4000);
				}
			}
			else ArcaneBlastTimer -= diff;

			if (HealTimer <= diff)
			{
				DoCast(me, DUNGEON_MODE(SPELL_HEAL_5N, SPELL_HEAL_5H));
				HealTimer = urand(5000, 6000);
			}
			else HealTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_azure_scale_binderAI(creature);
    }
};

class npc_crazed_mana_wraith : public CreatureScript
{
public:
    npc_crazed_mana_wraith() : CreatureScript("npc_crazed_mana_wraith") {}

    struct npc_crazed_mana_wraithAI : public QuantumBasicAI
    {
        npc_crazed_mana_wraithAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ArcaneMisiilesTimer;

        void Reset()
        {
			ArcaneMisiilesTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_GHOST_VISUAL, true);
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ArcaneMisiilesTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ARCANE_MISSILES);
					ArcaneMisiilesTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ArcaneMisiilesTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crazed_mana_wraithAI(creature);
    }
};

class npc_crazed_mana_surge : public CreatureScript
{
public:
    npc_crazed_mana_surge() : CreatureScript("npc_crazed_mana_surge") {}

    struct npc_crazed_mana_surgeAI : public QuantumBasicAI
    {
        npc_crazed_mana_surgeAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ArcaneNovaTimer;
		uint32 ManaBurnTimer;

        void Reset()
        {
			ArcaneNovaTimer = 2*IN_MILLISECONDS;
			ManaBurnTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* /*killer*/)
		{
			DoCastAOE(SPELL_LOOSE_MANA);
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
				ArcaneNovaTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else ArcaneNovaTimer -= diff;

			if (ManaBurnTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, DUNGEON_MODE(SPELL_MANA_BURN_5N, SPELL_MANA_BURN_5H));
						ManaBurnTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
					}
				}
			}
			else ManaBurnTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crazed_mana_surgeAI(creature);
    }
};

class npc_crystalline_frayer : public CreatureScript
{
public:
    npc_crystalline_frayer() : CreatureScript("npc_crystalline_frayer") {}

    struct npc_crystalline_frayerAI : public QuantumBasicAI
    {
        npc_crystalline_frayerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CrystalBloomTimer;
		uint32 EnsnareTimer;

        void Reset()
        {
			CrystalBloomTimer = 1000;
			EnsnareTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetReactState(REACT_AGGRESSIVE);
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustReachedHome()
		{
			Reset();
		}

		void DamageTaken(Unit* /*attacker*/, uint32 &amount)
		{
			if (amount >= me->GetHealth())
			{
				amount = 0;

				me->SetCurrentFaction(35);
				me->MonsterTextEmote(EMOTE_REGENERATE, LANG_UNIVERSAL, false);
				DoCast(me, SPELL_SEED_POD);
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CrystalBloomTimer <= diff)
			{
				DoCastAOE(SPELL_CRYSTAL_BLOOM);
				CrystalBloomTimer = urand(2000, 3000);
			}
			else CrystalBloomTimer -= diff;

			if (EnsnareTimer <= diff)
			{
				DoCastVictim(SPELL_ENSNARE);
				EnsnareTimer = urand(4000, 5000);
			}
			else EnsnareTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
				DoCastAOE(DUNGEON_MODE(SPELL_AURA_OF_REGEN_5N, SPELL_AURA_OF_REGEN_5H));

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crystalline_frayerAI(creature);
    }
};

class npc_crystalline_keeper : public CreatureScript
{
public:
    npc_crystalline_keeper() : CreatureScript("npc_crystalline_keeper") {}

    struct npc_crystalline_keeperAI : public QuantumBasicAI
    {
        npc_crystalline_keeperAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CrystalSpikeTimer;
		uint32 CrystalBarkTimer;

        void Reset()
        {
			CrystalSpikeTimer = 2000;
			CrystalBarkTimer = 4000;

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

			if (CrystalSpikeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_CRYSTAL_SPIKE_5N, SPELL_CRYSTAL_SPIKE_5H));
					CrystalSpikeTimer = urand(4000, 5000);
				}
			}
			else CrystalSpikeTimer -= diff;

			if (CrystalBarkTimer <= diff)
			{
				DoCast(me, SPELL_CRYSTAL_BARK);
				CrystalBarkTimer = urand(6000, 7000);
			}
			else CrystalBarkTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crystalline_keeperAI(creature);
    }
};

class npc_crystalline_protector : public CreatureScript
{
public:
    npc_crystalline_protector() : CreatureScript("npc_crystalline_protector") {}

    struct npc_crystalline_protectorAI : public QuantumBasicAI
    {
        npc_crystalline_protectorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ThunderClapTimer;
		uint32 ShardSprayTimer;
		uint32 CrystalChainsTimer;

        void Reset()
        {
			ThunderClapTimer = 2000;
			ShardSprayTimer = 3000;
			CrystalChainsTimer = 4000;

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

			if (ThunderClapTimer <= diff)
			{
				DoCastAOE(SPELL_THUNDERCLAP);
				ThunderClapTimer = urand(3000, 4000);
			}
			else ThunderClapTimer -= diff;

			if (ShardSprayTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_SHARD_SPRAY_5N, SPELL_SHARD_SPRAY_5H));
				ShardSprayTimer = urand(5000, 6000);
			}
			else ShardSprayTimer -= diff;

			if (CrystalChainsTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_CRYSTAL_CHAINS_5N, SPELL_CRYSTAL_CHAINS_5H));
				CrystalChainsTimer = urand(7000, 8000);
			}
			else CrystalChainsTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crystalline_protectorAI(creature);
    }
};

class npc_crystalline_tender : public CreatureScript
{
public:
    npc_crystalline_tender() : CreatureScript("npc_crystalline_tender") {}

    struct npc_crystalline_tenderAI : public QuantumBasicAI
    {
        npc_crystalline_tenderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ToughenHideTimer;
		uint32 TranquilityTimer;

        void Reset()
        {
			ToughenHideTimer = 2000;
			TranquilityTimer = 4000;

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

			if (ToughenHideTimer <= diff)
			{
				DoCastAOE(SPELL_TOUGHEN_HIDE);
				ToughenHideTimer = urand(3000, 4000);
			}
			else ToughenHideTimer -= diff;

			if (TranquilityTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_TRANQUILITY_5N, SPELL_TRANQUILITY_5H));
				TranquilityTimer = urand(6000, 7000);
			}
			else TranquilityTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crystalline_tenderAI(creature);
    }
};

class npc_crystalline_tangler : public CreatureScript
{
public:
	npc_crystalline_tangler() : CreatureScript("npc_crystalline_tangler") { }

    struct npc_crystalline_tanglerAI : public QuantumBasicAI
    {
        npc_crystalline_tanglerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 EntanglingRootsTimer;

        void Reset()
        {
            EntanglingRootsTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (EntanglingRootsTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
                    DoCast(target, SPELL_ENTANGLING_ROOTS, true);
                    EntanglingRootsTimer = 4500;
                }
            }
			else EntanglingRootsTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crystalline_tanglerAI(creature);
    }
};

void AddSC_nexus_trash()
{
	new npc_azure_magus();
	new npc_mage_slayer();
	new npc_mage_hunter_ascendant();
	new npc_alliance_cleric();
	new npc_alliance_berserker();
	new npc_alliance_ranger();
	new npc_horde_cleric();
	new npc_horde_berserker();
	new npc_horde_ranger();
	new npc_mage_hunter_initiate();
	new npc_steward();
	new npc_azure_enforcer();
	new npc_azure_scale_binder();
	new npc_crazed_mana_wraith();
	new npc_crazed_mana_surge();
	new npc_crystalline_frayer();
	new npc_crystalline_keeper();
	new npc_crystalline_protector();
	new npc_crystalline_tender();
	new npc_crystalline_tangler();
}