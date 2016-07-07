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
#include "Vehicle.h"
#include "ObjectMgr.h"
#include "QuantumEscortAI.h"
#include "CombatAI.h"
#include "PassiveAI.h"
#include "Player.h"
#include "SpellInfo.h"
#include "CreatureTextMgr.h"
#include "MoveSplineInit.h"

enum QuestTheEmblazonedRuneblade
{
	SPELL_EMBLAZON_RUNEBLADE = 51769,
	SPELL_SHADOW_STORM       = 51738,
	SPELL_ROTATE             = 51767,

	NPC_RUNEBLADED_SWORD     = 28476,
};

class npc_runeforge_se : public CreatureScript
{
public:
    npc_runeforge_se() : CreatureScript("npc_runeforge_se") {}

    struct npc_runeforge_seAI : public QuantumBasicAI
    {
		npc_runeforge_seAI(Creature* creature) : QuantumBasicAI(creature) {}

        void SpellHit(Unit* caster, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_EMBLAZON_RUNEBLADE)
            {
				if (Player* player = caster->ToPlayer())
				{
					DoCast(me, SPELL_SHADOW_STORM, true);
					caster->CastSpell(caster, SPELL_SHADOW_STORM, true);
					me->SummonCreature(NPC_RUNEBLADED_SWORD, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 20*IN_MILLISECONDS);
				}
			}
        }

		void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_runeforge_seAI(creature);
    }
};

class npc_runebladed_sword : public CreatureScript
{
public:
    npc_runebladed_sword() : CreatureScript("npc_runebladed_sword") {}

    struct npc_runebladed_swordAI : public QuantumBasicAI
    {
		npc_runebladed_swordAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset()
        {
			DoCast(me, SPELL_SHADOW_STORM, true);
			DoCast(me, SPELL_ROTATE, true);
        }

		void UpdateAI(const uint32 /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_runebladed_swordAI(creature);
    }
};

enum DeathKnightSpells
{
    SPELL_SOUL_PRISON_CHAIN_SELF = 54612,
    SPELL_SOUL_PRISON_CHAIN      = 54613,
    SPELL_DK_INITIATE_VISUAL     = 51519,
    SPELL_ICY_TOUCH              = 52372,
    SPELL_PLAGUE_STRIKE          = 52373,
    SPELL_BLOOD_STRIKE           = 52374,
    SPELL_DEATH_COIL             = 52375,
};

enum Events
{
	EVENT_ICY_TOUCH              = 1,
	EVENT_PLAGUE_STRIKE          = 2,
	EVENT_BLOOD_STRIKE           = 3,
	EVENT_DEATH_COIL             = 4,
};

//used by 29519, 29520, 29565, 29566, 29567 but signed for 29519
int32 say_event_start[8] =
{
    -1609000,
	-1609001,
	-1609002,
	-1609003,
    -1609004,
	-1609005,
	-1609006,
	-1609007,
};

int32 say_event_attack[9] =
{
    -1609008,
	-1609009,
	-1609010,
	-1609011,
	-1609012,
    -1609013,
	-1609014,
	-1609015,
	-1609016,
};

uint32 acherus_soul_prison[12] =
{
    191577,
    191580,
    191581,
    191582,
    191583,
    191584,
    191585,
    191586,
    191587,
    191588,
    191589,
    191590,
};

uint32 acherus_unworthy_initiate[5] =
{
    29519,
    29520,
    29565,
    29566,
    29567,
};

enum UnworthyInitiatePhase
{
    PHASE_CHAINED,
    PHASE_TO_EQUIP,
    PHASE_EQUIPING,
    PHASE_TO_ATTACK,
    PHASE_ATTACKING,
};

class npc_unworthy_initiate : public CreatureScript
{
public:
    npc_unworthy_initiate() : CreatureScript("npc_unworthy_initiate") { }

    struct npc_unworthy_initiateAI : public QuantumBasicAI
    {
        npc_unworthy_initiateAI(Creature* creature) : QuantumBasicAI(creature)
        {
            me->SetReactState(REACT_PASSIVE);

            if (!me->GetEquipmentId())
			{
                if (const CreatureTemplate* info = sObjectMgr->GetCreatureTemplate(28406))
				{
                    if (info->equipmentId)
                        const_cast<CreatureTemplate*>(me->GetCreatureTemplate())->equipmentId = info->equipmentId;
				}
			}
        }

        uint64 playerGUID;
        UnworthyInitiatePhase phase;
        uint32 WaitTimer;
        float anchorX, anchorY;
        uint64 anchorGUID;

        EventMap events;

        void Reset()
        {
            anchorGUID = 0;
            phase = PHASE_CHAINED;
            events.Reset();
            me->SetCurrentFaction(189);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_KNEEL);
            me->LoadEquipment(0, true);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            events.ScheduleEvent(EVENT_ICY_TOUCH, 1000, 1);
            events.ScheduleEvent(EVENT_PLAGUE_STRIKE, 3000, 1);
            events.ScheduleEvent(EVENT_BLOOD_STRIKE, 2000, 1);
            events.ScheduleEvent(EVENT_DEATH_COIL, 5000, 1);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == 1)
            {
                WaitTimer = 5000;
                me->CastSpell(me, SPELL_DK_INITIATE_VISUAL, true);

                if (Player* starter = Unit::GetPlayer(*me, playerGUID))
                    DoSendQuantumText(say_event_attack[rand()%9], me, starter);

                phase = PHASE_TO_ATTACK;
            }
        }

        void EventStart(Creature* anchor, Player* target)
        {
            WaitTimer = 5000;
            phase = PHASE_TO_EQUIP;

            me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
            me->RemoveAurasDueToSpell(SPELL_SOUL_PRISON_CHAIN_SELF);
            me->RemoveAurasDueToSpell(SPELL_SOUL_PRISON_CHAIN);

            float z;
            anchor->GetContactPoint(me, anchorX, anchorY, z, 1.0f);
            playerGUID = target->GetGUID();
            DoSendQuantumText(say_event_start[rand()%8], me, target);
        }

        void UpdateAI(const uint32 diff)
        {
            switch (phase)
            {
            case PHASE_CHAINED:
                if (!anchorGUID)
                {
                    if (Creature* anchor = me->FindCreatureWithDistance(29521, 30.0f))
                    {
                        anchor->AI()->SetGUID(me->GetGUID());
                        anchor->CastSpell(me, SPELL_SOUL_PRISON_CHAIN, true);
                        anchorGUID = anchor->GetGUID();
                    }
                    else
                        sLog->OutErrorConsole("npc_unworthy_initiateAI: unable to find anchor!");

                    float dist = 99.0f;
                    GameObject* prison = NULL;

                    for (uint8 i = 0; i < 12; ++i)
                    {
                        if (GameObject* temp_prison = me->FindGameobjectWithDistance(acherus_soul_prison[i], 30.0f))
                        {
                            if (me->IsWithinDist(temp_prison, dist, false))
                            {
                                dist = me->GetDistance2d(temp_prison);
                                prison = temp_prison;
                            }
                        }
                    }

                    if (prison)
                        prison->ResetDoorOrButton();
                    else
                        sLog->OutErrorConsole("npc_unworthy_initiateAI: unable to find prison!");
                }
                break;
            case PHASE_TO_EQUIP:
                if (WaitTimer)
                {
                    if (WaitTimer > diff)
                        WaitTimer -= diff;
                    else
                    {
                        me->GetMotionMaster()->MovePoint(1, anchorX, anchorY, me->GetPositionZ());
                        //sLog->outDebug(LOG_FILTER_QCSCR, "npc_unworthy_initiateAI: move to %f %f %f", anchorX, anchorY, me->GetPositionZ());
                        phase = PHASE_EQUIPING;
                        WaitTimer = 0;
                    }
                }
                break;
            case PHASE_TO_ATTACK:
                if (WaitTimer)
                {
                    if (WaitTimer > diff)
                        WaitTimer -= diff;
                    else
                    {
                        me->SetCurrentFaction(16);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                        phase = PHASE_ATTACKING;

                        if (Player* target = Unit::GetPlayer(*me, playerGUID))
                            me->AI()->AttackStart(target);
                        WaitTimer = 0;
                    }
                }
                break;
            case PHASE_ATTACKING:
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                    case EVENT_ICY_TOUCH:
                        DoCastVictim(SPELL_ICY_TOUCH);
                        events.DelayEvents(1000, 1);
                        events.ScheduleEvent(EVENT_ICY_TOUCH, 5000, 1);
                        break;
                    case EVENT_PLAGUE_STRIKE:
                        DoCastVictim(SPELL_PLAGUE_STRIKE);
                        events.DelayEvents(1000, 1);
                        events.ScheduleEvent(SPELL_PLAGUE_STRIKE, 5000, 1);
                        break;
                    case EVENT_BLOOD_STRIKE:
                        DoCastVictim(SPELL_BLOOD_STRIKE);
                        events.DelayEvents(1000, 1);
                        events.ScheduleEvent(EVENT_BLOOD_STRIKE, 5000, 1);
                        break;
                    case EVENT_DEATH_COIL:
                        DoCastVictim(SPELL_DEATH_COIL);
                        events.DelayEvents(1000, 1);
                        events.ScheduleEvent(EVENT_DEATH_COIL, 5000, 1);
                        break;
                    }
                }

                DoMeleeAttackIfReady();
                break;
            default:
                break;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unworthy_initiateAI(creature);
    }
};

class npc_unworthy_initiate_anchor : public CreatureScript
{
public:
    npc_unworthy_initiate_anchor() : CreatureScript("npc_unworthy_initiate_anchor") { }

    struct npc_unworthy_initiate_anchorAI : public PassiveAI
    {
        npc_unworthy_initiate_anchorAI(Creature* creature) : PassiveAI(creature), prisonerGUID(0) {}

        uint64 prisonerGUID;

        void SetGUID(uint64 guid, int32 /*id*/)
        {
            if (!prisonerGUID)
				prisonerGUID = guid;
        }

        uint64 GetGUID(int32 /*id*/)
		{
			return prisonerGUID;
		}
	};

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unworthy_initiate_anchorAI(creature);
    }
};

enum SoulPrison
{
	NPC_UNWORTHY_INITIATE_ANCHOR = 29521,
};

class go_acherus_soul_prison : public GameObjectScript
{
public:
    go_acherus_soul_prison() : GameObjectScript("go_acherus_soul_prison") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (Creature* anchor = go->FindCreatureWithDistance(NPC_UNWORTHY_INITIATE_ANCHOR, 15.0f))
		{
            if (uint64 prisonerGUID = anchor->AI()->GetGUID())
			{
                if (Creature* prisoner = Creature::GetCreature(*player, prisonerGUID))
					CAST_AI(npc_unworthy_initiate::npc_unworthy_initiateAI, prisoner->AI())->EventStart(anchor, player);
			}
		}
		return false;
    }
};

#define GOSSIP_ACCEPT_DUEL "I challenge you, death knight!"

enum Duels
{
	// Texts
    SAY_DUEL_A                  = -1609080,
    SAY_DUEL_B                  = -1609081,
    SAY_DUEL_C                  = -1609082,
    SAY_DUEL_D                  = -1609083,
    SAY_DUEL_E                  = -1609084,
    SAY_DUEL_F                  = -1609085,
    SAY_DUEL_G                  = -1609086,
    SAY_DUEL_H                  = -1609087,
    SAY_DUEL_I                  = -1609088,
	// Spells
	SPELL_DI_ICY_TOUCH          = 52372,
	SPELL_DI_PLAGUE_STRIKE      = 52373,
	SPELL_DI_BLOOD_STRIKE       = 52374,
	SPELL_DI_DEATH_COIL         = 52375,
    SPELL_DUEL                  = 52996,
	SPELL_GROVEL                = 7267,
    SPELL_DUEL_VICTORY          = 52994,
    SPELL_DUEL_FLAG             = 52991,

    QUEST_DEATH_CHALLENGE       = 12733,
    FACTION_HOSTILE             = 2068,
};

int32 RandomSay[] =
{
    SAY_DUEL_A,
	SAY_DUEL_B,
	SAY_DUEL_C,
	SAY_DUEL_D,
	SAY_DUEL_E,
	SAY_DUEL_F,
	SAY_DUEL_G,
	SAY_DUEL_H,
	SAY_DUEL_I
};

class npc_death_knight_initiate : public CreatureScript
{
public:
    npc_death_knight_initiate() : CreatureScript("npc_death_knight_initiate") { }

	bool OnGossipHello(Player* player, Creature* creature)
    {
        if (player->GetQuestStatus(QUEST_DEATH_CHALLENGE) == QUEST_STATUS_INCOMPLETE && creature->IsFullHealth())
        {
            if (player->HealthBelowPct(HEALTH_PERCENT_10))
                return true;

            if (player->IsInCombatActive() || creature->IsInCombatActive())
                return true;

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ACCEPT_DUEL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
            player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
        }
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*Sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        if (action == GOSSIP_ACTION_INFO_DEF)
        {
            player->CLOSE_GOSSIP_MENU();

            if (player->IsInCombatActive() || creature->IsInCombatActive())
                return true;

            if (npc_death_knight_initiateAI* pInitiateAI = CAST_AI(npc_death_knight_initiate::npc_death_knight_initiateAI, creature->AI()))
            {
                if (pInitiateAI->IsDuelInProgress)
                    return true;
            }

            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);

            int32 SayId = rand()% (sizeof(RandomSay)/sizeof(int32));
            DoSendQuantumText(RandomSay[SayId], creature, player);

            player->CastSpell(creature, SPELL_DUEL, false);
            player->CastSpell(player, SPELL_DUEL_FLAG, true);
        }
        return true;
    }

    struct npc_death_knight_initiateAI : public CombatAI
    {
        npc_death_knight_initiateAI(Creature* creature) : CombatAI(creature)
        {
            IsDuelInProgress = false;
        }

        bool Lose;
		bool IsDuelInProgress;

		uint32 IcyTouchTimer;
		uint32 PlagueStrikeTimer;
		uint32 BloodStrikeTimer;
		uint32 DeathCoilTimer;
		uint32 DuelStartTimer;

        uint64 DuelerGUID;

        void Reset()
        {
            Lose = false;
			IsDuelInProgress = false;

			IcyTouchTimer = 1000;
			PlagueStrikeTimer = 2000;
			BloodStrikeTimer = 3000;
			DeathCoilTimer = 4000;
			DuelStartTimer = 5000;

			DuelerGUID = 0;

            me->RestoreFaction();

			CombatAI::Reset();
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
			if (!IsDuelInProgress && spell->Id == SPELL_DUEL)
            {
                DuelerGUID = caster->GetGUID();
                IsDuelInProgress = true;
            }
		}

		void DamageTaken(Unit* DoneBy, uint32 &damage)
		{
			if (IsDuelInProgress && DoneBy->IsControlledByPlayer())
			{
				if (DoneBy->GetGUID() != DuelerGUID && DoneBy->GetOwnerGUID() != DuelerGUID) // other players cannot help
					damage = 0;

				else if (damage >= me->GetHealth())
				{
					damage = 0;

					if (!Lose)
					{
						DoneBy->RemoveGameObject(SPELL_DUEL_FLAG, true);
						DoneBy->AttackStop();
						me->CastSpell(DoneBy, SPELL_DUEL_VICTORY, true);
						Lose = true;
						me->CastSpell(me, SPELL_GROVEL, true);
						me->RestoreFaction();
					}
				}
			}
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
            {
                if (IsDuelInProgress)
                {
                    if (DuelStartTimer <= diff)
                    {
                        me->SetCurrentFaction(FACTION_HOSTILE);

                        if (Unit* unit = Unit::GetUnit(*me, DuelerGUID))
                            AttackStart(unit);
                    }
                    else DuelStartTimer -= diff;
                }
                return;
            }

			if (IcyTouchTimer <= diff)
			{
				DoCastVictim(SPELL_DI_ICY_TOUCH);
				IcyTouchTimer = urand(3000, 4000);
			}
			else IcyTouchTimer -= diff;

			if (PlagueStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_DI_PLAGUE_STRIKE);
				PlagueStrikeTimer = urand(5000, 6000);
			}
			else PlagueStrikeTimer -= diff;

			if (BloodStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_DI_BLOOD_STRIKE);
				BloodStrikeTimer = urand(7000, 8000);
			}
			else BloodStrikeTimer -= diff;

			if (DeathCoilTimer <= diff)
			{
				DoCastVictim(SPELL_DI_DEATH_COIL);
				DeathCoilTimer = urand(9000, 10000);
			}
			else DeathCoilTimer -= diff;

            if (IsDuelInProgress)
            {
                if (Lose)
                {
                    if (!me->HasAura(SPELL_GROVEL))
                        EnterEvadeMode();
                    return;
                }
                else if (me->GetVictim()->GetTypeId() == TYPE_ID_PLAYER && me->GetVictim()->HealthBelowPct(10))
                {
                    me->GetVictim()->CastSpell(me->GetVictim(), SPELL_GROVEL, true);
                    me->GetVictim()->RemoveGameObject(SPELL_DUEL_FLAG, true);
                    EnterEvadeMode();
                    return;
                }
            }

            CombatAI::UpdateAI(diff);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_death_knight_initiateAI(creature);
    }
};

enum EyeOfAcherus
{
    SPELL_EYE_VISUAL            = 51892,
    SPELL_EYE_FLIGHT_BOOST      = 51923,
    SPELL_EYE_FLIGHT            = 51890,

	MODEL_ID_EYE_OF_ACHERUS     = 26320,

	EMOTE_DESTIANTION           = -1420000,
    EMOTE_CONTROL               = -1420001,

    EVENT_MOVE_START            = 1,

    POINT_EYE_FALL              = 1,
    POINT_EYE_MOVE_END          = 3,
};

Position const EyeOFAcherusFallPoint = { 2361.21f, -5660.45f, 496.7444f, 0.0f };

class npc_eye_of_acherus : public CreatureScript
{
    public:
        npc_eye_of_acherus() : CreatureScript("npc_eye_of_acherus") { }

        struct npc_eye_of_acherusAI : public QuantumBasicAI
        {
            npc_eye_of_acherusAI(Creature* creature) : QuantumBasicAI(creature)
            {
                me->SetDisplayId(me->GetCreatureTemplate()->Modelid1);

                if (Player* owner = me->GetCharmerOrOwner()->ToPlayer())
                    owner->SendAutoRepeatCancel(me);

                me->GetMotionMaster()->MovePoint(POINT_EYE_FALL, EyeOFAcherusFallPoint);

                Movement::MoveSplineInit init(*me);
                init.MoveTo(EyeOFAcherusFallPoint.GetPositionX(), EyeOFAcherusFallPoint.GetPositionY(), EyeOFAcherusFallPoint.GetPositionZ());
                init.SetFall();
                init.Launch();
            }

			EventMap events;

			void Reset()
			{
				DoCast(me, SPELL_EYE_VISUAL, true);
				DoCast(me, SPELL_EYE_FLIGHT);

				me->SetReactState(REACT_PASSIVE);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE | UNIT_FLAG_PLAYER_CONTROLLED);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			}

            void OnCharmed(bool /*apply*/){}

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MOVE_START:
                        {
                            DoCast(me, SPELL_EYE_FLIGHT_BOOST);

                            me->SetControlled(false, UNIT_STATE_ROOT);
                            if (Player* owner = me->GetCharmerOrOwner()->ToPlayer())
                            {
                                for (uint8 i = 0; i < MAX_MOVE_TYPE; ++i)
                                    me->SetSpeed(UnitMoveType(i), owner->GetSpeedRate(UnitMoveType(i)), true);

								DoSendQuantumText(EMOTE_DESTIANTION, owner);

								me->SetDisplayId(MODEL_ID_EYE_OF_ACHERUS); 

                            }
							me->SetSpeed(MOVE_FLIGHT, 2.0f, true);
                            me->GetMotionMaster()->MovePath(me->GetEntry() * 100, false);
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            void MovementInform(uint32 movementType, uint32 pointId)
            {
                if (movementType == WAYPOINT_MOTION_TYPE && pointId == POINT_EYE_MOVE_END - 1)
                {
                    me->SetByteValue(UNIT_FIELD_BYTES_2, 0, SHEATH_STATE_MELEE); 
                    me->RemoveAllAuras();

                    if (Player* owner = me->GetCharmerOrOwner()->ToPlayer())
                    {
                        owner->RemoveAura(SPELL_EYE_FLIGHT_BOOST);
                        for (uint8 i = 0; i < MAX_MOVE_TYPE; ++i)
                            me->SetSpeed(UnitMoveType(i), owner->GetSpeedRate(UnitMoveType(i)), true);

						DoSendQuantumText(EMOTE_CONTROL, owner);
                    }
                    me->SetDisableGravity(false);
                    DoCast(me, SPELL_EYE_FLIGHT);
                }

                if (movementType == POINT_MOTION_TYPE && pointId == POINT_EYE_FALL)
                {
                    me->SetDisableGravity(true);
                    me->SetControlled(true, UNIT_STATE_ROOT);
                    events.ScheduleEvent(EVENT_MOVE_START, 5000);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_eye_of_acherusAI(creature);
        }
};

enum Rider
{
	SPELL_DESPAWN_HORSE = 52267,
};

#define SAY_DARK_RIDER "The realm of shadows awaits..."

class npc_dark_rider_of_acherus : public CreatureScript
{
public:
    npc_dark_rider_of_acherus() : CreatureScript("npc_dark_rider_of_acherus") { }

    struct npc_dark_rider_of_acherusAI : public QuantumBasicAI
    {
        npc_dark_rider_of_acherusAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 PhaseTimer;
        uint32 Phase;

		uint64 TargetGUID;

        bool Intro;

		void Reset()
        {
            PhaseTimer = 4000;
            Phase = 0;

			TargetGUID = 0;

            Intro = false;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!Intro || !TargetGUID)
                return;

            if (PhaseTimer <= diff)
            {
                switch (Phase)
                {
                   case 0:
                        me->MonsterSay(SAY_DARK_RIDER, LANG_UNIVERSAL, 0);
                        PhaseTimer = 5000;
                        Phase = 1;
                        break;
                    case 1:
                        if (Unit* target = Unit::GetUnit(*me, TargetGUID))
                            DoCast(target, SPELL_DESPAWN_HORSE, true);
                        PhaseTimer = 3000;
                        Phase = 2;
                        break;
                    case 2:
                        me->SetVisible(false);
                        PhaseTimer = 2000;
                        Phase = 3;
                        break;
                    case 3:
                        me->DespawnAfterAction();
                        break;
                    default:
                        break;
                }
            }
			else PhaseTimer -= diff;
        }

        void InitDespawnHorse(Unit* who)
        {
            if (!who)
                return;

            TargetGUID = who->GetGUID();
            me->SetWalk(true);
            me->SetSpeed(MOVE_RUN, 0.4f);
            me->GetMotionMaster()->MoveChase(who);
            me->SetTarget(TargetGUID);
            Intro = true;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dark_rider_of_acherusAI(creature);
    }
};

enum SalanarTheHorseman
{
	SPELL_EFFECT_STOLEN_HORSE         = 52263,
    SPELL_DELIVER_STOLEN_HORSE        = 52264,
    SPELL_CALL_DARK_RIDER             = 52266,
    SPELL_EFFECT_OVERTAKE             = 52349,
    SPELL_REALM_OF_SHADOWS            = 52693,

    GOSSIP_SALANAR_MENU               = 9739,
    GOSSIP_SALANAR_OPTION             = 0,

    SALANAR_SAY_COMPLETE_QUEST        = -1420107,

    QUEST_INTO_REALM_OF_SHADOWS       = 12687,

    NPC_DARK_RIDER_OF_ACHERUS         = 28654,
    NPC_SALANAR_IN_REALM_OF_SHADOWS   = 28788,
};

class npc_salanar_the_horseman : public CreatureScript
{
public:
    npc_salanar_the_horseman() : CreatureScript("npc_salanar_the_horseman") { }

    struct npc_salanar_the_horsemanAI : public QuantumBasicAI
    {
        npc_salanar_the_horsemanAI(Creature* creature) : QuantumBasicAI(creature) {}

		void sGossipSelect(Player* player, uint32 menuId, uint32 gossipListId)
        {
            if (menuId == GOSSIP_SALANAR_MENU && gossipListId == GOSSIP_SALANAR_OPTION)
            {
                player->CastSpell(player, SPELL_REALM_OF_SHADOWS, true);
                player->PlayerTalkClass->SendCloseGossip();
            }
		}

		void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_DELIVER_STOLEN_HORSE)
            {
                if (caster->GetTypeId() == TYPE_ID_UNIT && caster->IsVehicle())
                {
                    if (Unit* charmer = caster->GetCharmer())
                    {
                        if (charmer->HasAura(SPELL_EFFECT_STOLEN_HORSE))
                        {
                            charmer->RemoveAurasDueToSpell(SPELL_EFFECT_STOLEN_HORSE);
                            caster->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELL_CLICK);
                            caster->SetCurrentFaction(35);

                            DoCast(caster, SPELL_CALL_DARK_RIDER, true);

                            if (Creature* rider = me->FindCreatureWithDistance(NPC_DARK_RIDER_OF_ACHERUS, 15.0f))
                                CAST_AI(npc_dark_rider_of_acherus::npc_dark_rider_of_acherusAI, rider->AI())->InitDespawnHorse(caster);
                        }
                    }
                }
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            QuantumBasicAI::MoveInLineOfSight(who);

            if (who->GetTypeId() == TYPE_ID_UNIT && who->IsVehicle() && me->IsWithinDistInMap(who, 5.0f))
            {
                if (Unit* charmer = who->GetCharmer())
                {
                    if (charmer->GetTypeId() == TYPE_ID_PLAYER)
                    {
                        if (me->GetEntry() == NPC_SALANAR_IN_REALM_OF_SHADOWS && CAST_PLR(charmer)->GetQuestStatus(QUEST_INTO_REALM_OF_SHADOWS) == QUEST_STATUS_INCOMPLETE)
                        {
							CAST_PLR(charmer)->GroupEventHappens(QUEST_INTO_REALM_OF_SHADOWS, me);
							DoSendQuantumText(SALANAR_SAY_COMPLETE_QUEST, me);
                            charmer->RemoveAurasDueToSpell(SPELL_EFFECT_OVERTAKE);
                            CAST_CRE(who)->DespawnAfterAction();
                        }

						charmer->RemoveAurasDueToSpell(SPELL_REALM_OF_SHADOWS);
                    }
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_salanar_the_horsemanAI(creature);
    }
};

class npc_ros_dark_rider : public CreatureScript
{
public:
    npc_ros_dark_rider() : CreatureScript("npc_ros_dark_rider") { }

    struct npc_ros_dark_riderAI : public QuantumBasicAI
    {
        npc_ros_dark_riderAI(Creature* creature) : QuantumBasicAI(creature) {}

        void EnterToBattle(Unit* /*who*/)
        {
            me->ExitVehicle();
        }

        void Reset()
        {
            Creature* deathcharger = me->FindCreatureWithDistance(28782, 30.0f);
            if (!deathcharger)
				return;

            deathcharger->RestoreFaction();
            deathcharger->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELL_CLICK);
            deathcharger->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            if (!me->GetVehicle() && deathcharger->IsVehicle() && deathcharger->GetVehicleKit()->HasEmptySeat(0))
                me->EnterVehicle(deathcharger);
        }

        void JustDied(Unit* killer)
        {
            Creature* deathcharger = me->FindCreatureWithDistance(28782, 30.0f);
            if (!deathcharger)
				return;

            if (killer->GetTypeId() == TYPE_ID_PLAYER && deathcharger->GetTypeId() == TYPE_ID_UNIT && deathcharger->IsVehicle())
            {
                deathcharger->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELL_CLICK);
                deathcharger->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                deathcharger->SetCurrentFaction(2096);
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ros_dark_riderAI(creature);
    }
};

enum QuestTheGiftThatKeepsOnGiving
{
	SPELL_SCARLET_GHOUL_CREDIT          = 52517,

	QUEST_THE_GIFT_THAT_KEEPS_ON_GIVING = 12698,

    NPC_GHOUL                           = 28845,
    NPC_GHOST                           = 28846,
};

class npc_dkc1_gothik : public CreatureScript
{
public:
    npc_dkc1_gothik() : CreatureScript("npc_dkc1_gothik") { }

    struct npc_dkc1_gothikAI : public QuantumBasicAI
    {
        npc_dkc1_gothikAI(Creature* creature) : QuantumBasicAI(creature) {}

        void MoveInLineOfSight(Unit* who)
        {
            QuantumBasicAI::MoveInLineOfSight(who);

            if (who->GetEntry() == NPC_GHOUL && me->IsWithinDistInMap(who, 10.0f))
            {
                if (Unit* owner = who->GetOwner())
                {
                    if (owner->GetTypeId() == TYPE_ID_PLAYER)
                    {
                        if (CAST_PLR(owner)->GetQuestStatus(QUEST_THE_GIFT_THAT_KEEPS_ON_GIVING) == QUEST_STATUS_INCOMPLETE)
                            CAST_CRE(who)->CastSpell(owner, SPELL_SCARLET_GHOUL_CREDIT, true);

                        CAST_CRE(who)->DespawnAfterAction();

                        if (CAST_PLR(owner)->GetQuestStatus(QUEST_THE_GIFT_THAT_KEEPS_ON_GIVING) == QUEST_STATUS_COMPLETE)
                            owner->RemoveAllMinionsByEntry(NPC_GHOUL);
                    }
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dkc1_gothikAI(creature);
    }
};

enum ScarletGhoul
{
	SAY_GHOUL_SPAWN_1 = -1420002,
	SAY_GHOUL_SPAWN_2 = -1420003,
	SAY_GHOUL_SPAWN_3 = -1420004,
	SAY_GHOUL_SPAWN_4 = -1420005,
	SAY_GHOUL_SPAWN_5 = -1420006,
};

class npc_scarlet_ghoul : public CreatureScript
{
public:
    npc_scarlet_ghoul() : CreatureScript("npc_scarlet_ghoul") { }

    struct npc_scarlet_ghoulAI : public QuantumBasicAI
    {
        npc_scarlet_ghoulAI(Creature* creature) : QuantumBasicAI(creature)
        {
            //DoCast(me, 35177, true);
        }

		void Reset()
		{
			DoSendQuantumText(RAND(SAY_GHOUL_SPAWN_1, SAY_GHOUL_SPAWN_2, SAY_GHOUL_SPAWN_3, SAY_GHOUL_SPAWN_4, SAY_GHOUL_SPAWN_5), me);
            me->SetReactState(REACT_DEFENSIVE);
		}

        void FindMinions(Unit* owner)
        {
            std::list<Creature*> MinionList;
            owner->GetAllMinionsByEntry(MinionList, NPC_GHOUL);

            if (!MinionList.empty())
            {
                for (std::list<Creature*>::const_iterator itr = MinionList.begin(); itr != MinionList.end(); ++itr)
                {
                    if ((*itr)->GetOwner()->GetGUID() == me->GetOwner()->GetGUID())
                    {
                        if ((*itr)->IsInCombatActive() && (*itr)->getAttackerForHelper())
                            AttackStart((*itr)->getAttackerForHelper());
                    }
                }
            }
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!me->IsInCombatActive())
            {
                if (Unit* owner = me->GetOwner())
                {
                    Player* playerOwner = owner->ToPlayer();
                    if (playerOwner && playerOwner->IsInCombatActive())
                    {
                        if (playerOwner->getAttackerForHelper() && playerOwner->getAttackerForHelper()->GetEntry() == NPC_GHOST)
                            AttackStart(playerOwner->getAttackerForHelper());
                        else
                            FindMinions(owner);
                    }
                }
            }

            if (!UpdateVictim())
                return;

            //QuantumBasicAI::UpdateAI(diff);
            if (me->GetVictim()->GetEntry() == NPC_GHOST)
            {
                if (me->IsAttackReady())
                {
                    if (me->IsWithinMeleeRange(me->GetVictim()))
                    {
                        me->AttackerStateUpdate(me->GetVictim());
                        me->ResetAttackTimer();
                    }
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scarlet_ghoulAI(creature);
    }
};

enum MinerCart
{
	SPELL_CART_CHECK = 54173,
	SPELL_CART_DRAG  = 52465,
};

class npc_scarlet_miner_cart : public CreatureScript
{
public:
	npc_scarlet_miner_cart() : CreatureScript("npc_scarlet_miner_cart") { }

    struct npc_scarlet_miner_cartAI : public PassiveAI
    {
        npc_scarlet_miner_cartAI(Creature* creature) : PassiveAI(creature), minerGUID(0)
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            me->SetDisplayId(me->GetCreatureTemplate()->Modelid1);
        }

        uint64 minerGUID;

        void SetGUID(uint64 guid, int32 /*id*/)
        {
            minerGUID = guid;
        }

        void DoAction(const int32 /*param*/)
        {
            if (Creature* miner = Unit::GetCreature(*me, minerGUID))
            {
                me->SetWalk(false);
                me->SetSpeed(MOVE_RUN, 1.32f);
                me->GetMotionMaster()->MoveFollow(miner, 1.0f, 0);
            }
        }

        void PassengerBoarded(Unit* /*who*/, int8 /*seatId*/, bool apply)
        {
            if (!apply)
			{
                if (Creature* miner = Unit::GetCreature(*me, minerGUID))
                {
                    miner->DisappearAndDie();

                    if (Unit* player = Unit::GetPlayer(*me, me->GetCreatorGUID()))
                        player->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                }
			}
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scarlet_miner_cartAI(creature);
    }
};

#define SAY_SCARLET_MINER1 "Where'd this come from? I better get this down to the ships before the foreman sees it!"
#define SAY_SCARLET_MINER2 "Now I can have a rest!"

class npc_scarlet_miner : public CreatureScript
{
public:
    npc_scarlet_miner() : CreatureScript("npc_scarlet_miner") { }

    struct npc_scarlet_minerAI : public npc_escortAI
    {
        npc_scarlet_minerAI(Creature* creature) : npc_escortAI(creature)
        {
			me->SetReactState(REACT_PASSIVE);
        }

        uint32 IntroTimer;
        uint32 IntroPhase;
        uint64 CarGUID;

        void Reset()
        {
            CarGUID = 0;
            IntroTimer = 0;
            IntroPhase = 0;
        }

        void InitWaypoint()
        {
            AddWaypoint(1, 2389.03f,     -5902.74f,     109.014f, 5000);
            AddWaypoint(2, 2341.812012f, -5900.484863f, 102.619743f);
            AddWaypoint(3, 2306.561279f, -5901.738281f, 91.792419f);
            AddWaypoint(4, 2300.098389f, -5912.618652f, 86.014885f);
            AddWaypoint(5, 2294.142090f, -5927.274414f, 75.316849f);
            AddWaypoint(6, 2286.984375f, -5944.955566f, 63.714966f);
            AddWaypoint(7, 2280.001709f, -5961.186035f, 54.228283f);
            AddWaypoint(8, 2259.389648f, -5974.197754f, 42.359348f);
            AddWaypoint(9, 2242.882812f, -5984.642578f, 32.827850f);
            AddWaypoint(10, 2217.265625f, -6028.959473f, 7.675705f);
            AddWaypoint(11, 2202.595947f, -6061.325684f, 5.882018f);
            AddWaypoint(12, 2188.974609f, -6080.866699f, 3.370027f);

            if (urand(0, 1))
            {
                AddWaypoint(13, 2176.483887f, -6110.407227f, 1.855181f);
                AddWaypoint(14, 2172.516602f, -6146.752441f, 1.074235f);
                AddWaypoint(15, 2138.918457f, -6158.920898f, 1.342926f);
                AddWaypoint(16, 2129.866699f, -6174.107910f, 4.380779f);
                AddWaypoint(17, 2117.709473f, -6193.830078f, 13.3542f, 10000);
            }
            else
            {
                AddWaypoint(13, 2184.190186f, -6166.447266f, 0.968877f);
                AddWaypoint(14, 2234.265625f, -6163.741211f, 0.916021f);
                AddWaypoint(15, 2268.071777f, -6158.750977f, 1.822252f);
                AddWaypoint(16, 2270.028320f, -6176.505859f, 6.340538f);
                AddWaypoint(17, 2271.739014f, -6195.401855f, 13.3542f, 10000);
            }
        }

        void InitCartQuest(Player* who)
        {
            CarGUID = who->GetVehicleBase()->GetGUID();
            InitWaypoint();
            Start(false, false, who->GetGUID());
            SetDespawnAtFar(false);
        }

        void WaypointReached(uint32 id)
        {
            switch (id)
            {
                case 1:
                    if (Unit* car = Unit::GetCreature(*me, CarGUID))
                    {
                        me->SetInFront(car);
                        me->SendMovementFlagUpdate();
                    }
                    me->MonsterSay(SAY_SCARLET_MINER1, LANG_UNIVERSAL, 0);
                    SetRun(true);
                    IntroTimer = 4000;
                    IntroPhase = 1;
                    break;
                case 17:
                    if (Unit* car = Unit::GetCreature(*me, CarGUID))
                    {
                        me->SetInFront(car);
                        me->SendMovementFlagUpdate();
                        car->Relocate(car->GetPositionX(), car->GetPositionY(), me->GetPositionZ() + 1);
                        car->StopMoving();
                        car->RemoveAura(SPELL_CART_DRAG);
                        if (Unit* player = Unit::GetPlayer(*car, car->GetCreatorGUID()))
                        {
                            player->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                        }
                    }
                    me->MonsterSay(SAY_SCARLET_MINER2, LANG_UNIVERSAL, 0);
                    break;
                default:
                    break;
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (IntroPhase)
            {
                if (IntroTimer <= diff)
                {
                    if (IntroPhase == 1)
                    {
                        if (Creature* car = Unit::GetCreature(*me, CarGUID))
						{
							DoCast(car, SPELL_CART_DRAG);
						}
                        IntroTimer = 800;
                        IntroPhase = 2;
                    }
                    else
                    {
						if (Creature* car = Unit::GetCreature(*me, CarGUID))
							car->AI()->DoAction(0);
						IntroPhase = 0;
                    }
                }
				else IntroTimer-=diff;
            }
            npc_escortAI::UpdateAI(diff);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scarlet_minerAI(creature);
    }
};

enum MineCar
{
	SPELL_MINE_CART_SUMMON         = 52463,
	QUEST_MASSACRE_AT_LIGHTS_POINT = 12701,
	NPC_SCARLET_MINER              = 28841,
	NPC_MINE_CAR                   = 28817,
};

class go_inconspicuous_mine_car : public GameObjectScript
{
public:
    go_inconspicuous_mine_car() : GameObjectScript("go_inconspicuous_mine_car") { }

    bool OnGossipHello(Player* player, GameObject* /*go*/)
    {
        if (player->GetQuestStatus(QUEST_MASSACRE_AT_LIGHTS_POINT) == QUEST_STATUS_INCOMPLETE)
        {
            if (Creature* miner = player->SummonCreature(NPC_SCARLET_MINER, 2383.869629f, -5900.312500f, 107.996086f, player->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 1))
            {
                player->CastSpell(player, SPELL_MINE_CART_SUMMON, true);
                if (Creature* car = player->GetVehicleCreatureBase())
                {
                    if (car->GetEntry() == NPC_MINE_CAR)
                    {
                        car->AI()->SetGUID(miner->GetGUID());
                        player->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                        CAST_AI(npc_scarlet_miner::npc_scarlet_minerAI, miner->AI())->InitCartQuest(player);
                    }
					else sLog->OutErrorConsole("QUANTUMCORE SCRIPTS: OnGossipHello vehicle entry is not correct.");
                }
				else sLog->OutErrorConsole("QUANTUMCORE SCRIPTS: OnGossipHello player is not on the vehicle.");
            }
			else sLog->OutErrorConsole("QUANTUMCORE SCRIPTS: OnGossipHello Scarlet Miner cant be found by script.");
        }
        return true;
    }
};

enum TheLichKing
{
	QUEST_IN_SERVICE_OF_THE_LICH_KING = 12593,

	SPELL_ICEBOUND_VISAGE             = 53274,
	SPELL_EXPLOSION_BLUE              = 58024,

	SAY_LICH_KING_INTRO_1             = -1420016,
	SAY_LICH_KING_INTRO_2             = -1420017,
	SAY_LICH_KING_INTRO_3             = -1420018,

	ACTION_LICH_KING_INTRO_EVENT      = 1,

	EVENT_LICH_KING_INTRO_1           = 1,
	EVENT_LICH_KING_INTRO_2           = 2,
	EVENT_LICH_KING_INTRO_3           = 3,
	EVENT_RESET_STATES                = 4,
};

class npc_the_lich_king_eh : public CreatureScript
{
public:
	npc_the_lich_king_eh() : CreatureScript("npc_the_lich_king_eh") { }

	bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
	{
		if (quest->GetQuestId() == QUEST_IN_SERVICE_OF_THE_LICH_KING)
		{
			creature->CastSpell(creature, SPELL_EXPLOSION_BLUE, true);
			creature->AI()->DoAction(ACTION_LICH_KING_INTRO_EVENT);
			creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUEST_GIVER);
		}
		return true;
	}

    struct npc_the_lich_king_ehAI : public QuantumBasicAI
    {
        npc_the_lich_king_ehAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void DoAction(int32 const action)
		{
			switch (action)
			{
				case ACTION_LICH_KING_INTRO_EVENT:
					events.ScheduleEvent(EVENT_LICH_KING_INTRO_1, 4*IN_MILLISECONDS);
					break;
			}
		}

        void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			DoCast(me, SPELL_ICEBOUND_VISAGE, true);
		}

		void UpdateAI(uint32 const diff)
		{
			// Out of Combat Timer
			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch(eventId)
				{
				    case EVENT_LICH_KING_INTRO_1:
						DoSendQuantumText(SAY_LICH_KING_INTRO_1, me);
						events.ScheduleEvent(EVENT_LICH_KING_INTRO_2, 20*IN_MILLISECONDS);
						break;
					case EVENT_LICH_KING_INTRO_2:
						DoSendQuantumText(SAY_LICH_KING_INTRO_2, me);
						events.ScheduleEvent(EVENT_LICH_KING_INTRO_3, 24*IN_MILLISECONDS);
						break;
					case EVENT_LICH_KING_INTRO_3:
						DoSendQuantumText(SAY_LICH_KING_INTRO_3, me);
						events.ScheduleEvent(EVENT_RESET_STATES, 17*IN_MILLISECONDS);
						break;
					case EVENT_RESET_STATES:
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
						me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUEST_GIVER);
						Reset();
						break;
				}
			}
		}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_the_lich_king_ehAI(creature);
    }
};

enum Mograine
{
	SOUND_MOGRAINE_INTRO = 12893,

	MOGRAINE_MOUNT_ID    = 25279,
};

class npc_highlord_darion_mograine_eh : public CreatureScript
{
public:
    npc_highlord_darion_mograine_eh() : CreatureScript("npc_highlord_darion_mograine_eh") {}

    struct npc_highlord_darion_mograine_ehAI : public QuantumBasicAI
    {
        npc_highlord_darion_mograine_ehAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 MograineSoundTimer;

        void Reset()
        {
			MograineSoundTimer = 1000;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->Mount(MOGRAINE_MOUNT_ID);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (MograineSoundTimer <= diff)
			{
				me->PlayDistanceSound(SOUND_MOGRAINE_INTRO);
				MograineSoundTimer = 30000;
			}
			else MograineSoundTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_highlord_darion_mograine_ehAI(creature);
    }
};

enum TheLichKingChapter1
{
	SOUND_LICH_KING_RANDOM_1  = 14771,
	SOUND_LICH_KING_RANDOM_2  = 14772,
	SOUND_LICH_KING_RANDOM_3  = 14773,
	SOUND_LICH_KING_RANDOM_4  = 14774,
	SOUND_LICH_KING_RANDOM_5  = 14775,
	SOUND_LICH_KING_RANDOM_6  = 14776,
	SOUND_LICH_KING_RANDOM_7  = 14777,
	SOUND_LICH_KING_RANDOM_8  = 14778,
	SOUND_LICH_KING_RANDOM_9  = 14779,
	SOUND_LICH_KING_RANDOM_10 = 14780,
	SOUND_LICH_KING_RANDOM_11 = 14781,
	SOUND_LICH_KING_RANDOM_12 = 14782,
	SOUND_LICH_KING_RANDOM_13 = 14783,
	SOUND_LICH_KING_RANDOM_14 = 14784,
	SOUND_LICH_KING_RANDOM_15 = 14785,
	SOUND_LICH_KING_RANDOM_16 = 14786,
};

int32 RandomCommandSound[] =
{
	SOUND_LICH_KING_RANDOM_1,
	SOUND_LICH_KING_RANDOM_2,
	SOUND_LICH_KING_RANDOM_3,
	SOUND_LICH_KING_RANDOM_4,
	SOUND_LICH_KING_RANDOM_5,
	SOUND_LICH_KING_RANDOM_6,
	SOUND_LICH_KING_RANDOM_7,
	SOUND_LICH_KING_RANDOM_8,
	SOUND_LICH_KING_RANDOM_9,
	SOUND_LICH_KING_RANDOM_10,
	SOUND_LICH_KING_RANDOM_11,
	SOUND_LICH_KING_RANDOM_12,
	SOUND_LICH_KING_RANDOM_13,
	SOUND_LICH_KING_RANDOM_14,
	SOUND_LICH_KING_RANDOM_15,
	SOUND_LICH_KING_RANDOM_16,
};

class npc_the_lich_king_eh_chapter1 : public CreatureScript
{
public:
    npc_the_lich_king_eh_chapter1() : CreatureScript("npc_the_lich_king_eh_chapter1") {}

    struct npc_the_lich_king_eh_chapter1AI : public QuantumBasicAI
    {
        npc_the_lich_king_eh_chapter1AI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 RandomSoundTimer;		

        void Reset()
        {
			RandomSoundTimer = 3000;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			if (RandomSoundTimer <= diff && !me->IsInCombatActive())
			{
				int32 SoundId = rand()% (sizeof(RandomCommandSound)/sizeof(int32));
				me->PlayDistanceSound(RandomCommandSound[SoundId]);
				RandomSoundTimer = 35000;
			}
			else RandomSoundTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_the_lich_king_eh_chapter1AI(creature);
    }
};

void AddSC_the_scarlet_enclave_c1()
{
	new npc_runeforge_se();
	new npc_runebladed_sword();
    new npc_unworthy_initiate();
	new npc_eye_of_acherus();
    new npc_unworthy_initiate_anchor();
    new go_acherus_soul_prison();
    new npc_death_knight_initiate();
    new npc_salanar_the_horseman();
    new npc_dark_rider_of_acherus();
    new npc_ros_dark_rider();
    new npc_dkc1_gothik();
    new npc_scarlet_ghoul();
    new npc_scarlet_miner();
    new npc_scarlet_miner_cart();
    new go_inconspicuous_mine_car();
	new npc_the_lich_king_eh();
	new npc_highlord_darion_mograine_eh();
	new npc_the_lich_king_eh_chapter1();
}