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

enum UnderbogSpells
{
	SPELL_DUAL_WIELD           = 674,
	SPELL_ENRAGE               = 8599,
	SPELL_TENTACLE_LASH_5N     = 34171,
	SPELL_TENTACLE_LASH_5H     = 37956,
	SPELL_WILD_GROWTH          = 34161,
	SPELL_ALLERGIES            = 31427,
	SPELL_ITCHY_SPORES_5N      = 32329,
	SPELL_ITCHY_SPORES_5H      = 37965,
	SPELL_FUNGAL_REGROWTH_5N   = 34163,
	SPELL_FUNGAL_REGROWTH_5H   = 37967,
	SPELL_FUNGAL_DECAY_5N      = 32065,
	SPELL_FUNGAL_DECAY_5H      = 34158,
	SPELL_TRAMPLE              = 15550,
	SPELL_GROWTH               = 40318,
	SPELL_DISEASE_CLOUD_5N     = 37266,
	SPELL_DISEASE_CLOUD_5H     = 37863,
	SPELL_CORAL_CUT_5N         = 31410,
	SPELL_CORAL_CUT_5H         = 37973,
	SPELL_STRIKE               = 11976,
	SPELL_SHIELD_BASH          = 11972,
	SPELL_MT_STRIKE            = 12057,
	SPELL_THROW_5N             = 22887,
	SPELL_THROW_5H             = 40317,
	SPELL_VIPER_STING_5N       = 31407,
	SPELL_VIPER_STING_5H       = 39413,
	SPELL_RENEW_5N             = 34423,
	SPELL_RENEW_5H             = 37978,
	SPELL_HOLY_LIGHT_5N        = 29427,
	SPELL_HOLY_LIGHT_5H        = 37979,
	SPELL_PRAYER_OF_HEALING_5N = 15585,
	SPELL_PRAYER_OF_HEALING_5H = 35943,
	SPELL_FROSTBOLT_5N         = 15497,
	SPELL_FROSTBOLT_5H         = 12675,
	SPELL_FIREBALL_5N          = 14034,
	SPELL_FIREBALL_5H          = 15228,
	SPELL_SHADOW_BOLT_5N       = 12471,
	SPELL_SHADOW_BOLT_5H       = 15232,
	SPELL_AMPLIFY_DAMAGE_5N    = 12248,
	SPELL_AMPLIFY_DAMAGE_5H    = 12738,
	SPELL_FROST_NOVA_5N        = 32192,
	SPELL_FROST_NOVA_5H        = 15531,
	SPELL_CORRUPTION           = 31405,
	SPELL_SCORCH               = 37113,
	SPELL_ELEMENTAL_ARMOR      = 34880,
	SPELL_PSYCHIC_SCREAM       = 34984,
	SPELL_STINGER_RAGE         = 34392,
	SPELL_POISON_SPIT          = 32330,
};

enum Texts
{
	SAY_NAGA_AGGRO_1 = -1420036,
	SAY_NAGA_AGGRO_2 = -1420037,
	SAY_NAGA_AGGRO_3 = -1420038,
	SAY_NAGA_AGGRO_4 = -1420039,
	SAY_NAGA_AGGRO_5 = -1420040,
};

class npc_underbat : public CreatureScript
{
public:
    npc_underbat() : CreatureScript("npc_underbat") {}

    struct npc_underbatAI : public QuantumBasicAI
    {
        npc_underbatAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 TentacleLashTimer;

        void Reset()
		{
			TentacleLashTimer = 500;

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

			if (TentacleLashTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_TENTACLE_LASH_5N, SPELL_TENTACLE_LASH_5H));
				TentacleLashTimer = 4000;
			}
			else TentacleLashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_underbatAI(creature);
    }
};

class npc_underbog_lurker : public CreatureScript
{
public:
    npc_underbog_lurker() : CreatureScript("npc_underbog_lurker") {}

    struct npc_underbog_lurkerAI : public QuantumBasicAI
    {
        npc_underbog_lurkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

        void Reset()
		{
			me->RemoveAurasDueToSpell(SPELL_WILD_GROWTH);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_WILD_GROWTH);
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
        return new npc_underbog_lurkerAI(creature);
    }
};

class npc_underbog_shambler : public CreatureScript
{
public:
    npc_underbog_shambler() : CreatureScript("npc_underbog_shambler") {}

    struct npc_underbog_shamblerAI : public QuantumBasicAI
    {
        npc_underbog_shamblerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 AllergiesTimer;
		uint32 ItchySporesTimer;
		uint32 FungalRegrowthTimer;

        void Reset()
		{
			AllergiesTimer = 1000;
			ItchySporesTimer = 2000;
			FungalRegrowthTimer = 4000;

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

			if (AllergiesTimer <= diff)
			{
				DoCastVictim(SPELL_ALLERGIES);
				AllergiesTimer = urand(3000, 4000);
			}
			else AllergiesTimer -= diff;

			if (ItchySporesTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_ITCHY_SPORES_5N, SPELL_ITCHY_SPORES_5H), true);
					ItchySporesTimer = urand(5000, 6000);
				}
			}
			else ItchySporesTimer -= diff;

			if (FungalRegrowthTimer <= diff)
			{
				DoCast(me, DUNGEON_MODE(SPELL_FUNGAL_REGROWTH_5N, SPELL_FUNGAL_REGROWTH_5H));
				FungalRegrowthTimer = urand(7000, 8000);
			}
			else FungalRegrowthTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_underbog_shamblerAI(creature);
    }
};

class npc_underbog_lord : public CreatureScript
{
public:
    npc_underbog_lord() : CreatureScript("npc_underbog_lord") {}

    struct npc_underbog_lordAI : public QuantumBasicAI
    {
        npc_underbog_lordAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 GrowthTimer;
		uint32 FungalDecayTimer;
		uint32 TrampleTimer;

        void Reset()
		{
			GrowthTimer = 500;
			FungalDecayTimer = 2000;
			TrampleTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, DUNGEON_MODE(SPELL_DISEASE_CLOUD_5N, SPELL_DISEASE_CLOUD_5H));
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GrowthTimer <= diff && IsHeroic())
			{
				DoCast(me, SPELL_GROWTH);
				GrowthTimer = urand(3000, 4000);
			}
			else GrowthTimer -= diff;

			if (FungalDecayTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_FUNGAL_DECAY_5N, SPELL_FUNGAL_DECAY_5H));
				FungalDecayTimer = urand(5000, 6000);
			}
			else FungalDecayTimer -= diff;

			if (TrampleTimer <= diff)
			{
				DoCastAOE(SPELL_TRAMPLE);
				TrampleTimer = urand(7000, 8000);
			}
			else TrampleTimer -= diff;

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
        return new npc_underbog_lordAI(creature);
    }
};

class npc_bog_giant : public CreatureScript
{
public:
    npc_bog_giant() : CreatureScript("npc_bog_giant") {}

    struct npc_bog_giantAI : public QuantumBasicAI
    {
        npc_bog_giantAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 GrowthTimer;
		uint32 FungalDecayTimer;
		uint32 TrampleTimer;

        void Reset()
		{
			GrowthTimer = 500;
			FungalDecayTimer = 2000;
			TrampleTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, DUNGEON_MODE(SPELL_DISEASE_CLOUD_5N, SPELL_DISEASE_CLOUD_5H));
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GrowthTimer <= diff && IsHeroic())
			{
				DoCast(me, SPELL_GROWTH);
				GrowthTimer = urand(3000, 4000);
			}
			else GrowthTimer -= diff;

			if (FungalDecayTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_FUNGAL_DECAY_5N, SPELL_FUNGAL_DECAY_5H));
				FungalDecayTimer = urand(5000, 6000);
			}
			else FungalDecayTimer -= diff;

			if (TrampleTimer <= diff)
			{
				DoCastAOE(SPELL_TRAMPLE);
				TrampleTimer = urand(7000, 8000);
			}
			else TrampleTimer -= diff;

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
        return new npc_bog_giantAI(creature);
    }
};

class npc_wrathfin_myrmidon : public CreatureScript
{
public:
    npc_wrathfin_myrmidon() : CreatureScript("npc_wrathfin_myrmidon") {}

    struct npc_wrathfin_myrmidonAI : public QuantumBasicAI
    {
        npc_wrathfin_myrmidonAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 CoralCutTimer;

        void Reset()
		{
			CoralCutTimer = 1000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_NAGA_AGGRO_1, SAY_NAGA_AGGRO_2, SAY_NAGA_AGGRO_3, SAY_NAGA_AGGRO_4, SAY_NAGA_AGGRO_5), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CoralCutTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_CORAL_CUT_5N, SPELL_CORAL_CUT_5H));
				CoralCutTimer = 5000;
			}
			else CoralCutTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wrathfin_myrmidonAI(creature);
    }
};

class npc_wrathfin_sentry : public CreatureScript
{
public:
    npc_wrathfin_sentry() : CreatureScript("npc_wrathfin_sentry") {}

    struct npc_wrathfin_sentryAI : public QuantumBasicAI
    {
        npc_wrathfin_sentryAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 StrikeTimer;
		uint32 ShieldBashTimer;

        void Reset()
		{
			StrikeTimer = 1000;
			ShieldBashTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_NAGA_AGGRO_1, SAY_NAGA_AGGRO_2, SAY_NAGA_AGGRO_3, SAY_NAGA_AGGRO_4, SAY_NAGA_AGGRO_5), me);
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

			if (ShieldBashTimer <= diff)
			{
				DoCastVictim(SPELL_SHIELD_BASH);
				ShieldBashTimer = 6000;
			}
			else ShieldBashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wrathfin_sentryAI(creature);
    }
};

class npc_wrathfin_warrior : public CreatureScript
{
public:
    npc_wrathfin_warrior() : CreatureScript("npc_wrathfin_warrior") {}

    struct npc_wrathfin_warriorAI : public QuantumBasicAI
    {
        npc_wrathfin_warriorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 StrikeTimer;
		uint32 ShieldBashTimer;

        void Reset()
		{
			StrikeTimer = 1000;
			ShieldBashTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_NAGA_AGGRO_1, SAY_NAGA_AGGRO_2, SAY_NAGA_AGGRO_3, SAY_NAGA_AGGRO_4, SAY_NAGA_AGGRO_5), me);
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

			if (ShieldBashTimer <= diff)
			{
				DoCastVictim(SPELL_SHIELD_BASH);
				ShieldBashTimer = 6000;
			}
			else ShieldBashTimer -= diff;

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
        return new npc_wrathfin_warriorAI(creature);
    }
};

class npc_murkblood_tribesman : public CreatureScript
{
public:
    npc_murkblood_tribesman() : CreatureScript("npc_murkblood_tribesman") {}

    struct npc_murkblood_tribesmanAI : public QuantumBasicAI
    {
        npc_murkblood_tribesmanAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 StrikeTimer;

        void Reset()
		{
			StrikeTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_DUAL_WIELD);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_MT_STRIKE);
				StrikeTimer = 4000;
			}
			else StrikeTimer -= diff;

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
        return new npc_murkblood_tribesmanAI(creature);
    }
};

class npc_murkblood_spearman : public CreatureScript
{
public:
    npc_murkblood_spearman() : CreatureScript("npc_murkblood_spearman") {}

    struct npc_murkblood_spearmanAI : public QuantumBasicAI
    {
        npc_murkblood_spearmanAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ThrowTimer;
		uint32 ViperStingTimer;

        void Reset()
        {
			ThrowTimer = 500;
			ViperStingTimer = 1000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
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

			if (ThrowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_THROW_5N, SPELL_THROW_5H));
					ThrowTimer = urand(3000, 4000);
				}
			}
			else ThrowTimer -= diff;

			if (ViperStingTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, DUNGEON_MODE(SPELL_VIPER_STING_5N, SPELL_VIPER_STING_5H));
						ViperStingTimer = urand(5000, 6000);
					}
				}
			}
			else ViperStingTimer -= diff;

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

				if (ThrowTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, DUNGEON_MODE(SPELL_THROW_5N, SPELL_THROW_5H));
						ThrowTimer = urand(7000, 8000);
					}
				}
				else ThrowTimer -= diff;
			}

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_ENRAGE, 0))
				{
					DoCast(me, SPELL_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_murkblood_spearmanAI(creature);
    }
};

class npc_murkblood_healer : public CreatureScript
{
public:
    npc_murkblood_healer() : CreatureScript("npc_murkblood_healer") {}

    struct npc_murkblood_healerAI : public QuantumBasicAI
    {
        npc_murkblood_healerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 RenewTimer;
		uint32 HolyLightTimer;
		uint32 PrayerOfHealingTimer;

        void Reset()
		{
			RenewTimer = 500;
			HolyLightTimer = 2000;
			PrayerOfHealingTimer = 4000;

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

			if (RenewTimer <= diff)
			{
					if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(me, DUNGEON_MODE(SPELL_RENEW_5N, SPELL_RENEW_5H), true);
					DoCast(target, DUNGEON_MODE(SPELL_RENEW_5N, SPELL_RENEW_5H), true);
					RenewTimer = urand(3000, 4000);
				}
			}
			else RenewTimer -= diff;

			if (HolyLightTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, DUNGEON_MODE(SPELL_HOLY_LIGHT_5N, SPELL_HOLY_LIGHT_5H), true);
					HolyLightTimer = urand(6000, 7000);
				}
			}
			else HolyLightTimer -= diff;

			if (PrayerOfHealingTimer <= diff)
			{
				DoCast(me, DUNGEON_MODE(SPELL_PRAYER_OF_HEALING_5N, SPELL_PRAYER_OF_HEALING_5H));
				PrayerOfHealingTimer = urand(9000, 10000);
			}
			else PrayerOfHealingTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_60))
				DoCast(me, DUNGEON_MODE(SPELL_HOLY_LIGHT_5N, SPELL_HOLY_LIGHT_5H), true);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_murkblood_healerAI(creature);
    }
};

class npc_murkblood_oracle : public CreatureScript
{
public:
    npc_murkblood_oracle() : CreatureScript("npc_murkblood_oracle") {}

    struct npc_murkblood_oracleAI : public QuantumBasicAI
    {
        npc_murkblood_oracleAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ElementalArmorTimer;
		uint32 FrostboltTimer;
		uint32 FireballTimer;
		uint32 ShadowBoltTimer;
		uint32 AmplifyDamageTimer;
		uint32 FrostNovaTimer;
		uint32 RandomSpellTimer;

        void Reset()
		{
			ElementalArmorTimer = 100;
			FrostboltTimer = 500;
			FireballTimer = 2000;
			ShadowBoltTimer = 4000;
			AmplifyDamageTimer = 6000;
			FrostNovaTimer = 8000;
			RandomSpellTimer = 10000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (ElementalArmorTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_ELEMENTAL_ARMOR);
				ElementalArmorTimer = 120000; // 2 Minutes
			}
			else ElementalArmorTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Elemental Armor Buff Check in Combat
			if (!me->HasAura(SPELL_ELEMENTAL_ARMOR))
				DoCast(me, SPELL_ELEMENTAL_ARMOR, true);

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FROSTBOLT_5N, SPELL_FROSTBOLT_5H), true);
					FrostboltTimer = urand(3000, 4000);
				}
			}
			else FrostboltTimer -= diff;

			if (FireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FIREBALL_5N, SPELL_FIREBALL_5H), true);
					FireballTimer = urand(5000, 6000);
				}
			}
			else FireballTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BOLT_5N, SPELL_SHADOW_BOLT_5H), true);
					ShadowBoltTimer = urand(7000, 8000);
				}
			}
			else ShadowBoltTimer -= diff;

			if (AmplifyDamageTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_AMPLIFY_DAMAGE_5N, SPELL_AMPLIFY_DAMAGE_5H));
					AmplifyDamageTimer = urand(9000, 10000);
				}
			}
			else AmplifyDamageTimer -= diff;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_FROST_NOVA_5N, SPELL_FROST_NOVA_5H));
				FrostNovaTimer = urand(11000, 12000);
			}
			else FrostNovaTimer -= diff;

			if (RandomSpellTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAND(SPELL_CORRUPTION, SPELL_SCORCH));
					RandomSpellTimer = urand(13000, 14000);
				}
			}
			else RandomSpellTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_murkblood_oracleAI(creature);
    }
};

class npc_lykul_stinger : public CreatureScript
{
public:
    npc_lykul_stinger() : CreatureScript("npc_lykul_stinger") {}

    struct npc_lykul_stingerAI : public QuantumBasicAI
    {
        npc_lykul_stingerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 StingerRageTimer;

        void Reset()
		{
			StingerRageTimer = 500;

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

			if (StingerRageTimer <= diff)
			{
				DoCast(me, SPELL_STINGER_RAGE);
				DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				StingerRageTimer = 8000;
			}
			else StingerRageTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lykul_stingerAI(creature);
    }
};

class npc_lykul_wasp : public CreatureScript
{
public:
    npc_lykul_wasp() : CreatureScript("npc_lykul_wasp") {}

    struct npc_lykul_waspAI : public QuantumBasicAI
    {
        npc_lykul_waspAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 PoisonSpitTimer;

        void Reset()
		{
			PoisonSpitTimer = 500;

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

			if (PoisonSpitTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_POISON_SPIT);
					PoisonSpitTimer = urand(3000, 4000);
				}
			}
			else PoisonSpitTimer -= diff;

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
        return new npc_lykul_waspAI(creature);
    }
};

class npc_fen_ray : public CreatureScript
{
public:
    npc_fen_ray() : CreatureScript("npc_fen_ray") {}

    struct npc_fen_rayAI : public QuantumBasicAI
    {
        npc_fen_rayAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 PsychicScreamTimer;

        void Reset()
		{
			PsychicScreamTimer = 500;

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

			if (PsychicScreamTimer <= diff)
			{
				DoCastVictim(SPELL_PSYCHIC_SCREAM);
				PsychicScreamTimer = 4000;
			}
			else PsychicScreamTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fen_rayAI(creature);
    }
};

void AddSC_underbog_trash()
{
	new npc_underbat();
	new npc_underbog_lurker();
	new npc_underbog_shambler();
	new npc_underbog_lord();
	new npc_bog_giant();
	new npc_wrathfin_myrmidon();
	new npc_wrathfin_sentry();
	new npc_wrathfin_warrior();
	new npc_murkblood_tribesman();
	new npc_murkblood_spearman();
	new npc_murkblood_healer();
	new npc_murkblood_oracle();
	new npc_lykul_stinger();
	new npc_lykul_wasp();
	new npc_fen_ray();
}