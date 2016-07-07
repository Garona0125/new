/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

#include "ScriptMgr.h"
#include "QuantumCreature.h"

enum CoilfangGOs
{
    GO_COILFANG_WATERFALL = 184212,
};

class at_coilfang_waterfall : public AreaTriggerScript
{
public:
	at_coilfang_waterfall() : AreaTriggerScript("at_coilfang_waterfall"){}

	bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
	{
		if (GameObject* go = GetClosestGameObjectWithEntry(player, GO_COILFANG_WATERFALL, 35.0f))
			if (go->getLootState() == GO_READY)
				go->UseDoorOrButton();

		return false;
	}
};

enum LegionTeleporter
{
    SPELL_TELE_A_TO        = 37387,
    QUEST_GAINING_ACCESS_A = 10589,
    SPELL_TELE_H_TO        = 37389,
    QUEST_GAINING_ACCESS_H = 10604,
};

class at_legion_teleporter : public AreaTriggerScript
{
    public:
        at_legion_teleporter() : AreaTriggerScript("at_legion_teleporter"){}

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
        {
            if (player->IsAlive() && !player->IsInCombatActive())
            {
                if (player->GetTeam() == ALLIANCE && player->GetQuestRewardStatus(QUEST_GAINING_ACCESS_A))
                {
                    player->CastSpell(player, SPELL_TELE_A_TO, false);
                    return true;
                }

                if (player->GetTeam() == HORDE && player->GetQuestRewardStatus(QUEST_GAINING_ACCESS_H))
                {
                    player->CastSpell(player, SPELL_TELE_H_TO, false);
                    return true;
                }
                return false;
            }
            return false;
        }
};

enum StormwrightShelf
{
    QUEST_STRENGTH_OF_THE_TEMPEST          = 12741,
    SPELL_CREATE_TRUE_POWER_OF_THE_TEMPEST = 53067,
};

class at_stormwright_shelf : public AreaTriggerScript
{
    public:
        at_stormwright_shelf() : AreaTriggerScript("at_stormwright_shelf"){}

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
        {
            if (!player->IsDead() && player->GetQuestStatus(QUEST_STRENGTH_OF_THE_TEMPEST) == QUEST_STATUS_INCOMPLETE)
                player->CastSpell(player, SPELL_CREATE_TRUE_POWER_OF_THE_TEMPEST, false);

            return true;
        }
};

enum ScentLarkorwi
{
    QUEST_SCENT_OF_LARKORWI = 4291,
    NPC_LARKORWI_MATE       = 9683,
};

class at_scent_larkorwi : public AreaTriggerScript
{
    public:
		at_scent_larkorwi() : AreaTriggerScript("at_scent_larkorwi"){}

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
        {
            if (!player->IsDead() && player->GetQuestStatus(QUEST_SCENT_OF_LARKORWI) == QUEST_STATUS_INCOMPLETE)
            {
                if (!player->FindCreatureWithDistance(NPC_LARKORWI_MATE, 15.0f))
                    player->SummonCreature(NPC_LARKORWI_MATE, player->GetPositionX()+5, player->GetPositionY(), player->GetPositionZ(), 3.3f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 100000);
            }
            return false;
        }
};

enum AtLastRites
{
    QUEST_LAST_RITES       = 12019,
    QUEST_BREAKING_THROUGH = 11898,
};

class at_last_rites : public AreaTriggerScript
{
    public:
        at_last_rites() : AreaTriggerScript("at_last_rites"){}

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
        {
            if (!(player->GetQuestStatus(QUEST_LAST_RITES) == QUEST_STATUS_INCOMPLETE ||
                player->GetQuestStatus(QUEST_LAST_RITES) == QUEST_STATUS_COMPLETE ||
                player->GetQuestStatus(QUEST_BREAKING_THROUGH) == QUEST_STATUS_INCOMPLETE ||
                player->GetQuestStatus(QUEST_BREAKING_THROUGH) == QUEST_STATUS_COMPLETE))
                return false;

            WorldLocation pPosition;

            switch (trigger->id)
            {
                case 5332:
                case 5338:
                    pPosition = WorldLocation(571, 3733.68f, 3563.25f, 290.812f, 3.665192f);
                    break;
                case 5334:
                    pPosition = WorldLocation(571, 3802.38f, 3585.95f, 49.5765f, 0.0f);
                    break;
                case 5340:
                    pPosition = WorldLocation(571, 3687.91f, 3577.28f, 473.342f, 0.0f);
                    break;
                default:
                    return false;
            }
            player->TeleportTo(pPosition);
            return false;
		}
};

enum Waygate
{
    SPELL_SHOLAZAR_TO_UNGORO_TELEPORT = 52056,
    SPELL_UNGORO_TO_SHOLAZAR_TELEPORT = 52057,
    AT_SHOLAZAR                       = 5046,
    AT_UNGORO                         = 5047,
    QUEST_THE_MAKERS_OVERLOOK         = 12613,
    QUEST_THE_MAKERS_PERCH            = 12559,
    QUEST_MEETING_A_GREAT_ONE         = 13956,
};

class at_sholazar_waygate : public AreaTriggerScript
{
public:
	at_sholazar_waygate() : AreaTriggerScript("at_sholazar_waygate") {}

	bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
	{
		if (!player->IsDead() && (player->GetQuestStatus(QUEST_MEETING_A_GREAT_ONE) != QUEST_STATUS_NONE ||
			(player->GetQuestStatus(QUEST_THE_MAKERS_OVERLOOK) == QUEST_STATUS_REWARDED && player->GetQuestStatus(QUEST_THE_MAKERS_PERCH) == QUEST_STATUS_REWARDED)))
		{
			switch (trigger->id)
			{
			    case AT_SHOLAZAR:
					player->CastSpell(player, SPELL_SHOLAZAR_TO_UNGORO_TELEPORT, true);
					break;
				case AT_UNGORO:
					player->CastSpell(player, SPELL_UNGORO_TO_SHOLAZAR_TELEPORT, true);
					break;
			}
		}

		return false;
	}
};

enum NatsLanding
{
    QUEST_NATS_BARGAIN = 11209,
    SPELL_FISH_PASTE   = 42644,
    NPC_LURKING_SHARK  = 23928,
};

class at_nats_landing : public AreaTriggerScript
{
    public:
        at_nats_landing() : AreaTriggerScript("at_nats_landing") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
        {
            if (!player->IsAlive() || !player->HasAura(SPELL_FISH_PASTE))
                return false;

            if (player->GetQuestStatus(QUEST_NATS_BARGAIN) == QUEST_STATUS_INCOMPLETE)
            {
                if (!player->FindCreatureWithDistance(NPC_LURKING_SHARK, 20.0f))
                {
                    if (Creature* shark = player->SummonCreature(NPC_LURKING_SHARK, -4246.243f, -3922.356f, -7.488f, 5.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 100000))
                        shark->AI()->AttackStart(player);

                    return false;
                }
            }
            return true;
        }
};

enum Brewfest
{
    NPC_TAPPER_SWINDLEKEG     = 24711,
    NPC_IPFELKOFER_IRONKEG    = 24710,

    AT_BREWFEST_DUROTAR       = 4829,
    AT_BREWFEST_DUN_MOROGH    = 4820,

    SAY_WELCOME               = 4,

    AREATRIGGER_TALK_COOLDOWN = 5,
};

class at_brewfest : public AreaTriggerScript
{
    public:
        at_brewfest() : AreaTriggerScript("at_brewfest")
        {
            TriggerTimes[AT_BREWFEST_DUROTAR] = TriggerTimes[AT_BREWFEST_DUN_MOROGH] = 0;
        }

		std::map<uint32, time_t> TriggerTimes;

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
        {
            uint32 triggerId = trigger->id;

            if (sWorld->GetGameTime() - TriggerTimes[triggerId] < AREATRIGGER_TALK_COOLDOWN)
                return false;

            switch (triggerId)
            {
                case AT_BREWFEST_DUROTAR:
                    if (Creature* tapper = player->FindCreatureWithDistance(NPC_TAPPER_SWINDLEKEG, 20.0f))
						tapper->AI()->Talk(SAY_WELCOME, player->GetGUID());
					break;
                case AT_BREWFEST_DUN_MOROGH:
                    if (Creature* ipfelkofer = player->FindCreatureWithDistance(NPC_IPFELKOFER_IRONKEG, 20.0f))
						ipfelkofer->AI()->Talk(SAY_WELCOME, player->GetGUID());
                    break;
                default:
                    break;
            }

            TriggerTimes[triggerId] = sWorld->GetGameTime();
            return false;
        }
};

enum Area52Entrance
{
    SPELL_A52_NEURALYZER  = 34400,
    NPC_SPOTLIGHT         = 19913,
    SUMMON_COOLDOWN       = 5,

    AT_AREA_52_SOUTH      = 4472,
    AT_AREA_52_NORTH      = 4466,
    AT_AREA_52_WEST       = 4471,
    AT_AREA_52_EAST       = 4422,
};

class at_area_52_entrance : public AreaTriggerScript
{
    public:
        at_area_52_entrance() : AreaTriggerScript("at_area_52_entrance")
        {
            _triggerTimes[AT_AREA_52_SOUTH] = _triggerTimes[AT_AREA_52_NORTH] = _triggerTimes[AT_AREA_52_WEST] = _triggerTimes[AT_AREA_52_EAST] = 0;
        }

        bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
        {
            float x, y, z;

            if (!player->IsAlive())
                return false;

            uint32 triggerId = trigger->id;
            if (sWorld->GetGameTime() - _triggerTimes[trigger->id] < SUMMON_COOLDOWN)
                return false;

            switch (triggerId)
            {
                case AT_AREA_52_EAST:
                    x = 3044.176f;
                    y = 3610.692f;
                    z = 143.61f;
                    break;
                case AT_AREA_52_NORTH:
                    x = 3114.87f;
                    y = 3687.619f;
                    z = 143.62f;
                    break;
                case AT_AREA_52_WEST:
                    x = 3017.79f;
                    y = 3746.806f;
                    z = 144.27f;
                    break;
                case AT_AREA_52_SOUTH:
                    x = 2950.63f;
                    y = 3719.905f;
                    z = 143.33f;
                    break;
            }

            player->SummonCreature(NPC_SPOTLIGHT, x, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 5000);
            player->AddAura(SPELL_A52_NEURALYZER, player);
            _triggerTimes[trigger->id] = sWorld->GetGameTime();
            return false;
		}
        private:
			std::map<uint32, time_t> _triggerTimes;
};

/*#####
# at_azure_dragons_sanctuary
######*/

enum AzureDragonsSanctuary
{
	QUEST_END_OF_THE_LINE_ALLIANCE = 12107,
	QUEST_END_OF_THE_LINE_HORDE    = 12110,
};

class at_azure_dragons_sanctuary : public AreaTriggerScript
{
public:
	at_azure_dragons_sanctuary() : AreaTriggerScript("at_azure_dragons_sanctuary") { }

	bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
	{
		if (player->GetQuestStatus(QUEST_END_OF_THE_LINE_ALLIANCE) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_END_OF_THE_LINE_HORDE) == QUEST_STATUS_INCOMPLETE)
		{
			player->KilledMonsterCredit(26889, 0);
			return true;
		}
		return true;
	}
};

enum ToLegionHold
{
	SPELL_CRATE_DISGUISE = 37097,

    NPC_RAZUUN           = 21502,
    NPC_JOVAAN           = 21633,
};

class at_legion_hold_smvalley : public AreaTriggerScript
{
public:
	at_legion_hold_smvalley() : AreaTriggerScript("at_legion_hold_smvalley") { }

	bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
	{
		if (player->HasAura(SPELL_CRATE_DISGUISE))
		{
			player->SummonCreature(NPC_RAZUUN, -3302.599854f, 2928.284424f, 170.921692f, 2.609816f, TEMPSUMMON_TIMED_DESPAWN, 40000);
			player->SummonCreature(NPC_JOVAAN, -3306.278076f, 2932.812500f, 170.923660f, 5.535417f, TEMPSUMMON_TIMED_DESPAWN, 40000);
			player->KilledMonsterCredit(NPC_RAZUUN, 0);
			return true;
		}
		return true;
	}
};

/*######
# at_ymiron_house
#######*/

enum YmironHouse
{
	SPELL_ECHO_YMIRON    = 42786,
	NPC_ANCIENT_VRYKUL_F = 24315,
};

class at_ymiron_house : public AreaTriggerScript
{
public:
	at_ymiron_house() : AreaTriggerScript("at_ymiron_house") { }

	bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
	{
		if (player->HasAura(SPELL_ECHO_YMIRON))
		{
			if (Creature* vrykul = player->FindCreatureWithDistance(NPC_ANCIENT_VRYKUL_F, 10.0f))
				vrykul->AI()->SetGUID(player->GetGUID());
		}
		return true;
	}
};

enum WickermanFestival
{
	QUEST_CRASHING_WICKERMAN_FESTIVAL = 1658,
};

class at_wickerman_festival : public AreaTriggerScript
{
public:
	at_wickerman_festival() : AreaTriggerScript("at_wickerman_festival") { }

	bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
	{
		player->GroupEventHappens(QUEST_CRASHING_WICKERMAN_FESTIVAL, player);
		return true;
	}
};

enum Voltarus
{
	AT_VOLTARUS_MIDDLE = 5079,
	AT_VOLTARUS_UPPER  = 5080,

	QUEST_DISCLOURE    = 12710,

	NPC_DRAKURU_SECRET = 28929,
};

class at_voltarus : public AreaTriggerScript
{
public:
	at_voltarus() : AreaTriggerScript("at_voltarus") { }

	bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
	{
		switch (trigger->id)
		{
		    case AT_VOLTARUS_MIDDLE:
				if (player->GetQuestStatus(QUEST_DISCLOURE) == QUEST_STATUS_INCOMPLETE)
				{
					player->TeleportTo(571, 6257.07f, -1962.47f, 484.78f, 3.748721f);
					player->KilledMonsterCredit(NPC_DRAKURU_SECRET, 0);
				}
				else
					player->TeleportTo(571, 6175.6f, -2008.78f, 245.255f, 1.49857f);
				break;
			case AT_VOLTARUS_UPPER:
				player->TeleportTo(571, 6159.16f, -2028.6f, 408.168f, 3.74088f);
				break;
		}
		return false;
	}
};

enum FrostgripsHollow
{
    QUEST_THE_LONESOME_WATCHER = 12877,
    
    NPC_STORMFORGED_MONITOR   = 29862,
    NPC_STORMFORGED_ERADICTOR = 29861,
    
    TYPE_WAYPOINT             = 0,
    DATA_START                = 0
};

Position const stormforgedMonitorPosition = {6963.95f, 45.65f, 818.71f, 4.948f};
Position const stormforgedEradictorPosition = {6983.18f, 7.15f, 806.33f, 2.228f};

class at_frostgrips_hollow : public AreaTriggerScript
{
public:
    at_frostgrips_hollow() : AreaTriggerScript("at_frostgrips_hollow")
    {
        StormforgedMonitorGUID = 0;
        StormforgedEradictorGUID = 0;
    }

	uint64 StormforgedMonitorGUID;
    uint64 StormforgedEradictorGUID;

    bool OnTrigger(Player* player, AreaTriggerEntry const* /* trigger */)
    {
        if (player->GetQuestStatus(QUEST_THE_LONESOME_WATCHER) != QUEST_STATUS_INCOMPLETE)
            return false;

        Creature* stormforgedMonitor = Creature::GetCreature(*player, StormforgedMonitorGUID);
        if (stormforgedMonitor)
            return false;
        
        Creature* stormforgedEradictor = Creature::GetCreature(*player, StormforgedEradictorGUID);
        if (stormforgedEradictor)
            return false;
        
        if ((stormforgedMonitor = player->SummonCreature(NPC_STORMFORGED_MONITOR, stormforgedMonitorPosition, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000)))
        {
            StormforgedMonitorGUID = stormforgedMonitor->GetGUID();
            stormforgedMonitor->SetWalk(false);
            /// The npc would search an alternative way to get to the last waypoint without this unit state.
            stormforgedMonitor->GetMotionMaster()->MovePath(NPC_STORMFORGED_MONITOR * 100, false);
        }

        if ((stormforgedEradictor = player->SummonCreature(NPC_STORMFORGED_ERADICTOR, stormforgedEradictorPosition, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000)))
        {
            StormforgedEradictorGUID = stormforgedEradictor->GetGUID();
            stormforgedEradictor->GetMotionMaster()->MovePath(NPC_STORMFORGED_ERADICTOR * 100, false);
        }
        
        return true;
    }
};

enum AncientMisc
{
    QUEST_ANCIENT_LEAF      = 7632,

    NPC_VARTRUS             = 14524,
    NPC_STOMA               = 14525,
    NPC_HASTAT              = 14526,

    CREATURE_GROUP_ANCIENTS = 1,
};

class at_ancient_leaf : public AreaTriggerScript
{
public:
	at_ancient_leaf() : AreaTriggerScript("at_ancient_leaf") { }

	bool OnTrigger(Player* player, AreaTriggerEntry const* /*trigger*/)
	{
		if (player->IsGameMaster() || !player->IsAlive())
			return false;

		if (player->GetQuestStatus(QUEST_ANCIENT_LEAF) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_ANCIENT_LEAF) == QUEST_STATUS_REWARDED)
		{
			if (GetClosestCreatureWithEntry(player, NPC_VARTRUS, 50.0f) || GetClosestCreatureWithEntry(player, NPC_STOMA, 50.0f) || GetClosestCreatureWithEntry(player, NPC_HASTAT, 50.0f))
				return true;

			player->GetMap()->SummonCreatureGroup(CREATURE_GROUP_ANCIENTS);
		}
		return false;
	}
};

void AddSC_areatrigger_scripts()
{
    new at_coilfang_waterfall();
    new at_legion_teleporter();
    new at_stormwright_shelf();
    new at_scent_larkorwi();
    new at_last_rites();
    new at_sholazar_waygate();
	new at_nats_landing();
	new at_brewfest();
	new at_area_52_entrance();
	new at_legion_hold_smvalley();
	new at_ymiron_house();
	new at_azure_dragons_sanctuary();
	new at_wickerman_festival();
	new at_voltarus();
	new at_frostgrips_hollow();
	new at_ancient_leaf();
}