/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "ruins_of_ahnqiraj.h"

enum Spells
{
	SPELL_TRASH          = 3417,
	SPELL_FRENZY         = 8269,
	SPELL_POISON_STINGER = 25748,
    SPELL_STINGER_SPRAY  = 25749,
    SPELL_PARALYZE       = 25725, // need more info!
    SPELL_LASH           = 25852,
	SPELL_FEED           = 25721, // need more info!
};

enum Phases
{
	PHASE_ONE = 1,
	PHASE_TWO = 2,
};

class boss_ayamiss_the_hunter : public CreatureScript
{
    public:
        boss_ayamiss_the_hunter() : CreatureScript("boss_ayamiss_the_hunter") { }

        struct boss_ayamiss_the_hunterAI : public QuantumBasicAI
        {
            boss_ayamiss_the_hunterAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;

            uint32 StingerSprayTimer;
            uint32 PoisonStingerTimer;
			uint32 LashTimer;
            uint32 SummonSwarmerTimer;
            uint32 Phase;

            void Reset()
            {
				Phase = PHASE_ONE;

				LashTimer = 2*IN_MILLISECONDS;
                StingerSprayTimer = 10*IN_MILLISECONDS;
                PoisonStingerTimer = 12*IN_MILLISECONDS;
                SummonSwarmerTimer = 1*MINUTE*IN_MILLISECONDS;

				DoCast(me, SPELL_UNIT_DETECTION_ALL);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

			void EnterToBattle(Unit* /*who*/)
			{
				DoCast(me, SPELL_TRASH);
			}

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

                if (Phase == PHASE_ONE && !HealthAbovePct(HEALTH_PERCENT_70) && !me->IsNonMeleeSpellCasted(false))
                    Phase = PHASE_TWO;

				if (Phase == PHASE_ONE || Phase == PHASE_TWO && LashTimer <= diff)
                {
                    DoCastVictim(SPELL_LASH);
                    LashTimer = 7*IN_MILLISECONDS;
                }
				else LashTimer -= diff;

                if (Phase == PHASE_TWO && StingerSprayTimer <= diff)
                {
                    DoCastAOE(SPELL_STINGER_SPRAY);
                    StingerSprayTimer = 15*IN_MILLISECONDS;
                }
				else StingerSprayTimer -= diff;

                if (Phase == PHASE_ONE && PoisonStingerTimer <= diff)
                {
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, SPELL_POISON_STINGER);
						PoisonStingerTimer = 15*IN_MILLISECONDS;
					}
                }
				else PoisonStingerTimer -= diff;

				if (HealthBelowPct(HEALTH_PERCENT_30))
				{
					if (!me->HasAuraEffect(SPELL_FRENZY, 0))
						DoSendQuantumText(SAY_GENERIC_EMOTE_FRENZY, me);
				}

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_ayamiss_the_hunterAI(creature);
        }
};

void AddSC_boss_ayamiss_the_hunter()
{
    new boss_ayamiss_the_hunter();
}