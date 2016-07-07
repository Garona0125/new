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
#include "temple_of_ahnqiraj.h"

enum Spells
{
	SPELL_FRENZY          = 26051,
	SPELL_BERSERK         = 26068,
	SPELL_POISON_BOLT     = 26052,
	SPELL_NOXIOUS_POISON  = 26053,
	SPELL_WYVERN_STING    = 26180,
	SPELL_ACID_SPIT       = 26050,
};

class boss_princess_huhuran : public CreatureScript
{
public:
    boss_princess_huhuran() : CreatureScript("boss_princess_huhuran") { }

    struct boss_princess_huhuranAI : public QuantumBasicAI
    {
        boss_princess_huhuranAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 FrenzyTimer;
        uint32 WyvernTimer;
        uint32 SpitTimer;
        uint32 PoisonBoltTimer;
        uint32 NoxiousPoisonTimer;
        uint32 FrenzyBackTimer;

        bool Frenzy;
        bool Berserk;

        void Reset()
        {
            FrenzyTimer = urand(25000, 35000);
            WyvernTimer = urand(18000, 28000);
            SpitTimer = 8000;
            PoisonBoltTimer = 4000;
            NoxiousPoisonTimer = urand(10000, 20000);
            FrenzyBackTimer = 15000;

            Frenzy = false;
            Berserk = false;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
        }

        void EnterToBattle(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (FrenzyTimer <= diff && !Frenzy)
            {
                DoCast(me, SPELL_FRENZY);
                DoSendQuantumText(EMOTE_GENERIC_FRENZY_KILL, me);
                Frenzy = true;
                PoisonBoltTimer = 3000;
                FrenzyTimer = urand(25000, 35000);
            }
			else FrenzyTimer -= diff;

            if (WyvernTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_WYVERN_STING);
					WyvernTimer = urand(15000, 32000);
				}
            }
			else WyvernTimer -= diff;

            if (SpitTimer <= diff)
            {
                DoCastVictim(SPELL_ACID_SPIT);
                SpitTimer = urand(5000, 10000);
            }
			else SpitTimer -= diff;

            if (NoxiousPoisonTimer <= diff)
            {
                DoCastVictim(SPELL_NOXIOUS_POISON);
                NoxiousPoisonTimer = urand(12000, 24000);
            }
			else NoxiousPoisonTimer -= diff;

            if (Frenzy || Berserk)
            {
                if (PoisonBoltTimer <= diff)
                {
                    DoCastAOE(SPELL_POISON_BOLT);
                    PoisonBoltTimer = 3000;
                }
				else PoisonBoltTimer -= diff;
            }

            if (Frenzy && FrenzyBackTimer <= diff)
            {
                me->InterruptNonMeleeSpells(false);
                Frenzy = false;
                FrenzyBackTimer = 15000;
            }
			else FrenzyBackTimer -= diff;

            if (!Berserk && HealthBelowPct(31))
            {
                me->InterruptNonMeleeSpells(false);
                DoSendQuantumText(EMOTE_GENERIC_BERSERK, me);
                DoCast(me, SPELL_BERSERK);
                Berserk = true;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_princess_huhuranAI(creature);
    }
};

void AddSC_boss_princess_huhuran()
{
    new boss_princess_huhuran();
}