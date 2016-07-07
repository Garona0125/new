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
#include <cmath>

enum DraeneiSurvivor
{
    SAY_HEAL1           = -1000176,
    SAY_HEAL2           = -1000177,
    SAY_HEAL3           = -1000178,
    SAY_HEAL4           = -1000179,

    SAY_HELP1           = -1000180,
    SAY_HELP2           = -1000181,
    SAY_HELP3           = -1000182,
    SAY_HELP4           = -1000183,

    SPELL_IRRIDATION    = 35046,
    SPELL_STUNNED       = 28630,
};

class npc_draenei_survivor : public CreatureScript
{
public:
    npc_draenei_survivor() : CreatureScript("npc_draenei_survivor") { }

    struct npc_draenei_survivorAI : public QuantumBasicAI
    {
        npc_draenei_survivorAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint64 Caster;

        uint32 SayThanksTimer;
        uint32 RunAwayTimer;
        uint32 SayHelpTimer;

        bool CanSayHelp;

        void Reset()
        {
            Caster = 0;

            SayThanksTimer = 0;
            RunAwayTimer = 0;
            SayHelpTimer = 10000;

            CanSayHelp = true;

            DoCast(me, SPELL_IRRIDATION, true);

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE | UNIT_FLAG_IN_COMBAT);
            me->SetHealth(me->CountPctFromMaxHealth(HEALTH_PERCENT_10));
            me->SetStandState(UNIT_STAND_STATE_SLEEP);
        }

        void EnterToBattle(Unit* /*who*/) {}

        void MoveInLineOfSight(Unit* who)
        {
            if (CanSayHelp && who->GetTypeId() == TYPE_ID_PLAYER && me->IsFriendlyTo(who) && me->IsWithinDistInMap(who, 25.0f))
            {
                DoSendQuantumText(RAND(SAY_HELP1, SAY_HELP2, SAY_HELP3, SAY_HELP4), me, who);

                SayHelpTimer = 20000;
                CanSayHelp = false;
            }
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->SpellFamilyFlags[2] & 0x080000000)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
                me->SetStandState(UNIT_STAND_STATE_STAND);
                DoCast(me, SPELL_STUNNED, true);
                Caster = caster->GetGUID();

                SayThanksTimer = 5000;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (SayThanksTimer)
            {
                if (SayThanksTimer <= diff)
				{
					me->RemoveAurasDueToSpell(SPELL_IRRIDATION);

                    if (Player* player = Unit::GetPlayer(*me, Caster))
                    {
                        DoSendQuantumText(RAND(SAY_HEAL1, SAY_HEAL2, SAY_HEAL3, SAY_HEAL4), me, player);

                        player->TalkedToCreature(me->GetEntry(), me->GetGUID());
                    }

                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MovePoint(0, -4115.053711f, -13754.831055f, 73.508949f);

                    RunAwayTimer = 10000;
                    SayThanksTimer = 0;
                }
				else SayThanksTimer -= diff;

                return;
            }

            if (RunAwayTimer)
            {
                if (RunAwayTimer <= diff)
				{
                    me->DespawnAfterAction();
				}
                else RunAwayTimer -= diff;

                return;
            }

            if (SayHelpTimer <= diff)
            {
                CanSayHelp = true;
                SayHelpTimer = 20000;
            }
			else SayHelpTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_draenei_survivorAI (creature);
    }
};

enum Overgrind
{
    SAY_TEXT        = -1000184,
    SAY_EMOTE       = -1000185,
    ATTACK_YELL     = -1000186,

    AREA_COVE       = 3579,
    AREA_ISLE       = 3639,
    QUEST_GNOMERCY  = 9537,
    FACTION_HOSTILE = 14,
    SPELL_DYNAMITE  = 7978
};

#define GOSSIP_FIGHT "Traitor! You will be brought to justice!"

class npc_engineer_spark_overgrind : public CreatureScript
{
public:
    npc_engineer_spark_overgrind() : CreatureScript("npc_engineer_spark_overgrind") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_GNOMERCY) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_FIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->SetCurrentFaction(FACTION_HOSTILE);
            CAST_AI(npc_engineer_spark_overgrind::npc_engineer_spark_overgrindAI, creature->AI())->AttackStart(player);
        }
        return true;
    }

    struct npc_engineer_spark_overgrindAI : public QuantumBasicAI
    {
        npc_engineer_spark_overgrindAI(Creature* creature) : QuantumBasicAI(creature)
        {
            NormFaction = creature->GetFaction();
            NpcFlags = creature->GetUInt32Value(UNIT_NPC_FLAGS);

            if (creature->GetAreaId() == AREA_COVE || creature->GetAreaId() == AREA_ISLE)
                IsTreeEvent = true;
        }

        uint32 NormFaction;
        uint32 NpcFlags;
        uint32 DynamiteTimer;
        uint32 EmoteTimer;

        bool IsTreeEvent;

        void Reset()
        {
            DynamiteTimer = 8000;
            EmoteTimer = urand(120000, 150000);

            me->SetCurrentFaction(NormFaction);
            me->SetUInt32Value(UNIT_NPC_FLAGS, NpcFlags);

            IsTreeEvent = false;
        }

        void EnterToBattle(Unit* who)
        {
            DoSendQuantumText(ATTACK_YELL, me, who);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!me->IsInCombatActive() && !IsTreeEvent)
            {
                if (EmoteTimer <= diff)
                {
                    DoSendQuantumText(SAY_TEXT, me);
                    DoSendQuantumText(SAY_EMOTE, me);
                    EmoteTimer = urand(120000, 150000);
                }
				else EmoteTimer -= diff;
            }
            else if (IsTreeEvent)
                return;

            if (!UpdateVictim())
                return;

            if (DynamiteTimer <= diff)
            {
                DoCastVictim(SPELL_DYNAMITE);
                DynamiteTimer = 8000;
            }
			else DynamiteTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_engineer_spark_overgrindAI(creature);
    }
};

class npc_injured_draenei : public CreatureScript
{
public:
    npc_injured_draenei() : CreatureScript("npc_injured_draenei") { }

    struct npc_injured_draeneiAI : public QuantumBasicAI
    {
        npc_injured_draeneiAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
            me->SetHealth(me->CountPctFromMaxHealth(HEALTH_PERCENT_15));
            switch (urand(0, 1))
            {
                case 0:
					me->SetStandState(UNIT_STAND_STATE_SIT);
					break;
                case 1:
					me->SetStandState(UNIT_STAND_STATE_SLEEP);
					break;
            }
        }

        void EnterToBattle(Unit* /*who*/) {}

        void MoveInLineOfSight(Unit* /*who*/) {}

        void UpdateAI(const uint32 /*diff*/) {}

    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_injured_draeneiAI (creature);
    }
};

enum Magwin
{
    SAY_START                = -1000111,
    SAY_AGGRO                = -1000112,
    SAY_PROGRESS             = -1000113,
    SAY_END1                 = -1000114,
    SAY_END2                 = -1000115,
    EMOTE_HUG                = -1000116,

    QUEST_A_CRY_FOR_SAY_HELP = 9528,
};

class npc_magwin : public CreatureScript
{
public:
    npc_magwin() : CreatureScript("npc_magwin") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_A_CRY_FOR_SAY_HELP)
        {
            creature->SetCurrentFaction(113);
            if (npc_escortAI* EscortAI = CAST_AI(npc_escortAI, creature->AI()))
                EscortAI->Start(true, false, player->GetGUID());
        }
        return true;
    }

    struct npc_magwinAI : public npc_escortAI
    {
        npc_magwinAI(Creature* creature) : npc_escortAI(creature) {}

		void Reset() {}

		void EnterToBattle(Unit* who)
        {
            DoSendQuantumText(SAY_AGGRO, me, who);
        }

        void WaypointReached(uint32 waypointId)
        {
            if (Player* player = GetPlayerForEscort())
            {
                switch (waypointId)
                {
                    case 0:
                        DoSendQuantumText(SAY_START, me, player);
                        break;
                    case 17:
                        DoSendQuantumText(SAY_PROGRESS, me, player);
                        break;
                    case 28:
                        DoSendQuantumText(SAY_END1, me, player);
                        break;
                    case 29:
                        DoSendQuantumText(EMOTE_HUG, me, player);
                        DoSendQuantumText(SAY_END2, me, player);
                        player->GroupEventHappens(QUEST_A_CRY_FOR_SAY_HELP, me);
                        break;
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_magwinAI(creature);
    }
};

enum Geezle
{
    QUEST_TREES_COMPANY = 9531,

    SPELL_TREE_DISGUISE = 30298,

    GEEZLE_SAY_1    = -1000629,
    SPARK_SAY_2     = -1000630,
    SPARK_SAY_3     = -1000631,
    GEEZLE_SAY_4    = -1000632,
    SPARK_SAY_5     = -1000633,
    SPARK_SAY_6     = -1000634,
    GEEZLE_SAY_7    = -1000635,
    EMOTE_SPARK     = -1000636,

    NPC_SPARK       = 17243,
    GO_NAGA_FLAG    = 181694,
};

Position const SparkPos = {-5029.91f, -11291.79f, 8.096f, 0.0f};

class npc_geezle : public CreatureScript
{
public:
    npc_geezle() : CreatureScript("npc_geezle") { }

    struct npc_geezleAI : public QuantumBasicAI
    {
        npc_geezleAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint64 SparkGUID;

        uint8 Step;
        uint32 SayTimer;

        bool EventStarted;

        void Reset()
        {
            SparkGUID = 0;
            Step = 0;
            StartEvent();
        }

        void EnterToBattle(Unit* /*who*/){}

        void StartEvent()
        {
            Step = 0;
            EventStarted = true;

            if (Creature* Spark = me->SummonCreature(NPC_SPARK, SparkPos, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000))
            {
                SparkGUID = Spark->GetGUID();
                Spark->SetActive(true);
                Spark->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            }
            SayTimer = 8000;
        }

        uint32 NextStep(uint8 Step)
        {
            Creature* Spark = Unit::GetCreature(*me, SparkGUID);

            switch (Step)
            {
                case 0:
                    if (Spark)
                        Spark->GetMotionMaster()->MovePoint(0, -5080.70f, -11253.61f, 0.56f);
                    me->GetMotionMaster()->MovePoint(0, -5092.26f, -11252, 0.71f);
                    return 9000; // NPCs are walking up to fire
                case 1:
                    DespawnNagaFlag(true);
                    DoSendQuantumText(EMOTE_SPARK, Spark);
                    return 1000;
                case 2:
                    DoSendQuantumText(GEEZLE_SAY_1, me, Spark);
                    if (Spark)
                    {
                        Spark->SetInFront(me);
                        me->SetInFront(Spark);
                    }
                    return 5000;
                case 3:
					DoSendQuantumText(SPARK_SAY_2, Spark);
					return 7000;
                case 4:
					DoSendQuantumText(SPARK_SAY_3, Spark);
					return 8000;
                case 5:
					DoSendQuantumText(GEEZLE_SAY_4, me, Spark);
					return 8000;
                case 6:
					DoSendQuantumText(SPARK_SAY_5, Spark);
					return 9000;
                case 7:
					DoSendQuantumText(SPARK_SAY_6, Spark);
					return 8000;
                case 8:
					DoSendQuantumText(GEEZLE_SAY_7, me, Spark);
					return 2000;
                case 9:
                    me->GetMotionMaster()->MoveTargetedHome();
                    if (Spark)
                        Spark->GetMotionMaster()->MovePoint(0, SparkPos);
                    CompleteQuest();
                    return 9000;
                case 10:
                    if (Spark)
                        Spark->DisappearAndDie();
                    DespawnNagaFlag(false);
                    me->DisappearAndDie();
                default:
					return 99999999;
            }
        }

        void CompleteQuest()
        {
            float radius = 50.0f;
            std::list<Player*> players;
            Trinity::AnyPlayerInObjectRangeCheck checker(me, radius);
            Trinity::PlayerListSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(me, players, checker);
            me->VisitNearbyWorldObject(radius, searcher);

            for (std::list<Player*>::const_iterator itr = players.begin(); itr != players.end(); ++itr)
			{
                if ((*itr)->GetQuestStatus(QUEST_TREES_COMPANY) == QUEST_STATUS_INCOMPLETE && (*itr)->HasAura(SPELL_TREE_DISGUISE))
                    (*itr)->KilledMonsterCredit(NPC_SPARK, 0);
			}
        }

        void DespawnNagaFlag(bool despawn)
        {
            std::list<GameObject*> FlagList;
            me->GetGameObjectListWithEntryInGrid(FlagList, GO_NAGA_FLAG, 100.0f);

            if (!FlagList.empty())
            {
                for (std::list<GameObject*>::const_iterator itr = FlagList.begin(); itr != FlagList.end(); ++itr)
                {
                    if (despawn)
                        (*itr)->SetLootState(GO_JUST_DEACTIVATED);
                    else
                        (*itr)->Respawn();
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (SayTimer <= diff)
            {
                if (EventStarted)
                    SayTimer = NextStep(Step++);
            }
            else SayTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_geezleAI(creature);
    }
};

enum RavegerCage
{
    SPELL_REND            = 13443,
    SPELL_ENRAGING_BITE   = 30736,

	NPC_DEATH_RAVAGER     = 17556,
    QUEST_STRENGTH_ONE    = 9582,
};

class go_ravager_cage : public GameObjectScript
{
public:
    go_ravager_cage() : GameObjectScript("go_ravager_cage") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(QUEST_STRENGTH_ONE) == QUEST_STATUS_INCOMPLETE)
        {
            if (Creature* ravager = go->FindCreatureWithDistance(NPC_DEATH_RAVAGER, 5.0f, true))
            {
                ravager->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                ravager->SetReactState(REACT_AGGRESSIVE);
                ravager->AI()->AttackStart(player);
            }
        }
        return true;
    }
};

class npc_death_ravager : public CreatureScript
{
public:
    npc_death_ravager() : CreatureScript("npc_death_ravager") { }

    struct npc_death_ravagerAI : public QuantumBasicAI
    {
        npc_death_ravagerAI(Creature* creature) : QuantumBasicAI(creature){}

        uint32 RendTimer;
        uint32 EnragingBiteTimer;

        void Reset()
        {
            RendTimer = 30000;
            EnragingBiteTimer = 20000;

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetReactState(REACT_PASSIVE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (RendTimer <= diff)
            {
                DoCastVictim(SPELL_REND);
                RendTimer = 30000;
            }
            else RendTimer -= diff;

            if (EnragingBiteTimer <= diff)
            {
                DoCastVictim(SPELL_ENRAGING_BITE);
                EnragingBiteTimer = 15000;
            }
            else EnragingBiteTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_death_ravagerAI(creature);
    }
};

enum BristlelimbCage
{
    QUEST_THE_PROPHECY_OF_AKIDA    = 9544,
    NPC_STILLPINE_CAPITIVE         = 17375,
    GO_BRISTELIMB_CAGE             = 181714,

    CAPITIVE_SAY_1                 = -1000474,
    CAPITIVE_SAY_2                 = -1000475,
    CAPITIVE_SAY_3                 = -1000476,

    POINT_INIT                     = 1,
    EVENT_DESPAWN                  = 1,
};

class npc_stillpine_capitive : public CreatureScript
{
    public:
        npc_stillpine_capitive() : CreatureScript("npc_stillpine_capitive") { }

        struct npc_stillpine_capitiveAI : public QuantumBasicAI
        {
            npc_stillpine_capitiveAI(Creature* creature) : QuantumBasicAI(creature){}

			Player* player;
            EventMap events;
            bool movementComplete;

            void Reset()
            {
                if (GameObject* cage = me->FindGameobjectWithDistance(GO_BRISTELIMB_CAGE, 5.0f))
                {
                    cage->SetLootState(GO_JUST_DEACTIVATED);
                    cage->SetGoState(GO_STATE_READY);
                }
                events.Reset();
                player = NULL;
                movementComplete = false;
            }

            void StartMoving(Player* owner)
            {
                if (owner)
                {
                    DoSendQuantumText(RAND(CAPITIVE_SAY_1, CAPITIVE_SAY_2, CAPITIVE_SAY_3), me, owner);
                    player = owner;
                }
                Position pos;
                me->GetNearPosition(pos, 3.0f, 0.0f);
                me->GetMotionMaster()->MovePoint(POINT_INIT, pos);
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE || id != POINT_INIT)
                    return;

                if (player)
                    player->KilledMonsterCredit(me->GetEntry(), me->GetGUID());

                movementComplete = true;
                events.ScheduleEvent(EVENT_DESPAWN, 3500);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!movementComplete)
                    return;

                events.Update(diff);

                if (events.ExecuteEvent() == EVENT_DESPAWN)
                    me->DespawnAfterAction();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_stillpine_capitiveAI(creature);
        }
};

class go_bristlelimb_cage : public GameObjectScript
{
public:
	go_bristlelimb_cage() : GameObjectScript("go_bristlelimb_cage") { }

	bool OnGossipHello(Player* player, GameObject* go)
	{
		if (player->GetQuestStatus(QUEST_THE_PROPHECY_OF_AKIDA) == QUEST_STATUS_INCOMPLETE)
		{
			if (Creature* capitive = go->FindCreatureWithDistance(NPC_STILLPINE_CAPITIVE, 5.0f, true))
			{
				go->ResetDoorOrButton();
				CAST_AI(npc_stillpine_capitive::npc_stillpine_capitiveAI, capitive->AI())->StartMoving(player);
				return false;
			}
		}
		return true;
	}
};

void AddSC_azuremyst_isle()
{
    new npc_draenei_survivor();
    new npc_engineer_spark_overgrind();
    new npc_injured_draenei();
    new npc_magwin();
    new npc_geezle();
    new npc_death_ravager();
    new go_ravager_cage();
    new npc_stillpine_capitive();
    new go_bristlelimb_cage();
}