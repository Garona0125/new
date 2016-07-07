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
#include "utgarde_keep.h"

enum Texts
{
    SAY_AGGRO_1                    = -1574005,
    SAY_AGGRO_2                    = -1574006,
    SAY_DEAD_1                     = -1574007,
    SAY_DEAD_2                     = -1574008,
    SAY_SLAY_1                     = -1574009,
    SAY_SLAY_2                     = -1574010,
    SAY_RESSURECT                  = -1574023,
};

enum Spells
{
    SPELL_CLEAVE                        = 42724,
    SPELL_SMASH_5N                      = 42669,
    SPELL_SMASH_5H                      = 59706,
    SPELL_STAGGERING_ROAR_5N            = 42708,
    SPELL_STAGGERING_ROAR_5H            = 59708,
    SPELL_ENRAGE_5N                     = 42705,
    SPELL_ENRAGE_5H                     = 59707,
	SPELL_DREADFUL_ROAR_5N              = 42729,
    SPELL_DREADFUL_ROAR_5H              = 59734,
    SPELL_WOE_STRIKE_5N                 = 42730,
    SPELL_WOE_STRIKE_5H                 = 59735,
	SPELL_SHADOW_AXE_DAMAGE_5N          = 42750,
    SPELL_SHADOW_AXE_DAMAGE_5H          = 59719,
    SPELL_INGVAR_FEIGN_DEATH            = 42795,
    SPELL_SUMMON_BANSHEE                = 42912,
    SPELL_SCOURGE_RESURRECTION          = 42863,
	SPELL_SCOURGE_RESURRECTION_HEAL     = 42704,
    SPELL_SCOURGE_RESURRECTION_BEAM     = 42857,
    SPELL_SCOURGE_RESURRECTION_DUMMY    = 42862,
    SPELL_DARK_SMASH_5N                 = 42723,
	SPELL_DARK_SMASH_5H                 = 59709,
    SPELL_SHADOW_AXE_SUMMON             = 42749,
    SPELL_INGVAR_TRANSFORM              = 42796,
};

enum Events
{
    EVENT_SUMMON_ANNHYLDE = 1,
    EVENT_CLEAVE,
    EVENT_PRE_SMASH,
    EVENT_SMASH,
    EVENT_ENRAGE,
    EVENT_SHADOW_AXE,
    EVENT_ROAR
};

class boss_ingvar_the_plunderer : public CreatureScript
{
    public:
        boss_ingvar_the_plunderer() : CreatureScript("boss_ingvar_the_plunderer") { }

        struct boss_ingvar_the_plundererAI : public QuantumBasicAI
        {
            boss_ingvar_the_plundererAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
				IsUndead = false;
            }

			InstanceScript* instance;

			EventMap events;

            uint64 TargetGUID;

            bool IsUndead;

            bool EventInProgress;

            void Reset()
            {
                if (IsUndead)
                    me->UpdateEntry(NPC_INGVAR_THE_PLUNDERER);

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                me->SetStandState(UNIT_STAND_STATE_STAND);

                events.Reset();
                IsUndead = false;
                EventInProgress = false;
                TargetGUID = 0;

                events.ScheduleEvent(EVENT_CLEAVE, 5*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_PRE_SMASH, 12*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_ENRAGE, 10*IN_MILLISECONDS);

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

				instance->SetData(DATA_INGVAR_EVENT, NOT_STARTED);
            }

            void DamageTaken(Unit* /*attacker*/, uint32 &damage)
            {
                if (damage >= me->GetHealth() && !IsUndead)
                {
                    // DoCast(me, SPELL_INGVAR_FEIGN_DEATH, true);
                    // visual hack
                    me->SetHealth(0);
                    me->InterruptNonMeleeSpells(true);
                    me->RemoveAllAuras();
                    me->SetReactState(REACT_PASSIVE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                    me->GetMotionMaster()->MovementExpired(false);
                    me->GetMotionMaster()->MoveIdle();
                    me->SetStandState(UNIT_STAND_STATE_DEAD);

                    events.Reset();
                    events.ScheduleEvent(EVENT_SUMMON_ANNHYLDE, 3*IN_MILLISECONDS);
                    EventInProgress = true;
                    IsUndead = true;

                    DoSendQuantumText(SAY_DEAD_1, me);
                }

                if (EventInProgress)
                    damage = 0;
            }

            void StartZombiePhase()
            {
                IsUndead = true;
                EventInProgress = false;
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                me->UpdateEntry(NPC_INGVAR_UNDEAD);
                me->SetReactState(REACT_AGGRESSIVE);
                DoZoneInCombat(me, 100.0f);

                if (me->GetVictim())
                {
                    me->SetInCombatWith(me->GetVictim());
                    me->GetMotionMaster()->MoveChase(me->GetVictim());
                }

                events.ScheduleEvent(EVENT_CLEAVE, 3*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_PRE_SMASH, 7*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SHADOW_AXE, 15*IN_MILLISECONDS);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                DoSendQuantumText(IsUndead ? SAY_AGGRO_2 : SAY_AGGRO_1, me);

				instance->SetData(DATA_INGVAR_EVENT, IN_PROGRESS);
            }

            void JustDied(Unit* /*killer*/)
            {
                DoSendQuantumText(SAY_DEAD_2, me);

				instance->SetData(DATA_INGVAR_EVENT, DONE);
            }

            void KilledUnit(Unit* /*victim*/)
            {
                DoSendQuantumText(IsUndead ? SAY_SLAY_1 : SAY_SLAY_2, me);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (me->HasReactState(REACT_PASSIVE) && !EventInProgress && !me->GetVictim())
                    me->SetReactState(REACT_AGGRESSIVE);

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SUMMON_ANNHYLDE:
                            DoCast(me, SPELL_SUMMON_BANSHEE); // Summons directly on caster position
                            break;
                        case EVENT_CLEAVE:
                            DoCastVictim(IsUndead ? DUNGEON_MODE(SPELL_WOE_STRIKE_5N, SPELL_WOE_STRIKE_5H) : SPELL_CLEAVE);
                            events.ScheduleEvent(EVENT_CLEAVE, urand(4, 7) *IN_MILLISECONDS);
                            break;
                        case EVENT_PRE_SMASH:
                        {
                            // some workaround due to ingvar always trying to face victim when casting smash
                            float x, y, z;
                            z = me->GetPositionZ();
                            me->GetNearPoint2D(x, y, 3.0f, me->GetOrientation());

                            if (Creature* target = me->SummonCreature(NPC_WORLD_TRIGGER, x, y, z + 2.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 4000))
                            {
                                TargetGUID = target->GetGUID();
                                target->SetMaxHealth(100000);
                                target->SetHealth(100000);
                                target->SetReactState(REACT_PASSIVE);
                                target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                                AttackStart(target);
                                me->SetReactState(REACT_PASSIVE);
                                me->GetMotionMaster()->MoveFollow(target, 0.0f, 0.0f);
                            }
                            events.ScheduleEvent(EVENT_SMASH, 700);
                            return;
                        }
                        case EVENT_SMASH:
                        {
                            if (Creature* target = ObjectAccessor::GetCreature(*me, TargetGUID))
                            {
                                DoCast(target, IsUndead ? SPELL_DARK_SMASH_5N, SPELL_DARK_SMASH_5H : DUNGEON_MODE(SPELL_SMASH_5N, SPELL_SMASH_5H));
                                me->SetFacingToObject(target);
                            }
                            TargetGUID = 0;
                            events.ScheduleEvent(EVENT_ROAR, urand(4, 5) *IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_PRE_SMASH, 14300);
                            return;
                        }
                        case EVENT_ENRAGE:
                            DoCast(me, DUNGEON_MODE(SPELL_ENRAGE_5N, SPELL_ENRAGE_5H));
                            events.ScheduleEvent(EVENT_ENRAGE, 10*IN_MILLISECONDS);
                            break;
                        case EVENT_SHADOW_AXE:
                            // Spawn target for Axe
                            if (Unit* target = SelectTarget(TARGET_TOP_AGGRO, 1))
                            {
                                me->SummonCreature(NPC_THROW_TARGET, *target, TEMPSUMMON_TIMED_DESPAWN, 2000);
                                DoCast(me, SPELL_SHADOW_AXE_SUMMON);
                            }
                            events.ScheduleEvent(EVENT_SHADOW_AXE, 30*IN_MILLISECONDS);
                            break;
                        case EVENT_ROAR:
                            DoCast(me, IsUndead ? DUNGEON_MODE(SPELL_DREADFUL_ROAR_5N, SPELL_DREADFUL_ROAR_5H): DUNGEON_MODE(SPELL_STAGGERING_ROAR_5N, SPELL_STAGGERING_ROAR_5H));
                            break;
                        default:
                            break;
                    }
                }

                if (!EventInProgress)
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_ingvar_the_plundererAI(creature);
        }
};

class mob_annhylde_the_caller : public CreatureScript
{
    public:
        mob_annhylde_the_caller() : CreatureScript("mob_annhylde_the_caller") { }

        struct mob_annhylde_the_callerAI : public QuantumBasicAI
        {
            mob_annhylde_the_callerAI(Creature* creature) : QuantumBasicAI(creature)
            {
				instance = creature->GetInstanceScript();
            }

			float _x, _y, _z;
            InstanceScript* instance;
            uint32 ResurectTimer;
            uint8 ResurectPhase;

            void Reset()
            {
                me->AddUnitMovementFlag(MOVEMENTFLAG_FLYING | MOVEMENT_FLAG_HOVER);
                me->SetSpeed(MOVE_SWIM, 1.0f);
                me->SetSpeed(MOVE_RUN, 1.0f);
                me->SetSpeed(MOVE_WALK, 1.0f);

                me->GetPosition(_x, _y, _z);
                DoTeleportTo(_x + 1, _y, _z + 30);

                if (Unit* ingvar = ObjectAccessor::GetUnit(*me, instance ? instance->GetData64(DATA_INGVAR) : 0))
                    me->GetMotionMaster()->MovePoint(1, _x, _y, _z + 15);
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (Unit* ingvar = ObjectAccessor::GetUnit(*me, instance ? instance->GetData64(DATA_INGVAR) : 0))
                {
                    switch (id)
                    {
                        case 1:
                            DoSendQuantumText(SAY_RESSURECT, me);
                            ingvar->RemoveAura(SPELL_SUMMON_BANSHEE);
                            ingvar->CastSpell(ingvar, SPELL_SCOURGE_RESURRECTION_DUMMY, true);
                            DoCast(ingvar, SPELL_SCOURGE_RESURRECTION_BEAM);
                            ResurectTimer = 10000;
                            ResurectPhase = 1;
                            break;
                        case 2:
                            me->SetVisible(false);
                            me->DealDamage(me, me->GetHealth());
                            me->RemoveCorpse();
                            break;
                    }
                }
            }

            void AttackStart(Unit* /*who*/){}

            void MoveInLineOfSight(Unit* /*who*/){}

            void EnterToBattle(Unit* /*who*/){}

            void UpdateAI(uint32 const diff)
            {
                if (ResurectTimer)
                {
                    if (ResurectTimer <= diff)
                    {
                        if (ResurectPhase == 1)
                        {
                            if (Unit* ingvar = ObjectAccessor::GetUnit(*me, instance ? instance->GetData64(DATA_INGVAR) : 0))
                            {
                                ingvar->SetStandState(UNIT_STAND_STATE_STAND);
                                ingvar->CastSpell(ingvar, SPELL_SCOURGE_RESURRECTION_HEAL, true);
                            }
                            ResurectTimer = 3000;
                            ResurectPhase = 2;
                        }
                        else if (ResurectPhase == 2)
                        {
                            if (Creature* ingvar = ObjectAccessor::GetCreature(*me, instance ? instance->GetData64(DATA_INGVAR) : 0))
                            {
                                ingvar->RemoveAurasDueToSpell(SPELL_SCOURGE_RESURRECTION_DUMMY);

                                if (boss_ingvar_the_plunderer::boss_ingvar_the_plundererAI* ai = CAST_AI(boss_ingvar_the_plunderer::boss_ingvar_the_plundererAI, ingvar->AI()))
                                    ai->StartZombiePhase();

                                me->GetMotionMaster()->MovePoint(2, _x + 1, _y, _z + 30);
                                ++ResurectPhase;
                                ResurectTimer = 0;
                            }
                        }
                    }
                    else ResurectTimer -= diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_annhylde_the_callerAI(creature);
        }
};

class mob_ingvar_throw_dummy : public CreatureScript
{
    public:
        mob_ingvar_throw_dummy() : CreatureScript("mob_ingvar_throw_dummy") { }

        struct mob_ingvar_throw_dummyAI : public QuantumBasicAI
        {
            mob_ingvar_throw_dummyAI(Creature* creature) : QuantumBasicAI(creature) { }

			bool ReachedTarget;
            uint32 DespawnTimer;

            void Reset()
            {
                if (Unit* target = me->FindCreatureWithDistance(NPC_THROW_TARGET, 50.0f))
                {
                    float x, y, z;
                    target->GetPosition(x, y, z);
                    me->GetMotionMaster()->MovePoint(1, x, y, z);
                }
                ReachedTarget = false;

                DespawnTimer = 10000;
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                ReachedTarget = true;

                DoCast(me, DUNGEON_MODE(SPELL_SHADOW_AXE_DAMAGE_5N, SPELL_SHADOW_AXE_DAMAGE_5H));
            }

            void AttackStart(Unit* /*who*/){}

            void MoveInLineOfSight(Unit* /*who*/){}

            void EnterToBattle(Unit* /*who*/){}

            void UpdateAI(uint32 const diff)
            {
                if (ReachedTarget)
				{
                    if (DespawnTimer <= diff)
                    {
                        me->DealDamage(me, me->GetHealth());
                        me->RemoveCorpse();
                    }
                    else DespawnTimer -= diff;
				}
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_ingvar_throw_dummyAI(creature);
        }
};

void AddSC_boss_ingvar_the_plunderer()
{
    new boss_ingvar_the_plunderer();
    new mob_annhylde_the_caller();
    new mob_ingvar_throw_dummy();
}