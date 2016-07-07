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
#include "blood_furnace.h"

enum Texts
{
    SAY_AGGRO_1                 = -1542009,
    SAY_AGGRO_2                 = -1542010,
    SAY_AGGRO_3                 = -1542011,
    SAY_KILL_1                  = -1542012,
    SAY_KILL_2                  = -1542013,
    SAY_DIE                     = -1542014,
};

enum Spells
{
    SPELL_ACID_SPRAY            = 38153,
    SPELL_EXPLODING_BREAKER     = 30925,
    SPELL_KNOCKDOWN             = 20276,
    SPELL_DOMINATION            = 30923, // ???
};

class boss_the_maker : public CreatureScript
{
    public:
        boss_the_maker(): CreatureScript("boss_the_maker") { }

        struct boss_the_makerAI : public QuantumBasicAI
        {
            boss_the_makerAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 AcidSprayTimer;
            uint32 ExplodingBreakerTimer;
            uint32 DominationTimer;
            uint32 KnockdownTimer;

            void Reset()
            {
                AcidSprayTimer = 15000;
                ExplodingBreakerTimer = 6000;
                DominationTimer = 120000;
                KnockdownTimer = 10000;

                if (!instance)
                    return;

                instance->SetData(TYPE_THE_MAKER_EVENT, NOT_STARTED);
                instance->HandleGameObject(instance->GetData64(DATA_DOOR_2), true);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);

                if (!instance)
                    return;

                instance->SetData(TYPE_THE_MAKER_EVENT, IN_PROGRESS);
                instance->HandleGameObject(instance->GetData64(DATA_DOOR_2), false);
            }

            void KilledUnit(Unit* /*who*/)
            {
                DoSendQuantumText(RAND(SAY_KILL_1, SAY_KILL_2), me);
            }

            void JustDied(Unit* /*killer*/)
            {
                DoSendQuantumText(SAY_DIE, me);

                if (!instance)
                    return;

                instance->SetData(TYPE_THE_MAKER_EVENT, DONE);
                instance->HandleGameObject(instance->GetData64(DATA_DOOR_2), true);
                instance->HandleGameObject(instance->GetData64(DATA_DOOR_3), true);
			}

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

                if (AcidSprayTimer <= diff)
                {
                    DoCastVictim(SPELL_ACID_SPRAY);
                    AcidSprayTimer = 15000+rand()%8000;
                }
                else AcidSprayTimer -= diff;

                if (ExplodingBreakerTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, SPELL_EXPLODING_BREAKER);
						ExplodingBreakerTimer = 4000+rand()%8000;
					}
                }
                else ExplodingBreakerTimer -= diff;

                if (DominationTimer <= diff)
                {
					if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
					{
						DoCast(target, SPELL_DOMINATION);
						DominationTimer = 120000;
					}
				}
				else DominationTimer -= diff;

                if (KnockdownTimer <= diff)
                {
                    DoCastVictim(SPELL_KNOCKDOWN);
                    KnockdownTimer = 4000+rand()%8000;
                }
                else KnockdownTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_the_makerAI(creature);
        }
};

void AddSC_boss_the_maker()
{
    new boss_the_maker();
}