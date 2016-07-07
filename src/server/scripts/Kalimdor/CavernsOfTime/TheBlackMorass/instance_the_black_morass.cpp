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
#include "the_black_morass.h"
 
class instance_the_black_morass : public InstanceMapScript
{
public:
    instance_the_black_morass() : InstanceMapScript("instance_the_black_morass", 269) { }

    struct instance_the_black_morass_InstanceMapScript : public InstanceScript
    {
        instance_the_black_morass_InstanceMapScript(Map* map) : InstanceScript(map) {};
 
        uint64 Medivh;
        uint32 IsMedivhDead;
        uint32 IsChronoLordDejaDead;
        uint32 IsTemporusDead;
        uint32 IsAeonusDead;
        uint32 Wave;
        uint32 ShieldPercent;
 
        void Initialize()
        {
            Medivh = 0;
            IsMedivhDead = 0;
            IsChronoLordDejaDead = 0;
            IsTemporusDead = 0;
            IsAeonusDead = 0;
            Wave = 0;
            ShieldPercent = 100;
        }

        bool IsEncounterInProgress()
        {
            return ((Wave > 0) && (!IsAeonusDead));
        }

        void OnCreatureCreate(Creature* creature)
        {
            switch(creature->GetEntry())
            {
                case NPC_MEDIVH:
                    Medivh = creature->GetGUID();
                    break;
            }
        }
 
        uint64 GetData64(uint32 identifier)
        {
            if (identifier == DATA_MEDIVH)
                return Medivh;
            return 0;
        }
 
        uint32 GetData(uint32 type)
        { 
            if (type == DATA_MEDIVH_DEATH)
                return IsMedivhDead;
            if (type == DATA_CHRONO_LORD_DEJA_DEATH)
                return IsChronoLordDejaDead;
            if (type == DATA_TEMPORUS_DEATH)
                return IsTemporusDead;
            if (type == DATA_AEONUS_DEATH)
                return IsAeonusDead;
            if (type == DATA_WAVE)
                return Wave;
            if (type == DATA_SHIELD_PERCENT)
                return ShieldPercent;
            return 0;
        }
 
        void SetData(uint32 type, uint32 data)
        {
            if (type == DATA_MEDIVH_DEATH)
                IsMedivhDead = data;
            else if (type == DATA_CHRONO_LORD_DEJA_DEATH)
                IsChronoLordDejaDead = data;
            else if (type == DATA_TEMPORUS_DEATH)
                IsTemporusDead = data;
            else if (type == DATA_AEONUS_DEATH)
                IsAeonusDead = data;
            else if (type == DATA_WAVE)
                Wave = data;
            else if (type == DATA_SHIELD_PERCENT)
                ShieldPercent = data;
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;
            std::ostringstream stream;
            stream << IsChronoLordDejaDead << " " << IsTemporusDead << " " << IsAeonusDead;
            std::string data = stream.str();
            return data.c_str();
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
            stream >> IsChronoLordDejaDead >> IsTemporusDead >> IsAeonusDead;
            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_the_black_morass_InstanceMapScript(map);
    }
};

void AddSC_instance_the_black_morass()
{
    new instance_the_black_morass();
}