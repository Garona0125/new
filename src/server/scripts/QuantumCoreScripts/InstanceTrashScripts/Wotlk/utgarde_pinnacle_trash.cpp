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
#include "../../../scripts/Northrend/UtgardeKeep/UtgardePinnacle/utgarde_pinnacle.h"

enum UtgardePinnacleSpells
{
    SPELL_MIGHTY_BLOW           = 48697,
    SPELL_VOLATILE_INFECTION_5N = 56785,
    SPELL_VOLATILE_INFECTION_5H = 59228,
	SPELL_CLEAVE                = 42724,
	SPELL_GRAVE_STRIKE_5N       = 48851,
	SPELL_GRAVE_STRIKE_5H       = 59079,
	SPELL_FERVOR                = 48702,
	SPELL_LIGHTNING_BOLT_5N     = 48698,
	SPELL_LIGHTNING_BOLT_5H     = 59081,
	SPELL_CHAIN_LIGHTNING_5N    = 48699,
	SPELL_CHAIN_LIGHTNING_5H    = 59082,
	SPELL_HEALING_WAVE_5N       = 48700,
	SPELL_HEALING_WAVE_5H       = 59083,
	SPELL_SHOOT_5N              = 48854,
	SPELL_SHOOT_5H              = 59241,
	SPELL_AIMED_SHOT_5N         = 48871,
	SPELL_AIMED_SHOT_5H         = 59243,
	SPELL_MULTI_SHOT_5N         = 48872,
	SPELL_MULTI_SHOT_5H         = 59244,
	SPELL_BEASTS_MARK_5N        = 48876,
	SPELL_BEASTS_MARK_5H        = 59237,
	SPELL_REND_5N               = 48880,
	SPELL_REND_5H               = 59239,
	SPELL_FIXATE                = 49029,
	SPELL_SHRED_5N              = 49121,
	SPELL_SHRED_5H              = 61548,
	SPELL_TERRIFY               = 49106,
	SPELL_SHADOW_BOLT_5N        = 51432,
	SPELL_SHADOW_BOLT_5H        = 59254,
	SPELL_SHADOW_BOLT_VOLLEY_5N = 49205,
	SPELL_SHADOW_BOLT_VOLLEY_5H = 59255,
	SPELL_DARK_MENDING_5N       = 49204,
	SPELL_DARK_MENDING_5H       = 59252,
	SPELL_LYCANTHROPY           = 49170,
	SPELL_HAMSTRING             = 48639,
	SPELL_STRIKE                = 48640,
	SPELL_YWD_SHADOW_BOLT_5N    = 49084,
	SPELL_YWD_SHADOW_BOLT_5H    = 59246,
	SPELL_SHRINK_5N             = 49089,
	SPELL_SHRINK_5H             = 59247,
	SPELL_THROW                 = 49091,
	SPELL_NET                   = 49092,
};

class npc_scourge_hulk : public CreatureScript
{
public:
	npc_scourge_hulk() : CreatureScript("npc_scourge_hulk") { }

	struct npc_scourge_hulkAI : public QuantumBasicAI
	{
		npc_scourge_hulkAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		bool KilledByRitualStrike;

		uint32 MightyBlowTimer;
		uint32 VolatileInfectionTimer;

		void Reset()
		{
			MightyBlowTimer = 2000;
			VolatileInfectionTimer = 4000;

			KilledByRitualStrike = false;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		uint32 GetData(uint32 type)
		{
			return type == DATA_INCREDIBLE_HULK ? KilledByRitualStrike : 0;
		}

		void DamageTaken(Unit* attacker, uint32 &damage)
		{
			if (damage >= me->GetHealth() && attacker->GetEntry() == NPC_SVALA_SORROWGRAVE)
				KilledByRitualStrike = true;
		}

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MightyBlowTimer <= diff)
			{
				if (Unit* victim = me->GetVictim())
				{
					if (!victim->HasUnitState(UNIT_STATE_STUNNED))
						DoCast(victim, SPELL_MIGHTY_BLOW);

					MightyBlowTimer = urand(6000, 7000);
				}
			}
			else MightyBlowTimer -= diff;

			if (VolatileInfectionTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_VOLATILE_INFECTION_5N, SPELL_VOLATILE_INFECTION_5H));
				VolatileInfectionTimer = urand(8000, 9000);
			}
			else VolatileInfectionTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_scourge_hulkAI(creature);
	}
};

class npc_dragonflayer_deathseeker : public CreatureScript
{
public:
    npc_dragonflayer_deathseeker() : CreatureScript("npc_dragonflayer_deathseeker") {}

    struct npc_dragonflayer_deathseekerAI : public QuantumBasicAI
    {
        npc_dragonflayer_deathseekerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CleaveTimer;
		uint32 GraveStrikeTimer;

        void Reset()
        {
			CleaveTimer = 2000;
			GraveStrikeTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
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

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(3000, 4000);
			}
			else CleaveTimer -= diff;

			if (GraveStrikeTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_GRAVE_STRIKE_5N, SPELL_GRAVE_STRIKE_5H));
				GraveStrikeTimer = urand(5000, 6000);
			}
			else GraveStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_deathseekerAI(creature);
    }
};

class npc_dragonflayer_fanatic : public CreatureScript
{
public:
    npc_dragonflayer_fanatic() : CreatureScript("npc_dragonflayer_fanatic") {}

    struct npc_dragonflayer_fanaticAI : public QuantumBasicAI
    {
        npc_dragonflayer_fanaticAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FervorTimer;

        void Reset()
        {
			FervorTimer = 1000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
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

			if (FervorTimer <= diff)
			{
				DoCast(me, SPELL_FERVOR);
				FervorTimer = 8000;
			}
			else FervorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_fanaticAI(creature);
    }
};

class npc_dragonflayer_seer : public CreatureScript
{
public:
    npc_dragonflayer_seer() : CreatureScript("npc_dragonflayer_seer") {}

    struct npc_dragonflayer_seerAI : public QuantumBasicAI
    {
        npc_dragonflayer_seerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 LightningBoltTimer;
		uint32 ChainLightningTimer;
		uint32 HealingWaveTimer;

        void Reset()
        {
			LightningBoltTimer = 500;
			ChainLightningTimer = 2000;
			HealingWaveTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
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

			if (LightningBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_LIGHTNING_BOLT_5N, SPELL_LIGHTNING_BOLT_5H));
					LightningBoltTimer = urand(3000, 4000);
				}
			}
			else LightningBoltTimer -= diff;

			if (ChainLightningTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_CHAIN_LIGHTNING_5N, SPELL_CHAIN_LIGHTNING_5H));
					ChainLightningTimer = urand(5000, 6000);
				}
			}
			else ChainLightningTimer -= diff;

			if (HealingWaveTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, DUNGEON_MODE(SPELL_HEALING_WAVE_5N, SPELL_HEALING_WAVE_5H));
					HealingWaveTimer = urand(7000, 8000);
				}
			}
			else HealingWaveTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_60))
				DoCast(me, DUNGEON_MODE(SPELL_HEALING_WAVE_5N, SPELL_HEALING_WAVE_5H));

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dragonflayer_seerAI(creature);
    }
};

class npc_ymirjar_flesh_hunter : public CreatureScript
{
public:
    npc_ymirjar_flesh_hunter() : CreatureScript("npc_ymirjar_flesh_hunter") {}

    struct npc_ymirjar_flesh_hunterAI : public QuantumBasicAI
    {
        npc_ymirjar_flesh_hunterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShootTimer;
		uint32 AimedShotTimer;
		uint32 MultiShotTimer;

        void Reset()
        {
			ShootTimer = 500;
			AimedShotTimer = 1000;
			MultiShotTimer = 3000;

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

			if (MultiShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, DUNGEON_MODE(SPELL_MULTI_SHOT_5N, SPELL_MULTI_SHOT_5H));
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

				if (AimedShotTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
						{
							DoCast(target, DUNGEON_MODE(SPELL_AIMED_SHOT_5N, SPELL_AIMED_SHOT_5H), true);
							AimedShotTimer = urand(6000, 7000);
						}
					}
				}
				else AimedShotTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ymirjar_flesh_hunterAI(creature);
    }
};

class npc_ymirjar_savage : public CreatureScript
{
public:
    npc_ymirjar_savage() : CreatureScript("npc_ymirjar_savage") {}

    struct npc_ymirjar_savageAI : public QuantumBasicAI
    {
        npc_ymirjar_savageAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 BeastsMarkTimer;
		uint32 RendTimer;

        void Reset()
        {
			BeastsMarkTimer = 500;
			RendTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
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

			if (BeastsMarkTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_BEASTS_MARK_5N, SPELL_BEASTS_MARK_5H));
					BeastsMarkTimer = urand(3000, 4000);
				}
			}
			else BeastsMarkTimer -= diff;

			if (RendTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_REND_5N, SPELL_REND_5H));
				RendTimer = urand(5000, 6000);
			}
			else RendTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ymirjar_savageAI(creature);
    }
};

class npc_ymirjar_berserker : public CreatureScript
{
public:
    npc_ymirjar_berserker() : CreatureScript("npc_ymirjar_berserker") {}

    struct npc_ymirjar_berserkerAI : public QuantumBasicAI
    {
        npc_ymirjar_berserkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShredTimer;
		uint32 TerrifyTimer;

        void Reset()
        {
			ShredTimer = 500;
			TerrifyTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
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

			if (ShredTimer <= diff)
			{
				DoCast(me, DUNGEON_MODE(SPELL_SHRED_5N, SPELL_SHRED_5H));
				ShredTimer = urand(4000, 5000);
			}
			else ShredTimer -= diff;

			if (TerrifyTimer <= diff)
			{
				DoCastAOE(SPELL_TERRIFY);
				TerrifyTimer = urand(6000, 7000);
			}
			else TerrifyTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ymirjar_berserkerAI(creature);
    }
};

class npc_ymirjar_necromancer : public CreatureScript
{
public:
    npc_ymirjar_necromancer() : CreatureScript("npc_ymirjar_necromancer") {}

    struct npc_ymirjar_necromancerAI : public QuantumBasicAI
    {
        npc_ymirjar_necromancerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShadowBoltTimer;
		uint32 ShadowBoltVolleyTimer;

        void Reset()
        {
			ShadowBoltTimer = 500;
			ShadowBoltVolleyTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
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

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BOLT_5N, SPELL_SHADOW_BOLT_5H));
					ShadowBoltTimer = urand(3000, 4000);
				}
			}
			else ShadowBoltTimer -= diff;

			if (ShadowBoltVolleyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BOLT_VOLLEY_5N, SPELL_SHADOW_BOLT_VOLLEY_5H));
					ShadowBoltVolleyTimer = urand(5000, 6000);
				}
			}
			else ShadowBoltVolleyTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ymirjar_necromancerAI(creature);
    }
};

class npc_ymirjar_dusk_shaman : public CreatureScript
{
public:
    npc_ymirjar_dusk_shaman() : CreatureScript("npc_ymirjar_dusk_shaman") {}

    struct npc_ymirjar_dusk_shamanAI : public QuantumBasicAI
    {
        npc_ymirjar_dusk_shamanAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 DarkMendingTimer;
		uint32 LycanthropyTimer;

        void Reset()
        {
			DarkMendingTimer = 1000;
			LycanthropyTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
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

			if (DarkMendingTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, DUNGEON_MODE(SPELL_DARK_MENDING_5N, SPELL_DARK_MENDING_5H), true);
					DarkMendingTimer = urand(4000, 5000);
				}
			}
			else DarkMendingTimer -= diff;

			if (LycanthropyTimer <= diff)
			{
				DoCast(me, SPELL_LYCANTHROPY, true);
				LycanthropyTimer = 15000;
			}
			else LycanthropyTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_60))
				DoCast(me, DUNGEON_MODE(SPELL_DARK_MENDING_5N, SPELL_DARK_MENDING_5H));

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ymirjar_dusk_shamanAI(creature);
    }
};

class npc_ymirjar_warrior : public CreatureScript
{
public:
    npc_ymirjar_warrior() : CreatureScript("npc_ymirjar_warrior") {}

    struct npc_ymirjar_warriorAI : public QuantumBasicAI
    {
        npc_ymirjar_warriorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 StrikeTimer;
		uint32 HamstringTimer;

        void Reset()
        {
			StrikeTimer = 1000;
			HamstringTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
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

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_STRIKE);
				StrikeTimer = 4000;
			}
			else StrikeTimer -= diff;

			if (HamstringTimer <= diff)
			{
				DoCastVictim(SPELL_HAMSTRING);
				HamstringTimer = 6000;
			}
			else HamstringTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ymirjar_warriorAI(creature);
    }
};

class npc_ymirjar_witch_doctor : public CreatureScript
{
public:
    npc_ymirjar_witch_doctor() : CreatureScript("npc_ymirjar_witch_doctor") {}

    struct npc_ymirjar_witch_doctorAI : public QuantumBasicAI
    {
        npc_ymirjar_witch_doctorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShadowBoltTimer;
		uint32 ShrinkTimer;

        void Reset()
        {
			ShadowBoltTimer = 500;
			ShrinkTimer = 3000;

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

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_YWD_SHADOW_BOLT_5N, SPELL_YWD_SHADOW_BOLT_5H));
					ShadowBoltTimer = urand(3000, 4000);
				}
			}
			else ShadowBoltTimer -= diff;

			if (ShrinkTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHRINK_5N, SPELL_SHRINK_5H));
					ShrinkTimer = urand(5000, 6000);
				}
			}
			else ShrinkTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ymirjar_witch_doctorAI(creature);
    }
};

class npc_ymirjar_harpooner : public CreatureScript
{
public:
    npc_ymirjar_harpooner() : CreatureScript("npc_ymirjar_harpooner") {}

    struct npc_ymirjar_harpoonerAI : public QuantumBasicAI
    {
        npc_ymirjar_harpoonerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ThrowTimer;
		uint32 NetTimer;

        void Reset()
        {
			ThrowTimer = 500;
			NetTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
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

			if (ThrowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_THROW);
					ThrowTimer = urand(3000, 4000);
				}
			}
			else ThrowTimer -= diff;

			if (NetTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_NET);
					NetTimer = urand(5000, 6000);
				}
			}
			else NetTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ymirjar_harpoonerAI(creature);
    }
};

class npc_bloodthirsty_tundra_wolf : public CreatureScript
{
public:
    npc_bloodthirsty_tundra_wolf() : CreatureScript("npc_bloodthirsty_tundra_wolf") {}

    struct npc_bloodthirsty_tundra_wolfAI : public QuantumBasicAI
    {
        npc_bloodthirsty_tundra_wolfAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FixateTimer;

        void Reset()
        {
			FixateTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
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

			if (FixateTimer <= diff)
			{
				DoCast(me, SPELL_FIXATE);
				FixateTimer = urand(4000, 5000);
			}
			else FixateTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodthirsty_tundra_wolfAI(creature);
    }
};

void AddSC_utgarde_pinnacle_trash()
{
	new npc_scourge_hulk();
	new npc_dragonflayer_deathseeker();
	new npc_dragonflayer_fanatic();
	new npc_dragonflayer_seer();
	new npc_ymirjar_flesh_hunter();
	new npc_ymirjar_savage();
	new npc_ymirjar_berserker();
	new npc_ymirjar_necromancer();
	new npc_ymirjar_dusk_shaman();
	new npc_ymirjar_warrior();
	new npc_ymirjar_witch_doctor();
	new npc_ymirjar_harpooner();
	new npc_bloodthirsty_tundra_wolf();
}