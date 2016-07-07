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
#include "../../../scripts/Outland/CoilfangReservoir/TheSlavePens/the_slave_pens.h"

enum TheSlavePensSpells
{
	SPELL_PIERCING_JAB            = 31551,
	SPELL_DECAYED_STRENGTH        = 35760,
	SPELL_HAMSTRING               = 9080,
	SPELL_HEAD_CRACK              = 16172,
	SPELL_SLAP                    = 6754,
	SPELL_REND_5N                 = 13738,
	SPELL_REND_5H                 = 37662,
	SPELL_FROSTBOLT_5N            = 15497,
	SPELL_FROSTBOLT_5H            = 12675,
	SPELL_FROST_NOVA_5N           = 32192,
	SPELL_FROST_NOVA_5H           = 15531,
	SPELL_ELEMENTAL_ARMOR         = 34880,
	SPELL_HEAVY_DYNAMITE_5N       = 32191,
	SPELL_HEAVY_DYNAMITE_5H       = 37666,
	SPELL_IMMOLATE_5N             = 17883,
	SPELL_IMMOLATE_5H             = 37668,
	SPELL_DEADLY_POISON           = 36872,
	SPELL_SINISTER_STRIKE         = 15667,
	SPELL_HEAL_5N                 = 34945,
	SPELL_HEAL_5H                 = 39378,
	SPELL_HOLY_NOVA_5N            = 34944,
	SPELL_HOLY_NOVA_5H            = 37669,
	SPELL_POWER_WORD_SHIELD_5N    = 17139,
	SPELL_POWER_WORD_SHIELD_5H    = 36052,
	SPELL_PSYCHIC_HORROR          = 34984,
	SPELL_ENTANGLING_ROOTS        = 32173,
	SPELL_LIGHTNING_BOLT_5N       = 15234,
	SPELL_LIGHTNING_BOLT_5H       = 37664,
	SPELL_LIGHTNING_CLOUD_5N      = 32193,
	SPELL_LIGHTNING_CLOUD_5H      = 37665,
	SPELL_ARCANE_MISSILIES        = 15790,
	SPELL_DECAYED_INTELLECT       = 31555,
	SPELL_DOMINATION              = 30923,
	SPELL_SHIELD_SLAM             = 15655,
	SPELL_SPELL_REFLECTION        = 31554,
	SPELL_SLOW                    = 31589,
	SPELL_RAIN_OF_FIRE_5N         = 16005,
	SPELL_RAIN_OF_FIRE_5H         = 39376,
	SPELL_CRIPPLE                 = 33787,
	SPELL_REVENGE                 = 19130,
	SPELL_FRENZY                  = 8269,
	SPELL_CLEAVE                  = 15284,
	SPELL_SUNDER_ARMOR            = 16145,
	SPELL_FRIGHTENING_SHOUT       = 19134,
};

enum Texts
{
	SAY_SLAVEHANDLER_WORK_1  = -1420029,
	SAY_SLAVEHANDLER_WORK_2  = -1420030,
	SAY_SLAVEHANDLER_WORK_3  = -1420031,
	SAY_SLAVEHANDLER_WORK_4  = -1420032,
	SAY_SLAVEHANDLER_WORK_5  = -1420033,
	SAY_SLAVEHANDLER_WORK_6  = -1420034,
	SAY_SLAVEHANDLER_WORK_7  = -1420035,

	SAY_NAGA_AGGRO_1         = -1420036,
	SAY_NAGA_AGGRO_2         = -1420037,
	SAY_NAGA_AGGRO_3         = -1420038,
	SAY_NAGA_AGGRO_4         = -1420039,
	SAY_NAGA_AGGRO_5         = -1420040,

	SAY_WASTEWALKER_DEATH_1  = -1420041,
	SAY_WASTEWALKER_DEATH_2  = -1420042,
	SAY_WASTEWALKER_DEATH_3  = -1420043,
	SAY_WASTEWALKER_DEATH_4  = -1420044,
	SAY_WASTEWALKER_DEATH_5  = -1420045,
	SAY_WASTEWALKER_DEATH_6  = -1420046,
	SAY_WASTEWALKER_DEATH_7  = -1420047,

	SAY_NAGA_CASTER_AGGRO_1  = -1420048,
	SAY_NAGA_CASTER_AGGRO_2  = -1420049,
	SAY_NAGA_CASTER_AGGRO_3  = -1420050,
};

class npc_bogstrok : public CreatureScript
{
public:
    npc_bogstrok() : CreatureScript("npc_bogstrok") {}

    struct npc_bogstrokAI : public QuantumBasicAI
    {
        npc_bogstrokAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 PercingJabTimer;

        void Reset()
		{
			PercingJabTimer = 2000;

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

			if (PercingJabTimer <= diff)
			{
				DoCastVictim(SPELL_PIERCING_JAB);
				PercingJabTimer = 4000;
			}
			else PercingJabTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bogstrokAI(creature);
    }
};

class npc_greater_bogstrok : public CreatureScript
{
public:
    npc_greater_bogstrok() : CreatureScript("npc_greater_bogstrok") {}

    struct npc_greater_bogstrokAI : public QuantumBasicAI
    {
        npc_greater_bogstrokAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 DecayedStrengthTimer;

        void Reset()
		{
			DecayedStrengthTimer = 1000;

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

			if (DecayedStrengthTimer <= diff)
			{
				DoCastVictim(SPELL_DECAYED_STRENGTH);
				DecayedStrengthTimer = 5000;
			}
			else DecayedStrengthTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_greater_bogstrokAI(creature);
    }
};

class npc_wastewalker_worker : public CreatureScript
{
public:
    npc_wastewalker_worker() : CreatureScript("npc_wastewalker_worker") {}

    struct npc_wastewalker_workerAI : public QuantumBasicAI
    {
        npc_wastewalker_workerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 RendTimer;

        void Reset()
		{
			RendTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_WORK_MINING);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

		void JustDied(Unit* killer)
		{
			if (killer->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_WASTEWALKER_DEATH_1, SAY_WASTEWALKER_DEATH_2, SAY_WASTEWALKER_DEATH_3, SAY_WASTEWALKER_DEATH_4,
				SAY_WASTEWALKER_DEATH_5, SAY_WASTEWALKER_DEATH_6, SAY_WASTEWALKER_DEATH_7), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RendTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_REND_5N, SPELL_REND_5H));
				RendTimer = 6000;
			}
			else RendTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wastewalker_workerAI(creature);
    }
};

class npc_wastewalker_slave : public CreatureScript
{
public:
    npc_wastewalker_slave() : CreatureScript("npc_wastewalker_slave") {}

    struct npc_wastewalker_slaveAI : public QuantumBasicAI
    {
        npc_wastewalker_slaveAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FrostboltTimer;
		uint32 FrostNovaTimer;

        void Reset()
		{
			FrostboltTimer = 500;
			FrostNovaTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_WORK);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (killer->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_WASTEWALKER_DEATH_1, SAY_WASTEWALKER_DEATH_2, SAY_WASTEWALKER_DEATH_3, SAY_WASTEWALKER_DEATH_4,
				SAY_WASTEWALKER_DEATH_5, SAY_WASTEWALKER_DEATH_6, SAY_WASTEWALKER_DEATH_7), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Elemental Armor Buff Check in Combat
			if (!me->HasAuraEffect(SPELL_ELEMENTAL_ARMOR, 0))
				DoCast(me, SPELL_ELEMENTAL_ARMOR);

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FROSTBOLT_5N, SPELL_FROSTBOLT_5H));
					FrostboltTimer = urand(3000, 4000);
				}
			}
			else FrostboltTimer -= diff;

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
        return new npc_wastewalker_slaveAI(creature);
    }
};

class npc_coilfang_slavehandler : public CreatureScript
{
public:
    npc_coilfang_slavehandler() : CreatureScript("npc_coilfang_slavehandler") {}

    struct npc_coilfang_slavehandlerAI : public QuantumBasicAI
    {
        npc_coilfang_slavehandlerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SayWorkTimer;
		uint32 HamstringTimer;
		uint32 SlapTimer;
		uint32 HeadCrackTimer;

        void Reset()
		{
			SayWorkTimer = 1000;
			HamstringTimer = 2000;
			SlapTimer = 3000;
			HeadCrackTimer = 4000;

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
			// Out of Combat Timer
			if (SayWorkTimer <= diff && !me->IsInCombatActive())
			{
				DoSendQuantumText(RAND(SAY_SLAVEHANDLER_WORK_1, SAY_SLAVEHANDLER_WORK_2, SAY_SLAVEHANDLER_WORK_3, SAY_SLAVEHANDLER_WORK_4,
				SAY_SLAVEHANDLER_WORK_5, SAY_SLAVEHANDLER_WORK_6, SAY_SLAVEHANDLER_WORK_7), me);

				SayWorkTimer = 120000;
			}
			else SayWorkTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HamstringTimer <= diff)
			{
				DoCastVictim(SPELL_HAMSTRING);
				HamstringTimer = 4000;
			}
			else HamstringTimer -= diff;

			if (SlapTimer <= diff)
			{
				DoCastVictim(SPELL_SLAP);
				SlapTimer = 6000;
			}
			else SlapTimer -= diff;

			if (HeadCrackTimer <= diff)
			{
				DoCastVictim(SPELL_HEAD_CRACK);
				HeadCrackTimer = 8000;
			}
			else HeadCrackTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_slavehandlerAI(creature);
    }
};

class npc_coilfang_observer : public CreatureScript
{
public:
    npc_coilfang_observer() : CreatureScript("npc_coilfang_observer") {}

    struct npc_coilfang_observerAI : public QuantumBasicAI
    {
        npc_coilfang_observerAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 HeavyDynamiteTimer;
		uint32 ImmolateTimer;

		SummonList Summons;

        void Reset()
		{
			HeavyDynamiteTimer = 1000;
			ImmolateTimer = 3000;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_NAGA_AGGRO_1, SAY_NAGA_AGGRO_2, SAY_NAGA_AGGRO_3, SAY_NAGA_AGGRO_4, SAY_NAGA_AGGRO_5), me);

			me->SummonCreature(NPC_COILFANG_RAY, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_COILFANG_RAY)
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

			if (HeavyDynamiteTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_HEAVY_DYNAMITE_5N, SPELL_HEAVY_DYNAMITE_5H));
					HeavyDynamiteTimer = urand(3000, 4000);
				}
			}
			else HeavyDynamiteTimer -= diff;

			if (ImmolateTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_IMMOLATE_5N, SPELL_IMMOLATE_5H));
					ImmolateTimer = urand(5000, 6000);
				}
			}
			else ImmolateTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_observerAI(creature);
    }
};

class npc_coilfang_tempest : public CreatureScript
{
public:
    npc_coilfang_tempest() : CreatureScript("npc_coilfang_tempest") {}

    struct npc_coilfang_tempestAI : public QuantumBasicAI
    {
        npc_coilfang_tempestAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 DeadlyPoisonTimer;
		uint32 SinisterStrikeTimer;

        void Reset()
		{
			DeadlyPoisonTimer = 2000;
			SinisterStrikeTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_NAGA_AGGRO_1, SAY_NAGA_AGGRO_2, SAY_NAGA_AGGRO_3, SAY_NAGA_AGGRO_4, SAY_NAGA_AGGRO_5), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DeadlyPoisonTimer <= diff)
			{
				DoCastVictim(SPELL_DEADLY_POISON);
				DeadlyPoisonTimer = urand(3000, 4000);
			}
			else DeadlyPoisonTimer -= diff;

			if (SinisterStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_SINISTER_STRIKE);
				SinisterStrikeTimer = urand(5000, 6000);
			}
			else SinisterStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_tempestAI(creature);
    }
};

class npc_coilfang_scale_healer : public CreatureScript
{
public:
    npc_coilfang_scale_healer() : CreatureScript("npc_coilfang_scale_healer") {}

    struct npc_coilfang_scale_healerAI : public QuantumBasicAI
    {
        npc_coilfang_scale_healerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 PowerWordShieldTimer;
		uint32 HolyNovaTimer;
		uint32 HealTimer;

        void Reset()
		{
			PowerWordShieldTimer = 500;
			HolyNovaTimer = 2000;
			HealTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_NAGA_AGGRO_1, SAY_NAGA_AGGRO_2, SAY_NAGA_AGGRO_3, SAY_NAGA_AGGRO_4, SAY_NAGA_AGGRO_5,
				SAY_NAGA_CASTER_AGGRO_1, SAY_NAGA_CASTER_AGGRO_2, SAY_NAGA_CASTER_AGGRO_3), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PowerWordShieldTimer <= diff)
			{
				DoCast(me, DUNGEON_MODE(SPELL_POWER_WORD_SHIELD_5N, SPELL_POWER_WORD_SHIELD_5H));
				PowerWordShieldTimer = urand(4000, 5000);
			}
			else PowerWordShieldTimer -= diff;

			if (HolyNovaTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_HOLY_NOVA_5N, SPELL_HOLY_NOVA_5H));
				HolyNovaTimer = urand(6000, 7000);
			}
			else HolyNovaTimer -= diff;

			if (HealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, DUNGEON_MODE(SPELL_HEAL_5N, SPELL_HEAL_5H));
					HealTimer = urand(8000, 9000);
				}
			}
			else HealTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
				DoCast(me, DUNGEON_MODE(SPELL_HEAL_5N, SPELL_HEAL_5H));

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_scale_healerAI(creature);
    }
};

class npc_coilfang_ray : public CreatureScript
{
public:
    npc_coilfang_ray() : CreatureScript("npc_coilfang_ray") {}

    struct npc_coilfang_rayAI : public QuantumBasicAI
    {
        npc_coilfang_rayAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 PsychicHorrorTimer;

        void Reset()
		{
			PsychicHorrorTimer = 1000;

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

			if (PsychicHorrorTimer <= diff)
			{
				DoCastVictim(SPELL_PSYCHIC_HORROR);
				PsychicHorrorTimer = 4000;
			}
			else PsychicHorrorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_rayAI(creature);
    }
};

class npc_coilfang_enchantress : public CreatureScript
{
public:
    npc_coilfang_enchantress() : CreatureScript("npc_coilfang_enchantress") {}

    struct npc_coilfang_enchantressAI : public QuantumBasicAI
    {
        npc_coilfang_enchantressAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 EntanglingRootsTimer;
		uint32 LightningBoltTimer;
		uint32 LightningCloudTimer;

        void Reset()
		{
			EntanglingRootsTimer = 1000;
			LightningBoltTimer = 3000;
			LightningCloudTimer = 5000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_NAGA_AGGRO_1, SAY_NAGA_AGGRO_2, SAY_NAGA_AGGRO_3, SAY_NAGA_AGGRO_4, SAY_NAGA_AGGRO_5), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (EntanglingRootsTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ENTANGLING_ROOTS);
					EntanglingRootsTimer = urand(3000, 4000);
				}
			}
			else EntanglingRootsTimer -= diff;

			if (LightningBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_LIGHTNING_BOLT_5N, SPELL_LIGHTNING_BOLT_5H));
					LightningBoltTimer = urand(5000, 6000);
				}
			}
			else LightningBoltTimer -= diff;

			if (LightningCloudTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_LIGHTNING_CLOUD_5N, SPELL_LIGHTNING_CLOUD_5H), true);
					LightningCloudTimer = urand(7000, 8000);
				}
			}
			else LightningCloudTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_enchantressAI(creature);
    }
};

class npc_coilfang_soothsayer : public CreatureScript
{
public:
    npc_coilfang_soothsayer() : CreatureScript("npc_coilfang_soothsayer") {}

    struct npc_coilfang_soothsayerAI : public QuantumBasicAI
    {
        npc_coilfang_soothsayerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ArcaneMissiliesTimer;
		uint32 DecayedIntellectTimer;
		uint32 DominationTimer;

        void Reset()
		{
			ArcaneMissiliesTimer = 1000;
			DecayedIntellectTimer = 3000;
			DominationTimer = 5000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_NAGA_AGGRO_1, SAY_NAGA_AGGRO_2, SAY_NAGA_AGGRO_3, SAY_NAGA_AGGRO_4, SAY_NAGA_AGGRO_5), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ArcaneMissiliesTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ARCANE_MISSILIES);
					ArcaneMissiliesTimer = urand(3000, 4000);
				}
			}
			else ArcaneMissiliesTimer -= diff;

			if (DecayedIntellectTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DECAYED_INTELLECT);
					DecayedIntellectTimer = urand(5000, 6000);
				}
			}
			else DecayedIntellectTimer -= diff;

			if (DominationTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
					DoCast(target, SPELL_DOMINATION);
					DominationTimer = urand(7000, 8000);
				}
			}
			else DominationTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_soothsayerAI(creature);
    }
};

class npc_coilfang_defender : public CreatureScript
{
public:
    npc_coilfang_defender() : CreatureScript("npc_coilfang_defender") {}

    struct npc_coilfang_defenderAI : public QuantumBasicAI
    {
        npc_coilfang_defenderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SlowTimer;
		uint32 ShieldSlamTimer;
		uint32 SpellReflectionTimer;

        void Reset()
		{
			SlowTimer = 1000;
			ShieldSlamTimer = 2000;
			SpellReflectionTimer = 3000;

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

			if (SlowTimer <= diff)
			{
				DoCastVictim(SPELL_SLOW);
				SlowTimer = 4000;
			}
			else SlowTimer -= diff;

			if (ShieldSlamTimer <= diff)
			{
				DoCastVictim(SPELL_SHIELD_SLAM);
				ShieldSlamTimer = 6000;
			}
			else ShieldSlamTimer -= diff;

			if (SpellReflectionTimer <= diff)
			{
				DoCast(me, SPELL_SPELL_REFLECTION);
				SpellReflectionTimer = 10000;
			}
			else SpellReflectionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_defenderAI(creature);
    }
};

class npc_coilfang_technician : public CreatureScript
{
public:
    npc_coilfang_technician() : CreatureScript("npc_coilfang_technician") {}

    struct npc_coilfang_technicianAI : public QuantumBasicAI
    {
        npc_coilfang_technicianAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 RainOfFireTimer;

        void Reset()
		{
			RainOfFireTimer = 1000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_NAGA_AGGRO_1, SAY_NAGA_AGGRO_2, SAY_NAGA_AGGRO_3, SAY_NAGA_AGGRO_4, SAY_NAGA_AGGRO_5), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RainOfFireTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_RAIN_OF_FIRE_5N, SPELL_RAIN_OF_FIRE_5H));
					RainOfFireTimer = urand(4000, 5000);
				}
			}
			else RainOfFireTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_technicianAI(creature);
    }
};

class npc_coilfang_collaborator : public CreatureScript
{
public:
    npc_coilfang_collaborator() : CreatureScript("npc_coilfang_collaborator") {}

    struct npc_coilfang_collaboratorAI : public QuantumBasicAI
    {
        npc_coilfang_collaboratorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 CrippleTimer;
		uint32 RevengeTimer;

        void Reset()
		{
			CrippleTimer = 1000;
			RevengeTimer = 3000;

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

			if (CrippleTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CRIPPLE, true);
					CrippleTimer = 4000;
				}
			}
			else CrippleTimer -= diff;

			if (RevengeTimer <= diff)
			{
				DoCastVictim(SPELL_REVENGE);
				RevengeTimer = 6000;
			}
			else RevengeTimer -= diff;

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
        return new npc_coilfang_collaboratorAI(creature);
    }
};

class npc_coilfang_champion : public CreatureScript
{
public:
    npc_coilfang_champion() : CreatureScript("npc_coilfang_champion") {}

    struct npc_coilfang_championAI : public QuantumBasicAI
    {
        npc_coilfang_championAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SunderArmorTimer;
		uint32 CleaveTimer;
		uint32 FrighteningShoutTimer;

        void Reset()
		{
			SunderArmorTimer = 1000;
			CleaveTimer = 2000;
			FrighteningShoutTimer = 3000;

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

			if (SunderArmorTimer <= diff)
			{
				DoCastVictim(SPELL_SUNDER_ARMOR);
				SunderArmorTimer = 3000;
			}
			else SunderArmorTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = 5000;
			}
			else CleaveTimer -= diff;

			if (FrighteningShoutTimer <= diff)
			{
				DoCastAOE(SPELL_FRIGHTENING_SHOUT);
				FrighteningShoutTimer = 7000;
			}
			else FrighteningShoutTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilfang_championAI(creature);
    }
};

void AddSC_slave_pens_trash()
{
	new npc_bogstrok();
	new npc_greater_bogstrok();
	new npc_wastewalker_worker();
	new npc_wastewalker_slave();
	new npc_coilfang_slavehandler();
	new npc_coilfang_observer();
	new npc_coilfang_tempest();
	new npc_coilfang_scale_healer();
	new npc_coilfang_ray();
	new npc_coilfang_enchantress();
	new npc_coilfang_soothsayer();
	new npc_coilfang_defender();
	new npc_coilfang_technician();
	new npc_coilfang_collaborator();
	new npc_coilfang_champion();
}