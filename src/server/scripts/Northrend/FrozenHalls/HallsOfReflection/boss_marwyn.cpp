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
#include "halls_of_reflection.h"

enum Says
{
    SAY_INTRO              = -1594506,
    SAY_AGGRO              = -1668060,
    SAY_DEATH              = -1668063,
    SAY_SLAY_1             = -1668061,
    SAY_SLAY_2             = -1668062,
    SAY_CORRUPTED_FLESH    = -1668064,
    SAY_WELL_OF_CORRUPTION = -1668065,
};

enum Spells
{
    SPELL_OBLITERATE_5N          = 72360,
    SPELL_OBLITERATE_5H          = 72434,
    SPELL_SHARED_SUFFERING_5N    = 72368,
    SPELL_SHARED_SUFFERING_5H    = 72369,
	SPELL_CORRUPTED_FLESH_5N     = 72363,
    SPELL_CORRUPTED_FLESH_5H     = 72436,
    SPELL_WELL_OF_CORRUPTION     = 72362,
    SPELL_BERSERK                = 47008,
	SPELL_WELL_OF_SOULS          = 72630,
	SPELL_TIKI_WARRIOR_VISUAL    = 75041,
	SPELL_SPIRIT_ACTIVATE_VISUAL = 72130,
};

class boss_marwyn : public CreatureScript
{
public:
    boss_marwyn() : CreatureScript("boss_marwyn") { }

    struct boss_marwynAI : public QuantumBasicAI
    {
		boss_marwynAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();
			Reset();
		}

		InstanceScript* instance;

		bool IsCall;

		uint32 BerserkTimer;
		uint32 SharedSufferingTimer;
		uint32 WellOfCorruptionTimer;
		uint32 CorruptedFleshTimer;
		uint32 ObliterateTimer;
		uint32 SummonTimer;
		uint32 LocNo;
		uint64 SummonGUID[16];
		uint32 CheckSummon;
		uint8 SummonCount;
		uint32 WaveCount;
		uint32 Summon;

		void Reset()
		{
			BerserkTimer = 180000;
			SharedSufferingTimer = 4000;
			WellOfCorruptionTimer = 12000;
			CorruptedFleshTimer = 21000;
			ObliterateTimer = 5000;
			SummonTimer = 15000;
			SummonCount = 0;
			WaveCount = 0;
			IsCall = false;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			me->SetVisible(false);
			DespawnAfterWipe();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

			if (instance->GetData(DATA_MARWYN_EVENT) == IN_PROGRESS)
				instance->SetData(DATA_MARWYN_EVENT, NOT_STARTED);
		}

		void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);
            instance->DoUpdateWorldState(WORLD_STATE_HOR_WAVE_COUNT, 10);
        }

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
		}

		void DespawnAfterWipe()
		{
			std::list<Creature*> TrashList;
			me->GetCreatureListWithEntryInGrid(TrashList, NPC_PHANTOM_MAGE, 500.0f);
			me->GetCreatureListWithEntryInGrid(TrashList, NPC_SPECTRAL_FOOTMAN, 500.0f);
			me->GetCreatureListWithEntryInGrid(TrashList, NPC_TORTURED_RIFLEMAN, 500.0f);
			me->GetCreatureListWithEntryInGrid(TrashList, NPC_SHADOWY_MERCENARY, 500.0f);
			me->GetCreatureListWithEntryInGrid(TrashList, NPC_PHANTOM_HALLUCINATION, 500.0f);
			me->GetCreatureListWithEntryInGrid(TrashList, NPC_GHOSTLY_PRIEST, 500.0f);

			if (!TrashList.empty())
			{
				for (std::list<Creature*>::const_iterator itr = TrashList.begin(); itr != TrashList.end(); ++itr)
				{
					if (Creature* trash = *itr)
						trash->DespawnAfterAction();
				}
			}
		}

		void JustDied(Unit* killer)
        {
			instance->SetData(DATA_MARWYN_EVENT, DONE);
			instance->SetData(TYPE_PHASE, 3);

			if (killer->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(SAY_DEATH, me);
		}

        void AttackStart(Unit* who)
        {
			if (instance->GetData(DATA_MARWYN_EVENT) != IN_PROGRESS)
				return;

             QuantumBasicAI::AttackStart(who);
        }

		void summon()
        {
			LocNo = 14;

			for (uint8 i = 0; i < 14; i++)
			{
				switch(urand(0, 3))
                {
                   case 0:
                       switch(urand(1, 3))
                       {
                         case 1:
							 Summon = NPC_SHADOWY_MERCENARY;
							 break;
                         case 2:
							 Summon = NPC_SPECTRAL_FOOTMAN;
							 break;
                         case 3:
							 Summon = NPC_GHOSTLY_PRIEST;
							 break;
					   }
                       break;
                   case 1:
                       switch(urand(1, 3))
                       {
                         case 1:
							 Summon = NPC_TORTURED_RIFLEMAN;
							 break;
                         case 2:
							 Summon = NPC_SPECTRAL_FOOTMAN;
							 break;
                         case 3:
							 Summon = NPC_PHANTOM_MAGE;
							 break;
                       }
                       break;
                   case 2:
                       switch(urand(1, 3))
                       {
                         case 1:
							 Summon = NPC_TORTURED_RIFLEMAN;
							 break;
                         case 2:
							 Summon = NPC_PHANTOM_HALLUCINATION;
							 break;
                         case 3:
							 Summon = NPC_GHOSTLY_PRIEST;
							 break;
                       }
                       break;
                   case 3:
                       switch(urand(1, 3))
                       {
                         case 1:
							 Summon = NPC_SHADOWY_MERCENARY;
							 break;
                         case 2:
							 Summon = NPC_PHANTOM_HALLUCINATION;
							 break;
                         case 3:
							 Summon = NPC_PHANTOM_MAGE;
							 break;
                       }
					   break;
				}

				CheckSummon = 0;

				if (Creature* summon = me->SummonCreature(Summon, SpawnLoc[LocNo].x, SpawnLoc[LocNo].y, SpawnLoc[LocNo].z, SpawnLoc[LocNo].o, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000))
				{
					SummonGUID[i] = summon->GetGUID();
					summon->CastSpell(summon, SPELL_WELL_OF_SOULS, true);
					summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
					summon->SetCurrentFaction(2212);
					summon->SetReactState(REACT_PASSIVE);
				}
				LocNo++;
			}
		}

        void CallFallSoldier()
        {
			for (uint8 i = 0; i < 4; i++)
			{
				if (Creature* Summon = instance->instance->GetCreature(SummonGUID[CheckSummon]))
				{
					Summon->CastSpell(Summon, SPELL_TIKI_WARRIOR_VISUAL, true);
					Summon->CastSpell(Summon, SPELL_SPIRIT_ACTIVATE_VISUAL, true);
					Summon->SetCurrentFaction(2212);
					Summon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
					Summon->SetReactState(REACT_AGGRESSIVE);
					Summon->SetInCombatWithZone();
				}
				CheckSummon++;
			}
			WaveCount++;
			instance->DoUpdateWorldState(WORLD_STATE_HOR_WAVE_COUNT, 5 + WaveCount);
		}
		
		void UpdateAI(const uint32 diff)
		{
            if (instance->GetData(DATA_FALRIC_EVENT) == SPECIAL)
            {
                if (!IsCall)
                {
                   IsCall = true;
                   summon();
                }
            }

			if (instance->GetData(DATA_MARWYN_EVENT) == SPECIAL)
            {
				if (SummonTimer <= diff)
				{
					++SummonCount;
					if (SummonCount == 1)
						DoSendQuantumText(SAY_INTRO, me);

					if (SummonCount > 4)
					{
						instance->SetData(DATA_MARWYN_EVENT, IN_PROGRESS);
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
						me->SetInCombatWithZone();
					}
					else CallFallSoldier();

					SummonTimer = 60000;
				}
				else SummonTimer -= diff;
			}

            if (!UpdateVictim())
                return;

            if (ObliterateTimer <= diff)
            {
				DoCastVictim(DUNGEON_MODE(SPELL_OBLITERATE_5N, SPELL_OBLITERATE_5H));
                ObliterateTimer = urand(8000, 12000);
            }
			else ObliterateTimer -= diff;

            if (WellOfCorruptionTimer <= diff)
            {
                DoSendQuantumText(SAY_WELL_OF_CORRUPTION, me);

                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                   DoCast(target, SPELL_WELL_OF_CORRUPTION);
				   WellOfCorruptionTimer = urand(25000, 30000);
				}
            }
			else WellOfCorruptionTimer -= diff;

            if (SharedSufferingTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                   DoCast(target, DUNGEON_MODE(SPELL_SHARED_SUFFERING_5N, SPELL_SHARED_SUFFERING_5H));
				   SharedSufferingTimer = urand(15000, 20000);
				}
            }
			else SharedSufferingTimer -= diff;

            if (CorruptedFleshTimer <= diff)
            {
                DoSendQuantumText(SAY_CORRUPTED_FLESH, me);

                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_CORRUPTED_FLESH_5N, SPELL_CORRUPTED_FLESH_5H));
					CorruptedFleshTimer = urand(10000, 16000);
				}
            }
			else CorruptedFleshTimer -= diff;

            if (BerserkTimer <= diff)
            {
                DoCast(me, SPELL_BERSERK);
                BerserkTimer = 180000;
            }
			else BerserkTimer -= diff;

			DoMeleeAttackIfReady();
            return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_marwynAI(creature);
    }
};

void AddSC_boss_marwyn()
{
	new boss_marwyn();
}