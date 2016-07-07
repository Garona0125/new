/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef DEF_TEMPLE_OF_AHNQIRAJ_H
#define DEF_TEMPLE_OF_AHNQIRAJ_H

enum DataTypes
{
    DATA_PROPHET_SKERAM = 0,
    DATA_LORD_KRI       = 1,
    DATA_VEM            = 2,
	DATA_YAUJ           = 3,
	DATA_SATURA         = 4,
	DATA_FANKRISS       = 5,
	DATA_VISCIDUS       = 6,
	DATA_HUHURAN        = 7,
	DATA_VEKNILASH      = 8,
	DATA_VEKLOR         = 9,
	DATA_OURO           = 10,
	DATA_CTUN           = 11,
	DATA_CTHUN_PHASE    = 12,
	DATA_SKERAM_DOOR    = 13,
	DATA_TWINS_DOOR     = 14,
	DATA_CTUN_DOOR      = 15,
};

enum Creatures
{
    NPC_EYE_OF_CTHUN        = 15589,
    NPC_CTHUN_PORTAL        = 15896,
    NPC_CLAW_TENTACLE       = 15725,
    NPC_EYE_TENTACLE        = 15726,
    NPC_SMALL_PORTAL        = 15904,
    NPC_BODY_OF_CTHUN       = 15809,
    NPC_GIANT_CLAW_TENTACLE = 15728,
    NPC_GIANT_EYE_TENTACLE  = 15334,
    NPC_FLESH_TENTACLE      = 15802,
    NPC_GIANT_PORTAL        = 15910,
    NPC_VISCIDUS            = 15299,
    NPC_GLOB_OF_VISCIDUS    = 15667,
    NPC_PROPHET_SKERAM      = 15263,
	NPC_LORD_KRI            = 15511,
	NPC_YAUJ                = 15543,
    NPC_VEM                 = 15544,
    NPC_VEKLOR              = 15276,
    NPC_VEKNILASH           = 15275,
	NPC_SPAWN_OF_FANKRISS   = 15630,
	NPC_VEKNISS_HATCHING    = 15962,
	NPC_ANUBISATH_SENTINEL  = 15264,
	NPC_CTUN                = 15727,
	NPC_YAUJ_BROOD          = 15621,
	NPC_BATTLEGUARD_SATURA  = 15516,
	NPC_VEKNISS_BORER       = 15622,
	NPC_FANKRISS            = 15510,
	NPC_PRINCESS_HUHURAN    = 15509,
	NPC_QUIRAJI_SCARAB      = 15316,
	NPC_QUIRAJI_SCORPION    = 15317,
	NPC_OURO                = 15517,
	NPC_THE_MASTERS_EYE     = 15963,
	// ??
	NPC_DIRT_MOUND          = 15712,
};

enum GameObjects
{
	GO_SKERAM_DOOR  = 180636,
	GO_TWINS_DOOR   = 180634,
	GO_DOOR_TO_CTUN = 180635,
	GO_DIRT_MOUND   = 186944,
};

enum Taunt
{
	RANDOM_SOUND_WHISPER = 8663,
};

#endif