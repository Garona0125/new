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

enum Texts
{
	SAY_AGGRO  = -1531008,
	SAY_SLAY   = -1531009,
	SAY_DEATH  = -1531010,
};

enum Spells
{
	SPELL_WHIRLWIND   = 26083,
	SPELL_ENRAGE      = 28747,
	SPELL_ENRAGE_HARD = 28798,
};

class boss_battleguard_sartura : public CreatureScript
{
public:
    boss_battleguard_sartura() : CreatureScript("boss_battleguard_sartura") { }

    struct boss_battleguard_sarturaAI : public QuantumBasicAI
    {
        boss_battleguard_sarturaAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 WhirlWindTimer;
        uint32 WhirlWindRandomTimer;
        uint32 WhirlWindEndTimer;
        uint32 AggroResetTimer;
        uint32 AggroResetEndTimer;
        uint32 EnrageHardTimer;

        bool Enraged;
        bool EnragedHard;
        bool WhirlWind;
        bool AggroReset;

        void Reset()
        {
            WhirlWindTimer = 30000;
            WhirlWindRandomTimer = urand(3000, 7000);
            WhirlWindEndTimer = 15000;
            AggroResetTimer = urand(45000, 55000);
            AggroResetEndTimer = 5000;
            EnrageHardTimer = 10*60000;

            WhirlWind = false;
            AggroReset = false;
            Enraged = false;
            EnragedHard = false;

        }

        void EnterToBattle(Unit* who)
        {
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(SAY_AGGRO, me);
        }

         void JustDied(Unit* /*killer*/)
         {
             DoSendQuantumText(SAY_DEATH, me);
         }

         void KilledUnit(Unit* victim)
         {
			 if (victim->GetTypeId() == TYPE_ID_PLAYER)
				 DoSendQuantumText(SAY_SLAY, me);
         }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (WhirlWind)
            {
                if (WhirlWindRandomTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 100.0f, true))
                    {
                        me->AddThreat(target, 1.0f);
                        me->TauntApply(target);
                        AttackStart(target);
                    }
                    WhirlWindRandomTimer = urand(3000, 7000);
                }
				else WhirlWindRandomTimer -= diff;

                if (WhirlWindEndTimer <= diff)
                {
                    WhirlWind = false;
                    WhirlWindTimer = urand(25000, 40000);
                }
				else WhirlWindEndTimer -= diff;
            }

            if (!WhirlWind)
            {
                if (WhirlWindTimer <= diff)
                {
                    DoCast(me, SPELL_WHIRLWIND);
                    WhirlWind = true;
                    WhirlWindEndTimer = 15000;
                }
				else WhirlWindTimer -= diff;

                if (AggroResetTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 100.0f, true))
                    {
                        me->AddThreat(target, 1.0f);
                        me->TauntApply(target);
                        AttackStart(target);
                    }
                    AggroReset = true;
                    AggroResetTimer = urand(2000, 5000);
                }
				else AggroResetTimer -= diff;

                if (AggroReset)
                {
                    if (AggroResetEndTimer <= diff)
                    {
                        AggroReset = false;
                        AggroResetEndTimer = 5000;
                        AggroResetTimer = urand(35000, 45000);
                    }
					else AggroResetEndTimer -= diff;
                }

                if (!Enraged)
                {
                    if (!HealthAbovePct(50) && !me->IsNonMeleeSpellCasted(false))
                    {
						DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
                        DoCast(me, SPELL_ENRAGE);
                        Enraged = true;
                    }
                }

                if (!EnragedHard)
                {
                    if (EnrageHardTimer <= diff)
                    {
						DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
                        DoCast(me, SPELL_ENRAGE_HARD);
                        EnragedHard = true;
                    }
					else EnrageHardTimer -= diff;
                }

                DoMeleeAttackIfReady();
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_battleguard_sarturaAI (creature);
    }
};

void AddSC_boss_battleguard_sartura()
{
    new boss_battleguard_sartura();
}