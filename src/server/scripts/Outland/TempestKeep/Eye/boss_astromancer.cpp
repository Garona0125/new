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
#include "the_eye.h"

enum Texts
{
    SAY_AGGRO                           = -1550007,
    SAY_SUMMON1                         = -1550008,
    SAY_SUMMON2                         = -1550009,
    SAY_KILL1                           = -1550010,
    SAY_KILL2                           = -1550011,
    SAY_KILL3                           = -1550012,
    SAY_DEATH                           = -1550013,
    SAY_VOIDA                           = -1550014,
    SAY_VOIDB                           = -1550015,
};

enum Spells
{
    SPELL_ARCANE_MISSILES               = 33031,
    SPELL_WRATH_OF_THE_ASTROMANCER      = 42783,
	SPELL_WRATH_OF_THE_ASTROMANCER_DOT  = 42784,
    SPELL_BLINDING_LIGHT                = 33009,
    SPELL_FEAR                          = 34322,
    SPELL_VOID_BOLT                     = 39329,
    SPELL_SPOTLIGHT                     = 25824,
    MODEL_HUMAN                         = 18239,
    MODEL_VOIDWALKER                    = 18988,
    SPELL_SOLARIUM_GREAT_HEAL           = 33387,
    SPELL_SOLARIUM_HOLY_SMITE           = 25054,
    SPELL_SOLARIUM_ARCANE_TORRENT       = 33390,
    WV_ARMOR                            = 31000,
};

const float CENTER_X            = 432.909f;
const float CENTER_Y            = -373.424f;
const float CENTER_Z            = 17.9608f;
const float CENTER_O            = 1.06421f;
const float SMALL_PORTAL_RADIUS = 12.6f;
const float LARGE_PORTAL_RADIUS = 26.0f;
const float PORTAL_Z            = 17.005f;

/* not used                        // x,          y,      z,         o
static float SolarianPos[4] = {432.909f, -373.424f, 17.9608f, 1.06421f};
*/

class boss_high_astromancer_solarian : public CreatureScript
{
    public:
        boss_high_astromancer_solarian() : CreatureScript("boss_high_astromancer_solarian") { }

        struct boss_high_astromancer_solarianAI : public QuantumBasicAI
        {
            boss_high_astromancer_solarianAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
            {
                instance = creature->GetInstanceScript();

                DefaultArmor = creature->GetArmor();
                defaultsize = creature->GetFloatValue(OBJECT_FIELD_SCALE_X);
            }

            InstanceScript* instance;
            SummonList Summons;

            uint8 Phase;

            uint32 ArcaneMissilesTimer;
            uint32 WrathOfTheAstromancerTimer;
            uint32 BlindingLightTimer;
            uint32 FearTimer;
            uint32 VoidBoltTimer;
            uint32 Phase1Timer;
            uint32 Phase2Timer;
            uint32 Phase3Timer;
            uint32 AppearDelayTimer;
            uint32 DefaultArmor;
            uint32 WrathTimer;

            float defaultsize;
            float Portals[3][3];

            bool AppearDelay;
            bool BlindingLight;

            void Reset()
            {
                ArcaneMissilesTimer = 2000;
                WrathOfTheAstromancerTimer = 15000;
                BlindingLightTimer = 41000;
                FearTimer = 20000;
                VoidBoltTimer = 10000;
                Phase1Timer = 50000;
                Phase2Timer = 10000;
                Phase3Timer = 15000;
                AppearDelayTimer = 2000;
                BlindingLight = false;
                AppearDelay = false;
                WrathTimer = 20000+rand()%5000;//twice in phase one
                Phase = 1;

				instance->SetData(DATA_HIGH_ASTROMANCER_SOLARIAN, NOT_STARTED);

                me->SetArmor(DefaultArmor);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetVisible(true);
                me->SetObjectScale(defaultsize);
                me->SetDisplayId(MODEL_HUMAN);

                Summons.DespawnAll();
            }

            void KilledUnit(Unit* /*victim*/)
            {
                DoSendQuantumText(RAND(SAY_KILL1, SAY_KILL2, SAY_KILL3), me);
            }

            void JustDied(Unit* /*victim*/)
            {
                me->SetObjectScale(defaultsize);

                me->SetDisplayId(MODEL_HUMAN);

                DoSendQuantumText(SAY_DEATH, me);

				instance->SetData(DATA_HIGH_ASTROMANCER_SOLARIAN, DONE);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                DoSendQuantumText(SAY_AGGRO, me);

                DoZoneInCombat();

				instance->SetData(DATA_HIGH_ASTROMANCER_SOLARIAN, IN_PROGRESS);
            }

            void SummonMinion(uint32 entry, float x, float y, float z)
            {
                Creature* Summoned = me->SummonCreature(entry, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                if (Summoned)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                        Summoned->AI()->AttackStart(target);

                    Summons.Summon(Summoned);
                }
            }

            float Portal_X(float radius)
            {
                if (urand(0, 1))
                    radius = -radius;

                return radius * (float)(rand()%100)/100.0f + CENTER_X;
            }

            float Portal_Y(float x, float radius)
            {
                float z = RAND(1.0f, -1.0f);

                return (z*sqrt(radius*radius - (x - CENTER_X)*(x - CENTER_X)) + CENTER_Y);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;
                if (AppearDelay)
                {
                    me->StopMoving();
                    me->AttackStop();
                    if (AppearDelayTimer <= diff)
                    {
                        AppearDelay = false;
                        if (Phase == 2)
                        {
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            me->SetVisible(false);
                        }
                        AppearDelayTimer = 2000;
                    }
                    else AppearDelayTimer -= diff;
                }
                if (Phase == 1)
                {
                    if (BlindingLightTimer <= diff)
                    {
                        BlindingLight = true;
                        BlindingLightTimer = 45000;
                    }
                    else BlindingLightTimer -= diff;

                    if (WrathTimer <= diff)
                    {
                        me->InterruptNonMeleeSpells(false);
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 100, true))
                            DoCast(target, SPELL_WRATH_OF_THE_ASTROMANCER, true);
                        WrathTimer = 20000+rand()%5000;
                    }
                    else WrathTimer -= diff;

                    if (ArcaneMissilesTimer <= diff)
                    {
                        if (BlindingLight)
                        {
                            DoCastVictim(SPELL_BLINDING_LIGHT);
                            BlindingLight = false;
                        }
                        else
                        {
                            Unit* target = SelectTarget(TARGET_RANDOM, 0);
                            if (!me->HasInArc(2.5f, target))
                                target = me->GetVictim();
                            if (target)
                                DoCast(target, SPELL_ARCANE_MISSILES);
                        }
                        ArcaneMissilesTimer = 3000;
                    }
                    else ArcaneMissilesTimer -= diff;

                    if (WrathOfTheAstromancerTimer <= diff)
                    {
                        me->InterruptNonMeleeSpells(false);
                        //Target the tank ?
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
                        {
                            if (target->GetTypeId() == TYPE_ID_PLAYER)
                            {
                                DoCast(target, SPELL_WRATH_OF_THE_ASTROMANCER);
                                WrathOfTheAstromancerTimer = 25000;
                            }
                            else WrathOfTheAstromancerTimer = 1000;
                        }
                    }
                    else WrathOfTheAstromancerTimer -= diff;

                    //Phase1Timer
                    if (Phase1Timer <= diff)
                    {
                        Phase = 2;
                        Phase1Timer = 50000;
                        //After these 50 seconds she portals to the middle of the room and disappears, leaving 3 light portals behind.
                        me->GetMotionMaster()->Clear();
                        me->SetPosition(CENTER_X, CENTER_Y, CENTER_Z, CENTER_O);
                        for (uint8 i=0; i <= 2; ++i)
                        {
                            if (!i)
                            {
                                Portals[i][0] = Portal_X(SMALL_PORTAL_RADIUS);
                                Portals[i][1] = Portal_Y(Portals[i][0], SMALL_PORTAL_RADIUS);
                                Portals[i][2] = CENTER_Z;
                            }
                            else
                            {
                                Portals[i][0] = Portal_X(LARGE_PORTAL_RADIUS);
                                Portals[i][1] = Portal_Y(Portals[i][0], LARGE_PORTAL_RADIUS);
                                Portals[i][2] = PORTAL_Z;
                            }
                        }
                        if ((abs(Portals[2][0] - Portals[1][0]) < 7) && (abs(Portals[2][1] - Portals[1][1]) < 7))
                        {
                            int i=1;
                            if (abs(CENTER_X + 26.0f - Portals[2][0]) < 7)
                                i = -1;
                            Portals[2][0] = Portals[2][0]+7*i;
                            Portals[2][1] = Portal_Y(Portals[2][0], LARGE_PORTAL_RADIUS);
                        }
                        for (int i=0; i <= 2; ++i)
                        {
                            if (Creature* Summoned = me->SummonCreature(NPC_SPOTLIGHT, Portals[i][0], Portals[i][1], Portals[i][2], CENTER_O, TEMPSUMMON_TIMED_DESPAWN, Phase2Timer+Phase3Timer+AppearDelayTimer+1700))
                            {
                                Summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                Summoned->CastSpell(Summoned, SPELL_SPOTLIGHT, false);
                            }
                        }
                        AppearDelay = true;
                    }
                    else Phase1Timer-=diff;
                }
                else
                    if (Phase == 2)
                    {
                        //10 seconds after Solarian disappears, 12 mobs spawn out of the three portals.
                        me->AttackStop();
                        me->StopMoving();
                        if (Phase2Timer <= diff)
                        {
                            Phase = 3;
                            for (int i=0; i <= 2; ++i)
                                for (int j=1; j <= 4; j++)
                                    SummonMinion(NPC_SOLARIUM_AGENT, Portals[i][0], Portals[i][1], Portals[i][2]);

                            DoSendQuantumText(SAY_SUMMON1, me);
                            Phase2Timer = 10000;
                        }
                        else Phase2Timer -= diff;
                    }
                    else
                        if (Phase == 3)
                        {
                            me->AttackStop();
                            me->StopMoving();
                            //Check Phase3Timer
                            if (Phase3Timer <= diff)
                            {
                                Phase = 1;
                                //15 seconds later Solarian reappears out of one of the 3 portals. Simultaneously, 2 healers appear in the two other portals.
                                int i = rand()%3;
                                me->GetMotionMaster()->Clear();
                                me->SetPosition(Portals[i][0], Portals[i][1], Portals[i][2], CENTER_O);

                                for (int j=0; j <= 2; j++)
                                    if (j != i)
                                        SummonMinion(NPC_SOLARIUM_PRIEST, Portals[j][0], Portals[j][1], Portals[j][2]);

                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                me->SetVisible(true);

                                DoSendQuantumText(SAY_SUMMON2, me);
                                AppearDelay = true;
                                Phase3Timer = 15000;
                            }
                            else Phase3Timer -= diff;
                        }
                        else
                            if (Phase == 4)
                            {
                                if (FearTimer <= diff)
                                {
                                    DoCast(me, SPELL_FEAR);
                                    FearTimer = 20000;
                                }
                                else FearTimer -= diff;

                                if (VoidBoltTimer <= diff)
                                {
                                    DoCastVictim(SPELL_VOID_BOLT);
                                    VoidBoltTimer = 10000;
                                }
                                else VoidBoltTimer -= diff;
                            }
                            //When Solarian reaches 20% she will transform into a huge void walker.
                            if (Phase != 4 && me->HealthBelowPct(20))
                            {
                                Phase = 4;
                                //To make sure she wont be invisible or not selecatble
                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                me->SetVisible(true);
                                DoSendQuantumText(SAY_VOIDA, me);
                                DoSendQuantumText(SAY_VOIDB, me);
                                me->SetArmor(WV_ARMOR);
                                me->SetDisplayId(MODEL_VOIDWALKER);
                                me->SetObjectScale(defaultsize*2.5f);
                            }

							DoMeleeAttackIfReady();
             }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_high_astromancer_solarianAI(creature);
        }
};

class npc_solarium_priest : public CreatureScript
{
    public:
        npc_solarium_priest() : CreatureScript("npc_solarium_priest") { }

        struct npc_solarium_priestAI : public QuantumBasicAI
        {
            npc_solarium_priestAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint32 HealTimer;
            uint32 HolysmiteTimer;
            uint32 SilenceTimer;

            void Reset()
            {
                HealTimer = 9000;
                HolysmiteTimer = 500;
                SilenceTimer = 15000;
            }

            void EnterToBattle(Unit* /*who*/){}

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (HealTimer <= diff)
                {
                    Unit* target = NULL;
                    switch (urand(0, 1))
                    {
                        case 0:
							target = Unit::GetUnit(*me, instance->GetData64(DATA_HIGH_ASTROMANCER_SOLARIAN));
                            break;
                        case 1:
                            target = me;
                            break;
                    }

                    if (target)
                    {
                        DoCast(target, SPELL_SOLARIUM_GREAT_HEAL);
                        HealTimer = 9000;
                    }
                }
                else HealTimer -= diff;

                if (HolysmiteTimer <= diff)
                {
                    DoCastVictim(SPELL_SOLARIUM_HOLY_SMITE);
                    HolysmiteTimer = 4000;
                }
                else HolysmiteTimer -= diff;

                if (SilenceTimer <= diff)
                {
                    DoCastVictim(SPELL_SOLARIUM_ARCANE_TORRENT);
                    SilenceTimer = 13000;
                }
                else SilenceTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_solarium_priestAI(creature);
        }
};

class spell_astromancer_wrath_of_the_astromancer : public SpellScriptLoader
{
    public:
        spell_astromancer_wrath_of_the_astromancer() : SpellScriptLoader("spell_astromancer_wrath_of_the_astromancer") { }

        class spell_astromancer_wrath_of_the_astromancer_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_astromancer_wrath_of_the_astromancer_SpellScript);

            bool Validate(SpellInfo const* /*SpellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_WRATH_OF_THE_ASTROMANCER_DOT))
                    return false;
                return true;
            }

            bool Load()
            {
                _targetCount = 0;
                return true;
            }

            void CountTargets(std::list<Unit*>& targetList)
            {
                _targetCount = targetList.size();
            }

            void HandleDummy(SpellEffIndex /* effIndex */)
            {
                if (Unit* caster = GetOriginalCaster())
				{
                    if (Unit* target = GetHitUnit())
                    {
                        if (!target->IsAlive() || !_targetCount)
                            return;

                        int32 damage = 10000 / _targetCount;

                        SpellNonMeleeDamage damageInfo(caster, target, GetSpellInfo()->Id, GetSpellInfo()->SchoolMask);
                        damageInfo.damage = damage;

                        caster->CalcAbsorbResist(target, GetSpellInfo()->GetSchoolMask(), DOT, damage, &damageInfo.absorb, &damageInfo.resist, GetSpellInfo());
                        caster->DealDamageMods(target, damageInfo.damage, &damageInfo.absorb);
                        caster->SendSpellNonMeleeDamageLog(&damageInfo);
                        caster->DealSpellDamage(&damageInfo, false);
                    }
				}
            }

        private:
            int32 _targetCount;

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_astromancer_wrath_of_the_astromancer_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_astromancer_wrath_of_the_astromancer_SpellScript::CountTargets, EFFECT_0, TARGET_DEST_CASTER_RADIUS);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_astromancer_wrath_of_the_astromancer_SpellScript();
        }
};

void AddSC_boss_high_astromancer_solarian()
{
    new boss_high_astromancer_solarian();
    new npc_solarium_priest();
	new spell_astromancer_wrath_of_the_astromancer();
}