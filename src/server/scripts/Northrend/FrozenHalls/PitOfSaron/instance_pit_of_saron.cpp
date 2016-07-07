/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "pit_of_saron.h"

// position for Jaina and Sylvanas
Position const EventLeaderPos2 = {1054.368f, 107.14620f, 628.4467f, 0.0f};

Position const spawnPoints2 = {773.266174f, -43.121738f, 508.355469f, 3.954455f};

//Slaves For Alliance and Horde. Martin Victus and Gorkun Ironskull
const uint32 NpcSlaveAlliance[3] = {NPC_MARTIN_VICTUS_1, NPC_FREED_SLAVE_2_ALLIANCE, NPC_FREED_SLAVE_1_ALLIANCE};
const uint32 NpcSlaveHorde[3] = {NPC_GORKUN_IRONSKULL_2, NPC_FREED_SLAVE_2_HORDE, NPC_FREED_SLAVE_1_HORDE};
const uint32 NpcSlaveIfDeadGarfrost[2] = {NPC_RESCUED_SLAVE_ALLIANCE, NPC_RESCUED_SLAVE_HORDE};

//Dead Garfrost from sniff
static const Position SlaveLeaderPos[3] =
{
    {693.281555f, -169.690872f, 526.965454f, 1.485173f}, 
    {696.024902f, -169.953308f, 526.870850f, 1.603771f}, 
    {690.887512f, -169.970963f, 526.891357f, 1.269191f},
};

// if Dead IckandKrick from sniff
static const Position SlaveLeaderPos2[3] =
{
    {849.804016f, -9.097073f, 509.900574f, 2.183652f},
    {851.979919f, -7.567026f, 509.982391f, 2.040709f},
    {847.959351f, -11.114618f, 509.794922f, 2.366650f},
};

// Slaves Alliance and Horde If Gargrost Dead For Events  from sniff
const Position spawnPoints1[3] =
{
    {768.920044f, -38.462135f, 508.355469f, 3.903403f},
    {766.413635f, -36.130611f, 508.346466f, 4.056557f},
    {770.746033f, -40.480698f, 508.355469f, 3.915185f},
};

class instance_pit_of_saron : public InstanceMapScript
{
    public:
		instance_pit_of_saron() : InstanceMapScript(PoSScriptName, 658) {}

        struct instance_pit_of_saron_InstanceScript : public InstanceScript
        {
            instance_pit_of_saron_InstanceScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(MAX_ENCOUNTER);
                GarfrostGUID = 0;
                KrickGUID = 0;
                IckGUID = 0;
                TyrannusGUID = 0;
                RimefangGUID = 0;
                JainaOrSylvanas1GUID = 0;
                JainaOrSylvanas2GUID = 0;
                SindragosaGUID = 0;
                TeamInInstance = 0;
                HallsofRefflectionPortalGUID =0;
				SaroniteRockGUID = 0;
                VictusOrGorkunFreedGUID = 0;
                _tyrannusEventStart = NOT_STARTED;
                _areaTriggerYmirjar = NOT_STARTED;
                _areaTriggerFallen = NOT_STARTED;
                _areaTriggerIceCicle = NOT_STARTED;
                _areaTriggerSlaveOutroFargrost = NOT_STARTED;
                _areTriggerGeistAmbusher = NOT_STARTED;
            }

			uint64 GarfrostGUID;
            uint64 KrickGUID;
            uint64 IckGUID;
            uint64 TyrannusGUID;
            uint64 RimefangGUID;
            uint64 TyrannusEventGUID;
            uint64 JainaOrSylvanas1GUID;
            uint64 JainaOrSylvanas2GUID;
            uint64 IceWall;
            uint64 VictusOrGorkunFreedGUID;
            uint64 SindragosaGUID;
            uint64 HallsofRefflectionPortalGUID;
			uint64 SaroniteRockGUID;

            uint32 TeamInInstance;
            uint8 _tyrannusEventStart;
            uint8 _areaTriggerYmirjar;
            uint8 _areaTriggerFallen;
            uint8 _areaTriggerIceCicle;
            uint8 _areaTriggerSlaveOutroFargrost;
            uint8 _areTriggerGeistAmbusher;

            void OnPlayerEnter(Player* player)
            {
                if (!TeamInInstance)
					TeamInInstance = player->GetTeam();
				if (GetData(DATA_TYRANNUS_START) != DONE)
					SetData(DATA_TYRANNUS_START, IN_PROGRESS);
            }

            void OnCreatureCreate(Creature* creature)
            {
                if (!TeamInInstance)
                {
					Map::PlayerList const &players = instance->GetPlayers();
                    if (!players.isEmpty())
					{
						if (Player* player = players.begin()->getSource())
							TeamInInstance = player->GetTeam();
					}
                }

                switch (creature->GetEntry())
                {
                    case NPC_GARFROST:
                        GarfrostGUID = creature->GetGUID();
                        break;
                    case NPC_KRICK:
                        KrickGUID = creature->GetGUID();
                        break;
                    case NPC_ICK:
                        IckGUID = creature->GetGUID();
                        break;
                    case NPC_TYRANNUS:
                        TyrannusGUID = creature->GetGUID();
                        break;
                    case NPC_RIMEFANG:
                        RimefangGUID = creature->GetGUID();
                        break;
                    case NPC_SCOURGELORD_TYRANNUS_INTRO:
						if (TeamInInstance == ALLIANCE)
							creature->UpdateEntry(NPC_SCOURGELORD_TYRANNUS_INTRO, ALLIANCE);
                        TyrannusEventGUID = creature->GetGUID();
                        break;
                    case NPC_SYLVANAS_PART1:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_JAINA_PART1, ALLIANCE);
                        JainaOrSylvanas1GUID = creature->GetGUID();
                        break;
                    case NPC_SYLVANAS_PART2:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_JAINA_PART2, ALLIANCE);
                        JainaOrSylvanas2GUID = creature->GetGUID();
                        break;
                    case NPC_KILARA:
                        if (TeamInInstance == ALLIANCE)
                           creature->UpdateEntry(NPC_ELANDRA, ALLIANCE);
                        break;
                    case NPC_KORALEN:
                        if (TeamInInstance == ALLIANCE)
                           creature->UpdateEntry(NPC_KORLAEN, ALLIANCE);
                        break;
                    case NPC_CHAMPION_1_HORDE:
                        if (TeamInInstance == ALLIANCE)
                           creature->UpdateEntry(NPC_CHAMPION_1_ALLIANCE, ALLIANCE);
                        break;
                    case NPC_CHAMPION_2_HORDE:
                        if (TeamInInstance == ALLIANCE)
                           creature->UpdateEntry(NPC_CHAMPION_2_ALLIANCE, ALLIANCE);
                        break;
                    case NPC_CHAMPION_3_HORDE:
                        if (TeamInInstance == ALLIANCE)
                           creature->UpdateEntry(NPC_CHAMPION_3_ALLIANCE, ALLIANCE);
                        break;
                    case NPC_HORDE_SLAVE_1:
                        if (TeamInInstance == ALLIANCE)
                           creature->UpdateEntry(NPC_ALLIANCE_SLAVE_1, ALLIANCE);
                        break;
                    case NPC_HORDE_SLAVE_2:
                        if (TeamInInstance == ALLIANCE)
                           creature->UpdateEntry(NPC_ALLIANCE_SLAVE_2, ALLIANCE);
                        break;
                    case NPC_HORDE_SLAVE_3:
                        if (TeamInInstance == ALLIANCE)
                           creature->UpdateEntry(NPC_ALLIANCE_SLAVE_3, ALLIANCE);
                        break;
                    case NPC_HORDE_SLAVE_4:
                        if (TeamInInstance == ALLIANCE)
                           creature->UpdateEntry(NPC_ALLIANCE_SLAVE_4, ALLIANCE);
                        break;
                    case NPC_FREED_SLAVE_1_HORDE:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_FREED_SLAVE_1_ALLIANCE, ALLIANCE);
                        break;
                    case NPC_FREED_SLAVE_2_HORDE:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_FREED_SLAVE_2_ALLIANCE, ALLIANCE);
                        break;
                    case NPC_FREED_SLAVE_3_HORDE:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_FREED_SLAVE_3_ALLIANCE, ALLIANCE);
                        break;
                    case NPC_RESCUED_SLAVE_HORDE:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_RESCUED_SLAVE_ALLIANCE, ALLIANCE);
                        break;
                    case NPC_MARTIN_VICTUS_1:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_MARTIN_VICTUS_1, ALLIANCE);
                        break;
                    case NPC_MARTIN_VICTUS_2:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_MARTIN_VICTUS_2, ALLIANCE);
						VictusOrGorkunFreedGUID = creature->GetGUID();
                        break;
                    case NPC_GORKUN_IRONSKULL_1:
                        if (TeamInInstance == ALLIANCE)
                            creature->UpdateEntry(NPC_MARTIN_VICTUS_2, ALLIANCE);
                          VictusOrGorkunFreedGUID = creature->GetGUID();
                    case NPC_SINDRAGOSA:
                         SindragosaGUID = creature->GetGUID();
                        break;
                    default:
                        break;
                }
            }
            
            void OnGameObjectCreate(GameObject* go)
            {
               switch (go->GetEntry())
               {
                    case GO_ICE_WALL:
                        IceWall = go->GetGUID();
                        if (GetBossState(DATA_GARFROST) == DONE && GetBossState(DATA_ICK) == DONE)
                            HandleGameObject(0, true, go);
                         break;
                    case GO_HALLS_OF_REFLECT_PORT:
                         HallsofRefflectionPortalGUID = go->GetGUID();
                         break;
					case GO_SARONITE_ROCK:
						SaroniteRockGUID = go->GetGUID();
						break;
                }
            }

            bool SetBossState(uint32 type, EncounterState state)
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                switch (type)
                {
                    case DATA_ICK:
                        if (state == DONE)
                        {
                            if (Creature* summoner = instance->GetCreature(IckGUID))
                            {
                               for (int i = 0; i < 3; ++i)
                               {
                                if (TeamInInstance == ALLIANCE)
                                    summoner->SummonCreature(NpcSlaveAlliance[i], SlaveLeaderPos2[i], TEMPSUMMON_MANUAL_DESPAWN);
                                else
                                    summoner->SummonCreature(NpcSlaveHorde[i], SlaveLeaderPos2[i], TEMPSUMMON_MANUAL_DESPAWN);
                               }
                            }
                            if (GetBossState(DATA_GARFROST) == DONE)
                                HandleGameObject(IceWall, true, NULL);
                        }
                        break;
                    case DATA_GARFROST:
                        if (state == DONE)
                        {
                            SetData(DATA_SLAVE_OUTRO_GARFROST, IN_PROGRESS);
                            if (Creature* summoner = instance->GetCreature(GarfrostGUID))
                            {
                               for (int i = 0; i < 3; ++i)
                               {
                                if (TeamInInstance == ALLIANCE)
                                    summoner->SummonCreature(NpcSlaveAlliance[i], SlaveLeaderPos[i], TEMPSUMMON_MANUAL_DESPAWN);
                                else
                                    summoner->SummonCreature(NpcSlaveHorde[i], SlaveLeaderPos[i], TEMPSUMMON_MANUAL_DESPAWN);
                               }
                               summoner->SummonCreature(NPC_RESCUED_SLAVE_ALLIANCE, spawnPoints2, TEMPSUMMON_MANUAL_DESPAWN);
                               for (uint8 i = 0; i < 3; i++)
								   summoner->SummonCreature(NPC_RESCUED_SLAVE_HORDE, spawnPoints1[i], TEMPSUMMON_MANUAL_DESPAWN);
                            }
                            if (GetBossState(DATA_ICK) == DONE)
                                HandleGameObject(IceWall, true, NULL);
                        }
                        break;
                    case DATA_TYRANNUS:
                        if (state == DONE)
                        {
                            if (Creature* summoner = instance->GetCreature(TyrannusGUID))
                            {
								if (TeamInInstance == ALLIANCE)
                                    summoner->SummonCreature(NPC_JAINA_PART2, EventLeaderPos2, TEMPSUMMON_MANUAL_DESPAWN);
                                else
                                    summoner->SummonCreature(NPC_SYLVANAS_PART2, EventLeaderPos2, TEMPSUMMON_MANUAL_DESPAWN);
                            }
                        }
                        break;
                    default:
                        break;
                }

                return true;
            }

            uint32 GetData(uint32 type)
            {
                switch (type)
                {
                    case DATA_TEAM_IN_INSTANCE:
                        return TeamInInstance;
                    case DATA_TYRANNUS_START:
                        return _tyrannusEventStart;
                    case DATA_AREA_TRIGGER_YMIRJAR:
                        return _areaTriggerYmirjar;
                    case DATA_AREA_TRIGGER_FALLEN:
                        return _areaTriggerFallen;
                    case DATA_AREA_TRIGGER_ICE_CICLE:
                        return _areaTriggerIceCicle;
                    case DATA_SLAVE_OUTRO_GARFROST:
                        return _areaTriggerSlaveOutroFargrost;
                    case DATA_GEIST_AMBUSHER:
                        return _areTriggerGeistAmbusher;
                    default:
                        break;
                }

                return 0;
            }

            uint64 GetData64(uint32 type)
            {
                switch (type)
                {
                    case DATA_GARFROST:
                        return GarfrostGUID;
                    case DATA_KRICK:
                        return KrickGUID;
                    case DATA_ICK:
                        return IckGUID;
                    case DATA_TYRANNUS:
                        return TyrannusGUID;
                    case DATA_RIMEFANG:
                        return RimefangGUID;
                    case DATA_TYRANNUS_EVENT:
                        return TyrannusEventGUID;
                    case DATA_JAINA_SYLVANAS_1:
                        return JainaOrSylvanas1GUID;
                    case DATA_JAINA_SYLVANAS_2:
                        return JainaOrSylvanas2GUID;
                    case DATA_VICTUS_OR_GORKUN_FREED:
                        return VictusOrGorkunFreedGUID;
                    case DATA_SINDRAGOSA:
                         return SindragosaGUID;
                    case GO_HALLS_OF_REFLECT_PORT:
                         return HallsofRefflectionPortalGUID;
					case GO_SARONITE_ROCK:
						return SaroniteRockGUID;
                    default:
                        break;
                }

                return 0;
			}

           void SetData(uint32 type, uint32 data)
           {
			   if (type == DATA_TYRANNUS_START)
				   _tyrannusEventStart = data;
			   if (type == DATA_AREA_TRIGGER_YMIRJAR)
				   _areaTriggerYmirjar = data;
			   if (type == DATA_AREA_TRIGGER_FALLEN)
				   _areaTriggerFallen = data;
			   if (type == DATA_AREA_TRIGGER_ICE_CICLE)
				   _areaTriggerIceCicle = data;
			   if (type == DATA_SLAVE_OUTRO_GARFROST)
				   _areaTriggerSlaveOutroFargrost = data;
			   if (type == DATA_GEIST_AMBUSHER)
				   _areTriggerGeistAmbusher = data;
           }
            std::string GetSaveData()
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "P S " << GetBossSaveData();

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            void Load(const char* in)
            {
                if (!in)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                OUT_LOAD_INST_DATA(in);

                char dataHead1, dataHead2;

                std::istringstream loadStream(in);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'P' && dataHead2 == 'S')
                {
                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    {
						uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                            tmpState = NOT_STARTED;

                        SetBossState(i, EncounterState(tmpState));
                    }
                }
                else
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_pit_of_saron_InstanceScript(map);
        }
};

void AddSC_instance_pit_of_saron()
{
    new instance_pit_of_saron();
}