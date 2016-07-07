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

#ifndef DEF_GRUULS_LAIR_H
#define DEF_GRUULS_LAIR_H

enum Data
{
	DATA_MAULGAR_EVENT             = 0,
	DATA_BLIND_EYE_THE_SEER        = 1,
	DATA_KIGGLER_THE_CRAZED        = 2,
	DATA_KROSH_FIRE_HAND           = 3,
	DATA_OLM_THE_SUMMONER          = 4,
	DATA_MAULGAR                   = 5,
	DATA_GRUUL_EVENT               = 6,
	DATA_MAULGAR_DOOR              = 7,
	DATA_GRUUL_DOOR                = 8,
};

enum Creatures
{
	NPC_KIGGLER_THE_CRAZED = 18835,
	NPC_BLIND_EYE_THE_SEER = 18836,
	NPC_OLM_THE_SUMMONER   = 18834,
	NPC_KROSH_FIRE_HAND    = 18832,
	NPC_KING_MAULGAR       = 18831,
	NPC_WILD_FEL_STALKER   = 18847,
};

enum Gameobjects
{
	GO_MAULGAR_DOOR = 183817,
	GO_GRUUL_DOOR   = 184662,
};

enum Encounters
{
	MAX_ENCOUNTER = 2,
};

#endif