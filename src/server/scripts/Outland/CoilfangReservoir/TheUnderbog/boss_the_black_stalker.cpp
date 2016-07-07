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
	SPELL_LEVITATE             = 31704,
	SPELL_SUSPENSION           = 31719,
	SPELL_LEVITATION_PULSE     = 31701,
	SPELL_MAGNETIC_PULL        = 31705,
	SPELL_CHAIN_LIGHTNING      = 31717,
	SPELL_STATIC_CHARGE        = 31715,
	SPELL_SUMMON_SPORE_STRIDER = 38755,
};

class boss_the_black_stalker : public CreatureScript
{
public:
    boss_the_black_stalker() : CreatureScript("boss_the_black_stalker") { }

    struct boss_the_black_stalkerAI : public QuantumBasicAI
    {
        boss_the_black_stalkerAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 SporeStridersTimer;
        uint32 LevitateTimer;
        uint32 ChainLightningTimer;
        uint32 StaticChargeTimer;
        uint32 LevitatedTargetTimer;
		uint32 CheckTimer;

        bool InAir;

		uint64 LevitatedTarget;

        std::list<uint64> Striders;

        void Reset()
        {
            LevitateTimer = 12000;
            ChainLightningTimer = 6000;
            StaticChargeTimer = 10000;
            SporeStridersTimer = 10000+rand()%5000;
            CheckTimer = 5000;
            LevitatedTarget = 0;
            LevitatedTargetTimer = 0;
            Striders.clear();

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void EnterToBattle(Unit* /*who*/) {}

        void JustSummoned(Creature* summon)
        {
            if (summon && summon->GetEntry() == NPC_SPORE_STRIDER)
            {
                Striders.push_back(summon->GetGUID());
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
                    summon->AI()->AttackStart(target);
                else
                    if (me->GetVictim())
                        summon->AI()->AttackStart(me->GetVictim());
            }
        }

        void JustDied(Unit* /*who*/)
        {
            for (std::list<uint64>::const_iterator i = Striders.begin(); i != Striders.end(); ++i)
			{
                if (Creature* strider = Unit::GetCreature(*me, *i))
                    strider->DisappearAndDie();
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            // Evade if too far
            if (CheckTimer <= diff)
            {
                float x, y, z, o;
                me->GetHomePosition(x, y, z, o);
                if (!me->IsWithinDist3d(x, y, z, 60.0f))	
                {
                    EnterEvadeMode();
                    return;
                }
                CheckTimer = 1000;
            }
			else CheckTimer -= diff;

            if (SporeStridersTimer <= diff && IsHeroic())
            {
                DoCast(me, SPELL_SUMMON_SPORE_STRIDER);
                SporeStridersTimer = 10000+rand()%5000;
            }
			else SporeStridersTimer -= diff;

            if (LevitatedTarget)
            {
                if (LevitatedTargetTimer <= diff)
                {
                    if (Unit* target = Unit::GetUnit(*me, LevitatedTarget))
                    {
                        if (!target->HasAura(SPELL_LEVITATE))
                        {
                            LevitatedTarget = 0;
                            return;
                        }
                        if (InAir)
                        {
                            target->AddAura(SPELL_SUSPENSION, target);
                            LevitatedTarget = 0;
                        }
                        else
                        {
                            target->CastSpell(target, SPELL_MAGNETIC_PULL, true);
                            InAir = true;
                            LevitatedTargetTimer = 1500;
                        }
                    }
                    else LevitatedTarget = 0;
                }
				else LevitatedTargetTimer -= diff;
            }
            if (LevitateTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
                {
                    DoCast(target, SPELL_LEVITATE);
                    LevitatedTarget = target->GetGUID();
                    LevitatedTargetTimer = 2000;
                    InAir = false;
                }
                LevitateTimer = 12000+rand()%3000;
            }
			else LevitateTimer -= diff;

            if (ChainLightningTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, SPELL_CHAIN_LIGHTNING);
					ChainLightningTimer = 5000;
				}
            }
			else ChainLightningTimer -= diff;

            if (StaticChargeTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 30, true))
				{
                    DoCast(target, SPELL_STATIC_CHARGE);
					StaticChargeTimer = 10000;
				}
            }
			else StaticChargeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_the_black_stalkerAI(creature);
    }
};

void AddSC_boss_the_black_stalker()
{
    new boss_the_black_stalker();
}