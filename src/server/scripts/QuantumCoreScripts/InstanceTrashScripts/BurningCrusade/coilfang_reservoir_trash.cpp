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

enum CoilfangReservoirSpells
{
	SPELL_ACID_GEYSER            = 38971,
	SPELL_ATROPHIC_BLOW          = 39015,
	SPELL_INITIAL_INFECTION      = 39032,
	SPELL_SERPENTSHRINE_PARASITE = 39044,
	SPELL_SPORE_QUAKE            = 38976,
	SPELL_TOXIC_POOL             = 38718,
	SPELL_ENRAGE                 = 39031,
	SPELL_SILENCE                = 38491,
	SPELL_SONIC_SCREAM           = 38496,
	SPELL_THROW                  = 38904,
	SPELL_CLEAVE                 = 38474,
	SPELL_BESTIAL_WRATH          = 38484,
	SPELL_SHATTER_ARMOR          = 38591,
	SPELL_GREATER_HEAL           = 38580,
	SPELL_HOLY_SMITE             = 38582,
	SPELL_HOLY_FIRE              = 38585,
	SPELL_CHARGE                 = 38461,
	SPELL_SPORE_BURST            = 38471,
	SPELL_KICK                   = 38625,
	SPELL_HAMSTRING              = 38995,
	SPELL_ARCING_SMASH           = 28168,
	SPELL_TELEPORT               = 39700,
	SPELL_FROSTBRAND_ATTACK      = 38617,
	SPELL_POISON_SHIELD          = 39027,
	SPELL_SUMMON_WATER_TOTEM     = 38624,
	SPELL_SUMMON_WATER_ELEMENTAL = 38622,
	SPELL_FROST_NOVA             = 39035,
	SPELL_WATER_BOLT_VOLLEY      = 38623,
	SPELL_SHIELD_CHARGE          = 38630,
	SPELL_AVENGERS_SHIELD        = 38631,
	SPELL_POISON_BOLT_VOLLEY     = 38655,
	SPELL_RANCID_MUSHROOM        = 38650, // Spell Does Not Work
	SPELL_SPORE_CLOUD            = 38652, // Use by npc id 22250
	SPELL_ARCANE_DESTRUCTION     = 38647,
	SPELL_FIRE_DESTRUCTION       = 38648,
	SPELL_FROST_DESTRUCTION      = 38649,
	SPELL_FIREBALL               = 38641,
	SPELL_FROSTBOLT              = 38645,
	SPELL_ARCANE_LIGHTNING       = 38634,
	SPELL_ARCANE_VOLLEY          = 38633,
	SPELL_SCORCH                 = 38636,
	SPELL_RAIN_OF_FIRE           = 38635,
	SPELL_CONE_OF_COLD           = 38644,
	SPELL_BLINK                  = 38642,
	SPELL_HEALING_TOUCH          = 38658,
	SPELL_REJUVENATION           = 38657,
	SPELL_TRANQUILITY            = 38659,
	SPELL_UPPERCUT               = 39069,
	SPELL_BLOODTHIRST            = 39070,
	SPELL_TW_ENRAGE              = 38664,
	SPELL_LIGHTNING_BOLT         = 39065,
	SPELL_CHAIN_LIGHTNING        = 39066,
	SPELL_LIGHTNING_SHIELD       = 39067,
	SPELL_NET                    = 38661,
	SPELL_TH_THROW               = 39060,
	SPELL_IMPALE                 = 39061,
	SPELL_FROST_SHOCK            = 39062,
	SPELL_TDH_FROST_NOVA         = 39063,
	SPELL_TDH_FROSTBOLT          = 39064,
	SPELL_EXECUTE                = 38959,
	SPELL_FRIGHTENING_SHOUT      = 38945,
	SPELL_KNOCKBACK              = 38576,
	SPELL_MORTAL_CLEAVE          = 38572,
	SPELL_FRENZY                 = 38947,
	SAY_AGGRO                    = 0,
};

enum Creatues
{
	NPC_WATER_TOTEM     = 22236,
	NPC_WATER_ELEMENTAL = 22238,
	NPC_RANCID_MUSHROOM = 22250,
};

class npc_underbog_colossus : public CreatureScript
{
public:
    npc_underbog_colossus() : CreatureScript("npc_underbog_colossus") {}

    struct npc_underbog_colossusAI : public QuantumBasicAI
    {
        npc_underbog_colossusAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 AcidGeyserTimer;
		uint32 AtrophicBlowTimer;
		uint32 InitialInfectionTimer;
		uint32 SeprentShrineParasiteTimer;
		uint32 SporeQuakeTimer;

        void Reset()
		{
			AcidGeyserTimer = 3000;
			AtrophicBlowTimer = 11000;
			InitialInfectionTimer = 12000;
			SeprentShrineParasiteTimer = 13000;
			SporeQuakeTimer = 14000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* /*killer*/)
		{
			DoCastAOE(SPELL_TOXIC_POOL);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (AcidGeyserTimer <= diff)
			{
				DoCastAOE(SPELL_ACID_GEYSER);
				AcidGeyserTimer = 10000;
			}
			else AcidGeyserTimer -= diff;

			if (AtrophicBlowTimer <= diff)
			{
				DoCastVictim(SPELL_ATROPHIC_BLOW);
				AtrophicBlowTimer = 11000;
			}
			else AtrophicBlowTimer -= diff;
			
			if (InitialInfectionTimer <= diff)
			{
				DoCastVictim(SPELL_INITIAL_INFECTION);
				InitialInfectionTimer = 12000;
			}
			else InitialInfectionTimer -= diff;

			if (SeprentShrineParasiteTimer <= diff)
			{
				DoCastVictim(SPELL_SERPENTSHRINE_PARASITE);
				SeprentShrineParasiteTimer = 13000;
			}
			else SeprentShrineParasiteTimer -= diff;

			if (SporeQuakeTimer <= diff)
			{
				DoCastAOE(SPELL_SPORE_QUAKE);
				SporeQuakeTimer = 14000;
			}
			else SporeQuakeTimer -= diff;

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
        return new npc_underbog_colossusAI(creature);
    }
};

class npc_coilfang_hate_screamer : public CreatureScript
{
public:
    npc_coilfang_hate_screamer() : CreatureScript("npc_coilfang_hate_screamer") {}

    struct npc_coilfang_hate_screamerAI : public QuantumBasicAI
    {
        npc_coilfang_hate_screamerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 SilenceTimer;
		uint32 SonicScreamTimer;

        void Reset()
		{
			SilenceTimer = 3000;
			SonicScreamTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SilenceTimer <= diff)
			{
				DoCastVictim(SPELL_SILENCE);
				SilenceTimer = urand(3000, 4000);
			}
			else SilenceTimer -= diff;

			if (SonicScreamTimer <= diff)
			{
				DoCastVictim(SPELL_SONIC_SCREAM);
				SonicScreamTimer = urand(5000, 6000);
			}
			else SonicScreamTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_hate_screamerAI(creature);
    }
};

class npc_coilfang_beast_tamer : public CreatureScript
{
public:
    npc_coilfang_beast_tamer() : CreatureScript("npc_coilfang_beast_tamer") {}

    struct npc_coilfang_beast_tamerAI : public QuantumBasicAI
    {
        npc_coilfang_beast_tamerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CleaveTimer;
		uint32 BestialWrathTimer;

        void Reset()
		{
			CleaveTimer = 4000;
			BestialWrathTimer = 6000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_THROW);
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
				CleaveTimer = urand(5000, 6000);
			}
			else CleaveTimer -= diff;

			if (BestialWrathTimer <= diff)
			{
				DoCast(me, SPELL_BESTIAL_WRATH);
				BestialWrathTimer = urand(7000, 8000);
			}
			else BestialWrathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_beast_tamerAI(creature);
    }
};

class npc_coilfang_shatterer : public CreatureScript
{
public:
    npc_coilfang_shatterer() : CreatureScript("npc_coilfang_shatterer") {}

    struct npc_coilfang_shattererAI : public QuantumBasicAI
    {
        npc_coilfang_shattererAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShatterArmorTimer;

        void Reset()
		{
			ShatterArmorTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShatterArmorTimer <= diff)
			{
				DoCastVictim(SPELL_SHATTER_ARMOR);
				ShatterArmorTimer = urand(5000, 6000);
			}
			else ShatterArmorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_shattererAI(creature);
    }
};

class npc_coilfang_priestess : public CreatureScript
{
public:
    npc_coilfang_priestess() : CreatureScript("npc_coilfang_priestess") {}

    struct npc_coilfang_priestessAI : public QuantumBasicAI
    {
        npc_coilfang_priestessAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 HolySmiteTimer;
		uint32 HolyFireTimer;
		uint32 GreaterHealTimer;

        void Reset()
		{
			HolySmiteTimer = 2000;
			HolyFireTimer = 3000;
			GreaterHealTimer = 5000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HolySmiteTimer <= diff)
			{
				DoCastVictim(SPELL_HOLY_SMITE);
				HolySmiteTimer = urand(3000, 4000);
			}
			else HolySmiteTimer -= diff;

			if (HolyFireTimer <= diff)
			{
				DoCastVictim(SPELL_HOLY_FIRE);
				HolyFireTimer = urand(5000, 6000);
			}
			else HolyFireTimer -= diff;

			if (GreaterHealTimer <= diff)
			{
				DoCast(me, SPELL_GREATER_HEAL);
				GreaterHealTimer = urand(7000, 8000);
			}
			else GreaterHealTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_priestessAI(creature);
    }
};

class npc_serpentshrine_sporebat : public CreatureScript
{
public:
    npc_serpentshrine_sporebat() : CreatureScript("npc_serpentshrine_sporebat") {}

    struct npc_serpentshrine_sporebatAI : public QuantumBasicAI
    {
        npc_serpentshrine_sporebatAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 SporeBurstTimer;

        void Reset()
		{
			SporeBurstTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_CHARGE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SporeBurstTimer <= diff)
			{
				DoCastAOE(SPELL_SPORE_BURST);
				SporeBurstTimer = urand(3000, 4000);
			}
			else SporeBurstTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_serpentshrine_sporebatAI(creature);
    }
};

class npc_greyheart_skulker : public CreatureScript
{
public:
    npc_greyheart_skulker() : CreatureScript("npc_greyheart_skulker") {}

    struct npc_greyheart_skulkerAI : public QuantumBasicAI
    {
        npc_greyheart_skulkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 KickTimer;

        void Reset()
		{
			KickTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (KickTimer <= diff)
			{
				DoCastVictim(SPELL_KICK);
				KickTimer = urand(4000, 5000);
			}
			else KickTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_greyheart_skulkerAI(creature);
    }
};

class npc_coilfang_guardian : public CreatureScript
{
public:
    npc_coilfang_guardian() : CreatureScript("npc_coilfang_guardian") {}

    struct npc_coilfang_guardianAI : public QuantumBasicAI
    {
        npc_coilfang_guardianAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 HamstringTimer;
		uint32 ArcingSmashTimer;

        void Reset()
		{
			HamstringTimer = 2000;
			ArcingSmashTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			Talk(SAY_AGGRO);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HamstringTimer <= diff)
			{
				DoCastVictim(SPELL_HAMSTRING);
				HamstringTimer = urand(3000, 4000);
			}
			else HamstringTimer -= diff;

			if (ArcingSmashTimer <= diff)
			{
				DoCastAOE(SPELL_ARCING_SMASH);
				ArcingSmashTimer = urand(5000, 6000);
			}
			else ArcingSmashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_guardianAI(creature);
    }
};

class npc_greyheart_tidecaller : public CreatureScript
{
public:
    npc_greyheart_tidecaller() : CreatureScript("npc_greyheart_tidecaller") {}

    struct npc_greyheart_tidecallerAI : public QuantumBasicAI
    {
        npc_greyheart_tidecallerAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 PoisonShieldTimer;
		uint32 SummonWaterTotemTimer;

		SummonList Summons;

        void Reset()
		{
			PoisonShieldTimer = 1000;
			SummonWaterTotemTimer = 5000;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastAOE(SPELL_FROSTBRAND_ATTACK);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			switch (summon->GetEntry())
			{
				case NPC_WATER_TOTEM:
				case NPC_WATER_ELEMENTAL:
					Summons.Summon(summon);
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

			if (PoisonShieldTimer <= diff)
			{
				DoCast(me, SPELL_POISON_SHIELD);
				PoisonShieldTimer = urand(5000, 6000);
			}
			else PoisonShieldTimer -= diff;

			if (SummonWaterTotemTimer <= diff)
			{
				DoCastAOE(SPELL_SUMMON_WATER_TOTEM);
				SummonWaterTotemTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else SummonWaterTotemTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_greyheart_tidecallerAI(creature);
    }
};

class npc_serpentshrine_tidecaller : public CreatureScript
{
public:
    npc_serpentshrine_tidecaller() : CreatureScript("npc_serpentshrine_tidecaller") {}

    struct npc_serpentshrine_tidecallerAI : public QuantumBasicAI
    {
        npc_serpentshrine_tidecallerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FrostNovaTimer;
		uint32 WaterBoltVolleyTimer;

        void Reset()
		{
			FrostNovaTimer = 2000;
			WaterBoltVolleyTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(SPELL_FROST_NOVA);
				FrostNovaTimer = urand(3000, 4000);
			}
			else FrostNovaTimer -= diff;

			if (WaterBoltVolleyTimer <= diff)
			{
				DoCastVictim(SPELL_WATER_BOLT_VOLLEY);
				WaterBoltVolleyTimer = urand(5000, 6000);
			}
			else WaterBoltVolleyTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_serpentshrine_tidecallerAI(creature);
    }
};

class npc_greyheart_shield_bearer : public CreatureScript
{
public:
    npc_greyheart_shield_bearer() : CreatureScript("npc_greyheart_shield_bearer") {}

    struct npc_greyheart_shield_bearerAI : public QuantumBasicAI
    {
        npc_greyheart_shield_bearerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 AvengersShieldTimer;

        void Reset()
		{
			AvengersShieldTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_SHIELD_CHARGE);
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
					AvengersShieldTimer = urand(5000, 6000);
				}
			}
			else AvengersShieldTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_greyheart_shield_bearerAI(creature);
    }
};

class npc_serpentshrine_lurker : public CreatureScript
{
public:
    npc_serpentshrine_lurker() : CreatureScript("npc_serpentshrine_lurker") {}

    struct npc_serpentshrine_lurkerAI : public QuantumBasicAI
    {
        npc_serpentshrine_lurkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 PoisonBoltVolleyTimer;

        void Reset()
		{
			PoisonBoltVolleyTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PoisonBoltVolleyTimer <= diff)
			{
				DoCastVictim(SPELL_POISON_BOLT_VOLLEY);
				PoisonBoltVolleyTimer = urand(4000, 5000);
			}
			else PoisonBoltVolleyTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_serpentshrine_lurkerAI(creature);
    }
};

class npc_greyheart_nether_mage : public CreatureScript
{
public:
    npc_greyheart_nether_mage() : CreatureScript("npc_greyheart_nether_mage") {}

    struct npc_greyheart_nether_mageAI : public QuantumBasicAI
    {
        npc_greyheart_nether_mageAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 BuffTimer;
		uint32 FireballTimer;
		uint32 FrostboltTimer;
		uint32 ArcaneLightningTimer;
		uint32 ArcaneVolleyTimer;
		uint32 ScorchTimer;
		uint32 ConeOfColdTimer;
		uint32 RainOfFireTimer;

        void Reset()
		{
			BuffTimer = 1000;
			FireballTimer = 3000;
			FrostboltTimer = 4000;
			ArcaneLightningTimer = 5000;
			ArcaneVolleyTimer = 6000;
			ScorchTimer = 7000;
			ConeOfColdTimer = 8000;
			RainOfFireTimer = 9000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_FIRE_DESTRUCTION);
			DoCast(me, SPELL_FROST_DESTRUCTION);
			DoCast(me, SPELL_ARCANE_DESTRUCTION);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BuffTimer <= diff)
			{
				DoCast(me, SPELL_FIRE_DESTRUCTION);
				DoCast(me, SPELL_FROST_DESTRUCTION);
				DoCast(me, SPELL_ARCANE_DESTRUCTION);
				BuffTimer = urand(4000, 5000);
			}
			else BuffTimer -= diff;

			if (FireballTimer <= diff)
			{
				DoCastVictim(SPELL_FIREBALL);
				FireballTimer = urand(6000, 7000);
			}
			else FireballTimer -= diff;

			if (FrostboltTimer <= diff)
			{
				DoCastVictim(SPELL_FROSTBOLT);
				FrostboltTimer = urand(8000, 9000);
			}
			else FrostboltTimer -= diff;

			if (ArcaneLightningTimer <= diff)
			{
				DoCastVictim(SPELL_ARCANE_LIGHTNING);
				ArcaneLightningTimer = urand(10000, 11000);
			}
			else ArcaneLightningTimer -= diff;

			if (ArcaneVolleyTimer <= diff)
			{
				DoCastAOE(SPELL_ARCANE_VOLLEY);
				ArcaneVolleyTimer = urand(12000, 13000);
			}
			else ArcaneVolleyTimer -= diff;

			if (ScorchTimer <= diff)
			{
				DoCastVictim(SPELL_SCORCH);
				ScorchTimer = urand(14000, 15000);
			}
			else ScorchTimer -= diff;

			if (ConeOfColdTimer <= diff)
			{
				DoCast(SPELL_CONE_OF_COLD);
				ConeOfColdTimer = urand(16000, 17000);
			}
			else ConeOfColdTimer -= diff;

			if (RainOfFireTimer <= diff)
			{
				DoCastAOE(SPELL_RAIN_OF_FIRE);
				RainOfFireTimer = urand(18000, 19000);
			}
			else RainOfFireTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_greyheart_nether_mageAI(creature); // Need More Work (Total Complete 95%)
    }
};

class npc_greyheart_technican : public CreatureScript
{
public:
    npc_greyheart_technican() : CreatureScript("npc_greyheart_technican") {}

    struct npc_greyheart_technicanAI : public QuantumBasicAI
    {
        npc_greyheart_technicanAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 HamstringTimer;

        void Reset()
		{
			HamstringTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HamstringTimer <= diff)
			{
				DoCastVictim(SPELL_HAMSTRING);
				HamstringTimer = urand(4000, 5000);
			}
			else HamstringTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_greyheart_technicanAI(creature);
    }
};

class npc_tidewalker_depth_seer : public CreatureScript
{
public:
    npc_tidewalker_depth_seer() : CreatureScript("npc_tidewalker_depth_seer") {}

    struct npc_tidewalker_depth_seerAI : public QuantumBasicAI
    {
        npc_tidewalker_depth_seerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 HealingTouchTimer;
		uint32 RejuvenationTimer;
		uint32 TranquilityTimer;

        void Reset()
		{
			HealingTouchTimer = 2000;
			RejuvenationTimer = 4000;
			TranquilityTimer = 6000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HealingTouchTimer <= diff)
			{
				DoCast(me, SPELL_HEALING_TOUCH);
				HealingTouchTimer = urand(3000, 4000);
			}
			else HealingTouchTimer -= diff;

			if (RejuvenationTimer <= diff)
			{
				DoCast(me, SPELL_REJUVENATION);
				RejuvenationTimer = urand(4000, 6000);
			}
			else RejuvenationTimer -= diff;

			if (RejuvenationTimer <= diff)
			{
				DoCastAOE(SPELL_TRANQUILITY);
				RejuvenationTimer = urand(8000, 9000);
			}
			else RejuvenationTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tidewalker_depth_seerAI(creature);
    }
};

class npc_tidewalker_warrior : public CreatureScript
{
public:
    npc_tidewalker_warrior() : CreatureScript("npc_tidewalker_warrior") {}

    struct npc_tidewalker_warriorAI : public QuantumBasicAI
    {
        npc_tidewalker_warriorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 UppercutTimer;
		uint32 BloodthirstTimer;

        void Reset()
		{
			UppercutTimer = 3000;
			BloodthirstTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (UppercutTimer <= diff)
			{
				DoCastVictim(SPELL_UPPERCUT);
				UppercutTimer = urand(4000, 5000);
			}
			else UppercutTimer -= diff;

			if (BloodthirstTimer <= diff)
			{
				DoCastVictim(SPELL_BLOODTHIRST);
				BloodthirstTimer = urand(5000, 6000);
			}
			else BloodthirstTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_TW_ENRAGE, 0))
				{
					DoCast(me, SPELL_TW_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tidewalker_warriorAI(creature);
    }
};

class npc_tidewalker_shaman : public CreatureScript
{
public:
    npc_tidewalker_shaman() : CreatureScript("npc_tidewalker_shaman") {}

    struct npc_tidewalker_shamanAI : public QuantumBasicAI
    {
        npc_tidewalker_shamanAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 LightningShieldTimer;
		uint32 LightningBoltTimer;
		uint32 ChainLightningTimer;

        void Reset()
		{
			LightningShieldTimer = 1000;
			LightningBoltTimer = 3000;
			ChainLightningTimer = 5000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (LightningShieldTimer <= diff)
			{
				DoCast(me, SPELL_LIGHTNING_SHIELD);
				LightningShieldTimer = 3000;
			}
			else LightningShieldTimer -= diff;

			if (LightningBoltTimer <= diff)
			{
				DoCastVictim(SPELL_LIGHTNING_BOLT);
				LightningBoltTimer = 5000;
			}
			else LightningBoltTimer -= diff;

			if (ChainLightningTimer <= diff)
			{
				DoCastVictim(SPELL_CHAIN_LIGHTNING);
				ChainLightningTimer = 7000;
			}
			else ChainLightningTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tidewalker_shamanAI(creature);
    }
};

class npc_tidewalker_harpooner : public CreatureScript
{
public:
    npc_tidewalker_harpooner() : CreatureScript("npc_tidewalker_harpooner") {}

    struct npc_tidewalker_harpoonerAI : public QuantumBasicAI
    {
        npc_tidewalker_harpoonerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 NetTimer;
		uint32 ThrowTimer;
		uint32 ImpaleTimer;

        void Reset()
		{
			NetTimer = 2000;
			ThrowTimer = 3000;
			ImpaleTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (NetTimer <= diff)
			{
				DoCastVictim(SPELL_NET);
				NetTimer = urand(3000, 4000);
			}
			else NetTimer -= diff;

			if (ThrowTimer <= diff)
			{
				DoCastVictim(SPELL_TH_THROW);
				ThrowTimer = urand(5000, 6000);
			}
			else ThrowTimer -= diff;

			if (ImpaleTimer <= diff)
			{
				DoCastVictim(SPELL_IMPALE);
				ImpaleTimer = urand(7000, 8000);
			}
			else ImpaleTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tidewalker_harpoonerAI(creature);
    }
};

class npc_tidewalker_hydromancer : public CreatureScript
{
public:
    npc_tidewalker_hydromancer() : CreatureScript("npc_tidewalker_hydromancer") {}

    struct npc_tidewalker_hydromancerAI : public QuantumBasicAI
    {
        npc_tidewalker_hydromancerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FrostShockTimer;
		uint32 FrostNovaTimer;
		uint32 FrostboltTimer;

        void Reset()
		{
			FrostShockTimer = 2000;
			FrostNovaTimer = 3000;
			FrostboltTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostShockTimer <= diff)
			{
				DoCastVictim(SPELL_FROST_SHOCK);
				FrostShockTimer = urand(3000, 4000);
			}
			else FrostShockTimer -= diff;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(SPELL_TDH_FROST_NOVA);
				FrostNovaTimer = urand(4000, 5000);
			}
			else FrostNovaTimer -= diff;

			if (FrostboltTimer <= diff)
			{
				DoCastVictim(SPELL_TDH_FROSTBOLT);
				FrostboltTimer = urand(5000, 6000);
			}
			else FrostboltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tidewalker_hydromancerAI(creature);
    }
};

class npc_vashjir_honor_guard : public CreatureScript
{
public:
    npc_vashjir_honor_guard() : CreatureScript("npc_vashjir_honor_guard") {}

    struct npc_vashjir_honor_guardAI : public QuantumBasicAI
    {
        npc_vashjir_honor_guardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 MortalCleaveTimer;
		uint32 ExecuteTimer;
		uint32 KnockbackTimer;
		uint32 FrighteningShoutTimer;

        void Reset()
		{
			MortalCleaveTimer = 2000;
			ExecuteTimer = 3000;
			KnockbackTimer = 4000;
			FrighteningShoutTimer = 10000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MortalCleaveTimer <= diff)
			{
				DoCastVictim(SPELL_MORTAL_CLEAVE);
				MortalCleaveTimer = urand(3000, 4000);
			}
			else MortalCleaveTimer -= diff;

			if (ExecuteTimer <= diff)
			{
				DoCastVictim(SPELL_EXECUTE);
				ExecuteTimer = urand(5000, 6000);
			}
			else ExecuteTimer -= diff;

			if (KnockbackTimer <= diff)
			{
				DoCastVictim(SPELL_KNOCKBACK);
				KnockbackTimer = urand(7000, 8000);
			}
			else KnockbackTimer -= diff;

			if (FrighteningShoutTimer <= diff)
			{
				DoCastAOE(SPELL_FRIGHTENING_SHOUT);
				FrighteningShoutTimer = urand(13000, 14000);
			}
			else FrighteningShoutTimer -= diff;

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
        return new npc_vashjir_honor_guardAI(creature);
    }
};

void AddSC_coilfang_reservoir_trash()
{
	new npc_underbog_colossus();
	new npc_coilfang_hate_screamer();
	new npc_coilfang_beast_tamer();
	new npc_coilfang_shatterer();
	new npc_coilfang_priestess();
	new npc_serpentshrine_sporebat();
	new npc_greyheart_skulker();
	new npc_coilfang_guardian();
	new npc_greyheart_tidecaller();
	new npc_serpentshrine_tidecaller();
	new npc_greyheart_shield_bearer();
	new npc_serpentshrine_lurker();
	new npc_greyheart_nether_mage();
	new npc_greyheart_technican();
	new npc_tidewalker_depth_seer();
	new npc_tidewalker_warrior();
	new npc_tidewalker_shaman();
	new npc_tidewalker_harpooner();
	new npc_tidewalker_hydromancer();
	new npc_vashjir_honor_guard();
}