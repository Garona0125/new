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
#include "the_slave_pens.h"

enum Spells
{
	SPELL_POISON_BOLT_VOLLEY_5N = 34780,
	SPELL_POISON_BOLT_VOLLEY_5H = 39340,
	SPELL_CLEAVE                = 40504,
	SPELL_ACID_SPRAY            = 38153,
	SPELL_UPPERCUT              = 32055,
};

class boss_quagmirran : public CreatureScript
{
public:
	boss_quagmirran() : CreatureScript("boss_quagmirran") {}

    struct boss_quagmirranAI : public QuantumBasicAI
    {
        boss_quagmirranAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 PoisonBoltVolleyTimer;
		uint32 CleaveTimer;
		uint32 AcidSprayTimer;
		uint32 UppercutTimer;

		void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			PoisonBoltVolleyTimer = 2000;
			CleaveTimer = 3000;
			AcidSprayTimer = 4000;
			UppercutTimer = 6000;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PoisonBoltVolleyTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_POISON_BOLT_VOLLEY_5N, SPELL_POISON_BOLT_VOLLEY_5H));
				PoisonBoltVolleyTimer = urand(3000, 4000);
			}
			else PoisonBoltVolleyTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(5000, 6000);
			}
			else CleaveTimer -= diff;

			if (AcidSprayTimer <= diff)
			{
				DoCastAOE(SPELL_ACID_SPRAY);
				AcidSprayTimer = urand(7000, 8000);
			}
			else AcidSprayTimer -= diff;

			if (UppercutTimer <= diff)
			{
				DoCastVictim(SPELL_UPPERCUT);
				UppercutTimer = urand(9000, 10000);
			}
			else UppercutTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_quagmirranAI(creature);
    }
};

void AddSC_boss_quagmirran()
{
	new boss_quagmirran();
}