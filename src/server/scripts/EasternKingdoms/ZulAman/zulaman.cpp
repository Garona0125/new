/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

/* ScriptData
SDName: Zulaman
SD%Complete: 90
SDComment: Forest Frog will turn into different NPC's. Workaround to prevent new entry from running this script
SDCategory: Zul'Aman
EndScriptData */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "QuantumGossip.h"
#include "zulaman.h"

float GauntletWaypoints[8][3] =
{
    {227.510f, 1459.576f, 25.920f},// Gauntled -- Point 1 Start
    {227.684f, 1438.195f, 26.271f},// Gauntled -- Point 2 RampStart
    {229.847f, 1380.534f, 45.460f},// Gauntled -- Point 3 RampEnd
    {248.919f, 1370.393f, 49.322f},// Gauntled -- Point 4 secound flor
    {282.367f, 1379.441f, 49.323f},// Gauntled -- Point 5 Stares begin
    {301.168f, 1385.222f, 57.678f},// Gauntled -- Point 6 Stares middle 1
    {313.466f, 1388.549f, 57.401f},// Gauntled -- Point 7 Stares middle 2
    {335.775f, 1395.879f, 74.581f} // Gauntled -- Point 8 Stares end
};

float WarriorSpawnPos[3] = {247.396f, 1464.760f, 26.266f};
float EagleSpawnPos[3] = {301.168f, 1385.222f, 57.678f};

const float DrumPositions[4][3] =
{
    {-195.533f, 1357.579f, 0.4279f},
    {-185.469f, 1292.650f, 1.9637f},
    {-173.463f, 1212.119f, 0.1743f},
    {-127.524f, 1167.949f, 0.7608f}
};

const float ReinforcementPositions[4][3] = 
{
    {-178.348f, 1350.270f, -0.107f},
    {-177.929f, 1274.980f, 1.5715f},
    {-166.024f, 1191.630f, 0.9356f},
    {-112.116f, 1159.570f, 0.0933f}
};

enum ForestFrog
{
	SPELL_REMOVE_AMANI_CURSE = 43732,
	SPELL_PUSH_MOJO          = 43923,
};

class npc_forest_frog : public CreatureScript
{
public:
	npc_forest_frog() : CreatureScript("npc_forest_frog") {}

	struct npc_forest_frogAI : public QuantumBasicAI
	{
		npc_forest_frogAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;

		void Reset(){}

		void EnterToBattle(Unit* /*who*/) {}

		void DoSpawnRandom()
		{
			if (instance)
			{
				uint32 Entry = 0;

				switch (rand()%10)
				{
				    case 0:
						Entry = 24397;
						break;
					case 1: 
						Entry = 24403;
						break;
					case 2:
						Entry = 24404;
						break;
					case 3:
						Entry = 24405;
						break;
					case 4:
						Entry = 24406;
						break;
					case 5:
						Entry = 24407;
						break;
					case 6:
						Entry = 24445;
						break;
					case 7:
						Entry = 24448;
						break;
					case 8:
						Entry = 24453;
						break;
					case 9:
						Entry = 24455;
						break;
				}

				if (!instance->GetData(TYPE_RAND_VENDOR_1))
					if (rand()%10 == 1)
						Entry = 24408; // Gunter
				if (!instance->GetData(TYPE_RAND_VENDOR_2))
					if (rand()%10 == 1)
						Entry = 24409;  // Kyren

				if (Entry)
					me->UpdateEntry(Entry);
				if (Entry == 24408)
					instance->SetData(TYPE_RAND_VENDOR_1, DONE);
				if (Entry == 24409)
					instance->SetData(TYPE_RAND_VENDOR_2, DONE);
			}
		}

		void SpellHit(Unit* caster, const SpellInfo* spell)
		{
			if (spell->Id == SPELL_REMOVE_AMANI_CURSE && caster->GetTypeId() == TYPE_ID_PLAYER && me->GetEntry() == NPC_FOREST_FROG)
			{
				if (rand()%99 == 50)
					DoCast(caster, SPELL_PUSH_MOJO, true);
				else
					DoSpawnRandom();
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_forest_frogAI(creature);
	}
};

#define GOSSIP_HOSTAGE1 "I am glad to help you."

static uint32 HostageEntry[] = {23790, 23999, 24024, 24001};
static uint32 ChestEntry[] = {186648, 187021, 186672, 186667};

class npc_zulaman_hostage : public CreatureScript
{
public:
	npc_zulaman_hostage() : CreatureScript("npc_zulaman_hostage") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_HOSTAGE1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		if (action == GOSSIP_ACTION_INFO_DEF + 1)
			player->CLOSE_GOSSIP_MENU();

		if (!creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
			return true;

		creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

		InstanceScript* instance = creature->GetInstanceScript();
		if (instance)
		{
			instance->SetData(DATA_CHEST_LOOTED, 0);

			switch (creature->GetEntry())
			{
			    case NPC_TANZAR:
					creature->SummonGameObject(GO_TANZARS_TRUNK, -144.372f, 1336.84f, 48.1739f, 6.13396f, 0, 0, 0.0745459f, -0.997218f, 7*DAY);
					creature->DespawnAfterAction(30*IN_MILLISECONDS);
					break;
				case NPC_HARKOR:
					creature->SummonGameObject(GO_HARKORS_SATCHEL, 300.037f, 1459.99f, 81.5305f, 5.04138f, 0, 0, 0.581768f, -0.813355f, 7*DAY);
					creature->DespawnAfterAction(30*IN_MILLISECONDS);
					break;
				case NPC_ASHLI:
					creature->SummonGameObject(GO_ASHLIS_BAG, 377.146f, 1071.92f, 9.78035f, 4.64926f, 0, 0, 0.729071f, -0.684438f, 7*DAY);
					creature->DespawnAfterAction(30*IN_MILLISECONDS);
					break;
				case NPC_KRAZ:
					creature->SummonGameObject(GO_KRAZS_PACKAGE, -91.4605f, 1179.08f, 5.61446f, 5.88662f, 0, 0, 0.196988f, -0.980406f, 7*DAY);
					creature->DespawnAfterAction(30*IN_MILLISECONDS);
					break;
				default:
					break;
			}
		}
		return true;
	}

	struct npc_zulaman_hostageAI : public QuantumBasicAI
	{
		npc_zulaman_hostageAI(Creature* creature) : QuantumBasicAI(creature){}

		void Reset() {}

		void UpdateAI(const uint32 /*diff*/){}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_zulaman_hostageAI(creature);
	}
};

#define GOSSIP_JONES "Open the gates to Zul'Aman"

class npc_zulaman_harrison_jones : public CreatureScript
{
public:
    npc_zulaman_harrison_jones() : CreatureScript("npc_zulaman_harrison_jones") {}

    bool OnGossipHello(Player* player, Creature* creature)
    {
        InstanceScript* instance = creature->GetInstanceScript();

		if (instance->GetData(DATA_TIMER_START) == 0)
		{
			player->ADD_GOSSIP_ITEM(0, GOSSIP_JONES, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
			player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        }
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        if (action == GOSSIP_ACTION_INFO_DEF + 1)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        
            InstanceScript* instance = creature->GetInstanceScript();

			if (creature->GetMap()->IsDungeon())
				((InstanceMap*)creature->GetMap())->PermBindAllPlayers(player);

			instance->SetData(DATA_TIMER_START,IN_PROGRESS);
        }
        return true;
    }
};

struct npc_zulaman_trashAI : public QuantumBasicAI
{
    npc_zulaman_trashAI(Creature* creature) : QuantumBasicAI(creature)
    {
        instance = creature->GetInstanceScript();
    }

    InstanceScript* instance;

    uint32 AreaInCombatTimer;

    bool CanDetectVisibility;

    void Reset()
    {
        AreaInCombatTimer = 5000;

        SetVisibilityDetection(CanDetectVisibility);
    }

    void SetVisibilityDetection(bool canDetect)
    {
        if (canDetect)
            me->CastSpell(me,SPELL_UNIT_DETECTION_ALL,true);
    }

    void SendAttacker(Unit* target) // Exploit Fix
    {
        std::list<Creature*> templist;
        float x, y, z;
        me->GetPosition(x, y, z);
        {
            CellCoord pair(Trinity::ComputeCellCoord(x, y));
            Cell cell(pair);
            cell.SetNoCreate();

            Trinity::AllFriendlyCreaturesInGrid check(me);
            Trinity::CreatureListSearcher<Trinity::AllFriendlyCreaturesInGrid> searcher(me, templist, check);

            TypeContainerVisitor<Trinity::CreatureListSearcher<Trinity::AllFriendlyCreaturesInGrid>, GridTypeMapContainer> cSearcher(searcher);

            cell.Visit(pair, cSearcher, *(me->GetMap()), *me, SIZE_OF_GRIDS);
        }

        if (!templist.size())
            return;

        for(std::list<Creature*>::iterator i = templist.begin(); i != templist.end(); ++i)
        {
            if ((*i) && me->IsWithinDistInMap((*i),10))
            {
                if (!(*i)->IsInCombatActive() && !me->GetVictim())
                    (*i)->AI()->AttackStart(target);
            }
        }
    }

    void CheckBugusing()
    {
		if (instance->GetData(DATA_TIMER_START) != 1)
			instance->SetData(DATA_TIMER_DELETE, 1);
    }

    void EnterToBattle(Unit* who)
    {
        DoAttackerAreaInCombat(who, 100);

        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
            SendAttacker(target);

        CheckBugusing();
    }

    void KilledUnit(Unit* /*victim*/){}

    void JustDied(Unit* /*killer*/){}

    Creature* FindCreatureWithDistance(uint32 entry, float range)
    {
        Creature* creature = NULL;
        Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck checker(*me, entry, true, range);
        Trinity::CreatureLastSearcher<Trinity::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(me, creature, checker);
        me->VisitNearbyObject(range, searcher);
        return creature;
    }

    void DoAggroPuls(const uint32 diff)
    {
        if (AreaInCombatTimer <= diff)
        {
            DoAttackerAreaInCombat(me->GetVictim(), 100);
            AreaInCombatTimer = 5000;
        }
		else AreaInCombatTimer -= diff;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        DoAggroPuls(diff);

        DoMeleeAttackIfReady();
    }
};

#define SPELL_AG_REND                           43246
#define SPELL_AG_STARTLING_ROAR                 43249

class npc_amanishi_guardian : public CreatureScript
{
public:
    npc_amanishi_guardian() : CreatureScript("npc_amanishi_guardian") {}

    struct npc_amanishi_guardianAI : public npc_zulaman_trashAI
    {
        npc_amanishi_guardianAI(Creature* creature) : npc_zulaman_trashAI(creature)
        {
            npc_zulaman_trashAI::CanDetectVisibility = true;
        }

        uint32 RendTimer;
        uint32 StartlingRoarTimer;

        void Reset()
        {
            npc_zulaman_trashAI::Reset();

            RendTimer = urand(10000,15000);
            StartlingRoarTimer = urand(15000,20000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (RendTimer <= diff)
            {
                DoCastVictim(SPELL_AG_REND);
                RendTimer = urand(10000,15000);
            }
			else RendTimer -= diff;

            if (StartlingRoarTimer <= diff)
            {
				DoCastAOE(SPELL_AG_STARTLING_ROAR);
				StartlingRoarTimer = urand(10000,20000);
            }
			else StartlingRoarTimer -= diff;

            npc_zulaman_trashAI::UpdateAI(diff);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amanishi_guardianAI(creature);
    }
};

#define SPELL_AWW_CHAIN_HEAL                    43527
#define SPELL_AWW_FROST_SHOCK                   43524
#define SPELL_AWW_LIGHTNING_BOLT                43526

class npc_amanishi_wind_walker : public CreatureScript
{
public:

    npc_amanishi_wind_walker() : CreatureScript("npc_amanishi_wind_walker") {}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amanishi_wind_walkerAI(creature);
    }

    struct npc_amanishi_wind_walkerAI : public npc_zulaman_trashAI
    {
        npc_amanishi_wind_walkerAI(Creature* creature) : npc_zulaman_trashAI(creature)
        {
            npc_zulaman_trashAI::CanDetectVisibility = true;
        }
        uint32 ChainHealTimer;
        uint32 FrostShockTimer;
        uint32 LightningBoltTimer;

        void Reset()
        {
            npc_zulaman_trashAI::Reset();

            ChainHealTimer = 5000;
            FrostShockTimer = 2000;
            LightningBoltTimer = 500;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (ChainHealTimer <= diff)
            {
                if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, SPELL_AWW_CHAIN_HEAL);
					ChainHealTimer = urand(7000, 8000);
				}
            }
			else ChainHealTimer -= diff;

            if (FrostShockTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_AWW_FROST_SHOCK);
					FrostShockTimer = urand(5000, 6000);
				}
            }
			else FrostShockTimer -= diff;

            if (LightningBoltTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_AWW_LIGHTNING_BOLT);
					LightningBoltTimer = urand(3000, 4000);
				}
            }
			else LightningBoltTimer -= diff;

            npc_zulaman_trashAI::UpdateAI(diff);
        }
    };
};

#define SPELL_AP_CLEAVE                     15496
#define SPELL_AP_MORTAL_STRIKE              43529
#define SPELL_AP_PIERCING_HOWL              43530

class npc_amanishi_protector : public CreatureScript
{
public:

    npc_amanishi_protector() : CreatureScript("npc_amanishi_protector") {}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amanishi_protectorAI(creature);
    }

    struct npc_amanishi_protectorAI : public npc_zulaman_trashAI
    {
        npc_amanishi_protectorAI(Creature* creature) : npc_zulaman_trashAI(creature)
        {
            npc_zulaman_trashAI::CanDetectVisibility = false;
        }

        uint32 CleaveTimer;
        uint32 MortalStrikeTimer;
        uint32 PiercingHowlTimer;

        void Reset()
        {
            npc_zulaman_trashAI::Reset();

            CleaveTimer = 5000;
            MortalStrikeTimer = urand(5000,10000);
            PiercingHowlTimer = urand(15000,20000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_AP_CLEAVE);
                CleaveTimer = 5000;
            }
			else CleaveTimer -= diff;

            if (MortalStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_AP_MORTAL_STRIKE);
                MortalStrikeTimer = urand(5000,10000);
            }
			else MortalStrikeTimer -= diff;

            if (PiercingHowlTimer <= diff)
            {
				DoCastAOE(SPELL_AP_PIERCING_HOWL);
				PiercingHowlTimer = urand(10000,20000);
            }
			else PiercingHowlTimer -= diff;

            npc_zulaman_trashAI::UpdateAI(diff);
        }
    };
};

#define SPELL_AMM_CHAIN_HEAL                    42477
#define SPELL_AMM_LIGHTNING_BOLT                45075
#define SPELL_AMM_HEALING_WARD                  42376
#define SPELL_AMM_PROTECTIVE_WARD               42478

class npc_amanishi_medicine_man : public CreatureScript
{
public:

    npc_amanishi_medicine_man() : CreatureScript("npc_amanishi_medicine_man") {}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amanishi_medicine_manAI(creature);
    }

    struct npc_amanishi_medicine_manAI : public npc_zulaman_trashAI
    {
        npc_amanishi_medicine_manAI(Creature* creature) : npc_zulaman_trashAI(creature)
        {
            npc_zulaman_trashAI::CanDetectVisibility = false;
        }

        uint32 TotemTimer;
        uint32 ChainHealTimer;
        uint32 LightningBoltTimer;

        uint32 totem_count;

        void Reset()
        {
            npc_zulaman_trashAI::Reset();

            TotemTimer = 1000;
            ChainHealTimer = 3000;
            LightningBoltTimer = 5000;

            totem_count = 0;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (TotemTimer <= diff)
            {
				DoCastAOE(totem_count == 0 ? SPELL_AMM_PROTECTIVE_WARD : SPELL_AMM_HEALING_WARD);
				TotemTimer = 15000;
				totem_count = totem_count == 0 ? 1 : 0;
            }
			else TotemTimer -= diff;

            if (ChainHealTimer <= diff)
            {
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, SPELL_AMM_CHAIN_HEAL);
					ChainHealTimer = urand(7000, 8000);
				}
            }
			else ChainHealTimer -= diff;

            if (LightningBoltTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_AMM_LIGHTNING_BOLT);
					LightningBoltTimer = urand(3000, 4000);
				}
            }
			else LightningBoltTimer -= diff;

            npc_zulaman_trashAI::UpdateAI(diff);
        }
    };
};

#define SPELL_AMANI_PROTECTIVE_WARD_PASSIVE_1 42479
#define SPELL_AMANI_PROTECTIVE_WARD_PASSIVE_2 42481

class npc_amani_protective_ward : public CreatureScript
{
public:

    npc_amani_protective_ward() : CreatureScript("npc_amani_protective_ward") {}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amani_protective_wardAI(creature);
    }

    struct npc_amani_protective_wardAI : public NullCreatureAI
    {
        npc_amani_protective_wardAI(Creature* creature) : NullCreatureAI(creature) {Reset();}
        void Reset()
        {
            DoCast(me,SPELL_AMANI_PROTECTIVE_WARD_PASSIVE_2);
        }
        void UpdateAI(const uint32 diff)
        {
            me->RemoveAurasDueToSpell(SPELL_AMANI_PROTECTIVE_WARD_PASSIVE_1);
        }
    };
};

/*######
## Amani'shi Lookout
######*/

#define YELL_START "Akil'zon, the invaders approach!"

class npc_amanishi_lookout : public CreatureScript
{
public:
    npc_amanishi_lookout() : CreatureScript("npc_amanishi_lookout") {}

    struct npc_amanishi_lookoutAI : public QuantumBasicAI
    {
        npc_amanishi_lookoutAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();

            Reset();
        }

        InstanceScript *instance;
        uint32 MoveWaitTimer;
        uint32 NextWaypoint;

        bool GauntletStarted;
    
        void AttackStart(Unit* who)
        {
            if (!who || who == me)
                return;

            if (!me->IsAlive() || !who->IsAlive())
                return;
        
            if (who->GetTypeId()==TYPE_ID_PLAYER)
            {
                if (((Player*)who)->IsGameMaster())
                    return;

                 EnterToBattle(who);
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (me->GetVictim() || !me->IsHostileTo(who) || !who->IsInAccessiblePlaceFor(me))
                return;

            if (!me->CanFly() && me->GetDistanceZ(who) > 20)
                return;

            if (!me->IsWithinDistInMap(who, me->GetAttackDistance(who)) || !me->IsWithinLOSInMap(who))
                return;

            //if (me->canAttack(who))
            EnterToBattle(who);
        }

        void Reset()
        {
			GauntletStarted = instance->GetData(DATA_AKILZON_EVENT) == DONE;
			instance->SetData(DATA_GAUNTLET, NOT_STARTED);

            me->SetSpeed(MOVE_RUN, 2);
            me->SetWalk(false);

            MoveWaitTimer = 0;

            me->CastSpell(me, SPELL_UNIT_DETECTION_ALL, true);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            if (GauntletStarted)
                return;

            me->MonsterYell(YELL_START, LANG_UNIVERSAL, 0);

			instance->SetData(DATA_GAUNTLET, IN_PROGRESS);

            NextWaypoint = 0;
            MoveWaitTimer = 2000;
            GauntletStarted = true;
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (id < 0)
                return;

            if (type != POINT_MOTION_TYPE)
                return;

            if (!GauntletStarted)
                return;

            if (id < 7)
            {
                MoveWaitTimer = 1;
                NextWaypoint++;
            }
            else
            {
                me->SetVisible(false);
                //me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                //me->RemoveCorpse();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!GauntletStarted)
                return;

			if (instance->GetData(DATA_GAUNTLET) == DONE)
			{
				me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
				me->RemoveCorpse();
			}

			if (MoveWaitTimer)
			{
				if (MoveWaitTimer <= diff)
				{
					me->GetMotionMaster()->MovePoint(NextWaypoint,GauntletWaypoints[NextWaypoint][0],GauntletWaypoints[NextWaypoint][1],GauntletWaypoints[NextWaypoint][2]);
					MoveWaitTimer = 0;
				}
				else MoveWaitTimer -= diff;
			}
		}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amanishi_lookoutAI(creature);
    }
};

/*######
## Amani'shi Warrior
######*/

#define SPELL_ANANISHI_WARRIOR_KICK                 43518
#define SPELL_ANANISHI_WARRIOR_CHARGE               43519

class npc_amanishi_warrior : public CreatureScript
{
public:
    npc_amanishi_warrior() : CreatureScript("npc_amanishi_warrior") {}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amanishi_warriorAI (creature);
    }

    struct npc_amanishi_warriorAI : public QuantumBasicAI
    {
        npc_amanishi_warriorAI(Creature* creature) : QuantumBasicAI(creature)
        {
            NextWaypoint = 0;
            Reset();
        }

        uint32 MoveWaitTimer;
        uint32 NextWaypoint;

        uint32 ChargeTimer;
        uint32 KickTimer;

        void Reset()
        {
            me->SetWalk(false);
            MoveWaitTimer = 1;

            ChargeTimer = 1;
            KickTimer = 10000;
        }

        void EnterToBattle(Unit* /*who*/){}

        void MovementInform(uint32 type, uint32 id)
        {
            if (id < 0)
                return;

            if (type != POINT_MOTION_TYPE)
                return;

            if (id < 7)
            {
                MoveWaitTimer = 1;
                NextWaypoint++;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
            {
                if (MoveWaitTimer)
                    if (MoveWaitTimer <= diff)
                    {
                        me->GetMotionMaster()->MovePoint(NextWaypoint,GauntletWaypoints[NextWaypoint][0],GauntletWaypoints[NextWaypoint][1],GauntletWaypoints[NextWaypoint][2]);
                        MoveWaitTimer = 0;
                    }
					else MoveWaitTimer -= diff;
                return;
            }
            else
            {
                if (ChargeTimer <= diff)
                {
                    DoCastVictim(SPELL_ANANISHI_WARRIOR_CHARGE);
                    ChargeTimer = 20000;
                }
				else ChargeTimer -= diff;

                if (KickTimer <= diff)
                {
                    DoCastVictim(SPELL_ANANISHI_WARRIOR_KICK);
                    ChargeTimer = 20000;
                }
				else ChargeTimer -= diff;

                DoMeleeAttackIfReady();
            }
        }
    };
};

/*######
## Amani Eagle
######*/

#define SPELL_ANANI_EAGLE_TALON             43517

class npc_amani_eagle : public CreatureScript
{
public:
    npc_amani_eagle() : CreatureScript("npc_amani_eagle") {}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amani_eagleAI (creature);
    }

    struct npc_amani_eagleAI : public QuantumBasicAI
    {
        npc_amani_eagleAI(Creature* creature) : QuantumBasicAI(creature)
        {
            NextWaypoint = 5;
            Reset();
        }

        uint32 MoveWaitTimer;
        uint32 NextWaypoint;

        uint32 TalonTimer;

        void Reset()
        {
            me->SetWalk(false);
            MoveWaitTimer = 1;
            TalonTimer = 10000;
        }

        void EnterToBattle(Unit* /*who*/){}

        void MovementInform(uint32 type, uint32 id)
        {
            if (id < 0)
                return;

            if (type != POINT_MOTION_TYPE)
                return;

            if (id > 0)
            {
                MoveWaitTimer = 1;
                NextWaypoint--;
            }
        }

        void UpdateAI(const uint32 diff)
        {
        
            if (!UpdateVictim())
            {
                if (MoveWaitTimer)
                    if (MoveWaitTimer <= diff)
                    {
                        me->GetMotionMaster()->MovePoint(NextWaypoint,GauntletWaypoints[NextWaypoint][0],GauntletWaypoints[NextWaypoint][1],GauntletWaypoints[NextWaypoint][2]);
                        MoveWaitTimer = 0;
                    }
					else MoveWaitTimer -= diff;
                return;
            }
            else
            {
                if (TalonTimer <= diff)
                {
                    DoCastVictim(SPELL_ANANI_EAGLE_TALON);
                    TalonTimer = 20000;
                }
				else TalonTimer -= diff;

                DoMeleeAttackIfReady();
            }
        }
    };
};

/*######
## Amani'shi Tempest
######*/

#define SPELL_AMANISHI_TEMPEST_THUNDERCLAP              44033
#define MOB_AMANISHI_WARRIOR                            24225
#define MOB_AMANI_EAGLE                                 24159

class npc_amanishi_tempest : public CreatureScript
{
public:
    npc_amanishi_tempest() : CreatureScript("npc_amanishi_tempest") {}

    struct npc_amanishi_tempestAI : public QuantumBasicAI
    {
        npc_amanishi_tempestAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            Reset();
        }

        InstanceScript *instance;
        uint32 ThunderclapTimer;

        uint32 phase;
        uint32 EagleSpawnTimer;
        uint32 EagleCounter;
        uint32 WarriorSpawnTimer;
        uint32 WarriorCounter;

        void Reset()
        {
            EagleCounter = 5;
            WarriorCounter = 2;
            EagleSpawnTimer = 30000;
            WarriorSpawnTimer = 30000;
            ThunderclapTimer = 10000;
        }

        void EnterToBattle(Unit* /*who*/)
        {
			instance->SetData(DATA_GAUNTLET,DONE);
        }
   
        void UpdateAI(const uint32 diff)
        {
            if (instance->GetData(DATA_GAUNTLET) == IN_PROGRESS)
            {
                if (me->IsDead())
                    instance->SetData(DATA_GAUNTLET, DONE);
            
                if (WarriorSpawnTimer <= diff)
                {
                    if (WarriorCounter > 0)
                    {
                        me->SummonCreature(MOB_AMANISHI_WARRIOR,WarriorSpawnPos[0],WarriorSpawnPos[1],WarriorSpawnPos[2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,60000);
                        WarriorSpawnTimer = 500;
                        WarriorCounter--;
                    }
                    else
                    {
                        WarriorCounter = 2;
                        WarriorSpawnTimer = 59000;
                    }
                }
				else WarriorSpawnTimer -= diff;

                if (EagleSpawnTimer <= diff)
                {
                    if (EagleCounter > 0)
                    {
                        me->SummonCreature(MOB_AMANI_EAGLE,EagleSpawnPos[0],EagleSpawnPos[1],EagleSpawnPos[2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT,60000);
                        EagleSpawnTimer = 500;
                        EagleCounter--;
                    }
                    else
                    {
                        EagleCounter = 5;
                        EagleSpawnTimer = 27500;
                    }
                }
				else EagleSpawnTimer -= diff;
            }

            if (!UpdateVictim())
                return;
      
            if (ThunderclapTimer <= diff)
            {
                DoCastAOE(SPELL_AMANISHI_TEMPEST_THUNDERCLAP);
                ThunderclapTimer = 20000;
            }
			else ThunderclapTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amanishi_tempestAI(creature);
    }
};

/*#################################
## Nalorakk Trashmobs
#################################*/

#define SPELL_ATM_KICK                  36033
#define SPELL_ATM_CYCLONE_STRIKE        42495

class npc_amanishi_tribesman : public CreatureScript
{
public:
    npc_amanishi_tribesman() : CreatureScript("npc_amanishi_tribesman") {}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amanishi_tribesmanAI (creature);
    }

    struct npc_amanishi_tribesmanAI : public npc_zulaman_trashAI
    {
        npc_amanishi_tribesmanAI(Creature* creature) : npc_zulaman_trashAI(creature)
        {
            npc_zulaman_trashAI::CanDetectVisibility = false;
        }

        uint32 KickTimer;
        uint32 StrikeTimer;

        void Reset()
        {
            npc_zulaman_trashAI::Reset();

            KickTimer = urand(5000,10000);
            StrikeTimer = urand(5000,10000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (KickTimer <= diff)
			{
				DoCastVictim(SPELL_ATM_KICK);
				KickTimer = urand(5000,10000);
            }
			else KickTimer -= diff;

            if (StrikeTimer <= diff)
            {
				DoCastVictim(SPELL_ATM_CYCLONE_STRIKE);
				StrikeTimer = urand(5000, 10000);
            }
			else StrikeTimer -= diff;

            npc_zulaman_trashAI::UpdateAI(diff);
        }
    };
};

#define SPELL_AWB_CLEAVE                43273
#define SPELL_AWB_DISMOUNT              43274
#define SPELL_AWB_FRENZY                40743
#define SPELL_AWB_FURIOUS_ROAR          42496

class npc_amanishi_warbringer : public CreatureScript
{
public:
    npc_amanishi_warbringer() : CreatureScript("npc_amanishi_warbringer") {}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amanishi_warbringerAI (creature);
    }

    struct npc_amanishi_warbringerAI : public npc_zulaman_trashAI
    {
        npc_amanishi_warbringerAI(Creature* creature) : npc_zulaman_trashAI(creature)
        {
            npc_zulaman_trashAI::CanDetectVisibility = false;
            mounted = true;
            me->Mount(22467);
        }

        uint32 CleaveTimer;
        uint32 RoarTimer;

        bool mounted;

        void Reset()
        {
            npc_zulaman_trashAI::Reset();

            CleaveTimer = urand(5000,10000);
            RoarTimer = urand(15000,20000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (CleaveTimer <= diff)
            {
				DoCastVictim(SPELL_AWB_CLEAVE);
				CleaveTimer = urand(5000, 10000);
            }
			else CleaveTimer -= diff;

            if (RoarTimer <= diff)
            {
				DoCastVictim(SPELL_AWB_FURIOUS_ROAR);
				RoarTimer = urand(15000, 20000);
            }
			else RoarTimer -= diff;

            if (mounted)
            {
                if ((me->GetHealth() * 100 / me->GetMaxHealth()) <= 25)
                {
                    mounted = false;
                    me->InterruptNonMeleeSpells(false);
                    me->CastSpell(me,SPELL_AWB_DISMOUNT,false);
                    me->RemoveMount();
                }
            }

            if (!me->HasAuraEffect(SPELL_AWB_FRENZY,0))
            {
                if ((me->GetHealth() * 100 / me->GetMaxHealth()) <= 10)
                {
                    me->InterruptNonMeleeSpells(false);
                    me->CastSpell(me,SPELL_AWB_FRENZY,false);
                }
            }

            npc_zulaman_trashAI::UpdateAI(diff);
        }
    };
};

#define SPELL_AAT_AXE_VOLLEY                42359
#define SPELL_AAT_KNOCKDOWN                 35011
#define SPELL_AAT_RAPTOR_STRIKE             31566

class npc_amanishi_axe_thrower : public CreatureScript
{
public:
    npc_amanishi_axe_thrower() : CreatureScript("npc_amanishi_axe_thrower") {}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amanishi_axe_throwerAI (creature);
    }

    struct npc_amanishi_axe_throwerAI : public npc_zulaman_trashAI
    {
        npc_amanishi_axe_throwerAI(Creature* creature) : npc_zulaman_trashAI(creature)
        {
            npc_zulaman_trashAI::CanDetectVisibility = false;
        }

        uint32 VolleyTimer;
        uint32 KnockdownTimer;
        uint32 StrikeTimer;

        void Reset()
        {
            npc_zulaman_trashAI::Reset();

            KnockdownTimer = urand(10000,15000);
            StrikeTimer = urand(5000,10000);
            VolleyTimer = urand(15000,20000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (VolleyTimer <= diff)
            {
				DoCastAOE(SPELL_AAT_AXE_VOLLEY);
				VolleyTimer = urand(45000, 60000);
            }
			else VolleyTimer -= diff;

            if (StrikeTimer <= diff)
            {
				me->CastSpell(me->GetVictim() ,SPELL_AAT_RAPTOR_STRIKE, false);
                StrikeTimer = urand(5000, 10000);
            }
			else StrikeTimer -= diff;

            if (KnockdownTimer <= diff)
            {
				DoCastVictim(SPELL_AAT_KNOCKDOWN);
				KnockdownTimer = urand(15000, 20000);
            }
			else KnockdownTimer -= diff;

            npc_zulaman_trashAI::UpdateAI(diff);
        }
    };
};

#define SPELL_AB_CRUNCH_ARMOR               42747
#define SPELL_AB_ENRAGE                     42745

class npc_amani_bear : public CreatureScript
{
public:
    npc_amani_bear() : CreatureScript("npc_amani_bear") {}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amani_bearAI (creature);
    }

    struct npc_amani_bearAI : public npc_zulaman_trashAI
    {
        npc_amani_bearAI(Creature* creature) : npc_zulaman_trashAI(creature)
        {
            npc_zulaman_trashAI::CanDetectVisibility = false;
        }

        uint32 CrunchTimer;
        uint32 EnrageTimer;
 

        void Reset()
        {
            npc_zulaman_trashAI::Reset();

            CrunchTimer = urand(5000,10000);
            EnrageTimer = urand(20000,30000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (EnrageTimer <= diff)
            {
				DoCastAOE(SPELL_AB_ENRAGE);
				EnrageTimer = urand(20000, 30000);
            }
			else EnrageTimer -= diff;

            if (CrunchTimer <= diff)
            {
				DoCastVictim(SPELL_AB_CRUNCH_ARMOR);
				CrunchTimer = urand(10000, 20000);
            }
			else CrunchTimer -= diff;

            npc_zulaman_trashAI::UpdateAI(diff);
        }
    };
};

/*#################################
## Janalai Trashmobs
#################################*/

#define SPELL_ALERT_DRUMS                   42177
#define SPELL_MULTI_SHOT                    43205
#define SPELL_SHOOT                         16496
#define MOB_AMANISHI_REINFORCEMENTS         23587

class npc_amanishi_scout : public CreatureScript
{
public:
    npc_amanishi_scout() : CreatureScript("npc_amanishi_scout") {}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amanishi_scoutAI (creature);
    }

    struct npc_amanishi_scoutAI : public QuantumBasicAI
    {
        npc_amanishi_scoutAI(Creature* creature) : QuantumBasicAI(creature)
        {
            me->SetWalk(false);
            ReinforcementPhase = 0;
            Reset();
        }

        uint32 shootTimer;
        uint32 multi_shootTimer;
        uint32 waitTimer;
        uint32 summonTimer;
        uint32 summons;
        uint32 ReinforcementPhase; // 0 - never moved, 1 - moving,2 - reinformcment called
        uint64 trigger_guid;
        uint32 point;

        void Reset()
        {
            ReinforcementPhase = 0;
            trigger_guid = 0;

            shootTimer = 1000;
            multi_shootTimer = 15000;
            waitTimer = 0;
            point = 0;
        }

        uint32 GetNextDrumPos()
        {
            uint32 next;
            float distance = 99999999.9f;

            for(int i = 0; i < 4; i++)
            {
                float t_distance = me->GetDistance(DrumPositions[i][0],DrumPositions[i][1],DrumPositions[i][2]);
                if (distance > t_distance)
                {
                    next = (uint32)i;
                    distance = t_distance;
                }
            }
            return next;
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == 1 && ReinforcementPhase == 1)
            {
                waitTimer = 5000;
                summons = 2;
                summonTimer = 500;
                me->GetMotionMaster()->MoveIdle();
                ReinforcementPhase = 2;
            }
        }
       
        void AttackStart(Unit* who)
        {
            if (ReinforcementPhase == 0)
            {
                if (!who || who == me)
                return;

                if (!me->IsAlive() || !who->IsAlive())
                    return;
            
                if (who->GetTypeId()==TYPE_ID_PLAYER )
                {
                    if (((Player*)who)->IsGameMaster())
                        return;

                     CallReinforcements(who);
                }
            }
            else if (ReinforcementPhase == 3)
            {
                QuantumBasicAI::AttackStart(who);
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (ReinforcementPhase == 0)
            {
                if (me->GetVictim() || !me->IsHostileTo(who) || !who->IsInAccessiblePlaceFor(me))
                    return;

                if (!me->CanFly() && me->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE)
                    return;

                if (!me->IsWithinDistInMap(who, me->GetAttackDistance(who)) || !me->IsWithinLOSInMap(who))
                    return;
        
                if (me->CanCreatureAttack(who))
                    CallReinforcements(who);
            }

            else if (ReinforcementPhase == 3)
			{
				QuantumBasicAI::MoveInLineOfSight(who);
			}
        }

        void CallReinforcements(Unit* who)
        {
            trigger_guid = who->GetGUID();
            me->MonsterYell("Invaders! Sound the Alarm!", LANG_UNIVERSAL, 0);
            DoPlaySoundToSet(me,12104);
            ReinforcementPhase = 1;
            point = GetNextDrumPos();
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MovePoint(1, DrumPositions[point][0], DrumPositions[point][1], DrumPositions[point][2]);
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (ReinforcementPhase == 2)
            {
                if (summons > 0)
                {
                    if (summonTimer <= diff)
                    {
                        Unit* target = Unit::GetUnit(*me, trigger_guid);
                        if (target)
                        {
                            Creature* summon = me->SummonCreature(MOB_AMANISHI_REINFORCEMENTS,ReinforcementPositions[point][0],ReinforcementPositions[point][1],ReinforcementPositions[point][2], 0, TEMPSUMMON_DEAD_DESPAWN,60000);
                            summon->SetNoCallAssistance(true);
                            summon->AI()->AttackStart(target);
                        }
                        DoCast(me,SPELL_ALERT_DRUMS);
                        summons--;
                        summonTimer = 500;
                    }
					else summonTimer -= diff;
                }

                if (waitTimer > 0)
                    if (waitTimer <= diff)
                    {
                        me->InterruptNonMeleeSpells(false);
                        ReinforcementPhase = 3;
                        me->GetMotionMaster()->Clear();

                        Unit* target = Unit::GetUnit(*me, trigger_guid);
                        if (target)
                        {
                            AttackStart(target);
                        }

                        waitTimer = 0;
                    }
					else waitTimer -= diff;
            }

            if (!UpdateVictim())
                return;

            if (multi_shootTimer <= diff)
            {
                DoCastVictim(SPELL_MULTI_SHOT);
                multi_shootTimer = 15000;
            }
			else multi_shootTimer -= diff;

            if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == CHASE_MOTION_TYPE)
            {
                if (me->IsWithinDistInMap(me->GetVictim(), 7))
                {
                    DoMeleeAttackIfReady();
                }
                else
                {
                    if (me->IsWithinDistInMap(me->GetVictim(), 20))
                    {
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveIdle();
                    }
                }
            }
            else if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == IDLE_MOTION_TYPE)
            {
                if (!me->IsWithinDistInMap(me->GetVictim(), 5) && me->IsWithinDistInMap(me->GetVictim(), 20))
                {
                    if (shootTimer <= diff)
                    {
                        DoCast(SelectTarget(TARGET_RANDOM, 0), SPELL_SHOOT);
                        shootTimer = 2000;
                    }
					else shootTimer -= diff;
                }
                else
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveChase(me->GetVictim());
                }
            }
        }
    };
};

#define SPELL_AFC_CONFLAGRATION                 42220
#define SPELL_AFC_FIRE_BLAST                    43245
#define SPELL_AFC_FIREBALL_VOLLEY               43240
#define SPELL_AFC_HASTE                         43242

class npc_amanishi_flame_caster : public CreatureScript
{
public:
    npc_amanishi_flame_caster() : CreatureScript("npc_amanishi_flame_caster") {}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amanishi_flame_casterAI (creature);
    }

    struct npc_amanishi_flame_casterAI : public npc_zulaman_trashAI
    {
        npc_amanishi_flame_casterAI(Creature* creature) : npc_zulaman_trashAI(creature)
        {
            npc_zulaman_trashAI::CanDetectVisibility = true;
        }

        uint32 ConflagrationTimer;
        uint32 FireBlastTimer;
        uint32 VolleyTimer;
        uint32 HasteTimer;

        void Reset()
        {
            npc_zulaman_trashAI::Reset();

            ConflagrationTimer = urand(10000,15000);
            FireBlastTimer = urand(10000,15000);
            VolleyTimer = urand(15000,20000);
            HasteTimer = urand(10000,15000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (HasteTimer <= diff)
            {
				DoCastAOE(SPELL_AFC_HASTE);
				HasteTimer = urand(20000, 30000);
            }
			else HasteTimer -= diff;

            if (VolleyTimer <= diff)
            {
				DoCastAOE(SPELL_AFC_FIREBALL_VOLLEY);
				VolleyTimer = urand(15000, 20000);
            }
			else VolleyTimer -= diff;

            if (FireBlastTimer <= diff)
            {
                me->CastSpell(me->GetVictim(), SPELL_AFC_FIRE_BLAST, false);
                FireBlastTimer = urand(7000, 10000);
            }
			else FireBlastTimer -= diff;

            if (ConflagrationTimer <= diff)
            {

                Unit* target = SelectTarget(TARGET_RANDOM, 0, 30, true);
                if (!target) target = me->GetVictim();

				DoCast(target, SPELL_AFC_CONFLAGRATION);
				ConflagrationTimer = urand(10000, 15000);

            }
			else ConflagrationTimer -= diff;

            npc_zulaman_trashAI::UpdateAI(diff);
        }
    };
};

#define SPELL_AT_INCITE_RAGE                43292
#define SPELL_AT_SLEEP                      20989
#define ENTRY_AMANI_DRAGONHAWK              23834

class npc_amanishi_trainer : public CreatureScript
{
public:
    npc_amanishi_trainer() : CreatureScript("npc_amanishi_trainer") {}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amanishi_trainerAI (creature);
    }

    struct npc_amanishi_trainerAI : public npc_zulaman_trashAI
    {
        npc_amanishi_trainerAI(Creature* creature) : npc_zulaman_trashAI(creature)
        {
            npc_zulaman_trashAI::CanDetectVisibility = true;
        }

        uint32 RageTimer;
        uint32 SleepTimer;

        void Reset()
        {
            npc_zulaman_trashAI::Reset();

            RageTimer = urand(10000,15000);
            SleepTimer = urand(10000,15000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (RageTimer <= diff)
            {

                Unit* target = me->FindCreatureWithDistance(ENTRY_AMANI_DRAGONHAWK, 100, true);
                if (!target)
					target = me;

				DoCast(target, SPELL_AT_INCITE_RAGE);
				RageTimer = urand(10000, 15000);

            }
			else RageTimer -= diff;

            if (SleepTimer <= diff)
            {

                Unit* target = SelectTarget(TARGET_RANDOM, 0, 30, true);
                if (!target)
					target = me->GetVictim();

				DoCast(target, SPELL_AT_SLEEP);
				SleepTimer = urand(10000,15000);

            }
			else SleepTimer -= diff;

            npc_zulaman_trashAI::UpdateAI(diff);
        }
    };
};

#define SPELL_ADH_FLAME_BREATH              43294

class npc_amani_dragonhawk : public CreatureScript
{
public:
    npc_amani_dragonhawk() : CreatureScript("npc_amani_dragonhawk") {}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amani_dragonhawkAI (creature);
    }

    struct npc_amani_dragonhawkAI : public npc_zulaman_trashAI
    {
        npc_amani_dragonhawkAI(Creature* creature) : npc_zulaman_trashAI(creature)
        {
            npc_zulaman_trashAI::CanDetectVisibility = false;
        }

        uint32 BreathTimer;

        void Reset()
        {
            npc_zulaman_trashAI::Reset();

            BreathTimer = urand(10000,15000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (BreathTimer <= diff)
            {
				DoCastVictim(SPELL_ADH_FLAME_BREATH);
				BreathTimer = urand(15000, 20000);
            }
			else BreathTimer -= diff;

            npc_zulaman_trashAI::UpdateAI(diff);
        }
    };
};

/*#################################
## Halazzi Trashmobs
#################################*/

#define SPELL_AMH_TRANQUILIZING_POISON                  43364
#define SPELL_AMH_ELECTRIFIED_NET                       43362

class npc_amanishi_handler : public CreatureScript
{
public:
    npc_amanishi_handler() : CreatureScript("npc_amanishi_handler") {}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amanishi_handlerAI (creature);
    }

    struct npc_amanishi_handlerAI : public npc_zulaman_trashAI
    {
        npc_amanishi_handlerAI(Creature* creature) : npc_zulaman_trashAI(creature)
        {
            npc_zulaman_trashAI::CanDetectVisibility = true;
        }

        uint32 PoisonTimer;
        uint32 NetTimer;

        void Reset()
        {
            npc_zulaman_trashAI::Reset();

            PoisonTimer = urand(10000,15000);
            NetTimer = urand(10000,15000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (PoisonTimer <= diff)
            {
				DoCastVictim(SPELL_AMH_TRANQUILIZING_POISON);
				PoisonTimer = urand(10000, 15000);
            }
			else PoisonTimer -= diff;

            if (NetTimer <= diff)
            {

                Unit* target = SelectTarget(TARGET_RANDOM, 0, 30, true);
                if (!target)
					target = me->GetVictim();

				DoCast(target, SPELL_AMH_ELECTRIFIED_NET);
				NetTimer = urand(10000, 15000);

            }
			else NetTimer -= diff;

            npc_zulaman_trashAI::UpdateAI(diff);
        }
    };
};

#define SPELL_ABT_CALL_OF_THE_BEAST                 43359
#define SPELL_ABT_DOMESTICATE                       43361
#define ENTRY_AMANI_LYNX                            24043

class npc_amanishi_beast_tamer : public CreatureScript
{
public:
    npc_amanishi_beast_tamer() : CreatureScript("npc_amanishi_beast_tamer") {}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amanishi_beast_tamerAI (creature);
    }

    struct npc_amanishi_beast_tamerAI : public npc_zulaman_trashAI
    {
        npc_amanishi_beast_tamerAI(Creature* creature) : npc_zulaman_trashAI(creature)
        {
            npc_zulaman_trashAI::CanDetectVisibility = true;
        }

        uint32 DomesticateTimer;
        uint32 CallTimer;

        void KillCharmedPlayer()
        {
            if (!me->GetMap()->IsDungeon())
                return;

            InstanceMap::PlayerList const &playerliste = ((InstanceMap*)me->GetMap())->GetPlayers();
            InstanceMap::PlayerList::const_iterator it;

            Map::PlayerList const &PlayerList = ((InstanceMap*)me->GetMap())->GetPlayers();
            for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            {
                Player* player = i->getSource();
                if (player && player->IsAlive() && player->HasAuraEffect(SPELL_ABT_DOMESTICATE, 0))
                    player->DealDamage(player, player->GetHealth(), 0, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, 0, false);
            }
        }

        void Reset()
        {
            npc_zulaman_trashAI::Reset();

            CallTimer = urand(20000,30000);
            DomesticateTimer = urand(10000,30000);

            KillCharmedPlayer();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (CallTimer <= diff)
            {

                Unit* target = SelectTarget(TARGET_RANDOM, 0, 30, true);
                if (!target)
					target = me->GetVictim();
				
				DoCast(target, SPELL_ABT_CALL_OF_THE_BEAST);
				CallTimer = urand(20000, 30000);

            }
			else CallTimer -= diff;

            if (DomesticateTimer <= diff)
            {

                Unit* target = SelectTarget(TARGET_RANDOM, 0, 30, true);
                if (!target)
					target = me->GetVictim();
				
				DoCast(target, SPELL_ABT_DOMESTICATE);
				DomesticateTimer = urand(15000, 25000);

            }
			else DomesticateTimer -= diff;

            npc_zulaman_trashAI::UpdateAI(diff);
        }
    };
};

#define SPELL_AEL_FERAL_SWIPE               43357
#define SPELL_AEL_FRENZY                    34970
#define SPELL_AEL_POUNCE                    43356

class npc_amani_elder_lynx : public CreatureScript
{
public:
    npc_amani_elder_lynx() : CreatureScript("npc_amani_elder_lynx") {}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amani_elder_lynxAI (creature);
    }

    struct npc_amani_elder_lynxAI : public npc_zulaman_trashAI
    {
        npc_amani_elder_lynxAI(Creature* creature) : npc_zulaman_trashAI(creature)
        {
            npc_zulaman_trashAI::CanDetectVisibility = true;
        }

        uint32 SwipeTimer;
        uint32 FrenzyTimer;
        uint32 PounceTimer;

        void Reset()
        {
            npc_zulaman_trashAI::Reset();

            SwipeTimer = urand(10000,15000);
            FrenzyTimer = urand(20000,30000);
            PounceTimer = urand(10000,15000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (SwipeTimer <= diff)
            {
				DoCastVictim(SPELL_AEL_FERAL_SWIPE);
				SwipeTimer = urand(5000, 15000);
            }
			else SwipeTimer -= diff;

            if (FrenzyTimer <= diff)
            {
				DoCast(me, SPELL_AEL_FRENZY);
				FrenzyTimer = urand(20000, 30000);
            }
			else FrenzyTimer -= diff;

            if (PounceTimer <= diff)
            {
                Unit* target = SelectTarget(TARGET_RANDOM, 0, 10, true);
                if (!target)
					target = me->GetVictim();
				
				DoCast(target, SPELL_AEL_POUNCE);
				PounceTimer = urand(15000, 20000);
            }
			else PounceTimer -= diff;

            npc_zulaman_trashAI::UpdateAI(diff);
        }
    };
};

#define SPELL_AL_STEALTH                        42866

class npc_amani_lynx : public CreatureScript
{
public:
    npc_amani_lynx() : CreatureScript("npc_amani_lynx") {}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amani_lynxAI (creature);
    }

    struct npc_amani_lynxAI : public npc_zulaman_trashAI
    {
        npc_amani_lynxAI(Creature* creature) : npc_zulaman_trashAI(creature)
        {
            npc_zulaman_trashAI::CanDetectVisibility = true;
        }

        uint32 SwipeTimer;

        void Reset()
        {
            npc_zulaman_trashAI::Reset();

            SwipeTimer = urand(10000,15000);
            DoCast(me,SPELL_AL_STEALTH);
        }

        void EnterToBattle(Unit* who)
        {
            me->RemoveAurasDueToSpell(SPELL_AL_STEALTH);

            npc_zulaman_trashAI::EnterToBattle(who);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (SwipeTimer <= diff)
            {
				DoCastVictim(SPELL_AEL_FERAL_SWIPE);
				SwipeTimer = urand(5000, 15000);
            }
			else SwipeTimer -= diff;

            npc_zulaman_trashAI::UpdateAI(diff);
        }
    };
};

#define SPELL_ALC_STEALTH                  42943
#define SPELL_ALC_DASH                     43317
#define SPELL_ALC_GUT_RIP                  43358

class npc_amani_lynx_cub : public CreatureScript
{
public:
    npc_amani_lynx_cub() : CreatureScript("npc_amani_lynx_cub") {}

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amani_lynx_cubAI (creature);
    }

    struct npc_amani_lynx_cubAI : public npc_zulaman_trashAI
    {
        npc_amani_lynx_cubAI(Creature* creature) : npc_zulaman_trashAI(creature)
        {
            npc_zulaman_trashAI::CanDetectVisibility = false;
        }

        uint32 RipTimer;

        void Reset()
        {
            npc_zulaman_trashAI::Reset();

            RipTimer = urand(10000,15000);
            DoCast(me,SPELL_ALC_STEALTH);
        }

        void EnterToBattle(Unit* who)
        {
            me->RemoveAurasDueToSpell(SPELL_ALC_STEALTH);

            DoCast(me, SPELL_ALC_DASH);

            npc_zulaman_trashAI::EnterToBattle(who);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (RipTimer <= diff)
            {
				DoCastVictim(SPELL_ALC_GUT_RIP);
				RipTimer = urand(5000, 10000);
            }
			else RipTimer -= diff;

            npc_zulaman_trashAI::UpdateAI(diff);
        }
    };
};

#define SPELL_ACC_INFECTED_BITE                 43353
#define SPELL_ACC_TAIL_SWIPE                    43352

class npc_amani_crocolist : public CreatureScript
{
public:
    npc_amani_crocolist() : CreatureScript("npc_amani_crocolist") {}

    struct npc_amani_crocolistAI : public npc_zulaman_trashAI
    {
        npc_amani_crocolistAI(Creature* creature) : npc_zulaman_trashAI(creature)
        {
            npc_zulaman_trashAI::CanDetectVisibility = false;
        }

        uint32 TailTimer;
        uint32 BiteTimer;

        void Reset()
        {
            npc_zulaman_trashAI::Reset();

            TailTimer = urand(10000,15000);
            BiteTimer = urand(10000,15000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (BiteTimer <= diff)
            {
				DoCastVictim(SPELL_ACC_INFECTED_BITE);
				BiteTimer = urand(5000, 15000);
            }
			else BiteTimer -= diff;

            if (TailTimer <= diff)
            {
				DoCastAOE(SPELL_ACC_TAIL_SWIPE);
				TailTimer = urand(15000, 20000);
            }
			else TailTimer -= diff;

            npc_zulaman_trashAI::UpdateAI(diff);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_amani_crocolistAI (creature);
    }
};

void AddSC_zulaman()
{
    new npc_forest_frog();
    new npc_zulaman_hostage();
    new npc_zulaman_harrison_jones();
    new npc_amanishi_guardian();
    new npc_amanishi_wind_walker();
    new npc_amanishi_protector();
    new npc_amanishi_medicine_man();
    new npc_amani_protective_ward();
    new npc_amanishi_lookout();
    new npc_amanishi_warrior();
    new npc_amani_eagle();
    new npc_amanishi_tempest();
    new npc_amanishi_tribesman();
    new npc_amanishi_axe_thrower();
    new npc_amanishi_warbringer();
    new npc_amani_bear();
    new npc_amanishi_scout();
    new npc_amanishi_flame_caster();
    new npc_amanishi_trainer();
    new npc_amani_dragonhawk();
    new npc_amanishi_handler();
    new npc_amanishi_beast_tamer();
    new npc_amani_elder_lynx();
    new npc_amani_lynx();
    new npc_amani_lynx_cub();
    new npc_amani_crocolist();
}