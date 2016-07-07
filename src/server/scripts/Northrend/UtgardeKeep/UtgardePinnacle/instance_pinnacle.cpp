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
#include "utgarde_pinnacle.h"

class instance_utgarde_pinnacle : public InstanceMapScript
{
public:
    instance_utgarde_pinnacle() : InstanceMapScript("instance_utgarde_pinnacle", 575) { }

    struct instance_pinnacle : public InstanceScript
    {
        instance_pinnacle(Map* map) : InstanceScript(map) {}

        uint64 SvalaSorrowGraveGUID;
        uint64 GortokPalehoofGUID;
        uint64 SkadiTheRuthlessGUID;
        uint64 KingYmironGUID;
		uint64 FrenziedWorgenGUID;
        uint64 RavenousFurbolgGUID;
        uint64 FerociousRhinoGUID;
        uint64 MassiveJormungarGUID;
        uint64 PalehoofOrbGUID;
        uint64 SvalaGUID;

        uint64 SkadiTheRuthlessDoorGUID;
        uint64 KingYmironDoorGUID;
        uint64 GortokPalehoofSphereGUID;

        uint64 SacrificedPlayer;

        uint32 Encounter[MAX_ENCOUNTER];

        std::string str_data;

        void Initialize()
        {
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
               Encounter[i] = NOT_STARTED;

            SvalaSorrowGraveGUID = 0;
            GortokPalehoofGUID = 0;
            SkadiTheRuthlessGUID = 0;
            KingYmironGUID = 0;
			FrenziedWorgenGUID = 0;
            RavenousFurbolgGUID = 0;
            FerociousRhinoGUID = 0;
            MassiveJormungarGUID = 0;
            PalehoofOrbGUID = 0;
            SvalaGUID = 0;

            SkadiTheRuthlessDoorGUID = 0;
            KingYmironDoorGUID = 0;
            GortokPalehoofSphereGUID = 0;

            SacrificedPlayer = 0;
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
                case NPC_SVALA_SORROWGRAVE:
					SvalaSorrowGraveGUID = creature->GetGUID();
					break;
                case NPC_GORTOK_PALEHOOF:
					GortokPalehoofGUID = creature->GetGUID();
					break;
                case NPC_SKADI_RUTHLESS:
					SkadiTheRuthlessGUID = creature->GetGUID();
					break;
                case NPC_KING_YMIRON:
                    KingYmironGUID = creature->GetGUID();
                    creature->SetReactState(REACT_PASSIVE);
                    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                    break;
                case NPC_FRENZIED_WORGEN:
					FrenziedWorgenGUID = creature->GetGUID();
					break;
                case NPC_RAVENOUS_FURBOLG:
					RavenousFurbolgGUID = creature->GetGUID();
					break;
                case NPC_MASSIVE_JORMUNGAR:
					MassiveJormungarGUID = creature->GetGUID();
					break;
                case NPC_FEROCIOUS_RHINO:
					FerociousRhinoGUID = creature->GetGUID();
					break;
                case NPC_SVALA:
					SvalaGUID = creature->GetGUID();
					break;
                case NPC_PALEHOOF_ORB:
					PalehoofOrbGUID = creature->GetGUID();
					break;
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case GO_SKADI_THE_RUTHLESS_DOOR:
                    SkadiTheRuthlessDoorGUID = go->GetGUID();
                    if (Encounter[2] == DONE) HandleGameObject(0, true, go);
                    break;
                case GO_KING_YMIRON_DOOR:
                    KingYmironDoorGUID = go->GetGUID();
                    if (Encounter[3] == DONE) HandleGameObject(0, true, go);
                    break;
                case GO_GORK_PALEHOOF_SPHERE:
                    GortokPalehoofSphereGUID = go->GetGUID();
                    if (Encounter[1] == DONE)
                    {
                        HandleGameObject(0, true, go);
                        go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    }
                    break;
            }
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
                case DATA_SVALA_SORROWGRAVE_EVENT:
                    Encounter[0] = data;
                    break;
                case DATA_GORTOK_PALEHOOF_EVENT:
                    Encounter[1] = data;
                    break;
                case DATA_SKADI_THE_RUTHLESS_EVENT:
                    if (data == DONE)
                        HandleGameObject(SkadiTheRuthlessDoorGUID, true);
                    Encounter[2] = data;
                    break;
                case DATA_KING_YMIRON_EVENT:
                    if (data == DONE)
                        HandleGameObject(KingYmironDoorGUID, true);
                    Encounter[3] = data;
                    break;
            }

            if (data == DONE)
                SaveToDB();
        }

        void SetData64(uint32 type, uint64 data)
        {
			switch (type)
			{
			    case DATA_SACRIFICED_PLAYER:
					SacrificedPlayer = data;
					break;
			}
        }

        uint32 GetData(uint32 type)
        {
            switch (type)
            {
                case DATA_SVALA_SORROWGRAVE_EVENT:
					return Encounter[0];
                case DATA_GORTOK_PALEHOOF_EVENT:
					return Encounter[1];
				case DATA_SKADI_THE_RUTHLESS_EVENT:
					return Encounter[2];
                case DATA_KING_YMIRON_EVENT:
					return Encounter[3];
            }
            return 0;
        }

        uint64 GetData64(uint32 identifier)
        {
            switch (identifier)
            {
                case DATA_SVALA_SORROWGRAVE:
					return SvalaSorrowGraveGUID;
                case DATA_GORTOK_PALEHOOF:
					return GortokPalehoofGUID;
                case DATA_SKADI_THE_RUTHLESS:
					return SkadiTheRuthlessGUID;
                case DATA_KING_YMIRON:
					return KingYmironGUID;
                case DATA_FRENZIED_WORGEN:
					return FrenziedWorgenGUID;
                case DATA_RAVENOUS_FURBOLG:
					return RavenousFurbolgGUID;
                case DATA_MASSIVE_JORMUNGAR:
					return MassiveJormungarGUID;
                case DATA_FEROCIOUS_RHINO:
					return FerociousRhinoGUID;
                case DATA_MOB_ORB:
					return PalehoofOrbGUID;
                case DATA_SVALA:
					return SvalaGUID;
                case DATA_GORTOK_PALEHOOF_SPHERE:
					return GortokPalehoofSphereGUID;
                case DATA_SACRIFICED_PLAYER:
					return SacrificedPlayer;
            }
            return 0;
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << "U P " << Encounter[0] << ' ' << Encounter[1] << ' ' << Encounter[2] << ' ' << Encounter[3];

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
            uint16 data0, data1, data2, data3;

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3;

            if (dataHead1 == 'U' && dataHead2 == 'P')
            {
                Encounter[0] = data0;
                Encounter[1] = data1;
                Encounter[2] = data2;
                Encounter[3] = data3;

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (Encounter[i] == IN_PROGRESS)
                        Encounter[i] = NOT_STARTED;

            }
			else OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }

        void Update(uint32 diff)
		{
            if (!instance->HavePlayers())
                return;

            if ((GetData(DATA_SVALA_SORROWGRAVE_EVENT) == DONE) && (GetData(DATA_GORTOK_PALEHOOF_EVENT) == DONE) && (GetData(DATA_SKADI_THE_RUTHLESS_EVENT) == DONE))
			{
                if (Creature* ymiron = instance->GetCreature(KingYmironGUID))
				{
                    ymiron->SetReactState(REACT_AGGRESSIVE);
                    ymiron->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                }
            }
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_pinnacle(map);
    }
};

void AddSC_instance_utgarde_pinnacle()
{
    new instance_utgarde_pinnacle();
}