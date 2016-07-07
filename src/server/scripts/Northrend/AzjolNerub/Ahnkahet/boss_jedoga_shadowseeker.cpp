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
#include "GameEventMgr.h"
#include "ahnkahet.h"

enum Texts
{
    SAY_AGGRO                         = -1619000,
    SAY_SACRIFICE_1_1                 = -1619001,
    SAY_SACRIFICE_1_2                 = -1619002,
    SAY_SACRIFICE_2_1                 = -1619003,
    SAY_SACRIFICE_2_2                 = -1619004,
    SAY_SLAY_1                        = -1619005,
    SAY_SLAY_2                        = -1619006,
    SAY_SLAY_3                        = -1619007,
    SAY_DEATH                         = -1619008,
    SAY_PREACHING_1                   = -1619009,
    SAY_PREACHING_2                   = -1619010,
    SAY_PREACHING_3                   = -1619011,
    SAY_PREACHING_4                   = -1619012,
    SAY_PREACHING_5                   = -1619013,
};

enum Spells
{
    SPELL_SPHERE_VISUAL                = 56075,
    SPELL_GIFT_OF_THE_HERALD           = 56219,
    SPELL_CYCLONE_STRIKE_5N            = 56855,
    SPELL_CYCLONE_STRIKE_5H            = 60030,
    SPELL_LIGHTNING_BOLT_5N            = 56891,
    SPELL_LIGHTNING_BOLT_5H            = 60032,
    SPELL_THUNDERSHOCK_5N              = 56926,
    SPELL_THUNDERSHOCK_5H              = 60029,
	SPELL_WEAR_CHRISTMAS_HAT           = 61400,
};

const Position JedogaPosition[2] =
{
    {372.330994f, -705.278015f, -0.624178f,  5.427970f},
    {372.330994f, -705.278015f, -16.179716f, 5.427970f},
};

enum SeasonalData
{
	 GAME_EVENT_WINTER_VEIL = 2,
};

class boss_jedoga_shadowseeker : public CreatureScript
{
public:
    boss_jedoga_shadowseeker() : CreatureScript("boss_jedoga_shadowseeker") { }

    struct boss_jedoga_shadowseekerAI : public QuantumBasicAI
    {
        boss_jedoga_shadowseekerAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            bFirstTime = true;
            bPreDone = false;
        }

        InstanceScript* instance;

        uint32 CycloneStrikeTimer;
        uint32 LightningBoltTimer;
        uint32 ThunderShockTimer;
        uint32 HealthAmountModifier;

        bool bPreDone;
        bool bOpFerok;
        bool bOnGround;
        bool bOpFerokFail;
        bool bCanDown;
        bool bAchiev;

        bool bFirstTime;

        void Reset()
        {
            CycloneStrikeTimer = 3*IN_MILLISECONDS;
            LightningBoltTimer = 7*IN_MILLISECONDS;
            ThunderShockTimer = 12*IN_MILLISECONDS;
            HealthAmountModifier = 1;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            bOpFerok = false;
            bOpFerokFail = false;
            bOnGround = false;
            bCanDown = false;
            bAchiev = true;
			bFirstTime = false;

			if (!bFirstTime)
				instance->SetData(DATA_JEDOGA_SHADOWSEEKER_EVENT, FAIL);

			instance->SetData64(DATA_PL_JEDOGA_TARGET, 0);
			instance->SetData64(DATA_ADD_JEDOGA_OPFER, 0);
			instance->SetData(DATA_JEDOGA_RESET_INITIANDS, 0);

            MoveUp();

			if (sGameEventMgr->IsActiveEvent(GAME_EVENT_WINTER_VEIL) && !me->HasAura(SPELL_WEAR_CHRISTMAS_HAT))
				me->AddAura(SPELL_WEAR_CHRISTMAS_HAT, me);
        }

        void EnterToBattle(Unit* who)
        {
            if (!instance || (who->GetTypeId() == TYPE_ID_UNIT && who->GetEntry() == NPC_JEDOGA_CONTROLLER))
                return;

            DoSendQuantumText(SAY_AGGRO, me);
            me->SetInCombatWithZone();
            instance->SetData(DATA_JEDOGA_SHADOWSEEKER_EVENT, IN_PROGRESS);
        }

        void AttackStart(Unit* who)
        {
            if (!who || (who->GetTypeId() == TYPE_ID_UNIT && who->GetEntry() == NPC_JEDOGA_CONTROLLER))
                return;

            QuantumBasicAI::AttackStart(who);
        }

        void KilledUnit(Unit* victim)
        {
            if (!victim || victim->GetTypeId() != TYPE_ID_PLAYER)
                return;

            DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

			instance->SetData(DATA_JEDOGA_SHADOWSEEKER_EVENT, DONE);

			if (IsHeroic() && bAchiev)
				instance->DoCompleteAchievement(ACHIEVEMENT_VOLUNTEER_WORK);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (!instance || !who || (who->GetTypeId() == TYPE_ID_UNIT && who->GetEntry() == NPC_JEDOGA_CONTROLLER))
                return;

            if (!bPreDone && who->GetTypeId() == TYPE_ID_PLAYER && me->GetDistance(who) < 100.0f)
            {
                DoSendQuantumText(RAND(SAY_PREACHING_1, SAY_PREACHING_2, SAY_PREACHING_3, SAY_PREACHING_4, SAY_PREACHING_5), me);
                bPreDone = true;
            }

            if (instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) != IN_PROGRESS || !bOnGround)
                return;

            if (!me->GetVictim() && who->IsTargetableForAttack() && me->IsHostileTo(who) && who->IsInAccessiblePlaceFor(me))
            {
                float attackRadius = me->GetAttackDistance(who);
                if (me->IsWithinDistInMap(who, attackRadius) && me->IsWithinLOSInMap(who))
                {
                    if (!me->GetVictim())
                    {
                        who->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
                        AttackStart(who);
                    }
                    else if (me->GetMap()->IsDungeon())
                    {
                        who->SetInCombatWith(me);
                        me->AddThreat(who, 0.0f);
                    }
                }
            }
        }

        void MoveDown()
        {
            if (!instance)
                return;

            bOpFerokFail = false;

            instance->SetData(DATA_JEDOGA_TRIGGER_SWITCH, 0);
            me->GetMotionMaster()->MovePoint(1, JedogaPosition[1]);
            me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, false);
            me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, false);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE + UNIT_FLAG_NON_ATTACKABLE);

            me->RemoveAurasDueToSpell(SPELL_SPHERE_VISUAL);

            bOnGround = true;

            if (UpdateVictim())
            {
                AttackStart(me->GetVictim());
                me->GetMotionMaster()->MoveChase(me->GetVictim());
            }
            else
            {
                if (Unit* target = Unit::GetUnit(*me, instance->GetData64(DATA_PL_JEDOGA_TARGET)))
                {
                    AttackStart(target);
                    instance->SetData(DATA_JEDOGA_RESET_INITIANDS, 0);

                    if (instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) != IN_PROGRESS)
                        EnterToBattle(target);
                }
                else if (!me->IsInCombatActive())
					EnterEvadeMode();
            }
        }

        void MoveUp()
        {
            if (!instance)
                return;

            me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, true);
            me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, true);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE + UNIT_FLAG_NON_ATTACKABLE);

            me->AttackStop();
            me->RemoveAllAuras();
            me->LoadCreaturesAddon();
            me->GetMotionMaster()->MovePoint(0, JedogaPosition[0]);

            instance->SetData(DATA_JEDOGA_TRIGGER_SWITCH, 1);

            if (instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) == IN_PROGRESS)
                OpferRufen();

            bOnGround = false;
        }

        void OpferRufen()
        {
            if (!instance)
                return;

            uint64 opfer = instance->GetData64(DATA_ADD_JEDOGA_INITIAND);

            if (opfer)
            {
                DoSendQuantumText(RAND(SAY_SACRIFICE_1_1, SAY_SACRIFICE_1_2), me);
                instance->SetData64(DATA_ADD_JEDOGA_OPFER, opfer);
            }
			else bCanDown = true;
        }

        void Opfern()
        {
            DoSendQuantumText(RAND(SAY_SACRIFICE_2_1, SAY_SACRIFICE_2_2), me);

            me->InterruptNonMeleeSpells(false);
            DoCast(me, SPELL_GIFT_OF_THE_HERALD, false);

            bOpFerok = false;
            bCanDown = true;
            bAchiev = false;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!instance)
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) != IN_PROGRESS && instance->GetData(DATA_ALL_INITIAND_DEAD))
                MoveDown();

            if (bOpFerok && !bOnGround && !bCanDown)
                Opfern();

            if (bOpFerokFail && !bOnGround && !bCanDown)
                bCanDown = true;

            if (bCanDown)
            {
                MoveDown();
                bCanDown = false;
            }

            if (bOnGround)
            {
                if (!UpdateVictim())
                    return;

                if (CycloneStrikeTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCast(me, DUNGEON_MODE(SPELL_CYCLONE_STRIKE_5N, SPELL_CYCLONE_STRIKE_5H), false);
                        CycloneStrikeTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
                    }
                }
				else CycloneStrikeTimer -= diff;

                if (LightningBoltTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0, -5, true))
						{
                            me->CastSpell(target, DUNGEON_MODE(SPELL_LIGHTNING_BOLT_5N, SPELL_LIGHTNING_BOLT_5H), false);
							LightningBoltTimer = urand(8*IN_MILLISECONDS, 12*IN_MILLISECONDS);
						}
                    }
                }
				else LightningBoltTimer -= diff;

                if (ThunderShockTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
						{
                            me->CastSpell(target, DUNGEON_MODE(SPELL_THUNDERSHOCK_5N, SPELL_THUNDERSHOCK_5H), false);
							ThunderShockTimer = urand(15*IN_MILLISECONDS, 20*IN_MILLISECONDS);
						}
                    }
                }
				else ThunderShockTimer -= diff;

                if (me->HealthBelowPct(100 - 25 * HealthAmountModifier))
                {
                    ++HealthAmountModifier;
                    MoveUp();
                }

                DoMeleeAttackIfReady();
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_jedoga_shadowseekerAI(creature);
    }
};

class npc_twilight_initiate : public CreatureScript
{
public:
    npc_twilight_initiate() : CreatureScript("npc_twilight_initiate") {}

    struct npc_twilight_initiateAI : public QuantumBasicAI
    {
        npc_twilight_initiateAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 bCheckTimer;

        bool bWalking;

        void Reset()
        {
            if (!instance)
                return;

            bWalking = false;
            bCheckTimer = 2*IN_MILLISECONDS;

            if (instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) != IN_PROGRESS)
            {
                me->RemoveAurasDueToSpell(SPELL_SPHERE_VISUAL);
                me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, false);
                me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, false);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE + UNIT_FLAG_NON_ATTACKABLE);
            }
            else
            {
                DoCast(me, SPELL_SPHERE_VISUAL, false);
                me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, true);
                me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE + UNIT_FLAG_NON_ATTACKABLE);
            }
        }

        void JustDied(Unit* Killer)
        {
            if (!Killer || !instance) return;

            if (bWalking)
            {
                Creature* boss = me->GetMap()->GetCreature(instance->GetData64(DATA_JEDOGA_SHADOWSEEKER));
                if (boss && !CAST_AI(boss_jedoga_shadowseeker::boss_jedoga_shadowseekerAI, boss->AI())->bOpFerok) CAST_AI(boss_jedoga_shadowseeker::boss_jedoga_shadowseekerAI, boss->AI())->bOpFerokFail = true;

                if (Killer->GetTypeId() == TYPE_ID_PLAYER) instance->SetData(DATA_INITIAND_KILLED, 1);
                instance->SetData64(DATA_ADD_JEDOGA_OPFER, 0);

                bWalking = false;
            }
            if (Killer->GetTypeId() == TYPE_ID_PLAYER) instance->SetData64(DATA_PL_JEDOGA_TARGET, Killer->GetGUID());
        }

        void EnterToBattle(Unit* who)
        {
            if ((instance && instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) == IN_PROGRESS) || !who)
				return;
        }

        void AttackStart(Unit* victim)
        {
            if ((instance && instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) == IN_PROGRESS) || !victim)
				return;

            QuantumBasicAI::AttackStart(victim);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if ((instance && instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) == IN_PROGRESS) || !who)
				return;

            QuantumBasicAI::MoveInLineOfSight(who);
        }

        void MovementInform(uint32 type, uint32 PointId)
        {
            if (type != POINT_MOTION_TYPE || !instance)
				return;

            switch (PointId)
            {
                case 1:
				{
					Creature* boss = me->GetMap()->GetCreature(instance->GetData64(DATA_JEDOGA_SHADOWSEEKER));
					if (boss)
					{
						CAST_AI(boss_jedoga_shadowseeker::boss_jedoga_shadowseekerAI, boss->AI())->bOpFerok = true;
						CAST_AI(boss_jedoga_shadowseeker::boss_jedoga_shadowseekerAI, boss->AI())->bOpFerokFail = false;
						me->Kill(me);
					}
				}
				break;
			}
		}

        void UpdateAI(const uint32 diff)
        {
            if (bCheckTimer <= diff)
            {
                if (me->GetGUID() == instance->GetData64(DATA_ADD_JEDOGA_OPFER) && !bWalking)
                {
                    me->SetMaxHealth(DUNGEON_MODE(25705, 58648)); // TODO: implement npc entry 30385
                    me->SetHealth(DUNGEON_MODE(25705, 58648));
                    me->RemoveAurasDueToSpell(SPELL_SPHERE_VISUAL);
                    me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, false);
                    me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, false);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE + UNIT_FLAG_NON_ATTACKABLE);

                    float distance = me->GetDistance(JedogaPosition[1]);

                    if (distance < 9.0f)
                        me->SetSpeed(MOVE_WALK, 0.5f, true);
                    else if (distance < 15.0f)
                        me->SetSpeed(MOVE_WALK, 0.75f, true);
                    else if (distance < 20.0f)
                        me->SetSpeed(MOVE_WALK, 1.0f, true);

                    me->GetMotionMaster()->Clear(false);
                    me->GetMotionMaster()->MovePoint(1, JedogaPosition[1]);
                    bWalking = true;
                }
                if (!bWalking)
                {
                    if (instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) != IN_PROGRESS && me->HasAura(SPELL_SPHERE_VISUAL))
                    {
                        me->RemoveAurasDueToSpell(SPELL_SPHERE_VISUAL);
                        me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, false);
                        me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, false);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE + UNIT_FLAG_NON_ATTACKABLE);
                    }

                    if (instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) == IN_PROGRESS && !me->HasAura(SPELL_SPHERE_VISUAL))
                    {
                        DoCast(me, SPELL_SPHERE_VISUAL, false);
                        me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, true);
                        me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, true);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE + UNIT_FLAG_NON_ATTACKABLE);
                    }
                }
                bCheckTimer = 2*IN_MILLISECONDS;
            }
			else bCheckTimer -= diff;

            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_twilight_initiateAI(creature);
    }
};

enum ControllerSpell
{
	SPELL_BEAM_VISUAL_JEDOGAS_AUFSEHER_1  = 60342,
	SPELL_BEAM_VISUAL_JEDOGAS_AUFSEHER_2  = 56312,
};

class npc_jedoga_controller : public CreatureScript
{
public:
    npc_jedoga_controller() : CreatureScript("npc_jedoga_controller") { }

    struct npc_jedoga_controllerAI : public QuantumBasicAI
    {
        npc_jedoga_controllerAI(Creature* creature) : QuantumBasicAI(creature)
        {
			SetCombatMovement(false);
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        bool bRemoved;
        bool bRemoved2;
        bool bCasted;
        bool bCasted2;

        void EnterToBattle(Unit* /*who*/){}

        void AttackStart(Unit* /*victim*/){}

        void MoveInLineOfSight(Unit* /*who*/){}

		void Reset()
		{
			bRemoved = false;
            bRemoved2 = false;
            bCasted = false;
            bCasted2 = false;
		}

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!instance)
                return;

            if (!bRemoved && me->GetPositionX() > 440.0f)
            {
                if (instance->GetData(DATA_PRINCE_TALDARAM_EVENT) == DONE)
                {
                    me->InterruptNonMeleeSpells(true);
                    bRemoved = true;
                    return;
                }

                if (!bCasted)
                {
                    DoCast(me, SPELL_BEAM_VISUAL_JEDOGAS_AUFSEHER_1, false);
                    bCasted = true;
                }
            }
            if (!bRemoved2 && me->GetPositionX() < 440.0f)
            {
                if (!bCasted2 && instance->GetData(DATA_JEDOGA_TRIGGER_SWITCH))
                {
                    DoCast(me, SPELL_BEAM_VISUAL_JEDOGAS_AUFSEHER_2, false);
                    bCasted2 = true;
                }
                if (bCasted2 && !instance->GetData(DATA_JEDOGA_TRIGGER_SWITCH))
                {
                    me->InterruptNonMeleeSpells(true);
                    bCasted2 = false;
                }
                if (!bRemoved2 && instance->GetData(DATA_JEDOGA_SHADOWSEEKER_EVENT) == DONE)
                {
                    me->InterruptNonMeleeSpells(true);
                    bRemoved2 = true;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_jedoga_controllerAI(creature);
    }
};

void AddSC_boss_jedoga_shadowseeker()
{
    new boss_jedoga_shadowseeker();
    new npc_twilight_initiate();
    new npc_jedoga_controller();
}