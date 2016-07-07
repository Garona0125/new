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
#include "Vehicle.h"

#define GOSSIP_ITEM1 "You're free to go Orsonn, but first tell me what's wrong with the furbolg."
#define GOSSIP_ITEM2 "What happened then?"
#define GOSSIP_ITEM3 "Thank you, Son of Ursoc. I'll see what can be done."
#define GOSSIP_ITEM4 "Who was this stranger?"
#define GOSSIP_ITEM5 "Thank you, Kodian. I'll do what I can."

enum OrKodMisc
{
    GOSSIP_TEXTID_ORSONN1          = 12793,
    GOSSIP_TEXTID_ORSONN2          = 12794,
    GOSSIP_TEXTID_ORSONN3          = 12796,
    GOSSIP_TEXTID_KODIAN1          = 12797,
    GOSSIP_TEXTID_KODIAN2          = 12798,

    NPC_ORSONN                     = 27274,
    NPC_KODIAN                     = 27275,
    NPC_ORSONN_CREDIT              = 27322,
    NPC_KODIAN_CREDIT              = 27321,

    QUEST_CHILDREN_OF_URSOC        = 12247,
    QUEST_THE_BEAR_GODS_OFFSPRING  = 12231,
};

class npc_orsonn_and_kodian : public CreatureScript
{
public:
    npc_orsonn_and_kodian() : CreatureScript("npc_orsonn_and_kodian") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_CHILDREN_OF_URSOC) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_THE_BEAR_GODS_OFFSPRING) == QUEST_STATUS_INCOMPLETE)
        {
            switch (creature->GetEntry())
            {
                case NPC_ORSONN:
                    if (!player->GetReqKillOrCastCurrentCount(QUEST_CHILDREN_OF_URSOC, NPC_ORSONN_CREDIT) || !player->GetReqKillOrCastCurrentCount(QUEST_THE_BEAR_GODS_OFFSPRING, NPC_ORSONN_CREDIT))
                    {
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ORSONN1, creature->GetGUID());
                        return true;
                    }
                    break;
                case NPC_KODIAN:
                    if (!player->GetReqKillOrCastCurrentCount(QUEST_CHILDREN_OF_URSOC, NPC_KODIAN_CREDIT) || !player->GetReqKillOrCastCurrentCount(QUEST_THE_BEAR_GODS_OFFSPRING, NPC_KODIAN_CREDIT))
                    {
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
                        player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_KODIAN1, creature->GetGUID());
                        return true;
                    }
                    break;
            }
        }
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ORSONN2, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ORSONN3, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->CLOSE_GOSSIP_MENU();
                player->TalkedToCreature(NPC_ORSONN_CREDIT, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_KODIAN2, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+5:
                player->CLOSE_GOSSIP_MENU();
                player->TalkedToCreature(NPC_KODIAN_CREDIT, creature->GetGUID());
                break;
        }
        return true;
    }
};

enum Floppy
{
    NPC_MR_FLOPPY               = 26589,
    NPC_HUNGRY_WORG             = 26586,
    NPC_RAVENOUS_WORG           = 26590,
    NPC_EMILY                   = 26588,

    QUEST_PERILOUS_ADVENTURE    = 12027,

    SPELL_MRFLOPPY              = 47184,    //vehicle aura

    SAY_WORGHAGGRO1             = -1800001, //Um... I think one of those wolves is back...
    SAY_WORGHAGGRO2             = -1800002, //He's going for Mr. Floppy!
    SAY_WORGRAGGRO3             = -1800003, //Oh, no! Look, it's another wolf, and it's a biiiiiig one!
    SAY_WORGRAGGRO4             = -1800004, //He's gonna eat Mr. Floppy! You gotta help Mr. Floppy! You just gotta!
    SAY_RANDOMAGGRO             = -1800005, //There's a big meanie attacking Mr. Floppy! Help!
    SAY_VICTORY1                = -1800006, //Let's get out of here before more wolves find us!
    SAY_VICTORY2                = -1800007, //Don't go toward the light, Mr. Floppy!
    SAY_VICTORY3                = -1800008, //Mr. Floppy, you're ok! Thank you so much for saving Mr. Floppy!
    SAY_VICTORY4                = -1800009, //I think I see the camp! We're almost home, Mr. Floppy! Let's go!
    TEXT_EMOTE_WP1              = -1800010, //Mr. Floppy revives
    TEXT_EMOTE_AGGRO            = -1800011, //The Ravenous Worg chomps down on Mr. Floppy
    SAY_QUEST_ACCEPT            = -1800012, //Are you ready, Mr. Floppy? Stay close to me and watch out for those wolves!
    SAY_QUEST_COMPLETE          = -1800013,  //Thank you for helping me get back to the camp. Go tell Walter that I'm safe now!
};

class npc_emily : public CreatureScript
{
public:
    npc_emily() : CreatureScript("npc_emily") { }

	bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_PERILOUS_ADVENTURE)
        {
            DoSendQuantumText(SAY_QUEST_ACCEPT, creature);

            if (Creature* Mrfloppy = GetClosestCreatureWithEntry(creature, NPC_MR_FLOPPY, 180.0f))
                Mrfloppy->GetMotionMaster()->MoveFollow(creature, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);

            if (npc_escortAI* EscortAI = CAST_AI(npc_emily::npc_emilyAI, (creature->AI())))
                EscortAI->Start(true, false, player->GetGUID());
        }
        return true;
    }

    struct npc_emilyAI : public npc_escortAI
    {
        npc_emilyAI(Creature* creature) : npc_escortAI(creature) { }

        uint32 ChatTimer;
        uint64 RWorgGUID;
        uint64 MrfloppyGUID;

        bool Completed;

        void JustSummoned(Creature* summoned)
        {
            if (Creature* Mrfloppy = GetClosestCreatureWithEntry(me, NPC_MR_FLOPPY, 50.0f))
                summoned->AI()->AttackStart(Mrfloppy);
            else
                summoned->AI()->AttackStart(me->GetVictim());
        }

        void WaypointReached(uint32 id)
        {
            Player* player = GetPlayerForEscort();

            if (!player)
                return;

            switch (id)
            {
                case 9:
                    if (Creature* Mrfloppy = GetClosestCreatureWithEntry(me, NPC_MR_FLOPPY, 100.0f))
                        MrfloppyGUID = Mrfloppy->GetGUID();
                    break;
                case 10:
                    if (Unit::GetCreature(*me, MrfloppyGUID))
                    {
                        DoSendQuantumText(SAY_WORGHAGGRO1, me);
                        me->SummonCreature(NPC_HUNGRY_WORG, me->GetPositionX()+5, me->GetPositionY()+2, me->GetPositionZ()+1, 3.229f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000);
                    }
                    break;
                case 11:
                    if (Creature* Mrfloppy = Unit::GetCreature(*me, MrfloppyGUID))
                        Mrfloppy->GetMotionMaster()->MoveFollow(me, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                    break;
                case 17:
                    if (Creature* Mrfloppy = Unit::GetCreature(*me, MrfloppyGUID))
                        Mrfloppy->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
                    DoSendQuantumText(SAY_WORGRAGGRO3, me);
                    if (Creature* RWORG = me->SummonCreature(NPC_RAVENOUS_WORG, me->GetPositionX()+10, me->GetPositionY()+8, me->GetPositionZ()+2, 3.229f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000))
                    {
                        RWORG->SetCurrentFaction(35);
                        RWorgGUID = RWORG->GetGUID();
                    }
                    break;
                case 18:
                    if (Creature* Mrfloppy = Unit::GetCreature(*me, MrfloppyGUID))
                    {
                        if (Creature* RWORG = Unit::GetCreature(*me, RWorgGUID))
                            RWORG->GetMotionMaster()->MovePoint(0, Mrfloppy->GetPositionX(), Mrfloppy->GetPositionY(), Mrfloppy->GetPositionZ());
                        DoCast(Mrfloppy, SPELL_MRFLOPPY);
                    }
                    break;
                case 19:
                    if (Creature* Mrfloppy = Unit::GetCreature(*me, MrfloppyGUID))
                    {
                        if (Mrfloppy->HasAura(SPELL_MRFLOPPY, 0))
                        {
                            if (Creature* RWORG = Unit::GetCreature(*me, RWorgGUID))
								Mrfloppy->EnterVehicle(RWORG);
                        }
                    }
                    break;
                case 20:
                    if (Creature* RWORG = Unit::GetCreature(*me, RWorgGUID))
                        RWORG->HandleEmoteCommand(34);
                    break;
                case 21:
                    if (Creature* Mrfloppy = Unit::GetCreature(*me, MrfloppyGUID))
                    {
                        if (Creature* RWORG = Unit::GetCreature(*me, RWorgGUID))
                        {
                            RWORG->Kill(Mrfloppy);
                            Mrfloppy->ExitVehicle();
                            RWORG->SetCurrentFaction(16);
                            RWORG->GetMotionMaster()->MovePoint(0, RWORG->GetPositionX()+10, RWORG->GetPositionY()+80, RWORG->GetPositionZ());
                            DoSendQuantumText(SAY_VICTORY2, me);
                        }
                    }
                    break;
                case 22:
                    if (Creature* Mrfloppy = Unit::GetCreature(*me, MrfloppyGUID))
                    {
                        if (Mrfloppy->IsDead())
                        {
                            if (Creature* RWORG = Unit::GetCreature(*me, RWorgGUID))
                                RWORG->DisappearAndDie();
                            me->GetMotionMaster()->MovePoint(0, Mrfloppy->GetPositionX(), Mrfloppy->GetPositionY(), Mrfloppy->GetPositionZ());
                            Mrfloppy->setDeathState(ALIVE);
                            Mrfloppy->GetMotionMaster()->MoveFollow(me, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
                            DoSendQuantumText(SAY_VICTORY3, me);
                        }
                    }
                    break;
                case 24:
                    if (player)
                    {
                        Completed = true;
                        player->GroupEventHappens(QUEST_PERILOUS_ADVENTURE, me);
                        DoSendQuantumText(SAY_QUEST_COMPLETE, me, player);
                    }
                    me->SetWalk(false);
                    break;
                case 25:
                    DoSendQuantumText(SAY_VICTORY4, me);
                    break;
                case 27:
                    me->DisappearAndDie();
                    if (Creature* Mrfloppy = Unit::GetCreature(*me, MrfloppyGUID))
                        Mrfloppy->DisappearAndDie();
                    break;
            }
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_RANDOMAGGRO, me);
        }

        void Reset()
        {
            ChatTimer = 4000;
            MrfloppyGUID = 0;
            RWorgGUID = 0;
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (HasEscortState(STATE_ESCORT_ESCORTING))
            {
                if (ChatTimer <= diff)
                {
                    ChatTimer = 12000;
                }
                else ChatTimer -= diff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_emilyAI(creature);
    }
};

class npc_mr_floppy : public CreatureScript
{
public:
    npc_mr_floppy() : CreatureScript("npc_mr_floppy") { }

    struct npc_mr_floppyAI : public QuantumBasicAI
    {
        npc_mr_floppyAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint64 EmilyGUID;
        uint64 RWorgGUID;
        uint64 HWorgGUID;

        void Reset() {}

        void EnterToBattle(Unit* who)
        {
            if (Creature* emily = GetClosestCreatureWithEntry(me, NPC_EMILY, 50.0f))
            {
                switch (who->GetEntry())
                {
                    case NPC_HUNGRY_WORG:
                        DoSendQuantumText(SAY_WORGHAGGRO2, emily);
                        break;
                    case NPC_RAVENOUS_WORG:
                        DoSendQuantumText(SAY_WORGRAGGRO4, emily);
                        break;
                    default:
                        DoSendQuantumText(SAY_RANDOMAGGRO, emily);
                }
            }
        }

        void EnterEvadeMode(){}

        void MoveInLineOfSight(Unit* /*who*/){}

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mr_floppyAI(creature);
    }
};

enum OuthouseBunny
{
    SPELL_OUTHOUSE_GROANS        = 48382,
    SPELL_CAMERA_SHAKE           = 47533,
    SPELL_DUST_FIELD             = 48329,
};

enum Sounds
{
	SOUND_MALE   = 12670,
    SOUND_FEMALE = 12671,
};

class npc_outhouse_bunny : public CreatureScript
{
public:
    npc_outhouse_bunny() : CreatureScript("npc_outhouse_bunny") { }

    struct npc_outhouse_bunnyAI : public QuantumBasicAI
    {
        npc_outhouse_bunnyAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint8 Counter;
        uint8 Gender;

        void Reset()
        {
            Counter = 0;
            Gender = 0;
        }

        void SetData(uint32 type, uint32 data)
        {
            if (type == 1)
                Gender = data;
        }

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
			if (spell->Id == SPELL_OUTHOUSE_GROANS)
            {
                ++Counter;
                if (Counter < 5)
                    DoCast(caster, SPELL_CAMERA_SHAKE, true);
                else
                    Counter = 0;
                DoCast(me, SPELL_DUST_FIELD, true);

                switch (Gender)
                {
                    case GENDER_FEMALE:
						DoPlaySoundToSet(me, SOUND_FEMALE);
						break;
                    case GENDER_MALE:
						DoPlaySoundToSet(me, SOUND_MALE);
						break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_outhouse_bunnyAI(creature);
    }
};


enum TallhornStag
{
	SPELL_GORE = 32019,

    GO_HAUNCH  = 188665,
};

class npc_tallhorn_stag : public CreatureScript
{
public:
    npc_tallhorn_stag() : CreatureScript("npc_tallhorn_stag") { }

    struct npc_tallhorn_stagAI : public QuantumBasicAI
    {
        npc_tallhorn_stagAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint8 Phase;
        uint32 GoreTimer;

        void Reset()
        {
            Phase = 1;
            GoreTimer = 2*IN_MILLISECONDS;
        }

        void UpdateAI(const uint32 diff)
        {
            if (Phase == 1)
            {
                if (me->FindGameobjectWithDistance(GO_HAUNCH, 5.0f))
                {
                    me->SetStandState(UNIT_STAND_STATE_DEAD);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                    me->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                }
                Phase = 0;
            }

            if (!UpdateVictim())
                return;

            if (GoreTimer <= diff)
            {
                DoCast(SPELL_GORE);
                GoreTimer = 5*IN_MILLISECONDS;
            }
            else GoreTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tallhorn_stagAI(creature);
    }
};

enum AmberpineWoodsman
{
    NPC_TALLHORN_STAG = 26363,
};

class npc_amberpine_woodsman : public CreatureScript
{
public:
    npc_amberpine_woodsman() : CreatureScript("npc_amberpine_woodsman") { }

    struct npc_amberpine_woodsmanAI : public QuantumBasicAI
    {
        npc_amberpine_woodsmanAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint8 Phase;
        uint32 Timer;

        void Reset()
        {
            Timer = 0;
            Phase = 1;
        }

        void UpdateAI(const uint32 diff)
        {
            if (me->FindCreatureWithDistance(NPC_TALLHORN_STAG, 1.0f))
            {
                me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_USE_STANDING);
            }
            else
                if (Phase)
                {
                    if (Timer <= diff)
                    {
                        switch (Phase)
                        {
                            case 1:
                                me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_LOOT);
                                Timer = 3000;
                                Phase = 2;
                                break;
                            case 2:
                                me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_ONESHOT_ATTACK_1H);
                                Timer = 4000;
                                Phase = 1;
								break;
						}
					}
					else Timer -= diff;
				}
				QuantumBasicAI::UpdateAI(diff);
				
				UpdateVictim();
		}
	};
	
	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amberpine_woodsmanAI(creature);
    }
};

enum Skirmisher
{
    SPELL_RENEW_SKIRMISHER = 48812,
    SPELL_RENEW_INFANTRY   = 48845,
    CREDIT_NPC             = 27466,

    QUEST_LIFE_OR_DEATH    = 12296,
    QUEST_OVERWHELMED      = 12288,

    NPC_WOUNDED_INFANTRY   = 27482,
    NPC_WOUNDED_SKIRMISHER = 27463,

    RANDOM_SAY_1           = -1800044,        //Ahh..better..
    RANDOM_SAY_2           = -1800045,        //Whoa.. i nearly died there. Thank you, $Race!
    RANDOM_SAY_3           = -1800046,         //Thank you. $Class!
};

class npc_wounded_skirmisher : public CreatureScript
{
public:
    npc_wounded_skirmisher() : CreatureScript("npc_wounded_skirmisher") { }

    struct npc_wounded_skirmisherAI : public QuantumBasicAI
    {
        npc_wounded_skirmisherAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 DespawnTimer;

        void Reset ()
        {
            DespawnTimer = 5000;
        }

        void MovementInform(uint32, uint32 id)
        {
            if (id == 1)
                me->DespawnAfterAction(DespawnTimer);
        }

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (caster->GetTypeId() == TYPE_ID_PLAYER)
			{
                if ((me->GetEntry() == NPC_WOUNDED_INFANTRY && spell->Id == SPELL_RENEW_INFANTRY && caster->ToPlayer()->GetQuestStatus(QUEST_LIFE_OR_DEATH) == QUEST_STATUS_INCOMPLETE) ||
					(me->GetEntry() == NPC_WOUNDED_SKIRMISHER && spell->Id == SPELL_RENEW_SKIRMISHER && caster->ToPlayer()->GetQuestStatus(QUEST_OVERWHELMED) == QUEST_STATUS_INCOMPLETE))
                {
					caster->ToPlayer()->KilledMonsterCredit(CREDIT_NPC, 0);
                    DoSendQuantumText(RAND(RANDOM_SAY_1, RANDOM_SAY_2, RANDOM_SAY_3), me);
                    if (me->IsStandState())
                        me->GetMotionMaster()->MovePoint(1, me->GetPositionX()+7, me->GetPositionY()+7, me->GetPositionZ());
                    else
                    {
                        me->SetStandState(UNIT_STAND_STATE_STAND);
                        me->DespawnAfterAction(DespawnTimer);
                    }
				}
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
        return new npc_wounded_skirmisherAI(creature);
    }
};

enum Sentry
{
    QUEST_OR_MAYBE_WE_DONT_A    = 12138,
    QUEST_OR_MAYBE_WE_DONT_H    = 12198,

    NPC_LIGHTNING_SENTRY        = 26407,
    NPC_WAR_GOLEM               = 27017,

    SPELL_CHARGED_SENTRY_TOTEM  = 52703,
};

class npc_lightning_sentry : public CreatureScript
{
public:
    npc_lightning_sentry() : CreatureScript("npc_lightning_sentry") { }

    struct npc_lightning_sentryAI : public QuantumBasicAI
    {
        npc_lightning_sentryAI(Creature* creature) : QuantumBasicAI(creature) { }

        uint32 ChargedSentryTotem;

        void Reset()
        {
            ChargedSentryTotem = 0.5*IN_MILLISECONDS;
        }

		void JustDied(Unit* killer)
        {
			if (Player* player = killer->ToPlayer())
			{
				if (me->FindCreatureWithDistance(NPC_WAR_GOLEM, 10.0f, true))
				{
					if (player->GetQuestStatus(QUEST_OR_MAYBE_WE_DONT_A) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_OR_MAYBE_WE_DONT_H) == QUEST_STATUS_INCOMPLETE)
						player->KilledMonsterCredit(NPC_WAR_GOLEM, 0);
				}
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (ChargedSentryTotem <= diff)
            {
                DoCast(SPELL_CHARGED_SENTRY_TOTEM);
                ChargedSentryTotem = urand(10*IN_MILLISECONDS, 12*IN_MILLISECONDS);
            }
            else ChargedSentryTotem -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lightning_sentryAI(creature);
    }
};

enum SmokeEmOut
{
    SAY_SEO1                                     = -1603535,
    SAY_SEO2                                     = -1603536,
    SAY_SEO3                                     = -1603537,
    SAY_SEO4                                     = -1603538,
    SAY_SEO5                                     = -1603539,

	SPELL_SMOKE_BOMB                             = 49075,
    SPELL_CHOP                                   = 43410,
    SPELL_VENTURE_STRAGGLER_CREDIT               = 49093,

    QUEST_SMOKE_EM_OUT_A                         = 12323,
    QUEST_SMOKE_EM_OUT_H                         = 12324,
};

class npc_venture_co_straggler : public CreatureScript
{
    public:
        npc_venture_co_straggler() : CreatureScript("npc_venture_co_straggler") { }

        struct npc_venture_co_stragglerAI : public QuantumBasicAI
        {
            npc_venture_co_stragglerAI(Creature* creature) : QuantumBasicAI(creature) { }

            uint64 PlayerGUID;
            uint32 RunAwayTimer;
            uint32 Timer;
            uint32 ChopTimer;

            void Reset()
            {
                PlayerGUID = 0;
                Timer = 0;
                RunAwayTimer = 0;
                ChopTimer = urand(10000, 12500);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                me->SetReactState(REACT_AGGRESSIVE);
            }

			void SpellHit(Unit* caster, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_SMOKE_BOMB && caster->GetTypeId() == TYPE_ID_PLAYER)
                {
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                    me->SetReactState(REACT_PASSIVE);
                    me->CombatStop(false);
                    PlayerGUID = caster->GetGUID();
                    RunAwayTimer = 3500;
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (PlayerGUID && RunAwayTimer <= diff)
                {
                    if (Player* player = Unit::GetPlayer(*me, PlayerGUID))
                    {
                        switch (Timer)
                        {
                            case 0:
                                DoCast(player, SPELL_VENTURE_STRAGGLER_CREDIT);
                                me->GetMotionMaster()->MovePoint(0, me->GetPositionX()-7, me->GetPositionY()+7, me->GetPositionZ());
                                RunAwayTimer = 2500;
                                ++Timer;
                                break;
                            case 1:
                                DoSendQuantumText(RAND(SAY_SEO1, SAY_SEO2, SAY_SEO3, SAY_SEO4, SAY_SEO5), me);
                                me->GetMotionMaster()->MovePoint(0, me->GetPositionX()-7, me->GetPositionY()-5, me->GetPositionZ());
                                RunAwayTimer = 2500;
                                ++Timer;
                                break;
                            case 2:
                                me->GetMotionMaster()->MovePoint(0, me->GetPositionX()-5, me->GetPositionY()-5, me->GetPositionZ());
                                RunAwayTimer = 2500;
                                ++Timer;
                                break;
                            case 3:
                                me->DisappearAndDie();
                                Timer = 0;
                                break;
                        }
                    }
                }
                else if (RunAwayTimer)
                    RunAwayTimer -= diff;

                if (!UpdateVictim())
                    return;

                if (ChopTimer <= diff)
                {
                    DoCastVictim(SPELL_CHOP);
                    ChopTimer = urand(10000, 12000);
                }
                else ChopTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_venture_co_stragglerAI(creature);
        }
};

enum LakeFrog
{
    SPELL_WARTS                = 62581,
    SPELL_WARTS_B_GONE         = 62574,
    SPELL_TRANSFORM            = 62550,
    SPELL_SUMMON_ASHWOOD_BRAND = 62554,
    SPELL_FROG_LOVE            = 62537,

    NPC_NOT_MAIDEN_FROG        = 33211,
    NPC_MAIDEN_FROG            = 33224,
};

#define SAY_FREED              "Can it really be? Free after all these years?"
#define GOSSIP_TEXT_GET_WEAPON "Glad to help, my lady. I'm told you were once the guardian of a fabled sword. Do you know where I might find it?"

class npc_lake_frog : public CreatureScript
{
public:
    npc_lake_frog() : CreatureScript("npc_lake_frog") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TEXT_GET_WEAPON , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        player->CLOSE_GOSSIP_MENU();

        if (action == GOSSIP_ACTION_INFO_DEF + 1)
            creature->CastSpell(player, SPELL_SUMMON_ASHWOOD_BRAND, true);

        return true;
    }

    struct npc_lake_frogAI : public QuantumBasicAI
    {
        npc_lake_frogAI(Creature* creature) : QuantumBasicAI(creature)
		{
			AlreadyKissed = false;
		}

        bool AlreadyKissed;

        void ReceiveEmote(Player* player, uint32 emote)
        {
            if (emote == TEXT_EMOTE_KISS)
            {
                AlreadyKissed = true;

                if (urand(0, 3) == 0)
                {
                    DoCast(me, SPELL_TRANSFORM, true);
                    me->MonsterSay(SAY_FREED, LANG_UNIVERSAL, player->GetGUID());
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                }
				else
                {
                    if (!player->HasAura(SPELL_WARTS_B_GONE))
                        me->CastSpell(player, SPELL_WARTS);

					else player->RemoveAurasDueToSpell(SPELL_WARTS_B_GONE);

					me->CastSpell(me, SPELL_FROG_LOVE, true);
					me->GetMotionMaster()->MoveFollow(player, 1, float(rand_norm()*2*M_PI));
				}
                me->DespawnAfterAction(15*IN_MILLISECONDS);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lake_frogAI(creature);
    }
};

enum Shredders
{
	NPC_BROKEN_DOWN_SHREDDERS = 27354,
	NPC_KILL_CREDIT_SHREDDERS = 27396,
};

class npc_shredders_taker : public CreatureScript
{
public:
    npc_shredders_taker() : CreatureScript("npc_shredders_taker") { }

    struct npc_shredders_takerAI : public QuantumBasicAI
    {
        npc_shredders_takerAI(Creature* creature) : QuantumBasicAI(creature) { }

        void MoveInLineOfSight(Unit* who)
        {
            if (!who)
                return;

            if (who->GetTypeId() != TYPE_ID_UNIT || who->GetEntry() != NPC_BROKEN_DOWN_SHREDDERS || !who->IsCharmedOwnedByPlayerOrPlayer())
                return;

            Player* player = who->GetCharmerOrOwner()->ToPlayer();

            if (player && me->IsWithinDistInMap(who, 15.0f))
            {
                player->KilledMonsterCredit(NPC_KILL_CREDIT_SHREDDERS, 0);
                player->ExitVehicle();

                who->ToCreature()->DisappearAndDie();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shredders_takerAI(creature);
    }
};

class npc_brokendown_shredders : public CreatureScript
{
public:
    npc_brokendown_shredders() : CreatureScript("npc_brokendown_shredders") { }

    struct npc_brokendown_shreddersAI : public QuantumBasicAI
    {
        npc_brokendown_shreddersAI(Creature* creature) : QuantumBasicAI(creature) { }

        void UpdateAI(const uint32 /*diff*/)
        {
            Unit* player = me->GetVehicleKit()->GetPassenger(0);
            if (!player && ((me->GetPositionX() != me->GetHomePosition().GetPositionX()) && (me->GetPositionY() != me->GetHomePosition().GetPositionY()) && (me->GetPositionZ() != me->GetHomePosition().GetPositionZ())))
            {
                me->DisappearAndDie();
                me->Respawn();
                return;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_brokendown_shreddersAI(creature);
    }
};

enum RocketWarhead
{
	SPELL_WARHEAD_FUSE        = 49181,
	SPELL_SHOCK_VORTEX        = 71945,

	NPC_ALLIANCE_LUMBERT_BOAT = 27688,
	NPC_HORDE_LUMBERT_BOAT    = 27702,
};

class npc_rocket_warhead : public CreatureScript
{
public:
    npc_rocket_warhead() : CreatureScript("npc_rocket_warhead") { }

    struct npc_rocket_warheadAI : public QuantumBasicAI
    {
        npc_rocket_warheadAI(Creature* creature) : QuantumBasicAI(creature) { }

        bool Occupied;

        void Reset()
        {
            Occupied = false;

            me->SetHover(true);
        }

		void PassengerBoarded(Unit* /*passenger*/, int8 /*seatId*/, bool /*apply*/)
        {
            me->AddAura(SPELL_WARHEAD_FUSE, me);

            Occupied = true;
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!Occupied)
                return;

            Unit* player = me->GetVehicleKit()->GetPassenger(0);

            if (!player)
            {
                me->DisappearAndDie();
                return;
            }

            if (player && !me->HasAura(SPELL_WARHEAD_FUSE, 0))
            {
                player->ExitVehicle();
                DoCast(SPELL_SHOCK_VORTEX);
                me->DespawnAfterAction(1.5*IN_MILLISECONDS);
                return;
            }

            Unit* BombTarget;

            switch (player->ToPlayer()->GetTeam())
            {
                case ALLIANCE:
					BombTarget = me->FindCreatureWithDistance(NPC_HORDE_LUMBERT_BOAT, 8.0f);
					break;
                case HORDE:
					BombTarget = me->FindCreatureWithDistance(NPC_ALLIANCE_LUMBERT_BOAT, 8.0f);
					break;
            }

            if (player && BombTarget)
            {
                player->ToPlayer()->KilledMonsterCredit(BombTarget->GetEntry(), 0);
                player->ExitVehicle();
                DoCast(SPELL_SHOCK_VORTEX);
                me->DespawnAfterAction(1.5*IN_MILLISECONDS);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_rocket_warheadAI(creature);
    }
};

enum QuestFreeAtLast
{
	SPELL_RUNIC_COMPULSION        = 47329,
	SPELL_RUNE_FIST               = 49717,
	SPELL_STOMP                   = 55196,
	SPELL_GAVROCK_RUNEBREAKER     = 47604,
	SPELL_NATURE_WEAKNESS         = 25180,

	NPC_FREED_GIANT_KILL_CREDIT   = 26783,

	QUEST_FREE_AT_LAST            = 12099,

	RUNED_GIANT_SAY_AGGRO_1       = -1524700,
	RUNED_GIANT_SAY_AGGRO_2       = -1524701,
	RUNED_GIANT_SAY_AGGRO_3       = -1524702,
};

class npc_runed_giant : public CreatureScript
{
public:
    npc_runed_giant() : CreatureScript("npc_runed_giant") {}

    struct npc_runed_giantAI : public QuantumBasicAI
    {
		npc_runed_giantAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RuneFistTimer;
		uint32 StompTimer;

        void Reset()
        {
			DoCast(me, SPELL_RUNIC_COMPULSION);

			RuneFistTimer = 1000;
			StompTimer = 4000;
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoSendQuantumText(RAND(RUNED_GIANT_SAY_AGGRO_1, RUNED_GIANT_SAY_AGGRO_2, RUNED_GIANT_SAY_AGGRO_3), me);
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_GAVROCK_RUNEBREAKER)
			{
				if (!me->IsInCombatActive())
					return;

				if (Player* player = caster->ToPlayer())
				{
					if (player->GetQuestStatus(QUEST_FREE_AT_LAST) == QUEST_STATUS_COMPLETE)
						return;

					player->CastSpell(player, SPELL_NATURE_WEAKNESS, true);
					player->KilledMonsterCredit(NPC_FREED_GIANT_KILL_CREDIT, 0);
					me->SummonCreature(NPC_FREED_GIANT_KILL_CREDIT, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 30000);
					me->DespawnAfterAction();
				}
			}
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (RuneFistTimer <= diff)
			{
				DoCast(me, SPELL_RUNE_FIST);
				RuneFistTimer = urand(5000, 6000);
			}
			else RuneFistTimer -= diff;

			if (StompTimer <= diff)
            {
				DoCastAOE(SPELL_STOMP);
				StompTimer = urand(8000, 9000);
			}
			else StompTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_runed_giantAI(creature);
    }
};

enum QuestLatentPower
{
	SPELL_DRAW_POWER = 47542,
};

class npc_east_stone : public CreatureScript
{
public:
    npc_east_stone() : CreatureScript("npc_east_stone") {}

    struct npc_east_stoneAI : public QuantumBasicAI
    {
		npc_east_stoneAI(Creature* creature) : QuantumBasicAI(creature) {}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_DRAW_POWER)
            {
				if (Player* player = caster->ToPlayer())
					caster->ToPlayer()->KilledMonsterCredit(me->GetEntry(), 0);
			}
        }

		void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_east_stoneAI(creature);
    }
};

class npc_north_stone : public CreatureScript
{
public:
    npc_north_stone() : CreatureScript("npc_north_stone") {}

    struct npc_north_stoneAI : public QuantumBasicAI
    {
		npc_north_stoneAI(Creature* creature) : QuantumBasicAI(creature) {}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_DRAW_POWER)
            {
				if (Player* player = caster->ToPlayer())
					caster->ToPlayer()->KilledMonsterCredit(me->GetEntry(), 0);
			}
        }

		void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_north_stoneAI(creature);
    }
};

class npc_south_stone : public CreatureScript
{
public:
    npc_south_stone() : CreatureScript("npc_south_stone") {}

    struct npc_south_stoneAI : public QuantumBasicAI
    {
		npc_south_stoneAI(Creature* creature) : QuantumBasicAI(creature) {}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_DRAW_POWER)
            {
				if (Player* player = caster->ToPlayer())
					caster->ToPlayer()->KilledMonsterCredit(me->GetEntry(), 0);
			}
        }

		void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_south_stoneAI(creature);
    }
};

enum QuestRallyingTheTroops
{
	SPELL_TOSS_BOULDER        = 46815,
	SPELL_GIFT_OF_THE_GIANTS  = 47012,
	SPELL_KURUNS_BLESSING     = 47394,
	SPELL_SUMMON_THOR_MODAN   = 47410,

	QUEST_RALLYING_THE_TROOPS = 12070,

	NPC_BOULDER_TARGET        = 26264,
};

class npc_grizzly_hills_giant : public CreatureScript
{
public:
    npc_grizzly_hills_giant() : CreatureScript("npc_grizzly_hills_giant") {}

    struct npc_grizzly_hills_giantAI : public QuantumBasicAI
    {
		npc_grizzly_hills_giantAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 TossBoulderTimer;

		bool Blessed;

        void Reset()
        {
			TossBoulderTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_KURUNS_BLESSING)
			{
				if (Player* player = caster->ToPlayer())
				{
					if (player->GetQuestStatus(QUEST_RALLYING_THE_TROOPS) == QUEST_STATUS_COMPLETE)
						return;

					me->CastSpell(me, SPELL_SUMMON_THOR_MODAN);
				}
			}
		}

        void UpdateAI(const uint32 diff)
        {
			// Out of Combat Timer
			if (TossBoulderTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* target = me->FindCreatureWithDistance(NPC_BOULDER_TARGET, 150.0f))
				{
					me->SetFacingToObject(target);
					DoCast(target, SPELL_TOSS_BOULDER);
					TossBoulderTimer = urand(4*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else TossBoulderTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_grizzly_hills_giantAI(creature);
    }
};

enum QuestTheRuneOfCommand
{
	SPELL_RUNE_OF_COMMAND   = 49859,
	SPELL_GIANT_KILL_CREDIT = 43564,
	SPELL_SG_RUNE_FIST      = 49717,
	SPELL_SG_STONE_STOMP    = 49675,
	SPELL_SG_STONE_FIST     = 49676,
};

class npc_stone_giant : public CreatureScript
{
public:
    npc_stone_giant() : CreatureScript("npc_stone_giant") {}

    struct npc_stone_giantAI : public QuantumBasicAI
    {
		npc_stone_giantAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RuneFistTimer;
		uint32 StoneFistTimer;
		uint32 StoneStompTimer;

        void Reset()
        {
			RuneFistTimer = 0.5*IN_MILLISECONDS;
			StoneFistTimer = 1*IN_MILLISECONDS;
			StoneStompTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_RUNE_OF_COMMAND)
			{
				if (Player* player = caster->ToPlayer())
					player->CastSpell(player, SPELL_GIANT_KILL_CREDIT, true);
			}
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (RuneFistTimer <= diff)
			{
				DoCast(me, SPELL_SG_RUNE_FIST);
				RuneFistTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else RuneFistTimer -= diff;

			if (StoneStompTimer <= diff)
			{
				DoCastVictim(SPELL_SG_STONE_STOMP);
				StoneStompTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else StoneStompTimer -= diff;

			if (StoneFistTimer <= diff)
			{
				DoCast(me, SPELL_SG_STONE_FIST);
				StoneFistTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else StoneFistTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stone_giantAI(creature);
    }
};

enum QuestMarchOfTheGiants
{
	SPELL_DEACTIVATE_RUNES = 43666,
	SPELL_BIND_STONE_GIANT = 43270,
	SPELL_RSG_RUNE_FIST    = 49717,
	SPELL_RSG_STONE_STOMP  = 49675,
	SPELL_RSG_STONE_FIST   = 49676,
};

class npc_runed_stone_giant : public CreatureScript
{
public:
    npc_runed_stone_giant() : CreatureScript("npc_runed_stone_giant") {}

    struct npc_runed_stone_giantAI : public QuantumBasicAI
    {
		npc_runed_stone_giantAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BindStoneTimer;
		uint32 RuneFistTimer;
		uint32 StoneFistTimer;
		uint32 StoneStompTimer;

        void Reset()
        {
			BindStoneTimer = 0.2*IN_MILLISECONDS;
			RuneFistTimer = 0.5*IN_MILLISECONDS;
			StoneFistTimer = 1*IN_MILLISECONDS;
			StoneStompTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_DEACTIVATE_RUNES)
			{
				if (Player* player = caster->ToPlayer())
					me->DespawnAfterAction();
			}
		}

        void UpdateAI(const uint32 diff)
        {
			// Out of Combat Timer
			if (BindStoneTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_BIND_STONE_GIANT);
				BindStoneTimer = urand(28*IN_MILLISECONDS, 29*IN_MILLISECONDS);
			}
			else BindStoneTimer -= diff;

            if (!UpdateVictim())
                return;

			if (RuneFistTimer <= diff)
			{
				DoCast(me, SPELL_RSG_RUNE_FIST);
				RuneFistTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else RuneFistTimer -= diff;

			if (StoneStompTimer <= diff)
			{
				DoCastVictim(SPELL_RSG_STONE_STOMP);
				StoneStompTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else StoneStompTimer -= diff;

			if (StoneFistTimer <= diff)
			{
				DoCast(me, SPELL_RSG_STONE_FIST);
				StoneFistTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
			}
			else StoneFistTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_runed_stone_giantAI(creature);
    }
};

enum QuestBlackout
{
	SPELL_DESTROY_POWER_CORE = 47935,
	SPELL_COSMETIC_EXPLOSION = 60080,
};

class npc_dun_argol_power_core : public CreatureScript
{
public:
    npc_dun_argol_power_core() : CreatureScript("npc_dun_argol_power_core") {}

    struct npc_dun_argol_power_coreAI : public QuantumBasicAI
    {
		npc_dun_argol_power_coreAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_DESTROY_POWER_CORE)
			{
				if (Player* player = caster->ToPlayer())
				{
					player->KilledMonsterCredit(me->GetEntry(), 0);

					me->CastSpell(me, SPELL_COSMETIC_EXPLOSION);
					me->DespawnAfterAction(2*IN_MILLISECONDS);
				}
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
        return new npc_dun_argol_power_coreAI(creature);
    }
};

enum QuestCultivatingAnImage
{
	SPELL_CALL_LIGHTNING       = 32018,
	SPELL_LIGHTNING_GUN_SHOT   = 46982,
	SPELL_USE_CAMERA           = 48046,
	SPELL_SMELT_RUNE           = 52699,
	SPELL_LIGHTNING_CHARGED    = 52701,

	NPC_QUEST_CREDIT_IMAGE     = 26885,
};

class npc_iron_rune_smith : public CreatureScript
{
public:
	npc_iron_rune_smith() : CreatureScript("npc_iron_rune_smith") {}

    struct npc_iron_rune_smithAI : public QuantumBasicAI
    {
        npc_iron_rune_smithAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LightningChargedTimer;
		uint32 SmeltRuneTimer;

		void Reset()
        {
			LightningChargedTimer = 0.5*IN_MILLISECONDS;
			SmeltRuneTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_WORK_MINING);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_USE_CAMERA)
			{
				if (Player* player = caster->ToPlayer())
				{
					player->KilledMonsterCredit(NPC_QUEST_CREDIT_IMAGE, 0);

					me->DespawnAfterAction(1*IN_MILLISECONDS);
				}
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (LightningChargedTimer <= diff)
			{
				DoCast(me, SPELL_LIGHTNING_CHARGED);
				LightningChargedTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else LightningChargedTimer -= diff;

			if (SmeltRuneTimer <= diff)
			{
				DoCastVictim(SPELL_SMELT_RUNE);
				SmeltRuneTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else SmeltRuneTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_iron_rune_smithAI(creature);
    }
};

class npc_iron_rune_overseer : public CreatureScript
{
public:
	npc_iron_rune_overseer() : CreatureScript("npc_iron_rune_overseer") {}

    struct npc_iron_rune_overseerAI : public QuantumBasicAI
    {
        npc_iron_rune_overseerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CallLightningTimer;

		void Reset()
        {
			CallLightningTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_USE_CAMERA)
			{
				if (Player* player = caster->ToPlayer())
				{
					player->KilledMonsterCredit(NPC_QUEST_CREDIT_IMAGE, 0);

					me->DespawnAfterAction(1*IN_MILLISECONDS);
				}
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CallLightningTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CALL_LIGHTNING);
					CallLightningTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else CallLightningTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_iron_rune_overseerAI(creature);
    }
};

class npc_runic_lightning_gunner : public CreatureScript
{
public:
    npc_runic_lightning_gunner() : CreatureScript("npc_runic_lightning_gunner") {}

    struct npc_runic_lightning_gunnerAI : public QuantumBasicAI
    {
        npc_runic_lightning_gunnerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LightningGunShotTimer;

        void Reset()
        {
			LightningGunShotTimer = 0.2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_RANGED_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_RIFLE);
        }

		void AttackStart(Unit* who)
        {
			AttackStartNoMove(who);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_USE_CAMERA)
			{
				if (Player* player = caster->ToPlayer())
				{
					player->KilledMonsterCredit(NPC_QUEST_CREDIT_IMAGE, 0);

					me->DespawnAfterAction(1*IN_MILLISECONDS);
				}
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (LightningGunShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_LIGHTNING_GUN_SHOT);
					LightningGunShotTimer = 3*IN_MILLISECONDS;
				}
			}
			else LightningGunShotTimer -= diff;

			if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    DoStartMovement(me->GetVictim());

                DoMeleeAttackIfReady();
            }
			else
			{
				if (me->GetDistance2d(me->GetVictim()) > 10)
                    DoStartNoMovement(me->GetVictim());

                if (me->GetDistance2d(me->GetVictim()) > 30)
                    DoStartMovement(me->GetVictim());

				if (LightningGunShotTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, SPELL_LIGHTNING_GUN_SHOT);
						LightningGunShotTimer = 6*IN_MILLISECONDS;
					}
				}
				else LightningGunShotTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_runic_lightning_gunnerAI(creature);
    }
};

enum QuestTheThaneOfVoldrune
{
	SPELL_RIDE_FLAMEBRINGER   = 46598,
	SPELL_SUMMON_FLAMEBRINGER = 48606,
	SPELL_PARACHUTE           = 53208,
	SPELL_DRAKE_FLIGHT        = 55034,

	EMOTE_FLAMEBRINGER_OUT    = -1420112,

	AREA_VOLDRUNE             = 4207,

	GOSSIP_FLAMEBRINGER       = 9512,

	EVENT_CHECK_AREA          = 1,
};

Position const BringerHomePos = {2793.09f, -2506.13f, 47.7994f, 0.418879f};

class npc_flamebringer : public CreatureScript
{
public:
    npc_flamebringer() : CreatureScript("npc_flamebringer") { }

    struct npc_flamebringerAI : public VehicleAI
    {
        npc_flamebringerAI(Creature* creature) : VehicleAI(creature){}

		EventMap events;

		void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
		{
			if (menuId == GOSSIP_FLAMEBRINGER && gossipListId == 0)
			{
				player->RemoveMount();
				me->CastSpell(me, SPELL_SUMMON_FLAMEBRINGER, true);
				player->PlayerTalkClass->SendCloseGossip();
			}
		}

		void Reset()
		{
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustSummoned(Creature* summoned)
		{
			summoned->CastSpell(summoned, SPELL_RIDE_FLAMEBRINGER, true);

			if (Player* player = me->FindPlayerWithDistance(5.0f))
				player->EnterVehicle(summoned);
		}

		void OnCharmed(bool apply)
        {
			VehicleAI::OnCharmed(apply);
        }

		void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply)
        {
			if (passenger && apply)
			{
				me->SetCanFly(true);
				me->SetDisableGravity(true);
				me->SetUnitMovementFlags(MOVEMENTFLAG_FLYING);
				me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
				DoCast(me, SPELL_DRAKE_FLIGHT, true);

				me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

				events.ScheduleEvent(EVENT_CHECK_AREA, 0.5*IN_MILLISECONDS);
			}
			else
			{
				if (me->HasAura(SPELL_DRAKE_FLIGHT))
				{
					me->GetMotionMaster()->MovePoint(0, BringerHomePos);
					me->DespawnAfterAction(4*IN_MILLISECONDS);
				}
			}
        }

		void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
				    case EVENT_CHECK_AREA:
						if (me->GetAreaId() != AREA_VOLDRUNE)
						{
							if (Vehicle* vehicle = me->GetVehicleKit())
							{
								if (Unit* player = vehicle->GetPassenger(0))
								{
									if (player->GetTypeId() == TYPE_ID_PLAYER)
									{
										DoSendQuantumText(EMOTE_FLAMEBRINGER_OUT, me, player);
										player->ExitVehicle();
										player->CastSpell(player, SPELL_PARACHUTE, true);
										me->GetMotionMaster()->MovePoint(0, BringerHomePos);
										me->DespawnAfterAction(4*IN_MILLISECONDS);
									}
								}
							}
						}
					}
					events.ScheduleEvent(EVENT_CHECK_AREA, 0.5*IN_MILLISECONDS);
                    break;
			}
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_flamebringerAI(creature);
    }
};

enum Drakuru
{
	SPELL_HANDSHAKE_KILL_CREDIT = 50016,
	SPELL_BLOOD_OATH            = 50141,

	GOSSIP_DRAKURU              = 9615,
};

class npc_drakuru_gh : public CreatureScript
{
public:
    npc_drakuru_gh() : CreatureScript("npc_drakuru_gh") {}

    struct npc_drakuru_ghAI : public QuantumBasicAI
    {
        npc_drakuru_ghAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
		{
			if (menuId == GOSSIP_DRAKURU && gossipListId == 0)
			{
				player->CastSpell(player, SPELL_HANDSHAKE_KILL_CREDIT, true);
				player->RemoveAurasDueToSpell(SPELL_BLOOD_OATH);
				player->PlayerTalkClass->SendCloseGossip();
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
        return new npc_drakuru_ghAI(creature);
    }
};

enum VordrassilsEnergy
{
	SPELL_NATURE_SAMPLE       = 37846,
	SPELL_YOGG_SARON_PRESENCE = 49646,
	SPELL_SAMPLING_ENERGY     = 48218,
};

class npc_vordrassils_credit : public CreatureScript
{
public:
    npc_vordrassils_credit() : CreatureScript("npc_vordrassils_credit") {}

    struct npc_vordrassils_creditAI : public QuantumBasicAI
    {
		npc_vordrassils_creditAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_YOGG_SARON_PRESENCE);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_SAMPLING_ENERGY)
			{
				if (Player* player = caster->ToPlayer())
				{
					if (me->GetEntry())
					{
						player->KilledMonsterCredit(me->GetEntry(), 0);
						player->CastSpell(player, SPELL_NATURE_SAMPLE, true);
					}
				}
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
        return new npc_vordrassils_creditAI(creature);
    }
};

void AddSC_grizzly_hills()
{
    new npc_orsonn_and_kodian();
    new npc_emily();
    new npc_mr_floppy();
    new npc_outhouse_bunny();
    new npc_tallhorn_stag();
    new npc_amberpine_woodsman();
    new npc_wounded_skirmisher();
    new npc_lightning_sentry();
    new npc_venture_co_straggler();
    new npc_lake_frog();
	new npc_shredders_taker();
	new npc_brokendown_shredders();
	new npc_rocket_warhead();
	new npc_runed_giant();
	new npc_east_stone();
	new npc_north_stone();
	new npc_south_stone();
	new npc_grizzly_hills_giant();
	new npc_stone_giant();
	new npc_runed_stone_giant();
	new npc_dun_argol_power_core();
	new npc_iron_rune_smith();
	new npc_iron_rune_overseer();
	new npc_runic_lightning_gunner();
	new npc_flamebringer();
	new npc_drakuru_gh();
	new npc_vordrassils_credit();
}