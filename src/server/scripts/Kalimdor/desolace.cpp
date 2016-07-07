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

enum DyingKodo
{
	SAY_SMEED_HOME                  = 0,

    QUEST_KODO                      = 5561,

    NPC_SMEED                       = 11596,
    NPC_AGED_KODO                   = 4700,
    NPC_DYING_KODO                  = 4701,
    NPC_ANCIENT_KODO                = 4702,
    NPC_TAMED_KODO                  = 11627,

    SPELL_KODO_KOMBO_ITEM           = 18153,
    SPELL_KODO_KOMBO_PLAYER_BUFF    = 18172,
    SPELL_KODO_KOMBO_DESPAWN_BUFF   = 18377,
    SPELL_KODO_KOMBO_GOSSIP         = 18362,
};

class npc_aged_dying_ancient_kodo : public CreatureScript
{
public:
    npc_aged_dying_ancient_kodo() : CreatureScript("npc_aged_dying_ancient_kodo") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->HasAura(SPELL_KODO_KOMBO_PLAYER_BUFF) && creature->HasAura(SPELL_KODO_KOMBO_DESPAWN_BUFF))
        {
            player->TalkedToCreature(creature->GetEntry(), creature->GetGUID());
            player->RemoveAurasDueToSpell(SPELL_KODO_KOMBO_PLAYER_BUFF);

            creature->GetMotionMaster()->MoveIdle();
            creature->RemoveAllAuras();
            creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
            creature->CastSpell(creature, SPELL_KODO_KOMBO_GOSSIP, true);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }
        else
			return false;
    }

    struct npc_aged_dying_ancient_kodoAI : public QuantumBasicAI
    {
        npc_aged_dying_ancient_kodoAI(Creature* creature) : QuantumBasicAI(creature) { Reset(); }

        uint32 DespawnTimer;
        uint32 TimeoutTimer;
        bool Despawn;
        bool Timeout;

        void Reset()
        {
            DespawnTimer = 60000;
            TimeoutTimer = 300000;
            Despawn = false;
            Timeout = false;
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (who->GetEntry() == NPC_SMEED)
            {
                if (me->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
                    return;

                if (me->IsWithinDistInMap(who, 10.0f))
                {
					if (Creature* talker = who->ToCreature())
						talker->AI()->Talk(SAY_SMEED_HOME);
					me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                }
            }
        }

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_KODO_KOMBO_GOSSIP)
            {
                Despawn = true;
                Timeout = false;
            }
            else if (spell->Id == SPELL_KODO_KOMBO_ITEM)
            {
                if (!caster->HasAura(SPELL_KODO_KOMBO_PLAYER_BUFF) && !me->HasAura(SPELL_KODO_KOMBO_DESPAWN_BUFF))
                {
                    caster->CastSpell(caster, SPELL_KODO_KOMBO_PLAYER_BUFF, true);
                    me->UpdateEntry(NPC_TAMED_KODO);
                    me->CastSpell(me, SPELL_KODO_KOMBO_DESPAWN_BUFF, false);
                    Timeout = true;
                    me->CombatStop(true);
                    me->DeleteThreatList();
                    me->GetMotionMaster()->MoveFollow(caster, PET_FOLLOW_DIST, me->GetFollowAngle());
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (Despawn == true && DespawnTimer <= diff)
            {
                Reset();
                me->DespawnAfterAction(0);
                return;
            }
            else if (Despawn == true)
                DespawnTimer -= diff;

            if (Timeout == true && TimeoutTimer <= diff)
            {
                Reset();
                me->DespawnAfterAction(0);
                return;
            }
            else if (Timeout == true)
                TimeoutTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_aged_dying_ancient_kodoAI(creature);
    }
};

enum Iruxos
{
	QUEST_HAND_IRUXOS = 5381,

	NPC_DEMON_SPIRIT  = 11876,
};

class go_iruxos : public GameObjectScript
{
public:
	go_iruxos() : GameObjectScript("go_iruxos") { }

	bool OnGossipHello(Player* player, GameObject* go)
	{
		if (player->GetQuestStatus(QUEST_HAND_IRUXOS) == QUEST_STATUS_INCOMPLETE && !go->FindCreatureWithDistance(NPC_DEMON_SPIRIT, 25.0f, true))
			player->SummonCreature(NPC_DEMON_SPIRIT, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
		return true;
	}
};

enum Dalinda
{
    QUEST_RETURN_TO_VAHLARRIEL = 1440,
};

class npc_dalinda : public CreatureScript
{
public:
    npc_dalinda() : CreatureScript("npc_dalinda") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_RETURN_TO_VAHLARRIEL)
       {
            if (npc_escortAI* EscortAI = CAST_AI(npc_dalinda::npc_dalindaAI, creature->AI()))
            {
                EscortAI->Start(true, false, player->GetGUID());
                creature->SetCurrentFaction(113);
            }
        }
        return true;
    }

    struct npc_dalindaAI : public npc_escortAI
    {
        npc_dalindaAI(Creature* creature) : npc_escortAI(creature) { }

        void WaypointReached(uint32 i)
        {
            Player* player = GetPlayerForEscort();
            switch (i)
            {
                case 1:
                    me->IsStandState();
                    break;
                case 15:
                    if (player)
						player->GroupEventHappens(QUEST_RETURN_TO_VAHLARRIEL, me);
                    break;
            }
        }

		void Reset() {}

        void EnterToBattle(Unit* /*who*/) {}

        void JustDied(Unit* /*killer*/)
        {
            Player* player = GetPlayerForEscort();

            if (player)
                player->FailQuest(QUEST_RETURN_TO_VAHLARRIEL);
            return;
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dalindaAI(creature);
    }
};

enum DemonPortal
{
    NPC_DEMON_GUARDIAN         = 11937,
    QUEST_PORTAL_OF_THE_LEGION = 5581,
};

class go_demon_portal : public GameObjectScript
{
public:
	go_demon_portal() : GameObjectScript("go_demon_portal") { }

	bool OnGossipHello(Player* player, GameObject* go)
	{
		if (player->GetQuestStatus(QUEST_PORTAL_OF_THE_LEGION) == QUEST_STATUS_INCOMPLETE && !go->FindCreatureWithDistance(NPC_DEMON_GUARDIAN, 5.0f, true))
		{
			if (Creature* guardian = player->SummonCreature(NPC_DEMON_GUARDIAN, go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0))
				guardian->AI()->AttackStart(player);
		}
		return true;
	}
};

void AddSC_desolace()
{
    new npc_aged_dying_ancient_kodo();
    new go_iruxos();
    new npc_dalinda();
	new go_demon_portal();
}