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

enum DeathKnightSpells
{
	ICON_ID_IMPROVED_DEATH_STRIKE               = 2751,
	SPELL_DK_MAGIC_SUPRESSION_RANK_1            = 49224,
	SPELL_DK_MAGIC_SUPRESSION_RANK_2            = 49610,
	SPELL_DK_MAGIC_SUPRESSION_RANK_3            = 49611,
    SPELL_DK_RUNIC_POWER_ENERGIZE               = 49088,
    SPELL_DK_ANTI_MAGIC_SHELL_TALENT            = 51052,
    SPELL_DK_CORPSE_EXPLOSION_TRIGGERED         = 43999,
    SPELL_DK_CORPSE_EXPLOSION_VISUAL            = 51270,
	SPELL_DK_DEATH_AND_DECAY_DAMAGE             = 52212,
    SPELL_DK_GHOUL_EXPLODE                      = 47496,
    SPELL_DK_SCOURGE_STRIKE_TRIGGERED           = 70890,
    SPELL_DK_BLOOD_BOIL_TRIGGERED               = 65658,
    SPELL_DK_WILL_OF_THE_NECROPOLIS_TALENT_R1   = 49189,
    SPELL_DK_WILL_OF_THE_NECROPOLIS_AURA_R1     = 52284,
    SPELL_DK_BLOOD_PRESENCE                     = 48266,
    SPELL_DK_IMPROVED_BLOOD_PRESENCE_TRIGGERED  = 63611,
    SPELL_DK_UNHOLY_PRESENCE                    = 48265,
    SPELL_DK_IMPROVED_UNHOLY_PRESENCE_TRIGGERED = 63622,
    SPELL_DK_ITEM_T8_MELEE_4P_BONUS             = 64736,
    SPELL_DK_BLACK_ICE_R1                       = 49140,
    SPELL_DK_DEATH_STRIKE_HEAL                  = 45470,
    SPELL_DK_DEATH_COIL_DAMAGE                  = 47632,
    SPELL_DK_DEATH_COIL_HEAL                    = 47633,
	SPELL_DK_DEATH_GRIP                         = 49560,
    SPELL_DK_SIGIL_VENGEFUL_HEART               = 64962,
	SPELL_DK_RUNE_TAP                           = 59754,
};

class spell_dk_anti_magic_shell_raid : public SpellScriptLoader
{
    public:
        spell_dk_anti_magic_shell_raid() : SpellScriptLoader("spell_dk_anti_magic_shell_raid") { }

        class spell_dk_anti_magic_shell_raid_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_anti_magic_shell_raid_AuraScript);

            uint32 absorbPct;

            bool Load()
            {
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
            {
                // TODO: this should absorb limited amount of damage, but no info on calculation formula
                amount = -1;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                 absorbAmount = CalculatePctN(dmgInfo.GetDamage(), absorbPct);
            }

            void Register()
            {
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_anti_magic_shell_raid_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                 OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_anti_magic_shell_raid_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_anti_magic_shell_raid_AuraScript();
        }
};

class spell_dk_anti_magic_shell_self : public SpellScriptLoader
{
    public:
        spell_dk_anti_magic_shell_self() : SpellScriptLoader("spell_dk_anti_magic_shell_self") { }

        class spell_dk_anti_magic_shell_self_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_anti_magic_shell_self_AuraScript);

            uint32 absorbPct, hpPct;
            bool Load()
            {
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
				if (GetCaster()->HasSpell(SPELL_DK_MAGIC_SUPRESSION_RANK_1))
					absorbPct += 8;
				if (GetCaster()->HasSpell(SPELL_DK_MAGIC_SUPRESSION_RANK_2))
					absorbPct += 16;
				if (GetCaster()->HasSpell(SPELL_DK_MAGIC_SUPRESSION_RANK_3))
					absorbPct += 25;
                hpPct = GetSpellInfo()->Effects[EFFECT_1].CalcValue(GetCaster());
                return true;
            }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_RUNIC_POWER_ENERGIZE))
					return false;
				return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
            {
				amount = GetCaster()->CountPctFromMaxHealth(hpPct);
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                absorbAmount = std::min(CalculatePctN(dmgInfo.GetDamage(), absorbPct), GetTarget()->CountPctFromMaxHealth(hpPct));
            }

            void Trigger(AuraEffect* aurEff, DamageInfo & /*dmgInfo*/, uint32 & absorbAmount)
            {
                Unit* target = GetTarget();
                int32 bp = absorbAmount * 2 / 10;
                target->CastCustomSpell(target, SPELL_DK_RUNIC_POWER_ENERGIZE, &bp, NULL, NULL, true, NULL, aurEff);
            }

            void Register()
            {
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_anti_magic_shell_self_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                 OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_anti_magic_shell_self_AuraScript::Absorb, EFFECT_0);
                 AfterEffectAbsorb += AuraEffectAbsorbFn(spell_dk_anti_magic_shell_self_AuraScript::Trigger, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_anti_magic_shell_self_AuraScript();
        }
};

class spell_dk_anti_magic_zone : public SpellScriptLoader
{
    public:
        spell_dk_anti_magic_zone() : SpellScriptLoader("spell_dk_anti_magic_zone") { }

        class spell_dk_anti_magic_zone_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_anti_magic_zone_AuraScript);

            uint32 absorbPct;

            bool Load()
            {
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
                return true;
            }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_ANTI_MAGIC_SHELL_TALENT))
					return false;
				return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
            {
                SpellInfo const* talentSpell = sSpellMgr->GetSpellInfo(SPELL_DK_ANTI_MAGIC_SHELL_TALENT);
                amount = talentSpell->Effects[EFFECT_0].CalcValue(GetCaster());
				if (Player* player = GetCaster()->ToPlayer())
                     amount += int32(2 * player->GetTotalAttackPowerValue(BASE_ATTACK));
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                 absorbAmount = CalculatePctN(dmgInfo.GetDamage(), absorbPct);
            }

            void Register()
            {
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_anti_magic_zone_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                 OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_anti_magic_zone_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_anti_magic_zone_AuraScript();
        }
};

class spell_dk_corpse_explosion : public SpellScriptLoader
{
    public:
        spell_dk_corpse_explosion() : SpellScriptLoader("spell_dk_corpse_explosion") { }

        class spell_dk_corpse_explosion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_corpse_explosion_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
				if (!sSpellMgr->GetSpellInfo(SPELL_DK_CORPSE_EXPLOSION_TRIGGERED) || !sSpellMgr->GetSpellInfo(SPELL_DK_GHOUL_EXPLODE))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_CORPSE_EXPLOSION_VISUAL))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* unitTarget = GetHitUnit())
                {
                    int32 bp = 0;
                    if (unitTarget->IsAlive())  // Living ghoul as a target
                    {
                        bp = int32(unitTarget->CountPctFromMaxHealth(HEALTH_PERCENT_25));
                        unitTarget->CastCustomSpell(unitTarget, SPELL_DK_GHOUL_EXPLODE, &bp, NULL, NULL, false);
                    }
                    else                        // Some corpse
                    {
                        bp = GetEffectValue();
                        GetCaster()->CastCustomSpell(unitTarget, GetSpellInfo()->Effects[EFFECT_1].CalcValue(), &bp, NULL, NULL, true);
                        // Corpse Explosion (Suicide)
                        unitTarget->CastSpell(unitTarget, SPELL_DK_CORPSE_EXPLOSION_TRIGGERED, true);
                    }
                    // Set corpse look
                    GetCaster()->CastSpell(unitTarget, SPELL_DK_CORPSE_EXPLOSION_VISUAL, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_corpse_explosion_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_corpse_explosion_SpellScript();
        }
};

class spell_dk_ghoul_explode : public SpellScriptLoader
{
    public:
        spell_dk_ghoul_explode() : SpellScriptLoader("spell_dk_ghoul_explode") { }

        class spell_dk_ghoul_explode_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_ghoul_explode_SpellScript);

			bool Validate(SpellInfo const* /*spellEntry*/)
			{
				if (!sSpellMgr->GetSpellInfo(SPELL_DK_CORPSE_EXPLOSION_TRIGGERED))
					return false;
				return true;
			}

            void Suicide(SpellEffIndex /*effIndex*/)
            {
                if (Unit* unitTarget = GetHitUnit())
					// Corpse Explosion (Suicide)
					unitTarget->CastSpell(unitTarget, SPELL_DK_CORPSE_EXPLOSION_TRIGGERED, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_ghoul_explode_SpellScript::Suicide, EFFECT_1, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_ghoul_explode_SpellScript();
        }
};

class spell_dk_death_gate : public SpellScriptLoader
{
    public:
        spell_dk_death_gate() : SpellScriptLoader("spell_dk_death_gate") {}

        class spell_dk_death_gate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_gate_SpellScript);

            SpellCastResult CheckClass()
            {
                if (GetCaster()->GetCurrentClass() != CLASS_DEATH_KNIGHT)
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_BE_DEATH_KNIGHT);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
				if (Unit* target = GetHitUnit())
					target->CastSpell(target, GetEffectValue(), false);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_death_gate_SpellScript::CheckClass);
                OnEffectHitTarget += SpellEffectFn(spell_dk_death_gate_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_gate_SpellScript();
        }
};

class spell_dk_death_pact : public SpellScriptLoader
{
    public:
        spell_dk_death_pact() : SpellScriptLoader("spell_dk_death_pact") { }

        class spell_dk_death_pact_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_pact_SpellScript);

            SpellCastResult CheckCast()
            {
                // Check if we have valid targets, otherwise skip spell casting here
                if (Player* player = GetCaster()->ToPlayer())
                    for (Unit::ControlList::const_iterator itr = player->m_Controlled.begin(); itr != player->m_Controlled.end(); ++itr)
                        if (Creature* undeadPet = (*itr)->ToCreature())
                            if (undeadPet->IsAlive() &&
                                undeadPet->GetOwnerGUID() == player->GetGUID() &&
                                undeadPet->GetCreatureType() == CREATURE_TYPE_UNDEAD &&
                                undeadPet->IsWithinDist(player, 100.0f, false))
                                return SPELL_CAST_OK;
				return SPELL_FAILED_NO_PET;
            }

            void FilterTargets(std::list<Unit*>& unitList)
            {
                Unit* unit_to_add = NULL;
                for (std::list<Unit*>::iterator itr = unitList.begin(); itr != unitList.end(); ++itr)
                {
                    if ((*itr)->GetTypeId() == TYPE_ID_UNIT
                        && (*itr)->GetOwnerGUID() == GetCaster()->GetGUID() && (*itr)->ToCreature()->GetCreatureTemplate()->type == CREATURE_TYPE_UNDEAD)
                    {
                        unit_to_add = (*itr);
                        break;
                    }
                }

                unitList.clear();
                if (unit_to_add)
                    unitList.push_back(unit_to_add);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_dk_death_pact_SpellScript::CheckCast);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_dk_death_pact_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_pact_SpellScript();
        }
};

class spell_dk_scourge_strike : public SpellScriptLoader
{
    public:
        spell_dk_scourge_strike() : SpellScriptLoader("spell_dk_scourge_strike") { }

        class spell_dk_scourge_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_scourge_strike_SpellScript);
            float multiplier;

            bool Load()
            {
                multiplier = 1.0f;
                return true;
            }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_SCOURGE_STRIKE_TRIGGERED))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* unitTarget = GetHitUnit())
				{
                    multiplier = (GetEffectValue() * unitTarget->GetDiseasesByCaster(caster->GetGUID()) / 100.f);
					// Death Knight T8 Melee 4P Bonus
					if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_DK_ITEM_T8_MELEE_4P_BONUS, EFFECT_0))
						AddPctF(multiplier, aurEff->GetAmount());
				}
            }

            void HandleAfterHit()
            {
                Unit* caster = GetCaster();
                if (Unit* unitTarget = GetHitUnit())
                {
                    int32 bp = GetHitDamage() * multiplier;

					if (AuraEffect* aurEff = caster->GetAuraEffectOfRankedSpell(SPELL_DK_BLACK_ICE_R1, EFFECT_0))
						AddPctN(bp, aurEff->GetAmount());

                    caster->CastCustomSpell(unitTarget, SPELL_DK_SCOURGE_STRIKE_TRIGGERED, &bp, NULL, NULL, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_scourge_strike_SpellScript::HandleDummy, EFFECT_2, SPELL_EFFECT_DUMMY);
                AfterHit += SpellHitFn(spell_dk_scourge_strike_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_scourge_strike_SpellScript();
        }
};

class spell_dk_spell_deflection : public SpellScriptLoader
{
    public:
        spell_dk_spell_deflection() : SpellScriptLoader("spell_dk_spell_deflection") { }

        class spell_dk_spell_deflection_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_spell_deflection_AuraScript);

            uint32 absorbPct;

            bool Load()
            {
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
            {
                amount = -1;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                if ((dmgInfo.GetDamageType() == SPELL_DIRECT_DAMAGE) && roll_chance_f(GetTarget()->GetUnitParryChance()))
                    absorbAmount = CalculatePctN(dmgInfo.GetDamage(), absorbPct);
            }

            void Register()
            {
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_spell_deflection_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                 OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_spell_deflection_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_spell_deflection_AuraScript();
        }
};

class spell_dk_blood_boil : public SpellScriptLoader
{
    public:
        spell_dk_blood_boil() : SpellScriptLoader("spell_dk_blood_boil") { }

        class spell_dk_blood_boil_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_blood_boil_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_BLOOD_BOIL_TRIGGERED))
                    return false;
                return true;
            }

            bool Load()
            {
                _executed = false;
                return GetCaster()->GetTypeId() == TYPE_ID_PLAYER && GetCaster()->GetCurrentClass() == CLASS_DEATH_KNIGHT;
            }

            void HandleAfterHit()
            {
                if (_executed || !GetHitUnit())
                    return;

				_executed = true;
				GetCaster()->CastSpell(GetCaster(), SPELL_DK_BLOOD_BOIL_TRIGGERED, true);
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_dk_blood_boil_SpellScript::HandleAfterHit);
            }

            bool _executed;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_blood_boil_SpellScript();
        }
};

class spell_dk_will_of_the_necropolis : public SpellScriptLoader
{
    public:
        spell_dk_will_of_the_necropolis() : SpellScriptLoader("spell_dk_will_of_the_necropolis") { }

        class spell_dk_will_of_the_necropolis_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_will_of_the_necropolis_AuraScript);

            bool Validate(SpellInfo const* spellEntry)
            {
				if (sSpellMgr->GetFirstSpellInChain(SPELL_DK_WILL_OF_THE_NECROPOLIS_AURA_R1) != sSpellMgr->GetFirstSpellInChain(spellEntry->Id))
					return false;

                uint8 rank = sSpellMgr->GetSpellRank(spellEntry->Id);
                if (!sSpellMgr->GetSpellWithRank(SPELL_DK_WILL_OF_THE_NECROPOLIS_TALENT_R1, rank, true))
                    return false;

                return true;
            }

            uint32 absorbPct;

            bool Load()
            {
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue(GetCaster());
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
            {
                amount = -1;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                uint32 rank = sSpellMgr->GetSpellRank(GetSpellInfo()->Id);
                SpellInfo const* talentProto = sSpellMgr->GetSpellInfo(sSpellMgr->GetSpellWithRank(SPELL_DK_WILL_OF_THE_NECROPOLIS_TALENT_R1, rank));

                int32 remainingHp = int32(GetTarget()->GetHealth() - dmgInfo.GetDamage());
                int32 minHp = int32(GetTarget()->CountPctFromMaxHealth(talentProto->Effects[EFFECT_0].CalcValue(GetCaster())));

                if (remainingHp < minHp)
                    absorbAmount = CalculatePctN(dmgInfo.GetDamage(), absorbPct);
            }

            void Register()
            {
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_dk_will_of_the_necropolis_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                 OnEffectAbsorb += AuraEffectAbsorbFn(spell_dk_will_of_the_necropolis_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_will_of_the_necropolis_AuraScript();
        }
};

class spell_dk_improved_blood_presence : public SpellScriptLoader
{
public:
    spell_dk_improved_blood_presence() : SpellScriptLoader("spell_dk_improved_blood_presence") { }

    class spell_dk_improved_blood_presence_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dk_improved_blood_presence_AuraScript);

        bool Validate(SpellInfo const* /*entry*/)
        {
			if (!sSpellMgr->GetSpellInfo(SPELL_DK_BLOOD_PRESENCE) || !sSpellMgr->GetSpellInfo(SPELL_DK_IMPROVED_BLOOD_PRESENCE_TRIGGERED))
                return false;
            return true;
        }

        void HandleEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            if (!target->HasAura(SPELL_DK_BLOOD_PRESENCE) && !target->HasAura(SPELL_DK_IMPROVED_BLOOD_PRESENCE_TRIGGERED))
            {
                int32 basePoints1 = aurEff->GetAmount();
                target->CastCustomSpell(target, SPELL_DK_IMPROVED_BLOOD_PRESENCE_TRIGGERED, NULL, &basePoints1, NULL, true, 0, aurEff);
            }
        }

        void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            if (!target->HasAura(SPELL_DK_BLOOD_PRESENCE))
			{
                target->RemoveAura(SPELL_DK_IMPROVED_BLOOD_PRESENCE_TRIGGERED);
			}
        }

        void Register()
        {
            AfterEffectApply += AuraEffectApplyFn(spell_dk_improved_blood_presence_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            AfterEffectRemove += AuraEffectRemoveFn(spell_dk_improved_blood_presence_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_dk_improved_blood_presence_AuraScript();
    }
};

class spell_dk_improved_unholy_presence : public SpellScriptLoader
{
public:
    spell_dk_improved_unholy_presence() : SpellScriptLoader("spell_dk_improved_unholy_presence") { }

    class spell_dk_improved_unholy_presence_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_dk_improved_unholy_presence_AuraScript);

        bool Validate(SpellInfo const* /*entry*/)
        {
			if (!sSpellMgr->GetSpellInfo(SPELL_DK_UNHOLY_PRESENCE) || !sSpellMgr->GetSpellInfo(SPELL_DK_IMPROVED_UNHOLY_PRESENCE_TRIGGERED))
                return false;
            return true;
        }

        void HandleEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            if (target->HasAura(SPELL_DK_UNHOLY_PRESENCE) && !target->HasAura(SPELL_DK_IMPROVED_UNHOLY_PRESENCE_TRIGGERED))
            {
                int32 basePoints0 = aurEff->GetSpellInfo()->Effects[EFFECT_1].CalcValue();
                target->CastCustomSpell(target, SPELL_DK_IMPROVED_UNHOLY_PRESENCE_TRIGGERED, &basePoints0, &basePoints0, &basePoints0, true, 0, aurEff);
            }
        }

        void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
			GetTarget()->RemoveAura(SPELL_DK_IMPROVED_UNHOLY_PRESENCE_TRIGGERED);
        }

        void Register()
        {
            AfterEffectApply += AuraEffectApplyFn(spell_dk_improved_unholy_presence_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            AfterEffectRemove += AuraEffectRemoveFn(spell_dk_improved_unholy_presence_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_dk_improved_unholy_presence_AuraScript();
    }
};

class spell_dk_death_strike : public SpellScriptLoader
{
    public:
        spell_dk_death_strike() : SpellScriptLoader("spell_dk_death_strike") { }

        class spell_dk_death_strike_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_strike_SpellScript);

            bool Validate(SpellInfo const* /*SpellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_DEATH_STRIKE_HEAL))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
                {
                    uint32 count = target->GetDiseasesByCaster(caster->GetGUID());
                    int32 bp = int32(count * caster->CountPctFromMaxHealth(int32(GetSpellInfo()->Effects[EFFECT_0].DamageMultiplier)));
                    // Improved Death Strike

                    if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_AURA_ADD_PCT_MODIFIER, SPELL_FAMILY_DEATH_KNIGHT, ICON_ID_IMPROVED_DEATH_STRIKE, 0))
                        AddPctN(bp, caster->CalculateSpellDamage(caster, aurEff->GetSpellInfo(), 2));

                    caster->CastCustomSpell(caster, SPELL_DK_DEATH_STRIKE_HEAL, &bp, NULL, NULL, false);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_death_strike_SpellScript::HandleDummy, EFFECT_2, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_strike_SpellScript();
        }
};

class spell_dk_death_coil : public SpellScriptLoader
{
    public:
        spell_dk_death_coil() : SpellScriptLoader("spell_dk_death_coil") { }

        class spell_dk_death_coil_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_coil_SpellScript);

			bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DK_DEATH_COIL_DAMAGE) || !sSpellMgr->GetSpellInfo(SPELL_DK_DEATH_COIL_HEAL))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                int32 damage = GetEffectValue();
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
                {
                    if (caster->IsFriendlyTo(target))
                    {
                        int32 bp = int32(damage * 1.5f);
                        caster->CastCustomSpell(target, SPELL_DK_DEATH_COIL_HEAL, &bp, NULL, NULL, true);
                    }
                    else
                    {
                        if (AuraEffect const* auraEffect = caster->GetAuraEffect(SPELL_DK_SIGIL_VENGEFUL_HEART, EFFECT_1))
                            damage += auraEffect->GetBaseAmount();

                        caster->CastCustomSpell(target, SPELL_DK_DEATH_COIL_DAMAGE, &damage, NULL, NULL, true);
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_death_coil_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_coil_SpellScript();
        }
};

class spell_dk_death_grip : public SpellScriptLoader
{
    public:
        spell_dk_death_grip() : SpellScriptLoader("spell_dk_death_grip") { }

        class spell_dk_death_grip_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dk_death_grip_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                int32 damage = GetEffectValue();
                Position const* pos = GetExplTargetDest();
                if (Unit* target = GetHitUnit())
                {
                    if (!target->HasAuraType(SPELL_AURA_DEFLECT_SPELLS))
                        target->CastSpell(pos->GetPositionX(), pos->GetPositionY(), pos->GetPositionZ(), damage, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dk_death_grip_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dk_death_grip_SpellScript();
        }
};

class spell_dk_death_grip_initial : public SpellScriptLoader
{
public:
    spell_dk_death_grip_initial() : SpellScriptLoader("spell_dk_death_grip_initial") { }

    class spell_dk_death_grip_initial_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_dk_death_grip_initial_SpellScript);

        SpellCastResult CheckCast()
        {
            Unit* caster = GetCaster();
            // Death Grip should not be castable while jumping/falling
            if (caster->HasUnitState(UNIT_STATE_JUMPING) || caster->HasUnitMovementFlag(MOVEMENTFLAG_FALLING))
                return SPELL_FAILED_MOVING;

            // Patch 3.3.3 (2010-03-23): Minimum range has been changed to 8 yards in PvP.
            Unit* target = GetExplTargetUnit();
            if (target && target->GetTypeId() == TYPE_ID_PLAYER)
                if (caster->GetDistance(target) < 8.f)
                    return SPELL_FAILED_TOO_CLOSE;

            return SPELL_CAST_OK;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
			Unit* victim = GetHitUnit();

			//Spell reflection : The hit unit will be same as caster and CastSpell will do nothing. Use the original target as caster.
			if (caster == victim)
				caster = GetExplTargetUnit();

			caster->CastSpell(victim, SPELL_DK_DEATH_GRIP, true);
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_dk_death_grip_initial_SpellScript::CheckCast);
            OnEffectHitTarget += SpellEffectFn(spell_dk_death_grip_initial_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_dk_death_grip_initial_SpellScript();
    }
};

class spell_dk_death_and_decay : public SpellScriptLoader
{
    public:
        spell_dk_death_and_decay() : SpellScriptLoader("spell_dk_death_and_decay") { }

        class spell_dk_death_and_decay_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dk_death_and_decay_AuraScript);

            void HandleDummyTick(AuraEffect const* aurEff)
            {
                if (Unit* caster = GetCaster())
                    caster->CastCustomSpell(SPELL_DK_DEATH_AND_DECAY_DAMAGE, SPELLVALUE_BASE_POINT0, aurEff->GetAmount(), GetTarget(), true, NULL, aurEff);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dk_death_and_decay_AuraScript::HandleDummyTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dk_death_and_decay_AuraScript();
        }
};

class spell_dk_rune_tap : public SpellScriptLoader
{
public:
	spell_dk_rune_tap() : SpellScriptLoader("spell_dk_rune_tap") { }

	class spell_dk_rune_tap_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_dk_rune_tap_SpellScript);

		bool Validate(SpellInfo const* /*spell*/)
		{
			if (!sSpellMgr->GetSpellInfo(SPELL_DK_RUNE_TAP))
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
			OnEffectHitTarget += SpellEffectFn(spell_dk_rune_tap_SpellScript::HandleEffectScriptEffect, EFFECT_0, SPELL_EFFECT_HEAL_PCT);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_dk_rune_tap_SpellScript;
	}
};

void AddSC_deathknight_spell_scripts()
{
    new spell_dk_anti_magic_shell_raid();
    new spell_dk_anti_magic_shell_self();
    new spell_dk_anti_magic_zone();
    new spell_dk_corpse_explosion();
    new spell_dk_ghoul_explode();
    new spell_dk_death_gate();
    new spell_dk_death_pact();
    new spell_dk_scourge_strike();
    new spell_dk_spell_deflection();
    new spell_dk_blood_boil();
    new spell_dk_will_of_the_necropolis();
    new spell_dk_improved_blood_presence();
    new spell_dk_improved_unholy_presence();
	new spell_dk_death_strike();
	new spell_dk_death_coil();
	new spell_dk_death_grip();
	new spell_dk_death_grip_initial();
	new spell_dk_death_and_decay();
	new spell_dk_rune_tap();
}