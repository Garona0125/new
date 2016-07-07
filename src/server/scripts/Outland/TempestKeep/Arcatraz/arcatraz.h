/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef DEF_ARCATRAZ_H
#define DEF_ARCATRAZ_H

enum Data
{
	TYPE_ZEREKETH                      = 1,
	TYPE_DALLIAH                       = 2,
	TYPE_SOCCOTHRATES                  = 3,
	TYPE_HARBINGERSKYRISS              = 4,
	TYPE_WARDEN_1                      = 5,
	TYPE_WARDEN_2                      = 6,
	TYPE_WARDEN_3                      = 7,
	TYPE_WARDEN_4                      = 8,
	TYPE_WARDEN_5                      = 9,
	DATA_MELLICHAR                     = 10,
	TYPE_SHIELD_OPEN                   = 11,
	DATA_SPHERE_SHIELD                 = 12,
	DATA_DALLIAHTHEDOOMSAYEREVENT      = 13,
	DATA_WRATH_SCRYERSOCCOTHRATESEVENT = 14,
	DATA_WRATH_SCRYERSOCCOTHRATES      = 15,
	DATA_DALLIAHTHEDOOMSAYER           = 16,
	DATA_DALLIAH25                     = 17,
	DATA_SOCCOTHRATES25                = 18,
	DATA_MILLHOUSE                     = 19,
};

enum Creatures
{
	NPC_TRICKSTER            = 20905,
    NPC_PH_HUNTER            = 20906,
    NPC_MILLHOUSE_MANASTORM  = 20977,
    NPC_AKKIRIS              = 20908,
    NPC_SULFURON             = 20909,
    NPC_TW_DRAK              = 20910,
    NPC_BL_DRAK              = 20911,
    NPC_SKYRISS              = 20912,
	NPC_SKYRISS_CLONE_1      = 21466,
	NPC_SKYRISS_CLONE_2      = 21467,
	NPC_VOID_ZONE            = 21101,
	NPC_ALPHA_TARGET         = 21436,
	NPC_MELLICHAR            = 20904,
	NPC_ARCANE_SPAWN_TRIGGER = 21186,
	NPC_DALLIAH              = 20885,
	NPC_WRATH_SCRYER         = 20886,
	NPC_PROTEAN_SPAWN        = 21395,
	NPC_SIGHTLESS_EYE        = 21346,
	NPC_DESTROYED_SENTINEL   = 21761,
	NPC_NEGATON_FIELD        = 21414,
	NPC_WORLD_TRIGGER        = 22515,
	NPC_ALPHA_POD_TARGET     = 21436,
	NPC_BETA_POD_TARGET      = 21437,
	NPC_DELTA_POD_TARGET     = 21438,
	NPC_GAMMA_POD_TARGET     = 21439,
	NPC_OMEGA_POD_TARGET     = 21440,
};

enum Gameobjects
{
	GO_CONTAINMENT_CORE_SECURITY_FIELD_ALPHA = 184318,
	GO_CONTAINMENT_CORE_SECURITY_FIELD_BETA  = 184319,
	GO_POD_ALPHA                             = 183961,
	GO_POD_GAMMA                             = 183962,
	GO_POD_BETA                              = 183963,
	GO_POD_DELTA                             = 183964,
	GO_POD_OMEGA                             = 183965,
	GO_WARDEN_SHIELD                         = 184802,
	GO_SEAL_SPHERE                           = 184802,
};

enum Misc
{
	QUEST_TRIAL_OF_THE_NAARU_TENACITY = 10886,
	SPELL_COMPLETE_QUEST_10886        = 39564,
	MAX_ENCOUNTER                     = 9,
};

#endif