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
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Player.h"
#include "GridNotifiers.h"
#include "trial_of_the_crusader.h"

enum Texts
{
    SAY_GARROSH_KILL_ALLIANCE_PLAYER_1 = -1649118,
	SAY_GARROSH_KILL_ALLIANCE_PLAYER_2 = -1649119,
	SAY_GARROSH_KILL_ALLIANCE_PLAYER_3 = -1649120,
	SAY_GARROSH_KILL_ALLIANCE_PLAYER_4 = -1649121,

    SAY_VARIAN_KILL_HORDE_PLAYER_1     = -1649122,
	SAY_VARIAN_KILL_HORDE_PLAYER_2     = -1649123,
	SAY_VARIAN_KILL_HORDE_PLAYER_3     = -1649124,
	SAY_VARIAN_KILL_HORDE_PLAYER_4     = -1649125,
};

enum AIs
{
    AI_MELEE    = 0,
    AI_RANGED   = 1,
    AI_HEALER   = 2,
    AI_PET      = 3
};

enum Spells
{
    // generic
    SPELL_ANTI_AOE      = 68595,
    SPELL_PVP_TRINKET   = 65547,

    // Druid healer
	SPELL_REJUVENATION_10N = 66065,
	SPELL_REJUVENATION_25N = 67971,
	SPELL_REJUVENATION_10H = 67272,
	SPELL_REJUVENATION_25H = 67973,

    SPELL_LIFEBLOOM_10N = 66093,
	SPELL_LIFEBLOOM_25N = 67957,
	SPELL_LIFEBLOOM_10H = 67958,
	SPELL_LIFEBLOOM_25H = 67959,

    SPELL_NOURISH_10N = 66066,
	SPELL_NOURISH_25N = 67965,
	SPELL_NOURISH_10H = 67966,
	SPELL_NOURISH_25H = 67967,

    SPELL_REGROWTH_10N = 66067,
	SPELL_REGROWTH_25N = 67968,
	SPELL_REGROWTH_10H = 67969,
	SPELL_REGROWTH_25H = 67970,

    SPELL_TRANQUILITY_10N = 66086,
	SPELL_TRANQUILITY_25N = 67974,
	SPELL_TRANQUILITY_10H = 67975,
	SPELL_TRANQUILITY_25H = 67976,

    SPELL_BARKSKIN        = 65860,
    SPELL_THORNS          = 66068,
    SPELL_NATURE_GRASP    = 66071,

    // Shaman healer
    SPELL_HEALING_WAVE_10N = 66055,
	SPELL_HEALING_WAVE_25N = 68115,
	SPELL_HEALING_WAVE_10H = 68116,
	SPELL_HEALING_WAVE_25H = 68117,

    SPELL_RIPTIDE_10N = 66053,
	SPELL_RIPTIDE_25N = 68118,
	SPELL_RIPTIDE_10H = 68119,
	SPELL_RIPTIDE_25H = 68120,

    SPELL_SPIRIT_CLEANSE_10N = 66056,
	SPELL_SPIRIT_CLEANSE_25N = 68627,
	SPELL_SPIRIT_CLEANSE_10H = 68628,
	SPELL_SPIRIT_CLEANSE_25H = 68629,

	SPELL_EARTH_SHOCK_10N = 65973,
	SPELL_EARTH_SHOCK_25N = 68100,
	SPELL_EARTH_SHOCK_10H = 68101,
	SPELL_EARTH_SHOCK_25H = 68102,

    SPELL_HEROISM           = 65983,
    SPELL_BLOODLUST         = 65980,
    SPELL_HEX               = 66054,
    SPELL_EARTH_SHIELD      = 66063,
    AURA_EXHAUSTION         = 57723,
    AURA_SATED              = 57724,

    // Paladin healer
	SPELL_FLASH_OF_LIGHT_10N = 66113,
	SPELL_FLASH_OF_LIGHT_25N = 68008,
	SPELL_FLASH_OF_LIGHT_10H = 68009,
	SPELL_FLASH_OF_LIGHT_25H = 68010,

    SPELL_HOLY_LIGHT_10N = 66112,
	SPELL_HOLY_LIGHT_25N = 68011,
	SPELL_HOLY_LIGHT_10H = 68012,
	SPELL_HOLY_LIGHT_25H = 68013,

    SPELL_HOLY_SHOCK_10N = 66114,
	SPELL_HOLY_SHOCK_25N = 68014,
	SPELL_HOLY_SHOCK_10H = 68015,
	SPELL_HOLY_SHOCK_25H = 68016,

	SPELL_CLEANSE_10N = 66116,
	SPELL_CLEANSE_25N = 68621,
	SPELL_CLEANSE_10H = 68622,
	SPELL_CLEANSE_25H = 68623,

	SPELL_HAND_OF_FREEDOM_10N = 66115,
	SPELL_HAND_OF_FREEDOM_25N = 68756,
	SPELL_HAND_OF_FREEDOM_10H = 68757,
	SPELL_HAND_OF_FREEDOM_25H = 68758,

    SPELL_DIVINE_SHIELD             = 66010,
    SPELL_HAND_OF_PROTECTION        = 66009,
    SPELL_HAMMER_OF_JUSTICE         = 66613,
    SPELL_FORBEARANCE               = 25771,

    // Priest healer
    SPELL_RENEW_10N = 66177,
	SPELL_RENEW_25N = 68035,
	SPELL_RENEW_10H = 68036,
	SPELL_RENEW_25H = 68037,

    SPELL_POWER_WORD_SHIELD_10N = 66099,
	SPELL_POWER_WORD_SHIELD_25N = 68032,
	SPELL_POWER_WORD_SHIELD_10H = 68033,
	SPELL_POWER_WORD_SHIELD_25H = 68034,

    SPELL_FLASH_HEAL_10N = 66104,
	SPELL_FLASH_HEAL_25N = 68023,
	SPELL_FLASH_HEAL_10H = 68024,
	SPELL_FLASH_HEAL_25H = 68025,

	SPELL_PENNANCE_10N = 66098,
	SPELL_PENNANCE_25N = 68029,
	SPELL_PENNANCE_10H = 68030,
	SPELL_PENNANCE_25H = 68031,

    SPELL_DISPEL_MAGIC_10N = 65546,
	SPELL_DISPEL_MAGIC_25N = 68624,
	SPELL_DISPEL_MAGIC_10H = 68625,
	SPELL_DISPEL_MAGIC_25H = 68626,

	SPELL_MANA_BURN_10N = 66100,
	SPELL_MANA_BURN_25N = 68026,
	SPELL_MANA_BURN_10H = 68027,
	SPELL_MANA_BURN_25H = 68028,

    SPELL_PSYCHIC_SCREAM = 65543,

    // Priest dps
	SPELL_VAMPIRIC_TOUCH_10N = 65490,
	SPELL_VAMPIRIC_TOUCH_25N = 68091,
	SPELL_VAMPIRIC_TOUCH_10H = 68092,
	SPELL_VAMPIRIC_TOUCH_25H = 68093,

    SPELL_SHADOW_WORD_PAIN_10N = 65541,
	SPELL_SHADOW_WORD_PAIN_25N = 68088,
	SPELL_SHADOW_WORD_PAIN_10H = 68089,
	SPELL_SHADOW_WORD_PAIN_25H = 68090,

    SPELL_MIND_FLAY_10N = 65488,
	SPELL_MIND_FLAY_25N = 68042,
	SPELL_MIND_FLAY_10H = 68043,
	SPELL_MIND_FLAY_25H = 68044,

    SPELL_MIND_BLAST_10N = 65492,
	SPELL_MIND_BLAST_25N = 68038,
	SPELL_MIND_BLAST_10H = 68039,
	SPELL_MIND_BLAST_25H = 68040,

    SPELL_SILENCE                   = 65542,
    SPELL_HORROR                    = 65545,
    SPELL_DISPERSION                = 65544,
    SPELL_SHADOWFORM                = 16592,

    // Warlock
	SPELL_HELLFIRE_10N = 65816,
	SPELL_HELLFIRE_25N = 68145,
	SPELL_HELLFIRE_10H = 68146,
	SPELL_HELLFIRE_25H = 68147,

    SPELL_CORRUPTION_10N = 65810,
	SPELL_CORRUPTION_25N = 68133,
	SPELL_CORRUPTION_10H = 68134,
	SPELL_CORRUPTION_25H = 68135,

    SPELL_CURSE_OF_AGONY_10N = 65814,
	SPELL_CURSE_OF_AGONY_25N = 68136,
	SPELL_CURSE_OF_AGONY_10H = 68137,
	SPELL_CURSE_OF_AGONY_25H = 68138,

	SPELL_SHADOW_BOLT_10N = 65821,
	SPELL_SHADOW_BOLT_25N = 68151,
	SPELL_SHADOW_BOLT_10H = 68152,
	SPELL_SHADOW_BOLT_25H = 68153,

	SPELL_SEARING_PAIN_10N = 65819,
	SPELL_SEARING_PAIN_25N = 68148,
	SPELL_SEARING_PAIN_10H = 68149,
	SPELL_SEARING_PAIN_25H = 68150,

    SPELL_UNSTABLE_AFFLICTION_10N = 65812,
	SPELL_UNSTABLE_AFFLICTION_25N = 68154,
	SPELL_UNSTABLE_AFFLICTION_10H = 68155,
	SPELL_UNSTABLE_AFFLICTION_25H = 68156,

    SPELL_UNSTABLE_AFFLICTION_DISPEL_10N = 65813,
	SPELL_UNSTABLE_AFFLICTION_DISPEL_25N = 68157,
	SPELL_UNSTABLE_AFFLICTION_DISPEL_10H = 68158,
	SPELL_UNSTABLE_AFFLICTION_DISPEL_25H = 68159,

	SPELL_CURSE_OF_EXHAUSTION        = 65815,
    SPELL_FEAR                       = 65809,
    SPELL_SUMMON_FELHUNTER           = 67514,

    // Mage
	SPELL_ARCANE_BARRAGE_10N = 65799,
	SPELL_ARCANE_BARRAGE_25N = 67994,
	SPELL_ARCANE_BARRAGE_10H = 67995,
	SPELL_ARCANE_BARRAGE_25H = 67996,

    SPELL_ARCANE_BLAST_10N = 65791,
	SPELL_ARCANE_BLAST_25N = 67997,
	SPELL_ARCANE_BLAST_10H = 67998,
	SPELL_ARCANE_BLAST_25H = 67999,

    SPELL_ARCANE_EXPLOSION_10N = 65800,
	SPELL_ARCANE_EXPLOSION_25N = 68000,
	SPELL_ARCANE_EXPLOSION_10H = 68001,
	SPELL_ARCANE_EXPLOSION_25H = 68002,

	SPELL_FROSTBOLT_10N = 65807,
	SPELL_FROSTBOLT_25N = 68003,
	SPELL_FROSTBOLT_10H = 68004,
	SPELL_FROSTBOLT_25H = 68005,

    SPELL_BLINK                     = 65793,
    SPELL_COUNTERSPELL              = 65790,
    SPELL_FROST_NOVA                = 65792,
    SPELL_ICE_BLOCK                 = 65802,
    SPELL_POLYMORPH                 = 65801,

    // Hunter
	SPELL_AIMED_SHOT_10N = 65883,
	SPELL_AIMED_SHOT_25N = 67977,
	SPELL_AIMED_SHOT_10H = 67978,
	SPELL_AIMED_SHOT_25H = 67979,

	SPELL_EXPLOSIVE_SHOT_10N = 65866,
	SPELL_EXPLOSIVE_SHOT_25N = 67983,
	SPELL_EXPLOSIVE_SHOT_10H = 67984,
	SPELL_EXPLOSIVE_SHOT_25H = 67985,

	SPELL_SHOOT_10N = 65868,
	SPELL_SHOOT_25N = 67988,
	SPELL_SHOOT_10H = 67989,
	SPELL_SHOOT_25H = 67990,

    SPELL_DETERRENCE                = 65871,
    SPELL_DISENGAGE                 = 65869,
    SPELL_FROST_TRAP                = 65880,
    SPELL_STEADY_SHOT               = 65867,
    SPELL_WING_CLIP                 = 66207,
    SPELL_WYVERN_STING              = 65877,
    SPELL_CALL_PET                  = 67777,

    // Druid dps
	SPELL_INSECT_SWARM_10N = 65855,
	SPELL_INSECT_SWARM_25N = 67941,
	SPELL_INSECT_SWARM_10H = 67942,
	SPELL_INSECT_SWARM_25H = 67943,

    SPELL_MOONFIRE_10N = 65856,
	SPELL_MOONFIRE_25N = 67944,
	SPELL_MOONFIRE_10H = 67945,
	SPELL_MOONFIRE_25H = 67946,

    SPELL_STARFIRE_10N = 65854,
	SPELL_STARFIRE_25N = 67947,
	SPELL_STARFIRE_10H = 67948,
	SPELL_STARFIRE_25H = 67949,

    SPELL_WRATH_10N = 65862,
	SPELL_WRATH_25N = 67951,
	SPELL_WRATH_10H = 67952,
	SPELL_WRATH_25H = 67953,

    SPELL_CYCLONE                   = 65859,
    SPELL_ENTANGLING_ROOTS          = 65857,
    SPELL_FAERIE_FIRE               = 65863,
    SPELL_FORCE_OF_NATURE           = 65861,

    // Warrior
	SPELL_MORTAL_STRIKE_10N = 65926,
	SPELL_MORTAL_STRIKE_25N = 68782,
	SPELL_MORTAL_STRIKE_10H = 68783,
	SPELL_MORTAL_STRIKE_25H = 68784,

	SPELL_CHARGE_10N = 65927,
	SPELL_CHARGE_25N = 68762,
	SPELL_CHARGE_10H = 68763,
	SPELL_CHARGE_25H = 68764,

    SPELL_BLADESTORM                = 65947,
    SPELL_INTIMIDATING_SHOUT        = 65930,
    SPELL_DISARM                    = 65935,
    SPELL_OVERPOWER                 = 65924,
    SPELL_SUNDER_ARMOR              = 65936,
    SPELL_SHATTERING_THROW          = 65940,
    SPELL_RETALIATION               = 65932,

    // Death knight
	SPELL_DEATH_COIL_10N = 66019,
	SPELL_DEATH_COIL_25N = 67929,
	SPELL_DEATH_COIL_10H = 67930,
	SPELL_DEATH_COIL_25H = 67931,

	SPELL_FROST_STRIKE_10N = 66047,
	SPELL_FROST_STRIKE_25N = 67935,
	SPELL_FROST_STRIKE_10H = 67936,
	SPELL_FROST_STRIKE_25H = 67937,

	SPELL_ICY_TOUCH_10N = 66021,
	SPELL_ICY_TOUCH_25N = 67938,
	SPELL_ICY_TOUCH_10H = 67939,
	SPELL_ICY_TOUCH_25H = 67940,

	SPELL_DEATH_GRIP_10N = 66017,
	SPELL_DEATH_GRIP_25N = 68753,
	SPELL_DEATH_GRIP_10H = 68754,
	SPELL_DEATH_GRIP_25H = 68755,

    SPELL_CHAINS_OF_ICE             = 66020,
    SPELL_ICEBOUND_FORTITUDE        = 66023,
    SPELL_STRANGULATE               = 66018,
    SPELL_DEATH_GRIP_PULL           = 64431,    // used at spellscript

    // Rogue
	SPELL_EVISCERATE_10N = 65957,
	SPELL_EVISCERATE_25N = 68094,
	SPELL_EVISCERATE_10H = 68095,
	SPELL_EVISCERATE_25H = 68096,

    SPELL_FAN_OF_KNIVES_10N = 65955,
	SPELL_FAN_OF_KNIVES_25N = 68097,
	SPELL_FAN_OF_KNIVES_10H = 68098,
	SPELL_FAN_OF_KNIVES_25H = 68099,

	SPELL_SHADOWSTEP_10N = 66178,
	SPELL_SHADOWSTEP_25N = 68759,
	SPELL_SHADOWSTEP_10H = 68760,
	SPELL_SHADOWSTEP_25H = 68761,

    SPELL_BLIND                     = 65960,
    SPELL_CLOAK                     = 65961,
    SPELL_BLADE_FLURRY              = 65956,
    SPELL_HEMORRHAGE                = 65954,
    SPELL_WOUND_POISON              = 65962,

    // Shaman dps (some spells taken from shaman healer)
	SPELL_EARTH_SHOCK_ENH_10N       = 65973,
	SPELL_EARTH_SHOCK_ENH_25N       = 68100,
	SPELL_EARTH_SHOCK_ENH_10H       = 68101,
	SPELL_EARTH_SHOCK_ENH_25H       = 68102,
    SPELL_LAVA_LASH                 = 65974,
    SPELL_STORMSTRIKE               = 65970,
    SPELL_WINDFURY                  = 65976,

    // Paladin dps
	SPELL_JUDGEMENT_OF_COMMAND_10N = 66005,
	SPELL_JUDGEMENT_OF_COMMAND_25N = 68017,
	SPELL_JUDGEMENT_OF_COMMAND_10H = 68018,
	SPELL_JUDGEMENT_OF_COMMAND_25H = 68019,

    SPELL_SEAL_OF_COMMAND_10N = 66004,
	SPELL_SEAL_OF_COMMAND_25N = 68020,
	SPELL_SEAL_OF_COMMAND_10H = 68021,
	SPELL_SEAL_OF_COMMAND_25H = 68022,

    SPELL_AVENGING_WRATH                = 66011,
    SPELL_CRUSADER_STRIKE               = 66003,
    SPELL_DIVINE_STORM                  = 66006,
    SPELL_HAMMER_OF_JUSTICE_RETRIBUTION = 66007,
    SPELL_REPENTANCE                    = 66008,

    // Warlock pet
    SPELL_DEVOUR_MAGIC              = 67518,
    SPELL_SPELL_LOCK                = 67519,
    // Hunter pet
	SPELL_CLAW_10N = 67793,
	SPELL_CLAW_25N = 67980,
	SPELL_CLAW_10H = 67981,
	SPELL_CLAW_25H = 67982,
};

enum Events
{
    // generic
    EVENT_THREAT                    = 1,
    EVENT_REMOVE_CC                 = 2,
    // druid healer
    EVENT_LIFEBLOOM                 = 1,
    EVENT_NOURISH                   = 2,
    EVENT_REGROWTH                  = 3,
    EVENT_REJUVENATION              = 4,
    EVENT_TRANQUILITY               = 5,
    EVENT_HEAL_BARKSKIN             = 6,
    EVENT_THORNS                    = 7,
    EVENT_NATURE_GRASP              = 8,
    // shaman healer
    EVENT_HEALING_WAVE              = 1,
    EVENT_RIPTIDE                   = 2,
    EVENT_SPIRIT_CLEANSE            = 3,
    EVENT_HEAL_BLOODLUST_HEROISM    = 4,
    EVENT_HEX                       = 5,
    EVENT_EARTH_SHIELD              = 6,
    EVENT_HEAL_EARTH_SHOCK          = 7,
    // paladin healer
    EVENT_HAND_OF_FREEDOM           = 1,
    EVENT_HEAL_DIVINE_SHIELD        = 2,
    EVENT_CLEANSE                   = 3,
    EVENT_FLASH_OF_LIGHT            = 4,
    EVENT_HOLY_LIGHT                = 5,
    EVENT_HOLY_SHOCK                = 6,
    EVENT_HEAL_HAND_OF_PROTECTION   = 7,
    EVENT_HAMMER_OF_JUSTICE         = 8,
    // priest healer
    EVENT_RENEW                     = 1,
    EVENT_POWER_WORD_SHIELD         = 2,
    EVENT_FLASH_HEAL                = 3,
    EVENT_HEAL_DISPEL               = 4,
    EVENT_HEAL_PSYCHIC_SCREAM       = 5,
    EVENT_MANA_BURN                 = 6,
    EVENT_PENANCE                   = 7,
    // priest dps
    EVENT_SILENCE                   = 1,
    EVENT_VAMPIRIC_TOUCH            = 2,
    EVENT_SHADOW_WORD_PAIN          = 3,
    EVENT_MIND_BLAST                = 4,
    EVENT_HORROR                    = 5,
    EVENT_DISPERSION                = 6,
    EVENT_DPS_DISPEL                = 7,
    EVENT_DPS_PSYCHIC_SCREAM        = 8,
    // warlock
    EVENT_HELLFIRE                  = 1,
    EVENT_CORRUPTION                = 2,
    EVENT_CURSE_OF_AGONY            = 3,
    EVENT_CURSE_OF_EXHAUSTION       = 4,
    EVENT_FEAR                      = 5,
    EVENT_SEARING_PAIN              = 6,
    EVENT_UNSTABLE_AFFLICTION       = 7,
    // mage
    EVENT_ARCANE_BARRAGE            = 1,
    EVENT_ARCANE_BLAST              = 2,
    EVENT_ARCANE_EXPLOSION          = 3,
    EVENT_BLINK                     = 4,
    EVENT_COUNTERSPELL              = 5,
    EVENT_FROST_NOVA                = 6,
    EVENT_ICE_BLOCK                 = 7,
    EVENT_POLYMORPH                 = 8,
    // hunter
    EVENT_AIMED_SHOT                = 1,
    EVENT_DETERRENCE                = 2,
    EVENT_DISENGAGE                 = 3,
    EVENT_EXPLOSIVE_SHOT            = 4,
    EVENT_FROST_TRAP                = 5,
    EVENT_STEADY_SHOT               = 6,
    EVENT_WING_CLIP                 = 7,
    EVENT_WYVERN_STING              = 8,
    // druid dps
    EVENT_CYCLONE                   = 1,
    EVENT_ENTANGLING_ROOTS          = 2,
    EVENT_FAERIE_FIRE               = 3,
    EVENT_FORCE_OF_NATURE           = 4,
    EVENT_INSECT_SWARM              = 5,
    EVENT_MOONFIRE                  = 6,
    EVENT_STARFIRE                  = 7,
    EVENT_DPS_BARKSKIN              = 8,
    // warrior
    EVENT_BLADESTORM                = 1,
    EVENT_INTIMIDATING_SHOUT        = 2,
    EVENT_MORTAL_STRIKE             = 3,
    EVENT_WARR_CHARGE               = 4,
    EVENT_DISARM                    = 5,
    EVENT_OVERPOWER                 = 6,
    EVENT_SUNDER_ARMOR              = 7,
    EVENT_SHATTERING_THROW          = 8,
    EVENT_RETALIATION               = 9,
    // death knight
    EVENT_CHAINS_OF_ICE             = 1,
    EVENT_DEATH_COIL                = 2,
    EVENT_DEATH_GRIP                = 3,
    EVENT_FROST_STRIKE              = 4,
    EVENT_ICEBOUND_FORTITUDE        = 5,
    EVENT_ICY_TOUCH                 = 6,
    EVENT_STRANGULATE               = 7,
    // rogue
    EVENT_FAN_OF_KNIVES             = 1,
    EVENT_BLIND                     = 2,
    EVENT_CLOAK                     = 3,
    EVENT_BLADE_FLURRY              = 4,
    EVENT_SHADOWSTEP                = 5,
    EVENT_HEMORRHAGE                = 6,
    EVENT_EVISCERATE                = 7,
    EVENT_WOUND_POISON              = 8,
    // shaman dps
    EVENT_DPS_EARTH_SHOCK           = 1,
    EVENT_LAVA_LASH                 = 2,
    EVENT_STORMSTRIKE               = 3,
    EVENT_DPS_BLOODLUST_HEROISM     = 4,
    EVENT_DEPLOY_TOTEM              = 5,
    EVENT_WINDFURY                  = 6,
    // paladin dps
    EVENT_AVENGING_WRATH            = 1,
    EVENT_CRUSADER_STRIKE           = 2,
    EVENT_DIVINE_STORM              = 3,
    EVENT_HAMMER_OF_JUSTICE_RET     = 4,
    EVENT_JUDGEMENT_OF_COMMAND      = 5,
    EVENT_REPENTANCE                = 6,
    EVENT_DPS_HAND_OF_PROTECTION    = 7,
    EVENT_DPS_DIVINE_SHIELD         = 8,
    // warlock pet
    EVENT_DEVOUR_MAGIC              = 1,
    EVENT_SPELL_LOCK                = 2,
};

class boss_toc_champion_controller : public CreatureScript
{
public:
    boss_toc_champion_controller() : CreatureScript("boss_toc_champion_controller") { }

    struct boss_toc_champion_controllerAI : public QuantumBasicAI
    {
        boss_toc_champion_controllerAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
			Reset();
        }

        InstanceScript* instance;
        SummonList Summons;

        uint32 ChampionsNotStarted;
        uint32 ChampionsFailed;
        uint32 ChampionsKilled;
        bool InProgress;

		void Reset()
		{
			ChampionsNotStarted = 0;
            ChampionsFailed = 0;
            ChampionsKilled = 0;
            InProgress = false;
		}

        std::vector<uint32> SelectChampions(Team playerTeam)
        {
            std::vector<uint32> vHealersEntries;
            vHealersEntries.clear();
            vHealersEntries.push_back(playerTeam == ALLIANCE ? NPC_HORDE_DRUID_RESTORATION : NPC_ALLIANCE_DRUID_RESTORATION);
            vHealersEntries.push_back(playerTeam == ALLIANCE ? NPC_HORDE_PALADIN_HOLY : NPC_ALLIANCE_PALADIN_HOLY);
            vHealersEntries.push_back(playerTeam == ALLIANCE ? NPC_HORDE_PRIEST_DISCIPLINE : NPC_ALLIANCE_PRIEST_DISCIPLINE);
            vHealersEntries.push_back(playerTeam == ALLIANCE ? NPC_HORDE_SHAMAN_RESTORATION : NPC_ALLIANCE_SHAMAN_RESTORATION);

            std::vector<uint32> vOtherEntries;
            vOtherEntries.clear();
            vOtherEntries.push_back(playerTeam == ALLIANCE ? NPC_HORDE_DEATH_KNIGHT : NPC_ALLIANCE_DEATH_KNIGHT);
            vOtherEntries.push_back(playerTeam == ALLIANCE ? NPC_HORDE_HUNTER : NPC_ALLIANCE_HUNTER);
            vOtherEntries.push_back(playerTeam == ALLIANCE ? NPC_HORDE_MAGE : NPC_ALLIANCE_MAGE);
            vOtherEntries.push_back(playerTeam == ALLIANCE ? NPC_HORDE_ROGUE : NPC_ALLIANCE_ROGUE);
            vOtherEntries.push_back(playerTeam == ALLIANCE ? NPC_HORDE_WARLOCK : NPC_ALLIANCE_WARLOCK);
            vOtherEntries.push_back(playerTeam == ALLIANCE ? NPC_HORDE_WARRIOR : NPC_ALLIANCE_WARRIOR);

            uint8 healersSubtracted = 2;
            if (instance->instance->GetSpawnMode() == RAID_DIFFICULTY_25MAN_NORMAL || instance->instance->GetSpawnMode() == RAID_DIFFICULTY_25MAN_HEROIC)
                healersSubtracted = 1;
            for (uint8 i = 0; i < healersSubtracted; ++i)
            {
                uint8 pos = urand(0, vHealersEntries.size()-1);
                switch (vHealersEntries[pos])
                {
                    case NPC_ALLIANCE_DRUID_RESTORATION:
                        vOtherEntries.push_back(NPC_ALLIANCE_DRUID_BALANCE);
                        break;
                    case NPC_HORDE_DRUID_RESTORATION:
                        vOtherEntries.push_back(NPC_HORDE_DRUID_BALANCE);
                        break;
                    case NPC_ALLIANCE_PALADIN_HOLY:
                        vOtherEntries.push_back(NPC_ALLIANCE_PALADIN_RETRIBUTION);
                        break;
                    case NPC_HORDE_PALADIN_HOLY:
                        vOtherEntries.push_back(NPC_HORDE_PALADIN_RETRIBUTION);
                        break;
                    case NPC_ALLIANCE_PRIEST_DISCIPLINE:
                        vOtherEntries.push_back(NPC_ALLIANCE_PRIEST_SHADOW);
                        break;
                    case NPC_HORDE_PRIEST_DISCIPLINE:
                        vOtherEntries.push_back(NPC_HORDE_PRIEST_SHADOW);
                        break;
                    case NPC_ALLIANCE_SHAMAN_RESTORATION:
                        vOtherEntries.push_back(NPC_ALLIANCE_SHAMAN_ENHANCEMENT);
                        break;
                    case NPC_HORDE_SHAMAN_RESTORATION:
                        vOtherEntries.push_back(NPC_HORDE_SHAMAN_ENHANCEMENT);
                        break;
                }
                vHealersEntries.erase(vHealersEntries.begin()+pos);
            }

            if (instance->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_NORMAL || instance->instance->GetSpawnMode() == RAID_DIFFICULTY_10MAN_HEROIC)
                for (uint8 i = 0; i < 4; ++i)
                    vOtherEntries.erase(vOtherEntries.begin()+urand(0, vOtherEntries.size()-1));

            std::vector<uint32> vChampionEntries;
            vChampionEntries.clear();
            for (uint8 i = 0; i < vHealersEntries.size(); ++i)
                vChampionEntries.push_back(vHealersEntries[i]);
            for (uint8 i = 0; i < vOtherEntries.size(); ++i)
                vChampionEntries.push_back(vOtherEntries[i]);

            return vChampionEntries;
        }

        void SummonChampions(Team playerTeam)
        {
            std::vector<Position> vChampionJumpOrigin;
            if (playerTeam == ALLIANCE)
                for (uint8 i = 0; i < 5; i++)
                    vChampionJumpOrigin.push_back(FactionChampionLoc[i]);
            else
                for (uint8 i = 5; i < 10; i++)
                    vChampionJumpOrigin.push_back(FactionChampionLoc[i]);

            std::vector<Position> vChampionJumpTarget;
            for (uint8 i = 10; i < 20; i++)
                vChampionJumpTarget.push_back(FactionChampionLoc[i]);
            std::vector<uint32> vChampionEntries = SelectChampions(playerTeam);

            for (uint8 i = 0; i < vChampionEntries.size(); ++i)
            {
                uint8 pos = urand(0, vChampionJumpTarget.size()-1);
                if (Creature* temp = me->SummonCreature(vChampionEntries[i], vChampionJumpOrigin[urand(0, vChampionJumpOrigin.size()-1)], TEMPSUMMON_MANUAL_DESPAWN))
                {
                    Summons.Summon(temp);
                    temp->SetReactState(REACT_PASSIVE);
                    temp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                    if (playerTeam == ALLIANCE)
                    {
                        temp->SetHomePosition(vChampionJumpTarget[pos].GetPositionX(), vChampionJumpTarget[pos].GetPositionY(), vChampionJumpTarget[pos].GetPositionZ(), 0);
                        temp->GetMotionMaster()->MoveJump(vChampionJumpTarget[pos].GetPositionX(), vChampionJumpTarget[pos].GetPositionY(), vChampionJumpTarget[pos].GetPositionZ(), 20.0f, 20.0f);
                        temp->SetOrientation(0);
                    }
                    else
                    {
                        temp->SetHomePosition((ToCCommonLoc[1].GetPositionX()*2)-vChampionJumpTarget[pos].GetPositionX(), vChampionJumpTarget[pos].GetPositionY(), vChampionJumpTarget[pos].GetPositionZ(), 3);
                        temp->GetMotionMaster()->MoveJump((ToCCommonLoc[1].GetPositionX()*2)-vChampionJumpTarget[pos].GetPositionX(), vChampionJumpTarget[pos].GetPositionY(), vChampionJumpTarget[pos].GetPositionZ(), 20.0f, 20.0f);
                        temp->SetOrientation(3);
                    }
                }
                vChampionJumpTarget.erase(vChampionJumpTarget.begin()+pos);
            }
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
                case 0:
                    SummonChampions((Team)data);
                    break;
                case 1:
                    for (std::list<uint64>::iterator i = Summons.begin(); i != Summons.end(); ++i)
                    {
                        if (Creature* temp = Unit::GetCreature(*me, *i))
                        {
                            temp->SetReactState(REACT_AGGRESSIVE);
                            temp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                        }
                    }
                    break;
                case 2:
                    switch (data)
                    {
                        case FAIL:
                            ChampionsFailed++;
                            if (ChampionsFailed + ChampionsKilled >= Summons.size())
                            {
                                instance->SetData(TYPE_CRUSADERS, FAIL);
                                Summons.DespawnAll();
                                me->DespawnAfterAction();
                            }
                            break;
                        case IN_PROGRESS:
                            if (!InProgress)
                            {
                                ChampionsNotStarted = 0;
                                ChampionsFailed = 0;
                                ChampionsKilled = 0;
                                InProgress = true;
                                Summons.DoZoneInCombat();
                                instance->SetData(TYPE_CRUSADERS, IN_PROGRESS);
                            }
                            break;
                        case DONE:
                            ChampionsKilled++;
                            if (ChampionsKilled == 1)
                                instance->SetData(TYPE_CRUSADERS, SPECIAL);
                            else if (ChampionsKilled >= Summons.size())
                            {
                                instance->SetData(TYPE_CRUSADERS, DONE);
                                Summons.DespawnAll();
                                me->DespawnAfterAction();
                            }
                            break;
						default:
							break;
                    }
                    break;
				default:
					break;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_toc_champion_controllerAI(creature);
    }
};

struct boss_faction_championsAI : public BossAI
{
    boss_faction_championsAI(Creature* creature, uint32 aitype) : BossAI(creature, TYPE_CRUSADERS)
    {
        _aiType = aitype;
    }

    void Reset()
    {
		DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

		me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

        _events.ScheduleEvent(EVENT_THREAT, 5*IN_MILLISECONDS);
        if (IsHeroic() && (_aiType != AI_PET))
            _events.ScheduleEvent(EVENT_REMOVE_CC, 5*IN_MILLISECONDS);
    }

    void JustReachedHome()
    {
        if (instance)
		{
            if (Creature* pChampionController = Unit::GetCreature((*me), instance->GetData64(NPC_CHAMPIONS_CONTROLLER)))
                pChampionController->AI()->SetData(2, FAIL);
		}

        me->DespawnAfterAction();
    }

    float CalculateThreat(float distance, float armor, uint32 health)
    {
        float dist_mod = (_aiType == AI_MELEE || _aiType == AI_PET) ? 15.0f / (15.0f + distance) : 1.0f;
        float armor_mod = (_aiType == AI_MELEE || _aiType == AI_PET) ? armor / 16635.0f : 0.0f;
        float eh = (health + 1) * (1.0f + armor_mod);
        return dist_mod * 30000.0f / eh;
    }

    void UpdateThreat()
    {
        std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
        for (std::list<HostileReference*>::const_iterator itr = tList.begin(); itr != tList.end(); ++itr)
        {
            Unit* unit = Unit::GetUnit(*me, (*itr)->getUnitGuid());
            if (unit && me->getThreatManager().getThreat(unit))
            {
                if (unit->GetTypeId() == TYPE_ID_PLAYER)
                {
                    float threat = CalculateThreat(me->GetDistance2d(unit), (float)unit->GetArmor(), unit->GetHealth());
                    me->getThreatManager().modifyThreatPercent(unit, -100);
                    me->AddThreat(unit, 1000000.0f * threat);
                }
            }
        }
    }

    void UpdatePower()
    {
        if (me->GetPowerType() == POWER_MANA)
            me->ModifyPower(POWER_MANA, me->GetMaxPower(POWER_MANA) / 3);
    }

    void RemoveCC()
    {
        me->RemoveAurasByType(SPELL_AURA_MOD_STUN);
        me->RemoveAurasByType(SPELL_AURA_MOD_FEAR);
        me->RemoveAurasByType(SPELL_AURA_MOD_ROOT);
        me->RemoveAurasByType(SPELL_AURA_MOD_PACIFY);
        me->RemoveAurasByType(SPELL_AURA_MOD_CONFUSE);
        DoCast(me, SPELL_PVP_TRINKET);
    }

    void JustDied(Unit* /*killer*/)
    {
        if (_aiType != AI_PET)
            if (instance)
                if (Creature* pChampionController = Unit::GetCreature((*me), instance->GetData64(NPC_CHAMPIONS_CONTROLLER)))
                    pChampionController->AI()->SetData(2, DONE);
    }

    void EnterToBattle(Unit* /*who*/)
    {
        DoCast(me, SPELL_ANTI_AOE, true);

        _EnterToBattle();

		if (Creature* pChampionController = Unit::GetCreature((*me), instance->GetData64(NPC_CHAMPIONS_CONTROLLER)))
			pChampionController->AI()->SetData(2, IN_PROGRESS);
    }

    void KilledUnit(Unit* who)
    {
        if (who->GetTypeId() == TYPE_ID_PLAYER)
        {
            Map::PlayerList const &players = me->GetMap()->GetPlayers();
            uint32 TeamInInstance = 0;

            if (!players.isEmpty())
			{
                if (Player* player = players.begin()->getSource())
                    TeamInInstance = player->GetTeam();
			}

			if (TeamInInstance == HORDE)
			{
				if (Creature* varian = Unit::GetCreature(*me, instance->GetData64(NPC_KING_VARIAN_WRYNN)))
					DoSendQuantumText(RAND(SAY_VARIAN_KILL_HORDE_PLAYER_1, SAY_VARIAN_KILL_HORDE_PLAYER_2, SAY_VARIAN_KILL_HORDE_PLAYER_3, SAY_VARIAN_KILL_HORDE_PLAYER_4), varian);
			}

			if (TeamInInstance == ALLIANCE)
			{
				if (Creature* garrosh = me->FindCreatureWithDistance(NPC_GARROSH_HELLSCREAM, 300.f))
					DoSendQuantumText(RAND(SAY_GARROSH_KILL_ALLIANCE_PLAYER_1, SAY_GARROSH_KILL_ALLIANCE_PLAYER_2, SAY_GARROSH_KILL_ALLIANCE_PLAYER_3, SAY_GARROSH_KILL_ALLIANCE_PLAYER_4), garrosh);
			}
        }
    }

	void EnterEvadeMode()
	{
		BossAI::EnterEvadeMode();
	}

    Creature* SelectRandomFriendlyMissingBuff(uint32 spell)
    {
        std::list<Creature*> lst = DoFindFriendlyMissingBuff(40.0f, spell);
        std::list<Creature*>::const_iterator itr = lst.begin();
        if (lst.empty())
            return NULL;
        advance(itr, rand() % lst.size());
        return (*itr);
    }

    Unit* SelectEnemyCaster(bool /*casting*/)
    {
        std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
        std::list<HostileReference*>::const_iterator iter;
        Unit* target;
        for (iter = tList.begin(); iter!=tList.end(); ++iter)
        {
            target = Unit::GetUnit(*me, (*iter)->getUnitGuid());
            if (target && target->GetPowerType() == POWER_MANA)
                return target;
        }
        return NULL;
    }

    uint32 EnemiesInRange(float distance)
    {
        std::list<HostileReference*> const& tList = me->getThreatManager().getThreatList();
        std::list<HostileReference*>::const_iterator iter;
        uint32 count = 0;
        Unit* target;
        for (iter = tList.begin(); iter != tList.end(); ++iter)
        {
            target = Unit::GetUnit(*me, (*iter)->getUnitGuid());
                if (target && me->GetDistance2d(target) < distance)
                    ++count;
        }
        return count;
    }

    void AttackStart(Unit* who)
    {
        if (!who)
            return;

        if (me->Attack(who, true))
        {
            me->AddThreat(who, 10.0f);
            me->SetInCombatWith(who);
            who->SetInCombatWith(me);

            if (_aiType == AI_MELEE || _aiType == AI_PET)
                DoStartMovement(who);
            else
                DoStartMovement(who, 20.0f);
            SetCombatMovement(true);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        _events.Update(diff);

        while (uint32 eventId = _events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_THREAT:
                    UpdatePower();
                    UpdateThreat();
                    _events.ScheduleEvent(EVENT_THREAT, 4*IN_MILLISECONDS);
                    return;
                case EVENT_REMOVE_CC:
                    if (me->HasBreakableByDamageCrowdControlAura())
                    {
                        RemoveCC();
                        _events.RescheduleEvent(EVENT_REMOVE_CC, 2*MINUTE*IN_MILLISECONDS);
                    }
                    else
                        _events.RescheduleEvent(EVENT_REMOVE_CC, 3*IN_MILLISECONDS);
                    return;
                default:
                    return;
            }
        }

        if (_aiType == AI_MELEE || _aiType == AI_PET)
            DoMeleeAttackIfReady();
    }

    private:
        uint32 _aiType;
        // make sure that every bosses separate events dont mix with these _events
        EventMap _events;
};

/********************************************************************
                            HEALERS
********************************************************************/

class npc_trial_of_the_crusader_restor_druid : public CreatureScript
{
    public:
        npc_trial_of_the_crusader_restor_druid() : CreatureScript("npc_trial_of_the_crusader_restor_druid") { }

        struct npc_trial_of_the_crusader_restor_druidAI : public boss_faction_championsAI
        {
            npc_trial_of_the_crusader_restor_druidAI(Creature* creature) : boss_faction_championsAI(creature, AI_HEALER) {}

            void Reset()
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_LIFEBLOOM, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_NOURISH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_REGROWTH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_REJUVENATION, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_TRANQUILITY, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HEAL_BARKSKIN, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_THORNS, 2*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_NATURE_GRASP, urand(3*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                SetEquipmentSlots(false, 51799, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_LIFEBLOOM:
                            if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW))
								DoCast(target, RAID_MODE(SPELL_LIFEBLOOM_10N, SPELL_LIFEBLOOM_25N, SPELL_LIFEBLOOM_10H, SPELL_LIFEBLOOM_25H));

                            events.ScheduleEvent(EVENT_LIFEBLOOM, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_NOURISH:
                            if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW))
								DoCast(target, RAID_MODE(SPELL_NOURISH_10N, SPELL_NOURISH_25N, SPELL_NOURISH_10H, SPELL_NOURISH_25H));

                            events.ScheduleEvent(EVENT_NOURISH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_REGROWTH:
                            if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW))
								DoCast(target, RAID_MODE(SPELL_REGROWTH_10N, SPELL_REGROWTH_25N, SPELL_REGROWTH_10H, SPELL_REGROWTH_25H));

                            events.ScheduleEvent(EVENT_REGROWTH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_REJUVENATION:
                            if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW))
								DoCast(target, RAID_MODE(SPELL_REJUVENATION_10N, SPELL_REJUVENATION_25N, SPELL_REJUVENATION_10H, SPELL_REJUVENATION_25H));

                            events.ScheduleEvent(EVENT_REJUVENATION, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_TRANQUILITY:
							DoCastAOE(RAID_MODE(SPELL_TRANQUILITY_10N, SPELL_TRANQUILITY_25N, SPELL_TRANQUILITY_10H, SPELL_TRANQUILITY_25H));
                            events.ScheduleEvent(EVENT_TRANQUILITY, urand(15*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                            return;
                        case EVENT_HEAL_BARKSKIN:
                            if (HealthBelowPct(HEALTH_PERCENT_30))
                            {
                                DoCast(me, SPELL_BARKSKIN);
                                events.RescheduleEvent(EVENT_HEAL_BARKSKIN, 60*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_HEAL_BARKSKIN, 3*IN_MILLISECONDS);
                            return;
                        case EVENT_THORNS:
                            if (Creature* target = SelectRandomFriendlyMissingBuff(SPELL_THORNS))
                                DoCast(target, SPELL_THORNS);
                            events.ScheduleEvent(EVENT_THORNS, urand(25*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                            return;
                        case EVENT_NATURE_GRASP:
                            DoCast(me, SPELL_NATURE_GRASP);
                            events.ScheduleEvent(EVENT_NATURE_GRASP, 60*IN_MILLISECONDS);
                            return;
                        default:
                            return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_trial_of_the_crusader_restor_druidAI (creature);
        }
};

class npc_trial_of_the_crusader_shaman : public CreatureScript
{
    public:
        npc_trial_of_the_crusader_shaman() : CreatureScript("npc_trial_of_the_crusader_shaman") { }

        struct npc_trial_of_the_crusader_shamanAI : public boss_faction_championsAI
        {
            npc_trial_of_the_crusader_shamanAI(Creature* creature) : boss_faction_championsAI(creature, AI_HEALER) {}

            void Reset()
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_HEALING_WAVE, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_RIPTIDE, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SPIRIT_CLEANSE, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HEAL_BLOODLUST_HEROISM, 20*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_HEX, urand(5*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_EARTH_SHIELD, 1*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_HEAL_EARTH_SHOCK, urand(5*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                SetEquipmentSlots(false, 49992, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_HEALING_WAVE:
                            if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW))
								DoCast(target, RAID_MODE(SPELL_HEALING_WAVE_10N, SPELL_HEALING_WAVE_25N, SPELL_HEALING_WAVE_10H, SPELL_HEALING_WAVE_25H));

                            events.ScheduleEvent(EVENT_HEALING_WAVE, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                            return;
                        case EVENT_RIPTIDE:
                            if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW))
								DoCast(target, RAID_MODE(SPELL_RIPTIDE_10N, SPELL_RIPTIDE_25N, SPELL_RIPTIDE_10H, SPELL_RIPTIDE_25H));

                            events.ScheduleEvent(EVENT_RIPTIDE, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_SPIRIT_CLEANSE:
                            if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW))
								DoCast(target, RAID_MODE(SPELL_SPIRIT_CLEANSE_10N, SPELL_SPIRIT_CLEANSE_25N, SPELL_SPIRIT_CLEANSE_10H, SPELL_SPIRIT_CLEANSE_25H));

                            events.ScheduleEvent(EVENT_SPIRIT_CLEANSE, urand(15*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_HEAL_BLOODLUST_HEROISM:
                            if (me->GetFaction()) // alliance = 1
                            {
                                if (!me->HasAura(AURA_EXHAUSTION))
                                    DoCastAOE(SPELL_HEROISM);
                            }
                            else
                            {
                                if (!me->HasAura(AURA_SATED))
                                    DoCastAOE(SPELL_BLOODLUST);
                            }
                            events.ScheduleEvent(EVENT_HEAL_BLOODLUST_HEROISM, 5*MINUTE*IN_MILLISECONDS);
                            return;
                        case EVENT_HEX:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_HEX);

                            events.ScheduleEvent(EVENT_HEX, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_EARTH_SHIELD:
                            if (Creature* target = SelectRandomFriendlyMissingBuff(SPELL_EARTH_SHIELD))
                                DoCast(target, SPELL_EARTH_SHIELD);

                            events.ScheduleEvent(EVENT_EARTH_SHIELD, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_HEAL_EARTH_SHOCK:
                            if (Unit* target = SelectEnemyCaster(true))
								DoCast(target, RAID_MODE(SPELL_EARTH_SHOCK_10N, SPELL_EARTH_SHOCK_25N, SPELL_EARTH_SHOCK_10H, SPELL_EARTH_SHOCK_25H));

                            events.ScheduleEvent(EVENT_HEAL_EARTH_SHOCK, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        default:
                            return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_trial_of_the_crusader_shamanAI (creature);
        }
};

class npc_trial_of_the_crusader_holy_paladin : public CreatureScript
{
    public:
        npc_trial_of_the_crusader_holy_paladin() : CreatureScript("npc_trial_of_the_crusader_holy_paladin") { }

        struct npc_trial_of_the_crusader_holy_paladinAI : public boss_faction_championsAI
        {
            npc_trial_of_the_crusader_holy_paladinAI(Creature* creature) : boss_faction_championsAI(creature, AI_HEALER) {}

            void Reset()
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_HAND_OF_FREEDOM, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HEAL_DIVINE_SHIELD, 20*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_CLEANSE, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FLASH_OF_LIGHT, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HOLY_LIGHT, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HOLY_SHOCK, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HEAL_HAND_OF_PROTECTION, urand(30*IN_MILLISECONDS, 60*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HAMMER_OF_JUSTICE, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                SetEquipmentSlots(false, 50771, 47079, EQUIP_NO_CHANGE);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_HAND_OF_FREEDOM:
							if (Unit* target = SelectRandomFriendlyMissingBuff(SPELL_HAND_OF_FREEDOM_10N || SPELL_HAND_OF_FREEDOM_25N || SPELL_HAND_OF_FREEDOM_10H || SPELL_HAND_OF_FREEDOM_25H))
								DoCast(target, RAID_MODE(SPELL_HAND_OF_FREEDOM_10N, SPELL_HAND_OF_FREEDOM_25N, SPELL_HAND_OF_FREEDOM_10H, SPELL_HAND_OF_FREEDOM_25H));

                            events.ScheduleEvent(EVENT_HAND_OF_FREEDOM, urand(15*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_HEAL_DIVINE_SHIELD:
                            if (HealthBelowPct(HEALTH_PERCENT_30) && !me->HasAura(SPELL_FORBEARANCE))
                            {
                                DoCast(me, SPELL_DIVINE_SHIELD);
                                events.RescheduleEvent(EVENT_HEAL_DIVINE_SHIELD, 5*MINUTE*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_HEAL_DIVINE_SHIELD, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_CLEANSE:
                            if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW))
								DoCast(target, RAID_MODE(SPELL_CLEANSE_10N, SPELL_CLEANSE_25N, SPELL_CLEANSE_10H, SPELL_CLEANSE_25H));

                            events.ScheduleEvent(EVENT_CLEANSE, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_FLASH_OF_LIGHT:
                            if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW))
								DoCast(target, RAID_MODE(SPELL_FLASH_OF_LIGHT_10N, SPELL_FLASH_OF_LIGHT_25N, SPELL_FLASH_OF_LIGHT_10H, SPELL_FLASH_OF_LIGHT_25H));

                            events.ScheduleEvent(EVENT_FLASH_OF_LIGHT, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                            return;
                        case EVENT_HOLY_LIGHT:
                            if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW))
								DoCast(target, RAID_MODE(SPELL_HOLY_LIGHT_10N, SPELL_HOLY_LIGHT_25N, SPELL_HOLY_LIGHT_10H, SPELL_HOLY_LIGHT_25H));

                            events.ScheduleEvent(EVENT_HOLY_LIGHT, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        case EVENT_HOLY_SHOCK:
                            if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW))
								DoCast(target, RAID_MODE(SPELL_HOLY_SHOCK_10N, SPELL_HOLY_SHOCK_25N, SPELL_HOLY_SHOCK_10H, SPELL_HOLY_SHOCK_25H));

                            events.ScheduleEvent(EVENT_HOLY_SHOCK, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_HEAL_HAND_OF_PROTECTION:
                            if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW))
                            {
                                if (!target->HasAura(SPELL_FORBEARANCE))
                                {
                                    DoCast(target, SPELL_HAND_OF_PROTECTION);
                                    events.RescheduleEvent(EVENT_HEAL_HAND_OF_PROTECTION, 5*MINUTE*IN_MILLISECONDS);
                                }
                                else
                                    events.RescheduleEvent(EVENT_HEAL_HAND_OF_PROTECTION, 3*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_HEAL_HAND_OF_PROTECTION, 10*IN_MILLISECONDS);
                            return;
                        case EVENT_HAMMER_OF_JUSTICE:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                                DoCast(target, SPELL_HAMMER_OF_JUSTICE);
                            events.ScheduleEvent(EVENT_HAMMER_OF_JUSTICE, 40*IN_MILLISECONDS);
                            return;
                        default:
                            return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_trial_of_the_crusader_holy_paladinAI (creature);
        }
};

class npc_trial_of_the_crusader_discipline_priest : public CreatureScript
{
    public:
        npc_trial_of_the_crusader_discipline_priest() : CreatureScript("npc_trial_of_the_crusader_discipline_priest") { }

        struct npc_trial_of_the_crusader_discipline_priestAI : public boss_faction_championsAI
        {
            npc_trial_of_the_crusader_discipline_priestAI(Creature* creature) : boss_faction_championsAI(creature, AI_HEALER) {}

            void Reset()
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_RENEW, urand(3*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_POWER_WORD_SHIELD, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FLASH_HEAL, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HEAL_DISPEL, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HEAL_PSYCHIC_SCREAM, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_MANA_BURN, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_PENANCE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                SetEquipmentSlots(false, 49992, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_RENEW:
                            if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW))
								DoCast(target, RAID_MODE(SPELL_RENEW_10N, SPELL_RENEW_25N, SPELL_RENEW_10H, SPELL_RENEW_25H));

                            events.ScheduleEvent(EVENT_RENEW, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                            return;
                        case EVENT_POWER_WORD_SHIELD:
                            if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW))
								DoCast(target, RAID_MODE(SPELL_POWER_WORD_SHIELD_10N, SPELL_POWER_WORD_SHIELD_25N, SPELL_POWER_WORD_SHIELD_10H, SPELL_POWER_WORD_SHIELD_25H));

                            events.ScheduleEvent(EVENT_POWER_WORD_SHIELD, urand(15*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_FLASH_HEAL:
                            if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW))
								DoCast(target, RAID_MODE(SPELL_FLASH_HEAL_10N, SPELL_FLASH_HEAL_25N, SPELL_FLASH_HEAL_10H, SPELL_FLASH_HEAL_25H));

                            events.ScheduleEvent(EVENT_FLASH_HEAL, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                            return;
                        case EVENT_HEAL_DISPEL:
                            if (Unit* target = urand(0, 1) ? SelectTarget(TARGET_RANDOM, 0) : SelectTargetHpFriendly(HEAL_PERCENT_LOW))
								DoCast(target, RAID_MODE(SPELL_DISPEL_MAGIC_10N, SPELL_DISPEL_MAGIC_25N, SPELL_DISPEL_MAGIC_10H, SPELL_DISPEL_MAGIC_25H));

                            events.ScheduleEvent(EVENT_HEAL_DISPEL, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_HEAL_PSYCHIC_SCREAM:
                            if (EnemiesInRange(10.0f) >= 2)
                                DoCastAOE(SPELL_PSYCHIC_SCREAM);

                            events.ScheduleEvent(EVENT_HEAL_PSYCHIC_SCREAM, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_MANA_BURN:
                            if (Unit* target = SelectEnemyCaster(false))
								DoCast(target, RAID_MODE(SPELL_MANA_BURN_10N, SPELL_MANA_BURN_25N, SPELL_MANA_BURN_10H, SPELL_MANA_BURN_25H));

                            events.ScheduleEvent(EVENT_MANA_BURN, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_PENANCE:
                            if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW))
								DoCast(target, RAID_MODE(SPELL_PENNANCE_10N, SPELL_PENNANCE_25N, SPELL_PENNANCE_10H, SPELL_PENNANCE_25H));

                            events.ScheduleEvent(EVENT_PENANCE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        default:
                            return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_trial_of_the_crusader_discipline_priestAI (creature);
        }
};

/********************************************************************
                            RANGED
********************************************************************/

class npc_trial_of_the_crusader_shadow_priest : public CreatureScript
{
    public:
        npc_trial_of_the_crusader_shadow_priest() : CreatureScript("npc_trial_of_the_crusader_shadow_priest") { }

        struct npc_trial_of_the_crusader_shadow_priestAI : public boss_faction_championsAI
        {
            npc_trial_of_the_crusader_shadow_priestAI(Creature* creature) : boss_faction_championsAI(creature, AI_RANGED) {}

            void Reset()
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_SILENCE, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_VAMPIRIC_TOUCH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SHADOW_WORD_PAIN, urand(3*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_MIND_BLAST, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HORROR, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DISPERSION, urand(20*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DPS_DISPEL, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DPS_PSYCHIC_SCREAM, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                SetEquipmentSlots(false, 50040, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
                DoCast(me, SPELL_SHADOWFORM);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SILENCE:
                            if (Unit* target = SelectEnemyCaster(true))
                                DoCast(target, SPELL_SILENCE);

                            events.ScheduleEvent(EVENT_SILENCE, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_VAMPIRIC_TOUCH:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
								DoCast(target, RAID_MODE(SPELL_VAMPIRIC_TOUCH_10N, SPELL_VAMPIRIC_TOUCH_25N, SPELL_VAMPIRIC_TOUCH_10H, SPELL_VAMPIRIC_TOUCH_25H));

                            events.ScheduleEvent(EVENT_VAMPIRIC_TOUCH, urand(10*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_SHADOW_WORD_PAIN:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
								DoCast(target, RAID_MODE(SPELL_SHADOW_WORD_PAIN_10N, SPELL_SHADOW_WORD_PAIN_25N, SPELL_SHADOW_WORD_PAIN_10H, SPELL_SHADOW_WORD_PAIN_25H));

                            events.ScheduleEvent(EVENT_SHADOW_WORD_PAIN, urand(10*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_MIND_BLAST:
							DoCastVictim(RAID_MODE(SPELL_MIND_BLAST_10N, SPELL_MIND_BLAST_25N, SPELL_MIND_BLAST_10H, SPELL_MIND_BLAST_25H));
                            events.ScheduleEvent(EVENT_MIND_BLAST, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_HORROR:
                            DoCastVictim(SPELL_HORROR);
                            events.ScheduleEvent(EVENT_HORROR, urand(15*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_DISPERSION:
                            if (HealthBelowPct(HEALTH_PERCENT_40))
                            {
                                DoCast(me, SPELL_DISPERSION);
                                events.RescheduleEvent(EVENT_DISPERSION, 180*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_DISPERSION, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_DPS_DISPEL:
                            if (Unit* target = urand(0, 1) ? SelectTarget(TARGET_RANDOM, 0) : SelectTargetHpFriendly(HEAL_PERCENT_LOW))
								DoCast(target, RAID_MODE(SPELL_DISPEL_MAGIC_10N, SPELL_DISPEL_MAGIC_25N, SPELL_DISPEL_MAGIC_10H, SPELL_DISPEL_MAGIC_25H));

                            events.ScheduleEvent(EVENT_DPS_DISPEL, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_DPS_PSYCHIC_SCREAM:
                            if (EnemiesInRange(10.0f) >= 2)
                                DoCastAOE(SPELL_PSYCHIC_SCREAM);
                            events.ScheduleEvent(EVENT_DPS_PSYCHIC_SCREAM, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        default:
                            return;
                    }
                }
                DoSpellAttackIfReady(RAID_MODE(SPELL_MIND_FLAY_10N, SPELL_MIND_FLAY_25N, SPELL_MIND_FLAY_10H, SPELL_MIND_FLAY_25H));
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_trial_of_the_crusader_shadow_priestAI (creature);
        }
};

class npc_trial_of_the_crusader_warlock : public CreatureScript
{
    public:
        npc_trial_of_the_crusader_warlock() : CreatureScript("npc_trial_of_the_crusader_warlock") { }

        struct npc_trial_of_the_crusader_warlockAI : public boss_faction_championsAI
        {
            npc_trial_of_the_crusader_warlockAI(Creature* creature) : boss_faction_championsAI(creature, AI_RANGED) {}

            void Reset()
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_HELLFIRE, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_CORRUPTION, urand(2*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_CURSE_OF_AGONY, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_CURSE_OF_EXHAUSTION, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FEAR, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SEARING_PAIN, urand(5*IN_MILLISECONDS, 12*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_UNSTABLE_AFFLICTION, urand(7*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                SetEquipmentSlots(false, 49992, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            }

            void EnterToBattle(Unit* who)
            {
                boss_faction_championsAI::EnterToBattle(who);
                DoCast(SPELL_SUMMON_FELHUNTER);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_HELLFIRE:
                            if (EnemiesInRange(10.0f) >= 2)
								DoCastAOE(RAID_MODE(SPELL_HELLFIRE_10N, SPELL_HELLFIRE_25N, SPELL_HELLFIRE_10H, SPELL_HELLFIRE_25H));

                            events.ScheduleEvent(EVENT_HELLFIRE, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_CORRUPTION:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
								DoCast(target, RAID_MODE(SPELL_CORRUPTION_10N, SPELL_CORRUPTION_25N, SPELL_CORRUPTION_10H, SPELL_CORRUPTION_25H));

                            events.ScheduleEvent(EVENT_CORRUPTION, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_CURSE_OF_AGONY:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                                DoCast(target, RAID_MODE(SPELL_CURSE_OF_AGONY_10N, SPELL_CURSE_OF_AGONY_25N, SPELL_CURSE_OF_AGONY_10H, SPELL_CURSE_OF_AGONY_25H));

                            events.ScheduleEvent(EVENT_CURSE_OF_AGONY, urand(20*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_CURSE_OF_EXHAUSTION:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                                DoCast(target, SPELL_CURSE_OF_EXHAUSTION);

                            events.ScheduleEvent(EVENT_CURSE_OF_EXHAUSTION, urand(20*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_FEAR:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                                DoCast(target, SPELL_FEAR);

                            events.ScheduleEvent(EVENT_FEAR, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_SEARING_PAIN:
							DoCastVictim(RAID_MODE(SPELL_SEARING_PAIN_10N, SPELL_SEARING_PAIN_25N, SPELL_SEARING_PAIN_10H, SPELL_SEARING_PAIN_25H));
                            events.ScheduleEvent(EVENT_SEARING_PAIN, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_UNSTABLE_AFFLICTION:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
								DoCast(target, RAID_MODE(SPELL_UNSTABLE_AFFLICTION_10N, SPELL_UNSTABLE_AFFLICTION_25N, SPELL_UNSTABLE_AFFLICTION_10H, SPELL_UNSTABLE_AFFLICTION_25H));

                            events.ScheduleEvent(EVENT_UNSTABLE_AFFLICTION, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        default:
                            return;
                    }
                }
                DoSpellAttackIfReady(RAID_MODE(SPELL_SHADOW_BOLT_10N, SPELL_SHADOW_BOLT_25N, SPELL_SHADOW_BOLT_10H, SPELL_SHADOW_BOLT_25H));
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_trial_of_the_crusader_warlockAI (creature);
        }
};

class npc_trial_of_the_crusader_mage : public CreatureScript
{
    public:
        npc_trial_of_the_crusader_mage() : CreatureScript("npc_trial_of_the_crusader_mage") { }

        struct npc_trial_of_the_crusader_mageAI : public boss_faction_championsAI
        {
            npc_trial_of_the_crusader_mageAI(Creature* creature) : boss_faction_championsAI(creature, AI_RANGED) {}

            void Reset()
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_ARCANE_BARRAGE, urand(1*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_ARCANE_BLAST, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_ARCANE_EXPLOSION, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_BLINK, urand(15*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_COUNTERSPELL, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FROST_NOVA, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_ICE_BLOCK, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_POLYMORPH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                SetEquipmentSlots(false, 47524, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ARCANE_BARRAGE:
                            DoCastVictim(RAID_MODE(SPELL_ARCANE_BARRAGE_10N, SPELL_ARCANE_BARRAGE_25N, SPELL_ARCANE_BARRAGE_10H, SPELL_ARCANE_BARRAGE_25H));
                            events.ScheduleEvent(EVENT_ARCANE_BARRAGE, urand(5*IN_MILLISECONDS, 7*IN_MILLISECONDS));
                            return;
                        case EVENT_ARCANE_BLAST:
							DoCastVictim(RAID_MODE(SPELL_ARCANE_BLAST_10N, SPELL_ARCANE_BLAST_25N, SPELL_ARCANE_BLAST_10H, SPELL_ARCANE_BLAST_25H));
                            events.ScheduleEvent(EVENT_ARCANE_BLAST, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_ARCANE_EXPLOSION:
                            if (EnemiesInRange(10.0f) >= 2)
								DoCastAOE(RAID_MODE(SPELL_ARCANE_EXPLOSION_10N, SPELL_ARCANE_EXPLOSION_25N, SPELL_ARCANE_EXPLOSION_10H, SPELL_ARCANE_EXPLOSION_25H));

                            events.ScheduleEvent(EVENT_ARCANE_EXPLOSION, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_BLINK:
                            if (EnemiesInRange(10.0f) >= 2)
                                DoCast(SPELL_BLINK);
                            events.ScheduleEvent(EVENT_BLINK, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_COUNTERSPELL:
                            if (Unit* target = SelectEnemyCaster(true))
                                DoCast(target, SPELL_COUNTERSPELL);

                            events.ScheduleEvent(EVENT_COUNTERSPELL, 24*IN_MILLISECONDS);
                            return;
                        case EVENT_FROST_NOVA:
                            if (EnemiesInRange(10.0f) >= 2)
                                DoCastAOE(SPELL_FROST_NOVA);
                            events.ScheduleEvent(EVENT_FROST_NOVA, 25*IN_MILLISECONDS);
                            return;
                        case EVENT_ICE_BLOCK:
                            if (HealthBelowPct(HEALTH_PERCENT_30))
                            {
                                DoCast(SPELL_ICE_BLOCK);
                                events.RescheduleEvent(EVENT_ICE_BLOCK, 5*MINUTE*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_ICE_BLOCK, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_POLYMORPH:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_POLYMORPH);

                            events.ScheduleEvent(EVENT_POLYMORPH, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        default:
                            return;
                    }
                }
                DoSpellAttackIfReady(RAID_MODE(SPELL_FROSTBOLT_10N, SPELL_FROSTBOLT_25N, SPELL_FROSTBOLT_10H, SPELL_FROSTBOLT_25H));
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_trial_of_the_crusader_mageAI (creature);
        }
};

class npc_trial_of_the_crusader_hunter : public CreatureScript
{
    public:
        npc_trial_of_the_crusader_hunter() : CreatureScript("npc_trial_of_the_crusader_hunter") { }

        struct npc_trial_of_the_crusader_hunterAI : public boss_faction_championsAI
        {
            npc_trial_of_the_crusader_hunterAI(Creature* creature) : boss_faction_championsAI(creature, AI_RANGED) {}

            void Reset()
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_AIMED_SHOT, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DETERRENCE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DISENGAGE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_EXPLOSIVE_SHOT, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FROST_TRAP, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_STEADY_SHOT, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_WING_CLIP, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_WYVERN_STING, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                SetEquipmentSlots(false, 47156, EQUIP_NO_CHANGE, 48711);
            }

            void EnterToBattle(Unit* who)
            {
                boss_faction_championsAI::EnterToBattle(who);
                DoCast(SPELL_CALL_PET);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_AIMED_SHOT:
							DoCastVictim(RAID_MODE(SPELL_AIMED_SHOT_10N, SPELL_AIMED_SHOT_25N, SPELL_AIMED_SHOT_10H, SPELL_AIMED_SHOT_25H));
                            events.ScheduleEvent(EVENT_AIMED_SHOT, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_DETERRENCE:
                            if (HealthBelowPct(HEALTH_PERCENT_30))
                            {
                                DoCast(SPELL_DETERRENCE);
                                events.RescheduleEvent(EVENT_DETERRENCE, 150*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_DETERRENCE, 10*IN_MILLISECONDS);
                            return;
                        case EVENT_DISENGAGE:
                            if (EnemiesInRange(10.0f) >= 2)
                                DoCast(SPELL_DISENGAGE);

                            events.ScheduleEvent(EVENT_DISENGAGE, 30*IN_MILLISECONDS);
                            return;
                        case EVENT_EXPLOSIVE_SHOT:
							DoCastVictim(RAID_MODE(SPELL_EXPLOSIVE_SHOT_10N, SPELL_EXPLOSIVE_SHOT_25N, SPELL_EXPLOSIVE_SHOT_10H, SPELL_EXPLOSIVE_SHOT_25H));
                            events.ScheduleEvent(EVENT_EXPLOSIVE_SHOT, urand(6*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        case EVENT_FROST_TRAP:
                            if (EnemiesInRange(10.0f) >= 2)
                                DoCastAOE(SPELL_FROST_TRAP);

                            events.ScheduleEvent(EVENT_FROST_TRAP, 30*IN_MILLISECONDS);
                            return;
                        case EVENT_STEADY_SHOT:
                            DoCastVictim(SPELL_STEADY_SHOT);
                            events.ScheduleEvent(EVENT_STEADY_SHOT, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_WING_CLIP:
							if (Unit* target = me->GetVictim())
							{
								if (me->GetDistance2d(target) < 6.0f)
									DoCast(target, SPELL_WING_CLIP);
							}
                            events.ScheduleEvent(EVENT_WING_CLIP, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_WYVERN_STING:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_WYVERN_STING);

                            events.ScheduleEvent(EVENT_WYVERN_STING, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        default:
                            return;
                    }
                }
                DoSpellAttackIfReady(RAID_MODE(SPELL_SHOOT_10N, SPELL_SHOOT_25N, SPELL_SHOOT_10H, SPELL_SHOOT_25H));
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_trial_of_the_crusader_hunterAI (creature);
        }
};

class npc_trial_of_the_crusader_moonkin_druid : public CreatureScript
{
    public:
        npc_trial_of_the_crusader_moonkin_druid() : CreatureScript("npc_trial_of_the_crusader_moonkin_druid") { }

        struct npc_trial_of_the_crusader_moonkin_druidAI : public boss_faction_championsAI
        {
            npc_trial_of_the_crusader_moonkin_druidAI(Creature* creature) : boss_faction_championsAI(creature, AI_RANGED) {}

            void Reset()
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_CYCLONE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_ENTANGLING_ROOTS, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FAERIE_FIRE, urand(2*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FORCE_OF_NATURE, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_INSECT_SWARM, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_MOONFIRE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_STARFIRE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DPS_BARKSKIN, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));

                SetEquipmentSlots(false, 50966, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CYCLONE:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_CYCLONE);

                            events.ScheduleEvent(EVENT_CYCLONE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_ENTANGLING_ROOTS:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                                DoCast(target, SPELL_ENTANGLING_ROOTS);

                            events.ScheduleEvent(EVENT_ENTANGLING_ROOTS, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_FAERIE_FIRE:
                            DoCastVictim(SPELL_FAERIE_FIRE);
                            events.ScheduleEvent(EVENT_FAERIE_FIRE, urand(30*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                            return;
                        case EVENT_FORCE_OF_NATURE:
                            DoCastVictim(SPELL_FORCE_OF_NATURE);
                            events.ScheduleEvent(EVENT_FORCE_OF_NATURE, 180*IN_MILLISECONDS);
                            return;
                        case EVENT_INSECT_SWARM:
							DoCastVictim(RAID_MODE(SPELL_INSECT_SWARM_10N, SPELL_INSECT_SWARM_25N, SPELL_INSECT_SWARM_10H, SPELL_INSECT_SWARM_25H));
                            events.ScheduleEvent(EVENT_INSECT_SWARM, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_MOONFIRE:
							DoCastVictim(RAID_MODE(SPELL_MOONFIRE_10N, SPELL_MOONFIRE_25N, SPELL_MOONFIRE_10H, SPELL_MOONFIRE_25H));
                            events.ScheduleEvent(EVENT_MOONFIRE, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_STARFIRE:
							DoCastVictim(RAID_MODE(SPELL_STARFIRE_10N, SPELL_STARFIRE_25N, SPELL_STARFIRE_10H, SPELL_STARFIRE_25H));
                            events.ScheduleEvent(EVENT_STARFIRE, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_DPS_BARKSKIN:
                            if (HealthBelowPct(HEALTH_PERCENT_30))
                            {
                                DoCast(me, SPELL_BARKSKIN);
                                events.RescheduleEvent(EVENT_DPS_BARKSKIN, 60*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_DPS_BARKSKIN, 5*IN_MILLISECONDS);
                            return;
                        default:
                            return;
                    }
                }
                DoSpellAttackIfReady(RAID_MODE(SPELL_WRATH_10N, SPELL_WRATH_25N, SPELL_WRATH_10H, SPELL_WRATH_25H));
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_trial_of_the_crusader_moonkin_druidAI (creature);
        }
};

/********************************************************************
                            MELEE
********************************************************************/

class npc_trial_of_the_crusader_warrior : public CreatureScript
{
    public:
        npc_trial_of_the_crusader_warrior() : CreatureScript("npc_trial_of_the_crusader_warrior") { }

        struct npc_trial_of_the_crusader_warriorAI : public boss_faction_championsAI
        {
            npc_trial_of_the_crusader_warriorAI(Creature* creature) : boss_faction_championsAI(creature, AI_MELEE) {}

            void Reset()
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_BLADESTORM, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_INTIMIDATING_SHOUT, urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_MORTAL_STRIKE, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_WARR_CHARGE, 1*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_DISARM, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_OVERPOWER, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SUNDER_ARMOR, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SHATTERING_THROW, urand(20*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_RETALIATION, urand(5*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                SetEquipmentSlots(false, 47427, 46964, EQUIP_NO_CHANGE);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BLADESTORM:
                            DoCastAOE(SPELL_BLADESTORM);
                            events.ScheduleEvent(EVENT_BLADESTORM, 150*IN_MILLISECONDS);
                            return;
                        case EVENT_INTIMIDATING_SHOUT:
                            DoCastAOE(SPELL_INTIMIDATING_SHOUT);
                            events.ScheduleEvent(EVENT_INTIMIDATING_SHOUT, 120*IN_MILLISECONDS);
                            return;
                        case EVENT_MORTAL_STRIKE:
							DoCastVictim(RAID_MODE(SPELL_MORTAL_STRIKE_10N, SPELL_MORTAL_STRIKE_25N, SPELL_MORTAL_STRIKE_10H, SPELL_MORTAL_STRIKE_25H));
                            events.ScheduleEvent(EVENT_MORTAL_STRIKE, urand(10*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_WARR_CHARGE:
							if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
								DoCast(target, RAID_MODE(SPELL_CHARGE_10N, SPELL_CHARGE_25N, SPELL_CHARGE_10H, SPELL_CHARGE_25H));
                            events.ScheduleEvent(EVENT_WARR_CHARGE, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_DISARM:
                            DoCastVictim(SPELL_DISARM);
                            events.ScheduleEvent(EVENT_DISARM, urand(15*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                            return;
                        case EVENT_OVERPOWER:
                            DoCastVictim(SPELL_OVERPOWER);
                            events.ScheduleEvent(EVENT_OVERPOWER, urand(20*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                            return;
                        case EVENT_SUNDER_ARMOR:
                            DoCastVictim(SPELL_SUNDER_ARMOR);
                            events.ScheduleEvent(EVENT_SUNDER_ARMOR, urand(2*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                            return;
						case EVENT_SHATTERING_THROW:
                            if (Unit* target = me->GetVictim())
                            {
                                if (target->HasAuraWithMechanic(1 << MECHANIC_IMMUNE_SHIELD))
                                {
                                    DoCast(target, SPELL_SHATTERING_THROW);
                                    events.RescheduleEvent(EVENT_SHATTERING_THROW, 5*MINUTE*IN_MILLISECONDS);
                                    return;
                                }
                            }
                            events.RescheduleEvent(EVENT_SHATTERING_THROW, 3*IN_MILLISECONDS);
                            return;
                        case EVENT_RETALIATION:
                            if (HealthBelowPct(HEALTH_PERCENT_50))
                            {
                                DoCast(SPELL_RETALIATION);
                                events.RescheduleEvent(EVENT_RETALIATION, 5*MINUTE*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_RETALIATION, 5*IN_MILLISECONDS);
                            return;
                        default:
                            return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_trial_of_the_crusader_warriorAI (creature);
        }
};

class npc_trial_of_the_crusader_death_knight : public CreatureScript
{
    public:
        npc_trial_of_the_crusader_death_knight() : CreatureScript("npc_trial_of_the_crusader_death_knight") { }

        struct npc_trial_of_the_crusader_death_knightAI : public boss_faction_championsAI
        {
            npc_trial_of_the_crusader_death_knightAI(Creature* creature) : boss_faction_championsAI(creature, AI_MELEE) {}

            void Reset()
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_CHAINS_OF_ICE, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DEATH_COIL, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DEATH_GRIP, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_FROST_STRIKE, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_ICEBOUND_FORTITUDE, urand(25*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_ICY_TOUCH, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_STRANGULATE, urand(5*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                SetEquipmentSlots(false, 47518, 51021, EQUIP_NO_CHANGE);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHAINS_OF_ICE:
                            DoCastVictim(SPELL_CHAINS_OF_ICE);
                            events.ScheduleEvent(EVENT_CHAINS_OF_ICE, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            return;
                        case EVENT_DEATH_COIL:
							DoCastVictim(RAID_MODE(SPELL_DEATH_COIL_10N, SPELL_DEATH_COIL_25N, SPELL_DEATH_COIL_10H, SPELL_DEATH_COIL_25H));
                            events.ScheduleEvent(EVENT_DEATH_COIL, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
						case EVENT_DEATH_GRIP:
                            if (Unit* target = me->GetVictim())
                            {
                                if (me->IsInRange(target, 5.0f, 30.0f, false))
                                {
									DoCast(target, RAID_MODE(SPELL_DEATH_GRIP_10N, SPELL_DEATH_GRIP_25N, SPELL_DEATH_GRIP_10H, SPELL_DEATH_GRIP_25H));
                                    events.RescheduleEvent(EVENT_DEATH_GRIP, 35*IN_MILLISECONDS);
                                    return;
                                }
                            }
                            events.RescheduleEvent(EVENT_DEATH_GRIP, 3*IN_MILLISECONDS);
                            return;
                        case EVENT_FROST_STRIKE:
							DoCastVictim(RAID_MODE(SPELL_FROST_STRIKE_10N, SPELL_FROST_STRIKE_25N, SPELL_FROST_STRIKE_10H, SPELL_FROST_STRIKE_25H));
                            events.ScheduleEvent(EVENT_FROST_STRIKE, urand(6*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        case EVENT_ICEBOUND_FORTITUDE:
                            if (HealthBelowPct(HEALTH_PERCENT_50))
                            {
                                DoCast(SPELL_ICEBOUND_FORTITUDE);
                                events.RescheduleEvent(EVENT_ICEBOUND_FORTITUDE, 60*IN_MILLISECONDS);
                            }
                            else
								events.RescheduleEvent(EVENT_ICEBOUND_FORTITUDE, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_ICY_TOUCH:
							if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
								DoCast(target, RAID_MODE(SPELL_ICY_TOUCH_10N, SPELL_ICY_TOUCH_25N, SPELL_ICY_TOUCH_10H, SPELL_ICY_TOUCH_25H));

							events.ScheduleEvent(EVENT_ICY_TOUCH, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
						case EVENT_STRANGULATE:
                            if (Unit* target = SelectEnemyCaster(false))
                            {
                                DoCast(target, SPELL_STRANGULATE);
                                events.RescheduleEvent(EVENT_STRANGULATE, 120*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_STRANGULATE, 5*IN_MILLISECONDS);
                            return;
                        default:
                            return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_trial_of_the_crusader_death_knightAI (creature);
        }
};

class npc_trial_of_the_crusader_rogue : public CreatureScript
{
    public:
        npc_trial_of_the_crusader_rogue() : CreatureScript("npc_trial_of_the_crusader_rogue") { }

        struct npc_trial_of_the_crusader_rogueAI : public boss_faction_championsAI
        {
            npc_trial_of_the_crusader_rogueAI(Creature* creature) : boss_faction_championsAI(creature, AI_MELEE) {}

            void Reset()
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_FAN_OF_KNIVES, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_BLIND, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_CLOAK, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_BLADE_FLURRY, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SHADOWSTEP, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HEMORRHAGE, urand(3*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_EVISCERATE, urand(20*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_WOUND_POISON, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                SetEquipmentSlots(false, 47422, 49982, EQUIP_NO_CHANGE);
                me->SetPowerType(POWER_ENERGY);
                me->SetMaxPower(POWER_ENERGY, POWER_QUANTITY_MAX);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FAN_OF_KNIVES:
                            if (EnemiesInRange(10.0f) >= 2)
								DoCastAOE(RAID_MODE(SPELL_FAN_OF_KNIVES_10N, SPELL_FAN_OF_KNIVES_25N, SPELL_FAN_OF_KNIVES_10H, SPELL_FAN_OF_KNIVES_25H));

                            events.ScheduleEvent(EVENT_FAN_OF_KNIVES, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_BLIND:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_BLIND);

                            events.ScheduleEvent(EVENT_BLIND, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            return;
                        case EVENT_CLOAK:
                            if (HealthBelowPct(HEALTH_PERCENT_50))
                            {
                                DoCast(SPELL_CLOAK);
                                events.RescheduleEvent(EVENT_CLOAK, 90*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_CLOAK, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_BLADE_FLURRY:
                            if (EnemiesInRange(10.0f) >= 2)
                            {
                                DoCast(SPELL_BLADE_FLURRY);
                                events.RescheduleEvent(EVENT_BLADE_FLURRY, 120*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_BLADE_FLURRY, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_SHADOWSTEP:
							if (Unit* target = me->GetVictim())
                            {
								if (me->IsInRange(target, 10.0f, 40.0f, false))
								{
									DoCast(target, RAID_MODE(SPELL_SHADOWSTEP_10N, SPELL_SHADOWSTEP_25N, SPELL_SHADOWSTEP_10H, SPELL_SHADOWSTEP_25H));
									events.RescheduleEvent(EVENT_SHADOWSTEP, 30*IN_MILLISECONDS);
								}
                            }
							events.RescheduleEvent(EVENT_SHADOWSTEP, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_HEMORRHAGE:
                            DoCastVictim(SPELL_HEMORRHAGE);
                            events.ScheduleEvent(EVENT_HEMORRHAGE, urand(3*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        case EVENT_EVISCERATE:
							DoCastVictim(RAID_MODE(SPELL_EVISCERATE_10N, SPELL_EVISCERATE_25N, SPELL_EVISCERATE_10H, SPELL_EVISCERATE_25H));
                            events.ScheduleEvent(EVENT_EVISCERATE, urand(30*IN_MILLISECONDS, 40*IN_MILLISECONDS));
                            return;
                        case EVENT_WOUND_POISON:
                            DoCastVictim(SPELL_WOUND_POISON);
                            events.ScheduleEvent(EVENT_WOUND_POISON, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        default:
                            return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_trial_of_the_crusader_rogueAI (creature);
        }
};

class npc_trial_of_the_crusader_enhancement_shaman : public CreatureScript
{
    public:
        npc_trial_of_the_crusader_enhancement_shaman() : CreatureScript("npc_trial_of_the_crusader_enhancement_shaman") { }

        struct npc_trial_of_the_crusader_enhancement_shamanAI : public boss_faction_championsAI
        {
            npc_trial_of_the_crusader_enhancement_shamanAI(Creature* creature) : boss_faction_championsAI(creature, AI_MELEE) {}

            void Reset()
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_DPS_EARTH_SHOCK, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_LAVA_LASH, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_STORMSTRIKE, urand(2*IN_MILLISECONDS, 5*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DPS_BLOODLUST_HEROISM, 20*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_DEPLOY_TOTEM, 1*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_WINDFURY, urand(20*IN_MILLISECONDS, 50*IN_MILLISECONDS));

                _totemCount = 0;
                _totemOldCenterX = me->GetPositionX();
                _totemOldCenterY = me->GetPositionY();
                SetEquipmentSlots(false, 51803, 48013, EQUIP_NO_CHANGE);
                summons.DespawnAll();
            }

            void JustSummoned(Creature* summoned)
            {
                summons.Summon(summoned);
            }

            void SummonedCreatureDespawn(Creature* /*summoned*/)
            {
                --_totemCount;
            }

            void DeployTotem()
            {
                _totemCount = 4;
                _totemOldCenterX = me->GetPositionX();
                _totemOldCenterY = me->GetPositionY();
                /*
                -Windfury (16% melee haste)
                -Grounding (redirects one harmful magic spell to the totem)

                -Healing Stream (unable to find amount of healing in our logs)

                -Tremor (prevents fear effects)
                -Strength of Earth (155 strength and agil for the opposing team)

                -Searing (average ~3500 damage on a random target every ~3.5 seconds)
                */
            }

            void JustDied(Unit* killer)
            {
                boss_faction_championsAI::JustDied(killer);
                summons.DespawnAll();
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DPS_EARTH_SHOCK:
                            if (Unit* target = SelectEnemyCaster(true))
								DoCast(target, RAID_MODE(SPELL_EARTH_SHOCK_ENH_10N, SPELL_EARTH_SHOCK_ENH_25N, SPELL_EARTH_SHOCK_ENH_10H, SPELL_EARTH_SHOCK_ENH_25H));

                            events.ScheduleEvent(EVENT_DPS_EARTH_SHOCK, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_LAVA_LASH:
                            DoCastVictim(SPELL_LAVA_LASH);
                            events.ScheduleEvent(EVENT_LAVA_LASH, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_STORMSTRIKE:
                            DoCastVictim(SPELL_STORMSTRIKE);
                            events.ScheduleEvent(EVENT_STORMSTRIKE, urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        case EVENT_DPS_BLOODLUST_HEROISM:
                            if (me->GetFaction()) //Am i alliance?
                            {
                                if (!me->HasAura(AURA_EXHAUSTION))
                                    DoCastAOE(SPELL_HEROISM);
                            }
                            else
                            {
                                if (!me->HasAura(AURA_SATED))
                                    DoCastAOE(SPELL_BLOODLUST);
                            }
                            events.ScheduleEvent(EVENT_DPS_BLOODLUST_HEROISM, 5*MINUTE*IN_MILLISECONDS);
                            return;
                        case EVENT_DEPLOY_TOTEM:
                            if (_totemCount < 4 || me->GetDistance2d(_totemOldCenterX, _totemOldCenterY) > 20.0f)
                                DeployTotem();
                            events.ScheduleEvent(EVENT_DEPLOY_TOTEM, 1*IN_MILLISECONDS);
                            return;
                        case EVENT_WINDFURY:
                            DoCastVictim(SPELL_WINDFURY);
                            events.ScheduleEvent(EVENT_WINDFURY, urand(20*IN_MILLISECONDS, 60*IN_MILLISECONDS));
                            return;
                        default:
                            return;
                    }
                }
            }
            private:
                uint8  _totemCount;
                float  _totemOldCenterX, _totemOldCenterY;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_trial_of_the_crusader_enhancement_shamanAI (creature);
        }
};

class npc_trial_of_the_crusader_retribution_paladin : public CreatureScript
{
    public:
        npc_trial_of_the_crusader_retribution_paladin() : CreatureScript("npc_trial_of_the_crusader_retribution_paladin") { }

        struct npc_trial_of_the_crusader_retribution_paladinAI : public boss_faction_championsAI
        {
            npc_trial_of_the_crusader_retribution_paladinAI(Creature* creature) : boss_faction_championsAI(creature, AI_MELEE) {}

            void Reset()
            {
                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_AVENGING_WRATH, urand(25*IN_MILLISECONDS, 35*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_CRUSADER_STRIKE, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DIVINE_STORM, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_HAMMER_OF_JUSTICE_RET, urand(10*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_JUDGEMENT_OF_COMMAND, urand(5*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_REPENTANCE, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DPS_HAND_OF_PROTECTION, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_DPS_DIVINE_SHIELD, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                SetEquipmentSlots(false, 47519, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            }

            void EnterToBattle(Unit* who)
            {
                boss_faction_championsAI::EnterToBattle(who);
				DoCast(RAID_MODE(SPELL_SEAL_OF_COMMAND_10N, SPELL_SEAL_OF_COMMAND_25N, SPELL_SEAL_OF_COMMAND_10H, SPELL_SEAL_OF_COMMAND_25H));
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_AVENGING_WRATH:
                            DoCast(SPELL_AVENGING_WRATH);
                            events.ScheduleEvent(EVENT_AVENGING_WRATH, 180*IN_MILLISECONDS);
                            return;
                        case EVENT_CRUSADER_STRIKE:
                            DoCastVictim(SPELL_CRUSADER_STRIKE);
                            events.ScheduleEvent(EVENT_CRUSADER_STRIKE, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_DIVINE_STORM:
                            if (EnemiesInRange(10.0f) >= 2)
                                DoCast(SPELL_DIVINE_STORM);
                            events.ScheduleEvent(EVENT_DIVINE_STORM, urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS));
                            return;
                        case EVENT_HAMMER_OF_JUSTICE_RET:
                            DoCastVictim(SPELL_HAMMER_OF_JUSTICE_RETRIBUTION);
                            events.ScheduleEvent(EVENT_HAMMER_OF_JUSTICE_RET, 40*IN_MILLISECONDS);
                            return;
                        case EVENT_JUDGEMENT_OF_COMMAND:
							DoCastVictim(RAID_MODE(SPELL_JUDGEMENT_OF_COMMAND_10N, SPELL_JUDGEMENT_OF_COMMAND_25N, SPELL_JUDGEMENT_OF_COMMAND_10H, SPELL_JUDGEMENT_OF_COMMAND_25H));
                            events.ScheduleEvent(EVENT_JUDGEMENT_OF_COMMAND, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            return;
                        case EVENT_REPENTANCE:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                                DoCast(target, SPELL_REPENTANCE);

                            events.ScheduleEvent(EVENT_REPENTANCE, 60*IN_MILLISECONDS);
                            return;
                        case EVENT_DPS_HAND_OF_PROTECTION:
                            if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_LOW))
                            {
                                if (!target->HasAura(SPELL_FORBEARANCE))
                                {
                                    DoCast(target, SPELL_HAND_OF_PROTECTION);
                                    events.RescheduleEvent(EVENT_DPS_HAND_OF_PROTECTION, 5*MINUTE*IN_MILLISECONDS);
                                }
                                else
                                    events.RescheduleEvent(EVENT_DPS_HAND_OF_PROTECTION, 5*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_DPS_HAND_OF_PROTECTION, 5*IN_MILLISECONDS);
                            return;
                        case EVENT_DPS_DIVINE_SHIELD:
                            if (HealthBelowPct(HEALTH_PERCENT_30) && !me->HasAura(SPELL_FORBEARANCE))
                            {
                                DoCast(me, SPELL_DIVINE_SHIELD);
                                events.RescheduleEvent(EVENT_DPS_DIVINE_SHIELD, 5*MINUTE*IN_MILLISECONDS);
                            }
                            else
                                events.RescheduleEvent(EVENT_DPS_DIVINE_SHIELD, 5*IN_MILLISECONDS);
                            return;
                        default:
                            return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_trial_of_the_crusader_retribution_paladinAI (creature);
        }
};

class npc_trial_of_the_crusader_pet_warlock : public CreatureScript
{
    public:
        npc_trial_of_the_crusader_pet_warlock() : CreatureScript("npc_trial_of_the_crusader_pet_warlock") { }

        struct npc_trial_of_the_crusader_pet_warlockAI : public boss_faction_championsAI
        {
            npc_trial_of_the_crusader_pet_warlockAI(Creature* creature) : boss_faction_championsAI(creature, AI_PET) {}

            void Reset()
            {
				DoZoneInCombat();

                boss_faction_championsAI::Reset();
                events.ScheduleEvent(EVENT_DEVOUR_MAGIC, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_SPELL_LOCK, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);
                boss_faction_championsAI::UpdateAI(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DEVOUR_MAGIC:
                            DoCastVictim(SPELL_DEVOUR_MAGIC);
                            events.ScheduleEvent(EVENT_DEVOUR_MAGIC, urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS));
                            return;
                        case EVENT_SPELL_LOCK:
							if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
							{
								if (target && target->GetPowerType() == POWER_MANA && target->HasUnitState(UNIT_STATE_CASTING))
								{
									DoCast(target, SPELL_SPELL_LOCK);
									events.ScheduleEvent(EVENT_SPELL_LOCK, urand(24*IN_MILLISECONDS, 30*IN_MILLISECONDS));
								}
							}
                            return;
                        default:
                            return;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_trial_of_the_crusader_pet_warlockAI (creature);
        }
};

class npc_trial_of_the_crusader_pet_hunter : public CreatureScript
{
    public:
        npc_trial_of_the_crusader_pet_hunter() : CreatureScript("npc_trial_of_the_crusader_pet_hunter") { }

        struct npc_trial_of_the_crusader_pet_hunterAI : public boss_faction_championsAI
        {
            npc_trial_of_the_crusader_pet_hunterAI(Creature* creature) : boss_faction_championsAI(creature, AI_PET) {}

			uint32 ClawTimer;

            void Reset()
            {
				DoZoneInCombat();

                boss_faction_championsAI::Reset();
                ClawTimer = urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                boss_faction_championsAI::UpdateAI(diff);

                if (ClawTimer <= diff)
                {
					DoCastVictim(RAID_MODE(SPELL_CLAW_10N, SPELL_CLAW_25N, SPELL_CLAW_10H, SPELL_CLAW_25H));
                    ClawTimer = urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS);
                }
                else ClawTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_trial_of_the_crusader_pet_hunterAI (creature);
        }
};

class spell_faction_champion_warl_unstable_affliction : public SpellScriptLoader
{
    public:
        spell_faction_champion_warl_unstable_affliction() : SpellScriptLoader("spell_faction_champion_warl_unstable_affliction") { }

        class spell_faction_champion_warl_unstable_affliction_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_faction_champion_warl_unstable_affliction_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_UNSTABLE_AFFLICTION_DISPEL_10N))
                    return false;
				else if (!sSpellMgr->GetSpellInfo(SPELL_UNSTABLE_AFFLICTION_DISPEL_25N))
                    return false;
				else if (!sSpellMgr->GetSpellInfo(SPELL_UNSTABLE_AFFLICTION_DISPEL_10H))
                    return false;
				else if (!sSpellMgr->GetSpellInfo(SPELL_UNSTABLE_AFFLICTION_DISPEL_25H))
                    return false;
                return true;
            }

            void HandleDispel(DispelInfo* dispelInfo)
            {
                if (Unit* caster = GetCaster())
				{
                    caster->CastSpell(dispelInfo->GetDispeller(), SPELL_UNSTABLE_AFFLICTION_DISPEL_10N, true, 0, GetEffect(EFFECT_0));
					caster->CastSpell(dispelInfo->GetDispeller(), SPELL_UNSTABLE_AFFLICTION_DISPEL_25N, true, 0, GetEffect(EFFECT_0));
					caster->CastSpell(dispelInfo->GetDispeller(), SPELL_UNSTABLE_AFFLICTION_DISPEL_10H, true, 0, GetEffect(EFFECT_0));
					caster->CastSpell(dispelInfo->GetDispeller(), SPELL_UNSTABLE_AFFLICTION_DISPEL_25H, true, 0, GetEffect(EFFECT_0));
				}
            }

            void Register()
            {
                AfterDispel += AuraDispelFn(spell_faction_champion_warl_unstable_affliction_AuraScript::HandleDispel);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_faction_champion_warl_unstable_affliction_AuraScript();
        }
};

class spell_faction_champion_death_grip : public SpellScriptLoader
{
    public:
        spell_faction_champion_death_grip() : SpellScriptLoader("spell_faction_champion_death_grip") { }

        class spell_faction_champion_death_grip_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_faction_champion_death_grip_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DEATH_GRIP_PULL))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    if (Unit* caster = GetCaster())
                        target->CastSpell(caster, SPELL_DEATH_GRIP_PULL);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_faction_champion_death_grip_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }

        };

        SpellScript* GetSpellScript() const
        {
            return new spell_faction_champion_death_grip_SpellScript();
        }
};

class spell_toc_bloodlust : public SpellScriptLoader
{
    public:
        spell_toc_bloodlust() : SpellScriptLoader("spell_toc_bloodlust") { }

        class spell_toc_bloodlust_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_toc_bloodlust_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(AURA_SATED))
                    return false;
                return true;
            }

			void RemoveInvalidTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if(Trinity::UnitAuraCheck(true, AURA_SATED));
            }

            void ApplyDebuff()
            {
                if (Unit* target = GetHitUnit())
                    target->CastSpell(target, AURA_SATED, true);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_toc_bloodlust_SpellScript::RemoveInvalidTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_toc_bloodlust_SpellScript::RemoveInvalidTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
                AfterHit += SpellHitFn(spell_toc_bloodlust_SpellScript::ApplyDebuff);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_toc_bloodlust_SpellScript();
        }
};

class spell_toc_heroism : public SpellScriptLoader
{
    public:
        spell_toc_heroism() : SpellScriptLoader("spell_toc_heroism") { }

        class spell_toc_heroism_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_toc_heroism_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(AURA_EXHAUSTION))
                    return false;
                return true;
            }

			void RemoveInvalidTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if(Trinity::UnitAuraCheck(true, AURA_EXHAUSTION));
            }

            void ApplyDebuff()
            {
                if (Unit* target = GetHitUnit())
                    target->CastSpell(target, AURA_EXHAUSTION, true);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_toc_heroism_SpellScript::RemoveInvalidTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_toc_heroism_SpellScript::RemoveInvalidTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
                AfterHit += SpellHitFn(spell_toc_heroism_SpellScript::ApplyDebuff);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_toc_heroism_SpellScript();
        }
};

void AddSC_boss_faction_champions()
{
    new boss_toc_champion_controller();
    new npc_trial_of_the_crusader_restor_druid();
    new npc_trial_of_the_crusader_shaman();
    new npc_trial_of_the_crusader_holy_paladin();
    new npc_trial_of_the_crusader_discipline_priest();
    new npc_trial_of_the_crusader_shadow_priest();
    new npc_trial_of_the_crusader_mage();
    new npc_trial_of_the_crusader_warlock();
    new npc_trial_of_the_crusader_hunter();
    new npc_trial_of_the_crusader_moonkin_druid();
    new npc_trial_of_the_crusader_warrior();
    new npc_trial_of_the_crusader_death_knight();
    new npc_trial_of_the_crusader_rogue();
    new npc_trial_of_the_crusader_enhancement_shaman();
    new npc_trial_of_the_crusader_retribution_paladin();
    new npc_trial_of_the_crusader_pet_warlock();
    new npc_trial_of_the_crusader_pet_hunter();
    new spell_faction_champion_warl_unstable_affliction();
    new spell_faction_champion_death_grip();
    new spell_toc_bloodlust();
    new spell_toc_heroism();
}