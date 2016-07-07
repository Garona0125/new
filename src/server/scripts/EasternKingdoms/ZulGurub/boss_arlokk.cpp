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
SDName: Boss_Arlokk
SD%Complete: 95
SDComment: Wrong cleave and red aura is missing.
SDCategory: Zul'Gurub
EndScriptData */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "zulgurub.h"

enum Yells
{
	SAY_AGGRO                   = 0,
	SAY_FEAST_PANTHER           = 1,
	SAY_DEATH                   = 2,
};

enum Spells
{
    SPELL_SHADOW_WORD_PAIN      = 23952,
    SPELL_GOUGE                 = 24698,
    SPELL_MARK                  = 24210,
    SPELL_CLEAVE                = 26350,                    //Perhaps not right. Not a red aura...
    SPELL_PANTHER_TRANSFORM     = 24190,
};

enum AlrokkModels
{
    MODEL_ID_NORMAL             = 15218,
    MODEL_ID_PANTHER            = 15215,

    NPC_ZULIAN_PROWLER          = 15101,
};

class boss_arlokk : public CreatureScript
{
    public:
        boss_arlokk() : CreatureScript("boss_arlokk"){}

        struct boss_arlokkAI : public QuantumBasicAI
        {
            boss_arlokkAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 ShadowWordPainTimer;
            uint32 GougeTimer;
            uint32 MarkTimer;
            uint32 CleaveTimer;
            uint32 VanishTimer;
            uint32 VisibleTimer;

            uint32 SummonTimer;
            uint32 SummonCount;

            Unit* m_pMarkedTarget;
            uint64 MarkedTargetGUID;

            bool IsPhaseTwo;
            bool IsVanished;

            void Reset()
            {
                ShadowWordPainTimer = 8000;
                GougeTimer = 14000;
                MarkTimer = 35000;
                CleaveTimer = 4000;
                VanishTimer = 60000;
                VisibleTimer = 6000;

                SummonTimer = 5000;
                SummonCount = 0;

                IsPhaseTwo = false;
                IsVanished = false;

                MarkedTargetGUID = 0;

                me->SetDisplayId(MODEL_ID_NORMAL);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                Talk(SAY_AGGRO);
            }

            void JustReachedHome()
            {
                if (instance)
                    instance->SetData(DATA_ARLOKK, NOT_STARTED);

                //we should be summoned, so despawn
                me->DespawnAfterAction();
            }

            void JustDied(Unit* /*killer*/)
            {
                Talk(SAY_DEATH);

                me->SetDisplayId(MODEL_ID_NORMAL);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                if (instance)
                    instance->SetData(DATA_ARLOKK, DONE);
            }

            void DoSummonPhanters()
            {
				if (MarkedTargetGUID)
					Talk(SAY_FEAST_PANTHER, MarkedTargetGUID);

                me->SummonCreature(NPC_ZULIAN_PROWLER, -11532.7998f, -1649.6734f, 41.4800f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
                me->SummonCreature(NPC_ZULIAN_PROWLER, -11532.9970f, -1606.4840f, 41.2979f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 15000);
            }

            void JustSummoned(Creature* summoned)
            {
                if (Unit* pMarkedTarget = Unit::GetUnit(*me, MarkedTargetGUID))
                    summoned->AI()->AttackStart(pMarkedTarget);

                ++SummonCount;
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (!IsPhaseTwo)
                {
                    if (ShadowWordPainTimer <= diff)
                    {
                        DoCastVictim(SPELL_SHADOW_WORD_PAIN);
                        ShadowWordPainTimer = 15000;
                    }
                    else ShadowWordPainTimer -= diff;

                    if (MarkTimer <= diff)
                    {
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_MARK);
							MarkedTargetGUID = target->GetGUID();
							MarkTimer = 15000;
						}
                        else sLog->OutErrorConsole("QUANTUMCORE SCRIPTS: boss_arlokk could not accuire target.");
					}
                    else MarkTimer -= diff;
                }
                else
                {
                    if (CleaveTimer <= diff)
                    {
                        DoCastVictim(SPELL_CLEAVE);
                        CleaveTimer = 16000;
                    }
                    else CleaveTimer -= diff;

                    if (GougeTimer <= diff)
                    {
                        DoCastVictim(SPELL_GOUGE);
                        DoModifyThreatPercent(me->GetVictim(), -80);
                        GougeTimer = 17000+rand()%10000;
                    }
                    else GougeTimer -= diff;
                }

                if (SummonCount <= 30)
                {
                    if (SummonTimer <= diff)
                    {
                        DoSummonPhanters();
                        SummonTimer = 5000;
                    }
                    else SummonTimer -= diff;
                }

                if (VanishTimer <= diff)
                {
                    me->SetDisplayId(MODEL_ID_INVISIBLE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                    me->AttackStop();
                    DoResetThreat();

                    IsVanished = true;

                    VanishTimer = 45000;
                    VisibleTimer = 6000;
                }
                else VanishTimer -= diff;

                if (IsVanished)
                {
                    if (VisibleTimer <= diff)
                    {
                        //The Panther Model
                        me->SetDisplayId(MODEL_ID_PANTHER);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                        const CreatureTemplate* cinfo = me->GetCreatureTemplate();
                        me->SetBaseWeaponDamage(BASE_ATTACK, MIN_DAMAGE, (cinfo->mindmg +((cinfo->mindmg/100) * 35)));
                        me->SetBaseWeaponDamage(BASE_ATTACK, MAX_DAMAGE, (cinfo->maxdmg +((cinfo->maxdmg/100) * 35)));
                        me->UpdateDamagePhysical(BASE_ATTACK);

                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                            AttackStart(target);

                        IsPhaseTwo = true;
                        IsVanished = false;
                    }
                    else VisibleTimer -= diff;
                }
                else DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_arlokkAI(creature);
        }
};

class go_gong_of_bethekk : public GameObjectScript
{
    public:
        go_gong_of_bethekk() : GameObjectScript("go_gong_of_bethekk"){}

        bool OnGossipHello(Player* /*player*/, GameObject* go)
        {
            if (InstanceScript* instance = go->GetInstanceScript())
            {
                if (instance->GetData(DATA_ARLOKK) == DONE || instance->GetData(DATA_ARLOKK) == IN_PROGRESS)
                    return true;

                instance->SetData(DATA_ARLOKK, IN_PROGRESS);
                return true;
            }
            return true;
        }
};

void AddSC_boss_arlokk()
{
    new boss_arlokk();
    new go_gong_of_bethekk();
}