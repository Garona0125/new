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
#include "Vehicle.h"
#include "../../../scripts/Northrend/FrozenHalls/PitOfSaron/pit_of_saron.h"

enum PitOfSaronSpells
{
	SPELL_FROST_CHANNEL      = 45846,
	SPELL_CONVERSION_BEAM_5N = 69578,
	SPELL_CONVERSION_BEAM_5H = 70269,
	SPELL_SHADOW_BOLT_5N     = 69577,
	SPELL_SHADOW_BOLT_5H     = 70270,
	SPELL_BLINDING_DIRT      = 70302,
    SPELL_PUNCTURE_WOUND_5N  = 70278,
	SPELL_PUNCTURE_WOUND_5H  = 70279,
    SPELL_SHOVELLED_5N       = 69572,
	SPELL_SHOVELLED_5H       = 70280,
	SPELL_DS_SHADOW_BOLT_5N  = 70386,
	SPELL_DS_SHADOW_BOLT_5H  = 70387,
	SPELL_SEED_OF_CORRUPTION = 70388,
	SPELL_CURSE_OF_AGONY_5N  = 70391,
	SPELL_CURSE_OF_AGONY_5H  = 71112,
	SPELL_BLACK_BRAND        = 70392,
	SPELL_DEVOUR_FLESH       = 70393,
	SPELL_FROST_BREATH_5N    = 69527,
	SPELL_FROST_BREATH_5H    = 70272,
	SPELL_FROSTBLADE_5N      = 70291,
	SPELL_FROSTBLADE_5H      = 71325,
	SPELL_GLACIAL_STRIKE_5N  = 70292,
	SPELL_GLACIAL_STRIKE_5H  = 71316,
	SPELL_GARGOYLE_STRIKE_5N = 69520,
	SPELL_GARGOYLE_STRIKE_5H = 70275,
	SPELL_STONEFORM          = 69575,
	SPELL_BLIGHT_BOMB_5N     = 69582,
	SPELL_BLIGHT_BOMB_5H     = 71088,
	SPELL_PUSTULANT_FLESH_5N = 69581,
	SPELL_PUSTULANT_FLESH_5H = 70273,
	SPELL_TOXIC_WASTE_5N     = 69024,
	SPELL_TOXIC_WASTE_5H     = 70274,
	SPELL_DEVOUR_HUMANOID    = 69503,
	SPELL_TACTICAL_BLINK     = 69584,
	SPELL_FIREBALL_5N        = 69583,
	SPELL_FIREBALL_5H        = 70282,
	SPELL_HELLFIRE_5N        = 69586,
	SPELL_HELLFIRE_5H        = 70283,
	SPELL_BLIGHT_5N          = 69603,
	SPELL_BLIGHT_5H          = 70285,
	SPELL_EMP_SHADOWBOLT_5N  = 69528,
	SPELL_EMP_SHADOWBOLT_5H  = 70281,
	SPELL_SUMMON_UNDEAD      = 69516, // From DBC (SPELL_EFFECT_DUMMY) Need More Research For Script (Doesn`t Work)
	SPELL_ARCING_SLICE       = 69579,
	SPELL_DEMORALIZING_SHOUT = 69565,
	SPELL_SHIELD_BLOCK       = 69580,
	SPELL_FROSTBOLT_5N       = 69573,
	SPELL_FROSTBOLT_5H       = 70277,
	SPELL_FREEZING_CIRCLE_5N = 69574,
	SPELL_FREEZING_CIRCLE_5H = 70276,
	SPELL_WS_SHADOW_BOLT     = 75330,
	SPELL_AVALANCHE          = 55216,
	SPELL_SUMMON_VISUAL      = 30540, // Need scripting spell for interrupt casts in move
	SPELL_CLEAVE             = 59992,
	SPELL_HEROIC_STRIKE      = 69566,
	SPELL_FIREBALL           = 69570,
	SPELL_PYROBLAST          = 70516,
	SPELL_BLIZZARD           = 70421,
	SPELL_ICE_BLOCK          = 46604,
	SPELL_CHAIN_HEAL         = 70425,
	SPELL_EARTH_SHIELD       = 69569,
	SPELL_HEALING_TOTEM      = 70517,
};

#define EMOTE_STONEFORM "%s into begins to Stoneform"
#define EMOTE_BLIGHT_BOMB "%s begins to cast Blight Bomb!"

class npc_deathwhisper_necrolyte : public CreatureScript
{
public:
    npc_deathwhisper_necrolyte() : CreatureScript("npc_deathwhisper_necrolyte") {}

    struct npc_deathwhisper_necrolyteAI : public QuantumBasicAI
    {
        npc_deathwhisper_necrolyteAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShadowBoltTimer;
		uint32 ConversionBeamTimer;

        void Reset()
		{
			ShadowBoltTimer = 0.5*IN_MILLISECONDS;
			ConversionBeamTimer = 3*IN_MILLISECONDS;

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
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BOLT_5N, SPELL_SHADOW_BOLT_5H));
					ShadowBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			if (ConversionBeamTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_CONVERSION_BEAM_5N, SPELL_CONVERSION_BEAM_5H));
					ConversionBeamTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ConversionBeamTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_deathwhisper_necrolyteAI(creature);
    }
};

class npc_wrathbone_laborer : public CreatureScript
{
public:
	npc_wrathbone_laborer() : CreatureScript("npc_wrathbone_laborer") { }

	struct npc_wrathbone_laborerAI: public QuantumBasicAI
	{
		npc_wrathbone_laborerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 BlindingDirtTimer;
		uint32 PunctureWoundTimer;
		uint32 ShovelledTimer;

		void Reset()
		{
			BlindingDirtTimer = 1*IN_MILLISECONDS;
			PunctureWoundTimer = 3*IN_MILLISECONDS;
			ShovelledTimer = 5*IN_MILLISECONDS;

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

			if (BlindingDirtTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BLINDING_DIRT);
					BlindingDirtTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else BlindingDirtTimer -= diff;

			if (PunctureWoundTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_PUNCTURE_WOUND_5N, SPELL_PUNCTURE_WOUND_5H));
				PunctureWoundTimer = urand(6*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else PunctureWoundTimer -= diff;

			if (ShovelledTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHOVELLED_5N, SPELL_SHOVELLED_5H));
					ShovelledTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else ShovelledTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_wrathbone_laborerAI(creature);
	}
};

class npc_deathwhisper_shadowcaster : public CreatureScript
{
public:
    npc_deathwhisper_shadowcaster() : CreatureScript("npc_deathwhisper_shadowcaster") {}

    struct npc_deathwhisper_shadowcasterAI : public QuantumBasicAI
    {
        npc_deathwhisper_shadowcasterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShadowBoltTimer;
		uint32 SeedOfCorruptionTimer;

        void Reset()
		{
			ShadowBoltTimer = 1*IN_MILLISECONDS;
			SeedOfCorruptionTimer = 3*IN_MILLISECONDS;

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
					DoCast(target, DUNGEON_MODE(SPELL_DS_SHADOW_BOLT_5N, SPELL_DS_SHADOW_BOLT_5H));
					ShadowBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			if (SeedOfCorruptionTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SEED_OF_CORRUPTION);
					SeedOfCorruptionTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else SeedOfCorruptionTimer -= diff;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_deathwhisper_shadowcasterAI(creature);
    }
};

class npc_deathwhisper_torturer : public CreatureScript
{
public:
    npc_deathwhisper_torturer() : CreatureScript("npc_deathwhisper_torturer") {}

    struct npc_deathwhisper_torturerAI : public QuantumBasicAI
    {
        npc_deathwhisper_torturerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CurseOfAgonyTimer;
		uint32 BlackBrandTimer;

        void Reset()
		{
			CurseOfAgonyTimer = 1*IN_MILLISECONDS;
			BlackBrandTimer = 2*IN_MILLISECONDS;

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

			if (CurseOfAgonyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_CURSE_OF_AGONY_5N, SPELL_CURSE_OF_AGONY_5H));
					CurseOfAgonyTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else CurseOfAgonyTimer -= diff;

			if (BlackBrandTimer <= diff)
			{
				DoCastVictim(SPELL_BLACK_BRAND);
				BlackBrandTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else BlackBrandTimer -= diff;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_deathwhisper_torturerAI(creature);
    }
};

class npc_hungering_ghoul : public CreatureScript
{
public:
    npc_hungering_ghoul() : CreatureScript("npc_hungering_ghoul") {}

    struct npc_hungering_ghoulAI : public QuantumBasicAI
    {
        npc_hungering_ghoulAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 DevourFleshTimer;

        void Reset()
		{
			DevourFleshTimer = 1*IN_MILLISECONDS;

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

			if (DevourFleshTimer <= diff)
			{
				DoCastVictim(SPELL_DEVOUR_FLESH);
				DevourFleshTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else DevourFleshTimer -= diff;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hungering_ghoulAI(creature);
    }
};

class npc_iceborn_protodrake : public CreatureScript
{
public:
	npc_iceborn_protodrake() : CreatureScript("npc_iceborn_protodrake") { }

	struct npc_iceborn_protodrakeAI: public QuantumBasicAI
	{
		npc_iceborn_protodrakeAI(Creature* creature) : QuantumBasicAI(creature), vehicle(creature->GetVehicleKit())
		{
			ASSERT(vehicle);

			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		Vehicle* vehicle;
		uint32 FrostBreathTimer;

		void Reset()
		{
			FrostBreathTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			vehicle->RemoveAllPassengers();
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (FrostBreathTimer < diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_FROST_BREATH_5N, SPELL_FROST_BREATH_5H));
				FrostBreathTimer = 8*IN_MILLISECONDS;
			}
			else FrostBreathTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_iceborn_protodrakeAI(creature);
	}
};

class npc_ymirjar_skycaller : public CreatureScript
{
public:
    npc_ymirjar_skycaller() : CreatureScript("npc_ymirjar_skycaller") {}

    struct npc_ymirjar_skycallerAI : public QuantumBasicAI
    {
        npc_ymirjar_skycallerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FrostbladeTimer;
		uint32 GlacialStrikeTimer;

        void Reset()
		{
			FrostbladeTimer = 1*IN_MILLISECONDS;
			GlacialStrikeTimer = 3*IN_MILLISECONDS;

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

			if (FrostbladeTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_FROSTBLADE_5N, SPELL_FROSTBLADE_5H));
				FrostbladeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else FrostbladeTimer -= diff;

			if (GlacialStrikeTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_GLACIAL_STRIKE_5N, SPELL_GLACIAL_STRIKE_5H));
				GlacialStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else GlacialStrikeTimer -= diff;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ymirjar_skycallerAI(creature);
    }
};

class npc_stonespine_gargoyle : public CreatureScript
{
public:
    npc_stonespine_gargoyle() : CreatureScript("npc_stonespine_gargoyle") {}

    struct npc_stonespine_gargoyleAI : public QuantumBasicAI
    {
        npc_stonespine_gargoyleAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 GargoyleStrikeTimer;

        void Reset()
		{
			GargoyleStrikeTimer = 1*IN_MILLISECONDS;

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

			if (GargoyleStrikeTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_GARGOYLE_STRIKE_5N, SPELL_GARGOYLE_STRIKE_5H));
				GargoyleStrikeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else GargoyleStrikeTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_30))
			{
				if (!me->HasAuraEffect(SPELL_STONEFORM, 0))
				{
					me->MonsterTextEmote(EMOTE_STONEFORM, LANG_UNIVERSAL, true);
					DoCast(me, SPELL_STONEFORM);
				}
			}

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stonespine_gargoyleAI(creature);
    }
};

class npc_pit_plagueborn_horror : public CreatureScript
{
public:
    npc_pit_plagueborn_horror() : CreatureScript("npc_pit_plagueborn_horror") {}

    struct npc_pit_plagueborn_horrorAI : public QuantumBasicAI
    {
        npc_pit_plagueborn_horrorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 PustulantFleshTimer;
		uint32 ToxicWasteTimer;

        void Reset()
		{
			PustulantFleshTimer = 1*IN_MILLISECONDS;
			ToxicWasteTimer = 3*IN_MILLISECONDS;

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

			if (PustulantFleshTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_PUSTULANT_FLESH_5N, SPELL_PUSTULANT_FLESH_5H));
					PustulantFleshTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else PustulantFleshTimer -= diff;

			if (ToxicWasteTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_TOXIC_WASTE_5N, SPELL_TOXIC_WASTE_5H));
					ToxicWasteTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ToxicWasteTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_20))
			{
				me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
				me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SILENCE, true);
				me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
				me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
				me->MonsterTextEmote(EMOTE_BLIGHT_BOMB, LANG_UNIVERSAL, true);
				DoCast(DUNGEON_MODE(SPELL_BLIGHT_BOMB_5N, SPELL_BLIGHT_BOMB_5H));
			}

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_pit_plagueborn_horrorAI(creature);
    }
};

class npc_geist_ambusher : public CreatureScript
{
public:
    npc_geist_ambusher() : CreatureScript("npc_geist_ambusher") {}

    struct npc_geist_ambusherAI : public QuantumBasicAI
    {
        npc_geist_ambusherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_DEVOUR_HUMANOID);
		}

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_geist_ambusherAI(creature);
    }
};

class npc_ymirjar_flamebearer : public CreatureScript
{
public:
	npc_ymirjar_flamebearer() : CreatureScript("npc_ymirjar_flamebearer") {}

	struct npc_ymirjar_flamebearerAI: public QuantumBasicAI
	{
		npc_ymirjar_flamebearerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FireballTimer;
		uint32 TacticalBlinkTimer;

		void Reset()
		{
			FireballTimer = 1*IN_MILLISECONDS;
			TacticalBlinkTimer = 9*IN_MILLISECONDS;

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

			if (FireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FIREBALL_5N, SPELL_FIREBALL_5H));
					FireballTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FireballTimer -= diff;

			if (TacticalBlinkTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					DoCast(target, SPELL_TACTICAL_BLINK, true);

				DoCastAOE(DUNGEON_MODE(SPELL_HELLFIRE_5N, SPELL_HELLFIRE_5H), true);
				TacticalBlinkTimer = urand(11*IN_MILLISECONDS, 12*IN_MILLISECONDS);
			}
			else TacticalBlinkTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ymirjar_flamebearerAI(creature);
	}
};

class npc_ymirjar_wrathbringer : public CreatureScript
{
public:
	npc_ymirjar_wrathbringer() : CreatureScript("npc_ymirjar_wrathbringer") {}

	struct npc_ymirjar_wrathbringerAI: public QuantumBasicAI
	{
		npc_ymirjar_wrathbringerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 BlightTimer;

		void Reset()
		{
			BlightTimer = 1*IN_MILLISECONDS;

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

			if (BlightTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_BLIGHT_5N, SPELL_BLIGHT_5H));
					BlightTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else BlightTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ymirjar_wrathbringerAI(creature);
	}
};

class npc_pit_ymirjar_deathbringer : public CreatureScript
{
public:
	npc_pit_ymirjar_deathbringer() : CreatureScript("npc_pit_ymirjar_deathbringer") {}

	struct npc_pit_ymirjar_deathbringerAI: public QuantumBasicAI
	{
		npc_pit_ymirjar_deathbringerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 EmpoweredShadowboltTimer;

		void Reset()
		{
			EmpoweredShadowboltTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(SPELL_SUMMON_UNDEAD);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (EmpoweredShadowboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_EMP_SHADOWBOLT_5N, SPELL_EMP_SHADOWBOLT_5H));
					EmpoweredShadowboltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else EmpoweredShadowboltTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_pit_ymirjar_deathbringerAI(creature);
	}
};

class npc_fallen_warrior : public CreatureScript
{
public:
	npc_fallen_warrior() : CreatureScript("npc_fallen_warrior") {}

	struct npc_fallen_warriorAI: public QuantumBasicAI
	{
		npc_fallen_warriorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ArcingSliceTimer;
		uint32 DemoralizingShoutTimer;
		uint32 ShieldBlockTimer;

		void Reset()
		{
			ArcingSliceTimer = 1*IN_MILLISECONDS;
			DemoralizingShoutTimer = 2*IN_MILLISECONDS;
			ShieldBlockTimer = 3*IN_MILLISECONDS;

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

			if (ArcingSliceTimer <= diff)
			{
				DoCastVictim(SPELL_ARCING_SLICE);
				ArcingSliceTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else ArcingSliceTimer -= diff;

			if (DemoralizingShoutTimer <= diff)
			{
				DoCastAOE(SPELL_DEMORALIZING_SHOUT);
				DemoralizingShoutTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else DemoralizingShoutTimer -= diff;

			if (ShieldBlockTimer <= diff)
			{
				DoCast(me, SPELL_SHIELD_BLOCK);
				ShieldBlockTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else ShieldBlockTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_fallen_warriorAI(creature);
	}
};

class npc_wrathbone_coldwraith : public CreatureScript
{
public:
	npc_wrathbone_coldwraith() : CreatureScript("npc_wrathbone_coldwraith") {}

	struct npc_wrathbone_coldwraithAI: public QuantumBasicAI
	{
		npc_wrathbone_coldwraithAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FrostboltTimer;
		uint32 FreezingCircleTimer;

		void Reset()
		{
			FrostboltTimer = 0.5*IN_MILLISECONDS;
			FreezingCircleTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_FROST_CHANNEL, true);
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

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

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FROSTBOLT_5N, SPELL_FROSTBOLT_5H));
					FrostboltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			if (FreezingCircleTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FREEZING_CIRCLE_5N, SPELL_FREEZING_CIRCLE_5H));
					FreezingCircleTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else FreezingCircleTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_wrathbone_coldwraithAI(creature);
	}
};

class npc_wrathbone_sorcerer : public CreatureScript
{
public:
    npc_wrathbone_sorcerer() : CreatureScript("npc_wrathbone_sorcerer") {}

    struct npc_wrathbone_sorcererAI : public QuantumBasicAI
    {
        npc_wrathbone_sorcererAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShadowBoltTimer;
		uint32 SeedOfCorruptionTimer;

        void Reset()
		{
			ShadowBoltTimer = 0.5*IN_MILLISECONDS;
			SeedOfCorruptionTimer = 3*IN_MILLISECONDS;

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
					DoCast(target, DUNGEON_MODE(SPELL_DS_SHADOW_BOLT_5N, SPELL_DS_SHADOW_BOLT_5H));
					ShadowBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			if (SeedOfCorruptionTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SEED_OF_CORRUPTION);
					SeedOfCorruptionTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else SeedOfCorruptionTimer -= diff;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wrathbone_sorcererAI(creature);
    }
};

class npc_disturbed_glacial_revenant : public CreatureScript
{
public:
    npc_disturbed_glacial_revenant() : CreatureScript("npc_disturbed_glacial_revenant") {}

    struct npc_disturbed_glacial_revenantAI : public QuantumBasicAI
    {
        npc_disturbed_glacial_revenantAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 AvalancheTimer;

        void Reset()
		{
			AvalancheTimer = 0.5*IN_MILLISECONDS;

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

			if (AvalancheTimer <= diff)
			{
				DoCastVictim(SPELL_AVALANCHE);
				AvalancheTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else AvalancheTimer -= diff;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_disturbed_glacial_revenantAI(creature);
    }
};

class npc_gorkun_ironskull_pos : public CreatureScript
{
public:
    npc_gorkun_ironskull_pos() : CreatureScript("npc_gorkun_ironskull_pos") {}

    struct npc_gorkun_ironskull_posAI : public QuantumBasicAI
    {
        npc_gorkun_ironskull_posAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CleaveTimer;
		uint32 HeroicStrikeTimer;
		uint32 DemoralizingShoutTimer;

        void Reset()
		{
			CleaveTimer = 1*IN_MILLISECONDS;
			HeroicStrikeTimer = 2*IN_MILLISECONDS;
			DemoralizingShoutTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (who->GetEntry() == NPC_TYRANNUS)
				return;
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->CallForHelp(150.0f);
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
				CleaveTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (HeroicStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_HEROIC_STRIKE);
				HeroicStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else HeroicStrikeTimer -= diff;

			if (DemoralizingShoutTimer <= diff)
			{
				DoCastAOE(SPELL_DEMORALIZING_SHOUT);
				DemoralizingShoutTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else DemoralizingShoutTimer -= diff;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gorkun_ironskull_posAI(creature);
    }
};

class npc_martin_victus_pos : public CreatureScript
{
public:
    npc_martin_victus_pos() : CreatureScript("npc_martin_victus_pos") {}

    struct npc_martin_victus_posAI : public QuantumBasicAI
    {
        npc_martin_victus_posAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CleaveTimer;
		uint32 HeroicStrikeTimer;
		uint32 DemoralizingShoutTimer;

        void Reset()
		{
			CleaveTimer = 1*IN_MILLISECONDS;
			HeroicStrikeTimer = 2*IN_MILLISECONDS;
			DemoralizingShoutTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (who->GetEntry() == NPC_TYRANNUS)
				return;
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->CallForHelp(150.0f);
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
				CleaveTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (HeroicStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_HEROIC_STRIKE);
				HeroicStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else HeroicStrikeTimer -= diff;

			if (DemoralizingShoutTimer <= diff)
			{
				DoCastAOE(SPELL_DEMORALIZING_SHOUT);
				DemoralizingShoutTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else DemoralizingShoutTimer -= diff;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_martin_victus_posAI(creature);
    }
};

class npc_freed_horde_slave_1 : public CreatureScript
{
public:
    npc_freed_horde_slave_1() : CreatureScript("npc_freed_horde_slave_1") {}

    struct npc_freed_horde_slave_1AI : public QuantumBasicAI
    {
        npc_freed_horde_slave_1AI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FireballTimer;
		uint32 PyroblastTimer;
		uint32 BlizzardTimer;

        void Reset()
		{
			FireballTimer = 0.5*IN_MILLISECONDS;
			PyroblastTimer = 3*IN_MILLISECONDS;
			BlizzardTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (who->GetEntry() == NPC_TYRANNUS)
				return;
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->CallForHelp(150.0f);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FIREBALL, true);
					FireballTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FireballTimer -= diff;

			if (PyroblastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_PYROBLAST, true);
					PyroblastTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else PyroblastTimer -= diff;

			if (BlizzardTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BLIZZARD, true);
					BlizzardTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else BlizzardTimer -= diff;

			if (me->HealthBelowPct(HEALTH_PERCENT_30))
			{
				if (!me->HasAuraEffect(SPELL_ICE_BLOCK, 0))
					DoCast(me, SPELL_ICE_BLOCK, true);
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_freed_horde_slave_1AI(creature);
    }
};

class npc_freed_horde_slave_2 : public CreatureScript
{
public:
    npc_freed_horde_slave_2() : CreatureScript("npc_freed_horde_slave_2") {}

    struct npc_freed_horde_slave_2AI : public QuantumBasicAI
    {
        npc_freed_horde_slave_2AI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 EarthShieldTimer;
		uint32 ChainHealTimer;
		uint32 HealingStreamTotemTimer;

        void Reset()
		{
			EarthShieldTimer = 0.5*IN_MILLISECONDS;
			ChainHealTimer = 3*IN_MILLISECONDS;
			HealingStreamTotemTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_EARTH_SHIELD);
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (who->GetEntry() == NPC_TYRANNUS)
				return;
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->CallForHelp(150.0f);
		}

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
					DoCast(target, SPELL_EARTH_SHIELD, true);
					EarthShieldTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else EarthShieldTimer -= diff;

			if (ChainHealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, SPELL_CHAIN_HEAL, true);
					ChainHealTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ChainHealTimer -= diff;

			if (HealingStreamTotemTimer <= diff)
			{
				DoCast(me, SPELL_HEALING_TOTEM, true);
				HealingStreamTotemTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else HealingStreamTotemTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_freed_horde_slave_2AI(creature);
    }
};

class npc_freed_horde_slave_3 : public CreatureScript
{
public:
    npc_freed_horde_slave_3() : CreatureScript("npc_freed_horde_slave_3") {}

    struct npc_freed_horde_slave_3AI : public QuantumBasicAI
    {
        npc_freed_horde_slave_3AI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CleaveTimer;
		uint32 HeroicStrikeTimer;
		uint32 DemoralizingShoutTimer;

        void Reset()
		{
			CleaveTimer = 1*IN_MILLISECONDS;
			HeroicStrikeTimer = 2*IN_MILLISECONDS;
			DemoralizingShoutTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (who->GetEntry() == NPC_TYRANNUS)
				return;
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->CallForHelp(150.0f);
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
				CleaveTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (HeroicStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_HEROIC_STRIKE);
				HeroicStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else HeroicStrikeTimer -= diff;

			if (DemoralizingShoutTimer <= diff)
			{
				DoCastAOE(SPELL_DEMORALIZING_SHOUT);
				DemoralizingShoutTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else DemoralizingShoutTimer -= diff;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_freed_horde_slave_3AI(creature);
    }
};

class npc_freed_alliance_slave_1 : public CreatureScript
{
public:
    npc_freed_alliance_slave_1() : CreatureScript("npc_freed_alliance_slave_1") {}

    struct npc_freed_alliance_slave_1AI : public QuantumBasicAI
    {
        npc_freed_alliance_slave_1AI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FireballTimer;
		uint32 PyroblastTimer;
		uint32 BlizzardTimer;

        void Reset()
		{
			FireballTimer = 0.5*IN_MILLISECONDS;
			PyroblastTimer = 3*IN_MILLISECONDS;
			BlizzardTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (who->GetEntry() == NPC_TYRANNUS)
				return;
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->CallForHelp(150.0f);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FIREBALL, true);
					FireballTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FireballTimer -= diff;

			if (PyroblastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_PYROBLAST, true);
					PyroblastTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else PyroblastTimer -= diff;

			if (BlizzardTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BLIZZARD, true);
					BlizzardTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else BlizzardTimer -= diff;

			if (me->HealthBelowPct(HEALTH_PERCENT_30))
			{
				if (!me->HasAuraEffect(SPELL_ICE_BLOCK, 0))
					DoCast(me, SPELL_ICE_BLOCK, true);
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_freed_alliance_slave_1AI(creature);
    }
};

class npc_freed_alliance_slave_2 : public CreatureScript
{
public:
    npc_freed_alliance_slave_2() : CreatureScript("npc_freed_alliance_slave_2") {}

    struct npc_freed_alliance_slave_2AI : public QuantumBasicAI
    {
        npc_freed_alliance_slave_2AI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 EarthShieldTimer;
		uint32 ChainHealTimer;
		uint32 HealingStreamTotemTimer;

        void Reset()
		{
			EarthShieldTimer = 0.5*IN_MILLISECONDS;
			ChainHealTimer = 3*IN_MILLISECONDS;
			HealingStreamTotemTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_EARTH_SHIELD);
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (who->GetEntry() == NPC_TYRANNUS)
				return;
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->CallForHelp(150.0f);
		}

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
					DoCast(target, SPELL_EARTH_SHIELD, true);
					EarthShieldTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else EarthShieldTimer -= diff;

			if (ChainHealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, SPELL_CHAIN_HEAL, true);
					ChainHealTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ChainHealTimer -= diff;

			if (HealingStreamTotemTimer <= diff)
			{
				DoCast(me, SPELL_HEALING_TOTEM, true);
				HealingStreamTotemTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else HealingStreamTotemTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_freed_alliance_slave_2AI(creature);
    }
};

class npc_freed_alliance_slave_3 : public CreatureScript
{
public:
    npc_freed_alliance_slave_3() : CreatureScript("npc_freed_alliance_slave_3") {}

    struct npc_freed_alliance_slave_3AI : public QuantumBasicAI
    {
        npc_freed_alliance_slave_3AI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CleaveTimer;
		uint32 HeroicStrikeTimer;
		uint32 DemoralizingShoutTimer;

        void Reset()
		{
			CleaveTimer = 1*IN_MILLISECONDS;
			HeroicStrikeTimer = 2*IN_MILLISECONDS;
			DemoralizingShoutTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (who->GetEntry() == NPC_TYRANNUS)
				return;
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->CallForHelp(150.0f);
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
				CleaveTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (HeroicStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_HEROIC_STRIKE);
				HeroicStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else HeroicStrikeTimer -= diff;

			if (DemoralizingShoutTimer <= diff)
			{
				DoCastAOE(SPELL_DEMORALIZING_SHOUT);
				DemoralizingShoutTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else DemoralizingShoutTimer -= diff;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_freed_alliance_slave_3AI(creature);
    }
};

void AddSC_pit_of_saron_trash()
{
	new npc_deathwhisper_necrolyte();
	new npc_wrathbone_laborer();
	new npc_deathwhisper_shadowcaster();
	new npc_deathwhisper_torturer();
	new npc_hungering_ghoul();
	new npc_iceborn_protodrake();
	new npc_ymirjar_skycaller();
	new npc_stonespine_gargoyle();
	new npc_pit_plagueborn_horror();
	new npc_geist_ambusher();
	new npc_ymirjar_flamebearer();
	new npc_ymirjar_wrathbringer();
	new npc_pit_ymirjar_deathbringer();
	new npc_fallen_warrior();
	new npc_wrathbone_coldwraith();
	new npc_wrathbone_sorcerer();
	new npc_disturbed_glacial_revenant();
	new npc_gorkun_ironskull_pos();
	new npc_martin_victus_pos();
	new npc_freed_horde_slave_1();
	new npc_freed_horde_slave_2();
	new npc_freed_horde_slave_3();
	new npc_freed_alliance_slave_1();
	new npc_freed_alliance_slave_2();
	new npc_freed_alliance_slave_3();
}