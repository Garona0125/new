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
#include "Player.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "Vehicle.h"

enum Hanes
{
	SPELL_COSMETIC_LOW_POLY_FIRE = 56274,
	SPELL_HEALING_POTION         = 17534,

	NPC_APOTHECARY_HANES         = 23784,
	NPC_HANES_FIRE_TRIGGER       = 23968,

    FACTION_ESCORTEE_A           = 774,
    FACTION_ESCORTEE_H           = 775,

    QUEST_TRAIL_OF_FIRE          = 11241,
};

class npc_apothecary_hanes : public CreatureScript
{
public:
    npc_apothecary_hanes() : CreatureScript("npc_apothecary_hanes") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_TRAIL_OF_FIRE)
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
            CAST_AI(npc_escortAI, (creature->AI()))->Start(true, false, player->GetGUID());
        }
        return true;
    }

    struct npc_Apothecary_HanesAI : public npc_escortAI
    {
        npc_Apothecary_HanesAI(Creature* creature) : npc_escortAI(creature){}

        uint32 PotTimer;

        void Reset ()
        {
            SetDespawnAtFar(false);
            PotTimer = 10000; //10 sec cooldown on potion
        }

        void JustDied(Unit* /*killer*/)
        {
            if (Player* player = GetPlayerForEscort())
                player->FailQuest(QUEST_TRAIL_OF_FIRE);
        }

        void UpdateEscortAI(const uint32 diff)
        {
            if (HealthBelowPct(75))
            {
                if (PotTimer <= diff)
                {
					DoCast(me, SPELL_HEALING_POTION, true);
                    PotTimer = 10000;
                }
				else PotTimer -= diff;
            }
            if (GetAttack() && UpdateVictim())
			{
                DoMeleeAttackIfReady();
			}
        }

        void WaypointReached(uint32 i)
        {
            Player* player = GetPlayerForEscort();
            if (!player)
                return;
            switch (i)
            {
                case 1:
                    me->SetReactState(REACT_AGGRESSIVE);
                    SetRun(true);
                    break;
                case 23:
                    if (player)
                        player->GroupEventHappens(QUEST_TRAIL_OF_FIRE, me);
                    me->DespawnAfterAction();
                    break;
                case 5:
                    if (Unit* Trigger = me->FindCreatureWithDistance(NPC_HANES_FIRE_TRIGGER, 10.0f))
                        Trigger->CastSpell(Trigger, SPELL_COSMETIC_LOW_POLY_FIRE, false);
                    SetRun(false);
                    break;
                case 6:
                    if (Unit* Trigger = me->FindCreatureWithDistance(NPC_HANES_FIRE_TRIGGER, 10.0f))
                        Trigger->CastSpell(Trigger, SPELL_COSMETIC_LOW_POLY_FIRE, false);
                    SetRun(true);
                    break;
                case 8:
                    if (Unit* Trigger = me->FindCreatureWithDistance(NPC_HANES_FIRE_TRIGGER, 10.0f))
                        Trigger->CastSpell(Trigger, SPELL_COSMETIC_LOW_POLY_FIRE, false);
                    SetRun(false);
                    break;
                case 9:
                    if (Unit* Trigger = me->FindCreatureWithDistance(NPC_HANES_FIRE_TRIGGER, 10.0f))
                        Trigger->CastSpell(Trigger, SPELL_COSMETIC_LOW_POLY_FIRE, false);
                    break;
                case 10:
                    SetRun(true);
                    break;
                case 13:
                    SetRun(false);
                    break;
                case 14:
                    if (Unit* Trigger = me->FindCreatureWithDistance(NPC_HANES_FIRE_TRIGGER, 10.0f))
                        Trigger->CastSpell(Trigger, SPELL_COSMETIC_LOW_POLY_FIRE, false);
					SetRun(true);
					break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_Apothecary_HanesAI(creature);
    }
};

enum Plaguehound
{
    QUEST_SNIFF_OUT_ENEMY = 11253,
};

class npc_plaguehound_tracker : public CreatureScript
{
public:
    npc_plaguehound_tracker() : CreatureScript("npc_plaguehound_tracker") { }

    struct npc_plaguehound_trackerAI : public npc_escortAI
    {
        npc_plaguehound_trackerAI(Creature* creature) : npc_escortAI(creature) { }

        void Reset()
        {
            uint64 summonerGUID = 0;

            if (me->IsSummon())
                if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                    if (summoner->GetTypeId() == TYPE_ID_PLAYER)
                        summonerGUID = summoner->GetGUID();

            if (!summonerGUID)
                return;

            me->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
            Start(false, false, summonerGUID);
		}

        void WaypointReached(uint32 waypointId)
        {
            if (waypointId != 26)
                return;

            me->DespawnAfterAction();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_plaguehound_trackerAI(creature);
    }
};

#define GOSSIP_RAZAEL_REPORT "High Executor Anselm wants a report on the situation."
#define GOSSIP_LYANA_REPORT  "High Executor Anselm requests your report."

enum Razael
{
    QUEST_REPORTS_FROM_THE_FIELD = 11221,

    NPC_RAZAEL                   = 23998,
    NPC_LYANA                    = 23778,

    GOSSIP_TEXT_ID_RAZAEL_1      = 11562,
    GOSSIP_TEXT_ID_RAZAEL_2      = 11564,
    GOSSIP_TEXT_ID_LYANA_1       = 11586,
    GOSSIP_TEXT_ID_LYANA_2       = 11588,
};

class npc_razael_and_lyana : public CreatureScript
{
public:
    npc_razael_and_lyana() : CreatureScript("npc_razael_and_lyana") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_REPORTS_FROM_THE_FIELD) == QUEST_STATUS_INCOMPLETE)
        {
            switch (creature->GetEntry())
			{
			    case NPC_RAZAEL:
					if (!player->GetReqKillOrCastCurrentCount(QUEST_REPORTS_FROM_THE_FIELD, NPC_RAZAEL))
					{
						player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_RAZAEL_REPORT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
						player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_RAZAEL_1, creature->GetGUID());
						return true;
					}
					break;
				case NPC_LYANA:
					if (!player->GetReqKillOrCastCurrentCount(QUEST_REPORTS_FROM_THE_FIELD, NPC_LYANA))
					{
						player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LYANA_REPORT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
						player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_LYANA_1, creature->GetGUID());
						return true;
					}
					break;
			}
		}

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_RAZAEL_2, creature->GetGUID());
                player->TalkedToCreature(NPC_RAZAEL, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF + 2:
                player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_LYANA_2, creature->GetGUID());
                player->TalkedToCreature(NPC_LYANA, creature->GetGUID());
                break;
        }
        return true;
    }
};

#define GOSSIP_ITEM_MG_I  "Walt sent me to pick up some dark iron ingots."
#define GOSSIP_ITEM_MG_II "Yarp."

enum McGoyver
{
    QUEST_WE_CAN_REBUILD_IT         = 11483,

    SPELL_CREATURE_DARK_IRON_INGOTS = 44512,
    SPELL_TAXI_EXPLORERS_LEAGUE     = 44280,

    GOSSIP_TEXT_ID_MC_GOYVER        = 12193,
};

class npc_mcgoyver : public CreatureScript
{
public:
    npc_mcgoyver() : CreatureScript("npc_mcgoyver") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_WE_CAN_REBUILD_IT) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_MG_I, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
		
		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
		return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_MG_II, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_MC_GOYVER, creature->GetGUID());
                player->CastSpell(player, SPELL_CREATURE_DARK_IRON_INGOTS, true);
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->CastSpell(player, SPELL_TAXI_EXPLORERS_LEAGUE, true);
                player->CLOSE_GOSSIP_MENU();
                break;
        }
        return true;
    }
};

enum PlagueDragonflayer
{
	SPELL_FLAME_SHOCK             = 15616,
	SPELL_RADIATION               = 21862,
	SPELL_CORRUPTION              = 47782,
	SPELL_PLAGUE_SPRAY            = 43381,
	SPELL_PLAGUE_BLIGHT           = 43506,
	SPELL_PLAGUED_BLOOD_EXPLOSION = 42167,

	NPC_SPRAYED_TARGET_CREDIT     = 24281,
	NPC_PLAGUED_TRIBESMAN         = 23564,
	NPC_PLAGUED_RUNE_CASTER       = 24198,
	NPC_PLAGUED_HANDLER           = 24199,
};

class npc_plagued_dragonflayer_rune_caster : public CreatureScript
{
public:
    npc_plagued_dragonflayer_rune_caster() : CreatureScript("npc_plagued_dragonflayer_rune_caster") { }

    struct npc_plagued_dragonflayer_rune_casterAI: public QuantumBasicAI
    {
        npc_plagued_dragonflayer_rune_casterAI(Creature* creature) : QuantumBasicAI(creature) {}

        bool IsPlagued;

		uint32 FlameShockTimer;
		uint32 CorruptionTimer;
		uint32 PlagueCheckTimer;

        void Reset()
        {
            IsPlagued = false;

			FlameShockTimer = 1000;
			CorruptionTimer = 2000;
            PlagueCheckTimer = 5000;

			DoCast(me, SPELL_RADIATION, true);
			DoCast(me, SPELL_PLAGUE_BLIGHT, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/) {}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (!caster || caster->GetTypeId() != TYPE_ID_PLAYER)
                return;

            if (IsPlagued)
                return;

            if (spell->Id == SPELL_PLAGUE_SPRAY)
            {
                IsPlagued = true;
                caster->ToPlayer()->KilledMonsterCredit(NPC_SPRAYED_TARGET_CREDIT, 0);
                Creature *target = me->FindCreatureWithDistance(RAND(NPC_PLAGUED_TRIBESMAN, NPC_PLAGUED_RUNE_CASTER, NPC_PLAGUED_HANDLER), 50, true);
                if (target)
                    me->Attack(target, true);
            }
        }

		void JustDied(Unit* /*killer*/)
		{
			DoCastAOE(SPELL_PLAGUED_BLOOD_EXPLOSION, true);
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (FlameShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FLAME_SHOCK);
					FlameShockTimer = urand(3000, 4000);
				}
			}
			else FlameShockTimer -= diff;

			if (CorruptionTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CORRUPTION);
					CorruptionTimer = urand(5000, 6000);
				}
			}
			else CorruptionTimer -= diff;

            if (PlagueCheckTimer <= diff)
            {
                if (!me->HasAuraEffect(SPELL_PLAGUE_SPRAY, 0))
                    IsPlagued = false;

				PlagueCheckTimer = urand(7000, 8000);
            }
			else PlagueCheckTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_plagued_dragonflayer_rune_casterAI(creature);
    }
};

enum BjornHalgurdsson
{
	SPELL_CRUSH_ARMOR     = 33661,
	SPELL_MORTAL_STRIKE   = 32736,
	SPELL_VRYKUL_INSULT   = 43315,

	NPC_BJORN_KILL_CREDIT = 24275,
};

class npc_bjorn_halgurdsson : public CreatureScript
{
public:
    npc_bjorn_halgurdsson() : CreatureScript("npc_bjorn_halgurdsson") { }

    struct npc_bjorn_halgurdssonAI: public QuantumBasicAI
    {
        npc_bjorn_halgurdssonAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CrushArmorTimer;
        uint32 MortalStrikeTimer;

        void Reset()
        {
            me->SetReactState(REACT_DEFENSIVE);

            MortalStrikeTimer = 2000;
            CrushArmorTimer = 3000;
        }

        void EnterToBattle(Unit* /*who*/){}

        void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_VRYKUL_INSULT)
                me->SetReactState(REACT_AGGRESSIVE);
        }
		
		void DamageTaken(Unit *attacker, uint32 &damage)
        {
            if (damage >= me->GetHealth())
            {
				std::list<HostileReference*>& threatList = me->getThreatManager().getThreatList();
				for (std::list<HostileReference*>::iterator i = threatList.begin(); i != threatList.end(); ++i)
				{
					if (Unit* Temp = Unit::GetUnit(*me,(*i)->getUnitGuid()))
					{
						if (Temp->GetTypeId() == TYPE_ID_PLAYER)
							Temp->ToPlayer()->KilledMonsterCredit(NPC_BJORN_KILL_CREDIT, 0);
					}
				}
			}
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (MortalStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_MORTAL_STRIKE);
                MortalStrikeTimer = urand(4000, 5000);
            }
			else MortalStrikeTimer -= diff;

            if (CrushArmorTimer <= diff)
            {
                DoCastVictim(SPELL_CRUSH_ARMOR);
                CrushArmorTimer = urand(6000, 7000);
            }
			else CrushArmorTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bjorn_halgurdssonAI(creature);
    }
};

enum DecomposingGhoul
{
    QUEST_SHINING_LIGHT     = 11288,
    SPELL_SHINING_LIGHT     = 43202,

    SPELL_SHINING_LIGHT_HIT = 43203,
    SPELL_SELF_FEAR         = 31365,

    FEAR_RESET_DIST         = 50,
    FEAR_DISTANCE           = 10,
};

class mob_decomposing_ghoul : public CreatureScript
{
public:
    mob_decomposing_ghoul() : CreatureScript("mob_decomposing_ghoul") { }

    struct mob_decomposing_ghoulAI: public QuantumBasicAI
    {
        mob_decomposing_ghoulAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CheckTimer;
        bool SelfFeared;

        void Reset()
        {
            CheckTimer = 1000;
            SelfFeared = false;
            me->RemoveAura(SPELL_SELF_FEAR);
            me->CombatStop(true);
            me->SetReactState(REACT_AGGRESSIVE);
        }

        void EnterToBattle(Unit* /*who*/){}

        void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_SHINING_LIGHT_HIT)
            {
                DoCast(me, SPELL_SELF_FEAR);
                me->SetReactState(REACT_PASSIVE);
                SelfFeared = true;
                CheckTimer = 8000;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (CheckTimer <= diff)
            {
                if (SelfFeared && me->GetVictim() && !me->IsWithinDistInMap(me->GetVictim(), me->GetAttackDistance(me->GetVictim()), true))
                    EnterEvadeMode();

                CheckTimer = 1000;
            }
			else CheckTimer -= diff;

            if (me->GetVictim() && ((me->GetVictim()->ToPlayer() && !me->GetVictim()->ToPlayer()->HasAura(SPELL_SHINING_LIGHT)) || me->GetVictim()->ToCreature()))
                DoMeleeAttackIfReady();

            if (!UpdateVictim())
				return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_decomposing_ghoulAI(creature);
    }
};

enum alliance_banner
{
    NPC_WINTERSCORN_DEFENDER = 24015,
    DROP_IT_THEN_ROCK_IT     = 11429,

    WINTERSCRON_DESPAWN_TIME = 60000,
    SPELL_BANNER_DEFENDED    = 44124,
};

const Position WINTERSCORN_DEFENDER_1 = { 1526.36f, -5305.72f, 198.158f, 3.592f };
const Position WINTERSCORN_DEFENDER_2 = { 1508.58f, -5331.28f, 197.336f, 1.488f };
const Position WINTERSCORN_DEFENDER_3 = { 1493.72f, -5316.14f, 195.236f, 0.392f };

class npc_alliance_banner : public CreatureScript
{
public:
    npc_alliance_banner() : CreatureScript("npc_alliance_banner") { }

    struct npc_alliance_bannerAI: public QuantumBasicAI
    {
        npc_alliance_bannerAI(Creature* creature) : QuantumBasicAI(creature)
        {
			SetCombatMovement(false);
            part = 1;
            PartTimer = 5000;
        }

        uint32 PartTimer;
        uint32 part;

        void Reset()
		{
            me->SetReactState(REACT_PASSIVE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (part > 4 || me->IsDead())
                return;

            if (PartTimer <= diff)
            {
                Unit* tmp;
                switch (part)
                {
                    case 1:
                        tmp = me->SummonCreature(NPC_WINTERSCORN_DEFENDER, WINTERSCORN_DEFENDER_1, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, WINTERSCRON_DESPAWN_TIME);
                        if (tmp)
                            tmp->Attack(me, false);
                        break;
                    case 2:
                        tmp = me->SummonCreature(NPC_WINTERSCORN_DEFENDER, WINTERSCORN_DEFENDER_2, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, WINTERSCRON_DESPAWN_TIME);
                        if (tmp)
                            tmp->Attack(me, false);
                        break;
                    case 3:
                        tmp = me->SummonCreature(NPC_WINTERSCORN_DEFENDER, WINTERSCORN_DEFENDER_3, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, WINTERSCRON_DESPAWN_TIME);
                        if (tmp)
                            tmp->Attack(me, false);
                        break;
                    case 4:
                        Unit* owner = me->GetCharmerOrOwnerOrSelf();
                        if (owner && owner->GetTypeId() == TYPE_ID_PLAYER)
                        {
                            me->CastSpell(me, SPELL_BANNER_DEFENDED, true);
                        }
                        break;
                }

                part++;
                PartTimer = 55000;
            }
			else PartTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_alliance_bannerAI(creature);
    }
};

enum RivenWidowCocoon
{
	NPC_QUEST_TARGET         = 24211,
	QUEST_RIVENWOOD_CAPTIVES = 11296,
};

const uint32 rivenWidowCocoon[6] = {23989, 23990, 23991, 23662, 23959, 24026};

class npc_riven_widow_cocoon : public CreatureScript
{
public:
    npc_riven_widow_cocoon() : CreatureScript("npc_riven_widow_cocoon") { }

    struct npc_riven_widow_cocoonAI : public QuantumBasicAI
    {
        npc_riven_widow_cocoonAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset(){}

        void EnterToBattle(Unit* /*who*/){}

        void MoveInLineOfSight(Unit* /*who*/){}

        void JustDied(Unit* killer)
        {
            if (killer->GetTypeId() == TYPE_ID_PLAYER)
			{
                if (killer->ToPlayer()->GetQuestStatus(QUEST_RIVENWOOD_CAPTIVES) == QUEST_STATUS_INCOMPLETE)
				{
                    if (rand()%100 < 25)
                    {
                        me->SummonCreature(NPC_QUEST_TARGET, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                        killer->ToPlayer()->KilledMonsterCredit(NPC_QUEST_TARGET, 0);
                        return;
                    }
				}
			}

            me->SummonCreature(rivenWidowCocoon[rand()%6], 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_riven_widow_cocoonAI (creature);
    }
};

#define GOSSIP_ITEM_0 "I need a normal flight."
#define GOSSIP_ITEM_1 "Bring me after Neu-Agamand."

enum Orehammer
{
    QUEST_PLAGUE_THIS    = 11332,
    ITEM_PRECISION_BOMBS = 33634,
};

class npc_greer_orehammer : public CreatureScript
{
public:
    npc_greer_orehammer() : CreatureScript("npc_greer_orehammer") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->IsTaxi())
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, GOSSIP_ITEM_0, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        if (player->GetQuestStatus(QUEST_PLAGUE_THIS) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch(action)
        {
            case GOSSIP_ACTION_INFO_DEF:
                player->GetSession()->SendTaxiMenu(creature);
                break;
            case GOSSIP_ACTION_INFO_DEF+1:
                ItemPosCountVec dest;
                uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ITEM_PRECISION_BOMBS, 10);
                if (msg == EQUIP_ERR_OK)
                {
                     player->StoreNewItem(dest, ITEM_PRECISION_BOMBS, true);
                }
                player->CLOSE_GOSSIP_MENU();
                player->ActivateTaxiPathTo(745);
                break;
        }
        return true;
    }
};

enum Daegarnn
{
    QUEST_DEFEAT_AT_RING = 11300,
    NPC_FIRJUS           = 24213,
    NPC_JLARBORN         = 24215,
    NPC_YOROS            = 24214,
    NPC_OLUF             = 23931,
    NPC_PRISONER_1       = 24253,
    NPC_PRISONER_2       = 24254,
    NPC_PRISONER_3       = 24255,
};

static float afSummon[] = {838.81f, -4678.06f, -94.182f};
static float afCenter[] = {801.88f, -4721.87f, -96.143f};

class npc_daegarn : public CreatureScript
{
public:
    npc_daegarn() : CreatureScript("npc_daegarn") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_DEFEAT_AT_RING)
        {
            if (npc_daegarnAI* daegarnAI = CAST_AI(npc_daegarn::npc_daegarnAI, creature->AI()))
                daegarnAI->StartEvent(player->GetGUID());
        }
        return true;
    }

    // TODO: make prisoners help (unclear if summoned or using npc's from surrounding cages (summon inside small cages?))
    struct npc_daegarnAI : public QuantumBasicAI
    {
        npc_daegarnAI(Creature* creature) : QuantumBasicAI(creature) { }

        bool EventInProgress;
        uint64 PlayerGUID;

        void Reset()
        {
            EventInProgress = false;
            PlayerGUID = 0;
        }

        void StartEvent(uint64 uiGUID)
        {
            if (EventInProgress)
                return;

            PlayerGUID = uiGUID;

            SummonGladiator(NPC_FIRJUS);
        }

        void JustSummoned(Creature* summon)
        {
            if (Player* player = me->GetPlayer(*me, PlayerGUID))
            {
                if (player->IsAlive())
                {
                    summon->SetWalk(false);
                    summon->GetMotionMaster()->MovePoint(0, afCenter[0], afCenter[1], afCenter[2]);
                    summon->AI()->AttackStart(player);
                    return;
                }
            }
            Reset();
        }

        void SummonGladiator(uint32 entry)
        {
            me->SummonCreature(entry, afSummon[0], afSummon[1], afSummon[2], 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30*IN_MILLISECONDS);
        }

        void SummonedCreatureDies(Creature* summoned, Unit* /*killer*/)
        {
            uint32 entry = 0;

            // will eventually reset the event if something goes wrong
            switch (summoned->GetEntry())
            {
                case NPC_FIRJUS:
					entry = NPC_JLARBORN;
					break;
                case NPC_JLARBORN:
					entry = NPC_YOROS;
					break;
                case NPC_YOROS:
					entry = NPC_OLUF;
					break;
                case NPC_OLUF:
					Reset();
					return;
            }

            SummonGladiator(entry);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_daegarnAI(creature);
    }
};

#define GOSSIP_WORGEN_DISGUISE "Please give me a new Worg Disguise"

enum WorgDisguise_Quests
{
	// Alliance
	QUEST_IN_WORGENS_CLOTHING_A = 11325,
	QUEST_ALPHA_WORG_A          = 11326,

	// Horde
	QUEST_IN_WORGENS_CLOTHING_H = 11323,
	QUEST_ALPHA_WORG_H          = 11324,

	ITEM_WORG_DISGUISE          = 33618,
	SPELL_CREATE_WORG_DISGUISE  = 43379
};

class npc_item_worg_disguise : public CreatureScript
{
    public:
        npc_item_worg_disguise() : CreatureScript("npc_item_worg_disguise") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            if (!player->HasItemCount(ITEM_WORG_DISGUISE, 1, true)
                && (((player->GetQuestStatus(QUEST_IN_WORGENS_CLOTHING_A) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_IN_WORGENS_CLOTHING_A) == QUEST_STATUS_REWARDED) && !player->GetQuestRewardStatus(QUEST_ALPHA_WORG_A))
                || ((player->GetQuestStatus(QUEST_IN_WORGENS_CLOTHING_H) == QUEST_STATUS_COMPLETE || player->GetQuestStatus(QUEST_IN_WORGENS_CLOTHING_H) == QUEST_STATUS_REWARDED) && !player->GetQuestRewardStatus(QUEST_ALPHA_WORG_H))))            
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_WORGEN_DISGUISE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+0);

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
        {
            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF+0:
                    player->CastSpell(player, SPELL_CREATE_WORG_DISGUISE, true);
                    player->CLOSE_GOSSIP_MENU();
                    break;
            }
            return true;
        }
};

enum IronRuneConstructData
{
	QUEST_IRON_RUNE_CONST_BLUFF = 11491,
    SPELL_BLUFF                 = 44562,
};

class npc_iron_rune_construct : public CreatureScript
{
public:
    npc_iron_rune_construct() : CreatureScript("npc_iron_rune_construct") { }

    struct npc_iron_rune_constructAI : public QuantumBasicAI
    {
        npc_iron_rune_constructAI(Creature* creature) : QuantumBasicAI(creature) { }

        bool Ocuppied;

        uint8 seatID;

        void UpdateAI(const uint32 /*diff*/)
        {
            Unit* player = me->GetVehicleKit()->GetPassenger(seatID);

            if (!player)
                me->DisappearAndDie();
        }

        void PassengerBoarded(Unit* passenger, int8 seatId, bool /*apply*/)
        {
            seatID = seatId;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_iron_rune_constructAI(creature);
    }
};

class npc_lebronski : public CreatureScript
{
public:
    npc_lebronski() : CreatureScript("npc_lebronski") { }

    struct npc_lebronskiAI : public QuantumBasicAI
    {
        npc_lebronskiAI(Creature* creature) : QuantumBasicAI(creature) { }

		void Reset(){}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_BLUFF)
                caster->GetVehicleKit()->GetPassenger(0)->ToPlayer()->GroupEventHappens(QUEST_IRON_RUNE_CONST_BLUFF, me);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lebronskiAI(creature);
    }
};

enum AttractedReefBullData
{
    NPC_FEMALE_REEF_COW        = 24797,

	ITEM_TASTY_REEF_FISH       = 34127,

    QUEST_THE_WAY_TO_HIS_HEART = 11472,

    SPELL_ANUNIAQS_NET         = 21014,
    SPELL_TASTY_REEF_FISH      = 44454,
    SPELL_LOVE_COSMETIC        = 52148,
};

class npc_attracted_reef_bull : public CreatureScript
{
public:
	npc_attracted_reef_bull() : CreatureScript("npc_attracted_reef_bull") {}

	struct npc_attracted_reef_bullAI : public QuantumBasicAI
	{
		npc_attracted_reef_bullAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint64 playerGUID;
		uint8 point;

		void Reset()
		{
			playerGUID = 0;
			point = 0;
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (!caster->ToPlayer())
				return;

			if (spell->Id == SPELL_TASTY_REEF_FISH)
			{
				if (playerGUID == 0)
					playerGUID = caster->GetGUID();

				me->GetMotionMaster()->MovePoint(point, caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ());
				++point;
			}

			if (Creature* female = me->FindCreatureWithDistance(NPC_FEMALE_REEF_COW, 5.0f, true))
			{
				if (Player* player = me->GetPlayer(*me, playerGUID))
				{
					DoCast(me, SPELL_LOVE_COSMETIC);
					female->AI()->DoCast(female, SPELL_LOVE_COSMETIC);
					player->GroupEventHappens(QUEST_THE_WAY_TO_HIS_HEART, me);
					me->DespawnAfterAction(5*IN_MILLISECONDS);
				}
			}
		}

		void UpdateAI(const uint32 /*diff*/){}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_attracted_reef_bullAI(creature);
	}
};

class spell_anuniaqs_net : public SpellScriptLoader
{
public:
    spell_anuniaqs_net() : SpellScriptLoader("spell_anuniaqs_net") {}

    class spell_anuniaqs_net_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_anuniaqs_net_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_ANUNIAQS_NET))
                return false;         
            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Unit* caster = GetCaster())
			{
                if (caster->ToPlayer())
                    caster->ToPlayer()->AddItem(ITEM_TASTY_REEF_FISH, urand(1,5));
			}
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_anuniaqs_net_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_anuniaqs_net_SpellScript();
    }
};

enum FeknutBunnyData
{
    NPC_DARKCLAW_BAT   = 23959,

    SPELL_SUMMON_GUANO = 43307,
};

class npc_feknut_bunny : public CreatureScript
{
public:
    npc_feknut_bunny() : CreatureScript("npc_feknut_bunny") {}

    struct npc_feknut_bunnyAI : public QuantumBasicAI
    {
        npc_feknut_bunnyAI (Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CheckTimer;
        bool Checked;

        void Reset()
        {
            CheckTimer = 1000;
            Checked = false;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!Checked)
            {
                if (CheckTimer <= diff)
                {
                    if (Creature* bat = GetClosestCreatureWithEntry(me, NPC_DARKCLAW_BAT, 35.0f))
                    {
                        if (bat->IsAlive())
                        {
                            bat->CastSpell(me, SPELL_SUMMON_GUANO, false);
                            Checked = true;
                        }

                        if (Player* player = me->GetOwner()->ToPlayer())
                        {
                            bat->Attack(player, true);
                            bat->GetMotionMaster()->MoveChase(player);
                        }
                    }
                    me->DespawnAfterAction();
                }
				else CheckTimer -= diff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_feknut_bunnyAI(creature);
    }
};

enum IrulonData
{
    QUEST_GUIDED_BY_HONOR = 11289,

	ITEM_ASHBRINGER       = 13262,

    NPC_TIRION            = 24232,
    NPC_CLERIC            = 24233,

    SAY_TIRION_1          = -1002000,
    EMOTE_TIRION_1        = -1002001,
    SAY_IRULON_1          = -1002002,
    SAY_TIRION_2          = -1002003,
    SAY_TIRION_3          = -1002004,
    SAY_IRULON_2          = -1002005,
    EMOTE_TIRION_2        = -1002006,
    SAY_TIRION_4          = -1002007,
    SAY_TIRION_5          = -1002008,
    EMOTE_TIRION_3        = -1002009,
    YELL_TIRION           = -1002010,

    ACTION_START,
};

enum IluronEvents
{
    EVENT_NONE,
    EVENT_00,
    EVENT_01,
    EVENT_02,
    EVENT_03,
    EVENT_04,
    EVENT_05,
    EVENT_06,
    EVENT_07,
    EVENT_08,
    EVENT_09,
    EVENT_10,
};

class npc_irulon_trueblade : public CreatureScript
{
public:
    npc_irulon_trueblade() : CreatureScript("npc_irulon_trueblade") { }

    bool OnGossipHello(Player* player, Creature* me)
    {
        if (me->IsQuestGiver())
            player->PrepareQuestMenu(me->GetGUID());

        player->GroupEventHappens(QUEST_GUIDED_BY_HONOR, me);
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, me->GetGUID());
        return true;
    }

    bool OnQuestReward(Player* /*player*/, Creature* me, Quest const* quest, uint32 /*item*/)
    {
        if (quest->GetQuestId() == QUEST_GUIDED_BY_HONOR)
            me->AI()->DoAction(ACTION_START);
        return true;
    }

    struct npc_irulon_truebladeAI : public QuantumBasicAI
    {
        npc_irulon_truebladeAI(Creature* creature) : QuantumBasicAI(creature) { }

        EventMap events;
        uint64 Tirion;

        void Reset()
        {
            Tirion = 0;
            events.Reset();
        }

        void DoAction(const int32 actionId)
        {
            switch (actionId)
            {
                case ACTION_START:
                    Tirion = 0;
                    events.ScheduleEvent(EVENT_00, 1500);
                    break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);
            switch (events.ExecuteEvent())
            {
                case EVENT_00:
                    me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
                    if (Creature* pTirion = me->FindCreatureWithDistance(NPC_CLERIC, 7.0f))
                    {
                        Tirion = pTirion->GetGUID();
                        DoSendQuantumText(SAY_TIRION_1, pTirion);
                        pTirion->SetWalk(true);
                        pTirion->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_STAND);
                        pTirion->GetMotionMaster()->MovePoint(0, me->GetPositionX() + 3.0f, me->GetPositionY() + 3.0f, me->GetPositionZ() + 0.5f);
                    }
					events.ScheduleEvent(EVENT_01, 2000);
					break;
                case EVENT_01:
                    if (Creature* pTirion = me->GetCreature(*me, Tirion))
                    {
                        DoSendQuantumText(EMOTE_TIRION_1, pTirion);
                        pTirion->UpdateEntry(NPC_TIRION);
                        pTirion->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, 0);
                    }
                    events.ScheduleEvent(EVENT_02, 1000);
                    break;
                case EVENT_02:
                    DoSendQuantumText(SAY_IRULON_1, me);
                    events.ScheduleEvent(EVENT_03, 2000);
                    break;
                case EVENT_03:
                    if (Creature* pTirion = me->GetCreature(*me, Tirion))
                        DoSendQuantumText(SAY_TIRION_2, pTirion);
                    events.ScheduleEvent(EVENT_04, 3000);
                    break;
                case EVENT_04:
                    if (Creature* pTirion = me->GetCreature(*me, Tirion))
                        DoSendQuantumText(SAY_TIRION_3, pTirion);
                    events.ScheduleEvent(EVENT_05,1000);
                    break;
                case EVENT_05:
                    DoSendQuantumText(SAY_IRULON_2, me);
                    events.ScheduleEvent(EVENT_06, 2500);
                    break;
                case EVENT_06:
                    if (Creature* pTirion = me->GetCreature(*me, Tirion))
                        DoSendQuantumText(EMOTE_TIRION_2, pTirion);
                    events.ScheduleEvent(EVENT_07,1000);
                    break;
                case EVENT_07:
                    if (Creature* pTirion = me->GetCreature(*me, Tirion))
                        DoSendQuantumText(SAY_TIRION_4, pTirion);
                    events.ScheduleEvent(EVENT_08,1500);
                    break;
                case EVENT_08:
                    if (Creature* pTirion = me->GetCreature(*me, Tirion))
                        DoSendQuantumText(SAY_TIRION_5, pTirion);
                    events.ScheduleEvent(EVENT_09,1500);
                    break;
                case EVENT_09:
                    if (Creature* pTirion = me->GetCreature(*me, Tirion))
                    {
                        DoSendQuantumText(EMOTE_TIRION_3, pTirion);
                        pTirion->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, ITEM_ASHBRINGER);
                    }
                    events.ScheduleEvent(EVENT_10,2000);
                    break;
                case EVENT_10:
                    if (Creature* pTirion = me->GetCreature(*me, Tirion))
                    {
                        DoSendQuantumText(YELL_TIRION, pTirion);
                        pTirion->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, 0);
                        pTirion->DespawnAfterAction(5*IN_MILLISECONDS);
                    }
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
					break;
			}
		}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_irulon_truebladeAI(creature);
    }
};

enum YmironData
{
    QUEST_ECHO_YMIRON     = 11343,
    NPC_ANCIENT_VRYKUL_M  = 24314,
    NPC_LICHKING_HF       = 24248,
    NPC_VALKYR            = 24327,

    SAY_VRYKUL_1          = -1002011, // Male
    EMOTE_VRYKUL_1        = -1002012, // Male
    EMOTE_VRYKUL_2        = -1002013, // Female
    SAY_VRYKUL_2          = -1002014, // Male
    SAY_VRYKUL_3          = -1002015, // Female
    SAY_VRYKUL_4          = -1002016, // Male
    SAY_VRYKUL_5          = -1002017, // Female

    EMOTE_LK_1            = -1002018,
    EMOTE_LK_2            = -1002019,
    SAY_LK_1              = -1002020,
    SAY_LK_2              = -1002021, //Val'kyr
    SAY_LK_3              = -1002022,
    SAY_LK_4              = -1002023,
    SAY_LK_5              = -1002024,
    SAY_LK_6              = -1002025,

    SPELL_MAGNETIC_PULL   = 29661,
    SPELL_LK_GRASP        = 43489,
    SPELL_LK_WRATH        = 43488,
};

class npc_ancient_vrykul : public CreatureScript
{
public:
    npc_ancient_vrykul() : CreatureScript("npc_ancient_vrykul") { }

    struct npc_ancient_vrykulAI : public QuantumBasicAI
    {
        npc_ancient_vrykulAI(Creature* creature) : QuantumBasicAI(creature) { }

        EventMap events;

        Creature* pMale;

        uint64 uiPlayer;

        bool active;

        void Reset()
        {
            uiPlayer = 0;
            active = false;
            events.Reset();

            // Set both passive
            me->SetReactState(REACT_PASSIVE);

            if (Creature* male = me->FindCreatureWithDistance(NPC_ANCIENT_VRYKUL_M, 5.0f))
            {
                male->SetReactState(REACT_PASSIVE);
                pMale = male;
            }
        }

        void SetGUID(uint64 guid, int32 id)
        {
            if (active)
                return;
			
			uiPlayer = guid;

            events.ScheduleEvent(1, 2000);

            active = true;
        }

        void UpdateAI(const uint32 diff)
        {
            events.Update(diff);
            switch (events.ExecuteEvent())
            {
                case 1:
                    DoSendQuantumText(SAY_VRYKUL_1, pMale);
                    events.ScheduleEvent(2, 4000);
                    break;
                case 2:
                    DoSendQuantumText(EMOTE_VRYKUL_1, pMale);
                    events.ScheduleEvent(3, 2000);
                    break;
                case 3:
                    DoSendQuantumText(EMOTE_VRYKUL_2, me);
                    events.ScheduleEvent(4, 4000);
                    break;
                case 4:
                    DoSendQuantumText(SAY_VRYKUL_2, pMale);
                    events.ScheduleEvent(5, 6000);
                    break;
                case 5:
                    DoSendQuantumText(SAY_VRYKUL_3, me);
                    events.ScheduleEvent(6,2000);
                    break;
                case 6:
                    DoSendQuantumText(SAY_VRYKUL_4, pMale);
                    events.ScheduleEvent(7, 5000);
                    break;
                case 7:
                    DoSendQuantumText(SAY_VRYKUL_5, me);
                    events.ScheduleEvent(8, 2000);
                    break;
                case 8:
                    if (Player* player = me->GetPlayer(*me, uiPlayer))
                        player->GroupEventHappens(QUEST_ECHO_YMIRON, me);

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
        return new npc_ancient_vrykulAI(creature);
    }
};

class npc_lich_king_hfjord : public CreatureScript
{
public:
    npc_lich_king_hfjord() : CreatureScript("npc_lich_king_hfjord") { }

    struct npc_lich_king_hfjordAI : public QuantumBasicAI
    {
        npc_lich_king_hfjordAI(Creature* creature) : QuantumBasicAI(creature) {}

        EventMap events;
        Player* player;
        bool active;

        void Reset()
        {
            player = 0;
            active = false;
            events.Reset();

            me->SetReactState(REACT_PASSIVE);
            me->SetOrientation(3.29914f);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!active)
            {
                if (Unit* select = me->SelectNearestTarget(20.0f))
                {
                    if (select->GetTypeId() == TYPE_ID_PLAYER)
                    {
                        player = select->ToPlayer();
                        DoSendQuantumText(EMOTE_LK_1, me);
                        me->CastSpell(player, SPELL_MAGNETIC_PULL, true);
                        me->CastSpell(player, SPELL_LK_GRASP, true);
                        active = true;
                        events.ScheduleEvent(1, 2000);
                     }
                }
            }
			
			events.Update(diff);
            switch (events.ExecuteEvent())
            {
                case 1:
                    DoSendQuantumText(EMOTE_LK_2, me);
                    events.ScheduleEvent(2, 1000);
                    break;
                case 2:
                    DoSendQuantumText(SAY_LK_1, me);
                    events.ScheduleEvent(3, 5000);
                    break;
                case 3:
                    if (Creature* pValkyr = me->FindCreatureWithDistance(NPC_VALKYR, 5.0f))
					{
                        DoSendQuantumText(SAY_LK_2, pValkyr);
					}
                    events.ScheduleEvent(4, 5000);
                    break;
                case 4:
                    DoSendQuantumText(SAY_LK_3, me);
                    events.ScheduleEvent(5, 2000);
                    break;
                case 5:
                    DoSendQuantumText(SAY_LK_4, me);
                    events.ScheduleEvent(6, 8000);
                    break;
                case 6:
                    DoSendQuantumText(SAY_LK_5, me);
                    events.ScheduleEvent(7, 9000);
                    break;
                case 7:
                    DoSendQuantumText(SAY_LK_6, me);
                    events.ScheduleEvent(8, 5000);
                    break;
                case 8:
                    if (player)
					{
                        me->CastSpell(player, SPELL_LK_WRATH, true);
					}
                    events.ScheduleEvent(9, 500);
                case 9:
                    if (player)
					{
                        me->Kill(player);
					}
                    events.ScheduleEvent(10, 20000);
                    break;
                case 10:
                    Reset();
                    break;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lich_king_hfjordAI(creature);
    }
};

enum QuestWarningSomeAssemblyRequired
{
    EVENT_CHECK_CHARMED                        = 1,
	SPELL_RANDOM_CIRCUMFERENCE_POINT_POISON    = 42266,
	SPELL_COSMETIC_BLOOD_EXPLOSION_GREEN_LARGE = 43401,
};

class npc_mindless_abomination : public CreatureScript
{
public:
    npc_mindless_abomination() : CreatureScript("npc_mindless_abomination") {}

    struct npc_mindless_abominationAI : public QuantumBasicAI
    {
        npc_mindless_abominationAI(Creature* creature) : QuantumBasicAI(creature){}

		EventMap events;

        void Reset()
        {
            events.ScheduleEvent(EVENT_CHECK_CHARMED, 1000);
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CHECK_CHARMED:
                        if (!me->IsCharmedOwnedByPlayerOrPlayer())
                            me->DespawnAfterAction();
                        else
                            events.ScheduleEvent(EVENT_CHECK_CHARMED, 1000);
                        break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_mindless_abominationAI(creature);
    }
};

class spell_mindless_abomination_explosion_fx_master : public SpellScriptLoader
{
    public:
        spell_mindless_abomination_explosion_fx_master() : SpellScriptLoader("spell_mindless_abomination_explosion_fx_master") { }

        class spell_mindless_abomination_explosion_fx_master_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mindless_abomination_explosion_fx_master_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_RANDOM_CIRCUMFERENCE_POINT_POISON) || !sSpellMgr->GetSpellInfo(SPELL_COSMETIC_BLOOD_EXPLOSION_GREEN_LARGE))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*eff*/)
            {
                Creature* caster = GetCaster()->ToCreature();
                if (!caster)
                    return;

                caster->AI()->DoCast(caster, SPELL_COSMETIC_BLOOD_EXPLOSION_GREEN_LARGE);

                for (uint8 i = 0; i < 10; ++i)
                    caster->AI()->DoCast(caster, SPELL_RANDOM_CIRCUMFERENCE_POINT_POISON);

                caster->DespawnAfterAction(4*IN_MILLISECONDS);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_mindless_abomination_explosion_fx_master_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_mindless_abomination_explosion_fx_master_SpellScript();
        }
};

void AddSC_howling_fjord()
{
    new npc_apothecary_hanes();
    new npc_plaguehound_tracker();
    new npc_razael_and_lyana();
    new npc_mcgoyver();
    new npc_plagued_dragonflayer_rune_caster();
    new npc_daegarn;
    new npc_bjorn_halgurdsson();
    new mob_decomposing_ghoul();
    new npc_alliance_banner();
    new npc_riven_widow_cocoon();
    new npc_greer_orehammer();
	new npc_item_worg_disguise();
	new npc_iron_rune_construct();
	new npc_lebronski();
	new npc_attracted_reef_bull();
	new spell_anuniaqs_net();
	new npc_feknut_bunny();
	new npc_irulon_trueblade();
	new npc_ancient_vrykul();
	new npc_lich_king_hfjord();
	new npc_mindless_abomination();
	new spell_mindless_abomination_explosion_fx_master();
}