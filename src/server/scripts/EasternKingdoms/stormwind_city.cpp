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

#define GOSSIP_ITEM_MALIN "Can you send me to Theramore? I have an urgent message for Lady Jaina from Highlord Bolvar."

enum Malin
{
	SPELL_DUSTWALLOW_TELEPORT = 42711,
	QUEST_RETURN_TO_JAINA     = 11223,
};

class npc_archmage_malin : public CreatureScript
{
public:
    npc_archmage_malin() : CreatureScript("npc_archmage_malin") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_RETURN_TO_JAINA) == QUEST_STATUS_COMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_MALIN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->CastSpell(player, SPELL_DUSTWALLOW_TELEPORT, true);
        }
        return true;
    }
};

enum Bartleby
{
    FACTION_ENEMY = 168,
    QUEST_BEAT    = 1640,
};

class npc_bartleby : public CreatureScript
{
public:
    npc_bartleby() : CreatureScript("npc_bartleby") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_BEAT)
        {
            creature->SetCurrentFaction(FACTION_ENEMY);
            creature->AI()->AttackStart(player);
        }
        return true;
    }

    struct npc_bartlebyAI : public QuantumBasicAI
    {
        npc_bartlebyAI(Creature* creature) : QuantumBasicAI(creature)
        {
            NormalFaction = creature->GetFaction();
        }

        uint32 NormalFaction;

        void Reset()
        {
            if (me->GetFaction() != NormalFaction)
                me->SetCurrentFaction(NormalFaction);
        }

        void AttackedBy(Unit* attacker)
        {
            if (me->GetVictim())
                return;

            if (me->IsFriendlyTo(attacker))
                return;

            AttackStart(attacker);
        }

        void DamageTaken(Unit* DoneBy, uint32 &damage)
        {
            if (damage > me->GetHealth() || me->HealthBelowPctDamaged(15, damage))
            {
                damage = 0;

                if (DoneBy->GetTypeId() == TYPE_ID_PLAYER)
                    CAST_PLR(DoneBy)->AreaExploredOrEventHappens(QUEST_BEAT);

                EnterEvadeMode();
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_bartlebyAI(creature);
	}
};

#define GOSSIP_ITEM_KAT_1 "Pardon the intrusion, Lady Prestor, but Highlord Bolvar suggested that I seek your advice."
#define GOSSIP_ITEM_KAT_2 "My apologies, Lady Prestor."
#define GOSSIP_ITEM_KAT_3 "Begging your pardon, Lady Prestor. That was not my intent."
#define GOSSIP_ITEM_KAT_4 "Thank you for your time, Lady Prestor."

enum TrueMasters
{
	QUEST_THE_TRUE_MASTERS = 4185,
};

class npc_lady_katrana_prestor : public CreatureScript
{
public:
    npc_lady_katrana_prestor() : CreatureScript("npc_lady_katrana_prestor") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_THE_TRUE_MASTERS) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(2693, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
                player->SEND_GOSSIP_MENU(2694, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(2695, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_KAT_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
                player->SEND_GOSSIP_MENU(2696, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->CLOSE_GOSSIP_MENU();
                player->AreaExploredOrEventHappens(QUEST_THE_TRUE_MASTERS);
                break;
        }
        return true;
    }
};

enum LordGregorLescovar
{
	SAY_QUEST_ACCEPT_ATTACK  = -1000499,
	SAY_TYRION_1             = -1000450,
	SAY_SPYBOT_1             = -1000451,
	SAY_GUARD_1              = -1000452,
	SAY_SPYBOT_2             = -1000453,
	SAY_SPYBOT_3             = -1000454,
	SAY_LESCOVAR_1           = -1000455,
	SAY_SPYBOT_4             = -1000456,
    SAY_LESCOVAR_2           = -1000457,
    SAY_GUARD_2              = -1000458,
    SAY_LESCOVAR_3           = -1000459,
    SAY_MARZON_1             = -1000460,
    SAY_LESCOVAR_4           = -1000461,
    SAY_TYRION_2             = -1000462,
    SAY_MARZON_2             = -1000463,

	NPC_PRIESTESS_TYRIONA    = 7779,
    NPC_LORD_GREGOR_LESCOVAR = 1754,
    NPC_STORMWIND_ROYAL      = 1756,
    NPC_MARZON_BLADE         = 1755,
    NPC_TYRION               = 7766,
	NPC_TYRION_SPYBOT        = 8856,

    QUEST_THE_ATTACK    = 434,
};

class npc_lord_gregor_lescovar : public CreatureScript
{
public:
    npc_lord_gregor_lescovar() : CreatureScript("npc_lord_gregor_lescovar") { }

    struct npc_lord_gregor_lescovarAI : public npc_escortAI
    {
        npc_lord_gregor_lescovarAI(Creature* creature) : npc_escortAI(creature)
        {
			creature->RestoreFaction();
        }

        uint32 Timer;
        uint32 Phase;

        uint64 MarzonGUID;

        void Reset()
        {
            Timer = 0;
            Phase = 0;

            MarzonGUID = 0;
        }

        void EnterEvadeMode()
        {
            me->DisappearAndDie();

            if (Creature* Marzon = Unit::GetCreature(*me, MarzonGUID))
            {
                if (Marzon->IsAlive())
                    Marzon->DisappearAndDie();
            }
        }

        void EnterToBattle(Unit* who)
        {
            if (Creature* Marzon = Unit::GetCreature(*me, MarzonGUID))
            {
                if (Marzon->IsAlive() && !Marzon->IsInCombatActive())
                    Marzon->AI()->AttackStart(who);
            }
        }

        void WaypointReached(uint32 pointId)
        {
            switch (pointId)
            {
                case 14:
                    SetEscortPaused(true);
                    DoSendQuantumText(SAY_LESCOVAR_2, me);
                    Timer = 3000;
                    Phase = 1;
                    break;
                case 16:
                    SetEscortPaused(true);
                    if (Creature* Marzon = me->SummonCreature(NPC_MARZON_BLADE, -8411.360352f, 480.069733f, 123.760895f, 4.941504f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000))
                    {
                        Marzon->GetMotionMaster()->MovePoint(0, -8408.000977f, 468.611450f, 123.759903f);
                        MarzonGUID = Marzon->GetGUID();
                    }
                    Timer = 2000;
                    Phase = 4;
                    break;
            }
        }

        void DoGuardsDisappearAndDie()
        {
            std::list<Creature*> GuardList;
            me->GetCreatureListWithEntryInGrid(GuardList, NPC_STORMWIND_ROYAL, 8.0f);

            if (!GuardList.empty())
            {
                for (std::list<Creature*>::const_iterator itr = GuardList.begin(); itr != GuardList.end(); ++itr)
                {
                    if (Creature* Guard = *itr)
                        Guard->DisappearAndDie();
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (Phase)
            {
                if (Timer <= diff)
                {
                    switch (Phase)
                    {
                        case 1:
                            if (Creature* guard = me->FindCreatureWithDistance(NPC_STORMWIND_ROYAL, 8.0f, true))
								DoSendQuantumText(SAY_GUARD_2, guard);
                            Timer = 3000;
                            Phase = 2;
                            break;
                        case 2:
                            DoGuardsDisappearAndDie();
                            Timer = 2000;
                            Phase = 3;
                            break;
                        case 3:
                            SetEscortPaused(false);
                            Timer = 0;
                            Phase = 0;
                            break;
                        case 4:
							DoSendQuantumText(SAY_LESCOVAR_3, me);
                            Timer = 0;
                            Phase = 0;
                            break;
                        case 5:
                            if (Creature* Marzon = Unit::GetCreature(*me, MarzonGUID))
								DoSendQuantumText(SAY_MARZON_1, Marzon);
                            Timer = 3000;
                            Phase = 6;
                            break;
                        case 6:
							DoSendQuantumText(SAY_LESCOVAR_4, me);
                            if (Player* player = GetPlayerForEscort())
                                player->AreaExploredOrEventHappens(QUEST_THE_ATTACK);
							Timer = 2000;
                            Phase = 7;
                            break;
                        case 7:
                            if (Creature* Tyrion = me->FindCreatureWithDistance(NPC_TYRION, 20.0f, true))
								DoSendQuantumText(SAY_TYRION_2, Tyrion);

                            if (Creature* Marzon = Unit::GetCreature(*me, MarzonGUID))
                                Marzon->SetCurrentFaction(16);

							me->SetCurrentFaction(16);
                            Timer = 0;
                            Phase = 0;
                            break;
                    }
                }
				else Timer -= diff;
            }
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lord_gregor_lescovarAI(creature);
    }
};

class npc_marzon_silent_blade : public CreatureScript
{
public:
    npc_marzon_silent_blade() : CreatureScript("npc_marzon_silent_blade") { }

    struct npc_marzon_silent_bladeAI : public QuantumBasicAI
    {
        npc_marzon_silent_bladeAI(Creature* creature) : QuantumBasicAI(creature)
        {
            me->SetWalk(true);
        }

        void Reset()
        {
            me->RestoreFaction();
        }

        void EnterToBattle(Unit* who)
        {
			DoSendQuantumText(SAY_MARZON_2, me);

            if (me->IsSummon())
            {
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                {
                    if (summoner->GetTypeId() == TYPE_ID_UNIT && summoner->IsAlive() && !summoner->IsInCombatActive())
                        summoner->ToCreature()->AI()->AttackStart(who);
                }
            }
        }

        void EnterEvadeMode()
        {
            me->DisappearAndDie();

            if (me->IsSummon())
            {
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                {
                    if (summoner && summoner->GetTypeId() == TYPE_ID_UNIT && summoner->IsAlive())
                        summoner->ToCreature()->DisappearAndDie();
                }
            }
        }

        void MovementInform(uint32 uiType, uint32 /*uiId*/)
        {
            if (uiType != POINT_MOTION_TYPE)
                return;

            if (me->IsSummon())
            {
                Unit* summoner = me->ToTempSummon()->GetSummoner();
                if (summoner && summoner->GetTypeId() == TYPE_ID_UNIT && summoner->IsAIEnabled)
                {
                    npc_lord_gregor_lescovar::npc_lord_gregor_lescovarAI* ai = CAST_AI(npc_lord_gregor_lescovar::npc_lord_gregor_lescovarAI, summoner->GetAI());

                    if (ai)
                    {
                        ai->Timer = 2000;
                        ai->Phase = 5;
                    }
                    //me->ChangeOrient(0.0f, summoner);
                }
            }
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
		return new npc_marzon_silent_bladeAI(creature);
	}
};

class npc_tyrion_spybot : public CreatureScript
{
public:
    npc_tyrion_spybot() : CreatureScript("npc_tyrion_spybot") { }

    struct npc_tyrion_spybotAI : public npc_escortAI
    {
        npc_tyrion_spybotAI(Creature* creature) : npc_escortAI(creature) {}

        uint32 Timer;
        uint32 Phase;

        void Reset()
        {
			Timer = 0;
			Phase = 0;
        }

        void WaypointReached(uint32 pointId)
        {
            switch (pointId)
            {
                case 1:
                    SetEscortPaused(true);
                    Timer = 2000;
                    Phase = 1;
                    break;
                case 5:
                    SetEscortPaused(true);
                    DoSendQuantumText(SAY_SPYBOT_1, me);
                    Timer = 2000;
                    Phase = 5;
                    break;
                case 17:
                    SetEscortPaused(true);
                    DoSendQuantumText(SAY_SPYBOT_3, me);
                    Timer = 3000;
                    Phase = 8;
                    break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (Phase)
            {
                if (Timer <= diff)
                {
                    switch (Phase)
                    {
                        case 1:
                            DoSendQuantumText(SAY_QUEST_ACCEPT_ATTACK, me);
                            Timer = 3000;
                            Phase = 2;
                            break;
                        case 2:
                            if (Creature* Tyrion = me->FindCreatureWithDistance(NPC_TYRION, 10.0f))
								DoSendQuantumText(SAY_TYRION_1, Tyrion);
                            Timer = 3000;
                            Phase = 3;
                            break;
                        case 3:
                            me->UpdateEntry(NPC_PRIESTESS_TYRIONA, ALLIANCE);
                            Timer = 2000;
                            Phase = 4;
                            break;
                        case 4:
                           SetEscortPaused(false);
                           Phase = 0;
                           Timer = 0;
                           break;
                        case 5:
                            if (Creature* Guard = me->FindCreatureWithDistance(NPC_STORMWIND_ROYAL, 10.0f, true))
								DoSendQuantumText(SAY_GUARD_1, Guard);
                            Timer = 3000;
                            Phase = 6;
                            break;
                        case 6:
                            DoSendQuantumText(SAY_SPYBOT_2, me);
                            Timer = 3000;
                            Phase = 7;
                            break;
                        case 7:
                            SetEscortPaused(false);
                            Timer = 0;
                            Phase = 0;
                            break;
                        case 8:
                            if (Creature* Lescovar = me->FindCreatureWithDistance(NPC_LORD_GREGOR_LESCOVAR, 10.0f))
								DoSendQuantumText(SAY_LESCOVAR_1, Lescovar);
                            Timer = 3000;
                            Phase = 9;
                            break;
                        case 9:
                            DoSendQuantumText(SAY_SPYBOT_4, me);
                            Timer = 3000;
                            Phase = 10;
                            break;
                        case 10:
                            if (Creature* Lescovar = me->FindCreatureWithDistance(NPC_LORD_GREGOR_LESCOVAR, 10.0f))
                            {
                                if (Player* player = GetPlayerForEscort())
                                {
                                    CAST_AI(npc_lord_gregor_lescovar::npc_lord_gregor_lescovarAI, Lescovar->AI())->Start(false, false, player->GetGUID());
                                    CAST_AI(npc_lord_gregor_lescovar::npc_lord_gregor_lescovarAI, Lescovar->AI())->SetMaxPlayerDistance(200.0f);
                                }
                            }
                            me->DisappearAndDie();
                            Timer = 0;
                            Phase = 0;
                            break;
                    }
                }
				else Timer -= diff;
            }
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_tyrion_spybotAI(creature);
	}
};

class npc_tyrion : public CreatureScript
{
public:
    npc_tyrion() : CreatureScript("npc_tyrion") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_THE_ATTACK)
        {
            if (Creature* Spybot = creature->FindCreatureWithDistance(NPC_TYRION_SPYBOT, 5.0f, true))
            {
                CAST_AI(npc_tyrion_spybot::npc_tyrion_spybotAI, Spybot->AI())->Start(false, false, player->GetGUID());
                CAST_AI(npc_tyrion_spybot::npc_tyrion_spybotAI, Spybot->AI())->SetMaxPlayerDistance(200.0f);
            }
            return true;
        }
        return false;
    }
};

void AddSC_stormwind_city()
{
    new npc_archmage_malin();
    new npc_bartleby();
    new npc_lady_katrana_prestor();
    new npc_tyrion();
    new npc_tyrion_spybot();
    new npc_lord_gregor_lescovar();
    new npc_marzon_silent_blade();
}