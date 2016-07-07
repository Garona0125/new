/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef DEF_PIT_OF_SARON_H
#define DEF_PIT_OF_SARON_H

#define PoSScriptName "instance_pit_of_saron"

enum DataTypes
{
    // Encounter states and GUIDs
    DATA_GARFROST               = 0,
    DATA_ICK                    = 1,
    DATA_TYRANNUS               = 2,
    // GUIDs
    DATA_RIMEFANG               = 3,
    DATA_KRICK                  = 4,
    DATA_JAINA_SYLVANAS_1       = 5,    // GUID of either Jaina or Sylvanas part 1, depending on team, as it's the same spawn.
    DATA_JAINA_SYLVANAS_2       = 6,    // GUID of either Jaina or Sylvanas part 2, depending on team, as it's the same spawn.
    DATA_TYRANNUS_EVENT         = 7,
    DATA_TEAM_IN_INSTANCE       = 8,
    DATA_TYRANNUS_START         = 9,
    DATA_AREA_TRIGGER_YMIRJAR   = 10,
    DATA_AREA_TRIGGER_FALLEN    = 11,
    DATA_AREA_TRIGGER_ICE_CICLE = 12,
    DATA_VICTUS_OR_GORKUN_FREED = 13,
    DATA_SLAVE_OUTRO_GARFROST   = 14,
    DATA_GEIST_AMBUSHER         = 15,
    DATA_SINDRAGOSA             = 16,
};

enum CreatureIds
{
    NPC_GARFROST                                = 36494,
    NPC_KRICK                                   = 36477,
    NPC_ICK                                     = 36476,
    NPC_TYRANNUS                                = 36658,
    NPC_RIMEFANG                                = 36661,
	NPC_WRATHBONE_SIEGESMITH                    = 36907,
	NPC_SCOURGELORD_TYRANNUS_INTRO              = 36505,
	NPC_WORLD_TRIGGER                           = 22517,
	NPC_GEIST_AMBUSHER                          = 36886,
    NPC_SYLVANAS_PART1                          = 36990,
    NPC_SYLVANAS_PART2                          = 38189,
    NPC_JAINA_PART1                             = 36993,
    NPC_JAINA_PART2                             = 38188,
    NPC_KILARA                                  = 37583,
    NPC_ELANDRA                                 = 37774,
    NPC_KORALEN                                 = 37779,
    NPC_KORLAEN                                 = 37582,
    NPC_CHAMPION_1_HORDE                        = 37584,
    NPC_CHAMPION_2_HORDE                        = 37587,
    NPC_CHAMPION_3_HORDE                        = 37588,
    NPC_CHAMPION_1_ALLIANCE                     = 37496,
    NPC_CHAMPION_2_ALLIANCE                     = 37497,
	NPC_CHAMPION_3_ALLIANCE                     = 37498,
	NPC_CORRUPTED_CHAMPION                      = 36796,
    NPC_HORDE_SLAVE_1                           = 36770,
    NPC_HORDE_SLAVE_2                           = 36771,
    NPC_HORDE_SLAVE_3                           = 36772,
    NPC_HORDE_SLAVE_4                           = 36773,
    NPC_ALLIANCE_SLAVE_1                        = 36764,
    NPC_ALLIANCE_SLAVE_2                        = 36765,
    NPC_ALLIANCE_SLAVE_3                        = 36766,
    NPC_ALLIANCE_SLAVE_4                        = 36767,
    NPC_FREED_SLAVE_1_ALLIANCE                  = 37575,
    NPC_FREED_SLAVE_2_ALLIANCE                  = 37572,
    NPC_FREED_SLAVE_3_ALLIANCE                  = 37576,
    NPC_FREED_SLAVE_1_HORDE                     = 37579,
    NPC_FREED_SLAVE_2_HORDE                     = 37578,
    NPC_FREED_SLAVE_3_HORDE                     = 37577,
    NPC_RESCUED_SLAVE_ALLIANCE                  = 36888,
    NPC_RESCUED_SLAVE_HORDE                     = 36889,
    NPC_MARTIN_VICTUS_1                         = 37591,
    NPC_MARTIN_VICTUS_2                         = 37580,
    NPC_GORKUN_IRONSKULL_1                      = 37581,
    NPC_GORKUN_IRONSKULL_2                      = 37592,
    NPC_YMIRJAR_DEATHBRINGER                    = 36892,
    NPC_YMIRJAR_FLAMEBEARER                     = 36893,
    NPC_YMIRJAR_WRATBRINGER                     = 36840,
    NPC_FALLEN_WARRIOR                          = 36841,
    NPC_WRATHBONE_COLDWRAITH                    = 36842,
    NPC_WRATHBONE_REAVER                        = 37729,
    NPC_WRATHBONE_SORCERER                      = 37728,
    NPC_WRATHBONE_SKELETON                      = 36877,
    NPC_SINDRAGOSA                              = 37755,
    NPC_FORGEMASTER_STALKER                     = 36495,
    NPC_EXPLODING_ORB                           = 36610,
    NPC_ICY_BLAST                               = 36731,
};

enum Miscs
{
	SPELL_ARTHAS_PORTAL      = 51807,
	SPELL_BLINK_VISUAL       = 36937,
	SPELL_FROST_BOMB         = 70521,

	ACHIEVEMENT_DONT_LOOK_UP = 4525,

	DISPLAY_ID_PORTAL        = 17612,
	MOUNT_ID_TYRANNUS_INTRO  = 27982,
};

enum GameObjectIds
{
    GO_SARONITE_ROCK         = 196485,
    GO_ICE_WALL              = 201885,
    GO_HALLS_OF_REFLECT_PORT = 201848, // unlocked by jaina/sylvanas at last outro
};

enum Encounters
{
	MAX_ENCOUNTER = 3,
};

#endif