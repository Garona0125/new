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

/* ScriptData
SDName: Boss_Jin'do the Hexxer
SD%Complete: 85
SDComment: Mind Control not working because of core bug. Shades visible for all.
SDCategory: Zul'Gurub
EndScriptData */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "zulgurub.h"

enum Jindo
{
	SAY_AGGRO                  = 1,

	SPELL_BRAINWASHTOTEM       = 24262,
	SPELL_POWERFULLHEALINGWARD = 24309,
	SPELL_HEX                  = 24053,
	SPELL_DELUSIONSOFJINDO     = 24306,
	SPELL_SHADEOFJINDO         = 24308,
	SPELL_HEAL                 = 38588,
	SPELL_SHADOWSHOCK          = 19460,
	SPELL_INVISIBLE            = 24699,
};

class boss_jindo : public CreatureScript
{
    public:
        boss_jindo() : CreatureScript("boss_jindo") {}

        struct boss_jindoAI : public QuantumBasicAI
        {
            boss_jindoAI(Creature* creature) : QuantumBasicAI(creature) {}

            uint32 BrainWashTotemTimer;
            uint32 HealingWardTimer;
            uint32 HexTimer;
            uint32 DelusionsTimer;
            uint32 TeleportTimer;

            void Reset()
            {
                BrainWashTotemTimer = 20000;
                HealingWardTimer = 16000;
                HexTimer = 8000;
                DelusionsTimer = 10000;
                TeleportTimer = 5000;
            }

            void EnterToBattle(Unit* /*who*/)
            {
				Talk(SAY_AGGRO);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                //BrainWashTotemTimer
                if (BrainWashTotemTimer <= diff)
                {
                    DoCast(me, SPELL_BRAINWASHTOTEM);
                    BrainWashTotemTimer = urand(18000, 26000);
                }
				else BrainWashTotemTimer -= diff;

                //HealingWardTimer
                if (HealingWardTimer <= diff)
                {
                    //DoCast(me, SPELL_POWERFULLHEALINGWARD);
                    me->SummonCreature(14987, me->GetPositionX()+3, me->GetPositionY()-2, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 30000);
                    HealingWardTimer = urand(14000, 20000);
                }
				else HealingWardTimer -= diff;

                //HexTimer
                if (HexTimer <= diff)
                {
                    DoCastVictim(SPELL_HEX);

                    if (DoGetThreat(me->GetVictim()))
                        DoModifyThreatPercent(me->GetVictim(), -80);

                    HexTimer = urand(12000, 20000);
                }
				else HexTimer -= diff;

                //Casting the delusion curse with a shade. So shade will attack the same target with the curse.
                if (DelusionsTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                    {
                        DoCast(target, SPELL_DELUSIONSOFJINDO);

                        Creature* Shade = me->SummonCreature(14986, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                        if (Shade)
                            Shade->AI()->AttackStart(target);
                    }

                    DelusionsTimer = urand(4000, 12000);
                }
				else DelusionsTimer -= diff;

                //Teleporting a random gamer and spawning 9 skeletons that will attack this gamer
                if (TeleportTimer <= diff)
                {
                    Unit* target = NULL;
                    target = SelectTarget(TARGET_RANDOM, 0);
                    if (target && target->GetTypeId() == TYPE_ID_PLAYER)
                    {
                        DoTeleportPlayer(target, -11583.7783f, -1249.4278f, 77.5471f, 4.745f);

                        if (DoGetThreat(me->GetVictim()))
                            DoModifyThreatPercent(target, -100);

                        Creature* Skeletons;
                        Skeletons = me->SummonCreature(14826, target->GetPositionX()+2, target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                        if (Skeletons)
                            Skeletons->AI()->AttackStart(target);
                        Skeletons = me->SummonCreature(14826, target->GetPositionX()-2, target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                        if (Skeletons)
                            Skeletons->AI()->AttackStart(target);
                        Skeletons = me->SummonCreature(14826, target->GetPositionX()+4, target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                        if (Skeletons)
                            Skeletons->AI()->AttackStart(target);
                        Skeletons = me->SummonCreature(14826, target->GetPositionX()-4, target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                        if (Skeletons)
                            Skeletons->AI()->AttackStart(target);
                        Skeletons = me->SummonCreature(14826, target->GetPositionX(), target->GetPositionY()+2, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                        if (Skeletons)
                            Skeletons->AI()->AttackStart(target);
                        Skeletons = me->SummonCreature(14826, target->GetPositionX(), target->GetPositionY()-2, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                        if (Skeletons)
                            Skeletons->AI()->AttackStart(target);
                        Skeletons = me->SummonCreature(14826, target->GetPositionX(), target->GetPositionY()+4, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                        if (Skeletons)
                            Skeletons->AI()->AttackStart(target);
                        Skeletons = me->SummonCreature(14826, target->GetPositionX(), target->GetPositionY()-4, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                        if (Skeletons)
                            Skeletons->AI()->AttackStart(target);
                        Skeletons = me->SummonCreature(14826, target->GetPositionX()+3, target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                        if (Skeletons)
                            Skeletons->AI()->AttackStart(target);
                    }

                    TeleportTimer = urand(15000, 23000);
                }
				else TeleportTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_jindoAI(creature);
        }
};

//Healing Ward
class mob_healing_ward : public CreatureScript
{
    public:
        mob_healing_ward() : CreatureScript("mob_healing_ward") {}

        struct mob_healing_wardAI : public QuantumBasicAI
        {
            mob_healing_wardAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            uint32 HealTimer;

            InstanceScript* instance;

            void Reset()
            {
                HealTimer = 2000;
            }

            void EnterToBattle(Unit* /*who*/){}

            void UpdateAI (const uint32 diff)
            {
                //HealTimer
                if (HealTimer <= diff)
                {
                    if (instance)
                    {
                        Unit* pJindo = Unit::GetUnit(*me, instance->GetData64(DATA_JINDO));
                        if (pJindo)
                            DoCast(pJindo, SPELL_HEAL);
                    }
                    HealTimer = 3000;
                }
				else HealTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_healing_wardAI(creature);
        }
};

//Shade of Jindo
class mob_shade_of_jindo : public CreatureScript
{
    public:
        mob_shade_of_jindo() : CreatureScript("mob_shade_of_jindo") {}

        struct mob_shade_of_jindoAI : public QuantumBasicAI
        {
            mob_shade_of_jindoAI(Creature* creature) : QuantumBasicAI(creature) {}

            uint32 ShadowShockTimer;

            void Reset()
            {
                ShadowShockTimer = 1000;
                DoCast(me, SPELL_INVISIBLE, true);
            }

            void EnterToBattle(Unit* /*who*/){}

            void UpdateAI (const uint32 diff)
            {

                //ShadowShockTimer
                if (ShadowShockTimer <= diff)
                {
                    DoCastVictim(SPELL_SHADOWSHOCK);
                    ShadowShockTimer = 2000;
                }
				else ShadowShockTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_shade_of_jindoAI(creature);
        }
};

void AddSC_boss_jindo()
{
    new boss_jindo();
    new mob_healing_ward();
    new mob_shade_of_jindo();
}