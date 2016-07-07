/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "gruuls_lair.h"

class instance_gruuls_lair : public InstanceMapScript
{
public:
    instance_gruuls_lair() : InstanceMapScript("instance_gruuls_lair", 565) { }

    struct instance_gruuls_lair_InstanceMapScript : public InstanceScript
    {
        instance_gruuls_lair_InstanceMapScript(Map* map) : InstanceScript(map) {}

        uint32 Encounter[MAX_ENCOUNTER];

        uint64 KigglerTheCrazedGUID;
        uint64 BlindeyeTheSeerGUID;
        uint64 OlmTheSummonerGUID;
        uint64 KroshFirehandGUID;
        uint64 MaulgarGUID;
        uint64 MaulgarDoorGUID;
        uint64 GruulDoorGUID;

        void Initialize()
        {
            memset(&Encounter, 0, sizeof(Encounter));

            KigglerTheCrazedGUID = 0;
            BlindeyeTheSeerGUID = 0;
            OlmTheSummonerGUID = 0;
            KroshFirehandGUID = 0;
            MaulgarGUID = 0;
            MaulgarDoorGUID = 0;
            GruulDoorGUID = 0;
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
                case NPC_KIGGLER_THE_CRAZED:
					KigglerTheCrazedGUID = creature->GetGUID();
					break;
                case NPC_BLIND_EYE_THE_SEER:
					BlindeyeTheSeerGUID = creature->GetGUID();
					break;
                case NPC_OLM_THE_SUMMONER:
					OlmTheSummonerGUID = creature->GetGUID();
					break;
                case NPC_KROSH_FIRE_HAND:
					KroshFirehandGUID = creature->GetGUID();
					break;
                case NPC_KING_MAULGAR:
					MaulgarGUID = creature->GetGUID();
					break;
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case GO_MAULGAR_DOOR:
                    MaulgarDoorGUID = go->GetGUID();
                    if (Encounter[0] == DONE)
                        HandleGameObject(0, true, go);
                    break;
                case GO_GRUUL_DOOR:
                    GruulDoorGUID = go->GetGUID();
                    break;
            }
        }

        uint64 GetData64(uint32 identifier)
        {
            switch (identifier)
            {
                case DATA_KIGGLER_THE_CRAZED:
					return KigglerTheCrazedGUID;
                case DATA_BLIND_EYE_THE_SEER:
					return BlindeyeTheSeerGUID;
                case DATA_OLM_THE_SUMMONER:
					return OlmTheSummonerGUID;
                case DATA_KROSH_FIRE_HAND:
					return KroshFirehandGUID;
                case DATA_MAULGAR_DOOR:
					return MaulgarDoorGUID;
                case DATA_GRUUL_DOOR:
					return GruulDoorGUID;
                case DATA_MAULGAR:
					return MaulgarGUID;
            }
            return 0;
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
                case DATA_MAULGAR_EVENT:
                    if (data == DONE)
						HandleGameObject(MaulgarDoorGUID, true);
                    Encounter[0] = data;
					break;
                case DATA_GRUUL_EVENT:
                    if (data == IN_PROGRESS)
						HandleGameObject(GruulDoorGUID, false);
                    else
						HandleGameObject(GruulDoorGUID, true);
                    Encounter[1] = data;
					break;
            }

            if (data == DONE)
                SaveToDB();
        }

        uint32 GetData(uint32 type)
        {
            switch (type)
            {
                case DATA_MAULGAR_EVENT:
					return Encounter[0];
                case DATA_GRUUL_EVENT:
					return Encounter[1];
            }
            return 0;
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;
            std::ostringstream stream;
            stream << Encounter[0] << ' ' << Encounter[1];

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
            stream >> Encounter[0] >> Encounter[1];
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                if (Encounter[i] == IN_PROGRESS)
                    Encounter[i] = NOT_STARTED;
            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_gruuls_lair_InstanceMapScript(map);
    }
};

void AddSC_instance_gruuls_lair()
{
    new instance_gruuls_lair();
}