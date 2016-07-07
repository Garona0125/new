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
    SAY_AGGRO                    = -1574000,
    SAY_FROST_TOMB               = -1574001,
    SAY_SKELETONS                = -1574002,
    SAY_KILL                     = -1574003,
    SAY_DEATH                    = -1574004,
};

enum Spells
{
    SPELL_SHADOWBOLT_5N          = 43667,
    SPELL_SHADOWBOLT_5H          = 59389,
    SPELL_FROST_TOMB             = 48400,
    SPELL_FROST_TOMB_SUMMON      = 42714,
	SPELL_DECREPIFY_5N           = 42702,
	SPELL_DECREPIFY_5H           = 59397,
    SPELL_SCOURGE_RESSURRECTION  = 42704,
};

#define SKELETONSPAWN_Z  42.8668f

float SkeletonSpawnPoint[5][5] =
{
    {156.2559f, 259.2093f},
    {156.2559f, 259.2093f},
    {156.2559f, 259.2093f},
    {156.2559f, 259.2093f},
    {156.2559f, 259.2093f},
};

float AttackLoc[3] = {197.636f, 194.046f, 40.8164f};

bool ShatterFrostTomb;

class boss_keleseth : public CreatureScript
{
public:
    boss_keleseth() : CreatureScript("boss_keleseth") { }

    struct boss_kelesethAI : public QuantumBasicAI
    {
        boss_kelesethAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 FrostTombTimer;
        uint32 SummonSkeletonsTimer;
        uint32 RespawnSkeletonsTimer;
        uint32 ShadowboltTimer;

        uint64 SkeletonGUID[5];

        bool Skeletons;
        bool RespawnSkeletons;

        void Reset()
        {
            ShadowboltTimer = 500;

            Skeletons = false;

            ShatterFrostTomb = false;

            ResetTimer();

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			instance->SetData(DATA_PRINCE_KELESETH_EVENT, NOT_STARTED);
        }

        void KilledUnit(Unit* victim)
        {
            if (victim == me)
                return;

            DoSendQuantumText(SAY_KILL, me);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

            if (IsHeroic() && !ShatterFrostTomb)
				instance->DoCompleteAchievement(ACHIEVEMENT_ON_THE_ROCKS);

			instance->SetData(DATA_PRINCE_KELESETH_EVENT, DONE);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

            DoZoneInCombat();

			instance->SetData(DATA_PRINCE_KELESETH_EVENT, IN_PROGRESS);
        }

        void ResetTimer(uint32 inc = 0)
        {
            SummonSkeletonsTimer = 5000 + inc;
            FrostTombTimer = 28000 + inc;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (ShadowboltTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_TOP_AGGRO, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOWBOLT_5N, SPELL_SHADOWBOLT_5H));
					ShadowboltTimer = urand(3000, 4000);
				}
            }
			else ShadowboltTimer -= diff;

            if (!Skeletons)
            {
                if (SummonSkeletonsTimer <= diff)
                {
                    Creature* Skeleton;
                    DoSendQuantumText(SAY_SKELETONS, me);
                    for (uint8 i = 0; i < 5; ++i)
                    {
                        Skeleton = me->SummonCreature(NPC_SKELETON, SkeletonSpawnPoint[i][0], SkeletonSpawnPoint[i][1], SKELETONSPAWN_Z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20000);
                        if (Skeleton)
                        {
                            Skeleton->SetWalk(false);
                            Skeleton->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
                            Skeleton->AddThreat(me->GetVictim(), 0.0f);
                            DoZoneInCombat(Skeleton);
                        }
                    }
                    Skeletons = true;
                }
				else SummonSkeletonsTimer -= diff;
            }

            if (FrostTombTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 100.0f, true))
				{
                    if (target && target->IsAlive())
                    {
                        if (Creature* tomb = me->SummonCreature(NPC_FROST_TOMB, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 20000))
                        {
                            tomb->AI()->SetGUID(target->GetGUID(), 1);
                            tomb->CastSpell(target, SPELL_FROST_TOMB, true);

                            DoSendQuantumText(SAY_FROST_TOMB, me);
                        }
                    }
					FrostTombTimer = 15000;
				}
            }
			else FrostTombTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_kelesethAI (creature);
    }
};

class npc_frost_tomb : public CreatureScript
{
public:
    npc_frost_tomb() : CreatureScript("npc_frost_tomb") { }

    struct npc_frost_tombAI : public QuantumBasicAI
    {
        npc_frost_tombAI(Creature* creature) : QuantumBasicAI(creature)
        {
            FrostTombGUID = 0;
        }

        uint64 FrostTombGUID;

        void SetGUID(uint64 guid,uint32 event_id)
        {
            switch(event_id)
            {
            case 1:
				FrostTombGUID = guid;
				break;
            }
        }

        void Reset()
		{
			FrostTombGUID = 0;
		}

        void EnterToBattle(Unit* /*who*/) {}

        void AttackStart(Unit* /*who*/) {}

        void MoveInLineOfSight(Unit* /*who*/) {}

        void JustDied(Unit* killer)
        {
            if (killer->GetGUID() != me->GetGUID())
                ShatterFrostTomb = true;

            if (FrostTombGUID)
            {
                Unit* tomb = Unit::GetUnit(*me, FrostTombGUID);
                if (tomb)
                    tomb->RemoveAurasDueToSpell(SPELL_FROST_TOMB);
            }
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            Unit* temp = Unit::GetUnit(*me, FrostTombGUID);
            if ((temp && temp->IsAlive() && !temp->HasAura(SPELL_FROST_TOMB)) || !temp)
                me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frost_tombAI(creature);
    }
};

class npc_vrykul_skeleton : public CreatureScript
{
public:
    npc_vrykul_skeleton() : CreatureScript("npc_vrykul_skeleton") { }

    struct npc_vrykul_skeletonAI : public QuantumBasicAI
    {
        npc_vrykul_skeletonAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 RespawnTimer;
		uint32 DecrepifyTimer;

        uint64 TargetGuid;

        bool IsDead;

        void Reset()
        {
			RespawnTimer = 12000;
            DecrepifyTimer = urand(10000, 20000);
            IsDead = false;
        }

        void EnterToBattle(Unit* /*who*/){}

        void DamageTaken(Unit* doneby, uint32 &damage)
        {
            if (doneby->GetGUID() == me->GetGUID())
                return;

            if (damage >= me->GetHealth())
            {
                PretendToDie();
                damage = 0;
            }
        }

        void PretendToDie()
        {
            IsDead = true;
            me->InterruptNonMeleeSpells(true);
            me->RemoveAllAuras();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->GetMotionMaster()->MovementExpired(false);
            me->GetMotionMaster()->MoveIdle();
            me->SetStandState(UNIT_STAND_STATE_DEAD);
        }

        void Resurrect()
        {
            IsDead = false;
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetStandState(UNIT_STAND_STATE_STAND);
            DoCast(me, SPELL_SCOURGE_RESSURRECTION, true);

            if (me->GetVictim())
            {
                me->GetMotionMaster()->MoveChase(me->GetVictim());
                me->AI()->AttackStart(me->GetVictim());
            }
            else
                me->GetMotionMaster()->Initialize();
        }

        void UpdateAI(const uint32 diff)
        {
            if (instance && instance->GetData(DATA_PRINCE_KELESETH_EVENT) == IN_PROGRESS)
            {
                if (IsDead)
                {
                    if (RespawnTimer <= diff)
                    {
                        Resurrect();
                        RespawnTimer = 12000;
                    }
					else RespawnTimer -= diff;
                }
                else
                {
                    if (!UpdateVictim())
                        return;

                    if (DecrepifyTimer <= diff)
                    {
                        DoCastVictim(DUNGEON_MODE(SPELL_DECREPIFY_5N, SPELL_DECREPIFY_5H));
                        DecrepifyTimer = 6000;
                    }
					else DecrepifyTimer -= diff;

                    DoMeleeAttackIfReady();
                }
            }
			else
            {
				if (me->IsAlive())
					me->DealDamage(me, me->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vrykul_skeletonAI (creature);
    }
};

void AddSC_boss_keleseth()
{
    new boss_keleseth();
    new npc_frost_tomb();
    new npc_vrykul_skeleton();
}