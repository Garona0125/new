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
#include "halls_of_lightning.h"

enum Texts
{
    SAY_AGGRO     = -1602011,
    SAY_SLAY_1    = -1602012,
    SAY_SLAY_2    = -1602013,
    SAY_SLAY_3    = -1602014,
    SAY_DEATH     = -1602015,
    SAY_SPLIT_1   = -1602016,
    SAY_SPLIT_2   = -1602017,
};

enum Spells
{
    SPELL_BALL_LIGHTNING_5N        = 52780,
    SPELL_BALL_LIGHTNING_5H        = 59800,
    SPELL_STATIC_OVERLOAD_5N       = 52658,
    SPELL_STATIC_OVERLOAD_5H       = 59795,
	SPELL_SPARK_VISUAL_TRIGGER_5N  = 52667,
    SPELL_SPARK_VISUAL_TRIGGER_5H  = 59833,
    SPELL_DISPERSE                 = 52770,
    SPELL_SUMMON_SPARK             = 52746,
    SPELL_SPARK_DESPAWN            = 52776,
    SPELL_ARC_WELD                 = 59086,
};

enum Misc
{
    DATA_MAX_SPARKS                               = 5,
    DATA_MAX_SPARK_DISTANCE                       = 90, // Distance to boss - prevent runs through the whole instance
    DATA_POINT_CALLBACK                           = 0
};

class boss_ionar : public CreatureScript
{
public:
    boss_ionar() : CreatureScript("boss_ionar") { }

    struct boss_ionarAI : public QuantumBasicAI
    {
        boss_ionarAI(Creature* creature) : QuantumBasicAI(creature), lSparkList(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        SummonList lSparkList;

        bool bIsSplitPhase;

        uint32 SplitTimer;

        uint32 StaticOverloadTimer;
        uint32 BallLightningTimer;
        uint32 HealthAmountModifier;

        void Reset()
        {
            lSparkList.DespawnAll();

            bIsSplitPhase = true;
            HealthAmountModifier = 1;

            SplitTimer = 25*IN_MILLISECONDS;
            StaticOverloadTimer = 10*IN_MILLISECONDS;
            BallLightningTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_DISABLE_MOVE);
            me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);

            if (!me->IsVisible())
                me->SetVisible(true);

            if (instance)
                instance->SetData(TYPE_IONAR, NOT_STARTED);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

            if (instance)
                instance->SetData(TYPE_IONAR, IN_PROGRESS);

            DoCast(me, SPELL_DISPERSE, true);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

            lSparkList.DespawnAll();

            if (instance)
                instance->SetData(TYPE_IONAR, DONE);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
        }

        void CallBackSparks()
        {
            if (lSparkList.empty())
                return;

            Position pos;
            me->GetPosition(&pos);

            for (std::list<uint64>::const_iterator itr = lSparkList.begin(); itr != lSparkList.end(); ++itr)
            {
                if (Creature* spark = Unit::GetCreature(*me, *itr))
                {
                    if (spark->IsAlive())
                    {
                        spark->SetSpeed(MOVE_RUN, 2.0f);
                        spark->GetMotionMaster()->Clear();
                        spark->GetMotionMaster()->MovePoint(DATA_POINT_CALLBACK, pos);
                    }
                    else spark->DespawnAfterAction();
                }
            }
        }

        void SpellHit(Unit* caster, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_DISPERSE)
            {
                for (uint8 i = 0; i < DATA_MAX_SPARKS; ++i)
                    me->CastSpell(me, SPELL_SUMMON_SPARK, true);

                me->AttackStop();
                me->SetVisible(false);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE|UNIT_FLAG_DISABLE_MOVE);

                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveIdle();

                me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, true);
            }
        }

        void DamageTaken(Unit* /*pDoneBy*/, uint32 &damage)
        {
            if (!me->IsVisible())
                damage = 0;
        }

        void JustSummoned(Creature* summoned)
        {
            if (summoned->GetEntry() == NPC_SPARK_OF_IONAR)
            {
                lSparkList.Summon(summoned);

                summoned->CastSpell(summoned, DUNGEON_MODE(SPELL_SPARK_VISUAL_TRIGGER_5N, SPELL_SPARK_VISUAL_TRIGGER_5H), true);
                Unit* target = SelectTarget(TARGET_RANDOM, 0);
                if (target)
                {
                    summoned->SetInCombatWith(target);
                    summoned->GetMotionMaster()->Clear();
                    summoned->GetMotionMaster()->MoveFollow(target, 0.0f, 0.0f);
                }
            }
        }

        void SummonedCreatureDespawn(Creature* summoned)
        {
            if (summoned->GetEntry() == NPC_SPARK_OF_IONAR)
                lSparkList.Despawn(summoned);
        }

        void UpdateAI(const uint32 diff)
        {

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (!me->IsVisible())
            {
                if (SplitTimer <= diff)
                {
                    SplitTimer = 2500;

                    if (bIsSplitPhase)
                    {
                        CallBackSparks();
                        bIsSplitPhase = false;
                    }

                    else if (lSparkList.empty())
                    {
                        me->SetVisible(true);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE|UNIT_FLAG_NOT_SELECTABLE|UNIT_FLAG_DISABLE_MOVE);

                        DoCast(me, SPELL_SPARK_DESPAWN, false);

                        SplitTimer = 25*IN_MILLISECONDS;
                        bIsSplitPhase = true;

                        if (me->GetVictim())
                            me->GetMotionMaster()->MoveChase(me->GetVictim());
                    }
                }
                else SplitTimer -= diff;

                return;
            }

            if (StaticOverloadTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, DUNGEON_MODE(SPELL_STATIC_OVERLOAD_5N, SPELL_STATIC_OVERLOAD_5H));
						StaticOverloadTimer = 11*IN_MILLISECONDS;
					}
                }
            }
            else StaticOverloadTimer -= diff;

            if (BallLightningTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 100, true))
					{
                        DoCast(target, DUNGEON_MODE(SPELL_BALL_LIGHTNING_5N, SPELL_BALL_LIGHTNING_5H));
						BallLightningTimer = 10*IN_MILLISECONDS;
					}
                }
            }
            else BallLightningTimer -= diff;

            if (me->HealthBelowPct(100 - 20 * HealthAmountModifier))
            {
                ++HealthAmountModifier;

                DoSendQuantumText(RAND(SAY_SPLIT_1, SAY_SPLIT_2), me);

                if (me->IsNonMeleeSpellCasted(false))
                    me->InterruptNonMeleeSpells(false);

                me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_NATURE, false);
                DoCast(me, SPELL_DISPERSE, false);
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_ionarAI(creature);
    }
};

class npc_spark_of_ionar : public CreatureScript
{
public:
    npc_spark_of_ionar() : CreatureScript("npc_spark_of_ionar") { }

    struct npc_spark_of_ionarAI : public QuantumBasicAI
    {
        npc_spark_of_ionarAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 CheckTimer;
        uint32 ArcWeldTimer;

        void Reset()
        {
            ArcWeldTimer = 0;
            me->SetSpeed(MOVE_RUN, 0.7f);
            CheckTimer = 2*IN_MILLISECONDS;
            DoCast(DUNGEON_MODE(SPELL_SPARK_VISUAL_TRIGGER_5N, SPELL_SPARK_VISUAL_TRIGGER_5H));
        }

        void MovementInform(uint32 type, uint32 PointId)
        {
            if (type != POINT_MOTION_TYPE || !instance)
                return;

            if (PointId == DATA_POINT_CALLBACK)
                me->DespawnAfterAction();
        }

        void DamageTaken(Unit* /*DoneBy*/, uint32 &damage)
        {
            damage = 0;
        }

        void UpdateAI(const uint32 diff)
        {
            // Despawn if the encounter is not running
            if (instance && instance->GetData(TYPE_IONAR) != IN_PROGRESS)
            {
                me->DespawnAfterAction();
                return;
            }

            if (ArcWeldTimer <= diff)
            {
                DoCast(me, SPELL_ARC_WELD);
                ArcWeldTimer = 500;
            }
			else ArcWeldTimer -= diff;
            
            // Prevent them to follow players through the whole instance
            if (CheckTimer <= diff)
            {
                if (instance)
                {
                    Creature* ionar = instance->instance->GetCreature(instance->GetData64(DATA_IONAR));
                    if (ionar && ionar->IsAlive())
                    {
                        if (me->GetDistance(ionar) > DATA_MAX_SPARK_DISTANCE)
                        {
                            Position pos;
                            ionar->GetPosition(&pos);

                            me->SetSpeed(MOVE_RUN, 2.0f);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MovePoint(DATA_POINT_CALLBACK, pos);
                        }
                    }
                    else me->DespawnAfterAction();
                }
                CheckTimer = 2*IN_MILLISECONDS;
            }
            else CheckTimer -= diff;

        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_spark_of_ionarAI(creature);
    }
};

void AddSC_boss_ionar()
{
    new boss_ionar();
    new npc_spark_of_ionar();
}