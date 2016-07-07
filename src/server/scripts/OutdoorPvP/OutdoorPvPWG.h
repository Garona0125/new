/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef OUTDOOR_PVP_WG_
#define OUTDOOR_PVP_WG_

#include "OutdoorPvP.h"
#include "BattlegroundMgr.h"
#include "Player.h"

enum Wintergrasp
{
	ZONE_DALARAN             = 4395,
	ZONE_WINTERGRASP         = 4197,
	POS_X_CENTER             = 5100,
	MAX_VEHICLE_PER_WORKSHOP = 4,
};

enum OutdoorPvPWGSpell
{
   SPELL_RECRUIT                = 37795,
   SPELL_CORPORAL               = 33280,
   SPELL_LIEUTENANT             = 55629,
   SPELL_TENACITY               = 58549,
   SPELL_TENACITY_VEHICLE       = 59911,
   SPELL_TOWER_CONTROL          = 62064,
   SPELL_SPIRITUAL_IMMUNITY     = 58729,
   SPELL_WINTERGRASP_VICTORY    = 56902,
   SPELL_WINTERGRASP_DEFEAT     = 58494,
   SPELL_WINTERGRASP_WIN_VISUAL = 60044,
   SPELL_DAMAGED_TOWER          = 59135,
   SPELL_DESTROYED_TOWER        = 59136,
   SPELL_DAMAGED_BUILDING       = 59201,
   SPELL_INTACT_BUILDING        = 59203,
   SPELL_TELEPORT_ALLIENCE_CAMP = 58632,
   SPELL_TELEPORT_HORDE_CAMP    = 58633,
   SPELL_TELEPORT_FORTRESS      = 59096,
   SPELL_TELEPORT_DALARAN       = 53360,
   SPELL_ACTIVATE_CONTROL_ARMS  = 49899,
   SPELL_STRIKE                 = 52532,
   SPELL_RIDE_WG_VEHICLE        = 60968,
   SPELL_DESTROY_VEHICLE        = 67748,
   SPELL_HORDE_FLAG_VISUAL      = 14267,
   SPELL_ALLIANCE_FLAG_VISUAL   = 14268,
   SPELL_WINTERGRASP_WATER      = 36444,
};

enum OUTDOOR_PVP_WG_SOUNDs
{
   OUTDOOR_PVP_WG_SOUND_KEEP_CLAIMED            = 8192,
   OUTDOOR_PVP_WG_SOUND_KEEP_CAPTURED_ALLIANCE  = 8173,
   OUTDOOR_PVP_WG_SOUND_KEEP_CAPTURED_HORDE     = 8213,
   OUTDOOR_PVP_WG_SOUND_KEEP_ASSAULTED_ALLIANCE = 8212,
   OUTDOOR_PVP_WG_SOUND_KEEP_ASSAULTED_HORDE    = 8174,
   OUTDOOR_PVP_WG_SOUND_NEAR_VICTORY            = 8456,
   OUTDOOR_PVP_WG_SOUND_HORDE_WINS              = 8454,
   OUTDOOR_PVP_WG_SOUND_ALLIANCE_WINS           = 8455,
   OUTDOOR_PVP_WG_SOUND_WORKSHOP_HORDE          = 6205,
   OUTDOOR_PVP_WG_SOUND_WORKSHOP_ALLIANCE       = 6298,
   OUTDOOR_PVP_WG_SOUND_HORDE_CAPTAIN           = 8333,
   OUTDOOR_PVP_WG_SOUND_ALLIANCE_CAPTAIN        = 8232,
   //OUTDOOR_PVP_WG_SOUND_START_BATTLE          = 11803, // L70ETC Concert
   OUTDOOR_PVP_WG_SOUND_START_BATTLE            = 3439, // Standart BG Start sound
};

enum WGCreature
{
	NPC_KILL_CREDIT_PVP_KILL  = 31086,
	NPC_KILL_CREDIT_ENGINE    = 31093,

	NPC_GNOMISH_ENGINEER_A    = 30499,
	NPC_GNOMISH_ENGINEER_H    = 30400,
	NPC_SPIRIT_HEALER_A       = 31842,
	NPC_SPIRIT_HEALER_H       = 31841,
	NPC_SORCERESS_KAYLANA_A   = 31051,
	NPC_HOODOO_MASTER_FUJIN_H = 31101,

   WG_CREATURE_QUESTGIVER_1_A                  = 31052,
   WG_CREATURE_QUESTGIVER_1_H                  = 31102,
   WG_CREATURE_QUESTGIVER_2_A                  = 31109,
   WG_CREATURE_QUESTGIVER_2_H                  = 31107,
   WG_CREATURE_QUESTGIVER_3_A                  = 31153,
   WG_CREATURE_QUESTGIVER_3_H                  = 31151,
   WG_CREATURE_QUESTGIVER_4_A                  = 31108,
   WG_CREATURE_QUESTGIVER_4_H                  = 31106,
   WG_CREATURE_QUESTGIVER_5_A                  = 31054,
   WG_CREATURE_QUESTGIVER_5_H                  = 31053,
   WG_CREATURE_QUESTGIVER_6_A                  = 31036,
   WG_CREATURE_QUESTGIVER_6_H                  = 31091,
};

enum Gameobjects
{
	GO_TITAN_RELIC       = 192829,
	GO_GOBLIN_WORKSHOP_1 = 192028,
	GO_GOBLIN_WORKSHOP_2 = 192030,
	GO_GOBLIN_WORKSHOP_3 = 192032,
	GO_GOBLIN_WORKSHOP_4 = 192029,
	GO_GOBLIN_WORKSHOP_5 = 192031,
	GO_GOBLIN_WORKSHOP_6 = 192033,
};

enum OutdoorPvP_WG_KeepStatus
{
   OUTDOOR_PVP_WG_KEEP_TYPE_NEUTRAL           = 0,
   OUTDOOR_PVP_WG_KEEP_TYPE_CONTESTED         = 1,
   OUTDOOR_PVP_WG_KEEP_STATUS_ALLY_CONTESTED  = 1,
   OUTDOOR_PVP_WG_KEEP_STATUS_HORDE_CONTESTED = 2,
   OUTDOOR_PVP_WG_KEEP_TYPE_OCCUPIED          = 3,
   OUTDOOR_PVP_WG_KEEP_STATUS_ALLY_OCCUPIED   = 3,
   OUTDOOR_PVP_WG_KEEP_STATUS_HORDE_OCCUPIED  = 4,
};

enum WGQuest
{
	QUEST_VICTORY_IN_WG_A     = 13181,
	QUEST_VICTORY_IN_WG_H     = 13183,
};

enum WB_ACHIEVEMENTS
{
   ACHIEVEMENTS_WIN_WG                          = 1717,
   ACHIEVEMENTS_WIN_WG_100                      = 1718, //todo
   ACHIEVEMENTS_WG_GNOMESLAUGHTER               = 1723, //todo
   ACHIEVEMENTS_WG_TOWER_DESTROY                = 1727,
   ACHIEVEMENTS_DESTRUCTION_DERBY_A             = 1737, //todo
   ACHIEVEMENTS_WG_TOWER_CANNON_KILL            = 1751, //todo
   ACHIEVEMENTS_WG_MASTER_A                     = 1752, //todo
   ACHIEVEMENTS_WIN_WG_TIMER_10                 = 1755,
   ACHIEVEMENTS_STONE_KEEPER_50                 = 2085, //todo
   ACHIEVEMENTS_STONE_KEEPER_100                = 2086, //todo
   ACHIEVEMENTS_STONE_KEEPER_250                = 2087, //todo
   ACHIEVEMENTS_STONE_KEEPER_500                = 2088, //todo
   ACHIEVEMENTS_STONE_KEEPER_1000               = 2089, //todo
   ACHIEVEMENTS_WG_RANGER                       = 2199, //todo
   ACHIEVEMENTS_DESTRUCTION_DERBY_H             = 2476, //todo
   ACHIEVEMENTS_WG_MASTER_H	                    = 2776, //todo
};

enum DataId
{
   DATA_ENGINEER_DIE,
};

enum OutdoorPVPWGStatus
{
	WORLDSTATE_WINTERGRASP_WARTIME            = 31001,
	WORLDSTATE_WINTERGRASP_TIMER              = 31002,
	WORLDSTATE_WINTERGRASP_DEFENDERS          = 31003,
	WORLDSTATE_WINTERGRASP_CONTROLING_FACTION = 31004,
	WORLDSTATE_VALUE_COUNT,
};

enum WintergraspWorldStates
{
    WORLDSTATE_WINTERGRASP_STATE_VEHICLE_H           = 3490,
    WORLDSTATE_WINTERGRASP_STATE_MAX_VEHICLE_H       = 3491,
    WORLDSTATE_WINTERGRASP_STATE_VEHICLE_A           = 3680,
    WORLDSTATE_WINTERGRASP_STATE_MAX_VEHICLE_A       = 3681,
    WORLDSTATE_WINTERGRASP_STATE_ACTIVE              = 3801,
    WORLDSTATE_WINTERGRASP_STATE_DEFENDER            = 3802,
	WORLDSTATE_WINTERGRASP_STATE_ATTACKER            = 3803,
    WORLDSTATE_WINTERGRASP_STATE_SHOW_WORLDSTATE     = 3710,
	WORLDSTATE_WINTERGRASP_STATE_TIME_END_BATTLE     = 3781,
	WORLDSTATE_WINTERGRASP_STATE_TIME_TO_NEXT_BATTLE = 4354,
};

enum OutdoorPvPWGCreType
{
   CREATURE_OTHER,
   CREATURE_SIEGE_VEHICLE,
   CREATURE_TURRET,
   CREATURE_ENGINEER,
   CREATURE_GUARD,
   CREATURE_SPECIAL,
   CREATURE_SPIRIT_GUIDE,
   CREATURE_SPIRIT_HEALER,
   CREATURE_QUESTGIVER,
};

enum OutdoorPvPWGBuildingType
{
   BUILDING_WALL,
   BUILDING_WORKSHOP,
   BUILDING_TOWER,
};

enum OutdoorPvPWGDamageState
{ // Do not change order
   DAMAGE_INTACT,
   DAMAGE_DAMAGED,
   DAMAGE_DESTROYED,
};

typedef uint32 TeamPair[2];

const uint8 CapturePointArtKit[3] = {2, 1, 21};
const uint32 WintergraspFaction[3] = {1802, 1801, 35};
const uint32 WG_MARK_OF_HONOR = 43589;
const uint32 VehNumWorldState[2] = {WORLDSTATE_WINTERGRASP_STATE_VEHICLE_A, WORLDSTATE_WINTERGRASP_STATE_VEHICLE_H};
const uint32 MaxVehNumWorldState[2] = {WORLDSTATE_WINTERGRASP_STATE_MAX_VEHICLE_A, WORLDSTATE_WINTERGRASP_STATE_MAX_VEHICLE_H};
const uint32 ClockWorldState[2] = {WORLDSTATE_WINTERGRASP_STATE_TIME_END_BATTLE, WORLDSTATE_WINTERGRASP_STATE_TIME_TO_NEXT_BATTLE};
const Team TeamId2Team[3] = {ALLIANCE, HORDE, TEAM_OTHER};

char const* fmtstring(char const* format, ...);

const uint16 GameEventWintergraspDefender[2] = {50, 51};

const TeamPair OutdoorPvPWGCreEntryPair[] =
{
   {32307, 32308}, // Guards
   {30739, 30740}, // Champions
   {32296, 32294}, // Quartermaster
   {39173, 39172}, // Ros'slai & Marshal Magruder
   {32615, 32626}, // Warbringer & Brigadier General
   {0,0} // Do not delete Used in LoadTeamPair
};

const TeamPair OutdoorPvPWGGODisplayPair[] =
{
   {5651, 5652},
   {8256, 8257},
   {0,0} // Do not delete Used in LoadTeamPair
};

const uint32 AreaPOIIconId[3][3] = {{7,8,9},{4,5,6},{1,2,3}};
typedef std::list<const AreaPOIEntry*> AreaPOIList;

struct BuildingState
{
   explicit BuildingState(uint32 _worldState, TeamId _team, bool asDefault)
       : worldState(_worldState), health(0)
       , defaultTeam(asDefault ? _team : OTHER_TEAM(_team)), team(_team), damageState(DAMAGE_INTACT)
       , building(NULL), type(BUILDING_WALL), graveTeam(NULL)
   {}
   uint32 worldState;
   uint32 health;
   TeamId defaultTeam;
   OutdoorPvPWGDamageState damageState;
   GameObject* building;
   uint32* graveTeam;
   OutdoorPvPWGBuildingType type;

   void SendUpdate(Player* player) const
   {
       player->SendUpdateWorldState(worldState, AreaPOIIconId[team][damageState]);
   }

   void FillData(WorldPacket &data) const
   {
       data << worldState << AreaPOIIconId[team][damageState];
   }

   TeamId GetTeam() const { return team; }
   void SetTeam(TeamId t)
   {
       team = t;
       if(graveTeam)
           if (uint32 newTeam = TeamId2Team[t])
               *graveTeam = newTeam;
   }

private:
   TeamId team;
};

typedef std::map<uint32, uint32> TeamPairMap;

class OPvPCapturePointWG;

typedef std::set<Player*> PlayerSet;
typedef std::set<uint64> GuidSet;
typedef std::map<uint64,uint32> PlayerTimerMap;

class OutdoorPvPWG : public OutdoorPvP
{
   protected:
       typedef std::map<uint32, BuildingState*> BuildingStateMap;
       typedef std::set<Creature*> CreatureSet;
       typedef std::set<GameObject*> GameObjectSet;
       typedef std::map<std::pair<uint32, bool>, Position> QuestGiverPositionMap;
       typedef std::map<uint32, Creature*> QuestGiverMap;
   public:
       OutdoorPvPWG();
       bool SetupOutdoorPvP();
       int TeamIDsound;
       uint32 GetCreatureEntry(uint32 guidlow, const CreatureData* data);

       void OnCreatureCreate(Creature* creature);
       void OnGameObjectCreate(GameObject* go);
       void OnCreatureRemove(Creature* creature);
       void OnGameObjectRemove(GameObject* go);

       void ProcessEvent(WorldObject* objin, uint32 eventId);

       void HandlePlayerEnterZone(Player* player, uint32 zone);
       void HandlePlayerLeaveZone(Player* player, uint32 zone);
       void HandlePlayerResurrects(Player* player, uint32 zone);
       void HandleKill(Player* killer, Unit* victim);

       bool Update(uint32 diff);

       void BroadcastStateChange(BuildingState* state) const;

       uint32 GetData(uint32 id);
       void SetData(uint32 id, uint32 value) {};

       void ModifyWorkshopCount(TeamId team, bool add);
       uint32 GetTimer() const { return m_timer / 1000; };
       bool IsWarTime() const { return m_wartime; };
       bool IsWarForTeamFull(Player* player);
       bool HasPlayerInWG(Player* player, bool inwar = false) const;
       void setTimer(uint32 timer) { if (timer >= 0) m_timer = timer; };
       uint32 GetNumPlayersA() const { return m_players[TEAM_ALLIANCE].size(); };
       uint32 GetNumPlayersH() const { return m_players[TEAM_HORDE].size(); };
       TeamId GetDefenderTeam() const { return m_defender; };
       TeamId GetAttackerTeam() const { return OTHER_TEAM(m_defender); };
       void ForceChangeTeam();
       void ForceStopBattle();
       void ForceStartBattle();

       // Temporal BG specific till 3.2
       void SendAreaSpiritHealerQueryOpcode(Player* player, const uint64& guid);
       void AddPlayerToResurrectQueue(uint64 npc_guid, uint64 player_guid);
       void RemovePlayerFromResurrectQueue(uint64 player_guid);
       void RelocateAllianceDeadPlayers(Creature* cr);
       void RelocateHordeDeadPlayers(Creature* cr);
       void InvitePlayerToQueue(Player* player);
       void InvitePlayerToWar(Player* player);
       void PlayerAcceptInviteToQueue(Player* player);
       void PlayerAcceptInviteToWar(Player* player);
       void KickPlayerFromWintergrasp(uint64 guid);
       void PlayerAskToLeave(Player* player);
       bool IncrementQuest(Player* player, uint32 quest, bool complete = false);
       void DoCompleteOrIncrementAchievement(uint32 achievement, Player* player, uint8 incrementNumber = 1);

       Group* GetGroupPlayer(uint64 guid, uint32 TeamId);
       Group* GetFreeBfRaid(uint32 TeamId);
       bool AddOrSetPlayerToCorrectBfGroup(Player* player);

       void ResetCreatureEntry(Creature* creature, uint32 entry);
       void SendInitWorldStatesTo(Player* player = NULL) const;
       uint32 m_timer;
   protected:
       std::vector<uint64> m_ResurrectQueue;
       uint32 LastResurrectTime;
       uint32 minlevel;
       uint32 m_MaxPlayer; 

       std::map<uint64, std::vector<uint64> >  m_ReviveQueue;

       uint32 GetLastResurrectTime() const { return LastResurrectTime; }
       uint32 GetReviveQueueSize() const { return m_ReviveQueue.size(); }

       TeamId m_defender;
       int32 m_tenacityStack;

       BuildingStateMap BuildingStates;
       BuildingState* m_gate;

       CreatureSet m_creatures;
       CreatureSet vehicles[2];
       GameObjectSet m_gobjects;
       QuestGiverMap m_questgivers;

       TeamPairMap m_creEntryPair, m_goDisplayPair;

       PlayerTimerMap m_InvitedPlayers[2];
       PlayerTimerMap m_PlayersWillBeKick[2];
       PlayerSet m_PlayersInWar[2];
       GuidSet m_PlayersInQueue[2];

       GuidSet m_Groups[2];
       uint32 StartGroupingTimer;
       bool m_StartGrouping;

       void InvitePlayerInQueueToWar();
       void InvitePlayerInZoneToQueue();
       void InvitePlayerInZoneToWar();

       QuestGiverPositionMap m_qgPosMap;

       bool m_wartime;
       bool m_changeDefender;
       uint32 m_clock[2];
       uint32 m_workshopCount[2];
       uint32 m_towerDestroyedCount[2];
       uint32 m_towerDamagedCount[2];
       uint32 WorldStateSaveTimer;
       uint32 m_TimeForAcceptInvite;
       uint32 KickDontAcceptTimer;
       uint32 KickAfkTimer;

       OPvPCapturePointWG* GetWorkshop(uint32 lowguid) const;
       OPvPCapturePointWG* GetWorkshopByEngGuid(uint32 lowguid) const;
       OPvPCapturePointWG* GetWorkshopByGOGuid(uint64 lowguid) const;

       void StartBattle();
       void EndBattle();
       void UpdateClock();
       void UpdateClockDigit(uint32 &timer, uint32 digit, uint32 mod);
       void PromotePlayer(Player* player) const;
       void UpdateTenacityStack();
       void UpdateAllWorldObject();
       bool UpdateCreatureInfo(Creature* creature);
       bool UpdateGameObjectInfo(GameObject* go) const;

       bool CanBuildVehicle(OPvPCapturePointWG* workshop) const;
       OutdoorPvPWGCreType GetCreatureType(uint32 entry) const;

       void RebuildAllBuildings();
       void RemoveOfflinePlayerWGAuras();
       void RewardMarkOfHonor(Player* player, uint32 count);
       void MoveQuestGiver(uint32 guid);
       void LoadQuestGiverMap(uint32 guid, Position PosHorde, Position PosAlliance);
       bool UpdateQuestGiverPosition(uint32 guid, Creature* creature);
       void KickAfk();
};

class OPvPCapturePointWG : public OPvPCapturePoint
{
   public:
       explicit OPvPCapturePointWG(OutdoorPvPWG* outdoorPvP, BuildingState* state);
       void SetTeamByBuildingState();
       void ChangeState() {}
       void ChangeTeam(TeamId oldteam);

       uint32* m_spiEntry;
       uint32 m_spiGuid;
       Creature* m_spiritguide;
       Creature* m_spiritguide_horde;
       Creature* m_spiritguide_alliance;

       uint32* m_engEntry;
       uint32 m_engGuid;
       Creature* m_engineer;
       uint32 m_workshopGuid;
       BuildingState* m_buildingState;
   protected:
       OutdoorPvPWG* m_wintergrasp;
};

#endif