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

#define GOSSIP_CORPSE "Examine corpse in detail..."

enum BeatenCorpse
{
    QUEST_LOST_IN_BATTLE = 4921,
};

class npc_beaten_corpse : public CreatureScript
{
public:
    npc_beaten_corpse() : CreatureScript("npc_beaten_corpse") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_LOST_IN_BATTLE) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_LOST_IN_BATTLE) == QUEST_STATUS_COMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_CORPSE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(3557, creature->GetGUID());
        return true;
    }

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF +1)
        {
            player->SEND_GOSSIP_MENU(3558, creature->GetGUID());
            player->TalkedToCreature(creature->GetEntry(), creature->GetGUID());
        }
        return true;
    }
};

enum Gilthares
{
	SAY_GIL_START               = 0,
	SAY_GIL_AT_LAST             = 1,
	SAY_GIL_PROCEED             = 2,
	SAY_GIL_FREEBOOTERS         = 3,
	SAY_GIL_AGGRO               = 4,
	SAY_GIL_ALMOST              = 5,
	SAY_GIL_SWEET               = 6,
	SAY_GIL_FREED               = 7,

    QUEST_FREE_FROM_HOLD        = 898,
    AREA_MERCHANT_COAST         = 391,
    FACTION_ESCORTEE            = 232,
};

class npc_gilthares : public CreatureScript
{
public:
    npc_gilthares() : CreatureScript("npc_gilthares") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_FREE_FROM_HOLD)
        {
            creature->SetCurrentFaction(FACTION_ESCORTEE);
            creature->SetStandState(UNIT_STAND_STATE_STAND);
			creature->AI()->Talk(SAY_GIL_START, player->GetGUID());

            if (npc_giltharesAI* EscortAI = CAST_AI(npc_gilthares::npc_giltharesAI, creature->AI()))
                EscortAI->Start(false, false, player->GetGUID(), quest);
        }
        return true;
    }

    struct npc_giltharesAI : public npc_escortAI
    {
        npc_giltharesAI(Creature* creature) : npc_escortAI(creature) { }

        void Reset(){}

        void WaypointReached(uint32 PointId)
        {
            Player* player = GetPlayerForEscort();

            if (!player)
                return;

            switch (PointId)
            {
                case 16:
					Talk(SAY_GIL_AT_LAST, player->GetGUID());
                    break;
                case 17:
                    Talk(SAY_GIL_PROCEED, player->GetGUID());
                    break;
                case 18:
                    Talk(SAY_GIL_FREEBOOTERS, player->GetGUID());
                    break;
                case 37:
                    Talk(SAY_GIL_ALMOST,player->GetGUID());
                    break;
                case 47:
                    Talk(SAY_GIL_SWEET, player->GetGUID());
                    break;
                case 53:
                    Talk(SAY_GIL_FREED, player->GetGUID());
                    player->GroupEventHappens(QUEST_FREE_FROM_HOLD, me);
                    break;
            }
        }

        void EnterToBattle(Unit* who)
        {
            if (rand()%4)
                return;

            if (who->GetTypeId() != TYPE_ID_PLAYER && me->GetAreaId() == AREA_MERCHANT_COAST)
                Talk(SAY_GIL_AGGRO, who->GetGUID());
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_giltharesAI(creature);
	}
};

#define GOSSIP_SPUTTERVALVE "Can you tell me about this shard?"

class npc_sputtervalve : public CreatureScript
{
public:
    npc_sputtervalve() : CreatureScript("npc_sputtervalve") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(6981) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SPUTTERVALVE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF)
        {
            player->SEND_GOSSIP_MENU(2013, creature->GetGUID());
            player->AreaExploredOrEventHappens(6981);
        }
        return true;
    }
};

enum Fizzule
{
    FACTION_FRIENDLY_F = 35,
    SPELL_FLARE        = 10113,
    SPELL_FOLLY        = 10137,
};

class npc_taskmaster_fizzule : public CreatureScript
{
public:
    npc_taskmaster_fizzule() : CreatureScript("npc_taskmaster_fizzule") { }

    struct npc_taskmaster_fizzuleAI : public QuantumBasicAI
    {
        npc_taskmaster_fizzuleAI(Creature* creature) : QuantumBasicAI(creature)
        {
            FactionNorm = creature->GetFaction();
        }

        uint32 FactionNorm;
        bool IsFriend;
        uint32 ResetTimer;
        uint8 FlareCount;

        void Reset()
        {
            IsFriend = false;
            ResetTimer = 120000;
            FlareCount = 0;
            me->SetCurrentFaction(FactionNorm);
        }

        void DoFriend()
        {
            me->RemoveAllAuras();
            me->DeleteThreatList();
            me->CombatStop(true);

            me->StopMoving();
            me->GetMotionMaster()->MoveIdle();

            me->SetCurrentFaction(FACTION_FRIENDLY_F);
            me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
        }

		void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_FLARE || spell->Id == SPELL_FOLLY)
            {
                ++FlareCount;

                if (FlareCount >= 2)
                    IsFriend = true;
            }
        }

        void EnterToBattle(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (IsFriend)
            {
                if (ResetTimer <= diff)
                {
                    EnterEvadeMode();
                    return;
                }
				else ResetTimer -= diff;
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }

        void ReceiveEmote(Player* /*player*/, uint32 emote)
        {
            if (emote == TEXT_EMOTE_SALUTE)
            {
                if (FlareCount >= 2)
                {
                    if (me->GetFaction() == FACTION_FRIENDLY_F)
                        return;

                    DoFriend();
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_taskmaster_fizzuleAI(creature);
    }
};

enum TwiggyFlathead
{
    NPC_BIG_WILL              = 6238,
    NPC_AFFRAY_CHALLENGER     = 6240,

	SAY_BIG_WILL_READY          = 0,
	SAY_TWIGGY_FLATHEAD_BEGIN   = 0,
	SAY_TWIGGY_FLATHEAD_FRAY    = 1,
	SAY_TWIGGY_FLATHEAD_DOWN    = 2,
	SAY_TWIGGY_FLATHEAD_OVER    = 3,
};

Position const AffrayChallengerLoc[6] =
{
    {-1683.0f, -4326.0f, 2.79f, 0.0f},
    {-1682.0f, -4329.0f, 2.79f, 0.0f},
    {-1683.0f, -4330.0f, 2.79f, 0.0f},
    {-1680.0f, -4334.0f, 2.79f, 1.49f},
    {-1674.0f, -4326.0f, 2.79f, 3.49f},
    {-1677.0f, -4334.0f, 2.79f, 1.66f}
};

class npc_twiggy_flathead : public CreatureScript
{
public:
    npc_twiggy_flathead() : CreatureScript("npc_twiggy_flathead") { }

    struct npc_twiggy_flatheadAI : public QuantumBasicAI
    {
        npc_twiggy_flatheadAI(Creature* creature) : QuantumBasicAI(creature) {}

        bool EventInProgress;
        bool EventGrate;
        bool EventBigWill;
        bool ChallengerDown[6];
        uint8 Wave;
        uint32 WaveTimer;
        uint32 ChallengerChecker;
        uint64 PlayerGUID;
        uint64 AffrayChallenger[6];
        uint64 BigWill;

        void Reset()
        {
            EventInProgress = false;
            EventGrate = false;
            EventBigWill = false;
            WaveTimer = 600000;
            ChallengerChecker = 0;
            Wave = 0;
            PlayerGUID = 0;

            for (uint8 i = 0; i < 6; ++i)
            {
                AffrayChallenger[i] = 0;
                ChallengerDown[i] = false;
            }
            BigWill = 0;
        }

        void EnterToBattle(Unit* /*who*/) { }

        void MoveInLineOfSight(Unit* who)
        {
            if (!who || (!who->IsAlive()))
                return;

            if (me->IsWithinDistInMap(who, 10.0f) && (who->GetTypeId() == TYPE_ID_PLAYER) && CAST_PLR(who)->GetQuestStatus(1719) == QUEST_STATUS_INCOMPLETE && !EventInProgress)
            {
                PlayerGUID = who->GetGUID();
                EventInProgress = true;
            }
        }

        void KilledUnit(Unit* /*victim*/) { }

        void UpdateAI(const uint32 diff)
        {
            if (EventInProgress)
			{
                Player* warrior = NULL;

                if (PlayerGUID)
                    warrior = Unit::GetPlayer(*me, PlayerGUID);

                if (!warrior)
                    return;

                if (!warrior->IsAlive() && warrior->GetQuestStatus(1719) == QUEST_STATUS_INCOMPLETE)
				{
                    Talk(SAY_TWIGGY_FLATHEAD_DOWN);
                    warrior->FailQuest(1719);

                    for (uint8 i = 0; i < 6; ++i) // unsummon challengers
                    {
                        if (AffrayChallenger[i])
                        {
                            Creature* creature = Unit::GetCreature(*me, AffrayChallenger[i]);
                            if (creature && creature->IsAlive())
                                creature->DisappearAndDie();
                        }
                    }

                    if (BigWill) // unsummon bigWill
                    {
                        Creature* creature = Unit::GetCreature(*me, BigWill);
                        if (creature && creature->IsAlive())
                            creature->DisappearAndDie();
                    }
                    Reset();
                }

                if (!EventGrate && EventInProgress)
                {
                    float x, y, z;
                    warrior->GetPosition(x, y, z);

                    if (x >= -1684 && x <= -1674 && y >= -4334 && y <= -4324)
					{
                        warrior->AreaExploredOrEventHappens(1719);
						Talk(SAY_TWIGGY_FLATHEAD_BEGIN, warrior->GetGUID());

                        for (uint8 i = 0; i < 6; ++i)
                        {
                            Creature* creature = me->SummonCreature(NPC_AFFRAY_CHALLENGER, AffrayChallengerLoc[i], TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000);
                            if (!creature)
							{
                                continue;
							}
                            creature->SetCurrentFaction(35);
                            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            creature->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                            AffrayChallenger[i] = creature->GetGUID();
                        }
                        WaveTimer = 5000;
                        ChallengerChecker = 1000;
                        EventGrate = true;
                    }
                }
                else if (EventInProgress)
                {
                    if (ChallengerChecker <= diff)
                    {
                        for (uint8 i = 0; i < 6; ++i)
                        {
                            if (AffrayChallenger[i])
                            {
                                Creature* creature = Unit::GetCreature(*me, AffrayChallenger[i]);
                                if ((!creature || (!creature->IsAlive())) && !ChallengerDown[i])
                                {
									Talk(SAY_TWIGGY_FLATHEAD_DOWN);
                                    ChallengerDown[i] = true;
                                }
                            }
                        }
                        ChallengerChecker = 1000;
                    }
					else ChallengerChecker -= diff;

                    if (WaveTimer <= diff)
                    {
                        if (Wave < 6 && AffrayChallenger[Wave] && !EventBigWill)
                        {
                            Talk(SAY_TWIGGY_FLATHEAD_FRAY);
                            Creature* creature = Unit::GetCreature(*me, AffrayChallenger[Wave]);
                            if (creature && (creature->IsAlive()))
                            {
                                creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                creature->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                                creature->SetCurrentFaction(16);
                                creature->AI()->AttackStart(warrior);
                                ++Wave;
                                WaveTimer = 20000;
                            }
                        }
                        else if (Wave >= 6 && !EventBigWill)
						{
                            if (Creature* creature = me->SummonCreature(NPC_BIG_WILL, -1722, -4341, 6.12f, 6.26f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 480000))
                            {
                                BigWill = creature->GetGUID();
                                //creature->GetMotionMaster()->MovePoint(0, -1693, -4343, 4.32f);
                                //creature->GetMotionMaster()->MovePoint(1, -1684, -4333, 2.78f);
                                creature->GetMotionMaster()->MovePoint(2, -1682, -4329, 2.79f);
                                creature->HandleEmoteCommand(EMOTE_STATE_READY_UNARMED);
                                EventBigWill = true;
                                WaveTimer = 1000;
                            }
                        }
                        else if (Wave >= 6 && EventBigWill && BigWill)
                        {
                            Creature* creature = Unit::GetCreature(*me, BigWill);
                            if (!creature || !creature->IsAlive())
                            {
                                Talk(SAY_TWIGGY_FLATHEAD_OVER);
                                Reset();
                            }
							else if (creature)
							{
								creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
								creature->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
								creature->SetCurrentFaction(16);
								creature->AI()->AttackStart(warrior);
							}
                        }
                    }
					else WaveTimer -= diff;
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_twiggy_flatheadAI(creature);
    }
};

enum Wizzlecrank
{
	SAY_MERCENARY       = 0,
	SAY_START           = 0,
	SAY_STARTUP1        = 1,
	SAY_STARTUP2        = 2,
	SAY_PROGRESS_1      = 3,
	SAY_PROGRESS_2      = 4,
	SAY_PROGRESS_3      = 5,
	SAY_END             = 6,

    QUEST_ESCAPE        = 863,
    FACTION_RATCHET     = 637,
    NPC_PILOT_WIZZ      = 3451,
    NPC_MERCENARY       = 3282,
};

class npc_wizzlecrank_shredder : public CreatureScript
{
public:
    npc_wizzlecrank_shredder() : CreatureScript("npc_wizzlecrank_shredder") { }

	bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_ESCAPE)
        {
            creature->SetCurrentFaction(FACTION_RATCHET);
			creature->AI()->Talk(SAY_START);

            if (npc_escortAI* EscortAI = CAST_AI(npc_wizzlecrank_shredder::npc_wizzlecrank_shredderAI, creature->AI()))
                EscortAI->Start(true, false, player->GetGUID());
        }
        return true;
    }

    struct npc_wizzlecrank_shredderAI : public npc_escortAI
    {
        npc_wizzlecrank_shredderAI(Creature* creature) : npc_escortAI(creature)
        {
            IsPostEvent = false;
            PostEventTimer = 1000;
            PostEventCount = 0;
        }

        bool IsPostEvent;
        uint32 PostEventTimer;
        uint32 PostEventCount;

        void Reset()
        {
            if (!HasEscortState(STATE_ESCORT_ESCORTING))
            {
                if (me->getStandState() == UNIT_STAND_STATE_DEAD)
                     me->SetStandState(UNIT_STAND_STATE_STAND);

                IsPostEvent = false;
                PostEventTimer = 1000;
                PostEventCount = 0;
            }
        }

        void WaypointReached(uint32 waypointId)
        {
            switch (waypointId)
            {
                case 0:
                    Talk(SAY_STARTUP1);
                    break;
                case 9:
                    SetRun(false);
                    break;
                case 17:
                    if (Creature* creature = me->SummonCreature(NPC_MERCENARY, 1128.489f, -3037.611f, 92.701f, 1.472f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000))
                    {
                        creature->AI()->Talk(SAY_MERCENARY);
                        me->SummonCreature(NPC_MERCENARY, 1160.172f, -2980.168f, 97.313f, 3.690f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);
                    }
                    break;
                case 24:
                    IsPostEvent = true;
                    break;
            }
        }

        void WaypointStart(uint32 PointId)
        {
            Player* player = GetPlayerForEscort();

            if (!player)
                return;

            switch (PointId)
            {
                case 9:
                    Talk(SAY_STARTUP2, player->GetGUID());
                    break;
                case 18:
					Talk(SAY_PROGRESS_1, player->GetGUID());
                    SetRun();
                    break;
            }
        }

        void JustSummoned(Creature* summoned)
        {
            if (summoned->GetEntry() == NPC_PILOT_WIZZ)
                me->SetStandState(UNIT_STAND_STATE_DEAD);

            if (summoned->GetEntry() == NPC_MERCENARY)
                summoned->AI()->AttackStart(me);
        }

        void UpdateEscortAI(const uint32 Diff)
        {
            if (!UpdateVictim())
            {
                if (IsPostEvent)
                {
                    if (PostEventTimer <= Diff)
                    {
                        switch (PostEventCount)
                        {
                            case 0:
                                Talk(SAY_PROGRESS_2);
                                break;
                            case 1:
								Talk(SAY_PROGRESS_3);
                                break;
                            case 2:
								Talk(SAY_END);
                                break;
                            case 3:
                                if (Player* player = GetPlayerForEscort())
                                {
                                    player->GroupEventHappens(QUEST_ESCAPE, me);
                                    me->SummonCreature(NPC_PILOT_WIZZ, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 180000);
                                }
                                break;
                        }

                        ++PostEventCount;
                        PostEventTimer = 5000;
                    }
                    else PostEventTimer -= Diff;
                }
                return;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wizzlecrank_shredderAI(creature);
    }
};

enum DruidCuredQuests
{
	SPELL_SICKLY_CRITTER_AURA = 19502,
	SPELL_APPLY_SAVE          = 19512,

	NPC_CURED_GAZELLE         = 12297,
	NPC_CURED_DEER            = 12299,
};

class npc_sickly_gazelle : public CreatureScript
{
public:
    npc_sickly_gazelle() : CreatureScript("npc_sickly_gazelle") {}

    struct npc_sickly_gazelleAI : public QuantumBasicAI
    {
        npc_sickly_gazelleAI(Creature* creature) : QuantumBasicAI(creature) {}

		void Reset()
		{
			DoCast(me, SPELL_SICKLY_CRITTER_AURA);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
        {
			if (spell->Id == SPELL_APPLY_SAVE)
			{
				me->UpdateEntry(NPC_CURED_GAZELLE);
				me->RemoveAurasDueToSpell(SPELL_SICKLY_CRITTER_AURA);
				me->DespawnAfterAction(5*IN_MILLISECONDS);

				if (Player* player = caster->ToPlayer())
					caster->ToPlayer()->KilledMonsterCredit(NPC_CURED_GAZELLE, 0);
			}
		}

		void UpdateAI(const uint32 /*diff*/)
		{
			if (!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sickly_gazelleAI(creature);
    }
};

class npc_sickly_deer : public CreatureScript
{
public:
    npc_sickly_deer() : CreatureScript("npc_sickly_deer") {}

    struct npc_sickly_deerAI : public QuantumBasicAI
    {
        npc_sickly_deerAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_SICKLY_CRITTER_AURA);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_APPLY_SAVE)
			{
				me->UpdateEntry(NPC_CURED_DEER);
				me->RemoveAurasDueToSpell(SPELL_SICKLY_CRITTER_AURA);
				me->DespawnAfterAction(5*IN_MILLISECONDS);

				if (Player* player = caster->ToPlayer())
					caster->ToPlayer()->KilledMonsterCredit(NPC_CURED_DEER, 0);
			}
		}

		void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sickly_deerAI(creature);
    }
};

void AddSC_the_barrens()
{
    new npc_beaten_corpse();
    new npc_gilthares();
    new npc_sputtervalve();
    new npc_taskmaster_fizzule();
    new npc_twiggy_flathead();
    new npc_wizzlecrank_shredder();
	new npc_sickly_gazelle();
	new npc_sickly_deer();
}