/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DEF_DRAK_THARON_H
#define DEF_DRAK_THARON_H

enum Data
{
    DATA_TROLLGORE_EVENT  = 1,
    DATA_NOVOS_EVENT      = 2,
    DATA_DRED_EVENT       = 3,
    DATA_THARON_JA_EVENT  = 4,
    DATA_KING_DRED_ACHIEV = 5,
};

enum Data64
{
    DATA_TROLLGORE        = 1,
    DATA_NOVOS            = 2,
    DATA_DRED             = 3,
    DATA_THARON_JA        = 4,
    DATA_NOVOS_CRYSTAL_1  = 5,
    DATA_NOVOS_CRYSTAL_2  = 6,
    DATA_NOVOS_CRYSTAL_3  = 7,
    DATA_NOVOS_CRYSTAL_4  = 8,
};

enum Actions
{
	ACTION_RAPTOR_KILLED = 1,
	DATA_KING_DRED       = 2,
};

enum Creatures
{
	NPC_TROLLGORE          = 26630,
    NPC_NOVOS              = 26631,
    NPC_KING_DRED          = 27483,
    NPC_THARON_JA          = 26632,
    NPC_RISEN_SHADOWCASTER = 27600,
    NPC_FETID_TROLL_CORPSE = 27598,
    NPC_HULKING_CORPSE     = 27597,
    NPC_CRYSTAL_HANDLER    = 26627,
	NPC_RAPTOR_1           = 26641,
    NPC_RAPTOR_2           = 26628,
	NPC_DRAKKARI_INVADER_1 = 27753,
    NPC_DRAKKARI_INVADER_2 = 27709,
	// Novos Triggers
	NPC_CRYSTAL_TARGET_1   = 26712,
	NPC_CRYSTAL_TARGET_2   = 30612,
	NPC_CRYSTAL_TARGET_3   = 30618,
	NPC_CRYSTAL_TARGET_4   = 30620,
};

enum GameObjects
{
    GO_NOVOS_CRYSTAL_1 = 189299,
    GO_NOVOS_CRYSTAL_2 = 189300,
    GO_NOVOS_CRYSTAL_3 = 189301,
    GO_NOVOS_CRYSTAL_4 = 189302,
};

enum Achievements
{
    ACHIEVEMENT_CONSUMPTION_JUNCTION = 2151,
	ACHIEVEMENT_OH_NOVOS             = 2057,
};

enum Enconters
{
	MAX_ENCOUNTER = 4,
};

#endif