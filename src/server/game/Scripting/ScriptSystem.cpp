/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptPCH.h"
#include "ScriptSystem.h"
#include "ObjectMgr.h"
#include "DatabaseEnv.h"

ScriptPointVector const SystemMgr::_empty;

void SystemMgr::LoadVersion()
{
	// Get Version information
	QueryResult result = WorldDatabase.Query("SELECT script_version FROM version LIMIT 1");

	if (result)
	{
		Field* fields = result->Fetch();

		sLog->OutConsole("QUANTUMCORE SCRIPTS: Database version is: %s", fields[0].GetCString());
		sLog->OutConsole();
	}
	else
	{
		sLog->OutErrorConsole("QUANTUMCORE SCRIPTS: Missing `version`.`script_version` information.");
		sLog->OutConsole();
	}
}

void SystemMgr::LoadQuantumCoreTexts()
{
	sLog->OutConsole("QUANTUMCORE SCRIPTS: Loading Quantum Core Texts...");
    LoadQuantumSystemTexts("quantum_core_texts", TEXT_SOURCE_RANGE, 1+(TEXT_SOURCE_RANGE*2));

    sLog->OutConsole("QUANTUMCORE SCRIPTS: Loading Quantum Core Texts additional data...");
    uint32 oldMSTime = getMSTime();

	//                                                 0      1      2      3
    QueryResult result = WorldDatabase.Query("SELECT text_id, sound_id, text_type, language_type, emote_id FROM quantum_core_texts");

    if (!result)
    {
		sLog->OutConsole("=======================================================================================");
        sLog->OutConsole(">> Loaded 0 additional Quantum Core Texts data. DB table `quantum_core_texts` is empty.");
		sLog->OutConsole("=======================================================================================");
        sLog->OutConsole();
        return;
    }

    uint32 count = 0;

    do
    {
		Field* pFields = result->Fetch();
        StringTextData temp;

        int32 iId       = pFields[0].GetInt32();
        temp.uiSoundId  = pFields[1].GetUInt32();
		temp.uiType     = pFields[2].GetUInt8();
		temp.uiLanguage = pFields[3].GetUInt8();
		temp.uiEmote    = pFields[4].GetUInt16();

        if (iId >= 0)
        {
            sLog->OutErrorDatabase("QUANTUMCORE SCRIPTS: text_id %i in table `quantum_core_texts` is not a negative value.", iId);
            continue;
        }

        if (iId > TEXT_SOURCE_RANGE || iId <= TEXT_SOURCE_RANGE*2)
        {
            sLog->OutErrorDatabase("QUANTUMCORE SCRIPTS: text_id %i in table `quantum_core_texts` is out of accepted entry range for table.", iId);
            continue;
        }

        if (temp.uiSoundId)
        {
            if (!sSoundEntriesStore.LookupEntry(temp.uiSoundId))
                sLog->OutErrorDatabase("QUANTUMCORE SCRIPTS: text_id %i in table `quantum_core_texts` has soundId %u but sound does not exist.", iId, temp.uiSoundId);
        }

        if (!GetLanguageDescByID(temp.uiLanguage))
			sLog->OutErrorDatabase("QUANTUMCORE SCRIPTS: text_id %i in table `quantum_core_texts` using Language %u but Language does not exist.", iId, temp.uiLanguage);

        if (temp.uiType > CHAT_TYPE_ZONE_YELL)
            sLog->OutErrorDatabase("QUANTUMCORE SCRIPTS: text_id %i in table `quantum_core_texts` has Type %u but this Chat Type does not exist.", iId, temp.uiType);

        m_mTextDataMap[iId] = temp;
        ++count;
    } 
	while (result->NextRow());

	sLog->OutConsole("==========================================================");
    sLog->OutConsole(">> Loaded %u additional Quantum Core Texts data in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
	sLog->OutConsole("==========================================================");
    sLog->OutConsole();
}

void SystemMgr::LoadScriptWaypoints()
{
    uint32 oldMSTime = getMSTime();

    // Drop Existing Waypoint list
    m_mPointMoveMap.clear();

    uint64 uiCreatureCount = 0;

    // Load Waypoints
    QueryResult result = WorldDatabase.Query("SELECT COUNT(entry) FROM script_waypoint GROUP BY entry");
    if (result)
        uiCreatureCount = result->GetRowCount();

    sLog->OutConsole("QUANTUMCORE SCRIPTS: Loading Script Waypoints for " UI64FMTD " creature(s)...", uiCreatureCount);

	//                                     0       1         2           3           4           5
    result = WorldDatabase.Query("SELECT entry, pointid, location_x, location_y, location_z, waittime FROM script_waypoint ORDER BY pointid");

    if (!result)
    {
		sLog->OutConsole("==================================================================");
        sLog->OutConsole(">> Loaded 0 Script Waypoints. DB table `script_waypoint` is empty.");
		sLog->OutConsole("==================================================================");
        sLog->OutConsole();
        return;
    }

    uint32 count = 0;

    do
    {
        Field* pFields = result->Fetch();
        ScriptPointMove temp;

        temp.uiCreatureEntry = pFields[0].GetUInt32();
        uint32 uiEntry       = temp.uiCreatureEntry;
        temp.uiPointId       = pFields[1].GetUInt32();
        temp.fX              = pFields[2].GetFloat();
        temp.fY              = pFields[3].GetFloat();
        temp.fZ              = pFields[4].GetFloat();
        temp.uiWaitTime      = pFields[5].GetUInt32();

        CreatureTemplate const* pCInfo = sObjectMgr->GetCreatureTemplate(temp.uiCreatureEntry);

        if (!pCInfo)
        {
            sLog->OutErrorDatabase("QUANTUMCORE SCRIPTS: DB table script_waypoint has waypoint for non-existant creature entry %u", temp.uiCreatureEntry);
            continue;
        }

        if (!pCInfo->ScriptID)
            sLog->OutErrorDatabase("QUANTUMCORE SCRIPTS: DB table script_waypoint has waypoint for creature entry %u, but creature does not have ScriptName defined and then useless.", temp.uiCreatureEntry);

        m_mPointMoveMap[uiEntry].push_back(temp);
        ++count;
    }
	while (result->NextRow());

	sLog->OutConsole("=============================================");
    sLog->OutConsole(">> Loaded %u Script Waypoint nodes in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
	sLog->OutConsole("=============================================");
    sLog->OutConsole();
}