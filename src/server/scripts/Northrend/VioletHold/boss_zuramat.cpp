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
	SAY_AGGRO      = -1608037,
    SAY_SLAY_1     = -1608038,
    SAY_SLAY_2     = -1608039,
    SAY_SLAY_3     = -1608040,
    SAY_DEATH      = -1608041,
    SAY_SPAWN      = -1608042,
    SAY_VOID_SHIFT = -1608043,
    SAY_DARKNESS   = -1608044,
};

enum Spells
{
	SPELL_SHROUD_OF_DARKNESS_5N = 54524,
    SPELL_SHROUD_OF_DARKNESS_5H = 59745,
    SPELL_SUMMON_VOID_SENTRY    = 54369,
    SPELL_VOID_SHIFT_5N         = 54361,
    SPELL_VOID_SHIFT_5H         = 59743,
    SPELL_ZURAMAT_ADD_2_5N      = 54342,
    SPELL_ZURAMAT_ADD_2_5H      = 59747,
};

enum Actions
{
    ACTION_VOID_DEAD,
};

class boss_zuramat : public CreatureScript
{
public:
    boss_zuramat() : CreatureScript("boss_zuramat") { }

    struct boss_zuramatAI : public QuantumBasicAI
    {
        boss_zuramatAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
			instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        SummonList Summons;

        uint32 SpellVoidShiftTimer;
        uint32 SpellSummonVoidTimer;
        uint32 SpellShroudOfDarknessTimer;

		bool Intro;
        bool VoidWalkerKilled;

        void Reset()
        {
			SpellVoidShiftTimer = 500;
			SpellShroudOfDarknessTimer = 3500;
            SpellSummonVoidTimer = 6000;

			Intro = false;
            VoidWalkerKilled = false;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			Summons.DespawnAll();

			if (instance->GetData(DATA_WAVE_COUNT) == 6)
				instance->SetData(DATA_1ST_BOSS_EVENT, NOT_STARTED);
			else if (instance->GetData(DATA_WAVE_COUNT) == 12)
				instance->SetData(DATA_2ND_BOSS_EVENT, NOT_STARTED);
        }

		void MoveInLineOfSight(Unit* who)
        {
            if (!instance || Intro || who->GetTypeId() != TYPE_ID_PLAYER || !who->IsWithinDistInMap(me, 20.0f))
				return;

			if (instance && Intro == false && who->GetTypeId() == TYPE_ID_PLAYER && who->IsWithinDistInMap(me, 20.0f))
			{
				DoSendQuantumText(SAY_SPAWN, me);
				Intro = true;
			}
		}

        void DoAction(int32 const action)
        {
            switch(action)
            {
                case ACTION_VOID_DEAD:
					VoidWalkerKilled = true;
					break;
            }
        }

        void AttackStart(Unit* who)
        {
            if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER) || me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                return;

            if (me->Attack(who, true))
            {
                me->AddThreat(who, 0.0f);
                me->SetInCombatWith(who);
                who->SetInCombatWith(me);
                DoStartMovement(who);
            }
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

			if (GameObject* Zcell = instance->instance->GetGameObject(instance->GetData64(DATA_ZURAMAT_CELL)))
			{
				if (Zcell->GetGoState() == GO_STATE_READY)
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

		void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

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

			if (GetDifficulty() == DUNGEON_DIFFICULTY_HEROIC && !VoidWalkerKilled)
				instance->DoCompleteAchievement(ACHIEVEMENTS_THE_VOID_DANCE);

			Summons.DespawnAll();
        }

        void KilledUnit(Unit* victim)
        {
            if (victim == me)
                return;

            DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
        }

        void JustSummoned(Creature* summon)
        {
            Summons.Summon(summon);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (SpellVoidShiftTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoSendQuantumText(SAY_VOID_SHIFT, me);
					DoCast(target, DUNGEON_MODE(SPELL_VOID_SHIFT_5N, SPELL_VOID_SHIFT_5H));
					SpellVoidShiftTimer = 5000;
				}
            }
			else SpellVoidShiftTimer -=diff;

            if (SpellShroudOfDarknessTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoSendQuantumText(SAY_DARKNESS, me);
					DoCast(target, DUNGEON_MODE(SPELL_SHROUD_OF_DARKNESS_5N, SPELL_SHROUD_OF_DARKNESS_5H));
					SpellShroudOfDarknessTimer = 8000;
				}
            }
			else SpellShroudOfDarknessTimer -=diff;

			if (SpellSummonVoidTimer <= diff)
            {
                DoCastVictim(SPELL_SUMMON_VOID_SENTRY, false);
                SpellSummonVoidTimer = 11000;
            }
			else SpellSummonVoidTimer -=diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_zuramatAI(creature);
    }
};

class npc_void_sentry : public CreatureScript
{
public:
    npc_void_sentry() : CreatureScript("npc_void_sentry") { }

    struct npc_void_sentryAI : public QuantumBasicAI
    {
        npc_void_sentryAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            me->SetCurrentFaction(16);

			if (Creature* Zuramat = Creature::GetCreature(*me,instance->GetData64(DATA_ZURAMAT)))
				Zuramat->AI()->JustSummoned(me);
        }

        InstanceScript* instance;

        Unit* SelectPlayerTargetInRange(float range)
        {
            Player *target = NULL;
            Trinity::AnyPlayerInObjectRangeCheck u_check(me, range, true);
            Trinity::PlayerSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(me, target, u_check);
            me->VisitNearbyObject(range, searcher);
            return target;
        }

        void Reset()
        {
            if (Unit* target = SelectPlayerTargetInRange(100.0f))
                me->AI()->AttackStart(target);

            DoCast(me, DUNGEON_MODE(SPELL_ZURAMAT_ADD_2_5N, SPELL_ZURAMAT_ADD_2_5H), true);
            me->SetPhaseMask(17, true);
        }

        void JustDied(Unit* /*killer*/)
        {
            if (Creature* Zuramat = Creature::GetCreature(*me,instance->GetData64(DATA_ZURAMAT)))
                Zuramat->AI()->DoAction(ACTION_VOID_DEAD);
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_void_sentryAI(creature);
    }
};

void AddSC_boss_zuramat()
{
    new boss_zuramat();
    new npc_void_sentry();
}