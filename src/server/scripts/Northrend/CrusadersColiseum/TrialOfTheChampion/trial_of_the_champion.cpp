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
#include "QuantumGossip.h"
#include "trial_of_the_champion.h"
#include "Vehicle.h"

#define GOSSIP_START_EVENT1 "I'm ready to start challenge."
#define GOSSIP_START_EVENT2 "I'm ready for the next challenge."
#define GOSSIP_START_EVENT3 "Let's face our last challenge."

#define ORIENTATION 4.714f

const Position SpawnPosition = {746.565f, 665.056f, 411.756f, 4.77922f};
const Position GrandChampionSpawnPosition = {746.768f, 655.148f, 411.614f, 4.6842f};
const Position OutStadiumPosition = {747.03f, 687.483f, 412.373f, 1.53475f};
const Position AnnouncerPosition = {731.585f, 658.719f, 412.393f, 4.61586f};
const Position BlackKnightSpawnPosition = {796.404f, 643.282f, 466.518f, 2.33348f};

const Position FactionChampionPos[2][3] =
{
	{
	// Horde
	{724.854f, 640.344f, 411.829f, 5.60704f},
	{714.172f, 618.206f, 411.604f, 0.0f},
	{724.300f, 595.990f, 411.568f, 0.819248f},
},

{
	// Alliance
	{767.643f, 639.450f, 411.757f, 3.3663f},
	{779.169f, 618.351f, 411.607f, 3.06471f},
	{769.012f, 596.386f, 411.569f, 2.92883f},
	},
};

const Position ArgentSoldierPosition[3] =
{
    {746.875f, 650.358f, 411.569f, 4.77922f},
    {717.771f, 647.165f, 411.923f, 5.54734f},
    {775.734f, 644.413f, 411.919f, 3.79826f},
};

const Position BlackKnightCenterPos[1] =
{
	{746.482f, 623.164f, 411.417f, 4.75935f},
};

enum Texts
{
    // Grand Champions
    SAY_TIRION_INTRO_CHAMPS_1     = -1650000,
    SAY_THRALL_INTRO_CHAMPS_H     = -1650001,
    SAY_GARROSH_INTRO_CHAMPS_H    = -1650002,
    SAY_VARIAN_INTRO_CHAMPS_H     = -1650003,
    SAY_JAINA_INTRO_CHAMPS_H      = -1650004,
    SAY_VARIAN_INTRO_CHAMPS_A     = -1650005,
    SAY_JAINA_INTRO_CHAMPS_A      = -1650006,
    SAY_GARROSH_INTRO_CHAMPS_A    = -1650007,
    SAY_THRALL_INTRO_CHAMPS_A     = -1650008,
    SAY_TIRION_INTRO_CHAMPS_2     = -1650009,
	SAY_ANNOUNCER_INTRO_PLAYER_H  = -1650010,
	SAY_ANNOUNCER_INTRO_PLAYER_A  = -1650011,
    SAY_ANNOUNCER_NPC_MAGE_H      = -1650012,
	SAY_ANNOUNCER_NPC_MAGE_A      = -1650013,
    SAY_ANNOUNCER_NPC_SHAMAN_H    = -1650014,
	SAY_ANNOUNCER_NPC_SHAMAN_A    = -1650015,
    SAY_ANNOUNCER_NPC_HUNTER_H    = -1650016,
	SAY_ANNOUNCER_NPC_HUNTER_A    = -1650017,
    SAY_ANNOUNCER_NPC_WARRIOR_H   = -1650018,
	SAY_ANNOUNCER_NPC_WARRIOR_A   = -1650019,
    SAY_ANNOUNCER_NPC_ROGUE_H     = -1650020,
	SAY_ANNOUNCER_NPC_ROGUE_A     = -1650021,
    SAY_ANNOUNCER_PLAYER          = -1650022,
    EMOTE_FACTION_CHEER           = -1999900,
    SAY_TIRION_INTRO_ARGENT_1     = -1650024,
    SAY_ANNOUNCER_EADRIC          = -1650025,
    SAY_ANNOUNCER_PALETRESS       = -1650026,
    SAY_EADRIC_INTRO              = -1650027,
    SAY_PALETRESS_INTRO_1         = -1650028,
    SAY_PALETRESS_INTRO_2         = -1650029,
    SAY_TIRION_INTRO_ARGENT_2     = -1650030,
    SAY_TIRION_INTRO_BK_1         = -1650031,
    SAY_ANNOUNCER_BK              = -1650032,
    SAY_BK_INTRO_1                = -1650033,
    SAY_TIRION_INTRO_BK_2         = -1650034,
    SAY_BK_INTRO_2                = -1650035,
    SAY_BK_INTRO_3                = -1650036,
    SAY_VARIAN_INTRO_BK           = -1650037,
    SAY_GARROSH_INTRO_BK          = -1650038,
};

enum GossipTexts
{
	GOSSIP_TEXT_NOT_MOUNTED_HORDE          = 15043,
    GOSSIP_TEXT_NOT_MOUNTED_ALLIANCE       = 14757,
    GOSSIP_TEXT_FACTION_CHAMPIONS_DEFEATED = 14737,
    GOSSIP_TEXT_ARGENT_CHAMPIONS_DEFEATED  = 14738,
};

class npc_announcer_toc5 : public CreatureScript
{
public:
    npc_announcer_toc5() : CreatureScript("npc_announcer_toc5") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        InstanceScript* instance = creature->GetInstanceScript();
        uint32 gossipTextId = 0;

        if (player->IsGameMaster())
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            gossipTextId = player->GetGossipTextId(creature);
        }
        else if (instance->GetData(BOSS_GRAND_CHAMPIONS) != DONE)
        {
            if (CAST_AI(npc_announcer_toc5::npc_announcer_toc5AI, creature->AI())->AreAllPlayersMounted())
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

            gossipTextId = creature->GetEntry() == NPC_JAEREN_SUNSWORN ? GOSSIP_TEXT_NOT_MOUNTED_HORDE : GOSSIP_TEXT_NOT_MOUNTED_ALLIANCE;
        }
		else if (instance->GetData(BOSS_ARGENT_CHALLENGE_E) != DONE && instance->GetData(BOSS_ARGENT_CHALLENGE_P) != DONE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            gossipTextId = GOSSIP_TEXT_FACTION_CHAMPIONS_DEFEATED;
        }
        else
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START_EVENT3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            gossipTextId = GOSSIP_TEXT_ARGENT_CHAMPIONS_DEFEATED;
        }

        player->SEND_GOSSIP_MENU(gossipTextId, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        // Remove chest (if any) before starting anything else
        if (InstanceScript* instance = creature->GetInstanceScript())
		{
            if (GameObject* chest = GameObject::GetGameObject(*creature, instance->GetData64(DATA_CHEST)))
                chest->RemoveFromWorld();
		}

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF:
                if (creature->AI()->GetData(EVENT_INTRO) != IN_PROGRESS)
                {
                    creature->AI()->SetData(EVENT_INTRO, IN_PROGRESS);
                    creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    player->CLOSE_GOSSIP_MENU();
                }
                break;
            case GOSSIP_ACTION_INFO_DEF+1:
                if (creature->AI()->GetData(EVENT_INTRO_ARGENT) != IN_PROGRESS)
                {
                    creature->AI()->SetData(EVENT_INTRO_ARGENT, IN_PROGRESS);
                    creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    player->CLOSE_GOSSIP_MENU();
                }
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                if (creature->AI()->GetData(EVENT_BLACK_KNIGHT_INTRO) != IN_PROGRESS)
                {
                    creature->AI()->SetData(EVENT_BLACK_KNIGHT_INTRO, IN_PROGRESS);
                    creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    player->CLOSE_GOSSIP_MENU();
                }
                break;
        }
        return true;
    }

    struct npc_announcer_toc5AI : public QuantumBasicAI
    {
        npc_announcer_toc5AI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = me->GetInstanceScript();
        }

        InstanceScript* instance;
        EventMap events;

        uint32 eventIds[EVENTS_MAX];

        uint32 BossEntry[3];
        uint64 BossGUID[3];
        uint64 addsGUID[3][3];
        uint64 StalkerGUID;
        std::set<int32> announceID;
        std::set<uint64> playersGUID;

        bool addsAttacking;
        uint8 defeatedCount;

        const Position* FactionChampionPosition()
        {
            if (instance->GetData(DATA_TEAM) == HORDE)
                return FactionChampionPos[1];
            else
                return FactionChampionPos[0];
        }

        void Reset()
        {
            events.Reset();
            for (uint8 i = 0; i<EVENTS_MAX; i++)
                eventIds[i] = 0;

            for (uint8 i = 0; i<3; i++)
            {
                BossEntry[i] = 0;
                BossGUID[i] = 0;
                for (uint8 j=0; j<3; j++)
                    addsGUID[i][j] = 0;
            }

            announceID.clear();
            playersGUID.clear();
            StalkerGUID = 0;
            defeatedCount = 0;
            addsAttacking = false;
        }

        void JustSummoned(Creature* summon)
        {
            summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            summon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            summon->SetReactState(REACT_PASSIVE);
        }

        void SetData(uint32 type, uint32 data)
        {
            eventIds[type] = data;

            if (data == IN_PROGRESS)
            {
                events.ScheduleEvent(1, 0);
                if (type == EVENT_INTRO)
                {
                    Map::PlayerList const &PlList = me->GetMap()->GetPlayers();

                    if (!PlList.isEmpty())
                    {
                        for (Map::PlayerList::const_iterator itr = PlList.begin(); itr != PlList.end(); ++itr)
                        {
                            if (Player* player = itr->getSource())
                            {
                                if (player->IsGameMaster())
                                    continue;

                                playersGUID.insert(player->GetGUID());
                            }
                        }
                        for (uint8 i = 0; i<playersGUID.size(); i++)
                            announceID.insert(SAY_ANNOUNCER_PLAYER-i);
                    }

                    SetGrandChampionsForEncounter();
                }
            }
        }

        void SetGrandChampionsForEncounter()
        {
            if (instance->GetData(DATA_TEAM) == HORDE)
                BossEntry[0] = RAND(NPC_JACOB, NPC_LANA);
            else
                BossEntry[0] = RAND(NPC_MOKRA, NPC_VISCERI);

            while (BossEntry[1] == BossEntry[0] || BossEntry[2] == BossEntry[0] || BossEntry[2] == BossEntry[1])
            {
                if (instance->GetData(DATA_TEAM) == HORDE)
                {
                    BossEntry[1] = RAND(NPC_JACOB, NPC_AMBROSE, NPC_COLOSOS, NPC_JAELYNE, NPC_LANA);
                    BossEntry[2] = RAND(NPC_JACOB, NPC_AMBROSE, NPC_COLOSOS, NPC_JAELYNE, NPC_LANA);
                }
				else
                {
                    BossEntry[1] = RAND(NPC_MOKRA, NPC_ERESSEA, NPC_RUNOK, NPC_ZULTORE, NPC_VISCERI);
                    BossEntry[2] = RAND(NPC_MOKRA, NPC_ERESSEA, NPC_RUNOK, NPC_ZULTORE, NPC_VISCERI);
                }
            }
        }

        uint32 GetData(uint32 type)
        {
            return eventIds[type];
        }

        uint32 GetAddId(uint32 bossId)
        {
            switch(bossId)
            {
                // Alliance
                case NPC_JACOB:
					return NPC_STORMWIND_CHAMPION;
                case NPC_AMBROSE:
					return NPC_GNOMEREGAN_CHAMPION;
                case NPC_COLOSOS:
					return NPC_EXODAR_CHAMPION;
                case NPC_JAELYNE:
					return NPC_DARNASSUS_CHAMPION;
                case NPC_LANA:
					return NPC_IRONFORGE_CHAMPION;
                // Horde
                case NPC_MOKRA:
					return NPC_ORGRIMMAR_CHAMPION;
                case NPC_ERESSEA:
					return NPC_SILVERMOON_CHAMPION;
                case NPC_RUNOK:
					return NPC_THUNDER_BLUFF_CHAMPION;
                case NPC_ZULTORE:
					return NPC_SENJIN_CHAMPION;
                case NPC_VISCERI:
					return NPC_UNDERCITY_CHAMPION;
            }
            return 0;
        }

        void AnnounceChampion(Unit* champion)
        {
            if (champion->GetTypeId() == TYPE_ID_PLAYER)
            {
                if (!playersGUID.empty())
                {
                    if (Player* player = me->GetPlayer(*me, *playersGUID.begin()))
                    {
                        if (!announceID.empty())
                        {
                            int32 announce = Quantum::DataPackets::SelectRandomContainerElement(announceID);
                            DoSendQuantumText(announce, me, player);
                            announceID.erase(announce);
                        }
                        playersGUID.erase(*playersGUID.begin());
                        // Public emote
                        DoSendQuantumText(EMOTE_FACTION_CHEER-player->GetCurrentRace(), player);
                    }
                }
            }
            else
            {
                switch (champion->GetEntry())
                {
                    case NPC_MOKRA: // Warrior
                        DoSendQuantumText(SAY_ANNOUNCER_NPC_WARRIOR_H, me);
                        DoSendQuantumText(EMOTE_FACTION_CHEER-RACE_ORC, champion);
                        break;
                    case NPC_JACOB:
                        DoSendQuantumText(SAY_ANNOUNCER_NPC_WARRIOR_A, me);
                        DoSendQuantumText(EMOTE_FACTION_CHEER-RACE_HUMAN, champion);
                        break;
                    case NPC_ERESSEA: // Mage
                        DoSendQuantumText(SAY_ANNOUNCER_NPC_MAGE_H, me);
                        DoSendQuantumText(EMOTE_FACTION_CHEER-RACE_BLOOD_ELF, champion);
                        break;
                    case NPC_AMBROSE:
                        DoSendQuantumText(SAY_ANNOUNCER_NPC_MAGE_A, me);
                        DoSendQuantumText(EMOTE_FACTION_CHEER-RACE_GNOME, champion);
                        break;
                    case NPC_RUNOK: // Shaman
                        DoSendQuantumText(SAY_ANNOUNCER_NPC_SHAMAN_H, me);
                        DoSendQuantumText(EMOTE_FACTION_CHEER-RACE_TAUREN, champion);
                        break;
                    case NPC_COLOSOS:
                        DoSendQuantumText(SAY_ANNOUNCER_NPC_SHAMAN_A, me);
                        DoSendQuantumText(EMOTE_FACTION_CHEER-RACE_DRAENEI, champion);
                        break;
                    case NPC_JAELYNE: // Hunter
                        DoSendQuantumText(SAY_ANNOUNCER_NPC_HUNTER_A, me);
                        DoSendQuantumText(EMOTE_FACTION_CHEER-RACE_NIGHT_ELF, champion);
                        break;
                    case NPC_ZULTORE:
                        DoSendQuantumText(SAY_ANNOUNCER_NPC_HUNTER_H, me);
                        DoSendQuantumText(EMOTE_FACTION_CHEER-RACE_TROLL, champion);
                        break;
                    case NPC_VISCERI: // Rogue
                        DoSendQuantumText(SAY_ANNOUNCER_NPC_ROGUE_H, me);
                        DoSendQuantumText(EMOTE_FACTION_CHEER-RACE_UNDEAD_PLAYER, champion);
                        break;
                    case NPC_LANA:
                        DoSendQuantumText(SAY_ANNOUNCER_NPC_ROGUE_A, me);
                        DoSendQuantumText(EMOTE_FACTION_CHEER-RACE_DWARF, champion);
                        break;
                    case NPC_EADRIC_THE_PURE:
                        DoSendQuantumText(SAY_ANNOUNCER_EADRIC, me);
                        DoSendQuantumText(EMOTE_FACTION_CHEER, champion);
                        break;
                    case NPC_ARGENT_PALETRESS:
                        DoSendQuantumText(SAY_ANNOUNCER_PALETRESS, me);
                        DoSendQuantumText(EMOTE_FACTION_CHEER, champion);
                        break;
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);

            if (GetData(EVENT_INTRO) == IN_PROGRESS)
            {
                switch(events.ExecuteEvent())
                {
                    case 1:
						if (me->GetEntry() == NPC_JAEREN_SUNSWORN)
							DoSendQuantumText(SAY_ANNOUNCER_INTRO_PLAYER_H, me);
						else
							DoSendQuantumText(SAY_ANNOUNCER_INTRO_PLAYER_A, me);

                        events.ScheduleEvent(2, 7000);
                        break;
                    case 2:
						// RETURN VALUE
                        if (!playersGUID.empty())
                        {
                            if (Player* player = me->GetMap()->GetPlayers().begin()->getSource())
                                AnnounceChampion(player);

                            events.ScheduleEvent(2, 8000);
                        }
						else
                            events.ScheduleEvent(3, 8000);
                        break;
                    case 3:
                        if (Creature* tirion = me->GetCreature(*me, instance->GetData64(DATA_TIRION)))
							DoSendQuantumText(SAY_TIRION_INTRO_CHAMPS_1, tirion);

                        events.ScheduleEvent(4, 8000);
                        break;
                    case 4:
                        if (instance->GetData(DATA_TEAM) == HORDE)
                        {
                            if (Creature* thrall = me->GetCreature(*me, instance->GetData64(DATA_THRALL)))
								DoSendQuantumText(SAY_THRALL_INTRO_CHAMPS_H, thrall);
							events.ScheduleEvent(5, 5000);
                        }
						else
                        {
                            if (Creature* varian = me->GetCreature(*me, instance->GetData64(DATA_VARIAN)))
                                DoSendQuantumText(SAY_VARIAN_INTRO_CHAMPS_A, varian);
                            events.ScheduleEvent(5, 8000);
                        }
                        break;
                    case 5:
                        if (instance->GetData(DATA_TEAM) == HORDE)
                        {
                            if (Creature* garrosh = me->GetCreature(*me, instance->GetData64(DATA_GARROSH)))
                                DoSendQuantumText(SAY_GARROSH_INTRO_CHAMPS_H, garrosh);
                            events.ScheduleEvent(6, 8000);
                        }
						else
                        {
                            if (Creature* jaina = me->GetCreature(*me, instance->GetData64(DATA_JAINA)))
                                DoSendQuantumText(SAY_JAINA_INTRO_CHAMPS_A, jaina);
                            events.ScheduleEvent(6, 5000);
                        }
                        break;
                    case 6:
                        if (instance->GetData(DATA_TEAM) == HORDE)
                        {
                            if (Creature* varian = me->GetCreature(*me, instance->GetData64(DATA_VARIAN)))
                                DoSendQuantumText(SAY_VARIAN_INTRO_CHAMPS_H, varian);
                            events.ScheduleEvent(7, 8000);
                        }
						else
                        {
                            if (Creature* garrosh = me->GetCreature(*me, instance->GetData64(DATA_GARROSH)))
                                DoSendQuantumText(SAY_GARROSH_INTRO_CHAMPS_A, garrosh);
                            events.ScheduleEvent(7, 8000);
                        }
                        break;
                    case 7:
                        if (instance->GetData(DATA_TEAM) == HORDE)
                        {
                            if (Creature* jaina = me->GetCreature(*me, instance->GetData64(DATA_JAINA)))
                                DoSendQuantumText(SAY_JAINA_INTRO_CHAMPS_H, jaina);
                            events.ScheduleEvent(8, 5000);
                        }
						else
                        {
                            if (Creature* thrall = me->GetCreature(*me, instance->GetData64(DATA_THRALL)))
                                DoSendQuantumText(SAY_THRALL_INTRO_CHAMPS_A, thrall);
                            events.ScheduleEvent(8, 5000);
                        }
                        break;
                    case 8:
                        if (Creature* tirion = me->GetCreature(*me, instance->GetData64(DATA_TIRION)))
							DoSendQuantumText(SAY_TIRION_INTRO_CHAMPS_2, tirion);

                        // Open door
                        if (GameObject* go = GameObject::GetGameObject(*me, instance->GetData64(DATA_MAIN_GATE)))
                            instance->HandleGameObject(go->GetGUID(), true);

                        events.ScheduleEvent(9, 7000);
                        break;
                    case 9:
                        // Summon invisible trigger for orientation prupouses only
                        if (Creature* stalker = me->SummonCreature(20562, me->GetPositionX() , me->GetPositionY(), me->GetPositionZ(), me->GetOrientation()))
                            StalkerGUID = stalker->GetGUID();

                        // Summon 1st Boss and adds and make them follow him
                        if (Creature* boss = me->SummonCreature(BossEntry[0], SpawnPosition))
                        {
                            BossGUID[0] = boss->GetGUID();
                            boss->SetTarget(StalkerGUID);
                            AnnounceChampion(boss->ToUnit());

                            for (uint8 i = 0; i < 3; ++i)
                            {
                                if (Creature* add = me->SummonCreature(GetAddId(boss->GetEntry()), SpawnPosition, TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    add->SetTarget(StalkerGUID);
                                    addsGUID[0][i] = add->GetGUID();
                                    switch(i)
                                    {
                                        case 0:
											add->GetMotionMaster()->MoveFollow(boss, 2.0f, M_PI);
											break;
                                        case 1:
											add->GetMotionMaster()->MoveFollow(boss, 2.0f, M_PI / 2);
											break;
                                        case 2:
											add->GetMotionMaster()->MoveFollow(boss, 2.0f, M_PI / 2 + M_PI);
											break;
                                    }
                                }
                            }
                        }
                        events.ScheduleEvent(10, 2000);
                        break;
                    case 10:
                        // Move first boss to the new position
                        if (Creature* boss1 = me->GetCreature(*me, BossGUID[0]))
                            boss1->GetMotionMaster()->MovePoint(0, FactionChampionPosition()[0]);

                        events.ScheduleEvent(11, 5000);
                        break;
                    case 11:
                        // Refresh the adds position
                        for (uint8 i = 0; i<1; i++)
                        {
                            if (Creature* boss = me->GetCreature(*me, BossGUID[i]))
                            {
                                boss->SetFacingToObject(me);
                                for (uint8 j = 0; j<3; j++)
                                {
                                    if (Creature* add = me->GetCreature(*me, addsGUID[i][j]))
                                    {
                                        switch(j)
                                        {
                                            case 0:
												add->GetMotionMaster()->MoveFollow(boss, 2.0f, M_PI);
												break;
                                            case 1:
												add->GetMotionMaster()->MoveFollow(boss, 2.0f, M_PI / 2);
												break;
                                            case 2:
												add->GetMotionMaster()->MoveFollow(boss, 2.0f, M_PI / 2 + M_PI);
												break;
                                        }
                                    }
                                }
                            }
                        }
                        events.ScheduleEvent(12, 3000);
                        break;
                    case 12:
                        // Summon 2nd Boss and adds and make them follow him
                        if (Creature* boss = me->SummonCreature(BossEntry[1], SpawnPosition))
                        {
                            BossGUID[1] = boss->GetGUID();
                            boss->SetTarget(StalkerGUID);
                            AnnounceChampion(boss->ToUnit());

                            for (uint8 i = 0; i < 3; ++i)
                            {
                                if (Creature* add = me->SummonCreature(GetAddId(boss->GetEntry()), SpawnPosition, TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    add->SetTarget(StalkerGUID);
                                    addsGUID[1][i] = add->GetGUID();
                                    switch(i)
                                    {
                                        case 0:
											add->GetMotionMaster()->MoveFollow(boss, 2.0f, M_PI);
											break;
                                        case 1:
											add->GetMotionMaster()->MoveFollow(boss, 2.0f, M_PI / 2);
											break;
                                        case 2:
											add->GetMotionMaster()->MoveFollow(boss, 2.0f, M_PI / 2 + M_PI);
											break;
                                    }
                                }
                            }
                        }
                        events.ScheduleEvent(13, 5000);
                        break;
                    case 13:
                        // Move first boss to the new position
                        if (Creature* boss1 = me->GetCreature(*me, BossGUID[0]))
                            boss1->GetMotionMaster()->MovePoint(0, FactionChampionPosition()[1]);

                        // Move second boss to the new position
                        if (Creature* boss2 = me->GetCreature(*me, BossGUID[1]))
                            boss2->GetMotionMaster()->MovePoint(0, FactionChampionPosition()[0]);

                        events.ScheduleEvent(14, 5000);
                        break;
                    case 14:
                        // Refresh the adds position
                        for (uint8 i = 0; i<2; i++)
                        {
                            if (Creature* boss = me->GetCreature(*me, BossGUID[i]))
                            {
                                boss->SetFacingToObject(me);
                                for (uint8 j = 0; j<3; j++)
                                {
                                    if (Creature* add = me->GetCreature(*me, addsGUID[i][j]))
                                    {
                                        switch(j)
                                        {
                                            case 0:
												add->GetMotionMaster()->MoveFollow(boss, 2.0f, M_PI);
												break;
                                            case 1:
												add->GetMotionMaster()->MoveFollow(boss, 2.0f, M_PI / 2);
												break;
                                            case 2:
												add->GetMotionMaster()->MoveFollow(boss, 2.0f, M_PI / 2 + M_PI);
												break;
                                        }
                                    }
                                }
                            }
                        }
                        events.ScheduleEvent(15, 4000);
                        break;
                    case 15:
                        // Summon 3rd Boss and adds and make them follow him
                        if (Creature* boss = me->SummonCreature(BossEntry[2], SpawnPosition))
                        {
                            BossGUID[2] = boss->GetGUID();
                            boss->SetTarget(StalkerGUID);
                            AnnounceChampion(boss->ToUnit());

                            for (uint8 i = 0; i < 3; ++i)
                            {
                                if (Creature* add = me->SummonCreature(GetAddId(boss->GetEntry()), SpawnPosition, TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    add->SetTarget(StalkerGUID);
                                    addsGUID[2][i] = add->GetGUID();
                                    switch(i)
                                    {
                                        case 0:
											add->GetMotionMaster()->MoveFollow(boss, 2.0f, M_PI);
											break;
                                        case 1:
											add->GetMotionMaster()->MoveFollow(boss, 2.0f, M_PI / 2);
											break;
                                        case 2:
											add->GetMotionMaster()->MoveFollow(boss, 2.0f, M_PI / 2 + M_PI);
											break;
                                    }
                                }
                            }
                        }
                        events.ScheduleEvent(16, 4000);
                        break;
                    case 16:
                        // Move first boss to the new position
                        if (Creature* boss1 = me->GetCreature(*me, BossGUID[0]))
                            boss1->GetMotionMaster()->MovePoint(0, FactionChampionPosition()[2]);

                        // Move second boss to the new position
                        if (Creature* boss2 = me->GetCreature(*me, BossGUID[1]))
                            boss2->GetMotionMaster()->MovePoint(0, FactionChampionPosition()[1]);

                        // Move third boss to the new position
                        if (Creature* boss2 = me->GetCreature(*me, BossGUID[2]))
                            boss2->GetMotionMaster()->MovePoint(0, FactionChampionPosition()[0]);

                        events.ScheduleEvent(17, 5000);
                        break;
                    case 17:
                        // Refresh the adds position
                        for (uint8 i = 0; i<3; i++)
                        {
                            if (Creature* boss = me->GetCreature(*me, BossGUID[i]))
                            {
                                boss->SetFacingToObject(me);
                                for (uint8 j = 0; j<3; j++)
                                {
                                    if (Creature* add = me->GetCreature(*me, addsGUID[i][j]))
                                    {
                                        switch(j)
                                        {
                                            case 0:
												add->GetMotionMaster()->MoveFollow(boss, 2.0f, M_PI);
												break;
                                            case 1:
												add->GetMotionMaster()->MoveFollow(boss, 2.0f, M_PI / 2);
												break;
                                            case 2:
												add->GetMotionMaster()->MoveFollow(boss, 2.0f, M_PI / 2 + M_PI);
												break;
                                        }
                                    }
                                }
                            }
                        }
                        events.ScheduleEvent(18, 4000);
                        break;
                    case 18:
                        // Set home positions, in case of wipe, this avoids summons to go to the SpawnPos
                        for (uint8 i = 0; i<3; i++)
                            if (Creature* boss = me->GetCreature(*me, BossGUID[i]))
                            {
                                boss->SetTarget(0);
                                boss->CastSpell(boss, SPELL_MOUNT_LANCE_STAND, true);
                                boss->SetHomePosition(boss->GetPositionX(), boss->GetPositionY(), boss->GetPositionZ(), boss->GetOrientation());
                            }

							for (uint8 i = 0; i<3; i++)
								for (uint8 j=0; j<3; j++)
									if (Creature* add = me->GetCreature(*me, addsGUID[i][j]))
									{
										add->SetTarget(0);
										add->SetFacingToObject(me);
										add->CastSpell(add, SPELL_MOUNT_LANCE_STAND, true);
										add->SetHomePosition(add->GetPositionX(), add->GetPositionY(), add->GetPositionZ(), add->GetOrientation());
									}
									// Move to the door position
									me->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
									me->GetMotionMaster()->MovePoint(0, AnnouncerPosition);
									me->SetTarget(StalkerGUID);;
									events.ScheduleEvent(19, 19000);
									break;
					case 19:
						//Close Door
						if (GameObject* go = GameObject::GetGameObject(*me, instance->GetData64(DATA_MAIN_GATE)))
                            instance->HandleGameObject(go->GetGUID(), false);

                        events.Reset();
                        SetData(EVENT_INTRO, DONE);
                        SetData(EVENT_WAVES, IN_PROGRESS);
                        break;
                }
            }

            if (GetData(EVENT_WAVES) == IN_PROGRESS)
            {
                switch (events.ExecuteEvent())
                {
                    case 1:
                        // Future texts?
                        events.ScheduleEvent(2, 3000);
                        break;
                    case 2:
                        // Start attack of first wave of adds
                        if (!addsAttacking)
                        {
                            for (uint8 i = 0; i<3; i++)
                            {
                                if (Creature* add = me->GetCreature(*me, addsGUID[0][i]))
                                {
                                    add->RemoveAurasDueToSpell(SPELL_MOUNT_LANCE_STAND);
                                    add->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                    add->SetReactState(REACT_AGGRESSIVE);
									add->SetCurrentFaction(16);
									AggroAllPlayers(add);
                                }
                            }
                            addsAttacking = true;
                            events.ScheduleEvent(2, 1000);
                        }
						else // Wait for the death of all of them
                        {
                            defeatedCount = 0;
                            for (uint8 i = 0; i<3; i++)
                                if (Creature* add = me->GetCreature(*me, addsGUID[0][i]))
                                    if (add->IsDead())
                                        defeatedCount++;

                            if (defeatedCount>=3)
                            {
                                defeatedCount = 0;
                                addsAttacking = false;
                                events.ScheduleEvent(3, 4000);
                            }
							else
                                events.ScheduleEvent(2, 1000);
                        }
                        break;
                    case 3:
                        // Despawn previous wave
                        for (uint8 i = 0; i<3; i++)
                            if (Creature* add = me->GetCreature(*me, addsGUID[0][i]))
                                add->DespawnAfterAction();

                        // Start attack of second wave of adds
                        if (!addsAttacking)
                        {
                            for (uint8 i = 0; i<3; i++)
                            {
                                if (Creature* add = me->GetCreature(*me, addsGUID[1][i]))
                                {
                                    add->RemoveAurasDueToSpell(SPELL_MOUNT_LANCE_STAND);
                                    add->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                    add->SetReactState(REACT_AGGRESSIVE);
									add->SetCurrentFaction(16);
									AggroAllPlayers(add);
                                }
                            }
                            addsAttacking = true;
                            events.ScheduleEvent(3, 1000);
                        }
						else // Wait for the death of all of them
                        {
                            defeatedCount = 0;
                            for (uint8 i = 0; i<3; i++)
                                if (Creature* add = me->GetCreature(*me, addsGUID[1][i]))
                                    if (add->IsDead())
                                        defeatedCount++;

                            if (defeatedCount>=3)
                            {
                                defeatedCount = 0;
                                addsAttacking = false;
                                events.ScheduleEvent(4, 4000);
                            }
							else
                                events.ScheduleEvent(3, 1000);
                        }
                        break;
                    case 4:
                        // Despawn previous wave
                        for (uint8 i = 0; i<3; i++)
                            if (Creature* add = me->GetCreature(*me, addsGUID[1][i]))
                                add->DespawnAfterAction();

                        // Start attack of third wave of adds
                        if (!addsAttacking)
                        {
                            for (uint8 i = 0; i<3; i++)
                            {
                                if (Creature* add = me->GetCreature(*me, addsGUID[2][i]))
                                {
                                    add->RemoveAurasDueToSpell(SPELL_MOUNT_LANCE_STAND);
                                    add->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                    add->SetReactState(REACT_AGGRESSIVE);
									add->SetCurrentFaction(16);
									AggroAllPlayers(add);
                                }
                            }
                            addsAttacking = true;
                            events.ScheduleEvent(4, 1000);
                        }
						else // Wait for the death of all of them
                        {
                            defeatedCount = 0;
                            for (uint8 i = 0; i<3; i++)
                                if (Creature* add = me->GetCreature(*me, addsGUID[2][i]))
                                    if (add->IsDead())
                                        defeatedCount++;

                            if (defeatedCount>=3)
                            {
                                defeatedCount = 0;
                                addsAttacking = false;
                                events.ScheduleEvent(5, 4000);
                            }
							else
                                events.ScheduleEvent(4, 1000);
                        }
                        break;
                    case 5:
                        // Despawn previous wave
                        for (uint8 i = 0; i<3; i++)
                            if (Creature* add = me->GetCreature(*me, addsGUID[2][i]))
                                add->DespawnAfterAction();

                        // Start attack of wave of bosses
                        if (!addsAttacking)
                        {
                            for (uint8 i = 0; i<3; i++)
                            {
                                if (Creature* add = me->GetCreature(*me, BossGUID[i]))
                                {
                                    add->RemoveAurasDueToSpell(SPELL_MOUNT_LANCE_STAND);
                                    add->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                    add->SetReactState(REACT_AGGRESSIVE);
									add->SetCurrentFaction(16);
									AggroAllPlayers(add);
                                }
                            }
                            addsAttacking = true;
                            events.ScheduleEvent(5, 1000);
                        }
						else // Wait for the death of all of them
                        {
                            defeatedCount = 0;
                            for (uint8 i = 0; i<3; i++)
                                if (Creature* add = me->GetCreature(*me, BossGUID[i]))
                                    if (add->AI()->GetData(DATA_CHAMPION_DEFEATED) == 1)
                                        defeatedCount++;

                            if (defeatedCount>=3)
                            {
                                defeatedCount = 0;
                                addsAttacking = false;
                                events.ScheduleEvent(6, 0);
                            }
							else
                                events.ScheduleEvent(5, 1000);
                        }
                        break;
                    case 6:
                        // Despawn boss wave
                        for (uint8 i = 0; i<3; i++)
                            if (Creature* add = me->GetCreature(*me, BossGUID[i]))
                                add->DespawnAfterAction();

                        events.Reset();
                        SetData(EVENT_WAVES, DONE);
                        SetData(EVENT_CHAMPIONS, IN_PROGRESS);
                        instance->SetData(MINION_ENCOUNTER, DONE);
                        instance->SetData(BOSS_GRAND_CHAMPIONS, SPECIAL);
                        break;
                }
            }

            if (GetData(EVENT_CHAMPIONS) == IN_PROGRESS)
            {
                switch(events.ExecuteEvent())
                {
                    case 1:
                        AreAllPlayersMounted(true);
                        events.ScheduleEvent(2, 0);
                        break;
                    case 2:
                        for (uint8 i = 0; i<3; i++)
                        {
                            if (Creature* boss = me->SummonCreature(BossEntry[i], GrandChampionSpawnPosition))
                            {
                                BossGUID[i] = boss->GetGUID();
                                boss->SetTarget(StalkerGUID);
								//we force faction due to interfaction
								boss->SetCurrentFaction(16);
                                // Save current boss entry to inst for achievement check
                                instance->SetData64(DATA_GRAND_CHAMPION_GUID, boss->GetGUID());
                                instance->SetData(DATA_GRAND_CHAMPION_ENTRY, boss->GetEntry());
                                // Prevent bosses from falling down the ground
                                boss->SetPosition(boss->GetPositionX(), boss->GetPositionY(), boss->GetPositionZ()+0.1f, boss->GetOrientation());
                                // Set positions
                                switch(i)
                                {
                                    case 1:
										boss->GetMotionMaster()->MoveFollow(me->GetCreature(*me, BossGUID[0]), 5.0f, (M_PI / 2) + 0.5f);
										break;
                                    case 2:
										boss->GetMotionMaster()->MoveFollow(me->GetCreature(*me, BossGUID[0]), 5.0f, (M_PI / 2 + M_PI) - 0.5f);
										break;
                                }
                            }
                        }
                        events.ScheduleEvent(3, 2000);
                        break;
                    case 3:
                        for (uint8 i = 0; i<3; i++)
						{
                            if (Creature* boss = me->GetCreature(*me, BossGUID[i]))
                            {
                                if (Creature* tirion = me->GetCreature(*me, instance->GetData64(DATA_TIRION)))
                                    boss->SetFacingToObject(tirion);
                                boss->SetHomePosition(boss->GetPositionX(), boss->GetPositionY(), boss->GetPositionZ() + 1.0f, boss->GetOrientation());
                                boss->SetReactState(REACT_AGGRESSIVE);
                                boss->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                            }
						}
                        events.ScheduleEvent(4, 10000);
                        break;
                    case 4: // Wait for the death of all of them
                        if (instance->GetData(BOSS_GRAND_CHAMPIONS) == IN_PROGRESS)
                        {
                            for (uint8 i = 0; i<3; i++)
							{
                                if (Creature* boss = me->GetCreature(*me, BossGUID[i]))
								{
                                    if (!boss->IsInCombatActive())
										AggroAllPlayers(boss);
								}
							}
                        }

                        defeatedCount = 0;
                        for (uint8 i = 0; i<3; i++)
                            if (Creature* boss = me->GetCreature(*me, BossGUID[i]))
                                if (boss->AI()->GetData(DATA_CHAMPION_DEFEATED) == 1)
                                    defeatedCount++;

                        if (defeatedCount>=3)
                        {
                            defeatedCount = 0;
                            addsAttacking = false;
                            events.ScheduleEvent(5, 0);
                        }
						else
                            events.ScheduleEvent(4, 1000);
                        break;
                    case 5:
                        for (uint8 i = 0; i<3; i++)
						{
                            if (Creature* boss = me->GetCreature(*me, BossGUID[i]))
                            {
                                boss->GetMotionMaster()->MovePoint(0, OutStadiumPosition);
                                boss->DespawnAfterAction(4*IN_MILLISECONDS);
                            }
						}
                        SetData(EVENT_CHAMPIONS, DONE);
                        instance->SetData(BOSS_GRAND_CHAMPIONS, DONE);
                        break;
                }
            }

            if (GetData(EVENT_INTRO_ARGENT) == IN_PROGRESS)
            {
                switch(events.ExecuteEvent())
                {
                    case 1:
                        if (Creature* tirion = me->GetCreature(*me, instance->GetData64(DATA_TIRION)))
                            DoSendQuantumText(SAY_TIRION_INTRO_ARGENT_1, tirion);
                        // Open door
                        if (GameObject* go = GameObject::GetGameObject(*me, instance->GetData64(DATA_MAIN_GATE)))
                            instance->HandleGameObject(go->GetGUID(), true);
                        events.ScheduleEvent(2, 2000);
                        break;
                    case 2:
                        // Summon 1st wave
                        // The main add will move, and the two other will only follow him
                        if (Creature* mainAdd = me->SummonCreature(NPC_ARGENT_LIGHWIELDER, SpawnPosition))
                        {
                            addsGUID[0][0] = mainAdd->GetGUID();
                            mainAdd->SetTarget(StalkerGUID);

                            for (uint8 i = 0; i < 2; ++i)
                            {
                                uint32 addEntry = 0;
                                switch(i)
                                {
                                    case 0:
										addEntry = NPC_ARGENT_MONK;
										break;
                                    case 1:
										addEntry = NPC_ARGENT_PRIESTESS;
										break;
                                }

                                if (Creature* add = me->SummonCreature(addEntry, SpawnPosition, TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    add->SetTarget(StalkerGUID);
                                    addsGUID[0][i+1] = add->GetGUID();
                                    switch(i)
                                    {
                                        case 0:
											add->GetMotionMaster()->MoveFollow(mainAdd, 1.0f, (M_PI / 2) + 0.5f);
											break;
                                        case 1:
											add->GetMotionMaster()->MoveFollow(mainAdd, 1.0f, (M_PI / 2 + M_PI) - 0.5f);
											break;
                                    }
                                }
                            }
                        }
                        events.ScheduleEvent(3, 2000);
                        break;
                    case 3:
                        // Move first wave to his position
                        if (Creature* mainAdd1 = me->GetCreature(*me, addsGUID[0][0]))
                            mainAdd1->GetMotionMaster()->MovePoint(0, ArgentSoldierPosition[0]);

                        events.ScheduleEvent(4, 3000);
                        break;
                    case 4:
                        // Refresh the adds position
                        for (uint8 i = 0; i<1; i++)
                        {
                            if (Creature* mainAdd = me->GetCreature(*me, addsGUID[i][0]))
                            {
                                mainAdd->SetFacingToObject(me);
                                for (uint8 j = 0; j<3; j++)
                                {
                                    if (Creature* add = me->GetCreature(*me, addsGUID[i][j]))
                                    {
                                        switch(j)
                                        {
                                            case 1:
												add->GetMotionMaster()->MoveFollow(mainAdd, 1.0f, (M_PI / 2) + 0.5f);
												break;
                                            case 2:
												add->GetMotionMaster()->MoveFollow(mainAdd, 1.0f, (M_PI / 2 + M_PI) - 0.5f);
												break;
                                        }
                                    }
                                }
                            }
                        }
                        events.ScheduleEvent(5, 3000);
                        break;
                    case 5:
                        // Summon 2nd wave
                        if (Creature* mainAdd = me->SummonCreature(NPC_ARGENT_LIGHWIELDER, SpawnPosition))
                        {
                            addsGUID[1][0] = mainAdd->GetGUID();
                            mainAdd->SetTarget(StalkerGUID);

                            for (uint8 i = 0; i < 2; ++i)
                            {
                                uint32 addEntry = 0;
                                switch(i)
                                {
                                    case 0:
										addEntry = NPC_ARGENT_MONK;
										break;
                                    case 1:
										addEntry = NPC_ARGENT_PRIESTESS;
										break;
                                }

                                if (Creature* add = me->SummonCreature(addEntry, SpawnPosition, TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    add->SetTarget(StalkerGUID);
                                    addsGUID[1][i+1] = add->GetGUID();
                                    switch(i)
                                    {
                                        case 0:
											add->GetMotionMaster()->MoveFollow(mainAdd, 1.0f, (M_PI / 2) + 0.5f);
											break;
                                        case 1:
											add->GetMotionMaster()->MoveFollow(mainAdd, 1.0f, (M_PI / 2 + M_PI) - 0.5f);
											break;
                                    }
                                }
                            }
                        }
                        events.ScheduleEvent(6, 5000);
                        break;
                    case 6:
                        // Move first wave to the new position
                        if (Creature* mainAdd1 = me->GetCreature(*me, addsGUID[0][0]))
                            mainAdd1->GetMotionMaster()->MovePoint(0, ArgentSoldierPosition[1]);

                        // Move second wave to the new position
                        if (Creature* mainAdd2 = me->GetCreature(*me, addsGUID[1][0]))
                            mainAdd2->GetMotionMaster()->MovePoint(0, ArgentSoldierPosition[0]);

                        events.ScheduleEvent(7, 4000);
                        break;
                    case 7:
                        // Refresh the adds position
                        for (uint8 i = 0; i<2; i++)
                        {
                            if (Creature* mainAdd = me->GetCreature(*me, addsGUID[i][0]))
                            {
                                mainAdd->SetFacingToObject(me);
                                for (uint8 j = 0; j<3; j++)
                                {
                                    if (Creature* add = me->GetCreature(*me, addsGUID[i][j]))
                                    {
                                        switch(j)
                                        {
                                            case 1:
												add->GetMotionMaster()->MoveFollow(mainAdd, 1.0f, (M_PI / 2) + 0.5f);
												break;
                                            case 2:
												add->GetMotionMaster()->MoveFollow(mainAdd, 1.0f, (M_PI / 2 + M_PI) - 0.5f);
												break;
                                        }
                                    }
                                }
                            }
                        }
                        events.ScheduleEvent(8, 4000);
                        break;
                    case 8:
                        // Summon 3rd wave
                        if (Creature* mainAdd = me->SummonCreature(NPC_ARGENT_LIGHWIELDER, SpawnPosition))
                        {
                            addsGUID[2][0] = mainAdd->GetGUID();
                            mainAdd->SetTarget(StalkerGUID);

                            for (uint8 i = 0; i < 2; ++i)
                            {
                                uint32 addEntry = 0;
                                switch(i)
                                {
                                    case 0:
										addEntry = NPC_ARGENT_MONK; 
										break;
                                    case 1:
										addEntry = NPC_ARGENT_PRIESTESS;
										break;
                                }

                                if (Creature* add = me->SummonCreature(addEntry, SpawnPosition, TEMPSUMMON_MANUAL_DESPAWN))
                                {
                                    add->SetTarget(StalkerGUID);
                                    addsGUID[2][i+1] = add->GetGUID();
                                    switch(i)
                                    {
                                        case 0:
											add->GetMotionMaster()->MoveFollow(mainAdd, 1.0f, (M_PI / 2) + 0.5f);
											break;
                                        case 1:
											add->GetMotionMaster()->MoveFollow(mainAdd, 1.0f, (M_PI / 2 + M_PI) - 0.5f);
											break;
                                    }
                                }
                            }
                        }
                        events.ScheduleEvent(9, 4000);
                        break;
                    case 9:
                        // Move second wave to the new position
                        if (Creature* mainAdd2 = me->GetCreature(*me, addsGUID[1][0]))
                            mainAdd2->GetMotionMaster()->MovePoint(0, ArgentSoldierPosition[2]);

                        // Move third wave to the new position
                        if (Creature* mainAdd3 = me->GetCreature(*me, addsGUID[2][0]))
                            mainAdd3->GetMotionMaster()->MovePoint(0, ArgentSoldierPosition[0]);

                        events.ScheduleEvent(10, 4000);
                        break;
                    case 10:
                        // Refresh the adds position
                        for (uint8 i = 0; i<3; i++)
                        {
                            if (Creature* mainAdd = me->GetCreature(*me, addsGUID[i][0]))
                            {
                                mainAdd->SetFacingToObject(me);
                                for (uint8 j = 0; j<3; j++)
                                {
                                    if (Creature* add = me->GetCreature(*me, addsGUID[i][j]))
                                    {
                                        switch(j)
                                        {
                                            case 1:
												add->GetMotionMaster()->MoveFollow(mainAdd, 1.0f, (M_PI / 2) + 0.5f);
												break;
                                            case 2:
												add->GetMotionMaster()->MoveFollow(mainAdd, 1.0f, (M_PI / 2 + M_PI) - 0.5f);
												break;
                                        }
                                    }
                                }
                            }
                        }
                        events.ScheduleEvent(11, 4000);
                        break;
                    case 11:
                        // Summon the boss
                        bool chance;
                        chance = urand(0, 1);
                        if (Creature* boss = me->SummonCreature(chance ? NPC_EADRIC_THE_PURE : NPC_ARGENT_PALETRESS, SpawnPosition))
                        {
                            AnnounceChampion(boss);
                            BossGUID[0] = boss->GetGUID();
                        }
                        events.ScheduleEvent(12, 4000);
                        break;
                    case 12:
                        // Set home positions, in case of wipe, this avoids summons goin back to the SpawnPos
                        if (Creature* boss = me->GetCreature(*me, BossGUID[0]))
                            boss->SetHomePosition(boss->GetPositionX(), boss->GetPositionY(), boss->GetPositionZ(), boss->GetOrientation());

						// Eadric Intro
						if (Creature* eadric = me->FindCreatureWithDistance(NPC_EADRIC_THE_PURE, 250.0f))
							DoSendQuantumText(SAY_EADRIC_INTRO, eadric);

						// Paletress Intro
						if (Creature* paletress = me->FindCreatureWithDistance(NPC_ARGENT_PALETRESS, 250.0f))
							DoSendQuantumText(SAY_PALETRESS_INTRO_1, paletress);

                        for (uint8 i = 0; i<3; i++)
						{
                            for (uint8 j=0; j<3; j++)
							{
                                if (Creature* add = me->GetCreature(*me, addsGUID[i][j]))
                                {
                                    add->SetFacingToObject(me);
                                    add->SetHomePosition(add->GetPositionX(), add->GetPositionY(), add->GetPositionZ(), add->GetOrientation());
                                }
							}
						}
                        events.ScheduleEvent(13, 4000);
                        break;
                    case 13:
						// Paletress Intro 2
						if (Creature* paletress = me->FindCreatureWithDistance(NPC_ARGENT_PALETRESS, 250.0f))
							DoSendQuantumText(SAY_PALETRESS_INTRO_2, paletress);

                        // Move to the door position
                        me->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
                        me->GetMotionMaster()->MovePoint(0, AnnouncerPosition);
                        me->SetTarget(StalkerGUID);
                        events.ScheduleEvent(14, 19000);
                        break;
                    case 14:
                        //Close Door
                        if (GameObject* go = GameObject::GetGameObject(*me, instance->GetData64(DATA_MAIN_GATE)))
                            instance->HandleGameObject(go->GetGUID(), false);

                        if (Creature* tirion = me->GetCreature(*me, instance->GetData64(DATA_TIRION)))
                            DoSendQuantumText(SAY_TIRION_INTRO_ARGENT_2, tirion);

                        events.Reset();
                        SetData(EVENT_INTRO_ARGENT, DONE);
                        SetData(EVENT_WAVES_ARGENT, IN_PROGRESS);
                        break;
                }
            }

            if (GetData(EVENT_WAVES_ARGENT) == IN_PROGRESS)
            {
                switch(events.ExecuteEvent())
                {
                    case 1:
                        // Future texts?
                        events.ScheduleEvent(2, 3000);
                        break;
                    case 2:
                        // Start attack of first wave of adds
                        if (!addsAttacking)
                        {
                            for (uint8 j=0; j<3; j++)
                            {
                                for (uint8 i = 0; i<3; i++)
                                {
                                    if (Creature* add = me->GetCreature(*me, addsGUID[j][i]))
                                    {
                                        add->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                        add->SetReactState(REACT_AGGRESSIVE);
                                        add->SetCurrentFaction(16);
                                    }
                                }
                            }
                            addsAttacking = true;
                            events.ScheduleEvent(2, 1000);
						}
						else // Wait for the death of all of them
                        {
                            defeatedCount = 0;

                            for (uint8 j=0; j<3; j++)
                                for (uint8 i = 0; i<3; i++)
                                    if (Creature* add = me->GetCreature(*me, addsGUID[j][i]))
                                    {
                                        if (add->IsDead())
                                            defeatedCount++;
                                    }
									else defeatedCount++;

                            if (defeatedCount>=9)
                            {
                                defeatedCount = 0;
                                addsAttacking = false;
                                instance->SetData(MINION_ENCOUNTER, DONE);
                                events.ScheduleEvent(3, 4000);
                            }
							else events.ScheduleEvent(2, 1000);
                        }
                        break;
                    case 3:
                        // Despawn previous wave
                        for (uint8 j=0; j<3; j++)
                            for (uint8 i = 0; i<3; i++)
                                if (Creature* add = me->GetCreature(*me, addsGUID[j][i]))
                                    add->DespawnAfterAction();

                        // Prepare the Argent Champion for the fight
                        if (!addsAttacking)
                        {
                            if (Creature* boss = me->GetCreature(*me, BossGUID[0]))
                            {
                                boss->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
                                boss->GetMotionMaster()->MovePoint(0, ArgentSoldierPosition[0]);
                                boss->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                boss->SetReactState(REACT_AGGRESSIVE);
                                boss->SetCurrentFaction(16);
                            }
                            addsAttacking = true;
                            events.ScheduleEvent(3, 1000);
                        }
						else // Wait for his/her death
                        {
                            if (Creature* boss = me->GetCreature(*me, BossGUID[0]))
                            {
                                if (boss->AI()->GetData(DATA_CHAMPION_DEFEATED) == 1)
                                    events.ScheduleEvent(4, 0);
                                else
                                    events.ScheduleEvent(3, 1000);
                            }
                        }
                        break;
                    case 4:
                        // Mark encounter as completed
                        events.Reset();
                        SetData(EVENT_WAVES_ARGENT, DONE);
                        if (Creature* boss = me->GetCreature(*me, BossGUID[0]))
                        {
                            if (boss->GetEntry() == NPC_EADRIC_THE_PURE)
                                instance->SetData(BOSS_ARGENT_CHALLENGE_E, DONE);
                            else
                                instance->SetData(BOSS_ARGENT_CHALLENGE_P, DONE);
                        }
                        break;
                }
            }

            if (GetData(EVENT_BLACK_KNIGHT_INTRO) == IN_PROGRESS)
            {
                switch(events.ExecuteEvent())
                {
                    case 1:
                        if (Creature* tirion = me->GetCreature(*me, instance->GetData64(DATA_TIRION)))
                            DoSendQuantumText(SAY_TIRION_INTRO_BK_1, tirion);

                        events.ScheduleEvent(2, 3000);
                        break;
                    case 2:
                        if (Creature* mount = me->GetCreature(*me, instance->GetData64(DATA_BLACK_KNIGHT_GRYPHON)))
                        {
                            mount->SetPhaseMask(1, true);

                            if (Creature* knight = me->GetCreature(*me, instance->GetData64(DATA_BLACK_KNIGHT)))
                                knight->SetPhaseMask(1, true);

                            DoSendQuantumText(SAY_ANNOUNCER_BK, me);
                            mount->AI()->SetData(1, 0);
                            me->SetTarget(mount->GetGUID());
                        }
                        events.ScheduleEvent(3, 15000);
                        break;
                    case 3:
                        if (Creature* BlackKnight = me->GetCreature(*me, instance->GetData64(DATA_BLACK_KNIGHT)))
                        {
                            if (BlackKnight->GetVehicle())
                                events.ScheduleEvent(3, 2000);
                            else
                            {
                                BlackKnight->SetTarget(me->GetGUID());
                                me->SetTarget(BlackKnight->GetGUID());
                                events.ScheduleEvent(4, 2000);
                            }
                        }
                        break;
                    case 4:
                        if (Creature* BlackKnight = me->GetCreature(*me, instance->GetData64(DATA_BLACK_KNIGHT)))
                        {
                            DoSendQuantumText(SAY_BK_INTRO_1, BlackKnight);
                            BlackKnight->AI()->DoCast(SPELL_DEATH_RESPITE_INTRO);
                        }
                        events.ScheduleEvent(5, 4000);
                        break;
                    case 5:
                        if (Creature* BlackKnight = me->GetCreature(*me, instance->GetData64(DATA_BLACK_KNIGHT)))
                        {
							BlackKnight->SetFacingToObject(me);
                            BlackKnight->AI()->DoCast(me, SPELL_DEATH_PUSH_INTRO);
                            BlackKnight->SetTarget(0);
                        }
                        events.ScheduleEvent(6, 2000);
                        break;
                    case 6:
                        if (Creature* tirion = me->GetCreature(*me, instance->GetData64(DATA_TIRION)))
                        {
                            DoSendQuantumText(SAY_TIRION_INTRO_BK_2, tirion);

                            if (Creature* BlackKnight = me->GetCreature(*me, instance->GetData64(DATA_BLACK_KNIGHT)))
								BlackKnight->SetTarget(tirion->GetGUID());

							DoCast(SPELL_FEIGN_DEATH);
                        }
                        events.ScheduleEvent(7, 3000);
                        break;
                    case 7:
                        if (Creature* BlackKnight = me->GetCreature(*me, instance->GetData64(DATA_BLACK_KNIGHT)))
						{
							BlackKnight->SetWalk(true);
							BlackKnight->SetSpeed(MOVE_WALK, 0.8f, true);
                            DoSendQuantumText(SAY_BK_INTRO_2, BlackKnight);
							BlackKnight->GetMotionMaster()->MovePoint(0, BlackKnightCenterPos[0]);
						}
                        events.ScheduleEvent(8, 10000);
                        break;
                    case 8:
                        if (Creature* BlackKnight = me->GetCreature(*me, instance->GetData64(DATA_BLACK_KNIGHT)))
                            DoSendQuantumText(SAY_BK_INTRO_3, BlackKnight);
                        events.ScheduleEvent(9, 5000);
                        break;
                    case 9:
                        if (instance->GetData(DATA_TEAM) == HORDE)
                        {
                            if (Creature* garrosh = me->GetCreature(*me, instance->GetData64(DATA_GARROSH)))
                                DoSendQuantumText(SAY_GARROSH_INTRO_BK, garrosh);
                        }
						else
                        {
                            if (Creature* varian = me->GetCreature(*me, instance->GetData64(DATA_VARIAN)))
                                DoSendQuantumText(SAY_VARIAN_INTRO_BK, varian);
                        }

                        SetData(EVENT_BLACK_KNIGHT_INTRO, DONE);
                        events.Reset();
                        if (Creature* BlackKnight = me->GetCreature(*me, instance->GetData64(DATA_BLACK_KNIGHT)))
                        {
							BlackKnight->SetWalk(false);
							BlackKnight->SetSpeed(MOVE_WALK, 1.3f, true);
							BlackKnight->SetSpeed(MOVE_RUN, 1.3f, true);
                            BlackKnight->SetHomePosition(BlackKnight->GetPositionX(), BlackKnight->GetPositionY(), BlackKnight->GetPositionZ(), BlackKnight->GetOrientation());
                            BlackKnight->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            BlackKnight->SetReactState(REACT_AGGRESSIVE);
                            BlackKnight->Kill(me);
                        }
                        break;
                }
            }
        }

        bool AreAllPlayersMounted(bool dismountAndTeleport = false)
        {
            Map::PlayerList const &PlList = me->GetMap()->GetPlayers();

            if (PlList.isEmpty())
                return false;

            for (Map::PlayerList::const_iterator itr = PlList.begin(); itr != PlList.end(); ++itr)
			{
                if (Player* player = itr->getSource())
                {
                    if (player->IsGameMaster())
                        continue;

                    if (player->IsAlive())
                    {
                        if (Creature* mount = player->GetVehicleCreatureBase())
                        {
                            if (dismountAndTeleport)
                            {
                                player->ExitVehicle();
                                mount->DespawnAfterAction();
                            }
                        }
						else if (!dismountAndTeleport)
                            return false;

                        if (dismountAndTeleport)
                            player->NearTeleportTo(746.851f, 608.875f, 411.172f, 1.60308f);
                    }
                }
			}
			return true;
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_announcer_toc5AI(creature);
    }
};

void AddSC_trial_of_the_champion()
{
    new npc_announcer_toc5();
}