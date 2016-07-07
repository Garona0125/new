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

enum SteamVaultSpells
{
	SPELL_DEFENSIVE_STANCE     = 7164,
	SPELL_BATTLE_SHOUT         = 31403,
	SPELL_MORTAL_BLOW          = 35105,
	SPELL_NET                  = 6533,
	SPELL_BOMB_5N              = 40331,
	SPELL_BOMB_5H              = 40332,
	SPELL_FEAR                 = 38660,
	SPELL_LIGHTNING_BOLT_5N    = 15234,
	SPELL_LIGHTNING_BOLT_5H    = 37664,
	SPELL_ARCANE_FLARE_5N      = 35106,
	SPELL_ARCANE_FLARE_5H      = 37856,
	SPELL_FROST_SHOCK_5N       = 22582,
	SPELL_FROST_SHOCK_5H       = 37865,
	SPELL_HEAL_5N              = 31730,
	SPELL_HEAL_5H              = 22883,
	SPELL_SONIC_BURST          = 8281,
	SPELL_CLEAVE               = 15496,
	SPELL_EXECUTE              = 7160,
	SPELL_FROSTBOLT_5N         = 12675,
	SPELL_FROSTBOLT_5H         = 37930,
	SPELL_BLIZZARD_5N          = 39416,
	SPELL_BLIZZARD_5H          = 31581,
	SPELL_FROST_NOVA_5N        = 15063,
	SPELL_FROST_NOVA_5H        = 15531,
	SPELL_DISARM               = 6713,
	SPELL_FRENZY               = 8269,
	SPELL_GEYSER               = 10987,
	SPELL_DISEASE_CLOUD_5N     = 37266,
	SPELL_DISEASE_CLOUD_5H     = 37863,
	SPELL_POISON_BOLT_5N       = 37272,
	SPELL_POISON_BOLT_5H       = 37862,
	SPELL_FUNGAL_DECAY_5N      = 32065,
	SPELL_FUNGAL_DECAY_5H      = 34158,
	SPELL_TRAMPLE              = 40340,
	SPELL_ENRAGE               = 8599,
	SPELL_WATER_SPOUT          = 37250,
	SPELL_COSMETIC_STEAM       = 36151,
	SPELL_WATER_BOLT_5N        = 37252,
	SPELL_WATER_BOLT_5H        = 39412,
	SPELL_WATER_BOLT_VOLLEY_5N = 34449,
	SPELL_WATER_BOLT_VOLLEY_5H = 37924,
};

class npc_coilfang_warrior : public CreatureScript
{
public:
    npc_coilfang_warrior() : CreatureScript("npc_coilfang_warrior") {}

    struct npc_coilfang_warriorAI : public QuantumBasicAI
    {
        npc_coilfang_warriorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 BattleShoutTimer;
		uint32 MortalBlowTimer;

        void Reset()
		{
			BattleShoutTimer = 500;
			MortalBlowTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			DoCast(me, SPELL_DEFENSIVE_STANCE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BattleShoutTimer <= diff)
			{
				DoCastAOE(SPELL_BATTLE_SHOUT);
				BattleShoutTimer = 8000;
			}
			else BattleShoutTimer -= diff;

			if (MortalBlowTimer <= diff)
			{
				DoCastVictim(SPELL_MORTAL_BLOW);
				MortalBlowTimer = urand(3000, 4000);
			}
			else MortalBlowTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_warriorAI(creature);
    }
};

class npc_coilfang_engineer : public CreatureScript
{
public:
    npc_coilfang_engineer() : CreatureScript("npc_coilfang_engineer") {}

    struct npc_coilfang_engineerAI : public QuantumBasicAI
    {
        npc_coilfang_engineerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 BombTimer;
		uint32 NetTimer;

        void Reset()
		{
			BombTimer = 500;
			NetTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_WORK_MINING);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BombTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_BOMB_5N, SPELL_BOMB_5H));
					BombTimer = urand(3000, 4000);
				}
			}
			else BombTimer -= diff;

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
        return new npc_coilfang_engineerAI(creature);
    }
};

class npc_coilfang_siren : public CreatureScript
{
public:
    npc_coilfang_siren() : CreatureScript("npc_coilfang_siren") {}

    struct npc_coilfang_sirenAI : public QuantumBasicAI
    {
        npc_coilfang_sirenAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FearTimer;
		uint32 LightningBoltTimer;
		uint32 ArcaneFlareTimer;

        void Reset()
		{
			FearTimer = 500;
			LightningBoltTimer = 2000;
			ArcaneFlareTimer = 5000;

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

			if (FearTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FEAR);
					FearTimer = urand(10000, 11000);
				}
			}
			else FearTimer -= diff;

			if (LightningBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_LIGHTNING_BOLT_5N, SPELL_LIGHTNING_BOLT_5H));
					LightningBoltTimer = urand(3000, 4000);
				}
			}
			else LightningBoltTimer -= diff;

			if (ArcaneFlareTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_ARCANE_FLARE_5N, SPELL_ARCANE_FLARE_5H), true);
					ArcaneFlareTimer = urand(5000, 6000);
				}
			}
			else ArcaneFlareTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_sirenAI(creature);
    }
};

class npc_coilfang_oracle : public CreatureScript
{
public:
    npc_coilfang_oracle() : CreatureScript("npc_coilfang_oracle") {}

    struct npc_coilfang_oracleAI : public QuantumBasicAI
    {
        npc_coilfang_oracleAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FrostShockTimer;
		uint32 HealTimer;
		uint32 SonicBurstTimer;

        void Reset()
		{
			FrostShockTimer = 500;
			HealTimer = 2000;
			SonicBurstTimer = 5000;

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

			if (FrostShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FROST_SHOCK_5N, SPELL_FROST_SHOCK_5H));
					FrostShockTimer = urand(3000, 4000);
				}
			}
			else FrostShockTimer -= diff;

			if (HealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, DUNGEON_MODE(SPELL_HEAL_5N, SPELL_HEAL_5H));
					HealTimer = urand(5000, 6000);
				}
			}
			else HealTimer -= diff;

			if (SonicBurstTimer <= diff)
			{
				DoCastAOE(SPELL_SONIC_BURST);
				SonicBurstTimer = urand(7000, 8000);
			}
			else SonicBurstTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
				DoCast(me, DUNGEON_MODE(SPELL_HEAL_5N, SPELL_HEAL_5H));

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_oracleAI(creature);
    }
};

class npc_coilfang_myrmidon : public CreatureScript
{
public:
    npc_coilfang_myrmidon() : CreatureScript("npc_coilfang_myrmidon") {}

    struct npc_coilfang_myrmidonAI : public QuantumBasicAI
    {
        npc_coilfang_myrmidonAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 CleaveTimer;
		uint32 ExecuteTimer;

        void Reset()
		{
			CleaveTimer = 2000;
			ExecuteTimer = 4000;

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

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(3000, 4000);
			}
			else CleaveTimer -= diff;

			if (ExecuteTimer <= diff && me->GetVictim()->HealthBelowPct(HEALTH_PERCENT_20))
			{
				DoCastVictim(SPELL_EXECUTE);
				ExecuteTimer = urand(5000, 6000);
			}
			else ExecuteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_myrmidonAI(creature);
    }
};

class npc_coilfang_sorceress : public CreatureScript
{
public:
    npc_coilfang_sorceress() : CreatureScript("npc_coilfang_sorceress") {}

    struct npc_coilfang_sorceressAI : public QuantumBasicAI
    {
        npc_coilfang_sorceressAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FrostboltTimer;
		uint32 BlizzardTimer;
		uint32 FrostNovaTimer;

        void Reset()
		{
			FrostboltTimer = 500;
			BlizzardTimer = 2000;
			FrostNovaTimer = 4000;

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

			if (BlizzardTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_BLIZZARD_5N, SPELL_BLIZZARD_5H), true);
					BlizzardTimer = urand(5000, 6000);
				}
			}
			else BlizzardTimer -= diff;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_FROST_NOVA_5N, SPELL_FROST_NOVA_5H));
				FrostNovaTimer = urand(7000, 8000);
			}
			else FrostNovaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_sorceressAI(creature);
    }
};

class npc_coilfang_slavemaster : public CreatureScript
{
public:
    npc_coilfang_slavemaster() : CreatureScript("npc_coilfang_slavemaster") {}

    struct npc_coilfang_slavemasterAI : public QuantumBasicAI
    {
        npc_coilfang_slavemasterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 DisarmTimer;
		uint32 GeyserTimer;

        void Reset()
		{
			DisarmTimer = 2000;
			GeyserTimer = 3000;

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

			// Frenzy Buff Check in Combat
			if (!me->HasAura(SPELL_FRENZY))
			{
				DoCast(me, SPELL_FRENZY, true);
				DoSendQuantumText(SAY_GENERIC_EMOTE_FRENZY, me);
			}

			if (DisarmTimer <= diff)
			{
				DoCastVictim(SPELL_DISARM);
				DisarmTimer = urand(4000, 5000);
			}
			else DisarmTimer -= diff;

			if (GeyserTimer <= diff)
			{
				DoCastAOE(SPELL_GEYSER);
				GeyserTimer = urand(6000, 7000);
			}
			else GeyserTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_slavemasterAI(creature);
    }
};

class npc_coilfang_water_elemental : public CreatureScript
{
public:
    npc_coilfang_water_elemental() : CreatureScript("npc_coilfang_water_elemental") {}

    struct npc_coilfang_water_elementalAI : public QuantumBasicAI
    {
        npc_coilfang_water_elementalAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 WaterBoltVolleyTimer;

        void Reset()
		{
			WaterBoltVolleyTimer = 500;

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

			if (WaterBoltVolleyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_WATER_BOLT_VOLLEY_5N, SPELL_WATER_BOLT_VOLLEY_5H));
					WaterBoltVolleyTimer = urand(3000, 4000);
				}
			}
			else WaterBoltVolleyTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_water_elementalAI(creature);
    }
};

class npc_bog_overlord : public CreatureScript
{
public:
    npc_bog_overlord() : CreatureScript("npc_bog_overlord") {}

    struct npc_bog_overlordAI : public QuantumBasicAI
    {
        npc_bog_overlordAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 PoisonBoltTimer;
		uint32 FungalDecayTimer;
		uint32 TrampleTimer;

        void Reset()
		{
			PoisonBoltTimer = 500;
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

			if (PoisonBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_POISON_BOLT_5N, SPELL_POISON_BOLT_5H));
					PoisonBoltTimer = urand(3000, 4000);
				}
			}
			else PoisonBoltTimer -= diff;

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
        return new npc_bog_overlordAI(creature);
    }
};

class npc_tidal_surger : public CreatureScript
{
public:
    npc_tidal_surger() : CreatureScript("npc_tidal_surger") {}

    struct npc_tidal_surgerAI : public QuantumBasicAI
    {
        npc_tidal_surgerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 WaterSpoutTimer;
		uint32 FrostNovaTimer;

        void Reset()
		{
			WaterSpoutTimer = 500;
			FrostNovaTimer = 2000;

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

			if (WaterSpoutTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_WATER_SPOUT);
					WaterSpoutTimer = urand(3000, 4000);
				}
			}
			else WaterSpoutTimer -= diff;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_FROST_NOVA_5N, SPELL_FROST_NOVA_5H));
				FrostNovaTimer = urand(5000, 6000);
			}
			else FrostNovaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tidal_surgerAI(creature);
    }
};

class npc_steam_surger : public CreatureScript
{
public:
    npc_steam_surger() : CreatureScript("npc_steam_surger") {}

    struct npc_steam_surgerAI : public QuantumBasicAI
    {
        npc_steam_surgerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 WaterBoltTimer;

        void Reset()
		{
			WaterBoltTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_COSMETIC_STEAM);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WaterBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_WATER_BOLT_5N, SPELL_WATER_BOLT_5H));
					WaterBoltTimer = urand(3000, 4000);
				}
			}
			else WaterBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_steam_surgerAI(creature);
    }
};

void AddSC_steam_vault_trash()
{
	new npc_coilfang_warrior();
	new npc_coilfang_engineer();
	new npc_coilfang_siren();
	new npc_coilfang_oracle();
	new npc_coilfang_myrmidon();
	new npc_coilfang_sorceress();
	new npc_coilfang_slavemaster();
	new npc_coilfang_water_elemental();
	new npc_bog_overlord();
	new npc_tidal_surger();
	new npc_steam_surger();
}