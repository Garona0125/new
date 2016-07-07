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
    SAY_AGGRO            = -1608010,
    SAY_SLAY_1           = -1608011,
    SAY_SLAY_2           = -1608012,
    SAY_SLAY_3           = -1608013,
    SAY_DEATH            = -1608014,
    SAY_SPAWN            = -1608015,
    SAY_ADD_KILLED       = -1608016,
    SAY_BOTH_ADDS_KILLED = -1608017,
};

enum Spells
{
    SPELL_BLOODLUST       = 54516,
    SPELL_BREAK_BONDS     = 59463,
    SPELL_CHAIN_HEAL_5N   = 54481,
    SPELL_CHAIN_HEAL_5H   = 59473,
    SPELL_EARTH_SHIELD_5N = 54479,
    SPELL_EARTH_SHIELD_5H = 59471,
    SPELL_EARTH_SHOCK     = 54511,
    SPELL_LIGHTNING_BOLT  = 53044,
    SPELL_STORMSTRIKE     = 51876,
};

class boss_erekem : public CreatureScript
{
public:
    boss_erekem() : CreatureScript("boss_erekem") { }

    struct boss_erekemAI : public QuantumBasicAI
    {
        boss_erekemAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

		InstanceScript* instance;

        uint32 BloodlustTimer;
        uint32 ChainHealTimer;
        uint32 EarthShockTimer;
        uint32 LightningBoltTimer;
        uint32 EarthShieldTimer;
        uint32 BreakBoundsTimer;

		bool Intro;

        void Reset()
        {
            BloodlustTimer = 15000;
            ChainHealTimer = 0;
            EarthShockTimer = urand(2000, 8000);
            LightningBoltTimer = urand(5000, 10000);
            EarthShieldTimer = 20000;
            BreakBoundsTimer = urand(10000, 20000);

			Intro = false;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			if (instance->GetData(DATA_WAVE_COUNT) == 6)
				instance->SetData(DATA_1ST_BOSS_EVENT, NOT_STARTED);
			else if (instance->GetData(DATA_WAVE_COUNT) == 12)
				instance->SetData(DATA_2ND_BOSS_EVENT, NOT_STARTED);

            if (Creature* guard1 = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_EREKEM_GUARD_1) : 0))
            {
                if (!guard1->IsAlive())
                    guard1->Respawn();
            }

            if (Creature* guard2 = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_EREKEM_GUARD_2) : 0))
            {
                if (!guard2->IsAlive())
                    guard2->Respawn();
            }
        }

		void MoveInLineOfSight(Unit* who)
        {
            if (!instance || Intro || who->GetTypeId() != TYPE_ID_PLAYER || !who->IsWithinDistInMap(me, 25.0f))
				return;

			if (instance && Intro == false && who->GetTypeId() == TYPE_ID_PLAYER && who->IsWithinDistInMap(me, 25.0f))
			{
				DoSendQuantumText(SAY_SPAWN, me);
				Intro = true;
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

                if (Creature* guard1 = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_EREKEM_GUARD_1) : 0))
                {
                    guard1->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NON_ATTACKABLE);

                    if (!guard1->GetVictim() && guard1->AI())
						guard1->AI()->AttackStart(who);
                }

                if (Creature* guard2 = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_EREKEM_GUARD_2) : 0))
                {
                    guard2->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NON_ATTACKABLE);

                    if (!guard2->GetVictim() && guard2->AI())
                        guard2->AI()->AttackStart(who);
                }
            }
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

            DoCast(me, DUNGEON_MODE(SPELL_EARTH_SHIELD_5N, SPELL_EARTH_SHIELD_5H));

			if (GameObject* ecell = instance->instance->GetGameObject(instance->GetData64(DATA_EREKEM_CELL)))
			{
				if (ecell->GetGoState() == GO_STATE_READY)
				{
					EnterEvadeMode();
					return;
				}
			}

			if (instance->GetData(DATA_WAVE_COUNT) == 6)
				instance->SetData(DATA_1ST_BOSS_EVENT, IN_PROGRESS);
			else if (instance->GetData(DATA_WAVE_COUNT) == 12)
				instance->SetData(DATA_2ND_BOSS_EVENT, IN_PROGRESS);
        }

		void KilledUnit(Unit* victim)
        {
            if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
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
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (Creature* guard1 = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_EREKEM_GUARD_1) : 0))
			{
				if (Creature* guard2 = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_EREKEM_GUARD_2) : 0))
				{
					if (!guard1->IsAlive() && !guard2->IsAlive())
					{
						DoSendQuantumText(SAY_BOTH_ADDS_KILLED, me);
						DoCastVictim(SPELL_STORMSTRIKE);
					}
                }
            }

            if (EarthShieldTimer <= diff)
            {
				DoCast(me, DUNGEON_MODE(SPELL_EARTH_SHIELD_5N, SPELL_EARTH_SHIELD_5H));
				EarthShieldTimer = 20000;
            }
			else EarthShieldTimer -= diff;

            if (ChainHealTimer <= diff)
            {
                if (uint64 TargetGUID = GetChainHealTargetGUID())
                {
                    if (Creature* target = Unit::GetCreature(*me, TargetGUID))
                        DoCast(target, DUNGEON_MODE(SPELL_CHAIN_HEAL_5N, SPELL_CHAIN_HEAL_5H));

                    Creature* guard1 = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_EREKEM_GUARD_1) : 0);
                    Creature* guard2 = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_EREKEM_GUARD_2) : 0);
                    ChainHealTimer = ((guard1 && !guard1->IsAlive()) || (guard2 && !guard2->IsAlive()) ? 3000 : 8000) + rand()%3000+6000;
                }
            }
			else ChainHealTimer -= diff;

            if (BloodlustTimer <= diff)
            {
				DoCast(me, SPELL_BLOODLUST);
				BloodlustTimer = urand(35000, 45000);
            }
			else BloodlustTimer -= diff;

            if (EarthShockTimer <= diff)
            {
				DoCastVictim(SPELL_EARTH_SHOCK );
				EarthShockTimer = urand(8000, 13000);
            }
			else EarthShockTimer -= diff;

            if (LightningBoltTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
					DoCast(target, SPELL_LIGHTNING_BOLT);
                    LightningBoltTimer = urand(18000, 24000);
                }
            }
			else LightningBoltTimer -= diff;

            if (BreakBoundsTimer <= diff)
            {
				DoCast(me, SPELL_BREAK_BONDS);
				BreakBoundsTimer = urand(10000, 20000);
            }
			else BreakBoundsTimer -= diff;

            DoMeleeAttackIfReady();
        }

        uint64 GetChainHealTargetGUID()
        {
            if (HealthBelowPct(HEALTH_PERCENT_85))
                return me->GetGUID();

            Creature* guard1 = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_EREKEM_GUARD_1) : 0);
            if (guard1 && guard1->IsAlive() && !guard1->HealthAbovePct(75))
                return guard1->GetGUID();

            Creature* guard2 = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_EREKEM_GUARD_2) : 0);
            if (guard2 && guard2->IsAlive() && !guard2->HealthAbovePct(75))
                return guard2->GetGUID();

            return 0;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_erekemAI(creature);
    }
};

enum GuardSpells
{
    SPELL_GUSHING_WOUND   = 39215,
    SPELL_HOWLING_SCREECH = 54462,
    SPELL_STRIKE          = 14516,
};

class mob_erekem_guard : public CreatureScript
{
public:
    mob_erekem_guard() : CreatureScript("mob_erekem_guard") { }

    struct mob_erekem_guardAI : public QuantumBasicAI
    {
        mob_erekem_guardAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 GushingWoundTimer;
        uint32 HowlingScreechTimer;
        uint32 StrikeTimer;

        InstanceScript* instance;

        void Reset()
        {
            StrikeTimer = urand(4000, 8000);
            HowlingScreechTimer = urand(8000, 13000);
            GushingWoundTimer = urand(1000, 3000);
        }

		void JustDied(Unit* /*killer*/)
		{
			if (Creature* erekem = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_EREKEM) : 0))
				DoSendQuantumText(SAY_ADD_KILLED, erekem);
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

        void MoveInLineOfSight(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();

            if (StrikeTimer <= diff)
            {
                DoCastVictim(SPELL_STRIKE);
                StrikeTimer = urand(4000, 8000);
            }
			else StrikeTimer -= diff;

            if (HowlingScreechTimer <= diff)
            {
                DoCastVictim(SPELL_HOWLING_SCREECH);
                HowlingScreechTimer = urand(8000, 13000);
            }
			else HowlingScreechTimer -= diff;

            if (GushingWoundTimer <= diff)
            {
                DoCastVictim(SPELL_GUSHING_WOUND);
                GushingWoundTimer = urand(7000, 12000);
            }
			else GushingWoundTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_erekem_guardAI(creature);
    }
};

void AddSC_boss_erekem()
{
    new boss_erekem();
    new mob_erekem_guard();
}