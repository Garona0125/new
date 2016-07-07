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
#include "SpellAuraEffects.h"
#include "SpellScript.h"

enum Yells
{
    SAY_INTRO       = 0,
    SAY_AGGRO       = 1,
    SAY_SURPREME    = 2,
    SAY_KILL        = 3,
    SAY_DEATH       = 4,
    EMOTE_FRENZY    = 5,
    SAY_RAND        = 6,
};

enum Spells
{
    SPELL_SHADOW_VOLLEY         = 32963,
    SPELL_CLEAVE                = 31779,
    SPELL_THUNDERCLAP           = 36706,
    SPELL_VOID_BOLT             = 39329,
    SPELL_MARK_OF_KAZZAK        = 32960,
    SPELL_MARK_OF_KAZZAK_DAMAGE = 32961,
    SPELL_ENRAGE                = 32964,
    SPELL_CAPTURE_SOUL          = 32966,
    SPELL_TWISTED_REFLECTION    = 21063,
	SPELL_BERSERK               = 32965,
};

enum Events
{
    EVENT_SHADOW_VOLLEY         = 1,
    EVENT_CLEAVE                = 2,
    EVENT_THUNDERCLAP           = 3,
    EVENT_VOID_BOLT             = 4,
    EVENT_MARK_OF_KAZZAK        = 5,
    EVENT_ENRAGE                = 6,
	EVENT_TWISTED_REFLECTION    = 7,
	EVENT_BERSERK               = 8,
};

class boss_doomlord_kazzak : public CreatureScript
{
    public:
        boss_doomlord_kazzak() : CreatureScript("boss_doomlord_kazzak") { }

        struct boss_doomlordkazzakAI : public QuantumBasicAI
        {
            boss_doomlordkazzakAI(Creature* creature) : QuantumBasicAI(creature) {}

			EventMap events;

            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_SHADOW_VOLLEY, urand(6000, 10000));
                events.ScheduleEvent(EVENT_CLEAVE, 7000);
                events.ScheduleEvent(EVENT_THUNDERCLAP, urand(14000, 18000));
                events.ScheduleEvent(EVENT_VOID_BOLT, 30000);
                events.ScheduleEvent(EVENT_MARK_OF_KAZZAK, 25000);
                events.ScheduleEvent(EVENT_ENRAGE, 60000);
                events.ScheduleEvent(EVENT_TWISTED_REFLECTION, 33000);
				events.ScheduleEvent(EVENT_BERSERK, 180000);
            }

            void JustRespawned()
            {
                Talk(SAY_INTRO);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                Talk(SAY_AGGRO);
            }

            void KilledUnit(Unit* victim)
            {
                if (victim->GetTypeId() != TYPE_ID_PLAYER)
                    return;

                DoCast(me, SPELL_CAPTURE_SOUL);
                Talk(SAY_KILL);
            }

            void JustDied(Unit* /*victim*/)
            {
                Talk(SAY_DEATH);
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
                        case EVENT_SHADOW_VOLLEY:
                            DoCastVictim(SPELL_SHADOW_VOLLEY);
                            events.ScheduleEvent(EVENT_SHADOW_VOLLEY, urand(4000, 6000));
                            break;
                        case EVENT_CLEAVE:
                            DoCastVictim(SPELL_CLEAVE);
                            events.ScheduleEvent(EVENT_CLEAVE, urand(8000, 12000));
                            break;
                        case EVENT_THUNDERCLAP:
                            DoCastVictim(SPELL_THUNDERCLAP);
                            events.ScheduleEvent(EVENT_THUNDERCLAP, urand(10000, 14000));
                            break;
                        case EVENT_VOID_BOLT:
                            DoCastVictim(SPELL_VOID_BOLT);
                            events.ScheduleEvent(EVENT_VOID_BOLT, urand(15000, 18000));
                            break;
                        case EVENT_MARK_OF_KAZZAK:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 0.0f, true))
							{
                                DoCast(target, SPELL_MARK_OF_KAZZAK);
							}
                            events.ScheduleEvent(EVENT_MARK_OF_KAZZAK, 20000);
                            break;
                        case EVENT_ENRAGE:
                            Talk(EMOTE_FRENZY);
                            DoCast(me, SPELL_ENRAGE);
                            events.ScheduleEvent(EVENT_ENRAGE, 30000);
                            break;
                        case EVENT_TWISTED_REFLECTION:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 0.0f, true))
							{
                                DoCast(target, SPELL_TWISTED_REFLECTION);
							}
                            events.ScheduleEvent(EVENT_TWISTED_REFLECTION, 15000);
                            break;
						case EVENT_BERSERK:
							DoCast(me, SPELL_BERSERK);
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
            return new boss_doomlordkazzakAI(creature);
        }
};

class spell_mark_of_kazzak : public SpellScriptLoader
{
    public:
        spell_mark_of_kazzak() : SpellScriptLoader("spell_mark_of_kazzak") { }

        class spell_mark_of_kazzak_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mark_of_kazzak_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_MARK_OF_KAZZAK_DAMAGE))
                    return false;
                return true;
            }

            void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (Unit* owner = GetUnitOwner())
                    amount = CalculatePctU(owner->GetPower(POWER_MANA), 5);
            }

            void OnPeriodic(AuraEffect const* aurEff)
            {
                Unit* target = GetTarget();

                if (target->GetPower(POWER_MANA) == 0)
                {
                    target->CastSpell(target, SPELL_MARK_OF_KAZZAK_DAMAGE, true, NULL, aurEff);
                    SetDuration(0);
                }
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mark_of_kazzak_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_MANA_LEECH);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mark_of_kazzak_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_MANA_LEECH);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_mark_of_kazzak_AuraScript();
        }
};

void AddSC_boss_doomlordkazzak()
{
    new boss_doomlord_kazzak();
	new spell_mark_of_kazzak();
}