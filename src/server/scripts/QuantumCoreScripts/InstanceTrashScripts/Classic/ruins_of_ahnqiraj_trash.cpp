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
#include "../../../scripts/Kalimdor/RuinsOfAhnQiraj/ruins_of_ahnqiraj.h"

enum RuinsOfAhnqirajSpells
{
	SPELL_THRASH               = 3417,
	SPELL_DEAFENING_ROAR       = 3589,
	SPELL_TRAMPLE              = 5568,
	SPELL_VENOM_STING          = 8257,
	SPELL_FRENZY               = 8269,
	SPELL_ENRAGE               = 8599,
	SPELL_UPPERCUT             = 10966,
	SPELL_WEB_SPRAY            = 12252,
	SPELL_THUNDERCLAP          = 15588,
	SPELL_MORTAL_STRIKE        = 16856,
	SPELL_VENGEANCE            = 25164,
	SPELL_RETALIATION          = 22857,
	SPELL_VENOM_SPIT           = 25497,
	SPELL_ITCH                 = 25185,
	SPELL_HIVE_ZARA_CATALYST   = 25187,
	SPELL_POISON               = 25645,
	SPELL_TAIL_LASH            = 25654,
	SPELL_SAND_TRAP            = 25648,
	SPELL_SAND_TRAP_PROC       = 25656,
	SPELL_SUNDERING_CLEAVE     = 25174,
	SPELL_CLEAVE               = 26350,
	SPELL_PLAGUE               = 22997,
	SPELL_EXPLODE              = 25698,
	SPELL_SUMMON_PLAYER        = 20477,
	SPELL_CAPTAIN_CLEAVE       = 40504,
	SPELL_CAPTAIN_SUNDER_ARMOR = 24317,
	SPELL_ATTACK_ORDER         = 25471,
	SPELL_LIGHTNING_CLOUD      = 26550,
	SPELL_SHOCKWAVE            = 25425,
	SPELL_SWEEPING_SLAM        = 25322,
	SPELL_ENLARGE              = 25462,
	SPELL_BURROW               = 26381,
	SPELL_ARCANE_EXPLOSION     = 25679,
	SPELL_PURGE                = 25756,
	SPELL_CONSUME              = 25371,
	SPELL_POISON_BOLT          = 25424,
};

class npc_qiraji_gladiator : public CreatureScript
{
public:
    npc_qiraji_gladiator() : CreatureScript("npc_qiraji_gladiator") {}

    struct npc_qiraji_gladiatorAI : public QuantumBasicAI
    {
        npc_qiraji_gladiatorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_CONFUSE, true);
		}

		uint32 TrampleTimer;
		uint32 UppercutTimer;
		uint32 VengeanceTimer;

        void Reset()
		{
			TrampleTimer = 2*IN_MILLISECONDS;
			UppercutTimer = 4*IN_MILLISECONDS;
			VengeanceTimer = 6*IN_MILLISECONDS;

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

			if (TrampleTimer <= diff)
			{
				DoCastAOE(SPELL_TRAMPLE);
				TrampleTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else TrampleTimer -= diff;

			if (UppercutTimer <= diff)
			{
				DoCastVictim(SPELL_UPPERCUT);
				UppercutTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else UppercutTimer -= diff;

			if (VengeanceTimer <= diff)
			{
				DoCast(me, SPELL_VENGEANCE);
				VengeanceTimer = 35*IN_MILLISECONDS;
			}
			else VengeanceTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_qiraji_gladiatorAI(creature);
    }
};

class npc_qiraji_swarmguard : public CreatureScript
{
public:
    npc_qiraji_swarmguard() : CreatureScript("npc_qiraji_swarmguard") {}

    struct npc_qiraji_swarmguardAI : public QuantumBasicAI
    {
        npc_qiraji_swarmguardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_CONFUSE, true);
		}

		uint32 SunderingCleaveTimer;

        void Reset()
		{
			SunderingCleaveTimer = 2*IN_MILLISECONDS;

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

			if (SunderingCleaveTimer <= diff)
			{
				DoCastVictim(SPELL_SUNDERING_CLEAVE);
				SunderingCleaveTimer = 4*IN_MILLISECONDS;
			}
			else SunderingCleaveTimer -= diff;

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
        return new npc_qiraji_swarmguardAI(creature);
    }
};

class npc_qiraji_warrior : public CreatureScript
{
public:
    npc_qiraji_warrior() : CreatureScript("npc_qiraji_warrior") {}

    struct npc_qiraji_warriorAI : public QuantumBasicAI
    {
        npc_qiraji_warriorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_CONFUSE, true);
		}

		uint32 TrampleTimer;
		uint32 UppercutTimer;
		uint32 ThunderclapTimer;

        void Reset()
		{
			TrampleTimer = 2*IN_MILLISECONDS;
			UppercutTimer = 4*IN_MILLISECONDS;
			ThunderclapTimer = 6*IN_MILLISECONDS;

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

			if (TrampleTimer <= diff)
			{
				DoCastAOE(SPELL_TRAMPLE);
				TrampleTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else TrampleTimer -= diff;

			if (UppercutTimer <= diff)
			{
				DoCastVictim(SPELL_UPPERCUT);
				UppercutTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else UppercutTimer -= diff;

			if (ThunderclapTimer <= diff)
			{
				DoCastAOE(SPELL_THUNDERCLAP);
				ThunderclapTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else ThunderclapTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_qiraji_warriorAI(creature);
    }
};

class npc_hive_zara_stinger : public CreatureScript
{
public:
    npc_hive_zara_stinger() : CreatureScript("npc_hive_zara_stinger") { }

    struct npc_hive_zara_stingerAI : public QuantumBasicAI
    {
        npc_hive_zara_stingerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_CONFUSE, true);
		}

		uint32 RetaliationTimer;
		uint32 VenomSpitTimer;

        void Reset()
        {
			RetaliationTimer = 0.5*IN_MILLISECONDS;
			VenomSpitTimer = 1.5*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

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

			if (RetaliationTimer <= diff)
			{
				DoCast(me, SPELL_RETALIATION);
				RetaliationTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else RetaliationTimer -= diff;

			if (VenomSpitTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_VENOM_SPIT);
					VenomSpitTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else VenomSpitTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hive_zara_stingerAI(creature);
    }
};

class npc_hive_zara_wasp : public CreatureScript
{
public:
    npc_hive_zara_wasp() : CreatureScript("npc_hive_zara_wasp") { }

    struct npc_hive_zara_waspAI : public QuantumBasicAI
    {
        npc_hive_zara_waspAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_CONFUSE, true);
		}

		uint32 HiveZaraCatalystTimer;
		uint32 ItchTimer;

        void Reset()
        {
			HiveZaraCatalystTimer = 0.5*IN_MILLISECONDS;
			ItchTimer = 1.5*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

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

			if (HiveZaraCatalystTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_VENOM_STING, true);
					DoCast(target, SPELL_HIVE_ZARA_CATALYST, true);
					HiveZaraCatalystTimer = 10*IN_MILLISECONDS;
				}
			}
			else HiveZaraCatalystTimer -= diff;

			if (ItchTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ITCH);
					ItchTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ItchTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hive_zara_waspAI(creature);
    }
};

class npc_hive_zara_tail_lasher : public CreatureScript
{
public:
    npc_hive_zara_tail_lasher() : CreatureScript("npc_hive_zara_tail_lasher") {}

    struct npc_hive_zara_tail_lasherAI : public QuantumBasicAI
    {
        npc_hive_zara_tail_lasherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_CONFUSE, true);
		}

		uint32 TailLashTimer;
		uint32 PoisonTimer;

        void Reset()
		{
			TailLashTimer = 2*IN_MILLISECONDS;
			PoisonTimer = 4*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

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

			if (TailLashTimer <= diff)
			{
				DoCastAOE(SPELL_TAIL_LASH);
				TailLashTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else TailLashTimer -= diff;

			if (PoisonTimer <= diff)
			{
				DoCastVictim(SPELL_POISON);
				PoisonTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else PoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hive_zara_tail_lasherAI(creature);
    }
};

class npc_hive_zara_hornet : public CreatureScript
{
public:
    npc_hive_zara_hornet() : CreatureScript("npc_hive_zara_hornet") {}

    struct npc_hive_zara_hornetAI : public QuantumBasicAI
    {
        npc_hive_zara_hornetAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_CONFUSE, true);
		}

		uint32 SandTrapTimer;
		uint32 PoisonTimer;

        void Reset()
		{
			SandTrapTimer = 0.5*IN_MILLISECONDS;
			PoisonTimer = 2*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

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

			if (SandTrapTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
					if (Creature* trap = me->SummonCreature(NPC_SAND_TRAP_TRIGGER, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 20*IN_MILLISECONDS))
						trap->SetLevel(target->GetCurrentLevel());
				}
				SandTrapTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else SandTrapTimer -= diff;

			if (PoisonTimer <= diff)
			{
				DoCastVictim(SPELL_POISON);
				PoisonTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else PoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hive_zara_hornetAI(creature);
    }
};

class npc_hive_zara_sandstalker : public CreatureScript
{
public:
    npc_hive_zara_sandstalker() : CreatureScript("npc_hive_zara_sandstalker") {}

    struct npc_hive_zara_sandstalkerAI : public QuantumBasicAI
    {
        npc_hive_zara_sandstalkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_CONFUSE, true);
		}

		uint32 BurrowTimer;

        void Reset()
		{
			BurrowTimer = 0.5*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

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

			if (BurrowTimer <= diff)
			{
				DoCast(me, SPELL_BURROW);
				BurrowTimer = 5*IN_MILLISECONDS;
			}
			else BurrowTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hive_zara_sandstalkerAI(creature);
    }
};

class npc_hive_zara_collector : public CreatureScript
{
public:
    npc_hive_zara_collector() : CreatureScript("npc_hive_zara_collector") {}

    struct npc_hive_zara_collectorAI : public QuantumBasicAI
    {
        npc_hive_zara_collectorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_CONFUSE, true);
		}

		uint32 DeafeningRoarTimer;
		uint32 WebSprayTimer;

        void Reset()
		{
			DeafeningRoarTimer = 1*IN_MILLISECONDS;
			WebSprayTimer = 2*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

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

			if (DeafeningRoarTimer <= diff)
			{
				DoCastAOE(SPELL_DEAFENING_ROAR);
				DeafeningRoarTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else DeafeningRoarTimer -= diff;

			if (WebSprayTimer <= diff)
			{
				DoCast(SPELL_WEB_SPRAY);
				WebSprayTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else WebSprayTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hive_zara_collectorAI(creature);
    }
};

class npc_kaldorei_elite : public CreatureScript
{
public:
    npc_kaldorei_elite() : CreatureScript("npc_kaldorei_elite") {}

    struct npc_kaldorei_eliteAI : public QuantumBasicAI
    {
        npc_kaldorei_eliteAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_CONFUSE, true);
		}

		uint32 MortalStrikeTimer;
		uint32 CleaveTimer;

        void Reset()
		{
			MortalStrikeTimer = 2*IN_MILLISECONDS;
			CleaveTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
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

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_MORTAL_STRIKE);
				MortalStrikeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else MortalStrikeTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_kaldorei_eliteAI(creature);
    }
};

class npc_anubisath_guardian : public CreatureScript
{
public:
    npc_anubisath_guardian() : CreatureScript("npc_anubisath_guardian") {}

    struct npc_anubisath_guardianAI : public QuantumBasicAI
    {
        npc_anubisath_guardianAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_CONFUSE, true);
		}

		uint32 PlagueTimer;

		bool Exploded;

        void Reset()
		{
			PlagueTimer = 0.5*IN_MILLISECONDS;

			Exploded = false;

			me->SetWalk(true);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

		void JustDied(Unit* /*killer*/)
		{
			me->SetLevel(62);
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PlagueTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_PLAGUE);
					PlagueTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else PlagueTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_30))
            {
				if (!me->HasAuraEffect(SPELL_FRENZY, 0))
				{
					DoCast(me, SPELL_FRENZY);
					DoSendQuantumText(SAY_GENERIC_EMOTE_FRENZY, me);
				}
            }

			if (HealthBelowPct(HEALTH_PERCENT_10 && !Exploded))
			{
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
				me->SetLevel(me->GetVictim()->GetCurrentLevel());
				me->SetReactState(REACT_PASSIVE);
				me->RemoveAllAuras();
				DoCast(me, SPELL_EXPLODE);
				Exploded = true;
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anubisath_guardianAI(creature);
    }
};

class npc_mana_fiend : public CreatureScript
{
public:
    npc_mana_fiend() : CreatureScript("npc_mana_fiend") {}

    struct npc_mana_fiendAI : public QuantumBasicAI
    {
        npc_mana_fiendAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_CONFUSE, true);
		}

		uint32 ArcaneExplosionTimer;

        void Reset()
		{
			ArcaneExplosionTimer = 2*IN_MILLISECONDS;

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

			if (ArcaneExplosionTimer <= diff)
			{
				DoCastAOE(SPELL_ARCANE_EXPLOSION);
				ArcaneExplosionTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else ArcaneExplosionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mana_fiendAI(creature);
    }
};

class npc_obsidian_destroyer : public CreatureScript
{
public:
    npc_obsidian_destroyer() : CreatureScript("npc_obsidian_destroyer") {}

    struct npc_obsidian_destroyerAI : public QuantumBasicAI
    {
        npc_obsidian_destroyerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_CONFUSE, true);
		}

		uint32 PurgeTimer;

        void Reset()
		{
			PurgeTimer = 2*IN_MILLISECONDS;

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

			if (PurgeTimer <= diff)
			{
				DoCastAOE(SPELL_PURGE, true);
				PurgeTimer = 8*IN_MILLISECONDS;
			}
			else PurgeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_obsidian_destroyerAI(creature);
    }
};

class npc_flesh_hunter : public CreatureScript
{
public:
    npc_flesh_hunter() : CreatureScript("npc_flesh_hunter") {}

    struct npc_flesh_hunterAI : public QuantumBasicAI
    {
        npc_flesh_hunterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_CONFUSE, true);
		}

		uint32 PoisonBoltTimer;
		uint32 ConsumeTimer;

        void Reset()
		{
			PoisonBoltTimer = 0.5*IN_MILLISECONDS;
			ConsumeTimer = 2*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_THRASH);
		}

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
					DoCast(target, SPELL_POISON_BOLT, true);
					PoisonBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else PoisonBoltTimer -= diff;

			if (ConsumeTimer <= diff)
			{
				DoCastVictim(SPELL_CONSUME);
				ConsumeTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else ConsumeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_flesh_hunterAI(creature);
    }
};

void AddSC_ruins_of_ahnqiraj_trash()
{
	new npc_qiraji_gladiator();
	new npc_qiraji_swarmguard();
	new npc_qiraji_warrior();
	new npc_hive_zara_stinger();
	new npc_hive_zara_wasp();
	new npc_hive_zara_tail_lasher();
	new npc_hive_zara_hornet();
	new npc_hive_zara_sandstalker();
	new npc_hive_zara_collector();
	new npc_kaldorei_elite();
	new npc_anubisath_guardian();
	new npc_mana_fiend();
	new npc_obsidian_destroyer();
	new npc_flesh_hunter();
}