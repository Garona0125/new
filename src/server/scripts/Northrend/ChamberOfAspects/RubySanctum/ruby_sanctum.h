/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef DEF_RUBY_SANCTUM_H
#define DEF_RUBY_SANCTUM_H

enum Datas
{
    TYPE_EVENT			= 0,
    TYPE_RAGEFIRE		= 1,
    TYPE_BALTHARUS		= 2,
    TYPE_XERESTRASZA	= 3,
    TYPE_ZARITHRIAN		= 4,
    TYPE_HALION			= 5,
	MAX_ENCOUNTERS		= 6,
};

enum Types
{
    TYPE_COUNTER      = 6,
    TYPE_HALION_EVENT = 7,
    TYPE_EVENT_TIMER  = 50,
    TYPE_EVENT_NPC    = 51,
	COUNTER_OFF       = 255,
};

enum HalionDisplayIds
{
	DISPLAY_ID_HALION_COMBAT = 31952,
};

enum HalionTargets
{
	TARGETS_10 = 5,
	TARGETS_25 = 7,
	FR_RADIUS  = 45,
};

enum Creatures
{
    NPC_HALION_REAL                   = 39863,
    NPC_HALION_TWILIGHT	              = 40142,
    NPC_HALION_CONTROLLER	          = 40146,
    NPC_BALTHARUS		              = 39751,
    NPC_CLONE			              = 39899,
    NPC_ZARITHIAN		              = 39746,
    NPC_RAGEFIRE		              = 39747,
    NPC_XERESTRASZA		              = 40429,
    NPC_BALTHARUS_TARGET			  = 26712,
    NPC_ZARITHIAN_SPAWN_STALKER		  = 39794,
	NPC_ONYX_FLAMECALLER              = 39814,
    NPC_SHADOW_PULSAR_N               = 40083,
    NPC_SHADOW_PULSAR_S               = 40100,
    NPC_SHADOW_PULSAR_E               = 40468,
    NPC_SHADOW_PULSAR_W               = 40469,
    NPC_ORB_CARRIER					  = 40081,
    NPC_ORB_ROTATION_FOCUS			  = 40091,
	NPC_METEOR_STRIKE				  = 40029,
    NPC_METEOR_STRIKE_1				  = 40041,
    NPC_METEOR_STRIKE_2				  = 40042,
	NPC_ORB_CUTTER					  = 40081,
};

enum Gameojects
{
	GO_HALION_PORTAL_ENTER_TWILIGHT   = 202794,
    GO_HALION_PORTAL_ENTER_TWILIGHT_1 = 202795,
    GO_HALION_PORTAL_EXIT_TWILIGT     = 202796,
    GO_FIRE_FIELD					  = 203005,
    GO_FLAME_WALLS					  = 203006,
    GO_FLAME_RING					  = 203007,
};

enum DataEvents
{
    DATA_EVENT_TIMER				  = 101,
    DATA_EVENT						  = 102,
    DATA_ORB_DIRECTION				  = 110,
    DATA_ORB_S						  = 111,
    DATA_ORB_N						  = 112,
};

enum RubySanctumWorldStates
{
    UPDATE_STATE_UI_COUNT_R			= 5050,
    UPDATE_STATE_UI_COUNT_T			= 5050,
    UPDATE_STATE_UI_SHOW			= 5051,
};

#endif