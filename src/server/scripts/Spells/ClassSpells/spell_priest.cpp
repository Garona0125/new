/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"

enum PriestSpells
{
	ICON_ID_PAIN_AND_SUFFERING               = 2874,
	ICON_ID_EMPOWERED_RENEW_TALENT           = 3021,
    SPELL_PRIEST_GUARDIAN_SPIRIT_HEAL        = 48153,
    SPELL_PRIEST_PENANCE_R1                  = 47540,
    SPELL_PRIEST_PENANCE_R1_DAMAGE           = 47758,
    SPELL_PRIEST_PENANCE_R1_HEAL             = 47757,
    SPELL_PRIEST_REFLECTIVE_SHIELD_TRIGGERED = 33619,
    SPELL_PRIEST_REFLECTIVE_SHIELD_R1        = 33201,
	SPELL_PRIEST_VAMPIRIC_TOUCH_DISPEL       = 64085,
	SPELL_PRIEST_EMPOWERED_RENEW             = 63544,
	SPELL_PRIEST_SHADOW_WORD_DEATH           = 32409,
	SPELL_PRIEST_T9_HEALING_2_PIECE          = 67201,
	SPELL_PRIEST_HOLY_NOVA_RANK_1            = 15237,
	SPELL_PRIEST_HOLY_NOVA_RANK_2            = 15430,
	SPELL_PRIEST_HOLY_NOVA_RANK_3            = 15431,
	SPELL_PRIEST_HOLY_NOVA_RANK_4            = 27799,
	SPELL_PRIEST_HOLY_NOVA_RANK_5            = 27800,
	SPELL_PRIEST_HOLY_NOVA_RANK_6            = 27801,
	SPELL_PRIEST_HOLY_NOVA_RANK_7            = 25331,
	SPELL_PRIEST_HOLY_NOVA_RANK_8            = 48077,
	SPELL_PRIEST_HOLY_NOVA_RANK_9            = 48078,
};

class spell_pri_guardian_spirit : public SpellScriptLoader
{
    public:
        spell_pri_guardian_spirit() : SpellScriptLoader("spell_pri_guardian_spirit") { }

        class spell_pri_guardian_spirit_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_guardian_spirit_AuraScript);

            uint32 healPct;

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_GUARDIAN_SPIRIT_HEAL))
                    return false;
                return true;
            }

            bool Load()
            {
                healPct = GetSpellInfo()->Effects[EFFECT_1].CalcValue();
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
            {
                amount = -1;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                Unit* target = GetTarget();
                if (dmgInfo.GetDamage() < target->GetHealth())
                    return;

                int32 healAmount = int32(target->CountPctFromMaxHealth(healPct));
                Remove(AURA_REMOVE_BY_ENEMY_SPELL);
                target->CastCustomSpell(target, SPELL_PRIEST_GUARDIAN_SPIRIT_HEAL, &healAmount, NULL, NULL, true);
                absorbAmount = dmgInfo.GetDamage();
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_guardian_spirit_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pri_guardian_spirit_AuraScript::Absorb, EFFECT_1);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_guardian_spirit_AuraScript();
        }
};

class spell_pri_mana_burn : public SpellScriptLoader
{
    public:
        spell_pri_mana_burn() : SpellScriptLoader("spell_pri_mana_burn") { }

        class spell_pri_mana_burn_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_mana_burn_SpellScript);

            void HandleAfterHit()
            {
                if (Unit* unitTarget = GetHitUnit())
                    unitTarget->RemoveAurasWithMechanic((1 << MECHANIC_FEAR) | (1 << MECHANIC_POLYMORPH));
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_pri_mana_burn_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_mana_burn_SpellScript;
        }
};

class spell_pri_mind_sear : public SpellScriptLoader
{
    public:
        spell_pri_mind_sear() : SpellScriptLoader("spell_pri_mind_sear") { }

        class spell_pri_mind_sear_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_mind_sear_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if (Trinity::ObjectGUIDCheck(GetCaster()->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT)));
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_pri_mind_sear_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_mind_sear_SpellScript();
        }
};

class spell_pri_pain_and_suffering_proc : public SpellScriptLoader
{
    public:
        spell_pri_pain_and_suffering_proc() : SpellScriptLoader("spell_pri_pain_and_suffering_proc") { }

        class spell_pri_pain_and_suffering_proc_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_pain_and_suffering_proc_SpellScript);

            void HandleEffectScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (Unit* unitTarget = GetHitUnit())
				{
                    if (AuraEffect* aur = unitTarget->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELL_FAMILY_PRIEST, 0x8000, 0, 0, GetCaster()->GetGUID()))
                        aur->GetBase()->RefreshDuration();
				}
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_pain_and_suffering_proc_SpellScript::HandleEffectScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_pain_and_suffering_proc_SpellScript;
        }
};

class spell_pri_penance : public SpellScriptLoader
{
    public:
        spell_pri_penance() : SpellScriptLoader("spell_pri_penance") { }

        class spell_pri_penance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_penance_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            bool Validate(SpellInfo const* spellEntry)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_PENANCE_R1))
                    return false;
                // can't use other spell than this penance due to spell_ranks dependency
                if (sSpellMgr->GetFirstSpellInChain(SPELL_PRIEST_PENANCE_R1) != sSpellMgr->GetFirstSpellInChain(spellEntry->Id))
                    return false;

                uint8 rank = sSpellMgr->GetSpellRank(spellEntry->Id);
                if (!sSpellMgr->GetSpellWithRank(SPELL_PRIEST_PENANCE_R1_DAMAGE, rank, true))
                    return false;
                if (!sSpellMgr->GetSpellWithRank(SPELL_PRIEST_PENANCE_R1_HEAL, rank, true))
                    return false;

                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* unitTarget = GetHitUnit())
                {
                    if (!unitTarget->IsAlive())
                        return;

                    uint8 rank = sSpellMgr->GetSpellRank(GetSpellInfo()->Id);

                    if (caster->IsFriendlyTo(unitTarget))
                        caster->CastSpell(unitTarget, sSpellMgr->GetSpellWithRank(SPELL_PRIEST_PENANCE_R1_HEAL, rank), false, 0);
                    else
                        caster->CastSpell(unitTarget, sSpellMgr->GetSpellWithRank(SPELL_PRIEST_PENANCE_R1_DAMAGE, rank), false, 0);
                }
            }

            SpellCastResult CheckCast()
            {
                Player* caster = GetCaster()->ToPlayer();
				if (Unit* target = GetExplTargetUnit())
					if (!caster->IsFriendlyTo(target) && !caster->IsValidAttackTarget(target))
						return SPELL_FAILED_BAD_TARGETS;
                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_penance_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnCheckCast += SpellCheckCastFn(spell_pri_penance_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_penance_SpellScript;
        }
};

class spell_pri_reflective_shield_trigger : public SpellScriptLoader
{
    public:
        spell_pri_reflective_shield_trigger() : SpellScriptLoader("spell_pri_reflective_shield_trigger") { }

        class spell_pri_reflective_shield_trigger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_reflective_shield_trigger_AuraScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_REFLECTIVE_SHIELD_TRIGGERED) || !sSpellMgr->GetSpellInfo(SPELL_PRIEST_REFLECTIVE_SHIELD_R1))
                    return false;
                return true;
            }

            void Trigger(AuraEffect* aurEff, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                Unit* target = GetTarget();
                if (dmgInfo.GetAttacker() == target)
                    return;
                
                if (Unit* caster = GetCaster())
				{
                    if (AuraEffect* talentAurEff = target->GetAuraEffectOfRankedSpell(SPELL_PRIEST_REFLECTIVE_SHIELD_R1, EFFECT_0))
                    {
                        int32 bp = CalculatePctN(absorbAmount, talentAurEff->GetAmount());
                        target->CastCustomSpell(dmgInfo.GetAttacker(), SPELL_PRIEST_REFLECTIVE_SHIELD_TRIGGERED, &bp, NULL, NULL, true, NULL, aurEff);
                    }
				}
            }

            void Register()
            {
                 AfterEffectAbsorb += AuraEffectAbsorbFn(spell_pri_reflective_shield_trigger_AuraScript::Trigger, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_reflective_shield_trigger_AuraScript();
        }
};

class spell_pri_prayer_of_mending_heal : public SpellScriptLoader
{
public:
    spell_pri_prayer_of_mending_heal() : SpellScriptLoader("spell_pri_prayer_of_mending_heal") { }

    class spell_pri_prayer_of_mending_heal_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_prayer_of_mending_heal_SpellScript);

        void HandleHeal(SpellEffIndex /*effIndex*/)
        {
            if (Unit* caster = GetOriginalCaster())
            {
                if (AuraEffect* aurEff = caster->GetAuraEffect(SPELL_PRIEST_T9_HEALING_2_PIECE, EFFECT_0))
                {
                    int32 heal = GetHitHeal();
					AddPctN(heal, aurEff->GetAmount());
                    SetHitHeal(heal);
				}
			}
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_pri_prayer_of_mending_heal_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pri_prayer_of_mending_heal_SpellScript();
    }
};

class spell_pri_vampiric_touch : public SpellScriptLoader
{
    public:
        spell_pri_vampiric_touch() : SpellScriptLoader("spell_pri_vampiric_touch") { }

        class spell_pri_vampiric_touch_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_vampiric_touch_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_VAMPIRIC_TOUCH_DISPEL))
                    return false;
                return true;
            }

            void HandleDispel(DispelInfo* /*dispelInfo*/)
            {
                if (Unit* caster = GetCaster())
				{
                    if (Unit* target = GetUnitOwner())
					{
                        if (AuraEffect const* aurEff = GetEffect(EFFECT_1))
                        {
                            int32 damage = aurEff->GetAmount() * 8;
                            caster->CastCustomSpell(target, SPELL_PRIEST_VAMPIRIC_TOUCH_DISPEL, &damage, NULL, NULL, true, NULL, aurEff);
                        }
					}
				}
            }

            void Register()
            {
                AfterDispel += AuraDispelFn(spell_pri_vampiric_touch_AuraScript::HandleDispel);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pri_vampiric_touch_AuraScript();
        }
};

class spell_priest_renew : public SpellScriptLoader
{
    public:
        spell_priest_renew() : SpellScriptLoader("spell_priest_renew") { }

        class spell_priest_renew_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_priest_renew_AuraScript);

            bool Load()
            {
                return GetCaster() && GetCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            void HandleApplyEffect(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if (AuraEffect const* empoweredRenewAurEff = caster->GetDummyAuraEffect(SPELL_FAMILY_PRIEST, ICON_ID_EMPOWERED_RENEW_TALENT, EFFECT_1))
                    {
                        uint32 heal = caster->SpellHealingBonusDone(GetTarget(), GetSpellInfo(), GetEffect(EFFECT_0)->GetAmount(), DOT);
                        heal = GetTarget()->SpellHealingBonusTaken(caster, GetSpellInfo(), heal, DOT);

                        int32 basepoints0 = empoweredRenewAurEff->GetAmount() * GetEffect(EFFECT_0)->GetTotalTicks() * int32(heal) / 100;
                        caster->CastCustomSpell(GetTarget(), SPELL_PRIEST_EMPOWERED_RENEW, &basepoints0, NULL, NULL, true, NULL, aurEff);
                    }
                }
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_priest_renew_AuraScript::HandleApplyEffect, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_priest_renew_AuraScript();
        }
};

class spell_pri_shadow_word_death : public SpellScriptLoader
{
    public:
        spell_pri_shadow_word_death() : SpellScriptLoader("spell_pri_shadow_word_death") { }

        class spell_pri_shadow_word_death_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_shadow_word_death_SpellScript);

            void HandleDamage()
            {
                int32 damage = GetHitDamage();

                if (AuraEffect* aurEff = GetCaster()->GetDummyAuraEffect(SPELL_FAMILY_PRIEST, ICON_ID_PAIN_AND_SUFFERING, EFFECT_1))
                    AddPctN(damage, aurEff->GetAmount());

                GetCaster()->CastCustomSpell(GetCaster(), SPELL_PRIEST_SHADOW_WORD_DEATH, &damage, 0, 0, true);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_pri_shadow_word_death_SpellScript::HandleDamage);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pri_shadow_word_death_SpellScript();
        }
};

class spell_pri_holy_nova : public SpellScriptLoader
{
public:
	spell_pri_holy_nova() : SpellScriptLoader("spell_pri_holy_nova") { }

	class spell_pri_holy_nova_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_pri_holy_nova_SpellScript);

		bool Validate(SpellInfo const* /*spell*/)
		{
			if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_HOLY_NOVA_RANK_1))
				return false;
			if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_HOLY_NOVA_RANK_2))
				return false;
			if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_HOLY_NOVA_RANK_3))
				return false;
			if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_HOLY_NOVA_RANK_4))
				return false;
			if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_HOLY_NOVA_RANK_5))
				return false;
			if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_HOLY_NOVA_RANK_6))
				return false;
			if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_HOLY_NOVA_RANK_7))
				return false;
			if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_HOLY_NOVA_RANK_8))
				return false;
			if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_HOLY_NOVA_RANK_9))
				return false;
			return true;
		}

		void HandleEffectScriptEffect(SpellEffIndex /*effIndex*/)
		{
			Unit* caster = GetCaster();
			Unit* target = GetHitUnit();

			if (caster->GetGUID() != target->GetGUID() && caster->IsFriendlyTo(target) && (target->ToPlayer() != NULL && target->ToPlayer()->duel != NULL))
				return;
		}

		void Register()
		{
			OnEffectHitTarget += SpellEffectFn(spell_pri_holy_nova_SpellScript::HandleEffectScriptEffect, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_pri_holy_nova_SpellScript;
	}
};

void AddSC_priest_spell_scripts()
{
    new spell_pri_guardian_spirit();
    new spell_pri_mana_burn();
    new spell_pri_pain_and_suffering_proc();
    new spell_pri_penance();
    new spell_pri_reflective_shield_trigger();
    new spell_pri_mind_sear();
	new spell_pri_prayer_of_mending_heal();
	new spell_pri_vampiric_touch();
	new spell_priest_renew();
	new spell_pri_shadow_word_death();
	new spell_pri_holy_nova();
}