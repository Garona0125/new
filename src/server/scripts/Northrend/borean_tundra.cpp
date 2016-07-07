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
#include "QuantumFollowerAI.h"
#include "QuantumSystemText.h"

enum Sinkhole
{
    SPELL_SET_CART        = 46797,
    SPELL_EXPLODE_CART    = 46799,
    SPELL_SUMMON_CART     = 46798,
    SPELL_SUMMON_WORM     = 46800,

	NPC_SCOURGED_BURROWER = 26250,

	GO_EXPLOSIVES_CART    = 188160,

	QUEST_P_T_SINKHOLES   = 11897,
};

class npc_sinkhole_kill_credit : public CreatureScript
{
public:
    npc_sinkhole_kill_credit() : CreatureScript("npc_sinkhole_kill_credit") { }

    struct npc_sinkhole_kill_creditAI : public QuantumBasicAI
    {
        npc_sinkhole_kill_creditAI(Creature* creature) : QuantumBasicAI(creature){}

        uint32 PhaseTimer;
        uint8 Phase;
        uint64 casterGuid;

        void Reset()
        {
            PhaseTimer = 500;
            Phase = 0;
            casterGuid = 0;
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (Phase)
                return;

            if (spell->Id == SPELL_SET_CART && caster->ToPlayer()->GetQuestStatus(QUEST_P_T_SINKHOLES) == QUEST_STATUS_INCOMPLETE)
            {
                Phase = 1;
                casterGuid = caster->GetGUID();
            }
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!Phase)
                return;

            if (PhaseTimer <= diff)
            {
                switch (Phase)
                {
                    case 1:
                        DoCast(me, SPELL_EXPLODE_CART, true);
                        DoCast(me, SPELL_SUMMON_CART, true);
                        if (GameObject* cart = me->FindGameobjectWithDistance(GO_EXPLOSIVES_CART, 5.0f))
                            cart->SetUInt32Value(GAMEOBJECT_FACTION, 14);
                        PhaseTimer = 3000;
                        Phase = 2;
                        break;
                    case 2:
                        if (GameObject* cart = me->FindGameobjectWithDistance(GO_EXPLOSIVES_CART, 5.0f))
                            cart->UseDoorOrButton();
                        DoCast(me, SPELL_EXPLODE_CART, true);
                        PhaseTimer = 3000;
                        Phase = 3;
                        break;
                    case 3:
                        DoCast(me, SPELL_EXPLODE_CART, true);
                        PhaseTimer = 2000;
                        Phase = 4;
                    case 5:
                        DoCast(me, SPELL_SUMMON_WORM, true);
                        if (Unit* worm = me->FindCreatureWithDistance(NPC_SCOURGED_BURROWER, 5.0f))
                        {
                            worm->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            worm->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);
                        }
                        PhaseTimer = 1000;
                        Phase = 6;
                        break;
                    case 6:
                        DoCast(me, SPELL_EXPLODE_CART, true);
                        if (Unit* worm = me->FindCreatureWithDistance(NPC_SCOURGED_BURROWER, 5.0f))
                        {
                            me->Kill(worm);
                            worm->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                        }
                        PhaseTimer = 2000;
                        Phase = 7;
                        break;
                    case 7:
                        DoCast(me, SPELL_EXPLODE_CART, true);
                        if (Player* caster = Unit::GetPlayer(*me, casterGuid))
                            caster->KilledMonster(me->GetCreatureTemplate(), me->GetGUID());
                        PhaseTimer = 5000;
                        Phase = 8;
                        break;
                    case 8:
                        EnterEvadeMode();
                        break;
                }
            }
			else PhaseTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sinkhole_kill_creditAI(creature);
    }
};

enum KhunokWillKnow
{
	SPELL_MAMMOTH_CALF_CREDIT = 46231,

	NPC_ORPHANED_MAMMOTH_CALF = 25861,
};

class npc_khunok_the_behemoth : public CreatureScript
{
public:
    npc_khunok_the_behemoth() : CreatureScript("npc_khunok_the_behemoth") { }

    struct npc_khunok_the_behemothAI : public QuantumBasicAI
    {
		npc_khunok_the_behemothAI(Creature* creature) : QuantumBasicAI(creature) {}

		void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void MoveInLineOfSight(Unit* who)
        {
            QuantumBasicAI::MoveInLineOfSight(who);

            if (who->GetTypeId() != TYPE_ID_UNIT)
                return;

            if (who->GetEntry() == NPC_ORPHANED_MAMMOTH_CALF && me->IsWithinDistInMap(who, 10.0f))
            {
                if (Unit* owner = who->GetOwner())
                {
                    if (owner->GetTypeId() == TYPE_ID_PLAYER)
					{
						owner->CastSpell(owner, SPELL_MAMMOTH_CALF_CREDIT, true);
						owner->RemoveAllMinionsByEntry(NPC_ORPHANED_MAMMOTH_CALF);
						who->ToCreature()->DespawnAfterAction();
					}
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
        return new npc_khunok_the_behemothAI(creature);
    }
};

enum Keristrasza
{
    SPELL_TELEPORT_TO_SARAGOSA = 46772,

	QUEST_SARAGOSAS_END        = 11957,
};

#define GOSSIP_HELLO_KERI "I am prepared to face Saragosa!"

class npc_keristrasza : public CreatureScript
{
public:
    npc_keristrasza() : CreatureScript("npc_keristrasza") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_SARAGOSAS_END) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HELLO_KERI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->CLOSE_GOSSIP_MENU();
            player->CastSpell(player, SPELL_TELEPORT_TO_SARAGOSA, true);
        }
        return true;
    }
};

enum Corastrasza
{
	SPELL_FLIGHT                          = 55034,
    SPELL_SUMMON_WYRMREST_SKYTALON        = 61240,
    SPELL_WYRMREST_SKYTALON_RIDE_PERIODIC = 61244,

	QUEST_ACCESS_HIGH                     = 13413,
    QUEST_ACCESS_HIGH_DAILY               = 13414,
};

#define GOSSIP_ITEM_C_1 "I... I think so..."

class npc_corastrasza : public CreatureScript
{
public:
    npc_corastrasza() : CreatureScript("npc_corastrasza") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_ACCESS_HIGH) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_ACCESS_HIGH_DAILY) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_C_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
			player->RemoveAurasByType(SPELL_AURA_MOUNTED);
            player->CastSpell(player, SPELL_SUMMON_WYRMREST_SKYTALON, true);
			player->CLOSE_GOSSIP_MENU();
        }
        return true;
    }
};

class npc_wyrmrest_skytalon_vehicle : public CreatureScript
{
public:
    npc_wyrmrest_skytalon_vehicle() : CreatureScript("npc_wyrmrest_skytalon_vehicle") {}

    struct npc_wyrmrest_skytalon_vehicleAI : public VehicleAI
    {
        npc_wyrmrest_skytalon_vehicleAI(Creature* creature) : VehicleAI(creature)
		{
			DoCast(me, SPELL_FLIGHT, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        uint32 CheckRideTimer;

        bool IsInUse;

        void Reset()
        {
            CheckRideTimer = 6000;
        }

        void OnCharmed(bool apply)
        {
            IsInUse = apply;

            if (!apply)
                CheckRideTimer = 6000;
        }

		void JustDied(Unit* /*killer*/)
		{
			me->DespawnAfterAction();
		}

        void UpdateAI(const uint32 diff)
        {
            if (!me->IsVehicle())
                return;

            if (!IsInUse)
            {
                if (CheckRideTimer < diff)
                {
                    me->DespawnAfterAction();
                    CheckRideTimer = 6000;
                }
				else CheckRideTimer -= diff;
            }
        }
    };

    CreatureAI* GetAI(Creature *_Creature) const
    {
        return new  npc_wyrmrest_skytalon_vehicleAI(_Creature);
    }
};

enum Iruk
{
	SPELL_CREATURE_TOTEM_OF_ISSLIRUK = 46816,
	QUEST_SPIRITS_WATCH_OVER_US      = 11961,
	GOSSIP_TEXT_I                    = 12585,
};

#define GOSSIP_ITEM_I "<Search corpse for Issliruk's Totem.>"

class npc_iruk : public CreatureScript
{
public:
    npc_iruk() : CreatureScript("npc_iruk") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
		if (player->GetQuestStatus(QUEST_SPIRITS_WATCH_OVER_US) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_I, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_I, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
				player->CastSpell(player, SPELL_CREATURE_TOTEM_OF_ISSLIRUK, true);
                player->CLOSE_GOSSIP_MENU();
                break;
		}
        return true;
    }
};

enum NerubarVictim
{
	QUEST_TAKEN_BY_THE_SCOURGE = 11611,
	NPC_WARSONG_PEON           = 25270,
	SPELL_FREED_WARSONG_PEON   = 45532,
};

const uint32 nerubarVictims[3] =
{
    45526,
	45527,
	45514,
};

class npc_nerubar_victim : public CreatureScript
{
public:
    npc_nerubar_victim() : CreatureScript("npc_nerubar_victim") {}

    struct npc_nerubar_victimAI : public QuantumBasicAI
    {
        npc_nerubar_victimAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset(){}

        void JustDied(Unit* killer)
        {
            if (killer->GetTypeId() == TYPE_ID_PLAYER)
            {
                if (killer->ToPlayer()->GetQuestStatus(QUEST_TAKEN_BY_THE_SCOURGE) == QUEST_STATUS_INCOMPLETE)
                {
                    uint8 rand = urand(0, 99);
                    if (rand < 25)
                    {
                        killer->CastSpell(me, SPELL_FREED_WARSONG_PEON, true);
                        killer->ToPlayer()->KilledMonsterCredit(NPC_WARSONG_PEON, 0);
                    }
                    else
						if (rand < 75)
							killer->CastSpell(me, nerubarVictims[urand(0, 2)], true);
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nerubar_victimAI(creature);
    }
};

enum Jenny
{
	SPELL_GIVE_JENNY_CREDIT = 46358,
    SPELL_CRATES_CARRIED    = 46340,
    SPELL_DROP_CRATE        = 46342,

    QUEST_LOADER_UP         = 11881,

    NPC_FEZZIX_GEARTWIST    = 25849,
    NPC_JENNY               = 25969,
};

class npc_jenny : public CreatureScript
{
public:
    npc_jenny() : CreatureScript("npc_jenny") { }

    struct npc_jennyAI : public QuantumBasicAI
    {
        npc_jennyAI(Creature* creature) : QuantumBasicAI(creature) {}

        bool SetCrateNumber;

        void Reset()
        {
            if (!SetCrateNumber)
                SetCrateNumber = true;

            me->SetReactState(REACT_PASSIVE);

            switch (CAST_PLR(me->GetOwner())->GetTeamId())
            {
                case TEAM_ALLIANCE:
                    me->SetCurrentFaction(FACTION_ESCORT_A_NEUTRAL_ACTIVE);
                    break;
				default:
				case TEAM_HORDE:
                    me->SetCurrentFaction(FACTION_ESCORT_H_NEUTRAL_ACTIVE);
                    break;
            }
        }

        void DamageTaken(Unit* /*doneby*/, uint32& /*damage*/)
        {
            DoCast(me, SPELL_DROP_CRATE, true);
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (SetCrateNumber)
            {
                me->AddAura(SPELL_CRATES_CARRIED, me);
                SetCrateNumber = false;
            }

            if (!SetCrateNumber && !me->HasAura(SPELL_CRATES_CARRIED))
                me->DisappearAndDie();

            if (!UpdateVictim())
                return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jennyAI (creature);
    }
};

class npc_fezzix_geartwist : public CreatureScript
{
public:
    npc_fezzix_geartwist() : CreatureScript("npc_fezzix_geartwist") { }

    struct npc_fezzix_geartwistAI : public QuantumBasicAI
    {
        npc_fezzix_geartwistAI(Creature* creature) : QuantumBasicAI(creature) {}

        void MoveInLineOfSight(Unit* who)
        {
            QuantumBasicAI::MoveInLineOfSight(who);

            if (who->GetTypeId() != TYPE_ID_UNIT)
                return;

            if (who->GetEntry() == NPC_JENNY && me->IsWithinDistInMap(who, 10.0f))
            {
                if (Unit* owner = who->GetOwner())
                {
                    if (Player* player = owner->ToPlayer())
                    {
						Creature* creature = who->ToCreature();

                        if (who->HasAura(SPELL_CRATES_CARRIED))
                        {
                            owner->CastSpell(owner, SPELL_GIVE_JENNY_CREDIT, true);
                            player->CompleteQuest(QUEST_LOADER_UP);
                            creature->DisappearAndDie();
                        }
                    }
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fezzix_geartwistAI(creature);
    }
};

enum NesingwaryTrapper
{
    GO_HIGH_QUALITY_FUR = 187983,

    GO_CARIBOU_TRAP_1   = 187982,
    GO_CARIBOU_TRAP_2   = 187995,
    GO_CARIBOU_TRAP_3   = 187996,
    GO_CARIBOU_TRAP_4   = 187997,
    GO_CARIBOU_TRAP_5   = 187998,
    GO_CARIBOU_TRAP_6   = 187999,
    GO_CARIBOU_TRAP_7   = 188000,
    GO_CARIBOU_TRAP_8   = 188001,
    GO_CARIBOU_TRAP_9   = 188002,
    GO_CARIBOU_TRAP_10  = 188003,
    GO_CARIBOU_TRAP_11  = 188004,
    GO_CARIBOU_TRAP_12  = 188005,
    GO_CARIBOU_TRAP_13  = 188006,
    GO_CARIBOU_TRAP_14  = 188007,
    GO_CARIBOU_TRAP_15  = 188008,

    SPELL_TRAPPED       = 46104,

	CaribouTrapsNum     = 15,
};

const uint32 CaribouTraps[CaribouTrapsNum] =
{
	GO_CARIBOU_TRAP_1,
	GO_CARIBOU_TRAP_2,
	GO_CARIBOU_TRAP_3,
	GO_CARIBOU_TRAP_4,
	GO_CARIBOU_TRAP_5,
    GO_CARIBOU_TRAP_6,
	GO_CARIBOU_TRAP_7,
	GO_CARIBOU_TRAP_8,
	GO_CARIBOU_TRAP_9,
	GO_CARIBOU_TRAP_10,
    GO_CARIBOU_TRAP_11,
	GO_CARIBOU_TRAP_12,
	GO_CARIBOU_TRAP_13,
	GO_CARIBOU_TRAP_14,
	GO_CARIBOU_TRAP_15,
};

class npc_nesingwary_trapper : public CreatureScript
{
public:
    npc_nesingwary_trapper() : CreatureScript("npc_nesingwary_trapper") { }

    struct npc_nesingwary_trapperAI : public QuantumBasicAI
    {
        npc_nesingwary_trapperAI(Creature* creature) : QuantumBasicAI(creature)
		{
			creature->SetVisible(false);
		}

        uint64 GoCaribouGUID;
        uint8 Phase;
        uint32 PhaseTimer;

        void Reset()
        {
            me->SetVisible(false);
            PhaseTimer = 2500;
            Phase = 1;
            GoCaribouGUID = 0;
        }

        void EnterToBattle(Unit* /*who*/){}

        void MoveInLineOfSight(Unit* /*who*/){}

        void JustDied(Unit* /*who*/)
        {
            if (GameObject* GoCaribou = me->GetMap()->GetGameObject(GoCaribouGUID))
                GoCaribou->SetLootState(GO_JUST_DEACTIVATED);

            if (TempSummon* summon = me->ToTempSummon())
			{
                if (summon->IsSummon())
				{
                    if (Unit* temp = summon->GetSummoner())
					{
                        if (temp->GetTypeId() == TYPE_ID_PLAYER)
                            CAST_PLR(temp)->KilledMonsterCredit(me->GetEntry(), 0);
					}
				}
			}

            if (GameObject* GoCaribou = me->GetMap()->GetGameObject(GoCaribouGUID))
                GoCaribou->SetGoState(GO_STATE_READY);
        }

        void UpdateAI(const uint32 diff)
        {
            if (PhaseTimer <= diff)
            {
                switch (Phase)
                {
                    case 1:
                        me->SetVisible(true);
                        PhaseTimer = 2000;
                        Phase = 2;
                        break;
                    case 2:
                        if (GameObject* go_fur = me->FindGameobjectWithDistance(GO_HIGH_QUALITY_FUR, 11.0f))
                            me->GetMotionMaster()->MovePoint(0, go_fur->GetPositionX(), go_fur->GetPositionY(), go_fur->GetPositionZ());
                        PhaseTimer = 1500;
                        Phase = 3;
                        break;
                    case 3:
						//DoSendQuantumText(SAY_NESINGWARY_1, me);
                        PhaseTimer = 2000;
                        Phase = 4;
                        break;
                    case 4:
                        me->HandleEmoteCommand(EMOTE_ONESHOT_LOOT);
                        PhaseTimer = 1000;
                        Phase = 5;
                        break;
                    case 5:
                        me->HandleEmoteCommand(EMOTE_ONESHOT_NONE);
                        PhaseTimer = 500;
                        Phase = 6;
                        break;
                    case 6:
                        if (GameObject* go_fur = me->FindGameobjectWithDistance(GO_HIGH_QUALITY_FUR, 11.0f))
                            go_fur->Delete();
                        PhaseTimer = 500;
                        Phase = 7;
                        break;
                    case 7:
                    {
                        GameObject* GoCaribou = 0;
                        for (uint8 i = 0; i < CaribouTrapsNum; ++i)
                        {
                            GoCaribou = me->FindGameobjectWithDistance(CaribouTraps[i], 5.0f);
                            if (GoCaribou)
                            {
                                GoCaribou->SetGoState(GO_STATE_ACTIVE);
                                GoCaribouGUID = GoCaribou->GetGUID();
                                break;
                            }
                        }
                        Phase = 8;
                        PhaseTimer = 1000;
                    }
                        break;
                    case 8:
                        DoCast(me, SPELL_TRAPPED, true);
                        Phase = 0;
                        break;
                }
            }
			else PhaseTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nesingwary_trapperAI(creature);
    }
};

enum Lurgglbr
{
    QUEST_ESCAPE_WINTERFIN_CAVERNS = 11570,
    GO_CAGE                        = 187369,
    FACTION_ESCORTEE_A             = 774,
    FACTION_ESCORTEE_H             = 775,

	SAY_WP_1_LUR_START  = -1571004,
	SAY_WP_1_LUR_END    = -1571005,
	SAY_WP_41_LUR_START = -1571006,
	SAY_WP_41_LUR_END   = -1571007,
};

class npc_lurgglbr : public CreatureScript
{
public:
    npc_lurgglbr() : CreatureScript("npc_lurgglbr") { }

	bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_ESCAPE_WINTERFIN_CAVERNS)
        {
            if (GameObject* go = creature->FindGameobjectWithDistance(GO_CAGE, 5.0f))
            {
                go->SetRespawnTime(0);
                go->SetGoType(GAMEOBJECT_TYPE_BUTTON);
                go->UseDoorOrButton(20);
            }

            if (npc_escortAI* EscortAI = CAST_AI(npc_lurgglbr::npc_lurgglbrAI, creature->AI()))
                EscortAI->Start(true, false, player->GetGUID());

            switch (player->GetTeam())
            {
                case ALLIANCE:
                    creature->SetCurrentFaction(FACTION_ESCORTEE_A);
                    break;
                default:
                case HORDE:
                    creature->SetCurrentFaction(FACTION_ESCORTEE_H);
                    break;
            }
            return true;
        }
        return false;
    }

    struct npc_lurgglbrAI : public npc_escortAI
    {
        npc_lurgglbrAI(Creature* creature) : npc_escortAI(creature){}

        uint32 IntroTimer;
        uint32 IntroPhase;

        void Reset()
        {
            if (!HasEscortState(STATE_ESCORT_ESCORTING))
            {
                IntroTimer = 0;
                IntroPhase = 0;
            }
        }

        void WaypointReached(uint32 i)
        {
            switch (i)
            {
                case 0:
                    IntroPhase = 1;
                    IntroTimer = 2000;
                    break;
                case 41:
                    IntroPhase = 4;
                    IntroTimer = 2000;
                    break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (IntroPhase)
            {
                if (IntroTimer <= diff)
                {
                    switch (IntroPhase)
                    {
                        case 1:
							DoSendQuantumText(SAY_WP_1_LUR_START, me);
                            IntroPhase = 2;
                            IntroTimer = 7500;
                            break;
                        case 2:
							DoSendQuantumText(SAY_WP_1_LUR_END, me);
                            IntroPhase = 3;
                            IntroTimer = 7500;
                            break;
                        case 3:
                            me->SetReactState(REACT_AGGRESSIVE);
                            IntroPhase = 0;
                            IntroTimer = 0;
                            break;
                        case 4:
							DoSendQuantumText(SAY_WP_41_LUR_START, me);
                            IntroPhase = 5;
                            IntroTimer = 8000;
                            break;
                        case 5:
							DoSendQuantumText(SAY_WP_41_LUR_END, me);
                            IntroPhase = 6;
                            IntroTimer = 2500;
                            break;
                        case 6:
                            if (Player* player = GetPlayerForEscort())
                                player->GroupEventHappens(QUEST_ESCAPE_WINTERFIN_CAVERNS, me);
                            IntroPhase = 7;
                            IntroTimer = 2500;
                            break;
                        case 7:
                            me->DespawnAfterAction();
                            IntroPhase = 0;
                            IntroTimer = 0;
                            break;
                    }
                }
				else IntroTimer -= diff;
            }
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lurgglbrAI(creature);
    }
};

enum NexusDrakeHatchling
{
    SPELL_DRAKE_HARPOON             = 46607,
    SPELL_RED_DRAGONBLOOD           = 46620,
    SPELL_DRAKE_HATCHLING_SUBDUED   = 46691,
    SPELL_SUBDUED                   = 46675,

    NPC_RAELORASZ                   = 26117,

    QUEST_DRAKE_HUNT                = 11919,
    QUEST_DRAKE_HUNT_D              = 11940,
};

class npc_nexus_drake_hatchling : public CreatureScript
{
public:
    npc_nexus_drake_hatchling() : CreatureScript("npc_nexus_drake_hatchling") { }

    struct npc_nexus_drake_hatchlingAI : public FollowerAI //The spell who makes the npc follow the player is missing, also we can use FollowerAI!
    {
        npc_nexus_drake_hatchlingAI(Creature* creature) : FollowerAI(creature) {}

        uint64 HarpoonerGUID;

        bool WithRedDragonBlood;

        void Reset()
        {
           WithRedDragonBlood = false;
           HarpoonerGUID = 0;
        }

        void EnterToBattle(Unit* who)
        {
            if (me->IsValidAttackTarget(who))
                AttackStart(who);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_DRAKE_HARPOON && caster->GetTypeId() == TYPE_ID_PLAYER)
            {
                HarpoonerGUID = caster->GetGUID();
                DoCast(me, SPELL_RED_DRAGONBLOOD, true);
            }
            WithRedDragonBlood = true;
        }

        void MoveInLineOfSight(Unit* who)
        {
            FollowerAI::MoveInLineOfSight(who);

            if (!HarpoonerGUID)
                return;
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (WithRedDragonBlood && HarpoonerGUID && !me->HasAura(SPELL_RED_DRAGONBLOOD))
            {
                if (Player* Harpooner = Unit::GetPlayer(*me, HarpoonerGUID))
                {
                    EnterEvadeMode();
                    StartFollow(Harpooner, 35, 0);

                    DoCast(me, SPELL_SUBDUED, true);
                    Harpooner->CastSpell(Harpooner, SPELL_DRAKE_HATCHLING_SUBDUED, true);

                    me->AttackStop();
                    WithRedDragonBlood = false;
                }
            }

            if (me->HasAura(SPELL_SUBDUED))
            {
                if (Creature* Raelorasz = me->FindCreatureWithDistance(NPC_RAELORASZ, 10.0f))
                {
                    if (Player *Harpooner = GetLeaderForFollower())
                    {
                        Harpooner->KilledMonsterCredit(26175, 0);
                        Harpooner->RemoveAura(SPELL_DRAKE_HATCHLING_SUBDUED);
                        SetFollowComplete();
                        HarpoonerGUID = 0;
						me->DespawnAfterAction();
                    }
                }
            }

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_nexus_drake_hatchlingAI(creature);
    }
};

enum Thassarian
{
    QUEST_LAST_RITES        = 12019,

    SPELL_TRANSFORM_VALANAR = 46753,
    SPELL_STUN              = 46957,
    SPELL_SHADOW_BOLT       = 15537,

    NPC_IMAGE_LICH_KING     = 26203,
    NPC_COUNSELOR_TALBOT    = 25301,
    NPC_PRINCE_VALANAR      = 28189,
    NPC_GENERAL_ARLOS       = 25250,
    NPC_LERYSSA             = 25251,

    SAY_TALBOT_1            = -1571004,
    SAY_LICH_1              = -1571005,
    SAY_TALBOT_2            = -1571006,
    SAY_THASSARIAN_1        = -1571007,
    SAY_THASSARIAN_2        = -1571008,
    SAY_LICH_2              = -1571009,
    SAY_THASSARIAN_3        = -1571010,
    SAY_TALBOT_3            = -1571011,
    SAY_LICH_3              = -1571012,
    SAY_TALBOT_4            = -1571013,
    SAY_ARLOS_1             = -1571014,
    SAY_ARLOS_2             = -1571015,
    SAY_LERYSSA_1           = -1571016,
    SAY_THASSARIAN_4        = -1571017,
    SAY_LERYSSA_2           = -1571018,
    SAY_THASSARIAN_5        = -1571019,
    SAY_LERYSSA_3           = -1571020,
    SAY_THASSARIAN_6        = -1571021,
    SAY_LERYSSA_4           = -1571022,
    SAY_THASSARIAN_7        = -1571023,
};

#define GOSSIP_ITEM_T "Let's do this, Thassarian. It's now or never."

class npc_thassarian : public CreatureScript
{
public:
    npc_thassarian() : CreatureScript("npc_thassarian") { }

    struct npc_thassarianAI : public npc_escortAI
    {
        npc_thassarianAI(Creature* creature) : npc_escortAI(creature){}

        uint64 Arthas;
        uint64 Talbot;
        uint64 Leryssa;
        uint64 Arlos;

        bool ArthasInPosition;
        bool ArlosInPosition;
        bool LeryssaInPosition;
        bool TalbotInPosition;

        uint32 Phase;
        uint32 PhaseTimer;

        void Reset()
        {
            me->RestoreFaction();
            me->RemoveStandFlags(UNIT_STAND_STATE_SIT);

            Arthas = 0;
            Talbot = 0;
            Leryssa = 0;
            Arlos = 0;

            ArthasInPosition = false;
            ArlosInPosition = false;
            LeryssaInPosition = false;
            TalbotInPosition = false;

            Phase = 0;
            PhaseTimer = 0;
        }

        void WaypointReached(uint32 uiPointId)
        {
            Player* player = GetPlayerForEscort();

            if (!player)
                return;

            switch (uiPointId)
            {
                case 3:
                    SetEscortPaused(true);
                    if (Creature* pArthas = me->SummonCreature(NPC_IMAGE_LICH_KING, 3730.313f, 3518.689f, 473.324f, 1.562f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 120000))
                    {
                        Arthas = pArthas->GetGUID();
                        pArthas->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        pArthas->SetReactState(REACT_PASSIVE);
                        pArthas->SetWalk(true);
                        pArthas->GetMotionMaster()->MovePoint(0, 3737.374756f, 3564.841309f, 477.433014f);
                    }
                    if (Creature* pTalbot = me->SummonCreature(NPC_COUNSELOR_TALBOT, 3747.23f, 3614.936f, 473.321f, 4.462012f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 120000))
                    {
                        Talbot = pTalbot->GetGUID();
                        pTalbot->SetWalk(true);
                        pTalbot->GetMotionMaster()->MovePoint(0, 3738.000977f, 3568.882080f, 477.433014f);
                    }
                    me->SetWalk(false);
                    break;
                case 4:
                    SetEscortPaused(true);
                    Phase = 7;
                    break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (ArthasInPosition && TalbotInPosition)
            {
                Phase = 1;
                ArthasInPosition = false;
                TalbotInPosition = false;
            }

            if (ArlosInPosition && LeryssaInPosition)
            {
                ArlosInPosition   = false;
                LeryssaInPosition = false;
                DoSendQuantumText(SAY_THASSARIAN_1, me);
                SetEscortPaused(false);
            }

            if (PhaseTimer <= diff)
            {
                Creature* pTalbot = me->GetCreature(*me, Talbot);
                Creature* pArthas = me->GetCreature(*me, Arthas);
                switch (Phase)
                {
                    case 1:
                        if (pTalbot)
                            pTalbot->SetStandState(UNIT_STAND_STATE_KNEEL);
                        PhaseTimer = 3000;
                        ++Phase;
                        break;
                    case 2:
                        if (pTalbot)
                        {
                            pTalbot->UpdateEntry(NPC_PRINCE_VALANAR, ALLIANCE);
                            pTalbot->SetCurrentFaction(14);
                            pTalbot->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            pTalbot->SetReactState(REACT_PASSIVE);
                        }
                        PhaseTimer = 5000;
                        ++Phase;
                        break;
                    case 3:
                        if (pTalbot)
                            DoSendQuantumText(SAY_TALBOT_1, pTalbot);
                        PhaseTimer = 5000;
                        ++Phase;
                        break;
                    case 4:
                        if (pArthas)
                            DoSendQuantumText(SAY_LICH_1, pArthas);
                        PhaseTimer = 5000;
                        ++Phase;
                        break;
                    case 5:
                        if (pTalbot)
                            DoSendQuantumText(SAY_TALBOT_2, pTalbot);
                        PhaseTimer = 5000;
                        ++Phase;
                        break;
                    case 6:
                        if (Creature* pArlos = me->SummonCreature(NPC_GENERAL_ARLOS, 3745.527100f, 3615.655029f, 473.321533f, 4.447805f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 120000))
                        {
                            Arlos = pArlos->GetGUID();
                            pArlos->SetWalk(true);
                            pArlos->GetMotionMaster()->MovePoint(0, 3735.570068f, 3572.419922f, 477.441010f);
                        }
                        if (Creature* pLeryssa = me->SummonCreature(NPC_LERYSSA, 3749.654541f, 3614.959717f, 473.323486f, 4.524959f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 120000))
                        {
                            Leryssa = pLeryssa->GetGUID();
                            pLeryssa->SetWalk(true);
                            pLeryssa->SetReactState(REACT_PASSIVE);
                            pLeryssa->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            pLeryssa->GetMotionMaster()->MovePoint(0, 3741.969971f, 3571.439941f, 477.441010f);
                        }
                        PhaseTimer = 2000;
                        Phase = 0;
                        break;
                    case 7:
                        DoSendQuantumText(SAY_THASSARIAN_2, me);
                        PhaseTimer = 5000;
                        ++Phase;
                        break;
                    case 8:
                        if (pArthas && pTalbot)
                        {
                            pArthas->SetInFront(me); //The client doesen't update with the new orientation :l
                            pTalbot->SetStandState(UNIT_STAND_STATE_STAND);
                            DoSendQuantumText(SAY_LICH_2, pArthas);
                        }
                        PhaseTimer = 5000;
                        Phase = 9;
                        break;
                   case 9:
                        DoSendQuantumText(SAY_THASSARIAN_3, me);
                        PhaseTimer = 5000;
                        Phase = 10;
                        break;
                   case 10:
                        if (pTalbot)
                            DoSendQuantumText(SAY_TALBOT_3, pTalbot);
                        PhaseTimer = 5000;
                        Phase = 11;
                        break;
                   case 11:
                        if (pArthas)
                            DoSendQuantumText(SAY_LICH_3, pArthas);
                        PhaseTimer = 5000;
                        Phase = 12;
                        break;
                    case 12:
                        if (pTalbot)
                            DoSendQuantumText(SAY_TALBOT_4, pTalbot);
                        PhaseTimer = 2000;
                        Phase = 13;
                        break;
                    case 13:
                        if (pArthas)
                            pArthas->RemoveFromWorld();
                        ++Phase;
                        break;
                    case 14:
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        if (pTalbot)
                        {
                            pTalbot->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            pTalbot->SetReactState(REACT_AGGRESSIVE);
                            pTalbot->CastSpell(me, SPELL_SHADOW_BOLT, false);
                        }
                        PhaseTimer = 1500;
                        ++Phase;
                        break;
                    case 15:
                        me->SetReactState(REACT_AGGRESSIVE);
                        AttackStart(pTalbot);
                        Phase = 0;
                        break;
                    case 16:
                        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUEST_GIVER);
                        PhaseTimer = 20000;
                        ++Phase;
                        break;
                   case 17:
                        if (Creature* pLeryssa = me->GetCreature(*me, Leryssa))
                            pLeryssa->RemoveFromWorld();
                        if (Creature* pArlos= me->GetCreature(*me, Arlos))
                            pArlos->RemoveFromWorld();
                        if (pTalbot)
                            pTalbot->RemoveFromWorld();
                        me->RemoveStandFlags(UNIT_STAND_STATE_SIT);
                        SetEscortPaused(false);
                        PhaseTimer = 0;
                        Phase = 0;
                 }
            }
            else PhaseTimer -= diff;

            if (!UpdateVictim())
                return;
			
			DoMeleeAttackIfReady();
        }

        void JustDied(Unit* /*killer*/)
        {
            if (Creature* pTalbot = me->GetCreature(*me, Talbot))
                pTalbot->RemoveFromWorld();

            if (Creature* pLeryssa = me->GetCreature(*me, Leryssa))
                pLeryssa->RemoveFromWorld();

            if (Creature* pArlos = me->GetCreature(*me, Arlos))
                pArlos->RemoveFromWorld();

            if (Creature* pArthas = me->GetCreature(*me, Arthas))
                pArthas->RemoveFromWorld();
        }
    };

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_LAST_RITES) == QUEST_STATUS_INCOMPLETE && creature->GetAreaId() == 4128)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_T, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                CAST_AI(npc_escortAI, (creature->AI()))->Start(true, false, player->GetGUID());
                CAST_AI(npc_escortAI, (creature->AI()))->SetMaxPlayerDistance(200.0f);
                break;
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_thassarianAI(creature);
    }
};

class npc_image_lich_king : public CreatureScript
{
public:
    npc_image_lich_king() : CreatureScript("npc_image_lich_king") { }

    struct npc_image_lich_kingAI : public QuantumBasicAI
    {
        npc_image_lich_kingAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset()
        {
            me->RestoreFaction();
        }

        void MovementInform(uint32 type, uint32 /*id*/)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (me->IsSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    CAST_AI(npc_thassarian::npc_thassarianAI, CAST_CRE(summoner)->AI())->ArthasInPosition = true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_image_lich_kingAI(creature);
    }
};

class npc_general_arlos : public CreatureScript
{
public:
    npc_general_arlos() : CreatureScript("npc_general_arlos") { }

    struct npc_general_arlosAI : public QuantumBasicAI
    {
        npc_general_arlosAI(Creature* creature) : QuantumBasicAI(creature) {}

        void MovementInform(uint32 type, uint32 /*id*/)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            me->AddUnitState(UNIT_STATE_STUNNED);
            me->CastSpell(me, SPELL_STUN, true);
            if (me->IsSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    CAST_AI(npc_thassarian::npc_thassarianAI, CAST_CRE(summoner)->AI())->ArlosInPosition = true;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_general_arlosAI(creature);
    }
};

enum CounselorTalbot
{
    SPELL_DEFLECTION  = 51009,
    SPELL_SOUL_BLAST  = 50992,

	AREA_NAXXANAR     = 4128,
};

class npc_counselor_talbot : public CreatureScript
{
public:
    npc_counselor_talbot() : CreatureScript("npc_counselor_talbot") { }

    struct npc_counselor_talbotAI : public QuantumBasicAI
    {
        npc_counselor_talbotAI(Creature* creature) : QuantumBasicAI(creature)
        {
            creature->RestoreFaction();
        }

        uint64 LeryssaGUID;
        uint64 ArlosGUID;

        bool bCheck;

        uint32 ShadowBoltTimer;
        uint32 DeflectionTimer;
        uint32 SoulBlastTimer;

        void Reset()
        {
            LeryssaGUID = 0;
            ArlosGUID = 0;
            bCheck = false;
            ShadowBoltTimer = urand(5000, 12000);
            DeflectionTimer = urand(20000, 25000);
            SoulBlastTimer  = urand(12000,18000);
        }

        void MovementInform(uint32 type, uint32 /*id*/)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (me->IsSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    CAST_AI(npc_thassarian::npc_thassarianAI, CAST_CRE(summoner)->AI())->TalbotInPosition = true;
        }

        void UpdateAI(const uint32 diff)
        {
            if (bCheck)
            {
                if (Creature* leryssa = me->FindCreatureWithDistance(NPC_LERYSSA, 50.0f, true))
                    LeryssaGUID = leryssa->GetGUID();

                if (Creature* arlos = me->FindCreatureWithDistance(NPC_GENERAL_ARLOS, 50.0f, true))
                    ArlosGUID = arlos->GetGUID();
                bCheck = false;
            }

            if (!UpdateVictim())
                return;

            if (me->GetAreaId() == AREA_NAXXANAR)
            {
                if (ShadowBoltTimer <= diff)
                {
                    DoCastVictim(SPELL_SHADOW_BOLT);
                    ShadowBoltTimer = urand(5000, 12000);
                }
				else ShadowBoltTimer -= diff;

                if (DeflectionTimer <= diff)
                {
                    DoCastVictim(SPELL_DEFLECTION);
                    DeflectionTimer = urand(20000, 25000);
                }
				else DeflectionTimer -= diff;

                if (SoulBlastTimer <= diff)
                {
                    DoCastVictim(SPELL_SOUL_BLAST);
                    SoulBlastTimer = urand (12000, 18000);
                }
				else SoulBlastTimer -= diff;
            }

            DoMeleeAttackIfReady();
       }

       void JustDied(Unit* killer)
       {
		   if (!LeryssaGUID || !ArlosGUID)
                return;

            Creature* pLeryssa = Unit::GetCreature(*me, LeryssaGUID);
            Creature* pArlos = Unit::GetCreature(*me, ArlosGUID);
            if (!pLeryssa || !pArlos)
                return;

            DoSendQuantumText(SAY_ARLOS_1, pArlos);
            DoSendQuantumText(SAY_ARLOS_2, pArlos);
            DoSendQuantumText(SAY_LERYSSA_1, pLeryssa);
            pArlos->Kill(pArlos, false);
            pLeryssa->RemoveAura(SPELL_STUN);
            pLeryssa->ClearUnitState(UNIT_STATE_STUNNED);
            pLeryssa->SetWalk(false);
            pLeryssa->GetMotionMaster()->MovePoint(0, 3722.114502f, 3564.201660f, 477.441437f);

			if (Player* player = killer->ToPlayer())
				player->RewardPlayerAndGroupAtEvent(NPC_PRINCE_VALANAR, 0);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_counselor_talbotAI(creature);
    }
};

class npc_leryssa : public CreatureScript
{
public:
    npc_leryssa() : CreatureScript("npc_leryssa") { }

    struct npc_leryssaAI : public QuantumBasicAI
    {
        npc_leryssaAI(Creature* creature) : QuantumBasicAI(creature)
        {
            bDone = false;
            Phase = 0;
            PhaseTimer = 0;

            creature->RemoveStandFlags(UNIT_STAND_STATE_SIT);
        }

        bool bDone;

        uint32 Phase;
        uint32 PhaseTimer;

        void MovementInform(uint32 type, uint32 /*uiId*/)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (!bDone)
            {
                if (Creature* pTalbot = me->FindCreatureWithDistance(NPC_PRINCE_VALANAR, 50.0f, true))
                    CAST_AI(npc_counselor_talbot::npc_counselor_talbotAI, pTalbot->GetAI())->bCheck = true;

                me->AddUnitState(UNIT_STATE_STUNNED);
                me->CastSpell(me, SPELL_STUN, true);

                if (me->IsSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                        CAST_AI(npc_thassarian::npc_thassarianAI, summoner->GetAI())->LeryssaInPosition = true;
                bDone = true;
            }
            else
            {
                me->SetStandState(UNIT_STAND_STATE_SIT);
                if (me->IsSummon())
                    if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    summoner->SetStandState(UNIT_STAND_STATE_SIT);
                PhaseTimer = 1500;
                Phase = 1;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            QuantumBasicAI::UpdateAI(diff);

            if (PhaseTimer <= diff)
            {
                switch (Phase)
                {
                    case 1:
                        if (me->IsSummon())
                            if (Unit* pThassarian = me->ToTempSummon()->GetSummoner())
                                DoSendQuantumText(SAY_THASSARIAN_4, pThassarian);
                        PhaseTimer = 5000;
                        ++Phase;
                        break;
                    case 2:
                        DoSendQuantumText(SAY_LERYSSA_2, me);
                        PhaseTimer = 5000;
                        ++Phase;
                        break;
                    case 3:
                        if (me->IsSummon())
                            if (Unit* pThassarian = me->ToTempSummon()->GetSummoner())
                                DoSendQuantumText(SAY_THASSARIAN_5, pThassarian);
                        PhaseTimer = 5000;
                        ++Phase;
                        break;
                    case 4:
                        DoSendQuantumText(SAY_LERYSSA_3, me);
                        PhaseTimer = 5000;
                        ++Phase;
                        break;
                    case 5:
                        if (me->IsSummon())
                            if (Unit* pThassarian = me->ToTempSummon()->GetSummoner())
                        DoSendQuantumText(SAY_THASSARIAN_6, pThassarian);
                        PhaseTimer = 5000;
                        ++Phase;
                        break;
                    case 6:
                        DoSendQuantumText(SAY_LERYSSA_4, me);
                        PhaseTimer = 5000;
                        ++Phase;
                        break;
                    case 7:
                        if (me->IsSummon())
						{
                            if (Unit* pThassarian = me->ToTempSummon()->GetSummoner())
                            {
                                DoSendQuantumText(SAY_THASSARIAN_7, pThassarian);
                                CAST_AI(npc_thassarian::npc_thassarianAI, pThassarian->GetAI())->Phase = 16;
							}
						}
                        PhaseTimer = 5000;
                        Phase = 0;
                        break;
                }
            }
			else PhaseTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_leryssaAI(creature);
    }
};

enum BerylSorcerer
{
    NPC_CAPTURED_BERLY_SORCERER         = 25474,
    NPC_LIBRARIAN_DONATHAN              = 25262,

	SPELL_BLINK                         = 50648, // need implement
	SPELL_FROSTBOLT                     = 9672,

    SPELL_ARCANE_CHAINS                 = 45611,
    SPELL_COSMETIC_CHAINS               = 54324,
    SPELL_COSMETIC_ENSLAVE_CHAINS_SELF  = 45631,
};

class npc_beryl_sorcerer : public CreatureScript
{
public:
    npc_beryl_sorcerer() : CreatureScript("npc_beryl_sorcerer") { }

    struct npc_beryl_sorcererAI : public FollowerAI
    {
        npc_beryl_sorcererAI(Creature* creature) : FollowerAI(creature) {}

        bool bEnslaved;
		uint32 FrostboltTimer;

        void Reset()
        {
            me->SetReactState(REACT_AGGRESSIVE);
            bEnslaved = false;
			FrostboltTimer = 4000;
        }

        void EnterToBattle(Unit* who)
        {
            if (me->IsValidAttackTarget(who))
                AttackStart(who);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_ARCANE_CHAINS && caster->GetTypeId() == TYPE_ID_PLAYER && !HealthAbovePct(50) && !bEnslaved)
            {
                EnterEvadeMode();
                me->SetReactState(REACT_PASSIVE);
                StartFollow(caster->ToPlayer(), 0, 0);
                me->UpdateEntry(NPC_CAPTURED_BERLY_SORCERER, TEAM_NEUTRAL);
                DoCast(me, SPELL_COSMETIC_ENSLAVE_CHAINS_SELF, true);
                caster->ToPlayer()->KilledMonsterCredit(NPC_CAPTURED_BERLY_SORCERER, 0);
                bEnslaved = true;
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            FollowerAI::MoveInLineOfSight(who);

            if (who->GetEntry() == NPC_LIBRARIAN_DONATHAN && me->IsWithinDistInMap(who, INTERACTION_DISTANCE))
            {
                SetFollowComplete();
                me->DisappearAndDie();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FROSTBOLT);
					FrostboltTimer = 3000;
				}
			}
			else FrostboltTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_beryl_sorcererAI(creature);
    }
};

enum ImprisionedBerylSorcerer
{
    SPELL_NEURAL_NEEDLE             = 45634,

    NPC_IMPRISONED_BERYL_SORCERER   = 25478,

    SAY_IMPRISIONED_BERYL_1         = -1571024,
    SAY_IMPRISIONED_BERYL_2         = -1571025,
    SAY_IMPRISIONED_BERYL_3         = -1571026,
    SAY_IMPRISIONED_BERYL_4         = -1571027,
    SAY_IMPRISIONED_BERYL_5         = -1571028,
    SAY_IMPRISIONED_BERYL_6         = -1571029,
    SAY_IMPRISIONED_BERYL_7         = -1571030,
};

class npc_imprisoned_beryl_sorcerer : public CreatureScript
{
public:
    npc_imprisoned_beryl_sorcerer() : CreatureScript("npc_imprisoned_beryl_sorcerer") { }

    struct npc_imprisoned_beryl_sorcererAI : public QuantumBasicAI
    {
        npc_imprisoned_beryl_sorcererAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 RebuffTimer;

        void Reset()
        {
            if (me->GetReactState() != REACT_PASSIVE)
                me->SetReactState(REACT_PASSIVE);

            RebuffTimer = 0;
        }

        void UpdateAI(const uint32 diff)
        {
            UpdateVictim();

            if (RebuffTimer <= diff)
            {
                if (!me->HasAura(SPELL_COSMETIC_ENSLAVE_CHAINS_SELF))
                {
                    DoCast(me, SPELL_COSMETIC_ENSLAVE_CHAINS_SELF);
					RebuffTimer = 180000;
                }
            }
            else RebuffTimer -= diff;

            DoMeleeAttackIfReady();
        }

        void EnterToBattle(Unit* /*who*/){}

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_NEURAL_NEEDLE)
            {
                if (Player* player = caster->ToPlayer())
                    GotStinged(player->GetGUID());
            }
        }

        void GotStinged(uint64 casterGUID)
        {
            if (Player* caster = Player::GetPlayer(*me, casterGUID))
            {
                uint32 step = caster->GetAuraCount(SPELL_NEURAL_NEEDLE) + 1;
                switch (step)
                {
                case 1:
                    DoSendQuantumText(SAY_IMPRISIONED_BERYL_1, me);
                    break;
                case 2:
                    DoSendQuantumText(SAY_IMPRISIONED_BERYL_2, me, caster);
                    break;
                case 3:
                    DoSendQuantumText(SAY_IMPRISIONED_BERYL_3, me);
                    break;
                case 4:
                    DoSendQuantumText(SAY_IMPRISIONED_BERYL_4, me);
                    break;
                case 5:
                    DoSendQuantumText(SAY_IMPRISIONED_BERYL_5, me);
                    break;
                case 6:
                    DoSendQuantumText(SAY_IMPRISIONED_BERYL_6, me, caster);
                    break;
                case 7:
                    DoSendQuantumText(SAY_IMPRISIONED_BERYL_7, me);
                    caster->KilledMonsterCredit(NPC_IMPRISONED_BERYL_SORCERER, 0);
                    break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_imprisoned_beryl_sorcererAI(creature);
    }
};

enum Younger
{
	NPC_MOOTOO_THE_YOUNGER  = 25504,

    QUEST_ESCAPING_THE_MIST = 11664,

    SAY_1   = -1750040,
    SAY_2   = -1750041,
    SAY_3   = -1750042,
    SAY_4   = -1750043,
    SAY_5   = -1750044,
};

class npc_mootoo_the_younger : public CreatureScript
{
public:
    npc_mootoo_the_younger() : CreatureScript("npc_mootoo_the_younger") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_ESCAPING_THE_MIST)
        {
            switch (player->GetTeam())
            {
                case ALLIANCE:
					creature->SetCurrentFaction(FACTION_ESCORTEE_A);
					break;
				case HORDE:
					creature->SetCurrentFaction(FACTION_ESCORTEE_H);
					break;
			}
			creature->SetStandState(UNIT_STAND_STATE_STAND);
            DoSendQuantumText(SAY_1, player);
            CAST_AI(npc_escortAI, (creature->AI()))->Start(true, false, player->GetGUID());
        }
        return true;
    }

    struct npc_mootoo_the_youngerAI : public npc_escortAI
    {
        npc_mootoo_the_youngerAI(Creature* creature) : npc_escortAI(creature) {}

        void Reset()
        {
            SetDespawnAtFar(false);
        }

        void JustDied(Unit* /*killer*/)
        {
            if (Player* player = GetPlayerForEscort())
                player->FailQuest(QUEST_ESCAPING_THE_MIST);
        }

        void WaypointReached(uint32 i)
        {
            Player* player = GetPlayerForEscort();

            if (!player)
                return;

            switch (i)
            {
            case 10:
                me->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
                DoSendQuantumText(SAY_2, me);
                break;
            case 12:
                DoSendQuantumText(SAY_3, me);
                me->HandleEmoteCommand(EMOTE_ONESHOT_LOOT);
                break;
            case 16:
                DoSendQuantumText(SAY_4, me);
                me->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
                break;
            case 20:
                me->SetPhaseMask(1, true);
                DoSendQuantumText(SAY_5, me);
                me->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
                if (player)
                    player->GroupEventHappens(QUEST_ESCAPING_THE_MIST, me);
                SetRun(true);
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mootoo_the_youngerAI(creature);
    }
};

enum Bonker_Togglevolt_Entries
{
    NPC_BONKER_TOGGLEVOLT  = 25589,

    QUEST_GET_ME_OUTA_HERE = 11673,
};

enum Script_Texts_Bonker_Togglevolt
{
    SAY_bonker_1 = -1700002,
    SAY_bonker_2 = -1700003,
};

class npc_bonker_togglevolt : public CreatureScript
{
public:
    npc_bonker_togglevolt() : CreatureScript("npc_bonker_togglevolt") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_GET_ME_OUTA_HERE)
        {
            creature->SetStandState(UNIT_STAND_STATE_STAND);
            DoSendQuantumText(SAY_bonker_2, creature, player);
            CAST_AI(npc_escortAI, (creature->AI()))->Start(true, true, player->GetGUID());
        }
        return true;
    }

    struct npc_bonker_togglevoltAI : public npc_escortAI
    {
        npc_bonker_togglevoltAI(Creature* creature) : npc_escortAI(creature) {}
        uint32 Bonker_agro;

        void Reset()
        {
            Bonker_agro=0;
            SetDespawnAtFar(false);
        }

        void JustDied(Unit* /*killer*/)
        {
            if (Player* player = GetPlayerForEscort())
                player->FailQuest(QUEST_GET_ME_OUTA_HERE);
        }

        void UpdateEscortAI(const uint32 /*diff*/)
        {
            if (GetAttack() && UpdateVictim())
            {
                if (Bonker_agro == 0)
                {
                    DoSendQuantumText(SAY_bonker_1, me);
                    Bonker_agro++;
                }
                DoMeleeAttackIfReady();
            }
            else Bonker_agro=0;
        }

        void WaypointReached(uint32 i)
        {
            Player* player = GetPlayerForEscort();

            if (!player)
                return;

            switch (i)
            {
                case 29:
					if (player)
						player->GroupEventHappens(QUEST_GET_ME_OUTA_HERE, me);
					break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bonker_togglevoltAI(creature);
    }
};

enum HelpThemselves
{
    QUEST_CANNOT_HELP_THEMSELVES                  = 11876,

    GO_MAMMOTH_TRAP_1                             = 188022,
    GO_MAMMOTH_TRAP_2                             = 188024,
    GO_MAMMOTH_TRAP_3                             = 188025,
    GO_MAMMOTH_TRAP_4                             = 188026,
    GO_MAMMOTH_TRAP_5                             = 188027,
    GO_MAMMOTH_TRAP_6                             = 188028,
    GO_MAMMOTH_TRAP_7                             = 188029,
    GO_MAMMOTH_TRAP_8                             = 188030,
    GO_MAMMOTH_TRAP_9                             = 188031,
    GO_MAMMOTH_TRAP_10                            = 188032,
    GO_MAMMOTH_TRAP_11                            = 188033,
    GO_MAMMOTH_TRAP_12                            = 188034,
    GO_MAMMOTH_TRAP_13                            = 188035,
    GO_MAMMOTH_TRAP_14                            = 188036,
    GO_MAMMOTH_TRAP_15                            = 188037,
    GO_MAMMOTH_TRAP_16                            = 188038,
    GO_MAMMOTH_TRAP_17                            = 188039,
    GO_MAMMOTH_TRAP_18                            = 188040,
    GO_MAMMOTH_TRAP_19                            = 188041,
    GO_MAMMOTH_TRAP_20                            = 188042,
    GO_MAMMOTH_TRAP_21                            = 188043,
    GO_MAMMOTH_TRAP_22                            = 188044,

	MammothTrapsNum                               = 22,
};

const uint32 MammothTraps[MammothTrapsNum] =
{
    GO_MAMMOTH_TRAP_1,
	GO_MAMMOTH_TRAP_2,
	GO_MAMMOTH_TRAP_3,
	GO_MAMMOTH_TRAP_4,
	GO_MAMMOTH_TRAP_5,
    GO_MAMMOTH_TRAP_6,
	GO_MAMMOTH_TRAP_7,
	GO_MAMMOTH_TRAP_8,
	GO_MAMMOTH_TRAP_9,
	GO_MAMMOTH_TRAP_10,
    GO_MAMMOTH_TRAP_11,
	GO_MAMMOTH_TRAP_12,
	GO_MAMMOTH_TRAP_13,
	GO_MAMMOTH_TRAP_14,
	GO_MAMMOTH_TRAP_15,
    GO_MAMMOTH_TRAP_16,
	GO_MAMMOTH_TRAP_17,
	GO_MAMMOTH_TRAP_18,
	GO_MAMMOTH_TRAP_19,
	GO_MAMMOTH_TRAP_20,
    GO_MAMMOTH_TRAP_21,
	GO_MAMMOTH_TRAP_22,
};

class npc_trapped_mammoth_calf : public CreatureScript
{
public:
    npc_trapped_mammoth_calf() : CreatureScript("npc_trapped_mammoth_calf") { }

    struct npc_trapped_mammoth_calfAI : public QuantumBasicAI
    {
        npc_trapped_mammoth_calfAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 Timer;
        bool bStarted;

        void Reset()
        {
            Timer = 1500;
            bStarted = false;

            GameObject* pTrap;
            for (uint8 i = 0; i < MammothTrapsNum; ++i)
            {
                pTrap = me->FindGameobjectWithDistance(MammothTraps[i], 11.0f);
                if (pTrap)
                {
                    pTrap->SetGoState(GO_STATE_ACTIVE);
                    return;
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (bStarted)
            {
                if (Timer <= diff)
                {
                    Position pos;
                    me->GetRandomNearPosition(pos, 10.0f);
                    me->GetMotionMaster()->MovePoint(0, pos);
                    bStarted = false;
                }
                else Timer -= diff;
            }
        }

        void DoAction(const int32 param)
        {
            if (param == 1)
                bStarted = true;
        }

        void MovementInform(uint32 type, uint32 /*uiId*/)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            me->DisappearAndDie();
            GameObject* pTrap;
            for (uint8 i = 0; i < MammothTrapsNum; ++i)
            {
                pTrap = me->FindGameobjectWithDistance(MammothTraps[i], 11.0f);
                if (pTrap)
                {
                    pTrap->SetLootState(GO_JUST_DEACTIVATED);
                    return;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_trapped_mammoth_calfAI(creature);
    }
};

enum NotSoBig
{
    QUEST_YOU_RE_NOT_SO_BIG_NOW           = 11653,
    SPELL_AURA_NOTSOBIG_1                 = 45672,
    SPELL_AURA_NOTSOBIG_2                 = 45673,
    SPELL_AURA_NOTSOBIG_3                 = 45677,
    SPELL_AURA_NOTSOBIG_4                 = 45681,
};

class npc_magmoth_crusher : public CreatureScript
{
public:
    npc_magmoth_crusher() : CreatureScript("npc_magmoth_crusher") { }

    struct npc_magmoth_crusherAI : public QuantumBasicAI
    {
        npc_magmoth_crusherAI(Creature* creature) : QuantumBasicAI(creature) {}

        void JustDied(Unit* killer)
        {
            if (killer->GetTypeId() == TYPE_ID_PLAYER &&
                CAST_PLR(killer)->GetQuestStatus(QUEST_YOU_RE_NOT_SO_BIG_NOW) == QUEST_STATUS_INCOMPLETE &&
                (me->HasAura(SPELL_AURA_NOTSOBIG_1) || me->HasAura(SPELL_AURA_NOTSOBIG_2) ||
                me->HasAura(SPELL_AURA_NOTSOBIG_3) || me->HasAura(SPELL_AURA_NOTSOBIG_4)))
            {
                Quest const* QuestInfo = sObjectMgr->GetQuestTemplate(QUEST_YOU_RE_NOT_SO_BIG_NOW);
                if (QuestInfo)
                    CAST_PLR(killer)->KilledMonsterCredit(QuestInfo->RequiredNpcOrGo[0], 0);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_magmoth_crusherAI(creature);
    }
};

enum Cockroaches
{
	QUEST_BURY_THOSE_COCKROACHES           = 11608,
	SPELL_SEAFORIUM_DEPTH_CHARGE_EXPLOSION = 45502,
};

class npc_seaforium_depth_charge : public CreatureScript
{
public:
    npc_seaforium_depth_charge() : CreatureScript("npc_seaforium_depth_charge") { }

    struct npc_seaforium_depth_chargeAI : public QuantumBasicAI
    {
        npc_seaforium_depth_chargeAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ExplosionTimer;
        void Reset()
        {
            ExplosionTimer = urand(5000, 10000);
        }
        void UpdateAI(const uint32 diff)
        {
            if (ExplosionTimer < diff)
            {
                DoCast(SPELL_SEAFORIUM_DEPTH_CHARGE_EXPLOSION);
                for (uint8 i = 0; i < 4; ++i)
                {
                    if (Creature* cCredit = me->FindCreatureWithDistance(25402 + i, 10.0f))//25402-25405 credit markers
                    {
                        if (Unit* uOwner = me->GetOwner())
                        {
                            Player* owner = uOwner->ToPlayer();
                            if (owner && owner->GetQuestStatus(QUEST_BURY_THOSE_COCKROACHES) == QUEST_STATUS_INCOMPLETE)
                                owner->KilledMonsterCredit(cCredit->GetEntry(), cCredit->GetGUID());
                        }
                    }
                }
                me->Kill(me);
                return;
            }
			else ExplosionTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_seaforium_depth_chargeAI(creature);
    }
};

enum Valiancekeepcannons
{
    GO_VALIANCE_KEEP_CANNON_1 = 187560,
    GO_VALIANCE_KEEP_CANNON_2 = 188692,
};

class npc_valiance_keep_cannoneer : public CreatureScript
{
public:
    npc_valiance_keep_cannoneer() : CreatureScript("npc_valiance_keep_cannoneer") { }

    struct npc_valiance_keep_cannoneerAI : public QuantumBasicAI
    {
        npc_valiance_keep_cannoneerAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 Timer;

        void Reset()
        {
			Timer = urand(13000, 18000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (Timer <= diff)
            {
                me->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
                GameObject* pCannon = me->FindGameobjectWithDistance(GO_VALIANCE_KEEP_CANNON_1, 10.0f);
				pCannon = me->FindGameobjectWithDistance(GO_VALIANCE_KEEP_CANNON_2, 10.0f);
				pCannon->Use(me);
                Timer = urand(13000, 18000);
            }
            else Timer -= diff;

            if (!UpdateVictim())
                return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_valiance_keep_cannoneerAI(creature);
    }
};

enum Spells
{
    SPELL_TRANSITUS_SHIELD_BEAM  = 48310,
};

enum NPCs
{
    NPC_TRANSITUS_SHIELD_DUMMY   = 27306,
    NPC_WARMAGE_HOLLISTER        = 27906,
    NPC_WARMAGE_CALANDRA         = 27173,
    NPC_WARMAGE_WATKINS          = 27904,
};

class npc_warmage_coldarra : public CreatureScript
{
public:
    npc_warmage_coldarra() : CreatureScript("npc_warmage_coldarra") { }

    struct npc_warmage_coldarraAI : public QuantumBasicAI
    {
        npc_warmage_coldarraAI(Creature* creature) : QuantumBasicAI(creature){}

        uint32 Timer;

        void Reset()
        {
            Timer = 0;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void AttackStart(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (Timer <= diff)
            {
                std::list<Creature*> orbList;
                GetCreatureListWithEntryInGrid(orbList, me, NPC_TRANSITUS_SHIELD_DUMMY, 32.0f);

                switch (me->GetEntry())
                {
                    case NPC_WARMAGE_HOLLISTER:
                    {
                        if (!orbList.empty())
                        {
                            for (std::list<Creature*>::const_iterator itr = orbList.begin(); itr != orbList.end(); ++itr)
                            {
                                if (Creature* pOrb = *itr)
								{
                                    if (pOrb->GetPositionY() > 6680)
									{
                                        DoCast(pOrb, SPELL_TRANSITUS_SHIELD_BEAM);
									}
								}
                            }
                        }
                        Timer = urand(90000, 120000);
					}
					break;
					case NPC_WARMAGE_CALANDRA:
                    {
                        if (!orbList.empty())
                        {
                            for (std::list<Creature*>::const_iterator itr = orbList.begin(); itr != orbList.end(); ++itr)
                            {
                                if (Creature* pOrb = *itr)
                                    if ((pOrb->GetPositionY() < 6680) && (pOrb->GetPositionY() > 6630))
                                        DoCast(pOrb, SPELL_TRANSITUS_SHIELD_BEAM);
                            }
                        }
                        Timer = urand(90000, 120000);
                    }
                        break;
                    case NPC_WARMAGE_WATKINS:
                    {
                        if (!orbList.empty())
                        {
                            for (std::list<Creature*>::const_iterator itr = orbList.begin(); itr != orbList.end(); ++itr)
                            {
                                if (Creature* pOrb = *itr)
                                    if (pOrb->GetPositionY() < 6630)
                                        DoCast(pOrb, SPELL_TRANSITUS_SHIELD_BEAM);
                            }
                        }
                        Timer = urand(90000, 120000);
                    }
					break;
                }
            }
            else Timer -= diff;

            QuantumBasicAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_warmage_coldarraAI(creature);
    }
};

enum HiddenCultist
{
    SPELL_SHROUD_OF_THE_DEATH_CULTIST           = 46077, // not working
    SPELL_RIGHTEOUS_VISION                      = 46078, // player aura

    QUEST_THE_HUNT_IS_ON                        = 11794,

    GOSSIP_TEXT_SALTY_JOHN_THORPE               = 12529,
    GOSSIP_TEXT_GUARD_MITCHELSS                 = 12530,
    GOSSIP_TEXT_TOM_HEGGER                      = 12528,

    NPC_TOM_HEGGER                              = 25827,
    NPC_SALTY_JOHN_THORPE                       = 25248,
    NPC_GUARD_MITCHELLS                         = 25828,

    SAY_HIDDEN_CULTIST_1                        = -1571044,
    SAY_HIDDEN_CULTIST_2                        = -1571045,
    SAY_HIDDEN_CULTIST_3                        = -1571046,
    SAY_HIDDEN_CULTIST_4                        = -1571047,
};

const char* GOSSIP_ITEM_TOM_HEGGER = "What do you know about the Cult of the Damned?";
const char* GOSSIP_ITEM_GUARD_MITCHELLS = "How long have you worked for the Cult of the Damned?";
const char* GOSSIP_ITEM_SALTY_JOHN_THORPE = "I have a reason to believe you're involved in the cultist activity";

class npc_hidden_cultist : public CreatureScript
{
public:
    npc_hidden_cultist() : CreatureScript("npc_hidden_cultist") { }

    struct npc_hidden_cultistAI : public QuantumBasicAI
    {
        npc_hidden_cultistAI(Creature* creature) : QuantumBasicAI(creature)
        {
           EmoteState = creature->GetUInt32Value(UNIT_NPC_EMOTE_STATE);
           NpcFlags = creature->GetUInt32Value(UNIT_NPC_FLAGS);
        }

        uint32 EmoteState;
        uint32 NpcFlags;
        uint32 EventTimer;
        uint8 EventPhase;
        uint64 PlayerGUID;

        void Reset()
        {
            if (EmoteState)
                me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EmoteState);

            if (NpcFlags)
                me->SetUInt32Value(UNIT_NPC_FLAGS, NpcFlags);

            EventTimer = 0;
            EventPhase = 0;
            PlayerGUID = 0;

            DoCast(SPELL_SHROUD_OF_THE_DEATH_CULTIST);
            me->RestoreFaction();
        }

        void DoAction(const int32 /*iParam*/)
        {
            me->StopMoving();
            me->SetUInt32Value(UNIT_NPC_FLAGS, 0);
            if (Player* player = me->GetPlayer(*me, PlayerGUID))
            {
                me->SetInFront(player);
                me->SendMovementFlagUpdate();
            }
            EventTimer = 3000;
            EventPhase = 1;
        }

        void SetGUID(uint64 Guid, int32 /*iId*/)
        {
            PlayerGUID = Guid;
        }

        void AttackPlayer()
        {
            me->SetCurrentFaction(14);
            if (Player* player = me->GetPlayer(*me, PlayerGUID))
                me->AI()->AttackStart(player);
        }

        void UpdateAI(const uint32 diff)
        {
            if (EventTimer && EventTimer <= diff)
            {
                switch (EventPhase)
                {
                    case 1:
                        switch (me->GetEntry())
                        {
                            case NPC_SALTY_JOHN_THORPE:
                                me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, 0);
                                DoSendQuantumText(SAY_HIDDEN_CULTIST_1, me);
                                EventTimer = 5000;
                                EventPhase = 2;
                                break;
                            case NPC_GUARD_MITCHELLS:
                                DoSendQuantumText(SAY_HIDDEN_CULTIST_2, me);
                                EventTimer = 5000;
                                EventPhase = 2;
                                break;
                            case NPC_TOM_HEGGER:
                                DoSendQuantumText(SAY_HIDDEN_CULTIST_3, me);
                                EventTimer = 5000;
                                EventPhase = 2;
                                break;
                        }
                        break;
                    case 2:
                        switch (me->GetEntry())
                        {
                            case NPC_SALTY_JOHN_THORPE:
                                DoSendQuantumText(SAY_HIDDEN_CULTIST_4, me);
                                if (Player* player = me->GetPlayer(*me, PlayerGUID))
                                {
                                    me->SetInFront(player);
                                    me->SendMovementFlagUpdate();
                                }
                                EventTimer = 3000;
                                EventPhase = 3;
                                break;
                            case NPC_GUARD_MITCHELLS:
                            case NPC_TOM_HEGGER:
                                AttackPlayer();
                                EventPhase = 0;
                                break;
                        }
                        break;
                    case 3:
                        if (me->GetEntry() == NPC_SALTY_JOHN_THORPE)
                        {
                            AttackPlayer();
                            EventPhase = 0;
                        }
                        break;
                }
            }
			else EventTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hidden_cultistAI(creature);
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        uint32 GossipText = 0;
        const char* charGossipItem;

        switch (creature->GetEntry())
        {
            case NPC_TOM_HEGGER:
                GossipText = GOSSIP_TEXT_TOM_HEGGER;
                charGossipItem = GOSSIP_ITEM_TOM_HEGGER;
                break;
            case NPC_SALTY_JOHN_THORPE:
                GossipText = GOSSIP_TEXT_SALTY_JOHN_THORPE;
                charGossipItem = GOSSIP_ITEM_SALTY_JOHN_THORPE;
                break;
            case NPC_GUARD_MITCHELLS:
                GossipText = GOSSIP_TEXT_GUARD_MITCHELSS;
                charGossipItem = GOSSIP_ITEM_GUARD_MITCHELLS;
                break;
            default:
                charGossipItem = "";
                return false;
        }

        if (player->HasAura(SPELL_RIGHTEOUS_VISION) && player->GetQuestStatus(QUEST_THE_HUNT_IS_ON) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, charGossipItem, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        if (creature->IsVendor())
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_VENDOR_BROWSE_YOUR_GOODS), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        player->SEND_GOSSIP_MENU(GossipText, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->AI()->SetGUID(player->GetGUID());
            creature->AI()->DoAction(1);
        }

        if (action == GOSSIP_ACTION_TRADE)
            player->GetSession()->SendListInventory(creature->GetGUID());

        return true;
    }
};

enum ReconPilot
{
    QUEST_EMERGENCY_PROTOCOL_C            = 11795,
    QUEST_EMERGENCY_SUPPLIES              = 11887,
    SPELL_SUMMON_INSIGNIA                 = 46166,
    SPELL_GIVE_EMERGENCY_KIT              = 46362,
    GOSSIP_TEXT_PILOT                     = 12489,
};

#define GOSSIP_ITEM_PILOT_1 "Search the body for the pilot's insignia."
#define GOSSIP_ITEM_PILOT_2 "Search the body for the pilot's emergency toolkit."

class npc_recon_pilot : public CreatureScript
{
public:
    npc_recon_pilot() : CreatureScript("npc_recon_pilot") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_EMERGENCY_PROTOCOL_C) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_PILOT_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        if (player->GetQuestStatus(QUEST_EMERGENCY_SUPPLIES) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_PILOT_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

        player->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_PILOT, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                creature->CastSpell(player, SPELL_SUMMON_INSIGNIA, true);
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                creature->CastSpell(player, SPELL_GIVE_EMERGENCY_KIT, true);
                break;
        }
        player->CLOSE_GOSSIP_MENU();
        creature->DespawnAfterAction();
        return true;
    }
};

enum SteamRager
{
    SPELL_STEAM_BLAST       = 50375,
    SPELL_ENERGY_TRANSFER   = 46399,
	SPELL_STEAM_AURA        = 36151,
    QUEST_POWER_OF_ELEMENTS = 11893,
    NPC_WINDSOUL_TOTEM      = 25987,
};

class npc_steam_rager : public CreatureScript
{
public:
    npc_steam_rager() : CreatureScript("npc_steam_rager") { }

    struct npc_steam_ragerAI : public QuantumBasicAI
    {
        npc_steam_ragerAI(Creature* creature) : QuantumBasicAI(creature) { }

        uint32 SteamBlastTimer;

        void Reset()
        {
			DoCast(me, SPELL_STEAM_AURA);
            SteamBlastTimer = urand(3000, 5000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (SteamBlastTimer <= diff)
            {
                DoCastVictim(SPELL_STEAM_BLAST, false);
                SteamBlastTimer = urand(7000, 10000);
            }
			else SteamBlastTimer -= diff;

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* killer)
        {
            if (killer->GetTypeId() == TYPE_ID_PLAYER)
            {
                if (killer->ToPlayer()->GetQuestStatus(QUEST_POWER_OF_ELEMENTS) == QUEST_STATUS_INCOMPLETE)
                {
                    if (Unit* totem = me->FindCreatureWithDistance(NPC_WINDSOUL_TOTEM, 25.0f))
                    {
                        DoCast(totem, SPELL_ENERGY_TRANSFER, true);
                        killer->ToPlayer()->RewardPlayerAndGroupAtEvent(NPC_WINDSOUL_TOTEM, killer);
                    }
                }
            }
        }
	};

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_steam_ragerAI(creature);
    }
};

enum RaeloraszMisc
{   
    SPELL_PUSH_ARCANE_PRISON     = 46764,
    ITEM_AUGMENTED_ARCANE_PRISON = 35671,
    QUEST_THE_CELL               = 11943,
    QUEST_MUSTERING_THE_REDS     = 11967,
    GOSSIP_MENU_1                =  9283,
    GOSSIP_MENU_2                =  9284,
 };

#define GOSSIP_ITEM_PRISON    "I've lost my Arcane Prison Item, please give me a new one."
#define GOSSIP_ITEM_MALYGOS_0 "What is the cause of this conflict?"
#define GOSSIP_ITEM_MALYGOS_1 "Who is Malygos?"

class npc_raelorasz : public CreatureScript
{
    public:
        npc_raelorasz() : CreatureScript("npc_raelorasz") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            if (!player->HasItemCount(ITEM_AUGMENTED_ARCANE_PRISON, 1, true) && player->GetQuestStatus(QUEST_THE_CELL) == QUEST_STATUS_REWARDED && !player->GetQuestRewardStatus(QUEST_MUSTERING_THE_REDS))
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_PRISON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+0);
                
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_MALYGOS_0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);           
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF+0:
                    player->CLOSE_GOSSIP_MENU();
                    player->CastSpell(player, SPELL_PUSH_ARCANE_PRISON);
                    break;
                case GOSSIP_ACTION_INFO_DEF+1:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_MALYGOS_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                    player->SEND_GOSSIP_MENU(GOSSIP_MENU_1, creature->GetGUID());
                    break;
                case GOSSIP_ACTION_INFO_DEF+2:
                    player->SEND_GOSSIP_MENU(GOSSIP_MENU_2, creature->GetGUID());
                    break;
            }
            return true;
		}
};

enum CaravanData
{
	QUEST_PLAN_B           = 11658,
	QUEST_THE_HONORED_DEAD = 11593,
	ITEM_WARSONG_OUTFIT    = 34842,
	SPELL_RAGEFIST_TORCH   = 45474,
	GOSSIP_TEXT_GUARD      = 12388,
	NPC_BURNED_CORPSE_KC   = 25342,
};

#define GOSSIP_ITEM_GUARD_1 "Search the body for the guard's outfit"
#define GOSSIP_ITEM_GUARD_2 "Burn corpse to purify its soul"

class npc_fallen_caravan_guard : public CreatureScript
{
public:
    npc_fallen_caravan_guard() : CreatureScript("npc_fallen_caravan_guard") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
		if (player->GetQuestStatus(QUEST_PLAN_B) == QUEST_STATUS_INCOMPLETE)
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GUARD_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        if (player->GetQuestStatus(QUEST_THE_HONORED_DEAD) == QUEST_STATUS_INCOMPLETE)
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GUARD_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

        player->PlayerTalkClass->SendGossipMenu(GOSSIP_TEXT_GUARD, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->AddItem(ITEM_WARSONG_OUTFIT,1);
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->CastSpell(creature,SPELL_RAGEFIST_TORCH,false);
                player->KilledMonsterCredit(NPC_BURNED_CORPSE_KC,0);
                break;
        }
        player->CLOSE_GOSSIP_MENU();
        creature->DespawnAfterAction();
        return true;
    }
};

enum KeristraszaData
{
    QUEST_SPRINGING_TRAP  = 11969,

    NPC_KERISTRASZA       = 26237,
    NPC_SARAGOSA          = 26299,
    NPC_MALYGOS           = 26310,
    GO_SIGNAL_FIRE        = 194151,

    SPELL_RAELORASZ_SPARK = 62272,
    SPELL_FIRE_CORPSE     = 39199,
    SPELL_FIRE_BREATH     = 31962,
    SPELL_MALYGOS_EARTHQ  = 46853,
    SPELL_TAXI_KERISTASZA = 46814,
    SPELL_ICE_BLOCK       = 56644,
    SPELL_KR_FROSTBOLT    = 61461,

    SAY_KERISTRASZA_1     = -1002030,
    YELL_KERISTRASZA_1    = -1002031,
    YELL_KERISTRASZA_2    = -1002032,
    YELL_KERISTRASZA_3    = -1002033,
    SAY_KERISTRASZA_2     = -1002034,

    //After Fligth
    YELL_MALYGOS_1        = -1002035,
    SAY_KERISTRASZA_3     = -1002036,
    YELL_MALYGOS_2        = -1002037,
    YELL_KERISTRASZA_4    = -1002038,
};

const Position posKeristrasza[6] =
{
    {4157.00f, 7035.00f, 215.87f, 0.00f}, // Summon position
    {4063.72f, 7084.12f, 174.86f, 0.00f}, // Land position
    {4054.51f, 7084.29f, 168.12f, 0.00f}, // Burn Corpse positon
    {4048.90f, 7083.94f, 168.21f, 0.00f}, // Saragosa Corpse Spawn
    {3800.47f, 6557.50f, 170.98f, 1.55f}, // Keristrasza 2? Spawn
    {3791.76f, 6603.61f, 179.91f, 0.00f}, // Malygos Spawn
};

class npc_signal_fire : public CreatureScript
{
public:
    npc_signal_fire() : CreatureScript("npc_signal_fire") { }

    struct npc_signal_fireAI : public QuantumBasicAI
    {
		npc_signal_fireAI(Creature* creature) : QuantumBasicAI(creature) { }

        EventMap events;
        Creature* pKeristrasza;
        Creature* pSaragosa;
        Player* player;

        bool eventRunning;

        void Reset()
        {
            events.Reset();
            pKeristrasza, pSaragosa, player = 0;
            eventRunning = false;
            // Unlit signal
            if (GameObject* go = me->FindGameobjectWithDistance(GO_SIGNAL_FIRE, 2.0f))
                go->SetGoState(GO_STATE_READY);
        }

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (eventRunning)
                return;

            if (spell->Id == SPELL_RAELORASZ_SPARK)
            {
                player = caster->ToPlayer();

                if (GameObject* go = me->FindGameobjectWithDistance(GO_SIGNAL_FIRE, 2.0f))
                    go->SetGoState(GO_STATE_ACTIVE);

                if (Creature* pFind = me->SummonCreature(NPC_KERISTRASZA, posKeristrasza[0]))
                {
                    pKeristrasza = pFind;
                    pFind->SetCanFly(true);
                    pFind->GetMotionMaster()->MovePoint(1, posKeristrasza[1]);
                }
                eventRunning = true;
                events.ScheduleEvent(1, 6000);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);

            switch (events.ExecuteEvent())
            {
            case 1:
                pKeristrasza->SetCanFly(false);
                DoSendQuantumText(SAY_KERISTRASZA_1, pKeristrasza, player);
                events.ScheduleEvent(2, 3000);
                break;
            case 2:
                DoSendQuantumText(YELL_KERISTRASZA_1, pKeristrasza);
                pKeristrasza->GetMotionMaster()->MovePoint(1, posKeristrasza[2]);
                events.ScheduleEvent(3, 5000);
                break;
            case 3:
                DoSendQuantumText(YELL_KERISTRASZA_2, pKeristrasza);
                // Summon Saragosa and make her die
                if (Creature* pCorpse = me->SummonCreature(NPC_SARAGOSA, posKeristrasza[3]))
                    pSaragosa = pCorpse;
                events.ScheduleEvent(4, 3000);
                break;
            case 4:
                DoSendQuantumText(YELL_KERISTRASZA_3, pKeristrasza);
                pKeristrasza->CastSpell(pSaragosa, SPELL_FIRE_BREATH, true);
                events.ScheduleEvent(5, 1000);
                break;
            case 5:
                me->AddAura(SPELL_FIRE_CORPSE, pSaragosa);
                events.ScheduleEvent(6, 1000);
                break;
            case 6:
                player->CastSpell(player, SPELL_MALYGOS_EARTHQ, true);
                events.ScheduleEvent(7, 3000);
                break;
            case 7:
                DoSendQuantumText(SAY_KERISTRASZA_2, pKeristrasza, player);
                events.ScheduleEvent(8, 3000);
                break;
            case 8:
                player->CastSpell(player, SPELL_TAXI_KERISTASZA, true);
                pKeristrasza->AI()->SetGUID(player->GetGUID());
                pSaragosa->DespawnAfterAction(10000);
                events.ScheduleEvent(9, 20000);
                break;
            case 9:
                Reset();
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_signal_fireAI(creature);
    }
};

class npc_keristrasza_coldarra : public CreatureScript
{
public:
    npc_keristrasza_coldarra() : CreatureScript("npc_keristrasza_coldarra") { }

    struct npc_keristrasza_coldarraAI : public QuantumBasicAI
    {
        npc_keristrasza_coldarraAI(Creature* creature) : QuantumBasicAI(creature) { }

        EventMap events;
        uint64 uiPlayer;
        bool waiting;
        bool FinishedWay;
        Creature* pMalygos;

        void SetGUID(uint64 guid, int32 id)
        {
            me->NearTeleportTo(posKeristrasza[4].GetPositionX(), posKeristrasza[4].GetPositionY(), posKeristrasza[4].GetPositionZ(), posKeristrasza[4].GetOrientation());
            me->SetVisible(false);
            uiPlayer = guid;
            waiting = true;
        }

        void Reset()
        {
            events.Reset();
            me->SetSpeed(MOVE_FLIGHT, 3.2f, true);
            pMalygos = 0;
            uiPlayer = 0;
            waiting = false;
            FinishedWay = false;
            me->SetReactState(REACT_PASSIVE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!waiting)
                return;

            if (!FinishedWay)
            {
                if (Player* player = me->GetPlayer(*me, uiPlayer))
                {
                    if (!player->IsInFlight())
                    {
                        if (me->IsWithinDist(player, 10.0f))
                        {
                            FinishedWay = true;
                            me->SetVisible(true);
                            player->ToPlayer()->KilledMonsterCredit(NPC_KERISTRASZA, 0);
                            events.ScheduleEvent(1, 0);
                        }
						else me->DespawnAfterAction(0);
                    }
                    else return;
                }
            }

            events.Update(diff);

            switch (events.ExecuteEvent())
            {
            case 1:
                me->SetCanFly(false);
                if (Player* player = me->GetPlayer(*me, uiPlayer))
                    DoSendQuantumText(SAY_KERISTRASZA_3, me, player);

                events.ScheduleEvent(2, 5000);
                break;
            case 2:
                if (Creature* pSumm = me->SummonCreature(NPC_MALYGOS, posKeristrasza[5]))
                {
                    pMalygos = pSumm;
                    pSumm->SetCanFly(true);
                    pSumm->SetReactState(REACT_PASSIVE);
                    pSumm->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                    pSumm->SetObjectScale(0.4f);
                    DoSendQuantumText(YELL_MALYGOS_2, pMalygos);
                    pMalygos->SetUInt64Value(UNIT_FIELD_TARGET, me->GetGUID());
                    me->SetUInt64Value(UNIT_FIELD_TARGET, pMalygos->GetGUID());
                }
                events.ScheduleEvent(3, 6000);
                break;
            case 3:
                DoSendQuantumText(YELL_KERISTRASZA_4, me);
                me->AddAura(SPELL_ICE_BLOCK, me);

                if (pMalygos)
					pMalygos->DespawnAfterAction(7000);
				me->DespawnAfterAction(7000);
                break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_keristrasza_coldarraAI(creature);
    }
};

enum MammothDestroyer
{
	QUEST_KAW_THE_MAMMOTH_DESTROYER = 11879,
    SPELL_DROP_WAR_HALBERD          = 46310,
	SAY_AGGRO                       = 0,
};

class npc_kaw_the_mammoth_destroyer : public CreatureScript
{
public:
    npc_kaw_the_mammoth_destroyer() : CreatureScript("npc_kaw_the_mammoth_destroyer") { }

    struct npc_kaw_the_mammoth_destroyerAI : public QuantumBasicAI
    {
        npc_kaw_the_mammoth_destroyerAI(Creature* creature) : QuantumBasicAI(creature) {}

		void EnterToBattle(Unit* /*who*/)
		{
			Talk(SAY_AGGRO);
		}

		void JustDied(Unit* /*killer*/)
        {
			DoCast(me, SPELL_DROP_WAR_HALBERD);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
	};

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_kaw_the_mammoth_destroyerAI(creature);
    }
};

enum OilPool
{
	SPELL_OIL_COAT             = 45948,
	SPELL_OIL_COAT_SMALL       = 46011,
	SPELL_COLLECT_OIL          = 45990,
	SPELL_SUMMON_OIL_COLLECTOR = 45991,
	NPC_OIL_KILL_CREDIT        = 25781,
};

class npc_oil_pool : public CreatureScript
{
public:
    npc_oil_pool() : CreatureScript("npc_oil_pool") {}

    struct npc_oil_poolAI : public QuantumBasicAI
    {
        npc_oil_poolAI(Creature* creature) : QuantumBasicAI(creature) {}

		void Reset()
		{
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

			DoCast(me, SPELL_OIL_COAT_SMALL, true);
		}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_COLLECT_OIL)
            {
				if (Player* player = caster->ToPlayer())
				{
					DoCast(me, SPELL_SUMMON_OIL_COLLECTOR, true);
					caster->ToPlayer()->KilledMonsterCredit(NPC_OIL_KILL_CREDIT, 0);
				}

				me->DespawnAfterAction();
			}
        }

		void UpdateAI(uint32 const /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_oil_poolAI(creature);
    }
};

enum OilStainedWolf
{
	SPELL_THROW_WOLF_BAIT       = 53326,
	SPELL_SUMMON_WOLF_DROPPINGS = 46075,
	SPELL_OIL_COAT_AURA         = 45948,
	SPELL_OILY_COAT             = 50280,
	FACTION_FRIENDLY            = 35,
	FACTION_HATED               = 16,
};

class npc_oil_stained_wolf : public CreatureScript
{
public:
    npc_oil_stained_wolf() : CreatureScript("npc_oil_stained_wolf") {}

    struct npc_oil_stained_wolfAI : public QuantumBasicAI
    {
        npc_oil_stained_wolfAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 OilyCoatTimer;

		void Reset()
		{
			OilyCoatTimer = 500;

			me->AddAura(SPELL_OIL_COAT_AURA, me);
			me->SetCurrentFaction(FACTION_HATED);
		}

		void JustDied(Unit* /*killer*/)
		{
			me->RemoveAura(SPELL_OIL_COAT_AURA);
		}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_THROW_WOLF_BAIT)
            {
				if (Player* player = caster->ToPlayer())
				{
					me->SetCurrentFaction(FACTION_FRIENDLY);
					DoCast(me, SPELL_SUMMON_WOLF_DROPPINGS, true);
				}

				me->DespawnAfterAction();
			}
        }

		void UpdateAI(uint32 const diff)
		{
			// Out Of Combat Timer
			if (OilyCoatTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_OILY_COAT);
				OilyCoatTimer = 5000;
			}
			else OilyCoatTimer -= diff;

			if (!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_oil_stained_wolfAI(creature);
    }
};

enum IthrixtheHarvester
{
	SPELL_SUMMON_ITHRIX_SCALE = 45593,
	SPELL_IH_VENOM_SPIT       = 45592,
	SPELL_POISON_STINGER      = 25748,
	SPELL_STINGER_RAGE        = 34392,
};

class npc_ithrix_the_harvester : public CreatureScript
{
public:
    npc_ithrix_the_harvester() : CreatureScript("npc_ithrix_the_harvester") {}

    struct npc_ithrix_the_harvesterAI : public QuantumBasicAI
    {
        npc_ithrix_the_harvesterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 VenomSpitTimer;
		uint32 PoisonStingerTimer;

        void Reset()
        {
			VenomSpitTimer = 1000;
			PoisonStingerTimer = 3000;
		}

		void JustDied(Unit* /*killer*/)
		{
			DoCastAOE(SPELL_SUMMON_ITHRIX_SCALE);
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (VenomSpitTimer <= diff)
			{
				DoCastVictim(SPELL_IH_VENOM_SPIT);
				VenomSpitTimer = urand(4000, 5000);
			}
			else VenomSpitTimer -= diff;

			if (PoisonStingerTimer <= diff)
			{
				DoCastVictim(SPELL_POISON_STINGER);
				PoisonStingerTimer = urand(6000, 7000);
			}
			else PoisonStingerTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
				DoCast(me, SPELL_STINGER_RAGE);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ithrix_the_harvesterAI(creature);
    }
};

enum QuestTheEmissary
{
	QUEST_THE_EMISSARY         = 11626,

	SPELL_L_MORTAL_STRIKE      = 19643,
	SPELL_IMPALE_LEVIROTH      = 47170,
	SPELL_LEVIROTH_SELF_IMPALE = 49882,
};

class npc_leviroth : public CreatureScript
{
public:
    npc_leviroth() : CreatureScript("npc_leviroth") {}

    struct npc_levirothAI : public QuantumBasicAI
    {
        npc_levirothAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MortalStrikeTimer;

        void Reset()
        {
			MortalStrikeTimer = 2000;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
		}

		void MoveInLineOfSight(Unit* who)
		{
			if (Player* player = who->ToPlayer())
			{
				if (player->IsWithinDist(me, 25.0f) && player->GetQuestStatus(QUEST_THE_EMISSARY) == QUEST_STATUS_INCOMPLETE)
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
			}
		}

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_IMPALE_LEVIROTH)
			{
				me->Kill(me);
				caster->ToPlayer()->KilledMonsterCredit(me->GetEntry(), 0);
			}
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_L_MORTAL_STRIKE);
				MortalStrikeTimer = 4000;
			}
			else MortalStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_levirothAI(creature);
    }
};

enum QuestTakeNoChances
{
	SPELL_BURN_GRAIN      = 46574,

	QUEST_TAKE_NO_CHANCES = 11913,
};

class npc_farshire_grain_credit : public CreatureScript
{
public:
    npc_farshire_grain_credit() : CreatureScript("npc_farshire_grain_credit") {}

    struct npc_farshire_grain_creditAI : public QuantumBasicAI
    {
		npc_farshire_grain_creditAI(Creature* creature) : QuantumBasicAI(creature) {}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_BURN_GRAIN)
            {
				if (Player* player = caster->ToPlayer())
				{
					if (player->GetQuestStatus(QUEST_TAKE_NO_CHANCES) == QUEST_STATUS_COMPLETE)
						return;

					caster->ToPlayer()->KilledMonsterCredit(me->GetEntry(), 0);

					me->DespawnAfterAction();
				}
			}
        }

		void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_farshire_grain_creditAI(creature);
    }
};

enum QuestRepurposedTechnology
{
	SPELL_HARVEST_COLLECTOR     = 47166,
	SPELL_CLAW_SLASH            = 54185,

	NPC_RECKLESS_SCAVENGER      = 26658,

	QUEST_REPURPOSED_TECHNOLOGY = 12035,
};

class npc_harvest_collector : public CreatureScript
{
public:
    npc_harvest_collector() : CreatureScript("npc_harvest_collector") {}

    struct npc_harvest_collectorAI : public QuantumBasicAI
    {
        npc_harvest_collectorAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		uint32 ClawSlashTimer;

		SummonList Summons;

        void Reset()
		{
			ClawSlashTimer = 2000;

			Summons.DespawnAll();

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_RECKLESS_SCAVENGER)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 30.0f))
					summon->SetInCombatWith(target);

				Summons.Summon(summon);
			}
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_HARVEST_COLLECTOR)
            {
				if (Player* player = caster->ToPlayer())
				{
					if (player->GetQuestStatus(QUEST_REPURPOSED_TECHNOLOGY) == QUEST_STATUS_COMPLETE)
						return;

					me->SummonCreature(NPC_RECKLESS_SCAVENGER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
					me->DespawnAfterAction();
				}
			}
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ClawSlashTimer <= diff)
			{
				DoCastVictim(SPELL_CLAW_SLASH);
				ClawSlashTimer = urand(4000, 5000);
			}
			else ClawSlashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_harvest_collectorAI(creature);
    }
};

enum ScourgePrisoner
{
    GO_SCOURGE_CAGE = 187867,
};

class npc_scourge_prisoner : public CreatureScript
{
public:
    npc_scourge_prisoner() : CreatureScript("npc_scourge_prisoner") { }

    struct npc_scourge_prisonerAI : public QuantumBasicAI
    {
        npc_scourge_prisonerAI(Creature* creature) : QuantumBasicAI (creature){}

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);

            if (GameObject* cage = me->FindGameobjectWithDistance(GO_SCOURGE_CAGE, 5.0f))
			{
                if (cage->GetGoState() == GO_STATE_ACTIVE)
                    cage->SetGoState(GO_STATE_READY);
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scourge_prisonerAI(creature);
    }
};

enum QuestEmergencyProtocol
{
	SPELL_SCUTTLE_WRECKED_MACHINE = 46171,
	SPELL_SUMMON_EXPLOSIVE_CART   = 46799,
	SPELL_FLAMES                  = 39199,

	QUEST_EMERENCY_PROTOCOL_8_2   = 11796,
};

class npc_east_crash : public CreatureScript
{
public:
    npc_east_crash() : CreatureScript("npc_east_crash") {}

    struct npc_east_crashAI : public QuantumBasicAI
    {
        npc_east_crashAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RemoveFireTimer;

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_SCUTTLE_WRECKED_MACHINE)
            {
				if (Player* player = caster->ToPlayer())
				{
					if (player->GetQuestStatus(QUEST_EMERENCY_PROTOCOL_8_2) == QUEST_STATUS_COMPLETE)
						return;

					player->KilledMonsterCredit(me->GetEntry(), 0);
					player->CastSpell(player, SPELL_SUMMON_EXPLOSIVE_CART, true);

					RemoveFireTimer = 1*MINUTE*IN_MILLISECONDS;
				}
			}
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (RemoveFireTimer <= diff &&!me->IsInCombatActive())
			{
				me->RemoveAurasDueToSpell(SPELL_FLAMES);
				RemoveFireTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else RemoveFireTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_east_crashAI(creature);
    }
};

class npc_south_crash : public CreatureScript
{
public:
    npc_south_crash() : CreatureScript("npc_south_crash") {}

    struct npc_south_crashAI : public QuantumBasicAI
    {
        npc_south_crashAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RemoveFireTimer;

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_SCUTTLE_WRECKED_MACHINE)
            {
				if (Player* player = caster->ToPlayer())
				{
					if (player->GetQuestStatus(QUEST_EMERENCY_PROTOCOL_8_2) == QUEST_STATUS_COMPLETE)
						return;

					player->KilledMonsterCredit(me->GetEntry(), 0);
					player->CastSpell(player, SPELL_SUMMON_EXPLOSIVE_CART, true);

					RemoveFireTimer = 1*MINUTE*IN_MILLISECONDS;
				}
			}
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (RemoveFireTimer <= diff &&!me->IsInCombatActive())
			{
				me->RemoveAurasDueToSpell(SPELL_FLAMES);
				RemoveFireTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else RemoveFireTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_south_crashAI(creature);
    }
};

class npc_northwest_crash : public CreatureScript
{
public:
    npc_northwest_crash() : CreatureScript("npc_northwest_crash") {}

    struct npc_northwest_crashAI : public QuantumBasicAI
    {
        npc_northwest_crashAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RemoveFireTimer;

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_SCUTTLE_WRECKED_MACHINE)
            {
				if (Player* player = caster->ToPlayer())
				{
					if (player->GetQuestStatus(QUEST_EMERENCY_PROTOCOL_8_2) == QUEST_STATUS_COMPLETE)
						return;

					player->KilledMonsterCredit(me->GetEntry(), 0);
					player->CastSpell(player, SPELL_SUMMON_EXPLOSIVE_CART, true);

					RemoveFireTimer = 1*MINUTE*IN_MILLISECONDS;
				}
			}
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (RemoveFireTimer <= diff &&!me->IsInCombatActive())
			{
				me->RemoveAurasDueToSpell(SPELL_FLAMES);
				RemoveFireTimer = 1*MINUTE*IN_MILLISECONDS;
			}
			else RemoveFireTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_northwest_crashAI(creature);
    }
};

void AddSC_borean_tundra()
{
    new npc_sinkhole_kill_credit();
    new npc_khunok_the_behemoth();
    new npc_keristrasza();
    new npc_corastrasza();
    new npc_wyrmrest_skytalon_vehicle();
    new npc_iruk();
    new npc_nerubar_victim();
    new npc_jenny();
    new npc_fezzix_geartwist();
    new npc_nesingwary_trapper();
    new npc_lurgglbr();
    new npc_nexus_drake_hatchling();
    new npc_thassarian();
    new npc_image_lich_king();
    new npc_counselor_talbot();
    new npc_leryssa();
    new npc_general_arlos();
    new npc_beryl_sorcerer();
    new npc_imprisoned_beryl_sorcerer();
    new npc_mootoo_the_younger();
    new npc_bonker_togglevolt();
    new npc_trapped_mammoth_calf();
    new npc_magmoth_crusher();
    new npc_seaforium_depth_charge();
    //new npc_valiance_keep_cannoneer();
    new npc_warmage_coldarra();
    new npc_hidden_cultist();
    new npc_recon_pilot();
    new npc_steam_rager();
	new npc_raelorasz();
	new npc_fallen_caravan_guard();
	new npc_signal_fire();
	new npc_keristrasza_coldarra();
	new npc_kaw_the_mammoth_destroyer();
	new npc_oil_pool();
	new npc_oil_stained_wolf();
	new npc_ithrix_the_harvester();
	new npc_leviroth();
	new npc_farshire_grain_credit();
	new npc_harvest_collector();
	new npc_scourge_prisoner();
	new npc_east_crash();
	new npc_south_crash();
	new npc_northwest_crash();
}