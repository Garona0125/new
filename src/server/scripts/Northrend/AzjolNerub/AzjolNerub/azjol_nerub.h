/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef DEF_AZJOL_NERUB_H
#define DEF_AZJOL_NERUB_H

enum Data64
{
    DATA_KRIKTHIR_THE_GATEWATCHER,
    DATA_HADRONOX,
    DATA_ANUBARAK,
    DATA_WATCHER_GASHRA,
    DATA_WATCHER_SILTHIK,
    DATA_WATCHER_NARJIL
};

enum Data
{
    DATA_KRIKTHIR_THE_GATEWATCHER_EVENT,
    DATA_HADRONOX_EVENT,
    DATA_ANUBARAK_EVENT
};

enum Creatures
{
	NPC_KRIKTHIR_THE_GATEWATCHER      = 28684,
	NPC_WATCHER_GASHRA                = 28730,
	NPC_WATCHER_SILTHIK               = 28731,
	NPC_WATCHER_NARJIL                = 28729,
	NPC_HADRONOX                      = 28921,
	NPC_ANUBARAK                      = 29120,
	NPC_SKITTERING_SWARMER            = 28735,
    NPC_SKITTERING_SWARMER_CONTROLLER = 32593,
    NPC_SKITTERING_INFECTIOR          = 28736,
    NPC_ANUBAR_DATTER                 = 29213, // 53599
	NPC_ANUBAR_ASSASIN                = 29214, // 53609
	NPC_ANUBAR_GUARDIAN               = 29216, // 53613
    NPC_ANUBAR_VENOMANCER             = 29217, // 53615
    NPC_IMPALE_TARGET                 = 89,
};

enum Gameobjects
{
	GO_KRIKTHIR_DOOR   = 192395,
	GO_ANUBARAK_DOOR   = 192396,
	GO_ANUBARAK_DOOR_1 = 192397,
	GO_ANUBARAK_DOOR_2 = 192398,
};

enum Achievements
{
    ACHIEV_TIMED_START_EVENT = 20381,
};

enum Encounters
{
	MAX_ENCOUNTER = 3,
};

#endif