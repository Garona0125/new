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
#include "shattered_halls.h"

enum Says
{
	SAY_AGGRO_1 = -1465200,
	SAY_AGGRO_2 = -1465201,
	SAY_AGGRO_3 = -1465202,
	SAY_AGGRO_4 = -1465203,
};

enum Spells
{
	SPELL_CLEAVE = 15496,
};

class boss_blood_guard_porung : public CreatureScript
{
public:
	boss_blood_guard_porung() : CreatureScript("boss_blood_guard_porung") { }

    struct boss_blood_guard_porungAI : public QuantumBasicAI
    {
        boss_blood_guard_porungAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		InstanceScript* instance;

		uint32 CleaveTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			CleaveTimer = 2000;
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3, SAY_AGGRO_4), me);
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = 4000;
			}
			else CleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_blood_guard_porungAI(creature);
    }
};

void AddSC_boss_blood_guard_porung()
{
	new boss_blood_guard_porung();
}