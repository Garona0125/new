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
    SAY_AGGRO          = -1576020,
	SAY_SLAY           = -1576021,
	SAY_REFLECT        = -1576022,
	SAY_CRYSTAL_SPIKES = -1576023,
	SAY_DEATH          = -1576024,
};

enum Spells
{
    SPELL_SPELL_REFLECTION           = 47981,
    SPELL_TRAMPLE_5N                 = 48016,
	SPELL_TRAMPLE_5H                 = 57066,
    SPELL_FRENZY_5N                  = 48017,
	SPELL_FRENZY_5H                  = 57086,
    SPELL_SUMMON_CRYSTALLINE_TANGLER = 61564,
	SPELL_CRYSTAL_SPIKE_OBJECT       = 47936,
	SPELL_CRYSTAL_SPIKE_TRIGGER      = 47954,
	SPELL_CRYSTAL_SPIKE_DAMAGE_5N    = 47944,
	SPELL_CRYSTAL_SPIKE_DAMAGE_5H    = 57067,
};

enum CrystalSpikes
{
    DATA_COUNT = 1,
    MAX_COUNT  = 5,
};

uint32 const CrystalSpikeSummon[3] =
{
	47936,
	47942,
	47943,
};

class OrmorokTanglerPredicate
{
public:
	OrmorokTanglerPredicate(Unit* unit) : me(unit) {}

    bool operator() (WorldObject* object) const
    {
		return object->GetDistance2d(me) >= 5.0f;
    }

    private:
        Unit* me;
};

class boss_ormorok : public CreatureScript
{
public:
    boss_ormorok() : CreatureScript("boss_ormorok") { }

    struct boss_ormorokAI : public BossAI
    {
		boss_ormorokAI(Creature* creature) : BossAI(creature, DATA_ORMOROK_EVENT), Summons(me) {}

		uint32 CrystalSpikeTimer;
		uint32 TrampleTimer;
		uint32 SpellReflectionTimer;
		uint32 CrystalineTanglerTimer;

		SummonList Summons;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			Summons.DespawnAll();
		}

        void EnterToBattle(Unit* /*who*/)
        {
            _EnterToBattle();

			CrystalSpikeTimer = 500;
			TrampleTimer = 2000;
			SpellReflectionTimer = 4000;
			CrystalineTanglerTimer = 6000;

            DoSendQuantumText(SAY_AGGRO, me);

			instance->SetData(DATA_ORMOROK_EVENT, IN_PROGRESS);
        }

		void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(SAY_SLAY, me);
        }

        void JustDied(Unit* /*killer*/)
        {
            _JustDied();

            DoSendQuantumText(SAY_DEATH, me);

			Summons.DespawnAll();

			instance->SetData(DATA_ORMOROK_EVENT, DONE);
        }

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_CRYSTALINE_TANGLER)
				Summons.Summon(summon);
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

			if (CrystalSpikeTimer <= diff)
			{
				DoSendQuantumText(SAY_CRYSTAL_SPIKES, me);

				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CRYSTAL_SPIKE_OBJECT, true);
					DoCastAOE(DUNGEON_MODE(SPELL_CRYSTAL_SPIKE_DAMAGE_5N, SPELL_CRYSTAL_SPIKE_DAMAGE_5H));
					CrystalSpikeTimer = 5000;
				}
			}
			else CrystalSpikeTimer -= diff;

			if (TrampleTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_TRAMPLE_5N, SPELL_TRAMPLE_5H));
				TrampleTimer = 7000;
			}
			else TrampleTimer -= diff;

			if (SpellReflectionTimer <= diff)
			{
				DoSendQuantumText(SAY_REFLECT, me);

				DoCast(me, SPELL_SPELL_REFLECTION);
				SpellReflectionTimer = 10000;
			}
			else SpellReflectionTimer -= diff;

			if (CrystalineTanglerTimer <= diff && IsHeroic())
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0, OrmorokTanglerPredicate(me)))
				{
					DoCast(target, SPELL_SUMMON_CRYSTALLINE_TANGLER, true);
					DoCast(target, SPELL_SUMMON_CRYSTALLINE_TANGLER, true);
					DoCast(target, SPELL_SUMMON_CRYSTALLINE_TANGLER, true);
					CrystalineTanglerTimer = 12000;
				}
			}
			else CrystalineTanglerTimer -= diff;

			if (HealthBelowPct(25))
			{
				if (!me->HasAura(DUNGEON_MODE(SPELL_FRENZY_5N, SPELL_FRENZY_5H)))
				{
					DoCast(me, DUNGEON_MODE(SPELL_FRENZY_5N, SPELL_FRENZY_5H));
					DoSendQuantumText(SAY_GENERIC_EMOTE_FRENZY, me);
				}
			}

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_ormorokAI (creature);
    }
};

class npc_crystal_spike_trigger : public CreatureScript
{
public:
    npc_crystal_spike_trigger() : CreatureScript("npc_crystal_spike_trigger") { }

    struct npc_crystal_spike_triggerAI : public QuantumBasicAI
    {
        npc_crystal_spike_triggerAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 _count;
        uint32 DespawnTimer;

		void Reset(){}

        void IsSummonedBy(Unit* owner)
        {
            switch (me->GetEntry())
            {
                case NPC_CRYSTAL_SPIKE_INITIAL:
                     _count = 0;
                     me->SetFacingToObject(owner);
                     break;
                case NPC_CRYSTAL_SPIKE_TRIGGER:
                    if (Creature* trigger = owner->ToCreature())
                        _count = trigger->AI()->GetData(DATA_COUNT) + 1;
                    break;
                default:
                    _count = MAX_COUNT;
                    break;
            }

            if (me->GetEntry() == NPC_CRYSTAL_SPIKE_TRIGGER)
			{
                if (GameObject* trap = me->FindGameobjectWithDistance(GO_CRYSTAL_SPIKE_TRAP, 2.0f))
                    trap->Use(me);
			}

            DespawnTimer = 2000;
        }

        uint32 GetData(uint32 type)
        {
            return type == DATA_COUNT ? _count : 0;
        }

        void UpdateAI(const uint32 diff)
        {
            if (DespawnTimer <= diff)
            {
                if (me->GetEntry() == NPC_CRYSTAL_SPIKE_TRIGGER)
                    if (GameObject* trap = me->FindGameobjectWithDistance(GO_CRYSTAL_SPIKE_TRAP, 2.0f))
                        trap->Delete();

                me->DespawnAfterAction();
            }
            else DespawnTimer -= diff;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crystal_spike_triggerAI(creature);
    }
};

void AddSC_boss_ormorok()
{
    new boss_ormorok();
    new npc_crystal_spike_trigger();
}