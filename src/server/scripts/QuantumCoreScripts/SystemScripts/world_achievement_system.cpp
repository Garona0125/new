/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "AchievementMgr.h"
#include "Config.h"

enum Titles
{
	// Aliance
	TITLE_ID_PRIVATE              = 1,
	TITLE_ID_CORPORAL             = 2,
	TITLE_ID_SERGEANT_A           = 3,
	TITLE_ID_MASTER_SERGEANT      = 4,
	TITLE_ID_SERGEANT_MAJOR       = 5,
	TITLE_ID_KNIGHT               = 6,
	TITLE_ID_KNIGHT_LIEUTENANT    = 7,
	TITLE_ID_KNIGHT_CAPTAIN       = 8,
	TITLE_ID_KNIGHT_CHAMPION      = 9,
	TITLE_ID_LIEUTENANT_COMMANDER = 10,
	TITLE_ID_COMMANDER            = 11,
	TITLE_ID_MARSHAL              = 12,
	TITLE_ID_FIELD_MARSHAL        = 13,
	TITLE_ID_GRAND_MARSHAL        = 14,
	// Horde
	TITLE_ID_SCOUT                = 15,
	TITLE_ID_GRUNT                = 16,
	TITLE_ID_SERGEANT_H           = 17,
	TITLE_ID_SENIOR_SERGEANT      = 18,
	TITLE_ID_FIRST_SERGEANT       = 19,
	TITLE_ID_STONE_GUARD          = 20,
	TITLE_ID_BLOOD_GUARD          = 21,
	TITLE_ID_LEGIONNAIRE          = 22,
	TITLE_ID_CENTURION            = 23,
	TITLE_ID_CHAMPION             = 24,
	TITLE_ID_LIEUTENANT_GENERAL   = 25,
	TITLE_ID_GENERAL              = 26,
	TITLE_ID_WARLORD              = 27,
	TITLE_ID_HIGH_WARLORD         = 28,
};

enum Achievement
{
	// Aliance
	ACHIEVEMENT_PRIVATE              = 442,
	ACHIEVEMENT_CORPORAL             = 470,
	ACHIEVEMENT_SERGEANT_A           = 471,
	ACHIEVEMENT_MASTER_SERGEANT      = 441,
	ACHIEVEMENT_SERGEANT_MAJOR       = 440,
	ACHIEVEMENT_KNIGHT               = 439,
	ACHIEVEMENT_KNIGHT_LIEUTENANT    = 472,
	ACHIEVEMENT_KNIGHT_CAPTAIN       = 438,
	ACHIEVEMENT_KNIGHT_CHAMPION      = 437,
	ACHIEVEMENT_LIEUTENANT_COMMANDER = 436,
	ACHIEVEMENT_COMMANDER            = 435,
	ACHIEVEMENT_MARSHAL              = 473,
	ACHIEVEMENT_FIELD_MARSHAL        = 434,
	ACHIEVEMENT_GRAND_MARSHAL        = 433,
	// Horde
	ACHIEVEMENT_SCOUT                = 454,
	ACHIEVEMENT_GRUNT                = 468,
	ACHIEVEMENT_SERGEANT_H           = 453,
	ACHIEVEMENT_SENIOR_SERGEANT      = 450,
	ACHIEVEMENT_FIRST_SERGEANT       = 452,
	ACHIEVEMENT_STONE_GUARD          = 451,
	ACHIEVEMENT_BLOOD_GUARD          = 449,
	ACHIEVEMENT_LEGIONNAIRE          = 469,
	ACHIEVEMENT_CENTURION            = 448,
	ACHIEVEMENT_CHAMPION             = 447,
	ACHIEVEMENT_LIEUTENANT_GENERAL   = 444,
	ACHIEVEMENT_GENERAL              = 446,
	ACHIEVEMENT_WARLORD              = 445,
	ACHIEVEMENT_HIGH_WARLORD         = 443,
	// Proof of Demise
	ACHIEVEMENT_PROOF_OF_DEMISE      = 2019,
};

enum Quests
{
	QUEST_INGVAR_THE_PLUNDERER  = 13245,
	QUEST_KERISTRAZSA           = 13246,
	QUEST_LEY_GUARDIAN_EREGOS   = 13247,
	QUEST_KING_YMIRON           = 13248,
	QUEST_THE_PROPHET_THARON_JA = 13249,
	QUEST_GAL_DARAH             = 13250,
	QUEST_MAL_GANIS             = 13251,
	QUEST_SLONNIR_IRONSHAPER    = 13252,
	QUEST_LOKEN                 = 13253,
	QUEST_ANUBARAK              = 13254,
	QUEST_HERALD_VOLAZJ         = 13255,
	QUEST_CYANIGOSA             = 13256,
};

class world_pvp_achievement_system : public PlayerScript
{
public:
	world_pvp_achievement_system() : PlayerScript("world_pvp_achievement_system") {}

	uint32 CheckAchievementTimer;

	void OnUpdateTimer(Player* player, uint32 diff)
	{
		if (CheckAchievementTimer <= diff && sWorld->getBoolConfig(CONFIG_PVP_RANK_ACHIEVEMENTS_ENABLE))
		{
			// Private Achievement
			if (player->HasTitle(TITLE_ID_PRIVATE) && (!player->HasAchieved(ACHIEVEMENT_PRIVATE)))
			{
				AchievementEntry const* Private = sAchievementStore.LookupEntry(ACHIEVEMENT_PRIVATE);
				player->CompletedAchievement(Private);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Private] earned by %s", player->GetName());
			}

			// Scout Achievement
			if (player->HasTitle(TITLE_ID_SCOUT) && (!player->HasAchieved(ACHIEVEMENT_SCOUT)))
			{
				AchievementEntry const* Scout = sAchievementStore.LookupEntry(ACHIEVEMENT_SCOUT);
				player->CompletedAchievement(Scout);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Scout] earned by %s", player->GetName());
			}

			// Corporal Achievement
			if (player->HasTitle(TITLE_ID_CORPORAL) && (!player->HasAchieved(ACHIEVEMENT_CORPORAL)))
			{
				AchievementEntry const* Corporal = sAchievementStore.LookupEntry(ACHIEVEMENT_CORPORAL);
				player->CompletedAchievement(Corporal);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Corporal] earned by %s", player->GetName());
			}

			// Grunt Achievement
			if (player->HasTitle(TITLE_ID_GRUNT) && (!player->HasAchieved(ACHIEVEMENT_GRUNT)))
			{
				AchievementEntry const* Grunt = sAchievementStore.LookupEntry(ACHIEVEMENT_GRUNT);
				player->CompletedAchievement(Grunt);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Grunt] earned by %s", player->GetName());
			}

			// Sergeant Achievement (Alliance)
			if (player->HasTitle(TITLE_ID_SERGEANT_A) && (!player->HasAchieved(ACHIEVEMENT_SERGEANT_A)))
			{
				AchievementEntry const* SergeantA = sAchievementStore.LookupEntry(ACHIEVEMENT_SERGEANT_A);
				player->CompletedAchievement(SergeantA);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Sergeant (Alliance)] earned by %s", player->GetName());
			}

			// Sergeant Achievement (Horde)
			if (player->HasTitle(TITLE_ID_SERGEANT_H) && (!player->HasAchieved(ACHIEVEMENT_SERGEANT_H)))
			{
				AchievementEntry const* SergeantH = sAchievementStore.LookupEntry(ACHIEVEMENT_SERGEANT_H);
				player->CompletedAchievement(SergeantH);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Sergeant (Horde)] earned by %s", player->GetName());
			}

			// Master Sergeant
			if (player->HasTitle(TITLE_ID_MASTER_SERGEANT) && (!player->HasAchieved(ACHIEVEMENT_MASTER_SERGEANT)))
			{
				AchievementEntry const* MasterSergeant = sAchievementStore.LookupEntry(ACHIEVEMENT_MASTER_SERGEANT);
				player->CompletedAchievement(MasterSergeant);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Master Sergeant] earned by %s", player->GetName());
			}

			// Senior Sergeant
			if (player->HasTitle(TITLE_ID_SENIOR_SERGEANT) && (!player->HasAchieved(ACHIEVEMENT_SENIOR_SERGEANT)))
			{
				AchievementEntry const* SeniorSergeant = sAchievementStore.LookupEntry(ACHIEVEMENT_SENIOR_SERGEANT);
				player->CompletedAchievement(SeniorSergeant);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Senior Sergeant] earned by %s", player->GetName());
			}

			// Sergeant Major
			if (player->HasTitle(TITLE_ID_SERGEANT_MAJOR) && (!player->HasAchieved(ACHIEVEMENT_SERGEANT_MAJOR)))
			{
				AchievementEntry const* SergeantMajor = sAchievementStore.LookupEntry(ACHIEVEMENT_SERGEANT_MAJOR);
				player->CompletedAchievement(SergeantMajor);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Sergeant Major] earned by %s", player->GetName());
			}

			// First Sergeant
			if (player->HasTitle(TITLE_ID_FIRST_SERGEANT) && (!player->HasAchieved(ACHIEVEMENT_FIRST_SERGEANT)))
			{
				AchievementEntry const* FirstSergeant = sAchievementStore.LookupEntry(ACHIEVEMENT_FIRST_SERGEANT);
				player->CompletedAchievement(FirstSergeant);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: First Sergeant] earned by %s", player->GetName());
			}

			// Knight
			if (player->HasTitle(TITLE_ID_KNIGHT) && (!player->HasAchieved(ACHIEVEMENT_KNIGHT)))
			{
				AchievementEntry const* Knight = sAchievementStore.LookupEntry(ACHIEVEMENT_KNIGHT);
				player->CompletedAchievement(Knight);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Knight] earned by %s", player->GetName());
			}

			// Stone Guard
			if (player->HasTitle(TITLE_ID_STONE_GUARD) && (!player->HasAchieved(ACHIEVEMENT_STONE_GUARD)))
			{
				AchievementEntry const* StoneGuard = sAchievementStore.LookupEntry(ACHIEVEMENT_STONE_GUARD);
				player->CompletedAchievement(StoneGuard);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Stone Guard] earned by %s", player->GetName());
			}

			// Knight Lieutenant
			if (player->HasTitle(TITLE_ID_KNIGHT_LIEUTENANT) && (!player->HasAchieved(ACHIEVEMENT_KNIGHT_LIEUTENANT)))
			{
				AchievementEntry const* KnightLieutenant = sAchievementStore.LookupEntry(ACHIEVEMENT_KNIGHT_LIEUTENANT);
				player->CompletedAchievement(KnightLieutenant);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Knight Lieutenant] earned by %s", player->GetName());
			}

			// Blood Guard
			if (player->HasTitle(TITLE_ID_BLOOD_GUARD) && (!player->HasAchieved(ACHIEVEMENT_BLOOD_GUARD)))
			{
				AchievementEntry const* BloodGuard = sAchievementStore.LookupEntry(ACHIEVEMENT_BLOOD_GUARD);
				player->CompletedAchievement(BloodGuard);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Blood Guard] earned by %s", player->GetName());
			}

			// Knight Captain
			if (player->HasTitle(TITLE_ID_KNIGHT_CAPTAIN) && (!player->HasAchieved(ACHIEVEMENT_KNIGHT_CAPTAIN)))
			{
				AchievementEntry const* KnightCaptain = sAchievementStore.LookupEntry(ACHIEVEMENT_KNIGHT_CAPTAIN);
				player->CompletedAchievement(KnightCaptain);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Knight Captain] earned by %s", player->GetName());
			}

			// Legionnaire
			if (player->HasTitle(TITLE_ID_LEGIONNAIRE) && (!player->HasAchieved(ACHIEVEMENT_LEGIONNAIRE)))
			{
				AchievementEntry const* Legionnaire = sAchievementStore.LookupEntry(ACHIEVEMENT_LEGIONNAIRE);
				player->CompletedAchievement(Legionnaire);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Legionnaire] earned by %s", player->GetName());
			}

			// Knight Champion
			if (player->HasTitle(TITLE_ID_KNIGHT_CHAMPION) && (!player->HasAchieved(ACHIEVEMENT_KNIGHT_CHAMPION)))
			{
				AchievementEntry const* KnightChampion = sAchievementStore.LookupEntry(ACHIEVEMENT_KNIGHT_CHAMPION);
				player->CompletedAchievement(KnightChampion);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Knight Champion] earned by %s", player->GetName());
			}

			// Centurion
			if (player->HasTitle(TITLE_ID_CENTURION) && (!player->HasAchieved(ACHIEVEMENT_CENTURION)))
			{
				AchievementEntry const* Centurion = sAchievementStore.LookupEntry(ACHIEVEMENT_CENTURION);
				player->CompletedAchievement(Centurion);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Centurion] earned by %s", player->GetName());
			}

			// Lieutenant Commander
			if (player->HasTitle(TITLE_ID_LIEUTENANT_COMMANDER) && (!player->HasAchieved(ACHIEVEMENT_LIEUTENANT_COMMANDER)))
			{
				AchievementEntry const* LieutenantCommander = sAchievementStore.LookupEntry(ACHIEVEMENT_LIEUTENANT_COMMANDER);
				player->CompletedAchievement(LieutenantCommander);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Lieutenant Commander] earned by %s", player->GetName());
			}

			// Champion
			if (player->HasTitle(TITLE_ID_CHAMPION) && (!player->HasAchieved(ACHIEVEMENT_CHAMPION)))
			{
				AchievementEntry const* Champion = sAchievementStore.LookupEntry(ACHIEVEMENT_CHAMPION);
				player->CompletedAchievement(Champion);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Champion] earned by %s", player->GetName());
			}

			// Commander
			if (player->HasTitle(TITLE_ID_COMMANDER) && (!player->HasAchieved(ACHIEVEMENT_COMMANDER)))
			{
				AchievementEntry const* Commander = sAchievementStore.LookupEntry(ACHIEVEMENT_COMMANDER);
				player->CompletedAchievement(Commander);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Commander] earned by %s", player->GetName());
			}

			// Lieutenant General
			if (player->HasTitle(TITLE_ID_LIEUTENANT_GENERAL) && (!player->HasAchieved(ACHIEVEMENT_LIEUTENANT_GENERAL)))
			{
				AchievementEntry const* LieutenantGeneral = sAchievementStore.LookupEntry(ACHIEVEMENT_LIEUTENANT_GENERAL);
				player->CompletedAchievement(LieutenantGeneral);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Lieutenant General] earned by %s", player->GetName());
			}

			// Marshal
			if (player->HasTitle(TITLE_ID_MARSHAL) && (!player->HasAchieved(ACHIEVEMENT_MARSHAL)))
			{
				AchievementEntry const* Marshal = sAchievementStore.LookupEntry(ACHIEVEMENT_MARSHAL);
				player->CompletedAchievement(Marshal);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Marshal] earned by %s", player->GetName());
			}

			// General
			if (player->HasTitle(TITLE_ID_GENERAL) && (!player->HasAchieved(ACHIEVEMENT_GENERAL)))
			{
				AchievementEntry const* General = sAchievementStore.LookupEntry(ACHIEVEMENT_GENERAL);
				player->CompletedAchievement(General);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: General] earned by %s", player->GetName());
			}

			// Field Marshal
			if (player->HasTitle(TITLE_ID_FIELD_MARSHAL) && (!player->HasAchieved(ACHIEVEMENT_FIELD_MARSHAL)))
			{
				AchievementEntry const* FieldMarshal = sAchievementStore.LookupEntry(ACHIEVEMENT_FIELD_MARSHAL);
				player->CompletedAchievement(FieldMarshal);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Field Marshal] earned by %s", player->GetName());
			}

			// Warlord
			if (player->HasTitle(TITLE_ID_WARLORD) && (!player->HasAchieved(ACHIEVEMENT_WARLORD)))
			{
				AchievementEntry const* Warlord = sAchievementStore.LookupEntry(ACHIEVEMENT_WARLORD);
				player->CompletedAchievement(Warlord);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Warlord] earned by %s", player->GetName());
			}

			// Grand Marshal
			if (player->HasTitle(TITLE_ID_GRAND_MARSHAL) && (!player->HasAchieved(ACHIEVEMENT_GRAND_MARSHAL)))
			{
				AchievementEntry const* GrandMarshal = sAchievementStore.LookupEntry(ACHIEVEMENT_GRAND_MARSHAL);
				player->CompletedAchievement(GrandMarshal);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Grand Marshal] earned by %s", player->GetName());
			}

			// High Warlord
			if (player->HasTitle(TITLE_ID_HIGH_WARLORD) && (!player->HasAchieved(ACHIEVEMENT_HIGH_WARLORD)))
			{
				AchievementEntry const* HighWarlord = sAchievementStore.LookupEntry(ACHIEVEMENT_HIGH_WARLORD);
				player->CompletedAchievement(HighWarlord);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: High Warlord] earned by %s", player->GetName());
			}

			CheckAchievementTimer = 5*IN_MILLISECONDS;
		}
		else CheckAchievementTimer -= diff;
	}
};

class world_proof_of_demise_system : public PlayerScript
{
public:
	world_proof_of_demise_system() : PlayerScript("world_proof_of_demise_system") {}

	uint32 CheckAchievementTimer;

	void OnUpdateTimer(Player* player, uint32 diff)
	{
		if (CheckAchievementTimer <= diff)
		{
			// Private Achievement
			if (!player->HasAchieved(ACHIEVEMENT_PROOF_OF_DEMISE)
				&& player->GetQuestStatus(QUEST_INGVAR_THE_PLUNDERER) == QUEST_STATUS_REWARDED
				&& (QUEST_KERISTRAZSA) == QUEST_STATUS_REWARDED
				&& (QUEST_LEY_GUARDIAN_EREGOS) == QUEST_STATUS_REWARDED
				&& (QUEST_KING_YMIRON) == QUEST_STATUS_REWARDED
				&& (QUEST_THE_PROPHET_THARON_JA) == QUEST_STATUS_REWARDED
				&& (QUEST_GAL_DARAH) == QUEST_STATUS_REWARDED
				&& (QUEST_MAL_GANIS) == QUEST_STATUS_REWARDED
				&& (QUEST_SLONNIR_IRONSHAPER) == QUEST_STATUS_REWARDED
				&& (QUEST_LOKEN) == QUEST_STATUS_REWARDED
				&& (QUEST_ANUBARAK) == QUEST_STATUS_REWARDED
				&& (QUEST_HERALD_VOLAZJ) == QUEST_STATUS_REWARDED
				&& (QUEST_CYANIGOSA) == QUEST_STATUS_REWARDED)
			{
				AchievementEntry const* ProofOfDemise = sAchievementStore.LookupEntry(ACHIEVEMENT_PROOF_OF_DEMISE);
				player->CompletedAchievement(ProofOfDemise);
				sLog->OutConsole("ACHIEVEMENT MANAGER: [Achievement: Proof of Demise] earned by %s", player->GetName());
				std::string sText = ("Player:|CFFFF33FF " + std::string(player->GetName()) + "|r Earned Achievement [Proof of Demise]!");
				sWorld->SendServerMessage(SERVER_MSG_STRING, sText.c_str());
			}

			CheckAchievementTimer = 5*IN_MILLISECONDS;
		}
		else CheckAchievementTimer -= diff;
	}
};

void AddSC_world_achievement_system()
{
	new world_pvp_achievement_system();
	new world_proof_of_demise_system();
}