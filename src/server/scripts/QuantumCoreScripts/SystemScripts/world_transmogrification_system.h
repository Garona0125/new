/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#ifndef DEF_TRANSMOGRIFICATION_H
#define DEF_TRANSMOGRIFICATION_H

#include "Config.h"
#include "QuantumSystemText.h"

class Transmogrification
{
public:
    Transmogrification() {};
    ~Transmogrification() {};

    uint32 GetRequireGold() { return RequireGold; }
    float GetGoldModifier() { return GoldModifier; }
    uint32 GetGoldCost() { return GoldCost; }

    bool GetRequireToken() { return RequireToken; }
    uint32 GetTokenEntry() { return TokenEntry; }
    uint32 GetTokenAmount() { return TokenAmount; }

	uint32 RequireGold;
    uint32 GoldCost;
    uint32 TokenEntry;
    uint32 TokenAmount;
	float GoldModifier;

	bool RequireToken;
    bool AllowPoor;
    bool AllowCommon;
    bool AllowUncommon;
    bool AllowRare;
    bool AllowEpic;
    bool AllowLegendary;
    bool AllowArtifact;
    bool AllowHeirloom;

    bool AllowedQuality(uint32 quality) // Only thing used elsewhere (Player.cpp)
    {
        switch(quality)
        {
            case ITEM_QUALITY_POOR:
				return AllowPoor;
			case ITEM_QUALITY_NORMAL:
				return AllowCommon;
			case ITEM_QUALITY_UNCOMMON:
				return AllowUncommon;
			case ITEM_QUALITY_RARE:
				return AllowRare;
			case ITEM_QUALITY_EPIC:
				return AllowEpic;
			case ITEM_QUALITY_LEGENDARY:
				return AllowLegendary;
			case ITEM_QUALITY_ARTIFACT:
				return AllowArtifact;
			case ITEM_QUALITY_HEIRLOOM:
				return AllowHeirloom;
			default:
				return false;
        }
    }

    void LoadConfig()
    {
        RequireGold = (uint32)GetConfigSettings::GetIntState("Transmogrification.Require.Gold", 1);
        GoldModifier = GetConfigSettings::GetFloatDefault("Transmogrification.Gold.Modifier", 1.0f);
        GoldCost = (uint32)GetConfigSettings::GetIntState("Transmogrification.Gold.Cost", 100000);

        RequireToken = GetConfigSettings::GetBoolState("Transmogrification.Require.Token", false);
        TokenEntry = (uint32)GetConfigSettings::GetIntState("Transmogrification.Token.ID", 49426);
        TokenAmount = (uint32)GetConfigSettings::GetIntState("Transmogrification.Token.Amount", 1);

        AllowPoor = GetConfigSettings::GetBoolState("Transmogrification.Allow.Poor", false);
        AllowCommon = GetConfigSettings::GetBoolState("Transmogrification.Allow.Common", false);
        AllowUncommon = GetConfigSettings::GetBoolState("Transmogrification.Allow.Uncommon", true);
        AllowRare = GetConfigSettings::GetBoolState("Transmogrification.Allow.Rare", true);
        AllowEpic = GetConfigSettings::GetBoolState("Transmogrification.Allow.Epic", true);
        AllowLegendary = GetConfigSettings::GetBoolState("Transmogrification.Allow.Legendary", false);
        AllowArtifact = GetConfigSettings::GetBoolState("Transmogrification.Allow.Artifact", false);
        AllowHeirloom = GetConfigSettings::GetBoolState("Transmogrification.Allow.Heirloom", true);

        if (!sObjectMgr->GetItemTemplate(TokenEntry))
        {
            sLog->OutConsole("Transmogrification.TokenEntry (%u) does not exist. Using default.", TokenEntry);
            TokenEntry = 49426;
        }
    }
};

#define sTransmogrification ACE_Singleton<Transmogrification, ACE_Null_Mutex>::instance()

#endif