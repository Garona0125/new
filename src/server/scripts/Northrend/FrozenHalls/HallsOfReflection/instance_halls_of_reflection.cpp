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
#include "MapManager.h"
#include "Transport.h"
#include "halls_of_reflection.h"

enum Events
{
    EVENT_NONE,
    EVENT_START_LICH_KING,
};

class instance_halls_of_reflection : public InstanceMapScript
{
public:
    instance_halls_of_reflection() : InstanceMapScript("instance_halls_of_reflection", 668) { }

    struct instance_halls_of_reflection_InstanceMapScript : public InstanceScript
    {
        instance_halls_of_reflection_InstanceMapScript(Map* map) : InstanceScript(map) {};

		uint32 Encounter[MAX_ENCOUNTER];
		uint32 CheckCombatTimer;
        uint32 TeamInInstance;
        uint32 IntroDone;
        uint32 Summons;
        uint32 DataPhase;

        uint64 FalricGUID;
        uint64 MarwynGUID;
        uint64 LichKingGUID;
        uint64 JainaPart1GUID;
        uint64 SylvanasPart1GUID;
		uint64 ArchmageElandraGUID;
		uint64 DarkRangerLoralenGUID;
        uint64 LiderGUID;
        uint64 Captain;
        uint64 CaptainChestGUID;
        uint64 PortalGUID;
        uint64 FrostmourneGUID;
        uint64 FrontDoorGUID;
        uint64 FrostwornDoorGUID;
        uint64 ArthasDoor;
        uint64 RunDoorGUID;
        uint64 Wall[4];
        uint64 WallID[4];
        uint64 CaveDoorGUID;

		EventMap events;

        bool isLoaded;

        void Initialize()
        {
            events.Reset();

			TeamInInstance = 0;
			IntroDone = 0;

            FalricGUID = 0;
            MarwynGUID = 0;
            LichKingGUID = 0;
            JainaPart1GUID = 0;
            SylvanasPart1GUID = 0;
			ArchmageElandraGUID = 0;
			DarkRangerLoralenGUID = 0;
            LiderGUID = 0;
            Captain = 0;

            CaptainChestGUID = 0;
            PortalGUID = 0;
            FrostmourneGUID = 0;
            ArthasDoor = 0;
            FrostwornDoorGUID = 0;
            FrontDoorGUID = 0;
            CaveDoorGUID = 0;

            isLoaded = false;
            
            for (uint8 i = 0; i < 4; ++i)
            {
                Wall[i] = 0;
                WallID[i] = 0;
            }

            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                Encounter[i] = NOT_STARTED;
        }

        void OpenDoor(uint64 guid)
        {
			if (!guid)
				return;

			GameObject* go = instance->GetGameObject(guid);
			if (go)
				go->SetGoState(GO_STATE_ACTIVE);
        }

        void CloseDoor(uint64 guid)
        {
            if (!guid)
				return;

            GameObject* go = instance->GetGameObject(guid);

            if (go)
				go->SetGoState(GO_STATE_READY);
        }

		void OnPlayerEnter(Player* player)
        {
            if (!TeamInInstance)
                TeamInInstance = player->GetTeam();

			LoadGunship();
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

            switch(creature->GetEntry())
            {
                case NPC_FALRIC:
                    FalricGUID = creature->GetGUID();
                    break;
                case NPC_MARWYN:
                    MarwynGUID = creature->GetGUID();
                    break;
                case NPC_LICH_KING_EVENT:
                    break;
                case NPC_JAINA_PART1:
                    JainaPart1GUID = creature->GetGUID();                  
                    break;
                case NPC_SYLVANAS_PART1:
					if (TeamInInstance == ALLIANCE)
						creature->UpdateEntry(NPC_JAINA_PART1, ALLIANCE);

                    SylvanasPart1GUID = creature->GetGUID();
                    break;
				case NPC_ARCHMAGE_ELANDRA:
					creature->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
					ArchmageElandraGUID = creature->GetGUID();
					break;
				case NPC_DARK_RANGER_LORALEN:
					if (TeamInInstance == ALLIANCE)
						creature->UpdateEntry(NPC_ARCHMAGE_ELANDRA, ALLIANCE);
					DarkRangerLoralenGUID = creature->GetGUID();
					break;
                case NPC_FROSTSWORN_GENERAL:
                    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    break;
                case NPC_JAINA_OUTRO:
                    creature->SetHealth(252000);
                    LiderGUID = creature->GetGUID();
                    break;
				case NPC_SYLVANAS_OUTRO:
					if (TeamInInstance == ALLIANCE)
						creature->UpdateEntry(NPC_JAINA_OUTRO, ALLIANCE);

					creature->SetHealth(252000);
					LiderGUID = creature->GetGUID();
					break;
                case NPC_LICH_KING:
                    creature->SetHealth(20917000);
                    LichKingGUID = creature->GetGUID();
                    break;        
                case NPC_BARTLETT:
                case NPC_KORM:
					Captain = creature->GetGUID();
					break;
				default:
					break;
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch(go->GetEntry())
            {
                case GO_FROSTMOURNE:
					FrostmourneGUID = go->GetGUID();
					HandleGameObject(0, false, go);
					break;
                case GO_FROSTMOURNE_ALTAR:
                    go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
                    HandleGameObject(0, true, go);
                    break;
                case GO_FRONT_DOOR:
                    FrontDoorGUID = go->GetGUID();
                    go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
                    OpenDoor(FrontDoorGUID);
                    break;
                case GO_FROSTWORN_DOOR:
                    FrostwornDoorGUID = go->GetGUID();
                    go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);

                    if (Encounter[1] == DONE)
                        OpenDoor(FrostwornDoorGUID);
                    else
                        CloseDoor(FrostwornDoorGUID);
                    break;
                case GO_RUN_DOOR:
                    RunDoorGUID = go->GetGUID();
                    break;
                case GO_ARTHAS_DOOR:
                    ArthasDoor = go->GetGUID();
                    break;
                case GO_ICE_WALL_1:
                    WallID[0] = go->GetGUID();
                    break;
                case GO_ICE_WALL_2:
                    WallID[1] = go->GetGUID();
                    break;
                case GO_ICE_WALL_3:
                    WallID[2] = go->GetGUID();
                    break;
                case GO_ICE_WALL_4:
                    WallID[3] = go->GetGUID();
                    break;
                case GO_CAVE:
                    CaveDoorGUID = go->GetGUID();
                    break;
                case GO_CAPTAIN_CHEST_1:
                    if (!instance->IsHeroic() && TeamInInstance == HORDE)
                        CaptainChestGUID = go->GetGUID();
                    break;
                case GO_CAPTAIN_CHEST_3:
                    if (instance->IsHeroic() && TeamInInstance == HORDE)
                        CaptainChestGUID = go->GetGUID();
                    break;
                case GO_CAPTAIN_CHEST_2:
                    if (!instance->IsHeroic() && TeamInInstance == ALLIANCE)
                        CaptainChestGUID = go->GetGUID();
                    break;
                case GO_CAPTAIN_CHEST_4:
                    if (instance->IsHeroic() && TeamInInstance == ALLIANCE)
                        CaptainChestGUID = go->GetGUID();
                    break;
                case GO_PORTAL:
                    PortalGUID = go->GetGUID();
                    break;
            }
        }

		void SetData(uint32 type, uint32 data)
        {
            switch(type)
            {
                case DATA_INTRO_EVENT:
                    IntroDone = data;
                    break;
                case DATA_FALRIC_EVENT:
                    Encounter[0] = data;
                    break;
                case DATA_MARWYN_EVENT:
                    Encounter[1] = data;
                    if (data == DONE)
                    {
                        OpenDoor(FrostwornDoorGUID);
                        OpenDoor(FrontDoorGUID);

						DoUpdateWorldState(WORLD_STATE_HOR, 0);
						DoUpdateWorldState(WORLD_STATE_HOR_WAVE_COUNT, 0);
                    }
                    break;
                case DATA_FROSTSWORN_EVENT:
                    Encounter[2] = data;
                    if (data == DONE)
                    {
                        OpenDoor(ArthasDoor);
                        SetData(DATA_PHASE, 3);
                        instance->SummonCreature(NPC_LICH_KING, OutroSpawns[0]);
						instance->SummonCreature(NPC_SYLVANAS_OUTRO, OutroSpawns[1]);
                    }
                    break;
                case DATA_LICH_KING_EVENT:
                    Encounter[3] = data;
                    if (data == IN_PROGRESS)
                    {
                        OpenDoor(RunDoorGUID);

                        if (instance->IsHeroic())
                            DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEVEMENT_NOT_RETREATING_EVENT);
                    }
                    if (data == FAIL)
                    {
                        for (uint8 i = 0; i<4; i++)
							OpenDoor(WallID[i]);

                        CloseDoor(RunDoorGUID);
                        OpenDoor(FrontDoorGUID);

                        if (Creature* lich = instance->GetCreature(LichKingGUID))
                            lich->DespawnAfterAction(10*IN_MILLISECONDS);

                        if (Creature* lider = instance->GetCreature(LiderGUID))
                            lider->DespawnAfterAction(10*IN_MILLISECONDS);

						DoStopTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEVEMENT_NOT_RETREATING_EVENT);
                        DoCastSpellOnPlayers(SPELL_MASS_SUICIDE);

                        SetData(DATA_PHASE, 3);
                        instance->SummonCreature(NPC_LICH_KING, OutroSpawns[0]);
						instance->SummonCreature(NPC_SYLVANAS_OUTRO, OutroSpawns[1]);
                    }
                    if (data == DONE)
                    {
                        if (GameObject* pChest = instance->GetGameObject(CaptainChestGUID))
                            pChest->SetPhaseMask(1, true);

                        if (GameObject* pPortal = instance->GetGameObject(PortalGUID))
                            pPortal->SetPhaseMask(1, true);

                        OpenDoor(FrontDoorGUID);
                        DoCompleteAchievement(ACHIEVEMENT_HALLS_OF_REFLECTION_5N);
                        if (instance->IsHeroic())
                        {
                            DoCompleteAchievement(ACHIEVEMENT_HALLS_OF_REFLECTION_5H);
                            DoCastSpellOnPlayers(SPELL_ACHIEVEMENT_CHECK);
                            DoStopTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEVEMENT_NOT_RETREATING_EVENT);
                        }
                    }
                    break;
                case DATA_SUMMONS:
                    if (data == 3) Summons = 0;
                    else if (data == 1)
						++Summons;
                    else if (data == 0)
						--Summons;
                    data = NOT_STARTED;
                    break;
                case DATA_ICE_WALL_1:
                    Wall[0] = data;
                    break;
                case DATA_ICE_WALL_2:
                    Wall[1] = data;
                    break;
                case DATA_ICE_WALL_3:
                    Wall[2] = data;
                    break;
                case DATA_ICE_WALL_4:
                    Wall[3] = data;
                    break;
                case DATA_PHASE:
                    DataPhase = data;
                    break;
            }

            if (data == DONE)
                SaveToDB();
        }

        uint32 GetData(uint32 type)
        {
            switch(type)
            {
                case DATA_INTRO_EVENT:
					return IntroDone;
                case DATA_TEAM_IN_INSTANCE:
					return TeamInInstance;
                case DATA_FALRIC_EVENT:
					return Encounter[0];
                case DATA_MARWYN_EVENT:
					return Encounter[1];
                case DATA_FROSTSWORN_EVENT:
					return Encounter[2];
                case DATA_LICH_KING_EVENT:
					return Encounter[3];
                case DATA_ICE_WALL_1:
					return Wall[0];
                case DATA_ICE_WALL_2:
					return Wall[1];
                case DATA_ICE_WALL_3:
					return Wall[2];
                case DATA_ICE_WALL_4:
					return Wall[3];
                case DATA_SUMMONS:
					return Summons;
                case DATA_PHASE:
					return DataPhase;
            }
            return 0;
        }

        uint64 GetData64(uint32 identifier)
        {
            switch(identifier)
            {
                case DATA_FALRIC:
					return FalricGUID;
                case DATA_MARWYN:
					return MarwynGUID;
                case DATA_LICHKING:
					return LichKingGUID;
                case DATA_ESCAPE_LIDER:
					return LiderGUID;
                case DATA_FROSTMOURNE:
					return FrostmourneGUID;
                case DATA_FRONT_DOOR:
					return FrontDoorGUID;
                case DATA_FROSTWORN_DOOR:
					return FrostwornDoorGUID;
                case DATA_ARTHAS_DOOR:
					return ArthasDoor;
                case GO_ICE_WALL_1:
					return WallID[0];
                case GO_ICE_WALL_2:
					return WallID[1];
                case GO_ICE_WALL_3:
					return WallID[2];
                case GO_ICE_WALL_4:
					return WallID[3];
                case GO_CAVE:
					return CaveDoorGUID;
                case DATA_CAPTAIN:
					return Captain;
                case GO_CAPTAIN_CHEST_1:
                case GO_CAPTAIN_CHEST_2:
                case GO_CAPTAIN_CHEST_3:
                case GO_CAPTAIN_CHEST_4:
					return CaptainChestGUID;
            }
            return 0;
        }

		void Update(uint32 diff)
        {
			if (CheckCombatTimer <= diff)
			{
				if (GetData(DATA_FALRIC_EVENT) == DONE && GetData(DATA_MARWYN_EVENT) == DONE)
					return;

				Map::PlayerList const &playerList = instance->GetPlayers();

				if (playerList.isEmpty())
					return;

				for (Map::PlayerList::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
				{
					if (Player* player = itr->getSource())
					{
						if (player->GetAreaId() == 4820 && player->IsDead())
						{
							OpenDoor(FrontDoorGUID);

							if (GetData(DATA_FALRIC_EVENT) != DONE && GetData(DATA_MARWYN_EVENT) != DONE || GetData(DATA_FALRIC_EVENT) == DONE && GetData(DATA_MARWYN_EVENT) != DONE)
							{
								if (GameObject* frostmourne = instance->GetGameObject(FrostmourneGUID))
									frostmourne->SetPhaseMask(1, true);

								if (Creature* falric = instance->GetCreature(FalricGUID))
								{
									if (falric->IsAlive())
										falric->AI()->EnterEvadeMode();
									else
									{
										if (falric->IsDead())
											falric->Respawn();
									}
								}

								if (Creature* marwyn = instance->GetCreature(MarwynGUID))
								{
									if (marwyn->IsAlive())
										marwyn->AI()->EnterEvadeMode();
								}

								if (Creature* jaina = instance->GetCreature(JainaPart1GUID))
									jaina->Respawn();

								if (Creature* sylvanas = instance->GetCreature(SylvanasPart1GUID))
									sylvanas->Respawn();

								DoUpdateWorldState(WORLD_STATE_HOR, 0);
								DoUpdateWorldState(WORLD_STATE_HOR_WAVE_COUNT, 0);

								SetData(DATA_INTRO_EVENT, NOT_STARTED);
								SetData(DATA_WAVE_COUNT, NOT_STARTED);
								SetData(DATA_FALRIC_EVENT, FAIL);
								SetData(DATA_MARWYN_EVENT, FAIL);
							}
						}
					}
				}
				CheckCombatTimer = 1*IN_MILLISECONDS;
			}
			else CheckCombatTimer -= diff;

			if (!instance->HavePlayers())
				return;

            events.Update(diff);
        }

        void LoadGunship()
        {
			if (isLoaded)
				return;

            if (TeamInInstance == ALLIANCE)
            {
                if (Transport* gunship = sMapMgr->LoadTransportInMap(instance, GO_THE_SKYBREAKER, 23970))
					gunship->AddNPCPassengerInInstance(NPC_BARTLETT, -3.45774f, 10.2614f, 20.4492f, 1.53856f);
            }

            if (TeamInInstance == HORDE)
            {
                if (Transport* gunship = sMapMgr->LoadTransportInMap(instance, GO_ORGRIMS_HAMMER, 23970))
					gunship->AddNPCPassengerInInstance(NPC_KORM, 11.7784f, 19.0924f, 34.8982f, 1.53856f);
            }
            isLoaded = true;
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << "H R 1 " << Encounter[0] << " " << Encounter[1] << " " << Encounter[2] << " " << Encounter[3] << " " << IntroDone;

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
            uint16 version;
            uint16 data0, data1, data2, data3, data4;

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> version >> data0 >> data1 >> data2 >> data3 >> data4;

            if (dataHead1 == 'H' && dataHead2 == 'R')
            {
                Encounter[0] = data0;
                Encounter[1] = data1;
                Encounter[2] = data2;
                Encounter[3] = data3;
                IntroDone = data4;

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (Encounter[i] == IN_PROGRESS)
                        Encounter[i] = NOT_STARTED;

                OpenDoor(FrontDoorGUID);

                if (Encounter[1] == DONE)
                    OpenDoor(FrostwornDoorGUID);

                if (Encounter[2] == DONE)
                    OpenDoor(ArthasDoor);
            }
			else OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_halls_of_reflection_InstanceMapScript(map);
    }
};

void AddSC_instance_halls_of_reflection()
{
    new instance_halls_of_reflection();
}