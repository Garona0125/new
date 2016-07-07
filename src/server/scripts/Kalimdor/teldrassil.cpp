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
#include "QuantumFollowerAI.h"

enum Mist
{
    SAY_AT_HOME       = -1000323,
    EMOTE_AT_HOME     = -1000324,
    QUEST_MIST        = 938,
    NPC_ARYNIA        = 3519,
    FACTION_DARNASSUS = 79,
};

class npc_mist : public CreatureScript
{
public:
    npc_mist() : CreatureScript("npc_mist") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_MIST)
		{
            if (npc_mistAI* MistAI = CAST_AI(npc_mist::npc_mistAI, creature->AI()))
                MistAI->StartFollow(player, FACTION_DARNASSUS, quest);
		}
		return true;
    }

    struct npc_mistAI : public FollowerAI
    {
        npc_mistAI(Creature* creature) : FollowerAI(creature) { }

        void Reset(){}

        void MoveInLineOfSight(Unit* who)
        {
            FollowerAI::MoveInLineOfSight(who);

            if (!me->GetVictim() && !HasFollowState(STATE_FOLLOW_COMPLETE) && who->GetEntry() == NPC_ARYNIA)
            {
                if (me->IsWithinDistInMap(who, 10.0f))
                {
                    DoSendQuantumText(SAY_AT_HOME, who);
                    DoComplete();
                }
            }
        }

        void DoComplete()
        {
            DoSendQuantumText(EMOTE_AT_HOME, me);

            Player* player = GetLeaderForFollower();

            if (player && player->GetQuestStatus(QUEST_MIST) == QUEST_STATUS_INCOMPLETE)
				player->GroupEventHappens(QUEST_MIST, me);

            SetFollowComplete();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mistAI(creature);
    }
};

void AddSC_teldrassil()
{
    new npc_mist();
}