/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef DEF_STEAM_VAULT_H
#define DEF_STEAM_VAULT_H

enum Data
{
	TYPE_HYDROMANCER_THESPIA   = 1,
	TYPE_MEKGINEER_STEAMRIGGER = 2,
	TYPE_WARLORD_KALITHRESH    = 3,
	TYPE_DISTILLER             = 4,
	DATA_MEKGINEER_STEAMRIGGER = 5,
	DATA_KALITRESH             = 6,
	DATA_THESPIA               = 7,
};

enum Creatures
{
	NPC_GYDROMANCER_THESPIA   = 17797,
	NPC_MEKGINEER_STEAMRIGGER = 17796,
	NPC_WARLORD_KALITHRESH    = 17798,
	NPC_WATER_ELEMENTAL       = 17917,
	NPC_STREAMRIGGER_MECHANIC = 17951,
};

enum Gameobjects
{
	GO_KALITHRESH_DOOR          = 183049,
	GO_ACCESS_PANEL_HYDROMANCER = 184125,
	GO_ACCESS_PANEL_MEKGINEER   = 184126,
};

enum Encounters
{
	MAX_ENCOUNTER = 4,
};

#endif