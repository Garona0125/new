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
#include "scarlet_monastery.h"

enum Texts
{
	// Mograine says
	SAY_MO_AGGRO                 = 0,
	SAY_MO_KILL                  = 1,
	SAY_MO_RESSURECTED           = 2,
	// Whitemane says
	SAY_WH_INTRO                 = 0,
	SAY_WH_KILL                  = 1,
	SAY_WH_RESSURECT             = 2,
};

enum Spells
{
	//Mograine Spells
    SPELL_CRUSADER_STRIKE        = 14518,
    SPELL_HAMMER_OF_JUSTICE      = 5589,
    SPELL_LAY_ON_HANDS           = 9257,
    SPELL_RETRIBUTION_AURA       = 8990,
    //Whitemanes Spells
    SPELL_DEEP_SLEEP             = 9256,
    SPELL_SCARLET_RESURRECTION   = 9232,
    SPELL_DOMINATE_MIND          = 14515,
    SPELL_HOLY_SMITE             = 9481,
    SPELL_HEAL                   = 12039,
    SPELL_POWER_WORD_SHIELD      = 22187,
};

class boss_scarlet_commander_mograine : public CreatureScript
{
public:
    boss_scarlet_commander_mograine() : CreatureScript("boss_scarlet_commander_mograine") { }

    struct boss_scarlet_commander_mograineAI : public QuantumBasicAI
    {
        boss_scarlet_commander_mograineAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 CrusaderStrikeTimer;
        uint32 HammerOfJusticeTimer;

        bool HasDied;
        bool Heal;
        bool FakeDeath;

        void Reset()
        {
            CrusaderStrikeTimer = 10000;
            HammerOfJusticeTimer = 10000;

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            me->SetStandState(UNIT_STAND_STATE_STAND);

            if (instance)
                if (me->IsAlive())
                    instance->SetData(TYPE_MOGRAINE_AND_WHITE_EVENT, NOT_STARTED);

            HasDied = false;
            Heal = false;
            FakeDeath = false;
        }

        void JustReachedHome()
        {
            if (instance)
            {
                if (instance->GetData(TYPE_MOGRAINE_AND_WHITE_EVENT) != NOT_STARTED)
                    instance->SetData(TYPE_MOGRAINE_AND_WHITE_EVENT, FAIL);
            }
        }

        void EnterToBattle(Unit* /*who*/)
        {
            Talk(SAY_MO_AGGRO);
            DoCast(me, SPELL_RETRIBUTION_AURA);

            me->CallForHelp(VISIBLE_RANGE);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            Talk(SAY_MO_KILL);
        }

        void DamageTaken(Unit* /*doneBy*/, uint32 &damage)
        {
            if (damage < me->GetHealth() || HasDied || FakeDeath)
                return;

            if (!instance)
                return;

            if (Unit* Whitemane = Unit::GetUnit(*me, instance->GetData64(DATA_WHITEMANE)))
            {
                instance->SetData(TYPE_MOGRAINE_AND_WHITE_EVENT, IN_PROGRESS);

                Whitemane->GetMotionMaster()->MovePoint(1, 1163.113370f, 1398.856812f, 32.527786f);
                me->GetMotionMaster()->MovementExpired();
                me->GetMotionMaster()->MoveIdle();
                me->SetHealth(0);

                if (me->IsNonMeleeSpellCasted(false))
                    me->InterruptNonMeleeSpells(false);

                me->ClearComboPointHolders();
                me->RemoveAllAuras();
                me->ClearAllReactives();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetStandState(UNIT_STAND_STATE_DEAD);

                HasDied = true;
                FakeDeath = true;
                damage = 0;
            }
        }

        void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_SCARLET_RESURRECTION)
            {
                Talk(SAY_MO_RESSURECTED);
                FakeDeath = false;

                if (instance)
                    instance->SetData(TYPE_MOGRAINE_AND_WHITE_EVENT, SPECIAL);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (HasDied && !Heal && instance && instance->GetData(TYPE_MOGRAINE_AND_WHITE_EVENT) == SPECIAL)
            {
                if (Unit* Whitemane = Unit::GetUnit(*me, instance->GetData64(DATA_WHITEMANE)))
                {
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->SetStandState(UNIT_STAND_STATE_STAND);
                    DoCast(Whitemane, SPELL_LAY_ON_HANDS);

                    CrusaderStrikeTimer = 10000;
                    HammerOfJusticeTimer = 10000;

                    if (me->GetVictim())
                        me->GetMotionMaster()->MoveChase(me->GetVictim());

                    Heal = true;
                }
            }

            if (FakeDeath)
                return;

            if (CrusaderStrikeTimer <= diff)
            {
                DoCastVictim(SPELL_CRUSADER_STRIKE);
                CrusaderStrikeTimer = 10000;
            }
			else CrusaderStrikeTimer -= diff;

            if (HammerOfJusticeTimer <= diff)
            {
                DoCastVictim(SPELL_HAMMER_OF_JUSTICE);
                HammerOfJusticeTimer = 60000;
            }
			else HammerOfJusticeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_scarlet_commander_mograineAI(creature);
    }
};

class boss_high_inquisitor_whitemane : public CreatureScript
{
public:
    boss_high_inquisitor_whitemane() : CreatureScript("boss_high_inquisitor_whitemane") { }

    struct boss_high_inquisitor_whitemaneAI : public QuantumBasicAI
    {
        boss_high_inquisitor_whitemaneAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 HealTimer;
        uint32 PowerWordShieldTimer;
        uint32 HolySmiteTimer;
        uint32 WaitTimer;

        bool CanResurrectCheck;
        bool CanResurrect;

        void Reset()
        {
            WaitTimer = 7000;
            HealTimer = 10000;
            PowerWordShieldTimer = 15000;
            HolySmiteTimer = 6000;

            CanResurrectCheck = false;
            CanResurrect = false;

            if (instance)
			{
                if (me->IsAlive())
				{
                    instance->SetData(TYPE_MOGRAINE_AND_WHITE_EVENT, NOT_STARTED);
				}
			}
        }

        void AttackStart(Unit* who)
        {
            if (instance && instance->GetData(TYPE_MOGRAINE_AND_WHITE_EVENT) == NOT_STARTED)
                return;

            QuantumBasicAI::AttackStart(who);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            Talk(SAY_WH_INTRO);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            Talk(SAY_WH_KILL);
        }

		void DamageTaken(Unit* /*attacker*/, uint32& damage)
		{
			if (!CanResurrectCheck && damage >= me->GetHealth())
			{
				damage = me->GetHealth() - 1;
			}
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (CanResurrect)
            {
                if (instance && WaitTimer <= diff)
                {
                    if (Unit* Mograine = Unit::GetUnit(*me, instance->GetData64(DATA_MOGRAINE)))
                    {
                        DoCast(Mograine, SPELL_SCARLET_RESURRECTION);
                        Talk(SAY_WH_RESSURECT);
                        CanResurrect = false;
                    }
                }
                else WaitTimer -= diff;
            }

            if (!CanResurrectCheck && !HealthAbovePct(50))
            {
                if (me->IsNonMeleeSpellCasted(false))
				{
                    me->InterruptNonMeleeSpells(false);
				}

                DoCastVictim(SPELL_DEEP_SLEEP);
                CanResurrectCheck = true;
                CanResurrect = true;
                return;
            }

            if (CanResurrect)
                return;

            if (HealTimer <= diff)
            {
                Creature* target = NULL;

                if (!HealthAbovePct(75))
                    target = me;

                if (instance)
                {
                    if (Creature* pMograine = Unit::GetCreature(*me, instance->GetData64(DATA_MOGRAINE)))
                    {
                        if (CanResurrectCheck && pMograine->IsAlive() && !pMograine->HealthAbovePct(75))
						{
                            target = pMograine;
						}
                    }
                }

                if (target)
                    DoCast(target, SPELL_HEAL);

                HealTimer = 13000;
            }
			else HealTimer -= diff;

            if (PowerWordShieldTimer <= diff)
            {
                DoCast(me, SPELL_POWER_WORD_SHIELD);
                PowerWordShieldTimer = 15000;
            }
			else PowerWordShieldTimer -= diff;

            if (HolySmiteTimer <= diff)
            {
                DoCastVictim(SPELL_HOLY_SMITE);
                HolySmiteTimer = 6000;
            }
			else HolySmiteTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_high_inquisitor_whitemaneAI(creature);
    }
};

void AddSC_boss_mograine_and_whitemane()
{
    new boss_scarlet_commander_mograine();
    new boss_high_inquisitor_whitemane();
}