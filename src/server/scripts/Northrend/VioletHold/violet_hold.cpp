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
#include "QuantumEscortAI.h"
#include "QuantumGossip.h"
#include "violet_hold.h"

#define GOSSIP_START_EVENT "Get your people to safety, we'll keep the Blue Dragonflight's forces at bay."
#define GOSSIP_ITEM_1      "Activate the crystals when we get in trouble, right"
#define GOSSIP_I_WANT_IN   "I'm not fighting, so send me in now!"

enum TrashSpells
{
    SPELL_CLEAVE                = 15496,
    SPELL_IMPALE                = 58459,
    H_SPELL_IMPALE              = 59256,
    SPELL_BRUTAL_STRIKE         = 58460,
    SPELL_SUNDER_ARMOR          = 58461,
    SPELL_ARCANE_BLAST          = 58462,
    H_SPELL_ARCANE_BLAST        = 59257,
    SPELL_SLOW                  = 25603,
    SPELL_CHAINS_OF_ICE         = 58464,
    SPELL_CONE_OF_COLD          = 58463,
    H_SPELL_CONE_OF_COLD        = 59258,
    SPELL_ARCANE_BARRAGE        = 58456,
    H_SPELL_ARCANE_BARRAGE      = 59248,
    SPELL_ARCANE_EXPLOSION      = 58455,
    H_SPELL_ARCANE_EXPLOSION    = 59245,
    SPELL_FROST_NOVA            = 58458,
    H_SPELL_FROST_NOVA          = 59253,
    SPELL_FROSTBOLT             = 58457,
    H_SPELL_FROSTBOLT           = 59251,
    SPELL_ARCANE_EMPOWERMENT    = 58469,
    SPELL_SPELL_LOCK            = 30849,
    SPELL_MORTAL_STRIKE         = 32736,
    SPELL_WHIRLWIND_OF_STEEL    = 41057,
    SPELL_ARCANE_STREAM         = 60181,
    H_SPELL_ARCANE_STREAM       = 60204,
    SPELL_MANA_DETONATION       = 60182,
    H_SPELL_MANA_DETONATION     = 60205,
    SPELL_CONCUSSION_BLOW       = 52719,
    SPELL_MAGIC_REFLECTION      = 60158,
    SPELL_BACKSTAB              = 58471,
    SPELL_TACTICAL_BLINK        = 58470,
    SABOTEUR_SHIELD_DISRUPTION  = 58291,
    SABOTEUR_SHIELD_EFFECT      = 45775,
    SPELL_DESTROY_DOOR_SEAL     = 58040,
	SPELL_DEFENSE_SYSTEM_VISUAL = 57887,
};

float FirstPortalWPs[6][3] =
{
    {1877.670288f, 842.280273f, 43.333591f},
    {1877.338867f, 834.615356f, 38.762287f},
    {1872.161011f, 823.854309f, 38.645401f},
    {1864.860474f, 815.787170f, 38.784843f},
    {1858.953735f, 810.048950f, 44.008759f},
    {1843.707153f, 805.807739f, 44.135197f},
    //{1825.736084f, 807.305847f, 44.363785f},
};

float SecondPortalFirstWPs [9][3] =
{
    {1902.561401f, 853.334656f, 47.106117f},
    {1895.486084f, 855.376404f, 44.334591f},
    {1882.805176f, 854.993286f, 43.333591f},
    {1877.670288f, 842.280273f, 43.333591f},
    {1877.338867f, 834.615356f, 38.762287f},
    {1872.161011f, 823.854309f, 38.645401f},
    {1864.860474f, 815.787170f, 38.784843f},
    {1858.953735f, 810.048950f, 44.008759f},
    {1843.707153f, 805.807739f, 44.135197f},
    //{1825.736084f, 807.305847f, 44.363785f},
};

float SecondPortalSecondWPs [8][3] =
{
    {1929.392212f, 837.614990f, 47.136166f},
    {1928.290649f, 824.750427f, 45.474411f},
    {1915.544922f, 826.919373f, 38.642811f},
    {1900.933960f, 818.855652f, 38.801647f},
    {1886.810547f, 813.536621f, 38.490490f},
    {1869.079712f, 808.701538f, 38.689003f},
    {1860.843384f, 806.645020f, 44.008789f},
    {1843.707153f, 805.807739f, 44.135197f},
    //{1825.736084f, 807.305847f, 44.363785f}
};

float ThirdPortalWPs [8][3] =
{
    {1934.049438f, 815.778503f, 52.408699f},
    {1928.290649f, 824.750427f, 45.474411f},
    {1915.544922f, 826.919373f, 38.642811f},
    {1900.933960f, 818.855652f, 38.801647f},
    {1886.810547f, 813.536621f, 38.490490f},
    {1869.079712f, 808.701538f, 38.689003f},
    {1860.843384f, 806.645020f, 44.008789f},
    {1843.707153f, 805.807739f, 44.135197f},
    //{1825.736084f, 807.305847f, 44.363785f},
};

float FourthPortalWPs [9][3] =
{
    {1921.658447f, 761.657043f, 50.866741f},
    {1910.559814f, 755.780457f, 47.701447f},
    {1896.664673f, 752.920898f, 47.667004f},
    {1887.398804f, 763.633240f, 47.666851f},
    {1879.020386f, 775.396973f, 38.705990f},
    {1872.439087f, 782.568604f, 38.808292f},
    {1863.573364f, 791.173584f, 38.743660f},
    {1857.811890f, 796.765564f, 43.950329f},
    {1845.577759f, 800.681152f, 44.104248f},
    //{1827.100342f, 801.605957f, 44.363358f},
};

float FifthPortalWPs [6][3] =
{
    {1887.398804f, 763.633240f, 47.666851f},
    {1879.020386f, 775.396973f, 38.705990f},
    {1872.439087f, 782.568604f, 38.808292f},
    {1863.573364f, 791.173584f, 38.743660f},
    {1857.811890f, 796.765564f, 43.950329f},
    {1845.577759f, 800.681152f, 44.104248f},
    //{1827.100342f, 801.605957f, 44.363358f},
};

float SixthPoralWPs [4][3] =
{
    {1888.861084f, 805.074768f, 38.375790f},
    {1869.793823f, 804.135804f, 38.647018f},
    {1861.541504f, 804.149780f, 43.968292f},
    {1843.567017f, 804.288208f, 44.139091f},
    //{1826.889648f, 803.929993f, 44.363239f},
};

const float SaboteurFinalPos1[3][3] =
{
    {1892.502319f, 777.410767f, 38.630402f},
    {1891.165161f, 762.969421f, 47.666920f},
    {1893.168091f, 740.919189f, 47.666920f},
};

const float SaboteurFinalPos2[3][3] =
{
    {1882.242676f, 834.818726f, 38.646786f},
    {1879.220825f, 842.224854f, 43.333641f},
    {1873.842896f, 863.892456f, 43.333641f},
};

const float SaboteurFinalPos3[2][3] =
{
    {1904.298340f, 792.400391f, 38.646782f},
    {1935.716919f, 758.437073f, 30.627895f},
};

const float SaboteurFinalPos4[3] =
{
    1855.006104f, 760.641724f, 38.655266f,
};

const float SaboteurFinalPos5[3] =
{
    1906.667358f, 841.705566f, 38.637894f,
};

const float SaboteurFinalPos6[5][3] =
{
    {1911.437012f, 821.289246f, 38.684128f},
    {1920.734009f, 822.978027f, 41.525414f},
    {1928.262939f, 830.836609f, 44.668266f},
    {1929.338989f, 837.593933f, 47.137596f},
    {1931.063354f, 848.468445f, 47.190434f},
 };

const Position MovePosition = {1806.955566f, 803.851807f, 44.363323f, 0.0f};
const Position playerTeleportPosition = {1830.531006f, 803.939758f, 44.340508f, 6.281611f};
const Position sinclariOutsidePosition = {1817.315674f, 804.060608f, 44.363998f, 0.0f};

class npc_sinclari_vh : public CreatureScript
{
public:
    npc_sinclari_vh() : CreatureScript("npc_sinclari_vh") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (InstanceScript* instance = creature->GetInstanceScript())
        {
            switch (instance->GetData(DATA_MAIN_EVENT_PHASE))
            {
                case NOT_STARTED:
				case FAIL: // Allow to start event if not started or wiped and door integrity is bigger than 0
					if (instance->GetData(DATA_DOOR_INTEGRITY) > 0)
					{
						player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
						player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
						player->SEND_GOSSIP_MENU(13853, creature->GetGUID());
					}
                    break;
                case IN_PROGRESS: // Allow to teleport inside if event is in progress
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_I_WANT_IN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
                    player->SEND_GOSSIP_MENU(13853, creature->GetGUID());
                    break;
                default:
                    player->SEND_GOSSIP_MENU(13910, creature->GetGUID());
            }
        }
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->CLOSE_GOSSIP_MENU();
                CAST_AI(npc_sinclari_vh::npc_sinclariAI, (creature->AI()))->Phase = 1;
                if (InstanceScript* instance = creature->GetInstanceScript())
                    instance->SetData(DATA_MAIN_EVENT_PHASE, SPECIAL);
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->SEND_GOSSIP_MENU(13854, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->NearTeleportTo(playerTeleportPosition.GetPositionX(), playerTeleportPosition.GetPositionY(), playerTeleportPosition.GetPositionZ(), playerTeleportPosition.GetOrientation(), true);
                player->CLOSE_GOSSIP_MENU();
                break;
        }
        return true;
    }

    struct npc_sinclariAI : public QuantumBasicAI
    {
        npc_sinclariAI(Creature* creature) : QuantumBasicAI(creature)
        {
           instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint8 Phase;
        uint32 Timer;

        void Reset()
        {
            Phase = 0;
            Timer = 0;

            me->SetReactState(REACT_AGGRESSIVE);

            std::list<Creature*> GuardList;
            me->GetCreatureListWithEntryInGrid(GuardList, NPC_VIOLET_HOLD_GUARD, 40.0f);
            if (!GuardList.empty())
            {
                for (std::list<Creature*>::const_iterator itr = GuardList.begin(); itr != GuardList.end(); ++itr)
                {
                    if (Creature* Guard = *itr)
                    {
                        Guard->DisappearAndDie();
                        Guard->Respawn();
                        Guard->SetVisible(true);
                        Guard->SetReactState(REACT_AGGRESSIVE);
                    }
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            QuantumBasicAI::UpdateAI(diff);

            if (Phase)
            {
                if (Timer <= diff)
                {
                    switch (Phase)
                    {
                        case 1:
                            DoSendQuantumText(SAY_SINCLARI_1, me);
                            Timer = 4000;
                            Phase = 2;
                            break;
                        case 2:
                        {
                            std::list<Creature*> GuardList;
                            me->GetCreatureListWithEntryInGrid(GuardList, NPC_VIOLET_HOLD_GUARD, 40.0f);
                            if (!GuardList.empty())
                                for (std::list<Creature*>::const_iterator itr = GuardList.begin(); itr != GuardList.end(); ++itr)
                                {
                                    if (Creature* Guard = *itr)
                                    {
										Guard->SetWalk(false);
                                        Guard->GetMotionMaster()->MovePoint(0, MovePosition);
                                    }
								}

								Timer = 6000;
								Phase = 3;
								break;
                        }
                        case 3:
                        {
                            std::list<Creature*> GuardList;
                            me->GetCreatureListWithEntryInGrid(GuardList, NPC_VIOLET_HOLD_GUARD, 40.0f);
                            if (!GuardList.empty())
                                for (std::list<Creature*>::const_iterator itr = GuardList.begin(); itr != GuardList.end(); ++itr)
                                {
                                    if (Creature* Guard = *itr)
                                    {
                                        Guard->SetVisible(false);
                                        Guard->SetReactState(REACT_PASSIVE);
                                    }
                                }
								Timer = 2000;
								Phase = 4;
								break;
						}
                        case 4:
                            me->GetMotionMaster()->MovePoint(0, sinclariOutsidePosition);
                            Timer = 4000;
                            Phase = 5;
                            break;
                        case 5:
							instance->SetData(DATA_MAIN_EVENT_PHASE, IN_PROGRESS);
                            me->SetReactState(REACT_PASSIVE);
							Timer = 0;
                            Phase = 0;
                            break;
                    }
                }
                else Timer -= diff;
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sinclariAI(creature);
    }
};

class mob_azure_saboteur : public CreatureScript
{
public:
    mob_azure_saboteur() : CreatureScript("mob_azure_saboteur") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_azure_saboteurAI(creature);
    }

    struct mob_azure_saboteurAI : public npc_escortAI
    {
        mob_azure_saboteurAI(Creature* creature) : npc_escortAI(creature)
        {
            instance = creature->GetInstanceScript();
            bHasGotMovingPoints = false;
            Boss = 0;
            Reset();
        }

        InstanceScript* instance;

        bool bHasGotMovingPoints;

        uint32 Boss;

        void AttackStart(Unit* /*who*/){}

        void MoveInLineOfSight(Unit* /*who*/){}

        void Reset()
        {
            if (instance && !Boss)
                Boss = instance->GetData(DATA_WAVE_COUNT) == 6 ? instance->GetData(DATA_FIRST_BOSS) : instance->GetData(DATA_SECOND_BOSS);
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NOT_SELECTABLE);
        }

        void WaypointReached(uint32 WPointId)
        {
            switch (Boss)
            {
                case 1:
                    if (WPointId == 2)
                        FinishPointReached();
                    break;
                case 2:
                    if (WPointId == 2)
                        FinishPointReached();
                    break;
                case 3:
                    if (WPointId == 1)
                        FinishPointReached();
                    break;
                case 4:
                    if (WPointId == 0)
                        FinishPointReached();
                    break;
                case 5:
                    if (WPointId == 0)
                        FinishPointReached();
                    break;
                case 6:
                    if (WPointId == 4)
                        FinishPointReached();
                    break;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (instance && instance->GetData(DATA_MAIN_EVENT_PHASE) != IN_PROGRESS)
                me->CastStop();

            npc_escortAI::UpdateAI(diff);

            if (!bHasGotMovingPoints)
            {
                bHasGotMovingPoints = true;
                switch (Boss)
                {
                    case 1:
                        for (int i=0;i<3;i++)
                            AddWaypoint(i, SaboteurFinalPos1[i][0], SaboteurFinalPos1[i][1], SaboteurFinalPos1[i][2], 0);
                        me->SetHomePosition(SaboteurFinalPos1[2][0], SaboteurFinalPos1[2][1], SaboteurFinalPos1[2][2], 4.762346f);
                        break;
                    case 2:
                        for (int i=0;i<3;i++)
                            AddWaypoint(i, SaboteurFinalPos2[i][0], SaboteurFinalPos2[i][1], SaboteurFinalPos2[i][2], 0);
                        me->SetHomePosition(SaboteurFinalPos2[2][0], SaboteurFinalPos2[2][1], SaboteurFinalPos2[2][2], 1.862674f);
                        break;
                    case 3:
                        for (int i=0;i<2;i++)
                            AddWaypoint(i, SaboteurFinalPos3[i][0], SaboteurFinalPos3[i][1], SaboteurFinalPos3[i][2], 0);
                        me->SetHomePosition(SaboteurFinalPos3[1][0], SaboteurFinalPos3[1][1], SaboteurFinalPos3[1][2], 5.500638f);
                        break;
                    case 4:
                        AddWaypoint(0, SaboteurFinalPos4[0], SaboteurFinalPos4[1], SaboteurFinalPos4[2], 0);
                        me->SetHomePosition(SaboteurFinalPos4[0], SaboteurFinalPos4[1], SaboteurFinalPos4[2], 3.991108f);
                        break;
                    case 5:
                        AddWaypoint(0, SaboteurFinalPos5[0], SaboteurFinalPos5[1], SaboteurFinalPos5[2], 0);
                        me->SetHomePosition(SaboteurFinalPos5[0], SaboteurFinalPos5[1], SaboteurFinalPos5[2], 1.100841f);
                        break;
                    case 6:
                        for (int i=0;i<5;i++)
                            AddWaypoint(i, SaboteurFinalPos6[i][0], SaboteurFinalPos6[i][1], SaboteurFinalPos6[i][2], 0);
                        me->SetHomePosition(SaboteurFinalPos6[4][0], SaboteurFinalPos6[4][1], SaboteurFinalPos6[4][2], 0.983031f);
                        break;
                }

                SetDespawnAtEnd(false);
                Start(true, true);
            }
        }

        void FinishPointReached()
        {
            me->CastSpell(me, SABOTEUR_SHIELD_DISRUPTION, false);
            me->DisappearAndDie();
            Creature* SaboPort = Unit::GetCreature(*me, instance->GetData64(DATA_SABOTEUR_PORTAL));
            if (SaboPort)
                SaboPort->DisappearAndDie();
            instance->SetData(DATA_START_BOSS_ENCOUNTER, 1);
        }
    };
};

class npc_teleportation_portal_vh : public CreatureScript
{
public:
    npc_teleportation_portal_vh() : CreatureScript("npc_teleportation_portal_vh") { }

    struct npc_teleportation_portalAI : public QuantumBasicAI
    {
        npc_teleportation_portalAI(Creature* creature) : QuantumBasicAI(creature), listOfMobs(me)
        {
            instance = creature->GetInstanceScript();
            TypeOfMobsPortal = urand(0, 1);    // 0 - elite mobs   1 - portal guardian or portal keeper with regular mobs
            bPortalGuardianOrKeeperOrEliteSpawn = false;
        }

        uint32 SpawnTimer;
        bool bPortalGuardianOrKeeperOrEliteSpawn;
        uint8 TypeOfMobsPortal;

        SummonList listOfMobs;

        InstanceScript* instance;

        void Reset()
        {
            SpawnTimer = 10000;
            bPortalGuardianOrKeeperOrEliteSpawn = false;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void MoveInLineOfSight(Unit* /*who*/) {}

        void UpdateAI(uint32 const diff)
        {
            if (!instance) // Massive usage of instance, global check
                return;

            if (instance->GetData(DATA_REMOVE_NPC) == 1)
            {
				listOfMobs.DespawnAll();
                me->DespawnAfterAction();
				instance->SetData(DATA_REMOVE_NPC, 0);
            }

            uint8 WaveCount = instance->GetData(DATA_WAVE_COUNT);
            if ((WaveCount == 6) || (WaveCount == 12)) // Don't spawn mobs on boss encounters
                return;

            switch (TypeOfMobsPortal)
            {
                // spawn elite mobs and then set portals visibility to make it look like it dissapeard
                case 0:
                    if (!bPortalGuardianOrKeeperOrEliteSpawn)
                    {
                        if (SpawnTimer <= diff)
                        {
                            bPortalGuardianOrKeeperOrEliteSpawn = true;
                            uint8 k = WaveCount < 12 ? 2 : 3;
                            for (uint8 i = 0; i < k; ++i)
                            {
                                uint32 entry = RAND(NPC_AZURE_CAPTAIN, NPC_AZURE_RAIDER, NPC_AZURE_STALKER, NPC_AZURE_SORCEROR);
                                DoSummon(entry, me, 2.0f, 20000, TEMPSUMMON_DEAD_DESPAWN);
                            }
                            me->SetVisible(false);
                        }
						else SpawnTimer -= diff;
                    }
                    else
                    {
                        // if all spawned elites have died kill portal
                        if (listOfMobs.empty())
                        {
                            me->Kill(me, false);
                            me->RemoveCorpse();
                        }
                    }
                    break;
                // spawn portal guardian or portal keeper with regular mobs
                case 1:
                    if (SpawnTimer <= diff)
                    {
                        if (bPortalGuardianOrKeeperOrEliteSpawn)
                        {
                            uint8 k = instance->GetData(DATA_WAVE_COUNT) < 12 ? 3 : 4;
                            for (uint8 i = 0; i < k; ++i)
                            {
                                uint32 entry = RAND(NPC_AZURE_INVADER_1, NPC_AZURE_INVADER_2, NPC_AZURE_SPELLBREAKER_1,
									NPC_AZURE_SPELLBREAKER_2, NPC_AZURE_MAGE_SLAYER_1,
									NPC_AZURE_MAGE_SLAYER_2, NPC_AZURE_BINDER_1, NPC_AZURE_BINDER_2);
                                DoSummon(entry, me, 2.0f, 20000, TEMPSUMMON_DEAD_DESPAWN);
                            }
                        }
                        else
                        {
                            bPortalGuardianOrKeeperOrEliteSpawn = true;
                            uint32 entry = RAND(NPC_PORTAL_GUARDIAN, NPC_PORTAL_KEEPER);

                            if (Creature* PortalKeeper = DoSummon(entry, me, 2.0f, 0, TEMPSUMMON_DEAD_DESPAWN))
                                me->CastSpell(PortalKeeper, SPELL_PORTAL_CHANNEL, false);
                        }
                        SpawnTimer = SPAWN_TIME;
                    }
					else SpawnTimer -= diff;

                    if (bPortalGuardianOrKeeperOrEliteSpawn && !me->IsNonMeleeSpellCasted(false))
                    {
                        me->Kill(me, false);
                        me->RemoveCorpse();
                    }
                    break;
            }
        }

        void JustDied(Unit* /*killer*/)
        {
			instance->SetData(DATA_WAVE_COUNT, instance->GetData(DATA_WAVE_COUNT)+1);
        }

        void JustSummoned(Creature* summoned)
        {
            listOfMobs.Summon(summoned);
        }

        void SummonedMobDied(Creature* summoned)
        {
            listOfMobs.Despawn(summoned);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_teleportation_portalAI(creature);
    }
};

struct violet_hold_trashAI : public npc_escortAI
{
    violet_hold_trashAI(Creature* creature) : npc_escortAI(creature)
    {
        instance = creature->GetInstanceScript();

        bHasGotMovingPoints = false;
        isAtDoor = false;

		PortalLocationID = instance->GetData(DATA_PORTAL_LOCATION);

        Reset();
    }

    public:
        InstanceScript* instance;
        bool isAtDoor;
        bool bHasGotMovingPoints;
        uint32 PortalLocationID;
        uint32 SecondPortalRouteID;

    void WaypointReached(uint32 PointId)
    {
        switch (PortalLocationID)
        {
            case 0:
                if (PointId == 5)
                   CreatureStartAttackDoor();
                break;
            case 1:
                if ((PointId == 8 && SecondPortalRouteID == 0) || (PointId == 7 && SecondPortalRouteID == 1))
                    CreatureStartAttackDoor();
                break;
            case 2:
                if (PointId == 7)
                   CreatureStartAttackDoor();
                break;
            case 3:
                if (PointId == 8)
                    CreatureStartAttackDoor();
                break;
            case 4:
                if (PointId == 5)
                    CreatureStartAttackDoor();
                break;
            case 5:
                if (PointId == 3)
                    CreatureStartAttackDoor();
                break;
        }
    }

    void UpdateAI(uint32 const /*diff*/)
    {
        if (instance && instance->GetData(DATA_MAIN_EVENT_PHASE) != IN_PROGRESS)
            me->CastStop();

        if (!bHasGotMovingPoints)
        {
            bHasGotMovingPoints = true;
            switch (PortalLocationID)
            {
                case 0:
                    for (int i=0;i<6;i++)
                        AddWaypoint(i, FirstPortalWPs[i][0]+irand(-1, 1), FirstPortalWPs[i][1]+irand(-1, 1), FirstPortalWPs[i][2]+irand(-1, 1), 0);
                    me->SetHomePosition(FirstPortalWPs[5][0], FirstPortalWPs[5][1], FirstPortalWPs[5][2], 3.149439f);
                    break;
                case 1:
                    SecondPortalRouteID = urand(0, 1);
                    switch (SecondPortalRouteID)
                    {
                        case 0:
                            for (int i=0;i<9;i++)
                                AddWaypoint(i, SecondPortalFirstWPs[i][0]+irand(-1, 1), SecondPortalFirstWPs[i][1]+irand(-1, 1), SecondPortalFirstWPs[i][2], 0);
                            me->SetHomePosition(SecondPortalFirstWPs[8][0]+irand(-1, 1), SecondPortalFirstWPs[8][1]+irand(-1, 1), SecondPortalFirstWPs[8][2]+irand(-1, 1), 3.149439f);
                            break;
                        case 1:
                            for (int i=0;i<8;i++)
                                AddWaypoint(i, SecondPortalSecondWPs[i][0]+irand(-1, 1), SecondPortalSecondWPs[i][1]+irand(-1, 1), SecondPortalSecondWPs[i][2], 0);
                            me->SetHomePosition(SecondPortalSecondWPs[7][0], SecondPortalSecondWPs[7][1], SecondPortalSecondWPs[7][2], 3.149439f);
                            break;
                    }
                    break;
                case 2:
                    for (int i=0;i<8;i++)
                        AddWaypoint(i, ThirdPortalWPs[i][0]+irand(-1, 1), ThirdPortalWPs[i][1]+irand(-1, 1), ThirdPortalWPs[i][2], 0);
                        me->SetHomePosition(ThirdPortalWPs[7][0], ThirdPortalWPs[7][1], ThirdPortalWPs[7][2], 3.149439f);
                    break;
                case 3:
                    for (int i=0;i<9;i++)
                        AddWaypoint(i, FourthPortalWPs[i][0]+irand(-1, 1), FourthPortalWPs[i][1]+irand(-1, 1), FourthPortalWPs[i][2], 0);
                    me->SetHomePosition(FourthPortalWPs[8][0], FourthPortalWPs[8][1], FourthPortalWPs[8][2], 3.149439f);
                    break;
                case 4:
                    for (int i=0;i<6;i++)
                        AddWaypoint(i, FifthPortalWPs[i][0]+irand(-1, 1), FifthPortalWPs[i][1]+irand(-1, 1), FifthPortalWPs[i][2], 0);
                    me->SetHomePosition(FifthPortalWPs[5][0], FifthPortalWPs[5][1], FifthPortalWPs[5][2], 3.149439f);
                    break;
                case 5:
                    for (int i=0;i<4;i++)
                        AddWaypoint(i, SixthPoralWPs[i][0]+irand(-1, 1), SixthPoralWPs[i][1]+irand(-1, 1), SixthPoralWPs[i][2], 0);
                    me->SetHomePosition(SixthPoralWPs[3][0], SixthPoralWPs[3][1], SixthPoralWPs[3][2], 3.149439f);
                    break;
            }
            SetDespawnAtEnd(false);
            Start(true, true);
        }
    }

    void JustDied(Unit* /*unit*/)
    {
        if (Creature* portal = Unit::GetCreature(*me, instance->GetData64(DATA_TELEPORTATION_PORTAL)))
            CAST_AI(npc_teleportation_portal_vh::npc_teleportation_portalAI, portal->AI())->SummonedMobDied(me);

        if (instance && isAtDoor)
            instance->SetData(DATA_NPC_PRESENCE_AT_DOOR_REMOVE, 1);
    }

    void CreatureStartAttackDoor()
    {
        //me->SetReactState(REACT_PASSIVE);
        isAtDoor = true;
        DoCast(SPELL_DESTROY_DOOR_SEAL);

		instance->SetData(DATA_NPC_PRESENCE_AT_DOOR_ADD, 1);
    }
};

class mob_azure_invader : public CreatureScript
{
public:
    mob_azure_invader() : CreatureScript("mob_azure_invader") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_azure_invaderAI(creature);
    }

    struct mob_azure_invaderAI : public violet_hold_trashAI
    {
        mob_azure_invaderAI(Creature* creature) : violet_hold_trashAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 CleaveTimer;
        uint32 ImpaleTimer;
        uint32 BrutalStrikeTimer;
        uint32 SunderArmorTimer;

        void Reset()
        {
            CleaveTimer = 5000;
            ImpaleTimer = 4000;
            BrutalStrikeTimer = 5000;
            SunderArmorTimer = 4000;
        }

        void UpdateAI(uint32 const diff)
        {
            violet_hold_trashAI::UpdateAI(diff);
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            if (me->GetEntry() == NPC_AZURE_INVADER_1)
            {
                if (CleaveTimer <= diff)
                {
                    DoCastVictim(SPELL_CLEAVE);
                    CleaveTimer = 5000;
                }
				else CleaveTimer -= diff;

                if (ImpaleTimer <= diff)
                {
                    Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true);
                    if (target)
                        DoCast(target, SPELL_IMPALE);
                    ImpaleTimer = 4000;
                }
				else ImpaleTimer -= diff;
            }

            if (me->GetEntry() == NPC_AZURE_INVADER_2)
            {
                if (BrutalStrikeTimer <= diff)
                {
                    DoCastVictim(SPELL_BRUTAL_STRIKE);
                    BrutalStrikeTimer = 5000;
                }
				else BrutalStrikeTimer -= diff;

                if (SunderArmorTimer <= diff)
                {
                    DoCastVictim(SPELL_SUNDER_ARMOR);
                    SunderArmorTimer = urand(8000, 10000);
                }
				else SunderArmorTimer -= diff;
				
				DoMeleeAttackIfReady();
			}

            DoMeleeAttackIfReady();
        }
    };
};

class mob_azure_binder : public CreatureScript
{
public:
    mob_azure_binder() : CreatureScript("mob_azure_binder") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_azure_binderAI(creature);
    }

    struct mob_azure_binderAI : public violet_hold_trashAI
    {
        mob_azure_binderAI(Creature* creature) : violet_hold_trashAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 ArcaneExplosionTimer;
        uint32 ArcainBarrageTimer;
        uint32 FrostNovaTimer;
        uint32 FrostboltTimer;

        void Reset()
        {
            ArcaneExplosionTimer = 5000;
            ArcainBarrageTimer = 4000;
            FrostNovaTimer = 5000;
            FrostboltTimer = 4000;
        }

        void UpdateAI(uint32 const diff)
        {
            violet_hold_trashAI::UpdateAI(diff);
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            if (me->GetEntry() == NPC_AZURE_BINDER_1)
            {
                if (ArcaneExplosionTimer <= diff)
                {
                    DoCast(DUNGEON_MODE(SPELL_ARCANE_EXPLOSION, H_SPELL_ARCANE_EXPLOSION));
                    ArcaneExplosionTimer = 5000;
                }
				else ArcaneExplosionTimer -= diff;

				if (ArcainBarrageTimer <= diff)
                {
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
					{
						DoCast(target, DUNGEON_MODE(SPELL_ARCANE_BARRAGE, H_SPELL_ARCANE_BARRAGE));
						ArcainBarrageTimer = 6000;
					}
				}
				else ArcainBarrageTimer -= diff;
            }

            if (me->GetEntry() == NPC_AZURE_BINDER_2)
            {
                if (FrostNovaTimer <= diff)
                {
                    DoCast(DUNGEON_MODE(SPELL_FROST_NOVA, H_SPELL_FROST_NOVA));
                    FrostNovaTimer = 5000;
                }
				else FrostNovaTimer -= diff;

                if (FrostboltTimer <= diff)
                {
                    Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true);
                    if (target)
                        DoCast(target, DUNGEON_MODE(SPELL_FROSTBOLT, H_SPELL_FROSTBOLT));
                    FrostboltTimer = 6000;
                }
				else FrostboltTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };
};

class mob_azure_mage_slayer : public CreatureScript
{
public:
    mob_azure_mage_slayer() : CreatureScript("mob_azure_mage_slayer") { }

    struct mob_azure_mage_slayerAI : public violet_hold_trashAI
    {
        mob_azure_mage_slayerAI(Creature* creature) : violet_hold_trashAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 ArcaneEmpowermentTimer;
        uint32 SpellLockTimer;

        void Reset()
        {
            ArcaneEmpowermentTimer = 5000;
            SpellLockTimer = 5000;
        }

        void UpdateAI(uint32 const diff)
        {
            violet_hold_trashAI::UpdateAI(diff);
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            if (me->GetEntry() == NPC_AZURE_MAGE_SLAYER_1)
            {
                if (ArcaneEmpowermentTimer <= diff)
                {
                    DoCast(me, SPELL_ARCANE_EMPOWERMENT);
                        ArcaneEmpowermentTimer = 14000;
                }
				else ArcaneEmpowermentTimer -= diff;
            }

            if (me->GetEntry() == NPC_AZURE_MAGE_SLAYER_2)
            {
				if (SpellLockTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						if (target && target->GetPowerType() == POWER_MANA && target->HasUnitState(UNIT_STATE_CASTING))
						{
							DoCast(target, SPELL_SPELL_LOCK);
							SpellLockTimer = urand(7000, 8000);
						}
					}
				}
				else SpellLockTimer -= diff;
			}

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_azure_mage_slayerAI(creature);
    }
};

class mob_azure_raider : public CreatureScript
{
public:
    mob_azure_raider() : CreatureScript("mob_azure_raider") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_azure_raiderAI(creature);
    }

    struct mob_azure_raiderAI : public violet_hold_trashAI
    {
        mob_azure_raiderAI(Creature* creature) : violet_hold_trashAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 ConcussionBlowTimer;
        uint32 MagicReflectionTimer;

        void Reset()
        {
            ConcussionBlowTimer = 5000;
            MagicReflectionTimer = 8000;
        }

        void UpdateAI(uint32 const diff)
        {
            violet_hold_trashAI::UpdateAI(diff);
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            if (ConcussionBlowTimer <= diff)
            {
                DoCastVictim(SPELL_CONCUSSION_BLOW);
                ConcussionBlowTimer = 5000;
            }
			else ConcussionBlowTimer -= diff;

            if (MagicReflectionTimer <= diff)
            {
                DoCast(SPELL_MAGIC_REFLECTION);
                MagicReflectionTimer = urand(10000, 15000);
            }
			else MagicReflectionTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

class mob_azure_stalker : public CreatureScript
{
public:
    mob_azure_stalker() : CreatureScript("mob_azure_stalker") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_azure_stalkerAI(creature);
    }

    struct mob_azure_stalkerAI : public violet_hold_trashAI
    {
        mob_azure_stalkerAI(Creature* creature) : violet_hold_trashAI(creature)
        {
            instance = creature->GetInstanceScript();
        }
        uint32 BackstabTimer;
        uint32 TacticalBlinkTimer;
        bool TacticalBlinkCasted;

        void Reset()
        {
            BackstabTimer = 1300;
            TacticalBlinkTimer = 8000;
            TacticalBlinkCasted = false;
        }

        void UpdateAI(uint32 const diff)
        {
            violet_hold_trashAI::UpdateAI(diff);
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            if (!TacticalBlinkCasted)
            {
                if (TacticalBlinkTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 40, true))
                        DoCast(target, SPELL_TACTICAL_BLINK);
                        TacticalBlinkTimer = 6000;
                    TacticalBlinkCasted = true;
                }
				else TacticalBlinkTimer -= diff;
            }
            else
            {
                if (BackstabTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_NEAREST, 0, 10, true))
                        DoCast(target, SPELL_BACKSTAB);
                    TacticalBlinkCasted = false;
                    BackstabTimer =1300;
                }
				else BackstabTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };
};

class mob_azure_spellbreaker : public CreatureScript
{
public:
    mob_azure_spellbreaker() : CreatureScript("mob_azure_spellbreaker") { }

    struct mob_azure_spellbreakerAI : public violet_hold_trashAI
    {
        mob_azure_spellbreakerAI(Creature* creature) : violet_hold_trashAI(creature)
        {
             instance = creature->GetInstanceScript();
        }

        uint32 ArcaneBlastTimer;
        uint32 SlowTimer;
        uint32 ChainsOfIceTimer;
        uint32 ConeOfColdTimer;

        void Reset()
        {
            ArcaneBlastTimer = 5000;
            SlowTimer = 4000;
            ChainsOfIceTimer = 5000;
            ConeOfColdTimer = 4000;
        }

        void UpdateAI(uint32 const diff)
        {
            violet_hold_trashAI::UpdateAI(diff);
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            if (me->GetEntry() == NPC_AZURE_SPELLBREAKER_1)
            {
                if (ArcaneBlastTimer <= diff)
                {
                    Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true);
                    if (target)
                        DoCast(target, DUNGEON_MODE(SPELL_ARCANE_BLAST, H_SPELL_ARCANE_BLAST));
                    ArcaneBlastTimer = 6000;
                }
				else ArcaneBlastTimer -= diff;

                if (SlowTimer <= diff)
                {
                    Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true);
                    if (target)
                        DoCast(target, SPELL_SLOW);
                    SlowTimer = 5000;
                }
				else SlowTimer -= diff;
            }

            if (me->GetEntry() == NPC_AZURE_SPELLBREAKER_2)
            {
                if (ChainsOfIceTimer <= diff)
                {
                    Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true);
                    if (target)
                        DoCast(target, SPELL_CHAINS_OF_ICE);
                    ChainsOfIceTimer = 7000;
                }
				else ChainsOfIceTimer -= diff;

                if (ConeOfColdTimer <= diff)
                {
                    DoCast(DUNGEON_MODE(SPELL_CONE_OF_COLD, H_SPELL_CONE_OF_COLD));
                    ConeOfColdTimer = 5000;
                }
				else ConeOfColdTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_azure_spellbreakerAI(creature);
    }
};

class mob_azure_captain : public CreatureScript
{
public:
    mob_azure_captain() : CreatureScript("mob_azure_captain") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_azure_captainAI(creature);
    }

    struct  mob_azure_captainAI : public violet_hold_trashAI
    {
        mob_azure_captainAI(Creature* creature) : violet_hold_trashAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 MortalStrikeTimer;
        uint32 WhirlwindTimer;

        void Reset()
        {
            MortalStrikeTimer = 5000;
            WhirlwindTimer = 8000;
        }

        void UpdateAI(uint32 const diff)
        {
            violet_hold_trashAI::UpdateAI(diff);
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            if (MortalStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_MORTAL_STRIKE);
                MortalStrikeTimer = 5000;
            }
			else MortalStrikeTimer -= diff;

            if (WhirlwindTimer <= diff)
            {
                DoCast(me, SPELL_WHIRLWIND_OF_STEEL);
                WhirlwindTimer = 8000;
            }
			else WhirlwindTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

class mob_azure_sorceror : public CreatureScript
{
public:
    mob_azure_sorceror() : CreatureScript("mob_azure_sorceror") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_azure_sorcerorAI(creature);
    }

    struct  mob_azure_sorcerorAI : public violet_hold_trashAI
    {
        mob_azure_sorcerorAI(Creature* creature) : violet_hold_trashAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 ArcaneStreamTimer;
        uint32 ArcaneStreamTimerStartingValueHolder;
        uint32 ManaDetonationTimer;

        void Reset()
        {
            ArcaneStreamTimer = 4000;
            ArcaneStreamTimerStartingValueHolder = ArcaneStreamTimer;
            ManaDetonationTimer = 5000;
        }

        void UpdateAI(uint32 const diff)
        {
            violet_hold_trashAI::UpdateAI(diff);
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            if (ArcaneStreamTimer <= diff)
            {
                Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true);
                if (target)
                    DoCast(target, DUNGEON_MODE(SPELL_ARCANE_STREAM, H_SPELL_ARCANE_STREAM));
                ArcaneStreamTimer = urand(0, 5000)+5000;
                ArcaneStreamTimerStartingValueHolder = ArcaneStreamTimer;
            }
			else ArcaneStreamTimer -= diff;

            if (ManaDetonationTimer <= diff && ArcaneStreamTimer >=1500 && ArcaneStreamTimer <= ArcaneStreamTimerStartingValueHolder/2)
            {
                DoCast(DUNGEON_MODE(SPELL_MANA_DETONATION, H_SPELL_MANA_DETONATION));
                ManaDetonationTimer = urand(2000, 6000);
            }
			else ManaDetonationTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

class npc_defense_system : public CreatureScript
{
    public:
        npc_defense_system() : CreatureScript("npc_defense_system") { }

        struct npc_defense_systemAI : public QuantumBasicAI
        {
            npc_defense_systemAI(Creature* creature) : QuantumBasicAI(creature)
            {
                me->SetDisplayId(me->GetCreatureTemplate()->Modelid2);

				SetCombatMovement(false);
            }

            void Reset()
            {
                active = false;
                Wipe = false;
                VisualTimer = 2000;
                ActionTimer = 4000;
                me->SetVisible(false);
            }

            void DoAction(int32 const param)
            {
                switch (param)
                {
                    case 1:
                        break;
                    case 2:
                        Wipe = true;
                        break;
                }

                DoCast(me, SPELL_SPAWN_EFFECT, true);
				DoCast(me, SPELL_DEFENSE_SYSTEM_VISUAL, true);
                me->SetVisible(true);
                active = true;
            }

            void UpdateAI(uint32 const diff)
            {
                if (!active)
                    return;

                if (VisualTimer <= diff)
                {
                    DoCast(Wipe ? SPELL_ARCANE_LIGHTNING_VISUAL_2 : SPELL_ARCANE_LIGHTNING_VISUAL_1);
                    VisualTimer = 10000;
                }
                else
                    VisualTimer -= diff;

                if (ActionTimer <= diff)
                {
                    DoCast(Wipe ? SPELL_ARCANE_LIGHTNING_KILL : SPELL_ARCANE_LIGHTNING);
                    ActionTimer = 10000;
                    Reset();
                }
                else
                    ActionTimer -= diff;
            }

        private:
            bool active;
            bool Wipe;
            uint32 VisualTimer;
            uint32 ActionTimer;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_defense_systemAI(creature);
        }
};

void AddSC_violet_hold()
{
    new npc_sinclari_vh();
    new npc_teleportation_portal_vh();
    new mob_azure_invader();
    new mob_azure_spellbreaker();
    new mob_azure_binder();
    new mob_azure_mage_slayer();
    new mob_azure_captain();
    new mob_azure_sorceror();
    new mob_azure_raider();
    new mob_azure_stalker();
    new mob_azure_saboteur();
    new npc_defense_system();
}