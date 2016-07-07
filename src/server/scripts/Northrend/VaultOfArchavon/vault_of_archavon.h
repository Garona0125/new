/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef DEF_VAULT_OF_ARCHAVON_H
#define DEF_VAULT_OF_ARCHAVON_H

enum Creatures
{
	NPC_KORALON          = 35013,
    NPC_EMALON           = 33993,
	NPC_RELENTION        = 32951,
    NPC_TORAVON          = 38433,
	NPC_ARCHAVON         = 31125,
	NPC_ARCHAVON_WARDER  = 32353,
	NPC_TEMPEST_WARDER   = 33998,
	NPC_FLAME_WARDER     = 35143,
	NPC_FROST_WARDER     = 38482,
	NPC_FROZEN_ORB       = 38456,
};

enum Data64
{
	DATA_KORALON    = 1,
	DATA_EMALON     = 2,
	DATA_TORAVON    = 3,
    DATA_ARCHAVON   = 4,
};

enum AchievementCriteriaIds
{
    CRITERIA_EARTH_WIND_FIRE_10 = 12018,
    CRITERIA_EARTH_WIND_FIRE_25 = 12019,
};

enum AchievementSpells
{
    SPELL_EARTH_WIND_FIRE_ACHIEVEMENT_CHECK = 68308,
};

enum Encounter
{
	MAX_ENCOUNTER = 5,
};

#endif