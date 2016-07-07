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
#include "steam_vault.h"

enum Texts
{
	SAY_SUMMON  = -1545000,
	SAY_AGGRO_1 = -1545001,
	SAY_AGGRO_2 = -1545002,
	SAY_AGGRO_3 = -1545003,
	SAY_SLAY_1  = -1545004,
	SAY_SLAY_2  = -1545005,
	SAY_DEAD    = -1545006,
};

enum Spells
{
	SPELL_LIGHTNING_CLOUD  = 25033,
	SPELL_LUNG_BURST       = 31481,
	SPELL_ENVELOPING_WINDS = 31718,
};

const Position ElementalSpawnPosition[2] =
{
	{87.5848f, -321.879f, -7.87089f, 3.1765f},
	{88.4884f, -309.902f, -7.87089f, 3.20713f},
};

class boss_hydromancer_thespia : public CreatureScript
{
public:
    boss_hydromancer_thespia() : CreatureScript("boss_hydromancer_thespia") { }

    struct boss_thespiaAI : public QuantumBasicAI
    {
        boss_thespiaAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
		SummonList Summons;

        uint32 LightningCloudTimer;
        uint32 LungBurstTimer;
        uint32 EnvelopingWindsTimer;

        void Reset()
        {
            LightningCloudTimer = 500;
            LungBurstTimer = 4000;
            EnvelopingWindsTimer = 6000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            if (instance)
                instance->SetData(TYPE_HYDROMANCER_THESPIA, NOT_STARTED);

			for (uint8 i = 0; i < 2; i++)
			{
				DoSendQuantumText(SAY_SUMMON, me);
				me->SummonCreature(NPC_WATER_ELEMENTAL, ElementalSpawnPosition[i], TEMPSUMMON_DEAD_DESPAWN, 30000);
			}
        }

		void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);

			Summons.DoZoneInCombat();

            if (instance)
				instance->SetData(TYPE_HYDROMANCER_THESPIA, IN_PROGRESS);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

		void JustReachedHome()
		{
			Summons.DespawnAll();
			Reset();
		}

		void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEAD, me);

			Summons.DespawnAll();

            if (instance)
                instance->SetData(TYPE_HYDROMANCER_THESPIA, DONE);
        }

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_WATER_ELEMENTAL)
				Summons.Summon(summon);
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (LightningCloudTimer <= diff && IsHeroic())
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_LIGHTNING_CLOUD);
					LightningCloudTimer = urand(3000, 4000);
				}
            }
			else LightningCloudTimer -=diff;

            if (LungBurstTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_LUNG_BURST, true);
					LungBurstTimer = urand(6000, 7000);
				}
            }
			else LungBurstTimer -=diff;

            if (EnvelopingWindsTimer <= diff && IsHeroic())
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_ENVELOPING_WINDS, true);
					EnvelopingWindsTimer = urand(8000, 9000);
				}
            }
			else EnvelopingWindsTimer -=diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_thespiaAI(creature);
    }
};

void AddSC_boss_hydromancer_thespia()
{
    new boss_hydromancer_thespia();
}