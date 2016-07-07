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
#include "nexus.h"

enum Texts
{
    SAY_AGGRO  = -1576010,
    SAY_DEATH  = -1576011,
    SAY_RIFT   = -1576012,
    SAY_SHIELD = -1576013,
};

enum Spells
{
    SPELL_SPARK_5N                     = 47751,
    SPELL_SPARK_5H                     = 57062,
    SPELL_RIFT_SHIELD                  = 47748,
    SPELL_CHARGE_RIFT                  = 47747,
    SPELL_CREATE_RIFT                  = 47743, // Don't work, using WA
    SPELL_ARCANE_ATTRACTION            = 57063,
	SPELL_CHAOTIC_ENERGY_BURST         = 47688,
    SPELL_CHARGED_CHAOTIC_ENERGY_BURST = 47737,
	SPELL_ARCANE_FORM_1                = 45832,
    SPELL_ARCANE_FORM_2                = 48019,
	SPELL_CHAOTIC_RIFT_AURA_5N         = 47687,
	SPELL_CHAOTIC_RIFT_AURA_5H         = 47733,
	SPELL_CHAOTIC_RIFT_SUMMON_AURA_5N  = 47732,
	SPELL_CHAOTIC_RIFT_SUMMON_AURA_5H  = 47742,
	SPELL_CHAOTIC_RIFT_AGGRO           = 63577,
};

const Position RiftLocation[6] =
{
    {652.64f, -273.70f, -8.75f, 0.0f},
    {634.45f, -265.94f, -8.44f, 0.0f},
    {620.73f, -281.17f, -9.02f, 0.0f},
    {626.10f, -304.67f, -9.44f, 0.0f},
    {639.87f, -314.11f, -9.49f, 0.0f},
    {651.72f, -297.44f, -9.37f, 0.0f},
};

class boss_anomalus : public CreatureScript
{
public:
    boss_anomalus() : CreatureScript("boss_anomalus") { }

    struct boss_anomalusAI : public QuantumBasicAI
    {
        boss_anomalusAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 SparkTimer;
        uint32 CreateRiftTimer;
		uint32 ArcaneAttractionTimer;
        uint32 HealthAmountModifier;

        uint64 ChaoticRiftGUID;

        SummonList Summons;

        bool DeadChaoticRift;

        void Reset()
        {
            SparkTimer = 0.5*IN_MILLISECONDS;
			ArcaneAttractionTimer = 2*IN_MILLISECONDS;

			DeadChaoticRift = false;

			Summons.DespawnAll();

            HealthAmountModifier = 1;
            ChaoticRiftGUID = 0;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			instance->SetData(DATA_ANOMALUS_EVENT, NOT_STARTED);
        }

		void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

			instance->SetData(DATA_ANOMALUS_EVENT, IN_PROGRESS);
        }

		void EnterEvadeMode()
		{
			Summons.DespawnAll();

			_EnterEvadeMode();

			WraithDespawner();

			me->GetMotionMaster()->MoveTargetedHome();

			Reset();
		}

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

            Summons.DespawnAll();

			WraithDespawner();

			if (IsHeroic() && !DeadChaoticRift)
				instance->DoCompleteAchievement(ACHIEVEMENT_CHAOS_THEORY);

			instance->SetData(DATA_ANOMALUS_EVENT, DONE);
        }

		void SummonedCreatureDespawn(Creature* summon)
		{
			Summons.Despawn(summon);
		}

		void JustSummoned(Creature* summon)
		{
			Summons.Summon(summon);
		}

		void WraithDespawner()
		{
			std::list<Creature*> CrazedManaWraithList;
			me->GetCreatureListWithEntryInGrid(CrazedManaWraithList, NPC_CRAZED_MANA_WRAITH, 350.0f);

			if (!CrazedManaWraithList.empty())
			{
				for (std::list<Creature*>::const_iterator itr = CrazedManaWraithList.begin(); itr != CrazedManaWraithList.end(); ++itr)
				{
					if (Creature* wraith = *itr)
						wraith->DespawnAfterAction();
				}
			}
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (me->HasAura(SPELL_RIFT_SHIELD))
            {
                if (ChaoticRiftGUID)
                {
                    Unit* Rift = Unit::GetUnit(*me, ChaoticRiftGUID);
                    if (Rift && Rift->IsDead())
                    {
                        me->RemoveAurasDueToSpell(SPELL_RIFT_SHIELD);
                        ChaoticRiftGUID = 0;
                    }
                    return;
                }
            }
			else ChaoticRiftGUID = 0;

            if (me->HealthBelowPct(100 - 25 * HealthAmountModifier))
            {
                ++HealthAmountModifier;
                DoSendQuantumText(SAY_SHIELD, me);
                DoCast(me, SPELL_RIFT_SHIELD);
                Creature* rift = me->SummonCreature(NPC_CHAOTIC_RIFT, RiftLocation[urand(0, 5)], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1*IN_MILLISECONDS);
                if (rift)
                {
                    me->AddAura(SPELL_CHARGE_RIFT, rift);

                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                        rift->AI()->AttackStart(target);

                    ChaoticRiftGUID = rift->GetGUID();
                    DoSendQuantumText(SAY_RIFT , me);
                }
            }

            if (SparkTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SPARK_5N, SPELL_SPARK_5H));
					SparkTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
            }
			else SparkTimer -= diff;

			if (ArcaneAttractionTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ARCANE_ATTRACTION);
					ArcaneAttractionTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
            }
			else ArcaneAttractionTimer -= diff;

			if (me->GetDistance(me->GetHomePosition()) > 60.0f)
            {
                EnterEvadeMode();
                return;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_anomalusAI(creature);
    }
};

class npc_chaotic_rift : public CreatureScript
{
public:
    npc_chaotic_rift() : CreatureScript("npc_chaotic_rift") { }

    struct npc_chaotic_riftAI : public QuantumBasicAI
    {
        npc_chaotic_riftAI(Creature* creature) : QuantumBasicAI(creature)
        {
			SetCombatMovement(false);
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        void Reset()
        {
			DoCast(me, SPELL_ARCANE_FORM_1, true);
			DoCast(me, SPELL_ARCANE_FORM_2, true);
			DoCast(me, DUNGEON_MODE(SPELL_CHAOTIC_RIFT_AURA_5N, SPELL_CHAOTIC_RIFT_AURA_5H), true);
			DoCast(me, DUNGEON_MODE(SPELL_CHAOTIC_RIFT_SUMMON_AURA_5N, SPELL_CHAOTIC_RIFT_SUMMON_AURA_5H), true);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_CHAOTIC_RIFT_AGGRO);
		}

        void JustDied(Unit* /*killer*/)
        {
            if (Creature* anomalus = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_ANOMALUS) : 0))
			{
                if (instance->GetData(DATA_ANOMALUS_EVENT) == IN_PROGRESS)
                    CAST_AI(boss_anomalus::boss_anomalusAI, anomalus->AI())->DeadChaoticRift = true;
			}
        }

        void UpdateAI(const uint32 /*diff*/){}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_chaotic_riftAI(creature);
    }
};

void AddSC_boss_anomalus()
{
    new boss_anomalus();
    new npc_chaotic_rift();
}