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
#include "violet_hold.h"

enum Texts
{
    SAY_AGGRO   = -1608018,
    SAY_SLAY_1  = -1608019,
    SAY_SLAY_2  = -1608020,
    SAY_SLAY_3  = -1608021,
    SAY_DEATH   = -1608022,
    SAY_SPAWN   = -1608023,
    SAY_ENRAGE  = -1608024,
    SAY_SHATTER = -1608025,
    SAY_BUBBLE  = -1608026,
};

enum Spells
{
    SPELL_DRAINED              = 59820,
    SPELL_FRENZY_5N            = 54312,
    SPELL_FRENZY_5H            = 59522,
    SPELL_PROTECTIVE_BUBBLE    = 54306,
    SPELL_WATER_BLAST_5N       = 54237,
    SPELL_WATER_BLAST_5H       = 59520,
    SPELL_WATER_BOLT_VOLLEY_5N = 54241,
    SPELL_WATER_BOLT_VOLLEY_5H = 59521,
    SPELL_SPLASH               = 59516,
    SPELL_BURST                = 54379,
    SPELL_WATER_GLOBULE        = 54268,
};

enum Actions
{
    ACTION_WATER_ELEMENT_HIT                  = 1,
    ACTION_WATER_ELEMENT_KILLED               = 2,
};

#define DATA_DEHYDRATION 1
#define MAX_SPAWN_LOC    5

static Position SpawnLoc[MAX_SPAWN_LOC]=
{
    {1840.64f, 795.407f, 44.079f, 1.676f},
    {1886.24f, 757.733f, 47.750f, 5.201f},
    {1877.91f, 845.915f, 43.417f, 3.560f},
    {1918.97f, 850.645f, 47.225f, 4.136f},
    {1935.50f, 796.224f, 52.492f, 4.224f},
};

class boss_ichoron : public CreatureScript
{
public:
    boss_ichoron() : CreatureScript("boss_ichoron") { }

    struct boss_ichoronAI : public QuantumBasicAI
    {
        boss_ichoronAI(Creature* creature) : QuantumBasicAI(creature), WaterElements(creature)
        {
            instance  = creature->GetInstanceScript();
        }

        bool IsExploded;
        bool IsFrenzy;
        bool Dehydration;

        uint32 BubbleCheckerTimer;
        uint32 WaterBoltVolleyTimer;
        uint32 WaterBlastTimer;

        InstanceScript* instance;

        SummonList WaterElements;

        void Reset()
        {
            BubbleCheckerTimer = 1000;
            WaterBoltVolleyTimer = 500;
            WaterBlastTimer = 2000;

			IsExploded = false;
            IsFrenzy = false;
            Dehydration = true;

            me->SetVisible(true);
            DespawnWaterElements();

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			if (instance->GetData(DATA_WAVE_COUNT) == 6)
				instance->SetData(DATA_1ST_BOSS_EVENT, NOT_STARTED);
			else if (instance->GetData(DATA_WAVE_COUNT) == 12)
				instance->SetData(DATA_2ND_BOSS_EVENT, NOT_STARTED);
        }

		void MoveInLineOfSight(Unit* /*who*/) {}

		void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

            DoCast(me, SPELL_PROTECTIVE_BUBBLE);
			
			if (GameObject* Door = instance->instance->GetGameObject(instance->GetData64(DATA_ICHORON_CELL)))
			{
				if (Door->GetGoState() == GO_STATE_READY)
				{
					EnterEvadeMode();
					return;
				}

				if (instance->GetData(DATA_WAVE_COUNT) == 6)
					instance->SetData(DATA_1ST_BOSS_EVENT, IN_PROGRESS);
				else if (instance->GetData(DATA_WAVE_COUNT) == 12)
					instance->SetData(DATA_2ND_BOSS_EVENT, IN_PROGRESS);
			}
		}

        void AttackStart(Unit* who)
        {
			if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER || UNIT_FLAG_NON_ATTACKABLE))
                return;

            if (me->Attack(who, true))
            {
                me->AddThreat(who, 0.0f);
                me->SetInCombatWith(who);
                who->SetInCombatWith(me);
                DoStartMovement(who);
            }
        }

		void KilledUnit(Unit* victim)
        {
            if (victim == me)
                return;

            DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
        }

		void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

            if (IsExploded)
            {
                IsExploded = false;
                me->SetVisible(true);
            }

            DespawnWaterElements();

			if (instance->GetData(DATA_WAVE_COUNT) == 6)
			{
				if (IsHeroic() && instance->GetData(DATA_1ST_BOSS_EVENT) == DONE)
					me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

				instance->SetData(DATA_1ST_BOSS_EVENT, DONE);
				instance->SetData(DATA_WAVE_COUNT, 7);
			}
			else if (instance->GetData(DATA_WAVE_COUNT) == 12)
			{
				if (IsHeroic() && instance->GetData(DATA_2ND_BOSS_EVENT) == DONE)
					me->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

				instance->SetData(DATA_2ND_BOSS_EVENT, DONE);
				instance->SetData(DATA_WAVE_COUNT, 13);
			}
        }

		void JustSummoned(Creature* summoned)
        {
            if (summoned)
            {
                summoned->SetSpeed(MOVE_RUN, 0.3f);
                summoned->GetMotionMaster()->MoveFollow(me, 0, 0);
                WaterElements.push_back(summoned->GetGUID());
            }
        }

        void SummonedCreatureDespawn(Creature* summoned)
        {
            if (summoned)
                WaterElements.remove(summoned->GetGUID());
        }

        void DoAction(int32 const param)
        {
            if (!me->IsAlive())
                return;

			switch (param)
            {
                case ACTION_WATER_ELEMENT_HIT:
                    me->ModifyHealth(int32(me->CountPctFromMaxHealth(1)));
                    if (IsExploded)
                        DoExplodeCompleted();
                    Dehydration = false;
                    break;
                case ACTION_WATER_ELEMENT_KILLED:
                    uint32 damage = me->CountPctFromMaxHealth(3);
                    if (me->GetHealth() > damage)
                    {
                        me->ModifyHealth(-int32(damage));
                        me->LowerPlayerDamageReq(damage);
                    }
                    break;
			}
		}

        void DespawnWaterElements()
        {
            WaterElements.DespawnAll();
        }

        void DoExplodeCompleted()
        {
            IsExploded = false;

            if (!HealthBelowPct(HEALTH_PERCENT_25))
            {
                DoSendQuantumText(SAY_BUBBLE, me);
                DoCast(me, SPELL_PROTECTIVE_BUBBLE, true);
            }

            me->SetVisible(true);
            me->GetMotionMaster()->MoveChase(me->GetVictim());
        }

        uint32 GetData(uint32 type)
        {
            if (type == DATA_DEHYDRATION)
                return Dehydration ? 1 : 0;

            return 0;
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (!IsFrenzy && HealthBelowPct(HEALTH_PERCENT_25) && !IsExploded)
            {
                DoSendQuantumText(SAY_ENRAGE, me);
                DoCast(me, DUNGEON_MODE(SPELL_FRENZY_5N, SPELL_FRENZY_5H), true);
                IsFrenzy = true;
            }

            if (!IsFrenzy)
            {
                if (BubbleCheckerTimer <= diff)
                {
                    if (!IsExploded)
                    {
                        if (!me->HasAura(SPELL_PROTECTIVE_BUBBLE, 0))
                        {
                            DoSendQuantumText(SAY_SHATTER, me);
                            DoCast(me, SPELL_DRAINED, true);
                            DoCast(me, SPELL_BURST, true);
                            IsExploded = true;
                            me->AttackStop();
                            me->SetVisible(false);
                            for (uint8 i = 0; i < 10; i++)
                            {
                                int tmp = urand(0, MAX_SPAWN_LOC-1);
                                me->SummonCreature(NPC_ICHOR_GLOBULE, SpawnLoc[tmp], TEMPSUMMON_CORPSE_DESPAWN);
                            }
                        }
                    }
                    else
                    {
                        bool bIsWaterElementsAlive = false;

                        if (!WaterElements.empty())
                        {
                            for (std::list<uint64>::const_iterator itr = WaterElements.begin(); itr != WaterElements.end(); ++itr)
							{
                                if (Creature* pTemp = Unit::GetCreature(*me, *itr))
								{
                                    if (pTemp->IsAlive())
                                    {
                                        bIsWaterElementsAlive = true;
                                        break;
                                    }
								}
							}
                        }

                        if (!bIsWaterElementsAlive)
                            DoExplodeCompleted();
                    }
                    BubbleCheckerTimer = 1000;
                }
                else BubbleCheckerTimer -= diff;
            }

			if (WaterBlastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_WATER_BLAST_5N, SPELL_WATER_BLAST_5H));
					WaterBlastTimer = urand(3000, 4000);
				}
			}
			else WaterBlastTimer -= diff;

            if (!IsExploded)
            {
                if (WaterBoltVolleyTimer <= diff)
                {
					DoCastAOE(DUNGEON_MODE(SPELL_WATER_BOLT_VOLLEY_5N, SPELL_WATER_BOLT_VOLLEY_5H));
					WaterBoltVolleyTimer = urand(6000, 7000);
                }
                else WaterBoltVolleyTimer -= diff;

                DoMeleeAttackIfReady();
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_ichoronAI(creature);
    }
};

class npc_ichor_globule : public CreatureScript
{
public:
    npc_ichor_globule() : CreatureScript("npc_ichor_globule") { }

    struct npc_ichor_globuleAI : public QuantumBasicAI
    {
        npc_ichor_globuleAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 RangeCheckTimer;

        void Reset()
        {
            RangeCheckTimer = 1*IN_MILLISECONDS;

            DoCast(me, SPELL_WATER_GLOBULE, true);
        }

        void AttackStart(Unit* /*who*/)
        {
            return;
        }

		void JustDied(Unit* /*killer*/)
        {
            DoCastAOE(SPELL_SPLASH, true);

            if (Creature* Ichoron = Unit::GetCreature(*me, instance->GetData64(DATA_ICHORON)))
			{
                if (Ichoron->AI())
                    Ichoron->AI()->DoAction(ACTION_WATER_ELEMENT_KILLED);
			}
        }

        void UpdateAI(uint32 const diff)
        {
            if (RangeCheckTimer < diff)
            {
				if (Creature* Ichoron = Unit::GetCreature(*me, instance->GetData64(DATA_ICHORON)))
				{
					if (me->IsWithinDist(Ichoron, 2.0f, false))
					{
						if (Ichoron->AI())
							Ichoron->AI()->DoAction(ACTION_WATER_ELEMENT_HIT);

						me->DespawnAfterAction();
					}
				}
                RangeCheckTimer = 1*IN_MILLISECONDS;
            }
            else RangeCheckTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ichor_globuleAI (creature);
    }
};

class achievement_dehydration : public AchievementCriteriaScript
{
public:
	achievement_dehydration() : AchievementCriteriaScript("achievement_dehydration") { }

	bool OnCheck(Player* /*player*/, Unit* target)
	{
		if (!target)
			return false;

		if (Creature* Ichoron = target->ToCreature())
			if (Ichoron->AI()->GetData(DATA_DEHYDRATION))
				return true;

		return false;
	}
};

void AddSC_boss_ichoron()
{
    new boss_ichoron();
    new npc_ichor_globule();
    new achievement_dehydration();
}