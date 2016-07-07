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
#include "ahnkahet.h"

class instance_ahnkahet : public InstanceMapScript
{
public:
    instance_ahnkahet() : InstanceMapScript("instance_ahnkahet", 619) {}

    struct instance_ahnkahet_InstanceScript : public InstanceScript
    {
        instance_ahnkahet_InstanceScript(Map* map) : InstanceScript(map) {}

        uint64 ElderNadoxGUID;
        uint64 PrinceTaldaramGUID;
        uint64 JedogaShadowseekerGUID;
        uint64 HeraldVolazjGUID;
        uint64 AmanitarGUID;

        uint64 PrinceTaldaramSpheresGUID[2];
        uint64 PrinceTaldaramPlatformGUID;
        uint64 PrinceTaldaramGateGUID;

        std::set<uint64> InitiandGUIDs;
        uint64 JedogaSacrifices;
        uint64 JedogaTarget;

        uint32 Encounter[MAX_ENCOUNTER];
        uint32 spheres[2];
		
		uint8 InitiandCnt,switchtrigger,initiandkilled;

        std::string str_data;

        void Initialize()
        {
            memset(&Encounter, 0, sizeof(Encounter));
            InitiandGUIDs.clear();

            ElderNadoxGUID = 0;
            PrinceTaldaramGUID = 0;
            JedogaShadowseekerGUID = 0;
            HeraldVolazjGUID = 0;
            AmanitarGUID = 0;

            spheres[0] = NOT_STARTED;
            spheres[1] = NOT_STARTED;

            InitiandCnt = 0;
            switchtrigger = 0;
            initiandkilled = 0;
            JedogaSacrifices = 0;
            JedogaTarget = 0;
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
                case 29309:
					ElderNadoxGUID = creature->GetGUID();
					break;
                case 29308:
					PrinceTaldaramGUID = creature->GetGUID();
					break;
                case 29310:
					JedogaShadowseekerGUID = creature->GetGUID();
					break;
                case 29311:
					HeraldVolazjGUID = creature->GetGUID();
					break;
                case 30258:
					AmanitarGUID = creature->GetGUID();
					break;
                case 30114:
					InitiandGUIDs.insert(creature->GetGUID());
					break;
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case 193564:
                    PrinceTaldaramPlatformGUID = go->GetGUID();
                    if (Encounter[1] == DONE)
                        HandleGameObject(NULL, true, go);
                    break;
                case 193093:
                    PrinceTaldaramSpheresGUID[0] = go->GetGUID();
                    if (spheres[0] == IN_PROGRESS)
                    {
                        go->SetGoState(GO_STATE_ACTIVE);
                        go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    }
                    else go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    break;
                case 193094:
                    PrinceTaldaramSpheresGUID[1] = go->GetGUID();
                    if (spheres[1] == IN_PROGRESS)
                    {
                        go->SetGoState(GO_STATE_ACTIVE);
                        go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    }
                    else go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    break;
                case 192236:
                    PrinceTaldaramGateGUID = go->GetGUID(); // Web gate past Prince Taldaram
                    if (Encounter[1] == DONE)
                        HandleGameObject(NULL, true, go);
					break;
            }
        }

        void SetData64(uint32 idx, uint64 guid)
        {
            switch (idx)
            {
                case DATA_ADD_JEDOGA_OPFER:
					JedogaSacrifices = guid;
					break;
                case DATA_PL_JEDOGA_TARGET:
					JedogaTarget = guid;
					break;
            }
        }

        uint64 GetData64(uint32 identifier)
        {
            switch (identifier)
            {
                case DATA_ELDER_NADOX:
					return ElderNadoxGUID;
                case DATA_PRINCE_TALDARAM:
					return PrinceTaldaramGUID;
                case DATA_JEDOGA_SHADOWSEEKER:
					return JedogaShadowseekerGUID;
                case DATA_HERALD_VOLAZJ:
					return HeraldVolazjGUID;
                case DATA_AMANITAR:
					return AmanitarGUID;
                case DATA_SPHERE1:
					return PrinceTaldaramSpheresGUID[0];
                case DATA_SPHERE2:
					return PrinceTaldaramSpheresGUID[1];
                case DATA_PRINCE_TALDARAM_PLATFORM:
					return PrinceTaldaramPlatformGUID;
                case DATA_ADD_JEDOGA_INITIAND:
                {
                    std::vector<uint64> vInitiands;
                    vInitiands.clear();
                    for (std::set<uint64>::const_iterator itr = InitiandGUIDs.begin(); itr != InitiandGUIDs.end(); ++itr)
                    {
                        Creature* creature = instance->GetCreature(*itr);
                        if (creature && creature->IsAlive())
                            vInitiands.push_back(*itr);
                    }
                    if (vInitiands.empty())
                        return 0;
                    uint8 j = urand(0, vInitiands.size() -1);
                    return vInitiands[j];
                }
                case DATA_ADD_JEDOGA_OPFER:
					return JedogaSacrifices;
                case DATA_PL_JEDOGA_TARGET:
					return JedogaTarget;
            }
            return 0;
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
                case DATA_ELDER_NADOX_EVENT:
                    Encounter[0] = data;
                    break;
                case DATA_PRINCE_TALDARAM_EVENT:
                    if (data == DONE)
                        HandleGameObject(PrinceTaldaramGateGUID, true);
                    Encounter[1] = data;
                    break;
                case DATA_JEDOGA_SHADOWSEEKER_EVENT:
                    Encounter[2] = data;
                    if (data == DONE)
                    {
                        for (std::set<uint64>::const_iterator itr = InitiandGUIDs.begin(); itr != InitiandGUIDs.end(); ++itr)
                        {
                            Creature* creature = instance->GetCreature(*itr);
                            if (creature && creature->IsAlive())
                            {
                                creature->SetVisible(false);
                                creature->setDeathState(JUST_DIED);
                                creature->RemoveCorpse();
                            }
                        }
                        if (!initiandkilled && instance->IsHeroic())
                            DoCompleteAchievement(ACHIEVEMENT_VOLUNTEER_WORK);
                    }
                    break;
                case DATA_HERALD_VOLAZJ_EVENT:
                    Encounter[3] = data;
                    break;
                case DATA_AMANITAR_EVENT:
                    Encounter[4] = data;
                    break;
                case DATA_SPHERE1_EVENT:
                    spheres[0] = data;
                    break;
                case DATA_SPHERE2_EVENT:
                    spheres[1] = data;
                    break;
                case DATA_JEDOGA_TRIGGER_SWITCH:
                    switchtrigger = data;
                    break;
                case DATA_INITIAND_KILLED:
                    initiandkilled = data;
                    break;
                case DATA_JEDOGA_RESET_INITIANDS:
                    for (std::set<uint64>::const_iterator itr = InitiandGUIDs.begin(); itr != InitiandGUIDs.end(); ++itr)
                    {
                        Creature* creature = instance->GetCreature(*itr);
                        if (creature)
                        {
                            creature->Respawn();
                            if (!creature->IsInEvadeMode()) creature->AI()->EnterEvadeMode();
                        }
                    }
                    break;
            }
            if (data == DONE)
                SaveToDB();
        }

        uint32 GetData(uint32 type)
        {
            switch (type)
            {
                case DATA_ELDER_NADOX_EVENT:
					return Encounter[0];
                case DATA_PRINCE_TALDARAM_EVENT:
					return Encounter[1];
                case DATA_JEDOGA_SHADOWSEEKER_EVENT:
					return Encounter[2];
                case DATA_HERALD_VOLAZJ:
					return Encounter[3];
                case DATA_AMANITAR_EVENT:
					return Encounter[4];
                case DATA_SPHERE1_EVENT:
					return spheres[0];
                case DATA_SPHERE2_EVENT:
					return spheres[1];
                case DATA_ALL_INITIAND_DEAD:
                    for (std::set<uint64>::const_iterator itr = InitiandGUIDs.begin(); itr != InitiandGUIDs.end(); ++itr)
                    {
                        Creature* creature = instance->GetCreature(*itr);
                        if (!creature || (creature && creature->IsAlive())) return 0;
                    }
                    return 1;
                case DATA_JEDOGA_TRIGGER_SWITCH:
					return switchtrigger;
                case DATA_INITIAND_KILLED:
					return initiandkilled;
            }
            return 0;
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << "A K " << Encounter[0] << ' ' << Encounter[1] << ' '
                << Encounter[2] << ' ' << Encounter[3] << ' ' << Encounter[4] << ' '
                << spheres[0] << ' ' << spheres[1];

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
            uint16 data0, data1, data2, data3, data4, data5, data6;

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3 >> data4 >> data5 >> data6;

            if (dataHead1 == 'A' && dataHead2 == 'K')
            {
                Encounter[0] = data0;
                Encounter[1] = data1;
                Encounter[2] = data2;
                Encounter[3] = data3;
                Encounter[4] = data4;

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (Encounter[i] == IN_PROGRESS)
                        Encounter[i] = NOT_STARTED;

                spheres[0] = data5;
                spheres[1] = data6;

            }
			else OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
       return new instance_ahnkahet_InstanceScript(map);
    }
};

void AddSC_instance_ahnkahet()
{
   new instance_ahnkahet;
}