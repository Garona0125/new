/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "Config.h"

enum SpellMask
{
	SPELL_SECOND_TALENT_SPEC = 63624,
	SPELL_DUAL_SPEC_SWITCH   = 63680,
	// Spell Masks
	SPELL_MASK_CLASS         = 1,
	SPELL_MASK_RIDING        = 2,
	SPELL_MASK_MOUNT         = 4,
	SPELL_MASK_WEAPON        = 8,
	SPELL_MASK_PROFESSION    = 16,
	SPELL_MASK_DUAL_SPEC     = 32,
};

struct LearnSpellForClassInfo
{
	uint32 SpellId;
    uint8 SpellMask;
    uint32 RequiredClassMask;
    uint32 RequiredRaceMask;
    uint8 RequiredLevel;
    uint32 RequiredSpellId;
    uint16 RequiredSkillId;
    uint16 RequiredSkillValue;
};

bool AutoLearnEnable = false;

uint8 OnLevelSpellMask = 0;
uint8 OnSkillSpellMask = 0;
uint8 OnLoginSpellMask = 0;

std::vector<LearnSpellForClassInfo> LearnSpellForClass;

class mod_auto_learn_world_script : public WorldScript
{
public:
	mod_auto_learn_world_script() : WorldScript("mod_auto_learn_world_script") { }

    void OnConfigLoad(bool /*reload*/)
    {
        AutoLearnEnable = GetConfigSettings::GetBoolState("AutoLearn.Enable", false);

        if (!AutoLearnEnable)
            return;

		uint8 loadSpellMask = OnLevelSpellMask | OnSkillSpellMask;
        OnLevelSpellMask = 0;
        OnSkillSpellMask = 0;
        OnLoginSpellMask = 0;

        if (GetConfigSettings::GetBoolState("AutoLearn.Check.Level", false))
        {
			if (GetConfigSettings::GetBoolState("AutoLearn.Spell.Class", false))
				OnLevelSpellMask += SPELL_MASK_CLASS;

            if (GetConfigSettings::GetBoolState("AutoLearn.Spell.Riding", false))
				OnLevelSpellMask += SPELL_MASK_RIDING;

            if (GetConfigSettings::GetBoolState("AutoLearn.Spell.Mount", false))
                OnLevelSpellMask += SPELL_MASK_MOUNT;

            if (GetConfigSettings::GetBoolState("AutoLearn.Spell.Weapon", false))
                OnLevelSpellMask += SPELL_MASK_WEAPON;

            if (GetConfigSettings::GetBoolState("AutoLearn.Dual.Spec", false))
                OnLevelSpellMask += SPELL_MASK_DUAL_SPEC;

            if (GetConfigSettings::GetBoolState("AutoLearn.Login.Spell", false))
                OnLoginSpellMask += OnLevelSpellMask;
		}

        if (GetConfigSettings::GetBoolState("AutoLearn.Spell.Profession", false))
            OnSkillSpellMask += SPELL_MASK_PROFESSION;

        if (GetConfigSettings::GetBoolState("AutoLearn.Login.Skill", false))
            OnLoginSpellMask += OnSkillSpellMask;

        if (loadSpellMask != (OnLevelSpellMask | OnSkillSpellMask))
			LoadDataFromDataBase();
    }

    void LoadDataFromDataBase()
    {
        LearnSpellForClass.clear();
        uint8 spellMask = OnLevelSpellMask | OnSkillSpellMask;

        if (spellMask == 0)
            return;

        sLog->OutConsole("QUANTUMCORE SERVER: Loading AutoLearn Spells...");
        uint32 oldMSTime = getMSTime();

        QueryResult result = WorldDatabase.PQuery("SELECT SpellId, SpellMask, RequiredClassMask, RequiredRaceMask, RequiredLevel, RequiredSpellId, RequiredSkillId, RequiredSkillValue FROM `mod_auto_learn`");

        if (!result)
		{
			sLog->OutConsole("==================================================================");
			sLog->OutConsole(">> Loaded 0 Spell defenitions. DB table `mod_auto_learn` is empty!");
			sLog->OutConsole("==================================================================");
            return;
		}

        uint16 count = 0;

        do
        {
			Field* fields = result->Fetch();

            LearnSpellForClassInfo Spell;

            Spell.SpellId = fields[0].GetUInt32();
            Spell.SpellMask = fields[1].GetUInt16();
            Spell.RequiredClassMask = fields[2].GetUInt32();
            Spell.RequiredRaceMask = fields[3].GetUInt32();
            Spell.RequiredLevel = fields[4].GetUInt8();
            Spell.RequiredSpellId = fields[5].GetUInt32();
            Spell.RequiredSkillId = fields[6].GetUInt16();
            Spell.RequiredSkillValue = fields[7].GetUInt16();

            if (!sSpellMgr->GetSpellInfo(Spell.SpellId))
            {
                sLog->OutErrorDatabase("AutoLearn: Spell (ID: %u) non-existing", Spell.SpellId);
                continue;
            }

            if (!(Spell.SpellMask & spellMask))
                continue;

            if (Spell.RequiredClassMask != 0 && !(Spell.RequiredClassMask & CLASSMASK_ALL_PLAYABLE))
            {
                sLog->OutErrorDatabase("AutoLearn: Spell (ID: %u) RequiredClassMask (Mask: %u) non-existing", Spell.SpellId, Spell.RequiredClassMask);
                continue;
            }

            if (Spell.RequiredRaceMask != 0 && !(Spell.RequiredRaceMask & RACEMASK_ALL_PLAYABLE))
            {
                sLog->OutErrorDatabase("AutoLearn: Spell (ID: %u) RequiredRaceMask (Mask: %u) non-existing", Spell.SpellId, Spell.RequiredRaceMask);
                continue;
            }

            if (Spell.RequiredSpellId != 0 && !sSpellMgr->GetSpellInfo(Spell.RequiredSpellId))
            {
                sLog->OutErrorDatabase("AutoLearn: Spell (ID: %u) RequiredSpellId (ID: %u) non-existing", Spell.SpellId, Spell.RequiredSpellId);
                continue;
            }

            LearnSpellForClass.push_back(Spell);
            ++count;
        }
        while (result->NextRow());

		sLog->OutConsole("========================================");
        sLog->OutConsole(">> Loaded %u AutoLearn spells in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
		sLog->OutConsole("========================================");
        sLog->OutConsole();
    }
};

class mod_auto_learn_player_script : public PlayerScript
{
public:
	mod_auto_learn_player_script() : PlayerScript("mod_auto_learn_player_script") { }

    void OnLevelChanged(Player* player, uint8 /*oldLevel*/)
    {
		if (!AutoLearnEnable)
			return;

		AutoLearnSpell(OnLevelSpellMask, player);
    }

    void OnLogin(Player* player)
    {
        if (!AutoLearnEnable)
            return;

        AutoLearnSpell(OnLoginSpellMask, player);
    }

    void OnPlayerSkillUpdate(Player* player, uint16 SkillId, uint16 /*SkillValue*/, uint16 SkillNewValue)
    {
        if (!AutoLearnEnable)
            return;

        AutoLearnSpell(OnSkillSpellMask, player, SkillId, SkillNewValue);
    }

    void AutoLearnSpell(uint8 SpellMask, Player* player, uint16 SkillId = 0, uint16 SkillValue = 0)
    {
        if (SpellMask & SPELL_MASK_DUAL_SPEC)
        {
			learnDualSpec(player);
			SpellMask -= SPELL_MASK_DUAL_SPEC;
        }

        if (SpellMask == 0)
			return;

        uint32 PlayerClassMask = player->GetCurrentClassMask();
        uint32 PlayerRaceMask  = player->GetCurrentRaceMask();
        uint8 PlayerLevel = player->GetCurrentLevel();

        for (uint16 i = 0; i < LearnSpellForClass.size(); ++i)
        {
            LearnSpellForClassInfo &Spell = LearnSpellForClass[i];

            if (!(Spell.SpellMask & SpellMask))
				continue;
            if (Spell.RequiredClassMask != 0 && !(Spell.RequiredClassMask & PlayerClassMask))
				continue;
            if (Spell.RequiredRaceMask != 0 && !(Spell.RequiredRaceMask & PlayerRaceMask))
				continue;
            if (Spell.RequiredLevel > PlayerLevel)
				continue;
            if (Spell.RequiredSkillId != SkillId)
				continue;
            if (Spell.RequiredSkillValue > SkillValue)
				continue;
            if (player->HasSpell(Spell.SpellId))
				continue;
            if (Spell.RequiredSpellId != 0 && !player->HasSpell(Spell.RequiredSpellId))
				continue;

            player->learnSpell(Spell.SpellId, false);
        }
    }

    void learnDualSpec(Player* player)
    {
        if (player->GetCurrentLevel() < sWorld->getIntConfig(CONFIG_MIN_DUALSPEC_LEVEL))
			return;

        if (player->GetSpecsCount() != 1)
			return;

        player->CastSpell(player, SPELL_DUAL_SPEC_SWITCH, true, 0, 0, player->GetGUID());
        player->CastSpell(player, SPELL_SECOND_TALENT_SPEC, true, 0, 0, player->GetGUID());
    }
};

void AddSC_world_auto_learn_spell_system()
{
    new mod_auto_learn_player_script();
    new mod_auto_learn_world_script();
}