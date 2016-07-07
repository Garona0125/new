/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

/*
 * GOSSIP_ICON_CHAT          = 0    White chat bubble
 * GOSSIP_ICON_VENDOR        = 1    Brown bag
 * GOSSIP_ICON_TAXI          = 2    Flight
 * GOSSIP_ICON_TRAINER       = 3    Book
 * GOSSIP_ICON_INTERACT_1    = 4    Interaction wheel
 * GOSSIP_ICON_INTERACT_2    = 5    Interaction wheel
 * GOSSIP_ICON_MONEY_BAG     = 6    Brown bag with yellow dot
 * GOSSIP_ICON_TALK          = 7    White chat bubble with black dots
 * GOSSIP_ICON_TABARD        = 8    Tabard
 * GOSSIP_ICON_BATTLE        = 9    Two swords
 * GOSSIP_ICON_DOT           = 10   Yellow dot
 * GOSSIP_ICON_CHAT_11       = 11,  White chat bubble
 * GOSSIP_ICON_CHAT_12       = 12,  White chat bubble
 * GOSSIP_ICON_CHAT_13       = 13,  White chat bubble
 * GOSSIP_ICON_UNK_14        = 14,  INVALID - DO NOT USE
 * GOSSIP_ICON_UNK_15        = 15,  INVALID - DO NOT USE
 * GOSSIP_ICON_CHAT_16       = 16,  White chat bubble
 * GOSSIP_ICON_CHAT_17       = 17,  White chat bubble
 * GOSSIP_ICON_CHAT_18       = 18,  White chat bubble
 * GOSSIP_ICON_CHAT_19       = 19,  White chat bubble
 * GOSSIP_ICON_CHAT_20       = 20,  White chat bubble
 */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "QuantumGossip.h"
#include "Config.h"

#define EMOTE_NPC_OFF "|CFF9933FF Npc Disable!|r"

enum Enchants
{
    ENCHANT_WEP_BERSERKING              = 3789,
    ENCHANT_WEP_BLADE_WARD              = 3869,
    ENCHANT_WEP_BLOOD_DRAINING          = 3870,
    ENCHANT_WEP_ACCURACY                = 3788,
    ENCHANT_WEP_AGILITY_1H              = 1103,
    ENCHANT_WEP_SPIRIT                  = 3844,
    ENCHANT_WEP_BATTLEMASTER            = 2675,
    ENCHANT_WEP_BLACK_MAGIC             = 3790,
    ENCHANT_WEP_ICEBREAKER              = 3239,
    ENCHANT_WEP_LIFEWARD                = 3241,
    ENCHANT_WEP_MIGHTY_SPELL_POWER      = 3834, // One-hand
    ENCHANT_WEP_EXECUTIONER             = 3225,
    ENCHANT_WEP_POTENCY                 = 3833,
    ENCHANT_WEP_TITANGUARD              = 3851,
    ENCHANT_2WEP_MASSACRE               = 3827,
	ENCHANT_2WEP_SCOURGEBANE			= 3247,
	ENCHANT_2WEP_GIANT_SLAYER			= 3251,
	ENCHANT_2WEP_GREATER_SPELL_POWER    = 3854,
	ENCHANT_2WEP_AGILITY	            = 2670,
	ENCHANT_2WEP_MONGOOSE               = 2673,

    ENCHANT_SHIELD_DEFENSE              = 1952,
    ENCHANT_SHIELD_INTELLECT            = 1128,
    ENCHANT_SHIELD_RESILIENCE           = 3229,
	ENCHANT_SHIELD_BLOCK				= 2655,
	ENCHANT_SHIELD_STAMINA				= 1071,
	ENCHANT_SHIELD_TOUGHSHIELD			= 2653,
    ENCHANT_SHIELD_TITANIUM_PLATING     = 3849,

    ENCHANT_HEAD_BLISSFUL_MENDING       = 3819,
    ENCHANT_HEAD_BURNING_MYSTERIES      = 3820,
    ENCHANT_HEAD_DOMINANCE              = 3796,
    ENCHANT_HEAD_SAVAGE_GLADIATOR       = 3842,
    ENCHANT_HEAD_STALWART_PROTECTOR     = 3818,
    ENCHANT_HEAD_TORMENT                = 3817,
    ENCHANT_HEAD_TRIUMPH                = 3795,
	ENCHANT_HEAD_ECLIPSED_MOON			= 3815,
	ENCHANT_HEAD_FLAME_SOUL				= 3816,
	ENCHANT_HEAD_FLEEING_SHADOW			= 3814,
	ENCHANT_HEAD_FROSTY_SOUL			= 3812,
	ENCHANT_HEAD_TOXIC_WARDING			= 3813,

    ENCHANT_SHOULDER_MASTERS_AXE        = 3835,
    ENCHANT_SHOULDER_MASTERS_CRAG       = 3836,
    ENCHANT_SHOULDER_MASTERS_PINNACLE   = 3837,
    ENCHANT_SHOULDER_MASTERS_STORM      = 3838,
    ENCHANT_SHOULDER_GREATER_AXE        = 3808,
    ENCHANT_SHOULDER_GREATER_CRAG       = 3809,
    ENCHANT_SHOULDER_GREATER_GLADIATOR  = 3852,
    ENCHANT_SHOULDER_GREATER_PINNACLE   = 3811,
    ENCHANT_SHOULDER_GREATER_STORM      = 3810,
    ENCHANT_SHOULDER_DOMINANCE          = 3794,
    ENCHANT_SHOULDER_TRIUMPH            = 3793,

    ENCHANT_CLOAK_DARKGLOW_EMBROIDERY   = 3728,
    ENCHANT_CLOAK_SWORDGUARD_EMBROIDERY = 3730,
    ENCHANT_CLOAK_LIGHTWEAVE_EMBROIDERY = 3722,
    ENCHANT_CLOAK_SPRINGY_ARACHNOWEAVE  = 3859,
    ENCHANT_CLOAK_WISDOM                = 3296,
    ENCHANT_CLOAK_TITANWEAVE            = 1951,
    ENCHANT_CLOAK_SPELL_PIERCING        = 3243,
    ENCHANT_CLOAK_SHADOW_ARMOR          = 3256,
    ENCHANT_CLOAK_MIGHTY_ARMOR          = 3294,
    ENCHANT_CLOAK_MAJOR_AGILITY         = 1099,
    ENCHANT_CLOAK_GREATER_SPEED         = 3831,

	ENCHANT_LEG_EARTHEN					= 3853,
	ENCHANT_LEG_FROSTHIDE				= 3822,
	ENCHANT_LEG_ICESCALE				= 3823,
	ENCHANT_LEG_BRILLIANT_SPELLTHREAD	= 3719,
	ENCHANT_LEG_SAPPHIRE_SPELLTHREAD	= 3721,
	ENCHANT_LEG_DRAGONSCALE				= 3331,
	ENCHANT_LEG_WYRMSCALE				= 3332,

	ENCHANT_GLOVES_GREATER_BLASTING		= 3249,
	ENCHANT_GLOVES_ARMSMAN				= 3253,
	ENCHANT_GLOVES_CRUSHER				= 1603,
	ENCHANT_GLOVES_AGILITY				= 3222,
	ENCHANT_GLOVES_PRECISION			= 3234,
	ENCHANT_GLOVES_EXPERTISE			= 3231,
	ENCHANT_GLOVES_GREATER_SP			= 3246,

	ENCHANT_BRACERS_MAJOR_STAMINA		= 3850,
	ENCHANT_BRACERS_SUPERIOR_SP			= 2332,
	ENCHANT_BRACERS_GREATER_ASSUALT		= 3845,
	ENCHANT_BRACERS_MAJOR_SPIRT			= 1147,
	ENCHANT_BRACERS_EXPERTISE			= 3231,
	ENCHANT_BRACERS_GREATER_STATS		= 2661,
	ENCHANT_BRACERS_INTELLECT			= 1119,
	ENCHANT_BRACERS_FURL_ARCANE			= 3763,
	ENCHANT_BRACERS_FURL_FIRE			= 3759,
	ENCHANT_BRACERS_FURL_FROST			= 3760,
	ENCHANT_BRACERS_FURL_NATURE			= 3762,
	ENCHANT_BRACERS_FURL_SHADOW			= 3761,
	ENCHANT_BRACERS_FURL_ATTACK			= 3756,
	ENCHANT_BRACERS_FURL_STAMINA		= 3757,
	ENCHANT_BRACERS_FURL_SPELLPOWER		= 3758,

	ENCHANT_CHEST_POWERFUL_STATS        = 3832,
	ENCHANT_CHEST_SUPER_HEALTH          = 3297,
	ENCHANT_CHEST_GREATER_MAINA_REST    = 2381,
	ENCHANT_CHEST_EXCEPTIONAL_RESIL		= 3245,
	ENCHANT_CHEST_GREATER_DEFENSE		= 1953,

	ENCHANT_BOOTS_GREATER_ASSAULT       = 1597,
	ENCHANT_BOOTS_TUSKARS_VITLIATY      = 3232,
	ENCHANT_BOOTS_SUPERIOR_AGILITY      = 983,
	ENCHANT_BOOTS_GREATER_SPIRIT        = 1147,
	ENCHANT_BOOTS_GREATER_VITALITY      = 3244,
	ENCHANT_BOOTS_ICEWALKER             = 3826,
	ENCHANT_BOOTS_GREATER_FORTITUDE     = 1075,
	ENCHANT_BOOTS_NITRO_BOOTS           = 3606,
	ENCHANT_BOOTS_PYRO_ROCKET           = 3603,
	ENCHANT_BOOTS_HYPERSPEED            = 3604,
	ENCHANT_BOOTS_ARMOR_WEBBING         = 3860,

	ENCHANT_RING_ASSAULT                = 3839,
	ENCHANT_RING_GREATER_SP             = 3840,
	ENCHANT_RING_STAMINA                = 3791,
};
 
void Enchant(Player* player, Item* item, uint32 enchantid)
{
	if (!item)
    {
        player->GetSession()->SendNotification("You must first equip the item you are trying to enchant in order to enchant it!");
        return;
    }

    if (!enchantid)
    {
        player->GetSession()->SendNotification("Something went wrong in the code. It has been logged for developers and will be looked into, sorry for the inconvenience.");
        sLog->OutErrorConsole("Enchant NPC 'enchantid' is NULL, something went wrong here!");
        return;
    }
		
    item->ClearEnchantment(PERM_ENCHANTMENT_SLOT);
    item->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchantid, 0, 0);
    player->GetSession()->SendNotification("|cff800080%s |cffFF0000 Succesfully enchanted!", item->GetTemplate()->Name1.c_str());
}

void RemoveEnchant(Player* player, Item* item)
{
	if (!item)
	{
		player->GetSession()->SendNotification("You don't have the item equipped?");
		return;
	}

	item->ClearEnchantment(PERM_ENCHANTMENT_SLOT);
	player->GetSession()->SendNotification("|cff800080%s's |cffFF0000 enchant has successfully been removed!", item->GetTemplate()->Name1.c_str());
}

class npc_enchancement_master : public CreatureScript
{
public:
    npc_enchancement_master() : CreatureScript("npc_enchancement_master") { }

	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (GetConfigSettings::GetBoolState("Enchancement.Master.Enable", true))
		{
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Weapon", GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Off-Hand Weapon", GOSSIP_SENDER_MAIN, 13);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant 2H Weapon", GOSSIP_SENDER_MAIN, 2);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Shield", GOSSIP_SENDER_MAIN, 3);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Head", GOSSIP_SENDER_MAIN, 4);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Shoulders", GOSSIP_SENDER_MAIN, 5);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Cloak", GOSSIP_SENDER_MAIN, 6);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Chest", GOSSIP_SENDER_MAIN, 7);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Bracers", GOSSIP_SENDER_MAIN, 8);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Gloves", GOSSIP_SENDER_MAIN, 9);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Legs", GOSSIP_SENDER_MAIN, 10);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Feet", GOSSIP_SENDER_MAIN, 11);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());

			if (player->HasSkill(SKILL_ENCHANTING) && player->GetSkillValue(SKILL_ENCHANTING) == 450)
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Rings", GOSSIP_SENDER_MAIN, 12);
			
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "I wish to remove my enchant", GOSSIP_SENDER_MAIN, 14);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
		}
		else
		{
			player->CLOSE_GOSSIP_MENU();
			creature->MonsterTextEmote(EMOTE_NPC_OFF, LANG_UNIVERSAL, true);
		}
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
	{
		Item* item;

		player->PlayerTalkClass->ClearMenus();
		switch (action)
		{
		case 1: // Enchant Weapon
			if (player->HasSkill(SKILL_ENCHANTING) && player->GetSkillValue(SKILL_ENCHANTING) == 450)
			{
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Blade Ward", GOSSIP_SENDER_MAIN, 102);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Blood Draining", GOSSIP_SENDER_MAIN, 103);
			}

			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Exceptional Agility", GOSSIP_SENDER_MAIN, 100);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Exceptional Spirit", GOSSIP_SENDER_MAIN, 101);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Berserking", GOSSIP_SENDER_MAIN, 104);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Accuracy", GOSSIP_SENDER_MAIN, 105);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Black Magic", GOSSIP_SENDER_MAIN, 106);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Battlemaster", GOSSIP_SENDER_MAIN, 107);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Icebreaker", GOSSIP_SENDER_MAIN, 108);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Lifeward", GOSSIP_SENDER_MAIN, 109);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Titanguard", GOSSIP_SENDER_MAIN, 110);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Superior Potency", GOSSIP_SENDER_MAIN, 111);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Mighty Spellpower", GOSSIP_SENDER_MAIN, 112);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Mongoose", GOSSIP_SENDER_MAIN, 113);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Executioner", GOSSIP_SENDER_MAIN, 114);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "<-Back", GOSSIP_SENDER_MAIN, 300);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
			return true;
			break;
		case 2: // Enchant 2H Weapon
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND);

			if (!item)
			{
				player->GetSession()->SendNotification("You must equip a Two-Handed weapon first.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_2H_WEAPON)
			{
				player->PlayerTalkClass->ClearMenus();
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Berserking", GOSSIP_SENDER_MAIN, 104);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Greater Spellpower", GOSSIP_SENDER_MAIN, 115);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Major Agility", GOSSIP_SENDER_MAIN, 116);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Massacre", GOSSIP_SENDER_MAIN, 117);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Mongoose", GOSSIP_SENDER_MAIN, 113);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Executioner", GOSSIP_SENDER_MAIN, 114);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "<-Back", GOSSIP_SENDER_MAIN, 300);
				player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
				return true;
			}
			else
			{
				player->GetSession()->SendNotification("You don't have a Two-Handed weapon equipped.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 3: // Enchant Shield
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendNotification("You must equip a shield first.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_SHIELD)
			{
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Defense", GOSSIP_SENDER_MAIN, 118);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Greater Intellect", GOSSIP_SENDER_MAIN, 119);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Resilience", GOSSIP_SENDER_MAIN, 120);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Titanium Plating", GOSSIP_SENDER_MAIN, 121);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Major Stamina", GOSSIP_SENDER_MAIN, 122);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Tough Shield", GOSSIP_SENDER_MAIN, 123);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "<-Back", GOSSIP_SENDER_MAIN, 300);
				player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
				return true;
			}
			else
			{
				player->GetSession()->SendNotification("You don't have a shield equipped.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 4: // Enchant Head
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Arcanum of Blissful Mending", GOSSIP_SENDER_MAIN, 124);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Arcanum of Burning Mysteries", GOSSIP_SENDER_MAIN, 125);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Arcanum of Dominance", GOSSIP_SENDER_MAIN, 126);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Arcanum of The Savage Gladiator", GOSSIP_SENDER_MAIN, 127);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Arcanum of The Stalwart Protector", GOSSIP_SENDER_MAIN, 128);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Arcanum of Torment", GOSSIP_SENDER_MAIN, 129);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Arcanum of Triumph", GOSSIP_SENDER_MAIN, 130);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Arcanum of Eclipsed Moon", GOSSIP_SENDER_MAIN, 131);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Arcanum of the Flame's Soul", GOSSIP_SENDER_MAIN, 132);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Arcanum of the Fleeing Shadow", GOSSIP_SENDER_MAIN, 133);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Arcanum of the Frosty Soul", GOSSIP_SENDER_MAIN, 134);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Arcanum of Toxic Warding", GOSSIP_SENDER_MAIN, 135);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "<-Back", GOSSIP_SENDER_MAIN, 300);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
			return true;
			break;
		case 5: // Enchant Shoulders
			if (player->HasSkill(SKILL_INSCRIPTION) && player->GetSkillValue(SKILL_INSCRIPTION) == 450)
			{
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Master's Inscription of the Axe", GOSSIP_SENDER_MAIN, 136);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Master's Inscription of the Crag", GOSSIP_SENDER_MAIN, 137);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Master's Inscription of the Pinnacle", GOSSIP_SENDER_MAIN, 138);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Master's Inscription of the Storm", GOSSIP_SENDER_MAIN, 139);
			}

			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Greater Inscription of the Axe", GOSSIP_SENDER_MAIN, 140);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Greater Inscription of the Crag", GOSSIP_SENDER_MAIN, 141);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Greater Inscription of the Pinnacle", GOSSIP_SENDER_MAIN, 142);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Greater Inscription of the Gladiator", GOSSIP_SENDER_MAIN, 143);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Greater Inscription of the Storm", GOSSIP_SENDER_MAIN, 144);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Inscription of Dominance", GOSSIP_SENDER_MAIN, 145);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Inscription of Triumph", GOSSIP_SENDER_MAIN, 146);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "<-Back", GOSSIP_SENDER_MAIN, 300);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
			return true;
			break;
		case 6: // Enchant Cloak
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Springy Arachnoweave", GOSSIP_SENDER_MAIN, 147);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Shadow Armor", GOSSIP_SENDER_MAIN, 148);                    
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Darkglow Embroidery", GOSSIP_SENDER_MAIN, 149);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Lightweave Embroidery", GOSSIP_SENDER_MAIN, 150);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Swordguard Embroidery", GOSSIP_SENDER_MAIN, 151);                    
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Wisdom", GOSSIP_SENDER_MAIN, 152);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Titanweave", GOSSIP_SENDER_MAIN, 153);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Spell Piercing", GOSSIP_SENDER_MAIN, 154);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Mighty Armor", GOSSIP_SENDER_MAIN, 155);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Major Agility", GOSSIP_SENDER_MAIN, 156);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Greater Speed", GOSSIP_SENDER_MAIN, 157);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "<-Back", GOSSIP_SENDER_MAIN, 300);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
			return true;
			break;
		case 7: //Enchant chest
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Powerful Stats", GOSSIP_SENDER_MAIN, 158);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Super Health", GOSSIP_SENDER_MAIN, 159);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Greater Mana Restoration", GOSSIP_SENDER_MAIN, 160);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Exceptional Resilience", GOSSIP_SENDER_MAIN, 161);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Greater Defense", GOSSIP_SENDER_MAIN, 162);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "<-Back", GOSSIP_SENDER_MAIN, 300);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
			return true;
			break;
		case 8: //Enchant Bracers
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Major Stamina", GOSSIP_SENDER_MAIN, 163);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Superior Spell Power", GOSSIP_SENDER_MAIN, 164);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Greater Assault", GOSSIP_SENDER_MAIN, 165);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Major Spirit", GOSSIP_SENDER_MAIN, 166);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Expertise", GOSSIP_SENDER_MAIN, 167);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Greater Stats", GOSSIP_SENDER_MAIN, 168);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Exceptional Intellect", GOSSIP_SENDER_MAIN, 169);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Fur Lining - Arcane Resist", GOSSIP_SENDER_MAIN, 170);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Fur Lining - Fire Resist", GOSSIP_SENDER_MAIN, 171);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Fur Lining - Frost Resist", GOSSIP_SENDER_MAIN, 172);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Fur Lining - Nature Resist", GOSSIP_SENDER_MAIN, 173);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Fur Lining - Shadow Resist", GOSSIP_SENDER_MAIN, 174);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Fur Lining - Attack power", GOSSIP_SENDER_MAIN, 175);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Fur Lining - Stamina", GOSSIP_SENDER_MAIN, 176);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Fur Lining - Spellpower", GOSSIP_SENDER_MAIN, 177);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "<-Back", GOSSIP_SENDER_MAIN, 300);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
			return true;
			break;
		case 9: //Enchant Gloves
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Greater Blasting", GOSSIP_SENDER_MAIN, 178);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Armsman", GOSSIP_SENDER_MAIN, 179);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Crusher", GOSSIP_SENDER_MAIN, 180);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Agility", GOSSIP_SENDER_MAIN, 181);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Precision", GOSSIP_SENDER_MAIN, 182);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Expertise", GOSSIP_SENDER_MAIN, 183);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Greater Spell Power", GOSSIP_SENDER_MAIN, 500);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "<-Back", GOSSIP_SENDER_MAIN, 300);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
			return true;
			break;
		case 10: //Enchant legs
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Earthen Leg Armor", GOSSIP_SENDER_MAIN, 184);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Frosthide Leg Armor", GOSSIP_SENDER_MAIN, 185);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Icescale Leg Armor", GOSSIP_SENDER_MAIN, 186);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Brilliant Spellthread", GOSSIP_SENDER_MAIN, 187);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Sapphire Spellthread", GOSSIP_SENDER_MAIN, 188);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Dragonscale Leg Armor", GOSSIP_SENDER_MAIN, 189);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Wyrmscale Leg Armor", GOSSIP_SENDER_MAIN, 190);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "<-Back", GOSSIP_SENDER_MAIN, 300);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
			return true;
			break;
		case 11: //Enchant feet
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Greater Assault", GOSSIP_SENDER_MAIN, 191);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Tuskars Vitliaty", GOSSIP_SENDER_MAIN, 192);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Superior Agility", GOSSIP_SENDER_MAIN, 193);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Greater Spirit", GOSSIP_SENDER_MAIN, 194);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Greater Vitality", GOSSIP_SENDER_MAIN, 195);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Icewalker", GOSSIP_SENDER_MAIN, 196);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Greater Fortitude", GOSSIP_SENDER_MAIN, 197);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Nitro Boots", GOSSIP_SENDER_MAIN, 198);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Hand-Mounted Pyro Rocket", GOSSIP_SENDER_MAIN, 199);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Hyperspeed Accedlerators", GOSSIP_SENDER_MAIN, 200);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Reticulated Armor Webbing", GOSSIP_SENDER_MAIN, 201);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "<-Back", GOSSIP_SENDER_MAIN, 300);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
			return true;
			break;
		case 12: //Enchant rings
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Assault", GOSSIP_SENDER_MAIN, 202);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Greater Spell Power", GOSSIP_SENDER_MAIN, 203);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Stamina", GOSSIP_SENDER_MAIN, 204);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "<-Back", GOSSIP_SENDER_MAIN, 300);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
			return true;
			break;
		case 13: //Enchant Off-Hand weapons
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendNotification("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_WEAPON)
			{
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Blade Ward", GOSSIP_SENDER_MAIN, 205);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Blood Draining", GOSSIP_SENDER_MAIN, 219);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Exceptional Agility", GOSSIP_SENDER_MAIN, 206);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Exceptional Spirit", GOSSIP_SENDER_MAIN, 207);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Berserking", GOSSIP_SENDER_MAIN, 208);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Accuracy", GOSSIP_SENDER_MAIN, 209);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Black Magic", GOSSIP_SENDER_MAIN, 210);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Battlemaster", GOSSIP_SENDER_MAIN, 211);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Icebreaker", GOSSIP_SENDER_MAIN, 212);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Lifeward", GOSSIP_SENDER_MAIN, 213);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Titanguard", GOSSIP_SENDER_MAIN, 214);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Superior Potency", GOSSIP_SENDER_MAIN, 215);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Mighty Spellpower", GOSSIP_SENDER_MAIN, 216);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Mongoose", GOSSIP_SENDER_MAIN, 217);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Executioner", GOSSIP_SENDER_MAIN, 218);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "<-Back", GOSSIP_SENDER_MAIN, 300);
				player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
				return true;
			}
			else
			{
				player->GetSession()->SendNotification("Your Off-Hand is not a weapon.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 14: //Remove enchant menu
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Remove enchant on Main-hand", GOSSIP_SENDER_MAIN, 400);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Remove enchant on Off-hand", GOSSIP_SENDER_MAIN, 401);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Remove enchant on Head", GOSSIP_SENDER_MAIN, 402);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Remove enchant on Shoulders", GOSSIP_SENDER_MAIN, 403);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Remove enchant on Cloak", GOSSIP_SENDER_MAIN, 404);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Remove enchant on Chest", GOSSIP_SENDER_MAIN, 405);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Remove enchant on Bracers", GOSSIP_SENDER_MAIN, 406);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Remove enchant on Gloves", GOSSIP_SENDER_MAIN, 407);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Remove enchant on Legs", GOSSIP_SENDER_MAIN, 408);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Remove enchant on Feet", GOSSIP_SENDER_MAIN, 409);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Remove enchant on Rings", GOSSIP_SENDER_MAIN, 409);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_INTERACT_1, "<-Back", GOSSIP_SENDER_MAIN, 300);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
			return true;
			break;
		case 100:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND), ENCHANT_WEP_AGILITY_1H);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 101:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND), ENCHANT_WEP_SPIRIT);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 102:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND), ENCHANT_WEP_BLADE_WARD);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 103:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND), ENCHANT_WEP_BLOOD_DRAINING);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 104:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND), ENCHANT_WEP_BERSERKING);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 105:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND), ENCHANT_WEP_ACCURACY);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 106:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND), ENCHANT_WEP_BLACK_MAGIC);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 107:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND), ENCHANT_WEP_BATTLEMASTER);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 108:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND), ENCHANT_WEP_ICEBREAKER);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 109:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND), ENCHANT_WEP_LIFEWARD);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 110:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND), ENCHANT_WEP_TITANGUARD);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 111:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND), ENCHANT_WEP_POTENCY);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 112:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND), ENCHANT_WEP_MIGHTY_SPELL_POWER);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 113:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND), ENCHANT_2WEP_MONGOOSE);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 114:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND), ENCHANT_WEP_EXECUTIONER);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 115:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a 2H weapon equipped.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_2H_WEAPON)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND), ENCHANT_2WEP_GREATER_SPELL_POWER);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("You don't have a Two-Handed weapon equipped.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 116:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a 2H weapon equipped.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_2H_WEAPON)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND), ENCHANT_2WEP_AGILITY);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("You don't have a Two-Handed weapon equipped.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 117:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a 2H weapon equipped.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_2H_WEAPON)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND), ENCHANT_2WEP_MASSACRE);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("You don't have a Two-Handed weapon equipped.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 118:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a shield equipped.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_SHIELD)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND), ENCHANT_SHIELD_DEFENSE);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("You don't have a shield equipped.");
				player->CLOSE_GOSSIP_MENU();
			}						
		}
		break;
		case 119:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a shield equipped.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_SHIELD)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND), ENCHANT_SHIELD_INTELLECT);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("You don't have a shield equipped.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 120:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a shield equipped.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_SHIELD)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND), ENCHANT_SHIELD_RESILIENCE);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("You don't have a shield equipped.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 121:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a shield equipped.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_SHIELD)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND), ENCHANT_SHIELD_TITANIUM_PLATING);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("You don't have a shield equipped.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 122:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a shield equipped.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_SHIELD)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND), ENCHANT_SHIELD_STAMINA);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("You don't have a shield equipped.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 123:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a shield equipped.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_SHIELD)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND), ENCHANT_SHIELD_TOUGHSHIELD);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("You don't have a shield equipped.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 124:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_BLISSFUL_MENDING);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 125:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_BURNING_MYSTERIES);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 126:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_DOMINANCE);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 127:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_SAVAGE_GLADIATOR);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 128:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_STALWART_PROTECTOR);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 129:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_TORMENT);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 130:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_TRIUMPH);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 131:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_ECLIPSED_MOON);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 132:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_FLAME_SOUL);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 133:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_FLEEING_SHADOW);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 134:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_FROSTY_SOUL);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 135:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD), ENCHANT_HEAD_TOXIC_WARDING);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 136:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_MASTERS_AXE);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 137:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_MASTERS_CRAG);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 138:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_MASTERS_PINNACLE);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 139:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_MASTERS_STORM);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 140:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_GREATER_AXE);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 141:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_GREATER_CRAG);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 142:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_GREATER_GLADIATOR);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 143:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_GREATER_PINNACLE);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 144:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_GREATER_STORM);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 145:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_DOMINANCE);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 146:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS), ENCHANT_SHOULDER_TRIUMPH);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 147:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_SPRINGY_ARACHNOWEAVE);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 148:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_SHADOW_ARMOR);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 149:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_DARKGLOW_EMBROIDERY);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 150:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_LIGHTWEAVE_EMBROIDERY);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 151:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_SWORDGUARD_EMBROIDERY);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 152:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_WISDOM);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 153:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_TITANWEAVE);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 154:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_SPELL_PIERCING);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 155:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_MIGHTY_ARMOR);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 156:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_MAJOR_AGILITY);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 157:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_GREATER_SPEED);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 158:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_POWERFUL_STATS);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 159:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_SUPER_HEALTH);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 160:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_GREATER_MAINA_REST);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 161:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_EXCEPTIONAL_RESIL);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 162:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_GREATER_DEFENSE);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 163:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_MAJOR_STAMINA);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 164:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_SUPERIOR_SP);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 165:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_GREATER_ASSUALT);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 166:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_MAJOR_SPIRT);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 167:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_EXPERTISE);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 168:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_GREATER_STATS);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 169:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_INTELLECT);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 170:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_ARCANE);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 171:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_FIRE);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 172:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_FROST);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 173:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_NATURE);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 174:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_SHADOW);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 175:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_ATTACK);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 176:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_STAMINA);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 177:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_BRACERS_FURL_SPELLPOWER);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 178:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_GREATER_BLASTING);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 179:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_ARMSMAN);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 180:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_CRUSHER);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 181:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_AGILITY);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 182:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_PRECISION);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 183:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_EXPERTISE);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 500:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_GREATER_SP);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 184:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_LEG_EARTHEN);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 185:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_LEG_FROSTHIDE);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 186:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_LEG_ICESCALE);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 187:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_LEG_BRILLIANT_SPELLTHREAD);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 188:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_LEG_SAPPHIRE_SPELLTHREAD);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 189:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_LEG_DRAGONSCALE);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 190:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS), ENCHANT_LEG_WYRMSCALE);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 191:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_GREATER_ASSAULT);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 192:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_TUSKARS_VITLIATY);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 193:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_SUPERIOR_AGILITY);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 194:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_GREATER_SPIRIT);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 195:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_GREATER_VITALITY);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 196:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_ICEWALKER);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 197:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_GREATER_FORTITUDE);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 198:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_NITRO_BOOTS);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 199:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_PYRO_ROCKET);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 200:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_HYPERSPEED);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 201:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_ARMOR_WEBBING);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 202:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER_1), ENCHANT_RING_ASSAULT);
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER_2), ENCHANT_RING_ASSAULT);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 203:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER_1), ENCHANT_RING_GREATER_SP);
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER_2), ENCHANT_RING_GREATER_SP);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 204:
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER_1), ENCHANT_RING_STAMINA);
			Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER_2), ENCHANT_RING_STAMINA);
			player->CLOSE_GOSSIP_MENU();
			break;
		case 205:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_WEAPON)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND), ENCHANT_WEP_BLADE_WARD);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 206:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_WEAPON)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND), ENCHANT_WEP_AGILITY_1H);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 207:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_WEAPON)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND), ENCHANT_WEP_SPIRIT);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 208:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_WEAPON)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND), ENCHANT_WEP_BERSERKING);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 209:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_WEAPON)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND), ENCHANT_WEP_ACCURACY);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 210:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_WEAPON)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND), ENCHANT_WEP_BLACK_MAGIC);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 211:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_WEAPON)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND), ENCHANT_WEP_BATTLEMASTER);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 212:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_WEAPON)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND), ENCHANT_WEP_ICEBREAKER);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 213:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_WEAPON)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND), ENCHANT_WEP_LIFEWARD);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 214:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_WEAPON)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND), ENCHANT_WEP_TITANGUARD);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 215:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_WEAPON)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND), ENCHANT_WEP_POTENCY);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 216:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_WEAPON)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND), ENCHANT_WEP_MIGHTY_SPELL_POWER);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 217:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_WEAPON)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND), ENCHANT_2WEP_MONGOOSE);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 218:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_WEAPON)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND), ENCHANT_WEP_EXECUTIONER);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 219:
		{
			item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND);

			if (!item)
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
				return false;
			}

			if (item->GetTemplate()->InventoryType == INVENTORY_TYPE_WEAPON)
			{
				Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND), ENCHANT_WEP_BLOOD_DRAINING);
				player->CLOSE_GOSSIP_MENU();
			}
			else
			{
				player->GetSession()->SendAreaTriggerMessage("This enchant needs a one-hand weapon equipped in the off-hand.");
				player->CLOSE_GOSSIP_MENU();
			}
		}
		break;
		case 300: //<-Back menu
		{
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Weapon", GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Off-Hand Weapon", GOSSIP_SENDER_MAIN, 13);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant 2H Weapon", GOSSIP_SENDER_MAIN, 2);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Shield", GOSSIP_SENDER_MAIN, 3);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Head", GOSSIP_SENDER_MAIN, 4);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Shoulders", GOSSIP_SENDER_MAIN, 5);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Cloak", GOSSIP_SENDER_MAIN, 6);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Chest", GOSSIP_SENDER_MAIN, 7);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Bracers", GOSSIP_SENDER_MAIN, 8);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Gloves", GOSSIP_SENDER_MAIN, 9);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Legs", GOSSIP_SENDER_MAIN, 10);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Feet", GOSSIP_SENDER_MAIN, 11);

			if (player->HasSkill(SKILL_ENCHANTING) && player->GetSkillValue(SKILL_ENCHANTING) == 450)
			{
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "Enchant Rings", GOSSIP_SENDER_MAIN, 12);
			}

			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TALK, "I wish to remove my enchant", GOSSIP_SENDER_MAIN, 14);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
			return true;
		}
		break;
		case 400: //Remove enchant for mainhand
			RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAIN_HAND));
			player->CLOSE_GOSSIP_MENU();
			break;
		case 401: //Remove enchant for offhand
			RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFF_HAND));
			player->CLOSE_GOSSIP_MENU();
			break;
		case 402: //Remove enchant for head
			RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD));
			player->CLOSE_GOSSIP_MENU();
			break;
		case 403: //Remove enchant for shoulders
			RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS));
			player->CLOSE_GOSSIP_MENU();
			break;
		case 404: //remove enchant for cloak
			RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK));
			player->CLOSE_GOSSIP_MENU();
			break;
		case 405: //remove enchant for chest
			RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST));
			player->CLOSE_GOSSIP_MENU();
			break;
		case 406: //remove enchant for bracers
			RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS));
			player->CLOSE_GOSSIP_MENU();
			break;
		case 407: //remove enchant for gloves
			RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS));
			player->CLOSE_GOSSIP_MENU();
			break;
		case 408: //remove enchant for legs
			RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS));
			player->CLOSE_GOSSIP_MENU();
			break;
		case 409: //remove enchant for feet
			RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET));
			player->CLOSE_GOSSIP_MENU();
			break;
		case 410:
			RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER_1));
			RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER_2));
			player->CLOSE_GOSSIP_MENU();
			break;
         }
		 return true;
	 }
};

void AddSC_npc_enchancement_master()
{
  new npc_enchancement_master();
}