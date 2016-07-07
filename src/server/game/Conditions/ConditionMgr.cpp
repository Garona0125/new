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


#include "Player.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "GameEventMgr.h"
#include "ObjectMgr.h"
#include "InstanceScript.h"
#include "ConditionMgr.h"
#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "Guild.h"

// Checks if object meets the condition
// Can have CONDITION_SOURCE_TYPE_NONE && !mReferenceId if called from a special event (ie: eventAI)
bool Condition::Meets(ConditionSourceInfo& sourceInfo)
{
    ASSERT(ConditionTarget < MAX_CONDITION_TARGETS);
    WorldObject* object = sourceInfo.mConditionTargets[ConditionTarget];
    // object not present, return false
    if (!object)
    {
        sLog->outDebug(LOG_FILTER_CONDITIONSYS, "Condition object not found for condition (Entry: %u Type: %u Group: %u)", SourceEntry, SourceType, SourceGroup);
        return false;
    }
    bool condMeets = false;
    switch (ConditionType)
    {
        case CONDITION_NONE:
            condMeets = true;                                    // empty condition, always met
            break;
        case CONDITION_AURA:
        {
            if (Unit* unit = object->ToUnit())
                condMeets = unit->HasAuraEffect(ConditionValue1, ConditionValue2);
            break;
        }
        case CONDITION_ITEM:
        {
            if (Player* player = object->ToPlayer())
            {
                // don't allow 0 items (it's checked during table load)
                ASSERT(ConditionValue2);
                bool checkBank = ConditionValue3 ? true : false;
                condMeets = player->HasItemCount(ConditionValue1, ConditionValue2, checkBank);
            }
            break;
        }
        case CONDITION_ITEM_EQUIPPED:
        {
            if (Player* player = object->ToPlayer())
                condMeets = player->HasItemOrGemWithIdEquipped(ConditionValue1, 1);
            break;
        }
        case CONDITION_ZONEID:
            condMeets = object->GetZoneId() == ConditionValue1;
            break;
        case CONDITION_REPUTATION_RANK:
        {
            if (Player* player = object->ToPlayer())
            {
                if (FactionEntry const* faction = sFactionStore.LookupEntry(ConditionValue1))
                    condMeets = (ConditionValue2 & (1 << player->GetReputationMgr().GetRank(faction)));
            }
            break;
        }
        case CONDITION_ACHIEVEMENT:
        {
            if (Player* player = object->ToPlayer())
                condMeets = player->GetAchievementMgr().HasAchieved(ConditionValue1);
            break;
        }
        case CONDITION_TEAM:
        {
            if (Player* player = object->ToPlayer())
                condMeets = player->GetTeam() == ConditionValue1;
            break;
        }
        case CONDITION_CLASS:
        {
            if (Player* player = object->ToPlayer())
                condMeets = player->GetCurrentClassMask() & ConditionValue1;
            break;
        }
        case CONDITION_RACE:
        {
            if (Player* player = object->ToPlayer())
                condMeets = player->GetCurrentRaceMask() & ConditionValue1;
            break;
        }
		case CONDITION_GENDER:
		{
			if (Player* player = object->ToPlayer())
				condMeets = player->GetCurrentGender() == ConditionValue1;
			break;
		}
        case CONDITION_SKILL:
        {
            if (Player* player = object->ToPlayer())
                condMeets = player->HasSkill(ConditionValue1) && player->GetBaseSkillValue(ConditionValue1) >= ConditionValue2;
            break;
        }
        case CONDITION_QUESTREWARDED:
        {
            if (Player* player = object->ToPlayer())
                condMeets = player->GetQuestRewardStatus(ConditionValue1);
            break;
        }
        case CONDITION_QUESTTAKEN:
        {
            if (Player* player = object->ToPlayer())
            {
                QuestStatus status = player->GetQuestStatus(ConditionValue1);
				condMeets = (status == QUEST_STATUS_INCOMPLETE);
            }
            break;
        }
        case CONDITION_QUEST_COMPLETE:
        {
            if (Player* player = object->ToPlayer())
            {
                QuestStatus status = player->GetQuestStatus(ConditionValue1);
				condMeets = (status == QUEST_STATUS_COMPLETE && !player->GetQuestRewardStatus(ConditionValue1));
            }
            break;
        }
        case CONDITION_QUEST_NONE:
        {
            if (Player* player = object->ToPlayer())
            {
                QuestStatus status = player->GetQuestStatus(ConditionValue1);
                condMeets = (status == QUEST_STATUS_NONE);
            }
            break;
        }
        case CONDITION_ACTIVE_EVENT:
            condMeets = sGameEventMgr->IsActiveEvent(ConditionValue1);
            break;
        case CONDITION_INSTANCE_DATA:
        {
            Map* map = object->GetMap();
            if (map && map->IsDungeon() && ((InstanceMap*)map)->GetInstanceScript())
                condMeets = ((InstanceMap*)map)->GetInstanceScript()->GetData(ConditionValue1) == ConditionValue2;
            break;
        }
        case CONDITION_SPELL_SCRIPT_TARGET:
            condMeets = true;//spell target condition is handled in spellsystem, here it is always true
            break;
        case CONDITION_MAPID:
            condMeets = object->GetMapId() == ConditionValue1;
            break;
        case CONDITION_AREAID:
            condMeets = object->GetAreaId() == ConditionValue1;
            break;
        case CONDITION_SPELL:
        {
            if (Player* player = object->ToPlayer())
                condMeets = player->HasSpell(ConditionValue1);
            break;
        }
        case CONDITION_LEVEL:
        {
            if (Unit* unit = object->ToUnit())
                condMeets = CompareValues(static_cast<ComparisionType>(ConditionValue2), static_cast<uint32>(unit->GetCurrentLevel()), ConditionValue1);
            break;
        }
        case CONDITION_DRUNKENSTATE:
        {
            if (Player* player = object->ToPlayer())
                condMeets = (uint32)Player::GetDrunkenstateByValue(player->GetDrunkValue()) >= ConditionValue1;
            break;
        }
        case CONDITION_NEAR_CREATURE:
        {
            condMeets = GetClosestCreatureWithEntry(object, ConditionValue1, (float)ConditionValue2) ? true : false;
            break;
        }
        case CONDITION_NEAR_GAMEOBJECT:
        {
            condMeets = GetClosestGameObjectWithEntry(object, ConditionValue1, (float)ConditionValue2) ? true : false;
            break;
        }
        case CONDITION_OBJECT_ENTRY:
        {
            if (uint32(object->GetTypeId()) == ConditionValue1)
                condMeets = (!ConditionValue2) || (object->GetEntry() == ConditionValue2);
            break;
        }
        case CONDITION_TYPE_MASK:
        {
            condMeets = object->isType(ConditionValue1);
            break;
        }
        case CONDITION_RELATION_TO:
        {
            if (WorldObject* toObject = sourceInfo.mConditionTargets[ConditionValue1])
            {
                Unit* toUnit = toObject->ToUnit();
                Unit* unit = object->ToUnit();
                if (toUnit && unit)
                {
                    switch (ConditionValue2)
                    {
                        case RELATION_SELF:
                            condMeets = unit == toUnit;
                            break;
                        case RELATION_IN_PARTY:
                            condMeets = unit->IsInPartyWith(toUnit);
                            break;
                        case RELATION_IN_RAID_OR_PARTY:
                            condMeets = unit->IsInRaidWith(toUnit);
                            break;
                        case RELATION_OWNED_BY:
                            condMeets = unit->GetOwnerGUID() == toUnit->GetGUID();
                            break;
                        case RELATION_PASSENGER_OF:
                            condMeets = unit->IsOnVehicle(toUnit);
                            break;
						case RELATION_CREATED_BY:
							condMeets = unit->GetCreatorGUID() == toUnit->GetGUID();
							break;
                    }
                }
            }
            break;
        }
        case CONDITION_REACTION_TO:
        {
            if (WorldObject* toObject = sourceInfo.mConditionTargets[ConditionValue1])
            {
                Unit* toUnit = toObject->ToUnit();
                Unit* unit = object->ToUnit();
                if (toUnit && unit)
                    condMeets = (1 << unit->GetReactionTo(toUnit)) & ConditionValue2;
            }
            break;
        }
        case CONDITION_DISTANCE_TO:
        {
            if (WorldObject* toObject = sourceInfo.mConditionTargets[ConditionValue1])
                condMeets = CompareValues(static_cast<ComparisionType>(ConditionValue3), object->GetDistance(toObject), static_cast<float>(ConditionValue2));
            break;
        }
        case CONDITION_ALIVE:
        {
            if (Unit* unit = object->ToUnit())
                condMeets = unit->IsAlive();
            break;
        }
        case CONDITION_HP_VAL:
        {
            if (Unit* unit = object->ToUnit())
                condMeets = CompareValues(static_cast<ComparisionType>(ConditionValue2), unit->GetHealth(), static_cast<uint32>(ConditionValue1));
            break;
        }
        case CONDITION_HP_PCT:
        {
            if (Unit* unit = object->ToUnit())
                condMeets = CompareValues(static_cast<ComparisionType>(ConditionValue2), unit->GetHealthPct(), static_cast<float>(ConditionValue1));
            break;
        }
        case CONDITION_WORLD_STATE:
        {
            condMeets = ConditionValue2 == sWorld->getWorldState(ConditionValue1);
            break;
        }
        case CONDITION_PHASEMASK:
        {
            condMeets = object->GetPhaseMask() & ConditionValue1;
            break;
        }
		case CONDITION_TITLE:
		{
			if (Player* player = object->ToPlayer())
				condMeets = player->HasTitle(ConditionValue1);
			break;
		}
		case CONDITION_SPAWNMASK:
        {
			condMeets = ((1 << object->GetMap()->GetSpawnMode()) & ConditionValue1);
            break;
		}
		case CONDITION_GUILD_LEVEL:
		{
			if (Player* player = object->ToPlayer())
			{
				if (Guild* guild = player->GetGuild())
					condMeets = CompareValues(static_cast<ComparisionType>(ConditionValue2), static_cast<uint32>(guild->GetLevel()), ConditionValue1);
				break;
			}
		}
        default:
            condMeets = false;
            break;
    }

    if (NegativeCondition)
        condMeets = !condMeets;

    if (!condMeets)
        sourceInfo.mLastFailedCondition = this;

    bool script = sScriptMgr->OnConditionCheck(this, sourceInfo); // Returns true by default.
    return condMeets && script;
}

uint32 Condition::GetMaxAvailableConditionTargets()
{
    // returns number of targets which are available for given source type
    switch(SourceType)
    {
        case CONDITION_SOURCE_TYPE_SPELL:
        case CONDITION_SOURCE_TYPE_CREATURE_TEMPLATE_VEHICLE:
        case CONDITION_SOURCE_TYPE_VEHICLE_SPELL:
        case CONDITION_SOURCE_TYPE_GOSSIP_MENU:
        case CONDITION_SOURCE_TYPE_GOSSIP_MENU_OPTION:
        case CONDITION_SOURCE_TYPE_SMART_EVENT:
		case CONDITION_SOURCE_TYPE_NPC_VENDOR:
            return 2;
        default:
            return 1;
    }
}

ConditionMgr::ConditionMgr(){}

ConditionMgr::~ConditionMgr()
{
    Clean();
}

ConditionList ConditionMgr::GetConditionReferences(uint32 refId)
{
    ConditionList conditions;
    ConditionReferenceContainer::const_iterator ref = ConditionReferenceStore.find(refId);
    if (ref != ConditionReferenceStore.end())
        conditions = (*ref).second;
    return conditions;
}

bool ConditionMgr::IsObjectMeetToConditionList(ConditionSourceInfo& sourceInfo, ConditionList const& conditions)
{
    std::map<uint32, bool> ElseGroupStore;
    for (ConditionList::const_iterator i = conditions.begin(); i != conditions.end(); ++i)
    {
        sLog->outDebug(LOG_FILTER_CONDITIONSYS, "ConditionMgr::IsPlayerMeetToConditionList condType: %u val1: %u", (*i)->ConditionType, (*i)->ConditionValue1);
        if ((*i)->isLoaded())
        {
            std::map<uint32, bool>::const_iterator itr = ElseGroupStore.find((*i)->ElseGroup);
            if (itr == ElseGroupStore.end())
                ElseGroupStore[(*i)->ElseGroup] = true;
            else if (!(*itr).second)
                continue;

            if ((*i)->ReferenceId)//handle reference
            {
                ConditionReferenceContainer::const_iterator ref = ConditionReferenceStore.find((*i)->ReferenceId);
                if (ref != ConditionReferenceStore.end())
                {
                    if (!IsObjectMeetToConditionList(sourceInfo, (*ref).second))
                        ElseGroupStore[(*i)->ElseGroup] = false;
                }
                else
                {
                    sLog->outDebug(LOG_FILTER_CONDITIONSYS, "IsPlayerMeetToConditionList: Reference template -%u not found",
                        (*i)->ReferenceId);//checked at loading, should never happen
                }

            }
            else //handle normal condition
            {
                if (!(*i)->Meets(sourceInfo))
                    ElseGroupStore[(*i)->ElseGroup] = false;
            }
        }
    }
    for (std::map<uint32, bool>::const_iterator i = ElseGroupStore.begin(); i != ElseGroupStore.end(); ++i)
        if (i->second)
            return true;

    return false;
}

bool ConditionMgr::IsObjectMeetToConditions(WorldObject* object, ConditionList const& conditions)
{
    ConditionSourceInfo srcInfo = ConditionSourceInfo(object);
    return IsObjectMeetToConditions(srcInfo, conditions);
}

bool ConditionMgr::IsObjectMeetToConditions(WorldObject* object1, WorldObject* object2, ConditionList const& conditions)
{
    ConditionSourceInfo srcInfo = ConditionSourceInfo(object1, object2);
    return IsObjectMeetToConditions(srcInfo, conditions);
}

bool ConditionMgr::IsObjectMeetToConditions(ConditionSourceInfo& sourceInfo, ConditionList const& conditions)
{
    if (conditions.empty())
        return true;

    sLog->outDebug(LOG_FILTER_CONDITIONSYS, "ConditionMgr::IsObjectMeetToConditions");
    return IsObjectMeetToConditionList(sourceInfo, conditions);
}

ConditionList ConditionMgr::GetConditionsForNotGroupedEntry(ConditionSourceType sourceType, uint32 entry)
{
    ConditionList spellCond;
    if (sourceType > CONDITION_SOURCE_TYPE_NONE && sourceType < CONDITION_SOURCE_TYPE_MAX)
    {
        ConditionContainer::const_iterator itr = ConditionStore.find(sourceType);
        if (itr != ConditionStore.end())
        {
            ConditionTypeContainer::const_iterator i = (*itr).second.find(entry);
            if (i != (*itr).second.end())
            {
                spellCond = (*i).second;
                sLog->outDebug(LOG_FILTER_CONDITIONSYS, "GetConditionsForNotGroupedEntry: found conditions for type %u and entry %u", uint32(sourceType), entry);
            }
        }
    }
    return spellCond;
}

ConditionList ConditionMgr::GetConditionsForVehicleSpell(uint32 creatureID, uint32 spellID)
{
    ConditionList cond;
    VehicleSpellConditionContainer::const_iterator itr = VehicleSpellConditionStore.find(creatureID);
    if (itr != VehicleSpellConditionStore.end())
    {
        ConditionTypeContainer::const_iterator i = (*itr).second.find(spellID);
        if (i != (*itr).second.end())
        {
            cond = (*i).second;
            sLog->outDebug(LOG_FILTER_CONDITIONSYS, "GetConditionsForVehicleSpell: found conditions for Vehicle entry %u spell %u", creatureID, spellID);
        }
    }
    return cond;
}

ConditionList ConditionMgr::GetConditionsForSmartEvent(int32 entryOrGuid, uint32 eventId, uint32 sourceType)
{
    ConditionList cond;
    SmartEventConditionContainer::const_iterator itr = SmartEventConditionStore.find(std::make_pair(entryOrGuid, sourceType));
    if (itr != SmartEventConditionStore.end())
    {
        ConditionTypeContainer::const_iterator i = (*itr).second.find(eventId + 1);
        if (i != (*itr).second.end())
        {
            cond = (*i).second;
            sLog->outDebug(LOG_FILTER_CONDITIONSYS, "GetConditionsForSmartEvent: found conditions for Smart Event entry or guid %d event_id %u", entryOrGuid, eventId);
        }
    }
    return cond;
}

ConditionList ConditionMgr::GetConditionsForNpcVendorEvent(uint32 creatureId, uint32 itemId)
{
    ConditionList cond;
    NpcVendorConditionContainer::const_iterator itr = NpcVendorConditionContainerStore.find(creatureId);
    if (itr != NpcVendorConditionContainerStore.end())
    {
        ConditionTypeContainer::const_iterator i = (*itr).second.find(itemId);
        if (i != (*itr).second.end())
        {
            cond = (*i).second;
            sLog->outDebug(LOG_FILTER_CONDITIONSYS, "GetConditionsForNpcVendorEvent: found conditions for creature entry %u item %u", creatureId, itemId);
        }
    }
    return cond;
}

void ConditionMgr::LoadConditions(bool isReload)
{
    uint32 oldMSTime = getMSTime();

    Clean();

    //must clear all custom handled cases (groupped types) before reload
    if (isReload)
    {
        sLog->OutConsole("Reseting Loot Conditions...");
        LootTemplates_Creature.ResetConditions();
        LootTemplates_Fishing.ResetConditions();
        LootTemplates_Gameobject.ResetConditions();
        LootTemplates_Item.ResetConditions();
        LootTemplates_Mail.ResetConditions();
        LootTemplates_Milling.ResetConditions();
        LootTemplates_Pickpocketing.ResetConditions();
        LootTemplates_Reference.ResetConditions();
        LootTemplates_Skinning.ResetConditions();
        LootTemplates_Disenchant.ResetConditions();
        LootTemplates_Prospecting.ResetConditions();
        LootTemplates_Spell.ResetConditions();

        sLog->OutConsole("Re-Loading `gossip_menu` Table for Conditions!");
        sObjectMgr->LoadGossipMenu();

        sLog->OutConsole("Re-Loading `gossip_menu_option` Table for Conditions!");
        sObjectMgr->LoadGossipMenuItems();
    }

    QueryResult result = WorldDatabase.Query("SELECT SourceTypeOrReferenceId, SourceGroup, SourceEntry, SourceId, ElseGroup, ConditionTypeOrReference, ConditionTarget, "
                                             " ConditionValue1, ConditionValue2, ConditionValue3, NegativeCondition, ErrorType, ErrorTextId, ScriptName FROM conditions");

    if (!result)
    {
		sLog->OutConsole("=======================================================");
        sLog->OutConsole(">> Loaded 0 conditions. DB table `conditions` is empty!");
		sLog->OutConsole("=======================================================");
        sLog->OutConsole();
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        Condition* cond = new Condition();
        int32 iSourceTypeOrReferenceId  = fields[0].GetInt32();
        cond->SourceGroup               = fields[1].GetUInt32();
		cond->SourceEntry               = fields[2].GetInt32();
		cond->SourceId                  = fields[3].GetInt32();
        cond->ElseGroup                 = fields[4].GetUInt32();
        int32 iConditionTypeOrReference = fields[5].GetInt32();
        cond->ConditionTarget           = fields[6].GetUInt8();
        cond->ConditionValue1           = fields[7].GetUInt32();
        cond->ConditionValue2           = fields[8].GetUInt32();
        cond->ConditionValue3           = fields[9].GetUInt32();
        cond->NegativeCondition         = fields[10].GetUInt8();
		cond->ErrorType                 = fields[11].GetUInt32();
		cond->ErrorTextId               = fields[12].GetUInt32();
		cond->ScriptId                  = sObjectMgr->GetScriptId(fields[13].GetCString());

        if (iConditionTypeOrReference >= 0)
            cond->ConditionType = ConditionTypes(iConditionTypeOrReference);

        if (iSourceTypeOrReferenceId >= 0)
            cond->SourceType = ConditionSourceType(iSourceTypeOrReferenceId);

        if (iConditionTypeOrReference < 0)//it has a reference
        {
            if (iConditionTypeOrReference == iSourceTypeOrReferenceId)//self referencing, skip
            {
                sLog->OutErrorDatabase("Condition reference %i is referencing self, skipped", iSourceTypeOrReferenceId);
                delete cond;
                continue;
            }
            cond->ReferenceId = uint32(abs(iConditionTypeOrReference));

            const char* rowType = "reference template";
            if (iSourceTypeOrReferenceId >= 0)
                rowType = "reference";
            //check for useless data
            if (cond->ConditionTarget)
                sLog->OutErrorDatabase("Condition %s %i has useless data in ConditionTarget (%u)!", rowType, iSourceTypeOrReferenceId, cond->ConditionTarget);
            if (cond->ConditionValue1)
                sLog->OutErrorDatabase("Condition %s %i has useless data in value1 (%u)!", rowType, iSourceTypeOrReferenceId, cond->ConditionValue1);
            if (cond->ConditionValue2)
                sLog->OutErrorDatabase("Condition %s %i has useless data in value2 (%u)!", rowType, iSourceTypeOrReferenceId, cond->ConditionValue2);
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("Condition %s %i has useless data in value3 (%u)!", rowType, iSourceTypeOrReferenceId, cond->ConditionValue3);
            if (cond->NegativeCondition)
                sLog->OutErrorDatabase("Condition %s %i has useless data in NegativeCondition (%u)!", rowType, iSourceTypeOrReferenceId, cond->NegativeCondition);
            if (cond->SourceGroup && iSourceTypeOrReferenceId < 0)
                sLog->OutErrorDatabase("Condition %s %i has useless data in SourceGroup (%u)!", rowType, iSourceTypeOrReferenceId, cond->SourceGroup);
            if (cond->SourceEntry && iSourceTypeOrReferenceId < 0)
                sLog->OutErrorDatabase("Condition %s %i has useless data in SourceEntry (%u)!", rowType, iSourceTypeOrReferenceId, cond->SourceEntry);
        }
        else if (!isConditionTypeValid(cond))//doesn't have reference, validate ConditionType
        {
            delete cond;
            continue;
        }

        if (iSourceTypeOrReferenceId < 0)//it is a reference template
        {
            uint32 uRefId = abs(iSourceTypeOrReferenceId);
            if (ConditionReferenceStore.find(uRefId) == ConditionReferenceStore.end())//make sure we have a list for our conditions, based on reference id
            {
                ConditionList mCondList;
                ConditionReferenceStore[uRefId] = mCondList;
            }
            ConditionReferenceStore[uRefId].push_back(cond);//add to reference storage
            count++;
            continue;
        }//end of reference templates

        //if not a reference and SourceType is invalid, skip
        if (iConditionTypeOrReference >= 0 && !isSourceTypeValid(cond))
        {
            delete cond;
            continue;
        }

		if (cond->ErrorType && cond->SourceType != CONDITION_SOURCE_TYPE_SPELL)
		{
			sLog->OutErrorDatabase("Condition type %u entry %i can't have ErrorType (%u), set to 0!", uint32(cond->SourceType), cond->SourceEntry, cond->ErrorType);
			cond->ErrorType = 0;
		}

		if (cond->ErrorTextId && !cond->ErrorType)
		{
			sLog->OutErrorDatabase("Condition type %u entry %i has any ErrorType, ErrorTextId (%u) is set, set to 0!", uint32(cond->SourceType), cond->SourceEntry, cond->ErrorTextId);
			cond->ErrorTextId = 0;
		}

        //Grouping is only allowed for some types (loot templates, gossip menus, gossip items)
        if (cond->SourceGroup && !isGroupable(cond->SourceType))
        {
            sLog->OutErrorDatabase("Condition type %u has not allowed grouping %u!", uint32(cond->SourceType), cond->SourceGroup);
            delete cond;
            continue;
        }
        else if (cond->SourceGroup)
        {
            bool valid = false;
            //handle grouped conditions
            switch (cond->SourceType)
            {
                case CONDITION_SOURCE_TYPE_CREATURE_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Creature.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_DISENCHANT_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Disenchant.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_FISHING_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Fishing.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_GAMEOBJECT_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Gameobject.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_ITEM_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Item.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_MAIL_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Mail.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_MILLING_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Milling.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_PICKPOCKETING_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Pickpocketing.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_PROSPECTING_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Prospecting.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_REFERENCE_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Reference.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_SKINNING_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Skinning.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_SPELL_LOOT_TEMPLATE:
                    valid = addToLootTemplate(cond, LootTemplates_Spell.GetLootForConditionFill(cond->SourceGroup));
                    break;
                case CONDITION_SOURCE_TYPE_GOSSIP_MENU:
                    valid = addToGossipMenus(cond);
                    break;
                case CONDITION_SOURCE_TYPE_GOSSIP_MENU_OPTION:
                    valid = addToGossipMenuItems(cond);
                    break;
                case CONDITION_SOURCE_TYPE_VEHICLE_SPELL:
                {
                    //if no list for vehicle create one
                    if (VehicleSpellConditionStore.find(cond->SourceGroup) == VehicleSpellConditionStore.end())
                    {
                        ConditionTypeContainer cmap;
                        VehicleSpellConditionStore[cond->SourceGroup] = cmap;
                    }
                    //if no list for vehicle's spell create one
                    if (VehicleSpellConditionStore[cond->SourceGroup].find(cond->SourceEntry) == VehicleSpellConditionStore[cond->SourceGroup].end())
                    {
                        ConditionList clist;
                        VehicleSpellConditionStore[cond->SourceGroup][cond->SourceEntry] = clist;
                    }
                    VehicleSpellConditionStore[cond->SourceGroup][cond->SourceEntry].push_back(cond);
                    valid = true;
                    ++count;
                    continue;   // do not add to m_AllocatedMemory to avoid double deleting
                }
                case CONDITION_SOURCE_TYPE_SMART_EVENT:
                {
                    // If the entry does not exist, create a new list
                    std::pair<int32, uint32> key = std::make_pair(cond->SourceEntry, cond->SourceId);
                    if (SmartEventConditionStore.find(key) == SmartEventConditionStore.end())
                    {
                        ConditionTypeContainer cmap;
                        SmartEventConditionStore[key] = cmap;
                    }
                    if (SmartEventConditionStore[key].find(cond->SourceGroup) == SmartEventConditionStore[key].end())
                    {
                        ConditionList clist;
                        SmartEventConditionStore[key][cond->SourceGroup] = clist;
                    }
                    SmartEventConditionStore[key][cond->SourceGroup].push_back(cond);
                    valid = true;
                    ++count;
                    continue;
                }
				case CONDITION_SOURCE_TYPE_NPC_VENDOR:
                {
                    NpcVendorConditionContainerStore[cond->SourceGroup][cond->SourceEntry].push_back(cond);
                    valid =  true;
                    ++count;
                    continue;
                }
                default:
                    break;
            }

            if (!valid)
            {
                sLog->OutErrorDatabase("Not handled grouped condition, SourceGroup %u", cond->SourceGroup);
                delete cond;
            }
            else
            {
                AllocatedMemoryStore.push_back(cond);
                ++count;
            }
            continue;
        }

        //handle not grouped conditions
        //make sure we have a storage list for our SourceType
        if (ConditionStore.find(cond->SourceType) == ConditionStore.end())
        {
            ConditionTypeContainer mTypeMap;
            ConditionStore[cond->SourceType] = mTypeMap;//add new empty list for SourceType
        }

        //make sure we have a condition list for our SourceType's entry
        if (ConditionStore[cond->SourceType].find(cond->SourceEntry) == ConditionStore[cond->SourceType].end())
        {
            ConditionList mCondList;
            ConditionStore[cond->SourceType][cond->SourceEntry] = mCondList;
        }

        //add new Condition to storage based on Type/Entry
        ConditionStore[cond->SourceType][cond->SourceEntry].push_back(cond);
        ++count;
    }
    while (result->NextRow());

	sLog->OutConsole("==================================");
    sLog->OutConsole(">> Loaded %u conditions in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
	sLog->OutConsole("==================================");
    sLog->OutConsole();
}

bool ConditionMgr::addToLootTemplate(Condition* cond, LootTemplate* loot)
{
    if (!loot)
    {
        sLog->OutErrorDatabase("ConditionMgr: LootTemplate %u not found", cond->SourceGroup);
        return false;
    }

    if (loot->addConditionItem(cond))
        return true;

    sLog->OutErrorDatabase("ConditionMgr: Item %u not found in LootTemplate %u", cond->SourceEntry, cond->SourceGroup);
    return false;
}

bool ConditionMgr::addToGossipMenus(Condition* cond)
{
    GossipMenusMapBoundsNonConst pMenuBounds = sObjectMgr->GetGossipMenusMapBoundsNonConst(cond->SourceGroup);

    if (pMenuBounds.first != pMenuBounds.second)
    {
        for (GossipMenusContainer::iterator itr = pMenuBounds.first; itr != pMenuBounds.second; ++itr)
        {
            if ((*itr).second.entry == cond->SourceGroup && (*itr).second.text_id == uint32(cond->SourceEntry))
            {
                (*itr).second.conditions.push_back(cond);
                return true;
            }
        }
    }

    sLog->OutErrorDatabase("addToGossipMenus: GossipMenu %u not found", cond->SourceGroup);
    return false;
}

bool ConditionMgr::addToGossipMenuItems(Condition* cond)
{
    GossipMenuItemsMapBoundsNonConst pMenuItemBounds = sObjectMgr->GetGossipMenuItemsMapBoundsNonConst(cond->SourceGroup);
    if (pMenuItemBounds.first != pMenuItemBounds.second)
    {
        for (GossipMenuItemsContainer::iterator itr = pMenuItemBounds.first; itr != pMenuItemBounds.second; ++itr)
        {
            if ((*itr).second.MenuId == cond->SourceGroup && (*itr).second.OptionIndex == uint32(cond->SourceEntry))
            {
                (*itr).second.Conditions.push_back(cond);
                return true;
            }
        }
    }

    sLog->OutErrorDatabase("addToGossipMenuItems: GossipMenuId %u Item %u not found", cond->SourceGroup, cond->SourceEntry);
    return false;
}

bool ConditionMgr::isSourceTypeValid(Condition* cond)
{
    if (cond->SourceType == CONDITION_SOURCE_TYPE_NONE || cond->SourceType >= CONDITION_SOURCE_TYPE_MAX)
    {
        sLog->OutErrorDatabase("Invalid ConditionSourceType %u in `condition` table, ignoring.", uint32(cond->SourceType));
        return false;
    }

    switch (cond->SourceType)
    {
        case CONDITION_SOURCE_TYPE_CREATURE_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Creature.HaveLootFor(cond->SourceGroup))
            {
                sLog->OutErrorDatabase("SourceGroup %u in `condition` table, does not exist in `creature_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Creature.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                sLog->OutErrorDatabase("SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_DISENCHANT_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Disenchant.HaveLootFor(cond->SourceGroup))
            {
                sLog->OutErrorDatabase("SourceGroup %u in `condition` table, does not exist in `disenchant_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Disenchant.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                sLog->OutErrorDatabase("SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_FISHING_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Fishing.HaveLootFor(cond->SourceGroup))
            {
                sLog->OutErrorDatabase("SourceGroup %u in `condition` table, does not exist in `fishing_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Fishing.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                sLog->OutErrorDatabase("SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_GAMEOBJECT_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Gameobject.HaveLootFor(cond->SourceGroup))
            {
                sLog->OutErrorDatabase("SourceGroup %u in `condition` table, does not exist in `gameobject_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Gameobject.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                sLog->OutErrorDatabase("SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_ITEM_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Item.HaveLootFor(cond->SourceGroup))
            {
                sLog->OutErrorDatabase("SourceGroup %u in `condition` table, does not exist in `item_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Item.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                sLog->OutErrorDatabase("SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_MAIL_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Mail.HaveLootFor(cond->SourceGroup))
            {
                sLog->OutErrorDatabase("SourceGroup %u in `condition` table, does not exist in `mail_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Mail.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                sLog->OutErrorDatabase("SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_MILLING_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Milling.HaveLootFor(cond->SourceGroup))
            {
                sLog->OutErrorDatabase("SourceGroup %u in `condition` table, does not exist in `milling_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Milling.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                sLog->OutErrorDatabase("SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_PICKPOCKETING_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Pickpocketing.HaveLootFor(cond->SourceGroup))
            {
                sLog->OutErrorDatabase("SourceGroup %u in `condition` table, does not exist in `pickpocketing_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Pickpocketing.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                sLog->OutErrorDatabase("SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_PROSPECTING_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Prospecting.HaveLootFor(cond->SourceGroup))
            {
                sLog->OutErrorDatabase("SourceGroup %u in `condition` table, does not exist in `prospecting_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Prospecting.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                sLog->OutErrorDatabase("SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_REFERENCE_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Reference.HaveLootFor(cond->SourceGroup))
            {
                sLog->OutErrorDatabase("SourceGroup %u in `condition` table, does not exist in `reference_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Reference.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                sLog->OutErrorDatabase("SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_SKINNING_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Skinning.HaveLootFor(cond->SourceGroup))
            {
                sLog->OutErrorDatabase("SourceGroup %u in `condition` table, does not exist in `skinning_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Skinning.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                sLog->OutErrorDatabase("SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_SPELL_LOOT_TEMPLATE:
        {
            if (!LootTemplates_Spell.HaveLootFor(cond->SourceGroup))
            {
                sLog->OutErrorDatabase("SourceGroup %u in `condition` table, does not exist in `spell_loot_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            LootTemplate* loot = LootTemplates_Spell.GetLootForConditionFill(cond->SourceGroup);
            ItemTemplate const* pItemProto = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!pItemProto && !loot->isReference(cond->SourceEntry))
            {
                sLog->OutErrorDatabase("SourceType %u, SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceType, cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_SPELL_SCRIPT_TARGET:
        {
            if (cond->ConditionType != CONDITION_SPELL_SCRIPT_TARGET)
            {
                sLog->OutErrorDatabase("SourceEntry %u in `condition` table, has ConditionType %u. Only CONDITION_SPELL_SCRIPT_TARGET(18) is valid for CONDITION_SOURCE_TYPE_SPELL_SCRIPT_TARGET(14), ignoring.", cond->SourceEntry, uint32(cond->ConditionType));
                return false;
            }

            SpellInfo const* spellProto = sSpellMgr->GetSpellInfo(cond->SourceEntry);
            if (!spellProto)
            {
                sLog->OutErrorDatabase("SourceEntry %u in `condition` table, does not exist in `spell.dbc`, ignoring.", cond->SourceEntry);
                return false;
            }

            bool targetfound = false;
            for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
            {
                if (spellProto->Effects[i].TargetA.GetTarget() == TARGET_UNIT_SRC_AREA_ENTRY ||
                    spellProto->Effects[i].TargetB.GetTarget() == TARGET_UNIT_SRC_AREA_ENTRY ||
                    spellProto->Effects[i].TargetA.GetTarget() == TARGET_UNIT_DEST_AREA_ENTRY ||
                    spellProto->Effects[i].TargetB.GetTarget() == TARGET_UNIT_DEST_AREA_ENTRY ||
                    spellProto->Effects[i].TargetA.GetTarget() == TARGET_UNIT_NEARBY_ENTRY ||
                    spellProto->Effects[i].TargetB.GetTarget() == TARGET_UNIT_NEARBY_ENTRY ||
                    spellProto->Effects[i].TargetA.GetTarget() == TARGET_GAMEOBJECT_NEARBY_ENTRY ||
                    spellProto->Effects[i].TargetB.GetTarget() == TARGET_GAMEOBJECT_NEARBY_ENTRY ||
                    spellProto->Effects[i].TargetA.GetTarget() == TARGET_GAMEOBJECT_SRC_AREA ||
                    spellProto->Effects[i].TargetB.GetTarget() == TARGET_GAMEOBJECT_SRC_AREA ||
                    spellProto->Effects[i].TargetA.GetTarget() == TARGET_GAMEOBJECT_DEST_AREA ||
                    spellProto->Effects[i].TargetB.GetTarget() == TARGET_GAMEOBJECT_DEST_AREA ||
                    spellProto->Effects[i].TargetA.GetTarget() == TARGET_DEST_NEARBY_ENTRY ||
                    spellProto->Effects[i].TargetB.GetTarget() == TARGET_DEST_NEARBY_ENTRY ||
                    spellProto->Effects[i].TargetA.GetTarget() == TARGET_UNIT_CONE_ENTRY ||
                    spellProto->Effects[i].TargetB.GetTarget() == TARGET_UNIT_CONE_ENTRY)
                {
                    targetfound = true;
                    //break;
                }
                else if (cond->ConditionValue3 & (1 << i))
                {
                    cond->ConditionValue3 &= ~(1 << i);
                    sLog->OutErrorDatabase("SourceEntry %u in `condition` table does not have any implicit target TARGET_UNIT_NEARBY_ENTRY(38) or TARGET_DEST_NEARBY_ENTRY (46)"
                                    ", TARGET_UNIT_SRC_AREA_ENTRY(7), TARGET_UNIT_DEST_AREA_ENTRY(8), TARGET_UNIT_CONE_ENTRY(60), TARGET_GAMEOBJECT_NEARBY_ENTRY(40)"
                                    "TARGET_GAMEOBJECT_SRC_AREA(51), TARGET_GAMEOBJECT_DEST_AREA(52) in effect %u", cond->SourceEntry, uint32(i));
                }
            }
            if (!targetfound && !cond->ConditionValue3) // cond->mConditionValue3 already errored up there
            {
                sLog->OutErrorDatabase("SourceEntry %u in `condition` table does not have any implicit target TARGET_UNIT_NEARBY_ENTRY(38) or TARGET_DEST_NEARBY_ENTRY (46)"
                                ", TARGET_UNIT_SRC_AREA_ENTRY(7), TARGET_UNIT_DEST_AREA_ENTRY(8), TARGET_UNIT_CONE_ENTRY(60), TARGET_GAMEOBJECT_NEARBY_ENTRY(40)"
                                "TARGET_GAMEOBJECT_SRC_AREA(51), TARGET_GAMEOBJECT_DEST_AREA(52)", cond->SourceEntry);
                return false;
            }
            if ((cond->ConditionValue1 == SPELL_TARGET_TYPE_DEAD) && !spellProto->IsAllowingDeadTarget())
            {
                sLog->OutErrorDatabase("SourceEntry %u in `condition` table does have SPELL_TARGET_TYPE_DEAD specified but spell does not have SPELL_ATTR2_CAN_TARGET_DEAD", cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_CREATURE_TEMPLATE_VEHICLE:
        {
            if (!sObjectMgr->GetCreatureTemplate(cond->SourceEntry))
            {
                sLog->OutErrorDatabase("SourceEntry %u in `condition` table, does not exist in `creature_template`, ignoring.", cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_SPELL:
        {
            SpellInfo const* spellProto = sSpellMgr->GetSpellInfo(cond->SourceEntry);
            if (!spellProto)
            {
                sLog->OutErrorDatabase("SourceEntry %u in `condition` table, does not exist in `spell.dbc`, ignoring.", cond->SourceEntry);
                return false;
            }
            break;
        }
        case CONDITION_SOURCE_TYPE_QUEST_ACCEPT:
            if (!sObjectMgr->GetQuestTemplate(cond->SourceEntry))
            {
                sLog->OutErrorDatabase("CONDITION_SOURCE_TYPE_QUEST_ACCEPT specifies non-existing quest (%u), skipped", cond->SourceEntry);
                return false;
            }
            break;
        case CONDITION_SOURCE_TYPE_QUEST_SHOW_MARK:
            if (!sObjectMgr->GetQuestTemplate(cond->SourceEntry))
            {
                sLog->OutErrorDatabase("CONDITION_SOURCE_TYPE_QUEST_SHOW_MARK specifies non-existing quest (%u), skipped", cond->SourceEntry);
                return false;
            }
            break;
        case CONDITION_SOURCE_TYPE_VEHICLE_SPELL:
            if (!sObjectMgr->GetCreatureTemplate(cond->SourceGroup))
            {
                sLog->OutErrorDatabase("SourceEntry %u in `condition` table, does not exist in `creature_template`, ignoring.", cond->SourceGroup);
                return false;
            }

            if (!sSpellMgr->GetSpellInfo(cond->SourceEntry))
            {
                sLog->OutErrorDatabase("SourceEntry %u in `condition` table, does not exist in `spell.dbc`, ignoring.", cond->SourceEntry);
                return false;
            }
            break;
		case CONDITION_SOURCE_TYPE_NPC_VENDOR:
        {
            if (!sObjectMgr->GetCreatureTemplate(cond->SourceGroup))
            {
                sLog->OutErrorDatabase("SourceEntry %u in `condition` table, does not exist in `creature_template`, ignoring.", cond->SourceGroup);
                return false;
            }
            ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(cond->SourceEntry);
            if (!itemTemplate)
            {
                sLog->OutErrorDatabase("SourceEntry %u in `condition` table, does not exist in `item_template`, ignoring.", cond->SourceEntry);
                return false;
            }
            break;
		}
        case CONDITION_SOURCE_TYPE_UNUSED_18:
            sLog->OutErrorDatabase("Found SourceTypeOrReferenceId = CONDITION_SOURCE_TYPE_UNUSED_18 in `conditions` table - ignoring");
            return false;
        case CONDITION_SOURCE_TYPE_GOSSIP_MENU:
        case CONDITION_SOURCE_TYPE_GOSSIP_MENU_OPTION:
        case CONDITION_SOURCE_TYPE_SMART_EVENT:
        case CONDITION_SOURCE_TYPE_NONE:
        default:
            break;
    }

    return true;
}

bool ConditionMgr::isConditionTypeValid(Condition* cond)
{
    if (cond->ConditionType == CONDITION_NONE || cond->ConditionType >= CONDITION_MAX)
    {
        sLog->OutErrorDatabase("Invalid ConditionType %u at SourceEntry %u in `condition` table, ignoring.", uint32(cond->ConditionType), cond->SourceEntry);
        return false;
    }

    if (cond->ConditionTarget >= cond->GetMaxAvailableConditionTargets())
    {
        sLog->OutErrorDatabase("SourceType %u, SourceEntry %u, SourceGroup %u in `condition` table, has incorrect ConditionTarget set, ignoring.", cond->SourceType, cond->SourceEntry, cond->SourceGroup);
        return false;
    }

    switch (cond->ConditionType)
    {
        case CONDITION_AURA:
        {
            if (!sSpellMgr->GetSpellInfo(cond->ConditionValue1))
            {
                sLog->OutErrorDatabase("Aura condition has non existing spell (Id: %d), skipped", cond->ConditionValue1);
                return false;
            }

            if (cond->ConditionValue2 > 2)
            {
                sLog->OutErrorDatabase("Aura condition has non existing effect index (%u) (must be 0..2), skipped", cond->ConditionValue2);
                return false;
            }
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("Aura condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_ITEM:
        {
            ItemTemplate const* proto = sObjectMgr->GetItemTemplate(cond->ConditionValue1);
            if (!proto)
            {
                sLog->OutErrorDatabase("Item condition has non existing item (%u), skipped", cond->ConditionValue1);
                return false;
            }

            if (!cond->ConditionValue2)
            {
                sLog->OutErrorDatabase("Item condition has 0 set for item count in value2 (%u), skipped", cond->ConditionValue2);
                return false;
            }
            break;
        }
        case CONDITION_ITEM_EQUIPPED:
        {
            ItemTemplate const* proto = sObjectMgr->GetItemTemplate(cond->ConditionValue1);
            if (!proto)
            {
                sLog->OutErrorDatabase("ItemEquipped condition has non existing item (%u), skipped", cond->ConditionValue1);
                return false;
            }

            if (cond->ConditionValue2)
                sLog->OutErrorDatabase("ItemEquipped condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("ItemEquipped condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_ZONEID:
        {
            AreaTableEntry const* areaEntry = GetAreaEntryByAreaID(cond->ConditionValue1);
            if (!areaEntry)
            {
                sLog->OutErrorDatabase("ZoneID condition has non existing area (%u), skipped", cond->ConditionValue1);
                return false;
            }

            if (areaEntry->zone != 0)
            {
                sLog->OutErrorDatabase("ZoneID condition requires to be in area (%u) which is a subzone but zone expected, skipped", cond->ConditionValue1);
                return false;
            }

            if (cond->ConditionValue2)
                sLog->OutErrorDatabase("ZoneID condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("ZoneID condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_REPUTATION_RANK:
        {
            FactionEntry const* factionEntry = sFactionStore.LookupEntry(cond->ConditionValue1);
            if (!factionEntry)
            {
                sLog->OutErrorDatabase("Reputation condition has non existing faction (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("Reputation condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_TEAM:
        {
            if (cond->ConditionValue1 != ALLIANCE && cond->ConditionValue1 != HORDE)
            {
                sLog->OutErrorDatabase("Team condition specifies unknown team (%u), skipped", cond->ConditionValue1);
                return false;
            }

            if (cond->ConditionValue2)
                sLog->OutErrorDatabase("Team condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("Team condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_SKILL:
        {
            SkillLineEntry const* pSkill = sSkillLineStore.LookupEntry(cond->ConditionValue1);
            if (!pSkill)
            {
                sLog->OutErrorDatabase("Skill condition specifies non-existing skill (%u), skipped", cond->ConditionValue1);
                return false;
            }

            if (cond->ConditionValue2 < 1 || cond->ConditionValue2 > sWorld->GetConfigMaxSkillValue())
            {
				sLog->OutErrorDatabase("Skill condition specifies skill (%u) with invalid value (%u), skipped", cond->ConditionValue1, cond->ConditionValue2);
                return false;
            }
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("Skill condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_QUESTREWARDED:
        case CONDITION_QUESTTAKEN:
        case CONDITION_QUEST_NONE:
        case CONDITION_QUEST_COMPLETE:
        {
            if (!sObjectMgr->GetQuestTemplate(cond->ConditionValue1))
            {
                sLog->OutErrorDatabase("Quest condition specifies non-existing quest (%u), skipped", cond->ConditionValue1);
                return false;
            }

            if (cond->ConditionValue2 > 1)
                sLog->OutErrorDatabase("Quest condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("Quest condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_ACTIVE_EVENT:
        {
            GameEventMgr::GameEventDataMap const& events = sGameEventMgr->GetEventMap();
            if (cond->ConditionValue1 >=events.size() || !events[cond->ConditionValue1].isValid())
            {
                sLog->OutErrorDatabase("ActiveEvent condition has non existing event id (%u), skipped", cond->ConditionValue1);
                return false;
            }

            if (cond->ConditionValue2)
                sLog->OutErrorDatabase("ActiveEvent condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("ActiveEvent condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_ACHIEVEMENT:
        {
            AchievementEntry const* achievement = sAchievementStore.LookupEntry(cond->ConditionValue1);
            if (!achievement)
            {
                sLog->OutErrorDatabase("Achivement condition has non existing achivement id (%u), skipped", cond->ConditionValue1);
                return false;
            }

            if (cond->ConditionValue2)
                sLog->OutErrorDatabase("Achivement condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("Achivement condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_CLASS:
        {
            if (!(cond->ConditionValue1 & CLASSMASK_ALL_PLAYABLE))
            {
                sLog->OutErrorDatabase("Class condition has non existing classmask (%u), skipped", cond->ConditionValue1 & ~CLASSMASK_ALL_PLAYABLE);
                return false;
            }

            if (cond->ConditionValue2)
                sLog->OutErrorDatabase("Class condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("Class condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_RACE:
        {
            if (!(cond->ConditionValue1 & RACEMASK_ALL_PLAYABLE))
            {
                sLog->OutErrorDatabase("Race condition has non existing racemask (%u), skipped", cond->ConditionValue1 & ~RACEMASK_ALL_PLAYABLE);
                return false;
            }

            if (cond->ConditionValue2)
                sLog->OutErrorDatabase("Race condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("Race condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_SPELL_SCRIPT_TARGET:
        {
            if (cond->ConditionValue1 >= MAX_SPELL_TARGET_TYPE)
            {
                sLog->OutErrorDatabase("SpellTarget condition has non existing spell target type (%u), skipped", cond->ConditionValue1);
                return false;
            }

            switch (cond->ConditionValue1)
            {
                case SPELL_TARGET_TYPE_GAMEOBJECT:
                {
                    if (cond->ConditionValue2 && !sObjectMgr->GetGameObjectTemplate(cond->ConditionValue2))
                    {
                        sLog->OutErrorDatabase("SpellTarget condition has non existing gameobject (%u) as target, skipped", cond->ConditionValue2);
                        return false;
                    }
                    break;
                }
                case SPELL_TARGET_TYPE_CONTROLLED:
                case SPELL_TARGET_TYPE_CREATURE:
                case SPELL_TARGET_TYPE_DEAD:
                {
                    if (cond->ConditionValue2 && !sObjectMgr->GetCreatureTemplate(cond->ConditionValue2))
                    {
                        sLog->OutErrorDatabase("SpellTarget condition has non existing creature template entry (%u) as target, skipped", cond->ConditionValue2);
                        return false;
                    }

                    const CreatureTemplate* cInfo = sObjectMgr->GetCreatureTemplate(cond->ConditionValue2);
                    if (cond->SourceEntry == 30427 && !cInfo->SkinLootId)
                    {
                        sLog->OutErrorDatabase("SpellTarget condition has creature entry %u as a target of spellid 30427, but this creature has no skinlootid. Gas extraction will not work!, skipped", cond->ConditionValue2);
                        return false;
                    }
                    break;
                }
            }
            break;
        }
		case CONDITION_GENDER:
		{
			if (!Player::IsValidGender(uint8(cond->ConditionValue1)))
			{
				sLog->OutErrorDatabase("Gender condition has invalid gender (%u), skipped", cond->ConditionValue1);
				return false;
			}

			if (cond->ConditionValue2)
				sLog->OutErrorDatabase("Gender condition has useless data in value2 (%u)!", cond->ConditionValue2);
			if (cond->ConditionValue3)
				sLog->OutErrorDatabase("Gender condition has useless data in value3 (%u)!", cond->ConditionValue3);
			break;
		}
        case CONDITION_MAPID:
        {
            MapEntry const* me = sMapStore.LookupEntry(cond->ConditionValue1);
            if (!me)
            {
                sLog->OutErrorDatabase("Map condition has non existing map (%u), skipped", cond->ConditionValue1);
                return false;
            }

            if (cond->ConditionValue2)
                sLog->OutErrorDatabase("Map condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("Map condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_SPELL:
        {
            if (!sSpellMgr->GetSpellInfo(cond->ConditionValue1))
            {
                sLog->OutErrorDatabase("Spell condition has non existing spell (Id: %d), skipped", cond->ConditionValue1);
                return false;
            }

            if (cond->ConditionValue2)
                sLog->OutErrorDatabase("Spell condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("Spell condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_LEVEL:
        {
            if (cond->ConditionValue2 >= COMP_TYPE_MAX)
            {
                sLog->OutErrorDatabase("Level condition has invalid ComparisionType (%u), skipped", cond->ConditionValue2);
                return false;
            }
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("Level condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_DRUNKENSTATE:
        {
            if (cond->ConditionValue1 > DRUNKEN_SMASHED)
            {
                sLog->OutErrorDatabase("DrunkState condition has invalid state (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue2)
            {
                sLog->OutErrorDatabase("DrunkState condition has useless data in value2 (%u)!", cond->ConditionValue2);
                return false;
            }
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("DrunkState condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_NEAR_CREATURE:
        {
            if (!sObjectMgr->GetCreatureTemplate(cond->ConditionValue1))
            {
                sLog->OutErrorDatabase("NearCreature condition has non existing creature template entry (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("NearCreature condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_NEAR_GAMEOBJECT:
        {
            if (!sObjectMgr->GetGameObjectTemplate(cond->ConditionValue1))
            {
                sLog->OutErrorDatabase("NearGameObject condition has non existing gameobject template entry (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("NearGameObject condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_OBJECT_ENTRY:
        {
            switch (cond->ConditionValue1)
            {
                case TYPE_ID_UNIT:
                    if (cond->ConditionValue2 && !sObjectMgr->GetCreatureTemplate(cond->ConditionValue2))
                    {
                        sLog->OutErrorDatabase("ObjectEntry condition has non existing creature template entry  (%u), skipped", cond->ConditionValue2);
                        return false;
                    }
                    break;
                case TYPE_ID_GAMEOBJECT:
                    if (cond->ConditionValue2 && !sObjectMgr->GetGameObjectTemplate(cond->ConditionValue2))
                    {
                        sLog->OutErrorDatabase("ObjectEntry condition has non existing game object template entry  (%u), skipped", cond->ConditionValue2);
                        return false;
                    }
                    break;
                case TYPE_ID_PLAYER:
                case TYPE_ID_CORPSE:
                    if (cond->ConditionValue2)
                        sLog->OutErrorDatabase("ObjectEntry condition has useless data in value2 (%u)!", cond->ConditionValue2);
                    break;
                default:
                    sLog->OutErrorDatabase("ObjectEntry condition has wrong typeid set (%u), skipped", cond->ConditionValue1);
                    return false;
            }
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("ObjectEntry condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_TYPE_MASK:
        {
            if (!cond->ConditionValue1 || (cond->ConditionValue1 & ~(TYPE_MASK_UNIT | TYPE_MASK_PLAYER | TYPE_MASK_GAMEOBJECT | TYPE_MASK_CORPSE)))
            {
                sLog->OutErrorDatabase("Type Mask condition has invalid type mask set (%u), skipped", cond->ConditionValue2);
                return false;
            }
            if (cond->ConditionValue2)
                sLog->OutErrorDatabase("Type Mask condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("Type Mask condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_RELATION_TO:
        {
            if (cond->ConditionValue1 >= cond->GetMaxAvailableConditionTargets())
            {
                sLog->OutErrorDatabase("Relation To condition has invalid ConditionValue1(ConditionTarget selection) (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue1 == cond->ConditionTarget)
            {
                sLog->OutErrorDatabase("Relation To condition has ConditionValue1(ConditionTarget selection) set to self (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue2 >= RELATION_MAX)
            {
                sLog->OutErrorDatabase("Relation To condition has invalid ConditionValue2(RelationType) (%u), skipped", cond->ConditionValue2);
                return false;
            }
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("Relation To condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_REACTION_TO:
        {
            if (cond->ConditionValue1 >= cond->GetMaxAvailableConditionTargets())
            {
                sLog->OutErrorDatabase("Reaction To condition has invalid ConditionValue1(ConditionTarget selection) (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue1 == cond->ConditionTarget)
            {
                sLog->OutErrorDatabase("Reaction To condition has ConditionValue1(ConditionTarget selection) set to self (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (!cond->ConditionValue2)
            {
                sLog->OutErrorDatabase("mConditionValue2 condition has invalid ConditionValue2(rankMask) (%u), skipped", cond->ConditionValue2);
                return false;
            }
            break;
        }
        case CONDITION_DISTANCE_TO:
        {
            if (cond->ConditionValue1 >= cond->GetMaxAvailableConditionTargets())
            {
                sLog->OutErrorDatabase("Distance To condition has invalid ConditionValue1(ConditionTarget selection) (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue1 == cond->ConditionTarget)
            {
                sLog->OutErrorDatabase("Distance To condition has ConditionValue1(ConditionTarget selection) set to self (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue3 >= COMP_TYPE_MAX)
            {
                sLog->OutErrorDatabase("Distance To condition has invalid ComparisionType (%u), skipped", cond->ConditionValue3);
                return false;
            }
            break;
        }
        case CONDITION_ALIVE:
        {
            if (cond->ConditionValue1)
                sLog->OutErrorDatabase("Alive condition has useless data in value1 (%u)!", cond->ConditionValue1);
            if (cond->ConditionValue2)
                sLog->OutErrorDatabase("Alive condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("Alive condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_HP_VAL:
        {
            if (cond->ConditionValue2 >= COMP_TYPE_MAX)
            {
                sLog->OutErrorDatabase("HpVal condition has invalid ComparisionType (%u), skipped", cond->ConditionValue2);
                return false;
            }
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("HpVal condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_HP_PCT:
        {
            if (cond->ConditionValue1 > 100)
            {
                sLog->OutErrorDatabase("HpPct condition has too big percent value (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue2 >= COMP_TYPE_MAX)
            {
                sLog->OutErrorDatabase("HpPct condition has invalid ComparisionType (%u), skipped", cond->ConditionValue2);
                return false;
            }
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("HpPct condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
        case CONDITION_AREAID:
        case CONDITION_INSTANCE_DATA:
            break;
        case CONDITION_WORLD_STATE:
        {
            if (!sWorld->getWorldState(cond->ConditionValue1))
            {
                sLog->OutErrorDatabase("World state condition has non existing world state in value1 (%u), skipped", cond->ConditionValue1);
                return false;
            }
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("World state condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
        }
		case CONDITION_PHASEMASK:
        {
            if (cond->ConditionValue2)
                sLog->OutErrorDatabase("Phasemask condition has useless data in value2 (%u)!", cond->ConditionValue2);
            if (cond->ConditionValue3)
                sLog->OutErrorDatabase("Phasemask condition has useless data in value3 (%u)!", cond->ConditionValue3);
            break;
		}
		case CONDITION_TITLE:
		{
			CharTitlesEntry const* titleEntry = sCharTitlesStore.LookupEntry(cond->ConditionValue1);
			if (!titleEntry)
			{
				sLog->OutErrorDatabase("Title condition has non existing title in value1 (%u), skipped", cond->ConditionValue1);
				return false;
			}
			break;
		}
		case CONDITION_SPAWNMASK:
		{
			if (cond->ConditionValue1 > SPAWNMASK_RAID_ALL)
			{
				sLog->OutErrorDatabase("SpawnMask condition has non existing SpawnMask in value1 (%u), skipped", cond->ConditionValue1);
				return false;
			}
			break;
		}
		case CONDITION_GUILD_LEVEL:
		{
			if (cond->ConditionValue2 >= COMP_TYPE_MAX)
			{
				sLog->OutErrorDatabase("Guildlevel condition has invalid option (%u), skipped", cond->ConditionValue2);
				return false;
			}
			if (cond->ConditionValue3)
				sLog->OutErrorDatabase("Guildlevel condition has useless data in value3 (%u)!", cond->ConditionValue3);
			break;
		}
        case CONDITION_UNUSED_24:
            sLog->OutErrorDatabase("Found ConditionTypeOrReference = CONDITION_UNUSED_24 in `conditions` table - ignoring");
            return false;
        default:
            break;
    }
    return true;
}

void ConditionMgr::Clean()
{
    for (ConditionReferenceContainer::iterator itr = ConditionReferenceStore.begin(); itr != ConditionReferenceStore.end(); ++itr)
    {
        for (ConditionList::const_iterator it = itr->second.begin(); it != itr->second.end(); ++it)
            delete *it;
        itr->second.clear();
    }

    ConditionReferenceStore.clear();

    for (ConditionContainer::iterator itr = ConditionStore.begin(); itr != ConditionStore.end(); ++itr)
    {
        for (ConditionTypeContainer::iterator it = itr->second.begin(); it != itr->second.end(); ++it)
        {
            for (ConditionList::const_iterator i = it->second.begin(); i != it->second.end(); ++i)
                delete *i;
            it->second.clear();
        }
        itr->second.clear();
    }

    ConditionStore.clear();

    for (VehicleSpellConditionContainer::iterator itr = VehicleSpellConditionStore.begin(); itr != VehicleSpellConditionStore.end(); ++itr)
    {
        for (ConditionTypeContainer::iterator it = itr->second.begin(); it != itr->second.end(); ++it)
        {
            for (ConditionList::const_iterator i = it->second.begin(); i != it->second.end(); ++i)
                delete *i;
            it->second.clear();
        }
        itr->second.clear();
    }

    VehicleSpellConditionStore.clear();

    for (SmartEventConditionContainer::iterator itr = SmartEventConditionStore.begin(); itr != SmartEventConditionStore.end(); ++itr)
    {
        for (ConditionTypeContainer::iterator it = itr->second.begin(); it != itr->second.end(); ++it)
        {
            for (ConditionList::const_iterator i = it->second.begin(); i != it->second.end(); ++i)
                delete *i;
            it->second.clear();
        }
        itr->second.clear();
    }

    SmartEventConditionStore.clear();

	for (NpcVendorConditionContainer::iterator itr = NpcVendorConditionContainerStore.begin(); itr != NpcVendorConditionContainerStore.end(); ++itr)
    {
        for (ConditionTypeContainer::iterator it = itr->second.begin(); it != itr->second.end(); ++it)
        {
            for (ConditionList::const_iterator i = it->second.begin(); i != it->second.end(); ++i)
                delete *i;
            it->second.clear();
        }
        itr->second.clear();
    }

    NpcVendorConditionContainerStore.clear();

    // this is a BIG hack, feel free to fix it if you can figure out the ConditionMgr ;)
    for (std::list<Condition*>::const_iterator itr = AllocatedMemoryStore.begin(); itr != AllocatedMemoryStore.end(); ++itr)
        delete *itr;

    AllocatedMemoryStore.clear();
}