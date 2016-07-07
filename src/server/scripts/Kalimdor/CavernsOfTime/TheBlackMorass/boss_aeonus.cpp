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
	SAY_INTRO            = -1269012,
	SAY_AGGRO            = -1269013,
	SAY_DESPAWN_MEDIVH   = -1269014,
	SAY_SLAY_1           = -1269015,
	SAY_SLAY_2           = -1269016,
	SAY_DEATH            = -1269017,
	SAY_FRENZY           = -1269018,
};

enum Spells
{
	SPELL_CLEAVE         = 40504,
	SPELL_TIME_STOP      = 31422,
	SPELL_ENRAGE         = 37605,
	SPELL_SAND_BREATH_5N = 31473,
	SPELL_SAND_BREATH_5H = 39049,
};

class boss_aeonus : public CreatureScript
{
public:
    boss_aeonus() : CreatureScript("boss_aeonus") {}

    struct boss_aeonusAI : public QuantumBasicAI
    {
        boss_aeonusAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        InstanceScript* instance;

		bool Intro;
		uint32 CleaveTimer;
        uint32 SandBreathTimer;
        uint32 TimeStopTimer;
        uint32 EnrageTimer;

        void Reset()
        {
			CleaveTimer = 2000;
            SandBreathTimer = 4000;
            TimeStopTimer = 6000;
            EnrageTimer = 8000;

			Intro = true;

            if (instance)
                instance->SetData(DATA_AEONUS_DEATH, 0); 
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
                instance->SetData(DATA_AEONUS_DEATH, 1);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer < diff)
            {
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(3000, 4000);
            }
			else CleaveTimer -= diff;

            if (SandBreathTimer < diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_SAND_BREATH_5N, SPELL_SAND_BREATH_5H));
					SandBreathTimer = urand(5000, 6000);
				}
            }
			else SandBreathTimer -= diff;

            if (TimeStopTimer < diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_TIME_STOP);
					TimeStopTimer = urand(7000, 8000);
				}
            }
			else TimeStopTimer -= diff;

            if (EnrageTimer < diff)
            {
                DoCast(me, SPELL_ENRAGE);
                EnrageTimer = urand(10000, 11000);
            }
			else EnrageTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_aeonusAI(creature);
    }
};

void AddSC_boss_aeonus()
{
    new boss_aeonus();
}