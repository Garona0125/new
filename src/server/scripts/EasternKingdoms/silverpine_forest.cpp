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

#define GOSSIP_HAH "You're Astor Hadren, right?"
#define GOSSIP_SAH "You've got something I need, Astor. And I'll be taking it now."

class npc_astor_hadren : public CreatureScript
{
public:
    npc_astor_hadren() : CreatureScript("npc_astor_hadren") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
		if (player->GetQuestStatus(14420) == QUEST_STATUS_INCOMPLETE)
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HAH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        player->SEND_GOSSIP_MENU(623, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 Action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (Action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SAH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(624, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                player->CLOSE_GOSSIP_MENU();
                creature->SetCurrentFaction(21);
                if (player)
                    CAST_AI(npc_astor_hadren::npc_astor_hadrenAI, creature->AI())->AttackStart(player);
                break;
        }
        return true;
    }

    struct npc_astor_hadrenAI : public QuantumBasicAI
    {
        npc_astor_hadrenAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset()
        {
            me->SetCurrentFaction(68);
        }

        void EnterToBattle(Unit* /*who*/) {}

        void JustDied(Unit* /*who*/)
        {
			me->SetCurrentFaction(68);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_astor_hadrenAI(creature);
    }
};

enum Erland
{
    SAY_QUESTACCEPT     = 0,
    SAY_START           = 1,
    SAY_AGGRO           = 2,
    SAY_PROGRESS        = 3,
    SAY_LAST            = 4,

    SAY_RANE            = 0,
    SAY_RANE_ANSWER     = 5,
    SAY_MOVE_QUINN      = 6,

    SAY_QUINN           = 7,
    SAY_QUINN_ANSWER    = 0,
    SAY_BYE             = 8,

    QUEST_ESCORTING     = 435,
    NPC_RANE            = 1950,
    NPC_QUINN           = 1951
};

class npc_deathstalker_erland : public CreatureScript
{
public:
    npc_deathstalker_erland() : CreatureScript("npc_deathstalker_erland") { }

    struct npc_deathstalker_erlandAI : public npc_escortAI
    {
        npc_deathstalker_erlandAI(Creature* creature) : npc_escortAI(creature) {}

        void WaypointReached(uint32 waypointId)
        {
            Player* player = GetPlayerForEscort();

            if (!player)
                return;

            switch (waypointId)
            {
                case 1:
                    Talk(SAY_START, player->GetGUID());
                    break;
                case 10:
                    Talk(SAY_PROGRESS);
                    break;
                case 13:
                    Talk(SAY_LAST, player->GetGUID());
                    player->GroupEventHappens(QUEST_ESCORTING, me);
                    break;
                case 15:
                    if (Creature* rane = me->FindCreatureWithDistance(NPC_RANE, 20.0f))
                        rane->AI()->Talk(SAY_RANE);
                    break;
                case 16:
                    Talk(SAY_RANE_ANSWER);
                    break;
                case 17:
                    Talk(SAY_MOVE_QUINN);
                    break;
                case 24:
                    Talk(SAY_QUINN);
                    break;
                case 25:
                    if (Creature* quinn = me->FindCreatureWithDistance(NPC_QUINN, 20.0f))
                        quinn->AI()->Talk(SAY_QUINN_ANSWER);
                    break;
                case 26:
                    Talk(SAY_BYE);
                    break;
            }
        }

        void Reset() {}

        void EnterToBattle(Unit* who)
        {
            Talk(SAY_AGGRO, who->GetGUID());
        }
    };

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_ESCORTING)
        {
            creature->AI()->Talk(SAY_QUESTACCEPT, player->GetGUID());

            if (npc_escortAI* EscortAI = CAST_AI(npc_deathstalker_erland::npc_deathstalker_erlandAI, creature->AI()))
                EscortAI->Start(true, false, player->GetGUID());
        }

        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_deathstalker_erlandAI(creature);
    }
};

#define QUEST_PYREWOOD_AMBUSH 452

#define NPC_SAY_INIT "Get ready, they'll be arriving any minute..." //not blizzlike
#define NPC_SAY_END "Thanks for your help!" //not blizzlike

static float PyrewoodSpawnPoints[3][4] =
{
    //pos_x   pos_y     pos_z    orien
    //outside
    /*
    {-400.85f, 1513.64f, 18.67f, 0},
    {-397.32f, 1514.12f, 18.67f, 0},
    {-397.44f, 1511.09f, 18.67f, 0},
    */
    //door
    {-396.17f, 1505.86f, 19.77f, 0},
    {-396.91f, 1505.77f, 19.77f, 0},
    {-397.94f, 1504.74f, 19.77f, 0},
};

#define WAIT_SECS 6000

class pyrewood_ambush : public CreatureScript
{
public:
    pyrewood_ambush() : CreatureScript("pyrewood_ambush") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest *quest)
    {
        if (quest->GetQuestId() == QUEST_PYREWOOD_AMBUSH && !CAST_AI(pyrewood_ambush::pyrewood_ambushAI, creature->AI())->QuestInProgress)
        {
            CAST_AI(pyrewood_ambush::pyrewood_ambushAI, creature->AI())->QuestInProgress = true;
            CAST_AI(pyrewood_ambush::pyrewood_ambushAI, creature->AI())->Phase = 0;
            CAST_AI(pyrewood_ambush::pyrewood_ambushAI, creature->AI())->KillCount = 0;
            CAST_AI(pyrewood_ambush::pyrewood_ambushAI, creature->AI())->PlayerGUID = player->GetGUID();
        }
        return true;
    }

    struct pyrewood_ambushAI : public QuantumBasicAI
    {
        pyrewood_ambushAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
           QuestInProgress = false;
        }

        uint32 Phase;
        int8 KillCount;
        uint32 WaitTimer;
        uint64 PlayerGUID;
        SummonList Summons;

        bool QuestInProgress;

        void Reset()
        {
            WaitTimer = WAIT_SECS;

            if (!QuestInProgress)
            {
                Phase = 0;
                KillCount = 0;
                PlayerGUID = 0;
                Summons.DespawnAll();
            }
        }

        void EnterToBattle(Unit* /*who*/){}

        void JustSummoned(Creature* summoned)
        {
            Summons.Summon(summoned);
            ++KillCount;
        }

        void SummonedCreatureDespawn(Creature* summoned)
        {
            Summons.Despawn(summoned);
            --KillCount;
        }

        void SummonCreatureWithRandomTarget(uint32 creatureId, int position)
        {
            if (Creature* summoned = me->SummonCreature(creatureId, PyrewoodSpawnPoints[position][0], PyrewoodSpawnPoints[position][1], PyrewoodSpawnPoints[position][2], PyrewoodSpawnPoints[position][3], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15000))
            {
                Unit* target = NULL;
                if (PlayerGUID)
				{
                    if (Player* player = Unit::GetPlayer(*me, PlayerGUID))
					{
                        if (player->IsAlive() && RAND(0, 1))
                            target = player;
					}
				}

                if (!target)
                    target = me;

                summoned->SetCurrentFaction(168);
                summoned->AddThreat(target, 32.0f);
                summoned->AI()->AttackStart(target);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            if (PlayerGUID)
			{
                if (Player* player = Unit::GetPlayer(*me, PlayerGUID))
				{
                    if (player->GetQuestStatus(QUEST_PYREWOOD_AMBUSH) == QUEST_STATUS_INCOMPLETE)
                        player->FailQuest(QUEST_PYREWOOD_AMBUSH);
				}
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (!QuestInProgress)
                return;

            if (KillCount && Phase < 6)
            {
                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
                return;
            }

            switch (Phase)
            {
                case 0:
                    if (WaitTimer == WAIT_SECS)
                        me->MonsterSay(NPC_SAY_INIT, LANG_UNIVERSAL, 0); // no blizzlike

                    if (WaitTimer <= diff)
                    {
                        WaitTimer -= diff;
                        return;
                    }
                    break;
                case 1:
                    SummonCreatureWithRandomTarget(2060, 1);
                    break;
                case 2:
                    SummonCreatureWithRandomTarget(2061, 2);
                    SummonCreatureWithRandomTarget(2062, 0);
                    break;
                case 3:
                    SummonCreatureWithRandomTarget(2063, 1);
                    SummonCreatureWithRandomTarget(2064, 2);
                    SummonCreatureWithRandomTarget(2065, 0);
                    break;
                case 4:
                    SummonCreatureWithRandomTarget(2066, 1);
                    SummonCreatureWithRandomTarget(2067, 0);
                    SummonCreatureWithRandomTarget(2068, 2);
                    break;
                case 5:
                    if (PlayerGUID)
                    {
                        if (Player* player = Unit::GetPlayer(*me, PlayerGUID))
                        {
                            me->MonsterSay(NPC_SAY_END, LANG_UNIVERSAL, 0); //not blizzlike
                            player->GroupEventHappens(QUEST_PYREWOOD_AMBUSH, me);
                        }
                    }
                    QuestInProgress = false;
                    Reset();
                    break;
            }
            ++Phase;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new pyrewood_ambushAI(creature);
    }
};

void AddSC_silverpine_forest()
{
    new npc_astor_hadren();
    new npc_deathstalker_erland();
    new pyrewood_ambush();
}