/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef DEF_SUNWELLPLATEAU_H
#define DEF_SUNWELLPLATEAU_H

#define DATA_GO_FORECEFIELD_COLL_1  21
#define DATA_GO_FORECEFIELD_COLL_2  22
#define DATA_GO_FIRE_BARRIER        23
#define DATA_GATE_1                 24
#define DATA_GATE_2                 25
#define DATA_GATE_3                 26
#define DATA_GATE_4                 27
#define DATA_GATE_5                 28

enum Data
{
    DATA_KALECGOS_EVENT,
    DATA_BRUTALLUS_EVENT,
    DATA_FELMYST_EVENT,
    DATA_EREDAR_TWINS_EVENT,
    DATA_MURU_EVENT,
    DATA_KILJAEDEN_EVENT,
};

enum Data64
{
    DATA_KALECGOS_DRAGON,
    DATA_KALECGOS_HUMAN,
    DATA_SATHROVARR_THE_CORRUPTOR,
    DATA_BRUTALLUS,
    DATA_MADRIGOSA,
    DATA_FELMYST,
    DATA_ALYTHESS,
    DATA_SACROLASH,
    DATA_MURU,
    DATA_KILJAEDEN,
    DATA_KILJAEDEN_CONTROLLER,
    DATA_ANVEENA,
    DATA_KALECGOS_KJ,
    /*** GameObjects ***/
    DATA_GO_FORCEFIELD,
	DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_1,
	DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_2,
	DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_3,
	DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_4,
	DATA_PLAYER_GUID,
	DATA_QUELDELAR_INTRODUCER,
};

enum Creatures
{
    NPC_MURU                                 = 25741,
    NPC_ENTROPIUS                            = 25840,
    NPC_KALECGOS_DRAGON                      = 24850,
    NPC_KALECGOS_HUMAN                       = 24891,
    NPC_SATHROVARR_THE_CORRUPTOR             = 24892,
    NPC_UNYIELDING_DEAD                      = 25268,
    NPC_FLIGHT_LEFT                          = 25357,
    NPC_FLIGHT_RIGHT                         = 25358,
    NPC_DEATH_CLOUD                          = 25703,
    NPC_VAPOR                                = 25265,
    NPC_VAPOR_TRAIL                          = 25267,
    NPC_GRAND_WARLOCK_ALYTHESS               = 25166,
    NPC_SHADOW_IMAGE                         = 25214,
    NPC_LADY_SACROLASH                       = 25165,
	NPC_FELMYST                              = 25038,
	NPC_MADRIGOSA                            = 24895,
	NPC_BRUTALLUS                            = 24882,
    NPC_ANVEENA                              = 26046,
    NPC_KALECGOS_KILJAEDEN                   = 25319,
    NPC_PROPHET_VELEN                        = 26246,
    NPC_KILJAEDEN                            = 25315,
	NPC_KILJAEDEN_CONTROLLER                 = 25608,
    NPC_HAND_OF_THE_DECEIVER                 = 25588,
    NPC_FELFIRE_PORTAL                       = 25603,
    NPC_VOLATILE_FELFIRE_FIEND               = 25598,
    NPC_ARMAGEDDON_TARGET                    = 25735,
    NPC_SHIELD_ORB                           = 25502,
    NPC_THE_CORE_OF_ENTROPIUS                = 26262,
    NPC_POWER_OF_THE_BLUE_DRAGONFLIGHT       = 25653,
    NPC_SPIKE_TARGET_1                       = 30598,
    NPC_SPIKE_TARGET_2                       = 30614,
    NPC_SINISTER_REFLECTION                  = 25708,
    NPC_DARKNESS                             = 25879,
    NPC_DARK_FIEND                           = 25744,
    NPC_BERSERKER                            = 25798,
    NPC_FURY_MAGE                            = 25799,
    NPC_VOID_SENTINEL                        = 25772,
    NPC_VOID_SPAWN                           = 25824,
    NPC_SINGULARITY                          = 25855,
	NPC_SPECTRAL_REALM                       = 25796,
	NPC_CHAMBERLAIN_GALIROS                  = 38056,
	NPC_FIRE_FIEND                           = 26101,
	NPC_SUNWELL_PROTECTOR                    = 25507,
	NPC_VOLATILE_FIEND                       = 25851,
	NPC_GRAND_MASTER_ROMMATH                 = 37763,
	NPC_LORTHEMAR_THERON                     = 37764,
	NPC_CAPTAIN_AURIC_SUNCHASER              = 37765,
	NPC_SUNWELL_CASTER_BUNNY                 = 37746,
	NPC_SUNWELL_HONOR_GUARD                  = 37781,
};

enum GameObjects
{
	GO_SPECTRAL_RIFT                = 187055,
	GO_FORCE_FIELD_1                = 188421,
	GO_BOSS_COLLISION_1             = 188523,
	GO_BOSS_COLLISION_2             = 188524,
	GO_FIRE_BARRIER                 = 188075,
	GO_MURU_GATE                    = 187990,
	GO_GATE_1                       = 187770,
	GO_GATE_2                       = 187896,
	GO_GATE_3                       = 188118,
	GO_ORB_BLUE_FLIGHT_1            = 187869,
    GO_ORB_BLUE_FLIGHT_2            = 188114,
    GO_ORB_BLUE_FLIGHT_3            = 188115,
    GO_ORB_BLUE_FLIGHT_4            = 188116,
	GO_QUEL_DELAR                   = 201794,
};

enum QuestItems
{
	ITEM_TAINTED_QUELDELAR_1 = 49879,
	ITEM_TAINTED_QUELDELAR_2 = 49889,
};

enum SunwellSpells
{
	SPELL_SUNWELL_RADIANCE  = 45769,
	SPELL_QUEL_DELAR_WRATH  = 70493,
	SPELL_ICY_PRISON        = 70540,
};

enum Encounters
{
	MAX_ENCOUNTER = 6,
};

#endif