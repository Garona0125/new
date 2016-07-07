/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "pit_of_saron.h"

const Position spawnPoints1[4] =
{
    {877.666199f, 61.523361f, 527.974243f, 3.624145f},
    {885.715027f, 65.515602f, 533.431030f, 3.857180f},
    {912.531189f, 63.785076f, 547.925720f, 3.235373f},
    {909.602722f, 83.248398f, 551.600403f, 3.251082f},
};

const Position spawnPoints2[4] =
{    
    {879.286743f, 41.535030f, 521.009338f, 3.486701f},
    {889.974548f, 45.270569f, 527.154236f, 3.612364f},
    {919.853027f, 88.957771f, 558.705993f, 3.592729f},
    {921.595520f, 69.234627f, 557.946716f, 3.145053f},    
};

const Position SpawnPointsYmirjar[2] =
{
    {883.150024f, 54.626401f, 528.262024f, 3.678787f},
    {915.107971f, 75.316299f, 553.531006f, 3.678787f},
};

const Position SpawnPointsFallenWarrior[8] =
{
    {937.606506f, 0.776727f, 578.888000f, 1.090893f},
    {928.419006f, 8.786335f, 577.693970f, 1.122307f},
    {924.478699f, -7.662051f, 582.044983f, 1.489874f},
    {935.244568f, -10.427516f, 583.265503f, 1.358702f},
    {935.098694f, -24.272480f, 588.035400f, 1.653226f},
    {921.272644f, -22.194103f, 585.452576f, 1.331212f},
    {930.109009f, -56.889900f, 591.848999f, 2.353980f},
    {924.945984f, -60.164799f, 591.879028f, 2.237270f},
};

const Position SpawnPointsWrathboneColdwraith[4] =
{
    {925.477234f, -1.383301f, 580.479980f, 1.240126f},
    {935.6500513f, -6.321967f, 582.133972f, 1.141952f},
    {920.783020f, -44.854099f, 590.411987f, 1.590680f},
    {933.810974f, -45.009399f, 591.658997f, 1.582540f},
};

const Position IceCicleSpawnPointsFallenWarrior[8] =
{
    {997.252991f, -139.257004f, 615.875000f, 2.722710f},
    {1000.400024f, -127.873001f, 616.247009f, 3.403390f},
    {1049.770020f, -113.330002f, 629.814026f, 4.101520f},
    {1042.160034f, -104.300003f, 630.038025f, 3.892080f},
    {1062.150024f, -29.850700f, 633.879028f, 4.433140f},
    {1073.599976f, -31.012199f, 633.408997f, 4.607670f},
    {1069.910034f, 100.042000f, 631.062012f, 4.869470f},
    {1059.170044f, 95.906303f, 630.781006f, 4.939280f},
};

const Position IceCiclespawnPointsWrathboneSkeleton[12] =
{
    {1033.609863f, -113.968132f, 627.523987f, 3.794239f},
    {1043.910034f, -124.613998f, 627.747986f, 3.595380f},
    {1068.930054f, -88.752602f, 632.828003f, 4.223700f},
    {1050.0f, -69.646599f, 633.078979f, 4.363320f},
    {1055.680054f, -52.713402f, 633.510986f, 4.607670f},
    {1069.369995f, -52.008701f, 633.919983f, 4.520400f},
    {1069.390015f, -12.805800f, 633.627014f, 4.537860f},
    {1077.579956f, -14.718200f, 632.726013f, 4.450590f},
    {1079.089966f, 34.306599f, 629.799988f, 4.607670f},
    {1071.270020f, 38.016102f, 629.828979f, 4.956740f},
    {1058.099976f, 92.909897f, 630.413025f, 5.074410f},
    {1070.369995f, 96.143799f, 631.075012f, 4.910200f},
};

const Position IceCiclespawnPointsWrathboneSorcerer[2] =
{
    {1073.608643f, 49.570923f, 630.635559f, 4.996680f},
    {1067.034912f, 47.677979f, 630.472473f, 4.851380f},
};

static const Position MoveLocations2[9] =
{
    {1025.534790f, 129.039612f, 628.156189f, 0.000000f},
    {1035.394897f, 144.298599f, 628.156189f, 0.000000f},
    {1042.624390f, 156.986679f, 628.156189f, 0.000000f},
    {1037.534790f, 132.039612f, 628.156189f, 0.000000f},
    {1050.534790f, 140.039612f, 628.156189f, 0.000000f},
    {1043.534790f, 141.039612f, 628.156189f, 0.000000f},
    {1038.534790f, 130.039612f, 628.156189f, 0.000000f},
    {1029.534790f, 125.039612f, 628.156189f, 0.000000f},
    {1028.534790f, 158.039612f, 628.156189f, 0.000000f},
};

static const Position triggerPos1 = {1048.629150f, 110.203377f, 628.224060f, 2.118303f};
static const Position triggerPos2 = {1063.679932f, 119.296852f, 628.156189f, 2.251821f};
static const Position sindraPos = {986.353271f, 174.938004f, 670.492798f, 0.000000f};
static const Position leaderPosOutro2 = {988.998962f, 172.250290f, 628.156128f, 0.000000f};

enum DungeonTexts
{
    SAY_TYRRANUS_1                 = -1658074,
    SAY_TYRRANUS_2                 = -1658075,
    SAY_TYRRANUS_3                 = -1658078,
    SAY_TYRRANUS_4                 = -1658082,
    SAY_SYLVANAS_1                 = -1658076,
    SAY_SYLVANAS_2                 = -1658080,
    SAY_SYLVANAS_3                 = -1658085,
    SAY_SYLVANAS_4                 = -1658087,
    SAY_JAINA_1                    = -1658077,
    SAY_JAINA_2                    = -1658081,
    SAY_JAINA_3                    = -1658084,
    SAY_JAINA_4                    = -1658086,
	SAY_JAINA_5                    = -1658088,
	SAY_TYRANNUS_AMBUSH_1          = -1658050,
	SAY_TYRANNUS_AMBUSH_2          = -1658051,
	SAY_GAUNTLET_START             = -1658052,
	SAY_GORKUN_OUTRO_1             = -1658064,
    SAY_GORKUN_OUTRO_2             = -1658065,
    SAY_JAYNA_OUTRO_3              = -1658066,
    SAY_SYLVANAS_OUTRO_3           = -1658067,
    SAY_JAYNA_OUTRO_4              = -1658068,
    SAY_SYLVANAS_OUTRO_4           = -1658069,
    SAY_JAYNA_OUTRO_5              = -1658070,
	SAY_RESCUED_HORDE_ALLIANCE     = -1658071,
	SAY_SOUND                      = 16750,
};

enum PitOfSaronSpells
{
    SPELL_NECROMANTIC_POWER         = 69347,
	SPELL_NECROMANTIC_POWER_1       = 69753,
    SPELL_DEATH_ARMY_VISUAL         = 67691,
    SPELL_BLIZZARD_JAINA            = 70132,
    SPELL_ICE_LANCE_JAINA           = 70464,
    SPELL_SHADOW_BOLT_SYLVANAS      = 70512,
    SPELL_MULTI_SHOT_SYLVANAS       = 70513,
	SPELL_BLUE_EXPLOSION            = 70509,
	SPELL_STRANGULATING             = 69413,
	SPELL_RAISE_DEAD                = 69350,
	SPELL_ICICLE                    = 69424,
	SPELL_ICE_SHARDS                = 69425,
	SPELL_ICICLE_1                  = 69426,
	SPELL_ICICLE_2                  = 69428,
};

struct startPosition
{
	uint32 entry[2];
    Position movePosition;
}

StartPositions[] =
{
    { { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE }, { 445.286f, 214.251f, 528.71f, 0.375996f } },
    { { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE }, { 447.246f, 208.189f, 528.71f, 0.0264938f } },
    { { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE }, { 437.81f,  231.115f, 528.708f, 0.525224f } },
    { { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE }, { 437.444f, 225.798f, 528.712f, 0.36029f } },
    { { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE }, { 438.941f, 221.784f, 528.708f, 0.30924f } },
    { { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE }, { 440.384f, 216.785f, 528.71f, 0.203211f } },
    { { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE }, { 441.479f, 211.472f, 528.709f,0.203211f } },
    { { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE }, { 443.986f, 222.103f, 528.709f,6.27373f } },
    { { NPC_CHAMPION_1_ALLIANCE, NPC_CHAMPION_1_HORDE }, { 448.944f, 204.945f, 528.709f,0.0258984f } },
    { { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_2_HORDE }, { 446.613f, 211.37f,  528.709f,0.360472f } },
    { { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_2_HORDE }, { 442.647f, 204.635f, 528.707f,0.0887308f } },
    { { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_2_HORDE }, { 435.054f, 207.61f,  528.707f,0.173361f } },
    { { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_2_HORDE }, { 434.568f, 210.389f, 528.707f,0.173361f } },
    { { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_2_HORDE }, { 443.458f, 226.623f, 528.709f,0.232266f } },
    { { NPC_CHAMPION_2_ALLIANCE, NPC_CHAMPION_2_HORDE }, { 435.593f, 204.535f, 528.728f,0.173361f } },
    { { NPC_CHAMPION_3_ALLIANCE, NPC_CHAMPION_3_HORDE }, { 442.47f,  208.153f, 528.706f,0.259756f } },
    { { NPC_CHAMPION_3_ALLIANCE, NPC_CHAMPION_3_HORDE }, { 441.333f, 233.866f, 528.709f,0.255828f } },
    { { NPC_CHAMPION_3_ALLIANCE, NPC_CHAMPION_3_HORDE }, { 442.124f, 230.78f,  528.709f,0.244047f } },
    { { 0, 0 }, { 0.0f, 0.0f, 0.0f, 0.0f } }
};

class npc_pos_intro : public CreatureScript
{
public:
    npc_pos_intro() : CreatureScript("npc_pos_intro") { }

    struct pos_introAI : public QuantumBasicAI
    {
        pos_introAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            Reset();
        }

		InstanceScript* instance;
        std::list<Creature*> Champion1;
        std::list<Creature*> Champion2;
        std::list<Creature*> Champion3;
        uint32 StartTimer;
        uint8 StartPhase;
        bool event;

        void Reset()
        {
            StartPhase = 0;
            StartTimer = 5000;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetReactState(REACT_PASSIVE);
            event = true;
        }

        void UpdateAI(uint32 const diff)
        {
            if (instance->GetData(DATA_TYRANNUS_START) == IN_PROGRESS && event)
            {
                if (StartTimer <= diff)
                {
                    switch (StartPhase)
                    {
                        case 0:
                            if (Creature* sTyrannus = me->FindCreatureWithDistance(NPC_SCOURGELORD_TYRANNUS_INTRO, 1000.0f, true))
                            {
                                DoSendQuantumText(SAY_TYRRANUS_1, sTyrannus);
                                
                                int32 entryIndex;
								if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
									entryIndex = 0;
								else
									entryIndex = 1;

								Position spawnPoint = {427.36f, 212.636f, 529.47f, 6.2564f};

								for (int8 i = 0; StartPositions[i].entry[entryIndex] != 0; ++i)
								{
									if (Creature* summon = me->SummonCreature(StartPositions[i].entry[entryIndex], spawnPoint, TEMPSUMMON_DEAD_DESPAWN))
										summon->GetMotionMaster()->MovePoint(0, StartPositions[i].movePosition);
								}
							}
                            ++StartPhase;
                            StartTimer = 5000;
                            break;
                        case 1:
                            if (Creature* sTyrannus = me->FindCreatureWithDistance(NPC_SCOURGELORD_TYRANNUS_INTRO, 1000.0f, true))
							{
                                DoSendQuantumText(SAY_TYRRANUS_2, sTyrannus);
							}
                            ++StartPhase;
                            StartTimer = 15000;
                            break;
                        case 2:
                            if (me->GetEntry() == NPC_SYLVANAS_PART1)
                                DoSendQuantumText(SAY_SYLVANAS_1, me);
                            else if (me->GetEntry() == NPC_JAINA_PART1)
                                DoSendQuantumText(SAY_JAINA_1, me);

                            if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                            {
                                GetCreatureListWithEntryInGrid(Champion1, me, NPC_CHAMPION_1_ALLIANCE, 500.0f);
                                for (std::list<Creature*>::iterator itr = Champion1.begin(); itr != Champion1.end(); ++itr)
                                {
                                    Creature* sChampions = *itr;
                                    if (!sChampions)
                                        continue;

                                    if (sChampions->IsAlive())
									{
                                        sChampions->GetMotionMaster()->MovePoint(0, 487.463989f, 246.891006f, 528.708984f);
										sChampions->SetCurrentFaction(35);
									}
                                }

                                GetCreatureListWithEntryInGrid(Champion2, me, NPC_CHAMPION_2_ALLIANCE, 500.0f);
                                for (std::list<Creature*>::iterator itr = Champion2.begin(); itr != Champion2.end(); ++itr)
                                {
                                    Creature* sChampions = *itr;
                                    if (!sChampions)
                                        continue;

                                    if (sChampions->IsAlive())
									{
                                       sChampions->GetMotionMaster()->MovePoint(0, 504.444000f, 211.061996f, 528.710022f);
									   sChampions->SetCurrentFaction(35);
									}
                                }

								GetCreatureListWithEntryInGrid(Champion3, me, NPC_CHAMPION_3_ALLIANCE, 500.0f);
                                for (std::list<Creature*>::iterator itr = Champion3.begin(); itr != Champion3.end(); ++itr)
                                {
                                    Creature* sChampions = *itr;
                                    if (!sChampions)
                                        continue;

                                    if (sChampions->IsAlive())
									{
										sChampions->GetMotionMaster()->MovePoint(0, 504.444000f, 211.061996f, 528.710022f);
										sChampions->SetCurrentFaction(35);
									}
                                }
                            }
                            else
                            {
                                GetCreatureListWithEntryInGrid(Champion1, me, NPC_CHAMPION_1_HORDE, 500.0f);
                                for (std::list<Creature*>::iterator itr = Champion1.begin(); itr != Champion1.end(); ++itr)
                                {
                                    Creature* sChampions = *itr;
                                    if (!sChampions)
                                        continue;

                                    if (sChampions->IsAlive())
									{
                                        sChampions->GetMotionMaster()->MovePoint(0, 487.463989f, 246.891006f, 528.708984f);
										sChampions->SetCurrentFaction(35);
									}
                                }

                                GetCreatureListWithEntryInGrid(Champion2, me, NPC_CHAMPION_2_HORDE, 500.0f);
                                for (std::list<Creature*>::iterator itr = Champion2.begin(); itr != Champion2.end(); ++itr)
                                {
                                    Creature* sChampions = *itr;
                                    if (!sChampions)
                                        continue;

                                    if (sChampions->IsAlive())
									{
                                        sChampions->GetMotionMaster()->MovePoint(0, 504.444000f, 211.061996f, 528.710022f);
										sChampions->SetCurrentFaction(35);
									}
                                }

                                GetCreatureListWithEntryInGrid(Champion3, me, NPC_CHAMPION_3_HORDE, 500.0f);
                                for (std::list<Creature*>::iterator itr = Champion3.begin(); itr != Champion3.end(); ++itr)
                                {
                                    Creature* sChampions = *itr;
                                    if (!sChampions)
                                        continue;

                                    if (sChampions->IsAlive())
									{
										sChampions->GetMotionMaster()->MovePoint(0, 504.444000f, 211.061996f, 528.710022f);
										sChampions->SetCurrentFaction(35);
									}
                                }
                            }
                            ++StartPhase;
                            StartTimer = 1000;
                            break;
                        case 3:
                            if (Creature* sTyrannus = me->FindCreatureWithDistance(NPC_SCOURGELORD_TYRANNUS_INTRO, 1000.0f, true))
								DoSendQuantumText(SAY_TYRRANUS_3, sTyrannus);

                            ++StartPhase;
                            StartTimer = 3000;
                            break;
						case 4:
							for (std::list<Creature*>::iterator itr = Champion1.begin(); itr != Champion1.end(); ++itr)
							{
								Creature* sChampions = *itr;
								if (!sChampions)
									continue;

								if (sChampions->IsAlive())
								{
									sChampions->StopMoving();
									sChampions->CastSpell(sChampions, SPELL_STRANGULATING, true);
								}
							}
							for (std::list<Creature*>::iterator itr = Champion2.begin(); itr != Champion2.end(); ++itr)
							{
								Creature* sChampions = *itr;
								if (!sChampions)
									continue;

								if (sChampions->IsAlive())
								{
									sChampions->StopMoving();
									sChampions->CastSpell(sChampions, SPELL_STRANGULATING, true);
								}
							}
							for (std::list<Creature*>::iterator itr = Champion3.begin(); itr != Champion3.end(); ++itr)
							{
								Creature* sChampions = *itr;
								if (!sChampions)
									continue;

								if (sChampions->IsAlive())
								{
									sChampions->StopMoving();
									sChampions->CastSpell(sChampions, SPELL_STRANGULATING, true);
								}
							}
							++StartPhase;
							StartTimer = 3000;
							break;
                        case 5:
                            if (Creature* sTyrannus = me->FindCreatureWithDistance(NPC_SCOURGELORD_TYRANNUS_INTRO, 1000.0f, true))
                                DoPlaySoundToSet(sTyrannus, SAY_SOUND);

                            for (std::list<Creature*>::iterator itr = Champion1.begin(); itr != Champion1.end(); ++itr)
                            {
                                Creature* sChampions = *itr;
                                if (!sChampions)
                                    continue;

                                if (sChampions->IsAlive())
                                {
                                    sChampions->AttackStop();
                                    sChampions->GetMotionMaster()->MoveFall();
                                }
                            }

                            for (std::list<Creature*>::iterator itr = Champion2.begin(); itr != Champion2.end(); ++itr)
                            {
                                Creature* sChampions = *itr;
                                if (!sChampions)
                                    continue;

                                if (sChampions->IsAlive())
                                {
                                    sChampions->AttackStop();
                                    sChampions->GetMotionMaster()->MoveFall();
                                }
                            }

                            for (std::list<Creature*>::iterator itr = Champion3.begin(); itr != Champion3.end(); ++itr)
                            {
                                Creature* sChampions = *itr;
                                if (!sChampions)
                                    continue;

                                if (sChampions->IsAlive())
                                {
                                    sChampions->AttackStop();
                                    sChampions->GetMotionMaster()->MoveFall();
                                }
                            }
                            ++StartPhase;
                            StartTimer = 5000;
                            break;
                        case 6:
                            if (Creature* sTyrannus = me->FindCreatureWithDistance(NPC_SCOURGELORD_TYRANNUS_INTRO, 1000.0f, true))
							{
                                sTyrannus->CastSpell(sTyrannus, SPELL_NECROMANTIC_POWER, true);
								sTyrannus->CastSpell(sTyrannus, SPELL_NECROMANTIC_POWER_1, true);
							}

                            for (std::list<Creature*>::iterator itr = Champion1.begin(); itr != Champion1.end(); ++itr)
                            {
                                Creature* sChampions = *itr;
                                if (!sChampions)
                                    continue;

                                if (sChampions->IsAlive())
                                    sChampions->Kill(sChampions, false);
                            }

                            for (std::list<Creature*>::iterator itr = Champion2.begin(); itr != Champion2.end(); ++itr)
                            {
                                Creature* sChampions = *itr;
                                if (!sChampions)
                                    continue;

                                if (sChampions->IsAlive())
                                    sChampions->Kill(sChampions, false);
                            }

                            for (std::list<Creature*>::iterator itr = Champion3.begin(); itr != Champion3.end(); ++itr)
                            {
                                Creature* sChampions = *itr;
                                if (!sChampions)
                                    continue;

                                if (sChampions->IsAlive())
                                    sChampions->Kill(sChampions, false);
                            }

                            if (me->GetEntry() == NPC_SYLVANAS_PART1)
                                DoSendQuantumText(SAY_SYLVANAS_2, me);
                            else if (me->GetEntry() == NPC_JAINA_PART1)
                                DoSendQuantumText(SAY_JAINA_2, me);
                            ++StartPhase;
                            StartTimer = 5000;
                            break;
                        case 7:
                            for (std::list<Creature*>::iterator itr = Champion1.begin(); itr != Champion1.end(); ++itr)
                            {
                                Creature* sChampions = *itr;
                                if (!sChampions)
                                    continue;

                                sChampions->Respawn();
                                sChampions->CastSpell(sChampions, SPELL_DEATH_ARMY_VISUAL, true);
                                if (me->GetEntry() == NPC_SYLVANAS_PART1)
                                    sChampions->UpdateEntry(NPC_CORRUPTED_CHAMPION, ALLIANCE);
                                else
                                    sChampions->UpdateEntry(NPC_CORRUPTED_CHAMPION, HORDE);
								sChampions->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                sChampions->SetReactState(REACT_PASSIVE);
                            }

                            for (std::list<Creature*>::iterator itr = Champion2.begin(); itr != Champion2.end(); ++itr)
                            {
                                Creature* sChampions = *itr;
                                if (!sChampions)
                                    continue;

                                sChampions->Respawn();
                                sChampions->CastSpell(sChampions, SPELL_DEATH_ARMY_VISUAL, true);
                                if (me->GetEntry() == NPC_SYLVANAS_PART1)
                                    sChampions->UpdateEntry(NPC_CORRUPTED_CHAMPION, ALLIANCE);
                                else
                                    sChampions->UpdateEntry(NPC_CORRUPTED_CHAMPION, HORDE);
                                sChampions->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                sChampions->SetReactState(REACT_PASSIVE);
                            }

                            for (std::list<Creature*>::iterator itr = Champion3.begin(); itr != Champion3.end(); ++itr)
                            {
                                Creature* sChampions = *itr;
                                if (!sChampions)
                                    continue;

                                sChampions->Respawn();
                                sChampions->CastSpell(sChampions, SPELL_DEATH_ARMY_VISUAL, true);
                                if (me->GetEntry() == NPC_SYLVANAS_PART1)
                                    sChampions->UpdateEntry(NPC_CORRUPTED_CHAMPION, ALLIANCE);
                                else
                                    sChampions->UpdateEntry(NPC_CORRUPTED_CHAMPION, HORDE);
                                sChampions->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                sChampions->SetReactState(REACT_PASSIVE);
                            }

                            if (Creature* sTyrannus = me->FindCreatureWithDistance(NPC_SCOURGELORD_TYRANNUS_INTRO, 1000.0f, true))
                                DoSendQuantumText(SAY_TYRRANUS_4, sTyrannus);

                            for (std::list<Creature*>::iterator itr = Champion1.begin(); itr != Champion1.end(); ++itr)
                            {
                                Creature* sChampions = *itr;
                                if (!sChampions)
                                    continue;

                                if (Creature* pHelper = me->FindCreatureWithDistance(NPC_KORALEN, 500.0f, true))
                                {
                                    sChampions->Attack(pHelper, true);
                                    sChampions->GetMotionMaster()->MoveChase(pHelper);
                                }
                                else if (Creature* pHelper = me->FindCreatureWithDistance(NPC_KORLAEN, 500.0f, true))
                                {
                                    sChampions->Attack(pHelper, true);
                                    sChampions->GetMotionMaster()->MoveChase(pHelper);
                                }
                            }

                            for (std::list<Creature*>::iterator itr = Champion2.begin(); itr != Champion2.end(); ++itr)
                            {
                                Creature* sChampions = *itr;
                                if (!sChampions)
                                    continue;

                                if (Creature* pHelper = me->FindCreatureWithDistance(NPC_KORALEN, 500.0f, true))
                                {
                                    sChampions->Attack(pHelper, true);
                                    sChampions->GetMotionMaster()->MoveChase(pHelper);
                                }
                                else if (Creature* pHelper = me->FindCreatureWithDistance(NPC_KORLAEN, 500.0f, true))
                                {
                                    sChampions->Attack(pHelper, true);
                                    sChampions->GetMotionMaster()->MoveChase(pHelper);
                                }
                            }

                            for (std::list<Creature*>::iterator itr = Champion3.begin(); itr != Champion3.end(); ++itr)
                            {
                                Creature* sChampions = *itr;
                                if (!sChampions)
                                    continue;

                                if (Creature* pHelper = me->FindCreatureWithDistance(NPC_KORALEN, 500.0f, true))
                                {
                                    sChampions->Attack(pHelper, true);
                                    sChampions->GetMotionMaster()->MoveChase(pHelper);
                                }
                                else if (Creature* pHelper = me->FindCreatureWithDistance(NPC_KORLAEN, 500.0f, true))
                                {
                                    sChampions->Attack(pHelper, true);
                                    sChampions->GetMotionMaster()->MoveChase(pHelper);
                                }
                            }
                            ++StartPhase;
                            StartTimer = 3000;
                            break;
                        case 8:
                            for (std::list<Creature*>::iterator itr = Champion1.begin(); itr != Champion1.end(); ++itr)
                            {
                                Creature* sChampions = *itr;
                                if (!sChampions)
                                    continue;

                                if (me->GetEntry() == NPC_SYLVANAS_PART1)
								{
									me->CastSpell(sChampions, SPELL_MULTI_SHOT_SYLVANAS, true);
									me->CastSpell(sChampions, SPELL_BLUE_EXPLOSION, true);
								}
								else
									me->CastSpell(sChampions, SPELL_BLIZZARD_JAINA, true);
                            }

                            for (std::list<Creature*>::iterator itr = Champion2.begin(); itr != Champion2.end(); ++itr)
                            {
                                Creature* sChampions = *itr;
                                if (!sChampions)
                                    continue;

                                if (me->GetEntry() == NPC_SYLVANAS_PART1)
                                    me->CastSpell(sChampions, SPELL_MULTI_SHOT_SYLVANAS, true);
                                else
                                    me->CastSpell(sChampions, SPELL_BLIZZARD_JAINA, true);
                            }

                            for (std::list<Creature*>::iterator itr = Champion3.begin(); itr != Champion3.end(); ++itr)
                            {
                                Creature* sChampions = *itr;
                                if (!sChampions)
                                    continue;

                                if (me->GetEntry() == NPC_SYLVANAS_PART1)
                                    me->CastSpell(sChampions, SPELL_MULTI_SHOT_SYLVANAS, true);
                                else
                                    me->CastSpell(sChampions, SPELL_BLIZZARD_JAINA, true);
                            }
                            ++StartPhase;
                            StartTimer = 500;
                            break;
                        case 9:
                            for (std::list<Creature*>::iterator itr = Champion1.begin(); itr != Champion1.end(); ++itr)
                            {
                                Creature* sChampions = *itr;
                                if (!sChampions)
                                    continue;

                                if (me->GetEntry() == NPC_SYLVANAS_PART1)
                                    me->CastSpell(sChampions, SPELL_SHADOW_BOLT_SYLVANAS, true);
                                else
                                    me->CastSpell(sChampions, SPELL_ICE_LANCE_JAINA, true);
                            }

                            for (std::list<Creature*>::iterator itr = Champion2.begin(); itr != Champion2.end(); ++itr)
                            {
                                Creature* sChampions = *itr;
                                if (!sChampions)
                                    continue;

                                if (me->GetEntry() == NPC_SYLVANAS_PART1)
                                    me->CastSpell(sChampions, SPELL_SHADOW_BOLT_SYLVANAS, true);
                                else
                                    me->CastSpell(sChampions, SPELL_ICE_LANCE_JAINA, true);
                            }

                            for (std::list<Creature*>::iterator itr = Champion3.begin(); itr != Champion3.end(); ++itr)
                            {
                                Creature* sChampions = *itr;
                                if (!sChampions)
                                    continue;

                                if (me->GetEntry() == NPC_SYLVANAS_PART1)
                                    me->CastSpell(sChampions, SPELL_SHADOW_BOLT_SYLVANAS, true);
                                else
                                    me->CastSpell(sChampions, SPELL_ICE_LANCE_JAINA, true);
                            }
                            ++StartPhase;
                            StartTimer = 500;
                            break;
                        case 10:
                            for (std::list<Creature*>::iterator itr = Champion1.begin(); itr != Champion1.end(); ++itr)
                            {
                                Creature* sChampions = *itr;
                                if (!sChampions)
                                    continue;
                                me->Kill(sChampions, false);
                            }

                            for (std::list<Creature*>::iterator itr = Champion2.begin(); itr != Champion2.end(); ++itr)
                            {
                                Creature* sChampions = *itr;
                                if (!sChampions)
                                    continue;

                                me->Kill(sChampions, false);
                            }

                            for (std::list<Creature*>::iterator itr = Champion3.begin(); itr != Champion3.end(); ++itr)
                            {
                                Creature* sChampions = *itr;
                                if (!sChampions)
                                    continue;

                                me->Kill(sChampions, false);
                            }
                            ++StartPhase;
                            StartTimer = 3000;
                            break;
                        case 11:
                            if (me->GetEntry() == NPC_JAINA_PART1)
                                DoSendQuantumText(SAY_JAINA_3, me);
                            else 
                                DoSendQuantumText(SAY_SYLVANAS_3, me);
                            ++StartPhase;
                            StartTimer = 10000;
                            break;
                        case 12:
                            if (me->GetEntry() == NPC_JAINA_PART1)
								DoSendQuantumText(SAY_JAINA_4, me);
							else 
                                DoSendQuantumText(SAY_SYLVANAS_4, me);
                            ++StartPhase;
                            StartTimer = 5000;
                            break;
						case 13:
							if (me->GetEntry() == NPC_JAINA_PART1)
								DoSendQuantumText(SAY_JAINA_5, me);
							++StartPhase;
							StartTimer = 10000;
							break;
                        case 14:
                            if (Creature* sTyrannus = me->FindCreatureWithDistance(NPC_SCOURGELORD_TYRANNUS_INTRO, 1000.0f, true))
							{
								sTyrannus->GetMotionMaster()->MovePoint(0, 1127.14f, -5.37662f, 761.042f);
								sTyrannus->DespawnAfterAction(40*IN_MILLISECONDS);
							}
                            instance->SetData(DATA_TYRANNUS_START, DONE);
                            event = false;
                            ++StartPhase;
                            StartTimer = 10000;
                            break;
                        return;					
                    }
                }
                else StartTimer -= diff;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
		return new pos_introAI(creature);
    }
};

class at_ymirjar_flamebearer_pos : public AreaTriggerScript
{
public:
	at_ymirjar_flamebearer_pos() : AreaTriggerScript("at_ymirjar_flamebearer_pos") { }

	bool OnTrigger(Player* player, AreaTriggerEntry const* areaTrigger)
	{
		InstanceScript* instance = player->GetInstanceScript();

		if (instance->GetData(DATA_AREA_TRIGGER_YMIRJAR) == DONE || player->IsGameMaster() || !instance)
			return false;

		if (instance->GetBossState(DATA_GARFROST) == DONE && instance->GetBossState(DATA_ICK) == DONE)
		{
			instance->SetData(DATA_AREA_TRIGGER_YMIRJAR, DONE);

			if (Creature* pTyrannus = player->SummonCreature(NPC_SCOURGELORD_TYRANNUS_INTRO, 940.076355f, 91.047089f, 576.178040f, 3.596342f, TEMPSUMMON_DEAD_DESPAWN, 0))
			{
				DoSendQuantumText(SAY_TYRANNUS_AMBUSH_1, pTyrannus);
				pTyrannus->SetSpeed(MOVE_FLIGHT, 8.5f, true);
				pTyrannus->GetMotionMaster()->MovePoint(0, 961.456f, -37.304f, 654.228f);
			}

			for (uint8 i = 0; i < 4; i++)
			{
				player->SummonCreature(NPC_YMIRJAR_FLAMEBEARER, spawnPoints1[i], TEMPSUMMON_DEAD_DESPAWN, 0);
				player->SummonCreature(NPC_YMIRJAR_WRATBRINGER, spawnPoints2[i], TEMPSUMMON_DEAD_DESPAWN, 0);
			}

			for (uint8 k = 0; k < 2; k++)
				player->SummonCreature(NPC_YMIRJAR_DEATHBRINGER, SpawnPointsYmirjar[k], TEMPSUMMON_DEAD_DESPAWN, 0);

			return false;
		}
		return false;
	}
};

class at_fallen_warrior_pos : public AreaTriggerScript
{
public:
	at_fallen_warrior_pos() : AreaTriggerScript("at_fallen_warrior_pos") { }

	bool OnTrigger(Player* player, AreaTriggerEntry const* areaTrigger)
	{
		InstanceScript* instance = player->GetInstanceScript();

		if (instance->GetData(DATA_AREA_TRIGGER_FALLEN) == DONE || player->IsGameMaster() || !instance)
			return false;

		if (instance->GetData(DATA_AREA_TRIGGER_YMIRJAR) == DONE)  
		{
			instance->SetData(DATA_AREA_TRIGGER_FALLEN, DONE);

			if (Creature* pTyrannus = player->FindCreatureWithDistance(NPC_SCOURGELORD_TYRANNUS_INTRO, 250.0f, true))
				DoSendQuantumText(SAY_TYRANNUS_AMBUSH_2, pTyrannus);

			for (uint8 i = 0; i < 8; i++)
				player->SummonCreature(NPC_FALLEN_WARRIOR, SpawnPointsFallenWarrior[i], TEMPSUMMON_DEAD_DESPAWN, 0);

			for (uint8 i = 0; i < 4; i++)
				player->SummonCreature(NPC_WRATHBONE_COLDWRAITH, SpawnPointsWrathboneColdwraith[i], TEMPSUMMON_DEAD_DESPAWN, 0);

			return false;
		}
		return false;
	}
};

class at_ice_cicle_pos : public AreaTriggerScript
{
public:
	at_ice_cicle_pos() : AreaTriggerScript("at_ice_cicle_pos") { }

	bool OnTrigger(Player* player, AreaTriggerEntry const* areaTrigger)
	{    
		InstanceScript* instance = player->GetInstanceScript();

		if (instance->GetData(DATA_AREA_TRIGGER_ICE_CICLE) == IN_PROGRESS || instance->GetData(DATA_AREA_TRIGGER_ICE_CICLE) == DONE || player->IsGameMaster() || !instance)
			return false;

		if (instance->GetData(DATA_AREA_TRIGGER_FALLEN) == DONE && instance->GetData(DATA_AREA_TRIGGER_YMIRJAR) == DONE)  
		{
			instance->SetData(DATA_AREA_TRIGGER_ICE_CICLE, IN_PROGRESS);

			if (Creature* pTyrannus = player->FindCreatureWithDistance(NPC_SCOURGELORD_TYRANNUS_INTRO, 500.0f, true))
			{
				DoSendQuantumText(SAY_GAUNTLET_START, pTyrannus);
				pTyrannus->DespawnAfterAction();
			}

			for (uint8 i = 0; i < 8; i++)
				player->SummonCreature(NPC_FALLEN_WARRIOR, IceCicleSpawnPointsFallenWarrior[i], TEMPSUMMON_DEAD_DESPAWN, 0);

			for (uint8 i = 0; i < 12; i++)
				player->SummonCreature(NPC_WRATHBONE_SKELETON, IceCiclespawnPointsWrathboneSkeleton[i], TEMPSUMMON_DEAD_DESPAWN, 0);

			for (uint8 i = 0; i < 2; i++)
				player->SummonCreature(NPC_WRATHBONE_SORCERER, IceCiclespawnPointsWrathboneSorcerer[i], TEMPSUMMON_DEAD_DESPAWN, 0);

			return false;
		}
		return false;
	}    
};

class at_slave_rescued_pos : public AreaTriggerScript
{
public:
	at_slave_rescued_pos() : AreaTriggerScript("at_slave_rescued_pos") { }

	bool OnTrigger(Player* player, AreaTriggerEntry const* areaTrigger)
	{
		InstanceScript* instance = player->GetInstanceScript();

		if (instance->GetData(DATA_SLAVE_OUTRO_GARFROST) == DONE || player->IsGameMaster() || !instance)
			return false;

		if (instance->GetData(DATA_SLAVE_OUTRO_GARFROST) == IN_PROGRESS)
		{
			instance->SetData(DATA_SLAVE_OUTRO_GARFROST, DONE);

			std::list<Creature*> Rescued1;
			GetCreatureListWithEntryInGrid(Rescued1, player, NPC_RESCUED_SLAVE_HORDE, 150.0f);
			for (std::list<Creature*>::iterator itr = Rescued1.begin(); itr != Rescued1.end(); ++itr)
			{
				Creature* rescued = *itr;
				if (!rescued)
					continue;

				if (rescued->IsAlive())
					rescued->GetMotionMaster()->MovePoint(0, 840.661987f, 5.974489f, 510.107910f);
			}

			if (Creature* rSlave = player->FindCreatureWithDistance(NPC_RESCUED_SLAVE_ALLIANCE, 150.0f, true))
			{
				DoSendQuantumText(SAY_RESCUED_HORDE_ALLIANCE, rSlave);
				rSlave->GetMotionMaster()->MovePoint(0, 831.654968f, 6.049870f, 509.910583f); // not correct
			}
			return true;
		}
		return false;
	}
};

class at_geist_ambusher_pos : public AreaTriggerScript
{
public:
	at_geist_ambusher_pos() : AreaTriggerScript("at_geist_ambusher_pos") { }

	bool OnTrigger(Player* player, AreaTriggerEntry const* areaTrigger)
	{
		InstanceScript* instance = player->GetInstanceScript();

		if (player->IsGameMaster() || !instance)
			return false;

		if (instance->GetData(DATA_SLAVE_OUTRO_GARFROST) == DONE)
		{
			std::list<Creature*> Geist;
			GetCreatureListWithEntryInGrid(Geist, player, NPC_GEIST_AMBUSHER, 300.0f);
			for (std::list<Creature*>::iterator itr = Geist.begin(); itr != Geist.end(); ++itr)
			{
				Creature* geist = *itr;
				if (!geist)
					continue;

				if (geist->IsAlive())
					geist->GetMotionMaster()->MoveJump(835.122620f, 1.335451f, 509.846619f, 30.0f, 20.0f);
			} 
			return true;
		}
		return false;
	}
};

class npc_pos_outro : public CreatureScript
{
public:
    npc_pos_outro() : CreatureScript("npc_pos_outro") { }

    struct pos_outroAI : public QuantumBasicAI
    {
        pos_outroAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
			Reset();

			me->SetWalk(true);
			me->SetSpeed(MOVE_WALK, 2.5f, true);
			me->SetSpeed(MOVE_RUN, 2.5f, true);
        }

		InstanceScript* instance;

        uint32 IntroTimer1;

        uint8 IntroPhase1;

        std::list<Creature*> Champions1;
        std::list<Creature*> Champions2;
        std::list<Creature*> Champions3;

        void Reset()
        {
			IntroTimer1 = 0;
            IntroPhase1 = 0;
            me->SetVisible(false);
            Champions1.clear();
            Champions2.clear();
            Champions3.clear();

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(const uint32 diff)
        {
            if (instance->GetBossState(DATA_TYRANNUS) == DONE)
            {
                if (IntroTimer1 <= diff)
                {
                    switch (IntroPhase1)
                    {
                        case 0:
                            if (Creature* GorkunOrVictus = me->GetCreature(*me, instance->GetData64(DATA_VICTUS_OR_GORKUN_FREED)))
                                GorkunOrVictus->GetMotionMaster()->MovePoint(0, leaderPosOutro2);

                            if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                            {
                                uint8 i = 0;
                                GetCreatureListWithEntryInGrid(Champions1, me, NPC_FREED_SLAVE_3_ALLIANCE, 100.0f);
                                for (std::list<Creature*>::iterator itr = Champions1.begin(); itr != Champions1.end(); ++itr)
                                {
                                    Creature* slave = *itr;
                                    if (!slave)
                                        continue;

                                    if (slave->IsAlive())
                                        slave->GetMotionMaster()->MovePoint(0, MoveLocations2[i]);
                                    ++i;
                                }

                                i = 4;
                                GetCreatureListWithEntryInGrid(Champions2, me, NPC_FREED_SLAVE_1_ALLIANCE, 100.0f);
                                for (std::list<Creature*>::iterator itr = Champions2.begin(); itr != Champions2.end(); ++itr)
                                {
                                    Creature* slave = *itr;
                                    if (!slave)
                                        continue;

                                    if (slave->IsAlive())
                                        slave->GetMotionMaster()->MovePoint(0, MoveLocations2[i]);
                                    ++i;
                                }

                                i = 7;
                                GetCreatureListWithEntryInGrid(Champions3, me, NPC_FREED_SLAVE_2_ALLIANCE, 100.0f);
                                for (std::list<Creature*>::iterator itr = Champions3.begin(); itr != Champions3.end(); ++itr)
                                {
                                    Creature* slave = *itr;
                                    if (!slave)
                                        continue;

                                    if (slave->IsAlive())
                                        slave->GetMotionMaster()->MovePoint(0, MoveLocations2[i]);
                                    ++i;
                                }
                            }
                            else
                            {
                                uint8 i = 0;
                                GetCreatureListWithEntryInGrid(Champions1, me, NPC_FREED_SLAVE_3_HORDE, 100.0f);
                                for (std::list<Creature*>::iterator itr = Champions1.begin(); itr != Champions1.end(); ++itr)
                                {
                                    Creature* slave = *itr;
                                    if (!slave)
                                        continue;

                                    if (slave->IsAlive())
                                        slave->GetMotionMaster()->MovePoint(0, MoveLocations2[i]);
                                    ++i;
                                }

                                i = 4;
                                GetCreatureListWithEntryInGrid(Champions2, me, NPC_FREED_SLAVE_1_HORDE, 100.0f);
                                for (std::list<Creature*>::iterator itr = Champions2.begin(); itr != Champions2.end(); ++itr)
                                {
                                    Creature* slave = *itr;
                                    if (!slave)
                                        continue;

                                    if (slave->IsAlive())
                                        slave->GetMotionMaster()->MovePoint(0, MoveLocations2[i]);
                                    ++i;
                                }

                                i = 7;
                                GetCreatureListWithEntryInGrid(Champions3, me, NPC_FREED_SLAVE_2_HORDE, 100.0f);
                                for (std::list<Creature*>::iterator itr = Champions3.begin(); itr != Champions3.end(); ++itr)
                                {
                                    Creature* slave = *itr;
                                    if (!slave)
                                        continue;

                                    if (slave->IsAlive())
                                        slave->GetMotionMaster()->MovePoint(0, MoveLocations2[i]);
                                    ++i;
                                }
                            }
                            ++IntroPhase1;
                            IntroTimer1 = 15000;
                            break;
                        case 1:
                            if (Creature* GorkunOrVictus = me->GetCreature(*me, instance->GetData64(DATA_VICTUS_OR_GORKUN_FREED)))
                                DoSendQuantumText(SAY_GORKUN_OUTRO_1, GorkunOrVictus);
                            ++IntroPhase1;
                            IntroTimer1 = 15000;
                            break;
                        case 2:
                            if (Creature* GorkunOrVictus = me->GetCreature(*me, instance->GetData64(DATA_VICTUS_OR_GORKUN_FREED)))
                                DoSendQuantumText(SAY_GORKUN_OUTRO_2, GorkunOrVictus);
                            ++IntroPhase1;
                            IntroTimer1 = 5000;
                            break;
                        case 3:
                            me->SetVisible(true);
                            if (Creature* sindragosa = me->SummonCreature(NPC_SINDRAGOSA, sindraPos, TEMPSUMMON_CORPSE_DESPAWN, 3000))
							{
								sindragosa->SetCanFly(true);
								sindragosa->SetUnitMovementFlags(MOVEMENTFLAG_FLYING);
								sindragosa->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
							}
                            ++IntroPhase1;
                            IntroTimer1 = 5000;
                            break;
                        case 4:
                            if (Creature* portal = me->SummonCreature(NPC_WORLD_TRIGGER, triggerPos1, TEMPSUMMON_TIMED_DESPAWN, 30000))
                            {
                                portal->CastSpell(portal, SPELL_ARTHAS_PORTAL, false);
                                portal->SetDisplayId(DISPLAY_ID_PORTAL);
                            }
                            if (Creature* portal = me->SummonCreature(NPC_WORLD_TRIGGER, triggerPos2, TEMPSUMMON_TIMED_DESPAWN, 30000))
                            {
                                portal->CastSpell(portal, SPELL_ARTHAS_PORTAL, false);
                                portal->SetDisplayId(DISPLAY_ID_PORTAL);
                            }
                            if (me->GetEntry() == NPC_SYLVANAS_PART2)
                                DoSendQuantumText(SAY_SYLVANAS_OUTRO_3, me);
                            else
                                DoSendQuantumText(SAY_JAYNA_OUTRO_3, me);

                            if (instance)
                            {
                                Map* map = me->GetMap();
                                if (!map)
                                    return;

                                Map::PlayerList const &lPlayers = map->GetPlayers();
                                for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                                {
                                    if (!itr->getSource()->IsAlive())
                                        continue;
                                    me->CastSpell(itr->getSource(), SPELL_BLINK_VISUAL, true);
                                    itr->getSource()->NearTeleportTo(1065.114746f, 96.392105f, 630.999573f, 2.063386f);
                                }
                            }
                            if (Creature* GorkunOrVictus = me->GetCreature(*me, instance->GetData64(DATA_VICTUS_OR_GORKUN_FREED)))
                                if (Creature* sindragosa = me->GetCreature(*me, instance->GetData64(DATA_SINDRAGOSA)))
                                    sindragosa->CastSpell(GorkunOrVictus, SPELL_FROST_BOMB, true);

                            if (Creature* GorkunOrVictus = me->GetCreature(*me, instance->GetData64(DATA_VICTUS_OR_GORKUN_FREED)))
                                if (GorkunOrVictus->IsAlive() && GorkunOrVictus->IsInWorld())
                                    me->Kill(GorkunOrVictus, false);

                            for (std::list<Creature*>::iterator itr = Champions1.begin(); itr != Champions1.end(); ++itr)
                            {
                                Creature* slave = *itr;
                                if (!slave)
                                    continue;

                                if (slave->IsAlive() && slave->IsInWorld())
                                    slave->Kill(slave, false);
                            }

                            for (std::list<Creature*>::iterator itr = Champions2.begin(); itr != Champions2.end(); ++itr)
                            {
                                Creature* slave = *itr;
                                if (!slave)
                                    continue;

                                if (slave->IsAlive() && slave->IsInWorld())
                                    slave->Kill(slave, false);
                            }

                            for (std::list<Creature*>::iterator itr = Champions3.begin(); itr != Champions3.end(); ++itr)
                            {
                                Creature* slave = *itr;
                                if (!slave)
                                    continue;

                                if (slave->IsAlive() && slave->IsInWorld())
                                    slave->Kill(slave, false);
                            }
                            ++IntroPhase1;
                            IntroTimer1 = 5000;
                            break;
                        case 5:
                            if (Creature* sindragosa = me->GetCreature(*me, instance->GetData64(DATA_SINDRAGOSA)))
                                sindragosa->GetMotionMaster()->MoveCharge(804.957214f, 102.497406f, 728.966370f, 42.00f, 0);

                            if (me->GetEntry() == NPC_SYLVANAS_PART2)
                                DoSendQuantumText(SAY_SYLVANAS_OUTRO_4, me);
                            else
                                DoSendQuantumText(SAY_JAYNA_OUTRO_4, me);
                            ++IntroPhase1;
                            IntroTimer1 = 7000;
                            break;
                        case 6:
                            if (me->GetEntry() == NPC_JAINA_PART2)
                                DoSendQuantumText(SAY_JAYNA_OUTRO_5, me);
                            ++IntroPhase1;
                            IntroTimer1 = 7000;
                            break;
                        case 7:
							if (me->GetEntry() == NPC_SYLVANAS_PART2)
							{
								me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
								me->GetMotionMaster()->MovePath(me->GetEntry(), false);
							}
							else
							{
								if (me->GetEntry() == NPC_JAINA_PART2)
								{
									me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
									me->GetMotionMaster()->MovePath(me->GetEntry(), false);
								}
							}
							++IntroPhase1;
                            IntroTimer1 = 30000;
                            break;
                        case 8:
							if (me->GetEntry() == NPC_SYLVANAS_PART2)
								me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
							else
								if (me->GetEntry() == NPC_JAINA_PART2)
									me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

							instance->HandleGameObject(instance->GetData64(GO_HALLS_OF_REFLECT_PORT), true);
                            ++IntroPhase1;
                            IntroTimer1 = 25000;
                            break;                            
							return;					
                    }
                }
                else IntroTimer1 -= diff;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new pos_outroAI(creature);
    }
};

class npc_icicle_pos_trigger : public CreatureScript
{
public:
    npc_icicle_pos_trigger() : CreatureScript("npc_icicle_pos_trigger") { }

    struct npc_icicle_pos_triggerAI : public QuantumBasicAI
    {
		npc_icicle_pos_triggerAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = (InstanceScript*)creature->GetInstanceScript();
            Reset();
        }

		InstanceScript* instance;

		uint32 CollapsTimer;

		void Reset()
		{ 
			CollapsTimer = urand(1000, 10000);
		}

		void AttackStart(Unit* /*who*/) {}

		void UpdateAI(const uint32 diff)
		{
			if (!instance)
				return;

			if (instance->GetData(DATA_AREA_TRIGGER_ICE_CICLE) == IN_PROGRESS)
			{
				if (CollapsTimer <= diff)
				{
					DoCast(me, SPELL_ICICLE, false);
					CollapsTimer = urand(12000, 20000);
				}
				else CollapsTimer -= diff;
			}
			return;
		}
	};

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_icicle_pos_triggerAI(creature);
    }
};

class npc_icicle_pos : public CreatureScript
{
public:
    npc_icicle_pos() : CreatureScript("npc_icicle_pos") { }

    struct npc_icicle_posAI : public QuantumBasicAI
    {
		npc_icicle_posAI(Creature* creature) : QuantumBasicAI(creature)
        {
            Reset();
        }

        uint32 CollapsTimer;

		void Reset()
		{ 
			me->DespawnAfterAction(8*IN_MILLISECONDS);

			CollapsTimer = 1*IN_MILLISECONDS;
		}

		void UpdateAI(const uint32 diff)
		{
			if (CollapsTimer <= diff)
			{
				DoCast(me, SPELL_ICICLE_1, true);
				DoCast(me, SPELL_ICICLE_2, false);
				CollapsTimer = 20*IN_MILLISECONDS;
			}
			else CollapsTimer -= diff;
		}
	};

	CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_icicle_posAI(creature);
    }
};

class npc_scourgelord_tyrannus_iao : public CreatureScript
{
public:
    npc_scourgelord_tyrannus_iao() : CreatureScript("npc_scourgelord_tyrannus_iao") { }

    struct npc_scourgelord_tyrannus_iaoAI : public QuantumBasicAI
    {
		npc_scourgelord_tyrannus_iaoAI(Creature* creature) : QuantumBasicAI(creature){}

		void Reset()
		{
			me->Mount(MOUNT_ID_TYRANNUS_INTRO);

			me->SetCanFly(true);

			me->SetUnitMovementFlags(MOVEMENTFLAG_FLYING);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void UpdateAI(const uint32 /*diff*/){}
	};

	CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_scourgelord_tyrannus_iaoAI(creature);
    }
};

void AddSC_pit_of_saron()
{
	new npc_pos_intro();
    new at_ymirjar_flamebearer_pos();
    new at_fallen_warrior_pos();
    new at_ice_cicle_pos();
    new at_slave_rescued_pos();
    new at_geist_ambusher_pos();
    new npc_pos_outro();
    new npc_icicle_pos();
    new npc_icicle_pos_trigger();
	new npc_scourgelord_tyrannus_iao();
}