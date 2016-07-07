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
SDName: Boss_Jeklik
SD%Complete: 85
SDComment: Problem in finding the right flying batriders for spawning and making them fly.
SDCategory: Zul'Gurub
EndScriptData */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "zulgurub.h"

enum Says
{
	SAY_AGGRO                   = 0,
	SAY_RAIN_FIRE               = 1,
	SAY_DEATH                   = 2,
};

enum Spells
{
	SPELL_CHARGE             = 22911,
	SPELL_SONICBURST         = 23918,
	SPELL_SCREECH            = 6605,
	SPELL_SHADOW_WORD_PAIN   = 23952,
	SPELL_MIND_FLAY          = 23953,
	SPELL_CHAIN_MIND_FLAY    = 26044,
	SPELL_GREATERHEAL        = 23954,
	SPELL_BAT_FORM           = 23966,
	// Batriders Spell
	SPELL_BOMB               = 40332,
};

class boss_jeklik : public CreatureScript
{
    public:
        boss_jeklik() : CreatureScript("boss_jeklik") {}

        struct boss_jeklikAI : public QuantumBasicAI
        {
            boss_jeklikAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 ChargeTimer;
            uint32 SonicBurstTimer;
            uint32 ScreechTimer;
            uint32 SpawnBatsTimer;
            uint32 ShadowWordPainTimer;
            uint32 MindFlayTimer;
            uint32 ChainMindFlayTimer;
            uint32 GreaterHealTimer;
            uint32 SpawnFlyingBatsTimer;

            bool PhaseTwo;

            void Reset()
            {
                ChargeTimer = 20000;
                SonicBurstTimer = 8000;
                ScreechTimer = 13000;
                SpawnBatsTimer = 60000;
                ShadowWordPainTimer = 6000;
                MindFlayTimer = 11000;
                ChainMindFlayTimer = 26000;
                GreaterHealTimer = 50000;
                SpawnFlyingBatsTimer = 10000;

                PhaseTwo = false;
            }

            void EnterToBattle(Unit* /*who*/)
            {
                Talk(SAY_AGGRO);
                DoCast(me, SPELL_BAT_FORM);
            }

            void JustDied(Unit* /*killer*/)
            {
                Talk(SAY_DEATH);

                if (instance)
                    instance->SetData(DATA_JEKLIK, DONE);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (me->GetVictim() && me->IsAlive())
                {
                    if (HealthAbovePct(50))
                    {
                        if (ChargeTimer <= diff)
                        {
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                            {
                                DoCast(target, SPELL_CHARGE);
                                AttackStart(target);
                            }

                            ChargeTimer = urand(15000, 30000);
                        }
						else ChargeTimer -= diff;

                        if (SonicBurstTimer <= diff)
                        {
                            DoCastVictim(SPELL_SONICBURST);
                            SonicBurstTimer = urand(8000, 13000);
                        }
						else SonicBurstTimer -= diff;

                        if (ScreechTimer <= diff)
                        {
                            DoCastVictim(SPELL_SCREECH);
                            ScreechTimer = urand(18000, 26000);
                        }
						else ScreechTimer -= diff;

                        if (SpawnBatsTimer <= diff)
                        {
                            Unit* target = SelectTarget(TARGET_RANDOM, 0);

                            Creature* Bat = NULL;
                            Bat = me->SummonCreature(11368, -12291.6220f, -1380.2640f, 144.8304f, 5.483f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                            if (target && Bat) Bat ->AI()->AttackStart(target);

                            Bat = me->SummonCreature(11368, -12289.6220f, -1380.2640f, 144.8304f, 5.483f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                            if (target && Bat) Bat ->AI()->AttackStart(target);

                            Bat = me->SummonCreature(11368, -12293.6220f, -1380.2640f, 144.8304f, 5.483f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                            if (target && Bat) Bat ->AI()->AttackStart(target);

                            Bat = me->SummonCreature(11368, -12291.6220f, -1380.2640f, 144.8304f, 5.483f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                            if (target && Bat) Bat ->AI()->AttackStart(target);

                            Bat = me->SummonCreature(11368, -12289.6220f, -1380.2640f, 144.8304f, 5.483f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                            if (target && Bat) Bat ->AI()->AttackStart(target);
                            Bat = me->SummonCreature(11368, -12293.6220f, -1380.2640f, 144.8304f, 5.483f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                            if (target && Bat) Bat ->AI()->AttackStart(target);

                            SpawnBatsTimer = 60000;
                        }
						else SpawnBatsTimer -= diff;
                    }
                    else
                    {
                        if (PhaseTwo)
                        {
                            if (PhaseTwo && ShadowWordPainTimer <= diff)
                            {
                                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                                {
                                    DoCast(target, SPELL_SHADOW_WORD_PAIN);
                                    ShadowWordPainTimer = urand(12000, 18000);
                                }
                            }
							ShadowWordPainTimer -= diff;

                            if (MindFlayTimer <= diff)
                            {
                                DoCastVictim(SPELL_MIND_FLAY);
                                MindFlayTimer = 16000;
                            }
							MindFlayTimer -= diff;

                            if (ChainMindFlayTimer <= diff)
                            {
                                me->InterruptNonMeleeSpells(false);
                                DoCastVictim(SPELL_CHAIN_MIND_FLAY);
                                ChainMindFlayTimer = urand(15000, 30000);
                            }
							ChainMindFlayTimer -= diff;

                            if (GreaterHealTimer <= diff)
                            {
                                me->InterruptNonMeleeSpells(false);
                                DoCast(me, SPELL_GREATERHEAL);
                                GreaterHealTimer = urand(25000, 35000);
                            }
							GreaterHealTimer -= diff;

                            if (SpawnFlyingBatsTimer <= diff)
                            {
                                Unit* target = SelectTarget(TARGET_RANDOM, 0);
                                if (!target)
                                    return;

                                Creature* FlyingBat = me->SummonCreature(14965, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ()+15, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                                if (FlyingBat)
                                    FlyingBat->AI()->AttackStart(target);

                                SpawnFlyingBatsTimer = urand(10000, 15000);
                            }
							else SpawnFlyingBatsTimer -= diff;
                        }
                        else
                        {
                            me->SetDisplayId(15219);
                            DoResetThreat();
                            PhaseTwo = true;
                        }
                    }

                    DoMeleeAttackIfReady();
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_jeklikAI(creature);
        }
};

//Flying Bat
class mob_batrider : public CreatureScript
{
    public:
        mob_batrider() : CreatureScript("mob_batrider") {}

        struct mob_batriderAI : public QuantumBasicAI
        {
            mob_batriderAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 BombTimer;
            uint32 CheckTimer;

            void Reset()
            {
                BombTimer = 2000;
                CheckTimer = 1000;

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void EnterToBattle(Unit* /*who*/) {}

            void UpdateAI (const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                //BombTimer
                if (BombTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                    {
                        DoCast(target, SPELL_BOMB);
                        BombTimer = 5000;
                    }
                }
				else BombTimer -= diff;

                //CheckTimer
                if (CheckTimer <= diff)
                {
                    if (instance)
                    {
                        if (instance->GetData(DATA_JEKLIK) == DONE)
                        {
                            me->setDeathState(JUST_DIED);
                            me->RemoveCorpse();
                            return;
                        }
                    }

                    CheckTimer = 1000;
                }
				else CheckTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_batriderAI(creature);
        }
};

void AddSC_boss_jeklik()
{
    new boss_jeklik();
    new mob_batrider();
}