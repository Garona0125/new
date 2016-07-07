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
    SAY_SKARVALD_AGGRO                         = -1574011,
    SAY_SKARVALD_DAL_DIED                      = -1574012,
    SAY_SKARVALD_SKA_DIED_FIRST                = -1574013,
    SAY_SKARVALD_KILL                          = -1574014,
    SAY_SKARVALD_DAL_DIED_FIRST                = -1574015,

    SAY_DALRONN_AGGRO                          = -1574016,
    SAY_DALRONN_SKA_DIED                       = -1574017,
    SAY_DALRONN_DAL_DIED_FIRST                 = -1574018,
    SAY_DALRONN_KILL                           = -1574019,
    SAY_DALRONN_SKA_DIED_FIRST                 = -1574020,
};

enum Spells
{
	SPELL_CHARGE_5N                            = 43651,
	SPELL_CHARGE_5H                            = 59611,
    SPELL_STONE_STRIKE                         = 48583,
    SPELL_SUMMON_SKARVALD_GHOST                = 48613,
	SPELL_ENRAGE                               = 48193,
    SPELL_SHADOW_BOLT_5N                       = 43649,
    SPELL_SHADOW_BOLT_5H                       = 59575,
    SPELL_SUMMON_SKELETONS                     = 52611,
    SPELL_DEBILITATE_5N                        = 43650,
	SPELL_DEBILITATE_5H                        = 59577,
    SPELL_SUMMON_DALRONN_GHOST                 = 48612,
};

class boss_skarvald_the_constructor : public CreatureScript
{
public:
    boss_skarvald_the_constructor() : CreatureScript("boss_skarvald_the_constructor") { }

    struct boss_skarvald_the_constructorAI : public QuantumBasicAI
    {
        boss_skarvald_the_constructorAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();

			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        }

        InstanceScript* instance;

        bool ghost;
        uint32 ChargeTimer;
        uint32 StoneStrikeTimer;
        uint32 ResponseTimer;
        uint32 CheckTimer;
        bool DalronnIsDead;
		bool Enraged;

        void Reset()
        {
            ChargeTimer = 5000;
            StoneStrikeTimer = 10000;
            DalronnIsDead = false;
            CheckTimer = 5000;
			Enraged = false;

            me->RemoveLootMode(1);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

            ghost = (me->GetEntry() == NPC_SKARVALD_GHOST);
            if (!ghost && instance)
            {
                Unit* dalronn = Unit::GetUnit(*me, instance->GetData64(DATA_DALRONN));
                if (dalronn && dalronn->IsDead())
                    CAST_CRE(dalronn)->Respawn();

                instance->SetData(DATA_SKARVALD_DALRONN_EVENT, NOT_STARTED);
            }
        }

        void EnterToBattle(Unit* who)
        {
            if (!ghost && instance)
            {
                DoSendQuantumText(SAY_SKARVALD_AGGRO, me);

                Unit* dalronn = Unit::GetUnit(*me, instance->GetData64(DATA_DALRONN));
                if (dalronn && dalronn->IsAlive() && !dalronn->GetVictim())
                    dalronn->getThreatManager().addThreat(who, 0.0f);

                instance->SetData(DATA_SKARVALD_DALRONN_EVENT, IN_PROGRESS);
            }
        }

		void DamageTaken(Unit* /*attacker*/, uint32& damage)
		{
			if (!Enraged && !ghost && me->HealthBelowPctDamaged(15, damage))
			{
				Enraged = true;
				DoCast(me, SPELL_ENRAGE);
			}
		}

        void JustDied(Unit* killer)
        {
            if (!ghost && instance)
            {
                Unit* dalronn = Unit::GetUnit(*me, instance->GetData64(DATA_DALRONN));
                if (dalronn)
                {
                    if (dalronn->IsDead())
                    {
                        DoSendQuantumText(SAY_SKARVALD_DAL_DIED, me);

                        instance->SetData(DATA_SKARVALD_DALRONN_EVENT, DONE);
                    }
                    else
                    {
                        DoSendQuantumText(SAY_SKARVALD_SKA_DIED_FIRST, me);

                        dalronn->ToCreature()->AddLootMode(1);
                        //me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                        //DoCast(me, SPELL_SUMMON_SKARVALD_GHOST, true);
                        Creature* temp = me->SummonCreature(NPC_SKARVALD_GHOST, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 5000);
                        if (temp)
                        {
                            temp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            temp->AI()->AttackStart(killer);
                        }
                    }
                }
            }
        }

        void KilledUnit(Unit* /*victim*/)
        {
            if (!ghost)
                DoSendQuantumText(SAY_SKARVALD_KILL, me);
        }

        void UpdateAI(const uint32 diff)
        {
            if (ghost)
            {
                if (instance && instance->GetData(DATA_SKARVALD_DALRONN_EVENT) != IN_PROGRESS)
                    me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }

            if (!UpdateVictim())
                return;

            if (!ghost)
            {
                if (CheckTimer)
                {
                    if (CheckTimer <= diff)
                    {
                        CheckTimer = 5000;
                        Unit* dalronn = Unit::GetUnit(*me, instance ? instance->GetData64(DATA_DALRONN) : 0);
                        if (dalronn && dalronn->IsDead())
                        {
                            DalronnIsDead = true;
                            ResponseTimer = 2000;
                            CheckTimer = 0;
                        }
                    }
					else CheckTimer -= diff;
                }
                if (ResponseTimer && DalronnIsDead)
                {
                    if (ResponseTimer <= diff)
                    {
                        DoSendQuantumText(SAY_SKARVALD_DAL_DIED_FIRST, me);

                        ResponseTimer = 0;
                    }
					else ResponseTimer -= diff;
                }
            }

            if (ChargeTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target,  DUNGEON_MODE(SPELL_CHARGE_5N, SPELL_CHARGE_5H), true);
					ChargeTimer = 5000+rand()%5000;
				}
            }
			else ChargeTimer -= diff;

            if (StoneStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_STONE_STRIKE);
                StoneStrikeTimer = 5000+rand()%5000;
            }
			else StoneStrikeTimer -= diff;

            if (!me->HasUnitState(UNIT_STATE_CASTING))
				DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_skarvald_the_constructorAI(creature);
    }
};

class boss_dalronn_the_controller : public CreatureScript
{
public:
    boss_dalronn_the_controller() : CreatureScript("boss_dalronn_the_controller") { }

    struct boss_dalronn_the_controllerAI : public QuantumBasicAI
    {
        boss_dalronn_the_controllerAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        bool ghost;
        uint32 ShadowBoltTimer;
        uint32 DebilitateTimer;
        uint32 SummonTimer;

        uint32 ResponseTimer;
        uint32 CheckTimer;
        uint32 AggroYellTimer;
        bool SkarvaldIsDead;

        void Reset()
        {
            ShadowBoltTimer = 1000;
            DebilitateTimer = 5000;
            SummonTimer = 10000;
            CheckTimer = 5000;
            SkarvaldIsDead = false;
            AggroYellTimer = 0;

            me->RemoveLootMode(1);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

            ghost = me->GetEntry() == NPC_DALRONN_GHOST;
            if (!ghost && instance)
            {
                Unit* skarvald = Unit::GetUnit(*me, instance->GetData64(DATA_SKARVALD));
                if (skarvald && skarvald->IsDead())
                    CAST_CRE(skarvald)->Respawn();

                instance->SetData(DATA_SKARVALD_DALRONN_EVENT, NOT_STARTED);
            }
        }

        void EnterToBattle(Unit* who)
        {
            if (!ghost && instance)
            {
                Unit* skarvald = Unit::GetUnit(*me, instance->GetData64(DATA_SKARVALD));
                if (skarvald && skarvald->IsAlive() && !skarvald->GetVictim())
                    skarvald->getThreatManager().addThreat(who, 0.0f);

                AggroYellTimer = 5000;

				instance->SetData(DATA_SKARVALD_DALRONN_EVENT, IN_PROGRESS);
            }
        }

        void JustDied(Unit* Killer)
        {
            if (!ghost && instance)
            {
                Unit* skarvald = Unit::GetUnit(*me, instance->GetData64(DATA_SKARVALD));
                if (skarvald)
                {
                    if (skarvald->IsDead())
                    {
                        DoSendQuantumText(SAY_DALRONN_SKA_DIED, me);

						instance->SetData(DATA_SKARVALD_DALRONN_EVENT, DONE);
                    }
                    else
                    {
                        DoSendQuantumText(SAY_DALRONN_DAL_DIED_FIRST, me);

                        skarvald->ToCreature()->AddLootMode(1);
                        //me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
                        //DoCast(me, SPELL_SUMMON_DALRONN_GHOST, true);
                        Creature* temp = me->SummonCreature(NPC_DALRONN_GHOST, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 5000);
                        if (temp)
                        {
                            temp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            temp->AI()->AttackStart(Killer);
                        }
                    }
                }
            }
        }

        void KilledUnit(Unit* /*victim*/)
        {
            if (!ghost)
            {
                DoSendQuantumText(SAY_DALRONN_KILL, me);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (ghost)
            {
                if (instance && instance->GetData(DATA_SKARVALD_DALRONN_EVENT) != IN_PROGRESS)
                    me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }

            if (!UpdateVictim())
                return;

            if (AggroYellTimer)
            {
                if (AggroYellTimer <= diff)
                {
                    DoSendQuantumText(SAY_DALRONN_AGGRO, me);
                    AggroYellTimer = 0;
                }
				else AggroYellTimer -= diff;
            }

            if (!ghost)
            {
                if (CheckTimer)
                {
                    if (CheckTimer <= diff)
                    {
                        CheckTimer = 5000;
                        Unit* skarvald = Unit::GetUnit(*me, instance ? instance->GetData64(DATA_SKARVALD) : 0);
                        if (skarvald && skarvald->IsDead())
                        {
                            SkarvaldIsDead = true;
                            ResponseTimer = 2000;
                            CheckTimer = 0;
                        }
                    }
					else CheckTimer -= diff;
                }

                if (ResponseTimer && SkarvaldIsDead)
                {
                    if (ResponseTimer <= diff)
                    {
                        DoSendQuantumText(SAY_DALRONN_SKA_DIED_FIRST, me);
                        ResponseTimer = 0;
                    }
					else ResponseTimer -= diff;
                }
            }

            if (ShadowBoltTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BOLT_5N, SPELL_SHADOW_BOLT_5H));
						ShadowBoltTimer = 2100;
					}
                }
            }
			else ShadowBoltTimer -= diff;

            if (DebilitateTimer <= diff)
            {
                if (!me->IsNonMeleeSpellCasted(false))
                {
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, DUNGEON_MODE(SPELL_DEBILITATE_5N, SPELL_DEBILITATE_5H));
						DebilitateTimer = 5000+rand()%5000;
					}
                }
            }
			else DebilitateTimer -= diff;

            if (IsHeroic())
            {
                if (SummonTimer <= diff)
                {
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        DoCast(me, SPELL_SUMMON_SKELETONS);
                        SummonTimer = (rand()%10000) + 20000;
                    }
                }
				else SummonTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_dalronn_the_controllerAI(creature);
    }
};

void AddSC_boss_skarvald_dalronn()
{
    new boss_skarvald_the_constructor();
    new boss_dalronn_the_controller();
}