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
#include "SkillDiscovery.h"
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "InstanceScript.h"
#include "Group.h"
#include "LFGMgr.h"
#include "Vehicle.h"

class spell_gen_absorb0_hitlimit1 : public SpellScriptLoader
{
    public:
        spell_gen_absorb0_hitlimit1() : SpellScriptLoader("spell_gen_absorb0_hitlimit1") { }

        class spell_gen_absorb0_hitlimit1_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_absorb0_hitlimit1_AuraScript);

            uint32 limit;

            bool Load()
            {
                // Max absorb stored in 1 dummy effect
                limit = GetSpellInfo()->Effects[EFFECT_1].CalcValue();
                return true;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo& /*dmgInfo*/, uint32& absorbAmount)
            {
                absorbAmount = std::min(limit, absorbAmount);
            }

            void Register()
            {
                 OnEffectAbsorb += AuraEffectAbsorbFn(spell_gen_absorb0_hitlimit1_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_absorb0_hitlimit1_AuraScript();
        }
};

// 41337 Aura of Anger
class spell_gen_aura_of_anger : public SpellScriptLoader
{
    public:
        spell_gen_aura_of_anger() : SpellScriptLoader("spell_gen_aura_of_anger") { }

        class spell_gen_aura_of_anger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_aura_of_anger_AuraScript);

            void HandleEffectPeriodicUpdate(AuraEffect* aurEff)
            {
                if (AuraEffect* aurEff1 = aurEff->GetBase()->GetEffect(EFFECT_1))
                    aurEff1->ChangeAmount(aurEff1->GetAmount() + 5);
                aurEff->SetAmount(100 * aurEff->GetTickNumber());
            }

            void Register()
            {
                OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_gen_aura_of_anger_AuraScript::HandleEffectPeriodicUpdate, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_aura_of_anger_AuraScript();
        }
};

class spell_gen_av_drekthar_presence : public SpellScriptLoader
{
    public:
        spell_gen_av_drekthar_presence() : SpellScriptLoader("spell_gen_av_drekthar_presence") { }

        class spell_gen_av_drekthar_presence_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_av_drekthar_presence_AuraScript);

            bool CheckAreaTarget(Unit* target)
            {
                switch (target->GetEntry())
                {
                    // alliance
                    case 14762: // Dun Baldar North Marshal
                    case 14763: // Dun Baldar South Marshal
                    case 14764: // Icewing Marshal
                    case 14765: // Stonehearth Marshal
                    case 11948: // Vandar Stormspike
                    // horde
                    case 14772: // East Frostwolf Warmaster
                    case 14776: // Tower Point Warmaster
                    case 14773: // Iceblood Warmaster
                    case 14777: // West Frostwolf Warmaster
                    case 11946: // Drek'thar
                        return true;
                    default:
                        return false;
                        break;
                }
            }
            void Register()
            {
                DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_gen_av_drekthar_presence_AuraScript::CheckAreaTarget);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_av_drekthar_presence_AuraScript();
        }
};

// 46394 Brutallus Burn
class spell_gen_burn_brutallus : public SpellScriptLoader
{
    public:
        spell_gen_burn_brutallus() : SpellScriptLoader("spell_gen_burn_brutallus") { }

        class spell_gen_burn_brutallus_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_burn_brutallus_AuraScript);

            void HandleEffectPeriodicUpdate(AuraEffect* aurEff)
            {
                if (aurEff->GetTickNumber() % 11 == 0)
                    aurEff->SetAmount(aurEff->GetAmount() * 2);
            }

            void Register()
            {
                OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_gen_burn_brutallus_AuraScript::HandleEffectPeriodicUpdate, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_burn_brutallus_AuraScript();
        }
};

enum CannibalizeSpells
{
    SPELL_CANNIBALIZE_TRIGGERED = 20578,
};

class spell_gen_cannibalize : public SpellScriptLoader
{
    public:
        spell_gen_cannibalize() : SpellScriptLoader("spell_gen_cannibalize") { }

        class spell_gen_cannibalize_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_cannibalize_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_CANNIBALIZE_TRIGGERED))
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
                caster->CastSpell(caster, SPELL_CANNIBALIZE_TRIGGERED, false);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_cannibalize_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnCheckCast += SpellCheckCastFn(spell_gen_cannibalize_SpellScript::CheckIfCorpseNear);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_cannibalize_SpellScript();
        }
};

// 45472 Parachute
enum ParachuteSpells
{
    SPELL_PARACHUTE       = 45472,
    SPELL_PARACHUTE_BUFF  = 44795,
};

class spell_gen_parachute : public SpellScriptLoader
{
    public:
        spell_gen_parachute() : SpellScriptLoader("spell_gen_parachute") { }

        class spell_gen_parachute_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_parachute_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PARACHUTE) || !sSpellMgr->GetSpellInfo(SPELL_PARACHUTE_BUFF))
                    return false;
                return true;
            }

            void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (Player* target = GetTarget()->ToPlayer())
				{
                    if (target->IsFalling())
                    {
                        target->RemoveAurasDueToSpell(SPELL_PARACHUTE);
						target->CastSpell(target, SPELL_PARACHUTE_BUFF, true);
                    }
				}
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_parachute_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_parachute_AuraScript();
        }
};

enum NPCEntries
{
    NPC_DOOMGUARD   = 11859,
    NPC_INFERNAL    = 89,
    NPC_IMP         = 416,
};

class spell_gen_pet_summoned : public SpellScriptLoader
{
    public:
        spell_gen_pet_summoned() : SpellScriptLoader("spell_gen_pet_summoned") { }

        class spell_gen_pet_summoned_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_pet_summoned_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Player* player = GetCaster()->ToPlayer();

                if (player->GetLastPetNumber())
                {
                    PetType newPetType = (player->GetCurrentClass() == CLASS_HUNTER) ? HUNTER_PET : SUMMON_PET;
                    if (Pet* newPet = new Pet(player, newPetType))
                    {
                        if (newPet->LoadPetFromDB(player, 0, player->GetLastPetNumber(), true))
                        {
                            // revive the pet if it is dead
                            if (newPet->getDeathState() == DEAD)
                                newPet->setDeathState(ALIVE);

                            newPet->SetFullHealth();
                            newPet->SetPower(newPet->GetPowerType(), newPet->GetMaxPower(newPet->GetPowerType()));

                            switch (newPet->GetEntry())
                            {
                                case NPC_DOOMGUARD:
                                case NPC_INFERNAL:
                                    newPet->SetEntry(NPC_IMP);
                                    break;
                                default:
                                    break;
                            }
                        }
                        else
                            delete newPet;
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_pet_summoned_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_pet_summoned_SpellScript();
        }
};

class spell_gen_remove_flight_auras : public SpellScriptLoader
{
    public:
        spell_gen_remove_flight_auras() : SpellScriptLoader("spell_gen_remove_flight_auras") {}

        class spell_gen_remove_flight_auras_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_remove_flight_auras_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                {
                    target->RemoveAurasByType(SPELL_AURA_FLY);
                    target->RemoveAurasByType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_remove_flight_auras_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_remove_flight_auras_SpellScript();
        }
};

// 66118 Leeching Swarm
enum LeechingSwarmSpells
{
    SPELL_LEECHING_SWARM_DMG    = 66240,
    SPELL_LEECHING_SWARM_HEAL   = 66125,
};

class spell_gen_leeching_swarm : public SpellScriptLoader
{
    public:
        spell_gen_leeching_swarm() : SpellScriptLoader("spell_gen_leeching_swarm") { }

        class spell_gen_leeching_swarm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_leeching_swarm_AuraScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_LEECHING_SWARM_DMG) || !sSpellMgr->GetSpellInfo(SPELL_LEECHING_SWARM_HEAL))
                    return false;
                return true;
            }

            void HandleEffectPeriodic(AuraEffect const* aurEff)
            {
                Unit* caster = GetCaster();
                if (Unit* target = GetTarget())
                {
                    int32 lifeLeeched = target->CountPctFromCurHealth(aurEff->GetAmount());
                    if (lifeLeeched < 250)
                        lifeLeeched = 250;
                    // Damage
                    caster->CastCustomSpell(target, SPELL_LEECHING_SWARM_DMG, &lifeLeeched, 0, 0, false);
                    // Heal
                    caster->CastCustomSpell(caster, SPELL_LEECHING_SWARM_HEAL, &lifeLeeched, 0, 0, false);
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_leeching_swarm_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_leeching_swarm_AuraScript();
        }
};

enum EluneCandle
{
    SPELL_ELUNE_CANDLE_OMEN_HEAD   = 26622,
    SPELL_ELUNE_CANDLE_OMEN_CHEST  = 26624,
    SPELL_ELUNE_CANDLE_OMEN_HAND_R = 26625,
    SPELL_ELUNE_CANDLE_OMEN_HAND_L = 26649,
    SPELL_ELUNE_CANDLE_NORMAL      = 26636,

	NPC_OMEN                       = 15467,
};

class spell_gen_elune_candle : public SpellScriptLoader
{
    public:
        spell_gen_elune_candle() : SpellScriptLoader("spell_gen_elune_candle") {}

        class spell_gen_elune_candle_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_elune_candle_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_OMEN_HEAD))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_OMEN_CHEST))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_OMEN_HAND_R))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_OMEN_HAND_L))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_ELUNE_CANDLE_NORMAL))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                uint32 spellId = 0;

                if (GetHitUnit()->GetEntry() == NPC_OMEN)
                {
                    switch (urand(0, 3))
                    {
                        case 0:
							spellId = SPELL_ELUNE_CANDLE_OMEN_HEAD;
							break;
                        case 1:
							spellId = SPELL_ELUNE_CANDLE_OMEN_CHEST;
							break;
                        case 2:
							spellId = SPELL_ELUNE_CANDLE_OMEN_HAND_R;
							break;
                        case 3:
							spellId = SPELL_ELUNE_CANDLE_OMEN_HAND_L;
							break;
                    }
                }
                else
                    spellId = SPELL_ELUNE_CANDLE_NORMAL;

                GetCaster()->CastSpell(GetHitUnit(), spellId, true, NULL);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_elune_candle_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_elune_candle_SpellScript();
        }
};

// 24750 Trick
enum TrickSpells
{
    SPELL_PIRATE_COSTUME_MALE_1        = 24708,
    SPELL_PIRATE_COSTUME_FEMALE_1      = 24709,
    SPELL_NINJA_COSTUME_MALE_1         = 24710,
    SPELL_NINJA_COSTUME_FEMALE_1       = 24711,
    SPELL_LEPER_GNOME_COSTUME_MALE_1   = 24712,
    SPELL_LEPER_GNOME_COSTUME_FEMALE_1 = 24713,
    SPELL_SKELETON_COSTUME_1           = 24723,
    SPELL_GHOST_COSTUME_MALE_1         = 24735,
    SPELL_GHOST_COSTUME_FEMALE_1       = 24736,
    SPELL_TRICK_BUFF                   = 24753,
};

class spell_gen_trick : public SpellScriptLoader
{
    public:
        spell_gen_trick() : SpellScriptLoader("spell_gen_trick") {}

        class spell_gen_trick_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_trick_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PIRATE_COSTUME_MALE_1) || !sSpellMgr->GetSpellInfo(SPELL_PIRATE_COSTUME_FEMALE_1) || !sSpellMgr->GetSpellInfo(SPELL_NINJA_COSTUME_MALE_1)
                    || !sSpellMgr->GetSpellInfo(SPELL_NINJA_COSTUME_FEMALE_1) || !sSpellMgr->GetSpellInfo(SPELL_LEPER_GNOME_COSTUME_MALE_1) || !sSpellMgr->GetSpellInfo(SPELL_LEPER_GNOME_COSTUME_FEMALE_1)
                    || !sSpellMgr->GetSpellInfo(SPELL_SKELETON_COSTUME_1) || !sSpellMgr->GetSpellInfo(SPELL_GHOST_COSTUME_MALE_1) || !sSpellMgr->GetSpellInfo(SPELL_GHOST_COSTUME_FEMALE_1) || !sSpellMgr->GetSpellInfo(SPELL_TRICK_BUFF))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Player* target = GetHitPlayer())
                {
                    uint8 gender = target->GetCurrentGender();
                    uint32 spellId = SPELL_TRICK_BUFF;
                    switch (urand(0, 5))
                    {
                        case 1:
                            spellId = gender ? SPELL_LEPER_GNOME_COSTUME_FEMALE_1 : SPELL_LEPER_GNOME_COSTUME_MALE_1;
                            break;
                        case 2:
                            spellId = gender ? SPELL_PIRATE_COSTUME_FEMALE_1 : SPELL_PIRATE_COSTUME_MALE_1;
                            break;
                        case 3:
                            spellId = gender ? SPELL_GHOST_COSTUME_FEMALE_1 : SPELL_GHOST_COSTUME_MALE_1;
                            break;
                        case 4:
                            spellId = gender ? SPELL_NINJA_COSTUME_FEMALE_1 : SPELL_NINJA_COSTUME_MALE_1;
                            break;
                        case 5:
                            spellId = SPELL_SKELETON_COSTUME_1;
                            break;
                        default:
                            break;
                    }

                    caster->CastSpell(target, spellId, true, NULL);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_trick_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_trick_SpellScript();
        }
};

// 24751 Trick or Treat
enum TrickOrTreatSpells
{
    SPELL_TRICK                = 24714,
    SPELL_TREAT                = 24715,
    SPELL_TRICKED_OR_TREATED   = 24755,
    SPELL_TRICKY_TREAT_SPEED   = 42919,
    SPELL_TRICKY_TREAT_TRIGGER = 42965,
    SPELL_UPSET_TUMMY          = 42966,
};

class spell_gen_trick_or_treat : public SpellScriptLoader
{
    public:
        spell_gen_trick_or_treat() : SpellScriptLoader("spell_gen_trick_or_treat") {}

        class spell_gen_trick_or_treat_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_trick_or_treat_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TRICK))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_TREAT))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_TRICKED_OR_TREATED))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Player* target = GetHitPlayer())
                {
                    GetCaster()->CastSpell(target, roll_chance_i(50) ? SPELL_TRICK : SPELL_TREAT, true, NULL);
                    GetCaster()->CastSpell(target, SPELL_TRICKED_OR_TREATED, true, NULL);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_trick_or_treat_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_trick_or_treat_SpellScript();
        }
};

class spell_gen_tricky_treat : public SpellScriptLoader
{
    public:
        spell_gen_tricky_treat() : SpellScriptLoader("spell_gen_tricky_treat") {}

        class spell_gen_tricky_treat_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_tricky_treat_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TRICKY_TREAT_SPEED))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_TRICKY_TREAT_TRIGGER))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_UPSET_TUMMY))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
				{
                    if (caster->HasAura(SPELL_TRICKY_TREAT_TRIGGER) && caster->GetAuraCount(SPELL_TRICKY_TREAT_SPEED) > 3 && roll_chance_i(33))
                        caster->CastSpell(caster, SPELL_UPSET_TUMMY, true);
				}
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_tricky_treat_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_tricky_treat_SpellScript();
        }
};

class spell_creature_permanent_feign_death : public SpellScriptLoader
{
    public:
        spell_creature_permanent_feign_death() : SpellScriptLoader("spell_creature_permanent_feign_death") { }

        class spell_creature_permanent_feign_death_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_creature_permanent_feign_death_AuraScript);

            void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                target->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);

				if (target->GetTypeId() == TYPE_ID_UNIT)
				{
					target->ToCreature()->SetReactState(REACT_PASSIVE);
				}
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_creature_permanent_feign_death_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_creature_permanent_feign_death_AuraScript();
        }
};

enum PvPTrinketTriggeredSpells
{
    SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER      = 72752,
    SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER_WOTF = 72757,
};

class spell_pvp_trinket_wotf_shared_cd : public SpellScriptLoader
{
    public:
        spell_pvp_trinket_wotf_shared_cd() : SpellScriptLoader("spell_pvp_trinket_wotf_shared_cd") {}

        class spell_pvp_trinket_wotf_shared_cd_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pvp_trinket_wotf_shared_cd_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER) || !sSpellMgr->GetSpellInfo(SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER_WOTF))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Player* caster = GetCaster()->ToPlayer();
                SpellInfo const* spellInfo = GetSpellInfo();
				caster->AddSpellCooldown(spellInfo->Id, 0, time(NULL) + sSpellMgr->GetSpellInfo(SPELL_WILL_OF_THE_FORSAKEN_COOLDOWN_TRIGGER)->GetRecoveryTime() / IN_MILLISECONDS);
                WorldPacket data(SMSG_SPELL_COOLDOWN, 8+1+4);
                data << uint64(caster->GetGUID());
                data << uint8(0);
                data << uint32(spellInfo->Id);
                data << uint32(0);
                caster->GetSession()->SendPacket(&data);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_pvp_trinket_wotf_shared_cd_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pvp_trinket_wotf_shared_cd_SpellScript();
        }
};

enum AnimalBloodPoolSpell
{
    SPELL_ANIMAL_BLOOD      = 46221,
    SPELL_SPAWN_BLOOD_POOL  = 63471,
};

class spell_gen_animal_blood : public SpellScriptLoader
{
    public:
        spell_gen_animal_blood() : SpellScriptLoader("spell_gen_animal_blood") { }

        class spell_gen_animal_blood_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_animal_blood_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SPAWN_BLOOD_POOL))
                    return false;
                return true;
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                // Remove all auras with spell id 46221, except the one currently being applied
                while (Aura* aur = GetUnitOwner()->GetOwnedAura(SPELL_ANIMAL_BLOOD, 0, 0, 0, GetAura()))
                    GetUnitOwner()->RemoveOwnedAura(aur);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* owner = GetUnitOwner())
				{
                    if (owner->IsInWater())
                        owner->CastSpell(owner, SPELL_SPAWN_BLOOD_POOL, true);
				}
            }

            void Register()
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_gen_animal_blood_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_animal_blood_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_animal_blood_AuraScript();
        }
};

enum DivineStormSpell
{
    SPELL_DIVINE_STORM  = 53385,
};

// 70769 Divine Storm!
class spell_gen_divine_storm_cd_reset : public SpellScriptLoader
{
    public:
        spell_gen_divine_storm_cd_reset() : SpellScriptLoader("spell_gen_divine_storm_cd_reset") {}

        class spell_gen_divine_storm_cd_reset_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_divine_storm_cd_reset_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DIVINE_STORM))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Player* caster = GetCaster()->ToPlayer();

                if (caster->HasSpellCooldown(SPELL_DIVINE_STORM))
                    caster->RemoveSpellCooldown(SPELL_DIVINE_STORM, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_divine_storm_cd_reset_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_divine_storm_cd_reset_SpellScript();
        }
};

class spell_gen_gunship_portal : public SpellScriptLoader
{
    public:
        spell_gen_gunship_portal() : SpellScriptLoader("spell_gen_gunship_portal") { }

        class spell_gen_gunship_portal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_gunship_portal_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Player* caster = GetCaster()->ToPlayer();

                if (Battleground* bg = caster->GetBattleground())
				{
                    if (bg->GetTypeID(true) == BATTLEGROUND_IC)
                        bg->DoAction(1, caster->GetGUID());
				}
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_gunship_portal_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_gunship_portal_SpellScript();
        }
};

enum parachuteIC
{
    SPELL_PARACHUTE_IC = 66657,
};

class spell_gen_parachute_ic : public SpellScriptLoader
{
    public:
        spell_gen_parachute_ic() : SpellScriptLoader("spell_gen_parachute_ic") { }

        class spell_gen_parachute_ic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_parachute_ic_AuraScript);

            void HandleTriggerSpell(AuraEffect const* /*aurEff*/)
            {
                if (Player* target = GetTarget()->ToPlayer())
                    if (target->m_movementInfo.fallTime > 2000)
                        target->CastSpell(target, SPELL_PARACHUTE_IC, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_parachute_ic_AuraScript::HandleTriggerSpell, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_parachute_ic_AuraScript();
        }
};

class spell_gen_dungeon_credit : public SpellScriptLoader
{
    public:
        spell_gen_dungeon_credit() : SpellScriptLoader("spell_gen_dungeon_credit") { }

        class spell_gen_dungeon_credit_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_dungeon_credit_SpellScript);

            bool Load()
            {
                _handled = false;
                return GetCaster()->GetTypeId() == TYPE_ID_UNIT;
            }

            void CreditEncounter()
            {
                // This hook is executed for every target, make sure we only credit instance once
                if (_handled)
                    return;

                _handled = true;
                Unit* caster = GetCaster();
                if (InstanceScript* instance = caster->GetInstanceScript())
                    instance->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, GetSpellInfo()->Id, caster);
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_gen_dungeon_credit_SpellScript::CreditEncounter);
            }

            bool _handled;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_dungeon_credit_SpellScript();
        }
};

class spell_gen_profession_research : public SpellScriptLoader
{
    public:
        spell_gen_profession_research() : SpellScriptLoader("spell_gen_profession_research") {}

        class spell_gen_profession_research_SpellScript : public SpellScript
        {
			PrepareSpellScript(spell_gen_profession_research_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            SpellCastResult CheckRequirement()
            {
                if (HasDiscoveredAllSpells(GetSpellInfo()->Id, GetCaster()->ToPlayer()))
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_NOTHING_TO_DISCOVER);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Player* caster = GetCaster()->ToPlayer();
                uint32 spellId = GetSpellInfo()->Id;

                // learn random explicit discovery recipe (if any)
                if (uint32 discoveredSpellId = GetExplicitDiscoverySpell(spellId, caster))
                    caster->learnSpell(discoveredSpellId, false);

                caster->UpdateCraftSkill(spellId);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_profession_research_SpellScript::CheckRequirement);
                OnEffectHitTarget += SpellEffectFn(spell_gen_profession_research_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_profession_research_SpellScript();
        }
};

class spell_gen_clone : public SpellScriptLoader
{
    public:
        spell_gen_clone() : SpellScriptLoader("spell_gen_clone") { }

        class spell_gen_clone_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_clone_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetHitUnit()->CastSpell(GetCaster(), uint32(GetEffectValue()), true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_clone_SpellScript::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
                OnEffectHitTarget += SpellEffectFn(spell_gen_clone_SpellScript::HandleScriptEffect, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_clone_SpellScript();
        }
};

enum CloneWeaponSpells
{
    SPELL_COPY_WEAPON_AURA    = 41054,
    SPELL_COPY_WEAPON_2_AURA  = 63418,
    SPELL_COPY_WEAPON_3_AURA  = 69893,
    SPELL_COPY_OFFHAND_AURA   = 45205,
    SPELL_COPY_OFFHAND_2_AURA = 69896,
    SPELL_COPY_RANGED_AURA    = 57594,
};

class spell_gen_clone_weapon : public SpellScriptLoader
{
    public:
        spell_gen_clone_weapon() : SpellScriptLoader("spell_gen_clone_weapon") { }

        class spell_gen_clone_weapon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_clone_weapon_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetHitUnit()->CastSpell(GetCaster(), uint32(GetEffectValue()), true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_clone_weapon_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_clone_weapon_SpellScript();
        }
};

class spell_gen_clone_weapon_aura : public SpellScriptLoader
{
    public:
        spell_gen_clone_weapon_aura() : SpellScriptLoader("spell_gen_clone_weapon_aura") { }

        class spell_gen_clone_weapon_auraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_clone_weapon_auraScript);

			uint32 prevItem;

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_COPY_WEAPON_AURA) ||
                    !sSpellMgr->GetSpellInfo(SPELL_COPY_WEAPON_2_AURA) ||
                    !sSpellMgr->GetSpellInfo(SPELL_COPY_WEAPON_3_AURA) ||
                    !sSpellMgr->GetSpellInfo(SPELL_COPY_OFFHAND_AURA) ||
                    !sSpellMgr->GetSpellInfo(SPELL_COPY_OFFHAND_2_AURA) ||
                    !sSpellMgr->GetSpellInfo(SPELL_COPY_RANGED_AURA))
                    return false;
                return true;
            }

            bool Load()
            {
                prevItem = 0;
                return true;
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetTarget();
                if (!caster)
                    return;

                switch (GetSpellInfo()->Id)
                {
                    case SPELL_COPY_WEAPON_AURA:
                    case SPELL_COPY_WEAPON_2_AURA:
                    case SPELL_COPY_WEAPON_3_AURA:
                    {
                        prevItem = target->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID);

                        if (Player* player = caster->ToPlayer())
                        {
                            if (Item* mainItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND))
                                target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, mainItem->GetEntry());
                        }
                        else
                            target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, caster->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID));
                        break;
                    }
                    case SPELL_COPY_OFFHAND_AURA:
                    case SPELL_COPY_OFFHAND_2_AURA:
                    {
                        prevItem = target->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID) + 1;

                        if (Player* player = caster->ToPlayer())
                        {
                            if (Item* offItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND))
                                target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, offItem->GetEntry());
                        }
                        else
                            target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, caster->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1));
                        break;
                    }
                    case SPELL_COPY_RANGED_AURA:
                    {
                        prevItem = target->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID) + 2;

                        if (Player* player = caster->ToPlayer())
                        {
                            if (Item* rangedItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED))
                                target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, rangedItem->GetEntry());
                        }
                        else
                            target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, caster->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2));
                        break;
                    }
                    default:
                        break;
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();

                switch (GetSpellInfo()->Id)
                {
                    case SPELL_COPY_WEAPON_AURA:
                    case SPELL_COPY_WEAPON_2_AURA:
                    case SPELL_COPY_WEAPON_3_AURA:
                        target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, prevItem);
                        break;
                    case SPELL_COPY_OFFHAND_AURA:
                    case SPELL_COPY_OFFHAND_2_AURA:
                        target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, prevItem);
                        break;
                    case SPELL_COPY_RANGED_AURA:
                        target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, prevItem);
                        break;
                    default:
                        break;
                }
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_gen_clone_weapon_auraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectRemove += AuraEffectRemoveFn(spell_gen_clone_weapon_auraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_clone_weapon_auraScript();
        }
};

enum SeaforiumSpells
{
    SPELL_PLANT_CHARGES_CREDIT_ACHIEVEMENT = 60937,
};

class spell_gen_seaforium_blast : public SpellScriptLoader
{
    public:
        spell_gen_seaforium_blast() : SpellScriptLoader("spell_gen_seaforium_blast") {}

        class spell_gen_seaforium_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_seaforium_blast_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PLANT_CHARGES_CREDIT_ACHIEVEMENT))
                    return false;
                return true;
            }

            bool Load()
            {
                return GetOriginalCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            void AchievementCredit(SpellEffIndex /*effIndex*/)
            {
                if (Unit* originalCaster = GetOriginalCaster())
                    if (GameObject* go = GetHitGObj())
                        if (GetHitGObj()->GetGOInfo()->type == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
                            originalCaster->CastSpell(originalCaster, SPELL_PLANT_CHARGES_CREDIT_ACHIEVEMENT, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_seaforium_blast_SpellScript::AchievementCredit, EFFECT_1, SPELL_EFFECT_GAMEOBJECT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_seaforium_blast_SpellScript();
        }
};

enum FriendOrFowl
{
    SPELL_TURKEY_VENGEANCE = 25285,
};

class spell_gen_turkey_marker : public SpellScriptLoader
{
    public:
        spell_gen_turkey_marker() : SpellScriptLoader("spell_gen_turkey_marker") { }

        class spell_gen_turkey_marker_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_turkey_marker_AuraScript);

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                _applyTimes.push_back(getMSTime());
                Unit* target = GetTarget();

                if (GetStackAmount() >= 15)
                    target->CastSpell(target, SPELL_TURKEY_VENGEANCE, true, NULL, aurEff, GetCasterGUID());
            }

            void OnPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (_applyTimes.empty())
                    return;

                if (_applyTimes.front() + GetMaxDuration() < getMSTime())
                    ModStackAmount(-1, AURA_REMOVE_BY_EXPIRE);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_turkey_marker_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_turkey_marker_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }

            std::list<uint32> _applyTimes;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_turkey_marker_AuraScript();
        }
};

class spell_gen_lifeblood : public SpellScriptLoader
{
    public:
        spell_gen_lifeblood() : SpellScriptLoader("spell_gen_lifeblood") { }

        class spell_gen_lifeblood_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_lifeblood_AuraScript);

            void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (Unit* owner = GetUnitOwner())
                    amount += int32(CalculatePctF(owner->GetMaxHealth(), 1.5f / aurEff->GetTotalTicks()));
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_lifeblood_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_lifeblood_AuraScript();
        }
};

enum MagicRoosterSpells
{
    SPELL_MAGIC_ROOSTER_NORMAL       = 66122,
    SPELL_MAGIC_ROOSTER_DRAENEI_MALE = 66123,
    SPELL_MAGIC_ROOSTER_TAUREN_MALE  = 66124,
};

class spell_gen_magic_rooster : public SpellScriptLoader
{
    public:
        spell_gen_magic_rooster() : SpellScriptLoader("spell_gen_magic_rooster") { }

        class spell_gen_magic_rooster_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_magic_rooster_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (Player* target = GetHitPlayer())
                {
                    // prevent client crashes from stacking mounts
                    target->RemoveAurasByType(SPELL_AURA_MOUNTED);

                    uint32 spellId = SPELL_MAGIC_ROOSTER_NORMAL;
                    switch (target->GetCurrentRace())
                    {
                        case RACE_DRAENEI:
                            if (target->GetCurrentGender() == GENDER_MALE)
                                spellId = SPELL_MAGIC_ROOSTER_DRAENEI_MALE;
                            break;
                        case RACE_TAUREN:
                            if (target->GetCurrentGender() == GENDER_MALE)
                                spellId = SPELL_MAGIC_ROOSTER_TAUREN_MALE;
                            break;
                        default:
                            break;
                    }

                    target->CastSpell(target, spellId, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_magic_rooster_SpellScript::HandleScript, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_magic_rooster_SpellScript();
        }
};

class spell_gen_allow_cast_from_item_only : public SpellScriptLoader
{
    public:
        spell_gen_allow_cast_from_item_only() : SpellScriptLoader("spell_gen_allow_cast_from_item_only") { }

        class spell_gen_allow_cast_from_item_only_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_allow_cast_from_item_only_SpellScript);

            SpellCastResult CheckRequirement()
            {
                if (!GetCastItem())
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_allow_cast_from_item_only_SpellScript::CheckRequirement);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_allow_cast_from_item_only_SpellScript();
        }
};

enum RibbonPoleData
{
    SPELL_HAS_FULL_MIDSUMMER_SET  = 58933,
    SPELL_BURNING_HOT_POLE_DANCE  = 58934,
	SPELL_RIBBON_POLE_START_DANCE = 29531,
	SPELL_RIBBON_POLE_CHANNEL     = 29172,
	SPELL_RIBBON_POLE_DO_END      = 46829,
	SPELL_RIBBON_POLE_AFTER_END   = 46830,
	SPELL_RIBBON_POLE_CHECK_DANCE = 45390,
	SPELL_RIBBON_POLE_FLAME_RING  = 45422,
    SPELL_RIBBON_POLE_EXP         = 29175,

	SOUND_RIBBON_POLE_1           = 12319,
	SOUND_RIBBON_POLE_2           = 12325,

	ACHIEVEMENT_HOT_POLE_DANCE    = 271,

    GO_RIBBON_POLE                = 181605,
};

class spell_gen_ribbon_pole_dancer_check : public SpellScriptLoader
{
    public:
        spell_gen_ribbon_pole_dancer_check() : SpellScriptLoader("spell_gen_ribbon_pole_dancer_check") { }

        class spell_gen_ribbon_pole_dancer_check_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_ribbon_pole_dancer_check_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_HAS_FULL_MIDSUMMER_SET))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_BURNING_HOT_POLE_DANCE))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_RIBBON_POLE_EXP))
                    return false;
                return true;
            }

            void PeriodicTick(AuraEffect const* /*aurEff*/)
            {
                Unit* target = GetTarget();

                if (!target)
                    return;

                // check if aura needs to be removed
                if (!target->FindGameobjectWithDistance(GO_RIBBON_POLE, 20.0f) || !target->HasUnitState(UNIT_STATE_CASTING))
                {
                    target->InterruptNonMeleeSpells(false);
                    target->RemoveAurasDueToSpell(GetId());
                    return;
                }

                // Xp Buff Duration
                if (Aura* aur = target->GetAura(SPELL_RIBBON_POLE_EXP))
                {
                    aur->SetMaxDuration(aur->GetMaxDuration() >= 3600000 ? 3600000 : aur->GetMaxDuration() + 180000);
                    aur->RefreshDuration();

                    // Reward Achievement Criteria
                    if (aur->GetMaxDuration() == 3600000 && target->HasAura(SPELL_HAS_FULL_MIDSUMMER_SET))
					{
						if (AchievementEntry const* HotPoleDance = sAchievementStore.LookupEntry(ACHIEVEMENT_HOT_POLE_DANCE))
							target->ToPlayer()->CompletedAchievement(HotPoleDance);

						target->CastSpell(target, SPELL_RIBBON_POLE_DO_END, true);
						target->CastSpell(target, SPELL_RIBBON_POLE_AFTER_END, true);
						target->CastSpell(target, SPELL_RIBBON_POLE_FLAME_RING, true);

						target->RemoveAurasDueToSpell(SPELL_RIBBON_POLE_START_DANCE);
						target->RemoveAurasDueToSpell(SPELL_RIBBON_POLE_CHECK_DANCE);

						target->InterruptNonMeleeSpells(false);
						target->RemoveAurasDueToSpell(GetId());

						target->PlayDirectSound(SOUND_RIBBON_POLE_2);
					}
                }
                else
                    target->AddAura(SPELL_RIBBON_POLE_EXP, target);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_ribbon_pole_dancer_check_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_ribbon_pole_dancer_check_AuraScript();
        }
};

enum TorchCatchingData
{
    SPELL_FLING_TORCH_MISSILE     = 45669,
    SPELL_TOSS_TORCH_SHADOW       = 46105,
    SPELL_TORCH_TARGET_PICKER     = 45907,
    SPELL_TORCHES_COUGHT          = 45693,
    SPELL_JUGGLE_TORCH_MISSED     = 45676,
    SPELL_TORCH_CATCHING_SUCCESS  = 46081,
    SPELL_TORCH_DAILY_SUCCESS     = 46654,

    NPC_JUGGLE_TARGET             = 25515,

    QUEST_TORCH_CATCHING_A        = 11657,
    QUEST_TORCH_CATCHING_H        = 11923,
    QUEST_MORE_TORCH_CATCHING_A   = 11924,
    QUEST_MORE_TORCH_CATCHING_H   = 11925,
};

class spell_gen_torch_target_picker : public SpellScriptLoader
{
    public:
        spell_gen_torch_target_picker() : SpellScriptLoader("spell_gen_torch_target_picker") {}

        class spell_gen_torch_target_picker_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_torch_target_picker_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_FLING_TORCH_MISSILE))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_TOSS_TORCH_SHADOW))
                    return false;
                return true;
            }

            void FilterTargets(std::list<Unit*>& unitList)
            {
                Unit* caster = GetCaster();

                if (!caster)
                    return;

                std::list<Creature*> juggleList;
                caster->GetCreatureListWithEntryInGrid(juggleList, NPC_JUGGLE_TARGET, 10.0f);

                if (!juggleList.empty())
				{
                    for (std::list<Creature*>::iterator iter = juggleList.begin(); iter != juggleList.end(); ++iter)
                        unitList.remove(*iter);
				}

                if (unitList.empty())
                    return;

                std::list<Unit*>::iterator itr = unitList.begin();
                std::advance(itr, urand(0, unitList.size() - 1));

                Unit* target = *itr;
                unitList.clear();
                unitList.push_back(target);
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();

                if (!caster || !target)
                    return;

                caster->CastSpell(target, SPELL_FLING_TORCH_MISSILE, true);
                caster->CastSpell(target, SPELL_TOSS_TORCH_SHADOW, true);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_gen_torch_target_picker_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnEffectHitTarget += SpellEffectFn(spell_gen_torch_target_picker_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_torch_target_picker_SpellScript();
        }
};

enum TorchToss
{
    SPELL_TORCH_TOSSING_COMPLETE_A = 45719,
    SPELL_TORCH_TOSSING_COMPLETE_H = 46651,
    SPELL_TORCH_TOSSING_TRAINING   = 45716,
    SPELL_TORCH_TOSSING_PRACTICE   = 46630,
    SPELL_BRAZIERS_HIT             = 45724,
	SPELL_QUEST_COMPLETE_VISUAL_1  = 46830,
	SPELL_QUEST_COMPLETE_VISUAL_2  = 45422,

	QUEST_TORCH_TOSSING_A          = 11731,
	QUEST_TORCH_TOSSING_H          = 11922,
	QUEST_MORE_TORCH_TOSSING_A     = 11921,
	QUEST_MORE_TORCH_TOSSING_H     = 11926,
};

class spell_gen_torch_toss_land : public SpellScriptLoader
{
    public:
        spell_gen_torch_toss_land() : SpellScriptLoader("spell_gen_torch_toss_land") { }

        class spell_gen_torch_toss_land_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_torch_toss_land_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BRAZIERS_HIT))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
				Unit* caster = GetCaster();
				Player* player = caster->ToPlayer();

				player->CastSpell(player, SPELL_BRAZIERS_HIT, true);
				player->CastSpell(player, SPELL_TORCH_TOSSING_TRAINING, true);

				// QUEST_TORCH_TOSSING_A
				if (player->GetAuraCount(SPELL_BRAZIERS_HIT) >= 8 && player->GetQuestStatus(QUEST_TORCH_TOSSING_A) == QUEST_STATUS_INCOMPLETE)
				{
					player->CastSpell(player, SPELL_TORCH_TOSSING_COMPLETE_A, true);

					player->RemoveAurasDueToSpell(SPELL_BRAZIERS_HIT);
					player->RemoveAurasDueToSpell(SPELL_TORCH_TOSSING_TRAINING);

					player->CastSpell(player, SPELL_QUEST_COMPLETE_VISUAL_1, true);
					player->CastSpell(player, SPELL_QUEST_COMPLETE_VISUAL_2, true);
				}

				// QUEST_TORCH_TOSSING_H
				if (player->GetAuraCount(SPELL_BRAZIERS_HIT) >= 8 && player->GetQuestStatus(QUEST_TORCH_TOSSING_H) == QUEST_STATUS_INCOMPLETE)
				{
					player->CastSpell(player, SPELL_TORCH_TOSSING_COMPLETE_H, true);

					player->RemoveAurasDueToSpell(SPELL_BRAZIERS_HIT);
					player->RemoveAurasDueToSpell(SPELL_TORCH_TOSSING_TRAINING);

					player->CastSpell(player, SPELL_QUEST_COMPLETE_VISUAL_1, true);
					player->CastSpell(player, SPELL_QUEST_COMPLETE_VISUAL_2, true);
				}

				// QUEST_MORE_TORCH_TOSSING_A
				if (player->GetAuraCount(SPELL_BRAZIERS_HIT) >= 20 && player->GetQuestStatus(QUEST_MORE_TORCH_TOSSING_A) == QUEST_STATUS_INCOMPLETE)
				{
					player->CastSpell(player, SPELL_TORCH_TOSSING_COMPLETE_A, true);

					player->RemoveAurasDueToSpell(SPELL_BRAZIERS_HIT);
					player->RemoveAurasDueToSpell(SPELL_TORCH_TOSSING_TRAINING);

					player->CastSpell(player, SPELL_QUEST_COMPLETE_VISUAL_1, true);
					player->CastSpell(player, SPELL_QUEST_COMPLETE_VISUAL_2, true);
				}

				// QUEST_MORE_TORCH_TOSSING_H
				if (player->GetAuraCount(SPELL_BRAZIERS_HIT) >= 20 && player->GetQuestStatus(QUEST_MORE_TORCH_TOSSING_H) == QUEST_STATUS_INCOMPLETE)
				{
					player->CastSpell(player, SPELL_TORCH_TOSSING_COMPLETE_H, true);

					player->RemoveAurasDueToSpell(SPELL_BRAZIERS_HIT);
					player->RemoveAurasDueToSpell(SPELL_TORCH_TOSSING_TRAINING);

					player->CastSpell(player, SPELL_QUEST_COMPLETE_VISUAL_1, true);
					player->CastSpell(player, SPELL_QUEST_COMPLETE_VISUAL_2, true);
				}
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_torch_toss_land_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_torch_toss_land_SpellScript();
        }
};

class spell_gen_juggle_torch_catch : public SpellScriptLoader
{
    public:
        spell_gen_juggle_torch_catch() : SpellScriptLoader("spell_gen_juggle_torch_catch") {}

        class spell_gen_juggle_torch_catch_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_juggle_torch_catch_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TORCH_TARGET_PICKER))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_TORCHES_COUGHT))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_JUGGLE_TORCH_MISSED))
                    return false;
                return true;
            }

            void FilterTargets(std::list<Unit*>& unitList)
            {
                Unit* caster = GetCaster();
                Unit* juggleTarget = NULL;
                bool missed = true;

                if (unitList.empty() || !caster || !caster->ToPlayer())
                     return;

                for (std::list<Unit*>::iterator iter = unitList.begin(); iter != unitList.end(); ++iter)
                {
                    if (*iter == caster)
                        missed = false;

                    if ((*iter)->ToCreature())
                        juggleTarget = *iter;
                }

                if (missed)
                {
                    if (juggleTarget)
                        juggleTarget->CastSpell(juggleTarget, SPELL_JUGGLE_TORCH_MISSED, true);
                    caster->RemoveAurasDueToSpell(SPELL_TORCHES_COUGHT);
                }
                else
                {
                    uint8 neededCatches;

                    if (caster->ToPlayer()->GetQuestStatus(QUEST_TORCH_CATCHING_A) == QUEST_STATUS_INCOMPLETE || caster->ToPlayer()->GetQuestStatus(QUEST_TORCH_CATCHING_H) == QUEST_STATUS_INCOMPLETE)
                    {
                        neededCatches = 4;
                    }
                    else if (caster->ToPlayer()->GetQuestStatus(QUEST_MORE_TORCH_CATCHING_A) == QUEST_STATUS_INCOMPLETE || caster->ToPlayer()->GetQuestStatus(QUEST_MORE_TORCH_CATCHING_H) == QUEST_STATUS_INCOMPLETE)
                    {
                        neededCatches = 10;
                    }
                    else
                    {
                        caster->RemoveAurasDueToSpell(SPELL_TORCHES_COUGHT);
                        return;
                    }

                    caster->CastSpell(caster, SPELL_TORCH_TARGET_PICKER, true);
                    caster->CastSpell(caster, SPELL_TORCHES_COUGHT, true);

                    // Reward Quest
                    if (caster->GetAuraCount(SPELL_TORCHES_COUGHT) >= neededCatches)
                    {
                        caster->CastSpell(caster, SPELL_TORCH_CATCHING_SUCCESS, true);
                        caster->CastSpell(caster, SPELL_TORCH_DAILY_SUCCESS, true);
                        caster->RemoveAurasDueToSpell(SPELL_TORCHES_COUGHT);
                    }
                }
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_gen_juggle_torch_catch_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_juggle_torch_catch_SpellScript();
        }
};

enum JuggleTorch
{
	SPELL_JUGGLE_TORCH        = 45638,
	SPELL_JUGGLE_TORCH_SLOW   = 45792,
	SPELL_JUGGLE_TORCH_MEDIUM = 45806,
	SPELL_JUGGLE_TORCH_FAST   = 45816,
};

class spell_gen_throw_torch : public SpellScriptLoader
{
    public:
        spell_gen_throw_torch() : SpellScriptLoader("spell_gen_throw_torch") {}

        class spell_gen_throw_torch_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_throw_torch_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_JUGGLE_TORCH))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_JUGGLE_TORCH_SLOW))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_JUGGLE_TORCH_MEDIUM))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_JUGGLE_TORCH_FAST))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitPlayer();

                if (!caster || !target)
                    return;
                
                if (caster != target)
                {
                    uint32 spellId;
                    switch (urand(0, 2))
                    {
                        case 1:
                            spellId = SPELL_JUGGLE_TORCH_FAST;
                            break;
                        case 2:
                            spellId = SPELL_JUGGLE_TORCH_MEDIUM;
                            break;
                        default:
                            spellId = SPELL_JUGGLE_TORCH_SLOW;
                            break;
                    }
                    caster->CastSpell(target, spellId, true);
                }
                else
                    caster->CastSpell(caster, SPELL_JUGGLE_TORCH, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_throw_torch_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_throw_torch_SpellScript();
        }
};

enum Launch
{
    SPELL_LAUNCH_NO_FALLING_DAMAGE = 66251,
};

class spell_gen_launch : public SpellScriptLoader
{
    public:
        spell_gen_launch() : SpellScriptLoader("spell_gen_launch") {}

        class spell_gen_launch_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_launch_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Player* player = GetHitPlayer())
                    player->AddAura(SPELL_LAUNCH_NO_FALLING_DAMAGE, player); // prevents falling damage
            }

            void Launch()
            {
                WorldLocation const* const position = GetExplTargetDest();

                if (Player* player = GetHitPlayer())
                {
                    player->ExitVehicle();

                    float speedZ = 10.0f;
                    float dist = position->GetExactDist2d(player->GetPositionX(), player->GetPositionY());
                    float speedXY = dist;

                    player->GetMotionMaster()->MoveJump(position->GetPositionX(), position->GetPositionY(), position->GetPositionZ(), speedXY, speedZ);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_launch_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_FORCE_CAST);
                AfterHit += SpellHitFn(spell_gen_launch_SpellScript::Launch);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_launch_SpellScript();
        }
};

enum BoneGryphon
{
    NPC_GRYPHON_RIDER = 29333
};

class GryphonRiderCheck
{
public:
	bool operator() (Unit* unit)
	{
		if (unit->ToCreature() && unit->GetEntry() == NPC_GRYPHON_RIDER)
			return false;
		return true;
	}
};

class spell_gen_bone_gryphon_frost_breath : public SpellScriptLoader
{
    public:
        spell_gen_bone_gryphon_frost_breath() : SpellScriptLoader("spell_gen_bone_gryphon_frost_breath") { }

        class spell_gen_bone_gryphon_frost_breath_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_bone_gryphon_frost_breath_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if(GryphonRiderCheck());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_gen_bone_gryphon_frost_breath_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_bone_gryphon_frost_breath_SpellScript();
        }
};

enum VehicleScaling
{
    SPELL_GEAR_SCALING      = 66668,
	SPELL_SOTA_DEMO_SCALING = 65636,
};

class spell_gen_vehicle_scaling : public SpellScriptLoader
{
    public:
        spell_gen_vehicle_scaling() : SpellScriptLoader("spell_gen_vehicle_scaling") { }

        class spell_gen_vehicle_scaling_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_vehicle_scaling_AuraScript);

            bool Load()
            {
                return GetCaster() && GetCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                Unit* caster = GetCaster();
                float factor;
                uint16 baseItemLevel;

                // TODO: Reserach coeffs for different vehicles
                switch (GetId())
                {
                    case SPELL_GEAR_SCALING:
                        factor = 1.0f;
                        baseItemLevel = 205;
                        break;
					case SPELL_SOTA_DEMO_SCALING:
						factor = 1.0f;
						baseItemLevel = 230; // based of ~185k hp for ~270 item lvl
						break;
                    default:
                        factor = 1.0f;
                        baseItemLevel = 170;
                        break;
                }

                float avgILvl = caster->ToPlayer()->GetAverageItemLevel();
                if (avgILvl < baseItemLevel)
                    return;

                amount = uint16((avgILvl - baseItemLevel) * factor);
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_vehicle_scaling_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_MOD_HEALING_PCT);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_vehicle_scaling_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_vehicle_scaling_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_vehicle_scaling_AuraScript();
        }
};

class spell_gen_oracle_wolvar_reputation : public SpellScriptLoader
{
    public:
        spell_gen_oracle_wolvar_reputation() : SpellScriptLoader("spell_gen_oracle_wolvar_reputation") { }

        class spell_gen_oracle_wolvar_reputation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_oracle_wolvar_reputation_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            void HandleDummy(SpellEffIndex effIndex)
            {
                Player* player = GetCaster()->ToPlayer();
                uint32 FactionId = GetSpellInfo()->Effects[effIndex].CalcValue();
                int32 RepChange = GetSpellInfo()->Effects[EFFECT_1].CalcValue();

                FactionEntry const* Faction = sFactionStore.LookupEntry(FactionId);

                if (!Faction)
                    return;

                // Set rep to baserep + basepoints (expecting spillover for oposite faction -> become hated)
                // Not when player already has equal or higher rep with this faction
                if (player->GetReputationMgr().GetBaseReputation(Faction) < RepChange)
                    player->GetReputationMgr().SetReputation(Faction, RepChange);

                // EFFECT_INDEX_2 most likely update at war state, we already handle this in SetReputation
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_oracle_wolvar_reputation_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_oracle_wolvar_reputation_SpellScript();
        }
};

class spell_gen_luck_of_the_draw : public SpellScriptLoader
{
    public:
        spell_gen_luck_of_the_draw() : SpellScriptLoader("spell_gen_luck_of_the_draw") { }

        class spell_gen_luck_of_the_draw_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_luck_of_the_draw_AuraScript);

            bool Load()
            {
                return GetUnitOwner()->GetTypeId() == TYPE_ID_PLAYER;
            }

            // cheap hax to make it have update calls
            void CalcPeriodic(AuraEffect const* /*effect*/, bool& isPeriodic, int32& amplitude)
            {
                isPeriodic = true;
                amplitude = 5 * IN_MILLISECONDS;
            }

            void Update(AuraEffect* /*effect*/)
            {
                if (Player* owner = GetUnitOwner()->ToPlayer())
                {
                    const LfgDungeonSet dungeons = sLFGMgr->GetSelectedDungeons(owner->GetGUID());
                    LfgDungeonSet::const_iterator itr = dungeons.begin();

                    if (itr == dungeons.end())
                    {
                        Remove(AURA_REMOVE_BY_DEFAULT);
                        return;
                    }


                    LFGDungeonEntry const* randomDungeon = sLFGDungeonStore.LookupEntry(*itr);
                    if (Group* group = owner->GetGroup())
					{
                        if (Map const* map = owner->GetMap())
						{
                            if (group->isLFGGroup())
							{
                                if (uint32 dungeonId = sLFGMgr->GetDungeon(group->GetGUID(), true))
								{
                                    if (LFGDungeonEntry const* dungeon = sLFGDungeonStore.LookupEntry(dungeonId))
									{
                                        if (uint32(dungeon->map) == map->GetId() && dungeon->difficulty == uint32(map->GetDifficulty()))
										{
                                            if (randomDungeon && randomDungeon->type == LFG_TYPE_RANDOM)
                                                return; // in correct dungeon
										}
									}
								}
							}
						}
					}

                    Remove(AURA_REMOVE_BY_DEFAULT);
                }
            }

            void Register()
            {
                DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_gen_luck_of_the_draw_AuraScript::CalcPeriodic, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
                OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_gen_luck_of_the_draw_AuraScript::Update, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_luck_of_the_draw_AuraScript();
        }
};

enum WintergraspRpggSpells
{
    SPELL_RPGG_DAMAGE = 49769,
};

class WintergraspVehicleCheck
{
    public:
        bool operator() (Unit* unit)
        {
            if (unit->IsVehicle() && unit->GetEntry() != 28366) // exclude cannons
                return false;
            return true;
        }
};

class spell_gen_rocket_propelled_goblin_grenade : public SpellScriptLoader
{
    public:
        spell_gen_rocket_propelled_goblin_grenade() : SpellScriptLoader("spell_gen_rocket_propelled_goblin_grenade") { }

        class spell_gen_rocket_propelled_goblin_grenade_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_rocket_propelled_goblin_grenade_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_RPGG_DAMAGE))
                    return false;
                return true;
            }

            bool Load()
            {
                _target = NULL;
                return true;
            }

            void SelectTarget(std::list<Unit*>& unitList)
            {
                if (unitList.empty())
                    return;

                unitList.remove_if(WintergraspVehicleCheck());
                if (unitList.empty())
                    return;

                _target = Quantum::DataPackets::SelectRandomContainerElement(unitList);
                unitList.clear();
                unitList.push_back(_target);
            }

            void HandleDummy(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (_target)
                    GetCaster()->CastSpell(_target, SPELL_RPGG_DAMAGE, true);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_gen_rocket_propelled_goblin_grenade_SpellScript::SelectTarget, EFFECT_1, TARGET_UNIT_CONE_ENEMY_24/*TARGET_UNIT_CONE_ENEMY*/);
                OnEffectLaunch += SpellEffectFn(spell_gen_rocket_propelled_goblin_grenade_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
            }

            Unit* _target;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_rocket_propelled_goblin_grenade_SpellScript();
        }
};

enum DamageReductionAura
{
    SPELL_BLESSING_OF_SANCTUARY         = 20911,
    SPELL_GREATER_BLESSING_OF_SANCTUARY = 25899,
    SPELL_RENEWED_HOPE                  = 63944,
    SPELL_VIGILANCE                     = 50720,
    SPELL_DAMAGE_REDUCTION_AURA         = 68066,
};

class spell_gen_damage_reduction_aura : public SpellScriptLoader
{
    public:
        spell_gen_damage_reduction_aura() : SpellScriptLoader("spell_gen_damage_reduction_aura") { }

        class spell_gen_damage_reduction_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_damage_reduction_AuraScript);

            bool Validate(SpellInfo const* /*SpellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DAMAGE_REDUCTION_AURA))
                    return false;
                return true;
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                target->CastSpell(target, SPELL_DAMAGE_REDUCTION_AURA, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                if (target->HasAura(SPELL_DAMAGE_REDUCTION_AURA) && !(target->HasAura(SPELL_BLESSING_OF_SANCTUARY) ||
                    target->HasAura(SPELL_GREATER_BLESSING_OF_SANCTUARY) ||
                    target->HasAura(SPELL_RENEWED_HOPE) ||
                    target->HasAura(SPELL_VIGILANCE)))
                        target->RemoveAurasDueToSpell(SPELL_DAMAGE_REDUCTION_AURA);
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_gen_damage_reduction_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectRemove += AuraEffectRemoveFn(spell_gen_damage_reduction_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }

        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_damage_reduction_AuraScript();
        }
};

enum DalaranDisguiseSpells
{
    SPELL_SUNREAVER_DISGUISE_TRIGGER       = 69672,
    SPELL_SUNREAVER_DISGUISE_FEMALE        = 70973,
    SPELL_SUNREAVER_DISGUISE_MALE          = 70974,

    SPELL_SILVER_COVENANT_DISGUISE_TRIGGER = 69673,
    SPELL_SILVER_COVENANT_DISGUISE_FEMALE  = 70971,
    SPELL_SILVER_COVENANT_DISGUISE_MALE    = 70972,
};

class spell_gen_dalaran_disguise : public SpellScriptLoader
{
    public:
        spell_gen_dalaran_disguise(const char* name) : SpellScriptLoader(name) {}

        class spell_gen_dalaran_disguise_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_dalaran_disguise_SpellScript);

            bool Validate(SpellInfo const* spellEntry)
            {
                switch (spellEntry->Id)
                {
                    case SPELL_SUNREAVER_DISGUISE_TRIGGER:
                        if (!sSpellMgr->GetSpellInfo(SPELL_SUNREAVER_DISGUISE_FEMALE) || !sSpellMgr->GetSpellInfo(SPELL_SUNREAVER_DISGUISE_MALE))
                            return false;
                        break;
                    case SPELL_SILVER_COVENANT_DISGUISE_TRIGGER:
                        if (!sSpellMgr->GetSpellInfo(SPELL_SILVER_COVENANT_DISGUISE_FEMALE) || !sSpellMgr->GetSpellInfo(SPELL_SILVER_COVENANT_DISGUISE_MALE))
                            return false;
                        break;
                }
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Player* player = GetHitPlayer())
                {
                    uint8 gender = player->GetCurrentGender();

                    uint32 spellId = GetSpellInfo()->Id;

                    switch (spellId)
                    {
                        case SPELL_SUNREAVER_DISGUISE_TRIGGER:
                            spellId = gender ? SPELL_SUNREAVER_DISGUISE_FEMALE : SPELL_SUNREAVER_DISGUISE_MALE;
                            break;
                        case SPELL_SILVER_COVENANT_DISGUISE_TRIGGER:
                            spellId = gender ? SPELL_SILVER_COVENANT_DISGUISE_FEMALE : SPELL_SILVER_COVENANT_DISGUISE_MALE;
                            break;
                        default:
                            break;
                    }
                    GetCaster()->CastSpell(player, spellId, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_dalaran_disguise_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_dalaran_disguise_SpellScript();
        }
};

enum BreakShieldSpells
{
	SPELL_BREAK_SHIELD_DAMAGE_2K                 = 62626,
    SPELL_BREAK_SHIELD_DAMAGE_10K                = 64590,
    SPELL_BREAK_SHIELD_TRIGGER_FACTION_MOUNTS    = 62575,
    SPELL_BREAK_SHIELD_TRIGGER_CAMPAING_WARHORSE = 64595,
    SPELL_BREAK_SHIELD_TRIGGER_UNK               = 66480,
};

class spell_gen_break_shield: public SpellScriptLoader
{
    public:
        spell_gen_break_shield(const char* name) : SpellScriptLoader(name) {}

        class spell_gen_break_shield_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_break_shield_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                Unit* target = GetHitUnit();

                switch (effIndex)
                {
                    case EFFECT_0: // On spells wich trigger the damaging spell (and also the visual)
                    {
                        uint32 spellId;

                        switch (GetSpellInfo()->Id)
                        {
                            case SPELL_BREAK_SHIELD_TRIGGER_UNK:
                            case SPELL_BREAK_SHIELD_TRIGGER_CAMPAING_WARHORSE:
                                spellId = SPELL_BREAK_SHIELD_DAMAGE_10K;
                                break;
                            case SPELL_BREAK_SHIELD_TRIGGER_FACTION_MOUNTS:
                                spellId = SPELL_BREAK_SHIELD_DAMAGE_2K;
                                break;
                            default:
                                return;
                        }

                        if (Unit* rider = GetCaster()->GetCharmer())
                            rider->CastSpell(target, spellId, false);
                        else
                            GetCaster()->CastSpell(target, spellId, false);
                        break;
                    }
                    case EFFECT_1: // On damaging spells, for removing a defend layer
                    {
                        Unit::AuraApplicationMap const& auras = target->GetAppliedAuras();
                        for (Unit::AuraApplicationMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                        {
                            if (Aura* aura = itr->second->GetBase())
                            {
                                SpellInfo const* auraInfo = aura->GetSpellInfo();
                                if (auraInfo && auraInfo->SpellIconID == 2007 && aura->HasEffectType(SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN))
                                {
                                    aura->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);
                                    // Remove dummys from rider (Necessary for updating visual shields)
                                    if (Unit* rider = target->GetCharmer())
									{
                                        if (Aura* defend = rider->GetAura(aura->GetId()))
                                            defend->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);
									}
                                    break;
                                }
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_break_shield_SpellScript::HandleScriptEffect, EFFECT_FIRST_FOUND, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_break_shield_SpellScript();
        }
};

enum ChargeSpells
{
    SPELL_CHARGE_DAMAGE_8K5             = 62874,
    SPELL_CHARGE_DAMAGE_20K             = 68498,
    SPELL_CHARGE_DAMAGE_45K             = 64591,

    SPELL_CHARGE_CHARGING_EFFECT_8K5    = 63661,
    SPELL_CHARGE_CHARGING_EFFECT_20K_1  = 68284,
    SPELL_CHARGE_CHARGING_EFFECT_20K_2  = 68501,
    SPELL_CHARGE_CHARGING_EFFECT_45K_1  = 62563,
    SPELL_CHARGE_CHARGING_EFFECT_45K_2  = 66481,

    SPELL_CHARGE_TRIGGER_FACTION_MOUNTS = 62960,
    SPELL_CHARGE_TRIGGER_TRIAL_CHAMPION = 68282,

    SPELL_CHARGE_MISS_EFFECT            = 62977,
};

class spell_gen_mounted_charge: public SpellScriptLoader
{
    public:
        spell_gen_mounted_charge() : SpellScriptLoader("spell_gen_mounted_charge") { }

        class spell_gen_mounted_charge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_mounted_charge_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                Unit* target = GetHitUnit();

                switch (effIndex)
                {
                    case EFFECT_0: // On spells wich trigger the damaging spell (and also the visual)
                    {
                        uint32 spellId;

                        switch (GetSpellInfo()->Id)
                        {
                            case SPELL_CHARGE_TRIGGER_TRIAL_CHAMPION:
                                spellId = SPELL_CHARGE_CHARGING_EFFECT_20K_1;
                                break;
                            case SPELL_CHARGE_TRIGGER_FACTION_MOUNTS:
                                spellId = SPELL_CHARGE_CHARGING_EFFECT_8K5;
                                break;
                            default:
                                return;
                        }

                        // If target isn't a training dummy there's a chance of failing the charge
                        if (!target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE) && roll_chance_f(12.5f))
                            spellId = SPELL_CHARGE_MISS_EFFECT;

                        if (Unit* vehicle = GetCaster()->GetVehicleBase())
                            vehicle->CastSpell(target, spellId, false);
                        else
                            GetCaster()->CastSpell(target, spellId, false);
                        break;
                    }
                    case EFFECT_1: // On damaging spells, for removing a defend layer
                    case EFFECT_2:
                    {
                        Unit::AuraApplicationMap const& auras = target->GetAppliedAuras();
                        for (Unit::AuraApplicationMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                        {
                            if (Aura* aura = itr->second->GetBase())
                            {
                                SpellInfo const* auraInfo = aura->GetSpellInfo();
                                if (auraInfo && auraInfo->SpellIconID == 2007 && aura->HasEffectType(SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN))
                                {
                                    aura->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);
                                    // Remove dummys from rider (Necessary for updating visual shields)
                                    if (Unit* rider = target->GetCharmer())
									{
                                        if (Aura* defend = rider->GetAura(aura->GetId()))
                                            defend->ModStackAmount(-1, AURA_REMOVE_BY_ENEMY_SPELL);
									}
                                    break;
                                }
                            }
                        }
                        break;
                    }
                }
            }

            void HandleChargeEffect(SpellEffIndex /*effIndex*/)
            {
                uint32 spellId;

                switch (GetSpellInfo()->Id)
                {
                    case SPELL_CHARGE_CHARGING_EFFECT_8K5:
                        spellId = SPELL_CHARGE_DAMAGE_8K5;
                        break;
                    case SPELL_CHARGE_CHARGING_EFFECT_20K_1:
                    case SPELL_CHARGE_CHARGING_EFFECT_20K_2:
                        spellId = SPELL_CHARGE_DAMAGE_20K;
                        break;
                    case SPELL_CHARGE_CHARGING_EFFECT_45K_1:
                    case SPELL_CHARGE_CHARGING_EFFECT_45K_2:
                        spellId = SPELL_CHARGE_DAMAGE_45K;
                        break;
                    default:
                        return;
                }

                if (Unit* rider = GetCaster()->GetCharmer())
                    rider->CastSpell(GetHitUnit(), spellId, false);
                else
                    GetCaster()->CastSpell(GetHitUnit(), spellId, false);
            }

            void Register()
            {
                SpellInfo const* spell = sSpellMgr->GetSpellInfo(m_scriptSpellId);

                if (spell->HasEffect(SPELL_EFFECT_SCRIPT_EFFECT))
                    OnEffectHitTarget += SpellEffectFn(spell_gen_mounted_charge_SpellScript::HandleScriptEffect, EFFECT_FIRST_FOUND, SPELL_EFFECT_SCRIPT_EFFECT);

                if (spell->Effects[EFFECT_0].Effect == SPELL_EFFECT_CHARGE)
                    OnEffectHitTarget += SpellEffectFn(spell_gen_mounted_charge_SpellScript::HandleChargeEffect, EFFECT_0, SPELL_EFFECT_CHARGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_mounted_charge_SpellScript();
        }
};

enum DefendVisuals
{
    SPELL_VISUAL_SHIELD_1 = 63130,
    SPELL_VISUAL_SHIELD_2 = 63131,
    SPELL_VISUAL_SHIELD_3 = 63132,
};

class spell_gen_defend : public SpellScriptLoader
{
    public:
        spell_gen_defend() : SpellScriptLoader("spell_gen_defend") { }

        class spell_gen_defend_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_defend_AuraScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_VISUAL_SHIELD_1))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_VISUAL_SHIELD_2))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_VISUAL_SHIELD_3))
                    return false;
                return true;
            }

            void RefreshVisualShields(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
				if (GetCaster())
                {
                    Unit* target = GetTarget();

                    for (uint8 i = 0; i < GetSpellInfo()->StackAmount; ++i)
                        target->RemoveAurasDueToSpell(SPELL_VISUAL_SHIELD_1 + i);

                    target->CastSpell(target, SPELL_VISUAL_SHIELD_1 + GetAura()->GetStackAmount() - 1, true, NULL, aurEff);
                }
                else
                    GetTarget()->RemoveAurasDueToSpell(GetId());
            }

            void RemoveVisualShields(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                for (uint8 i = 0; i < GetSpellInfo()->StackAmount; ++i)
                    GetTarget()->RemoveAurasDueToSpell(SPELL_VISUAL_SHIELD_1 + i);
            }

            void RemoveDummyFromDriver(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
				{
                    if (TempSummon* vehicle = caster->ToTempSummon())
					{
                        if (Unit* rider = vehicle->GetSummoner())
                            rider->RemoveAurasDueToSpell(GetId());
					}
				}
            }

            void Register()
            {
                SpellInfo const* spell = sSpellMgr->GetSpellInfo(m_scriptSpellId);

                // Defend spells casted by NPCs (add visuals)
                if (spell->Effects[EFFECT_0].ApplyAuraName == SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN)
                {
                    AfterEffectApply += AuraEffectApplyFn(spell_gen_defend_AuraScript::RefreshVisualShields, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                    OnEffectRemove += AuraEffectRemoveFn(spell_gen_defend_AuraScript::RemoveVisualShields, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
                }

                // Remove Defend spell from player when he dismounts
                if (spell->Effects[EFFECT_2].ApplyAuraName == SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN)
                    OnEffectRemove += AuraEffectRemoveFn(spell_gen_defend_AuraScript::RemoveDummyFromDriver, EFFECT_2, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);

                // Defend spells casted by players (add/remove visuals)
                if (spell->Effects[EFFECT_1].ApplyAuraName == SPELL_AURA_DUMMY)
                {
                    AfterEffectApply += AuraEffectApplyFn(spell_gen_defend_AuraScript::RefreshVisualShields, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                    OnEffectRemove += AuraEffectRemoveFn(spell_gen_defend_AuraScript::RemoveVisualShields, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_CHANGE_AMOUNT_MASK);
                }
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_defend_AuraScript();
        }
};

enum MountedDuelSpells
{
    SPELL_ON_TOURNAMENT_MOUNT = 63034,
    SPELL_MOUNTED_DUEL        = 62875,
};

class spell_gen_tournament_duel : public SpellScriptLoader
{
    public:
        spell_gen_tournament_duel() : SpellScriptLoader("spell_gen_tournament_duel") { }

        class spell_gen_tournament_duel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_tournament_duel_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ON_TOURNAMENT_MOUNT))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_MOUNTED_DUEL))
                    return false;
                return true;
            }

			void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (Unit* rider = GetCaster()->GetCharmer())
                {
                    if (Player* plrTarget = GetHitPlayer())
                    {
                        if (plrTarget->HasAura(SPELL_ON_TOURNAMENT_MOUNT) && plrTarget->GetVehicleBase())
                            rider->CastSpell(plrTarget, SPELL_MOUNTED_DUEL, true);
                    }
                    else if (Unit* unitTarget = GetHitUnit())
                    {
                        if (unitTarget->GetCharmer() && unitTarget->GetCharmer()->GetTypeId() == TYPE_ID_PLAYER && unitTarget->GetCharmer()->HasAura(SPELL_ON_TOURNAMENT_MOUNT))
                            rider->CastSpell(unitTarget->GetCharmer(), SPELL_MOUNTED_DUEL, true);
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_tournament_duel_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_tournament_duel_SpellScript();
        }
};

enum TournamentMountsSpells
{
    SPELL_LANCE_EQUIPPED = 62853,
};

class spell_gen_summon_tournament_mount : public SpellScriptLoader
{
    public:
        spell_gen_summon_tournament_mount() : SpellScriptLoader("spell_gen_summon_tournament_mount") { }

        class spell_gen_summon_tournament_mount_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_summon_tournament_mount_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_LANCE_EQUIPPED))
                    return false;
                return true;
            }

            SpellCastResult CheckIfLanceEquiped()
            {
                if (GetCaster()->IsInDisallowedMountForm())
                    GetCaster()->RemoveAurasByType(SPELL_AURA_MOD_SHAPESHIFT);

                if (!GetCaster()->HasAura(SPELL_LANCE_EQUIPPED))
                {
                    SetCustomCastResultMessage(SPELL_CUSTOM_ERROR_MUST_HAVE_LANCE_EQUIPPED);
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_summon_tournament_mount_SpellScript::CheckIfLanceEquiped);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_summon_tournament_mount_SpellScript();
        }
};

class spell_gen_throw_shield : public SpellScriptLoader
{
    public:
        spell_gen_throw_shield() : SpellScriptLoader("spell_gen_throw_shield") { }

        class spell_gen_throw_shield_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_throw_shield_SpellScript);

            void HandleScriptEffect(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetCaster()->CastSpell(GetHitUnit(), uint32(GetEffectValue()), true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_throw_shield_SpellScript::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_throw_shield_SpellScript();
        }
};

enum TournamentPennantSpells
{
    SPELL_PENNANT_STORMWIND_ASPIRANT      = 62595,
    SPELL_PENNANT_STORMWIND_VALIANT       = 62596,
    SPELL_PENNANT_STORMWIND_CHAMPION      = 62594,
    SPELL_PENNANT_GNOMEREGAN_ASPIRANT     = 63394,
    SPELL_PENNANT_GNOMEREGAN_VALIANT      = 63395,
    SPELL_PENNANT_GNOMEREGAN_CHAMPION     = 63396,
    SPELL_PENNANT_SEN_JIN_ASPIRANT        = 63397,
    SPELL_PENNANT_SEN_JIN_VALIANT         = 63398,
    SPELL_PENNANT_SEN_JIN_CHAMPION        = 63399,
    SPELL_PENNANT_SILVERMOON_ASPIRANT     = 63401,
    SPELL_PENNANT_SILVERMOON_VALIANT      = 63402,
    SPELL_PENNANT_SILVERMOON_CHAMPION     = 63403,
    SPELL_PENNANT_DARNASSUS_ASPIRANT      = 63404,
    SPELL_PENNANT_DARNASSUS_VALIANT       = 63405,
    SPELL_PENNANT_DARNASSUS_CHAMPION      = 63406,
    SPELL_PENNANT_EXODAR_ASPIRANT         = 63421,
    SPELL_PENNANT_EXODAR_VALIANT          = 63422,
    SPELL_PENNANT_EXODAR_CHAMPION         = 63423,
    SPELL_PENNANT_IRONFORGE_ASPIRANT      = 63425,
    SPELL_PENNANT_IRONFORGE_VALIANT       = 63426,
    SPELL_PENNANT_IRONFORGE_CHAMPION      = 63427,
    SPELL_PENNANT_UNDERCITY_ASPIRANT      = 63428,
    SPELL_PENNANT_UNDERCITY_VALIANT       = 63429,
    SPELL_PENNANT_UNDERCITY_CHAMPION      = 63430,
    SPELL_PENNANT_ORGRIMMAR_ASPIRANT      = 63431,
    SPELL_PENNANT_ORGRIMMAR_VALIANT       = 63432,
    SPELL_PENNANT_ORGRIMMAR_CHAMPION      = 63433,
    SPELL_PENNANT_THUNDER_BLUFF_ASPIRANT  = 63434,
    SPELL_PENNANT_THUNDER_BLUFF_VALIANT   = 63435,
    SPELL_PENNANT_THUNDER_BLUFF_CHAMPION  = 63436,
    SPELL_PENNANT_ARGENT_CRUSADE_ASPIRANT = 63606,
    SPELL_PENNANT_ARGENT_CRUSADE_VALIANT  = 63500,
    SPELL_PENNANT_ARGENT_CRUSADE_CHAMPION = 63501,
    SPELL_PENNANT_EBON_BLADE_ASPIRANT     = 63607,
    SPELL_PENNANT_EBON_BLADE_VALIANT      = 63608,
    SPELL_PENNANT_EBON_BLADE_CHAMPION     = 63609,
};

enum TournamentMounts
{
    NPC_STORMWIND_STEED             = 33217,
    NPC_IRONFORGE_RAM               = 33316,
    NPC_GNOMEREGAN_MECHANOSTRIDER   = 33317,
    NPC_EXODAR_ELEKK                = 33318,
    NPC_DARNASSIAN_NIGHTSABER       = 33319,
    NPC_ORGRIMMAR_WOLF              = 33320,
    NPC_DARK_SPEAR_RAPTOR           = 33321,
    NPC_THUNDER_BLUFF_KODO          = 33322,
    NPC_SILVERMOON_HAWKSTRIDER      = 33323,
    NPC_FORSAKEN_WARHORSE           = 33324,
    NPC_ARGENT_WARHORSE             = 33782,
    NPC_ARGENT_STEED_ASPIRANT       = 33845,
    NPC_ARGENT_HAWKSTRIDER_ASPIRANT = 33844,
};

enum TournamentQuestsAchievements
{
    ACHIEVEMENT_CHAMPION_STORMWIND     = 2781,
    ACHIEVEMENT_CHAMPION_DARNASSUS     = 2777,
    ACHIEVEMENT_CHAMPION_IRONFORGE     = 2780,
    ACHIEVEMENT_CHAMPION_GNOMEREGAN    = 2779,
    ACHIEVEMENT_CHAMPION_THE_EXODAR    = 2778,
    ACHIEVEMENT_CHAMPION_ORGRIMMAR     = 2783,
    ACHIEVEMENT_CHAMPION_SEN_JIN       = 2784,
    ACHIEVEMENT_CHAMPION_THUNDER_BLUFF = 2786,
    ACHIEVEMENT_CHAMPION_UNDERCITY     = 2787,
    ACHIEVEMENT_CHAMPION_SILVERMOON    = 2785,
    ACHIEVEMENT_ARGENT_VALOR           = 2758,
    ACHIEVEMENT_CHAMPION_ALLIANCE      = 2782,
    ACHIEVEMENT_CHAMPION_HORDE         = 2788,

    QUEST_VALIANT_OF_STORMWIND         = 13593,
    QUEST_A_VALIANT_OF_STORMWIND       = 13684,
    QUEST_VALIANT_OF_DARNASSUS         = 13706,
    QUEST_A_VALIANT_OF_DARNASSUS       = 13689,
    QUEST_VALIANT_OF_IRONFORGE         = 13703,
    QUEST_A_VALIANT_OF_IRONFORGE       = 13685,
    QUEST_VALIANT_OF_GNOMEREGAN        = 13704,
    QUEST_A_VALIANT_OF_GNOMEREGAN      = 13688,
    QUEST_VALIANT_OF_THE_EXODAR        = 13705,
    QUEST_A_VALIANT_OF_THE_EXODAR      = 13690,
    QUEST_VALIANT_OF_ORGRIMMAR         = 13707,
    QUEST_A_VALIANT_OF_ORGRIMMAR       = 13691,
    QUEST_VALIANT_OF_SEN_JIN           = 13708,
    QUEST_A_VALIANT_OF_SEN_JIN         = 13693,
    QUEST_VALIANT_OF_THUNDER_BLUFF     = 13709,
    QUEST_A_VALIANT_OF_THUNDER_BLUFF   = 13694,
    QUEST_VALIANT_OF_UNDERCITY         = 13710,
    QUEST_A_VALIANT_OF_UNDERCITY       = 13695,
    QUEST_VALIANT_OF_SILVERMOON        = 13711,
    QUEST_A_VALIANT_OF_SILVERMOON      = 13696,
};

class spell_gen_on_tournament_mount : public SpellScriptLoader
{
    public:
        spell_gen_on_tournament_mount() : SpellScriptLoader("spell_gen_on_tournament_mount") { }

        class spell_gen_on_tournament_mount_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_on_tournament_mount_AuraScript);

            uint32 _pennantSpellId;

            bool Load()
            {
                _pennantSpellId = 0;
                return GetCaster() && GetCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            void HandleApplyEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    if (Unit* vehicle = caster->GetVehicleBase())
                    {
                        _pennantSpellId = GetPennatSpellId(caster->ToPlayer(), vehicle);
                        caster->CastSpell(caster, _pennantSpellId, true);
                    }
                }
            }

            void HandleRemoveEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    caster->RemoveAurasDueToSpell(_pennantSpellId);
            }

            uint32 GetPennatSpellId(Player* player, Unit* mount)
            {
                switch (mount->GetEntry())
                {
                    case NPC_ARGENT_STEED_ASPIRANT:
                    case NPC_STORMWIND_STEED:
                    {
                        if (player->GetAchievementMgr().HasAchieved(ACHIEVEMENT_CHAMPION_STORMWIND))
                            return SPELL_PENNANT_STORMWIND_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_STORMWIND) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_STORMWIND))
                            return SPELL_PENNANT_STORMWIND_VALIANT;
                        else
                            return SPELL_PENNANT_STORMWIND_ASPIRANT;
                    }
                    case NPC_GNOMEREGAN_MECHANOSTRIDER:
                    {
                        if (player->GetAchievementMgr().HasAchieved(ACHIEVEMENT_CHAMPION_GNOMEREGAN))
                            return SPELL_PENNANT_GNOMEREGAN_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_GNOMEREGAN) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_GNOMEREGAN))
                            return SPELL_PENNANT_GNOMEREGAN_VALIANT;
                        else
                            return SPELL_PENNANT_GNOMEREGAN_ASPIRANT;
                    }
                    case NPC_DARK_SPEAR_RAPTOR:
                    {
                        if (player->GetAchievementMgr().HasAchieved(ACHIEVEMENT_CHAMPION_SEN_JIN))
                            return SPELL_PENNANT_SEN_JIN_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_SEN_JIN) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_SEN_JIN))
                            return SPELL_PENNANT_SEN_JIN_VALIANT;
                        else
                            return SPELL_PENNANT_SEN_JIN_ASPIRANT;
                    }
                    case NPC_ARGENT_HAWKSTRIDER_ASPIRANT:
                    case NPC_SILVERMOON_HAWKSTRIDER:
                    {
                        if (player->GetAchievementMgr().HasAchieved(ACHIEVEMENT_CHAMPION_SILVERMOON))
                            return SPELL_PENNANT_SILVERMOON_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_SILVERMOON) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_SILVERMOON))
                            return SPELL_PENNANT_SILVERMOON_VALIANT;
                        else
                            return SPELL_PENNANT_SILVERMOON_ASPIRANT;
                    }
                    case NPC_DARNASSIAN_NIGHTSABER:
                    {
                        if (player->GetAchievementMgr().HasAchieved(ACHIEVEMENT_CHAMPION_DARNASSUS))
                            return SPELL_PENNANT_DARNASSUS_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_DARNASSUS) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_DARNASSUS))
                            return SPELL_PENNANT_DARNASSUS_VALIANT;
                        else
                            return SPELL_PENNANT_DARNASSUS_ASPIRANT;
                    }
                    case NPC_EXODAR_ELEKK:
                    {
                        if (player->GetAchievementMgr().HasAchieved(ACHIEVEMENT_CHAMPION_THE_EXODAR))
                            return SPELL_PENNANT_EXODAR_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_THE_EXODAR) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_THE_EXODAR))
                            return SPELL_PENNANT_EXODAR_VALIANT;
                        else
                            return SPELL_PENNANT_EXODAR_ASPIRANT;
                    }
                    case NPC_IRONFORGE_RAM:
                    {
                        if (player->GetAchievementMgr().HasAchieved(ACHIEVEMENT_CHAMPION_IRONFORGE))
                            return SPELL_PENNANT_IRONFORGE_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_IRONFORGE) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_IRONFORGE))
                            return SPELL_PENNANT_IRONFORGE_VALIANT;
                        else
                            return SPELL_PENNANT_IRONFORGE_ASPIRANT;
                    }
                    case NPC_FORSAKEN_WARHORSE:
                    {
                        if (player->GetAchievementMgr().HasAchieved(ACHIEVEMENT_CHAMPION_UNDERCITY))
                            return SPELL_PENNANT_UNDERCITY_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_UNDERCITY) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_UNDERCITY))
                            return SPELL_PENNANT_UNDERCITY_VALIANT;
                        else
                            return SPELL_PENNANT_UNDERCITY_ASPIRANT;
                    }
                    case NPC_ORGRIMMAR_WOLF:
                    {
                        if (player->GetAchievementMgr().HasAchieved(ACHIEVEMENT_CHAMPION_ORGRIMMAR))
                            return SPELL_PENNANT_ORGRIMMAR_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_ORGRIMMAR) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_ORGRIMMAR))
                            return SPELL_PENNANT_ORGRIMMAR_VALIANT;
                        else
                            return SPELL_PENNANT_ORGRIMMAR_ASPIRANT;
                    }
                    case NPC_THUNDER_BLUFF_KODO:
                    {
                        if (player->GetAchievementMgr().HasAchieved(ACHIEVEMENT_CHAMPION_THUNDER_BLUFF))
                            return SPELL_PENNANT_THUNDER_BLUFF_CHAMPION;
                        else if (player->GetQuestRewardStatus(QUEST_VALIANT_OF_THUNDER_BLUFF) || player->GetQuestRewardStatus(QUEST_A_VALIANT_OF_THUNDER_BLUFF))
                            return SPELL_PENNANT_THUNDER_BLUFF_VALIANT;
                        else
                            return SPELL_PENNANT_THUNDER_BLUFF_ASPIRANT;
                    }
                    case NPC_ARGENT_WARHORSE:
                    {
                        if (player->GetAchievementMgr().HasAchieved(ACHIEVEMENT_CHAMPION_ALLIANCE) || player->GetAchievementMgr().HasAchieved(ACHIEVEMENT_CHAMPION_HORDE))
                            return player->GetCurrentClass() == CLASS_DEATH_KNIGHT ? SPELL_PENNANT_EBON_BLADE_CHAMPION : SPELL_PENNANT_ARGENT_CRUSADE_CHAMPION;
                        else if (player->GetAchievementMgr().HasAchieved(ACHIEVEMENT_ARGENT_VALOR))
                            return player->GetCurrentClass() == CLASS_DEATH_KNIGHT ? SPELL_PENNANT_EBON_BLADE_VALIANT : SPELL_PENNANT_ARGENT_CRUSADE_VALIANT;
                        else
                            return player->GetCurrentClass() == CLASS_DEATH_KNIGHT ? SPELL_PENNANT_EBON_BLADE_ASPIRANT : SPELL_PENNANT_ARGENT_CRUSADE_ASPIRANT;
                    }
                    default:
                        return 0;
                }
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_on_tournament_mount_AuraScript::HandleApplyEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                OnEffectRemove += AuraEffectRemoveFn(spell_gen_on_tournament_mount_AuraScript::HandleRemoveEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_on_tournament_mount_AuraScript();
        }
};

class spell_gen_tournament_pennant : public SpellScriptLoader
{
    public:
        spell_gen_tournament_pennant() : SpellScriptLoader("spell_gen_tournament_pennant") { }

        class spell_gen_tournament_pennant_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_tournament_pennant_AuraScript);

            bool Load()
            {
                return GetCaster() && GetCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            void HandleApplyEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    if (!caster->GetVehicleBase())
                        caster->RemoveAurasDueToSpell(GetId());
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_gen_tournament_pennant_AuraScript::HandleApplyEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_tournament_pennant_AuraScript();
        }
};

enum ChaosBlast
{
    SPELL_CHAOS_BLAST   = 37675,
};

class spell_gen_chaos_blast : public SpellScriptLoader
{
    public:
        spell_gen_chaos_blast() : SpellScriptLoader("spell_gen_chaos_blast") { }

        class spell_gen_chaos_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_chaos_blast_SpellScript);

            bool Validate(SpellInfo const* /*SpellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_CHAOS_BLAST))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                int32 basepoints0 = 100;
                Unit* caster = GetCaster();

                if (Unit* target = GetHitUnit())
                    caster->CastCustomSpell(target, SPELL_CHAOS_BLAST, &basepoints0, NULL, NULL, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_chaos_blast_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_chaos_blast_SpellScript();
        }
};

class spell_gen_toy_train_set : public SpellScriptLoader
{
    public:
        spell_gen_toy_train_set() : SpellScriptLoader("spell_gen_toy_train_set") { }

        class spell_gen_toy_train_set_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_toy_train_set_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Player* target = GetHitPlayer())
                {
                    target->HandleEmoteCommand(EMOTE_ONESHOT_TRAIN);

                    WorldPacket data(SMSG_TEXT_EMOTE, 8 + 4 + 4 + 4 + 1);
                    data << uint64(target->GetGUID());
                    data << uint32(TEXT_EMOTE_TRAIN);
                    data << uint32(0);
                    data << uint32(1);
                    data << uint8(0);
                    target->SendMessageToSet(&data, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_toy_train_set_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_toy_train_set_SpellScript();
        }
};

enum DummyTrigger
{
    SPELL_PERSISTANT_SHIELD_TRIGGERED       = 26470,
    SPELL_PERSISTANT_SHIELD                 = 26467,
};

class spell_gen_dummy_trigger : public SpellScriptLoader
{
    public:
        spell_gen_dummy_trigger() : SpellScriptLoader("spell_gen_dummy_trigger") { }

        class spell_gen_dummy_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_dummy_trigger_SpellScript);

            bool Validate(SpellInfo const* /*SpellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PERSISTANT_SHIELD_TRIGGERED) || !sSpellMgr->GetSpellInfo(SPELL_PERSISTANT_SHIELD))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                int32 damage = GetEffectValue();
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
                    if (SpellInfo const* triggeredByAuraSpell = GetTriggeringSpell())
                        if (triggeredByAuraSpell->Id == SPELL_PERSISTANT_SHIELD_TRIGGERED)
                            caster->CastCustomSpell(target, SPELL_PERSISTANT_SHIELD_TRIGGERED, &damage, NULL, NULL, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_dummy_trigger_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_dummy_trigger_SpellScript();
        }
};

class spell_gen_spirit_healer_res : public SpellScriptLoader
{
    public:
        spell_gen_spirit_healer_res(): SpellScriptLoader("spell_gen_spirit_healer_res") { }

        class spell_gen_spirit_healer_res_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_spirit_healer_res_SpellScript);

            bool Load()
            {
                return GetOriginalCaster() && GetOriginalCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Player* originalCaster = GetOriginalCaster()->ToPlayer();
                if (Unit* target = GetHitUnit())
                {
                    WorldPacket data(SMSG_SPIRIT_HEALER_CONFIRM, 8);
                    data << uint64(target->GetGUID());
                    originalCaster->GetSession()->SendPacket(&data);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_spirit_healer_res_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_spirit_healer_res_SpellScript();
        }
};

enum TransporterBackfires
{
    SPELL_TRANSPORTER_MALFUNCTION_POLYMORPH     = 23444,
    SPELL_TRANSPORTER_EVIL_TWIN                 = 23445,
    SPELL_TRANSPORTER_MALFUNCTION_MISS          = 36902,
};

class spell_gen_gadgetzan_transporter_backfire : public SpellScriptLoader
{
    public:
        spell_gen_gadgetzan_transporter_backfire() : SpellScriptLoader("spell_gen_gadgetzan_transporter_backfire") { }

        class spell_gen_gadgetzan_transporter_backfire_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_gadgetzan_transporter_backfire_SpellScript);

            bool Validate(SpellInfo const* /*SpellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_MALFUNCTION_POLYMORPH) || !sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_EVIL_TWIN)
                    || !sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_MALFUNCTION_MISS))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                int32 r = irand(0, 119);
                if (r < 20)                           // Transporter Malfunction - 1/6 polymorph
                    caster->CastSpell(caster, SPELL_TRANSPORTER_MALFUNCTION_POLYMORPH, true);
                else if (r < 100)                     // Evil Twin               - 4/6 evil twin
                    caster->CastSpell(caster, SPELL_TRANSPORTER_EVIL_TWIN, true);
                else                                    // Transporter Malfunction - 1/6 miss the target
                    caster->CastSpell(caster, SPELL_TRANSPORTER_MALFUNCTION_MISS, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_gadgetzan_transporter_backfire_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_gadgetzan_transporter_backfire_SpellScript();
        }
};

enum GnomishTransporter
{
    SPELL_TRANSPORTER_SUCCESS                   = 23441,
    SPELL_TRANSPORTER_FAILURE                   = 23446,
};

class spell_gen_gnomish_transporter : public SpellScriptLoader
{
    public:
        spell_gen_gnomish_transporter() : SpellScriptLoader("spell_gen_gnomish_transporter") { }

        class spell_gen_gnomish_transporter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_gnomish_transporter_SpellScript);

            bool Validate(SpellInfo const* /*SpellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_SUCCESS) || !sSpellMgr->GetSpellInfo(SPELL_TRANSPORTER_FAILURE))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                caster->CastSpell(caster, roll_chance_i(50) ? SPELL_TRANSPORTER_SUCCESS : SPELL_TRANSPORTER_FAILURE , true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_gnomish_transporter_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_gnomish_transporter_SpellScript();
        }
};

class spell_gen_ds_flush_knockback : public SpellScriptLoader
{
    public:
        spell_gen_ds_flush_knockback() : SpellScriptLoader("spell_gen_ds_flush_knockback") {}

        class spell_gen_ds_flush_knockback_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_ds_flush_knockback_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                // Here the target is the water spout and determines the position where the player is knocked from
                if (Unit* target = GetHitUnit())
                {
                    if (Player* player = GetCaster()->ToPlayer())
                    {
                        float horizontalSpeed = 20.0f + (40.0f - GetCaster()->GetDistance(target));
                        float verticalSpeed = 8.0f;
                        // This method relies on the Dalaran Sewer map disposition and Water Spout position
                        // What we do is knock the player from a position exactly behind him and at the end of the pipe
                        GetCaster()->KnockbackFrom(target->GetPositionX(), GetCaster()->GetPositionY(), horizontalSpeed, verticalSpeed);
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_ds_flush_knockback_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_ds_flush_knockback_SpellScript();
        }
};

class spell_gen_wg_water : public SpellScriptLoader
{
    public:
        spell_gen_wg_water() : SpellScriptLoader("spell_gen_wg_water") {}

        class spell_gen_wg_water_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_wg_water_SpellScript);

            SpellCastResult CheckCast()
            {
                if (!GetSpellInfo()->CheckTargetCreatureType(GetCaster()))
                    return SPELL_FAILED_DONT_REPORT;
                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_wg_water_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_wg_water_SpellScript();
        }
};

class spell_gen_count_pct_from_max_hp : public SpellScriptLoader
{
    public:
        spell_gen_count_pct_from_max_hp(char const* name, int32 damagePct = 0) : SpellScriptLoader(name), _damagePct(damagePct) { }

        class spell_gen_count_pct_from_max_hp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_count_pct_from_max_hp_SpellScript);

        public:
            spell_gen_count_pct_from_max_hp_SpellScript(int32 damagePct) : SpellScript(), _damagePct(damagePct) { }

            void RecalculateDamage()
            {
                if (!_damagePct)
                    _damagePct = GetHitDamage();

                SetHitDamage(GetHitUnit()->CountPctFromMaxHealth(_damagePct));
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_gen_count_pct_from_max_hp_SpellScript::RecalculateDamage);
            }

        private:
            int32 _damagePct;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_count_pct_from_max_hp_SpellScript(_damagePct);
        }

    private:
        int32 _damagePct;
};

class spell_gen_despawn_self : public SpellScriptLoader
{
public:
    spell_gen_despawn_self() : SpellScriptLoader("spell_gen_despawn_self") { }

    class spell_gen_despawn_self_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_despawn_self_SpellScript);

        bool Load()
        {
            return GetCaster()->GetTypeId() == TYPE_ID_UNIT;
        }

        void HandleDummy(SpellEffIndex effIndex)
        {
			if (GetSpellInfo()->Effects[effIndex].Effect == SPELL_EFFECT_DUMMY || GetSpellInfo()->Effects[effIndex].Effect == SPELL_EFFECT_SCRIPT_EFFECT)
				GetCaster()->ToCreature()->DespawnAfterAction();
        }

        void Register()
        {
			OnEffectHitTarget += SpellEffectFn(spell_gen_despawn_self_SpellScript::HandleDummy, EFFECT_ALL, SPELL_EFFECT_ANY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_despawn_self_SpellScript();
    }
};

enum GenericBandage
{
    SPELL_RECENTLY_BANDAGED = 11196,
};

class spell_gen_bandage : public SpellScriptLoader
{
    public:
        spell_gen_bandage() : SpellScriptLoader("spell_gen_bandage") { }

        class spell_gen_bandage_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_bandage_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_RECENTLY_BANDAGED))
                    return false;
                return true;
            }

            SpellCastResult CheckCast()
            {
                if (Unit* target = GetExplTargetUnit())
                {
                    if (target->HasAura(SPELL_RECENTLY_BANDAGED))
                        return SPELL_FAILED_TARGET_AURASTATE;
                }
                return SPELL_CAST_OK;
            }

            void HandleScript()
            {
                if (Unit* target = GetHitUnit())
                    GetCaster()->CastSpell(target, SPELL_RECENTLY_BANDAGED, true);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_gen_bandage_SpellScript::CheckCast);
                AfterHit += SpellHitFn(spell_gen_bandage_SpellScript::HandleScript);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_bandage_SpellScript();
        }
};

enum GenericLifebloom
{
    SPELL_HEXLORD_MALACRASS_LIFEBLOOM_FINAL_HEAL        = 43422,
    SPELL_TUR_RAGEPAW_LIFEBLOOM_FINAL_HEAL              = 52552,
    SPELL_CENARION_SCOUT_LIFEBLOOM_FINAL_HEAL           = 53692,
    SPELL_TWISTED_VISAGE_LIFEBLOOM_FINAL_HEAL           = 57763,
    SPELL_FACTION_CHAMPIONS_DRU_LIFEBLOOM_FINAL_HEAL    = 66094,
};

class spell_gen_lifebloom : public SpellScriptLoader
{
    public:
        spell_gen_lifebloom(const char* name, uint32 spellId) : SpellScriptLoader(name), _spellId(spellId) { }

        class spell_gen_lifebloom_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_lifebloom_AuraScript);

        public:
            spell_gen_lifebloom_AuraScript(uint32 spellId) : AuraScript(), _spellId(spellId) { }

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(_spellId))
                    return false;
                return true;
            }

            void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                // Final heal only on duration end
				if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE && GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_ENEMY_SPELL)
                    return;

                // final heal
                GetTarget()->CastSpell(GetTarget(), _spellId, true, NULL, aurEff, GetCasterGUID());
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_lifebloom_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL);
            }

        private:
            uint32 _spellId;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_lifebloom_AuraScript(_spellId);
        }

    private:
        uint32 _spellId;
};

enum SummonElemental
{
    SPELL_SUMMON_FIRE_ELEMENTAL  = 8985,
    SPELL_SUMMON_EARTH_ELEMENTAL = 19704
};

class spell_gen_summon_elemental : public SpellScriptLoader
{
    public:
        spell_gen_summon_elemental(const char* name, uint32 spellId) : SpellScriptLoader(name), _spellId(spellId) { }

        class spell_gen_summon_elemental_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_summon_elemental_AuraScript);

        public:
            spell_gen_summon_elemental_AuraScript(uint32 spellId) : AuraScript(), _spellId(spellId) { }

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(_spellId))
                    return false;
                return true;
            }

            void AfterApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
				{
                    if (Unit* owner = GetCaster()->GetOwner())
                        owner->CastSpell(owner, _spellId, true);
				}
            }

            void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetCaster())
				{
                    if (Unit* owner = GetCaster()->GetOwner())
					{
                        if (owner->GetTypeId() == TYPE_ID_PLAYER) // todo: this check is maybe wrong
                            owner->ToPlayer()->RemovePet(NULL, PET_SAVE_NOT_IN_SLOT, true);
					}
				}
            }

            void Register()
            {
                 AfterEffectApply += AuraEffectApplyFn(spell_gen_summon_elemental_AuraScript::AfterApply, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                 AfterEffectRemove += AuraEffectRemoveFn(spell_gen_summon_elemental_AuraScript::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }

        private:
            uint32 _spellId;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_summon_elemental_AuraScript(_spellId);
        }

    private:
        uint32 _spellId;
};

enum Mounts
{
    SPELL_COLD_WEATHER_FLYING           = 54197,
    // Magic Broom
    SPELL_MAGIC_BROOM_60                = 42680,
    SPELL_MAGIC_BROOM_100               = 42683,
    SPELL_MAGIC_BROOM_150               = 42667,
    SPELL_MAGIC_BROOM_280               = 42668,
    // Headless Horseman's Mount
    SPELL_HEADLESS_HORSEMAN_MOUNT_60    = 51621,
    SPELL_HEADLESS_HORSEMAN_MOUNT_100   = 48024,
    SPELL_HEADLESS_HORSEMAN_MOUNT_150   = 51617,
    SPELL_HEADLESS_HORSEMAN_MOUNT_280   = 48023,
    // Winged Steed of the Ebon Blade
    SPELL_WINGED_STEED_150              = 54726,
    SPELL_WINGED_STEED_280              = 54727,
    // Big Love Rocket
    SPELL_BIG_LOVE_ROCKET_0             = 71343,
    SPELL_BIG_LOVE_ROCKET_60            = 71344,
    SPELL_BIG_LOVE_ROCKET_100           = 71345,
    SPELL_BIG_LOVE_ROCKET_150           = 71346,
    SPELL_BIG_LOVE_ROCKET_310           = 71347,
    // Invincible
    SPELL_INVINCIBLE_60                 = 72281,
    SPELL_INVINCIBLE_100                = 72282,
    SPELL_INVINCIBLE_150                = 72283,
    SPELL_INVINCIBLE_310                = 72284,
    // Blazing Hippogryph
    SPELL_BLAZING_HIPPOGRYPH_150        = 74854,
    SPELL_BLAZING_HIPPOGRYPH_280        = 74855,
    // Celestial Steed
    SPELL_CELESTIAL_STEED_60            = 75619,
    SPELL_CELESTIAL_STEED_100           = 75620,
    SPELL_CELESTIAL_STEED_150           = 75617,
    SPELL_CELESTIAL_STEED_280           = 75618,
    SPELL_CELESTIAL_STEED_310           = 76153,
    // X-53 Touring Rocket
    SPELL_X53_TOURING_ROCKET_150        = 75957,
    SPELL_X53_TOURING_ROCKET_280        = 75972,
    SPELL_X53_TOURING_ROCKET_310        = 76154,
};

class spell_gen_mount : public SpellScriptLoader
{
    public:
        spell_gen_mount(const char* name, uint32 mount0 = 0, uint32 mount60 = 0, uint32 mount100 = 0, uint32 mount150 = 0, uint32 mount280 = 0, uint32 mount310 = 0) : SpellScriptLoader(name),
            _mount0(mount0), _mount60(mount60), _mount100(mount100), _mount150(mount150), _mount280(mount280), _mount310(mount310) { }

        class spell_gen_mount_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_mount_SpellScript);

        public:
            spell_gen_mount_SpellScript(uint32 mount0, uint32 mount60, uint32 mount100, uint32 mount150, uint32 mount280, uint32 mount310) : SpellScript(),
                _mount0(mount0), _mount60(mount60), _mount100(mount100), _mount150(mount150), _mount280(mount280), _mount310(mount310) { }

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (_mount0 && !sSpellMgr->GetSpellInfo(_mount0))
                    return false;
                if (_mount60 && !sSpellMgr->GetSpellInfo(_mount60))
                    return false;
                if (_mount100 && !sSpellMgr->GetSpellInfo(_mount100))
                    return false;
                if (_mount150 && !sSpellMgr->GetSpellInfo(_mount150))
                    return false;
                if (_mount280 && !sSpellMgr->GetSpellInfo(_mount280))
                    return false;
                if (_mount310 && !sSpellMgr->GetSpellInfo(_mount310))
                    return false;
                return true;
            }

            void HandleMount(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                if (Player* target = GetHitPlayer())
                {
                    // Prevent stacking of mounts and client crashes upon dismounting
                    target->RemoveAurasByType(SPELL_AURA_MOUNTED, 0, GetHitAura());

                    // Triggered spell id dependent on riding skill and zone
                    bool canFly = false;
					uint32 map = GetVirtualMapForMapAndZone(target->GetMapId(), target->GetZoneId());
					if (map == 530 || (map == 571 && target->HasSpell(SPELL_COLD_WEATHER_FLYING)))
						canFly = true;

					float x, y, z;
					target->GetPosition(x, y, z);
					uint32 areaFlag = target->GetBaseMap()->GetAreaFlag(x, y, z);
					AreaTableEntry const* area = sAreaStore.LookupEntry(areaFlag);
                    if (!area || (canFly && (area->flags & AREA_FLAG_NO_FLY_ZONE)))
                        canFly = false;

                    uint32 mount = 0;
                    switch (target->GetBaseSkillValue(SKILL_RIDING))
                    {
                        case 0:
                            mount = _mount0;
                            break;
                        case 75:
                            mount = _mount60;
                            break;
                        case 150:
                            mount = _mount100;
                            break;
                        case 225:
                            if (canFly)
                                mount = _mount150;
                            else
                                mount = _mount100;
                            break;
                        case 300:
                            if (canFly)
                            {
                                if (_mount310 && target->Has310Flyer(false))
                                    mount = _mount310;
                                else
                                    mount = _mount280;
                            }
                            else
                                mount = _mount100;
                            break;
                        default:
                            break;
                    }

                    if (mount)
                    {
                        PreventHitAura();
                        target->CastSpell(target, mount, true);
                    }
                }
            }

            void Register()
            {
                 OnEffectHitTarget += SpellEffectFn(spell_gen_mount_SpellScript::HandleMount, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }

        private:
            uint32 _mount0;
            uint32 _mount60;
            uint32 _mount100;
            uint32 _mount150;
            uint32 _mount280;
            uint32 _mount310;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_mount_SpellScript(_mount0, _mount60, _mount100, _mount150, _mount280, _mount310);
        }

    private:
        uint32 _mount0;
        uint32 _mount60;
        uint32 _mount100;
        uint32 _mount150;
        uint32 _mount280;
        uint32 _mount310;
};

enum TheTurkinator
{
    SPELL_KILL_COUNTER_VISUAL     = 62015,
    SPELL_KILL_COUNTER_VISUAL_MAX = 62021,

	TEXT_THE_TURKINATOR_10        = -1310050,
	TEXT_THE_TURKINATOR_20        = -1310051,
	TEXT_THE_TURKINATOR_30        = -1310052,
	TEXT_THE_TURKINATOR_40        = -1310053,
};

class spell_gen_turkey_tracker : public SpellScriptLoader
{
    public:
        spell_gen_turkey_tracker() : SpellScriptLoader("spell_gen_turkey_tracker") { }

        class spell_gen_turkey_tracker_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_turkey_tracker_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_KILL_COUNTER_VISUAL))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_KILL_COUNTER_VISUAL_MAX))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster()->GetAura(SPELL_KILL_COUNTER_VISUAL_MAX))
                    return;

                Player* target = GetHitPlayer();
                if (!target)
                    return;

                if (Aura const* aura = GetCaster()->ToPlayer()->GetAura(GetSpellInfo()->Id))
                {
                    switch (aura->GetStackAmount())
                    {
                    case 10:
						DoSendQuantumText(TEXT_THE_TURKINATOR_10, target);
                        GetCaster()->CastSpell(target, SPELL_KILL_COUNTER_VISUAL);
                        break;
                    case 20:
                        DoSendQuantumText(TEXT_THE_TURKINATOR_20, target);
                        GetCaster()->CastSpell(target, SPELL_KILL_COUNTER_VISUAL);
                        break;
                    case 30:
                        DoSendQuantumText(TEXT_THE_TURKINATOR_30, target);
                        GetCaster()->CastSpell(target, SPELL_KILL_COUNTER_VISUAL);
                        break;
                    case 40:
                        DoSendQuantumText(TEXT_THE_TURKINATOR_40, target);
                        GetCaster()->CastSpell(target, SPELL_KILL_COUNTER_VISUAL);
                        GetCaster()->CastSpell(target, SPELL_KILL_COUNTER_VISUAL_MAX);
                        break;
                    default:
                        break;
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_turkey_tracker_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_turkey_tracker_SpellScript();
        }
};

class spell_gen_feast_on : public SpellScriptLoader
{
    public:
        spell_gen_feast_on() : SpellScriptLoader("spell_gen_feast_on") { }

        class spell_gen_feast_on_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_feast_on_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                int32 bp0 = GetSpellInfo()->Effects[EFFECT_0].CalcValue();

                Unit* caster = GetCaster();
				if (caster->GetVehicleKit())
				{
                    if (Unit* player = caster->GetVehicleKit()->GetPassenger(0))
                        caster->CastSpell(player, bp0, true, NULL, NULL, player->ToPlayer()->GetGUID());
				}
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_feast_on_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_feast_on_SpellScript();
        }
};

enum WellFedPilgrimsBounty
{
    // Feast On
    SPELL_A_SERVING_OF_TURKEY         = 61807,
    SPELL_A_SERVING_OF_CRANBERRIES    = 61804,
    SPELL_A_SERVING_OF_STUFFING       = 61806,
    SPELL_A_SERVING_OF_SWEET_POTATOES = 61808,
    SPELL_A_SERVING_OF_PIE            = 61805,
    // Well Fed
    SPELL_WELL_FED_AP                 = 65414,
    SPELL_WELL_FED_ZM                 = 65412,
    SPELL_WELL_FED_HIT                = 65416,
    SPELL_WELL_FED_HASTE              = 65410,
    SPELL_WELL_FED_SPIRIT             = 65415,
    // Pilgrim's Paunch
    SPELL_THE_SPIRIT_OF_SHARING       = 61849,
};

class spell_gen_well_fed_pilgrims_bounty : public SpellScriptLoader
{
    private:
        uint32 _triggeredSpellId1;
        uint32 _triggeredSpellId2;

    public:
        spell_gen_well_fed_pilgrims_bounty(const char* name, uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScriptLoader(name),
            _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

        class spell_gen_well_fed_pilgrims_bounty_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_well_fed_pilgrims_bounty_SpellScript)
        private:
            uint32 _triggeredSpellId1;
            uint32 _triggeredSpellId2;

        public:
            spell_gen_well_fed_pilgrims_bounty_SpellScript(uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScript(),
                _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(_triggeredSpellId2))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Player* target = GetHitPlayer();
                if (!target)
                    return;

                Aura const* Turkey = target->GetAura(SPELL_A_SERVING_OF_TURKEY);
                Aura const* Cranberies = target->GetAura(SPELL_A_SERVING_OF_CRANBERRIES);
                Aura const* Stuffing = target->GetAura(SPELL_A_SERVING_OF_STUFFING);
                Aura const* SweetPotatoes = target->GetAura(SPELL_A_SERVING_OF_SWEET_POTATOES);
                Aura const* Pie = target->GetAura(SPELL_A_SERVING_OF_PIE);

                if (Aura const* aura = target->GetAura(_triggeredSpellId1))
                {
                    if (aura->GetStackAmount() == 5)
                        target->CastSpell(target, _triggeredSpellId2, true);
                }

                // The Spirit of Sharing - Achievement Credit
                if (!target->GetAura(SPELL_THE_SPIRIT_OF_SHARING))
                {
                    if ((Turkey && Turkey->GetStackAmount() == 5) && (Cranberies && Cranberies->GetStackAmount() == 5) && (Stuffing && Stuffing->GetStackAmount() == 5) &&
                        (SweetPotatoes && SweetPotatoes->GetStackAmount() == 5) && (Pie && Pie->GetStackAmount() == 5))
                        target->CastSpell(target, SPELL_THE_SPIRIT_OF_SHARING, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_well_fed_pilgrims_bounty_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_well_fed_pilgrims_bounty_SpellScript(_triggeredSpellId1, _triggeredSpellId2);
        }
};

enum OnPlatePilgrimsBounty
{
    // "FOOD FIGHT!" - Achivement Credit
    SPELL_ON_PLATE_TURKEY           = 61928,
    SPELL_ON_PLATE_CRANBERRIES      = 61925,
    SPELL_ON_PLATE_STUFFING         = 61927,
    SPELL_ON_PLATE_SWEET_POTATOES   = 61929,
    SPELL_ON_PLATE_PIE              = 61926,

    // Sharing is Caring - Achivement Credit
    SPELL_PASS_THE_TURKEY           = 66373,
    SPELL_PASS_THE_CRANBERRIES      = 66372,
    SPELL_PASS_THE_STUFFING         = 66375,
    SPELL_PASS_THE_SWEET_POTATOES   = 66376,
    SPELL_PASS_THE_PIE              = 66374,
};

class spell_gen_on_plate_pilgrims_bounty : public SpellScriptLoader
{
    private:
        uint32 _triggeredSpellId1;
        uint32 _triggeredSpellId2;

    public:
        spell_gen_on_plate_pilgrims_bounty(const char* name, uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScriptLoader(name),
            _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

        class spell_gen_on_plate_pilgrims_bounty_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_on_plate_pilgrims_bounty_SpellScript);
        private:
            uint32 _triggeredSpellId1;
            uint32 _triggeredSpellId2;

        public:
            spell_gen_on_plate_pilgrims_bounty_SpellScript(uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScript(),
                _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(_triggeredSpellId1))
                    return false;
                if (!sSpellMgr->GetSpellInfo(_triggeredSpellId2))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
				if (caster->GetVehicleKit())
                {
                    Unit* player = caster->GetVehicleKit()->GetPassenger(0);
                    if (!player)
                        return;

                    player->CastSpell(GetHitUnit(), _triggeredSpellId1, true, NULL, NULL, player->ToPlayer()->GetGUID());
                    player->CastSpell(player, _triggeredSpellId2, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_on_plate_pilgrims_bounty_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_on_plate_pilgrims_bounty_SpellScript(_triggeredSpellId1, _triggeredSpellId2);
        }
};

enum BountifulFeast
{
    // Bountiful Feast
    SPELL_BOUNTIFUL_FEAST_DRINK       = 66041,
    SPELL_BOUNTIFUL_FEAST_FOOD        = 66478,
	SPELL_FOOD                        = 65422,
    SPELL_BOUNTIFUL_FEAST_REFRESHMENT = 66622,
};

class spell_gen_bountiful_feast : public SpellScriptLoader
{
    public:
        spell_gen_bountiful_feast() : SpellScriptLoader("spell_gen_bountiful_feast") { }

        class spell_gen_bountiful_feast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_bountiful_feast_SpellScript);

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                caster->CastSpell(caster, SPELL_BOUNTIFUL_FEAST_DRINK, true);
                caster->CastSpell(caster, SPELL_BOUNTIFUL_FEAST_FOOD, true);
				caster->CastSpell(caster, SPELL_FOOD, true);
                caster->CastSpell(caster, SPELL_BOUNTIFUL_FEAST_REFRESHMENT, true);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_bountiful_feast_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_bountiful_feast_SpellScript();
        }
};

enum PilgrimsBountyBuffFood
{
    // Pilgrims Bounty Buff Food
    SPELL_WELL_FED_AP_TRIGGER       = 65414,
    SPELL_WELL_FED_ZM_TRIGGER       = 65412,
    SPELL_WELL_FED_HIT_TRIGGER      = 65416,
    SPELL_WELL_FED_HASTE_TRIGGER    = 65410,
    SPELL_WELL_FED_SPIRIT_TRIGGER   = 65415
};

class spell_pilgrims_bounty_buff_food : public SpellScriptLoader
{
    private:
        uint32 _triggeredSpellId;
    public:
        spell_pilgrims_bounty_buff_food(const char* name, uint32 triggeredSpellId) : SpellScriptLoader(name), _triggeredSpellId(triggeredSpellId) { }

        class spell_pilgrims_bounty_buff_food_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pilgrims_bounty_buff_food_AuraScript)
        private:
            uint32 _triggeredSpellId;

        public:
            spell_pilgrims_bounty_buff_food_AuraScript(uint32 triggeredSpellId) : AuraScript(), _triggeredSpellId(triggeredSpellId) { }

            bool Load()
            {
                _handled = false;
                return true;
            }

            void HandleTriggerSpell(AuraEffect const* /*aurEff*/)
            {
                if (_handled)
                    return;

                _handled = true;
                GetTarget()->CastSpell(GetTarget(), _triggeredSpellId, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pilgrims_bounty_buff_food_AuraScript::HandleTriggerSpell, EFFECT_2, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }

            bool _handled;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pilgrims_bounty_buff_food_AuraScript(_triggeredSpellId);
        }
};

enum ScarletSpells
{
    SPELL_SCARLET_RAVEN_PRIEST_IMAGE_MALE   = 48763,
    SPELL_SCARLET_RAVEN_PRIEST_IMAGE_FEMALE = 48761,
};

class spell_gen_scarlet_raven_priest_image : public SpellScriptLoader
{
public:
    spell_gen_scarlet_raven_priest_image() : SpellScriptLoader("spell_gen_scarlet_raven_priest_image") {}

    class spell_gen_scarlet_raven_priest_image_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_scarlet_raven_priest_image_SpellScript);

        bool Validate(SpellInfo const* /*SpellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_SCARLET_RAVEN_PRIEST_IMAGE_MALE) || !sSpellMgr->GetSpellInfo(SPELL_SCARLET_RAVEN_PRIEST_IMAGE_FEMALE))
                return false;
            return true;
        }

        void HandleScript(SpellEffIndex  /*effIndex*/)
        {            
            if (Unit* target = GetHitUnit())
            {
                if (Player* player = target->ToPlayer())
                    player->CastSpell(player, player->GetCurrentGender() == GENDER_FEMALE ? SPELL_SCARLET_RAVEN_PRIEST_IMAGE_FEMALE : SPELL_SCARLET_RAVEN_PRIEST_IMAGE_MALE, false);
            }     
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_scarlet_raven_priest_image_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_scarlet_raven_priest_image_SpellScript();
    }
};

enum Fumping
{
	SPELL_FUMPING          = 39238,

    NPC_MATURE_BONE_SIFTER = 22482,
    NPC_SAND_GNOME         = 22483,
};

class spell_gen_fumping_39238 : public SpellScriptLoader
{
public:
    spell_gen_fumping_39238() : SpellScriptLoader("spell_gen_fumping_39238") {}

    class spell_gen_fumping_39238SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_fumping_39238SpellScript);

        bool Validate(SpellInfo const * /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_FUMPING))
				return false;
			return true;
		}

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Unit* player = GetCaster())
            {
                switch (urand(1, 2))
                {
                    case 1:
						player->SummonCreature(NPC_MATURE_BONE_SIFTER, player->GetPositionX()+rand()%10, player->GetPositionY()+rand()%10, player->GetPositionZ()+5, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);
						break;
                    case 2:
						player->SummonCreature(NPC_SAND_GNOME, player->GetPositionX()+rand()%10, player->GetPositionY()+rand()%10, player->GetPositionZ()+5, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);
						break;
                }
            }
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_gen_fumping_39238SpellScript::HandleDummy, EFFECT_ALL, SPELL_EFFECT_ANY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_fumping_39238SpellScript();
    }
};

enum AnotherFumpingSpellData
{
	SPELL_FUMPING2 = 39246,
	NPC_HAISHULUD  = 22038,
};

class spell_gen_fumping_39246 : public SpellScriptLoader
{
public:
    spell_gen_fumping_39246() : SpellScriptLoader("spell_gen_fumping_39246") {}

    class spell_gen_fumping_39246SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_fumping_39246SpellScript);

        bool Validate(SpellInfo const * /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_FUMPING2))
                return false;
            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Unit* player = GetCaster())
            {
                switch (urand(1, 3))
                {
                    case 1:
                        player->SummonCreature(NPC_MATURE_BONE_SIFTER, player->GetPositionX()+rand()%10, player->GetPositionY()+rand()%10, player->GetPositionZ()+5, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);
                        player->SummonCreature(NPC_MATURE_BONE_SIFTER, player->GetPositionX()+rand()%10, player->GetPositionY()-rand()%10, player->GetPositionZ()+5, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);
                        player->SummonCreature(NPC_MATURE_BONE_SIFTER, player->GetPositionX()-rand()%10, player->GetPositionY()+rand()%10, player->GetPositionZ()+5, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);
                        break;
                    case 2:
                        player->SummonCreature(NPC_SAND_GNOME, player->GetPositionX()+rand()%10, player->GetPositionY()+rand()%10, player->GetPositionZ()+5, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);
                        player->SummonCreature(NPC_SAND_GNOME, player->GetPositionX()+rand()%10, player->GetPositionY()-rand()%10, player->GetPositionZ()+5, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);
                        player->SummonCreature(NPC_SAND_GNOME, player->GetPositionX()-rand()%10, player->GetPositionY()+rand()%10, player->GetPositionZ()+5, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 300000);
                        break;
                    case 3:
                        player->SummonCreature(NPC_HAISHULUD, player->GetPositionX()+rand()%5, player->GetPositionY()+rand()%5, player->GetPositionZ()+5, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000);
                        break;
                }
            }
        }
        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_gen_fumping_39246SpellScript::HandleDummy, EFFECT_ALL, SPELL_EFFECT_ANY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_fumping_39246SpellScript();
    }
};

class spell_gen_shadowmeld : public SpellScriptLoader
{
    public:
        spell_gen_shadowmeld() : SpellScriptLoader("spell_gen_shadowmeld") { }

        class spell_gen_shadowmeld_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_shadowmeld_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                caster->InterruptSpell(CURRENT_AUTOREPEAT_SPELL); // break Auto Shot and autohit
                caster->InterruptSpell(CURRENT_CHANNELED_SPELL); // break channeled spells

                bool InstantExit = true;
                if (Player* player = caster->ToPlayer()) // if is a creature instant exits combat, else check if someone in party is in combat in visibility distance
                {
                    uint64 myGUID = player->GetGUID();
                    float visibilityRange = player->GetMap()->GetVisibilityRange();
                    if (Group *group = player->GetGroup())
                    {
                        const Group::MemberSlotList membersList = group->GetMemberSlots();
                        for (Group::member_citerator itr=membersList.begin(); itr!=membersList.end() && InstantExit; ++itr)
						{
                            if (itr->guid != myGUID)
							{
                                if (Player* GroupMember = Unit::GetPlayer(*player, itr->guid))
								{
                                    if (GroupMember->IsInCombatActive() && player->GetMap()==GroupMember->GetMap() && player->IsWithinDistInMap(GroupMember, visibilityRange))
										InstantExit = false;
								}
							}
						}
                    }

                    player->SendAttackSwingCancelAttack();
				}

                if (!caster->GetInstanceScript() || !caster->GetInstanceScript()->IsEncounterInProgress()) //Don't leave combat if you are in combat with a boss
                {
                    if (!InstantExit)
                        caster->getHostileRefManager().deleteReferences(); // exit combat after 6 seconds
                    else caster->CombatStop(); // isn't necessary to call AttackStop because is just called in CombatStop
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_shadowmeld_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_shadowmeld_SpellScript();
        }
};

enum HalloweenData
{
    SPELL_PIRATE_COSTUME_MALE        = 24708,
    SPELL_PIRATE_COSTUME_FEMALE      = 24709,
    SPELL_NINJA_COSTUME_MALE         = 24711,
    SPELL_NINJA_COSTUME_FEMALE       = 24710,
    SPELL_LEPER_GNOME_COSTUME_MALE   = 24712,
    SPELL_LEPER_GNOME_COSTUME_FEMALE = 24713,
    SPELL_GHOST_COSTUME_MALE         = 24735,
    SPELL_GHOST_COSTUME_FEMALE       = 24736,

    SPELL_HALLOWEEN_WAND_PIRATE      = 24717,
    SPELL_HALLOWEEN_WAND_NINJA       = 24718,
    SPELL_HALLOWEEN_WAND_LEPER_GNOME = 24719,
    SPELL_HALLOWEEN_WAND_RANDOM      = 24720,
    SPELL_HALLOWEEN_WAND_SKELETON    = 24724,
    SPELL_HALLOWEEN_WAND_WISP        = 24733,
    SPELL_HALLOWEEN_WAND_GHOST       = 24737,
    SPELL_HALLOWEEN_WAND_BAT         = 24741,
};

class spell_gen_halloween_wand : public SpellScriptLoader
{
public:
    spell_gen_halloween_wand() : SpellScriptLoader("spell_gen_halloween_wand") { }

    class spell_gen_halloween_wand_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_halloween_wand_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PIRATE_COSTUME_MALE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_PIRATE_COSTUME_FEMALE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_NINJA_COSTUME_MALE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_NINJA_COSTUME_FEMALE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_LEPER_GNOME_COSTUME_MALE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_LEPER_GNOME_COSTUME_FEMALE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_GHOST_COSTUME_MALE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_GHOST_COSTUME_FEMALE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_PIRATE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_NINJA))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_LEPER_GNOME))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_RANDOM))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_SKELETON))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_WISP))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_GHOST))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWEEN_WAND_BAT))
                return false;
            return true;
        }

        void HandleScriptEffect()
        {
            Unit* caster = GetCaster();
            Unit* target = GetHitPlayer();

            if (!caster || !target)
                return;

            uint32 spellId = 0;
            uint8 gender = target->GetCurrentGender();

            switch (GetSpellInfo()->Id)
            {
                case SPELL_HALLOWEEN_WAND_LEPER_GNOME:
                    spellId = gender ? SPELL_LEPER_GNOME_COSTUME_FEMALE : SPELL_LEPER_GNOME_COSTUME_MALE;
                    break;
                case SPELL_HALLOWEEN_WAND_PIRATE:
                    spellId = gender ? SPELL_PIRATE_COSTUME_FEMALE : SPELL_PIRATE_COSTUME_MALE;
                    break;
                case SPELL_HALLOWEEN_WAND_GHOST:
                    spellId = gender ? SPELL_GHOST_COSTUME_FEMALE : SPELL_GHOST_COSTUME_MALE;
                    break;
                case SPELL_HALLOWEEN_WAND_NINJA:
                    spellId = gender ? SPELL_NINJA_COSTUME_FEMALE : SPELL_NINJA_COSTUME_MALE;
                    break;
                case SPELL_HALLOWEEN_WAND_RANDOM:
                    spellId = RAND(SPELL_HALLOWEEN_WAND_PIRATE, SPELL_HALLOWEEN_WAND_NINJA, SPELL_HALLOWEEN_WAND_LEPER_GNOME, SPELL_HALLOWEEN_WAND_SKELETON, SPELL_HALLOWEEN_WAND_WISP, SPELL_HALLOWEEN_WAND_GHOST, SPELL_HALLOWEEN_WAND_BAT);
                    break;
            }
            caster->CastSpell(target, spellId, true);
        }

        void Register()
        {
            AfterHit += SpellHitFn(spell_gen_halloween_wand_SpellScript::HandleScriptEffect);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_halloween_wand_SpellScript();
    }
};

enum BarricadeData
{
    SPELL_CONSTRUCT_BARRICADE = 59925,
    SPELL_SUMMON_BARRICADE_A  = 59922,
    SPELL_SUMMON_BARRICADE_B  = 59923,
    SPELL_SUMMON_BARRICADE_C  = 59924,
    NPC_EBON_BLADE_MARKER     = 31887,
};

class spell_gen_construct_barricade : public SpellScriptLoader
{
public:
    spell_gen_construct_barricade() : SpellScriptLoader("spell_gen_construct_barricade") { }

    class spell_gen_construct_barricade_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_construct_barricade_SpellScript);

        bool Validate(SpellInfo const * /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_CONSTRUCT_BARRICADE))
                return false;
            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Unit* caster = GetCaster())
			{
                if (Player* player = caster->ToPlayer())
                {
                    uint32 SummonBarricadeSpell = 0;

                    switch (urand(1, 3))
                    {
                        case 1:
							SummonBarricadeSpell = SPELL_SUMMON_BARRICADE_A;
							break;
                        case 2:
							SummonBarricadeSpell = SPELL_SUMMON_BARRICADE_B;
							break;
                        case 3:
							SummonBarricadeSpell = SPELL_SUMMON_BARRICADE_C;
							break;
                    }
                    player->CastSpell(player, SummonBarricadeSpell, true);
                    player->KilledMonsterCredit(NPC_EBON_BLADE_MARKER, 0);
                }
			}
        }
        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_gen_construct_barricade_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_construct_barricade_SpellScript();
    }
};

enum TossStinkyBomb
{
	NPC_STINKY_BOMB_CREDIT = 15415,
};

class spell_gen_toss_stinky_bomb : public SpellScriptLoader
{
public:
    spell_gen_toss_stinky_bomb() : SpellScriptLoader("spell_gen_toss_stinky_bomb") {}

    class spell_gen_toss_stinky_bomb_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_toss_stinky_bomb_SpellScript);

        void HandleScriptEffect(SpellEffIndex effIndex)
        {
            Unit* caster = GetCaster();

            if (caster->GetTypeId() == TYPE_ID_PLAYER)
                caster->ToPlayer()->KilledMonsterCredit(NPC_STINKY_BOMB_CREDIT, 0);
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_gen_toss_stinky_bomb_SpellScript::HandleScriptEffect, EFFECT_2, SPELL_EFFECT_SEND_EVENT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_toss_stinky_bomb_SpellScript();
    }
};

enum CleanStinkyBomb
{
	GO_STINKY_BOMB_CLOUD = 180450,
};

class spell_gen_clean_stinky_bomb : public SpellScriptLoader
{
public:
    spell_gen_clean_stinky_bomb() : SpellScriptLoader("spell_gen_clean_stinky_bomb") {}

    class spell_gen_clean_stinky_bomb_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_clean_stinky_bomb_SpellScript);

        SpellCastResult CheckIfNearBomb()
        {
            Unit* caster = GetCaster();

            if (GameObject* stinky = GetClosestGameObjectWithEntry(caster, GO_STINKY_BOMB_CLOUD, 15.0f))
			{
                return SPELL_CAST_OK;
			}
            else
			{
                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
			}
        }

        void HandleCleanBombEffect(SpellEffIndex effIndex)
        {
            Unit* caster = GetCaster();

            if (GameObject* stinky = GetClosestGameObjectWithEntry(caster, GO_STINKY_BOMB_CLOUD, 15.0f))
                stinky->RemoveFromWorld();
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_gen_clean_stinky_bomb_SpellScript::CheckIfNearBomb);
            OnEffectHit += SpellEffectFn(spell_gen_clean_stinky_bomb_SpellScript::HandleCleanBombEffect, EFFECT_1, SPELL_EFFECT_ACTIVATE_OBJECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_clean_stinky_bomb_SpellScript();
    }
};

enum FoamSword
{
    ITEM_FOAM_SWORD_GREEN   = 45061,
    ITEM_FOAM_SWORD_PINK    = 45176,
    ITEM_FOAM_SWORD_BLUE    = 45177,
    ITEM_FOAM_SWORD_RED     = 45178,
    ITEM_FOAM_SWORD_YELLOW  = 45179,

    SPELL_BONKED            = 62991,
    SPELL_FOAM_SWORD_DEFEAT = 62994,
    SPELL_ON_GUARD          = 62972,
};

class spell_gen_upper_deck_create_foam_sword : public SpellScriptLoader
{
    public:
        spell_gen_upper_deck_create_foam_sword() : SpellScriptLoader("spell_gen_upper_deck_create_foam_sword") { }

        class spell_gen_upper_deck_create_foam_sword_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_upper_deck_create_foam_sword_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                if (Player* player = GetHitPlayer())
                {
                    static uint32 const itemId[5] =
					{
						ITEM_FOAM_SWORD_GREEN, ITEM_FOAM_SWORD_PINK, ITEM_FOAM_SWORD_BLUE, ITEM_FOAM_SWORD_RED, ITEM_FOAM_SWORD_YELLOW
					};
                    // player can only have one of these items
                    for (uint8 i = 0; i < 5; ++i)
                    {
                        if (player->HasItemCount(itemId[i], 1, true))
                            return;
                    }

                    CreateItem(effIndex, itemId[urand(0, 4)]);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_upper_deck_create_foam_sword_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_upper_deck_create_foam_sword_SpellScript();
        }
};

class spell_gen_bonked : public SpellScriptLoader
{
    public:
        spell_gen_bonked() : SpellScriptLoader("spell_gen_bonked") { }

        class spell_gen_bonked_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_bonked_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Player* player = GetHitPlayer())
                {
                    Aura const* aura = GetHitAura();
                    if (!(aura && aura->GetStackAmount() == 3))
                        return;

                    player->CastSpell(player, SPELL_FOAM_SWORD_DEFEAT, true);
                    player->RemoveAurasDueToSpell(SPELL_BONKED);

                    if (Aura const* aura = player->GetAura(SPELL_ON_GUARD))
                    {
                        if (Item* item = player->GetItemByGuid(aura->GetCastItemGUID()))
                            player->DestroyItemCount(item->GetEntry(), 1, true);
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_bonked_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_bonked_SpellScript();
        }
};

class spell_gen_gift_of_naaru : public SpellScriptLoader
{
    public:
        spell_gen_gift_of_naaru() : SpellScriptLoader("spell_gen_gift_of_naaru") { }

        class spell_gen_gift_of_naaru_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_gift_of_naaru_AuraScript);

            void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (!GetCaster())
                    return;

                float heal = 0.0f;
                switch (GetSpellInfo()->SpellFamilyName)
                {
                    case SPELL_FAMILY_MAGE:
                    case SPELL_FAMILY_WARLOCK:
                    case SPELL_FAMILY_PRIEST:
                        heal = 1.885f * float(GetCaster()->SpellBaseDamageBonusDone(GetSpellInfo()->GetSchoolMask()));
                        break;
                    case SPELL_FAMILY_PALADIN:
                    case SPELL_FAMILY_SHAMAN:
                        heal = std::max(1.885f * float(GetCaster()->SpellBaseDamageBonusDone(GetSpellInfo()->GetSchoolMask())), 1.1f * float(GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK)));
                        break;
                    case SPELL_FAMILY_WARRIOR:
                    case SPELL_FAMILY_HUNTER:
                    case SPELL_FAMILY_DEATH_KNIGHT:
                        heal = 1.1f * float(std::max(GetCaster()->GetTotalAttackPowerValue(BASE_ATTACK), GetCaster()->GetTotalAttackPowerValue(RANGED_ATTACK)));
                        break;
                    case SPELL_FAMILY_GENERIC:
                    default:
                        break;
                }

                int32 healTick = floor(heal / aurEff->GetTotalTicks());
                amount += int32(std::max(healTick, 0));
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_gift_of_naaru_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_gift_of_naaru_AuraScript();
        }
};

class spell_gen_av_honorable_defender : public SpellScriptLoader
{
    public:
        spell_gen_av_honorable_defender() : SpellScriptLoader("spell_gen_av_honorable_defender") { }

        class spell_gen_av_honorable_defender_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_av_honorable_defender_AuraScript);

            bool CheckAreaTarget(Unit* target)
            {
                if (target->GetTypeId() == TYPE_ID_PLAYER)
                    return true;
                return false;
            }

            void Register()
            {
                DoCheckAreaTarget += AuraCheckAreaTargetFn(spell_gen_av_honorable_defender_AuraScript::CheckAreaTarget);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_av_honorable_defender_AuraScript();
        }
};

enum CloseRift
{
    SPELL_DESPAWN_RIFT = 61665,
};

class spell_gen_close_rift : public SpellScriptLoader
{
    public:
        spell_gen_close_rift() : SpellScriptLoader("spell_gen_close_rift") { }

        class spell_gen_close_rift_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_close_rift_AuraScript);

			uint8 counter;

            bool Load()
            {
                counter = 0;
                return true;
            }

            bool Validate(SpellInfo const* /*spell*/)
            {
                return sSpellMgr->GetSpellInfo(SPELL_DESPAWN_RIFT);
            }

            void HandlePeriodic(AuraEffect const* /* aurEff */)
            {
                if (++counter == 5)
                    GetTarget()->CastSpell((Unit*)NULL, SPELL_DESPAWN_RIFT, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_close_rift_AuraScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_close_rift_AuraScript();
        }
};

enum Replenishment
{
    SPELL_REPLENISHMENT          = 57669,
    SPELL_INFINITE_REPLENISHMENT = 61782,
};

class spell_gen_replenishment : public SpellScriptLoader
{
    public:
        spell_gen_replenishment() : SpellScriptLoader("spell_gen_replenishment") { }

        class spell_gen_replenishment_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_replenishment_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_REPLENISHMENT) ||
                   !sSpellMgr->GetSpellInfo(SPELL_INFINITE_REPLENISHMENT))
                    return false;
                return true;
            }

            bool Load()
            {
                return GetUnitOwner()->GetPower(POWER_MANA);
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                switch (GetSpellInfo()->Id)
                {
                    case SPELL_REPLENISHMENT:
                        amount = GetUnitOwner()->GetMaxPower(POWER_MANA) * 0.002f;
                        break;
                    case SPELL_INFINITE_REPLENISHMENT:
                        amount = GetUnitOwner()->GetMaxPower(POWER_MANA) * 0.0025f;
                        break;
                    default:
                        break;
                }
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_replenishment_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_ENERGIZE);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_replenishment_AuraScript();
        }
};

enum ServiceUniform
{
    SPELL_SERVICE_UNIFORM = 71450,

    MODEL_GOBLIN_MALE     = 31002,
    MODEL_GOBLIN_FEMALE   = 31003,
};

class spell_gen_aura_service_uniform : public SpellScriptLoader
{
    public:
        spell_gen_aura_service_uniform() : SpellScriptLoader("spell_gen_aura_service_uniform") { }

        class spell_gen_aura_service_uniform_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_aura_service_uniform_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SERVICE_UNIFORM))
                    return false;
                return true;
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                // Apply model goblin
                Unit* target = GetTarget();
                if (target->GetTypeId() == TYPE_ID_PLAYER)
                {
                    if (target->GetCurrentGender() == GENDER_MALE)
                        target->SetDisplayId(MODEL_GOBLIN_MALE);
                    else
                        target->SetDisplayId(MODEL_GOBLIN_FEMALE);
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                if (target->GetTypeId() == TYPE_ID_PLAYER)
                    target->RestoreDisplayId();
            }

            void Register()
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_gen_aura_service_uniform_AuraScript::OnApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_aura_service_uniform_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_aura_service_uniform_AuraScript();
        }
};

enum OrcDisguiseSpells
{
    SPELL_ORC_DISGUISE_TRIGGER = 45759,
    SPELL_ORC_DISGUISE_MALE    = 45760,
    SPELL_ORC_DISGUISE_FEMALE  = 45762,
};

class spell_gen_orc_disguise : public SpellScriptLoader
{
    public:
        spell_gen_orc_disguise() : SpellScriptLoader("spell_gen_orc_disguise") { }

        class spell_gen_orc_disguise_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_orc_disguise_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ORC_DISGUISE_TRIGGER) || !sSpellMgr->GetSpellInfo(SPELL_ORC_DISGUISE_MALE) || 
                    !sSpellMgr->GetSpellInfo(SPELL_ORC_DISGUISE_FEMALE))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Player* target = GetHitPlayer())
                {
                    uint8 gender = target->GetCurrentGender();
                    if (!gender)
                        caster->CastSpell(target, SPELL_ORC_DISGUISE_MALE, true);
                    else
                        caster->CastSpell(target, SPELL_ORC_DISGUISE_FEMALE, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_orc_disguise_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_orc_disguise_SpellScript();
        }
};

enum WhisperGulchYoggSaronWhisper
{
	SPELL_YOGG_SARON_WHISPER_DUMMY = 29072,
};

class spell_gen_whisper_gulch_yogg_saron_whisper : public SpellScriptLoader
{
    public:
        spell_gen_whisper_gulch_yogg_saron_whisper() : SpellScriptLoader("spell_gen_whisper_gulch_yogg_saron_whisper") { }

        class spell_gen_whisper_gulch_yogg_saron_whisper_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_whisper_gulch_yogg_saron_whisper_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_YOGG_SARON_WHISPER_DUMMY))
                    return false;
                return true;
            }

            void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();
				GetTarget()->CastSpell((Unit*)NULL, SPELL_YOGG_SARON_WHISPER_DUMMY, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_whisper_gulch_yogg_saron_whisper_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_whisper_gulch_yogg_saron_whisper_AuraScript();
        }
};

enum SpectatorCheerTrigger

{
    EMOTE_ONE_SHOT_CHEER       = 4,
    EMOTE_ONE_SHOT_EXCLAMATION = 5,
    EMOTE_ONE_SHOT_APPLAUD     = 21,
};

uint8 const EmoteArray [3] = { EMOTE_ONE_SHOT_CHEER, EMOTE_ONE_SHOT_EXCLAMATION, EMOTE_ONE_SHOT_APPLAUD };

class spell_gen_spectator_cheer_trigger : public SpellScriptLoader
{
    public:
        spell_gen_spectator_cheer_trigger() : SpellScriptLoader("spell_gen_spectator_cheer_trigger") { }

        class spell_gen_spectator_cheer_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_spectator_cheer_trigger_SpellScript)

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetCaster())
                    caster->HandleEmoteCommand(EmoteArray [urand(0,2)]);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_spectator_cheer_trigger_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_spectator_cheer_trigger_SpellScript();
        }
};

enum VendorBarkTrigger
{
    NPC_AMPHITHEATER_VENDOR = 30098,
    SAY_AMPHITHEATER_VENDOR = 0,
};

class spell_gen_vendor_bark_trigger : public SpellScriptLoader
{
    public:
        spell_gen_vendor_bark_trigger() : SpellScriptLoader("spell_gen_vendor_bark_trigger") { }

        class spell_gen_vendor_bark_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_vendor_bark_trigger_SpellScript)

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Creature* vendor = GetCaster()->ToCreature())
                    if (vendor->GetEntry() == NPC_AMPHITHEATER_VENDOR)
                        vendor->AI()->Talk(SAY_AMPHITHEATER_VENDOR);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_vendor_bark_trigger_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_vendor_bark_trigger_SpellScript();
        }
};

class spell_gen_glacial_strike : public SpellScriptLoader
{
public:
	spell_gen_glacial_strike() : SpellScriptLoader("spell_gen_glacial_strike") {}

	class spell_gen_glacial_strike_AuraScript : public AuraScript
	{
		PrepareAuraScript(spell_gen_glacial_strike_AuraScript);

		void PeriodicTick(AuraEffect const* /*aurEff*/)
		{
			if (GetTarget()->IsFullHealth())
			{
				GetTarget()->RemoveAura(GetId(), 0, 0, AURA_REMOVE_BY_ENEMY_SPELL);
				PreventDefaultAction();
			}
		}

		void Register()
		{
			OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_glacial_strike_AuraScript::PeriodicTick, EFFECT_2, SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
		}
	};

	AuraScript* GetAuraScript() const
	{
		return new spell_gen_glacial_strike_AuraScript();
	}
};

enum MineSweeper
{
	ACHIEVEMENT_MINE_SWEEPER = 1428,
};

class spell_gen_landmine_knockback : public SpellScriptLoader
{
public:
    spell_gen_landmine_knockback() : SpellScriptLoader("spell_gen_landmine_knockback") { }

    class spell_gen_landmine_knockback_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_landmine_knockback_SpellScript);

        void HandleScript(SpellEffIndex /*effIndex*/)
        {
            if (Player* target = GetHitPlayer())
            {
                Aura const* aura = GetHitAura();
                if (!aura || aura->GetStackAmount() != 10)
                    return;

                AchievementEntry const* sweeper = sAchievementStore.LookupEntry(ACHIEVEMENT_MINE_SWEEPER);
                target->CompletedAchievement(sweeper);
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_gen_landmine_knockback_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_landmine_knockback_SpellScript();
    }
};

enum GMFreeze
{
    SPELL_GM_FREEZE = 9454,
};

class spell_gen_gm_freeze : public SpellScriptLoader
{
    public:
        spell_gen_gm_freeze() : SpellScriptLoader("spell_gen_gm_freeze") { }

        class spell_gen_gm_freeze_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_gm_freeze_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_GM_FREEZE))
                    return false;
                return true;
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* player = GetTarget()->ToPlayer())
                {
                    player->SetCurrentFaction(35);
                    player->CombatStop();

                    if (player->IsNonMeleeSpellCasted(true))
                        player->InterruptNonMeleeSpells(true);

                    player->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                    if ((player->GetCurrentClass() == CLASS_HUNTER) || (player->GetCurrentClass() == CLASS_WARLOCK))
                    {
                        if (Pet* pet = player->GetPet())
                        {
                            pet->SavePetToDB(PET_SAVE_AS_CURRENT);

                            if (pet->IsAlive())
                                player->RemovePet(pet, PET_SAVE_NOT_IN_SLOT);
                        }
                    }
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Player* player = GetTarget()->ToPlayer())
                {
                    player->setFactionForRace(player->GetCurrentRace());
                    player->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    player->SaveToDB();
                }
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_gen_gm_freeze_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_gen_gm_freeze_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_gm_freeze_AuraScript();
        }
};

enum VanityPetSpells
{
    SPELL_ROCKET_BOT_PASSIVE = 45266,
    SPELL_ROCKET_BOT_ATTACK  = 45269,
};

class spell_gen_vanity_pet_focus : public SpellScriptLoader
{
public:
    spell_gen_vanity_pet_focus(const char* name, uint32 _passiveId = 0) : SpellScriptLoader(name), passiveId(_passiveId) { }

    class spell_gen_vanity_pet_focus_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_gen_vanity_pet_focus_SpellScript);

    public:
        spell_gen_vanity_pet_focus_SpellScript(int32 _passiveId) : SpellScript(), passiveId(_passiveId) { }

        bool Validate(SpellInfo const* spellInfo)
        {
            if (!sSpellMgr->GetSpellInfo(spellInfo->Effects[EFFECT_0].BasePoints))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_ROCKET_BOT_ATTACK))
                return false;
            return true;
        }

		void FilterTargets(std::list<Unit*>& unitList)
        {
            if (passiveId == SPELL_ROCKET_BOT_PASSIVE)
                unitList.remove_if(Trinity::UnitAuraCheck(false, SPELL_ROCKET_BOT_PASSIVE));

            if (unitList.empty())
                return;

			Unit* target = Quantum::DataPackets::SelectRandomContainerElement(unitList);
            unitList.clear();
            unitList.push_back(target);
        }

        void HandleScript(SpellEffIndex /*effIndex*/)
        {
            // bit of a work around the orange clockwork robot doesn't fire 49058 correctly
            if (passiveId == SPELL_ROCKET_BOT_PASSIVE)
                GetCaster()->CastSpell(GetHitUnit(), SPELL_ROCKET_BOT_ATTACK, true);
            else
                GetCaster()->CastSpell(GetHitUnit(), uint32(GetEffectValue()), true);
        }

        void Register()
        {
            OnUnitTargetSelect += SpellUnitTargetFn(spell_gen_vanity_pet_focus_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            OnEffectHitTarget += SpellEffectFn(spell_gen_vanity_pet_focus_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }

    private:
        int32 passiveId;
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_gen_vanity_pet_focus_SpellScript(passiveId);
    }

	int32 passiveId;
};

enum SeaforiumBombs
{
	SPELL_BOMB_SMALL_CREDIT = 68366,
	SPELL_BOMB_HUGE_CREDIT  = 68367,
};

class spell_gen_small_ic_seaforium_blast : public SpellScriptLoader
{
    public:
        spell_gen_small_ic_seaforium_blast() : SpellScriptLoader("spell_gen_small_ic_seaforium_blast") {}

        class spell_gen_small_ic_seaforium_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_small_ic_seaforium_blast_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BOMB_SMALL_CREDIT))
                    return false;
                return true;
            }

            bool Load()
            {
                return GetOriginalCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            void AchievementCredit(SpellEffIndex /*effIndex*/)
            {
                if (Unit* originalCaster = GetOriginalCaster())
				{
                    if (GameObject* go = GetHitGObj())
					{
                        if (GetHitGObj()->GetGOInfo()->type == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
                            originalCaster->CastSpell(originalCaster, SPELL_BOMB_SMALL_CREDIT, true);
					}
				}
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_small_ic_seaforium_blast_SpellScript::AchievementCredit, EFFECT_1, SPELL_EFFECT_GAMEOBJECT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_small_ic_seaforium_blast_SpellScript();
        }
};

class spell_gen_huge_ic_seaforium_blast : public SpellScriptLoader
{
    public:
        spell_gen_huge_ic_seaforium_blast() : SpellScriptLoader("spell_gen_huge_ic_seaforium_blast") {}

        class spell_gen_huge_ic_seaforium_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_huge_ic_seaforium_blast_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BOMB_HUGE_CREDIT))
                    return false;
                return true;
            }

            bool Load()
            {
                return GetOriginalCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            void AchievementCredit(SpellEffIndex /*effIndex*/)
            {
                if (Unit* originalCaster = GetOriginalCaster())
				{
                    if (GameObject* go = GetHitGObj())
					{
                        if (GetHitGObj()->GetGOInfo()->type == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
                            originalCaster->CastSpell(originalCaster, SPELL_BOMB_HUGE_CREDIT, true);
					}
				}
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_huge_ic_seaforium_blast_SpellScript::AchievementCredit, EFFECT_1, SPELL_EFFECT_GAMEOBJECT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_huge_ic_seaforium_blast_SpellScript();
        }
};

enum PumkinTreat
{
	SPELL_HALLOWS_END_CANDY   = 24930,
	SPELL_HALLOWS_END_CANDY_1 = 24924,
	SPELL_HALLOWS_END_CANDY_2 = 24925,
	SPELL_HALLOWS_END_CANDY_3 = 24926,
	SPELL_HALLOWS_END_CANDY_4 = 24927,
};

class spell_gen_hallows_end_candy : public SpellScriptLoader
{
    public:
        spell_gen_hallows_end_candy() : SpellScriptLoader("spell_gen_hallows_end_candy") {}

        class spell_gen_hallows_end_candy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_hallows_end_candy_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_HALLOWS_END_CANDY))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
				Unit* caster = GetCaster();
                uint32 spellId = 0;

				if (Player* caster = GetHitPlayer())
                {
                    switch (urand(0, 3))
                    {
                        case 0:
							spellId = SPELL_HALLOWS_END_CANDY_1;
							break;
                        case 1:
							spellId = SPELL_HALLOWS_END_CANDY_2;
							break;
                        case 2:
							spellId = SPELL_HALLOWS_END_CANDY_3;
							break;
                        case 3:
							spellId = SPELL_HALLOWS_END_CANDY_4;
							break;
                    }
				}

				caster->CastSpell(caster, spellId, true, NULL);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_hallows_end_candy_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_hallows_end_candy_SpellScript();
        }
};

enum Fixate
{
    SPELL_FIXATE_TRIGGER = 40415,
};

class spell_gen_fixate : public SpellScriptLoader
{
public:
	spell_gen_fixate() : SpellScriptLoader("spell_gen_fixate") { }

	class spell_gen_fixate_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_gen_fixate_SpellScript);

		bool Validate(SpellInfo const* /*spellEntry*/)
		{
			return (bool) sSpellMgr->GetSpellInfo(SPELL_FIXATE_TRIGGER);
		}

		void HandleScriptEffect(SpellEffIndex /*effIndex*/)
		{
			GetCaster()->CastSpell(GetCaster(), SPELL_FIXATE_TRIGGER, true, 0, 0, GetHitUnit()->GetGUID());
		}

		void Register()
		{
			OnEffectHitTarget += SpellEffectFn(spell_gen_fixate_SpellScript::HandleScriptEffect, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_gen_fixate_SpellScript();
	}
};

enum SpellsPicnic
{
    SPELL_BASKET_CHECK              = 45119, // Holiday - Valentine - Romantic Picnic Near Basket Check
    SPELL_MEAL_PERIODIC             = 45103, // Holiday - Valentine - Romantic Picnic Meal Periodic - effect dummy
    SPELL_MEAL_EAT_VISUAL           = 45120, // Holiday - Valentine - Romantic Picnic Meal Eat Visual
    //SPELL_MEAL_PARTICLE             = 45114, // Holiday - Valentine - Romantic Picnic Meal Particle - unused
    SPELL_DRINK_VISUAL              = 45121, // Holiday - Valentine - Romantic Picnic Drink Visual
    SPELL_ROMANTIC_PICNIC_ACHIEV    = 45123, // Romantic Picnic periodic = 5000
};

class spell_gen_romantic_picnic : public SpellScriptLoader
{
    public:
        spell_gen_romantic_picnic() : SpellScriptLoader("spell_gen_romantic_picnic") { }

        class spell_gen_romantic_picnic_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_romantic_picnic_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                target->SetStandState(UNIT_STAND_STATE_SIT);
                target->CastSpell(target, SPELL_MEAL_PERIODIC, false);
            }

            void OnPeriodic(AuraEffect const* /*aurEff*/)
            {
                Unit* target = GetTarget();
                Unit* caster = GetCaster();

                if (target->getStandState() != UNIT_STAND_STATE_SIT)
                {
                    target->RemoveAura(SPELL_ROMANTIC_PICNIC_ACHIEV);
                    target->RemoveAura(GetAura());
                    return;
                }

                target->CastSpell(target, SPELL_BASKET_CHECK, false); // unknown use, it targets Romantic Basket
                target->CastSpell(target, RAND(SPELL_MEAL_EAT_VISUAL, SPELL_DRINK_VISUAL), false);

                bool foundSomeone = false;
                std::list<Player*> playerList;
                Trinity::AnyPlayerInObjectRangeCheck checker(target, INTERACTION_DISTANCE*2);
                Trinity::PlayerListSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(target, playerList, checker);
                target->VisitNearbyWorldObject(INTERACTION_DISTANCE*2, searcher);
                for (std::list<Player*>::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
                {
                    if ((*itr) != target && (*itr)->HasAura(GetId())) // && (*itr)->getStandState() == UNIT_STAND_STATE_SIT)
                    {
                        if (caster)
                        {
                            caster->CastSpell(*itr, SPELL_ROMANTIC_PICNIC_ACHIEV, true);
                            caster->CastSpell(target, SPELL_ROMANTIC_PICNIC_ACHIEV, true);
                        }
                        foundSomeone = true;
                        // break;
                    }
                }

                if (!foundSomeone && target->HasAura(SPELL_ROMANTIC_PICNIC_ACHIEV))
                    target->RemoveAura(SPELL_ROMANTIC_PICNIC_ACHIEV);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_romantic_picnic_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_romantic_picnic_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_romantic_picnic_AuraScript();
        }
};

class spell_gen_eject_passenger : public SpellScriptLoader
{
    public:
        spell_gen_eject_passenger() : SpellScriptLoader("spell_gen_eject_passenger") { }

        class spell_gen_eject_passenger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_eject_passenger_SpellScript);

            bool Validate(SpellInfo const* spellInfo)
            {
                if (spellInfo->Effects[EFFECT_0].CalcValue() < 1)
                    return false;
                return true;
            }

            void EjectPassenger(SpellEffIndex /*effIndex*/)
            {
                if (Vehicle* vehicle = GetHitUnit()->GetVehicleKit())
                {
                    if (Unit* passenger = vehicle->GetPassenger(GetEffectValue() - 1))
                        passenger->ExitVehicle();
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_eject_passenger_SpellScript::EjectPassenger, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_eject_passenger_SpellScript();
        }
};

enum RequiredMixologySpells
{
    SPELL_MIXOLOGY                      = 53042,
    // Flasks
    SPELL_FLASK_OF_THE_FROST_WYRM       = 53755,
    SPELL_FLASK_OF_STONEBLOOD           = 53758,
    SPELL_FLASK_OF_ENDLESS_RAGE         = 53760,
    SPELL_FLASK_OF_PURE_MOJO            = 54212,
    SPELL_LESSER_FLASK_OF_RESISTANCE    = 62380,
    SPELL_LESSER_FLASK_OF_TOUGHNESS     = 53752,
    SPELL_FLASK_OF_BLINDING_LIGHT       = 28521,
    SPELL_FLASK_OF_CHROMATIC_WONDER     = 42735,
    SPELL_FLASK_OF_FORTIFICATION        = 28518,
    SPELL_FLASK_OF_MIGHTY_RESTORATION   = 28519,
    SPELL_FLASK_OF_PURE_DEATH           = 28540,
    SPELL_FLASK_OF_RELENTLESS_ASSAULT   = 28520,
    SPELL_FLASK_OF_CHROMATIC_RESISTANCE = 17629,
    SPELL_FLASK_OF_DISTILLED_WISDOM     = 17627,
    SPELL_FLASK_OF_SUPREME_POWER        = 17628,
    SPELL_FLASK_OF_THE_TITANS           = 17626,
    // Elixirs
    SPELL_ELIXIR_OF_MIGHTY_AGILITY      = 28497,
    SPELL_ELIXIR_OF_ACCURACY            = 60340,
    SPELL_ELIXIR_OF_DEADLY_STRIKES      = 60341,
    SPELL_ELIXIR_OF_MIGHTY_DEFENSE      = 60343,
    SPELL_ELIXIR_OF_EXPERTISE           = 60344,
    SPELL_ELIXIR_OF_ARMOR_PIERCING      = 60345,
    SPELL_ELIXIR_OF_LIGHTNING_SPEED     = 60346,
    SPELL_ELIXIR_OF_MIGHTY_FORTITUDE    = 53751,
    SPELL_ELIXIR_OF_MIGHTY_MAGEBLOOD    = 53764,
    SPELL_ELIXIR_OF_MIGHTY_STRENGTH     = 53748,
    SPELL_ELIXIR_OF_MIGHTY_TOUGHTS      = 60347,
    SPELL_ELIXIR_OF_PROTECTION          = 53763,
    SPELL_ELIXIR_OF_SPIRIT              = 53747,
    SPELL_GURUS_ELIXIR                  = 53749,
    SPELL_SHADOWPOWER_ELIXIR            = 33721,
    SPELL_WRATH_ELIXIR                  = 53746,
    SPELL_ELIXIR_OF_EMPOWERMENT         = 28514,
    SPELL_ELIXIR_OF_MAJOR_MAGEBLOOD     = 28509,
    SPELL_ELIXIR_OF_MAJOR_SHADOW_POWER  = 28503,
    SPELL_ELIXIR_OF_MAJOR_DEFENSE       = 28502,
    SPELL_FEL_STRENGTH_ELIXIR           = 38954,
    SPELL_ELIXIR_OF_IRONSKIN            = 39628,
    SPELL_ELIXIR_OF_MAJOR_AGILITY       = 54494,
    SPELL_ELIXIR_OF_DRAENIC_WISDOM      = 39627,
    SPELL_ELIXIR_OF_MAJOR_FIREPOWER     = 28501,
    SPELL_ELIXIR_OF_MAJOR_FROST_POWER   = 28493,
    SPELL_EARTHEN_ELIXIR                = 39626,
    SPELL_ELIXIR_OF_MASTERY             = 33726,
    SPELL_ELIXIR_OF_HEALING_POWER       = 28491,
    SPELL_ELIXIR_OF_MAJOR_FORTITUDE     = 39625,
    SPELL_ELIXIR_OF_MAJOR_STRENGTH      = 28490,
    SPELL_ADEPTS_ELIXIR                 = 54452,
    SPELL_ONSLAUGHT_ELIXIR              = 33720,
    SPELL_MIGHTY_TROLLS_BLOOD_ELIXIR    = 24361,
    SPELL_GREATER_ARCANE_ELIXIR         = 17539,
    SPELL_ELIXIR_OF_THE_MONGOOSE        = 17538,
    SPELL_ELIXIR_OF_BRUTE_FORCE         = 17537,
    SPELL_ELIXIR_OF_SAGES               = 17535,
    SPELL_ELIXIR_OF_SUPERIOR_DEFENSE    = 11348,
    SPELL_ELIXIR_OF_DEMONSLAYING        = 11406,
    SPELL_ELIXIR_OF_GREATER_FIREPOWER   = 26276,
    SPELL_ELIXIR_OF_SHADOW_POWER        = 11474,
    SPELL_MAGEBLOOD_ELIXIR              = 24363,
    SPELL_ELIXIR_OF_GIANTS              = 11405,
    SPELL_ELIXIR_OF_GREATER_AGILITY     = 11334,
    SPELL_ARCANE_ELIXIR                 = 11390,
    SPELL_ELIXIR_OF_GREATER_INTELLECT   = 11396,
    SPELL_ELIXIR_OF_GREATER_DEFENSE     = 11349,
    SPELL_ELIXIR_OF_FROST_POWER         = 21920,
    SPELL_ELIXIR_OF_AGILITY             = 11328,
    SPELL_MAJOR_TROLLS_BLLOOD_ELIXIR    =  3223,
    SPELL_ELIXIR_OF_FORTITUDE           =  3593,
    SPELL_ELIXIR_OF_OGRES_STRENGTH      =  3164,
    SPELL_ELIXIR_OF_FIREPOWER           =  7844,
    SPELL_ELIXIR_OF_LESSER_AGILITY      =  3160,
    SPELL_ELIXIR_OF_DEFENSE             =  3220,
    SPELL_STRONG_TROLLS_BLOOD_ELIXIR    =  3222,
    SPELL_ELIXIR_OF_MINOR_ACCURACY      = 63729,
    SPELL_ELIXIR_OF_WISDOM              =  3166,
    SPELL_ELIXIR_OF_GIANTH_GROWTH       =  8212,
    SPELL_ELIXIR_OF_MINOR_AGILITY       =  2374,
    SPELL_ELIXIR_OF_MINOR_FORTITUDE     =  2378,
    SPELL_WEAK_TROLLS_BLOOD_ELIXIR      =  3219,
    SPELL_ELIXIR_OF_LIONS_STRENGTH      =  2367,
    SPELL_ELIXIR_OF_MINOR_DEFENSE       =   673
};

class spell_gen_mixology_bonus : public SpellScriptLoader
{
public:
    spell_gen_mixology_bonus() : SpellScriptLoader("spell_gen_mixology_bonus") { }

    class spell_gen_mixology_bonus_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_gen_mixology_bonus_AuraScript);

    public:
        spell_gen_mixology_bonus_AuraScript()
        {
            bonus = 0;
        }

    private:
        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_MIXOLOGY))
                return false;
            return true;
        }

        bool Load()
        {
            return GetCaster() && GetCaster()->GetTypeId() == TYPE_ID_PLAYER;
        }

        void SetBonusValueForEffect(SpellEffIndex effIndex, int32 value, AuraEffect const* aurEff)
        {
            if (aurEff->GetEffIndex() == uint32(effIndex))
                bonus = value;
        }

        void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
        {
            if (GetCaster()->HasAura(SPELL_MIXOLOGY) && GetCaster()->HasSpell(GetSpellInfo()->Effects[EFFECT_0].TriggerSpell))
            {
                switch (GetId())
                {
                    case SPELL_WEAK_TROLLS_BLOOD_ELIXIR:
                    case SPELL_MAGEBLOOD_ELIXIR:
                        bonus = amount;
                        break;
                    case SPELL_ELIXIR_OF_FROST_POWER:
                    case SPELL_LESSER_FLASK_OF_TOUGHNESS:
                    case SPELL_LESSER_FLASK_OF_RESISTANCE:
                        bonus = CalculatePctF(amount, 80);
                        break;
                    case SPELL_ELIXIR_OF_MINOR_DEFENSE:
                    case SPELL_ELIXIR_OF_LIONS_STRENGTH:
                    case SPELL_ELIXIR_OF_MINOR_AGILITY:
                    case SPELL_MAJOR_TROLLS_BLLOOD_ELIXIR:
                    case SPELL_ELIXIR_OF_SHADOW_POWER:
                    case SPELL_ELIXIR_OF_BRUTE_FORCE:
                    case SPELL_MIGHTY_TROLLS_BLOOD_ELIXIR:
                    case SPELL_ELIXIR_OF_GREATER_FIREPOWER:
                    case SPELL_ONSLAUGHT_ELIXIR:
                    case SPELL_EARTHEN_ELIXIR:
                    case SPELL_ELIXIR_OF_MAJOR_AGILITY:
                    case SPELL_FLASK_OF_THE_TITANS:
                    case SPELL_FLASK_OF_RELENTLESS_ASSAULT:
                    case SPELL_FLASK_OF_STONEBLOOD:
                    case SPELL_ELIXIR_OF_MINOR_ACCURACY:
                        bonus = CalculatePctF(amount, 50);
                        break;
                    case SPELL_ELIXIR_OF_PROTECTION:
                        bonus = 280;
                        break;
                    case SPELL_ELIXIR_OF_MAJOR_DEFENSE:
                        bonus = 200;
                        break;
                    case SPELL_ELIXIR_OF_GREATER_DEFENSE:
                    case SPELL_ELIXIR_OF_SUPERIOR_DEFENSE:
                        bonus = 140;
                        break;
                    case SPELL_ELIXIR_OF_FORTITUDE:
                        bonus = 100;
                        break;
                    case SPELL_FLASK_OF_ENDLESS_RAGE:
                        bonus = 82;
                        break;
                    case SPELL_ELIXIR_OF_DEFENSE:
                        bonus = 70;
                        break;
                    case SPELL_ELIXIR_OF_DEMONSLAYING:
                        bonus = 50;
                        break;
                    case SPELL_FLASK_OF_THE_FROST_WYRM:
                        bonus = 47;
                        break;
                    case SPELL_WRATH_ELIXIR:
                        bonus = 32;
                        break;
                    case SPELL_ELIXIR_OF_MAJOR_FROST_POWER:
                    case SPELL_ELIXIR_OF_MAJOR_FIREPOWER:
                    case SPELL_ELIXIR_OF_MAJOR_SHADOW_POWER:
                        bonus = 29;
                        break;
                    case SPELL_ELIXIR_OF_MIGHTY_TOUGHTS:
                        bonus = 27;
                        break;
                    case SPELL_FLASK_OF_SUPREME_POWER:
                    case SPELL_FLASK_OF_BLINDING_LIGHT:
                    case SPELL_FLASK_OF_PURE_DEATH:
                    case SPELL_SHADOWPOWER_ELIXIR:
                        bonus = 23;
                        break;
                    case SPELL_ELIXIR_OF_MIGHTY_AGILITY:
                    case SPELL_FLASK_OF_DISTILLED_WISDOM:
                    case SPELL_ELIXIR_OF_SPIRIT:
                    case SPELL_ELIXIR_OF_MIGHTY_STRENGTH:
                    case SPELL_FLASK_OF_PURE_MOJO:
                    case SPELL_ELIXIR_OF_ACCURACY:
                    case SPELL_ELIXIR_OF_DEADLY_STRIKES:
                    case SPELL_ELIXIR_OF_MIGHTY_DEFENSE:
                    case SPELL_ELIXIR_OF_EXPERTISE:
                    case SPELL_ELIXIR_OF_ARMOR_PIERCING:
                    case SPELL_ELIXIR_OF_LIGHTNING_SPEED:
                        bonus = 20;
                        break;
                    case SPELL_FLASK_OF_CHROMATIC_RESISTANCE:
                        bonus = 17;
                        break;
                    case SPELL_ELIXIR_OF_MINOR_FORTITUDE:
                    case SPELL_ELIXIR_OF_MAJOR_STRENGTH:
                        bonus = 15;
                        break;
                    case SPELL_FLASK_OF_MIGHTY_RESTORATION:
                        bonus = 13;
                        break;
                    case SPELL_ARCANE_ELIXIR:
                        bonus = 12;
                        break;
                    case SPELL_ELIXIR_OF_GREATER_AGILITY:
                    case SPELL_ELIXIR_OF_GIANTS:
                        bonus = 11;
                        break;
                    case SPELL_ELIXIR_OF_AGILITY:
                    case SPELL_ELIXIR_OF_GREATER_INTELLECT:
                    case SPELL_ELIXIR_OF_SAGES:
                    case SPELL_ELIXIR_OF_IRONSKIN:
                    case SPELL_ELIXIR_OF_MIGHTY_MAGEBLOOD:
                        bonus = 10;
                        break;
                    case SPELL_ELIXIR_OF_HEALING_POWER:
                        bonus = 9;
                        break;
                    case SPELL_ELIXIR_OF_DRAENIC_WISDOM:
                    case SPELL_GURUS_ELIXIR:
                        bonus = 8;
                        break;
                    case SPELL_ELIXIR_OF_FIREPOWER:
                    case SPELL_ELIXIR_OF_MAJOR_MAGEBLOOD:
                    case SPELL_ELIXIR_OF_MASTERY:
                        bonus = 6;
                        break;
                    case SPELL_ELIXIR_OF_LESSER_AGILITY:
                    case SPELL_ELIXIR_OF_OGRES_STRENGTH:
                    case SPELL_ELIXIR_OF_WISDOM:
                    case SPELL_ELIXIR_OF_THE_MONGOOSE:
                        bonus = 5;
                        break;
                    case SPELL_STRONG_TROLLS_BLOOD_ELIXIR:
                    case SPELL_FLASK_OF_CHROMATIC_WONDER:
                        bonus = 4;
                        break;
                    case SPELL_ELIXIR_OF_EMPOWERMENT:
                        bonus = -10;
                        break;
                    case SPELL_ADEPTS_ELIXIR:
                        SetBonusValueForEffect(EFFECT_0, 13, aurEff);
                        SetBonusValueForEffect(EFFECT_1, 13, aurEff);
                        SetBonusValueForEffect(EFFECT_2, 8, aurEff);
                        break;
                    case SPELL_ELIXIR_OF_MIGHTY_FORTITUDE:
                        SetBonusValueForEffect(EFFECT_0, 160, aurEff);
                        break;
                    case SPELL_ELIXIR_OF_MAJOR_FORTITUDE:
                        SetBonusValueForEffect(EFFECT_0, 116, aurEff);
                        SetBonusValueForEffect(EFFECT_1, 6, aurEff);
                        break;
                    case SPELL_FEL_STRENGTH_ELIXIR:
                        SetBonusValueForEffect(EFFECT_0, 40, aurEff);
                        SetBonusValueForEffect(EFFECT_1, 40, aurEff);
                        break;
                    case SPELL_FLASK_OF_FORTIFICATION:
                        SetBonusValueForEffect(EFFECT_0, 210, aurEff);
                        SetBonusValueForEffect(EFFECT_1, 5, aurEff);
                        break;
                    case SPELL_GREATER_ARCANE_ELIXIR:
                        SetBonusValueForEffect(EFFECT_0, 19, aurEff);
                        SetBonusValueForEffect(EFFECT_1, 19, aurEff);
                        SetBonusValueForEffect(EFFECT_2, 5, aurEff);
                        break;
                    case SPELL_ELIXIR_OF_GIANTH_GROWTH:
                        SetBonusValueForEffect(EFFECT_0, 5, aurEff);
                        break;
                    default:
                        sLog->OutErrorConsole("SpellId %u couldn't be processed in spell_gen_mixology_bonus", GetId());
                        break;
                }
                amount += bonus;
            }
        }

        int32 bonus;

        void Register()
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_gen_mixology_bonus_AuraScript::CalculateAmount, EFFECT_ALL, SPELL_AURA_ANY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_gen_mixology_bonus_AuraScript();
    }
};

enum WarlockSoulstone
{
	SPELL_TWISTING_NETHER       = 23700,
	SPELL_TWISTING_RESURRECTION = 25035,
};

class spell_gen_twisting_nether : public SpellScriptLoader
{
    public:
        spell_gen_twisting_nether() : SpellScriptLoader("spell_gen_twisting_nether") {}

        class spell_gen_twisting_nether_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_twisting_nether_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TWISTING_NETHER))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
				if (Unit* caster = GetCaster())
				{
					caster->CastSpell(caster, SPELL_TWISTING_RESURRECTION, true);
					caster->PlayDirectSound(7354);
				}
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_twisting_nether_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SELF_RESURRECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_twisting_nether_SpellScript();
        }
};

enum CleansingFlames
{
	// SpellHit
	SPELL_HIT_FLAMES_DARNASSUS       = 29126,
	SPELL_HIT_FLAMES_IRONFORGE       = 29135,
	SPELL_HIT_FLAMES_ORGRIMMAR       = 29136,
	SPELL_HIT_FLAMES_STORMWIND       = 29137,
	SPELL_HIT_FLAMES_THUNDER_BLUFF   = 29138,
	SPELL_HIT_FLAMES_UNDERCITY       = 29139,
	SPELL_HIT_FLAMES_EXODAR          = 46671,
	SPELL_HIT_FLAMES_SILVERMOON      = 46672,
	// Items
	SPELL_CREATE_FLAME_DARNASSUS     = 29099,
	SPELL_CREATE_FLAME_STORMWIND     = 29101,
	SPELL_CREATE_FLAME_IRONFORGE     = 29102,
	SPELL_CREATE_FLAME_ORGRIMMAR     = 29130,
	SPELL_CREATE_FLAME_THUNDER_BLUFF = 29132,
	SPELL_CREATE_FLAME_UNDERCITY     = 29133,
	SPELL_CREATE_FLAME_SILVERMOON    = 46689,
	SPELL_CREATE_FLAME_EXODAR        = 46690,
};

class spell_gen_cleansing_flames_orgrimmar : public SpellScriptLoader
{
    public:
        spell_gen_cleansing_flames_orgrimmar() : SpellScriptLoader("spell_gen_cleansing_flames_orgrimmar") { }

        class spell_gen_cleansing_flames_orgrimmar_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_cleansing_flames_orgrimmar_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_HIT_FLAMES_ORGRIMMAR))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
				if (Unit* caster = GetCaster())
					caster->CastSpell(caster, SPELL_CREATE_FLAME_ORGRIMMAR, true);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_cleansing_flames_orgrimmar_SpellScript::HandleScript, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_cleansing_flames_orgrimmar_SpellScript();
        }
};

class spell_gen_cleansing_flames_undercity : public SpellScriptLoader
{
    public:
        spell_gen_cleansing_flames_undercity() : SpellScriptLoader("spell_gen_cleansing_flames_undercity") { }

        class spell_gen_cleansing_flames_undercity_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_cleansing_flames_undercity_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_HIT_FLAMES_UNDERCITY))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
				if (Unit* caster = GetCaster())
					caster->CastSpell(caster, SPELL_CREATE_FLAME_UNDERCITY, true);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_cleansing_flames_undercity_SpellScript::HandleScript, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_cleansing_flames_undercity_SpellScript();
        }
};

class spell_gen_cleansing_flames_thunder_bluff : public SpellScriptLoader
{
    public:
        spell_gen_cleansing_flames_thunder_bluff() : SpellScriptLoader("spell_gen_cleansing_flames_thunder_bluff") { }

        class spell_gen_cleansing_flames_thunder_bluff_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_cleansing_flames_thunder_bluff_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_HIT_FLAMES_THUNDER_BLUFF))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
				if (Unit* caster = GetCaster())
					caster->CastSpell(caster, SPELL_CREATE_FLAME_THUNDER_BLUFF, true);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_cleansing_flames_thunder_bluff_SpellScript::HandleScript, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_cleansing_flames_thunder_bluff_SpellScript();
        }
};

class spell_gen_cleansing_flames_silvermoon : public SpellScriptLoader
{
    public:
		spell_gen_cleansing_flames_silvermoon() : SpellScriptLoader("spell_gen_cleansing_flames_silvermoon") { }

        class spell_gen_cleansing_flames_silvermoon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_cleansing_flames_silvermoon_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_HIT_FLAMES_SILVERMOON))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
				if (Unit* caster = GetCaster())
					caster->CastSpell(caster, SPELL_CREATE_FLAME_SILVERMOON, true);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_cleansing_flames_silvermoon_SpellScript::HandleScript, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_cleansing_flames_silvermoon_SpellScript();
        }
};

class spell_gen_cleansing_flames_stormwind : public SpellScriptLoader
{
    public:
        spell_gen_cleansing_flames_stormwind() : SpellScriptLoader("spell_gen_cleansing_flames_stormwind") { }

        class spell_gen_cleansing_flames_stormwind_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_cleansing_flames_stormwind_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_HIT_FLAMES_STORMWIND))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
				if (Unit* caster = GetCaster())
					caster->CastSpell(caster, SPELL_CREATE_FLAME_STORMWIND, true);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_cleansing_flames_stormwind_SpellScript::HandleScript, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_cleansing_flames_stormwind_SpellScript();
        }
};

class spell_gen_cleansing_flames_darnassus : public SpellScriptLoader
{
    public:
        spell_gen_cleansing_flames_darnassus() : SpellScriptLoader("spell_gen_cleansing_flames_darnassus") { }

        class spell_gen_cleansing_flames_darnassus_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_cleansing_flames_darnassus_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_HIT_FLAMES_DARNASSUS))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
				if (Unit* caster = GetCaster())
					caster->CastSpell(caster, SPELL_CREATE_FLAME_DARNASSUS, true);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_cleansing_flames_darnassus_SpellScript::HandleScript, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_cleansing_flames_darnassus_SpellScript();
        }
};

class spell_gen_cleansing_flames_ironforge : public SpellScriptLoader
{
    public:
        spell_gen_cleansing_flames_ironforge() : SpellScriptLoader("spell_gen_cleansing_flames_ironforge") { }

        class spell_gen_cleansing_flames_ironforge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_cleansing_flames_ironforge_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_HIT_FLAMES_IRONFORGE))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
				if (Unit* caster = GetCaster())
					caster->CastSpell(caster, SPELL_CREATE_FLAME_IRONFORGE, true);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_cleansing_flames_ironforge_SpellScript::HandleScript, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_cleansing_flames_ironforge_SpellScript();
        }
};

class spell_gen_cleansing_flames_exodar : public SpellScriptLoader
{
    public:
        spell_gen_cleansing_flames_exodar() : SpellScriptLoader("spell_gen_cleansing_flames_exodar") { }

        class spell_gen_cleansing_flames_exodar_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_cleansing_flames_exodar_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_HIT_FLAMES_EXODAR))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
				if (Unit* caster = GetCaster())
					caster->CastSpell(caster, SPELL_CREATE_FLAME_EXODAR, true);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_cleansing_flames_exodar_SpellScript::HandleScript, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_cleansing_flames_exodar_SpellScript();
        }
};

enum CrabDisguise
{
	SPELL_CRAB_DISGUISE = 46337,
	SPELL_APPLY_DIGUISE = 34804,
	SPELL_FADE_DIGUISE  = 47693,
};

class spell_gen_crab_disguise: public SpellScriptLoader
{
    public:
        spell_gen_crab_disguise() : SpellScriptLoader("spell_gen_crab_disguise") { }

        class spell_gen_crab_disguise_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_crab_disguise_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_CRAB_DISGUISE))
                    return false;
                return true;
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
				if (Unit* caster = GetCaster())
					caster->CastSpell(caster, SPELL_APPLY_DIGUISE, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
				if (Unit* caster = GetCaster())
					caster->CastSpell(caster, SPELL_FADE_DIGUISE, true);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectRemoveFn(spell_gen_crab_disguise_AuraScript::OnApply, EFFECT_0, SPELL_AURA_FORCE_REACTION, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_crab_disguise_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_FORCE_REACTION, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_crab_disguise_AuraScript();
        }
};

enum WintergraspVictory
{
	SPELL_WINTERGRASP_VICTORY    = 56902,
	SPELL_WINTERGRASP_WIN_VISUAL = 60044,
};

class spell_gen_wintergrasp_victory : public SpellScriptLoader
{
    public:
        spell_gen_wintergrasp_victory() : SpellScriptLoader("spell_gen_wintergrasp_victory") { }

        class spell_gen_wintergrasp_victory_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_wintergrasp_victory_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_WINTERGRASP_VICTORY))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
				if (Unit* caster = GetCaster())
					caster->CastSpell(caster, SPELL_WINTERGRASP_WIN_VISUAL, true);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_wintergrasp_victory_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_ADD_HONOR);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_wintergrasp_victory_SpellScript();
        }
};

class spell_gen_summon_crawler : public SpellScriptLoader
{
public:
	spell_gen_summon_crawler() : SpellScriptLoader("spell_gen_summon_crawler") { }

	class spell_gen_summon_crawler_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_gen_summon_crawler_SpellScript);

		void ChangeSummonPos(SpellEffIndex /*effIndex*/)
		{
			WorldLocation summonPos = *GetExplTargetDest();
			Position offset = { 5.0f, 0.0f, 0.0f, 0.0f };
			summonPos.RelocateOffset(offset);
			SetExplTargetDest(summonPos);
		}

		void Register()
		{
			OnEffectHit += SpellEffectFn(spell_gen_summon_crawler_SpellScript::ChangeSummonPos, EFFECT_0, SPELL_EFFECT_SUMMON_PET);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_gen_summon_crawler_SpellScript();
	}
};

class spell_gen_burning_depths_necrolyte_image : public SpellScriptLoader
{
    public:
        spell_gen_burning_depths_necrolyte_image() : SpellScriptLoader("spell_gen_burning_depths_necrolyte_image") { }

        class spell_gen_burning_depths_necrolyte_image_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_burning_depths_necrolyte_image_AuraScript);

            bool Validate(SpellInfo const* spellInfo)
            {
                if (!sSpellMgr->GetSpellInfo(uint32(spellInfo->Effects[EFFECT_2].CalcValue())))
                    return false;
                return true;
            }

            void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    caster->CastSpell(GetTarget(), uint32(GetSpellInfo()->Effects[EFFECT_2].CalcValue()));
            }

            void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->RemoveAurasDueToSpell(uint32(GetSpellInfo()->Effects[EFFECT_2].CalcValue()), GetCasterGUID());
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_burning_depths_necrolyte_image_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_gen_burning_depths_necrolyte_image_AuraScript::HandleRemove, EFFECT_0, SPELL_AURA_TRANSFORM, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_burning_depths_necrolyte_image_AuraScript();
        }
};

enum SoulFeast
{
	SPELL_SOUL_FEAST_VISUAL = 50091,
	SPELL_SOUL_FEAST        = 71203,

	QUEST_A_FEAST_OF_SOULS  = 24547,

	ITEM_ID_SHADOWS_EDGE    = 49888,

	SOUND_LICH_WHISPER_1    = 17235,
	SOUND_LICH_WHISPER_2    = 17236,
	SOUND_LICH_WHISPER_3    = 17237,
	SOUND_LICH_WHISPER_4    = 17238,
	SOUND_LICH_WHISPER_5    = 17239,
	SOUND_LICH_WHISPER_6    = 17240,
	SOUND_LICH_WHISPER_7    = 17241,
	SOUND_LICH_WHISPER_8    = 17242,
	SOUND_LICH_WHISPER_9    = 17243,
	SOUND_LICH_WHISPER_10   = 17244,
	SOUND_LICH_WHISPER_11   = 17245,
	SOUND_LICH_WHISPER_12   = 17246,
	SOUND_LICH_WHISPER_13   = 17247,
	SOUND_LICH_WHISPER_14   = 17248,
	SOUND_LICH_WHISPER_15   = 17249,
	SOUND_LICH_WHISPER_16   = 17250,
};

class spell_gen_soul_feast : public SpellScriptLoader
{
    public:
        spell_gen_soul_feast() : SpellScriptLoader("spell_gen_soul_feast") { }

        class spell_gen_soul_feast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_soul_feast_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SOUL_FEAST))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
				if (Player* player = GetCaster()->ToPlayer())
				{
					// Visual Spell Proc
					player->CastSpell(player, SPELL_SOUL_FEAST_VISUAL, true);

					// Sounds played only players in active quest and Hasitem Shadow`s Edge
					if (player->GetQuestStatus(QUEST_A_FEAST_OF_SOULS) == QUEST_STATUS_INCOMPLETE && player->HasItemCount(ITEM_ID_SHADOWS_EDGE, 1))
					{
						// Whisper Lich King to player 25% chance
						if (urand(0, 99) < 25)
							player->PlayDistanceSound(RAND(SOUND_LICH_WHISPER_1, SOUND_LICH_WHISPER_2, SOUND_LICH_WHISPER_3,
							SOUND_LICH_WHISPER_4, SOUND_LICH_WHISPER_5, SOUND_LICH_WHISPER_6, SOUND_LICH_WHISPER_7,
							SOUND_LICH_WHISPER_8, SOUND_LICH_WHISPER_9, SOUND_LICH_WHISPER_10, SOUND_LICH_WHISPER_11,
							SOUND_LICH_WHISPER_12, SOUND_LICH_WHISPER_13, SOUND_LICH_WHISPER_14, SOUND_LICH_WHISPER_15, SOUND_LICH_WHISPER_16), player);
					}
				}
            }

            void Register()
            {
				OnEffectHitTarget += SpellEffectFn(spell_gen_soul_feast_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_soul_feast_SpellScript();
        }
};

enum SeveredEssence
{
	SPELL_SEVERED_ESSENCE_1     = 71906,
	SPELL_SEVERED_ESSENCE_2     = 71942,

	NPC_SEVERED_ESSENCE_SHAMAN  = 38410,
	NPC_SEVERED_ESSENCE_DK      = 38510,
	NPC_SEVERED_ESSENCE_WARRIOR = 38512,
	NPC_SEVERED_ESSENCE_PALADIN = 38514,
	NPC_SEVERED_ESSENCE_HUNTER  = 38516,
	NPC_SEVERED_ESSENCE_DRUID   = 38518,
	NPC_SEVERED_ESSENCE_ROGUE   = 38520,
	NPC_SEVERED_ESSENCE_MAGE    = 38522,
	NPC_SEVERED_ESSENCE_WARLOCK = 38530,
	NPC_SEVERED_ESSENCE_PRIEST  = 38532,
};

class spell_gen_severed_essence : public SpellScriptLoader
{
    public:
        spell_gen_severed_essence() : SpellScriptLoader("spell_gen_severed_essence") { }

        class spell_gen_severed_essence_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_severed_essence_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SEVERED_ESSENCE_1))
                    return false;
				if (!sSpellMgr->GetSpellInfo(SPELL_SEVERED_ESSENCE_2))
                    return false;
                return true;
            }

			void SummonEssence()
            {
				if (Player* player = GetHitPlayer())
				{
					switch (player->GetCurrentClass())
					{
						case CLASS_SHAMAN:
							player->DealDamage(player, 4838, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_SHADOW, NULL, false);
							player->SummonCreature(NPC_SEVERED_ESSENCE_SHAMAN, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5*IN_MILLISECONDS);
							break;
						case CLASS_DEATH_KNIGHT:
							player->DealDamage(player, 4838, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_SHADOW, NULL, false);
							player->SummonCreature(NPC_SEVERED_ESSENCE_DK, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5*IN_MILLISECONDS);
							break;
						case CLASS_WARRIOR:
							player->DealDamage(player, 4838, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_SHADOW, NULL, false);
							player->SummonCreature(NPC_SEVERED_ESSENCE_WARRIOR, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5*IN_MILLISECONDS);
							break;
						case CLASS_PALADIN:
							player->DealDamage(player, 4838, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_SHADOW, NULL, false);
							player->SummonCreature(NPC_SEVERED_ESSENCE_PALADIN, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5*IN_MILLISECONDS);
							break;
						case CLASS_HUNTER:
							player->DealDamage(player, 4838, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_SHADOW, NULL, false);
							player->SummonCreature(NPC_SEVERED_ESSENCE_HUNTER, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5*IN_MILLISECONDS);
							break;
						case CLASS_DRUID:
							player->DealDamage(player, 4838, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_SHADOW, NULL, false);
							player->SummonCreature(NPC_SEVERED_ESSENCE_DRUID, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5*IN_MILLISECONDS);
							break;
						case CLASS_ROGUE:
							player->DealDamage(player, 4838, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_SHADOW, NULL, false);
							player->SummonCreature(NPC_SEVERED_ESSENCE_ROGUE, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5*IN_MILLISECONDS);
							break;
						case CLASS_MAGE:
							player->DealDamage(player, 4838, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_SHADOW, NULL, false);
							player->SummonCreature(NPC_SEVERED_ESSENCE_MAGE, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5*IN_MILLISECONDS);
							break;
						case CLASS_WARLOCK:
							player->DealDamage(player, 4838, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_SHADOW, NULL, false);
							player->SummonCreature(NPC_SEVERED_ESSENCE_WARLOCK, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5*IN_MILLISECONDS);
							break;
						case CLASS_PRIEST:
							player->DealDamage(player, 4838, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_SHADOW, NULL, false);
							player->SummonCreature(NPC_SEVERED_ESSENCE_PRIEST, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5*IN_MILLISECONDS);
							break;
						default:
							break;
					}
				}
            }

            void Register()
            {
				AfterHit += SpellHitFn(spell_gen_severed_essence_SpellScript::SummonEssence);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_severed_essence_SpellScript();
        }
};

void AddSC_generic_spell_scripts()
{
    new spell_gen_absorb0_hitlimit1();
    new spell_gen_aura_of_anger();
    new spell_gen_av_drekthar_presence();
    new spell_gen_burn_brutallus();
    new spell_gen_cannibalize();
    new spell_gen_leeching_swarm();
    new spell_gen_parachute();
    new spell_gen_pet_summoned();
    new spell_gen_remove_flight_auras();
    new spell_gen_trick();
    new spell_gen_trick_or_treat();
    new spell_gen_tricky_treat();
    new spell_creature_permanent_feign_death();
    new spell_pvp_trinket_wotf_shared_cd();
    new spell_gen_animal_blood();
    new spell_gen_divine_storm_cd_reset();
    new spell_gen_parachute_ic();
    new spell_gen_gunship_portal();
    new spell_gen_dungeon_credit();
    new spell_gen_profession_research();
    new spell_gen_clone();
    new spell_gen_clone_weapon();
	new spell_gen_clone_weapon_aura();
    new spell_gen_seaforium_blast();
    new spell_gen_turkey_marker();
    new spell_gen_lifeblood();
    new spell_gen_magic_rooster();
    new spell_gen_allow_cast_from_item_only();
    new spell_gen_ribbon_pole_dancer_check();
    new spell_gen_torch_target_picker();
	new spell_gen_torch_toss_land();
    new spell_gen_juggle_torch_catch();
    new spell_gen_throw_torch();
    new spell_gen_launch();
    new spell_gen_vehicle_scaling();
    new spell_gen_bone_gryphon_frost_breath();
    new spell_gen_oracle_wolvar_reputation();
	new spell_gen_luck_of_the_draw();
	new spell_gen_rocket_propelled_goblin_grenade();
	new spell_gen_damage_reduction_aura();
	new spell_gen_dalaran_disguise("spell_gen_sunreaver_disguise");
	new spell_gen_dalaran_disguise("spell_gen_silver_covenant_disguise");
	new spell_gen_break_shield("spell_gen_break_shield");
	new spell_gen_break_shield("spell_gen_tournament_counterattack");
	new spell_gen_mounted_charge();
	new spell_gen_defend();
	new spell_gen_tournament_duel();
	new spell_gen_summon_tournament_mount();
	new spell_gen_throw_shield();
	new spell_gen_on_tournament_mount();
	new spell_gen_tournament_pennant();
	new spell_gen_elune_candle();
	new spell_gen_toy_train_set();
	new spell_gen_dummy_trigger();
	new spell_gen_spirit_healer_res();
	new spell_gen_gadgetzan_transporter_backfire();
	new spell_gen_gnomish_transporter();
	new spell_gen_chaos_blast();
	new spell_gen_ds_flush_knockback();
	new spell_gen_wg_water();
	new spell_gen_count_pct_from_max_hp("spell_gen_default_count_pct_from_max_hp");
	new spell_gen_count_pct_from_max_hp("spell_gen_50pct_count_pct_from_max_hp", 50);
	new spell_gen_count_pct_from_max_hp("spell_gen_80pct_count_pct_from_max_hp", 80);
	new spell_gen_despawn_self();
	new spell_gen_bandage();
    new spell_gen_lifebloom("spell_hexlord_lifebloom", SPELL_HEXLORD_MALACRASS_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_lifebloom("spell_tur_ragepaw_lifebloom", SPELL_TUR_RAGEPAW_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_lifebloom("spell_cenarion_scout_lifebloom", SPELL_CENARION_SCOUT_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_lifebloom("spell_twisted_visage_lifebloom", SPELL_TWISTED_VISAGE_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_lifebloom("spell_faction_champion_dru_lifebloom", SPELL_FACTION_CHAMPIONS_DRU_LIFEBLOOM_FINAL_HEAL);
    new spell_gen_summon_elemental("spell_gen_summon_fire_elemental", SPELL_SUMMON_FIRE_ELEMENTAL);
    new spell_gen_summon_elemental("spell_gen_summon_earth_elemental", SPELL_SUMMON_EARTH_ELEMENTAL);
    new spell_gen_mount("spell_magic_broom", 0, SPELL_MAGIC_BROOM_60, SPELL_MAGIC_BROOM_100, SPELL_MAGIC_BROOM_150, SPELL_MAGIC_BROOM_280);
    new spell_gen_mount("spell_headless_horseman_mount", 0, SPELL_HEADLESS_HORSEMAN_MOUNT_60, SPELL_HEADLESS_HORSEMAN_MOUNT_100, SPELL_HEADLESS_HORSEMAN_MOUNT_150, SPELL_HEADLESS_HORSEMAN_MOUNT_280);
    new spell_gen_mount("spell_winged_steed_of_the_ebon_blade", 0, 0, 0, SPELL_WINGED_STEED_150, SPELL_WINGED_STEED_280);
    new spell_gen_mount("spell_big_love_rocket", SPELL_BIG_LOVE_ROCKET_0, SPELL_BIG_LOVE_ROCKET_60, SPELL_BIG_LOVE_ROCKET_100, SPELL_BIG_LOVE_ROCKET_150, SPELL_BIG_LOVE_ROCKET_310);
    new spell_gen_mount("spell_invincible", 0, SPELL_INVINCIBLE_60, SPELL_INVINCIBLE_100, SPELL_INVINCIBLE_150, SPELL_INVINCIBLE_310);
    new spell_gen_mount("spell_blazing_hippogryph", 0, 0, 0, SPELL_BLAZING_HIPPOGRYPH_150, SPELL_BLAZING_HIPPOGRYPH_280);
    new spell_gen_mount("spell_celestial_steed", 0, SPELL_CELESTIAL_STEED_60, SPELL_CELESTIAL_STEED_100, SPELL_CELESTIAL_STEED_150, SPELL_CELESTIAL_STEED_280, SPELL_CELESTIAL_STEED_310);
    new spell_gen_mount("spell_x53_touring_rocket", 0, 0, 0, SPELL_X53_TOURING_ROCKET_150, SPELL_X53_TOURING_ROCKET_280, SPELL_X53_TOURING_ROCKET_310);
	new spell_gen_turkey_tracker();
    new spell_gen_feast_on();
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_ap", SPELL_A_SERVING_OF_TURKEY, SPELL_WELL_FED_AP);
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_zm", SPELL_A_SERVING_OF_CRANBERRIES, SPELL_WELL_FED_ZM);
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_hit", SPELL_A_SERVING_OF_STUFFING, SPELL_WELL_FED_HIT);
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_haste", SPELL_A_SERVING_OF_SWEET_POTATOES, SPELL_WELL_FED_HASTE);
    new spell_gen_well_fed_pilgrims_bounty("spell_gen_well_fed_pilgrims_bounty_spirit", SPELL_A_SERVING_OF_PIE, SPELL_WELL_FED_SPIRIT);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_turkey", SPELL_ON_PLATE_TURKEY, SPELL_PASS_THE_TURKEY);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_cranberries", SPELL_ON_PLATE_CRANBERRIES, SPELL_PASS_THE_CRANBERRIES);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_stuffing", SPELL_ON_PLATE_STUFFING, SPELL_PASS_THE_STUFFING);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_sweet_potatoes", SPELL_ON_PLATE_SWEET_POTATOES, SPELL_PASS_THE_SWEET_POTATOES);
    new spell_gen_on_plate_pilgrims_bounty("spell_gen_on_plate_pilgrims_bounty_pie", SPELL_ON_PLATE_PIE, SPELL_PASS_THE_PIE);
    new spell_gen_bountiful_feast();
	new spell_pilgrims_bounty_buff_food("spell_gen_slow_roasted_turkey", SPELL_WELL_FED_AP_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_gen_cranberry_chutney", SPELL_WELL_FED_ZM_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_gen_spice_bread_stuffing", SPELL_WELL_FED_HIT_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_gen_pumpkin_pie", SPELL_WELL_FED_SPIRIT_TRIGGER);
    new spell_pilgrims_bounty_buff_food("spell_gen_candied_sweet_potato", SPELL_WELL_FED_HASTE_TRIGGER);
	new spell_gen_scarlet_raven_priest_image();
	new spell_gen_fumping_39238();
	new spell_gen_fumping_39246();
	new spell_gen_shadowmeld();
	new spell_gen_halloween_wand();
	new spell_gen_construct_barricade();
	new spell_gen_toss_stinky_bomb();
	new spell_gen_clean_stinky_bomb();
	new spell_gen_upper_deck_create_foam_sword();
	new spell_gen_bonked();
	new spell_gen_gift_of_naaru();
	new spell_gen_av_honorable_defender();
	new spell_gen_close_rift();
	new spell_gen_replenishment();
	new spell_gen_aura_service_uniform();
	new spell_gen_orc_disguise();
	new spell_gen_whisper_gulch_yogg_saron_whisper();
	new spell_gen_spectator_cheer_trigger();
	new spell_gen_vendor_bark_trigger();
	new spell_gen_glacial_strike();
	new spell_gen_landmine_knockback();
	new spell_gen_gm_freeze();
	new spell_gen_vanity_pet_focus("spell_gen_lil_kt_focus");
    new spell_gen_vanity_pet_focus("spell_gen_willy_focus");
    new spell_gen_vanity_pet_focus("spell_gen_scorchling_focus");
    new spell_gen_vanity_pet_focus("spell_gen_toxic_wasteling_focus");
    new spell_gen_vanity_pet_focus("spell_gen_rocket_bot_focus", SPELL_ROCKET_BOT_PASSIVE);
	new spell_gen_small_ic_seaforium_blast();
	new spell_gen_huge_ic_seaforium_blast();
	new spell_gen_hallows_end_candy();
	new spell_gen_fixate();
	new spell_gen_romantic_picnic();
	new spell_gen_eject_passenger();
	new spell_gen_mixology_bonus();
	new spell_gen_twisting_nether();
	new spell_gen_cleansing_flames_orgrimmar();
	new spell_gen_cleansing_flames_undercity();
	new spell_gen_cleansing_flames_thunder_bluff();
	new spell_gen_cleansing_flames_silvermoon();
	new spell_gen_cleansing_flames_stormwind();
	new spell_gen_cleansing_flames_darnassus();
	new spell_gen_cleansing_flames_ironforge();
	new spell_gen_cleansing_flames_exodar();
	new spell_gen_crab_disguise();
	new spell_gen_wintergrasp_victory();
	new spell_gen_summon_crawler();
	new spell_gen_burning_depths_necrolyte_image();
	new spell_gen_soul_feast();
	new spell_gen_severed_essence();
}