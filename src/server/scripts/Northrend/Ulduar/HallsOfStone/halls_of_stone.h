/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef DEF_HALLS_OF_STONE_H
#define DEF_HALLS_OF_STONE_H

enum Data
{
    DATA_KRYSTALLUS_EVENT      = 0,
    DATA_MAIDEN_OF_GRIEF_EVENT = 1,
    DATA_SJONNIR_EVENT         = 2,
    DATA_BRANN_EVENT           = 3,
};

enum Data64
{
    DATA_KRYSTALLUS          = 0,
    DATA_MAIDEN_OF_GRIEF     = 1,
    DATA_SJONNIR             = 2,
    DATA_KADDRAK             = 3,
    DATA_MARNAK              = 4,
    DATA_ABEDNEUM            = 5,
    DATA_GO_TRIBUNAL_CONSOLE = 6,
    DATA_GO_KADDRAK          = 7,
    DATA_GO_MARNAK           = 8,
    DATA_GO_ABEDNEUM         = 9,
    DATA_GO_SKY_FLOOR        = 10,
    DATA_SJONNIR_DOOR        = 11,
    DATA_MAIDEN_DOOR         = 12,
};

enum Creatures
{
    NPC_MAIDEN_OF_GRIEF       = 27975,
    NPC_KRYSTALLUS            = 27977,
    NPC_SJONNIR               = 27978,
    NPC_MARNAK                = 30897,
    NPC_KADDRAK               = 30898,
    NPC_ABEDNEUM              = 30899,
	NPC_TRIBUNAL_OF_THE_AGES  = 28234,
    NPC_BRANN_BRONZEBEARD     = 28070,
    NPC_DARK_MATTER           = 28235,
    NPC_DARK_MATTER_TARGET    = 28237,
    NPC_SEARING_GAZE_TARGET   = 28265,
    NPC_DARK_RUNE_PROTECTOR   = 27983,
    NPC_DARK_RUNE_STORMCALLER = 27984,
    NPC_IRON_GOLEM_CUSTODIAN  = 27985,
	NPC_FORGED_IRON_DWARF     = 27982,
    NPC_FORGED_IRON_TROGG     = 27979,
    NPC_MALFORMED_OOZE        = 27981,
    NPC_IRON_SLUDGE           = 28165,
    NPC_EARTHEN_DWARF         = 27980,
	NPC_LESSER_AIR_ELEMENTAL  = 28384,
	NPC_CHANNEL_TARGET        = 28055,
	NPC_ERODED_SHARDLING      = 27974,
};

enum GameObjects
{
    GO_ABEDNEUM               = 191669,
    GO_MARNAK                 = 191670,
    GO_KADDRAK                = 191671,
    GO_MAIDEN_DOOR            = 191292,
    GO_BRANN_DOOR             = 191295,
    GO_SJONNIR_DOOR           = 191296,
    GO_TRIBUNAL_CONSOLE       = 193907,
    GO_TRIBUNAL_CHEST         = 190586,
    GO_TRIBUNAL_CHEST_HERO    = 193996,
    GO_TRIBUNAL_SKYROOM_FLOOR = 191527,
};

enum Encounters
{
	MAX_ENCOUNTER = 4,
};

#endif