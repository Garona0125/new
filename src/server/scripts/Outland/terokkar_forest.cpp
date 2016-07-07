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
#include "QuantumSystemText.h"
#include "QuantumEscortAI.h"
#include "QuantumFollowerAI.h"
#include "SpellScript.h"
#include "Group.h"

enum Unkor
{
	SAY_SUBMIT                  = -1000194,

	FACTION_HOSTILE             = 45,
	FACTION_FRIENDLY            = 35,

	QUEST_DONT_KILL_THE_FATONE  = 9889,

	SPELL_PULVERIZE             = 2676,
	//SPELL_QUID9889            = 32174,
};

class npc_unkor_the_ruthless : public CreatureScript
{
public:
    npc_unkor_the_ruthless() : CreatureScript("npc_unkor_the_ruthless") { }

    struct npc_unkor_the_ruthlessAI : public QuantumBasicAI
    {
        npc_unkor_the_ruthlessAI(Creature* creature) : QuantumBasicAI(creature) {}

        bool CanDoQuest;
        uint32 UnkorUnfriendlyTimer;
        uint32 PulverizeTimer;

        void Reset()
        {
            CanDoQuest = false;
            UnkorUnfriendlyTimer = 0;
            PulverizeTimer = 3000;
            me->SetStandState(UNIT_STAND_STATE_STAND);
            me->SetCurrentFaction(FACTION_HOSTILE);
        }

        void EnterToBattle(Unit* /*who*/) {}

        void DoNice()
        {
            DoSendQuantumText(SAY_SUBMIT, me);
            me->SetCurrentFaction(FACTION_FRIENDLY);
            me->SetStandState(UNIT_STAND_STATE_SIT);
            me->RemoveAllAuras();
            me->DeleteThreatList();
            me->CombatStop(true);
            UnkorUnfriendlyTimer = 60000;
        }

        void DamageTaken(Unit* doneby, uint32 &damage)
        {
			if (doneby->GetTypeId() == TYPE_ID_PLAYER)
				if (me->HealthBelowPctDamaged(HEALTH_PERCENT_30, damage))
				{
					if (Group* group = CAST_PLR(doneby)->GetGroup())
					{
						for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
						{
							Player* group = itr->getSource();
							if (group &&
								group->GetQuestStatus(QUEST_DONT_KILL_THE_FATONE) == QUEST_STATUS_INCOMPLETE &&
								group->GetReqKillOrCastCurrentCount(QUEST_DONT_KILL_THE_FATONE, 18260) == 10)
							{
								group->AreaExploredOrEventHappens(QUEST_DONT_KILL_THE_FATONE);
								if (!CanDoQuest)
									CanDoQuest = true;
							}
						}
					}
					else
						if (CAST_PLR(doneby)->GetQuestStatus(QUEST_DONT_KILL_THE_FATONE) == QUEST_STATUS_INCOMPLETE &&
							CAST_PLR(doneby)->GetReqKillOrCastCurrentCount(QUEST_DONT_KILL_THE_FATONE, 18260) == 10)
						{
							CAST_PLR(doneby)->AreaExploredOrEventHappens(QUEST_DONT_KILL_THE_FATONE);
							CanDoQuest = true;
						}
				}
		}

        void UpdateAI(const uint32 diff)
        {
            if (CanDoQuest)
            {
                if (!UnkorUnfriendlyTimer)
                {
                    //DoCast(me, SPELL_QUID9889);        //not using spell for now
                    DoNice();
                }
                else
                {
                    if (UnkorUnfriendlyTimer <= diff)
                    {
                        EnterEvadeMode();
                        return;
                    }
					else UnkorUnfriendlyTimer -= diff;
                }
            }

            if (!UpdateVictim())
                return;

            if (PulverizeTimer <= diff)
            {
                DoCast(me, SPELL_PULVERIZE);
                PulverizeTimer = 9000;
            }
			else PulverizeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unkor_the_ruthlessAI(creature);
    }
};

class npc_skywing : public CreatureScript
{
public:
    npc_skywing() : CreatureScript("npc_skywing") { }

    struct npc_skywingAI : public npc_escortAI
    {
    public:
        npc_skywingAI(Creature* creature) : npc_escortAI(creature) {}

        void WaypointReached(uint32 id)
        {
            Player* player = GetPlayerForEscort();

            if (!player)
                return;

            switch (id)
            {
                case 8:
                    player->AreaExploredOrEventHappens(10898);
                    break;
            }
        }

        void EnterToBattle(Unit* /*who*/) {}

        void MoveInLineOfSight(Unit* who)
        {
            if (HasEscortState(STATE_ESCORT_ESCORTING))
                return;

            if (who->GetTypeId() == TYPE_ID_PLAYER)
            {
                if (CAST_PLR(who)->GetQuestStatus(10898) == QUEST_STATUS_INCOMPLETE)
                {
                    float Radius = 10.0f;

                    if (me->IsWithinDistInMap(who, Radius))
                        Start(false, false, who->GetGUID());
                }
            }
        }

        void Reset() {}

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skywingAI(creature);
    }
};

#define QUEST_TARGET        22459
//#define SPELL_FREE_WEBBED   38950

const uint32 netherwebVictims[6] = {18470, 16805, 21242, 18452, 22482, 21285};

class mob_netherweb_victim : public CreatureScript
{
public:
    mob_netherweb_victim() : CreatureScript("mob_netherweb_victim") { }

    struct mob_netherweb_victimAI : public QuantumBasicAI
    {
        mob_netherweb_victimAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset(){}

        void EnterToBattle(Unit* /*who*/){}

        void MoveInLineOfSight(Unit* /*who*/){}

        void JustDied(Unit* killer)
        {
			if (killer->GetTypeId() == TYPE_ID_PLAYER)
			{
				if (CAST_PLR(killer)->GetQuestStatus(10873) == QUEST_STATUS_INCOMPLETE)
				{
					if (rand()%100 < 25)
                    {
						me->SummonCreature(QUEST_TARGET, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                        CAST_PLR(killer)->KilledMonsterCredit(QUEST_TARGET, 0);
                        return;
					}
				}
			}
			me->SummonCreature(netherwebVictims[rand()%6], 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
		}
    };
	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_netherweb_victimAI(creature);
    }
};

#define GOSSIP_FLOON1 "You owe Sim'salabim money. Hand them over or die!"
#define GOSSIP_FLOON2 "Hand over the money or die...again!"

enum Floon
{
    SAY_FLOON_ATTACK     = -1000195,
    SPELL_SILENCE        = 6726,
    SPELL_FROSTBOLT      = 9672,
    SPELL_FROST_NOVA     = 11831,
    FACTION_HOSTILE_FL   = 1738,
    QUEST_CRACK_SKULLS   = 10009,
};

class npc_floon : public CreatureScript
{
public:
    npc_floon() : CreatureScript("npc_floon") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_CRACK_SKULLS) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_FLOON1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(9442, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_FLOON2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(9443, creature->GetGUID());
        }

        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->SetCurrentFaction(FACTION_HOSTILE_FL);
            DoSendQuantumText(SAY_FLOON_ATTACK, creature, player);
            creature->AI()->AttackStart(player);
        }
        return true;
    }

    struct npc_floonAI : public QuantumBasicAI
    {
        npc_floonAI(Creature* creature) : QuantumBasicAI(creature)
        {
            NormFaction = creature->GetFaction();
        }

        uint32 NormFaction;
        uint32 SilenceTimer;
        uint32 FrostboltTimer;
        uint32 FrostNovaTimer;

        void Reset()
        {
            SilenceTimer = 2000;
            FrostboltTimer = 4000;
            FrostNovaTimer = 9000;

            if (me->GetFaction() != NormFaction)
                me->SetCurrentFaction(NormFaction);
        }

        void EnterToBattle(Unit* /*who*/) { }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (SilenceTimer <= diff)
            {
                DoCastVictim(SPELL_SILENCE);
                SilenceTimer = 30000;
            }
			else SilenceTimer -= diff;

            if (FrostNovaTimer <= diff)
            {
                DoCast(me, SPELL_FROST_NOVA);
                FrostNovaTimer = 20000;
            }
			else FrostNovaTimer -= diff;

            if (FrostboltTimer <= diff)
            {
                DoCastVictim(SPELL_FROSTBOLT);
                FrostboltTimer = 5000;
            }
			else FrostboltTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_floonAI(creature);
    }
};

enum IslaStarmaneData
{
    SAY_PROGRESS_1  = -1000571,
    SAY_PROGRESS_2  = -1000572,
    SAY_PROGRESS_3  = -1000573,
    SAY_PROGRESS_4  = -1000574,

    QUEST_EFTW_H    = 10052,
    QUEST_EFTW_A    = 10051,
    GO_CAGE         = 182794,
    SPELL_CAT       = 32447,
};

class npc_isla_starmane : public CreatureScript
{
public:
    npc_isla_starmane() : CreatureScript("npc_isla_starmane") { }

    struct npc_isla_starmaneAI : public npc_escortAI
    {
        npc_isla_starmaneAI(Creature* creature) : npc_escortAI(creature) {}

        void WaypointReached(uint32 id)
        {
            Player* player = GetPlayerForEscort();

            if (!player)
                return;

            switch (id)
            {
			case 0:
				{
					GameObject* Cage = me->FindGameobjectWithDistance(GO_CAGE, 10.0f);
					if (Cage)
						Cage->SetGoState(GO_STATE_ACTIVE);
				}
				break;
			case 2:
				DoSendQuantumText(SAY_PROGRESS_1, me, player);
				break;
            case 5:
				DoSendQuantumText(SAY_PROGRESS_2, me, player);
				break;
            case 6:
				DoSendQuantumText(SAY_PROGRESS_3, me, player);
				break;
            case 29:
				DoSendQuantumText(SAY_PROGRESS_4, me, player);
                if (player)
                {
                    if (player->GetTeam() == ALLIANCE)
                        player->GroupEventHappens(QUEST_EFTW_A, me);
                    else if (player->GetTeam() == HORDE)
                        player->GroupEventHappens(QUEST_EFTW_H, me);
                }
                me->SetInFront(player);
				break;
            case 30:
				me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
				break;
            case 31:
				DoCast(me, SPELL_CAT);
                me->SetWalk(false);
				break;
            }
        }

        void Reset()
        {
            me->RestoreFaction();
        }

        void JustDied(Unit* /*killer*/)
        {
            if (Player* player = GetPlayerForEscort())
            {
                if (player->GetTeam() == ALLIANCE)
                    player->FailQuest(QUEST_EFTW_A);
                else if (player->GetTeam() == HORDE)
                    player->FailQuest(QUEST_EFTW_H);
            }
        }
    };

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_EFTW_H || quest->GetQuestId() == QUEST_EFTW_A)
        {
            CAST_AI(npc_escortAI, (creature->AI()))->Start(true, false, player->GetGUID());
            creature->SetCurrentFaction(113);
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_isla_starmaneAI(creature);
    }
};

#define GOSSIP_S_DARKSCREECHER_AKKARAI "Summon Darkscreecher Akkarai"
#define GOSSIP_S_KARROG                "Summon Karrog"
#define GOSSIP_S_GEZZARAK_THE_HUNTRESS "Summon Gezzarak the Huntress"
#define GOSSIP_S_VAKKIZ_THE_WINDRAGER  "Summon Vakkiz the Windrager"

class go_skull_pile : public GameObjectScript
{
public:
    go_skull_pile() : GameObjectScript("go_skull_pile") { }

	bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(11885) == QUEST_STATUS_INCOMPLETE || player->GetQuestRewardStatus(11885))
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_S_DARKSCREECHER_AKKARAI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_S_KARROG, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_S_GEZZARAK_THE_HUNTRESS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_S_VAKKIZ_THE_WINDRAGER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
        }

        player->SEND_GOSSIP_MENU(go->GetGOInfo()->questgiver.gossipID, go->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, GameObject* go, uint32 sender, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (sender)
        {
            case GOSSIP_SENDER_MAIN:
				SendActionMenu(player, go, action);
				break;
        }
        return true;
    }

    void SendActionMenu(Player* player, GameObject* /*go*/, uint32 action)
    {
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                  player->CastSpell(player, 40642, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                  player->CastSpell(player, 40640, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                  player->CastSpell(player, 40632, false);
                break;
            case GOSSIP_ACTION_INFO_DEF + 4:
                  player->CastSpell(player, 40644, false);
                break;
        }
    }
};

enum Slim
{
    FACTION_CONSORTIUM  = 933,
};

class npc_slim : public CreatureScript
{
public:
    npc_slim() : CreatureScript("npc_slim") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsVendor() && player->GetReputationRank(FACTION_CONSORTIUM) >= REP_FRIENDLY)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_VENDOR_BROWSE_YOUR_GOODS), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
            player->SEND_GOSSIP_MENU(9896, creature->GetGUID());
        }
        else
            player->SEND_GOSSIP_MENU(9895, creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_TRADE)
            player->GetSession()->SendListInventory(creature->GetGUID());

        return true;
    }
};

enum Akuno
{
    QUEST_ESCAPING_THE_TOMB = 10887,
    NPC_CABAL_SKRIMISHER    = 21661,
};

class npc_akuno : public CreatureScript
{
public:
    npc_akuno() : CreatureScript("npc_akuno") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_ESCAPING_THE_TOMB)
        {
            if (npc_akunoAI* EscortAI = CAST_AI(npc_akuno::npc_akunoAI, creature->AI()))
                EscortAI->Start(false, false, player->GetGUID());

            if (player->GetTeamId() == TEAM_ALLIANCE)
                creature->SetCurrentFaction(FACTION_ESCORT_A_NEUTRAL_PASSIVE);
            else
                creature->SetCurrentFaction(FACTION_ESCORT_H_NEUTRAL_PASSIVE);
        }
        return true;
    }

    struct npc_akunoAI : public npc_escortAI
    {
        npc_akunoAI(Creature* creature) : npc_escortAI(creature) {}

        void WaypointReached(uint32 i)
        {
            Player* player = GetPlayerForEscort();

            if (!player)
                return;

            switch (i)
            {
                case 3:
                    me->SummonCreature(NPC_CABAL_SKRIMISHER, -2795.99f, 5420.33f, -34.53f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                    me->SummonCreature(NPC_CABAL_SKRIMISHER, -2793.55f, 5412.79f, -34.53f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                    break;
                case 11:
                    if (player && player->GetTypeId() == TYPE_ID_PLAYER)
                        player->GroupEventHappens(QUEST_ESCAPING_THE_TOMB, me);
                    break;
            }
        }

        void JustSummoned(Creature* summon)
        {
            summon->AI()->AttackStart(me);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_akunoAI(creature);
    }
};

enum TerokkData
{
    SPELL_CLEAVE              = 15284,
    SPELL_DIVINE_SHIELD       = 40733,
    SPELL_FRENZY              = 28747,
    SPELL_SHADOW_BOLT_VOLLEY  = 40721,
    SPELL_WILL_OF_THE_ARAKKOA = 40722,
    SPELL_VISUAL_MARKER       = 40656,
    NPC_MARKER_TRIGGER        = 21028,
    SAY_SUMMONED              = 0,
    SAY_CHOSEN                = 1,
    SAY_IMMUNE                = 2,
};

class npc_terokk : public CreatureScript
{
public:
    npc_terokk() : CreatureScript("npc_terokk") {}

    struct npc_terokkAI : public QuantumBasicAI
    {
        npc_terokkAI(Creature* creature) : QuantumBasicAI(creature) {}

        bool inCombat;
        bool Chosen;
        bool isImmune;

        uint32 CleaveTimer;
        uint32 VolleyTimer;
        uint32 ShieldTimer;
        uint32 MarkerTimer;

        void Reset()
        {
			inCombat = false;
            Chosen = false;
            isImmune = false;

            CleaveTimer = 5000;
            VolleyTimer = 3000;
            ShieldTimer = 999999;
            MarkerTimer = 15000;
        }

        void EnterToBattle (Unit* /*who*/)
        {
            if (!inCombat)
            {
                Talk(SAY_SUMMONED);
                inCombat = true;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = urand(9000, 15000);
            }
			else CleaveTimer -= diff;

            if (VolleyTimer <= diff)
            {
                DoCastAOE(SPELL_SHADOW_BOLT_VOLLEY);
                VolleyTimer = urand(12000, 20000);
            }
			else VolleyTimer -= diff;

            if (MarkerTimer <= diff)
            {
                switch (urand(1, 4))
                {
                    case 1:
                        if (Creature* trigger = me->SummonCreature(NPC_MARKER_TRIGGER, me->GetPositionX()+30, me->GetPositionY(), me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 30000))
						{
							trigger->AddAura(SPELL_VISUAL_MARKER, trigger);
						}
						break;
                    case 2:
                        if (Creature* trigger = me->SummonCreature(NPC_MARKER_TRIGGER, me->GetPositionX(), me->GetPositionY()+30, me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 30000))
						{
                            trigger->AddAura(SPELL_VISUAL_MARKER, trigger);
						}
						break;
                    case 3:
                        if (Creature* trigger = me->SummonCreature(NPC_MARKER_TRIGGER, me->GetPositionX()-30, me->GetPositionY(), me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 30000))
						{
							trigger->AddAura(SPELL_VISUAL_MARKER, trigger);
						}
						break;
                    case 4:
                        if (Creature* trigger = me->SummonCreature(NPC_MARKER_TRIGGER, me->GetPositionX(), me->GetPositionY()-30, me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 30000))
						{
                            trigger->AddAura(SPELL_VISUAL_MARKER, trigger);
						}
						break;
                }
                MarkerTimer = urand(25000, 30000);
            }
			else MarkerTimer -= diff;

            if (HealthBelowPct(HEALTH_PERCENT_50) && !Chosen)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
                    if (target && target->IsAlive())
                    {
                        Talk(SAY_CHOSEN);
                        DoCast(target, SPELL_WILL_OF_THE_ARAKKOA);
                        Chosen = true;
                    }
                }
            }

            if (HealthBelowPct(HEALTH_PERCENT_25) && !isImmune)
            {
                if (ShieldTimer <= diff)
                {
                    Talk(SAY_IMMUNE);
                    DoCast(me, SPELL_DIVINE_SHIELD);
                    ShieldTimer = 60000;
                    isImmune = true;
                }
				else ShieldTimer -= diff;
            }

            if (me->FindCreatureWithDistance(NPC_MARKER_TRIGGER, 5.0f, true) && isImmune)
            {
                me->RemoveAura(SPELL_DIVINE_SHIELD);
                DoCast(me, SPELL_FRENZY);
                isImmune = false;
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_terokkAI(creature);
    }
};

enum BombTarget
{
	SPELL_EXPLOSION_VISUAL   = 40162,
	SPELL_SKETTIS_BOMB       = 39844,
	SPELL_OGRILAR_BOMB       = 40160,
	SPELL_BOMBING_MASK       = 40196,

	NPC_BOMBING_TARGET_BUNNY = 23118,
	GO_MONSTROUS_KALIRI_EGG  = 185549,
	GO_FELL_CANNONBALL_STACK = 185861,
};

class npc_bomb_target : public CreatureScript
{
public:
    npc_bomb_target() : CreatureScript("npc_bomb_target") { }

    struct npc_bomb_targetAI : public QuantumBasicAI
    {
        npc_bomb_targetAI(Creature* creature) : QuantumBasicAI(creature) { }

        void Reset()
        {
            switch (me->GetEntry())
            {
                case NPC_BOMBING_TARGET_BUNNY:
					DoCast(me, SPELL_BOMBING_MASK, true);
					break;
            }
        }

        void EnterToBattle(Unit* /*who*/){}

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (caster && caster->GetTypeId() == TYPE_ID_PLAYER)
            {
                GameObject* go;
                switch (spell->Id)
                {
                case SPELL_SKETTIS_BOMB:
                    go = GetClosestGameObjectWithEntry(me, GO_MONSTROUS_KALIRI_EGG, 5);
                    if (go)
                    {
                        go->SetLootState(GO_NOT_READY);
                        go->SetRespawnTime(120);
                        go->SendObjectDeSpawnAnim(caster->GetGUID());
                    }
                    me->DealDamage(me, me->GetHealth());
                    me->RemoveCorpse();
                    me->SetRespawnTime(120);
                    break;
                case SPELL_OGRILAR_BOMB:
                    go = GetClosestGameObjectWithEntry(me, GO_FELL_CANNONBALL_STACK, 5);
                    if (go)
                    {
						go->SetLootState(GO_NOT_READY);
                        go->SetRespawnTime(120);
                        go->SendObjectDeSpawnAnim(caster->GetGUID());
                    }
                    me->CastSpell(me, SPELL_EXPLOSION_VISUAL, true);
                    me->DealDamage(me, me->GetHealth());
                    me->RemoveCorpse();
                    me->SetRespawnTime(120);
                    break;
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bomb_targetAI(creature);
    }
};

enum TheFinalCode
{
	SPELL_MANA_BOMB_LIGHTNING   = 37843,
    SPELL_MANA_BOMB_EXPL        = 35513,
	SPELL_EXPLOSION_KILL        = 35958,
	SPELL_MANA_BOMB_VISUAL      = 63660,
    NPC_MANA_BOMB_EXPL_TRIGGER  = 20767,
    NPC_MANA_BOMB_KILL_TRIGGER  = 21039,
	ACTION_START_EVENT          = 1,
};

#define SAY_TIMER_1 "1..."
#define SAY_TIMER_2 "2..."
#define SAY_TIMER_3 "3..."
#define SAY_TIMER_4 "4..."
#define SAY_TIMER_5 "5..."

class npc_mana_bomb_exp_trigger : public CreatureScript
{
public:
	npc_mana_bomb_exp_trigger() : CreatureScript("npc_mana_bomb_exp_trigger") { }

	struct npc_mana_bomb_exp_triggerAI : public QuantumBasicAI
	{
		npc_mana_bomb_exp_triggerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_EXPLOSION_KILL, true);
		}

		bool IsActivated;
		uint32 EventTimer;
		uint32 EventCounter;

		void Reset()
		{
			IsActivated = false;
			EventTimer = 1000;
			EventCounter = 0;
		}

		void DoAction(const int32 action)
		{
			if (action == ACTION_START_EVENT)
			{
				if (IsActivated)
					return;

				IsActivated = true;
			}
		}

		void UpdateAI(const uint32 diff)
		{
			if (!IsActivated)
				return;

			if (EventTimer < diff)
			{
				EventTimer = 1000;

				if (EventCounter < 10)
					me->CastSpell(me, SPELL_MANA_BOMB_LIGHTNING, true);

				switch (EventCounter)
				{
				    case 5:
						me->MonsterSay(SAY_TIMER_1, LANG_UNIVERSAL, 0);
						break;
					case 6:
						me->MonsterSay(SAY_TIMER_2, LANG_UNIVERSAL, 0);
						break;
					case 7:
						me->MonsterSay(SAY_TIMER_3, LANG_UNIVERSAL, 0);
						break;
					case 8:
						me->MonsterSay(SAY_TIMER_4, LANG_UNIVERSAL, 0);
						break;
					case 9:
						me->MonsterSay(SAY_TIMER_5, LANG_UNIVERSAL, 0);
						break;
					case 10:
						me->CastSpell(me, SPELL_MANA_BOMB_EXPL, true);
						me->CastSpell(me, SPELL_MANA_BOMB_VISUAL, true);
						Reset();
						break;
				}

				++EventCounter;
			}
			else EventTimer -= diff;
		}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mana_bomb_exp_triggerAI(creature);
    }
};

class go_mana_bomb : public GameObjectScript
{
public:
	go_mana_bomb() : GameObjectScript("go_mana_bomb") { }

	bool OnGossipHello(Player* player, GameObject* go)
	{
		player->KilledMonsterCredit(NPC_MANA_BOMB_KILL_TRIGGER, player->GetGUID());

		if (Creature* mana = GetClosestCreatureWithEntry(go, NPC_MANA_BOMB_EXPL_TRIGGER, 10.0f))
		{
			if (mana)
				mana->AI()->DoAction(ACTION_START_EVENT);
		}
		return true;
    }
};

enum QuestByAnyMeansNecessary
{
	SPELL_E_FROST_SHOCK          = 12548,
	SPELL_E_LIGHTNING_SHIELD     = 12550,
	SPELL_E_REND                 = 11977,
	SPELL_E_DEMORALIZING_SHOUT   = 13730,

	SAY_EMPOOR_AGGRO             = -1300039,

	FACTION_E_HATED              = 16,
	FACTION_E_FRIENDLY           = 35,

	QUEST_BY_ANY_MEANS_NECESSARY = 9978,
	NPC_EMPOOR_BODYGUARD         = 18483,
};

#define GOSSIP_MENU_EMPOOR "Empoor, you're going to tell me what I want to know, or else!"

class npc_empoor : public CreatureScript
{
public:
    npc_empoor() : CreatureScript("npc_empoor") {}

	bool OnGossipHello(Player* player, Creature* creature)
    {
		if (creature->IsQuestGiver())
			player->PrepareQuestMenu(creature->GetGUID());

		if (player->GetQuestStatus(QUEST_BY_ANY_MEANS_NECESSARY) == QUEST_STATUS_INCOMPLETE)
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_MENU_EMPOOR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

		if (action == GOSSIP_ACTION_INFO_DEF)
        {
			creature->SetCurrentFaction(FACTION_E_HATED);
            player->CLOSE_GOSSIP_MENU();
        }
        return true;
    }

    struct npc_empoorAI : public QuantumBasicAI
    {
        npc_empoorAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 LightningShieldTimer;
		uint32 FrostShockTimer;

		SummonList Summons;

        void Reset()
        {
			LightningShieldTimer = 100;
			FrostShockTimer = 500;

			me->SummonCreature(NPC_EMPOOR_BODYGUARD, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN);
			me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			if (Creature* bodygard = me->FindCreatureWithDistance(NPC_EMPOOR_BODYGUARD, 50.0f, true))
				bodygard->SetCurrentFaction(FACTION_E_HATED);

			DoSendQuantumText(SAY_EMPOOR_AGGRO, me);
		}

		void DamageTaken(Unit* attacker, uint32 &amount)
        {
            if (amount >= me->GetHealth())
            {
				if (Player* player = attacker->ToPlayer())
				{
					amount = 0;

					me->RemoveAllAuras();
					me->AttackStop();
					me->SetCurrentFaction(FACTION_E_FRIENDLY);
					me->SetHealth(me->GetMaxHealth());
					player->CompleteQuest(QUEST_BY_ANY_MEANS_NECESSARY);
					me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
				}
            }
        }

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_EMPOOR_BODYGUARD)
			{
				Summons.Summon(summon);
				summon->GetMotionMaster()->MoveFollow(me, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
			}
		}

		void JustReachedHome()
		{
			Summons.DespawnAll();
			Reset();
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (LightningShieldTimer <= diff && me->IsInCombatActive())
			{
				DoCast(me, SPELL_E_LIGHTNING_SHIELD);
				LightningShieldTimer = 60000; // 2 minutes
			}
			else LightningShieldTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Check Ligtning Shield in Combat
			if (!me->HasAura(SPELL_E_LIGHTNING_SHIELD))
				DoCast(me, SPELL_E_LIGHTNING_SHIELD, true);

			if (FrostShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_E_FROST_SHOCK);
					FrostShockTimer = urand(3000, 4000);
				}
			}
			else FrostShockTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_empoorAI(creature);
    }
};

class npc_empoors_bodyguard : public CreatureScript
{
public:
	npc_empoors_bodyguard() : CreatureScript("npc_empoors_bodyguard") {}

    struct npc_empoors_bodyguardAI : public QuantumBasicAI
    {
        npc_empoors_bodyguardAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RendTimer;
		uint32 DemoralizingShoutTimer;

		void Reset()
        {
			RendTimer = 1000;
			DemoralizingShoutTimer = 2000;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void DamageTaken(Unit* attacker, uint32 &amount)
        {
            if (amount >= me->GetHealth())
            {
				if (Player* player = attacker->ToPlayer())
				{
					amount = 0;

					me->RemoveAllAuras();
					me->AttackStop();
					me->SetHealth(me->GetMaxHealth());
					me->SetCurrentFaction(FACTION_E_FRIENDLY);
				}
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RendTimer <= diff)
			{
				DoCastVictim(SPELL_E_REND);
				RendTimer = urand(3000, 4000);
			}
			else RendTimer -= diff;

			if (DemoralizingShoutTimer <= diff)
			{
				DoCastAOE(SPELL_E_DEMORALIZING_SHOUT);
				DemoralizingShoutTimer = urand(5000, 6000);
			}
			else DemoralizingShoutTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_empoors_bodyguardAI(creature);
    }
};

void AddSC_terokkar_forest()
{
    new npc_unkor_the_ruthless();
    new mob_netherweb_victim();
    new npc_floon();
    new npc_isla_starmane();
    new go_skull_pile();
    new npc_skywing();
    new npc_slim();
    new npc_akuno();
	new npc_terokk();
	new npc_bomb_target();
	new npc_mana_bomb_exp_trigger();
	new go_mana_bomb();
	new npc_empoor();
	new npc_empoors_bodyguard();
}