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
#include "gundrak.h"

enum Texts
{
	EMOTE_SPAWN = -1604030,
};

enum Spells
{
    SPELL_ECK_BITE     = 55813,
    SPELL_ECK_SPIT     = 55814, 
	SPELL_ECK_BERSERK  = 55816,
    SPELL_ECK_SPRING_1 = 55815,
	SPELL_ECK_SPRING_2 = 55837,
};

class boss_eck : public CreatureScript
{
    public:
        boss_eck() : CreatureScript("boss_eck") { }

        struct boss_eckAI : public QuantumBasicAI
        {
            boss_eckAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();

				DoSendQuantumText(EMOTE_SPAWN, me);
            }

			InstanceScript* instance;

            uint32 BerserkTimer;
            uint32 BiteTimer;
            uint32 SpitTimer;
            uint32 SpringTimer;

            bool Berserk;

            void Reset()
            {
                BerserkTimer = 90000;
				BiteTimer = 5000;
                SpitTimer = 7000;
                SpringTimer = 12000;

                Berserk = false;

				instance->SetData(DATA_ECK_THE_FEROCIOUS_EVENT, NOT_STARTED);

				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

            void EnterToBattle(Unit* /*who*/)
            {
				instance->SetData(DATA_ECK_THE_FEROCIOUS_EVENT, IN_PROGRESS);
            }

			void JustDied(Unit* /*killer*/)
            {
				instance->SetData(DATA_ECK_THE_FEROCIOUS_EVENT, DONE);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

				if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (!Berserk)
                {
                    if (BerserkTimer <= diff)
                    {
                        DoCast(me, SPELL_ECK_BERSERK, true);
                        Berserk = true;
                    }
                    else BerserkTimer -= diff;
                }

                if (BiteTimer <= diff)
                {
                    DoCastVictim(SPELL_ECK_BITE);
                    BiteTimer = urand(8000, 12000);
                }
                else BiteTimer -= diff;

                if (SpitTimer <= diff)
                {
                    DoCast(SPELL_ECK_SPIT);
                    SpitTimer = urand(11000, 20000);
                }
                else SpitTimer -= diff;

                if (SpringTimer <= diff)
                {
					if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 35.0f, true))
                    {
						DoCast(target, RAND(SPELL_ECK_SPRING_1, SPELL_ECK_SPRING_2));
						SpringTimer = urand(9000, 15000);
                    }
                }
                else SpringTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_eckAI(creature);
        }
};

void AddSC_boss_eck()
{
    new boss_eck();
}