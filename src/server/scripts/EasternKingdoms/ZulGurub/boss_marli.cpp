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
SDName: Boss_Marli
SD%Complete: 80
SDComment: Charging healers and casters not working. Perhaps wrong Spell Timers.
SDCategory: Zul'Gurub
EndScriptData */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "zulgurub.h"

enum Says
{
	SAY_AGGRO               = 0,
	SAY_TRANSFORM           = 1,
	SAY_SPIDER_SPAWN        = 2,
	SAY_DEATH               = 3,
};

enum Spells
{
	SPELL_CHARGE             = 22911,
	SPELL_ASPECT_OF_MARLI    = 24686,
	SPELL_ENVOLWING_WEB      = 24110,
	SPELL_POISON_VOLLEY      = 24099,
	SPELL_SPIDER_FORM        = 24084,
	SPELL_LEVELUP            = 24312,
};

class boss_marli : public CreatureScript
{
    public:
        boss_marli() : CreatureScript("boss_marli") {}

        struct boss_marliAI : public QuantumBasicAI
        {
            boss_marliAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 SpawnStartSpidersTimer;
            uint32 PoisonVolleyTimer;
            uint32 SpawnSpiderTimer;
            uint32 ChargeTimer;
            uint32 AspectTimer;
            uint32 TransformTimer;
            uint32 TransformBackTimer;

            bool Spawned;
            bool PhaseTwo;

            void Reset()
            {
                SpawnStartSpidersTimer = 1000;
                PoisonVolleyTimer = 15000;
                SpawnSpiderTimer = 30000;
                ChargeTimer = 1500;
                AspectTimer = 12000;
                TransformTimer = 45000;
                TransformBackTimer = 25000;

                Spawned = false;
                PhaseTwo = false;
            }

            void EnterToBattle(Unit* /*who*/)
            {
                Talk(SAY_AGGRO);
            }

            void JustDied(Unit* /*Killer*/)
            {
                Talk(SAY_DEATH);
                if (instance)
                    instance->SetData(DATA_MARLI, DONE);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (me->GetVictim() && me->IsAlive())
                {
                    if (PoisonVolleyTimer <= diff)
                    {
                        DoCastVictim(SPELL_POISON_VOLLEY);
                        PoisonVolleyTimer = urand(10000, 20000);
                    }
					else PoisonVolleyTimer -= diff;

                    if (!PhaseTwo && AspectTimer <= diff)
                    {
                        DoCastVictim(SPELL_ASPECT_OF_MARLI);
                        AspectTimer = urand(13000, 18000);
                    }
					else AspectTimer -= diff;

                    if (!Spawned && SpawnStartSpidersTimer <= diff)
                    {
						Talk(SAY_SPIDER_SPAWN);

                        Unit* target = SelectTarget(TARGET_RANDOM, 0);
                        if (!target)
                            return;

                        Creature* Spider = NULL;

                        Spider = me->SummonCreature(15041, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                        if (Spider)
                            Spider->AI()->AttackStart(target);
                        Spider = me->SummonCreature(15041, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                        if (Spider)
                            Spider->AI()->AttackStart(target);
                        Spider = me->SummonCreature(15041, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                        if (Spider)
                            Spider->AI()->AttackStart(target);
                        Spider = me->SummonCreature(15041, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                        if (Spider)
                            Spider->AI()->AttackStart(target);

                        Spawned = true;
                    }
					else SpawnStartSpidersTimer -= diff;

                    if (SpawnSpiderTimer <= diff)
                    {
                        Unit* target = SelectTarget(TARGET_RANDOM, 0);
                        if (!target)
                            return;

                        Creature* Spider = me->SummonCreature(15041, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                        if (Spider)
                            Spider->AI()->AttackStart(target);
                        SpawnSpiderTimer = urand(12000, 17000);
                    }
					else SpawnSpiderTimer -= diff;

                    if (!PhaseTwo && TransformTimer <= diff)
                    {
                        Talk(SAY_TRANSFORM);
                        DoCast(me, SPELL_SPIDER_FORM);
                        const CreatureTemplate* cinfo = me->GetCreatureTemplate();
                        me->SetBaseWeaponDamage(BASE_ATTACK, MIN_DAMAGE, (cinfo->mindmg +((cinfo->mindmg/100) * 35)));
                        me->SetBaseWeaponDamage(BASE_ATTACK, MAX_DAMAGE, (cinfo->maxdmg +((cinfo->maxdmg/100) * 35)));
                        me->UpdateDamagePhysical(BASE_ATTACK);
                        DoCastVictim(SPELL_ENVOLWING_WEB);

                        if (DoGetThreat(me->GetVictim()))
                            DoModifyThreatPercent(me->GetVictim(), -100);

                        PhaseTwo = true;
                        TransformTimer = urand(35000, 60000);
                    }
					else TransformTimer -= diff;

                    if (PhaseTwo)
                    {
                        if (ChargeTimer <= diff)
                        {
                            Unit* target = NULL;
                            int i = 0;
                            while (i < 3)                           // max 3 tries to get a random target with power_mana
                            {
                                ++i;
                                target = SelectTarget(TARGET_RANDOM, 1, 100, true);  // not aggro leader
                                if (target && target->GetPowerType() == POWER_MANA)
									i = 3;
                            }

                            if (target)
                            {
                                DoCast(target, SPELL_CHARGE);
                                //me->SetPosition(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0);
                                //me->SendMonsterMove(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, true, 1);
                                AttackStart(target);
                            }

                            ChargeTimer = 8000;
                        }
						else ChargeTimer -= diff;

                        if (TransformBackTimer <= diff)
                        {
                            me->SetDisplayId(15220);
                            const CreatureTemplate* cinfo = me->GetCreatureTemplate();
                            me->SetBaseWeaponDamage(BASE_ATTACK, MIN_DAMAGE, (cinfo->mindmg +((cinfo->mindmg/100) * 1)));
                            me->SetBaseWeaponDamage(BASE_ATTACK, MAX_DAMAGE, (cinfo->maxdmg +((cinfo->maxdmg/100) * 1)));
                            me->UpdateDamagePhysical(BASE_ATTACK);

                            PhaseTwo = false;
                            TransformBackTimer = urand(25000, 40000);
                        }
						else TransformBackTimer -= diff;
                    }

                    DoMeleeAttackIfReady();
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_marliAI(creature);
        }
};

class mob_spawn_of_marli : public CreatureScript
{
    public:
        mob_spawn_of_marli() : CreatureScript("mob_spawn_of_marli") {}

        struct mob_spawn_of_marliAI : public QuantumBasicAI
        {
            mob_spawn_of_marliAI(Creature* creature) : QuantumBasicAI(creature) {}

            uint32 LevelUpTimer;

            void Reset()
            {
                LevelUpTimer = 3000;
            }

            void EnterToBattle(Unit* /*who*/){}

            void UpdateAI (const uint32 diff)
            {
                //Return since we have no target
                if (!UpdateVictim())
                    return;

                //LevelUpTimer
                if (LevelUpTimer <= diff)
                {
                    DoCast(me, SPELL_LEVELUP);
                    LevelUpTimer = 3000;
                }
				else LevelUpTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_spawn_of_marliAI(creature);
        }
};

void AddSC_boss_marli()
{
    new boss_marli();
    new mob_spawn_of_marli();
}