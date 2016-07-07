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
#include "QuantumSystemText.h"
#include "SpellScript.h"

enum GreganBrewspewer
{
	QUEST_THE_VIDERE_ELIXIR_1 = 3909,
	QUEST_THE_VIDERE_ELIXIR_2 = 4041,
};

#define GOSSIP_HELLO "Buy somethin', will ya?"

class npc_gregan_brewspewer : public CreatureScript
{
public:
    npc_gregan_brewspewer() : CreatureScript("npc_gregan_brewspewer") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->IsVendor() && player->GetQuestStatus(QUEST_THE_VIDERE_ELIXIR_1) == QUEST_STATUS_INCOMPLETE
			|| player->GetQuestStatus(QUEST_THE_VIDERE_ELIXIR_2) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(2433, creature->GetGUID());
        return true;
	}

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_VENDOR_BROWSE_YOUR_GOODS), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
            player->SEND_GOSSIP_MENU(2434, creature->GetGUID());
        }

        if (action == GOSSIP_ACTION_TRADE)
            player->GetSession()->SendListInventory(creature->GetGUID());

        return true;
    }
};

enum OOX
{
    SAY_OOX_START        = -1000287,
    SAY_OOX_AGGRO1       = -1000288,
    SAY_OOX_AGGRO2       = -1000289,
    SAY_OOX_AMBUSH       = -1000290,
    SAY_OOX_END          = -1000292,

    NPC_YETI             = 7848,
    NPC_GORILLA          = 5260,
    NPC_WOODPAW_REAVER   = 5255,
    NPC_WOODPAW_BRUTE    = 5253,
    NPC_WOODPAW_ALPHA    = 5258,
    NPC_WOODPAW_MYSTIC   = 5254,

    QUEST_RESCUE_OOX22FE = 2767,
    FACTION_ESCORTEE_A   = 774,
    FACTION_ESCORTEE_H   = 775,
};

class npc_oox22fe : public CreatureScript
{
public:
    npc_oox22fe() : CreatureScript("npc_oox22fe") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_RESCUE_OOX22FE)
        {
            DoSendQuantumText(SAY_OOX_START, creature);

            creature->SetStandState(UNIT_STAND_STATE_STAND);

            if (player->GetTeam() == ALLIANCE)
                creature->SetCurrentFaction(FACTION_ESCORTEE_A);

            if (player->GetTeam() == HORDE)
                creature->SetCurrentFaction(FACTION_ESCORTEE_H);

            if (npc_escortAI* EscortAI = CAST_AI(npc_oox22fe::npc_oox22feAI, creature->AI()))
                EscortAI->Start(true, false, player->GetGUID());
        }
        return true;
    }

    struct npc_oox22feAI : public npc_escortAI
    {
        npc_oox22feAI(Creature* creature) : npc_escortAI(creature) { }

        void WaypointReached(uint32 i)
        {
            switch (i)
            {
				// First Ambush(3 Yetis)
                case 11:
                    DoSendQuantumText(SAY_OOX_AMBUSH, me);
                    me->SummonCreature(NPC_YETI, -4841.01f, 1593.91f, 73.42f, 3.98f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                    me->SummonCreature(NPC_YETI, -4837.61f, 1568.58f, 78.21f, 3.13f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                    me->SummonCreature(NPC_YETI, -4841.89f, 1569.95f, 76.53f, 0.68f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                    break;
                // Second Ambush(3 Gorillas)
                case 21:
                    DoSendQuantumText(SAY_OOX_AMBUSH, me);
                    me->SummonCreature(NPC_GORILLA, -4595.81f, 2005.99f, 53.08f, 3.74f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                    me->SummonCreature(NPC_GORILLA, -4597.53f, 2008.31f, 52.70f, 3.78f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                    me->SummonCreature(NPC_GORILLA, -4599.37f, 2010.59f, 52.77f, 3.84f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                    break;
                // Third Ambush(4 Gnolls)
                case 30:
                    DoSendQuantumText(SAY_OOX_AMBUSH, me);
                    me->SummonCreature(NPC_WOODPAW_REAVER, -4425.14f, 2075.87f, 47.77f, 3.77f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                    me->SummonCreature(NPC_WOODPAW_BRUTE, -4426.68f, 2077.98f, 47.57f, 3.77f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                    me->SummonCreature(NPC_WOODPAW_MYSTIC, -4428.33f, 2080.24f, 47.43f, 3.87f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                    me->SummonCreature(NPC_WOODPAW_ALPHA, -4430.04f, 2075.54f, 46.83f, 3.81f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);
                    break;
                case 37:
                    DoSendQuantumText(SAY_OOX_END, me);
                    // Award quest credit
                    if (Player* player = GetPlayerForEscort())
						player->GroupEventHappens(QUEST_RESCUE_OOX22FE, me);
                    break;
            }
        }

        void Reset()
        {
			if (!HasEscortState(STATE_ESCORT_ESCORTING))
				me->SetStandState(UNIT_STAND_STATE_DEAD);
		}

        void EnterToBattle(Unit* /*who*/)
        {
            if (urand(0, 9) > 7)
                DoSendQuantumText(RAND(SAY_OOX_AGGRO1, SAY_OOX_AGGRO2), me);
        }

        void JustSummoned(Creature* summoned)
        {
            summoned->AI()->AttackStart(me);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_oox22feAI(creature);
	}
};

class npc_screecher_spirit : public CreatureScript
{
public:
    npc_screecher_spirit() : CreatureScript("npc_screecher_spirit") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        player->SEND_GOSSIP_MENU(2039, creature->GetGUID());
        player->TalkedToCreature(creature->GetEntry(), creature->GetGUID());
        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
		creature->DespawnAfterAction(3*IN_MILLISECONDS);
        return true;
    }
};

enum GordunniTrap
{
    GO_GORDUNNI_DIRT_MOUND = 144064,
};

class spell_gordunni_trap : public SpellScriptLoader
{
public:
	spell_gordunni_trap() : SpellScriptLoader("spell_gordunni_trap") { }

	class spell_gordunni_trap_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_gordunni_trap_SpellScript);

		void HandleDummy()
		{
			if (Unit* caster = GetCaster())
			{
				if (GameObject* chest = caster->SummonGameObject(GO_GORDUNNI_DIRT_MOUND, caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ(), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0))
				{
					chest->SetSpellId(GetSpellInfo()->Id);
					caster->RemoveGameObject(chest, false);
				}
			}
		}

		void Register()
		{
			OnCast += SpellCastFn(spell_gordunni_trap_SpellScript::HandleDummy);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_gordunni_trap_SpellScript();
	}
};

void AddSC_feralas()
{
    new npc_gregan_brewspewer();
    new npc_oox22fe();
    new npc_screecher_spirit();
	new spell_gordunni_trap();
}