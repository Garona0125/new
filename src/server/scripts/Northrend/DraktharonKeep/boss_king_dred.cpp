/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
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
#include "SpellScript.h"
#include "QuantumCreature.h"
#include "drak_tharon_keep.h"

enum Spells
{
    SPELL_BELLOWING_ROAR   = 22686,
    SPELL_GRIEVOUS_BITE    = 48920,
    SPELL_MANGLING_SLASH   = 48873,
    SPELL_FEARSOME_ROAR_5N = 48849,
    SPELL_FEARSOME_ROAR_5H = 59422,
    SPELL_PIERCING_SLASH   = 48878,
    SPELL_RAPTOR_CALL      = 59416,
};

class boss_king_dred : public CreatureScript
{
    public:
        boss_king_dred() : CreatureScript("boss_king_dred") { }

        struct boss_king_dredAI : public QuantumBasicAI
        {
            boss_king_dredAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = me->GetInstanceScript();
            }

            uint32 BellowingRoarTimer;
            uint32 GrievousBiteTimer;
            uint32 ManglingSlashTimer;
            uint32 FearsomeRoarTimer;
            uint32 PiercingSlashTimer;
            uint32 RaptorCallTimer;

            uint8 RaptorsKilled;

            InstanceScript* instance;

            void Reset()
            {
				instance->SetData(DATA_DRED_EVENT, NOT_STARTED);

                BellowingRoarTimer = 33000;
                GrievousBiteTimer = 20000;
                ManglingSlashTimer = 18500;
                FearsomeRoarTimer = urand(10000, 20000);
                PiercingSlashTimer = 17000;
                RaptorCallTimer = urand(20000, 25000);
            }

            void EnterToBattle(Unit* /*who*/)
            {
				instance->SetData(DATA_DRED_EVENT, IN_PROGRESS);
            }

			void JustDied(Unit* /*who*/)
            {
				instance->SetData(DATA_DRED_EVENT, DONE);
            }

			void DoAction(const int32 action)
            {
				if (action == ACTION_RAPTOR_KILLED)
					++RaptorsKilled;
            }

            uint32 GetData(uint32 type)
            {
				if (type == DATA_KING_DRED)
					return RaptorsKilled;

                return 0;
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (BellowingRoarTimer <= diff)
                {
                    DoCastAOE(SPELL_BELLOWING_ROAR);
                    BellowingRoarTimer = 40000;
                }
                else BellowingRoarTimer -= diff;

                if (GrievousBiteTimer <= diff)
                {
                    DoCastVictim(SPELL_GRIEVOUS_BITE);
                    GrievousBiteTimer = 20000;
                }
                else GrievousBiteTimer -= diff;

                if (ManglingSlashTimer <= diff)
                {
                    DoCastVictim(SPELL_MANGLING_SLASH);
                    ManglingSlashTimer = 20000;
                }
                else ManglingSlashTimer -= diff;

                if (FearsomeRoarTimer <= diff)
                {
                    DoCastAOE(DUNGEON_MODE(SPELL_FEARSOME_ROAR_5N, SPELL_FEARSOME_ROAR_5H));
                    FearsomeRoarTimer = urand(16000, 18000);
                }
                else FearsomeRoarTimer -= diff;

                if (PiercingSlashTimer <= diff)
                {
                    DoCastVictim(SPELL_PIERCING_SLASH);
                    PiercingSlashTimer = 20000;
                }
                else PiercingSlashTimer -= diff;

                if (RaptorCallTimer <= diff)
                {
                    DoCast(SPELL_RAPTOR_CALL);

                    float x, y, z;

                    me->GetClosePoint(x, y, z, me->GetObjectSize() / 3, 10.0f);
                    me->SummonCreature(RAND(NPC_RAPTOR_1, NPC_RAPTOR_2), x, y, z, 0, TEMPSUMMON_DEAD_DESPAWN, 1000);

                    RaptorCallTimer = urand(20000, 25000);
                }
                else RaptorCallTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_king_dredAI(creature);
        }
};

class achievement_king_dred : public AchievementCriteriaScript
{
public:
	achievement_king_dred() : AchievementCriteriaScript("achievement_king_dred") { }

	bool OnCheck(Player* /*player*/, Unit* target)
	{
		if (!target)
			return false;

		if (Creature* Dred = target->ToCreature())
			if (Dred->AI()->GetData(DATA_KING_DRED) >= 6)
				return true;

		return false;
	}
};

void AddSC_boss_king_dred()
{
    new boss_king_dred();
    new achievement_king_dred();
}