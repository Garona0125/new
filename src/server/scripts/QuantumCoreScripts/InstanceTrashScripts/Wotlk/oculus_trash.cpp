/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2009-2014 QuantumCore
 *
 * Copyright (C) 2009-2014 Crispi Custom Scripts Special For TrinityCore
 *
 * Copyright (C) 2013-2014 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "../../../scripts/Northrend/Nexus/Oculus/oculus.h"

enum OculusSpells
{
	SPELL_ARCANE_BOLT_5N        = 50705,
	SPELL_ARCANE_BOLT_5H        = 59210,
	SPELL_THROW_5N              = 51454,
	SPELL_THROW_5H              = 59209,
	SPELL_ARCANE_CLEAVE         = 50573,
	SPELL_IMMOBILIZING_FIELD    = 50690,
	SPELL_ARCANE_VOLLEY_5N      = 50702,
	SPELL_ARCANE_VOLLEY_5H      = 59212,
	SPELL_MIND_WARP_5N          = 50566,
	SPELL_MIND_WARP_5H          = 38047,
	SPELL_POWER_SAP             = 50572,
	SPELL_ENERGY_LINK           = 50088,
	SPELL_VERTEX_BRIGHT_RED     = 69844,
	SPELL_EMPOWERING_BLOWS_5N   = 50044,
	SPELL_EMPOWERING_BLOWS_5H   = 59213,
	SPELL_NEXUS_ENERGY_COSMETIC = 50036,
	SPELL_CHARGED_SKIN_5N       = 50717,
	SPELL_CHARGED_SKIN_5H       = 59276,
	SPELL_CONJURE_SNOWFLAKE     = 50719,
	SPELL_ENCHANTMENT_HASTE     = 44604,
	SPELL_FROSTBOLT_5N          = 50721,
	SPELL_FROSTBOLT_5H          = 59280,
	SPELL_FLAMESTRIKE_5N        = 16102,
	SPELL_FLAMESTRIKE_5H        = 61402,
	SPELL_BLIZZARD_5N           = 50715,
	SPELL_BLIZZARD_5H           = 59278,
	SPELL_ICE_BEAM_5N           = 49549,
	SPELL_ICE_BEAM_5H           = 59211,
	SPELL_GLW_ARCANE_BOLT_5N    = 51243,
	SPELL_GLW_ARCANE_BOLT_5H    = 59215,
};

class npc_azure_ley_whelp : public CreatureScript
{
public:
    npc_azure_ley_whelp() : CreatureScript("npc_azure_ley_whelp") { }

    struct npc_azure_ley_whelpAI : public QuantumBasicAI
    {
        npc_azure_ley_whelpAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ArcaneBoltTimer;

        void Reset()
        {
            ArcaneBoltTimer = 500;

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

			if (ArcaneBoltTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_ARCANE_BOLT_5N, SPELL_ARCANE_BOLT_5H));
					ArcaneBoltTimer = urand(3000, 4000);
				}
            }
			else ArcaneBoltTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_azure_ley_whelpAI(creature);
    }
};

class npc_azure_spellbinder : public CreatureScript
{
public:
    npc_azure_spellbinder() : CreatureScript("npc_azure_spellbinder") { }

    struct npc_azure_spellbinderAI : public QuantumBasicAI
    {
        npc_azure_spellbinderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ArcaveVolleyTimer;
		uint32 MindWarpTimer;
		uint32 PowerSapTimer;

        void Reset()
        {
            ArcaveVolleyTimer = 500;
			MindWarpTimer = 2000;
			PowerSapTimer = 4000;

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

			if (ArcaveVolleyTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_ARCANE_VOLLEY_5N, SPELL_ARCANE_VOLLEY_5H), true);
					ArcaveVolleyTimer = urand(3000, 4000);
				}
            }
			else ArcaveVolleyTimer -= diff;

			if (MindWarpTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_MIND_WARP_5N, SPELL_MIND_WARP_5H), true);
					MindWarpTimer = urand(5000, 6000);
				}
            }
			else MindWarpTimer -= diff;

			if (PowerSapTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_POWER_SAP, true);
					PowerSapTimer = urand(7000, 8000);
				}
            }
			else PowerSapTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_azure_spellbinderAI(creature);
    }
};

class npc_azure_inquisitor : public CreatureScript
{
public:
    npc_azure_inquisitor() : CreatureScript("npc_azure_inquisitor") { }

    struct npc_azure_inquisitorAI : public QuantumBasicAI
    {
        npc_azure_inquisitorAI(Creature* creature) : QuantumBasicAI(creature)
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
		uint32 ArcaneCleaveTimer;
		uint32 ImmobilizingFieldTimer;

        void Reset()
        {
            ThrowTimer = 500;
			ArcaneCleaveTimer = 2000;
			ImmobilizingFieldTimer = 3000;

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
					DoCast(target, DUNGEON_MODE(SPELL_THROW_5N, SPELL_THROW_5H));
					ThrowTimer = urand(3000, 4000);
				}
            }
			else ThrowTimer -= diff;

			if (ArcaneCleaveTimer <= diff)
            {
				DoCastVictim(SPELL_ARCANE_CLEAVE);
				ArcaneCleaveTimer = urand(5000, 6000);
            }
			else ArcaneCleaveTimer -= diff;

			if (ImmobilizingFieldTimer <= diff)
            {
				DoCastAOE(SPELL_IMMOBILIZING_FIELD);
				ImmobilizingFieldTimer = urand(7000, 8000);
            }
			else ImmobilizingFieldTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_azure_inquisitorAI(creature);
    }
};

class npc_azure_ring_guardian : public CreatureScript
{
public:
    npc_azure_ring_guardian() : CreatureScript("npc_azure_ring_guardian") { }

    struct npc_azure_ring_guardianAI : public QuantumBasicAI
    {
        npc_azure_ring_guardianAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 IceBeamTimer;

        void Reset()
        {
            IceBeamTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (IceBeamTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_ICE_BEAM_5N, SPELL_ICE_BEAM_5H));
					IceBeamTimer = urand(3000, 4000);
				}
            }
			else IceBeamTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_azure_ring_guardianAI(creature);
    }
};

class npc_greater_ley_whelp : public CreatureScript
{
public:
    npc_greater_ley_whelp() : CreatureScript("npc_greater_ley_whelp") { }

    struct npc_greater_ley_whelpAI : public QuantumBasicAI
    {
        npc_greater_ley_whelpAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ArcaneBoltTimer;

        void Reset()
        {
            ArcaneBoltTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ArcaneBoltTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_GLW_ARCANE_BOLT_5N, SPELL_GLW_ARCANE_BOLT_5H));
					ArcaneBoltTimer = urand(3000, 4000);
				}
            }
			else ArcaneBoltTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_greater_ley_whelpAI(creature);
    }
};

class npc_centrifuge_construct : public CreatureScript
{
public:
    npc_centrifuge_construct() : CreatureScript("npc_centrifuge_construct") { }

    struct npc_centrifuge_constructAI : public QuantumBasicAI
    {
        npc_centrifuge_constructAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 EmpoweringBlowsTimer;

        void Reset()
        {
            EmpoweringBlowsTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_ENERGY_LINK, true);
			DoCast(me, SPELL_VERTEX_BRIGHT_RED, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_VERTEX_BRIGHT_RED);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (EmpoweringBlowsTimer <= diff)
            {
				DoCast(me, DUNGEON_MODE(SPELL_EMPOWERING_BLOWS_5N, SPELL_EMPOWERING_BLOWS_5H));
				EmpoweringBlowsTimer = 1500;
            }
			else EmpoweringBlowsTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_centrifuge_constructAI(creature);
    }
};

class npc_ring_lord_conjurer : public CreatureScript
{
public:
    npc_ring_lord_conjurer() : CreatureScript("npc_ring_lord_conjurer") {}

    struct npc_ring_lord_conjurerAI : public QuantumBasicAI
    {
        npc_ring_lord_conjurerAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ChargedSkinTimer;
		uint32 EnergyCosmeticTimer;
		uint32 ConjureSnowflakeTimer;

		SummonList Summons;

        void Reset()
		{
			ChargedSkinTimer = 100;
			EnergyCosmeticTimer = 500;
			ConjureSnowflakeTimer = 2000;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->InterruptSpell(CURRENT_CHANNELED_SPELL);
			me->CallForHelp(80.0f);
		}

		void JustReachedHome()
		{
			Reset();
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_SNOWFLAKE)
				Summons.Summon(summon);
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (ChargedSkinTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, DUNGEON_MODE(SPELL_CHARGED_SKIN_5N, SPELL_CHARGED_SKIN_5H), true);
				ChargedSkinTimer = 120000; // 2 Minutes
			}
			else ChargedSkinTimer -= diff;

			// Out of Combat Timer
			if (EnergyCosmeticTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* construct = me->FindCreatureWithDistance(NPC_CENTRIFUGE_CONSTRUCT, 125.0f))
				{
					me->SetFacingToObject(construct);
					DoCast(construct, SPELL_NEXUS_ENERGY_COSMETIC);
					EnergyCosmeticTimer = 240000; // 4 Minutes
				}
			}
			else EnergyCosmeticTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Charged Skin Buff Check in Combat
			if (!me->HasAura(DUNGEON_MODE(SPELL_CHARGED_SKIN_5N, SPELL_CHARGED_SKIN_5H)))
				DoCast(me, DUNGEON_MODE(SPELL_CHARGED_SKIN_5N, SPELL_CHARGED_SKIN_5H), true);

			if (ConjureSnowflakeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CONJURE_SNOWFLAKE);
					ConjureSnowflakeTimer = 3000;
				}
			}
			else ConjureSnowflakeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ring_lord_conjurerAI(creature);
    }
};

class npc_ring_lord_sorceress : public CreatureScript
{
public:
    npc_ring_lord_sorceress() : CreatureScript("npc_ring_lord_sorceress") {}

    struct npc_ring_lord_sorceressAI : public QuantumBasicAI
    {
        npc_ring_lord_sorceressAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 EnergyCosmeticTimer;
		uint32 FlamestrikeTimer;
		uint32 BlizzardTimer;

        void Reset()
		{
			EnergyCosmeticTimer = 100;
			FlamestrikeTimer = 500;
			BlizzardTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->InterruptSpell(CURRENT_CHANNELED_SPELL);
			me->CallForHelp(65.0f);
		}

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (EnergyCosmeticTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* construct = me->FindCreatureWithDistance(NPC_CENTRIFUGE_CONSTRUCT, 125.0f))
				{
					me->SetFacingToObject(construct);
					DoCast(construct, SPELL_NEXUS_ENERGY_COSMETIC);
					EnergyCosmeticTimer = 240000; // 4 Minutes
				}
			}
			else EnergyCosmeticTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FlamestrikeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FLAMESTRIKE_5N, SPELL_FLAMESTRIKE_5H));
					FlamestrikeTimer = urand(3000, 4000);
				}
			}
			else FlamestrikeTimer -= diff;

			if (BlizzardTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_BLIZZARD_5N, SPELL_BLIZZARD_5H));
					BlizzardTimer = urand(6000, 7000);
				}
			}
			else BlizzardTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ring_lord_sorceressAI(creature);
    }
};

class npc_snowflake : public CreatureScript
{
public:
    npc_snowflake() : CreatureScript("npc_snowflake") { }

    struct npc_snowflakeAI : public QuantumBasicAI
    {
        npc_snowflakeAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 FrostboltTimer;

        void Reset()
        {
            FrostboltTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_ENCHANTMENT_HASTE, true);
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
					FrostboltTimer = 2500;
				}
            }
			else FrostboltTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_snowflakeAI(creature);
    }
};

void AddSC_oculus_trash()
{
	new npc_azure_ley_whelp();
	new npc_azure_spellbinder();
	new npc_azure_inquisitor();
	new npc_azure_ring_guardian();
	new npc_greater_ley_whelp();
	new npc_centrifuge_construct();
	new npc_ring_lord_conjurer();
	new npc_ring_lord_sorceress();
	new npc_snowflake();
}