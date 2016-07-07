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
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "QuantumEscortAI.h"
#include "Vehicle.h"
#include "CombatAI.h"
#include "Player.h"

enum WarGate
{
    QUEST_RETURN_TO_AG_A = 12499,
    QUEST_RETURN_TO_AG_H = 12500,

    MOVIE_ID_GATES       = 14,
};

#define GOSSIP_ITEM_WHAT_HAPPENED "Alexstrasza, can you show me what happened here?"

class npc_alexstrasza_wr_gate : public CreatureScript
{
public:
    npc_alexstrasza_wr_gate() : CreatureScript("npc_alexstrasza_wr_gate") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestRewardStatus(QUEST_RETURN_TO_AG_A) || player->GetQuestRewardStatus(QUEST_RETURN_TO_AG_H))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_WHAT_HAPPENED, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* /*creature*/, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {
            player->CLOSE_GOSSIP_MENU();
            player->SendMovieStart(MOVIE_ID_GATES);
        }
        return true;
    }
};

enum DenouncementTarget
{
	SPELL_COMPELLED                          = 48714,
	SPELL_DENOUNCEMENT_JORDAN_KILL_CREDIT    = 48723,
	SPELL_DENOUNCEMENT_ZIERHUT_KILL_CREDIT   = 48725,
	SPELL_DENOUNCEMENT_GOODMAN_KILL_CREDIT   = 48727,
	SPELL_DENOUNCEMENT_MERCER_KILL_CREDIT    = 48729,
	SPELL_JORDAN_WRATH                       = 50837,
	SPELL_JORDAN_CONSECRATION                = 32773,
	SPELL_ZIERHUT_TORCHE                     = 50832,
	SPELL_GOODMAN_CRUSHARMOR                 = 33661,
	SPELL_GOODMAN_CRACK                      = 15621,
	SPELL_MERCER_SUMMON_WARHORSE             = 50829,

	ENTRY_JORDAN                             = 27237,
	ENTRY_ZIERHUT                            = 27235,
	ENTRY_GOODMAN                            = 27234,
	ENTRY_MERCER                             = 27236,
};

class npc_denouncement_target : public CreatureScript
{
public:
    npc_denouncement_target() : CreatureScript("npc_denouncement_target") { }

    struct npc_denouncement_targetAI : public QuantumBasicAI
    {
        npc_denouncement_targetAI(Creature* creature) : QuantumBasicAI(creature) { }

        uint32 SpellTimer1;
        uint32 SpellTimer2;

        void Reset()
        {
            me->RemoveAurasDueToSpell(SPELL_COMPELLED);

            switch (me->GetEntry())
            {
            case ENTRY_JORDAN:
                SpellTimer1 = 10000;
                SpellTimer2 = 15000;
                break;
            case ENTRY_ZIERHUT:
                SpellTimer1 = 10000;
                break;
            case ENTRY_GOODMAN:
                SpellTimer1 = 10000;
                SpellTimer2 = 15000;
                break;
            case ENTRY_MERCER:
                SpellTimer1 = 5000;
                break;
            }
        }

        void JustDied(Unit* killer)
        {
            if (killer)
            {
                if (me->HasAuraEffect(SPELL_COMPELLED, 0))
                {
                    Unit* target = killer->GetCharmerOrOwnerOrSelf();

                    if (target->GetTypeId()== TYPE_ID_PLAYER)
                    {
                        switch (me->GetEntry())
                        {
                            case ENTRY_JORDAN:
								target->CastSpell(target, SPELL_DENOUNCEMENT_JORDAN_KILL_CREDIT, true);
								break;
							case ENTRY_ZIERHUT:
								target->CastSpell(target, SPELL_DENOUNCEMENT_ZIERHUT_KILL_CREDIT, true);
								break;
							case ENTRY_GOODMAN:
								target->CastSpell(target, SPELL_DENOUNCEMENT_GOODMAN_KILL_CREDIT, true);
								break;
							case ENTRY_MERCER:
								target->CastSpell(target, SPELL_DENOUNCEMENT_MERCER_KILL_CREDIT, true);
								break;
                        }
                    }
                }
            }
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (SpellTimer1)
            if (SpellTimer1 <= diff)
            {
                switch (me->GetEntry())
                {
                case ENTRY_JORDAN:
                    DoCastVictim(SPELL_JORDAN_WRATH);
                    SpellTimer1 = 10000;
                    break;
                case ENTRY_ZIERHUT:
                    DoCastVictim(SPELL_ZIERHUT_TORCHE);
                    SpellTimer1 = 10000;
                    break;
                case ENTRY_GOODMAN:
                    DoCastVictim(SPELL_GOODMAN_CRUSHARMOR);
                    SpellTimer1 = 10000;
                    break;
                case ENTRY_MERCER:
                    DoCastVictim(SPELL_MERCER_SUMMON_WARHORSE);
                    SpellTimer1 = 0;
                    break;
                }

            }
			else SpellTimer1 -= diff;

            if (SpellTimer2)
			if (SpellTimer2 <= diff)
            {
                switch (me->GetEntry())
                {
                case ENTRY_JORDAN:
                    DoCastVictim(SPELL_JORDAN_CONSECRATION);
                    SpellTimer2 = 15000;
                    break;
                case ENTRY_GOODMAN:
                    DoCastVictim(SPELL_GOODMAN_CRACK);
                    SpellTimer2 = 15000;
                    break;
                }
            }
			else SpellTimer2 -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new  npc_denouncement_targetAI(creature);
    }
};

/*######
## vehicle_forsaken_blight_spreader
######*/

enum ForsakenAreas
{
	AREA_VENOMSPITE        = 4186,
	AREA_VENEGANCE_PASS    = 4232,
	AREA_CARRION_FIELDS    = 4188,
	AREA_DRAGONBLIGHT      = 65,
};

class vehicle_forsaken_blight_spreader : public CreatureScript
{
public:
    vehicle_forsaken_blight_spreader() : CreatureScript("vehicle_forsaken_blight_spreader") { }

    struct vehicle_forsaken_blight_spreaderAI : public VehicleAI
    {
        vehicle_forsaken_blight_spreaderAI(Creature* creature) : VehicleAI(creature) {}

        uint32 CheckTimer;
        bool isInUse;

        void Reset()
        {
            CheckTimer = 5000;
        }

        void OnCharmed(bool apply)
        {
            isInUse = apply;

            if (!apply)
                CheckTimer = 30000;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!me->IsVehicle())
                return;

            if (isInUse)
            {
                if (CheckTimer < diff)
                {
                    uint32 area = me->GetAreaId();
                    switch (area)
                    {
                        case AREA_VENOMSPITE:
                        case AREA_VENEGANCE_PASS:
                        case AREA_CARRION_FIELDS:
                        case AREA_DRAGONBLIGHT:
							break;
						default:
							me->DealDamage(me, me->GetHealth());
							break;
                    }

                    CheckTimer = 5000;
                }
				else CheckTimer -= diff;
            }
			else
            {
                if (CheckTimer < diff)
                {
                    uint32 area = me->GetAreaId();
                    if (area != AREA_VENOMSPITE)
                    {
                        me->DealDamage(me, me->GetHealth());
						CheckTimer = 5000;
					}
                }
				else CheckTimer -= diff;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new vehicle_forsaken_blight_spreaderAI(creature);
    }
};

enum BattleStandart
{
	SPELL_SUMMON_ANUBAR_INVADER = 47303,
	NPC_ANUBAR_INVADER          = 26676,
	QUEST_MIGHT_OF_HORDE        = 12053,
};

class npc_warsong_battle_standard : public CreatureScript
{
public:
    npc_warsong_battle_standard() : CreatureScript("npc_warsong_battle_standard") { }

    struct npc_warsong_battle_standardAI : public QuantumBasicAI
    {
        npc_warsong_battle_standardAI(Creature* creature) : QuantumBasicAI(creature)
        {
            Part = 1;
            PartTimer = 5000;

			SetCombatMovement(false);
        }

        uint32 PartTimer;
        uint32 Part;

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (Part > 4 || me->IsDead())
                return;

            if (PartTimer <= diff)
            {
                switch (Part)
                {
                case 1:
                    DoCast(me, SPELL_SUMMON_ANUBAR_INVADER, true);
                    Part++;
                    break;
                case 2:
                    DoCast(me, SPELL_SUMMON_ANUBAR_INVADER, true);
                    DoCast(me, SPELL_SUMMON_ANUBAR_INVADER, true);
                    Part++;
                    break;
                case 3:
                    DoCast(me, SPELL_SUMMON_ANUBAR_INVADER, true);
                    DoCast(me, SPELL_SUMMON_ANUBAR_INVADER, true);
                    DoCast(me, SPELL_SUMMON_ANUBAR_INVADER, true);
                    Part++;
                    break;
                case 4:
                    Unit* owner = me->GetCharmerOrOwnerOrSelf();
                    if (owner && owner->GetTypeId() == TYPE_ID_PLAYER)
                        owner->ToPlayer()->CompleteQuest(QUEST_MIGHT_OF_HORDE);
                    Part++;
                    break;
                }
                PartTimer = 58000;
            }
			else PartTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new  npc_warsong_battle_standardAI(creature);
    }
};

enum EmissaryBrighthoof
{
	QUEST_BLOOD_OATH_HORDE = 11983,
};

class npc_emissary_brighthoof : public CreatureScript
{
public:
    npc_emissary_brighthoof() : CreatureScript("npc_emissary_brighthoof") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestRewardStatus(QUEST_BLOOD_OATH_HORDE) && player->getQuestStatusMap()[QUEST_BLOOD_OATH_HORDE].CreatureOrGOCount[0] >= 5)
            player->CompleteQuest(QUEST_BLOOD_OATH_HORDE);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }
};

enum WintergardeMineBomb
{
	SPELL_WMB_EXPLOSION = 48742,

	NPC_WINTERGARDE_MINE_SHAFT       = 27436,
	NPC_LOWER_WINTERGARDE_MINE_SHAFT = 27437,
};

class npc_wintergarde_mine_bomb : public CreatureScript
{
public:
    npc_wintergarde_mine_bomb() : CreatureScript("npc_wintergarde_mine_bomb") { }

    struct npc_wintergarde_mine_bombAI : public QuantumBasicAI
    {
        npc_wintergarde_mine_bombAI(Creature* creature) : QuantumBasicAI(creature)
        {
			ExploteTimer = 13000;

			SetCombatMovement(false);
        }

        uint32 ExploteTimer;

        void Reset()
        {
            me->SetReactState(REACT_PASSIVE);
        }

        void UpdateAI(const uint32 diff)
        {
            if (ExploteTimer <= diff)
            {
                Unit* owner = me->GetCharmerOrOwnerOrSelf();

                Creature* target = me->FindCreatureWithDistance(NPC_WINTERGARDE_MINE_SHAFT, 10.0f, true);
                if (target && owner->ToPlayer())
                    owner->ToPlayer()->KilledMonsterCredit(NPC_WINTERGARDE_MINE_SHAFT, 0);

                target = me->FindCreatureWithDistance(NPC_LOWER_WINTERGARDE_MINE_SHAFT, 10.0f, true);
                if (target && owner->ToPlayer())
                    owner->ToPlayer()->KilledMonsterCredit(NPC_LOWER_WINTERGARDE_MINE_SHAFT, 0);

                DoCast(SPELL_WMB_EXPLOSION);
                ExploteTimer = 9999999;
            }
			else ExploteTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new  npc_wintergarde_mine_bombAI(creature);
    }
};

float DevoutBodyguardWay[1][21][3] =
{
	{{2790.324f, -490.081f, 119.616f},
	{2792.576f, -483.441f, 119.616f},
	{2801.183f, -478.036f, 119.616f},
	{2814.852f, -465.822f, 119.613f},
	{2818.68f, -469.324f, 119.614f},
	{2821.951f, -471.869f, 123.61f},
	{2828.52f, -472.507f, 129.532f},
	{2834.976f, -469.976f, 135.162f},
	{2840.456f, -460.922f, 135.362f},
	{2838.088f, -453.457f, 135.362f},
	{2831.782f, -448.045f, 135.362f},
	{2823.282f, -447.724f, 135.362f},
	{2816.99f, -452.967f, 135.362f},
	{2814.536f, -460.418f, 135.362f},
	{2816.803f, -468.241f, 135.361f},
	{2818.536f, -469.547f, 135.355f},
	{2823.803f, -472.822f, 141.208f},
	{2831.668f, -471.995f, 148.345f},
	{2834.233f, -470.502f, 150.673f},
	{2837.822f, -467.322f, 150.836f},
	{2832.710f, -462.756f, 150.835f}},
};

class npc_devout_bodyguard : public CreatureScript
{
public:
    npc_devout_bodyguard() : CreatureScript("npc_devout_bodyguard") { }

    struct npc_devout_bodyguardAI: public QuantumBasicAI
    {
		npc_devout_bodyguardAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 waypoint;
		uint32 WaitTimer;
		bool isMoving;

		void Reset()
		{
			me->SetWalk(false);
			isMoving = false;
			waypoint = 0;
			WaitTimer = 1;
		}

		void EnterToBattle(Unit* /*who*/){}

		void StartMove()
		{
			me->SetWalk(true);
			WaitTimer = 1;
			isMoving = true;
		}

		void MovementInform(uint32, uint32)
		{
			if (waypoint == 20)
			{
				isMoving = false;
				me->DisappearAndDie();
			}
			WaitTimer = 1;
		}

		void UpdateAI(const uint32 diff)
		{
			if (WaitTimer == 1)
			{
				me->GetMotionMaster()->Clear();
				if (isMoving)
					me->GetMotionMaster()->MovePoint(0, DevoutBodyguardWay[0][waypoint][0], DevoutBodyguardWay[0][waypoint][1], DevoutBodyguardWay[0][waypoint][2]);
				++waypoint;
				WaitTimer = 0;
			}

			if (!UpdateVictim())
				return;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_devout_bodyguardAI(creature);
    }
};

enum HighAbbotLandgren
{
    QUEST_A_FALL_FROM_GRACE                 = 12274,
    SPELL_SCARLET_RAVEN_PRIEST_IMAGE_FEMALE = 48761,
    SPELL_SCARLET_RAVEN_PRIEST_IMAGE_MALE   = 48763,
    TEXT_1                                  = 12848,
    TEXT_2                                  = 12849,
    NPC_HIGH_ABBOT_LANDGREN                 = 27245,
    NPC_HIGH_ABBOT_LANDGREN_ESCORTEE        = 27439,
    NPC_DEVOUT_BODYGUARD                    = 27247,
};

float HighAbbotLandgrenSpawnPos[1][1][4] = {2828.981934f, -439.187012f, 119.624001f, 1.647600f};
float HighAbbotLandgrenJumpPos[1][1][3] = {2719.657f, -554.755f, 23.988f};

static const char* HighAbbotText[] =
{
	"I am ready, your grace. <kiss the ring>",
	"I know a place nearby where we could speak in private, my child. Follow me.",
	"Did you think that i could not see through your flimsy disguise, $N.",
	"There is much that you do not understand, $R. The Master sees all.",
	"He told me that you would come for me. I won't die by your hand, through. I have seen what you have done to my compatriots.",
	"No i will leave this world in a manner of my own choosing. And i will return, the grand admiral's permitting.",
	"AAAEEEEIIIiiiiiiiiiiiiiiiiiiiiiiiiiiii........................................"
};

class npc_high_abbot_landgren : public CreatureScript
{
public:
    npc_high_abbot_landgren() : CreatureScript("npc_high_abbot_landgren") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (creature->GetEntry() == NPC_HIGH_ABBOT_LANDGREN && player->GetQuestStatus(QUEST_A_FALL_FROM_GRACE) == QUEST_STATUS_INCOMPLETE && player->getQuestStatusMap()[QUEST_A_FALL_FROM_GRACE].CreatureOrGOCount[0] == 1 && (player->HasAura(SPELL_SCARLET_RAVEN_PRIEST_IMAGE_MALE) || player->HasAura(SPELL_SCARLET_RAVEN_PRIEST_IMAGE_FEMALE)))
        {
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, HighAbbotText[0], GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(TEXT_1, creature->GetGUID());
            return true;
        }

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
                player->SEND_GOSSIP_MENU(TEXT_2, creature->GetGUID());
                CAST_AI(npc_high_abbot_landgren::npc_high_abbot_landgrenAI, creature->AI())->StartEvent();
                break;
        }
        return true;
    }

    struct npc_high_abbot_landgrenAI : public npc_escortAI
    {
        npc_high_abbot_landgrenAI(Creature* creature) : npc_escortAI(creature) { }

        bool BodyGuardStart;
        bool CheckPlayerDist;
        bool EventStarted;
        bool EndSequence;

		float Range;

        uint8 GuardCount;
        uint8 EndSayCount;

        uint32 AuraCheckTimer;
        uint32 BodyGuardMoveTimer;
        uint32 EndSequenceTimer;

		uint64 PlayerGUID;
        void WaypointReached(uint32 id)
        {
            if (me->GetEntry() != NPC_HIGH_ABBOT_LANDGREN_ESCORTEE)
                return;

            if (PlayerGUID)
			{
				if (Player* player = Unit::GetPlayer(*me, PlayerGUID))
                {
                    switch (id)
                    {
                        case 0:
                            me->MonsterSay(HighAbbotText[1], LANG_UNIVERSAL, 0);
                            break;
                        case 12:
                            EndSequence = true;
                            break;
                    }
				}
			}
		}

        void Reset()
        {
            BodyGuardStart = true;
            CheckPlayerDist = false;
            EventStarted = false;
            EndSequence = false;
            GuardCount = 0;
            EndSayCount = 2;
            AuraCheckTimer = 300;
            BodyGuardMoveTimer = 2000;
            EndSequenceTimer = 500;
            Range = 3.0f;
            PlayerGUID = 0;
        }

        void StartEvent()
        {
            CheckPlayerDist = true;
        }

        void StartMove()
        {
            if ( Creature* tmp = me->SummonCreature(NPC_HIGH_ABBOT_LANDGREN_ESCORTEE, HighAbbotLandgrenSpawnPos[0][0][0], HighAbbotLandgrenSpawnPos[0][0][1], HighAbbotLandgrenSpawnPos[0][0][2], HighAbbotLandgrenSpawnPos[0][0][3]))
            {
                CAST_AI(npc_high_abbot_landgren::npc_high_abbot_landgrenAI, tmp->AI())->PlayerGUID = PlayerGUID;
                me->DespawnAfterAction();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (PlayerGUID)
            {
                Player* AuraPlayer = Unit::GetPlayer(*me,PlayerGUID);

                if (!AuraPlayer)
                {
                    me->DisappearAndDie();
                    return;
                }
                else
                {
                    if (AuraPlayer->IsDead())
                    {
                        AuraPlayer->FailQuest(QUEST_A_FALL_FROM_GRACE);
                        me->DisappearAndDie();
                        return;
                    }

                    if (EndSequence)
                    {
                        if (EndSequenceTimer <= diff)
                        {
                            EndSequenceTimer = 12000;
                            if (EndSayCount == 6)
                            {
                                me->GetMotionMaster()->MoveJump(HighAbbotLandgrenJumpPos[0][0][0], HighAbbotLandgrenJumpPos[0][0][1], HighAbbotLandgrenJumpPos[0][0][2], 0.5f, 8.0f);
                                EndSequenceTimer = 2000;
                            }
                            if (EndSayCount == 7)
                            {
                                me->MonsterSay(HighAbbotText[6], LANG_UNIVERSAL, 0);
                                EndSequenceTimer = 2000;
                            }
                            if (EndSayCount == 8)
                            {
                                AuraPlayer->KilledMonsterCredit(27444, 0);
                                if (AuraPlayer->HasAura(SPELL_SCARLET_RAVEN_PRIEST_IMAGE_FEMALE))
                                    AuraPlayer->RemoveAura(SPELL_SCARLET_RAVEN_PRIEST_IMAGE_FEMALE);
                                if (AuraPlayer->HasAura(SPELL_SCARLET_RAVEN_PRIEST_IMAGE_MALE))
                                    AuraPlayer->RemoveAura(SPELL_SCARLET_RAVEN_PRIEST_IMAGE_MALE);
                                EndSequence = false;
                            }

                            if (EndSayCount < 6)
                                me->MonsterSay(HighAbbotText[EndSayCount], LANG_UNIVERSAL, PlayerGUID);

                            EndSayCount++;
                        }
						else EndSequenceTimer -= diff;
                    }

                    if (!EventStarted && me->GetEntry() == NPC_HIGH_ABBOT_LANDGREN_ESCORTEE)
                    {
                        Start(false, false, PlayerGUID, 0, false);
                        EventStarted = true;
                    }

                    if (CheckPlayerDist)
                        if (AuraPlayer->GetDistance(2827.796f, -420.191f, 118.196f) < 4)
                            StartMove();

                    if (AuraCheckTimer <= diff)
					{
                        if (AuraPlayer && AuraPlayer->GetQuestStatus(QUEST_A_FALL_FROM_GRACE) == QUEST_STATUS_INCOMPLETE && AuraPlayer->getQuestStatusMap()[QUEST_A_FALL_FROM_GRACE].CreatureOrGOCount[0] == 1 && !AuraPlayer->HasAura(SPELL_SCARLET_RAVEN_PRIEST_IMAGE_MALE) && !AuraPlayer->HasAura(SPELL_SCARLET_RAVEN_PRIEST_IMAGE_FEMALE))
						{
                            switch (AuraPlayer->GetCurrentGender())
                            {
                                case GENDER_FEMALE:
									AuraPlayer->CastSpell(AuraPlayer, SPELL_SCARLET_RAVEN_PRIEST_IMAGE_FEMALE, false);
									break;
								case GENDER_MALE:
									AuraPlayer->CastSpell(AuraPlayer, SPELL_SCARLET_RAVEN_PRIEST_IMAGE_MALE, false);
									break;
							}
                        }
						AuraCheckTimer = 300;
                    }
					else AuraCheckTimer -= diff;

                    if (me->GetEntry() != NPC_HIGH_ABBOT_LANDGREN_ESCORTEE)
                    {
                        if (BodyGuardStart && AuraPlayer->GetQuestStatus(QUEST_A_FALL_FROM_GRACE) == QUEST_STATUS_INCOMPLETE && AuraPlayer->getQuestStatusMap()[QUEST_A_FALL_FROM_GRACE].CreatureOrGOCount[0] == 1) {
                            if (Creature* tmp = me->FindCreatureWithDistance(NPC_DEVOUT_BODYGUARD, Range, true))
                            {
                                if (BodyGuardMoveTimer <= diff)
                                {
                                    CAST_AI(npc_devout_bodyguard::npc_devout_bodyguardAI, tmp->AI())->StartMove();
                                    BodyGuardMoveTimer = 6000;
                                    Range = 4.0f;
                                    if (GuardCount == 1)
                                        BodyGuardStart = false;
									GuardCount++;
                                }
								else BodyGuardMoveTimer -= diff;
                            }
                        }
                    }
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_high_abbot_landgrenAI(creature);
    }
};

class AFallFromGracePlayerScript : public PlayerScript
{
public:
    AFallFromGracePlayerScript() : PlayerScript("AFallFromGracePlayerScript") { }

    void OnLogin(Player* player)
    {
        if (player->GetQuestStatus(QUEST_A_FALL_FROM_GRACE) == QUEST_STATUS_INCOMPLETE)
            player->FailQuest(QUEST_A_FALL_FROM_GRACE);
    }
};

enum LegionEngineer
{
	ENTRY_ENGENEER                      = 27163,
	ENTRY_SCOURGE_PLAGE_CATAPULT        = 27607,
	ENTRY_PLAGUE_PULT_CREDIT            = 27625,
	SPELL_PLACE_SCOURGE_DISCOMBOBULATER = 49114,
	SPELL_DESTURCTION                   = 49215,
	SPELL_DESTURCTION_TRIGGER           = 49218,
};

class npc_7th_legion_siege_engineer : public CreatureScript
{
public:
    npc_7th_legion_siege_engineer() : CreatureScript("npc_7th_legion_siege_engineer") { }

    struct npc_7th_legion_siege_engineerAI : public QuantumBasicAI
    {
        npc_7th_legion_siege_engineerAI(Creature* creature) : QuantumBasicAI (creature) { }

        uint64 GuidOwner;
        uint64 GuidPult;

        uint32 CheckTimer;
        uint32 phase;

        void Reset()
        {
            GuidOwner = 0;
            GuidPult = 0;
            phase = 0; //0 - Check for PlageCatapult, 1 - MoveToPult, 2 - Summon Object, 3 - Give Credit
            CheckTimer = 2000;
        }

        void SpellHitTarget(Unit* target, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_DESTURCTION_TRIGGER)
            {
                if (Player* owner = Unit::GetPlayer(*me, GuidOwner))
                    owner->KilledMonsterCredit(ENTRY_PLAGUE_PULT_CREDIT, 0);

                if (target->ToCreature())
                    target->DealDamage(target, target->GetHealth());
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (GuidOwner > 0)
                return;

            if (me->GetDistance2d(who) > 20)
                return;

            if (who->GetTypeId() == TYPE_ID_UNIT)
            {
                if (who->IsVehicle() && who->GetCharmer() && who->GetCharmer()->ToPlayer())
                    GuidOwner = who->GetCharmer()->ToPlayer()->GetGUID();
            }

            if (who->GetTypeId() == TYPE_ID_PLAYER)
                GuidOwner = who->ToPlayer()->GetGUID();
        }

        void AttackStart(Unit* /*attacker*/) {}

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id != 1)
                return;

            phase = 2;
            CheckTimer = 2000;
        }

        void UpdateAI(const uint32 diff)
        {
            if (CheckTimer <= diff)
            {
                switch (phase)
                {
                case 0:
                    if (Unit::GetPlayer(*me, GuidOwner))
                    {
                        Creature* pult = me->FindCreatureWithDistance(ENTRY_SCOURGE_PLAGE_CATAPULT, 50.0f);
                        if (pult)
                        {
                            GuidPult = pult->GetGUID();
                            me->GetMotionMaster()->MovePoint(1, pult->GetPositionX(), pult->GetPositionY(), pult->GetPositionZ());
                            phase = 1;
                        }
                    }
                    else
						me->DealDamage(me, me->GetHealth());
                    break;
                case 1:
                    return;
                case 2:
                    if (Creature* pult = Creature::GetCreature(*me,GuidPult))
                    {
                        me->CastSpell(pult,SPELL_DESTURCTION,true);
                        me->CastSpell(me,SPELL_PLACE_SCOURGE_DISCOMBOBULATER,false);
                        phase++;
                    }
                    break;
                }

            }
			else CheckTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_7th_legion_siege_engineerAI(creature);
    }
};

enum SteamAreas
{
	AREA_CARRION_FIELDS_1     = 4188,
	AREA_WINTERGARD_MAUSOLEUM = 4246,
	AREA_THORSONS_POINT       = 4190,
};

class vehicle_alliance_steamtank : public CreatureScript
{
public:
    vehicle_alliance_steamtank() : CreatureScript("vehicle_alliance_steamtank") { }

    struct vehicle_alliance_steamtankAI : public VehicleAI
    {
        vehicle_alliance_steamtankAI(Creature* creature) : VehicleAI(creature) { }

        uint32 CheckTimer;
        bool isInUse;

        void Reset()
        {
            CheckTimer = 5000;
        }

        void OnCharmed(bool apply)
        {
            isInUse = apply;

            if (!apply)
                CheckTimer = 30000;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!me->IsVehicle())
                return;

            if (isInUse)
            {
                if (CheckTimer < diff)
                {
                    uint32 area = me->GetAreaId();
                    switch (area)
                    {
                    case AREA_CARRION_FIELDS_1:
                    case AREA_WINTERGARD_MAUSOLEUM:
                    case AREA_THORSONS_POINT:
                        break;
                    default:
                        me->DealDamage(me, me->GetHealth());
                        break;
                    }

                    CheckTimer = 5000;
                }
				else CheckTimer -= diff;
            }
			else
            {
                if (CheckTimer < diff)
                {
                    uint32 area = me->GetAreaId();
                    if (area != AREA_THORSONS_POINT)
                    {
                        me->DealDamage(me, me->GetHealth());
                    }
                    CheckTimer = 5000;
                }
				else CheckTimer -= diff;
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new  vehicle_alliance_steamtankAI(creature);
    }
};

/*######
## npc_agent_skully
######*/

const char* AgentSkullySay[] =
{
	"Please wait $N, someone else is doing the Task! Come back later and try again!"
};

class npc_agent_skully : public CreatureScript
{
public:
    npc_agent_skully() : CreatureScript("npc_agent_skully") { }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
    {
        if (quest->GetQuestId() == QUEST_A_FALL_FROM_GRACE)
        {
            if (player->GetCurrentGender() == GENDER_FEMALE)
                creature->CastSpell(player, SPELL_SCARLET_RAVEN_PRIEST_IMAGE_FEMALE, false);

            if (player->GetCurrentGender() == GENDER_MALE)
                creature->CastSpell(player, SPELL_SCARLET_RAVEN_PRIEST_IMAGE_MALE, false);

            if (Creature* AIcreature = creature->FindCreatureWithDistance(NPC_HIGH_ABBOT_LANDGREN, 30.0f))
            {
                if (CAST_AI(npc_high_abbot_landgren::npc_high_abbot_landgrenAI, AIcreature->AI())->PlayerGUID == 0)
                    CAST_AI(npc_high_abbot_landgren::npc_high_abbot_landgrenAI, AIcreature->AI())->PlayerGUID = player->GetGUID();
                else
                {
                    if (Player* player = Unit::GetPlayer(*creature, CAST_AI(npc_high_abbot_landgren::npc_high_abbot_landgrenAI, creature->AI())->PlayerGUID))
                    {
                        if (!player->IsInWorld() || player->GetAreaId() != 4180 || player->GetQuestStatus(QUEST_A_FALL_FROM_GRACE) != QUEST_STATUS_INCOMPLETE)
                            CAST_AI(npc_high_abbot_landgren::npc_high_abbot_landgrenAI, AIcreature->AI())->PlayerGUID = player->GetGUID();
                        else
                        {
                            creature->MonsterSay(AgentSkullySay[0], LANG_UNIVERSAL, player->GetGUID());
                            player->FailQuest(QUEST_A_FALL_FROM_GRACE);
                        }
                    }
                }
            }
            else
            {
                creature->MonsterSay(AgentSkullySay[0], LANG_UNIVERSAL, player->GetGUID());
                player->FailQuest(QUEST_A_FALL_FROM_GRACE);
            }
        }
        return true;
    }
};

enum WoodlandsWalker
{
    QUEST_ANCIENTS_ALLINACE  = 12092,
    QUEST_ANCIENTS_HORDE     = 12096,
};

//This function is called when the player opens the gossip menubool

class npc_woodlands_walker : public CreatureScript
{
public:
    npc_woodlands_walker() : CreatureScript("npc_woodlands_walker") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_ANCIENTS_ALLINACE) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_ANCIENTS_HORDE) == QUEST_STATUS_INCOMPLETE)
            creature->SetCurrentFaction(16);

        player->CLOSE_GOSSIP_MENU();
        return true;
    }
};

enum QuestInfiniteMysteryRedux
{
    NPC_INFINITE_ASSAILANT          = 27896,
    NPC_INFINITE_DESTROYER          = 27897,
	NPC_INFINITE_CHRONO_MAGUS       = 27898,
    NPC_INFINITE_TIMERENDER_1       = 27900,
	NPC_INFINITE_TIMERENDER_2       = 32352,

	SPELL_MYSTERY_SOUND             = 50057,
	SPELL_AURA_OF_HOURGLASS         = 50867,
	SPELL_INFINITE_ASSAILANT        = 49900,
	SPELL_INFINITE_DESTROYER        = 49901,
	SPELL_INFINITE_CHRONO_MAGUS     = 49902,
	SPELL_INFINITE_TIMERENDER_1     = 49905,
	SPELL_INFINITE_TIMERENDER_2     = 60887,
	SPELL_IT_TIME_LAPSE             = 51020,

	QUEST_MYSTERY_OF_THE_INFINITE_1 = 12470,
	QUEST_MYSTERY_OF_THE_INFINITE_2 = 13343,
};

class npc_hourglass_of_eternity : public CreatureScript
{
public:
    npc_hourglass_of_eternity() : CreatureScript("npc_hourglass_of_eternity") { }

    struct npc_hourglass_of_eternityAI : public QuantumBasicAI
    {
		npc_hourglass_of_eternityAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			SetCombatMovement(false);
		}

        uint64 WaveTimer;
        uint32 WaveCounter;

		SummonList Summons;

        void Reset()
        {
			DoCast(me, SPELL_AURA_OF_HOURGLASS, true);
			DoCast(me, SPELL_MYSTERY_SOUND, true);

            WaveTimer = 5*IN_MILLISECONDS;

            WaveCounter = 0;

			Summons.DespawnAll();
        }

		void JustDied(Unit* /*killer*/)
        {
			if (TempSummon* summon = me->ToTempSummon())
			{
				if (Unit* summoner = summon->GetSummoner())
				{
					summoner->ToPlayer()->FailQuest(QUEST_MYSTERY_OF_THE_INFINITE_1);
					summoner->ToPlayer()->FailQuest(QUEST_MYSTERY_OF_THE_INFINITE_2);
				}
            }

			Summons.DespawnAll();
        }

		void JustSummoned(Creature* summon)
		{
			switch (summon->GetEntry())
			{
			    case NPC_INFINITE_CHRONO_MAGUS:
				case NPC_INFINITE_ASSAILANT:
				case NPC_INFINITE_DESTROYER:
				case NPC_INFINITE_TIMERENDER_1:
				case NPC_INFINITE_TIMERENDER_2:
					summon->AddThreat(me, 20.0f);
					summon->AI()->AttackStart(me);
					Summons.Summon(summon);
					break;
				default:
					break;
			}
		}

        void SummonWave()
		{
			switch (WaveCounter)
			{
		        case 0:
					DoCast(me, SPELL_INFINITE_ASSAILANT);
					DoCast(me, SPELL_INFINITE_DESTROYER);
					DoCast(me, SPELL_INFINITE_CHRONO_MAGUS);
					break;
				case 1:
					DoCast(me, SPELL_INFINITE_DESTROYER);
					DoCast(me, SPELL_INFINITE_CHRONO_MAGUS);
					DoCast(me, SPELL_INFINITE_CHRONO_MAGUS);
					break;
				case 2:
					DoCast(me, SPELL_INFINITE_ASSAILANT);
					DoCast(me, SPELL_INFINITE_DESTROYER);
					DoCast(me, SPELL_INFINITE_DESTROYER);
					break;
				case 3:
					DoCast(me, SPELL_INFINITE_ASSAILANT);
					DoCast(me, SPELL_INFINITE_DESTROYER);
					DoCast(me, SPELL_INFINITE_CHRONO_MAGUS);
					break;
				case 4:
					DoCast(me, SPELL_INFINITE_TIMERENDER_1);
					break;
				case 5:
					DoCast(me, SPELL_INFINITE_TIMERENDER_2);
					break;
			}
		}

        void UpdateAI(const uint32 diff)
        {
            if (WaveTimer <= diff)
            {
                if (WaveCounter <= 5)
                {
                    SummonWave();
                    WaveTimer = 15*IN_MILLISECONDS;
                    WaveCounter++;
                }
				else me->DespawnAfterAction();
			}
			else WaveTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_hourglass_of_eternityAI(creature);
    }
};

class npc_infinite_timerender : public CreatureScript
{
public:
    npc_infinite_timerender() : CreatureScript("npc_infinite_timerender") {}

    struct npc_infinite_timerenderAI : public QuantumBasicAI
    {
        npc_infinite_timerenderAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 TimeLapseTimer;

        void Reset()
		{
			TimeLapseTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void JustDied(Unit* killer)
		{
			Player* player = killer->ToPlayer();

			switch (me->GetEntry())
			{
			    case NPC_INFINITE_TIMERENDER_1:
					player->AreaExploredOrEventHappens(QUEST_MYSTERY_OF_THE_INFINITE_1);
					break;
				case NPC_INFINITE_TIMERENDER_2:
					player->AreaExploredOrEventHappens(QUEST_MYSTERY_OF_THE_INFINITE_2);
					break;
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TimeLapseTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_IT_TIME_LAPSE);
					TimeLapseTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else TimeLapseTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_infinite_timerenderAI(creature);
    }
};

#define GOSSIP_ROANAUK1 "Greetings, High Chief. Would you do me the honor of accepting my invitation to join the horde as an official member and leader of the Taunka ?."
#define GOSSIP_ROANAUK2 "It is you who honor me, High Chief. Please read form this scroll. It is the oath of alegiance."

enum Roanauk
{
	QUEST_ALL_HAIL_ROANAUK = 12140,
	NPC_ROANAUK            = 26810,
};

class npc_roanauk : public CreatureScript
{
public:
    npc_roanauk() : CreatureScript("npc_roanauk") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        if (player->GetQuestStatus(QUEST_ALL_HAIL_ROANAUK) == QUEST_STATUS_INCOMPLETE)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ROANAUK1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

        player->SEND_GOSSIP_MENU(268100, creature->GetGUID());
		return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        switch (action)
        {
        case GOSSIP_ACTION_INFO_DEF:
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ROANAUK1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            player->SEND_GOSSIP_MENU(268100, creature->GetGUID());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            player->SEND_GOSSIP_MENU(268101, creature->GetGUID());
            player->KilledMonsterCredit(NPC_ROANAUK, creature->GetGUID());
        }
        return true;
    }
};

enum NoMercyForTheCaptured
{
    NPC_DEATHGUARD_SCHNEIDER        = 27376,
    NPC_SENIOR_SCRIVENER_BARRIGA    = 27378,
    NPC_ENGINEER_BURKE              = 27379,
    NPC_CHANCELLOR_AMAI             = 27381,
    QUEST_NO_MERCY_FOR_THE_CAPTURED = 12245,
};

#define GOSSIP_PRISONERS "What do you mean my time has come? I'll kill you where you stand!"

class npc_no_mercy_for_the_captured : public CreatureScript
{
public:
    npc_no_mercy_for_the_captured() : CreatureScript("npc_no_mercy_for_the_captured") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_NO_MERCY_FOR_THE_CAPTURED) == QUEST_STATUS_INCOMPLETE)
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_PRISONERS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF+1)
        {            
            player->CLOSE_GOSSIP_MENU();
            creature->SetCurrentFaction(16);
            creature->AI()->AttackStart(player);
        }
        return true;
    }

    struct npc_no_mercy_for_the_capturedAI : public QuantumBasicAI
    {
        npc_no_mercy_for_the_capturedAI(Creature* creature) : QuantumBasicAI(creature) 
        { 
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
        }

        void Reset()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING);
            me->RestoreFaction();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_no_mercy_for_the_capturedAI(creature);
    }
};

enum StrengthenAncientsMisc
{
    SAY_WALKER_FRIENDLY = 0,
    SAY_WALKER_ENEMY = 1,
    SAY_LOTHALOR = 0,

    SPELL_CREATE_ITEM_BARK = 47550,
    SPELL_CONFUSED = 47044,

    NPC_LOTHALOR = 26321,

    FACTION_WALKER_ENEMY = 14,
};

class spell_q12096_q12092_dummy : public SpellScriptLoader // Strengthen the Ancients: On Interact Dummy to Woodlands Walker
{
public:
    spell_q12096_q12092_dummy() : SpellScriptLoader("spell_q12096_q12092_dummy") { }

    class spell_q12096_q12092_dummy_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q12096_q12092_dummy_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            uint32 roll = rand() % 2;

            Creature* tree = GetHitCreature();
            Player* player = GetCaster()->ToPlayer();

            if (!tree || !player)
                return;

            tree->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELL_CLICK);

            if (roll == 1) // friendly version
            {
                tree->CastSpell(player, SPELL_CREATE_ITEM_BARK);
                tree->AI()->Talk(SAY_WALKER_FRIENDLY, player->GetGUID());
                tree->DespawnAfterAction(1000);
            }
            else if (roll == 0) // enemy version
            {
                tree->AI()->Talk(SAY_WALKER_ENEMY, player->GetGUID());
                tree->SetCurrentFaction(FACTION_WALKER_ENEMY);
                tree->Attack(player, true);
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_q12096_q12092_dummy_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q12096_q12092_dummy_SpellScript();
    }
};

class spell_q12096_q12092_bark : public SpellScriptLoader // Bark of the Walkers
{
public:
    spell_q12096_q12092_bark() : SpellScriptLoader("spell_q12096_q12092_bark") { }

    class spell_q12096_q12092_bark_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q12096_q12092_bark_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Creature* lothalor = GetHitCreature();
            if (!lothalor || lothalor->GetEntry() != NPC_LOTHALOR)
                return;

            lothalor->AI()->Talk(SAY_LOTHALOR);
            lothalor->RemoveAura(SPELL_CONFUSED);
            lothalor->DespawnAfterAction(4000);
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_q12096_q12092_bark_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q12096_q12092_bark_SpellScript();
    }
};

enum Slim
{
	QUEST_ITEM_SLIM    = 36765,
	QUEST_SLIM_PICKING = 12075,
};

class npc_slim_giant : public CreatureScript
{
public:
	npc_slim_giant() : CreatureScript("npc_slim_giant") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (player->GetQuestStatus(QUEST_SLIM_PICKING) == QUEST_STATUS_INCOMPLETE)
		{
			player->PlayerTalkClass->ClearMenus();
			player->AddItem(QUEST_ITEM_SLIM, 1);
		}
		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
		return true;
	}
};

enum WyrmDefenderEnum
{
    // Quest data
    QUEST_DEFENDING_WYRMREST_TEMPLE       = 12372,
    GOSSIP_TEXTID_DEF1                    = 12899,
    // Gossip data
    GOSSIP_TEXTID_DEF2                    = 12900,
    // Spells data
    SPELL_CHARACTER_SCRIPT                = 49213,
    SPELL_DEFENDER_ON_LOW_HEALTH_EMOTE    = 52421, // ID - 52421 Wyrmrest Defender: On Low Health Boss Emote to Controller - Random /self/
    SPELL_RENEW                           = 49263, // casted to heal drakes
    SPELL_WYRMREST_DEFENDER_MOUNT         = 49256,
    // Texts data
    WHISPER_MOUNTED                      = 0,
    BOSS_EMOTE_ON_LOW_HEALTH             = 2,
};

#define GOSSIP_ITEM_1 "We need to get into the fight. Are you ready?"

class npc_wyrmrest_defender : public CreatureScript
{
    public:
        npc_wyrmrest_defender() : CreatureScript("npc_wyrmrest_defender") { }

        bool OnGossipHello(Player* player, Creature* creature)
        {
            if (player->GetQuestStatus(QUEST_DEFENDING_WYRMREST_TEMPLE) == QUEST_STATUS_INCOMPLETE)
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEF1, creature->GetGUID());
            }
            else
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            if (action == GOSSIP_ACTION_INFO_DEF+1)
            {
                player->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEF2, creature->GetGUID());
                player->CastSpell(player, SPELL_CHARACTER_SCRIPT, true);
            }

            return true;
        }

        struct npc_wyrmrest_defenderAI : public VehicleAI
        {
            npc_wyrmrest_defenderAI(Creature* creature) : VehicleAI(creature) { }

            bool hpWarningReady;
            bool renewRecoveryCanCheck;

            uint32 RenewRecoveryChecker;

            void Reset()
            {
                hpWarningReady = true;
                renewRecoveryCanCheck = false;

                RenewRecoveryChecker = 0;
            }

			void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                switch (spell->Id)
                {
                    case SPELL_WYRMREST_DEFENDER_MOUNT:
                        Talk(WHISPER_MOUNTED, me->GetCharmerOrOwnerGUID());
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
                        break;
                    // Both below are for checking low hp warning
                    case SPELL_DEFENDER_ON_LOW_HEALTH_EMOTE:
                        Talk(BOSS_EMOTE_ON_LOW_HEALTH, me->GetCharmerOrOwnerGUID());
                        break;
                    case SPELL_RENEW:
                        if (!hpWarningReady && RenewRecoveryChecker <= 100)
                        {
                            RenewRecoveryChecker = 20000;
                        }
                        renewRecoveryCanCheck = true;
                        break;
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (hpWarningReady && me->GetHealthPct() <= 30.0f)
                {
                    me->CastSpell(me, SPELL_DEFENDER_ON_LOW_HEALTH_EMOTE);
                    hpWarningReady = false;
                }

                if (renewRecoveryCanCheck)
                {
                    if (RenewRecoveryChecker <= diff)
                    {
                        renewRecoveryCanCheck = false;
                        hpWarningReady = true;
                    }
                    else RenewRecoveryChecker -= diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_wyrmrest_defenderAI(creature);
        }
};

enum HulkingJormungar
{
	SPELL_POTENT_EXPLOSIVE_CHARGE = 47305,
	SPELL_CORROSIVE_POISON        = 50293,
	SPELL_JORMUNGAR_MEAT_SPAWNER  = 47311,
	EMOTE_OPENING_MOUTH           = 0,
};

class npc_hulking_jormungar : public CreatureScript
{
public:
    npc_hulking_jormungar() : CreatureScript("npc_hulking_jormungar") { }

    struct npc_hulking_jormungarAI : public QuantumBasicAI
    {
        npc_hulking_jormungarAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 CorrosivePoisonTimer;

		void Reset()
		{
			CorrosivePoisonTimer = 4000;
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_POTENT_EXPLOSIVE_CHARGE)
			{
				Talk(EMOTE_OPENING_MOUTH);
				DoCast(me, SPELL_JORMUNGAR_MEAT_SPAWNER);
				caster->Kill(me);
			}
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CorrosivePoisonTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM))
				{
					DoCast(target, SPELL_CORROSIVE_POISON);
					CorrosivePoisonTimer = 6000;
				}
			}
			else CorrosivePoisonTimer -= diff;

            DoMeleeAttackIfReady();
        }
	};

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hulking_jormungarAI(creature);
    }
};

enum RubyKeeper
{
	SPELL_EMBER_FLAME_BURNING_CORPSE = 49132,
	SPELL_FALLING_DRAGON_FEIGN_DEATH = 55795,
	SPELL_STUN_PERMAMENT             = 61204,
	SPELL_EMBER_FLAME_STRAFE_SPAWN   = 49209,
	SPELL_PLANTING_RUBY_ACORN        = 49349,
	NPC_RUBY_KEEPER_KILL_CREDIT      = 27530,
};

class npc_ruby_keeper : public CreatureScript
{
public:
    npc_ruby_keeper() : CreatureScript("npc_ruby_keeper") {}

    struct npc_ruby_keeperAI : public QuantumBasicAI
    {
        npc_ruby_keeperAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_EMBER_FLAME_BURNING_CORPSE, true);
			DoCast(me, SPELL_FALLING_DRAGON_FEIGN_DEATH, true);
			DoCast(me, SPELL_STUN_PERMAMENT, true);
        }

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_PLANTING_RUBY_ACORN)
			{
				if (Player* player = caster->ToPlayer())
					caster->ToPlayer()->KilledMonsterCredit(NPC_RUBY_KEEPER_KILL_CREDIT, 0);

				me->RemoveAurasDueToSpell(SPELL_EMBER_FLAME_BURNING_CORPSE);
				me->DespawnAfterAction();
			}
		}

        void UpdateAI(const uint32 diff){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ruby_keeperAI(creature);
    }
};

enum ReanimatedFrostWyrm
{
	SPELL_HOVER_ANIM_OVERRIDE   = 57764,
	SPELL_FROST_BREATH          = 47425,
	SPELL_SEEDS_OF_NATURE_WRATH = 49587,

	NPC_WEAKENED_FROST_WYRM     = 27821,
};

class npc_reanimated_frost_wyrm : public CreatureScript
{
public:
    npc_reanimated_frost_wyrm() : CreatureScript("npc_reanimated_frost_wyrm") {}

    struct npc_reanimated_frost_wyrmAI : public QuantumBasicAI
    {
        npc_reanimated_frost_wyrmAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 FrostBreathTimer;

        void Reset()
        {
			DoCast(me, SPELL_HOVER_ANIM_OVERRIDE);

			FrostBreathTimer = 2*IN_MILLISECONDS;
        }

		void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_SEEDS_OF_NATURE_WRATH)
				me->UpdateEntry(NPC_WEAKENED_FROST_WYRM);
		}

		void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostBreathTimer <= diff)
			{
				DoCastAOE(SPELL_FROST_BREATH);
				FrostBreathTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else FrostBreathTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_reanimated_frost_wyrmAI(creature);
    }
};

enum OverseerDeathgaze
{
	SPELL_CONVERSION_BEAM = 50659,
	SPELL_OD_SHADOW_BOLT  = 12739,

	NPC_WEAKENED_OVERSEER = 27807,
};

class npc_overseer_deathgaze : public CreatureScript
{
public:
    npc_overseer_deathgaze() : CreatureScript("npc_overseer_deathgaze") {}

    struct npc_overseer_deathgazeAI : public QuantumBasicAI
    {
        npc_overseer_deathgazeAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ConversionBeamTimer;
		uint32 ShadowBoltTimer;

        void Reset()
        {
			ConversionBeamTimer = 3000;
			ShadowBoltTimer = 4000;
        }

		void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_SEEDS_OF_NATURE_WRATH)
				me->UpdateEntry(NPC_WEAKENED_OVERSEER);
		}

		void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ConversionBeamTimer <= diff)
			{
				DoCastVictim(SPELL_CONVERSION_BEAM);
				ConversionBeamTimer = urand(4000, 5000);
			}
			else ConversionBeamTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				DoCastVictim(SPELL_OD_SHADOW_BOLT);
				ShadowBoltTimer = urand(6000, 7000);
			}
			else ShadowBoltTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_overseer_deathgazeAI(creature);
    }
};

enum TurgidTheVile
{
	SPELL_SCOURGE_HOOK  = 50335,
	SPELL_VILE_VOMIT    = 51356,
	NPC_WEAKENED_TURGID = 27809,
};

class npc_turgid_the_vile : public CreatureScript
{
public:
    npc_turgid_the_vile() : CreatureScript("npc_turgid_the_vile") {}

    struct npc_turgid_the_vileAI : public QuantumBasicAI
    {
        npc_turgid_the_vileAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 VileVomitTimer;

        void Reset()
        {
			VileVomitTimer = 3000;
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(SPELL_SCOURGE_HOOK);
		}

		void SpellHit(Unit* caster, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_SEEDS_OF_NATURE_WRATH)
				me->UpdateEntry(NPC_WEAKENED_TURGID);
		}

		void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (VileVomitTimer <= diff)
			{
				DoCastVictim(SPELL_VILE_VOMIT);
				VileVomitTimer = urand(4000, 5000);
			}
			else VileVomitTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_turgid_the_vileAI(creature);
    }
};

enum RoanaukIcemist
{
	SPELL_PERMANENT_FEIGN_DEATH    = 29266,
	SPELL_ICEMIST_PRISON           = 47273,
	SPELL_GLORY_OF_ANCESTORS       = 47378,
	SPELL_ICEMISTS_BLESSING        = 47379,
	SPELL_SUMMON_ANUBARS_INVADER   = 47303,
	SPELL_COSMETIC_CHAINS          = 55009,

	NPC_ANUBAR_PRISONER            = 23981,
	NPC_ROANAUK_ICEMIST            = 26654,
	NPC_ANUBAR_PRISON              = 26656,
	NPC_ICEMIST_WARRIOR            = 26772,

	GO_ANUBETKHAN_CARAPACE         = 188462,
};

class npc_roanauk_icemist : public CreatureScript
{
public:
    npc_roanauk_icemist() : CreatureScript("npc_roanauk_icemist") {}

    struct npc_roanauk_icemistAI : public QuantumBasicAI
    {
        npc_roanauk_icemistAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_KNEEL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
        return new npc_roanauk_icemistAI(creature);
    }
};

class npc_anubar_prison : public CreatureScript
{
public:
    npc_anubar_prison() : CreatureScript("npc_anubar_prison") {}

    struct npc_anubar_prisonAI : public QuantumBasicAI
    {
        npc_anubar_prisonAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ChainsTimer;

        void Reset()
		{
			ChainsTimer = 500;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (ChainsTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* roanauk = me->FindCreatureWithDistance(NPC_ROANAUK_ICEMIST, 50.0f, true))
				{
					DoCast(roanauk, SPELL_COSMETIC_CHAINS, true);
					ChainsTimer = 15*MINUTE*IN_MILLISECONDS;
				}
			}
			else ChainsTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_anubar_prisonAI(creature);
    }
};

enum BeachfrontProperty_TheForgottenTale_Quests
{
	SPELL_FORGOTTEN_PEASANT_CREDIT   = 48829,
	SPELL_FORGOTTEN_RIFLEMAN_CREDIT  = 48830,
	SPELL_FORGOTTEN_KNIGHT_CREDIT    = 48831,
	SPELL_FORGOTTEN_CAPTAIN_CREDIT   = 48832,
	SPELL_FORGOTTEN_AURA             = 48143,
	SPELL_WINTERGLADE_INVISIBILITY_B = 48357,
	SPELL_FK_THROW                   = 38556,
	SPELL_FR_SHOOT                   = 15547,
	SPELL_FR_CONCUSSIVE_SHOT         = 17174,
	SPELL_FP_BONK                    = 51601,
	SPELL_FO_SHIELD_BLOCK            = 32587,

	QUEST_THE_FORGOTTEN_TALE         = 12291,

	NPC_KILL_CREDIT_FORGOTTEN_KNIGHT = 27220,

	FORGOTTEN_KNIGHT_MOUNT_ID        = 2410,
};

#define GOSSIP_MENU_KNIGHT   "I must be going now, soldier. Stay vigilant!"
#define GOSSIP_MENU_RIFLEMAN "I'm sure Arthas will be back any day now, soldier. Keep your chin up!"
#define GOSSIP_MENU_PEASANT  "Sorry to have bothered you, friend. Carry on!"
#define GOSSIP_MENU_FOOTMAN  "I'm sure everything will work out, footman."

class npc_forgotten_knight : public CreatureScript
{
public:
    npc_forgotten_knight() : CreatureScript("npc_forgotten_knight") {}

	bool OnGossipHello(Player* player, Creature* creature)
    {
		if (player->GetQuestStatus(QUEST_THE_FORGOTTEN_TALE) == QUEST_STATUS_INCOMPLETE)
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_MENU_KNIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

		if (action == GOSSIP_ACTION_INFO_DEF)
        {
            player->CLOSE_GOSSIP_MENU();
			player->CastSpell(player, SPELL_FORGOTTEN_KNIGHT_CREDIT, true);
			creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }
        return true;
    }

    struct npc_forgotten_knightAI : public QuantumBasicAI
    {
        npc_forgotten_knightAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ResetGossipTimer;
		uint32 ThrowTimer;

        void Reset()
        {
			ResetGossipTimer = 100;
			ThrowTimer = 500;

			DoCast(me, SPELL_FORGOTTEN_AURA, true);
			DoCast(me, SPELL_WINTERGLADE_INVISIBILITY_B, true);

			me->Mount(FORGOTTEN_KNIGHT_MOUNT_ID);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveMount();

			me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->KilledMonsterCredit(NPC_KILL_CREDIT_FORGOTTEN_KNIGHT, 0);
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (ResetGossipTimer <= diff && me->IsInCombatActive())
			{
				me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
				ResetGossipTimer = 60000; // 1 minutes
			}
			else ResetGossipTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ThrowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FK_THROW);
					ThrowTimer = urand(3000, 4000);
				}
			}
			else ThrowTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_forgotten_knightAI(creature);
    }
};

class npc_forgotten_rifleman : public CreatureScript
{
public:
    npc_forgotten_rifleman() : CreatureScript("npc_forgotten_rifleman") {}

	bool OnGossipHello(Player* player, Creature* creature)
    {
		if (player->GetQuestStatus(QUEST_THE_FORGOTTEN_TALE) == QUEST_STATUS_INCOMPLETE)
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_MENU_RIFLEMAN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

		if (action == GOSSIP_ACTION_INFO_DEF)
        {
            player->CLOSE_GOSSIP_MENU();
			player->CastSpell(player, SPELL_FORGOTTEN_RIFLEMAN_CREDIT, true);
			creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }
        return true;
    }

    struct npc_forgotten_riflemanAI : public QuantumBasicAI
    {
        npc_forgotten_riflemanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ResetGossipTimer;
		uint32 ShootTimer;
		uint32 ConcussiveShotTimer;

        void Reset()
        {
			ResetGossipTimer = 100;
			ShootTimer = 500;
			ConcussiveShotTimer = 1000;

			DoCast(me, SPELL_FORGOTTEN_AURA, true);
			DoCast(me, SPELL_WINTERGLADE_INVISIBILITY_B, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->KilledMonsterCredit(NPC_KILL_CREDIT_FORGOTTEN_KNIGHT, 0);
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (ResetGossipTimer <= diff && me->IsInCombatActive())
			{
				me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
				ResetGossipTimer = 60000; // 1 minutes
			}
			else ResetGossipTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShootTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FR_SHOOT);
					ShootTimer = urand(2000, 3000);
				}
			}
			else ShootTimer -= diff;

			if (ConcussiveShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FR_CONCUSSIVE_SHOT);
					ConcussiveShotTimer = urand(5000, 6000);
				}
			}
			else ConcussiveShotTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_forgotten_riflemanAI(creature);
    }
};

class npc_forgotten_peasant : public CreatureScript
{
public:
    npc_forgotten_peasant() : CreatureScript("npc_forgotten_peasant") {}

	bool OnGossipHello(Player* player, Creature* creature)
    {
		if (player->GetQuestStatus(QUEST_THE_FORGOTTEN_TALE) == QUEST_STATUS_INCOMPLETE)
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_MENU_PEASANT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

		if (action == GOSSIP_ACTION_INFO_DEF)
        {
            player->CLOSE_GOSSIP_MENU();
			player->CastSpell(player, SPELL_FORGOTTEN_PEASANT_CREDIT, true);
			creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }
        return true;
    }

    struct npc_forgotten_peasantAI : public QuantumBasicAI
    {
        npc_forgotten_peasantAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ResetGossipTimer;
		uint32 BonkTimer;

        void Reset()
        {
			ResetGossipTimer = 100;
			BonkTimer = 1000;

			DoCast(me, SPELL_FORGOTTEN_AURA, true);
			DoCast(me, SPELL_WINTERGLADE_INVISIBILITY_B, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->KilledMonsterCredit(NPC_KILL_CREDIT_FORGOTTEN_KNIGHT, 0);
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (ResetGossipTimer <= diff && me->IsInCombatActive())
			{
				me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
				ResetGossipTimer = 60000; // 1 minutes
			}
			else ResetGossipTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BonkTimer <= diff)
			{
				DoCastVictim(SPELL_FP_BONK);
				BonkTimer = urand(4000, 5000);
			}
			else BonkTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_forgotten_peasantAI(creature);
    }
};

class npc_forgotten_footman : public CreatureScript
{
public:
    npc_forgotten_footman() : CreatureScript("npc_forgotten_footman") {}

	bool OnGossipHello(Player* player, Creature* creature)
    {
		if (player->GetQuestStatus(QUEST_THE_FORGOTTEN_TALE) == QUEST_STATUS_INCOMPLETE)
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_MENU_FOOTMAN, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);

		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

		if (action == GOSSIP_ACTION_INFO_DEF)
        {
            player->CLOSE_GOSSIP_MENU();
			player->CastSpell(player, SPELL_FORGOTTEN_CAPTAIN_CREDIT, true);
			creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }
        return true;
    }

    struct npc_forgotten_footmanAI : public QuantumBasicAI
    {
        npc_forgotten_footmanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 ResetGossipTimer;
		uint32 ShieldBlockTimer;

        void Reset()
        {
			ResetGossipTimer = 100;
			ShieldBlockTimer = 500;

			DoCast(me, SPELL_FORGOTTEN_AURA, true);
			DoCast(me, SPELL_WINTERGLADE_INVISIBILITY_B, true);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
		}

		void JustDied(Unit* killer)
		{
			if (Player* player = killer->ToPlayer())
				player->KilledMonsterCredit(NPC_KILL_CREDIT_FORGOTTEN_KNIGHT, 0);
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (ResetGossipTimer <= diff && me->IsInCombatActive())
			{
				me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
				ResetGossipTimer = 60000; // 1 minutes
			}
			else ResetGossipTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShieldBlockTimer <= diff)
			{
				DoCast(me, SPELL_FO_SHIELD_BLOCK);
				ShieldBlockTimer = 5000;
			}
			else ShieldBlockTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_forgotten_footmanAI(creature);
    }
};

void AddSC_dragonblight()
{
    new npc_alexstrasza_wr_gate();
    new npc_denouncement_target();
    new vehicle_forsaken_blight_spreader();
    new npc_warsong_battle_standard();
    new npc_emissary_brighthoof();
    new npc_wintergarde_mine_bomb();
    new npc_devout_bodyguard();
    new npc_high_abbot_landgren();
    new npc_agent_skully();
    new AFallFromGracePlayerScript();
    new npc_7th_legion_siege_engineer();
    new vehicle_alliance_steamtank();
    new npc_woodlands_walker();
	new npc_hourglass_of_eternity();
	new npc_infinite_timerender();
	new npc_roanauk();
	new npc_no_mercy_for_the_captured();
	new spell_q12096_q12092_dummy();
	new spell_q12096_q12092_bark();
	new npc_slim_giant();
	new npc_wyrmrest_defender();
	new npc_hulking_jormungar();
	new npc_ruby_keeper();
	new npc_reanimated_frost_wyrm();
	new npc_overseer_deathgaze();
	new npc_turgid_the_vile();
	new npc_roanauk_icemist();
	new npc_anubar_prison();
	new npc_forgotten_knight();
	new npc_forgotten_rifleman();
	new npc_forgotten_peasant();
	new npc_forgotten_footman();
}