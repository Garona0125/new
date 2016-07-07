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

enum Spells
{
	SPELL_SHADOW_VOLLEY      = 21341,
	SPELL_CLEAVE             = 20677,
	SPELL_THUNDERCLAP        = 23931,
	SPELL_TWISTED_REFLECTION = 21063,
	SPELL_VOIDBOLT           = 21066,
	SPELL_RAGE               = 21340,
	SPELL_CAPTURE_SOUL       = 21054,
};

class boss_kruul : public CreatureScript
{
public:
    boss_kruul() : CreatureScript("boss_kruul") { }

    struct boss_kruulAI : public QuantumBasicAI
    {
        boss_kruulAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ShadowVolleyTimer;
        uint32 CleaveTimer;
        uint32 ThunderClapTimer;
        uint32 TwistedReflectionTimer;
        uint32 VoidBoltTimer;
        uint32 RageTimer;
        uint32 HoundTimer;

        void Reset()
        {
            ShadowVolleyTimer = 10000;
            CleaveTimer = 14000;
            ThunderClapTimer = 20000;
            TwistedReflectionTimer = 25000;
            VoidBoltTimer = 30000;
            RageTimer = 60000;                                 //Cast rage after 1 minute
            HoundTimer = 8000;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void KilledUnit(Unit* /*victim*/)
        {
            DoCast(me, SPELL_CAPTURE_SOUL);
        }

        void SummonHounds(Unit* victim)
        {
            if (Creature* Hound = DoSpawnCreature(19207, float(irand(-9, 9)), float(irand(-9, 9)), 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 300000))
                Hound->AI()->AttackStart(victim);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (ShadowVolleyTimer <= diff)
            {
                if (urand(0, 99) < 45)
                    DoCastVictim(SPELL_SHADOW_VOLLEY);

                ShadowVolleyTimer = 5000;
            }
			else ShadowVolleyTimer -= diff;

            if (CleaveTimer <= diff)
            {
                if (urand(0, 1))
                    DoCastVictim(SPELL_CLEAVE);

                CleaveTimer = 10000;
            }
			else CleaveTimer -= diff;

            if (ThunderClapTimer <= diff)
            {
                if (urand(0, 9) < 2)
                    DoCastVictim(SPELL_THUNDERCLAP);

                ThunderClapTimer = 12000;
            }
			else ThunderClapTimer -= diff;

            if (TwistedReflectionTimer <= diff)
            {
                DoCastVictim(SPELL_TWISTED_REFLECTION);
                TwistedReflectionTimer = 30000;
            }
			else TwistedReflectionTimer -= diff;

            if (VoidBoltTimer <= diff)
            {
                if (urand(0, 9) < 4)
                    DoCastVictim(SPELL_VOIDBOLT);

                VoidBoltTimer = 18000;
            }
			else VoidBoltTimer -= diff;

            if (RageTimer <= diff)
            {
                DoCast(me, SPELL_RAGE);
                RageTimer = 70000;
            }
			else RageTimer -= diff;

            if (HoundTimer <= diff)
            {
                SummonHounds(me->GetVictim());
                SummonHounds(me->GetVictim());
                SummonHounds(me->GetVictim());

                HoundTimer = 45000;
            } 
			else HoundTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_kruulAI(creature);
    }
};

void AddSC_boss_kruul()
{
    new boss_kruul();
}