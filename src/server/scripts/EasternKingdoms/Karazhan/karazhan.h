/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef DEF_KARAZHAN_H
#define DEF_KARAZHAN_H

enum KarazhanDataTypes
{
    TYPE_ATTUMEN                    = 1,
    TYPE_MOROES                     = 2,
    TYPE_MAIDEN                     = 3,
    TYPE_OPTIONAL_BOSS              = 4,
    TYPE_OPERA                      = 5,
    TYPE_CURATOR                    = 6,
    TYPE_ARAN                       = 7,
    TYPE_TERESTIAN                  = 8,
    TYPE_NETHERSPITE                = 9,
    TYPE_CHESS                      = 10,
    TYPE_MALCHEZZAR                 = 11,
    TYPE_NIGHTBANE                  = 12,

    DATA_OPERA_PERFORMANCE          = 13,
    DATA_OPERA_OZ_DEATHCOUNT        = 14,
    DATA_KILREK                     = 15,
    DATA_TERESTIAN                  = 16,
    DATA_MOROES                     = 17,
    DATA_GO_CURTAINS                = 18,
    DATA_GO_STAGEDOORLEFT           = 19,
    DATA_GO_STAGEDOORRIGHT          = 20,
    DATA_GO_LIBRARY_DOOR            = 21,
    DATA_GO_MASSIVE_DOOR            = 22,
    DATA_GO_NETHER_DOOR             = 23,
    DATA_GO_GAME_DOOR               = 24,
    DATA_GO_GAME_EXIT_DOOR          = 25,
    DATA_IMAGE_OF_MEDIVH            = 26,
    DATA_MASTERS_TERRACE_DOOR_1     = 27,
    DATA_MASTERS_TERRACE_DOOR_2     = 28,
    DATA_GO_SIDE_ENTRANCE_DOOR      = 29,
};

enum Creatures
{
	NPC_MOROES                    = 15687,
	NPC_TERESTIAN                 = 15688,
	NPC_KILREK                    = 17229,
	NPC_ROKAD_THE_RAVAGER         = 16181,
    NPC_HYAKISS_THE_LURKER        = 16179,
    NPC_SHADIKITH_THE_G_L_I_D_E_R = 16180,
	NPC_ASTRAL_FLARE              = 17096,
	NPC_SPOTLIGHT                 = 19525,
	NPC_DEMON_CHAINS              = 17248,
	NPC_FIENDISH_IMP              = 17267,
	NPC_PORTAL                    = 17265,
	NPC_WATER_ELEMENTAL           = 17167,
	NPC_SHADOW_OF_ARAN            = 18254,
	NPC_ARAN_BLIZZARD             = 17161,
};

enum OperaEvents
{
    EVENT_OZ                        = 1,
    EVENT_HOOD                      = 2,
    EVENT_RAJ                       = 3,
};

enum Encounters
{
	MAX_ENCOUNTER = 12,
};

enum Quests
{
	QUEST_NIGHTBANE = 9644,
};

#endif