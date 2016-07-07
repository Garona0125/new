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

#ifndef DEF_SERPENT_SHRINE_H
#define DEF_SERPENT_SHRINE_H

enum Creatures
{
	// Lady Vasj
	NPC_SHIED_GENERATOR_CHANNEL = 19870,
	NPC_ENCHANTED_ELEMENTAL     = 21958,
	NPC_TAINTED_ELEMENTAL       = 22009,
	NPC_COILFANG_STRIDER        = 22056,
	NPC_COILFANG_ELITE          = 22055,
	NPC_TOXIC_SPOREBAT          = 22140,
	NPC_TOXIC_SPORES_TRIGGER    = 22207,
	// Fantom Lord
	NPC_CYCLONE                 = 22104,
	NPC_FATHOM_SPOREBAT         = 22120,
	NPC_FATHOM_LURKER           = 22119,
	NPC_SPITFIRE_TOTEM          = 22091,
	NPC_EARTHBIND_TOTEM         = 22486,
	NPC_POISON_CLEANSING_TOTEM  = 22487,
	// Hydros
	NPC_PURE_SPAWN              = 22035,
	NPC_TAINTED_SPAWN           = 22036,
	NPC_BEAM_DUMMY              = 21934,
	// Leoretas
	NPC_SPELLBINDER             = 21806,
	// Tidewalker
	NPC_WATER_GLOBULE           = 21913,
    NPC_TIDEWALKER_LURKER       = 21920,
	// Lurker
	NPC_COILFANG_GUARDIAN       = 21873,
	NPC_COILFANG_AMBUSHER       = 21865,
};

enum WaterEventState
{
    WATERSTATE_NONE     = 0,
    WATERSTATE_FRENZY   = 1,
    WATERSTATE_SCALDING = 2
};

enum DataTypes
{
    DATA_CANSTARTPHASE3             = 1,
    DATA_CARIBDIS                   = 2,
    DATA_HYDROSSTHEUNSTABLEEVENT    = 3,
    DATA_KARATHRESS                 = 4,
    DATA_KARATHRESSEVENT            = 5,
    DATA_KARATHRESSEVENT_STARTER    = 6,
    DATA_LADYVASHJ                  = 7,
    DATA_LADYVASHJEVENT             = 8,
    DATA_LEOTHERASTHEBLINDEVENT     = 9,
    DATA_MOROGRIMTIDEWALKEREVENT    = 10,
    DATA_SHARKKIS                   = 11,
    DATA_SHIELDGENERATOR1           = 12,
    DATA_SHIELDGENERATOR2           = 13,
    DATA_SHIELDGENERATOR3           = 14,
    DATA_SHIELDGENERATOR4           = 15,
    DATA_THELURKERBELOW             = 16,
    DATA_THELURKERBELOWEVENT        = 17,
    DATA_TIDALVESS                  = 18,
    DATA_FATHOMLORDKARATHRESSEVENT  = 19,
    DATA_LEOTHERAS                  = 20,
    DATA_LEOTHERAS_EVENT_STARTER    = 21,
    DATA_CONTROL_CONSOLE            = 22,
    DATA_STRANGE_POOL               = 23,
    DATA_WATER                      = 24,
    DATA_TRASH                      = 25,
};

#endif