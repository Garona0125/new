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
#include "CreatureAI.h"
#include "molten_core.h"

class instance_molten_core : public InstanceMapScript
{
    public:
        instance_molten_core() : InstanceMapScript("instance_molten_core", 409) { }

        struct instance_molten_core_InstanceMapScript : public InstanceScript
        {
            instance_molten_core_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                SetBossNumber(MAX_ENCOUNTER);
                golemaggTheIncineratorGUID = 0;
                majordomoExecutusGUID = 0;
                cacheOfTheFirelordGUID = 0;
                executusSchedule = NULL;
                deadBossCount = 0;
                ragnarosAddDeaths = 0;
                isLoading = false;
                summonedExecutus = false;
            }

			uint8 deadBossCount;
            uint8 ragnarosAddDeaths;

			uint64 golemaggTheIncineratorGUID;
            uint64 majordomoExecutusGUID;
            uint64 cacheOfTheFirelordGUID;

			bool* executusSchedule;
            bool isLoading;
            bool summonedExecutus;

            ~instance_molten_core_InstanceMapScript()
            {
                delete executusSchedule;
            }

            void OnPlayerEnter(Player* /*player*/)
            {
                if (executusSchedule)
                {
                    SummonMajordomoExecutus(*executusSchedule);
                    delete executusSchedule;
                    executusSchedule = NULL;
                }
            }

            void OnCreatureCreate(Creature* creature)
            {
                switch (creature->GetEntry())
                {
                    case NPC_GOLEMAGG_THE_INCINERATOR:
                        golemaggTheIncineratorGUID = creature->GetGUID();
                        break;
                    case NPC_MAJORDOMO_EXECUTUS:
                        majordomoExecutusGUID = creature->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            void OnGameObjectCreate(GameObject* go)
            {
                switch (go->GetEntry())
                {
                    case GO_CACHE_OF_THE_FIRELORD:
                        cacheOfTheFirelordGUID = go->GetGUID();
                        break;
                    default:
                        break;
                }
            }

            void SetData(uint32 type, uint32 data)
            {
                if (type == DATA_RAGNAROS_ADDS)
                {
                    if (data == 1)
                        ++ragnarosAddDeaths;
                    else if (data == 0)
                        ragnarosAddDeaths = 0;
                }
            }

            uint32 GetData(uint32 type)
            {
                switch (type)
                {
                    case DATA_RAGNAROS_ADDS:
                        return ragnarosAddDeaths;
                }

                return 0;
            }

            uint64 GetData64(uint32 type)
            {
                switch (type)
                {
                    case BOSS_GOLEMAGG_THE_INCINERATOR:
                        return golemaggTheIncineratorGUID;
                    case BOSS_MAJORDOMO_EXECUTUS:
                        return majordomoExecutusGUID;
                }

                return 0;
            }

            bool SetBossState(uint32 bossId, EncounterState state)
            {
                if (!InstanceScript::SetBossState(bossId, state))
                    return false;

                if (state == DONE && bossId < BOSS_MAJORDOMO_EXECUTUS)
                    ++deadBossCount;

                if (isLoading)
                    return true;

                if (deadBossCount == 8)
                    SummonMajordomoExecutus(false);

                if (bossId == BOSS_MAJORDOMO_EXECUTUS && state == DONE)
                    DoRespawnGameObject(cacheOfTheFirelordGUID, 7 * DAY);

                return true;
            }

            void SummonMajordomoExecutus(bool done)
            {
                if (summonedExecutus)
                    return;

                summonedExecutus = true;
                if (!done)
                {
                    instance->SummonCreature(NPC_MAJORDOMO_EXECUTUS, SummonPositions[0]);
                    instance->SummonCreature(NPC_FLAMEWAKER_HEALER, SummonPositions[1]);
                    instance->SummonCreature(NPC_FLAMEWAKER_HEALER, SummonPositions[2]);
                    instance->SummonCreature(NPC_FLAMEWAKER_HEALER, SummonPositions[3]);
                    instance->SummonCreature(NPC_FLAMEWAKER_HEALER, SummonPositions[4]);
                    instance->SummonCreature(NPC_FLAMEWAKER_ELITE, SummonPositions[5]);
                    instance->SummonCreature(NPC_FLAMEWAKER_ELITE, SummonPositions[6]);
                    instance->SummonCreature(NPC_FLAMEWAKER_ELITE, SummonPositions[7]);
                    instance->SummonCreature(NPC_FLAMEWAKER_ELITE, SummonPositions[8]);
                }
                else if (TempSummon* summon = instance->SummonCreature(NPC_MAJORDOMO_EXECUTUS, RagnarosTelePos))
                        summon->AI()->DoAction(ACTION_START_RAGNAROS_ALT);
            }

            std::string GetSaveData()
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;
                saveStream << "M C " << GetBossSaveData();

                OUT_SAVE_INST_DATA_COMPLETE;
                return saveStream.str();
            }

            void Load(char const* data)
            {
                if (!data)
                {
                    OUT_LOAD_INST_DATA_FAIL;
                    return;
                }

                isLoading = true;
                OUT_LOAD_INST_DATA(data);

                char dataHead1, dataHead2;

                std::istringstream loadStream(data);
                loadStream >> dataHead1 >> dataHead2;

                if (dataHead1 == 'M' && dataHead2 == 'C')
                {
                    EncounterState states[MAX_ENCOUNTER];
                    uint8 executusCounter = 0;

                    // need 2 loops to check spawning executus/ragnaros
                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    {
                        uint32 tmpState;
                        loadStream >> tmpState;
                        if (tmpState == IN_PROGRESS || tmpState > TO_BE_DECIDED)
                            tmpState = NOT_STARTED;
                        states[i] = EncounterState(tmpState);

                         if (tmpState == DONE && i < BOSS_MAJORDOMO_EXECUTUS)
                            ++executusCounter;
                   }

                    if (executusCounter >= 8 && states[BOSS_RAGNAROS] != DONE)
                        executusSchedule = new bool(states[BOSS_MAJORDOMO_EXECUTUS] == DONE);

                    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                        SetBossState(i, states[i]);
                }
                else
                    OUT_LOAD_INST_DATA_FAIL;

                OUT_LOAD_INST_DATA_COMPLETE;
                isLoading = false;
            }
        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const
        {
            return new instance_molten_core_InstanceMapScript(map);
        }
};

void AddSC_instance_molten_core()
{
    new instance_molten_core();
}