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

enum WarlockSpells
{
	ICON_ID_IMPROVED_LIFE_TAP                    = 208,
    ICON_ID_MANA_FEED                            = 1982,
    SPELL_WARLOCK_DEMONIC_EMPOWERMENT_SUCCUBUS   = 54435,
    SPELL_WARLOCK_DEMONIC_EMPOWERMENT_VOIDWALKER = 54443,
    SPELL_WARLOCK_DEMONIC_EMPOWERMENT_FELGUARD   = 54508,
    SPELL_WARLOCK_DEMONIC_EMPOWERMENT_FELHUNTER  = 54509,
    SPELL_WARLOCK_DEMONIC_EMPOWERMENT_IMP        = 54444,
    SPELL_WARLOCK_IMPROVED_HEALTHSTONE_R1        = 18692,
    SPELL_WARLOCK_IMPROVED_HEALTHSTONE_R2        = 18693,
	SPELL_WARLOCK_SOULSHATTER                    = 32835,
	SPELL_WARLOCK_LIFE_TAP_ENERGIZE_1            = 31818,
    SPELL_WARLOCK_LIFE_TAP_ENERGIZE_2            = 32553,
	SPELL_WARLOCK_DEMONIC_CIRCLE_SUMMON          = 48018,
	SPELL_WARLOCK_DEMONIC_CIRCLE_TELEPORT        = 48020,
	SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST      = 62388,
	SPELL_WARLOCK_HAUNT                          = 48181,
	SPELL_WARLOCK_HAUNT_HEAL                     = 48210,
	SPELL_WARLOCK_UNSTABLE_AFFLICTION_DISPEL     = 31117,
	SPELL_WARLOCK_CURSE_OF_DOOM_EFFECT           = 18662,
	SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_R1      = 18703,
	SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_R2      = 18704,
	SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R1 = 60955,
	SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R2 = 60956,
	SPELL_WARLOCK_FELHUNTER_SHADOWBITE_R1        = 54049,
    SPELL_WARLOCK_FELHUNTER_SHADOWBITE_R2        = 54050,
    SPELL_WARLOCK_FELHUNTER_SHADOWBITE_R3        = 54051,
    SPELL_WARLOCK_FELHUNTER_SHADOWBITE_R4        = 54052,
    SPELL_WARLOCK_FELHUNTER_SHADOWBITE_R5        = 54053,
    SPELL_WARLOCK_IMPROVED_FELHUNTER_R1          = 54037,
    SPELL_WARLOCK_IMPROVED_FELHUNTER_R2          = 54038,
    SPELL_WARLOCK_IMPROVED_FELHUNTER_EFFECT      = 54425,
};

class spell_warl_banish : public SpellScriptLoader
{
public:
    spell_warl_banish() : SpellScriptLoader("spell_warl_banish") { }

    class spell_warl_banish_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warl_banish_SpellScript);

        bool Load()
        {
            _removed = false;
            return true;
        }

        void HandleBanish()
        {
            if (Unit* target = GetHitUnit())
            {
                if (target->GetAuraEffect(SPELL_AURA_SCHOOL_IMMUNITY, SPELL_FAMILY_WARLOCK, 0, 0x08000000, 0))
                {
                    //No need to remove old aura since its removed due to not stack by current Banish aura
                    PreventHitDefaultEffect(EFFECT_0);
                    PreventHitDefaultEffect(EFFECT_1);
                    PreventHitDefaultEffect(EFFECT_2);
                    _removed = true;
                }
            }
        }

        void RemoveAura()
        {
            if (_removed)
                PreventHitAura();
        }

        void Register()
        {
            BeforeHit += SpellHitFn(spell_warl_banish_SpellScript::HandleBanish);
            AfterHit += SpellHitFn(spell_warl_banish_SpellScript::RemoveAura);
        }

        bool _removed;
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_warl_banish_SpellScript();
    }
};

class spell_warl_shadow_bite : public SpellScriptLoader
{
public:
    spell_warl_shadow_bite() : SpellScriptLoader("spell_warl_shadow_bite") { }

    class spell_warl_shadow_bite_SpellScript : public SpellScript
    {
		PrepareSpellScript(spell_warl_shadow_bite_SpellScript)

        bool Validate(SpellInfo const * /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_WARLOCK_FELHUNTER_SHADOWBITE_R1))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_WARLOCK_FELHUNTER_SHADOWBITE_R2))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_WARLOCK_FELHUNTER_SHADOWBITE_R3))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_WARLOCK_FELHUNTER_SHADOWBITE_R4))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_WARLOCK_FELHUNTER_SHADOWBITE_R5))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_WARLOCK_IMPROVED_FELHUNTER_R1))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_WARLOCK_IMPROVED_FELHUNTER_R2))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_WARLOCK_IMPROVED_FELHUNTER_EFFECT))
                return false;

            return true;
        }

        void HandleScriptEffect(SpellEffIndex /*effIndex*/)
        {
            //Unit *caster = GetCaster();
            // Get DoTs on target by owner (15% increase by dot)
            // need to get this here from SpellEffects.cpp ?
            // damage *= float(100.f + 15.f * caster->getVictim()->GetDoTsByCaster(caster->GetOwnerGUID())) / 100.f;
        }

        void HandleAfterHitEffect()
        {
            Unit *caster = GetCaster();

            if (!caster)
			{
				return;
			};

            if (!(caster->GetTypeId() == TYPE_ID_UNIT && caster->ToCreature()->IsPet()))
			{
				return;
			};

            Unit *owner = caster->GetOwner();
            if (!(owner && (owner->GetTypeId() == TYPE_ID_PLAYER)))
			{
				return;
			};
            
            int32 amount;

            if (owner->HasAura(SPELL_WARLOCK_IMPROVED_FELHUNTER_R1))
			{
				amount = 5;
			};

            if (owner->HasAura(SPELL_WARLOCK_IMPROVED_FELHUNTER_R2))
			{
				amount = 9;
			};

            if (AuraEffect * aurEff = owner->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELL_FAMILY_WARLOCK, 214, 0))
                caster->CastCustomSpell(caster, SPELL_WARLOCK_IMPROVED_FELHUNTER_EFFECT, &amount, NULL, NULL, true, NULL, aurEff, caster->GetGUID());
        }

        void Register()
        {
			//OnEffectHitTarget += SpellEffectFn(spell_warl_shadow_bite_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            AfterHit += SpellHitFn(spell_warl_shadow_bite_SpellScript::HandleAfterHitEffect);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_warl_shadow_bite_SpellScript();
    }
};

class spell_warl_demonic_empowerment : public SpellScriptLoader
{
    public:
        spell_warl_demonic_empowerment() : SpellScriptLoader("spell_warl_demonic_empowerment") { }

        class spell_warl_demonic_empowerment_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_demonic_empowerment_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_WARLOCK_DEMONIC_EMPOWERMENT_SUCCUBUS)
					|| !sSpellMgr->GetSpellInfo(SPELL_WARLOCK_DEMONIC_EMPOWERMENT_VOIDWALKER)
					|| !sSpellMgr->GetSpellInfo(SPELL_WARLOCK_DEMONIC_EMPOWERMENT_FELGUARD)
					|| !sSpellMgr->GetSpellInfo(SPELL_WARLOCK_DEMONIC_EMPOWERMENT_FELHUNTER)
					|| !sSpellMgr->GetSpellInfo(SPELL_WARLOCK_DEMONIC_EMPOWERMENT_IMP))
                    return false;
                return true;
            }

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (Creature* targetCreature = GetHitCreature())
                {
                    if (targetCreature->IsPet())
                    {
                        CreatureTemplate const* ci = sObjectMgr->GetCreatureTemplate(targetCreature->GetEntry());

                        switch (ci->family)
                        {
                        case CREATURE_FAMILY_SUCCUBUS:
                            targetCreature->CastSpell(targetCreature, SPELL_WARLOCK_DEMONIC_EMPOWERMENT_SUCCUBUS, true);
                            break;
                        case CREATURE_FAMILY_VOIDWALKER:
                        {
                            SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_WARLOCK_DEMONIC_EMPOWERMENT_VOIDWALKER);
                            int32 hp = int32(targetCreature->CountPctFromMaxHealth(GetCaster()->CalculateSpellDamage(targetCreature, spellInfo, 0)));
                            targetCreature->CastCustomSpell(targetCreature, SPELL_WARLOCK_DEMONIC_EMPOWERMENT_VOIDWALKER, &hp, NULL, NULL, true);
                            //unitTarget->CastSpell(unitTarget, 54441, true);
                            break;
                        }
                        case CREATURE_FAMILY_FELGUARD:
                            targetCreature->CastSpell(targetCreature, SPELL_WARLOCK_DEMONIC_EMPOWERMENT_FELGUARD, true);
                            break;
                        case CREATURE_FAMILY_FELHUNTER:
                            targetCreature->CastSpell(targetCreature, SPELL_WARLOCK_DEMONIC_EMPOWERMENT_FELHUNTER, true);
                            break;
                        case CREATURE_FAMILY_IMP:
                            targetCreature->CastSpell(targetCreature, SPELL_WARLOCK_DEMONIC_EMPOWERMENT_IMP, true);
                            break;
                        }
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warl_demonic_empowerment_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_demonic_empowerment_SpellScript();
        }
};

class spell_warl_create_healthstone : public SpellScriptLoader
{
    public:
        spell_warl_create_healthstone() : SpellScriptLoader("spell_warl_create_healthstone") { }

        class spell_warl_create_healthstone_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_create_healthstone_SpellScript);

            static uint32 const iTypes[8][3];

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_WARLOCK_IMPROVED_HEALTHSTONE_R1) || !sSpellMgr->GetSpellInfo(SPELL_WARLOCK_IMPROVED_HEALTHSTONE_R2))
                    return false;
                return true;
            }

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                if (Unit* unitTarget = GetHitUnit())
                {
                    uint32 rank = 0;

                    if (AuraEffect const* aurEff = unitTarget->GetDummyAuraEffect(SPELL_FAMILY_WARLOCK, 284, 0))
                    {
                        switch (aurEff->GetId())
                        {
                            case SPELL_WARLOCK_IMPROVED_HEALTHSTONE_R1:
								rank = 1;
								break;
                            case SPELL_WARLOCK_IMPROVED_HEALTHSTONE_R2:
								rank = 2;
								break;
                            default:
                                sLog->OutErrorConsole("Unknown rank of Improved Healthstone id: %d", aurEff->GetId());
                                break;
                        }
                    }
                    uint8 spellRank = sSpellMgr->GetSpellRank(GetSpellInfo()->Id);
                    if (spellRank > 0 && spellRank <= 8)
                        CreateItem(effIndex, iTypes[spellRank - 1][rank]);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warl_create_healthstone_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_create_healthstone_SpellScript();
        }
};

uint32 const spell_warl_create_healthstone::spell_warl_create_healthstone_SpellScript::iTypes[8][3] =
{
	{ 5512, 19004, 19005},              // Minor Healthstone
    { 5511, 19006, 19007},              // Lesser Healthstone
    { 5509, 19008, 19009},              // Healthstone
    { 5510, 19010, 19011},              // Greater Healthstone
    { 9421, 19012, 19013},              // Major Healthstone
    {22103, 22104, 22105},              // Master Healthstone
    {36889, 36890, 36891},              // Demonic Healthstone
    {36892, 36893, 36894}               // Fel Healthstone
};

class spell_warl_everlasting_affliction : public SpellScriptLoader
{
    public:
        spell_warl_everlasting_affliction() : SpellScriptLoader("spell_warl_everlasting_affliction") { }

        class spell_warl_everlasting_affliction_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_everlasting_affliction_SpellScript);

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (Unit* unitTarget = GetHitUnit())
                    // Refresh corruption on target
                    if (AuraEffect* aur = unitTarget->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELL_FAMILY_WARLOCK, 0x2, 0, 0, GetCaster()->GetGUID()))
                        aur->GetBase()->RefreshDuration();
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warl_everlasting_affliction_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_everlasting_affliction_SpellScript();
        }
};

class spell_warl_ritual_of_doom_effect : public SpellScriptLoader
{
public:
    spell_warl_ritual_of_doom_effect() : SpellScriptLoader("spell_warl_ritual_of_doom_effect") { }

    class spell_warl_ritual_of_doom_effect_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_warl_ritual_of_doom_effect_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            caster->CastSpell(caster, GetEffectValue(), true);
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_warl_ritual_of_doom_effect_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_warl_ritual_of_doom_effect_SpellScript();
    }
};

class spell_warl_seed_of_corruption : public SpellScriptLoader
{
    public:
        spell_warl_seed_of_corruption() : SpellScriptLoader("spell_warl_seed_of_corruption") { }

        class spell_warl_seed_of_corruption_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_seed_of_corruption_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
				if (GetExplTargetUnit())
					unitList.remove(GetExplTargetUnit());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_warl_seed_of_corruption_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_seed_of_corruption_SpellScript();
        }
};

class spell_warl_soulshatter : public SpellScriptLoader
{
    public:
        spell_warl_soulshatter() : SpellScriptLoader("spell_warl_soulshatter") { }

        class spell_warl_soulshatter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_soulshatter_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_WARLOCK_SOULSHATTER))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
                {
                    if (target->CanHaveThreatList() && target->getThreatManager().getThreat(caster) > 0.0f)
                        caster->CastSpell(target, SPELL_WARLOCK_SOULSHATTER, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warl_soulshatter_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_soulshatter_SpellScript();
        }
};

class spell_warl_life_tap : public SpellScriptLoader
{
    public:
        spell_warl_life_tap() : SpellScriptLoader("spell_warl_life_tap") { }

        class spell_warl_life_tap_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_life_tap_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_WARLOCK_LIFE_TAP_ENERGIZE_1) || !sSpellMgr->GetSpellInfo(SPELL_WARLOCK_LIFE_TAP_ENERGIZE_2))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Player* caster = GetCaster()->ToPlayer();
                if (Unit* target = GetHitUnit())
                {
                    int32 damage = GetEffectValue();
                    int32 mana = int32(damage + (caster->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS+SPELL_SCHOOL_SHADOW) * 0.5f));

                    // Shouldn't Appear in Combat Log
                    target->ModifyHealth(-damage);

                    // Improved Life Tap mod
                    if (AuraEffect const* aurEff = caster->GetDummyAuraEffect(SPELL_FAMILY_WARLOCK, ICON_ID_IMPROVED_LIFE_TAP, 0))
                        AddPctN(mana, aurEff->GetAmount());

                    caster->CastCustomSpell(target, SPELL_WARLOCK_LIFE_TAP_ENERGIZE_1, &mana, NULL, NULL, false);

                    // Mana Feed
                    int32 manaFeedVal = 0;
                    if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELL_FAMILY_WARLOCK, ICON_ID_MANA_FEED, 0))
                        manaFeedVal = aurEff->GetAmount();

                    if (manaFeedVal > 0)
                    {
                        ApplyPctN(manaFeedVal, mana);
                        caster->CastCustomSpell(caster, SPELL_WARLOCK_LIFE_TAP_ENERGIZE_2, &manaFeedVal, NULL, NULL, true, NULL);
                    }
                }
            }

            SpellCastResult CheckCast()
            {
                if ((int32(GetCaster()->GetHealth()) > int32(GetSpellInfo()->Effects[EFFECT_0].CalcValue() + (6.3875 * GetSpellInfo()->BaseLevel))))
                    return SPELL_CAST_OK;
                return SPELL_FAILED_FIZZLE;
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_warl_life_tap_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnCheckCast += SpellCheckCastFn(spell_warl_life_tap_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_life_tap_SpellScript();
        }
};

class spell_warl_demonic_circle_summon : public SpellScriptLoader
{
    public:
        spell_warl_demonic_circle_summon() : SpellScriptLoader("spell_warl_demonic_circle_summon") { }

        class spell_warl_demonic_circle_summon_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_demonic_circle_summon_AuraScript);

            void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes mode)
            {
                if (!(mode & AURA_EFFECT_HANDLE_REAPPLY))
                    GetTarget()->RemoveGameObject(GetId(), true);

                GetTarget()->RemoveAura(SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST);
            }

            void HandleDummyTick(AuraEffect const* /*aurEff*/)
            {
                if (GameObject* circle = GetTarget()->GetGameObject(GetId()))
                {
                    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(SPELL_WARLOCK_DEMONIC_CIRCLE_TELEPORT);

                    if (GetTarget()->IsWithinDist(circle, spellInfo->GetMaxRange(true)))
                    {
                        if (!GetTarget()->HasAura(SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST))
                            GetTarget()->CastSpell(GetTarget(), SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST, true);
                    }
                    else
                        GetTarget()->RemoveAura(SPELL_WARLOCK_DEMONIC_CIRCLE_ALLOW_CAST);
                }
            }

            void Register()
            {
                OnEffectRemove += AuraEffectApplyFn(spell_warl_demonic_circle_summon_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_warl_demonic_circle_summon_AuraScript::HandleDummyTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warl_demonic_circle_summon_AuraScript();
        }
};

class spell_warl_demonic_circle_teleport : public SpellScriptLoader
{
    public:
        spell_warl_demonic_circle_teleport() : SpellScriptLoader("spell_warl_demonic_circle_teleport") { }

        class spell_warl_demonic_circle_teleport_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_demonic_circle_teleport_AuraScript);

            void HandleTeleport(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* player = GetTarget()->ToPlayer())
                {
                    if (GameObject* circle = player->GetGameObject(SPELL_WARLOCK_DEMONIC_CIRCLE_SUMMON))
                    {
                        player->NearTeleportTo(circle->GetPositionX(), circle->GetPositionY(), circle->GetPositionZ(), circle->GetOrientation());
                        player->RemoveMovementImpairingAuras();
                    }
                }
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_warl_demonic_circle_teleport_AuraScript::HandleTeleport, EFFECT_0, SPELL_AURA_MECHANIC_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warl_demonic_circle_teleport_AuraScript();
        }
};

class spell_warl_haunt : public SpellScriptLoader
{
    public:
        spell_warl_haunt() : SpellScriptLoader("spell_warl_haunt") { }

        class spell_warl_haunt_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_haunt_SpellScript);

			void HandleAfterHit()
            {
                if (Aura* aura = GetHitAura())
                    if (AuraEffect* aurEff = aura->GetEffect(EFFECT_1))
                        aurEff->SetAmount(CalculatePctN(aurEff->GetAmount(), GetHitDamage()));
            }

            void Register()
            {
				AfterHit += SpellHitFn(spell_warl_haunt_SpellScript::HandleAfterHit);
            }
        };

        class spell_warl_haunt_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_haunt_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_WARLOCK_HAUNT_HEAL))
                    return false;
                return true;
            }

            void HandleRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    int32 amount = aurEff->GetAmount();
                    GetTarget()->CastCustomSpell(caster, SPELL_WARLOCK_HAUNT_HEAL, &amount, NULL, NULL, true, NULL, aurEff, GetCasterGUID());
                }
            }

            void Register()
            {
                OnEffectRemove += AuraEffectApplyFn(spell_warl_haunt_AuraScript::HandleRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_haunt_SpellScript();
        }

        AuraScript* GetAuraScript() const
        {
            return new spell_warl_haunt_AuraScript();
        }
};

class spell_warl_unstable_affliction : public SpellScriptLoader
{
    public:
        spell_warl_unstable_affliction() : SpellScriptLoader("spell_warl_unstable_affliction") { }

        class spell_warl_unstable_affliction_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_unstable_affliction_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_WARLOCK_UNSTABLE_AFFLICTION_DISPEL))
                    return false;
                return true;
            }

            void HandleDispel(DispelInfo* dispelInfo)
            {
                if (Unit* caster = GetCaster())
				{
                    if (AuraEffect const* aurEff = GetEffect(EFFECT_0))
                    {
                        int32 damage = aurEff->GetAmount() * 9;
                        caster->CastCustomSpell(dispelInfo->GetDispeller(), SPELL_WARLOCK_UNSTABLE_AFFLICTION_DISPEL, &damage, NULL, NULL, true, NULL, aurEff);
                    }
				}
            }

            void Register()
            {
                AfterDispel += AuraDispelFn(spell_warl_unstable_affliction_AuraScript::HandleDispel);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warl_unstable_affliction_AuraScript();
        }
};

class spell_warl_curse_of_doom : public SpellScriptLoader
{
    public:
        spell_warl_curse_of_doom() : SpellScriptLoader("spell_warl_curse_of_doom") { }

        class spell_warl_curse_of_doom_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_warl_curse_of_doom_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_WARLOCK_CURSE_OF_DOOM_EFFECT))
                    return false;
                return true;
            }

            bool Load()
            {
                return GetCaster() && GetCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (!GetCaster())
                    return;

                AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
                if (removeMode != AURA_REMOVE_BY_DEATH || !IsExpired())
                    return;

                if (GetCaster()->ToPlayer()->IsHonorOrXPTarget(GetTarget()))
                    GetCaster()->CastSpell(GetTarget(), SPELL_WARLOCK_CURSE_OF_DOOM_EFFECT, true, NULL, aurEff);
            }

            void Register()
            {
                 AfterEffectRemove += AuraEffectRemoveFn(spell_warl_curse_of_doom_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_warl_curse_of_doom_AuraScript();
        }
};

class spell_warl_health_funnel : public SpellScriptLoader
{
public:
	spell_warl_health_funnel() : SpellScriptLoader("spell_warl_health_funnel") { }

    class spell_warl_health_funnel_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_warl_health_funnel_AuraScript);

        void ApplyEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
			Unit* caster = GetCaster();
            if (!caster)
                return;

            Unit* target = GetTarget();
            if (caster->HasAura(SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_R2))
                target->CastSpell(target, SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R2, true);
            else if (caster->HasAura(SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_R1))
                target->CastSpell(target, SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R1, true);
        }

        void RemoveEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            target->RemoveAurasDueToSpell(SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R1);
            target->RemoveAurasDueToSpell(SPELL_WARLOCK_IMPROVED_HEALTH_FUNNEL_BUFF_R2);
        }

        void Register()
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_warl_health_funnel_AuraScript::RemoveEffect, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
            OnEffectApply += AuraEffectApplyFn(spell_warl_health_funnel_AuraScript::ApplyEffect, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
	{
		return new spell_warl_health_funnel_AuraScript();
	}
};

enum WarlockSoulstone
{
	SPELL_USE_SOULSTONE_1        = 3026,
	SPELL_USE_SOULSTONE_2        = 20758,
	SPELL_USE_SOULSTONE_3        = 20759,
	SPELL_USE_SOULSTONE_4        = 20760,
	SPELL_USE_SOULSTONE_5        = 20761,
	SPELL_USE_SOULSTONE_6        = 27240,
	SPELL_USE_SOULSTONE_7        = 47882,
	SPELL_WARLOCK_RESURRECTION   = 25035,
};

class spell_warl_use_soulstone : public SpellScriptLoader
{
    public:
        spell_warl_use_soulstone() : SpellScriptLoader("spell_warl_use_soulstone") {}

        class spell_warl_use_soulstone_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_warl_use_soulstone_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_USE_SOULSTONE_1)
					|| !sSpellMgr->GetSpellInfo(SPELL_USE_SOULSTONE_2)
					|| !sSpellMgr->GetSpellInfo(SPELL_USE_SOULSTONE_3)
					|| !sSpellMgr->GetSpellInfo(SPELL_USE_SOULSTONE_4)
					|| !sSpellMgr->GetSpellInfo(SPELL_USE_SOULSTONE_5)
					|| !sSpellMgr->GetSpellInfo(SPELL_USE_SOULSTONE_6)
					|| !sSpellMgr->GetSpellInfo(SPELL_USE_SOULSTONE_7))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
				if (Unit* caster = GetCaster())
				{
					caster->CastSpell(caster, SPELL_WARLOCK_RESURRECTION, true);
					caster->PlayDirectSound(7354);
				}
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_warl_use_soulstone_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SELF_RESURRECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_warl_use_soulstone_SpellScript();
        }
};

void AddSC_warlock_spell_scripts()
{
    new spell_warl_banish();
	new spell_warl_shadow_bite();
    new spell_warl_demonic_empowerment();
    new spell_warl_create_healthstone();
    new spell_warl_everlasting_affliction();
    new spell_warl_ritual_of_doom_effect();
    new spell_warl_seed_of_corruption();
    new spell_warl_soulshatter();
    new spell_warl_life_tap();
	new spell_warl_demonic_circle_summon();
	new spell_warl_demonic_circle_teleport();
	new spell_warl_haunt();
	new spell_warl_unstable_affliction();
	new spell_warl_curse_of_doom();
	new spell_warl_health_funnel();
	new spell_warl_use_soulstone();
}