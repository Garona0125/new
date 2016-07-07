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

enum Calvin
{
    SAY_COMPLETE        = 0,
    SPELL_DRINK         = 2639,
    QUEST_ROGUES_DEALS  = 590,
    FACTION_HOSTILE     = 168,
};

class npc_calvin_montague : public CreatureScript
{
public:
    npc_calvin_montague() : CreatureScript("npc_calvin_montague") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_ROGUES_DEALS)
        {
            creature->SetCurrentFaction(FACTION_HOSTILE);
            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            CAST_AI(npc_calvin_montague::npc_calvin_montagueAI, creature->AI())->AttackStart(player);
        }
        return true;
    }

    struct npc_calvin_montagueAI : public QuantumBasicAI
    {
        npc_calvin_montagueAI(Creature* creature) : QuantumBasicAI(creature) { }

        uint32 Phase;
        uint32 PhaseTimer;
        uint64 PlayerGUID;

        void Reset()
        {
            Phase = 0;
            PhaseTimer = 5000;
            PlayerGUID = 0;

            me->RestoreFaction();

            if (!me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER))
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
        }

        void EnterToBattle(Unit* /*who*/) {}

        void AttackedBy(Unit* attacker)
        {
            if (me->GetVictim() || me->IsFriendlyTo(attacker))
                return;

            AttackStart(attacker);
        }

        void DamageTaken(Unit* DoneBy, uint32 &damage)
        {
            if (damage > me->GetHealth() || me->HealthBelowPctDamaged(15, damage))
            {
                damage = 0;

                me->RestoreFaction();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                me->CombatStop(true);

                Phase = 1;

                if (DoneBy->GetTypeId() == TYPE_ID_PLAYER)
                    PlayerGUID = DoneBy->GetGUID();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (Phase)
            {
                if (PhaseTimer <= diff)
                    PhaseTimer = 7500;
                else
                {
                    PhaseTimer -= diff;
                    return;
                }
                switch (Phase)
                {
                    case 1:
                        Talk(SAY_COMPLETE);
                        ++Phase;
                        break;
                    case 2:
                        if (Player* player = Unit::GetPlayer(*me, PlayerGUID))
                            player->AreaExploredOrEventHappens(QUEST_ROGUES_DEALS);

                        DoCast(me, SPELL_DRINK, true);
                        ++Phase;
                        break;
                    case 3:
                        EnterEvadeMode();
                        break;
                }
                return;
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_calvin_montagueAI(creature);
	}
};

enum Mausoleum
{
    QUEST_ULAG  = 1819,
    NPC_ULAG    = 6390,
    GO_TRIGGER  = 104593,
    GO_DOOR     = 176594,
};

class go_mausoleum_door : public GameObjectScript
{
public:
    go_mausoleum_door() : GameObjectScript("go_mausoleum_door") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/)
    {
        if (player->GetQuestStatus(QUEST_ULAG) != QUEST_STATUS_INCOMPLETE)
            return false;

        if (GameObject* trigger = player->FindGameobjectWithDistance(GO_TRIGGER, 30.0f))
        {
            trigger->SetGoState(GO_STATE_READY);
            player->SummonCreature(NPC_ULAG, 2390.26f, 336.47f, 40.01f, 2.26f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);
            return false;
        }
        return false;
    }
};

class go_mausoleum_trigger : public GameObjectScript
{
public:
    go_mausoleum_trigger() : GameObjectScript("go_mausoleum_trigger") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(QUEST_ULAG) != QUEST_STATUS_INCOMPLETE)
            return false;

        if (GameObject* door = player->FindGameobjectWithDistance(GO_DOOR, 30.0f))
        {
            go->SetGoState(GO_STATE_ACTIVE);
            door->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
            return true;
        }
        return false;
    }
};

void AddSC_tirisfal_glades()
{
    new npc_calvin_montague();
    new go_mausoleum_door();
    new go_mausoleum_trigger();
}