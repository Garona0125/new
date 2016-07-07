/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "MapManager.h"
#include "Transport.h"
#include "Group.h"
#include "ObjectMgr.h"
#include "Object.h"
#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "QuantumGossip.h"
#include "SpellAuras.h"
#include "SpellScript.h"
#include "Vehicle.h"
#include "MoveSpline.h"
#include "icecrown_citadel.h"

enum Actions
{
    ACTION_INTRO_START             = 1,
    ACTION_BATTLE_EVENT            = 2,
    ACTION_MAGE_DIE                = 3,
    ACTION_ROCK_DIE                = 4,
    ACTION_AXES_RIFL_DIE           = 5,
    ACTION_DONE                    = 6,
    ACTION_FAIL                    = 7,
};

enum Texts
{
    // Muradin Bronzebeard
    SAY_INTRO_ALLIANCE_0          = -1409019,
    SAY_INTRO_ALLIANCE_1          = -1409020,
    SAY_INTRO_ALLIANCE_2          = -1409021,
    SAY_INTRO_ALLIANCE_3          = -1409022,
    SAY_INTRO_ALLIANCE_4          = -1409023,
    SAY_INTRO_ALLIANCE_5          = -1409024,
    SAY_INTRO_ALLIANCE_7          = -1409025,
    SAY_INTRO_HORDE_3             = -1409026,
    SAY_BOARDING_SKYBREAKER_1     = -1409027,
    SAY_BOARDING_ORGRIMS_HAMMER_0 = -1409028,
    SAY_NEW_RIFLEMAN_SPAWNED      = -1409029,
    SAY_NEW_MORTAR_TEAM_SPAWNED   = -1409030,
    SAY_NEW_MAGE_SPAWNED          = -1409031,
    SAY_ALLIANCE_VICTORY          = -1409032,
    SAY_ALLIANCE_DEFEAT           = -1409033,
    // High Overlord Saurfang
    SAY_INTRO_HORDE_0             = -1409034,
    SAY_INTRO_HORDE_0_1           = -1409035,
    SAY_INTRO_HORDE_1             = -1409036,
    SAY_INTRO_HORDE_2             = -1409037,
    SAY_INTRO_HORDE_4             = -1409038,
    SAY_BOARDING_SKYBREAKER_0     = -1409039,
    SAY_BOARDING_ORGRIMS_HAMMER_1 = -1409040,
    SAY_NEW_AXETHROWER_SPAWNED    = -1409041,
    SAY_NEW_ROCKETEERS_SPAWNED    = -1409042,
    SAY_NEW_BATTLE_MAGE_SPAWNED   = -1409043,
    SAY_HORDE_VICTORY             = -1409044,
    SAY_HORDE_DEFEAT              = -1409045,
	SAY_INTRO_ALLIANCE_6          = -1409046,
};

enum Spells
{
    SPELL_BURNING_PITCH_A             = 71335,
    SPELL_BURNING_PITCH_H             = 71339,
    SPELL_BURNING_PITCH_SIEGE_DMG_A   = 70383,
    SPELL_BURNING_PITCH_SIEGE_DMG_H   = 70374,
    SPELL_BURNING_PITCH_AOE_DAMAGE    = 69660,
    SPELL_WOUNDING_STRIKE_10_NM       = 69651,
    SPELL_WOUNDING_STRIKE_10_HM       = 72570,
    SPELL_WOUNDING_STRIKE_25_NM       = 72569,
    SPELL_WOUNDING_STRIKE_25_HM       = 72571,
    SPELL_TELEPORT_VISUAL             = 64446,
    SPELL_BLADESTORM                  = 69652,
    SPELL_BLADESTORM_TRIGGER          = 69653,
    SPELL_HEAT_DRAIN                  = 69470,
    SPELL_OVERHEAT                    = 69487,
    SPELL_CANNON_BLAST                = 69399,
	SPELL_CANNON_BLAST_PROC_10        = 69400,
	SPELL_CANNON_BLAST_PROC_25        = 70173,
    SPELL_INCINERATING_BLAST          = 69401,
    SPELL_ON_ORGRIMS_HAMMERS_DECK     = 70121,
    SPELL_ON_SKYBREAKERS_DECK         = 70120,
    SPELL_CLEAVE                      = 15284,
    SPELL_RENDING_THROW               = 70309,
    SPELL_TASTE_OF_BLOOD              = 69634,
	SPELL_BATTLE_FURY                 = 69638,
    SPELL_BELOW_ZERO                  = 69705,
    SPELL_SHADOW_CHANNELING           = 45104,
    SPELL_EXPERIENCED                 = 71188,
    SPELL_VETERAN                     = 71193,
    SPELL_ELITE                       = 71195,
    SPELL_DESPERATE_RESOLVE_10_NM     = 69647,
    SPELL_DESPERATE_RESOLVE_10_HM     = 72537,
    SPELL_DESPERATE_RESOLVE_25_NM     = 72536,
    SPELL_DESPERATE_RESOLVE_25_HM     = 72538,
    SPELL_HURL_AXE_10                 = 70161,
	SPELL_HURL_AXE_25                 = 72540,
    SPELL_SHOOT_10                    = 70162,
	SPELL_SHOOT_25                    = 72567,
    SPELL_ROCKET_ARTILLERY_MARKER     = 71371,
    SPELL_ROCKET_ARTILLERY_TRIGGERED  = 69679,
    SPELL_ROCKET_ARTILLERY_HORDE      = 69678,
    SPELL_ROCKET_ARTILLERY_ALLIANCE   = 70609,
    SPELL_EXPLOSION                   = 69680,
    SPELL_SHIP_EXPLOSION              = 72137,
    SPELL_ROCKET_PACK                 = 68721,
    SPELL_REMOVE_ROCKET_PACK          = 70713,
    SPELL_ACHIEVEMENT_CHECK           = 72959,
};

enum Events
{
    EVENT_NONE,
    EVENT_INTRO_ALLIANCE_1,
    EVENT_INTRO_ALLIANCE_2,
    EVENT_INTRO_ALLIANCE_3,
    EVENT_INTRO_ALLIANCE_4,
    EVENT_INTRO_ALLIANCE_5,
    EVENT_INTRO_ALLIANCE_6,
    EVENT_INTRO_ALLIANCE_7,
    EVENT_INTRO_ALLIANCE_8,

    EVENT_INTRO_HORDE_1,
    EVENT_INTRO_HORDE_1_1,
    EVENT_INTRO_HORDE_2,
    EVENT_INTRO_HORDE_3,
    EVENT_INTRO_HORDE_4,
    EVENT_INTRO_HORDE_5,

    EVENT_OUTRO_ALLIANCE_1,
    EVENT_OUTRO_ALLIANCE_2,
    EVENT_OUTRO_ALLIANCE_3,

    EVENT_OUTRO_HORDE_1,
    EVENT_OUTRO_HORDE_2,
    EVENT_OUTRO_HORDE_3,

    EVENT_START_FLY,
    EVENT_WALK_MOBS,
    EVENT_SUMMON_PORTAL,
    EVENT_FREEZE_CANNON,
    EVENT_SHADOW_CHANNELING,
    EVENT_UNSUMMON_PORTAL,
    EVENT_BOARDING_GUNSHIP,
    EVENT_BOARDING_TALK,
    EVENT_BURNING_PITCH,
    EVENT_WOUNDING_STRIKE,
    EVENT_BLADE_STORM,
    EVENT_DONE,
    EVENT_FAIL,
    EVENT_RESTART_EVENT,
    EVENT_BOARDING_REAVERS_MARINE,
    EVENT_WIPE_CHECK,
    EVENT_EXPERIENCED,
    EVENT_VETERAN,
    EVENT_ELITE,
    EVENT_ATACK_START,
    EVENT_HURL_AXE,
    EVENT_SHOOT,
    EVENT_ROCKET_ART,
    EVENT_RENDING_THROW,
    EVENT_TASTE_OF_BLOOD,
    EVENT_SPAWN_MAGE,
    EVENT_RESPAWN_AXES_RIFLEMAN,
    EVENT_RESPAWN_ROCKETEER,
};

struct mortarMarksLoc
{
    uint32 durationBeforeRefreshing;
    Position location;
};

// Function find player special for Gunship battle
typedef std::list<Player*> TPlayerLists;

TPlayerLists GetPlayersInTheMaps(Map* map)
{
    TPlayerLists players;
    const Map::PlayerList &PlayerList = map->GetPlayers();
    if (!PlayerList.isEmpty())
	{
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
		{
            if (Player* player = i->getSource())
                players.push_back(player);
		}
	}
    return players;
}

Player* SelectRandomPlayerFromLists(TPlayerLists &players)
{
    if (players.empty())
        return NULL;

    TPlayerLists::iterator it = players.begin();
    std::advance(it, urand(0, players.size()-1));
    return *it;
}

Player* SelectRandomPlayerInTheMaps(Map* map)
{
    TPlayerLists players = GetPlayersInTheMaps(map);
    return SelectRandomPlayerFromLists(players);
}

// Function start motion of the ship
void StartFlyShip(Transport* t)
{
    t->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
    t->SetGoState(GO_STATE_ACTIVE);
    t->SetUInt32Value(GAMEOBJECT_DYNAMIC, 0x10830010); // Seen in sniffs
    t->SetFloatValue(GAMEOBJECT_PARENTROTATION + 3, 1.0f);

    Map* map = t->GetMap();
    std::set<uint32> mapsUsed;
    GameObjectTemplate const* goinfo = t->GetGOInfo();

    t->GenerateWaypoints(goinfo->moTransport.taxiPathId, mapsUsed);

    for (Map::PlayerList::const_iterator itr = map->GetPlayers().begin(); itr != map->GetPlayers().end(); ++itr)
    {
        if (Player* player = itr->getSource())
        {
            UpdateData transData;
            t->BuildCreateUpdateBlockForPlayer(&transData, player);
            WorldPacket packet;
            transData.BuildPacket(&packet);
            player->SendDirectMessage(&packet);
        }
    }
}

void UpdateTransportMotionInMap(Transport* t)
{
    Map* map = t->GetMap();

    for (Map::PlayerList::const_iterator itr = map->GetPlayers().begin(); itr != map->GetPlayers().end(); ++itr)
    {
        if (Player* player = itr->getSource())
        {
            UpdateData transData;
            t->BuildCreateUpdateBlockForPlayer(&transData, player);
            WorldPacket packet;
            transData.BuildPacket(&packet);
            player->SendDirectMessage(&packet);
        }
    }
}

void RelocateTransport(Transport* t)
{
    Map* map = t->GetMap();
    InstanceScript* instance = t->GetInstanceScript();

    if (!t || !instance)
        return;

    // Transoprt movemend on server-side is ugly hack, so we do sincronize positions
    switch (t->GetEntry())
    {
        case GO_THE_SKYBREAKER_ALLIANCE_ICC:
            if (instance->GetBossState(DATA_GUNSHIP_EVENT) != DONE)
                t->Relocate(-377.184021f, 2073.548584f, 445.753387f);
            else if (instance->GetBossState(DATA_GUNSHIP_EVENT) == DONE)
                t->Relocate(-583.942627f, 2212.364990f, 534.673889f);
            break;
        case GO_ORGRIM_S_HAMMER_ALLIANCE_ICC:
            if (instance->GetBossState(DATA_GUNSHIP_EVENT) != DONE)
                t->Relocate(-384.878479f, 1989.831665f, 431.549438f);
            break;
        case GO_ORGRIM_S_HAMMER_HORDE_ICC:
            if (instance->GetBossState(DATA_GUNSHIP_EVENT) != DONE)
                t->Relocate(-438.142365f, 2395.725830f, 436.781647f);
            else if (instance->GetBossState(DATA_GUNSHIP_EVENT) == DONE)
                t->Relocate(-583.942627f, 2212.364990f, 534.673889f);
            break;
        case GO_THE_SKYBREAKER_HORDE_ICC:
            if (instance->GetBossState(DATA_GUNSHIP_EVENT) != DONE)
                t->Relocate(-435.854156f, 2475.328125f, 449.364105f);
            break;
    }

    t->Update(0);
    t->UpdateNPCPositions();
}

// Function stop motion of the ship
void StopFlyShip(Transport* t)
{
    Map* map = t->GetMap();
    t->m_WayPoints.clear();
    RelocateTransport(t);
    t->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
    t->SetGoState(GO_STATE_READY);

    for (Map::PlayerList::const_iterator itr = map->GetPlayers().begin(); itr != map->GetPlayers().end(); ++itr)
    {
        if (Player* player = itr->getSource())
        {
            UpdateData transData;
            t->BuildCreateUpdateBlockForPlayer(&transData, player);
            WorldPacket packet;
            transData.BuildPacket(&packet);
            player->SendDirectMessage(&packet);
        }
    }
}

// Find Unfriendy transport
Transport* CheckUnfriendlyShip(Creature* me, InstanceScript* instance, uint32 data)
{
    if (Creature* captain = ObjectAccessor::GetCreature(*me, instance->GetData64(data)))
        return captain->GetTransport();
    else
        return NULL;
}

// Teleport players
void TeleportPlayers(Map* map, uint64 TeamInInstance)
{
    if (map)
    {
        Map::PlayerList const &lPlayers = map->GetPlayers();
        if (!lPlayers.isEmpty())
        {
            for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            {
                if (Player* player = itr->getSource())
                {
                    if (player->IsDead() && !player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
					{
                        player->ResurrectPlayer(1.0f);
						player->CastSpell(player, 6962, true);
						player->CastSpell(player, SPELL_SPIRIT_HEAL_MANA, true);
						player->CastSpell(player, SPELL_RESURRECTION_VISUAL, true);
					}

                    if (TeamInInstance == ALLIANCE)
                        player->TeleportTo(631, -437.498505f, 2425.954f, 192.997f, 2.247f);

					if (TeamInInstance == HORDE)
                        player->TeleportTo(631, -437.498505f, 1997.954f, 192.997f, 2.247f);

					player->RemoveAllAttackers();
					player->AttackStop();
					player->DeleteThreatList();
					player->CombatStop();
                }
            }
        }
    }
}

// Ship explosion
void DoShipExplosion(Transport* t)
{
    for (Transport::CreatureSet::iterator itr = t->m_NPCPassengerSet.begin(); itr != t->m_NPCPassengerSet.end();)
    {
        if (Creature* hull = *itr)
        {
            if (hull->GetEntry() == NPC_GB_GUNSHIP_HULL)
                hull->CastSpell(hull, SPELL_SHIP_EXPLOSION, true);
        }
        ++itr;
    }
}

// Wipe check
bool DoWipeCheck(Transport* t)
{
    for (Transport::PlayerSet::const_iterator itr = t->GetPassengers().begin(); itr != t->GetPassengers().end();)
    {
        Player* plr = *itr;
        ++itr;

        if (plr && plr->IsAlive())
            return true;
    }
    return false;
}

// Check falling players
void DoCheckFallingPlayer(Creature* me)
{
    Map* map = me->GetMap();

    if (map)
    {
        Map::PlayerList const &lPlayers = map->GetPlayers();
        if (!lPlayers.isEmpty())
        {
            for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            {
                if (Player* player = itr->getSource())
                {
                    if (player->GetPositionZ() < 420.0f && player->IsWithinDistInMap(me, 300.0f))
                        player->NearTeleportTo(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 5.0f, me->GetOrientation());
                }
            }
        }
    }
}

// Restart event
void RestartEvent(Transport* t1, Transport* t2, Map* instance, uint64 TeamInInstance)
{
    sMapMgr->UnLoadTransportFromMap(t1);
    sMapMgr->UnLoadTransportFromMap(t2);
	sMapMgr->LoadTransports(); // Try

    Map::PlayerList const& players = instance->GetPlayers();
    if (players.isEmpty())
        return;

                if (TeamInInstance == ALLIANCE)
                {
                    if (Transport* th = sMapMgr->LoadTransportInMap(instance, GO_ORGRIM_S_HAMMER_ALLIANCE_ICC, 108000))
                    {
                        th->AddNPCPassengerInInstance(NPC_GB_ORGRIMS_HAMMER, 1.845810f, 1.268872f, 34.526218f, 1.5890f);
                        th->AddNPCPassengerInInstance(NPC_GB_HIGH_OVERLORD_SAURFANG, 37.18615f, 0.00016f, 36.78849f, 3.13683f);
                        th->AddNPCPassengerInInstance(NPC_GB_INVISIBLE_STALKER, 37.18615f, 0.00016f, 36.78849f, 3.13683f);
                        th->AddNPCPassengerInInstance(NPC_GB_KORKRON_BATTLE_MAGE, 47.2929f, -4.308941f, 37.5555f, 3.05033f);
                        th->AddNPCPassengerInInstance(NPC_GB_KORKRON_BATTLE_MAGE, 47.34621f, 4.032004f, 37.70952f, 3.05033f);
                        th->AddNPCPassengerInInstance(NPC_GB_KORKRON_BATTLE_MAGE, 15.03016f, 0.00016f, 37.70952f, 1.55138f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -13.19547f, -27.160213f, 35.47252f, 3.10672f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -18.33902f, -25.230491f, 33.04052f, 3.00672f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -60.1251f, -1.27014f, 42.8335f, 5.16073f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -48.2651f, 16.78034f, 34.2515f, 0.04292f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -14.8356f, 27.931688f, 33.363f, 1.73231f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 10.2702f, 20.62966f, 35.37483f, 1.6f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 39.32459f, 14.50176f, 36.88428f, 1.6f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 46.17223f, -6.638763f, 37.35444f, 1.32f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 27.4456f, -13.397498f, 36.34746f, 1.6f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 18.16184f, 1.37897f, 35.31705f, 1.6f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -18.11516f, -0.196236f, 45.15709f, 2.9f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -18.11844f, -0.19624f, 49.18192f, 1.6f);

                        if (instance->ToInstanceMap()->GetMaxPlayers() == 10)
                        {
							th->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, -3.170555f, 28.30652f, 34.21082f, 1.66527f);
							th->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, -12.0928f, 27.65942f, 33.58557f, 1.66527f);
							th->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, 14.92804f, 26.18018f, 35.47803f, 1.66527f);
							th->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, 24.70331f, 25.36584f, 35.97845f, 1.66527f);
							th->AddNPCPassengerInInstance(NPC_GB_KORKRON_ROCKETEER, -11.44849f, -25.71838f, 33.64343f, 1.49248f);
							th->AddNPCPassengerInInstance(NPC_GB_KORKRON_ROCKETEER, 12.30336f, -25.69653f, 35.32373f, 1.49248f);
                        }                        
                        else
                        {
							th->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, -3.170555f, 28.30652f, 34.21082f, 1.66527f);
							th->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, -12.0928f, 27.65942f, 33.58557f, 1.66527f);
							th->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, 14.92804f, 26.18018f, 35.47803f, 1.66527f);
							th->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, 24.70331f, 25.36584f, 35.97845f, 1.66527f);
							th->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, 19.92804f, 27.18018f, 35.47803f, 1.66527f);
							th->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, -7.70331f, 28.36584f, 33.88557f, 1.66527f);
							th->AddNPCPassengerInInstance(NPC_GB_KORKRON_ROCKETEER, -11.44849f, -25.71838f, 33.64343f, 1.49248f);
							th->AddNPCPassengerInInstance(NPC_GB_KORKRON_ROCKETEER, 12.30336f, -25.69653f, 35.32373f, 1.49248f);
							th->AddNPCPassengerInInstance(NPC_GB_KORKRON_ROCKETEER, -3.44849f, -25.71838f, 34.21082f, 1.49248f);
							th->AddNPCPassengerInInstance(NPC_GB_KORKRON_ROCKETEER, 3.30336f, -25.69653f, 35.32373f, 1.49248f);
                        }
                    }

                    if (Transport* t = sMapMgr->LoadTransportInMap(instance, GO_THE_SKYBREAKER_ALLIANCE_ICC, 108000))
                    {
                        t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER, -17.156807f, -1.633260f, 20.81273f, 4.52672f);
                        t->AddNPCPassengerInInstance(NPC_GB_MURADIN_BRONZEBEARD, 13.51547f, -0.160213f, 20.87252f, 3.10672f);
                        t->AddNPCPassengerInInstance(NPC_GB_HIHG_CAPTAIN_JUSTIN_BARTLETT, 42.78902f, -0.010491f, 25.24052f, 3.00672f);
                        //t->AddNPCPassengerInInstance(NPC_GB_HIGH_OVERLORD_SAURFANG_NOT_VISUAL, -12.9806f, -22.9462f, 21.659f, 4.72416f);
                        t->AddNPCPassengerInInstance(NPC_GB_ZAFOD_BOOMBOX, 18.8042f, 9.907914f, 20.33559f, 3.10672f);
                        t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_DECKHAND, -64.8423f, 4.4658f, 23.4352f, 2.698897f);
                        t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_DECKHAND, 35.54972f, 19.93269f, 25.0333f, 4.71242f);
                        t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_DECKHAND, -36.39837f, 3.13127f, 20.4496f, 1.5708f);
                        t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_DECKHAND, -36.23974f, -2.75767f, 20.4506f, 4.69496f);
                        t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_DECKHAND, 41.94677f, 44.08411f, 24.66587f, 1.62032f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 13.51547f, -0.160213f, 20.87252f, 3.10672f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 42.78902f, -0.010491f, 25.24052f, 3.00672f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 14.0551f, 3.65014f, 20.7935f, 3.16073f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 14.0551f, -4.65034f, 20.7915f, 3.04292f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -17.8356f, 0.031688f, 20.823f, 4.73231f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -34.2702f, -26.18966f, 21.37483f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -11.64459f, -19.85176f, 20.88428f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -19.88223f, -6.578763f, 20.57444f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -41.4456f, -7.647498f, 20.49746f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 0.554884f, -1.232897f, 20.53705f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -50.16516f, 9.716236f, 23.58709f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 11.45844f, 16.36624f, 20.54192f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 19.72286f, -2.193787f, 33.06982f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 19.72286f, -2.193787f, 33.06982f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 8.599396f, -28.55855f, 24.79919f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 38.94339f, -33.808f,  25.39618f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 58.15474f, 0.748094f, 41.87663f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 5.607554f, -6.350654f, 34.00357f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 4.780305f, -29.05227f, 35.09634f, 1.6f);

                        if (instance->ToInstanceMap()->GetMaxPlayers() == 10)
                        {
                            t->AddNPCPassengerInInstance(NPC_GB_ALLIANCE_CANON, -5.15231f, -22.9462f, 21.659f, 4.72416f);
                            t->AddNPCPassengerInInstance(NPC_GB_ALLIANCE_CANON, -28.0876f, -22.9462f, 21.659f, 4.72416f);
                        }
                        else
                        {
                            t->AddNPCPassengerInInstance(NPC_GB_ALLIANCE_CANON, -5.15231f, -22.9462f, 21.659f, 4.72416f);
                            t->AddNPCPassengerInInstance(NPC_GB_ALLIANCE_CANON, -14.9806f, -22.9462f, 21.659f, 4.72416f);
                            t->AddNPCPassengerInInstance(NPC_GB_ALLIANCE_CANON, -21.7406f, -22.9462f, 21.659f, 4.72416f);
                            t->AddNPCPassengerInInstance(NPC_GB_ALLIANCE_CANON, -28.0876f, -22.9462f, 21.659f, 4.72416f);
                        }
                    }
                }

                if (TeamInInstance == HORDE)
                {
                    if (Transport* t = sMapMgr->LoadTransportInMap(instance, GO_THE_SKYBREAKER_HORDE_ICC, 77800))
                    {
                        t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER, -17.156807f, -1.633260f, 20.81273f, 4.52672f);
                        t->AddNPCPassengerInInstance(NPC_GB_MURADIN_BRONZEBEARD, 13.51547f, -0.160213f, 20.87252f, 3.10672f);
                        t->AddNPCPassengerInInstance(NPC_GB_HIHG_CAPTAIN_JUSTIN_BARTLETT, 42.78902f, -0.010491f, 25.24052f, 3.00672f);
                        t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_SORCERERS, 14.0551f, 3.65014f, 20.7935f, 3.16073f);
                        t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_SORCERERS, 14.0551f, -4.65034f, 20.7915f, 3.04292f);
                        t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_SORCERERS, -17.8356f, 0.031688f, 20.823f, 4.73231f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 13.51547f, -0.160213f, 20.87252f, 3.10672f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 42.78902f, -0.010491f, 25.24052f, 3.00672f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 14.0551f, 3.65014f, 20.7935f, 3.16073f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 14.0551f, -4.65034f, 20.7915f, 3.04292f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -17.8356f, 0.031688f, 20.823f, 4.73231f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -34.2702f, -26.18966f, 21.37483f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -11.64459f, -19.85176f, 20.88428f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -19.88223f, -6.578763f, 20.57444f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -41.4456f, -7.647498f, 20.49746f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 0.554884f, -1.232897f, 20.53705f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -50.16516f, 9.716236f, 23.58709f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 11.45844f, 16.36624f, 20.54192f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 19.72286f, -2.193787f, 33.06982f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 19.72286f, -2.193787f, 33.06982f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 8.599396f, -28.55855f, 24.79919f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 38.94339f, -33.808f,  25.39618f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 58.15474f, 0.748094f, 41.87663f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 5.607554f, -6.350654f, 34.00357f, 1.6f);
                        t->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 4.780305f, -29.05227f, 35.09634f, 1.6f);

                        if (instance->ToInstanceMap()->GetMaxPlayers() == 10)
                        {
							t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -5.15231f, -22.9462f, 21.659f, 4.72416f);
							t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -14.9806f, -22.9462f, 21.659f, 4.72416f);
							t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -21.7406f, -22.9462f, 21.659f, 4.72416f);
							t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -28.0876f, -22.9462f, 21.659f, 4.72416f);
							t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, -8.61003f, 15.483f, 20.4158f, 4.69854f);
							t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, -27.9583f, 14.8875f, 20.4428f, 4.77865f);
                        }
                        else
                        {
							t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, 0.15231f, -22.9462f, 21.659f, 4.72416f);
							t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -5.15231f, -22.9462f, 21.659f, 4.72416f);
							t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -14.9806f, -22.9462f, 21.659f, 4.72416f);
							t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -21.7406f, -22.9462f, 21.659f, 4.72416f);
							t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -28.0876f, -22.9462f, 21.659f, 4.72416f);
							t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -33.0876f, -22.9462f, 21.659f, 4.72416f);
							t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, -8.61003f, 15.483f, 20.4158f, 4.69854f);
							t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, -27.9583f, 14.8875f, 20.4428f, 4.77865f);
							t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, -15.61003f, 15.483f, 20.4158f, 4.69854f);
							t->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, -20.9583f, 14.8875f, 20.4428f, 4.77865f);
                        }
                    }

                    if (Transport* th = sMapMgr->LoadTransportInMap(instance,GO_ORGRIM_S_HAMMER_HORDE_ICC, 77800))
                    {
                        th->AddNPCPassengerInInstance(NPC_GB_ORGRIMS_HAMMER, 1.845810f, 1.268872f, 34.526218f, 1.5890f);
                        th->AddNPCPassengerInInstance(NPC_GB_HIGH_OVERLORD_SAURFANG, 37.18615f, 0.00016f, 36.78849f, 3.13683f);
                        //th->AddNPCPassengerInInstance(NPC_GB_MURADIN_BRONZEBEARD_NOT_VISUAL, -7.09684f, 30.582f, 34.5013f, 1.53591f);
                        th->AddNPCPassengerInInstance(NPC_GB_INVISIBLE_STALKER, 37.30764f, -0.143823f, 36.7936f, 3.13683f);
                        th->AddNPCPassengerInInstance(NPC_GB_ZAFOD_BOOMBOX, 35.18615f, 15.30652f, 37.64343f, 3.05033f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -13.19547f, -27.160213f, 35.47252f, 3.10672f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -18.33902f, -25.230491f, 33.04052f, 3.00672f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -60.1251f, -1.27014f, 42.8335f, 5.16073f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -48.2651f, 16.78034f, 34.2515f, 0.04292f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -14.8356f, 27.931688f, 33.363f, 1.73231f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 10.2702f, 20.62966f, 35.37483f, 1.6f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 39.32459f, 14.50176f, 36.88428f, 1.6f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 46.17223f, -6.638763f, 37.35444f, 1.32f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 27.4456f, -13.397498f, 36.34746f, 1.6f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, 18.16184f, 1.37897f, 35.31705f, 1.6f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -18.11516f, -0.196236f, 45.15709f, 2.9f);
                        th->AddNPCPassengerInInstance(NPC_GB_GUNSHIP_HULL, -18.11844f, -0.19624f, 49.18192f, 1.6f);

                        if (instance->ToInstanceMap()->GetMaxPlayers() == 10)
                        {
                            th->AddNPCPassengerInInstance(NPC_GB_HORDE_CANON, 22.6225f, 28.9309f, 36.3929f, 1.53591f);
                            th->AddNPCPassengerInInstance(NPC_GB_HORDE_CANON, -21.7509f, 29.4207f, 34.2588f, 1.53591f);
                        }
                        else
                        {
							th->AddNPCPassengerInInstance(NPC_GB_HORDE_CANON, 22.6225f, 28.9309f, 36.3929f, 1.53591f);
                            th->AddNPCPassengerInInstance(NPC_GB_HORDE_CANON, 9.87745f, 30.5047f, 35.7147f, 1.53591f);
                            th->AddNPCPassengerInInstance(NPC_GB_HORDE_CANON, -7.09684f, 30.582f, 34.5013f, 1.53591f);
                            th->AddNPCPassengerInInstance(NPC_GB_HORDE_CANON, -21.7509f, 29.4207f, 34.2588f, 1.53591f);
                        }
                    }
                }


}

// Stop Fight
void StopFight(Transport* t1, Transport* t2)
{
    Map* map = t1->GetMap();

    for (Transport::CreatureSet::iterator itr = t1->m_NPCPassengerSet.begin(); itr != t1->m_NPCPassengerSet.end();)
    {
        if (Creature* npc = *itr)
        {
            if (npc->GetEntry() != NPC_GB_SKYBREAKER && npc->GetEntry() != NPC_GB_ORGRIMS_HAMMER && npc->GetEntry() != NPC_GB_HIGH_OVERLORD_SAURFANG
            && npc->GetEntry() != NPC_GB_MURADIN_BRONZEBEARD && npc->GetEntry() != NPC_GB_GUNSHIP_HULL)
            {
                if (Vehicle* veh = npc->GetVehicleKit())
                    veh->RemoveAllPassengers();

                npc->Kill(npc);
				npc->DespawnAfterAction();
            }
        }
        ++itr;
    }

    for (Transport::CreatureSet::iterator itr = t2->m_NPCPassengerSet.begin(); itr != t2->m_NPCPassengerSet.end();)
    {
        if (Creature* npc = *itr)
        {
            if (npc->GetEntry() != NPC_GB_GUNSHIP_HULL && npc->GetEntry() != NPC_GB_SKYBREAKER && npc->GetEntry() != NPC_GB_ORGRIMS_HAMMER)
			{
                npc->Kill(npc);
				npc->DespawnAfterAction();
			}
        }
        ++itr;
    }

    for (Map::PlayerList::const_iterator itr = map->GetPlayers().begin(); itr != map->GetPlayers().end(); ++itr)
    {
        if (Player* player = itr->getSource())
            player->CombatStop();
    }
}

class npc_muradin_bronzebeard_gunship : public CreatureScript
{
    public:
        npc_muradin_bronzebeard_gunship() : CreatureScript("npc_muradin_bronzebeard_gunship") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            InstanceScript* instance = creature->GetInstanceScript();

            if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
            {
                if ((!player->GetGroup() || !player->GetGroup()->IsLeader(player->GetGUID())) && !player->IsGameMaster())
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I'm not the raid leader...", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                    return true;
                }

                player->ADD_GOSSIP_ITEM(0, "My companions are all accounted for, Muradin. Let's go!", 631, 1001);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                return true;
            }
            return false;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            InstanceScript* instance = creature->GetInstanceScript();

            player->PlayerTalkClass->ClearMenus();
            player->CLOSE_GOSSIP_MENU();

            if (action == GOSSIP_ACTION_INFO_DEF+2)
                creature->MonsterSay("I'll wait for the raid leader", LANG_UNIVERSAL, player->GetGUID());

            if (action == 1001)
            {
                creature->AI()->DoAction(ACTION_INTRO_START);
                creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
            return true;
        }

        struct npc_muradin_bronzebeard_gunshipAI : public QuantumBasicAI
        {
            npc_muradin_bronzebeard_gunshipAI(Creature* creature) : QuantumBasicAI(creature), instance(creature->GetInstanceScript()){}

			Transport* skybreaker;
			Map* map;
			EventMap events;
			InstanceScript* instance;

			uint32 SummonCount;
			uint32 count;
			uint32 RocketerDieCount;
			uint32 RiflDieCount;
			uint32 RocketerCount;
			uint32 RiflemanCount;

			bool EventScheduled;

            void Reset()
            {
                if (instance->GetBossState(DATA_GUNSHIP_EVENT) == IN_PROGRESS)
                    return;

                me->SetReactState(REACT_PASSIVE);
                me->SetCurrentFaction(1802);
                events.Reset();
                map = me->GetMap();
                skybreaker = me->GetTransport();
				UpdateTransportMotionInMap(skybreaker);
                SummonCount = RAID_MODE(3, 5, 4, 6);
                count = 0;
                RocketerCount = RAID_MODE(2, 4, 2, 4);
                RiflemanCount = RAID_MODE(4, 6, 4, 6);
                RocketerDieCount = 0;
                RiflDieCount = 0;
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                EventScheduled = false;
            }

			void DeletePlayerShirts()
			{
				Map::PlayerList const& Players = me->GetMap()->GetPlayers();
				for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
				{
					if (Player* player = itr->getSource())
						player->DestroyItemCount(ITEM_THE_ROCKET_PACK, 1, true);
				}
			}

            // Send packet to all players
            void SendPacketToPlayers(WorldPacket const* data) const
            {
                Map::PlayerList const& players = me->GetMap()->GetPlayers();
                if (!players.isEmpty())
				{
                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
					{
                        if (Player* player = itr->getSource())
                            player->GetSession()->SendPacket(data);
					}
				}
            }

            bool CanAIAttack(Unit const* target) const
            {
                if (target->GetEntry() == NPC_GB_KORKRON_SERGANTE || target->GetEntry() == NPC_GB_KORKRON_REAVERS)
                    return true;

                return false;
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_INTRO_START:
                        events.ScheduleEvent(EVENT_INTRO_ALLIANCE_1, 1000);
                        events.ScheduleEvent(EVENT_START_FLY, 2500);
                        events.ScheduleEvent(EVENT_INTRO_ALLIANCE_2, 7000);
                        events.ScheduleEvent(EVENT_INTRO_ALLIANCE_3, 28000);
                        events.ScheduleEvent(EVENT_INTRO_ALLIANCE_4, 35000);
                        events.ScheduleEvent(EVENT_INTRO_ALLIANCE_5, 40000);
                        events.ScheduleEvent(EVENT_INTRO_ALLIANCE_6, 47000);
                        events.ScheduleEvent(EVENT_INTRO_ALLIANCE_7, 53000);
                        events.ScheduleEvent(EVENT_INTRO_ALLIANCE_8, 58900);
                        break;
                    case ACTION_BATTLE_EVENT:
                    {
                        if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                            events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);

                        Creature* pAllianceBoss = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_SKYBREAKER_BOSS));
                        Creature* pHordeBoss = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_ORGRIMMAR_HAMMER_BOSS));

                        if (pHordeBoss && pAllianceBoss)
                        {
                             me->SetReactState(REACT_AGGRESSIVE);
                             pHordeBoss->SetInCombatWith(pAllianceBoss);
                             pAllianceBoss->SetInCombatWith(pHordeBoss);
                             pAllianceBoss->AddThreat(pHordeBoss, 0.0f);
                             pHordeBoss->AddThreat(pAllianceBoss, 0.0f);
                             instance->SetBossState(DATA_GUNSHIP_EVENT, IN_PROGRESS);
                             events.ScheduleEvent(EVENT_SUMMON_PORTAL, 30000);
                             RelocateTransport(skybreaker);
                             RelocateTransport(CheckUnfriendlyShip(me, instance, DATA_GB_HIGH_OVERLORD_SAURFANG));
                        }
                        else
                            me->AI()->DoAction(ACTION_FAIL);
                        break;
                    }
                    case ACTION_DONE:
                        if (Creature* pAllianceBoss = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_SKYBREAKER_BOSS)))
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, pAllianceBoss);

                        if (Creature* pHordeBoss = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_ORGRIMMAR_HAMMER_BOSS)))
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, pHordeBoss);

						DeletePlayerShirts();

                        me->SetReactState(REACT_PASSIVE);
                        instance->SetBossState(DATA_GUNSHIP_EVENT, DONE);
                        instance->DoCastSpellOnPlayers(SPELL_REMOVE_ROCKET_PACK);
                        DoSendQuantumText(SAY_ALLIANCE_VICTORY, me);
                        StartFlyShip(CheckUnfriendlyShip(me, instance, DATA_GB_HIGH_OVERLORD_SAURFANG));
                        StopFight(skybreaker, CheckUnfriendlyShip(me, instance, DATA_GB_HIGH_OVERLORD_SAURFANG));
                        events.CancelEvent(EVENT_WIPE_CHECK);
                        events.ScheduleEvent(EVENT_OUTRO_ALLIANCE_1, 3500);
                        events.ScheduleEvent(EVENT_OUTRO_ALLIANCE_2, 23000);
                        events.ScheduleEvent(EVENT_OUTRO_ALLIANCE_3, 32000);
                        break;
                    case ACTION_FAIL:
                        events.CancelEvent(EVENT_WIPE_CHECK);
                        DoSendQuantumText(SAY_ALLIANCE_DEFEAT, me);
                        if (Creature* pAllianceBoss = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_SKYBREAKER_BOSS)))
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, pAllianceBoss);

                        if (Creature* pHordeBoss = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_ORGRIMMAR_HAMMER_BOSS)))
                            instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, pHordeBoss);

                        StopFight(skybreaker, CheckUnfriendlyShip(me, instance, DATA_GB_HIGH_OVERLORD_SAURFANG));
                        events.ScheduleEvent(EVENT_FAIL, 10000);
                        break;
                    case ACTION_MAGE_DIE:
					    //Check, if there really is no mage
					    if (me->GetEntry() != NPC_GB_SKYBREAKER_SORCERERS)
						{
							if (instance->GetBossState(DATA_GUNSHIP_EVENT) != DONE)
					  			events.ScheduleEvent(EVENT_SPAWN_MAGE, 33000);
					    }
                        break;
                    case ACTION_ROCK_DIE:
                        ++RocketerDieCount; 
                        if (RocketerDieCount == RocketerCount)
							if (instance->GetBossState(DATA_GUNSHIP_EVENT) != DONE)
								events.ScheduleEvent(EVENT_RESPAWN_ROCKETEER, 60000);
                        break;
                    case ACTION_AXES_RIFL_DIE:
                        ++RiflDieCount;
                        if (RiflDieCount == RiflemanCount)
							if (instance->GetBossState(DATA_GUNSHIP_EVENT) != DONE)
                            events.ScheduleEvent(EVENT_RESPAWN_AXES_RIFLEMAN, 60000);
                        break;
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage)
            {
                if (instance->GetData(DATA_TEAM_IN_INSTANCE) == HORDE && me->GetHealthPct() < 2.0f)
                   damage = 0;
                
                if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE && me->GetHealthPct() < 2.0f)
                {
                   damage = 0;
                   me->AI()->DoAction(ACTION_FAIL);
                }
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (id = 1)
                    me->DespawnAfterAction(1*IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const diff)
            {
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                
                if (!HealthAbovePct(75))
                {
                    me->SetHealth(me->GetMaxHealth() / 100 * 76); // find a better way to avoid the hardcore spell spam ....
                    DoCast(me, SPELL_TASTE_OF_BLOOD);
                }

                if (UpdateVictim())
                {
                    if (!EventScheduled)
                    {
                        EventScheduled = true; // should temp fix the hardcore casting
                        events.ScheduleEvent(EVENT_RENDING_THROW, 3000);
                    }
                }

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_WIPE_CHECK:
                            DoCheckFallingPlayer(me);
                            if (DoWipeCheck(skybreaker))
                                events.ScheduleEvent(EVENT_WIPE_CHECK, 3000);
                            else
                                me->AI()->DoAction(ACTION_FAIL);
                            break;
                        case EVENT_INTRO_ALLIANCE_1:
                            DoSendQuantumText(SAY_INTRO_ALLIANCE_0, me);
                            break;
                        case EVENT_START_FLY:
                            StartFlyShip(skybreaker);
                            break;
                        case EVENT_INTRO_ALLIANCE_2:
                            DoSendQuantumText(SAY_INTRO_ALLIANCE_1, me);
                            break;
                        case EVENT_INTRO_ALLIANCE_3:
                            DoSendQuantumText(SAY_INTRO_ALLIANCE_2, me);
                            StartFlyShip(CheckUnfriendlyShip(me, instance, DATA_GB_HIGH_OVERLORD_SAURFANG));
                            break;
                        case EVENT_INTRO_ALLIANCE_4:
                            DoSendQuantumText(SAY_INTRO_ALLIANCE_3, me);
                            break;
                        case EVENT_INTRO_ALLIANCE_5:
                            StopFlyShip(skybreaker);
                            StopFlyShip(CheckUnfriendlyShip(me, instance, DATA_GB_HIGH_OVERLORD_SAURFANG));
                            DoSendQuantumText(SAY_INTRO_ALLIANCE_4, me);
                            break;
                        case EVENT_INTRO_ALLIANCE_6:
                            DoSendQuantumText(SAY_INTRO_ALLIANCE_5, me);
							me->GetMap()->SetZoneMusic(AREA_ICECROWN_CITADEL, SOUND_ID_GUNSHIP_BATTLE);
                            me->AI()->DoAction(ACTION_BATTLE_EVENT);
                            break;
                        case EVENT_INTRO_ALLIANCE_7:
                            if (Creature* saurfang = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_GB_HIGH_OVERLORD_SAURFANG))) // DATA_HIGH_OVERLORD_SAURFANG_NOT_VISUAL
                            {
								DoSendQuantumText(SAY_INTRO_ALLIANCE_6, saurfang);
                                saurfang->SetReactState(REACT_PASSIVE);
                                saurfang->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                            }
                            break;
                        case EVENT_INTRO_ALLIANCE_8:
                            DoSendQuantumText(SAY_INTRO_ALLIANCE_7, me);
                            break;
                        case EVENT_SUMMON_PORTAL:
                            if (instance->GetBossState(DATA_GUNSHIP_EVENT) != DONE)
                            {
                                events.ScheduleEvent(EVENT_BOARDING_TALK, 10000);
                                events.ScheduleEvent(EVENT_BOARDING_GUNSHIP, 2500); // TODO: Fix the timers
                                skybreaker->AddNPCPassengerInInstance(NPC_GB_PORTAL, -15.51547f, -0.160213f, 28.87252f, 1.56211f);
                                CheckUnfriendlyShip(me, instance, DATA_GB_HIGH_OVERLORD_SAURFANG)->AddNPCPassengerInInstance(NPC_GB_PORTAL, 47.55099f, -0.101778f, 37.61111f, 1.55138f);
                            }
                            break;
                        case EVENT_RENDING_THROW:
                            if (UpdateVictim())
                                if (me->GetVictim()->IsWithinDistInMap(me, 30.0f, false))
                                {
                                    DoCastVictim(SPELL_RENDING_THROW);
                                    EventScheduled = false;
                                }
                                else
                                    events.CancelEvent(EVENT_RENDING_THROW);
                            break;
                        case EVENT_TASTE_OF_BLOOD:
                            DoCast(me, SPELL_TASTE_OF_BLOOD, true);
							DoCast(me, SPELL_BATTLE_FURY, true);
                            break;
                        case EVENT_BOARDING_TALK:
                            DoSendQuantumText(SAY_BOARDING_SKYBREAKER_1, me);
                            break;
                        case EVENT_BOARDING_GUNSHIP:
							if (instance->GetBossState(DATA_GUNSHIP_EVENT) != DONE)
							{
								if (Creature* saurfang = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_GB_HIGH_OVERLORD_SAURFANG))) // DATA_HIGH_OVERLORD_SAURFANG_NOT_VISUAL
									DoSendQuantumText(SAY_BOARDING_SKYBREAKER_0, saurfang);

								if (Creature* Sergante = skybreaker->AddNPCPassengerInInstance(NPC_GB_KORKRON_SERGANTE, -15.51547f, -0.160213f, 20.87252f, 1.56211f))
									Sergante->CastSpell(Sergante, SPELL_TELEPORT_VISUAL, true);

								events.ScheduleEvent(EVENT_SUMMON_PORTAL, 90000);
								events.ScheduleEvent(EVENT_BOARDING_REAVERS_MARINE, 3000);
								count = 0;
							}
                            break;
                        case EVENT_BOARDING_REAVERS_MARINE:
							if (instance->GetBossState(DATA_GUNSHIP_EVENT) != DONE)
							{
								if (count <= SummonCount)
								{
									if (Creature* Reavers = skybreaker->AddNPCPassengerInInstance(NPC_GB_KORKRON_REAVERS, -15.51547f, -0.160213f, 20.87252f, 1.56211f))
									{
										Reavers->CastSpell(Reavers, SPELL_TELEPORT_VISUAL, true);
										events.ScheduleEvent(EVENT_BOARDING_REAVERS_MARINE, 21000 / SummonCount);
										++count;
									}
								}
							}
                            break;
                        case EVENT_OUTRO_ALLIANCE_1:
                            instance->DoCompleteAchievement(RAID_MODE(ACHIEVEMENT_IM_ON_A_BOAT_10, ACHIEVEMENT_IM_ON_A_BOAT_25, ACHIEVEMENT_IM_ON_A_BOAT_10, ACHIEVEMENT_IM_ON_A_BOAT_25));
                            instance->DoCastSpellOnPlayers(SPELL_ACHIEVEMENT_CHECK);
                            StartFlyShip(skybreaker);
                            StopFlyShip(CheckUnfriendlyShip(me, instance, DATA_GB_HIGH_OVERLORD_SAURFANG));
                            break;
                        case EVENT_OUTRO_ALLIANCE_2:
                            StopFlyShip(skybreaker);
                            me->SummonGameObject(RAID_MODE(GO_CAPITAN_CHEST_A_10N, GO_CAPITAN_CHEST_A_25N, GO_CAPITAN_CHEST_A_10H, GO_CAPITAN_CHEST_A_25H), -590.200022f, 2241.193115f, 538.588269f, 0, 0, 0, 0, 0, 100000);
                            me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                            me->GetMotionMaster()->MovePoint(0, -590.700f, 2213.01f, 539.1f);
                            break;
                        case EVENT_OUTRO_ALLIANCE_3:
                            me->GetMotionMaster()->MovePoint(1, -555.59f, 2213.01f, 539.28f);
                            break;
                        case EVENT_FAIL:
                            TeleportPlayers(map, ALLIANCE);
							DeletePlayerShirts();
                            events.ScheduleEvent(EVENT_RESTART_EVENT, 2000);
                            instance->SetBossState(DATA_GUNSHIP_EVENT, NOT_STARTED);
                            break;
                        case EVENT_RESTART_EVENT:
							if (instance->GetBossState(DATA_GUNSHIP_EVENT) != DONE)
							{
								instance->SetBossState(DATA_GUNSHIP_EVENT, FAIL);
								RestartEvent(skybreaker, CheckUnfriendlyShip(me,instance,DATA_GB_HIGH_OVERLORD_SAURFANG), map, ALLIANCE);
							}
                            break;
                        case EVENT_SPAWN_MAGE:
							if (me->GetEntry() != NPC_GB_SKYBREAKER_SORCERERS)
							{
								DoSendQuantumText(SAY_NEW_MAGE_SPAWNED, me);
								skybreaker->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_SORCERERS, -17.8356f, 0.031688f, 20.823f, 4.73231f);
							}
							break;
                        case EVENT_RESPAWN_ROCKETEER:
                            DoSendQuantumText(SAY_NEW_MORTAR_TEAM_SPAWNED, me);
                            if (RocketerCount == 2)
                            {
                                skybreaker->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, -8.61003f, 15.483f, 20.4158f, 4.69854f);
                                skybreaker->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, -27.9583f, 14.8875f, 20.4428f, 4.77865f);
                            }
                            if (RocketerCount == 4)
                            {
                                skybreaker->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, -8.61003f, 15.483f, 20.4158f, 4.69854f);
                                skybreaker->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, -27.9583f, 14.8875f, 20.4428f, 4.77865f);
                                skybreaker->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, -15.61003f, 15.483f, 20.4158f, 4.69854f);
                                skybreaker->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_MORTAR_SOLDIER, -20.9583f, 14.8875f, 20.4428f, 4.77865f);
                            }
                            break;
                        case EVENT_RESPAWN_AXES_RIFLEMAN:
                            DoSendQuantumText(SAY_NEW_RIFLEMAN_SPAWNED, me);
                            if (RiflemanCount == 4)
                            {
                                skybreaker->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -28.0876f, -22.9462f, 21.659f, 4.72416f);
                                skybreaker->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -21.7406f, -22.9462f, 21.659f, 4.72416f);
                                skybreaker->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -14.9806f, -22.9462f, 21.659f, 4.72416f);
                                skybreaker->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -5.15231f, -22.9462f, 21.659f, 4.72416f);
                            }
                            if (RiflemanCount == 6)
                            {
                                skybreaker->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -33.0876f, -22.9462f, 21.659f, 4.72416f);
                                skybreaker->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -28.0876f, -22.9462f, 21.659f, 4.72416f);
                                skybreaker->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -21.7406f, -22.9462f, 21.659f, 4.72416f);
                                skybreaker->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -14.9806f, -22.9462f, 21.659f, 4.72416f);
                                skybreaker->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, -5.15231f, -22.9462f, 21.659f, 4.72416f);
                                skybreaker->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_RIFLEMAN, 0.15231f, -22.9462f, 21.659f, 4.72416f);
                            }
                            break;
                      }
                }

                if (!me->GetCurrentSpell(CURRENT_MELEE_SPELL))
                    DoCastVictim(SPELL_CLEAVE);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_muradin_bronzebeard_gunshipAI(creature);
        }
};

class npc_gunship_skybreaker : public CreatureScript
{
    public:
        npc_gunship_skybreaker() : CreatureScript("npc_gunship_skybreaker") { }

        struct npc_gunship_skybreakerAI : public QuantumBasicAI
        {
            npc_gunship_skybreakerAI(Creature* creature) : QuantumBasicAI(creature), instance(creature->GetInstanceScript())
            {
                Reset();
            }

			EventMap events;
			InstanceScript* instance;

            void Reset()
            {
                QuantumBasicAI::Reset();
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                me->SetDisplayId(MODEL_ID_INVISIBLE);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                SetCombatMovement(false);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void JustDied(Unit* /*killer*/)
            {
                if (Transport* t = me->GetTransport())
                    DoShipExplosion(t);

                if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                {
                    if (Creature* muradin = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_GB_MURADIN_BRONZEBEARD)))
                        muradin->AI()->DoAction(ACTION_FAIL);
                }
                
                else if (instance->GetData(DATA_TEAM_IN_INSTANCE) == HORDE)
                {
                    if (Creature* saurfang = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_GB_HIGH_OVERLORD_SAURFANG)))
                        saurfang->AI()->DoAction(ACTION_DONE);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_gunship_skybreakerAI(creature);
        }
};

class npc_gunship_orgrimmar : public CreatureScript
{
    public:
        npc_gunship_orgrimmar() : CreatureScript("npc_gunship_orgrimmar") { }

        struct npc_gunship_orgrimmarAI : public QuantumBasicAI
        {
            npc_gunship_orgrimmarAI(Creature* creature) : QuantumBasicAI(creature), instance(creature->GetInstanceScript())
            {
                Reset();
            }

			EventMap events;
			InstanceScript* instance;

            void Reset()
            {
                QuantumBasicAI::Reset();
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                me->SetDisplayId(MODEL_ID_INVISIBLE);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                SetCombatMovement(false);
                instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void JustDied(Unit* /*killer*/)
            {
                if (Transport* t = me->GetTransport())
                    DoShipExplosion(t);

                if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                {
                    if (Creature* muradin = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_GB_MURADIN_BRONZEBEARD)))
                        muradin->AI()->DoAction(ACTION_DONE);
                }
                
                else if (instance->GetData(DATA_TEAM_IN_INSTANCE) == HORDE)
                {
                    if (Creature* saurfang = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_GB_HIGH_OVERLORD_SAURFANG)))
                        saurfang->AI()->DoAction(ACTION_FAIL);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_gunship_orgrimmarAI(creature);
        }
};

class npc_korkron_axethrower_rifleman : public CreatureScript
{
    public:
        npc_korkron_axethrower_rifleman() : CreatureScript("npc_korkron_axethrower_rifleman") { }

        struct npc_korkron_axethrower_riflemanAI : public QuantumBasicAI
        {
            npc_korkron_axethrower_riflemanAI(Creature* creature) : QuantumBasicAI(creature), instance(creature->GetInstanceScript())
            {
                Reset();
				SetCombatMovement(false);
            }

			EventMap events;
			InstanceScript* instance;

			bool Desperated;

			uint32 DesperateResolve;
			uint32 TargetTimer;
            uint32 AttackTimer;

            void Reset()
            {
                QuantumBasicAI::Reset();
                DesperateResolve = RAID_MODE(SPELL_DESPERATE_RESOLVE_10_NM, SPELL_DESPERATE_RESOLVE_25_NM, SPELL_DESPERATE_RESOLVE_10_HM, SPELL_DESPERATE_RESOLVE_25_HM);
                Desperated = false;
                me->RemoveAurasDueToSpell(SPELL_EXPERIENCED);
                me->RemoveAurasDueToSpell(SPELL_ELITE);
                me->RemoveAurasDueToSpell(SPELL_VETERAN);
                events.ScheduleEvent(EVENT_EXPERIENCED, urand(19000, 21000)); // ~20 sec
                events.ScheduleEvent(EVENT_VETERAN, urand(39000, 41000));     // ~40 sec
                events.ScheduleEvent(EVENT_ELITE, urand(59000, 61000));       // ~60 sec

				TargetTimer = 500;
                AttackTimer = urand(1000, 3000);

                if (me->GetEntry() == NPC_GB_SKYBREAKER_RIFLEMAN)
                    SetEquipmentSlots(false, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE, 47267);

				if (me->GetEntry() == NPC_GB_KORKRON_AXETHROWER)
                    SetEquipmentSlots(false, 49691, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
            {
                if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE && me->GetHealthPct() < 20.0f && !Desperated)
                {
                    Desperated = true;
                    DoCast(me, DesperateResolve);
                }
            }

            void JustDied(Unit* /*killer*/)
            {
                if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                {
                    if (Creature* saurfang = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_GB_HIGH_OVERLORD_SAURFANG)))
                        saurfang->AI()->DoAction(ACTION_AXES_RIFL_DIE);
                }
                 
                if (instance->GetData(DATA_TEAM_IN_INSTANCE) == HORDE)
                {
                    if (Creature* muradin = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_GB_MURADIN_BRONZEBEARD)))
                        muradin->AI()->DoAction(ACTION_AXES_RIFL_DIE);
                }
            }

            void UpdateAI(uint32 const diff)
            {
				// Out Of Combat Timer
				if (TargetTimer <= diff && !me->IsInCombatActive())
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 150.0f, true))
					{
						me->AI()->EnterToBattle(target);
						me->AI()->AttackStart(target);
						TargetTimer = urand(5000, 10000);
					}
				}
				else TargetTimer -= diff;

                if (instance->GetBossState(DATA_GUNSHIP_EVENT) != IN_PROGRESS)
                    return;
                
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (!UpdateVictim())
                    return;

				if (AttackTimer <= diff)
                {
                    if (me->GetEntry() == NPC_GB_KORKRON_AXETHROWER)
					{
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 150.0f, true))
							DoCast(target, RAID_MODE(SPELL_HURL_AXE_10, SPELL_HURL_AXE_25, SPELL_HURL_AXE_10, SPELL_HURL_AXE_25));
					}

					if (me->GetEntry() == NPC_GB_SKYBREAKER_RIFLEMAN)
					{
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 150.0f, true))
							DoCast(target, RAID_MODE(SPELL_SHOOT_10, SPELL_SHOOT_25, SPELL_SHOOT_10, SPELL_SHOOT_25));
					}

                    AttackTimer = urand(6000, 15000);
                }
				else AttackTimer -= diff;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_EXPERIENCED:
                            DoCast(me, SPELL_EXPERIENCED);
                            break;
                        case EVENT_VETERAN:
                            me->RemoveAurasDueToSpell(SPELL_EXPERIENCED);
                            DoCast(me, SPELL_VETERAN);
                            break;
                        case EVENT_ELITE:
                            me->RemoveAurasDueToSpell(SPELL_VETERAN);
                            DoCast(me, SPELL_ELITE);
                            break;
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_korkron_axethrower_riflemanAI(creature);
        }
};

class npc_sergeant : public CreatureScript
{
    public:
        npc_sergeant() : CreatureScript("npc_sergeant") { }

        struct npc_sergeantAI : public QuantumBasicAI
        {
            npc_sergeantAI(Creature* creature) : QuantumBasicAI(creature), instance(creature->GetInstanceScript())
            {
                Reset();
            }

			EventMap events;
			InstanceScript* instance;

			uint32 DesperateResolve;

			bool Desperated;

            void Reset()
            {
                QuantumBasicAI::Reset();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                me->SetReactState(REACT_PASSIVE);
                events.ScheduleEvent(EVENT_WALK_MOBS, 1500);
                Desperated = false;
                me->RemoveAurasDueToSpell(SPELL_EXPERIENCED);
                me->RemoveAurasDueToSpell(SPELL_ELITE);
                DesperateResolve = RAID_MODE(SPELL_DESPERATE_RESOLVE_10_NM, SPELL_DESPERATE_RESOLVE_25_NM, SPELL_DESPERATE_RESOLVE_10_HM, SPELL_DESPERATE_RESOLVE_25_HM);
                events.ScheduleEvent(EVENT_EXPERIENCED, urand(19000, 21000));  // ~20 sec
                events.ScheduleEvent(EVENT_VETERAN, urand(39000, 41000));      // ~40 sec
                events.ScheduleEvent(EVENT_BURNING_PITCH, urand(60000, 62000));// ~61 sec

                if (me->GetMap()->ToInstanceMap()->GetMaxPlayers() == 25 && me->GetMap()->IsHeroic())
                    events.ScheduleEvent(EVENT_ELITE, urand(59000, 61000));       // ~60 sec
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
            {
                if (me->GetHealthPct() < 20.0f && !Desperated)
                {
                    Desperated = true;
                    DoCast(me, DesperateResolve);
                }
            }

            bool CanAIAttack(Unit const* target) const
            {
                if (target->GetEntry() == NPC_GB_GUNSHIP_HULL)
                    return false;

                return true;
            }

            void UpdateAI(uint32 const diff)
            {
                if (instance->GetBossState(DATA_GUNSHIP_EVENT) != IN_PROGRESS)
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                    
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_WALK_MOBS:
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                            me->SetReactState(REACT_AGGRESSIVE);

                            switch (me->GetEntry())
                            {
                                case NPC_GB_KORKRON_SERGANTE:
                                    if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 0.0f, true, SPELL_ON_SKYBREAKERS_DECK))
                                    {
                                        me->GetMotionMaster()->MoveChase(target);
                                        me->AI()->AttackStart(target);
                                    }
									break;
                                case NPC_GB_SKYBREAKER_SERGANTE:
                                    if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 0.0f, true, SPELL_ON_ORGRIMS_HAMMERS_DECK))
                                    {
                                        me->GetMotionMaster()->MoveChase(target);
                                        me->AI()->AttackStart(target);
                                    }
									break;
                            }
                            events.ScheduleEvent(EVENT_WOUNDING_STRIKE, 5000);
                            events.ScheduleEvent(EVENT_BLADE_STORM, 3000);
                            break;
                        case EVENT_BURNING_PITCH:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100.0f, true))
                            {
                                if (me->GetEntry() == NPC_GB_KORKRON_SERGANTE)
                                    DoCast(target, SPELL_BURNING_PITCH_H);
                                else
                                    DoCast(target, SPELL_BURNING_PITCH_A);
                            }
                            events.ScheduleEvent(EVENT_BURNING_PITCH, urand(60000, 62000));// ~61 sec
                            break;
                        case EVENT_EXPERIENCED:
                            DoCast(me, SPELL_EXPERIENCED);
                            break;
                        case EVENT_VETERAN:
                            me->RemoveAurasDueToSpell(SPELL_EXPERIENCED);
                            DoCast(me, SPELL_VETERAN);
                            break;
                        case EVENT_ELITE:
                            me->RemoveAurasDueToSpell(SPELL_VETERAN);
                            DoCast(me, SPELL_ELITE);
                            break;
                        case EVENT_WOUNDING_STRIKE:
                            if (UpdateVictim())
                                DoCastVictim(RAID_MODE(SPELL_WOUNDING_STRIKE_10_NM, SPELL_WOUNDING_STRIKE_25_NM, SPELL_WOUNDING_STRIKE_10_HM, SPELL_WOUNDING_STRIKE_25_HM));
                            events.ScheduleEvent(EVENT_WOUNDING_STRIKE, urand(5000, 8000));
                            break;
                        case EVENT_BLADE_STORM:
                            DoCast(me, SPELL_BLADESTORM);
                            events.ScheduleEvent(EVENT_BLADE_STORM, urand(9000, 16000));
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_sergeantAI(creature);
        }
};

class npc_marine_or_reaver : public CreatureScript
{
    public:
        npc_marine_or_reaver() : CreatureScript("npc_marine_or_reaver") { }

        struct npc_marine_or_reaverAI : public QuantumBasicAI
        {
            npc_marine_or_reaverAI(Creature* creature) : QuantumBasicAI(creature), instance(creature->GetInstanceScript())
            {
                Reset();
            }

			EventMap events;
			InstanceScript* instance;

			uint32 DesperateResolve;

			bool Desperated;

            void Reset()
            {
                QuantumBasicAI::Reset();
                Desperated = false;
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                me->SetReactState(REACT_PASSIVE);
                events.ScheduleEvent(EVENT_WALK_MOBS, 1500);
                me->RemoveAurasDueToSpell(SPELL_EXPERIENCED);
                me->RemoveAurasDueToSpell(SPELL_ELITE);
                DesperateResolve = RAID_MODE(SPELL_DESPERATE_RESOLVE_10_NM, SPELL_DESPERATE_RESOLVE_25_NM, SPELL_DESPERATE_RESOLVE_10_HM, SPELL_DESPERATE_RESOLVE_25_HM);
                events.ScheduleEvent(EVENT_EXPERIENCED, urand(19000, 21000));  // ~20 sec
                events.ScheduleEvent(EVENT_VETERAN, urand(39000, 41000));      // ~40 sec
                events.ScheduleEvent(EVENT_BURNING_PITCH, urand(60000, 62000));// ~61 sec
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
            {
                if (me->GetHealthPct() < 20.0f && !Desperated)
                {
                    Desperated = true;
                    DoCast(me, DesperateResolve);
                }
            }

            bool CanAIAttack(Unit const* target) const
            {
                if (target->GetEntry() == NPC_GB_GUNSHIP_HULL)
                    return false;

                return true;
            }

            void UpdateAI(uint32 const diff)
            {
                if (instance->GetBossState(DATA_GUNSHIP_EVENT) != IN_PROGRESS)
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_WALK_MOBS:
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                            me->SetReactState(REACT_AGGRESSIVE);
                            switch (me->GetEntry())
                            {
                                case NPC_GB_KORKRON_REAVERS:
                                    if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 0.0f, true, SPELL_ON_SKYBREAKERS_DECK))
                                    {
                                        me->GetMotionMaster()->MoveChase(target);
                                        me->AI()->AttackStart(target);
                                    }
									break;
                                case NPC_GB_SKYBREAKER_MARINE:
                                    if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 0.0f, true, SPELL_ON_ORGRIMS_HAMMERS_DECK))
                                    {
                                        me->GetMotionMaster()->MoveChase(target);
                                        me->AI()->AttackStart(target);
                                    }
									break;
                            }
                            break;
                        case EVENT_BURNING_PITCH:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
                            {
                                if (me->GetEntry() == NPC_GB_KORKRON_REAVERS)
                                    DoCast(target, SPELL_BURNING_PITCH_H);
                                else
                                    DoCast(target, SPELL_BURNING_PITCH_A);
                            }
                            events.ScheduleEvent(EVENT_BURNING_PITCH, urand(60000, 62000));// ~61 sec
                            break;
                        case EVENT_EXPERIENCED:
                            DoCast(me, SPELL_EXPERIENCED);
                            break;
                        case EVENT_VETERAN:
                            me->RemoveAurasDueToSpell(SPELL_EXPERIENCED);
                            DoCast(me, SPELL_VETERAN);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_marine_or_reaverAI(creature);
        }
};

class npc_gunship_mage : public CreatureScript
{
    public:
        npc_gunship_mage() : CreatureScript("npc_gunship_mage") { }

        struct npc_gunship_mageAI : public QuantumBasicAI
        {
            npc_gunship_mageAI(Creature* creature) : QuantumBasicAI(creature), instance(creature->GetInstanceScript())
            {
				me->SetReactState(REACT_PASSIVE);
				me->SetInCombatWithZone();
				me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
				me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                Reset();
            }

			EventMap events;
			InstanceScript* instance;

			uint32 BelowZeroTimer;

            void Reset()
            {
                QuantumBasicAI::Reset();
                BelowZeroTimer = 500;
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case EVENT_FREEZE_CANNON:
                        if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                        {
                            std::list<Creature*> cannonsA;
                            GetCreatureListWithEntryInGrid(cannonsA, me, NPC_GB_ALLIANCE_CANON, 500.0f);
                            for (std::list<Creature*>::iterator itr = cannonsA.begin(); itr != cannonsA.end(); ++itr)
                            {
                                if (Vehicle* veh = (*itr)->GetVehicleKit())
                                    veh->RemoveAllPassengers();
									
                                DoCast((*itr), SPELL_BELOW_ZERO, true);
                            }
                        }
                        else if (instance->GetData(DATA_TEAM_IN_INSTANCE) == HORDE)
                        {
                            std::list<Creature*> cannonsH;
                            GetCreatureListWithEntryInGrid(cannonsH, me, NPC_GB_HORDE_CANON, 500.0f);
                            for (std::list<Creature*>::iterator itr = cannonsH.begin(); itr != cannonsH.end(); ++itr)
                            {
                                if (Vehicle* veh = (*itr)->GetVehicleKit())
                                    veh->RemoveAllPassengers();
									
                                 DoCast((*itr), SPELL_BELOW_ZERO, true);
                            }
                        }
						break;
				}
			}

            void JustDied(Unit* /*killer*/)
            {
                if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                {
                    if (me->GetGUID() == instance->GetData64(DATA_GB_BATTLE_MAGE))
                    {
                        if (Creature* saurfang = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_GB_HIGH_OVERLORD_SAURFANG)))
                            saurfang->AI()->DoAction(ACTION_MAGE_DIE);
                    }
                 }
                 
                 if (instance->GetData(DATA_TEAM_IN_INSTANCE) == HORDE)
                 {
                     if (me->GetGUID() == instance->GetData64(DATA_GB_BATTLE_MAGE))
                     {
                         if (Creature* muradin = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_GB_MURADIN_BRONZEBEARD)))
                             muradin->AI()->DoAction(ACTION_MAGE_DIE);
                     }
                 }
            }

            void UpdateAI(uint32 const diff)
            {
                if (instance->GetBossState(DATA_GUNSHIP_EVENT) != IN_PROGRESS)
                    return;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                    
                if (me->GetGUID() == instance->GetData64(DATA_GB_BATTLE_MAGE))
                {
                    if (BelowZeroTimer <= diff)
                    {
                        me->AI()->DoAction(EVENT_FREEZE_CANNON);
                        BelowZeroTimer = urand(10000, 15000);
                    }
					else BelowZeroTimer -= diff;
                }
                else
					DoCast(me, SPELL_SHADOW_CHANNELING);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_gunship_mageAI(creature);
        }
};

class npc_gunship_cannon : public CreatureScript
{
    public:
        npc_gunship_cannon() : CreatureScript("npc_gunship_cannon") { }

        struct npc_gunship_cannonAI : public QuantumBasicAI
        {
            npc_gunship_cannonAI(Creature* creature) : QuantumBasicAI(creature), instance(creature->GetInstanceScript())
			{
				SetCombatMovement(false);
			}

			InstanceScript* instance;

            void Reset()
            {
                DoCast(me, SPELL_HEAT_DRAIN, true);
				me->SetReactState(REACT_PASSIVE);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_BELOW_ZERO)
                    me->GetVehicleKit()->RemoveAllPassengers();
            }

            void DamageTaken(Unit* attacker, uint32& damage)
            {
                if (me->GetEntry() == NPC_GB_ALLIANCE_CANON)
                {
                    if (Creature* pAllianceBoss = ObjectAccessor::GetCreature(*me, me->GetInstanceScript()->GetData64(DATA_SKYBREAKER_BOSS)))
                        attacker->DealDamage(pAllianceBoss, damage);
                }

                if (me->GetEntry() == NPC_GB_HORDE_CANON)
                {
                    if (Creature* pHordeBoss = ObjectAccessor::GetCreature(*me, me->GetInstanceScript()->GetData64(DATA_ORGRIMMAR_HAMMER_BOSS)))
                        attacker->DealDamage(pHordeBoss, damage);
                }
            }

            void UpdateAI(uint32 const diff)
            {
            	me->RemoveExtraUnitMovementFlag(MOVEMENTFLAG_STRAFE_LEFT);
				me->RemoveExtraUnitMovementFlag(MOVEMENTFLAG_STRAFE_RIGHT);
				me->RemoveExtraUnitMovementFlag(MOVEMENTFLAG2_INTERPOLATED_TURNING);

                if (me->HasAura(SPELL_BELOW_ZERO) && instance->GetBossState(DATA_GUNSHIP_EVENT) == IN_PROGRESS)
                {
					me->RemoveAurasByType(SPELL_AURA_CONTROL_VEHICLE);
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
					
                    if (Vehicle* veh = me->GetVehicleKit())
                        veh->RemoveAllPassengers();
                }

                else if (!me->HasAura(SPELL_BELOW_ZERO) && instance->GetBossState(DATA_GUNSHIP_EVENT) == IN_PROGRESS)
                {
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
					me->RemoveExtraUnitMovementFlag(MOVEMENTFLAG_STRAFE_LEFT);
					me->RemoveExtraUnitMovementFlag(MOVEMENTFLAG_STRAFE_RIGHT);
					me->RemoveExtraUnitMovementFlag(MOVEMENTFLAG2_INTERPOLATED_TURNING);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_gunship_cannonAI(creature);
        }
};

class npc_mortar_soldier_or_rocketeer : public CreatureScript
{
    public:
        npc_mortar_soldier_or_rocketeer() : CreatureScript("npc_mortar_soldier_or_rocketeer") { }

        struct npc_mortar_soldier_or_rocketeerAI : public QuantumBasicAI
        {
            npc_mortar_soldier_or_rocketeerAI(Creature* creature) : QuantumBasicAI(creature),instance(creature->GetInstanceScript())
            {
				SetCombatMovement(false);
                Reset();
            }

			EventMap events;
			InstanceScript* instance;

            void Reset()
            {
                QuantumBasicAI::Reset();
                me->RemoveAurasDueToSpell(SPELL_EXPERIENCED);
                me->RemoveAurasDueToSpell(SPELL_ELITE);
                me->RemoveAurasDueToSpell(SPELL_VETERAN);
                events.ScheduleEvent(EVENT_EXPERIENCED, urand(19000, 21000)); // ~20 sec
                events.ScheduleEvent(EVENT_VETERAN, urand(39000, 41000));     // ~40 sec
                events.ScheduleEvent(EVENT_ELITE, urand(59000, 61000));       // ~60 sec
                events.ScheduleEvent(EVENT_ROCKET_ART, urand(10000, 15000));   // ~12 sec
            }

            void JustDied(Unit* /*killer*/)
            {
                if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                {
                    if (Creature* saurfang = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_GB_HIGH_OVERLORD_SAURFANG)))
                        saurfang->AI()->DoAction(ACTION_ROCK_DIE);
                }

                if (instance->GetData(DATA_TEAM_IN_INSTANCE) == HORDE)
                {
                    if (Creature* muradin = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_GB_MURADIN_BRONZEBEARD)))
                        muradin->AI()->DoAction(ACTION_ROCK_DIE);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (instance->GetBossState(DATA_GUNSHIP_EVENT) != IN_PROGRESS)
                    return;

				me->SetReactState(REACT_DEFENSIVE);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                    
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_EXPERIENCED:
                            DoCast(me, SPELL_EXPERIENCED);
                            break;
                        case EVENT_VETERAN:
                            me->RemoveAurasDueToSpell(SPELL_EXPERIENCED);
                            DoCast(me, SPELL_VETERAN);
                            break;
                        case EVENT_ELITE:
                            me->RemoveAurasDueToSpell(SPELL_VETERAN);
                            DoCast(me, SPELL_ELITE);
                            break;
                        case EVENT_ROCKET_ART:
                            if (Player* target = SelectRandomPlayerInTheMaps(me->GetMap()))
                            {
                                if (me->GetEntry() == NPC_GB_KORKRON_ROCKETEER && target->HasAura(SPELL_ON_SKYBREAKERS_DECK))
                                {
                                    me->CastSpell(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), SPELL_ROCKET_ARTILLERY_TRIGGERED, true);
                                    me->CastSpell(target, SPELL_CANNON_BLAST_PROC_10, true);
                                }
                                else if (me->GetEntry() == NPC_GB_SKYBREAKER_MORTAR_SOLDIER && target->HasAura(SPELL_ON_ORGRIMS_HAMMERS_DECK))
                                {
                                    me->CastSpell(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), SPELL_ROCKET_ARTILLERY_TRIGGERED, true);
                                    me->CastSpell(target, SPELL_CANNON_BLAST_PROC_25, true);
                                }
                            }
                            events.ScheduleEvent(EVENT_ROCKET_ART, urand(10000, 15000));   // ~12 sec
                            break;
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_mortar_soldier_or_rocketeerAI(creature);
        }
};

class npc_zafod_boombox : public CreatureScript
{
    public:
        npc_zafod_boombox() : CreatureScript("npc_zafod_boombox") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            // Maybe this isn't blizzlike but I can't find any spell in the DBCs
            if (player->GetItemCount(ITEM_THE_ROCKET_PACK, false) == 0)
                player->ADD_GOSSIP_ITEM(0, "Yeah, I'm sure that safety is your top priority. Give me a jetpack.", 631, 1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            player->CLOSE_GOSSIP_MENU();

            if (action == 1)
            {
                // Seurity, this shouldn't happen. Maybe useless.
                uint32 curItemCount = player->GetItemCount(ITEM_THE_ROCKET_PACK, false);
                if (curItemCount >= 1)
                {
                    creature->MonsterWhisper("You already have my jetpack!", player->GetGUIDLow());
                    return false;
                }

                ItemPosCountVec dest;
                uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ITEM_THE_ROCKET_PACK, 1);
                if (msg == EQUIP_ERR_OK)
                {
                    Item* item = player->StoreNewItem(dest, ITEM_THE_ROCKET_PACK, true);
                    player->SendNewItem(item, 1, true, false);
                }
                else
                {
                    creature->MonsterWhisper("You do not have empty space for my jet-pack!", player->GetGUIDLow());
                    return false;
                }
            }

            return true;
        }
};

class npc_high_overlord_saurfang_gunship : public CreatureScript
{
    public:
        npc_high_overlord_saurfang_gunship() : CreatureScript("npc_high_overlord_saurfang_gunship") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            InstanceScript* instance = creature->GetInstanceScript();

            if (instance->GetData(DATA_TEAM_IN_INSTANCE) == HORDE)
            {
                if ((!player->GetGroup() || !player->GetGroup()->IsLeader(player->GetGUID())) && !player->IsGameMaster())
                {
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I'm not the raid leader...", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
                    return true;
                }

                player->ADD_GOSSIP_ITEM(0, "My companions are all accounted for, Saurfang. Let's go!", 631, 1001);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                return true;
            }
            return false;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            InstanceScript* instance = creature->GetInstanceScript();

            player->PlayerTalkClass->ClearMenus();
            player->CLOSE_GOSSIP_MENU();

            if (action == GOSSIP_ACTION_INFO_DEF+2)
                creature->MonsterSay("I'll wait for the raid leader.", LANG_UNIVERSAL, player->GetGUID());

            if (action == 1001)
            {
                creature->AI()->DoAction(ACTION_INTRO_START);
                creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
            return true;
        }

        struct npc_high_overlord_saurfang_gunshipAI : public QuantumBasicAI
        {
            npc_high_overlord_saurfang_gunshipAI(Creature* creature) : QuantumBasicAI(creature), instance(creature->GetInstanceScript()){}

			Transport* orgrimmar;
			Map* map;
			EventMap events;
			InstanceScript* instance;

			uint32 RocketerDieCount;
			uint32 AxesDieCount;
			uint32 RocketerCount;
			uint32 AxesCount;
			uint32 SummonCount;
			uint32 count;

			bool EventScheduled;

            void Reset()
            {
                if (instance->GetBossState(DATA_GUNSHIP_EVENT) == IN_PROGRESS)
                    return;

                me->SetReactState(REACT_PASSIVE);
                me->SetCurrentFaction(1801);
                events.Reset();
                map = me->GetMap();
                orgrimmar = me->GetTransport();
				 UpdateTransportMotionInMap(orgrimmar);
                RocketerDieCount = 0;
                AxesDieCount = 0;
                SummonCount = RAID_MODE(3, 5, 4, 6);
                count = 0;
                RocketerCount = RAID_MODE(2, 4, 2, 4);
                AxesCount = RAID_MODE(4, 6, 4, 6);
                EventScheduled = false;
            }

			void DeletePlayerShirts()
			{
				Map::PlayerList const& Players = me->GetMap()->GetPlayers();
				for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
				{
					if (Player* player = itr->getSource())
						player->DestroyItemCount(ITEM_THE_ROCKET_PACK, 1, true);
				}
			}

            // Send packet to all players
            void SendPacketToPlayers(WorldPacket const* data) const
            {
                Map::PlayerList const& players = me->GetMap()->GetPlayers();
                if (!players.isEmpty())
				{
                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
					{
                        if (Player* player = itr->getSource())
                            player->GetSession()->SendPacket(data);
					}
				}
            }

            bool CanAIAttack(Unit const* target) const
            {
                if (target->GetEntry() == NPC_GB_SKYBREAKER_SERGANTE || target->GetEntry() == NPC_GB_SKYBREAKER_MARINE)
                    return true;

                return false;
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                     case ACTION_INTRO_START:
                         events.ScheduleEvent(EVENT_INTRO_HORDE_1, 1000);
                         events.ScheduleEvent(EVENT_INTRO_HORDE_1_1, 11000);
                         events.ScheduleEvent(EVENT_START_FLY, 5000);
                         events.ScheduleEvent(EVENT_INTRO_HORDE_2, 32000);
                         events.ScheduleEvent(EVENT_INTRO_HORDE_3, 51000);
                         events.ScheduleEvent(EVENT_INTRO_HORDE_4, 57000);
                         events.ScheduleEvent(EVENT_INTRO_HORDE_5, 62000);
                         break;
                     case ACTION_BATTLE_EVENT:
                     {
                         if (instance->GetData(DATA_TEAM_IN_INSTANCE) == HORDE)
                             events.ScheduleEvent(EVENT_WIPE_CHECK, 5000);

                         Creature* pHordeBoss = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_ORGRIMMAR_HAMMER_BOSS));
                         Creature* pAllianceBoss = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_SKYBREAKER_BOSS));

                         if (pHordeBoss && pAllianceBoss)
                         {
                             me->SetReactState(REACT_AGGRESSIVE);
                             pHordeBoss->SetInCombatWith(pAllianceBoss);
                             pAllianceBoss->SetInCombatWith(pHordeBoss);
                             pAllianceBoss->AddThreat(pHordeBoss, 0.0f);
                             pHordeBoss->AddThreat(pAllianceBoss, 0.0f);
                             instance->SetBossState(DATA_GUNSHIP_EVENT, IN_PROGRESS);
                             events.ScheduleEvent(EVENT_SUMMON_PORTAL, 30000);
                             RelocateTransport(orgrimmar);
                             RelocateTransport(CheckUnfriendlyShip(me, instance, DATA_GB_MURADIN_BRONZEBEARD));
                         }
                         else
                             me->AI()->DoAction(ACTION_FAIL);
                         break;
                     }
                     case ACTION_DONE:
                         if (Creature* pAllianceBoss = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_SKYBREAKER_BOSS)))
                             instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, pAllianceBoss);

                         if (Creature* pHordeBoss = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_ORGRIMMAR_HAMMER_BOSS)))
                             instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, pHordeBoss);

						 DeletePlayerShirts();

                         me->SetReactState(REACT_PASSIVE);
                         instance->SetBossState(DATA_GUNSHIP_EVENT, DONE);
                         instance->DoCastSpellOnPlayers(SPELL_REMOVE_ROCKET_PACK);
                         DoSendQuantumText(SAY_HORDE_VICTORY, me);
                         StartFlyShip(CheckUnfriendlyShip(me, instance, DATA_GB_MURADIN_BRONZEBEARD));
                         StopFight(orgrimmar, CheckUnfriendlyShip(me, instance, DATA_GB_MURADIN_BRONZEBEARD));
                         events.CancelEvent(EVENT_WIPE_CHECK);
                         events.ScheduleEvent(EVENT_OUTRO_HORDE_1, 3500);
                         events.ScheduleEvent(EVENT_OUTRO_HORDE_2, 21000);
                         events.ScheduleEvent(EVENT_OUTRO_HORDE_3, 32000);
                         break;
                     case ACTION_FAIL:
                         events.CancelEvent(EVENT_WIPE_CHECK);
                         DoSendQuantumText(SAY_HORDE_DEFEAT, me);
                         if (Creature* pHordeBoss = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_ORGRIMMAR_HAMMER_BOSS)))
                             instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, pHordeBoss);

                         if (Creature* pAllianceBoss = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_SKYBREAKER_BOSS)))
                             instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, pAllianceBoss);

                         StopFight(orgrimmar, CheckUnfriendlyShip(me, instance, DATA_GB_MURADIN_BRONZEBEARD));
                         events.ScheduleEvent(EVENT_FAIL, 10000);
                         break;
                     case ACTION_MAGE_DIE:
						 //Check, if there really is no mage
						 if (me->GetEntry() != NPC_GB_KORKRON_BATTLE_MAGE)
							 events.ScheduleEvent(EVENT_SPAWN_MAGE, 60000);
						 break;
                     case ACTION_ROCK_DIE:
                         ++RocketerDieCount;
                         if (RocketerDieCount == RocketerCount)
                             events.ScheduleEvent(EVENT_RESPAWN_ROCKETEER, 60000);
                         break;
                     case ACTION_AXES_RIFL_DIE:
                         ++AxesDieCount;
                         if (AxesDieCount == AxesCount)
                             events.ScheduleEvent(EVENT_RESPAWN_AXES_RIFLEMAN, 60000);
                         break;
                 }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage)
            {
                if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE && me->GetHealthPct() < 2.0f )
                    damage = 0;
                
                if (instance->GetData(DATA_TEAM_IN_INSTANCE) == HORDE && me->GetHealthPct() < 2.0f )
                {
                    damage = 0;
                    me->AI()->DoAction(ACTION_FAIL);
                }
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (id = 1)
                    me->DespawnAfterAction(1*IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const diff)
            {
                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;
                    
                if (instance->GetBossState(DATA_GUNSHIP_EVENT) == IN_PROGRESS)
                {
                    if (!HealthAbovePct(HEALTH_PERCENT_75))
                    {
                        me->SetHealth(me->GetMaxHealth() / 100 * 76); // find a better way to avoid the hardcore spell spam ....
                        DoCast(me, SPELL_TASTE_OF_BLOOD, true);
						DoCast(me, SPELL_BATTLE_FURY, true);
                    } 

                    if (UpdateVictim())
                    {
                        if (!EventScheduled)
                        {
                            events.ScheduleEvent(EVENT_RENDING_THROW, 1500);
                            EventScheduled = true;
                        }
                    }
                }

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_WIPE_CHECK:
                            DoCheckFallingPlayer(me);
                            if (DoWipeCheck(orgrimmar))
                                events.ScheduleEvent(EVENT_WIPE_CHECK, 3000);
                            else
                                me->AI()->DoAction(ACTION_FAIL);
                            break;
                        case EVENT_INTRO_HORDE_1:
                            DoSendQuantumText(SAY_INTRO_HORDE_0, me);
                            StartFlyShip(orgrimmar);
                            break;
                        case EVENT_INTRO_HORDE_1_1:
                            DoSendQuantumText(SAY_INTRO_HORDE_0_1, me);
                            break;
                        case EVENT_START_FLY:
							StartFlyShip(orgrimmar); // Try
                            break;
                        case EVENT_INTRO_HORDE_2:
                            StopFlyShip(orgrimmar);
                            StartFlyShip(CheckUnfriendlyShip(me, instance, DATA_GB_MURADIN_BRONZEBEARD));
                            DoSendQuantumText(SAY_INTRO_HORDE_1, me);
                            break;
                        case EVENT_INTRO_HORDE_3:
                            StopFlyShip(CheckUnfriendlyShip(me, instance, DATA_GB_MURADIN_BRONZEBEARD));
                            DoSendQuantumText(SAY_INTRO_HORDE_2, me);
                            break;
                        case EVENT_INTRO_HORDE_4:
							me->GetMap()->SetZoneMusic(AREA_ICECROWN_CITADEL, SOUND_ID_GUNSHIP_BATTLE);
                            me->AI()->DoAction(ACTION_BATTLE_EVENT);
                            if (Creature* muradin = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_GB_MURADIN_BRONZEBEARD))) // DATA_MURADIN_BRONZEBEARD_NOT_VISUAL
                            {
								DoSendQuantumText(SAY_BOARDING_ORGRIMS_HAMMER_0, muradin);
                                muradin->SetReactState(REACT_PASSIVE);
                                muradin->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                            }
                            break;
                        case EVENT_INTRO_HORDE_5:
                            DoSendQuantumText(SAY_INTRO_HORDE_4, me);
                            break;
                        case EVENT_SUMMON_PORTAL:
							if (instance->GetBossState(DATA_GUNSHIP_EVENT) != DONE)
							{
								orgrimmar->AddNPCPassengerInInstance(NPC_GB_PORTAL, 15.03016f, -7.00016f, 37.70952f, 1.55138f);
								CheckUnfriendlyShip(me, instance, DATA_GB_MURADIN_BRONZEBEARD)->AddNPCPassengerInInstance(NPC_GB_PORTAL, 3.51547f, -0.160213f, 20.87252f, 3.05033f); // DATA_MURADIN_BRONZEBEARD_NOT_VISUAL
								events.ScheduleEvent(EVENT_BOARDING_TALK, 10000);
								events.ScheduleEvent(EVENT_BOARDING_GUNSHIP, 2500); // TODO: Fix the timers
							}
                            break;
                        case EVENT_BOARDING_TALK:
                            DoSendQuantumText(SAY_BOARDING_ORGRIMS_HAMMER_1, me);
                            break;
                        case EVENT_BOARDING_GUNSHIP:
							if (instance->GetBossState(DATA_GUNSHIP_EVENT) != DONE)
							{
								count = 0;
								if (Creature* muradin = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_GB_MURADIN_BRONZEBEARD))) // DATA_MURADIN_BRONZEBEARD_NOT_VISUAL
									DoSendQuantumText(SAY_INTRO_HORDE_3, muradin);

								if (Creature* Sergante = orgrimmar->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_SERGANTE, 15.03016f, -7.00016f, 37.70952f, 1.55138f))
									Sergante->CastSpell(Sergante, SPELL_TELEPORT_VISUAL, true);

								events.ScheduleEvent(EVENT_BOARDING_REAVERS_MARINE, 3000);
								events.ScheduleEvent(EVENT_SUMMON_PORTAL, 90000);
							}
                            break;
                        case EVENT_BOARDING_REAVERS_MARINE:
							if (instance->GetBossState(DATA_GUNSHIP_EVENT) != DONE)
							{
								if (count <= SummonCount)
								{
									if (Creature* Marine = orgrimmar->AddNPCPassengerInInstance(NPC_GB_SKYBREAKER_MARINE, 15.03016f, -7.00016f, 37.70952f, 1.55138f))
									{
										Marine->CastSpell(Marine, SPELL_TELEPORT_VISUAL, true);
										count++;
										events.ScheduleEvent(EVENT_BOARDING_REAVERS_MARINE, 2500);
									}
								}
							}
                            break;
                        case EVENT_OUTRO_HORDE_1:
                            instance->DoCompleteAchievement(RAID_MODE(ACHIEVEMENT_IM_ON_A_BOAT_10, ACHIEVEMENT_IM_ON_A_BOAT_25, ACHIEVEMENT_IM_ON_A_BOAT_10, ACHIEVEMENT_IM_ON_A_BOAT_25));
                            instance->DoCastSpellOnPlayers(SPELL_ACHIEVEMENT_CHECK);
                            StartFlyShip(orgrimmar);
                            StopFlyShip(CheckUnfriendlyShip(me,instance,DATA_GB_MURADIN_BRONZEBEARD));
                            break;
                        case EVENT_OUTRO_HORDE_2:
                            StopFlyShip(orgrimmar);
                            me->SummonGameObject(RAID_MODE(GO_CAPITAN_CHEST_H_10N, GO_CAPITAN_CHEST_H_25N, GO_CAPITAN_CHEST_H_10H, GO_CAPITAN_CHEST_H_25H), -590.200022f, 2241.193115f, 539.588269f, 0, 0, 0, 0, 0, 100000);
                            me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
                            me->GetMotionMaster()->MovePoint(0, -590.700f, 2213.01f, 539.1f);
                            break;
                        case EVENT_OUTRO_HORDE_3:
                            me->GetMotionMaster()->MovePoint(1, -555.59f, 2213.01f, 539.28f);
                            break;
                        case EVENT_FAIL:
                            TeleportPlayers(map, HORDE);
							DeletePlayerShirts();
                            events.ScheduleEvent(EVENT_RESTART_EVENT, 2000);
                            instance->SetBossState(DATA_GUNSHIP_EVENT, NOT_STARTED);
                            break;
                        case EVENT_RESTART_EVENT:
							if (instance->GetBossState(DATA_GUNSHIP_EVENT) != DONE)
							{
								instance->SetBossState(DATA_GUNSHIP_EVENT, FAIL);
								RestartEvent(orgrimmar, CheckUnfriendlyShip(me,instance,DATA_GB_MURADIN_BRONZEBEARD), map, HORDE);
							}
                            break;
                        case EVENT_RENDING_THROW:
							if (UpdateVictim())
								if (me->GetVictim()->IsWithinDistInMap(me, 30.0f, false))
                                {
									DoCastVictim(SPELL_RENDING_THROW);
                                    EventScheduled = false;
								}
								else
									events.CancelEvent(EVENT_RENDING_THROW);
							break;
						case EVENT_SPAWN_MAGE:
							//Prevent from spawning multiple Mages
							if (me->GetEntry() != NPC_GB_KORKRON_BATTLE_MAGE)
							{
								DoSendQuantumText(SAY_NEW_BATTLE_MAGE_SPAWNED, me);
								orgrimmar->AddNPCPassengerInInstance(NPC_GB_KORKRON_BATTLE_MAGE, 15.03016f, 0.00016f, 37.70952f, 1.55138f);
							}
							break;
                        case EVENT_RESPAWN_ROCKETEER:
                            DoSendQuantumText(SAY_NEW_ROCKETEERS_SPAWNED, me);
                            if (RocketerCount == 2)
                            {
                                orgrimmar->AddNPCPassengerInInstance(NPC_GB_KORKRON_ROCKETEER, -11.44849f, -25.71838f, 33.64343f, 1.49248f);
                                orgrimmar->AddNPCPassengerInInstance(NPC_GB_KORKRON_ROCKETEER, 12.30336f, -25.69653f, 35.32373f, 1.49248f);
                            }
                            if (RocketerCount == 4)
                            {
                                orgrimmar->AddNPCPassengerInInstance(NPC_GB_KORKRON_ROCKETEER, -11.44849f, -25.71838f, 33.64343f, 1.49248f);
                                orgrimmar->AddNPCPassengerInInstance(NPC_GB_KORKRON_ROCKETEER, 12.30336f, -25.69653f, 35.32373f, 1.49248f);
                                orgrimmar->AddNPCPassengerInInstance(NPC_GB_KORKRON_ROCKETEER, -3.44849f, -25.71838f, 34.21082f, 1.49248f);
                                orgrimmar->AddNPCPassengerInInstance(NPC_GB_KORKRON_ROCKETEER, 3.30336f, -25.69653f, 35.32373f, 1.49248f);
                            }
                            break;
                         case EVENT_RESPAWN_AXES_RIFLEMAN:
                            DoSendQuantumText(SAY_NEW_AXETHROWER_SPAWNED, me);
                            if (AxesCount == 4)
                            {
                                orgrimmar->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, -3.170555f, 28.30652f, 34.21082f, 1.66527f);
                                orgrimmar->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, -12.0928f, 27.65942f, 33.58557f, 1.66527f);
                                orgrimmar->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, 14.92804f, 26.18018f, 35.47803f, 1.66527f);
                                orgrimmar->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, 24.70331f, 25.36584f, 35.97845f, 1.66527f);
                            }
                            if (AxesCount == 6)
                            {
                                orgrimmar->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, -3.170555f, 28.30652f, 34.21082f, 1.66527f);
                                orgrimmar->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, -12.0928f, 27.65942f, 33.58557f, 1.66527f);
                                orgrimmar->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, 14.92804f, 26.18018f, 35.47803f, 1.66527f);
                                orgrimmar->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, 24.70331f, 25.36584f, 35.97845f, 1.66527f);
                                orgrimmar->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, 19.92804f, 27.18018f, 35.47803f, 1.66527f);
                                orgrimmar->AddNPCPassengerInInstance(NPC_GB_KORKRON_AXETHROWER, -7.70331f, 28.36584f, 33.88557f, 1.66527f);
                           }
                           break;
                      }
                }

                if (!me->GetCurrentSpell(CURRENT_MELEE_SPELL))
                    DoCastVictim(SPELL_CLEAVE);

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_high_overlord_saurfang_gunshipAI(creature);
        }
};

class npc_gunship_portal : public CreatureScript
{
    public:
        npc_gunship_portal() : CreatureScript("npc_gunship_portal") { }

        struct npc_gunship_portalAI : public QuantumBasicAI
        {
            npc_gunship_portalAI(Creature* creature) : QuantumBasicAI(creature), instance(creature->GetInstanceScript())
            {
                Reset();
            }

			EventMap events;
			InstanceScript* instance;

            void Reset()
            {
               events.ScheduleEvent(EVENT_UNSUMMON_PORTAL, 20500); // TODO: Fix the timers
               me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void JustDied(Unit* /*killer*/)
            {
                me->RemoveFromWorld();
            }

            void UpdateAI(const uint32 diff)
            {
				events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_UNSUMMON_PORTAL:
                            me->RemoveFromWorld();
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_gunship_portalAI(creature);
        }
};


class npc_gunship_trigger : public CreatureScript
{
    public:
        npc_gunship_trigger() : CreatureScript("npc_gunship_trigger") { }

        struct npc_gunship_triggerAI : public QuantumBasicAI
        {
            npc_gunship_triggerAI(Creature* creature) : QuantumBasicAI(creature), instance(creature->GetInstanceScript())
            {
				SetCombatMovement(false);
                Reset();
            }

			EventMap events;
			InstanceScript* instance;

            void Reset()
            {
                me->SetReactState(REACT_PASSIVE);
            }

            void EnterToBattle(Unit* /*who*/){}

            void DamageTaken(Unit* attacker, uint32& damage)
            {
                damage = 0;
            }

            void UpdateAI(uint32 const /*diff*/){}
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_gunship_triggerAI(creature);
        }
};

class go_gunship_battle_transport : public TransportScript
{
public:
	go_gunship_battle_transport() : TransportScript("go_gunship_battle_transport") { }

	void OnRelocate(Transport* transport, uint32 waypointId, uint32 mapId, float x, float y, float z){}

	void OnAddPassenger(Transport* transport, Player* player)
	{
		if (!transport || !player)
			return;

		switch (transport->GetEntry())
		{
		    case GO_THE_SKYBREAKER_ALLIANCE_ICC:
				player->AddAura(SPELL_ON_SKYBREAKERS_DECK, player);
				break;
			case GO_ORGRIM_S_HAMMER_HORDE_ICC:
				player->AddAura(SPELL_ON_ORGRIMS_HAMMERS_DECK, player);
				break;
			default:
				break;
		}
	}

	void OnRemovePassenger(Transport* /*transport*/, Player* player)
	{
		if (!player)
			return;

		player->RemoveAurasDueToSpell(SPELL_ON_ORGRIMS_HAMMERS_DECK);
		player->RemoveAurasDueToSpell(SPELL_ON_SKYBREAKERS_DECK);
	}
};

class spell_remove_rocket_pack : public SpellScriptLoader
{
    public:
        spell_remove_rocket_pack() : SpellScriptLoader("spell_remove_rocket_pack") { }
 
        class spell_remove_rocket_pack_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_remove_rocket_pack_SpellScript);

			bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_REMOVE_ROCKET_PACK))
                    return false;
                return true;
            }
 
			void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Player* player = GetHitPlayer())
				{
					player->DestroyItemCount(ITEM_THE_ROCKET_PACK, 1, true);
					player->CastSpell(player, 64079, true);
				}
            }
 
            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_remove_rocket_pack_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };
 
        SpellScript* GetSpellScript() const
        {
            return new spell_remove_rocket_pack_SpellScript();
        }
};

class spell_gb_heat_drain : public SpellScriptLoader
{
    public:
        spell_gb_heat_drain() : SpellScriptLoader("spell_gb_heat_drain") { }

        class spell_gb_heat_drain_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gb_heat_drain_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();

                if (caster->GetPower(POWER_ENERGY) != 0)
                    caster->ModifyPower(POWER_ENERGY, -1);

                if (caster->GetPower(POWER_ENERGY) >= 99)
                    caster->CastSpell(caster, SPELL_OVERHEAT, true);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gb_heat_drain_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gb_heat_drain_SpellScript();
        }
};

class spell_gb_overheat_drain : public SpellScriptLoader
{
    public:
        spell_gb_overheat_drain() : SpellScriptLoader("spell_gb_overheat_drain") { }

        class spell_gb_overheat_drain_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gb_overheat_drain_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();

                if (caster->GetPower(POWER_ENERGY) >= 10)
                    caster->ModifyPower(POWER_ENERGY, -10);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gb_overheat_drain_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gb_overheat_drain_SpellScript();
        }
};

class spell_gb_incinerating_blast : public SpellScriptLoader
{
    public:
        spell_gb_incinerating_blast() : SpellScriptLoader("spell_gb_incinerating_blast") { }

        class spell_gb_incinerating_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gb_incinerating_blast_SpellScript);

            void AddExtraDamage()
            {
                Unit* caster = GetCaster();
                if (!caster || !caster->GetPower(POWER_ENERGY))
                    return;

                SetHitDamage(int32(GetHitDamage() + (caster->GetPower(POWER_ENERGY) * 100.0f))); // TODO: How much should be?
                caster->SetPower(POWER_ENERGY, 0);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_gb_incinerating_blast_SpellScript::AddExtraDamage);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gb_incinerating_blast_SpellScript();
        }
};

class spell_gb_burning_pitch : public SpellScriptLoader
{
    public:
        spell_gb_burning_pitch() : SpellScriptLoader("spell_gb_burning_pitch") { }

        class spell_gb_burning_pitch_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gb_burning_pitch_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BURNING_PITCH_SIEGE_DMG_A))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_BURNING_PITCH_SIEGE_DMG_H))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_BURNING_PITCH_AOE_DAMAGE))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();

                if (!caster || !target)
                    return;

                if (GetSpellInfo()->Id == SPELL_BURNING_PITCH_A)
                    caster->CastSpell(target, SPELL_BURNING_PITCH_SIEGE_DMG_A, true);
                else
                    caster->CastSpell(target, SPELL_BURNING_PITCH_SIEGE_DMG_H, true);

                caster->CastSpell(target, SPELL_BURNING_PITCH_AOE_DAMAGE, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gb_burning_pitch_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gb_burning_pitch_SpellScript();
        }
};

class spell_rocket_pack : public SpellScriptLoader
{
    public:
        spell_rocket_pack() : SpellScriptLoader("spell_rocket_pack") { }

        class spell_rocket_pack_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_rocket_pack_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->CastSpell(GetTarget(), SPELL_ROCKET_PACK, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->RemoveAurasDueToSpell(SPELL_ROCKET_PACK);
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_rocket_pack_AuraScript::OnApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_rocket_pack_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_rocket_pack_AuraScript();
        }
};

void AddSC_boss_gunship_battle()
{
    new npc_muradin_bronzebeard_gunship();
    new npc_high_overlord_saurfang_gunship();
    new npc_zafod_boombox();
    new npc_korkron_axethrower_rifleman();
    new npc_mortar_soldier_or_rocketeer();
    new npc_gunship_mage();
    new npc_gunship_cannon();
    new npc_sergeant();
    new npc_marine_or_reaver();
    new npc_gunship_trigger();
    new npc_gunship_portal();
    new npc_gunship_skybreaker();
    new npc_gunship_orgrimmar();
    new go_gunship_battle_transport();
    new spell_remove_rocket_pack();
    new spell_gb_heat_drain();
    new spell_gb_overheat_drain();
    new spell_gb_incinerating_blast();
    new spell_gb_burning_pitch();
	new spell_rocket_pack();
}