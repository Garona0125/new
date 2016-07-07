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
#include "the_eye.h"

enum Spells
{
    SPELL_FLAME_BUFFET           = 34121, // Flame Buffet - every 1, 5 secs in phase 1 if there is no victim in melee range and after Dive Bomb in phase 2 with same conditions
    SPELL_FLAME_QUILLS           = 34229, // Randomly after changing position in phase after watching tons of movies, set probability 20%
    SPELL_REBIRTH                = 34342, // Rebirth - beginning of second phase(after losing all health in phase 1)
    SPELL_REBIRTH_2              = 35369, // Rebirth(another, without healing to full HP) - after Dive Bomb in phase 2
    SPELL_MELT_ARMOR             = 35410, // Melt Armor - every 60 sec in phase 2
    SPELL_CHARGE                 = 35412, // Charge - 30 sec cooldown
    SPELL_DIVE_BOMB_VISUAL       = 35367, // Bosskillers says 30 sec cooldown, wowwiki says 30 sec colldown, DBM and BigWigs addons says ~47 sec
    SPELL_DIVE_BOMB              = 35181, // after watching tonns of movies, set cooldown to 40+rand()%5.
    SPELL_BERSERK                = 45078, // 10 minutes after phase 2 starts(id is wrong, but proper id is unknown)
    SPELL_EMBER_BLAST            = 34133, // When Ember of Al'ar dies, it casts Ember Blast
    SPELL_FLAME_PATCH            = 35380, //
};

static float waypoint[6][3] =
{
    {340.15f, 58.65f, 17.71f},
    {388.09f, 31.54f, 20.18f},
    {388.18f, -32.85f, 20.18f},
    {340.29f, -60.19f, 17.72f},
    {332.0f, 0.01f, 39.0f}, // better not use the same xy coord
    {331.0f, 0.01f, -2.39f},
};

enum WaitEventType
{
    WE_NONE     = 0,
    WE_DUMMY    = 1,
    WE_PLATFORM = 2,
    WE_QUILL    = 3,
    WE_DIE      = 4,
    WE_REVIVE   = 5,
    WE_CHARGE   = 6,
    WE_METEOR   = 7,
    WE_DIVE     = 8,
    WE_LAND     = 9,
    WE_SUMMON   = 10,
};

class boss_alar : public CreatureScript
{
    public:
        boss_alar() : CreatureScript("boss_alar") { }

        struct boss_alarAI : public QuantumBasicAI
        {
            boss_alarAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
                DefaultMoveSpeedRate = creature->GetSpeedRate(MOVE_RUN);
            }

            InstanceScript* instance;

            WaitEventType WaitEvent;
            uint32 WaitTimer;

            bool AfterMoving;

            uint32 PlatformsMoveTimer;
            uint32 DiveBombTimer;
            uint32 MeltArmorTimer;
            uint32 ChargeTimer;
            uint32 FlamePatchTimer;
            uint32 BerserkTimer;

            float DefaultMoveSpeedRate;

            bool Phase1;
            bool ForceMove;
            uint32 ForceTimer;

            int8 CurrentWaypoint;

            void Reset()
            {
				instance->SetData(DATA_ALAR, NOT_STARTED);

                BerserkTimer = 1200000;
                PlatformsMoveTimer = 0;

                Phase1 = true;
                WaitEvent = WE_NONE;
                WaitTimer = 0;
                AfterMoving = false;
                ForceMove = false;
                ForceTimer = 5000;

                CurrentWaypoint = 4;

                me->SetDisplayId(me->GetNativeDisplayId());
                me->SetSpeed(MOVE_RUN, DefaultMoveSpeedRate);
                me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
                me->SetUnitMovementFlags(MOVEMENTFLAG_DISABLE_GRAVITY);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetActive(false);
            }

            void EnterToBattle(Unit* /*who*/)
            {
				instance->SetData(DATA_ALAR, IN_PROGRESS);

                me->SetUnitMovementFlags(MOVEMENTFLAG_DISABLE_GRAVITY); // after enterevademode will be set walk movement
                DoZoneInCombat();
                me->SetActive(true);
            }

            void JustDied(Unit* /*victim*/)
            {
				instance->SetData(DATA_ALAR, DONE);
            }

            void JustSummoned(Creature* summon)
            {
                if (summon->GetEntry() == NPC_EMBER_OF_ALAR)
				{
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                        summon->AI()->AttackStart(target);
				}
            }

            void MoveInLineOfSight(Unit* /*who*/) {}

            void AttackStart(Unit* who)
            {
                if (Phase1)
                    AttackStartNoMove(who);
                else
                    QuantumBasicAI::AttackStart(who);
            }

            void DamageTaken(Unit* /*killer*/, uint32 &damage)
            {
                if (damage >= me->GetHealth() && Phase1)
                {
                    damage = 0;
                    if (!WaitEvent)
                    {
                        WaitEvent = WE_DIE;
                        WaitTimer = 0;
                        me->SetHealth(0);
                        me->InterruptNonMeleeSpells(true);
                        me->RemoveAllAuras();
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->AttackStop();
                        me->SetTarget(0);
                        me->SetSpeed(MOVE_RUN, 5.0f);
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MovePoint(0, waypoint[5][0], waypoint[5][1], waypoint[5][2]);
                    }
                }
            }

            void SpellHit(Unit*, const SpellInfo* spell)
            {
                if (spell->Id == SPELL_DIVE_BOMB_VISUAL)
                {
                    me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
                    me->SetDisplayId(MODEL_ID_INVISIBLE);
                    //me->SendUpdateObjectToAllExcept(NULL);
                }
            }

            void MovementInform(uint32 type, uint32 /*id*/)
            {
                if (type == POINT_MOTION_TYPE)
                {
                    WaitTimer = 1;
                    AfterMoving = true;
                    ForceMove = false;
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (!me->IsInCombatActive()) // sometimes IsInCombatActive but !incombat, faction bug?
                    return;

                if (BerserkTimer <= diff)
                {
                    DoCast(me, SPELL_BERSERK, true);
                    BerserkTimer = 60000;
                }
                else BerserkTimer -= diff;

                if (ForceMove)
                {
                    if (ForceTimer <= diff)
                    {
                        me->GetMotionMaster()->MovePoint(0, waypoint[CurrentWaypoint][0], waypoint[CurrentWaypoint][1], waypoint[CurrentWaypoint][2]);
                        ForceTimer = 5000;
                    }
                    else ForceTimer -= diff;
                }

                if (WaitEvent)
                {
                    if (WaitTimer)
                    {
                        if (WaitTimer <= diff)
                        {
                            if (AfterMoving)
                            {
                                me->GetMotionMaster()->MoveIdle();
                                AfterMoving = false;
                            }

                            switch (WaitEvent)
                            {
                            case WE_PLATFORM:
                                PlatformsMoveTimer = 30000+rand()%5000;
                                break;
                            case WE_QUILL:
                                DoCast(me, SPELL_FLAME_QUILLS, true);
                                PlatformsMoveTimer = 1;
                                WaitTimer = 10000;
                                WaitEvent = WE_DUMMY;
                                return;
                            case WE_DIE:
                                ForceMove = false;
                                me->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_DEAD);
                                WaitTimer = 5000;
                                WaitEvent = WE_REVIVE;
                                return;
                            case WE_REVIVE:
                                me->SetUInt32Value(UNIT_FIELD_BYTES_1, UNIT_STAND_STATE_STAND);
                                me->SetFullHealth();
                                me->SetSpeed(MOVE_RUN, DefaultMoveSpeedRate);
                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                DoZoneInCombat();
                                DoCast(me, SPELL_REBIRTH, true);
                                MeltArmorTimer = 60000;
                                ChargeTimer = 7000;
                                DiveBombTimer = 40000+rand()%5000;
                                FlamePatchTimer = 30000;
                                Phase1 = false;
                                break;
                            case WE_METEOR:
                                me->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, false);
                                DoCast(me, SPELL_DIVE_BOMB_VISUAL, false);
                                WaitEvent = WE_DIVE;
                                WaitTimer = 4000;
                                return;
                            case WE_DIVE:
                                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                                {
                                    me->RemoveAurasDueToSpell(SPELL_DIVE_BOMB_VISUAL);
                                    DoCast(target, SPELL_DIVE_BOMB, true);
                                    float dist = 3.0f;
                                    if (me->IsWithinDist3d(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 5.0f))
                                        dist = 5.0f;
                                    WaitTimer = 1000 + uint32(floor(dist / 80 * 1000.0f));
                                    me->SetPosition(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0.0f);
                                    me->StopMoving();
                                    WaitEvent = WE_LAND;
                                }
                                else
                                {
                                    EnterEvadeMode();
                                    return;
                                }
                            case WE_LAND:
                                WaitEvent = WE_SUMMON;
                                WaitTimer = 2000;
                                return;
                            case WE_SUMMON:
                                for (uint8 i = 0; i < 2; ++i)
                                    DoSpawnCreature(NPC_EMBER_OF_ALAR, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                                me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 10);
                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                me->SetDisplayId(me->GetNativeDisplayId());
                                DoCast(me, SPELL_REBIRTH_2, true);
                                break;
                            case WE_DUMMY:
                            default:
                                break;
                            }

                            WaitEvent = WE_NONE;
                            WaitTimer = 0;
                        }
                        else WaitTimer -= diff;
                    }
                    return;
                }

                if (Phase1)
                {
                    if (me->getThreatManager().getThreatList().empty())
                    {
                        EnterEvadeMode();
                        return;
                    }

                    if (PlatformsMoveTimer <= diff)
                    {
                        if (CurrentWaypoint == 4)
                        {
                            CurrentWaypoint = 0;
                            WaitEvent = WE_PLATFORM;
                        }
                        else
                        {
                            if (urand(0, 4)) // next platform
                            {
                                DoSpawnCreature(NPC_EMBER_OF_ALAR, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                                if (CurrentWaypoint == 3)
                                    CurrentWaypoint = 0;
                                else
                                    ++CurrentWaypoint;
                                WaitEvent = WE_PLATFORM;
                            }
                            else // flame quill
                            {
                                CurrentWaypoint = 4;
                                WaitEvent = WE_QUILL;
                            }
                        }
                        ForceMove = true;
                        ForceTimer = 5000;
                        me->GetMotionMaster()->MovePoint(0, waypoint[CurrentWaypoint][0], waypoint[CurrentWaypoint][1], waypoint[CurrentWaypoint][2]);
                        WaitTimer = 0;
                        return;
                    }
                    else PlatformsMoveTimer -= diff;
                }
                else
                {
                    if (ChargeTimer <= diff)
                    {
                        if (Unit* target= SelectTarget(TARGET_RANDOM, 1, 100, true))
						{
                            DoCast(target, SPELL_CHARGE);
							ChargeTimer = 30000;
						}
                    }
                    else ChargeTimer -= diff;

                    if (MeltArmorTimer <= diff)
                    {
                        DoCastVictim(SPELL_MELT_ARMOR);
                        MeltArmorTimer = 60000;
                    }
                    else MeltArmorTimer -= diff;

                    if (DiveBombTimer <= diff)
                    {
                        me->AttackStop();
                        me->GetMotionMaster()->MovePoint(6, waypoint[4][0], waypoint[4][1], waypoint[4][2]);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 50);
                        WaitEvent = WE_METEOR;
                        WaitTimer = 0;
                        DiveBombTimer = 40000+rand()%5000;
                        return;
                    }
                    else DiveBombTimer -= diff;

                    if (FlamePatchTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                        {
                            Creature* patch = me->SummonCreature(NPC_FLAME_PATCH_ALAR, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 120000);
                            if (patch)
                            {
                                patch->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                patch->SetObjectScale(patch->GetFloatValue(OBJECT_FIELD_SCALE_X)*2.5f);
                                patch->SetDisplayId(MODEL_ID_INVISIBLE);
                                patch->SetCurrentFaction(me->GetFaction());
                                patch->SetLevel(me->GetCurrentLevel());
                                patch->CastSpell(patch, SPELL_FLAME_PATCH, false);
                            }
                        }
                        FlamePatchTimer = 30000;
                    }
                    else FlamePatchTimer -= diff;
                }

                DoMeleeAttackIfReady();
            }

            void DoMeleeAttackIfReady()
            {
                if (me->IsAttackReady() && !me->IsNonMeleeSpellCasted(false))
                {
                    if (me->IsWithinMeleeRange(me->GetVictim()))
                    {
                        me->AttackerStateUpdate(me->GetVictim());
                        me->ResetAttackTimer();
                    }
                    else
                    {
                        Unit* target = NULL;
                        target = me->SelectNearestTargetInAttackDistance(5);
                        if (target)
                            me->AI()->AttackStart(target);
                        else
                        {
							if (Phase1)
								DoCast(me, SPELL_FLAME_BUFFET, true);
							me->SetAttackTimer(BASE_ATTACK, 1500);
                        }
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* Creature) const
        {
            return new boss_alarAI(Creature);
        }
};

class npc_ember_of_alar : public CreatureScript
{
    public:
        npc_ember_of_alar() : CreatureScript("npc_ember_of_alar") {}

        struct npc_ember_of_alarAI : public QuantumBasicAI
        {
            npc_ember_of_alarAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
                creature->SetUnitMovementFlags(MOVEMENTFLAG_DISABLE_GRAVITY);
                creature->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FIRE, true);
            }

            InstanceScript* instance;

            bool ToDie;

            void Reset()
            {
                ToDie = false;
            }

            void EnterToBattle(Unit* /*who*/)
            {
                DoZoneInCombat();
            }

            void EnterEvadeMode()
            {
                me->setDeathState(JUST_DIED);
            }

            void DamageTaken(Unit* killer, uint32 &damage)
            {
                if (damage >= me->GetHealth() && killer != me && !ToDie)
                {
                    damage = 0;
                    DoCast(me, SPELL_EMBER_BLAST, true);
                    me->SetDisplayId(MODEL_ID_INVISIBLE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    if (instance && instance->GetData(DATA_ALAR) == 2)
                    {
                        if (Unit* Alar = Unit::GetUnit(*me, instance->GetData64(DATA_ALAR)))
                        {
                            int32 AlarHealth = int32(Alar->GetHealth()) - int32(Alar->CountPctFromMaxHealth(3));
                            if (AlarHealth > 0)
                                Alar->SetHealth(AlarHealth);
                            else
                                Alar->SetHealth(1);
                        }
                    }
                    ToDie = true;
                }
            }

            void UpdateAI(const uint32 /*diff*/)
            {
                if (!UpdateVictim())
                    return;

                if (ToDie)
                    me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

                DoMeleeAttackIfReady();
            }

        };

        CreatureAI* GetAI(Creature* Creature) const
        {
            return new npc_ember_of_alarAI(Creature);
        }
};

class npc_flame_patch_alar : public CreatureScript
{
    public:
        npc_flame_patch_alar() : CreatureScript("npc_flame_patch_alar") {}

        struct npc_flame_patch_alarAI : public QuantumBasicAI
        {
            npc_flame_patch_alarAI(Creature* creature) : QuantumBasicAI(creature){}

            void Reset(){}

            void EnterToBattle(Unit* /*who*/){}

            void AttackStart(Unit* /*who*/){}

            void MoveInLineOfSight(Unit* /*who*/){}

            void UpdateAI(const uint32 /*diff*/){}
        };

        CreatureAI* GetAI(Creature* Creature) const
        {
            return new npc_flame_patch_alarAI(Creature);
        }
};

void AddSC_boss_alar()
{
    new boss_alar();
    new npc_ember_of_alar();
    new npc_flame_patch_alar();
}