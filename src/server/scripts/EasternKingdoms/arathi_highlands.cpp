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

enum Phizzlethorpe
{
    SAY_PROGRESS_1      = -1000264,
    SAY_PROGRESS_2      = -1000265,
    SAY_PROGRESS_3      = -1000266,
    EMOTE_PROGRESS_4    = -1000267,
    SAY_AGGRO           = -1000268,
    SAY_PROGRESS_5      = -1000269,
    SAY_PROGRESS_6      = -1000270,
    SAY_PROGRESS_7      = -1000271,
    EMOTE_PROGRESS_8    = -1000272,
    SAY_PROGRESS_9      = -1000273,

    QUEST_SUNKEN_TREASURE   = 665,

    NPC_VENGEFUL_SURGE      = 2776,
};

class npc_professor_phizzlethorpe : public CreatureScript
{
    public:
        npc_professor_phizzlethorpe() : CreatureScript("npc_professor_phizzlethorpe") { }

        struct npc_professor_phizzlethorpeAI : public npc_escortAI
        {
            npc_professor_phizzlethorpeAI(Creature* creature) : npc_escortAI(creature) {}

            void WaypointReached(uint32 uiPointId)
            {
                Player* player = GetPlayerForEscort();

                if (!player)
                    return;

                switch (uiPointId)
                {
                case 4:
					DoSendQuantumText(SAY_PROGRESS_2, me, player);
					break;
                case 5:
					DoSendQuantumText(SAY_PROGRESS_3, me, player);
					break;
                case 8:
					DoSendQuantumText(EMOTE_PROGRESS_4, me);
					break;
                case 9:
				{
					me->SummonCreature(NPC_VENGEFUL_SURGE, -2052.96f, -2142.49f, 20.15f, 1.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);
					me->SummonCreature(NPC_VENGEFUL_SURGE, -2052.96f, -2142.49f, 20.15f, 1.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);
					break;
				}
                case 10:
					DoSendQuantumText(SAY_PROGRESS_5, me, player);
					break;
                case 11:
                    DoSendQuantumText(SAY_PROGRESS_6, me, player);
                    SetRun();
                    break;
                case 19:
					DoSendQuantumText(SAY_PROGRESS_7, me, player);
					break;
                case 20:
                    DoSendQuantumText(EMOTE_PROGRESS_8, me);
                    DoSendQuantumText(SAY_PROGRESS_9, me, player);
                    if (player)
                        CAST_PLR(player)->GroupEventHappens(QUEST_SUNKEN_TREASURE, me);
                    break;
                }
            }

            void JustSummoned(Creature* summoned)
            {
                summoned->AI()->AttackStart(me);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                DoSendQuantumText(SAY_AGGRO, me);
            }

            void UpdateAI(const uint32 diff)
            {
                npc_escortAI::UpdateAI(diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_professor_phizzlethorpeAI(creature);
        }

        bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
        {
            if (quest->GetQuestId() == QUEST_SUNKEN_TREASURE)
            {
                DoSendQuantumText(SAY_PROGRESS_1, creature, player);
                if (npc_escortAI* EscortAI = CAST_AI(npc_professor_phizzlethorpeAI, (creature->AI())))
                    EscortAI->Start(false, false, player->GetGUID(), quest);

                creature->SetCurrentFaction(113);
            }
            return true;
        }
};

void AddSC_arathi_highlands()
{
    new npc_professor_phizzlethorpe();
}