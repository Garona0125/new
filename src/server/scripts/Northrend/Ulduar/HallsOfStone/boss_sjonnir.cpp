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
#include "halls_of_stone.h"

enum Texts
{
    SAY_AGGRO              = -1599011,
    SAY_SLAY_1             = -1599012,
    SAY_SLAY_2             = -1599013,
    SAY_SLAY_3             = -1599014,
    SAY_DEATH              = -1599015,
	SAY_FRENZY             = -1000002,
	SAY_SPAWN_DWARF        = -1599025,
    SAY_SPAWN_TROGG        = -1599026,
    SAY_SPAWN_OOZE         = -1599027,
    SAY_SPAWN_EARTHEN      = -1599028,
	SAY_VICTORY_SJONNIR_1  = -1599066,
    SAY_VICTORY_SJONNIR_2  = -1599067,
};

enum Spells
{
	SPELL_LIGHTING_RING_5N   = 51849,
	SPELL_LIGHTING_RING_5H   = 59861,
	SPELL_LIGHTING_RING_1_5N = 50840,
	SPELL_LIGHTING_RING_1_5H = 59848,
	SPELL_STATIC_CHARGE_5N   = 50834,
	SPELL_STATIC_CHARGE_5H   = 59846,
	SPELL_CHAIN_LIGHTING_5N  = 50830,
	SPELL_CHAIN_LIGHTING_5H  = 59844,
	SPELL_LIGHTING_SHIELD_5N = 50831,
	SPELL_LIGHTING_SHIELD_5H = 59845,
	SPELL_TOXIC_VOLLEY_5N    = 50838,
	SPELL_TOXIC_VOLLEY_5H    = 59853,
	SPELL_FRENZY             = 28747,
	SPELL_IRON_SLUDGE_VISUAL = 50777,
};

enum Misc
{
	ACHIEVEMENT_ABUSE_THE_OOZE = 2155,
	DATA_TIME_BEFORE_OOZE      = 150000,
	// Actions
	ACTION_OOZE_DEAD           = 1,
	DATA_ABUSE_THE_OOZE        = 2,
};

struct Locations
{
    float x, y, z;
};

static Locations CenterPoint = {1295.21f, 667.157f, 189.691f};

static Locations PipeLocations[] =
{
    {1295.44f, 734.07f, 200.3f}, //left
    {1297.7f,  595.6f,  199.9f}, //right
};

class boss_sjonnir : public CreatureScript
{
public:
    boss_sjonnir() : CreatureScript("boss_sjonnir") { }

    struct boss_sjonnirAI : public QuantumBasicAI
    {
        boss_sjonnirAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
        }

        bool bIsFrenzy;

        uint32 ChainLightningTimer;
        uint32 StaticChargeTimer;
        uint32 LightningRingTimer;
        uint32 LightningShieldTimer;
        uint32 SummonTimer;
        uint32 CheckPhaseTimer;
        uint8 abuseTheOoze;
        uint32 SummonEntry;        
        uint8 SummonPhase;

        SummonList Summons;

        InstanceScript* instance;

        void Reset()
        {
            bIsFrenzy = false;

            ChainLightningTimer = urand(5000, 8000);
            StaticChargeTimer = urand(15000, 20000);
            LightningRingTimer = urand(25000, 30000);
            LightningShieldTimer = urand(10000, 15000);
            SummonTimer = 1000;
            CheckPhaseTimer = 1000;
            abuseTheOoze = 0;
            SummonPhase = 1;
            SummonEntry = NPC_FORGED_IRON_DWARF;

            Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			instance->SetData(DATA_SJONNIR_EVENT, NOT_STARTED);
            
            CheckLightningShield();
        }

        void CheckLightningShield()
        {
            if (IsHeroic())
                me->RemoveAurasDueToSpell(SPELL_LIGHTING_SHIELD_5N);

            if (!me->HasAura(SPELL_LIGHTING_SHIELD_5N || SPELL_LIGHTING_SHIELD_5H))
				DoCast(me, DUNGEON_MODE(SPELL_LIGHTING_SHIELD_5N, SPELL_LIGHTING_SHIELD_5H));
        }

        void JustReachedHome()
        {
            CheckLightningShield();
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

			if (GameObject* door = instance->instance->GetGameObject(instance->GetData64(DATA_SJONNIR_DOOR)))
			{
				if (door->GetGoState() == GO_STATE_READY)
				{
					EnterEvadeMode();
					return;
				}
			}

			instance->SetData(DATA_SJONNIR_EVENT, IN_PROGRESS);
        }

		void KilledUnit(Unit* victim)
        {
            if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
        }

		void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

            Summons.DespawnAll();

            me->SummonCreature(NPC_BRANN_BRONZEBEARD, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());

			instance->SetData(DATA_SJONNIR_EVENT, DONE);
        }

		void JustSummoned(Creature* summon)
        {
            switch (summon->GetEntry())
            {
                case NPC_EARTHEN_DWARF:
                    summon->AI()->AttackStart(me);
                    break;
                case NPC_FORGED_IRON_DWARF:
                case NPC_FORGED_IRON_TROGG:
                    if (Unit* target = me->GetVictim())
                        summon->AI()->AttackStart(target);
                    break;
                case NPC_MALFORMED_OOZE:
                    summon->SetReactState(REACT_PASSIVE);
                    summon->GetMotionMaster()->MovePoint(0, CenterPoint.x, CenterPoint.y, CenterPoint.z);
                    break;
            }

            if (summon->GetEntry() != NPC_BRANN_BRONZEBEARD)
                Summons.Summon(summon);

            if (summon->GetEntry() == NPC_FORGED_IRON_TROGG) // DB?
                summon->SetCurrentFaction(16);
        }

		void DoAction(int32 const action)
        {
            if (action == ACTION_OOZE_DEAD)
                ++abuseTheOoze;
        }

        uint32 GetData(uint32 type)
        {
            if (type == DATA_ABUSE_THE_OOZE)
                return abuseTheOoze;

            return 0;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ChainLightningTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
					DoCast(target, DUNGEON_MODE(SPELL_CHAIN_LIGHTING_5N, SPELL_CHAIN_LIGHTING_5H));
					ChainLightningTimer = urand(12000, 15000);
				}
            }
			else ChainLightningTimer -= diff;

            if (StaticChargeTimer <= diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
					DoCast(target, DUNGEON_MODE(SPELL_STATIC_CHARGE_5N, SPELL_STATIC_CHARGE_5H));
					StaticChargeTimer = 25000;
				}
            }
			else StaticChargeTimer -= diff;

            if (LightningRingTimer <= diff)
            {
				DoCast(me, DUNGEON_MODE(SPELL_LIGHTING_RING_5N, SPELL_LIGHTING_RING_5H));
				LightningRingTimer = urand(30000, 35000);
            }
			else LightningRingTimer -= diff;

            if (LightningShieldTimer <= diff)
            {
				CheckLightningShield();
				LightningShieldTimer = urand(45000, 55000);
            }
			else LightningShieldTimer -= diff;

            if (CheckPhaseTimer <= diff)
            {
                if (HealthBelowPct(HEALTH_PERCENT_75) && SummonPhase == 1)
                {
                    SummonEntry = NPC_FORGED_IRON_TROGG;
                    SummonTimer = 1000;
                    SummonPhase = 2;
                } 
                else if (HealthBelowPct(HEALTH_PERCENT_50) && SummonPhase == 2)
                {
                    SummonEntry = NPC_MALFORMED_OOZE;
                    SummonTimer = 1000;
                    SummonPhase = 3;
                }
                else if (HealthBelowPct(HEALTH_PERCENT_25) && SummonPhase == 3)
                {
                    SummonEntry = NPC_EARTHEN_DWARF;
                    SummonTimer = 1000;
                    SummonPhase = 4;
                }
                
                CheckPhaseTimer = 1000;

            }
			else CheckPhaseTimer -= diff;

            if (SummonTimer <= diff)
            {
                uint32 rnd = urand(0, 1);
                
                if (SummonEntry)
                    me->SummonCreature(SummonEntry, PipeLocations[rnd].x, PipeLocations[rnd].y, PipeLocations[rnd].z, 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);

                switch (SummonPhase)
                {
                    case 1:
						SummonTimer = 20000;
						break;
                    case 2:
						SummonTimer = 7500;
						break;
                    case 3:
						SummonTimer = 2500;
						break;
                    case 4:
						SummonTimer = 5000;
						break;
                }
            }
			else SummonTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_25))
			{
				if (!me->HasAuraEffect(SPELL_FRENZY, 0))
				{
					DoSendQuantumText(SAY_FRENZY, me);
					DoCast(me, SPELL_FRENZY);
				}
			}

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_sjonnirAI(creature);
    }
};

class npc_malformed_ooze : public CreatureScript
{
public:
    npc_malformed_ooze() : CreatureScript("npc_malformed_ooze") { }

    struct npc_malformed_oozeAI : public QuantumBasicAI
    {
        npc_malformed_oozeAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 MergeTimer;

        void Reset()
        {
            MergeTimer = 7500;
        }

        void JustSummoned(Creature* summon)
        {             
			if (Creature* sjonnir = Unit::GetCreature(*me, instance->GetData64(DATA_SJONNIR)))
			{
				if (Unit* target = CAST_AI(boss_sjonnir::boss_sjonnirAI, sjonnir->AI())->SelectTarget(TARGET_RANDOM, 0, 100, true))
					summon->AI()->AttackStart(target);
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (MergeTimer <= diff)
            {
                if (Creature* creature = me->FindCreatureWithDistance(NPC_MALFORMED_OOZE, 5.0f, true)) // can return self?
				{
                    if (creature != me)
                    {
                        DoSpawnCreature(NPC_IRON_SLUDGE, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);

                        creature->DisappearAndDie();
                        me->DisappearAndDie();
                    }
				}

                MergeTimer = 7500;
            }
			else MergeTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_malformed_oozeAI(creature);
    }
};

class npc_iron_sludge : public CreatureScript
{
public:
    npc_iron_sludge() : CreatureScript("npc_iron_sludge") { }

    struct npc_iron_sludgeAI : public QuantumBasicAI
    {
        npc_iron_sludgeAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        uint32 ToxicVolleyTimer;
        
        void Reset()
        {
			DoCast(me, SPELL_IRON_SLUDGE_VISUAL);

            ToxicVolleyTimer = 2000;
        }

		void JustDied(Unit* /*killer*/)
        {
			if (Creature* sjonnir = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_SJONNIR)))
				sjonnir->AI()->DoAction(ACTION_OOZE_DEAD);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (ToxicVolleyTimer <= diff)
            {
				DoCastAOE(DUNGEON_MODE(SPELL_TOXIC_VOLLEY_5N, SPELL_TOXIC_VOLLEY_5H));
                ToxicVolleyTimer = urand(5000, 10000);
            }
			else ToxicVolleyTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_iron_sludgeAI(creature);
    }
};

class achievement_abuse_the_ooze : public AchievementCriteriaScript
{
public:
	achievement_abuse_the_ooze() : AchievementCriteriaScript("achievement_abuse_the_ooze") { }

	bool OnCheck(Player* /*player*/, Unit* target)
	{
		if (!target)
			return false;

		if (Creature* Sjonnir = target->ToCreature())
			if (Sjonnir->AI()->GetData(DATA_ABUSE_THE_OOZE) >= 5)
				return true;

		return false;
	}
};

void AddSC_boss_sjonnir()
{
    new boss_sjonnir();
    new npc_malformed_ooze();
    new npc_iron_sludge();
    new achievement_abuse_the_ooze();
}