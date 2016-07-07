/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "nexus.h"

class instance_nexus : public InstanceMapScript
{
public:
    instance_nexus() : InstanceMapScript("instance_nexus", 576) { }

    struct instance_nexus_InstanceMapScript : public InstanceScript
    {
        instance_nexus_InstanceMapScript(Map* map) : InstanceScript(map) {}

        uint32 Encounter[MAX_ENCOUNTERS];
		uint32 TeamInInstance;

		uint64 GrandMagusTelestraGUID;
        uint64 AnomalusGUID;
		uint64 OrmorokGUID;
        uint64 KeristraszaGUID;

        uint64 AnomalusSphereGUID;
        uint64 OrmoroksSphereGUID;
        uint64 TelestrasSphereGUID;

        std::set<uint64> FrayerGUIDlist;
        std::string strInstData;

        void Initialize()
        {
            memset(&Encounter, 0, sizeof(Encounter));

			GrandMagusTelestraGUID = 0;
            AnomalusGUID           = 0;
			OrmorokGUID            = 0;
            KeristraszaGUID        = 0;

			TeamInInstance         = 0;

            FrayerGUIDlist.clear();
        }

		void OnPlayerEnter(Player* player)
		{
			if (!TeamInInstance)
				TeamInInstance = player->GetTeam();
		}

        void OnCreatureCreate(Creature* creature)
        {
			if (!TeamInInstance)
			{
				Map::PlayerList const &players = instance->GetPlayers();
				if (!players.isEmpty())
				{
					if (Player* player = players.begin()->getSource())
						TeamInInstance = player->GetTeam();
				}
			}

            switch (creature->GetEntry())
            {
			    case NPC_GRAND_MAGUS_TELESTRA:
					GrandMagusTelestraGUID = creature->GetGUID();
					break;
                case NPC_ANOMALUS:
                    AnomalusGUID = creature->GetGUID();
                    break;
				case NPC_ORMOROK:
					OrmorokGUID = creature->GetGUID();
					break;
                case NPC_KERISTRASZA:
                    KeristraszaGUID = creature->GetGUID();
                    break;
                case NPC_CRYSTALINE_FRAYER:
                    if (GetData(DATA_ORMOROK_EVENT) == DONE)
                    {
                        creature->UpdateEntry(NPC_WILTED_FRAYER);
                        creature->SetCurrentFaction(35);
                    }
                    else
                    {
                        if (creature->IsAlive())
                            FrayerGUIDlist.insert(creature->GetGUID());
                    }
                    break;
                case NPC_ALLIANCE_BERSERKER:
                {
                    if (ServerAllowsTwoSideGroups())
                        creature->SetCurrentFaction(16);
                    if (TeamInInstance == ALLIANCE)
                        creature->UpdateEntry(NPC_HORDE_BERSERKER, HORDE);
                    break;
                }
				case NPC_ALLIANCE_RANGER:
                {
                    if (ServerAllowsTwoSideGroups())
                        creature->SetCurrentFaction(16);
                    if (TeamInInstance == ALLIANCE)
                        creature->UpdateEntry(NPC_HORDE_RANGER, HORDE);
                    break;
                }
                case NPC_ALLIANCE_CLERIC:
                {
                    if (ServerAllowsTwoSideGroups())
                        creature->SetCurrentFaction(16);
                    if (TeamInInstance == ALLIANCE)
                        creature->UpdateEntry(NPC_HORDE_CLERIC, HORDE);
                    break;
                }
                case NPC_ALLIANCE_COMMANDER:
                {
                    if (ServerAllowsTwoSideGroups())
                        creature->SetCurrentFaction(16);
                    if (TeamInInstance == ALLIANCE)
                        creature->UpdateEntry(NPC_HORDE_COMMANDER, HORDE);
                    break;
                }
                case NPC_COMMANDER_STOUTBEARD:
                {
                    if (ServerAllowsTwoSideGroups())
                        creature->SetCurrentFaction(16);
                    if (TeamInInstance == ALLIANCE)
                        creature->UpdateEntry(NPC_COMMANDER_KOLURG, HORDE);
                    break;
                }
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
				case GO_TELESTRA_SPHERE:
                {
                    TelestrasSphereGUID = go->GetGUID();
                    if (Encounter[0] == DONE)
                        go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    break;
                }
                case GO_ANOMALUS_SPHERE:
                {
                    AnomalusSphereGUID = go->GetGUID();
                    if (Encounter[1] == DONE)
                        go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    break;
                }
                case GO_OMOROK_SPHERE:
                {
                    OrmoroksSphereGUID = go->GetGUID();
                    if (Encounter[2] == DONE)
                        go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    break;
                }
            }
        }

        void ConvertFrayer()
        {
            if (!FrayerGUIDlist.empty())
			{
                for (std::set<uint64>::const_iterator itr = FrayerGUIDlist.begin(); itr != FrayerGUIDlist.end(); ++itr)
                {
                    Creature* frayer = instance->GetCreature(*itr);
                    if (frayer && frayer->IsAlive())
                    {
                        frayer->UpdateEntry(NPC_WILTED_FRAYER);
                        frayer->RemoveAllAuras();
                        frayer->SetCurrentFaction(35);
                        frayer->AI()->EnterEvadeMode();
                    }
                }
			}
        }

        uint32 GetData(uint32 identifier)
        {
            switch (identifier)
            {
                case DATA_MAGUS_TELESTRA_EVENT:
					return Encounter[0];
                case DATA_ANOMALUS_EVENT:
					return Encounter[1];
                case DATA_ORMOROK_EVENT:
					return Encounter[2];
                case DATA_KERISTRASZA_EVENT:
					return Encounter[3];
            }
            return 0;
        }

        void SetData(uint32 identifier, uint32 data)
        {
            switch (identifier)
            {
                case DATA_MAGUS_TELESTRA_EVENT:
                {
                    if (data == DONE)
                    {
                        GameObject* sphere = instance->GetGameObject(TelestrasSphereGUID);
                        if (sphere)
                            sphere->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    }
                    Encounter[0] = data;
                    break;
                }
                case DATA_ANOMALUS_EVENT:
                {
                    if (data == DONE)
                    {
                        if (GameObject* sphere = instance->GetGameObject(AnomalusSphereGUID))
                            sphere->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    }
                    Encounter[1] = data;
                    break;
                }
                case DATA_ORMOROK_EVENT:
                {
                    if (data == DONE)
                    {
                        if (GameObject* sphere = instance->GetGameObject(OrmoroksSphereGUID))
                            sphere->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);

                        ConvertFrayer();
                    }
                    Encounter[2] = data;
                    break;
                }
                case DATA_KERISTRASZA_EVENT:
                    Encounter[3] = data;
                    break;
            }

            if (data == DONE)
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << Encounter[0] << ' ' << Encounter[1] << ' ' << Encounter[2] << ' '
                    << Encounter[3];

                strInstData = saveStream.str();

                SaveToDB();
                OUT_SAVE_INST_DATA_COMPLETE;
            }
        }

		uint64 GetData64(uint32 data)
        {
            switch (data)
            {
			    case DATA_MAGUS_TELESTRA:
					return GrandMagusTelestraGUID;
			    case DATA_ANOMALUS:
					return AnomalusGUID;
				case DATA_ORMOROK:
					return OrmorokGUID;
                case DATA_KERISTRASZA:
					return KeristraszaGUID;
                case ANOMALUS_CONTAINMET_SPHERE:
					return AnomalusSphereGUID;
                case ORMOROKS_CONTAINMET_SPHERE:
					return OrmoroksSphereGUID;
                case TELESTRAS_CONTAINMET_SPHERE:
					return TelestrasSphereGUID;
            }
            return 0;
        }

        std::string GetSaveData()
        {
            return strInstData;
        }

        void Load(const char* chrIn)
        {
            if (!chrIn)
            {
                OUT_LOAD_INST_DATA_FAIL;
                return;
            }

            OUT_LOAD_INST_DATA(chrIn);

            std::istringstream loadStream(chrIn);
            loadStream >> Encounter[0] >> Encounter[1] >> Encounter[2] >> Encounter[3];

            for (uint8 i = 0; i < MAX_ENCOUNTERS; ++i)
                if (Encounter[i] == IN_PROGRESS)
                    Encounter[i] = NOT_STARTED;

            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_nexus_InstanceMapScript(map);
    }
};

void AddSC_instance_nexus()
{
    new instance_nexus();
}