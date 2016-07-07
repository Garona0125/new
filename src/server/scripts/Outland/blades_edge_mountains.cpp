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

bool obelisk_one, obelisk_two, obelisk_three, obelisk_four, obelisk_five;

enum LegionObelisk
{
	GO_LEGION_OBELISK_ONE   = 185193,
	GO_LEGION_OBELISK_TWO   = 185195,
	GO_LEGION_OBELISK_THREE = 185196,
	GO_LEGION_OBELISK_FOUR  = 185197,
	GO_LEGION_OBELISK_FIVE  = 185198,
};

enum Netherdrake
{
    SAY_NIHIL_1                 = -1000169, // signed for 5955
    SAY_NIHIL_2                 = -1000170, // signed for 5955
    SAY_NIHIL_3                 = -1000171, // signed for 5955
    SAY_NIHIL_4                 = -1000172, // signed for 20021, used by 20021, 21817, 21820, 21821, 21823
    SAY_NIHIL_INTERRUPT         = -1000173, // signed for 20021, used by 20021, 21817, 21820, 21821, 21823

    ENTRY_WHELP                 = 20021,
    ENTRY_PROTO                 = 21821,
    ENTRY_ADOLE                 = 21817,
    ENTRY_MATUR                 = 21820,
    ENTRY_NIHIL                 = 21823,

    SPELL_T_PHASE_MODULATOR     = 37573,

    SPELL_ARCANE_BLAST          = 38881,
    SPELL_MANA_BURN             = 38884,
    SPELL_INTANGIBLE_PRESENCE   = 36513,
};

class mobs_nether_drake : public CreatureScript
{
public:
    mobs_nether_drake() : CreatureScript("mobs_nether_drake") { }

    struct mobs_nether_drakeAI : public QuantumBasicAI
    {
        mobs_nether_drakeAI(Creature* creature) : QuantumBasicAI(creature) {}

        bool IsNihil;
        uint32 NihilSpeechTimer;
        uint32 NihilSpeechPhase;

        uint32 ArcaneBlastTimer;
        uint32 ManaBurnTimer;
        uint32 IntangiblePresenceTimer;

        void Reset()
        {
            IsNihil = false;
            NihilSpeechTimer = 3000;
            NihilSpeechPhase = 0;

            ArcaneBlastTimer = 7500;
            ManaBurnTimer = 10000;
            IntangiblePresenceTimer = 15000;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void MoveInLineOfSight(Unit* who)
        {
            if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                return;

            QuantumBasicAI::MoveInLineOfSight(who);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == 0)
            {
                me->setDeathState(JUST_DIED);
                me->RemoveCorpse();
                me->SetHealth(0);
            }
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_T_PHASE_MODULATOR && caster->GetTypeId() == TYPE_ID_PLAYER)
            {
                const uint32 entry_list[4] = {ENTRY_PROTO, ENTRY_ADOLE, ENTRY_MATUR, ENTRY_NIHIL};
                int cid = rand()%(4-1);

                if (entry_list[cid] == me->GetEntry())
                    ++cid;

                //we are nihil, so say before transform
                if (me->GetEntry() == ENTRY_NIHIL)
                {
                    DoSendQuantumText(SAY_NIHIL_INTERRUPT, me);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    IsNihil = false;
                }

                if (me->UpdateEntry(entry_list[cid]))
                {
                    if (entry_list[cid] == ENTRY_NIHIL)
                    {
                        EnterEvadeMode();
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        IsNihil = true;
                    }
					else AttackStart(caster);
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (IsNihil)
            {
                if (NihilSpeechTimer <= diff)
                {
                    switch (NihilSpeechPhase)
                    {
                        case 0:
                            DoSendQuantumText(SAY_NIHIL_1, me);
                            ++NihilSpeechPhase;
                            break;
                        case 1:
                            DoSendQuantumText(SAY_NIHIL_2, me);
                            ++NihilSpeechPhase;
                            break;
                        case 2:
                            DoSendQuantumText(SAY_NIHIL_3, me);
                            ++NihilSpeechPhase;
                            break;
                        case 3:
                            DoSendQuantumText(SAY_NIHIL_4, me);
                            ++NihilSpeechPhase;
                            break;
                        case 4:
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            //take off to location above
                            me->GetMotionMaster()->MovePoint(0, me->GetPositionX()+50.0f, me->GetPositionY(), me->GetPositionZ()+50.0f);
                            ++NihilSpeechPhase;
                            break;
                    }
                    NihilSpeechTimer = 5000;
                }
				else NihilSpeechTimer -=diff;

                //anything below here is not interesting for Nihil, so skip it
                return;
            }

            if (!UpdateVictim())
                return;

            if (IntangiblePresenceTimer <= diff)
            {
                DoCastVictim(SPELL_INTANGIBLE_PRESENCE);
                IntangiblePresenceTimer = 15000+rand()%15000;
            }
			else IntangiblePresenceTimer -= diff;

            if (ManaBurnTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_MANA_BURN);
						ManaBurnTimer = 8000+rand()%8000;
					}
				}
            }
			else ManaBurnTimer -= diff;

            if (ArcaneBlastTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ARCANE_BLAST);
					ArcaneBlastTimer = 2500+rand()%5000;
				}
            }
			else ArcaneBlastTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mobs_nether_drakeAI(creature);
    }
};

enum Daranelle
{
    SAY_SPELL_INFLUENCE       = -1000174,

    SPELL_LASHHAN_CHANNEL     = 36904,
	SPELL_DISPELLING_ANALYSIS = 37028,
};

class npc_daranelle : public CreatureScript
{
public:
    npc_daranelle() : CreatureScript("npc_daranelle") { }

    struct npc_daranelleAI : public QuantumBasicAI
    {
        npc_daranelleAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset() {}

        void EnterToBattle(Unit* /*who*/) {}

        void MoveInLineOfSight(Unit* who)
        {
            if (who->GetTypeId() == TYPE_ID_PLAYER)
            {
                if (who->HasAura(SPELL_LASHHAN_CHANNEL) && me->IsWithinDistInMap(who, 10.0f))
                {
                    DoSendQuantumText(SAY_SPELL_INFLUENCE, me, who);
                    DoCast(who, SPELL_DISPELLING_ANALYSIS, true);
                }
            }

            QuantumBasicAI::MoveInLineOfSight(who);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_daranelleAI(creature);
    }
};

#define GOSSIP_HELLO_ON "Overseer, I am here to negotiate on behalf of the Cenarion Expedition."

class npc_overseer_nuaar : public CreatureScript
{
public:
    npc_overseer_nuaar() : CreatureScript("npc_overseer_nuaar") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(10682) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(10532, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->SEND_GOSSIP_MENU(10533, creature->GetGUID());
            player->AreaExploredOrEventHappens(10682);
        }
        return true;
    }
};

#define GOSSIP_HELLO_STE  "Yes... yes, it's me."
#define GOSSIP_SELECT_STE "Yes elder. Tell me more of the book."

class npc_saikkal_the_elder : public CreatureScript
{
public:
    npc_saikkal_the_elder() : CreatureScript("npc_saikkal_the_elder") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(10980) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_STE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(10794, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SELECT_STE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(10795, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->TalkedToCreature(creature->GetEntry(), creature->GetGUID());
                player->SEND_GOSSIP_MENU(10796, creature->GetGUID());
                break;
        }
        return true;
    }
};

enum Obelisk
{
	QUEST_YOURE_FIRED = 10821,
	NPC_DOOMCRYER     = 19963,
};

class go_legion_obelisk : public GameObjectScript
{
public:
    go_legion_obelisk() : GameObjectScript("go_legion_obelisk") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(QUEST_YOURE_FIRED) == QUEST_STATUS_INCOMPLETE)
        {
            switch (go->GetEntry())
            {
                case GO_LEGION_OBELISK_ONE:
					obelisk_one = true;
					break;
                case GO_LEGION_OBELISK_TWO:
					obelisk_two = true;
					break;
                case GO_LEGION_OBELISK_THREE:
					obelisk_three = true;
					break;
                case GO_LEGION_OBELISK_FOUR:
					obelisk_four = true;
					break;
                case GO_LEGION_OBELISK_FIVE:
					obelisk_five = true;
					break;
            }

            if (obelisk_one == true && obelisk_two == true && obelisk_three == true && obelisk_four == true && obelisk_five == true)
            {
                go->SummonCreature(NPC_DOOMCRYER, 2943.40f, 4778.20f, 284.49f, 0.94f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 120000);

                obelisk_one = false;
                obelisk_two = false;
                obelisk_three = false;
                obelisk_four = false;
                obelisk_five = false;
            }
        }
        return true;
    }
};

enum Bloodmaul
{
    NPC_BLADESPIRE_BRUTE                = 19995,
    NPC_QUEST_CREDIT                    = 21241,
	SPELL_BLOODMAUL_INTOXICATION        = 35240,
	SPELL_CLEAVE                        = 15496,
    GO_KEG                              = 184315,
    QUEST_GETTING_THE_BLADESPIRE_TANKED = 10512,
    QUEST_BLADESPIRE_KEGGER             = 10545,
};

class npc_bloodmaul_brutebane : public CreatureScript
{
public:
    npc_bloodmaul_brutebane() : CreatureScript("npc_bloodmaul_brutebane") { }

    struct npc_bloodmaul_brutebaneAI : public QuantumBasicAI
    {
        npc_bloodmaul_brutebaneAI(Creature* creature) : QuantumBasicAI(creature)
        {
           if (Creature* ogre = me->FindCreatureWithDistance(NPC_BLADESPIRE_BRUTE, 50, true))
           {
               ogre->SetReactState(REACT_DEFENSIVE);
               ogre->GetMotionMaster()->MovePoint(1, me->GetPositionX()-1, me->GetPositionY()+1, me->GetPositionZ());
           }
        }

        uint64 OgreGUID;

        void Reset()
        {
            OgreGUID = 0;
        }

        void UpdateAI(const uint32 /*diff*/){}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodmaul_brutebaneAI(creature);
    }
};

class npc_bladespire_brute : public CreatureScript
{
public:
    npc_bladespire_brute() : CreatureScript("npc_bladespire_brute") { }

    struct npc_bladespire_bruteAI : public QuantumBasicAI
    {
        npc_bladespire_bruteAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 CleaveTimer;
		uint32 BloodmaulIntoxicationTimer;

        uint64 PlayerGUID;

        void Reset()
        {
            PlayerGUID = 0;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!who || (!who->IsAlive()))
				return;

            if (me->IsWithinDistInMap(who, 50.0f))
            {
                if (who->GetTypeId() == TYPE_ID_PLAYER)
				{
                    if (who->ToPlayer()->GetQuestStatus(QUEST_GETTING_THE_BLADESPIRE_TANKED) == QUEST_STATUS_INCOMPLETE || who->ToPlayer()->GetQuestStatus(QUEST_BLADESPIRE_KEGGER) == QUEST_STATUS_INCOMPLETE)
						PlayerGUID = who->GetGUID();
				}
            }
        }

        void MovementInform(uint32 /*type*/, uint32 id)
        {
            Player* player = Unit::GetPlayer(*me, PlayerGUID);

            if (id == 1)
            {
                if (GameObject* keg = me->FindGameobjectWithDistance(GO_KEG, 20.0f))
					keg->Delete();

                me->HandleEmoteCommand(EMOTE_ONESHOT_EAT);
                me->SetReactState(REACT_AGGRESSIVE);
                me->GetMotionMaster()->MoveTargetedHome();

                if (Creature* credit = me->FindCreatureWithDistance(NPC_QUEST_CREDIT, 50.0f, true))
                    player->KilledMonster(credit->GetCreatureTemplate(), credit->GetGUID());
            }
        }

        void UpdateAI(const uint32 diff)
        {
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(3000, 4000);
			}
			else CleaveTimer -= diff;

			if (BloodmaulIntoxicationTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BLOODMAUL_INTOXICATION);
					BloodmaulIntoxicationTimer = urand(5000, 6000);
				}
			}
			else BloodmaulIntoxicationTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bladespire_bruteAI(creature);
    }
};

enum Rays
{
	SPELL_WRANGLING_ROPE    = 40856,

	NPC_WRANGLED_AETHER_RAY = 23343,
};

class npc_wrangled_aether_ray : public CreatureScript
{
public:
    npc_wrangled_aether_ray() : CreatureScript("npc_wrangled_aether_ray") {}

    struct npc_wrangled_aether_rayAI : public QuantumBasicAI
    {
        npc_wrangled_aether_rayAI(Creature* creature) : QuantumBasicAI(creature)
		{
			owner = 0;
		}

        uint64 owner;

        void Reset(){}

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
            {
                Unit* ownerlink = me->GetUnit(*me, me->GetOwnerGUID());
                if (ownerlink)
                    me->GetMotionMaster()->MoveFollow(ownerlink, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wrangled_aether_rayAI(creature);
    }
};

#define EMOTE_READY "appears ready to be wrangled."

class npc_aether_ray : public CreatureScript
{
public:
    npc_aether_ray() : CreatureScript("npc_aether_ray") { }

    struct npc_aether_rayAI : public QuantumBasicAI
    {
        npc_aether_rayAI(Creature* creature) : QuantumBasicAI(creature) { }

        bool isready;
        uint64 wranglerGUID;

        void Reset()
        {
            isready = false;
            wranglerGUID = 0;
        }

        void JustSummoned(Creature* summon)
        {
            Player* wrangler = me->GetPlayer(*summon, wranglerGUID);
            if (wrangler)
            {
                //summon->CastSpell(wrangler, 40926,true);
                //summon->SetOwnerGUID(wrangler->GetGUID());
                wrangler->KilledMonsterCredit(summon->GetEntry(),summon->GetGUID());
                me->DealDamage(me, me->GetHealth());
                me->RemoveCorpse();
                //summon->CastSpell(wrangler, 40926, true);
                //wrangler->CastSpell(summon, 40926, true);
            }
        }

        void EnterToBattle(Unit* /*who*/){}

        void DamageTaken(Unit* doneBy, uint32 &damage)
        {
            if (!isready)
			{
                if (me->GetHealth() - damage > 0 && me->GetHealth() - damage < (me->GetMaxHealth() *0.3))
                {
                    //DoTextEmote(EMOTE_READY, NULL);
                    isready = true;
                }
			}
        }

        void SpellHit(Unit* target, const SpellInfo* spell)
        {
            if (isready)
            {
                if (target->GetTypeId() == TYPE_ID_PLAYER)
                {
                    if (spell->Id == SPELL_WRANGLING_ROPE)
                    {
                        //DoCast(target, 40917);
                        //DoCast(target, 40907);
                        wranglerGUID = target->GetGUID();
                        DoSpawnCreature(NPC_WRANGLED_AETHER_RAY, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN, 20000);
                    }
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_aether_rayAI(creature);
    }
};

enum TheThunderspike
{
    NPC_GOR_GRIMGUT    = 21319,

    QUEST_THUNDERSPIKE = 10526,
};

class go_thunderspike : public GameObjectScript
{
public:
	go_thunderspike() : GameObjectScript("go_thunderspike") { }

	bool OnGossipHello(Player* player, GameObject* go)
	{
		if (player->GetQuestStatus(QUEST_THUNDERSPIKE) == QUEST_STATUS_INCOMPLETE && !go->FindCreatureWithDistance(NPC_GOR_GRIMGUT, 25.0f, true))
			if (Creature* gorGrimgut = go->SummonCreature(NPC_GOR_GRIMGUT, -2413.4f, 6914.48f, 25.01f, 3.67f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 300000))
				gorGrimgut->AI()->AttackStart(player);

		return true;
	}
};

enum SimonGame
{
    NPC_SIMON_BUNNY                 = 22923,
    NPC_APEXIS_GUARDIAN             = 22275,

    GO_APEXIS_RELIC                 = 185890,
    GO_APEXIS_MONUMENT              = 185944,
    GO_AURA_BLUE                    = 185872,
    GO_AURA_GREEN                   = 185873,
    GO_AURA_RED                     = 185874,
    GO_AURA_YELLOW                  = 185875,

    GO_BLUE_CLUSTER_DISPLAY         = 7369,
    GO_GREEN_CLUSTER_DISPLAY        = 7371,
    GO_RED_CLUSTER_DISPLAY          = 7373,
    GO_YELLOW_CLUSTER_DISPLAY       = 7375,
    GO_BLUE_CLUSTER_DISPLAY_LARGE   = 7364,
    GO_GREEN_CLUSTER_DISPLAY_LARGE  = 7365,
    GO_RED_CLUSTER_DISPLAY_LARGE    = 7366,
    GO_YELLOW_CLUSTER_DISPLAY_LARGE = 7367,

    SPELL_PRE_GAME_BLUE             = 40176,
    SPELL_PRE_GAME_GREEN            = 40177,
    SPELL_PRE_GAME_RED              = 40178,
    SPELL_PRE_GAME_YELLOW           = 40179,
    SPELL_VISUAL_BLUE               = 40244,
    SPELL_VISUAL_GREEN              = 40245,
    SPELL_VISUAL_RED                = 40246,
    SPELL_VISUAL_YELLOW             = 40247,

    SOUND_BLUE                      = 11588,
    SOUND_GREEN                     = 11589,
    SOUND_RED                       = 11590,
    SOUND_YELLOW                    = 11591,
    SOUND_DISABLE_NODE              = 11758,

    SPELL_AUDIBLE_GAME_TICK         = 40391,
    SPELL_VISUAL_START_PLAYER_LEVEL = 40436,
    SPELL_VISUAL_START_AI_LEVEL     = 40387,

    SPELL_BAD_PRESS_TRIGGER         = 41241,
    SPELL_BAD_PRESS_DAMAGE          = 40065,
    SPELL_REWARD_BUFF_1             = 40310,
    SPELL_REWARD_BUFF_2             = 40311,
    SPELL_REWARD_BUFF_3             = 40312,
};

enum SimonEvents
{
    EVENT_SIMON_SETUP_PRE_GAME         = 1,
    EVENT_SIMON_PLAY_SEQUENCE          = 2,
    EVENT_SIMON_RESET_CLUSTERS         = 3,
    EVENT_SIMON_PERIODIC_PLAYER_CHECK  = 4,
    EVENT_SIMON_TOO_LONG_TIME          = 5,
    EVENT_SIMON_GAME_TICK              = 6,
    EVENT_SIMON_ROUND_FINISHED         = 7,

    ACTION_SIMON_CORRECT_FULL_SEQUENCE = 8,
    ACTION_SIMON_WRONG_SEQUENCE        = 9,
    ACTION_SIMON_ROUND_FINISHED        = 10,
};

enum SimonColors
{
    SIMON_BLUE          = 0,
    SIMON_RED           = 1,
    SIMON_GREEN         = 2,
    SIMON_YELLOW        = 3,
    SIMON_MAX_COLORS    = 4,
};

class npc_simon_bunny : public CreatureScript
{
    public:
        npc_simon_bunny() : CreatureScript("npc_simon_bunny") { }

        struct npc_simon_bunnyAI : public QuantumBasicAI
        {
            npc_simon_bunnyAI(Creature* creature) : QuantumBasicAI(creature) { }

            bool large;
            bool listening;
            uint8 gameLevel;
            uint8 fails;
            uint8 gameTicks;
            uint64 playerGUID;
            uint32 clusterIds[SIMON_MAX_COLORS];
            float zCoordCorrection;
            float searchDistance;
            EventMap events;
            std::list<uint8> colorSequence, playableSequence, playerSequence;

            void UpdateAI(const uint32 diff)
            {
                events.Update(diff);

                switch(events.ExecuteEvent())
                {
                    case EVENT_SIMON_PERIODIC_PLAYER_CHECK:
                        if (!CheckPlayer())
                            ResetNode();
                        else
                            events.ScheduleEvent(EVENT_SIMON_PERIODIC_PLAYER_CHECK, 2000);
                        break;
                    case EVENT_SIMON_SETUP_PRE_GAME:
                        SetUpPreGame();
                        events.CancelEvent(EVENT_SIMON_GAME_TICK);
                        events.ScheduleEvent(EVENT_SIMON_PLAY_SEQUENCE, 1000);
                        break;
                    case EVENT_SIMON_PLAY_SEQUENCE:
                        if (!playableSequence.empty())
                        {
                            PlayNextColor();
                            events.ScheduleEvent(EVENT_SIMON_PLAY_SEQUENCE, 1500);
                        }
                        else
                        {
                            listening = true;
                            DoCast(SPELL_VISUAL_START_PLAYER_LEVEL);
                            playerSequence.clear();
                            PrepareClusters();
                            gameTicks = 0;
                            events.ScheduleEvent(EVENT_SIMON_GAME_TICK, 3000);
                        }
                        break;
                    case EVENT_SIMON_GAME_TICK:
                        DoCast(SPELL_AUDIBLE_GAME_TICK);

                        if (gameTicks > gameLevel)
                            events.ScheduleEvent(EVENT_SIMON_TOO_LONG_TIME, 500);
                        else
                            events.ScheduleEvent(EVENT_SIMON_GAME_TICK, 3000);
                        gameTicks++;
                        break;
                    case EVENT_SIMON_RESET_CLUSTERS:
                        PrepareClusters(true);
                        break;
                    case EVENT_SIMON_TOO_LONG_TIME:
                        DoAction(ACTION_SIMON_WRONG_SEQUENCE);
                        break;
                    case EVENT_SIMON_ROUND_FINISHED:
                        DoAction(ACTION_SIMON_ROUND_FINISHED);
                        break;
                }
            }

            void DoAction(const int32 action)
            {
                switch (action)
                {
                    case ACTION_SIMON_ROUND_FINISHED:
                        listening = false;
                        DoCast(SPELL_VISUAL_START_AI_LEVEL);
                        GiveRewardForLevel(gameLevel);
                        events.CancelEventGroup(0);
                        if (gameLevel == 10)
                            ResetNode();
                        else
                            events.ScheduleEvent(EVENT_SIMON_SETUP_PRE_GAME, 1000);
                        break;
                    case ACTION_SIMON_CORRECT_FULL_SEQUENCE:
                        gameLevel++;
                        DoAction(ACTION_SIMON_ROUND_FINISHED);
                        break;
                    case ACTION_SIMON_WRONG_SEQUENCE:
                        GivePunishment();
                        DoAction(ACTION_SIMON_ROUND_FINISHED);
                        break;
                }
            }

            // Called by color clusters script (go_simon_cluster) and used for knowing the button pressed by player
            void SetData(uint32 type, uint32 /*data*/)
            {
                if (!listening)
                    return;

                uint8 pressedColor = SIMON_MAX_COLORS;

                if (type == clusterIds[SIMON_RED])
                    pressedColor = SIMON_RED;

                else if (type == clusterIds[SIMON_BLUE])
                    pressedColor = SIMON_BLUE;

                else if (type == clusterIds[SIMON_GREEN])
                    pressedColor = SIMON_GREEN;

                else if (type == clusterIds[SIMON_YELLOW])
                    pressedColor = SIMON_YELLOW;

                PlayColor(pressedColor);
                playerSequence.push_back(pressedColor);
                events.ScheduleEvent(EVENT_SIMON_RESET_CLUSTERS, 500);
                CheckPlayerSequence();
            }

            // Used for getting involved player guid. Parameter id is used for defining if is a large(Monument) or small(Relic) node
            void SetGUID(uint64 guid, int32 id)
            {
                me->SetCanFly(true);
                large = (bool)id;
                playerGUID = guid;
                StartGame();
            }

            void StartGame()
            {
                listening = false;
                gameLevel = 0;
                fails = 0;
                gameTicks = 0;
                zCoordCorrection = large ? 8.0f : 2.75f;
                searchDistance = large ? 13.0f : 5.0f;
                colorSequence.clear();
                playableSequence.clear();
                playerSequence.clear();
				me->SetObjectScale(large ? 2.0f : 1.0f);

                std::list<WorldObject*> ClusterList;
                Trinity::AllWorldObjectsInRange objects(me, searchDistance);
                Trinity::WorldObjectListSearcher<Trinity::AllWorldObjectsInRange> searcher(me, ClusterList, objects);
                me->VisitNearbyObject(searchDistance, searcher);

                for (std::list<WorldObject*>::const_iterator i = ClusterList.begin(); i != ClusterList.end(); ++i)
                {
                    if (GameObject* go = (*i)->ToGameObject())
                    {
                        // We are checking for displayid because all simon nodes have 4 clusters with different entries
                        if (large)
                        {
                            switch (go->GetGOInfo()->displayId)
                            {
                                case GO_BLUE_CLUSTER_DISPLAY_LARGE:
									clusterIds[SIMON_BLUE] = go->GetEntry();
									break;
                                case GO_RED_CLUSTER_DISPLAY_LARGE:
									clusterIds[SIMON_RED] = go->GetEntry();
									break;
                                case GO_GREEN_CLUSTER_DISPLAY_LARGE:
									clusterIds[SIMON_GREEN] = go->GetEntry();
									break;
                                case GO_YELLOW_CLUSTER_DISPLAY_LARGE:
									clusterIds[SIMON_YELLOW] = go->GetEntry();
									break;
                            }
                        }
                        else
                        {
                            switch (go->GetGOInfo()->displayId)
                            {
                                case GO_BLUE_CLUSTER_DISPLAY:
									clusterIds[SIMON_BLUE] = go->GetEntry();
									break;
                                case GO_RED_CLUSTER_DISPLAY:
									clusterIds[SIMON_RED] = go->GetEntry();
									break;
                                case GO_GREEN_CLUSTER_DISPLAY:
									clusterIds[SIMON_GREEN] = go->GetEntry();
									break;
                                case GO_YELLOW_CLUSTER_DISPLAY:
									clusterIds[SIMON_YELLOW] = go->GetEntry();
									break;
                            }
                        }
                    }
                }

                events.Reset();
                events.ScheduleEvent(EVENT_SIMON_ROUND_FINISHED, 1000);
                events.ScheduleEvent(EVENT_SIMON_PERIODIC_PLAYER_CHECK, 2000);

                if (GameObject* relic = me->FindGameobjectWithDistance(large ? GO_APEXIS_MONUMENT : GO_APEXIS_RELIC, searchDistance))
                    relic->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
            }

            // Called when despawning the bunny. Sets all the node GOs to their default states.
            void ResetNode()
            {
                DoPlaySoundToSet(me, SOUND_DISABLE_NODE);

                for (uint32 clusterId = SIMON_BLUE; clusterId < SIMON_MAX_COLORS; clusterId++)
                    if (GameObject* cluster = me->FindGameobjectWithDistance(clusterIds[clusterId], searchDistance))
                        cluster->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);

                for (uint32 auraId = GO_AURA_BLUE; auraId <= GO_AURA_YELLOW; auraId++)
                    if (GameObject* auraGo = me->FindGameobjectWithDistance(auraId, searchDistance))
                        auraGo->RemoveFromWorld();

                if (GameObject* relic = me->FindGameobjectWithDistance(large ? GO_APEXIS_MONUMENT : GO_APEXIS_RELIC, searchDistance))
                    relic->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);

                me->DespawnAfterAction(1*IN_MILLISECONDS);
            }

            void CheckPlayerSequence()
            {
                bool correct = true;
                if (playerSequence.size() <= colorSequence.size())
                    for (std::list<uint8>::const_iterator i = playerSequence.begin(), j = colorSequence.begin(); i != playerSequence.end(); ++i, ++j)
                        if ((*i) != (*j))
                            correct = false;

                if (correct && (playerSequence.size() == colorSequence.size()))
                    DoAction(ACTION_SIMON_CORRECT_FULL_SEQUENCE);
                else if (!correct)
                    DoAction(ACTION_SIMON_WRONG_SEQUENCE);
            }

            void GenerateColorSequence()
            {
                colorSequence.clear();
                for (uint8 i = 0; i <= gameLevel; i++)
                    colorSequence.push_back(RAND(SIMON_BLUE, SIMON_RED, SIMON_GREEN, SIMON_YELLOW));

                for (std::list<uint8>::const_iterator i = colorSequence.begin(); i != colorSequence.end(); ++i)
                    playableSequence.push_back(*i);
            }

            // Remove any existant glowing auras over clusters and set clusters ready for interating with them.
            void PrepareClusters(bool clustersOnly = false)
            {
                for (uint32 clusterId = SIMON_BLUE; clusterId < SIMON_MAX_COLORS; clusterId++)
                    if (GameObject* cluster = me->FindGameobjectWithDistance(clusterIds[clusterId], searchDistance))
                        cluster->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);

                if (clustersOnly)
                    return;

                for (uint32 auraId = GO_AURA_BLUE; auraId <= GO_AURA_YELLOW; auraId++)
                    if (GameObject* auraGo = me->FindGameobjectWithDistance(auraId, searchDistance))
                        auraGo->RemoveFromWorld();
            }

            void PlayNextColor()
            {
                PlayColor(*playableSequence.begin());
                playableSequence.erase(playableSequence.begin());
            }

            // Casts a spell and plays a sound depending on parameter color.
            void PlayColor(uint8 color)
            {
                switch (color)
                {
                    case SIMON_BLUE:
                        DoCast(SPELL_VISUAL_BLUE);
                        DoPlaySoundToSet(me, SOUND_BLUE);
                        break;
                    case SIMON_GREEN:
                        DoCast(SPELL_VISUAL_GREEN);
                        DoPlaySoundToSet(me, SOUND_GREEN);
                        break;
                    case SIMON_RED:
                        DoCast(SPELL_VISUAL_RED);
                        DoPlaySoundToSet(me, SOUND_RED);
                        break;
                    case SIMON_YELLOW:
                        DoCast(SPELL_VISUAL_YELLOW);
                        DoPlaySoundToSet(me, SOUND_YELLOW);
                        break;
                }
            }

            void SetUpPreGame()
            {
                for (uint32 clusterId = SIMON_BLUE; clusterId < SIMON_MAX_COLORS; clusterId++)
                {
                    if (GameObject* cluster = me->FindGameobjectWithDistance(clusterIds[clusterId], 2.0f*searchDistance))
                    {
                        cluster->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);

                        // break since we don't need glowing auras for large clusters
                        if (large)
                            break;

                        float x, y, z, o;
                        cluster->GetPosition(x, y, z, o);
                        me->NearTeleportTo(x, y, z, o);

                        uint32 preGameSpellId;
                        if (cluster->GetEntry() == clusterIds[SIMON_RED])
                            preGameSpellId = SPELL_PRE_GAME_RED;

                        else if (cluster->GetEntry() == clusterIds[SIMON_BLUE])
                            preGameSpellId = SPELL_PRE_GAME_BLUE;

                        else if (cluster->GetEntry() == clusterIds[SIMON_GREEN])
                            preGameSpellId = SPELL_PRE_GAME_GREEN;

                        else if (cluster->GetEntry() == clusterIds[SIMON_YELLOW])
                            preGameSpellId = SPELL_PRE_GAME_YELLOW;
                        else break;

                        me->CastSpell(cluster, preGameSpellId, true);
                    }
                }

                if (GameObject* relic = me->FindGameobjectWithDistance(large ? GO_APEXIS_MONUMENT : GO_APEXIS_RELIC, searchDistance))
                {
                    float x, y, z, o;
                    relic->GetPosition(x, y, z, o);
                    me->NearTeleportTo(x, y, z + zCoordCorrection, o);
                }

                GenerateColorSequence();
            }

            // Handles the spell rewards. The spells also have the QuestCompleteEffect, so quests credits are working.
            void GiveRewardForLevel(uint8 level)
            {
                uint32 rewSpell = 0;
                switch (level)
                {
                    case 6: 
                        if (large)
                            GivePunishment();
                        else
                            rewSpell = SPELL_REWARD_BUFF_1;
                        break;
                    case 8:
                        rewSpell = SPELL_REWARD_BUFF_2;
                        break;
                    case 10:
                        rewSpell = SPELL_REWARD_BUFF_3;
                        break;
                }

                if (rewSpell)
                    if (Player* player = me->GetPlayer(*me, playerGUID))
                        DoCast(player, rewSpell, true);
            }

            void GivePunishment()
            {
                if (large)
                {
                    if (Player* player = me->GetPlayer(*me, playerGUID))
                        if (Creature* guardian = me->SummonCreature(NPC_APEXIS_GUARDIAN, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() - zCoordCorrection, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 20000))
                            guardian->AI()->AttackStart(player);

                    ResetNode();
                }
                else
                {
                    fails++;

                    if (Player* player = me->GetPlayer(*me, playerGUID))
                        DoCast(player, SPELL_BAD_PRESS_TRIGGER, true);

                    if (fails >= 4)
                        ResetNode();
                }
            }

            void SpellHitTarget(Unit* target, const SpellInfo* spell)
            {
                if (spell->Id == SPELL_BAD_PRESS_TRIGGER)
                {
                    int32 bp = (int32)((float)(fails)*0.33f*target->GetMaxHealth());
                    target->CastCustomSpell(target, SPELL_BAD_PRESS_DAMAGE, &bp, NULL, NULL, true);
                }
            }

            // Checks if player has already die or has get too far from the current node
            bool CheckPlayer()
            {
                if (Player* player = me->GetPlayer(*me, playerGUID))
                {
                    if (player->IsDead())
                        return false;
                    if (player->GetDistance2d(me) >= 2.0f*searchDistance)
                    {
                        GivePunishment();
                        return false;
                    }
                }
                else
                    return false;

                return true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_simon_bunnyAI(creature);
        }
};

class go_simon_cluster : public GameObjectScript
{
public:
	go_simon_cluster() : GameObjectScript("go_simon_cluster") { }

	bool OnGossipHello(Player* player, GameObject* go)
	{
		if (Creature* bunny = go->FindCreatureWithDistance(NPC_SIMON_BUNNY, 12.0f, true))
			bunny->AI()->SetData(go->GetEntry(), 0);

		player->CastSpell(player, go->GetGOInfo()->goober.spellId, true);
		go->AddUse();
		return true;
	}
};

enum ApexisRelic
{
    QUEST_CRYSTALS            = 11025,
    GOSSIP_TEXT_ID            = 10948,
    ITEM_APEXIS_SHARD         = 32569,
    SPELL_TAKE_REAGENTS_SOLO  = 41145,
    SPELL_TAKE_REAGENTS_GROUP = 41146,
};

class go_apexis_relic : public GameObjectScript
{
    public:
        go_apexis_relic() : GameObjectScript("go_apexis_relic") { }

        bool OnGossipHello(Player* player, GameObject* go)
        {
            player->PrepareGossipMenu(go, go->GetGOInfo()->questgiver.gossipID);
            player->SendPreparedGossip(go);
            return true;
        }

        bool OnGossipSelect(Player* player, GameObject* go, uint32 /*sender*/, uint32 /*action*/)
        {
            player->CLOSE_GOSSIP_MENU();

            bool large = (go->GetEntry() == GO_APEXIS_MONUMENT);
            if (player->HasItemCount(ITEM_APEXIS_SHARD, large ? 35 : 1))
            {
                player->CastSpell(player, large ? SPELL_TAKE_REAGENTS_GROUP : SPELL_TAKE_REAGENTS_SOLO, false);

                if (Creature* bunny = player->SummonCreature(NPC_SIMON_BUNNY, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ()))
                    bunny->AI()->SetGUID(player->GetGUID(), large);
            }
            return true;
        }
};

enum ScalewingSerpent
{
    SPELL_LIGHTNING_STRIKE = 37841,
    SPELL_MAGNETO_SPHERE   = 37830,

    NPC_RIDE_THE_LIGHTNING = 21910  // Quest: Ride the Lightning, #10657
};

class npc_scalewing_serpent : public CreatureScript
{
    public:
        npc_scalewing_serpent() : CreatureScript("npc_scalewing_serpent") { }

        struct npc_scalewing_serpentAI : public QuantumBasicAI
        {
            npc_scalewing_serpentAI(Creature* creature) : QuantumBasicAI(creature){}

			uint32 LightningStrikeTimer;

            void Reset()
            {
                LightningStrikeTimer = 5000;
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_LIGHTNING_STRIKE)
				{
					if (target->ToPlayer()->HasAura(SPELL_MAGNETO_SPHERE))
						target->ToPlayer()->KilledMonsterCredit(NPC_RIDE_THE_LIGHTNING, 0);
				}
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (LightningStrikeTimer <= diff)
                {
                    DoCast(SPELL_LIGHTNING_STRIKE);
                    LightningStrikeTimer = urand(4000, 8000);
                }
                else LightningStrikeTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_scalewing_serpentAI(creature);
        }
};

enum AntelarionMisc
{
	SPELL_CREATE_FELSWORD_GAS_MASK = 39101,
	ITEM_FELSWORD_GAS_MASK         = 31366,
	QUEST_FELSWORD_GAS_MASK        = 10819,
	QUEST_YOU_RE_FIRED             = 10821,
 };

#define SAY_GOSSIP_HELLO "It's a good thing I was able to make duplicates of the Felsworn Gas Mask."
#define GOSSIP_ITEM_GAS_MASK "Please, give me a new Felsworn Gas Mask."

class npc_antelarion : public CreatureScript
{
    public:
        npc_antelarion() : CreatureScript("npc_antelarion") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

			if (!player->HasItemCount(ITEM_FELSWORD_GAS_MASK, 1, true) && player->GetQuestStatus(QUEST_FELSWORD_GAS_MASK) == QUEST_STATUS_REWARDED && !player->GetQuestRewardStatus(QUEST_YOU_RE_FIRED))
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GAS_MASK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 0);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
			creature->MonsterSay(SAY_GOSSIP_HELLO, LANG_UNIVERSAL, 0);

			player->PlayerTalkClass->ClearMenus();

			switch (action)
            {
			   case GOSSIP_ACTION_INFO_DEF + 0:
				   player->CLOSE_GOSSIP_MENU();
				   player->CastSpell(player, SPELL_CREATE_FELSWORD_GAS_MASK);
				   break;
			}
			return true;
		}
};

enum EvergroveDruid
{
	SPELL_DRUID_SIGNAL = 38782,
    SPELL_DRUID_FORM   = 39158,
    SPELL_CROW_FORM    = 38776,

    QUEST_DEATH_DOOR   = 10910,

    ITEM_DRUID_SIGNAL  = 31763,
};
class npc_evergrove_druid : public CreatureScript
{
public:
    npc_evergrove_druid() : CreatureScript("npc_evergrove_druid") { }

    struct npc_evergrove_druidAI : public QuantumBasicAI
    {
        npc_evergrove_druidAI(Creature* creature) : QuantumBasicAI(creature) { }

        Player* player;
        Position playerPos;

        bool summoned;
        float x, y, z;

        void Reset()
        {
            me->SetCanFly(true);
            me->SetSpeed(MOVE_FLIGHT, 3.2f, true);
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUEST_GIVER | UNIT_NPC_FLAG_GOSSIP);
            me->SetVisible(false);
            player = NULL;
            summoned = false;
        }

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (summoned)
                return;

            if (spell->Id == SPELL_DRUID_SIGNAL)
            {
                summoned = true;

                if (caster)
                    player = caster->ToPlayer();

                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUEST_GIVER | UNIT_NPC_FLAG_GOSSIP);
                me->SetUInt64Value(UNIT_FIELD_TARGET, player->GetGUID());
                me->SetVisible(true);
                me->CastSpell(me, SPELL_CROW_FORM, true);

                if (player)
                {
                    x = player->GetPositionX();
					y = player->GetPositionY();
					z = player->GetPositionZ();
                    me->GetMotionMaster()->MovePoint(0, x, y, z);
                }
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (!player)
                return;

            me->UpdatePosition(x, y, z, 0.0f, true);

            if (!player->IsFlying())
            {
                me->CastSpell(me, SPELL_DRUID_FORM, true);
                me->RemoveAurasDueToSpell(SPELL_CROW_FORM);
                me->SetCanFly(false);
                me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_STAND);
            }

            me->DespawnAfterAction(1*MINUTE*IN_MILLISECONDS);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_evergrove_druidAI(creature);
    }
};

enum QuestBanishMoreDemons
{
	SPELL_CHANNEL                       = 36854,
	SPELL_KILL_CREDIT_ON_MASTER         = 40828,
	SPELL_BANISHMENT_PERIODIC_AURA      = 40849,
	SPELL_BANISHMENT_BEAM_PERIODIC_AURA = 40857,
};

class npc_banishing_crystal_bunny : public CreatureScript
{
public:
    npc_banishing_crystal_bunny() : CreatureScript("npc_banishing_crystal_bunny") {}

    struct npc_banishing_crystal_bunnyAI : public QuantumBasicAI
    {
        npc_banishing_crystal_bunnyAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			DoCast(me, SPELL_BANISHMENT_PERIODIC_AURA);
			DoCast(me, SPELL_BANISHMENT_BEAM_PERIODIC_AURA);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_banishing_crystal_bunnyAI(creature);
    }
};

enum FelCannonDummy
{
	SPELL_FEL_CANNON_TRIGGER = 40110,
	SPELL_FEL_CANNON_AGRRO   = 40119,
	SPELL_FEL_CANNON_AURA    = 40113,

	NPC_LEGION_FLAK_CANNON   = 23076,
};

class npc_knockdown_fel_cannon_dummy : public CreatureScript
{
public:
    npc_knockdown_fel_cannon_dummy() : CreatureScript("npc_knockdown_fel_cannon_dummy") {}

    struct npc_knockdown_fel_cannon_dummyAI : public QuantumBasicAI
    {
        npc_knockdown_fel_cannon_dummyAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        void Reset()
        {
			me->AddAura(SPELL_FEL_CANNON_AURA, me);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_FEL_CANNON_TRIGGER)
			{
				if (Creature* cannon = me->FindCreatureWithDistance(NPC_LEGION_FLAK_CANNON, 10.0f))
					DoCast(cannon, SPELL_FEL_CANNON_AGRRO);
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_knockdown_fel_cannon_dummyAI(creature);
    }
};

void AddSC_blades_edge_mountains()
{
    new mobs_nether_drake();
    new npc_daranelle();
    new npc_overseer_nuaar();
    new npc_saikkal_the_elder();
    new go_legion_obelisk();
    new npc_bloodmaul_brutebane();
    new npc_bladespire_brute();
    new npc_aether_ray();
    new npc_wrangled_aether_ray();
	new go_thunderspike();
	new npc_simon_bunny();
	new go_simon_cluster();
	new go_apexis_relic();
	new npc_scalewing_serpent();
	new npc_antelarion();
	new npc_evergrove_druid();
	new npc_banishing_crystal_bunny();
	new npc_knockdown_fel_cannon_dummy();
}