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

enum Shenthul
{
    QUEST_SHATTERED_SALUTE = 2460,
};

class npc_shenthul : public CreatureScript
{
public:
    npc_shenthul() : CreatureScript("npc_shenthul") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_SHATTERED_SALUTE)
        {
            CAST_AI(npc_shenthul::npc_shenthulAI, creature->AI())->CanTalk = true;
            CAST_AI(npc_shenthul::npc_shenthulAI, creature->AI())->PlayerGUID = player->GetGUID();
        }
        return true;
    }

    struct npc_shenthulAI : public QuantumBasicAI
    {
        npc_shenthulAI(Creature* creature) : QuantumBasicAI(creature) {}

        bool CanTalk;
        bool CanEmote;

        uint32 SaluteTimer;
        uint32 ResetTimer;

        uint64 PlayerGUID;

        void Reset()
        {
            CanTalk = false;
            CanEmote = false;

            SaluteTimer = 6000;
            ResetTimer = 0;

            PlayerGUID = 0;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (CanEmote)
            {
                if (ResetTimer <= diff)
                {
                    if (Player* player = Unit::GetPlayer(*me, PlayerGUID))
                    {
                        if (player->GetTypeId() == TYPE_ID_PLAYER && player->GetQuestStatus(QUEST_SHATTERED_SALUTE) == QUEST_STATUS_INCOMPLETE)
                            player->FailQuest(QUEST_SHATTERED_SALUTE);
                    }

                    Reset();
                }
				else ResetTimer -= diff;
            }

            if (CanTalk && !CanEmote)
            {
                if (SaluteTimer <= diff)
                {
                    me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                    CanEmote = true;
                    ResetTimer = 60000;
                }
				else SaluteTimer -= diff;
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }

        void ReceiveEmote(Player* player, uint32 emote)
        {
            if (emote == TEXT_EMOTE_SALUTE && player->GetQuestStatus(QUEST_SHATTERED_SALUTE) == QUEST_STATUS_INCOMPLETE)
            {
                if (CanEmote)
                {
                    player->AreaExploredOrEventHappens(QUEST_SHATTERED_SALUTE);
                    Reset();
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shenthulAI(creature);
    }
};

void AddSC_orgrimmar()
{
    new npc_shenthul();
}