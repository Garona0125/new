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
#include "../../../scripts/Northrend/Ulduar/HallsOfStone/halls_of_stone.h"

#define EMOTE_IRON_MIGHT "%s Begins into Iron Might!"
#define EMOTE_IRON_FIST "%s Begins into Iron Fist!"

enum HallsOfStoneSpells
{
	// Cosmetic Spells
	SPELL_BLUE_BEAM         = 46846,
	SPELL_HEROIC_STRIKE_5N       = 53395,
	SPELL_HEROIC_STRIKE_5H       = 59035,
	SPELL_CLEAVE                 = 42724,
	SPELL_LIGHTNING_BOLT_5N      = 15801,
	SPELL_LIGHTNING_BOLT_5H      = 35010,
	SPELL_FORK_LIGHTNING_5N      = 53167,
	SPELL_FORK_LIGHTNING_5H      = 59152,
	SPELL_IRON_MIGHT             = 51484,
	SPELL_IRON_FIST              = 51494,
	SPELL_STOMP_5N               = 51493,
	SPELL_STOMP_5H               = 59026,
	SPELL_ARCANE_HASTE           = 32693,
	SPELL_EL_LIGHT_BOLT_5N       = 53314,
	SPELL_EL_LIGHT_BOLT_5H       = 59024,
	SPELL_LIGHT_SHIELD_5N        = 51776,
	SPELL_LIGHT_SHIELD_5H        = 59025,
	SPELL_SUMMON_AIR_ELEM        = 51475,
	SPELL_ELEM_LIGHT_BOLT        = 15801,
	SPELL_CHISELING_RAY_5N       = 51496,
	SPELL_CHISELING_RAY_5H       = 59034,
	SPELL_ARMOR_PIERCE           = 46202,
	SPELL_DISGRUNTLED_ANGER      = 51499,
	SPELL_RUNIC_INTELLECT        = 51799,
	SPELL_STATIC_ARREST_5N       = 51612,
	SPELL_STATIC_ARREST_5H       = 59033,
	SPELL_CRYSTALINE_GROWTH      = 51805,
	SPELL_DOMINATION             = 51503,
	SPELL_SUMMON_SHARDLING       = 51507,
	SPELL_CHARGE_5N              = 51842,
	SPELL_CHARGE_5H              = 59040,
	SPELL_SHORT_CIRCUIT_5N       = 51832,
	SPELL_SHORT_CIRCUIT_5H       = 61513,
	SPELL_UNREL_STRIKE_5N        = 51491,
	SPELL_UNREL_STRIKE_5H        = 59039,
	SPELL_ARCING_SMASH           = 28168,
	SPELL_POTENT_JOLT_5N         = 51819,
	SPELL_POTENT_JOLT_5H         = 61514,
	SPELL_CHAIN_LIGHTNING_5N     = 52383,
	SPELL_CHAIN_LIGHTNING_5H     = 61528,
	SPELL_ELECTRICAL_OVERLOAD_5N = 52341,
	SPELL_ELECTRICAL_OVERLOAD_5H = 59038,
};

class npc_dark_rune_warrior : public CreatureScript
{
public:
    npc_dark_rune_warrior() : CreatureScript("npc_dark_rune_warrior") {}

    struct npc_dark_rune_warriorAI : public QuantumBasicAI
    {
        npc_dark_rune_warriorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CleaveTimer;
		uint32 HeroicStrikeTimer;

        void Reset()
        {
			CleaveTimer = 1000;
			HeroicStrikeTimer = 3000;

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

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(2000, 3000);
			}
			else CleaveTimer -= diff;

			if (HeroicStrikeTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_HEROIC_STRIKE_5N, SPELL_HEROIC_STRIKE_5H));
				HeroicStrikeTimer = urand(4000, 5000);
			}
			else HeroicStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_rune_warriorAI(creature);
    }
};

class npc_dark_rune_theurgist : public CreatureScript
{
public:
    npc_dark_rune_theurgist() : CreatureScript("npc_dark_rune_theurgist") {}

    struct npc_dark_rune_theurgistAI : public QuantumBasicAI
    {
        npc_dark_rune_theurgistAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 LightningBoltTimer;
		uint32 ForkedLightningTimer;

        void Reset()
        {
			LightningBoltTimer = 500;
			ForkedLightningTimer = 2000;

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

			if (LightningBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_LIGHTNING_BOLT_5N, SPELL_LIGHTNING_BOLT_5H));
					LightningBoltTimer = urand(3000, 4000);
				}
			}
			else LightningBoltTimer -= diff;

			if (ForkedLightningTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_FORK_LIGHTNING_5N, SPELL_FORK_LIGHTNING_5H));
				ForkedLightningTimer = urand(5000, 6000);
			}
			else ForkedLightningTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_IRON_MIGHT, 0))
				{
					me->MonsterTextEmote(EMOTE_IRON_MIGHT, LANG_UNIVERSAL, false);
					DoCast(me, SPELL_IRON_MIGHT);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_rune_theurgistAI(creature);
    }
};

class npc_dark_rune_giant : public CreatureScript
{
public:
    npc_dark_rune_giant() : CreatureScript("npc_dark_rune_giant") {}

    struct npc_dark_rune_giantAI : public QuantumBasicAI
    {
        npc_dark_rune_giantAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 StompTimer;

        void Reset()
        {
			StompTimer = 3000;

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

			if (StompTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_STOMP_5N, SPELL_STOMP_5H));
				StompTimer = urand(5000, 6000);
			}
			else StompTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_IRON_FIST, 0))
				{
					me->MonsterTextEmote(EMOTE_IRON_FIST, LANG_UNIVERSAL, false);
					DoCast(me, SPELL_IRON_FIST);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_rune_giantAI(creature);
    }
};

class npc_dark_rune_elementalist : public CreatureScript
{
public:
    npc_dark_rune_elementalist() : CreatureScript("npc_dark_rune_elementalist") {}

    struct npc_dark_rune_elementalistAI : public QuantumBasicAI
    {
        npc_dark_rune_elementalistAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ArcaneHasteTimer;
		uint32 LightningShieldTimer;
		uint32 LightningBoltTimer;
		SummonList Summons;

        void Reset()
        {
			ArcaneHasteTimer = 1000;
			LightningShieldTimer = 2000;
			LightningBoltTimer = 3500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_LESSER_AIR_ELEMENTAL)
				Summons.Summon(summon);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SUMMON_AIR_ELEM, true);
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

			if (ArcaneHasteTimer <= diff)
			{
				DoCast(me, SPELL_ARCANE_HASTE);
				ArcaneHasteTimer = urand(2000, 3000);
			}
			else ArcaneHasteTimer -= diff;

			if (LightningShieldTimer <= diff)
			{
				DoCast(me, DUNGEON_MODE(SPELL_LIGHT_SHIELD_5N, SPELL_LIGHT_SHIELD_5H));
				LightningShieldTimer = urand(4000, 5000);
			}
			else LightningShieldTimer -= diff;

			if (LightningBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_EL_LIGHT_BOLT_5N, SPELL_EL_LIGHT_BOLT_5H));
					LightningBoltTimer = urand(5000, 6000);
				}
			}
			else LightningBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_rune_elementalistAI(creature);
    }
};

class npc_lesser_air_elemental : public CreatureScript
{
public:
    npc_lesser_air_elemental() : CreatureScript("npc_lesser_air_elemental") {}

    struct npc_lesser_air_elementalAI : public QuantumBasicAI
    {
        npc_lesser_air_elementalAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 LightningBoltTimer;

        void Reset()
        {
			LightningBoltTimer = 2000;

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

			if (LightningBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ELEM_LIGHT_BOLT);
					LightningBoltTimer = urand(3000, 4000);
				}
			}
			else LightningBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lesser_air_elementalAI(creature);
    }
};

class npc_dark_rune_shaper : public CreatureScript
{
public:
    npc_dark_rune_shaper() : CreatureScript("npc_dark_rune_shaper") {}

    struct npc_dark_rune_shaperAI : public QuantumBasicAI
    {
        npc_dark_rune_shaperAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 BlueBeamTimer;
		uint32 ChiselingRayBoltTimer;

        void Reset()
        {
			BlueBeamTimer = 100;
			ChiselingRayBoltTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

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
			if (BlueBeamTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* channel = me->FindCreatureWithDistance(NPC_CHANNEL_TARGET, 30.0f, true))
				{
					me->SetFacingToObject(channel);
					DoCast(channel, SPELL_BLUE_BEAM, true);
					BlueBeamTimer = 2*MINUTE*IN_MILLISECONDS;
				}
			}
			else BlueBeamTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ChiselingRayBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_CHISELING_RAY_5N, SPELL_CHISELING_RAY_5H));
					ChiselingRayBoltTimer = urand(3000, 5000);
				}
			}
			else ChiselingRayBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_rune_shaperAI(creature);
    }
};

class npc_dark_rune_worker : public CreatureScript
{
public:
    npc_dark_rune_worker() : CreatureScript("npc_dark_rune_worker") {}

    struct npc_dark_rune_workerAI : public QuantumBasicAI
    {
        npc_dark_rune_workerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ArmorPierceTimer;

        void Reset()
        {
			ArmorPierceTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_WORK_MINING);
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

			if (ArmorPierceTimer <= diff)
			{
				DoCastVictim(SPELL_ARMOR_PIERCE);
				ArmorPierceTimer = urand(4000, 5000);
			}
			else ArmorPierceTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_DISGRUNTLED_ANGER, 0))
					DoCast(me, SPELL_DISGRUNTLED_ANGER);
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_rune_workerAI(creature);
    }
};

class npc_dark_rune_scholar : public CreatureScript
{
public:
    npc_dark_rune_scholar() : CreatureScript("npc_dark_rune_scholar") {}

    struct npc_dark_rune_scholarAI : public QuantumBasicAI
    {
        npc_dark_rune_scholarAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 StaticArrestTimer;

        void Reset()
        {
			StaticArrestTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_RUNIC_INTELLECT);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StaticArrestTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_STATIC_ARREST_5N, SPELL_STATIC_ARREST_5H));
				StaticArrestTimer = urand(3000, 4000);
			}
			else StaticArrestTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_rune_scholarAI(creature);
    }
};

class npc_dark_rune_controller : public CreatureScript
{
public:
    npc_dark_rune_controller() : CreatureScript("npc_dark_rune_controller") {}

    struct npc_dark_rune_controllerAI : public QuantumBasicAI
    {
        npc_dark_rune_controllerAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CrystalineGrowthTimer;
		uint32 SummonShardlingTimer;
		uint32 DominationTimer;

		SummonList Summons;

        void Reset()
        {
			Summons.DespawnAll();

			CrystalineGrowthTimer = 2000;
			SummonShardlingTimer = 3000;
			DominationTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_ERODED_SHARDLING)
				Summons.Summon(summon);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CrystalineGrowthTimer <= diff)
			{
				DoCastAOE(SPELL_CRYSTALINE_GROWTH);
				CrystalineGrowthTimer = urand(3000, 4000);
			}
			else CrystalineGrowthTimer -= diff;

			if (SummonShardlingTimer <= diff)
			{
				DoCastAOE(SPELL_SUMMON_SHARDLING);
				SummonShardlingTimer = urand(4000, 5000);
			}
			else SummonShardlingTimer -= diff;

			if (DominationTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
					DoCast(target, SPELL_DOMINATION);
					DominationTimer = urand(5000, 6000);
				}
			}
			else DominationTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_rune_controllerAI(creature);
    }
};

class npc_unrelenting_construct : public CreatureScript
{
public:
    npc_unrelenting_construct() : CreatureScript("npc_unrelenting_construct") {}

    struct npc_unrelenting_constructAI : public QuantumBasicAI
    {
        npc_unrelenting_constructAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 UnrelentingStrikeTimer;

        void Reset()
        {
			UnrelentingStrikeTimer = 1000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			DoCastVictim(DUNGEON_MODE(SPELL_CHARGE_5N, SPELL_CHARGE_5H));
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (UnrelentingStrikeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_UNREL_STRIKE_5N, SPELL_UNREL_STRIKE_5H));
					UnrelentingStrikeTimer = urand(5000, 6000);
				}
			}
			else UnrelentingStrikeTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_30))
				DoCastAOE(DUNGEON_MODE(SPELL_SHORT_CIRCUIT_5N, SPELL_SHORT_CIRCUIT_5H));

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unrelenting_constructAI(creature);
    }
};

class npc_raging_construct : public CreatureScript
{
public:
    npc_raging_construct() : CreatureScript("npc_raging_construct") {}

    struct npc_raging_constructAI : public QuantumBasicAI
    {
        npc_raging_constructAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ArcingSmashTimer;

        void Reset()
        {
			ArcingSmashTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

		void DoSeparationCheck()
		{
			if (me->HasAura(DUNGEON_MODE(SPELL_POTENT_JOLT_5N, SPELL_POTENT_JOLT_5H)))
				return;

			DoCast(me, DUNGEON_MODE(SPELL_POTENT_JOLT_5N, SPELL_POTENT_JOLT_5H));
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			DoSeparationCheck();

			if (ArcingSmashTimer <= diff)
			{
				DoCastVictim(SPELL_ARCING_SMASH);
				ArcingSmashTimer = urand(4000, 5000);
			}
			else ArcingSmashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_raging_constructAI(creature);
    }
};

class npc_lightning_construct : public CreatureScript
{
public:
    npc_lightning_construct() : CreatureScript("npc_lightning_construct") {}

    struct npc_lightning_constructAI : public QuantumBasicAI
    {
        npc_lightning_constructAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ChainLightningTimer;
		uint32 ElectricalOverloadTimer;

        void Reset()
        {
			ChainLightningTimer = 2000;
			ElectricalOverloadTimer = 6000;

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

			if (ChainLightningTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_CHAIN_LIGHTNING_5N, SPELL_CHAIN_LIGHTNING_5H));
				ChainLightningTimer = urand(4000, 5000);
			}
			else ChainLightningTimer -= diff;

			if (ChainLightningTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_ELECTRICAL_OVERLOAD_5N, SPELL_ELECTRICAL_OVERLOAD_5H));
				ChainLightningTimer = urand(7000, 8000);
			}
			else ChainLightningTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lightning_constructAI(creature);
    }
};

void AddSC_halls_of_stone_trash()
{
	new npc_dark_rune_warrior();
	new npc_dark_rune_theurgist();
	new npc_dark_rune_giant();
	new npc_dark_rune_elementalist();
	new npc_lesser_air_elemental();
	new npc_dark_rune_shaper();
	new npc_dark_rune_worker();
	new npc_dark_rune_scholar();
	new npc_dark_rune_controller();
	new npc_unrelenting_construct();
	new npc_raging_construct();
	new npc_lightning_construct();
}