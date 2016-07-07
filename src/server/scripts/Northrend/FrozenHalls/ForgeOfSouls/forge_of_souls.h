/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef DEF_FORGE_OF_SOULS_H
#define DEF_FORGE_OF_SOULS_H

#define FoSScriptName "instance_forge_of_souls"

enum Data
{
    DATA_BRONJAHM         = 0,
    DATA_DEVOURER         = 1,
    DATA_DEVOURER_EVENT   = 2,
    DATA_TEAM_IN_INSTANCE = 3,
	DATA_SOUL_FRAGMENTS   = 4,
};

enum Creatures
{
    NPC_BRONJAHM            = 36497,
    NPC_DEVOURER_OF_SOULS   = 36502,
    NPC_SYLVANAS_PART1      = 37596,
    NPC_SYLVANAS_PART2      = 38161,
    NPC_JAINA_PART1         = 37597,
    NPC_JAINA_PART2         = 38160,
    NPC_KALIRA              = 37583,
    NPC_ELANDRA             = 37774,
    NPC_LORALEN             = 37779,
    NPC_KORELN              = 37582,
    NPC_CHAMPION_1_HORDE    = 37584,
    NPC_CHAMPION_2_HORDE    = 37587,
    NPC_CHAMPION_3_HORDE    = 37588,
    NPC_CHAMPION_1_ALLIANCE = 37496,
    NPC_CHAMPION_2_ALLIANCE = 37497,
    NPC_CRUCIBLE_OF_SOULS   = 37094,
	NPC_CORRUPT_SOUL        = 36535,
	NPC_SOUL_HORROR         = 36522,
};

enum Encounters
{
	MAX_ENCOUNTER = 2,
};

#endif