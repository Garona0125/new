/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "QuantumCreature.h"
#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "OutdoorPvPMgr.h"
#include "GroupMgr.h"
#include "Group.h"
#include "SpellAuras.h"
#include "Vehicle.h"
#include "Chat.h"
#include "MapManager.h"
#include "Config.h"
#include "World.h"
#include "OutdoorPvPWG.h"

Creature* FortressSpirit;

uint32 entry;
uint32 guide_entry;
uint32 guide_entry_fortress_horde;
uint32 guide_entry_fortress_alliance;

Map* map;

char const* fmtstring(char const* format, ...)
{
   va_list argptr;
   #define MAX_FMT_STRING 32000
   static char temp_buffer[MAX_FMT_STRING];
   static char string[MAX_FMT_STRING];
   static int index = 0;
   char* buf;
   int len;

   va_start(argptr, format);
   vsnprintf(temp_buffer, MAX_FMT_STRING, format, argptr);
   va_end(argptr);

   len = strlen(temp_buffer);

   if (len >= MAX_FMT_STRING)
       return "ERROR";

   if (len + index >= MAX_FMT_STRING-1)
	   index = 0;

   buf = &string[index];
   memcpy(buf, temp_buffer, len+1);

   index += len + 1;

   return buf;
}

OutdoorPvPWG::OutdoorPvPWG()
{
   m_TypeId = OUTDOOR_PVP_WG;
   LastResurrectTime = 0; // Temporal copy of BG system till 3.2
}

void _LoadTeamPair(TeamPairMap &pairMap, const TeamPair* pair)
{
   while((*pair)[0])
   {
       pairMap[(*pair)[0]] = (*pair)[1];
       pairMap[(*pair)[1]] = (*pair)[0];
       ++pair;
   }
}

void OutdoorPvPWG::ResetCreatureEntry(Creature* creature, uint32 entry)
{
   if (creature)
   {
       creature->UpdateEntry(entry); // SetOriginalEntry as used before may lead to crash
       //if (creature->GetAreaId() == 4575)
           //if (creature->AI())
               //creature->AI()->EnterEvadeMode();
       if (entry != creature->GetEntry() || !creature->IsAlive())
           creature->Respawn(true);
       creature->SetVisible(true);
   }
}

void _RespawnCreatureIfNeeded(Creature* creature, uint32 entry)
{
   if (creature)
   {
       //creature->SetOriginalEntry(entry);
       creature->UpdateEntry(entry); // SetOriginalEntry as used before may lead to crash
       if (entry != creature->GetEntry() || !creature->IsAlive())
           creature->Respawn(true);
       creature->SetVisible(true);
   }
}

bool OutdoorPvPWG::SetupOutdoorPvP()
{
   if (!sWorld->getBoolConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_ENABLED))
   {
       sWorld->setWorldState(WORLDSTATE_WINTERGRASP_CONTROLING_FACTION, TEAM_NEUTRAL);
       return false;
   }

   // load worlstates
   QueryResult result = CharacterDatabase.PQuery("SELECT entry, value FROM worldstates WHERE entry IN (31001, 31002, 31003) ORDER BY entry");
   
   WorldStateSaveTimer = sWorld->getIntConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_SAVE_STATE_PERIOD);

   if (result)
   {
       do
       {
           Field* fields = result->Fetch();
           switch (fields[0].GetUInt32())
           {
               case WORLDSTATE_WINTERGRASP_WARTIME:
                   m_wartime = fields[1].GetBool();
                   break;
               case WORLDSTATE_WINTERGRASP_TIMER:
                   m_timer = fields[1].GetUInt32();
                   break;
               case WORLDSTATE_WINTERGRASP_DEFENDERS:
                   m_defender = TeamId(fields[1].GetUInt32());
                   break;
           }
       }
	   while(result->NextRow());
   }
   else
   {
       m_wartime = false;
       m_timer = sWorld->getIntConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_START_TIME)*MINUTE*IN_MILLISECONDS;
       m_defender = TeamId(rand()%2);
   }

   sWorld->setWorldState(WORLDSTATE_WINTERGRASP_CONTROLING_FACTION, GetDefenderTeam());
   m_changeDefender = false;
   m_workshopCount[TEAM_ALLIANCE] = 0;
   m_workshopCount[TEAM_HORDE] = 0;
   m_tenacityStack = 0;
   m_gate = NULL;
   minlevel = sWorld->getIntConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_MIN_LEVEL);
   m_MaxPlayer = sWorld->getIntConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_MAX_PLAYERS);

   StartGroupingTimer = 15*MINUTE*IN_MILLISECONDS;
   m_StartGrouping = false;

   m_TimeForAcceptInvite = 20;
   KickDontAcceptTimer = 2000;
   KickAfkTimer = 2000;

   std::list<uint32> engGuids;
   std::list<uint32> spiritGuids;

   // Store Eng, spirit guide guids and questgiver for later use
   result = WorldDatabase.PQuery("SELECT guid, id FROM creature WHERE creature.map = 571"
        " AND creature.id IN (%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u);",
        NPC_GNOMISH_ENGINEER_A, NPC_GNOMISH_ENGINEER_H, NPC_SPIRIT_HEALER_A, NPC_SPIRIT_HEALER_H, NPC_HOODOO_MASTER_FUJIN_H, NPC_SORCERESS_KAYLANA_A, 31102, 31052,
        31107, 31109, 31151, 31153, 31106, 31108, 31053, 31054, 31091, 31036);
   if (!result)
       sLog->OutErrorConsole("Cannot find siege workshop master or spirit guides in creature!");
   else
   {
       do
       {
           Position PosHorde, PosAlliance;
           Field* fields = result->Fetch();
           switch (fields[1].GetUInt32())
           {
               case NPC_GNOMISH_ENGINEER_A:
               case NPC_GNOMISH_ENGINEER_H:
                   engGuids.push_back(fields[0].GetUInt32());
                   break;
               case NPC_SPIRIT_HEALER_A:
               case NPC_SPIRIT_HEALER_H:
                   spiritGuids.push_back(fields[0].GetUInt32());
                   break;
               case NPC_SORCERESS_KAYLANA_A:
                   PosHorde.Relocate(5081.7f, 2173.73f, 365.878f, 0.855211f);
                   PosAlliance.Relocate(5296.56f, 2789.87f, 409.275f, 0.733038f);
                   LoadQuestGiverMap(fields[0].GetUInt32(), PosHorde, PosAlliance);
                   break;
               case NPC_HOODOO_MASTER_FUJIN_H:
                   PosHorde.Relocate(5296.56f, 2789.87f, 409.275f, 0.733038f);
                   PosAlliance.Relocate(5016.57f, 3677.53f, 362.982f, 5.7525f);
                   LoadQuestGiverMap(fields[0].GetUInt32(), PosHorde, PosAlliance);
                   break;
               case 31052:
                   PosHorde.Relocate(5100.07f, 2168.89f, 365.779f, 1.97222f);
                   PosAlliance.Relocate(5298.43f, 2738.76f, 409.316f, 3.97174f);
                   LoadQuestGiverMap(fields[0].GetUInt32(), PosHorde, PosAlliance);
                   break;
               case 31102:
                   PosHorde.Relocate(5298.43f, 2738.76f, 409.316f, 3.97174f);
                   PosAlliance.Relocate(5030.44f, 3659.82f, 363.194f, 1.83336f);
                   LoadQuestGiverMap(fields[0].GetUInt32(), PosHorde, PosAlliance);
                   break;
               case 31109:
                   PosHorde.Relocate(5080.4f, 2199.0f, 359.489f, 2.96706f);
                   PosAlliance.Relocate(5234.97f, 2883.4f, 409.275f, 4.29351f);
                   LoadQuestGiverMap(fields[0].GetUInt32(), PosHorde, PosAlliance);
                   break;
               case 31107:
                   PosHorde.Relocate(5234.97f, 2883.4f, 409.275f, 4.29351f);
                   PosAlliance.Relocate(5008.64f, 3659.91f, 361.07f, 4.0796f);
                   LoadQuestGiverMap(fields[0].GetUInt32(), PosHorde, PosAlliance);
                   break;
               case 31153:
                   PosHorde.Relocate(5088.49f, 2188.18f, 365.647f, 5.25344f);
                   PosAlliance.Relocate(5366.13f, 2833.4f, 409.323f, 3.14159f);
                   LoadQuestGiverMap(fields[0].GetUInt32(), PosHorde, PosAlliance);
                   break;
               case 31151:
                   PosHorde.Relocate(5366.13f, 2833.4f, 409.323f, 3.14159f);
                   PosAlliance.Relocate(5032.33f, 3680.7f, 363.018f, 3.43167f);
                   LoadQuestGiverMap(fields[0].GetUInt32(), PosHorde, PosAlliance);
                   break;
               case 31108:
                   PosHorde.Relocate(5095.67f, 2193.28f, 365.924f, 4.93928f);
                   PosAlliance.Relocate(5295.56f, 2926.67f, 409.275f, 0.872665f);
                   LoadQuestGiverMap(fields[0].GetUInt32(), PosHorde, PosAlliance);
                   break;
               case 31106:
                   PosHorde.Relocate(5295.56f, 2926.67f, 409.275f, 0.872665f);
                   PosAlliance.Relocate(5032.66f, 3674.28f, 363.053f, 2.9447f);
                   LoadQuestGiverMap(fields[0].GetUInt32(), PosHorde, PosAlliance);
                   break;
               case 31054:
                   PosHorde.Relocate(5088.61f, 2167.66f, 365.689f, 0.680678f);
                   PosAlliance.Relocate(5371.4f, 3026.51f, 409.206f, 3.25003f);
                   LoadQuestGiverMap(fields[0].GetUInt32(), PosHorde, PosAlliance);
                   break;
               case 31053:
                   PosHorde.Relocate(5371.4f, 3026.51f, 409.206f, 3.25003f);
                   PosAlliance.Relocate(5032.44f, 3668.66f, 363.11f, 2.87402f);
                   LoadQuestGiverMap(fields[0].GetUInt32(), PosHorde, PosAlliance);
                   break;
               case 31036:
                   PosHorde.Relocate(5078.28f, 2183.7f, 365.029f, 1.46608f);
                   PosAlliance.Relocate(5359.13f, 2837.99f, 409.364f, 4.69893f);
                   LoadQuestGiverMap(fields[0].GetUInt32(), PosHorde, PosAlliance);
                   break;
               case 31091:
                   PosHorde.Relocate(5359.13f, 2837.99f, 409.364f, 4.69893f);
                   PosAlliance.Relocate(5022.43f, 3659.91f, 361.61f, 1.35426f);
                   LoadQuestGiverMap(fields[0].GetUInt32(), PosHorde, PosAlliance);
                   break;
               default:
                   break;
           }
       }
	   while(result->NextRow());
   }

   // Select POI
   AreaPOIList areaPOIs;
   float minX = 9999, minY = 9999, maxX = -9999, maxY = -9999;
   for (uint32 i = 0; i < sAreaPOIStore.GetNumRows(); ++i)
   {
       const AreaPOIEntry* poiInfo = sAreaPOIStore.LookupEntry(i);
       if (poiInfo && poiInfo->zoneId == ZONE_WINTERGRASP)
       {
           areaPOIs.push_back(poiInfo);

           if (minX > poiInfo->x)
			   minX = poiInfo->x;

		   if (minY > poiInfo->y)
			   minY = poiInfo->y;

           if (maxX < poiInfo->x)
			   maxX = poiInfo->x;

           if (maxY < poiInfo->y)
			   maxY = poiInfo->y;
       }
   }
   minX -= 20; minY -= 20; maxX += 20; maxY += 20;

   // Coords: 4290.330078, 1790.359985 - 5558.379883, 4048.889893
   result = WorldDatabase.PQuery("SELECT guid FROM gameobject, gameobject_template" " WHERE gameobject.map = 571" " AND gameobject.position_x>%f AND gameobject.position_y>%f"
	   " AND gameobject.position_x<%f AND gameobject.position_y<%f" " AND gameobject_template.type = 33" " AND gameobject.id = gameobject_template.entry", minX, minY, maxX, maxY);
   if (!result)
       return false;

   do
   {
       Field* fields = result->Fetch();

       uint32 guid = fields[0].GetUInt32();
       GameObjectData const* goData = sObjectMgr->GetGOData(guid);
       if (!goData) // this should not happen
           continue;

       float x = goData->posX, y = goData->posY;
       float minDist = 100;
       AreaPOIList::iterator poi = areaPOIs.end();
       for (AreaPOIList::iterator itr = areaPOIs.begin(); itr != areaPOIs.end(); ++itr)
       {
           if (!(*itr)->icon[1]) // note: may for other use
               continue;

           float dist = (abs((*itr)->x - x) + abs((*itr)->y - y));
           if (minDist > dist)
           {
               minDist = dist;
               poi = itr;
           }
       }

       if (poi == areaPOIs.end())
           continue;

       // add building to the list
       TeamId teamId = x > POS_X_CENTER-300 ? GetDefenderTeam() : GetAttackerTeam();
       BuildingStates[guid] = new BuildingState((*poi)->worldState, teamId, GetDefenderTeam() != TEAM_ALLIANCE);
       if ((*poi)->id == 2246)
           m_gate = BuildingStates[guid];
       areaPOIs.erase(poi);

       // add capture point
       uint32 capturePointEntry = 0;

       switch (goData->id)
       {    
           // West Goblin Workshops
           case GO_GOBLIN_WORKSHOP_1: // NW
           case GO_GOBLIN_WORKSHOP_2: // W
           case GO_GOBLIN_WORKSHOP_3: // SW
               capturePointEntry = 190475;
               break;
           // East Goblin Workshops
           case GO_GOBLIN_WORKSHOP_4: // NE
           case GO_GOBLIN_WORKSHOP_5: // E
           case GO_GOBLIN_WORKSHOP_6: // SE
               capturePointEntry = 190487;
               break;
       }

       if (capturePointEntry)
       {
           uint32 engGuid = 0;
           uint32 spiritGuid = 0;
           // Find closest Eng to Workshop
           float minDist = 100;
           for (std::list<uint32>::iterator itr = engGuids.begin(); itr != engGuids.end(); ++itr)
           {
               const CreatureData* creatureData = sObjectMgr->GetCreatureData(*itr);
               if (!creatureData)
                   continue;

               float dist = (abs(creatureData->posX - x) + abs(creatureData->posY - y));
               if (minDist > dist)
               {
                   minDist = dist;
                   engGuid = *itr;
               }
           }

           if (!engGuid)
           {
               sLog->OutErrorConsole("Cannot find nearby siege workshop master!");
               continue;
           }
           else
               engGuids.remove(engGuid);
           // Find closest Spirit Guide to Workshop
           minDist = 255;
           for (std::list<uint32>::iterator itr = spiritGuids.begin(); itr != spiritGuids.end(); ++itr)
           {
               const CreatureData* creatureData = sObjectMgr->GetCreatureData(*itr);
               if (!creatureData)
                   continue;

               float dist = (abs(creatureData->posX - x) + abs(creatureData->posY - y));
               if (minDist > dist)
               {
                   minDist = dist;
                   spiritGuid = *itr;
               }
           }

           // Inside fortress won't be capturable
           OPvPCapturePointWG* workshop = new OPvPCapturePointWG(this, BuildingStates[guid]);
           if (goData->posX < POS_X_CENTER && !workshop->SetCapturePointData(capturePointEntry, goData->mapid, goData->posX + 40* cos(goData->orientation + M_PI / 2), goData->posY + 40* sin(goData->orientation + M_PI / 2), goData->posZ))
           {
               delete workshop;
               sLog->OutErrorConsole("Cannot add capture point!");
               continue;
           }
             // West fortress Workshop fix
           if (goData->id == GO_GOBLIN_WORKSHOP_1 || goData->id == GO_GOBLIN_WORKSHOP_4)
              workshop->m_capturePointGUID = goData->id;

           const CreatureData* creatureData = sObjectMgr->GetCreatureData(engGuid);
           if (!creatureData)
               continue;

           workshop->m_engEntry = const_cast<uint32*>(&creatureData->id);
           const_cast<CreatureData*>(creatureData)->displayid = 0;
           workshop->m_engGuid = engGuid;

           // Back spirit is linked to one of the inside workshops, 1 workshop wont have spirit
           if (spiritGuid)
           {
               spiritGuids.remove(spiritGuid);

               const CreatureData* spiritData = sObjectMgr->GetCreatureData(spiritGuid);
               if (!spiritData)
                   continue;

               workshop->m_spiEntry = const_cast<uint32*>(&spiritData->id);
               const_cast<CreatureData*>(spiritData)->displayid = 0;
               workshop->m_spiGuid = spiritGuid;
           }
           else
               workshop->m_spiGuid = 0;
           workshop->m_workshopGuid = guid;
           AddCapturePoint(workshop);
           BuildingStates[guid]->type = BUILDING_WORKSHOP;
           workshop->SetTeamByBuildingState();
       }
   }
   while(result->NextRow());

   engGuids.clear();
   spiritGuids.clear();

   if (!m_gate)
   {
       sLog->OutErrorConsole("Cannot find wintergrasp fortress gate!");
       return false;
   }

   // Load Graveyard
   GraveYardContainer::const_iterator graveLow = sObjectMgr->GraveYardStore.lower_bound(ZONE_WINTERGRASP);
   GraveYardContainer::const_iterator graveUp = sObjectMgr->GraveYardStore.upper_bound(ZONE_WINTERGRASP);
   for (AreaPOIList::iterator itr = areaPOIs.begin(); itr != areaPOIs.end();)
   {
       if ((*itr)->icon[1] == 8)
       {
           // find or create grave yard
           const WorldSafeLocsEntry* loc = sObjectMgr->GetClosestGraveYard((*itr)->x, (*itr)->y, (*itr)->z, (*itr)->mapId, 0);
           if (!loc)
           {
               ++itr;
               continue;
           }

           GraveYardContainer::const_iterator graveItr;
           for (graveItr = graveLow; graveItr != graveUp; ++graveItr)
               if (graveItr->second.safeLocId == loc->ID)
                   break;
           if (graveItr == graveUp)
           {
               GraveYardData graveData;
               graveData.safeLocId = loc->ID;
               graveData.team = 0;
               graveItr = sObjectMgr->GraveYardStore.insert(std::make_pair(ZONE_WINTERGRASP, graveData));
           }

           for (BuildingStateMap::iterator stateItr = BuildingStates.begin(); stateItr != BuildingStates.end(); ++stateItr)
           {
               if (stateItr->second->worldState == (*itr)->worldState)
               {
                   stateItr->second->graveTeam = const_cast<uint32*>(&graveItr->second.team);
                   break;
               }
           }
           areaPOIs.erase(itr++);
       }
       else
           ++itr;
   }

   sObjectMgr->AddGameobjectData(GO_TITAN_RELIC, 571, 5440.0f, 2840.8f, 420.43f + 10.0f, 0);

   _LoadTeamPair(m_goDisplayPair, OutdoorPvPWGGODisplayPair);
   _LoadTeamPair(m_creEntryPair, OutdoorPvPWGCreEntryPair);

   sWorld->SendWintergraspState();

   m_towerDamagedCount[TEAM_ALLIANCE] = 0;
   m_towerDestroyedCount[TEAM_ALLIANCE] = 0;
   m_towerDamagedCount[TEAM_HORDE] = 0;
   m_towerDestroyedCount[TEAM_HORDE] = 0;

   RemoveOfflinePlayerWGAuras();

   RegisterZone(ZONE_WINTERGRASP);

   if (m_wartime)
   {
       uint32 m_WSTimer = m_timer;
       StartBattle();
       m_timer = m_WSTimer;
   }

   return true;
}

void OutdoorPvPWG::ProcessEvent(WorldObject* objin, uint32 eventId)
{
   GameObject* obj = objin->ToGameObject();
   if (!obj)
		return;

   if (obj->GetEntry() == GO_TITAN_RELIC)
   {
       if (obj->GetGOInfo()->goober.eventId == eventId && IsWarTime() && m_gate && m_gate->damageState == DAMAGE_DESTROYED)
       {
           m_changeDefender = true;
           m_timer = 0;
       }
   }
   else if (obj->GetGoType() == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
   {
       BuildingStateMap::const_iterator itr = BuildingStates.find(obj->GetDBTableGUIDLow());
       if (itr == BuildingStates.end())
           return;

       std::string msgStr;
       switch (eventId)
       { // TODO - Localized msgs of GO names
           case 19672:
		   case 19675: // Flamewatch Tower
               msgStr = "Flamewatch";
               break;
           case 18553:
		   case 19677: // Shadowsight Tower
               msgStr = "Shadowsight";
               break;
           case 19673:
		   case 19676: // Winter's Edge Tower
               msgStr = "Winter's Edge";
               break;
           case 19776:
		   case 19778: // E Workshop damaged
               msgStr = "Sunken Ring";
               break;
           case 19777:
		   case 19779: // W Workshop damaged
               msgStr = "Broken Temple";
               break;
           case 19782:
		   case 19786: // NW Workshop damaged
               msgStr = "North-Western";
               break;
           case 19783:
		   case 19787: // NE Workshop damaged
               msgStr = "North-Eastern";
               break;
           case 19784:
		   case 19788: // SW Workshop damaged
               msgStr = "Westpark";
               break;
           case 19785:
		   case 19789: // SE Workshop damaged
               msgStr = "Eastpark";
               break;
           case 19657:
		   case 19661: // NW Wintergrasp Keep Tower damaged
               msgStr = "North-Western";
               break;
           case 19658:
		   case 19663: // NE Wintergrasp Keep Tower damaged
               msgStr = "North-Eastern";
               break;
           case 19659:
		   case 19662: // SW Wintergrasp Keep Tower damaged
               msgStr = "South-Western";
               break;
           case 19660:
		   case 19664: // SE Wintergrasp Keep Tower damaged
               msgStr = "South-Eastern";
               break;
           default:
               msgStr = "";
       }

       BuildingState* state = itr->second;
       if (eventId == obj->GetGOInfo()->building.damagedEvent)
       {
           state->damageState = DAMAGE_DAMAGED;
           switch (state->type)
           {
               case BUILDING_WORKSHOP:
                   {
                       msgStr = fmtstring(sObjectMgr->GetQuantumSystemTextForDBCLocale(LANG_BG_WG_WORKSHOP_DAMAGED), msgStr.c_str(), sObjectMgr->GetQuantumSystemTextForDBCLocale(GetDefenderTeam() == TEAM_ALLIANCE ? LANG_BG_AB_ALLY : LANG_BG_AB_HORDE));
                       sWorld->SendZoneText(ZONE_WINTERGRASP, msgStr.c_str());
                   }
                   break;
               case BUILDING_WALL:
                   {
					   sWorld->SendZoneText(ZONE_WINTERGRASP, sObjectMgr->GetQuantumSystemTextForDBCLocale(LANG_BG_WG_FORTRESS_UNDER_ATTACK));
					   for (PlayerSet::iterator itr = m_PlayersInWar[GetDefenderTeam()].begin(); itr != m_PlayersInWar[GetDefenderTeam()].end(); ++itr)
                       {
	                        if (GetDefenderTeam() == TEAM_ALLIANCE)
	                        {
                               TeamIDsound = OUTDOOR_PVP_WG_SOUND_KEEP_ASSAULTED_HORDE; //Alliance Worn Sound
	                        }
							else TeamIDsound = OUTDOOR_PVP_WG_SOUND_KEEP_ASSAULTED_ALLIANCE;  //Horde Worn Sound
							(*itr)->PlayDirectSound(TeamIDsound); // Wintergrasp Fortress under Siege
                       }

                       for (PlayerSet::iterator itr = m_PlayersInWar[GetAttackerTeam()].begin(); itr != m_PlayersInWar[GetAttackerTeam()].end(); ++itr)
                       {
	                        if (GetDefenderTeam() == TEAM_ALLIANCE)
	                        {
                               TeamIDsound = OUTDOOR_PVP_WG_SOUND_KEEP_ASSAULTED_HORDE; //Alliance Worn Sound
	                        }
							else TeamIDsound = OUTDOOR_PVP_WG_SOUND_KEEP_ASSAULTED_ALLIANCE;  //Horde Worn Sound
							(*itr)->PlayDirectSound(TeamIDsound); // Wintergrasp Fortress under Siege
                       }
                   }
				   break;
               case BUILDING_TOWER:
                   {
                       ++m_towerDamagedCount[state->GetTeam()];
                       msgStr = fmtstring(sObjectMgr->GetQuantumSystemTextForDBCLocale(LANG_BG_WG_TOWER_DAMAGED), msgStr.c_str());
                       sWorld->SendZoneText(ZONE_WINTERGRASP, msgStr.c_str());

			            for (PlayerSet::iterator itr = m_PlayersInWar[GetDefenderTeam()].begin(); itr != m_PlayersInWar[GetDefenderTeam()].end(); ++itr)
						{
	                        if (GetDefenderTeam() == TEAM_ALLIANCE)
	                        {
                               TeamIDsound = OUTDOOR_PVP_WG_SOUND_KEEP_CAPTURED_HORDE; // Alliance Worn Sound
	                        }
							else TeamIDsound = OUTDOOR_PVP_WG_SOUND_KEEP_CAPTURED_ALLIANCE;  // Horde Worn Sound
							(*itr)->PlayDirectSound(TeamIDsound); // Wintergrasp Fortress under Siege
                       }

                       for (PlayerSet::iterator itr = m_PlayersInWar[GetAttackerTeam()].begin(); itr != m_PlayersInWar[GetAttackerTeam()].end(); ++itr)
                       {
	                        if (GetDefenderTeam() == TEAM_ALLIANCE)
	                        {
                               TeamIDsound = OUTDOOR_PVP_WG_SOUND_KEEP_CAPTURED_HORDE; // Alliance Worn Sound
	                        }
							else TeamIDsound = OUTDOOR_PVP_WG_SOUND_KEEP_CAPTURED_ALLIANCE;  // Horde Worn Sound
							(*itr)->PlayDirectSound(TeamIDsound); // Wintergrasp Fortress under Siege
                       }
                   }
                   break;
           }
       }
       else if (eventId == obj->GetGOInfo()->building.destroyedEvent)
       {
           state->damageState = DAMAGE_DESTROYED;

           for (PlayerSet::const_iterator itr = m_PlayersInWar[GetAttackerTeam()].begin(); itr != m_PlayersInWar[GetAttackerTeam()].end(); ++itr)
           {
               if ((*itr))
               {
                   IncrementQuest((*itr), 13222);
                   IncrementQuest((*itr), 13223);
               }
           }

           switch (state->type)
           {
               case BUILDING_WORKSHOP:
                   ModifyWorkshopCount(state->GetTeam(), false);
                   msgStr = fmtstring(sObjectMgr->GetQuantumSystemTextForDBCLocale(LANG_BG_WG_WORKSHOP_DESTROYED), msgStr.c_str(), sObjectMgr->GetQuantumSystemTextForDBCLocale(GetDefenderTeam() == TEAM_ALLIANCE ? LANG_BG_AB_ALLY : LANG_BG_AB_HORDE));
                   sWorld->SendZoneText(ZONE_WINTERGRASP, msgStr.c_str());
                   break;
               case BUILDING_WALL:
                   sWorld->SendZoneText(ZONE_WINTERGRASP, sObjectMgr->GetQuantumSystemTextForDBCLocale(LANG_BG_WG_FORTRESS_UNDER_ATTACK));
			        for (PlayerSet::iterator itr = m_PlayersInWar[GetDefenderTeam()].begin(); itr != m_PlayersInWar[GetDefenderTeam()].end(); ++itr)
                   {
	                    if (GetDefenderTeam() == TEAM_ALLIANCE)
	                    {
                           TeamIDsound = OUTDOOR_PVP_WG_SOUND_KEEP_CAPTURED_HORDE; // Alliance Worn Sound
	                    }
						else TeamIDsound = OUTDOOR_PVP_WG_SOUND_KEEP_CAPTURED_ALLIANCE;  // Horde Worn Sound
						(*itr)->PlayDirectSound(TeamIDsound); // Wintergrasp Fortress under Siege
                   }
                   for (PlayerSet::iterator itr = m_PlayersInWar[GetAttackerTeam()].begin(); itr != m_PlayersInWar[GetAttackerTeam()].end(); ++itr)
                   {
	                    if (GetDefenderTeam() == TEAM_ALLIANCE)
	                    {
                           TeamIDsound = OUTDOOR_PVP_WG_SOUND_KEEP_CAPTURED_HORDE; // Alliance Worn Sound
	                    }
						else TeamIDsound = OUTDOOR_PVP_WG_SOUND_KEEP_CAPTURED_ALLIANCE;  // Horde Worn Sound
						(*itr)->PlayDirectSound(TeamIDsound); // Wintergrasp Fortress under Siege
                   }
                   break;
               case BUILDING_TOWER:
                   --m_towerDamagedCount[state->GetTeam()];
                   ++m_towerDestroyedCount[state->GetTeam()];

                   switch (eventId)
                   {
                       case 19672:
                       case 19675: // Flamewatch Tower
                       case 18553:
                       case 19677: // Shadowsight Tower
                       case 19673:
                       case 19676: // Winter's Edge Tower
                           for (PlayerSet::const_iterator itr = m_PlayersInWar[GetDefenderTeam()].begin(); itr != m_PlayersInWar[GetDefenderTeam()].end(); ++itr)
                           {
							   IncrementQuest((*itr), 13538, true);
							   IncrementQuest((*itr), 13539, true);

							   (*itr)->AreaExploredOrEventHappens(13538);
							   (*itr)->AreaExploredOrEventHappens(13539);

							   DoCompleteOrIncrementAchievement(ACHIEVEMENTS_WG_TOWER_DESTROY, (*itr));
                           }
                           break;
                       default:
                           break;
                   }

                   if (state->GetTeam() == GetAttackerTeam())
                   {
                       TeamCastSpell(GetAttackerTeam(), -SPELL_TOWER_CONTROL);
                       TeamCastSpell(GetDefenderTeam(), -SPELL_TOWER_CONTROL);
                       uint32 attStack = 3 - m_towerDestroyedCount[GetAttackerTeam()];

                       if (m_towerDestroyedCount[GetAttackerTeam()])
                       {
                           for (PlayerSet::iterator itr = m_PlayersInWar[GetDefenderTeam()].begin(); itr != m_PlayersInWar[GetDefenderTeam()].end(); ++itr)
                               if ((*itr)->GetCurrentLevel() > minlevel)
                                   (*itr)->SetAuraStack(SPELL_TOWER_CONTROL, (*itr), m_towerDestroyedCount[GetAttackerTeam()]);
                       }

                       if (attStack)
                       {
                           for (PlayerSet::iterator itr = m_PlayersInWar[GetAttackerTeam()].begin(); itr != m_PlayersInWar[GetAttackerTeam()].end(); ++itr)
                               if ((*itr)->GetCurrentLevel() > minlevel)
                                   (*itr)->SetAuraStack(SPELL_TOWER_CONTROL, (*itr), attStack);
                       }
                       else
                       {
                           if (m_timer <600000)
                               m_timer = 0;
                           else
                               m_timer = m_timer - 600000; // - 10 mins
                       }
                   }
                   msgStr = fmtstring(sObjectMgr->GetQuantumSystemTextForDBCLocale(LANG_BG_WG_TOWER_DESTROYED), msgStr.c_str());
                   sWorld->SendZoneText(ZONE_WINTERGRASP, msgStr.c_str());

				   for (PlayerSet::iterator itr = m_PlayersInWar[GetDefenderTeam()].begin(); itr != m_PlayersInWar[GetDefenderTeam()].end(); ++itr)
                   {
	                    if (GetDefenderTeam() == TEAM_ALLIANCE)
	                    {
                           TeamIDsound = OUTDOOR_PVP_WG_SOUND_HORDE_CAPTAIN; // Alliance Worn Sound
	                    }
						else TeamIDsound = OUTDOOR_PVP_WG_SOUND_ALLIANCE_CAPTAIN;  // Horde Worn Sound
	                        (*itr)->PlayDirectSound(TeamIDsound); // Wintergrasp Fortress under Siege
                   }
                   for (PlayerSet::iterator itr = m_PlayersInWar[GetAttackerTeam()].begin(); itr != m_PlayersInWar[GetAttackerTeam()].end(); ++itr)
                   {
	                    if (GetDefenderTeam( )== TEAM_ALLIANCE)
	                    {
                           TeamIDsound = OUTDOOR_PVP_WG_SOUND_HORDE_CAPTAIN; // Alliance Worn Sound
	                    }
						else TeamIDsound = OUTDOOR_PVP_WG_SOUND_ALLIANCE_CAPTAIN;  // Horde Worn Sound
						(*itr)->PlayDirectSound(TeamIDsound); // Wintergrasp Fortress destroyed Siege
                   }
                   break;
           }
           BroadcastStateChange(state);
       }
   }
}

void OutdoorPvPWG::RemoveOfflinePlayerWGAuras()
{
   CharacterDatabase.PExecute("DELETE FROM character_aura WHERE spell IN (%u, %u, %u, %u, %u)", SPELL_RECRUIT, SPELL_CORPORAL, SPELL_LIEUTENANT, SPELL_TENACITY, SPELL_TOWER_CONTROL);
}

void OutdoorPvPWG::ModifyWorkshopCount(TeamId team, bool add)
{
   if (team == TEAM_NEUTRAL)
       return;

   if (add)
       ++m_workshopCount[team];
   else if (m_workshopCount[team])
       --m_workshopCount[team];
   else
       sLog->OutErrorConsole("OutdoorPvPWG::ModifyWorkshopCount: negative workshop count!");

   SendUpdateWorldState(MaxVehNumWorldState[team], m_workshopCount[team]* MAX_VEHICLE_PER_WORKSHOP);
}

uint32 OutdoorPvPWG::GetCreatureEntry(uint32 guidlow, const CreatureData* creatureData)
{
   if (GetDefenderTeam() == TEAM_ALLIANCE)
   {
       TeamPairMap::const_iterator itr = m_creEntryPair.find(creatureData->id);
       if (itr != m_creEntryPair.end())
       {
           const_cast<CreatureData*>(creatureData)->displayid = 0;
           return itr->second;
       }
   }
   return creatureData->id;
}

OutdoorPvPWGCreType OutdoorPvPWG::GetCreatureType(uint32 entry) const
{
   switch (entry)
   {
       case 27881: // Catapult
       case 28094: // Demolisher
       case 28312: // Alliance Siege Engine
       case 32627: // Horde Siege Engine
       case 28319: // Siege turret
       case 32629: // Siege turret
           return CREATURE_SIEGE_VEHICLE;
       case 28366: // Wintergrasp Tower cannon
           return CREATURE_TURRET;
       case NPC_GNOMISH_ENGINEER_A: // Alliance Engineer
       case NPC_GNOMISH_ENGINEER_H: // Horde Engineer
           return CREATURE_ENGINEER;
       case 30739:
	   case 30740: // Champions
       case 32307:
	   case 32308: // Guards
           return CREATURE_GUARD;
       case NPC_SPIRIT_HEALER_A: // Dwarven Spirit Guide
       case NPC_SPIRIT_HEALER_H: // Taunka Spirit Guide
           return CREATURE_SPIRIT_GUIDE;
       case 6491: // Spirit Healers
           return CREATURE_SPIRIT_HEALER;
       case NPC_HOODOO_MASTER_FUJIN_H:
	   case NPC_SORCERESS_KAYLANA_A: // Hoodoo Master & Sorceress
       case 31102:
	   case 31052: // Vieron Blazefeather & Bowyer
       case 31107:
	   case 31109: // Lieutenant & Senior Demolitionist
       case 31151:
	   case 31153: // Tactical Officer
       case 31106:
	   case 31108: // Siegesmith & Siege Master
       case 31053:
	   case 31054: // Primalist & Anchorite
       case 31091:
	   case 31036: // Commander
           return CREATURE_QUESTGIVER;
       case 32615:
	   case 32626: // Warbringer && Brigadier General
       case 32296:
	   case 32294: // Quartermaster
       case 39173:
	   case 39172: // Ros'slai && Marshal Magruder
       case 30870:
	   case 30869: // Flight Masters
           return CREATURE_SPECIAL;
       default:
           return CREATURE_OTHER; // Revenants, Elementals, etc
   }
}

void OutdoorPvPWG::OnCreatureCreate(Creature* creature)
{
	uint32 entry = creature->GetEntry();
	switch (GetCreatureType(entry))
	{
	    case CREATURE_SIEGE_VEHICLE:
		{
			if (!creature->IsSummon())
				return;

			TeamId team;

			if (creature->GetFaction() == WintergraspFaction[TEAM_ALLIANCE])
				team = TEAM_ALLIANCE;
			else if (creature->GetFaction() == WintergraspFaction[TEAM_HORDE])
				team = TEAM_HORDE;
			else
				return;

			if (uint32 engLowguid = GUID_LOPART(((TempSummon*)creature)->GetSummonerGUID()))
			{
				if (OPvPCapturePointWG* workshop = GetWorkshopByEngGuid(engLowguid))
				{
					if (CanBuildVehicle(workshop))
						vehicles[team].insert(creature);
					else
					{
						creature->setDeathState(DEAD);
						creature->SetRespawnTime(DAY);
						return;
					}
				}

				if (m_tenacityStack > 0 && team == TEAM_ALLIANCE)
					creature->SetAuraStack(SPELL_TENACITY_VEHICLE, creature, m_tenacityStack);
				else if (m_tenacityStack < 0 && team == TEAM_HORDE)
					creature->SetAuraStack(SPELL_TENACITY_VEHICLE, creature, -m_tenacityStack);
			}
			SendUpdateWorldState(VehNumWorldState[team], vehicles[team].size());
			break;
		}
		case CREATURE_QUESTGIVER:
		{
			m_questgivers[creature->GetDBTableGUIDLow()] = creature;
			creature->SetReactState(REACT_PASSIVE);
		}
		break;
		case CREATURE_ENGINEER:
		{
			for (OutdoorPvP::OPvPCapturePointMap::iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
			{
				if (OPvPCapturePointWG* workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
				{
					if (workshop->m_engGuid == creature->GetDBTableGUIDLow())
					{
						workshop->m_engineer = creature;
						break;
					}
				}
			}
		}
		break;
		case CREATURE_SPIRIT_GUIDE:
		{
			for (OutdoorPvP::OPvPCapturePointMap::iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
			{
				if (OPvPCapturePointWG* workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
				{
					if (workshop->m_spiGuid == creature->GetDBTableGUIDLow())
					{
						workshop->m_spiritguide = creature;
						break;
					}
				}
			}
			creature->SetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT, creature->GetGUID());
			creature->CastSpell(creature, SPELL_SPIRITUAL_IMMUNITY, true);
			creature->SetUInt32Value(UNIT_CHANNEL_SPELL, SPELL_SPIRIT_HEAL_CHANNEL);
			creature->SetUInt32Value(UNIT_FIELD_BYTES_2, 1);
			creature->SetFloatValue(UNIT_MOD_CAST_SPEED, 1.0f);
		}
		case CREATURE_SPIRIT_HEALER:
		case CREATURE_TURRET:
		case CREATURE_OTHER:
			UpdateCreatureInfo(creature);
		default:
			m_creatures.insert(creature);
			break;
	}
}

void OutdoorPvPWG::OnCreatureRemove(Creature* creature)
{
   uint32 entry = creature->GetEntry();
   switch (GetCreatureType(entry))
   {
       case CREATURE_SIEGE_VEHICLE:
       {
           if (!creature->IsSummon())
               return;

		   TeamId team;

           // TODO: now you have to wait until the corpse of vehicle disappear to build a new one
           if (vehicles[TEAM_ALLIANCE].erase(creature))
               team = TEAM_ALLIANCE;
           else if (vehicles[TEAM_HORDE].erase(creature))
               team = TEAM_HORDE;
           else
               return;

           SendUpdateWorldState(VehNumWorldState[team], vehicles[team].size());
           break;
       }
       case CREATURE_QUESTGIVER:
           m_questgivers.erase(creature->GetDBTableGUIDLow());
           break;
       case CREATURE_ENGINEER:
           for (OutdoorPvP::OPvPCapturePointMap::iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
           {
               if (OPvPCapturePointWG* workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
			   {
                   if (workshop->m_engGuid == creature->GetDBTableGUIDLow())
                   {
                       workshop->m_engineer = NULL;
                       break;
                   }
			   }
           }
           break;
       case CREATURE_SPIRIT_GUIDE:
           for (OutdoorPvP::OPvPCapturePointMap::iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
           {
               if (OPvPCapturePointWG* workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
			   {
                   if (workshop->m_spiGuid == creature->GetDBTableGUIDLow())
                   {
                       workshop->m_spiritguide = NULL;
                       break;
                   }
			   }
           }
           creature->CastSpell(creature, SPELL_SPIRITUAL_IMMUNITY, true);
       default:
           m_creatures.erase(creature);
           break;
   }
}

void OutdoorPvPWG::OnGameObjectCreate(GameObject* go)
{
   OutdoorPvP::OnGameObjectCreate(go);

   if (UpdateGameObjectInfo(go))
       m_gobjects.insert(go);

   // do we need to store building?
   else if (go->GetGoType() == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
   {
       BuildingStateMap::const_iterator itr = BuildingStates.find(go->GetDBTableGUIDLow());
       if (itr != BuildingStates.end())
       {
           itr->second->building = go;
           if (go->GetGOInfo()->displayId == 7878 || go->GetGOInfo()->displayId == 7900)
               itr->second->type = BUILDING_TOWER;

           if (itr->second->damageState == DAMAGE_INTACT && !itr->second->health)
           {
               itr->second->health = go->GetGOValue()->Building.Health;
               go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);
           }
           else
           {
               go->GetGOValue()->Building.Health = itr->second->health;
               if (itr->second->damageState == DAMAGE_DAMAGED)
                   go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED);
               else if (itr->second->damageState == DAMAGE_DESTROYED)
                   go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);
           }
       }
   }
}

void OutdoorPvPWG::OnGameObjectRemove(GameObject* go)
{
   OutdoorPvP::OnGameObjectRemove(go);

   if (UpdateGameObjectInfo(go))
       m_gobjects.erase(go);

   // do we need to store building?
   else if (go->GetGoType() == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
   {
       BuildingStateMap::const_iterator itr = BuildingStates.find(go->GetDBTableGUIDLow());
       if (itr != BuildingStates.end())
       {
           itr->second->building = NULL;
           if (go->GetGOInfo()->displayId == 7878 || go->GetGOInfo()->displayId == 7900)
               itr->second->type = BUILDING_TOWER;
           if (itr->second->damageState == DAMAGE_INTACT && !itr->second->health)
           {
               itr->second->health = go->GetGOValue()->Building.Health;
               go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);
           }
           else
           {
               go->GetGOValue()->Building.Health = itr->second->health;
               if (itr->second->damageState == DAMAGE_DAMAGED)
                   go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED);
               else if (itr->second->damageState == DAMAGE_DESTROYED)
                   go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);
           }
       }
   }
}

void OutdoorPvPWG::UpdateAllWorldObject()
{
   // update cre and go factions
   for (GameObjectSet::const_iterator itr = m_gobjects.begin(); itr != m_gobjects.end(); ++itr)
       UpdateGameObjectInfo(*itr);
   for (CreatureSet::const_iterator itr = m_creatures.begin(); itr != m_creatures.end(); ++itr)
       UpdateCreatureInfo(*itr);
   for (QuestGiverMap::const_iterator itr = m_questgivers.begin(); itr != m_questgivers.end(); ++itr)
       UpdateQuestGiverPosition((*itr).first, (*itr).second);

   // rebuild and update building states
   RebuildAllBuildings();

   // update capture points
   for (OPvPCapturePointMap::const_iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
   {
       if (OPvPCapturePointWG* workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
           workshop->SetTeamByBuildingState();
   }
}

void OutdoorPvPWG::RebuildAllBuildings()
{
   for (BuildingStateMap::const_iterator itr = BuildingStates.begin(); itr != BuildingStates.end(); ++itr)
   {
       if (itr->second->building)
       {
           UpdateGameObjectInfo(itr->second->building);
           //itr->second->building->Rebuild();
           itr->second->building->SetDestructibleState(GO_DESTRUCTIBLE_REBUILDING, NULL, true);
           itr->second->health = itr->second->building->GetGOValue()->Building.Health;
       }
       else
           itr->second->health = 0;

       if (itr->second->damageState == DAMAGE_DESTROYED)
       {
           if (itr->second->type == BUILDING_WORKSHOP)
               ModifyWorkshopCount(itr->second->GetTeam(), true);
       }

       itr->second->damageState = DAMAGE_INTACT;
       itr->second->SetTeam(GetDefenderTeam() == TEAM_ALLIANCE ? OTHER_TEAM(itr->second->defaultTeam) : itr->second->defaultTeam);
   }
   m_towerDamagedCount[TEAM_ALLIANCE] = 0;
   m_towerDestroyedCount[TEAM_ALLIANCE] = 0;
   m_towerDamagedCount[TEAM_HORDE] = 0;
   m_towerDestroyedCount[TEAM_HORDE] = 0;
}

void OutdoorPvPWG::SendInitWorldStatesTo(Player* player) const
{
	WorldPacket data(SMSG_INIT_WORLD_STATES, 4 + 4 + 4 + 2 + (BuildingStates.size() * 8));

	data << uint32(571);
	data << uint32(ZONE_WINTERGRASP);
	data << uint32(0);
	data << uint16(10 + BuildingStates.size());

	data << uint32(WORLDSTATE_WINTERGRASP_STATE_ATTACKER) << uint32(GetAttackerTeam());
	data << uint32(WORLDSTATE_WINTERGRASP_STATE_DEFENDER) << uint32(GetDefenderTeam());
	data << uint32(WORLDSTATE_WINTERGRASP_STATE_ACTIVE) << uint32(IsWarTime() ? 0 : 1);
	data << uint32(WORLDSTATE_WINTERGRASP_STATE_SHOW_WORLDSTATE) << uint32(IsWarTime() ? 1 : 0);

	for (uint32 i = 0; i < 2; ++i)
		data << ClockWorldState[i] << m_clock[i];

   data << uint32(WORLDSTATE_WINTERGRASP_STATE_VEHICLE_H) << uint32(vehicles[TEAM_HORDE].size());
   data << uint32(WORLDSTATE_WINTERGRASP_STATE_MAX_VEHICLE_H) << m_workshopCount[TEAM_HORDE]* MAX_VEHICLE_PER_WORKSHOP;
   data << uint32(WORLDSTATE_WINTERGRASP_STATE_VEHICLE_A) << uint32(vehicles[TEAM_ALLIANCE].size());
   data << uint32(WORLDSTATE_WINTERGRASP_STATE_MAX_VEHICLE_A) << m_workshopCount[TEAM_ALLIANCE]* MAX_VEHICLE_PER_WORKSHOP;

   for (BuildingStateMap::const_iterator itr = BuildingStates.begin(); itr != BuildingStates.end(); ++itr)
       itr->second->FillData(data);

   if (player)
       player->GetSession()->SendPacket(&data);
   else
       BroadcastPacket(data);
}

void OutdoorPvPWG::BroadcastStateChange(BuildingState* state) const
{
   if (m_sendUpdate)
   {
       for (uint8 team = 0; team < 2; ++team)
	   {
           for (PlayerSet::const_iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
               state->SendUpdate(*itr);
	   }
   }
}

// Called at Start and Battle End
bool OutdoorPvPWG::UpdateCreatureInfo(Creature* creature)
{
	if (!creature)
		return false;

	uint32 entry = creature->GetEntry();
	switch (GetCreatureType(entry))
	{
        case CREATURE_TURRET:
			if (IsWarTime())
			{
				if (!creature->IsAlive())
					creature->Respawn(true);

				creature->SetCurrentFaction(WintergraspFaction[GetDefenderTeam()]);
				creature->SetVisible(true);
			}
			else
           {
               if (creature->IsVehicle() && creature->GetVehicleKit())
                   creature->GetVehicleKit()->RemoveAllPassengers();
               creature->SetVisible(false);
               creature->SetCurrentFaction(35);
           }
           return false;
        case CREATURE_OTHER:
        {
            if (IsWarTime())
            {
                creature->SetVisible(false);
                creature->SetCurrentFaction(35);

                // Prevent from hiding
                switch (entry)
                {
                    case 30560: // The RP-GG
                    case 27852: // This creature is neded for spell_target in workshops while building siege machines
                    case 27869: // Wintergrasp Detection Unit
                    case 23472: // World Trigger (Large AOI, Not Immune PC/NPC)
                    {
                        creature->SetPhaseMask(1, true);
                        creature->RestoreFaction();
                        creature->SetVisible(true);
                    }
                    break;
                }
            }
			else
			{
                creature->RestoreFaction();
                creature->SetVisible(true);
            }
            return false;
		}
       case CREATURE_SPIRIT_GUIDE:
           map = creature->GetMap();
           if (IsWarTime())
           {
              if (creature->GetAreaId() == 4575) // Select Fortress Spirit
              {
				  FortressSpirit = creature;

                if (GetDefenderTeam() == TEAM_ALLIANCE) // Fortress Spirit Alliance
                   FortressSpirit->UpdateEntry(NPC_SPIRIT_HEALER_A);

                if (GetDefenderTeam() == TEAM_HORDE) // Fortress Spirit Horde
                   FortressSpirit->UpdateEntry(NPC_SPIRIT_HEALER_H);
              }
			  creature->SetVisible(true);
           }
           else
           {
               creature->SetVisible(false);
           }
           return false;
       case CREATURE_SPIRIT_HEALER:
           creature->SetVisible(IsWarTime() ? false : true);
           return false;
       case CREATURE_ENGINEER:
           return false;
       case CREATURE_SIEGE_VEHICLE:
           if (!IsWarTime())
           {
               if (creature->IsVehicle() && creature->GetVehicleKit())
                   creature->GetVehicleKit()->RemoveAllPassengers();
               creature->DisappearAndDie();
           }
           return false;
       case CREATURE_GUARD:
       case CREATURE_SPECIAL:
           {
               if (creature && creature->GetAreaId() == 4575)
               {
                   switch (entry)
                   {
                       case 30740://Alliance champion 
                       case 32308://Alliance guard
                       {
                           if (GetDefenderTeam() == TEAM_ALLIANCE)
                               creature->SetPhaseMask(1, true);
                           else 
                               creature->SetPhaseMask(2, true);
                           break;
                       }
                       case 30739://Horde champion
                       case 32307://Horde guard
                       {
                           if (GetDefenderTeam() == TEAM_ALLIANCE)
                               creature->SetPhaseMask(2, true);
                           else 
                               creature->SetPhaseMask(1, true);
                           break;
                       }
                   }
                   ResetCreatureEntry(creature, entry);
                   creature->AI()->EnterEvadeMode();
                   return false;
               }
               else 
               {
                   TeamPairMap::const_iterator itr = m_creEntryPair.find(creature->GetCreatureData()->id);
                   if (itr != m_creEntryPair.end())
                   {
                       entry = GetDefenderTeam() == TEAM_ALLIANCE ? itr->second : itr->first;
                       ResetCreatureEntry(creature, entry);
                       creature->AI()->EnterEvadeMode();
                   }
                   return false;
               }
           }
       default:
           return false;
   }
}

bool OutdoorPvPWG::UpdateQuestGiverPosition(uint32 guid, Creature* creature)
{
   assert(guid);
   Position pos = m_qgPosMap[std::pair<uint32, bool>(guid, GetDefenderTeam() == TEAM_HORDE)];

   if (creature && creature->IsInWorld())
   {
       if (creature->GetPositionX() == pos.GetPositionX() && creature->GetPositionY() == pos.GetPositionY() && creature->GetPositionZ() == pos.GetPositionZ())
           return false;

       if (creature->IsAlive() && creature->IsInCombatActive())
       {
           creature->CombatStop(true);
           creature->getHostileRefManager().deleteReferences();
       }
       creature->SetHomePosition(pos);
       creature->DestroyForNearbyPlayers();
       if (!creature->GetMap()->IsGridLoaded(pos.GetPositionX(), pos.GetPositionY()))
           creature->GetMap()->LoadGrid(pos.GetPositionX(), pos.GetPositionY());
       creature->GetMap()->CreatureRelocation(creature, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation());
       if (!creature->IsAlive())
           creature->Respawn(true);
   }
   else
       sObjectMgr->MoveCreatureData(guid, 571, pos);

   return true;
}

bool OutdoorPvPWG::UpdateGameObjectInfo(GameObject* go) const
{
   uint32 AttackerFaction = 35;
   uint32 DefenderFaction = 35;

   if (IsWarTime())
   {
       AttackerFaction = WintergraspFaction[GetAttackerTeam()];
       DefenderFaction = WintergraspFaction[GetDefenderTeam()];
   }

   switch (go->GetGOInfo()->displayId)
   {
       case 8244: // Defender's Portal - Vehicle Teleporter
           go->SetUInt32Value(GAMEOBJECT_FACTION, WintergraspFaction[GetDefenderTeam()]);
           return true;
       case 7967: // Titan relic
           go->SetUInt32Value(GAMEOBJECT_FACTION, WintergraspFaction[GetAttackerTeam()]);
           return true;

       case 8165: // Wintergrasp Keep Door
       case 7877: // Wintergrasp Fortress Wall
       case 7878: // Wintergrasp Keep Tower
       case 7906: // Wintergrasp Fortress Gate
       case 7909: // Wintergrasp Wall
           go->SetUInt32Value(GAMEOBJECT_FACTION, DefenderFaction);
           return false;
       case 8256://Alliance Banner
       case 5651://Alliance Banner
           if (GetDefenderTeam() == TEAM_ALLIANCE)
           {
               if (go->GetAreaId() == 4575 || go->GetAreaId() == 4539 || go->GetAreaId() == 4538)
                   go->SetPhaseMask(1, true);
               else go->SetPhaseMask(2, true);
           } 
           else 
           {
               if (go->GetAreaId() == 4575 || go->GetAreaId() == 4539 || go->GetAreaId() == 4538)
                   go->SetPhaseMask(2, true);
               else go->SetPhaseMask(1, true);
           }
           return true;
        case 8257://Horde Banner
        case 5652://Horde Banner
           if (GetDefenderTeam() == TEAM_ALLIANCE)
           {
               if (go->GetAreaId() == 4575 || go->GetAreaId() == 4539 || go->GetAreaId() == 4538)
                   go->SetPhaseMask(2, true);
               else 
                   go->SetPhaseMask(1, true);
           } 
           else 
           {
               if (go->GetAreaId() == 4575 || go->GetAreaId() == 4539 || go->GetAreaId() == 4538)
                   go->SetPhaseMask(1, true);
               else go->SetPhaseMask(2, true);
           }
           return true;
       case 7900: // Flamewatch Tower - Shadowsight Tower - Winter's Edge Tower
           go->SetUInt32Value(GAMEOBJECT_FACTION, AttackerFaction);
           return false;
       case 8208: // Goblin Workshop
           OPvPCapturePointWG* workshop = GetWorkshopByGOGuid(go->GetGUID());
           if (workshop)
               go->SetUInt32Value(GAMEOBJECT_FACTION, WintergraspFaction[workshop->m_buildingState->GetTeam()]);
           return false;
   }

   // Note: this is only for test, still need db support
   TeamPairMap::const_iterator itr = m_goDisplayPair.find(go->GetGOInfo()->displayId);
   if (itr != m_goDisplayPair.end())
   {
       go->SetUInt32Value(GAMEOBJECT_DISPLAYID, GetDefenderTeam() == TEAM_ALLIANCE ?
           itr->second : itr->first);
       return true;
   }
   return false;
}

void OutdoorPvPWG::HandlePlayerEnterZone(Player* player, uint32 zone)
{
   if (!sWorld->getBoolConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_ENABLED))
       return;

   SendInitWorldStatesTo(player);
   OutdoorPvP::HandlePlayerEnterZone(player, zone);
   UpdateTenacityStack();

   if (IsWarTime())
   {
       // Check if player is not already in war
       if (m_PlayersInWar[player->GetTeamId()].count(player) || m_InvitedPlayers[player->GetTeamId()].count(player->GetGUID()))
       return;

       if (player->GetCurrentLevel() > minlevel)
       {
           if (m_PlayersInWar[player->GetTeamId()].size()+m_InvitedPlayers[player->GetTeamId()].size() < m_MaxPlayer) //Not fully
           {
               InvitePlayerToWar(player);
           }
           else // Full
           {
               // TODO: Send packet for announce it to player
               m_PlayersWillBeKick[player->GetTeamId()][player->GetGUID()]=time(NULL)+10;
               InvitePlayerToQueue(player);
           }
       }
   }
   else
   {
       // If time left is <15 minutes invite player to join queue
       if (!IsWarTime())
           if (m_timer <= StartGroupingTimer)
               InvitePlayerToQueue(player);
   }
}

// Reapply Auras if needed
void OutdoorPvPWG::HandlePlayerResurrects(Player* player, uint32 zone)
{
   if (!sWorld->getBoolConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_ENABLED))
       return;

   if (IsWarTime())
   {
       if (player->GetCurrentLevel() > minlevel)
       {
           if (player->GetTeamId() == TEAM_ALLIANCE && m_tenacityStack > 0 || player->GetTeamId() == TEAM_HORDE && m_tenacityStack < 0)
           {
               if (player->HasAura(SPELL_TENACITY))
				   player->RemoveAurasDueToSpell(SPELL_TENACITY);

               int32 newStack = m_tenacityStack < 0 ? -m_tenacityStack : m_tenacityStack;
               if (newStack > 20)
                   newStack = 20;
               player->SetAuraStack(SPELL_TENACITY, player, newStack);
           }

           // Tower Control
           if (player->GetTeamId() == GetAttackerTeam())
           {
               if (m_towerDestroyedCount[GetAttackerTeam()] < 3)
                   player->SetAuraStack(SPELL_TOWER_CONTROL, player, 3 - m_towerDestroyedCount[GetAttackerTeam()]);
           }
           else
           {
               if (m_towerDestroyedCount[GetAttackerTeam()])
                   player->SetAuraStack(SPELL_TOWER_CONTROL, player, m_towerDestroyedCount[GetAttackerTeam()]);
           }
       }
   }
   OutdoorPvP::HandlePlayerResurrects(player, zone);
}

void OutdoorPvPWG::HandlePlayerLeaveZone(Player* player, uint32 zone)
{
   if (!sWorld->getBoolConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_ENABLED))
       return;

   if (IsWarTime())
   {
	   if (m_PlayersInWar[player->GetTeamId()].find(player)!=m_PlayersInWar[player->GetTeamId()].end())
       {
           m_PlayersInWar[player->GetTeamId()].erase(player);
           player->GetSession()->SendBfLeaveMessage(1);

           if (Group* group = GetGroupPlayer(player->GetGUID(), player->GetTeamId())) 
           {
               if (!group->RemoveMember(player->GetGUID()))
               {
                   m_Groups[player->GetTeamId()].erase(group->GetGUID());
                   group->SetBattlegroundGroup(NULL);
               }
           }
       }
       else if (m_InvitedPlayers[player->GetTeamId()].count(player->GetGUID()))
       {
           player->GetSession()->SendBfLeaveMessage(1);
       }
   }

   m_InvitedPlayers[player->GetTeamId()].erase(player->GetGUID());
   m_PlayersWillBeKick[player->GetTeamId()].erase(player->GetGUID());

   if (!player->GetSession()->PlayerLogout())
   {
       if (player->GetVehicle())
           player->GetVehicle()->Dismiss();

       player->RemoveAurasDueToSpell(SPELL_RECRUIT);
       player->RemoveAurasDueToSpell(SPELL_CORPORAL);
       player->RemoveAurasDueToSpell(SPELL_LIEUTENANT);
       player->RemoveAurasDueToSpell(SPELL_TOWER_CONTROL);
       player->RemoveAurasDueToSpell(SPELL_SPIRITUAL_IMMUNITY);
   }
   player->RemoveAurasDueToSpell(SPELL_TENACITY);
   OutdoorPvP::HandlePlayerLeaveZone(player, zone);
   UpdateTenacityStack();
}

void OutdoorPvPWG::PromotePlayer(Player* killer) const
{
   Aura* aura;
   if (aura = killer->GetAura(SPELL_RECRUIT))
   {
       if (aura->GetStackAmount() >= 5)
       {
           killer->RemoveAura(SPELL_RECRUIT);
           killer->CastSpell(killer, SPELL_CORPORAL, true);
           ChatHandler(killer).PSendSysMessage(LANG_BG_WG_RANK1);
       }
       else
           killer->CastSpell(killer, SPELL_RECRUIT, true);
   }
   else if (aura = killer->GetAura(SPELL_CORPORAL))
   {
       if (aura->GetStackAmount() >= 5)
       {
           killer->RemoveAura(SPELL_CORPORAL);
           killer->CastSpell(killer, SPELL_LIEUTENANT, true);
           ChatHandler(killer).PSendSysMessage(LANG_BG_WG_RANK2);
       }
       else
           killer->CastSpell(killer, SPELL_CORPORAL, true);
   }
}

void OutdoorPvPWG::HandleKill(Player* killer, Unit* victim)
{
	if (!sWorld->getBoolConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_ENABLED) || !IsWarTime())
		return;

	bool ok = false;
	if (victim->GetTypeId() == TYPE_ID_PLAYER)
	{
		if (victim->GetCurrentLevel() >= 70)
			ok = true;

       IncrementQuest(killer, 13178); // Horde
       IncrementQuest(killer, 13180); // Horde
       IncrementQuest(killer, 13177); // Alliance
       IncrementQuest(killer, 13179); // Alliance
       //killer->RewardPlayerAndGroupAtEvent(CRE_PVP_KILL, victim);
   }
   else
   {
       switch (GetCreatureType(victim->GetEntry()))
       {
           case CREATURE_SIEGE_VEHICLE:
			   // Support For Quest (Stop The Siege)
			   if (killer->ToPlayer()->GetQuestStatus(13185) == QUEST_STATUS_INCOMPLETE || killer->ToPlayer()->GetQuestStatus(13186) == QUEST_STATUS_INCOMPLETE)
			   {
				   if (victim->HasAura(36444)) // Wintergrasp Water
					   return;

				   killer->ToPlayer()->KilledMonsterCredit(NPC_KILL_CREDIT_ENGINE, 0);
			   }
               ok = true;
               break;
           case CREATURE_GUARD:
               //killer->RewardPlayerAndGroupAtEvent(CRE_PVP_KILL, victim);
               ok = true;
               break;
           case CREATURE_TURRET:
               ok = true;
               break;
       }
   }

   if (ok)
   {
       if (Group* pGroup = killer->GetGroup())
       {
           for (GroupReference* itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
               if (itr->getSource()->IsAtGroupRewardDistance(killer) && itr->getSource()->GetCurrentLevel() > minlevel)
                   PromotePlayer(itr->getSource());
       }
       else if (killer->GetCurrentLevel() > minlevel)
           PromotePlayer(killer);
   }
}

void OutdoorPvPWG::UpdateTenacityStack()
{
    if (!IsWarTime())
        return;

    TeamId team = TEAM_NEUTRAL;
    uint32 allianceNum = 0;
    uint32 hordeNum = 0;
    int32 newStack = 0;

    for (PlayerSet::iterator itr = m_players[TEAM_ALLIANCE].begin(); itr != m_players[TEAM_ALLIANCE].end(); ++itr)
        if ((*itr)->GetCurrentLevel() > 74)
            ++allianceNum;

    for (PlayerSet::iterator itr = m_players[TEAM_HORDE].begin(); itr != m_players[TEAM_HORDE].end(); ++itr)
        if ((*itr)->GetCurrentLevel() > 74)
            ++hordeNum;

    if (allianceNum && hordeNum)
    {
        if (allianceNum < hordeNum)
            newStack = int32((float(hordeNum) / float(allianceNum) - 1)*4); // positive, should cast on alliance
        else if (allianceNum > hordeNum)
            newStack = int32((1 - float(allianceNum) / float(hordeNum))*4); // negative, should cast on horde
    }

    if (newStack == m_tenacityStack)
        return;

    if (m_tenacityStack > 0 && newStack <= 0) // old buff was on alliance
        team = TEAM_ALLIANCE;
    else if (m_tenacityStack < 0 && newStack >= 0) // old buff was on horde
        team = TEAM_HORDE;

    m_tenacityStack = newStack;

    // Remove old buff
    if (team != TEAM_NEUTRAL)
    {
        for (PlayerSet::const_iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
            if ((*itr)->GetCurrentLevel() > 74)
                (*itr)->RemoveAurasDueToSpell(SPELL_TENACITY);

        for (CreatureSet::const_iterator itr = vehicles[team].begin(); itr != vehicles[team].end(); ++itr)
            (*itr)->RemoveAurasDueToSpell(SPELL_TENACITY_VEHICLE);
    }

    // Apply new buff
    if (newStack)
    {
        team = newStack > 0 ? TEAM_ALLIANCE : TEAM_HORDE;
        if (newStack < 0)
            newStack = -newStack;
        if (newStack > 20)
            newStack = 20;

        for (PlayerSet::const_iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
            if ((*itr)->GetCurrentLevel() > 74)
                (*itr)->SetAuraStack(SPELL_TENACITY, (*itr), newStack);

        for (CreatureSet::const_iterator itr = vehicles[team].begin(); itr != vehicles[team].end(); ++itr)
            (*itr)->SetAuraStack(SPELL_TENACITY_VEHICLE, (*itr), newStack);
    }
}

void OutdoorPvPWG::UpdateClockDigit(uint32 &timer, uint32 digit, uint32 mod)
{
   uint32 value = timer%mod;

   if (m_clock[digit] != value)
   {
       m_clock[digit] = value;
       SendUpdateWorldState(ClockWorldState[digit], uint32(timer + time(NULL)));
       sWorld->SetWintergrapsTimer(uint32(timer + time(NULL)), digit);
   }
}

void OutdoorPvPWG::UpdateClock()
{
	uint32 timer = m_timer / 1000;
	
	if (!IsWarTime())
		UpdateClockDigit(timer, 1, 10);
	else
		UpdateClockDigit(timer, 0, 10);

	if ((m_timer> 1800000) && (m_timer <1801000) && (m_wartime == false))
	{
       sWorld->SendWorldText(LANG_BG_WG_WORLD_ANNOUNCE_30);
	   sLog->OutConsole("BATTLEGROUND MANAGER: Until The Battle of Wintergrasp Stay 30 Minutes!");
	}

	if ((m_timer> 600000) && (m_timer <601000) && (m_wartime == false))
	{
       sWorld->SendWorldText(LANG_BG_WG_WORLD_ANNOUNCE_10);
	   sLog->OutConsole("BATTLEGROUND MANAGER: Until The Battle of Wintergrasp Stay 10 Minutes!");
	}

	if (sWorld->getBoolConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_TIMER_ANNOUNCE_ENABLE))
	{
		if ((m_timer> 300000) && (m_timer <301000) && (m_wartime == false))
		{
			sWorld->SendWorldText(LANG_BG_WG_WORLD_ANNOUNCE_5_MIN);
			sLog->OutConsole("BATTLEGROUND MANAGER: Until The Battle of Wintergrasp Stay 5 Minutes!");
		}

		if ((m_timer> 240000) && (m_timer <241000) && (m_wartime == false))
		{
			sWorld->SendWorldText(LANG_BG_WG_WORLD_ANNOUNCE_4_MIN);
			sLog->OutConsole("BATTLEGROUND MANAGER: Until The Battle of Wintergrasp Stay 4 Minutes!");
		}

		if ((m_timer> 180000) && (m_timer <181000) && (m_wartime == false))
		{
			sWorld->SendWorldText(LANG_BG_WG_WORLD_ANNOUNCE_3_MIN);
			sLog->OutConsole("BATTLEGROUND MANAGER: Until The Battle of Wintergrasp Stay 3 Minutes!");
		}

		if ((m_timer> 120000) && (m_timer <121000) && (m_wartime == false))
		{
			sWorld->SendWorldText(LANG_BG_WG_WORLD_ANNOUNCE_2_MIN);
			sLog->OutConsole("BATTLEGROUND MANAGER: Until The Battle of Wintergrasp Stay 2 Minutes!");
		}

		if ((m_timer> 60000) && (m_timer <61000) && (m_wartime == false))
		{
			sWorld->SendWorldText(LANG_BG_WG_WORLD_ANNOUNCE_1_MIN);
			sLog->OutConsole("BATTLEGROUND MANAGER: Until The Battle of Wintergrasp Stay 1 Minutes!");
		}

		if ((m_timer> 30000) && (m_timer <31000) && (m_wartime == false))
		{
			sWorld->SendWorldText(LANG_BG_WG_WORLD_ANNOUNCE_30_SEC);
			sLog->OutConsole("BATTLEGROUND MANAGER: Until The Battle of Wintergrasp Stay 30 Seconds!");
		}

		if ((m_timer> 15000) && (m_timer <16000) && (m_wartime == false))
		{
			sWorld->SendWorldText(LANG_BG_WG_WORLD_ANNOUNCE_15_SEC);
			sLog->OutConsole("BATTLEGROUND MANAGER: Until The Battle of Wintergrasp Stay 15 Seconds!");
		}

		if ((m_timer> 10000) && (m_timer <11000) && (m_wartime == false))
		{
			sWorld->SendWorldText(LANG_BG_WG_WORLD_ANNOUNCE_10_SEC);
			sLog->OutConsole("BATTLEGROUND MANAGER: Until The Battle of Wintergrasp Stay 10 Seconds!");
		}

		if ((m_timer> 9000) && (m_timer <10000) && (m_wartime == false))
		{
			sWorld->SendWorldText(LANG_BG_WG_WORLD_ANNOUNCE_9_SEC);
			sLog->OutConsole("BATTLEGROUND MANAGER: Until The Battle of Wintergrasp Stay 9 Seconds!");
		}

		if ((m_timer> 8000) && (m_timer <9000) && (m_wartime == false))
		{
			sWorld->SendWorldText(LANG_BG_WG_WORLD_ANNOUNCE_8_SEC);
			sLog->OutConsole("BATTLEGROUND MANAGER: Until The Battle of Wintergrasp Stay 8 Seconds!");
		}

		if ((m_timer> 7000) && (m_timer <8000) && (m_wartime == false))
		{
			sWorld->SendWorldText(LANG_BG_WG_WORLD_ANNOUNCE_7_SEC);
			sLog->OutConsole("BATTLEGROUND MANAGER: Until The Battle of Wintergrasp Stay 7 Seconds!");
		}

		if ((m_timer> 6000) && (m_timer <7000) && (m_wartime == false))
		{
			sWorld->SendWorldText(LANG_BG_WG_WORLD_ANNOUNCE_6_SEC);
			sLog->OutConsole("BATTLEGROUND MANAGER: Until The Battle of Wintergrasp Stay 6 Seconds!");
		}

		if ((m_timer> 5000) && (m_timer <6000) && (m_wartime == false))
		{
			sWorld->SendWorldText(LANG_BG_WG_WORLD_ANNOUNCE_5_SEC);
			sLog->OutConsole("BATTLEGROUND MANAGER: Until The Battle of Wintergrasp Stay 5 Seconds!");
		}

		if ((m_timer> 4000) && (m_timer <5000) && (m_wartime == false))
		{
			sWorld->SendWorldText(LANG_BG_WG_WORLD_ANNOUNCE_4_SEC);
			sLog->OutConsole("BATTLEGROUND MANAGER: Until The Battle of Wintergrasp Stay 4 Seconds!");
		}

		if ((m_timer> 3000) && (m_timer <4000) && (m_wartime == false))
		{
			sWorld->SendWorldText(LANG_BG_WG_WORLD_ANNOUNCE_3_SEC);
			sLog->OutConsole("BATTLEGROUND MANAGER: Until The Battle of Wintergrasp Stay 3 Seconds!");
		}

		if ((m_timer> 2000) && (m_timer <3000) && (m_wartime == false))
		{
			sWorld->SendWorldText(LANG_BG_WG_WORLD_ANNOUNCE_2_SEC);
			sLog->OutConsole("BATTLEGROUND MANAGER: Until The Battle of Wintergrasp Stay 2 Seconds!");
		}

		if ((m_timer> 1000) && (m_timer <2000) && (m_wartime == false))
		{
			sWorld->SendWorldText(LANG_BG_WG_WORLD_ANNOUNCE_1_SEC);
			sLog->OutConsole("BATTLEGROUND MANAGER: Until The Battle of Wintergrasp Stay 1 Seconds!");
			sLog->OutConsole("BATTLEGROUND MANAGER: Battle For The Wintergrasp Started!");
		}
	}

	if (sWorld->getBoolConfig(CONFIG_OUTDOOR_PVP_WG_STATS_ENABLED))
    {
		uint32 team = 0;
		uint32 inWar = 0;

		if (IsWarTime())
			inWar = 1;
		else
			inWar = 0;

	   if (GetDefenderTeam() == TEAM_ALLIANCE)
		   team = 1;

	   else if (GetDefenderTeam() == TEAM_HORDE)
		   team = 2;

	   if (timer % sWorld->getIntConfig(CONFIG_OUTDOOR_PVP_WG_STATS_UPDATE_INTERVAL) == 0)
		   LoginDatabase.PExecute("REPLACE account_timer SET timer = '%u', team = %u, inWar = %u, id = %u", timer, team, inWar, sWorld->getIntConfig(CONFIG_OUTDOOR_PVP_WG_STATS_DATA_ID));
	}
}

bool OutdoorPvPWG::Update(uint32 diff)
{
	if (!sWorld->getBoolConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_ENABLED))
		return false;

	if (m_timer > diff)
	{
		m_timer -= diff;

		if (IsWarTime())
		{
			OutdoorPvP::Update(diff);

			if (KickDontAcceptTimer <= diff)
			{
				for(uint8 team = 0; team<2; ++team)
				{
					for (PlayerTimerMap::iterator itr = m_InvitedPlayers[team].begin(); itr != m_InvitedPlayers[team].end(); ++itr)
					{
						if ((*itr).second<=time(NULL))
							KickPlayerFromWintergrasp((*itr).first);
					}
				}

				for(uint8 team = 0; team<2; ++team)
				{
					for (PlayerTimerMap::iterator itr = m_PlayersWillBeKick[team].begin(); itr != m_PlayersWillBeKick[team].end(); ++itr)
					{
						if ((*itr).second <= time(NULL))
							KickPlayerFromWintergrasp((*itr).first);
					}
				}

				InvitePlayerInZoneToWar();

				KickDontAcceptTimer = 1*IN_MILLISECONDS;
			}
			else KickDontAcceptTimer -= diff;

			if (KickAfkTimer <= diff)
			{
				KickAfkTimer = 2*IN_MILLISECONDS;
				KickAfk();
			}
			else KickAfkTimer -= diff;

           /*********************************************************/
           /***      BATTLEGROUND VEHICLE TELEPORTER SYSTEM       ***/
           /*********************************************************/

           if (map)
           {
			   Map::PlayerList const &PlayerList = map->GetPlayers();
			   for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
               {
                   if (GetDefenderTeam() == TEAM_ALLIANCE)
                   {
                       if (i->getSource()->ToPlayer()->GetTeam() == ALLIANCE)
                       {
                           if (i->getSource()->GetDistance2d(5314.51f, 2703.69f) <= 5 && i->getSource()->IsOnVehicle(i->getSource()->GetVehicleCreatureBase()) && i->getSource()->IsAlive())
                           {
                               Creature* Old = i->getSource()->GetVehicleCreatureBase();
                               if (Old->GetEntry() == 28319) // Alliance Turret
                               {
                                   i->getSource()->ExitVehicle();
                                   i->getSource()->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                   Old->SetVisible(true);
                                   Old->ToTempSummon()->UnSummon();
                               }
                               else
                               {
								    Unit* Driver = Old->GetVehicleKit()->GetPassenger(0);
								    Unit* Passenger1 = Old->GetVehicleKit()->GetPassenger(1);
								    Unit* Passenger2 = Old->GetVehicleKit()->GetPassenger(2);
								    Unit* Passenger3 = Old->GetVehicleKit()->GetPassenger(3);

								    if (Driver && Driver->IsAlive())
								    {
                                       Creature* New = i->getSource()->SummonCreature(Old->GetEntry(), 5141.191406f, 2841.045410f, 408.703217f, 3.163321f, TEMPSUMMON_MANUAL_DESPAWN);
                                       New->SetPower(POWER_MANA, Old->GetPower(POWER_MANA));
                                       New->SetPower(POWER_RAGE, Old->GetPower(POWER_RAGE));
                                       New->SetPower(POWER_FOCUS, Old->GetPower(POWER_FOCUS));
                                       New->SetPower(POWER_ENERGY, Old->GetPower(POWER_ENERGY));
                                       New->SetHealth(Old->GetHealth());
                                       New->SetRespawnTime(Old->GetRespawnTime());
                                       Old->GetVehicleKit()->Uninstall();
                                       Old->SetVisible(true);
                                       Old->DespawnAfterAction();
                                       Vehicle* vehicle = New->GetVehicleKit();
								        Driver->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
								        Driver->_EnterVehicle(vehicle, 0);

								        if (Passenger1 && Passenger1->IsAlive())
								        {
								            Passenger1->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
								            //Passenger1->_EnterVehicle(vehicle, 1); Seatid doesn't work :S
								        }

								        if (Passenger2 && Passenger2->IsAlive())
								        {
								            Passenger2->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
								            //Passenger2->_EnterVehicle(vehicle, 2); Seatid doesn't work :S
								        }

								        if (Passenger3 && Passenger3->IsAlive())
								        {
								            Passenger3->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
								            //Passenger3->_EnterVehicle(vehicle, 3); Seatid doesn't work :S
								        }
								    }
                               }
                           }

                           if (i->getSource()->GetDistance2d(5316.25f, 2977.04f) <= 5 && i->getSource()->IsOnVehicle(i->getSource()->GetVehicleCreatureBase()))
                           {
							   Creature* Old = i->getSource()->GetVehicleCreatureBase();
                               if (Old->GetEntry() == 28319) // Alliance Turret
                               {
                                   i->getSource()->ExitVehicle();
                                   i->getSource()->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                   Old->SetVisible(true);
                                   Old->ToTempSummon()->UnSummon();
                               }
                               else
							   {
								   Unit* Driver = Old->GetVehicleKit()->GetPassenger(0);
								   Unit* Passenger1 = Old->GetVehicleKit()->GetPassenger(1);
								   Unit* Passenger2 = Old->GetVehicleKit()->GetPassenger(2);
								   Unit* Passenger3 = Old->GetVehicleKit()->GetPassenger(3);
								   if (Driver && Driver->IsAlive())
								   {
									   Creature* New = i->getSource()->SummonCreature(Old->GetEntry(), 5141.191406f, 2841.045410f, 408.703217f, 3.163321f, TEMPSUMMON_MANUAL_DESPAWN);
									   New->SetPower(POWER_MANA, Old->GetPower(POWER_MANA));
									   New->SetPower(POWER_RAGE, Old->GetPower(POWER_RAGE));
									   New->SetPower(POWER_FOCUS, Old->GetPower(POWER_FOCUS));
									   New->SetPower(POWER_ENERGY, Old->GetPower(POWER_ENERGY));
									   New->SetHealth(Old->GetHealth());
									   New->SetRespawnTime(Old->GetRespawnTime());
									   Old->GetVehicleKit()->Uninstall();
									   Old->SetVisible(true);
									   Old->DespawnAfterAction();
									   Vehicle* vehicle = New->GetVehicleKit();
									   Driver->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
									   Driver->_EnterVehicle(vehicle, 0);
									   if (Passenger1 && Passenger1->IsAlive())
									   {
										   Passenger1->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
										   //Passenger1->_EnterVehicle(vehicle, 1); Seatid doesn't work :S
									   }
									   if (Passenger2 && Passenger2->IsAlive())
									   {
										   Passenger2->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
										   //Passenger2->_EnterVehicle(vehicle, 2); Seatid doesn't work :S
									   }
									   if (Passenger3 && Passenger3->IsAlive())
									   {
										   Passenger3->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
										   //Passenger3->_EnterVehicle(vehicle, 3); Seatid doesn't work :S
									   }
								   }
                               }
                           }
                       }
                   }
                   else
                   {
                       if (i->getSource()->ToPlayer()->GetTeam() == HORDE)
                       {
                           if (i->getSource()->GetDistance2d(5314.51f, 2703.69f) <= 5 && i->getSource()->IsOnVehicle(i->getSource()->GetVehicleCreatureBase()) && i->getSource()->IsAlive())
                           {
                               Creature* Old = i->getSource()->GetVehicleCreatureBase();
                               if (Old->GetEntry() == 32629) // Horde Turret
                               {
                                   i->getSource()->ExitVehicle();
                                   i->getSource()->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                   Old->SetVisible(true);
                                   Old->ToTempSummon()->UnSummon();
                               }
                               else
                               {
								   Unit* Driver = Old->GetVehicleKit()->GetPassenger(0);
								   Unit* Passenger1 = Old->GetVehicleKit()->GetPassenger(1);
								   Unit* Passenger2 = Old->GetVehicleKit()->GetPassenger(2);
								   Unit* Passenger3 = Old->GetVehicleKit()->GetPassenger(3);
								   if (Driver && Driver->IsAlive())
								   {
									   Creature* New = i->getSource()->SummonCreature(Old->GetEntry(), 5141.191406f, 2841.045410f, 408.703217f, 3.163321f, TEMPSUMMON_MANUAL_DESPAWN);
									   New->SetPower(POWER_MANA, Old->GetPower(POWER_MANA));
									   New->SetPower(POWER_RAGE, Old->GetPower(POWER_RAGE));
									   New->SetPower(POWER_FOCUS, Old->GetPower(POWER_FOCUS));
									   New->SetPower(POWER_ENERGY, Old->GetPower(POWER_ENERGY));
									   New->SetHealth(Old->GetHealth());
									   New->SetRespawnTime(Old->GetRespawnTime());
									   Old->GetVehicleKit()->Uninstall();
									   Old->SetVisible(true);
									   Old->DespawnAfterAction();
									   Vehicle* vehicle = New->GetVehicleKit();
									   Driver->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
									   Driver->_EnterVehicle(vehicle, 0);
									   if (Passenger1 && Passenger1->IsAlive())
									   {
										   Passenger1->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
										   //Passenger1->_EnterVehicle(vehicle, 1); Seatid doesn't work :S
									   }
									   if (Passenger2 && Passenger2->IsAlive())
									   {
										   Passenger2->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
										   //Passenger2->_EnterVehicle(vehicle, 2); Seatid doesn't work :S
									   }
									   if (Passenger3 && Passenger3->IsAlive())
									   {
										   Passenger3->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
										   //Passenger3->_EnterVehicle(vehicle, 3); Seatid doesn't work :S
									   }
								   }
							   }
                           }
                           if (i->getSource()->GetDistance2d(5316.25f, 2977.04f) <= 5 && i->getSource()->IsOnVehicle(i->getSource()->GetVehicleCreatureBase()))
                           {
                               Creature* Old = i->getSource()->GetVehicleCreatureBase();
                               if (Old->GetEntry() == 32629) // Horde Turret
                               {
                                   i->getSource()->ExitVehicle();
                                   i->getSource()->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
                                   Old->SetVisible(true);
                                   Old->ToTempSummon()->UnSummon();
                               }
                               else
                               {
								   Unit* Driver = Old->GetVehicleKit()->GetPassenger(0);
								   Unit* Passenger1 = Old->GetVehicleKit()->GetPassenger(1);
								   Unit* Passenger2 = Old->GetVehicleKit()->GetPassenger(2);
								   Unit* Passenger3 = Old->GetVehicleKit()->GetPassenger(3);
								   if (Driver && Driver->IsAlive())
								   {
									   Creature* New = i->getSource()->SummonCreature(Old->GetEntry(), 5141.191406f, 2841.045410f, 408.703217f, 3.163321f, TEMPSUMMON_MANUAL_DESPAWN);
									   New->SetPower(POWER_MANA, Old->GetPower(POWER_MANA));
									   New->SetPower(POWER_RAGE, Old->GetPower(POWER_RAGE));
									   New->SetPower(POWER_FOCUS, Old->GetPower(POWER_FOCUS));
									   New->SetPower(POWER_ENERGY, Old->GetPower(POWER_ENERGY));
									   New->SetHealth(Old->GetHealth());
									   New->SetRespawnTime(Old->GetRespawnTime());
									   Old->GetVehicleKit()->Uninstall();
									   Old->SetVisible(true);
									   Old->DespawnAfterAction();
									   Vehicle* vehicle = New->GetVehicleKit();
									   Driver->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
									   Driver->_EnterVehicle(vehicle, 0);
									   if (Passenger1 && Passenger1->IsAlive())
									   {
										   Passenger1->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
										   //Passenger1->_EnterVehicle(vehicle, 1); Seatid doesn't work :S
									   }
									   if (Passenger2 && Passenger2->IsAlive())
									   {
										   Passenger2->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
										   //Passenger2->_EnterVehicle(vehicle, 2); Seatid doesn't work :S
									   }
									   if (Passenger3 && Passenger3->IsAlive())
									   {
										   Passenger3->NearTeleportTo(5141.191406f, 2841.045410f, 408.703217f, 3.163321f, true); // Out of the Fortress Gate
										   //Passenger3->_EnterVehicle(vehicle, 3); Seatid doesn't work :S
									   }
								   }
							   }
                           }
                       }
                   }
               }
           }

           /*********************************************************/
           /***        BATTLEGROUND RESSURECTION SYSTEM           ***/
           /*********************************************************/

           // this should be handled by spell system
           LastResurrectTime += diff;
           if (LastResurrectTime >= RESURRECTION_INTERVAL)
           {
               if (GetReviveQueueSize())
               {
                   for (std::map<uint64, std::vector<uint64> >::iterator itr = m_ReviveQueue.begin(); itr != m_ReviveQueue.end(); ++itr)
                   {
                       Creature* sh = NULL;
                       for (std::vector<uint64>::const_iterator itr2 = (itr->second).begin(); itr2 != (itr->second).end(); ++itr2)
                       {
                           Player* player = ObjectAccessor::FindPlayer(*itr2);
                           if (!player)
                               continue;

                           if (!sh && player->IsInWorld())
                           {
                               sh = player->GetMap()->GetCreature(itr->first);

                               if (sh)
								   sh->CastSpell(sh, SPELL_SPIRIT_HEAL, true);
						   }

						   if (sh)
						   {
							   if (player->GetDistance2d(sh) <= 18.0f)
							   {
								   player->CastSpell(player, SPELL_RESURRECTION_VISUAL, true);
								   m_ResurrectQueue.push_back(*itr2);
							   }
						   }
					   }
					   (itr->second).clear();
                   }

                   m_ReviveQueue.clear();
                   LastResurrectTime = 0;
               }
               else LastResurrectTime = 0;
           }
           else if (LastResurrectTime > 500)
           {
               for (std::vector<uint64>::const_iterator itr = m_ResurrectQueue.begin(); itr != m_ResurrectQueue.end(); ++itr)
               {
                   Player* player = ObjectAccessor::FindPlayer(*itr);
                   if (!player)
                       continue;

                   player->ResurrectPlayer(1.0f);
                   player->CastSpell(player, 6962, true);
                   player->CastSpell(player, SPELL_SPIRIT_HEAL_MANA, true);
                   sObjectAccessor->ConvertCorpseForPlayer(*itr);
               }
               m_ResurrectQueue.clear();
           }
       }
       UpdateClock();
   }
   else
   {
       m_sendUpdate = false;
       int32 entry = LANG_BG_WG_DEFENDED;

       if (m_changeDefender)
       {
           m_changeDefender = false;
           m_defender = GetAttackerTeam();
           entry = LANG_BG_WG_CAPTURED;
       }

       if (IsWarTime())
       {
           if (m_timer != 1) // 1 = ForceStopBattle
               sWorld->SendZoneText(ZONE_WINTERGRASP, fmtstring(sObjectMgr->GetQuantumSystemTextForDBCLocale(entry), sObjectMgr->GetQuantumSystemTextForDBCLocale(GetDefenderTeam() == TEAM_ALLIANCE ? LANG_BG_AB_ALLY : LANG_BG_AB_HORDE)));
           EndBattle();
       }
       else
       {
           if (m_timer != 1) // 1 = ForceStartBattle
               sWorld->SendZoneText(ZONE_WINTERGRASP, sObjectMgr->GetQuantumSystemTextForDBCLocale(LANG_BG_WG_BATTLE_STARTS));
           StartBattle();
       }

       UpdateAllWorldObject();
       UpdateClock();
       SendInitWorldStatesTo();
       m_sendUpdate = true;
   }

   if (WorldStateSaveTimer <= diff)
   {
	   CharacterDatabase.PExecute("UPDATE worldstates SET value = %d, comment = 'Wintergrasp Wartime' WHERE entry = 31001", m_wartime);
       CharacterDatabase.PExecute("UPDATE worldstates SET value = %d, comment = 'Wintergrasp Timer' WHERE entry = 31002", m_timer);
       CharacterDatabase.PExecute("UPDATE worldstates SET value = %d, comment = 'Wintergrasp Defender' WHERE entry = 31003", m_defender);

       WorldStateSaveTimer = sWorld->getIntConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_SAVE_STATE_PERIOD);
   }
   else WorldStateSaveTimer -= diff;

   // Some times before battle start invite player to queue
   if (!IsWarTime())
       if (!m_StartGrouping && m_timer <= StartGroupingTimer)
       {
           m_StartGrouping = true;
           InvitePlayerInZoneToQueue();
       }
	   return false;
}

void OutdoorPvPWG::ForceStartBattle()
{ 
	OutdoorPvPWG* pvpWG = (OutdoorPvPWG*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(ZONE_WINTERGRASP);

	m_wartime = false;

	if (m_timer != 1)
	{
		m_timer = 1;
		sWorld->SendWorldText(LANG_BG_WG_BATTLE_FORCE_START);
	}
	sWorld->SendWintergraspState();

	if (sWorld->getBoolConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_STATISTIC_ENABLE))
	{
		sWorld->SendWorldText(LANG_BG_WG_STATUS, sObjectMgr->GetQuantumSystemTextForDBCLocale(
		pvpWG->GetDefenderTeam() == TEAM_ALLIANCE ? LANG_BG_AB_ALLY : LANG_BG_AB_HORDE),
		secsToTimeString(pvpWG->GetTimer(), true).c_str(),
		pvpWG->IsWarTime() ? "Yes" : "No",
		pvpWG->GetNumPlayersH(),
		pvpWG->GetNumPlayersA());
	}
}

void OutdoorPvPWG::ForceStopBattle()
{ 
   if (!IsWarTime())
       m_wartime = true;

   if (m_timer != 1)
   {
       m_timer = 1;
       sWorld->SendWorldText(LANG_BG_WG_BATTLE_FORCE_STOP);
   }
	sWorld->SendWintergraspState();
}

void OutdoorPvPWG::ForceChangeTeam()
{
   m_changeDefender = true;
   m_timer = 1;
   sWorld->SendZoneText(ZONE_WINTERGRASP, fmtstring(sObjectMgr->GetQuantumSystemTextForDBCLocale(LANG_BG_WG_SWITCH_FACTION), sObjectMgr->GetQuantumSystemTextForDBCLocale(GetAttackerTeam() == TEAM_ALLIANCE ? LANG_BG_AB_ALLY : LANG_BG_AB_HORDE)));
   if (IsWarTime())
       ForceStartBattle();
   else
       ForceStopBattle();
}

// Can be forced by gm's while in battle so have to reset in case it was wartime
void OutdoorPvPWG::StartBattle()
{
   uint32 CountDef=0;
   uint32 CountAtk=0;

   m_wartime = true;
   m_timer = sWorld->getIntConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_BATTLE_TIME) *MINUTE*IN_MILLISECONDS;

   InvitePlayerInQueueToWar();

   /*  
   for (PlayerSet::iterator itr = m_players[GetDefenderTeam()].begin(); itr != m_players[GetDefenderTeam()].end(); ++itr)
   {
       if ((*itr) && (*itr)->GetCurrentLevel() < minlevel)
       {
          (*itr)->CastSpell((*itr), SPELL_TELEPORT_DALARAN, true);
       }
   }

   for (PlayerSet::iterator itr = m_players[GetAttackerTeam()].begin(); itr != m_players[GetAttackerTeam()].end(); ++itr)
   {
       if ((*itr) && (*itr)->GetCurrentLevel() < minlevel)
       {
         (*itr)->CastSpell((*itr), SPELL_TELEPORT_DALARAN, true);
       }
   } 
   */

   if (sWorld->getBoolConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_ANTIFARM_ENABLE))
   {
	   if ((CountAtk < sWorld->getIntConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_ANTIFARM_ATK)) || (CountDef < sWorld->getIntConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_ANTIFARM_DEF)))
	   {
		   if (CountAtk <= CountDef)
			   sWorld->SendWorldText(LANG_BG_WG_WORLD_NO_ATK);

		   if (CountDef<CountAtk)
		   {  
			   sWorld->SendWorldText(LANG_BG_WG_WORLD_NO_DEF);
			   m_changeDefender = true;
		   }
		   ForceStopBattle();
		   return;
	   }
   }

   //TeamCastSpell(GetDefenderTeam(), SPELL_TELEPORT_FORTRESS);

   // Remove Essence of Wintergrasp to all players
   sWorld->setWorldState(WORLDSTATE_WINTERGRASP_CONTROLING_FACTION, TEAM_NEUTRAL);
   sWorld->UpdateAreaDependentAuras();

   // Destroy all vehicle
   for (uint8 team = 0; team < 2; ++team)
   {
	   for (PlayerSet::iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
	   {
		   (*itr)->ExitVehicle();

		   if (Creature* veh = (*itr)->FindCreatureWithDistance(27881, 550.0f, true))
		   {
			   veh->CastSpell(veh, SPELL_DESTROY_VEHICLE, true);
			   veh->DespawnAfterAction(12000);
		   }

		   if (Creature* veh = (*itr)->FindCreatureWithDistance(28094, 550.0f, true))
		   {
			   veh->CastSpell(veh, SPELL_DESTROY_VEHICLE, true);
			   veh->DespawnAfterAction(12000);
		   }

		   if (Creature* veh = (*itr)->FindCreatureWithDistance(28312, 550.0f, true))
		   {
			   veh->CastSpell(veh, SPELL_DESTROY_VEHICLE, true);
			   veh->DespawnAfterAction(12000);
		   }

		   if (Creature* veh = (*itr)->FindCreatureWithDistance(32627, 550.0f, true))
		   {
			   veh->CastSpell(veh, SPELL_DESTROY_VEHICLE, true);
			   veh->DespawnAfterAction(12000);
		   }
	   }
   }

   UpdateTenacityStack();
   sWorld->SendWintergraspState();
}

void OutdoorPvPWG::EndBattle()
{
   // Cast Essence of Wintergrasp to all players (CheckCast will determine who to cast)
   sWorld->setWorldState(WORLDSTATE_WINTERGRASP_CONTROLING_FACTION, GetDefenderTeam());
   sWorld->UpdateAreaDependentAuras();
   // Sound on End Battle
   for (PlayerSet::iterator itr = m_players[GetDefenderTeam()].begin(); itr != m_players[GetDefenderTeam()].end(); ++itr)
   {
	   if (GetDefenderTeam() == TEAM_ALLIANCE)
		   TeamIDsound = OUTDOOR_PVP_WG_SOUND_ALLIANCE_WINS; //Alliance Win

       else TeamIDsound = OUTDOOR_PVP_WG_SOUND_HORDE_WINS;  //Horde Win

	   (*itr)->PlayDirectSound(TeamIDsound); // SoundOnEndWin
   }

   for (PlayerSet::iterator itr = m_players[GetAttackerTeam()].begin(); itr != m_players[GetAttackerTeam()].end(); ++itr)
      (*itr)->PlayDirectSound(OUTDOOR_PVP_WG_SOUND_NEAR_VICTORY); // SoundOnEndLoose

   for (uint8 team = 0; team < 2; ++team)
   {
	   // Destroy all vehicle
	   for (PlayerSet::iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
	   {
		   (*itr)->ExitVehicle();

		   if (Creature* veh = (*itr)->FindCreatureWithDistance(27881, 550.0f, true))
		   {
			   veh->CastSpell(veh, SPELL_DESTROY_VEHICLE, true);
			   veh->DespawnAfterAction(12000);
		   }

		   if (Creature* veh = (*itr)->FindCreatureWithDistance(28094, 550.0f, true))
		   {
			   veh->CastSpell(veh, SPELL_DESTROY_VEHICLE, true);
			   veh->DespawnAfterAction(12000);
		   }

		   if (Creature* veh = (*itr)->FindCreatureWithDistance(28312, 550.0f, true))
		   {
			   veh->CastSpell(veh, SPELL_DESTROY_VEHICLE, true);
			   veh->DespawnAfterAction(12000);
		   }

		   if (Creature* veh = (*itr)->FindCreatureWithDistance(32627, 550.0f, true))
		   {
			   veh->CastSpell(veh, SPELL_DESTROY_VEHICLE, true);
			   veh->DespawnAfterAction(12000);
		   }
	   }

       if (m_PlayersInWar[team].empty())
           continue;

       for (PlayerSet::iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
       {
           // When WG ends the zone is cleaned including corpses, revive all players if dead
           if ((*itr)->IsDead())
           {
			   (*itr)->ResurrectPlayer(1.0f);
			   (*itr)->CastSpell((*itr), 6962, true);
			   (*itr)->CastSpell((*itr), SPELL_SPIRIT_HEAL_MANA, true);
			   sObjectAccessor->ConvertCorpseForPlayer((*itr)->GetGUID());
           }

           (*itr)->RemoveAurasDueToSpell(SPELL_TENACITY);
           (*itr)->CombatStop(true);
           (*itr)->getHostileRefManager().deleteReferences();
       }

       if (m_timer == 1) // Battle End was forced so no reward.
       {
           for (PlayerSet::iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
           {
               (*itr)->RemoveAurasDueToSpell(SPELL_RECRUIT);
               (*itr)->RemoveAurasDueToSpell(SPELL_CORPORAL);
               (*itr)->RemoveAurasDueToSpell(SPELL_LIEUTENANT);
               (*itr)->RemoveAurasDueToSpell(SPELL_TOWER_CONTROL);
               (*itr)->RemoveAurasDueToSpell(SPELL_SPIRITUAL_IMMUNITY);
           }
           continue;
       }

       // calculate rewards
       uint32 intactNum = 0;
       uint32 damagedNum = 0;
       for (OutdoorPvP::OPvPCapturePointMap::const_iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
	   {
           if (OPvPCapturePointWG* workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
		   {
               if (workshop->m_buildingState->GetTeam() == team)
			   {
                   if (workshop->m_buildingState->damageState == DAMAGE_DAMAGED)
                       ++damagedNum;

                   else if (workshop->m_buildingState->damageState == DAMAGE_INTACT)
                       ++intactNum;
			   }
		   }
	   }

       uint32 spellRewardId = team == GetDefenderTeam() ? SPELL_WINTERGRASP_VICTORY : SPELL_WINTERGRASP_DEFEAT;
       uint32 baseHonor = 0;
       uint32 marks = 0;
       uint32 playersWithRankNum = 0;
       uint32 honor = 0;

       if (sWorld->getBoolConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_CUSTOM_HONOR))
       {
           // Calculate Level 70+ with Corporal or Lieutenant rank
           for (PlayerSet::iterator itr = m_PlayersInWar[team].begin(); itr != m_PlayersInWar[team].end(); ++itr)
		   {
               if ((*itr)->GetCurrentLevel() > minlevel && ((*itr)->HasAura(SPELL_LIEUTENANT) || (*itr)->HasAura(SPELL_CORPORAL)))
                   ++playersWithRankNum;
		   }

           baseHonor = team == GetDefenderTeam() ? sWorld->getIntConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_WIN_BATTLE) : sWorld->getIntConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_LOSE_BATTLE);
           baseHonor += (sWorld->getIntConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_DAMAGED_TOWER)* m_towerDamagedCount[OTHER_TEAM(team)]);
           baseHonor += (sWorld->getIntConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_DESTROYED_TOWER)* m_towerDestroyedCount[OTHER_TEAM(team)]);
           baseHonor += (sWorld->getIntConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_INTACT_BUILDING)* intactNum);
           baseHonor += (sWorld->getIntConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_DAMAGED_BUILDING)* damagedNum);
           if (playersWithRankNum)
               baseHonor /= playersWithRankNum;
       }

       for (PlayerSet::iterator itr = m_PlayersInWar[team].begin(); itr != m_PlayersInWar[team].end(); ++itr)
       {
           if (!(*itr))
               continue;

           if ((*itr)->GetCurrentLevel() < minlevel)
               continue; // No rewards for level <75

           // give rewards
           if (sWorld->getBoolConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_CUSTOM_HONOR))
           {
               if (team == GetDefenderTeam())
               {
                   if ((*itr)->HasAura(SPELL_LIEUTENANT))
                   {
                       marks = 3;
                       honor = baseHonor;
                   }
                   else if ((*itr)->HasAura(SPELL_CORPORAL))
                   {
                       marks = 2;
                       honor = baseHonor;
                   }
                   else
                   {
                       marks = 1;
                       honor = 0;
                   }
               }
               else
               {
                   if ((*itr)->HasAura(SPELL_LIEUTENANT))
                   {
                       marks = 1;
                       honor = baseHonor;
                   }
                   else if ((*itr)->HasAura(SPELL_CORPORAL))
                   {
                       marks = 1;
                       honor = baseHonor;
                   }
                   else
                   {
                       marks = 0;
                       honor = 0;
                   }
               }
               (*itr)->RewardHonor(NULL, 1, honor);
               RewardMarkOfHonor(*itr, marks);
               (*itr)->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, spellRewardId);
           }
           else
           {
               if ((*itr)->HasAura(SPELL_LIEUTENANT) || (*itr)->HasAura(SPELL_CORPORAL))
               {
                   // TODO - Honor from SpellReward should be shared by team players
                   // TODO - Marks should be given depending on Rank but 3 are given
                   // each time so Won't give any to recruits
                   (*itr)->CastSpell(*itr, spellRewardId, true);
                   for (uint32 i = 0; i < intactNum; ++i)
                       (*itr)->CastSpell(*itr, SPELL_INTACT_BUILDING, true);
                   for (uint32 i = 0; i < damagedNum; ++i)
                       (*itr)->CastSpell(*itr, SPELL_DAMAGED_BUILDING, true);
                   for (uint32 i = 0; i < m_towerDamagedCount[OTHER_TEAM(team)]; ++i)
                       (*itr)->CastSpell(*itr, SPELL_DAMAGED_TOWER, true);
                   for (uint32 i = 0; i < m_towerDestroyedCount[OTHER_TEAM(team)]; ++i)
                       (*itr)->CastSpell(*itr, SPELL_DESTROYED_TOWER, true);
               }
           }
           if (team == GetDefenderTeam())
           {
               if ((*itr)->HasAura(SPELL_LIEUTENANT) || (*itr)->HasAura(SPELL_CORPORAL))
               {
                   IncrementQuest((*itr), QUEST_VICTORY_IN_WG_A, true);
                   IncrementQuest((*itr), QUEST_VICTORY_IN_WG_H, true);
               }
           }
           (*itr)->RemoveAurasDueToSpell(SPELL_RECRUIT);
           (*itr)->RemoveAurasDueToSpell(SPELL_CORPORAL);
           (*itr)->RemoveAurasDueToSpell(SPELL_LIEUTENANT);
           (*itr)->RemoveAurasDueToSpell(SPELL_TOWER_CONTROL);
           (*itr)->RemoveAurasDueToSpell(SPELL_SPIRITUAL_IMMUNITY);
       }
   }

   m_wartime = false;
   m_timer = sWorld->getIntConfig(CONFIG_OUTDOOR_PVP_WINTERGRASP_INTERVAL) *MINUTE*IN_MILLISECONDS;
   RemoveOfflinePlayerWGAuras();
   // Update timer in players battlegrounds tab
   sWorld->SendWintergraspState();

   for (uint8 team = 0; team < 2; ++team)
   {
       for(GuidSet::const_iterator itr=m_Groups[team].begin();itr!=m_Groups[team].end();++itr)
       {
           if (Group* group = sGroupMgr->GetGroupByGUID(*itr))
           {
              group->Disband();
              group->SetBattlegroundGroup(NULL);
           }
       }
   }

   for (uint8 team = 0; team < 2; ++team)
   {
       m_PlayersInQueue[team].clear();
       m_PlayersInWar[team].clear();
       m_InvitedPlayers[team].clear();
       m_PlayersWillBeKick[team].clear();
       m_Groups[team].clear();
   }

   m_StartGrouping = false;
}

bool OutdoorPvPWG::CanBuildVehicle(OPvPCapturePointWG* workshop) const
{
   TeamId team = workshop->m_buildingState->GetTeam();
   if (team == TEAM_NEUTRAL)
       return false;

   return IsWarTime() && workshop->m_buildingState->damageState != DAMAGE_DESTROYED && vehicles[team].size() < m_workshopCount[team]* MAX_VEHICLE_PER_WORKSHOP;
}

uint32 OutdoorPvPWG::GetData(uint32 id)
{
   // if can build more vehicles
   if (OPvPCapturePointWG* workshop = GetWorkshopByEngGuid(id))
       return CanBuildVehicle(workshop) ? 1 : 0;

   return 0;
}

void OutdoorPvPWG::RewardMarkOfHonor(Player* player, uint32 count)
{
   if (player->HasAura(SPELL_AURA_PLAYER_INACTIVE))
       return;

   if (count == 0)
       return;

   ItemPosCountVec dest;
   uint32 no_space_count = 0;
   uint8 msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, WG_MARK_OF_HONOR, count, &no_space_count);

   if (msg == EQUIP_ERR_ITEM_NOT_FOUND)
   {
       sLog->OutErrorDatabase("Wintergrasp reward item (Entry %u) not exist in `item_template`.", WG_MARK_OF_HONOR);
       return;
   }

   if (msg != EQUIP_ERR_OK)
       count -= no_space_count;

   if (count != 0 && !dest.empty()) 
       if (Item* item = player->StoreNewItem(dest, WG_MARK_OF_HONOR, true, 0))
           player->SendNewItem(item, count, true, false);
}

void OutdoorPvPWG::LoadQuestGiverMap(uint32 guid, Position PosHorde, Position PosAlliance)
{
   m_qgPosMap[std::pair<uint32, bool>(guid, true)] = PosHorde,
   m_qgPosMap[std::pair<uint32, bool>(guid, false)] = PosAlliance,
   m_questgivers[guid] = NULL;

   if (GetDefenderTeam() == TEAM_ALLIANCE)
       sObjectMgr->MoveCreatureData(guid, 571, PosAlliance);
}

OPvPCapturePointWG* OutdoorPvPWG::GetWorkshop(uint32 lowguid) const
{
   if (OPvPCapturePoint* creaturePoint = GetCapturePoint(lowguid))
       return dynamic_cast<OPvPCapturePointWG*>(creaturePoint);
   return NULL;
}

OPvPCapturePointWG* OutdoorPvPWG::GetWorkshopByEngGuid(uint32 lowguid) const
{
   for (OutdoorPvP::OPvPCapturePointMap::const_iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
       if (OPvPCapturePointWG* workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
           if (workshop->m_engGuid == lowguid)
               return workshop;
   return NULL;
}

OPvPCapturePointWG* OutdoorPvPWG::GetWorkshopByGOGuid(uint64 lowguid) const
{
   for (OutdoorPvP::OPvPCapturePointMap::const_iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
       if (OPvPCapturePointWG* workshop = dynamic_cast<OPvPCapturePointWG*>(itr->second))
           if (workshop->m_workshopGuid == lowguid)
               return workshop;
   return NULL;
}

/*########################################################
* Copy of Battleground system to make Spirit Guides Work
*#######################################################*/

void OutdoorPvPWG::SendAreaSpiritHealerQueryOpcode(Player* player, const uint64& guid)
{
   WorldPacket data(SMSG_AREA_SPIRIT_HEALER_TIME, 12);
   uint32 time_ = 30000 - GetLastResurrectTime();
   if (time_ == uint32(-1))
       time_ = 0;
   data << guid << time_;
   player->GetSession()->SendPacket(&data);
}

void OutdoorPvPWG::AddPlayerToResurrectQueue(uint64 npc_guid, uint64 player_guid)
{
   m_ReviveQueue[npc_guid].push_back(player_guid);

   Player* player = ObjectAccessor::FindPlayer(player_guid);

   if (!player)
       return;

   player->CastSpell(player, SPELL_WAITING_FOR_RESURRECT, true);
}

void OutdoorPvPWG::RemovePlayerFromResurrectQueue(uint64 player_guid)
{
   for (std::map<uint64, std::vector<uint64> >::iterator itr = m_ReviveQueue.begin(); itr != m_ReviveQueue.end(); ++itr)
   {
       for (std::vector<uint64>::iterator itr2 =(itr->second).begin(); itr2 != (itr->second).end(); ++itr2)
       {
           if (*itr2 == player_guid)
           {
               (itr->second).erase(itr2);

               Player* player = ObjectAccessor::FindPlayer(player_guid);

               if (!player)
                   return;

               player->RemoveAurasDueToSpell(SPELL_WAITING_FOR_RESURRECT);
               return;
           }
       }
   }
}

void OutdoorPvPWG::RelocateAllianceDeadPlayers(Creature* creature)
{
	if (!creature || GetCreatureType(creature->GetEntry()) != CREATURE_SPIRIT_GUIDE)
		return;

	// Those who are waiting to resurrect at this node are taken to the closest own node's graveyard
	std::vector<uint64> ghost_list = m_ReviveQueue[creature->GetGUID()];
	if (!ghost_list.empty())
	{
		WorldSafeLocsEntry const* ClosestGrave = NULL;
		for (std::vector<uint64>::const_iterator itr = ghost_list.begin(); itr != ghost_list.end(); ++itr)
		{
			Player* player = ObjectAccessor::FindPlayer(*itr);

			if (!player)
				continue;

			if (player->GetFaction() == ALLIANCE)
			{
				if (!ClosestGrave)
					ClosestGrave = sObjectMgr->GetClosestGraveYard(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetMapId(), player->GetTeam());

				if (ClosestGrave)
					player->TeleportTo(player->GetMapId(), ClosestGrave->x, ClosestGrave->y, ClosestGrave->z, player->GetOrientation());
			}
		}
	}
}

void OutdoorPvPWG::RelocateHordeDeadPlayers(Creature* creature)
{
   if (!creature || GetCreatureType(creature->GetEntry()) != CREATURE_SPIRIT_GUIDE)
       return;

   // Those who are waiting to resurrect at this node are taken to the closest own node's graveyard
   std::vector<uint64> ghost_list = m_ReviveQueue[creature->GetGUID()];
   if (!ghost_list.empty())
   {
       WorldSafeLocsEntry const* ClosestGrave = NULL;
       for (std::vector<uint64>::const_iterator itr = ghost_list.begin(); itr != ghost_list.end(); ++itr)
       {
           Player* player = ObjectAccessor::FindPlayer(*itr);

			if (!player)
				continue;

			if (player->GetFaction() == HORDE)
			{
				if (!ClosestGrave)
					ClosestGrave = sObjectMgr->GetClosestGraveYard(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetMapId(), player->GetTeam());

				if (ClosestGrave)
					player->TeleportTo(player->GetMapId(), ClosestGrave->x, ClosestGrave->y, ClosestGrave->z, player->GetOrientation());
			}
	   }
   }
}

void OutdoorPvPWG::KickAfk()
{
   for (uint8 team = 0; team < 2; ++team)
   {
       for (PlayerSet::const_iterator itr = m_PlayersInWar[team].begin(); itr != m_PlayersInWar[team].end(); ++itr)
       {
           if ((*itr) && (*itr)->IsAFK())
		   {
			   (*itr)->RemoveMount();
               (*itr)->TeleportTo(571, 5758.83f, 583.86f, 649.518f, 0.827114f);
		   }
       }
   }
}

void OutdoorPvPWG::KickPlayerFromWintergrasp(uint64 guid)
{
   if (Player* player = sObjectAccessor->FindPlayer(guid))
   {
       if (player->GetZoneId() == ZONE_WINTERGRASP)
	   {
		   player->RemoveMount();
		   player->TeleportTo(571, 5758.83f, 583.86f, 649.518f, 0.827114f);
	   }
   }
}

void OutdoorPvPWG::PlayerAskToLeave(Player* player)
{
   m_PlayersInQueue[player->GetTeamId()].erase(player->GetGUID());
   player->GetSession()->SendBfLeaveMessage(1);
   //KickPlayerFromWintergrasp(player->GetGUID());
}

void OutdoorPvPWG::InvitePlayerToWar(Player* player)
{
   if (player->GetCurrentLevel() < minlevel)
   {
	   if (!m_PlayersWillBeKick[player->GetTeamId()].count(player->GetGUID()))
           m_PlayersWillBeKick[player->GetTeamId()][player->GetGUID()] = time(NULL)+10;
       return;
   }

   // Check if player is not already in war
   if (m_PlayersInWar[player->GetTeamId()].count(player) || m_InvitedPlayers[player->GetTeamId()].count(player->GetGUID()))
       return;

   // player is battleground queue
   if (player->InBattlegroundQueue())
   {
       WorldPacket data;
       sBattlegroundMgr->BuildGroupJoinedBattlegroundPacket(&data, ERR_BATTLEGROUND_TOO_MANY_QUEUES);
       player->GetSession()->SendPacket(&data);
       return;
   }

   // player is using dungeon finder or raid finder
   if (player->isUsingLfg())
   {
       WorldPacket data;
       sBattlegroundMgr->BuildGroupJoinedBattlegroundPacket(&data, ERR_LFG_CANT_USE_BATTLEGROUND);
       player->GetSession()->SendPacket(&data);
       return;
   }

   m_PlayersWillBeKick[player->GetTeamId()].erase(player->GetGUID());
   m_InvitedPlayers[player->GetTeamId()][player->GetGUID()] = time(NULL) + m_TimeForAcceptInvite;
   player->GetSession()->SendBfInvitePlayerToWar(1, ZONE_WINTERGRASP, m_TimeForAcceptInvite);
}

void OutdoorPvPWG::InvitePlayerInZoneToQueue()
{    
   for (uint8 team = 0; team < 2; ++team)
   {
       for (PlayerSet::const_iterator itr = m_players[team].begin(); itr != m_players[team].end(); ++itr)
		   InvitePlayerToQueue((*itr));
   }
}

void OutdoorPvPWG::InvitePlayerToQueue(Player* player)
{   
   if (m_PlayersInQueue[player->GetTeamId()].count(player->GetGUID()))
       return;

   if (player->InBattlegroundQueue())
   {
       WorldPacket data;
       sBattlegroundMgr->BuildGroupJoinedBattlegroundPacket(&data, ERR_BATTLEGROUND_TOO_MANY_QUEUES);
       player->GetSession()->SendPacket(&data);
       return;
   }

   // player is using dungeon finder or raid finder
   if (player->isUsingLfg())
   {
       WorldPacket data;
       sBattlegroundMgr->BuildGroupJoinedBattlegroundPacket(&data, ERR_LFG_CANT_USE_BATTLEGROUND);
       player->GetSession()->SendPacket(&data);
       return;
   }
       
   if (player->GetCurrentLevel() > minlevel)
       player->GetSession()->SendBfInvitePlayerToQueue(1);
}

void OutdoorPvPWG::InvitePlayerInZoneToWar()
{
   for (uint8 team = 0; team < 2; ++team)
   {
       for (GuidSet::const_iterator itr = m_PlayersInQueue[team].begin(); itr != m_PlayersInQueue[team].end(); ++itr)
       {         
           if (Player* player = sObjectAccessor->FindPlayer(*itr))
           {
               if (m_PlayersInWar[player->GetTeamId()].count(player) || m_InvitedPlayers[player->GetTeamId()].count(player->GetGUID()))
                   continue;

               if (m_PlayersInWar[player->GetTeamId()].size()+m_InvitedPlayers[player->GetTeamId()].size()< m_MaxPlayer)
                   InvitePlayerToWar(player);
               else
               {
                   if (!m_PlayersWillBeKick[player->GetTeamId()].count(player->GetGUID()))
                        m_PlayersWillBeKick[player->GetTeamId()][player->GetGUID()] = time(NULL)+10;
               }
           }
       }
   }
}

void OutdoorPvPWG::InvitePlayerInQueueToWar()
{    
   for (uint8 team = 0; team < 2; ++team)
   {
       for (GuidSet::const_iterator itr = m_PlayersInQueue[team].begin(); itr != m_PlayersInQueue[team].end(); ++itr)
       {         
           if (Player* player = sObjectAccessor->FindPlayer(*itr))
           {
               if (m_PlayersInWar[player->GetTeamId()].size()+m_InvitedPlayers[player->GetTeamId()].size()< m_MaxPlayer)
                   InvitePlayerToWar(player);
               else
               {
				   // Full
               }
           }
       }
   }
   //m_PlayersInQueue[0].clear();
   //m_PlayersInQueue[1].clear();
}

void OutdoorPvPWG::PlayerAcceptInviteToQueue(Player* player)
{
    m_PlayersInQueue[player->GetTeamId()].insert(player->GetGUID());
    player->GetSession()->SendBfQueueInviteResponce(1, ZONE_WINTERGRASP);
}

void OutdoorPvPWG::PlayerAcceptInviteToWar(Player* player)
{
   if (m_PlayersInWar[player->GetTeamId()].count(player))
       return;

   if (!(m_InvitedPlayers[player->GetTeamId()].count(player->GetGUID())))
       return;

   m_PlayersInQueue[player->GetTeamId()].erase(player->GetGUID());

   if (AddOrSetPlayerToCorrectBfGroup(player))
   {
       player->GetSession()->SendBfEntered(1);
       m_PlayersInWar[player->GetTeamId()].insert(player);
       m_InvitedPlayers[player->GetTeamId()].erase(player->GetGUID());
       // Remove player AFK
       if (player->IsAFK())
           player->ToggleAFK();

       player->RemoveAurasDueToSpell(SPELL_RECRUIT);
       player->RemoveAurasDueToSpell(SPELL_CORPORAL);
       player->RemoveAurasDueToSpell(SPELL_LIEUTENANT);
       player->RemoveAurasDueToSpell(SPELL_TOWER_CONTROL);
       player->RemoveAurasDueToSpell(SPELL_SPIRITUAL_IMMUNITY);

       if (player->GetCurrentLevel() > minlevel)
       {
           if (player->GetTeamId() == GetDefenderTeam())
           {
              player->TeleportTo(571, 5345.0f, 2842.0f, 410.0f, 3.14f);
           }
           else
           {
              if (player->GetTeamId() == TEAM_HORDE)
                  player->TeleportTo(571, 5025.857422f, 3674.628906f, 362.737122f, 4.135169f);
              else
                  player->TeleportTo(571, 5101.284f, 2186.564f, 373.549f, 3.812f);
           }   
       

           player->CastSpell(player, SPELL_RECRUIT, true);

           if (player->GetTeamId() == GetAttackerTeam())
           {
               if (m_towerDestroyedCount[GetAttackerTeam()] < 3)
                   player->SetAuraStack(SPELL_TOWER_CONTROL, player, 3 - m_towerDestroyedCount[GetAttackerTeam()]);
           }
           else
           {
               if (m_towerDestroyedCount[GetAttackerTeam()])
                   player->SetAuraStack(SPELL_TOWER_CONTROL, player, m_towerDestroyedCount[GetAttackerTeam()]);
           }
       }

       player->RemoveAurasByType(SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED);
       player->RemoveAurasByType(SPELL_AURA_FLY);
       player->CastSpell(player, 45472, true);
       player->PlayDirectSound(OUTDOOR_PVP_WG_SOUND_START_BATTLE);
       UpdateTenacityStack();
   }
}

Group* OutdoorPvPWG::GetGroupPlayer(uint64 guid, uint32 TeamId)
{
   for(GuidSet::const_iterator itr=m_Groups[TeamId].begin();itr!=m_Groups[TeamId].end();++itr)
   {
       if (Group* group = sGroupMgr->GetGroupByGUID(*itr))
           if (group->IsMember(guid))
	        return group;
   }
   return NULL;
}

Group* OutdoorPvPWG::GetFreeBfRaid(uint32 TeamId)
{
   // if found free group we return it
   for(GuidSet::const_iterator itr=m_Groups[TeamId].begin();itr!=m_Groups[TeamId].end();++itr)
   {
       if (Group* group = sGroupMgr->GetGroupByGUID(*itr))
          if (!group->IsFull())
              return group;
   }
   return NULL;
}

bool OutdoorPvPWG::IsWarForTeamFull(Player* player)
{
   if (m_PlayersInWar[player->GetTeamId()].size() + m_InvitedPlayers[player->GetTeamId()].size() >= m_MaxPlayer)
       return true;
   return false;
}

bool OutdoorPvPWG::HasPlayerInWG(Player* player, bool inwar) const
{
   if (inwar && IsWarTime())
   {
       if (m_PlayersInWar[player->GetTeamId()].count(player) || m_InvitedPlayers[player->GetTeamId()].count(player->GetGUID()))
           return true;
   }
   else if (m_PlayersInWar[player->GetTeamId()].count(player)
	   || m_InvitedPlayers[player->GetTeamId()].count(player->GetGUID())
	   || m_PlayersInQueue[player->GetTeamId()].count(player->GetGUID()))
        return true;

   return false;
}

bool OutdoorPvPWG::AddOrSetPlayerToCorrectBfGroup(Player* player)
{
   if (!player->IsInWorld())
       return false;

   //if (Group* group = player->GetGroup())
       //group->RemoveMember(player->GetGUID(),GROUP_REMOVEMETHOD_DEFAULT);
   //return true;


   Group* group = GetFreeBfRaid(player->GetTeamId());
   if (!group)
   {
       group = new Group;
       // its crazy thing
       Battleground* bg = (Battleground*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(ZONE_WINTERGRASP);
       group->SetBattlegroundGroup(bg);
       group->Create(player);
       sGroupMgr->AddGroup(group);
       m_Groups[player->GetTeamId()].insert(group->GetGUID());
   }
   else if (group->IsMember(player->GetGUID()))
   {
       uint8 subgroup = group->GetMemberGroup(player->GetGUID());
       player->SetBattlegroundRaid(group, subgroup);
   }
   else
   {
	   group->AddMember(player);
       if (Group* originalGroup = player->GetOriginalGroup())
           if (originalGroup->IsLeader(player->GetGUID()))
               group->ChangeLeader(player->GetGUID());
   }
   return true;
}

bool OutdoorPvPWG::IncrementQuest(Player* player, uint32 quest, bool complete)
{
	if (!player)
		return false;

	Quest const* pQuest = sObjectMgr->GetQuestTemplate(quest);

	if (!pQuest || player->GetQuestStatus(quest) == QUEST_STATUS_NONE)
		return false;

	if (complete)
	{
		player->CompleteQuest(quest);
		return true;
	}
	else
	{
		for (uint8 i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
		{
			int32 creature = pQuest->RequiredNpcOrGo[i];
			if (uint32 spell_id = pQuest->RequiredSpellCast[i])
			{
				player->CastedCreatureOrGO(creature, 0, spell_id);
				return true;
			}
			else if (creature > 0)
			{
				player->KilledMonsterCredit(creature, 0);
				return true;
			}
			else if (creature < 0)
			{
				player->CastedCreatureOrGO(creature, 0, 0);
				return true;
			}
			return true;
		}
	}
	return false;
}

void OutdoorPvPWG::DoCompleteOrIncrementAchievement(uint32 achievement, Player* player, uint8 /*incrementNumber*/)
{
   AchievementEntry const* AE = sAchievementStore.LookupEntry(achievement);
   
   switch (achievement)
   {
       case ACHIEVEMENTS_WIN_WG_100 :
       {
          //player->GetAchievementMgr().UpdateAchievementCriteria();
       }
       default:
       {
           if (AE && player)
               player->CompletedAchievement(AE);
       }
       break;
   }
}

OPvPCapturePointWG::OPvPCapturePointWG(OutdoorPvPWG* opvp, BuildingState* state): OPvPCapturePoint(opvp), m_buildingState(state), m_wintergrasp(opvp), m_engineer(NULL), m_engGuid(0), m_spiritguide(NULL)
, m_spiritguide_horde(NULL), m_spiritguide_alliance(NULL), m_spiGuid(0){}

void OPvPCapturePointWG::SetTeamByBuildingState()
{
   if (m_buildingState->GetTeam() == TEAM_ALLIANCE)
   {
       m_value = m_maxValue;
       m_State = OBJECTIVE_STATE_ALLIANCE;
   }
   else if (m_buildingState->GetTeam() == TEAM_HORDE)
   {
       m_value = -m_maxValue;
       m_State = OBJECTIVE_STATE_HORDE;
   }
   else
   {
       m_value = 0;
       m_State = OBJECTIVE_STATE_NEUTRAL;
   }

   if (m_team != m_buildingState->GetTeam())
   {
       TeamId oldTeam = m_team;
       m_team = m_buildingState->GetTeam();
       ChangeTeam(oldTeam);
   }

   SendChangePhase();
}

void OPvPCapturePointWG::ChangeTeam(TeamId oldTeam)
{
	entry = 0;
	guide_entry = 0;
	guide_entry_fortress_horde = 0;
	guide_entry_fortress_alliance = 0;

	if (oldTeam != TEAM_NEUTRAL)
		m_wintergrasp->ModifyWorkshopCount(oldTeam, false);

	if (m_team != TEAM_NEUTRAL)
	{
		entry = m_team == TEAM_ALLIANCE ? NPC_GNOMISH_ENGINEER_A : NPC_GNOMISH_ENGINEER_H;
		guide_entry = m_team == TEAM_ALLIANCE ? NPC_SPIRIT_HEALER_A : NPC_SPIRIT_HEALER_H;
		guide_entry_fortress_horde = m_team == TEAM_HORDE ? NPC_SPIRIT_HEALER_H : NPC_SPIRIT_HEALER_H;
		guide_entry_fortress_alliance = m_team == TEAM_ALLIANCE ? NPC_SPIRIT_HEALER_A : NPC_SPIRIT_HEALER_A;
		m_wintergrasp->ModifyWorkshopCount(m_team, true);
	}

	if (m_capturePoint)
		GameObject::SetGoArtKit(CapturePointArtKit[m_team], m_capturePoint, m_capturePointGUID);

	m_buildingState->SetTeam(m_team);
	// TODO: this may be sent twice
	m_wintergrasp->BroadcastStateChange(m_buildingState);

   if (m_buildingState->building)
       m_buildingState->building->SetUInt32Value(GAMEOBJECT_FACTION, WintergraspFaction[m_team]);

   if (entry)
   {
       if (m_engGuid)
       {
           *m_engEntry = entry;
           _RespawnCreatureIfNeeded(m_engineer, entry);
       }

       if (m_spiGuid)
       {
		   if (m_wintergrasp->GetAttackerTeam() == TEAM_ALLIANCE)
		   {
			   *m_spiEntry = guide_entry;
			   _RespawnCreatureIfNeeded(m_spiritguide_horde, guide_entry_fortress_horde);
			   m_wintergrasp->RelocateHordeDeadPlayers(m_spiritguide_horde); // Horde
			   _RespawnCreatureIfNeeded(m_spiritguide, guide_entry);
			   m_wintergrasp->RelocateAllianceDeadPlayers(m_spiritguide); // Alliance
		   }
        else
		{
			*m_spiEntry = guide_entry;
			_RespawnCreatureIfNeeded(m_spiritguide_alliance, guide_entry_fortress_alliance);
			m_wintergrasp->RelocateAllianceDeadPlayers(m_spiritguide_alliance); // Alliance
			_RespawnCreatureIfNeeded(m_spiritguide, guide_entry);
			m_wintergrasp->RelocateHordeDeadPlayers(m_spiritguide); // Horde
		   }
	   }
   }
   else if (m_engineer)
	   m_engineer->SetVisible(false);

   sLog->outDebug(LOG_FILTER_OUTDOORPVP, "Wintergrasp workshop now belongs to %u.", (uint32)m_buildingState->GetTeam());
}

void WorldSession::SendBfInvitePlayerToWar(uint32 BattleId,uint32 ZoneId,uint32 p_time)
{
   // Send packet 
   WorldPacket data(SMSG_BATTLEFIELD_MGR_ENTRY_INVITE, 12);
   data << uint32(BattleId);
   data << uint32(ZoneId);
   data << uint32((time(NULL)+p_time));
   SendPacket(&data);
}

void WorldSession::SendBfInvitePlayerToQueue(uint32 BattleId)
{
   WorldPacket data(SMSG_BATTLEFIELD_MGR_QUEUE_INVITE, 5);

   data << uint32(BattleId);
   data << uint8(1);
   SendPacket(&data);
}

void WorldSession::SendBfQueueInviteResponce(uint32 BattleId,uint32 ZoneId)
{
   WorldPacket data(SMSG_BATTLEFIELD_MGR_QUEUE_REQUEST_RESPONSE, 11);
   data << uint32(BattleId);
   data << uint32(ZoneId);
   data << uint8(1); //Accepted
   data << uint8(0); //Logging In
   data << uint8(1); //Warmup

   SendPacket(&data);
}

// This is call when player accept to join war 
// Param1:(BattleId) the BattleId of Bf
void WorldSession::SendBfEntered(uint32 BattleId)
{
   WorldPacket data(SMSG_BATTLEFIELD_MGR_ENTERED, 7);
   data << uint32(BattleId);
   data << uint8(1); // unk
   data << uint8(1); // unk
   data << uint8(_player->IsAFK()? 1:0);// Clear AFK

   SendPacket(&data);
}

// Send when player is kick from Battlefield
void WorldSession::SendBfLeaveMessage(uint32 BattleId)
{
   WorldPacket data(SMSG_BATTLEFIELD_MGR_EJECTED, 7);
   data << uint32(BattleId);
   data << uint8(8); //byte Reason
   data << uint8(2); //byte BattleStatus
   data << uint8(0); //bool Relocated

   SendPacket(&data);
}
// Send by client when he click on accept for queue
void WorldSession::HandleBfQueueInviteResponse(WorldPacket &recv_data)
{
   uint32 BattleId;                                             
   uint8 Accepted;             

   recv_data >> BattleId >> Accepted;
   sLog->outDebug(LOG_FILTER_OUTDOORPVP, "HandleQueueInviteResponse: BattleID: %u Accepted: %u", BattleId, Accepted);

   OutdoorPvPWG* pvpWG = (OutdoorPvPWG*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(ZONE_WINTERGRASP);
   if (!pvpWG)
       return;

   if (Accepted)
       pvpWG->PlayerAcceptInviteToQueue(_player);
}

// Send by client on clicking in accept or refuse of invitation windows for join game
void WorldSession::HandleBfEntryInviteResponse(WorldPacket &recv_data)
{
   uint32 BattleId;                                             
   uint8 Accepted;             

   recv_data >> BattleId >> Accepted;
   sLog->outDebug(LOG_FILTER_OUTDOORPVP, "HandleBattlefieldInviteResponse: BattleID: %u Accepted: %u", BattleId, Accepted);

   OutdoorPvPWG* pvpWG = (OutdoorPvPWG*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(ZONE_WINTERGRASP);
   if (!pvpWG)
       return;

   // If player accept invitation
   if (Accepted)
   {
       pvpWG->PlayerAcceptInviteToWar(_player);
   }        
   else
   {
       if (_player->GetZoneId() == ZONE_WINTERGRASP)
           pvpWG->KickPlayerFromWintergrasp(_player->GetGUID());
   }
}

void WorldSession::HandleBfExitRequest(WorldPacket &recv_data)
{
   uint32 BattleId;                                                         

   recv_data >> BattleId;
   sLog->outDebug(LOG_FILTER_OUTDOORPVP, "HandleBfExitRequest: BattleID: %u", BattleId);

   OutdoorPvPWG* pvpWG = (OutdoorPvPWG*)sOutdoorPvPMgr->GetOutdoorPvPToZoneId(ZONE_WINTERGRASP);

   if (!pvpWG)
	   return;

   pvpWG->PlayerAskToLeave(_player);
}

class OutdoorPvP_wintergrasp : public OutdoorPvPScript
{
public:
	OutdoorPvP_wintergrasp(): OutdoorPvPScript("outdoor_pvp_wg"){}

	OutdoorPvP* GetOutdoorPvP() const
	{
		return new OutdoorPvPWG();
	}
};

void AddSC_outdoor_pvp_wg()
{
   new OutdoorPvP_wintergrasp();
}