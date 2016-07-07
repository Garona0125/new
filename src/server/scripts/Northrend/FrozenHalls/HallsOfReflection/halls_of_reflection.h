/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef DEF_HALLS_OF_REFLECTION_H
#define DEF_HALLS_OF_REFLECTION_H

enum Data
{
    MAX_ENCOUNTER          = 4,

    TYPE_PHASE             = 0,
    DATA_FALRIC_EVENT      = 1,
    DATA_MARWYN_EVENT      = 2,
    DATA_FROSTSWORN_EVENT  = 3,
    DATA_LICH_KING_EVENT   = 4,
    DATA_INTRO_EVENT       = 5,
    DATA_TEAM_IN_INSTANCE  = 6,
    DATA_WAVE_COUNT        = 7,

    DATA_SUMMONS           = 8,
    DATA_ICE_WALL_1        = 9,
    DATA_ICE_WALL_2        = 10,
    DATA_ICE_WALL_3        = 11,
    DATA_ICE_WALL_4        = 12,
    DATA_PHASE             = 13,
};

enum Data64
{
    DATA_FALRIC            = 101,
    DATA_MARWYN            = 102,
    DATA_LICHKING          = 103,
    DATA_ESCAPE_LIDER      = 104,
    DATA_FROSTMOURNE       = 105,
    DATA_FRONT_DOOR        = 106,
    DATA_FROSTWORN_DOOR    = 107,
    DATA_ARTHAS_DOOR       = 108,
    DATA_CAPTAIN           = 109,
};

enum HallsOfReflectionSpells
{
	SPELL_FULL_HEAL         = 25840,
	SPELL_MASS_SUICIDE      = 67375,
	SPELL_ACHIEVEMENT_CHECK = 72830,
};

enum Creatures
{
    NPC_FALRIC                = 38112,
    NPC_MARWYN                = 38113,
    NPC_FROSTSWORN_GENERAL    = 36723,
	NPC_SPIRITUAL_REFLECTION  = 37068,
    NPC_LICH_KING_EVENT       = 36954,
    NPC_QUELDELAR             = 37158,
    NPC_LICH_KING             = 37226,
    NPC_UTHER                 = 37225,
    NPC_JAINA_PART1           = 37221,
    NPC_JAINA_OUTRO           = 36955,
    NPC_SYLVANAS_PART1        = 37223,
    NPC_SYLVANAS_OUTRO        = 37554,
	NPC_SYLVANAS_QUEST        = 38161,
	NPC_JAINA_QUEST           = 32364,
	NPC_ARCHMAGE_ELANDRA      = 37774,
	NPC_DARK_RANGER_LORALEN   = 37779,
    NPC_BARTLETT              = 37182,
    NPC_KORM                  = 37833,
	NPC_PHANTOM_MAGE          = 38172,
	NPC_SPECTRAL_FOOTMAN      = 38173,
	NPC_TORTURED_RIFLEMAN     = 38176,
	NPC_SHADOWY_MERCENARY     = 38177,
	NPC_PHANTOM_HALLUCINATION = 38567,
	NPC_GHOSTLY_PRIEST        = 38175,
    NPC_ICE_WALL              = 37014,
    NPC_RAGING_GHOUL          = 36940,
    NPC_RISEN_WITCH_DOCTOR    = 36941,
    NPC_LUMBERING_ABOMINATION = 37069,
	NPC_KILL_CREDIT_LICH_KING = 38211,
};

enum GameObjects
{
    GO_FROSTMOURNE          = 202302,
    GO_FROSTMOURNE_ALTAR    = 202236,
    GO_FRONT_DOOR           = 201976,
    GO_FROSTWORN_DOOR       = 197341,
    GO_ARTHAS_DOOR          = 197342,
    GO_RUN_DOOR             = 197343,
    GO_ICE_WALL_1           = 201385,
    GO_ICE_WALL_2           = 201885,
    GO_ICE_WALL_3           = 202396,
    GO_ICE_WALL_4           = 202397,
    GO_CAVE                 = 201596,
    GO_STAIRS_SKYBREAKER    = 201709,
    GO_STAIRS_ORGRIM_HAMMER = 202211,
    GO_PORTAL               = 202079,
	GO_THE_SKYBREAKER       = 201598,
    GO_ORGRIMS_HAMMER       = 201599,
    GO_CAPTAIN_CHEST_1      = 202212,
    GO_CAPTAIN_CHEST_2      = 201710,
    GO_CAPTAIN_CHEST_3      = 202337,
    GO_CAPTAIN_CHEST_4      = 202336,
};

enum Achievements
{
    ACHIEVEMENT_HALLS_OF_REFLECTION_5N = 4518,
    ACHIEVEMENT_HALLS_OF_REFLECTION_5H = 4521,
    ACHIEVEMENT_NOT_RETREATING_EVENT   = 22615,
};

enum Quests
{
	QUEST_DELIVRANCE_FROM_THE_PIT_A  = 24710,
    QUEST_DELIVRANCE_FROM_THE_PIT_H  = 24712,
    QUEST_WRATH_OF_THE_LICH_KING_A   = 24500,
    QUEST_WRATH_OF_THE_LICH_KING_H   = 24802,
	QUEST_THE_HALLS_OF_REFFLECTION_A = 24480,
	QUEST_THE_HALLS_OF_REFFLECTION_H = 24561,
};

enum HorWorldStates
{
    WORLD_STATE_HOR            = 4884,
    WORLD_STATE_HOR_WAVE_COUNT = 4882,
};

const Position OutroSpawns[2] =
{
    {5551.290039f, 2261.330078f, 733.012034f, 3.93f}, // Lich King
    {5547.270020f, 2256.949951f, 733.0109f, 0.8f},    // Jaina/Sylvana
};

enum Actions
{
    ACTION_ENTER_COMBAT,
};

struct boss_horAI : QuantumBasicAI
{
    boss_horAI(Creature* creature) : QuantumBasicAI(creature), summons(creature)
    {
        instance = me->GetInstanceScript();
    }

    InstanceScript* instance;
    EventMap events;
    SummonList summons;

    void Reset()
    {
        events.Reset();
        me->SetVisible(false);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        me->SetReactState(REACT_PASSIVE);
    }

    void DamageTaken(Unit* /*who*/, uint32 &damage)
    {
        if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            damage = 0;
    }

	void DoAction(const int32 action)
    {
        switch (action)
        {
            case ACTION_ENTER_COMBAT:
                me->SetVisible(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_AGGRESSIVE);

                if (Unit* unit = me->SelectNearestTarget())
                    AttackStart(unit);

                DoZoneInCombat();
                break;
        }
    }

    void JustSummoned(Creature* summoned)
    {
        summons.Summon(summoned);

        if (Unit* target = summoned->SelectNearestTarget())
        {
            if (summoned->AI())
                summoned->AI()->AttackStart(target);
            else
            {
                summoned->GetMotionMaster()->MoveChase(target);
                summoned->Attack(target, true);
            }
        }

        if (summoned->AI())
            summoned->AI()->DoZoneInCombat();
    }

    void SummonedCreatureDespawn(Creature* summoned)
    {
        summons.Despawn(summoned);
        if (summons.empty())
        {
            if (summoned->IsAlive())
                instance->SetData(DATA_WAVE_COUNT, NOT_STARTED);
            else
                instance->SetData(DATA_WAVE_COUNT, SPECIAL);
        }
    }
};

struct Locations
{
    float x, y, z, o;
    uint32 id;
};

static Locations SpawnLoc[] =
{
    //Wing 01
    {5277.409f, 1993.161f, 707.694f, 0.05f}, //27
    {5301.876f, 2041.699f, 707.694f, 4.71f}, //1
    {5339.830f, 2020.887f, 707.694f, 3.14f}, //13
    {5311.041f, 2042.935f, 707.694f, 4.71f}, //3
    {5314.750f, 2039.969f, 707.694f, 4.71f}, //4
    {5342.823f, 2003.801f, 707.694f, 3.14f}, //10
    {5311.579f, 1972.823f, 707.694f, 1.62f}, //16
    //Wing 02
    {5272.491f, 2005.673f, 707.694f, 0.05f}, //23
    {5302.669f, 1973.050f, 707.694f, 1.62f}, //18
    {5346.187f, 2008.058f, 707.694f, 3.14f}, //9
    {5319.752f, 2041.321f, 707.694f, 4.71f}, //5
    {5344.882f, 1998.714f, 707.694f, 3.14f}, //11
    {5340.552f, 1994.735f, 707.694f, 3.14f}, //12
    {5306.441f, 2040.358f, 707.694f, 4.71f}, //2
    //Wing 03
    {5273.297f, 2014.009f, 707.694f, 0.05f}, //25
    {5316.062f, 1970.777f, 707.694f, 1.62f}, //15
    {5322.498f, 2037.415f, 707.694f, 4.71f}, //6
    {5307.087f, 1970.065f, 707.694f, 1.62f}, //17
    {5342.460f, 2012.391f, 707.694f, 3.14f}, //8
    {5297.601f, 1971.420f, 707.694f, 1.62f}, //19
    {5295.668f, 1975.853f, 707.694f, 1.62f}, //20
    //Wing 04
    {5273.661f, 1996.767f, 707.694f, 0.05f}, //21
    {5275.228f, 2001.275f, 707.694f, 0.05f}, //22
    {5344.153f, 2017.753f, 707.694f, 3.14f}, //7
    {5275.310f, 2009.686f, 707.694f, 0.05f}, //24
    {5319.835f, 1975.177f, 707.694f, 1.62f}, //14
    {5277.445f, 2017.197f, 707.694f, 0.05f}, //26
    {5298.198f, 2037.762f, 707.694f, 4.71f},  //0
};

#endif