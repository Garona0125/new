/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef DEF_EYE_OF_ETERNITY_H
#define DEF_EYE_OF_ETERNITY_H

enum InstanceData
{
    BOSS_MALYGOS,
    MAX_ENCOUNTER,
};

enum Data64
{
    DATA_MALYGOS,
    DATA_PLATFORM,
};

enum Creatures
{
    NPC_MALYGOS                 = 28859,
    NPC_VORTEX                  = 30090,
    NPC_POWER_SPARK             = 30084,
    NPC_ARCANE_OVERLOAD         = 30282,
    NPC_SURGE_OF_POWER          = 30334,
    NPC_NEXUS_LORD              = 30245,
    NPC_SCION_OF_ETERNITY       = 30249,
    NPC_STATIC_FIELD            = 30592,
    NPC_WYRMREST_SKYTALON       = 30161,
    NPC_ALEXSTRASZA             = 32295,
    NPC_ALEXSTRASZAS_GIFT       = 32448,
    NPC_HOVER_DISK_CASTER       = 30234, // Nexus Lord
    NPC_HOVER_DISK_MELEE        = 30248, // Scion of eternity from disk
	NPC_PORTAL_TRIGGER          = 30118,
};

enum GameObjects
{
    GO_PLATFORM                 = 193070,
    GO_EXIT_PORTAL              = 193908,
    GO_FOCUSING_IRIS_10         = 193958,
    GO_FOCUSING_IRIS_25         = 193960,
    GO_ALEXSTRASZAS_GIFT_10     = 193905,
    GO_ALEXSTRASZAS_GIFT_25     = 193967,
    GO_HEART_OF_MAGIC_10        = 194158,
    GO_HEART_OF_MAGIC_25        = 194159,
};

enum AchievementData
{
    ACHIEV_TIMED_START_EVENT    = 20387,
};

enum Areaids
{
    AREA_EYE_OF_ETERNITY        = 4500,
};

#endif