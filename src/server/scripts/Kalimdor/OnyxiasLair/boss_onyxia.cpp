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
#include "onyxias_lair.h"

/* ScriptData
SDName: Boss_Onyxia
SD%Complete: 95
SDComment: <Known bugs>
Ground visual for Deep Breath effect;
Not summoning whelps on phase 3 (lacks info)
</Known bugs>
SDCategory: Onyxia's Lair
EndScriptData */

enum Yells
{
	SAY_AGGRO                   = 0,
	SAY_KILL                    = 1,
	SAY_PHASE_2_TRANS           = 2,
	SAY_PHASE_3_TRANS           = 3,
	EMOTE_BREATH                = 4,
};

enum Spells
{
    // Phase 1 spells
    SPELL_WING_BUFFET    = 18500,
    SPELL_FLAME_BREATH   = 18435,
    SPELL_CLEAVE         = 68868,
    SPELL_TAIL_SWEEP     = 68867,
    // Phase 2 spells
    SPELL_FIREBALL       = 18392,
    SPELL_BREATH_1       = 17086,
    SPELL_BREATH_2       = 18351,
    SPELL_BREATH_3       = 18576,
    SPELL_BREATH_4       = 18609,
    SPELL_BREATH_5       = 18564,
    SPELL_BREATH_6       = 18584,
    SPELL_BREATH_7       = 18596,
    SPELL_BREATH_8       = 18617,
    // Phase 3 spells
    SPELL_BELLOWING_ROAR = 18431,
};

struct OnyxMove
{
    uint32 LocId;
    uint32 LocIdEnd;
    uint32 SpellId;
    float fX, fY, fZ;
};

static OnyxMove MoveData[] =
{
    {0, 1, SPELL_BREATH_1, -33.5561f, -182.682f, -56.9457f}, //west
    {1, 0, SPELL_BREATH_2, -31.4963f, -250.123f, -55.1278f}, //east
    {2, 4, SPELL_BREATH_3, 6.8951f, -180.246f, -55.896f}, //north-west
    {3, 5, SPELL_BREATH_4, 10.2191f, -247.912f, -55.896f}, //north-east
    {4, 2, SPELL_BREATH_5, -63.5156f, -240.096f, -55.477f}, //south-east
    {5, 3, SPELL_BREATH_6, -58.2509f, -189.020f, -55.790f}, //south-west
    {6, 7, SPELL_BREATH_7, -65.8444f, -213.809f, -55.2985f}, //south
    {7, 6, SPELL_BREATH_8, 22.8763f, -217.152f, -55.0548f}, //north
};

const Position MiddleRoomLocation =
{
	-23.6155f, -215.357f, -55.7344f, 0.0f
};

const Position Phase2Location =
{
	-80.924f, -214.299f, -82.942f, 0.0f
};

static Position SpawnLocations[3] =
{
    //Whelps
    {-30.127f, -254.463f, -89.440f, 0.0f},
    {-30.817f, -177.106f, -89.258f, 0.0f},
    //Lair Guard
    {-145.950f, -212.831f, -68.659f, 0.0f},
};

class boss_onyxia : public CreatureScript
{
public:
    boss_onyxia() : CreatureScript("boss_onyxia") { }

    struct boss_onyxiaAI : public QuantumBasicAI
    {
        boss_onyxiaAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
            Reset();
        }

        InstanceScript* instance;
        SummonList Summons;

        uint32 Phase;

        uint32 FlameBreathTimer;
        uint32 CleaveTimer;
        uint32 TailSweepTimer;
        uint32 WingBuffetTimer;

        uint32 MovePoint;
        uint32 MovementTimer;
        OnyxMove* PointData;

        uint32 FireballTimer;
		uint32 BreathTimer;
        uint32 WhelpTimer;
        uint32 LairGuardTimer;
        uint32 DeepBreathTimer;

        uint32 BellowingRoarTimer;

        uint8 SummonWhelpCount;
        bool IsMoving;

        void Reset()
        {
            if (!IsCombatMovementAllowed())
                SetCombatMovement(true);

            Phase = PHASE_START;

            FlameBreathTimer = urand(10000, 20000);
            TailSweepTimer = urand(15000, 20000);
            CleaveTimer = urand(2000, 5000);
            WingBuffetTimer = urand(10000, 20000);

            MovePoint = urand(0, 5);
            MovementTimer = 14000;
            PointData = GetMoveData();

            FireballTimer = 15000;
			BreathTimer = 20000;
            WhelpTimer = 60000;
            LairGuardTimer = 60000;
            DeepBreathTimer = 85000;

            BellowingRoarTimer = 30000;

            Summons.DespawnAll();
            SummonWhelpCount = 0;
            IsMoving = false;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_SLEEP);

			instance->SetData(DATA_ONYXIA, NOT_STARTED);
			instance->SetData(DATA_ONYXIA_PHASE, Phase);
			instance->DoStopTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEVEMENT_TIMED_START_EVENT);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            Talk(SAY_AGGRO);
            me->SetInCombatWithZone();
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);

			instance->SetData(DATA_ONYXIA, IN_PROGRESS);
			instance->DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEVEMENT_TIMED_START_EVENT);
        }

        void JustDied(Unit* /*killer*/)
        {
			instance->SetData(DATA_ONYXIA, DONE);

            Summons.DespawnAll();
        }

        void JustSummoned(Creature* summoned)
        {
            summoned->SetInCombatWithZone();
            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                summoned->AI()->AttackStart(target);

            switch (summoned->GetEntry())
            {
                case NPC_WHELP:
                    ++SummonWhelpCount;
                    break;
                case NPC_LAIRGUARD:
                    summoned->SetActive(true);
                    break;
            }
            Summons.Summon(summoned);
        }

        void SummonedCreatureDespawn(Creature* summon)
        {
            Summons.Despawn(summon);
        }

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				Talk(SAY_KILL);
        }

        void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_BREATH_1 ||
                spell->Id == SPELL_BREATH_2 ||
                spell->Id == SPELL_BREATH_3 ||
                spell->Id == SPELL_BREATH_4 ||
                spell->Id == SPELL_BREATH_5 ||
                spell->Id == SPELL_BREATH_6 ||
				spell->Id == SPELL_BREATH_7 ||
				spell->Id == SPELL_BREATH_8)
            {
                PointData = GetMoveData();
                MovePoint = PointData->LocIdEnd;

                me->SetSpeed(MOVE_FLIGHT, 2.0f);
                me->GetMotionMaster()->MovePoint(8, MiddleRoomLocation);
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type == POINT_MOTION_TYPE)
            {
                switch (id)
                {
                    case 8: // Pre Flight Phase
                        PointData = GetMoveData();
                        if (PointData)
                        {
							me->SetCanFly(true);
                            me->SetSpeed(MOVE_FLIGHT, 2.0f);
							me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
							me->HandleEmoteCommand(EMOTE_ONESHOT_FLY_SIT_GROUND_UP);
                            me->GetMotionMaster()->MovePoint(PointData->LocId, PointData->fX, PointData->fY, PointData->fZ);
                        }
                        break;
                    case 9:
                        me->GetMotionMaster()->MoveChase(me->GetVictim());
                        BellowingRoarTimer = 1000;
                        break;
                    case 10: // Flight Phase
                        me->SetCanFly(true);
						me->SetSpeed(MOVE_FLIGHT, 2.0f);
						me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
						me->HandleEmoteCommand(EMOTE_ONESHOT_FLY_SIT_GROUND_UP);
                        me->GetMotionMaster()->MovePoint(11, Phase2Location.GetPositionX(), Phase2Location.GetPositionY(), Phase2Location.GetPositionZ()+25);
                        Talk(SAY_PHASE_2_TRANS);
						instance->SetData(DATA_ONYXIA_PHASE, Phase);
                        WhelpTimer = 5000;
                        LairGuardTimer = 15000;
                        break;
                    case 11:
                        if (PointData)
                            me->GetMotionMaster()->MovePoint(PointData->LocId, PointData->fX, PointData->fY, PointData->fZ);
                        me->GetMotionMaster()->Clear(false);
                        me->GetMotionMaster()->MoveIdle();
                        break;
                    default:
                        IsMoving = false;
                        break;
                }
            }
        }

        void SpellHitTarget(Unit* target, const SpellInfo* Spell)
        {
            //Workaround - Couldn't find a way to group this spells (All Eruption)
            if (((Spell->Id >= 17086 && Spell->Id <= 17095) ||
                (Spell->Id == 17097) ||
                (Spell->Id >= 18351 && Spell->Id <= 18361) ||
                (Spell->Id >= 18564 && Spell->Id <= 18576) ||
                (Spell->Id >= 18578 && Spell->Id <= 18607) ||
                (Spell->Id == 18609) ||
                (Spell->Id >= 18611 && Spell->Id <= 18628) ||
                (Spell->Id >= 21132 && Spell->Id <= 21133) ||
                (Spell->Id >= 21135 && Spell->Id <= 21139) ||
                (Spell->Id >= 22191 && Spell->Id <= 22202) ||
                (Spell->Id >= 22267 && Spell->Id <= 22268)) &&
                (target->GetTypeId() == TYPE_ID_PLAYER))
            {
				instance->SetData(DATA_SHE_DEEP_BREATH_MORE, FAIL);
            }
        }

        OnyxMove* GetMoveData()
        {
            uint32 MaxCount = sizeof(MoveData)/sizeof(OnyxMove);

            for (uint32 i = 0; i < MaxCount; ++i)
            {
                if (MoveData[i].LocId == MovePoint)
                    return &MoveData[i];
            }

            return NULL;
        }

        void SetNextRandomPoint()
        {
            uint32 MaxCount = sizeof(MoveData)/sizeof(OnyxMove);
            uint32 iTemp = rand()%(MaxCount-1);

            if (iTemp >= MovePoint)
                ++iTemp;

            MovePoint = iTemp;
        }

		bool CheckInRoom()
        {
            if (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 95.0f)
            {
                EnterEvadeMode();
                return false;
            }

            return true;
        }

        void UpdateAI(const uint32 diff)
        {
			if (!UpdateVictim() || !CheckInRoom())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (Phase == PHASE_START || Phase == PHASE_END)
            {
                if (Phase == PHASE_START)
                {
                    if (HealthBelowPct(HEALTH_PERCENT_60))
                    {
                        SetCombatMovement(false);
                        Phase = PHASE_BREATH;
						me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, true);
						me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                        me->GetMotionMaster()->MovePoint(10, Phase2Location);
                        return;
                    }
                }
                else
                {
                    if (BellowingRoarTimer <= diff)
                    {
                        DoCastAOE(SPELL_BELLOWING_ROAR);
                        // Eruption
                        GameObject* floor = NULL;
                        Trinity::GameObjectInRangeCheck check(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 15);
                        Trinity::GameObjectLastSearcher<Trinity::GameObjectInRangeCheck> searcher(me, floor, check);
                        me->VisitNearbyGridObject(30, searcher);
                        if (floor)
                            instance->SetData64(DATA_FLOOR_ERUPTION_GUID, floor->GetGUID());
                        BellowingRoarTimer = 30000;
                    }
                    else BellowingRoarTimer -= diff;
                }

                if (FlameBreathTimer <= diff)
                {
					me->SetFacingToObject(me->GetVictim());
                    DoCast(SPELL_FLAME_BREATH);
                    FlameBreathTimer = urand(10000, 20000);
                }
                else FlameBreathTimer -= diff;

                if (TailSweepTimer <= diff)
                {
                    DoCastAOE(SPELL_TAIL_SWEEP);
                    TailSweepTimer = urand(15000, 20000);
                }
                else TailSweepTimer -= diff;

                if (CleaveTimer <= diff)
                {
                    DoCastVictim(SPELL_CLEAVE);
                    CleaveTimer = urand(2000, 5000);
                }
                else CleaveTimer -= diff;

                if (WingBuffetTimer <= diff)
                {
                    DoCast(SPELL_WING_BUFFET);
                    WingBuffetTimer = urand(15000, 30000);
                }
                else WingBuffetTimer -= diff;

                DoMeleeAttackIfReady();
            }
            else
            {
                if (HealthBelowPct(HEALTH_PERCENT_40))
                {
                    Phase = PHASE_END;
					instance->SetData(DATA_ONYXIA_PHASE, Phase);
                    Talk(SAY_PHASE_3_TRANS);
                    SetCombatMovement(true);
                    me->SetCanFly(false);
					me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
					me->HandleEmoteCommand(EMOTE_ONESHOT_FLY_SIT_GROUND_DOWN);
                    IsMoving = false;
					me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, false);
					me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
					me->GetMotionMaster()->MovePoint(9, me->GetHomePosition());
                    return;
                }

                if (DeepBreathTimer <= diff)
                {
                    if (!IsMoving)
                    {
                        if (me->IsNonMeleeSpellCasted(false))
                            me->InterruptNonMeleeSpells(false);

                        Talk(EMOTE_BREATH);
                        DoCast(me, PointData->SpellId);
                        DeepBreathTimer = 70000;
                    }
                }
                else DeepBreathTimer -= diff;

                if (MovementTimer <= diff)
                {
                    if (!IsMoving && !(me->HasUnitState(UNIT_STATE_CASTING)))
                    {
                        SetNextRandomPoint();
                        PointData = GetMoveData();

                        if (!PointData)
                            return;

                        me->GetMotionMaster()->MovePoint(PointData->LocId, PointData->fX, PointData->fY, PointData->fZ);
                        IsMoving = true;
                        MovementTimer = 25000;
                    }
                }
                else MovementTimer -= diff;

                if (FireballTimer <= diff)
                {
                    if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != POINT_MOTION_TYPE)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
                            DoCast(target, SPELL_FIREBALL, true);
							FireballTimer = 6000;
						}
                    }
                }
                else FireballTimer -= diff;

				if (BreathTimer <= diff)
                {
                    if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != POINT_MOTION_TYPE)
                    {
						me->SetFacingToObject(me->GetVictim());
						DoCastAOE(RAND(SPELL_BREATH_1, SPELL_BREATH_2, SPELL_BREATH_3, SPELL_BREATH_4, SPELL_BREATH_5, SPELL_BREATH_6, SPELL_BREATH_7, SPELL_BREATH_8));
						BreathTimer = 18000;
                    }
                }
                else BreathTimer -= diff;

                if (LairGuardTimer <= diff)
                {
                    me->SummonCreature(NPC_LAIRGUARD, SpawnLocations[2].GetPositionX(), SpawnLocations[2].GetPositionY(), SpawnLocations[2].GetPositionZ(), 0.0f, TEMPSUMMON_CORPSE_DESPAWN);
                    LairGuardTimer = 30000;
                }
                else LairGuardTimer -= diff;

                if (WhelpTimer <= diff)
                {
                    me->SummonCreature(NPC_WHELP, SpawnLocations[0].GetPositionX(), SpawnLocations[0].GetPositionY(), SpawnLocations[0].GetPositionZ(), 0.0f, TEMPSUMMON_CORPSE_DESPAWN);
                    me->SummonCreature(NPC_WHELP, SpawnLocations[1].GetPositionX(), SpawnLocations[1].GetPositionY(), SpawnLocations[1].GetPositionZ(), 0.0f, TEMPSUMMON_CORPSE_DESPAWN);
                    if (SummonWhelpCount >= RAID_MODE(20, 40))
                    {
                        SummonWhelpCount = 0;
                        WhelpTimer = 90000;
                    }
                    else WhelpTimer = 500;
                }
                else WhelpTimer -= diff;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_onyxiaAI(creature);
    }
};

void AddSC_boss_onyxia()
{
    new boss_onyxia();
}