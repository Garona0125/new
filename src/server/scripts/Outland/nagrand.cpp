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

class mob_shattered_rumbler : public CreatureScript
{
public:
    mob_shattered_rumbler() : CreatureScript("mob_shattered_rumbler") { }

    struct mob_shattered_rumblerAI : public QuantumBasicAI
    {
        mob_shattered_rumblerAI(Creature* creature) : QuantumBasicAI(creature) {}

        bool Spawn;

        void Reset() 
        { 
            Spawn = false; 
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == 32001 && !Spawn)
            {
                float x = me->GetPositionX();
                float y = me->GetPositionY();
                float z = me->GetPositionZ();

                caster->SummonCreature(18181, x+(0.7f * (rand()%30)), y+(rand()%5), z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
                caster->SummonCreature(18181, x+(rand()%5), y-(rand()%5), z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
                caster->SummonCreature(18181, x-(rand()%5), y+(0.5f *(rand()%60)), z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
                me->setDeathState(CORPSE);
                Spawn = true;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_shattered_rumblerAI(creature);
    }
};

enum LumpMisc
{
	SPELL_VISUAL_SLEEP  = 16093,
	SPELL_SPEAR_THROW   = 32248,
	LUMP_SAY0           = -1000190,
	LUMP_SAY1           = -1000191,
	LUMP_DEFEAT         = -1000192,
};

#define GOSSIP_HL  "I need answers, ogre!"
#define GOSSIP_SL1 "Why are Boulderfist out this far? You know that this is Kurenai territory."
#define GOSSIP_SL2 "And you think you can just eat anything you want? You're obviously trying to eat the Broken of Telaar."
#define GOSSIP_SL3 "This means war, Lump! War I say!"

class mob_lump : public CreatureScript
{
    public:
        mob_lump() : CreatureScript("mob_lump") { }

		bool OnGossipHello(Player* player, Creature* creature)
        {
            if (player->GetQuestStatus(9918) == QUEST_STATUS_INCOMPLETE)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

            player->SEND_GOSSIP_MENU(9352, creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SL1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                    player->SEND_GOSSIP_MENU(9353, creature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+1:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SL2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                    player->SEND_GOSSIP_MENU(9354, creature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+2:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SL3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                    player->SEND_GOSSIP_MENU(9355, creature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+3:
                    player->SEND_GOSSIP_MENU(9356, creature->GetGUID());
                    player->TalkedToCreature(18354, creature->GetGUID());
                    break;
            }
            return true;
        }

        struct mob_lumpAI : public QuantumBasicAI
        {
            mob_lumpAI(Creature* creature) : QuantumBasicAI(creature)
            {
                Restart = false;
            }

			uint32 ResetTimer;
            uint32 SpearThrowTimer;

            bool Restart;

            void Reset()
            {
                ResetTimer = 60000;
                SpearThrowTimer = 2000;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
            }

            void DamageTaken(Unit* attacker, uint32& damage)
            {
                if (attacker->ToPlayer() && me->HealthBelowPctDamaged(30, damage))
                {
                    if (!Restart && attacker->ToPlayer()->GetQuestStatus(9918) == QUEST_STATUS_INCOMPLETE)
                    {
                        damage = 0;
                        attacker->AttackStop();
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->RemoveAllAuras();
                        me->DeleteThreatList();
                        me->CombatStop(true);
                        me->SetCurrentFaction(1080); // friendly
                        me->SetStandState(UNIT_STAND_STATE_SIT);
                        DoSendQuantumText(LUMP_DEFEAT, me);
                        Restart = true;
                    }
                }
            }

            void EnterToBattle(Unit* /*who*/)
            {
                if (me->HasAura(SPELL_VISUAL_SLEEP))
                    me->RemoveAura(SPELL_VISUAL_SLEEP);

                if (!me->IsStandState())
                     me->SetStandState(UNIT_STAND_STATE_STAND);

                DoSendQuantumText(RAND(LUMP_SAY0, LUMP_SAY1), me);
            }

            void UpdateAI(uint32 const diff)
            {
                if (Restart)
                {
                    if (ResetTimer <= diff)
                    {
                        EnterEvadeMode();
                        Restart = false;
                        me->SetCurrentFaction(1711); // hostile
                        return;
                    }
                    else ResetTimer -= diff;
                }

                if (!UpdateVictim())
                    return;

                if (SpearThrowTimer <= diff)
                {
                    DoCastVictim(SPELL_SPEAR_THROW);
                    SpearThrowTimer = 20000;
                }
                else SpearThrowTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_lumpAI(creature);
        }
};

#define GOSSIP_HATS1 "I see twisted steel and smell sundered earth."
#define GOSSIP_HATS2 "Well...?"
#define GOSSIP_HATS3 "[PH] Story about Illidan's Pupil"
#define GOSSIP_SATS1 "Legion?"
#define GOSSIP_SATS2 "And now?"
#define GOSSIP_SATS3 "How do you see them now?"
#define GOSSIP_SATS4 "Forge camps?"
#define GOSSIP_SATS5 "Ok."
#define GOSSIP_SATS6 "Story done"

class npc_altruis_the_sufferer : public CreatureScript
{
public:
    npc_altruis_the_sufferer() : CreatureScript("npc_altruis_the_sufferer") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(9991) == QUEST_STATUS_NONE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HATS1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+10);

        if (player->GetQuestStatus(9991) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HATS2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+20);

        if (player->GetQuestStatus(10646) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HATS3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+30);

        player->SEND_GOSSIP_MENU(9419, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+10:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SATS1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
                player->SEND_GOSSIP_MENU(9420, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+11:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SATS2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
                player->SEND_GOSSIP_MENU(9421, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+12:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SATS3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
                player->SEND_GOSSIP_MENU(9422, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+13:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SATS4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
                player->SEND_GOSSIP_MENU(9423, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+14:
                player->SEND_GOSSIP_MENU(9424, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+20:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SATS5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 21);
                player->SEND_GOSSIP_MENU(9427, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+21:
                player->CLOSE_GOSSIP_MENU();
                player->AreaExploredOrEventHappens(9991);
                break;
            case GOSSIP_ACTION_INFO_DEF+30:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SATS6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 31);
                player->SEND_GOSSIP_MENU(384, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+31:
                player->CLOSE_GOSSIP_MENU();
                player->AreaExploredOrEventHappens(10646);
                break;
        }
        return true;
    }

	bool OnQuestAccept(Player* player, Creature* /*creature*/, Quest const* /*quest*/)
    {
        if (!player->GetQuestRewardStatus(9991))
        {
            player->CLOSE_GOSSIP_MENU();
            player->ActivateTaxiPathTo(532);
        }
        return true;
    }
};

#define GOSSIP_HGG1 "Hello, Greatmother. Garrosh told me that you wanted to speak with me."
#define GOSSIP_HGG2 "Garrosh is beyond redemption, Greatmother. I fear that in helping the Mag'har, I have convinced Garrosh that he is unfit to lead."
#define GOSSIP_SGG1 "You raised all of the orcs here, Greatmother?"
#define GOSSIP_SGG2 "Do you believe that?"
#define GOSSIP_SGG3 "What can be done? I have tried many different things. I have done my best to help the people of Nagrand. Each time I have approached Garrosh, he has dismissed me."
#define GOSSIP_SGG4 "Left? How can you choose to leave?"
#define GOSSIP_SGG5 "What is this duty?"
#define GOSSIP_SGG6 "Is there anything I can do for you, Greatmother?"
#define GOSSIP_SGG7 "I have done all that I could, Greatmother. I thank you for your kind words."
#define GOSSIP_SGG8 "Greatmother, you are the mother of Durotan?"
#define GOSSIP_SGG9 "Greatmother, I never had the honor. Durotan died long before my time, but his heroics are known to all on my world. The orcs of Azeroth reside in a place known as Durotar, named after your son. And ... (You take a moment to breathe and think through what you are about to tell the Greatmother.)"
#define GOSSIP_SGG10 "It is my Warchief, Greatmother. The leader of my people. From my world. He ... He is the son of Durotan. He is your grandchild."
#define GOSSIP_SGG11 "I will return to Azeroth at once, Greatmother."

enum GreatmotherGeyah
{
	QUEST_THERE_IS_NO_HOPE      = 10172,
	QUEST_VISIT_THE_GREATMOTHER = 10044,
};

class npc_greatmother_geyah : public CreatureScript
{
public:
    npc_greatmother_geyah() : CreatureScript("npc_greatmother_geyah") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_VISIT_THE_GREATMOTHER) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HGG1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        }
        else if (player->GetQuestStatus(QUEST_THERE_IS_NO_HOPE) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HGG2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        }
        else
			player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SGG1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SGG2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SGG3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 4:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SGG4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 5:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SGG5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 6:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SGG6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 7:
                player->AreaExploredOrEventHappens(QUEST_VISIT_THE_GREATMOTHER);
                player->CLOSE_GOSSIP_MENU();
                break;
            case GOSSIP_ACTION_INFO_DEF + 10:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SGG7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 11:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SGG8, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 12:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SGG9, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 13:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SGG10, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 14:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SGG11, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 15:
                player->AreaExploredOrEventHappens(QUEST_THERE_IS_NO_HOPE);
                player->CLOSE_GOSSIP_MENU();
                break;
        }
        return true;
    }
};

#define GOSSIP_HLB "I have killed many of your ogres, Lantresor. I have no fear."
#define GOSSIP_SLB1 "Should I know? You look like an orc to me."
#define GOSSIP_SLB2 "And the other half?"
#define GOSSIP_SLB3 "I have heard of your kind, but I never thought to see the day when I would meet a half-breed."
#define GOSSIP_SLB4 "My apologies. I did not mean to offend. I am here on behalf of my people."
#define GOSSIP_SLB5 "My people ask that you pull back your Boulderfist ogres and cease all attacks on our territories. In return, we will also pull back our forces."
#define GOSSIP_SLB6 "We will fight you until the end, then, Lantresor. We will not stand idly by as you pillage our towns and kill our people."
#define GOSSIP_SLB7 "What do I need to do?"

enum Lantresor
{
	QUEST_DIPLOMATIC_MEASURES_H = 10107,
	QUEST_DIPLOMATIC_MEASURES_A = 10108,
};

class npc_lantresor_of_the_blade : public CreatureScript
{
public:
    npc_lantresor_of_the_blade() : CreatureScript("npc_lantresor_of_the_blade") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_DIPLOMATIC_MEASURES_H) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_DIPLOMATIC_MEASURES_A) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HLB, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(9361, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SLB1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(9362, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SLB2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(9363, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SLB3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                player->SEND_GOSSIP_MENU(9364, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SLB4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
                player->SEND_GOSSIP_MENU(9365, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SLB5, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
                player->SEND_GOSSIP_MENU(9366, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+5:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SLB6, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
                player->SEND_GOSSIP_MENU(9367, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+6:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SLB7, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
                player->SEND_GOSSIP_MENU(9368, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+7:
                player->SEND_GOSSIP_MENU(9369, creature->GetGUID());

                if (player->GetQuestStatus(QUEST_DIPLOMATIC_MEASURES_H) == QUEST_STATUS_INCOMPLETE)
                    player->AreaExploredOrEventHappens(QUEST_DIPLOMATIC_MEASURES_H);

                if (player->GetQuestStatus(QUEST_DIPLOMATIC_MEASURES_A) == QUEST_STATUS_INCOMPLETE)
                    player->AreaExploredOrEventHappens(QUEST_DIPLOMATIC_MEASURES_A);
				break;
        }
        return true;
    }
};

/*#####
## npc_maghar_captive
#####*/

enum MagharCaptive
{
    SAY_MAG_START               = -1000482,
    SAY_MAG_NO_ESCAPE           = -1000483,
    SAY_MAG_MORE                = -1000484,
    SAY_MAG_MORE_REPLY          = -1000485,
    SAY_MAG_LIGHTNING           = -1000486,
    SAY_MAG_SHOCK               = -1000487,
    SAY_MAG_COMPLETE            = -1000488,

    SPELL_CHAIN_LIGHTNING       = 16006,
    SPELL_EARTHBIND_TOTEM       = 15786,
    SPELL_FROST_SHOCK           = 12548,
    SPELL_HEALING_WAVE          = 12491,

    QUEST_TOTEM_KARDASH_H       = 9868,

    NPC_MURK_RAIDER             = 18203,
    NPC_MURK_BRUTE              = 18211,
    NPC_MURK_SCAVENGER          = 18207,
    NPC_MURK_PUTRIFIER          = 18202,
};

static float m_afAmbushA[]= {-1568.805786f, 8533.873047f, 1.958f};
static float m_afAmbushB[]= {-1491.554321f, 8506.483398f, 1.248f};

class npc_maghar_captive : public CreatureScript
{
public:
    npc_maghar_captive() : CreatureScript("npc_maghar_captive") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_TOTEM_KARDASH_H)
        {
            if (npc_maghar_captiveAI* EscortAI = dynamic_cast<npc_maghar_captiveAI*>(creature->AI()))
            {
                creature->SetStandState(UNIT_STAND_STATE_STAND);
                creature->SetCurrentFaction(232);
                EscortAI->Start(true, false, player->GetGUID(), quest);
                DoSendQuantumText(SAY_MAG_START, creature);
                creature->SummonCreature(NPC_MURK_RAIDER, m_afAmbushA[0]+2.5f, m_afAmbushA[1]-2.5f, m_afAmbushA[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                creature->SummonCreature(NPC_MURK_PUTRIFIER, m_afAmbushA[0]-2.5f, m_afAmbushA[1]+2.5f, m_afAmbushA[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                creature->SummonCreature(NPC_MURK_BRUTE, m_afAmbushA[0], m_afAmbushA[1], m_afAmbushA[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            }
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_maghar_captiveAI(creature);
    }

    struct npc_maghar_captiveAI : public npc_escortAI
    {
        npc_maghar_captiveAI(Creature* creature) : npc_escortAI(creature) { Reset(); }

        uint32 ChainLightningTimer;
        uint32 HealTimer;
        uint32 FrostShockTimer;

        void Reset()
        {
            ChainLightningTimer = 1000;
            HealTimer = 0;
            FrostShockTimer = 6000;
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoCast(me, SPELL_EARTHBIND_TOTEM, false);
        }

        void WaypointReached(uint32 uiPointId)
        {
            switch (uiPointId)
            {
                case 7:
                    DoSendQuantumText(SAY_MAG_MORE, me);

                    if (Creature* temp = me->SummonCreature(NPC_MURK_PUTRIFIER, m_afAmbushB[0], m_afAmbushB[1], m_afAmbushB[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000))
                        DoSendQuantumText(SAY_MAG_MORE_REPLY, temp);

                    me->SummonCreature(NPC_MURK_PUTRIFIER, m_afAmbushB[0]-2.5f, m_afAmbushB[1]-2.5f, m_afAmbushB[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);

                    me->SummonCreature(NPC_MURK_SCAVENGER, m_afAmbushB[0]+2.5f, m_afAmbushB[1]+2.5f, m_afAmbushB[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                    me->SummonCreature(NPC_MURK_SCAVENGER, m_afAmbushB[0]+2.5f, m_afAmbushB[1]-2.5f, m_afAmbushB[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                    break;
                case 16:
                    DoSendQuantumText(SAY_MAG_COMPLETE, me);

                    if (Player* player = GetPlayerForEscort())
                        player->GroupEventHappens(QUEST_TOTEM_KARDASH_H, me);

                    SetRun();
                    break;
            }
        }

        void JustSummoned(Creature* summoned)
        {
            if (summoned->GetEntry() == NPC_MURK_BRUTE)
                DoSendQuantumText(SAY_MAG_NO_ESCAPE, summoned);

            if (summoned->IsTotem())
                return;

            summoned->SetWalk(false);
            summoned->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
            summoned->AI()->AttackStart(me);

        }

        void SpellHitTarget(Unit* /*target*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_CHAIN_LIGHTNING)
            {
                if (rand()%10)
                    return;

                DoSendQuantumText(SAY_MAG_LIGHTNING, me);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);
            if (!me->GetVictim())
                return;

            if (ChainLightningTimer <= diff)
            {
                DoCastVictim(SPELL_CHAIN_LIGHTNING);
                ChainLightningTimer = urand(7000, 14000);
            }
            else ChainLightningTimer -= diff;

            if (HealthBelowPct(30))
            {
                if (HealTimer <= diff)
                {
                    DoCast(me, SPELL_HEALING_WAVE);
                    HealTimer = 5000;
                }
                else HealTimer -= diff;
            }

            if (FrostShockTimer <= diff)
            {
                DoCastVictim(SPELL_FROST_SHOCK);
                FrostShockTimer = urand(7500, 15000);
            }
            else FrostShockTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

/*######
## npc_creditmarker_visist_with_ancestors
######*/

class npc_creditmarker_visit_with_ancestors : public CreatureScript
{
public:
    npc_creditmarker_visit_with_ancestors() : CreatureScript("npc_creditmarker_visit_with_ancestors") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_creditmarker_visit_with_ancestorsAI(creature);
    }

    struct npc_creditmarker_visit_with_ancestorsAI : public QuantumBasicAI
    {
        npc_creditmarker_visit_with_ancestorsAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset(){}

        void EnterToBattle(Unit* /*who*/){}

        void MoveInLineOfSight(Unit* who)
        {
            if (!who)
                return;

            if (who->GetTypeId() == TYPE_ID_PLAYER)
            {
                if (CAST_PLR(who)->GetQuestStatus(10085) == QUEST_STATUS_INCOMPLETE)
                {
                    uint32 creditMarkerId = me->GetEntry();
                    if ((creditMarkerId >= 18840) && (creditMarkerId <= 18843))
                    {
                        // 18840: Sunspring, 18841: Laughing, 18842: Garadar, 18843: Bleeding
                        if (!CAST_PLR(who)->GetReqKillOrCastCurrentCount(10085, creditMarkerId))
                            CAST_PLR(who)->KilledMonsterCredit(creditMarkerId, 0);
                    }
                }
            }
        }
    };
};

/*######
## mob_sparrowhawk
######*/

enum SparrowhankSpells
{
	SPELL_SPARROWHAWK_NET           = 39810,
	SPELL_ITEM_CAPTIVE_SPARROWHAWK  = 39812,
};

class mob_sparrowhawk : public CreatureScript
{
public:
    mob_sparrowhawk() : CreatureScript("mob_sparrowhawk") { }

    struct mob_sparrowhawkAI : public QuantumBasicAI
    {
        mob_sparrowhawkAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CheckTimer;
        uint64 PlayerGUID;
        bool fleeing;

        void Reset()
        {
            me->RemoveAurasDueToSpell(SPELL_SPARROWHAWK_NET);
            CheckTimer = 1000;
            PlayerGUID = 0;
            fleeing = false;
        }
        void AttackStart(Unit* who)
        {
            if (PlayerGUID)
                return;

            QuantumBasicAI::AttackStart(who);
        }

        void EnterToBattle(Unit* /*who*/){}

        void MoveInLineOfSight(Unit* who)
        {
            if (!who || PlayerGUID)
                return;

            if (!PlayerGUID && who->GetTypeId() == TYPE_ID_PLAYER && me->IsWithinDistInMap(who, 30) && CAST_PLR(who)->GetQuestStatus(10987) == QUEST_STATUS_INCOMPLETE)
            {
                PlayerGUID = who->GetGUID();
                return;
            }

            QuantumBasicAI::MoveInLineOfSight(who);
        }

        void UpdateAI(const uint32 diff)
        {
            if (CheckTimer <= diff)
            {
                if (PlayerGUID)
                {
                    if (fleeing && me->GetMotionMaster()->GetCurrentMovementGeneratorType() != FLEEING_MOTION_TYPE)
                        fleeing = false;

                    Player* player = Unit::GetPlayer(*me, PlayerGUID);
                    if (player && me->IsWithinDistInMap(player, 30))
                    {
                        if (!fleeing)
                        {
                            me->DeleteThreatList();
                            me->GetMotionMaster()->MoveFleeing(player);
                            fleeing = true;
                        }
                    }
                    else if (fleeing)
                    {
                        me->GetMotionMaster()->MovementExpired(false);
                        PlayerGUID = 0;
                        fleeing = false;
                    }
                }
                CheckTimer = 1000;
            }
			else CheckTimer -= diff;

            if (PlayerGUID)
                return;

            QuantumBasicAI::UpdateAI(diff);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (caster->GetTypeId() == TYPE_ID_PLAYER)
            {
                if (spell->Id == SPELL_SPARROWHAWK_NET && CAST_PLR(caster)->GetQuestStatus(10987) == QUEST_STATUS_INCOMPLETE)
                {
                    DoCast(caster, SPELL_ITEM_CAPTIVE_SPARROWHAWK, true);
                    me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                }
            }
            return;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_sparrowhawkAI(creature);
    }
};

enum CorkiData
{
	// first quest
	QUEST_HELP                                    = 9923,
	NPC_CORKI                                     = 18445,
	NPC_CORKI_CREDIT_1                            = 18369,
	GO_CORKIS_PRISON                              = 182349,
	CORKI_SAY_THANKS                              = -1800071,
	// 2nd quest
	QUEST_CORKIS_GONE_MISSING_AGAIN               = 9924,
	NPC_CORKI_2                                   = 20812,
	GO_CORKIS_PRISON_2                            = 182350,
	CORKI_SAY_PROMISE                             = -1800072,
	// 3rd quest
	QUEST_CHOWAR_THE_PILLAGER                     = 9955,
	NPC_CORKI_3                                   = 18369,
	NPC_CORKI_CREDIT_3                            = 18444,
	GO_CORKIS_PRISON_3                            = 182521,
	CORKI_SAY_LAST                                = -1800073,
};

class go_corkis_prison : public GameObjectScript
{
public:
  go_corkis_prison() : GameObjectScript("go_corkis_prison") { }

  bool OnGossipHello(Player* player, GameObject* go)
  {
      if (go->GetEntry() == GO_CORKIS_PRISON)
      {
          if (Creature* corki = go->FindCreatureWithDistance(NPC_CORKI, 25.0f, true))
          {
              go->SetGoState(GO_STATE_READY);
              corki->GetMotionMaster()->MovePoint(1, go->GetPositionX()+5, go->GetPositionY(), go->GetPositionZ());
              if (player)
                  player->KilledMonsterCredit(NPC_CORKI_CREDIT_1, 0);
          }
      }
      if (go->GetEntry() == GO_CORKIS_PRISON_2)
      {
          if (Creature* corki = go->FindCreatureWithDistance(NPC_CORKI_2, 25.0f, true))
          {
              go->SetGoState(GO_STATE_READY);
              corki->GetMotionMaster()->MovePoint(1, go->GetPositionX()-5, go->GetPositionY(), go->GetPositionZ());
              if (player)
                  player->KilledMonsterCredit(NPC_CORKI_2, 0);
          }
      }
      if (go->GetEntry() == GO_CORKIS_PRISON_3)
      {
          if (Creature* corki = go->FindCreatureWithDistance(NPC_CORKI_3, 25.0f, true))
          {
              go->SetGoState(GO_STATE_READY);
              corki->GetMotionMaster()->MovePoint(1, go->GetPositionX()+4, go->GetPositionY(), go->GetPositionZ());
              if (player)
                  player->KilledMonsterCredit(NPC_CORKI_CREDIT_3, 0);
          }
      }
	  return true;
   }
};

class npc_corki : public CreatureScript
{
public:
  npc_corki() : CreatureScript("npc_corki") { }

  CreatureAI* GetAI(Creature* creature) const
  {
      return new npc_corkiAI(creature);
  }

  struct npc_corkiAI : public QuantumBasicAI
  {
      npc_corkiAI(Creature* creature) : QuantumBasicAI(creature) {}

      uint32 SayTimer;
      bool ReleasedFromCage;

      void Reset()
      {
          SayTimer = 5000;
          ReleasedFromCage = false;
      }

      void UpdateAI(uint32 const diff)
      {
          if (ReleasedFromCage)
          {
              if (SayTimer <= diff)
              {
                  me->DespawnAfterAction();

                  ReleasedFromCage = false;
              }
              else SayTimer -= diff;
          }
      }

      void MovementInform(uint32 type, uint32 id)
      {
          if (type == POINT_MOTION_TYPE && id == 1)
          {
              SayTimer = 5000;
              ReleasedFromCage = true;

              if (me->GetEntry() == NPC_CORKI)
                  DoSendQuantumText(CORKI_SAY_THANKS, me);

              if (me->GetEntry() == NPC_CORKI_2)
                  DoSendQuantumText(CORKI_SAY_PROMISE, me);

              if (me->GetEntry() == NPC_CORKI_3)
                  DoSendQuantumText(CORKI_SAY_LAST, me);
          }
      };
   };
};

/*#####
## npc_kurenai_captive
#####*/

enum KurenaiCaptive
{
    SAY_KUR_START                   = 0,
    SAY_KUR_NO_ESCAPE               = 1,
    SAY_KUR_MORE                    = 2,
    SAY_KUR_MORE_TWO                = 3,
    SAY_KUR_LIGHTNING               = 4,
    SAY_KUR_SHOCK                   = 5,
    SAY_KUR_COMPLETE                = 6,

    SPELL_KUR_CHAIN_LIGHTNING       = 16006,
    SPELL_KUR_EARTHBIND_TOTEM       = 15786,
    SPELL_KUR_FROST_SHOCK           = 12548,
    SPELL_KUR_HEALING_WAVE          = 12491,

    QUEST_TOTEM_KARDASH_A           = 9879,

    NPC_KUR_MURK_RAIDER             = 18203,
    NPC_KUR_MURK_BRUTE              = 18211,
    NPC_KUR_MURK_SCAVENGER          = 18207,
    NPC_KUR_MURK_PUTRIFIER          = 18202,
};

static float kurenaiAmbushA[]= {-1568.805786f, 8533.873047f, 1.958f};
static float kurenaiAmbushB[]= {-1491.554321f, 8506.483398f, 1.248f};

class npc_kurenai_captive : public CreatureScript
{
public:
    npc_kurenai_captive() : CreatureScript("npc_kurenai_captive") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_TOTEM_KARDASH_A)
        {
            if (npc_kurenai_captiveAI* EscortAI = dynamic_cast<npc_kurenai_captiveAI*>(creature->AI()))
            {
                creature->SetStandState(UNIT_STAND_STATE_STAND);
                EscortAI->Start(true, false, player->GetGUID(), quest);
                DoSendQuantumText(SAY_KUR_START, creature);
                creature->SummonCreature(NPC_KUR_MURK_RAIDER, kurenaiAmbushA[0]+2.5f, kurenaiAmbushA[1]-2.5f, kurenaiAmbushA[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                creature->SummonCreature(NPC_KUR_MURK_BRUTE, kurenaiAmbushA[0]-2.5f, kurenaiAmbushA[1]+2.5f, kurenaiAmbushA[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                creature->SummonCreature(NPC_KUR_MURK_SCAVENGER, kurenaiAmbushA[0], kurenaiAmbushA[1], kurenaiAmbushA[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
            }
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_kurenai_captiveAI(creature);
    }

    struct npc_kurenai_captiveAI : public npc_escortAI
    {
        npc_kurenai_captiveAI(Creature* creature) : npc_escortAI(creature) { }

        uint32 ChainLightningTimer;
        uint32 HealTimer;
        uint32 FrostShockTimer;

        void Reset()
        {
            ChainLightningTimer = 1000;
            HealTimer = 0;
            FrostShockTimer = 6000;
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoCast(me, SPELL_KUR_EARTHBIND_TOTEM, false);
        }

        void JustDied(Unit* /*killer*/)
        {
            if (!HasEscortState(STATE_ESCORT_ESCORTING))
                return;

            if (Player* player = GetPlayerForEscort())
            {
                if (player->GetQuestStatus(QUEST_TOTEM_KARDASH_A) != QUEST_STATUS_COMPLETE)
                    player->FailQuest(QUEST_TOTEM_KARDASH_A);
            }
        }

        void WaypointReached(uint32 PointId)
        {
            switch(PointId)
            {
                case 3:
                {
                    Talk(SAY_KUR_MORE);

                    if (me->SummonCreature(NPC_KUR_MURK_PUTRIFIER, kurenaiAmbushB[0], kurenaiAmbushB[1], kurenaiAmbushB[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000))
                        Talk(SAY_KUR_MORE_TWO);

                    me->SummonCreature(NPC_KUR_MURK_PUTRIFIER, kurenaiAmbushB[0]-2.5f, kurenaiAmbushB[1]-2.5f, kurenaiAmbushB[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                    me->SummonCreature(NPC_KUR_MURK_SCAVENGER, kurenaiAmbushB[0]+2.5f, kurenaiAmbushB[1]+2.5f, kurenaiAmbushB[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                    me->SummonCreature(NPC_KUR_MURK_SCAVENGER, kurenaiAmbushB[0]+2.5f, kurenaiAmbushB[1]-2.5f, kurenaiAmbushB[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
                    break;
                }
                case 7:
                {
                    Talk(SAY_KUR_COMPLETE);

                    if (Player* player = GetPlayerForEscort())
                        player->GroupEventHappens(QUEST_TOTEM_KARDASH_A, me);

                    SetRun();
                    break;
                }
            }
        }

        void JustSummoned(Creature* summoned)
        {
            if (summoned->GetEntry() == NPC_KUR_MURK_BRUTE)
                Talk(SAY_KUR_NO_ESCAPE);

            // This function is for when we summoned enemies to fight - so that does NOT mean we should make our totem count in this!
            if (summoned->IsTotem())
                return;

            summoned->SetWalk(false);
            summoned->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
            summoned->AI()->AttackStart(me);
        }

        void SpellHitTarget(Unit* /*target*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_KUR_CHAIN_LIGHTNING)
            {
                if (rand()%30)
                    return;

                Talk(SAY_KUR_LIGHTNING);
            }

            if (spell->Id == SPELL_KUR_FROST_SHOCK)
            {
                if (rand()%30)
                    return;

                Talk(SAY_KUR_SHOCK);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (ChainLightningTimer <= diff)
            {
                DoCastVictim(SPELL_KUR_CHAIN_LIGHTNING);
                ChainLightningTimer = urand(7000,14000);
            }
			else ChainLightningTimer -= diff;

            if (HealthBelowPct(30))
            {
                if (HealTimer <= diff)
                {
                    DoCast(me, SPELL_KUR_HEALING_WAVE);
                    HealTimer = 5000;
                }
				else HealTimer -= diff;
            }

            if (FrostShockTimer <= diff)
            {
                DoCastVictim(SPELL_KUR_FROST_SHOCK);
                FrostShockTimer = urand(7500,15000);
            }
			else FrostShockTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };
};

/*######
## go_warmaul_prison
######*/

enum FindingTheSurvivorsData
{
    QUEST_FINDING_THE_SURVIVORS                     = 9948,
    NPC_MAGHAR_PRISONER                             = 18428,
    SAY_FREE                                        = 0,
};

class go_warmaul_prison : public GameObjectScript
{
    public:
        go_warmaul_prison() : GameObjectScript("go_warmaul_prison") { }

        bool OnGossipHello(Player* player, GameObject* go)
        {
            if (player->GetQuestStatus(QUEST_FINDING_THE_SURVIVORS) != QUEST_STATUS_INCOMPLETE)
                return false;

            if (Creature* prisoner = go->FindCreatureWithDistance(NPC_MAGHAR_PRISONER, 5.0f))
            {
				go->UseDoorOrButton();
				player->KilledMonsterCredit(NPC_MAGHAR_PRISONER, 0);

				prisoner->AI()->Talk(SAY_FREE, player->GetGUID());
				prisoner->DespawnAfterAction(6*IN_MILLISECONDS);
            }
            return true;
        }
};

enum BurningBladePure
{
	SPELL_PALCE_MAGHAR_BATTLE_STANDART = 32205,
	NPC_KILL_CREDIT_1                  = 18305,
	NPC_KILL_CREDIT_2                  = 18306,
	NPC_KILL_CREDIT_3                  = 18307,
};

class npc_burning_blade_pure01 : public CreatureScript
{
public:
    npc_burning_blade_pure01() : CreatureScript("npc_burning_blade_pure01") { }

    struct npc_burning_blade_pure01AI : public QuantumBasicAI
    {
        npc_burning_blade_pure01AI(Creature* creature) : QuantumBasicAI(creature) {}

		void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_PALCE_MAGHAR_BATTLE_STANDART)
            {
				if (Player* player = caster->ToPlayer())
				{
					caster->ToPlayer()->KilledMonsterCredit(NPC_KILL_CREDIT_1, 0);
				}
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_burning_blade_pure01AI(creature);
    }
};

class npc_burning_blade_pure02 : public CreatureScript
{
public:
    npc_burning_blade_pure02() : CreatureScript("npc_burning_blade_pure02") { }

    struct npc_burning_blade_pure02AI : public QuantumBasicAI
    {
        npc_burning_blade_pure02AI(Creature* creature) : QuantumBasicAI(creature) {}

		void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_PALCE_MAGHAR_BATTLE_STANDART)
            {
				if (Player* player = caster->ToPlayer())
				{
					caster->ToPlayer()->KilledMonsterCredit(NPC_KILL_CREDIT_2, 0);
				}
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_burning_blade_pure02AI(creature);
    }
};

class npc_burning_blade_pure03 : public CreatureScript
{
public:
    npc_burning_blade_pure03() : CreatureScript("npc_burning_blade_pure03") { }

    struct npc_burning_blade_pure03AI : public QuantumBasicAI
    {
        npc_burning_blade_pure03AI(Creature* creature) : QuantumBasicAI(creature) {}

		void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_PALCE_MAGHAR_BATTLE_STANDART)
            {
				if (Player* player = caster->ToPlayer())
				{
					caster->ToPlayer()->KilledMonsterCredit(NPC_KILL_CREDIT_3, 0);
				}
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_burning_blade_pure03AI(creature);
    }
};

void AddSC_nagrand()
{
    new mob_shattered_rumbler();
    new mob_lump();
    new npc_altruis_the_sufferer();
    new npc_greatmother_geyah();
    new npc_lantresor_of_the_blade();
    new npc_maghar_captive();
    new npc_creditmarker_visit_with_ancestors();
    new mob_sparrowhawk();
    new npc_corki();
    new go_corkis_prison();
    new npc_kurenai_captive();
    new go_warmaul_prison();
	new npc_burning_blade_pure01();
	new npc_burning_blade_pure02();
	new npc_burning_blade_pure03();
}