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
#include "oculus.h"

class instance_oculus : public InstanceMapScript
{
public:
    instance_oculus() : InstanceMapScript("instance_oculus", 578) { }

    struct instance_oculus_InstanceMapScript : public InstanceScript
    {
        instance_oculus_InstanceMapScript(Map* map) : InstanceScript(map) {}

		uint64 DrakosInterrogatorGUID;
		uint64 VarosCloudstriderGUID;
		uint64 MageLordUromGUID;
		uint64 LeyGuardianEregosGUID;
		uint64 EregosCacheGUID;

		uint8 UromPlatform;
		uint8 CentrifugueConstructCounter;

		std::string str_data;
		std::list<uint64> gameObjectList;
		std::list<uint64> azureDragonsList;

        void Initialize()
        {
            SetBossNumber(MAX_ENCOUNTER);

            DrakosInterrogatorGUID = 0;
            VarosCloudstriderGUID = 0;
            MageLordUromGUID = 0;
            LeyGuardianEregosGUID = 0;

            UromPlatform = 0;
            CentrifugueConstructCounter = 0;

            EregosCacheGUID = 0;

            azureDragonsList.clear();
            gameObjectList.clear();
        }

        void OnCreatureDeath(Creature* creature)
        {
            if (creature->GetEntry() != NPC_CENTRIFUGE_CONSTRUCT)
                return;

             DoUpdateWorldState(WORLD_STATE_CENTRIFUGE_CONSTRUCT_AMOUNT, --CentrifugueConstructCounter);

             if (!CentrifugueConstructCounter)
                if (Creature* varos = instance->GetCreature(VarosCloudstriderGUID))
                    varos->RemoveAllAuras();
        }

        void OnPlayerEnter(Player* player)
        {
            if (GetBossState(DATA_DRAKOS_EVENT) == DONE && GetBossState(DATA_VAROS_EVENT) != DONE)
            {
                player->SendUpdateWorldState(WORLD_STATE_CENTRIFUGE_CONSTRUCT_SHOW, 1);
                player->SendUpdateWorldState(WORLD_STATE_CENTRIFUGE_CONSTRUCT_AMOUNT, CentrifugueConstructCounter);
            }
            else
            {
                player->SendUpdateWorldState(WORLD_STATE_CENTRIFUGE_CONSTRUCT_SHOW, 0);
                player->SendUpdateWorldState(WORLD_STATE_CENTRIFUGE_CONSTRUCT_AMOUNT, 0);
            }
        }

        void ProcessEvent(WorldObject* /*unit*/, uint32 eventId)
        {
            if (eventId != EVENT_CALL_DRAGON)
                return;

            Creature* varos = instance->GetCreature(VarosCloudstriderGUID);

            if (!varos)
                return;

            if (Creature* drake = varos->SummonCreature(NPC_AZURE_RING_CAPTAIN, varos->GetPositionX(), varos->GetPositionY(), varos->GetPositionZ() + 40))
                drake->AI()->DoAction(ACTION_CALL_DRAGON_EVENT);
        }

        void OnCreatureCreate(Creature* creature)
        {
            switch (creature->GetEntry())
            {
                case NPC_DRAKOS_THE_INTERROGATOR:
                    DrakosInterrogatorGUID = creature->GetGUID();
                    break;
                case NPC_VAROS_CLOUDSTRIDER:
                    VarosCloudstriderGUID = creature->GetGUID();
                    break;
                case NPC_LORD_MAGE_UROM:
                    MageLordUromGUID = creature->GetGUID();
                    if (GetBossState(DATA_VAROS_EVENT) != DONE)
						creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                    break;
                case NPC_LEY_GUARDIAN_EREGOS:
                    LeyGuardianEregosGUID = creature->GetGUID();
                    if (GetBossState(DATA_UROM_EVENT) != DONE)
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                    break;
                case NPC_CENTRIFUGE_CONSTRUCT:
                    if (creature->IsAlive())
                        DoUpdateWorldState(WORLD_STATE_CENTRIFUGE_CONSTRUCT_AMOUNT, ++CentrifugueConstructCounter);
                    break;
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case GO_DRAGON_CAGE_DOOR:
                    if (GetBossState(DATA_DRAKOS_EVENT) == DONE)
                        go->SetGoState(GO_STATE_ACTIVE);
                    else
                        go->SetGoState(GO_STATE_READY);
                    gameObjectList.push_back(go->GetGUID());
                    break;
                case GO_EREGOS_CACHE_5N:
                case GO_EREGOS_CACHE_5H:
                    EregosCacheGUID = go->GetGUID();
                    break;
                default:
                    break;
            }
        }

        bool SetBossState(uint32 type, EncounterState state)
        {
            if (!InstanceScript::SetBossState(type, state))
                return false;

            switch (type)
            {
                case DATA_DRAKOS_EVENT:
                    if (state == DONE)
                    {
                        DoUpdateWorldState(WORLD_STATE_CENTRIFUGE_CONSTRUCT_SHOW, 1);
                        DoUpdateWorldState(WORLD_STATE_CENTRIFUGE_CONSTRUCT_AMOUNT, CentrifugueConstructCounter);
                        OpenCageDoors();
                    }
                    break;
                case DATA_VAROS_EVENT:
                    if (state == DONE)
                    {
                        DoUpdateWorldState(WORLD_STATE_CENTRIFUGE_CONSTRUCT_SHOW, 0);
						if (Creature* urom = instance->GetCreature(MageLordUromGUID))
						{
							urom->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
							urom->RemoveAurasDueToSpell(SPELL_ARCANE_SHIELD);
						}
                    }
                    break;
                case DATA_UROM_EVENT:
                    if (state == DONE)
					{
						if (Creature* eregos = instance->GetCreature(LeyGuardianEregosGUID))
							eregos->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
					}
                    break;
                case DATA_EREGOS_EVENT:
                    if (state == DONE)
                        DoRespawnGameObject(EregosCacheGUID, 7*DAY);
                    break;
            }

            return true;
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
                case DATA_UROM_PLATFORM:
                    UromPlatform = data;
                    break;
            }
        }

        uint32 GetData(uint32 type)
        {
            switch (type)
            {
                case DATA_UROM_PLATFORM:
					return UromPlatform;
                // used by condition system
                case DATA_UROM_EVENT:
					return GetBossState(DATA_UROM_EVENT);
            }

            return 0;
        }

        uint64 GetData64(uint32 identifier)
        {
            switch (identifier)
            {
                case DATA_DRAKOS:
					return DrakosInterrogatorGUID;
                case DATA_VAROS:
					return VarosCloudstriderGUID;
                case DATA_UROM:
					return MageLordUromGUID;
                case DATA_EREGOS:
					return LeyGuardianEregosGUID;
            }

            return 0;
        }

        void OpenCageDoors()
        {
            if (gameObjectList.empty())
                return;

            for (std::list<uint64>::const_iterator itr = gameObjectList.begin(); itr != gameObjectList.end(); ++itr)
            {
                if (GameObject* go = instance->GetGameObject(*itr))
                    go->SetGoState(GO_STATE_ACTIVE);
            }
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << "T O " << GetBossSaveData();

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

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2;

            if (dataHead1 == 'T' && dataHead2 == 'O')
            {
                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                {
                    uint32 tmpState;
                    loadStream >> tmpState;
                    if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                        tmpState = NOT_STARTED;
                    SetBossState(i, EncounterState(tmpState));
                }
            }
			else OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_oculus_InstanceMapScript(map);
    }
};

void AddSC_instance_oculus()
{
	new instance_oculus();
}