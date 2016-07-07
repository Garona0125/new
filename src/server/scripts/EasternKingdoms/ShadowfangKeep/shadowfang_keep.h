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

#ifndef DEF_SHADOWFANG_H
#define DEF_SHADOWFANG_H

enum Data
{
    TYPE_FREE_NPC               = 1,
    TYPE_RETHILGORE             = 2,
    TYPE_FENRUS                 = 3,
    TYPE_NANDOS                 = 4,
    TYPE_CROWN                  = 5,
};

enum Creatures
{
    NPC_FRYE                = 36272,
    NPC_HUMMEL              = 36296,
    NPC_BAXTER              = 36565,
	NPC_CRAZED_APOTHECARY   = 36568,
    NPC_VIAL_BUNNY          = 36530,
    NPC_ASH                 = 3850,
    NPC_ADA                 = 3849,
    NPC_ARCHMAGE_ARUGAL     = 4275,
    NPC_ARUGAL_VOIDWALKER   = 4627,
};

enum Gameobjects
{
    GO_COURTYARD_DOOR        = 18895,
    GO_SORCERER_DOOR         = 18972,
    GO_ARUGAL_DOOR           = 18971,
};

enum Misc
{
    SAY_BOSS_DIE_AD          = -1033007,
    SAY_BOSS_DIE_AS          = -1033008,
    SAY_ARCHMAGE             = -1033009,

    SPELL_ASHCROMBE_TELEPORT = 15742,
	SPELL_UNLOCK             = 6421,
    SPELL_DARK_OFFERING      = 7154,

    ACTION_SPAWN_CRAZED      = 3,
	MAX_ENCOUNTER            = 5,
};

enum Data64
{
    DATA_DOOR,
    DATA_FRYE,
    DATA_HUMMEL,
    DATA_BAXTER
};

#endif