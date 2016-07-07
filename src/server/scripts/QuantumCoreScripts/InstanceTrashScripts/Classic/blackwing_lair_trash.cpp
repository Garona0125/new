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

enum BlackwingLairSpells
{
	SPELL_CLEAVE              = 15284,
	SPELL_STRIKE              = 15580,
	SPELL_DRAGONBANE          = 23967,
	SPELL_SLOW                = 13747,
	SPELL_ARCANE_MISSILES     = 22272,
	SPELL_GREATER_POLYMORPH   = 22274,
	SPELL_SHADOW_SHOCK        = 17289,
	SPELL_HEALING_CIRCLE      = 22458,
	SPELL_GROWING_FLAMES      = 22442,
	SPELL_FLAMESTRIKE         = 22275,
	SPELL_BERSERKER_CHARGE    = 16636,
	SPELL_FLAME_SHOCK         = 22423,
	SPELL_BROOD_POWER_BRONZE  = 22642,
	SPELL_BROOD_POWER_RED     = 22558,
	SPELL_BROOD_POWER_BLUE    = 22559,
	SPELL_BROOD_POWER_BLACK   = 22560,
	SPELL_BROOD_POWER_GREEN   = 22561,
	SPELL_DOUBLE_ATTACK       = 19818,
	SPELL_FLAME_BUFFET        = 22433,
	SPELL_ENRAGE              = 22428,
	SPELL_FIREBALL            = 17290,
	SPELL_ARCANE_EXPLOSION    = 22271,
	SPELL_AURA_OF_FLAMES      = 22436,
	SPELL_DT_CLEAVE           = 15496,
	SPELL_INTIMIDATING_ORDERS = 22440,
	SPELL_MARK_OF_DETONATION  = 22438,
	SPELL_FIREBALL_VOLLEY     = 22425,
	SPELL_BLAST_WAVE          = 22424,
	SPELL_FLAME_BLAST         = 20623,
	SPELL_DEMON_PORTAL        = 22372,
	SPELL_SHADOW_BOLT         = 22336,
	SPELL_RAIN_OF_FIRE        = 19717,
	SPELL_SUMMON_FELGUARDS    = 22392,
	SPELL_SUMMON_INFERNALS    = 23426,
	SPELL_IMMOLATION          = 12744,
	SPELL_INFERNAL_AWAKENING  = 22703,
	SPELL_FELGUARD_SPAWN      = 22393,
	SPELL_EFG_BLAST_WAVE      = 16046,
	SPELL_EIF_STRIKE          = 15580,
	SPELL_THUNDERCLAP         = 15548,
	SPELL_FRENZY              = 8269,
	SPELL_BOMB                = 22334,
	SPELL_BOTTLE_OF_POISON    = 22335,
	SPELL_WAR_STOMP           = 24375,
};

enum Creatures
{
	NPC_DEMON_PORTAL       = 14081,
	NPC_ENRAGED_FELGUARD   = 14101,
	NPC_CORRUPTED_INFERNAL = 14668,
};

enum Texts
{
	SAY_GRETHOK_DEATH = -1420108,
};

class npc_blackwing_technician : public CreatureScript
{
public:
    npc_blackwing_technician() : CreatureScript("npc_blackwing_technician") {}

    struct npc_blackwing_technicianAI : public QuantumBasicAI
    {
        npc_blackwing_technicianAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 BombTimer;
		uint32 BottleOfPoisonTimer;

        void Reset()
		{
			BombTimer = 0.5*IN_MILLISECONDS;
			BottleOfPoisonTimer = 3*IN_MILLISECONDS;

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

			if (BombTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BOMB);
					BombTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else BombTimer -= diff;

			if (BottleOfPoisonTimer <= diff)
			{
				DoCastVictim(SPELL_BOTTLE_OF_POISON);
				BottleOfPoisonTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else BottleOfPoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_blackwing_technicianAI(creature);
    }
};

class npc_blackwing_guardsman : public CreatureScript
{
public:
    npc_blackwing_guardsman() : CreatureScript("npc_blackwing_guardsman") {}

    struct npc_blackwing_guardsmanAI : public QuantumBasicAI
    {
        npc_blackwing_guardsmanAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 CleaveTimer;
		uint32 StrikeTimer;
		uint32 DragonbaneTimer;

        void Reset()
		{
			CleaveTimer = 2*IN_MILLISECONDS;
			StrikeTimer = 3*IN_MILLISECONDS;
			DragonbaneTimer = 4*IN_MILLISECONDS;

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

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_STRIKE);
				StrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else StrikeTimer -= diff;
			
			if (DragonbaneTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				DragonbaneTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else DragonbaneTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_blackwing_guardsmanAI(creature);
    }
};

class npc_blackwing_legionnaire : public CreatureScript
{
public:
    npc_blackwing_legionnaire() : CreatureScript("npc_blackwing_legionnaire") {}

    struct npc_blackwing_legionnaireAI : public QuantumBasicAI
    {
        npc_blackwing_legionnaireAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 CleaveTimer;
		uint32 StrikeTimer;
		uint32 DragonbaneTimer;

        void Reset()
		{
			CleaveTimer = 2*IN_MILLISECONDS;
			StrikeTimer = 3*IN_MILLISECONDS;
			DragonbaneTimer = 4*IN_MILLISECONDS;

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

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_STRIKE);
				StrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else StrikeTimer -= diff;
			
			if (DragonbaneTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				DragonbaneTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else DragonbaneTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_blackwing_legionnaireAI(creature);
    }
};

class npc_blackwing_taskmaster : public CreatureScript
{
public:
    npc_blackwing_taskmaster() : CreatureScript("npc_blackwing_taskmaster") {}

    struct npc_blackwing_taskmasterAI : public QuantumBasicAI
    {
        npc_blackwing_taskmasterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 ShadowShockTimer;
		uint32 HealingCircleTimer;

        void Reset()
		{
			ShadowShockTimer = 0.5*IN_MILLISECONDS;
			HealingCircleTimer = 2*IN_MILLISECONDS;

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

			if (ShadowShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_SHOCK);
					ShadowShockTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowShockTimer -= diff;

			if (HealingCircleTimer <= diff)
			{
				DoCast(me, SPELL_HEALING_CIRCLE);
				HealingCircleTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else HealingCircleTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_blackwing_taskmasterAI(creature);
    }
};

class npc_blackwing_mage : public CreatureScript
{
public:
    npc_blackwing_mage() : CreatureScript("npc_blackwing_mage") {}

    struct npc_blackwing_mageAI : public QuantumBasicAI
    {
        npc_blackwing_mageAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 FireballTimer;
		uint32 ArcaneExplosionTimer;

        void Reset()
		{
			FireballTimer = 0.5*IN_MILLISECONDS;
			ArcaneExplosionTimer = 3*IN_MILLISECONDS;

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

			if (FireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FIREBALL);
					FireballTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else FireballTimer -= diff;
			
			if (ArcaneExplosionTimer <= diff)
			{
				DoCastAOE(SPELL_ARCANE_EXPLOSION);
				ArcaneExplosionTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else ArcaneExplosionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_blackwing_mageAI(creature);
    }
};

class npc_blackwing_spellbinder : public CreatureScript
{
public:
    npc_blackwing_spellbinder() : CreatureScript("npc_blackwing_spellbinder") {}

    struct npc_blackwing_spellbinderAI : public QuantumBasicAI
    {
        npc_blackwing_spellbinderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 FlamestrikeTimer;
		uint32 GreaterPolymorphTimer;

        void Reset()
		{
			FlamestrikeTimer = 0.5*IN_MILLISECONDS;
			GreaterPolymorphTimer = 3*IN_MILLISECONDS;

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

			if (FlamestrikeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FLAMESTRIKE);
					FlamestrikeTimer = 3*IN_MILLISECONDS;
				}
			}
			else FlamestrikeTimer -= diff;

			if (GreaterPolymorphTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
					DoCast(target, SPELL_GREATER_POLYMORPH);
					GreaterPolymorphTimer = 8*IN_MILLISECONDS;
				}
			}
			else GreaterPolymorphTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_blackwing_spellbinderAI(creature);
    }
};

class npc_blackwing_warlock : public CreatureScript
{
public:
    npc_blackwing_warlock() : CreatureScript("npc_blackwing_warlock"){}

    struct npc_blackwing_warlockAI : public QuantumBasicAI
    {
        npc_blackwing_warlockAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 ShadowBoltTimer;
		uint32 RainOfFireTimer;

		SummonList Summons;

        void Reset()
		{
			ShadowBoltTimer = 2*IN_MILLISECONDS;
			RainOfFireTimer = 4*IN_MILLISECONDS;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_DEMON_PORTAL);
			DoCast(me, SPELL_SUMMON_FELGUARDS, true);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			switch (summon->GetEntry())
			{
			    case NPC_DEMON_PORTAL:
				case NPC_ENRAGED_FELGUARD:
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

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_BOLT);
					ShadowBoltTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;
			
			if (RainOfFireTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_RAIN_OF_FIRE);
					RainOfFireTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else RainOfFireTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_blackwing_warlockAI(creature);
    }
};

class npc_demon_portal : public CreatureScript
{
public:
    npc_demon_portal() : CreatureScript("npc_demon_portal"){}

    struct npc_demon_portalAI : public QuantumBasicAI
    {
        npc_demon_portalAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_DISABLE_MOVE);
		}

        void Reset()
		{
			DoCast(me, SPELL_SUMMON_INFERNALS);
        }

        void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_demon_portalAI(creature);
    }
};

class npc_enraged_felguard : public CreatureScript
{
public:
    npc_enraged_felguard() : CreatureScript("npc_enraged_felguard") {}

    struct npc_enraged_felguardAI : public QuantumBasicAI
    {
        npc_enraged_felguardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 StrikeTimer;
		uint32 BlastWaveTimer;
		uint32 ThunderclapTimer;

        void Reset()
		{
			StrikeTimer = 2*IN_MILLISECONDS;
			BlastWaveTimer = 3*IN_MILLISECONDS;
			ThunderclapTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (Player* player = who->ToPlayer())
			{
				if (player->IsWithinDist(me, 200.0f))
				{
					me->AI()->EnterToBattle(player);
					me->AI()->AttackStart(player);
					me->AddThreat(player, 3.0f);
				}
			}
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
				DoCastVictim(SPELL_STRIKE);
				StrikeTimer = 4*IN_MILLISECONDS;
			}
			else StrikeTimer -= diff;

			if (BlastWaveTimer <= diff)
			{
				DoCastAOE(SPELL_EFG_BLAST_WAVE);
				BlastWaveTimer = 6*IN_MILLISECONDS;
			}
			else BlastWaveTimer -= diff;

			if (ThunderclapTimer <= diff)
			{
				DoCastAOE(SPELL_THUNDERCLAP);
				ThunderclapTimer = 8*IN_MILLISECONDS;
			}
			else ThunderclapTimer -= diff;

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
        return new npc_enraged_felguardAI(creature);
    }
};

class npc_corrupted_infernal : public CreatureScript
{
public:
    npc_corrupted_infernal() : CreatureScript("npc_corrupted_infernal") {}

    struct npc_corrupted_infernalAI : public QuantumBasicAI
    {
        npc_corrupted_infernalAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 ImmolationTimer;
		uint32 InfernalAwakeningTimer;

        void Reset()
		{
			ImmolationTimer = 2*IN_MILLISECONDS;
			InfernalAwakeningTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (Player* player = who->ToPlayer())
			{
				if (player->IsWithinDist(me, 200.0f))
				{
					me->AI()->EnterToBattle(player);
					me->AI()->AttackStart(player);
					me->AddThreat(player, 3.0f);
				}
			}
		}

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ImmolationTimer <= diff)
			{
				DoCastVictim(SPELL_IMMOLATION);
				ImmolationTimer = 4*IN_MILLISECONDS;
			}
			else ImmolationTimer -= diff;

			if (InfernalAwakeningTimer <= diff)
			{
				DoCastAOE(SPELL_INFERNAL_AWAKENING);
				InfernalAwakeningTimer = 6*IN_MILLISECONDS;
			}
			else InfernalAwakeningTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_corrupted_infernalAI(creature);
    }
};

class npc_grethok_the_controller : public CreatureScript
{
public:
    npc_grethok_the_controller() : CreatureScript("npc_grethok_the_controller") {}

    struct npc_grethok_the_controllerAI : public QuantumBasicAI
    {
        npc_grethok_the_controllerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 SlowTimer;
		uint32 ArcaneMissilesTimer;
		uint32 GreaterPolymorphTimer;

        void Reset()
		{
			SlowTimer = 2*IN_MILLISECONDS;
			ArcaneMissilesTimer = 4*IN_MILLISECONDS;
			GreaterPolymorphTimer = 6*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* /*killer*/)
		{
			DoSendQuantumText(SAY_GRETHOK_DEATH, me);
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
					SlowTimer = urand(4*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else SlowTimer -= diff;
			
			if (ArcaneMissilesTimer <= diff)
			{
				DoCastVictim(SPELL_ARCANE_MISSILES);
				ArcaneMissilesTimer = urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else ArcaneMissilesTimer -= diff;

			if (GreaterPolymorphTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_GREATER_POLYMORPH);
					GreaterPolymorphTimer = urand(12*IN_MILLISECONDS, 14*IN_MILLISECONDS);
				}
			}
			else GreaterPolymorphTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_grethok_the_controllerAI(creature);
    }
};

class npc_death_talon_hatcher : public CreatureScript
{
public:
    npc_death_talon_hatcher() : CreatureScript("npc_death_talon_hatcher") {}

    struct npc_death_talon_hatcherAI : public QuantumBasicAI
    {
        npc_death_talon_hatcherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 GrowingFlamesTimer;
		uint32 FlamestrikeTimer;

        void Reset()
		{
			GrowingFlamesTimer = 0.5*IN_MILLISECONDS;
			FlamestrikeTimer = 3*IN_MILLISECONDS;

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

			if (GrowingFlamesTimer <= diff)
			{
				DoCastAOE(SPELL_GROWING_FLAMES);
				GrowingFlamesTimer = 0.5*IN_MILLISECONDS;
			}
			else GrowingFlamesTimer -= diff;

			if (FlamestrikeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FLAMESTRIKE);
					FlamestrikeTimer = 4*IN_MILLISECONDS;
				}
			}
			else FlamestrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_death_talon_hatcherAI(creature);
    }
};

class npc_death_talon_flamescale : public CreatureScript
{
public:
    npc_death_talon_flamescale() : CreatureScript("npc_death_talon_flamescale") {}

    struct npc_death_talon_flamescaleAI : public QuantumBasicAI
    {
        npc_death_talon_flamescaleAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 FlameShockTimer;

        void Reset()
		{
			FlameShockTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_BERSERKER_CHARGE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FlameShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FLAME_SHOCK);
					FlameShockTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else FlameShockTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_death_talon_flamescaleAI(creature);
    }
};

class npc_death_talon_dragonspawn : public CreatureScript
{
public:
    npc_death_talon_dragonspawn() : CreatureScript("npc_death_talon_dragonspawn") {}

    struct npc_death_talon_dragonspawnAI : public QuantumBasicAI
    {
        npc_death_talon_dragonspawnAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 CleaveTimer;
		uint32 StrikeTimer;
		uint32 DragonbaneTimer;

        void Reset()
		{
			CleaveTimer = 2*IN_MILLISECONDS;
			StrikeTimer = 3*IN_MILLISECONDS;
			DragonbaneTimer = 4*IN_MILLISECONDS;

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

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_STRIKE);
				StrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else StrikeTimer -= diff;
			
			if (DragonbaneTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				DragonbaneTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else DragonbaneTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_death_talon_dragonspawnAI(creature);
    }
};

class npc_death_talon_seether : public CreatureScript
{
public:
    npc_death_talon_seether() : CreatureScript("npc_death_talon_seether") {}

    struct npc_death_talon_seetherAI : public QuantumBasicAI
    {
        npc_death_talon_seetherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 FlameBuffetTimer;
		uint32 EnrageTimer;

        void Reset()
		{
			FlameBuffetTimer = 0.5*IN_MILLISECONDS;
			EnrageTimer = 6*IN_MILLISECONDS;

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

			if (FlameBuffetTimer <= diff)
			{
				DoCastVictim(SPELL_FLAME_BUFFET);
				FlameBuffetTimer = 2*IN_MILLISECONDS;
			}
			else FlameBuffetTimer -= diff;

			if (EnrageTimer <= diff)
			{
				DoCast(me, SPELL_ENRAGE);
				EnrageTimer = 7*IN_MILLISECONDS;
			}
			else EnrageTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_death_talon_seetherAI(creature);
    }
};

class npc_death_talon_captain : public CreatureScript
{
public:
    npc_death_talon_captain() : CreatureScript("npc_death_talon_captain") {}

    struct npc_death_talon_captainAI : public QuantumBasicAI
    {
        npc_death_talon_captainAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 CleaveTimer;
		uint32 MarkOfDetonationTimer;
		uint32 IntimidatingOrdersTimer;

        void Reset()
		{
			CleaveTimer = 2*IN_MILLISECONDS;
			MarkOfDetonationTimer = 4*IN_MILLISECONDS;
			IntimidatingOrdersTimer = 6*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_AURA_OF_FLAMES);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_DT_CLEAVE);
				CleaveTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (MarkOfDetonationTimer <= diff)
			{
				DoCastAOE(SPELL_MARK_OF_DETONATION);
				MarkOfDetonationTimer = urand(6*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else MarkOfDetonationTimer -= diff;

			if (IntimidatingOrdersTimer <= diff)
			{
				DoCastAOE(SPELL_INTIMIDATING_ORDERS);
				IntimidatingOrdersTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else IntimidatingOrdersTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_death_talon_captainAI(creature);
    }
};

class npc_death_talon_wyrmkin : public CreatureScript
{
public:
    npc_death_talon_wyrmkin() : CreatureScript("npc_death_talon_wyrmkin") {}

    struct npc_death_talon_wyrmkinAI : public QuantumBasicAI
    {
        npc_death_talon_wyrmkinAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 FireballVolleyTimer;
		uint32 BlastWaveTimer;

        void Reset()
		{
			FireballVolleyTimer = 0.5*IN_MILLISECONDS;
			BlastWaveTimer = 3*IN_MILLISECONDS;

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

			if (FireballVolleyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FIREBALL_VOLLEY);
					FireballVolleyTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
				}
			}
			else FireballVolleyTimer -= diff;

			if (BlastWaveTimer <= diff)
			{
				DoCastAOE(SPELL_BLAST_WAVE);
				BlastWaveTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else BlastWaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_death_talon_wyrmkinAI(creature);
    }
};

class npc_death_talon_overseer : public CreatureScript
{
public:
    npc_death_talon_overseer() : CreatureScript("npc_death_talon_overseer") {}

    struct npc_death_talon_overseerAI : public QuantumBasicAI
    {
        npc_death_talon_overseerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 FireBlastTimer;
		uint32 CleaveTimer;

        void Reset()
		{
			FireBlastTimer = 0.5*IN_MILLISECONDS;
			CleaveTimer = 3*IN_MILLISECONDS;

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

			if (FireBlastTimer <= diff)
			{
				DoCastVictim(SPELL_FLAME_BLAST);
				FireBlastTimer = 2*IN_MILLISECONDS;
			}
			else FireBlastTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = 4*IN_MILLISECONDS;
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_death_talon_overseerAI(creature);
    }
};

class npc_death_talon_wyrmguard : public CreatureScript
{
public:
    npc_death_talon_wyrmguard() : CreatureScript("npc_death_talon_wyrmguard") {}

    struct npc_death_talon_wyrmguardAI : public QuantumBasicAI
    {
        npc_death_talon_wyrmguardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 OverCastTimer;
		uint32 WarStompTimer;

        void Reset()
		{
			OverCastTimer = 2*IN_MILLISECONDS;
			WarStompTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_DOUBLE_ATTACK);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (OverCastTimer <= diff)
			{
				DoCastAOE(SPELL_BROOD_POWER_BLUE);
				DoCastAOE(SPELL_BROOD_POWER_BRONZE);
				DoCastAOE(SPELL_BROOD_POWER_GREEN);
				DoCastAOE(SPELL_BROOD_POWER_RED);
				DoCastAOE(SPELL_BROOD_POWER_BLACK);
				OverCastTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else OverCastTimer -= diff;

			if (WarStompTimer <= diff)
			{
				DoCastAOE(SPELL_WAR_STOMP);
				WarStompTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else WarStompTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_death_talon_wyrmguardAI(creature);
    }
};

class npc_green_drakonid : public CreatureScript
{
public:
    npc_green_drakonid() : CreatureScript("npc_green_drakonid") {}

    struct npc_green_drakonidAI : public QuantumBasicAI
    {
        npc_green_drakonidAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 BroodPowerGreenTimer;

        void Reset()
		{
			BroodPowerGreenTimer = 1500;

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

			if (BroodPowerGreenTimer <= diff)
			{
				DoCastAOE(SPELL_BROOD_POWER_GREEN);
				BroodPowerGreenTimer = 3*IN_MILLISECONDS;
			}
			else BroodPowerGreenTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_green_drakonidAI(creature);
    }
};

class npc_red_drakonid : public CreatureScript
{
public:
    npc_red_drakonid() : CreatureScript("npc_red_drakonid") {}

    struct npc_red_drakonidAI : public QuantumBasicAI
    {
        npc_red_drakonidAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 BroodPowerRedTimer;

        void Reset()
		{
			BroodPowerRedTimer = 1500;

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

			if (BroodPowerRedTimer <= diff)
			{
				DoCastAOE(SPELL_BROOD_POWER_RED);
				BroodPowerRedTimer = 3*IN_MILLISECONDS;
			}
			else BroodPowerRedTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_red_drakonidAI(creature);
    }
};

class npc_blue_drakonid : public CreatureScript
{
public:
    npc_blue_drakonid() : CreatureScript("npc_blue_drakonid") {}

    struct npc_blue_drakonidAI : public QuantumBasicAI
    {
        npc_blue_drakonidAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 BroodPowerBlueTimer;

        void Reset()
		{
			BroodPowerBlueTimer = 1500;

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

			if (BroodPowerBlueTimer <= diff)
			{
				DoCastAOE(SPELL_BROOD_POWER_BLUE);
				BroodPowerBlueTimer = 3*IN_MILLISECONDS;
			}
			else BroodPowerBlueTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_blue_drakonidAI(creature);
    }
};

class npc_bronze_drakonid : public CreatureScript
{
public:
    npc_bronze_drakonid() : CreatureScript("npc_bronze_drakonid") {}

    struct npc_bronze_drakonidAI : public QuantumBasicAI
    {
        npc_bronze_drakonidAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 BroodPowerBronzeTimer;

        void Reset()
		{
			BroodPowerBronzeTimer = 1500;

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

			if (BroodPowerBronzeTimer <= diff)
			{
				DoCastAOE(SPELL_BROOD_POWER_BRONZE);
				BroodPowerBronzeTimer = 3*IN_MILLISECONDS;
			}
			else BroodPowerBronzeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bronze_drakonidAI(creature);
    }
};

class npc_black_drakonid : public CreatureScript
{
public:
    npc_black_drakonid() : CreatureScript("npc_black_drakonid") {}

    struct npc_black_drakonidAI : public QuantumBasicAI
    {
        npc_black_drakonidAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 BroodPowerBlackTimer;

        void Reset()
		{
			BroodPowerBlackTimer = 1500;

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

			if (BroodPowerBlackTimer <= diff)
			{
				DoCastAOE(SPELL_BROOD_POWER_BLACK);
				BroodPowerBlackTimer = 3*IN_MILLISECONDS;
			}
			else BroodPowerBlackTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_black_drakonidAI(creature);
    }
};

class npc_chromatic_drakonid : public CreatureScript
{
public:
    npc_chromatic_drakonid() : CreatureScript("npc_chromatic_drakonid") {}

    struct npc_chromatic_drakonidAI : public QuantumBasicAI
    {
        npc_chromatic_drakonidAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 BroodPowerBlackTimer;

        void Reset()
		{
			BroodPowerBlackTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_AURA_OF_FLAMES);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BroodPowerBlackTimer <= diff)
			{
				DoCastAOE(SPELL_BROOD_POWER_BLACK);
				BroodPowerBlackTimer = 3*IN_MILLISECONDS;
			}
			else BroodPowerBlackTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_chromatic_drakonidAI(creature);
    }
};

void AddSC_blackwing_lair_trash()
{
	new npc_blackwing_technician();
	new npc_blackwing_guardsman();
	new npc_blackwing_legionnaire();
	new npc_blackwing_taskmaster();
	new npc_blackwing_mage();
	new npc_blackwing_spellbinder();
	new npc_blackwing_warlock();
	new npc_demon_portal();
	new npc_enraged_felguard();
	new npc_corrupted_infernal();
	new npc_grethok_the_controller();
	new npc_death_talon_hatcher();
	new npc_death_talon_flamescale();
	new npc_death_talon_dragonspawn();
	new npc_death_talon_seether();
	new npc_death_talon_captain();
	new npc_death_talon_wyrmkin();
	new npc_death_talon_overseer();
	new npc_death_talon_wyrmguard();
	new npc_green_drakonid();
	new npc_red_drakonid();
	new npc_blue_drakonid();
	new npc_bronze_drakonid();
	new npc_black_drakonid();
	new npc_chromatic_drakonid();
}