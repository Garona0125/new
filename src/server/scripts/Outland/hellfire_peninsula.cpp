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

enum Aeranas
{
	SAY_SUMMON              = -1000138,
    SAY_FREE                = -1000139,

    FACTION_HOSTILE         = 16,
    FACTION_FRIENDLY        = 35,

    SPELL_ENVELOPING_WINDS  = 15535,
    SPELL_SHOCK             = 12553,

    NPC_AERANAS             = 17085,

	QUEST_AVRUUS_ORB        = 9418,
};

class npc_aeranas : public CreatureScript
{
public:
    npc_aeranas() : CreatureScript("npc_aeranas") { }

    struct npc_aeranasAI : public QuantumBasicAI
    {
        npc_aeranasAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 FactionTimer;
        uint32 EnvelopingWindsTimer;
        uint32 ShockTimer;

        void Reset()
        {
            FactionTimer = 8000;
            EnvelopingWindsTimer = 9000;
            ShockTimer = 5000;

            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUEST_GIVER);
            me->SetCurrentFaction(FACTION_FRIENDLY);

            DoSendQuantumText(SAY_SUMMON, me);
        }

        void UpdateAI(const uint32 diff)
        {
            if (FactionTimer)
            {
                if (FactionTimer <= diff)
                {
                    me->SetCurrentFaction(FACTION_HOSTILE);
                    FactionTimer = 0;
                }
				else FactionTimer -= diff;
            }

            if (!UpdateVictim())
                return;

            if (HealthBelowPct(30))
            {
                me->SetCurrentFaction(FACTION_FRIENDLY);
                me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUEST_GIVER);
                me->RemoveAllAuras();
                me->DeleteThreatList();
                me->CombatStop(true);
                DoSendQuantumText(SAY_FREE, me);
                return;
            }

            if (ShockTimer <= diff)
            {
                DoCastVictim(SPELL_SHOCK);
                ShockTimer = 10000;
            }
			else ShockTimer -= diff;

            if (EnvelopingWindsTimer <= diff)
            {
                DoCastVictim(SPELL_ENVELOPING_WINDS);
                EnvelopingWindsTimer = 25000;
            }
			else EnvelopingWindsTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_aeranasAI(creature);
    }
};

enum AncestralWolf
{
    EMOTE_WOLF_LIFT_HEAD       = -1000496,
    EMOTE_WOLF_HOWL            = -1000497,
    SAY_WOLF_WELCOME           = -1000498,

    SPELL_ANCESTRAL_WOLF_BUFF  = 29938,

    NPC_RYGA                   = 17123,
};

class npc_ancestral_wolf : public CreatureScript
{
public:
    npc_ancestral_wolf() : CreatureScript("npc_ancestral_wolf") { }

    struct npc_ancestral_wolfAI : public npc_escortAI
    {
        npc_ancestral_wolfAI(Creature* creature) : npc_escortAI(creature)
        {
            if (creature->GetOwner() && creature->GetOwner()->GetTypeId() == TYPE_ID_PLAYER)
                Start(false, false, creature->GetOwner()->GetGUID());
            else
                sLog->OutErrorConsole("npc_ancestral_wolf can not obtain owner or owner is not a player.");

            creature->SetSpeed(MOVE_WALK, 1.5f);
            Reset();
        }

        void Reset(){}

        void EnterEvadeMode()
        {
            npc_escortAI::EnterEvadeMode();
            DoCast(me, SPELL_ANCESTRAL_WOLF_BUFF, true);
        }

        void WaypointReached(uint32 waypointId)
        {
            switch (waypointId)
            {
                case 0:
                    DoSendQuantumText(EMOTE_WOLF_LIFT_HEAD, me);
                    break;
                case 2:
					DoSendQuantumText(EMOTE_WOLF_HOWL, me);
                    DoCast(me, SPELL_ANCESTRAL_WOLF_BUFF, true);
                    break;
                case 48:
                    if (Creature* ryga = me->FindCreatureWithDistance(NPC_RYGA, 70.0f))
                    {
                        if (ryga->IsAlive() && !ryga->IsInCombatActive())
                        {
                            ryga->SetWalk(true);
                            ryga->SetSpeed(MOVE_WALK, 1.5f);
                            ryga->GetMotionMaster()->MovePoint(0, 517.340698f, 3885.03975f, 190.455978f);
                            Reset();
                        }
                    }
                    break;
                case 50:
                    if (Creature* ryga = me->FindCreatureWithDistance(NPC_RYGA, 70.0f))
                    {
                        if (ryga->IsAlive() && !ryga->IsInCombatActive())
                        {
                            ryga->SetFacingTo(0.776773f);
                            ryga->SetStandState(UNIT_STAND_STATE_KNEEL);
							DoSendQuantumText(SAY_WOLF_WELCOME, ryga);
                            Reset();
                        }
                    }
                    break;
                case 51:
                    if (Creature* ryga = me->FindCreatureWithDistance(NPC_RYGA, 70.0f))
                    {
                        if (ryga->IsAlive() && !ryga->IsInCombatActive())
                        {
                            float fRetX, fRetY, fRetZ, fRetO;
                            ryga->GetRespawnPosition(fRetX, fRetY, fRetZ, &fRetO);
                            ryga->SetHomePosition(fRetX, fRetY, fRetZ, fRetO);
                            ryga->SetStandState(UNIT_STAND_STATE_STAND);
                            ryga->GetMotionMaster()->MoveTargetedHome();
                            Reset();
                        }
                    }
                    break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ancestral_wolfAI(creature);
    }
};

class go_haaleshi_altar : public GameObjectScript
{
public:
    go_haaleshi_altar() : GameObjectScript("go_haaleshi_altar") { }

	bool OnGossipHello(Player* player, GameObject* go)
    {
		if (player->HasQuest(QUEST_AVRUUS_ORB) && !go->FindCreatureWithDistance(NPC_AERANAS, 25.0f))
			go->SummonCreature(NPC_AERANAS, -1321.79f, 4043.80f, 116.24f, 1.25f, TEMPSUMMON_TIMED_DESPAWN, 180000);
        return false;
    }
};

#define GOSSIP_NALADU_ITEM1 "Why don't you escape?"

enum Naladu
{
    GOSSIP_TEXTID_NALADU1 = 9788,
};

class npc_naladu : public CreatureScript
{
public:
    npc_naladu() : CreatureScript("npc_naladu") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_NALADU_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF+1)

			player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_NALADU1, creature->GetGUID());

		return true;
    }
};

#define GOSSIP_TEXT_REDEEM_MARKS     "I have marks to redeem!"
#define GOSSIP_TRACY_PROUDWELL_ITEM1 "I heard that your dog Fei Fei took Klatu's prayer beads..."
#define GOSSIP_TRACY_PROUDWELL_ITEM2 "<back>"

enum Tracy
{
    GOSSIP_TEXTID_TRACY_PROUDWELL1 = 10689,
    QUEST_DIGGING_FOR_PRAYER_BEADS = 10916,
};

class npc_tracy_proudwell : public CreatureScript
{
public:
    npc_tracy_proudwell() : CreatureScript("npc_tracy_proudwell") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->IsVendor())
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_REDEEM_MARKS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        if (player->GetQuestStatus(QUEST_DIGGING_FOR_PRAYER_BEADS) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TRACY_PROUDWELL_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TRACY_PROUDWELL_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_TRACY_PROUDWELL1, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_TRADE:
                player->GetSession()->SendListInventory(creature->GetGUID());
                break;
        }
        return true;
    }
};

#define GOSSIP_TROLLBANE_ITEM1 "Tell me of the Sons of Lothar."
#define GOSSIP_TROLLBANE_ITEM2 "<more>"
#define GOSSIP_TROLLBANE_ITEM3 "Tell me of your homeland."

enum Trollbane
{
    GOSSIP_TEXTID_TROLLBANE1 = 9932,
    GOSSIP_TEXTID_TROLLBANE2 = 9933,
    GOSSIP_TEXTID_TROLLBANE3 = 8772,
};

class npc_trollbane : public CreatureScript
{
public:
    npc_trollbane() : CreatureScript("npc_trollbane") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TROLLBANE_ITEM1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TROLLBANE_ITEM3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TROLLBANE_ITEM2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_TROLLBANE1, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_TROLLBANE2, creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_TROLLBANE3, creature->GetGUID());
                break;
        }
        return true;
    }
};

enum WoundedBloodElf
{
    SAY_ELF_START               = -1000117,
    SAY_ELF_SUMMON1             = -1000118,
    SAY_ELF_RESTING             = -1000119,
    SAY_ELF_SUMMON2             = -1000120,
    SAY_ELF_COMPLETE            = -1000121,
    SAY_ELF_AGGRO               = -1000122,

    QUEST_ROAD_TO_FALCON_WATCH  = 9375,
};

class npc_wounded_blood_elf : public CreatureScript
{
public:
    npc_wounded_blood_elf() : CreatureScript("npc_wounded_blood_elf") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_ROAD_TO_FALCON_WATCH)
        {
            if (npc_escortAI* EscortAI = CAST_AI(npc_wounded_blood_elf::npc_wounded_blood_elfAI, creature->AI()))
				EscortAI->Start(true, false, player->GetGUID());

            creature->SetCurrentFaction(775);
        }
        return true;
    }

    struct npc_wounded_blood_elfAI : public npc_escortAI
    {
        npc_wounded_blood_elfAI(Creature* creature) : npc_escortAI(creature) {}

        void WaypointReached(uint32 i)
        {
            Player* player = GetPlayerForEscort();

			if (!player)
				return;

            switch (i)
            {
            case 0:
                DoSendQuantumText(SAY_ELF_START, me, player);
                break;
            case 9:
                DoSendQuantumText(SAY_ELF_SUMMON1, me, player);
                // Spawn two Haal'eshi Talonguard
                DoSpawnCreature(16967, -15, -15, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                DoSpawnCreature(16967, -17, -17, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                break;
            case 13:
                DoSendQuantumText(SAY_ELF_RESTING, me, player);
                break;
            case 14:
                DoSendQuantumText(SAY_ELF_SUMMON2, me, player);
                // Spawn two Haal'eshi Windwalker
                DoSpawnCreature(16966, -15, -15, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                DoSpawnCreature(16966, -17, -17, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                break;
            case 27:
                DoSendQuantumText(SAY_ELF_COMPLETE, me, player);
                // Award quest credit
                player->GroupEventHappens(QUEST_ROAD_TO_FALCON_WATCH, me);
                break;
            }
        }

        void Reset() { }

        void EnterToBattle(Unit* /*who*/)
        {
            if (HasEscortState(STATE_ESCORT_ESCORTING))
                DoSendQuantumText(SAY_ELF_AGGRO, me);
        }

        void JustSummoned(Creature* summoned)
        {
             // Change faction so mobs attack
            summoned->SetCurrentFaction(775);
            summoned->AI()->AttackStart(me);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wounded_blood_elfAI(creature);
    }
};

class npc_forward_commander_toarch : public CreatureScript
{
public:
    npc_forward_commander_toarch() : CreatureScript("npc_forward_commander_toarch") { }

    struct npc_forward_commander_toarchAI : public QuantumBasicAI
    {
        npc_forward_commander_toarchAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset(){}
        void MoveInLineOfSight(Unit *who)
        {
            if (who && me->GetDistance(who) <= 10)
            {
                if (who->GetTypeId() == TYPE_ID_PLAYER)
                {
					if (CAST_PLR(who)->getQuestStatusMap()[10162].CreatureOrGOCount[0] == 20 &&
						CAST_PLR(who)->getQuestStatusMap()[10162].CreatureOrGOCount[1] == 5 &&
						CAST_PLR(who)->getQuestStatusMap()[10162].CreatureOrGOCount[2] == 5 && CAST_PLR(who)->GetQuestStatus(10162) == QUEST_STATUS_INCOMPLETE)
					{
						CAST_PLR(who)->AreaExploredOrEventHappens(10162);
					}
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature *_Creature) const
    {
        return new npc_forward_commander_toarchAI (_Creature);
    }
};

/*######
## npc_fel_guard_hound
######*/

enum FelGuard
{
    SPELL_SUMMON_POO     = 37688,
    NPC_DERANGED_HELBOAR = 16863,
};

class npc_fel_guard_hound : public CreatureScript
{
public:
    npc_fel_guard_hound() : CreatureScript("npc_fel_guard_hound") { }

    struct npc_fel_guard_houndAI : public QuantumBasicAI
    {
        npc_fel_guard_houndAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CheckTimer;
        uint64 HelboarGUID;

        void Reset()
        {
            CheckTimer = 5000; //check for creature every 5 sec
            HelboarGUID = 0;
        }

        void MovementInform(uint32 uiType, uint32 uiId)
        {
            if (uiType != POINT_MOTION_TYPE || uiId != 1)
                return;

            if (Creature* pHelboar = me->GetCreature(*me, HelboarGUID))
            {
                pHelboar->RemoveCorpse();
                DoCast(SPELL_SUMMON_POO);

                if (Player* owner = me->GetCharmerOrOwnerPlayerOrPlayerItself())
				{
                    me->GetMotionMaster()->MoveFollow(owner, 0.0f, 0.0f);
				}
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (CheckTimer <= diff)
            {
                if (Creature* pHelboar = me->FindCreatureWithDistance(NPC_DERANGED_HELBOAR, 10.0f, false))
                {
                    if (pHelboar->GetGUID() != HelboarGUID && me->GetMotionMaster()->GetCurrentMovementGeneratorType() != POINT_MOTION_TYPE && !me->FindCurrentSpellBySpellId(SPELL_SUMMON_POO))
                    {
                        HelboarGUID = pHelboar->GetGUID();
                        me->GetMotionMaster()->MovePoint(1, pHelboar->GetPositionX(), pHelboar->GetPositionY(), pHelboar->GetPositionZ());
                    }
                }
                CheckTimer = 5000;
            }
			else CheckTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fel_guard_houndAI(creature);
    }
};

/*######
## npc_shattered_hand_berserker
######*/

enum ShatteredHandBerserker
{
    SPELL_ENRAGE              = 8599,
    SPELL_CHARGE              = 35570,
    SPELL_SUMMON_FEL_SPIRIT   = 39206,
    AURA_ANCHORITE_CONTRITION = 39184,
};

class npc_shattered_hand_berserker : public CreatureScript
{
public:
    npc_shattered_hand_berserker() : CreatureScript("npc_shattered_hand_berserker") { }

    struct npc_shattered_hand_berserkerAI : public QuantumBasicAI
    {
        npc_shattered_hand_berserkerAI(Creature* creature) : QuantumBasicAI(creature) {}

        bool Enraged;

        void Reset()
        {
			Enraged = false;
        }

        void EnterToBattle(Unit* who)
        {
            DoCast(who, SPELL_CHARGE);
        }

        void DamageTaken(Unit* doneby, uint32 &damage)
        {
            if (damage >= me->GetHealth())
			{
                if (me->HasAura(AURA_ANCHORITE_CONTRITION))
				{
                    if (Player* killer = doneby->GetCharmerOrOwnerPlayerOrPlayerItself())
                        killer->CastSpell(killer, SPELL_SUMMON_FEL_SPIRIT, true);
				}
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (!Enraged && HealthBelowPct(30))
            {
                DoCast(me, SPELL_ENRAGE);
                Enraged = true;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shattered_hand_berserkerAI(creature);
    }
};

/*######
## npc_felblood_initiate
######*/

enum FelbloodInitiate
{
	SPELL_FEL_SIPHON_DUMMY = 44936,

	NPC_FELBLOOD_INITIATE  = 24918,
	NPC_EMACIATED_FELBLOOD = 24955,
};

class npc_felblood_initiate : public CreatureScript
{
public:
    npc_felblood_initiate() : CreatureScript("npc_felblood_initiate") { }

    struct npc_felblood_initiateAI : public QuantumBasicAI
    {
        npc_felblood_initiateAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset() {}

        void EnterToBattle(Unit* /*who*/){}

        void SpellHit(Unit* /*caster*/, SpellInfo* const spell)
        {
            switch (spell->Id)
			{
			    case SPELL_FEL_SIPHON_DUMMY:
					me->UpdateEntry(NPC_EMACIATED_FELBLOOD);
					me->GetMotionMaster()->Clear(true);
					me->GetMotionMaster()->MoveChase(me->GetVictim());
					break;
			}
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
        return new npc_felblood_initiateAI(creature);
    }
};

enum LivingFlare
{
	SPELL_LIVING_FLARE_COSMETIC   = 44880,
	SPELL_LIVING_FLARE_MASTER     = 44877,
	SPELL_UNSTABLE_FLAME_COSMETIC = 46196,
	SPELL_FLAME_QUEST_CREDIT      = 44947,
	SPELL_LIVING_FLARE_DETONATION = 44948,
	SPELL_FLARE_FLAME_UP          = 44944,
	QUEST_BLAST_THE_GATEWAY       = 11516,
};

float PortalPoint[3] = {835,2521,292.5};

class npc_living_flare : public CreatureScript
{
public:
    npc_living_flare() : CreatureScript("npc_living_flare") { }

    struct npc_living_flareAI : public QuantumBasicAI
    {
        npc_living_flareAI(Creature* creature) : QuantumBasicAI(creature)
        {
			isUnstable = false;
            explosion = false;
            GrowState = 0;
        }

        uint32 GrowState;
        bool isUnstable;
        bool explosion;

        void Reset()
        {
            me->CastSpell(me, isUnstable ? SPELL_UNSTABLE_FLAME_COSMETIC : SPELL_LIVING_FLARE_COSMETIC, true);
        }

        void EnterToBattle(Unit* /*who*/){}

        void MovementInform(uint32 type, uint32 id)
        {
             if (type != POINT_MOTION_TYPE)
				 return;

             if (id == 1)
             {
                 Unit* owner = me->GetOwner();

                 if (owner && owner->GetTypeId() == TYPE_ID_PLAYER)
                 {
                     Player* pOwner = (Player*)owner;

                     if (pOwner->GetQuestStatus(QUEST_BLAST_THE_GATEWAY) == QUEST_STATUS_INCOMPLETE)
					 {
                         pOwner->CompleteQuest(QUEST_BLAST_THE_GATEWAY);
					 }
                 }
                 //me->CastSpell(owner, SPELL_FLAME_QUEST_CREDIT, true);
                 me->CastSpell(me, SPELL_LIVING_FLARE_DETONATION, false);
                 me->DespawnAfterAction();
             }
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (isUnstable)
                return;

            if (spell->Id == SPELL_LIVING_FLARE_MASTER)
            {
                GrowState++;
                me->CastSpell(me,SPELL_FLARE_FLAME_UP,true);
            }

            if (GrowState >= 8)
            {
                me->UpdateEntry(24958);
                me->SetUInt32Value(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE);
                isUnstable = true;
                me->SetHealth(me->GetMaxHealth());
                me->CastSpell(me,SPELL_UNSTABLE_FLAME_COSMETIC,true);
            }
		}

        void UpdateAI(const uint32 diff)
        {
            if (!explosion)
            {
                if (!me->HasUnitState(UNIT_STATE_FOLLOW))
                {
                    if (me->GetOwner())
                    {
                        me->GetMotionMaster()->MoveFollow(me->GetOwner(),PET_FOLLOW_DIST,PET_FOLLOW_ANGLE);
                    }
                }

                if (!isUnstable)
                    return;

                float distance = me->GetDistance2d(PortalPoint[0],PortalPoint[1]);

                if (distance < 25)
                {
                    me->GetMotionMaster()->MovePoint(1,PortalPoint[0],PortalPoint[1],PortalPoint[2]);
                    explosion = true;
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_living_flareAI(creature);
    }
};

class npc_incandescent_fel_spark : public CreatureScript
{
public:
    npc_incandescent_fel_spark() : CreatureScript("npc_incandescent_fel_spark") { }

    struct incandescent_fel_sparkAI : public QuantumBasicAI
    {
        incandescent_fel_sparkAI(Creature* creature) : QuantumBasicAI(creature){}

        uint32 WaitTimer;
        uint64 killer;

        void Reset()
        {
            killer = 0;
            WaitTimer = 0;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void JustDied(Unit* killer)
        {
            if (killer->GetTypeId() == TYPE_ID_PLAYER)
            {
                 Player* pKiller = (Player*)killer;

                 if (pKiller->GetQuestStatus(QUEST_BLAST_THE_GATEWAY) == QUEST_STATUS_INCOMPLETE)
                     pKiller->CastSpell(killer, SPELL_LIVING_FLARE_MASTER, true, 0, 0, me->GetGUID());
            }
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
        return new incandescent_fel_sparkAI(creature);
    }
};

enum EarthbinderGalandriaNightbreeze
{
	SPELL_ENTANGLING_ROOTS = 33844,
	SPELL_WRATH            = 21807,
	SPELL_MOONFIRE         = 31401,
	QUEST_THE_EARTHBIDER   = 10349,
	SAY_QUEST_START        = -1581200,
	SAY_QUEST_END          = -1581201,
};

class npc_earthbinder_galandria_nightbreeze : public CreatureScript
{
public:
    npc_earthbinder_galandria_nightbreeze() : CreatureScript("npc_earthbinder_galandria_nightbreeze") {}

	bool OnQuestAccept(Player* /*player*/, Creature* creature, const Quest* quest)
    {
        if (quest->GetQuestId() == QUEST_THE_EARTHBIDER)
		{
			DoSendQuantumText(SAY_QUEST_START, creature);
			creature->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
		}
		return true;
	}

	bool OnQuestReward(Player* player, Creature* creature, Quest const* quest, uint32 /*item*/)
	{
		if (quest->GetQuestId() == QUEST_THE_EARTHBIDER)
			DoSendQuantumText(SAY_QUEST_END, creature);

		return true;
	}

    struct npc_earthbinder_galandria_nightbreezeAI : public QuantumBasicAI
    {
        npc_earthbinder_galandria_nightbreezeAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 EntanglingRootsTimer;
		uint32 WrathTimer;
		uint32 MoonfireTimer;

        void Reset()
		{
			MoonfireTimer = 3000;
			WrathTimer = 4000;
			EntanglingRootsTimer = 5000;
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_MOONFIRE);
		}

		void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MoonfireTimer <= diff)
			{
				DoCastVictim(SPELL_MOONFIRE);
				MoonfireTimer = urand(3000, 4000);
			}
			else MoonfireTimer -= diff;

			if (WrathTimer <= diff)
			{
				DoCastVictim(SPELL_WRATH);
				WrathTimer = urand(5000, 6000);
			}
			else WrathTimer -= diff;

			if (EntanglingRootsTimer <= diff)
			{
				DoCastVictim(SPELL_ENTANGLING_ROOTS);
				EntanglingRootsTimer = urand(7000, 8000);
			}
			else EntanglingRootsTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_earthbinder_galandria_nightbreezeAI(creature);
    }
};

enum ExorcismSpells
{
    SPELL_JULES_GOES_PRONE     = 39283, //Jules bleibt in de rluft liegen (fuer schwebephase)
    SPELL_JULES_THREATENS_AURA = 39284, //Visueller Effekt, bleibt das ganze Event lang aktiv
    SPELL_JULES_GOES_UPRIGHT   = 39294, //Jules haengt kopfueber (fuer flugphase)
    SPELL_JULES_VOMITS_AURA    = 39295, //Visueller Effekt fuer flugphase (uebelkeit und uebergeben)

    SPELL_BARADAS_COMMAND      = 39277, //Castanimation
    SPELL_BARADA_FALTERS       = 39278, //Cast startet die schwebephase (etwa 20sek nach eventbeginn)
};

enum ExorcismTexts
{
    SAY_BARADA_1 = 0,
    SAY_BARADA_2 = 1,
    SAY_BARADA_3 = 2,
    SAY_BARADA_4 = 3,
    SAY_BARADA_5 = 4,
    SAY_BARADA_6 = 5,
    SAY_BARADA_7 = 6,
    SAY_BARADA_8 = 7,

    SAY_JULES_1  = 0,
    SAY_JULES_2  = 1,
    SAY_JULES_3  = 2,
    SAY_JULES_4  = 3,
    SAY_JULES_5  = 4,
};

Position const exorcismPos[11] =
{
    { -707.123f, 2751.686f, 101.592f, 4.577416f }, //Barada Waypoint-1      0
    { -710.731f, 2749.075f, 101.592f, 1.513286f }, //Barada Castposition    1
    { -710.332f, 2754.394f, 102.948f, 3.207566f }, //Jules Schwebeposition  2
    { -714.261f, 2747.754f, 103.391f, 0.0f },      //Jules Waypoint-1       3
    { -713.113f, 2750.194f, 103.391f, 0.0f },      //Jules Waypoint-2       4
    { -710.385f, 2750.896f, 103.391f, 0.0f },      //Jules Waypoint-3       5
    { -708.309f, 2750.062f, 103.391f, 0.0f },      //Jules Waypoint-4       6
    { -707.401f, 2747.696f, 103.391f, 0.0f },      //Jules Waypoint-5       7  uebergeben
    { -708.591f, 2745.266f, 103.391f, 0.0f },      //Jules Waypoint-6       8
    { -710.597f, 2744.035f, 103.391f, 0.0f },      //Jules Waypoint-7       9
    { -713.089f, 2745.302f, 103.391f, 0.0f },      //Jules Waypoint-8      10
};

enum ExorcismMisc
{
    NPC_DARKNESS_RELEASED               = 22507,
    NPC_FOUL_PURGE                      = 22506,
    NPC_COLONEL_JULES                   = 22432,

    BARADAS_GOSSIP_MESSAGE              = 10683,

    QUEST_THE_EXORCISM_OF_COLONEL_JULES = 10935,

    ACTION_START_EVENT                  = 1,
    ACTION_JULES_HOVER                  = 2,
    ACTION_JULES_FLIGH                  = 3,
    ACTION_JULES_MOVE_HOME              = 4,
};

enum ExorcismEvents
{
    EVENT_BARADAS_TALK = 1,

    //Colonel Jules
    EVENT_SUMMON_SKULL = 1,
};

class npc_anchorite_barada : public CreatureScript
{
public:
    npc_anchorite_barada() : CreatureScript("npc_anchorite_barada") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_THE_EXORCISM_OF_COLONEL_JULES) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(0, "I am ready, Anchorite. Let us begin the exorcism.", GOSSIP_SENDER_MAIN, 1);

        player->SEND_GOSSIP_MENU(BARADAS_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == 1)
        {
            creature->AI()->Talk(SAY_BARADA_1);
            creature->GetAI()->DoAction(ACTION_START_EVENT);
            CAST_AI(npc_anchorite_barada::npc_anchorite_baradaAI, creature->GetAI())->PlayerGUID = player->GetGUID();
            player->CLOSE_GOSSIP_MENU();
        }
        return true;
    }

    struct npc_anchorite_baradaAI : public QuantumBasicAI
    {
        npc_anchorite_baradaAI(Creature* creature) : QuantumBasicAI(creature) { }

        EventMap events;
        uint8 step;

        Creature* jules;

        uint64 PlayerGUID;

        void Reset()
        {
            events.Reset();
            step = 0;

            PlayerGUID = 0;

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_KNEEL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void DoAction(int32 const action)
        {
            if (action == ACTION_START_EVENT)
            {
                jules = me->FindCreatureWithDistance(NPC_COLONEL_JULES, 20.0f, true);

                if (jules)
                    jules->AI()->Talk(SAY_JULES_1);

                me->GetMotionMaster()->MovePoint(0, exorcismPos[1]);
                Talk(SAY_BARADA_2);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == 0)
                me->GetMotionMaster()->MovePoint(1, exorcismPos[1]);

            if (id == 1)
                events.ScheduleEvent(EVENT_BARADAS_TALK, 2000);
        }

        void JustDied(Unit* /*killer*/)
        {
            if (jules)
            {
                jules->GetAI()->DoAction(ACTION_JULES_MOVE_HOME);
                jules->RemoveAllAuras();
            }
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_BARADAS_TALK:
                        switch (step)
                        {
                            case 0:
                                me->SetFacingTo(1.513286f);
                                me->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
                                events.ScheduleEvent(EVENT_BARADAS_TALK, 3000);
                                step++;
                                break;
                            case 1:
                                DoCast(SPELL_BARADAS_COMMAND);
                                events.ScheduleEvent(EVENT_BARADAS_TALK, 5000);
                                step++;
                                break;
                            case 2:
                                Talk(SAY_BARADA_3);
                                events.ScheduleEvent(EVENT_BARADAS_TALK, 7000);
                                step++;
                                break;
                            case 3:
                                if (jules)
                                    jules->AI()->Talk(SAY_JULES_2);

                                events.ScheduleEvent(EVENT_BARADAS_TALK, 18000);
                                step++;
                                break;
                            case 4:
                                DoCast(SPELL_BARADA_FALTERS);
                                me->HandleEmoteCommand(EMOTE_STAND_STATE_NONE);

                                if (jules)
                                    jules->GetAI()->DoAction(ACTION_JULES_HOVER);

                                events.ScheduleEvent(EVENT_BARADAS_TALK, 11000);
                                step++;
                                break;
                            case 5:
                                if (jules)
                                    jules->AI()->Talk(SAY_JULES_3);

                                events.ScheduleEvent(EVENT_BARADAS_TALK, 13000);
                                step++;
                                break;
                            case 6:
                                Talk(SAY_BARADA_4);
                                events.ScheduleEvent(EVENT_BARADAS_TALK, 5000);
                                step++;
                                break;
                            case 7:
                                if (jules)
                                    jules->AI()->Talk(SAY_JULES_3);

                                events.ScheduleEvent(EVENT_BARADAS_TALK, 13000);
                                step++;
                                break;
                            case 8:
                                Talk(SAY_BARADA_4);
                                events.ScheduleEvent(EVENT_BARADAS_TALK, 12000);
                                step++;
                                break;
                            case 9:
                                if (jules)
                                    jules->AI()->Talk(SAY_JULES_4);

                                events.ScheduleEvent(EVENT_BARADAS_TALK, 12000);
                                step++;
                                break;
                            case 10:
                                Talk(SAY_BARADA_4);
                                events.ScheduleEvent(EVENT_BARADAS_TALK, 5000);
                                step++;
                                break;
                            case 11:
                                if (jules)
                                    jules->GetAI()->DoAction(ACTION_JULES_FLIGH);

                                events.ScheduleEvent(EVENT_BARADAS_TALK, 10000);
                                step++;
                                break;
                            case 12:
                                if (jules)
                                    jules->AI()->Talk(SAY_JULES_4);

                                events.ScheduleEvent(EVENT_BARADAS_TALK, 8000);
                                step++;
                                break;
                            case 13:
                                Talk(SAY_BARADA_5);
                                events.ScheduleEvent(EVENT_BARADAS_TALK, 10000);
                                step++;
                                break;
                            case 14:
                                if (jules)
                                    jules->AI()->Talk(SAY_JULES_4);

                                events.ScheduleEvent(EVENT_BARADAS_TALK, 10000);
                                step++;
                                break;
                            case 15:
                                Talk(SAY_BARADA_6);
                                events.ScheduleEvent(EVENT_BARADAS_TALK, 10000);
                                step++;
                                break;
                            case 16:
                                if (jules)
                                    jules->AI()->Talk(SAY_JULES_5);

                                events.ScheduleEvent(EVENT_BARADAS_TALK, 10000);
                                step++;
                                break;
                            case 17:
                                Talk(SAY_BARADA_7);
                                events.ScheduleEvent(EVENT_BARADAS_TALK, 10000);
                                step++;
                                break;
                            case 18:
                                if (jules)
                                    jules->AI()->Talk(SAY_JULES_3);

                                events.ScheduleEvent(EVENT_BARADAS_TALK, 10000);
                                step++;
                                break;
                            case 19:
                                Talk(SAY_BARADA_7);
                                events.ScheduleEvent(EVENT_BARADAS_TALK, 10000);
                                step++;
                                break;
                            case 20:
                                if (jules)
                                {
                                    jules->GetAI()->DoAction(ACTION_JULES_MOVE_HOME);
                                    jules->RemoveAura(SPELL_JULES_VOMITS_AURA);
                                }
                                events.ScheduleEvent(EVENT_BARADAS_TALK, 10000);
                                step++;
                                break;
                            case 21:
                                if (PlayerGUID)
                                {
                                    if (Player* player = Unit::GetPlayer(*me, PlayerGUID))
                                        player->KilledMonsterCredit(NPC_COLONEL_JULES, 0);
                                }

                                if (jules)
                                    jules->RemoveAllAuras();

                                me->RemoveAura(SPELL_BARADAS_COMMAND);
                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                                Talk(SAY_BARADA_8);
                                me->GetMotionMaster()->MoveTargetedHome();
                                EnterEvadeMode();
                                break;
                        }
                        break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anchorite_baradaAI(creature);
    }
};

class npc_colonel_jules : public CreatureScript
{
public:
    npc_colonel_jules() : CreatureScript("npc_colonel_jules") { }

    struct npc_colonel_julesAI : public QuantumBasicAI
    {
        npc_colonel_julesAI(Creature* creature) : QuantumBasicAI(creature), summons(me) { }

        EventMap events;
        SummonList summons;

        uint8 circleRounds;
        uint8 point;

        bool wpreached;

        void Reset()
        {
            events.Reset();

            summons.DespawnAll();
            circleRounds = 0;
            point = 3;
            wpreached = false;

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_SLEEP);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
                case ACTION_JULES_HOVER:
                    me->AddAura(SPELL_JULES_GOES_PRONE, me);
                    me->AddAura(SPELL_JULES_THREATENS_AURA, me);
                    me->SetCanFly(true);
                    me->SetSpeed(MOVE_RUN, 0.2f);
                    me->SetFacingTo(3.207566f);
                    me->GetMotionMaster()->MoveJump(exorcismPos[2], 2.0f, 2.0f);
                    events.ScheduleEvent(EVENT_SUMMON_SKULL, 10000);
                    break;
                case ACTION_JULES_FLIGH:
                    circleRounds++;
                    me->RemoveAura(SPELL_JULES_GOES_PRONE);
                    me->AddAura(SPELL_JULES_GOES_UPRIGHT, me);
                    me->AddAura(SPELL_JULES_VOMITS_AURA, me);
                    wpreached = true;
                    me->GetMotionMaster()->MovePoint(point, exorcismPos[point]);
                    break;
                case ACTION_JULES_MOVE_HOME:
                    wpreached = false;
                    me->SetSpeed(MOVE_RUN, 1.0f);
                    me->GetMotionMaster()->MovePoint(11, exorcismPos[2]);
                    events.CancelEvent(EVENT_SUMMON_SKULL);
                    break;
            }
        }

        void JustSummoned(Creature* summon)
        {
            summons.Summon(summon);

            if (summon->GetAI())
                summon->GetMotionMaster()->MoveRandom(10.0f);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id < 10)
                wpreached = true;

            if (id == 8)
            {
                for (uint8 i = 0; i < circleRounds; i++)
                    DoSummon(NPC_FOUL_PURGE, exorcismPos[8]);
            }

            if (id == 10)
            {
                wpreached = true;
                point = 3;
                circleRounds++;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (wpreached)
            {
                me->GetMotionMaster()->MovePoint(point, exorcismPos[point]);
                point++;
                wpreached = false;
            }

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_SUMMON_SKULL:
                        uint8 summonCount = urand(1,3);

                        for (uint8 i = 0; i < summonCount; i++)
                            me->SummonCreature(NPC_DARKNESS_RELEASED, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ() + 1.5f, 0, TEMPSUMMON_MANUAL_DESPAWN);

                        events.ScheduleEvent(EVENT_SUMMON_SKULL, urand(10000, 15000));
                        break;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_colonel_julesAI(creature);
    }
};

enum QuestBurnItUpTheHorde
{
	SPELL_IGNITE_HORDE_SIEGE_ENGINE    = 32979,
	SPELL_IGNITE_ALLIANCE_SIEGE_ENGINE = 33067,
	SPELL_HELLFIRE_VISUAL              = 32475,
	SPELL_HELLFIRE_FIRE                = 34386,
};

class npc_burn_it_up_for_the_horde_east : public CreatureScript
{
public:
	npc_burn_it_up_for_the_horde_east() : CreatureScript("npc_burn_it_up_for_the_horde_east") {}

    struct npc_burn_it_up_for_the_horde_eastAI : public QuantumBasicAI
    {
        npc_burn_it_up_for_the_horde_eastAI(Creature* creature) : QuantumBasicAI(creature){}

		void Reset()
        {
			me->SetCorpseDelay(0);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_IGNITE_ALLIANCE_SIEGE_ENGINE)
			{
				if (Player* player = caster->ToPlayer())
				{
					player->CastSpell(player, SPELL_HELLFIRE_VISUAL, true);
					player->KilledMonsterCredit(me->GetEntry(), 0);

					me->CastSpell(me, SPELL_HELLFIRE_FIRE, true);
				}
			}
		}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_burn_it_up_for_the_horde_eastAI(creature);
    }
};

class npc_burn_it_up_for_the_horde_west : public CreatureScript
{
public:
	npc_burn_it_up_for_the_horde_west() : CreatureScript("npc_burn_it_up_for_the_horde_west") {}

    struct npc_burn_it_up_for_the_horde_westAI : public QuantumBasicAI
    {
        npc_burn_it_up_for_the_horde_westAI(Creature* creature) : QuantumBasicAI(creature){}

		void Reset()
        {
			me->SetCorpseDelay(0);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_IGNITE_ALLIANCE_SIEGE_ENGINE)
			{
				if (Player* player = caster->ToPlayer())
				{
					player->CastSpell(player, SPELL_HELLFIRE_VISUAL, true);
					player->KilledMonsterCredit(me->GetEntry(), 0);

					me->CastSpell(me, SPELL_HELLFIRE_FIRE, true);
				}
			}
		}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_burn_it_up_for_the_horde_westAI(creature);
    }
};

class npc_laying_waste_to_unwanted_east : public CreatureScript
{
public:
	npc_laying_waste_to_unwanted_east() : CreatureScript("npc_laying_waste_to_unwanted_east") {}

    struct npc_laying_waste_to_unwanted_eastAI : public QuantumBasicAI
    {
        npc_laying_waste_to_unwanted_eastAI(Creature* creature) : QuantumBasicAI(creature){}

		void Reset()
        {
			me->SetCorpseDelay(0);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_IGNITE_HORDE_SIEGE_ENGINE)
			{
				if (Player* player = caster->ToPlayer())
				{
					player->CastSpell(player, SPELL_HELLFIRE_VISUAL, true);
					player->KilledMonsterCredit(me->GetEntry(), 0);

					me->CastSpell(me, SPELL_HELLFIRE_FIRE, true);
				}
			}
		}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_laying_waste_to_unwanted_eastAI(creature);
    }
};

class npc_laying_waste_to_unwanted_east_02 : public CreatureScript
{
public:
	npc_laying_waste_to_unwanted_east_02() : CreatureScript("npc_laying_waste_to_unwanted_east_02") {}

    struct npc_laying_waste_to_unwanted_east_02AI : public QuantumBasicAI
    {
        npc_laying_waste_to_unwanted_east_02AI(Creature* creature) : QuantumBasicAI(creature){}

		void Reset()
        {
			me->SetCorpseDelay(0);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_IGNITE_HORDE_SIEGE_ENGINE)
			{
				if (Player* player = caster->ToPlayer())
				{
					player->CastSpell(player, SPELL_HELLFIRE_VISUAL, true);
					player->KilledMonsterCredit(me->GetEntry(), 0);

					me->CastSpell(me, SPELL_HELLFIRE_FIRE, true);
				}
			}
		}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_laying_waste_to_unwanted_east_02AI(creature);
    }
};

class npc_laying_waste_to_unwanted_west : public CreatureScript
{
public:
	npc_laying_waste_to_unwanted_west() : CreatureScript("npc_laying_waste_to_unwanted_west") {}

    struct npc_laying_waste_to_unwanted_westAI : public QuantumBasicAI
    {
        npc_laying_waste_to_unwanted_westAI(Creature* creature) : QuantumBasicAI(creature){}

		void Reset()
        {
			me->SetCorpseDelay(0);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_IGNITE_HORDE_SIEGE_ENGINE)
			{
				if (Player* player = caster->ToPlayer())
				{
					player->CastSpell(player, SPELL_HELLFIRE_VISUAL, true);
					player->KilledMonsterCredit(me->GetEntry(), 0);

					me->CastSpell(me, SPELL_HELLFIRE_FIRE, true);
				}
			}
		}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_laying_waste_to_unwanted_westAI(creature);
    }
};

class npc_laying_waste_to_unwanted_west_02 : public CreatureScript
{
public:
	npc_laying_waste_to_unwanted_west_02() : CreatureScript("npc_laying_waste_to_unwanted_west_02") {}

    struct npc_laying_waste_to_unwanted_west_02AI : public QuantumBasicAI
    {
        npc_laying_waste_to_unwanted_west_02AI(Creature* creature) : QuantumBasicAI(creature){}

		void Reset()
        {
			me->SetCorpseDelay(0);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_IGNITE_HORDE_SIEGE_ENGINE)
			{
				if (Player* player = caster->ToPlayer())
				{
					player->CastSpell(player, SPELL_HELLFIRE_VISUAL, true);
					player->KilledMonsterCredit(me->GetEntry(), 0);

					me->CastSpell(me, SPELL_HELLFIRE_FIRE, true);
				}
			}
		}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_laying_waste_to_unwanted_west_02AI(creature);
    }
};

void AddSC_hellfire_peninsula()
{
    new npc_aeranas();
    new npc_ancestral_wolf();
    new go_haaleshi_altar();
    new npc_naladu();
    new npc_tracy_proudwell();
    new npc_trollbane();
    new npc_wounded_blood_elf();
    new npc_fel_guard_hound();
    new npc_forward_commander_toarch();
    new npc_shattered_hand_berserker();
	new npc_felblood_initiate();
	new npc_living_flare();
	new npc_incandescent_fel_spark();
	new npc_earthbinder_galandria_nightbreeze();
	new npc_anchorite_barada();
    new npc_colonel_jules();
	new npc_burn_it_up_for_the_horde_east();
	new npc_burn_it_up_for_the_horde_west();
	new npc_laying_waste_to_unwanted_east();
	new npc_laying_waste_to_unwanted_east_02();
	new npc_laying_waste_to_unwanted_west();
	new npc_laying_waste_to_unwanted_west_02();
}