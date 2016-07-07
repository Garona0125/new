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

enum SethekkHallsSpells
{
	SPELL_THUNDERCLAP           = 33967,
	SPELL_AVENGERS_SHIELD       = 32674,
	SPELL_TALON_OF_JUSTICE      = 32654,
	SPELL_BLOODTHIRST_5N        = 33964,
	SPELL_BLOODTHIRST_5H        = 40423,
	SPELL_HOWLING_SCREECH       = 32651,
	SPELL_FRENZY                = 34970,
	SPELL_FAERIE_FIRE           = 32129,
	SPELL_ARCANE_LIGHTNING_5N   = 32690,
	SPELL_ARCANE_LIGHTNING_5H   = 38146,
	SPELL_SLOW                  = 31589,
	SPELL_SPELL_REFLECTION      = 33961,
	SPELL_SUNDER_ARMOR          = 16145,
	SPELL_EARTH_SHOCK_5N        = 15501,
	SPELL_EARTH_SHOCK_5H        = 22885,
	SPELL_SUMMON_DARK_VORTEX    = 32663,
	SPELL_FEAR                  = 27641,
	SPELL_SHRINK                = 35013,
	SPELL_SUMMON_CHARMING_TOTEM = 32764,
	SPELL_ARCANE_DESTRUCTION    = 32689,
	SPELL_ARCANE_MISSILES_5N    = 33989,
	SPELL_ARCANE_MISSILES_5H    = 22273,
	SPELL_FLASH_HEAL_5N         = 17843,
	SPELL_FLASH_HEAL_5H         = 17138,
	SPELL_REJUVENATION_5N       = 12160,
	SPELL_REJUVENATION_5H       = 15981,
	SPELL_SONIC_CHARGE_5N       = 38059,
	SPELL_SONIC_CHARGE_5H       = 39197,
	SPELL_CARNIVOROUS_BITE_5N   = 32901,
	SPELL_CARNIVOROUS_BITE_5H   = 39198,
	SPELL_SHADOW_BOLT           = 12471,
	SPELL_CURSE_OF_DARK_TALON   = 32682,
	SPELL_FROSTBOLT_5N          = 38238,
	SPELL_FROSTBOLT_5H          = 17503,
	SPELL_LIGHTNING_BREATH_5N   = 38193,
	SPELL_LIGHTNING_BREATH_5H   = 38133,
	SPELL_WING_BUFFET           = 38110,
	SPELL_FLESH_RIP             = 38056,
	SPELL_SWOOP                 = 18144,
};

enum Creatures
{
	NPC_DARK_VORTEX    = 18701,
	NPC_CHARMING_TOTEM = 20343,
};

enum Texts
{
	SAY_SETHEKK_AGGRO_1 = -1420069,
	SAY_SETHEKK_AGGRO_2 = -1420070,
	SAY_SETHEKK_AGGRO_3 = -1420071,
	SAY_SETHEKK_AGGRO_4 = -1420072,
	SAY_SETHEKK_AGGRO_5 = -1420073,
};

class npc_sethekk_guard : public CreatureScript
{
public:
    npc_sethekk_guard() : CreatureScript("npc_sethekk_guard") {}

    struct npc_sethekk_guardAI : public QuantumBasicAI
    {
        npc_sethekk_guardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ThunderclapTimer;

        void Reset()
		{
			ThunderclapTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SETHEKK_AGGRO_1, SAY_SETHEKK_AGGRO_2, SAY_SETHEKK_AGGRO_3, SAY_SETHEKK_AGGRO_4, SAY_SETHEKK_AGGRO_5), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ThunderclapTimer <= diff)
			{
				DoCastAOE(SPELL_THUNDERCLAP);
				ThunderclapTimer = 4000;
			}
			else ThunderclapTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sethekk_guardAI(creature);
    }
};

class npc_sethekk_talon_lord : public CreatureScript
{
public:
    npc_sethekk_talon_lord() : CreatureScript("npc_sethekk_talon_lord") {}

    struct npc_sethekk_talon_lordAI : public QuantumBasicAI
    {
        npc_sethekk_talon_lordAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 AvengersShieldTimer;
		uint32 TalonOfJusticeTimer;

        void Reset()
		{
			AvengersShieldTimer = 2000;
			TalonOfJusticeTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			DoCastVictim(SPELL_AVENGERS_SHIELD, true);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SETHEKK_AGGRO_1, SAY_SETHEKK_AGGRO_2, SAY_SETHEKK_AGGRO_3, SAY_SETHEKK_AGGRO_4, SAY_SETHEKK_AGGRO_5), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (AvengersShieldTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_AVENGERS_SHIELD);
					AvengersShieldTimer = 4000;
				}
			}
			else AvengersShieldTimer -= diff;

			if (TalonOfJusticeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_TALON_OF_JUSTICE);
					TalonOfJusticeTimer = 8000;
				}
			}
			else TalonOfJusticeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sethekk_talon_lordAI(creature);
    }
};

class npc_sethekk_ravenguard : public CreatureScript
{
public:
    npc_sethekk_ravenguard() : CreatureScript("npc_sethekk_ravenguard") {}

    struct npc_sethekk_ravenguardAI : public QuantumBasicAI
    {
        npc_sethekk_ravenguardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 BloodthirstTimer;
		uint32 HowlingScreechTimer;

        void Reset()
		{
			BloodthirstTimer = 2000;
			HowlingScreechTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SETHEKK_AGGRO_1, SAY_SETHEKK_AGGRO_2, SAY_SETHEKK_AGGRO_3, SAY_SETHEKK_AGGRO_4, SAY_SETHEKK_AGGRO_5), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BloodthirstTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_BLOODTHIRST_5N, SPELL_BLOODTHIRST_5H));
				BloodthirstTimer = 4000;
			}
			else BloodthirstTimer -= diff;

			if (HowlingScreechTimer <= diff)
			{
				DoCastAOE(SPELL_HOWLING_SCREECH);
				HowlingScreechTimer = 6000;
			}
			else HowlingScreechTimer -= diff;

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
        return new npc_sethekk_ravenguardAI(creature);
    }
};

class npc_sethekk_oracle : public CreatureScript
{
public:
    npc_sethekk_oracle() : CreatureScript("npc_sethekk_oracle") {}

    struct npc_sethekk_oracleAI : public QuantumBasicAI
    {
        npc_sethekk_oracleAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FaerieFireTimer;
		uint32 ArcaneLightningTimer;

        void Reset()
		{
			FaerieFireTimer = 1000;
			ArcaneLightningTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SETHEKK_AGGRO_1, SAY_SETHEKK_AGGRO_2, SAY_SETHEKK_AGGRO_3, SAY_SETHEKK_AGGRO_4, SAY_SETHEKK_AGGRO_5), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FaerieFireTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FAERIE_FIRE);
					FaerieFireTimer = 3000;
				}
			}
			else FaerieFireTimer -= diff;

			if (ArcaneLightningTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_ARCANE_LIGHTNING_5N, SPELL_ARCANE_LIGHTNING_5H));
					ArcaneLightningTimer = 4000;
				}
			}
			else ArcaneLightningTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sethekk_oracleAI(creature);
    }
};

class npc_sethekk_initiate : public CreatureScript
{
public:
    npc_sethekk_initiate() : CreatureScript("npc_sethekk_initiate") {}

    struct npc_sethekk_initiateAI : public QuantumBasicAI
    {
        npc_sethekk_initiateAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SlowTimer;
		uint32 SunderArmorTimer;
		uint32 SpellReflectionTimer;

        void Reset()
		{
			SlowTimer = 1000;
			SunderArmorTimer = 2000;
			SpellReflectionTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SETHEKK_AGGRO_1, SAY_SETHEKK_AGGRO_2, SAY_SETHEKK_AGGRO_3, SAY_SETHEKK_AGGRO_4, SAY_SETHEKK_AGGRO_5), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SlowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SLOW);
					SlowTimer = 4000;
				}
			}
			else SlowTimer -= diff;

			if (SunderArmorTimer <= diff)
			{
				DoCastVictim(SPELL_SUNDER_ARMOR);
				SunderArmorTimer = 5000;
			}
			else SunderArmorTimer -= diff;

			if (SpellReflectionTimer <= diff)
			{
				DoCast(me, SPELL_SPELL_REFLECTION, true);
				SpellReflectionTimer = 8000;
			}
			else SpellReflectionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sethekk_initiateAI(creature);
    }
};

class npc_sethekk_shaman : public CreatureScript
{
public:
    npc_sethekk_shaman() : CreatureScript("npc_sethekk_shaman") {}

    struct npc_sethekk_shamanAI : public QuantumBasicAI
    {
        npc_sethekk_shamanAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SummonDarkVortexTimer;
		uint32 EarthShockTimer;

		SummonList Summons;

        void Reset()
		{
			SummonDarkVortexTimer = 1000;
			EarthShockTimer = 2000;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SETHEKK_AGGRO_1, SAY_SETHEKK_AGGRO_2, SAY_SETHEKK_AGGRO_3, SAY_SETHEKK_AGGRO_4, SAY_SETHEKK_AGGRO_5), me);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_DARK_VORTEX)
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

			if (EarthShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_EARTH_SHOCK_5N, SPELL_EARTH_SHOCK_5H));
					EarthShockTimer = 4000;
				}
			}
			else EarthShockTimer -= diff;

			if (SummonDarkVortexTimer <= diff)
			{
				DoCast(me, SPELL_SUMMON_DARK_VORTEX, true);
				SummonDarkVortexTimer = 10000;
			}
			else SummonDarkVortexTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sethekk_shamanAI(creature);
    }
};

class npc_sethekk_prophet : public CreatureScript
{
public:
    npc_sethekk_prophet() : CreatureScript("npc_sethekk_prophet") {}

    struct npc_sethekk_prophetAI : public QuantumBasicAI
    {
        npc_sethekk_prophetAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FearTimer;

        void Reset()
		{
			FearTimer = 1000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SETHEKK_AGGRO_1, SAY_SETHEKK_AGGRO_2, SAY_SETHEKK_AGGRO_3, SAY_SETHEKK_AGGRO_4, SAY_SETHEKK_AGGRO_5), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FearTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FEAR);
					FearTimer = 5000;
				}
			}
			else FearTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sethekk_prophetAI(creature);
    }
};

class npc_time_lost_controller : public CreatureScript
{
public:
    npc_time_lost_controller() : CreatureScript("npc_time_lost_controller") {}

    struct npc_time_lost_controllerAI : public QuantumBasicAI
    {
        npc_time_lost_controllerAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SummonCharmingTotemTimer;
		uint32 ShrinkTimer;

		SummonList Summons;

        void Reset()
		{
			SummonCharmingTotemTimer = 1000;
			ShrinkTimer = 2000;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SETHEKK_AGGRO_1, SAY_SETHEKK_AGGRO_2, SAY_SETHEKK_AGGRO_3, SAY_SETHEKK_AGGRO_4, SAY_SETHEKK_AGGRO_5), me);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_CHARMING_TOTEM)
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

			if (ShrinkTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHRINK);
					ShrinkTimer = 4000;
				}
			}
			else ShrinkTimer -= diff;

			if (SummonCharmingTotemTimer <= diff)
			{
				DoCast(me, SPELL_SUMMON_CHARMING_TOTEM, true);
				SummonCharmingTotemTimer = 6000;
			}
			else SummonCharmingTotemTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_time_lost_controllerAI(creature);
    }
};

class npc_time_lost_scryer : public CreatureScript
{
public:
    npc_time_lost_scryer() : CreatureScript("npc_time_lost_scryer") {}

    struct npc_time_lost_scryerAI : public QuantumBasicAI
    {
        npc_time_lost_scryerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ArcaneDestructionTimer;
		uint32 ArcaneMissilesTimer;
		uint32 FlashHealTimer;
		uint32 RejuvenationTimer;

        void Reset()
		{
			ArcaneDestructionTimer = 500;
			ArcaneMissilesTimer = 1000;
			FlashHealTimer = 4000;
			RejuvenationTimer = 6000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SETHEKK_AGGRO_1, SAY_SETHEKK_AGGRO_2, SAY_SETHEKK_AGGRO_3, SAY_SETHEKK_AGGRO_4, SAY_SETHEKK_AGGRO_5), me);
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (ArcaneDestructionTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_ARCANE_DESTRUCTION);
				ArcaneDestructionTimer = 120000; // 2 Minutes
			}
			else ArcaneDestructionTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Arcane Destruction Buff Check in Combat
			if (!me->HasAura(SPELL_ARCANE_DESTRUCTION))
				DoCast(me, SPELL_ARCANE_DESTRUCTION, true);

			if (ArcaneMissilesTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_ARCANE_MISSILES_5N, SPELL_ARCANE_MISSILES_5H));
					ArcaneMissilesTimer = urand(3000, 4000);
				}
			}
			else ArcaneMissilesTimer -= diff;

			if (FlashHealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FLASH_HEAL_5N, SPELL_FLASH_HEAL_5H));
					FlashHealTimer = urand(5000, 6000);
				}
			}
			else FlashHealTimer -= diff;

			if (RejuvenationTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, DUNGEON_MODE(SPELL_REJUVENATION_5N, SPELL_REJUVENATION_5H));
					RejuvenationTimer = urand(7000, 8000);
				}
			}
			else RejuvenationTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				DoCast(me, DUNGEON_MODE(SPELL_FLASH_HEAL_5N, SPELL_FLASH_HEAL_5H), true);
				DoCast(me, DUNGEON_MODE(SPELL_REJUVENATION_5N, SPELL_REJUVENATION_5H), true);
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_time_lost_scryerAI(creature);
    }
};

class npc_time_lost_shadowmage : public CreatureScript
{
public:
    npc_time_lost_shadowmage() : CreatureScript("npc_time_lost_shadowmage") {}

    struct npc_time_lost_shadowmageAI : public QuantumBasicAI
    {
        npc_time_lost_shadowmageAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 CurseOfDarkTalonTimer;

        void Reset()
		{
			CurseOfDarkTalonTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SETHEKK_AGGRO_1, SAY_SETHEKK_AGGRO_2, SAY_SETHEKK_AGGRO_3, SAY_SETHEKK_AGGRO_4, SAY_SETHEKK_AGGRO_5), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CurseOfDarkTalonTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CURSE_OF_DARK_TALON);
					CurseOfDarkTalonTimer = urand(3000, 4000);
				}
			}
			else CurseOfDarkTalonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_time_lost_shadowmageAI(creature);
    }
};

class npc_avian_darkhawk : public CreatureScript
{
public:
    npc_avian_darkhawk() : CreatureScript("npc_avian_darkhawk") {}

    struct npc_avian_darkhawkAI : public QuantumBasicAI
    {
        npc_avian_darkhawkAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 CarnivorousBiteTimer;
		uint32 SonicChargeTimer;

        void Reset()
		{
			CarnivorousBiteTimer = 1000;
			SonicChargeTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			DoCastVictim(DUNGEON_MODE(SPELL_SONIC_CHARGE_5N, SPELL_SONIC_CHARGE_5H));
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CarnivorousBiteTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_CARNIVOROUS_BITE_5N, SPELL_CARNIVOROUS_BITE_5H));
				CarnivorousBiteTimer = 4000;
			}
			else CarnivorousBiteTimer -= diff;

			if (SonicChargeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SONIC_CHARGE_5N, SPELL_SONIC_CHARGE_5H));
					SonicChargeTimer = 6000;
				}
			}
			else SonicChargeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_avian_darkhawkAI(creature);
    }
};

class npc_avian_warhawk : public CreatureScript
{
public:
    npc_avian_warhawk() : CreatureScript("npc_avian_warhawk") {}

    struct npc_avian_warhawkAI : public QuantumBasicAI
    {
        npc_avian_warhawkAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 CarnivorousBiteTimer;
		uint32 SwoopTimer;
		uint32 SonicChargeTimer;

        void Reset()
		{
			CarnivorousBiteTimer = 1000;
			SwoopTimer = 2000;
			SonicChargeTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			DoCastVictim(DUNGEON_MODE(SPELL_SONIC_CHARGE_5N, SPELL_SONIC_CHARGE_5H));
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CarnivorousBiteTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_CARNIVOROUS_BITE_5N, SPELL_CARNIVOROUS_BITE_5H));
				CarnivorousBiteTimer = 4000;
			}
			else CarnivorousBiteTimer -= diff;

			if (SwoopTimer <= diff)
			{
				DoCastVictim(SPELL_SWOOP);
				SwoopTimer = 6000;
			}
			else SwoopTimer -= diff;

			if (SonicChargeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SONIC_CHARGE_5N, SPELL_SONIC_CHARGE_5H));
					SonicChargeTimer = 8000;
				}
			}
			else SonicChargeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_avian_warhawkAI(creature);
    }
};

class npc_avian_ripper : public CreatureScript
{
public:
    npc_avian_ripper() : CreatureScript("npc_avian_ripper") {}

    struct npc_avian_ripperAI : public QuantumBasicAI
    {
        npc_avian_ripperAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FleshRipTimer;

        void Reset()
		{
			FleshRipTimer = 1000;

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

			if (FleshRipTimer <= diff)
			{
				DoCastVictim(SPELL_FLESH_RIP);
				FleshRipTimer = 3000;
			}
			else FleshRipTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_avian_ripperAI(creature);
    }
};

class npc_dark_vortex : public CreatureScript
{
public:
    npc_dark_vortex() : CreatureScript("npc_dark_vortex") {}

    struct npc_dark_vortexAI : public QuantumBasicAI
    {
        npc_dark_vortexAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShadowBoltTimer;

        void Reset()
		{
			ShadowBoltTimer = 500;

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
					DoCast(target, SPELL_SHADOW_BOLT);
					ShadowBoltTimer = 3000;
				}
			}
			else ShadowBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_vortexAI(creature);
    }
};

class npc_cobalt_serpent : public CreatureScript
{
public:
    npc_cobalt_serpent() : CreatureScript("npc_cobalt_serpent") {}

    struct npc_cobalt_serpentAI : public QuantumBasicAI
    {
        npc_cobalt_serpentAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FrostboltTimer;
		uint32 LightningBreathTimer;
		uint32 WingBuffetTimer;

        void Reset()
		{
			FrostboltTimer = 500;
			LightningBreathTimer = 1000;
			WingBuffetTimer = 2000;

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

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FROSTBOLT_5N, SPELL_FROSTBOLT_5H));
					FrostboltTimer = urand(3000, 4000);
				}
			}
			else FrostboltTimer -= diff;

			if (LightningBreathTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_LIGHTNING_BREATH_5N, SPELL_LIGHTNING_BREATH_5H));
					LightningBreathTimer = urand(5000, 6000);
				}
			}
			else LightningBreathTimer -= diff;

			if (WingBuffetTimer <= diff)
			{
				DoCastAOE(SPELL_WING_BUFFET, true);
				WingBuffetTimer = urand(7000, 8000);
			}
			else WingBuffetTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_cobalt_serpentAI(creature);
    }
};

void AddSC_sethekk_halls_trash()
{
	new npc_sethekk_guard();
	new npc_sethekk_talon_lord();
	new npc_sethekk_ravenguard();
	new npc_sethekk_oracle();
	new npc_sethekk_initiate();
	new npc_sethekk_shaman();
	new npc_sethekk_prophet();
	new npc_time_lost_controller();
	new npc_time_lost_scryer();
	new npc_time_lost_shadowmage();
	new npc_avian_darkhawk();
	new npc_avian_warhawk();
	new npc_avian_ripper();
	new npc_dark_vortex();
	new npc_cobalt_serpent();
}