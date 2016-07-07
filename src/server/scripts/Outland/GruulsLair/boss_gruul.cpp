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
SDName: Boss_Gruul
SD%Complete: 60
SDComment: Ground Slam need further development (knock back effect must be added to the core)
SDCategory: Gruul's Lair
EndScriptData */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "SpellScript.h"
#include "gruuls_lair.h"

enum Texts
{
    SAY_AGGRO                   = -1565010,
    SAY_SLAM1                   = -1565011,
    SAY_SLAM2                   = -1565012,
    SAY_SHATTER1                = -1565013,
    SAY_SHATTER2                = -1565014,
    SAY_SLAY1                   = -1565015,
    SAY_SLAY2                   = -1565016,
    SAY_SLAY3                   = -1565017,
    SAY_DEATH                   = -1565018,
    EMOTE_GROW                  = -1565019,
};

enum Spells
{
    SPELL_GROWTH                = 36300,
    SPELL_CAVE_IN               = 36240,
    SPELL_GROUND_SLAM           = 33525,
    SPELL_REVERBERATION         = 36297,
    SPELL_SHATTER               = 33654,
    SPELL_SHATTER_EFFECT        = 33671,
    SPELL_HURTFUL_STRIKE        = 33813,
    SPELL_STONED                = 33652,
    SPELL_MAGNETIC_PULL         = 28337,
    SPELL_KNOCK_BACK            = 24199,
};

class boss_gruul : public CreatureScript
{
public:
    boss_gruul() : CreatureScript("boss_gruul") { }

    struct boss_gruulAI : public QuantumBasicAI
    {
        boss_gruulAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 GrowthTimer;
        uint32 CaveInTimer;
        uint32 CaveInStaticTimer;
        uint32 GroundSlamTimer;
        uint32 HurtfulStrikeTimer;
        uint32 ReverberationTimer;

        bool PerformingGroundSlam;

        void Reset()
        {
            GrowthTimer = 30000;
            CaveInTimer = 27000;
            CaveInStaticTimer = 30000;
            GroundSlamTimer = 35000;
            PerformingGroundSlam = false;
            HurtfulStrikeTimer = 8000;
            ReverberationTimer = 60000+45000;

			instance->SetData(DATA_GRUUL_EVENT, NOT_STARTED);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

			instance->SetData(DATA_GRUUL_EVENT, IN_PROGRESS);
        }

        void KilledUnit(Unit* /*who*/)
        {
            DoSendQuantumText(RAND(SAY_SLAY1, SAY_SLAY2, SAY_SLAY3), me);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

			instance->SetData(DATA_GRUUL_EVENT, DONE);
			instance->HandleGameObject(instance->GetData64(DATA_GRUUL_DOOR), true);
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_GROUND_SLAM)
            {
                if (target->GetTypeId() == TYPE_ID_PLAYER)
                {
                    switch (urand(0, 1))
                    {
                        case 0:
							target->CastSpell(target, SPELL_MAGNETIC_PULL, true, NULL, NULL, me->GetGUID());
							break;
                        case 1:
							target->CastSpell(target, SPELL_KNOCK_BACK, true, NULL, NULL, me->GetGUID());
							break;
                    }
                }
            }

            //this part should be in the core
            if (spell->Id == SPELL_SHATTER)
            {
				// todo: use eventmap to kill this stuff
                //clear this, if we are still performing
                if (PerformingGroundSlam)
                {
                    PerformingGroundSlam = false;

                    if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    {
                        if (me->GetVictim())
                            me->GetMotionMaster()->MoveChase(me->GetVictim());
                    }
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if (me->HasAura(SPELL_STONED))
                me->RemoveAurasDueToSpell(SPELL_STONED);

            if (GrowthTimer <= diff)
            {
                DoSendQuantumText(EMOTE_GROW, me);
                DoCast(me, SPELL_GROWTH);
                GrowthTimer = 30000;
            }
            else
                GrowthTimer -= diff;

            if (PerformingGroundSlam)
            {
                if (GroundSlamTimer <= diff)
                {
                    GroundSlamTimer =120000;
                    HurtfulStrikeTimer= 8000;

                    if (ReverberationTimer < 10000)     //Give a little time to the players to undo the damage from shatter
                        ReverberationTimer += 10000;

                    DoCast(me, SPELL_SHATTER);
                }
                else GroundSlamTimer -= diff;
            }
            else
            {
                if (HurtfulStrikeTimer <= diff)
                {
                    Unit* target = SelectTarget(TARGET_TOP_AGGRO, 1);

                    if (target && me->IsWithinMeleeRange(me->GetVictim()))
                        DoCast(target, SPELL_HURTFUL_STRIKE);
                    else
                        DoCastVictim(SPELL_HURTFUL_STRIKE);

                    HurtfulStrikeTimer= 8000;
                }
                else HurtfulStrikeTimer -= diff;

                if (ReverberationTimer <= diff)
                {
                    DoCastVictim(SPELL_REVERBERATION, true);
                    ReverberationTimer = urand(15000, 25000);
                }
                else
                    ReverberationTimer -= diff;

                // Cave In
                if (CaveInTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                        DoCast(target, SPELL_CAVE_IN);

                    if (CaveInStaticTimer >= 4000)
                        CaveInStaticTimer -= 2000;

                        CaveInTimer = CaveInStaticTimer;
                }
                else CaveInTimer -= diff;

                if (GroundSlamTimer <= diff)
                {
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveIdle();

                    PerformingGroundSlam= true;
                    GroundSlamTimer = 10000;

                    DoCast(me, SPELL_GROUND_SLAM);
                }
                else GroundSlamTimer -= diff;

                DoMeleeAttackIfReady();
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_gruulAI(creature);
    }
};

class spell_gruul_shatter : public SpellScriptLoader
{
    public:
        spell_gruul_shatter() : SpellScriptLoader("spell_gruul_shatter") { }

        class spell_gruul_shatter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gruul_shatter_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_STONED))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_SHATTER_EFFECT))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    target->RemoveAurasDueToSpell(SPELL_STONED);
                    target->CastSpell((Unit*)NULL, SPELL_SHATTER_EFFECT, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gruul_shatter_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gruul_shatter_SpellScript();
        }
};

class spell_gruul_shatter_effect : public SpellScriptLoader
{
    public:
        spell_gruul_shatter_effect() : SpellScriptLoader("spell_gruul_shatter_effect") { }

        class spell_gruul_shatter_effect_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gruul_shatter_effect_SpellScript);

            void CalculateDamage()
            {
                if (!GetHitUnit())
                    return;

                float radius = GetSpellInfo()->Effects[EFFECT_0].CalcRadius(GetCaster());
                if (!radius)
                    return;

                float distance = GetCaster()->GetDistance2d(GetHitUnit());
                if (distance > 1.0f)
                    SetHitDamage(int32(GetHitDamage() * ((radius - distance) / radius)));
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_gruul_shatter_effect_SpellScript::CalculateDamage);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gruul_shatter_effect_SpellScript();
        }
};

void AddSC_boss_gruul()
{
    new boss_gruul();
	new spell_gruul_shatter();
	new spell_gruul_shatter_effect();
}