/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
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
#include "drak_tharon_keep.h"

class instance_drak_tharon_keep : public InstanceMapScript
{
public:
    instance_drak_tharon_keep() : InstanceMapScript("instance_drak_tharon_keep", 600) { }

    struct instance_drak_tharon_keep_InstanceScript : public InstanceScript
    {
        instance_drak_tharon_keep_InstanceScript(Map* map) : InstanceScript(map) {}

        uint8 DredAchievCounter;

        uint64 Trollgore;
        uint64 Novos;
        uint64 Dred;
        uint64 TharonJa;

        uint64 NovosCrystal1;
        uint64 NovosCrystal2;
        uint64 NovosCrystal3;
        uint64 NovosCrystal4;

        uint16 Encounter[MAX_ENCOUNTER];

        std::string str_data;

        void Initialize()
        {
            Trollgore = 0;
            Novos = 0;
            Dred = 0;
            TharonJa = 0;
            NovosCrystal1 = 0;
            NovosCrystal2 = 0;
            NovosCrystal3 = 0;
            NovosCrystal4 = 0;
            DredAchievCounter = 0;
        }

        bool IsEncounterInProgress() const
        {
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                if (Encounter[i] == IN_PROGRESS) return true;

            return false;
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case GO_NOVOS_CRYSTAL_1:
                    NovosCrystal1 = go->GetGUID();
                    break;
                case GO_NOVOS_CRYSTAL_2:
                    NovosCrystal2 = go->GetGUID();
                    break;
                case GO_NOVOS_CRYSTAL_3:
                    NovosCrystal3 = go->GetGUID();
                    break;
                case GO_NOVOS_CRYSTAL_4:
                    NovosCrystal4 = go->GetGUID();
                    break;
            }
        }

        void OnCreatureCreate(Creature* creature)
        {
            switch (creature->GetEntry())
            {
                case NPC_TROLLGORE:
                    Trollgore = creature->GetGUID();
                    break;
                case NPC_NOVOS:
                    Novos = creature->GetGUID();
                    break;
                case NPC_KING_DRED:
                    Dred = creature->GetGUID();
                    break;
                case NPC_THARON_JA:
                    TharonJa = creature->GetGUID();
                    break;
            }
        }

        uint64 GetData64(uint32 identifier)
        {
            switch (identifier)
            {
                case DATA_TROLLGORE:
					return Trollgore;
                case DATA_NOVOS:
					return Novos;
                case DATA_DRED:
					return Dred;
                case DATA_THARON_JA:
					return TharonJa;
                case DATA_NOVOS_CRYSTAL_1:
					return NovosCrystal1;
                case DATA_NOVOS_CRYSTAL_2:
					return NovosCrystal2;
                case DATA_NOVOS_CRYSTAL_3:
					return NovosCrystal3;
                case DATA_NOVOS_CRYSTAL_4:
					return NovosCrystal4;
            }
            return 0;
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
                case DATA_TROLLGORE_EVENT:
                    Encounter[0] = data;
                    break;
                case DATA_NOVOS_EVENT:
                    Encounter[1] = data;
                    break;
                case DATA_DRED_EVENT:
                    Encounter[2] = data;
                    break;
                case DATA_THARON_JA_EVENT:
                    Encounter[3] = data;
                    break;
                case DATA_KING_DRED_ACHIEV:
                    DredAchievCounter = data;
                    break;
            }

            if (data == DONE)
            {
                SaveToDB();
            }
        }

        uint32 GetData(uint32 type)
        {
            switch (type)
            {
                case DATA_TROLLGORE_EVENT:
					return Encounter[0];
                case DATA_NOVOS_EVENT:
					return Encounter[1];
                case DATA_DRED_EVENT:
					return Encounter[2];
                case DATA_THARON_JA_EVENT:
					return Encounter[3];
                case DATA_KING_DRED_ACHIEV:
					return DredAchievCounter;
            }
            return 0;
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << "D K " << Encounter[0] << ' ' << Encounter[1] << ' '
                << Encounter[2] << ' ' << Encounter[3];

            OUT_SAVE_INST_DATA_COMPLETE;
            return saveStream.str();
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

            if (dataHead1 == 'D' && dataHead2 == 'K')
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
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_drak_tharon_keep_InstanceScript(map);
    }
};

void AddSC_instance_drak_tharon_keep()
{
    new instance_drak_tharon_keep;
}