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
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "naxxramas.h"

enum Horsemen
{
    HORSEMEN_THANE,
    HORSEMEN_LADY,
    HORSEMEN_BARON,
    HORSEMEN_SIR,
};

enum Spells
{
	SPELL_MARK_DAMAGE = 28836,
};

enum Events
{
    EVENT_NONE,
    EVENT_MARK,
    EVENT_CAST,
    EVENT_BERSERK,
};

const Position WaypointPositions[12] =
{
    // Thane waypoints
    {2542.3f, -2984.1f, 241.49f, 5.362f},
    {2547.6f, -2999.4f, 241.34f, 5.049f},
    {2542.9f, -3015.0f, 241.35f, 4.654f},
    // Lady waypoints
    {2498.3f, -2961.8f, 241.28f, 3.267f},
    {2487.7f, -2959.2f, 241.28f, 2.890f},
    {2469.4f, -2947.6f, 241.28f, 2.576f},
    // Baron waypoints
    {2553.8f, -2968.4f, 241.33f, 5.757f},
    {2564.3f, -2972.5f, 241.33f, 5.890f},
    {2583.9f, -2971.67f, 241.35f, 0.008f},
    // Sir waypoints
    {2534.5f, -2921.7f, 241.53f, 1.363f},
    {2523.5f, -2902.8f, 241.28f, 2.095f},
    {2517.8f, -2896.6f, 241.28f, 2.315f},
};

const Position HomePositions[4] =
{
	// Thane
	{2520.5f, -2955.38f, 245.635f, 5.58f},
    // Lady
    {2517.62f, -2959.38f, 245.636f, 5.72f},
    // Baron
    {2524.32f, -2951.28f, 245.633f, 5.43f},
    // Sir
    {2528.79f, -2948.58f, 245.633f, 5.27f}
};

#define SPELL_PRIMARY(i)            RAID_MODE(SPELL_PRIMARY_N[i], SPELL_PRIMARY_H[i])
#define SPELL_SECONDARY(i)          RAID_MODE(SPELL_SECONDARY_N[i], SPELL_SECONDARY_H[i])
#define SPELL_BERSERK               26662
#define SPELL_VOID_ZONE_PERIODIC    46262

const uint32 MOB_HORSEMEN[]      = {16063, 16064, 16065, 30549};
const uint32 SPELL_MARK[]        = {28832, 28833, 28834, 28835};
const uint32 SPELL_PRIMARY_N[]   = {28884, 28863, 28882, 28883};
const uint32 SPELL_PRIMARY_H[]   = {57467, 57463, 57369, 57466};
const uint32 SPELL_SECONDARY_N[] = {0, 57374, 0, 57376};
const uint32 SPELL_SECONDARY_H[] = {0, 57464, 0, 57465};
const uint32 SPELL_PUNISH[]      = {0, 57381, 0, 57377};

// used by 16063, 16064, 16065, 30549, but signed for 16063
const int32 SAY_AGGRO[]     =   {-1533051, -1533044, -1533065, -1533058};
const int32 SAY_TAUNT[3][4] ={  {-1533052, -1533045, -1533071, -1533059},
                                {-1533053, -1533046, -1533072, -1533060},
                                {-1533054, -1533047, -1533073, -1533061}, };
const int32 SAY_SPECIAL[]   =   {-1533055, -1533048, -1533070, -1533062};
const int32 SAY_SLAY[]      =   {-1533056, -1533049, -1533068, -1533063};
const int32 SAY_DEATH[]     =   {-1533057, -1533050, -1533074, -1533064};

#define SAY_BARON_AGGRO     RAND (-1533065, -1533066, -1533067)
#define SAY_BARON_SLAY      RAND (-1533068, -1533069)

class boss_four_horsemen : public CreatureScript
{
public:
    boss_four_horsemen() : CreatureScript("boss_four_horsemen") { }

    struct boss_four_horsemenAI : public BossAI
    {
        boss_four_horsemenAI(Creature* creature) : BossAI(creature, BOSS_HORSEMEN)
        {
            id = Horsemen(0);
            for (uint8 i = 0; i < 4; ++i)
			{
                if (me->GetEntry() == MOB_HORSEMEN[i])
					id = Horsemen(i);
			}
            caster = (id == HORSEMEN_LADY || id == HORSEMEN_SIR);
        }

        Horsemen id;
        uint64 EventStarterGUID;
        uint8 nextWP;
        uint32 PunishTimer;
        bool caster;
        bool NextMovementStarted;
        bool MovementCompleted;
        bool MovementStarted;
        bool EncounterActionAttack;
        bool EncounterActionReset;
        bool DoDelayPunish;

        void Reset()
        {
            if (!EncounterActionReset)
                DoEncounterAction(NULL, false, true, false);

			instance->SetData(DATA_HORSEMEN0 + id, NOT_STARTED);

            me->SetReactState(REACT_AGGRESSIVE);
            EventStarterGUID = 0;
            nextWP = 0;
            PunishTimer = 2000;
            NextMovementStarted = false;
            MovementCompleted = false;
            MovementStarted = false;
            EncounterActionAttack = false;
            EncounterActionReset = false;
            DoDelayPunish = false;
            _Reset();

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterEvadeMode()
        {
            _EnterEvadeMode();
            me->GetMotionMaster()->MovePoint(15, HomePositions[id]);
            Reset();
        }

        bool DoEncounterAction(Unit* who, bool attack, bool reset, bool checkAllDead)
        {
            if (!instance)
                return false;

			Creature* Sir = CAST_CRE(Unit::GetUnit(*me, instance->GetData64(DATA_SIR_ZELIEK)));
			Creature* Thane = CAST_CRE(Unit::GetUnit(*me, instance->GetData64(DATA_THANE_KORTHAZZ)));
            Creature* Lady = CAST_CRE(Unit::GetUnit(*me, instance->GetData64(DATA_LADY_BLAUMEUX)));
            Creature* Baron = CAST_CRE(Unit::GetUnit(*me, instance->GetData64(DATA_BARON_RIVENDARE)));

            if (Thane && Lady && Baron && Sir)
            {
                if (attack && who)
                {
                    CAST_AI(boss_four_horsemen::boss_four_horsemenAI, Thane->AI())->EncounterActionAttack = true;
                    CAST_AI(boss_four_horsemen::boss_four_horsemenAI, Lady->AI())->EncounterActionAttack = true;
                    CAST_AI(boss_four_horsemen::boss_four_horsemenAI, Baron->AI())->EncounterActionAttack = true;
                    CAST_AI(boss_four_horsemen::boss_four_horsemenAI, Sir->AI())->EncounterActionAttack = true;
                    CAST_AI(boss_four_horsemen::boss_four_horsemenAI, Thane->AI())->AttackStart(who);
                    CAST_AI(boss_four_horsemen::boss_four_horsemenAI, Lady->AI())->AttackStart(who);
                    CAST_AI(boss_four_horsemen::boss_four_horsemenAI, Baron->AI())->AttackStart(who);
                    CAST_AI(boss_four_horsemen::boss_four_horsemenAI, Sir->AI())->AttackStart(who);
                }

                if (reset)
                {
                    if (instance->GetBossState(BOSS_HORSEMEN) != NOT_STARTED)
                    {
                        if (!Thane->IsAlive())
                            Thane->Respawn();

                        if (!Lady->IsAlive())
                            Lady->Respawn();

                        if (!Baron->IsAlive())
                            Baron->Respawn();

                        if (!Sir->IsAlive())
                            Sir->Respawn();

                        CAST_AI(boss_four_horsemen::boss_four_horsemenAI, Thane->AI())->EncounterActionReset = true;
                        CAST_AI(boss_four_horsemen::boss_four_horsemenAI, Lady->AI())->EncounterActionReset = true;
                        CAST_AI(boss_four_horsemen::boss_four_horsemenAI, Baron->AI())->EncounterActionReset = true;
                        CAST_AI(boss_four_horsemen::boss_four_horsemenAI, Sir->AI())->EncounterActionReset = true;
                        CAST_AI(boss_four_horsemen::boss_four_horsemenAI, Thane->AI())->EnterEvadeMode();
                        CAST_AI(boss_four_horsemen::boss_four_horsemenAI, Lady->AI())->EnterEvadeMode();
                        CAST_AI(boss_four_horsemen::boss_four_horsemenAI, Baron->AI())->EnterEvadeMode();
                        CAST_AI(boss_four_horsemen::boss_four_horsemenAI, Sir->AI())->EnterEvadeMode();
                    }
                }

                if (checkAllDead)
                    return !Thane->IsAlive() && !Lady->IsAlive() && !Baron->IsAlive() && !Sir->IsAlive();
            }
            return false;
        }

        void BeginFourHorsemenMovement()
        {
            MovementStarted = true;
            me->SetReactState(REACT_PASSIVE);
            me->SetWalk(false);
            me->SetSpeed(MOVE_RUN, me->GetSpeedRate(MOVE_RUN), true);

            switch (id)
            {
                case HORSEMEN_THANE:
                    me->GetMotionMaster()->MovePoint(0, WaypointPositions[0]);
                    break;
                case HORSEMEN_LADY:
                    me->GetMotionMaster()->MovePoint(3, WaypointPositions[3]);
                    break;
                case HORSEMEN_BARON:
                    me->GetMotionMaster()->MovePoint(6, WaypointPositions[6]);
                    break;
                case HORSEMEN_SIR:
                    me->GetMotionMaster()->MovePoint(9, WaypointPositions[9]);
                    break;
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE || id == 15)
                return;

            if (id == 2 || id == 5 || id == 8 || id == 11)
            {
                MovementCompleted = true;
                me->SetReactState(REACT_AGGRESSIVE);

                Unit* eventStarter = Unit::GetUnit(*me, EventStarterGUID);

                if (eventStarter && me->IsValidAttackTarget(eventStarter))
                    AttackStart(eventStarter);
                else if (!UpdateVictim())
                {
                    EnterEvadeMode();
                    return;
                }

                if (caster)
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveIdle();
                }
                return;
            }

            NextMovementStarted = false;
            nextWP = id + 1;
        }

        void SelectNearestTarget(Unit* who)
        {
            if (me->GetVictim() && me->GetDistanceOrder(who, me->GetVictim()) && me->IsValidAttackTarget(who))
            {
                me->getThreatManager().modifyThreatPercent(me->GetVictim(), -100);
                me->AddThreat(who, 99999999.9f);
            }
        }

        void MoveInLineOfSight(Unit* who)
        {
            BossAI::MoveInLineOfSight(who);
            if (caster)
                SelectNearestTarget(who);
        }

        void AttackStart(Unit* who)
        {
            if (!MovementCompleted && !MovementStarted)
            {
                EventStarterGUID = who->GetGUID();
                BeginFourHorsemenMovement();

                if (!EncounterActionAttack)
                    DoEncounterAction(who, true, false, false);
            }
            else if (MovementCompleted && MovementStarted)
            {
                if (caster)
                    me->Attack(who, false);
                else
                    BossAI::AttackStart(who);
            }
        }

        void KilledUnit(Unit* victim)
        {
            if (!(rand()%5))
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
				{
					if (id == HORSEMEN_BARON)
						DoSendQuantumText(SAY_BARON_SLAY, me);
					else
						DoSendQuantumText(SAY_SLAY[id], me);
				}
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            events.Reset();
            summons.DespawnAll();

			instance->SetData(DATA_HORSEMEN0 + id, DONE);

            if (instance && DoEncounterAction(NULL, false, false, true))
            {
				if (Creature* kel = me->FindCreatureWithDistance(NPC_MB_KEL_THUZAD, 500.0f))
					DoSendQuantumText(SAY_KELTHUZAD_TAUNT_4, kel);

                instance->SetBossState(BOSS_HORSEMEN, DONE);
                instance->SaveToDB();
                instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, 59450);
            }

            DoSendQuantumText(SAY_DEATH[id], me);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            _EnterToBattle();

            if (id == HORSEMEN_BARON)
                DoSendQuantumText(SAY_BARON_AGGRO, me);
            else
                DoSendQuantumText(SAY_AGGRO[id], me);

            events.ScheduleEvent(EVENT_MARK, 24000);
            events.ScheduleEvent(EVENT_CAST, 20000+rand()%5000);
            events.ScheduleEvent(EVENT_BERSERK, 15*100*1000);
        }

        void SpellHitTarget(Unit* target, const SpellInfo *spell)
        {
            if (target->GetTypeId() != TYPE_ID_PLAYER)
                return;

            if (spell->Id == SPELL_MARK[0] || spell->Id == SPELL_MARK[1] || spell->Id == SPELL_MARK[2] || spell->Id == SPELL_MARK[3])
                me->getThreatManager().modifyThreatPercent(target,-50);
        }

        void UpdateAI(const uint32 diff)
        {
            if (nextWP && MovementStarted && !MovementCompleted && !NextMovementStarted)
            {
                NextMovementStarted = true;
                me->GetMotionMaster()->MovePoint(nextWP, WaypointPositions[nextWP]);
            }

            if (!UpdateVictim() || !CheckInRoom() || !MovementCompleted)
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_MARK:
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            if (!(rand()%5))
                                DoSendQuantumText(SAY_SPECIAL[id], me);
                            DoCastAOE(SPELL_MARK[id]);
                            events.ScheduleEvent(EVENT_MARK, caster ? 15000 : 12000);
                        }
						else
							events.ScheduleEvent(EVENT_MARK, 100);
                        break;
                    case EVENT_CAST:
                        if (!me->IsNonMeleeSpellCasted(false))
                        {
                            if (!(rand()%5))
                                DoSendQuantumText(SAY_TAUNT[rand()%3][id], me);

                            if (caster)
                            {
                                if (Unit *target = SelectTarget(TARGET_RANDOM, 0, 45.0f))
                                    DoCast(target, SPELL_PRIMARY(id));
                            }
                            else
                                DoCastVictim(SPELL_PRIMARY(id));

                            events.ScheduleEvent(EVENT_CAST, 15000);
                        }
                        break;
                    case EVENT_BERSERK:
                        DoSendQuantumText(SAY_SPECIAL[id], me);
                        DoCast(me, SPELL_BERSERK, true);
						break;
				}
			}

            if (PunishTimer <= diff)
            {
                if (DoDelayPunish)
                {
                    Unit* pTemp = NULL;

                    std::list<Unit *> playerList;
                    SelectTargetList(playerList, 10, TARGET_NEAREST, 45);
                    for (std::list<Unit*>::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
                    {
                        pTemp = (*itr);
                        if (me->IsWithinLOSInMap(pTemp) && !pTemp->IsImmunedToDamage(SPELL_SCHOOL_MASK_ALL))
                        {
                            SelectNearestTarget(pTemp);
                            break;
                        }
                        pTemp = NULL;
                    }

                    if (!pTemp)
                        DoCastAOE(SPELL_PUNISH[id], true);

                    DoDelayPunish = false;
                }
                PunishTimer = 2000;
            }
			else PunishTimer -= diff;

            if (!caster)
                DoMeleeAttackIfReady();
            else if ((!DoSpellAttackIfReady(SPELL_SECONDARY(id)) || !me->IsWithinLOSInMap(me->GetVictim())) && MovementCompleted && !DoDelayPunish)
				DoDelayPunish = true;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_four_horsemenAI (creature);
    }
};

class spell_four_horsemen_mark : public SpellScriptLoader
{
    public:
        spell_four_horsemen_mark() : SpellScriptLoader("spell_four_horsemen_mark") { }

        class spell_four_horsemen_mark_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_four_horsemen_mark_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    int32 damage;
                    switch (GetStackAmount())
                    {
                        case 1:
                            damage = 0;
                            break;
                        case 2:
                            damage = 500;
                            break;
                        case 3:
                            damage = 1000;
                            break;
                        case 4:
                            damage = 1500;
                            break;
                        case 5:
                            damage = 4000;
                            break;
                        case 6:
                            damage = 12000;
                            break;
                        default:
                            damage = 20000 + 1000 * (GetStackAmount() - 7);
                            break;
                    }
                    if (damage)
                        caster->CastCustomSpell(SPELL_MARK_DAMAGE, SPELLVALUE_BASE_POINT0, damage, GetTarget());
                }
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_four_horsemen_mark_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_four_horsemen_mark_AuraScript();
        }
};

class npc_void_zone_four_horsemen : public CreatureScript
{
public:
	npc_void_zone_four_horsemen() : CreatureScript("npc_void_zone_four_horsemen") {}

	struct npc_void_zone_four_horsemenAI : public QuantumBasicAI
	{
		npc_void_zone_four_horsemenAI(Creature* creature) : QuantumBasicAI(creature){}

		void Reset()
		{
			DoCastAOE(SPELL_VOID_ZONE_PERIODIC);
		}

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_void_zone_four_horsemenAI(creature);
	}
};

void AddSC_boss_four_horsemen()
{
    new boss_four_horsemen();
	new spell_four_horsemen_mark();
	new npc_void_zone_four_horsemen();
}