/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef DEF_OBSIDIAN_SANCTUM_H
#define DEF_OBSIDIAN_SANCTUM_H

enum Types
{
    TYPE_SARTHARION_EVENT       = 1,
    TYPE_TENEBRON_PREKILLED     = 2,
    TYPE_SHADRON_PREKILLED      = 3,
    TYPE_VESPERON_PREKILLED     = 4,
};

enum Data
{
    DATA_SARTHARION             = 10,
    DATA_TENEBRON               = 11,
    DATA_SHADRON                = 12,
    DATA_VESPERON               = 13,
};

enum Creatures
{
    NPC_SARTHARION              = 28860,
    NPC_TENEBRON                = 30452,
    NPC_SHADRON                 = 30451,
    NPC_VESPERON                = 30449,
	NPC_ACOLYTE_OF_SHADRON      = 31218,
	NPC_ACOLYTE_OF_VESPERON     = 31219,
	NPC_TWILIGHT_EGG            = 30882,
	NPC_TWILIGHT_WHELP          = 30890,
	NPC_FLAME_TSUNAMI           = 30616,
    NPC_LAVA_BLAZE              = 30643,
};

enum Gameobjects
{
    GO_TWILIGHT_PORTAL          = 193988,
};

enum Achievements
{
	ACHIEVEMENT_VOLCANO_10 = 2047, 
    ACHIEVEMENT_VOLCANO_25 = 2048,
};

enum Misc
{
	MAX_WAYPOINT          = 6,
	MAX_ENCOUNTER         = 4,
	TWILIGHT_ACHIEVEMENTS = 1,
};

#endif