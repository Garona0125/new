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

enum Stilwell
{
	SAY_DS_START      = -1000293,
    SAY_DS_DOWN_1     = -1000294,
    SAY_DS_DOWN_2     = -1000295,
    SAY_DS_DOWN_3     = -1000296,
    SAY_DS_PROLOGUE   = -1000297,

    SPELL_SHOOT       = 6660,
    QUEST_TOME_VALOR  = 1651,
    NPC_DEFIAS_RAIDER = 6180,
};

class npc_daphne_stilwell : public CreatureScript
{
public:
    npc_daphne_stilwell() : CreatureScript("npc_daphne_stilwell") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_TOME_VALOR)
        {
			DoSendQuantumText(SAY_DS_START, creature);

            if (npc_escortAI* EscortAI = CAST_AI(npc_daphne_stilwell::npc_daphne_stilwellAI, creature->AI()))
                EscortAI->Start(true, true, player->GetGUID());
        }
        return true;
    }

    struct npc_daphne_stilwellAI : public npc_escortAI
    {
        npc_daphne_stilwellAI(Creature* creature) : npc_escortAI(creature) {}

        uint32 WPHolder;

        void Reset()
        {
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, 0);
        }

		void EnterToBattle(Unit* /*who*/){}

        void WaypointReached(uint32 point)
        {
            Player* player = GetPlayerForEscort();

            if (!player)
                return;

            WPHolder = point;

            switch (point)
            {
                case 4:
					me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_RANGED_WEAPON);
                    me->HandleEmoteCommand(EMOTE_STATE_USE_STANDING_NO_SHEATHE);
                    break;
                case 7:
					DoSendQuantumText(SAY_DS_DOWN_1, me);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11450.836f, 1569.755f, 54.267f, 4.230f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11449.697f, 1569.124f, 54.421f, 4.206f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11448.237f, 1568.307f, 54.620f, 4.206f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    break;
                case 8:
					DoSendQuantumText(SAY_DS_DOWN_2, me);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11450.836f, 1569.755f, 54.267f, 4.230f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11449.697f, 1569.124f, 54.421f, 4.206f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11448.237f, 1568.307f, 54.620f, 4.206f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11448.037f, 1570.213f, 54.961f, 4.283f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    break;
                case 9:
					DoSendQuantumText(SAY_DS_DOWN_3, me);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11450.836f, 1569.755f, 54.267f, 4.230f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11449.697f, 1569.124f, 54.421f, 4.206f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11448.237f, 1568.307f, 54.620f, 4.206f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11448.037f, 1570.213f, 54.961f, 4.283f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    me->SummonCreature(NPC_DEFIAS_RAIDER, -11449.018f, 1570.738f, 54.828f, 4.220f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                    break;
                case 10:
                    SetRun(false);
                    break;
                case 11:
                    DoSendQuantumText(SAY_DS_PROLOGUE, me);
                    break;
                case 13:
                    SetEquipmentSlots(true);
					me->SetUInt32Value(UNIT_FIELD_BYTES_2, 0);
                    me->HandleEmoteCommand(EMOTE_STATE_USE_STANDING_NO_SHEATHE);
                    break;
                case 17:
                    player->GroupEventHappens(QUEST_TOME_VALOR, me);
					me->SummonCreature(me->GetEntry(), -11480.8f, 1558.52f, 48.5744f, 4.658f, TEMPSUMMON_TIMED_DESPAWN, 90000);
                    break;
            }
        }

        void AttackStart(Unit* who)
        {
            if (!who)
                return;

            if (me->Attack(who, false))
            {
                me->AddThreat(who, 0.0f);
                me->SetInCombatWith(who);
                who->SetInCombatWith(me);
                me->GetMotionMaster()->MoveChase(who, 30.0f);
            }
        }

        void JustSummoned(Creature* summoned)
        {
            summoned->AI()->AttackStart(me);
        }

        void Update(const uint32 diff)
        {
            //npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

			DoSpellAttackIfReady(SPELL_SHOOT);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_daphne_stilwellAI(creature);
	}
};

enum Traitor
{
	QUEST_DEFIAS_BROTHERHOOD = 155,

	SAY_START                = 0,
	SAY_PROGRESS             = 1,
	SAY_END                  = 2,
	SAY_AGGRO                = 3,
};

class npc_defias_traitor : public CreatureScript
{
public:
    npc_defias_traitor() : CreatureScript("npc_defias_traitor") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_DEFIAS_BROTHERHOOD)
        {
            if (npc_escortAI* EscortAI = CAST_AI(npc_defias_traitor::npc_defias_traitorAI, creature->AI()))
                EscortAI->Start(true, true, player->GetGUID());

			creature->AI()->Talk(SAY_START, player->GetGUID());
        }
        return true;
    }

    struct npc_defias_traitorAI : public npc_escortAI
    {
        npc_defias_traitorAI(Creature* creature) : npc_escortAI(creature)
		{
			Reset();
		}

		void Reset() {}

		void EnterToBattle(Unit* who)
        {
			Talk(SAY_AGGRO, who->GetGUID());
        }

        void WaypointReached(uint32 i)
        {
            Player* player = GetPlayerForEscort();

            if (!player)
                return;

            switch (i)
            {
                case 35:
                    SetRun(false);
                    break;
                case 36:
					Talk(SAY_PROGRESS, player->GetGUID());
                    break;
                case 44:
					Talk(SAY_END, player->GetGUID());
                    {
                        if (player)
                            player->GroupEventHappens(QUEST_DEFIAS_BROTHERHOOD, me);
                    }
                    break;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_defias_traitorAI(creature);
	}
};

void AddSC_westfall()
{
    new npc_daphne_stilwell();
    new npc_defias_traitor();
}