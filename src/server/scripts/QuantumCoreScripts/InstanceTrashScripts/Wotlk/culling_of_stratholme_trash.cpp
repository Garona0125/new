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
#include "../../../scripts/Kalimdor/CavernsOfTime/CullingOfStratholme/culling_of_stratholme.h"

enum CullingOfStratholmeSpells
{
	SPELL_ENRAGE              = 8599,
	SPELL_CLEAVE              = 15284,
	SPELL_UNSTOPPABLE_ENRAGE  = 52461,
	SPELL_CRIPPLE_5N          = 52498,
	SPELL_CRIPPLE_5H          = 20812,
	SPELL_DRAIN_MANA_5N       = 58772,
	SPELL_DRAIN_MANA_5H       = 58770,
	SPELL_SHADOW_BOLT_5N      = 15537,
	SPELL_SHADOW_BOLT_5H      = 61558,
	SPELL_SUMMON_SKELETONS    = 52611,
	SPELL_DEVOUR_FLESH_5N     = 52352,
	SPELL_DEVOUR_FLESH_5H     = 58758,
	SPELL_STEAL_FLESH         = 52709,
	SPELL_HEMOTOXIN_5N        = 52522,
	SPELL_HEMOTOXIN_5H        = 58782,
	SPELL_WEB_EXPLOSION       = 52491,
	SPELL_CRYPT_SCARABS       = 52496,
	SPELL_WRETCHING_BILE_5N   = 52527,
	SPELL_WRETCHING_BILE_5H   = 58810,
	SPELL_BG_DISEASE_CLOUD_5N = 28156,
	SPELL_BG_DISEASE_CLOUD_5H = 31607,
	SPELL_PC_DISEASE_CLOUD_5N = 52525,
	SPELL_PC_DISEASE_CLOUD_5H = 58808,
	SPELL_VOID_STRIKE_5N      = 60590,
	SPELL_VOID_STRIKE_5H      = 52633,
	SPELL_VOID_SHIELD_5N      = 52634,
	SPELL_VOID_SHIELD_5H      = 58813,
	SPELL_TIME_SHIFT_5N       = 52636,
	SPELL_TIME_SHIFT_5H       = 61084,
	SPELL_ACCELERATE_5N       = 52635,
	SPELL_ACCELERATE_5H       = 58820,
	SPELL_ARCANE_WAVE_5N      = 52660,
	SPELL_ARCANE_WAVE_5H      = 58817,
	SPELL_TEMPORAL_VORTEX_5N  = 52657,
	SPELL_TEMPORAL_VORTEX_5H  = 58816,
	SPELL_SHADOW_SHOCK_5N     = 17439,
	SPELL_SHADOW_SHOCK_5H     = 17234,
	SPELL_CURSE_OF_AGONY_5N   = 14875,
	SPELL_CURSE_OF_AGONY_5H   = 46190,
	SPELL_CORRUPTION_5N       = 39621,
	SPELL_CORRUPTION_5H       = 58811,
};

class npc_dark_necromancer : public CreatureScript
{
public:
    npc_dark_necromancer() : CreatureScript("npc_dark_necromancer") { }

    struct npc_dark_necromancerAI : public QuantumBasicAI
    {
        npc_dark_necromancerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShadowBoltTimer;
		uint32 CrippleTimer;
		uint32 DrainManaTimer;

        void Reset()
        {
            ShadowBoltTimer = 0.5*IN_MILLISECONDS;
            CrippleTimer = 2*IN_MILLISECONDS;
			DrainManaTimer = 4*IN_MILLISECONDS;

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

            if (CrippleTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_CRIPPLE_5N, SPELL_CRIPPLE_5H));
					CrippleTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
            }
			else CrippleTimer -= diff;

			if (DrainManaTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, DUNGEON_MODE(SPELL_DRAIN_MANA_5N, SPELL_DRAIN_MANA_5H));
						DrainManaTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
					}
				}
			}
			else DrainManaTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_necromancerAI(creature);
    }
};

class npc_master_necromancer : public CreatureScript
{
public:
    npc_master_necromancer() : CreatureScript("npc_master_necromancer") { }

    struct npc_master_necromancerAI : public QuantumBasicAI
    {
        npc_master_necromancerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShadowBoltTimer;
		uint32 SummonSkeletonsTimer;

        void Reset()
        {
            ShadowBoltTimer = 0.5*IN_MILLISECONDS;
            SummonSkeletonsTimer = 2*IN_MILLISECONDS;

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

            if (SummonSkeletonsTimer <= diff)
            {
				DoCast(me, SPELL_SUMMON_SKELETONS);
				SummonSkeletonsTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
            }
			else SummonSkeletonsTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_master_necromancerAI(creature);
    }
};

class npc_devouring_ghoul : public CreatureScript
{
public:
    npc_devouring_ghoul() : CreatureScript("npc_devouring_ghoul") {}

    struct npc_devouring_ghoulAI : public QuantumBasicAI
    {
        npc_devouring_ghoulAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 DevourFleshTimer;

        void Reset()
        {
			DevourFleshTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
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

			if (DevourFleshTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_DEVOUR_FLESH_5N, SPELL_DEVOUR_FLESH_5H));
				DevourFleshTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else DevourFleshTimer -= diff;

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
        return new npc_devouring_ghoulAI(creature);
    }
};

class npc_ghoul_minion_cos : public CreatureScript
{
public:
    npc_ghoul_minion_cos() : CreatureScript("npc_ghoul_minion_cos") {}

    struct npc_ghoul_minion_cosAI : public QuantumBasicAI
    {
        npc_ghoul_minion_cosAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 StealFleshTimer;

        void Reset()
        {
			StealFleshTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
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

			if (StealFleshTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_STEAL_FLESH);
					StealFleshTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else StealFleshTimer -= diff;

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
        return new npc_ghoul_minion_cosAI(creature);
    }
};

class npc_tomb_stalker : public CreatureScript
{
public:
    npc_tomb_stalker() : CreatureScript("npc_tomb_stalker") { }

    struct npc_tomb_stalkerAI : public QuantumBasicAI
    {
        npc_tomb_stalkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 HemotoxinTimer;

        void Reset()
        {
            HemotoxinTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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

			if (HemotoxinTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_HEMOTOXIN_5N, SPELL_HEMOTOXIN_5H));
					HemotoxinTimer = 4*IN_MILLISECONDS;
				}
			}
			else HemotoxinTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tomb_stalkerAI(creature);
    }
};

class npc_crypt_fiend : public CreatureScript
{
public:
    npc_crypt_fiend() : CreatureScript("npc_crypt_fiend") { }

    struct npc_crypt_fiendAI : public QuantumBasicAI
    {
       npc_crypt_fiendAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 WebExplosionTimer;
        uint32 CryptScarabsTimer;

        void Reset()
        {
            WebExplosionTimer = 1*IN_MILLISECONDS;
            CryptScarabsTimer = 2*IN_MILLISECONDS;

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

			if (WebExplosionTimer <= diff)
            {
				DoCastAOE(SPELL_WEB_EXPLOSION);
				WebExplosionTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else WebExplosionTimer -= diff;

            if (CryptScarabsTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CRYPT_SCARABS);
					CryptScarabsTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
            }
			else CryptScarabsTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crypt_fiendAI(creature);
    }
};

class npc_enraging_ghoul : public CreatureScript
{
public:
    npc_enraging_ghoul() : CreatureScript("npc_enraging_ghoul") { }

    struct npc_enraging_ghoulAI : public QuantumBasicAI
    {
        npc_enraging_ghoulAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 UnstoppableEnrageTimer;

        void Reset()
        {
            UnstoppableEnrageTimer = 0.5*IN_MILLISECONDS;

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

			if (UnstoppableEnrageTimer <= diff)
            {
				DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				DoCast(me, SPELL_UNSTOPPABLE_ENRAGE);
				UnstoppableEnrageTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else UnstoppableEnrageTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_enraging_ghoulAI(creature);
    }
};

class npc_bile_golem : public CreatureScript
{
public:
    npc_bile_golem() : CreatureScript("npc_bile_golem") { }

    struct npc_bile_golemAI : public QuantumBasicAI
    {
        npc_bile_golemAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 WretchingBileTimer;

        void Reset()
        {
            WretchingBileTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, DUNGEON_MODE(SPELL_BG_DISEASE_CLOUD_5N, SPELL_BG_DISEASE_CLOUD_5H), true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WretchingBileTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_WRETCHING_BILE_5N, SPELL_WRETCHING_BILE_5H));
					WretchingBileTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else WretchingBileTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bile_golemAI(creature);
    }
};

class npc_patchwork_construct : public CreatureScript
{
public:
    npc_patchwork_construct() : CreatureScript("npc_patchwork_construct") { }

    struct npc_patchwork_constructAI : public QuantumBasicAI
    {
        npc_patchwork_constructAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 CleaveTimer;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, DUNGEON_MODE(SPELL_PC_DISEASE_CLOUD_5N, SPELL_PC_DISEASE_CLOUD_5H), true);
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
				CleaveTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_patchwork_constructAI(creature);
    }
};

class npc_infinite_adversary : public CreatureScript
{
public:
    npc_infinite_adversary() : CreatureScript("npc_infinite_adversary") { }

    struct npc_infinite_adversaryAI : public QuantumBasicAI
    {
        npc_infinite_adversaryAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 VoidShieldTimer;
		uint32 VoidStrikeTimer;

        void Reset()
        {
			VoidShieldTimer = 0.5*IN_MILLISECONDS;
			VoidStrikeTimer = 2*IN_MILLISECONDS;

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

			if (VoidShieldTimer <= diff)
            {
				DoCast(me, DUNGEON_MODE(SPELL_VOID_SHIELD_5N, SPELL_VOID_SHIELD_5H));
				VoidShieldTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else VoidShieldTimer -= diff;

			if (VoidStrikeTimer <= diff)
            {
				DoCastVictim(DUNGEON_MODE(SPELL_VOID_STRIKE_5N, SPELL_VOID_STRIKE_5H));
				VoidStrikeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else VoidStrikeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_infinite_adversaryAI(creature);
    }
};

class npc_infinite_hunter : public CreatureScript
{
public:
    npc_infinite_hunter() : CreatureScript("npc_infinite_hunter") { }

    struct npc_infinite_hunterAI : public QuantumBasicAI
    {
        npc_infinite_hunterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 AccelerateTimer;
		uint32 TimeShiftTimer;

        void Reset()
        {
			AccelerateTimer = 0.5*IN_MILLISECONDS;
			TimeShiftTimer = 2*IN_MILLISECONDS;

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

			if (AccelerateTimer <= diff)
            {
				DoCast(me, DUNGEON_MODE(SPELL_ACCELERATE_5N, SPELL_ACCELERATE_5H));
				AccelerateTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else AccelerateTimer -= diff;

			if (TimeShiftTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_TIME_SHIFT_5N, SPELL_TIME_SHIFT_5H));
					TimeShiftTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else TimeShiftTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_infinite_hunterAI(creature);
    }
};

class npc_infinite_agent : public CreatureScript
{
public:
    npc_infinite_agent() : CreatureScript("npc_infinite_agent") { }

    struct npc_infinite_agentAI : public QuantumBasicAI
    {
        npc_infinite_agentAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ArcaneWaveTimer;
		uint32 TemporalVortexTimer;

        void Reset()
        {
            ArcaneWaveTimer = 0.5*IN_MILLISECONDS;
            TemporalVortexTimer = 2*IN_MILLISECONDS;

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

			if (ArcaneWaveTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_ARCANE_WAVE_5N, SPELL_ARCANE_WAVE_5H));
					ArcaneWaveTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ArcaneWaveTimer -= diff;

            if (TemporalVortexTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_TEMPORAL_VORTEX_5N, SPELL_TEMPORAL_VORTEX_5H));
					TemporalVortexTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
            }
			else TemporalVortexTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_infinite_agentAI(creature);
    }
};

class npc_acolyte : public CreatureScript
{
public:
    npc_acolyte() : CreatureScript("npc_acolyte") { }

    struct npc_acolyteAI : public QuantumBasicAI
    {
        npc_acolyteAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_BANISH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CurseOfAgonyTimer;
		uint32 CorruptionTimer;
		uint32 ShadowShockTimer;

        void Reset()
        {
            CurseOfAgonyTimer = 0.5*IN_MILLISECONDS;
			CorruptionTimer = 1*IN_MILLISECONDS;
            ShadowShockTimer = 2*IN_MILLISECONDS;

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
					CurseOfAgonyTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CurseOfAgonyTimer -= diff;

			if (CorruptionTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_CORRUPTION_5N, SPELL_CORRUPTION_5H));
					CorruptionTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
            }
			else CorruptionTimer -= diff;

            if (ShadowShockTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_SHOCK_5N, SPELL_SHADOW_SHOCK_5H));
					ShadowShockTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
            }
			else ShadowShockTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_acolyteAI(creature);
    }
};

void AddSC_culling_of_stratholme_trash()
{
	new npc_dark_necromancer();
	new npc_master_necromancer();
	new npc_devouring_ghoul();
	new npc_ghoul_minion_cos();
	new npc_tomb_stalker();
	new npc_crypt_fiend();
	new npc_enraging_ghoul();
	new npc_bile_golem();
	new npc_patchwork_construct();
	new npc_infinite_adversary();
	new npc_infinite_hunter();
	new npc_infinite_agent();
	new npc_acolyte();
}