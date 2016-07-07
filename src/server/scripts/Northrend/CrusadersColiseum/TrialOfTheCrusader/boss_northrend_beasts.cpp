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
#include "SpellScript.h"
#include "Vehicle.h"
#include "Player.h"
#include "trial_of_the_crusader.h"

enum Texts
{
    SAY_SNOBOLLED     = -1649000,
    SAY_SUBMERGE      = -1649010,
    SAY_EMERGE        = -1649011,
    SAY_BERSERK       = -1649012,
    SAY_TRAMPLE_STARE = -1649020,
    SAY_TRAMPLE_FAIL  = -1649021,
    SAY_TRAMPLE_START = -1649022,
};

enum Equipment
{
    EQUIP_MAIN    = 50760,
    EQUIP_OFFHAND = 48040,
    EQUIP_RANGED  = 47267,
    EQUIP_DONE    = EQUIP_NO_CHANGE,
};

enum Model
{
    MODEL_ACIDMAW_STATIONARY    = 29815,
    MODEL_ACIDMAW_MOBILE        = 29816,
    MODEL_DREADSCALE_STATIONARY = 26935,
    MODEL_DREADSCALE_MOBILE     = 24564,
};

#define SPELL_ACID_SPITE_HELPER RAID_MODE<uint32>(66880,67606,67607,67608)
#define SPELL_PARALYTIC_BITE_HELPER RAID_MODE<uint32>(66824,67612,67613,67614)
#define SPELL_PARALYTIC_SPRAY_HELPER RAID_MODE<uint32>(66901,67615,67616,67617)
#define SPELL_SWEEP_0_HELPER RAID_MODE<uint32>(66794,67644,67645,67646)
#define SPELL_SUMMON_SLIME_POOL_HELPER RAID_MODE<uint32>(66883,67641,67642,67643)
#define SPELL_FIRE_SPIT_HELPER RAID_MODE<uint32>(66796,67632,67633,67634)
#define SPELL_BURNING_SPRAY_HELPER RAID_MODE<uint32>(66902,67627,67628,67629)
#define SPELL_BURNING_BITE_HELPER RAID_MODE<uint32>(66879,67624,67625,67626)
#define SPELL_SWEEP_1_HELPER RAID_MODE<uint32>(66794,67644,67645,67646)

enum BossSpells
{
    // Gormok
    SPELL_IMPALE_10N           = 66331,
	SPELL_IMPALE_25N           = 67477,
	SPELL_IMPALE_10H           = 67478,
	SPELL_IMPALE_25H           = 67479,
    SPELL_STAGGERING_STOMP_10N = 66330,
	SPELL_STAGGERING_STOMP_25N = 67647,
	SPELL_STAGGERING_STOMP_10H = 67648,
	SPELL_STAGGERING_STOMP_25H = 67649,
    SPELL_RISING_ANGER         = 66636,
	// Snobold
    SPELL_SNOBOLLED            = 66406,
    SPELL_BATTER               = 66408,
	SPELL_FIRE_BOMB            = 66313,
	SPELL_FIRE_BOMB_1          = 66317,
	SPELL_FIRE_BOMB_DOT        = 66318,
    SPELL_HEAD_CRACK           = 66407,
	// Acidmaw and Dreadscale (No Raid Mode Spell)
    SPELL_ACID_SPEW            = 66819,
	SPELL_MOLTEN_SPEW          = 66821,
    SPELL_EMERGE_0             = 66947,
    SPELL_SUBMERGE_0           = 66948,
    SPELL_ENRAGE               = 68335,
    SPELL_SLIME_POOL_EFFECT    = 66882,
    // Icehowl
    SPELL_FEROCIOUS_BUTT_10N   = 66770,
	SPELL_FEROCIOUS_BUTT_25N   = 67654,
	SPELL_FEROCIOUS_BUTT_10H   = 67655,
	SPELL_FEROCIOUS_BUTT_25H   = 67656,
    SPELL_MASSIVE_CRASH_10N    = 66683,
	SPELL_MASSIVE_CRASH_25N    = 67660,
	SPELL_MASSIVE_CRASH_10H    = 67661,
	SPELL_MASSIVE_CRASH_25H    = 67662,
    SPELL_WHIRL_10N            = 67345,
	SPELL_WHIRL_25N            = 67663,
	SPELL_WHIRL_10H            = 67664,
	SPELL_WHIRL_25H            = 67665,
    SPELL_ARCTIC_BREATH_10N    = 66689,
	SPELL_ARCTIC_BREATH_25N    = 67650,
	SPELL_ARCTIC_BREATH_10H    = 67651,
	SPELL_ARCTIC_BREATH_25H    = 67652,
    SPELL_STAGGERED_DAZE       = 66758,
	SPELL_TRAMPLE              = 66734,
	// Remove Debuff From Player After Kill Acidmaw & Dreadscale
	SPELL_PARALYTIC_TOXIN_1    = 66823,
	SPELL_PARALYTIC_TOXIN_2    = 67618,
	SPELL_PARALYTIC_TOXIN_3    = 67619,
	SPELL_PARALYTIC_TOXIN_4    = 67620,
	SPELL_BURNING_BILE_1       = 66870,
	SPELL_BURNING_BILE_2       = 67621,
	SPELL_BURNING_BILE_3       = 67622,
	SPELL_BURNING_BILE_4       = 67623,
};

enum MyActions
{
	ACTION_ENABLE_FIRE_BOMB  = 1,
	ACTION_DISABLE_FIRE_BOMB = 2,
};

enum Stages
{
	STAGE_MOBILE        = 0,
	STAGE_SUBMERGE_1    = 1,
	STAGE_WAIT_EMERGE_1 = 2,
	STAGE_EMERGE_1      = 3,
	STAGE_STATIONARY    = 4,
	STAGE_SUBMERGE_2    = 5,
	STAGE_WAIT_EMERGE_2 = 6,
	STAGE_EMERGE_2      = 7,
};

enum Events
{
	EVENT_FEROCIOUS_BUTT  = 1,
    EVENT_MASSIVE_CRASH   = 2,
    EVENT_WHIRL           = 3,
    EVENT_ARCTIC_BREATH   = 4,
    EVENT_TRAMPLE         = 5,
};

class boss_gormok_the_impaler : public CreatureScript
{
public:
    boss_gormok_the_impaler() : CreatureScript("boss_gormok_the_impaler") { }

    struct boss_gormok_the_impalerAI : public QuantumBasicAI
    {
        boss_gormok_the_impalerAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
			instance = creature->GetInstanceScript();
        }

		InstanceScript* instance;

		SummonList Summons;

        uint32 ImpaleTimer;
        uint32 StaggeringStompTimer;
		uint32 ThrowTimer;
        uint32 SummonCount;

        void Reset()
        {
            ImpaleTimer = urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS);
            StaggeringStompTimer = 15*IN_MILLISECONDS;
			ThrowTimer = urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            if (GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL || GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
                SummonCount = 5;
            else
				SummonCount = 4;

            Summons.DespawnAll();
        }

        void EnterEvadeMode()
        {
            instance->DoUseDoorOrButton(instance->GetData64(GO_MAIN_GATE_DOOR));
            QuantumBasicAI::EnterEvadeMode();
        }

        void MovementInform(uint32 type, uint32 pointId)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            switch (pointId)
            {
                case 0:
                    instance->DoUseDoorOrButton(instance->GetData64(GO_MAIN_GATE_DOOR));
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->SetInCombatWithZone();
                    break;
            }
        }

		void CleanGormokAuras()
		{
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_RISING_ANGER);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_IMPALE_10N);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_IMPALE_25N);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_IMPALE_10H);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_IMPALE_25H);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STAGGERING_STOMP_10N);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STAGGERING_STOMP_25N);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STAGGERING_STOMP_10H);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STAGGERING_STOMP_25H);
		}

        void JustDied(Unit* /*killer*/)
        {
			CleanGormokAuras();

			instance->SetData(TYPE_NORTHREND_BEASTS, GORMOK_DONE);
        }

        void JustReachedHome()
        {
			instance->DoUseDoorOrButton(instance->GetData64(GO_MAIN_GATE_DOOR));

			instance->SetData(TYPE_NORTHREND_BEASTS, FAIL);

            me->DespawnAfterAction();
        }

        void EnterToBattle(Unit* /*who*/)
        {
            me->SetInCombatWithZone();
            instance->SetData(TYPE_NORTHREND_BEASTS, GORMOK_IN_PROGRESS);

			for (uint8 i = 0; i < 4; i++)
			{
				if (Creature* snobold = DoSpawnCreature(NPC_SNOBOLD_VASSAL, 0, 0, 0, 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
				{
					snobold->EnterVehicle(me, i);
					snobold->SetInCombatWithZone();
					snobold->AI()->DoAction(ACTION_ENABLE_FIRE_BOMB);
				}
			}
		}

        void JustSummoned(Creature* summon)
        {
            if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 0.0f, true))
            {
                if (summon->GetEntry() == NPC_SNOBOLD_VASSAL)
                {
                    summon->GetMotionMaster()->MoveJump(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 10.0f, 20.0f);

                    --SummonCount;
                }
                summon->AI()->AttackStart(target);
            }
            Summons.Summon(summon);
        }

        void SummonedCreatureDespawn(Creature* summon)
        {
            if (summon->GetEntry() == NPC_SNOBOLD_VASSAL)
			{
                if (summon->IsAlive())
                    ++SummonCount;
			}
            Summons.Despawn(summon);
        }

		void DamageTaken(Unit* /*who*/, uint32& damage)
		{
			if (damage >= me->GetHealth())
			{
				for (uint8 i = 0; i < 4; ++i)
				{
					if (Unit* snobold = me->GetVehicleKit()->GetPassenger(i))
						snobold->ToCreature()->DespawnAfterAction();
				}
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            if (ImpaleTimer <= diff)
            {
				DoCastVictim(RAID_MODE(SPELL_IMPALE_10N, SPELL_IMPALE_25N, SPELL_IMPALE_10H, SPELL_IMPALE_25H));
                ImpaleTimer = urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS);
            }
			else ImpaleTimer -= diff;

            if (StaggeringStompTimer <= diff)
            {
				DoCastAOE(RAID_MODE(SPELL_STAGGERING_STOMP_10N, SPELL_STAGGERING_STOMP_25N, SPELL_STAGGERING_STOMP_10H, SPELL_STAGGERING_STOMP_25H));
                StaggeringStompTimer = urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS);
            }
			else StaggeringStompTimer -= diff;

            if (ThrowTimer <= diff)
            {
				for (uint8 i = 0; i < 4; ++i)
                {
					if (Unit* snobold = me->GetVehicleKit()->GetPassenger(i))
					{
						snobold->ExitVehicle();
						snobold->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
						snobold->ToCreature()->SetReactState(REACT_AGGRESSIVE);
						snobold->ToCreature()->AI()->DoAction(ACTION_DISABLE_FIRE_BOMB);
						snobold->CastSpell(me, SPELL_RISING_ANGER, true);
						DoSendQuantumText(SAY_SNOBOLLED, me);
						break;
					}
                }
                ThrowTimer = urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS);
            }
			else ThrowTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_gormok_the_impalerAI(creature);
    }
};

enum SnoboldVassal
{
	EVENT_FIRE_BOMB  = 1,
	EVENT_BATTER     = 2,
	EVENT_HEAD_CRACK = 3,
};

class npc_snobold_vassal : public CreatureScript
{
public:
    npc_snobold_vassal() : CreatureScript("npc_snobold_vassal") { }

    struct npc_snobold_vassalAI : public QuantumBasicAI
    {
        npc_snobold_vassalAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
			instance->SetData(DATA_SNOBOLD_COUNT, INCREASE);
        }

		InstanceScript* instance;

		EventMap events;

        uint64 BossGUID;
        uint64 TargetGUID;

        bool bTargetDied;

        void Reset()
        {
            events.ScheduleEvent(EVENT_BATTER, 5*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_HEAD_CRACK, 25*IN_MILLISECONDS);

            TargetGUID = 0;
            bTargetDied = false;

			BossGUID = instance->GetData64(NPC_GORMOK_THE_IMPALER);

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        }

        void EnterEvadeMode()
        {
            QuantumBasicAI::EnterEvadeMode();
        }

        void EnterToBattle(Unit* who)
        {
            TargetGUID = who->GetGUID();
            me->TauntApply(who);
            DoCast(who, SPELL_SNOBOLLED, true);
        }

		/*void DamageTaken(Unit* DoneBy, uint32 &damage)
        {
            if (DoneBy->GetGUID() == TargetGUID)
                damage = 0;
        }*/

        void MovementInform(uint32 type, uint32 pointId)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            switch (pointId)
            {
                case 0:
                    if (bTargetDied)
                        me->DespawnAfterAction();
                    break;
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            if (Unit* target = ObjectAccessor::GetPlayer(*me, TargetGUID))
			{
                if (target->IsAlive())
                    target->RemoveAurasDueToSpell(SPELL_SNOBOLLED);
			}
			
			instance->SetData(DATA_SNOBOLD_COUNT, DECREASE);
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
				case ACTION_ENABLE_FIRE_BOMB:
					events.ScheduleEvent(EVENT_FIRE_BOMB, urand(5*IN_MILLISECONDS, 30*IN_MILLISECONDS));
					break;
                case ACTION_DISABLE_FIRE_BOMB:
                    events.CancelEvent(EVENT_FIRE_BOMB);
                    break;
            }
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim() || bTargetDied)
                return;

            if (Unit* target = ObjectAccessor::GetPlayer(*me, TargetGUID))
            {
                if (!target->IsAlive())
                {
					Unit* gormok = ObjectAccessor::GetCreature(*me, instance->GetData64(NPC_GORMOK_THE_IMPALER));
					if (gormok && gormok->IsAlive())
					{
						SetCombatMovement(false);
						bTargetDied = true;

						for (uint8 i = 0; i < 4; i++)
						{
							if (!gormok->GetVehicleKit()->GetPassenger(i))
							{
								me->EnterVehicle(gormok, i);
								DoAction(ACTION_ENABLE_FIRE_BOMB);
								break;
							}
						}
					}
					else if (Unit* target2 = SelectTarget(TARGET_RANDOM, 0))
					{
						TargetGUID = target2->GetGUID();
						me->GetMotionMaster()->MoveJump(target2->GetPositionX(), target2->GetPositionY(), target2->GetPositionZ(), 15.0f, 15.0f);
                    }
                }
            }

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

			while (uint32 event = events.ExecuteEvent())
            {
                switch (event)
                {
                    case EVENT_FIRE_BOMB:
						if (me->GetVehicleBase())
						{
							if (Unit* target = SelectTarget(TARGET_RANDOM, 0, -me->GetVehicleBase()->GetCombatReach()))
								me->CastSpell(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), SPELL_FIRE_BOMB, true);
						}
                        events.ScheduleEvent(EVENT_FIRE_BOMB, 20*IN_MILLISECONDS);
                        return;
                    case EVENT_HEAD_CRACK:
                        DoCastVictim(SPELL_HEAD_CRACK);
                        events.ScheduleEvent(EVENT_HEAD_CRACK, 30*IN_MILLISECONDS);
                        return;
                    case EVENT_BATTER:
                        DoCastVictim(SPELL_BATTER);
                        events.ScheduleEvent(EVENT_BATTER, 10*IN_MILLISECONDS);
                        return;
                }
            }

            if (instance->GetData(TYPE_NORTHREND_BEASTS) == FAIL)
                me->DespawnAfterAction();

            if (!me->GetVehicleBase())
                DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_snobold_vassalAI(creature);
    }
};

class npc_fire_bomb_toc : public CreatureScript
{
public:
    npc_fire_bomb_toc() : CreatureScript("npc_fire_bomb_toc") { }

    struct npc_fire_bomb_tocAI : public QuantumBasicAI
    {
        npc_fire_bomb_tocAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();

			SetCombatMovement(false);
        }

		InstanceScript* instance;

        void Reset()
        {
            DoCast(me, SPELL_FIRE_BOMB_DOT, true);

            me->SetReactState(REACT_PASSIVE);
        }

        void UpdateAI(uint32 const /*diff*/)
        {
            if (instance->GetData(TYPE_NORTHREND_BEASTS) != GORMOK_IN_PROGRESS)
                me->DespawnAfterAction();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fire_bomb_tocAI(creature);
    }
};

struct boss_jormungarAI : public QuantumBasicAI
{
    boss_jormungarAI(Creature* creature) : QuantumBasicAI(creature)
    {
        instance = creature->GetInstanceScript();
    }

	InstanceScript* instance;

    uint32 OtherWormEntry;

    uint32 ModelStationary;
    uint32 ModelMobile;

    uint32 BiteSpell;
    uint32 SpewSpell;
    uint32 SpitSpell;
    uint32 SpraySpell;

    uint32 BiteTimer;
    uint32 SpewTimer;
    uint32 SlimePoolTimer;
    uint32 SpitTimer;
    uint32 SprayTimer;
    uint32 SweepTimer;
    uint32 SubmergeTimer;

    uint8 Stage;

    bool Enraged;

    void Reset()
    {
        Enraged = false;
        BiteTimer = urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS);
        SpewTimer = urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS);
        SlimePoolTimer = 15*IN_MILLISECONDS;
        SpitTimer = urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS);
        SprayTimer = urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS);
        SweepTimer = urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS);

		DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

		me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
    }

	void CleanJormungarAuras()
	{
		instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PARALYTIC_TOXIN_1);
		instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PARALYTIC_TOXIN_2);
		instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PARALYTIC_TOXIN_3);
		instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PARALYTIC_TOXIN_4);
		instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BURNING_BITE_HELPER);
		instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BURNING_BILE_1);
		instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BURNING_BILE_2);
		instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BURNING_BILE_3);
		instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BURNING_BILE_4);
	}

    void JustDied(Unit* /*killer*/)
    {
		if (Creature* otherWorm = Unit::GetCreature(*me, instance->GetData64(OtherWormEntry)))
		{
			if (!otherWorm->IsAlive())
			{
				instance->SetData(TYPE_NORTHREND_BEASTS, SNAKES_DONE);

				CleanJormungarAuras();

				me->DespawnAfterAction();
				otherWorm->DespawnAfterAction();
			}
			else
				instance->SetData(TYPE_NORTHREND_BEASTS, SNAKES_SPECIAL);
        }
    }

    void JustReachedHome()
    {
		if (instance->GetData(TYPE_NORTHREND_BEASTS) != FAIL)
            instance->SetData(TYPE_NORTHREND_BEASTS, FAIL);

        me->DespawnAfterAction();
    }

    void EnterToBattle(Unit* /*who*/)
    {
        me->SetInCombatWithZone();

		instance->SetData(TYPE_NORTHREND_BEASTS, SNAKES_IN_PROGRESS);
    }

    void UpdateAI(uint32 const diff)
    {
        if (!UpdateVictim()) return;

        if (instance->GetData(TYPE_NORTHREND_BEASTS) == SNAKES_SPECIAL && !Enraged)
        {
            DoSendQuantumText(SAY_EMERGE, me);
            me->RemoveAurasDueToSpell(SPELL_SUBMERGE_0);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            DoCast(SPELL_ENRAGE);
            Enraged = true;
            DoSendQuantumText(SAY_BERSERK, me);
        }

        switch (Stage)
        {
            case STAGE_MOBILE:
                if (BiteTimer <= diff)
                {
                    DoCastVictim(BiteSpell);
                    BiteTimer = urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS);
                }
				else BiteTimer -= diff;

                if (SpewTimer <= diff)
                {
                    DoCastAOE(SpewSpell);
                    SpewTimer = urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS);
                }
				else SpewTimer -= diff;

                if (SlimePoolTimer <= diff)
                {
                    DoCast(me, SPELL_SUMMON_SLIME_POOL_HELPER);
                    SlimePoolTimer = 30*IN_MILLISECONDS;
                }
				else SlimePoolTimer -= diff;

                if (SubmergeTimer <= diff && !Enraged)
                {
                    Stage = STAGE_SUBMERGE_1;
                    SubmergeTimer = 5*IN_MILLISECONDS;
                }
				else SubmergeTimer -= diff;
                DoMeleeAttackIfReady();
                break;
            case STAGE_SUBMERGE_1:
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                DoCast(me, SPELL_SUBMERGE_0);
                DoSendQuantumText(SAY_SUBMERGE, me);
                me->GetMotionMaster()->MovePoint(0, ToCCommonLoc[1].GetPositionX()+ frand(-40.0f, 40.0f), ToCCommonLoc[1].GetPositionY() + frand(-40.0f, 40.0f), ToCCommonLoc[1].GetPositionZ());
				Stage = STAGE_WAIT_EMERGE_1;
				break;
			case STAGE_WAIT_EMERGE_1:
                if (SubmergeTimer <= diff)
                {
                    Stage = STAGE_EMERGE_1;
                    SubmergeTimer = 50*IN_MILLISECONDS;
                }
				else SubmergeTimer -= diff;
                break;
            case STAGE_EMERGE_1:
                me->SetDisplayId(ModelStationary);
                DoSendQuantumText(SAY_EMERGE, me);
                me->RemoveAurasDueToSpell(SPELL_SUBMERGE_0);
                DoCast(me, SPELL_EMERGE_0);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                SetCombatMovement(false);
                me->GetMotionMaster()->MoveIdle();
                Stage = STAGE_STATIONARY;
                break;
            case STAGE_STATIONARY:
                if (SprayTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, SpraySpell);
						SprayTimer = urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS);
					}
                }
				else SprayTimer -= diff;

                if (SweepTimer <= diff)
                {
                    DoCastAOE(SPELL_SWEEP_0_HELPER);
                    SweepTimer = urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS);
                }
				else SweepTimer -= diff;

                if (SubmergeTimer <= diff)
                {
                    Stage = STAGE_SUBMERGE_2;
                    SubmergeTimer = 10*IN_MILLISECONDS;
                }
				else SubmergeTimer -= diff;

				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
				me->GetMotionMaster()->MoveIdle();
				me->GetMotionMaster()->Clear();
                DoSpellAttackIfReady(SpitSpell);
                break;
            case STAGE_SUBMERGE_2:
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                DoCast(me, SPELL_SUBMERGE_0);
                DoSendQuantumText(SAY_SUBMERGE, me);
                me->GetMotionMaster()->MovePoint(0, ToCCommonLoc[1].GetPositionX() + frand(-40.0f, 40.0f), ToCCommonLoc[1].GetPositionY() + frand(-40.0f, 40.0f), ToCCommonLoc[1].GetPositionZ());
				Stage = STAGE_WAIT_EMERGE_2;
				break;
			case STAGE_WAIT_EMERGE_2:
                if (SubmergeTimer <= diff)
                {
                    Stage = STAGE_EMERGE_2;
                    SubmergeTimer = 45*IN_MILLISECONDS;
                }
				else SubmergeTimer -= diff;
                break;
            case STAGE_EMERGE_2:
                me->SetDisplayId(ModelMobile);
                DoSendQuantumText(SAY_EMERGE, me);
                me->RemoveAurasDueToSpell(SPELL_SUBMERGE_0);
                DoCast(me, SPELL_EMERGE_0);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                SetCombatMovement(true);
                me->GetMotionMaster()->MoveChase(me->GetVictim());
                Stage = STAGE_MOBILE;
                break;
        }
    }
};

class boss_acidmaw : public CreatureScript
{
public:
	boss_acidmaw() : CreatureScript("boss_acidmaw") { }

    struct boss_acidmawAI : public boss_jormungarAI
    {
        boss_acidmawAI(Creature* creature) : boss_jormungarAI(creature) { }

        void Reset()
        {
            boss_jormungarAI::Reset();
            BiteSpell = SPELL_PARALYTIC_BITE_HELPER;
			SpraySpell = SPELL_PARALYTIC_SPRAY_HELPER;
            SpitSpell = SPELL_ACID_SPITE_HELPER;
			SpewSpell = SPELL_ACID_SPEW;
            ModelStationary = MODEL_ACIDMAW_STATIONARY;
            ModelMobile = MODEL_ACIDMAW_MOBILE;
            OtherWormEntry = NPC_DREADSCALE;

            SubmergeTimer = 500;
            DoCast(me, SPELL_SUBMERGE_0);
            Stage = STAGE_WAIT_EMERGE_1;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_acidmawAI(creature);
    }
};

class boss_dreadscale : public CreatureScript
{
public:
    boss_dreadscale() : CreatureScript("boss_dreadscale") { }

    struct boss_dreadscaleAI : public boss_jormungarAI
    {
        boss_dreadscaleAI(Creature* creature) : boss_jormungarAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        void Reset()
        {
            boss_jormungarAI::Reset();
			SpitSpell = SPELL_FIRE_SPIT_HELPER;
			SpraySpell = SPELL_BURNING_SPRAY_HELPER;
            BiteSpell = SPELL_BURNING_BITE_HELPER;
            SpewSpell = SPELL_MOLTEN_SPEW;
            ModelStationary = MODEL_DREADSCALE_STATIONARY;
            ModelMobile = MODEL_DREADSCALE_MOBILE;
            OtherWormEntry = NPC_ACIDMAW;

            SubmergeTimer = 45 * IN_MILLISECONDS;
            Stage = STAGE_MOBILE;
        }

        void MovementInform(uint32 type, uint32 pointId)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            switch (pointId)
            {
                case 0:
                    instance->DoUseDoorOrButton(instance->GetData64(GO_MAIN_GATE_DOOR));
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->SetInCombatWithZone();
                    if (Creature* otherWorm = Unit::GetCreature(*me, instance->GetData64(OtherWormEntry)))
                    {
						otherWorm->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        otherWorm->SetReactState(REACT_AGGRESSIVE);
                        otherWorm->SetVisible(true);
                        otherWorm->SetInCombatWithZone();
                    }
                    break;
            }
        }

        void EnterEvadeMode()
        {
            instance->DoUseDoorOrButton(instance->GetData64(GO_MAIN_GATE_DOOR));
            boss_jormungarAI::EnterEvadeMode();
        }

        void JustReachedHome()
        {
			instance->DoUseDoorOrButton(instance->GetData64(GO_MAIN_GATE_DOOR));

            boss_jormungarAI::JustReachedHome();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
	{
		return new boss_dreadscaleAI(creature);
    }
};

class npc_slime_pool : public CreatureScript
{
public:
    npc_slime_pool() : CreatureScript("npc_slime_pool") { }

    struct npc_slime_poolAI : public QuantumBasicAI
    {
        npc_slime_poolAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();

			SetCombatMovement(false);
		}

		InstanceScript* instance;

        bool Casted;

        void Reset()
        {
            Casted = false;
            me->SetReactState(REACT_PASSIVE);
        }

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!Casted)
            {
                Casted = true;
                DoCast(me, SPELL_SLIME_POOL_EFFECT);
            }

			if (instance->GetData(TYPE_NORTHREND_BEASTS) != SNAKES_IN_PROGRESS)
				me->DespawnAfterAction();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_slime_poolAI(creature);
    }
};

#define SPELL_FROTHING_RAGE_HELPER RAID_MODE<uint32>(66759,67657,67658,67659)

class boss_icehowl : public CreatureScript
{
    public:
        boss_icehowl() : CreatureScript("boss_icehowl") { }

        struct boss_icehowlAI : public BossAI
        {
            boss_icehowlAI(Creature* creature) : BossAI(creature, TYPE_NORTHREND_BEASTS){}

            void Reset()
            {
				events.ScheduleEvent(EVENT_FEROCIOUS_BUTT, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_ARCTIC_BREATH, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_WHIRL, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_MASSIVE_CRASH, 30*IN_MILLISECONDS);

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

                _movementStarted = false;
                _movementFinish = false;
                _trampleCasted = false;
                _trampleTargetGUID = 0;
                _trampleTargetX = 0;
                _trampleTargetY = 0;
                _trampleTargetZ = 0;
                _stage = 0;
            }

			void CleanIceHowlAuras()
			{
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FEROCIOUS_BUTT_10N);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FEROCIOUS_BUTT_25N);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FEROCIOUS_BUTT_10H);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FEROCIOUS_BUTT_25H);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MASSIVE_CRASH_10N);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MASSIVE_CRASH_25N);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MASSIVE_CRASH_10H);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MASSIVE_CRASH_25H);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WHIRL_10N);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WHIRL_25N);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WHIRL_25N);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_WHIRL_25N);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ARCTIC_BREATH_10N);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ARCTIC_BREATH_25N);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ARCTIC_BREATH_10H);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_ARCTIC_BREATH_25H);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FROTHING_RAGE_HELPER);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_STAGGERED_DAZE);
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TRAMPLE);
			}

			void EnterToBattle(Unit* /*who*/)
            {
                _EnterToBattle();

				instance->SetData(TYPE_NORTHREND_BEASTS, ICEHOWL_IN_PROGRESS);
            }

			void JustDied(Unit* /*killer*/)
            {
                _JustDied();

				instance->SetData(TYPE_NORTHREND_BEASTS, ICEHOWL_DONE);
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_TRAMPLE && target->GetTypeId() == TYPE_ID_PLAYER)
                {
                    if (!_trampleCasted)
                    {
						DoCast(me, SPELL_FROTHING_RAGE_HELPER, true);
                        _trampleCasted = true;
                    }
                }
            }

            void MovementInform(uint32 type, uint32 pointId)
            {
                if (type != POINT_MOTION_TYPE && type != EFFECT_MOTION_TYPE)
                    return;

                switch (pointId)
                {
                    case 0:
                        if (_stage != 0)
                        {
                            if (me->GetDistance2d(ToCCommonLoc[1].GetPositionX(), ToCCommonLoc[1].GetPositionY()) < 6.0f)
                                // Middle of the room
                                _stage = 1;
                            else
                            {
                                // Landed from Hop backwards (start trample)
                                if (ObjectAccessor::GetPlayer(*me, _trampleTargetGUID))
                                    _stage = 4;
                                else
                                    _stage = 6;
                            }
                        }
                        break;
                    case 1: // Finish trample
                        _movementFinish = true;
                        break;
                    case 2:
                        instance->DoUseDoorOrButton(instance->GetData64(GO_MAIN_GATE_DOOR));
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->SetInCombatWithZone();
                        break;
                    default:
                        break;
                }
            }

            void EnterEvadeMode()
            {
                instance->DoUseDoorOrButton(instance->GetData64(GO_MAIN_GATE_DOOR));
                QuantumBasicAI::EnterEvadeMode();
            }

            void JustReachedHome()
            {
				instance->DoUseDoorOrButton(instance->GetData64(GO_MAIN_GATE_DOOR));
				instance->SetData(TYPE_NORTHREND_BEASTS, FAIL);

                me->DespawnAfterAction();
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                switch (_stage)
                {
                    case 0:
                    {
                        while (uint32 eventId = events.ExecuteEvent())
                        {
                            switch (eventId)
                            {
                                case EVENT_FEROCIOUS_BUTT:
									DoCastVictim(RAID_MODE(SPELL_FEROCIOUS_BUTT_10N, SPELL_FEROCIOUS_BUTT_25N, SPELL_FEROCIOUS_BUTT_10H, SPELL_FEROCIOUS_BUTT_25H));
									events.ScheduleEvent(EVENT_FEROCIOUS_BUTT, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                                    return;
                                case EVENT_ARCTIC_BREATH:
                                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 0.0f, true))
									{
                                        DoCast(target, RAID_MODE(SPELL_ARCTIC_BREATH_10N, SPELL_ARCTIC_BREATH_25N, SPELL_ARCTIC_BREATH_10H, SPELL_ARCTIC_BREATH_25H));
									}
                                    return;
                                case EVENT_WHIRL:
                                    DoCastAOE(RAID_MODE(SPELL_WHIRL_10N, SPELL_WHIRL_25N, SPELL_WHIRL_10H, SPELL_WHIRL_25H));
                                    events.ScheduleEvent(EVENT_WHIRL, urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                                    return;
                                case EVENT_MASSIVE_CRASH:
									me->GetMotionMaster()->MoveJump(ToCCommonLoc[1].GetPositionX(), ToCCommonLoc[1].GetPositionY(), ToCCommonLoc[1].GetPositionZ(), 20.0f, 20.0f, 0); // 1: Middle of the room
                                    SetCombatMovement(false);
                                    me->AttackStop();
                                    _stage = 7; //Invalid (Do nothing more than move)
                                    return;
                                default:
                                    break;
                            }
                        }
                        DoMeleeAttackIfReady();
                        break;
                    }
                    case 1:
                        DoCast(me, RAID_MODE(SPELL_MASSIVE_CRASH_10N, SPELL_MASSIVE_CRASH_25N, SPELL_MASSIVE_CRASH_10H, SPELL_MASSIVE_CRASH_25H), true);
                        me->StopMoving();
                        me->AttackStop();
                        _stage = 2;
                        break;
                    case 2:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 0.0f, true))
                        {
                            me->StopMoving();
                            me->AttackStop();
                            _trampleTargetGUID = target->GetGUID();
                            me->SetTarget(_trampleTargetGUID);
							DoSendQuantumText(SAY_TRAMPLE_STARE, me, target);
                            _trampleCasted = false;
                            SetCombatMovement(false);
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE);
                            me->GetMotionMaster()->Clear();
                            me->GetMotionMaster()->MoveIdle();
                            events.ScheduleEvent(EVENT_TRAMPLE, 4*IN_MILLISECONDS);
                            _stage = 3;
                        }
                        else
                            _stage = 6;
                        break;
                    case 3:
                        while (uint32 eventId = events.ExecuteEvent())
                        {
                            switch (eventId)
                            {
                                case EVENT_TRAMPLE:
                                {
                                    if (Unit* target = ObjectAccessor::GetPlayer(*me, _trampleTargetGUID))
                                    {
                                        me->StopMoving();
                                        me->AttackStop();
                                        _trampleCasted = false;
                                        _movementStarted = true;
                                        _trampleTargetX = target->GetPositionX();
                                        _trampleTargetY = target->GetPositionY();
                                        _trampleTargetZ = target->GetPositionZ();
                                        // 2: Hop Backwards
										me->GetMotionMaster()->MoveJump(2*me->GetPositionX() - _trampleTargetX, 2*me->GetPositionY() - _trampleTargetY, me->GetPositionZ(), 30.0f, 20.0f, 0);
                                        _stage = 7; //Invalid (Do nothing more than move)
                                    }
                                    else
                                        _stage = 6;
                                    break;
                                }
                                default:
                                    break;
                            }
                        }
                        break;
                    case 4:
                        me->StopMoving();
                        me->AttackStop();
						DoSendQuantumText(SAY_TRAMPLE_START, me);
                        me->GetMotionMaster()->MoveCharge(_trampleTargetX, _trampleTargetY, _trampleTargetZ, 42, 1);
                        me->SetTarget(0);
                        _stage = 5;
                        break;
                    case 5:
                        if (_movementFinish)
                        {
                            DoCastAOE(SPELL_TRAMPLE);
                            _movementFinish = false;
                            _stage = 6;
                            return;
                        }
                        if (events.ExecuteEvent() == EVENT_TRAMPLE)
                        {
                            Map::PlayerList const &lPlayers = me->GetMap()->GetPlayers();
                            for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                            {
								if (Player* player = itr->getSource())
                                {
                                    if (player->IsAlive() && player->IsWithinDistInMap(me, 6.0f))
                                    {
                                        DoCastAOE(SPELL_TRAMPLE);
                                        events.ScheduleEvent(EVENT_TRAMPLE, 4*IN_MILLISECONDS);
                                        break;
                                    }
                                }
                            }
                        }
                        break;
                    case 6:
                        if (!_trampleCasted)
						{
                            DoCast(me, SPELL_STAGGERED_DAZE);
						}
                        else
                        {
                            DoCast(me, SPELL_FROTHING_RAGE_HELPER, true);
                            DoSendQuantumText(SAY_TRAMPLE_FAIL, me);
                        }
                        _movementStarted = false;
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE);
                        SetCombatMovement(true);
                        me->GetMotionMaster()->MovementExpired();
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveChase(me->GetVictim());
                        AttackStart(me->GetVictim());
                        events.ScheduleEvent(EVENT_MASSIVE_CRASH, 40*IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_ARCTIC_BREATH, urand(15*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                        _stage = 0;
                        break;
                    default:
                        break;
                }
            }

            private:
                float  _trampleTargetX, _trampleTargetY, _trampleTargetZ;
                uint64 _trampleTargetGUID;
                bool   _movementStarted;
                bool   _movementFinish;
                bool   _trampleCasted;
                uint8  _stage;
                Unit*  _target;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_icehowlAI(creature);
        }
};

class spell_gormok_fire_bomb : public SpellScriptLoader
{
    public:
        spell_gormok_fire_bomb() : SpellScriptLoader("spell_gormok_fire_bomb") { }

        class spell_gormok_fire_bomb_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gormok_fire_bomb_SpellScript);

            void TriggerFireBomb(SpellEffIndex /*effIndex*/)
            {
                if (const WorldLocation* pos = GetExplTargetDest())
                {
                    if (Unit* caster = GetCaster())
                        caster->SummonCreature(NPC_FIRE_BOMB, pos->GetPositionX(), pos->GetPositionY(), pos->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 30*IN_MILLISECONDS);
                }
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gormok_fire_bomb_SpellScript::TriggerFireBomb, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gormok_fire_bomb_SpellScript();
        }
};

void AddSC_boss_northrend_beasts()
{
    new boss_gormok_the_impaler();
    new npc_snobold_vassal();
	new npc_fire_bomb_toc();
    new boss_acidmaw();
    new boss_dreadscale();
    new npc_slime_pool();
    new boss_icehowl();
	new spell_gormok_fire_bomb();
}