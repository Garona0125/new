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
#include "the_underbog.h"

enum Spells
{
	SPELL_ENRAGE      = 15716,
	SPELL_ACID_BREATH = 34268,
	SPELL_ACID_SPIT   = 34290,
	SPELL_TAIL_SWEEP  = 38737,
};

class boss_ghazan : public CreatureScript
{
public:
    boss_ghazan() : CreatureScript("boss_ghazan") {}

    struct boss_ghazanAI : public QuantumBasicAI
    {
        boss_ghazanAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 AcidBreathTimer;
		uint32 AcidSpitTimer;
		uint32 TailSweepTimer;

        void Reset()
        {
			AcidBreathTimer = 500;
			AcidSpitTimer = 2000;
            TailSweepTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (AcidBreathTimer < diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM))
				{
					DoCast(target, SPELL_ACID_BREATH);
					AcidBreathTimer = urand(3000, 4000);
				}
            }
			else AcidBreathTimer -= diff;

            if (AcidSpitTimer < diff)
            {
                DoCastAOE(SPELL_ACID_SPIT);
                AcidSpitTimer = urand(5000, 6000);
            }
			else AcidSpitTimer -= diff;

            if (TailSweepTimer < diff && IsHeroic())
            {
                DoCast(me, SPELL_TAIL_SWEEP);
                TailSweepTimer = urand(7000, 8000);
            }
			else TailSweepTimer -= diff;

			if (HealthBelowPct(50))
            {
				if (!me->HasAuraEffect(SPELL_ENRAGE, 0))
				{
					DoCast(me, SPELL_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
            }

			if (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 35)
			{
				EnterEvadeMode();
				return;
			}

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_ghazanAI(creature);
    }
};

void AddSC_boss_ghazan()
{
	new boss_ghazan();
}