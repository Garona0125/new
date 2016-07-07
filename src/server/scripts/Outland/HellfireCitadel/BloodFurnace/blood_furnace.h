/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

#ifndef DEF_BLOOD_FURNACE_H
#define DEF_BLOOD_FURNACE_H

enum Data
{
	DATA_THE_MAKER                  = 0,
	DATA_BROGGOK                    = 1,
	DATA_KELIDAN_THE_MAKER          = 2,
	TYPE_THE_MAKER_EVENT            = 3,
	TYPE_BROGGOK_EVENT              = 4,
	TYPE_KELIDAN_THE_BREAKER_EVENT  = 5,
	DATA_DOOR_1                     = 6,
	DATA_DOOR_2                     = 7,
	DATA_DOOR_3                     = 8,
	DATA_DOOR_4                     = 9,
	DATA_DOOR_5                     = 10,
	DATA_DOOR_6                     = 11,
	DATA_PRISON_CELL_1              = 12,
	DATA_PRISON_CELL_2              = 13,
	DATA_PRISON_CELL_3              = 14,
	DATA_PRISON_CELL_4              = 15,
	DATA_PRISON_CELL_5              = 16,
	DATA_PRISON_CELL_6              = 17,
	DATA_PRISON_CELL_7              = 18,
	DATA_PRISON_CELL_8              = 19,
};

enum Encounters
{
	MAX_ENCOUNTER = 3,
};

enum Creatures
{
	NPC_KELIDAN               = 17377,
	NPC_BROGGOK               = 17380,
	NPC_THE_MAKER             = 17381,
	NPC_LAUGHING_SKULL_WARDEN = 17624,
	NPC_FEL_ORC_NEOPHYTE      = 17429,
    NPC_CHANNELER             = 17653,
	NPC_BROGGOK_POISON_CLOUD  = 17662,
	NPC_FELHOUND_MANASTALKER  = 17401,
	NPC_SEDUCTRESS            = 17399,
};

enum Gameobjects
{
	GO_SUMMON_DOOR_1         = 181823,
	GO_PRISON_DOOR_1         = 181766,
	GO_PRISON_DOOR_2         = 181811,
	GO_PRISON_DOOR_3         = 181812,
	GO_PRISON_DOOR_4         = 181819,
	GO_PRISON_DOOR_5         = 181822,
	GO_PRISON_CELL_DOOR_1    = 181813,
	GO_PRISON_CELL_DOOR_2    = 181814,
	GO_PRISON_CELL_DOOR_3    = 181816,
	GO_PRISON_CELL_DOOR_4    = 181815,
	GO_PRISON_CELL_DOOR_5    = 181821,
	GO_PRISON_CELL_DOOR_6    = 181818,
	GO_PRISON_CELL_DOOR_7    = 181820,
	GO_PRISON_CELL_DOOR_8    = 181817,
};

#endif