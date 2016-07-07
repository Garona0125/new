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

enum Galen
{
    QUEST_GALENS_ESCAPE     = 1393,
    GO_GALENS_CAGE          = 37118,

	SAY_PERIODIC            = 0,
	SAY_QUEST_ACCEPTED      = 1,
	SAY_ATTACKED            = 2,
	SAY_QUEST_COMPLETE      = 3,
	EMOTE_WHISPER           = 4,
	EMOTE_DISAPPEAR         = 5,
};

class npc_galen_goodward : public CreatureScript
{
public:
    npc_galen_goodward() : CreatureScript("npc_galen_goodward") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_GALENS_ESCAPE)
        {
            CAST_AI(npc_galen_goodward::npc_galen_goodwardAI, creature->AI())->Start(false, false, player->GetGUID());
            creature->SetCurrentFaction(FACTION_ESCORT_N_NEUTRAL_ACTIVE);
			creature->AI()->Talk(SAY_QUEST_ACCEPTED);
        }
        return true;
    }

    struct npc_galen_goodwardAI : public npc_escortAI
    {
        npc_galen_goodwardAI(Creature* creature) : npc_escortAI(creature)
        {
            ResetTimerGalensCageGUID = 0;
            Reset();
        }

        uint64 ResetTimerGalensCageGUID;
        uint32 PeriodicSayTimer;

        void Reset()
        {
            PeriodicSayTimer = 6000;
        }

        void EnterToBattle(Unit* who)
        {
            if (HasEscortState(STATE_ESCORT_ESCORTING))
				Talk(SAY_ATTACKED, who->GetGUID());
        }

        void WaypointStart(uint32 PointId)
        {
            switch (PointId)
            {
            case 0:
			{
				GameObject* cage = NULL;

				if (ResetTimerGalensCageGUID)
				{
					cage = me->GetMap()->GetGameObject(ResetTimerGalensCageGUID);
				}
				else
					cage = GetClosestGameObjectWithEntry(me, GO_GALENS_CAGE, INTERACTION_DISTANCE);
				if (cage)
				{
					cage->UseDoorOrButton();
					ResetTimerGalensCageGUID = cage->GetGUID();
				}
				break;
			}
			case 21:
                Talk(EMOTE_DISAPPEAR);
                break;
            }
        }

        void WaypointReached(uint32 PointId)
        {
            switch (PointId)
            {
            case 0:
                if (GameObject* cage = me->GetMap()->GetGameObject(ResetTimerGalensCageGUID))
                    cage->ResetDoorOrButton();
                break;
            case 20:
                if (Player* player = GetPlayerForEscort())
                {
                    me->SetFacingToObject(player);
					Talk(SAY_QUEST_COMPLETE, player->GetGUID());
					Talk(EMOTE_WHISPER, player->GetGUID());
                    player->GroupEventHappens(QUEST_GALENS_ESCAPE, me);
                }
                SetRun(true);
                break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (HasEscortState(STATE_ESCORT_NONE))
                return;

            if (PeriodicSayTimer < diff)
            {
                if (!HasEscortState(STATE_ESCORT_ESCORTING))
					Talk(SAY_PERIODIC);
                PeriodicSayTimer = 15000;
            }
            else PeriodicSayTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_galen_goodwardAI(creature);
    }
};

void AddSC_swamp_of_sorrows()
{
    new npc_galen_goodward();
}