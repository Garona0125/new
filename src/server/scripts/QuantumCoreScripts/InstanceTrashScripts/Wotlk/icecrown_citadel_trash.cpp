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
#include "../../../scripts/Northrend/IcecrownCitadel/icecrown_citadel.h"

enum Spells
{
	SPELL_DUAL_WIELD              = 674,
	SPELL_SPAWN_ESSENCE           = 47693,
	SPELL_FROST_CHANNEL           = 45846,
	SPELL_BONE_FLURRY             = 70960,
    SPELL_SHATTERED_BONES         = 70961, // Need Scripting
	SPELL_SABER_LASH              = 71021,
	SPELL_DISRUPTING_SHOUT        = 71022,
	SPELL_GLACIAL_BLAST           = 71029,
	SPELL_SHIELD_BASH             = 70964,
	SPELL_CRYPT_SCARABS           = 70965,
	SPELL_DARK_MENDING            = 71020,
	SPELL_WEB_WRAP                = 70980,
	SPELL_WEB_WRAP_STUN           = 71010,
	SPELL_CHAOS_BOLT_10           = 69576,
	SPELL_CHAOS_BOLT_25           = 71108,
	SPELL_CURSE_OF_AGONY_10       = 69404,
	SPELL_CURSE_OF_AGONY_25       = 71112,
	SPELL_CONSUMING_SHADOWS       = 69405,
	SPELL_SHADOW_BOLT             = 69387,
    SPELL_DARK_BLESSING           = 69391,
	SPELL_SHADOW_MEND_10          = 69389,
	SPELL_SHADOW_MEND_25          = 71107,
	SPELL_SHADOW_NOVA_10          = 69355,
	SPELL_SHADOW_NOVA_25          = 71106,
	SPELL_SHADOW_CLEAVE           = 69492,
	SPELL_AURA_OF_DARKNESS        = 69491,
	SPELL_DARK_RECKONING          = 69483,
	SPELL_SF_BLIZZARD_10          = 70362,
	SPELL_SF_BLIZZARD_25          = 71118,
	SPELL_SF_CLEAVE               = 70361,
	SPELL_FROST_BREATH_10         = 70116,
	SPELL_FROST_BREATH_25         = 72641,
	SPELL_FA_CLEAVE               = 70191,
	SPELL_FA_ENRAGE               = 70371,
	SPELL_POISON_SPIT             = 70189,
	SPELL_CANNIBALIZE             = 70363,
	SPELL_GHOUL_SLASH             = 70396,
	SPELL_SMITE_10                = 69967,
	SPELL_SMITE_25                = 71146,
	SPELL_GREATER_HEAL_10         = 69963,
	SPELL_GREATER_HEAL_25         = 71131,
	SPELL_PAIN_SUPRESSION         = 69910,
	SPELL_CURSE_OF_DOOM_10        = 69969,
	SPELL_CURSE_OF_DOOM_25        = 71124,
	SPELL_INCINERATE_10           = 69973,
	SPELL_INCINERATE_25           = 71135,
	SPELL_SHADOW_BOLT_10          = 69972,
	SPELL_SHADOW_BOLT_25          = 71143,
	SPELL_WRATH_10                = 69968,
	SPELL_WRATH_25                = 71148,
	SPELL_HEALING_TOUCH_10        = 69899,
	SPELL_HEALING_TOUCH_25        = 71121,
	SPELL_REJUVENATION_10         = 69898,
	SPELL_REJUVENATION_25         = 71142,
	SPELL_REGROWTH_10             = 69882,
	SPELL_REGROWTH_25             = 71141,
	SPELL_EXPLOSIVE_SHOT_10       = 69975,
	SPELL_EXPLOSIVE_SHOT_25       = 71126,
	SPELL_ARCANE_SHOT_10          = 69989,
	SPELL_ARCANE_SHOT_25          = 71116,
	SPELL_SHOOT_10                = 69974,
	SPELL_SHOOT_25                = 71144,
	SPELL_THREAT_AURA             = 70115,
	SPELL_ICY_TOUCH               = 69916,
	SPELL_PLAGUE_STRIKE           = 69912,
	SPELL_SPELL_REFLECT           = 69901,
	SPELL_DEVASTATE               = 69902,
	SPELL_SHIELD_SLAM_10          = 69903,
	SPELL_SHIELD_SLAM_25          = 72645,
	SPELL_THUNDERCLAP_10          = 69965,
	SPELL_THUNDERCLAP_25          = 71147,
	SPELL_AVENGERS_SHIELD_10      = 69927,
	SPELL_AVENGERS_SHIELD_25      = 71117,
	SPELL_CONSECRATION_10         = 69930,
	SPELL_CONSECRATION_25         = 71122,
	SPELL_HOLY_WRATH_10           = 69934,
	SPELL_HOLY_WRATH_25           = 71134,
	SPELL_LIGHTNING_BOLT_10       = 69970,
	SPELL_LIGHTNING_BOLT_25       = 71136,
	SPELL_EARTH_SHIELD            = 69926,
	SPELL_CHAIN_HEAL_10           = 69923,
	SPELL_CHAIN_HEAL_25           = 71120,
	SPELL_HEALING_WAVE_10         = 69958,
	SPELL_HEALING_WAVE_25         = 71133,
	SPELL_FAN_OF_KNIVES_10        = 69921,
	SPELL_FAN_OF_KNIVES_25        = 71128,
	SPELL_SINISTER_STRIKE_10      = 69920,
	SPELL_SINISTER_STRIKE_25      = 71145,
	SPELL_SHADOWSTEP              = 70431,
	SPELL_FROSTFIRE_BOLT_10       = 69869,
	SPELL_FROSTFIRE_BOLT_25       = 71130,
	SPELL_BLINK                   = 69904,
	SPELL_SUMMON_SKYBREAKER_ST    = 69810,
	SPELL_SUMMON_KORKRON_ST       = 69811,
	SPELL_SKYBREAKER_AURA         = 69808,
	SPELL_KORKRON_AURA            = 69809,
	SPELL_SEVERED_ESSENCE_10      = 71906,
	SPELL_SEVERED_ESSENCE_25      = 71942,
	SPELL_CLEAVE                  = 40504,
    SPELL_PLAGUE_CLOUD            = 71150,
    SPELL_SCOURGE_HOOK            = 71140,
	SPELL_LEAPING_FACE_MAUL       = 71164,
	SPELL_PLAGUE_BLAST            = 73079,
    SPELL_PLAGUE_STREAM           = 69871,
    SPELL_COMBOBULATING_SPRAY     = 71103,
	SPELL_BUBBLING_PUS_10         = 71089,
	SPELL_BUBBLING_PUS_25         = 71090,
	SPELL_BLIGHT_BOMB             = 71088,
	SPELL_MASSIVE_STOMP_10        = 71114,
	SPELL_MASSIVE_STOMP_25        = 71115,
	SPELL_AMPLIFY_MAGIC_10        = 70408,
	SPELL_AMPLIFY_MAGIC_25        = 72336,
	SPELL_FIREBALL_10             = 70409,
	SPELL_FIREBALL_25             = 71153,
	SPELL_BLAST_WAVE_10           = 70407,
	SPELL_BLAST_WAVE_25           = 71151,
	SPELL_POLYMORPH_SPIDER        = 70410,
	SPELL_VAMPIRIC_AURA           = 71736,
    SPELL_UNHOLY_STRIKE           = 70437,
	SPELL_CHAINS_OF_SHADOW        = 70645,
	SPELL_NOOBLE_SHADOW_BOLT_10   = 72960,
	SPELL_NOOBLE_SHADOW_BOLT_25   = 72961,
	SPELL_SHROUD_OF_PROTECTION    = 72065,
    SPELL_SHROUD_OF_SPELL_WARDING = 72066,
	SPELL_LICH_SLAP_10            = 72057,
	SPELL_LICH_SLAP_25            = 72421,
	SPELL_BATTLE_SHOUT            = 70750,
	SPELL_VAMPIRE_RUSH_10         = 70449,
	SPELL_VAMPIRE_RUSH_25         = 71155,
    SPELL_VAMPIRIC_CURSE          = 70423,
	SPELL_REND_FLESH_10           = 70435,
	SPELL_REND_FLESH_25           = 71154,
	SPELL_BLOOD_SAP               = 70432,
	SPELL_DISEASE_CLOUD           = 41290,
	SPELL_LEECHING_ROT            = 70671,
	SPELL_LEECHING_EVF_ROT        = 70710,
	SPELL_EMPOWERED_BLOOD         = 70711,
	SPELL_TWISTED_WINDS           = 71306,
    SPELL_SPIRIT_STREAM           = 69929,
	SPELL_SUMMON_YMIRJAR          = 71303,
	SPELL_SHADOW_BOLT_YMIRJAR_10  = 71296,
    SPELL_SHADOW_BOLT_YMIRJAR_25  = 71297,
    SPELL_DEATHS_EMBRACE_10       = 71299,
    SPELL_DEATHS_EMBRACE_25       = 71300,
	SPELL_BANISH                  = 71298,
	SPELL_ARCTIC_CHILL            = 71270,
    SPELL_FROZEN_ORB              = 71274,
    SPELL_BARBARIC_STRIKE         = 71257,
	SPELL_ADRENALINE_RUSH         = 71258,
	SPELL_ICE_TRAP                = 71249,
    SPELL_RAPID_SHOT              = 71251,
    SPELL_SHOOT                   = 71253,
    SPELL_VOLLEY                  = 71252,
    SPELL_SUMMON_WARHAWK          = 71705,
	SPELL_WHIRLWIND               = 41056,
	SPELL_NW_CRYPT_SCARABS        = 71326,
	SPELL_WEB                     = 71327,
	SPELL_RUSH                    = 71801,
	SPELL_FROSTBOLT               = 71318,
	SPELL_FROST_NOVA              = 71320,
	SPELL_ICE_TOMB                = 71331,
	SPELL_GLACIAL_STRIKE_10       = 71316,
	SPELL_GLACIAL_STRIKE_25       = 71317,
	SPELL_FROSTBLADE              = 71325,
	SPELL_PLAGUETHROWER_BACKPACK1 = 71589,
	SPELL_PLAGUETHROWER_BACKPACK2 = 71856,
	SPELL_PLAGUETHROWER_BACKPACK3 = 71869,
	SPELL_SHADOW_CHANNELING       = 43897,
	SPELL_AGGRO_FOR_ME            = 53623,
	SPELL_WARLORDS_PRESENCE       = 71244,
	SPELL_INFECTED_WOUND          = 71157,
	SPELL_VILE_GASS               = 69240,
	SPELL_SE_AVENGING_WRATH       = 66011,
	SPELL_SE_CRUSADER_STRIKE      = 71549,
	SPELL_SE_RADIANCE_AURA        = 71953,
	SPELL_SE_PLAGUE_STRIKE        = 71924,
	SPELL_SE_MANGLE               = 71925,
	SPELL_SE_RIP                  = 71926,
	SPELL_SE_FOCUSED_ATTACKS      = 71955,
	SPELL_SE_EVISCERATE           = 71933,
	SPELL_SE_EXPLOSIVE_SHOT       = 71126,
	SPELL_SE_SHOOT                = 71927,
	SPELL_SE_FROSTBOLT            = 71889,
	SPELL_SE_FIREBALL             = 71928,
	SPELL_SE_FROST_NOVA           = 71929,
	SPELL_SE_FLASH_OF_LIGHT       = 71930,
	SPELL_SE_SMITE                = 71546,
	SPELL_SE_GREATER_HEAL         = 71931,
	SPELL_SE_RENEW                = 71932,
	SPELL_SE_LIGHTNING_BOLT       = 71934,
	SPELL_SE_THUNDERSTORM         = 71935,
	SPELL_SE_SHADOW_BOLT          = 71936,
	SPELL_SE_CORRUPTION           = 71937,
	SPELL_SE_BLOODTHIRST          = 71938,
	SPELL_SE_NECROTIC_STRIKE      = 71951,
	SPELL_SE_REPLENISHING_RAINS   = 71956,
	SPELL_SE_HEROIC_LEAP          = 71961,
	SPELL_SE_RAIN_OF_CHAOS        = 71965,
};

enum Events
{
	EVENT_FIRST_SQUAD_ASSISTED_1 = 1,
	EVENT_FIRST_SQUAD_ASSISTED_2 = 2,
	EVENT_FIRST_SQUAD_ASSISTED_3 = 3,
	EVENT_FIRST_SQUAD_ASSISTED_4 = 4,
	EVENT_FIRST_SQUAD_ASSISTED_5 = 5,
};

enum EventActions
{
	ACTION_START_HORDE_BATTLE    = 1,
	ACTION_START_ALLIANCE_BATTLE = 2,
};

enum Texts
{
	SAY_FIRST_SQUAD_RESCUED_HORDE_0     = -1420092,
	SAY_FIRST_SQUAD_RESCUED_HORDE_1     = -1420093,
	SAY_SECOND_SQUAD_RESCUED_HORDE_0    = -1420094,
	SAY_SECOND_SQUAD_RESCUED_HORDE_1    = -1420095,
	SAY_FROSTWYRM_SUMMON                = -1420096,
	SAY_FIRST_SQUAD_RESCUED_ALLIANCE_0  = -1420097,
	SAY_FIRST_SQUAD_RESCUED_ALLIANCE_1  = -1420098,
	SAY_SECOND_SQUAD_RESCUED_ALLIANCE_0 = -1420099,
	SAY_SECOND_SQUAD_RESCUED_ALLIANCE_1 = -1420100,
	SAY_SECOND_SQUAD_RESCUED_ALLIANCE_2 = -1420101,
	SAY_FROSTWYRM_LAND_H                = -1420102,
	SAY_FROSTWYRM_LAND_A                = -1420103,
	SAY_SUMMON_KORKRON_STANDARD         = -1420104,
	SAY_SUMMON_SKYBREAKER_STANDARD      = -1420105,
};

Position const FrostWyrmPosH = {-435.429f, 2077.556f, 219.1148f, 2.623434f};
Position const FrostWyrmPosA = {-437.409f, 2349.026f, 219.1148f, 3.510935f};

class npc_deathbound_ward : public CreatureScript
{
public:
	npc_deathbound_ward() : CreatureScript("npc_deathbound_ward") { }

	struct npc_deathbound_wardAI : public QuantumBasicAI
	{
		npc_deathbound_wardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 DisruptingShoutTimer;
		uint32 SaberLashTimer;

		void Reset()
		{
			SaberLashTimer = 2*IN_MILLISECONDS;
			DisruptingShoutTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_STONEFORM);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_CUSTOM_SPELL_02);
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void JustReachedHome()
		{
			Reset();
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SaberLashTimer <= diff)
			{
				DoCastVictim(SPELL_SABER_LASH);
				SaberLashTimer = 5*IN_MILLISECONDS;
			}
			else SaberLashTimer -= diff;

			if (DisruptingShoutTimer <= diff)
			{
				DoCastAOE(SPELL_DISRUPTING_SHOUT);
				DisruptingShoutTimer = 9*IN_MILLISECONDS;
			}
			else DisruptingShoutTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_deathbound_wardAI(creature);
	}
};

class npc_servant_of_the_throne : public CreatureScript
{
public:
    npc_servant_of_the_throne() : CreatureScript("npc_servant_of_the_throne") { }

    struct npc_servant_of_the_throneAI : public QuantumBasicAI
    {
        npc_servant_of_the_throneAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 GlacialBlastTimer;

        void Reset()
        {
			GlacialBlastTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (GlacialBlastTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_GLACIAL_BLAST);
					GlacialBlastTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
            }
            else GlacialBlastTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_servant_of_the_throneAI(creature);
    }
};

class npc_ancient_skeletal_soldier : public CreatureScript
{
public:
    npc_ancient_skeletal_soldier() : CreatureScript("npc_ancient_skeletal_soldier") { }

    struct npc_ancient_skeletal_soldierAI : public QuantumBasicAI
    {
        npc_ancient_skeletal_soldierAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ShieldBashTimer;

        void Reset()
        {
			ShieldBashTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ShieldBashTimer <= diff)
            {
                DoCastVictim(SPELL_SHIELD_BASH);
                ShieldBashTimer = 4*IN_MILLISECONDS;
            }
            else ShieldBashTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ancient_skeletal_soldierAI(creature);
    }
};

class npc_nerubar_broodkeeper : public CreatureScript
{
public:
	npc_nerubar_broodkeeper() : CreatureScript("npc_nerubar_broodkeeper") { }

	struct npc_nerubar_broodkeeperAI : public QuantumBasicAI
	{
		npc_nerubar_broodkeeperAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CryptScarabsTimer;
		uint32 DarkMendingTimer;
		uint32 WebWrapsTimer;

		bool WakeUp;

		void Reset()
		{
			CryptScarabsTimer = 0.5*IN_MILLISECONDS;
			WebWrapsTimer = 2*IN_MILLISECONDS;
			DarkMendingTimer = 5*IN_MILLISECONDS;

			WakeUp = false;

			me->SetCanFly(true);
			me->SetDisableGravity(true);
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void MoveInLineOfSight(Unit* who)
		{
			if (WakeUp || who->GetTypeId() != TYPE_ID_PLAYER || who->ToPlayer()->IsGameMaster() || !who->IsWithinDistInMap(me, 45.0f))
				return;

			if (WakeUp == false && who->GetTypeId() == TYPE_ID_PLAYER && who->IsWithinDistInMap(me, 45.0f))
			{
				me->GetMotionMaster()->MoveFall();
				me->SetCanFly(false);
				me->SetDisableGravity(false);
				me->SetReactState(REACT_AGGRESSIVE);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
				me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
				WakeUp = true;
			}
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

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
					DoCast(target, SPELL_CRYPT_SCARABS, true);
					CryptScarabsTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CryptScarabsTimer -= diff;

			if (WebWrapsTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
				{
					if (target->GetTypeId() == TYPE_ID_PLAYER)
						DoCast(target, SPELL_WEB_WRAP);

					WebWrapsTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else WebWrapsTimer -= diff;

			if (DarkMendingTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, SPELL_DARK_MENDING);
					DarkMendingTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
				}
			}
			else DarkMendingTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_nerubar_broodkeeperAI(creature);
	}
};

class npc_deathspeaker_servant : public CreatureScript
{
public:
    npc_deathspeaker_servant() : CreatureScript("npc_deathspeaker_servant") { }

    struct npc_deathspeaker_servantAI : public QuantumBasicAI
    {
        npc_deathspeaker_servantAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ChaosBoltTimer;
        uint32 ConsumingShadowsTimer;
        uint32 CurseOfAgonyTimer;

        void Reset()
        {
			ChaosBoltTimer = 0.5*IN_MILLISECONDS;
            ConsumingShadowsTimer = 3*IN_MILLISECONDS;
            CurseOfAgonyTimer = 4500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_KNEEL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ChaosBoltTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_CHAOS_BOLT_10, SPELL_CHAOS_BOLT_25, SPELL_CHAOS_BOLT_10, SPELL_CHAOS_BOLT_25));
					ChaosBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
            }
            else ChaosBoltTimer -= diff;

            if (ConsumingShadowsTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CONSUMING_SHADOWS);
					ConsumingShadowsTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
            }
            else ConsumingShadowsTimer -= diff;

            if (CurseOfAgonyTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, RAID_MODE(SPELL_CURSE_OF_AGONY_10, SPELL_CURSE_OF_AGONY_25, SPELL_CURSE_OF_AGONY_10, SPELL_CURSE_OF_AGONY_25));
					CurseOfAgonyTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
            }
            else CurseOfAgonyTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_deathspeaker_servantAI(creature);
    }
};

class npc_deathspeaker_disciple : public CreatureScript
{
public:
    npc_deathspeaker_disciple() : CreatureScript("npc_deathspeaker_disciple") { }

    struct npc_deathspeaker_discipleAI : public QuantumBasicAI
    {
        npc_deathspeaker_discipleAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ShadowBoltTimer;
        uint32 ShadowMendTimer;

        void Reset()
        {
			ShadowBoltTimer = 0.5*IN_MILLISECONDS;
            ShadowMendTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, SPELL_FROST_CHANNEL, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void JustReachedHome()
		{
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Dark Blessing Buff Check in Combat
			if (!me->HasAura(SPELL_DARK_BLESSING))
				DoCast(me, SPELL_DARK_BLESSING, true);

            if (ShadowBoltTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_BOLT);
					ShadowBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
            }
            else ShadowBoltTimer -= diff;

            if (ShadowMendTimer <= diff)
            {
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, RAID_MODE(SPELL_SHADOW_MEND_10, SPELL_SHADOW_MEND_25, SPELL_SHADOW_MEND_10, SPELL_SHADOW_MEND_25), true);
					ShadowMendTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
            }
            else ShadowMendTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_deathspeaker_discipleAI(creature);
    }
};

class npc_deathspeaker_attendant : public CreatureScript
{
public:
    npc_deathspeaker_attendant() : CreatureScript("npc_deathspeaker_attendant") { }

    struct npc_deathspeaker_attendantAI : public QuantumBasicAI
    {
        npc_deathspeaker_attendantAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ShadowBoltTimer;
        uint32 ShadowNovaTimer;

        void Reset()
        {
			ShadowBoltTimer = 0.5*IN_MILLISECONDS;
            ShadowNovaTimer = 2500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_KNEEL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
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
					ShadowBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
            }
            else ShadowBoltTimer -= diff;

            if (ShadowNovaTimer <= diff)
            {
                DoCastAOE(RAID_MODE(SPELL_SHADOW_NOVA_10, SPELL_SHADOW_NOVA_25, SPELL_SHADOW_NOVA_10, SPELL_SHADOW_NOVA_25));
                ShadowNovaTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
            }
            else ShadowNovaTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_deathspeaker_attendantAI(creature);
    }
};

class npc_deathspeaker_zealot : public CreatureScript
{
public:
    npc_deathspeaker_zealot() : CreatureScript("npc_deathspeaker_zealot") { }

    struct npc_deathspeaker_zealotAI : public QuantumBasicAI
    {
        npc_deathspeaker_zealotAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 ShadowCleaveTimer;

        void Reset()
        {
			ShadowCleaveTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_KNEEL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ShadowCleaveTimer <= diff)
            {
                DoCastVictim(SPELL_SHADOW_CLEAVE);
                ShadowCleaveTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
            }
            else ShadowCleaveTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_deathspeaker_zealotAI(creature);
    }
};

class npc_deathspeaker_high_priest : public CreatureScript
{
public:
    npc_deathspeaker_high_priest() : CreatureScript("npc_deathspeaker_high_priest") { }

    struct npc_deathspeaker_high_priestAI : public QuantumBasicAI
    {
        npc_deathspeaker_high_priestAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 DarkReckoningTimer;

        void Reset()
        {
			DarkReckoningTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_AURA_OF_DARKNESS);
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (DarkReckoningTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_DARK_RECKONING);
					DarkReckoningTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
            }
            else DarkReckoningTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_deathspeaker_high_priestAI(creature);
    }
};

class npc_spire_frostwyrm: public CreatureScript
{
public:
	npc_spire_frostwyrm() : CreatureScript("npc_spire_frostwyrm") { }

	struct npc_spire_frostwyrmAI : public QuantumBasicAI
	{
		npc_spire_frostwyrmAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();

			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		InstanceScript* instance;

		uint32 BlizzardTimer;
		uint32 FrostBreathTimer;
		uint32 CleaveTimer;

		bool Expired;

		void Reset()
		{
			BlizzardTimer = 3*IN_MILLISECONDS;
			FrostBreathTimer = 5*IN_MILLISECONDS;
			CleaveTimer = 7*IN_MILLISECONDS;

			Expired = false;

			me->SetCanFly(true);
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->AddUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
		}

		void EnterToBattle(Unit* /*who*/){}

		void MoveInLineOfSight(Unit* who)
		{
			if (Expired || who->GetTypeId() != TYPE_ID_PLAYER || who->ToPlayer()->IsGameMaster() || !who->IsWithinDistInMap(me, 30.0f))
				return;

			if (Expired == false && who->GetTypeId() == TYPE_ID_PLAYER && who->IsWithinDistInMap(me, 30.0f))
			{
				if (instance->GetData(DATA_TEAM_IN_INSTANCE) == HORDE)
					DoSendQuantumText(SAY_FROSTWYRM_LAND_H, me);
				else
					DoSendQuantumText(SAY_FROSTWYRM_LAND_A, me);

				me->SetCanFly(false);
				me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
				me->HandleEmoteCommand(EMOTE_ONESHOT_FLY_SIT_GROUND_DOWN);
				me->RemoveUnitMovementFlag(MOVEMENTFLAG_DISABLE_GRAVITY);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
				me->SetInCombatWith(who);
				me->AddThreat(who, 10.0f);
				me->GetMotionMaster()->MoveChase(who);
				Expired = true;
			}
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BlizzardTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 40, true))
				{
					DoCast(target, RAID_MODE(SPELL_SF_BLIZZARD_10, SPELL_SF_BLIZZARD_25, SPELL_SF_BLIZZARD_10, SPELL_SF_BLIZZARD_25));
					BlizzardTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else BlizzardTimer -= diff;

			if (FrostBreathTimer <= diff)
			{
				DoCastVictim(RAID_MODE(SPELL_FROST_BREATH_10, SPELL_FROST_BREATH_25, SPELL_FROST_BREATH_10, SPELL_FROST_BREATH_25));
				FrostBreathTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else FrostBreathTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_SF_CLEAVE);
				CleaveTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_spire_frostwyrmAI(creature);
	}
};

class npc_frenzied_abomination : public CreatureScript
{
public:
    npc_frenzied_abomination() : CreatureScript("npc_frenzied_abomination") {}

    struct npc_frenzied_abominationAI : public QuantumBasicAI
    {
        npc_frenzied_abominationAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CleaveTimer;

        void Reset()
        {
			CleaveTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_FA_CLEAVE);
				CleaveTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_FA_ENRAGE, 0))
				{
					DoCast(me, SPELL_FA_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frenzied_abominationAI(creature);
    }
};

class npc_spire_gargoyle : public CreatureScript
{
public:
    npc_spire_gargoyle() : CreatureScript("npc_spire_gargoyle") {}

    struct npc_spire_gargoyleAI : public QuantumBasicAI
    {
        npc_spire_gargoyleAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 PoisonSpitTimer;

        void Reset()
        {
			PoisonSpitTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_HOVER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PoisonSpitTimer <= diff)
			{
				DoCastVictim(SPELL_POISON_SPIT);
				PoisonSpitTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else PoisonSpitTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spire_gargoyleAI(creature);
    }
};

class npc_spire_minion : public CreatureScript
{
public:
    npc_spire_minion() : CreatureScript("npc_spire_minion") {}

    struct npc_spire_minionAI : public QuantumBasicAI
    {
        npc_spire_minionAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 GhoulSlashTimer;

        void Reset()
        {
			GhoulSlashTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GhoulSlashTimer <= diff)
			{
				DoCastVictim(SPELL_GHOUL_SLASH);
				GhoulSlashTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else GhoulSlashTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
				DoCast(me, SPELL_CANNIBALIZE);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spire_minionAI(creature);
    }
};

class npc_skybreaker_vicar : public CreatureScript
{
public:
    npc_skybreaker_vicar() : CreatureScript("npc_skybreaker_vicar") {}

    struct npc_skybreaker_vicarAI : public QuantumBasicAI
    {
        npc_skybreaker_vicarAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 SmiteTimer;
		uint32 GreaterHealTimer;
		uint32 PainSupressionTimer;

        void Reset()
        {
			SmiteTimer = 0.5*IN_MILLISECONDS;
			GreaterHealTimer = 3*IN_MILLISECONDS;
			PainSupressionTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SmiteTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_SMITE_10, SPELL_SMITE_25, SPELL_SMITE_10, SPELL_SMITE_25));
					SmiteTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else SmiteTimer -= diff;

			if (GreaterHealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, RAID_MODE(SPELL_GREATER_HEAL_10, SPELL_GREATER_HEAL_25, SPELL_GREATER_HEAL_10, SPELL_GREATER_HEAL_25));
					GreaterHealTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else GreaterHealTimer -= diff;

			if (PainSupressionTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, SPELL_PAIN_SUPRESSION);
					PainSupressionTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
				}
			}
			else PainSupressionTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_60))
				DoCast(me, RAID_MODE(SPELL_GREATER_HEAL_10, SPELL_GREATER_HEAL_25, SPELL_GREATER_HEAL_10, SPELL_GREATER_HEAL_25));

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_PAIN_SUPRESSION, 0))
					DoCast(me, SPELL_PAIN_SUPRESSION);
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skybreaker_vicarAI(creature);
    }
};

class npc_skybreaker_summoner : public CreatureScript
{
public:
    npc_skybreaker_summoner() : CreatureScript("npc_skybreaker_summoner") {}

    struct npc_skybreaker_summonerAI : public QuantumBasicAI
    {
        npc_skybreaker_summonerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CurseOfDoomTimer;
		uint32 IncinerateTimer;
		uint32 ShadowBoltTimer;

        void Reset()
        {
			CurseOfDoomTimer = 0.5*IN_MILLISECONDS;
			IncinerateTimer = 1*IN_MILLISECONDS;
			ShadowBoltTimer = 2500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CurseOfDoomTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_CURSE_OF_DOOM_10, SPELL_CURSE_OF_DOOM_25, SPELL_CURSE_OF_DOOM_10, SPELL_CURSE_OF_DOOM_25));
					CurseOfDoomTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CurseOfDoomTimer -= diff;

			if (IncinerateTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_INCINERATE_10, SPELL_INCINERATE_25, SPELL_INCINERATE_10, SPELL_INCINERATE_25));
					IncinerateTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else IncinerateTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_SHADOW_BOLT_10, SPELL_SHADOW_BOLT_25, SPELL_SHADOW_BOLT_10, SPELL_SHADOW_BOLT_25));
					ShadowBoltTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skybreaker_summonerAI(creature);
    }
};

class npc_skybreaker_hierophant : public CreatureScript
{
public:
    npc_skybreaker_hierophant() : CreatureScript("npc_skybreaker_hierophant") {}

    struct npc_skybreaker_hierophantAI : public QuantumBasicAI
    {
        npc_skybreaker_hierophantAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 WrathTimer;
		uint32 HealingTouchTimer;
		uint32 RegrowthTimer;
		uint32 RejuvenationTimer;

        void Reset()
        {
			WrathTimer = 0.5*IN_MILLISECONDS;
			HealingTouchTimer = 2*IN_MILLISECONDS;
			RegrowthTimer = 3500;
			RejuvenationTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WrathTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_WRATH_10, SPELL_WRATH_25, SPELL_WRATH_10, SPELL_WRATH_25));
					WrathTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else WrathTimer -= diff;

			if (HealingTouchTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, RAID_MODE(SPELL_HEALING_TOUCH_10, SPELL_HEALING_TOUCH_25, SPELL_HEALING_TOUCH_10, SPELL_HEALING_TOUCH_25));
					HealingTouchTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else HealingTouchTimer -= diff;

			if (RegrowthTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, RAID_MODE(SPELL_REGROWTH_10, SPELL_REGROWTH_25, SPELL_REGROWTH_10, SPELL_REGROWTH_25));
					RegrowthTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
				}
			}
			else RegrowthTimer -= diff;

			if (RejuvenationTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, RAID_MODE(SPELL_REJUVENATION_10, SPELL_REJUVENATION_25, SPELL_REJUVENATION_10, SPELL_REJUVENATION_25));
					RejuvenationTimer = urand(10*IN_MILLISECONDS, 11*IN_MILLISECONDS);
				}
			}
			else RejuvenationTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_40))
				DoCast(me, RAID_MODE(SPELL_HEALING_TOUCH_10, SPELL_HEALING_TOUCH_25, SPELL_HEALING_TOUCH_10, SPELL_HEALING_TOUCH_25));

			if (HealthBelowPct(HEALTH_PERCENT_50))
				DoCast(me, RAID_MODE(SPELL_REJUVENATION_10, SPELL_REJUVENATION_25, SPELL_REJUVENATION_10, SPELL_REJUVENATION_25));

			if (HealthBelowPct(HEALTH_PERCENT_60))
				DoCast(me, RAID_MODE(SPELL_REGROWTH_10, SPELL_REGROWTH_25, SPELL_REGROWTH_10, SPELL_REGROWTH_25));

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skybreaker_hierophantAI(creature);
    }
};

class npc_skybreaker_marksman : public CreatureScript
{
public:
    npc_skybreaker_marksman() : CreatureScript("npc_skybreaker_marksman") {}

    struct npc_skybreaker_marksmanAI : public QuantumBasicAI
    {
        npc_skybreaker_marksmanAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShootTimer;
		uint32 ArcaneShotTimer;
		uint32 ExplosiveShotTimer;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;
			ArcaneShotTimer = 1*IN_MILLISECONDS;
			ExplosiveShotTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_RANGED_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_BOW);
        }

		void EnterToBattle(Unit* /*who*/){}

		void AttackStart(Unit* who)
        {
			AttackStartNoMove(who);
        }

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShootTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, RAID_MODE(SPELL_SHOOT_10, SPELL_SHOOT_25, SPELL_SHOOT_10, SPELL_SHOOT_25));
						ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
					}
				}
			}
			else ShootTimer -= diff;

			if (ExplosiveShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, RAID_MODE(SPELL_EXPLOSIVE_SHOT_10, SPELL_EXPLOSIVE_SHOT_25, SPELL_EXPLOSIVE_SHOT_10, SPELL_EXPLOSIVE_SHOT_25));
						ExplosiveShotTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
					}
				}
			}
			else ExplosiveShotTimer -= diff;

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

				if (ArcaneShotTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
						{
							DoCast(target, RAID_MODE(SPELL_ARCANE_SHOT_10, SPELL_ARCANE_SHOT_25, SPELL_ARCANE_SHOT_10, SPELL_ARCANE_SHOT_25));
							ArcaneShotTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
						}
					}
				}
				else ArcaneShotTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skybreaker_marksmanAI(creature);
    }
};

class npc_skybreaker_dreadblade : public CreatureScript
{
public:
    npc_skybreaker_dreadblade() : CreatureScript("npc_skybreaker_dreadblade") {}

    struct npc_skybreaker_dreadbladeAI : public QuantumBasicAI
    {
        npc_skybreaker_dreadbladeAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 IcyTouchTimer;
		uint32 PlagueStrikeTimer;

        void Reset()
        {
			IcyTouchTimer = 0.5*IN_MILLISECONDS;
			PlagueStrikeTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_THREAT_AURA);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustReachedHome()
		{
			Reset();
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (IcyTouchTimer <= diff)
			{
				DoCastVictim(SPELL_ICY_TOUCH);
				IcyTouchTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else IcyTouchTimer -= diff;

			if (PlagueStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_PLAGUE_STRIKE);
				PlagueStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else PlagueStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skybreaker_dreadbladeAI(creature);
    }
};

class npc_skybreaker_protector : public CreatureScript
{
public:
    npc_skybreaker_protector() : CreatureScript("npc_skybreaker_protector") {}

    struct npc_skybreaker_protectorAI : public QuantumBasicAI
    {
        npc_skybreaker_protectorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 SpellReflectTimer;
		uint32 DevastateTimer;
		uint32 ShieldSlamTimer;
		uint32 ThunderclapTimer;

        void Reset()
        {
			SpellReflectTimer = 0.5*IN_MILLISECONDS;
			DevastateTimer = 2*IN_MILLISECONDS;
			ShieldSlamTimer = 3*IN_MILLISECONDS;
			ThunderclapTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_THREAT_AURA);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

		void JustReachedHome()
		{
			Reset();
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SpellReflectTimer <= diff)
			{
				DoCast(me, SPELL_SPELL_REFLECT);
				SpellReflectTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else SpellReflectTimer -= diff;

			if (DevastateTimer <= diff)
			{
				DoCastVictim(SPELL_DEVASTATE);
				DevastateTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else DevastateTimer -= diff;

			if (ShieldSlamTimer <= diff)
			{
				DoCastVictim(RAID_MODE(SPELL_SHIELD_SLAM_10, SPELL_SHIELD_SLAM_25, SPELL_SHIELD_SLAM_10, SPELL_SHIELD_SLAM_25));
				ShieldSlamTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else ShieldSlamTimer -= diff;

			if (ThunderclapTimer <= diff)
			{
				DoCastAOE(RAID_MODE(SPELL_THUNDERCLAP_10, SPELL_THUNDERCLAP_25, SPELL_THUNDERCLAP_10, SPELL_THUNDERCLAP_25));
				ThunderclapTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else ThunderclapTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skybreaker_protectorAI(creature);
    }
};

class npc_skybreaker_vindicator : public CreatureScript
{
public:
    npc_skybreaker_vindicator() : CreatureScript("npc_skybreaker_vindicator") {}

    struct npc_skybreaker_vindicatorAI : public QuantumBasicAI
    {
        npc_skybreaker_vindicatorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();

			StartBattle = false;

			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		bool StartBattle;

		EventMap events;
		InstanceScript* instance;

		uint32 AvengersShieldTimer;
		uint32 ConsecrationTimer;
		uint32 HolyWrathTimer;

        void Reset()
        {
			AvengersShieldTimer = 2*IN_MILLISECONDS;
			ConsecrationTimer = 3*IN_MILLISECONDS;
			HolyWrathTimer = 4*IN_MILLISECONDS;

			events.Reset();

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_THREAT_AURA);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (!instance)
				return;

			if (instance->GetData(DATA_TEAM_IN_INSTANCE) != ALLIANCE)
				return;

			if (who->GetTypeId() != TYPE_ID_PLAYER)
				return;

			if (!StartBattle && me->IsWithinDistInMap(who, 35.0f))
			{
				if (Creature* protector = me->FindCreatureWithDistance(NPC_GB_SKYBREAKER_PROTECTOR, 25.0f))
				{
					if (protector->IsAlive())
					{
						events.ScheduleEvent(EVENT_FIRST_SQUAD_ASSISTED_1, 1*IN_MILLISECONDS);
						events.ScheduleEvent(EVENT_FIRST_SQUAD_ASSISTED_2, 15*IN_MILLISECONDS);
						events.ScheduleEvent(EVENT_FIRST_SQUAD_ASSISTED_3, 17*IN_MILLISECONDS);
						events.ScheduleEvent(EVENT_FIRST_SQUAD_ASSISTED_4, 24*IN_MILLISECONDS);
						events.ScheduleEvent(EVENT_FIRST_SQUAD_ASSISTED_5, 26*IN_MILLISECONDS);

						StartBattle = true;
					}
				}
			}
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->CallForHelp(50.0f);
			DoCastVictim(RAID_MODE(SPELL_AVENGERS_SHIELD_10, SPELL_AVENGERS_SHIELD_25, SPELL_AVENGERS_SHIELD_10, SPELL_AVENGERS_SHIELD_25));
		}

		void JustReachedHome()
		{
			Reset();
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
			if (!instance)
				return;

			events.Update(diff);

			// Out of Combat
			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				    case EVENT_FIRST_SQUAD_ASSISTED_1:
						DoSendQuantumText(SAY_FIRST_SQUAD_RESCUED_ALLIANCE_0, me);
						break;
					case EVENT_FIRST_SQUAD_ASSISTED_2:
						if (Creature* sorcerer = me->FindCreatureWithDistance(NPC_GB_SKYBREAKER_SORCERER, 25.0f))
						{
							if (sorcerer->IsAlive())
							{
								DoSendQuantumText(SAY_FIRST_SQUAD_RESCUED_ALLIANCE_1, sorcerer);
								DoSendQuantumText(SAY_SUMMON_SKYBREAKER_STANDARD, sorcerer);
								sorcerer->CastSpell(sorcerer, SPELL_SUMMON_SKYBREAKER_ST, true);
							}
						}
						break;
					case EVENT_FIRST_SQUAD_ASSISTED_3:
						if (Creature* protector = me->FindCreatureWithDistance(NPC_GB_SKYBREAKER_PROTECTOR, 25.0f))
						{
							// Summon Spire Frostwyrm
							me->SummonCreature(NPC_SPIRE_FROSTWYRM, FrostWyrmPosA);

							if (protector->IsAlive())
								DoSendQuantumText(SAY_SECOND_SQUAD_RESCUED_ALLIANCE_0, protector);
						}
						break;
					case EVENT_FIRST_SQUAD_ASSISTED_4:
						if (Creature* protector = me->FindCreatureWithDistance(NPC_GB_SKYBREAKER_PROTECTOR, 25.0f))
						{
							if (protector->IsAlive())
								DoSendQuantumText(SAY_SECOND_SQUAD_RESCUED_ALLIANCE_1, protector);
						}
						break;
					case EVENT_FIRST_SQUAD_ASSISTED_5:
						if (Creature* protector = me->FindCreatureWithDistance(NPC_GB_SKYBREAKER_PROTECTOR, 25.0f))
						{
							if (protector->IsAlive())
							{
								DoSendQuantumText(SAY_SECOND_SQUAD_RESCUED_ALLIANCE_2, protector);
								DoSendQuantumText(SAY_FROSTWYRM_SUMMON, protector);
							}
						}

						if (Creature* protector = me->FindCreatureWithDistance(NPC_GB_SKYBREAKER_PROTECTOR, 25.0f))
						{
							protector->CastSpell(protector, SPELL_AGGRO_FOR_ME);
							protector->CallForHelp(65.0f);
						}
						break;
				}
			}

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (AvengersShieldTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_AVENGERS_SHIELD_10, SPELL_AVENGERS_SHIELD_25, SPELL_AVENGERS_SHIELD_10, SPELL_AVENGERS_SHIELD_25));
					AvengersShieldTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else AvengersShieldTimer -= diff;

			if (ConsecrationTimer <= diff)
			{
				DoCastAOE(RAID_MODE(SPELL_CONSECRATION_10, SPELL_CONSECRATION_25, SPELL_CONSECRATION_10, SPELL_CONSECRATION_25));
				ConsecrationTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else ConsecrationTimer -= diff;

			if (HolyWrathTimer <= diff)
			{
				DoCastAOE(RAID_MODE(SPELL_HOLY_WRATH_10, SPELL_HOLY_WRATH_25, SPELL_HOLY_WRATH_10, SPELL_HOLY_WRATH_25));
				HolyWrathTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else HolyWrathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skybreaker_vindicatorAI(creature);
    }
};

class npc_skybreaker_luminary : public CreatureScript
{
public:
    npc_skybreaker_luminary() : CreatureScript("npc_skybreaker_luminary") {}

    struct npc_skybreaker_luminaryAI : public QuantumBasicAI
    {
        npc_skybreaker_luminaryAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 LightningBoltTimer;
		uint32 HealingWaveTimer;
		uint32 EarthShieldTimer;
		uint32 ChainHealTimer;

        void Reset()
        {
			LightningBoltTimer = 0.5*IN_MILLISECONDS;
			HealingWaveTimer = 2*IN_MILLISECONDS;
			ChainHealTimer = 4*IN_MILLISECONDS;
			EarthShieldTimer = 6*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Earth Shield Buff Check in Combat
			if (!me->HasAura(SPELL_EARTH_SHIELD))
				DoCast(me, SPELL_EARTH_SHIELD, true);

			if (LightningBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_LIGHTNING_BOLT_10, SPELL_LIGHTNING_BOLT_25, SPELL_LIGHTNING_BOLT_10, SPELL_LIGHTNING_BOLT_25));
					LightningBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else LightningBoltTimer -= diff;

			if (HealingWaveTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, RAID_MODE(SPELL_HEALING_WAVE_10, SPELL_HEALING_WAVE_25, SPELL_HEALING_WAVE_10, SPELL_HEALING_WAVE_25));
					HealingWaveTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else HealingWaveTimer -= diff;

			if (ChainHealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, RAID_MODE(SPELL_CHAIN_HEAL_10, SPELL_CHAIN_HEAL_25, SPELL_CHAIN_HEAL_10, SPELL_CHAIN_HEAL_25));
					ChainHealTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else ChainHealTimer -= diff;

			if (EarthShieldTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, SPELL_EARTH_SHIELD);
					EarthShieldTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
				}
			}
			else EarthShieldTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_60))
				DoCast(me, RAID_MODE(SPELL_HEALING_WAVE_10, SPELL_HEALING_WAVE_25, SPELL_HEALING_WAVE_10, SPELL_HEALING_WAVE_25));

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skybreaker_luminaryAI(creature);
    }
};

class npc_skybreaker_assassin : public CreatureScript
{
public:
    npc_skybreaker_assassin() : CreatureScript("npc_skybreaker_assassin") {}

    struct npc_skybreaker_assassinAI : public QuantumBasicAI
    {
        npc_skybreaker_assassinAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FanOfKnivesTimer;
		uint32 SinisterStrikeTimer;

        void Reset()
        {
			FanOfKnivesTimer = 2*IN_MILLISECONDS;
			SinisterStrikeTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, SPELL_THREAT_AURA, true);
			DoCast(me, SPELL_DUAL_WIELD, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);

			me->SetPowerType(POWER_ENERGY);
			me->SetMaxPower(POWER_ENERGY, POWER_QUANTITY_MAX);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_SHADOWSTEP);
		}

		void JustReachedHome()
		{
			Reset();
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FanOfKnivesTimer <= diff)
			{
				DoCastAOE(RAID_MODE(SPELL_FAN_OF_KNIVES_10, SPELL_FAN_OF_KNIVES_25, SPELL_FAN_OF_KNIVES_10, SPELL_FAN_OF_KNIVES_25));
				FanOfKnivesTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else FanOfKnivesTimer -= diff;

			if (SinisterStrikeTimer <= diff)
			{
				DoCastVictim(RAID_MODE(SPELL_SINISTER_STRIKE_10, SPELL_SINISTER_STRIKE_25, SPELL_SINISTER_STRIKE_10, SPELL_SINISTER_STRIKE_25));
				SinisterStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else SinisterStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skybreaker_assassinAI(creature);
    }
};

class npc_skybreaker_sorcerer : public CreatureScript
{
public:
    npc_skybreaker_sorcerer() : CreatureScript("npc_skybreaker_sorcerer") {}

    struct npc_skybreaker_sorcererAI : public QuantumBasicAI
    {
        npc_skybreaker_sorcererAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FrostfireBoltTimer;
		uint32 BlinkTimer;

		SummonList Summons;

        void Reset()
        {
			FrostfireBoltTimer = 0.5*IN_MILLISECONDS;
			BlinkTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);

			Summons.DespawnAll();
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SUMMON_SKYBREAKER_ST, true);
			DoSendQuantumText(SAY_SUMMON_SKYBREAKER_STANDARD, me);
		}

		void JustDied(Unit* killer)
		{
			Summons.DespawnAll();
			
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_SKYBREAKER_BATTLE_STANDARD)
				Summons.Summon(summon);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostfireBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					me->SetFacingToObject(target);

					DoCast(target, RAID_MODE(SPELL_FROSTFIRE_BOLT_10, SPELL_FROSTFIRE_BOLT_25, SPELL_FROSTFIRE_BOLT_10, SPELL_FROSTFIRE_BOLT_25));
					FrostfireBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostfireBoltTimer -= diff;

			if (BlinkTimer <= diff)
			{
				DoCast(SelectTarget(TARGET_FARTHEST, 0, 10.0f), SPELL_BLINK);
				BlinkTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else BlinkTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skybreaker_sorcererAI(creature);
    }
};

class npc_skybreaker_battle_standard : public CreatureScript
{
public:
    npc_skybreaker_battle_standard() : CreatureScript("npc_skybreaker_battle_standard") {}

    struct npc_skybreaker_battle_standardAI : public QuantumBasicAI
    {
        npc_skybreaker_battle_standardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        void Reset()
        {
			DoCastAOE(SPELL_SKYBREAKER_AURA);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 /*diff*/)
		{
			me->DespawnAfterAction(1*MINUTE*IN_MILLISECONDS);
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skybreaker_battle_standardAI(creature);
    }
};

class npc_korkron_templar : public CreatureScript
{
public:
    npc_korkron_templar() : CreatureScript("npc_korkron_templar") {}

    struct npc_korkron_templarAI : public QuantumBasicAI
    {
        npc_korkron_templarAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 SmiteTimer;
		uint32 GreaterHealTimer;
		uint32 PainSupressionTimer;

        void Reset()
        {
			SmiteTimer = 0.5*IN_MILLISECONDS;
			GreaterHealTimer = 3*IN_MILLISECONDS;
			PainSupressionTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SmiteTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_SMITE_10, SPELL_SMITE_25, SPELL_SMITE_10, SPELL_SMITE_25));
					SmiteTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else SmiteTimer -= diff;

			if (GreaterHealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, RAID_MODE(SPELL_GREATER_HEAL_10, SPELL_GREATER_HEAL_25, SPELL_GREATER_HEAL_10, SPELL_GREATER_HEAL_25));
					GreaterHealTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else GreaterHealTimer -= diff;

			if (PainSupressionTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, SPELL_PAIN_SUPRESSION);
					PainSupressionTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
				}
			}
			else PainSupressionTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_60))
				DoCast(me, RAID_MODE(SPELL_GREATER_HEAL_10, SPELL_GREATER_HEAL_25, SPELL_GREATER_HEAL_10, SPELL_GREATER_HEAL_25), true);

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_PAIN_SUPRESSION, 0))
					DoCast(me, SPELL_PAIN_SUPRESSION);
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_korkron_templarAI(creature);
    }
};

class npc_korkron_necrolyte : public CreatureScript
{
public:
    npc_korkron_necrolyte() : CreatureScript("npc_korkron_necrolyte") {}

    struct npc_korkron_necrolyteAI : public QuantumBasicAI
    {
        npc_korkron_necrolyteAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CurseOfDoomTimer;
		uint32 IncinerateTimer;
		uint32 ShadowBoltTimer;

        void Reset()
        {
			CurseOfDoomTimer = 0.5*IN_MILLISECONDS;
			IncinerateTimer = 1*IN_MILLISECONDS;
			ShadowBoltTimer = 2500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CurseOfDoomTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_CURSE_OF_DOOM_10, SPELL_CURSE_OF_DOOM_25, SPELL_CURSE_OF_DOOM_10, SPELL_CURSE_OF_DOOM_25));
					CurseOfDoomTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CurseOfDoomTimer -= diff;

			if (IncinerateTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_INCINERATE_10, SPELL_INCINERATE_25, SPELL_INCINERATE_10, SPELL_INCINERATE_25));
					IncinerateTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else IncinerateTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_SHADOW_BOLT_10, SPELL_SHADOW_BOLT_25, SPELL_SHADOW_BOLT_10, SPELL_SHADOW_BOLT_25));
					ShadowBoltTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_korkron_necrolyteAI(creature);
    }
};

class npc_korkron_primalist : public CreatureScript
{
public:
    npc_korkron_primalist() : CreatureScript("npc_korkron_primalist") {}

    struct npc_korkron_primalistAI : public QuantumBasicAI
    {
        npc_korkron_primalistAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();

			StartBattle = false;

			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		bool StartBattle;

		EventMap events;
		InstanceScript* instance;

		uint32 WrathTimer;
		uint32 HealingTouchTimer;
		uint32 RegrowthTimer;
		uint32 RejuvenationTimer;

        void Reset()
        {
			WrathTimer = 0.5*IN_MILLISECONDS;
			HealingTouchTimer = 2*IN_MILLISECONDS;
			RegrowthTimer = 3500;
			RejuvenationTimer = 5*IN_MILLISECONDS;

			events.Reset();

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (!instance)
				return;

			if (instance->GetData(DATA_TEAM_IN_INSTANCE) != HORDE)
				return;

			if (who->GetTypeId() != TYPE_ID_PLAYER)
				return;

			if (!StartBattle && me->IsWithinDistInMap(who, 35.0f))
			{
				if (Creature* defender = me->FindCreatureWithDistance(NPC_GB_KORKRON_DEFENDER, 25.0f))
				{
					if (defender->IsAlive())
					{
						events.ScheduleEvent(EVENT_FIRST_SQUAD_ASSISTED_1, 1*IN_MILLISECONDS);
						events.ScheduleEvent(EVENT_FIRST_SQUAD_ASSISTED_2, 18*IN_MILLISECONDS);
						events.ScheduleEvent(EVENT_FIRST_SQUAD_ASSISTED_3, 20*IN_MILLISECONDS);
						events.ScheduleEvent(EVENT_FIRST_SQUAD_ASSISTED_4, 22*IN_MILLISECONDS);

						StartBattle = true;
					}
				}
			}
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->CallForHelp(50.0f);
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
			if (!instance)
				return;

			events.Update(diff);

			// Out of Combat
			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				    case EVENT_FIRST_SQUAD_ASSISTED_1:
						DoSendQuantumText(SAY_FIRST_SQUAD_RESCUED_HORDE_0, me);
						break;
					case EVENT_FIRST_SQUAD_ASSISTED_2:
						if (Creature* invoker = me->FindCreatureWithDistance(NPC_GB_KORKRON_INVOKER, 25.0f))
						{
							if (invoker->IsAlive())
							{
								DoSendQuantumText(SAY_FIRST_SQUAD_RESCUED_HORDE_1, invoker);
								DoSendQuantumText(SAY_SUMMON_KORKRON_STANDARD, invoker);
								invoker->CastSpell(invoker, SPELL_SUMMON_KORKRON_ST, true);
							}
						}
						break;
					case EVENT_FIRST_SQUAD_ASSISTED_3:
						if (Creature* defender = me->FindCreatureWithDistance(NPC_GB_KORKRON_DEFENDER, 25.0f))
						{
							// Summon Spire Frostwyrm
							me->SummonCreature(NPC_SPIRE_FROSTWYRM, FrostWyrmPosH);

							if (defender->IsAlive())
								DoSendQuantumText(SAY_SECOND_SQUAD_RESCUED_HORDE_0, defender);
						}
						break;
					case EVENT_FIRST_SQUAD_ASSISTED_4:
						if (Creature* defender = me->FindCreatureWithDistance(NPC_GB_KORKRON_DEFENDER, 25.0f))
						{
							if (defender->IsAlive())
								DoSendQuantumText(SAY_SECOND_SQUAD_RESCUED_HORDE_1, defender);
						}

						if (Creature* defender = me->FindCreatureWithDistance(NPC_GB_KORKRON_DEFENDER, 25.0f))
						{
							if (defender->IsAlive())
								DoSendQuantumText(SAY_FROSTWYRM_SUMMON, defender);
						}

						if (Creature* defender = me->FindCreatureWithDistance(NPC_GB_KORKRON_DEFENDER, 25.0f))
						{
							defender->CastSpell(defender, SPELL_AGGRO_FOR_ME);
							defender->CallForHelp(65.0f);
						}
						break;
				}
			}

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WrathTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_WRATH_10, SPELL_WRATH_25, SPELL_WRATH_10, SPELL_WRATH_25));
					WrathTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else WrathTimer -= diff;

			if (HealingTouchTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, RAID_MODE(SPELL_HEALING_TOUCH_10, SPELL_HEALING_TOUCH_25, SPELL_HEALING_TOUCH_10, SPELL_HEALING_TOUCH_25));
					HealingTouchTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else HealingTouchTimer -= diff;

			if (RegrowthTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, RAID_MODE(SPELL_REGROWTH_10, SPELL_REGROWTH_25, SPELL_REGROWTH_10, SPELL_REGROWTH_25));
					RegrowthTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
				}
			}
			else RegrowthTimer -= diff;

			if (RejuvenationTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, RAID_MODE(SPELL_REJUVENATION_10, SPELL_REJUVENATION_25, SPELL_REJUVENATION_10, SPELL_REJUVENATION_25));
					RejuvenationTimer = urand(10*IN_MILLISECONDS, 11*IN_MILLISECONDS);
				}
			}
			else RejuvenationTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_40))
				DoCast(me, RAID_MODE(SPELL_HEALING_TOUCH_10, SPELL_HEALING_TOUCH_25, SPELL_HEALING_TOUCH_10, SPELL_HEALING_TOUCH_25));

			if (HealthBelowPct(HEALTH_PERCENT_50))
				DoCast(me, RAID_MODE(SPELL_REJUVENATION_10, SPELL_REJUVENATION_25, SPELL_REJUVENATION_10, SPELL_REJUVENATION_25));

			if (HealthBelowPct(HEALTH_PERCENT_60))
				DoCast(me, RAID_MODE(SPELL_REGROWTH_10, SPELL_REGROWTH_25, SPELL_REGROWTH_10, SPELL_REGROWTH_25));

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_korkron_primalistAI(creature);
    }
};

class npc_korkron_sniper : public CreatureScript
{
public:
    npc_korkron_sniper() : CreatureScript("npc_korkron_sniper") {}

    struct npc_korkron_sniperAI : public QuantumBasicAI
    {
        npc_korkron_sniperAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShootTimer;
		uint32 ArcaneShotTimer;
		uint32 ExplosiveShotTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_RANGED_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_RIFLE);

			ShootTimer = 0.5*IN_MILLISECONDS;
			ArcaneShotTimer = 1*IN_MILLISECONDS;
			ExplosiveShotTimer = 3*IN_MILLISECONDS;
        }

		void EnterToBattle(Unit* /*who*/){}

		void AttackStart(Unit* who)
        {
			AttackStartNoMove(who);
        }

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShootTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 20)
					{
						DoCast(target, RAID_MODE(SPELL_SHOOT_10, SPELL_SHOOT_25, SPELL_SHOOT_10, SPELL_SHOOT_25));
						ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
					}
				}
			}
			else ShootTimer -= diff;

			if (ExplosiveShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 20)
					{
						DoCast(target, RAID_MODE(SPELL_EXPLOSIVE_SHOT_10, SPELL_EXPLOSIVE_SHOT_25, SPELL_EXPLOSIVE_SHOT_10, SPELL_EXPLOSIVE_SHOT_25));
						ExplosiveShotTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
					}
				}
			}
			else ExplosiveShotTimer -= diff;

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

				if (ArcaneShotTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 20)
						{
							DoCast(target, RAID_MODE(SPELL_ARCANE_SHOT_10, SPELL_ARCANE_SHOT_25, SPELL_ARCANE_SHOT_10, SPELL_ARCANE_SHOT_25));
							ArcaneShotTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
						}
					}
				}
				else ArcaneShotTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_korkron_sniperAI(creature);
    }
};

class npc_korkron_reaver : public CreatureScript
{
public:
    npc_korkron_reaver() : CreatureScript("npc_korkron_reaver") {}

    struct npc_korkron_reaverAI : public QuantumBasicAI
    {
        npc_korkron_reaverAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 IcyTouchTimer;
		uint32 PlagueStrikeTimer;

        void Reset()
        {
			IcyTouchTimer = 0.5*IN_MILLISECONDS;
			PlagueStrikeTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_THREAT_AURA);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustReachedHome()
		{
			Reset();
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (IcyTouchTimer <= diff)
			{
				DoCastVictim(SPELL_ICY_TOUCH);
				IcyTouchTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else IcyTouchTimer -= diff;

			if (PlagueStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_PLAGUE_STRIKE);
				PlagueStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else PlagueStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_korkron_reaverAI(creature);
    }
};

class npc_korkron_defender : public CreatureScript
{
public:
    npc_korkron_defender() : CreatureScript("npc_korkron_defender") {}

    struct npc_korkron_defenderAI : public QuantumBasicAI
    {
        npc_korkron_defenderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 SpellReflectTimer;
		uint32 DevastateTimer;
		uint32 ShieldSlamTimer;
		uint32 ThunderclapTimer;

        void Reset()
        {
			SpellReflectTimer = 0.5*IN_MILLISECONDS;
			DevastateTimer = 2*IN_MILLISECONDS;
			ShieldSlamTimer = 3*IN_MILLISECONDS;
			ThunderclapTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_THREAT_AURA);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

		void JustReachedHome()
		{
			Reset();
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
			if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SpellReflectTimer <= diff)
			{
				DoCast(me, SPELL_SPELL_REFLECT);
				SpellReflectTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else SpellReflectTimer -= diff;

			if (DevastateTimer <= diff)
			{
				DoCastVictim(SPELL_DEVASTATE);
				DevastateTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else DevastateTimer -= diff;

			if (ShieldSlamTimer <= diff)
			{
				DoCastVictim(RAID_MODE(SPELL_SHIELD_SLAM_10, SPELL_SHIELD_SLAM_25, SPELL_SHIELD_SLAM_10, SPELL_SHIELD_SLAM_25));
				ShieldSlamTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else ShieldSlamTimer -= diff;

			if (ThunderclapTimer <= diff)
			{
				DoCastAOE(RAID_MODE(SPELL_THUNDERCLAP_10, SPELL_THUNDERCLAP_25, SPELL_THUNDERCLAP_10, SPELL_THUNDERCLAP_25));
				ThunderclapTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else ThunderclapTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_korkron_defenderAI(creature);
    }
};

class npc_korkron_vanquisher : public CreatureScript
{
public:
    npc_korkron_vanquisher() : CreatureScript("npc_korkron_vanquisher") {}

    struct npc_korkron_vanquisherAI : public QuantumBasicAI
    {
        npc_korkron_vanquisherAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 AvengersShieldTimer;
		uint32 ConsecrationTimer;
		uint32 HolyWrathTimer;

        void Reset()
        {
			AvengersShieldTimer = 2*IN_MILLISECONDS;
			ConsecrationTimer = 3*IN_MILLISECONDS;
			HolyWrathTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_THREAT_AURA);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(RAID_MODE(SPELL_AVENGERS_SHIELD_10, SPELL_AVENGERS_SHIELD_25, SPELL_AVENGERS_SHIELD_10, SPELL_AVENGERS_SHIELD_25));
		}

		void JustReachedHome()
		{
			Reset();
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
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
					DoCast(target, RAID_MODE(SPELL_AVENGERS_SHIELD_10, SPELL_AVENGERS_SHIELD_25, SPELL_AVENGERS_SHIELD_10, SPELL_AVENGERS_SHIELD_25));
					AvengersShieldTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else AvengersShieldTimer -= diff;

			if (ConsecrationTimer <= diff)
			{
				DoCastAOE(RAID_MODE(SPELL_CONSECRATION_10, SPELL_CONSECRATION_25, SPELL_CONSECRATION_10, SPELL_CONSECRATION_25));
				ConsecrationTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else ConsecrationTimer -= diff;

			if (HolyWrathTimer <= diff)
			{
				DoCastAOE(RAID_MODE(SPELL_HOLY_WRATH_10, SPELL_HOLY_WRATH_25, SPELL_HOLY_WRATH_10, SPELL_HOLY_WRATH_25));
				HolyWrathTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else HolyWrathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_korkron_vanquisherAI(creature);
    }
};

class npc_korkron_oracle : public CreatureScript
{
public:
    npc_korkron_oracle() : CreatureScript("npc_korkron_oracle") {}

    struct npc_korkron_oracleAI : public QuantumBasicAI
    {
        npc_korkron_oracleAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 LightningBoltTimer;
		uint32 HealingWaveTimer;
		uint32 EarthShieldTimer;
		uint32 ChainHealTimer;

        void Reset()
        {
			LightningBoltTimer = 0.5*IN_MILLISECONDS;
			HealingWaveTimer = 2*IN_MILLISECONDS;
			ChainHealTimer = 4*IN_MILLISECONDS;
			EarthShieldTimer = 6*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Earth Shield Buff Check in Combat
			if (!me->HasAura(SPELL_EARTH_SHIELD))
				DoCast(me, SPELL_EARTH_SHIELD, true);

			if (LightningBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_LIGHTNING_BOLT_10, SPELL_LIGHTNING_BOLT_25, SPELL_LIGHTNING_BOLT_10, SPELL_LIGHTNING_BOLT_25));
					LightningBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else LightningBoltTimer -= diff;

			if (HealingWaveTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, RAID_MODE(SPELL_HEALING_WAVE_10, SPELL_HEALING_WAVE_25, SPELL_HEALING_WAVE_10, SPELL_HEALING_WAVE_25));
					HealingWaveTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else HealingWaveTimer -= diff;

			if (ChainHealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, RAID_MODE(SPELL_CHAIN_HEAL_10, SPELL_CHAIN_HEAL_25, SPELL_CHAIN_HEAL_10, SPELL_CHAIN_HEAL_25));
					ChainHealTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else ChainHealTimer -= diff;

			if (EarthShieldTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, SPELL_EARTH_SHIELD);
					EarthShieldTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
				}
			}
			else EarthShieldTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_60))
				DoCast(me, RAID_MODE(SPELL_HEALING_WAVE_10, SPELL_HEALING_WAVE_25, SPELL_HEALING_WAVE_10, SPELL_HEALING_WAVE_25));

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_korkron_oracleAI(creature);
    }
};

class npc_korkron_stalker : public CreatureScript
{
public:
    npc_korkron_stalker() : CreatureScript("npc_korkron_stalker") {}

    struct npc_korkron_stalkerAI : public QuantumBasicAI
    {
        npc_korkron_stalkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FanOfKnivesTimer;
		uint32 SinisterStrikeTimer;

        void Reset()
        {
			FanOfKnivesTimer = 2*IN_MILLISECONDS;
			SinisterStrikeTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, SPELL_THREAT_AURA, true);
			DoCast(me, SPELL_DUAL_WIELD, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);

			me->SetPowerType(POWER_ENERGY);
			me->SetMaxPower(POWER_ENERGY, POWER_QUANTITY_MAX);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_SHADOWSTEP);
		}

		void JustReachedHome()
		{
			Reset();
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FanOfKnivesTimer <= diff)
			{
				DoCastAOE(RAID_MODE(SPELL_FAN_OF_KNIVES_10, SPELL_FAN_OF_KNIVES_25, SPELL_FAN_OF_KNIVES_10, SPELL_FAN_OF_KNIVES_25));
				FanOfKnivesTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else FanOfKnivesTimer -= diff;

			if (SinisterStrikeTimer <= diff)
			{
				DoCastVictim(RAID_MODE(SPELL_SINISTER_STRIKE_10, SPELL_SINISTER_STRIKE_25, SPELL_SINISTER_STRIKE_10, SPELL_SINISTER_STRIKE_25));
				SinisterStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else SinisterStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_korkron_stalkerAI(creature);
    }
};

class npc_korkron_invoker : public CreatureScript
{
public:
    npc_korkron_invoker() : CreatureScript("npc_korkron_invoker") {}

    struct npc_korkron_invokerAI : public QuantumBasicAI
    {
        npc_korkron_invokerAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FrostfireBoltTimer;
		uint32 BlinkTimer;

		SummonList Summons;

        void Reset()
        {
			FrostfireBoltTimer = 0.5*IN_MILLISECONDS;
			BlinkTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);

			Summons.DespawnAll();
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SUMMON_KORKRON_ST, true);
			DoSendQuantumText(SAY_SUMMON_KORKRON_STANDARD, me);
		}

		void JustDied(Unit* killer)
		{
			Summons.DespawnAll();

			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_KORKRON_BATTLE_STANDARD)
				Summons.Summon(summon);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostfireBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					me->SetFacingToObject(target);

					DoCast(target, RAID_MODE(SPELL_FROSTFIRE_BOLT_10, SPELL_FROSTFIRE_BOLT_25, SPELL_FROSTFIRE_BOLT_10, SPELL_FROSTFIRE_BOLT_25));
					FrostfireBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostfireBoltTimer -= diff;

			if (BlinkTimer <= diff)
			{
				DoCast(SelectTarget(TARGET_FARTHEST, 0, 10.0f), SPELL_BLINK);
				BlinkTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else BlinkTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_korkron_invokerAI(creature);
    }
};

class npc_korkron_battle_standard : public CreatureScript
{
public:
	npc_korkron_battle_standard() : CreatureScript("npc_korkron_battle_standard") {}

    struct npc_korkron_battle_standardAI : public QuantumBasicAI
    {
        npc_korkron_battle_standardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        void Reset()
        {
			DoCastAOE(SPELL_KORKRON_AURA);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 /*diff*/)
		{
			me->DespawnAfterAction(1*MINUTE*IN_MILLISECONDS);
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_korkron_battle_standardAI(creature);
    }
};

class npc_valkyr_herald : public CreatureScript
{
public:
	npc_valkyr_herald() : CreatureScript("npc_valkyr_herald") { }

	struct npc_valkyr_heraldAI : public QuantumBasicAI
	{
		npc_valkyr_heraldAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 SeveredEssenceTimer;

		void Reset()
		{
			SeveredEssenceTimer = 2*IN_MILLISECONDS;

			me->SetCanFly(true);
			me->HandleEmoteCommand(EMOTE_ONESHOT_FLY_SIT_GROUND_UP);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetCanFly(false);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
			me->HandleEmoteCommand(EMOTE_ONESHOT_FLY_SIT_GROUND_DOWN);
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SeveredEssenceTimer <= diff)
			{
				DoCastAOE(RAID_MODE(SPELL_SEVERED_ESSENCE_10, SPELL_SEVERED_ESSENCE_25, SPELL_SEVERED_ESSENCE_10, SPELL_SEVERED_ESSENCE_25));
				SeveredEssenceTimer = urand(18*IN_MILLISECONDS, 20*IN_MILLISECONDS);
			}
			else SeveredEssenceTimer -= diff;

			DoMeleeAttackIfReady();   
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_valkyr_heraldAI(creature);
	}
};

class npc_blighted_abomination : public CreatureScript
{
public:
    npc_blighted_abomination() : CreatureScript("npc_blighted_abomination") { }

    struct npc_blighted_abominationAI : public QuantumBasicAI
    {
        npc_blighted_abominationAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ScourgeHookTimer;
		uint32 CleaveTimer;
        uint32 PlagueCloudTimer;

        void Reset()
        {
			ScourgeHookTimer = 0.5*IN_MILLISECONDS;
			PlagueCloudTimer = 1.5*IN_MILLISECONDS;
			CleaveTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ScourgeHookTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_SCOURGE_HOOK);
					ScourgeHookTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
            }
            else ScourgeHookTimer -= diff;

			if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
            }
            else CleaveTimer -= diff;

            if (PlagueCloudTimer <= diff)
            {
                DoCastAOE(SPELL_PLAGUE_CLOUD);
                PlagueCloudTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
            }
            else PlagueCloudTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_blighted_abominationAI(creature);
    }
};

class npc_vengeful_fleshreaper : public CreatureScript
{
public:
    npc_vengeful_fleshreaper() : CreatureScript("npc_vengeful_fleshreaper") { }

    struct npc_vengeful_fleshreaperAI : public QuantumBasicAI
    {
        npc_vengeful_fleshreaperAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 JumpTimer;

        void Reset()
        {
			JumpTimer = 0.2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (JumpTimer <= diff)
            {
				DoCastVictim(SPELL_LEAPING_FACE_MAUL);
				JumpTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
            }
            else JumpTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vengeful_fleshreaperAI(creature);
    }
};

class npc_plague_scientist : public CreatureScript
{
public:
    npc_plague_scientist() : CreatureScript("npc_plague_scientist") { }

    struct npc_plague_scientistAI : public QuantumBasicAI
    {
        npc_plague_scientistAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 PlagueBlastTimer;
        uint32 PlagueStreamTimer;
        uint32 CombobulatingSprayTimer;

        void Reset()
        {
			PlagueBlastTimer = 0.5*IN_MILLISECONDS;
			PlagueStreamTimer = 1*IN_MILLISECONDS;
            CombobulatingSprayTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, RAND(SPELL_PLAGUETHROWER_BACKPACK1, SPELL_PLAGUETHROWER_BACKPACK2, SPELL_PLAGUETHROWER_BACKPACK3), true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_USE_STANDING);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PlagueBlastTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_PLAGUE_BLAST, true);
					PlagueBlastTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
            }
            else PlagueBlastTimer -= diff;

            if (PlagueStreamTimer <= diff)
            {
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, SPELL_PLAGUE_STREAM);
					PlagueStreamTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
            }
            else PlagueStreamTimer -= diff;

            if (CombobulatingSprayTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_COMBOBULATING_SPRAY);
					CombobulatingSprayTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
            }
            else CombobulatingSprayTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_plague_scientistAI(creature);
    }
};

class npc_pustulating_horror : public CreatureScript
{
public:
    npc_pustulating_horror() : CreatureScript("npc_pustulating_horror") { }

    struct npc_pustulating_horrorAI : public QuantumBasicAI
    {
        npc_pustulating_horrorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 BublingPusTimer;
        uint32 BlightBombTimer;

        void Reset()
        {
			BublingPusTimer = 0.5*IN_MILLISECONDS;
			BlightBombTimer = 12*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (BublingPusTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_BUBBLING_PUS_10, SPELL_BUBBLING_PUS_25, SPELL_BUBBLING_PUS_10, SPELL_BUBBLING_PUS_25));
					BublingPusTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else BublingPusTimer -= diff;

			if (BlightBombTimer <= diff)
            {
                DoCastAOE(SPELL_BLIGHT_BOMB);
                BlightBombTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
            }
            else BlightBombTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_pustulating_horrorAI(creature);
    }
};

class npc_decaying_colossus : public CreatureScript
{
public:
	npc_decaying_colossus() : CreatureScript("npc_decaying_colossus") { }

	struct npc_decaying_colossusAI : public QuantumBasicAI
	{
		npc_decaying_colossusAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 MassiveStompTimer;

		void Reset()
		{
			MassiveStompTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MassiveStompTimer <= diff)
			{
				DoCastVictim(RAID_MODE(SPELL_MASSIVE_STOMP_10, SPELL_MASSIVE_STOMP_25, SPELL_MASSIVE_STOMP_10, SPELL_MASSIVE_STOMP_25));
				MassiveStompTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else MassiveStompTimer -= diff;

			DoMeleeAttackIfReady();   
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_decaying_colossusAI(creature);
	}
};

class npc_darkfallen_archmage : public CreatureScript
{
public:
    npc_darkfallen_archmage() : CreatureScript("npc_darkfallen_archmage") { }

    struct npc_darkfallen_archmageAI : public QuantumBasicAI
    {
        npc_darkfallen_archmageAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 AmplifyMagicTimer;
		uint32 FireballTimer;
		uint32 BlastWaveTimer;
        uint32 PolymorphTimer;
		uint32 VampiricFiendTimer;

        void Reset()
        {
			AmplifyMagicTimer = 0.5*IN_MILLISECONDS;
			FireballTimer = 1*IN_MILLISECONDS;
			BlastWaveTimer = 3*IN_MILLISECONDS;
            PolymorphTimer = 4*IN_MILLISECONDS;
			VampiricFiendTimer = 6*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SummonCreature(NPC_VAMPIRIC_FIEND, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (AmplifyMagicTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_AMPLIFY_MAGIC_10, SPELL_AMPLIFY_MAGIC_25, SPELL_AMPLIFY_MAGIC_10, SPELL_AMPLIFY_MAGIC_25));
					AmplifyMagicTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
            }
            else AmplifyMagicTimer -= diff;

			if (FireballTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_FIREBALL_10, SPELL_FIREBALL_25, SPELL_FIREBALL_10, SPELL_FIREBALL_25));
					FireballTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
            }
            else FireballTimer -= diff;

            if (BlastWaveTimer <= diff)
            {
				DoCastAOE(RAID_MODE(SPELL_BLAST_WAVE_10, SPELL_BLAST_WAVE_25, SPELL_BLAST_WAVE_10, SPELL_BLAST_WAVE_25));
                BlastWaveTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
            }
            else BlastWaveTimer -= diff;

            if (PolymorphTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_POLYMORPH_SPIDER);
					PolymorphTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
				}
            }
            else PolymorphTimer -= diff;

			if (VampiricFiendTimer <= diff)
            {
				me->SummonCreature(NPC_VAMPIRIC_FIEND, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
                VampiricFiendTimer = urand(11*IN_MILLISECONDS, 12*IN_MILLISECONDS);
            }
            else VampiricFiendTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_darkfallen_archmageAI(creature);
    }
};

class npc_darkfallen_blood_knight : public CreatureScript
{
public:
    npc_darkfallen_blood_knight() : CreatureScript("npc_darkfallen_blood_knight") { }

    struct npc_darkfallen_blood_knightAI : public QuantumBasicAI
    {
        npc_darkfallen_blood_knightAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 VampiricAuraTimer;
        uint32 UnholyStrikeTimer;

        void Reset()
        {
			VampiricAuraTimer = 0.5*IN_MILLISECONDS;
			UnholyStrikeTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (VampiricAuraTimer <= diff)
            {
                DoCast(me, SPELL_VAMPIRIC_AURA);
                VampiricAuraTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
            }
            else VampiricAuraTimer -= diff;

            if (UnholyStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_UNHOLY_STRIKE);
                UnholyStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
            }
            else UnholyStrikeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_darkfallen_blood_knightAI(creature);
    }
};

class npc_darkfallen_noble : public CreatureScript
{
public:
    npc_darkfallen_noble() : CreatureScript("npc_darkfallen_noble") { }

    struct npc_darkfallen_nobleAI : public QuantumBasicAI
    {
        npc_darkfallen_nobleAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 NoobleShadowBoltTimer;
        uint32 ChainsOfShadowTimer;
		uint32 EmpoweredVampiricFiendTimer;

        void Reset()
        {
			NoobleShadowBoltTimer = 0.5*IN_MILLISECONDS;
            ChainsOfShadowTimer = 3*IN_MILLISECONDS;
			EmpoweredVampiricFiendTimer = 6*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SummonCreature(NPC_EMPOWERED_VAMPIRIC_FIEND, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (NoobleShadowBoltTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_NOOBLE_SHADOW_BOLT_10, SPELL_NOOBLE_SHADOW_BOLT_25, SPELL_NOOBLE_SHADOW_BOLT_10, SPELL_NOOBLE_SHADOW_BOLT_25));
					NoobleShadowBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
            }
            else NoobleShadowBoltTimer -= diff;

            if (ChainsOfShadowTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_CHAINS_OF_SHADOW);
					ChainsOfShadowTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
            }
            else ChainsOfShadowTimer -= diff;

			if (EmpoweredVampiricFiendTimer <= diff)
            {
				me->SummonCreature(NPC_EMPOWERED_VAMPIRIC_FIEND, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
                EmpoweredVampiricFiendTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
            }
            else EmpoweredVampiricFiendTimer -= diff;

            DoMeleeAttackIfReady();   
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_darkfallen_nobleAI(creature);
    }
};

class npc_darkfallen_advisor : public CreatureScript
{
public:
    npc_darkfallen_advisor() : CreatureScript("npc_darkfallen_advisor") { }

    struct npc_darkfallen_advisorAI : public QuantumBasicAI
    {
        npc_darkfallen_advisorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShroudOfProtectionTimer;
		uint32 ShroudOfSpellWardingTimer;
        uint32 LichSlapTimer;

        void Reset()
        {
			ShroudOfProtectionTimer = 0.5*IN_MILLISECONDS;
			ShroudOfSpellWardingTimer = 1.5*IN_MILLISECONDS;
			LichSlapTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_SHADOW_CHANNELING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_SHADOW_CHANNELING);
		}

		void JustReachedHome()
		{
			Reset();
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShroudOfProtectionTimer <= diff)
            {
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, SPELL_SHROUD_OF_PROTECTION);
					ShroudOfProtectionTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
				}
            }
            else ShroudOfProtectionTimer -= diff;

			if (ShroudOfSpellWardingTimer <= diff)
            {
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, SPELL_SHROUD_OF_SPELL_WARDING);
					ShroudOfSpellWardingTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
            }
            else ShroudOfSpellWardingTimer -= diff;

            if (LichSlapTimer <= diff)
            {
				DoCastVictim(RAID_MODE(SPELL_LICH_SLAP_10, SPELL_LICH_SLAP_25, SPELL_LICH_SLAP_10, SPELL_LICH_SLAP_25));
                LichSlapTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
            }
            else LichSlapTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_60))
				DoCast(me, SPELL_SHROUD_OF_PROTECTION);

			if (HealthBelowPct(HEALTH_PERCENT_50))
				DoCast(me, SPELL_SHROUD_OF_SPELL_WARDING);

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_darkfallen_advisorAI(creature);
    }
};

class npc_darkfallen_commander : public CreatureScript
{
public:
    npc_darkfallen_commander() : CreatureScript("npc_darkfallen_commander") { }

    struct npc_darkfallen_commanderAI : public QuantumBasicAI
    {
        npc_darkfallen_commanderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 BattleShoutTimer;
        uint32 VampireRushTimer;

        void Reset()
        {
			BattleShoutTimer = 0.5*IN_MILLISECONDS;
            VampireRushTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (BattleShoutTimer <= diff)
            {
                DoCast(me, SPELL_BATTLE_SHOUT);
                BattleShoutTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
            }
            else BattleShoutTimer -= diff;

            if (VampireRushTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_VAMPIRE_RUSH_10, SPELL_VAMPIRE_RUSH_25, SPELL_VAMPIRE_RUSH_10, SPELL_VAMPIRE_RUSH_25));
					VampireRushTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
            }
            else VampireRushTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_darkfallen_commanderAI(creature);
    }
};

class npc_darkfallen_lieutenant : public CreatureScript
{
public:
    npc_darkfallen_lieutenant() : CreatureScript("npc_darkfallen_lieutenant") { }

    struct npc_darkfallen_lieutenantAI : public QuantumBasicAI
    {
        npc_darkfallen_lieutenantAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 VampiricCurseTimer;
        uint32 RendFleshTimer;

        void Reset()
        {
			VampiricCurseTimer = 0.5*IN_MILLISECONDS;
			RendFleshTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (VampiricCurseTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_VAMPIRIC_CURSE);
					VampiricCurseTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
            }
            else VampiricCurseTimer -= diff;

            if (RendFleshTimer <= diff)
            {
				DoCastVictim(RAID_MODE(SPELL_REND_FLESH_10, SPELL_REND_FLESH_25, SPELL_REND_FLESH_10, SPELL_REND_FLESH_25));
                RendFleshTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
            }
            else RendFleshTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_darkfallen_lieutenantAI(creature);
    }
};

class npc_darkfallen_tactician : public CreatureScript
{
public:
    npc_darkfallen_tactician() : CreatureScript("npc_darkfallen_tactician") { }

    struct npc_darkfallen_tacticianAI : public QuantumBasicAI
    {
        npc_darkfallen_tacticianAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 BloodSapTimer;
        uint32 ShadowstepTimer;
		uint32 UnholyStrikeTimer;

        void Reset()
        {
			ShadowstepTimer = 0.2*IN_MILLISECONDS;
			BloodSapTimer = 1*IN_MILLISECONDS;
            UnholyStrikeTimer = 2*IN_MILLISECONDS;

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_DUAL_WIELD);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShadowstepTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_SHADOWSTEP);
					ShadowstepTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
            }
            else ShadowstepTimer -= diff;

			if (BloodSapTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_BLOOD_SAP);
					BloodSapTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
            }
            else BloodSapTimer -= diff;

            if (UnholyStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_UNHOLY_STRIKE);
                UnholyStrikeTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
            }
            else UnholyStrikeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_darkfallen_tacticianAI(creature);
    }
};

class npc_vampiric_fiend : public CreatureScript
{
public:
	npc_vampiric_fiend() : CreatureScript("npc_vampiric_fiend") { }

	struct npc_vampiric_fiendAI : public QuantumBasicAI
	{
		npc_vampiric_fiendAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 LeechingRotTimer;

		void Reset()
		{
			LeechingRotTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetReactState(REACT_AGGRESSIVE);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_DISEASE_CLOUD);
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (LeechingRotTimer <= diff)
			{
				DoCastAOE(SPELL_LEECHING_ROT, true);
				me->DespawnAfterAction(3*IN_MILLISECONDS);
				LeechingRotTimer = 10*IN_MILLISECONDS;
			}
			else LeechingRotTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_vampiric_fiendAI(creature);
	}
};

class npc_empowered_vampiric_fiend : public CreatureScript
{
public:
	npc_empowered_vampiric_fiend() : CreatureScript("npc_empowered_vampiric_fiend") { }

	struct npc_empowered_vampiric_fiendAI : public QuantumBasicAI
	{
		npc_empowered_vampiric_fiendAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 LeechingRotTimer;

		void Reset()
		{
			LeechingRotTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetReactState(REACT_AGGRESSIVE);
		}

		void EnterToBattle(Unit* /*target*/)
		{
			DoCast(me, SPELL_DISEASE_CLOUD);
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Empowered Blood Buff Check in Combat
			if (!me->HasAura(SPELL_EMPOWERED_BLOOD))
				DoCast(me, SPELL_EMPOWERED_BLOOD);

			if (LeechingRotTimer <= diff)
			{
				DoCastAOE(SPELL_LEECHING_EVF_ROT, true);
				me->DespawnAfterAction(3*IN_MILLISECONDS);
				LeechingRotTimer = 10*IN_MILLISECONDS;
			}
			else LeechingRotTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_empowered_vampiric_fiendAI(creature);
	}
};

class npc_ymirjar_deathbringer : public CreatureScript
{
public:
	npc_ymirjar_deathbringer() : CreatureScript("npc_ymirjar_deathbringer") { }

	struct npc_ymirjar_deathbringerAI : public QuantumBasicAI
	{
		npc_ymirjar_deathbringerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShadowBoltTimer;
		uint32 DeathEmbraceTimer;
		uint32 BanishTimer;

		void Reset()
		{
			ShadowBoltTimer = 0.5*IN_MILLISECONDS;
			DeathEmbraceTimer = 2*IN_MILLISECONDS;
			BanishTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastAOE(SPELL_SUMMON_YMIRJAR, true);
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
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
					DoCast(target, RAID_MODE(SPELL_SHADOW_BOLT_YMIRJAR_10, SPELL_SHADOW_BOLT_YMIRJAR_25, SPELL_SHADOW_BOLT_YMIRJAR_10, SPELL_SHADOW_BOLT_YMIRJAR_25));
					ShadowBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			if (DeathEmbraceTimer <= diff)
			{
				DoCast(me, RAID_MODE(SPELL_DEATHS_EMBRACE_10, SPELL_DEATHS_EMBRACE_25, SPELL_DEATHS_EMBRACE_10, SPELL_DEATHS_EMBRACE_25));
				DeathEmbraceTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else DeathEmbraceTimer -= diff;

			if (BanishTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BANISH);
					BanishTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else BanishTimer -= diff;

			DoMeleeAttackIfReady();   
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ymirjar_deathbringerAI(creature);
	}
};

class npc_ymirjar_frostbinder : public CreatureScript
{
public:
	npc_ymirjar_frostbinder() : CreatureScript("npc_ymirjar_frostbinder") { }

	struct npc_ymirjar_frostbinderAI : public QuantumBasicAI
	{
		npc_ymirjar_frostbinderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FrozenOrbTimer;

		void Reset()
		{
			FrozenOrbTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastAOE(SPELL_ARCTIC_CHILL);
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrozenOrbTimer <= diff)
			{
				DoCastAOE(SPELL_FROZEN_ORB);
				FrozenOrbTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else FrozenOrbTimer -= diff;

			DoMeleeAttackIfReady();   
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ymirjar_frostbinderAI(creature);
	}
};

class npc_ymirjar_battle_maiden : public CreatureScript
{
public:
	npc_ymirjar_battle_maiden() : CreatureScript("npc_ymirjar_battle_maiden") { }

	struct npc_ymirjar_battle_maidenAI : public QuantumBasicAI
	{
		npc_ymirjar_battle_maidenAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 BarbaricStrikeTimer;
		uint32 AdrenalineRushTimer;

		void Reset()
		{
			BarbaricStrikeTimer = 2*IN_MILLISECONDS;
			AdrenalineRushTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_DUAL_WIELD);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BarbaricStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_BARBARIC_STRIKE);
				BarbaricStrikeTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
			}
			else BarbaricStrikeTimer -= diff;

			if (AdrenalineRushTimer <= diff)
			{
				DoCast(me, SPELL_ADRENALINE_RUSH, true);
				AdrenalineRushTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else AdrenalineRushTimer -= diff;

			DoMeleeAttackIfReady();   
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ymirjar_battle_maidenAI(creature);
	}
};

class npc_ymirjar_huntress : public CreatureScript
{
public:
	npc_ymirjar_huntress() : CreatureScript("npc_ymirjar_huntress") { }

	struct npc_ymirjar_huntressAI : public QuantumBasicAI
	{
		npc_ymirjar_huntressAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShootTimer;
		uint32 VolleyTimer;
		uint32 RapidShotTimer;
		uint32 TrapTimer;

		void Reset()
		{
			ShootTimer = 0.5*IN_MILLISECONDS;
			VolleyTimer = 1*IN_MILLISECONDS;
			RapidShotTimer = 3*IN_MILLISECONDS;
			TrapTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_RANGED_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_BOW);
		}

		void EnterToBattle(Unit* /*target*/)
		{
			if (Is25ManRaid())
				DoCastAOE(SPELL_SUMMON_WARHAWK);
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (VolleyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target,SPELL_VOLLEY);
					VolleyTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else VolleyTimer -= diff;

			if (RapidShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_RAPID_SHOT);
					RapidShotTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else RapidShotTimer -= diff;

			if (TrapTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ICE_TRAP);
					TrapTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
				}
			}
			else TrapTimer -= diff;

			if (me->IsAttackReady())
			{
				if (me->IsWithinMeleeRange(me->GetVictim()))
					DoMeleeAttackIfReady();

				else if (ShootTimer <= diff)
				{
					DoCastVictim(SPELL_SHOOT);
					ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
				}
				else ShootTimer -= diff;
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ymirjar_huntressAI(creature);
	}
};

class npc_ymirjar_warlord : public CreatureScript
{
public:
	npc_ymirjar_warlord() : CreatureScript("npc_ymirjar_warlord") { }

	struct npc_ymirjar_warlordAI : public QuantumBasicAI
	{
		npc_ymirjar_warlordAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 WhirlwindTimer;

		void Reset()
		{
			WhirlwindTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_WARLORDS_PRESENCE);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
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
				WhirlwindTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else WhirlwindTimer -= diff;

			DoMeleeAttackIfReady();   
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ymirjar_warlordAI(creature);
	}
};

class npc_nerubar_webweaver : public CreatureScript
{
public:
	npc_nerubar_webweaver() : CreatureScript("npc_nerubar_webweaver") { }

	struct npc_nerubar_webweaverAI : public QuantumBasicAI
	{
		npc_nerubar_webweaverAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CryptCryptScarabsTimer;
		uint32 WebTimer;

		void Reset()
		{
			CryptCryptScarabsTimer = 0.5*IN_MILLISECONDS;
			WebTimer = 1.5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CryptCryptScarabsTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_NW_CRYPT_SCARABS, true);
					CryptCryptScarabsTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CryptCryptScarabsTimer -= diff;

			if (WebTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_WEB);
					WebTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else WebTimer -= diff;

			DoMeleeAttackIfReady();   
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_nerubar_webweaverAI(creature);
	}
};

class npc_nerubar_champion : public CreatureScript
{
public:
	npc_nerubar_champion() : CreatureScript("npc_nerubar_champion") { }

	struct npc_nerubar_championAI : public QuantumBasicAI
	{
		npc_nerubar_championAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 RushTimer;

		void Reset()
		{
			RushTimer = 0.5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RushTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_RUSH);
					RushTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else RushTimer -= diff;

			DoMeleeAttackIfReady();   
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_nerubar_championAI(creature);
	}
};

class npc_frostwarden_sorceress : public CreatureScript
{
public:
	npc_frostwarden_sorceress() : CreatureScript("npc_frostwarden_sorceress") { }

	struct npc_frostwarden_sorceressAI : public QuantumBasicAI
	{
		npc_frostwarden_sorceressAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FrostboltTimer;
		uint32 FrostNovaTimer;
		uint32 IceTombTimer;

		void Reset()
		{
			FrostboltTimer = 0.5*IN_MILLISECONDS;
			FrostNovaTimer = 1.5*IN_MILLISECONDS;
			IceTombTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

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
					DoCast(target, SPELL_FROSTBOLT, true);
					FrostboltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(SPELL_FROST_NOVA);
				FrostNovaTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else FrostNovaTimer -= diff;

			if (IceTombTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ICE_TOMB);
					IceTombTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else IceTombTimer -= diff;

			DoMeleeAttackIfReady();   
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_frostwarden_sorceressAI(creature);
	}
};

class npc_frostwarden_warrior : public CreatureScript
{
public:
	npc_frostwarden_warrior() : CreatureScript("npc_frostwarden_warrior") { }

	struct npc_frostwarden_warriorAI : public QuantumBasicAI
	{
		npc_frostwarden_warriorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FrostbladeTimer;
		uint32 GlacialStrikeTimer;

		void Reset()
		{
			FrostbladeTimer = 1*IN_MILLISECONDS;
			GlacialStrikeTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostbladeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FROSTBLADE);
					FrostbladeTimer = 4*IN_MILLISECONDS;
				}
			}
			else FrostbladeTimer -= diff;

			if (GlacialStrikeTimer <= diff)
			{
				DoCastVictim(RAID_MODE(SPELL_GLACIAL_STRIKE_10, SPELL_GLACIAL_STRIKE_25, SPELL_GLACIAL_STRIKE_10, SPELL_GLACIAL_STRIKE_25));
				GlacialStrikeTimer = 6*IN_MILLISECONDS;
			}
			else GlacialStrikeTimer -= diff;

			DoMeleeAttackIfReady();   
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_frostwarden_warriorAI(creature);
	}
};

class npc_flesh_eating_insect : public CreatureScript
{
public:
	npc_flesh_eating_insect() : CreatureScript("npc_flesh_eating_insect") {}

    struct npc_flesh_eating_insectAI : public QuantumBasicAI
    {
        npc_flesh_eating_insectAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		void Reset()
        {
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (Player* player = who->ToPlayer())
			{
				if (player->IsWithinDist(me, 250.0f))
					DoZoneInCombat();
			}
		}

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_flesh_eating_insectAI(creature);
    }
};

class npc_blood_beast : public CreatureScript
{
public:
	npc_blood_beast() : CreatureScript("npc_blood_beast") {}

    struct npc_blood_beastAI : public QuantumBasicAI
    {
        npc_blood_beastAI(Creature* creature) : QuantumBasicAI(creature){}

		void Reset()
        {
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_blood_beastAI(creature);
    }
};

class npc_web_wrap_icc : public CreatureScript
{
public:
	npc_web_wrap_icc() : CreatureScript("npc_web_wrap_icc") {}

    struct npc_web_wrap_iccAI : public QuantumBasicAI
    {
        npc_web_wrap_iccAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

		void Reset(){}

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* /*killer*/)
		{
			if (Player* player = me->FindPlayerWithDistance(1.0f))
			{
				player->RemoveAurasDueToSpell(SPELL_WEB_WRAP_STUN);
				player->RestoreDisplayId();
				player->GetMotionMaster()->Clear();
			}
		}

        void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_web_wrap_iccAI(creature);
    }
};

class npc_severed_essence_shaman : public CreatureScript
{
public:
	npc_severed_essence_shaman() : CreatureScript("npc_severed_essence_shaman") { }

	struct npc_severed_essence_shamanAI : public QuantumBasicAI
	{
		npc_severed_essence_shamanAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 LightningBoltTimer;
		uint32 ReplenishingRainsTimer;
		uint32 ThunderstormTimer;

		void Reset()
		{
			LightningBoltTimer = 0.5*IN_MILLISECONDS;
			ThunderstormTimer = 3*IN_MILLISECONDS;
			ReplenishingRainsTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, SPELL_SPAWN_ESSENCE, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (LightningBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SE_LIGHTNING_BOLT);
					LightningBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else LightningBoltTimer -= diff;

			if (ThunderstormTimer <= diff)
			{
				DoCast(me, SPELL_SE_THUNDERSTORM);
				ThunderstormTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else ThunderstormTimer -= diff;

			if (ReplenishingRainsTimer <= diff)
			{
				DoCast(me, SPELL_SE_REPLENISHING_RAINS);
				ReplenishingRainsTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else ReplenishingRainsTimer -= diff;

			DoMeleeAttackIfReady();   
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_severed_essence_shamanAI(creature);
	}
};

class npc_severed_essence_death_knight : public CreatureScript
{
public:
	npc_severed_essence_death_knight() : CreatureScript("npc_severed_essence_death_knight") { }

	struct npc_severed_essence_death_knightAI : public QuantumBasicAI
	{
		npc_severed_essence_death_knightAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 PlagueStrikeTimer;
		uint32 NecroticStrikeTimer;

		void Reset()
		{
			PlagueStrikeTimer = 1*IN_MILLISECONDS;
			NecroticStrikeTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, SPELL_SPAWN_ESSENCE, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PlagueStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_SE_PLAGUE_STRIKE);
				PlagueStrikeTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else PlagueStrikeTimer -= diff;

			if (NecroticStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_SE_NECROTIC_STRIKE);
				NecroticStrikeTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else NecroticStrikeTimer -= diff;

			DoMeleeAttackIfReady();   
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_severed_essence_death_knightAI(creature);
	}
};

class npc_severed_essence_warrior : public CreatureScript
{
public:
	npc_severed_essence_warrior() : CreatureScript("npc_severed_essence_warrior") { }

	struct npc_severed_essence_warriorAI : public QuantumBasicAI
	{
		npc_severed_essence_warriorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 HeroicLeapTimer;
		uint32 BloodthirstTimer;

		void Reset()
		{
			HeroicLeapTimer = 0.5*IN_MILLISECONDS;
			BloodthirstTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, SPELL_SPAWN_ESSENCE, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HeroicLeapTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SE_HEROIC_LEAP);
					HeroicLeapTimer = 4*IN_MILLISECONDS;
				}
			}
			else HeroicLeapTimer -= diff;

			if (BloodthirstTimer <= diff)
			{
				DoCastVictim(SPELL_SE_BLOODTHIRST);
				BloodthirstTimer = 6*IN_MILLISECONDS;
			}
			else BloodthirstTimer -= diff;

			DoMeleeAttackIfReady();   
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_severed_essence_warriorAI(creature);
	}
};

class npc_severed_essence_paladin : public CreatureScript
{
public:
	npc_severed_essence_paladin() : CreatureScript("npc_severed_essence_paladin") { }

	struct npc_severed_essence_paladinAI : public QuantumBasicAI
	{
		npc_severed_essence_paladinAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_DISPEL, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_STEAL_BENEFICIAL_BUFF, true);
		}

		uint32 AvengingWrathTimer;
		uint32 CrusaderStrikeTimer;
		uint32 FlashOfLightTimer;

		void Reset()
		{
			AvengingWrathTimer = 0.5*IN_MILLISECONDS;
			CrusaderStrikeTimer = 2*IN_MILLISECONDS;
			FlashOfLightTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, SPELL_SPAWN_ESSENCE, true);
			DoCast(me, SPELL_SE_RADIANCE_AURA, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (AvengingWrathTimer <= diff)
			{
				DoCast(me, SPELL_SE_AVENGING_WRATH);
				AvengingWrathTimer = 21*IN_MILLISECONDS;
			}
			else AvengingWrathTimer -= diff;

			if (CrusaderStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_SE_CRUSADER_STRIKE);
				CrusaderStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else CrusaderStrikeTimer -= diff;

			if (FlashOfLightTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, SPELL_SE_FLASH_OF_LIGHT);
					FlashOfLightTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else FlashOfLightTimer -= diff;

			DoMeleeAttackIfReady();   
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_severed_essence_paladinAI(creature);
	}
};

class npc_severed_essence_hunter : public CreatureScript
{
public:
	npc_severed_essence_hunter() : CreatureScript("npc_severed_essence_hunter") { }

	struct npc_severed_essence_hunterAI : public QuantumBasicAI
	{
		npc_severed_essence_hunterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShootTimer;
		uint32 ExplosiveShotTimer;

		void Reset()
		{
			ShootTimer = 0.5*IN_MILLISECONDS;
			ExplosiveShotTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, SPELL_SPAWN_ESSENCE, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShootTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SE_SHOOT, true);
					ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
				}
			}
			else ShootTimer -= diff;

			if (ExplosiveShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SE_EXPLOSIVE_SHOT, true);
					ExplosiveShotTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else ExplosiveShotTimer -= diff;

			DoMeleeAttackIfReady();   
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_severed_essence_hunterAI(creature);
	}
};

class npc_severed_essence_druid : public CreatureScript
{
public:
	npc_severed_essence_druid() : CreatureScript("npc_severed_essence_druid") { }

	struct npc_severed_essence_druidAI : public QuantumBasicAI
	{
		npc_severed_essence_druidAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 MangleTimer;
		uint32 RipTimer;

		void Reset()
		{
			MangleTimer = 1*IN_MILLISECONDS;
			RipTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, SPELL_SPAWN_ESSENCE, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			me->RestoreDisplayId();

			me->SetPowerType(POWER_ENERGY);
			me->SetMaxPower(POWER_ENERGY, POWER_QUANTITY_MAX);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			if (me->GetVictim()->ToPlayer()->GetTeamId() == TEAM_ALLIANCE)
				me->SetDisplayId(RAND(892, 29405, 29406, 29407, 29408));

			if (me->GetVictim()->ToPlayer()->GetTeamId() == TEAM_HORDE)
				me->SetDisplayId(RAND(8571, 29409, 29410, 29411, 29412));
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MangleTimer <= diff)
			{
				DoCastVictim(SPELL_SE_MANGLE, true);
				MangleTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else MangleTimer -= diff;

			if (RipTimer <= diff)
			{
				DoCastVictim(SPELL_SE_RIP, true);
				RipTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else RipTimer -= diff;

			DoMeleeAttackIfReady();   
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_severed_essence_druidAI(creature);
	}
};

class npc_severed_essence_rogue : public CreatureScript
{
public:
	npc_severed_essence_rogue() : CreatureScript("npc_severed_essence_rogue") { }

	struct npc_severed_essence_rogueAI : public QuantumBasicAI
	{
		npc_severed_essence_rogueAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 FocusedAttacksTimer;
		uint32 EviscerateTimer;

		void Reset()
		{
			FocusedAttacksTimer = 0.5*IN_MILLISECONDS;
			EviscerateTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, SPELL_SPAWN_ESSENCE, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FocusedAttacksTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SE_FOCUSED_ATTACKS);
					FocusedAttacksTimer = urand(10*IN_MILLISECONDS, 12*IN_MILLISECONDS);
				}
			}
			else FocusedAttacksTimer -= diff;

			if (EviscerateTimer <= diff)
			{
				DoCastVictim(SPELL_SE_EVISCERATE);
				EviscerateTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else EviscerateTimer -= diff;

			DoMeleeAttackIfReady();   
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_severed_essence_rogueAI(creature);
	}
};

class npc_severed_essence_mage : public CreatureScript
{
public:
	npc_severed_essence_mage() : CreatureScript("npc_severed_essence_mage") { }

	struct npc_severed_essence_mageAI : public QuantumBasicAI
	{
		npc_severed_essence_mageAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 BoltTimer;
		uint32 FrostNovaTimer;

		void Reset()
		{
			BoltTimer = 0.5*IN_MILLISECONDS;
			FrostNovaTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, SPELL_SPAWN_ESSENCE, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAND(SPELL_SE_FROSTBOLT, SPELL_SE_FIREBALL));
					BoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else BoltTimer -= diff;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(SPELL_SE_FROST_NOVA);
				FrostNovaTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else FrostNovaTimer -= diff;

			DoMeleeAttackIfReady();   
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_severed_essence_mageAI(creature);
	}
};

class npc_severed_essence_warlock : public CreatureScript
{
public:
	npc_severed_essence_warlock() : CreatureScript("npc_severed_essence_warlock") { }

	struct npc_severed_essence_warlockAI : public QuantumBasicAI
	{
		npc_severed_essence_warlockAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CorruptionTimer;
		uint32 ShadowBoltTimer;
		uint32 RainOfChaosTimer;

		void Reset()
		{
			CorruptionTimer = 0.2*IN_MILLISECONDS;
			ShadowBoltTimer = 2*IN_MILLISECONDS;
			RainOfChaosTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, SPELL_SPAWN_ESSENCE, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CorruptionTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SE_CORRUPTION);
					CorruptionTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CorruptionTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SE_SHADOW_BOLT);
					ShadowBoltTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			if (RainOfChaosTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SE_RAIN_OF_CHAOS);
					RainOfChaosTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
				}
			}
			else RainOfChaosTimer -= diff;

			DoMeleeAttackIfReady();   
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_severed_essence_warlockAI(creature);
	}
};

class npc_severed_essence_priest : public CreatureScript
{
public:
	npc_severed_essence_priest() : CreatureScript("npc_severed_essence_priest") { }

	struct npc_severed_essence_priestAI : public QuantumBasicAI
	{
		npc_severed_essence_priestAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 SmiteTimer;
		uint32 RenewTimer;
		uint32 GreaterHealTimer;

		void Reset()
		{
			SmiteTimer = 0.5*IN_MILLISECONDS;
			RenewTimer = 2*IN_MILLISECONDS;
			GreaterHealTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			DoCast(me, SPELL_SPAWN_ESSENCE, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SmiteTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SE_SMITE);
					SmiteTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
				}
			}
			else SmiteTimer -= diff;

			if (RenewTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, SPELL_SE_RENEW);
					RenewTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else RenewTimer -= diff;

			if (GreaterHealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, SPELL_SE_GREATER_HEAL);
					GreaterHealTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else GreaterHealTimer -= diff;

			DoMeleeAttackIfReady();   
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_severed_essence_priestAI(creature);
	}
};

class npc_plagued_zombie : public CreatureScript
{
public:
    npc_plagued_zombie() : CreatureScript("npc_plagued_zombie") { }

    struct npc_plagued_zombieAI : public QuantumBasicAI
    {
        npc_plagued_zombieAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        uint32 InfectedWoundTimer;
		uint32 VileGassTimer;

        void Reset()
        {
			InfectedWoundTimer = 1*IN_MILLISECONDS;
			VileGassTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->CastSpell(player, SPELL_SOUL_FEAST, true);

			me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (InfectedWoundTimer <= diff)
            {
				DoCastVictim(SPELL_INFECTED_WOUND);
				InfectedWoundTimer = 4*IN_MILLISECONDS;
            }
            else InfectedWoundTimer -= diff;

			if (VileGassTimer <= diff)
            {
				DoCast(me, SPELL_VILE_GASS);
				VileGassTimer = 6*IN_MILLISECONDS;
            }
            else VileGassTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_plagued_zombieAI(creature);
    }
};

void AddSC_icecrown_citadel_trash()
{
	new npc_deathbound_ward();
	new npc_servant_of_the_throne();
	new npc_ancient_skeletal_soldier();
	new npc_nerubar_broodkeeper();
	new npc_deathspeaker_servant();
	new npc_deathspeaker_disciple();
	new npc_deathspeaker_attendant();
	new npc_deathspeaker_zealot();
	new npc_deathspeaker_high_priest();
	new npc_spire_frostwyrm();
	new npc_frenzied_abomination();
	new npc_spire_gargoyle();
	new npc_spire_minion();
	new npc_skybreaker_vicar();
	new npc_skybreaker_summoner();
	new npc_skybreaker_hierophant();
	new npc_skybreaker_marksman();
	new npc_skybreaker_dreadblade();
	new npc_skybreaker_protector();
	new npc_skybreaker_vindicator();
	new npc_skybreaker_luminary();
	new npc_skybreaker_assassin();
	new npc_skybreaker_sorcerer();
	new npc_skybreaker_battle_standard();
	new npc_korkron_templar();
	new npc_korkron_necrolyte();
	new npc_korkron_primalist();
	new npc_korkron_sniper();
	new npc_korkron_reaver();
	new npc_korkron_defender();
	new npc_korkron_vanquisher();
	new npc_korkron_oracle();
	new npc_korkron_stalker();
	new npc_korkron_invoker();
	new npc_korkron_battle_standard();
	new npc_valkyr_herald();
	new npc_blighted_abomination();
	new npc_vengeful_fleshreaper();
	new npc_plague_scientist();
	new npc_pustulating_horror();
	new npc_decaying_colossus();
	new npc_darkfallen_archmage();
	new npc_darkfallen_blood_knight();
	new npc_darkfallen_noble();
	new npc_darkfallen_advisor();
	new npc_darkfallen_commander();
	new npc_darkfallen_lieutenant();
	new npc_darkfallen_tactician();
	new npc_vampiric_fiend();
	new npc_empowered_vampiric_fiend();
	new npc_ymirjar_deathbringer();
	new npc_ymirjar_frostbinder();
	new npc_ymirjar_battle_maiden();
	new npc_ymirjar_huntress();
	new npc_ymirjar_warlord();
	new npc_nerubar_webweaver();
	new npc_nerubar_champion();
	new npc_frostwarden_sorceress();
	new npc_frostwarden_warrior();
	new npc_flesh_eating_insect();
	new npc_blood_beast();
	new npc_web_wrap_icc();
	new npc_severed_essence_shaman();
	new npc_severed_essence_death_knight();
	new npc_severed_essence_warrior();
	new npc_severed_essence_paladin();
	new npc_severed_essence_hunter();
	new npc_severed_essence_druid();
	new npc_severed_essence_rogue();
	new npc_severed_essence_mage();
	new npc_severed_essence_warlock();
	new npc_severed_essence_priest();
	new npc_plagued_zombie();
}