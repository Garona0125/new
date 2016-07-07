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
#include "../../../scripts/Outland/Auchindoun/ShadowLabyrinth/shadow_labyrinth.h"

enum ShadowLabyrinthSpells
{
	SPELL_ENRAGE                = 8599,
	SPELL_BLACK_CLEAVE_5N       = 33480,
	SPELL_BLACK_CLEAVE_5H       = 38226,
	SPELL_KNOCKDOWN             = 11428,
	SPELL_HEAL_5N               = 12039,
	SPELL_HEAL_5H               = 38209,
	SPELL_RENEW_5N              = 25058,
	SPELL_RENEW_5H              = 38210,
	SPELL_DEMON_ARMOR           = 13787,
	SPELL_SHADOW_BOLT_5N        = 12471,
	SPELL_SHADOW_BOLT_5H        = 15232,
	SPELL_SEED_OF_CORRUPTION_5N = 32863,
	SPELL_SEED_OF_CORRUPTION_5H = 38252,
	SPELL_THRASH                = 3417,
	SPELL_KICK                  = 15614,
	SPELL_CRYSTAL_CHANNEL       = 32958,
	SPELL_CONTAINMENT_BEAM      = 36220,
	SPELL_FROSTBOLT_5N          = 15497,
	SPELL_FROSTBOLT_5H          = 12675,
	SPELL_FROST_NOVA_5N         = 15532,
	SPELL_FROST_NOVA_5H         = 15063,
	SPELL_ARCANE_MISSILES_5N    = 33832,
	SPELL_ARCANE_MISSILES_5H    = 38263,
	SPELL_FIRE_BLAST_5N         = 20795,
	SPELL_FIRE_BLAST_5H         = 14145,
	SPELL_ADDLE_HUMANOID        = 33487,
	SPELL_SHADOWFORM            = 16592,
	SPELL_SHADOW_WORD_PAIN_5N   = 14032,
	SPELL_SHADOW_WORD_PAIN_5H   = 17146,
	SPELL_MIND_FLAY_5N          = 17165,
	SPELL_MIND_FLAY_5H          = 38243,
	SPELL_CHARGE                = 24023,
	SPELL_FIXATED_RAGE          = 40416,
	SPELL_CZ_SHADOW_BOLT_5N     = 12471,
	SPELL_CZ_SHADOW_BOLT_5H     = 15472,
	SPELL_CZ_ENRAGE             = 33958,
	SPELL_SHAPE_OF_THE_BEAST    = 33499,
	SPELL_FIREBALL_5N           = 14034,
	SPELL_FIREBALL_5H           = 15228,
	SPELL_CS_SHADOW_BOLT        = 33335,
	SPELL_SUMM_CABAL_DEATHSWORN = 33506,
	SPELL_SUMM_CABAL_ACOLYTE    = 33507,
	SPELL_CE_ENRAGE             = 30485,
	SPELL_WHIRLWIND             = 33500,
	SPELL_EXECUTE               = 7160,
	SPELL_WHITE_BEAM            = 40193,
	SPELL_BRAIN_WASH            = 33502,
	SPELL_SPELL_SHOCK           = 32691,
	SPELL_ARCANE_DESTRUCTION    = 32689,
	SPELL_STEALTH               = 30991,
	SPELL_CHEAP_SHOT            = 30986,
	SPELL_BACKSTAB              = 30992,
	SPELL_CRIPPLING_POISON      = 30981,
	SPELL_WOUND_POISON          = 36974,
	SPELL_FIREBOLT_5N           = 20801,
	SPELL_FIREBOLT_5H           = 38239,
	SPELL_MANA_BURN             = 13321,
	SPELL_SPELL_LOCK            = 30849,
	SPELL_MORTAL_STRIKE         = 16856,
	SPELL_FRIGHTENING_SHOUT     = 19134,
	SPELL_INTERCEPT             = 27577,
	SPELL_UPPERCUT              = 30471,
};

enum Texts
{
	SAY_ACOLYTE_AGGRO_1  = -1300059,
	SAY_ACOLYTE_AGGRO_2  = -1300060,
	SAY_ACOLYTE_AGGRO_3  = -1300061,
	SAY_ACOLYTE_AGGRO_4  = -1300062,
	SAY_ACOLYTE_DEATH_1  = -1300063,
	SAY_ACOLYTE_DEATH_2  = -1300064,

	SAY_OVERSEER_AGGRO_1 = -1300065,
	SAY_OVERSEER_AGGRO_2 = -1300066,
	SAY_OVERSEER_AGGRO_3 = -1300067,
	SAY_OVERSEER_AGGRO_4 = -1300068,
	SAY_OVERSEER_AGGRO_5 = -1300069,
	SAY_OVERSEER_AGGRO_6 = -1300070,
	SAY_OVERSEER_AGGRO_7 = -1300071,
	SAY_OVERSEER_AGGRO_8 = -1300072,
};

class npc_cabal_deathsworn : public CreatureScript
{
public:
    npc_cabal_deathsworn() : CreatureScript("npc_cabal_deathsworn") {}

    struct npc_cabal_deathswornAI : public QuantumBasicAI
    {
        npc_cabal_deathswornAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 BlackCleaveTimer;
		uint32 KnockdownTimer;

        void Reset()
		{
			BlackCleaveTimer = 2000;
			KnockdownTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->CallForHelp(10.0f);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_AGGRO_1, SAY_ACOLYTE_AGGRO_2, SAY_ACOLYTE_AGGRO_3, SAY_ACOLYTE_AGGRO_4), me);
		}

		void JustDied(Unit* killer)
		{
			if (killer->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_DEATH_1, SAY_ACOLYTE_DEATH_2), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BlackCleaveTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_BLACK_CLEAVE_5N, SPELL_BLACK_CLEAVE_5H));
				BlackCleaveTimer = 4000;
			}
			else BlackCleaveTimer -= diff;

			if (KnockdownTimer <= diff)
			{
				DoCastVictim(SPELL_KNOCKDOWN);
				KnockdownTimer = 6000;
			}
			else KnockdownTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_ENRAGE, 0))
				{
					DoCast(me, SPELL_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cabal_deathswornAI(creature);
    }
};

class npc_cabal_acolyte : public CreatureScript
{
public:
    npc_cabal_acolyte() : CreatureScript("npc_cabal_acolyte") {}

    struct npc_cabal_acolyteAI : public QuantumBasicAI
    {
        npc_cabal_acolyteAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 RenewTimer;
		uint32 HealTimer;

        void Reset()
		{
			RenewTimer = 500;
			HealTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->CallForHelp(10.0f);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_AGGRO_1, SAY_ACOLYTE_AGGRO_2, SAY_ACOLYTE_AGGRO_3, SAY_ACOLYTE_AGGRO_4), me);
		}

		void JustDied(Unit* killer)
		{
			if (killer->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_DEATH_1, SAY_ACOLYTE_DEATH_2), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RenewTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, DUNGEON_MODE(SPELL_RENEW_5N, SPELL_RENEW_5H));
					RenewTimer = urand(3000, 4000);
				}
			}
			else RenewTimer -= diff;

			if (HealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, DUNGEON_MODE(SPELL_HEAL_5N, SPELL_HEAL_5H));
					HealTimer = urand(5000, 6000);
				}
			}
			else HealTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cabal_acolyteAI(creature);
    }
};

class npc_cabal_warlock : public CreatureScript
{
public:
    npc_cabal_warlock() : CreatureScript("npc_cabal_warlock") {}

    struct npc_cabal_warlockAI : public QuantumBasicAI
    {
        npc_cabal_warlockAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 DemonArmorTimer;
		uint32 SeedOfCorruptionTimer;
		uint32 ShadowBoltTimer;

        void Reset()
		{
			DemonArmorTimer = 100;
			SeedOfCorruptionTimer = 500;
			ShadowBoltTimer = 1000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->CallForHelp(10.0f);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_AGGRO_1, SAY_ACOLYTE_AGGRO_2, SAY_ACOLYTE_AGGRO_3, SAY_ACOLYTE_AGGRO_4), me);
		}

		void JustDied(Unit* killer)
		{
			if (killer->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_DEATH_1, SAY_ACOLYTE_DEATH_2), me);
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (DemonArmorTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_DEMON_ARMOR);
				DemonArmorTimer = 120000; // 2 Minutes
			}
			else DemonArmorTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Demon Armor Buff Check in Combat
			if (!me->HasAura(SPELL_DEMON_ARMOR))
				DoCast(me, SPELL_DEMON_ARMOR, true);

			if (SeedOfCorruptionTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SEED_OF_CORRUPTION_5N, SPELL_SEED_OF_CORRUPTION_5H), true);
					SeedOfCorruptionTimer = urand(3000, 4000);
				}
			}
			else SeedOfCorruptionTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BOLT_5N, SPELL_SHADOW_BOLT_5H));
					ShadowBoltTimer = urand(5000, 6000);
				}
			}
			else ShadowBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cabal_warlockAI(creature);
    }
};

class npc_cabal_cultist : public CreatureScript
{
public:
    npc_cabal_cultist() : CreatureScript("npc_cabal_cultist") {}

    struct npc_cabal_cultistAI : public QuantumBasicAI
    {
        npc_cabal_cultistAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 KickTimer;

        void Reset()
		{
			KickTimer = 2000;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->CallForHelp(10.0f);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_AGGRO_1, SAY_ACOLYTE_AGGRO_2, SAY_ACOLYTE_AGGRO_3, SAY_ACOLYTE_AGGRO_4), me);

			DoCast(me, SPELL_THRASH);
		}

		void JustDied(Unit* killer)
		{
			if (killer->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_DEATH_1, SAY_ACOLYTE_DEATH_2), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (KickTimer <= diff)
			{
				DoCastVictim(SPELL_KICK);
				KickTimer = 4000;
			}
			else KickTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cabal_cultistAI(creature);
    }
};

class npc_cabal_ritualist : public CreatureScript
{
public:
    npc_cabal_ritualist() : CreatureScript("npc_cabal_ritualist") {}

    struct npc_cabal_ritualistAI : public QuantumBasicAI
    {
        npc_cabal_ritualistAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 CrystalChannelTimer;
		uint32 FrostboltTimer;
		uint32 ArcaneMissilesTimer;
		uint32 FireBlastTimer;
		uint32 AddleHumanoidTimer;
		uint32 FrostNovaTimer;

        void Reset()
		{
			CrystalChannelTimer = 100;
			FrostboltTimer = 500;
			ArcaneMissilesTimer = 1000;
			FireBlastTimer = 3000;
			AddleHumanoidTimer = 4000;
			FrostNovaTimer = 6000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->CallForHelp(10.0f);

			me->RemoveAurasDueToSpell(SPELL_CRYSTAL_CHANNEL);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_AGGRO_1, SAY_ACOLYTE_AGGRO_2, SAY_ACOLYTE_AGGRO_3, SAY_ACOLYTE_AGGRO_4), me);
		}

		void JustReachedHome()
		{
			Reset();
		}

		void JustDied(Unit* killer)
		{
			if (killer->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_DEATH_1, SAY_ACOLYTE_DEATH_2), me);
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (CrystalChannelTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* target = me->FindCreatureWithDistance(NPC_INVISIBLE_TARGET, 30.0f))
				{
					DoCast(target, SPELL_CRYSTAL_CHANNEL);
					CrystalChannelTimer = 120000; // 2 Minutes
				}
			}
			else CrystalChannelTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FROSTBOLT_5N, SPELL_FROSTBOLT_5H), true);
					FrostboltTimer = urand(3000, 4000);
				}
			}
			else FrostboltTimer -= diff;

			if (ArcaneMissilesTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_ARCANE_MISSILES_5N, SPELL_ARCANE_MISSILES_5H), true);
					ArcaneMissilesTimer = urand(5000, 6000);
				}
			}
			else ArcaneMissilesTimer -= diff;

			if (FireBlastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FIRE_BLAST_5N, SPELL_FIRE_BLAST_5H));
					FireBlastTimer = urand(7000, 8000);
				}
			}
			else FireBlastTimer -= diff;

			if (AddleHumanoidTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ADDLE_HUMANOID);
					AddleHumanoidTimer = urand(9000, 10000);
				}
			}
			else AddleHumanoidTimer -= diff;

			if (FrostNovaTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCastAOE(DUNGEON_MODE(SPELL_FROST_NOVA_5N, SPELL_FROST_NOVA_5H), true);
					FrostNovaTimer = urand(11000, 12000);
				}
			}
			else FrostNovaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cabal_ritualistAI(creature);
    }
};

class npc_cabal_shadow_priest : public CreatureScript
{
public:
    npc_cabal_shadow_priest() : CreatureScript("npc_cabal_shadow_priest") {}

    struct npc_cabal_shadow_priestAI : public QuantumBasicAI
    {
        npc_cabal_shadow_priestAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}
		
		uint32 ShadowWordPainTimer;
		uint32 MindFlayTimer;

        void Reset()
		{
			ShadowWordPainTimer = 500;
			MindFlayTimer = 1000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL, true);
			DoCast(me, SPELL_SHADOWFORM, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_KNEEL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_RANGED_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->CallForHelp(10.0f);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_AGGRO_1, SAY_ACOLYTE_AGGRO_2, SAY_ACOLYTE_AGGRO_3, SAY_ACOLYTE_AGGRO_4), me);
		}

		void JustDied(Unit* killer)
		{
			if (killer->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_DEATH_1, SAY_ACOLYTE_DEATH_2), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShadowWordPainTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_WORD_PAIN_5N, SPELL_SHADOW_WORD_PAIN_5H));
					ShadowWordPainTimer = urand(3000, 4000);
				}
			}
			else ShadowWordPainTimer -= diff;

			if (MindFlayTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_MIND_FLAY_5N, SPELL_MIND_FLAY_5H));
					MindFlayTimer = urand(5000, 6000);
				}
			}
			else MindFlayTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cabal_shadow_priestAI(creature);
    }
};

class npc_cabal_fanatic : public CreatureScript
{
public:
    npc_cabal_fanatic() : CreatureScript("npc_cabal_fanatic") {}

    struct npc_cabal_fanaticAI : public QuantumBasicAI
    {
        npc_cabal_fanaticAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ChargeTimer;;

        void Reset()
		{
			ChargeTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->CallForHelp(10.0f);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_AGGRO_1, SAY_ACOLYTE_AGGRO_2, SAY_ACOLYTE_AGGRO_3, SAY_ACOLYTE_AGGRO_4), me);
		}

		void JustDied(Unit* killer)
		{
			if (killer->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_DEATH_1, SAY_ACOLYTE_DEATH_2), me);
		}

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
					DoCast(target, SPELL_CHARGE);
					DoCast(me, SPELL_FIXATED_RAGE, true);
					ChargeTimer = 4000;
				}
			}
			else ChargeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cabal_fanaticAI(creature);
    }
};

class npc_cabal_zealot : public CreatureScript
{
public:
    npc_cabal_zealot() : CreatureScript("npc_cabal_zealot") {}

    struct npc_cabal_zealotAI : public QuantumBasicAI
    {
        npc_cabal_zealotAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}
		
		uint32 ShadowBoltTimer;
		uint32 EnrageTimer;

        void Reset()
		{
			ShadowBoltTimer = 500;
			EnrageTimer = 2500;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->CallForHelp(10.0f);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_AGGRO_1, SAY_ACOLYTE_AGGRO_2, SAY_ACOLYTE_AGGRO_3, SAY_ACOLYTE_AGGRO_4), me);
		}

		void JustDied(Unit* killer)
		{
			if (killer->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_DEATH_1, SAY_ACOLYTE_DEATH_2), me);
		}

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
					DoCast(target, DUNGEON_MODE(SPELL_CZ_SHADOW_BOLT_5N, SPELL_CZ_SHADOW_BOLT_5H));
					ShadowBoltTimer = urand(3000, 4000);
				}
			}
			else ShadowBoltTimer -= diff;

			if (EnrageTimer <= diff)
			{
				DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				DoCast(me, SPELL_CZ_ENRAGE);
				EnrageTimer = urand(9000, 10000);
			}
			else EnrageTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_30))
			{
				if (!me->HasAuraEffect(SPELL_SHAPE_OF_THE_BEAST, 0))
					DoCast(me, SPELL_SHAPE_OF_THE_BEAST);
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cabal_zealotAI(creature);
    }
};

class npc_cabal_summoner : public CreatureScript
{
public:
    npc_cabal_summoner() : CreatureScript("npc_cabal_summoner") {}

    struct npc_cabal_summonerAI : public QuantumBasicAI
    {
        npc_cabal_summonerAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FireballTimer;
		uint32 ShadowBoltTimer;
		uint32 SummonAcolyteTimer;

		SummonList Summons;

        void Reset()
		{
			FireballTimer = 500;
			ShadowBoltTimer = 1000;
			SummonAcolyteTimer = 4000;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* who)
		{
			me->CallForHelp(10.0f);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_AGGRO_1, SAY_ACOLYTE_AGGRO_2, SAY_ACOLYTE_AGGRO_3, SAY_ACOLYTE_AGGRO_4), me);
		}

		void JustDied(Unit* killer)
		{
			if (killer->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_DEATH_1, SAY_ACOLYTE_DEATH_2), me);

			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			switch (summon->GetEntry())
			{
                case NPC_SUMMONED_CABAL_ACOLYTE:
				case NPC_SUMMONED_CABAL_DEATHSWORN:
					Summons.Summon(summon);
					Summons.DoZoneInCombat();
					break;
				default:
					break;
			}
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
					DoCast(target, DUNGEON_MODE(SPELL_FIREBALL_5N, SPELL_FIREBALL_5H), true);
					FireballTimer = urand(2000, 3000);
				}
			}
			else FireballTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CS_SHADOW_BOLT);
					ShadowBoltTimer = urand(4000, 5000);
				}
			}
			else ShadowBoltTimer -= diff;

			if (SummonAcolyteTimer <= diff)
			{
				DoCast(me, SPELL_SUMM_CABAL_DEATHSWORN, true);
				DoCast(me, SPELL_SUMM_CABAL_ACOLYTE, true);
				SummonAcolyteTimer = 20000;
			}
			else SummonAcolyteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cabal_summonerAI(creature);
    }
};

class npc_cabal_executioner : public CreatureScript
{
public:
    npc_cabal_executioner() : CreatureScript("npc_cabal_executioner") {}

    struct npc_cabal_executionerAI : public QuantumBasicAI
    {
        npc_cabal_executionerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 WhirlwindTimer;
		uint32 ExecuteTimer;

        void Reset()
		{
			WhirlwindTimer = 2000;
			ExecuteTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->CallForHelp(10.0f);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_AGGRO_1, SAY_ACOLYTE_AGGRO_2, SAY_ACOLYTE_AGGRO_3, SAY_ACOLYTE_AGGRO_4), me);
		}

		void JustDied(Unit* killer)
		{
			if (killer->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_DEATH_1, SAY_ACOLYTE_DEATH_2), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WhirlwindTimer <= diff)
			{
				DoCastAOE(SPELL_WHIRLWIND);
				WhirlwindTimer = 4000;
			}
			else WhirlwindTimer -= diff;

			if (ExecuteTimer <= diff && me->GetVictim()->HealthBelowPct(HEALTH_PERCENT_20))
			{
				DoCastVictim(SPELL_EXECUTE);
				ExecuteTimer = 6000;
			}
			else ExecuteTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_CE_ENRAGE, 0))
				{
					DoCast(me, SPELL_CE_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cabal_executionerAI(creature);
    }
};

class npc_cabal_spellbinder : public CreatureScript
{
public:
    npc_cabal_spellbinder() : CreatureScript("npc_cabal_spellbinder") {}

    struct npc_cabal_spellbinderAI : public QuantumBasicAI
    {
        npc_cabal_spellbinderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 WhiteBeamTimer;
		uint32 ShadowBoltTimer;
		uint32 SpellShockTimer;
		uint32 BrainWashTimer;

        void Reset()
		{
			WhiteBeamTimer = 1000;
			ShadowBoltTimer = 1500;
			SpellShockTimer = 3000;
			BrainWashTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* who)
		{
			me->CallForHelp(10.0f);

			me->InterruptSpell(CURRENT_CHANNELED_SPELL);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_AGGRO_1, SAY_ACOLYTE_AGGRO_2, SAY_ACOLYTE_AGGRO_3, SAY_ACOLYTE_AGGRO_4), me);
		}

		void JustReachedHome()
		{
			Reset();
		}

		void JustDied(Unit* killer)
		{
			if (killer->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_DEATH_1, SAY_ACOLYTE_DEATH_2), me);
		}

        void UpdateAI(uint32 const diff)
        {
			// Arcane Destruction Buff Check
			if (!me->HasAura(SPELL_ARCANE_DESTRUCTION))
				DoCast(me, SPELL_ARCANE_DESTRUCTION, true);

			// Out of Combat Timer
			if (WhiteBeamTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* Murmur = me->FindCreatureWithDistance(NPC_MURMUR, 35.0f))
				{
					me->SetFacingToObject(Murmur);
					DoCast(Murmur, SPELL_WHITE_BEAM, true);
					WhiteBeamTimer = 120000; // 2 Minutes
				}
			}
			else WhiteBeamTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CS_SHADOW_BOLT, true);
					ShadowBoltTimer = urand(2000, 3000);
				}
			}
			else ShadowBoltTimer -= diff;

			if (SpellShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->HasUnitState(UNIT_STATE_CASTING))
					{
						DoCast(target, SPELL_CS_SHADOW_BOLT);
						SpellShockTimer = urand(4000, 5000);
					}
				}
			}
			else SpellShockTimer -= diff;

			if (BrainWashTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BRAIN_WASH);
					BrainWashTimer = urand(7000, 8000);
				}
			}
			else BrainWashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cabal_spellbinderAI(creature);
    }
};

class npc_cabal_assassin : public CreatureScript
{
public:
    npc_cabal_assassin() : CreatureScript("npc_cabal_assassin") {}

    struct npc_cabal_assassinAI : public QuantumBasicAI
    {
        npc_cabal_assassinAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 PoisonTimer;
		uint32 BackstabTimer;

        void Reset()
		{
			PoisonTimer = 2000;
			BackstabTimer = 3000;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_STEALTH);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			DoCast(who, SPELL_CHEAP_SHOT, true);

			me->CallForHelp(10.0f);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_AGGRO_1, SAY_ACOLYTE_AGGRO_2, SAY_ACOLYTE_AGGRO_3, SAY_ACOLYTE_AGGRO_4), me);
		}

		void JustDied(Unit* killer)
		{
			if (killer->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_ACOLYTE_DEATH_1, SAY_ACOLYTE_DEATH_2), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PoisonTimer <= diff)
			{
				DoCastVictim(SPELL_CRIPPLING_POISON, true);
				DoCastVictim(SPELL_WOUND_POISON, true);
				PoisonTimer = 4000;
			}
			else PoisonTimer -= diff;

			if (BackstabTimer <= diff)
			{
				DoCastVictim(SPELL_BACKSTAB);
				BackstabTimer = 6000;
			}
			else BackstabTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cabal_assassinAI(creature);
    }
};

class npc_cabal_familiar : public CreatureScript
{
public:
    npc_cabal_familiar() : CreatureScript("npc_cabal_familiar") {}

    struct npc_cabal_familiarAI : public QuantumBasicAI
    {
        npc_cabal_familiarAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}
		
		uint32 FireboltTimer;

        void Reset()
		{
			FireboltTimer = 500;

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

			if (FireboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FIREBOLT_5N, SPELL_FIREBOLT_5H));
					FireboltTimer = 2000;
				}
			}
			else FireboltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cabal_familiarAI(creature);
    }
};

class npc_fel_guardhound : public CreatureScript
{
public:
    npc_fel_guardhound() : CreatureScript("npc_fel_guardhound") {}

    struct npc_fel_guardhoundAI : public QuantumBasicAI
    {
        npc_fel_guardhoundAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ManaBurnTimer;
		uint32 SpellLockTimer;

        void Reset()
		{
			ManaBurnTimer = 500;
			SpellLockTimer = 2500;

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

			if (ManaBurnTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_MANA_BURN);
						ManaBurnTimer = urand(3000, 4000);
					}
				}
			}
			else ManaBurnTimer -= diff;

			if (SpellLockTimer < diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA && target->HasUnitState(UNIT_STATE_CASTING))
					{
						DoCast(target, SPELL_SPELL_LOCK);
						SpellLockTimer = urand(5000, 6000);
					}
				}
            }
			else SpellLockTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fel_guardhoundAI(creature);
    }
};

class npc_fel_overseer : public CreatureScript
{
public:
    npc_fel_overseer() : CreatureScript("npc_fel_overseer") {}

    struct npc_fel_overseerAI : public QuantumBasicAI
    {
        npc_fel_overseerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 MortalStrikeTimer;
		uint32 FrighteningShout;
		uint32 UppercutTimer;

        void Reset()
		{
			MortalStrikeTimer = 2000;
			FrighteningShout = 4000;
			UppercutTimer = 6000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			DoCast(who, SPELL_INTERCEPT, true);

			me->CallForHelp(10.0f);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_OVERSEER_AGGRO_1, SAY_OVERSEER_AGGRO_2, SAY_OVERSEER_AGGRO_3, SAY_OVERSEER_AGGRO_4,
				SAY_OVERSEER_AGGRO_5, SAY_OVERSEER_AGGRO_6, SAY_OVERSEER_AGGRO_7, SAY_OVERSEER_AGGRO_8), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_MORTAL_STRIKE);
				MortalStrikeTimer = 4000;
			}
			else MortalStrikeTimer -= diff;

			if (FrighteningShout <= diff)
			{
				DoCastAOE(SPELL_FRIGHTENING_SHOUT);
				FrighteningShout = 6000;
			}
			else FrighteningShout -= diff;

			if (UppercutTimer <= diff)
			{
				DoCastVictim(SPELL_UPPERCUT);
				UppercutTimer = 8000;
			}
			else UppercutTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_ENRAGE, 0))
				{
					DoCast(me, SPELL_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fel_overseerAI(creature);
    }
};

void AddSC_shadow_labyrinth_trash()
{
	new npc_cabal_deathsworn();
	new npc_cabal_acolyte();
	new npc_cabal_warlock();
	new npc_cabal_cultist();
	new npc_cabal_ritualist();
	new npc_cabal_shadow_priest();
	new npc_cabal_fanatic();
	new npc_cabal_zealot();
	new npc_cabal_summoner();
	new npc_cabal_executioner();
	new npc_cabal_spellbinder();
	new npc_cabal_assassin();
	new npc_cabal_familiar();
	new npc_fel_guardhound();
	new npc_fel_overseer();
}