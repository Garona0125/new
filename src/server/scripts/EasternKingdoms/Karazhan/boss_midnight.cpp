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

enum Texts
{
	SAY_MIDNIGHT_KILL = -1532000,
	SAY_APPEAR_1      = -1532001,
	SAY_APPEAR_2      = -1532002,
	SAY_APPEAR_3      = -1532003,
	SAY_MOUNT         = -1532004,
	SAY_KILL1         = -1532005,
	SAY_KILL2         = -1532006,
	SAY_DISARMED      = -1532007,
	SAY_DEATH         = -1532008,
	SAY_RANDOM_1      = -1532009,
	SAY_RANDOM_2      = -1532010,
};

enum Spells
{
	SPELL_SHADOWCLEAVE         = 29832,
	SPELL_INTANGIBLE_PRESENCE  = 29833,
	SPELL_BERSERKER_CHARGE     = 26561,
};

enum Miscs
{
	MOUNTED_DISPLAY_ID         = 16040,
	SUMMON_ATTUMEN             = 15550,
};

class boss_attumen : public CreatureScript
{
public:
    boss_attumen() : CreatureScript("boss_attumen") { }

    struct boss_attumenAI : public QuantumBasicAI
    {
        boss_attumenAI(Creature* creature) : QuantumBasicAI(creature)
        {
            Phase = 1;
            CleaveTimer = urand(10000, 15000);
            CurseTimer = 30000;
            RandomYellTimer = urand(30000, 60000);
            ChargeTimer = 20000;
        }

        uint64 Midnight;
        uint8 Phase;
        uint32 CleaveTimer;
        uint32 CurseTimer;
        uint32 RandomYellTimer;
        uint32 ChargeTimer;
        uint32 ResetTimer;

        void Reset()
        {
			ResetTimer = 0;
		}

		void EnterEvadeMode()
		{
			QuantumBasicAI::EnterEvadeMode();
            ResetTimer = 2000;
        }

        void EnterToBattle(Unit* /*who*/) {}

        void KilledUnit(Unit* /*victim*/)
        {
			DoSendQuantumText(RAND(SAY_KILL1, SAY_KILL2), me);
        }

        void JustDied(Unit* /*victim*/)
        {
			DoSendQuantumText(SAY_DEATH, me);
			if (Unit* midnight = Unit::GetUnit(*me, Midnight))
			{
				midnight->Kill(midnight);
			}
        }

        void UpdateAI(const uint32 diff);

        void SpellHit(Unit* /*source*/, const SpellInfo* spell)
        {
            if (spell->Mechanic == MECHANIC_DISARM)
			{
				DoSendQuantumText(SAY_DISARMED, me);
			}
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_attumenAI (creature);
    }
};

class boss_midnight : public CreatureScript
{
public:
    boss_midnight() : CreatureScript("boss_midnight") { }

    struct boss_midnightAI : public QuantumBasicAI
    {
        boss_midnightAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint64 Attumen;
        uint8 Phase;
        uint32 MountTimer;

        void Reset()
        {
            Phase = 1;
            Attumen = 0;
            MountTimer = 0;

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetVisible(true);
        }

        void EnterToBattle(Unit* /*who*/) {}

        void KilledUnit(Unit* /*victim*/)
        {
            if (Phase == 2)
            {
                if (Unit* unit = Unit::GetUnit(*me, Attumen))
					DoSendQuantumText(SAY_MIDNIGHT_KILL, unit);
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (Phase == 1 && HealthBelowPct(95))
            {
                Phase = 2;

				if (Creature* attumen = me->SummonCreature(SUMMON_ATTUMEN, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 30000))
                {
					Attumen = attumen->GetGUID();
					attumen->AI()->AttackStart(me->GetVictim());
					SetMidnight(attumen, me->GetGUID());
					DoSendQuantumText(RAND(SAY_APPEAR_1, SAY_APPEAR_2, SAY_APPEAR_3), attumen);
                }
            }
            else if (Phase == 2 && HealthBelowPct(25))
            {
                if (Unit* pAttumen = Unit::GetUnit(*me, Attumen))
                    Mount(pAttumen);
            }
            else if (Phase == 3)
            {
                if (MountTimer)
                {
                    if (MountTimer <= diff)
                    {
                        MountTimer = 0;
                        me->SetVisible(false);
                        me->GetMotionMaster()->MoveIdle();
                        if (Unit* pAttumen = Unit::GetUnit(*me, Attumen))
                        {
                            pAttumen->SetDisplayId(MOUNTED_DISPLAY_ID);
                            pAttumen->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            if (pAttumen->GetVictim())
                            {
                                pAttumen->GetMotionMaster()->MoveChase(pAttumen->GetVictim());
                                pAttumen->SetTarget(pAttumen->GetVictim()->GetGUID());
                            }
                            pAttumen->SetObjectScale(1.0f);
							pAttumen->SetHealth(pAttumen->GetMaxHealth());
                        }
                    }
					else MountTimer -= diff;
                }
            }

            if (Phase != 3)
                DoMeleeAttackIfReady();
        }

        void Mount(Unit* pAttumen)
        {
			DoSendQuantumText(SAY_MOUNT, pAttumen);
            Phase = 3;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pAttumen->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            float angle = me->GetAngle(pAttumen);
            float distance = me->GetDistance2d(pAttumen);
            float newX = me->GetPositionX() + cos(angle)*(distance/2);
            float newY = me->GetPositionY() + sin(angle)*(distance/2);
            float newZ = 50;
            //me->Relocate(newX, newY, newZ, angle);
            //me->SendMonsterMove(newX, newY, newZ, 0, true, 1000);
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MovePoint(0, newX, newY, newZ);
            distance += 10;
            newX = me->GetPositionX() + cos(angle)*(distance/2);
            newY = me->GetPositionY() + sin(angle)*(distance/2);
            pAttumen->GetMotionMaster()->Clear();
            pAttumen->GetMotionMaster()->MovePoint(0, newX, newY, newZ);
            //pAttumen->Relocate(newX, newY, newZ, -angle);
            //pAttumen->SendMonsterMove(newX, newY, newZ, 0, true, 1000);
            MountTimer = 1000;
        }

        void SetMidnight(Creature* pAttumen, uint64 value)
        {
            CAST_AI(boss_attumen::boss_attumenAI, pAttumen->AI())->Midnight = value;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_midnightAI(creature);
    }
};

void boss_attumen::boss_attumenAI::UpdateAI(const uint32 diff)
{
    if (ResetTimer)
    {
        if (ResetTimer <= diff)
        {
            ResetTimer = 0;
            Unit* pMidnight = Unit::GetUnit(*me, Midnight);
            if (pMidnight)
            {
                pMidnight->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                pMidnight->SetVisible(true);
            }
            Midnight = 0;
            me->SetVisible(false);
            me->DealDamage(me,me->GetHealth());
        }
    }
	else ResetTimer -= diff;

    //Return since we have no target
    if (!UpdateVictim())
        return;

    if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE))
        return;

    if (CleaveTimer <= diff)
    {
        DoCastVictim(SPELL_SHADOWCLEAVE);
        CleaveTimer = urand(10000, 15000);
    }
	else CleaveTimer -= diff;

    if (CurseTimer <= diff)
    {
        DoCastVictim(SPELL_INTANGIBLE_PRESENCE);
        CurseTimer = 30000;
    }
	else CurseTimer -= diff;

    if (RandomYellTimer <= diff)
    {
		DoSendQuantumText(RAND(SAY_RANDOM_1, SAY_RANDOM_2), me);
        RandomYellTimer = urand(30000, 60000);
    }
	else RandomYellTimer -= diff;

    if (me->GetUInt32Value(UNIT_FIELD_DISPLAY_ID) == MOUNTED_DISPLAY_ID)
    {
        if (ChargeTimer <= diff)
        {
            Unit* target = NULL;
            std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
            std::vector<Unit*> target_list;
            for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
            {
                target = Unit::GetUnit(*me, (*itr)->getUnitGuid());
                if (target && !target->IsWithinDist(me, ATTACK_DISTANCE, false))
				{
                    target_list.push_back(target);
				}
                target = NULL;
            }
            if (!target_list.empty())
			{
                target = *(target_list.begin()+rand()%target_list.size());
			}

            DoCast(target, SPELL_BERSERKER_CHARGE);
            ChargeTimer = 20000;
        }
		else ChargeTimer -= diff;
    }
    else
    {
        if (HealthBelowPct(25))
        {
            Creature* pMidnight = Unit::GetCreature(*me, Midnight);
            if (pMidnight && pMidnight->GetTypeId() == TYPE_ID_UNIT)
            {
                CAST_AI(boss_midnight::boss_midnightAI, (pMidnight->AI()))->Mount(me);
                me->SetHealth(me->GetMaxHealth());
                DoResetThreat();
            }
        }
    }
    DoMeleeAttackIfReady();
}

void AddSC_boss_attumen()
{
    new boss_attumen();
    new boss_midnight();
}