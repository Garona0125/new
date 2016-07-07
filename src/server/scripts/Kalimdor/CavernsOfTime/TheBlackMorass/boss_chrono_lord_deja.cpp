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
#include "the_black_morass.h"

enum Texts
{
	SAY_INTRO                 = -1269006,
	SAY_AGGRO                 = -1269007,
	SAY_DESPAWN_MEDIVH        = -1269008,
	SAY_SLAY_1                = -1269009,
	SAY_SLAY_2                = -1269010,
	SAY_DEATH                 = -1269011,
};

enum Spells
{
	SPELL_ARCANE_BLAST_5N     = 31457,
	SPELL_ARCANE_BLAST_5H     = 38538,
	SPELL_ARCANE_DISCHARGE_5N = 31472,
	SPELL_ARCANE_DISCHARGE_5H = 38539,
	SPELL_TIME_LAPSE          = 31467,
	SPELL_ATTRACTION          = 38540,
};

class boss_chrono_lord_deja : public CreatureScript
{
public:
    boss_chrono_lord_deja() : CreatureScript("boss_chrono_lord_deja") {}

    struct boss_chrono_lord_dejaAI : public QuantumBasicAI
    {
        boss_chrono_lord_dejaAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }
    
        InstanceScript* instance;

		bool Intro;
        uint32 ArcaneBlastTimer;
		uint32 ArcaneDiscargeTimer;
        uint32 TimeLapseTimer;
		uint32 AttractionTimer;

        void Reset()
        {
            ArcaneBlastTimer = 2000;
			ArcaneDiscargeTimer = 4000;
            TimeLapseTimer = 6000;
			AttractionTimer = 8000;

			Intro = true;

            if (instance)
                if (IsHeroic())
					instance->SetData(DATA_CHRONO_LORD_DEJA_DEATH, 0);
        }

		void MoveInLineOfSight(Unit* who)
        {
			if (me->IsWithinDistInMap(who, 1500.0f))
			{
				if (who->GetTypeId() == TYPE_ID_PLAYER)
				{
					if (Intro)
					{
						DoSendQuantumText(SAY_INTRO, me);
						Intro = false;
					}
				}
			}
		}

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);
        }

        void KilledUnit(Unit* /*who*/)
        {
			DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

            if (instance)
                instance->SetData(DATA_CHRONO_LORD_DEJA_DEATH, 1);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ArcaneBlastTimer < diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_ARCANE_BLAST_5N, SPELL_ARCANE_BLAST_5H));
					ArcaneBlastTimer = urand(4000, 5000);
				}
            }
			else ArcaneBlastTimer -= diff;

            if (ArcaneDiscargeTimer < diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_ARCANE_DISCHARGE_5N, SPELL_ARCANE_DISCHARGE_5H));
					ArcaneDiscargeTimer = urand(7000, 8000);
				}
			}
			else ArcaneDiscargeTimer -= diff;

			if (TimeLapseTimer < diff)
            {
				DoCastAOE(SPELL_TIME_LAPSE);
				TimeLapseTimer = urand(9000, 10000);
            }
			else TimeLapseTimer -= diff;

			if (AttractionTimer < diff && IsHeroic())
            {
				DoCastAOE(SPELL_ATTRACTION);
				AttractionTimer = urand(11000, 12000);
            }
			else AttractionTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_chrono_lord_dejaAI(creature);
    }
};

void AddSC_boss_chrono_lord_deja()
{
    new boss_chrono_lord_deja();
}