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
#include "QuantumEscortAI.h"
#include "SpellScript.h"
#include "halls_of_reflection.h"

enum Texts
{
	SAY_LICH_KING_WALL_01              = -1594486,
    SAY_LICH_KING_WALL_02              = -1594491,
    SAY_LICH_KING_GNOUL                = -1594482,
    SAY_LICH_KING_ABON                 = -1594483,
    SAY_LICH_KING_WINTER               = -1594481,
    SAY_LICH_KING_END_DUN              = -1594504,
    SAY_LICH_KING_WIN                  = -1594485,
};

enum Spells
{
    SPELL_WINTER                       = 69780,
    SPELL_FURY_OF_FROSTMOURNE          = 70063,
    SPELL_SOUL_REAPER                  = 73797,
    SPELL_RAISE_DEAD                   = 69818,
    SPELL_ICE_PRISON                   = 69708,
    SPELL_DARK_ARROW                   = 70194,
    SPELL_HARVEST_SOUL                 = 70070,
    SPELL_EMERGE_VISUAL                = 50142,
    SPELL_GNOUL_JUMP                   = 70150,
    SPELL_COURSE_OF_DOOM               = 70144,
    H_SPELL_COURSE_OF_DOOM             = 70183,
    SPELL_SHADOW_BOLT_VOLLEY           = 70145,
    H_SPELL_SHADOW_BOLT_VOLLEY         = 70184,
    SPELL_SHADOW_BOLT                  = 70080,
    H_SPELL_SHADOW_BOLT                = 70182,
    SPELL_ABON_STRIKE                  = 40505,
    SPELL_VOMIT_SPRAY                  = 70176,
    H_SPELL_VOMIT_SPRAY                = 70181,
	SPELL_RAGING_GHOUL_SPAWN           = 69636,
	SPELL_RISEN_WITH_DOCTOR_SPAWN      = 69639,
};

class boss_lich_king_hor : public CreatureScript
{
public:
    boss_lich_king_hor() : CreatureScript("boss_lich_king_hor") { }

    struct boss_lich_king_horAI : public npc_escortAI
    {
        boss_lich_king_horAI(Creature* creature) : npc_escortAI(creature)
        {
            instance = (InstanceScript*)creature->GetInstanceScript();
            Reset();
        }

        InstanceScript* instance;
        uint32 Step;
        uint32 StepTimer;
        uint32 Wall;
        bool StartEscort;
        bool NonFight;
        float WalkSpeed;

        void Reset()
        {
			NonFight = false;
            StartEscort = false;
            WalkSpeed = 1.0f;
            Wall = 0;
        }

        void JustDied(Unit* /*killer*/) {}

        void WaypointReached(uint32 i)
        {
            if (instance->GetData(DATA_ICE_WALL_1) == IN_PROGRESS)
            {
                Wall = 1;
                SetEscortPaused(true);
            }

            if (instance->GetData(DATA_ICE_WALL_2) == IN_PROGRESS)
            {
                Wall = 2;
                SetEscortPaused(true);
            }

            if (instance->GetData(DATA_ICE_WALL_3) == IN_PROGRESS)
            {
                Wall = 3;
                SetEscortPaused(true);
            }

            if (instance->GetData(DATA_ICE_WALL_4) == IN_PROGRESS)
            {
                Wall = 4;
                SetEscortPaused(true);
            }

            switch(i)
            {
                case 66:
                    SetEscortPaused(true);
                    instance->SetData(DATA_LICH_KING_EVENT, SPECIAL);
                    DoSendQuantumText(SAY_LICH_KING_END_DUN, me);
                    if (Creature* Lider = ((Creature*)Unit::GetUnit(*me, instance->GetData64(DATA_ESCAPE_LIDER))))
					{
                        me->CastSpell(Lider, SPELL_HARVEST_SOUL, false);
					}
                    me->SetActive(false);
                    break;
			}
		}

        void AttackStart(Unit* who)
        {
            if (!instance || !who)
                return;

            if (NonFight)
                return;

            if (instance->GetData(DATA_LICH_KING_EVENT) == IN_PROGRESS || who->GetTypeId() == TYPE_ID_PLAYER)
                return;

            npc_escortAI::AttackStart(who);
        }

        void JustSummoned(Creature* summoned)
        {
            if (!instance || !summoned)
                return;

            summoned->SetInCombatWithZone();
            summoned->SetActive(true);

            instance->SetData(DATA_SUMMONS, 1);
            if (Unit* Lider = Unit::GetUnit(*me, instance->GetData64(DATA_ESCAPE_LIDER)))
            {
                summoned->GetMotionMaster()->MoveChase(Lider);
                summoned->AddThreat(Lider, 100.0f);
            }
        }

        void CallGuard(uint32 GuardID)
        {
			me->SummonCreature(GuardID,(me->GetPositionX()-5)+rand()%10, (me->GetPositionY()-5)+rand()%10, me->GetPositionZ(),4.17f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,360000);
        }

        void Wall01()
        {
            switch(Step)
            {
                case 0:
                    instance->SetData(DATA_SUMMONS, 3);
                    DoSendQuantumText(SAY_LICH_KING_WALL_01, me);
                    StepTimer = 2000;
                    ++Step;
                    break;
                case 1:
                    DoCast(me, SPELL_RAISE_DEAD);
                    DoSendQuantumText(SAY_LICH_KING_GNOUL, me);
                    StepTimer = 7000;
                    ++Step;
                    break;
                case 2:
                    DoCast(me, SPELL_WINTER);
                    DoSendQuantumText(SAY_LICH_KING_WINTER, me);
                    me->SetSpeed(MOVE_WALK, WalkSpeed, true);
                    StepTimer = 1000;
                    ++Step;
                    break;
                case 3:
                    StepTimer = 2000;
                    ++Step;
                    break;
                case 4:
                    CallGuard(NPC_RISEN_WITCH_DOCTOR);
                    instance->SetData(DATA_ICE_WALL_1, DONE);
                    StepTimer = 100;
                    Step = 0;
                    Wall = 0;
                    SetEscortPaused(false);
                    break;
            }
        }

        void Wall02()
        {
            switch(Step)
            {
                case 0:
                    instance->SetData(DATA_SUMMONS, 3);
                    DoSendQuantumText(SAY_LICH_KING_GNOUL, me);
                    DoCast(me, SPELL_RAISE_DEAD);
                    StepTimer = 6000;
                    ++Step;
                    break;
                case 1:
                    CallGuard(NPC_RISEN_WITCH_DOCTOR);
                    CallGuard(NPC_RISEN_WITCH_DOCTOR);
                    CallGuard(NPC_LUMBERING_ABOMINATION);
                    instance->SetData(DATA_ICE_WALL_2, DONE);
                    StepTimer = 5000;
                    Step = 0;
                    Wall = 0;
                    SetEscortPaused(false);
                    break;
            }
        }

        void Wall03()
        {
            switch(Step)
            {
                case 0:
                    instance->SetData(DATA_SUMMONS, 3);
                    DoCast(me, SPELL_RAISE_DEAD);
                    DoSendQuantumText(SAY_LICH_KING_GNOUL, me);
                    StepTimer = 6000;
                    ++Step;
                    break;
                case 1:
                    DoSendQuantumText(SAY_LICH_KING_ABON, me);
                    CallGuard(NPC_RISEN_WITCH_DOCTOR);
                    CallGuard(NPC_RISEN_WITCH_DOCTOR);
                    CallGuard(NPC_RISEN_WITCH_DOCTOR);
                    CallGuard(NPC_LUMBERING_ABOMINATION);
                    CallGuard(NPC_LUMBERING_ABOMINATION);
                    instance->SetData(DATA_ICE_WALL_3, DONE);
                    StepTimer = 5000;
                    Step = 0;
                    Wall = 0;
                    SetEscortPaused(false);
                    break;
            }
        }

        void Wall04()
        {
            switch(Step)
            {
                case 0:
                    instance->SetData(DATA_SUMMONS, 3);
                    DoCast(me, SPELL_RAISE_DEAD);
                    DoSendQuantumText(SAY_LICH_KING_GNOUL, me);
                    StepTimer = 6000;
                    ++Step;
                    break;
                case 1:
                    CallGuard(NPC_RISEN_WITCH_DOCTOR);
                    CallGuard(NPC_RISEN_WITCH_DOCTOR);
                    CallGuard(NPC_RISEN_WITCH_DOCTOR);
                    CallGuard(NPC_LUMBERING_ABOMINATION);
                    CallGuard(NPC_LUMBERING_ABOMINATION);
                    StepTimer = 15000;
                    ++Step;
                    break;
                case 2:
                    DoSendQuantumText(SAY_LICH_KING_ABON, me);
                    CallGuard(NPC_RISEN_WITCH_DOCTOR);
                    CallGuard(NPC_RISEN_WITCH_DOCTOR);
                    instance->SetData(DATA_ICE_WALL_4, DONE);
                    Wall = 0;
                    SetEscortPaused(false);
                    ++Step;
                    break;
            }
        }

        void UpdateEscortAI(const uint32 diff)
        {
            if (instance->GetData(DATA_LICH_KING_EVENT) == NOT_STARTED || instance->GetData(DATA_LICH_KING_EVENT) == FAIL)
            {
                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }

            if (me->IsInCombatActive() && instance->GetData(DATA_LICH_KING_EVENT) == IN_PROGRESS)
                npc_escortAI::EnterEvadeMode();

            // Start chase for leader
            if (instance->GetData(DATA_LICH_KING_EVENT) == IN_PROGRESS && StartEscort != true)
            {
                StartEscort = true;
                me->RemoveAurasDueToSpell(SPELL_ICE_PRISON);
                me->RemoveAurasDueToSpell(SPELL_DARK_ARROW);
                me->SetActive(true);
                NonFight = true;
                me->AttackStop();
                me->SetSpeed(MOVE_WALK, 2.5f, true);
                Start(false, false);
                Step = 0;
                StepTimer = 100;
            }

            // Leader caught, wipe
            if (Creature* Lider = ((Creature*)Unit::GetUnit(*me, instance->GetData64(DATA_ESCAPE_LIDER))))
            {
                if (Lider->IsWithinDistInMap(me, 2.0f) && instance->GetData(DATA_LICH_KING_EVENT) == IN_PROGRESS)
                {
                    me->SetActive(false);
                    SetEscortPaused(false);
                    me->StopMoving();
                    DoSendQuantumText(SAY_LICH_KING_WIN, me);
                    me->CastSpell(me, SPELL_FURY_OF_FROSTMOURNE, false);
                    me->DealDamage(Lider, Lider->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                }
            }

            if (Wall == 1)
            {
                if (StepTimer <= diff)
                    Wall01();
                else
                    StepTimer -= diff;
            }

            if (Wall == 2)
            {
                if (StepTimer <= diff)
                    Wall02();
                else
                    StepTimer -= diff;
            }

            if (Wall == 3)
            {
                if (StepTimer <= diff)
                    Wall03();
                else
                    StepTimer -= diff;
            }

            if (Wall == 4)
            {
                if (StepTimer <= diff)
                    Wall04();
                else
                    StepTimer -= diff;
            }
            return;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_lich_king_horAI(creature);
    }
};

class npc_raging_gnoul : public CreatureScript
{
public:
    npc_raging_gnoul() : CreatureScript("npc_raging_gnoul") { }

    struct npc_raging_gnoulAI : public QuantumBasicAI
    {
        npc_raging_gnoulAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = (InstanceScript*)creature->GetInstanceScript();
            me->SetActive(true);
            Reset();
        }

        InstanceScript* instance;
        uint32 EmergeTimer;

        bool Emerge;
        bool Jumped;
        uint64 LiderGUID;

        void Reset()
        {
            DoCast(me, SPELL_EMERGE_VISUAL, true);
			DoCast(me, SPELL_RAGING_GHOUL_SPAWN, true);

            EmergeTimer = 4000;

            Emerge = false;
            Jumped = false;
        }

        void JustDied(Unit* /*killer*/)
        {
            instance->SetData(DATA_SUMMONS, 0);
        }

        void AttackStart(Unit* who)
        {
            if (!who)
                return;

            if (Emerge == false)
                return;

            QuantumBasicAI::AttackStart(who);
        }

        void UpdateAI(const uint32 diff)
        {
            if (instance->GetData(DATA_LICH_KING_EVENT) == IN_PROGRESS)
            {
                LiderGUID = instance->GetData64(DATA_ESCAPE_LIDER);
                Creature* Lider = ((Creature*)Unit::GetUnit(*me, LiderGUID));

                if (Emerge != true)
                {
                    if (EmergeTimer <= diff)
                    {
                        Emerge = true;
                        LiderGUID = instance->GetData64(DATA_ESCAPE_LIDER);
                        if (Lider)
                        {
                            DoResetThreat();
                            me->AI()->AttackStart(Lider);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveChase(Lider);
                        }
                    }
                    else EmergeTimer -= diff;
                }

                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 50.0f))
                {
                    if (!Jumped && me->IsWithinDistInMap(target, 30.0f) && !me->IsWithinDistInMap(target, 5.0f))
                    {
                        Jumped = true;
                        DoCast(target, SPELL_GNOUL_JUMP);
                    }
                }
            }
            else if (instance->GetData(DATA_LICH_KING_EVENT) == FAIL || instance->GetData(DATA_LICH_KING_EVENT) == NOT_STARTED)
                me->DespawnAfterAction();

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_raging_gnoulAI(creature);
    }
};

class npc_risen_witch_doctor : public CreatureScript
{
public:
	npc_risen_witch_doctor() : CreatureScript("npc_risen_witch_doctor") { }

    struct npc_risen_witch_doctorAI : public QuantumBasicAI
    {
        npc_risen_witch_doctorAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = (InstanceScript*)creature->GetInstanceScript();
            me->SetActive(true);
            Reset();
        }

        InstanceScript* instance;
        uint32 EmergeTimer;
        bool Emerge;
        uint64 LiderGUID;
        uint32 BoltTimer;
        uint32 BoltVolleyTimer;
        uint32 CurseTimer;

        void Reset()
        {
            DoCast(me, SPELL_EMERGE_VISUAL, true);
			DoCast(me, SPELL_RISEN_WITH_DOCTOR_SPAWN, true);
            EmergeTimer = 5000;
            BoltTimer = 6000;
            BoltVolleyTimer = 15000;
            CurseTimer = 7000;
            Emerge = false;
        }

        void JustDied(Unit* /*killer*/)
        {
            instance->SetData(DATA_SUMMONS, 0);
        }

        void AttackStart(Unit* who)
        {
            if (!who)
                return;

            if (Emerge == false)
                return;

            QuantumBasicAI::AttackStart(who);
        }

        void UpdateAI(const uint32 diff)
        {
            if (instance->GetData(DATA_LICH_KING_EVENT) == IN_PROGRESS)
            {
                if (Emerge != true)
                {
                    if (EmergeTimer <= diff)
                    {
                        Emerge = true;
                        LiderGUID = instance->GetData64(DATA_ESCAPE_LIDER);
                        if (Creature* Lider = ((Creature*)Unit::GetUnit(*me, LiderGUID)))
                        {
                            DoResetThreat();
                            me->AI()->AttackStart(Lider);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveChase(Lider);
                        }
                    }
                    else EmergeTimer -= diff;
                }

                if (CurseTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, SPELL_COURSE_OF_DOOM);
						CurseTimer = urand(10000, 15000);
					}
                }
				else CurseTimer -= diff;

                if (BoltTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_TOP_AGGRO))
					{
                        DoCast(target, SPELL_SHADOW_BOLT);
						BoltTimer = urand(2000, 3000);
					}
                }
				else BoltTimer -= diff;

                if (BoltVolleyTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_TOP_AGGRO))
					{
                        DoCast(target, SPELL_SHADOW_BOLT_VOLLEY);
						BoltVolleyTimer = urand(15000, 22000);
					}
                }
				else BoltVolleyTimer -= diff;
            }
            else if (instance->GetData(DATA_LICH_KING_EVENT) == FAIL || instance->GetData(DATA_LICH_KING_EVENT) == NOT_STARTED)
                me->DespawnAfterAction();

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_risen_witch_doctorAI(creature);
    }
};

class npc_lumbering_abomination : public CreatureScript
{
public:
    npc_lumbering_abomination() : CreatureScript("npc_lumbering_abomination") { }

    struct npc_lumbering_abominationAI : public QuantumBasicAI
    {
        npc_lumbering_abominationAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = (InstanceScript*)creature->GetInstanceScript();
            me->SetActive(true);
            Reset();
        }

        InstanceScript* instance;
        uint64 LiderGUID;
        bool Walk;
        uint32 StrikeTimer;
        uint32 VomitTimer;

        void Reset()
        {
            Walk = false;
            VomitTimer = 15000;
            StrikeTimer = 6000;
        }

        void UpdateAI(const uint32 diff)
        {
            if (instance->GetData(DATA_LICH_KING_EVENT) == IN_PROGRESS)
            {
                if (Walk != true)
                {
                    Walk = true;
                    LiderGUID = instance->GetData64(DATA_ESCAPE_LIDER);
                    if (Creature* Lider = ((Creature*)Unit::GetUnit(*me, LiderGUID)))
                    {
                        DoResetThreat();
                        me->AI()->AttackStart(Lider);
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveChase(Lider);
                    }
                }

                if (StrikeTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_TOP_AGGRO))
					{
                        DoCast(target, SPELL_ABON_STRIKE);
						StrikeTimer = urand(7000, 9000);
					}
                }
				else StrikeTimer -= diff;

                if (VomitTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_TOP_AGGRO))
					{
                        DoCast(target, SPELL_VOMIT_SPRAY);
						VomitTimer = urand(15000, 20000);
					}
                }
				else VomitTimer -= diff;
            }
            else
				if (instance->GetData(DATA_LICH_KING_EVENT) == FAIL || instance->GetData(DATA_LICH_KING_EVENT) == NOT_STARTED)
					me->DespawnAfterAction();
			DoMeleeAttackIfReady();
        }

        void JustDied(Unit* /*killer*/)
        {
            instance->SetData(DATA_SUMMONS, 0);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
		return new npc_lumbering_abominationAI(creature);
    }
};

void AddSC_boss_lich_king_hr()
{
    new boss_lich_king_hor();
    new npc_raging_gnoul();
    new npc_risen_witch_doctor();
    new npc_lumbering_abomination();
}