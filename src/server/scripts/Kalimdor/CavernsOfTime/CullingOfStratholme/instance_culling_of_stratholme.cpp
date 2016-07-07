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
#include "culling_of_stratholme.h"

class instance_culling_of_stratholme : public InstanceMapScript
{
public:
    instance_culling_of_stratholme() : InstanceMapScript("instance_culling_of_stratholme", 595) { }

    struct instance_culling_of_stratholme_InstanceMapScript : public InstanceScript
    {
        instance_culling_of_stratholme_InstanceMapScript(Map* map) : InstanceScript(map) {}

        uint64 Arthas;
        uint64 Meathook;
        uint64 Salramm;
        uint64 Epoch;
        uint64 MalGanis;
        uint64 Infinite;

        uint64 ShkafGate;
        uint64 MalGanisGate1;
        uint64 MalGanisGate2;
        uint64 ExitGate;
        uint64 MalGanisChestGUID;
        uint32 CountdownTimer;
        uint16 CountdownMinute;

        uint32 Encounter[MAX_ENCOUNTER];
        std::string str_data;

        uint32 uiCountCrates;
   

        void Initialize()
        {
            memset(&Encounter, 0, sizeof(Encounter));

            uiCountCrates = 0;
            CountdownTimer = 0;
            CountdownMinute = 26;
        }

        bool IsEncounterInProgress() const
        {
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                if (Encounter[i] == IN_PROGRESS) return true;

            return false;
        }

        void OnCreatureCreate(Creature* creature)
        {
            switch (creature->GetEntry())
            {
                case NPC_ARTHAS:
                    Arthas = creature->GetGUID();
                    break;
                case NPC_MEATHOOK:
                    Meathook = creature->GetGUID();
                    break;
                case NPC_SALRAMM:
                    Salramm = creature->GetGUID();
                    break;
                case NPC_EPOCH_HUNTER:
                    Epoch = creature->GetGUID();
                    break;
                case NPC_MAL_GANIS:
                    MalGanis = creature->GetGUID();
                    break;
                case NPC_INFINITE_THE_CORRUPTOR:
                    Infinite = creature->GetGUID();
                    creature->SetVisible(false);
                    creature->SetCurrentFaction(35);
                    break;
                case NPC_ZOMBIE:
                    if (Encounter[6] == NOT_STARTED && creature->GetPositionX() < 2205.0f && creature->GetPositionY() < 1320.0f)
                        creature->UpdateEntry(roll_chance_i(50) ? NPC_CITY_MAN : NPC_CITY_MAN2);
                    creature->SetCorpseDelay(3);
                    break;
            }
        }

        void OnCreatureDeath(Creature* creature)
        {
            if (creature->GetEntry() == NPC_ZOMBIE)
            {
                Map::PlayerList const &PlayerList = instance->GetPlayers();

                if (PlayerList.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
				{
                    if (Player* player = i->getSource())
                    {
                        if (player->IsAtGroupRewardDistance(creature)) // already handled
                            continue;

                        if (player->IsAlive() || !player->GetCorpse())
                            player->KilledMonsterCredit(NPC_ZOMBIE, 0);
                    }
				}
            }
        }

        void GiveQuestKillAllInInstance(uint32 entry)
        {
            Map::PlayerList const &PlayerList = instance->GetPlayers();

            if (PlayerList.isEmpty())
                return;

            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            {
                if (Player* player = i->getSource())
                    player->KilledMonsterCredit(entry, 0);
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case GO_SHKAF_GATE:
                    ShkafGate = go->GetGUID();
                    break;
                case GO_MALGANIS_GATE_1:
                    MalGanisGate1 = go->GetGUID();
                    break;
                case GO_MALGANIS_GATE_2:
                    MalGanisGate2 = go->GetGUID();
                    break;
                case GO_EXIT_GATE:
                    ExitGate = go->GetGUID();
                    if (Encounter[4] == DONE)
                        HandleGameObject(ExitGate, true);
                    break;
                case GO_MALGANIS_CHEST_N:
                case GO_MALGANIS_CHEST_H:
                    MalGanisChestGUID = go->GetGUID();
                    if (Encounter[4] == DONE)
                        go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
                    break;
            }
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
                case DATA_CRATES_EVENT:
                    switch (data)
                    {
                        case 0: // Entfernt Worldstate
                            DoUpdateWorldState(WORLDSTATE_NUMBER_CRATES_SHOW, 0);
                            break;
                        case 1: // Addiert ein Punkt
                            ++uiCountCrates;
                            DoUpdateWorldState(WORLDSTATE_NUMBER_CRATES_COUNT, uiCountCrates);
                            if (uiCountCrates >= 5)
                            {
                                Encounter[0] = DONE;
                                GiveQuestKillAllInInstance(NPC_DISPELLING_ILLUSIONS);
                                SaveToDB();
                            }
                            break;
                        case 2: // Startet den Worldstate
                            uiCountCrates = 0;
                            Encounter[0] = IN_PROGRESS;
                            DoUpdateWorldState(WORLDSTATE_NUMBER_CRATES_SHOW, 1);
                            DoUpdateWorldState(WORLDSTATE_NUMBER_CRATES_COUNT, uiCountCrates);
                            break;
                    }
                    break;
                case DATA_MEATHOOK_EVENT:
                    Encounter[1] = data;
                    break;
                case DATA_SALRAMM_EVENT:
                    Encounter[2] = data;
                    break;
                case DATA_EPOCH_EVENT:
                    Encounter[3] = data;
                    break;
                case DATA_MAL_GANIS_EVENT:
                    Encounter[4] = data;
                    switch (data)
                    {
                        case NOT_STARTED:
                            HandleGameObject(MalGanisGate2, true);
                            break;
                        case IN_PROGRESS:
                            HandleGameObject(MalGanisGate2, false);
                            break;
                        case DONE:
                            GiveQuestKillAllInInstance(NPC_A_ROYAL_ESCORT);
                            HandleGameObject(ExitGate, true);
                            HandleGameObject(MalGanisGate2, true);
                            if (GameObject* go = instance->GetGameObject(MalGanisChestGUID))
                                go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
                            break;
                    }
                    break;
                case DATA_INFINITE_EVENT:
                    Encounter[5] = data;
                    switch (data)
                    {
                        case DONE:
                            CountdownMinute = 0;
                            DoUpdateWorldState(WORLDSTATE_NUMBER_INFINITE_SHOW, 0);
                            break;
                        case IN_PROGRESS:
                        {
                            if (Creature* corrupter = instance->GetCreature(Infinite))
                            {
                                corrupter->SetVisible(true);
                                corrupter->RestoreFaction();
                            }
                            break;
                        }
                    }
                    break;
                case DATA_ARTHAS_EVENT:
                    Encounter[6] = data;
                    break;
            }

            if (data == DONE)
                SaveToDB();
        }

        uint32 GetData(uint32 type)
        {
            switch (type)
            {
                case DATA_CRATES_EVENT:
					return Encounter[0];
                case DATA_MEATHOOK_EVENT:
					return Encounter[1];
                case DATA_SALRAMM_EVENT:
					return Encounter[2];
                case DATA_EPOCH_EVENT:
					return Encounter[3];
                case DATA_MAL_GANIS_EVENT:
					return Encounter[4];
                case DATA_INFINITE_EVENT:
					return Encounter[5];
                case DATA_ARTHAS_EVENT:
					return Encounter[6];
                case DATA_COUNTDOWN:
					return CountdownMinute;
            }
            return 0;
        }

        uint64 GetData64(uint32 identifier)
        {
            switch (identifier)
            {
                case DATA_ARTHAS:
					return Arthas;
                case DATA_MEATHOOK:
					return Meathook;
                case DATA_SALRAMM:
					return Salramm;
                case DATA_EPOCH:
					return Epoch;
                case DATA_MAL_GANIS:
					return MalGanis;
                case DATA_INFINITE:
					return Infinite;
                case DATA_SHKAF_GATE:
					return ShkafGate;
                case DATA_MAL_GANIS_GATE_1:
					return MalGanisGate1;
                case DATA_MAL_GANIS_GATE_2:
					return MalGanisGate2;
                case DATA_EXIT_GATE:
					return ExitGate;
                case DATA_MAL_GANIS_CHEST:
					return MalGanisChestGUID;
            }
            return 0;
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << "C S " << Encounter[0] << " " << Encounter[1] << " "
                << Encounter[2] << " " << Encounter[3] << " " << Encounter[4] << " " << Encounter[5] << " " << Encounter[6] << " " << CountdownMinute;

            str_data = saveStream.str();

            OUT_SAVE_INST_DATA_COMPLETE;
            return str_data;
        }

        void Load(const char* in)
        {
            if (!in)
            {
                OUT_LOAD_INST_DATA_FAIL;
                return;
            }

            OUT_LOAD_INST_DATA(in);

            char dataHead1, dataHead2;
            uint16 data0, data1, data2, data3, data4, data5, data6, data7;

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3 >> data4 >> data5 >> data6 >> data7;

            if (dataHead1 == 'C' && dataHead2 == 'S')
            {
                Encounter[0] = data0;
                Encounter[1] = data1;
                Encounter[2] = data2;
                Encounter[3] = data3;
                Encounter[4] = data4;
                Encounter[5] = data5;
                Encounter[6] = data6;
                CountdownMinute = data7;

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (Encounter[i] == IN_PROGRESS || Encounter[i] == SPECIAL)
                        Encounter[i] = NOT_STARTED;

            }
			else OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }

        void Update(uint32 diff)
        {
            if (GetData(DATA_INFINITE_EVENT) == SPECIAL || GetData(DATA_INFINITE_EVENT) == IN_PROGRESS)
			{
                if (CountdownMinute)
                {
                    if (CountdownTimer < diff)
                    {
                        --CountdownMinute;

                        if (CountdownMinute)
                        {
                            DoUpdateWorldState(WORLDSTATE_NUMBER_INFINITE_SHOW, 1);
                            DoUpdateWorldState(WORLDSTATE_NUMBER_INFINITE_TIMER, CountdownMinute);
                        }
                        else
                        {
                            DoUpdateWorldState(WORLDSTATE_NUMBER_INFINITE_SHOW, 0);
                        }
                        SaveToDB();
                        CountdownTimer += 60000;
                    }
                    CountdownTimer -= diff;
				}
			}
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_culling_of_stratholme_InstanceMapScript(map);
    }
};

void AddSC_instance_culling_of_stratholme()
{
    new instance_culling_of_stratholme();
}