/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef DEF_HALLS_OF_LIGHTNING_H
#define DEF_HALLS_OF_LIGHTNING_H

enum Data
{
	DATA_BJARNGRIM          = 1,
    DATA_IONAR              = 2,
    DATA_LOKEN              = 3,
    DATA_VOLKHAN            = 4,
	DATA_VOLKHAN_ANVIL		= 9,
    TYPE_BJARNGRIM          = 10,
    TYPE_IONAR              = 11,
    TYPE_LOKEN              = 12,
    TYPE_VOLKHAN            = 13,
};

enum Creatures
{
    NPC_STORMFORGED_LIEUTENANT = 29240,
    NPC_BJARNGRIM              = 28586,
    NPC_VOLKHAN                = 28587,
    NPC_IONAR                  = 28546,
    NPC_LOKEN                  = 28923,
	NPC_VOLKHAN_ANVIL		   = 28823,
	NPC_SPARK_OF_IONAR         = 28926,
	NPC_MOLTEN_GOLEM           = 28695,
    NPC_BRITTLE_GOLEM          = 28681,
};

enum Gameobjects
{
    GO_BJARNGRIM_DOOR       = 191416,
    GO_VOLKHAN_DOOR         = 191325,
    GO_IONAR_DOOR           = 191326,
    GO_LOKEN_DOOR           = 191324,
    GO_LOKEN_THRONE         = 192654,
};

enum Achievements
{
	ACHIEV_TIMELY_DEATH_START_EVENT = 20384,
};

enum Encounters
{
	MAX_ENCOUNTER = 4,
};

#endif