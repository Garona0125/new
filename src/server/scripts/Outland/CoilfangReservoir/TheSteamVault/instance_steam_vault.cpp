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
#include "steam_vault.h"

class go_main_chambers_access_panel : public GameObjectScript
{
public:
    go_main_chambers_access_panel() : GameObjectScript("go_main_chambers_access_panel") { }

    bool OnGossipHello(Player* /*player*/, GameObject* go)
    {
        InstanceScript* instance = go->GetInstanceScript();

        if (!instance)
            return false;

        if (go->GetEntry() == GO_ACCESS_PANEL_HYDROMANCER && (instance->GetData(TYPE_HYDROMANCER_THESPIA) == DONE || instance->GetData(TYPE_HYDROMANCER_THESPIA) == SPECIAL))
            instance->SetData(TYPE_HYDROMANCER_THESPIA, SPECIAL);

        if (go->GetEntry() == GO_ACCESS_PANEL_MEKGINEER && (instance->GetData(TYPE_MEKGINEER_STEAMRIGGER) == DONE || instance->GetData(TYPE_MEKGINEER_STEAMRIGGER) == SPECIAL))
            instance->SetData(TYPE_MEKGINEER_STEAMRIGGER, SPECIAL);

        return true;
    }
};

class instance_steam_vault : public InstanceMapScript
{
public:
    instance_steam_vault() : InstanceMapScript("instance_steam_vault", 545) { }

    struct instance_steam_vault_InstanceMapScript : public InstanceScript
    {
        instance_steam_vault_InstanceMapScript(Map* map) : InstanceScript(map) {}

        uint32 Encounter[MAX_ENCOUNTER];

        uint64 HydromancerThespiaGUID;
        uint64 MekgineerSteamriggerGUID;
        uint64 WarlordKalithreshGUID;

        uint64 KalithreshDoorGUID;
        uint64 AccessPanelHydromancerGUID;
        uint64 AccessPanelMekgineerGUID;

        void Initialize()
        {
            memset(&Encounter, 0, sizeof(Encounter));

            HydromancerThespiaGUID     = 0;
            MekgineerSteamriggerGUID   = 0;
            WarlordKalithreshGUID      = 0;
            KalithreshDoorGUID         = 0;
            AccessPanelHydromancerGUID = 0;
            AccessPanelMekgineerGUID   = 0;
        }

        bool IsEncounterInProgress() const
        {
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                 if (Encounter[i] == IN_PROGRESS)
                     return true;

            return false;
        }

        void OnCreatureCreate(Creature* creature)
        {
              switch (creature->GetEntry())
			  {
              case NPC_GYDROMANCER_THESPIA:
				  HydromancerThespiaGUID = creature->GetGUID();
				  break;
              case NPC_MEKGINEER_STEAMRIGGER:
				  MekgineerSteamriggerGUID = creature->GetGUID();
				  break;
              case NPC_WARLORD_KALITHRESH:
				  WarlordKalithreshGUID = creature->GetGUID();
				  break;
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
            case GO_KALITHRESH_DOOR:
				KalithreshDoorGUID = go->GetGUID();
				break;
            case GO_ACCESS_PANEL_HYDROMANCER:
				AccessPanelHydromancerGUID = go->GetGUID();
				break;
            case GO_ACCESS_PANEL_MEKGINEER:
				AccessPanelMekgineerGUID = go->GetGUID();
				break;
            }
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
                case TYPE_HYDROMANCER_THESPIA:
                    if (data == SPECIAL)
                    {
                        HandleGameObject(AccessPanelHydromancerGUID, true);

                        if (GetData(TYPE_MEKGINEER_STEAMRIGGER) == SPECIAL)
                            HandleGameObject(KalithreshDoorGUID, true);

                        sLog->outDebug(LOG_FILTER_QCSCR, "QUANTUMCORE SCRIPTS: Instance Steamvault: Access panel used.");
                    }
                    Encounter[0] = data;
                    break;
                case TYPE_MEKGINEER_STEAMRIGGER:
                    if (data == SPECIAL)
                    {
                        HandleGameObject(AccessPanelMekgineerGUID, true);

                        if (GetData(TYPE_HYDROMANCER_THESPIA) == SPECIAL)
                            HandleGameObject(KalithreshDoorGUID, true);

                        sLog->outDebug(LOG_FILTER_QCSCR, "QUANTUMCORE SCRIPTS: Instance Steamvault: Access panel used.");
                    }
                    Encounter[1] = data;
                    break;
                case TYPE_WARLORD_KALITHRESH:
                    Encounter[2] = data;
                    break;
                case TYPE_DISTILLER:
                    Encounter[3] = data;
                    break;
            }

            if (data == DONE || data == SPECIAL)
                SaveToDB();
        }

        uint32 GetData(uint32 type)
        {
            switch (type)
            {
                case TYPE_HYDROMANCER_THESPIA:
                    return Encounter[0];
                case TYPE_MEKGINEER_STEAMRIGGER:
                    return Encounter[1];
                case TYPE_WARLORD_KALITHRESH:
                    return Encounter[2];
                case TYPE_DISTILLER:
                    return Encounter[3];
            }
            return 0;
        }

        uint64 GetData64(uint32 data)
        {
            switch (data)
            {
                case DATA_THESPIA:
                    return HydromancerThespiaGUID;
                case DATA_MEKGINEER_STEAMRIGGER:
                    return MekgineerSteamriggerGUID;
                case DATA_KALITRESH:
                    return WarlordKalithreshGUID;
            }
            return 0;
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream stream;
            stream << Encounter[0] << ' ' << Encounter[1] << ' ' << Encounter[2] << ' ' << Encounter[3];

			OUT_SAVE_INST_DATA_COMPLETE;
			return stream.str();
        }

        void Load(const char* in)
        {
            if (!in)
            {
                OUT_LOAD_INST_DATA_FAIL;
                return;
            }
            OUT_LOAD_INST_DATA(in);
            std::istringstream stream(in);
            stream >> Encounter[0] >> Encounter[1] >> Encounter[2] >> Encounter[3];
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                if (Encounter[i] == IN_PROGRESS)
                    Encounter[i] = NOT_STARTED;
            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_steam_vault_InstanceMapScript(map);
    }
};

void AddSC_instance_steam_vault()
{
    new go_main_chambers_access_panel();
    new instance_steam_vault();
}