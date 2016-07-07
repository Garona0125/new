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
#include "wailing_caverns.h"

#define MAX_ENCOUNTER 9

class instance_wailing_caverns : public InstanceMapScript
{
public:
    instance_wailing_caverns() : InstanceMapScript("instance_wailing_caverns", 43) {}

    struct instance_wailing_caverns_InstanceMapScript : public InstanceScript
    {
        instance_wailing_caverns_InstanceMapScript(Map* map) : InstanceScript(map) {}

        uint32 Encounter[MAX_ENCOUNTER];

        bool Yelled;
        uint64 NaralexGUID;

        void Initialize()
        {
            memset(&Encounter, 0, sizeof(Encounter));

            Yelled = false;
            NaralexGUID = 0;
        }

        void OnCreatureCreate(Creature* creature)
        {
            if (creature->GetEntry() == DATA_NARALEX)
                NaralexGUID = creature->GetGUID();
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
                case TYPE_LORD_COBRAHN:
					Encounter[0] = data;
					break;
                case TYPE_LORD_PYTHAS:
					Encounter[1] = data;
					break;
                case TYPE_LADY_ANACONDRA:
					Encounter[2] = data;
					break;
                case TYPE_LORD_SERPENTIS:
					Encounter[3] = data;
					break;
                case TYPE_NARALEX_EVENT:
					Encounter[4] = data;
					break;
                case TYPE_NARALEX_PART1:
					Encounter[5] = data;
					break;
                case TYPE_NARALEX_PART2:
					Encounter[6] = data;
					break;
                case TYPE_NARALEX_PART3:
					Encounter[7] = data;
					break;
                case TYPE_MUTANUS_THE_DEVOURER:
					Encounter[8] = data;
					break;
                case TYPE_NARALEX_YELLED:
					Yelled = true;
					break;
            }
            if (data == DONE)SaveToDB();
        }

        uint32 GetData(uint32 type)
        {
            switch (type)
            {
                case TYPE_LORD_COBRAHN:
					return Encounter[0];
                case TYPE_LORD_PYTHAS:
					return Encounter[1];
                case TYPE_LADY_ANACONDRA:
					return Encounter[2];
                case TYPE_LORD_SERPENTIS:
					return Encounter[3];
                case TYPE_NARALEX_EVENT:
					return Encounter[4];
                case TYPE_NARALEX_PART1:
					return Encounter[5];
                case TYPE_NARALEX_PART2:
					return Encounter[6];
                case TYPE_NARALEX_PART3:
					return Encounter[7];
                case TYPE_MUTANUS_THE_DEVOURER:
					return Encounter[8];
                case TYPE_NARALEX_YELLED:
					return Yelled;
            }
            return 0;
        }

        uint64 GetData64(uint32 data)
        {
            if (data == DATA_NARALEX)
				return NaralexGUID;
            return 0;
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << Encounter[0] << ' ' << Encounter[1] << ' ' << Encounter[2] << ' '
                << Encounter[3] << ' ' << Encounter[4] << ' ' << Encounter[5] << ' '
                << Encounter[6] << ' ' << Encounter[7] << ' ' << Encounter[8];

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

            std::istringstream loadStream(in);
            loadStream >> Encounter[0] >> Encounter[1] >> Encounter[2] >> Encounter[3]
            >> Encounter[4] >> Encounter[5] >> Encounter[6] >> Encounter[7] >> Encounter[8];

            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
				if (Encounter[i] != DONE)
					Encounter[i] = NOT_STARTED;

            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_wailing_caverns_InstanceMapScript(map);
    }
};

void AddSC_instance_wailing_caverns()
{
    new instance_wailing_caverns();
}