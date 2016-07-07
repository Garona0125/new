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

#ifndef DEF_MAGISTERS_TERRACE_H
#define DEF_MAGISTERS_TERRACE_H

#define ERROR_INST_DATA "TSCR Error: Instance Data not set properly for Magister's Terrace instance (map 585). Encounters will be buggy."
#endif

enum Data
{
    DATA_SELIN_EVENT          = 0,
    DATA_VEXALLUS_EVENT       = 1,
    DATA_DELRISSA_EVENT       = 2,
    DATA_KAELTHAS_EVENT       = 3,
    DATA_SELIN                = 4,
    DATA_FEL_CRYSTAL          = 5,
    DATA_FEL_CRYSTAL_SIZE     = 6,
    DATA_VEXALLUS_DOOR        = 7,
    DATA_DELRISSA             = 8,
    DATA_DELRISSA_DOOR        = 9,
    DATA_KAEL_DOOR            = 10,
    DATA_KAEL_STATUE_LEFT     = 11,
    DATA_KAEL_STATUE_RIGHT    = 12,
	DATA_KAELTHAS_STATUES     = 13,
    DATA_ESCAPE_ORB           = 14,
};

enum Creatures
{
	NPC_SELIN         = 24723,
    NPC_DELRISSA      = 24560,
    NPC_FEL_CRYSTALS  = 24722,
	NPC_PURE_ENERGY   = 24745,
	NPC_SLIVER        = 24552,
	NPC_FIZZLE        = 24656,
	NPC_PHOENIX       = 24674,
	NPC_PHOENIX_EGG   = 24675,
	NPC_ARCANE_SPHERE = 24708,
	NPC_KAEL          = 24848,
	NPC_KAGANI        = 24557,
	NPC_ELLRYS        = 24558,
	NPC_ERAMAS        = 24554,
	NPC_YAZZAI        = 24561,
	NPC_SALARIS       = 24559,
	NPC_GARAXXAS      = 24555,
	NPC_APOKO         = 24553,
	NPC_ZELFAN        = 24556,
};

enum GameObjects
{
    GO_VEXALLUS_DOOR        = 187896,
    GO_SELIN_DOOR           = 187979,
    GO_SELIN_ENCOUNTER_DOOR = 188065,
    GO_DELRISSA_DOOR        = 187770,
    GO_KAEL_DOOR            = 188064,
    GO_KAEL_STATUE_1        = 188165,
    GO_KAEL_STATUE_2        = 188166,
    GO_ESCAPE_ORB           = 188173,
};

enum Encounters
{
	MAX_ENCOUNTER = 4,
};