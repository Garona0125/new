/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
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

#include "Common.h"
#include "DatabaseEnv.h"
#include "Config.h"
#include "SystemConfig.h"
#include "Log.h"
#include "Opcodes.h"
#include "WorldSession.h"
#include "WorldPacket.h"
#include "Player.h"
#include "Vehicle.h"
#include "SkillExtraItems.h"
#include "SkillDiscovery.h"
#include "World.h"
#include "AccountMgr.h"
#include "AchievementMgr.h"
#include "AuctionHouseMgr.h"
#include "ObjectMgr.h"
#include "ArenaTeamMgr.h"
#include "GuildMgr.h"
#include "TicketMgr.h"
#include "CreatureEventAIMgr.h"
#include "SpellMgr.h"
#include "GroupMgr.h"
#include "Chat.h"
#include "DBCStores.h"
#include "LootMgr.h"
#include "ItemEnchantmentMgr.h"
#include "MapManager.h"
#include "CreatureAIRegistry.h"
#include "BattlegroundMgr.h"
#include "OutdoorPvPMgr.h"
#include "TemporarySummon.h"
#include "WaypointMovementGenerator.h"
#include "VMapFactory.h"
#include "GameEventMgr.h"
#include "PoolMgr.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "InstanceSaveMgr.h"
#include "Util.h"
#include "QuantumSystemText.h"
#include "CreatureGroups.h"
#include "Transport.h"
#include "ScriptMgr.h"
#include "AddonMgr.h"
#include "LFGMgr.h"
#include "ConditionMgr.h"
#include "DisableMgr.h"
#include "CharacterDatabaseCleaner.h"
#include "ScriptMgr.h"
#include "WeatherMgr.h"
#include "CreatureTextMgr.h"
#include "SmartAI.h"
#include "Channel.h"
#include "WardenCheckMgr.h"
#include "Warden.h"
#include "CalendarMgr.h"
#include "OutdoorPvPWG.h"
#include "Guild.h"

ACE_Atomic_Op<ACE_Thread_Mutex, bool> World::m_stopEvent = false;
uint8 World::m_ExitCode = SHUTDOWN_EXIT_CODE;
volatile uint32 World::m_worldLoopCounter = 0;

float World::m_MaxVisibleDistanceOnContinents = DEFAULT_VISIBILITY_DISTANCE;
float World::m_MaxVisibleDistanceInInstances = DEFAULT_VISIBILITY_INSTANCE;
float World::m_MaxVisibleDistanceInBGArenas = DEFAULT_VISIBILITY_BGARENAS;

int32 World::m_visibility_notify_periodOnContinents = DEFAULT_VISIBILITY_NOTIFY_PERIOD;
int32 World::m_visibility_notify_periodInInstances = DEFAULT_VISIBILITY_NOTIFY_PERIOD;
int32 World::m_visibility_notify_periodInBGArenas = DEFAULT_VISIBILITY_NOTIFY_PERIOD;

/// World constructor
World::World()
{
    m_playerLimit = 0;
    m_allowedSecurityLevel = SEC_PLAYER;
    m_allowMovement = true;
    m_ShutdownMask = 0;
    m_ShutdownTimer = 0;
    m_gameTime = time(NULL);
    m_startTime = m_gameTime;
    m_maxActiveSessionCount = 0;
    m_maxQueuedSessionCount = 0;
    m_PlayerCount = 0;
    m_MaxPlayerCount = 0;
    m_NextDailyQuestReset = 0;
    m_NextWeeklyQuestReset = 0;

    m_defaultDbcLocale = LOCALE_enUS;
    m_availableDbcLocaleMask = 0;

    m_updateTimeSum = 0;
    m_updateTimeCount = 0;

    m_isClosed = false;

    m_CleaningFlags = 0;
}

/// World destructor
World::~World()
{
    ///- Empty the kicked session set
    while (!m_sessions.empty())
    {
        // not remove from queue, prevent loading new sessions
        delete m_sessions.begin()->second;
        m_sessions.erase(m_sessions.begin());
    }

    CliCommandHolder* command = NULL;
    while (cliCmdQueue.next(command))
        delete command;

    VMAP::VMapFactory::clear();
}

/// Find a player in a specified zone
Player* World::FindPlayerInZone(uint32 zone)
{
    ///- circle through active sessions and return the first player found in the zone
    SessionMap::const_iterator itr;
    for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (!itr->second)
            continue;

        Player* player = itr->second->GetPlayer();
        if (!player)
            continue;

        if (player->IsInWorld() && player->GetZoneId() == zone)
        {
            // Used by the weather system. We return the player to broadcast the change weather message to him and all players in the zone.
            return player;
        }
    }
    return NULL;
}

bool World::IsClosed() const
{
    return m_isClosed;
}

void World::SetClosed(bool val)
{
    m_isClosed = val;

    // Invert the value, for simplicity for scripters.
    sScriptMgr->OnOpenStateChange(!val);
}

void World::SetMotd(const std::string& motd)
{
    m_motd = motd;

    sScriptMgr->OnMotdChange(m_motd);
}

const char* World::GetMotd() const
{
    return m_motd.c_str();
}

/// Find a session by its id
WorldSession* World::FindSession(uint32 id) const
{
    SessionMap::const_iterator itr = m_sessions.find(id);

    if (itr != m_sessions.end())
        return itr->second;                                 // also can return NULL for kicked session
    else
        return NULL;
}

/// Remove a given session
bool World::RemoveSession(uint32 id)
{
    ///- Find the session, kick the user, but we can't delete session at this moment to prevent iterator invalidation
    SessionMap::const_iterator itr = m_sessions.find(id);

    if (itr != m_sessions.end() && itr->second)
    {
        if (itr->second->PlayerLoading())
            return false;
        itr->second->KickPlayer();
    }
    return true;
}

void World::AddSession(WorldSession* s)
{
    addSessQueue.add(s);
}

void World::AddSession_(WorldSession* s)
{
    ASSERT (s);

    //NOTE - Still there is race condition in WorldSession* being used in the Sockets

    ///- kick already loaded player with same account (if any) and remove session
    ///- if player is in loading and want to load again, return
    if (!RemoveSession (s->GetAccountId()))
    {
        s->KickPlayer();
        delete s;                                           // session not added yet in session list, so not listed in queue
        return;
    }

    // decrease session counts only at not reconnection case
    bool decrease_session = true;
    {
        SessionMap::const_iterator old = m_sessions.find(s->GetAccountId());

        if (old != m_sessions.end())
        {
            // prevent decrease sessions count if session queued
            if (RemoveQueuedPlayer(old->second))
                decrease_session = false;
            // not remove replaced session form queue if listed
            delete old->second;
        }
    }

    m_sessions[s->GetAccountId()] = s;

    uint32 Sessions = GetActiveAndQueuedSessionCount();
    uint32 pLimit = GetPlayerAmountLimit();
    uint32 QueueSize = GetQueuedSessionCount();             //number of players in the queue

    //so we don't count the user trying to
    //login as a session and queue the socket that we are using
    if (decrease_session)
        --Sessions;

    if (pLimit > 0 && Sessions >= pLimit && AccountMgr::IsPlayerAccount(s->GetSecurity()) && !HasRecentlyDisconnected(s))
    {
        AddQueuedPlayer (s);
        UpdateMaxSessionCounters();
        sLog->outDetail ("PlayerQueue: Account id %u is in Queue Position (%u).", s->GetAccountId(), ++QueueSize);
        return;
    }

    s->SendAuthResponse(AUTH_OK, true);
    s->SendAddonsInfo();
    s->SendClientCacheVersion(sWorld->getIntConfig(CONFIG_CLIENTCACHE_VERSION));
    s->SendTutorialsData();

    UpdateMaxSessionCounters();

    if (pLimit > 0)
    {
        float popu = (float)GetActiveSessionCount();              // updated number of users on the server
        popu /= pLimit;
        popu *= 2;
        sLog->outDetail ("Server Population (%f).", popu);
    }
}

bool World::HasRecentlyDisconnected(WorldSession* session)
{
    if (!session)
        return false;

    if (uint32 tolerance = getIntConfig(CONFIG_INTERVAL_DISCONNECT_TOLERANCE))
    {
        for (DisconnectMap::iterator i = m_disconnects.begin(); i != m_disconnects.end();)
        {
            if (difftime(i->second, time(NULL)) < tolerance)
            {
                if (i->first == session->GetAccountId())
                    return true;
                ++i;
            }
            else
                m_disconnects.erase(i++);
        }
    }
    return false;
 }

int32 World::GetQueuePos(WorldSession* sess)
{
    uint32 position = 1;

    for (Queue::const_iterator iter = m_QueuedPlayer.begin(); iter != m_QueuedPlayer.end(); ++iter, ++position)
        if ((*iter) == sess)
            return position;

    return 0;
}

void World::AddQueuedPlayer(WorldSession* sess)
{
    sess->SetInQueue(true);
    m_QueuedPlayer.push_back(sess);

    // The 1st SMSG_AUTH_RESPONSE needs to contain other info too.
    sess->SendAuthResponse(AUTH_WAIT_QUEUE, false, GetQueuePos(sess));
}

bool World::RemoveQueuedPlayer(WorldSession* sess)
{
    // sessions count including queued to remove (if removed_session set)
    uint32 sessions = GetActiveSessionCount();

    uint32 position = 1;
    Queue::iterator iter = m_QueuedPlayer.begin();

    // search to remove and count skipped positions
    bool found = false;

    for (; iter != m_QueuedPlayer.end(); ++iter, ++position)
    {
        if (*iter == sess)
        {
            sess->SetInQueue(false);
            sess->ResetTimeOutTime();
            iter = m_QueuedPlayer.erase(iter);
            found = true;                                   // removing queued session
            break;
        }
    }

    if (!found && sessions)
        --sessions;

    if ((!m_playerLimit || sessions < m_playerLimit) && !m_QueuedPlayer.empty())
    {
        WorldSession* pop_sess = m_QueuedPlayer.front();
        pop_sess->SetInQueue(false);
        pop_sess->ResetTimeOutTime();
        pop_sess->SendAuthWaitQue(0);
        pop_sess->SendAddonsInfo();

        pop_sess->SendClientCacheVersion(sWorld->getIntConfig(CONFIG_CLIENTCACHE_VERSION));
        pop_sess->SendAccountDataTimes(GLOBAL_CACHE_MASK);
        pop_sess->SendTutorialsData();

        m_QueuedPlayer.pop_front();

        // update iter to point first queued socket or end() if queue is empty now
        iter = m_QueuedPlayer.begin();
        position = 1;
    }

    for (; iter != m_QueuedPlayer.end(); ++iter, ++position)
        (*iter)->SendAuthWaitQue(position);

    return found;
}

/// Initialize config values
void World::LoadConfigSettings(bool reload)
{
    if (reload)
    {
        if (!GetConfigSettings::Load())
        {
            sLog->OutErrorConsole("World settings reload fail: can't read settings from %s.", GetConfigSettings::GetFilename().c_str());
            return;
        }

        sLog->ReloadConfig(); // Reload log levels and filters
    }

    ///- Read the player limit and the Message of the day from the config file
    SetPlayerAmountLimit(GetConfigSettings::GetIntState("PlayerLimit", 100));
    SetMotd(GetConfigSettings::GetStringDefault("Motd", "Welcome to Quantum Core Server by Crispi."));

    ///- Read ticket system setting from the config file
    m_bool_configs[CONFIG_ALLOW_TICKETS] = GetConfigSettings::GetBoolState("AllowTickets", true);

    ///- Get string for new logins (newly created characters)
    SetNewCharString(GetConfigSettings::GetStringDefault("PlayerStart.String", ""));

    ///- Send server info on login?
    m_int_configs[CONFIG_ENABLE_SINFO_LOGIN] = GetConfigSettings::GetIntState("Server.LoginInfo", 0);

    // XP.Boost
    rate_values[RATE_XP_BOOST_SOLO] = GetConfigSettings::GetFloatDefault("XP.Boost.Solo", 1.0f);
    rate_values[RATE_XP_BOOST_GROUP] = GetConfigSettings::GetFloatDefault("XP.Boost.Group", 1.0f);
    m_int_configs[CONFIG_XP_BOOST_ITEM_ID] = GetConfigSettings::GetIntState("XP.Boost.ItemId", 0);
    m_int_configs[CONFIG_XP_BOOST_MAX_LEVEL] = GetConfigSettings::GetIntState("XP.Boost.MaxLevel", 0);

    ///- Read all rates from the config file
    rate_values[RATE_HEALTH] = GetConfigSettings::GetFloatDefault("Rate.Health", 1);
    if (rate_values[RATE_HEALTH] < 0)
    {
        sLog->OutErrorConsole("Rate.Health (%f) must be > 0. Using 1 instead.", rate_values[RATE_HEALTH]);
        rate_values[RATE_HEALTH] = 1;
    }
    rate_values[RATE_POWER_MANA] = GetConfigSettings::GetFloatDefault("Rate.Mana", 1);
    if (rate_values[RATE_POWER_MANA] < 0)
    {
        sLog->OutErrorConsole("Rate.Mana (%f) must be > 0. Using 1 instead.", rate_values[RATE_POWER_MANA]);
        rate_values[RATE_POWER_MANA] = 1;
    }
    rate_values[RATE_POWER_RAGE_INCOME] = GetConfigSettings::GetFloatDefault("Rate.Rage.Income", 1);
    rate_values[RATE_POWER_RAGE_LOSS] = GetConfigSettings::GetFloatDefault("Rate.Rage.Loss", 1);
    if (rate_values[RATE_POWER_RAGE_LOSS] < 0)
    {
        sLog->OutErrorConsole("Rate.Rage.Loss (%f) must be > 0. Using 1 instead.", rate_values[RATE_POWER_RAGE_LOSS]);
        rate_values[RATE_POWER_RAGE_LOSS] = 1;
    }
    rate_values[RATE_POWER_RUNICPOWER_INCOME] = GetConfigSettings::GetFloatDefault("Rate.RunicPower.Income", 1);
    rate_values[RATE_POWER_RUNICPOWER_LOSS] = GetConfigSettings::GetFloatDefault("Rate.RunicPower.Loss", 1);
    if (rate_values[RATE_POWER_RUNICPOWER_LOSS] < 0)
    {
        sLog->OutErrorConsole("Rate.RunicPower.Loss (%f) must be > 0. Using 1 instead.", rate_values[RATE_POWER_RUNICPOWER_LOSS]);
        rate_values[RATE_POWER_RUNICPOWER_LOSS] = 1;
    }
    rate_values[RATE_POWER_FOCUS] = GetConfigSettings::GetFloatDefault("Rate.Focus", 1.0f);
    rate_values[RATE_POWER_ENERGY] = GetConfigSettings::GetFloatDefault("Rate.Energy", 1.0f);

    rate_values[RATE_SKILL_DISCOVERY] = GetConfigSettings::GetFloatDefault("Rate.Skill.Discovery", 1.0f);

    rate_values[RATE_DROP_ITEM_POOR] = GetConfigSettings::GetFloatDefault("Rate.Drop.Item.Poor", 1.0f);
    rate_values[RATE_DROP_ITEM_NORMAL] = GetConfigSettings::GetFloatDefault("Rate.Drop.Item.Normal", 1.0f);
    rate_values[RATE_DROP_ITEM_UNCOMMON] = GetConfigSettings::GetFloatDefault("Rate.Drop.Item.Uncommon", 1.0f);
    rate_values[RATE_DROP_ITEM_RARE] = GetConfigSettings::GetFloatDefault("Rate.Drop.Item.Rare", 1.0f);
    rate_values[RATE_DROP_ITEM_EPIC] = GetConfigSettings::GetFloatDefault("Rate.Drop.Item.Epic", 1.0f);
    rate_values[RATE_DROP_ITEM_LEGENDARY] = GetConfigSettings::GetFloatDefault("Rate.Drop.Item.Legendary", 1.0f);
    rate_values[RATE_DROP_ITEM_ARTIFACT] = GetConfigSettings::GetFloatDefault("Rate.Drop.Item.Artifact", 1.0f);
    rate_values[RATE_DROP_ITEM_REFERENCED] = GetConfigSettings::GetFloatDefault("Rate.Drop.Item.Referenced", 1.0f);
    rate_values[RATE_DROP_ITEM_REFERENCED_AMOUNT] = GetConfigSettings::GetFloatDefault("Rate.Drop.Item.ReferencedAmount", 1.0f);
    rate_values[RATE_DROP_MONEY] = GetConfigSettings::GetFloatDefault("Rate.Drop.Money", 1.0f);
	rate_values[RATE_XP_KILL] = GetConfigSettings::GetFloatDefault("Rate.XP.Kill", 1.0f);
	rate_values[RATE_XP_BG_KILL]  = GetConfigSettings::GetFloatDefault("Rate.XP.BattlegroundKill", 1.0f);
	rate_values[RATE_XP_KILL_PREMIUM] = GetConfigSettings::GetFloatDefault("Rate.XP.Kill.Premium", 1.0f);
	rate_values[RATE_XP_QUEST] = GetConfigSettings::GetFloatDefault("Rate.XP.Quest", 1.0f);
	rate_values[RATE_XP_QUEST_PREMIUM] = GetConfigSettings::GetFloatDefault("Rate.XP.Quest.Premium", 1.0f);
	rate_values[RATE_XP_EXPLORE] = GetConfigSettings::GetFloatDefault("Rate.XP.Explore", 1.0f);
	rate_values[RATE_XP_EXPLORE_PREMIUM] = GetConfigSettings::GetFloatDefault("Rate.XP.Explore.Premium", 1.0f);
    rate_values[RATE_REPAIRCOST] = GetConfigSettings::GetFloatDefault("Rate.RepairCost", 1.0f);
    if (rate_values[RATE_REPAIRCOST] < 0.0f)
    {
        sLog->OutErrorConsole("Rate.RepairCost (%f) must be >=0. Using 0.0 instead.", rate_values[RATE_REPAIRCOST]);
        rate_values[RATE_REPAIRCOST] = 0.0f;
    }
    rate_values[RATE_REPUTATION_GAIN] = GetConfigSettings::GetFloatDefault("Rate.Reputation.Gain", 1.0f);
    rate_values[RATE_REPUTATION_LOWLEVEL_KILL] = GetConfigSettings::GetFloatDefault("Rate.Reputation.LowLevel.Kill", 1.0f);
    rate_values[RATE_REPUTATION_LOWLEVEL_QUEST] = GetConfigSettings::GetFloatDefault("Rate.Reputation.LowLevel.Quest", 1.0f);
    rate_values[RATE_REPUTATION_RECRUIT_A_FRIEND_BONUS] = GetConfigSettings::GetFloatDefault("Rate.Reputation.RecruitAFriendBonus", 0.1f);

    rate_values[RATE_CREATURE_AGGRO] = GetConfigSettings::GetFloatDefault("Rate.Creature.Aggro", 1.0f);
    rate_values[RATE_REST_INGAME] = GetConfigSettings::GetFloatDefault("Rate.Rest.InGame", 1.0f);
    rate_values[RATE_REST_OFFLINE_IN_TAVERN_OR_CITY] = GetConfigSettings::GetFloatDefault("Rate.Rest.Offline.InTavernOrCity", 1.0f);
    rate_values[RATE_REST_OFFLINE_IN_WILDERNESS] = GetConfigSettings::GetFloatDefault("Rate.Rest.Offline.InWilderness", 1.0f);
    rate_values[RATE_DAMAGE_FALL] = GetConfigSettings::GetFloatDefault("Rate.Damage.Fall", 1.0f);
    rate_values[RATE_AUCTION_TIME] = GetConfigSettings::GetFloatDefault("Rate.Auction.Time", 1.0f);
    rate_values[RATE_AUCTION_DEPOSIT] = GetConfigSettings::GetFloatDefault("Rate.Auction.Deposit", 1.0f);
    rate_values[RATE_AUCTION_CUT] = GetConfigSettings::GetFloatDefault("Rate.Auction.Cut", 1.0f);
    rate_values[RATE_HONOR] = GetConfigSettings::GetFloatDefault("Rate.Honor", 1.0f);
	rate_values[RATE_HONOR_PREMIUM] = GetConfigSettings::GetFloatDefault("Rate.Honor.Premium", 1.0f);
    rate_values[RATE_MINING_AMOUNT] = GetConfigSettings::GetFloatDefault("Rate.Mining.Amount", 1.0f);
    rate_values[RATE_MINING_NEXT] = GetConfigSettings::GetFloatDefault("Rate.Mining.Next", 1.0f);
    rate_values[RATE_INSTANCE_RESET_TIME] = GetConfigSettings::GetFloatDefault("Rate.InstanceResetTime", 1.0f);
    rate_values[RATE_TALENT] = GetConfigSettings::GetFloatDefault("Rate.Talent", 1.0f);

    if (rate_values[RATE_TALENT] < 0.0f)
    {
        sLog->OutErrorConsole("Rate.Talent (%f) must be > 0. Using 1 instead.", rate_values[RATE_TALENT]);
        rate_values[RATE_TALENT] = 1.0f;
    }
    rate_values[RATE_MOVESPEED] = GetConfigSettings::GetFloatDefault("Rate.MoveSpeed", 1.0f);
    if (rate_values[RATE_MOVESPEED] < 0)
    {
        sLog->OutErrorConsole("Rate.MoveSpeed (%f) must be > 0. Using 1 instead.", rate_values[RATE_MOVESPEED]);
        rate_values[RATE_MOVESPEED] = 1.0f;
    }
    for (uint8 i = 0; i < MAX_MOVE_TYPE; ++i) playerBaseMoveSpeed[i] = baseMoveSpeed[i] * rate_values[RATE_MOVESPEED];
    rate_values[RATE_CORPSE_DECAY_LOOTED] = GetConfigSettings::GetFloatDefault("Rate.Corpse.Decay.Looted", 0.5f);

    rate_values[RATE_TARGET_POS_RECALCULATION_RANGE] = GetConfigSettings::GetFloatDefault("TargetPosRecalculateRange", 1.5f);
    if (rate_values[RATE_TARGET_POS_RECALCULATION_RANGE] < CONTACT_DISTANCE)
    {
        sLog->OutErrorConsole("TargetPosRecalculateRange (%f) must be >= %f. Using %f instead.", rate_values[RATE_TARGET_POS_RECALCULATION_RANGE], CONTACT_DISTANCE, CONTACT_DISTANCE);
        rate_values[RATE_TARGET_POS_RECALCULATION_RANGE] = CONTACT_DISTANCE;
    }
    else if (rate_values[RATE_TARGET_POS_RECALCULATION_RANGE] > NOMINAL_MELEE_RANGE)
    {
        sLog->OutErrorConsole("TargetPosRecalculateRange (%f) must be <= %f. Using %f instead.",
            rate_values[RATE_TARGET_POS_RECALCULATION_RANGE], NOMINAL_MELEE_RANGE, NOMINAL_MELEE_RANGE);
        rate_values[RATE_TARGET_POS_RECALCULATION_RANGE] = NOMINAL_MELEE_RANGE;
    }

    rate_values[RATE_DURABILITY_LOSS_ON_DEATH] = GetConfigSettings::GetFloatDefault("DurabilityLoss.OnDeath", 10.0f);
    if (rate_values[RATE_DURABILITY_LOSS_ON_DEATH] < 0.0f)
    {
        sLog->OutErrorConsole("DurabilityLoss.OnDeath (%f) must be >=0. Using 0.0 instead.", rate_values[RATE_DURABILITY_LOSS_ON_DEATH]);
        rate_values[RATE_DURABILITY_LOSS_ON_DEATH] = 0.0f;
    }
    if (rate_values[RATE_DURABILITY_LOSS_ON_DEATH] > 100.0f)
    {
        sLog->OutErrorConsole("DurabilityLoss.OnDeath (%f) must be <= 100. Using 100.0 instead.", rate_values[RATE_DURABILITY_LOSS_ON_DEATH]);
        rate_values[RATE_DURABILITY_LOSS_ON_DEATH] = 0.0f;
    }
    rate_values[RATE_DURABILITY_LOSS_ON_DEATH] = rate_values[RATE_DURABILITY_LOSS_ON_DEATH] / 100.0f;

    rate_values[RATE_DURABILITY_LOSS_DAMAGE] = GetConfigSettings::GetFloatDefault("DurabilityLossChance.Damage", 0.5f);
    if (rate_values[RATE_DURABILITY_LOSS_DAMAGE] < 0.0f)
    {
        sLog->OutErrorConsole("DurabilityLossChance.Damage (%f) must be >=0. Using 0.0 instead.", rate_values[RATE_DURABILITY_LOSS_DAMAGE]);
        rate_values[RATE_DURABILITY_LOSS_DAMAGE] = 0.0f;
    }
    rate_values[RATE_DURABILITY_LOSS_ABSORB] = GetConfigSettings::GetFloatDefault("DurabilityLossChance.Absorb", 0.5f);
    if (rate_values[RATE_DURABILITY_LOSS_ABSORB] < 0.0f)
    {
        sLog->OutErrorConsole("DurabilityLossChance.Absorb (%f) must be >=0. Using 0.0 instead.", rate_values[RATE_DURABILITY_LOSS_ABSORB]);
        rate_values[RATE_DURABILITY_LOSS_ABSORB] = 0.0f;
    }
    rate_values[RATE_DURABILITY_LOSS_PARRY] = GetConfigSettings::GetFloatDefault("DurabilityLossChance.Parry", 0.05f);
    if (rate_values[RATE_DURABILITY_LOSS_PARRY] < 0.0f)
    {
        sLog->OutErrorConsole("DurabilityLossChance.Parry (%f) must be >=0. Using 0.0 instead.", rate_values[RATE_DURABILITY_LOSS_PARRY]);
        rate_values[RATE_DURABILITY_LOSS_PARRY] = 0.0f;
    }
    rate_values[RATE_DURABILITY_LOSS_BLOCK] = GetConfigSettings::GetFloatDefault("DurabilityLossChance.Block", 0.05f);
    if (rate_values[RATE_DURABILITY_LOSS_BLOCK] < 0.0f)
    {
        sLog->OutErrorConsole("DurabilityLossChance.Block (%f) must be >=0. Using 0.0 instead.", rate_values[RATE_DURABILITY_LOSS_BLOCK]);
        rate_values[RATE_DURABILITY_LOSS_BLOCK] = 0.0f;
	}
	rate_values[RATE_MONEY_QUEST] = GetConfigSettings::GetFloatDefault("Rate.Quest.Money.Reward", 1.0f);
    if (rate_values[RATE_MONEY_QUEST] < 0.0f)
    {
        sLog->OutErrorConsole("Rate.Quest.Money.Reward (%f) must be >=0. Using 0 instead.", rate_values[RATE_MONEY_QUEST]);
        rate_values[RATE_MONEY_QUEST] = 0.0f;
    }
    rate_values[RATE_MONEY_MAX_LEVEL_QUEST] = GetConfigSettings::GetFloatDefault("Rate.Quest.Money.Max.Level.Reward", 1.0f);
    if (rate_values[RATE_MONEY_MAX_LEVEL_QUEST] < 0.0f)
    {
        sLog->OutErrorConsole("Rate.Quest.Money.Max.Level.Reward (%f) must be >=0. Using 0 instead.", rate_values[RATE_MONEY_MAX_LEVEL_QUEST]);
        rate_values[RATE_MONEY_MAX_LEVEL_QUEST] = 0.0f;
    }
	///- Read other configuration items from the config file

    m_bool_configs[CONFIG_DURABILITY_LOSS_IN_PVP] = GetConfigSettings::GetBoolState("DurabilityLoss.InPvP", false);

    m_int_configs[CONFIG_COMPRESSION] = GetConfigSettings::GetIntState("Compression", 1);
    if (m_int_configs[CONFIG_COMPRESSION] < 1 || m_int_configs[CONFIG_COMPRESSION] > 9)
    {
        sLog->OutErrorConsole("Compression level (%i) must be in range 1..9. Using default compression level (1).", m_int_configs[CONFIG_COMPRESSION]);
        m_int_configs[CONFIG_COMPRESSION] = 1;
    }
    m_bool_configs[CONFIG_ADDON_CHANNEL] = GetConfigSettings::GetBoolState("AddonChannel", true);
    m_bool_configs[CONFIG_CLEAN_CHARACTER_DB] = GetConfigSettings::GetBoolState("CleanCharacterDB", false);
    m_int_configs[CONFIG_PERSISTENT_CHARACTER_CLEAN_FLAGS] = GetConfigSettings::GetIntState("PersistentCharacterCleanFlags", 0);
    m_int_configs[CONFIG_CHAT_CHANNEL_LEVEL_REQ] = GetConfigSettings::GetIntState("ChatLevelReq.Channel", 1);
    m_int_configs[CONFIG_CHAT_WHISPER_LEVEL_REQ] = GetConfigSettings::GetIntState("ChatLevelReq.Whisper", 1);
    m_int_configs[CONFIG_CHAT_SAY_LEVEL_REQ] = GetConfigSettings::GetIntState("ChatLevelReq.Say", 1);
    m_int_configs[CONFIG_TRADE_LEVEL_REQ] = GetConfigSettings::GetIntState("LevelReq.Trade", 1);
    m_int_configs[CONFIG_TICKET_LEVEL_REQ] = GetConfigSettings::GetIntState("LevelReq.Ticket", 1);
    m_int_configs[CONFIG_AUCTION_LEVEL_REQ] = GetConfigSettings::GetIntState("LevelReq.Auction", 1);
    m_int_configs[CONFIG_MAIL_LEVEL_REQ] = GetConfigSettings::GetIntState("LevelReq.Mail", 1);
    m_bool_configs[CONFIG_ALLOW_PLAYER_COMMANDS] = GetConfigSettings::GetBoolState("AllowPlayerCommands", 1);
    m_bool_configs[CONFIG_PRESERVE_CUSTOM_CHANNELS] = GetConfigSettings::GetBoolState("PreserveCustomChannels", false);
    m_int_configs[CONFIG_PRESERVE_CUSTOM_CHANNEL_DURATION] = GetConfigSettings::GetIntState("PreserveCustomChannelDuration", 14);
    m_bool_configs[CONFIG_GRID_UNLOAD] = GetConfigSettings::GetBoolState("GridUnload", true);
    m_int_configs[CONFIG_INTERVAL_SAVE] = GetConfigSettings::GetIntState("PlayerSaveInterval", 15*MINUTE *IN_MILLISECONDS);
    m_int_configs[CONFIG_INTERVAL_DISCONNECT_TOLERANCE] = GetConfigSettings::GetIntState("DisconnectToleranceInterval", 0);
    m_bool_configs[CONFIG_STATS_SAVE_ONLY_ON_LOGOUT] = GetConfigSettings::GetBoolState("PlayerSave.Stats.SaveOnlyOnLogout", true);

    m_int_configs[CONFIG_MIN_LEVEL_STAT_SAVE] = GetConfigSettings::GetIntState("PlayerSave.Stats.MinLevel", 0);
    if (m_int_configs[CONFIG_MIN_LEVEL_STAT_SAVE] > MAX_LEVEL)
    {
        sLog->OutErrorConsole("PlayerSave.Stats.MinLevel (%i) must be in range 0..80. Using default, do not save character stats (0).", m_int_configs[CONFIG_MIN_LEVEL_STAT_SAVE]);
        m_int_configs[CONFIG_MIN_LEVEL_STAT_SAVE] = 0;
    }

    m_int_configs[CONFIG_INTERVAL_GRIDCLEAN] = GetConfigSettings::GetIntState("GridCleanUpDelay", 5 * MINUTE * IN_MILLISECONDS);
    if (m_int_configs[CONFIG_INTERVAL_GRIDCLEAN] < MIN_GRID_DELAY)
    {
        sLog->OutErrorConsole("GridCleanUpDelay (%i) must be greater %u. Use this minimal value.", m_int_configs[CONFIG_INTERVAL_GRIDCLEAN], MIN_GRID_DELAY);
        m_int_configs[CONFIG_INTERVAL_GRIDCLEAN] = MIN_GRID_DELAY;
    }
    if (reload)
        sMapMgr->SetGridCleanUpDelay(m_int_configs[CONFIG_INTERVAL_GRIDCLEAN]);

    m_int_configs[CONFIG_INTERVAL_MAPUPDATE] = GetConfigSettings::GetIntState("MapUpdateInterval", 100);
    if (m_int_configs[CONFIG_INTERVAL_MAPUPDATE] < MIN_MAP_UPDATE_DELAY)
    {
        sLog->OutErrorConsole("MapUpdateInterval (%i) must be greater %u. Use this minimal value.", m_int_configs[CONFIG_INTERVAL_MAPUPDATE], MIN_MAP_UPDATE_DELAY);
        m_int_configs[CONFIG_INTERVAL_MAPUPDATE] = MIN_MAP_UPDATE_DELAY;
    }
    if (reload)
        sMapMgr->SetMapUpdateInterval(m_int_configs[CONFIG_INTERVAL_MAPUPDATE]);

    m_int_configs[CONFIG_INTERVAL_CHANGEWEATHER] = GetConfigSettings::GetIntState("ChangeWeatherInterval", 10 * MINUTE * IN_MILLISECONDS);

    if (reload)
    {
        uint32 val = GetConfigSettings::GetIntState("WorldServerPort", 8085);
        if (val != m_int_configs[CONFIG_PORT_WORLD])
            sLog->OutErrorConsole("WorldServerPort option can't be changed at worldserver.conf reload, using current value (%u).", m_int_configs[CONFIG_PORT_WORLD]);
    }
    else
        m_int_configs[CONFIG_PORT_WORLD] = GetConfigSettings::GetIntState("WorldServerPort", 8085);

    m_int_configs[CONFIG_SOCKET_TIMEOUTTIME] = GetConfigSettings::GetIntState("SocketTimeOutTime", 900000);
    m_int_configs[CONFIG_SESSION_ADD_DELAY] = GetConfigSettings::GetIntState("SessionAddDelay", 10000);

    m_float_configs[CONFIG_GROUP_XP_DISTANCE] = GetConfigSettings::GetFloatDefault("MaxGroupXPDistance", 74.0f);
    m_float_configs[CONFIG_MAX_RECRUIT_A_FRIEND_DISTANCE] = GetConfigSettings::GetFloatDefault("MaxRecruitAFriendBonusDistance", 100.0f);

    /// \todo Add MonsterSight and GuarderSight (with meaning) in worldserver.conf or put them as define
    m_float_configs[CONFIG_SIGHT_MONSTER] = GetConfigSettings::GetFloatDefault("MonsterSight", 50);
    m_float_configs[CONFIG_SIGHT_GUARDER] = GetConfigSettings::GetFloatDefault("GuarderSight", 50);

    if (reload)
    {
        uint32 val = GetConfigSettings::GetIntState("GameType", 0);
        if (val != m_int_configs[CONFIG_GAME_TYPE])
            sLog->OutErrorConsole("GameType option can't be changed at worldserver.conf reload, using current value (%u).", m_int_configs[CONFIG_GAME_TYPE]);
    }
    else
        m_int_configs[CONFIG_GAME_TYPE] = GetConfigSettings::GetIntState("GameType", 0);

    if (reload)
    {
        uint32 val = GetConfigSettings::GetIntState("RealmZone", REALM_ZONE_DEVELOPMENT);
        if (val != m_int_configs[CONFIG_REALM_ZONE])
            sLog->OutErrorConsole("RealmZone option can't be changed at worldserver.conf reload, using current value (%u).", m_int_configs[CONFIG_REALM_ZONE]);
    }
    else
        m_int_configs[CONFIG_REALM_ZONE] = GetConfigSettings::GetIntState("RealmZone", REALM_ZONE_DEVELOPMENT);

    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_ACCOUNTS]            = GetConfigSettings::GetBoolState("AllowTwoSide.Accounts", true);
	m_bool_configs[CONFIG_ALLOW_TWO_SIDE_INTERACTION_CALENDAR]= GetConfigSettings::GetBoolState("AllowTwoSide.Interaction.Calendar", false);
    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHAT]    = GetConfigSettings::GetBoolState("AllowTwoSide.Interaction.Chat", false);
    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_INTERACTION_CHANNEL] = GetConfigSettings::GetBoolState("AllowTwoSide.Interaction.Channel", false);
    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP]   = GetConfigSettings::GetBoolState("AllowTwoSide.Interaction.Group", false);
    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_INTERACTION_GUILD]   = GetConfigSettings::GetBoolState("AllowTwoSide.Interaction.Guild", false);
    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_INTERACTION_AUCTION] = GetConfigSettings::GetBoolState("AllowTwoSide.Interaction.Auction", false);
    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_INTERACTION_MAIL]    = GetConfigSettings::GetBoolState("AllowTwoSide.Interaction.Mail", false);
    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_WHO_LIST]            = GetConfigSettings::GetBoolState("AllowTwoSide.WhoList", false);
    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_ADD_FRIEND]          = GetConfigSettings::GetBoolState("AllowTwoSide.AddFriend", false);
    m_bool_configs[CONFIG_ALLOW_TWO_SIDE_TRADE]               = GetConfigSettings::GetBoolState("AllowTwoSide.trade", false);
    m_int_configs[CONFIG_STRICT_PLAYER_NAMES]                 = GetConfigSettings::GetIntState("StrictPlayerNames", 0);
    m_int_configs[CONFIG_STRICT_CHARTER_NAMES]                = GetConfigSettings::GetIntState("StrictCharterNames", 0);
    m_int_configs[CONFIG_STRICT_PET_NAMES]                    = GetConfigSettings::GetIntState("StrictPetNames", 0);

    m_int_configs[CONFIG_MIN_PLAYER_NAME] = GetConfigSettings::GetIntState("MinPlayerName",  2);
    if (m_int_configs[CONFIG_MIN_PLAYER_NAME] < 1 || m_int_configs[CONFIG_MIN_PLAYER_NAME] > MAX_PLAYER_NAME)
    {
        sLog->OutErrorConsole("MinPlayerName (%i) must be in range 1..%u. Set to 2.", m_int_configs[CONFIG_MIN_PLAYER_NAME], MAX_PLAYER_NAME);
        m_int_configs[CONFIG_MIN_PLAYER_NAME] = 2;
    }

    m_int_configs[CONFIG_MIN_CHARTER_NAME] = GetConfigSettings::GetIntState("MinCharterName", 2);
    if (m_int_configs[CONFIG_MIN_CHARTER_NAME] < 1 || m_int_configs[CONFIG_MIN_CHARTER_NAME] > MAX_CHARTER_NAME)
    {
        sLog->OutErrorConsole("MinCharterName (%i) must be in range 1..%u. Set to 2.", m_int_configs[CONFIG_MIN_CHARTER_NAME], MAX_CHARTER_NAME);
        m_int_configs[CONFIG_MIN_CHARTER_NAME] = 2;
    }

    m_int_configs[CONFIG_MIN_PET_NAME] = GetConfigSettings::GetIntState("MinPetName",     2);
    if (m_int_configs[CONFIG_MIN_PET_NAME] < 1 || m_int_configs[CONFIG_MIN_PET_NAME] > MAX_PET_NAME)
    {
        sLog->OutErrorConsole("MinPetName (%i) must be in range 1..%u. Set to 2.", m_int_configs[CONFIG_MIN_PET_NAME], MAX_PET_NAME);
        m_int_configs[CONFIG_MIN_PET_NAME] = 2;
    }

	m_int_configs[CONFIG_CHARTER_COST_GUILD] = GetConfigSettings::GetIntState("Guild.CharterCost", 1000);
    m_int_configs[CONFIG_CHARTER_COST_ARENA_2V2] = GetConfigSettings::GetIntState("ArenaTeam.CharterCost.2v2", 800000);
    m_int_configs[CONFIG_CHARTER_COST_ARENA_3V3] = GetConfigSettings::GetIntState("ArenaTeam.CharterCost.3v3", 1200000);
    m_int_configs[CONFIG_CHARTER_COST_ARENA_5V5] = GetConfigSettings::GetIntState("ArenaTeam.CharterCost.5v5", 2000000);

    m_int_configs[CONFIG_CHARACTER_CREATING_DISABLED] = GetConfigSettings::GetIntState("CharacterCreating.Disabled", 0);
    m_int_configs[CONFIG_CHARACTER_CREATING_DISABLED_RACEMASK] = GetConfigSettings::GetIntState("CharacterCreating.Disabled.RaceMask", 0);
    m_int_configs[CONFIG_CHARACTER_CREATING_DISABLED_CLASSMASK] = GetConfigSettings::GetIntState("CharacterCreating.Disabled.ClassMask", 0);

    m_int_configs[CONFIG_CHARACTERS_PER_REALM] = GetConfigSettings::GetIntState("CharactersPerRealm", 10);
    if (m_int_configs[CONFIG_CHARACTERS_PER_REALM] < 1 || m_int_configs[CONFIG_CHARACTERS_PER_REALM] > 10)
    {
        sLog->OutErrorConsole("CharactersPerRealm (%i) must be in range 1..10. Set to 10.", m_int_configs[CONFIG_CHARACTERS_PER_REALM]);
        m_int_configs[CONFIG_CHARACTERS_PER_REALM] = 10;
    }
    m_int_configs[CONFIG_CHARACTERS_PER_ACCOUNT] = GetConfigSettings::GetIntState("CharactersPerAccount", 50);
    if (m_int_configs[CONFIG_CHARACTERS_PER_ACCOUNT] < m_int_configs[CONFIG_CHARACTERS_PER_REALM])
    {
        sLog->OutErrorConsole("CharactersPerAccount (%i) can't be less than CharactersPerRealm (%i).", m_int_configs[CONFIG_CHARACTERS_PER_ACCOUNT], m_int_configs[CONFIG_CHARACTERS_PER_REALM]);
        m_int_configs[CONFIG_CHARACTERS_PER_ACCOUNT] = m_int_configs[CONFIG_CHARACTERS_PER_REALM];
    }

    m_int_configs[CONFIG_HEROIC_CHARACTERS_PER_REALM] = GetConfigSettings::GetIntState("HeroicCharactersPerRealm", 1);
    if (int32(m_int_configs[CONFIG_HEROIC_CHARACTERS_PER_REALM]) < 0 || m_int_configs[CONFIG_HEROIC_CHARACTERS_PER_REALM] > 10)
    {
        sLog->OutErrorConsole("HeroicCharactersPerRealm (%i) must be in range 0..10. Set to 1.", m_int_configs[CONFIG_HEROIC_CHARACTERS_PER_REALM]);
        m_int_configs[CONFIG_HEROIC_CHARACTERS_PER_REALM] = 1;
    }

    m_int_configs[CONFIG_CHARACTER_CREATING_MIN_LEVEL_FOR_HEROIC_CHARACTER] = GetConfigSettings::GetIntState("CharacterCreating.MinLevelForHeroicCharacter", 55);

    m_int_configs[CONFIG_SKIP_CINEMATICS] = GetConfigSettings::GetIntState("SkipCinematics", 0);
    if (int32(m_int_configs[CONFIG_SKIP_CINEMATICS]) < 0 || m_int_configs[CONFIG_SKIP_CINEMATICS] > 2)
    {
        sLog->OutErrorConsole("SkipCinematics (%i) must be in range 0..2. Set to 0.", m_int_configs[CONFIG_SKIP_CINEMATICS]);
        m_int_configs[CONFIG_SKIP_CINEMATICS] = 0;
    }

    if (reload)
    {
        uint32 val = GetConfigSettings::GetIntState("MaxPlayerLevel", DEFAULT_MAX_LEVEL);
        if (val != m_int_configs[CONFIG_MAX_PLAYER_LEVEL])
            sLog->OutErrorConsole("MaxPlayerLevel option can't be changed at config reload, using current value (%u).", m_int_configs[CONFIG_MAX_PLAYER_LEVEL]);
    }
    else
        m_int_configs[CONFIG_MAX_PLAYER_LEVEL] = GetConfigSettings::GetIntState("MaxPlayerLevel", DEFAULT_MAX_LEVEL);

    if (m_int_configs[CONFIG_MAX_PLAYER_LEVEL] > MAX_LEVEL)
    {
        sLog->OutErrorConsole("MaxPlayerLevel (%i) must be in range 1..%u. Set to %u.", m_int_configs[CONFIG_MAX_PLAYER_LEVEL], MAX_LEVEL, MAX_LEVEL);
        m_int_configs[CONFIG_MAX_PLAYER_LEVEL] = MAX_LEVEL;
    }

    m_int_configs[CONFIG_MIN_DUALSPEC_LEVEL] = GetConfigSettings::GetIntState("MinDualSpecLevel", 40);

    m_int_configs[CONFIG_START_PLAYER_LEVEL] = GetConfigSettings::GetIntState("StartPlayerLevel", 1);
    if (m_int_configs[CONFIG_START_PLAYER_LEVEL] < 1)
    {
        sLog->OutErrorConsole("StartPlayerLevel (%i) must be in range 1..MaxPlayerLevel(%u). Set to 1.", m_int_configs[CONFIG_START_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL]);
        m_int_configs[CONFIG_START_PLAYER_LEVEL] = 1;
    }
    else if (m_int_configs[CONFIG_START_PLAYER_LEVEL] > m_int_configs[CONFIG_MAX_PLAYER_LEVEL])
    {
        sLog->OutErrorConsole("StartPlayerLevel (%i) must be in range 1..MaxPlayerLevel(%u). Set to %u.", m_int_configs[CONFIG_START_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL]);
        m_int_configs[CONFIG_START_PLAYER_LEVEL] = m_int_configs[CONFIG_MAX_PLAYER_LEVEL];
    }

    m_int_configs[CONFIG_START_HEROIC_PLAYER_LEVEL] = GetConfigSettings::GetIntState("StartDeathKnightLevel", 55);
    if (m_int_configs[CONFIG_START_HEROIC_PLAYER_LEVEL] < 1)
    {
        sLog->OutErrorConsole("StartDeathKnightLevel (%i) must be in range 1..MaxPlayerLevel(%u). Set to 55.",
            m_int_configs[CONFIG_START_HEROIC_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL]);
        m_int_configs[CONFIG_START_HEROIC_PLAYER_LEVEL] = 55;
    }
    else if (m_int_configs[CONFIG_START_HEROIC_PLAYER_LEVEL] > m_int_configs[CONFIG_MAX_PLAYER_LEVEL])
    {
        sLog->OutErrorConsole("StartDeathKnightLevel (%i) must be in range 1..MaxPlayerLevel(%u). Set to %u.",
            m_int_configs[CONFIG_START_HEROIC_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL]);
        m_int_configs[CONFIG_START_HEROIC_PLAYER_LEVEL] = m_int_configs[CONFIG_MAX_PLAYER_LEVEL];
    }

    m_int_configs[CONFIG_START_PLAYER_MONEY] = GetConfigSettings::GetIntState("StartPlayerMoney", 0);
    if (int32(m_int_configs[CONFIG_START_PLAYER_MONEY]) < 0)
    {
        sLog->OutErrorConsole("StartPlayerMoney (%i) must be in range 0..%u. Set to %u.", m_int_configs[CONFIG_START_PLAYER_MONEY], MAX_MONEY_AMOUNT, 0);
            m_int_configs[CONFIG_START_PLAYER_MONEY] = 0;
    }
    else if (m_int_configs[CONFIG_START_PLAYER_MONEY] > MAX_MONEY_AMOUNT)
    {
        sLog->OutErrorConsole("StartPlayerMoney (%i) must be in range 0..%u. Set to %u.",
            m_int_configs[CONFIG_START_PLAYER_MONEY], MAX_MONEY_AMOUNT, MAX_MONEY_AMOUNT);
        m_int_configs[CONFIG_START_PLAYER_MONEY] = MAX_MONEY_AMOUNT;
    }

    m_int_configs[CONFIG_MAX_HONOR_POINTS] = GetConfigSettings::GetIntState("MaxHonorPoints", 75000);
    if (int32(m_int_configs[CONFIG_MAX_HONOR_POINTS]) < 0)
    {
        sLog->OutErrorConsole("MaxHonorPoints (%i) can't be negative. Set to 0.", m_int_configs[CONFIG_MAX_HONOR_POINTS]);
        m_int_configs[CONFIG_MAX_HONOR_POINTS] = 0;
    }

    m_int_configs[CONFIG_START_HONOR_POINTS] = GetConfigSettings::GetIntState("StartHonorPoints", 0);
    if (int32(m_int_configs[CONFIG_START_HONOR_POINTS]) < 0)
    {
        sLog->OutErrorConsole("StartHonorPoints (%i) must be in range 0..MaxHonorPoints(%u). Set to %u.",
            m_int_configs[CONFIG_START_HONOR_POINTS], m_int_configs[CONFIG_MAX_HONOR_POINTS], 0);
        m_int_configs[CONFIG_START_HONOR_POINTS] = 0;
    }
    else if (m_int_configs[CONFIG_START_HONOR_POINTS] > m_int_configs[CONFIG_MAX_HONOR_POINTS])
    {
        sLog->OutErrorConsole("StartHonorPoints (%i) must be in range 0..MaxHonorPoints(%u). Set to %u.",
            m_int_configs[CONFIG_START_HONOR_POINTS], m_int_configs[CONFIG_MAX_HONOR_POINTS], m_int_configs[CONFIG_MAX_HONOR_POINTS]);
        m_int_configs[CONFIG_START_HONOR_POINTS] = m_int_configs[CONFIG_MAX_HONOR_POINTS];
    }

    m_int_configs[CONFIG_MAX_ARENA_POINTS] = GetConfigSettings::GetIntState("MaxArenaPoints", 10000);
    if (int32(m_int_configs[CONFIG_MAX_ARENA_POINTS]) < 0)
    {
        sLog->OutErrorConsole("MaxArenaPoints (%i) can't be negative. Set to 0.", m_int_configs[CONFIG_MAX_ARENA_POINTS]);
        m_int_configs[CONFIG_MAX_ARENA_POINTS] = 0;
    }

    m_int_configs[CONFIG_START_ARENA_POINTS] = GetConfigSettings::GetIntState("StartArenaPoints", 0);
    if (int32(m_int_configs[CONFIG_START_ARENA_POINTS]) < 0)
    {
        sLog->OutErrorConsole("StartArenaPoints (%i) must be in range 0..MaxArenaPoints(%u). Set to %u.",
            m_int_configs[CONFIG_START_ARENA_POINTS], m_int_configs[CONFIG_MAX_ARENA_POINTS], 0);
        m_int_configs[CONFIG_START_ARENA_POINTS] = 0;
    }
    else if (m_int_configs[CONFIG_START_ARENA_POINTS] > m_int_configs[CONFIG_MAX_ARENA_POINTS])
    {
        sLog->OutErrorConsole("StartArenaPoints (%i) must be in range 0..MaxArenaPoints(%u). Set to %u.",
            m_int_configs[CONFIG_START_ARENA_POINTS], m_int_configs[CONFIG_MAX_ARENA_POINTS], m_int_configs[CONFIG_MAX_ARENA_POINTS]);
        m_int_configs[CONFIG_START_ARENA_POINTS] = m_int_configs[CONFIG_MAX_ARENA_POINTS];
    }

    m_int_configs[CONFIG_MAX_RECRUIT_A_FRIEND_BONUS_PLAYER_LEVEL] = GetConfigSettings::GetIntState("RecruitAFriend.MaxLevel", 60);
    if (m_int_configs[CONFIG_MAX_RECRUIT_A_FRIEND_BONUS_PLAYER_LEVEL] > m_int_configs[CONFIG_MAX_PLAYER_LEVEL])
    {
        sLog->OutErrorConsole("RecruitAFriend.MaxLevel (%i) must be in the range 0..MaxLevel(%u). Set to %u.",
            m_int_configs[CONFIG_MAX_RECRUIT_A_FRIEND_BONUS_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL], 60);
        m_int_configs[CONFIG_MAX_RECRUIT_A_FRIEND_BONUS_PLAYER_LEVEL] = 60;
    }

    m_int_configs[CONFIG_MAX_RECRUIT_A_FRIEND_BONUS_PLAYER_LEVEL_DIFFERENCE] = GetConfigSettings::GetIntState("RecruitAFriend.MaxDifference", 4);
    m_bool_configs[CONFIG_ALL_TAXI_PATHS] = GetConfigSettings::GetBoolState("AllFlightPaths", false);
    m_bool_configs[CONFIG_INSTANT_TAXI] = GetConfigSettings::GetBoolState("InstantFlightPaths", false);

    m_bool_configs[CONFIG_INSTANCE_IGNORE_LEVEL] = GetConfigSettings::GetBoolState("Instance.IgnoreLevel", false);
    m_bool_configs[CONFIG_INSTANCE_IGNORE_RAID] = GetConfigSettings::GetBoolState("Instance.IgnoreRaid", false);

    m_bool_configs[CONFIG_CAST_UNSTUCK] = GetConfigSettings::GetBoolState("CastUnstuck", true);
    m_int_configs[CONFIG_INSTANCE_RESET_TIME_HOUR] = GetConfigSettings::GetIntState("Instance.ResetTimeHour", 4);
    m_int_configs[CONFIG_INSTANCE_UNLOAD_DELAY] = GetConfigSettings::GetIntState("Instance.UnloadDelay", 30 * MINUTE * IN_MILLISECONDS);

    m_int_configs[CONFIG_MAX_PRIMARY_TRADE_SKILL] = GetConfigSettings::GetIntState("MaxPrimaryTradeSkill", 2);
    m_int_configs[CONFIG_MIN_PETITION_SIGNS] = GetConfigSettings::GetIntState("MinPetitionSigns", 9);
    if (m_int_configs[CONFIG_MIN_PETITION_SIGNS] > 9)
    {
        sLog->OutErrorConsole("MinPetitionSigns (%i) must be in range 0..9. Set to 9.", m_int_configs[CONFIG_MIN_PETITION_SIGNS]);
        m_int_configs[CONFIG_MIN_PETITION_SIGNS] = 9;
    }
    rate_values[RATE_PVP_RANK_EXTRA_HONOR] = GetConfigSettings::GetFloatDefault("PvP.Rank.Rate.Extra.Honor", 1);
    std::string s_pvp_ranks = GetConfigSettings::GetStringDefault("PvP.Rank.HK.Per.Rank", "10,50,100,200,450,750,1300,2000,3500,6000,9500,15000,21000,30000");
    char *c_pvp_ranks = const_cast<char*>(s_pvp_ranks.c_str());
    for (int i = 0; i !=HKRANKMAX; i++)
    {
        if (i == 0)
            pvp_ranks[0] = 0;
        else if (i == 1)
            pvp_ranks[1] = atoi(strtok (c_pvp_ranks, ","));
        else
            pvp_ranks[i] = atoi(strtok (NULL, ","));
    }

    m_int_configs[CONFIG_GM_LOGIN_STATE] = GetConfigSettings::GetIntState("GM.LoginState", 2);
    m_int_configs[CONFIG_GM_VISIBLE_STATE] = GetConfigSettings::GetIntState("GM.Visible", 2);
    m_int_configs[CONFIG_GM_CHAT] = GetConfigSettings::GetIntState("GM.Chat", 2);
    m_int_configs[CONFIG_GM_WHISPERING_TO] = GetConfigSettings::GetIntState("GM.WhisperingTo", 2);
	m_int_configs[CONFIG_GM_FREEZE_DURATION] = GetConfigSettings::GetIntState("GM.FreezeAuraDuration", 0);

    m_int_configs[CONFIG_GM_LEVEL_IN_GM_LIST] = GetConfigSettings::GetIntState("GM.InGMList.Level", SEC_ADMINISTRATOR);
    m_int_configs[CONFIG_GM_LEVEL_IN_WHO_LIST] = GetConfigSettings::GetIntState("GM.InWhoList.Level", SEC_ADMINISTRATOR);
    m_int_configs[CONFIG_GM_LEVEL_ALLOW_ACHIEVEMENTS] = GetConfigSettings::GetIntState("GM.AllowAchievementGain.Level", SEC_PLAYER);
    m_bool_configs[CONFIG_GM_LOG_TRADE] = GetConfigSettings::GetBoolState("GM.LogTrade", false);
    m_int_configs[CONFIG_START_GM_LEVEL] = GetConfigSettings::GetIntState("GM.StartLevel", 1);
    if (m_int_configs[CONFIG_START_GM_LEVEL] < m_int_configs[CONFIG_START_PLAYER_LEVEL])
    {
        sLog->OutErrorConsole("GM.StartLevel (%i) must be in range StartPlayerLevel(%u)..%u. Set to %u.",
            m_int_configs[CONFIG_START_GM_LEVEL], m_int_configs[CONFIG_START_PLAYER_LEVEL], MAX_LEVEL, m_int_configs[CONFIG_START_PLAYER_LEVEL]);
        m_int_configs[CONFIG_START_GM_LEVEL] = m_int_configs[CONFIG_START_PLAYER_LEVEL];
    }
    else if (m_int_configs[CONFIG_START_GM_LEVEL] > MAX_LEVEL)
    {
        sLog->OutErrorConsole("GM.StartLevel (%i) must be in range 1..%u. Set to %u.", m_int_configs[CONFIG_START_GM_LEVEL], MAX_LEVEL, MAX_LEVEL);
        m_int_configs[CONFIG_START_GM_LEVEL] = MAX_LEVEL;
    }
    m_bool_configs[CONFIG_ALLOW_GM_GROUP] = GetConfigSettings::GetBoolState("GM.AllowInvite", false);
    m_bool_configs[CONFIG_ALLOW_GM_FRIEND] = GetConfigSettings::GetBoolState("GM.AllowFriend", false);
    m_bool_configs[CONFIG_GM_LOWER_SECURITY] = GetConfigSettings::GetBoolState("GM.LowerSecurity", false);
    m_float_configs[CONFIG_CHANCE_OF_GM_SURVEY] = GetConfigSettings::GetFloatDefault("GM.TicketSystem.ChanceOfGMSurvey", 50.0f);

    m_int_configs[CONFIG_GROUP_VISIBILITY] = GetConfigSettings::GetIntState("Visibility.GroupMode", 1);

    m_int_configs[CONFIG_MAIL_DELIVERY_DELAY] = GetConfigSettings::GetIntState("MailDeliveryDelay", HOUR);

    m_int_configs[CONFIG_UPTIME_UPDATE] = GetConfigSettings::GetIntState("UpdateUptimeInterval", 10);

    if (int32(m_int_configs[CONFIG_UPTIME_UPDATE]) <= 0)
    {
        sLog->OutErrorConsole("UpdateUptimeInterval (%i) must be > 0, set to default 10.", m_int_configs[CONFIG_UPTIME_UPDATE]);
        m_int_configs[CONFIG_UPTIME_UPDATE] = 10;
    }
    if (reload)
    {
        m_timers[WUPDATE_UPTIME].SetInterval(m_int_configs[CONFIG_UPTIME_UPDATE]*MINUTE*IN_MILLISECONDS);
        m_timers[WUPDATE_UPTIME].Reset();
    }

    // log db cleanup interval
    m_int_configs[CONFIG_LOGDB_CLEARINTERVAL] = GetConfigSettings::GetIntState("LogDB.Opt.ClearInterval", 10);
    if (int32(m_int_configs[CONFIG_LOGDB_CLEARINTERVAL]) <= 0)
    {
        sLog->OutErrorConsole("LogDB.Opt.ClearInterval (%i) must be > 0, set to default 10.", m_int_configs[CONFIG_LOGDB_CLEARINTERVAL]);
        m_int_configs[CONFIG_LOGDB_CLEARINTERVAL] = 10;
    }
    if (reload)
    {
        m_timers[WUPDATE_CLEANDB].SetInterval(m_int_configs[CONFIG_LOGDB_CLEARINTERVAL] * MINUTE * IN_MILLISECONDS);
        m_timers[WUPDATE_CLEANDB].Reset();
    }
    m_int_configs[CONFIG_LOGDB_CLEARTIME] = GetConfigSettings::GetIntState("LogDB.Opt.ClearTime", 1209600); // 14 days default
    sLog->OutConsole("Will clear `logs` table of entries older than %i seconds every %u minutes.",
        m_int_configs[CONFIG_LOGDB_CLEARTIME], m_int_configs[CONFIG_LOGDB_CLEARINTERVAL]);

    m_int_configs[CONFIG_SKILL_CHANCE_ORANGE] = GetConfigSettings::GetIntState("SkillChance.Orange", 100);
    m_int_configs[CONFIG_SKILL_CHANCE_YELLOW] = GetConfigSettings::GetIntState("SkillChance.Yellow", 75);
    m_int_configs[CONFIG_SKILL_CHANCE_GREEN] = GetConfigSettings::GetIntState("SkillChance.Green", 25);
    m_int_configs[CONFIG_SKILL_CHANCE_GREY] = GetConfigSettings::GetIntState("SkillChance.Grey", 0);

    m_int_configs[CONFIG_SKILL_CHANCE_MINING_STEPS] = GetConfigSettings::GetIntState("SkillChance.MiningSteps", 75);
    m_int_configs[CONFIG_SKILL_CHANCE_SKINNING_STEPS] = GetConfigSettings::GetIntState("SkillChance.SkinningSteps", 75);

    m_bool_configs[CONFIG_SKILL_PROSPECTING] = GetConfigSettings::GetBoolState("SkillChance.Prospecting", false);
    m_bool_configs[CONFIG_SKILL_MILLING] = GetConfigSettings::GetBoolState("SkillChance.Milling", false);

    m_int_configs[CONFIG_SKILL_GAIN_CRAFTING] = GetConfigSettings::GetIntState("SkillGain.Crafting", 1);

    m_int_configs[CONFIG_SKILL_GAIN_DEFENSE] = GetConfigSettings::GetIntState("SkillGain.Defense", 1);

    m_int_configs[CONFIG_SKILL_GAIN_GATHERING] = GetConfigSettings::GetIntState("SkillGain.Gathering", 1);

    m_int_configs[CONFIG_SKILL_GAIN_WEAPON] = GetConfigSettings::GetIntState("SkillGain.Weapon", 1);

    m_int_configs[CONFIG_MAX_OVERSPEED_PINGS] = GetConfigSettings::GetIntState("MaxOverspeedPings", 2);
    if (m_int_configs[CONFIG_MAX_OVERSPEED_PINGS] != 0 && m_int_configs[CONFIG_MAX_OVERSPEED_PINGS] < 2)
    {
        sLog->OutErrorConsole("MaxOverspeedPings (%i) must be in range 2..infinity (or 0 to disable check). Set to 2.", m_int_configs[CONFIG_MAX_OVERSPEED_PINGS]);
        m_int_configs[CONFIG_MAX_OVERSPEED_PINGS] = 2;
    }

    m_bool_configs[CONFIG_SAVE_RESPAWN_TIME_IMMEDIATELY] = GetConfigSettings::GetBoolState("SaveRespawnTimeImmediately", true);

	if (!m_bool_configs[CONFIG_SAVE_RESPAWN_TIME_IMMEDIATELY])
	{
		sLog->OutErrorConsole("SaveRespawnTimeImmediately triggers assertions when disabled, overridden to Enabled");
		m_bool_configs[CONFIG_SAVE_RESPAWN_TIME_IMMEDIATELY] = true;
	}

    m_bool_configs[CONFIG_WEATHER] = GetConfigSettings::GetBoolState("ActivateWeather", true);

    m_int_configs[CONFIG_DISABLE_BREATHING] = GetConfigSettings::GetIntState("DisableWaterBreath", SEC_MAIN_ADMINISTRATOR);

    m_bool_configs[CONFIG_ALWAYS_MAX_SKILL_FOR_LEVEL] = GetConfigSettings::GetBoolState("AlwaysMaxSkillForLevel", false);

    if (reload)
    {
        uint32 val = GetConfigSettings::GetIntState("Expansion", 2);
        if (val != m_int_configs[CONFIG_EXPANSION])
            sLog->OutErrorConsole("Expansion option can't be changed at worldserver.conf reload, using current value (%u).", m_int_configs[CONFIG_EXPANSION]);
    }
    else
        m_int_configs[CONFIG_EXPANSION] = GetConfigSettings::GetIntState("Expansion", 2);

    m_int_configs[CONFIG_CHATFLOOD_MESSAGE_COUNT] = GetConfigSettings::GetIntState("ChatFlood.MessageCount", 10);
    m_int_configs[CONFIG_CHATFLOOD_MESSAGE_DELAY] = GetConfigSettings::GetIntState("ChatFlood.MessageDelay", 1);
    m_int_configs[CONFIG_CHATFLOOD_MUTE_TIME] = GetConfigSettings::GetIntState("ChatFlood.MuteTime", 10);

	m_bool_configs[CONFIG_EVENT_ANNOUNCE] = GetConfigSettings::GetIntState("Event.Announce", false);

    m_float_configs[CONFIG_CREATURE_FAMILY_FLEE_ASSISTANCE_RADIUS] = GetConfigSettings::GetFloatDefault("CreatureFamilyFleeAssistanceRadius", 30.0f);
    m_float_configs[CONFIG_CREATURE_FAMILY_ASSISTANCE_RADIUS] = GetConfigSettings::GetFloatDefault("CreatureFamilyAssistanceRadius", 10.0f);
    m_int_configs[CONFIG_CREATURE_FAMILY_ASSISTANCE_DELAY] = GetConfigSettings::GetIntState("CreatureFamilyAssistanceDelay", 1500);
    m_int_configs[CONFIG_CREATURE_FAMILY_FLEE_DELAY] = GetConfigSettings::GetIntState("CreatureFamilyFleeDelay", 7000);

    m_int_configs[CONFIG_WORLD_BOSS_LEVEL_DIFF] = GetConfigSettings::GetIntState("WorldBossLevelDiff", 3);

	m_bool_configs[CONFIG_QUEST_ENABLE_QUEST_TRACKER] = GetConfigSettings::GetBoolState("Quests.EnableQuestTracker", false);

    // note: disable value (-1) will assigned as 0xFFFFFFF, to prevent overflow at calculations limit it to max possible player level MAX_LEVEL(100)
    m_int_configs[CONFIG_QUEST_LOW_LEVEL_HIDE_DIFF] = GetConfigSettings::GetIntState("Quests.LowLevelHideDiff", 4);
    if (m_int_configs[CONFIG_QUEST_LOW_LEVEL_HIDE_DIFF] > MAX_LEVEL)
        m_int_configs[CONFIG_QUEST_LOW_LEVEL_HIDE_DIFF] = MAX_LEVEL;
    m_int_configs[CONFIG_QUEST_HIGH_LEVEL_HIDE_DIFF] = GetConfigSettings::GetIntState("Quests.HighLevelHideDiff", 7);
    if (m_int_configs[CONFIG_QUEST_HIGH_LEVEL_HIDE_DIFF] > MAX_LEVEL)
        m_int_configs[CONFIG_QUEST_HIGH_LEVEL_HIDE_DIFF] = MAX_LEVEL;
    m_bool_configs[CONFIG_QUEST_IGNORE_RAID] = GetConfigSettings::GetBoolState("Quests.IgnoreRaid", false);
	m_bool_configs[CONFIG_QUEST_IGNORE_AUTO_ACCEPT] = GetConfigSettings::GetBoolState("Quests.IgnoreAutoAccept", false);
	m_bool_configs[CONFIG_QUEST_IGNORE_AUTO_COMPLETE] = GetConfigSettings::GetBoolState("Quests.IgnoreAutoComplete", false);

    m_int_configs[CONFIG_RANDOM_BG_RESET_HOUR] = GetConfigSettings::GetIntState("Battleground.Random.ResetHour", 6);
    if (m_int_configs[CONFIG_RANDOM_BG_RESET_HOUR] > 23)
    {
        sLog->OutErrorConsole("Battleground.Random.ResetHour (%i) can't be load. Set to 6.", m_int_configs[CONFIG_RANDOM_BG_RESET_HOUR]);
        m_int_configs[CONFIG_RANDOM_BG_RESET_HOUR] = 6;
    }

	m_int_configs[CONFIG_GUILD_RESET_HOUR] = GetConfigSettings::GetIntState("Guild.ResetHour", 6);
	if (m_int_configs[CONFIG_GUILD_RESET_HOUR] > 23)
	{
		sLog->OutErrorConsole("Guild.ResetHour (%i) can't be load. Set to 6.", m_int_configs[CONFIG_GUILD_RESET_HOUR]);
		m_int_configs[CONFIG_GUILD_RESET_HOUR] = 6;
	}

    m_bool_configs[CONFIG_DETECT_POS_COLLISION] = GetConfigSettings::GetBoolState("DetectPosCollision", true);

    m_bool_configs[CONFIG_RESTRICTED_LFG_CHANNEL] = GetConfigSettings::GetBoolState("Channel.RestrictedLfg", true);
    m_bool_configs[CONFIG_SILENTLY_GM_JOIN_TO_CHANNEL] = GetConfigSettings::GetBoolState("Channel.SilentlyGMJoin", false);

    m_bool_configs[CONFIG_TALENTS_INSPECTING] = GetConfigSettings::GetBoolState("TalentsInspecting", true);
    m_bool_configs[CONFIG_CHAT_FAKE_MESSAGE_PREVENTING] = GetConfigSettings::GetBoolState("ChatFakeMessagePreventing", false);
    m_int_configs[CONFIG_CHAT_STRICT_LINK_CHECKING_SEVERITY] = GetConfigSettings::GetIntState("ChatStrictLinkChecking.Severity", 0);
    m_int_configs[CONFIG_CHAT_STRICT_LINK_CHECKING_KICK] = GetConfigSettings::GetIntState("ChatStrictLinkChecking.Kick", 0);

    m_int_configs[CONFIG_CORPSE_DECAY_NORMAL] = GetConfigSettings::GetIntState("Corpse.Decay.NORMAL", 60);
    m_int_configs[CONFIG_CORPSE_DECAY_RARE] = GetConfigSettings::GetIntState("Corpse.Decay.RARE", 300);
    m_int_configs[CONFIG_CORPSE_DECAY_ELITE] = GetConfigSettings::GetIntState("Corpse.Decay.ELITE", 300);
    m_int_configs[CONFIG_CORPSE_DECAY_RAREELITE] = GetConfigSettings::GetIntState("Corpse.Decay.RAREELITE", 300);
    m_int_configs[CONFIG_CORPSE_DECAY_WORLDBOSS] = GetConfigSettings::GetIntState("Corpse.Decay.WORLDBOSS", 3600);

    m_int_configs[CONFIG_DEATH_SICKNESS_LEVEL] = GetConfigSettings::GetIntState("Death.SicknessLevel", 11);
    m_bool_configs[CONFIG_DEATH_CORPSE_RECLAIM_DELAY_PVP] = GetConfigSettings::GetBoolState("Death.CorpseReclaimDelay.PvP", true);
    m_bool_configs[CONFIG_DEATH_CORPSE_RECLAIM_DELAY_PVE] = GetConfigSettings::GetBoolState("Death.CorpseReclaimDelay.PvE", true);
    m_bool_configs[CONFIG_DEATH_BONES_WORLD] = GetConfigSettings::GetBoolState("Death.Bones.World", true);
    m_bool_configs[CONFIG_DEATH_BONES_BG_OR_ARENA] = GetConfigSettings::GetBoolState("Death.Bones.BattlegroundOrArena", true);

    m_bool_configs[CONFIG_DIE_COMMAND_MODE] = GetConfigSettings::GetBoolState("Die.Command.Mode", true);

    m_float_configs[CONFIG_THREAT_RADIUS] = GetConfigSettings::GetFloatDefault("ThreatRadius", 60.0f);

    // always use declined names in the russian client
    m_bool_configs[CONFIG_DECLINED_NAMES_USED] =
        (m_int_configs[CONFIG_REALM_ZONE] == REALM_ZONE_RUSSIAN) ? true : GetConfigSettings::GetBoolState("DeclinedNames", false);

    m_float_configs[CONFIG_LISTEN_RANGE_SAY] = GetConfigSettings::GetFloatDefault("ListenRange.Say", 25.0f);
    m_float_configs[CONFIG_LISTEN_RANGE_TEXT_EMOTE] = GetConfigSettings::GetFloatDefault("ListenRange.TextEmote", 25.0f);
    m_float_configs[CONFIG_LISTEN_RANGE_YELL] = GetConfigSettings::GetFloatDefault("ListenRange.Yell", 300.0f);

    m_bool_configs[CONFIG_BATTLEGROUND_CAST_DESERTER]               = GetConfigSettings::GetBoolState("Battleground.CastDeserter", true);
    m_bool_configs[CONFIG_BATTLEGROUND_QUEUE_ANNOUNCER_ENABLE]      = GetConfigSettings::GetBoolState("Battleground.QueueAnnouncer.Enable", false);
    m_bool_configs[CONFIG_BATTLEGROUND_QUEUE_ANNOUNCER_PLAYERONLY]  = GetConfigSettings::GetBoolState("Battleground.QueueAnnouncer.PlayerOnly", false);
	m_bool_configs[CONFIG_BATTLEGROUND_STORE_STATISTICS_ENABLE]     = GetConfigSettings::GetBoolState("Battleground.StoreStatistics.Enable", false);
	m_bool_configs[CONFIG_BATTLEGROUND_TRACK_DESERTERS]             = GetConfigSettings::GetBoolState("Battleground.TrackDeserters.Enable", false);
    m_int_configs[CONFIG_BATTLEGROUND_INVITATION_TYPE]              = GetConfigSettings::GetIntState("Battleground.InvitationType", 0);
    m_int_configs[CONFIG_BATTLEGROUND_PREMATURE_FINISH_TIMER]       = GetConfigSettings::GetIntState("Battleground.PrematureFinishTimer", 5 * MINUTE * IN_MILLISECONDS);
    m_int_configs[CONFIG_BATTLEGROUND_PREMADE_GROUP_WAIT_FOR_MATCH] = GetConfigSettings::GetIntState("Battleground.PremadeGroupWaitForMatch", 30 * MINUTE * IN_MILLISECONDS);
    m_bool_configs[CONFIG_BG_XP_FOR_KILL]                           = GetConfigSettings::GetBoolState("Battleground.GiveXPForKills", false);
    m_int_configs[CONFIG_ARENA_MAX_RATING_DIFFERENCE]               = GetConfigSettings::GetIntState("Arena.MaxRatingDifference", 150);
    m_int_configs[CONFIG_ARENA_RATING_DISCARD_TIMER]                = GetConfigSettings::GetIntState("Arena.RatingDiscardTimer", 10 * MINUTE * IN_MILLISECONDS);
	m_int_configs[CONFIG_ARENA_RATED_UPDATE_TIMER]                  = GetConfigSettings::GetIntState("Arena.RatedUpdateTimer", 5 * IN_MILLISECONDS);
    m_bool_configs[CONFIG_ARENA_AUTO_DISTRIBUTE_POINTS]             = GetConfigSettings::GetBoolState("Arena.AutoDistributePoints", false);
    m_int_configs[CONFIG_ARENA_AUTO_DISTRIBUTE_INTERVAL_DAYS]       = GetConfigSettings::GetIntState("Arena.AutoDistributeInterval", 7);
    m_bool_configs[CONFIG_ARENA_QUEUE_ANNOUNCER_ENABLE]             = GetConfigSettings::GetBoolState("Arena.QueueAnnouncer.Enable", false);
    m_bool_configs[CONFIG_ARENA_QUEUE_ANNOUNCER_PLAYERONLY]         = GetConfigSettings::GetBoolState("Arena.QueueAnnouncer.PlayerOnly", false);
	m_int_configs[CONFIG_ARENA_SEASON_ID]                           = GetConfigSettings::GetIntState("Arena.ArenaSeason.ID", 1);
    m_int_configs[CONFIG_ARENA_START_RATING]                        = GetConfigSettings::GetIntState("Arena.ArenaStartRating", 0);
    m_int_configs[CONFIG_ARENA_START_PERSONAL_RATING]               = GetConfigSettings::GetIntState("Arena.ArenaStartPersonalRating", 1000);
    m_int_configs[CONFIG_ARENA_START_MATCHMAKER_RATING]             = GetConfigSettings::GetIntState("Arena.ArenaStartMatchmakerRating", 1500);
	m_bool_configs[CONFIG_ARENA_SEASON_IN_PROGRESS]                 = GetConfigSettings::GetBoolState("Arena.ArenaSeason.InProgress", true);
    m_bool_configs[CONFIG_ARENA_LOG_EXTENDED_INFO]                  = GetConfigSettings::GetBoolState("ArenaLog.ExtendedInfo", false);

    m_bool_configs[CONFIG_OFF_HAND_CHECK_AT_SPELL_UNLEARN]          = GetConfigSettings::GetBoolState("OffhandCheckAtSpellUnlearn", true);

    if (int32 clientCacheId = GetConfigSettings::GetIntState("ClientCacheVersion", 0))
    {
        // overwrite DB/old value
        if (clientCacheId > 0)
        {
            m_int_configs[CONFIG_CLIENTCACHE_VERSION] = clientCacheId;
            sLog->OutConsole("Client cache version set to: %u", clientCacheId);
        }
        else
            sLog->OutErrorConsole("ClientCacheVersion can't be negative %d, ignored.", clientCacheId);
    }

    m_int_configs[CONFIG_INSTANT_LOGOUT] = GetConfigSettings::GetIntState("InstantLogout", SEC_MODERATOR);

    m_int_configs[CONFIG_GUILD_EVENT_LOG_COUNT] = GetConfigSettings::GetIntState("Guild.EventLogRecordsCount", GUILD_EVENTLOG_MAX_RECORDS);
    if (m_int_configs[CONFIG_GUILD_EVENT_LOG_COUNT] > GUILD_EVENTLOG_MAX_RECORDS)
	{
        m_int_configs[CONFIG_GUILD_EVENT_LOG_COUNT] = GUILD_EVENTLOG_MAX_RECORDS;
	}
    m_int_configs[CONFIG_GUILD_BANK_EVENT_LOG_COUNT] = GetConfigSettings::GetIntState("Guild.BankEventLogRecordsCount", GUILD_BANKLOG_MAX_RECORDS);
    if (m_int_configs[CONFIG_GUILD_BANK_EVENT_LOG_COUNT] > GUILD_BANKLOG_MAX_RECORDS)
	{
        m_int_configs[CONFIG_GUILD_BANK_EVENT_LOG_COUNT] = GUILD_BANKLOG_MAX_RECORDS;
	}

    //visibility on continents
    m_MaxVisibleDistanceOnContinents = GetConfigSettings::GetFloatDefault("Visibility.Distance.Continents", DEFAULT_VISIBILITY_DISTANCE);
    if (m_MaxVisibleDistanceOnContinents < 45*sWorld->getRate(RATE_CREATURE_AGGRO))
    {
        sLog->OutErrorConsole("Visibility.Distance.Continents can't be less max aggro radius %f", 45*sWorld->getRate(RATE_CREATURE_AGGRO));
        m_MaxVisibleDistanceOnContinents = 45*sWorld->getRate(RATE_CREATURE_AGGRO);
    }
    else if (m_MaxVisibleDistanceOnContinents > MAX_VISIBILITY_DISTANCE)
    {
        sLog->OutErrorConsole("Visibility.Distance.Continents can't be greater %f", MAX_VISIBILITY_DISTANCE);
        m_MaxVisibleDistanceOnContinents = MAX_VISIBILITY_DISTANCE;
    }

    //visibility in instances
    m_MaxVisibleDistanceInInstances = GetConfigSettings::GetFloatDefault("Visibility.Distance.Instances", DEFAULT_VISIBILITY_INSTANCE);
    if (m_MaxVisibleDistanceInInstances < 45*sWorld->getRate(RATE_CREATURE_AGGRO))
    {
        sLog->OutErrorConsole("Visibility.Distance.Instances can't be less max aggro radius %f", 45*sWorld->getRate(RATE_CREATURE_AGGRO));
        m_MaxVisibleDistanceInInstances = 45*sWorld->getRate(RATE_CREATURE_AGGRO);
    }
    else if (m_MaxVisibleDistanceInInstances > MAX_VISIBILITY_DISTANCE)
    {
        sLog->OutErrorConsole("Visibility.Distance.Instances can't be greater %f", MAX_VISIBILITY_DISTANCE);
        m_MaxVisibleDistanceInInstances = MAX_VISIBILITY_DISTANCE;
    }

    //visibility in BG/Arenas
    m_MaxVisibleDistanceInBGArenas = GetConfigSettings::GetFloatDefault("Visibility.Distance.BGArenas", DEFAULT_VISIBILITY_BGARENAS);
    if (m_MaxVisibleDistanceInBGArenas < 45*sWorld->getRate(RATE_CREATURE_AGGRO))
    {
        sLog->OutErrorConsole("Visibility.Distance.BGArenas can't be less max aggro radius %f", 45*sWorld->getRate(RATE_CREATURE_AGGRO));
        m_MaxVisibleDistanceInBGArenas = 45*sWorld->getRate(RATE_CREATURE_AGGRO);
    }
    else if (m_MaxVisibleDistanceInBGArenas > MAX_VISIBILITY_DISTANCE)
    {
        sLog->OutErrorConsole("Visibility.Distance.BGArenas can't be greater %f", MAX_VISIBILITY_DISTANCE);
        m_MaxVisibleDistanceInBGArenas = MAX_VISIBILITY_DISTANCE;
    }

    m_visibility_notify_periodOnContinents = GetConfigSettings::GetIntState("Visibility.Notify.Period.OnContinents", DEFAULT_VISIBILITY_NOTIFY_PERIOD);
    m_visibility_notify_periodInInstances = GetConfigSettings::GetIntState("Visibility.Notify.Period.InInstances", DEFAULT_VISIBILITY_NOTIFY_PERIOD);
    m_visibility_notify_periodInBGArenas = GetConfigSettings::GetIntState("Visibility.Notify.Period.InBGArenas", DEFAULT_VISIBILITY_NOTIFY_PERIOD);

    ///- Load the CharDelete related config options
    m_int_configs[CONFIG_CHAR_DELETE_METHOD] = GetConfigSettings::GetIntState("CharDelete.Method", 0);
    m_int_configs[CONFIG_CHAR_DELETE_MIN_LEVEL] = GetConfigSettings::GetIntState("CharDelete.MinLevel", 0);
    m_int_configs[CONFIG_CHAR_DELETE_KEEP_DAYS] = GetConfigSettings::GetIntState("CharDelete.KeepDays", 30);

	m_int_configs[CONFIG_NO_GRAY_AGGRO_ABOVE] = GetConfigSettings::GetIntState("No.Gray.Aggro.Above", 0);
    m_int_configs[CONFIG_NO_GRAY_AGGRO_BELOW] = GetConfigSettings::GetIntState("No.Gray.Aggro.Below", 0);
    if (m_int_configs[CONFIG_NO_GRAY_AGGRO_ABOVE] > m_int_configs[CONFIG_MAX_PLAYER_LEVEL])
    {
       sLog->OutErrorConsole("No.Gray.Aggro.Above (%i) must be in range 0..%u. Set to %u.", m_int_configs[CONFIG_NO_GRAY_AGGRO_ABOVE], m_int_configs[CONFIG_MAX_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL]);
       m_int_configs[CONFIG_NO_GRAY_AGGRO_ABOVE] = m_int_configs[CONFIG_MAX_PLAYER_LEVEL];
    }
    if (m_int_configs[CONFIG_NO_GRAY_AGGRO_BELOW] > m_int_configs[CONFIG_MAX_PLAYER_LEVEL])
    {
       sLog->OutErrorConsole("No.Gray.Aggro.Below (%i) must be in range 0..%u. Set to %u.", m_int_configs[CONFIG_NO_GRAY_AGGRO_BELOW], m_int_configs[CONFIG_MAX_PLAYER_LEVEL], m_int_configs[CONFIG_MAX_PLAYER_LEVEL]);
       m_int_configs[CONFIG_NO_GRAY_AGGRO_BELOW] = m_int_configs[CONFIG_MAX_PLAYER_LEVEL];
    }
    if (m_int_configs[CONFIG_NO_GRAY_AGGRO_ABOVE] > 0 && m_int_configs[CONFIG_NO_GRAY_AGGRO_ABOVE] < m_int_configs[CONFIG_NO_GRAY_AGGRO_BELOW])
    {
       sLog->OutErrorConsole("No.Gray.Aggro.Below (%i) cannot be greater than No.Gray.Aggro.Above (%i). Set to %i.", m_int_configs[CONFIG_NO_GRAY_AGGRO_BELOW], m_int_configs[CONFIG_NO_GRAY_AGGRO_ABOVE], m_int_configs[CONFIG_NO_GRAY_AGGRO_ABOVE]);
       m_int_configs[CONFIG_NO_GRAY_AGGRO_BELOW] = m_int_configs[CONFIG_NO_GRAY_AGGRO_ABOVE];
    }

    ///- Read the "Data" directory from the config file
    std::string dataPath = GetConfigSettings::GetStringDefault("DataDir", "./");
    if (dataPath.at(dataPath.length()-1) != '/' && dataPath.at(dataPath.length()-1) != '\\')
        dataPath.push_back('/');

    if (reload)
    {
        if (dataPath != m_dataPath)
            sLog->OutErrorConsole("DataDir option can't be changed at worldserver.conf reload, using current value (%s).", m_dataPath.c_str());
    }
    else
    {
        m_dataPath = dataPath;
        sLog->OutConsole("Using DataDir %s", m_dataPath.c_str());
    }

    m_bool_configs[CONFIG_VMAP_INDOOR_CHECK] = GetConfigSettings::GetBoolState("vmap.enableIndoorCheck", 0);
    bool enableIndoor = GetConfigSettings::GetBoolState("vmap.enableIndoorCheck", true);
    bool enableLOS = GetConfigSettings::GetBoolState("vmap.enableLOS", true);
    bool enableHeight = GetConfigSettings::GetBoolState("vmap.enableHeight", true);
    bool enablePetLOS = GetConfigSettings::GetBoolState("vmap.petLOS", true);

    if (!enableHeight)
        sLog->OutErrorConsole("VMap height checking disabled! Creatures movements and other various things WILL be broken! Expect no support.");

    VMAP::VMapFactory::createOrGetVMapManager()->setEnableLineOfSightCalc(enableLOS);
    VMAP::VMapFactory::createOrGetVMapManager()->setEnableHeightCalc(enableHeight);
    sLog->OutConsole("WORLD: VMap support included. LineOfSight: %i, getHeight: %i, indoorCheck: %i PetLOS: %i", enableLOS, enableHeight, enableIndoor, enablePetLOS);
    sLog->OutConsole("WORLD: VMap data directory is: %svmaps", m_dataPath.c_str());

    m_int_configs[CONFIG_MAX_WHO] = GetConfigSettings::GetIntState("MaxWhoListReturns", 49);
    m_bool_configs[CONFIG_PET_LOS] = GetConfigSettings::GetBoolState("vmap.petLOS", true);
    m_bool_configs[CONFIG_START_ALL_SPELLS] = GetConfigSettings::GetBoolState("PlayerStart.AllSpells", false);
    if (m_bool_configs[CONFIG_START_ALL_SPELLS])
        sLog->OutConsole("WORLD: WARNING: PlayerStart.AllSpells enabled - may not function as intended!");
    m_int_configs[CONFIG_HONOR_AFTER_DUEL] = GetConfigSettings::GetIntState("HonorPointsAfterDuel", 0);
    m_bool_configs[CONFIG_START_ALL_EXPLORED] = GetConfigSettings::GetBoolState("PlayerStart.MapsExplored", false);
    m_bool_configs[CONFIG_START_ALL_REP] = GetConfigSettings::GetBoolState("PlayerStart.AllReputation", false);
    m_bool_configs[CONFIG_ALWAYS_MAXSKILL] = GetConfigSettings::GetBoolState("AlwaysMaxWeaponSkill", false);
    m_bool_configs[CONFIG_PVP_TOKEN_ENABLE] = GetConfigSettings::GetBoolState("PvPToken.Enable", false);
    m_int_configs[CONFIG_PVP_TOKEN_MAP_TYPE] = GetConfigSettings::GetIntState("PvPToken.MapAllowType", 4);
    m_int_configs[CONFIG_PVP_TOKEN_ID] = GetConfigSettings::GetIntState("PvPToken.ItemID", 29434);
    m_int_configs[CONFIG_PVP_TOKEN_COUNT] = GetConfigSettings::GetIntState("PvPToken.ItemCount", 1);
    if (m_int_configs[CONFIG_PVP_TOKEN_COUNT] < 1)
        m_int_configs[CONFIG_PVP_TOKEN_COUNT] = 1;

	m_bool_configs[CONFIG_ALLOW_TRACK_BOTH_RESOURCES] = GetConfigSettings::GetBoolState("AllowTrackBothResources", false);
    m_bool_configs[CONFIG_NO_RESET_TALENT_COST] = GetConfigSettings::GetBoolState("NoResetTalentsCost", false);
    m_bool_configs[CONFIG_SHOW_KICK_IN_WORLD] = GetConfigSettings::GetBoolState("ShowKickInWorld", false);
	m_bool_configs[CONFIG_SHOW_MUTE_IN_WORLD] = GetConfigSettings::GetBoolState("ShowMuteInWorld", false);
	m_bool_configs[CONFIG_SHOW_BAN_IN_WORLD] = GetConfigSettings::GetBoolState("ShowBanInWorld", false);
    m_int_configs[CONFIG_INTERVAL_LOG_UPDATE] = GetConfigSettings::GetIntState("RecordUpdateTimeDiffInterval", 60000);
    m_int_configs[CONFIG_MIN_LOG_UPDATE] = GetConfigSettings::GetIntState("MinRecordUpdateTimeDiff", 100);
    m_int_configs[CONFIG_NUMTHREADS] = GetConfigSettings::GetIntState("MapUpdate.Threads", 1);
    m_int_configs[CONFIG_MAX_RESULTS_LOOKUP_COMMANDS] = GetConfigSettings::GetIntState("Command.LookupMaxResults", 0);

    // chat logging
    m_bool_configs[CONFIG_CHATLOG_CHANNEL] = GetConfigSettings::GetBoolState("ChatLogs.Channel", false);
    m_bool_configs[CONFIG_CHATLOG_WHISPER] = GetConfigSettings::GetBoolState("ChatLogs.Whisper", false);
    m_bool_configs[CONFIG_CHATLOG_SYSCHAN] = GetConfigSettings::GetBoolState("ChatLogs.SysChan", false);
    m_bool_configs[CONFIG_CHATLOG_PARTY] = GetConfigSettings::GetBoolState("ChatLogs.Party", false);
    m_bool_configs[CONFIG_CHATLOG_RAID] = GetConfigSettings::GetBoolState("ChatLogs.Raid", false);
    m_bool_configs[CONFIG_CHATLOG_GUILD] = GetConfigSettings::GetBoolState("ChatLogs.Guild", false);
    m_bool_configs[CONFIG_CHATLOG_PUBLIC] = GetConfigSettings::GetBoolState("ChatLogs.Public", false);
    m_bool_configs[CONFIG_CHATLOG_ADDON] = GetConfigSettings::GetBoolState("ChatLogs.Addon", false);
    m_bool_configs[CONFIG_CHATLOG_BGROUND] = GetConfigSettings::GetBoolState("ChatLogs.Battleground", false);

	// Warden
	m_bool_configs[CONFIG_WARDEN_ENABLED] = GetConfigSettings::GetBoolState("Warden.Enabled", false);
	m_int_configs[CONFIG_WARDEN_NUM_MEM_CHECKS] = GetConfigSettings::GetIntState("Warden.NumMemChecks", 3);
	m_int_configs[CONFIG_WARDEN_NUM_OTHER_CHECKS] = GetConfigSettings::GetIntState("Warden.NumOtherChecks", 7);
	m_int_configs[CONFIG_WARDEN_CLIENT_BAN_DURATION] = GetConfigSettings::GetIntState("Warden.BanDuration", 86400);
	m_int_configs[CONFIG_WARDEN_CLIENT_CHECK_HOLDOFF] = GetConfigSettings::GetIntState("Warden.ClientCheckHoldOff", 30);
	m_int_configs[CONFIG_WARDEN_CLIENT_FAIL_ACTION] = GetConfigSettings::GetIntState("Warden.ClientCheckFailAction", 0);
	m_int_configs[CONFIG_WARDEN_CLIENT_RESPONSE_DELAY] = GetConfigSettings::GetIntState("Warden.ClientResponseDelay", 600);

    // Dungeon finder
    m_bool_configs[CONFIG_DUNGEON_FINDER_ENABLE] = GetConfigSettings::GetBoolState("DungeonFinder.Enable", false);

    // DBC_ItemAttributes
    m_bool_configs[CONFIG_DBC_ENFORCE_ITEM_ATTRIBUTES] = GetConfigSettings::GetBoolState("DBC.EnforceItemAttributes", true);

    // Max instances per hour
    m_int_configs[CONFIG_MAX_INSTANCES_PER_HOUR] = GetConfigSettings::GetIntState("AccountInstancesPerHour", 5);

	// Anounce reset of instance to whole party
	m_bool_configs[CONFIG_INSTANCES_RESET_ANNOUNCE] = GetConfigSettings::GetBoolState("InstancesResetAnnounce", false);

    // AutoBroadcast
    m_bool_configs[CONFIG_AUTOBROADCAST] = GetConfigSettings::GetBoolState("AutoBroadcast.On", false);
    m_int_configs[CONFIG_AUTOBROADCAST_CENTER] = GetConfigSettings::GetIntState("AutoBroadcast.Center", 0);
    m_int_configs[CONFIG_AUTOBROADCAST_INTERVAL] = GetConfigSettings::GetIntState("AutoBroadcast.Timer", 60000);
	if (reload)
	{
		m_timers[WUPDATE_AUTOBROADCAST].SetInterval(m_int_configs[CONFIG_AUTOBROADCAST_INTERVAL]);
		m_timers[WUPDATE_AUTOBROADCAST].Reset();
	}

    // MySQL ping time interval
    m_int_configs[CONFIG_DB_PING_INTERVAL] = GetConfigSettings::GetIntState("MaxPingTime", 30);

	// Reset Duel Cooldown 
	m_bool_configs[CONFIG_DUEL_RESET_COOLDOWN_ON_START] = GetConfigSettings::GetBoolState("Reset.Cooldown.OnStart", false);
	m_bool_configs[CONFIG_DUEL_RESET_COOLDOWN_ON_FINISH] = GetConfigSettings::GetBoolState("Reset.Cooldown.OnFinish", false);
	m_bool_configs[CONFIG_DUEL_RESET_COOLDOWN_RESET_ENERGY_ON_START] = GetConfigSettings::GetBoolState("Reset.Energy.OnStart", false);
	m_bool_configs[CONFIG_DUEL_RESET_COOLDOWN_MAX_ENERGY_ON_START] = GetConfigSettings::GetBoolState("Maximum.Energy.OnStart", false);
	m_bool_configs[CONFIG_DUEL_RESET_COOLDOWN_ONLY_IN_ELWYNN_AND_DUROTAR] = GetConfigSettings::GetBoolState("Reset.Cooldown.Only.ElwynnForest.and.Durotar", false);

    // misc
    m_bool_configs[CONFIG_PDUMP_NO_PATHS] = GetConfigSettings::GetBoolState("PlayerDump.Disallow.Paths", true);
    m_bool_configs[CONFIG_PDUMP_NO_OVERWRITE] = GetConfigSettings::GetBoolState("PlayerDump.Disallow.Overwrite", true);

	m_int_configs[CONFIG_BIRTHDAY_TIME] = GetConfigSettings::GetIntState("BirthdayTime", 1222964635);

	// Wintergrasp
    m_bool_configs[CONFIG_OUTDOOR_PVP_WINTERGRASP_ENABLED] = GetConfigSettings::GetBoolState("Wintergrasp.Enabled", true);
    m_int_configs[CONFIG_OUTDOOR_PVP_WINTERGRASP_SAVE_STATE_PERIOD] = GetConfigSettings::GetIntState("Wintergrasp.SaveState.Period", 10000);
    m_int_configs[CONFIG_OUTDOOR_PVP_WINTERGRASP_START_TIME] = GetConfigSettings::GetIntState("Wintergrasp.StartTime", 30);
    m_int_configs[CONFIG_OUTDOOR_PVP_WINTERGRASP_BATTLE_TIME] = GetConfigSettings::GetIntState("Wintergrasp.BattleTime", 30);
    m_int_configs[CONFIG_OUTDOOR_PVP_WINTERGRASP_INTERVAL] = GetConfigSettings::GetIntState("Wintergrasp.Interval", 150);
    m_bool_configs[CONFIG_OUTDOOR_PVP_WINTERGRASP_CUSTOM_HONOR] = GetConfigSettings::GetBoolState("Wintergrasp.CustomHonorRewards", false);
    m_int_configs[CONFIG_OUTDOOR_PVP_WINTERGRASP_WIN_BATTLE] = GetConfigSettings::GetIntState("Wintergrasp.CustomHonorBattleWin", 3000);
    m_int_configs[CONFIG_OUTDOOR_PVP_WINTERGRASP_LOSE_BATTLE] = GetConfigSettings::GetIntState("Wintergrasp.CustomHonorBattleLose", 1250);
    m_int_configs[CONFIG_OUTDOOR_PVP_WINTERGRASP_DAMAGED_TOWER] = GetConfigSettings::GetIntState("Wintergrasp.CustomHonorDamageTower", 750);
    m_int_configs[CONFIG_OUTDOOR_PVP_WINTERGRASP_DESTROYED_TOWER] = GetConfigSettings::GetIntState("Wintergrasp.CustomHonorDestroyedTower", 750);
    m_int_configs[CONFIG_OUTDOOR_PVP_WINTERGRASP_DAMAGED_BUILDING] = GetConfigSettings::GetIntState("Wintergrasp.CustomHonorDamagedBuilding", 750);
    m_int_configs[CONFIG_OUTDOOR_PVP_WINTERGRASP_INTACT_BUILDING] = GetConfigSettings::GetIntState("Wintergrasp.CustomHonorIntactBuilding", 1500);
    m_bool_configs[CONFIG_OUTDOOR_PVP_WINTERGRASP_ANTIFARM_ENABLE] = GetConfigSettings::GetBoolState("Wintergrasp.Antifarm.Enable", false);
    m_int_configs[CONFIG_OUTDOOR_PVP_WINTERGRASP_ANTIFARM_ATK] = GetConfigSettings::GetIntState("Wintergrasp.Antifarm.Atk", 5);
    m_int_configs[CONFIG_OUTDOOR_PVP_WINTERGRASP_ANTIFARM_DEF] = GetConfigSettings::GetIntState("Wintergrasp.Antifarm.Def", 5);
	m_int_configs[CONFIG_OUTDOOR_PVP_WINTERGRASP_MIN_LEVEL] = GetConfigSettings::GetIntState("Wintergrasp.Minlevel", 74);
	m_int_configs[CONFIG_OUTDOOR_PVP_WINTERGRASP_MAX_PLAYERS] = GetConfigSettings::GetIntState("Wintergrasp.MaxPlayers", 40);
	// Custom Wintergrasp Options
	m_bool_configs[CONFIG_OUTDOOR_PVP_WINTERGRASP_TIMER_ANNOUNCE_ENABLE] = GetConfigSettings::GetBoolState("Wintergrasp.Timer.Announce.Enable", false);
	m_bool_configs[CONFIG_OUTDOOR_PVP_WINTERGRASP_STATISTIC_ENABLE] = GetConfigSettings::GetBoolState("Wintergrasp.Statistic.Enable", false);
	m_bool_configs[CONFIG_OUTDOOR_PVP_WG_STATS_ENABLED] = GetConfigSettings::GetBoolState("Wintergrasp.Stats.Enable", false);
	m_int_configs[CONFIG_OUTDOOR_PVP_WG_STATS_UPDATE_INTERVAL] = GetConfigSettings::GetIntState("Wintergrasp.Stats.Update.Interval", 2000);
	m_int_configs[CONFIG_OUTDOOR_PVP_WG_STATS_DATA_ID] = GetConfigSettings::GetIntState("Wintergrasp.Stats.Data.ID", 1);

	// External Mail
	m_bool_configs[CONFIG_EXTERNAL_MAIL] = GetConfigSettings::GetBoolState("External.Mail.Enable", false);
	m_int_configs[CONFIG_EXTERNAL_MAIL_INTERVAL] = GetConfigSettings::GetIntState("External.Mail.Interval", 5);

	// Start item sets
	m_int_configs[CONFIG_START_PLAYERS_ITEMS] = GetConfigSettings::GetIntState("Player.Item.Sets.Enable", 0);

	// Name command
	m_int_configs[CONFIG_NAME_COMMAND] = GetConfigSettings::GetIntState("Name.Announce.Command.Enable", 1);
	
	//Vip commands
    m_bool_configs[CONFIG_VIP_DEBUFF_COMMAND] = GetConfigSettings::GetBoolState("VIP.Debuff.Command", true);
    m_bool_configs[CONFIG_VIP_BANK_COMMAND] = GetConfigSettings::GetBoolState("VIP.Bank.Command", true);
    m_bool_configs[CONFIG_VIP_REPAIR_COMMAND] = GetConfigSettings::GetBoolState("VIP.Repair.Command", true);
    m_bool_configs[CONFIG_VIP_RESET_TALENTS_COMMAND] = GetConfigSettings::GetBoolState("VIP.Reset.Talents.Command", true);
    m_bool_configs[CONFIG_VIP_TAXI_COMMAND] = GetConfigSettings::GetBoolState("VIP.Taxi.Command", true);
    m_bool_configs[CONFIG_VIP_HOME_COMMAND] = GetConfigSettings::GetBoolState("VIP.Home.Command", true);
    m_bool_configs[CONFIG_VIP_CAPITAL_COMMAND] = GetConfigSettings::GetBoolState("VIP.Capital.Command", true);

	// Antispam
	m_int_configs[CONFIG_CHAT_CHANNEL_TIME_REQ] = GetConfigSettings::GetIntState("Guard.Chat.Global.Channel", 1800);
	m_int_configs[CONFIG_CHAT_WHISPER_TIME_REQ] = GetConfigSettings::GetIntState("Guard.Chat.Whisper", 1800);
	m_int_configs[CONFIG_CHAT_SAY_TIME_REQ] = GetConfigSettings::GetIntState("Guard.Chat.Say.Yell.Emote", 1800);

    // World of Warcraft Armory
    m_bool_configs[CONFIG_ARMORY_ENABLE] = GetConfigSettings::GetBoolState("Armory.Enable", false);

	// Gm monitoring
	m_bool_configs[CONFIG_GM_ENTER_IN_GAME] = GetConfigSettings::GetBoolState("Gm.Monitoring.Enter.Game", false);
	m_bool_configs[CONFIG_GM_LEAVE_IN_GAME] = GetConfigSettings::GetBoolState("Gm.Monitoring.Leave.Game", false);

	// Chat lfg
	m_bool_configs[CONFIG_LFG_LOOKING_FOR_GROUP] = GetConfigSettings::GetBoolState("Looking.For.Group.Join.All", true);

	// Northrend access with 68 lvl
	m_bool_configs[CONFIG_NORTHREND_WITH_68_LEVEL] = GetConfigSettings::GetBoolState("Northrend.Access.Min.68.Level", true);

	// Damage checker
	m_int_configs[CONFIG_DAMAGE_CHECKER_MAX_DAMAGE] = GetConfigSettings::GetIntState("Damage.Checker.Max.Damage", 257000);
	m_int_configs[CONFIG_DAMAGE_CHECKER_IMMUNE_MIN_GM_LEVEL] = GetConfigSettings::GetIntState("Damage.Checker.Immune.Min.GM.Level", 1);
	m_int_configs[CONFIG_DAMAGE_CHECKER_BAN_VALUE] = GetConfigSettings::GetIntState("Damage.Checker.Ban.Value", 3600);

	// Played Timed Reward
	m_int_configs[CONFIG_PLAYED_TIME_REWARD_INTERVAL_TIME] = GetConfigSettings::GetIntState("Played.Time.Reward.Interval.Time", 3600000);
	m_int_configs[CONFIG_PLAYED_TIME_REWARD_MONEY] = GetConfigSettings::GetIntState("Played.Time.Reward.Money", 15);
	m_int_configs[CONFIG_PLAYED_TIME_REWARD_HONOR] = GetConfigSettings::GetIntState("Played.Time.Reward.Honor", 25);
	m_int_configs[CONFIG_PLAYED_TIME_REWARD_ARENA] = GetConfigSettings::GetIntState("Played.Time.Reward.Arena", 5);

	// PVP Rank Achievements
	m_bool_configs[CONFIG_PVP_RANK_ACHIEVEMENTS_ENABLE] = GetConfigSettings::GetBoolState("PvP.Rank.Achievements.Enable", true);

	// Color Icons In Chat System
	m_bool_configs[CONFIG_COLOR_ICON_CHAT_ENABLE] = GetConfigSettings::GetBoolState("Colored.Icons.InChat.Enable", true);

	// Login Info System
	m_bool_configs[CONFIG_CHAR_LOGIN_INFO_ENABLE] = GetConfigSettings::GetBoolState("Character.Login.Info.Enable", true);

	// Guild Level Bonus System
	m_bool_configs[CONFIG_GUILD_BONUS_GOLD_BANK_ENABLE] = GetConfigSettings::GetBoolState("Guild.Bonus.Gold.From.Bank.Enable", true);
	m_bool_configs[CONFIG_GUILD_BONUS_FOR_REPAIR_ENABLE] = GetConfigSettings::GetBoolState("Guild.Bonus.Repair.From.Bank.Enable", true);
	m_bool_configs[CONFIG_GUILD_BONUS_XP_ENABLE] = GetConfigSettings::GetBoolState("Guild.Bonus.Xperience.Gain.Enable", true);
	m_bool_configs[CONFIG_GUILD_BONUS_REPUTATION_ENABLE] = GetConfigSettings::GetBoolState("Guild.Bonus.Reputation.Gain.Enable", true);
	m_bool_configs[CONFIG_GUILD_BONUS_HONOR_ENABLE] = GetConfigSettings::GetBoolState("Guild.Bonus.Honor.Gain.Enable", true);
	m_bool_configs[CONFIG_GUILD_BONUS_MOUNT_SPEED_ENABLE] = GetConfigSettings::GetBoolState("Guild.Bonus.Mount.Speed.Enable", true);
	m_bool_configs[CONFIG_GUILD_BONUS_FAST_SPIRIT_ENABLE] = GetConfigSettings::GetBoolState("Guild.Bonus.Fast.Spirit.Enable", true);

	// QuantumCore Altershop System by Crispi
	m_bool_configs[CONFIG_NEW_FUN_SERVER_SYSTEM_ENABLE] = GetConfigSettings::GetBoolState("QuantumCore.New.Fun.System.Enable", true);
	m_bool_configs[CONFIG_OLD_FUN_SERVER_SYSTEM_ENABLE] = GetConfigSettings::GetBoolState("QuantumCore.Old.Fun.System.Enable", true);
	m_bool_configs[CONFIG_FUN_INSTANT_CAST_TIME_FOR_CRAFT] = GetConfigSettings::GetBoolState("QuantumCore.Fun.InstantCraft.CastTime.Enable", true);
	m_bool_configs[CONFIG_FUN_GUARD_DETECTION_ENABLE] = GetConfigSettings::GetBoolState("QuantumCore.Fun.Guard.Detection.Enable", true);
	m_bool_configs[CONFIG_FUN_BUFF_FOR_VENDORS_ENABLE] = GetConfigSettings::GetBoolState("QuantumCore.Fun.BuffForNpc.Enable", true);
	m_bool_configs[CONFIG_FUN_TRAINER_CAST_CLASS_SPELLS] = GetConfigSettings::GetBoolState("QuantumCore.Fun.Trainer.CastSpells.Enable", true);
	m_bool_configs[CONFIG_FUN_CASCADE_OF_ROSES_ENABLE] = GetConfigSettings::GetBoolState("QuantumCore.Fun.RoseAura.ForNpc.Enable", true);

	// Guild-Level-System
	LoadGuildBonusInfo();

	// Ñall ScriptMgr if we're reloading the configuration
	if (reload)
		sScriptMgr->OnConfigLoad(reload);
}

extern void LoadGameObjectModelList();

/// Initialize the World
void World::SetInitialWorldSettings()
{
    ///- Server startup begin
    uint32 StartupBegin = getMSTime();

    ///- Initialize the random number generator
    srand((unsigned int)time(NULL));

    ///- Initialize config settings
    LoadConfigSettings();

    ///- Initialize Allowed Security Level
    LoadDBAllowedSecurityLevel();

    ///- Init highest guids before any table loading to prevent using not initialized guids in some code.
    sObjectMgr->SetHighestGuids();

    ///- Check the existence of the map files for all races' startup areas.
	if (!MapManager::ExistMapAndVMap(0, -6240.32f, 331.033f)
		|| !MapManager::ExistMapAndVMap(0, -8949.95f, -132.493f)
		|| !MapManager::ExistMapAndVMap(1, -618.518f, -4251.67f)
        || !MapManager::ExistMapAndVMap(0, 1676.35f, 1677.45f)
        || !MapManager::ExistMapAndVMap(1, 10311.3f, 832.463f)
        || !MapManager::ExistMapAndVMap(1, -2917.58f, -257.98f)
        || (m_int_configs[CONFIG_EXPANSION] && (
		!MapManager::ExistMapAndVMap(530, 10349.6f, -6357.29f) ||
		!MapManager::ExistMapAndVMap(530, -3961.64f, -13931.2f))))
	{
        sLog->OutErrorConsole("Correct *.map files not found in path '%smaps' or *.vmtree/*.vmtile files in '%svmaps'. Please place *.map/*.vmtree/*.vmtile files in appropriate directories or correct the DataDir value in the worldserver.conf file.", m_dataPath.c_str(), m_dataPath.c_str());
        exit(1);
	}

    sPoolMgr->Initialize();

    sGameEventMgr->Initialize();

    sLog->OutConsole();
    sLog->OutConsole("QUANTUMCORE SERVER: Loading Quantum System Texts...");
    if (!sObjectMgr->LoadQuantumSystemTexts())
        exit(1);

    uint32 server_type;
    if (IsFFAPvPRealm())
        server_type = REALM_TYPE_PVP;
    else
        server_type = getIntConfig(CONFIG_GAME_TYPE);

    uint32 realm_zone = getIntConfig(CONFIG_REALM_ZONE);

    LoginDatabase.PExecute("UPDATE realmlist SET icon = %u, timezone = %u WHERE id = '%d'", server_type, realm_zone, realmID);      // One-time query

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_OLD_CORPSES);
    stmt->setUInt32(0, 3 * DAY);
    CharacterDatabase.Execute(stmt);

    sLog->OutConsole("Initialize Data Stores...");
    LoadDBCStores(m_dataPath);
    DetectDBCLang();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading spell dbc data corrections...");
    sSpellMgr->LoadDbcDataCorrections();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading SpellInfo Store...");
    sSpellMgr->LoadSpellInfoStore();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading Skill Line Ability Multi Map Data...");
	sSpellMgr->LoadSkillLineAbilityMap();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Spell Custom Attributes...");
    sSpellMgr->LoadSpellCustomAttributes();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading GameObject Models...");
	LoadGameObjectModelList();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Script Names...");
    sObjectMgr->LoadScriptNames();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Instance Template...");
    sObjectMgr->LoadInstanceTemplate();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading instances...");
    sInstanceSaveMgr->LoadInstances();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Localization Strings...");
    uint32 oldMSTime = getMSTime();
    sObjectMgr->LoadCreatureLocales();
    sObjectMgr->LoadGameObjectLocales();
    sObjectMgr->LoadItemLocales();
    sObjectMgr->LoadItemSetNameLocales();
    sObjectMgr->LoadQuestLocales();
    sObjectMgr->LoadNpcTextLocales();
    sObjectMgr->LoadPageTextLocales();
    sObjectMgr->LoadGossipMenuItemsLocales();
    sObjectMgr->LoadPointOfInterestLocales();

    sObjectMgr->SetDBCLocaleIndex(GetDefaultDbcLocale());
	sLog->OutConsole("=========================================");
    sLog->OutConsole(">> Localization strings loaded in %u ms", GetMSTimeDiffToNow(oldMSTime));
	sLog->OutConsole("=========================================");
    sLog->OutConsole();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Page Texts...");
    sObjectMgr->LoadPageTexts();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Game Object Templates...");
    sObjectMgr->LoadGameObjectTemplate();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Spell Rank Data...");
    sSpellMgr->LoadSpellRanks();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Spell Required Data...");
    sSpellMgr->LoadSpellRequired();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Spell Group Types...");
    sSpellMgr->LoadSpellGroups();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Spell Learn Skills...");
    sSpellMgr->LoadSpellLearnSkills();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Spell Learn Spells...");
    sSpellMgr->LoadSpellLearnSpells();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Spell Proc Event Conditions...");
    sSpellMgr->LoadSpellProcEvents();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Spell Proc Conditions and Data...");
    sSpellMgr->LoadSpellProcs();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Spell Bonus Data...");
    sSpellMgr->LoadSpellBonusess();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Aggro Spells Definitions...");
    sSpellMgr->LoadSpellThreats();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Spell Group Stack Rules...");
    sSpellMgr->LoadSpellGroupStackRules();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Gossip Menu Texts...");
    sObjectMgr->LoadGossipText();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Enchant Spells Proc Datas...");
    sSpellMgr->LoadSpellEnchantProcData();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Item Random Enchantments Table...");
    LoadRandomEnchantmentsTable();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Disables");
    DisableMgr::LoadDisables();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Items...");
    sObjectMgr->LoadItemTemplates();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Item Set Names...");
    sObjectMgr->LoadItemSetNames();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading Transmogrification Items...");
	sObjectMgr->LoadTransmogrifications();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Creature Model Based Info Data...");
    sObjectMgr->LoadCreatureModelInfo();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Equipment Templates...");
    sObjectMgr->LoadEquipmentTemplates();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Creature Template...");
    sObjectMgr->LoadCreatureTemplates();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Creature Template Addons...");
    sObjectMgr->LoadCreatureTemplateAddons();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Reputation Reward Rate Data...");
    sObjectMgr->LoadReputationRewardRate();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Creature Reputation OnKill Data...");
    sObjectMgr->LoadReputationOnKill();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Reputation Spillover Data..." );
    sObjectMgr->LoadReputationSpilloverTemplate();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Points Of Interest Data...");
    sObjectMgr->LoadPointsOfInterest();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Creature Base Stats...");
    sObjectMgr->LoadCreatureClassLevelStats();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Creature Spawn Positions...");
    sObjectMgr->LoadCreatures();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading Temporary Summon Data...");
	sObjectMgr->LoadTempSummons();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Pet LevelUp Spells...");
    sSpellMgr->LoadPetLevelupSpellMap();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Pet Default Spells Additional To Levelup Spells...");
    sSpellMgr->LoadPetDefaultSpells();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Creature Addon Data...");
    sObjectMgr->LoadCreatureAddons();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Gameobject Spawn Positions...");
    sObjectMgr->LoadGameobjects();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading GameObject Addon Data...");
	sObjectMgr->LoadGameObjectAddons();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Creature Linked Respawn Data...");
    sObjectMgr->LoadLinkedRespawn();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Weather Data...");
    WeatherMgr::LoadWeatherData();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Quest Template...");
    sObjectMgr->LoadQuests();

    sLog->OutConsole("Checking Quest Disables");
    DisableMgr::CheckQuestDisables();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Quest POI Points");
    sObjectMgr->LoadQuestPOI();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Quests Relations...");
    sObjectMgr->LoadQuestRelations();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Objects Pooling Data...");
    sPoolMgr->LoadFromDB();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Game Event Data...");
    sGameEventMgr->LoadFromDB();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Spellclick Data...");
    sObjectMgr->LoadNPCSpellClickSpells();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading Chat Filter Words...");
	sObjectMgr->LoadChatFilter();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Vehicle Template Accessories...");
    sObjectMgr->LoadVehicleTemplateAccessories();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Dungeon Boss Data...");
    sObjectMgr->LoadInstanceEncounters();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading LFG Rewards...");
    sLFGMgr->LoadRewards();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading LFG Entrance Positions...");
	sLFGMgr->LoadEntrancePositions();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Spell Area Data...");
    sSpellMgr->LoadSpellAreas();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading AreaTrigger Definitions...");
    sObjectMgr->LoadAreaTriggerTeleports();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Access Requirements...");
    sObjectMgr->LoadAccessRequirements();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Quest Area Triggers...");
    sObjectMgr->LoadQuestAreaTriggers();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Tavern Area Triggers...");
    sObjectMgr->LoadTavernAreaTriggers();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading AreaTrigger Script Names...");
    sObjectMgr->LoadAreaTriggerScripts();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Graveyard-Zone Links...");
    sObjectMgr->LoadGraveyardZones();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Spell Pet Auras...");
    sSpellMgr->LoadSpellPetAuras();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Spell Target Coordinates...");
    sSpellMgr->LoadSpellTargetPositions();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Enchant Custom Attributes...");
    sSpellMgr->LoadEnchantCustomAttr();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Linked Spells...");
    sSpellMgr->LoadSpellLinked();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Player Create Data...");
    sObjectMgr->LoadPlayerInfo();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Exploration Base XP Data...");
    sObjectMgr->LoadExplorationBaseXP();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Pet Name Parts...");
    sObjectMgr->LoadPetNames();

    CharacterDatabaseCleaner::CleanDatabase();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading The Max Pet Number...");
    sObjectMgr->LoadPetNumber();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Pet Level Stats...");
    sObjectMgr->LoadPetLevelInfo();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Player Corpses...");
    sObjectMgr->LoadCorpses();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Player Level Dependent Mail Rewards...");
    sObjectMgr->LoadMailLevelRewards();

    LoadLootTables();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Skill Discovery Table...");
    LoadSkillDiscoveryTable();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Skill Extra Item Table...");
    LoadSkillExtraItemTable();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading Skill Perfection Data Table...");
	LoadSkillPerfectItemTable();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Skill Fishing Base Level Requirements...");
    sObjectMgr->LoadFishingBaseSkillLevel();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Achievements...");
    sAchievementMgr->LoadAchievementReferenceList();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Achievement Criteria Lists...");
    sAchievementMgr->LoadAchievementCriteriaList();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Achievement Criteria Data...");
    sAchievementMgr->LoadAchievementCriteriaData();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Achievement Rewards...");
    sAchievementMgr->LoadRewards();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Achievement Reward Locales...");
    sAchievementMgr->LoadRewardLocales();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Completed Achievements...");
    sAchievementMgr->LoadCompletedAchievements();

    sLog->OutConsole("Deleting Expired Auctions...");
    sAuctionMgr->DeleteExpiredAuctionsAtStartup();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Item Auctions...");
    sAuctionMgr->LoadAuctionItems();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Auctions...");
    sAuctionMgr->LoadAuctions();

    sGuildMgr->LoadGuilds();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading ArenaTeams...");
    sArenaTeamMgr->LoadArenaTeams();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Groups...");
    sGroupMgr->LoadGroups();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Reserved Names...");
    sObjectMgr->LoadReservedPlayersNames();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading GameObjects For Quests...");
    sObjectMgr->LoadGameObjectForQuests();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Battlemasters...");
    sBattlegroundMgr->LoadBattleMastersEntry();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading GameTeleports...");
    sObjectMgr->LoadGameTele();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Gossip Menu...");
    sObjectMgr->LoadGossipMenu();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Gossip Menu Options...");
    sObjectMgr->LoadGossipMenuItems();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Vendors...");
    sObjectMgr->LoadVendors();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Trainers...");
    sObjectMgr->LoadTrainerSpell();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Waypoints...");
    sWaypointMgr->Load();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading SmartAI Waypoints...");
    sSmartWaypointMgr->LoadFromDB();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Creature Formations...");
    sFormationMgr->LoadCreatureFormations();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading World States...");
	LoadWorldStates();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Conditions...");
    sConditionMgr->LoadConditions();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Faction Change Achievements...");
    sObjectMgr->LoadFactionChangeAchievements();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Faction Change Spells...");
    sObjectMgr->LoadFactionChangeSpells();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading Faction Change Quests...");
	sObjectMgr->LoadFactionChangeQuests();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Faction Change Items...");
    sObjectMgr->LoadFactionChangeItems();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Faction Change Reputations...");
    sObjectMgr->LoadFactionChangeReputations();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading Faction Change Titles...");
	sObjectMgr->LoadFactionChangeTitles();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Game Master Tickets...");
    sTicketMgr->LoadTickets();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Game Master Surveys...");
    sTicketMgr->LoadSurveys();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Client Addons...");
    AddonMgr::LoadFromDB();

    sLog->OutConsole("Returning Old Mails...");
    sObjectMgr->ReturnOrDeleteOldMails(false);

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Autobroadcasts...");
    LoadAutobroadcasts();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading Ip2nation...");
	LoadIp2nation();

    sObjectMgr->LoadQuestStartScripts();
    sObjectMgr->LoadQuestEndScripts();
    sObjectMgr->LoadSpellScripts();
    sObjectMgr->LoadGameObjectScripts();
    sObjectMgr->LoadEventScripts();
    sObjectMgr->LoadWaypointScripts();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Quantum Text Locales...");
    sObjectMgr->LoadDbScriptStrings();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Creature EventAI Texts...");
    sEventAIMgr->LoadCreatureEventAI_Texts();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Creature EventAI Scripts...");
    sEventAIMgr->LoadCreatureEventAI_Scripts();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Spell Script Names...");
    sObjectMgr->LoadSpellScriptNames();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Creature Texts...");
    sCreatureTextMgr->LoadCreatureTexts();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading Creature Text Locales...");
	sCreatureTextMgr->LoadCreatureTextLocales();

    sLog->OutConsole("Initializing Scripts...");
    sScriptMgr->Initialize();
	sScriptMgr->OnConfigLoad(false);

    sLog->OutConsole("Validating Spell Scripts...");
    sObjectMgr->ValidateSpellScripts();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading SmartAI Scripts...");
    sSmartScriptMgr->LoadSmartAIFromDB();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading Calendar Data...");
	sCalendarMgr->LoadFromDB();

    sLog->OutConsole("Initialize Game Time and Timers...");
    m_gameTime = time(NULL);
	m_startTime = m_gameTime;

	// Special Delete Info (Account Chatacters Per Realm)
	LoginDatabase.PExecute("DELETE FROM account_char_per_realm WHERE accountid NOT IN (SELECT id FROM account)");
	// Special Delete Info (Uptime)
	LoginDatabase.PExecute("DELETE FROM uptime");
	LoginDatabase.PExecute("INSERT INTO uptime (realm_id, start_time, uptime, revision) VALUES(%u, %u, 0, '%s')", realmID, uint32(m_startTime), _FULLVERSION);

    m_timers[WUPDATE_WEATHERS].SetInterval(1*IN_MILLISECONDS);
    m_timers[WUPDATE_AUCTIONS].SetInterval(MINUTE*IN_MILLISECONDS);
    m_timers[WUPDATE_UPTIME].SetInterval(m_int_configs[CONFIG_UPTIME_UPDATE]*MINUTE*IN_MILLISECONDS);
    m_timers[WUPDATE_CORPSES].SetInterval(20 * MINUTE * IN_MILLISECONDS);
    m_timers[WUPDATE_CLEANDB].SetInterval(m_int_configs[CONFIG_LOGDB_CLEARINTERVAL]*MINUTE*IN_MILLISECONDS);
    m_timers[WUPDATE_AUTOBROADCAST].SetInterval(getIntConfig(CONFIG_AUTOBROADCAST_INTERVAL));
    m_timers[WUPDATE_DELETECHARS].SetInterval(DAY*IN_MILLISECONDS);
    m_timers[WUPDATE_PINGDB].SetInterval(getIntConfig(CONFIG_DB_PING_INTERVAL)*MINUTE*IN_MILLISECONDS);

    mail_timer = ((((localtime(&m_gameTime)->tm_hour + 20) % 24)*HOUR*IN_MILLISECONDS) / m_timers[WUPDATE_AUCTIONS].GetInterval());

    mail_timer_expires = ((DAY * IN_MILLISECONDS) / (m_timers[WUPDATE_AUCTIONS].GetInterval()));
    sLog->outDetail("Mail timer set to: " UI64FMTD ", mail return is called every " UI64FMTD " minutes", uint64(mail_timer), uint64(mail_timer_expires));

	// handle timer for external mail
	m_timers[WUPDATE_EXTMAIL].SetInterval(m_int_configs[CONFIG_EXTERNAL_MAIL_INTERVAL] * MINUTE * IN_MILLISECONDS);

    ///- Initilize static helper structures
    AIRegistry::Initialize();

    ///- Initialize MapManager
    sLog->OutConsole("Starting Map System...");
    sMapMgr->Initialize();

    sLog->OutConsole("Starting Game Event System...");
    uint32 nextGameEvent = sGameEventMgr->StartSystem();
    m_timers[WUPDATE_EVENTS].SetInterval(nextGameEvent);    //depend on next event

    // Delete all characters which have been deleted X days before
    Player::DeleteOldCharacters();

    Channel::CleanOldChannelsInDB();

    sLog->OutConsole("Starting Arena Season...");
    sGameEventMgr->StartArenaSeason();

    sTicketMgr->Initialize();

    sLog->OutConsole("Starting Battleground System");
    sBattlegroundMgr->CreateInitialBattlegrounds();
    sBattlegroundMgr->InitAutomaticArenaPointDistribution();

    sLog->OutConsole("Starting Outdoor PVP System");
    sOutdoorPvPMgr->InitOutdoorPvP();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Transports...");
    sMapMgr->LoadTransports();

    sLog->OutConsole("QUANTUMCORE SERVER: Loading Transport Creatures...");
    sMapMgr->LoadTransportNPCs();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading Warden Checks..." );
	sWardenCheckMgr->LoadWardenChecks();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading Warden Action Overrides..." );
	sWardenCheckMgr->LoadWardenOverrides();

    sLog->OutConsole("Deleting Expired Bans...");
    LoginDatabase.Execute("DELETE FROM ip_banned WHERE unbandate <= UNIX_TIMESTAMP() AND unbandate<>bandate"); // One-time query

	sLog->OutConsole("Updating Preserved Accounts...");
    LoginDatabase.Execute("UPDATE account SET last_login = NOW() WHERE id IN(SELECT account FROM account_preserved) AND id NOT IN (SELECT id FROM account_banned WHERE bandate = unbandate OR unbandate > NOW())");

    sLog->OutConsole("Calculate Next Daily Quest Reset Time...");
    InitDailyQuestResetTime();

    sLog->OutConsole("Calculate Next Weekly Quest Reset Time..." );
    InitWeeklyQuestResetTime();

	sLog->OutConsole("Calculate Next Monthly Quest Reset Time...");
	InitMonthlyQuestResetTime();

    sLog->OutConsole("Calculate Random Battleground Reset Time..." );
    InitRandomBGResetTime();

	sLog->OutConsole("Calculate Guild Cap Reset Time...");
	InitGuildResetTime();

    if (sLog->GetLogDBLater())
    {
        sLog->OutConsole("Enabling Database Logging...");
        sLog->SetLogDBLater(false);
        sLog->SetLogDB(true);
    }
    else sLog->SetLogDB(false);

    uint32 StartupDuration = GetMSTimeDiffToNow(StartupBegin);
    sLog->OutConsole();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading Characters...");
	LoadCharacterNameData();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading Accounts...");
	LoadAccounts();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading GM Accounts...");
	LoadGMAccounts();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading Premium Accounts...");
	LoadPremAccounts();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading Server Commands...");
	LoadServerCommands();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading Rules For Commands...");
	LoadServerCommandRules();

	sLog->OutConsole("QUANTUMCORE SERVER: Loading Characters Have Reached 80 Level...");
	LoadCharCountFromMaxLevel();

	sLog->OutConsole("%s ", _FULLVERSION);
	sLog->OutConsole("%s (library: %s)", OPENSSL_VERSION_TEXT, SSLeay_version(SSLEAY_VERSION));
	sLog->OutConsole("ACE %s", ACE_VERSION);
	sLog->OutConsole("Cmake 2.8.10");
	sLog->OutConsole("Microsoft Visual Studio 2008 Professional");
	sLog->OutConsole();

	sLog->OutConsole("WARNING BE CAREFUL OF FAKES...");
	sLog->OutConsole("ORIGINAL SOFTWARE ONLY IN:");
	sLog->OutConsole("REAL AUTOR OF THE SERVER: Crispi");
	sLog->OutConsole("REAL SKYPE OF THE AUTOR: (Pandemonium902)");
	sLog->OutConsole("REAL CONTACT OF THE AUTOR: https://vk.com/shpierkommel");
	sLog->OutConsole();

	sLog->OutConsole("======================================");
	sLog->OutConsole("Server Loaded in %u minutes %u seconds", (StartupDuration / 60000), ((StartupDuration % 60000) / 1000));
	sLog->OutConsole("======================================");
	sLog->OutConsole();
}

void World::DetectDBCLang()
{
    uint8 m_lang_confid = GetConfigSettings::GetIntState("DBC.Locale", 255);

    if (m_lang_confid != 255 && m_lang_confid >= TOTAL_LOCALES)
    {
        sLog->OutErrorConsole("Incorrect DBC.Locale! Must be >= 0 and < %d (set to 0)", TOTAL_LOCALES);
        m_lang_confid = LOCALE_enUS;
    }

    ChrRacesEntry const* race = sChrRacesStore.LookupEntry(1);

    std::string availableLocalsStr;

    uint8 default_locale = TOTAL_LOCALES;
    for (uint8 i = default_locale-1; i < TOTAL_LOCALES; --i)
    {
        if (race->name[i][0] != '\0')
        {
            default_locale = i;
            m_availableDbcLocaleMask |= (1 << i);
            availableLocalsStr += localeNames[i];
            availableLocalsStr += " ";
        }
    }

    if (default_locale != m_lang_confid && m_lang_confid < TOTAL_LOCALES && (m_availableDbcLocaleMask & (1 << m_lang_confid)))
        default_locale = m_lang_confid;

    if (default_locale >= TOTAL_LOCALES)
    {
        sLog->OutErrorConsole("Unable to determine your DBC Locale! (corrupt DBC?)");
        exit(1);
    }

    m_defaultDbcLocale = LocaleConstant(default_locale);

    sLog->OutConsole("Using %s DBC Locale as default. All available DBC locales: %s", localeNames[m_defaultDbcLocale], availableLocalsStr.empty() ? "<none>" : availableLocalsStr.c_str());
    sLog->OutConsole();
}

void World::RecordTimeDiff(const char *text, ...)
{
    if (m_updateTimeCount != 1)
        return;
    if (!text)
    {
        m_currentTime = getMSTime();
        return;
    }

    uint32 thisTime = getMSTime();
    uint32 diff = getMSTimeDiff(m_currentTime, thisTime);

    if (diff > m_int_configs[CONFIG_MIN_LOG_UPDATE])
    {
        va_list ap;
        char str[256];
        va_start(ap, text);
        vsnprintf(str, 256, text, ap);
        va_end(ap);
        sLog->outDetail("Difftime %s: %u.", str, diff);
    }

    m_currentTime = thisTime;
}

void World::LoadAutobroadcasts()
{
    uint32 oldMSTime = getMSTime();

    m_Autobroadcasts.clear();

    QueryResult result = WorldDatabase.Query("SELECT text FROM autobroadcast");

    if (!result)
    {
		sLog->OutConsole("==========================================================================");
        sLog->OutConsole(">> Loaded 0 autobroadcasts definitions. DB table `autobroadcast` is empty!");
		sLog->OutConsole("==========================================================================");
        sLog->OutConsole();
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        std::string message = fields[0].GetString();

        m_Autobroadcasts.push_back(message);

        ++count;
    }
	while (result->NextRow());

	sLog->OutConsole("==================================================");
    sLog->OutConsole(">> Loaded %u autobroadcasts definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
	sLog->OutConsole("==================================================");
    sLog->OutConsole();
}

void World::LoadIp2nation()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = WorldDatabase.Query("SELECT count(c.code) FROM ip2nationCountries c, ip2nation i WHERE c.code = i.country");
    uint32 count = 0;

    if (result)
    {
        Field* fields = result->Fetch();
        count = fields[0].GetUInt32();
    }

	sLog->OutConsole("=============================================");
    sLog->OutConsole(">> Loaded %u ip2nation definitions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
	sLog->OutConsole("=============================================");
    sLog->OutConsole();
}

void World::Update(uint32 diff)
{
    m_updateTime = diff;

    if (m_int_configs[CONFIG_INTERVAL_LOG_UPDATE] && diff > m_int_configs[CONFIG_MIN_LOG_UPDATE])
    {
        if (m_updateTimeSum > m_int_configs[CONFIG_INTERVAL_LOG_UPDATE])
        {
            sLog->outBasic("Update time diff: %u. Players online: %u.", m_updateTimeSum / m_updateTimeCount, GetActiveSessionCount());
            m_updateTimeSum = m_updateTime;
            m_updateTimeCount = 1;
        }
        else
        {
            m_updateTimeSum += m_updateTime;
            ++m_updateTimeCount;
        }
    }

    for (int i = 0; i < WUPDATE_COUNT; ++i)
    {
        if (m_timers[i].GetCurrent() >= 0)
            m_timers[i].Update(diff);
        else
            m_timers[i].SetCurrent(0);
    }

    _UpdateGameTime();

    if (m_gameTime > m_NextDailyQuestReset)
    {
        ResetDailyQuests();
        m_NextDailyQuestReset += DAY;
    }

    if (m_gameTime > m_NextWeeklyQuestReset)
        ResetWeeklyQuests();

	if (m_gameTime > m_NextMonthlyQuestReset)
		ResetMonthlyQuests();

    if (m_gameTime > m_NextRandomBGReset)
        ResetRandomBG();

	if (m_gameTime > m_NextGuildReset)
		ResetGuildCap();

	if (m_bool_configs[CONFIG_EXTERNAL_MAIL])
	{
		if (m_timers[WUPDATE_EXTMAIL].Passed())
		{
			sObjectMgr->SendExternalMails();
			m_timers[WUPDATE_EXTMAIL].Reset();
		}
	}

    /// <ul><li> Handle auctions when the timer has passed
    if (m_timers[WUPDATE_AUCTIONS].Passed())
    {
        m_timers[WUPDATE_AUCTIONS].Reset();

        if (++mail_timer > mail_timer_expires)
        {
            mail_timer = 0;
            sObjectMgr->ReturnOrDeleteOldMails(true);
        }

        sAuctionMgr->Update();
    }

    /// <li> Handle session updates when the timer has passed
    RecordTimeDiff(NULL);
    UpdateSessions(diff);
    RecordTimeDiff("UpdateSessions");

    /// <li> Handle weather updates when the timer has passed
    if (m_timers[WUPDATE_WEATHERS].Passed())
    {
        m_timers[WUPDATE_WEATHERS].Reset();
        WeatherMgr::Update(uint32(m_timers[WUPDATE_WEATHERS].GetInterval()));
    }

    /// <li> Update uptime table
    if (m_timers[WUPDATE_UPTIME].Passed())
    {
        uint32 tmpDiff = uint32(m_gameTime - m_startTime);
        uint32 maxOnlinePlayers = GetMaxPlayerCount();

        m_timers[WUPDATE_UPTIME].Reset();

        PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_UPTIME_PLAYERS);

        stmt->setUInt32(0, tmpDiff);
        stmt->setUInt16(1, uint16(maxOnlinePlayers));
        stmt->setUInt32(2, realmID);
        stmt->setUInt32(3, uint32(m_startTime));

        LoginDatabase.Execute(stmt);
    }

    /// <li> Clean logs table
    if (sWorld->getIntConfig(CONFIG_LOGDB_CLEARTIME) > 0) // if not enabled, ignore the timer
    {
        if (m_timers[WUPDATE_CLEANDB].Passed())
        {
            m_timers[WUPDATE_CLEANDB].Reset();

            PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_OLD_LOGS);

            stmt->setUInt32(0, sWorld->getIntConfig(CONFIG_LOGDB_CLEARTIME));
            stmt->setUInt32(1, uint32(time(0)));

            LoginDatabase.Execute(stmt);
        }
    }

    /// <li> Handle all other objects
    ///- Update objects when the timer has passed (maps, transport, creatures, ...)
	RecordTimeDiff(NULL);
    sMapMgr->Update(diff);
	RecordTimeDiff("UpdateMapMgr");

    if (sWorld->getBoolConfig(CONFIG_AUTOBROADCAST))
    {
        if (m_timers[WUPDATE_AUTOBROADCAST].Passed())
        {
            m_timers[WUPDATE_AUTOBROADCAST].Reset();
            SendAutoBroadcast();
        }
    }

    sBattlegroundMgr->Update(diff);
    RecordTimeDiff("UpdateBattlegroundMgr");

    sOutdoorPvPMgr->Update(diff);
    RecordTimeDiff("UpdateOutdoorPvPMgr");

    ///- Delete all characters which have been deleted X days before
    if (m_timers[WUPDATE_DELETECHARS].Passed())
    {
        m_timers[WUPDATE_DELETECHARS].Reset();
        Player::DeleteOldCharacters();
    }

    sLFGMgr->Update(diff);
    RecordTimeDiff("UpdateLFGMgr");

    // execute callbacks from sql queries that were queued recently
    ProcessQueryCallbacks();
    RecordTimeDiff("ProcessQueryCallbacks");

    ///- Erase corpses once every 20 minutes
    if (m_timers[WUPDATE_CORPSES].Passed())
    {
        m_timers[WUPDATE_CORPSES].Reset();
        sObjectAccessor->RemoveOldCorpses();
    }

    ///- Process Game events when necessary
    if (m_timers[WUPDATE_EVENTS].Passed())
    {
        m_timers[WUPDATE_EVENTS].Reset();                   // to give time for Update() to be processed
        uint32 nextGameEvent = sGameEventMgr->Update();
        m_timers[WUPDATE_EVENTS].SetInterval(nextGameEvent);
        m_timers[WUPDATE_EVENTS].Reset();
    }

    ///- Ping to keep MySQL connections alive
    if (m_timers[WUPDATE_PINGDB].Passed())
    {
        m_timers[WUPDATE_PINGDB].Reset();
        sLog->outDetail("Ping MySQL to keep connection alive");
        CharacterDatabase.KeepAlive();
        LoginDatabase.KeepAlive();
        WorldDatabase.KeepAlive();
    }

    // update the instance reset times
    sInstanceSaveMgr->Update();

    // update delayed realmcomplete achievments
    sAchievementMgr->Update(diff);

    // And last, but not least handle the issued cli commands
    ProcessCliCommands();

    sScriptMgr->OnWorldUpdate(diff);
}

void World::ForceGameEventUpdate()
{
    m_timers[WUPDATE_EVENTS].Reset();                   // to give time for Update() to be processed
    uint32 nextGameEvent = sGameEventMgr->Update();
    m_timers[WUPDATE_EVENTS].SetInterval(nextGameEvent);
    m_timers[WUPDATE_EVENTS].Reset();
}

/// Send a packet to all players (except self if mentioned)
void World::SendGlobalMessage(WorldPacket* packet, WorldSession* self, uint32 team)
{
    SessionMap::const_iterator itr;
    for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (itr->second &&
            itr->second->GetPlayer() &&
            itr->second->GetPlayer()->IsInWorld() &&
            itr->second != self &&
            (team == 0 || itr->second->GetPlayer()->GetTeam() == team))
		{
			itr->second->SendPacket(packet);
        }
    }
}

/// Send a packet to all GMs (except self if mentioned)
void World::SendGlobalGMMessage(WorldPacket* packet, WorldSession* self, uint32 team)
{
    SessionMap::iterator itr;
    for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (itr->second &&
            itr->second->GetPlayer() &&
            itr->second->GetPlayer()->IsInWorld() &&
            itr->second != self &&
            !AccountMgr::IsPlayerAccount(itr->second->GetSecurity()) &&
            (team == 0 || itr->second->GetPlayer()->GetTeam() == team))
        {
            itr->second->SendPacket(packet);
        }
    }
}

namespace Trinity
{
    class WorldWorldTextBuilder
    {
        public:
            typedef std::vector<WorldPacket*> WorldPacketList;
            explicit WorldWorldTextBuilder(int32 textId, va_list* args = NULL) : i_textId(textId), i_args(args) {}
            void operator()(WorldPacketList& data_list, LocaleConstant loc_idx)
            {
                char const* text = sObjectMgr->GetQuantumSystemText(i_textId, loc_idx);

                if (i_args)
                {
                    // we need copy va_list before use or original va_list will corrupted
                    va_list ap;
                    va_copy(ap, *i_args);

                    char str[2048];
                    vsnprintf(str, 2048, text, ap);
                    va_end(ap);

                    do_helper(data_list, &str[0]);
                }
                else
                    do_helper(data_list, (char*)text);
			}
	private:
		char* lineFromMessage(char*& pos) { char* start = strtok(pos, "\n"); pos = NULL; return start; }
		void do_helper(WorldPacketList& data_list, char* text)
		{
			char* pos = text;

			while (char* line = lineFromMessage(pos))
			{
				WorldPacket* data = new WorldPacket();

				uint32 lineLength = (line ? strlen(line) : 0) + 1;

				data->Initialize(SMSG_MESSAGECHAT, 100);
				*data << uint8(CHAT_MSG_SYSTEM);
				*data << uint32(LANG_UNIVERSAL);
				*data << uint64(0);
				*data << uint32(0);
				*data << uint64(0);
				*data << uint32(lineLength);
				*data << line;
				*data << uint8(0);

				data_list.push_back(data);
			}
		}

		int32 i_textId;
		va_list* i_args;
	};
}                                                           // namespace Trinity

/// Send a System Message to all players (except self if mentioned)
void World::SendWorldText(int32 string_id, ...)
{
    va_list ap;
    va_start(ap, string_id);

    Trinity::WorldWorldTextBuilder wt_builder(string_id, &ap);
    Trinity::LocalizedPacketListDo<Trinity::WorldWorldTextBuilder> wt_do(wt_builder);
    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (!itr->second || !itr->second->GetPlayer() || !itr->second->GetPlayer()->IsInWorld())
            continue;

        wt_do(itr->second->GetPlayer());
    }

    va_end(ap);
}

/// Send a System Message to all GMs (except self if mentioned)
void World::SendGMText(int32 string_id, ...)
{
    va_list ap;
    va_start(ap, string_id);

    Trinity::WorldWorldTextBuilder wt_builder(string_id, &ap);
    Trinity::LocalizedPacketListDo<Trinity::WorldWorldTextBuilder> wt_do(wt_builder);
    for (SessionMap::iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (!itr->second || !itr->second->GetPlayer() || !itr->second->GetPlayer()->IsInWorld())
            continue;

        if (AccountMgr::IsPlayerAccount(itr->second->GetSecurity()))
            continue;

        wt_do(itr->second->GetPlayer());
    }

    va_end(ap);
}

void World::SendGlobalText(const char* text, WorldSession* self)
{
    WorldPacket data;

    char* buf = strdup(text);
    char* pos = buf;

    while (char* line = ChatHandler::LineFromMessage(pos))
    {
        ChatHandler::FillMessageData(&data, NULL, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, NULL, 0, line, NULL);
        SendGlobalMessage(&data, self);
    }

    free(buf);
}

void World::SendZoneMessage(uint32 zone, WorldPacket* packet, WorldSession* self, uint32 team)
{
    SessionMap::const_iterator itr;
    for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (itr->second &&
            itr->second->GetPlayer() &&
            itr->second->GetPlayer()->IsInWorld() &&
            itr->second->GetPlayer()->GetZoneId() == zone &&
            itr->second != self &&
            (team == 0 || itr->second->GetPlayer()->GetTeam() == team))
        {
            itr->second->SendPacket(packet);
        }
    }
}

/// Send a System Message to all players in the zone (except self if mentioned)
void World::SendZoneText(uint32 zone, const char* text, WorldSession* self, uint32 team)
{
    WorldPacket data;
    ChatHandler::FillMessageData(&data, NULL, CHAT_MSG_SYSTEM, LANG_UNIVERSAL, NULL, 0, text, NULL);
    SendZoneMessage(zone, &data, self, team);
}

/// Kick (and save) all players
void World::KickAll()
{
    m_QueuedPlayer.clear();                                 // prevent send queue update packet and login queued sessions

    // session not removed at kick and will removed in next update tick
    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        itr->second->KickPlayer();
}

/// Kick (and save) all players with security level less `sec`
void World::KickAllLess(AccountTypes sec)
{
    // session not removed at kick and will removed in next update tick
    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->GetSecurity() < sec)
            itr->second->KickPlayer();
}

/// Ban an account or ban an IP address, duration will be parsed using TimeStringToSecs if it is positive, otherwise permban
BanReturn World::BanAccount(BanMode mode, std::string nameOrIP, std::string duration, std::string reason, std::string author)
{
    uint32 duration_secs = TimeStringToSecs(duration);
    PreparedQueryResult resultAccounts = PreparedQueryResult(NULL); //used for kicking
    PreparedStatement* stmt = NULL;

    ///- Update the database with ban information
    switch (mode)
    {
        case BAN_IP:
            // No SQL injection with prepared statements
            stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_BY_IP);
            stmt->setString(0, nameOrIP);
            resultAccounts = LoginDatabase.Query(stmt);
            stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_IP_BANNED);
            stmt->setString(0, nameOrIP);
            stmt->setUInt32(1, duration_secs);
            stmt->setString(2, author);
            stmt->setString(3, reason);
            LoginDatabase.Execute(stmt);
            break;
        case BAN_ACCOUNT:
            // No SQL injection with prepared statements
            stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_ID_BY_NAME);
            stmt->setString(0, nameOrIP);
            resultAccounts = LoginDatabase.Query(stmt);
            break;
        case BAN_CHARACTER:
            // No SQL injection with prepared statements
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ACCOUNT_BY_NAME);
            stmt->setString(0, nameOrIP);
            resultAccounts = CharacterDatabase.Query(stmt);
            break;
        default:
            return BAN_SYNTAX_ERROR;
    }

    if (!resultAccounts)
    {
        if (mode == BAN_IP)
            return BAN_SUCCESS;                             // ip correctly banned but nobody affected (yet)
        else
            return BAN_NOTFOUND;                            // Nobody to ban
    }

    ///- Disconnect all affected players (for IP it can be several)
    SQLTransaction trans = LoginDatabase.BeginTransaction();
    do
    {
        Field* fieldsAccount = resultAccounts->Fetch();
        uint32 account = fieldsAccount[0].GetUInt32();

        if (mode != BAN_IP)
        {
            // make sure there is only one active ban
            stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_ACCOUNT_NOT_BANNED);
            stmt->setUInt32(0, account);
            trans->Append(stmt);
            // No SQL injection with prepared statements
            stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_ACCOUNT_BANNED);
            stmt->setUInt32(0, account);
            stmt->setUInt32(1, duration_secs);
            stmt->setString(2, author);
            stmt->setString(3, reason);
            trans->Append(stmt);
        }

        if (WorldSession* sess = FindSession(account))
            if (std::string(sess->GetPlayerName()) != author)
                sess->KickPlayer();
    }
	while (resultAccounts->NextRow());

    LoginDatabase.CommitTransaction(trans);

    return BAN_SUCCESS;
}

/// Remove a ban from an account or IP address
bool World::RemoveBanAccount(BanMode mode, std::string nameOrIP)
{
    PreparedStatement* stmt = NULL;
    if (mode == BAN_IP)
    {
        stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_IP_NOT_BANNED);
        stmt->setString(0, nameOrIP);
        LoginDatabase.Execute(stmt);
    }
    else
    {
        uint32 account = 0;
        if (mode == BAN_ACCOUNT)
            account = AccountMgr::GetId(nameOrIP);
        else if (mode == BAN_CHARACTER)
            account = sObjectMgr->GetPlayerAccountIdByPlayerName(nameOrIP);

        if (!account)
            return false;

        //NO SQL injection as account is uint32
        stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_ACCOUNT_NOT_BANNED);
        stmt->setUInt32(0, account);
        LoginDatabase.Execute(stmt);
    }
    return true;
}

/// Ban an account or ban an IP address, duration will be parsed using TimeStringToSecs if it is positive, otherwise permban
BanReturn World::BanCharacter(std::string name, std::string duration, std::string reason, std::string author)
{
    Player* pBanned = sObjectAccessor->FindPlayerByName(name.c_str());
    uint32 guid = 0;

    uint32 duration_secs = TimeStringToSecs(duration);

    /// Pick a player to ban if not online
    if (!pBanned)
    {
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUID_BY_NAME);
        stmt->setString(0, name);
        PreparedQueryResult resultCharacter = CharacterDatabase.Query(stmt);

        if (!resultCharacter)
            return BAN_NOTFOUND;                                    // Nobody to ban

        guid = (*resultCharacter)[0].GetUInt32();
    }
    else
        guid = pBanned->GetGUIDLow();

    // make sure there is only one active ban
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHARACTER_BAN);
    stmt->setUInt32(0, guid);
    CharacterDatabase.Execute(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHARACTER_BAN);
    stmt->setUInt32(0, guid);
    stmt->setUInt32(1, duration_secs);
    stmt->setString(2, author);
    stmt->setString(3, reason);
    CharacterDatabase.Execute(stmt);

    if (pBanned)
        pBanned->GetSession()->KickPlayer();

    return BAN_SUCCESS;
}

bool World::RemoveBanCharacter(std::string name)
{
    Player* pBanned = sObjectAccessor->FindPlayerByName(name.c_str());
    uint32 guid = 0;

    if (!pBanned)
    {
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUID_BY_NAME);
        stmt->setString(0, name);
        PreparedQueryResult resultCharacter = CharacterDatabase.Query(stmt);

        if (!resultCharacter)
            return false;

        guid = (*resultCharacter)[0].GetUInt32();
    }
    else
        guid = pBanned->GetGUIDLow();

    if (!guid)
        return false;

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHARACTER_BAN);
    stmt->setUInt32(0, guid);
    CharacterDatabase.Execute(stmt);
    return true;
}

/// Update the game time
void World::_UpdateGameTime()
{
    ///- update the time
    time_t thisTime = time(NULL);
    uint32 elapsed = uint32(thisTime - m_gameTime);
    m_gameTime = thisTime;

    ///- if there is a shutdown timer
    if (!IsStopped() && m_ShutdownTimer > 0 && elapsed > 0)
    {
        ///- ... and it is overdue, stop the world (set m_stopEvent)
        if (m_ShutdownTimer <= elapsed)
        {
            if (!(m_ShutdownMask & SHUTDOWN_MASK_IDLE) || GetActiveAndQueuedSessionCount() == 0)
                m_stopEvent = true;                         // exist code already set
            else
                m_ShutdownTimer = 1;                        // minimum timer value to wait idle state
        }
        else
        {
            m_ShutdownTimer -= elapsed;

            ShutdownMsg();
        }
    }
}

void World::ShutdownServ(uint32 time, uint32 options, uint8 exitcode)
{
    // ignore if server shutdown at next tick
    if (IsStopped())
        return;

    m_ShutdownMask = options;
    m_ExitCode = exitcode;

    ///- If the shutdown time is 0, set m_stopEvent (except if shutdown is 'idle' with remaining sessions)
    if (time == 0)
    {
        if (!(options & SHUTDOWN_MASK_IDLE) || GetActiveAndQueuedSessionCount() == 0)
            m_stopEvent = true;                             // exist code already set
        else
            m_ShutdownTimer = 1;                            //So that the session count is re-evaluated at next world tick
    }
    else
    {
        m_ShutdownTimer = time;
        ShutdownMsg(true);
    }
	
	sScriptMgr->OnShutdownInitiate(ShutdownExitCode(exitcode), ShutdownMask(options));
}

void World::ShutdownMsg(bool show, Player* player)
{
    if (m_ShutdownMask & SHUTDOWN_MASK_IDLE)
        return;

    if (show ||
		(m_ShutdownTimer < 5 * MINUTE && (m_ShutdownTimer % 15) == 0) ||
		(m_ShutdownTimer < 15 * MINUTE && (m_ShutdownTimer % MINUTE) == 0) ||
		(m_ShutdownTimer < 30 * MINUTE && (m_ShutdownTimer % (5 * MINUTE)) == 0) ||
        (m_ShutdownTimer < 12 * HOUR && (m_ShutdownTimer % HOUR) == 0) ||
		(m_ShutdownTimer > 12 * HOUR && (m_ShutdownTimer % (12 * HOUR)) == 0))
	{
        std::string str = secsToTimeString(m_ShutdownTimer);

        ServerMessageType msgid = (m_ShutdownMask & SHUTDOWN_MASK_RESTART) ? SERVER_MSG_RESTART_TIME : SERVER_MSG_SHUTDOWN_TIME;

        SendServerMessage(msgid, str.c_str(), player);
        sLog->outStaticDebug("Server is %s in %s", (m_ShutdownMask & SHUTDOWN_MASK_RESTART ? "restart" : "shuttingdown"), str.c_str());
    }
}

void World::ShutdownCancel()
{
    if (!m_ShutdownTimer || m_stopEvent.value())
        return;

    ServerMessageType msgid = (m_ShutdownMask & SHUTDOWN_MASK_RESTART) ? SERVER_MSG_RESTART_CANCELLED : SERVER_MSG_SHUTDOWN_CANCELLED;

    m_ShutdownMask = 0;
    m_ShutdownTimer = 0;
    m_ExitCode = SHUTDOWN_EXIT_CODE;                       // to default value
    SendServerMessage(msgid);

    sLog->outStaticDebug("Server %s cancelled.", (m_ShutdownMask & SHUTDOWN_MASK_RESTART ? "restart" : "shuttingdown"));

    sScriptMgr->OnShutdownCancel();
}

void World::SendServerMessage(ServerMessageType type, const char *text, Player* player)
{
    WorldPacket data(SMSG_SERVER_MESSAGE, 50);              // guess size
    data << uint32(type);
    if (type <= SERVER_MSG_STRING)
        data << text;

    if (player)
        player->GetSession()->SendPacket(&data);
    else
        SendGlobalMessage(&data);
}

void World::UpdateSessions(uint32 diff)
{
    WorldSession* sess = NULL;
    while (addSessQueue.next(sess))
        AddSession_ (sess);

    for (SessionMap::iterator itr = m_sessions.begin(), next; itr != m_sessions.end(); itr = next)
    {
        next = itr;
        ++next;

        WorldSession* pSession = itr->second;
        WorldSessionFilter updater(pSession);

        if (!pSession->Update(diff, updater))
        {
            if (!RemoveQueuedPlayer(itr->second) && itr->second && getIntConfig(CONFIG_INTERVAL_DISCONNECT_TOLERANCE))
                m_disconnects[itr->second->GetAccountId()] = time(NULL);
            RemoveQueuedPlayer(pSession);
            m_sessions.erase(itr);
            delete pSession;
        }
    }
}

void World::ProcessCliCommands()
{
    CliCommandHolder::Print* zprint = NULL;
    void* callbackArg = NULL;
    CliCommandHolder* command = NULL;
    while (cliCmdQueue.next(command))
    {
        sLog->outDetail("CLI command under processing...");
        zprint = command->m_print;
        callbackArg = command->m_callbackArg;
        CliHandler handler(callbackArg, zprint);
        handler.ParseCommands(command->m_command);
        if (command->m_commandFinished)
            command->m_commandFinished(callbackArg, !handler.HasSentErrorMessage());
        delete command;
    }
}

void World::SendAutoBroadcast()
{
    if (m_Autobroadcasts.empty())
        return;

    std::string msg;

    msg = Quantum::DataPackets::SelectRandomContainerElement(m_Autobroadcasts);

    uint32 abcenter = sWorld->getIntConfig(CONFIG_AUTOBROADCAST_CENTER);

    if (abcenter == 0)
        sWorld->SendWorldText(LANG_AUTO_BROADCAST, msg.c_str());

    else if (abcenter == 1)
    {
        WorldPacket data(SMSG_NOTIFICATION, (msg.size()+1));
        data << msg;
        sWorld->SendGlobalMessage(&data);
    }

    else if (abcenter == 2)
    {
        sWorld->SendWorldText(LANG_AUTO_BROADCAST, msg.c_str());

        WorldPacket data(SMSG_NOTIFICATION, (msg.size()+1));
        data << msg;
        sWorld->SendGlobalMessage(&data);
    }

    sLog->outDetail("AutoBroadcast: '%s'", msg.c_str());
}

void World::UpdateRealmCharCount(uint32 accountId)
{
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_COUNT);
    stmt->setUInt32(0, accountId);
    PreparedQueryResultFuture result = CharacterDatabase.AsyncQuery(stmt);
    m_realmCharCallbacks.insert(result);
}

void World::_UpdateRealmCharCount(PreparedQueryResult resultCharCount)
{
    if (resultCharCount)
    {
        Field* fields = resultCharCount->Fetch();
        uint32 accountId = fields[0].GetUInt32();
        uint8 charCount = uint8(fields[1].GetUInt64());

        PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_REALM_CHARACTERS_BY_REALM);
        stmt->setUInt32(0, accountId);
        stmt->setUInt32(1, realmID);
        LoginDatabase.Execute(stmt);

        stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_REALM_CHARACTERS);
        stmt->setUInt8(0, charCount);
        stmt->setUInt32(1, accountId);
        stmt->setUInt32(2, realmID);
        LoginDatabase.Execute(stmt);
    }
}

void World::InitWeeklyQuestResetTime()
{
    time_t wstime = uint64(sWorld->getWorldState(WS_WEEKLY_QUEST_RESET_TIME));
    time_t curtime = time(NULL);
    m_NextWeeklyQuestReset = wstime < curtime ? curtime : time_t(wstime);
}

void World::InitDailyQuestResetTime()
{
    time_t mostRecentQuestTime;

    QueryResult result = CharacterDatabase.Query("SELECT MAX(time) FROM character_queststatus_daily");
    if (result)
    {
        Field* fields = result->Fetch();
        mostRecentQuestTime = time_t(fields[0].GetUInt32());
    }
    else
        mostRecentQuestTime = 0;

    // client built-in time for reset is 6:00 AM
    // FIX ME: client not show day start time
    time_t curTime = time(NULL);
    tm localTm = *localtime(&curTime);
    localTm.tm_hour = 6;
    localTm.tm_min = 0;
    localTm.tm_sec = 0;

    time_t curDayResetTime = mktime(&localTm);

    time_t resetTime = (curTime < curDayResetTime) ? curDayResetTime - DAY : curDayResetTime;

    if (mostRecentQuestTime && mostRecentQuestTime <= resetTime)
        m_NextDailyQuestReset = mostRecentQuestTime;
    else
        m_NextDailyQuestReset = (curTime >= curDayResetTime) ? curDayResetTime + DAY : curDayResetTime;
}

void World::InitMonthlyQuestResetTime()
{
	time_t wstime = uint64(sWorld->getWorldState(WS_MONTHLY_QUEST_RESET_TIME));
	time_t curtime = time(NULL);
	m_NextMonthlyQuestReset = wstime < curtime ? curtime : time_t(wstime);
}

void World::InitRandomBGResetTime()
{
    time_t bgtime = uint64(sWorld->getWorldState(WS_BG_DAILY_RESET_TIME));
    if (!bgtime)
        m_NextRandomBGReset = time_t(time(NULL));         // game time not yet init

    time_t curTime = time(NULL);
    tm localTm = *localtime(&curTime);
    localTm.tm_hour = getIntConfig(CONFIG_RANDOM_BG_RESET_HOUR);
    localTm.tm_min = 0;
    localTm.tm_sec = 0;

    time_t nextDayResetTime = mktime(&localTm);

    if (curTime >= nextDayResetTime)
        nextDayResetTime += DAY;

    m_NextRandomBGReset = bgtime < curTime ? nextDayResetTime - DAY : nextDayResetTime;

    if (!bgtime)
        sWorld->setWorldState(WS_BG_DAILY_RESET_TIME, uint64(m_NextRandomBGReset));
}

void World::InitGuildResetTime()
{
	time_t gtime = uint64(getWorldState(WS_GUILD_DAILY_RESET_TIME));
    if (!gtime)
        m_NextGuildReset = time_t(time(NULL));         // game time not yet init

    // generate time by config
    time_t curTime = time(NULL);
    tm localTm = *localtime(&curTime);
    localTm.tm_hour = getIntConfig(CONFIG_GUILD_RESET_HOUR);
    localTm.tm_min = 0;
    localTm.tm_sec = 0;

    // current day reset time
    time_t nextDayResetTime = mktime(&localTm);

    // next reset time before current moment
    if (curTime >= nextDayResetTime)
        nextDayResetTime += DAY;

    // normalize reset time
    m_NextGuildReset = gtime < curTime ? nextDayResetTime - DAY : nextDayResetTime;

    if (!gtime)
		sWorld->setWorldState(WS_GUILD_DAILY_RESET_TIME, uint64(m_NextGuildReset));
}

void World::ResetDailyQuests()
{
    sLog->outDetail("Daily quests reset for all characters.");

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_RESET_CHARACTER_QUESTSTATUS_DAILY);
    CharacterDatabase.Execute(stmt);

    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->GetPlayer())
            itr->second->GetPlayer()->ResetDailyQuestStatus();

    sPoolMgr->ChangeDailyQuests();
}

void World::LoadDBAllowedSecurityLevel()
{
	PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_SEL_REALM_LIST_SECURITY_LEVEL);
	stmt->setInt32(0, int32(realmID));
	PreparedQueryResult result = LoginDatabase.Query(stmt);

    if (result)
        SetPlayerSecurityLimit(AccountTypes(result->Fetch()->GetUInt8()));
}

void World::SetPlayerSecurityLimit(AccountTypes _sec)
{
    AccountTypes sec = _sec < SEC_MAIN_ADMINISTRATOR ? _sec : SEC_PLAYER;
    bool update = sec > m_allowedSecurityLevel;
    m_allowedSecurityLevel = sec;
    if (update)
        KickAllLess(m_allowedSecurityLevel);
}

void World::ResetWeeklyQuests()
{
	sLog->outDetail("Weekly quests reset for all characters.");

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_RESET_CHARACTER_QUESTSTATUS_WEEKLY);
    CharacterDatabase.Execute(stmt);

    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->GetPlayer())
            itr->second->GetPlayer()->ResetWeeklyQuestStatus();

    m_NextWeeklyQuestReset = time_t(m_NextWeeklyQuestReset + WEEK);
    sWorld->setWorldState(WS_WEEKLY_QUEST_RESET_TIME, uint64(m_NextWeeklyQuestReset));

    sPoolMgr->ChangeWeeklyQuests();
}

void World::ResetMonthlyQuests()
{
    sLog->outDetail("Monthly quests reset for all characters.");

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_RESET_CHARACTER_QUESTSTATUS_MONTHLY);
    CharacterDatabase.Execute(stmt);

    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->GetPlayer())
            itr->second->GetPlayer()->ResetMonthlyQuestStatus();

    time_t mostRecentQuestTime = 0;

    // generate time
    time_t curTime = time(NULL);
    tm localTm = *localtime(&curTime);

    int month = localTm.tm_mon;
    int year = localTm.tm_year;

    ++month;

    // month 11 is december, next is january (0)
    if (month > 11)
    {
        month = 0;
        year += 1;
    }

    // reset time for next month
    localTm.tm_year = year;
    localTm.tm_mon = month;
    localTm.tm_mday = 1;
    localTm.tm_hour = 0;
    localTm.tm_min = 0;
    localTm.tm_sec = 0;

    time_t nextMonthResetTime = mktime(&localTm);

    // last reset time before current moment
    time_t resetTime = (curTime < nextMonthResetTime) ? nextMonthResetTime - MONTH : nextMonthResetTime;

    // need reset (if we have quest time before last reset time (not processed by some reason)
    if (mostRecentQuestTime && mostRecentQuestTime <= resetTime)
        m_NextMonthlyQuestReset = mostRecentQuestTime;
    else // plan next reset time
        m_NextMonthlyQuestReset = (curTime >= nextMonthResetTime) ? nextMonthResetTime + MONTH : nextMonthResetTime;

    sWorld->setWorldState(WS_MONTHLY_QUEST_RESET_TIME, uint64(m_NextMonthlyQuestReset));
}

void World::ResetEventSeasonalQuests(uint16 event_id)
{
    sLog->outDetail("Seasonal quests reset for all characters.");
	PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_RESET_CHARACTER_QUESTSTATUS_SEASONAL_BY_EVENT);
    stmt->setUInt16(0, event_id);
    CharacterDatabase.Execute(stmt);

    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->GetPlayer())
            itr->second->GetPlayer()->ResetSeasonalQuestStatus(event_id);
}

void World::ResetRandomBG()
{
    sLog->outDetail("Random BG status reset for all characters.");

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_BATTLEGROUND_RANDOM_ALL);
    CharacterDatabase.Execute(stmt);

    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
        if (itr->second->GetPlayer())
            itr->second->GetPlayer()->SetRandomWinner(false);

    m_NextRandomBGReset = time_t(m_NextRandomBGReset + DAY);
    sWorld->setWorldState(WS_BG_DAILY_RESET_TIME, uint64(m_NextRandomBGReset));
}

void World::ResetGuildCap()
{
	sLog->outDetail("Guild Daily Cap reset.");

	m_NextGuildReset = time_t(m_NextGuildReset + DAY);
	sWorld->setWorldState(WS_GUILD_DAILY_RESET_TIME, uint64(m_NextGuildReset));
	sGuildMgr->ResetTimes();
}

void World::UpdateMaxSessionCounters()
{
    m_maxActiveSessionCount = std::max(m_maxActiveSessionCount, uint32(m_sessions.size()-m_QueuedPlayer.size()));
    m_maxQueuedSessionCount = std::max(m_maxQueuedSessionCount, uint32(m_QueuedPlayer.size()));
}

void World::LoadDBVersion()
{
    QueryResult result = WorldDatabase.Query("SELECT db_version, script_version, cache_id FROM version LIMIT 1");
    if (result)
    {
        Field* fields = result->Fetch();
        m_DBVersion = fields[0].GetString();
		m_CreatureEventAIVersion = fields[1].GetString();

        // will be overwrite by config values if different and non-0
        m_int_configs[CONFIG_CLIENTCACHE_VERSION] = fields[2].GetUInt32();
    }

    if (m_DBVersion.empty())
        m_DBVersion = "Unknown world database.";

	if (m_CreatureEventAIVersion.empty())
		m_CreatureEventAIVersion = "Unknown creature EventAI.";
}

void World::ProcessStartEvent()
{
    isEventKillStart = true;
}

void World::ProcessStopEvent()
{
    isEventKillStart = false;
}

void World::UpdateAreaDependentAuras()
{
    SessionMap::const_iterator itr;
    for (itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
	{
		if (itr->second && itr->second->GetPlayer() && itr->second->GetPlayer()->IsInWorld())
        {
            itr->second->GetPlayer()->UpdateAreaDependentAuras(itr->second->GetPlayer()->GetAreaId());
            itr->second->GetPlayer()->UpdateZoneDependentAuras(itr->second->GetPlayer()->GetZoneId());
		}
	}
}

void World::LoadWorldStates()
{
    uint32 oldMSTime = getMSTime();

    QueryResult result = CharacterDatabase.Query("SELECT entry, value FROM worldstates");

    if (!result)
    {
		sLog->OutConsole("==========================================================");
        sLog->OutConsole(">> Loaded 0 world states. DB table `worldstates` is empty!");
		sLog->OutConsole("==========================================================");
        sLog->OutConsole();
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();
        m_worldstates[fields[0].GetUInt32()] = fields[1].GetUInt32();
        ++count;
    }
    while (result->NextRow());

	sLog->OutConsole("====================================");
    sLog->OutConsole(">> Loaded %u world states in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
	sLog->OutConsole("====================================");
    sLog->OutConsole();
}

void World::setWorldState(uint32 index, uint64 value)
{
    WorldStatesMap::const_iterator it = m_worldstates.find(index);
    if (it != m_worldstates.end())
    {
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_WORLDSTATE);

        stmt->setUInt32(0, uint32(value));
        stmt->setUInt32(1, index);

        CharacterDatabase.Execute(stmt);
    }
    else
    {
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_WORLDSTATE);

        stmt->setUInt32(0, index);
        stmt->setUInt32(1, uint32(value));

        CharacterDatabase.Execute(stmt);
    }
    m_worldstates[index] = value;
}

uint64 World::getWorldState(uint32 index) const
{
    WorldStatesMap::const_iterator it = m_worldstates.find(index);
    return it != m_worldstates.end() ? it->second : 0;
}

void World::ProcessQueryCallbacks()
{
    PreparedQueryResult result;

    while (!m_realmCharCallbacks.is_empty())
    {
        ACE_Future<PreparedQueryResult> lResult;
        ACE_Time_Value timeout = ACE_Time_Value::zero;
        if (m_realmCharCallbacks.next_readable(lResult, &timeout) != 1)
            break;

        if (lResult.ready())
        {
            lResult.get(result);
            _UpdateRealmCharCount(result);
            lResult.cancel();
        }
    }
}

void World::SendWintergraspState()
{
    OutdoorPvPWG* pvpWG =(OutdoorPvPWG*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(4197);

    if (!pvpWG)
        return;

    for (SessionMap::const_iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
    {
        if (!itr->second || !itr->second->GetPlayer() || !itr->second->GetPlayer()->IsInWorld())
            continue;

        if (pvpWG->IsWarTime()) // "Battle in progress"
        {
            itr->second->GetPlayer()->SendUpdateWorldState(ClockWorldState[1], uint32(time(NULL)));
        } 
        else // Time to next battle
        {
			pvpWG->SendInitWorldStatesTo(itr->second->GetPlayer());
            itr->second->GetPlayer()->SendUpdateWorldState(ClockWorldState[1], uint32(time(NULL) + pvpWG->GetTimer()));
            // Hide unneeded info which in center of screen
			itr->second->GetPlayer()->SendInitWorldStates(itr->second->GetPlayer()->GetZoneId(), itr->second->GetPlayer()->GetAreaId());
        }
    }
}

void World::LoadCharacterNameData()
{
	uint32 oldMSTime = getMSTime();

	QueryResult result = CharacterDatabase.Query("SELECT guid, name, race, gender, class, level FROM characters WHERE deleteDate IS NULL");
    if (!result)
    {
		sLog->OutConsole("================================================================");
        sLog->OutConsole("CHARACTER MANAGER: No characters DB table `characters` is empty!");
		sLog->OutConsole("================================================================");
		sLog->OutConsole();
        return;
    }

    uint32 count = 0;

    do
    {
		Field* fields = result->Fetch();
		AddCharacterNameData(fields[0].GetUInt32(), fields[1].GetString(),
			fields[3].GetUInt8() /*gender*/, fields[2].GetUInt8() /*race*/, fields[4].GetUInt8() /*class*/, fields[5].GetUInt8() /*level*/);
		++count;
    }
	while (result->NextRow());

	sLog->OutConsole("=======================================================");
	sLog->OutConsole("CHARACTER MANAGER: Loaded %u characters from DB in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
	sLog->OutConsole("=======================================================");
	sLog->OutConsole();
}

void World::AddCharacterNameData(uint32 guid, std::string const& name, uint8 gender, uint8 race, uint8 playerClass, uint8 level)
{
    CharacterNameData& data = _characterNameDataMap[guid];
    data.m_name = name;
    data.m_race = race;
    data.m_gender = gender;
    data.m_class = playerClass;
	data.m_level = level;
}

void World::UpdateCharacterNameData(uint32 guid, std::string const& name, uint8 gender /*= GENDER_NONE*/, uint8 race /*= RACE_NONE*/)
{
    std::map<uint32, CharacterNameData>::iterator itr = _characterNameDataMap.find(guid);
    if (itr == _characterNameDataMap.end())
        return;

    itr->second.m_name = name;

    if (gender != GENDER_NONE)
        itr->second.m_gender = gender;

    if (race != RACE_NONE)
        itr->second.m_race = race;

	WorldPacket data(SMSG_INVALIDATE_PLAYER, 8);
	data << MAKE_NEW_GUID(guid, 0, HIGHGUID_PLAYER);
	SendGlobalMessage(&data);
}

void World::UpdateCharacterNameDataLevel(uint32 guid, uint8 level)
{
	std::map<uint32, CharacterNameData>::iterator itr = _characterNameDataMap.find(guid);
	if (itr == _characterNameDataMap.end())
		return;

	itr->second.m_level = level;
}

CharacterNameData const* World::GetCharacterNameData(uint32 guid) const
{
    std::map<uint32, CharacterNameData>::const_iterator itr = _characterNameDataMap.find(guid);
    if (itr != _characterNameDataMap.end())
        return &itr->second;
    else
        return NULL;
}

void World::LoadAccounts()
{
	uint32 oldMSTime = getMSTime();

	QueryResult result = LoginDatabase.Query("SELECT id FROM account");
    if (!result)
    {
		sLog->OutConsole("=========================================================");
        sLog->OutConsole("ACCOUNT MANAGER: No accounts DB table `account` is empty!");
		sLog->OutConsole("=========================================================");
		sLog->OutConsole();
        return;
    }

    uint32 count = 0;

    do
    {
		++count;
    }
	while (result->NextRow());

	sLog->OutConsole("===================================================");
	sLog->OutConsole("ACCOUNT MANAGER: Loaded %u accounts from DB in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
	sLog->OutConsole("===================================================");
	sLog->OutConsole();
}

void World::LoadGMAccounts()
{
	uint32 oldMSTime = getMSTime();

	QueryResult result = LoginDatabase.Query("SELECT id FROM account_security");
    if (!result)
    {
		sLog->OutConsole("=====================================================================");
        sLog->OutConsole("ACCOUNT MANAGER: No GM accounts DB table `account_security` is empty!");
		sLog->OutConsole("=====================================================================");
		sLog->OutConsole();
        return;
    }

    uint32 count = 0;

    do
    {
		++count;
    }
	while (result->NextRow());

	sLog->OutConsole("======================================================");
	sLog->OutConsole("ACCOUNT MANAGER: Loaded %u GM accounts from DB in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
	sLog->OutConsole("======================================================");
	sLog->OutConsole();
}

void World::LoadPremAccounts()
{
	uint32 oldMSTime = getMSTime();

	QueryResult result = LoginDatabase.Query("SELECT id FROM account_premium");
    if (!result)
    {
		sLog->OutConsole("=========================================================================");
        sLog->OutConsole("ACCOUNT MANAGER: No premium accounts DB table `account_premium` is empty!");
		sLog->OutConsole("=========================================================================");
		sLog->OutConsole();
        return;
    }

    uint32 count = 0;

    do
    {
		++count;
    }
	while (result->NextRow());

	sLog->OutConsole("===========================================================");
	sLog->OutConsole("ACCOUNT MANAGER: Loaded %u premium accounts from DB in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
	sLog->OutConsole("===========================================================");
	sLog->OutConsole();
}

void World::LoadServerCommands()
{
	uint32 oldMSTime = getMSTime();

	QueryResult result = WorldDatabase.Query("SELECT type FROM command");
    if (!result)
    {
		sLog->OutConsole("================================================================");
        sLog->OutConsole("COMMAND MANAGER: No usable commands DB table `command` is empty!");
		sLog->OutConsole("================================================================");
		sLog->OutConsole();
        return;
    }

    uint32 count = 0;

    do
    {
		++count;
    }
	while (result->NextRow());

	sLog->OutConsole("=====================================================");
	sLog->OutConsole("COMMAND MANAGER: Loaded %u commands from DB in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
	sLog->OutConsole("=====================================================");
	sLog->OutConsole();
}

void World::LoadServerCommandRules()
{
	uint32 oldMSTime = getMSTime();

	QueryResult result = WorldDatabase.Query("SELECT rule_id FROM command");
    if (!result)
    {
		sLog->OutConsole("=======================================================================");
		sLog->OutConsole("COMMAND MANAGER: No rules for commands: WARNING COMMANDS IS VULNERABLE!");
		sLog->OutConsole("=======================================================================");
		sLog->OutConsole();
        return;
    }

    uint32 count = 0;

    do
    {
		++count;
    }
	while (result->NextRow());

	sLog->OutConsole("=====================================================");
	sLog->OutConsole("COMMAND MANAGER: Loaded %u rules from DB in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
	sLog->OutConsole("=====================================================");
	sLog->OutConsole();
}

void World::LoadCharCountFromMaxLevel()
{
	uint32 oldMSTime = getMSTime();

	QueryResult result = CharacterDatabase.Query("SELECT level FROM characters WHERE level = 80");
    if (!result)
    {
		sLog->OutConsole("========================================================");
        sLog->OutConsole("CHARACTER MANAGER: No players have reached the 80 level!");
		sLog->OutConsole("========================================================");
		sLog->OutConsole();
        return;
    }

    uint32 count = 0;

    do
    {
		++count;
    }
	while (result->NextRow());

	sLog->OutConsole("==================================================================");
	sLog->OutConsole("CHARACTER MANAGER: Loaded %u [80] level characters from DB in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
	sLog->OutConsole("==================================================================");
	sLog->OutConsole();
}

// Guild-Level-System
void World::LoadGuildBonusInfo()
{
	m_req_guildLevel_gold_1 = SelectReqGuildLevelForBonus(GUILD_BONUS_GOLD_BANK_1);
    m_req_guildLevel_xp_1 = SelectReqGuildLevelForBonus(GUILD_BONUS_XP_1);
    m_req_guildLevel_schneller_geist = SelectReqGuildLevelForBonus(GUILD_BONUS_FAST_SPIRIT);
    m_req_guildLevel_reperatur_1 = SelectReqGuildLevelForBonus(GUILD_BONUS_FOR_REPAIR_1);
    m_req_guildLevel_gold_2 = SelectReqGuildLevelForBonus(GUILD_BONUS_GOLD_BANK_2);
    m_req_guildLevel_reittempo_1 = SelectReqGuildLevelForBonus(GUILD_BONUS_MOUNT_SPEED_1);
    m_req_guildLevel_reputation_1 = SelectReqGuildLevelForBonus(GUILD_BONUS_REPUTATION_1);
    m_req_guildLevel_xp_2 = SelectReqGuildLevelForBonus(GUILD_BONUS_XP_2);
    m_req_guildLevel_reperatur_2 = SelectReqGuildLevelForBonus(GUILD_BONUS_FOR_REPAIR_2);
    m_req_guildLevel_reittempo_2 = SelectReqGuildLevelForBonus(GUILD_BONUS_MOUNT_SPEED_2);
    m_req_guildLevel_reputation_2 = SelectReqGuildLevelForBonus(GUILD_BONUS_REPUTATION_2);
    m_req_guildLevel_honor_1 = SelectReqGuildLevelForBonus(GUILD_BONUS_HONOR_1);
    m_req_guildLevel_honor_2 = SelectReqGuildLevelForBonus(GUILD_BONUS_HONOR_2);
}

uint8 World::SelectReqGuildLevelForBonus(uint8 guildBonus)
{
    PreparedStatement* stmt;
    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUILD_BONUS_INFO);
    stmt->setUInt8(0, guildBonus);
    PreparedQueryResult result = CharacterDatabase.Query(stmt);
 
    if (result)
        return (*result)[0].GetUInt8();
    else
        return 0;
}

uint8 World::GetReqGuildLevelForBonus(uint8 guildBonus)
{
    switch (guildBonus)
    {
        case GUILD_BONUS_GOLD_BANK_1:
			return m_req_guildLevel_gold_1;
			break;
		case GUILD_BONUS_GOLD_BANK_2:
			return m_req_guildLevel_gold_2;
			break;
		case GUILD_BONUS_HONOR_1:
			return m_req_guildLevel_honor_1;
			break;
		case GUILD_BONUS_HONOR_2:
			return m_req_guildLevel_honor_2;
			break;
		case GUILD_BONUS_MOUNT_SPEED_1:
			return m_req_guildLevel_reittempo_1;
			break;
		case GUILD_BONUS_MOUNT_SPEED_2:
			return m_req_guildLevel_reittempo_2;
			break;
		case GUILD_BONUS_FOR_REPAIR_1:
			return m_req_guildLevel_reperatur_1;
			break;
		case GUILD_BONUS_FOR_REPAIR_2:
			return m_req_guildLevel_reperatur_2;
			break;
		case GUILD_BONUS_REPUTATION_1:
			return m_req_guildLevel_reputation_1;
			break;
		case GUILD_BONUS_REPUTATION_2:
			return m_req_guildLevel_reputation_2;
			break;
		case GUILD_BONUS_XP_1:
			return m_req_guildLevel_xp_1;
			break;
		case GUILD_BONUS_XP_2:
			return m_req_guildLevel_xp_2;
			break;
		case GUILD_BONUS_FAST_SPIRIT:
			return m_req_guildLevel_schneller_geist;
			break;
		default:
			return 0;
			break;
	}
}