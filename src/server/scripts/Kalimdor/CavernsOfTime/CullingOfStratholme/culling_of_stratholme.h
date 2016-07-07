/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef DEF_CULLING_OF_STRATHOLME_H
#define DEF_CULLING_OF_STRATHOLME_H

enum Data
{
    DATA_CRATES_EVENT    = 0,
    DATA_MEATHOOK_EVENT  = 1,
    DATA_SALRAMM_EVENT   = 2,
    DATA_EPOCH_EVENT     = 3,
    DATA_MAL_GANIS_EVENT = 4,
    DATA_INFINITE_EVENT  = 5,
    DATA_ARTHAS_EVENT    = 6,
    DATA_COUNTDOWN       = 7,
};

enum Data64
{
    DATA_ARTHAS           = 0,
    DATA_MEATHOOK         = 1,
    DATA_SALRAMM          = 2,
    DATA_EPOCH            = 3,
    DATA_MAL_GANIS        = 4,
    DATA_INFINITE         = 5,
    DATA_SHKAF_GATE       = 6,
    DATA_MAL_GANIS_GATE_1 = 7,
    DATA_MAL_GANIS_GATE_2 = 8,
    DATA_EXIT_GATE        = 9,
    DATA_MAL_GANIS_CHEST  = 10,
};

enum Creatures
{
    NPC_MEATHOOK               = 26529,
    NPC_SALRAMM                = 26530,
    NPC_EPOCH_HUNTER           = 26532,
    NPC_MAL_GANIS              = 26533,
    NPC_INFINITE_THE_CORRUPTOR = 32273,
    NPC_ARTHAS                 = 26499,
    NPC_JAINA                  = 26497,
    NPC_UTHER                  = 26528,
    NPC_GENERIC_BUNNY          = 28960,
    NPC_ENRAGED_GHOUL          = 27729,
	NPC_ACOLYTE                = 27731,
	NPC_MASTER_NECROMANCER     = 27732,
	NPC_GHOUL_MINION           = 27733,
	NPC_CRYPT_FIEND            = 27734,
	NPC_PATCHWORK_CONSTRUCT    = 27736,
	NPC_ZOMBIE                 = 27737,
	NPC_INFINITE_ADVERSARY     = 27742,
    NPC_INFINITE_HUNTER        = 27743,
    NPC_INFINITE_AGENT         = 27744,
	NPC_TOMB_STALKER           = 28199,
	NPC_DARK_NECROMANCER       = 28200,
    NPC_BILE_GOLEM             = 28201,
	NPC_DEVOURING_GHOUL        = 28249,
	NPC_TIME_RIFT              = 28409,
	NPC_SKELETAL_MINION        = 28878,
    NPC_INVIS_TARGET           = 20562,
    NPC_KNIGHT_ESCORT          = 27745,
    NPC_PRIEST_ESCORT          = 27747,
    NPC_CITY_MAN               = 28167,
    NPC_CITY_MAN2              = 28169,
    NPC_CITY_MAN3              = 31126,
    NPC_CITY_MAN4              = 31127,
	NPC_CHROMIE_PART_1         = 26527,
	NPC_CHROMIE_PART_2         = 27915,
	NPC_CHROMIE_PART_3         = 30997,
	NPC_A_ROYAL_ESCORT         = 31006,
    NPC_DISPELLING_ILLUSIONS   = 30996,
	NPC_WORLD_TRIGGER          = 18721,
};

enum GameObjects
{
    GO_SHKAF_GATE         = 188686,
    GO_MALGANIS_GATE_1    = 187711,
    GO_MALGANIS_GATE_2    = 187723,
    GO_EXIT_GATE          = 191788,
    GO_MALGANIS_CHEST_N   = 190663,
    GO_MALGANIS_CHEST_H   = 193597,
    GO_SUSPICIOUS_CRATE   = 190094,
    GO_PLAGUED_CRATE      = 190095,
};

enum Achievements
{
	ACHIEVEMENT_CULLING_OF_TIME = 1817,
};

enum WorldStatesCoS
{
    WORLDSTATE_NUMBER_CRATES_SHOW              = 3479,
    WORLDSTATE_NUMBER_CRATES_COUNT             = 3480,
    WORLDSTATE_NUMBER_SCOURGE_WAVES_SHOW_COUNT = 3504,
    WORLDSTATE_NUMBER_INFINITE_TIMER           = 3931,
    WORLDSTATE_NUMBER_INFINITE_SHOW            = 3932,
};

enum Misc
{
	ITEM_ID_ARCANE_DISRUPTOR   = 37888,
	MODEL_ID_TIME_RIFT         = 18877,
	QUEST_DISPELLING_ILLUSIONS = 13149,
};

enum Encounters
{
	MAX_ENCOUNTER = 7,
};

#endif