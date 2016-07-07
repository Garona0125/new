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

enum Kanati
{
    SAY_KAN_START        = -1000410,

    QUEST_PROTECT_KANATI = 4966,

    NPC_GALAK_ASS        = 10720,
};

Position const GalakLoc =
{
	-4867.387695f, -1357.353760f, -48.226f, 0.0f
};

class npc_kanati : public CreatureScript
{
public:
    npc_kanati() : CreatureScript("npc_kanati") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_PROTECT_KANATI)
		{
            if (npc_kanatiAI* EscortAI = CAST_AI(npc_kanati::npc_kanatiAI, creature->AI()))
                EscortAI->Start(false, false, player->GetGUID(), quest, true);
		}
		return true;
    }

    struct npc_kanatiAI : public npc_escortAI
    {
        npc_kanatiAI(Creature* creature) : npc_escortAI(creature) { }

        void Reset() {}

        void WaypointReached(uint32 waypointId)
        {
            switch (waypointId)
            {
                case 0:
                    DoSendQuantumText(SAY_KAN_START, me);
                    DoSpawnGalak();
                    break;
                case 1:
                    if (Player* player = GetPlayerForEscort())
                        player->GroupEventHappens(QUEST_PROTECT_KANATI, me);
                    break;
            }
        }

        void DoSpawnGalak()
        {
            for (int i = 0; i < 3; ++i)
                me->SummonCreature(NPC_GALAK_ASS, GalakLoc, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 25000);
        }

        void JustSummoned(Creature* summoned)
        {
            summoned->AI()->AttackStart(me);
        }
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_kanatiAI(creature);
	}
};

enum Lakota
{
    SAY_LAKOTA_START      = -1100365,
    SAY_LAKOTA_LOOK_OUT   = -1100366,
    SAY_LAKOTA_HERE_COME  = -1100367,
    SAY_LAKOTA_MORE       = -1100368,
    SAY_LAKOTA_END        = -1100369,

    QUEST_FREE_AT_LAST    = 4904,
    NPC_GRIM_BANDIT       = 10758,
    FACTION_ESCORTEE_LAKO = 232,

    ID_AMBUSH_1 = 0,
    ID_AMBUSH_2 = 2,
    ID_AMBUSH_3 = 4,
};

Position const BanditLoc[6] =
{
    {-4905.479492f, -2062.732666f, 84.352f, 0.0f},
    {-4915.201172f, -2073.528320f, 84.733f, 0.0f},
    {-4878.883301f, -1986.947876f, 91.966f, 0.0f},
    {-4877.503906f, -1966.113403f, 91.859f, 0.0f},
    {-4767.985352f, -1873.169189f, 90.192f, 0.0f},
    {-4788.861328f, -1888.007813f, 89.888f, 0.0f}
};

class npc_lakota_windsong : public CreatureScript
{
public:
    npc_lakota_windsong() : CreatureScript("npc_lakota_windsong") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_FREE_AT_LAST)
        {
            DoSendQuantumText(SAY_LAKOTA_START, creature, player);
            creature->SetCurrentFaction(FACTION_ESCORTEE_LAKO);

            if (npc_lakota_windsongAI* EscortAI = CAST_AI(npc_lakota_windsong::npc_lakota_windsongAI, creature->AI()))
                EscortAI->Start(false, false, player->GetGUID(), quest);
        }
        return true;
    }

    struct npc_lakota_windsongAI : public npc_escortAI
    {
        npc_lakota_windsongAI(Creature* creature) : npc_escortAI(creature) { }

        void Reset() {}

        void WaypointReached(uint32 waypointId)
        {
            switch (waypointId)
            {
                case 8:
                    DoSendQuantumText(SAY_LAKOTA_LOOK_OUT, me);
                    DoSpawnBandits(ID_AMBUSH_1);
                    break;
                case 14:
                    DoSendQuantumText(SAY_LAKOTA_HERE_COME, me);
                    DoSpawnBandits(ID_AMBUSH_2);
                    break;
                case 21:
                    DoSendQuantumText(SAY_LAKOTA_MORE, me);
                    DoSpawnBandits(ID_AMBUSH_3);
                    break;
                case 45:
                    if (Player* player = GetPlayerForEscort())
                        player->GroupEventHappens(QUEST_FREE_AT_LAST, me);
                    break;
            }
        }

        void DoSpawnBandits(int AmbushId)
        {
            for (int i = 0; i < 2; ++i)
                me->SummonCreature(NPC_GRIM_BANDIT, BanditLoc[i+AmbushId], TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lakota_windsongAI(creature);
    }
};

enum Packa
{
    SAY_START         = -1000362,
    SAY_WYVERN        = -1000363,
    SAY_COMPLETE      = -1000364,

    QUEST_HOMEWARD    = 4770,
    NPC_WYVERN        = 4107,
    FACTION_ESCORTEE  = 232,
};

Position const WyvernLoc[3] =
{
    {-4990.606f, -906.057f, -5.343f, 0.0f},
    {-4970.241f, -927.378f, -4.951f, 0.0f},
    {-4985.364f, -952.528f, -5.199f, 0.0f},
};

class npc_paoka_swiftmountain : public CreatureScript
{
public:
    npc_paoka_swiftmountain() : CreatureScript("npc_paoka_swiftmountain") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_HOMEWARD)
        {
            DoSendQuantumText(SAY_START, creature, player);
            creature->SetCurrentFaction(FACTION_ESCORTEE);

            if (npc_paoka_swiftmountainAI* EscortAI = CAST_AI(npc_paoka_swiftmountain::npc_paoka_swiftmountainAI, creature->AI()))
                EscortAI->Start(false, false, player->GetGUID(), quest);
        }
        return true;
    }

    struct npc_paoka_swiftmountainAI : public npc_escortAI
    {
        npc_paoka_swiftmountainAI(Creature* creature) : npc_escortAI(creature) { }

        void Reset() {}

        void WaypointReached(uint32 id)
        {
            switch (id)
            {
                case 15:
                    DoSendQuantumText(SAY_WYVERN, me);
                    DoSpawnWyvern();
                    break;
                case 26:
                    DoSendQuantumText(SAY_COMPLETE, me);
                    break;
                case 27:
                    if (Player* player = GetPlayerForEscort())
                        player->GroupEventHappens(QUEST_HOMEWARD, me);
                    break;
            }
        }

        void DoSpawnWyvern()
        {
            for (int i = 0; i < 3; ++i)
                me->SummonCreature(NPC_WYVERN, WyvernLoc[i], TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 60000);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_paoka_swiftmountainAI(creature);
    }
};

#define GOSSIP_P "Please tell me the Phrase.."

enum Plucky
{
    FACTION_FRIENDLY     = 35,
    QUEST_SCOOP          = 1950,
    SPELL_PLUCKY_HUMAN   = 9192,
    SPELL_PLUCKY_CHICKEN = 9220,
};

class npc_plucky : public CreatureScript
{
public:
    npc_plucky() : CreatureScript("npc_plucky") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_SCOOP) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_P, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(738, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
				player->CLOSE_GOSSIP_MENU();
				player->CompleteQuest(QUEST_SCOOP);
				break;
        }
        return true;
    }

    struct npc_pluckyAI : public QuantumBasicAI
    {
        npc_pluckyAI(Creature* creature) : QuantumBasicAI(creature)
		{
			NormFaction = creature->GetFaction();
		}

        uint32 NormFaction;
        uint32 ResetTimer;

        void Reset()
        {
            ResetTimer = 120000;

            if (me->GetFaction() != NormFaction)
                me->SetCurrentFaction(NormFaction);

            if (me->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
                me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

            DoCast(me, SPELL_PLUCKY_CHICKEN, false);
        }

        void ReceiveEmote(Player* player, uint32 TextEmote)
        {
            if (player->GetQuestStatus(QUEST_SCOOP) == QUEST_STATUS_INCOMPLETE)
            {
                if (TextEmote == TEXT_EMOTE_BECKON)
                {
                    me->SetCurrentFaction(FACTION_FRIENDLY);
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    DoCast(me, SPELL_PLUCKY_HUMAN, false);
                }
            }

            if (TextEmote == TEXT_EMOTE_CHICKEN)
            {
                if (me->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
                    return;

                else
                {
                    me->SetCurrentFaction(FACTION_FRIENDLY);
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    DoCast(me, SPELL_PLUCKY_HUMAN, false);
                    me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
                }
            }
        }

        void UpdateAI(const uint32 Diff)
        {
            if (me->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
            {
                if (ResetTimer <= Diff)
                {
                    if (!me->GetVictim())
                        EnterEvadeMode();

                    else
						me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    return;
                }
                else ResetTimer -= Diff;
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_pluckyAI(creature);
    }
};

enum PantherCage
{
	QUEST_CAPACITOR_GZIMO = 5151,
    NPC_ENRAGED_PANTHER   = 10992,
};

class go_panther_cage : public GameObjectScript
{
public:
    go_panther_cage() : GameObjectScript("go_panther_cage") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(QUEST_CAPACITOR_GZIMO) == QUEST_STATUS_INCOMPLETE)
        {
            if (Creature* panther = go->FindCreatureWithDistance(NPC_ENRAGED_PANTHER, 10.0f, true))
            {
                panther->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                panther->SetReactState(REACT_AGGRESSIVE);
                panther->AI()->AttackStart(player);
            }
        }
        return true;
    }
};

class npc_enraged_panther : public CreatureScript
{
public:
    npc_enraged_panther() : CreatureScript("npc_enraged_panther") { }

    struct npc_enraged_pantherAI : public QuantumBasicAI
    {
        npc_enraged_pantherAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetReactState(REACT_PASSIVE);
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_enraged_pantherAI(creature);
    }
};

void AddSC_thousand_needles()
{
    new npc_kanati();
    new npc_lakota_windsong();
    new npc_paoka_swiftmountain();
    new npc_plucky();
	new go_panther_cage();
    new npc_enraged_panther();
}