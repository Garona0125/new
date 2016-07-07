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
#include "QuantumGossip.h"
#include "Config.h"
#include "../../../scripts/QuantumCoreScripts/SystemScripts/world_fun_system.h"

enum ProfessionTexts
{
	TEXT_ID_ALCHEMY        = 12249,
	TEXT_ID_BLACKSMITHING  = 12250,
	TEXT_ID_ENCHANTING     = 12251,
	TEXT_ID_ENGINEERING    = 12252,
	TEXT_ID_HERBALISM      = 12253,
	TEXT_ID_INSCRIPTION    = 12254,
	TEXT_ID_JEWELCRAFTING  = 12255,
	TEXT_ID_LEATHERWORKING = 12256,
	TEXT_ID_MINING         = 12257,
	TEXT_ID_SKINNING       = 12258,
	TEXT_ID_TAILORING      = 12259,
	TEXT_ID_COOKING        = 12260,
	TEXT_ID_FIRST_AID      = 12261,
	TEXT_ID_FISHING        = 12262,
	TEXT_ID_SECOND_MENU    = 12263,
	TEXT_ID_ALREADY_SKILL  = 12264,
	TEXT_ID_MAIN_MENU      = 12034,
};

class npc_profession_master : public CreatureScript
{
public:
	npc_profession_master() : CreatureScript("npc_profession_master") { }

	void CompleteLearnProfession(Player* player, Creature* creature, SkillType skill)
	{
		if (PlayerAlreadyHasNineProfessions(player) && !IsSecondarySkill(skill))
			creature->MonsterWhisper(TEXT_ID_ALREADY_SKILL, LANG_UNIVERSAL, player->GetGUID());
		else
		{
			if (!LearnAllRecipesInProfession(player, skill))
				creature->MonsterWhisper("Error!", player->GetGUID());
		}
	}

	bool IsSecondarySkill(SkillType skill) const
	{
		return skill == SKILL_COOKING || skill == SKILL_FIRST_AID || skill == SKILL_FISHING;
	}

	bool PlayerAlreadyHasNineProfessions(const Player* player) const
	{
		uint32 SkillCount = 0;

		if (player->HasSkill(SKILL_MINING))
			SkillCount++;

		if (player->HasSkill(SKILL_SKINNING))
			SkillCount++;

		if (player->HasSkill(SKILL_HERBALISM))
			SkillCount++;

		if (SkillCount >= 3)
			return true;

		for (uint32 i = 0; i < sSkillLineStore.GetNumRows(); ++i)
		{
			SkillLineEntry const *SkillInfo = sSkillLineStore.LookupEntry(i);

			if (!SkillInfo)
				continue;

			if (SkillInfo->categoryId == SKILL_CATEGORY_SECONDARY)
				continue;

			if ((SkillInfo->categoryId != SKILL_CATEGORY_PROFESSION) || !SkillInfo->canLink)
				continue;

			const uint32 skillID = SkillInfo->id;
			if (player->HasSkill(skillID))
				SkillCount++;

			if (SkillCount >= 3)
				return true;
		}
		return false;
	}

	bool LearnAllRecipesInProfession(Player* player, SkillType skill)
	{
		ChatHandler handler(player);
		char* skill_name;

		SkillLineEntry const *SkillInfo = sSkillLineStore.LookupEntry(skill);
		skill_name = SkillInfo->name[handler.GetSessionDbcLocale()];

		if (!SkillInfo)
		{
			sLog->OutErrorConsole("Profession Master: received non-valid skill ID (LearnAllRecipesInProfession)");
			return false;
		}

		LearnSkillRecipesHelper(player, SkillInfo->id);

		uint16 maxLevel = player->GetPureMaxSkillValue(SkillInfo->id);
		player->SetSkill(SkillInfo->id, player->GetSkillStep(SkillInfo->id), maxLevel, maxLevel);
		handler.PSendSysMessage(LANG_COMMAND_LEARN_ALL_RECIPES, skill_name);
		return true;
	}

	void LearnSkillRecipesHelper(Player* player, uint32 skill_id)
	{
		uint32 classmask = player->GetCurrentClassMask();

		for (uint32 j = 0; j < sSkillLineAbilityStore.GetNumRows(); ++j)
		{
			SkillLineAbilityEntry const *skillLine = sSkillLineAbilityStore.LookupEntry(j);
			if (!skillLine)
				continue;

			// Wrong skill
			if (skillLine->skillId != skill_id)
				continue;

			// Not high rank
			if (skillLine->forward_spellid)
				continue;

			// Skip racial skills
			if (skillLine->racemask != 0)
				continue;

			// Skip wrong class skills
			if (skillLine->classmask && (skillLine->classmask & classmask) == 0)
				continue;

			SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(skillLine->spellId);
			if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo, player, false))
				continue;

			player->learnSpell(skillLine->spellId, false);
		}
	}

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (GetConfigSettings::GetBoolState("Profession.Master.Enable", true))
		{
			player->PlayerTalkClass->ClearMenus();
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_ALCHEMY), GOSSIP_SENDER_MAIN, 2);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_BLACKSMITHING), GOSSIP_SENDER_MAIN, 3);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_ENCHANTING), GOSSIP_SENDER_MAIN, 4);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_ENGINEERING), GOSSIP_SENDER_MAIN, 5);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_HERBALISM), GOSSIP_SENDER_MAIN, 6);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_INSCRIPTION), GOSSIP_SENDER_MAIN, 7);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_JEWELCRAFTING), GOSSIP_SENDER_MAIN, 8);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_LEATHERWORKING), GOSSIP_SENDER_MAIN, 9);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_MINING), GOSSIP_SENDER_MAIN, 10);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_SKINNING), GOSSIP_SENDER_MAIN, 11);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_TAILORING), GOSSIP_SENDER_MAIN, 12);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_SECOND_MENU), GOSSIP_SENDER_MAIN, 13);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
			return true;
		}

		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
	{
		switch (action)
		{
            case 2:
				CompleteLearnProfession(player, creature, SKILL_ALCHEMY);
				player->CastSpell(player, SPELL_BLESSING_VISUAL, true);
				player->CLOSE_GOSSIP_MENU();
				break;
			case 3:
				CompleteLearnProfession(player, creature, SKILL_BLACKSMITHING);
				player->learnSpell(17040, false);
				player->CastSpell(player, SPELL_BLESSING_VISUAL, true);
				player->CLOSE_GOSSIP_MENU();
				break;
			case 4:
				CompleteLearnProfession(player, creature,  SKILL_ENCHANTING);
				player->CastSpell(player, SPELL_BLESSING_VISUAL, true);
				player->CLOSE_GOSSIP_MENU();
				break;
			case 5:
				CompleteLearnProfession(player, creature, SKILL_ENGINEERING);
				player->CastSpell(player, SPELL_BLESSING_VISUAL, true);
				player->CLOSE_GOSSIP_MENU();
				break;
			case 6:
				CompleteLearnProfession(player, creature, SKILL_HERBALISM);
				player->CastSpell(player, SPELL_BLESSING_VISUAL, true);
				player->CLOSE_GOSSIP_MENU();
				break;
        case 7:
			CompleteLearnProfession(player, creature, SKILL_INSCRIPTION);
			player->CastSpell(player, SPELL_BLESSING_VISUAL, true);
			player->CLOSE_GOSSIP_MENU();
			break;
        case 8:
			CompleteLearnProfession(player, creature, SKILL_JEWELCRAFTING);
			player->CastSpell(player, SPELL_BLESSING_VISUAL, true);
			player->CLOSE_GOSSIP_MENU();
			break;
        case 9:
			CompleteLearnProfession(player, creature, SKILL_LEATHERWORKING);
			player->CastSpell(player, SPELL_BLESSING_VISUAL, true);
			player->CLOSE_GOSSIP_MENU();
			break;
        case 10:
			CompleteLearnProfession(player, creature, SKILL_MINING);
			player->CastSpell(player, SPELL_BLESSING_VISUAL, true);
			player->CLOSE_GOSSIP_MENU();
			break;
        case 11:
			CompleteLearnProfession(player, creature, SKILL_SKINNING);
			player->CastSpell(player, SPELL_BLESSING_VISUAL, true);
			player->CLOSE_GOSSIP_MENU();
			break;
        case 12:
			CompleteLearnProfession(player, creature, SKILL_TAILORING);
			player->CastSpell(player, SPELL_BLESSING_VISUAL, true);
			player->CLOSE_GOSSIP_MENU();
			break;
        case 13:
			player->PlayerTalkClass->ClearMenus();
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_COOKING), GOSSIP_SENDER_MAIN, 14);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_FIRST_AID), GOSSIP_SENDER_MAIN, 15);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_FISHING), GOSSIP_SENDER_MAIN, 16);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_MAIN_MENU), GOSSIP_SENDER_MAIN, 17);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
			break;
        case 14:
			CompleteLearnProfession(player, creature, SKILL_COOKING);
			player->CastSpell(player, SPELL_BLESSING_VISUAL, true);
			player->CLOSE_GOSSIP_MENU();
			break;
        case 15:
			CompleteLearnProfession(player, creature, SKILL_FIRST_AID);
			player->CastSpell(player, SPELL_BLESSING_VISUAL, true);
			player->CLOSE_GOSSIP_MENU();
			break;
        case 16:
			CompleteLearnProfession(player, creature, SKILL_FISHING);
			player->CastSpell(player, SPELL_BLESSING_VISUAL, true);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 17:
			OnGossipHello(player, creature);
			break;
		}
		return true;
	}

	struct npc_profession_masterAI : public QuantumBasicAI
    {
        npc_profession_masterAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 BuffTimer;

        void Reset()
        {
			BuffTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

        void UpdateAI(uint32 const diff)
        {
			if (!me->IsInCombatActive() && !me->IsInCombatActive())
			{
				if (!me->HasAuraEffect(SPELL_CASCADE_OF_ROSES, 0))
					DoCast(me, SPELL_CASCADE_OF_ROSES, true);
			}

			if (BuffTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_WATER_SHIELD, true);
				DoCast(me, SPELL_STAMINA, true);
				DoCast(me, SPELL_FLASK_OF_PURE_MOJO, true);
				DoCast(me, SPELL_HOLY_CHAMPION, true);
				BuffTimer = 10*MINUTE*IN_MILLISECONDS;
			}
			else BuffTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_profession_masterAI(creature);
    }
};

 void AddSC_npc_profession_master()
 {
	 new npc_profession_master();
 }