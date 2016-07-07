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

enum TapokeSlim
{
    QUEST_MISSING_DIPLO_PT11 = 1249,

    FACTION_ENEMY            = 168,

    SPELL_STEALTH            = 1785,
    SPELL_CALL_FRIENDS       = 16457,

    NPC_SLIMS_FRIEND         = 4971,
    NPC_TAPOKE_SLIM_JAHN     = 4962,
};

class npc_tapoke_slim_jahn : public CreatureScript
{
public:
    npc_tapoke_slim_jahn() : CreatureScript("npc_tapoke_slim_jahn") { }

    struct npc_tapoke_slim_jahnAI : public npc_escortAI
    {
        npc_tapoke_slim_jahnAI(Creature* creature) : npc_escortAI(creature) { }

        bool FriendSummoned;

        void Reset()
        {
            if (!HasEscortState(STATE_ESCORT_ESCORTING))
                FriendSummoned = false;
        }

        void WaypointReached(uint32 PointId)
        {
            switch (PointId)
            {
                case 2:
                    if (me->HasStealthAura())
                        me->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);

                    SetRun();
                    me->SetCurrentFaction(FACTION_ENEMY);
                    break;
            }
        }

        void EnterToBattle(Unit* /*who*/)
        {
            Player* player = GetPlayerForEscort();

            if (HasEscortState(STATE_ESCORT_ESCORTING) && !FriendSummoned && player)
            {
                for (uint8 i = 0; i < 3; ++i)
                    DoCast(me, SPELL_CALL_FRIENDS, true);

                FriendSummoned = true;
            }
        }

        void JustSummoned(Creature* summoned)
        {
            if (Player* player = GetPlayerForEscort())
                summoned->AI()->AttackStart(player);
        }

        void AttackedBy(Unit* attacker)
        {
            if (me->GetVictim())
                return;

            if (me->IsFriendlyTo(attacker))
                return;

            AttackStart(attacker);
        }

        void DamageTaken(Unit* /*pDoneBy*/, uint32& Damage)
        {
            if (Damage >= me->GetHealth())
                Damage = me->GetHealth() - 1;

            if (HealthBelowPct(HEALTH_PERCENT_20))
            {
                if (Player* player = GetPlayerForEscort())
                {
                    if (player->GetTypeId() == TYPE_ID_PLAYER)
                        CAST_PLR(player)->GroupEventHappens(QUEST_MISSING_DIPLO_PT11, me);

                    Damage = 0;

                    me->RestoreFaction();
                    me->RemoveAllAuras();
                    me->DeleteThreatList();
                    me->CombatStop(true);
                    SetRun(false);
                    me->DespawnAfterAction(10*IN_MILLISECONDS);
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_tapoke_slim_jahnAI(creature);
	}
};

class npc_mikhail : public CreatureScript
{
public:
    npc_mikhail() : CreatureScript("npc_mikhail") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_MISSING_DIPLO_PT11)
        {
            Creature* Slim = creature->FindCreatureWithDistance(NPC_TAPOKE_SLIM_JAHN, 25.0f);

            if (!Slim)
                return false;

            if (!Slim->HasStealthAura())
                Slim->CastSpell(Slim, SPELL_STEALTH, true);

            if (npc_tapoke_slim_jahn::npc_tapoke_slim_jahnAI* EscortAI = CAST_AI(npc_tapoke_slim_jahn::npc_tapoke_slim_jahnAI, Slim->AI()))
                EscortAI->Start(false, false, player->GetGUID(), quest);
        }
        return false;
    }
};

void AddSC_wetlands()
{
    new npc_tapoke_slim_jahn();
    new npc_mikhail();
}