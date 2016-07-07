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
#include "SpellScript.h"
#include "shattered_halls.h"

enum Says
{
    SAY_AGGRO1       = -1540042,
    SAY_AGGRO2       = -1540043,
    SAY_AGGRO3       = -1540044,
    SAY_SLAY1        = -1540045,
    SAY_SLAY2        = -1540046,
    SAY_DEATH        = -1540047,
};

enum Spells
{
     SPELL_BLADE_DANCE = 30739,
     SPELL_CHARGE      = 25821,
};

#define TARGET_NUM                      5

float AssassEntrance[3] = {275.136f, -84.29f, 2.3f}; // y -8
float AssassExit[3] = {184.233f, -84.29f, 2.3f}; // y -8
float AddsEntrance[3] = {306.036f, -84.29f, 1.93f};

class boss_warchief_kargath_bladefist : public CreatureScript
{
    public:
        boss_warchief_kargath_bladefist() : CreatureScript("boss_warchief_kargath_bladefist") { }

        struct boss_warchief_kargath_bladefistAI : public QuantumBasicAI
        {
            boss_warchief_kargath_bladefistAI(Creature* creature) : QuantumBasicAI(creature){}

            std::vector<uint64> adds;
            std::vector<uint64> assassins;

            uint32 ChargeTimer;
            uint32 BladeDanceTimer;
            uint32 SummonAssistantTimer;
            uint32 ResetcheckTimer;
            uint32 WaitTimer;

            uint32 AssassinsTimer;

            uint32 summoned;
            bool InBlade;

            uint32 target_num;

            void Reset()
            {
				DoCast(me, SPELL_UNIT_DETECTION_ALL);

                removeAdds();

                me->SetSpeed(MOVE_RUN, 2);
                me->SetWalk(false);

                summoned = 2;
                InBlade = false;
                WaitTimer = 0;

                ChargeTimer = 0;
                BladeDanceTimer = 45000;
                SummonAssistantTimer = 30000;
                AssassinsTimer = 5000;
                ResetcheckTimer = 5000;
            }

            void EnterToBattle(Unit* /*who*/)
            {
                DoSendQuantumText(RAND(SAY_AGGRO1, SAY_AGGRO2, SAY_AGGRO3), me);
            }

            void JustSummoned(Creature* summoned)
            {
                switch (summoned->GetEntry())
                {
                    case NPC_HEARTHEN_GUARD:
                    case NPC_SHARPSHOOTER_GUARD:
                    case NPC_REAVER_GUARD:
                        summoned->AI()->AttackStart(SelectTarget(TARGET_RANDOM, 0));
                        adds.push_back(summoned->GetGUID());
                        break;
                    case NPC_SHATTERED_ASSASSIN:
                        assassins.push_back(summoned->GetGUID());
                        break;
                }
            }

            void KilledUnit(Unit* victim)
            {
                if (victim->GetTypeId() == TYPE_ID_PLAYER)
                    DoSendQuantumText(RAND(SAY_SLAY1, SAY_SLAY2), me);
            }

            void JustDied(Unit* /*killer*/)
            {
                DoSendQuantumText(SAY_DEATH, me);
                removeAdds();
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (InBlade)
                {
                    if (type != POINT_MOTION_TYPE)
                        return;

                    if (id != 1)
                        return;

                    if (target_num > 0) // to prevent loops
                    {
                        WaitTimer = 1;
                        DoCast(me, SPELL_BLADE_DANCE, true);
                        target_num--;
                    }
                }
            }

            void removeAdds()
            {
                for (std::vector<uint64>::const_iterator itr = adds.begin(); itr!= adds.end(); ++itr)
                {
                    Unit* temp = Unit::GetUnit(*me, *itr);
                    if (temp && temp->IsAlive())
                    {
                        (*temp).GetMotionMaster()->Clear(true);
                        me->DealDamage(temp, temp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        CAST_CRE(temp)->RemoveCorpse();
                    }
                }
                adds.clear();

                for (std::vector<uint64>::const_iterator itr = assassins.begin(); itr!= assassins.end(); ++itr)
                {
                    Unit* temp = Unit::GetUnit(*me, *itr);
                    if (temp && temp->IsAlive())
                    {
                        (*temp).GetMotionMaster()->Clear(true);
                        me->DealDamage(temp, temp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        CAST_CRE(temp)->RemoveCorpse();
                    }
                }
                assassins.clear();
            }

            void SpawnAssassin()
            {
                me->SummonCreature(NPC_SHATTERED_ASSASSIN, AssassEntrance[0], AssassEntrance[1]+8, AssassEntrance[2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                me->SummonCreature(NPC_SHATTERED_ASSASSIN, AssassEntrance[0], AssassEntrance[1]-8, AssassEntrance[2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                me->SummonCreature(NPC_SHATTERED_ASSASSIN, AssassExit[0], AssassExit[1]+8, AssassExit[2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                me->SummonCreature(NPC_SHATTERED_ASSASSIN, AssassExit[0], AssassExit[1]-8, AssassExit[2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (AssassinsTimer)
                {
                    if (AssassinsTimer <= diff)
                    {
                        SpawnAssassin();
                        AssassinsTimer = 0;
                    }
                    else AssassinsTimer -= diff;
                }

                if (InBlade)
                {
                    if (WaitTimer)
                    {
                        if (WaitTimer <= diff)
                        {
                            if (target_num <= 0)
                            {
                                InBlade = false;
                                me->SetSpeed(MOVE_RUN, 2);
                                me->GetMotionMaster()->MoveChase(me->GetVictim());
                                BladeDanceTimer = 30000;
                                WaitTimer = 0;
                                if (IsHeroic())
                                    ChargeTimer = 5000;
                            }
                            else
                            {
                                float x, y, randx, randy;
                                randx = 0.0f + rand()%40;
                                randy = 0.0f + rand()%40;
                                x = 210+ randx;
                                y = -60- randy;
                                me->GetMotionMaster()->MovePoint(1, x, y, me->GetPositionZ());
                                WaitTimer = 0;
                            }
                        }
                        else WaitTimer -= diff;
                    }
                }
                else
                {
                    if (BladeDanceTimer)
                    {
                        if (BladeDanceTimer <= diff)
                        {
                            target_num = TARGET_NUM;
                            WaitTimer = 1;
                            InBlade = true;
                            BladeDanceTimer = 0;
                            me->SetSpeed(MOVE_RUN, 4);
                            return;
                        }
                        else BladeDanceTimer -= diff;
                    }

                    if (ChargeTimer)
                    {
                        if (ChargeTimer <= diff)
                        {
                            DoCast(SelectTarget(TARGET_RANDOM, 0), SPELL_CHARGE);
                            ChargeTimer = 0;
                        }
                        else ChargeTimer -= diff;
                    }

                    if (SummonAssistantTimer <= diff)
                    {
                        for (uint8 i = 0; i < summoned; ++i)
                        {
                            switch (urand(0, 2))
                            {
                                case 0:
                                    me->SummonCreature(NPC_HEARTHEN_GUARD, AddsEntrance[0], AddsEntrance[1], AddsEntrance[2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                                    break;
                                case 1:
                                    me->SummonCreature(NPC_SHARPSHOOTER_GUARD, AddsEntrance[0], AddsEntrance[1], AddsEntrance[2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                                    break;
                                case 2:
                                    me->SummonCreature(NPC_REAVER_GUARD, AddsEntrance[0], AddsEntrance[1], AddsEntrance[2], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000);
                                    break;
                            }
                        }
                        if (urand(0, 9) < 2)
                            ++summoned;
                        SummonAssistantTimer = urand(25000, 35000);
                    }
                    else SummonAssistantTimer -= diff;

                    DoMeleeAttackIfReady();
                }

                if (ResetcheckTimer <= diff)
                {
                    uint32 tempx = uint32(me->GetPositionX());
                    if (tempx > 255 || tempx < 205)
                    {
                        EnterEvadeMode();
                        return;
                    }
                    ResetcheckTimer = 5000;
                }
                else ResetcheckTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_warchief_kargath_bladefistAI(creature);
        }
};

void AddSC_boss_warchief_kargath_bladefist()
{
    new boss_warchief_kargath_bladefist();
}