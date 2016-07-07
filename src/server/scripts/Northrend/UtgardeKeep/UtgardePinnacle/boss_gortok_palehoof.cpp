/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include <algorithm>
#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "utgarde_pinnacle.h"

enum Texts
{
    SAY_AGGRO    = -1575000,
    SAY_SLAY_1   = -1575001,
    SAY_SLAY_2   = -1575002,
    SAY_DEATH    = -1575003,
};

enum Spells
{
    SPELL_ARCING_SMASH                 = 48260,
    SPELL_IMPALE_5N                    = 48261,
    SPELL_IMPALE_5H                    = 59268,
    SPELL_WITHERING_ROAR_5N            = 48256,
    SPELL_WITHERING_ROAR_5H            = 59267,
    SPELL_FREEZE                       = 16245,
	SPELL_AWAKEN_SUBBOSS               = 47669,
	SPELL_ORB_VISUAL                   = 48044,
    SPELL_ORB_CHANNEL                  = 48048,
	SPELL_CHAIN_LIGHTING_5N            = 48140,
    SPELL_CHAIN_LIGHTING_5H            = 59273,
    SPELL_CRAZED                       = 48139,
    SPELL_TERRIFYING_ROAR              = 48144,
	SPELL_MORTAL_WOUND_5N              = 48137,
    SPELL_MORTAL_WOUND_5H              = 59265,
    SPELL_ENRAGE_1                     = 48138,
    SPELL_ENRAGE_2                     = 48142,
	SPELL_GORE_5N                      = 48130,
    SPELL_GORE_5H                      = 59264,
    SPELL_GRIEVOUS_WOUND_5N            = 48105,
    SPELL_GRIEVOUS_WOUND_5H            = 59263,
    SPELL_STOMP_5N                     = 48131,
	SPELL_STOMP_5H                     = 59744,
    SPELL_ACID_SPIT                    = 48132,
    SPELL_ACID_SPLATTER_5N             = 48136,
    SPELL_ACID_SPLATTER_5H             = 59272,
    SPELL_POISON_BREATH_5N             = 48133,
    SPELL_POISON_BREATH_5H             = 59271,
};

struct Locations
{
    float x, y, z;
};

struct Locations moveLocs[] =
{
    {261.6f, -449.3f, 109.5f},
    {263.3f, -454.0f, 109.5f},
    {291.5f, -450.4f, 109.5f},
    {291.5f, -454.0f, 109.5f},
    {310.0f, -453.4f, 109.5f},
    {238.6f, -460.7f, 109.5f},
};

enum Phase
{
    PHASE_FRENZIED_WORGEN,
    PHASE_RAVENOUS_FURLBORG,
    PHASE_MASSIVE_JORMUNGAR,
    PHASE_FEROCIOUS_RHINO,
    PHASE_GORTOK_PALEHOOF,
    PHASE_NONE,
};

class boss_gortok_palehoof : public CreatureScript
{
public:
    boss_gortok_palehoof() : CreatureScript("boss_gortok_palehoof") { }

    struct boss_gortok_palehoofAI : public QuantumBasicAI
    {
        boss_gortok_palehoofAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 ArcingSmashTimer;
        uint32 ImpaleTimer;
        uint32 WhiteringRoarTimer;
        uint32 WaitingTimer;
        Phase CurrentPhase;
        uint8 AddCount;
        bool DoneAdds[4];

        InstanceScript* instance;

        void Reset()
        {
            ArcingSmashTimer = 15000;
            ImpaleTimer = 12000;
            WhiteringRoarTimer = 10000;

            me->GetMotionMaster()->MoveTargetedHome();

            for (uint32 i = 0; i < 4; i++)
                DoneAdds[i] = false;
            AddCount = 0;

            CurrentPhase = PHASE_NONE;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			instance->SetData(DATA_GORTOK_PALEHOOF_EVENT, NOT_STARTED);

			Creature* temp = Unit::GetCreature(*me, instance->GetData64(DATA_FRENZIED_WORGEN));
			if (temp && !temp->IsAlive())
				temp->Respawn();

			temp = Unit::GetCreature(*me, instance->GetData64(DATA_FEROCIOUS_RHINO));
			if (temp && !temp->IsAlive())
				temp->Respawn();

			temp = Unit::GetCreature(*me, instance->GetData64(DATA_MASSIVE_JORMUNGAR));
			if (temp && !temp->IsAlive())
				temp->Respawn();

			temp = Unit::GetCreature(*me, instance->GetData64(DATA_RAVENOUS_FURBOLG));
			if (temp && !temp->IsAlive())
				temp->Respawn();

			temp = Unit::GetCreature(*me, instance->GetData64(DATA_MOB_ORB));
			if (temp)
				temp->DisappearAndDie();

			GameObject* go = instance->instance->GetGameObject(instance->GetData64(DATA_GORTOK_PALEHOOF_SPHERE));

			if (go)
			{
				go->SetGoState(GO_STATE_READY);
				go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
            }
        }

        void EnterToBattle(Unit* who)
        {
            me->GetMotionMaster()->MoveChase(who);

            DoSendQuantumText(SAY_AGGRO, me);
        }

        void AttackStart(Unit* who)
        {
            if (!who)
                return;

            if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
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
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

		void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

			instance->SetData(DATA_GORTOK_PALEHOOF_EVENT, DONE);

            if (Creature* orb = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_MOB_ORB) : 0))
			{
				if (orb->IsAlive())
					orb->DisappearAndDie();
			}
        }

		void JustReachedHome()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            me->SetStandState(UNIT_STAND_STATE_STAND);
            DoCast(me, SPELL_FREEZE);
        }

		void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_ORB_VISUAL)
				DoCastAOE(SPELL_AWAKEN_SUBBOSS);
        }

		void NextPhase()
        {
            if (CurrentPhase == PHASE_NONE)
            {
                instance->SetData(DATA_GORTOK_PALEHOOF_EVENT, IN_PROGRESS);
                me->SummonCreature(NPC_STASIS_CONTROLLER, moveLocs[5].x, moveLocs[5].y, moveLocs[5].z, 0, TEMPSUMMON_CORPSE_DESPAWN);
            }
            Phase move = PHASE_NONE;
            if (AddCount >= DUNGEON_MODE(2, 4))
                move = PHASE_GORTOK_PALEHOOF;
            else
            {
                uint8 next = urand(0, 3);
                for (uint8 i = 0; i < 16; i++)
                {
                    if (!DoneAdds[i % 4])
                    {
                        if (next == 0)
                        {
                            move = (Phase)(i % 4);
                            break;
                        }
                        else if (next > 0)
                            --next;
                    }
                }
                ++AddCount;
                DoneAdds[move] = true;
                move = (Phase)(move % 4);
            }
            //send orb to summon spot
            Creature* orb = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_MOB_ORB) : 0);
            if (orb && orb->IsAlive())
            {
                if (CurrentPhase == PHASE_NONE)
                    orb->CastSpell(me, SPELL_ORB_VISUAL, true);
                orb->GetMotionMaster()->MovePoint(move, moveLocs[move].x, moveLocs[move].y, moveLocs[move].z);
            }
            CurrentPhase = move;
        }

        void UpdateAI(const uint32 diff)
        {
            if (CurrentPhase != PHASE_GORTOK_PALEHOOF)
                return;

            if (!UpdateVictim())
                return;

            if (Creature* orb = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_MOB_ORB) : 0))
			{
				if (orb->IsAlive())
					orb->DisappearAndDie();
			}

            if (ArcingSmashTimer <= diff)
            {
                DoCast(me, SPELL_ARCING_SMASH);
                ArcingSmashTimer = urand(13000, 17000);
            }
			else ArcingSmashTimer -= diff;

            if (ImpaleTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
					DoCast(target, DUNGEON_MODE(SPELL_IMPALE_5N, SPELL_IMPALE_5H));
					ImpaleTimer = urand(8000, 12000);
				}
            }
			else ImpaleTimer -= diff;

            if (WhiteringRoarTimer <= diff)
            {
                DoCastAOE(DUNGEON_MODE(SPELL_WITHERING_ROAR_5N, SPELL_WITHERING_ROAR_5H));
                WhiteringRoarTimer = urand(8000, 12000);
            }
			else WhiteringRoarTimer -= diff;

            DoMeleeAttackIfReady();
        }
	};

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_gortok_palehoofAI(creature);
    }
};

class npc_ravenous_furbolg : public CreatureScript
{
public:
    npc_ravenous_furbolg() : CreatureScript("npc_ravenous_furbolg") { }

    struct npc_ravenous_furbolgAI : public QuantumBasicAI
    {
        npc_ravenous_furbolgAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 ChainLightingTimer;
        uint32 CrazedTimer;
        uint32 TerrifyingRoarTimer;

        InstanceScript* instance;

        void Reset()
        {
            ChainLightingTimer = 5000;
            CrazedTimer = 10000;
            TerrifyingRoarTimer = 15000;

            me->GetMotionMaster()->MoveTargetedHome();

			if (instance->GetData(DATA_GORTOK_PALEHOOF_EVENT) == IN_PROGRESS)
			{
				if (Creature* palehoof = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_GORTOK_PALEHOOF) : 0))
				{
					if (palehoof->IsAlive())
						CAST_AI(boss_gortok_palehoof::boss_gortok_palehoofAI, palehoof->AI())->Reset();
				}
			}
        }

        void EnterToBattle(Unit* who)
        {
            me->GetMotionMaster()->MoveChase(who);
        }

		void AttackStart(Unit* who)
        {
            if (!who)
                return;

            if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                return;

            if (me->Attack(who, true))
            {
                me->AddThreat(who, 0.0f);
                me->SetInCombatWith(who);
                who->SetInCombatWith(me);
                DoStartMovement(who);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
			if (Creature* palehoof = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_GORTOK_PALEHOOF) : 0))
			{
				if (palehoof)
					CAST_AI(boss_gortok_palehoof::boss_gortok_palehoofAI, palehoof->AI())->NextPhase();
			}
        }

        void JustReachedHome()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            me->SetStandState(UNIT_STAND_STATE_STAND);
            DoCast(me, SPELL_FREEZE);
        }

		void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_ORB_VISUAL)
				DoCastAOE(SPELL_AWAKEN_SUBBOSS);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (ChainLightingTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_CHAIN_LIGHTING_5N, SPELL_CHAIN_LIGHTING_5H));
					ChainLightingTimer = 5000 + rand() % 5000;
				}
            }
			else ChainLightingTimer -= diff;

            if (CrazedTimer <= diff)
            {
                DoCast(me, SPELL_CRAZED);
                CrazedTimer = 8000 + rand() % 4000;
            }
			else CrazedTimer -= diff;

            if (TerrifyingRoarTimer <= diff)
            {
                DoCast(me, SPELL_TERRIFYING_ROAR);
                TerrifyingRoarTimer = 10000 + rand() % 10000;
            }
			else TerrifyingRoarTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ravenous_furbolgAI(creature);
    }
};

class npc_frenzied_worgen : public CreatureScript
{
public:
    npc_frenzied_worgen() : CreatureScript("npc_frenzied_worgen") { }

    struct npc_frenzied_worgenAI : public QuantumBasicAI
    {
        npc_frenzied_worgenAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 MortalWoundTimer;
        uint32 Enrage1Timer;
        uint32 Enrage2Timer;

        InstanceScript* instance;

        void Reset()
        {
            MortalWoundTimer = 5000;
            Enrage1Timer = 15000;
            Enrage2Timer = 10000;

            me->GetMotionMaster()->MoveTargetedHome();

			if (instance->GetData(DATA_GORTOK_PALEHOOF_EVENT) == IN_PROGRESS)
			{
				if (Creature* palehoof = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_GORTOK_PALEHOOF) : 0))
				{
					if (palehoof->IsAlive())
						CAST_AI(boss_gortok_palehoof::boss_gortok_palehoofAI, palehoof->AI())->Reset();
				}
			}
        }

        void EnterToBattle(Unit* who)
        {
            me->GetMotionMaster()->MoveChase(who);
        }

		void AttackStart(Unit* who)
        {
            if (!who)
                return;

            if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                return;

            if (me->Attack(who, true))
            {
                me->AddThreat(who, 0.0f);
                me->SetInCombatWith(who);
                who->SetInCombatWith(me);
                DoStartMovement(who);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
			if (Creature* palehoof = Unit::GetCreature(*me, instance->GetData64(DATA_GORTOK_PALEHOOF)))
			{
				if (palehoof)
					CAST_AI(boss_gortok_palehoof::boss_gortok_palehoofAI, palehoof->AI())->NextPhase();
			}
        }

        void JustReachedHome()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            me->SetStandState(UNIT_STAND_STATE_STAND);
            DoCast(me, SPELL_FREEZE);
        }

		void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_ORB_VISUAL)
				DoCastAOE(SPELL_AWAKEN_SUBBOSS);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (MortalWoundTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_MORTAL_WOUND_5N, SPELL_MORTAL_WOUND_5H));
                MortalWoundTimer = 3000 + rand() % 4000;
            }
			else MortalWoundTimer -= diff;

            if (Enrage1Timer <= diff)
            {
                DoCast(me, SPELL_ENRAGE_1);
                Enrage1Timer = 15000;
            }
			else Enrage1Timer -= diff;

            if (Enrage2Timer <= diff)
            {
                DoCast(me, SPELL_ENRAGE_2);
                Enrage2Timer = 10000;
            }
			else Enrage2Timer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_frenzied_worgenAI(creature);
    }
};

class npc_ferocious_rhino : public CreatureScript
{
public:
    npc_ferocious_rhino() : CreatureScript("npc_ferocious_rhino") { }

    struct npc_ferocious_rhinoAI : public QuantumBasicAI
    {
        npc_ferocious_rhinoAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 StompTimer;
        uint32 GoreTimer;
        uint32 GrievousWoundTimer;

        InstanceScript* instance;

        void Reset()
        {
            StompTimer = 10000;
            GoreTimer = 15000;
            GrievousWoundTimer = 20000;

            me->GetMotionMaster()->MoveTargetedHome();

			if (instance->GetData(DATA_GORTOK_PALEHOOF_EVENT) == IN_PROGRESS)
			{
				if (Creature* palehoof = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_GORTOK_PALEHOOF) : 0))
				{
					if (palehoof->IsAlive())
						CAST_AI(boss_gortok_palehoof::boss_gortok_palehoofAI, palehoof->AI())->Reset();
				}
			}
        }

        void EnterToBattle(Unit* who)
        {
            me->GetMotionMaster()->MoveChase(who);
        }

		void AttackStart(Unit* who)
        {
            if (!who)
                return;

            if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                return;

            if (me->Attack(who, true))
            {
                me->AddThreat(who, 0.0f);
                me->SetInCombatWith(who);
                who->SetInCombatWith(me);
                DoStartMovement(who);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
			if (Creature* palehoof = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_GORTOK_PALEHOOF) : 0))
			{
				if (palehoof)
					CAST_AI(boss_gortok_palehoof::boss_gortok_palehoofAI, palehoof->AI())->NextPhase();
			}
        }

        void JustReachedHome()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            me->SetStandState(UNIT_STAND_STATE_STAND);
            DoCast(me, SPELL_FREEZE);
        }

		void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_ORB_VISUAL)
				DoCastAOE(SPELL_AWAKEN_SUBBOSS);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (StompTimer <= diff)
            {
                DoCastAOE(DUNGEON_MODE(SPELL_STOMP_5N, SPELL_STOMP_5H));
                StompTimer = 8000 + rand() % 4000;
            }
			else StompTimer -= diff;

            if (GoreTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_GORE_5N, SPELL_GORE_5H));
                GoreTimer = 13000 + rand() % 4000;
            }
			else GoreTimer -= diff;

            if (GrievousWoundTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_GRIEVOUS_WOUND_5N, SPELL_GRIEVOUS_WOUND_5N));
					GrievousWoundTimer = 18000 + rand() % 4000;
				}
            }
			else GrievousWoundTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ferocious_rhinoAI(creature);
    }
};

class npc_massive_jormungar : public CreatureScript
{
public:
    npc_massive_jormungar() : CreatureScript("npc_massive_jormungar") { }

    struct npc_massive_jormungarAI : public QuantumBasicAI
    {
        npc_massive_jormungarAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 AcidSpitTimer;
        uint32 AcidSplatterTimer;
        uint32 PoisonBreathTimer;

        InstanceScript* instance;

        void Reset()
        {
            AcidSpitTimer = 3000;
            AcidSplatterTimer = 12000;
            PoisonBreathTimer = 10000;

            me->GetMotionMaster()->MoveTargetedHome();

			if (instance->GetData(DATA_GORTOK_PALEHOOF_EVENT) == IN_PROGRESS)
			{
				if (Creature* palehoof = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_GORTOK_PALEHOOF) : 0))
				{
					if (palehoof->IsAlive())
						CAST_AI(boss_gortok_palehoof::boss_gortok_palehoofAI, palehoof->AI())->Reset();
				}
			}
        }

        void EnterToBattle(Unit* who)
        {
            me->GetMotionMaster()->MoveChase(who);
        }

		void AttackStart(Unit* who)
        {
            if (!who)
                return;

            if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                return;

            if (me->Attack(who, true))
            {
                me->AddThreat(who, 0.0f);
                me->SetInCombatWith(who);
                who->SetInCombatWith(me);
                DoStartMovement(who);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
			if (Creature* palehoof = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_GORTOK_PALEHOOF) : 0))
			{
				if (palehoof)
					CAST_AI(boss_gortok_palehoof::boss_gortok_palehoofAI, palehoof->AI())->NextPhase();
			}
        }

        void JustReachedHome()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            me->SetStandState(UNIT_STAND_STATE_STAND);
            DoCast(me, SPELL_FREEZE);
        }

		void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_ORB_VISUAL)
				DoCastAOE(SPELL_AWAKEN_SUBBOSS);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (AcidSpitTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
                    DoCast(target, SPELL_ACID_SPIT);
					AcidSpitTimer = 2000 + rand() % 2000;
				}
            }
			else AcidSpitTimer -= diff;

            if (AcidSplatterTimer <= diff)
            {
                DoCast(me, DUNGEON_MODE(SPELL_ACID_SPLATTER_5N, SPELL_ACID_SPLATTER_5H));
                AcidSplatterTimer = 10000 + rand() % 4000;
            }
			else AcidSplatterTimer -= diff;

            if (PoisonBreathTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_POISON_BREATH_5N, SPELL_POISON_BREATH_5H));
					PoisonBreathTimer = 8000 + rand() % 4000;
				}
            }
			else PoisonBreathTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_massive_jormungarAI(creature);
    }
};

class npc_stasis_orb : public CreatureScript
{
public:
    npc_stasis_orb() : CreatureScript("npc_stasis_orb") { }

    struct npc_stasis_orbAI : public QuantumBasicAI
    {
        npc_stasis_orbAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
		Phase CurrentPhase;

        uint32 SummonTimer;

        void Reset()
        {
            CurrentPhase = PHASE_NONE;

            SummonTimer = 5000;

            me->AddUnitMovementFlag(MOVEMENTFLAG_FLYING);
            me->RemoveAurasDueToSpell(SPELL_ORB_VISUAL);
            me->SetSpeed(MOVE_FLIGHT, 0.5f);
        }

        void UpdateAI(const uint32 diff)
        {
			if (CurrentPhase == PHASE_NONE)
				return;

            if (SummonTimer <= diff)
            {
                if (CurrentPhase<5 && CurrentPhase >= 0)
                {
					Creature* next = NULL;
					switch (CurrentPhase)
					{
                        case PHASE_FRENZIED_WORGEN:
							next = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_FRENZIED_WORGEN) : 0);
							break;
                        case PHASE_RAVENOUS_FURLBORG:
							next = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_RAVENOUS_FURBOLG) : 0);
							break;
                        case PHASE_MASSIVE_JORMUNGAR:
							next = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_MASSIVE_JORMUNGAR) : 0);
							break;
                        case PHASE_FEROCIOUS_RHINO:
							next = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_FEROCIOUS_RHINO) : 0);
							break;
                        case PHASE_GORTOK_PALEHOOF:
							next = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_GORTOK_PALEHOOF) : 0);
							break;
                        default:
							break;
                   }

                   if (next)
                   {
					   next->RemoveAurasDueToSpell(SPELL_FREEZE);
					   next->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
					   next->SetStandState(UNIT_STAND_STATE_STAND);
					   next->SetInCombatWithZone();
					   next->AI()->AttackStart(next->SelectNearestTarget(100));

                   }
                   CurrentPhase = PHASE_NONE;
                }
            }
			else SummonTimer -= diff;
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id > 4)
                return;

            Creature* next = NULL;

            switch (id)
            {
                case PHASE_FRENZIED_WORGEN:
					next = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_FRENZIED_WORGEN) : 0);
					break;
                case PHASE_RAVENOUS_FURLBORG:
					next = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_RAVENOUS_FURBOLG) : 0);
					break;
                case PHASE_MASSIVE_JORMUNGAR:
					next = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_MASSIVE_JORMUNGAR) : 0);
					break;
                case PHASE_FEROCIOUS_RHINO:
					next = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_FEROCIOUS_RHINO) : 0);
					break;
                case PHASE_GORTOK_PALEHOOF:
					next = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_GORTOK_PALEHOOF) : 0);
					break;
                default:
					break;
            }

            if (next)
                DoCast(next, SPELL_ORB_CHANNEL, false);

            CurrentPhase = (Phase)id;
            SummonTimer = 5000;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stasis_orbAI(creature);
    }
};

class go_palehoof_sphere : public GameObjectScript
{
public:
    go_palehoof_sphere() : GameObjectScript("go_palehoof_sphere") { }

    bool OnGossipHello(Player* /*player*/, GameObject* go)
    {
        InstanceScript* instance = go->GetInstanceScript();

        if (Creature* palehoof = Unit::GetCreature(*go, instance ? instance->GetData64(DATA_GORTOK_PALEHOOF) : 0))
		{
			if (palehoof->IsAlive())
			{
				go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
				go->SetGoState(GO_STATE_ACTIVE);

				if (InstanceScript* instance = go->GetInstanceScript())
					if (instance->GetData(DATA_GORTOK_PALEHOOF_EVENT) == IN_PROGRESS)
						return true;

				CAST_AI(boss_gortok_palehoof::boss_gortok_palehoofAI, palehoof->AI())->NextPhase();
			}
        }
        return true;
    }
};

void AddSC_boss_gortok_palehoof()
{
    new boss_gortok_palehoof();
    new npc_ravenous_furbolg();
    new npc_frenzied_worgen();
    new npc_ferocious_rhino();
    new npc_massive_jormungar();
    new npc_stasis_orb();
    new go_palehoof_sphere();
}