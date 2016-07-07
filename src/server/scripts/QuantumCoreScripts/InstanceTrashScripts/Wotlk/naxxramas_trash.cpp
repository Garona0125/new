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
#include "../../../scripts/Northrend/Naxxramas/naxxramas.h"

enum NaxxramasTrashSpells
{
	SPELL_ENRAGE                   = 8599,
    SPELL_VEIL_OF_SHADOW_10        = 28440,
    SPELL_VEIL_OF_SHADOW_25        = 53803,
	SPELL_POISON_SPRAY_10          = 30043,
    SPELL_POISON_SPRAY_25          = 56032,
    SPELL_SPIDER_WEB               = 28434,
	SPELL_POISON_CHARGE_10         = 28431,
    SPELL_POISON_CHARGE_25         = 53809,
	SPELL_CR_CLEAVE                = 40504,
    SPELL_CR_FRENZY                = 56625,
	SPELL_CRYPT_SCARAB_SWARM_10    = 54313,
    SPELL_CRYPT_SCARAB_SWARM_25    = 54317,
    SPELL_CRYPT_SCARABS_10         = 54311,
    SPELL_CRYPT_SCARABS_25         = 54316,
    SPELL_POINTY_SPIKE             = 28614,
    SPELL_SPIKE_VOLLEY_10          = 28615,
	SPELL_SHADOW_BURST_10          = 53850,
    SPELL_SHADOW_BURST_25          = 53851,
	SPELL_NA_ARCANE_EXPLOSION_10   = 56063,
    SPELL_NA_ARCANE_EXPLOSION_25   = 56067,
    SPELL_NA_SHADOW_BOLT_VOLLEY_10 = 56064,
    SPELL_NA_SHADOW_BOLT_VOLLEY_25 = 56065,
	SPELL_VS_SHADOW_BOLT_VOLLEY_10 = 55850,
    SPELL_VS_SHADOW_BOLT_VOLLEY_25 = 55851,
	SPELL_INVISIBLE                = 30831,
	SPELL_PLAGUE_SLIME_BLACK       = 28987,
    SPELL_PLAGUE_SLIME_BLUE        = 28988,
    SPELL_PLAGUE_SLIME_GREEN       = 28989,
    SPELL_PLAGUE_SLIME_RED         = 28990,
	SPELL_FLESH_ROT_10             = 29915,
    SPELL_FLESH_ROT_25             = 54709,
    SPELL_REND_10                  = 54703,
    SPELL_REND_25                  = 54708,
	SPELL_INFG_FRENZY              = 54701,
	SPELL_ACID_VOLLEY_10           = 29325,
    SPELL_ACID_VOLLEY_25           = 54714,
    SPELL_STONESKIN_10             = 28995,
    SPELL_STONESKIN_25             = 54722,
	SPELL_SLIME_BURST_10           = 30109,
    SPELL_SLIME_BURST_25           = 54769,
	SPELL_PLAGUE_SPLASH_10         = 54780,
    SPELL_PLAGUE_SPLASH_25         = 56538,
	SPELL_FRENZIED_DIVE            = 54781,
    SPELL_ES_MIND_FLAY_10          = 29407,
    SPELL_ES_MIND_FLAY_25          = 54805,
    SPELL_PWG_CLEAVE               = 27794,
    SPELL_DISEASE_CLOUD_10         = 28156,
	SPELL_DISEASE_CLOUD_25         = 31607,
    SPELL_EXECUTE_10               = 7160,
    SPELL_EXECUTE_25               = 56426,
    SPELL_WAR_STOMP_10             = 27758,
    SPELL_WAR_STOMP_25             = 56427,
    SPELL_BILE_VOMIT_10            = 27807,
    SPELL_BILE_VOMIT_25            = 54326,
	SPELL_ACIDIC_SLUDGE_10         = 27891,
    SPELL_ACIDIC_SLUDGE_25         = 54331,
	SPELL_SL_DISEASE_CLOUD         = 27793,
    SPELL_GREAT_HEAL_10            = 28306,
    SPELL_GREAT_HEAL_25            = 54337,
    SPELL_MANA_BURN_10             = 28301,
    SPELL_MANA_BURN_25             = 54338,
    SPELL_CHAIN_LIGHTNING_10       = 28293,
    SPELL_CHAIN_LIGHTNING_25       = 54334,
    SPELL_LIGHTNING_TOTEM          = 28294,
	SPELL_SA_MIND_FLAY_10          = 28310,
    SPELL_SA_MIND_FLAY_25          = 54339,
    SPELL_EMBALMING_CLOUD          = 28322,
	SPELL_UNSTOPPABLE_ENRAGE       = 54356,
    SPELL_KNOCKBACK                = 28405,
    SPELL_MASSIVE_STOMP_10         = 55821,
    SPELL_MASSIVE_STOMP_25         = 55826,
    SPELL_CHARGE                   = 55317,
	SPELL_PLAGUE_STRIKE_10         = 55255,
    SPELL_PLAGUE_STRIKE_25         = 55321,
    SPELL_UNHOLY_PRESENCE          = 55222,
    SPELL_RAISE_DEATH              = 28353,
    SPELL_BLOOD_PRESENCE           = 55212,
    SPELL_DEATH_COIL_HEAL          = 55210,
    SPELL_DEATH_COIL_DAMAGE_10     = 55209,
    SPELL_DEATH_COIL_DAMAGE_25     = 55320,
    SPELL_HYSTERIA                 = 55213,
    SPELL_PIERCE_ARMOR             = 55318,
    SPELL_DTW_WHIRLWIND            = 55266,
    SPELL_DISMOUNT_DEATHCHARGER    = 55294,
    SPELL_BONE_ARMOR_10            = 55315,
    SPELL_BONE_ARMOR_25            = 55336,
    SPELL_ICY_TOUCH_10             = 55313,
    SPELL_ICY_TOUCH_25             = 55331,
    SPELL_STRANGULATE_10           = 55314,
    SPELL_STRANGULATE_25           = 55334,
	SPELL_SUMMON_PORTAL_OF_SHADOWS = 28383,
    SPELL_SHADOW_BOLT_VOLLEY_10    = 28407,
    SPELL_SHADOW_BOLT_VOLLEY_25    = 55323,
	SPELL_ARCANE_EXPLOSION         = 15453,
	SPELL_BLAST_WAVE               = 30092,
	SPELL_BLINK                    = 28391,
	SPELL_FLAMESTRIKE              = 30091,
	SPELL_NC_FROST_NOVA            = 30094,
	SPELL_CONE_OF_COLD             = 30095,
    SPELL_BRUTAL_SWIPE_10          = 55319,
    SPELL_BRUTAL_SWIPE_25          = 55324,
	SPELL_DISARM                   = 6713,
	SPELL_CRUSH_ARMOR              = 33661,
	SPELL_MORTAL_STRIKE            = 16856,
    SPELL_UA_WHIRLWIND             = 55463,
    SPELL_US_ARCANE_EXPLOSION_10   = 28450,
    SPELL_US_ARCANE_EXPLOSION_25   = 55467,
    SPELL_FROST_NOVA               = 29849,
    SPELL_POLYMORPH                = 29848,
    SPELL_US_CLEAVE                = 15284,
    SPELL_THRASH                   = 3417,
	SPELL_PW_CLEAVE                = 15496,
	SPELL_PC_MORTAL_STRIKE         = 32736,
    SPELL_SHADOW_SHOCK_10          = 30138,
    SPELL_SHADOW_SHOCK_25          = 54889,
	SPELL_ARCANE_EXPLOSION_10      = 54890,
    SPELL_ARCANE_EXPLOSION_25      = 54891,
    SPELL_BLINK_1                  = 29208,
    SPELL_BLINK_2                  = 29209,
    SPELL_BLINK_3                  = 29210,
    SPELL_BLINK_4                  = 29211,
	SPELL_DARK_BLAST_10            = 28457,
	SPELL_DARK_BLAST_25            = 55713,
	SPELL_WAILS_OF_SOULS_10        = 28460,
	SPELL_WAILS_OF_SOULS_25        = 55717,
	SPELL_FS_DISEASE_CLOUD         = 28156,
	SPELL_FRENZIED_LEAP            = 56729,
};

enum Mounts
{
	MOUNT_DEATHCHARGER_ID = 26541,
};

class npc_dread_creeper : public CreatureScript
{
public:
    npc_dread_creeper() : CreatureScript("npc_dread_creeper") { }

    struct npc_dread_creeperAI : QuantumBasicAI
    {
        npc_dread_creeperAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 VeilOfShadowTimer;

        void Reset()
        {
            VeilOfShadowTimer = 1000;

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

            if (VeilOfShadowTimer <= diff)
            {
				DoCastAOE(RAID_MODE(SPELL_VEIL_OF_SHADOW_10, SPELL_VEIL_OF_SHADOW_25));
				VeilOfShadowTimer = 6000;
            }
			else VeilOfShadowTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dread_creeperAI(creature);
    }
};

class npc_carrion_spinner : public CreatureScript
{
public:
    npc_carrion_spinner() : CreatureScript("npc_carrion_spinner") { }

	struct npc_carrion_spinnerAI : QuantumBasicAI
    {
        npc_carrion_spinnerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 SpiderWebTimer;
        uint32 PoisonSprayTimer;

        void Reset()
        {
            SpiderWebTimer = 500;
            PoisonSprayTimer = 3000;

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

            if (SpiderWebTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SPIDER_WEB);
					SpiderWebTimer = 6000;
				}
            }
			else SpiderWebTimer -= diff;

            if (PoisonSprayTimer <= diff)
            {
				DoCast(RAID_MODE(SPELL_POISON_SPRAY_10, SPELL_POISON_SPRAY_25));
				PoisonSprayTimer = 8000;
            }
			else PoisonSprayTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_carrion_spinnerAI(creature);
    }
};

class npc_venom_stalker : public CreatureScript
{
public:
    npc_venom_stalker() : CreatureScript("npc_venom_stalker") { }

	struct npc_venom_stalkerAI : QuantumBasicAI
    {
        npc_venom_stalkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 PoisonChargeTimer;

        void Reset()
        {
            PoisonChargeTimer = 200;

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

            if (PoisonChargeTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_POISON_CHARGE_10, SPELL_POISON_CHARGE_25));
					PoisonChargeTimer = 5000;
				}
            }
			else PoisonChargeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_venom_stalkerAI(creature);
    }
};

class npc_crypt_reaver : public CreatureScript
{
public:
    npc_crypt_reaver() : CreatureScript("npc_crypt_reaver") { }

    struct npc_crypt_reaverAI : QuantumBasicAI
    {
		npc_crypt_reaverAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 CleaveTimer;

        void Reset()
        {
            CleaveTimer = 1000;

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

            if (CleaveTimer <= diff)
            {
				DoCastVictim(SPELL_CR_CLEAVE);
				CleaveTimer = 4*IN_MILLISECONDS;
            }
			else CleaveTimer -= diff;

            if (HealthBelowPct(HEALTH_PERCENT_30))
            {
				if (!me->HasAuraEffect(SPELL_CR_FRENZY, 0))
				{
					DoSendQuantumText(SAY_GENERIC_EMOTE_FRENZY, me);
					DoCast(me, SPELL_CR_FRENZY);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crypt_reaverAI(creature);
    }
};

class npc_tomb_horror : public CreatureScript
{
public:
    npc_tomb_horror() : CreatureScript("npc_tomb_horror") { }

    struct npc_tomb_horrorAI : QuantumBasicAI
    {
        npc_tomb_horrorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 CryptScarabsTimer;
        uint32 CryptScarabSwarmTimer;

        void Reset()
        {
            CryptScarabsTimer = 500;
            CryptScarabSwarmTimer = 3000;

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

            if (CryptScarabsTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_CRYPT_SCARABS_10, SPELL_CRYPT_SCARABS_25));
					CryptScarabsTimer = 4*IN_MILLISECONDS;
				}
            }
			else CryptScarabsTimer -= diff;

            if (CryptScarabSwarmTimer <= diff)
            {
				DoCastAOE(RAID_MODE(SPELL_CRYPT_SCARAB_SWARM_10, SPELL_CRYPT_SCARAB_SWARM_25));
				CryptScarabSwarmTimer = 6000;
            }
			else CryptScarabSwarmTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tomb_horrorAI(creature);
    }
};

class npc_naxxramas_cultist : public CreatureScript
{
public:
    npc_naxxramas_cultist() : CreatureScript("npc_naxxramas_cultist") { }

    struct npc_naxxramas_cultistAI : QuantumBasicAI
    {
        npc_naxxramas_cultistAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ShadowBurstTimer;

        void Reset()
        {
            ShadowBurstTimer = 1000;

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

            if (ShadowBurstTimer <= diff)
            {
				DoCastAOE(RAID_MODE(SPELL_SHADOW_BURST_10, SPELL_SHADOW_BURST_25));
				ShadowBurstTimer = 5000;
            }
			else ShadowBurstTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_naxxramas_cultistAI(creature);
	}
};

class npc_naxxramas_acolyte : public CreatureScript
{
public:
    npc_naxxramas_acolyte() : CreatureScript("npc_naxxramas_acolyte") { }

    struct npc_naxxramas_acolyteAI : QuantumBasicAI
    {
        npc_naxxramas_acolyteAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ArcaneExplosionTimer;
        uint32 ShadowBoltVolleyTimer;

        void Reset()
        {
            ShadowBoltVolleyTimer = 500;
			ArcaneExplosionTimer = 2000;

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

			if (ShadowBoltVolleyTimer <= diff)
            {
				DoCastAOE(RAID_MODE(SPELL_NA_SHADOW_BOLT_VOLLEY_10, SPELL_NA_SHADOW_BOLT_VOLLEY_25));
				ShadowBoltVolleyTimer = 4*IN_MILLISECONDS;
            }
			else ShadowBoltVolleyTimer -= diff;

            if (ArcaneExplosionTimer <= diff)
            {
				DoCastAOE(RAID_MODE(SPELL_NA_ARCANE_EXPLOSION_10, SPELL_NA_ARCANE_EXPLOSION_25));
				ArcaneExplosionTimer = 6000;
            }
			else ArcaneExplosionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_naxxramas_acolyteAI(creature);
    }
};

class npc_vigilant_shade : public CreatureScript
{
public:
    npc_vigilant_shade() : CreatureScript("npc_vigilant_shade") { }

    struct npc_vigilant_shadeAI : QuantumBasicAI
    {
        npc_vigilant_shadeAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ShadowBoltVolleyTimer;

        void Reset()
        {
            ShadowBoltVolleyTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_INVISIBLE);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            me->RemoveAurasDueToSpell(SPELL_INVISIBLE);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ShadowBoltVolleyTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_VS_SHADOW_BOLT_VOLLEY_10, SPELL_VS_SHADOW_BOLT_VOLLEY_25));
					ShadowBoltVolleyTimer = 3000;
				}
            }
			else ShadowBoltVolleyTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vigilant_shadeAI(creature);
    }
};

class npc_plague_slime : public CreatureScript
{
public:
    npc_plague_slime() : CreatureScript("npc_plague_slime") { }

	struct npc_plague_slimeAI : QuantumBasicAI
    {
        npc_plague_slimeAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 TauntCheckTimer;
        uint32 CurrentSpell;
        uint64 TankGuid;

        void Reset()
        {
            TauntCheckTimer = 1000;
            CurrentSpell = 0;
            TankGuid = 0;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterToBattle(Unit* who)
        {
            TankGuid = who->GetGUID();
            CurrentSpell = RAND(SPELL_PLAGUE_SLIME_BLACK, SPELL_PLAGUE_SLIME_BLUE, SPELL_PLAGUE_SLIME_GREEN, SPELL_PLAGUE_SLIME_RED);
            DoCast(me, CurrentSpell, true);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (TauntCheckTimer <= diff)
            {
                if (me->GetVictim() && TankGuid != me->GetVictim()->GetGUID())
                {
                    TankGuid = me->GetVictim()->GetGUID();

                    uint32 NewSpell = 0;
                    do
                    {
                        NewSpell = RAND(SPELL_PLAGUE_SLIME_BLACK, SPELL_PLAGUE_SLIME_BLUE, SPELL_PLAGUE_SLIME_GREEN, SPELL_PLAGUE_SLIME_RED);
                    }
					while (NewSpell == CurrentSpell);
                    CurrentSpell = NewSpell;
                    DoCast(me, CurrentSpell, true);
                }
                TauntCheckTimer = 10000;
            }
			else TauntCheckTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

   	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_plague_slimeAI(creature);
    }
};

class npc_infectious_ghoul : public CreatureScript
{
public:
    npc_infectious_ghoul() : CreatureScript("npc_infectious_ghoul") { }

    struct npc_infectious_ghoulAI : QuantumBasicAI
    {
        npc_infectious_ghoulAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 FleshRotTimer;
        uint32 RendTimer;

        void Reset()
        {
            FleshRotTimer = 1000;
            RendTimer = 3000;

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

            if (FleshRotTimer <= diff)
            {
				DoCastVictim(RAID_MODE(SPELL_FLESH_ROT_10, SPELL_FLESH_ROT_25));
				FleshRotTimer = 4*IN_MILLISECONDS;
            }
			else FleshRotTimer -= diff;

            if (RendTimer <= diff)
            {
				DoCastVictim(RAID_MODE(SPELL_REND_10, SPELL_REND_25));
				RendTimer = 6000;
            }
			else RendTimer -= diff;

            if (HealthBelowPct(HEALTH_PERCENT_30))
            {
				if (!me->HasAuraEffect(SPELL_INFG_FRENZY, 0))
				{
					DoSendQuantumText(SAY_GENERIC_EMOTE_FRENZY, me);
					DoCast(me, SPELL_INFG_FRENZY);
				}
            }

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_infectious_ghoulAI(creature);
    }
};

class npc_stoneskin_gargoyle : public CreatureScript
{
public:
    npc_stoneskin_gargoyle() : CreatureScript("npc_stoneskin_gargoyle") { }

    struct npc_stoneskin_gargoyleAI : QuantumBasicAI
    {
        npc_stoneskin_gargoyleAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 AcidVolleyTimer;

        void Reset()
        {
            AcidVolleyTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void SpellHit(Unit* target, const SpellInfo* spell)
        {
            if (target == me && spell->Id == RAID_MODE(SPELL_STONESKIN_10, SPELL_STONESKIN_25))
                me->SetStandState(UNIT_STAND_STATE_SUBMERGED);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (me->HasAuraEffect(RAID_MODE(SPELL_STONESKIN_10, SPELL_STONESKIN_25), 0))
                return;

            else if (me->getStandState() == UNIT_STAND_STATE_SUBMERGED)
				me->SetStandState(UNIT_STAND_STATE_STAND);

            if (AcidVolleyTimer <= diff)
            {
				DoCastAOE(RAID_MODE(SPELL_ACID_VOLLEY_10, SPELL_ACID_VOLLEY_25));
				AcidVolleyTimer = 6000;
            }
			else AcidVolleyTimer -= diff;

            if (HealthBelowPct(HEALTH_PERCENT_30))
				DoCast(me, RAID_MODE(SPELL_STONESKIN_10, SPELL_STONESKIN_25));

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stoneskin_gargoyleAI(creature);
    }
};

class npc_mutated_grub : public CreatureScript
{
public:
    npc_mutated_grub() : CreatureScript("npc_mutated_grub") { }

    struct npc_mutated_grubAI : QuantumBasicAI
    {
        npc_mutated_grubAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 SlimeBurstTimer;

        void Reset()
        {
            SlimeBurstTimer = 1000;

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

            if (SlimeBurstTimer <= diff)
            {
				DoCastAOE(RAID_MODE(SPELL_SLIME_BURST_10, SPELL_SLIME_BURST_25));
				SlimeBurstTimer = 4*IN_MILLISECONDS;
            }
			else SlimeBurstTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mutated_grubAI(creature);
    }
};

class npc_plague_beast : public CreatureScript
{
public:
    npc_plague_beast() : CreatureScript("npc_plague_beast") { }

    struct npc_plague_beastAI : QuantumBasicAI
    {
        npc_plague_beastAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 PlagueSplashTimer;

        void Reset()
        {
            PlagueSplashTimer = 2000;

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

            if (PlagueSplashTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_PLAGUE_SPLASH_10, SPELL_PLAGUE_SPLASH_25));
					PlagueSplashTimer = 4*IN_MILLISECONDS;
				}
            }
			else PlagueSplashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_plague_beastAI(creature);
    }
};

class npc_frenzied_bat : public CreatureScript
{
public:
    npc_frenzied_bat() : CreatureScript("npc_frenzied_bat") { }

    struct npc_frenzied_batAI : QuantumBasicAI
    {
        npc_frenzied_batAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 FrenziedDiveTimer;

        void Reset()
        {
            FrenziedDiveTimer = 1000;

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

            if (FrenziedDiveTimer <= diff)
            {
				DoCastVictim(SPELL_FRENZIED_DIVE);
				FrenziedDiveTimer = 4*IN_MILLISECONDS;
            }
			else FrenziedDiveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frenzied_batAI(creature);
    }
};

class npc_eye_stalk : public CreatureScript
{
public:
    npc_eye_stalk() : CreatureScript("npc_eye_stalk") { }

    struct npc_eye_stalkAI : QuantumBasicAI
    {
        npc_eye_stalkAI(Creature* creature) : QuantumBasicAI(creature)
        {
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);

			SetCombatMovement(false);
        }

        uint32 MindFlayTimer;

        void Reset()
        {
            MindFlayTimer = 500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void AttackStart(Unit* who)
        {
            QuantumBasicAI::AttackStartNoMove(who);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (MindFlayTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_ES_MIND_FLAY_10, SPELL_ES_MIND_FLAY_25));
					MindFlayTimer = 4*IN_MILLISECONDS;
				}
            }
			else MindFlayTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_eye_stalkAI(creature);
    }
};

class npc_patchwork_golem : public CreatureScript
{
public:
    npc_patchwork_golem() : CreatureScript("npc_patchwork_golem") { }

    struct npc_patchwork_golemAI : QuantumBasicAI
    {
        npc_patchwork_golemAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 CleaveTimer;
        uint32 ExecuteTimer;
        uint32 WarStompTimer;

        void Reset()
        {
            CleaveTimer = 1000;
            WarStompTimer = 3000;
			ExecuteTimer = 5000;

			DoCast(me, DUNGEON_MODE(SPELL_DISEASE_CLOUD_10, SPELL_DISEASE_CLOUD_25), true);
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
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
				DoCastVictim(SPELL_PWG_CLEAVE);
				CleaveTimer = 4*IN_MILLISECONDS;
            }
			else CleaveTimer -= diff;

            if (WarStompTimer <= diff)
            {
				DoCastAOE(RAID_MODE(SPELL_WAR_STOMP_10, SPELL_WAR_STOMP_25));
				WarStompTimer = 6000;
            }
			else WarStompTimer -= diff;

            if (ExecuteTimer <= diff)
            {
                if (me->GetVictim() && me->GetVictim()->HealthBelowPct(HEALTH_PERCENT_20))
				{
					DoCastVictim(RAID_MODE(SPELL_EXECUTE_10, SPELL_EXECUTE_25));
					ExecuteTimer = 8000;
				}
            }
			else ExecuteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_patchwork_golemAI(creature);
    }
};

class npc_bile_retcher : public CreatureScript
{
public:
    npc_bile_retcher() : CreatureScript("npc_bile_retcher") { }

    struct npc_bile_retcherAI : QuantumBasicAI
    {
        npc_bile_retcherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 BileVomitTimer;

        void Reset()
        {
            BileVomitTimer = 1000;

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

            if (BileVomitTimer <= diff)
            {
				DoCastVictim(RAID_MODE(SPELL_BILE_VOMIT_10, SPELL_BILE_VOMIT_25));
				BileVomitTimer = 15000;
            }
			else BileVomitTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bile_retcherAI(creature);
    }
};

class npc_sludge_belcher : public CreatureScript
{
public:
    npc_sludge_belcher() : CreatureScript("npc_sludge_belcher") { }

    struct npc_sludge_belcherAI : QuantumBasicAI
    {
        npc_sludge_belcherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 AcidicSludgeTimer;

        void Reset()
        {
            AcidicSludgeTimer = 500;

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

            if (AcidicSludgeTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_ACIDIC_SLUDGE_10, SPELL_ACIDIC_SLUDGE_25));
					AcidicSludgeTimer = urand (4*IN_MILLISECONDS, 5000);
				}
            }
			else AcidicSludgeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sludge_belcherAI(creature);
    }
};

class npc_sewage_slime : public CreatureScript
{
public:
    npc_sewage_slime() : CreatureScript("npc_sewage_slime") { }

    struct npc_sewage_slimeAI : QuantumBasicAI
    {
        npc_sewage_slimeAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        void Reset()
        {
			DoCast(me, SPELL_SL_DISEASE_CLOUD);
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
		}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sewage_slimeAI(creature);
    }
};

class npc_mad_scientist : public CreatureScript
{
public:
     npc_mad_scientist() : CreatureScript("npc_mad_scientist") { }

    struct npc_mad_scientistAI : QuantumBasicAI
    {
		npc_mad_scientistAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 GreatHealTimer;
        uint32 ManaBurnTimer;

        void Reset()
        {
            GreatHealTimer = 1000;
            ManaBurnTimer = 500;

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

            if (GreatHealTimer <= diff)
            {
                if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, RAID_MODE(SPELL_GREAT_HEAL_10, SPELL_GREAT_HEAL_25));
					GreatHealTimer = urand(5000, 6000);
				}
            }
			else GreatHealTimer -= diff;

            if (ManaBurnTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
                    if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, RAID_MODE(SPELL_MANA_BURN_10, SPELL_MANA_BURN_25));
						ManaBurnTimer = urand(7000, 8000);
					}
                }
            }
			else ManaBurnTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mad_scientistAI(creature);
    }
};

class npc_living_monstrosity : public CreatureScript
{
public:
	npc_living_monstrosity() : CreatureScript("npc_living_monstrosity") { }

    struct npc_living_monstrosityAI : QuantumBasicAI
    {
        npc_living_monstrosityAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ChainLightningTimer;
        uint32 TotemTimer;

        void Reset()
        {
            ChainLightningTimer = 500;
            TotemTimer = 2000;

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

            if (ChainLightningTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_CHAIN_LIGHTNING_10, SPELL_CHAIN_LIGHTNING_25));
					ChainLightningTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
            }
			else ChainLightningTimer -= diff;

            if (TotemTimer <= diff)
            {
				DoCastAOE(SPELL_LIGHTNING_TOTEM);
				TotemTimer = urand(10000, 12000);
            }
			else TotemTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_living_monstrosityAI(creature);
    }
};

class npc_surgical_assistant : public CreatureScript
{
public:
	npc_surgical_assistant() : CreatureScript("npc_surgical_assistant") { }

    struct npc_surgical_assistantAI : QuantumBasicAI
    {
        npc_surgical_assistantAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 MindFlayTimer;

        void Reset()
        {
            MindFlayTimer = 500;

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

            if (MindFlayTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_SA_MIND_FLAY_10, SPELL_SA_MIND_FLAY_25));
					MindFlayTimer = urand(4*IN_MILLISECONDS, 5000);
				}
            }
			else MindFlayTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_surgical_assistantAI(creature);
    }
};

class npc_embalming_slime : public CreatureScript
{
public:
	npc_embalming_slime() : CreatureScript("npc_embalming_slime") { }

    struct npc_embalming_slimeAI : QuantumBasicAI
    {
        npc_embalming_slimeAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 EmbalmingCloudTimer;

        void Reset()
        {
            EmbalmingCloudTimer = 2000;

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

            if (EmbalmingCloudTimer <= diff)
            {
				DoCastAOE(SPELL_EMBALMING_CLOUD);
				EmbalmingCloudTimer = 4*IN_MILLISECONDS;
            }
			else EmbalmingCloudTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_embalming_slimeAI(creature);
    }
};

class npc_stitched_giant : public CreatureScript
{
public:
	npc_stitched_giant() : CreatureScript("npc_stitched_giant") { }

    struct npc_stitched_giantAI : QuantumBasicAI
    {
        npc_stitched_giantAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 KnockbackTimer;

        void Reset()
        {
            KnockbackTimer = 2000;

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

            if (KnockbackTimer <= diff)
            {
				DoCastAOE(SPELL_KNOCKBACK);
				KnockbackTimer = urand(5000, 7000);
            }
			else KnockbackTimer -= diff;

            if (HealthBelowPct(HEALTH_PERCENT_50))
            {
				if (!me->HasAuraEffect(SPELL_UNSTOPPABLE_ENRAGE, 0))
				{
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
					DoCast(me, SPELL_UNSTOPPABLE_ENRAGE);
				}
            }

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stitched_giantAI(creature);
    }
};

class npc_stitched_colossus : public CreatureScript
{
public:
	npc_stitched_colossus() : CreatureScript("npc_stitched_colossus") { }

    struct npc_stitched_colossusAI : QuantumBasicAI
    {
        npc_stitched_colossusAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 KnockbackTimer;

        void Reset()
        {
            KnockbackTimer = 2000;

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

            if (KnockbackTimer <= diff)
            {
				DoCastAOE(RAID_MODE(SPELL_MASSIVE_STOMP_10, SPELL_MASSIVE_STOMP_25));
				KnockbackTimer = urand(5000, 7000);
            }
			else KnockbackTimer -= diff;

            if (HealthBelowPct(HEALTH_PERCENT_50))
            {
				if (!me->HasAuraEffect(SPELL_UNSTOPPABLE_ENRAGE, 0))
				{
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
					DoCast(me, SPELL_UNSTOPPABLE_ENRAGE);
				}
            }

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stitched_colossusAI(creature);
    }
};

class npc_deathcharger_steed : public CreatureScript
{
public:
	npc_deathcharger_steed() : CreatureScript("npc_deathcharger_steed") { }

    struct npc_deathcharger_steedAI : QuantumBasicAI
    {
		npc_deathcharger_steedAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ChargeTimer;

        void Reset()
        {
            ChargeTimer = 200;

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

            if (ChargeTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CHARGE);
					ChargeTimer = 6000;
				}
            }
			else ChargeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_deathcharger_steedAI(creature);
    }
};

class npc_death_knight_captain : public CreatureScript
{
public:
	npc_death_knight_captain() : CreatureScript("npc_death_knight_captain") { }

    struct npc_death_knight_captainAI : QuantumBasicAI
    {
        npc_death_knight_captainAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 PlagueStrikeTimer;
        uint32 RaiseDeathTimer;

        void Reset()
        {
            PlagueStrikeTimer = 1000;
            RaiseDeathTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoCast(me, SPELL_UNHOLY_PRESENCE);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (PlagueStrikeTimer <= diff)
            {
				DoCastVictim(RAID_MODE(SPELL_PLAGUE_STRIKE_10, SPELL_PLAGUE_STRIKE_25));
				PlagueStrikeTimer = urand(5000, 6000);
            }
			else PlagueStrikeTimer -= diff;

            if (RaiseDeathTimer <= diff)
            {
				DoCast(me, SPELL_RAISE_DEATH);
				RaiseDeathTimer = urand(10000, 12000);
            }
			else RaiseDeathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_death_knight_captainAI(creature);
    }
};

class npc_death_knight : public CreatureScript
{
public:
	npc_death_knight() : CreatureScript("npc_death_knight") { }

    struct npc_death_knightAI : QuantumBasicAI
    {
        npc_death_knightAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 DeathCoilTimer;
        uint32 HysteriaTimer;

        void Reset()
        {
            DeathCoilTimer = 1000;
            HysteriaTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoCast(me, SPELL_BLOOD_PRESENCE);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (DeathCoilTimer <= diff)
            {
                if (rand()%2 == 0)
                {
                    if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
					{
						DoCast(target, SPELL_DEATH_COIL_HEAL);
						DeathCoilTimer = 5000;
					}
                }
				else
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, RAID_MODE(SPELL_DEATH_COIL_DAMAGE_10, SPELL_DEATH_COIL_DAMAGE_25));
						DeathCoilTimer = 5000;
					}
                }
            }
			else DeathCoilTimer -= diff;

            if (HysteriaTimer <= diff)
            {
				std::list<Creature*> FriendlyList = DoFindFriendlyMissingBuff(15.0f, SPELL_HYSTERIA);
				if (!FriendlyList.empty())
				{
					if (Unit* target = *(FriendlyList.begin()))
					{
						DoCast(target, SPELL_HYSTERIA);
						HysteriaTimer = 8000;
					}
				}
            }
			else HysteriaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_death_knightAI(creature);
    }
};

class npc_risen_squire : public CreatureScript
{
public:
	npc_risen_squire() : CreatureScript("npc_risen_squire") { }

    struct npc_risen_squireAI : QuantumBasicAI
    {
        npc_risen_squireAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 PieceArmorTimer;

        void Reset()
        {
            PieceArmorTimer = 1000;

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

            if (PieceArmorTimer <= diff)
            {
				DoCastVictim(SPELL_PIERCE_ARMOR);
				PieceArmorTimer = urand(5000, 6000);
            }
			else PieceArmorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_risen_squireAI(creature);
    }
};

class npc_dark_touched_warrior : public CreatureScript
{
public:
	npc_dark_touched_warrior() : CreatureScript("npc_dark_touched_warrior") { }

    struct npc_dark_touched_warriorAI : QuantumBasicAI
    {
        npc_dark_touched_warriorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 WhirlwindTimer;

        void Reset()
        {
            WhirlwindTimer = 2000;

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

            if (WhirlwindTimer <= diff)
            {
				DoCastAOE(SPELL_DTW_WHIRLWIND);
				WhirlwindTimer = 8000;
            }
			else WhirlwindTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_touched_warriorAI(creature);
    }
};

class npc_death_knight_cavalier : public CreatureScript
{
public:
	npc_death_knight_cavalier() : CreatureScript("npc_death_knight_cavalier") { }

    struct npc_death_knight_cavalierAI : QuantumBasicAI
    {
        npc_death_knight_cavalierAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 BoneArmorTimer;
        uint32 IcyTouchTimer;
        uint32 StrangulateTimer;

        void Reset()
        {
            BoneArmorTimer = 500;
            IcyTouchTimer = 1000;
            StrangulateTimer = 3000;

			me->RemoveMount();

			me->Mount(MOUNT_DEATHCHARGER_ID);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void JustDied(Unit* /*victim*/)
        {
            me->InterruptNonMeleeSpells(false);

            DoCast(me, SPELL_DISMOUNT_DEATHCHARGER, true);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (BoneArmorTimer <= diff)
            {
				DoCast(me, RAID_MODE(SPELL_BONE_ARMOR_10, SPELL_BONE_ARMOR_25));
				BoneArmorTimer = 4*IN_MILLISECONDS;
            }
			else BoneArmorTimer -= diff;

            if (IcyTouchTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_ICY_TOUCH_10, SPELL_ICY_TOUCH_25));
					IcyTouchTimer = 5000;
				}
            }
			else IcyTouchTimer -= diff;

            if (StrangulateTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_STRANGULATE_10, SPELL_STRANGULATE_25));
					StrangulateTimer = 7000;
				}
            }
			else StrangulateTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_death_knight_cavalierAI(creature);
    }
};

class npc_shade_of_naxxramas : public CreatureScript
{
public:
	npc_shade_of_naxxramas() : CreatureScript("npc_shade_of_naxxramas") { }

    struct npc_shade_of_naxxramasAI : QuantumBasicAI
    {
        npc_shade_of_naxxramasAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		SummonList Summons;

        uint32 ShadowBoltVolleyTimer;
        uint32 SummonPortalTimer;

        void Reset()
        {
            ShadowBoltVolleyTimer = 500;
            SummonPortalTimer = 1000;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_INVISIBLE);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
        {
            me->RemoveAurasDueToSpell(SPELL_INVISIBLE);
        }

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_PORTAL_OF_SHADOWS)
			{
				summon->SetObjectScale(1.5f);
				Summons.Summon(summon);
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ShadowBoltVolleyTimer <= diff)
            {
				DoCastAOE(RAID_MODE(SPELL_SHADOW_BOLT_VOLLEY_10, SPELL_SHADOW_BOLT_VOLLEY_25));
				ShadowBoltVolleyTimer = urand(4*IN_MILLISECONDS, 5000);
            }
			else ShadowBoltVolleyTimer -= diff;

            if (SummonPortalTimer <= diff)
            {
				DoCastAOE(SPELL_SUMMON_PORTAL_OF_SHADOWS);
				SummonPortalTimer = urand(9000, 10000);
            }
			else SummonPortalTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shade_of_naxxramasAI(creature);
    }
};

class npc_necro_knight : public CreatureScript
{
public:
	npc_necro_knight() : CreatureScript("npc_necro_knight") { }

    struct npc_necro_knightAI : QuantumBasicAI
    {
        npc_necro_knightAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ConeOfColdTimer;
		uint32 FlamestrikeTimer;
		uint32 ArcaneExplosionTimer;
		uint32 BlastWaveTimer;
		uint32 BlinkTimer;

        void Reset()
        {
			ConeOfColdTimer = 1000;
			FlamestrikeTimer = 3000;
			BlastWaveTimer = 5000;
            ArcaneExplosionTimer = 7000;
			BlinkTimer = 9000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* victim)
        {
			DoCastAOE(SPELL_NC_FROST_NOVA);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ConeOfColdTimer <= diff)
            {
				DoCast(SPELL_CONE_OF_COLD);
				ConeOfColdTimer = urand(3000, 4*IN_MILLISECONDS);
            }
			else ConeOfColdTimer -= diff;

			if (FlamestrikeTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FLAMESTRIKE);
					FlamestrikeTimer = urand(5000, 6000);
				}
            }
			else FlamestrikeTimer -= diff;

			if (BlastWaveTimer <= diff)
            {
				DoCastAOE(SPELL_BLAST_WAVE);
				BlastWaveTimer = urand(7000, 8000);
            }
			else BlastWaveTimer -= diff;

			if (ArcaneExplosionTimer <= diff)
            {
				DoCastAOE(SPELL_ARCANE_EXPLOSION);
				ArcaneExplosionTimer = urand(9000, 10000);
            }
			else ArcaneExplosionTimer -= diff;

			if (BlinkTimer <= diff)
            {
				DoCast(me, SPELL_BLINK);
				BlinkTimer = urand(11000, 12000);
            }
			else BlinkTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_necro_knightAI(creature);
    }
};

class npc_bony_construct : public CreatureScript
{
public:
	npc_bony_construct() : CreatureScript("npc_bony_construct") { }

    struct npc_bony_constructAI : QuantumBasicAI
    {
        npc_bony_constructAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 BrutalSwipeTimer;

        void Reset()
        {
            BrutalSwipeTimer = 1000;

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

            if (BrutalSwipeTimer <= diff)
            {
				DoCastVictim(RAID_MODE(SPELL_BRUTAL_SWIPE_10, SPELL_BRUTAL_SWIPE_25));
				BrutalSwipeTimer = urand(3000, 50000);
            }
			else BrutalSwipeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bony_constructAI(creature);
    }
};

class npc_skeletal_smith : public CreatureScript
{
public:
	npc_skeletal_smith() : CreatureScript("npc_skeletal_smith") { }

    struct npc_skeletal_smithAI : QuantumBasicAI
    {
        npc_skeletal_smithAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 DisarmTimer;
        uint32 CrushArmorTimer;

        void Reset()
        {
            DisarmTimer = 1000;
            CrushArmorTimer = 2000;

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

            if (DisarmTimer <= diff)
            {
				DoCastVictim(SPELL_DISARM);
				DisarmTimer = 5*IN_MILLISECONDS;
            }
			else DisarmTimer -= diff;

            if (CrushArmorTimer <= diff)
            {
				DoCastVictim(SPELL_CRUSH_ARMOR);
				CrushArmorTimer = 6000;
            }
			else CrushArmorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skeletal_smithAI(creature);
    }
};

class npc_unholy_axe : public CreatureScript
{
public:
	npc_unholy_axe() : CreatureScript("npc_unholy_axe") { }

    struct npc_unholy_axeAI : QuantumBasicAI
    {
        npc_unholy_axeAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 MortalStrikeTimer;
        uint32 WhirlwindTimer;

        void Reset()
        {
            MortalStrikeTimer = 2000;
            WhirlwindTimer = 4*IN_MILLISECONDS;

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

            if (MortalStrikeTimer <= diff)
            {
				DoCastVictim(SPELL_MORTAL_STRIKE);
				MortalStrikeTimer = urand(4*IN_MILLISECONDS, 5000);
            }
			else MortalStrikeTimer -= diff;

            if (WhirlwindTimer <= diff)
            {
				DoCastAOE(SPELL_UA_WHIRLWIND);
				WhirlwindTimer = urand(6000, 7000);
            }
			else WhirlwindTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unholy_axeAI(creature);
    }
};

class npc_unholy_staff : public CreatureScript
{
public:
	npc_unholy_staff() : CreatureScript("npc_unholy_staff") { }

    struct npc_unholy_staffAI : QuantumBasicAI
    {
        npc_unholy_staffAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ArcaneExplosionTimer;
        uint32 FrostNovaTimer;
        uint32 PolymorphTimer;

        void Reset()
        {
            ArcaneExplosionTimer = 2000;
            FrostNovaTimer = 4*IN_MILLISECONDS;
            PolymorphTimer = 6000;

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

            if (ArcaneExplosionTimer <= diff)
            {
				DoCastAOE(RAID_MODE(SPELL_US_ARCANE_EXPLOSION_10, SPELL_US_ARCANE_EXPLOSION_25));
				ArcaneExplosionTimer = urand(3000, 4*IN_MILLISECONDS);
            }
			else ArcaneExplosionTimer -= diff;

            if (FrostNovaTimer <= diff)
            {
				DoCastAOE(SPELL_FROST_NOVA);
				FrostNovaTimer = urand(5000, 6000);
            }
			else FrostNovaTimer -= diff;

            if (PolymorphTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
					DoCast(target, SPELL_POLYMORPH);
					PolymorphTimer = urand(7000, 8000);
				}
            }
			else PolymorphTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unholy_staffAI(creature);
    }
};

class npc_unholy_swords : public CreatureScript
{
public:
	npc_unholy_swords() : CreatureScript("npc_unholy_swords") { }

    struct npc_unholy_swordsAI : QuantumBasicAI
    {
        npc_unholy_swordsAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 CleaveTimer;

        void Reset()
        {
            CleaveTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
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

            if (CleaveTimer <= diff)
            {
				DoCastVictim(SPELL_US_CLEAVE);
				CleaveTimer = 4*IN_MILLISECONDS;
            }
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unholy_swordsAI(creature);
    }
};

class npc_plagued_warrior : public CreatureScript
{
public:
    npc_plagued_warrior() : CreatureScript("npc_plagued_warrior") {}

    struct npc_plagued_warriorAI : public QuantumBasicAI
    {
        npc_plagued_warriorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CleaveTimer;

        void Reset()
        {
			CleaveTimer = 2000;

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

			if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_PW_CLEAVE);
                CleaveTimer = 4*IN_MILLISECONDS;
            }
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_plagued_warriorAI(creature);
    }
};

class npc_plagued_champion : public CreatureScript
{
public:
    npc_plagued_champion() : CreatureScript("npc_plagued_champion") {}

    struct npc_plagued_championAI : public QuantumBasicAI
    {
        npc_plagued_championAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 MortalStrikeTimer;
		uint32 ShadowShockTimer;

        void Reset()
        {
			MortalStrikeTimer = 2000;
			ShadowShockTimer = 3000;

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

			if (MortalStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_PC_MORTAL_STRIKE);
                MortalStrikeTimer = 4*IN_MILLISECONDS;
            }
			else MortalStrikeTimer -= diff;

            if (ShadowShockTimer <= diff)
            {
                DoCastAOE(RAID_MODE(SPELL_SHADOW_SHOCK_10, SPELL_SHADOW_SHOCK_25));
                ShadowShockTimer = 6000;
            }
			else ShadowShockTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_plagued_championAI(creature);
    }
};

class npc_plagued_guardian : public CreatureScript
{
public:
    npc_plagued_guardian() : CreatureScript("npc_plagued_guardian") {}

    struct npc_plagued_guardianAI : QuantumBasicAI
    {
        npc_plagued_guardianAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ArcaneExplosionTimer;
        uint32 BlinkTimer;

        void Reset()
        {
            ArcaneExplosionTimer = 2000;
            BlinkTimer = 4*IN_MILLISECONDS;

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

			if (ArcaneExplosionTimer <= diff)
            {
                DoCastAOE(RAID_MODE(SPELL_ARCANE_EXPLOSION_10, SPELL_ARCANE_EXPLOSION_25));
                ArcaneExplosionTimer = urand(3000, 4*IN_MILLISECONDS);
            }
			else ArcaneExplosionTimer -= diff;

            if (BlinkTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, RAND(SPELL_BLINK_1, SPELL_BLINK_2, SPELL_BLINK_3, SPELL_BLINK_4));
					BlinkTimer = urand(6000, 7000);
				}
            }
			else BlinkTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_plagued_guardianAI(creature);
    }
};

class npc_soldier_of_the_frozen_wastes : public CreatureScript
{
public:
    npc_soldier_of_the_frozen_wastes() : CreatureScript("npc_soldier_of_the_frozen_wastes") {}

    struct npc_soldier_of_the_frozen_wastesAI : QuantumBasicAI
    {
        npc_soldier_of_the_frozen_wastesAI(Creature* creature) : QuantumBasicAI(creature)
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
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

        void JustDied(Unit* /*killer*/)
        {
            DoCast(me, RAID_MODE(SPELL_DARK_BLAST_10, SPELL_DARK_BLAST_25), true);
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
		return new npc_soldier_of_the_frozen_wastesAI(creature);
	}
};

class npc_soul_weaver : public CreatureScript
{
public:
    npc_soul_weaver() : CreatureScript("npc_soul_weaver") {}

    struct npc_soul_weaverAI : QuantumBasicAI
    {
        npc_soul_weaverAI(Creature* creature) : QuantumBasicAI(creature)
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
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void EnterToBattle(Unit* /*who*/)
        {
			DoCast(me, RAID_MODE(SPELL_WAILS_OF_SOULS_10, SPELL_WAILS_OF_SOULS_25));
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
		return new npc_soul_weaverAI(creature);
	}
};

class npc_fallout_slime : public CreatureScript
{
public:
    npc_fallout_slime() : CreatureScript("npc_fallout_slime") { }

    struct npc_fallout_slimeAI : QuantumBasicAI
    {
        npc_fallout_slimeAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_FS_DISEASE_CLOUD);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fallout_slimeAI(creature);
    }
};

class npc_marauding_geist : public CreatureScript
{
public:
    npc_marauding_geist() : CreatureScript("npc_marauding_geist") {}

    struct npc_marauding_geistAI : QuantumBasicAI
    {
        npc_marauding_geistAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 FrenziedLeapTimer;

        void Reset()
        {
            FrenziedLeapTimer = 200;

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

			if (FrenziedLeapTimer <= diff)
            {
                DoCast(SPELL_FRENZIED_LEAP);
                FrenziedLeapTimer = 5000;
            }
			else FrenziedLeapTimer -= diff;

			if (me->HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (me->HasAuraEffect(SPELL_ENRAGE, 0))
				{
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
					DoCast(me, SPELL_ENRAGE);
				}
			}

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_marauding_geistAI(creature);
    }
};

void AddSC_naxxramas_trash()
{
	new npc_dread_creeper();
	new npc_carrion_spinner();
	new npc_venom_stalker();
	new npc_crypt_reaver();
	new npc_tomb_horror();
	new npc_naxxramas_cultist();
	new npc_naxxramas_acolyte();
	new npc_vigilant_shade();
	new npc_plague_slime();
	new npc_infectious_ghoul();
	new npc_stoneskin_gargoyle();
	new npc_mutated_grub();
	new npc_plague_beast();
	new npc_frenzied_bat();
	new npc_eye_stalk();
	new npc_patchwork_golem();
	new npc_bile_retcher();
	new npc_sludge_belcher();
	new npc_sewage_slime();
	new npc_mad_scientist();
	new npc_living_monstrosity();
	new npc_surgical_assistant();
	new npc_embalming_slime();
	new npc_stitched_giant();
	new npc_stitched_colossus();
	new npc_deathcharger_steed();
	new npc_death_knight_captain();
	new npc_death_knight();
	new npc_risen_squire();
	new npc_dark_touched_warrior();
	new npc_death_knight_cavalier();
	new npc_shade_of_naxxramas();
	new npc_necro_knight();
	new npc_bony_construct();
	new npc_skeletal_smith();
	new npc_unholy_axe();
	new npc_unholy_staff();
	new npc_unholy_swords();
	new npc_plagued_warrior();
	new npc_plagued_champion();
	new npc_plagued_guardian();
	new npc_soldier_of_the_frozen_wastes();
	new npc_soul_weaver();
	new npc_fallout_slime();
	new npc_marauding_geist();
}