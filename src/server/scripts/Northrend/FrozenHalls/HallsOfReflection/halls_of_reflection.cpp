/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "QuantumEscortAI.h"
#include "QuantumGossip.h"
#include "MapManager.h"
#include "Transport.h"
#include "halls_of_reflection.h"

enum Yells
{
    SAY_JAINA_INTRO_1                             = -1668001,
    SAY_JAINA_INTRO_2                             = -1668002,
    SAY_JAINA_INTRO_3                             = -1668003,
    SAY_JAINA_INTRO_4                             = -1668004,
    SAY_UTHER_INTRO_A2_1                          = -1668005,
    SAY_JAINA_INTRO_5                             = -1668006,
    SAY_UTHER_INTRO_A2_2                          = -1668007,
    SAY_JAINA_INTRO_6                             = -1668008,
    SAY_UTHER_INTRO_A2_3                          = -1668009,
    SAY_JAINA_INTRO_7                             = -1668010,
    SAY_UTHER_INTRO_A2_4                          = -1668011,
    SAY_JAINA_INTRO_8                             = -1668012,
    SAY_UTHER_INTRO_A2_5                          = -1668013,
    SAY_JAINA_INTRO_9                             = -1668014,
    SAY_UTHER_INTRO_A2_6                          = -1668015,
    SAY_UTHER_INTRO_A2_7                          = -1668016,
    SAY_JAINA_INTRO_10                            = -1668017,
    SAY_UTHER_INTRO_A2_8                          = -1668018,
    SAY_JAINA_INTRO_11                            = -1668019,
    SAY_UTHER_INTRO_A2_9                          = -1668020,

    SAY_SYLVANAS_INTRO_1                          = -1668021,
    SAY_SYLVANAS_INTRO_2                          = -1668022,
    SAY_SYLVANAS_INTRO_3                          = -1668023,
    SAY_UTHER_INTRO_H2_1                          = -1668024,
    SAY_SYLVANAS_INTRO_4                          = -1668025,
    SAY_UTHER_INTRO_H2_2                          = -1668026,
    SAY_SYLVANAS_INTRO_5                          = -1668027,
    SAY_UTHER_INTRO_H2_3                          = -1668028,
    SAY_SYLVANAS_INTRO_6                          = -1668029,
    SAY_UTHER_INTRO_H2_4                          = -1668030,
    SAY_SYLVANAS_INTRO_7                          = -1668031,
    SAY_UTHER_INTRO_H2_5                          = -1668032,
    SAY_UTHER_INTRO_H2_6                          = -1668033,
    SAY_SYLVANAS_INTRO_8                          = -1668034,
    SAY_UTHER_INTRO_H2_7                          = -1668035,

    SAY_LK_INTRO_1                                = -1668036,
    SAY_LK_INTRO_2                                = -1668037,
    SAY_LK_INTRO_3                                = -1668038,
    SAY_FALRIC_INTRO_1                            = -1668039,
    SAY_MARWYN_INTRO_1                            = -1668040,
    SAY_FALRIC_INTRO_2                            = -1668041,

    SAY_JAINA_INTRO_END                           = -1668042,
    SAY_SYLVANAS_INTRO_END                        = -1668043,
    SAY_LK_JAINA_INTRO_END                        = -1594473,
    SAY_LK_SYLVANAS_INTRO_END                     = -1594474,
};

enum Events
{
    EVENT_NONE,

    EVENT_START_PREINTRO,
    EVENT_PREINTRO_1_A,
    EVENT_PREINTRO_1_H,
    EVENT_PREINTRO_2,

    EVENT_START_INTRO,
    EVENT_SKIP_INTRO,

    EVENT_INTRO_A2_1,
    EVENT_INTRO_A2_2,
    EVENT_INTRO_A2_3,
    EVENT_INTRO_A2_4,
    EVENT_INTRO_A2_5,
    EVENT_INTRO_A2_6,
    EVENT_INTRO_A2_7,
    EVENT_INTRO_A2_8,
    EVENT_INTRO_A2_9,
    EVENT_INTRO_A2_10,
    EVENT_INTRO_A2_11,
    EVENT_INTRO_A2_12,
    EVENT_INTRO_A2_13,
    EVENT_INTRO_A2_14,
    EVENT_INTRO_A2_15,
    EVENT_INTRO_A2_16,
    EVENT_INTRO_A2_17,
    EVENT_INTRO_A2_18,
    EVENT_INTRO_A2_19,

    EVENT_INTRO_H2_1,
    EVENT_INTRO_H2_2,
    EVENT_INTRO_H2_3,
    EVENT_INTRO_H2_4,
    EVENT_INTRO_H2_5,
    EVENT_INTRO_H2_6,
    EVENT_INTRO_H2_7,
    EVENT_INTRO_H2_8,
    EVENT_INTRO_H2_9,
    EVENT_INTRO_H2_10,
    EVENT_INTRO_H2_11,
    EVENT_INTRO_H2_12,
    EVENT_INTRO_H2_13,
    EVENT_INTRO_H2_14,
    EVENT_INTRO_H2_15,

    EVENT_INTRO_LK_1,
    EVENT_INTRO_LK_2,
    EVENT_INTRO_LK_3,
    EVENT_INTRO_LK_4,
    EVENT_INTRO_LK_5,
    EVENT_INTRO_LK_6,
    EVENT_INTRO_LK_7,
    EVENT_INTRO_LK_8,
    EVENT_INTRO_LK_9,
    EVENT_INTRO_LK_10,

    EVENT_INTRO_END,
    
    EVENT_OUTRO_1,
    EVENT_OUTRO_2,
    EVENT_OUTRO_3,
    EVENT_OUTRO_4,
    
    EVENT_QUEST_QUEL_1,
    EVENT_QUEST_QUEL_2,
    EVENT_QUEST_QUEL_3,
    EVENT_QUEST_QUEL_4,
    EVENT_QUEST_QUEL_5,
    EVENT_QUEST_QUEL_6,
    EVENT_QUEST_QUEL_7,
    EVENT_QUEST_QUEL_8,
    EVENT_QUEST_QUEL_9,
    EVENT_QUEST_QUEL_10,
    EVENT_QUEST_QUEL_11,
	EVENT_ATTACK_START,
    EVENT_MORTAL_STRIKE,
    EVENT_HEROIC_STRIKE,
	EVENT_BLADESTORM,
};

enum RefflectionMisc
{
    ACTION_START_PREINTRO,
    ACTION_START_INTRO,
    ACTION_SKIP_INTRO,
    ACTION_START_FLY,
};

enum Spells
{
    SPELL_CAST_VISUAL         = 65633,
    SPELL_BOSS_SPAWN_AURA     = 72712,
    SPELL_UTHER_DESPAWN       = 70693,
    SPELL_TAKE_FROSTMOURNE    = 72729,
    SPELL_FROSTMOURNE_DESPAWN = 72726,
    SPELL_FROSTMOURNE_VISUAL  = 73220,
    SPELL_FROSTMOURNE_SOUNDS  = 70667,
};

const Position HallsofReflectionLocs[] =
{
    {5283.234863f, 1990.946777f, 707.695679f, 0.929097f},   // 2 Loralen Follows
    {5408.031250f, 2102.918213f, 707.695251f, 0.792756f},   // 9 Sylvanas Follows
    {5401.866699f, 2110.837402f, 707.695251f, 0.800610f},   // 10 Loralen follows
};

const Position SpawnPos              = {5262.540527f, 1949.693726f, 707.695007f, 0.808736f}; // Jaina/Sylvanas Beginning Position
const Position MoveDoorPos           = {5268.254395f, 1955.522705f, 707.699585f, 1.277278f};
const Position MoveThronePos         = {5306.952148f, 1998.499023f, 709.341431f, 1.277278f}; // Jaina/Sylvanas walks to throne
const Position UtherSpawnPos         = {5308.310059f, 2003.857178f, 709.341431f, 4.650315f};
const Position LichKingSpawnPos      = {5362.917480f, 2062.307129f, 707.695374f, 3.945812f};
const Position LichKingMoveThronePos = {5312.080566f, 2009.172119f, 709.341431f, 3.973301f}; // Lich King walks to throne
const Position LichKingMoveAwayPos   = {5400.069824f, 2102.7131689f, 707.69525f, 0.843803f}; // Lich King walks away

void StartFly(Transport* transport)
{
    transport->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
    transport->SetGoState(GO_STATE_ACTIVE);
    transport->SetUInt32Value(GAMEOBJECT_DYNAMIC, 0x10830010); // Seen in sniffs
    transport->SetFloatValue(GAMEOBJECT_PARENTROTATION + 3, 1.0f);

    Map* map = transport->GetMap();
    std::set<uint32> mapsUsed;
    GameObjectTemplate const* goinfo = transport->GetGOInfo();

    transport->GenerateWaypoints(goinfo->moTransport.taxiPathId, mapsUsed);

    for (Map::PlayerList::const_iterator itr = map->GetPlayers().begin(); itr != map->GetPlayers().end(); ++itr)
    {
        if (Player* player = itr->getSource())
        {
			UpdateData transData;
            transport->BuildCreateUpdateBlockForPlayer(&transData, player);
            WorldPacket packet;
            transData.BuildPacket(&packet);
            player->SendDirectMessage(&packet);
        }
    }
}

void StopFly(Transport* transport)
{
    Map* map = transport->GetMap();
    transport->m_WayPoints.clear();
    //RelocateTransport(transport);
    transport->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
    transport->SetGoState(GO_STATE_READY);

    for (Map::PlayerList::const_iterator itr = map->GetPlayers().begin(); itr != map->GetPlayers().end(); ++itr)
    {
        if (Player* player = itr->getSource())
        {
            UpdateData transData;
            transport->BuildCreateUpdateBlockForPlayer(&transData, player);
            WorldPacket packet;
            transData.BuildPacket(&packet);
            player->SendDirectMessage(&packet);
        }
    }
}

class npc_jaina_or_sylvanas_hor : public CreatureScript
{
private:
    bool Sylvana;

public:
    npc_jaina_or_sylvanas_hor (bool isSylvana, const char* name) : CreatureScript(name), Sylvana(isSylvana) { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
		{
			player->PrepareQuestMenu(creature->GetGUID());
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Can you remove the sworld?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
		}
		return true;
	}

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                if (creature->AI())
					creature->AI()->DoAction(ACTION_START_INTRO);
				creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
                player->CLOSE_GOSSIP_MENU();
                break;
        }
        return true;
    }

    // AI of Part1: handle the intro till start of gauntlet event.
    struct npc_jaina_or_sylvanas_horAI : public QuantumBasicAI
    {
        npc_jaina_or_sylvanas_horAI(Creature* creature) : QuantumBasicAI(creature)
        {
			instance = me->GetInstanceScript();
        }

        InstanceScript* instance;
        uint64 Uther;
        uint64 LichKing;
        bool Start;
        EventMap events;

        void Reset()
        {
            events.Reset();

			Start = false;

			Uther = 0;
            LichKing = 0;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetStandState(UNIT_STAND_STATE_STAND);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
			me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);

            me->SetVisible(true);
        }

        void DoAction(const int32 actionId)
        {
            switch(actionId)
            {
                case ACTION_START_PREINTRO:
                    events.ScheduleEvent(EVENT_START_PREINTRO, 3000);
                    break;
                case ACTION_START_INTRO:
                    events.ScheduleEvent(EVENT_START_INTRO, 1000);
                    break;
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (Start)
                return;

            if (who->GetTypeId() != TYPE_ID_PLAYER)
                return;

            me->AI()->DoAction(ACTION_START_PREINTRO);
            Start = true;
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);
            switch(events.ExecuteEvent())
            {
                case EVENT_START_PREINTRO:
                    me->SetWalk(false);                   
                    me->GetMotionMaster()->MovePoint(0, MoveDoorPos);

                    if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
						events.ScheduleEvent(EVENT_PREINTRO_1_A, 0);
					else
						events.ScheduleEvent(EVENT_PREINTRO_1_H, 0);
					break;
                case EVENT_PREINTRO_1_A:
					DoSendQuantumText(SAY_JAINA_INTRO_1, me);
					events.ScheduleEvent(EVENT_PREINTRO_2, 6000);
                    break;
                case EVENT_PREINTRO_1_H:
					events.ScheduleEvent(EVENT_INTRO_H2_1, 8000);
                    break;
                case EVENT_INTRO_H2_1:
					DoSendQuantumText(SAY_SYLVANAS_INTRO_1, me);
					me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
                    break;
                case EVENT_PREINTRO_2:
					DoSendQuantumText(SAY_JAINA_INTRO_2, me);
					me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
                    break;
                case EVENT_START_INTRO:
                    me->SetWalk(false);
                    me->GetMotionMaster()->MovePoint(0, MoveThronePos);
                    if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
						events.ScheduleEvent(EVENT_INTRO_A2_1, 0);
                    else
						events.ScheduleEvent(EVENT_INTRO_H2_2, 0);
                    break;
					// A2 Intro Events
                case EVENT_INTRO_A2_1:               
                    DoSendQuantumText(SAY_JAINA_INTRO_3, me);
                    events.ScheduleEvent(EVENT_INTRO_A2_2, 5000);
                    break;
                case EVENT_INTRO_A2_2:
                    DoSendQuantumText(SAY_JAINA_INTRO_4, me);
                    events.ScheduleEvent(EVENT_INTRO_A2_3, 7000);
                    break;
                case EVENT_INTRO_A2_3:
                    DoCast(me, SPELL_CAST_VISUAL);
					instance->HandleGameObject(instance->GetData64(DATA_FROSTMOURNE), true);
					me->CastSpell(me, SPELL_FROSTMOURNE_SOUNDS, true);
                    events.ScheduleEvent(EVENT_INTRO_A2_4, 10000);
                    break;
                case EVENT_INTRO_A2_4:
                    if (Creature* pUther = me->SummonCreature(NPC_UTHER, UtherSpawnPos, TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        pUther->GetMotionMaster()->MoveIdle();
                        pUther->SetReactState(REACT_PASSIVE);
                        Uther = pUther->GetGUID();
                        me->SetUInt64Value(UNIT_FIELD_TARGET, Uther);
						me->SetFacingToObject(pUther);
                        pUther->SetUInt64Value(UNIT_FIELD_TARGET, me->GetGUID());
						pUther->SetFacingToObject(me);
                    }
                    events.ScheduleEvent(EVENT_INTRO_A2_5, 2000);
                    break;
                case EVENT_INTRO_A2_5:
                    if (Creature* pUther = me->GetCreature(*me, Uther))
                        DoSendQuantumText(SAY_UTHER_INTRO_A2_1, pUther);

                    events.ScheduleEvent(EVENT_INTRO_A2_6, 3000);
                    break;
                case EVENT_INTRO_A2_6:
                    DoSendQuantumText(SAY_JAINA_INTRO_5, me);
                    events.ScheduleEvent(EVENT_INTRO_A2_7, 6000);
                    break;
                case EVENT_INTRO_A2_7:
                    if (Creature* pUther = me->GetCreature(*me, Uther))
                        DoSendQuantumText(SAY_UTHER_INTRO_A2_2, pUther);

                    events.ScheduleEvent(EVENT_INTRO_A2_8, 6500);
                    break;
                case EVENT_INTRO_A2_8:
                    DoSendQuantumText(SAY_JAINA_INTRO_6, me);
                    events.ScheduleEvent(EVENT_INTRO_A2_9, 2500);
                    break;
                case EVENT_INTRO_A2_9:
                    if (Creature* pUther = me->GetCreature(*me, Uther))
                        DoSendQuantumText(SAY_UTHER_INTRO_A2_3, pUther);

                    events.ScheduleEvent(EVENT_INTRO_A2_10, 10000);
                    break;
                case EVENT_INTRO_A2_10:
                    DoSendQuantumText(SAY_JAINA_INTRO_7, me);
                    events.ScheduleEvent(EVENT_INTRO_A2_11, 5000);
                    break;
                case EVENT_INTRO_A2_11:
                    if (Creature* pUther = me->GetCreature(*me, Uther))
                        DoSendQuantumText(SAY_UTHER_INTRO_A2_4, pUther);

                    events.ScheduleEvent(EVENT_INTRO_A2_12, 12000);
                    break;
                case EVENT_INTRO_A2_12:
                    DoSendQuantumText(SAY_JAINA_INTRO_8, me);
                    events.ScheduleEvent(EVENT_INTRO_A2_13, 5000);
                    break;
                case EVENT_INTRO_A2_13:
                    if (Creature* pUther = me->GetCreature(*me, Uther))
                        DoSendQuantumText(SAY_UTHER_INTRO_A2_5, pUther);

                    events.ScheduleEvent(EVENT_INTRO_A2_14, 12500);
                    break;
                case EVENT_INTRO_A2_14:
                    DoSendQuantumText(SAY_JAINA_INTRO_9, me);
                    events.ScheduleEvent(EVENT_INTRO_A2_15, 10000);
                    break;
                case EVENT_INTRO_A2_15:
                    if (Creature* pUther = me->GetCreature(*me, Uther))
                        DoSendQuantumText(SAY_UTHER_INTRO_A2_6, pUther);

                    events.ScheduleEvent(EVENT_INTRO_A2_16, 23000);
                    break;
                case EVENT_INTRO_A2_16:
                    if (Creature* pUther = me->GetCreature(*me, Uther))
                        DoSendQuantumText(SAY_UTHER_INTRO_A2_7, pUther);

                    events.ScheduleEvent(EVENT_INTRO_A2_17, 4000);
                    break;
                case EVENT_INTRO_A2_17:
                    DoSendQuantumText(SAY_JAINA_INTRO_10, me);
                    events.ScheduleEvent(EVENT_INTRO_A2_18, 2000);
                    break;
                case EVENT_INTRO_A2_18:
                    if (Creature* pUther = me->GetCreature(*me, Uther))
                    {
                        pUther->HandleEmoteCommand(EMOTE_ONESHOT_NO);
                        DoSendQuantumText(SAY_UTHER_INTRO_A2_8, pUther);
                    }
                    events.ScheduleEvent(EVENT_INTRO_A2_19, 11000);
                    break;
                case EVENT_INTRO_A2_19:
                    DoSendQuantumText(SAY_JAINA_INTRO_11, me);
                    events.ScheduleEvent(EVENT_INTRO_LK_1, 2000);
                    break;
					// H2 Intro Events
                case EVENT_INTRO_H2_2:
                    DoSendQuantumText(SAY_SYLVANAS_INTRO_2, me);
                    events.ScheduleEvent(EVENT_INTRO_H2_3, 8000);
                    break;
                case EVENT_INTRO_H2_3:
                    DoSendQuantumText(SAY_SYLVANAS_INTRO_3, me);
                    DoCast(me, SPELL_CAST_VISUAL);
					instance->HandleGameObject(instance->GetData64(DATA_FROSTMOURNE), true);
                    me->CastSpell(me, SPELL_FROSTMOURNE_SOUNDS, true);
                    events.ScheduleEvent(EVENT_INTRO_H2_4, 6000);
                    break;
                case EVENT_INTRO_H2_4:
                    // Spawn Uther during speach 2
                    if (Creature* pUther = me->SummonCreature(NPC_UTHER, UtherSpawnPos, TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        pUther->GetMotionMaster()->MoveIdle();
                        pUther->SetReactState(REACT_PASSIVE);
                        Uther = pUther->GetGUID();
                        me->SetUInt64Value(UNIT_FIELD_TARGET, Uther);
						me->SetFacingToObject(pUther);
                        pUther->SetUInt64Value(UNIT_FIELD_TARGET, me->GetGUID());
						pUther->SetFacingToObject(me);
                    }
                    events.ScheduleEvent(EVENT_INTRO_H2_5, 2000);
                    break;
                case EVENT_INTRO_H2_5:
                    if (Creature* pUther = me->GetCreature(*me, Uther))
                        DoSendQuantumText(SAY_UTHER_INTRO_H2_1, pUther);
                    events.ScheduleEvent(EVENT_INTRO_H2_6, 11000);
                    break;
                case EVENT_INTRO_H2_6:
                    DoSendQuantumText(SAY_SYLVANAS_INTRO_4, me);
                    events.ScheduleEvent(EVENT_INTRO_H2_7, 3000);
                    break;
                case EVENT_INTRO_H2_7:
                    if (Creature* pUther = me->GetCreature(*me, Uther))
                        DoSendQuantumText(SAY_UTHER_INTRO_H2_2, pUther);
                    events.ScheduleEvent(EVENT_INTRO_H2_8, 6000);
                    break;
                case EVENT_INTRO_H2_8:
                    DoSendQuantumText(SAY_SYLVANAS_INTRO_5, me);
                    events.ScheduleEvent(EVENT_INTRO_H2_9, 5000);
                    break;
                case EVENT_INTRO_H2_9:
                    if (Creature* pUther = me->GetCreature(*me, Uther))
                        DoSendQuantumText(SAY_UTHER_INTRO_H2_3, pUther);
                    events.ScheduleEvent(EVENT_INTRO_H2_10, 19000);
                    break;
                case EVENT_INTRO_H2_10:
                    DoSendQuantumText(SAY_SYLVANAS_INTRO_6, me);
                    events.ScheduleEvent(EVENT_INTRO_H2_11, 1500);
                    break;
                case EVENT_INTRO_H2_11:
                    if (Creature* pUther = me->GetCreature(*me, Uther))
                        DoSendQuantumText(SAY_UTHER_INTRO_H2_4, pUther);
                    events.ScheduleEvent(EVENT_INTRO_H2_12, 19500);
                    break;
                case EVENT_INTRO_H2_12:
                    DoSendQuantumText(SAY_SYLVANAS_INTRO_7, me);
                    events.ScheduleEvent(EVENT_INTRO_H2_13, 2000);
                    break;
                case EVENT_INTRO_H2_13:
                    if (Creature* pUther = me->GetCreature(*me, Uther))
                    {
                        pUther->HandleEmoteCommand(EMOTE_ONESHOT_NO);
                        DoSendQuantumText(SAY_UTHER_INTRO_H2_5, pUther);
                    }
                    events.ScheduleEvent(EVENT_INTRO_H2_14, 12000);
                    break;
                case EVENT_INTRO_H2_14:
                    if (Creature* pUther = me->GetCreature(*me, Uther))
                        DoSendQuantumText(SAY_UTHER_INTRO_H2_6, pUther);
                    events.ScheduleEvent(EVENT_INTRO_H2_15, 8000);
                    break;
                case EVENT_INTRO_H2_15:
                    DoSendQuantumText(SAY_SYLVANAS_INTRO_8, me);
                    events.ScheduleEvent(EVENT_INTRO_LK_1, 2000);
                    break;
					// Remaining Intro Events common for both faction
                case EVENT_INTRO_LK_1:
					// Spawn LK in front of door, and make him move to the sword.
                    if (Creature* pLichKing = me->SummonCreature(NPC_LICH_KING_EVENT, LichKingSpawnPos, TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        pLichKing->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
                        pLichKing->GetMotionMaster()->MovePoint(0, LichKingMoveThronePos);
                        pLichKing->SetReactState(REACT_PASSIVE);
                        LichKing = pLichKing->GetGUID();
                        pLichKing->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                        if (GameObject* gate = instance->instance->GetGameObject(instance->GetData64(DATA_FROSTWORN_DOOR)))
							gate->SetGoState(GO_STATE_ACTIVE);

                        me->SetUInt64Value(UNIT_FIELD_TARGET, LichKing);
						me->SetFacingToObject(pLichKing);
                        pLichKing->SetUInt64Value(UNIT_FIELD_TARGET, me->GetGUID());
						pLichKing->SetFacingToObject(me);
                    }

                    if (Creature* pUther = me->GetCreature(*me, Uther))
                    {
                        pUther->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_COWER);

                        if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                            DoSendQuantumText(SAY_UTHER_INTRO_A2_9, pUther);

                        else
                            DoSendQuantumText(SAY_UTHER_INTRO_H2_7, pUther);
                    }
                    events.ScheduleEvent(EVENT_INTRO_LK_2, 11000);
                    break;
                case EVENT_INTRO_LK_2:
					if (Creature* pLichKing = me->GetCreature(*me, LichKing))
						DoSendQuantumText(SAY_LK_INTRO_1, pLichKing);

                    if (GameObject* gate = instance->instance->GetGameObject(instance->GetData64(DATA_FROSTWORN_DOOR)))
                        gate->SetGoState(GO_STATE_READY);

					events.ScheduleEvent(EVENT_INTRO_LK_3, 2000);
					break;
				case EVENT_INTRO_LK_3:
					if (Creature* pUther = me->GetCreature(*me, Uther))
					{
						pUther->CastSpell(pUther, SPELL_UTHER_DESPAWN, true);
						pUther->DisappearAndDie();
						Uther = 0;
					}
					events.ScheduleEvent(EVENT_INTRO_LK_4, 7000);
					break;
                case EVENT_INTRO_LK_4:
                      if (Creature* pLichKing = me->GetCreature(*me, LichKing))
						  DoSendQuantumText(SAY_LK_INTRO_2, pLichKing);

					  if (GameObject* frostmourne = instance->instance->GetGameObject(instance->GetData64(DATA_FROSTMOURNE)))
						  frostmourne->SetPhaseMask(0, true);

                      if (Creature* pLichKing = me->GetCreature(*me, LichKing))
                      {
						  pLichKing->CastSpell(pLichKing, SPELL_TAKE_FROSTMOURNE, true);
						  pLichKing->CastSpell(pLichKing, SPELL_FROSTMOURNE_VISUAL, true);  
                      }
                      me->RemoveAllAuras();
					  events.ScheduleEvent(EVENT_INTRO_LK_5, 10000);
					  break;
				case EVENT_INTRO_LK_5:
                    if (Creature* pFalric = me->GetCreature(*me, instance->GetData64(DATA_FALRIC)))
                    {
                        pFalric->CastSpell(pFalric, SPELL_BOSS_SPAWN_AURA, true);
                        pFalric->SetVisible(true);
                        pFalric->GetMotionMaster()->MovePoint(0, 5283.309f, 2031.173f, 709.319f);
                    }

                    if (Creature* pMarwyn = me->GetCreature(*me, instance->GetData64(DATA_MARWYN)))
                    {
                        pMarwyn->CastSpell(pMarwyn, SPELL_BOSS_SPAWN_AURA, true);
                        pMarwyn->SetVisible(true);
                        pMarwyn->GetMotionMaster()->MovePoint(0, 5335.585f, 1981.439f, 709.319f);
                    }

                    if (Creature* pLichKing = me->GetCreature(*me, LichKing))
						DoSendQuantumText(SAY_LK_INTRO_3, pLichKing);

					events.ScheduleEvent(EVENT_INTRO_LK_6, 8000);
					break;
                case EVENT_INTRO_LK_6:
                    if (Creature* pFalric = me->GetCreature(*me, instance->GetData64(DATA_FALRIC)))
                        DoSendQuantumText(SAY_FALRIC_INTRO_1, pFalric);

					events.ScheduleEvent(EVENT_INTRO_LK_7, 2000);
                    break;
                case EVENT_INTRO_LK_7:
                    if (Creature* pMarwyn = me->GetCreature(*me, instance->GetData64(DATA_MARWYN)))
                        DoSendQuantumText(SAY_MARWYN_INTRO_1, pMarwyn);

					if (Creature* pLichKing = me->GetCreature(*me, LichKing))
                    {
                        if (GameObject* gate = instance->instance->GetGameObject(instance->GetData64(DATA_FROSTWORN_DOOR)))
                           gate->SetGoState(GO_STATE_ACTIVE);

						pLichKing->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
                        pLichKing->GetMotionMaster()->MovePoint(0, LichKingMoveAwayPos);
                    }
                    events.ScheduleEvent(EVENT_INTRO_LK_8, 2000);
                    break;
                case EVENT_INTRO_LK_8:
                    if (Creature* pFalric = me->GetCreature(*me, instance->GetData64(DATA_FALRIC)))
                    {
                        instance->SetData(DATA_FALRIC_EVENT, SPECIAL);
                        DoSendQuantumText(SAY_FALRIC_INTRO_2, pFalric);
                        instance->DoUpdateWorldState(WORLD_STATE_HOR, 1);
                        instance->DoUpdateWorldState(WORLD_STATE_HOR_WAVE_COUNT, 0);
                    }
                    events.ScheduleEvent(EVENT_INTRO_LK_9, 5000);
                    break;
                case EVENT_INTRO_LK_9:
                    if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                        DoSendQuantumText(SAY_JAINA_INTRO_END, me);
                    else
						DoSendQuantumText(SAY_SYLVANAS_INTRO_END, me);

					me->GetMotionMaster()->MovePoint(0, LichKingMoveAwayPos);

                    if (Creature* pLichKing = me->GetCreature(*me, LichKing))
                    {
						pLichKing->SetWalk(false);
						pLichKing->SetSpeed(MOVE_RUN, 2.0f, true);
                        pLichKing->GetMotionMaster()->MovePoint(0, LichKingMoveAwayPos);
                    }
                    events.ScheduleEvent(EVENT_INTRO_LK_10, 5000);
                    break;
                case EVENT_INTRO_LK_10:
                     if (Creature* pLichKing = me->GetCreature(*me, LichKing))
					 {
                         if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                            DoSendQuantumText(SAY_LK_JAINA_INTRO_END, pLichKing);
                         else
                             DoSendQuantumText(SAY_LK_SYLVANAS_INTRO_END, pLichKing);
					 }
                     events.ScheduleEvent(EVENT_INTRO_END, 7000);
                     break;
                case EVENT_INTRO_END:
					instance->SetData(DATA_INTRO_EVENT, DONE);
					instance->SetData(DATA_WAVE_COUNT, SPECIAL);

                    if (GameObject* gate = instance->instance->GetGameObject(instance->GetData64(DATA_FROSTWORN_DOOR)))
                        gate->SetGoState(GO_STATE_READY);

                    if (GameObject* door = instance->instance->GetGameObject(instance->GetData64(DATA_FRONT_DOOR)))
						door->SetGoState(GO_STATE_READY);

					me->DisappearAndDie();

                    if (Creature* pLichKing = me->GetCreature(*me, LichKing))
						pLichKing->DisappearAndDie();
					break;
				case EVENT_SKIP_INTRO:
					// Spawn LK in front of door, and make him move to the sword.
					if (Creature* pLichKing = me->SummonCreature(NPC_LICH_KING_EVENT, LichKingSpawnPos, TEMPSUMMON_MANUAL_DESPAWN))
                    {
                        pLichKing->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
                        pLichKing->GetMotionMaster()->MovePoint(0, LichKingMoveThronePos);
                        pLichKing->SetReactState(REACT_PASSIVE);
                        LichKing = pLichKing->GetGUID();
                        me->SetUInt64Value(UNIT_FIELD_TARGET, LichKing);
						me->SetFacingToObject(pLichKing);
                        pLichKing->SetUInt64Value(UNIT_FIELD_TARGET, me->GetGUID());
						pLichKing->SetFacingToObject(me);
                    }
                    me->SetWalk(false);
                    me->GetMotionMaster()->MovePoint(0, MoveThronePos);
                    events.ScheduleEvent(EVENT_INTRO_LK_4, 20000);
					break;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_jaina_or_sylvanas_horAI(creature);
    }
};

enum FrostwornGeneral
{
    SAY_AGGRO  = -1594519,
    SAY_DEATH  = -1594520,

    SPELL_SHIELD_THROWN_5N = 69222,
    SPELL_SHIELD_THROWN_5H = 73076,
    SPELL_ICE_SPIKES_5N    = 69184,
    SPELL_ICE_SPIKES_5H    = 70399,
    SPELL_CLONE_ME         = 45204,
	SPELL_CLONE_NAME       = 57507,
};

class npc_frostsworn_general : public CreatureScript
{
public:
    npc_frostsworn_general() : CreatureScript("npc_frostsworn_general") { }

    struct npc_frostsworn_generalAI : public QuantumBasicAI
    {
        npc_frostsworn_generalAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();

			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

        InstanceScript* instance;

        uint32 ShieldThrowTimer;
        uint32 IceSpikesTimer;
        uint32 SinisterReflectionTimer;

		bool Intro;

        void Reset()
        {
			ShieldThrowTimer = 0.5*IN_MILLISECONDS;
            IceSpikesTimer = 3*IN_MILLISECONDS;
            SinisterReflectionTimer = 5*IN_MILLISECONDS;

			Intro = false;

            instance->SetData(DATA_FROSTSWORN_EVENT, NOT_STARTED);
        }

		void MoveInLineOfSight(Unit* who)
        {
            if (!instance || Intro || who->GetTypeId() != TYPE_ID_PLAYER || instance->GetData(DATA_FALRIC_EVENT) != DONE && instance->GetData(DATA_MARWYN_EVENT) != DONE || !who->IsWithinDistInMap(me, 35.0f))
				return;

			if (instance && Intro == false && who->GetTypeId() == TYPE_ID_PLAYER && instance->GetData(DATA_FALRIC_EVENT) == DONE && instance->GetData(DATA_MARWYN_EVENT) == DONE && who->IsWithinDistInMap(me, 35.0f))
			{
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
				me->SetInCombatWithZone();
				Intro = true;
			}
		}

        void EnterToBattle(Unit* /*victim*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

            instance->SetData(DATA_FROSTSWORN_EVENT, IN_PROGRESS);
        }

		void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

            instance->SetData(DATA_FROSTSWORN_EVENT, DONE);
        }

		void SummonClones()
        {
            std::list<Unit*> playerList;
            SelectTargetList(playerList, 5, TARGET_RANDOM, 0, true);
            for (std::list<Unit*>::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
            {
                Unit* creature = (*itr);
                Creature* reflection = me->SummonCreature(NPC_SPIRITUAL_REFLECTION, creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ(), creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 3000);
                reflection->SetName(creature->GetName());
                creature->CastSpell(reflection, SPELL_CLONE_NAME, true);
                creature->CastSpell(reflection, SPELL_CLONE_ME, true);
                reflection->SetCurrentFaction(me->GetFaction());
                reflection->AI()->DoZoneInCombat();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ShieldThrowTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHIELD_THROWN_5N, SPELL_SHIELD_THROWN_5H));
					ShieldThrowTimer = 4*IN_MILLISECONDS;
				}
            }
            else ShieldThrowTimer -= diff;

            if (IceSpikesTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_ICE_SPIKES_5N, SPELL_ICE_SPIKES_5H));
					IceSpikesTimer = 6*IN_MILLISECONDS;
				}
            }
            else IceSpikesTimer -= diff;

            if (SinisterReflectionTimer <= diff)
            {
                SummonClones();
                SinisterReflectionTimer = 8*IN_MILLISECONDS;
            }
            else SinisterReflectionTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frostsworn_generalAI(creature);
    }
};

enum Defs
{
	// Intro Pre Escape
    SAY_LICH_KING_AGGRO_A              = -1594477,
    SAY_LICH_KING_AGGRO_H              = -1594478,
    SAY_JAINA_AGGRO                    = -1594479,
    SAY_SYLVANA_AGGRO                  = -1594480,

	// Escape
    SAY_JAINA_WALL_01                  = -1594487,
    SAY_SYLVANA_WALL_01                = -1594488,
    SAY_JAINA_WALL_02                  = -1594489,
    SAY_SYLVANA_WALL_02                = -1594490,
    SAY_LICH_KING_WALL_02              = -1594491,
    SAY_LICH_KING_WALL_03              = -1594492,
    SAY_LICH_KING_WALL_04              = -1594493,
    SAY_JAINA_WALL_03                  = -1594494,
    SAY_JAINA_WALL_04                  = -1594495,
    SAY_SYLVANA_WALL_03                = -1594496,
    SAY_SYLVANA_WALL_04                = -1594497,
    SAY_JAINA_ESCAPE_01                = -1594498,
    SAY_JAINA_ESCAPE_02                = -1594499,
    SAY_SYLVANA_ESCAPE_01              = -1594500,
    SAY_SYLVANA_ESCAPE_02              = -1594501,
    SAY_JAINA_TRAP                     = -1594502,
    SAY_SYLVANA_TRAP                   = -1594503,
    SAY_MATHEAS_JAINA                  = -1594505,
	SAY_LICH_KING_END_01               = -1594506,
    SAY_LICH_KING_END_02               = -1594507,
    SAY_LICH_KING_END_03               = -1594508,

    SAY_SYLVANA_FINAL_1                = -1594521,
    SAY_SYLVANA_FINAL_2                = -1594522,

    SAY_JAINA_FINAL_1                  = -1594523,
    SAY_JAINA_FINAL_2                  = -1594524,
	SAY_JAINA_FINAL_3                  = -1594525,

	SAY_BARTLETT_FIRE                  = -1594526,
    SAY_KROM_FIRE                      = -1594527,

	SAY_BARTLETT_BOARD                 = -1594528,
	SAY_KROM_BOARD                     = -1594529,

	// Spells
    SPELL_WINTER                       = 69780,
    SPELL_FURY_OF_FROSTMOURNE          = 70063,
    SPELL_ICE_PRISON_VISUAL            = 69708,
    SPELL_DARK_ARROW                   = 70194,
    SPELL_ICE_BARRIER                  = 69787,
    SPELL_DESTROY_ICE_WALL_01          = 69784, // Jaina
    SPELL_DESTROY_ICE_WALL_03          = 70225, // Sylvana
    SPELL_SYLVANA_JUMP                 = 68339,
    SPELL_SYLVANA_STEP                 = 69087,
    SPELL_FIRE_CANNON                  = 67461,

    FACTION                            = 2076,
};

const Position CannonSpawns[4] = 
{
    {5215.912109f, 1658.796997f, 802.862976f, 0.00f},
    {5229.619141f, 1646.045166f, 802.862976f, 0.00f},
    {5244.881348f, 1626.080688f, 802.862976f, 0.00f},
    {5256.842285f, 1617.717041f, 802.051331f, 0.00f},
};

class npc_jaina_and_sylvana_hor_part2 : public CreatureScript
{
public:
    npc_jaina_and_sylvana_hor_part2() : CreatureScript("npc_jaina_and_sylvana_hor_part2") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        InstanceScript* instance = (InstanceScript*)creature->GetInstanceScript();

        if (instance->GetData(DATA_LICH_KING_EVENT) == DONE)
            return false;

        if (creature->IsQuestGiver())
			player->PrepareQuestMenu(creature->GetGUID());

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "We run away!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        InstanceScript* instance = (InstanceScript*)creature->GetInstanceScript();

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->CLOSE_GOSSIP_MENU();
                ((npc_jaina_and_sylvana_hor_part2AI*)creature->AI())->Start(false, true);
                creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
                creature->SetUInt64Value(UNIT_FIELD_TARGET, 0);
                creature->SetActive(true);

				instance->SetData64(DATA_ESCAPE_LIDER, creature->GetGUID());
				instance->SetData(DATA_LICH_KING_EVENT, IN_PROGRESS);

				return true;
			default:
				return false;
        }
    }

    struct npc_jaina_and_sylvana_hor_part2AI : public npc_escortAI
    {
        npc_jaina_and_sylvana_hor_part2AI(Creature* creature) : npc_escortAI(creature)
        {
            instance = (InstanceScript*)creature->GetInstanceScript();
            Reset();
        }

        InstanceScript* instance;

		Creature* pLichKing;

        uint32 CastTimer;
        uint32 StepTimer;
        uint32 Step;
        int32 HoldTimer;
        uint32 Count;
		uint32 ChestID;

		uint64 LichKingGUID;
        uint64 LiderGUID;
        uint64 IceWallGUID;
        uint64 WallTargetGUID;
        uint64 Captain;

        bool Fight;
        bool Event;
        bool PreFight;
        bool WallCast;

        void Reset()
        {
            if (instance->GetData(DATA_LICH_KING_EVENT) == IN_PROGRESS)
                return;

            Step = 0;
            StepTimer = 500;
            Fight = true;
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
            WallTargetGUID = 0;

            if (me->GetEntry() == NPC_JAINA_OUTRO)
            {
                me->CastSpell(me, SPELL_ICE_BARRIER, false);
                me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
            }

            if (instance->GetData(DATA_LICH_KING_EVENT) == DONE)
                me->SetVisible(false);
        }

        void AttackStart(Unit* who)
        {
            if (!who)
                return;

            if (me->GetEntry() != NPC_SYLVANAS_OUTRO)
                return;

            if (instance->GetData(DATA_LICH_KING_EVENT) == IN_PROGRESS || Fight != true)
                return;

            npc_escortAI::AttackStart(who);
        }

        void JustDied(Unit* /*killer*/)
        {
            instance->SetData(DATA_LICH_KING_EVENT, FAIL);
        }

        void WaypointReached(uint32 id)
        {
            switch(id)
            {
                case 3:
                    instance->SetData(DATA_ICE_WALL_1, IN_PROGRESS);
                    if (GameObject* gate = instance->instance->GetGameObject(instance->GetData64(GO_ICE_WALL_1)))
                    {
                        gate->SetGoState(GO_STATE_READY);
                        IceWallGUID = gate->GetGUID();
                    }
                    break;
                case 4:
                    if (me->GetEntry() == NPC_JAINA_OUTRO)
                        DoSendQuantumText(SAY_JAINA_WALL_01, me);

                    if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                        DoSendQuantumText(SAY_SYLVANA_WALL_01, me);
                    CastTimer = 1000;
                    HoldTimer = 30000;
                    SetEscortPaused(true);
                    if (Creature* walltrigger = me->SummonCreature(NPC_ICE_WALL, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 720000))
                    {
                        WallTargetGUID = walltrigger->GetGUID();
                        walltrigger->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        if (me->GetEntry() == NPC_JAINA_OUTRO)
                            me->CastSpell(me, SPELL_DESTROY_ICE_WALL_01, false);
                    }
                    WallCast = true;
                    break;
                case 6:
                    instance->SetData(DATA_ICE_WALL_2, IN_PROGRESS);
                    if (Creature* walltrigger = instance->instance->GetCreature(WallTargetGUID))
                    {
                        if (walltrigger->IsAlive())
                        {
                            walltrigger->DespawnAfterAction();
                            WallTargetGUID = 0;
                        }
                    }
                    break;
                case 8:
                    if (me->GetEntry() == NPC_JAINA_OUTRO)
                        DoSendQuantumText(SAY_JAINA_WALL_02, me);

                    if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                        DoSendQuantumText(SAY_SYLVANA_WALL_02, me);

                    CastTimer = 1000;
                    HoldTimer = 30000;
                    SetEscortPaused(true);
                    if (Creature* walltrigger = me->SummonCreature(NPC_ICE_WALL, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 720000))
                    {
                        WallTargetGUID = walltrigger->GetGUID();
                        walltrigger->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

                        if (me->GetEntry() == NPC_JAINA_OUTRO)
                            me->CastSpell(me, SPELL_DESTROY_ICE_WALL_01, false);
                    }
                    WallCast = true;
                    break;
                case 9:
                    if (me->GetEntry() == NPC_JAINA_OUTRO)
                        DoSendQuantumText(SAY_JAINA_ESCAPE_01, me);

                    if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                        DoSendQuantumText(SAY_SYLVANA_ESCAPE_01, me);
                    break;
                case 11:
                    instance->SetData(DATA_ICE_WALL_3, IN_PROGRESS);
                    if (Creature* walltrigger = instance->instance->GetCreature(WallTargetGUID))
                    {
                        if (walltrigger->IsAlive())
                        {
                            walltrigger->DespawnAfterAction();
                            WallTargetGUID = 0;
                        }
                    }
                    break;
                case 12:
                    if (me->GetEntry() == NPC_JAINA_OUTRO)
                        DoSendQuantumText(SAY_JAINA_WALL_03, me);

                    if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                        DoSendQuantumText(SAY_SYLVANA_WALL_03, me);
                    CastTimer = 1000;
                    HoldTimer = 30000;
                    SetEscortPaused(true);
                    if (Creature* walltrigger = me->SummonCreature(NPC_ICE_WALL, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 720000))
                    {
                        WallTargetGUID = walltrigger->GetGUID();
                        walltrigger->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

                        if (me->GetEntry() == NPC_JAINA_OUTRO)
                            me->CastSpell(me, SPELL_DESTROY_ICE_WALL_01, false);
                    }
                    WallCast = true;
                    break;
                case 13:
                    if (me->GetEntry() == NPC_JAINA_OUTRO)
                        DoSendQuantumText(SAY_JAINA_ESCAPE_02, me);

                    if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                        DoSendQuantumText(SAY_SYLVANA_ESCAPE_02, me);
                    break;
                case 15:
                    instance->SetData(DATA_ICE_WALL_4, IN_PROGRESS);
                    if (Creature* walltrigger = instance->instance->GetCreature(WallTargetGUID))
                    {
                        if (walltrigger->IsAlive())
                        {
                            walltrigger->DespawnAfterAction();
                            WallTargetGUID = 0;
                        }
                    }
                    break;
                case 16:
                    if (me->GetEntry() == NPC_JAINA_OUTRO)
                        DoSendQuantumText(SAY_JAINA_WALL_04, me);
                    if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                        DoSendQuantumText(SAY_SYLVANA_WALL_04, me);
                    CastTimer = 1000;
                    HoldTimer = 30000;
                    SetEscortPaused(true);
                    if (Creature* walltrigger = me->SummonCreature(NPC_ICE_WALL, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 720000))
                    {
                        WallTargetGUID = walltrigger->GetGUID();
                        walltrigger->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

                        if (me->GetEntry() == NPC_JAINA_OUTRO)
                            me->CastSpell(me, SPELL_DESTROY_ICE_WALL_01, false);
                    }
                    WallCast = true;
                    break;
                case 19:
                    if (me->GetEntry() == NPC_JAINA_OUTRO)
                        DoSendQuantumText(SAY_JAINA_TRAP, me);

                    if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                        DoSendQuantumText(SAY_SYLVANA_TRAP, me);
                    break;
                case 20:
                    if (Creature* walltrigger = instance->instance->GetCreature(WallTargetGUID))
                    {
                        if (walltrigger->IsAlive())
                        {
                            walltrigger->DespawnAfterAction();
                            WallTargetGUID = 0;
                        }
                    }
                    SetEscortPaused(true);
					if (me->GetEntry() == NPC_JAINA_OUTRO)
                        me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H_L);

                    if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                        me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
                    break;
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (who->GetTypeId() != TYPE_ID_PLAYER)
                return;

            Player* player = (Player *)who;

            if (player->GetTeam() == ALLIANCE && me->GetEntry() == NPC_SYLVANAS_OUTRO)
                return;

            if (player->GetTeam() == HORDE && me->GetEntry() == NPC_JAINA_OUTRO)
                return;

            if (me->IsWithinDistInMap(who, 50.0f) && instance->GetData(DATA_FROSTSWORN_EVENT) == DONE && instance->GetData(DATA_PHASE) == 3)
            {
                player = (Player*)who;
                Event = true;
                me->SetCurrentFaction(FACTION);
                instance->SetData(DATA_PHASE, 4);
            }
        }

        void DamageTaken(Unit* /*DoneBy*/, uint32 damage)
        {
            if (instance->GetData(DATA_LICH_KING_EVENT) != IN_PROGRESS)
            {
				damage = 0;
                return;
            }

            if (instance->GetData(DATA_LICH_KING_EVENT) == IN_PROGRESS && WallCast == true)
            {
                HoldTimer = HoldTimer + 100;
                return;
            }
        }

        void JumpNextStep(uint32 Time)
        {
            StepTimer = Time;
            Step++;
        }

        void Intro()
        {
            switch(Step)
            {
                case 0:
                    me->SetWalk(false);
                    LichKingGUID = instance->GetData64(DATA_LICHKING);
                    pLichKing = instance->instance->GetCreature(LichKingGUID);
                    me->SetUInt64Value(UNIT_FIELD_TARGET, pLichKing->GetGUID());
                    JumpNextStep(100);
                    break;
                case 1:
                    HoRQuestComplete(me->GetEntry());
                    if (pLichKing)
                    {
                        if (me->GetEntry() == NPC_JAINA_OUTRO)
                            DoSendQuantumText(SAY_LICH_KING_AGGRO_A, pLichKing);

                        if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                            DoSendQuantumText(SAY_LICH_KING_AGGRO_H, pLichKing);

						pLichKing->AI()->AttackStart(me);
                        me->AI()->AttackStart(pLichKing);
                    }
                    JumpNextStep(3000);
                    break;
                case 2:
                    if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                    {
                        Fight = false;
                        if (pLichKing)
                            me->GetMotionMaster()->MovePoint(0, (me->GetPositionX()-5)+rand()%10, (me->GetPositionY()-5)+rand()%10, me->GetPositionZ());
                        JumpNextStep(3000);
                    }
                    else
                        JumpNextStep(100);
                    break;
                case 3:
                    if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                        Fight = true;
                    JumpNextStep(100);
                    break;
                case 4:
                    if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                    {
                        if (pLichKing)
                            me->CastSpell(pLichKing, SPELL_SYLVANA_STEP, false);

                        JumpNextStep(3000);
                    }
                    else
                        JumpNextStep(100);
                    break;
                case 5:
                    if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                    {
                        Fight = false;

                        if (pLichKing)
                            me->GetMotionMaster()->MovePoint(0, (me->GetPositionX()-5)+rand()%10, (me->GetPositionY()-5)+rand()%10, me->GetPositionZ());

                        JumpNextStep(3000);
                    }
                    else
                        JumpNextStep(12000);
                    break;
                case 6:
                    Fight = true;

                    if (pLichKing)
                    {
                        if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                            DoCast(SPELL_SYLVANA_JUMP);

                        DoCast(pLichKing, me->GetEntry() == NPC_JAINA_OUTRO ? SPELL_ICE_PRISON_VISUAL : SPELL_DARK_ARROW, false);
                        pLichKing->AttackStop();
                    }
                    JumpNextStep(1480);
                    break;
                case 7:
                    me->RemoveAllAuras();

                    if (pLichKing && !pLichKing->HasAura(SPELL_ICE_PRISON_VISUAL))
                    {
                        pLichKing->AddAura(me->GetEntry() == NPC_JAINA_OUTRO ? SPELL_ICE_PRISON_VISUAL : SPELL_DARK_ARROW, pLichKing);
                        pLichKing->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    }

                    me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_STAND);
                    me->AttackStop();

                    if (me->GetEntry() == NPC_JAINA_OUTRO)
                    {
                        me->RemoveAurasDueToSpell(SPELL_ICE_BARRIER);
                        DoSendQuantumText(SAY_JAINA_AGGRO, me);
                    }

                    if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                        DoSendQuantumText(SAY_SYLVANA_AGGRO, me);

                    JumpNextStep(3000);
                    break;
                case 8:
                    me->GetMotionMaster()->MovePoint(0, 5577.187f, 2236.003f, 733.012f);
                    if (pLichKing && !pLichKing->HasAura(SPELL_ICE_PRISON_VISUAL))
                    {
                        pLichKing->AddAura(me->GetEntry() == NPC_JAINA_OUTRO ? SPELL_ICE_PRISON_VISUAL : SPELL_DARK_ARROW, pLichKing);
                        me->SetUInt64Value(UNIT_FIELD_TARGET, pLichKing->GetGUID());
                    }
                    JumpNextStep(10000);
                    break;
                case 9:
                    if (pLichKing && (!pLichKing->HasAura(SPELL_ICE_PRISON_VISUAL) || !pLichKing->HasAura(SPELL_DARK_ARROW)))
                    {
                        pLichKing->AddAura(me->GetEntry() == NPC_JAINA_OUTRO ? SPELL_ICE_PRISON_VISUAL : SPELL_DARK_ARROW, pLichKing);
                        me->SetUInt64Value(UNIT_FIELD_TARGET, pLichKing->GetGUID());
                    }
                    me->RemoveAllAuras();
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
					Count = 1;
                    JumpNextStep(100);
                    break;
            }
        }

        void Outro()
        {
            switch(Step)
            {
                case 10:
                    JumpNextStep(5000);
                    break;
                case 11:
                    if (Creature* pCaptain = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_CAPTAIN)))
						pCaptain->AI()->DoAction(ACTION_START_FLY);

                    JumpNextStep(10000);
                    break;
                case 12:
                    if (GameObject* pCave = instance->instance->GetGameObject(instance->GetData64(GO_CAVE)))
                    {
                        Creature* pCaveTarget = me->SummonCreature(NPC_ICE_WALL, pCave->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 720000);
                        pCaveTarget->SetFloatValue(OBJECT_FIELD_SCALE_X, 4);

                        for(int8 i = 0; i<4; i++)
                        {
                            if (Creature* cannoner = me->SummonCreature(NPC_ICE_WALL, CannonSpawns[i], TEMPSUMMON_MANUAL_DESPAWN, 86400))
                            {
								cannoner->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);
                                cannoner->SetCurrentFaction(me->GetFaction());
                                cannoner->CastSpell(pCaveTarget, SPELL_FIRE_CANNON, true);
                            }
                        }
                    }        
                    JumpNextStep(4000);
                    break;
                case 13:
                    me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_STAND);

                    if (GameObject* cave = instance->instance->GetGameObject(instance->GetData64(GO_CAVE)))
                        cave->SetGoState(GO_STATE_READY);

					me->RemoveAllAuras();

                    if (pLichKing)
						pLichKing->DespawnAfterAction();

                    HoRQuestComplete(NPC_KILL_CREDIT_LICH_KING);
                    JumpNextStep(10000);
                    break;
                case 14:
                    DoSendQuantumText(me->GetEntry() == NPC_JAINA_OUTRO ? SAY_JAINA_FINAL_3 : SAY_SYLVANA_FINAL_2, me);
                    JumpNextStep(25000);
                    break;
				case 15:
					if (me->GetEntry() == NPC_JAINA_OUTRO)
					{
						DoSendQuantumText(SAY_JAINA_FINAL_2, me);
						if (Creature* jaina = me->SummonCreature(NPC_JAINA_QUEST, 5256.78f, 1684.58f, 784.301f, 4.98793f, TEMPSUMMON_MANUAL_DESPAWN))
							jaina->SetHealth(250000);

						me->DespawnAfterAction();
					}

					if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
					{
						if (Creature* sylvanas = me->SummonCreature(NPC_SYLVANAS_QUEST, 5256.78f, 1684.58f, 784.301f, 4.98793f, TEMPSUMMON_MANUAL_DESPAWN))
							sylvanas->SetHealth(250000);

						me->DespawnAfterAction();
					}
					break;
            }
        }

		void HoRQuestComplete(uint32 killCredit)
        {
			Map::PlayerList const &PlayerList = instance->instance->GetPlayers();

			if (PlayerList.isEmpty())
				return;

			for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
			{
				if (Player* player = itr->getSource())
					itr->getSource()->KilledMonsterCredit(killCredit, 0);
            }
        }

        void UpdateEscortAI(const uint32 diff)
        {
            if (!instance || !Event)
                return;

            DoMeleeAttackIfReady();

            if (instance->GetData(DATA_PHASE) == 4 && instance->GetData(DATA_LICH_KING_EVENT) != IN_PROGRESS)
            {
                if (StepTimer <= diff)
                    Intro();

                else StepTimer -= diff;
            }

            if (instance->GetData(DATA_LICH_KING_EVENT) == SPECIAL && instance->GetData(DATA_PHASE) != 6) // End Cinematic
            {
                instance->SetData(DATA_PHASE, 6);
                Step = 10;
            }

            if (instance->GetData(DATA_PHASE) == 6)
            {
                if (StepTimer <= diff)
                    Outro();
                else
                    StepTimer -= diff;
                return;
            }

            if (WallCast == true && CastTimer <= diff)
            {
                if (me->GetEntry() == NPC_SYLVANAS_OUTRO)
                {
                   if (Creature* walltrigger = instance->instance->GetCreature(WallTargetGUID))
				   {
                        me->CastSpell(walltrigger, SPELL_DESTROY_ICE_WALL_03, false);
						CastTimer = 1000;
				   }
                }
            }
            else CastTimer -= diff;

            if (WallCast == true && HoldTimer < 10000 && (instance->GetData(DATA_SUMMONS) == 0 || !me->IsInCombatActive()))
            {
                WallCast = false;
                me->InterruptNonMeleeSpells(false);
                SetEscortPaused(false);

                if (GameObject* gate = instance->instance->GetGameObject(IceWallGUID))
                    gate->SetGoState(GO_STATE_ACTIVE);

                ++Count;
                switch(Count)
                {
                    case 2:
                        if (GameObject* gate = instance->instance->GetGameObject(instance->GetData64(GO_ICE_WALL_2)))
                        {
                            gate->SetGoState(GO_STATE_READY);

                            if (pLichKing && pLichKing->IsAlive())
                                DoSendQuantumText(SAY_LICH_KING_WALL_02, pLichKing);

                            IceWallGUID = gate->GetGUID();
                        }
                        break;
                    case 3:
                        if (GameObject* gate = instance->instance->GetGameObject(instance->GetData64(GO_ICE_WALL_3)))
                        {
                            gate->SetGoState(GO_STATE_READY);

                            if (pLichKing && pLichKing->IsAlive())
                                DoSendQuantumText(SAY_LICH_KING_WALL_03, pLichKing);

                            IceWallGUID = gate->GetGUID();
                        }
                        break;
                    case 4:
                        if (GameObject* gate = instance->instance->GetGameObject(instance->GetData64(GO_ICE_WALL_4)))
                        {
                            gate->SetGoState(GO_STATE_READY);

                            if (pLichKing && pLichKing->IsAlive())
                                DoSendQuantumText(SAY_LICH_KING_WALL_04, pLichKing);

                            IceWallGUID = gate->GetGUID();
                        }
                        break;
                    case 5:
                        if (pLichKing && pLichKing->IsAlive())
                        {
                            pLichKing->RemoveAurasDueToSpell(SPELL_WINTER);
							pLichKing->SetSpeed(MOVE_RUN, 3.28f, true);
                            pLichKing->SetSpeed(MOVE_WALK, 3.28f, true);
                            Step = 0;
                        }
                        break;
				}
            }
            else
            {
                HoldTimer -= diff;
				if (HoldTimer <= 0)
					HoldTimer = 0;
            }
            return;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jaina_and_sylvana_hor_part2AI(creature);
    }
};

class npc_capitan_gunship: public CreatureScript
{
    public:
        npc_capitan_gunship() : CreatureScript("npc_capitan_gunship") { }

        struct npc_capitan_gunshipAI : public QuantumBasicAI
        {
            npc_capitan_gunshipAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

			Transport* gunship;
			EventMap events;
			InstanceScript* instance;

            void Reset()
            {
				gunship = me->GetTransport();
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_START_FLY:
                        StartFly(gunship);
                        events.ScheduleEvent(EVENT_OUTRO_1, 6000);
                        break;
                }
            }

            void UpdateAI(const uint32 diff)
            {
                events.Update(diff);
                
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_OUTRO_1:
                            if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                            {
                                DoSendQuantumText(SAY_BARTLETT_FIRE, me);
                                events.ScheduleEvent(EVENT_OUTRO_2, 5500);
                            }
							else
                            {
                                DoSendQuantumText(SAY_KROM_FIRE, me);
                                events.ScheduleEvent(EVENT_OUTRO_2, 5500);
                            }
                            StopFly(gunship);
                            break;
                        case EVENT_OUTRO_2:
                            StartFly(gunship);
                            events.ScheduleEvent(EVENT_OUTRO_3, 4000);
                            break;
                        case EVENT_OUTRO_3:
                            StopFly(gunship);
                            instance->SetData(DATA_LICH_KING_EVENT, DONE);
                            events.ScheduleEvent(EVENT_OUTRO_4, 20000);
                            if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                                DoSendQuantumText(SAY_JAINA_FINAL_1, me);
							else
                                DoSendQuantumText(SAY_SYLVANA_FINAL_1, me);
                            break;
                        case EVENT_OUTRO_4:
                            if (instance->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                            {
								DoSendQuantumText(SAY_BARTLETT_BOARD, me);
                                me->SummonGameObject(GO_PORTAL, 5261.8f, 1643.63f, 784.302f, 1.14382f, 0, 0, 0, 0, 7*DAY);
                                me->SummonGameObject(DUNGEON_MODE(GO_CAPTAIN_CHEST_2, GO_CAPTAIN_CHEST_4), 5245.7f, 1659.32f, 784.302f, 5.70699f, 0, 0, 0, 0, 7*DAY);
                            }
							else
                            {
								DoSendQuantumText(SAY_KROM_BOARD, me);
                                me->SummonGameObject(GO_PORTAL, 5261.8f, 1643.63f, 784.302f, 1.14382f, 0, 0, 0, 0, 7*DAY);
                                me->SummonGameObject(DUNGEON_MODE(GO_CAPTAIN_CHEST_1, GO_CAPTAIN_CHEST_3), 5245.7f, 1659.32f, 784.302f, 5.70699f, 0, 0, 0, 0, 7*DAY);
                            }
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_capitan_gunshipAI(creature);
        }
};

enum Texts
{
	SAY_QUEST_QUEL  = -1652000,
    SAY_QUEST_QUEL1 = -1652001,
    SAY_QUEST_QUEL2 = -1652002,
    SAY_QUEST_QUEL3 = -1652003,
    SAY_QUEST_QUEL4 = -1652004,
    SAY_QUEST_QUEL5 = -1652006,
    SAY_QUEST_QUEL6 = -1652007,
    SAY_QUEST_QUEL7 = -1652008,
};

enum QuelDelar
{
	SPELL_MORTAL_STRIKE    = 16856,
	SPELL_HERIOC_STRIKE    = 29426,
	SPELL_BLADESTORM       = 67541,
	SPELL_QUEL_DELAR_AURA  = 70013,
	SPELL_WHIRLWIND_VISUAL = 70300,
};

class npc_quel_delar : public CreatureScript
{
public:
    npc_quel_delar() : CreatureScript("npc_quel_delar") { }

    struct npc_quel_delarAI  : public QuantumBasicAI
    {
        npc_quel_delarAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;
		Creature* Uther;

		bool StartEvent;

        void Reset()
        {
            me->SetVisible(false);
            StartEvent = false;
			Uther = 0;
        }

		void MoveInLineOfSight(Unit* who)
		{
           if (!StartEvent)
           {
			   if (who->GetTypeId() != TYPE_ID_PLAYER || who->ToPlayer()->GetQuestStatus(QUEST_THE_HALLS_OF_REFFLECTION_A) != QUEST_STATUS_INCOMPLETE && who->ToPlayer()->GetQuestStatus(QUEST_THE_HALLS_OF_REFFLECTION_H) != QUEST_STATUS_INCOMPLETE)
			   {
				   me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				   me->SetVisible(false);
				   return;
			   }

			   if (me->IsWithinDistInMap(who, 35.0f))
			   {
				   me->SetVisible(true);
				   me->SetInCombatWithZone();
				   events.ScheduleEvent(EVENT_QUEST_QUEL_1, 100);
				   StartEvent = true;
			   }
		   }
		}

		void KilledUnit(Unit* /*victim*/)
        {
			Uther->DespawnAfterAction();
		}

		void JustDied(Unit* /*killer*/)
        {
            if (Creature* uther = me->FindCreatureWithDistance(NPC_UTHER, 100.0f, true))
                uther->AI()->DoAction(1);
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;
        
            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_QUEST_QUEL_1:
						DoCast(me, SPELL_CAST_VISUAL);
						if (Creature* pUther = me->SummonCreature(NPC_UTHER, 5297.84f, 1985.44f, 707.696f, 3.93116f, TEMPSUMMON_MANUAL_DESPAWN))
						{
							Uther = pUther;
							Uther->SetVisible(true);
							DoSendQuantumText(SAY_QUEST_QUEL, Uther);
						}
						events.ScheduleEvent(EVENT_QUEST_QUEL_2, 8000);
						break;
					case EVENT_QUEST_QUEL_2:
						DoSendQuantumText(SAY_QUEST_QUEL1, me);
						DoCast(me, SPELL_WHIRLWIND_VISUAL);
						events.ScheduleEvent(EVENT_QUEST_QUEL_3, 3000);
						break;
					case EVENT_QUEST_QUEL_3:
						DoSendQuantumText(SAY_QUEST_QUEL2, Uther);
						events.ScheduleEvent(EVENT_QUEST_QUEL_4, 7000);
						break;
					case EVENT_QUEST_QUEL_4:
						Uther->SetWalk(false);
						Uther->GetMotionMaster()->MovePoint(0, 5335.5f, 1981.74f, 709.32f);
						events.ScheduleEvent(EVENT_QUEST_QUEL_5, 6000);
						break;
					case EVENT_QUEST_QUEL_5:
						Uther->SetFacingToObject(me);
						DoSendQuantumText(SAY_QUEST_QUEL3, me);
						events.ScheduleEvent(EVENT_ATTACK_START, 3000);
						break;
					case EVENT_ATTACK_START:
						me->RemoveAurasDueToSpell(SPELL_WHIRLWIND_VISUAL);
						me->SetReactState(REACT_AGGRESSIVE);
						Uther->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SIT);

						if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
						{
							me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
							AttackStart(target);
						}
						events.ScheduleEvent(EVENT_MORTAL_STRIKE, 2000);
						events.ScheduleEvent(EVENT_HEROIC_STRIKE, 4000);
						events.ScheduleEvent(EVENT_BLADESTORM, 6000);
						break;
					case EVENT_MORTAL_STRIKE:
						if (UpdateVictim())
						{
							DoCastVictim(SPELL_MORTAL_STRIKE);
							events.ScheduleEvent(EVENT_MORTAL_STRIKE, 4000);
						}
						break;
					case EVENT_HEROIC_STRIKE:
						if (UpdateVictim())
						{
							DoCastVictim(SPELL_HERIOC_STRIKE);
							events.ScheduleEvent(EVENT_HEROIC_STRIKE, 6000);
						}
						break;
					case EVENT_BLADESTORM:
						if (UpdateVictim())
						{
							DoCastAOE(SPELL_BLADESTORM);
							events.ScheduleEvent(EVENT_BLADESTORM, 8000);
						}
						break;
				}

                DoMeleeAttackIfReady();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_quel_delarAI(creature);
    }
};

class npc_uther : public CreatureScript
{
public:
    npc_uther() : CreatureScript("npc_uther") { }

    struct npc_utherAI  : public QuantumBasicAI
    {
        npc_utherAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void DoAction(const int32 actionId)
        {
            if (actionId == 1)
				events.ScheduleEvent(EVENT_QUEST_QUEL_6, 100);
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);
        
            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case EVENT_QUEST_QUEL_6:
                    me->HandleEmoteCommand(EMOTE_STATE_STAND);
                    me->SetWalk(true);
                    me->GetMotionMaster()->MovePoint(0, 5309.5f, 1991.74f, 707.32f);
                    events.ScheduleEvent(EVENT_QUEST_QUEL_7, 5000);
                    break;
                case EVENT_QUEST_QUEL_7:
                    DoSendQuantumText(SAY_QUEST_QUEL4, me);
                    events.ScheduleEvent(EVENT_QUEST_QUEL_8, 10000);
                    break;
                case EVENT_QUEST_QUEL_8:
                    DoSendQuantumText(SAY_QUEST_QUEL5, me);
                    events.ScheduleEvent(EVENT_QUEST_QUEL_9, 10000);
                    break;
                case EVENT_QUEST_QUEL_9:
                    DoSendQuantumText(SAY_QUEST_QUEL6, me);
                    events.ScheduleEvent(EVENT_QUEST_QUEL_10, 10000);
                    break;    
                case EVENT_QUEST_QUEL_10:
                    DoSendQuantumText(SAY_QUEST_QUEL7, me);
                    events.ScheduleEvent(EVENT_QUEST_QUEL_11, 5000);
                    break;
                case EVENT_QUEST_QUEL_11:
                    me->CastSpell(me, SPELL_UTHER_DESPAWN, false);
                    me->DespawnAfterAction(2000);
                    break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_utherAI(creature);
    }
};

void AddSC_halls_of_reflection()
{
    new npc_jaina_or_sylvanas_hor(true, "npc_sylvanas_hor_part1");
    new npc_jaina_or_sylvanas_hor(false, "npc_jaina_hor_part1");
    new npc_jaina_and_sylvana_hor_part2();
    new npc_frostsworn_general();
    new npc_capitan_gunship();
    new npc_quel_delar();
    new npc_uther();
}