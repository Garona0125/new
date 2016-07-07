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
#include "QuantumEscortAI.h"

enum OOX
{
	SAY_OOX_START           = 0,
	SAY_OOX_AGGRO           = 1,
	SAY_OOX_AMBUSH          = 3,
	SAY_OOX_AMBUSH_REPLY    = 4,
	SAY_OOX_END             = 5,

    QUEST_RESQUE_OOX_09     = 836,

    NPC_MARAUDING_OWL       = 7808,
    NPC_VILE_AMBUSHER       = 7809,

    FACTION_ESCORTEE_A      = 774,
    FACTION_ESCORTEE_H      = 775,
};

class npc_00x09hl : public CreatureScript
{
public:
    npc_00x09hl() : CreatureScript("npc_00x09hl") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_RESQUE_OOX_09)
        {
            creature->SetStandState(UNIT_STAND_STATE_STAND);

            if (player->GetTeam() == ALLIANCE)
                creature->SetCurrentFaction(FACTION_ESCORTEE_A);
            else if (player->GetTeam() == HORDE)
                creature->SetCurrentFaction(FACTION_ESCORTEE_H);

            creature->AI()->Talk(SAY_OOX_START, player->GetGUID());

            if (npc_00x09hlAI* EscortAI = CAST_AI(npc_00x09hl::npc_00x09hlAI, creature->AI()))
                EscortAI->Start(false, false, player->GetGUID(), quest);
        }
        return true;
    }

    struct npc_00x09hlAI : public npc_escortAI
    {
        npc_00x09hlAI(Creature* creature) : npc_escortAI(creature) { }

        void Reset() { }

        void WaypointReached(uint32 id)
        {
            switch (id)
            {
                case 26:
					Talk(SAY_OOX_AMBUSH);
                    for (uint8 i = 0; i < 3; ++i)
                    {
                        const Position src = {147.927444f, -3851.513428f, 130.893f, 0};
                        Position dst;
                        me->GetRandomPoint(src, 7.0f, dst);
                        me->SummonCreature(NPC_MARAUDING_OWL, dst, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    }
                    break;
                case 43:
                    Talk(SAY_OOX_AMBUSH);
                    for (uint8 i = 0; i < 3; ++i)
                    {
                        const Position src = {-141.151581f, -4291.213867f, 120.130f, 0};
                        Position dst;
                        me->GetRandomPoint(src, 7.0f, dst);
                        me->SummonCreature(NPC_VILE_AMBUSHER, dst, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    }
                    break;
                case 64:
                    Talk(SAY_OOX_END);
                    if (Player* player = GetPlayerForEscort())
                        player->GroupEventHappens(QUEST_RESQUE_OOX_09, me);
                    break;
            }
        }

        void EnterToBattle(Unit* who)
        {
            if (who->GetEntry() == NPC_MARAUDING_OWL || who->GetEntry() == NPC_VILE_AMBUSHER)
                return;

			Talk(SAY_OOX_AGGRO);
        }

        void JustSummoned(Creature* summoned)
        {
            summoned->AI()->AttackStart(me);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_00x09hlAI(creature);
    }
};

enum Rinji
{
	SAY_RIN_BY_OUTRUNNER    = 0,

	SAY_RIN_FREE            = 0, //from here
	SAY_RIN_HELP            = 1,
	SAY_RIN_COMPLETE        = 2,
	SAY_RIN_PROGRESS_1      = 3,
	SAY_RIN_PROGRESS_2      = 4,

    QUEST_RINJI_TRAPPED     = 2742,
    NPC_RANGER              = 2694,
    NPC_OUTRUNNER           = 2691,
    GO_RINJI_CAGE           = 142036,
};

struct Location
{
    float m_fX, m_fY, m_fZ;
};

Location m_afAmbushSpawn[] =
{
    {191.296204f, -2839.329346f, 107.388f},
    {70.972466f,  -2848.674805f, 109.459f}
};

Location m_afAmbushMoveTo[] =
{
    {166.630386f, -2824.780273f, 108.153f},
    {70.886589f,  -2874.335449f, 116.675f}
};

class npc_rinji : public CreatureScript
{
public:
    npc_rinji() : CreatureScript("npc_rinji") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_RINJI_TRAPPED)
        {
            if (GameObject* go = creature->FindGameobjectWithDistance(GO_RINJI_CAGE, INTERACTION_DISTANCE))
                go->UseDoorOrButton();

            if (npc_rinjiAI* EscortAI = CAST_AI(npc_rinji::npc_rinjiAI, creature->AI()))
                EscortAI->Start(false, false, player->GetGUID(), quest);
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_rinjiAI(creature);
    }

    struct npc_rinjiAI : public npc_escortAI
    {
        npc_rinjiAI(Creature* creature) : npc_escortAI(creature)
        {
            m_bIsByOutrunner = false;
            SpawnId = 0;
        }

        bool m_bIsByOutrunner;
        uint32 PostEventCount;
        uint32 PostEventTimer;
        int SpawnId;

        void Reset()
        {
            PostEventCount = 0;
            PostEventTimer = 3000;
        }

        void JustRespawned()
        {
            m_bIsByOutrunner = false;
            SpawnId = 0;

            npc_escortAI::JustRespawned();
        }

        void EnterToBattle(Unit* who)
        {
            if (HasEscortState(STATE_ESCORT_ESCORTING))
            {
                if (who->GetEntry() == NPC_OUTRUNNER && !m_bIsByOutrunner)
                {
                    if (Creature* talker = who->ToCreature())
						talker->AI()->Talk(SAY_RIN_BY_OUTRUNNER);
                    m_bIsByOutrunner = true;
                }

                if (rand()%4)
                    return;

                //only if attacked and escorter is not in combat?
                Talk(SAY_RIN_HELP);
            }
        }

        void DoSpawnAmbush(bool bFirst)
        {
            if (!bFirst)
                SpawnId = 1;

            me->SummonCreature(NPC_RANGER,
                m_afAmbushSpawn[SpawnId].m_fX, m_afAmbushSpawn[SpawnId].m_fY, m_afAmbushSpawn[SpawnId].m_fZ, 0.0f,
                TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60000);

            for (int i = 0; i < 2; ++i)
                me->SummonCreature(NPC_OUTRUNNER, m_afAmbushSpawn[SpawnId].m_fX, m_afAmbushSpawn[SpawnId].m_fY, m_afAmbushSpawn[SpawnId].m_fZ, 0.0f, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 60000);
        }

        void JustSummoned(Creature* summoned)
        {
            summoned->SetWalk(false);
            summoned->GetMotionMaster()->MovePoint(0, m_afAmbushMoveTo[SpawnId].m_fX, m_afAmbushMoveTo[SpawnId].m_fY, m_afAmbushMoveTo[SpawnId].m_fZ);
        }

        void WaypointReached(uint32 uiPointId)
        {
            Player* player = GetPlayerForEscort();

            if (!player)
                return;

            switch (uiPointId)
            {
                case 1:
                    Talk(SAY_RIN_FREE, player->GetGUID());
                    break;
                case 7:
                    DoSpawnAmbush(true);
                    break;
                case 13:
                    DoSpawnAmbush(false);
                    break;
                case 17:
					Talk(SAY_RIN_COMPLETE, player->GetGUID());
                    player->GroupEventHappens(QUEST_RINJI_TRAPPED, me);
                    SetRun();
                    PostEventCount = 1;
                    break;
            }
        }

        void UpdateEscortAI(const uint32 diff)
        {
            if (!UpdateVictim())
            {
                if (HasEscortState(STATE_ESCORT_ESCORTING) && PostEventCount)
                {
                    if (PostEventTimer <= diff)
                    {
                        PostEventTimer = 3000;

                        if (Unit* player = GetPlayerForEscort())
                        {
                            switch (PostEventCount)
                            {
                                case 1:
									Talk(SAY_RIN_PROGRESS_1, player->GetGUID());
                                    ++PostEventCount;
                                    break;
                                case 2:
									Talk(SAY_RIN_PROGRESS_2, player->GetGUID());
                                    PostEventCount = 0;
                                    break;
                            }
                        }
                        else
                        {
                            me->DespawnAfterAction();
                            return;
                        }
                    }
                    else PostEventTimer -= diff;
                }
                return;
            }
            DoMeleeAttackIfReady();
        }
    };
};

void AddSC_hinterlands()
{
    new npc_00x09hl();
    new npc_rinji();
}