/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "Player.h"
#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "SpellScript.h"
#include "SpellAuras.h"
#include "Vehicle.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"

class spell_generic_quest_update_entry_SpellScript : public SpellScript
{
    PrepareSpellScript(spell_generic_quest_update_entry_SpellScript);

private:
    uint16 _spellEffect;
    uint8 _effIndex;
    uint32 _originalEntry;
    uint32 _newEntry;
    bool _shouldAttack;
    uint32 _despawnTime;

public:
    spell_generic_quest_update_entry_SpellScript(uint16 spellEffect, uint8 effIndex, uint32 originalEntry, uint32 newEntry, bool shouldAttack, uint32 despawnTime = 0) :
        SpellScript(), _spellEffect(spellEffect), _effIndex(effIndex), _originalEntry(originalEntry),
        _newEntry(newEntry), _shouldAttack(shouldAttack), _despawnTime(despawnTime) { }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Creature* creatureTarget = GetHitCreature())
            if (!creatureTarget->IsPet() && creatureTarget->GetEntry() == _originalEntry)
            {
                creatureTarget->UpdateEntry(_newEntry);
                if (_shouldAttack && creatureTarget->IsAIEnabled)
                    creatureTarget->AI()->AttackStart(GetCaster());

                if (_despawnTime)
                    creatureTarget->DespawnAfterAction(_despawnTime);
            }
    }

    void Register()
    {
        OnEffectHitTarget += SpellEffectFn(spell_generic_quest_update_entry_SpellScript::HandleDummy, _effIndex, _spellEffect);
    }
};

enum Quest55Data
{
    NPC_MORBENT          = 1200,
    NPC_WEAKENED_MORBENT = 24782,
};

class spell_q55_sacred_cleansing : public SpellScriptLoader
{
public:
    spell_q55_sacred_cleansing() : SpellScriptLoader("spell_q55_sacred_cleansing") { }

    SpellScript* GetSpellScript() const
    {
        return new spell_generic_quest_update_entry_SpellScript(SPELL_EFFECT_DUMMY, EFFECT_1, NPC_MORBENT, NPC_WEAKENED_MORBENT, true);
    }
};

// http://www.wowhead.com/quest=5206 Marauders of Darrowshire
// 17271 Test Fetid Skull

enum Quest5206Data
{
    SPELL_CREATE_RESONATING_SKULL = 17269,
    SPELL_CREATE_BONE_DUST = 17270
};

class spell_q5206_test_fetid_skull : public SpellScriptLoader
{
public:
    spell_q5206_test_fetid_skull() : SpellScriptLoader("spell_q5206_test_fetid_skull") { }

    class spell_q5206_test_fetid_skull_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q5206_test_fetid_skull_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_CREATE_RESONATING_SKULL))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_CREATE_BONE_DUST))
                return false;
            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();

            if (caster->GetTypeId() == TYPE_ID_PLAYER)
            {
                uint32 spellId = roll_chance_i(50) ? SPELL_CREATE_RESONATING_SKULL : SPELL_CREATE_BONE_DUST;
                caster->CastSpell(caster, spellId, true, NULL);
            }
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_q5206_test_fetid_skull_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q5206_test_fetid_skull_SpellScript();
    }
};

// http://www.wowhead.com/quest=6124 Curing the Sick (A)
// http://www.wowhead.com/quest=6129 Curing the Sick (H)
// 19512 Apply Salve

enum Quests6124_6129Data
{
    NPC_SICKLY_GAZELLE  = 12296,
    NPC_CURED_GAZELLE   = 12297,
    NPC_SICKLY_DEER     = 12298,
    NPC_CURED_DEER      = 12299,
    DESPAWN_TIME        = 30000
};

class spell_q6124_6129_apply_salve : public SpellScriptLoader
{
public:
    spell_q6124_6129_apply_salve() : SpellScriptLoader("spell_q6124_6129_apply_salve") { }

    class spell_q6124_6129_apply_salve_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q6124_6129_apply_salve_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (GetCastItem())
			{
                if (Player* caster = GetCaster()->ToPlayer())
				{
                    if (Creature* creatureTarget = GetHitCreature())
                    {
                        uint32 NewEntry = 0;

                        switch (caster->GetTeam())
                        {
                            case HORDE:
                                if (creatureTarget->GetEntry() == NPC_SICKLY_GAZELLE)
                                    NewEntry = NPC_CURED_GAZELLE;
                                break;
                            case ALLIANCE:
                                if (creatureTarget->GetEntry() == NPC_SICKLY_DEER)
                                    NewEntry = NPC_CURED_DEER;
                                break;
                        }
                        if (NewEntry)
                        {
							caster->CastedCreatureOrGO(creatureTarget->GetEntry(), 0, GetSpellInfo()->Id);
                            creatureTarget->UpdateEntry(NewEntry);
                            creatureTarget->DespawnAfterAction(DESPAWN_TIME);
                        }
                    }
				}
			}
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_q6124_6129_apply_salve_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q6124_6129_apply_salve_SpellScript();
    }
};

enum Quest10255Data
{
    NPC_HELBOAR   = 16880,
    NPC_DREADTUSK = 16992,
};

class spell_q10255_administer_antidote : public SpellScriptLoader
{
public:
    spell_q10255_administer_antidote() : SpellScriptLoader("spell_q10255_administer_antidote") { }

    SpellScript* GetSpellScript() const
    {
        return new spell_generic_quest_update_entry_SpellScript(SPELL_EFFECT_DUMMY, EFFECT_0, NPC_HELBOAR, NPC_DREADTUSK, true);
    }
};

enum Quest11396_11399Data
{
    SPELL_FORCE_SHIELD_ARCANE_PURPLE_X3 = 43874,
    SPELL_SCOURGING_CRYSTAL_CONTROLLER  = 43878
};

class spell_q11396_11399_force_shield_arcane_purple_x3 : public SpellScriptLoader
{
public:
    spell_q11396_11399_force_shield_arcane_purple_x3() : SpellScriptLoader("spell_q11396_11399_force_shield_arcane_purple_x3") { }

    class spell_q11396_11399_force_shield_arcane_purple_x3_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_q11396_11399_force_shield_arcane_purple_x3_AuraScript);

        void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            target->AddUnitState(UNIT_STATE_ROOT);
        }

        void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            GetTarget()->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_q11396_11399_force_shield_arcane_purple_x3_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            OnEffectRemove += AuraEffectRemoveFn(spell_q11396_11399_force_shield_arcane_purple_x3_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_q11396_11399_force_shield_arcane_purple_x3_AuraScript();
    }
};

class spell_q11396_11399_scourging_crystal_controller : public SpellScriptLoader
{
public:
    spell_q11396_11399_scourging_crystal_controller() : SpellScriptLoader("spell_q11396_11399_scourging_crystal_controller") { }

    class spell_q11396_11399_scourging_crystal_controller_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q11396_11399_scourging_crystal_controller_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_FORCE_SHIELD_ARCANE_PURPLE_X3))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_SCOURGING_CRYSTAL_CONTROLLER))
                return false;
            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Unit* target = GetExplTargetUnit())
			{
                if (target->GetTypeId() == TYPE_ID_UNIT && target->HasAura(SPELL_FORCE_SHIELD_ARCANE_PURPLE_X3))
				{
                    if (!target->HasAura(SPELL_SCOURGING_CRYSTAL_CONTROLLER))
                        GetCaster()->CastSpell(target, SPELL_SCOURGING_CRYSTAL_CONTROLLER, true, GetCastItem());
				}
			}
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_q11396_11399_scourging_crystal_controller_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q11396_11399_scourging_crystal_controller_SpellScript();
    };
};

class spell_q11396_11399_scourging_crystal_controller_dummy : public SpellScriptLoader
{
public:
    spell_q11396_11399_scourging_crystal_controller_dummy() : SpellScriptLoader("spell_q11396_11399_scourging_crystal_controller_dummy") { }

    class spell_q11396_11399_scourging_crystal_controller_dummy_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q11396_11399_scourging_crystal_controller_dummy_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_FORCE_SHIELD_ARCANE_PURPLE_X3))
                return false;
            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Unit* target = GetExplTargetUnit())
			{
                if (target->GetTypeId() == TYPE_ID_UNIT)
                    target->RemoveAurasDueToSpell(SPELL_FORCE_SHIELD_ARCANE_PURPLE_X3);
			}
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_q11396_11399_scourging_crystal_controller_dummy_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q11396_11399_scourging_crystal_controller_dummy_SpellScript();
    };
};

enum Quest11515Data
{
    NPC_FELBLOOD_INITIATE   = 24918,
    NPC_EMACIATED_FELBLOOD  = 24955
};

class spell_q11515_fel_siphon_dummy : public SpellScriptLoader
{
public:
    spell_q11515_fel_siphon_dummy() : SpellScriptLoader("spell_q11515_fel_siphon_dummy") { }

    SpellScript* GetSpellScript() const
    {
        return new spell_generic_quest_update_entry_SpellScript(SPELL_EFFECT_DUMMY, EFFECT_0, NPC_FELBLOOD_INITIATE, NPC_EMACIATED_FELBLOOD, true);
    }
};

enum Quest11587Data
{
    SPELL_SUMMON_ARCANE_PRISONER_MALE   = 45446,    // Summon Arcane Prisoner - Male
    SPELL_SUMMON_ARCANE_PRISONER_FEMALE = 45448,    // Summon Arcane Prisoner - Female
    SPELL_ARCANE_PRISONER_KILL_CREDIT   = 45456     // Arcane Prisoner Kill Credit
};

class spell_q11587_arcane_prisoner_rescue : public SpellScriptLoader
{
public:
    spell_q11587_arcane_prisoner_rescue() : SpellScriptLoader("spell_q11587_arcane_prisoner_rescue") { }

    class spell_q11587_arcane_prisoner_rescue_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q11587_arcane_prisoner_rescue_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_SUMMON_ARCANE_PRISONER_MALE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_SUMMON_ARCANE_PRISONER_FEMALE))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_ARCANE_PRISONER_KILL_CREDIT))
                return false;
            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();

            if (Unit* unitTarget = GetHitUnit())
            {
                uint32 spellId = SPELL_SUMMON_ARCANE_PRISONER_MALE;
                if (rand() % 2)
                    spellId = SPELL_SUMMON_ARCANE_PRISONER_FEMALE;
				caster->CastSpell(caster, spellId, true);
				unitTarget->CastSpell(caster, SPELL_ARCANE_PRISONER_KILL_CREDIT, true);
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_q11587_arcane_prisoner_rescue_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q11587_arcane_prisoner_rescue_SpellScript();
    }
};

enum Quest11730Data
{
    SPELL_SUMMON_SCAVENGEBOT_004A8  = 46063,
    SPELL_SUMMON_SENTRYBOT_57K      = 46068,
    SPELL_SUMMON_DEFENDOTANK_66D    = 46058,
    SPELL_SUMMON_SCAVENGEBOT_005B6  = 46066,
    SPELL_SUMMON_55D_COLLECTATRON   = 46034,
    SPELL_ROBOT_KILL_CREDIT         = 46027,
    NPC_SCAVENGEBOT_004A8           = 25752,
    NPC_SENTRYBOT_57K               = 25753,
    NPC_DEFENDOTANK_66D             = 25758,
    NPC_SCAVENGEBOT_005B6           = 25792,
    NPC_55D_COLLECTATRON            = 25793,
};

class spell_q11730_ultrasonic_screwdriver : public SpellScriptLoader
{
public:
    spell_q11730_ultrasonic_screwdriver() : SpellScriptLoader("spell_q11730_ultrasonic_screwdriver") { }

    class spell_q11730_ultrasonic_screwdriver_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q11730_ultrasonic_screwdriver_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_SUMMON_SCAVENGEBOT_004A8))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_SUMMON_SENTRYBOT_57K))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_SUMMON_DEFENDOTANK_66D))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_SUMMON_SCAVENGEBOT_005B6))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_SUMMON_55D_COLLECTATRON))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_ROBOT_KILL_CREDIT))
                return false;
            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Item* castItem = GetCastItem();
            if (!castItem)
                return;

            Unit* caster = GetCaster();
            if (caster->GetTypeId() != TYPE_ID_PLAYER)
                return;

            Creature* target = GetHitCreature();
            if (!target)
                return;

            uint32 spellId = 0;
            switch (target->GetEntry())
            {
                case NPC_SCAVENGEBOT_004A8:
					spellId = SPELL_SUMMON_SCAVENGEBOT_004A8;
					break;
                case NPC_SENTRYBOT_57K:
					spellId = SPELL_SUMMON_SENTRYBOT_57K;
					break;
                case NPC_DEFENDOTANK_66D:
					spellId = SPELL_SUMMON_DEFENDOTANK_66D;
					break;
                case NPC_SCAVENGEBOT_005B6:
					spellId = SPELL_SUMMON_SCAVENGEBOT_005B6;
					break;
                case NPC_55D_COLLECTATRON:
					spellId = SPELL_SUMMON_55D_COLLECTATRON;
					break;
                default:
                    return;
            }
            caster->CastSpell(caster, spellId, true, castItem);
            caster->CastSpell(caster, SPELL_ROBOT_KILL_CREDIT, true);
            target->DespawnAfterAction();
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_q11730_ultrasonic_screwdriver_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q11730_ultrasonic_screwdriver_SpellScript();
    }
};

// http://www.wowhead.com/quest=12459 That Which Creates Can Also Destroy
// 49587 Seeds of Nature's Wrath

enum Quest12459Data
{
    NPC_REANIMATED_FROSTWYRM       = 26841,
    NPC_WEAK_REANIMATED_FROSTWYRM  = 27821,

    NPC_TURGID                     = 27808,
    NPC_WEAK_TURGID                = 27809,

    NPC_DEATHGAZE                  = 27122,
    NPC_WEAK_DEATHGAZE             = 27807,
};

class spell_q12459_seeds_of_natures_wrath : public SpellScriptLoader
{
public:
    spell_q12459_seeds_of_natures_wrath() : SpellScriptLoader("spell_q12459_seeds_of_natures_wrath") { }

    class spell_q12459_seeds_of_natures_wrath_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q12459_seeds_of_natures_wrath_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Creature* creatureTarget = GetHitCreature())
            {
                uint32 NewEntry = 0;
                switch (creatureTarget->GetEntry())
                {
                    case NPC_REANIMATED_FROSTWYRM:
						NewEntry = NPC_WEAK_REANIMATED_FROSTWYRM;
						break;
                    case NPC_TURGID:
						NewEntry = NPC_WEAK_TURGID;
						break;
                    case NPC_DEATHGAZE:
						NewEntry = NPC_WEAK_DEATHGAZE;
						break;
                }
                if (NewEntry)
                    creatureTarget->UpdateEntry(NewEntry);
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_q12459_seeds_of_natures_wrath_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q12459_seeds_of_natures_wrath_SpellScript();
    }
};

// http://www.wowhead.com/quest=12634 Some Make Lemonade, Some Make Liquor
// 51840 Despawn Fruit Tosser

enum Quest12634Data
{
    SPELL_BANANAS_FALL_TO_GROUND    = 51836,
    SPELL_ORANGE_FALLS_TO_GROUND    = 51837,
    SPELL_PAPAYA_FALLS_TO_GROUND    = 51839,
    SPELL_SUMMON_ADVENTUROUS_DWARF  = 52070
};

class spell_q12634_despawn_fruit_tosser : public SpellScriptLoader
{
public:
    spell_q12634_despawn_fruit_tosser() : SpellScriptLoader("spell_q12634_despawn_fruit_tosser") { }

    class spell_q12634_despawn_fruit_tosser_SpellScript : public SpellScript
    {
    public:
        PrepareSpellScript(spell_q12634_despawn_fruit_tosser_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_BANANAS_FALL_TO_GROUND))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_ORANGE_FALLS_TO_GROUND))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_PAPAYA_FALLS_TO_GROUND))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_SUMMON_ADVENTUROUS_DWARF))
                return false;
            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            uint32 spellId = SPELL_BANANAS_FALL_TO_GROUND;

            switch (urand(0, 3))
            {
                case 1:
					spellId = SPELL_ORANGE_FALLS_TO_GROUND;
					break;
                case 2:
					spellId = SPELL_PAPAYA_FALLS_TO_GROUND;
					break;
            }
            // sometimes, if you're lucky, you get a dwarf
            if (roll_chance_i(5))
                spellId = SPELL_SUMMON_ADVENTUROUS_DWARF;
            GetCaster()->CastSpell(GetCaster(), spellId, true, NULL);
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_q12634_despawn_fruit_tosser_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q12634_despawn_fruit_tosser_SpellScript();
    }
};

// http://www.wowhead.com/quest=12683 Burning to Help
// 52308 Take Sputum Sample

class spell_q12683_take_sputum_sample : public SpellScriptLoader
{
public:
    spell_q12683_take_sputum_sample() : SpellScriptLoader("spell_q12683_take_sputum_sample") { }

    class spell_q12683_take_sputum_sample_SpellScript : public SpellScript
    {
    public:
        PrepareSpellScript(spell_q12683_take_sputum_sample_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            uint32 reqAuraId = GetSpellInfo()->Effects[EFFECT_1].CalcValue();

            Unit* caster = GetCaster();
            if (caster->HasAuraEffect(reqAuraId, 0))
            {
                uint32 spellId = GetSpellInfo()->Effects[EFFECT_0].CalcValue();
                caster->CastSpell(caster, spellId, true, NULL);
            }
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_q12683_take_sputum_sample_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q12683_take_sputum_sample_SpellScript();
    }
};

enum Quest12851Data
{
    NPC_FROSTGIANT          = 29351,
    NPC_FROSTWORG           = 29358,
    SPELL_FROSTGIANT_CREDIT = 58184,
    SPELL_FROSTWORG_CREDIT  = 58183,
    SPELL_IMMOLATION        = 54690,
    SPELL_ABLAZE            = 54683,
};

class spell_q12851_going_bearback : public SpellScriptLoader
{
public:
    spell_q12851_going_bearback() : SpellScriptLoader("spell_q12851_going_bearback") { }

    class spell_q12851_going_bearback_AuraScript : public AuraScript
    {
    public:
        PrepareAuraScript(spell_q12851_going_bearback_AuraScript);

        void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Unit* caster = GetCaster())
            {
                Unit* target = GetTarget();
				// Already in fire
				if (target->HasAura(SPELL_ABLAZE))
					return;

                if (Player* player = caster->GetCharmerOrOwnerPlayerOrPlayerItself())
                {
                    switch (target->GetEntry())
                    {
                        case NPC_FROSTWORG:
                            target->CastSpell(player, SPELL_FROSTWORG_CREDIT, true);
                            target->CastSpell(target, SPELL_IMMOLATION, true);
                            target->CastSpell(target, SPELL_ABLAZE, true);
                            break;
                        case NPC_FROSTGIANT:
                            target->CastSpell(player, SPELL_FROSTGIANT_CREDIT, true);
                            target->CastSpell(target, SPELL_IMMOLATION, true);
                            target->CastSpell(target, SPELL_ABLAZE, true);
                            break;
                    }
                }
            }
        }

        void Register()
        {
            AfterEffectApply += AuraEffectApplyFn(spell_q12851_going_bearback_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }

    };

    AuraScript* GetAuraScript() const
    {
        return new spell_q12851_going_bearback_AuraScript();
    }
};

// http://www.wowhead.com/quest=12937 Relief for the Fallen
// 55804 Healing Finished

enum Quest12937Data
{
    SPELL_TRIGGER_AID_OF_THE_EARTHEN  = 55809,
    NPC_FALLEN_EARTHEN_DEFENDER       = 30035,
};

class spell_q12937_relief_for_the_fallen : public SpellScriptLoader
{
public:
    spell_q12937_relief_for_the_fallen() : SpellScriptLoader("spell_q12937_relief_for_the_fallen") { }

    class spell_q12937_relief_for_the_fallen_SpellScript : public SpellScript
    {
    public:
        PrepareSpellScript(spell_q12937_relief_for_the_fallen_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_TRIGGER_AID_OF_THE_EARTHEN))
                return false;
            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            if (Player* player = caster->ToPlayer())
            {
                if (Creature* target = GetHitCreature())
                {
                    player->CastSpell(player, SPELL_TRIGGER_AID_OF_THE_EARTHEN, true, NULL);
                    player->KilledMonsterCredit(NPC_FALLEN_EARTHEN_DEFENDER, 0);
                    target->DespawnAfterAction();
                }
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_q12937_relief_for_the_fallen_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q12937_relief_for_the_fallen_SpellScript();
    }
};

enum Quest11307Data
{
    SPELL_PLAGUE_SPRAY  = 43381,
    QUEST_11307_CREDIT  = 24281,
};

class spell_q11307_plagued_vrykul_dummy : public SpellScriptLoader
{
    public:
        spell_q11307_plagued_vrykul_dummy() : SpellScriptLoader("spell_q11307_plagued_vrykul_dummy") { }

        class spell_q11307_plagued_vrykul_dummy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q11307_plagued_vrykul_dummy_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PLAGUE_SPRAY))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Aura* spray = GetCaster()->GetAura(SPELL_PLAGUE_SPRAY))
				{
                    if (Unit* unit = spray->GetCaster())
                    {
                        if (!unit->ToPlayer())
                            return;

                        unit->ToPlayer()->KilledMonsterCredit(QUEST_11307_CREDIT, 0);
                        GetCaster()->Kill(GetCaster());
                    }
				}
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_q11307_plagued_vrykul_dummy_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q11307_plagued_vrykul_dummy_SpellScript();
        };
};

enum Whoarethey
{
    SPELL_QUESTGIVER       = 48917,
    SPELL_MALE_DISGUISE    = 38080,
    SPELL_FEMALE_DISGUISE  = 38081,
    SPELL_GENERIC_DISGUISE = 32756
};

class spell_q10041_q10040_who_are_they : public SpellScriptLoader
{
    public:
        spell_q10041_q10040_who_are_they() : SpellScriptLoader("spell_q10041_q10040_who_are_they") { }

        class spell_q10041_q10040_who_are_they_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q10041_q10040_who_are_they_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_QUESTGIVER))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (!GetHitUnit() || !GetHitUnit()->ToPlayer())
                    return;

                GetHitUnit()->CastSpell(GetHitUnit(), GetHitUnit()->GetCurrentGender() == GENDER_MALE ? SPELL_MALE_DISGUISE : SPELL_FEMALE_DISGUISE, true);
                GetHitUnit()->CastSpell(GetHitUnit(), SPELL_GENERIC_DISGUISE, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_q10041_q10040_who_are_they_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q10041_q10040_who_are_they_SpellScript();
        }
};

enum SymbolofLife
{
    SPELL_PERMANENT_FEIGN_DEATH = 29266,
};

class spell_symbol_of_life_dummy : public SpellScriptLoader
{
public:
    spell_symbol_of_life_dummy() : SpellScriptLoader("spell_symbol_of_life_dummy") { }

    class spell_symbol_of_life_dummy_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_symbol_of_life_dummy_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Creature* target = GetHitCreature())
            {
                if (target->HasAura(SPELL_PERMANENT_FEIGN_DEATH))
                {
                    target->RemoveAurasDueToSpell(SPELL_PERMANENT_FEIGN_DEATH);
                    target->SetUInt32Value(UNIT_DYNAMIC_FLAGS, 0);
                    target->SetUInt32Value(UNIT_FIELD_FLAGS_2, 0);
                    target->SetHealth(target->GetMaxHealth() / 2);
                    target->SetPower(POWER_MANA, uint32(target->GetMaxPower(POWER_MANA) * 0.75f));
                }
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_symbol_of_life_dummy_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_symbol_of_life_dummy_SpellScript();
    };
};

enum Quest12659Data
{
    NPC_SCALPS_KC_BUNNY = 28622,
};

class spell_q12659_ahunaes_knife : public SpellScriptLoader
{
public:
    spell_q12659_ahunaes_knife() : SpellScriptLoader("spell_q12659_ahunaes_knife") { }

    class spell_q12659_ahunaes_knife_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q12659_ahunaes_knife_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Player* caster = GetCaster()->ToPlayer();
            if (!caster)
                return;

            if (Creature* target = GetExplTargetUnit()->ToCreature())
            {
                target->DespawnAfterAction();
                caster->KilledMonsterCredit(NPC_SCALPS_KC_BUNNY, 0);
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_q12659_ahunaes_knife_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q12659_ahunaes_knife_SpellScript();
    };
};

enum StoppingTheSpread
{
    NPC_VILLAGER_KILL_CREDIT = 18240,
    SPELL_FLAMES             = 39199,
};

class spell_q9874_liquid_fire : public SpellScriptLoader
{
    public:
        spell_q9874_liquid_fire() : SpellScriptLoader("spell_q9874_liquid_fire"){}

        class spell_q9874_liquid_fire_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q9874_liquid_fire_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Player* caster = GetCaster()->ToPlayer();
                if (Creature* target = GetHitCreature())
				{
                    if (target && !target->HasAura(SPELL_FLAMES))
                    {
                        caster->KilledMonsterCredit(NPC_VILLAGER_KILL_CREDIT, 0);
                        target->CastSpell(target, SPELL_FLAMES, true);
                        target->DespawnAfterAction(60000);
                    }
				}
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_q9874_liquid_fire_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q9874_liquid_fire_SpellScript();
        };
};

enum SalvagingLifesStength
{
    NPC_SHARD_KILL_CREDIT = 29303,
};

class spell_q12805_lifeblood_dummy : public SpellScriptLoader
{
    public:
        spell_q12805_lifeblood_dummy() : SpellScriptLoader("spell_q12805_lifeblood_dummy") { }

        class spell_q12805_lifeblood_dummy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q12805_lifeblood_dummy_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Player* caster = GetCaster()->ToPlayer();
                Creature* target = GetHitUnit()->ToCreature();
                if (!caster || !target)
                    return;

                caster->KilledMonsterCredit(NPC_SHARD_KILL_CREDIT, 0);
                target->CastSpell(target, uint32(GetEffectValue()), true);
                target->DespawnAfterAction(2000);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_q12805_lifeblood_dummy_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q12805_lifeblood_dummy_SpellScript();
        };
};

/*
 http://www.wowhead.com/quest=13283 King of the Mountain
 http://www.wowhead.com/quest=13280 King of the Mountain
 59643 Plant Horde Battle Standard
 4338 Plant Alliance Battle Standard
 */

enum BattleStandard
{
    NPC_KING_OF_THE_MOUNTAINT_KC = 31766,
};

class spell_q13280_13283_plant_battle_standard: public SpellScriptLoader
{
public:
    spell_q13280_13283_plant_battle_standard() : SpellScriptLoader("spell_q13280_13283_plant_battle_standard") { }

    class spell_q13280_13283_plant_battle_standard_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q13280_13283_plant_battle_standard_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            if (caster->IsVehicle())
			{
                if (Unit* player = caster->GetVehicleKit()->GetPassenger(0))
                     player->ToPlayer()->KilledMonsterCredit(NPC_KING_OF_THE_MOUNTAINT_KC, 0);
			}
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_q13280_13283_plant_battle_standard_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q13280_13283_plant_battle_standard_SpellScript();
    }
};

enum ChumTheWaterSummons
{
    SUMMON_ANGRY_KVALDIR        = 66737,
    SUMMON_NORTH_SEA_MAKO       = 66738,
    SUMMON_NORTH_SEA_THRESHER   = 66739,
    SUMMON_NORTH_SEA_BLUE_SHARK = 66740,
};

class spell_q14112_14145_chum_the_water: public SpellScriptLoader
{
public:
    spell_q14112_14145_chum_the_water() : SpellScriptLoader("spell_q14112_14145_chum_the_water") { }

    class spell_q14112_14145_chum_the_water_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q14112_14145_chum_the_water_SpellScript);
    
        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SUMMON_ANGRY_KVALDIR))
                return false;
            if (!sSpellMgr->GetSpellInfo(SUMMON_NORTH_SEA_MAKO))
                return false;
            if (!sSpellMgr->GetSpellInfo(SUMMON_NORTH_SEA_THRESHER))
                return false;
            if (!sSpellMgr->GetSpellInfo(SUMMON_NORTH_SEA_BLUE_SHARK))
                return false;
            return true;
        }

        void HandleScriptEffect(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            caster->CastSpell(caster, RAND(SUMMON_ANGRY_KVALDIR, SUMMON_NORTH_SEA_MAKO, SUMMON_NORTH_SEA_THRESHER, SUMMON_NORTH_SEA_BLUE_SHARK));
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_q14112_14145_chum_the_water_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q14112_14145_chum_the_water_SpellScript();
    }
};

/*##########
# Quest 9361
###########*/

enum PurifyMeatSpells
{
    SPELL_PURIFY_HELBOAR_MEAT = 29200,
    SPELL_SUMMON_PURIFIED_HELBOAR_MEAT = 29277,
    SPELL_SUMMON_TOXIC_HELBOAR_MEAT = 29278
};

class spell_q9361_purify_helboar_meat : public SpellScriptLoader
{
public:
    spell_q9361_purify_helboar_meat() : SpellScriptLoader("spell_q9361_purify_helboar_meat") {}

    class spell_q9361_purify_helboar_meat_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q9361_purify_helboar_meat_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PURIFY_HELBOAR_MEAT))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_SUMMON_PURIFIED_HELBOAR_MEAT))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_SUMMON_TOXIC_HELBOAR_MEAT))
                return false;
            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Unit* caster = GetCaster())
			{
                if (Player* player = caster->ToPlayer())
				{
                    switch (urand(0, 1))
                    {
                        case 0:
							player->CastSpell(player, SPELL_SUMMON_PURIFIED_HELBOAR_MEAT, false);
							break;
                        case 1:
							player->CastSpell(player, SPELL_SUMMON_TOXIC_HELBOAR_MEAT, false);
							break;
                    }
				}
			}
        }

        void Register()
        {
            OnEffectHit += SpellEffectFn(spell_q9361_purify_helboar_meat_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q9361_purify_helboar_meat_SpellScript();
    }
};

enum FlightOfTheWintergardeDefender
{
    NPC_HELPLESS_VILLAGER_A  = 27315,
    NPC_HELPLESS_VILLAGER_B  = 27336,
    SPELL_RIDE_VEHICLE       = 43671,
    GO_TEMP_GRYPHON_STATION  = 300199,
};

class spell_q12237_rescue_villager: public SpellScriptLoader
{
    public:
        spell_q12237_rescue_villager() : SpellScriptLoader("spell_q12237_rescue_villager") { }

        class spell_q12237_rescue_villager_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q12237_rescue_villager_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_RIDE_VEHICLE))
                    return false;

                return true;
            }

            SpellCastResult CheckCast()
            {
                Player* master = GetCaster()->GetCharmerOrOwnerPlayerOrPlayerItself();

                if (!master)
                    return SPELL_FAILED_DONT_REPORT;

                SpellCustomErrors extension = SPELL_CUSTOM_ERROR_NONE;
                SpellCastResult result = SPELL_CAST_OK;

                if (!GetCaster()->FindCreatureWithDistance(NPC_HELPLESS_VILLAGER_A, 5.0f) && !GetCaster()->FindCreatureWithDistance(NPC_HELPLESS_VILLAGER_B, 5.0f))
                {
                    extension = SPELL_CUSTOM_ERROR_MUST_BE_NEAR_HELPLESS_VILLAGER;
                    result = SPELL_FAILED_CUSTOM_ERROR;
                }

                if (GetCaster()->FindGameobjectWithDistance(GO_TEMP_GRYPHON_STATION, 15.0f))
                {
                    extension = SPELL_CUSTOM_ERROR_NEED_HELPLESS_VILLAGER;
                    result = SPELL_FAILED_CUSTOM_ERROR;
                }

                if (GetCaster()->HasAura(SPELL_RIDE_VEHICLE))
                    result = SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                if (result != SPELL_CAST_OK)
                {
                    Spell::SendCastResult(master, GetSpellInfo(), 0, result, extension);
                    return result;
                }

                return SPELL_CAST_OK;
            }

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();

                if (target && !caster->HasAura(SPELL_RIDE_VEHICLE))
                    target->CastSpell(caster, SPELL_RIDE_VEHICLE, true);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_q12237_rescue_villager_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_q12237_rescue_villager_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q12237_rescue_villager_SpellScript();
        }
};

class spell_q12237_drop_off_villager: public SpellScriptLoader
{
    public:
        spell_q12237_drop_off_villager() : SpellScriptLoader("spell_q12237_drop_off_villager") { }

        class spell_q12237_drop_off_villager_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q12237_drop_off_villager_SpellScript);

            SpellCastResult CheckCast()
            {
                Player* master = GetCaster()->GetCharmerOrOwnerPlayerOrPlayerItself();

                if (!master)
                    return SPELL_FAILED_DONT_REPORT;

                SpellCustomErrors extension = SPELL_CUSTOM_ERROR_NONE;
                SpellCastResult result = SPELL_CAST_OK;

                if (!GetCaster()->HasAura(SPELL_RIDE_VEHICLE))
                {
                    extension = SPELL_CUSTOM_ERROR_NO_PASSENGER;
                    result = SPELL_FAILED_CUSTOM_ERROR;
                }
                else if (!GetCaster()->FindGameobjectWithDistance(GO_TEMP_GRYPHON_STATION, 10.0f))
                    result = SPELL_FAILED_REQUIRES_SPELL_FOCUS;

                if (result != SPELL_CAST_OK)
                {
                    Spell::SendCastResult(master, GetSpellInfo(), 0, result, extension);
                    return result;
                }
                return SPELL_CAST_OK;
            }

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();

                if (Vehicle* gryphon = GetCaster()->GetVehicleKit())
				{
                    if (Unit* villager = gryphon->GetPassenger(1))
                    {
                        villager->ExitVehicle();

                        if (villager->ToCreature())
                            villager->ToCreature()->DespawnAfterAction(7*IN_MILLISECONDS);
                    }
				}
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_q12237_drop_off_villager_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_q12237_drop_off_villager_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q12237_drop_off_villager_SpellScript();
        }
};

// http://old01.wowhead.com/quest=9452 - Red Snapper - Very Tasty!

enum RedSnapperVeryTasty
{
    SPELL_CAST_NET         = 29866,
    ITEM_RED_SNAPPER       = 23614,
    SPELL_NEW_SUMMON_TEST  = 49214,
};

class spell_q9452_cast_net: public SpellScriptLoader
{
    public:
        spell_q9452_cast_net() : SpellScriptLoader("spell_q9452_cast_net") { }

        class spell_q9452_cast_net_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q9452_cast_net_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPE_ID_PLAYER;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Player* caster = GetCaster()->ToPlayer();
                if (roll_chance_i(66))
                    caster->AddItem(ITEM_RED_SNAPPER, 1);
                else
                    caster->CastSpell(caster, SPELL_NEW_SUMMON_TEST, true);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_q9452_cast_net_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q9452_cast_net_SpellScript();
        }
};

enum LeaveNothingToChance
{
    NPC_UPPER_MINE_SHAFT           = 27436,
    NPC_LOWER_MINE_SHAFT           = 27437,

    SPELL_UPPER_MINE_SHAFT_CREDIT  = 48744,
    SPELL_LOWER_MINE_SHAFT_CREDIT  = 48745,
};

class spell_q12277_wintergarde_mine_explosion : public SpellScriptLoader
{
    public:
        spell_q12277_wintergarde_mine_explosion() : SpellScriptLoader("spell_q12277_wintergarde_mine_explosion") { }

        class spell_q12277_wintergarde_mine_explosion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q12277_wintergarde_mine_explosion_SpellScript);
 
            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Creature* unitTarget = GetHitCreature())
                {
                    if (Unit* caster = GetCaster())
                    {
                        if (caster->GetTypeId() == TYPE_ID_UNIT)
                        {
                            if (Unit* owner = caster->GetOwner())
                            {
                                switch (unitTarget->GetEntry())
                                {
                                    case NPC_UPPER_MINE_SHAFT:
                                        caster->CastSpell(owner, SPELL_UPPER_MINE_SHAFT_CREDIT, true);
                                        break;
                                    case NPC_LOWER_MINE_SHAFT:
                                        caster->CastSpell(owner, SPELL_LOWER_MINE_SHAFT_CREDIT, true);
                                        break;
                                    default:
                                        break;
                                }
                            }
                        }
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_q12277_wintergarde_mine_explosion_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q12277_wintergarde_mine_explosion_SpellScript();
        }
};

#define SAY_1 "Sons of Hodir! I humbly present to you..."
#define SAY_2 "The Helm of Hodir!"
#define NPC_KILLCREDIT 30210

class spell_q12987_read_pronouncement : public SpellScriptLoader
{
public:
    spell_q12987_read_pronouncement() : SpellScriptLoader("spell_q12987_read_pronouncement") { }

    class spell_q12987_read_pronouncement_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_q12987_read_pronouncement_AuraScript);

        void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Player* target = GetTarget()->ToPlayer())
            {
                target->MonsterWhisper(SAY_1, target->GetGUID(), true);
                target->KilledMonsterCredit(NPC_KILLCREDIT, 0);
                target->MonsterWhisper(SAY_2, target->GetGUID(), true);
            }
        }

        void Register()
        {
            AfterEffectApply += AuraEffectApplyFn(spell_q12987_read_pronouncement_AuraScript::OnApply, EFFECT_0, SPELL_AURA_NONE, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_q12987_read_pronouncement_AuraScript();
    }
};

enum FocusOnTheBeach
{
    SPELL_BUNNY_CREDIT_BEAM = 47390,
};

class spell_q12066_bunny_kill_credit : public SpellScriptLoader
{
public:
    spell_q12066_bunny_kill_credit() : SpellScriptLoader("spell_q12066_bunny_kill_credit") { }

    class spell_q12066_bunny_kill_credit_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q12066_bunny_kill_credit_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Creature* target = GetHitCreature())
			{
                target->CastSpell(GetCaster(), SPELL_BUNNY_CREDIT_BEAM, false);
			}
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_q12066_bunny_kill_credit_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q12066_bunny_kill_credit_SpellScript();
    }
};

enum ACleansingSong
{
    SPELL_SUMMON_SPIRIT_ATAH     = 52954,
    SPELL_SUMMON_SPIRIT_HAKHALAN = 52958,
    SPELL_SUMMON_SPIRIT_KOOSU    = 52959,
    AREA_BITTERTIDE_LAKE         = 4385,
    AREA_RIVERS_HEART            = 4290,
    AREA_WINTERGRASP_RIVER       = 4388,
};

class spell_q12735_song_of_cleansing : public SpellScriptLoader
{
    public:
        spell_q12735_song_of_cleansing() : SpellScriptLoader("spell_q12735_song_of_cleansing") { }

        class spell_q12735_song_of_cleansing_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q12735_song_of_cleansing_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();

                switch (caster->GetAreaId())
                {
                    case AREA_BITTERTIDE_LAKE:
                        caster->CastSpell(caster, SPELL_SUMMON_SPIRIT_ATAH);
                        break;
                    case AREA_RIVERS_HEART:
                        caster->CastSpell(caster, SPELL_SUMMON_SPIRIT_HAKHALAN);
                        break;
                    case AREA_WINTERGRASP_RIVER:
                        caster->CastSpell(caster, SPELL_SUMMON_SPIRIT_KOOSU);
                        break;
                    default:
                        break;
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_q12735_song_of_cleansing_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q12735_song_of_cleansing_SpellScript();
        }
};

enum DefendingWyrmrestTemple
{
    SPELL_SUMMON_WYRMREST_DEFENDER = 49207,
};

class spell_q12372_cast_from_gossip_trigger : public SpellScriptLoader
{
public:
	spell_q12372_cast_from_gossip_trigger() : SpellScriptLoader("spell_q12372_cast_from_gossip_trigger") { }

	class spell_q12372_cast_from_gossip_trigger_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_q12372_cast_from_gossip_trigger_SpellScript);

		void HandleScript(SpellEffIndex /*effIndex*/)
		{
			GetCaster()->CastSpell(GetCaster(), SPELL_SUMMON_WYRMREST_DEFENDER, true);
		}

		void Register()
		{
			OnEffectHitTarget += SpellEffectFn(spell_q12372_cast_from_gossip_trigger_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_q12372_cast_from_gossip_trigger_SpellScript();
	}
};

enum Quest12372Data
{
    NPC_WYRMREST_TEMPLE_CREDIT      = 27698,
	WHISPER_ON_HIT_BY_FORCE_WHISPER = 1,
};

class spell_q12372_destabilize_azure_dragonshrine_dummy : public SpellScriptLoader
{
public:
	spell_q12372_destabilize_azure_dragonshrine_dummy() : SpellScriptLoader("spell_q12372_destabilize_azure_dragonshrine_dummy") { }

	class spell_q12372_destabilize_azure_dragonshrine_dummy_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_q12372_destabilize_azure_dragonshrine_dummy_SpellScript);

		void HandleDummy(SpellEffIndex /*effIndex*/)
		{
			if (GetHitCreature())
			{
				if (Unit* caster = GetOriginalCaster())
				{
					if (Vehicle* vehicle = caster->GetVehicleKit())
					{
						if (Unit* passenger = vehicle->GetPassenger(0))
						{
							if (Player* player = passenger->ToPlayer())
							{
								player->KilledMonsterCredit(NPC_WYRMREST_TEMPLE_CREDIT, 0);
							}
						}
					}
				}
			}
		} 

		void Register()
		{
			OnEffectHitTarget += SpellEffectFn(spell_q12372_destabilize_azure_dragonshrine_dummy_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_q12372_destabilize_azure_dragonshrine_dummy_SpellScript();
	}
};

// ID - 50287 Azure Dragon: On Death Force Cast Wyrmrest Defender to Whisper to Controller - Random (casted from Azure Dragons and Azure Drakes on death)
class spell_q12372_azure_on_death_force_whisper : public SpellScriptLoader
{
    public:
        spell_q12372_azure_on_death_force_whisper() : SpellScriptLoader("spell_q12372_azure_on_death_force_whisper") { }

        class spell_q12372_azure_on_death_force_whisper_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q12372_azure_on_death_force_whisper_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Creature* defender = GetHitCreature())
                    defender->AI()->Talk(WHISPER_ON_HIT_BY_FORCE_WHISPER, defender->GetCharmerOrOwnerGUID());
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_q12372_azure_on_death_force_whisper_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q12372_azure_on_death_force_whisper_SpellScript();
        }
};

enum Quest11010_11102_11023Data
{
    SPELL_FLAK_CANNON_TRIGGER = 40110,
    SPELL_CHOOSE_LOC          = 40056,
    SPELL_AGGRO_CHECK         = 40112,
    NPC_FEL_CANNON2           = 23082,
};

class spell_q11010_q11102_q11023_aggro_check_aura : public SpellScriptLoader
{
    public:
        spell_q11010_q11102_q11023_aggro_check_aura() : SpellScriptLoader("spell_q11010_q11102_q11023_aggro_check_aura") { }

        class spell_q11010_q11102_q11023_aggro_check_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_q11010_q11102_q11023_aggro_check_aura_AuraScript);

            void HandleTriggerSpell(AuraEffect const* /*aurEff*/)
            {
                if (Unit* target = GetTarget())
				{
                    // On trigger proccing
                    target->CastSpell(target, SPELL_AGGRO_CHECK);
				}
            }

            void Register()
            {
               OnEffectPeriodic += AuraEffectPeriodicFn(spell_q11010_q11102_q11023_aggro_check_aura_AuraScript::HandleTriggerSpell, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_q11010_q11102_q11023_aggro_check_aura_AuraScript();
        }
};

class spell_q11010_q11102_q11023_aggro_check : public SpellScriptLoader
{
    public:
        spell_q11010_q11102_q11023_aggro_check() : SpellScriptLoader("spell_q11010_q11102_q11023_aggro_check") { }

        class spell_q11010_q11102_q11023_aggro_check_SpellScript : public SpellScript
		{
			PrepareSpellScript(spell_q11010_q11102_q11023_aggro_check_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Player* playerTarget = GetHitPlayer())
				{
                    // Check if found player target is on fly mount or using flying form
                    if (playerTarget->HasAuraType(SPELL_AURA_FLY) || playerTarget->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED))
					{
                        playerTarget->CastSpell(playerTarget, SPELL_FLAK_CANNON_TRIGGER, TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE);
					}
				}
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_q11010_q11102_q11023_aggro_check_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q11010_q11102_q11023_aggro_check_SpellScript();
        }
};

class spell_q11010_q11102_q11023_aggro_burst : public SpellScriptLoader
{
    public:
        spell_q11010_q11102_q11023_aggro_burst() : SpellScriptLoader("spell_q11010_q11102_q11023_aggro_burst") { }

        class spell_q11010_q11102_q11023_aggro_burst_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_q11010_q11102_q11023_aggro_burst_AuraScript);

            void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (Unit* target = GetTarget())
				{
                    // On each tick cast Choose Loc to trigger summon
                    target->CastSpell(target, SPELL_CHOOSE_LOC);
				}
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_q11010_q11102_q11023_aggro_burst_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_q11010_q11102_q11023_aggro_burst_AuraScript();
        }
};

class spell_q11010_q11102_q11023_choose_loc : public SpellScriptLoader
{
    public:
        spell_q11010_q11102_q11023_choose_loc() : SpellScriptLoader("spell_q11010_q11102_q11023_choose_loc") { }

        class spell_q11010_q11102_q11023_choose_loc_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q11010_q11102_q11023_choose_loc_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
				Unit* caster = GetCaster();
                // Check for player that is in 65 y range
				std::list<Player*> playerList;
				Trinity::AnyPlayerInObjectRangeCheck checker(caster, 65.0f);
				Trinity::PlayerListSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(caster, playerList, checker);
				caster->VisitNearbyWorldObject(65.0f, searcher);
				for (std::list<Player*>::const_iterator itr = playerList.begin(); itr != playerList.end(); ++itr)
				{
					// Check if found player target is on fly mount or using flying form
					if ((*itr)->HasAuraType(SPELL_AURA_FLY) || (*itr)->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED))
						caster->SummonCreature(NPC_FEL_CANNON2, (*itr)->GetPositionX(), (*itr)->GetPositionY(), (*itr)->GetPositionZ());
				}
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_q11010_q11102_q11023_choose_loc_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q11010_q11102_q11023_choose_loc_SpellScript();
        }
};

class spell_q11010_q11102_q11023_q11008_check_fly_mount : public SpellScriptLoader
{
    public:
        spell_q11010_q11102_q11023_q11008_check_fly_mount() : SpellScriptLoader("spell_q11010_q11102_q11023_q11008_check_fly_mount") { }

        class spell_q11010_q11102_q11023_q11008_check_fly_mount_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q11010_q11102_q11023_q11008_check_fly_mount_SpellScript);

            SpellCastResult CheckRequirement()
            {
                Unit* caster = GetCaster();
                // This spell will be cast only if caster has one of these auras
                if (!(caster->HasAuraType(SPELL_AURA_FLY) || caster->HasAuraType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED)))
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_q11010_q11102_q11023_q11008_check_fly_mount_SpellScript::CheckRequirement);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q11010_q11102_q11023_q11008_check_fly_mount_SpellScript();
        }
};

enum SpellZuldrakRat
{
    SPELL_SUMMON_GORGED_LURKING_BASILISK = 50928,
};

class spell_q12527_zuldrak_rat : public SpellScriptLoader
{
    public:
        spell_q12527_zuldrak_rat() : SpellScriptLoader("spell_q12527_zuldrak_rat") { }

        class spell_q12527_zuldrak_rat_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q12527_zuldrak_rat_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SUMMON_GORGED_LURKING_BASILISK))
                    return false;
                return true;
            }

            void HandleScriptEffect(SpellEffIndex /* effIndex */)
            {
                if (GetHitAura() && GetHitAura()->GetStackAmount() >= GetSpellInfo()->StackAmount)
                {
                    GetHitUnit()->CastSpell((Unit*) NULL, SPELL_SUMMON_GORGED_LURKING_BASILISK, true);
                    if (Creature* basilisk = GetHitUnit()->ToCreature())
                        basilisk->DespawnAfterAction();
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_q12527_zuldrak_rat_SpellScript::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q12527_zuldrak_rat_SpellScript();
        }
};

enum Quest13291_13292_13239_13261Data
{
    // NPCs
    NPC_SKYTALON = 31583,
    NPC_DECOY    = 31578,
    SPELL_RIDE   = 56687,
};

class spell_q13291_q13292_q13239_q13261_frostbrood_skytalon_grab_decoy : public SpellScriptLoader
{
    public:
        spell_q13291_q13292_q13239_q13261_frostbrood_skytalon_grab_decoy() : SpellScriptLoader("spell_q13291_q13292_q13239_q13261_frostbrood_skytalon_grab_decoy") { }

        class spell_q13291_q13292_q13239_q13261_frostbrood_skytalon_grab_decoy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q13291_q13292_q13239_q13261_frostbrood_skytalon_grab_decoy_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_RIDE))
                    return false;

                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (!GetHitCreature())
                    return;
                // TO DO: Being triggered is hack, but in checkcast it doesn't pass aurastate requirements.
                // Beside that the decoy won't keep it's freeze animation state when enter.
                GetHitCreature()->CastSpell(GetCaster(), SPELL_RIDE, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_q13291_q13292_q13239_q13261_frostbrood_skytalon_grab_decoy_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q13291_q13292_q13239_q13261_frostbrood_skytalon_grab_decoy_SpellScript();
        }
};

class spell_q13291_q13292_q13239_q13261_armored_decoy_summon_skytalon : public SpellScriptLoader
{
    public:
        spell_q13291_q13292_q13239_q13261_armored_decoy_summon_skytalon() : SpellScriptLoader("spell_q13291_q13292_q13239_q13261_armored_decoy_summon_skytalon") { }

        class spell_q13291_q13292_q13239_q13261_armored_decoy_summon_skytalon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q13291_q13292_q13239_q13261_armored_decoy_summon_skytalon_SpellScript);

            void ChangeSummonPos(SpellEffIndex /*effIndex*/)
            {
                // Adjust effect summon position
                WorldLocation summonPos = *GetExplTargetDest();
                Position offset = { 0.0f, 0.0f, 20.0f, 0.0f };
                summonPos.RelocateOffset(offset);
                SetExplTargetDest(summonPos);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_q13291_q13292_q13239_q13261_armored_decoy_summon_skytalon_SpellScript::ChangeSummonPos, EFFECT_0, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q13291_q13292_q13239_q13261_armored_decoy_summon_skytalon_SpellScript();
        }
};

enum ThaumaturgyChannel
{
    SPELL_THAUMATURGY_CHANNEL = 21029,
};

class spell_q2203_thaumaturgy_channel : public SpellScriptLoader
{
    public:
        spell_q2203_thaumaturgy_channel() : SpellScriptLoader("spell_q2203_thaumaturgy_channel") { }

        class spell_q2203_thaumaturgy_channel_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_q2203_thaumaturgy_channel_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_THAUMATURGY_CHANNEL))
                    return false;
                return true;
            }

            void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();
                if (Unit* caster = GetCaster())
                    caster->CastSpell(caster, SPELL_THAUMATURGY_CHANNEL, false);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_q2203_thaumaturgy_channel_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_q2203_thaumaturgy_channel_AuraScript();
        }
};

enum TheHunterAndThePrince
{
    SPELL_ILLIDAN_KILL_CREDIT = 61748,
};

class spell_q13400_illidan_kill_master : public SpellScriptLoader
{
    public:
        spell_q13400_illidan_kill_master() : SpellScriptLoader("spell_q13400_illidan_kill_master") { }

        class spell_q13400_illidan_kill_master_SpellScript : public SpellScript
        {
           PrepareSpellScript(spell_q13400_illidan_kill_master_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ILLIDAN_KILL_CREDIT))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (caster->IsVehicle())
                    if (Unit* passenger = caster->GetVehicleKit()->GetPassenger(0))
                         passenger->CastSpell(passenger, SPELL_ILLIDAN_KILL_CREDIT, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_q13400_illidan_kill_master_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q13400_illidan_kill_master_SpellScript();
        }
};

enum RelicOfTheEarthenRing
{
    SPELL_TOTEM_OF_THE_EARTHEN_RING = 66747
};

class spell_q14100_q14111_make_player_destroy_totems : public SpellScriptLoader
{
    public:
        spell_q14100_q14111_make_player_destroy_totems() : SpellScriptLoader("spell_q14100_q14111_make_player_destroy_totems") { }

        class spell_q14100_q14111_make_player_destroy_totems_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q14100_q14111_make_player_destroy_totems_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_TOTEM_OF_THE_EARTHEN_RING))
                    return false;
                return true;
            }

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (Player* player = GetHitPlayer())
					player->CastSpell(player, SPELL_TOTEM_OF_THE_EARTHEN_RING, TRIGGERED_FULL_MASK);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_q14100_q14111_make_player_destroy_totems_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q14100_q14111_make_player_destroy_totems_SpellScript();
        }
};

enum RecallEyeOfAcherus
{
	SPELL_THE_EYE_OF_ACHERUS = 51852,
};

class spell_q12641_recall_eye_of_acherus : public SpellScriptLoader
{
    public:
        spell_q12641_recall_eye_of_acherus() : SpellScriptLoader("spell_q12641_recall_eye_of_acherus") { }

        class spell_q12641_recall_eye_of_acherus_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q12641_recall_eye_of_acherus_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Player* player = GetCaster()->GetCharmerOrOwner()->ToPlayer())
                {
                    player->StopCastingCharm();
                    player->StopCastingBindSight();
					player->RemoveAura(SPELL_THE_EYE_OF_ACHERUS);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_q12641_recall_eye_of_acherus_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q12641_recall_eye_of_acherus_SpellScript();
        }
};

class spell_q28813_set_health_random : public SpellScriptLoader
{
public:
    spell_q28813_set_health_random() : SpellScriptLoader("spell_q28813_set_health_random") { }

    class spell_q28813_set_health_random_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_q28813_set_health_random_SpellScript);

        void HandleDummyEffect()
        {
            Unit* caster = GetCaster();
            caster->SetHealth(caster->CountPctFromMaxHealth(urand(3, 5)*10));
        }

        void Register()
        {
            OnCast += SpellCastFn(spell_q28813_set_health_random_SpellScript::HandleDummyEffect);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q28813_set_health_random_SpellScript();
    }
};

class spell_q12661_q12669_q12676_q12677_q12713_summon_stefan : public SpellScriptLoader
{
    public:
        spell_q12661_q12669_q12676_q12677_q12713_summon_stefan() : SpellScriptLoader("spell_q12661_q12669_q12676_q12677_q12713_summon_stefan") { }

        class spell_q12661_q12669_q12676_q12677_q12713_summon_stefan_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q12661_q12669_q12676_q12677_q12713_summon_stefan_SpellScript);

            void ChangeSummonPos(SpellEffIndex /*effIndex*/)
            {
                // Adjust effect summon position
                WorldLocation summonPos = *GetExplTargetDest();
                Position offset = { 0.0f, 0.0f, 20.0f, 0.0f };
                summonPos.RelocateOffset(offset);
                SetExplTargetDest(summonPos);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_q12661_q12669_q12676_q12677_q12713_summon_stefan_SpellScript::ChangeSummonPos, EFFECT_0, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q12661_q12669_q12676_q12677_q12713_summon_stefan_SpellScript();
        }
};

class spell_q12414_hand_over_reins : public SpellScriptLoader
{
public:
	spell_q12414_hand_over_reins() : SpellScriptLoader("spell_q12414_hand_over_reins") { }

	class spell_q12414_hand_over_reins_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_q12414_hand_over_reins_SpellScript);

		void HandleScript(SpellEffIndex /*effIndex*/)
		{
			Creature* caster = GetCaster()->ToCreature();
			GetHitUnit()->ExitVehicle();

			if (caster)
				caster->DespawnAfterAction();
		}

		void Register()
		{
			OnEffectHitTarget += SpellEffectFn(spell_q12414_hand_over_reins_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_q12414_hand_over_reins_SpellScript();
	}
};

class spell_q12847_summon_soul_moveto_bunny : public SpellScriptLoader
{
    public:
        spell_q12847_summon_soul_moveto_bunny() : SpellScriptLoader("spell_q12847_summon_soul_moveto_bunny") { }

        class spell_q12847_summon_soul_moveto_bunny_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_q12847_summon_soul_moveto_bunny_SpellScript);

			void ChangeSummonPos(SpellEffIndex /*effIndex*/)
            {
				// Adjust effect summon position
                WorldLocation summonPos = *GetExplTargetDest();
                Position offset = { 0.0f, 0.0f, 15.5f, 0.0f };
                summonPos.RelocateOffset(offset);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_q12847_summon_soul_moveto_bunny_SpellScript::ChangeSummonPos, EFFECT_0, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_q12847_summon_soul_moveto_bunny_SpellScript();
        }
};

enum TheIronColossus
{
	SPELL_SUBMERGE_DUMMY = 56504, // CLICK (NEED SCRIPT) нажать для исчезновения
	SPELL_SUBMERGE_STATE = 37550, // для исчезновения
	SPELL_EMERGE_CLICK   = 56508, // добавляется на панель после исчезновения
	SPELL_EMERGE_VISUAL  = 50142, // применяется после снятия SPELL_SUBMERGE_STATE путем клика по SPELL_EMERGE_CLICK
};

class spell_q13007_the_iron_colossus : public SpellScriptLoader
{
public:
	spell_q13007_the_iron_colossus() : SpellScriptLoader("spell_q13007_the_iron_colossus") { }

    class spell_q13007_the_iron_colossus_SpellScript : public SpellScript
    {
		PrepareSpellScript(spell_q13007_the_iron_colossus_SpellScript);

        bool Validate(SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_SUBMERGE_DUMMY))
                return false;
            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
			if (Creature* target = GetHitCreature())
			{
				if (!target->HasAura(SPELL_SUBMERGE_STATE))
				{
					target->AddAura(SPELL_SUBMERGE_STATE, target);
					target->AddAura(SPELL_EMERGE_CLICK, target);
				}

				else if (!target->HasAura(SPELL_EMERGE_CLICK))
				{
					target->RemoveAurasDueToSpell(SPELL_SUBMERGE_STATE);
					target->RemoveAurasDueToSpell(SPELL_EMERGE_CLICK);
					target->CastSpell(target, SPELL_EMERGE_VISUAL, true);
				}
			}
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_q13007_the_iron_colossus_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_q13007_the_iron_colossus_SpellScript();
    }
};

void AddSC_quest_spell_scripts()
{
    new spell_q55_sacred_cleansing();
    new spell_q5206_test_fetid_skull();
    new spell_q6124_6129_apply_salve();
    new spell_q10255_administer_antidote();
    new spell_q11396_11399_force_shield_arcane_purple_x3();
    new spell_q11396_11399_scourging_crystal_controller();
    new spell_q11396_11399_scourging_crystal_controller_dummy();
    new spell_q11515_fel_siphon_dummy();
    new spell_q11587_arcane_prisoner_rescue();
    new spell_q11730_ultrasonic_screwdriver();
    new spell_q12459_seeds_of_natures_wrath();
    new spell_q12634_despawn_fruit_tosser();
    new spell_q12683_take_sputum_sample();
    new spell_q12851_going_bearback();
    new spell_q12937_relief_for_the_fallen();
    new spell_q11307_plagued_vrykul_dummy();
    new spell_q10041_q10040_who_are_they();
    new spell_symbol_of_life_dummy();
    new spell_q12659_ahunaes_knife();
    new spell_q9874_liquid_fire();
    new spell_q12805_lifeblood_dummy();
    new spell_q13280_13283_plant_battle_standard();
    new spell_q14112_14145_chum_the_water();
	new spell_q9361_purify_helboar_meat();
	new spell_q12237_rescue_villager();
	new spell_q12237_drop_off_villager();
	new spell_q9452_cast_net();
	new spell_q12277_wintergarde_mine_explosion();
	new spell_q12987_read_pronouncement();
	new spell_q12066_bunny_kill_credit();
	new spell_q12735_song_of_cleansing();
	new spell_q12372_cast_from_gossip_trigger();
	new spell_q12372_destabilize_azure_dragonshrine_dummy();
	new spell_q11010_q11102_q11023_aggro_check_aura();
	new spell_q11010_q11102_q11023_aggro_check();
	new spell_q11010_q11102_q11023_aggro_burst();
	new spell_q11010_q11102_q11023_choose_loc();
	new spell_q11010_q11102_q11023_q11008_check_fly_mount();
	new spell_q12372_azure_on_death_force_whisper();
	new spell_q12527_zuldrak_rat();
	new spell_q13291_q13292_q13239_q13261_frostbrood_skytalon_grab_decoy();
	new spell_q13291_q13292_q13239_q13261_armored_decoy_summon_skytalon();
	new spell_q2203_thaumaturgy_channel();
	new spell_q13400_illidan_kill_master();
	new spell_q14100_q14111_make_player_destroy_totems();
	new spell_q12641_recall_eye_of_acherus();
	new spell_q28813_set_health_random();
	new spell_q12661_q12669_q12676_q12677_q12713_summon_stefan();
	new spell_q12414_hand_over_reins();
	new spell_q12847_summon_soul_moveto_bunny();
	new spell_q13007_the_iron_colossus();
}