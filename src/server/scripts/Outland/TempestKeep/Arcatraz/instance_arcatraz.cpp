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
#include "arcatraz.h"

class instance_arcatraz : public InstanceMapScript
{
public:
    instance_arcatraz() : InstanceMapScript("instance_arcatraz", 552) { }

    struct instance_arcatraz_InstanceMapScript : public InstanceScript
    {
		instance_arcatraz_InstanceMapScript(Map* map) : InstanceScript(map) {}

        uint32 Encounter[MAX_ENCOUNTER];

        uint64 Containment_Core_Security_Field_Alpha;
        uint64 Containment_Core_Security_Field_Beta;
        GameObject* Pod_Alpha;
        GameObject* Pod_Gamma;
        GameObject* Pod_Beta;
        GameObject* Pod_Delta;
        GameObject* Pod_Omega;
        GameObject* Wardens_Shield;

        uint64 GoSphereGUID;

        uint64 MellicharGUID;
		uint64 MillhouseGUID;

        uint32 socco_dead;
        uint32 dalli_dead;
        uint64 DalliahGUID;
        uint64 SoccothraesGUID;
        uint32 d_hp25;
        uint32 s_hp25;

        void Initialize()
        {
            memset(&Encounter, 0, sizeof(Encounter));

            Containment_Core_Security_Field_Alpha = NULL;
            Containment_Core_Security_Field_Beta  = NULL;
            Pod_Alpha                             = NULL;
            Pod_Beta                              = NULL;
            Pod_Delta                             = NULL;
            Pod_Gamma                             = NULL;
            Pod_Omega                             = NULL;
            Wardens_Shield                        = NULL;

            socco_dead                            = NOT_STARTED;
            dalli_dead                            = NOT_STARTED;

            GoSphereGUID                          = NULL;
            MellicharGUID                         = NULL;
			MillhouseGUID                         = NULL;

            DalliahGUID                           = NULL;
            SoccothraesGUID                       = NULL;
            d_hp25                                = NULL;
            s_hp25                                = NULL;
        }

        bool IsEncounterInProgress() const
        {
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                if (Encounter[i] == IN_PROGRESS)
					return true;

            return false;
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch(go->GetEntry())
            {
                case GO_CONTAINMENT_CORE_SECURITY_FIELD_ALPHA: 
                    Containment_Core_Security_Field_Alpha = go->GetGUID();
                    if (socco_dead == DONE)
                        HandleGameObject(Containment_Core_Security_Field_Alpha, true, go);
                    else
                        HandleGameObject(Containment_Core_Security_Field_Alpha, false, go);
                    break;
                case GO_CONTAINMENT_CORE_SECURITY_FIELD_BETA:
                    Containment_Core_Security_Field_Beta = go->GetGUID();
                     if (dalli_dead == DONE)
                        HandleGameObject(Containment_Core_Security_Field_Beta, true, go);
                    else
                        HandleGameObject(Containment_Core_Security_Field_Beta, false, go);
                    break;
                    break;
                case GO_SEAL_SPHERE:
					GoSphereGUID = go->GetGUID();
					break;
                case GO_POD_ALPHA:
					Pod_Alpha = go;
					break;
                case GO_POD_BETA:
					Pod_Beta =  go;
					break;
                case GO_POD_DELTA:
					Pod_Delta = go;
					break;
                case GO_POD_GAMMA:
					Pod_Gamma = go;
					break;
                case GO_POD_OMEGA:
					Pod_Omega = go;
					break;
                //case GO_WARDENS_SHIELD:
					//Wardens_Shield = go;
					//break;
            }
        }

        void OnCreatureCreate(Creature* creature)
        {
            switch(creature->GetEntry())
            {
                case NPC_MELLICHAR:
					MellicharGUID = creature->GetGUID();
					break;
				case NPC_DALLIAH:
					DalliahGUID = creature->GetGUID();
					break;
				case NPC_WRATH_SCRYER:
					SoccothraesGUID = creature->GetGUID();
					break;
				case NPC_MILLHOUSE_MANASTORM:
					MillhouseGUID = creature->GetGUID();
					break;
            }
        }

        void SetData(uint32 type, uint32 data)
        {
            switch(type)
            {
                case TYPE_ZEREKETH:
                    Encounter[0] = data;
                    break;
                case DATA_WRATH_SCRYERSOCCOTHRATESEVENT:
                    if (data == DONE)
                        HandleGameObject(Containment_Core_Security_Field_Beta, true);
                    if (Encounter[1] != DONE)
                        Encounter[1] = data;
					break;
                case DATA_DALLIAHTHEDOOMSAYEREVENT:
                    if (data == DONE)
                        HandleGameObject(Containment_Core_Security_Field_Alpha, true);
                    if (Encounter[2] != DONE)
                        Encounter[2] = data;
                    break;
                case TYPE_HARBINGERSKYRISS:
                    if (data == NOT_STARTED || data == FAIL)
                    {
                        Encounter[4] = NOT_STARTED;
                        Encounter[5] = NOT_STARTED;
                        Encounter[6] = NOT_STARTED;
                        Encounter[7] = NOT_STARTED;
                        Encounter[8] = NOT_STARTED;
                    }
                    Encounter[3] = data;
                    break;
                case TYPE_WARDEN_1:
                    if (data == IN_PROGRESS)
                        if (Pod_Alpha)
                            Pod_Alpha->UseDoorOrButton();
                    Encounter[4] = data;
                    break;
                case TYPE_WARDEN_2:
                    if (data == IN_PROGRESS)
                        if (Pod_Beta)
                            Pod_Beta->UseDoorOrButton();
                    Encounter[5] = data;
                    break;
                case TYPE_WARDEN_3:
                    if (data == IN_PROGRESS)
                        if (Pod_Delta)
                            Pod_Delta->UseDoorOrButton();
                    Encounter[6] = data;
                    break;
                case TYPE_WARDEN_4:
                    if (data == IN_PROGRESS)
                        if (Pod_Gamma)
                            Pod_Gamma->UseDoorOrButton();
                    Encounter[7] = data;
                    break;
                case TYPE_WARDEN_5:
                    if (data == IN_PROGRESS)
                        if (Pod_Omega)
                            Pod_Omega->UseDoorOrButton();
                    Encounter[8] = data;
                    break;
                case TYPE_SHIELD_OPEN:
                    if (data == IN_PROGRESS)
                        if (Wardens_Shield)
                            Wardens_Shield->UseDoorOrButton();
                    break;
                case DATA_DALLIAH25:
                    d_hp25 = data;
                    break;
                case DATA_SOCCOTHRATES25:
                    s_hp25 = data;
                    break;
            }

            SaveToDB();
        }

        uint32 GetData(uint32 type)
        {
             switch(type)
            {
                case TYPE_HARBINGERSKYRISS:
                    return Encounter[3];
                case TYPE_WARDEN_1:
                    return Encounter[4];
                case TYPE_WARDEN_2:
                    return Encounter[5];
                case TYPE_WARDEN_3:
                    return Encounter[6];
                case TYPE_WARDEN_4:
                    return Encounter[7];
                case TYPE_WARDEN_5:
                    return Encounter[8];
                case DATA_DALLIAH25:
                    return d_hp25;
                break;
                case DATA_SOCCOTHRATES25:
                    return s_hp25;
					break;
                case DATA_WRATH_SCRYERSOCCOTHRATESEVENT:
                    return Encounter[1];
                break;
                case DATA_DALLIAHTHEDOOMSAYEREVENT:
                    return Encounter[2];
					break;
            }
            return 0;
        }

		uint64 GetData64(uint32 data)
        {
            switch(data)
            {
                case DATA_MELLICHAR:
                    return MellicharGUID;
                case DATA_SPHERE_SHIELD:
                    return GoSphereGUID;
                case DATA_WRATH_SCRYERSOCCOTHRATES:
                    return SoccothraesGUID;
					break;
                case DATA_DALLIAHTHEDOOMSAYER:
                    return DalliahGUID;
					break;
				case DATA_MILLHOUSE:
					return MillhouseGUID;
					break;
            }
            return 0;
        }

        std::string GetSaveData()
        {
            std::ostringstream ss;
            ss << "A T " << socco_dead << " " << dalli_dead;
            return ss.str();;
        }

        void Load(const char* load)
        {
            if (!load) return;
            std::istringstream ss(load);
            char dataHead1;
            char dataHead2;
            uint32 data1, data2;
            ss >> dataHead1 >> dataHead2 >> data1 >> data2;
            if (dataHead1 == 'A' && dataHead2 == 'T')
            {
                socco_dead = data1;
                dalli_dead = data2;
            }
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_arcatraz_InstanceMapScript(map);
    }
};

void AddSC_instance_arcatraz()
{
    new instance_arcatraz();
}