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
#include "MapManager.h"
#include "oculus.h"

enum Yells
{
    SAY_AGGRO                          = -1578022,
    SAY_SLAY_1                         = -1578023,
    SAY_SLAY_2                         = -1578024,
    SAY_DEATH                          = -1578025,
    SAY_STRIKE_1                       = -1578026,
    SAY_STRIKE_2                       = -1578027,
    SAY_STRIKE_3                       = -1578028,
    SAY_SPAWN                          = -1578029,
};

enum Spells
{
    SPELL_ENERGIZE_CORES_VISUAL       = 62136,
    SPELL_ENERGIZE_CORES_5N           = 50785, // Damage 5938 to 6562, effec2 Triggers 54069, effect3 Triggers 56251
    SPELL_ENERGIZE_CORES_5H           = 59372,
    SPELL_CALL_AZURE_RING_CAPTAIN     = 51002, // Effect Send Event (12229)
    /*SPELL_CALL_AZURE_RING_CAPTAIN_2 = 51006, // Effect Send Event (10665)
    SPELL_CALL_AZURE_RING_CAPTAIN_3   = 51007, // Effect Send Event (18454)
    SPELL_CALL_AZURE_RING_CAPTAIN_4   = 51008, // Effect Send Event (18455)*/
    SPELL_CALL_AMPLIFY_MAGIC          = 51054,
    SPELL_CALL_AMPLIFY_MAGIC_H        = 59371,
    SPELL_ARCANE_BEAM_PERIODIC        = 51019,
    SPELL_SUMMON_ARCANE_BEAM          = 51017,
    SPELL_ARCANE_BEAM_VISUAL          = 51024,
    SPELL_CENTRIFUGE_CORE_PASSIVE     = 50798,
};

enum Events
{
    EVENT_ENERGIZE_CORES             = 1,
    EVENT_CALL_AZURE                 = 2,
    EVENT_AMPLIFY_MAGIC              = 3,
    EVENT_ENERGIZE_CORES_VISUAL      = 4,
};

static Position corePositions[] =
{
    {1305.189f, 1029.883f, 438.942f, 0.0f},
    {1276.570f, 1026.267f, 438.942f, 0.0f},
    {1245.383f, 1050.026f, 438.942f, 0.0f},
    {1241.266f, 1080.340f, 438.942f, 0.0f},
    {1264.629f, 1110.341f, 438.942f, 0.0f},
    {1295.459f, 1114.321f, 438.942f, 0.0f},
    {1325.323f, 1091.365f, 438.942f, 0.0f},
    {1329.289f, 1060.288f, 438.942f, 0.0f},
};

class boss_varos : public CreatureScript
{
    public:
        boss_varos() : CreatureScript("boss_varos") { }

        struct boss_varosAI : public BossAI
        {
            boss_varosAI(Creature* creature) : BossAI(creature, DATA_VAROS_EVENT)
            {
                if (instance->GetBossState(DATA_DRAKOS_EVENT) != DONE)
                    DoCast(me, SPELL_CENTRIFUGE_SHIELD);
            }

			uint8 EnergizedCore;
            uint32 SpawntextTimer;

            void Reset()
            {
                _Reset();

                events.ScheduleEvent(EVENT_AMPLIFY_MAGIC, urand(20, 25) * IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_ENERGIZE_CORES_VISUAL, 5000);
                // not sure if this is handled by a timer or hp percentage
                events.ScheduleEvent(EVENT_CALL_AZURE, urand(15, 30) * IN_MILLISECONDS);

                EnergizedCore = 0; // clockwise?

                SpawntextTimer = urand (1, 60) *IN_MILLISECONDS;
            }

            void EnterToBattle(Unit* /*who*/)
            {
                _EnterToBattle();

                DoSendQuantumText(SAY_AGGRO, me);

                SpawnCores();
            }

            void JustSummoned(Creature* summon)
            {
                summons.Summon(summon);
            }

            float GetCoreEnergizeOrientation(bool first)
            {
                uint8 mod = first ? EnergizedCore : EnergizedCore + 3;
                if (mod >= 8)
                    mod -= 8;

                float angle = me->GetAngle(corePositions[mod].GetPositionX(), corePositions[mod].GetPositionY());
                angle += first ? 0.2f : -0.2f;
                angle = MapManager::NormalizeOrientation(angle);
                return angle;
            }

            void SpawnCores()
            {
                for (uint8 i = 0; i < 8; ++i)
				{
                    if (Creature* core = me->SummonCreature(NPC_CENTRIFUGE_CORE, corePositions[i]))
                    {
                        core->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                        core->SetDisplayId(core->GetCreatureTemplate()->Modelid2);
                        core->AddAura(SPELL_CENTRIFUGE_CORE_PASSIVE, core);
                    }
				}
            }

            void UpdateAI(uint32 const diff)
            {
                if (!me->IsInCombatActive() && me->IsAlive())
                {
                    if (SpawntextTimer <= diff)
                    {
                        DoSendQuantumText(SAY_SPAWN, me);
                        SpawntextTimer = 60 *IN_MILLISECONDS;
                    }
                    else SpawntextTimer -= diff;
                }                
            
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ENERGIZE_CORES:
                            DoCast(me, DUNGEON_MODE(SPELL_ENERGIZE_CORES_5N, SPELL_ENERGIZE_CORES_5H));
                            break;
                        case EVENT_ENERGIZE_CORES_VISUAL:
                            EnergizedCore += 2;
                            if (EnergizedCore >= 8)
                                EnergizedCore -= 8;
                            DoCast(me, SPELL_ENERGIZE_CORES_VISUAL);
                            events.ScheduleEvent(EVENT_ENERGIZE_CORES_VISUAL, 5000);
                            events.ScheduleEvent(EVENT_ENERGIZE_CORES, 4000);
                            break;
                        case EVENT_CALL_AZURE:
                            DoCast(me, SPELL_CALL_AZURE_RING_CAPTAIN);
                            DoSendQuantumText(RAND(SAY_STRIKE_1, SAY_STRIKE_2, SAY_STRIKE_3), me);
                            events.ScheduleEvent(EVENT_CALL_AZURE, urand(20, 25) * IN_MILLISECONDS);
                            break;
                        case EVENT_AMPLIFY_MAGIC:
                            DoCastVictim(DUNGEON_MODE(SPELL_CALL_AMPLIFY_MAGIC, SPELL_CALL_AMPLIFY_MAGIC_H));
                            events.ScheduleEvent(EVENT_AMPLIFY_MAGIC, urand(17, 20) * IN_MILLISECONDS);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            void JustDied(Unit* /*killer*/)
            {
                DoSendQuantumText(SAY_DEATH, me);
                _JustDied();
                DoCast(me, SPELL_DEATH_SPELL, true);
            }
            
            void KilledUnit(Unit* /*victim*/)
            {
                DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_varosAI(creature);
        }
};

class npc_azure_ring_captain : public CreatureScript
{
    public:
        npc_azure_ring_captain() : CreatureScript("npc_azure_ring_captain") { }

        struct npc_azure_ring_captainAI : public QuantumBasicAI
        {
            npc_azure_ring_captainAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

			uint64 targetGUID;
            InstanceScript* instance;

            void Reset()
            {
                targetGUID = 0;

                me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING | MOVEMENTFLAG_FLYING);
                me->SetReactState(REACT_AGGRESSIVE);
            }

            void UpdateAI(uint32 const /*diff*/)
            {
                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE || id != ACTION_CALL_DRAGON_EVENT)
                    return;

                me->GetMotionMaster()->MoveIdle();

                if (Unit* target = ObjectAccessor::GetUnit(*me, targetGUID))
                {
                    if (Creature* summoned = target->SummonCreature(28239, *target, TEMPSUMMON_TIMED_DESPAWN, 10000))
                    {
                        summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
                        DoCast(summoned, SPELL_ARCANE_BEAM_VISUAL);
                    }
                }
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_CALL_DRAGON_EVENT:
					{
						if (instance)
                        {
                            if (Creature* varos = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_VAROS)))
                            {
                                if (Unit* victim = varos->AI()->SelectTarget(TARGET_RANDOM, 0))
                                {
                                    me->DespawnAfterAction(20*IN_MILLISECONDS);
                                    me->SetReactState(REACT_PASSIVE);
                                    me->SetWalk(false);
                                    me->GetMotionMaster()->MovePoint(ACTION_CALL_DRAGON_EVENT, victim->GetPositionX(), victim->GetPositionY(), victim->GetPositionZ() + 20.0f);
                                    targetGUID = victim->GetGUID();
                                }
                            }
                        }
					}
					break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_azure_ring_captainAI(creature);
        }
};

class spell_varos_centrifuge_shield : public SpellScriptLoader
{
    public:
        spell_varos_centrifuge_shield() : SpellScriptLoader("spell_varos_centrifuge_shield") { }

        class spell_varos_centrifuge_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_varos_centrifuge_shield_AuraScript);

            bool Load()
            {
                Unit* caster = GetCaster();
                return (caster && caster->ToCreature());
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    // flags taken from sniffs
                    if (caster->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING | UNIT_FLAG_NO_AGGRO_FOR_CREATURE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_UNK_6))
                    {
                        caster->ToCreature()->SetReactState(REACT_PASSIVE);
                        caster->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING | UNIT_FLAG_NO_AGGRO_FOR_CREATURE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_UNK_6);
                    }
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    caster->ToCreature()->SetReactState(REACT_DEFENSIVE);
                    caster->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SWIMMING | UNIT_FLAG_NO_AGGRO_FOR_CREATURE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_UNK_6);
                }
            }

            void Register()
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_varos_centrifuge_shield_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectApply += AuraEffectApplyFn(spell_varos_centrifuge_shield_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_varos_centrifuge_shield_AuraScript();
        }
};

class spell_varos_energize_core_area_enemy : public SpellScriptLoader
{
    public:
        spell_varos_energize_core_area_enemy() : SpellScriptLoader("spell_varos_energize_core_area_enemy") {}

        class spell_varos_energize_core_area_enemySpellScript : public SpellScript
        {
            PrepareSpellScript(spell_varos_energize_core_area_enemySpellScript)

            void FilterTargets(std::list<Unit*>& targetList)
            {
                Creature* varos = GetCaster()->ToCreature();
                if (!varos)
                    return;

                if (varos->GetEntry() != NPC_VAROS_CLOUDSTRIDER)
                    return;

                float lborder = CAST_AI(boss_varos::boss_varosAI, varos->AI())->GetCoreEnergizeOrientation(true);
                float rborder = CAST_AI(boss_varos::boss_varosAI, varos->AI())->GetCoreEnergizeOrientation(false);
                std::list<Unit*> tempList;

                for (std::list<Unit*>::iterator itr = targetList.begin(); itr != targetList.end(); ++itr)
                {
                    float angle = varos->GetAngle((*itr)->GetPositionX(), (*itr)->GetPositionY());

                    if (lborder < rborder)
                    {
                        if (angle >= lborder && angle <= rborder)
                            continue;
                    }
                    else if (angle >= lborder || angle <= rborder)
                        continue;

                    tempList.push_back(*itr);
                }

                targetList = tempList;
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_varos_energize_core_area_enemySpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_varos_energize_core_area_enemySpellScript();
        }
};

class spell_varos_energize_core_area_entry : public SpellScriptLoader
{
    public:
        spell_varos_energize_core_area_entry() : SpellScriptLoader("spell_varos_energize_core_area_entry") {}

        class spell_varos_energize_core_area_entrySpellScript : public SpellScript
        {
            PrepareSpellScript(spell_varos_energize_core_area_entrySpellScript)

            void FilterTargets(std::list<Unit*>& targetList)
            {
                Creature* varos = GetCaster()->ToCreature();
                if (!varos)
                    return;

                if (varos->GetEntry() != NPC_VAROS_CLOUDSTRIDER)
                    return;

                float lborder = CAST_AI(boss_varos::boss_varosAI, varos->AI())->GetCoreEnergizeOrientation(true);
                float rborder = CAST_AI(boss_varos::boss_varosAI, varos->AI())->GetCoreEnergizeOrientation(false);
                std::list<Unit*> tempList;

                for (std::list<Unit*>::iterator itr = targetList.begin(); itr != targetList.end(); ++itr)
                {
                    float angle = varos->GetAngle((*itr)->GetPositionX(), (*itr)->GetPositionY());

                    if (lborder < rborder)
                    {
                        if (angle >= lborder && angle <= rborder)
                            continue;
                    }
                    else if (angle >= lborder || angle <= rborder)
                        continue;

                    tempList.push_back(*itr);
                }

                targetList = tempList;
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_varos_energize_core_area_entrySpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_varos_energize_core_area_entrySpellScript();
        }
};

void AddSC_boss_varos()
{
    new boss_varos();
    new npc_azure_ring_captain();
    new spell_varos_centrifuge_shield();
    new spell_varos_energize_core_area_enemy();
    new spell_varos_energize_core_area_entry();
}