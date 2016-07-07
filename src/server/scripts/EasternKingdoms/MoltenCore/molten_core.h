/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef DEF_MOLTEN_CORE_H
#define DEF_MOLTEN_CORE_H

enum Data
{
    BOSS_LUCIFRON                   = 0,
    BOSS_MAGMADAR                   = 1,
    BOSS_GEHENNAS                   = 2,
    BOSS_GARR                       = 3,
    BOSS_SHAZZRAH                   = 4,
    BOSS_BARON_GEDDON               = 5,
    BOSS_SULFURON_HARBINGER         = 6,
    BOSS_GOLEMAGG_THE_INCINERATOR   = 7,
    BOSS_MAJORDOMO_EXECUTUS         = 8,
    BOSS_RAGNAROS                   = 9,
};

enum Actions
{
    ACTION_START_RAGNAROS       = 0,
    ACTION_START_RAGNAROS_ALT   = 1,
};

Position const RagnarosTelePos   = {829.159f, -815.773f, -228.972f, 5.30500f};
Position const RagnarosSummonPos = {838.510f, -829.840f, -232.000f, 2.00000f};

Position const SummonPositions[10] =
{
    {737.850f, -1145.35f, -120.288f, 4.71368f},
    {744.162f, -1151.63f, -119.726f, 4.58204f},
    {751.247f, -1152.82f, -119.744f, 4.49673f},
    {759.206f, -1155.09f, -120.051f, 4.30104f},
    {755.973f, -1152.33f, -120.029f, 4.25588f},
    {731.712f, -1147.56f, -120.195f, 4.95955f},
    {726.499f, -1149.80f, -120.156f, 5.24055f},
    {722.408f, -1152.41f, -120.029f, 5.33087f},
    {718.994f, -1156.36f, -119.805f, 5.75738f},
    {838.510f, -829.840f, -232.000f, 2.00000f},
};

enum Creatures
{
    NPC_LUCIFRON                 = 12118,
    NPC_MAGMADAR                 = 11982,
    NPC_GEHENNAS                 = 12259,
    NPC_GARR                     = 12057,
    NPC_SHAZZRAH                 = 12264,
    NPC_BARON_GEDDON             = 12056,
    NPC_SULFURON_HARBINGER       = 12098,
    NPC_GOLEMAGG_THE_INCINERATOR = 11988,
    NPC_MAJORDOMO_EXECUTUS       = 12018,
    NPC_RAGNAROS                 = 11502,
    NPC_FLAMEWAKER_HEALER        = 11663,
    NPC_FLAMEWAKER_ELITE         = 11664,
	NPC_LAVA_SPAWN               = 12265,
	NPC_SON_OF_FLAME             = 12143,
};

enum GameObjects
{
    GO_CACHE_OF_THE_FIRELORD     = 179703,
};

enum RagnarosData
{
    DATA_RAGNAROS_ADDS  = 0,
};

enum Encounters
{
	MAX_ENCOUNTER = 10,
};

#endif