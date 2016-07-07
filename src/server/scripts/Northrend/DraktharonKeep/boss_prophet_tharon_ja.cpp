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
    SPELL_CURSE_OF_LIFE_5N    = 49527,
    SPELL_CURSE_OF_LIFE_5H    = 59972,
    SPELL_RAIN_OF_FIRE_5N     = 49518,
    SPELL_RAIN_OF_FIRE_5H     = 59971,
    SPELL_SHADOW_VOLLEY_5N    = 49528,
    SPELL_SHADOW_VOLLEY_5H    = 59973,
    SPELL_DECAY_FLESH         = 49356,
    SPELL_GIFT_OF_THARON_JA   = 52509,
    SPELL_EYE_BEAM_5N         = 49544,
    SPELL_EYE_BEAM_5H         = 59965,
    SPELL_LIGHTNING_BREATH_5N = 49537,
    SPELL_LIGHTNING_BREATH_5H = 59963,
    SPELL_POISON_CLOUD_5N     = 49548,
    SPELL_POISON_CLOUD_5H     = 59969,
    SPELL_RETURN_FLESH        = 53463,
    SPELL_ACHIEVEMENT_CHECK   = 61863,
};

enum Yells
{
    SAY_AGGRO       = -1600011,
    SAY_KILL_1      = -1600012,
    SAY_KILL_2      = -1600013,
    SAY_FLESH_1     = -1600014,
    SAY_FLESH_2     = -1600015,
    SAY_SKELETON_1  = -1600016,
    SAY_SKELETON_2  = -1600017,
    SAY_DEATH       = -1600018,
};

enum ProphetModels
{
    MODEL_FLESH    = 27073,
    MODEL_SKELETON = 27511,
};

enum CombatPhase
{
    SKELETAL,
    GOING_FLESH,
    FLESH,
    GOING_SKELETAL,
};

class boss_prophet_tharon_ja : public CreatureScript
{
public:
    boss_prophet_tharon_ja() : CreatureScript("boss_prophet_tharon_ja") { }

    struct boss_prophet_tharon_jaAI : public QuantumBasicAI
    {
        boss_prophet_tharon_jaAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        uint32 PhaseTimer;
        uint32 CurseOfLifeTimer;
        uint32 RainOfFireTimer;
        uint32 ShadowVolleyTimer;
        uint32 EyeBeamTimer;
        uint32 LightningBreathTimer;
        uint32 PoisonCloudTimer;

        CombatPhase Phase;

        InstanceScript* instance;

        void Reset()
        {
            PhaseTimer = 20*IN_MILLISECONDS;
            CurseOfLifeTimer = 1*IN_MILLISECONDS;
            RainOfFireTimer = urand(14*IN_MILLISECONDS, 18*IN_MILLISECONDS);
            ShadowVolleyTimer = urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS);
            Phase = SKELETAL;
            me->SetDisplayId(me->GetNativeDisplayId());
            if (instance)
                instance->SetData(DATA_THARON_JA_EVENT, NOT_STARTED);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

            if (instance)
                instance->SetData(DATA_THARON_JA_EVENT, IN_PROGRESS);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            switch (Phase)
            {
                case SKELETAL:
                    if (CurseOfLifeTimer < diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
						{
                            DoCast(target, DUNGEON_MODE(SPELL_CURSE_OF_LIFE_5N, SPELL_CURSE_OF_LIFE_5H));
							CurseOfLifeTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
						}
                    }
					else CurseOfLifeTimer -= diff;

                    if (ShadowVolleyTimer < diff)
                    {
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
						{
							DoCast(target, DUNGEON_MODE(SPELL_SHADOW_VOLLEY_5N, SPELL_SHADOW_VOLLEY_5H));
							ShadowVolleyTimer = urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS);
						}
                    }
					else ShadowVolleyTimer -= diff;

                    if (RainOfFireTimer < diff)
                    {
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
						{
							DoCast(target, DUNGEON_MODE(SPELL_RAIN_OF_FIRE_5N, SPELL_RAIN_OF_FIRE_5H));
							RainOfFireTimer = urand(14*IN_MILLISECONDS, 18*IN_MILLISECONDS);
						}
                    }
					else RainOfFireTimer -= diff;

                    if (PhaseTimer < diff)
                    {
                        DoCast(SPELL_DECAY_FLESH);
                        Phase = GOING_FLESH;
                        PhaseTimer = 6*IN_MILLISECONDS;
                    }
					else PhaseTimer -= diff;
					DoMeleeAttackIfReady();
                    break;
                case GOING_FLESH:
                    if (PhaseTimer < diff)
                    {
                        DoSendQuantumText(RAND(SAY_FLESH_1, SAY_FLESH_2), me);
                        me->SetDisplayId(MODEL_FLESH);

                        std::list<Unit*> playerList;
                        SelectTargetList(playerList, 5, TARGET_TOP_AGGRO, 0, true);
                        for (std::list<Unit*>::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
                        {
                            Unit* temp = (*itr);
                            me->AddAura(SPELL_GIFT_OF_THARON_JA, temp);
                            temp->SetDisplayId(MODEL_SKELETON);
                        }
                        PhaseTimer = 20*IN_MILLISECONDS;
                        LightningBreathTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
                        EyeBeamTimer = urand(4*IN_MILLISECONDS, 8*IN_MILLISECONDS);
                        PoisonCloudTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
                        Phase = FLESH;
                    }
					else PhaseTimer -= diff;
                    break;
                case FLESH:
                    if (LightningBreathTimer < diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
						{
                            DoCast(target, DUNGEON_MODE(SPELL_LIGHTNING_BREATH_5N, SPELL_LIGHTNING_BREATH_5H));
							LightningBreathTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
						}
                    }
					else LightningBreathTimer -= diff;

                    if (EyeBeamTimer < diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
						{
                            DoCast(target, DUNGEON_MODE(SPELL_EYE_BEAM_5N, SPELL_EYE_BEAM_5H));
							EyeBeamTimer = urand(4*IN_MILLISECONDS, 6*IN_MILLISECONDS);
						}
                    }
					else EyeBeamTimer -= diff;

                    if (PoisonCloudTimer < diff)
                    {
                        DoCastAOE(DUNGEON_MODE(SPELL_POISON_CLOUD_5N, SPELL_POISON_CLOUD_5H));
                        PoisonCloudTimer = urand(10*IN_MILLISECONDS, 12*IN_MILLISECONDS);
                    }
					else PoisonCloudTimer -= diff;

                    if (PhaseTimer < diff)
                    {
                        DoCast(SPELL_RETURN_FLESH);
                        Phase = GOING_SKELETAL;
                        PhaseTimer = 6*IN_MILLISECONDS;
                    }
					else PhaseTimer -= diff;
                    DoMeleeAttackIfReady();
                    break;
                case GOING_SKELETAL:
                    if (PhaseTimer < diff)
                    {
                        DoSendQuantumText(RAND(SAY_SKELETON_1, SAY_SKELETON_2), me);
                        me->DeMorph();
                        Phase = SKELETAL;
                        PhaseTimer = 20*IN_MILLISECONDS;
                        CurseOfLifeTimer = 1*IN_MILLISECONDS;
                        RainOfFireTimer = urand(14*IN_MILLISECONDS, 18*IN_MILLISECONDS);
                        ShadowVolleyTimer = urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS);

                        std::list<Unit*> playerList;
                        SelectTargetList(playerList, 5, TARGET_TOP_AGGRO, 0, true);
                        for (std::list<Unit*>::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
                        {
                            Unit* temp = (*itr);
                            if (temp->HasAura(SPELL_GIFT_OF_THARON_JA))
                                temp->RemoveAura(SPELL_GIFT_OF_THARON_JA);
                            temp->DeMorph();
                        }
                    }
					else PhaseTimer -= diff;
                    break;
            }
        }

        void KilledUnit(Unit* who)
        {
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_KILL_1, SAY_KILL_2), me);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

            if (instance)
            {
                Map::PlayerList const &PlayerList = instance->instance->GetPlayers();

                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    if (Player* player = i->getSource())
                        player->DeMorph();
                instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET_2, SPELL_ACHIEVEMENT_CHECK);

                instance->SetData(DATA_THARON_JA_EVENT, DONE);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_prophet_tharon_jaAI(creature);
    }
};

void AddSC_boss_prophet_tharon_ja()
{
    new boss_prophet_tharon_ja;
}