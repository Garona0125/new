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
#include "QuantumFollowerAI.h"

enum Kerlonian
{
    SAY_KER_START               = -1000434,

    EMOTE_KER_SLEEP_1           = -1000435,
    EMOTE_KER_SLEEP_2           = -1000436,
    EMOTE_KER_SLEEP_3           = -1000437,

    SAY_KER_SLEEP_1             = -1000438,
    SAY_KER_SLEEP_2             = -1000439,
    SAY_KER_SLEEP_3             = -1000440,
    SAY_KER_SLEEP_4             = -1000441,

    EMOTE_KER_AWAKEN            = -1000445,

    SAY_KER_ALERT_1             = -1000442,
    SAY_KER_ALERT_2             = -1000443,

    SAY_KER_END                 = -1000444,

    SPELL_SLEEP_VISUAL          = 25148,
    SPELL_AWAKEN                = 17536,
    QUEST_SLEEPER_AWAKENED      = 5321,
    NPC_LILADRIS                = 11219,                    //attackers entries unknown
    FACTION_KER_ESCORTEE        = 113
};

//TODO: make concept similar as "ringo" -escort. Find a way to run the scripted attacks, _if_ player are choosing road.
class npc_kerlonian : public CreatureScript
{
public:
    npc_kerlonian() : CreatureScript("npc_kerlonian") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_SLEEPER_AWAKENED)
        {
            if (npc_kerlonianAI* pKerlonianAI = CAST_AI(npc_kerlonian::npc_kerlonianAI, creature->AI()))
            {
                creature->SetStandState(UNIT_STAND_STATE_STAND);
                DoSendQuantumText(SAY_KER_START, creature, player);
                pKerlonianAI->StartFollow(player, FACTION_KER_ESCORTEE, quest);
            }
        }
        return true;
    }

    struct npc_kerlonianAI : public FollowerAI
    {
        npc_kerlonianAI(Creature* creature) : FollowerAI(creature) { }

        uint32 FallAsleepTimer;

        void Reset()
        {
            FallAsleepTimer = urand(10000, 45000);
        }

        void MoveInLineOfSight(Unit* who)
        {
            FollowerAI::MoveInLineOfSight(who);

            if (!me->GetVictim() && !HasFollowState(STATE_FOLLOW_COMPLETE) && who->GetEntry() == NPC_LILADRIS)
            {
                if (me->IsWithinDistInMap(who, INTERACTION_DISTANCE*5))
                {
                    if (Player* player = GetLeaderForFollower())
                    {
                        if (player->GetQuestStatus(QUEST_SLEEPER_AWAKENED) == QUEST_STATUS_INCOMPLETE)
                            player->GroupEventHappens(QUEST_SLEEPER_AWAKENED, me);

                        DoSendQuantumText(SAY_KER_END, me);
                    }

                    SetFollowComplete();
                }
            }
        }

        void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (HasFollowState(STATE_FOLLOW_INPROGRESS | STATE_FOLLOW_PAUSED) && spell->Id == SPELL_AWAKEN)
                ClearSleeping();
        }

        void SetSleeping()
        {
            SetFollowPaused(true);
            DoSendQuantumText(RAND(EMOTE_KER_SLEEP_1, EMOTE_KER_SLEEP_2, EMOTE_KER_SLEEP_3), me);
            DoSendQuantumText(RAND(SAY_KER_SLEEP_1, SAY_KER_SLEEP_2, SAY_KER_SLEEP_3, SAY_KER_SLEEP_4), me);
            me->SetStandState(UNIT_STAND_STATE_SLEEP);
            DoCast(me, SPELL_SLEEP_VISUAL, false);
        }

        void ClearSleeping()
        {
            me->RemoveAurasDueToSpell(SPELL_SLEEP_VISUAL);
            me->SetStandState(UNIT_STAND_STATE_STAND);

            DoSendQuantumText(EMOTE_KER_AWAKEN, me);

            SetFollowPaused(false);
        }

        void UpdateFollowerAI(const uint32 diff)
        {
            if (!UpdateVictim())
            {
                if (!HasFollowState(STATE_FOLLOW_INPROGRESS))
                    return;

                if (!HasFollowState(STATE_FOLLOW_PAUSED))
                {
                    if (FallAsleepTimer <= diff)
                    {
                        SetSleeping();
                        FallAsleepTimer = urand(25000, 90000);
                    }
                    else FallAsleepTimer -= diff;
                }
                return;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_kerlonianAI(creature);
    }
};

enum Remtravel
{
    SAY_REM_START               = -1000327,
    SAY_REM_AGGRO               = -1000328,
    SAY_REM_RAMP1_1             = -1000329,
    SAY_REM_RAMP1_2             = -1000330,
    SAY_REM_BOOK                = -1000331,
    SAY_REM_TENT1_1             = -1000332,
    SAY_REM_TENT1_2             = -1000333,
    SAY_REM_MOSS                = -1000334,
    EMOTE_REM_MOSS              = -1000335,
    SAY_REM_MOSS_PROGRESS       = -1000336,
    SAY_REM_PROGRESS            = -1000337,
    SAY_REM_REMEMBER            = -1000338,
    EMOTE_REM_END               = -1000339,

    FACTION_ESCORTEE            = 10,

    QUEST_ABSENT_MINDED_PT2     = 731,

    NPC_GRAVEL_SCOUT            = 2158,
    NPC_GRAVEL_BONE             = 2159,
    NPC_GRAVEL_GEO              = 2160
};

class npc_prospector_remtravel : public CreatureScript
{
public:
    npc_prospector_remtravel() : CreatureScript("npc_prospector_remtravel") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_ABSENT_MINDED_PT2)
        {
            if (npc_escortAI* EscortAI = CAST_AI(npc_prospector_remtravel::npc_prospector_remtravelAI, creature->AI()))
                EscortAI->Start(false, false, player->GetGUID());

            creature->SetCurrentFaction(FACTION_ESCORTEE);
        }
        return true;
    }

    struct npc_prospector_remtravelAI : public npc_escortAI
    {
        npc_prospector_remtravelAI(Creature* creature) : npc_escortAI(creature) {}

		void Reset(){}

        void EnterToBattle(Unit* who)
        {
            if (urand(0, 1))
                DoSendQuantumText(SAY_REM_AGGRO, me, who);
        }

        void JustSummoned(Creature* /*summoned*/)
        {
            //unsure if it should be any
            //summoned->AI()->AttackStart(me);
        }

        void WaypointReached(uint32 waypointId)
        {
            if (Player* player = GetPlayerForEscort())
            {
                switch (waypointId)
                {
                    case 0:
                        DoSendQuantumText(SAY_REM_START, me, player);
                        break;
                    case 5:
                        DoSendQuantumText(SAY_REM_RAMP1_1, me, player);
                        break;
                    case 6:
                        DoSpawnCreature(NPC_GRAVEL_SCOUT, -10.0f, 5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                        DoSpawnCreature(NPC_GRAVEL_BONE, -10.0f, 7.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                        break;
                    case 9:
                        DoSendQuantumText(SAY_REM_RAMP1_2, me, player);
                        break;
                    case 14:
                        //depend quest rewarded?
                        DoSendQuantumText(SAY_REM_BOOK, me, player);
                        break;
                    case 15:
                        DoSendQuantumText(SAY_REM_TENT1_1, me, player);
                        break;
                    case 16:
                        DoSpawnCreature(NPC_GRAVEL_SCOUT, -10.0f, 5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                        DoSpawnCreature(NPC_GRAVEL_BONE, -10.0f, 7.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                        break;
                    case 17:
                        DoSendQuantumText(SAY_REM_TENT1_2, me, player);
                        break;
                    case 26:
                        DoSendQuantumText(SAY_REM_MOSS, me, player);
                        break;
                    case 27:
                        DoSendQuantumText(EMOTE_REM_MOSS, me, player);
                        break;
                    case 28:
                        DoSendQuantumText(SAY_REM_MOSS_PROGRESS, me, player);
                        break;
                    case 29:
                        DoSpawnCreature(NPC_GRAVEL_SCOUT, -15.0f, 3.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                        DoSpawnCreature(NPC_GRAVEL_BONE, -15.0f, 5.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                        DoSpawnCreature(NPC_GRAVEL_GEO, -15.0f, 7.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                        break;
                    case 31:
                        DoSendQuantumText(SAY_REM_PROGRESS, me, player);
                        break;
                    case 41:
                        DoSendQuantumText(SAY_REM_REMEMBER, me, player);
                        break;
                    case 42:
                        DoSendQuantumText(EMOTE_REM_END, me, player);
                        player->GroupEventHappens(QUEST_ABSENT_MINDED_PT2, me);
                        break;
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_prospector_remtravelAI(creature);
    }
};

enum Threshwackonator
{
    EMOTE_START        = -1000325,
    SAY_AT_CLOSE       = -1000326,

    QUEST_GYROMAST_REV = 2078,

    NPC_GELKAK         = 6667,

    FACTION_HOSTILE    = 16,
};

#define GOSSIP_ITEM_INSERT_KEY "Insert key"

class npc_threshwackonator : public CreatureScript
{
public:
    npc_threshwackonator() : CreatureScript("npc_threshwackonator") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_GYROMAST_REV) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_INSERT_KEY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->CLOSE_GOSSIP_MENU();

            if (npc_threshwackonatorAI* ThreshAI = CAST_AI(npc_threshwackonator::npc_threshwackonatorAI, creature->AI()))
            {
                DoSendQuantumText(EMOTE_START, creature);
                ThreshAI->StartFollow(player);
            }
        }
        return true;
    }

    struct npc_threshwackonatorAI : public FollowerAI
    {
        npc_threshwackonatorAI(Creature* creature) : FollowerAI(creature) { }

        void Reset() {}

        void MoveInLineOfSight(Unit* who)
        {
            FollowerAI::MoveInLineOfSight(who);

            if (!me->GetVictim() && !HasFollowState(STATE_FOLLOW_COMPLETE) && who->GetEntry() == NPC_GELKAK)
            {
                if (me->IsWithinDistInMap(who, 10.0f))
                {
                    DoSendQuantumText(SAY_AT_CLOSE, who);
                    DoAtEnd();
                }
            }
        }

        void DoAtEnd()
        {
            me->SetCurrentFaction(FACTION_HOSTILE);

            if (Player* holder = GetLeaderForFollower())
                me->AI()->AttackStart(holder);

            SetFollowComplete();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_threshwackonatorAI(creature);
    }
};

void AddSC_darkshore()
{
    new npc_kerlonian();
    new npc_prospector_remtravel();
    new npc_threshwackonator();
}