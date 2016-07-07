/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "MapManager.h"

enum HunterSpells
{
	DRAENEI_SPELL_GIFT_OF_THE_NAARU                 = 59543,
	SPELL_HUNTER_READINESS                          = 23989,
	SPELL_HUNTER_SNIPER_TRAINING_R1                 = 53302,
    SPELL_HUNTER_SNIPER_TRAINING_BUFF_R1            = 64418,
    SPELL_HUNTER_BESTIAL_WRATH                      = 19574,
    SPELL_HUNTER_PET_SPELL_LAST_STAND_TRIGGERED     = 53479,
    SPELL_HUNTER_PET_HEART_OF_THE_PHOENIX           = 55709,
    SPELL_HUNTER_PET_HEART_OF_THE_PHOENIX_TRIGGERED = 54114,
    SPELL_HUNTER_PET_HEART_OF_THE_PHOENIX_DEBUFF    = 55711,
    SPELL_HUNTER_PET_SPELL_CARRION_FEEDER_TRIGGERED = 54045,
    SPELL_HUNTER_INVIGORATION_TRIGGERED             = 53398,
    SPELL_HUNTER_MASTERS_CALL_TRIGGERED             = 62305,
    SPELL_HUNTER_CHIMERA_SHOT_SERPENT               = 53353,
    SPELL_HUNTER_CHIMERA_SHOT_VIPER                 = 53358,
    SPELL_HUNTER_CHIMERA_SHOT_SCORPID               = 53359,
    SPELL_HUNTER_ASPECT_OF_THE_BEAST_PET            = 61669,
	SPELL_HUNTER_ASPECT_OF_THE_VIPER                = 34074,
	SPELL_HUNTER_VICIOUS_VIPER                      = 61609,
	SPELL_HUNTER_VIPER_ATTACK_SPEED                 = 60144,
	SPELL_HUNTER_ANIMAL_HANDLER                     = 34453,
};

class spell_hun_aspect_of_the_beast : public SpellScriptLoader
{
    public:
        spell_hun_aspect_of_the_beast() : SpellScriptLoader("spell_hun_aspect_of_the_beast") { }

        class spell_hun_aspect_of_the_beast_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_aspect_of_the_beast_AuraScript);
            
            bool Load()
            {
				return GetOwner()->GetTypeId() == TYPE_ID_PLAYER;
            }
            
            bool Validate(SpellInfo const* /*entry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_HUNTER_ASPECT_OF_THE_BEAST_PET))
                    return false;
                return true;
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
				Player* player = GetTarget()->ToPlayer();

                if (Pet* pet = player->GetPet())
                    pet->RemoveAurasDueToSpell(SPELL_HUNTER_ASPECT_OF_THE_BEAST_PET);
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
				Player* player = GetTarget()->ToPlayer();

                if (player->GetPet())
                    player->CastSpell(player, SPELL_HUNTER_ASPECT_OF_THE_BEAST_PET, true);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_aspect_of_the_beast_AuraScript::OnApply, EFFECT_0, SPELL_AURA_UNTRACKABLE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_hun_aspect_of_the_beast_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_UNTRACKABLE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_hun_aspect_of_the_beast_AuraScript();
        }
};

class spell_hun_chimera_shot : public SpellScriptLoader
{
    public:
        spell_hun_chimera_shot() : SpellScriptLoader("spell_hun_chimera_shot") { }

        class spell_hun_chimera_shot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_chimera_shot_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_HUNTER_CHIMERA_SHOT_SERPENT) || !sSpellMgr->GetSpellInfo(SPELL_HUNTER_CHIMERA_SHOT_VIPER) || !sSpellMgr->GetSpellInfo(SPELL_HUNTER_CHIMERA_SHOT_SCORPID))
                    return false;
                return true;
            }

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* unitTarget = GetHitUnit())
                {
                    uint32 spellId = 0;
                    int32 basePoint = 0;
                    Unit::AuraApplicationMap& Auras = unitTarget->GetAppliedAuras();
                    for (Unit::AuraApplicationMap::iterator i = Auras.begin(); i != Auras.end(); ++i)
                    {
                        Aura* aura = i->second->GetBase();
                        if (aura->GetCasterGUID() != caster->GetGUID())
                            continue;

                        // Search only Serpent Sting, Viper Sting, Scorpid Sting auras
                        flag96 familyFlag = aura->GetSpellInfo()->SpellFamilyFlags;
                        if (!(familyFlag[1] & 0x00000080 || familyFlag[0] & 0x0000C000))
                            continue;
                        if (AuraEffect const* aurEff = aura->GetEffect(0))
                        {
                            // Serpent Sting - Instantly deals 40% of the damage done by your Serpent Sting.
                            if (familyFlag[0] & 0x4000)
                            {
                                int32 TickCount = aurEff->GetTotalTicks();
                                spellId = SPELL_HUNTER_CHIMERA_SHOT_SERPENT;
                                basePoint = caster->SpellDamageBonusDone(unitTarget, aura->GetSpellInfo(), aurEff->GetAmount(), DOT, aura->GetStackAmount());
                                ApplyPctN(basePoint, TickCount * 40);
								basePoint = unitTarget->SpellDamageBonusTaken(caster, aura->GetSpellInfo(), basePoint, DOT, aura->GetStackAmount());
								if (Player* modOwner = caster->GetSpellModOwner())
									modOwner->ApplySpellMod(aura->GetSpellInfo()->Id, SPELLMOD_DOT, basePoint);
                            }
                            // Viper Sting - Instantly restores mana to you equal to 60% of the total amount drained by your Viper Sting.
                            else if (familyFlag[1] & 0x00000080)
                            {
                                int32 TickCount = aura->GetEffect(0)->GetTotalTicks();
                                spellId = SPELL_HUNTER_CHIMERA_SHOT_VIPER;

                                // Amount of one aura tick
                                basePoint = int32(CalculatePctN(unitTarget->GetMaxPower(POWER_MANA), aurEff->GetAmount()));
                                int32 casterBasePoint = aurEff->GetAmount() * unitTarget->GetMaxPower(POWER_MANA) / 50; // TODO: WTF? caster uses unitTarget?
                                if (basePoint > casterBasePoint)
                                    basePoint = casterBasePoint;
                                ApplyPctN(basePoint, TickCount * 60);
                            }
                            // Scorpid Sting - Attempts to Disarm the target for 10 sec. This effect cannot occur more than once per 1 minute.
                            else if (familyFlag[0] & 0x00008000)
                                spellId = SPELL_HUNTER_CHIMERA_SHOT_SCORPID;
                            // ?? nothing say in spell desc (possibly need addition check)
                            //if (familyFlag & 0x0000010000000000LL || // dot
                            //    familyFlag & 0x0000100000000000LL)   // stun
                            //{
                            //    spellId = 53366; // 53366 Chimera Shot - Wyvern
                            //}

                            // Refresh aura duration
                            aura->RefreshDuration();
                        }
                        break;
                    }
                    if (spellId)
                        caster->CastCustomSpell(unitTarget, spellId, &basePoint, 0, 0, true);
                    if (spellId == SPELL_HUNTER_CHIMERA_SHOT_SCORPID && caster->ToPlayer()) // Scorpid Sting - Add 1 minute cooldown
                        caster->ToPlayer()->AddSpellCooldown(spellId, 0, uint32(time(NULL) + 60));
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_hun_chimera_shot_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_hun_chimera_shot_SpellScript();
        }
};

class spell_hun_invigoration : public SpellScriptLoader
{
    public:
        spell_hun_invigoration() : SpellScriptLoader("spell_hun_invigoration") { }

        class spell_hun_invigoration_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_invigoration_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_HUNTER_INVIGORATION_TRIGGERED))
                    return false;
                return true;
            }

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (Unit* unitTarget = GetHitUnit())
                    if (AuraEffect* aurEff = unitTarget->GetDummyAuraEffect(SPELL_FAMILY_HUNTER, 3487, 0))
                        if (roll_chance_i(aurEff->GetAmount()))
                            unitTarget->CastSpell(unitTarget, SPELL_HUNTER_INVIGORATION_TRIGGERED, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_hun_invigoration_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_hun_invigoration_SpellScript();
        }
};

class spell_hun_last_stand_pet : public SpellScriptLoader
{
    public:
        spell_hun_last_stand_pet() : SpellScriptLoader("spell_hun_last_stand_pet") { }

        class spell_hun_last_stand_pet_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_last_stand_pet_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_HUNTER_PET_SPELL_LAST_STAND_TRIGGERED))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                int32 healthModSpellBasePoints0 = int32(caster->CountPctFromMaxHealth(HEALTH_PERCENT_30));
                caster->CastCustomSpell(caster, SPELL_HUNTER_PET_SPELL_LAST_STAND_TRIGGERED, &healthModSpellBasePoints0, NULL, NULL, true, NULL);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_hun_last_stand_pet_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_hun_last_stand_pet_SpellScript();
        }
};

class spell_hun_masters_call : public SpellScriptLoader
{
    public:
        spell_hun_masters_call() : SpellScriptLoader("spell_hun_masters_call") { }

        class spell_hun_masters_call_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_masters_call_SpellScript);

            bool Validate(SpellInfo const* spellEntry)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_HUNTER_MASTERS_CALL_TRIGGERED) || !sSpellMgr->GetSpellInfo(spellEntry->Effects[EFFECT_0].CalcValue()) || !sSpellMgr->GetSpellInfo(spellEntry->Effects[EFFECT_1].CalcValue()))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* ally = GetHitUnit())
				{
                    if (Player* caster = GetCaster()->ToPlayer())
					{
                        if (Pet* target = caster->GetPet())
                        {
                            TriggerCastFlags castMask = TriggerCastFlags(TRIGGERED_FULL_MASK & ~TRIGGERED_IGNORE_CASTER_AURASTATE);
							if (target->HasUnitState(UNIT_STATE_ROOT))
								target->RemoveMovementImpairingAuras();
                            target->CastSpell(ally, GetEffectValue(), castMask);
                            target->CastSpell(ally, GetSpellInfo()->Effects[EFFECT_0].CalcValue(), castMask);
                        }
					}
				}
            }

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    TriggerCastFlags castMask = TriggerCastFlags(TRIGGERED_FULL_MASK & ~TRIGGERED_IGNORE_CASTER_AURASTATE);
                    target->CastSpell(target, SPELL_HUNTER_MASTERS_CALL_TRIGGERED, castMask);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_hun_masters_call_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnEffectHitTarget += SpellEffectFn(spell_hun_masters_call_SpellScript::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_hun_masters_call_SpellScript();
        }
};

class spell_hun_readiness : public SpellScriptLoader
{
    public:
        spell_hun_readiness() : SpellScriptLoader("spell_hun_readiness") { }

        class spell_hun_readiness_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_readiness_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Player* caster = GetCaster()->ToPlayer();
                // immediately finishes the cooldown on your other Hunter abilities except Bestial Wrath
                const SpellCooldowns& cm = caster->ToPlayer()->GetSpellCooldownMap();
                for (SpellCooldowns::const_iterator itr = cm.begin(); itr != cm.end();)
                {
                    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(itr->first);

                    ///! If spellId in cooldown map isn't valid, the above will return a null pointer.
                    if (spellInfo &&
                        spellInfo->SpellFamilyName == SPELL_FAMILY_HUNTER &&
                        spellInfo->Id != SPELL_HUNTER_READINESS &&
                        spellInfo->Id != SPELL_HUNTER_BESTIAL_WRATH &&
						spellInfo->Id != DRAENEI_SPELL_GIFT_OF_THE_NAARU &&
                        spellInfo->GetRecoveryTime() > 0)
                        caster->RemoveSpellCooldown((itr++)->first, true);
                    else
                        ++itr;
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_hun_readiness_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_hun_readiness_SpellScript();
        }
};

class spell_hun_scatter_shot : public SpellScriptLoader
{
    public:
        spell_hun_scatter_shot() : SpellScriptLoader("spell_hun_scatter_shot") { }

        class spell_hun_scatter_shot_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_scatter_shot_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Player* caster = GetCaster()->ToPlayer();
                // break Auto Shot and autohit
                caster->InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
                caster->AttackStop();
                caster->SendAttackSwingCancelAttack();
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_hun_scatter_shot_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_hun_scatter_shot_SpellScript();
        }
};

class spell_hun_sniper_training : public SpellScriptLoader
{
    public:
        spell_hun_sniper_training() : SpellScriptLoader("spell_hun_sniper_training") { }

        class spell_hun_sniper_training_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_sniper_training_AuraScript);

            bool Validate(SpellInfo const* /*entry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_HUNTER_SNIPER_TRAINING_R1) || !sSpellMgr->GetSpellInfo(SPELL_HUNTER_SNIPER_TRAINING_BUFF_R1))
                    return false;
                return true;
            }

            void HandlePeriodic(AuraEffect const* aurEff)
            {
                PreventDefaultAction();
                if (aurEff->GetAmount() <= 0)
                {
                    Unit* caster = GetCaster();
                    uint32 spellId = SPELL_HUNTER_SNIPER_TRAINING_BUFF_R1 + GetId() - SPELL_HUNTER_SNIPER_TRAINING_R1;
                    if (Unit* target = GetTarget())
                        if (!target->HasAura(spellId))
                        {
                            SpellInfo const* triggeredSpellInfo = sSpellMgr->GetSpellInfo(spellId);
                            Unit* triggerCaster = triggeredSpellInfo->NeedsToBeTriggeredByCaster() ? caster : target;
                            triggerCaster->CastSpell(target, triggeredSpellInfo, true, 0, aurEff);
                        }
                }
            }

            void HandleUpdatePeriodic(AuraEffect* aurEff)
            {
                if (Player* playerTarget = GetUnitOwner()->ToPlayer())
                {
                    int32 baseAmount = aurEff->GetBaseAmount();
                    int32 amount = playerTarget->isMoving() ?
                    playerTarget->CalculateSpellDamage(playerTarget, GetSpellInfo(), aurEff->GetEffIndex(), &baseAmount) :
                    aurEff->GetAmount() - 1;
                    aurEff->SetAmount(amount);
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_hun_sniper_training_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_hun_sniper_training_AuraScript::HandleUpdatePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_hun_sniper_training_AuraScript();
        }
};

class spell_hun_pet_heart_of_the_phoenix : public SpellScriptLoader
{
    public:
        spell_hun_pet_heart_of_the_phoenix() : SpellScriptLoader("spell_hun_pet_heart_of_the_phoenix") { }

        class spell_hun_pet_heart_of_the_phoenix_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_pet_heart_of_the_phoenix_SpellScript);

            bool Load()
            {
                if (!GetCaster()->IsPet())
                    return false;
                return true;
            }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_HUNTER_PET_HEART_OF_THE_PHOENIX_TRIGGERED) || !sSpellMgr->GetSpellInfo(SPELL_HUNTER_PET_HEART_OF_THE_PHOENIX_DEBUFF))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* owner = caster->GetOwner())
                    if (!caster->HasAura(SPELL_HUNTER_PET_HEART_OF_THE_PHOENIX_DEBUFF))
                    {
                        owner->CastCustomSpell(SPELL_HUNTER_PET_HEART_OF_THE_PHOENIX_TRIGGERED, SPELLVALUE_BASE_POINT0, 100, caster, true);
                        caster->CastSpell(caster, SPELL_HUNTER_PET_HEART_OF_THE_PHOENIX_DEBUFF, true);
                    }
            }

            void Register()
            {
                // add dummy effect spell handler to pet's Last Stand
                OnEffectHitTarget += SpellEffectFn(spell_hun_pet_heart_of_the_phoenix_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_hun_pet_heart_of_the_phoenix_SpellScript();
        }
};

class spell_hun_pet_carrion_feeder : public SpellScriptLoader
{
    public:
        spell_hun_pet_carrion_feeder() : SpellScriptLoader("spell_hun_pet_carrion_feeder") { }

        class spell_hun_pet_carrion_feeder_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_pet_carrion_feeder_SpellScript);
            
            bool Load()
            {
                if (!GetCaster()->IsPet())
                    return false;
                return true;
            }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_HUNTER_PET_SPELL_CARRION_FEEDER_TRIGGERED))
                    return false;
                return true;
            }

            SpellCastResult CheckIfCorpseNear()
            {
                Unit* caster = GetCaster();
                float max_range = GetSpellInfo()->GetMaxRange(false);
                WorldObject* result = NULL;
                // search for nearby enemy corpse in range
                Trinity::AnyDeadUnitSpellTargetInRangeCheck check(caster, max_range, GetSpellInfo(), TARGET_SELECT_CHECK_ENEMY);
                Trinity::WorldObjectSearcher<Trinity::AnyDeadUnitSpellTargetInRangeCheck> searcher(caster, result, check);
                caster->GetMap()->VisitFirstFound(caster->m_positionX, caster->m_positionY, max_range, searcher);
                if (!result)
                    return SPELL_FAILED_NO_EDIBLE_CORPSES;
                return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                caster->CastSpell(caster, SPELL_HUNTER_PET_SPELL_CARRION_FEEDER_TRIGGERED, false);
            }

            void Register()
            {
                // add dummy effect spell handler to pet's Last Stand
                OnEffectHit += SpellEffectFn(spell_hun_pet_carrion_feeder_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnCheckCast += SpellCheckCastFn(spell_hun_pet_carrion_feeder_SpellScript::CheckIfCorpseNear);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_hun_pet_carrion_feeder_SpellScript();
        }
};

// 34477 Misdirection
class spell_hun_misdirection : public SpellScriptLoader
{
    public:
        spell_hun_misdirection() : SpellScriptLoader("spell_hun_misdirection") { }

        class spell_hun_misdirection_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_misdirection_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
					if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_DEFAULT)
                        caster->SetReducedThreatPercent(0, 0);
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_hun_misdirection_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_hun_misdirection_AuraScript();
        }
};

// 35079 Misdirection proc
class spell_hun_misdirection_proc : public SpellScriptLoader
{
    public:
        spell_hun_misdirection_proc() : SpellScriptLoader("spell_hun_misdirection_proc") { }

        class spell_hun_misdirection_proc_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_misdirection_proc_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
                    GetCaster()->SetReducedThreatPercent(0, 0);
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_hun_misdirection_proc_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_hun_misdirection_proc_AuraScript();
        }
};

class spell_hun_disengage : public SpellScriptLoader
{
    public:
        spell_hun_disengage() : SpellScriptLoader("spell_hun_disengage") { }

        class spell_hun_disengage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_disengage_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* caster = GetCaster();
                if (caster->GetTypeId() == TYPE_ID_PLAYER && !caster->IsInCombatActive())
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_hun_disengage_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_hun_disengage_SpellScript();
        }
};

class spell_hun_tame_beast : public SpellScriptLoader
{
    public:
        spell_hun_tame_beast() : SpellScriptLoader("spell_hun_tame_beast") { }

        class spell_hun_tame_beast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_hun_tame_beast_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* caster = GetCaster();
                if (caster->GetTypeId() != TYPE_ID_PLAYER)
                    return SPELL_FAILED_DONT_REPORT;

                if (!GetExplTargetUnit())
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                if (Creature* target = GetExplTargetUnit()->ToCreature())
                {
                    if (target->GetCurrentLevel() > caster->GetCurrentLevel())
                        return SPELL_FAILED_HIGHLEVEL;

                    // use SMSG_PET_TAME_FAILURE?
                    if (!target->GetCreatureTemplate()->isTameable(caster->ToPlayer()->CanTameExoticPets()))
                        return SPELL_FAILED_BAD_TARGETS;

                    if (caster->GetPetGUID())
                        return SPELL_FAILED_ALREADY_HAVE_SUMMON;

                    if (caster->GetCharmGUID())
                        return SPELL_FAILED_ALREADY_HAVE_CHARM;
                }
                else
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_hun_tame_beast_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_hun_tame_beast_SpellScript();
        }
};

class spell_hun_viper_attack_speed : public SpellScriptLoader
{
    public:
        spell_hun_viper_attack_speed() : SpellScriptLoader("spell_hun_viper_attack_speed") { }

        class spell_hun_viper_attack_speed_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_viper_attack_speed_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_HUNTER_ASPECT_OF_THE_VIPER) ||  !sSpellMgr->GetSpellInfo(SPELL_HUNTER_VICIOUS_VIPER))
                    return false;
                return true;
            }

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget()->HasAura(SPELL_HUNTER_ASPECT_OF_THE_VIPER))
                    GetTarget()->CastSpell(GetTarget(), SPELL_HUNTER_VICIOUS_VIPER, true, NULL, aurEff);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                // possible exploit
                GetTarget()->RemoveAurasDueToSpell(SPELL_HUNTER_VICIOUS_VIPER);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_hun_viper_attack_speed_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_hun_viper_attack_speed_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_hun_viper_attack_speed_AuraScript();
        }
};

class spell_hun_freezing_arrow : public SpellScriptLoader
{
public:
    spell_hun_freezing_arrow() : SpellScriptLoader("spell_hun_freezing_arrow") { }

    class spell_hun_freezing_arrow_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_hun_freezing_arrow_SpellScript);

        bool Validate(SpellInfo const* spellInfo)
        {
            if (!sSpellMgr->GetSpellInfo(spellInfo->Effects[0].TriggerSpell))
                return false;
            return true;
        }

        void TriggerTrap(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
            uint32 trigger_spell = GetSpellInfo()->Effects[effIndex].TriggerSpell;
			Position const* pos = GetExplTargetDest();
            GetCaster()->ToPlayer()->RemoveSpellCooldown(trigger_spell);
            GetCaster()->CastSpell(pos->GetPositionX(), pos->GetPositionY(), pos->GetPositionZ(), trigger_spell, true);
			float ang = MapManager::NormalizeOrientation(pos->GetAngle(GetCaster()->ToPlayer()));
        }

        void HandleHit(SpellEffIndex effIndex)
        {
            PreventHitDefaultEffect(effIndex);
        }

        void Register()
        {
            OnEffectLaunch += SpellEffectFn(spell_hun_freezing_arrow_SpellScript::TriggerTrap, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
            OnEffectHit += SpellEffectFn(spell_hun_freezing_arrow_SpellScript::HandleHit, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_hun_freezing_arrow_SpellScript();
    }
};

class spell_hun_animal_handler : public SpellScriptLoader
{
public:
    spell_hun_animal_handler() : SpellScriptLoader("spell_hun_animal_handler") { }

    class spell_hun_animal_handler_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_hun_animal_handler_AuraScript);

        bool Load()
        {
            if (!GetCaster() || !GetCaster()->GetOwner() || GetCaster()->GetOwner()->GetTypeId() != TYPE_ID_PLAYER)
                return false;
            return true;
        }

        void CalculateAmountDamageDone(AuraEffect const* /* aurEff */, int32& amount, bool& /*canBeRecalculated*/)
        {
            if (!GetCaster() || !GetCaster()->GetOwner())
                return;
            if (Player* owner = GetCaster()->GetOwner()->ToPlayer())
            {
                if (AuraEffect* /* aurEff */ect = owner->GetAuraEffectOfRankedSpell(SPELL_HUNTER_ANIMAL_HANDLER, EFFECT_1))
                    amount = /* aurEff */ect->GetAmount();
                else
                    amount = 0;
            }
        }

        void Register()
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hun_animal_handler_AuraScript::CalculateAmountDamageDone, EFFECT_0, SPELL_AURA_MOD_ATTACK_POWER_PCT);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_hun_animal_handler_AuraScript();
    }
};

// 56654, 58882 - Rapid Recuperation
class spell_hun_rapid_recuperation : public SpellScriptLoader
{
    public:
        spell_hun_rapid_recuperation() : SpellScriptLoader("spell_hun_rapid_recuperation") { }

        class spell_hun_rapid_recuperation_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_hun_rapid_recuperation_AuraScript);

            bool Validate(SpellInfo const* spellInfo)
            {
                if (!sSpellMgr->GetSpellInfo(spellInfo->Effects[EFFECT_0].TriggerSpell))
                    return false;
                return true;
            }

            void HandlePeriodic(AuraEffect const* aurEff)
            {
                PreventDefaultAction();

                Unit* target = GetTarget();
                uint32 mana = CalculatePctN(target->GetMaxPower(POWER_MANA), aurEff->GetAmount());

                target->CastCustomSpell(GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell, SPELLVALUE_BASE_POINT0, int32(mana), target, true, NULL, aurEff);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_hun_rapid_recuperation_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_hun_rapid_recuperation_AuraScript();
        }
};

void AddSC_hunter_spell_scripts()
{
    new spell_hun_aspect_of_the_beast();
    new spell_hun_chimera_shot();
    new spell_hun_invigoration();
    new spell_hun_last_stand_pet();
    new spell_hun_masters_call();
    new spell_hun_readiness();
    new spell_hun_scatter_shot();
    new spell_hun_sniper_training();
    new spell_hun_pet_heart_of_the_phoenix();
    new spell_hun_pet_carrion_feeder();
	new spell_hun_misdirection();
	new spell_hun_misdirection_proc();
	new spell_hun_disengage();
	new spell_hun_tame_beast();
	new spell_hun_viper_attack_speed();
	new spell_hun_freezing_arrow();
	new spell_hun_animal_handler();
	new spell_hun_rapid_recuperation();
}