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
#include "molten_core.h"

enum Spells
{
    SPELL_ARCANE_EXPLOSION    = 19712,
    SPELL_SHAZZRAH_CURSE      = 19713,
    SPELL_MAGIC_GROUNDING     = 19714,
    SPELL_COUNTERSPELL        = 19715,
    SPELL_SHAZZRAH_GATE_DUMMY = 23138,
    SPELL_SHAZZRAH_GATE       = 23139,
};

enum Events
{
    EVENT_ARCANE_EXPLOSION           = 1,
    EVENT_ARCANE_EXPLOSION_TRIGGERED = 2,
    EVENT_SHAZZRAH_CURSE             = 3,
    EVENT_MAGIC_GROUNDING            = 4,
    EVENT_COUNTERSPELL               = 5,
    EVENT_SHAZZRAH_GATE              = 6,
};

class boss_shazzrah : public CreatureScript
{
    public:
        boss_shazzrah() : CreatureScript("boss_shazzrah") { }

        struct boss_shazzrahAI : public BossAI
        {
            boss_shazzrahAI(Creature* creature) : BossAI(creature, BOSS_SHAZZRAH) { }

            void EnterToBattle(Unit* victim)
            {
                BossAI::EnterToBattle(victim);

                events.ScheduleEvent(EVENT_ARCANE_EXPLOSION, 6000);
                events.ScheduleEvent(EVENT_SHAZZRAH_CURSE, 10000);
                events.ScheduleEvent(EVENT_MAGIC_GROUNDING, 24000);
                events.ScheduleEvent(EVENT_COUNTERSPELL, 15000);
                events.ScheduleEvent(EVENT_SHAZZRAH_GATE, 45000);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_ARCANE_EXPLOSION:
                            DoCastVictim(SPELL_ARCANE_EXPLOSION);
                            events.ScheduleEvent(EVENT_ARCANE_EXPLOSION, urand(4000, 7000));
                            break;
                        case EVENT_ARCANE_EXPLOSION_TRIGGERED:
                            DoCastVictim(SPELL_ARCANE_EXPLOSION);
                            break;
                        case EVENT_SHAZZRAH_CURSE:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 0.0f, true, -SPELL_SHAZZRAH_CURSE))
							{
                                DoCast(target, SPELL_SHAZZRAH_CURSE);
							}
                            events.ScheduleEvent(EVENT_SHAZZRAH_CURSE, urand(25000, 30000));
                            break;
                        case EVENT_MAGIC_GROUNDING:
                            DoCast(me, SPELL_MAGIC_GROUNDING);
                            events.ScheduleEvent(EVENT_MAGIC_GROUNDING, 35000);
                            break;
                        case EVENT_COUNTERSPELL:
                            DoCastVictim(SPELL_COUNTERSPELL);
                            events.ScheduleEvent(EVENT_COUNTERSPELL, urand(16000, 20000));
                            break;
                        case EVENT_SHAZZRAH_GATE:
                            DoResetThreat();
                            DoCastAOE(SPELL_SHAZZRAH_GATE_DUMMY);
                            events.ScheduleEvent(EVENT_ARCANE_EXPLOSION_TRIGGERED, 2000);
                            events.RescheduleEvent(EVENT_ARCANE_EXPLOSION, urand(3000, 6000));
                            events.ScheduleEvent(EVENT_SHAZZRAH_GATE, 45000);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_shazzrahAI(creature);
        }
};

class spell_shazzrah_gate_dummy : public SpellScriptLoader
{
    public:
        spell_shazzrah_gate_dummy() : SpellScriptLoader("spell_shazzrah_gate_dummy") { }

        class spell_shazzrah_gate_dummy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_shazzrah_gate_dummy_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SHAZZRAH_GATE))
                    return false;
                return true;
            }

			void FilterTargets(std::list<Unit*>& unitList)
            {
				if (unitList.empty())
					return;

				Unit* target = Quantum::DataPackets::SelectRandomContainerElement(unitList);
				unitList.clear();
				unitList.push_back(target);
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    target->CastSpell(GetCaster(), SPELL_SHAZZRAH_GATE, true);
                    if (Creature* creature = GetCaster()->ToCreature())
                        creature->AI()->AttackStart(target);
                }
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_shazzrah_gate_dummy_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_shazzrah_gate_dummy_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_shazzrah_gate_dummy_SpellScript();
        }
};

void AddSC_boss_shazzrah()
{
    new boss_shazzrah();
    new spell_shazzrah_gate_dummy();
}