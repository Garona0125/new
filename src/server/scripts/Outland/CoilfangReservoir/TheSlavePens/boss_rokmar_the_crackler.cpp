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
	SPELL_ENSNARING_MOSS    = 31948,
	SPELL_FRENZY            = 34970,
	SPELL_GRIEVOUS_WOUND_5N = 31956,
	SPELL_GRIEVOUS_WOUND_5H = 38801,
	SPELL_WATER_SPIT        = 35008,
};

class boss_rokmar_the_crackler : public CreatureScript
{
public:
	boss_rokmar_the_crackler() : CreatureScript("boss_rokmar_the_crackler") {}

    struct boss_rokmar_the_cracklerAI : public QuantumBasicAI
    {
        boss_rokmar_the_cracklerAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 EnsnaringMossTimer;
		uint32 GrievousWoundTimer;
		uint32 WaterSpitTimer;

		void Reset()
        {
			EnsnaringMossTimer = 2000;
			GrievousWoundTimer = 3000;
			WaterSpitTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (EnsnaringMossTimer <= diff)
			{
				DoCastVictim(SPELL_ENSNARING_MOSS);
				EnsnaringMossTimer = urand(3000, 4000);
			}
			else EnsnaringMossTimer -= diff;

			if (GrievousWoundTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_GRIEVOUS_WOUND_5N, SPELL_GRIEVOUS_WOUND_5H));
				GrievousWoundTimer = urand(5000, 6000);
			}
			else GrievousWoundTimer -= diff;

			if (WaterSpitTimer <= diff)
			{
				DoCastVictim(SPELL_WATER_SPIT);
				WaterSpitTimer = urand(7000, 8000);
			}
			else WaterSpitTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_FRENZY, 0))
				{
					DoCast(me, SPELL_FRENZY);
					DoSendQuantumText(SAY_GENERIC_EMOTE_FRENZY, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_rokmar_the_cracklerAI(creature);
    }
};

void AddSC_boss_rokmar_the_crackler()
{
	new boss_rokmar_the_crackler();
}