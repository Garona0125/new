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
#include "the_eye.h"

enum Texts
{
    SAY_AGGRO      = -1550000,
    SAY_SLAY_1     = -1550001,
    SAY_SLAY_2     = -1550002,
    SAY_SLAY_3     = -1550003,
    SAY_DEATH      = -1550004,
    SAY_POUNDING_1 = -1550005,
    SAY_POUNDING_2 = -1550006,
};

enum Spells
{
    SPELL_POUNDING    = 34162,
    SPELL_ARCANE_ORB  = 34172,
    SPELL_KNOCK_AWAY  = 25778,
    SPELL_BERSERK     = 27680,
};

class boss_void_reaver : public CreatureScript
{
    public:
        boss_void_reaver() : CreatureScript("boss_void_reaver") { }

        struct boss_void_reaverAI : public QuantumBasicAI
        {
            boss_void_reaverAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 PoundingTimer;
            uint32 ArcaneOrbTimer;
            uint32 KnockAwayTimer;
            uint32 BerserkTimer;

            bool Enraged;

            void Reset()
            {
                PoundingTimer = 15000;
                ArcaneOrbTimer = 3000;
                KnockAwayTimer = 30000;
                BerserkTimer = 600000;

                Enraged = false;

				instance->SetData(DATA_VOID_REAVER, NOT_STARTED);
            }

            void KilledUnit(Unit* /*victim*/)
            {
                DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
            }

            void JustDied(Unit* /*killer*/)
            {
                DoSendQuantumText(SAY_DEATH, me);

				instance->SetData(DATA_VOID_REAVER, DONE);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                DoSendQuantumText(SAY_AGGRO, me);

				DoZoneInCombat();

				instance->SetData(DATA_VOID_REAVER, IN_PROGRESS);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (PoundingTimer <= diff)
                {
                    DoCast(SPELL_POUNDING);
                    DoSendQuantumText(RAND(SAY_POUNDING_1, SAY_POUNDING_2), me);
                    PoundingTimer = 15000;
                }
                else PoundingTimer -= diff;

                if (ArcaneOrbTimer <= diff)
                {
                    Unit* target = NULL;
                    std::list<HostileReference*> t_list = me->getThreatManager().getThreatList();
                    std::vector<Unit*> target_list;
                    for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                    {
                        target = Unit::GetUnit(*me, (*itr)->getUnitGuid());
                        if (!target)
                            continue;

                        if (target->GetTypeId() == TYPE_ID_PLAYER && target->IsAlive() && !target->IsWithinDist(me, 18, false))
                            target_list.push_back(target);
                        target = NULL;
                    }

                    if (!target_list.empty())
                        target = *(target_list.begin()+rand()%target_list.size());
                    else
                        target = me->GetVictim();

                    if (target)
                        me->CastSpell(target, SPELL_ARCANE_ORB, false, NULL, NULL, 0);
                    ArcaneOrbTimer = 3000;
                }
                else ArcaneOrbTimer -= diff;

                if (KnockAwayTimer <= diff)
                {
                    DoCastVictim(SPELL_KNOCK_AWAY);

                    if (DoGetThreat(me->GetVictim()))
                        DoModifyThreatPercent(me->GetVictim(), -25);

                    KnockAwayTimer = 30000;
                }
                else KnockAwayTimer -= diff;

                if (BerserkTimer < diff && !Enraged)
                {
                    DoCast(me, SPELL_BERSERK);
                    Enraged = true;
                }
                else BerserkTimer -= diff;

                DoMeleeAttackIfReady();

                EnterEvadeIfOutOfCombatArea(diff);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_void_reaverAI(creature);
        }
};

void AddSC_boss_void_reaver()
{
    new boss_void_reaver();
}