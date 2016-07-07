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
#include "SpellScript.h"
#include "forge_of_souls.h"

enum Events
{
    EVENT_NONE    = 0,
    EVENT_INTRO_1 = 1,
    EVENT_INTRO_2 = 2,
    EVENT_INTRO_3 = 3,
    EVENT_INTRO_4 = 4,
    EVENT_INTRO_5 = 5,
    EVENT_INTRO_6 = 6,
    EVENT_INTRO_7 = 7,
    EVENT_INTRO_8 = 8,
};

#define GOSSIP_SYLVANAS_ITEM "What would you have of me, Banshee Queen?"
#define GOSSIP_JAINA_ITEM    "What would you have of me, my lady?"

enum Yells
{
    SAY_JAINA_INTRO_1     = -1632040,
    SAY_JAINA_INTRO_2     = -1632041,
    SAY_JAINA_INTRO_3     = -1632042,
    SAY_JAINA_INTRO_4     = -1632043,
    SAY_JAINA_INTRO_5     = -1632044,
    SAY_JAINA_INTRO_6     = -1632045,
    SAY_JAINA_INTRO_7     = -1632046,
    SAY_JAINA_INTRO_8     = -1632047,
    SAY_SYLVANAS_INTRO_1  = -1632050,
    SAY_SYLVANAS_INTRO_2  = -1632051,
    SAY_SYLVANAS_INTRO_3  = -1632052,
    SAY_SYLVANAS_INTRO_4  = -1632053,
    SAY_SYLVANAS_INTRO_5  = -1632054,
    SAY_SYLVANAS_INTRO_6  = -1632055,
};

enum Sylvanas
{
	GOSSIP_SPEECHINTRO = 13525,
    ACTION_INTRO,
};

enum Phase
{
    PHASE_NORMAL,
    PHASE_INTRO,
};

class npc_sylvanas_fos : public CreatureScript
{
public:
    npc_sylvanas_fos() : CreatureScript("npc_sylvanas_fos") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->GetEntry() == NPC_JAINA_PART1)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_JAINA_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        else
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SYLVANAS_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->CLOSE_GOSSIP_MENU();

                if (creature->AI())
					creature->AI()->DoAction(ACTION_INTRO);

                break;
        }
        return true;
    }

    struct npc_sylvanas_fosAI : public QuantumBasicAI
    {
        npc_sylvanas_fosAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = me->GetInstanceScript();
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }

        InstanceScript* instance;

        EventMap events;
        Phase phase;

        void Reset()
        {
            events.Reset();
            phase = PHASE_NORMAL;
        }

        void DoAction(const int32 actionId)
        {
            switch (actionId)
            {
                case ACTION_INTRO:
                {
                    phase = PHASE_INTRO;
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    events.Reset();
                    events.ScheduleEvent(EVENT_INTRO_1, 1000);
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (phase == PHASE_INTRO)
            {
                if (!instance)
                    return;

                events.Update(diff);

                switch (events.ExecuteEvent())
                {
                    case EVENT_INTRO_1:
                        DoSendQuantumText(SAY_SYLVANAS_INTRO_1, me);
                        events.ScheduleEvent(EVENT_INTRO_2, 11500);
                        break;
                    case EVENT_INTRO_2:
                        DoSendQuantumText(SAY_SYLVANAS_INTRO_2, me);
                        events.ScheduleEvent(EVENT_INTRO_3, 10500);
                        break;
                    case EVENT_INTRO_3:
                        DoSendQuantumText(SAY_SYLVANAS_INTRO_3, me);
                        events.ScheduleEvent(EVENT_INTRO_4, 9500);
                        break;
                    case EVENT_INTRO_4:
                        DoSendQuantumText(SAY_SYLVANAS_INTRO_4, me);
                        events.ScheduleEvent(EVENT_INTRO_5, 10500);
                        break;
                    case EVENT_INTRO_5:
                        DoSendQuantumText(SAY_SYLVANAS_INTRO_5, me);
                        events.ScheduleEvent(EVENT_INTRO_6, 9500);
                        break;
                    case EVENT_INTRO_6:
                        DoSendQuantumText(SAY_SYLVANAS_INTRO_6, me);
                        phase = PHASE_NORMAL;
                        break;
                }
            }

            if (!UpdateVictim())
                return;

            events.Update(diff);
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sylvanas_fosAI(creature);
    }
};

class npc_jaina_fos : public CreatureScript
{
public:
    npc_jaina_fos() : CreatureScript("npc_jaina_fos") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->GetEntry() == NPC_JAINA_PART1)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_JAINA_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        else
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_SYLVANAS_ITEM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->CLOSE_GOSSIP_MENU();

                if (creature->AI())
                    creature->AI()->DoAction(ACTION_INTRO);
                break;
        }
        return true;
    }

    struct npc_jaina_fosAI: public QuantumBasicAI
    {
        npc_jaina_fosAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = me->GetInstanceScript();
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }

        InstanceScript* instance;

        EventMap events;
        Phase phase;

        void Reset()
        {
            events.Reset();
            phase = PHASE_NORMAL;
        }

        void DoAction(const int32 actionId)
        {
            switch (actionId)
            {
                case ACTION_INTRO:
                {
                    phase = PHASE_INTRO;
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    events.Reset();
                    events.ScheduleEvent(EVENT_INTRO_1, 1000);
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (phase == PHASE_INTRO)
            {
                if (!instance)
                    return;

                events.Update(diff);
                switch (events.ExecuteEvent())
                {
                    case EVENT_INTRO_1:
                        DoSendQuantumText(SAY_JAINA_INTRO_1, me);
                        events.ScheduleEvent(EVENT_INTRO_2, 8000);
                        break;
                    case EVENT_INTRO_2:
                        DoSendQuantumText(SAY_JAINA_INTRO_2, me);
                        events.ScheduleEvent(EVENT_INTRO_3, 8500);
                        break;
                    case EVENT_INTRO_3:
                        DoSendQuantumText(SAY_JAINA_INTRO_3, me);
                        events.ScheduleEvent(EVENT_INTRO_4, 8000);
                        break;
                    case EVENT_INTRO_4:
                        DoSendQuantumText(SAY_JAINA_INTRO_4, me);
                        events.ScheduleEvent(EVENT_INTRO_5, 10000);
                        break;
                    case EVENT_INTRO_5:
                        DoSendQuantumText(SAY_JAINA_INTRO_5, me);
                        events.ScheduleEvent(EVENT_INTRO_6, 8000);
                        break;
                    case EVENT_INTRO_6:
                        DoSendQuantumText(SAY_JAINA_INTRO_6, me);
                        events.ScheduleEvent(EVENT_INTRO_7, 12000);
                        break;
                    case EVENT_INTRO_7:
                        DoSendQuantumText(SAY_JAINA_INTRO_7, me);
                        events.ScheduleEvent(EVENT_INTRO_8, 8000);
                        break;
                    case EVENT_INTRO_8:
                        DoSendQuantumText(SAY_JAINA_INTRO_8, me);
                        phase = PHASE_NORMAL;
                        break;
                }
            }

            if (!UpdateVictim())
                return;

            events.Update(diff);

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jaina_fosAI(creature);
    }
};

enum CrucibleOfSouls
{
	SPELL_CRUCIBLE_OF_SOULS_VISUAL   = 69859,
	SPELL_TEMPER_QUELDELAR           = 69922,
	SPELL_SHADOW_NOVA                = 70663,
	SPELL_RETURN_TEMPERED_QUEL_DELAR = 69956,
};

class npc_crucible_of_souls : public CreatureScript
{
public:
    npc_crucible_of_souls() : CreatureScript("npc_crucible_of_souls") {}

    struct npc_crucible_of_soulsAI : public QuantumBasicAI
    {
        npc_crucible_of_soulsAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_CRUCIBLE_OF_SOULS_VISUAL);
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_TEMPER_QUELDELAR)
            {
				DoCastAOE(SPELL_SHADOW_NOVA);
				me->CastSpell(caster, SPELL_RETURN_TEMPERED_QUEL_DELAR, true);
            }
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crucible_of_soulsAI(creature);
    }
};

void AddSC_forge_of_souls()
{
    new npc_sylvanas_fos();
    new npc_jaina_fos();
	new npc_crucible_of_souls();
}