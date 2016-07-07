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
#include "Vehicle.h"

enum DrakuruShackles
{
    SPELL_LEFT_CHAIN           = 59951,
    SPELL_RIGHT_CHAIN          = 59952,
    SPELL_UNLOCK_SHACKLE       = 55083,
    SPELL_FREE_RAGECLAW        = 55223,

    NPC_RAGECLAW               = 29686,
    QUEST_TROLLS_IS_GONE_CRAZY = 12861,
};

class npc_drakuru_shackles : public CreatureScript
{
public:
    npc_drakuru_shackles() : CreatureScript("npc_drakuru_shackles") { }

    struct npc_drakuru_shacklesAI : public QuantumBasicAI
    {
        npc_drakuru_shacklesAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint64 RageclawGUID;

        void Reset()
        {
            RageclawGUID = 0;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            float x, y, z;
            me->GetClosePoint(x, y, z, me->GetObjectSize() / 3, 0.1f);

            if (Unit* summon = me->SummonCreature(NPC_RAGECLAW, x, y, z, 0, TEMPSUMMON_DEAD_DESPAWN, 1000))
            {
                RageclawGUID = summon->GetGUID();
                LockRageclaw();
            }
        }

        void LockRageclaw()
        {
            Unit* Rageclaw = Unit::GetCreature(*me, RageclawGUID);
            // pointer check not needed
            me->SetInFront(Rageclaw);
            Rageclaw->SetInFront(me);

            DoCast(Rageclaw, SPELL_LEFT_CHAIN, true);
            DoCast(Rageclaw, SPELL_RIGHT_CHAIN, true);
        }

        void UnlockRageclaw(Unit* who)
        {
            if (!who)
                return;

            Creature* rageclaw = Unit::GetCreature(*me, RageclawGUID);
            // pointer check not needed
            DoCast(rageclaw, SPELL_FREE_RAGECLAW, true);

            me->setDeathState(DEAD);

            if (who->GetTypeId() == TYPE_ID_PLAYER)
                who->ToPlayer()->KilledMonsterCredit(NPC_RAGECLAW,0);
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_UNLOCK_SHACKLE)
            {
                if (caster->ToPlayer()->GetQuestStatus(QUEST_TROLLS_IS_GONE_CRAZY) == QUEST_STATUS_INCOMPLETE)
                {
                    if (Creature* pRageclaw = Unit::GetCreature(*me, RageclawGUID))
                    {
                        UnlockRageclaw(caster);
                        caster->ToPlayer()->KilledMonster(pRageclaw->GetCreatureTemplate(), RageclawGUID);
                        me->DisappearAndDie();
                    }
                    else me->setDeathState(JUST_DIED);
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drakuru_shacklesAI(creature);
    }
};

enum Rageclaw
{
    SPELL_UNSHACKLED = 55085,
    SPELL_KNEEL      = 39656,
};

const char * SAY_RAGECLAW_1 = "I poop on you, trollses!";
const char * SAY_RAGECLAW_2 = "ARRRROOOOGGGGAAAA!";
const char * SAY_RAGECLAW_3 = "No more mister nice wolvar!";

#define SAY_RAGECLAW RAND(SAY_RAGECLAW_1, SAY_RAGECLAW_2, SAY_RAGECLAW_3)

class npc_captured_rageclaw : public CreatureScript
{
public:
    npc_captured_rageclaw() : CreatureScript("npc_captured_rageclaw") { }

    struct npc_captured_rageclawAI : public QuantumBasicAI
    {
        npc_captured_rageclawAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 DespawnTimer;
        bool Despawn;

        void Reset()
        {
            Despawn = false;
            DespawnTimer = 0;
            me->SetCurrentFaction(35);
            DoCast(me, SPELL_KNEEL, true); // Little Hack for kneel - Thanks Illy :P
        }

        void MoveInLineOfSight(Unit* /*who*/){}

        void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_FREE_RAGECLAW)
            {
                me->RemoveAurasDueToSpell(SPELL_LEFT_CHAIN);
                me->RemoveAurasDueToSpell(SPELL_RIGHT_CHAIN);
                me->RemoveAurasDueToSpell(SPELL_KNEEL);
                me->SetCurrentFaction(me->GetCreatureTemplate()->faction_H);

                DoCast(me, SPELL_UNSHACKLED, true);
                me->MonsterSay(SAY_RAGECLAW, LANG_UNIVERSAL, 0);
                me->GetMotionMaster()->MoveRandom(10);

                DespawnTimer = 10000;
                Despawn = true;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (UpdateVictim())
            {
                DoMeleeAttackIfReady();
                return;
            }

            if (!Despawn)
                return;

            if (DespawnTimer <= diff)
			{
                me->DisappearAndDie();
			}
            else DespawnTimer -= diff;
		}
	};

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_captured_rageclawAI(creature);
	}
};

#define GOSSIP_ITEM_G "I'm ready, Gymer. Let's go!"

enum Gymer
{
    QUEST_STORM_KING_VENGEANCE = 12919,
    SPELL_GYMER                = 55568,
};

class npc_gymer : public CreatureScript
{
public:
    npc_gymer() : CreatureScript("npc_gymer") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

		if (player->GetQuestStatus(QUEST_STORM_KING_VENGEANCE) == QUEST_STATUS_INCOMPLETE)
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_G, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(13640, creature->GetGUID());
        }

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->CLOSE_GOSSIP_MENU();
            player->CastSpell(player, SPELL_GYMER, true);
        }
        return true;
    }
};

enum Gurgthock
{
    QUEST_AMPHITHEATER_ANGUISH_YGGDRAS_1          = 12932,
    QUEST_AMPHITHEATER_ANGUISH_MAGNATAUR          = 12933,
    QUEST_AMPHITHEATER_ANGUISH_FROM_BEYOND        = 12934,
	QUEST_AMPHITHEATER_ANGUISH_TUSKARRMAGEDDON    = 12935,
	QUEST_AMPHITHEATER_ANGUISH_KORRAK_BLOODRAGER  = 12936,
	QUEST_AMPHITHEATER_ANGUISH_VLADOF             = 12948,
	QUEST_AMPHITHEATER_ANGUISH_YGGDRAS_2          = 12954,

    NPC_ORINOKO_TUSKBREAKER                       = 30020,
    NPC_KORRAK_BLOODRAGER                         = 30023,
    NPC_YGGDRAS                                   = 30014,
    NPC_STINKBEARD                                = 30017,
	NPC_VLADOF_THE_BUTCHER                        = 30022,
    NPC_AZ_BARIN                                  = 30026, // air
    NPC_DUKE_SINGEN                               = 30019, // fire
    NPC_ERATHIUS                                  = 30025, // earth
    NPC_GARGORAL                                  = 30024, // water
    NPC_FIEND_FIRE                                = 30042,
    NPC_FIEND_EARTH                               = 30043,
	NPC_FIEND_WATER                               = 30044,
	NPC_FIEND_AIR                                 = 30045,

    SAY_QUEST_ACCEPT_TUSKARRMAGEDON               = -1571031,
    SAY_QUEST_ACCEPT_KORRAK_1                     = -1571033,
    SAY_QUEST_ACCEPT_KORRAK_2                     = -1571034,
    SAY_QUEST_ACCEPT_MAGNATAUR                    = -1571035,

    EMOTE_YGGDRAS_SPAWN                           = -1571039,
    SAY_STINKBEARD_SPAWN                          = -1571040,
    SAY_GURGTHOCK_ELEMENTAL_SPAWN                 = -1571041,

	SPELL_DUAL_WIELD                              = 674,
    SPELL_CRASHING_WAVE                           = 55909, // water
    SPELL_SHOCKWAVE                               = 55918, // earth
    SPELL_BLAST_OF_AIR                            = 55912, // air
    SPELL_MAGMA_WAVE                              = 55916, // fire

    SPELL_ORB_OF_WATER                             = 55888, // fiend of water spell
    SPELL_ORB_OF_STORMS                            = 55882, // fiend of air spell
    SPELL_BOULDER                                  = 55886, // fiend of earth spell
    SPELL_ORB_OF_FLAME                             = 55872, // fiend of fire spell
};

struct BossAndAdd
{
    uint32 uiBoss;
    uint32 uiAdd;
    uint32 uiSpell;
    uint32 uiAddSpell;
};

static BossAndAdd Boss[] =
{
    {NPC_GARGORAL, NPC_FIEND_WATER, SPELL_CRASHING_WAVE, SPELL_ORB_OF_WATER},
    {NPC_AZ_BARIN, NPC_FIEND_AIR, SPELL_BLAST_OF_AIR, SPELL_ORB_OF_STORMS},
    {NPC_DUKE_SINGEN, NPC_FIEND_FIRE, SPELL_MAGMA_WAVE, SPELL_ORB_OF_FLAME},
    {NPC_ERATHIUS, NPC_FIEND_EARTH, SPELL_SHOCKWAVE, SPELL_BOULDER},
};

const Position SpawnPosition[] =
{
    {5754.692f, -2939.46f, 286.276123f, 5.156380f}, // stinkbeard || orinoko || korrak
    {5762.054199f, -2954.385010f, 273.826955f, 5.108289f},  //yggdras
    {5776.855f, -2989.77979f, 272.96814f, 5.194f} // elementals
};

const Position AddSpawnPosition[] =
{
    {5722.487f, -3010.75f, 312.751648f, 0.478f}, // caster location
    {5724.983f, -2969.89551f, 286.359619f, 0.478f},
    {5733.76025f, -3000.34644f, 286.359619f, 0.478f},
    {5739.8125f, -2981.524f, 290.7671f, 0.478f}, // caster location
    {5742.101f, -2950.75586f, 286.2643f, 5.21f},
    {5743.305f, -3011.29736f, 290.7671f, 0.478f}, // caster location
    {5744.417f, -3025.528f, 286.35965f, 0.478f},
    {5763.189f, -3029.67529f, 290.7671f, 0.478f},
    {5769.401f, -2935.121f, 286.335754f, 5.21f},
    {5793.061f, -2934.593f, 286.359619f, 3.53f},
    {5797.32129f, -2955.26855f, 290.7671f, 3.53f}, // caster location
    {5813.94531f, -2956.74683f, 286.359619f, 3.53f},
    {5816.85547f, -2974.476f, 290.7671f, 3.53f}, // caster location
    {5820.30859f, -3002.83716f, 290.7671f, 3.53f}, // caster location
    {5828.50244f, -2981.737f, 286.359619f, 3.53f},
    {5828.899f, -2960.15479f, 312.751648f, 3.53f}, // caster location
};

class npc_gurgthock : public CreatureScript
{
public:
    npc_gurgthock() : CreatureScript("npc_gurgthock") { }

    struct npc_gurgthockAI : public QuantumBasicAI
    {
        npc_gurgthockAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint64 SummonGUID;
        uint64 PlayerGUID;
        uint32 Timer;
        uint32 Phase;
        uint32 RemoveFlagTimer;
        uint32 Quest;
        uint8 BossRandom;

        bool RemoveFlag;

        void Reset()
        {
            SummonGUID = 0;
            PlayerGUID = 0;

            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUEST_GIVER);

            Timer = 0;
            Phase = 0;
            Quest = 0;

            RemoveFlagTimer = 5000;

            BossRandom = 0;
            RemoveFlag = false;
        }

        void SetGUID(uint64 guid, int32 /*id*/)
        {
            PlayerGUID = guid;
        }

		void SetData(uint32 id, uint32 value)
        {
            RemoveFlag = true;
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUEST_GIVER);

			switch (id)
            {
                case 1:
					switch (value)
                    {
                        case QUEST_AMPHITHEATER_ANGUISH_TUSKARRMAGEDDON:
                            DoSendQuantumText(SAY_QUEST_ACCEPT_TUSKARRMAGEDON, me);
                            Phase = 1;
                            Timer = 4000;
                            break;
                        case QUEST_AMPHITHEATER_ANGUISH_KORRAK_BLOODRAGER:
                            DoSendQuantumText(SAY_QUEST_ACCEPT_KORRAK_1, me);
                            Phase = 3;
                            Timer = 3000;
                            break;
                        case QUEST_AMPHITHEATER_ANGUISH_YGGDRAS_2:
                        case QUEST_AMPHITHEATER_ANGUISH_YGGDRAS_1:
                            Phase = 6;
                            Timer = 3000;
                            break;
                        case QUEST_AMPHITHEATER_ANGUISH_MAGNATAUR:
                            Timer = 5000;
                            Phase = 7;
                            break;
                        case QUEST_AMPHITHEATER_ANGUISH_FROM_BEYOND:
                            Timer = 2000;
                            Phase = 12;
                            break;
						case QUEST_AMPHITHEATER_ANGUISH_VLADOF:
							Timer = 2000;
							Phase = 15;
							break;
					}
					break;
			}
        }

        void UpdateAI(const uint32 diff)
        {
            QuantumBasicAI::UpdateAI(diff);

            if (RemoveFlag)
            {
                if (RemoveFlagTimer <= diff)
                {
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUEST_GIVER);
                    RemoveFlag = false;

                    RemoveFlagTimer = 10*IN_MILLISECONDS;
                }
				else RemoveFlagTimer -= diff;
            }

            if (Phase)
            {
                Player* player = me->GetPlayer(*me, PlayerGUID);

                if (Timer <= diff)
                {
                    switch (Phase)
                    {
                        case 1:
                            if (Creature* summon = me->SummonCreature(NPC_ORINOKO_TUSKBREAKER, SpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 45*IN_MILLISECONDS))
                                SummonGUID = summon->GetGUID();
                            Phase = 2;
                            Timer = 4000;
                            break;
                         case 2:
                            if (Creature* summon = Unit::GetCreature(*me, SummonGUID))
                                summon->GetMotionMaster()->MoveJump(5776.319824f, -2981.005371f, 273.100037f, 10.0f, 20.0f);
                            Phase = 0;
                            SummonGUID = 0;
                            break;
                        case 3:
                            DoSendQuantumText(SAY_QUEST_ACCEPT_KORRAK_2, me);
                            Timer = 3000;
                            Phase = 4;
                            break;
                        case 4:
                            if (Creature* summon = me->SummonCreature(NPC_KORRAK_BLOODRAGER, SpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 45*IN_MILLISECONDS))
                                SummonGUID = summon->GetGUID();
                            Timer = 3000;
                            Phase = 0;
                            break;
                        case 6:
                            {
                                if (!player)
                                    return;

                                std::string sText = ("The grand Amphitheater of Anguish awaits, " + std::string(player->GetName()) + ". Remember, once a battle starts you have to stay in the area. WIN OR DIE!");

                                me->MonsterSay(sText.c_str(), LANG_UNIVERSAL, 0);
                                Timer = 5000;
                                Phase = 9;
                            }
                            break;
                        case 7:
                            {
                               if (!player)
                                   return;

                                std::string sText = ("Prepare to make you stand, " + std::string(player->GetName()) + "! Get in the Amphitheater and stand ready! Remember, you and your opponent must stay in the arena at all times or you will be disqualified!");
                                me->MonsterSay(sText.c_str(), LANG_UNIVERSAL, 0);
                                Timer = 3000;
                                Phase = 8;
                            }
                            break;
                        case 8:
                            DoSendQuantumText(SAY_QUEST_ACCEPT_MAGNATAUR, me);
                            Timer = 5000;
                            Phase = 11;
                            break;
                        case 9:
                            {
                                if (!player)
                                    return;

                                std::string sText = ("Here we are once again, ladies and gentlemen. The epic struggle between life and death in the Amphitheater of Anguish! For this round we have " + std::string(player->GetName()) + " versus the hulking jormungar, Yg... Yggd? Yggdoze? Who comes up with these names?! " + std::string(player->GetName()) + " versus big worm!");
                                me->MonsterYell(sText.c_str(), LANG_UNIVERSAL, 0);
                                Timer = 10000;
                                Phase = 10;
                            }
                            break;
                        case 10:
                            me->SummonCreature(NPC_YGGDRAS, SpawnPosition[1], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 45*IN_MILLISECONDS);
                            DoSendQuantumText(EMOTE_YGGDRAS_SPAWN, me);
                            Phase = 0;
                            break;
                        case 11:
                            if (Creature* creature = me->SummonCreature(NPC_STINKBEARD, SpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 45*IN_MILLISECONDS))
                                DoSendQuantumText(SAY_STINKBEARD_SPAWN, creature);
                            Phase = 0;
                            break;
                        case 12:
                        {
                            if (!player)
                                return;

                            std::string sText = ("Prepare to make you stand, " + std::string(player->GetName()) + "! Get in the Amphitheater and stand ready! Remember, you and your opponent must stay in the arena at all times or you will be disqualified!");
                            me->MonsterSay(sText.c_str(), LANG_UNIVERSAL, 0);
                            Timer = 5000;
                            Phase = 13;
                        }
                        break;
                        case 13:
                            DoSendQuantumText(SAY_GURGTHOCK_ELEMENTAL_SPAWN, me);
                            Timer = 3000;
                            Phase = 14;
                            break;
                        case 14:
                            BossRandom = urand(0, 3);
                            if (Creature* creature = me->SummonCreature(Boss[BossRandom].uiBoss, SpawnPosition[2], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 45*IN_MILLISECONDS))
                                creature->AI()->SetData(1, BossRandom);
                            Phase = 0;
                            break;
						case 15:
							if (Creature* pSummon = me->SummonCreature(NPC_VLADOF_THE_BUTCHER, SpawnPosition[0], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 45*IN_MILLISECONDS))
								SummonGUID = pSummon->GetGUID();
							Phase = 16;
							Timer = 4000;
							break;
						case 16:
							if (Creature* pSummon = Unit::GetCreature(*me, SummonGUID))
								pSummon->GetMotionMaster()->MoveJump(5776.319824f, -2981.005371f, 273.100037f, 10.0f, 20.0f);
							Phase = 0;
							SummonGUID = 0;
							break;
                    }
                }
				else Timer -= diff;
            }
        }
    };

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        switch (quest->GetQuestId())
        {
            case QUEST_AMPHITHEATER_ANGUISH_TUSKARRMAGEDDON:
                creature->AI()->SetData(1, quest->GetQuestId());
                break;
            case QUEST_AMPHITHEATER_ANGUISH_KORRAK_BLOODRAGER:
                creature->AI()->SetData(1, quest->GetQuestId());
                break;
            case QUEST_AMPHITHEATER_ANGUISH_YGGDRAS_2:
            case QUEST_AMPHITHEATER_ANGUISH_YGGDRAS_1:
                creature->AI()->SetData(1, quest->GetQuestId());
                break;
            case QUEST_AMPHITHEATER_ANGUISH_MAGNATAUR:
                creature->AI()->SetData(1, quest->GetQuestId());
                break;
            case QUEST_AMPHITHEATER_ANGUISH_FROM_BEYOND:
                creature->AI()->SetData(1, quest->GetQuestId());
                break;
			case QUEST_AMPHITHEATER_ANGUISH_VLADOF:
				creature->AI()->SetData(1, quest->GetQuestId());
				break;
        }

        creature->AI()->SetGUID(player->GetGUID());
        return false;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gurgthockAI(creature);
    }
};

enum OrinokoTuskbreaker
{
    SPELL_BATTLE_SHOUT      = 32064,
    SPELL_FISHY_SCENT       = 55937,
    SPELL_IMPALE            = 55929,
    SPELL_SUMMON_WHISKER    = 55946,

    NPC_WHISKER             = 30113,
    NPC_HUNGRY_PENGUIN      = 30110,

    SAY_CALL_FOR_HELP       = -1571032,
};

class npc_orinoko_tuskbreaker : public CreatureScript
{
public:
    npc_orinoko_tuskbreaker() : CreatureScript("npc_orinoko_tuskbreaker") { }

    struct npc_orinoko_tuskbreakerAI : public QuantumBasicAI
    {
        npc_orinoko_tuskbreakerAI(Creature* creature) : QuantumBasicAI(creature)
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            me->SetReactState(REACT_PASSIVE);
        }

        bool Summoned;
        bool BattleShout;
        bool FishyScent;

        uint32 BattleShoutTimer;
        uint32 FishyScentTimer;

        uint64 AffectedGUID;
        uint64 WhiskerGUID;

        void Reset()
        {
            Summoned = false;
            BattleShout = false;
            FishyScent = false;

            BattleShoutTimer = 0;
            FishyScentTimer = 10*IN_MILLISECONDS;

            WhiskerGUID = 0;
            AffectedGUID = 0;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterEvadeMode()
        {
            if (Creature* whisker = me->GetCreature(*me, WhiskerGUID))
                whisker->RemoveFromWorld();
        }

        void MovementInform(uint32 type, uint32 /*pointId*/)
        {
			if (type != EFFECT_MOTION_TYPE)
                return;

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            me->SetReactState(REACT_AGGRESSIVE);
            me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
            BattleShoutTimer  = 7*IN_MILLISECONDS;
        }

        void EnterToBattle(Unit* who)
        {
            DoCast(who, SPELL_IMPALE);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (BattleShoutTimer <= diff && !BattleShout)
            {
                DoCast(me, SPELL_BATTLE_SHOUT);
                BattleShout = true;
            }
			else BattleShoutTimer -= diff;

            if (FishyScentTimer <= diff)
            {
                if (Unit* pAffected = SelectTarget(TARGET_RANDOM, 0))
                {
                    DoCast(pAffected, SPELL_FISHY_SCENT);
                    AffectedGUID = pAffected->GetGUID();
					FishyScentTimer = 20*IN_MILLISECONDS;
				}
            }
			else FishyScentTimer -= diff;

            if (!Summoned && !HealthAbovePct(HEALTH_PERCENT_50))
            {
                DoSendQuantumText(SAY_CALL_FOR_HELP, me);

                if (Creature* whisker = me->SummonCreature(NPC_WHISKER, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 30*IN_MILLISECONDS))
                    WhiskerGUID = whisker->GetGUID();

                Summoned = true;
            }

            DoMeleeAttackIfReady();
        }

        void JustSummoned(Creature* summon)
        {
            switch (summon->GetEntry())
            {
                case NPC_WHISKER:
                    summon->AI()->AttackStart(me->GetVictim());
                    break;
                case NPC_HUNGRY_PENGUIN:
                    if (Unit* affected = Unit::GetUnit(*me, AffectedGUID))
                    {
                        if (affected->IsAlive())
                            summon->AI()->AttackStart(affected);
                    }
                    break;
            }
        }

        void JustDied(Unit* killer)
        {
            if (WhiskerGUID)
			{
                if (Creature* whisker = me->GetCreature(*me, WhiskerGUID))
                    whisker->DespawnAfterAction();
			}

            if (killer->GetTypeId() == TYPE_ID_PLAYER)
                killer->GetCharmerOrOwnerPlayerOrPlayerItself()->GroupEventHappens(QUEST_AMPHITHEATER_ANGUISH_TUSKARRMAGEDDON, killer);

        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_orinoko_tuskbreakerAI(creature);
    }
};

enum VladofTheButcher
{
	SPELL_BLOOD_PRESENCE   = 50689,
	SPELL_BLOOD_PLAGUE     = 55973,
	SPELL_BLOOD_BOIL       = 55974,
	SPELL_HYSTERIA         = 55975,
	SPELL_SPELL_DEFLECTION = 55976,

	VLADOF_MOUNT_ID        = 26510,
};

class npc_vladof_the_butcher : public CreatureScript
{
public:
    npc_vladof_the_butcher() : CreatureScript("npc_vladof_the_butcher") { }

    struct npc_vladof_the_butcherAI : public QuantumBasicAI
    {
        npc_vladof_the_butcherAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BloodBoilTimer;
		uint32 BloodPlagueTimer;
		uint32 HysteriaTimer;
		uint32 SpellDeflectionTimer;

        void Reset()
        {
			DoCast(me, SPELL_DUAL_WIELD, true);
			DoCast(me, SPELL_BLOOD_PRESENCE, true);

			BloodBoilTimer = 1*IN_MILLISECONDS;
			BloodPlagueTimer = 2*IN_MILLISECONDS;
			HysteriaTimer = 4*IN_MILLISECONDS;
			SpellDeflectionTimer = 6*IN_MILLISECONDS;

			me->Mount(VLADOF_MOUNT_ID);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveMount();

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (BloodBoilTimer <= diff)
            {
				DoCastAOE(SPELL_BLOOD_BOIL);
                BloodBoilTimer = 4*IN_MILLISECONDS;
            }
			else BloodBoilTimer -= diff;

            if (BloodPlagueTimer <= diff)
            {
                DoCastVictim(SPELL_BLOOD_PLAGUE);
                BloodPlagueTimer = 6*IN_MILLISECONDS;
            }
			else BloodPlagueTimer -= diff;

			if (HysteriaTimer <= diff)
            {
                DoCast(me, SPELL_HYSTERIA);
                HysteriaTimer = 8*IN_MILLISECONDS;
            }
			else HysteriaTimer -= diff;

			if (SpellDeflectionTimer <= diff)
            {
                DoCast(me, SPELL_SPELL_DEFLECTION);
                SpellDeflectionTimer = 10*IN_MILLISECONDS;
            }
			else SpellDeflectionTimer -= diff;

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* killer)
        {
            if (killer->GetTypeId() == TYPE_ID_PLAYER)
                killer->GetCharmerOrOwnerPlayerOrPlayerItself()->GroupEventHappens(QUEST_AMPHITHEATER_ANGUISH_VLADOF, killer);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vladof_the_butcherAI(creature);
    }
};

enum KorrakBloodrager
{
    SPELL_GROW     = 55948,
    SPELL_CHARGE   = 24193,
    SPELL_UPPERCUT = 30471,
    SPELL_ENRAGE   = 42745
};

class npc_korrak_bloodrager : public CreatureScript
{
public:
    npc_korrak_bloodrager() : CreatureScript("npc_korrak_bloodrager") { }

    struct npc_korrak_bloodragerAI : public npc_escortAI
    {
        npc_korrak_bloodragerAI(Creature* creature) : npc_escortAI(creature)
        {
            Start(true, true, 0, 0);
            SetDespawnAtEnd(false);
        }

        uint32 ChargeTimer;
        uint32 UppercutTimer;

        bool Enrage;

        void Reset()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            me->SetReactState(REACT_PASSIVE);

            ChargeTimer = 1*IN_MILLISECONDS;
            UppercutTimer = 3*IN_MILLISECONDS;

            Enrage = false;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void WaypointReached(uint32 id)
        {
            switch (id)
            {
                case 6:
                    me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                    me->SetReactState(REACT_AGGRESSIVE);
                    break;
            }
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoCast(me, SPELL_GROW);
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            if (ChargeTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_FARTHEST, 0))
				{
                    DoCast(target, SPELL_CHARGE);
					ChargeTimer = 6*IN_MILLISECONDS;
				}
            }
			else ChargeTimer -= diff;

			if (UppercutTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_NEAREST, 0))
				{
                    DoCast(target, SPELL_UPPERCUT);
					UppercutTimer = 8*IN_MILLISECONDS;
				}
            }
			else UppercutTimer -= diff;

            if (!Enrage && !HealthAbovePct(HEALTH_PERCENT_50))
            {
                DoCast(me, SPELL_ENRAGE);
                Enrage = true;
            }
            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* killer)
        {
            if (Player* player = killer->GetCharmerOrOwnerPlayerOrPlayerItself())
                player->GroupEventHappens(QUEST_AMPHITHEATER_ANGUISH_KORRAK_BLOODRAGER, killer);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_korrak_bloodragerAI(creature);
    }
};

enum Yggdras
{
    SPELL_CLEAVE          = 40504,
    SPELL_CORRODE_FLESH   = 57076,
    SPELL_JORMUNGAR_SPAWN = 55859,
};

class npc_yggdras : public CreatureScript
{
public:
    npc_yggdras() : CreatureScript("npc_yggdras") { }

    struct npc_yggdrasAI : public QuantumBasicAI
    {
        npc_yggdrasAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 CleaveTimer;
        uint32 CorrodeFleshTimer;

        void Reset()
        {
            CleaveTimer = 2000;
            CorrodeFleshTimer = 4000;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (me->GetVictim()->GetPositionZ() >= 286.276f)
            {
                std::list<HostileReference *> t_list = me->getThreatManager().getThreatList();
                for (std::list<HostileReference *>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                {
                    if (Unit* unit = Unit::GetUnit(*me, (*itr)->getUnitGuid()))
                    {
                        if (unit->GetPositionZ() <= 286.276f)
                        {
                            me->getThreatManager().resetAllAggro();
                            me->AddThreat(unit, 5.0f);
                            break;
                        }
                        EnterEvadeMode();
                    }
                }
            }

            if (CleaveTimer <= diff)
            {
                DoCastVictim(SPELL_CLEAVE);
                CleaveTimer = 4000;
            }
			else CleaveTimer -= diff;

            if (CorrodeFleshTimer <= diff)
            {
                DoCastVictim(SPELL_CORRODE_FLESH);
                CorrodeFleshTimer = 6000;
            }
			else CorrodeFleshTimer -= diff;

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* killer)
        {
            if (Unit* summoner = me->ToTempSummon()->GetSummoner())
            {
                std::string sText = (std::string(killer->GetName()) + " has defeated Yg.. Yggg-really big worm!");
                summoner->MonsterYell(sText.c_str(), LANG_UNIVERSAL, 0);
            }

            if (Player* player = killer->GetCharmerOrOwnerPlayerOrPlayerItself())
            {
                player->GroupEventHappens(QUEST_AMPHITHEATER_ANGUISH_YGGDRAS_1, killer);
                player->GroupEventHappens(QUEST_AMPHITHEATER_ANGUISH_YGGDRAS_2, killer);
            }

            for (uint8 i = 0; i < 3; ++i)
                DoCast(killer, SPELL_JORMUNGAR_SPAWN, true);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_yggdrasAI(creature);
    }
};

enum Stinkbeard
{
    SPELL_ENRAGE_STINKBEARD = 50420,
    SPELL_KNOCK_AWAY        = 31389,
    SPELL_STINKY_BEARD      = 55867,
    SPELL_THUNDERBLADE      = 55866,
    SPELL_THUNDERCLAP       = 15588
};

class npc_stinkbeard : public CreatureScript
{
public:
    npc_stinkbeard() : CreatureScript("npc_stinkbeard") { }

    struct npc_stinkbeardAI : public npc_escortAI
    {
        npc_stinkbeardAI(Creature* creature) : npc_escortAI(creature)
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            me->SetReactState(REACT_PASSIVE);
            Start(true, true, 0, 0);
            SetDespawnAtEnd(false);
        }

        uint32 KnockAwayTimer;
        uint32 StinkyBeardTimer;

        bool Enrage;
        bool ThunderClap;

        void Reset()
        {
			DoCast(me, SPELL_DUAL_WIELD, true);

            me->AddAura(SPELL_THUNDERBLADE, me);

            KnockAwayTimer = 10000;
            StinkyBeardTimer = 15000;

            Enrage = false;
            ThunderClap = false;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void WaypointReached(uint32 id)
        {
            switch (id)
            {
                case 7:
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->SetHomePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                    break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;

            if (Unit* victim = me->GetVictim())
            {
                if (victim->GetPositionZ() >= 286.276f)
                {
                    std::list<HostileReference *> t_list = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference *>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        if (Unit* unit = Unit::GetUnit(*me, (*itr)->getUnitGuid()))
                        {
                            if (unit->GetPositionZ() <= 286.276f)
                            {
                                me->getThreatManager().resetAllAggro();
                                me->AddThreat(unit, 5.0f);
                                break;
                            }
                            EnterEvadeMode();
                        }
                    }
                }
            }

            if (ThunderClap && !HealthAbovePct(HEALTH_PERCENT_10))
            {
                DoCastAOE(SPELL_THUNDERCLAP);
                ThunderClap = true;
            }

            if (KnockAwayTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
                    if (target && target->IsAlive())
					{
                        DoCast(target, SPELL_KNOCK_AWAY);
						KnockAwayTimer = 10000;
					}
				}
            }
			else KnockAwayTimer -= diff;

            if (StinkyBeardTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                {
                    if (target && target->IsAlive())
                        DoCast(target, SPELL_STINKY_BEARD);
                }
                StinkyBeardTimer = 15000;
            }
			else StinkyBeardTimer -= diff;

            if (!Enrage && !HealthAbovePct(HEALTH_PERCENT_20))
            {
                DoCast(me, SPELL_ENRAGE_STINKBEARD);
                Enrage = true;
            }

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* killer)
        {
            if (Player* player = killer->GetCharmerOrOwnerPlayerOrPlayerItself())
                player->GetCharmerOrOwnerPlayerOrPlayerItself()->GroupEventHappens(QUEST_AMPHITHEATER_ANGUISH_MAGNATAUR, killer);

            std::string sText = ("And with AUTHORITY, " + std::string(killer->GetName()) + " dominates the magnataur lord! Stinkbeard's clan is gonna miss him back home in the Dragonblight!");
            me->MonsterYell(sText.c_str(), LANG_UNIVERSAL, 0);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stinkbeardAI(creature);
    }
};

class npc_elemental_lord : public CreatureScript
{
public:
    npc_elemental_lord() : CreatureScript("npc_elemental_lord") { }

    struct npc_elemental_lordAI : public QuantumBasicAI
    {
        npc_elemental_lordAI(Creature* creature) : QuantumBasicAI(creature) {}

        std::list<uint64> SummonList;

        uint32 ElementalSpellTimer;

        uint8 BossRandom;
        uint32 uiSpellInfo;

        bool bAddAttack;

        void Reset()
        {
            BossRandom = 0;
            uiSpellInfo = 0;
            ElementalSpellTimer = urand(5000, 8000);

            bAddAttack = false;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void SetData(uint32 uiData, uint32 uiValue)
        {
            if (uiData == 1)
            {
                BossRandom = uiValue;
                SummonAdds();
            }
        }

        void SummonAdds()
        {
            if (!Boss[BossRandom].uiAdd)
                return;

            SummonList.clear();

            for (uint8 uiI = 0; uiI < 16; uiI++)
            {
                if (Creature* summon = me->SummonCreature(Boss[BossRandom].uiAdd, AddSpawnPosition[uiI]))
                {
                    summon->AI()->SetData(1, BossRandom);
                    SummonList.push_back(summon->GetGUID());
                }
            }
        }

        void EnterToBattle(Unit* unit)
        {
            if (!SummonList.empty())
			{
                for (std::list<uint64>::const_iterator itr = SummonList.begin(); itr != SummonList.end(); ++itr)
                {
                    if (Creature* temp = Unit::GetCreature(*me, *itr))
                    {
                        temp->m_CombatDistance = 100.0f; // ugly hack? we are not in a instance sorry. :(
                        temp->AI()->AttackStart(unit);
                    }
                }
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (me->GetVictim()->GetPositionZ() >= 286.276f)
            {
                std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                {
                    if (Unit* unit = Unit::GetUnit(*me, (*itr)->getUnitGuid()))
                    {
                        if (unit->GetPositionZ() <= 286.276f)
                        {
                            me->getThreatManager().resetAllAggro();
                            me->AddThreat(unit, 5.0f);
                            break;
                        }
                        EnterEvadeMode();
                    }
                }
            }

            if (ElementalSpellTimer <= diff)
            {
                DoCastVictim(Boss[BossRandom].uiSpell);

                ElementalSpellTimer = urand(5000, 8000);
            }
			else ElementalSpellTimer -= diff;

            if (!bAddAttack && !HealthAbovePct(HEALTH_PERCENT_20))
            {
                if (!SummonList.empty())
                    for (std::list<uint64>::const_iterator itr = SummonList.begin(); itr != SummonList.end(); ++itr)
                    {
                        if (Creature* temp = Unit::GetCreature(*me, *itr))
                        {
                            if (temp->GetPositionZ() >= 287.00f)
                                continue;

                            if (temp->GetVictim())
                                temp->GetMotionMaster()->MoveChase(temp->GetVictim());
                        }
                    }

					bAddAttack = true;
            }

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit* killer)
        {
            if (!SummonList.empty())
                for (std::list<uint64>::const_iterator itr = SummonList.begin(); itr != SummonList.end(); ++itr)
                    if (Creature* temp = Unit::GetCreature(*me, *itr))
                        temp->DespawnAfterAction();

            if (Player* player = killer->GetCharmerOrOwnerPlayerOrPlayerItself())
                player->GetCharmerOrOwnerPlayerOrPlayerItself()->GroupEventHappens(QUEST_AMPHITHEATER_ANGUISH_FROM_BEYOND, killer);

            std::string sText = (std::string(killer->GetName()) + " is victorious once more!");

            if (Unit* summoner = me->ToTempSummon()->GetSummoner())
                summoner->MonsterYell(sText.c_str(), LANG_UNIVERSAL, 0);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_elemental_lordAI(creature);
    }
};

class npc_fiend_elemental : public CreatureScript
{
public:
    npc_fiend_elemental() : CreatureScript("npc_fiend_elemental") { }

    struct npc_fiend_elementalAI : public QuantumBasicAI
    {
        npc_fiend_elementalAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 MissleTimer;
		uint32 Spell;

        void Reset()
        {
            if (me->GetPositionZ() >= 287.0f)
                me->GetMotionMaster()->MoveIdle();

			Spell = 0;
            MissleTimer = urand(2000, 7000);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void AttackStart(Unit* who)
        {
            if (!who)
                return;
			
			AttackStartNoMove(who);
		}

        void SetData(uint32 data, uint32 value)
        {
            if (data == 1)
                Spell = Boss[value].uiAddSpell;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (me->GetPositionZ() >= 287.0f)
            {
                if (MissleTimer <= diff)
                {
                    DoCast(me, SPELL_ORB_OF_FLAME);
                    MissleTimer = urand(2000, 7000);

                }
				else MissleTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fiend_elementalAI(creature);
    }
};

class npc_released_offspring_harkoa : public CreatureScript
{
public:
    npc_released_offspring_harkoa() : CreatureScript("npc_released_offspring_harkoa") { }

    struct npc_released_offspring_harkoaAI : public QuantumBasicAI
    {
        npc_released_offspring_harkoaAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset()
        {
            float x, y, z;
            me->GetClosePoint(x, y, z, me->GetObjectSize() / 3, 25.0f);
            me->GetMotionMaster()->MovePoint(0, x, y, z);
        }

        void MovementInform(uint32 type, uint32 /*pointId*/)
        {
			if (type != EFFECT_MOTION_TYPE)
				return;

			me->DisappearAndDie();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_released_offspring_harkoaAI(creature);
    }
};

enum CrusadeRecruit
{
    SPELL_QUEST_CREDIT                      = 50633,
    QUEST_TROLL_PATROL_INTESTINAL_FORTITUDE = 12509,

    GOSSIP_CRUSADE_TEXT                     = 13069,

    SAY_RECRUIT_1                           = -1571036,
    SAY_RECRUIT_2                           = -1571037,
    SAY_RECRUIT_3                           = -1571038
};

#define GOSSIP_ITEM_1 "Get out there and make those Scourge wish they were never reborn!"

class npc_crusade_recruit : public CreatureScript
{
public:
    npc_crusade_recruit() : CreatureScript("npc_crusade_recruit") { }

    struct npc_crusade_recruitAI : public QuantumBasicAI
    {
        npc_crusade_recruitAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint8 Phase;
        uint32 Timer;
        float Heading;

        void Reset()
        {
            Timer = 0;
            Phase = 0;
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_COWER);
            Heading = me->GetOrientation();
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
                            // say random text
                            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                            me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_ONESHOT_NONE);
                            DoSendQuantumText(RAND(SAY_RECRUIT_1, SAY_RECRUIT_2, SAY_RECRUIT_3), me);
                            Timer = 3000;
                            Phase = 2;
                            break;
                        case 2:
                            // walk forward
                            me->SetWalk(true);
                            me->GetMotionMaster()->MovePoint(0, me->GetPositionX() + (cos(Heading) * 10), me->GetPositionY() + (sin(Heading) * 10), me->GetPositionZ());
                            Timer = 5000;
                            Phase = 3;
                            break;
                        case 3:
                            // despawn
                            me->DisappearAndDie();
                            Timer = 0;
                            Phase = 0;
                            break;
                    }
                }
                else
                Timer -= diff;
            }
            QuantumBasicAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;
        }
    };

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_TROLL_PATROL_INTESTINAL_FORTITUDE) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        player->SEND_GOSSIP_MENU(GOSSIP_CRUSADE_TEXT, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF +1)
        {
            player->CLOSE_GOSSIP_MENU();
            creature->CastSpell(player, SPELL_QUEST_CREDIT, true);
            CAST_AI(npc_crusade_recruit::npc_crusade_recruitAI, (creature->AI()))->Phase = 1;
            creature->SetInFront(player);
            creature->SendMovementFlagUpdate();
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crusade_recruitAI(creature);
    }
};

enum Patrol
{
	QUEST_TROLL_PATROL_ALCHEMIST_APPRENTICE = 12541,
	QUEST_SOMETHING_FOR_THE_PAIN            = 12597,
	QUEST_CAN_YOU_DIG_IT                    = 12588,
	QUEST_HIGH_STANDARDS                    = 12502,
	QUEST_D_SOMETHING_FOR_THE_PAIN          = 12564,
	NPC_TROLL_PATROL_KILL_CREDIT            = 28042,
	NPC_ALCHEMIST_CREDIT                    = 28248,
	SPELL_THROW_INGREDIENT                  = 51025,
};

class npc_captain_brandon_zuldrak : public CreatureScript
{
public:
    npc_captain_brandon_zuldrak() : CreatureScript("npc_captain_brandon_zuldrak") { }

    bool OnQuestReward(Player* player, Creature* creature, Quest const* quest, uint32 /*item*/)
    {
        switch(quest->GetQuestId())
        {
		    case QUEST_SOMETHING_FOR_THE_PAIN:
			case QUEST_CAN_YOU_DIG_IT:
			case QUEST_HIGH_STANDARDS:
			case QUEST_D_SOMETHING_FOR_THE_PAIN:
				player->KilledMonsterCredit(NPC_TROLL_PATROL_KILL_CREDIT, creature->GetGUID());
				break;
		}
		return true;
    }
};

#define GOSSIP_START "I'm ready to begin. What is the first Ingredient you require?"

static const char * Intro_sayings[6] =
{
	"Quickly, get me some...",
    "Find me some...",
    "I think it needs...",
    "Alright, now fetch me some...",
    "Before it thickens, we must add...",
    "It's thickening! Quickly, get some...",
};

static const char * Ingredients[] =
{
	"Hairy Herring Heads",
	"Icecrown Bottled Water",
	"Knotroot",
	"Muddy Mire Maggots",
	"Pickled Eagle Egg",
	"Pulverized Gargoyle Teeth",
	"Putrid Pirate Perspiration",
	"Seasoned Slider Cider",
	"Speckled Guano",
	"Spiky Spider Eggs",
	"Withered Batwing",
	
	"Prismatic Mojo",
	"Raptor Claw",
	"Amberseed",
	//"Ancient Ectoplasm",
	"Shrunken Dragon's Claw",
	"Wasp's Wings",
	
	"Abomination Guts",
	"Blight Crystal",
	"Chilled Serpent Mucus",
	"Crushed Basilisk Crystals",
	"Frozen Spider Ichor",
	"Crystallized Hogsnot",
	"Ghoul Drool",
	"Trollbane"
};

/*
(190480,190482,190466,190478,190467,190477,190481,190476,190470,190479,190473,190468,
190469,190459,190464,190472,191180,191182,190462,190474,190465,190463,191181,190471)
*/

static const uint32 Ingredients_Spells[24][3] =
{
    {51072,51075,38396},//Hairy Herring Heads
    {51079,51080,38398},//Icecrown Bottled Water
    {51018,51047,38338},//Knotroot
    {51067,51068,38386},//Muddy Mire Maggots
    {51055,51056,38341},//Pickled Eagle Egg
    {51064,51065,38384},//Pulverized Gargoyle Teeth
    {51077,51078,38397},//Putrid Pirate Perspiration
    {51062,51063,38381},//Seasoned Cider Slider
    {51057,51058,38337},//Speckled Guano
    {51069,51070,38393},//Spiky Spider Eggs
    {51059,51060,38339},//Withered Batwing

    {51083,51084,38343},//Prismatic Mojo
    {51085,51086,38370},//Raptor Claw
    {51087,51088,38340},//Amberseed
    //{51089,51090,38335},//Ancient Ectoplasm //Nicht benutzt
    {51091,51092,38344},//Shrunken Dragon's Claw
    {51081,51082,38369},//Wasp's Wings

    {53150,53147,39668},//Abomination Guts
    {53158,53149,39670},//Blight Crystal
    {51093,51094,38346},//Chilled Serpent Mucus
    {51097,51098,38379},//Crushed Basilisk Crystals
    {51102,51104,38345},//Frozen Spider Ichor
    {51095,51096,38336},//Crystallized Hogsnot
    {53153,53146,39669},//Ghoul Drool
    {51100,51101,38342} //Trollbane
};

class npc_alchemist_finklestein : public CreatureScript
{
public:
    npc_alchemist_finklestein() : CreatureScript("npc_alchemist_finklestein") { }

    struct npc_alchemist_finklesteinAI : public QuantumBasicAI
    {
        npc_alchemist_finklesteinAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint8 Phase;                  //The current phase we are in
        uint32 Timer;                 //Timer until phase transition
        uint64 StarterGUID;
        uint32 NextIncedient;

        bool HasFound;
        bool ShortBreak;

        void Reset()
        {
            Timer = 0;
            Phase = 0;
            NextIncedient = 999;
            HasFound = false;
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }

        void StartTruthSerum(Player* pStarter)
        {
            if (!pStarter)
                return;

            HasFound = true;
            ShortBreak = true;
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            StarterGUID = pStarter->GetGUID();
            Phase = 1;
        }

        void Next_Phase()
        {
            if (!HasFound)
            {
                if (Player* player = Unit::GetPlayer(*me, StarterGUID))
                {
                    //player->CastSpell(me,SPELL_THROW_INGREDIENT,false);
                    player->RemoveAurasDueToSpell(Ingredients_Spells[NextIncedient][0]);
                    HasFound = true;
                    Timer = 2000;
                    ShortBreak = true;
                    Phase++;

                    if (Phase > 6)
                    {
                        player->KilledMonsterCredit(NPC_ALCHEMIST_CREDIT, me->GetGUID());
                        player->CompleteQuest(QUEST_TROLL_PATROL_ALCHEMIST_APPRENTICE);
                        EnterEvadeMode();
                        return;
                    }
                }
            }
        }

        void MoveInLineOfSight(Unit *who)
        {
            if (HasFound)
                return;

            if (who->GetTypeId() == TYPE_ID_PLAYER && who->ToPlayer()->GetGUID() == StarterGUID && me->IsWithinDist(who,5))
            {
                if (who->ToPlayer()->HasItemCount(Ingredients_Spells[NextIncedient][2],1))
					Next_Phase();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (Phase)
            {
                if (Player* player = Unit::GetPlayer(*me,StarterGUID))
                {
                    if (player->IsDead() || player->GetDistance2d(me) > 200)
                    {
                        EnterEvadeMode();
                        return;
                    }

                    if (Timer <= diff)
                    {
                        if (ShortBreak)
                        {
                            if (!HasFound)
                            {
                                player->FailQuest(QUEST_TROLL_PATROL_ALCHEMIST_APPRENTICE);
                                EnterEvadeMode();
                                return;
                            }
							else
                            {
                                me->MonsterTextEmote(Intro_sayings[Phase-1],StarterGUID,true);
                                ShortBreak = false;
                                Timer = 2000;
                            }
                        }
						else
                        {
                            uint32 NewIncedient;
                            switch(Phase)
                            {
                                case 1:
                                case 2:
                                case 3:
									do
                                    {
										NewIncedient = urand(0,10);
                                    }
									while (NewIncedient == NextIncedient);
                                    NextIncedient = NewIncedient;
                                    break;
                                case 4:
                                case 5:
                                    do
                                    {
										NewIncedient = urand(11,15);
                                    }
									while (NewIncedient == NextIncedient);
                                    NextIncedient = NewIncedient;
                                    break;
                                case 6:
                                    NextIncedient = urand(16,23);
                                    break;
                            }
                            me->MonsterTextEmote(Ingredients[NextIncedient], StarterGUID, true);
                            if (Player* player = Unit::GetPlayer(*me, StarterGUID))
                                me->CastSpell(player,Ingredients_Spells[NextIncedient][0], true);

                            ShortBreak = true;
                            HasFound = false;
                            Timer = 45000;
                        }
                    }
                    else Timer -= diff;
                }
				else
                {
                    EnterEvadeMode();
                    return;
                }
            }
            //QuantumBasicAI::UpdateAI(diff);

            //if (!UpdateVictim())
            //    return;
        }
    };

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        if (action == GOSSIP_ACTION_INFO_DEF +1)
        {
            player->CLOSE_GOSSIP_MENU();
            CAST_AI(npc_alchemist_finklesteinAI, (creature->AI()))->StartTruthSerum(player);
        }

        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());


        if (CAST_AI(npc_alchemist_finklesteinAI, (creature->AI()))->Phase == 0 && player->GetQuestStatus(QUEST_TROLL_PATROL_ALCHEMIST_APPRENTICE) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_alchemist_finklesteinAI(creature);
    }
};

enum CrusaderLeaving
{
	QUEST_WE_ARE_LEAVING      = 12504,
	NPC_ARGENT_SOLDIER_CREDIT = 28041,
	SPELL_ARGENT_SUNDER_ARMOR = 50370,
};

#define GOSSIP_LEAVING "Soldier, you have new orders. You're to pull back and report to the sergeant!"

static char* Phrases[] =
{
	"I wonder where we're headed to. And who's going to deal with these guys?",
    "Right, I'd better get back to the sergeant then.",
    "These Drakkari are just bad news. We need to leave and head to Justice Keep!",
    "Watch your back. These Drakkari are a nasty lot.",
    "Careful here, $N. These trolls killed their own snake god!",
    "Don't stay out here too long.",
    "See you around.",
    "Thanks, $N."
};

class npc_argent_crusade_we_are_leaving : public CreatureScript
{
public:
    npc_argent_crusade_we_are_leaving() : CreatureScript("npc_argent_crusade_we_are_leaving") { }

    struct npc_argent_crusade_we_are_leavingAI : public QuantumBasicAI
    {
        npc_argent_crusade_we_are_leavingAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 Timer;
        uint32 SunderTimer;

        void Reset()
        {
            Timer = 10000;
            SunderTimer = urand(10000,15000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!me->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP))
            {
                if (Timer <= diff)
                {
                    me->DealDamage(me, me->GetHealth());
                    Timer = 30000;
                }
				else Timer -= diff;
            }

            if (!UpdateVictim())
                return;

            if (SunderTimer <= diff)
            {
                DoCastVictim(SPELL_ARGENT_SUNDER_ARMOR);
                SunderTimer = urand(10000,15000);
            }
			else SunderTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        if (action == GOSSIP_ACTION_INFO_DEF +1)
        {
			creature->MonsterSay(Phrases[rand() % 8], LANG_UNIVERSAL, 0);
            player->KilledMonsterCredit(NPC_ARGENT_SOLDIER_CREDIT, 0);
            creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }
        player->CLOSE_GOSSIP_MENU();
        return true;
    }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_WE_ARE_LEAVING) == QUEST_STATUS_INCOMPLETE && !creature->IsInCombatActive())
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LEAVING, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_argent_crusade_we_are_leavingAI(creature);
    }
};

enum ScourgeEnclosure
{
    QUEST_OUR_ONLY_HOPE = 12916,

    NPC_GYMER_DUMMY     = 29928,
};

class go_scourge_enclosure : public GameObjectScript
{
public:
    go_scourge_enclosure() : GameObjectScript("go_scourge_enclosure") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (player->GetQuestStatus(QUEST_OUR_ONLY_HOPE) == QUEST_STATUS_INCOMPLETE)
        {
            Creature* pGymerDummy = go->FindCreatureWithDistance(NPC_GYMER_DUMMY, 20.0f);
            if (pGymerDummy)
            {
                go->UseDoorOrButton();
                player->KilledMonsterCredit(pGymerDummy->GetEntry(), pGymerDummy->GetGUID());
                pGymerDummy->CastSpell(pGymerDummy, 55529, true);
                pGymerDummy->DisappearAndDie();
            }
        }
        return true;
    }
};

#define GOSSIP_BLOODROSE              "Stefan told me you would demonstrate the purpose of this item."
#define TEXT1                         "Indeed. Watch this, $R."
#define TEXT2                         "Here, troll... a gift!"
#define TEXT3                         "For me? Really, mon?"
#define TEXT4                         "It....it be beautiful!"
#define TEXT5                         "Ugh... disgusting!"

enum Datura
{
	SPELL_KICK_NASS             = 51866,
    SPELL_NASS_KILL_CREDIT      = 51871,

	QUEST_NEAR_MISS             = 12637,
	QUEST_CLOSE_CALL            = 12638,

	NPC_CAPTURED_DRAKKARI_SCOUT = 28541,
	NPC_WITHERED_TROLL          = 28519,
	NPC_BLOODROSE_CREDIT        = 28532,
};

class npc_bloodrose_datura : public CreatureScript
{
public:
    npc_bloodrose_datura() : CreatureScript("npc_bloodrose_datura") { }

    struct npc_bloodrose_daturaAI : public QuantumBasicAI
    {
        npc_bloodrose_daturaAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 Timer;
        uint8 Step;
        bool Stepping;

        void Reset()
        {
            Stepping = false;
            Timer = 0;
            Step = 0;
        }

        void UpdateAI(const uint32 diff)
        {
            if (Stepping)
            {
                if (Timer < diff)
                {
                    Creature* pScout = me->FindCreatureWithDistance(NPC_CAPTURED_DRAKKARI_SCOUT, 10.0f);
                    Creature* pTroll = me->FindCreatureWithDistance(NPC_WITHERED_TROLL, 2.0f);

                    if (pScout || pTroll)
                    {
                        switch (Step)
                        {
                            case 1:
                                me->SetWalk(true);
                                me->GetMotionMaster()->MovePoint(0, 5203.73f, -1317.08f, 242.767f);
                                Timer = 3000;
                                Step = 2;
                                break;
                            case 2:
                                pScout->UpdateEntry(NPC_WITHERED_TROLL);
                                Timer = 5000;
                                Step = 3;
                                break;
                            case 3:
                                me->DealDamage(pTroll, pTroll->GetMaxHealth());
                                me->GetMotionMaster()->MoveTargetedHome();
                                Stepping = false;
                                Step = 0;
                                break;
                        }
                    }
					else Reset();

                }
				else Timer -= diff;
            }
        }
    };

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if ((player->GetQuestStatus(QUEST_NEAR_MISS) == QUEST_STATUS_INCOMPLETE) || (player->GetQuestStatus(QUEST_CLOSE_CALL) == QUEST_STATUS_INCOMPLETE))
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_BLOODROSE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        }
        else
            player->SEND_GOSSIP_MENU(13289, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        npc_bloodrose_daturaAI* pAI = CAST_AI(npc_bloodrose_datura::npc_bloodrose_daturaAI, creature->AI());

        if (!pAI)
            return false;

        if (action == GOSSIP_ACTION_INFO_DEF +1)
        {
            player->CLOSE_GOSSIP_MENU();
            player->KilledMonsterCredit(NPC_BLOODROSE_CREDIT, 0);

            pAI->Stepping = true;
            pAI->Step = 1;
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_bloodrose_daturaAI(creature);
    }
};

class npc_nass : public CreatureScript
{
public:
    npc_nass() : CreatureScript("npc_nass") { }

    struct npc_nassAI : public QuantumBasicAI
    {
        npc_nassAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint64 TrollGUID;
        bool Collecting;

        void Reset()
        {
            TrollGUID = 0;
            Collecting = false;
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_KICK_NASS)
            {
                if (Creature* pTroll = me->FindCreatureWithDistance(NPC_WITHERED_TROLL, 20.0f, false))
                {
                    if (!Collecting && pTroll->getDeathState() == CORPSE)
                    {
                        Collecting = true;
                        TrollGUID = pTroll->GetGUID();
                        me->SetReactState(REACT_PASSIVE);
                        me->GetMotionMaster()->MovePoint(1, pTroll->GetPositionX(), pTroll->GetPositionY(), pTroll->GetPositionZ());
                    }
                }
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE || id != 1)
                return;

            Creature* pTroll = me->GetCreature(*me, TrollGUID);
            Player* pOwner = me->GetCharmerOrOwnerPlayerOrPlayerItself();

            if (!pOwner)
                return;

            if (pTroll)
            {
                pTroll->RemoveCorpse();
                DoCast(pOwner, SPELL_NASS_KILL_CREDIT);
            }

            me->GetMotionMaster()->MoveFollow(pOwner, 0.0f, 0.0f);
            Collecting = false;
            me->SetReactState(REACT_AGGRESSIVE);
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
        return new npc_nassAI(creature);
    }
};

enum CommanderKunz
{
    QUEST_PATROLL     = 12596,
    QUEST_RUPERT_A    = 12598,
    QUEST_RUPERT_B    = 12606,
    SPELL_RUPERT      = 50634,
    QUEST_GRONDEL     = 12599,
    SPELL_GRONDEL     = 51233,
    QUEST_FINKLESTEIN = 12557,
    SPELL_FINKLESTEIN = 51232,
    QUEST_BRANDON     = 12597,
    CREDIT_BRANDON    = 28042,
};

class npc_commander_kunz : public CreatureScript
{
public:
    npc_commander_kunz() : CreatureScript("npc_commander_kunz") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_PATROLL)
        {
            if (player->GetQuestRewardStatus(QUEST_RUPERT_A) || player->GetQuestRewardStatus(QUEST_RUPERT_B))
                creature->CastSpell(player, SPELL_RUPERT, true);
            if (player->GetQuestRewardStatus(QUEST_GRONDEL))
                creature->CastSpell(player, SPELL_GRONDEL, true);
            if (player->GetQuestRewardStatus(QUEST_FINKLESTEIN))
                creature->CastSpell(player, SPELL_FINKLESTEIN, true);
            if (player->GetQuestRewardStatus(QUEST_BRANDON))
                player->KilledMonsterCredit(CREDIT_BRANDON, 0);
        }
        return true;
    }
};

enum TangledSkein
{
	SPELL_TANGLED_SKEIN_THROWER  = 51165,
    SPELL_SPRAYER_HUGE_EXPLOSION = 53236,
    SPELL_SUMMON_BROKEN_SPRAYER  = 51314,

    NPC_PLAGUE_SPRAYER_TRIGGER   = 29457,
    NPC_PLAGUE_SPRAYER_CREDIT    = 28289,
};

class npc_plague_sprayer : public CreatureScript
{
public:
    npc_plague_sprayer() : CreatureScript("npc_plague_sprayer") {}

    struct npc_plague_sprayerAI : public QuantumBasicAI
    {
       npc_plague_sprayerAI(Creature* creature) : QuantumBasicAI(creature) {}

        EventMap events;
        uint64 _playerGUID;
        bool hit;

        void Reset()
        {
            _playerGUID = 0;
            hit = false;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (!caster->ToPlayer())
                return;

            if (spell->Id == SPELL_TANGLED_SKEIN_THROWER && !hit)
            {
                _playerGUID = caster->GetGUID();
                me->GetMotionMaster()->MovePoint(0, caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ());
                events.ScheduleEvent(1, 4000);
                hit = true;
            }
        }

        void UpdateAI(const uint32 diff)
        {                                
            events.Update(diff);

	 	    switch (events.ExecuteEvent())
            {
                case 1:
                    if (Player* player = me->GetPlayer(*me, _playerGUID))
                    {
                        DoCast(me, SPELL_SPRAYER_HUGE_EXPLOSION);
                        player->KilledMonsterCredit(NPC_PLAGUE_SPRAYER_CREDIT, 0);
                        events.ScheduleEvent(2, 1000);
                    }
                    break;
                case 2:
                    if (Creature* trigger = me->FindCreatureWithDistance(NPC_PLAGUE_SPRAYER_TRIGGER, 15.0f, true)) // what is this trigger supposed to do? there's only 1
                    {
                        trigger->AI()->DoCast(me, SPELL_SUMMON_BROKEN_SPRAYER); // when should this be casted? who casts it?
                        me->DespawnAfterAction();
                    }
					else me->DespawnAfterAction();
                    break;
            }

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_plague_sprayerAI(creature);
    }
};

enum StefanVaduMisc
{
    SPELL_SUMMON_NASS                 = 51865,
    QUEST_KICKIN_NASS_AND_TAKIN_MANES = 12630,

    SPELL_PUSH_ENSORCELLED_CHOKER     = 53810,
    ITEM_ENSORCELLED_CHOKER           = 38699,
    QUEST_DRESSING_DOWN               = 12648,
    QUEST_SUIT_UP                     = 12649,
    QUEST_BETRAYAL                    = 12713,
};

#define GOSSIP_NASS   "I've lost Nass."
#define GOSSIP_CHOKER "I've lost my choker, please give me a new one."

class npc_stefan_vadu : public CreatureScript
{
    public:
        npc_stefan_vadu() : CreatureScript("npc_stefan_vadu") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (creature->IsQuestGiver())
                player->PrepareQuestMenu(creature->GetGUID());

            if (player->GetQuestStatus(QUEST_KICKIN_NASS_AND_TAKIN_MANES) == QUEST_STATUS_INCOMPLETE)
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_NASS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 0);

            if (!player->HasItemCount(ITEM_ENSORCELLED_CHOKER, 1, true)
				&& (player->GetQuestStatus(QUEST_DRESSING_DOWN) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_DRESSING_DOWN) == QUEST_STATUS_REWARDED
                || player->GetQuestStatus(QUEST_SUIT_UP) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_SUIT_UP) == QUEST_STATUS_REWARDED)
                && !player->GetQuestRewardStatus(QUEST_BETRAYAL))
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_CHOKER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
			
			player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            switch (action)
            {
                case GOSSIP_ACTION_INFO_DEF + 0:
                    player->CLOSE_GOSSIP_MENU();
                    player->CastSpell(player, SPELL_SUMMON_NASS);
                    break;
                case GOSSIP_ACTION_INFO_DEF + 1:
                    player->CLOSE_GOSSIP_MENU();
                    player->CastSpell(player, SPELL_PUSH_ENSORCELLED_CHOKER);
                    break;
            }
            return true;
		}
};

enum Quest12859
{
	SPELL_FIRE_EXTINGUISHER = 55037,
	SPELL_EXPLOSIVE_FIRE    = 46799,
	SPELL_BUILDING_FIRE     = 21650,

	NPC_HUT_FIRE            = 29692,
	QUEST_FIRE_STILL_HOT    = 12859,
};

class npc_hut_fire : public CreatureScript
{
public:
	npc_hut_fire() : CreatureScript("npc_hut_fire") {}

    struct npc_hut_fireAI : public QuantumBasicAI
    {
       npc_hut_fireAI(Creature* creature) : QuantumBasicAI(creature) {}

	   uint32 BurningTimer;

        void Reset()
		{
			BurningTimer = 1000;

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_HOVER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_FIRE_EXTINGUISHER)
            {
				if (caster->ToPlayer()->GetQuestStatus(QUEST_FIRE_STILL_HOT) == QUEST_STATUS_COMPLETE)
					return;

				if (Player* player = caster->ToPlayer())
					caster->ToPlayer()->KilledMonsterCredit(NPC_HUT_FIRE, 0);

				me->DisappearAndDie();
            }
        }

        void UpdateAI(const uint32 diff)
        {                                
			UpdateVictim();

			if (BurningTimer <= diff)
			{
				DoCast(me, SPELL_EXPLOSIVE_FIRE);
				DoCast(me, SPELL_BUILDING_FIRE);
				BurningTimer = 5000;
			}
			else BurningTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hut_fireAI(creature);
    }
};

enum ThrowingDown
{
	SPELL_TOSS_GENADE       = 51381,
	SPELL_SUMMON_SKIMMER    = 51435,

	NPC_CRATER_BUNNY_CREDIT = 28352,

	QUEST_THROWING_DOWN_1   = 12598,
	QUEST_THROWING_DOWN_2   = 12591,
};

class npc_nethurbian_crater_kc_bunny : public CreatureScript
{
public:
    npc_nethurbian_crater_kc_bunny() : CreatureScript("npc_nethurbian_crater_kc_bunny") {}

    struct npc_nethurbian_crater_kc_bunnyAI : public QuantumBasicAI
    {
        npc_nethurbian_crater_kc_bunnyAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_TOSS_GENADE)
            {
				if (caster->ToPlayer()->GetQuestStatus(QUEST_THROWING_DOWN_1) == QUEST_STATUS_COMPLETE || caster->ToPlayer()->GetQuestStatus(QUEST_THROWING_DOWN_2) == QUEST_STATUS_COMPLETE)
					return;

				if (Player* player = caster->ToPlayer())
				{
					caster->ToPlayer()->KilledMonsterCredit(NPC_CRATER_BUNNY_CREDIT, 0);

					me->CastSpell(me, SPELL_SUMMON_SKIMMER, true);
					me->CastSpell(me, SPELL_SUMMON_SKIMMER, true);
					me->CastSpell(me, SPELL_SUMMON_SKIMMER, true);
				}

				me->DisappearAndDie();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_nethurbian_crater_kc_bunnyAI(creature);
    }
};

enum NerubianCocon
{
	SPELL_SUMMON_SCOURGED_CAPTIVE = 51597,
	SPELL_SUMMON_CAPTIVE_FOOTMAN  = 51599,

	NPC_CAPTIVE_DRAKKARI          = 28414,
	NPC_CAPTIVE_FOOTMAN           = 28415,
};

class npc_nerubian_cocoon : public CreatureScript
{
public:
    npc_nerubian_cocoon() : CreatureScript("npc_nerubian_cocoon") {}

    struct npc_nerubian_cocoonAI : public QuantumBasicAI
    {
        npc_nerubian_cocoonAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
		}

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void JustDied(Unit* killer)
		{
			Player* player = killer->ToPlayer();

			switch (urand(0, 1))
			{
	            case 0:
					me->CastSpell(me, SPELL_SUMMON_SCOURGED_CAPTIVE, true);
					break;
				case 1:
					player->KilledMonsterCredit(NPC_CAPTIVE_FOOTMAN, 0);
					me->CastSpell(me, SPELL_SUMMON_CAPTIVE_FOOTMAN, true);
					break;
			}
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_nerubian_cocoonAI(creature);
    }
};

enum QuestDoneToDeath
{
	SPELL_FEIGN_DEATH                = 29266,
	SPELL_FLAMES_COSMETIC            = 39199,
	SPELL_INCINERATE_CORPSE          = 51276,
	SPELL_ARGENT_FOOTMAN_KILL_CREDIT = 51279,

	QUEST_DONE_TO_DEATH              = 12568,
};

class npc_defeated_argent_footman : public CreatureScript
{
public:
    npc_defeated_argent_footman() : CreatureScript("npc_defeated_argent_footman") {}

    struct npc_defeated_argent_footmanAI : public QuantumBasicAI
    {
        npc_defeated_argent_footmanAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_FEIGN_DEATH);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FEIGN_DEATH | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_INCINERATE_CORPSE)
            {
				if (caster->ToPlayer()->GetQuestStatus(QUEST_DONE_TO_DEATH) == QUEST_STATUS_COMPLETE)
					return;

				if (Player* player = caster->ToPlayer())
				{
					me->CastSpell(caster, SPELL_ARGENT_FOOTMAN_KILL_CREDIT, true);
					me->CastSpell(me, SPELL_FLAMES_COSMETIC, true);
				}

				me->DespawnAfterAction(6*IN_MILLISECONDS);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_defeated_argent_footmanAI(creature);
    }
};

enum QuestCanYouDigIt
{
	SPELL_DIG_FOR_TREASURE        = 51333,
	SPELL_ANCIENT_DRAKKARI_CHEST  = 51345,

	NPC_ANCIENT_BUNNY_KILL_CREDIT = 28330,
	GO_ANCIENT_DIRT_MOUND         = 190550,
	QUEST_CAN_YOU_DIG_IT_1        = 12588,
};

class npc_ancient_dirt_kc_bunny : public CreatureScript
{
public:
    npc_ancient_dirt_kc_bunny() : CreatureScript("npc_ancient_dirt_kc_bunny") {}

    struct npc_ancient_dirt_kc_bunnyAI : public QuantumBasicAI
    {
        npc_ancient_dirt_kc_bunnyAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_DIG_FOR_TREASURE)
            {
				if (caster->ToPlayer()->GetQuestStatus(QUEST_CAN_YOU_DIG_IT_1) == QUEST_STATUS_COMPLETE)
					return;

				if (Player* player = caster->ToPlayer())
				{
					caster->ToPlayer()->KilledMonsterCredit(NPC_ANCIENT_BUNNY_KILL_CREDIT, 0);
					me->CastSpell(me, SPELL_ANCIENT_DRAKKARI_CHEST, true);

					if (GameObject* dirt = me->FindGameobjectWithDistance(GO_ANCIENT_DIRT_MOUND, 5.0f))
						dirt->RemoveFromWorld();
				}

				me->DespawnAfterAction();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ancient_dirt_kc_bunnyAI(creature);
    }
};

enum CrusaderParachute
{
	SPELL_CRUSADER_PARACHUTE  = 53031,
	SPELL_DEPLOY_PARACHUTE    = 53039, // Need Implement
	SPELL_TURN_EVIL           = 10326,
	SPELL_CRUSADER_STRIKE     = 14517,
	SPELL_AS_REND             = 11977,
	SPELL_AS_SHIELD_BLOCK     = 32587,

	NPC_PARACHUTE_KILL_CREDIT = 29060,
};

class npc_argent_crusader : public CreatureScript
{
public:
    npc_argent_crusader() : CreatureScript("npc_argent_crusader") {}

    struct npc_argent_crusaderAI : public QuantumBasicAI
    {
        npc_argent_crusaderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CrusaderStrikeTimer;
		uint32 TurnEvilTimer;

        void Reset()
        {
			CrusaderStrikeTimer = 2000;
			TurnEvilTimer = 4000;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_2H);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_CRUSADER_PARACHUTE);
		}

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_CRUSADER_PARACHUTE)
            {
				if (Player* player = caster->ToPlayer())
					caster->ToPlayer()->KilledMonsterCredit(NPC_PARACHUTE_KILL_CREDIT, 0);
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CrusaderStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_CRUSADER_STRIKE);
				CrusaderStrikeTimer = 3000;
			}
			else CrusaderStrikeTimer -= diff;

			if (TurnEvilTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM))
				{
					DoCast(target, SPELL_TURN_EVIL);
					TurnEvilTimer = 5000;
				}
			}
			else TurnEvilTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_argent_crusaderAI(creature);
    }
};

class npc_argent_shieldman : public CreatureScript
{
public:
    npc_argent_shieldman() : CreatureScript("npc_argent_shieldman") {}

    struct npc_argent_shieldmanAI : public QuantumBasicAI
    {
        npc_argent_shieldmanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RendTimer;
		uint32 ShieldBlockTimer;

        void Reset()
        {
			RendTimer = 2000;
			ShieldBlockTimer = 3000;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveAurasDueToSpell(SPELL_CRUSADER_PARACHUTE);
		}

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_CRUSADER_PARACHUTE)
            {
				if (Player* player = caster->ToPlayer())
					caster->ToPlayer()->KilledMonsterCredit(NPC_PARACHUTE_KILL_CREDIT, 0);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (RendTimer <= diff)
			{
				DoCastVictim(SPELL_AS_REND);
				RendTimer = 3000;
			}
			else RendTimer -= diff;

			if (ShieldBlockTimer <= diff)
			{
				DoCast(me, SPELL_AS_SHIELD_BLOCK);
				ShieldBlockTimer = 4000;
			}
			else ShieldBlockTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_argent_shieldmanAI(creature);
    }
};

enum Quest12519
{
	SPELL_CREATE_MEDALLION       = 50737,
	SPELL_CREATE_MEDALLION_COVER = 51026,
	SPELL_DC_FEIGN_DEATH         = 29266,

	QUEST_WHATDYA_WANT_A_MEDAL   = 12519,
};

class npc_drakkari_corpse : public CreatureScript
{
public:
    npc_drakkari_corpse() : CreatureScript("npc_drakkari_corpse") {}

    struct npc_drakkari_corpseAI : public QuantumBasicAI
    {
        npc_drakkari_corpseAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_DC_FEIGN_DEATH);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FEIGN_DEATH);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_CREATE_MEDALLION_COVER)
            {
				if (caster->ToPlayer()->GetQuestStatus(QUEST_WHATDYA_WANT_A_MEDAL) == QUEST_STATUS_COMPLETE)
					return;

				if (Player* player = caster->ToPlayer())
					caster->CastSpell(caster, SPELL_CREATE_MEDALLION, true);

				me->DespawnAfterAction();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_drakkari_corpseAI(creature);
    }
};

enum QuestFeedinDaGoolz
{
	SPELL_FLESH_ROT        = 28913,
	SPELL_FD_DUMMY         = 58178,
	SPELL_SCOURGE_DISGUISE = 51966,

	NPC_GHOUL_KILL_CREDIT  = 28591,
	GO_BOWELS_AND_BRAINS   = 190656,
};

class npc_ghoul_feeding_kc_bunny : public CreatureScript
{
public:
    npc_ghoul_feeding_kc_bunny() : CreatureScript("npc_ghoul_feeding_kc_bunny") {}

    struct npc_ghoul_feeding_kc_bunnyAI : public QuantumBasicAI
    {
        npc_ghoul_feeding_kc_bunnyAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_FD_DUMMY);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
        return new npc_ghoul_feeding_kc_bunnyAI(creature);
    }
};

class npc_decaying_ghoul : public CreatureScript
{
public:
    npc_decaying_ghoul() : CreatureScript("npc_decaying_ghoul") {}

    struct npc_decaying_ghoulAI : public QuantumBasicAI
    {
        npc_decaying_ghoulAI(Creature* creature) : QuantumBasicAI(creature){}

		bool Feeded;

		uint32 FleshRotTimer;

        void Reset()
        {
			FleshRotTimer = 1000;

			Feeded = false;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, 0);
        }

		void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_FD_DUMMY && !Feeded)
            {
				Feeded = true;

				if (Creature* bunny = me->FindCreatureWithDistance(NPC_GHOUL_KILL_CREDIT, 5.0f))
				{
					me->SetFacingToObject(bunny);
					me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_EAT);
					bunny->DespawnAfterAction(7*IN_MILLISECONDS);
				}

				if (Player* player = me->FindPlayerWithDistance(15.0f))
				{
					if (player->HasAura(SPELL_SCOURGE_DISGUISE))
					{
						player->KilledMonsterCredit(NPC_GHOUL_KILL_CREDIT, 0);
						me->DespawnAfterAction(7*IN_MILLISECONDS);
					}
				}
            }
        }

		void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (FleshRotTimer <= diff)
			{
				DoCastVictim(SPELL_FLESH_ROT);
				FleshRotTimer = 4000;
			}
			else FleshRotTimer -= diff;

			DoMeleeAttackIfReady();
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_decaying_ghoulAI(creature);
    }
};

enum GorebagQuests
{
	SPELL_SUMMON_GARGOYLE        = 52194,
	SPELL_CREDIT_FOR_QUEST_12664 = 52220, // dont work
	SPELL_FLIGHT                 = 54422,

	QUEST_REUNTIED               = 12663,
	QUEST_DARK_HORIZON           = 12664,

	NPC_FLYING_FIEND             = 28669,
	NPC_KILL_CREDIT_Q12664       = 28663,

	AREA_VOLTAR                  = 4314,

	GOSSIP_GOREBAG               = 9732,

	EVENT_CHECK_AREA             = 1,
    EVENT_REACHED_HOME           = 2,
};

class npc_flying_fiend_vehicle : public CreatureScript
{
public:
    npc_flying_fiend_vehicle() : CreatureScript("npc_flying_fiend_vehicle") { }

    struct npc_flying_fiend_vehicleAI : public VehicleAI
    {
        npc_flying_fiend_vehicleAI(Creature* creature) : VehicleAI(creature)
		{
			DoCast(me, SPELL_FLIGHT, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        EventMap events;

        void Reset()
        {
            events.ScheduleEvent(EVENT_CHECK_AREA, 1000);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != WAYPOINT_MOTION_TYPE)
                return;

            if (id == 62)
				events.ScheduleEvent(EVENT_REACHED_HOME, 1000);
        }

        void UpdateAI(uint32 const diff)
        {
            events.Update(diff);

            switch (events.ExecuteEvent())
            {
                case EVENT_CHECK_AREA:
                    if (me->GetAreaId() == AREA_VOLTAR)
                    {
                        if (Vehicle* vehicle = me->GetVehicleKit())
						{
                            if (Unit* passenger = vehicle->GetPassenger(0))
                                me->GetMotionMaster()->MovePath(NPC_FLYING_FIEND, false);
						}
					}
					else events.ScheduleEvent(EVENT_CHECK_AREA, 1000);
                    break;
				case EVENT_REACHED_HOME:
                    if (Vehicle* vehicle = me->GetVehicleKit())
					{
						if (Unit* passenger = vehicle->GetPassenger(0))
						{
							if (Player* player = passenger->ToPlayer())
							{
								player->KilledMonsterCredit(NPC_KILL_CREDIT_Q12664, 0);
								player->ExitVehicle();
							}
						}
					}
					break;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_flying_fiend_vehicleAI(creature);
    }
};

class npc_gorebag : public CreatureScript
{
public:
    npc_gorebag() : CreatureScript("npc_gorebag") {}

    struct npc_gorebagAI : public QuantumBasicAI
    {
        npc_gorebagAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
		{
			if (menuId == GOSSIP_GOREBAG && gossipListId == 0)
			{
				player->CastSpell(player, SPELL_SUMMON_GARGOYLE, true);
				player->PlayerTalkClass->SendCloseGossip();
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
        return new npc_gorebagAI(creature);
    }
};

enum CursedOffspringofHarkoa
{
	SPELL_CURSE_OF_BLOOD      = 54604,
	SPELL_SUMMON_HARKOA       = 51891,
	SPELL_WHISKER_OF_HARKOA   = 51895,

	QUEST_BUT_FIRST_OFFSPRING = 12632,
	NPC_RELEASED_HARKOA       = 28526,
};

class npc_cursed_offspring_of_harkoa : public CreatureScript
{
public:
    npc_cursed_offspring_of_harkoa() : CreatureScript("npc_cursed_offspring_of_harkoa") {}

    struct npc_cursed_offspring_of_harkoaAI : public QuantumBasicAI
    {
        npc_cursed_offspring_of_harkoaAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 CurseOfBloodTimer;

        void Reset()
        {
			CurseOfBloodTimer = 1000;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_WHISKER_OF_HARKOA)
            {
				if (Player* player = caster->ToPlayer())
				{
					if (player->GetQuestStatus(QUEST_BUT_FIRST_OFFSPRING) == QUEST_STATUS_INCOMPLETE)
					{
						player->CastSpell(player, SPELL_SUMMON_HARKOA, true);
						player->KilledMonsterCredit(NPC_RELEASED_HARKOA, 0);
						me->DespawnAfterAction();
					}
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CurseOfBloodTimer <= diff)
			{
				DoCastVictim(SPELL_CURSE_OF_BLOOD);
				CurseOfBloodTimer = 4000;
			}
			else CurseOfBloodTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_cursed_offspring_of_harkoaAI(creature);
    }
};

enum QuestFuelForTheFire
{
	SPELL_FLATULATE               = 52497,
	SPELL_CHIEFTAIN_CREDIT        = 52620,
	SPELL_BURST_AT_THE_SEANS_VIS  = 52510,
	SPELL_BURST_AT_THE_SEANS_KILL = 52508,
	SPELL_BURST_AT_THE_SEANS_BONE = 52516,

	QUEST_FUEL_FOR_THE_FIRE       = 12690,
	NPC_KILL_CREDIT_CRUSHER       = 29099,
	NPC_BLOATED_ABOMINATION       = 28843,
};

class npc_drakkari_skullcrusher : public CreatureScript
{
public:
    npc_drakkari_skullcrusher() : CreatureScript("npc_drakkari_skullcrusher") {}

    struct npc_drakkari_skullcrusherAI : public QuantumBasicAI
    {
        npc_drakkari_skullcrusherAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 KillTimer;

		bool Boom;

        void Reset()
        {
			Boom = false;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_FLATULATE && !Boom)
            {
				if (Player* player = me->FindPlayerWithDistance(125.0f, true))
				{
					if (player->GetQuestStatus(QUEST_FUEL_FOR_THE_FIRE) == QUEST_STATUS_INCOMPLETE)
					{
						Boom = true;

						DoCast(me, SPELL_BURST_AT_THE_SEANS_VIS, true);

						if (Creature* abomination = me->FindCreatureWithDistance(NPC_BLOATED_ABOMINATION, 125.0f, true))
							me->Kill(abomination);

						KillTimer = 500;

						player->KilledMonsterCredit(NPC_KILL_CREDIT_CRUSHER, 0);
					}
				}
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (KillTimer <= diff && Boom)
            {
				DoCast(me, SPELL_BURST_AT_THE_SEANS_BONE, true);
				DoCast(me, SPELL_BURST_AT_THE_SEANS_KILL, true);
				KillTimer = 9999999;
            }
            else KillTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_drakkari_skullcrusherAI(creature);
    }
};

class npc_drakkari_chieftain : public CreatureScript
{
public:
    npc_drakkari_chieftain() : CreatureScript("npc_drakkari_chieftain") {}

    struct npc_drakkari_chieftainAI : public QuantumBasicAI
    {
        npc_drakkari_chieftainAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 KillTimer;

		bool Boom;

        void Reset()
        {
			Boom = false;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void SpellHit(Unit* caster, const SpellInfo* spell)
        {
			if (spell->Id == SPELL_FLATULATE && !Boom)
            {
				if (Player* player = me->FindPlayerWithDistance(125.0f, true))
				{
					if (player->GetQuestStatus(QUEST_FUEL_FOR_THE_FIRE) == QUEST_STATUS_INCOMPLETE)
					{
						Boom = true;

						DoCast(me, SPELL_BURST_AT_THE_SEANS_VIS, true);

						KillTimer = 500;

						if (Creature* abomination = me->FindCreatureWithDistance(NPC_BLOATED_ABOMINATION, 125.0f, true))
							me->Kill(abomination);

						player->CastSpell(player, SPELL_CHIEFTAIN_CREDIT, true);
					}
                }
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (KillTimer <= diff && Boom)
            {
				DoCast(me, SPELL_BURST_AT_THE_SEANS_BONE, true);
				DoCast(me, SPELL_BURST_AT_THE_SEANS_KILL, true);
				KillTimer = 9999999;
            }
            else KillTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_drakkari_chieftainAI(creature);
    }
};

void AddSC_zuldrak()
{
    new npc_drakuru_shackles();
    new npc_captured_rageclaw();
    new npc_gymer();
    new npc_gurgthock();
	new npc_vladof_the_butcher();
    new npc_orinoko_tuskbreaker();
    new npc_korrak_bloodrager();
    new npc_yggdras();
    new npc_stinkbeard();
    new npc_released_offspring_harkoa();
    new npc_crusade_recruit();
    new npc_elemental_lord();
    new npc_fiend_elemental();
    new npc_captain_brandon_zuldrak();
    new npc_alchemist_finklestein();
    new npc_argent_crusade_we_are_leaving();
    new go_scourge_enclosure();
    new npc_bloodrose_datura();
    new npc_nass();
    new npc_commander_kunz();
	new npc_plague_sprayer;
	new npc_stefan_vadu();
	new npc_hut_fire();
	new npc_nethurbian_crater_kc_bunny();
	new npc_nerubian_cocoon();
	new npc_defeated_argent_footman();
	new npc_ancient_dirt_kc_bunny();
	new npc_argent_crusader();
	new npc_argent_shieldman();
	new npc_drakkari_corpse();
	new npc_ghoul_feeding_kc_bunny();
	new npc_decaying_ghoul();
	new npc_flying_fiend_vehicle();
	new npc_gorebag();
	new npc_cursed_offspring_of_harkoa();
	new npc_drakkari_skullcrusher();
	new npc_drakkari_chieftain();
}