/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "Channel.h"
#include "Chat.h"
#include "Config.h"
#include <sstream>

const char* CLASS_ICON;
const char* RACE_ICON;

class world_chat_icon_system : public PlayerScript
{
public:
	world_chat_icon_system() : PlayerScript("world_chat_icon_system") {}

	void OnChat(Player* player, uint32 /*type*/, uint32 lang, std::string& msg, Channel* channel)
	{
		if (sWorld->getBoolConfig(CONFIG_COLOR_ICON_CHAT_ENABLE))
		{
			if (!player || !channel)
				return;

			std::string color;
			std::string team;
			std::stringstream ssMsg;

			switch (player->GetCurrentRace())
			{
				// Bloodelf
			    case RACE_BLOOD_ELF:
					if (player->GetCurrentGender() == GENDER_MALE)
						RACE_ICON = "|TInterface/ICONS/Achievement_Character_Bloodelf_Male:18|t";
					else
						RACE_ICON = "|TInterface/ICONS/Achievement_Character_Bloodelf_Female:18|t";
					break;
					// Dranei
				case RACE_DRAENEI:
					if (player->GetCurrentGender() == GENDER_FEMALE)
						RACE_ICON = "|TInterface/ICONS/Achievement_Character_Draenei_Female:18|t";
					else
						RACE_ICON = "|TInterface/ICONS/Achievement_Character_Draenei_Male:18|t";
					break;
				case RACE_DWARF:
					if (player->GetCurrentGender() == GENDER_FEMALE)
						RACE_ICON = "|TInterface/ICONS/Achievement_Character_Dwarf_Female:18|t";
					else
						RACE_ICON = "|TInterface/ICONS/Achievement_Character_Dwarf_Male:18|t";
					break;
					// Gnome
				case RACE_GNOME:
					if (player->GetCurrentGender() == GENDER_FEMALE)
						RACE_ICON = "|TInterface/ICONS/Achievement_Character_Gnome_Female:18|t";
					else
						RACE_ICON = "|TInterface/ICONS/Achievement_Character_Gnome_Male:18|t";
					break;
					// Human
				case RACE_HUMAN:
					if (player->GetCurrentGender() == GENDER_FEMALE)
						RACE_ICON = "|TInterface/ICONS/Achievement_Character_Human_Female:18|t";
					else
						RACE_ICON = "|TInterface/ICONS/Achievement_Character_Human_Male:18|t";
					break;
				case RACE_NIGHT_ELF:
					if (player->GetCurrentGender() == GENDER_FEMALE)
						RACE_ICON = "|TInterface/ICONS/Achievement_Character_Nightelf_Female:18|t";
					else
						RACE_ICON = "|TInterface/ICONS/Achievement_Character_Nightelf_Male:18|t";
					break;
				case RACE_ORC:
					if (player->GetCurrentGender() == GENDER_FEMALE)
						RACE_ICON = "|TInterface/ICONS/Achievement_Character_Orc_Female:18|t";
					else
						RACE_ICON = "|TInterface/ICONS/Achievement_Character_Orc_Male:18|t";
					break;
					// Tauren
				case RACE_TAUREN:
					if (player->GetCurrentGender() == GENDER_FEMALE)
						RACE_ICON = "|TInterface/ICONS/Achievement_Character_Tauren_Female:18|t";
					else
						RACE_ICON = "|TInterface/ICONS/Achievement_Character_Tauren_Male:18|t";
					break;
				case RACE_TROLL:
					if (player->GetCurrentGender() == GENDER_FEMALE)
						RACE_ICON = "|TInterface/ICONS/Achievement_Character_Troll_Female:18|t";
					else
						RACE_ICON = "|TInterface/ICONS/Achievement_Character_Troll_Male:18|t";
					break;
				case RACE_UNDEAD_PLAYER:
					if (player->GetCurrentGender() == GENDER_FEMALE)
						RACE_ICON = "|TInterface/ICONS/Achievement_Character_Undead_Female:18|t";
					else
						RACE_ICON = "|TInterface/ICONS/Achievement_Character_Undead_Male:18|t";
					break;
			}

			switch (player->GetCurrentClass())
			{
			    case CLASS_DEATH_KNIGHT:
					color = "|cffC41F3B";
					//CLASS_ICON = "|TInterface\\icons\\Spell_Deathknight_ClassIcon:18|t|r";
					break;
				case CLASS_DRUID:
					color = "|cffFF7D0A";
					//CLASS_ICON = "|TInterface\\icons\\Ability_Druid_Maul:18|t|r";
					break;
				case CLASS_HUNTER:
					color = "|cffABD473";
					//CLASS_ICON = "|TInterface\\icons\\INV_Weapon_Bow_07:18|t|r";
					break;
				case CLASS_MAGE:
					color = "|cff69CCF0";
					//CLASS_ICON = "|TInterface\\icons\\INV_Staff_13:18|t|r";
					break;
				case CLASS_PALADIN:
					color = "|cffF58CBA";
					//CLASS_ICON = "|TInterface\\icons\\INV_Hammer_01:18|t|r";
					break;
				case CLASS_PRIEST:
					color = "|cffFFFFFF";
					//CLASS_ICON = "|TInterface\\icons\\INV_Staff_30:18|t|r";
					break;
				case CLASS_ROGUE:
					color = "|cffFFF569";
					//CLASS_ICON = "|TInterface\\icons\\INV_ThrowingKnife_04:18|t|r";
					break;
				case CLASS_SHAMAN:
					color = "|cff0070DE";
					//CLASS_ICON = "|TInterface\\icons\\Spell_Nature_BloodLust:18|t|r";
					break;
				case CLASS_WARLOCK:
					color = "|cff9482C9";
					//CLASS_ICON = "|TInterface\\icons\\Spell_Nature_FaerieFire:18|t|r";
					break;
				case CLASS_WARRIOR:
					color = "|cffC79C6E";
					//CLASS_ICON = "|TInterface\\icons\\INV_Sword_27.png:18|t|r";
					break;
			}

			ssMsg << RACE_ICON + color << msg;
			msg = ssMsg.str();
		}
	}
};

void AddSC_world_chat_icon_system()
{
	new world_chat_icon_system();
}