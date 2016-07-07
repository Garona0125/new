/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "magisters_terrace.h"

enum Intervals
{
	INTERVAL_MODIFIER               = 15,
	INTERVAL_SWITCH                 = 6,
};

enum Texts
{
	SAY_AGGRO                       = 0,
	SAY_ENERGY                      = 1,
	SAY_OVERLOAD                    = 2,
	SAY_KILL                        = 3,
	EMOTE_DISCHARGE_ENERGY          = 4,
};

enum Spells
{
    SPELL_ENERGY_BOLT               = 46156,
    SPELL_ENERGY_FEEDBACK           = 44335,
    SPELL_CHAIN_LIGHTNING_5N        = 44318,
    SPELL_CHAIN_LIGHTNING_5H        = 46380,
    SPELL_OVERLOAD                  = 44353,
    SPELL_ARCANE_SHOCK_5N           = 44319,
    SPELL_ARCANE_SHOCK_5H           = 46381,
    SPELL_SUMMON_PURE_ENERGY        = 44322,
    H_SPELL_SUMMON_PURE_ENERGY_1    = 46154,
    H_SPELL_SUMMON_PURE_ENERGY_2    = 46159,
};

class boss_vexallus : public CreatureScript
{
public:
    boss_vexallus() : CreatureScript("boss_vexallus") { }

    struct boss_vexallusAI : public QuantumBasicAI
    {
        boss_vexallusAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        uint32 ChainLightningTimer;
        uint32 ArcaneShockTimer;
        uint32 OverloadTimer;
        uint32 IntervalHealthAmount;
        bool Enraged;

        void Reset()
        {
            ChainLightningTimer = 8000;
            ArcaneShockTimer = 5000;
            OverloadTimer = 1200;
            IntervalHealthAmount = 1;
            Enraged = false;

            if (instance)
                instance->SetData(DATA_VEXALLUS_EVENT, NOT_STARTED);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            Talk(SAY_KILL);
        }

        void JustDied(Unit* /*victim*/)
        {
            if (instance)
                instance->SetData(DATA_VEXALLUS_EVENT, DONE);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            Talk(SAY_AGGRO);

            if (instance)
                instance->SetData(DATA_VEXALLUS_EVENT, IN_PROGRESS);
        }

        void JustSummoned(Creature* summoned)
        {
            if (Unit* temp = SelectTarget(TARGET_RANDOM, 0))
                summoned->GetMotionMaster()->MoveFollow(temp, 0, 0);

            summoned->CastSpell(summoned, SPELL_ENERGY_BOLT, false, 0, 0, me->GetGUID());
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (!Enraged)
            {
                if (!HealthAbovePct(100 - INTERVAL_MODIFIER * IntervalHealthAmount))
                {
                    if (IntervalHealthAmount == INTERVAL_SWITCH)
                    {
                        Enraged = true;
                        return;
                    }
                    else ++IntervalHealthAmount;

                    Talk(SAY_ENERGY);
                    Talk(EMOTE_DISCHARGE_ENERGY);

                    if (IsHeroic())
                    {
                        DoCast(me, H_SPELL_SUMMON_PURE_ENERGY_1, false);
                        DoCast(me, H_SPELL_SUMMON_PURE_ENERGY_2, false);
                    }
                    else
                        DoCast(me, SPELL_SUMMON_PURE_ENERGY, false);

                    me->SummonCreature(NPC_PURE_ENERGY, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);

                    if (IsHeroic())
                        me->SummonCreature(NPC_PURE_ENERGY, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);
                }

                if (ChainLightningTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, DUNGEON_MODE(SPELL_CHAIN_LIGHTNING_5N, SPELL_CHAIN_LIGHTNING_5H));
						ChainLightningTimer = 8000;
					}
                }
				else ChainLightningTimer -= diff;

                if (ArcaneShockTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, DUNGEON_MODE(SPELL_ARCANE_SHOCK_5N, SPELL_ARCANE_SHOCK_5H));
						ArcaneShockTimer = 8000;
					}
                }
				else ArcaneShockTimer -= diff;
            }
            else
            {
                if (OverloadTimer <= diff)
                {
                    DoCastVictim(SPELL_OVERLOAD);

                    OverloadTimer = 2000;
                }
				else OverloadTimer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_vexallusAI(creature);
    }
};

class npc_pure_energy : public CreatureScript
{
public:
    npc_pure_energy() : CreatureScript("npc_pure_energy") { }

    struct npc_pure_energyAI : public QuantumBasicAI
    {
        npc_pure_energyAI(Creature* creature) : QuantumBasicAI(creature) {}

        void Reset()
        {
            me->SetDisplayId(28988);
        }

        void JustDied(Unit* slayer)
        {
            me->DespawnAfterAction(0.5*IN_MILLISECONDS);

            if (Unit* temp = me->GetOwner())
			{
                if (temp->IsAlive())
                    slayer->CastSpell(slayer, SPELL_ENERGY_FEEDBACK, true, 0, 0, temp->GetGUID());
			}
        }

        void EnterToBattle(Unit* /*who*/) {}

        void MoveInLineOfSight(Unit* /*who*/) {}

        void AttackStart(Unit* /*who*/) {}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_pure_energyAI(creature);
    }
};

void AddSC_boss_vexallus()
{
    new boss_vexallus();
    new npc_pure_energy();
}