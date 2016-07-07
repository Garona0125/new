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
	SAY_AGGRO         = -1668050,
    SAY_DEATH         = -1668053,
    SAY_SLAY_1        = -1668051,
    SAY_SLAY_2        = -1668052,
    SAY_DESPAIR       = -1668054,
    SAY_DEFILING      = -1668055,
};

enum Spells
{
    SPELL_HOPELESSNESS           = 72395,
    SPELL_IMPENDING_DESPAIR      = 72426,
    SPELL_DEFILING_HORROR_5N     = 72435,
    SPELL_DEFILING_HORROR_5H     = 72452,
    SPELL_QUIVERING_STRIKE_5N    = 72422,
    SPELL_QUIVERING_STRIKE_5H    = 72453,
    SPELL_BERSERK                = 47008,
	SPELL_WELL_OF_SOULS          = 72630,
	SPELL_TIKI_WARRIOR_VISUAL    = 75041,
	SPELL_SPIRIT_ACTIVATE_VISUAL = 72130,
};

class boss_falric : public CreatureScript
{
public:
    boss_falric() : CreatureScript("boss_falric") { }

    struct boss_falricAI : public QuantumBasicAI
    {
		boss_falricAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();
			Reset();
		}

		InstanceScript* instance;

		bool IsCall;

		uint32 BerserkTimer;
		uint32 DefilingHorrorTimer;
		uint32 ImpendingDespairTimer;
		uint32 QuiveringStrikeTimer;
		uint32 SummonTimer;
		uint32 LocNo;
		uint64 SummonGUID[16];
		uint32 CheckSummon;
		uint32 WaveCount;

		uint8 SummonCount;
		uint32 Summon;

		void Reset()
		{
			BerserkTimer = 180000;
			DefilingHorrorTimer = 20000;
			ImpendingDespairTimer = urand(14000, 20000);
			QuiveringStrikeTimer = 2000;
			SummonTimer = 11000;
			SummonCount = 0;
			WaveCount = 0;
			IsCall = false;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			me->SetVisible(false);
			DespawnAfterWipe();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

			if (instance->GetData(DATA_FALRIC_EVENT) == IN_PROGRESS)
				instance->SetData(DATA_FALRIC_EVENT, NOT_STARTED);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoSendQuantumText(SAY_AGGRO, me);

			DoCast(me, SPELL_HOPELESSNESS);

			instance->DoUpdateWorldState(WORLD_STATE_HOR_WAVE_COUNT, 5);
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
			instance->SetData(DATA_MARWYN_EVENT, SPECIAL);
			instance->SetData(DATA_FALRIC_EVENT, DONE);

			if (killer->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(SAY_DEATH, me);
		}

		void AttackStart(Unit* who)
		{ 
			if (instance->GetData(DATA_FALRIC_EVENT) != IN_PROGRESS)
				return; 

			QuantumBasicAI::AttackStart(who);
		}

		void summon()
		{
			LocNo = 0;

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
			instance->DoUpdateWorldState(WORLD_STATE_HOR_WAVE_COUNT, WaveCount);
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

                if (SummonTimer <= diff) 
                {
					++SummonCount;

					if (SummonCount > 4) 
					{
						instance->SetData(DATA_FALRIC_EVENT, IN_PROGRESS);
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

            if (QuiveringStrikeTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_QUIVERING_STRIKE_5N, SPELL_QUIVERING_STRIKE_5H));
                QuiveringStrikeTimer = urand(7000, 14000);
            }
            else QuiveringStrikeTimer -= diff;

            if (ImpendingDespairTimer <= diff)
            {
                DoSendQuantumText(SAY_DESPAIR, me);

                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_IMPENDING_DESPAIR);
					ImpendingDespairTimer = urand(15000, 25000);
				}
            }
            else ImpendingDespairTimer -= diff;

            if (DefilingHorrorTimer <= diff)
            {
                DoSendQuantumText(SAY_DEFILING, me);
                DoCastVictim(DUNGEON_MODE(SPELL_DEFILING_HORROR_5N, SPELL_DEFILING_HORROR_5H));
                DefilingHorrorTimer = urand(25000, 30000);
            }
            else DefilingHorrorTimer -= diff;

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
        return new boss_falricAI(creature);
    }
};

void AddSC_boss_falric()
{
	new boss_falric();
}