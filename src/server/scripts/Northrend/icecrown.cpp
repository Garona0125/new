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
#include "SpellAuras.h"
#include "SpellScript.h"
#include "Player.h"
#include "TemporarySummon.h"
#include "CombatAI.h"
#include "Vehicle.h"
#include "CombatAI.h"

#define GOSSIP_ARETE_ITEM1 "Lord-Commander, I would hear your tale."
#define GOSSIP_ARETE_ITEM2 "<You nod slightly but do not complete the motion as the Lord-Commander narrows his eyes before he continues.>"
#define GOSSIP_ARETE_ITEM3 "I thought that they now called themselves the Scarlet Onslaught?"
#define GOSSIP_ARETE_ITEM4 "Where did the grand admiral go?"
#define GOSSIP_ARETE_ITEM5 "That's fine. When do I start?"
#define GOSSIP_ARETE_ITEM6 "Let's finish this!"
#define GOSSIP_ARETE_ITEM7 "That's quite a tale, Lord-Commander."

enum Arete
{
	GOSSIP_TEXTID_ARETE1        = 13525,
    GOSSIP_TEXTID_ARETE2        = 13526,
    GOSSIP_TEXTID_ARETE3        = 13527,
    GOSSIP_TEXTID_ARETE4        = 13528,
    GOSSIP_TEXTID_ARETE5        = 13529,
    GOSSIP_TEXTID_ARETE6        = 13530,
    GOSSIP_TEXTID_ARETE7        = 13531,

    QUEST_THE_STORY_THUS_FAR    = 12807,
};

class npc_arete : public CreatureScript
{
public:
    npc_arete() : CreatureScript("npc_arete") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_THE_STORY_THUS_FAR) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE1, creature->GetGUID());
            return true;
        }

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE2, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE3, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE4, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 4:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE5, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 5:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE6, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 6:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ARETE_ITEM7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_ARETE7, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 7:
                player->CLOSE_GOSSIP_MENU();
                player->AreaExploredOrEventHappens(QUEST_THE_STORY_THUS_FAR);
                break;
        }
        return true;
    }
};

enum DameEnvikiKapsalis
{
	TITLE_CRUSADER = 123,
};

class npc_dame_evniki_kapsalis : public CreatureScript
{
public:
    npc_dame_evniki_kapsalis() : CreatureScript("npc_dame_evniki_kapsalis") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->HasTitle(TITLE_CRUSADER))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_VENDOR_BROWSE_YOUR_GOODS), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
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

enum SquireDavid
{
    QUEST_THE_ASPIRANT_S_CHALLENGE_H = 13680,
    QUEST_THE_ASPIRANT_S_CHALLENGE_A = 13679,
    NPC_ARGENT_VALIANT               = 33448,
	SPELL_SQUIRE_MOUNT_VEHICLE       = 63034,
    SPELL_SUMMON_ARGENT_VALIANT      = 63028,
    GOSSIP_TEXTID_SQUIRE             = 14407,
};

#define GOSSIP_SQUIRE_ITEM_1 "I am ready to fight!"
#define GOSSIP_SQUIRE_ITEM_2 "How do the Argent Crusader raiders fight?"

class npc_squire_david : public CreatureScript
{
public:
    npc_squire_david() : CreatureScript("npc_squire_david") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->HasAura(SPELL_SQUIRE_MOUNT_VEHICLE))
		{
            if (player->GetQuestStatus(QUEST_THE_ASPIRANT_S_CHALLENGE_H) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_THE_ASPIRANT_S_CHALLENGE_A) == QUEST_STATUS_INCOMPLETE)
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SQUIRE_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SQUIRE_ITEM_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            }
		}

        player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_SQUIRE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->CastSpell(player, SPELL_SUMMON_ARGENT_VALIANT, false);
        }
        return true;
    }
};

enum ArgentValiant
{
    SPELL_THRUST                = 62544,
    SPELL_AV_CHARGE             = 63010,
    SPELL_AV_SHIELD_BREAKER     = 65147,
    SPELL_DEFEND                = 62719,
	SPELL_KILL_CREDIT           = 63049,
	SPELL_PLAYER_VEHICLE_MOUNT  = 63034,
    NPC_ARGENT_VALIANT_CREDIT   = 38595,
};

class npc_argent_valiant : public CreatureScript
{
public:
    npc_argent_valiant() : CreatureScript("npc_argent_valiant") { }

    struct npc_argent_valiantAI : public QuantumBasicAI
    {
        npc_argent_valiantAI(Creature* creature) : QuantumBasicAI(creature)
        {
            creature->GetMotionMaster()->MovePoint(0, 8599.258f, 963.951f, 547.553f);
            creature->SetCurrentFaction(35); //wrong faction in db?
        }

        uint32 ChargeTimer;
        uint32 ShieldBreakerTimer;

        void Reset()
        {
            ChargeTimer = 7000;
            ShieldBreakerTimer = 10000;
        }

        void MovementInform(uint32 type, uint32 /*id*/)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
            me->SetCurrentFaction(16);
        }

        void DamageTaken(Unit* doneBy, uint32& damage)
        {
            if (damage > me->GetHealth() && doneBy->GetTypeId() == TYPE_ID_PLAYER)
            {
                damage = 0;
                if (doneBy->HasAura(SPELL_PLAYER_VEHICLE_MOUNT))
                    doneBy->CastSpell(doneBy, SPELL_KILL_CREDIT, true);
                me->SetCurrentFaction(35);
                me->DespawnAfterAction(5*IN_MILLISECONDS);
                me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                EnterEvadeMode();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (ChargeTimer <= diff)
            {
                DoCastVictim(SPELL_AV_CHARGE);
                ChargeTimer = 7000;
            }
			else
				ChargeTimer -= diff;

            if (ShieldBreakerTimer <= diff)
            {
                DoCastVictim(SPELL_AV_SHIELD_BREAKER);
                ShieldBreakerTimer = 10000;
            }
			else 
				ShieldBreakerTimer -= diff;

            if (me->IsAttackReady())
            {
                DoCastVictim(SPELL_THRUST, true);
                me->ResetAttackTimer();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_argent_valiantAI(creature);
    }
};

enum Misc
{
    SPELL_CHAIN          = 68341,
    NPC_FJOLA_LIGHTBANE  = 36065,
    NPC_EYDIS_DARKBANE   = 36066,
    NPC_PRIESTESS_ALORAH = 36101,
    NPC_PRIEST_GRIMMIN   = 36102
};

class npc_alorah_and_grimmin : public CreatureScript
{
public:
    npc_alorah_and_grimmin() : CreatureScript("npc_alorah_and_grimmin") { }

    struct npc_alorah_and_grimminAI : public QuantumBasicAI
    {
		npc_alorah_and_grimminAI(Creature* creature) : QuantumBasicAI(creature) {}

        bool cast;

        void Reset()
        {
            cast = false;
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (cast)
                return;
            cast = true;
            Creature* target = 0;

            switch (me->GetEntry())
            {
                case NPC_PRIESTESS_ALORAH:
                    target = me->FindCreatureWithDistance(NPC_EYDIS_DARKBANE, 10.0f);
                    break;
                case NPC_PRIEST_GRIMMIN:
                    target = me->FindCreatureWithDistance(NPC_FJOLA_LIGHTBANE, 10.0f);
                    break;
            }
            if (target)
                DoCast(target, SPELL_CHAIN);

            if (!UpdateVictim())
                return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_alorah_and_grimminAI(creature);
    }
};

/*######
## npc_guardian_pavilion
######*/

enum GuardianPavilion
{
	SPELL_TRESPASSER_A            = 63986,
    SPELL_TRESPASSER_H            = 63987,
    AREA_SUNREAVER_PAVILION       = 4676,
    AREA_SILVER_COVENANT_PAVILION = 4677,
};

class npc_guardian_pavilion : public CreatureScript
{
public:
    npc_guardian_pavilion() : CreatureScript("npc_guardian_pavilion") { }

    struct npc_guardian_pavilionAI : public QuantumBasicAI
    {
        npc_guardian_pavilionAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        void MoveInLineOfSight(Unit* who)
        {
            if (me->GetAreaId() != AREA_SUNREAVER_PAVILION && me->GetAreaId() != AREA_SILVER_COVENANT_PAVILION)
                return;

            if (!who || who->GetTypeId() != TYPE_ID_PLAYER || !me->IsHostileTo(who) || !me->isInBackInMap(who, 5.0f))
                return;

            if (who->HasAura(SPELL_TRESPASSER_H) || who->HasAura(SPELL_TRESPASSER_A))
                return;

            if (who->ToPlayer()->GetTeamId() == TEAM_ALLIANCE)
                who->CastSpell(who, SPELL_TRESPASSER_H, true);
            else
                who->CastSpell(who, SPELL_TRESPASSER_A, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_guardian_pavilionAI(creature);
    }
};

enum VerethTheCunning
{
    NPC_GEIST_RETURN_BUNNY_KC   = 31049,
    NPC_LITHE_STALKER           = 30894,
    SPELL_SUBDUED_LITHE_STALKER = 58151,
};

class npc_vereth_the_cunning : public CreatureScript
{
public:
    npc_vereth_the_cunning() : CreatureScript("npc_vereth_the_cunning") { }

    struct npc_vereth_the_cunningAI : public QuantumBasicAI
    {
        npc_vereth_the_cunningAI(Creature* creature) : QuantumBasicAI(creature) {}

        void MoveInLineOfSight(Unit* who)
        {
            QuantumBasicAI::MoveInLineOfSight(who);

            if (who->GetEntry() == NPC_LITHE_STALKER && me->IsWithinDistInMap(who, 10.0f))
            {
                if (Unit* owner = who->GetCharmer())
                {
                    if (who->HasAura(SPELL_SUBDUED_LITHE_STALKER))
					{
						owner->ToPlayer()->KilledMonsterCredit(NPC_GEIST_RETURN_BUNNY_KC, 0);
						who->ToCreature()->DisappearAndDie();
                    }
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vereth_the_cunningAI(creature);
    }
};

enum ArgentCannonSpells
{
    SPELL_ARGENT_CANNON_SHOOT         = 57385,
    SPELL_ARGENT_CANNON_SHOOT_TRIGGER = 57387,
    SPELL_RECONING_BOMB               = 57412,
    SPELL_RECONING_BOMB_TRIGGER       = 57414
};

class spell_argent_cannon : public SpellScriptLoader
{
    public:
        spell_argent_cannon() : SpellScriptLoader("spell_argent_cannon") { }

        class spell_argent_cannon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_argent_cannon_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ARGENT_CANNON_SHOOT_TRIGGER))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_RECONING_BOMB_TRIGGER))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex effIndex)
            {
                const WorldLocation* loc = GetExplTargetDest();

                switch (GetSpellInfo()->Id)
                {
                case SPELL_ARGENT_CANNON_SHOOT:
                    GetCaster()->CastSpell(loc->m_positionX, loc->m_positionY, loc->m_positionZ, SPELL_ARGENT_CANNON_SHOOT_TRIGGER, true);
                    break;
                case SPELL_RECONING_BOMB:
                    GetCaster()->CastSpell(loc->m_positionX, loc->m_positionY, loc->m_positionZ, SPELL_RECONING_BOMB_TRIGGER, true);
                    break;
                }

                PreventHitDefaultEffect(effIndex);
            }

            void Register()
            {
                //OnEffectHitTarget += SpellEffectFn(spell_argent_cannon_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnEffectHitTarget += SpellEffectFn(spell_argent_cannon_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_argent_cannon_SpellScript();
        }
};

// Battle for Crusaders' Pinnacle
enum BlessedBanner
{
    SPELL_BLESSING_OF_THE_CRUSADE       = 58026,
    SPELL_THREAT_PULSE                  = 58113,
    SPELL_CRUSADERS_SPIRE_VICTORY       = 58084,
    SPELL_TORCH                         = 58121,

    NPC_BLESSED_BANNER                  = 30891,
    NPC_CRUSADER_LORD_DALFORS           = 31003,
    NPC_ARGENT_BATTLE_PRIEST            = 30919,
    NPC_ARGENT_MASON                    = 30900,
    NPC_REANIMATED_CAPTAIN              = 30986,
    NPC_SCOURGE_DRUDGE                  = 30984,
    NPC_HIDEOUS_PLAGEBRINGER            = 30987,
    NPC_HALOF_THE_DEATHBRINGER          = 30989,
    NPC_LK                              = 31013,

    BANNER_SAY                          = 0, // "The Blessed Banner of the Crusade has been planted.\n Defend the banner from all attackers!"
    DALFORS_SAY_PRE_1                   = 0, // "BY THE LIGHT! Those damned monsters! Look at what they've done to our people!"
    DALFORS_SAY_PRE_2                   = 1, // "Burn it down, boys. Burn it all down."
    DALFORS_SAY_START                   = 2, // "Let 'em come. They'll pay for what they've done!"
    DALFORS_YELL_FINISHED               = 3, // "We've done it, lads! We've taken the pinnacle from the Scourge! Report to Father Gustav at once and tell him the good news! We're gonna get to buildin' and settin' up! Go!"

    LK_TALK_1                           = 0, // "Leave no survivors!"
    LK_TALK_2                           = 1, // "Cower before my terrible creations!"
    LK_TALK_3                           = 2, // "Feast my children! Feast upon the flesh of the living!"
    LK_TALK_4                           = 3, // "Lay down your arms and surrender your souls!"

    EVENT_SPAWN                         = 1,
    EVENT_INTRO_1                       = 2,
    EVENT_INTRO_2                       = 3,
    EVENT_INTRO_3                       = 4,
    EVENT_MASON_ACTION                  = 5,
    EVENT_START_FIGHT                   = 6,
    EVENT_WAVE_SPAWN                    = 7,
    EVENT_HALOF                         = 8,
    EVENT_ENDED                         = 9,
};

Position const DalforsPos[3] =
{
    {6458.703f, 403.858f, 490.498f, 3.1205f}, // Dalfors spawn point
    {6422.950f, 423.335f, 510.451f, 0.0f}, // Dalfors intro pos
    {6426.343f, 420.515f, 508.650f, 0.0f}, // Dalfors fight pos
};

Position const Priest1Pos[2] =
{
    {6462.025f, 403.681f, 489.721f, 3.1007f}, // priest1 spawn point
    {6421.480f, 423.576f, 510.781f, 5.7421f}, // priest1 intro pos
};

Position const Priest2Pos[2] =
{
    {6463.969f, 407.198f, 489.240f, 2.2689f}, // priest2 spawn point
    {6419.778f, 421.404f, 510.972f, 5.7421f}, // priest2 intro pos
};

Position const Priest3Pos[2] =
{
    {6464.371f, 400.944f, 489.186f, 6.1610f}, // priest3 spawn point
    {6423.516f, 425.782f, 510.774f, 5.7421f}, // priest3 intro pos
};

Position const Mason1Pos[3] =
{
    {6462.929f, 409.826f, 489.392f, 3.0968f}, // mason1 spawn point
    {6428.163f, 421.960f, 508.297f, 0.0f}, // mason1 intro pos
    {6414.335f, 454.904f, 511.395f, 2.8972f}, // mason1 action pos
};

Position const Mason2Pos[3] =
{
    {6462.650f, 405.670f, 489.576f, 2.9414f}, // mason2 spawn point
    {6426.250f, 419.194f, 508.219f, 0.0f}, // mason2 intro pos
    {6415.014f, 446.849f, 511.395f, 3.1241f}, // mason2 action pos
};

Position const Mason3Pos[3] =
{
    {6462.646f, 401.218f, 489.601f, 2.7864f}, // mason3 spawn point
    {6423.855f, 416.598f, 508.305f, 0.0f}, // mason3 intro pos
    {6417.070f, 438.824f, 511.395f, 3.6651f}, // mason3 action pos
};

class npc_blessed_banner : public CreatureScript
{
public:
    npc_blessed_banner() : CreatureScript("npc_blessed_banner") { }

    struct npc_blessed_bannerAI : public QuantumBasicAI
    {
        npc_blessed_bannerAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
            HalofSpawned = false;
            PhaseCount = 0;

            SetCombatMovement(false);
        }

        EventMap events;

        bool HalofSpawned;

        uint32 PhaseCount;

        SummonList Summons;

        uint64 guidDalfors;
        uint64 guidPriest[3];
        uint64 guidMason[3];
        uint64 guidHalof;

        void Reset()
        {
            me->SetRegeneratingHealth(false);
            DoCast(SPELL_THREAT_PULSE);
            Talk(BANNER_SAY);
            events.ScheduleEvent(EVENT_SPAWN, 3000);
        }

        void EnterToBattle(Unit* /*who*/){}

        void MoveInLineOfSight(Unit* /*who*/){}


        void JustSummoned(Creature* Summoned)
        {
            Summons.Summon(Summoned);
        }

        void JustDied(Unit* /*killer*/)
        {
            Summons.DespawnAll();
            me->DespawnAfterAction();
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

            switch (events.ExecuteEvent())
            {
                case EVENT_SPAWN:
                    {
                        if (Creature* Dalfors = DoSummon(NPC_CRUSADER_LORD_DALFORS, DalforsPos[0]))
                        {
                            guidDalfors = Dalfors->GetGUID();
                            Dalfors->GetMotionMaster()->MovePoint(0, DalforsPos[1]);
                        }
                        if (Creature* Priest1 = DoSummon(NPC_ARGENT_BATTLE_PRIEST, Priest1Pos[0]))
                        {
                            guidPriest[0] = Priest1->GetGUID();
                            Priest1->GetMotionMaster()->MovePoint(0, Priest1Pos[1]);
                        }
                        if (Creature* Priest2 = DoSummon(NPC_ARGENT_BATTLE_PRIEST, Priest2Pos[0]))
                        {
                            guidPriest[1] = Priest2->GetGUID();
                            Priest2->GetMotionMaster()->MovePoint(0, Priest2Pos[1]);
                        }
                        if (Creature* Priest3 = DoSummon(NPC_ARGENT_BATTLE_PRIEST, Priest3Pos[0]))
                        {
                            guidPriest[2] = Priest3->GetGUID();
                            Priest3->GetMotionMaster()->MovePoint(0, Priest3Pos[1]);
                        }
                        if (Creature* Mason1 = DoSummon(NPC_ARGENT_MASON, Mason1Pos[0]))
                        {
                            guidMason[0] = Mason1->GetGUID();
                            Mason1->GetMotionMaster()->MovePoint(0, Mason1Pos[1]);
                        }
                        if (Creature* Mason2 = DoSummon(NPC_ARGENT_MASON, Mason2Pos[0]))
                        {
                            guidMason[1] = Mason2->GetGUID();
                            Mason2->GetMotionMaster()->MovePoint(0, Mason2Pos[1]);
                        }
                        if (Creature* Mason3 = DoSummon(NPC_ARGENT_MASON, Mason3Pos[0]))
                        {
                            guidMason[2] = Mason3->GetGUID();
                            Mason3->GetMotionMaster()->MovePoint(0, Mason3Pos[1]);
                        }
                        events.ScheduleEvent(EVENT_INTRO_1, 15000);
                    }
                    break;
                case EVENT_INTRO_1:
                    {
                        if (Creature* dalfors = ObjectAccessor::GetCreature(*me, guidDalfors))
                            dalfors->AI()->Talk(DALFORS_SAY_PRE_1);
                        events.ScheduleEvent(EVENT_INTRO_2, 5000);
                    }
                    break;
                case EVENT_INTRO_2:
                    {
                        if (Creature* dalfors = ObjectAccessor::GetCreature(*me, guidDalfors))
                        {
                            dalfors->SetFacingTo(6.215f);
                            dalfors->AI()->Talk(DALFORS_SAY_PRE_2);
                        }
						events.ScheduleEvent(EVENT_INTRO_3, 5000);
                    }
                    break;
                case EVENT_INTRO_3:
                    {
                        if (Creature* dalfors = ObjectAccessor::GetCreature(*me, guidDalfors))
                        {
                            dalfors->GetMotionMaster()->MovePoint(0, DalforsPos[2]);
                            dalfors->SetHomePosition(DalforsPos[2]);
                        }
                        if (Creature* Priest1 = ObjectAccessor::GetCreature(*me, guidPriest[0]))
                        {
                            Priest1->SetFacingTo(5.7421f);
                            Priest1->SetHomePosition(Priest1Pos[1]);
                        }
                        if (Creature* Priest2 = ObjectAccessor::GetCreature(*me, guidPriest[1]))
                        {
                            Priest2->SetFacingTo(5.7421f);
                            Priest2->SetHomePosition(Priest2Pos[1]);
                        }
                        if (Creature* Priest3 = ObjectAccessor::GetCreature(*me, guidPriest[2]))
                        {
                            Priest3->SetFacingTo(5.7421f);
                            Priest3->SetHomePosition(Priest3Pos[1]);
                        }
                        if (Creature* Mason1 = ObjectAccessor::GetCreature(*me, guidMason[0]))
                        {
                            Mason1->GetMotionMaster()->MovePoint(0, Mason1Pos[2]);
                            Mason1->SetHomePosition(Mason1Pos[2]);
                        }
                        if (Creature* Mason2 = ObjectAccessor::GetCreature(*me, guidMason[1]))
                        {
                            Mason2->GetMotionMaster()->MovePoint(0, Mason2Pos[2]);
                            Mason2->SetHomePosition(Mason2Pos[2]);
                        }
                        if (Creature* Mason3 = ObjectAccessor::GetCreature(*me, guidMason[2]))
                        {
                            Mason3->GetMotionMaster()->MovePoint(0, Mason3Pos[2]);
                            Mason3->SetHomePosition(Mason3Pos[2]);
                        }
                        events.ScheduleEvent(EVENT_START_FIGHT, 5000);
                        events.ScheduleEvent(EVENT_MASON_ACTION, 15000);
                    }
                    break;
                case EVENT_MASON_ACTION:
                    {
                        if (Creature* Mason1 = ObjectAccessor::GetCreature(*me, guidMason[0]))
                        {
                            Mason1->SetFacingTo(2.8972f);
                            Mason1->AI()->SetData(1, 1); // triggers SAI actions on npc
                        }
                        if (Creature* Mason2 = ObjectAccessor::GetCreature(*me, guidMason[1]))
                        {
                            Mason2->SetFacingTo(3.1241f);
                            Mason2->AI()->SetData(1, 1); // triggers SAI actions on npc
                        }
                        if (Creature* Mason3 = ObjectAccessor::GetCreature(*me, guidMason[2]))
                        {
                            Mason3->SetFacingTo(3.6651f);
                            Mason3->AI()->SetData(1, 1); // triggers SAI actions on npc
                        }
                    }
                    break;
                case EVENT_START_FIGHT:
                    {
                        if (Creature* LK = GetClosestCreatureWithEntry(me, NPC_LK, 100))
                            LK->AI()->Talk(LK_TALK_1);
                        if (Creature* Dalfors = ObjectAccessor::GetCreature(*me, guidDalfors))
                            Dalfors->AI()->Talk(DALFORS_SAY_START);
                        events.ScheduleEvent(EVENT_WAVE_SPAWN, 1000);
                    }
                    break;
                case EVENT_WAVE_SPAWN:
                    {
                        if (PhaseCount == 3)
                        {
                            if (Creature* LK = GetClosestCreatureWithEntry(me, NPC_LK, 100))
                                LK->AI()->Talk(LK_TALK_2);
                        }
                        else if (PhaseCount == 6)
                        {
                            if (Creature* LK = GetClosestCreatureWithEntry(me, NPC_LK, 100))
                                LK->AI()->Talk(LK_TALK_3);
                        }
                        if (Creature* tempsum = DoSummon(NPC_SCOURGE_DRUDGE, Mason3Pos[0]))
                            {
                                tempsum->SetHomePosition(DalforsPos[2]);
                                tempsum->AI()->AttackStart(GetClosestCreatureWithEntry(me, NPC_BLESSED_BANNER, 100));
                            }
                        if (urand(0, 1) == 0)
                        {
                            if (Creature* tempsum = DoSummon(NPC_HIDEOUS_PLAGEBRINGER, Mason1Pos[0]))
                            {
                                tempsum->SetHomePosition(DalforsPos[2]);
                                tempsum->AI()->AttackStart(GetClosestCreatureWithEntry(me, NPC_BLESSED_BANNER, 100));
                            }
                            if (Creature* tempsum = DoSummon(NPC_HIDEOUS_PLAGEBRINGER, Mason2Pos[0]))
                            {
                                tempsum->SetHomePosition(DalforsPos[2]);
                                tempsum->AI()->AttackStart(GetClosestCreatureWithEntry(me, NPC_BLESSED_BANNER, 100));
                            }
                        }
                        else
                        {
                            if (Creature* tempsum = DoSummon(NPC_REANIMATED_CAPTAIN, Mason1Pos[0]))
                            {
                                tempsum->SetHomePosition(DalforsPos[2]);
                                tempsum->AI()->AttackStart(GetClosestCreatureWithEntry(me, NPC_BLESSED_BANNER, 100));
                            }
                            if (Creature* tempsum = DoSummon(NPC_REANIMATED_CAPTAIN, Mason2Pos[0]))
                            {
                                tempsum->SetHomePosition(DalforsPos[2]);
                                tempsum->AI()->AttackStart(GetClosestCreatureWithEntry(me, NPC_BLESSED_BANNER, 100));
                            }
                        }

                        PhaseCount++;

                        if (PhaseCount < 8)
                            events.ScheduleEvent(EVENT_WAVE_SPAWN, urand(10000, 20000));
                        else
                            events.ScheduleEvent(EVENT_HALOF, urand(10000, 20000));
                    }
                    break;
                case EVENT_HALOF:
                    {
                        if (Creature* LK = GetClosestCreatureWithEntry(me, NPC_LK, 100))
                            LK->AI()->Talk(LK_TALK_4);
                        if (Creature* tempsum = DoSummon(NPC_SCOURGE_DRUDGE, Mason1Pos[0]))
                        {
                            tempsum->SetHomePosition(DalforsPos[2]);
                            tempsum->AI()->AttackStart(GetClosestCreatureWithEntry(me, NPC_BLESSED_BANNER, 100));
                        }
                        if (Creature* tempsum = DoSummon(NPC_SCOURGE_DRUDGE, Mason2Pos[0]))
                        {
                            tempsum->SetHomePosition(DalforsPos[2]);
                            tempsum->AI()->AttackStart(GetClosestCreatureWithEntry(me, NPC_BLESSED_BANNER, 100));
                        }
                        if (Creature* tempsum = DoSummon(NPC_HALOF_THE_DEATHBRINGER, DalforsPos[0]))
                        {
                            HalofSpawned = true;
                            guidHalof = tempsum->GetGUID();
                            tempsum->SetHomePosition(DalforsPos[2]);
                            tempsum->AI()->AttackStart(GetClosestCreatureWithEntry(me, NPC_BLESSED_BANNER, 100));
                        }
                    }
                    break;
                case EVENT_ENDED:
                    {
                        Summons.DespawnAll();
                        me->DespawnAfterAction();
                    }
                    break;
            }

            if (PhaseCount == 8)
                if (Creature* Halof = ObjectAccessor::GetCreature(*me, guidHalof))
                    if (Halof->IsDead())
                    {
                        DoCast(me, SPELL_CRUSADERS_SPIRE_VICTORY, true);
                        Summons.DespawnEntry(NPC_HIDEOUS_PLAGEBRINGER);
                        Summons.DespawnEntry(NPC_REANIMATED_CAPTAIN);
                        Summons.DespawnEntry(NPC_SCOURGE_DRUDGE);
                        Summons.DespawnEntry(NPC_HALOF_THE_DEATHBRINGER);
                        if (Creature* Dalfors = ObjectAccessor::GetCreature(*me, guidDalfors))
                            Dalfors->AI()->Talk(DALFORS_YELL_FINISHED);
                        events.ScheduleEvent(EVENT_ENDED, 10000);
                    }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_blessed_bannerAI(creature);
    }
};

enum CapturedCrusader
{
    SPELL_NERUBIAN_WEBS          = 56726,
    SPELL_GRAB_CAPTURED_CRUSADER = 56683,
    SPELL_RIDE_VEHICLE           = 56687,
};

class npc_captured_crusader : public CreatureScript
{
    public:
        npc_captured_crusader() : CreatureScript("npc_captured_crusader") { }

        struct npc_captured_crusaderAI : public QuantumBasicAI
        {
            npc_captured_crusaderAI(Creature* creature) : QuantumBasicAI(creature) { }

			bool Vehicled;

            uint32 VehicleTimer;

            void Reset()
            {
                Vehicled = false;

                VehicleTimer = 5000;

                DoCast(me, SPELL_NERUBIAN_WEBS, true);
            }

            void SpellHit(Unit* caster, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_GRAB_CAPTURED_CRUSADER)
                {
                    Vehicled = true;

                    DoCast(caster, SPELL_RIDE_VEHICLE, true);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!Vehicled || Vehicled && me->GetVehicle())
                    return;

                if (VehicleTimer <= diff)
                {
                    Vehicled = false;
                    me->DespawnAfterAction();
                }
                else VehicleTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_captured_crusaderAI(creature);
        }
};

enum TheOcularSpells
{
    SPELL_THE_OCULAR_TRANSFORM = 55162,
    SPELL_DEATHLY_STARE        = 55269,
    SPELL_OCULAR_ON_DEATH      = 55288,
    SPELL_OCULAR_KILL_CREDIT   = 55289,
	SPELL_EYESORE_BLASTER      = 29142,
};

enum ReqCreatures
{
   NPC_THE_OCULAR        = 29747,
   NPC_KILL_CREDIT_BUNNY = 29803,
};

class npc_the_ocular : public CreatureScript
{
public:
    npc_the_ocular() : CreatureScript("npc_the_ocular") { }

    struct npc_the_ocularAI : public QuantumBasicAI
    {
        npc_the_ocularAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        uint32 DeathlyStareTimer;

        void Reset()
        {
            DeathlyStareTimer = 1000;
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_EYESORE_BLASTER)
			{
				me->AI()->EnterToBattle(caster);
				me->AI()->AttackStart(caster);
			}
		}

        void JustDied (Unit* killer)
        {
            if (killer && killer->ToPlayer())
                killer->ToPlayer()->CastSpell(killer, SPELL_OCULAR_KILL_CREDIT, true);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!me->HasAura(SPELL_THE_OCULAR_TRANSFORM))
                DoCast(me, SPELL_THE_OCULAR_TRANSFORM, true);

            if (!UpdateVictim())
                return;

            if (DeathlyStareTimer <= diff)
            {
                DoCastVictim(SPELL_DEATHLY_STARE);
                DeathlyStareTimer = 6000;
            }
            else DeathlyStareTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_the_ocularAI(creature);
    }
};

/*########
## npc_general_lightsbane
#########*/

enum GeneralLightsbaneSpells
{
    SPELL_CLEAVE          = 15284,
    SPELL_DEATH_AND_DECAY = 60160,
    SPELL_PLAGUE_STRIKE   = 60186,
};

enum Npcs
{
    NPC_VILE            = 29860,
    NPC_THE_LEAPER      = 29859,
    NPC_LADY_NIGHTSWOOD = 29858,
};

class npc_general_lightsbane : public CreatureScript
{
public:
    npc_general_lightsbane() : CreatureScript("npc_general_lightsbane") { }

    struct npc_general_lightsbaneAI : public QuantumBasicAI
    {
        npc_general_lightsbaneAI(Creature* creature) : QuantumBasicAI(creature) { }

        uint32 CleaveTimer;
        uint32 DeathDecayTimer;
        uint32 PlagueStrikeTimer;
        uint32 SummonSupportTimer;
        bool SupportSummoned;

        void Reset()
        {
            CleaveTimer = urand(2000,3000);
            DeathDecayTimer = urand(15000,20000);
            PlagueStrikeTimer = urand(5000,10000);

            std::list<Creature*> TargetList;
            me->GetCreatureListWithEntryInGrid(TargetList, me->GetEntry(), 100.0f);

            if (TargetList.size() > 1)
                me->DespawnAfterAction(1*IN_MILLISECONDS);

            SummonSupportTimer = 5000;
            SupportSummoned = false;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (!SupportSummoned)
                if (SummonSupportTimer <= diff)
                {
                    Creature* creature = DoSummon(NPC_VILE, me, 5, 20000,TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT);
                    creature->AI()->AttackStart(me);

                    creature = DoSummon(NPC_THE_LEAPER, me, 5, 20000, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT);
                    creature->AI()->AttackStart(me);

                    creature = DoSummon(NPC_LADY_NIGHTSWOOD, me, 5, 20000, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT);
                    creature->AI()->AttackStart(me);

                    SummonSupportTimer = (urand(4000, 5000));
                    SupportSummoned = true;
                }
                else
					SummonSupportTimer -= diff;

            if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = (urand(4000,5000));
            }
            else
				CleaveTimer -= diff;

            if (DeathDecayTimer <= diff)
            {
                DoCastVictim(SPELL_DEATH_AND_DECAY);
                DeathDecayTimer = urand(15000,20000);
            }
            else
				DeathDecayTimer -= diff;

            if (PlagueStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_PLAGUE_STRIKE);
                PlagueStrikeTimer = urand(5000,10000);
            }
            else
				PlagueStrikeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_general_lightsbaneAI(creature);
    }
};

/*########
## npc_free_your_mind_vile
#########*/

enum FreeYourMindNPCSpells
{
    SPELL_SOVEREIGN_ROD             = 29070,
    SPELL_SOVEREIGN_ROD_TRIGGERED   = 29071,
    // Vile Abilities
    SPELL_VILE_ENRAGE               = 56646,    // <50% HP ?
    SPELL_VILE_BACKHAND             =  6253,
    // Lady Nightswood Abilities
    SPELL_BANSHEE_CURSE             = 5884,
    SPELL_BANSHEE_SHRIEK            = 16838,
    // The Leaper Abilities
    SPELL_LEAPER_SNISTER_STRIKE     = 60195,
    SPELL_LEAPER_HUNGER_FOR_BLOOD   = 60177,
};

enum FreeYourMindNPCEntrys
{
    NPC_FYM_VILE          = 29769,
    NPC_FYM_LADY          = 29770,
    NPC_FYM_LEAPER        = 29840
};

#define SAY_VILE_AGGRO            "Crush... maim... DESTROY!"
#define SAY_VILE_FREED            "Vile free? Vile love %n"
#define SAY_LADY_NIGHTSWOOD_AGGRO "Who intrudes upon my ritual?"
#define SAY_LADY_NIGHTSWOOD_FREED " You dare? Where is Baron Sliver? I would have words with him!"
#define SAY_THE_LEAPER_AGGRO      "Mrrfrmrfrmrrrrr!"
#define SAY_THE_LEAPER_FREED      "Mrmrmmrmrmrmrm... mrmrmrmr?!"

class npc_free_your_mind : public CreatureScript
{
public:
    npc_free_your_mind() : CreatureScript("npc_free_your_mind") { }

    struct npc_free_your_mindAI : public QuantumBasicAI
    {
        npc_free_your_mindAI(Creature* creature) : QuantumBasicAI(creature) { }

        bool Enraged;

        uint32 Spell1EntryTimer;
        uint32 Spell2EntryTimer;
        uint32 Spell1Entry;
        uint32 Spell2Entry;

        void Reset()
        {
            switch (me->GetEntry())
            {
            case NPC_FYM_VILE:
                Spell1Entry = SPELL_VILE_BACKHAND;
                Spell1EntryTimer = urand(4000,6000);
                Spell2Entry = SPELL_VILE_ENRAGE;
                break;
            case NPC_FYM_LADY:
                Spell1Entry = SPELL_BANSHEE_CURSE;
                Spell1EntryTimer = urand(5000,6000);
                Spell2Entry = SPELL_BANSHEE_SHRIEK;
                Spell2EntryTimer = urand(10000,12000);
                break;
            case NPC_FYM_LEAPER:
                Spell1Entry = SPELL_LEAPER_SNISTER_STRIKE;
                Spell1EntryTimer = urand(4000,6000);
                Spell2Entry = SPELL_LEAPER_HUNGER_FOR_BLOOD;
                break;
            }
            me->RestoreFaction();
        }

        void EnterToBattle(Unit* who)
        {
            Enraged = false;

            switch (me->GetEntry())
            {
            case NPC_FYM_VILE:
                me->MonsterSay(SAY_VILE_AGGRO, LANG_UNIVERSAL, who->GetGUID());
                break;
            case NPC_FYM_LEAPER:
                me->MonsterSay(SAY_THE_LEAPER_AGGRO, LANG_UNIVERSAL,who->GetGUID());
                break;
            case NPC_FYM_LADY:
                me->MonsterSay(SAY_LADY_NIGHTSWOOD_AGGRO, LANG_UNIVERSAL, who->GetGUID());
                break;
            }
        }

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_SOVEREIGN_ROD_TRIGGERED)
            {
                if (caster && caster->ToPlayer())
                {
                    me->setDeathState(ALIVE);
                    me->SetCurrentFaction(35);
                    me->DespawnAfterAction(4*IN_MILLISECONDS);

                    switch (me->GetEntry())
                    {
                    case NPC_FYM_VILE:
                        me->MonsterSay(SAY_VILE_FREED,LANG_UNIVERSAL, caster->GetGUID());
                        caster->ToPlayer()->KilledMonsterCredit(29845, 0);
                        break;
                    case NPC_FYM_LEAPER:
                        me->MonsterSay(SAY_THE_LEAPER_FREED,LANG_UNIVERSAL, caster->GetGUID());
                        caster->ToPlayer()->KilledMonsterCredit(29847, 0);
                        break;
                    case NPC_FYM_LADY:
                        me->MonsterSay(SAY_LADY_NIGHTSWOOD_FREED,LANG_UNIVERSAL, caster->GetGUID());
                        caster->ToPlayer()->KilledMonsterCredit(29846, 0);
                        break;
                    }
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (Spell1EntryTimer <= diff)
            {
                DoCastVictim(Spell1Entry);
                switch (me->GetEntry())
                {
                case NPC_FYM_VILE:
                case NPC_FYM_LEAPER:
                    Spell1EntryTimer = (urand(7000,9000));
                    break;
                case NPC_FYM_LADY:
                     Spell1EntryTimer = (urand(10000,15000));
                     break;
                }
            }
            else
				Spell1EntryTimer -= diff;

            if (me->GetEntry() == NPC_FYM_VILE)
            {
                if (!Enraged && HealthBelowPct(30))
                {
                    DoCast(me, Spell2Entry, true);
                    Enraged = true;
                }
            }
            else
            {
                if (Spell2EntryTimer <= diff)
                {
                    DoCastVictim(Spell2Entry);
                    Spell2EntryTimer = (urand(8000,10000));
                }
                else
					Spell2EntryTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_free_your_mindAI(creature);
    }
};

/*########
## Saronite Mine Slave
#########*/

enum EntrysSlaveToSaronite
{
    QUEST_SLAVES_TO_SARONITE_ALLIANCE = 13300,
    QUEST_SLAVES_TO_SARONITE_HORDE    = 13302,
    NPC_SLAVE_QUEST_CREDIT            = 31866,
    SPELL_SLAVE_ENRAGE                = 8599,
    SPELL_HEAD_CRACK                  = 3148,
    ACTION_ENRAGED                    = 0,
    ACTION_INSANE                     = 1,
    ACTION_FREED                      = 2,
};

const Position FreedPos[2] =
{
    { 7030.0f,  1862.0f, 533.2f, 0.0f },
    { 6947.0f,  2027.0f, 519.7f, 0.0f }
};

#define GOSSIP_OPTION_FREE  "Go on, you're free. Get out of here!"

#define SAY_SLAVE_AGGRO_1 "AHAHAHAHA... you'll join us soon enough!"
#define SAY_SLAVE_AGGRO_2 "I don't want to leave! I want to stay here!"
#define SAY_SLAVE_AGGRO_3 "I won't leave!"
#define SAY_SLAVE_AGGRO_4 "NO! You're wrong! The voices in my head are beautiful!"

#define SAY_SLAVE_INSANE_1 "I must get further underground to where he is. I must jump!"
#define SAY_SLAVE_INSANE_2 "I'll never return. The whole reason for my existence awaits below!"
#define SAY_SLAVE_INSANE_3 "I'm coming, master!"
#define SAY_SLAVE_INSANE_4 "My life for you!"

class npc_saronite_mine_slave : public CreatureScript
{
public:
    npc_saronite_mine_slave() : CreatureScript("npc_saronite_mine_slave") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if ((player->GetQuestStatus(QUEST_SLAVES_TO_SARONITE_HORDE) == QUEST_STATUS_INCOMPLETE) || (player->GetQuestStatus(QUEST_SLAVES_TO_SARONITE_ALLIANCE) == QUEST_STATUS_INCOMPLETE))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_OPTION_FREE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        player->CLOSE_GOSSIP_MENU();

        if (action == (GOSSIP_ACTION_INFO_DEF + 1))
        {
            creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
            if (urand(0, 1) == 0)
            {
                creature->AI()->DoAction(ACTION_FREED);
                player->KilledMonsterCredit(NPC_SLAVE_QUEST_CREDIT , 0);
            }
			else
            {
                if (urand(0, 1) == 0)
                {
                    creature->AI()->DoAction(ACTION_ENRAGED);
                    creature->SetCurrentFaction(16);
                    creature->CastSpell(creature, SPELL_SLAVE_ENRAGE);
                    creature->AI()->AttackStart(player);
                }
				else
					creature->AI()->DoAction(ACTION_INSANE);
            }
        }
        return true;
    }

    struct npc_saronite_mine_slaveAI : public QuantumBasicAI
    {
        npc_saronite_mine_slaveAI(Creature* creature) : QuantumBasicAI(creature)
        {
			alreadyFreed = false;
            enraged = false;
        }

        bool enraged;
        bool alreadyFreed;

        uint32 HeadCrackTimer;

        void DoAction(const int32 action)
        {
            switch (action)
            {
            case ACTION_ENRAGED:
                enraged = true;
                alreadyFreed = true;
                switch (urand(0, 3))
                {
				    case 0:
						me->MonsterYell(SAY_SLAVE_AGGRO_1, LANG_UNIVERSAL, 0);
						break;
					case 1:
						me->MonsterYell(SAY_SLAVE_AGGRO_2, LANG_UNIVERSAL, 0);
						break;
					case 2:
						me->MonsterYell(SAY_SLAVE_AGGRO_3, LANG_UNIVERSAL, 0);
						break;
					case 3:
						me->MonsterYell(SAY_SLAVE_AGGRO_4, LANG_UNIVERSAL, 0);
						break;
				}
                break;
            case ACTION_FREED:
                alreadyFreed = true;
                me->SetWalk(false);
                me->GetMotionMaster()->MovePoint(0,FreedPos[0]);
                me->DespawnAfterAction(15*IN_MILLISECONDS);
                break;
            case ACTION_INSANE:
                alreadyFreed = true;
                switch (urand(0, 3))
                {
                    case 0:
						me->MonsterYell(SAY_SLAVE_INSANE_1, LANG_UNIVERSAL, 0);
						break;
					case 1:
						me->MonsterYell(SAY_SLAVE_INSANE_2, LANG_UNIVERSAL, 0);
						break;
					case 2:
						me->MonsterYell(SAY_SLAVE_INSANE_3, LANG_UNIVERSAL, 0);
						break;
					case 3:
						me->MonsterYell(SAY_SLAVE_INSANE_4, LANG_UNIVERSAL, 0);
						break;
                }
                me->SetWalk(false);
                me->GetMotionMaster()->MovePoint(0,FreedPos[1]);
                me->DespawnAfterAction(15*IN_MILLISECONDS);
                break;
            }
        }

        void MoveInLineOfSight(Unit* mover)
        {
            if (!enraged)
                return;

            QuantumBasicAI::MoveInLineOfSight(mover);
        }

        void AttackStart(Unit* attacker)
        {
            if (!enraged)
                return;

            QuantumBasicAI::AttackStart(attacker);
        }

        void Reset()
        {
            me->RestoreFaction();
			
			if (alreadyFreed)
            {
                alreadyFreed = false;
                me->DespawnAfterAction(10*IN_MILLISECONDS);
            }
			else
            {
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                alreadyFreed = false;
                enraged = false;
            }
        }

        void EnterToBattle(Unit* /*who*/)
        {
            HeadCrackTimer = urand(5000, 7000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (HeadCrackTimer <= diff)
            {
                DoCastVictim(SPELL_HEAD_CRACK);
                HeadCrackTimer = (urand(7000, 9000));
            }
            else HeadCrackTimer -= diff;
			
			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_saronite_mine_slaveAI(creature);
    }
};

/*######
## npc_vendor_tournament_fraction_champion
######*/

enum AchievementsTournamentFactionChampion
{
	ACHIEVEMENT_CHAMPION_OF_DARNASSUS       = 2777,
    ACHIEVEMENT_CHAMPION_OF_EXODAR          = 2778,
    ACHIEVEMENT_CHAMPION_OF_GNOMEREGAN      = 2779,
    ACHIEVEMENT_CHAMPION_OF_IRONFORGE       = 2780,
    ACHIEVEMENT_CHAMPION_OF_STORMWIND       = 2781,
    ACHIEVEMENT_CHAMPION_OF_ORGRIMMAR       = 2783,
    ACHIEVEMENT_CHAMPION_OF_DARKSPEARS      = 2784,
    ACHIEVEMENT_CHAMPION_OF_SILVERMOON      = 2785,
    ACHIEVEMENT_CHAMPION_OF_THUNDERBLUFF    = 2786,
    ACHIEVEMENT_CHAMPION_OF_UNDERCITY       = 2787,
};

enum NPCVendorEntrys
{
    NPC_EXODAR_VENDOR                     = 33657,
    NPC_GNOMEREGAN_VENDOR                 = 33650,
    NPC_DARNASSUS_VENDOR                  = 33653,
    NPC_STORMWIND_VENDOR                  = 33307,
    NPC_IRONFORGE_VENDOR                  = 33310,

    NPC_ORGRIMMAR_VENDOR                  = 33553,
    NPC_DARKSPEARS_VENDOR                 = 33554,
    NPC_SILVERMOON_VENDOR                 = 33557,
    NPC_THUNDERBLUFF_VENDOR               = 33556,
    NPC_UNDERCITY_VENDOR                  = 33555,

};

class npc_vendor_tournament_fraction_champion : public CreatureScript
{
public:
    npc_vendor_tournament_fraction_champion() : CreatureScript("npc_vendor_tournament_fraction_champion") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        bool showVendor = false;

        switch (creature->GetEntry())
        {
        case NPC_EXODAR_VENDOR:
            showVendor = player->HasAchieved(ACHIEVEMENT_CHAMPION_OF_EXODAR);
            break;
        case NPC_GNOMEREGAN_VENDOR:
            showVendor = player->HasAchieved(ACHIEVEMENT_CHAMPION_OF_GNOMEREGAN);
            break;
        case NPC_DARNASSUS_VENDOR:
            showVendor = player->HasAchieved(ACHIEVEMENT_CHAMPION_OF_DARNASSUS);
            break;
        case NPC_STORMWIND_VENDOR:
            showVendor = player->HasAchieved(ACHIEVEMENT_CHAMPION_OF_STORMWIND);
            break;
        case NPC_IRONFORGE_VENDOR:
            showVendor = player->HasAchieved(ACHIEVEMENT_CHAMPION_OF_IRONFORGE);
            break;
        case NPC_ORGRIMMAR_VENDOR:
            showVendor = player->HasAchieved(ACHIEVEMENT_CHAMPION_OF_ORGRIMMAR);
            break;
        case NPC_DARKSPEARS_VENDOR:
            showVendor = player->HasAchieved(ACHIEVEMENT_CHAMPION_OF_DARKSPEARS);
            break;
        case NPC_SILVERMOON_VENDOR:
            showVendor = player->HasAchieved(ACHIEVEMENT_CHAMPION_OF_SILVERMOON);
            break;
        case NPC_THUNDERBLUFF_VENDOR:
            showVendor = player->HasAchieved(ACHIEVEMENT_CHAMPION_OF_THUNDERBLUFF);
            break;
        case NPC_UNDERCITY_VENDOR:
            showVendor = player->HasAchieved(ACHIEVEMENT_CHAMPION_OF_UNDERCITY);
            break;
        }

        if (showVendor)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_VENDOR_BROWSE_YOUR_GOODS), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        }
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

enum FactionValiantChampion
{
	SPELL_FVC_CHARGE            = 63010,
	SPELL_FVC_SHIELD_BREAKER    = 65147,
    SPELL_REFRESH_MOUNT         = 66483,
	SPELL_DEFEND_AURA_PERIODIC  = 64223,
    SPELL_GIVE_VALIANT_MARK_1   = 62724,
    SPELL_GIVE_VALIANT_MARK_2   = 62770,
    SPELL_GIVE_VALIANT_MARK_3   = 62771,
    SPELL_GIVE_VALIANT_MARK_4   = 62995,
    SPELL_GIVE_VALIANT_MARK_5   = 62996,
    SPELL_GIVE_CHAMPION_MARK    = 63596,

    QUEST_THE_GRAND_MELEE_0     = 13665,
    QUEST_THE_GRAND_MELEE_1     = 13745,
    QUEST_THE_GRAND_MELEE_2     = 13750,
    QUEST_THE_GRAND_MELEE_3     = 13756,
    QUEST_THE_GRAND_MELEE_4     = 13761,
    QUEST_THE_GRAND_MELEE_5     = 13767,
    QUEST_THE_GRAND_MELEE_6     = 13772,
    QUEST_THE_GRAND_MELEE_7     = 13777,
    QUEST_THE_GRAND_MELEE_8     = 13782,
    QUEST_THE_GRAND_MELEE_9     = 13787,

    QUEST_AMONG_THE_CHAMPIONS_0 = 13790,
    QUEST_AMONG_THE_CHAMPIONS_1 = 13793,
    QUEST_AMONG_THE_CHAMPIONS_2 = 13811,
    QUEST_AMONG_THE_CHAMPIONS_3 = 13814,

    SPELL_BESTED_DARNASSUS      = 64805,
    SPELL_BESTED_GNOMEREGAN     = 64809,
    SPELL_BESTED_IRONFORGE      = 64810,
    SPELL_BESTED_ORGRIMMAR      = 64811,
    SPELL_BESTED_SENJIN         = 64812,
    SPELL_BESTED_SILVERMOON     = 64813,
    SPELL_BESTED_STORMWIND      = 64814,
    SPELL_BESTED_EXODAR         = 64808,
    SPELL_BESTED_UNDERCITY      = 64816,
    SPELL_BESTED_THUNDERBLUFF   = 64815,
};

#define GOSSIP_MELEE_FIGHT "I'am ready to fight!"

class npc_faction_valiant_champion : public CreatureScript
{
public:
    npc_faction_valiant_champion() : CreatureScript("npc_faction_valiant_champion") { }

    struct npc_faction_valiant_championAI : public QuantumBasicAI
    {
        npc_faction_valiant_championAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ChargeTimer;
        uint32 ShieldBreakerTimer;

        uint64 GuidAttacker;

        bool Chargeing;

        void Reset()
        {
            ChargeTimer = 5000;
            ShieldBreakerTimer = 9000;

            me->SetCurrentFaction(35);
        }

        void EnterToBattle(Unit* who)
        {
            GuidAttacker = who->GetGUID();

            DoCast(me, SPELL_DEFEND_AURA_PERIODIC, true);

            if (Aura* aura = me->AddAura(SPELL_DEFEND, me))
                aura->ModStackAmount(1);

			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id != 1)
                return;

            Chargeing = false;

            DoCastVictim(SPELL_FVC_CHARGE);

            if (me->GetVictim())
                me->GetMotionMaster()->MoveChase(me->GetVictim());
        }

        void DamageTaken(Unit* doneBy, uint32& damage)
        {
            if (doneBy && doneBy->GetGUID() != GuidAttacker)
                damage = 0;

            if (damage > me->GetHealth() && doneBy->GetTypeId() == TYPE_ID_PLAYER)
            {
                damage = 0;

                if (doneBy->HasAura(63034))
                {
					switch (me->GetEntry())
					{
                        case 33559: // Darnassus
						case 33562: // Exodar
						case 33558: // Gnomeregan
						case 33564: // Ironforge
						case 33306: // Orgrimmar
						case 33285: // Sen'jin
						case 33382: // Silvermoon
						case 33561: // Stormwind
						case 33383: // Thunder Bluff
						case 33384: // Undercity
						{
							doneBy->CastSpell(doneBy, SPELL_GIVE_VALIANT_MARK_1, true);
							break;
						}
						case 33738: // Darnassus
						case 33739: // Exodar
						case 33740: // Gnomeregan
						case 33743: // Ironforge
						case 33744: // Orgrimmar
						case 33745: // Sen'jin
						case 33746: // Silvermoon
						case 33747: // Stormwind
						case 33748: // Thunder Bluff
						case 33749: // Undercity
						{
							doneBy->CastSpell(doneBy, SPELL_GIVE_CHAMPION_MARK, true);
							break;
						}
					}

                    switch (me->GetEntry())
                    {
                        case 33738: // Darnassus
                            doneBy->CastSpell(doneBy, SPELL_BESTED_DARNASSUS, true);
							break;
                        case 33739: // Exodar
                            doneBy->CastSpell(doneBy, SPELL_BESTED_EXODAR, true);
							break;
                        case 33740: // Gnomeregan
                            doneBy->CastSpell(doneBy, SPELL_BESTED_GNOMEREGAN, true);
							break;
                        case 33743: // Ironforge
                            doneBy->CastSpell(doneBy, SPELL_BESTED_IRONFORGE, true);
							break;
                        case 33744: // Orgrimmar
                            doneBy->CastSpell(doneBy, SPELL_BESTED_ORGRIMMAR, true);
							break;
                        case 33745: // Sen'jin
                            doneBy->CastSpell(doneBy, SPELL_BESTED_SENJIN, true);
							break;
                        case 33746: // Silvermoon
                            doneBy->CastSpell(doneBy, SPELL_BESTED_SILVERMOON, true);
							break;
                        case 33747: // Stormwind
                            doneBy->CastSpell(doneBy, SPELL_BESTED_STORMWIND, true);
							break;
                        case 33748: // Thunder Bluff
                            doneBy->CastSpell(doneBy, SPELL_BESTED_THUNDERBLUFF, true);
							break;
                        case 33749: // Undercity
                            doneBy->CastSpell(doneBy, SPELL_BESTED_UNDERCITY, true);
							break;
                    }
                }

                me->SetCurrentFaction(35);
                EnterEvadeMode();
                me->CastSpell(me, SPELL_REFRESH_MOUNT, true);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (ChargeTimer <= diff)
            {
                Chargeing = true;
                float x, y, z;
                me->GetNearPoint(me, x, y, z, 1.0f, 15.0f, float(M_PI*2*rand_norm()));
                me->GetMotionMaster()->MovePoint(1, x, y, z);
                ChargeTimer = 15000;
            }
			else ChargeTimer -= diff;

            if (ShieldBreakerTimer <= diff)
            {
                DoCastVictim(SPELL_FVC_SHIELD_BREAKER);
                ShieldBreakerTimer = 10000;
            }
			else ShieldBreakerTimer -= diff;

            if (me->IsAttackReady())
            {
                DoCastVictim(SPELL_THRUST, true);
                me->ResetAttackTimer();
            }

            if (Player* player = Player::GetPlayer(*me, GuidAttacker))
			{
				if (!player->HasAura(63034))
					EnterEvadeMode();
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_faction_valiant_championAI(creature);
    }

    bool CanMakeDuel(Player* player, uint32 npcEntry)
    {
        switch (npcEntry)
        {
        case 33738: // Darnassus
            return !player->HasAura(SPELL_BESTED_DARNASSUS);
        case 33739: // Exodar
            return !player->HasAura(SPELL_BESTED_EXODAR);
        case 33740: // Gnomeregan
            return !player->HasAura(SPELL_BESTED_GNOMEREGAN);
        case 33743: // Ironforge
            return !player->HasAura(SPELL_BESTED_IRONFORGE);
        case 33744: // Orgrimmar
            return !player->HasAura(SPELL_BESTED_ORGRIMMAR);
        case 33745: // Sen'jin
            return !player->HasAura(SPELL_BESTED_SENJIN);
        case 33746: // Silvermoon
            return !player->HasAura(SPELL_BESTED_SILVERMOON);
        case 33747: // Stormwind
            return !player->HasAura(SPELL_BESTED_STORMWIND);
        case 33748: // Thunder Bluff
            return !player->HasAura(SPELL_BESTED_THUNDERBLUFF);
        case 33749: // Undercity
            return !player->HasAura(SPELL_BESTED_UNDERCITY);
        }
        return true;
    }

    void AddMeleeFightGossip(Player* player)
    {
        if (!player)
            return;

        if (player->HasAura(63034)
			&& player->GetQuestStatus(QUEST_THE_GRAND_MELEE_0) == QUEST_STATUS_INCOMPLETE
			|| player->GetQuestStatus(QUEST_THE_GRAND_MELEE_1) == QUEST_STATUS_INCOMPLETE
			|| player->GetQuestStatus(QUEST_THE_GRAND_MELEE_2) == QUEST_STATUS_INCOMPLETE
			|| player->GetQuestStatus(QUEST_THE_GRAND_MELEE_3) == QUEST_STATUS_INCOMPLETE
			|| player->GetQuestStatus(QUEST_THE_GRAND_MELEE_4) == QUEST_STATUS_INCOMPLETE
			|| player->GetQuestStatus(QUEST_THE_GRAND_MELEE_5) == QUEST_STATUS_INCOMPLETE
			|| player->GetQuestStatus(QUEST_THE_GRAND_MELEE_6) == QUEST_STATUS_INCOMPLETE
			|| player->GetQuestStatus(QUEST_THE_GRAND_MELEE_7) == QUEST_STATUS_INCOMPLETE
			|| player->GetQuestStatus(QUEST_THE_GRAND_MELEE_8) == QUEST_STATUS_INCOMPLETE
			|| player->GetQuestStatus(QUEST_THE_GRAND_MELEE_9) == QUEST_STATUS_INCOMPLETE)
		{
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_MELEE_FIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        }
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        switch (creature->GetEntry())
        {
            case 33559: // Darnassus
			case 33562: // Exodar
			case 33558: // Gnomeregan
			case 33564: // Ironforge
			case 33561: // Stormwind
			{
				if (player->GetTeamId() == TEAM_ALLIANCE)
					AddMeleeFightGossip(player);
				break;
			}
			case 33306: // Orgrimmar
			case 33285: // Sen'jin
			case 33382: // Silvermoon
			case 33383: // Thunder Bluff
			case 33384: // Undercity
			{
				if (player->GetTeamId() == TEAM_HORDE)
					AddMeleeFightGossip(player);
				break;
			}
			case 33738: // Darnassus
			case 33739: // Exodar
			case 33740: // Gnomeregan
			case 33743: // Ironforge
			case 33744: // Orgrimmar
			case 33745: // Sen'jin
			case 33746: // Silvermoon
			case 33747: // Stormwind
			case 33748: // Thunder Bluff
			case 33749: // Undercity
			{
				if (player->HasAura(63034)
					&& player->GetQuestStatus(QUEST_AMONG_THE_CHAMPIONS_0) == QUEST_STATUS_INCOMPLETE
					|| player->GetQuestStatus(QUEST_AMONG_THE_CHAMPIONS_1) == QUEST_STATUS_INCOMPLETE
					|| player->GetQuestStatus(QUEST_AMONG_THE_CHAMPIONS_2) == QUEST_STATUS_INCOMPLETE
					|| player->GetQuestStatus(QUEST_AMONG_THE_CHAMPIONS_3) == QUEST_STATUS_INCOMPLETE)
				{
					if (CanMakeDuel(player, creature->GetEntry()))
						player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_MELEE_FIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
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
        player->CLOSE_GOSSIP_MENU();
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            creature->SetCurrentFaction(16);
            creature->AI()->AttackStart(player);
            return true;
        }

        if (action == GOSSIP_ACTION_INFO_DEF + 2)
        {
            creature->SetCurrentFaction(16);
            creature->AI()->AttackStart(player);
            return true;
        }
        return true;
    }
};

enum SquireDanny
{
    QUEST_THE_VALIANT_S_CHALLENGE_HORDE_UNDERCITY     = 13729,
    QUEST_THE_VALIANT_S_CHALLENGE_HORDE_SENJIN        = 13727,
    QUEST_THE_VALIANT_S_CHALLENGE_HORDE_THUNDERBLUFF  = 13728,
    QUEST_THE_VALIANT_S_CHALLENGE_HORDE_SILVERMOON    = 13731,
    QUEST_THE_VALIANT_S_CHALLENGE_HORDE_ORGRIMMAR     = 13726,
    QUEST_THE_VALIANT_S_CHALLENGE_ALLIANCE_DARNASSUS  = 13725,
    QUEST_THE_VALIANT_S_CHALLENGE_ALLIANCE_IRONFORGE  = 13713,
    QUEST_THE_VALIANT_S_CHALLENGE_ALLIANCE_GNOMEREGAN = 13723,
    QUEST_THE_VALIANT_S_CHALLENGE_ALLIANCE_EXODAR     = 13724,
    QUEST_THE_VALIANT_S_CHALLENGE_ALLIANCE_STORMWIND  = 13699,

    NPC_ARGENT_CHAMPION          = 33707,
    SPELL_SUMMON_ARGENT_CHAMPION = 63171,
	SPELL_S_RIDE_VEHICLE         = 63034,

    GOSSIP_TEXTID_SQUIRE_DANNY   = 14407,
};

class npc_squire_danny : public CreatureScript
{
public:
    npc_squire_danny() : CreatureScript("npc_squire_danny") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->CastSpell(player, SPELL_SUMMON_ARGENT_CHAMPION, false);
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->HasAura(SPELL_S_RIDE_VEHICLE)
            && player->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_HORDE_UNDERCITY) == QUEST_STATUS_INCOMPLETE
            || player->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_HORDE_SENJIN) == QUEST_STATUS_INCOMPLETE
            || player->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_HORDE_THUNDERBLUFF) == QUEST_STATUS_INCOMPLETE
            || player->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_HORDE_SILVERMOON) == QUEST_STATUS_INCOMPLETE
            || player->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_HORDE_ORGRIMMAR) == QUEST_STATUS_INCOMPLETE
            || player->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_ALLIANCE_DARNASSUS) == QUEST_STATUS_INCOMPLETE
            || player->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_ALLIANCE_IRONFORGE) == QUEST_STATUS_INCOMPLETE
            || player->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_ALLIANCE_GNOMEREGAN) == QUEST_STATUS_INCOMPLETE
            || player->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_ALLIANCE_EXODAR) == QUEST_STATUS_INCOMPLETE
            || player->GetQuestStatus(QUEST_THE_VALIANT_S_CHALLENGE_ALLIANCE_STORMWIND) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SQUIRE_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SQUIRE_ITEM_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        }
		
		player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_SQUIRE_DANNY, creature->GetGUID());
		return true;
	}
};

enum SquireCalvin
{
    QUEST_THE_BLACK_KNIGHTS_FALL = 13664,
    NPC_BLACK_KNIGHT             = 33785,
	SPELL_SC_RIDE_VEHICLE_AURA   = 63151,
};

#define GOSSIP_SUMMON_BLACK_KNIGHT "Ask Cavin to summon the Black Knight."

class npc_squire_cavin : public CreatureScript
{
public:
    npc_squire_cavin() : CreatureScript("npc_squire_cavin") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            Position pos;

            creature->GetPosition(&pos);
            {
				if (TempSummon* temp = creature->SummonCreature(NPC_BLACK_KNIGHT, pos, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000))
                    temp->AI()->SetGUID(player->GetGUID());
            }
            player->CLOSE_GOSSIP_MENU();
        }
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->HasAura(SPELL_SC_RIDE_VEHICLE_AURA) && (player->GetQuestStatus(QUEST_THE_BLACK_KNIGHTS_FALL) == QUEST_STATUS_INCOMPLETE))
        {
            std::list<Creature*> checkList;
            creature->GetCreatureListWithEntryInGrid(checkList, NPC_BLACK_KNIGHT, 100.0f);

            if (checkList.size() == 0)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SUMMON_BLACK_KNIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        }

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }
};

enum BlackKnight
{
    SPELL_BK_CHARGE         = 63003,
	SPELL_BK_SHIELD_BREAKER = 65147,
    SPELL_DARK_SHIELD       = 64505,
};

#define YELL_ATTACK_PHASE_1_END "Get off that horse and fight me man-to-man!"
#define YELL_ATTACK_PHASE_2     "I will not fail you, master!"

class npc_the_black_knight : public CreatureScript
{
public:
    npc_the_black_knight() : CreatureScript("npc_the_black_knight") { }

    struct npc_the_black_knightAI : public QuantumBasicAI
    {
        npc_the_black_knightAI(Creature* creature) : QuantumBasicAI(creature) { }

        uint32 ChargeTimer;
        uint32 ShieldBreakerTimer;
        uint64 GuidAttacker;
        uint32 ReattackTimer;

        bool Chargeing;
        bool MountDuel;
        bool HandDuel;

        void Reset()
        {
            ChargeTimer = 7000;
            ShieldBreakerTimer = 10000;
            ReattackTimer = 10000;
			me->SetCurrentFaction(35);
            MountDuel = false;
            HandDuel = false;
        }

        void SetGUID(uint64 guid, int32)
        {
            if (Player* player = Player::GetPlayer(*me, guid))
            {
				GuidAttacker = guid;
                MountDuel = true;
                HandDuel = false;
                me->Mount(28652);
				me->SetCurrentFaction(16);
                AttackStart(player);
                // Move Point
                me->SetMaxHealth(50000);
                me->SetHealth(50000);
            }
        }

        void JustDied(Unit* killer)
        {
            me->DespawnAfterAction(5*IN_MILLISECONDS);
        }

        void EnterToBattle(Unit* /*who*/)
        {
			DoCast(me, SPELL_DEFEND_AURA_PERIODIC, true);

			if (Aura* aura = me->AddAura(SPELL_DEFEND, me))
				aura->ModStackAmount(1);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == 1)
            {
				Chargeing = false;

                DoCastVictim(SPELL_BK_CHARGE);

                if (me->GetVictim())
					me->GetMotionMaster()->MoveChase(me->GetVictim());
            }
			else if (id == 2)
            {
                if (Player* player = Player::GetPlayer(*me, GuidAttacker))
                    AttackStart(player);
            }
        }

        void DamageTaken(Unit* doneBy, uint32& damage)
        {
            if (doneBy && doneBy->GetGUID() != GuidAttacker)
                damage = 0;

            if (HandDuel)
                return;

            if (!MountDuel)
                return;

            if (damage > me->GetHealth() && doneBy->GetTypeId() == TYPE_ID_PLAYER)
            {
                damage = 0;
                MountDuel = false;
                me->SetHealth(50000);
                me->RemoveMount();
                me->GetMotionMaster()->MoveIdle();
                me->RemoveAurasDueToSpell(SPELL_DEFEND_AURA_PERIODIC);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->MonsterYell(YELL_ATTACK_PHASE_1_END, LANG_UNIVERSAL, GuidAttacker);
                ReattackTimer = 10000;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (MountDuel)
            {
                if (ChargeTimer <= diff)
                {
                    Chargeing = true;
                    float x,y,z;
                    me->GetNearPoint(me, x, y, z, 1.0f, 15.0f, float(M_PI*2*rand_norm()));
                    me->GetMotionMaster()->MovePoint(1, x, y, z);

                    ChargeTimer = 7000;
                }
				else ChargeTimer -= diff;

                if (ShieldBreakerTimer <= diff)
                {
                    DoCastVictim(SPELL_BK_SHIELD_BREAKER);
                    ShieldBreakerTimer = 10000;
                }
				else ShieldBreakerTimer -= diff;

                if (me->IsAttackReady())
                {
                    DoCastVictim(SPELL_THRUST, true);
                    me->ResetAttackTimer();
                }
            }
			else if (HandDuel)
            {
                if (ShieldBreakerTimer <= diff)
                {
                    DoCastVictim(SPELL_DARK_SHIELD);
                    ShieldBreakerTimer = 30000;
                }
				else ShieldBreakerTimer -= diff;

                DoMeleeAttackIfReady();
            }
			else
            {
                if (ReattackTimer <= diff)
                {
                    HandDuel = true;
                    if (me->GetVictim())
						me->GetMotionMaster()->MoveChase(me->GetVictim());
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                    if (Player* player = Player::GetPlayer(*me, GuidAttacker))
						player->ExitVehicle();

                    me->SetMaxHealth(12500);
                    me->SetHealth(12500);
                    me->MonsterYell(YELL_ATTACK_PHASE_2, LANG_UNIVERSAL, GuidAttacker);
                    ReattackTimer = 99999999;
                }
				else ReattackTimer -= diff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_the_black_knightAI(creature);
    }
};

enum ArgentChampion
{
	SPELL_AG_CHARGE                 = 63010,
	SPELL_AG_SHIELD_BREAKER         = 65147,
	SPELL_PLAYER_TOURNAMENT_MOUNT   = 63034,
    SPELL_ARGENT_CRUSADE_CHAMPION   = 63501,
    SPELL_GIVE_KILL_CREDIT_CHAMPION = 63516,
};

class npc_argent_champion : public CreatureScript
{
public:
    npc_argent_champion() : CreatureScript("npc_argent_champion") { }

    struct npc_argent_championAI : public QuantumBasicAI
    {
        npc_argent_championAI(Creature* creature) : QuantumBasicAI(creature)
        {
            creature->GetMotionMaster()->MovePoint(0, 8561.30f, 1113.30f, 556.9f);
            creature->SetCurrentFaction(35); //wrong faction in db?
        }

        uint32 ChargeTimer;
        uint32 ShieldBreakerTimer;

        void Reset()
        {
            ChargeTimer = 7000;
            ShieldBreakerTimer = 10000;
        }

        void MovementInform(uint32 type, uint32 /*id*/)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
            me->SetCurrentFaction(16);
        }

        void DamageTaken(Unit* doneBy, uint32& damage)
        {
            if (damage > me->GetHealth() && doneBy->GetTypeId() == TYPE_ID_PLAYER)
            {
                damage = 0;
                if (doneBy->HasAura(SPELL_PLAYER_TOURNAMENT_MOUNT))
                    doneBy->CastSpell(doneBy, SPELL_GIVE_KILL_CREDIT_CHAMPION, true);

                me->SetCurrentFaction(35);
                me->DespawnAfterAction(5*IN_MILLISECONDS);
                me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                EnterEvadeMode();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (ChargeTimer <= diff)
            {
                DoCastVictim(SPELL_AG_CHARGE);
                ChargeTimer = 7000;
            }
			else ChargeTimer -= diff;

            if (ShieldBreakerTimer <= diff)
            {
                DoCastVictim(SPELL_AG_SHIELD_BREAKER);
                ShieldBreakerTimer = 10000;
            }
			else ShieldBreakerTimer -= diff;

            if (me->IsAttackReady())
            {
                DoCastVictim(SPELL_THRUST, true);
                me->ResetAttackTimer();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_argent_championAI(creature);
    }
};

#define GOSSIP_TRADE       "Visit a trader."
#define GOSSIP_BANK        "Visit a bank."
#define GOSSIP_MAIL        "Visit a mailbox."

#define GOSSIP_ORC_PENNANT "Orgrimmar Champion's Pennant"
#define GOSSIP_SEN_PENNANT "Darkspear Champion's Pennant"
#define GOSSIP_UND_PENNANT "Forsaken Champion's Pennant"
#define GOSSIP_SIL_PENNANT "Silvermoon Champion's Pennant"
#define GOSSIP_TBL_PENNANT "Thunder Bluff Champion's Pennant"

#define GOSSIP_STW_PENNANT "Stormwind Champion's Pennant"
#define GOSSIP_IFR_PENNANT "Ironforge Champion's Pennant"
#define GOSSIP_GNO_PENNANT "Gnomeregan Champion's Pennant"
#define GOSSIP_DAR_PENNANT "Darnassus Champion's Pennant"
#define GOSSIP_EXO_PENNANT "Exodar Champion's Pennant"

enum SquireGruntling
{
    SPELL_BANK_ERRAND_H             = 68849,
    SPELL_POSTMAN_H                 = 68850,
    SPELL_SHOP_H                    = 68851,
    SPELL_TIRED_H                   = 68852,

    SPELL_BANK_ERRAND_A             = 67368,
    SPELL_POSTMAN_A                 = 67376,
    SPELL_SHOP_A                    = 67377,
    SPELL_TIRED_A                   = 67401,

    SPELL_PEND_DAR                  = 63443,
    SPELL_PEND_GNO                  = 63442,
    SPELL_PEND_IRO                  = 63440,
    SPELL_PEND_ORG                  = 63444,
    SPELL_PEND_SEN                  = 63446,
    SPELL_PEND_SIL                  = 63438,
    SPELL_PEND_STO                  = 62727,
    SPELL_PEND_EXO                  = 63439,
    SPELL_PEND_UND                  = 63441,
    SPELL_PEND_THU                  = 63445,


    ACHIEVEMENT_CHAMP_DARNASSUS     = 2777,
    ACHIEVEMENT_CHAMP_GNOMEREGAN    = 2779,
    ACHIEVEMENT_CHAMP_IRONFORGE     = 2780,
    ACHIEVEMENT_CHAMP_ORGRIMMAR     = 2783,
    ACHIEVEMENT_CHAMP_SENJIN        = 2784,
    ACHIEVEMENT_CHAMP_SILVERMOON    = 2785,
    ACHIEVEMENT_CHAMP_STORMWIND     = 2781,
    ACHIEVEMENT_CHAMP_EXODAR        = 2778,
    ACHIEVEMENT_CHAMP_UNDERCITY     = 2787,
    ACHIEVEMENT_CHAMP_THUNDERBLUFF  = 2786,

    GOS_CHAMP_DAR                   = 1,
    GOS_CHAMP_GNO                   = 2,
    GOS_CHAMP_IRO                   = 3,
    GOS_CHAMP_ORG                   = 4,
    GOS_CHAMP_SEN                   = 5,
    GOS_CHAMP_SIL                   = 6,
    GOS_CHAMP_STO                   = 7,
    GOS_CHAMP_EXO                   = 8,
    GOS_CHAMP_UND                   = 9,
    GOS_CHAMP_THU                   = 10,

    NPC_SQUIRE                    = 33238,
    NPC_GRUNTLING                 = 33239,
};

class npc_argent_squire_gruntling : public CreatureScript
{
public:
    npc_argent_squire_gruntling() : CreatureScript("npc_argent_squire_gruntling") { }

    bool canShowPostman(Creature* creature)
    {
        if (creature->HasAura(SPELL_POSTMAN_H) || creature->HasAura(SPELL_POSTMAN_A))
            return true;

        if (creature->HasAura(SPELL_BANK_ERRAND_H) || creature->HasAura(SPELL_BANK_ERRAND_A))
            return false;

        if (creature->HasAura(SPELL_SHOP_H) || creature->HasAura(SPELL_SHOP_A))
            return false;

        if (creature->HasAura(SPELL_TIRED_H) || creature->HasAura(SPELL_TIRED_A))
            return false;

        return true;
    }

    bool canShowShop(Creature* creature)
    {
        if (creature->HasAura(SPELL_POSTMAN_H) || creature->HasAura(SPELL_POSTMAN_A))
            return false;

        if (creature->HasAura(SPELL_BANK_ERRAND_H) || creature->HasAura(SPELL_BANK_ERRAND_A))
            return false;

        if (creature->HasAura(SPELL_SHOP_H) || creature->HasAura(SPELL_SHOP_A))
            return true;

        if (creature->HasAura(SPELL_TIRED_H) || creature->HasAura(SPELL_TIRED_A))
            return false;

        return true;
    }

    bool canShowBank(Creature* creature)
    {
        if (creature->HasAura(SPELL_POSTMAN_H) || creature->HasAura(SPELL_POSTMAN_A))
            return false;

        if (creature->HasAura(SPELL_BANK_ERRAND_H) || creature->HasAura(SPELL_BANK_ERRAND_A))
            return true;

        if (creature->HasAura(SPELL_SHOP_H) || creature->HasAura(SPELL_SHOP_A))
            return false;

        if (creature->HasAura(SPELL_TIRED_H) || creature->HasAura(SPELL_TIRED_A))
            return false;

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (!player)
            return true;

        //if (canShowBank(creature))
        //    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, GOSSIP_BANK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_BANK);

        //if (canShowShop(creature))
        //    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TRADE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        //if (canShowPostman(creature))
        //    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_MAIL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        if (player->HasAchieved(ACHIEVEMENT_CHAMP_DARNASSUS))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_DAR_PENNANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOS_CHAMP_DAR);

        if (player->HasAchieved(ACHIEVEMENT_CHAMP_GNOMEREGAN))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_GNO_PENNANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOS_CHAMP_GNO);

        if (player->HasAchieved(ACHIEVEMENT_CHAMP_IRONFORGE))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_IFR_PENNANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOS_CHAMP_IRO);

        if (player->HasAchieved(ACHIEVEMENT_CHAMP_ORGRIMMAR))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ORC_PENNANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOS_CHAMP_ORG);

        if (player->HasAchieved(ACHIEVEMENT_CHAMP_SENJIN))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_DAR_PENNANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOS_CHAMP_SEN);

        if (player->HasAchieved(ACHIEVEMENT_CHAMP_SILVERMOON))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SIL_PENNANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOS_CHAMP_SIL);

        if (player->HasAchieved(ACHIEVEMENT_CHAMP_STORMWIND))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_STW_PENNANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOS_CHAMP_STO);

        if (player->HasAchieved(ACHIEVEMENT_CHAMP_EXODAR))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_EXO_PENNANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOS_CHAMP_EXO);

        if (player->HasAchieved(ACHIEVEMENT_CHAMP_UNDERCITY))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_UND_PENNANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOS_CHAMP_UND);

        if (player->HasAchieved(ACHIEVEMENT_CHAMP_THUNDERBLUFF))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TBL_PENNANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + GOS_CHAMP_THU);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    void cleanUpAllAuras(Creature* creature)
    {
        creature->RemoveAurasDueToSpell(SPELL_PEND_DAR);
        creature->RemoveAurasDueToSpell(SPELL_PEND_GNO);
        creature->RemoveAurasDueToSpell(SPELL_PEND_IRO);
        creature->RemoveAurasDueToSpell(SPELL_PEND_ORG);
        creature->RemoveAurasDueToSpell(SPELL_PEND_SEN);
        creature->RemoveAurasDueToSpell(SPELL_PEND_SIL);
        creature->RemoveAurasDueToSpell(SPELL_PEND_STO);
        creature->RemoveAurasDueToSpell(SPELL_PEND_EXO);
        creature->RemoveAurasDueToSpell(SPELL_PEND_UND);
        creature->RemoveAurasDueToSpell(SPELL_PEND_THU);
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
        //case GOSSIP_ACTION_BANK:
        //    player->GetSession()->SendShowBank(creature->GetGUID());
        //    break;
        //case GOSSIP_ACTION_TRADE:
        //    player->GetSession()->SendListInventory(creature->GetGUID());
        //    break;
        //case GOSSIP_ACTION_INFO_DEF:
        //    creature->MonsterSay("Nein, das kann ich nicht",LANG_UNIVERSAL,player->GetGUID());
        //    break;

        case GOSSIP_ACTION_INFO_DEF + GOS_CHAMP_DAR:
            cleanUpAllAuras(creature);
            creature->CastSpell(creature,SPELL_PEND_DAR, true);
            break;
        case GOSSIP_ACTION_INFO_DEF + GOS_CHAMP_GNO:
            cleanUpAllAuras(creature);
            creature->CastSpell(creature,SPELL_PEND_GNO, true);
            break;
        case GOSSIP_ACTION_INFO_DEF + GOS_CHAMP_IRO:
            cleanUpAllAuras(creature);
            creature->CastSpell(creature,SPELL_PEND_IRO, true);
            break;
        case GOSSIP_ACTION_INFO_DEF + GOS_CHAMP_ORG:
            cleanUpAllAuras(creature);
            creature->CastSpell(creature,SPELL_PEND_ORG, true);
            break;
        case GOSSIP_ACTION_INFO_DEF + GOS_CHAMP_SEN:
            cleanUpAllAuras(creature);
            creature->CastSpell(creature,SPELL_PEND_SEN, true);
            break;
        case GOSSIP_ACTION_INFO_DEF + GOS_CHAMP_SIL:
            cleanUpAllAuras(creature);
            creature->CastSpell(creature,SPELL_PEND_SIL, true);
            break;
        case GOSSIP_ACTION_INFO_DEF + GOS_CHAMP_STO:
            cleanUpAllAuras(creature);
            creature->CastSpell(creature,SPELL_PEND_STO, true);
            break;
        case GOSSIP_ACTION_INFO_DEF + GOS_CHAMP_EXO:
            cleanUpAllAuras(creature);
            creature->CastSpell(creature,SPELL_PEND_EXO, true);
            break;
        case GOSSIP_ACTION_INFO_DEF + GOS_CHAMP_UND:
            cleanUpAllAuras(creature);
            creature->CastSpell(creature,SPELL_PEND_UND, true);
            break;
        case GOSSIP_ACTION_INFO_DEF + GOS_CHAMP_THU:
            cleanUpAllAuras(creature);
            creature->CastSpell(creature,SPELL_PEND_THU, true);
            break;
        }
        player->CLOSE_GOSSIP_MENU();
        return true;
    }
};

enum BlackKnightGryphon
{
	SPELL_FLIGHT = 54422,
};

const Position BlackKnightGryphonWaypoints[19] =
{
    {8522.41f, 582.23f, 552.29f, 0.0f},
    {8502.92f, 610.34f, 550.01f, 0.0f},
    {8502.50f, 628.61f, 547.38f, 0.0f},
    {8484.50f, 645.16f, 547.30f, 0.0f},
    {8454.49f, 693.96f, 547.30f, 0.0f},
    {8403.00f, 742.34f, 547.30f, 0.0f},
    {8374.00f, 798.35f, 547.93f, 0.0f},
    {8376.43f, 858.33f, 548.00f, 0.0f},
    {8388.22f, 868.56f, 547.78f, 0.0f},
    {8473.82f, 875.34f, 547.30f, 0.0f},
    {8478.29f, 1014.63f, 547.30f, 0.0f},
    {8530.86f, 1037.65f, 547.30f, 0.0f},
    {8537.69f, 1078.33f, 554.80f, 0.0f},
    {8537.69f, 1078.33f, 578.10f, 0.0f},
    {8740.47f, 1611.72f, 496.19f, 0.0f},
    {9025.06f, 1799.67f, 171.54f, 0.0f},
    {9138.47f, 2013.83f, 104.24f, 0.0f},
    {9081.39f, 2158.26f, 72.98f, 0.0f},
    {9054.00f, 2124.85f, 57.13f, 0.0f},
};

class npc_black_knights_gryphon : public CreatureScript
{
public:
    npc_black_knights_gryphon() : CreatureScript("npc_black_knights_gryphon") { }

    struct npc_black_knights_gryphonAI : public VehicleAI
    {
        npc_black_knights_gryphonAI(Creature* creature) : VehicleAI(creature)
        {
             if (VehicleSeatEntry* vehSeat = const_cast<VehicleSeatEntry*>(sVehicleSeatStore.LookupEntry(3548)))
                vehSeat->m_flags |= VEHICLE_SEAT_FLAG_UNCONTROLLED;
        }

        bool IsInUse;
        bool WpReached;

        uint8 count;

        void Reset()
        {
            count = 0;
            WpReached = false;
            IsInUse = false;
        }

        void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply)
        {
            if (who && apply)
            {
                IsInUse = apply;
                WpReached = true;
                me->SetWalk(true);
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id < 18)
            {
                if (id > 11)
                {
                    DoCast(me, SPELL_FLIGHT);
                    me->SetSpeed(MOVE_FLIGHT, 4.0f);
                }

                ++count;
                WpReached = true;
            }
            else
            {
                if (Unit* player = me->GetVehicleKit()->GetPassenger(0))
				{
					if (player && player->GetTypeId() == TYPE_ID_PLAYER)
					{
						player->ToPlayer()->KilledMonsterCredit(me->GetEntry(), 0);
						player->ExitVehicle();
						me->DespawnAfterAction(5*IN_MILLISECONDS);
					}
                }
            }
        }

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!me->IsVehicle())
                return;

            if (!IsInUse)
                return;

            if (WpReached)
            {
                WpReached = false;
                me->GetMotionMaster()->MovePoint(count, BlackKnightGryphonWaypoints[count]);
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_black_knights_gryphonAI(creature);
    }
};

enum GraveSpells
{
    SPELL_COMPLETE_QUEST     = 66785,
	SPELL_BLACK_HOLE_GRAVE   = 67549,

	NPC_CULT_SABOTEUR        = 35116,
    NPC_CULT_ASSASSIN        = 35127,

	QUEST_BLACK_KNIGHT_CURSE = 14016,
};

#define SAY_SABOTEUR_1 "What do you think you're doing?"
#define SAY_SABOTEUR_2 "You're too late to stop our plan."
#define SAY_SABOTEUR_3 "See to it that I'm not followed."
#define SAY_ASSASSIN_1 "Gladly."

class npc_black_knights_grave : public CreatureScript
{
public:
    npc_black_knights_grave() : CreatureScript("npc_black_knights_grave") { }

    struct npc_black_knights_graveAI : public QuantumBasicAI
    {
        npc_black_knights_graveAI(Creature* creature) : QuantumBasicAI(creature), summons(creature)
		{
			SetCombatMovement(false);
			me->SetActive(true);
		}

        SummonList summons;

        bool EventRunning;

		uint32 SpeakTimer;
        uint32 SpeakPhase;

        uint64 GuidSaboteur;
        uint64 GuidAssassin;

        void Reset()
        {
            EventRunning = false;
            SpeakPhase = 0;

			DoCast(me, SPELL_BLACK_HOLE_GRAVE);
        }

        void StartEvent(uint64 attacker = 0)
        {
            if (EventRunning)
                return;

            EventRunning = true;

            Creature* creature = DoSummon(NPC_CULT_SABOTEUR, me, 5.0f, 30000, TEMPSUMMON_TIMED_DESPAWN);
            if (creature)
            {
                creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                creature->SetReactState(REACT_PASSIVE);
                creature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
                creature->SetCurrentFaction(16);
                GuidSaboteur = creature->GetGUID();
            }

            creature = DoSummon(NPC_CULT_ASSASSIN, me, 5.0f, 40000, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT);
            if (creature)
            {
                creature->SetReactState(REACT_PASSIVE);
                creature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
                creature->SetCurrentFaction(16);
                GuidAssassin = creature->GetGUID();
            }

            SpeakTimer = 7000;
            SpeakPhase = 1;
        }

        void StopEvent()
        {
            DoCastAOE(SPELL_COMPLETE_QUEST, true);
            EventRunning = false;
            summons.DespawnAll(2000);
            SpeakPhase = 0;
        }

        void JustSummoned(Creature* summoned)
        {
            summons.Summon(summoned);
        }

        void MoveInLineOfSight(Unit* who) 
        {
            if (!who || !who->ToPlayer())
                return;

            if (who->IsWithinDist(me, 20.0f))
                if (who->ToPlayer()->GetQuestStatus(QUEST_BLACK_KNIGHT_CURSE) == QUEST_STATUS_INCOMPLETE)
                    StartEvent(who->GetGUID());

            return;
        }

        void UpdateAI(const uint32 diff)
        {
            if (EventRunning)
            {
                if (SpeakTimer <= diff)
                {
                    switch (SpeakPhase)
                    {
                    case 1:
                        if (Creature* creature = Creature::GetCreature(*me, GuidSaboteur))
                            creature->MonsterSay(SAY_SABOTEUR_1,LANG_UNIVERSAL,0);
                        break;
                    case 2:
                        if (Creature* creature = Creature::GetCreature(*me, GuidSaboteur))
                            creature->MonsterSay(SAY_SABOTEUR_2,LANG_UNIVERSAL,0);
                        break;
                    case 3:
                        if (Creature* creature = Creature::GetCreature(*me, GuidSaboteur))
                            creature->MonsterSay(SAY_SABOTEUR_3,LANG_UNIVERSAL,0);
                        break;
                    case 4:
                        if (Creature* creature = Creature::GetCreature(*me, GuidAssassin))
                        {
                            creature->MonsterSay(SAY_ASSASSIN_1, LANG_UNIVERSAL,0);
                            creature->SetReactState(REACT_AGGRESSIVE);
                            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            std::list<Player*> pList;
                            pList = creature->GetNearestPlayersList(20.0f, true);
                            if (pList.size() > 0)
                            {
                                std::list<Player*>::const_iterator itr = pList.begin();
                                if ((*itr))
									creature->AI()->AttackStart((*itr));
                            }
                            summons.DespawnEntry(NPC_CULT_SABOTEUR, 2000);
                        }
                        break;
                    }
                    SpeakPhase++;
                    SpeakTimer = 3000;
                }
				else SpeakTimer -= diff;

                if (Creature* creature = Creature::GetCreature(*me, GuidAssassin))
                {
                    if (creature->IsDead())
                        StopEvent();
                }
				else
                {
                    EventRunning = false;
                    summons.DespawnAll();
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_black_knights_graveAI(creature);
    }
};

enum SlainTualiqVillager
{
	SPELL_READ_LAST_RITES = 66390,
};

class npc_slain_tualiq_villager : public CreatureScript
{
public:
    npc_slain_tualiq_villager() : CreatureScript("npc_slain_tualiq_villager") { }

    struct npc_slain_tualiq_villagerAI : public QuantumBasicAI
    {
        npc_slain_tualiq_villagerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        bool Credited;

        void Reset()
        {
            Credited = false;
        }

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (Credited)
                return;

            if (caster && caster->ToPlayer())
            {
                if (spell->Id == SPELL_READ_LAST_RITES)
                {
                    Credited = true;
                    caster->ToPlayer()->KilledMonsterCredit(me->GetEntry(), 0);
                    me->DespawnAfterAction(3*IN_MILLISECONDS);
                }
            }
        }

        void AttackStart(Unit* /*who*/){}

        void MoveInLineOfSight(Unit* /*who*/){}

        void UpdateAI(uint32 const /*diff*/){}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_slain_tualiq_villagerAI(creature);
    }
};

class spell_flaming_spear_targeting : public SpellScriptLoader
{
    public:
        spell_flaming_spear_targeting() : SpellScriptLoader("spell_flaming_spear_targeting") { }

        class spell_flaming_spear_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_flaming_spear_targeting_SpellScript);

            bool Load()
            {
                return GetCaster()->ToPlayer() != 0;
            }

            void GetTargets(std::list<Unit*>& targetList)
            {
                targetList.clear();

                std::list<Creature*> TargetList;
                GetCaster()->GetCreatureListWithEntryInGrid(TargetList, 35092, 150.0f);
                GetCaster()->GetCreatureListWithEntryInGrid(TargetList, 34925, 150.0f);

                if (TargetList.empty())
                    return;

                TargetList.sort(Trinity::ObjectDistanceOrderPred(GetCaster()));

                std::list<Creature*>::iterator itr = TargetList.begin();
                uint8 i = 3;
                for (std::list<Creature*>::const_iterator itr = TargetList.begin(); itr != TargetList.end(); ++itr)
                {
                    if (i == 0)
                        break;

                    if ((*itr)->IsAlive())
                    {
                        targetList.push_back(*itr);
                        i--;
                    }
                }
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_flaming_spear_targeting_SpellScript::GetTargets, EFFECT_0, TARGET_UNIT_TARGET_ANY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_flaming_spear_targeting_SpellScript();
        }
};

enum TournamentDummy
{
    NPC_CHARGE_TARGET         = 33272,
    NPC_MELEE_TARGET          = 33229,
    NPC_RANGED_TARGET         = 33243,

    SPELL_CHARGE_CREDIT       = 62658,
    SPELL_MELEE_CREDIT        = 62672,
    SPELL_RANGED_CREDIT       = 62673,
    SPELL_PLAYER_THRUST       = 62544,
    SPELL_PLAYER_BREAK_SHIELD = 62626,
    SPELL_PLAYER_CHARGE       = 62874,
    SPELL_RANGED_DEFEND       = 62719,
    SPELL_CHARGE_DEFEND       = 64100,
    SPELL_VULNERABLE          = 62665,
    SPELL_COUNTERATTACK       = 62709,

    EVENT_DUMMY_RECAST_DEFEND = 1,
    EVENT_DUMMY_RESET         = 2,
};

class npc_tournament_training_dummy : public CreatureScript
{
    public:
        npc_tournament_training_dummy(): CreatureScript("npc_tournament_training_dummy"){}

        struct npc_tournament_training_dummyAI : QuantumBasicAI
        {
            npc_tournament_training_dummyAI(Creature* creature) : QuantumBasicAI(creature) {}

            EventMap events;
            bool IsVulnerable;

            void Reset()
            {
                me->SetControlled(true, UNIT_STATE_STUNNED);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                IsVulnerable = false;

                switch (me->GetEntry())
                {
                    case NPC_CHARGE_TARGET:
                        DoCast(SPELL_CHARGE_DEFEND);
                        break;
                    case NPC_RANGED_TARGET:
                        me->CastCustomSpell(SPELL_RANGED_DEFEND, SPELLVALUE_AURA_STACK, 3, me);
                        break;
                }

                events.Reset();
                events.ScheduleEvent(EVENT_DUMMY_RECAST_DEFEND, 5000);
            }

            void EnterEvadeMode()
            {
                if (!_EnterEvadeMode())
                    return;

                Reset();
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage)
            {
                damage = 0;
                events.RescheduleEvent(EVENT_DUMMY_RESET, 10000);
            }

            void SpellHit(Unit* caster, SpellInfo const* spell)
            {
                switch (me->GetEntry())
                {
                    case NPC_CHARGE_TARGET:
                        if (spell->Id == SPELL_PLAYER_CHARGE)
						{
                            if (IsVulnerable)
                                DoCast(caster, SPELL_CHARGE_CREDIT, true);
						}
                        break;
                    case NPC_MELEE_TARGET:
                        if (spell->Id == SPELL_PLAYER_THRUST)
                        {
                            DoCast(caster, SPELL_MELEE_CREDIT, true);

                            if (Unit* target = caster->GetVehicleBase())
                                DoCast(target, SPELL_COUNTERATTACK, true);
                        }
                        break;
                    case NPC_RANGED_TARGET:
                        if (spell->Id == SPELL_PLAYER_BREAK_SHIELD)
						{
                            if (IsVulnerable)
                                DoCast(caster, SPELL_RANGED_CREDIT, true);
						}
                        break;
                }

                if (spell->Id == SPELL_PLAYER_BREAK_SHIELD)
                    if (!me->HasAura(SPELL_CHARGE_DEFEND) && !me->HasAura(SPELL_RANGED_DEFEND))
                        IsVulnerable = true;
            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_DUMMY_RECAST_DEFEND:
                        switch (me->GetEntry())
                        {
                            case NPC_CHARGE_TARGET:
                            {
                                if (!me->HasAura(SPELL_CHARGE_DEFEND))
                                    DoCast(SPELL_CHARGE_DEFEND);
                                break;
                            }
                            case NPC_RANGED_TARGET:
                            {
                                Aura* defend = me->GetAura(SPELL_RANGED_DEFEND);
                                if (!defend || defend->GetStackAmount() < 3 || defend->GetDuration() <= 8000)
                                    DoCast(SPELL_RANGED_DEFEND);
                                break;
                            }
                        }
                        IsVulnerable = false;
                        events.ScheduleEvent(EVENT_DUMMY_RECAST_DEFEND, 5000);
                        break;
                    case EVENT_DUMMY_RESET:
                        if (UpdateVictim())
                        {
                            EnterEvadeMode();
                            events.ScheduleEvent(EVENT_DUMMY_RESET, 10000);
                        }
                        break;
                }

                if (!UpdateVictim())
                    return;

                if (!me->HasUnitState(UNIT_STATE_STUNNED))
                    me->SetControlled(true, UNIT_STATE_STUNNED);
            }

            void MoveInLineOfSight(Unit* /*who*/){}
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_tournament_training_dummyAI(creature);
        }
};

enum NotDeadYet
{
    QUEST_NOT_DEAD_YET_A  = 13221,
    QUEST_NOT_DEAD_YET_H  = 13229,
    NPC_NOT_DEAD_YET_TRIG = 97002,
    SPELL_PW_FORTITUDE    = 23947,
    SPELL_PW_SHIELD       = 20697,
};

const Position KamarosWp[54] =
{
	{6718.48f, 3439.88f, 682.24f, 0.00f},
    {6723.54f, 3433.79f, 682.20f, 0.00f},
    {6734.04f, 3436.02f, 682.03f, 0.00f},
    {6737.66f, 3439.00f, 680.97f, 0.00f},
    {6742.07f, 3442.76f, 679.93f, 0.00f},
    {6745.65f, 3446.09f, 678.85f, 0.00f},
    {6750.47f, 3450.57f, 677.51f, 0.00f},
    {6755.94f, 3455.28f, 675.78f, 0.00f},
    {6759.58f, 3458.61f, 674.71f, 0.00f},
    {6765.04f, 3463.30f, 673.37f, 0.00f},
    {6769.22f, 3467.40f, 673.00f, 0.00f},
    {6771.95f, 3471.55f, 673.01f, 0.00f},
    {6777.21f, 3477.04f, 672.99f, 0.00f},
    {6782.91f, 3482.89f, 674.34f, 0.00f},
    {6779.54f, 3479.36f, 673.32f, 0.00f},
    {6773.88f, 3473.81f, 673.03f, 0.00f},
    {6786.32f, 3484.85f, 675.62f, 0.00f},
    {6792.51f, 3484.66f, 677.20f, 0.00f},
    {6797.39f, 3484.49f, 678.76f, 0.00f},
    {6800.64f, 3484.44f, 680.03f, 0.00f},
    {6805.98f, 3484.26f, 682.17f, 0.00f},
    {6810.80f, 3484.14f, 683.97f, 0.00f},
    {6816.15f, 3483.58f, 686.18f, 0.00f},
    {6822.91f, 3483.01f, 688.17f, 0.00f},
    {6829.09f, 3481.91f, 689.49f, 0.00f},
    {6836.49f, 3480.96f, 690.69f, 0.00f},
    {6844.57f, 3480.25f, 691.89f, 0.00f},
    {6853.38f, 3479.60f, 692.98f, 0.00f},
    {6859.95f, 3479.59f, 693.60f, 0.00f},
    {6867.60f, 3479.74f, 694.21f, 0.00f},
    {6875.25f, 3479.95f, 694.86f, 0.00f},
    {6882.69f, 3480.14f, 695.82f, 0.00f},
    {6889.85f, 3479.97f, 697.36f, 0.00f},
    {6896.19f, 3479.88f, 698.54f, 0.00f},
    {6902.49f, 3479.66f, 699.74f, 0.00f},
    {6908.08f, 3479.89f, 700.81f, 0.00f},
    {6913.89f, 3480.32f, 701.86f, 0.00f},
    {6920.57f, 3482.18f, 703.10f, 0.00f},
    {6925.61f, 3481.11f, 704.38f, 0.00f},
    {6930.92f, 3480.19f, 705.91f, 0.00f},
    {6936.04f, 3479.73f, 707.06f, 0.00f},
    {6942.06f, 3479.21f, 708.06f, 0.00f},
    {6947.22f, 3477.91f, 709.01f, 0.00f},
    {6953.44f, 3477.53f, 709.63f, 0.00f},
    {6959.23f, 3477.21f, 710.30f, 0.00f},
    {6967.35f, 3474.84f, 711.05f, 0.00f},
    {6980.54f, 3470.97f, 710.86f, 0.00f},
    {6985.85f, 3469.15f, 708.75f, 0.00f},
    {6991.36f, 3467.22f, 705.94f, 0.00f},
    {6997.30f, 3465.15f, 702.91f, 0.00f},
    {7004.14f, 3462.46f, 699.36f, 0.00f},
    {7009.42f, 3460.61f, 696.67f, 0.00f},
    {7018.43f, 3455.71f, 696.67f, 0.00f},
    {7029.21f, 3447.94f, 696.17f, 0.00f},
};

class npc_father_kamaros : public CreatureScript
{
public:
    npc_father_kamaros() : CreatureScript("npc_father_kamaros") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_NOT_DEAD_YET_A || quest->GetQuestId() == QUEST_NOT_DEAD_YET_H)
        {
            creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            creature->AI()->SetGUID(player->GetGUID());
            creature->CastSpell(player,SPELL_PW_FORTITUDE, true);
            player->CastSpell(player,SPELL_PW_SHIELD, true);
        }
        return true;
    }

    struct npc_father_kamarosAI : public QuantumBasicAI
    {
        npc_father_kamarosAI(Creature* creature) : QuantumBasicAI(creature) { }

        uint8 count;

        bool WpReached;
        bool MovementStarted;
        bool Finished;

        uint64 PlayerGUID;
        Player* player;

        void SetGUID(uint64 guid, int32 id)
        {
            MovementStarted = true;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            me->SetReactState(REACT_PASSIVE);
            me->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
            me->GetMotionMaster()->MovePoint(0, KamarosWp[0]);
            PlayerGUID = guid;
        }

        void Reset()
        {
            count = 0;
            WpReached = false;
            MovementStarted = false;
            Finished = false;
            PlayerGUID = 0;
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE || id != count)
                return;

            if (MovementStarted && id < 54)
            {
                ++count;
                WpReached = true;
            }
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (MovementStarted)
            {
                player = me->GetPlayer(*me, PlayerGUID);

                if (!player || !player->IsAlive() || me->GetDistance(player) > 30.0f)
                    me->DisappearAndDie();

                if (player->IsInCombatActive() && player->GetUInt64Value(UNIT_FIELD_TARGET))
                    me->Attack(me->GetUnit(*me, player->GetUInt64Value(UNIT_FIELD_TARGET)), true);
            }

            if (WpReached && !Finished)
            {
                WpReached = false;
                me->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
                me->GetMotionMaster()->MovePoint(count, KamarosWp[count]);
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (who->GetEntry() == NPC_NOT_DEAD_YET_TRIG && me->IsWithinDistInMap(who, 10.0f, true))
            {
                if (Player* player = me->GetPlayer(*me, PlayerGUID))
                {
                    switch (player->GetTeam())
                    {
                        case ALLIANCE:
							player->GroupEventHappens(QUEST_NOT_DEAD_YET_A, me);
							break;
                        case HORDE:
							player->GroupEventHappens(QUEST_NOT_DEAD_YET_H, me);
							break;
                    }
                    MovementStarted = false;
                    Finished = true;
                    me->DespawnAfterAction(10*IN_MILLISECONDS);
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_father_kamarosAI(creature);
    }
};

enum MalykrissBunny
{
    SPELL_ALTAR_OF_SACRIFICE_PULSE = 58196,
    SPELL_BLOOD_FORGE_PULSE        = 58198,
    SPELL_ICY_LOOKOUT_PULSE        = 58195,
    SPELL_RUNEWORKS_PULSE          = 58197,

	NPC_ALTAR_OF_SACRIFICE         = 31065,
    NPC_BLOOD_FORGE                = 31068,
    NPC_ICY_LOOKOUT                = 31064,
    NPC_RUNEWORKS                  = 31066,
};

class npc_malykriss_bunny : public CreatureScript
{
public:
    npc_malykriss_bunny() : CreatureScript("npc_malykriss_bunny") { }

    struct npc_malykriss_bunnyAI : public QuantumBasicAI
    {
		npc_malykriss_bunnyAI(Creature* creature) : QuantumBasicAI(creature) {}

        void MoveInLineOfSight(Unit* who)
        {
            if (who->GetTypeId() == TYPE_ID_PLAYER && me->IsWithinDistInMap(who, 60.0f))
			{
                switch (me->GetEntry())
                {
                    case NPC_ALTAR_OF_SACRIFICE:
                        DoCast(SPELL_ALTAR_OF_SACRIFICE_PULSE);
                        break;
                    case NPC_BLOOD_FORGE:
                        DoCast(SPELL_BLOOD_FORGE_PULSE);
                        break;
                    case NPC_ICY_LOOKOUT:
                        DoCast(SPELL_ICY_LOOKOUT_PULSE);
                        break;
                    case NPC_RUNEWORKS:
                        DoCast(SPELL_RUNEWORKS_PULSE);
                        break;
                }
			}
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_malykriss_bunnyAI(creature);
    }
};

enum BorrowedTechnologyAndVolatility
{
    // Spells
    SPELL_GRAB        = 59318,
    SPELL_PING_BUNNY  = 59375,
    SPELL_IMMOLATION  = 54690,
    SPELL_EXPLOSION   = 59335,
    SPELL_RIDE        = 56687,
	SPELL_FS_FLIGHT   = 60534,
    // Points
    POINT_GRAB_DECOY  = 1,
    POINT_FLY_AWAY    = 2,
    // Events
    EVENT_FLY_AWAY    = 1,
};

class npc_frostbrood_skytalon : public CreatureScript
{
    public:
        npc_frostbrood_skytalon() : CreatureScript("npc_frostbrood_skytalon") { }

        struct npc_frostbrood_skytalonAI : public VehicleAI
        {
            npc_frostbrood_skytalonAI(Creature* creature) : VehicleAI(creature) { }

            EventMap events;

			void Reset()
			{
				DoCast(me, SPELL_FS_FLIGHT);

				me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			}

            void IsSummonedBy(Unit* summoner)
            {
                me->GetMotionMaster()->MovePoint(POINT_GRAB_DECOY, summoner->GetPositionX(), summoner->GetPositionY(), summoner->GetPositionZ());
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (id == POINT_GRAB_DECOY)
				{
                    if (TempSummon* summon = me->ToTempSummon())
					{
                        if (Unit* summoner = summon->GetSummoner())
                            DoCast(summoner, SPELL_GRAB);
					}
				}
            }

            void UpdateAI(const uint32 diff)
            {
                VehicleAI::UpdateAI(diff);
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    if (eventId == EVENT_FLY_AWAY)
                    {
                        Position randomPosOnRadius;
                        randomPosOnRadius.m_positionZ = (me->GetPositionZ() + 40.0f);
                        me->GetNearPoint2D(randomPosOnRadius.m_positionX, randomPosOnRadius.m_positionY, 40.0f, me->GetAngle(me));
                        me->GetMotionMaster()->MovePoint(POINT_FLY_AWAY, randomPosOnRadius);
                    }
                }
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                switch (spell->Id)
                {
                    case SPELL_EXPLOSION:
                        DoCast(me, SPELL_IMMOLATION);
                        break;
                    case SPELL_RIDE:
                        DoCastAOE(SPELL_PING_BUNNY);
                        events.ScheduleEvent(EVENT_FLY_AWAY, 100);
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_frostbrood_skytalonAI(creature);
        }
};

enum ArgentSkyTalon
{
	SPELL_RIDE_SKYTALON    = 46598,
	SPELL_PARACHUTE        = 53208,
	SPELL_FLIGHT_AURA      = 56682,
	SPELL_COMMAND_SKYTALON = 56678,

	NPC_SKYTALON_MOUNT     = 30228,

	AREA_ARGENT_VANGUARD   = 4501,
	AREA_VALLEY_OF_ECHOES  = 4504,
	AREA_THE_BREACH        = 4505,
	AREA_SCOURGEHOLME      = 4506,

	GOSSIP_SKYTALON        = 15608,

	EVENT_CHECK_AREA       = 1,
};

class npc_argent_skytalon_av : public CreatureScript
{
public:
    npc_argent_skytalon_av() : CreatureScript("npc_argent_skytalon_av") { }

    struct npc_argent_skytalon_avAI : public QuantumBasicAI
    {
        npc_argent_skytalon_avAI(Creature* creature) : QuantumBasicAI(creature){}

		void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
		{
			if (menuId == GOSSIP_SKYTALON && gossipListId == 0)
			{
				player->RemoveMount();
				player->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);
				me->SummonCreature(NPC_SKYTALON_MOUNT, 6126.79f, -20.0397f, 385.338f, 0.779469f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
				player->PlayerTalkClass->SendCloseGossip();
			}
		}

		void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331649);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void JustSummoned(Creature* summoned)
		{
			summoned->CastSpell(summoned, SPELL_RIDE_SKYTALON, true);

			if (Player* player = me->FindPlayerWithDistance(10.0f))
				player->EnterVehicle(summoned);
		}

		void UpdateAI(uint32 const diff)
        {
			if (!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_argent_skytalon_avAI(creature);
    }
};

class npc_argent_skytalon_mount : public CreatureScript
{
public:
    npc_argent_skytalon_mount() : CreatureScript("npc_argent_skytalon_mount") { }

    struct npc_argent_skytalon_mountAI : public VehicleAI
    {
		npc_argent_skytalon_mountAI(Creature* creature) : VehicleAI(creature)
		{
			DoCast(me, SPELL_FLIGHT_AURA, true);
			me->SetUnitMovementFlags(MOVEMENTFLAG_FLYING);
		}

		EventMap events;

		void Reset()
		{
			me->SetCanFly(true);
			me->SetDisableGravity(true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void OnCharmed(bool apply)
        {
			VehicleAI::OnCharmed(apply);
        }

		void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply)
		{
			if (passenger && apply)
			{
				DoCast(me, SPELL_COMMAND_SKYTALON, true);
				events.ScheduleEvent(EVENT_CHECK_AREA, 0.5*IN_MILLISECONDS);
			}
			else
			{
				if (me->HasAura(SPELL_FLIGHT_AURA))
				{
					if (Player* player = passenger->ToPlayer())
						player->CastSpell(player, SPELL_PARACHUTE, true);

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
						if (me->GetAreaId() != AREA_ARGENT_VANGUARD && me->GetAreaId() != AREA_VALLEY_OF_ECHOES && me->GetAreaId() != AREA_THE_BREACH && me->GetAreaId() != AREA_SCOURGEHOLME)
						{
							if (Vehicle* vehicle = me->GetVehicleKit())
							{
								if (Unit* player = vehicle->GetPassenger(0))
								{
									if (player->GetTypeId() == TYPE_ID_PLAYER)
									{
										player->ExitVehicle();
										player->CastSpell(player, SPELL_PARACHUTE, true);
										me->GetMotionMaster()->MoveRandom(25.0f);
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
        return new npc_argent_skytalon_mountAI(creature);
    }
};

enum ReanimatedCrusader
{
	SPELL_SPRINKLE_HOLY_WATER = 57806,
	SPELL_FREED_CRUSADER_SOUL = 57808,
	SPELL_AVENGERS_SHIELD     = 32674,
	SPELL_HAMMER_OF_INJUSTICE = 58154,
	SPELL_UNHOLY_LIGHT        = 58153,
};

class npc_reanimated_crusader_q13110 : public CreatureScript
{
public:
    npc_reanimated_crusader_q13110() : CreatureScript("npc_reanimated_crusader_q13110") {}

    struct npc_reanimated_crusader_q13110AI : public QuantumBasicAI
    {
        npc_reanimated_crusader_q13110AI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 HammerOfInjusticeTimer;

        void Reset()
        {
			HammerOfInjusticeTimer = 4000;
        }

		void SpellHit(Unit* caster, const SpellInfo *spell)
        {
            if (spell->Id == SPELL_SPRINKLE_HOLY_WATER)
			{
				caster->CastSpell(caster, SPELL_FREED_CRUSADER_SOUL, true);
				me->DespawnAfterAction();
			}
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_AVENGERS_SHIELD);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HammerOfInjusticeTimer <= diff)
			{
				DoCastVictim(SPELL_HAMMER_OF_INJUSTICE);
				HammerOfInjusticeTimer = urand(7000, 8000);
			}
			else HammerOfInjusticeTimer -= diff;

			if (HealthBelowPct(30))
				DoCast(me, SPELL_UNHOLY_LIGHT);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_reanimated_crusader_q13110AI(creature);
    }
};

/*
*************************************************
* Scripting Quest The Air Stands Still by Crispi
*
* http://www.youtube.com/watch?v=wvm6Pxex37A
*
* SD Data: Total Complete 100% Need Added Texts
*
* npc_salranax_the_flesh_render (Done 100%)
* npc_underking_talonox (Done 100%)
* npc_high_priest_yathamon (Done 100%)
* npc_munch (Done 100%)
* npc_jayde (Done 100%)
* npc_highlord_darion_mograine_ic (Done 100%)
* npc_melt (Done 100%)
*
*************************************************
*/

enum TheAirStandsStill
{
	// Portals
	NPC_DEATH_GATE_MOGRAINE       = 30841,
	NPC_DEATH_GATE_MUNCH          = 30850,
	NPC_DEATH_GATE_JAYDE          = 30852,
	// Bosses
	NPC_SALRANAX_THE_FLESH_RENDER = 30829,
	NPC_UNDERKING_TALONOX         = 30830,
	NPC_HIGH_PRIEST_YATHAMON      = 30831,
	// Npcs
	NPC_MOGRAINE                  = 30838,
	NPC_JAYDE                     = 30839,
	NPC_MUNCH                     = 30840,
	NPC_MELT                      = 30851,
	// Salranax Spells
	SPELL_FROST_ARMOR             = 18100,
	SPELL_COUNTERSPELL            = 15122,
	SPELL_FIREBALL                = 15242,
	SPELL_CONE_OF_COLD            = 15244,
	// Talonox Spells
	SPELL_MANDIBLE_CRUSH          = 60802,
	SPELL_BLINDING_SWARM          = 50284,
	// Yathamon Spells
	SPELL_VENOMOUS_BITE           = 61705,
	SPELL_ENCASING_WEBS           = 4962,
	SPELL_PSYCHIC_SCREAM          = 34322,
	SPELL_MIND_FLAY               = 38243,
	// Quest Spells
	SPELL_WARN_HORN_OF_ACHERUS    = 57906,
	// Quest Checking ID
	THE_AIR_STANDS_STEEL          = 13125,
	THE_PURGING_OF_SCOURGEHOLME   = 13118,
	// Creature Abilities
	SPELL_DEATH_GRIP              = 49576,
	SPELL_ICY_TOUCH               = 52372,
	SPELL_AS_PLAGUE_STRIKE        = 52373,
	SPELL_BLOOD_STRIKE            = 52374,
	SPELL_CLAW                    = 47468,
	SPELL_BLOOD_BOIL              = 49941,
	SPELL_ICY_TOUCH_R5            = 49909,
	SPELL_OBLITERATE              = 51425,
};

#define MUNCH_SAY_1    "Say hello to my little friend!"
#define MUNCH_SAY_2    "We are done here. I am out."
#define MUNCH_SAY_MELT "Arise Melt!"
#define JAYDE_SAY_1    "Too weak to finish your own battles? Very well then..."
#define JAYDE_SAY_2    "Anything else you need help with before I go? Right, I thought not..."
#define MOGRAINE_SAY_1 "You challenge death itself! Come then, King. Face the might of Mograine!"
#define MOGRAINE_SAY_2 "I will see you back at the Valley of Echoes. Suffer well, ally."

static Position MunchPos = {6842.97f, 599.432f, 426.228f, 4.0209f};
static Position MeltPos = {6841.18f, 600.559f, 426.228f, 4.0209f};
static Position MunchGatePos = {6844.6f, 601.28f, 426.228f, 4.0209f};
static Position JaydePos = {6865.28f, 414.058f, 471.699f, 1.03422f};
static Position JaydeGatePos = {6864.47f, 412.434f, 471.697f, 1.11276f};
static Position MograinePos = {7164.06f, 669.813f, 494.339f, 5.85838f};
static Position MograineGatePos = {7162.89f, 670.613f, 494.398f, 5.68166f};

class npc_salranax_the_flesh_render : public CreatureScript
{
public:
    npc_salranax_the_flesh_render() : CreatureScript("npc_salranax_the_flesh_render") {}

    struct npc_salranax_the_flesh_renderAI : public QuantumBasicAI
    {
		npc_salranax_the_flesh_renderAI(Creature* creature) : QuantumBasicAI(creature), Summons(me) {}

		uint32 FireballTimer;
		uint32 ConeOfColdTimer;
		uint32 CounterspellTimer;

		SummonList Summons;

        void Reset()
        {
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);

			FireballTimer = 3000;
			ConeOfColdTimer = 4000;
			CounterspellTimer = 6000;

			Summons.DespawnAll();
        }

		void MoveInLineOfSight(Unit* who)
        {
			if (Player* player = who->ToPlayer())
			{
				if (player->IsWithinDist(me, 60.0f))
				{
					if (player->GetQuestStatus(THE_AIR_STANDS_STEEL) == QUEST_STATUS_INCOMPLETE && player->GetQuestStatus(THE_PURGING_OF_SCOURGEHOLME) == QUEST_STATUS_REWARDED)
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
				}
			}
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_WARN_HORN_OF_ACHERUS)
			{
				me->AI()->AttackStart(caster);

				me->SummonCreature(NPC_MUNCH, MunchPos, TEMPSUMMON_MANUAL_DESPAWN, 600000);
				me->SummonCreature(NPC_MELT, MeltPos, TEMPSUMMON_MANUAL_DESPAWN, 600000);
				me->SummonCreature(NPC_DEATH_GATE_MUNCH, MunchGatePos, TEMPSUMMON_MANUAL_DESPAWN, 600000);
			}
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_FROST_ARMOR);
		}

		void JustDied(Unit* /*killer*/)
        {
			Summons.DespawnAll();
        }

		void JustReachedHome()
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			Summons.Summon(summon);

			if (summon->GetEntry() == NPC_MUNCH)
				return;
			if (summon->GetEntry() == NPC_MELT)
				return;
			if (summon->GetEntry() == NPC_DEATH_GATE_MUNCH)
				return;
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireballTimer <= diff)
			{
				DoCastVictim(SPELL_FIREBALL);
				FireballTimer = urand(4000, 5000);
			}
			else FireballTimer -= diff;

			if (ConeOfColdTimer <= diff)
			{
				DoCast(SPELL_CONE_OF_COLD);
				ConeOfColdTimer = urand(6000, 7000);
			}
			else ConeOfColdTimer -= diff;
			
			if (CounterspellTimer <= diff)
			{
				DoCastVictim(SPELL_COUNTERSPELL);
				CounterspellTimer = urand(10000, 11000);
			}
			else CounterspellTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_salranax_the_flesh_renderAI(creature);
    }
};

class npc_underking_talonox : public CreatureScript
{
public:
    npc_underking_talonox() : CreatureScript("npc_underking_talonox") {}

    struct npc_underking_talonoxAI : public QuantumBasicAI
    {
		npc_underking_talonoxAI(Creature* creature) : QuantumBasicAI(creature), Summons(me) {}

		uint32 MandibleCrushTimer;
		uint32 BlindingSwarmTimer;

		SummonList Summons;

        void Reset()
        {
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);

			MandibleCrushTimer = 4000;
			BlindingSwarmTimer = 5000;

			Summons.DespawnAll();
        }

		void MoveInLineOfSight(Unit* who)
        {
			if (Player* player = who->ToPlayer())
			{
				if (player->IsWithinDist(me, 60.0f))
				{
					if (player->GetQuestStatus(THE_AIR_STANDS_STEEL) == QUEST_STATUS_INCOMPLETE && player->GetQuestStatus(THE_PURGING_OF_SCOURGEHOLME) == QUEST_STATUS_REWARDED)
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
				}
			}
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_WARN_HORN_OF_ACHERUS)
			{
				me->AI()->AttackStart(caster);

				me->SummonCreature(NPC_MOGRAINE, MograinePos, TEMPSUMMON_MANUAL_DESPAWN, 600000);
				me->SummonCreature(NPC_DEATH_GATE_MOGRAINE, MograineGatePos, TEMPSUMMON_MANUAL_DESPAWN, 600000);
			}
		}

		void JustDied(Unit* /*killer*/)
        {
			Summons.DespawnAll();
        }

		void JustReachedHome()
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			Summons.Summon(summon);

			if (summon->GetEntry() == NPC_MOGRAINE)
				return;
			if (summon->GetEntry() == NPC_DEATH_GATE_MOGRAINE)
				return;
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MandibleCrushTimer <= diff)
			{
				DoCastVictim(SPELL_MANDIBLE_CRUSH);
				MandibleCrushTimer = urand(6000, 7000);
			}
			else MandibleCrushTimer -= diff;

			if (BlindingSwarmTimer <= diff)
			{
				DoCastVictim(SPELL_BLINDING_SWARM);
				BlindingSwarmTimer = urand(9000, 10000);
			}
			else BlindingSwarmTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_underking_talonoxAI(creature);
    }
};

class npc_high_priest_yathamon : public CreatureScript
{
public:
    npc_high_priest_yathamon() : CreatureScript("npc_high_priest_yathamon") {}

    struct npc_high_priest_yathamonAI : public QuantumBasicAI
    {
		npc_high_priest_yathamonAI(Creature* creature) : QuantumBasicAI(creature), Summons(me) {}

		uint32 VenomousBiteTimer;
		uint32 EncasingWebsTimer;
		uint32 PsychicScreamTimer;
		uint32 MindFlayTimer;

		SummonList Summons;

        void Reset()
        {
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);

			VenomousBiteTimer = 1000;
			EncasingWebsTimer = 3000;
			PsychicScreamTimer = 5000;
			MindFlayTimer = 7000;

			Summons.DespawnAll();
        }

		void MoveInLineOfSight(Unit* who)
        {
			if (Player* player = who->ToPlayer())
			{
				if (player->IsWithinDist(me, 60.0f))
				{
					if (player->GetQuestStatus(THE_AIR_STANDS_STEEL) == QUEST_STATUS_INCOMPLETE && player->GetQuestStatus(THE_PURGING_OF_SCOURGEHOLME) == QUEST_STATUS_REWARDED)
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
				}
			}
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_WARN_HORN_OF_ACHERUS)
			{
				me->AI()->AttackStart(caster);

				me->SummonCreature(NPC_JAYDE, JaydePos, TEMPSUMMON_MANUAL_DESPAWN, 600000);
				me->SummonCreature(NPC_DEATH_GATE_JAYDE, JaydeGatePos, TEMPSUMMON_MANUAL_DESPAWN, 600000);
			}
		}

		void JustDied(Unit* /*killer*/)
        {
			Summons.DespawnAll();
        }

		void JustReachedHome()
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			Summons.Summon(summon);

			if (summon->GetEntry() == NPC_JAYDE)
				return;
			if (summon->GetEntry() == NPC_DEATH_GATE_JAYDE)
				return;
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (VenomousBiteTimer <= diff)
			{
				DoCastVictim(SPELL_VENOMOUS_BITE);
				VenomousBiteTimer = urand(3000, 4000);
			}
			else VenomousBiteTimer -= diff;

			if (EncasingWebsTimer <= diff)
			{
				DoCastVictim(SPELL_ENCASING_WEBS);
				EncasingWebsTimer = urand(5000, 6000);
			}
			else EncasingWebsTimer -= diff;

			if (PsychicScreamTimer <= diff)
			{
				DoCastAOE(SPELL_PSYCHIC_SCREAM);
				PsychicScreamTimer = urand(7000, 8000);
			}
			else PsychicScreamTimer -= diff;

			if (MindFlayTimer <= diff)
			{
				DoCastVictim(SPELL_MIND_FLAY);
				MindFlayTimer = urand(9000, 10000);
			}
			else MindFlayTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_high_priest_yathamonAI(creature);
    }
};

class npc_munch : public CreatureScript
{
public:
    npc_munch() : CreatureScript("npc_munch") {}

    struct npc_munchAI : public QuantumBasicAI
    {
        npc_munchAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 IcyTouchTimer;
		uint32 PlagueStrikeTimer;
		uint32 BloodStrikeTimer;

        void Reset()
        {
			IcyTouchTimer = 2000;
			PlagueStrikeTimer = 3000;
			BloodStrikeTimer = 4000;
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->MonsterSay(MUNCH_SAY_MELT, LANG_UNIVERSAL, 0);
			me->MonsterSay(urand(0, 1) ? MUNCH_SAY_1 : MUNCH_SAY_2, LANG_UNIVERSAL, 0);
			DoCastVictim(SPELL_DEATH_GRIP);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (IcyTouchTimer <= diff)
			{
				DoCastVictim(SPELL_ICY_TOUCH);
				IcyTouchTimer = urand(3000, 4000);
			}
			else IcyTouchTimer -= diff;

			if (PlagueStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_AS_PLAGUE_STRIKE);
				PlagueStrikeTimer = urand(5000, 6000);
			}
			else PlagueStrikeTimer -= diff;

			if (BloodStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_BLOOD_STRIKE);
				BloodStrikeTimer = urand(7000, 8000);
			}
			else BloodStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_munchAI(creature);
    }
};

class npc_jayde : public CreatureScript
{
public:
    npc_jayde() : CreatureScript("npc_jayde") {}

    struct npc_jaydeAI : public QuantumBasicAI
    {
        npc_jaydeAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 IcyTouchTimer;
		uint32 PlagueStrikeTimer;
		uint32 BloodStrikeTimer;

        void Reset()
        {
			IcyTouchTimer = 2000;
			PlagueStrikeTimer = 3000;
			BloodStrikeTimer = 4000;
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->MonsterSay(urand(0, 1) ? JAYDE_SAY_1 : JAYDE_SAY_2, LANG_UNIVERSAL, 0);
			DoCastVictim(SPELL_DEATH_GRIP);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (IcyTouchTimer <= diff)
			{
				DoCastVictim(SPELL_ICY_TOUCH);
				IcyTouchTimer = urand(3000, 4000);
			}
			else IcyTouchTimer -= diff;

			if (PlagueStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_AS_PLAGUE_STRIKE);
				PlagueStrikeTimer = urand(5000, 6000);
			}
			else PlagueStrikeTimer -= diff;

			if (BloodStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_BLOOD_STRIKE);
				BloodStrikeTimer = urand(7000, 8000);
			}
			else BloodStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jaydeAI(creature);
    }
};

class npc_highlord_darion_mograine_ic : public CreatureScript
{
public:
    npc_highlord_darion_mograine_ic() : CreatureScript("npc_highlord_darion_mograine_ic") {}

    struct npc_highlord_darion_mograine_icAI : public QuantumBasicAI
    {
        npc_highlord_darion_mograine_icAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 IcyTouchTimer;
		uint32 BloodBoilTimer;
		uint32 ObliterateTimer;

        void Reset()
        {
			IcyTouchTimer = 2000;
			BloodBoilTimer = 3000;
			ObliterateTimer = 4000;
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->MonsterSay(urand(0, 1) ? MOGRAINE_SAY_1 : MOGRAINE_SAY_2, LANG_UNIVERSAL, 0);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (IcyTouchTimer <= diff)
			{
				DoCastVictim(SPELL_ICY_TOUCH_R5);
				IcyTouchTimer = urand(3000, 4000);
			}
			else IcyTouchTimer -= diff;

			if (BloodBoilTimer <= diff)
			{
				DoCastAOE(SPELL_BLOOD_BOIL);
				BloodBoilTimer = urand(5000, 6000);
			}
			else BloodBoilTimer -= diff;

			if (ObliterateTimer <= diff)
			{
				DoCastVictim(SPELL_OBLITERATE);
				ObliterateTimer = urand(7000, 8000);
			}
			else ObliterateTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_highlord_darion_mograine_icAI(creature);
    }
};

class npc_melt : public CreatureScript
{
public:
    npc_melt() : CreatureScript("npc_melt") {}

    struct npc_meltAI : public QuantumBasicAI
    {
        npc_meltAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ClawTimer;

        void Reset()
        {
			ClawTimer = 2000;
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ClawTimer <= diff)
			{
				DoCastVictim(SPELL_CLAW);
				ClawTimer = urand(3000, 4000);
			}
			else ClawTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_meltAI(creature);
    }
};

enum FleshGiant
{
    QUEST_FLESH_GIANT_CHAMPION = 13235,
	SPELL_SIMPLE_TELEPORT      = 64195,
    NPC_MORBIDUS               = 30698,
    NPC_LICH_KING              = 31301,
    NPC_OLAKIN                 = 31428,
    NPC_DHAKAR                 = 31306,
    FACTION_HOSTILE            = 16,
    FACTION_BASIC              = 2102,

    EVENT_INTRO                = 1,
    EVENT_LK_SAY_1             = 2,
    EVENT_LK_SAY_2             = 3,
    EVENT_LK_SAY_3             = 4,
    EVENT_LK_SAY_4             = 5,
    EVENT_LK_SAY_5             = 6,
    EVENT_OUTRO                = 7,
    EVENT_START                = 8,

    SAY_DHAKAR_START           = 0,
    SAY_LK_1                   = 0,
    SAY_LK_2                   = 1,
    SAY_LK_3                   = 2,
    SAY_LK_4                   = 3,
    SAY_LK_5                   = 4,
    SAY_OLAKIN_PAY             = 0,
};

#define GOSSIP_DHAKAR "Let's kill this... thing... and get this over with."

static Position LichKingSpawnPos = {6863.82f, 3583.17f, 735.807f, 5.70603f};

class npc_margrave_dhakar : public CreatureScript
{
    public:
        npc_margrave_dhakar() : CreatureScript("npc_margrave_dhakar") { }

        struct npc_margrave_dhakarAI : public QuantumBasicAI
        {
            npc_margrave_dhakarAI(Creature* creature) : QuantumBasicAI(creature), summons(me), LichKingGuid(0) { }

			EventMap events;
            SummonList summons;
            uint64 LichKingGuid;
			bool EventStart;

            void Reset()
            {
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_NONE);

                events.Reset();
                summons.DespawnAll();
				EventStart = true;
            }

			void MoveInLineOfSight(Unit* who)
			{
				if (Player* player = who->ToPlayer())
				{
					if (player->IsWithinDist(me, 1.0f))
					{
						if (player->GetQuestStatus(QUEST_FLESH_GIANT_CHAMPION) == QUEST_STATUS_INCOMPLETE)
						{
							if (EventStart)
							{
								events.ScheduleEvent(EVENT_INTRO, 1000);
								me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

								EventStart = false;
							}
						}
					}
				}
			}

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INTRO:
                        {
                            Talk(SAY_DHAKAR_START);
                            me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);

                            if (Creature* morbidus = me->FindCreatureWithDistance(NPC_MORBIDUS, 50.0f, true))
                            {
                                if (Creature* lichKing = me->SummonCreature(NPC_LICH_KING, LichKingSpawnPos))
                                {
                                    LichKingGuid = lichKing->GetGUID();
                                    lichKing->SetFacingTo(morbidus->GetOrientation());
                                    lichKing->CastSpell(lichKing, SPELL_SIMPLE_TELEPORT, true);
                                }
                            }

                            events.ScheduleEvent(EVENT_LK_SAY_1, 5000);
                            break;
                        }
                        case EVENT_LK_SAY_1:
                        {
                            if (Creature* lichKing = Unit::GetCreature(*me, LichKingGuid))
							{
                                lichKing->AI()->Talk(SAY_LK_1);
							}
                            events.ScheduleEvent(EVENT_LK_SAY_2, 5000);
                            break;
                        }
                        case EVENT_LK_SAY_2:
                        {
                            if (Creature* lichKing = Unit::GetCreature(*me, LichKingGuid))
							{
                                lichKing->AI()->Talk(SAY_LK_2);
							}
                            events.ScheduleEvent(EVENT_LK_SAY_3, 5000);
                            break;
                        }
                        case EVENT_LK_SAY_3:
                        {
                            if (Creature* lichKing = Unit::GetCreature(*me, LichKingGuid))
							{
                                lichKing->AI()->Talk(SAY_LK_3);
							}
                            events.ScheduleEvent(EVENT_LK_SAY_4, 5000);
                            break;
                        }
                        case EVENT_LK_SAY_4:
                        {
                            if (Creature* lichKing = Unit::GetCreature(*me, LichKingGuid))
							{
                                lichKing->AI()->Talk(SAY_LK_4);
							}
                            events.ScheduleEvent(EVENT_OUTRO, 12000);
                            break;
                        }
                        case EVENT_LK_SAY_5:
                        {
                            if (Creature* lichKing = Unit::GetCreature(*me, LichKingGuid))
							{
                                lichKing->AI()->Talk(SAY_LK_5);
							}
                            events.ScheduleEvent(EVENT_OUTRO, 8000);
                            break;
                        }
                        case EVENT_OUTRO:
                        {
                            if (Creature* olakin = me->FindCreatureWithDistance(NPC_OLAKIN, 50.0f, true))
							{
                                olakin->AI()->Talk(SAY_OLAKIN_PAY);
							}
                            if (Creature* lichKing = Unit::GetCreature(*me, LichKingGuid))
							{
                                lichKing->DespawnAfterAction(0);
							}
                            events.ScheduleEvent(EVENT_START, 5000);
                            break;
                        }
                        case EVENT_START:
                        {
                            if (Creature* morbidus = me->FindCreatureWithDistance(NPC_MORBIDUS, 50.0f, true))
                            {
                                morbidus->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE | UNIT_FLAG_DISABLE_MOVE);
                                morbidus->SetCurrentFaction(FACTION_HOSTILE);
                            }
                            break;
                        }
                    }
				}

				DoMeleeAttackIfReady();
            }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_margrave_dhakarAI(creature);
    }
};

class npc_morbidus : public CreatureScript
{
public:
	npc_morbidus() : CreatureScript("npc_morbidus") { }

	struct npc_morbidusAI : public QuantumBasicAI
	{
		npc_morbidusAI(Creature* creature) : QuantumBasicAI(creature) { }

		void Reset()
		{
			if (Creature* dhakar = me->FindCreatureWithDistance(NPC_DHAKAR, 50.0f, true))
				dhakar->AI()->Reset();

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			me->SetReactState(REACT_PASSIVE);
			me->SetCurrentFaction(FACTION_BASIC);
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_morbidusAI(creature);
	}
};

enum FallenHeroSpirit
{
	SPELL_BLESSING_OF_PEACE     = 66719,
	SPELL_COSMETIC_GHOST_VISUAL = 57932,
	SPELL_FH_STRIKE             = 11976,
	NPC_FALLEN_HERO_KILL_CREDIT = 35055,
	SAY_BLESSED                 = 0,
};

class npc_fallen_hero_spirit : public CreatureScript
{
public:
    npc_fallen_hero_spirit() : CreatureScript("npc_fallen_hero_spirit") {}

    struct npc_fallen_hero_spiritAI : public QuantumBasicAI
    {
        npc_fallen_hero_spiritAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 StrikeTimer;

        void Reset()
        {
			DoCast(me, SPELL_COSMETIC_GHOST_VISUAL);

			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

			StrikeTimer = 4000;
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_BLESSING_OF_PEACE)
			{
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				me->AttackStop();
				me->SetReactState(REACT_PASSIVE);
				me->AI()->Talk(SAY_BLESSED);
				me->DespawnAfterAction(6*IN_MILLISECONDS);

				if (Player* player = caster->ToPlayer())
					caster->ToPlayer()->KilledMonsterCredit(NPC_FALLEN_HERO_KILL_CREDIT, 0);
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_FH_STRIKE);
				StrikeTimer = urand(5000, 6000);
			}
			else StrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fallen_hero_spiritAI(creature);
    }
};

enum ThaneUfrangTheMighty
{
	SPELL_BRUTAL_STRIKE       = 58460,
	SPELL_POWERFUL_SMASH      = 60868,
	SPELL_TUTM_REND           = 16509,
	SPELL_SHADOW_VAULT_DECREE = 31696,
	QUEST_SHADOW_VAULT_DECREE = 12943,
};

class npc_thane_ufrang_the_mighty : public CreatureScript
{
public:
    npc_thane_ufrang_the_mighty() : CreatureScript("npc_thane_ufrang_the_mighty") {}

    struct npc_thane_ufrang_the_mightyAI : public QuantumBasicAI
    {
        npc_thane_ufrang_the_mightyAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BrutalStrikeTimer;
		uint32 PowerfulSmashTimer;
		uint32 RendTimer;

        void Reset()
        {
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);

			RendTimer = 2000;
			BrutalStrikeTimer = 4000;
			PowerfulSmashTimer = 5000;
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (Player* player = who->ToPlayer())
			{
				if (player->IsWithinDist(me, 3.0f))
				{
					if (player->GetQuestStatus(QUEST_SHADOW_VAULT_DECREE) == QUEST_STATUS_INCOMPLETE)
					{
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
						me->AI()->AttackStart(player);
					}
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
				DoCastVictim(SPELL_TUTM_REND);
				RendTimer = urand(3000, 4000);
			}
			else RendTimer -= diff;

			if (BrutalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_BRUTAL_STRIKE);
				BrutalStrikeTimer = urand(5000, 6000);
			}
			else BrutalStrikeTimer -= diff;

			if (PowerfulSmashTimer <= diff)
			{
				DoCastAOE(SPELL_POWERFUL_SMASH);
				PowerfulSmashTimer = urand(7000, 8000);
			}
			else PowerfulSmashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_thane_ufrang_the_mightyAI(creature);
    }
};

enum BloodInTheWater
{
	SPELL_UNDERWATER_BLOOD   = 47172,
	SPELL_THRASH             = 3417,
	SPELL_GORE_BLADDER       = 6509,
	NPC_RAVENOUS_KILL_CREDIT = 29391,
};

class npc_ravenous_jaws : public CreatureScript
{
public:
    npc_ravenous_jaws() : CreatureScript("npc_ravenous_jaws") {}

    struct npc_ravenous_jawsAI : public QuantumBasicAI
    {
        npc_ravenous_jawsAI(Creature* creature) : QuantumBasicAI(creature) {}

		void Reset(){}

		void JustDied(Unit* /*killer*/)
		{
			DoCastAOE(SPELL_UNDERWATER_BLOOD);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_THRASH);
		}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_GORE_BLADDER)
            {
				if (Player* player = caster->ToPlayer())
				{
					me->RemoveAurasDueToSpell(SPELL_UNDERWATER_BLOOD);
					caster->ToPlayer()->KilledMonsterCredit(NPC_RAVENOUS_KILL_CREDIT, 0);
				}

				me->DespawnAfterAction();
			}
        }

		void UpdateAI(uint32 const /*diff*/)
		{
			if (!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ravenous_jawsAI(creature);
    }
};

enum OnslaughtHarborGuard
{
	SPELL_DARKMENDERS_TINCTURE      = 52741,
	SPELL_SUMMON_CORRUPTED_ONSLAUGT = 54415,
	SPELL_FROST_SHOT                = 18802,
	SPELL_SHOOT                     = 6660,
	NPC_KILL_CREDIT_THEIR_CORPSES   = 29398,
};

class npc_onslaught_harbor_guard : public CreatureScript
{
public:
    npc_onslaught_harbor_guard() : CreatureScript("npc_onslaught_harbor_guard") {}

    struct npc_onslaught_harbor_guardAI : public QuantumBasicAI
    {
        npc_onslaught_harbor_guardAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 ShootTimer;
		uint32 FrostShotTimer;

		void Reset()
		{
			ShootTimer = 500;
			FrostShotTimer = 1000;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_USE_STANDING);
		}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_DARKMENDERS_TINCTURE)
            {
				if (Player* player = caster->ToPlayer())
				{
					DoCastAOE(SPELL_SUMMON_CORRUPTED_ONSLAUGT);
					caster->ToPlayer()->KilledMonsterCredit(NPC_KILL_CREDIT_THEIR_CORPSES, 0);
				}

				me->DespawnAfterAction();
			}
        }

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;
			
			if (ShootTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_SHOOT);
						ShootTimer = urand(2000, 3000);
					}
				}
			}
			else ShootTimer -= diff;

			if (FrostShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_FROST_SHOT);
						FrostShotTimer = urand(4000, 5000);
					}
				}
			}
			else FrostShotTimer -= diff;

			if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    DoStartMovement(me->GetVictim());
            }
			else
			{
				if (me->GetDistance2d(me->GetVictim()) > 10)
                    DoStartNoMovement(me->GetVictim());

                if (me->GetDistance2d(me->GetVictim()) > 30)
                    DoStartMovement(me->GetVictim());
			}

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_onslaught_harbor_guardAI(creature);
    }
};

enum JotunheimTrashAndQuests
{
	SPELL_EBON_DLADE_BANNER  = 23301,
	SPELL_MAGUS_FROSTBOLT    = 9672,
	SPELL_MAGUS_FROST_NOVA   = 15532,
	SPELL_SUM_WATER_TERROR   = 7855,
	SPELL_CHOP               = 43410,
	SPELL_DEMORALIZE         = 23262,
	SPELL_BACKSTAB           = 38029,
	SPELL_NSS_THROW_SPEAR    = 43413,
	SPELL_BLUDGEONING_STRIKE = 60870,
	SPELL_JS_DEMORALIZ_SHOUT = 13730,
	NPC_WATER_TERROR         = 30633,
	SAY_WATER_MAGUS_AGGRO    = 0,
	SAY_JOT_WARRIOR_AGGRO    = 0,
	SAY_SPEAR_SISTER_AGGRO   = 0,
	SAY_SLEEP_WATCHER_AGGRO  = 0,
};

class npc_mjordin_water_magus : public CreatureScript
{
public:
    npc_mjordin_water_magus() : CreatureScript("npc_mjordin_water_magus") {}

    struct npc_mjordin_water_magusAI : public QuantumBasicAI
    {
		npc_mjordin_water_magusAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 FrostboltTimer;
		uint32 FrostNovaTimer;
		SummonList Summons;

        void Reset()
		{
			FrostboltTimer = 1000;
			FrostNovaTimer = 3000;

			Summons.DespawnAll();
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			Talk(SAY_WATER_MAGUS_AGGRO);
			DoCast(me, SPELL_SUM_WATER_TERROR, true);
		}

		void JustSummoned(Creature* summon)
		{
			switch (summon->GetEntry())
			{
				case NPC_WATER_TERROR:
					break;
				default:
					break;
			}
			Summons.Summon(summon);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_EBON_DLADE_BANNER)
				me->DespawnAfterAction();
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostboltTimer <= diff)
			{
				DoCastVictim(SPELL_MAGUS_FROSTBOLT);
				FrostboltTimer = urand(3000, 4000);
			}
			else FrostboltTimer -= diff;

			if (FrostNovaTimer <= diff)
			{
				DoCastAOE(SPELL_MAGUS_FROST_NOVA);
				FrostNovaTimer = urand(5000, 6000);
			}
			else FrostNovaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mjordin_water_magusAI(creature);
    }
};

class npc_jotunheim_warrior : public CreatureScript
{
public:
    npc_jotunheim_warrior() : CreatureScript("npc_jotunheim_warrior") {}

    struct npc_jotunheim_warriorAI : public QuantumBasicAI
    {
		npc_jotunheim_warriorAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ChopTimer;
		uint32 DemoralizeTimer;

        void Reset()
		{
			ChopTimer = 1000;
			DemoralizeTimer = 3000;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			Talk(SAY_JOT_WARRIOR_AGGRO);
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_EBON_DLADE_BANNER)
				me->DespawnAfterAction();
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ChopTimer <= diff)
			{
				DoCastVictim(SPELL_CHOP);
				ChopTimer = urand(3000, 4000);
			}
			else ChopTimer -= diff;

			if (DemoralizeTimer <= diff)
			{
				DoCastAOE(SPELL_DEMORALIZE);
				DemoralizeTimer = urand(5000, 6000);
			}
			else DemoralizeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jotunheim_warriorAI(creature);
    }
};

class npc_njorndar_spear_sister : public CreatureScript
{
public:
    npc_njorndar_spear_sister() : CreatureScript("npc_njorndar_spear_sister") {}

    struct npc_njorndar_spear_sisterAI : public QuantumBasicAI
    {
		npc_njorndar_spear_sisterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BackstabTimer;

        void Reset()
		{
			BackstabTimer = 1000;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			Talk(SAY_SPEAR_SISTER_AGGRO);
			DoCastVictim(SPELL_NSS_THROW_SPEAR);
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_EBON_DLADE_BANNER)
				me->DespawnAfterAction();
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BackstabTimer <= diff)
			{
				DoCastVictim(SPELL_BACKSTAB);
				BackstabTimer = urand(3000, 4000);
			}
			else BackstabTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_njorndar_spear_sisterAI(creature);
    }
};

class npc_jotunheim_sleep_watcher : public CreatureScript
{
public:
    npc_jotunheim_sleep_watcher() : CreatureScript("npc_jotunheim_sleep_watcher") {}

    struct npc_jotunheim_sleep_watcherAI : public QuantumBasicAI
    {
		npc_jotunheim_sleep_watcherAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BludgeoningStrikeTimer;
		uint32 DemoralizeShoutTimer;

        void Reset()
		{
			BludgeoningStrikeTimer = 1000;
			DemoralizeShoutTimer = 3000;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			Talk(SAY_SLEEP_WATCHER_AGGRO);
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_EBON_DLADE_BANNER)
				me->DespawnAfterAction();
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BludgeoningStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_BLUDGEONING_STRIKE);
				BludgeoningStrikeTimer = urand(3000, 4000);
			}
			else BludgeoningStrikeTimer -= diff;

			if (DemoralizeShoutTimer <= diff)
			{
				DoCastAOE(SPELL_JS_DEMORALIZ_SHOUT);
				DemoralizeShoutTimer = urand(5000, 6000);
			}
			else DemoralizeShoutTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jotunheim_sleep_watcherAI(creature);
    }
};

enum IcecrownAndMoongladeQuests
{
	SPELL_FORCECAST_PORTAL_MOONGLADE = 57536,
	SPELL_TELEPORT_TO_MOONGLADE      = 57654,
	SPELL_RETURN_FROM_MOONGLADE      = 57539,
	SPELL_NATURE_PORTAL_STATE        = 61722,

	QUEST_THE_KEEPERS_FAVOR          = 13073,
	QUEST_THE_BOON_OF_REMULOS        = 13075,
};

class npc_moonglade_portal : public CreatureScript
{
public:
    npc_moonglade_portal() : CreatureScript("npc_moonglade_portal") { }

	struct npc_moonglade_portalAI : public QuantumBasicAI
    {
		npc_moonglade_portalAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			DoCast(me, SPELL_NATURE_PORTAL_STATE);
		}

		void MoveInLineOfSight(Unit* who)
		{
			if (Player* player = who->ToPlayer())
			{
				if (player->IsWithinDist(me, 2.0f) && player->GetQuestStatus(QUEST_THE_KEEPERS_FAVOR) == QUEST_STATUS_COMPLETE)
					player->CastSpell(player, SPELL_TELEPORT_TO_MOONGLADE, true);
			}
		}

        void UpdateAI(const uint32 /*diff*/){}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_moonglade_portalAI(creature);
	}
};

class npc_moonglade_return_portal : public CreatureScript
{
public:
    npc_moonglade_return_portal() : CreatureScript("npc_moonglade_return_portal") { }

	struct npc_moonglade_return_portalAI : public QuantumBasicAI
    {
		npc_moonglade_return_portalAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			DoCast(me, SPELL_NATURE_PORTAL_STATE);
		}

		void MoveInLineOfSight(Unit* who)
		{
			if (Player* player = who->ToPlayer())
			{
				if (player->IsWithinDist(me, 2.0f) && player->GetQuestStatus(QUEST_THE_BOON_OF_REMULOS) == QUEST_STATUS_COMPLETE)
					player->CastSpell(player, SPELL_RETURN_FROM_MOONGLADE, true);
			}
		}

        void UpdateAI(const uint32 /*diff*/){}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_moonglade_return_portalAI(creature);
	}
};

enum QuestThatsAbominable
{
	SPELL_ABOMINATION_EXPLOSION = 58596,

	NPC_ICY_GHOUL_CREDIT        = 31743,
	NPC_RISEN_SOLDIER_CREDIT    = 32167,
	NPC_VICIOUS_GEIST_CREDIT    = 32168,
};

class npc_icy_ghoul : public CreatureScript
{
public:
    npc_icy_ghoul() : CreatureScript("npc_icy_ghoul") {}

    struct npc_icy_ghoulAI : public QuantumBasicAI
    {
        npc_icy_ghoulAI(Creature* creature) : QuantumBasicAI(creature) {}

		void SpellHit(Unit* caster, SpellInfo const* spell)
        {
			if (spell->Id == SPELL_ABOMINATION_EXPLOSION)
			{
				if (Player* player = me->FindPlayerWithDistance(250.0f, true))
					player->ToPlayer()->KilledMonsterCredit(NPC_ICY_GHOUL_CREDIT, 0);
			}
		}

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_icy_ghoulAI(creature);
    }
};

class npc_risen_alliance_soldier : public CreatureScript
{
public:
    npc_risen_alliance_soldier() : CreatureScript("npc_risen_alliance_soldier") {}

    struct npc_risen_alliance_soldierAI : public QuantumBasicAI
    {
        npc_risen_alliance_soldierAI(Creature* creature) : QuantumBasicAI(creature) {}

		void SpellHit(Unit* caster, SpellInfo const* spell)
        {
			if (spell->Id == SPELL_ABOMINATION_EXPLOSION)
			{
				if (Player* player = me->FindPlayerWithDistance(250.0f, true))
					player->ToPlayer()->KilledMonsterCredit(NPC_RISEN_SOLDIER_CREDIT, 0);
			}
		}

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_risen_alliance_soldierAI(creature);
    }
};

class npc_vicious_geist : public CreatureScript
{
public:
    npc_vicious_geist() : CreatureScript("npc_vicious_geist") {}

    struct npc_vicious_geistAI : public QuantumBasicAI
    {
        npc_vicious_geistAI(Creature* creature) : QuantumBasicAI(creature) {}

		void SpellHit(Unit* caster, SpellInfo const* spell)
        {
			if (spell->Id == SPELL_ABOMINATION_EXPLOSION)
			{
				if (Player* player = me->FindPlayerWithDistance(250.0f, true))
					player->ToPlayer()->KilledMonsterCredit(NPC_VICIOUS_GEIST_CREDIT, 0);
			}
		}

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vicious_geistAI(creature);
    }
};

enum VileLikeFire
{
	SPELL_SUMMON_NJORNDAR_DRAKE = 57401,
	SPELL_COSMETIC_FLAMES       = 39199,
	SPELL_FIRE                  = 7448,
	SPELL_STRAFE_JOTUNHEIM      = 7769,

	QUEST_VILE_LIKE_FIRE        = 13071,
	NPC_KILL_VILE_CREDIT_BUNNY  = 30576,
	NPC_FIRE_BUNNY              = 30599,
};

class npc_njorndar_proto_drake : public CreatureScript
{
public:
    npc_njorndar_proto_drake() : CreatureScript("npc_njorndar_proto_drake") { }

    struct npc_njorndar_proto_drakeAI : public QuantumBasicAI
    {
        npc_njorndar_proto_drakeAI(Creature* creature) : QuantumBasicAI(creature){}

		void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void MoveInLineOfSight(Unit* who)
		{
			if (Player* player = who->ToPlayer())
			{
				if (player->IsWithinDist(me, 15.0f) && player->GetQuestStatus(QUEST_VILE_LIKE_FIRE) == QUEST_STATUS_INCOMPLETE)
					player->CastSpell(player, SPELL_SUMMON_NJORNDAR_DRAKE, true);
			}
		}

		void UpdateAI(uint32 const /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_njorndar_proto_drakeAI(creature);
    }
};

class npc_vile_like_fire_bunny : public CreatureScript
{
public:
    npc_vile_like_fire_bunny() : CreatureScript("npc_vile_like_fire_bunny") {}

    struct npc_vile_like_fire_bunnyAI : public QuantumBasicAI
    {
        npc_vile_like_fire_bunnyAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 ResetTimer;

		bool Burned;

        void Reset()
        {
			ResetTimer = 60000;

			Burned = false;

			me->RemoveAurasDueToSpell(SPELL_COSMETIC_FLAMES);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_STRAFE_JOTUNHEIM && !Burned)
			{
				Burned = true;

				DoCast(me, SPELL_COSMETIC_FLAMES, true);

				if (Player* player = me->FindPlayerWithDistance(200.0f))
					player->KilledMonsterCredit(me->GetEntry(), 0);
			}
        }

        void UpdateAI(uint32 const diff)
		{
			if (ResetTimer <= diff)
			{
				Reset();
				ResetTimer = 60000;
			}
			else ResetTimer -= diff;
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vile_like_fire_bunnyAI(creature);
    }
};

enum QuestByFireBePurged
{
	SPELL_FEIGN_DEATH_ROOT = 31261,
	SPELL_OLAKINS_TORCH    = 58641,
	SPELL_BURNING_CORPSE   = 59216,
};

class npc_festering_corpse : public CreatureScript
{
public:
    npc_festering_corpse() : CreatureScript("npc_festering_corpse") {}

    struct npc_festering_corpseAI : public QuantumBasicAI
    {
        npc_festering_corpseAI(Creature* creature) : QuantumBasicAI(creature){}

		bool Burned;

        void Reset()
        {
			DoCast(me, SPELL_FEIGN_DEATH_ROOT, true);

			Burned = false;

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FEIGN_DEATH);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_OLAKINS_TORCH && !Burned)
			{
				Burned = true;

				DoCast(me, SPELL_BURNING_CORPSE, true);

				if (Player* player = caster->ToPlayer())
					player->KilledMonsterCredit(me->GetEntry(), 0);

				me->DespawnAfterAction(6*IN_MILLISECONDS);
			}
        }

        void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_festering_corpseAI(creature);
    }
};

enum QuestFindTheAncientHero
{
	NPC_ISKANDER_CREDIT_BUNNY = 30880,
	NPC_ISKALDER              = 30884,
	NPC_SUBJUGATED_ISKALDER   = 30886,

	SPELL_BONE_WITCH_AMULET   = 3921,
	SPELL_INVISIBILITY        = 23452,
	SPELL_COSMETIC_ANCIENT    = 24240,
	SPELL_SUMMON_ISKANDLER    = 25730,
	SPELL_ANCIENT_HERO_CREDIT = 25729, // No Works
	SPELL_POSSESSION          = 58102,

	QUEST_FIND_ANCIENT_HERO   = 13133,

	GOSSIP_MENU_MJORDIN       = 10008,

	SAY_ISKALDER_SUMMON       = -1420113,
};

class npc_the_bone_witch : public CreatureScript
{
public:
    npc_the_bone_witch() : CreatureScript("npc_the_bone_witch") { }

    struct npc_the_bone_witchAI : public QuantumBasicAI
    {
		npc_the_bone_witchAI(Creature* creature) : QuantumBasicAI(creature) {}

		void Reset()
		{
			DoCast(me, SPELL_POSSESSION);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void MoveInLineOfSight(Unit* who)
        {
            QuantumBasicAI::MoveInLineOfSight(who);

            if (who->GetTypeId() != TYPE_ID_UNIT)
                return;

            if (who->GetEntry() == NPC_SUBJUGATED_ISKALDER && me->IsWithinDistInMap(who, 10.0f))
            {
                if (Unit* owner = who->GetOwner())
                {
                    if (Player* player = owner->ToPlayer())
					{
						player->CastSpell(player, SPELL_COSMETIC_ANCIENT, true);
						player->KilledMonsterCredit(NPC_ISKANDER_CREDIT_BUNNY, 0);
						player->RemoveAllMinionsByEntry(NPC_SUBJUGATED_ISKALDER);
						who->ToCreature()->DespawnAfterAction();
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
        return new npc_the_bone_witchAI(creature);
    }
};

class npc_slumbering_mjordin : public CreatureScript
{
public:
    npc_slumbering_mjordin() : CreatureScript("npc_slumbering_mjordin") {}

    struct npc_slumbering_mjordinAI : public QuantumBasicAI
    {
        npc_slumbering_mjordinAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

			me->SetCorpseDelay(5);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_CUSTOM_SPELL_02);
		}

		void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
		{
			if (menuId == GOSSIP_MENU_MJORDIN && gossipListId == 0)
			{
				switch (urand(0, 3))
                {
                    case 0:
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
						me->AI()->AttackStart(player);
						break;
                    case 1:
						me->CastSpell(me, SPELL_COSMETIC_ANCIENT);
						me->DespawnAfterAction(1*IN_MILLISECONDS);
						break;
					case 2:
						me->SummonCreature(NPC_ISKALDER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30*IN_MILLISECONDS);
						me->DespawnAfterAction();
						break;
					case 3:
						me->CastSpell(me, SPELL_INVISIBILITY);
						break;
				}

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
        return new npc_slumbering_mjordinAI(creature);
    }
};

class npc_iskalder : public CreatureScript
{
public:
    npc_iskalder() : CreatureScript("npc_iskalder") { }

    struct npc_iskalderAI : public QuantumBasicAI
    {
		npc_iskalderAI(Creature* creature) : QuantumBasicAI(creature) {}

		void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_SUMMON_ISKANDLER)
			{
				if (Player* player = caster->ToPlayer())
					me->DespawnAfterAction();
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
        return new npc_iskalderAI(creature);
    }
};

class npc_subjugated_iskalder : public CreatureScript
{
public:
    npc_subjugated_iskalder() : CreatureScript("npc_subjugated_iskalder") { }

    struct npc_subjugated_iskalderAI : public QuantumBasicAI
    {
		npc_subjugated_iskalderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 SayTimer;

		bool Follow;

		void Reset()
		{
			SayTimer = 1*IN_MILLISECONDS;

			Follow = false;

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void MoveInLineOfSight(Unit* who)
        {
            QuantumBasicAI::MoveInLineOfSight(who);

			if (Follow)
				return;

			if (Player* player = who->ToPlayer())
			{
				if (me->IsWithinDistInMap(player, 5.0f) && player->GetQuestStatus(QUEST_FIND_ANCIENT_HERO) == QUEST_STATUS_INCOMPLETE)
				{
					me->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, me->GetFollowAngle(), MOTION_SLOT_ACTIVE);

					Follow = true;
				}
            }
        }

		void UpdateAI(const uint32 diff)
        {
			// Out of Combat Timer
			if (SayTimer <= diff && !me->IsInCombatActive())
			{
				DoSendQuantumText(SAY_ISKALDER_SUMMON, me);
				SayTimer = 1*HOUR*IN_MILLISECONDS;
			}
			else SayTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_subjugated_iskalderAI(creature);
    }
};

enum QuestValkyrionMustBurn
{
	SPELL_VALKYRION_FIRE_COMETIC = 55934,
	SPELL_FLAMING_HARPOON_1      = 55812,
	SPELL_FLAMING_HARPOON_2      = 55896,
	SPELL_VEHICLE_HARPOON        = 57053,

	NPC_VALKYRION_FIRE_BUNNY     = 30103,
	NPC_VALKYRION_CANNON         = 30066,
};

class npc_dry_haystack : public CreatureScript
{
public:
    npc_dry_haystack() : CreatureScript("npc_dry_haystack") { }

    struct npc_dry_haystackAI : public QuantumBasicAI
    {
		npc_dry_haystackAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

		void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void DamageTaken(Unit* attacker, uint32 &amount)
		{
			if (amount >= me->GetHealth())
				amount = 0;

			if (attacker->ToCreature() && attacker->GetEntry() == NPC_VALKYRION_CANNON)
			{
				if (Player* player = me->FindPlayerWithDistance(420.0f, true))
				{
					if (player->HasAura(SPELL_VEHICLE_HARPOON))
					{
						me->Kill(me);
						player->KilledMonsterCredit(me->GetEntry(), 0);

						// Cosmetic
						std::list<Creature*> TrashList;
						me->GetCreatureListWithEntryInGrid(TrashList, NPC_VALKYRION_FIRE_BUNNY, 12.0f);

						if (!TrashList.empty())
						{
							for (std::list<Creature*>::const_iterator itr = TrashList.begin(); itr != TrashList.end(); ++itr)
							{
								if (Creature* fire = *itr)
									fire->CastSpell(fire, SPELL_VALKYRION_FIRE_COMETIC, true);
							}
						}
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
        return new npc_dry_haystackAI(creature);
    }
};

void AddSC_icecrown()
{
    new npc_arete();
    new npc_dame_evniki_kapsalis();
    new npc_squire_david();
    new npc_argent_valiant();
    new npc_alorah_and_grimmin();
    new npc_guardian_pavilion();
    new npc_vereth_the_cunning;
    new spell_argent_cannon();
    new npc_blessed_banner();
    new npc_captured_crusader();
    new npc_the_ocular();
    new npc_general_lightsbane();
    new npc_free_your_mind();
    new npc_saronite_mine_slave();
    new npc_vendor_tournament_fraction_champion();
    new npc_faction_valiant_champion();
    new npc_squire_danny();
    new npc_squire_cavin();
    new npc_the_black_knight();
    new npc_argent_champion();
    new npc_argent_squire_gruntling();
    new npc_black_knights_gryphon();
    new npc_black_knights_grave();
    new npc_slain_tualiq_villager();
    new spell_flaming_spear_targeting();
	new npc_tournament_training_dummy();
	new npc_father_kamaros;
	new npc_malykriss_bunny();
	new npc_frostbrood_skytalon();
	new npc_argent_skytalon_av();
	new npc_argent_skytalon_mount();
	new npc_reanimated_crusader_q13110();
	new npc_salranax_the_flesh_render();
	new npc_underking_talonox();
	new npc_high_priest_yathamon();
	new npc_munch();
	new npc_jayde();
	new npc_highlord_darion_mograine_ic();
	new npc_melt();
	new npc_margrave_dhakar();
	new npc_morbidus();
	new npc_fallen_hero_spirit();
	new npc_thane_ufrang_the_mighty();
	new npc_ravenous_jaws();
	new npc_onslaught_harbor_guard();
	new npc_mjordin_water_magus();
	new npc_jotunheim_warrior();
	new npc_njorndar_spear_sister();
	new npc_jotunheim_sleep_watcher();
	new npc_moonglade_portal();
	new npc_moonglade_return_portal();
	new npc_icy_ghoul();
	new npc_risen_alliance_soldier();
	new npc_vicious_geist();
	new npc_njorndar_proto_drake();
	new npc_vile_like_fire_bunny();
	new npc_festering_corpse();
	new npc_the_bone_witch();
	new npc_slumbering_mjordin();
	new npc_iskalder();
	new npc_subjugated_iskalder();
	new npc_dry_haystack();
}