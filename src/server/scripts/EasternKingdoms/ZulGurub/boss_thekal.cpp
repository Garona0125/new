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
#include "zulgurub.h"

enum Says
{
	SAY_AGGRO              = 0,
	SAY_DEATH              = 1,
};

enum Spells
{
    SPELL_MORTALCLEAVE        = 22859,
    SPELL_SILENCE             = 22666,
    SPELL_FRENZY              = 8269,
    SPELL_FORCEPUNCH          = 24189,
    SPELL_CHARGE              = 24193,
    SPELL_ENRAGE              = 8269,
    SPELL_SUMMONTIGERS        = 24183,
    SPELL_TIGER_FORM          = 24169,
    SPELL_RESURRECT           = 24173,                    //We will not use this spell.

	// Zealot Lor'Khan Spells
    SPELL_SHIELD              = 20545,
    SPELL_BLOODLUST           = 24185,
    SPELL_GREATERHEAL         = 24208,
    SPELL_DISARM              = 6713,

	// Zealot Zath Spells
    SPELL_SWEEPINGSTRIKES     = 18765,
    SPELL_SINISTERSTRIKE      = 15581,
    SPELL_GOUGE               = 12540,
    SPELL_KICK                = 15614,
    SPELL_BLIND               = 21060,
};

class boss_thekal : public CreatureScript
{
    public:
        boss_thekal() : CreatureScript("boss_thekal") { }

        struct boss_thekalAI : public QuantumBasicAI
        {
            boss_thekalAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            uint32 MortalCleaveTimer;
            uint32 SilenceTimer;
            uint32 FrenzyTimer;
            uint32 ForcePunchTimer;
            uint32 ChargeTimer;
            uint32 EnrageTimer;
            uint32 SummonTigersTimer;
            uint32 CheckTimer;
            uint32 ResurrectTimer;

            InstanceScript* instance;
            bool Enraged;
            bool PhaseTwo;
            bool WasDead;

            void Reset()
            {
                MortalCleaveTimer = 4000;
                SilenceTimer = 9000;
                FrenzyTimer = 30000;
                ForcePunchTimer = 4000;
                ChargeTimer = 12000;
                EnrageTimer = 32000;
                SummonTigersTimer = 25000;
                CheckTimer = 10000;
                ResurrectTimer = 10000;

                Enraged = false;
                PhaseTwo = false;
                WasDead = false;
            }

            void EnterToBattle(Unit* /*who*/)
            {
                Talk(SAY_AGGRO);
            }

            void JustDied(Unit* /*Killer*/)
            {
                Talk(SAY_DEATH);
                if (instance)
                    instance->SetData(DATA_THEKAL, DONE);
            }

            void JustReachedHome()
            {
                if (instance)
                    instance->SetData(DATA_THEKAL, NOT_STARTED);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                    //CheckTimer for the death of LorKhan and Zath.
                    if (!WasDead && CheckTimer <= diff)
                    {
                        if (instance)
                        {
                            if (instance->GetData(DATA_LORKHAN) == SPECIAL)
                            {
                                //Resurrect LorKhan
                                if (Unit* pLorKhan = Unit::GetUnit(*me, instance->GetData64(DATA_LORKHAN)))
                                {
                                    pLorKhan->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                                    pLorKhan->SetCurrentFaction(16);
                                    pLorKhan->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                    pLorKhan->SetFullHealth();

                                    instance->SetData(DATA_LORKHAN, DONE);
                                }
                            }

                            if (instance->GetData(DATA_ZATH) == SPECIAL)
                            {
                                //Resurrect Zath
                                Unit* pZath = Unit::GetUnit(*me, instance->GetData64(DATA_ZATH));
                                if (pZath)
                                {
                                    pZath->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                                    pZath->SetCurrentFaction(16);
                                    pZath->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                    pZath->SetFullHealth();

                                    instance->SetData(DATA_ZATH, DONE);
                                }
                            }
                        }

                        CheckTimer = 5000;
                    } else CheckTimer -= diff;

                    if (!PhaseTwo && MortalCleaveTimer <= diff)
                    {
                        DoCastVictim(SPELL_MORTALCLEAVE);
                        MortalCleaveTimer = urand(15000, 20000);
                    } else MortalCleaveTimer -= diff;

                    if (!PhaseTwo && SilenceTimer <= diff)
                    {
                        DoCastVictim(SPELL_SILENCE);
                        SilenceTimer = urand(20000, 25000);
                    } else SilenceTimer -= diff;

                    if (!PhaseTwo && !WasDead && !HealthAbovePct(5))
                    {
                        me->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
                        me->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
                        me->RemoveAurasByType(SPELL_AURA_PERIODIC_LEECH);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        me->SetStandState(UNIT_STAND_STATE_SLEEP);
                        me->AttackStop();

                        if (instance)
                            instance->SetData(DATA_THEKAL, SPECIAL);

                        WasDead=true;
                    }

                    //Thekal will transform to Tiger if he died and was not resurrected after 10 seconds.
                    if (!PhaseTwo && WasDead)
                    {
                        if (ResurrectTimer <= diff)
                        {
                            DoCast(me, SPELL_TIGER_FORM);
                            me->SetObjectScale(2.00f);
                            me->SetStandState(UNIT_STAND_STATE_STAND);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            me->SetFullHealth();
                            const CreatureTemplate* cinfo = me->GetCreatureTemplate();
                            me->SetBaseWeaponDamage(BASE_ATTACK, MIN_DAMAGE, (cinfo->mindmg +((cinfo->mindmg/100) * 40)));
                            me->SetBaseWeaponDamage(BASE_ATTACK, MAX_DAMAGE, (cinfo->maxdmg +((cinfo->maxdmg/100) * 40)));
                            me->UpdateDamagePhysical(BASE_ATTACK);
                            DoResetThreat();
                            PhaseTwo = true;
                        }
						else ResurrectTimer -= diff;
                    }

                    if (me->IsFullHealth() && WasDead)
                        WasDead = false;

                    if (PhaseTwo)
                    {
                        if (ChargeTimer <= diff)
                        {
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                            {
                                DoCast(target, SPELL_CHARGE);
                                DoResetThreat();
                                AttackStart(target);
                            }
                            ChargeTimer = urand(15000, 22000);
                        }
						else ChargeTimer -= diff;

                        if (FrenzyTimer <= diff)
                        {
                            DoCast(me, SPELL_FRENZY);
                            FrenzyTimer = 30000;
                        }
						else FrenzyTimer -= diff;

                        if (ForcePunchTimer <= diff)
                        {
                            DoCastVictim(SPELL_SILENCE);
                            ForcePunchTimer = urand(16000, 21000);
                        }
						else ForcePunchTimer -= diff;

                        if (SummonTigersTimer <= diff)
                        {
                            DoCastVictim(SPELL_SUMMONTIGERS);
                            SummonTigersTimer = urand(10000, 14000);
                        }
						else SummonTigersTimer -= diff;

                        if (HealthBelowPct(11) && !Enraged)
                        {
                            DoCast(me, SPELL_ENRAGE);
                            Enraged = true;
                        }
                    }
                    DoMeleeAttackIfReady();

            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_thekalAI(creature);
        }
};

//Zealot Lor'Khan
class mob_zealot_lorkhan : public CreatureScript
{
    public:
        mob_zealot_lorkhan() : CreatureScript("mob_zealot_lorkhan") { }

        struct mob_zealot_lorkhanAI : public QuantumBasicAI
        {
            mob_zealot_lorkhanAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            uint32 ShieldTimer;
            uint32 BloodLustTimer;
            uint32 GreaterHealTimer;
            uint32 DisarmTimer;
            uint32 CheckTimer;

            bool FakeDeath;

            InstanceScript* instance;

            void Reset()
            {
                ShieldTimer = 1000;
                BloodLustTimer = 16000;
                GreaterHealTimer = 32000;
                DisarmTimer = 6000;
                CheckTimer = 10000;

                FakeDeath = false;

                if (instance)
                    instance->SetData(DATA_LORKHAN, NOT_STARTED);

                me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void EnterToBattle(Unit* /*who*/){}

            void UpdateAI (const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                //ShieldTimer
                if (ShieldTimer <= diff)
                {
                    DoCast(me, SPELL_SHIELD);
                    ShieldTimer = 61000;
                }
				else ShieldTimer -= diff;

                //BloodLustTimer
                if (BloodLustTimer <= diff)
                {
                    DoCast(me, SPELL_BLOODLUST);
                    BloodLustTimer = 20000+rand()%8000;
                }
				else BloodLustTimer -= diff;

                //Casting Greaterheal to Thekal or Zath if they are in meele range.
                if (GreaterHealTimer <= diff)
                {
                    if (instance)
                    {
                        Unit* pThekal = Unit::GetUnit(*me, instance->GetData64(DATA_THEKAL));
                        Unit* pZath = Unit::GetUnit(*me, instance->GetData64(DATA_ZATH));

                        if (!pThekal || !pZath)
                            return;

                        switch (urand(0, 1))
                        {
                            case 0:
                                if (me->IsWithinMeleeRange(pThekal))
                                    DoCast(pThekal, SPELL_GREATERHEAL);
                                break;
                            case 1:
                                if (me->IsWithinMeleeRange(pZath))
                                    DoCast(pZath, SPELL_GREATERHEAL);
                                break;
                        }
                    }

                    GreaterHealTimer = 15000+rand()%5000;
                }
				else GreaterHealTimer -= diff;

                //DisarmTimer
                if (DisarmTimer <= diff)
                {
                    DoCastVictim(SPELL_DISARM);
                    DisarmTimer = 15000+rand()%10000;
                }
				else DisarmTimer -= diff;

                //CheckTimer for the death of LorKhan and Zath.
                if (!FakeDeath && CheckTimer <= diff)
                {
                    if (instance)
                    {
                        if (instance->GetData(DATA_THEKAL) == SPECIAL)
                        {
                            //Resurrect Thekal
                            if (Unit* pThekal = Unit::GetUnit(*me, instance->GetData64(DATA_THEKAL)))
                            {
                                pThekal->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                                pThekal->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                pThekal->SetCurrentFaction(16);
                                pThekal->SetFullHealth();
                            }
                        }

                        if (instance->GetData(DATA_ZATH) == SPECIAL)
                        {
                            //Resurrect Zath
                            if (Unit* pZath = Unit::GetUnit(*me, instance->GetData64(DATA_ZATH)))
                            {
                                pZath->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                                pZath->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                pZath->SetCurrentFaction(16);
                                pZath->SetFullHealth();
                            }
                        }
                    }

                    CheckTimer = 5000;
                } else CheckTimer -= diff;

                if (!HealthAbovePct(5))
                {
                    me->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
                    me->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
                    me->RemoveAurasByType(SPELL_AURA_PERIODIC_LEECH);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->SetStandState(UNIT_STAND_STATE_SLEEP);
                    me->SetCurrentFaction(35);
                    me->AttackStop();

                    if (instance)
                        instance->SetData(DATA_LORKHAN, SPECIAL);

                    FakeDeath = true;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_zealot_lorkhanAI(creature);
        }
};

//Zealot Zath
class mob_zealot_zath : public CreatureScript
{
    public:
        mob_zealot_zath() : CreatureScript("mob_zealot_zath") { }

        struct mob_zealot_zathAI : public QuantumBasicAI
        {
            mob_zealot_zathAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            uint32 SweepingStrikesTimer;
            uint32 SinisterStrikeTimer;
            uint32 GougeTimer;
            uint32 KickTimer;
            uint32 BlindTimer;
            uint32 CheckTimer;

            bool FakeDeath;

            InstanceScript* instance;

            void Reset()
            {
                SweepingStrikesTimer = 13000;
                SinisterStrikeTimer = 8000;
                GougeTimer = 25000;
                KickTimer = 18000;
                BlindTimer = 5000;
                CheckTimer = 10000;

                FakeDeath = false;

                if (instance)
                    instance->SetData(DATA_ZATH, NOT_STARTED);

                me->SetStandState(UNIT_STAND_STATE_STAND);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void EnterToBattle(Unit* /*who*/){}

            void UpdateAI (const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                //SweepingStrikesTimer
                if (SweepingStrikesTimer <= diff)
                {
                    DoCastVictim(SPELL_SWEEPINGSTRIKES);
                    SweepingStrikesTimer = 22000+rand()%4000;
                }
				else SweepingStrikesTimer -= diff;

                // SinisterStrikeTimer
                if (SinisterStrikeTimer <= diff)
                {
                    DoCastVictim(SPELL_SINISTERSTRIKE);
                    SinisterStrikeTimer = 8000+rand()%8000;
                }
				else SinisterStrikeTimer -= diff;

                // GougeTimer
                if (GougeTimer <= diff)
                {
                    DoCastVictim(SPELL_GOUGE);

                    if (DoGetThreat(me->GetVictim()))
                        DoModifyThreatPercent(me->GetVictim(), -100);

                    GougeTimer = 17000+rand()%10000;
                }
				else GougeTimer -= diff;

                // KickTimer
                if (KickTimer <= diff)
                {
                    DoCastVictim(SPELL_KICK);
                    KickTimer = 15000+rand()%10000;
                }
				else KickTimer -= diff;

                // BlindTimer
                if (BlindTimer <= diff)
                {
                    DoCastVictim(SPELL_BLIND);
                    BlindTimer = 10000+rand()%10000;
                }
				else BlindTimer -= diff;

                // CheckTimer for the death of LorKhan and Zath.
                if (!FakeDeath && CheckTimer <= diff)
                {
                    if (instance)
                    {
                        if (instance->GetData(DATA_LORKHAN) == SPECIAL)
                        {
                            // Resurrect LorKhan
                            if (Unit* pLorKhan = Unit::GetUnit(*me, instance->GetData64(DATA_LORKHAN)))
                            {
                                pLorKhan->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                                pLorKhan->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                pLorKhan->SetCurrentFaction(16);
                                pLorKhan->SetFullHealth();
                            }
                        }

                        if (instance->GetData(DATA_THEKAL) == SPECIAL)
                        {
                            //Resurrect Thekal
                            if (Unit* pThekal = Unit::GetUnit(*me, instance->GetData64(DATA_THEKAL)))
                            {
                                pThekal->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                                pThekal->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                pThekal->SetCurrentFaction(16);
                                pThekal->SetFullHealth();
                            }
                        }
                    }

                    CheckTimer = 5000;
                }
				else CheckTimer -= diff;

                if (!HealthAbovePct(5))
                {
                    me->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
                    me->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
                    me->RemoveAurasByType(SPELL_AURA_PERIODIC_LEECH);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->SetStandState(UNIT_STAND_STATE_SLEEP);
                    me->SetCurrentFaction(35);
                    me->AttackStop();

                    if (instance)
                        instance->SetData(DATA_ZATH, SPECIAL);

                    FakeDeath = true;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_zealot_zathAI(creature);
        }
};

void AddSC_boss_thekal()
{
    new boss_thekal();
    new mob_zealot_lorkhan();
    new mob_zealot_zath();
}