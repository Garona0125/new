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
#include "the_botanica.h"

enum Says
{
    SAY_AGGRO                   = -1441000,
    SAY_SLAY_1                  = -1441001,
	SAY_SLAY_2                  = -1441002,
    SAY_ARCANE_RESONANCE        = -1441003,
    SAY_ARCANE_DEVASTATION      = -1441004,
    EMOTE_SUMMON                = -1441005,
    SAY_SUMMON                  = -1441006,
    SAY_DEATH                   = -1441007,
};

enum Spells
{
    SPELL_ARCANE_RESONANCE      = 34794,
    SPELL_ARCANE_DEVASTATION    = 34799,
    SPELL_SUMMON_REINFORCEMENTS = 34803,
};

enum Events
{
    EVENT_ARCANE_RESONANCE      = 1,
    EVENT_ARCANE_DEVASTATION    = 2,
};

class boss_commander_sarannis : public CreatureScript
{
    public: boss_commander_sarannis() : CreatureScript("boss_commander_sarannis") { }

        struct boss_commander_sarannisAI : public BossAI
        {
            boss_commander_sarannisAI(Creature* creature) : BossAI(creature, DATA_COMMANDER_SARANNIS) { }

			bool phase;

            void Reset()
            {
				_Reset();
				phase = true;

				DoCast(me, SPELL_UNIT_DETECTION_ALL);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                _EnterToBattle();

                DoSendQuantumText(SAY_AGGRO, me);

                events.ScheduleEvent(EVENT_ARCANE_RESONANCE, 42700);
                events.ScheduleEvent(EVENT_ARCANE_DEVASTATION, 15200);
            }

            void KilledUnit(Unit* victim)
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

            void JustDied(Unit* /*killer*/)
            {
                _JustDied();
                DoSendQuantumText(SAY_DEATH, me);
            }

            void DamageTaken(Unit* /*killer*/, uint32 &damage)
            {
                if (me->HealthBelowPctDamaged(50, damage) && phase)
                {
                    phase = false;
                    DoSendQuantumText(EMOTE_SUMMON, me);
                    DoSendQuantumText(SAY_SUMMON, me);
                    DoCast(me, SPELL_SUMMON_REINFORCEMENTS);
                }
            }

            void JustSummoned(Creature* summon)
            {
                BossAI::JustSummoned(summon);
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
                        case EVENT_ARCANE_RESONANCE:
                            DoSendQuantumText(SAY_ARCANE_RESONANCE, me);
                            DoCastVictim(SPELL_ARCANE_RESONANCE, true);
                            events.ScheduleEvent(EVENT_ARCANE_RESONANCE, 42700);
                            break;
                        case EVENT_ARCANE_DEVASTATION:
                            DoSendQuantumText(SAY_ARCANE_DEVASTATION, me);
                            DoCastVictim(SPELL_ARCANE_DEVASTATION, true);
                            events.ScheduleEvent(EVENT_ARCANE_DEVASTATION, urand(11000, 19200));
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
            return new boss_commander_sarannisAI(creature);
        }
};

Position const PosSummonReinforcements[4] =
{
    { 160.4483f, 287.6435f, -3.887904f, 2.3841f },
    { 153.4406f, 289.9929f, -4.736916f, 2.3841f },
    { 154.4137f, 292.8956f, -4.683603f, 2.3841f },
    { 157.1544f, 294.2599f, -4.726504f, 2.3841f }
};

class spell_commander_sarannis_summon_reinforcements : public SpellScriptLoader
{
    public:
        spell_commander_sarannis_summon_reinforcements() : SpellScriptLoader("spell_commander_sarannis_summon_reinforcements") { }

        class spell_commander_sarannis_summon_reinforcements_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_commander_sarannis_summon_reinforcements_SpellScript);

            void HandleCast(SpellEffIndex /*effIndex*/)
            {
                GetCaster()->SummonCreature(NPC_SUMMONED_BLOODWARDER_MENDER, PosSummonReinforcements[0], TEMPSUMMON_CORPSE_DESPAWN);
                GetCaster()->SummonCreature(NPC_SUMMONED_BLOODWARDER_RESERVIST, PosSummonReinforcements[1], TEMPSUMMON_CORPSE_DESPAWN);
                GetCaster()->SummonCreature(NPC_SUMMONED_BLOODWARDER_RESERVIST, PosSummonReinforcements[2], TEMPSUMMON_CORPSE_DESPAWN);
                if (GetCaster()->GetMap()->IsHeroic())
                    GetCaster()->SummonCreature(NPC_SUMMONED_BLOODWARDER_RESERVIST, PosSummonReinforcements[3], TEMPSUMMON_CORPSE_DESPAWN);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_commander_sarannis_summon_reinforcements_SpellScript::HandleCast, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_commander_sarannis_summon_reinforcements_SpellScript();
        }
};

void AddSC_boss_commander_sarannis()
{
    new boss_commander_sarannis();
    new spell_commander_sarannis_summon_reinforcements();
}