/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef DEF_UTGARDE_KEEP_H
#define DEF_UTGARDE_KEEP_H

enum Data
{
	DATA_PRINCE_KELESETH        = 1,
	DATA_PRINCE_KELESETH_EVENT  = 2,
	DATA_SKARVALD               = 3,
	DATA_DALRONN                = 4,
	DATA_SKARVALD_DALRONN_EVENT = 5,
	DATA_INGVAR                 = 6,
	DATA_INGVAR_EVENT           = 7,
	EVENT_FORGE_1               = 8,
	EVENT_FORGE_2               = 9,
	EVENT_FORGE_3               = 10,
};

enum Creatures
{
	NPC_PRINCE_KELESETH          = 23953,
	NPC_SKARVALD_THE_CONSTRUCTOR = 24200,
	NPC_DALRONN_THE_CONTROLLER   = 24201,
	NPC_INGVAR_THE_PLUNDERER     = 23954,
	NPC_FROST_TOMB               = 23965,
    NPC_SKELETON                 = 23970,
    NPC_SKARVALD_GHOST           = 27390,
	NPC_DALRONN_GHOST            = 27389,
	NPC_ANNHYLDE_THE_CALLER      = 24068,
    NPC_INGVAR_UNDEAD            = 23980,
    NPC_THROW_TARGET             = 23996,
	NPC_WORLD_TRIGGER            = 22515,
};

enum Gameobjects
{
	GO_BELLOW_1           = 186688,
	GO_BELLOW_2           = 186689,
	GO_BELLOW_3           = 186690,
	GO_FORGE_FIRE_1       = 186692,
	GO_FORGE_FIRE_2       = 186693,
	GO_FORGE_FIRE_3       = 186691,
	GO_GLOWING_ANVIL_1    = 186609,
	GO_GLOWING_ANVIL_2    = 186610,
	GO_GLOWING_ANVIL_3    = 186611,
	GO_GIANT_PORTCULLIS_0 = 186608,
	GO_GIANT_PORTCULLIS_1 = 186756,
	GO_GIANT_PORTCULLIS_2 = 186694,
};

enum Achievements
{
	ACHIEVEMENT_ON_THE_ROCKS = 1919,
};

enum Encounters
{
	MAX_ENCOUNTER = 3,
};

#endif