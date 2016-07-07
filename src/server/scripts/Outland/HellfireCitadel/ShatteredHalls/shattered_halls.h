/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef DEF_SHATTERED_H
#define DEF_SHATTERED_H

enum Data
{
	TYPE_NETHEKURSE                 = 1,
	DATA_NETHEKURSE                 = 2,
	DATA_NETHEKURSE_DOOR_ENTRANCE   = 3,
	TYPE_OMROGG                     = 4,
	DATA_NETHEKURSE_DOOR_EXIT       = 5,
};

enum Creatures
{
	NPC_GRAND_WARLOCK_NETERCURSE = 16807,
	NPC_FEL_ORC_CONVERT          = 17083,
	NPC_LEFT_HEAD                = 19523,
    NPC_RIGHT_HEAD               = 19524,
	NPC_SHATTERED_ASSASSIN       = 17695,
    NPC_HEARTHEN_GUARD           = 17621,
    NPC_SHARPSHOOTER_GUARD       = 17622,
    NPC_REAVER_GUARD             = 17623,
	NPC_SCOUT_ORGARR             = 17297,
	NPC_KORAG                    = 17295,
	NPC_CAPT_BONESHATTER         = 17296,
	NPC_RIFLEMAN_BROWNBEARD      = 17289,
	NPC_PROVATE_JACINT           = 17292,
	NPC_CAPT_ALINA               = 17290,
};

enum Gameobjects
{
	GO_GRAND_WARLOCK_CHAMBER_DOOR_ENTRANCE = 182539,
	GO_GRAND_WARLOCK_CHAMBER_DOOR_EXIT     = 182540,
};

enum Misc
{
	TIMER_NOT_STARTED       = 1,
	TIMER_STARTED           = 2,
	TIMER_NEXT_PHASE        = 3,
	TIMER_ENDED             = 4,
	TIMER_STATE             = 5,
	EXECUTION_PHASE         = 6,
	MAX_ENCOUNTER           = 2,
};

#endif