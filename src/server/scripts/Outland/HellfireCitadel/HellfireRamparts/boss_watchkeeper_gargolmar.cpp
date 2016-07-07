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
#include "hellfire_ramparts.h"

enum Text
{
    SAY_TAUNT    = -1543000,
    SAY_HEAL     = -1543001,
    SAY_SURGE    = -1543002,
    SAY_AGGRO_1  = -1543003,
    SAY_AGGRO_2  = -1543004,
    SAY_AGGRO_3  = -1543005,
    SAY_KILL_1   = -1543006,
    SAY_KILL_2   = -1543007,
    SAY_DIE      = -1543008,
};

enum Spells
{
    SPELL_MORTAL_WOUND_5N  = 30641,
    SPELL_MORTAL_WOUND_5H  = 36814,
    SPELL_SURGE            = 34645,
    SPELL_RETALIATION      = 22857,
};

class boss_watchkeeper_gargolmar : public CreatureScript
{
    public:
        boss_watchkeeper_gargolmar() : CreatureScript("boss_watchkeeper_gargolmar") { }

        struct boss_watchkeeper_gargolmarAI : public QuantumBasicAI
        {
            boss_watchkeeper_gargolmarAI(Creature* creature) : QuantumBasicAI(creature) {}

            uint32 SurgeTimer;
            uint32 MortalWoundTimer;
            uint32 RetaliationTimer;

            bool HasTaunted;
            bool YelledForHeal;

            void Reset()
            {
                SurgeTimer = 5000;
                MortalWoundTimer = 4000;
                RetaliationTimer = 0;

                HasTaunted = false;
                YelledForHeal = false;

				RespawnHealers();

				DoCast(me, SPELL_UNIT_DETECTION_ALL);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

			void RespawnHealers()
			{
				std::list<Creature*> TrashList;
				me->GetCreatureListWithEntryInGrid(TrashList, NPC_HELLFIRE_WATCHER, 850.0f);

				if (!TrashList.empty())
				{
					for (std::list<Creature*>::const_iterator itr = TrashList.begin(); itr != TrashList.end(); ++itr)
					{
						if (Creature* watcher = *itr)
							watcher->Respawn();
					}
				}
			}

            void EnterToBattle(Unit* /*who*/)
            {
                DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);
            }

            void MoveInLineOfSight(Unit* who)
            {
                if (!me->GetVictim() && me->CanCreatureAttack(who))
                {
                    if (!me->CanFly() && me->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE)
                        return;

                    float attackRadius = me->GetAttackDistance(who);
                    if (me->IsWithinDistInMap(who, attackRadius) && me->IsWithinLOSInMap(who))
                    {
                        //who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                        AttackStart(who);
                    }
                    else if (!HasTaunted && me->IsWithinDistInMap(who, 60.0f))
                    {
                        DoSendQuantumText(SAY_TAUNT, me);
                        HasTaunted = true;
                    }
                }
            }

            void KilledUnit(Unit* /*victim*/)
            {
                DoSendQuantumText(RAND(SAY_KILL_1, SAY_KILL_2), me);
            }

            void JustDied(Unit* /*Killer*/)
            {
                DoSendQuantumText(SAY_DIE, me);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (MortalWoundTimer <= diff)
                {
                    DoCastVictim(DUNGEON_MODE(SPELL_MORTAL_WOUND_5N, SPELL_MORTAL_WOUND_5H));
                    MortalWoundTimer = 5000+rand()%8000;
                }
                else MortalWoundTimer -= diff;

                if (SurgeTimer <= diff)
                {
                    DoSendQuantumText(SAY_SURGE, me);

                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, SPELL_SURGE);
						SurgeTimer = 5000+rand()%8000;
					}
                }
                else SurgeTimer -= diff;

                if (HealthBelowPct(20))
                {
                    if (RetaliationTimer <= diff)
                    {
                        DoCast(me, SPELL_RETALIATION);
                        RetaliationTimer = 30000;
                    }
                    else RetaliationTimer -= diff;
                }

                if (!YelledForHeal)
                {
                    if (HealthBelowPct(50))
                    {
                        DoSendQuantumText(SAY_HEAL, me);
                        YelledForHeal = true;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_watchkeeper_gargolmarAI(creature);
        }
};

void AddSC_boss_watchkeeper_gargolmar()
{
    new boss_watchkeeper_gargolmar();
}