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
#include "temple_of_ahnqiraj.h"

class instance_temple_of_ahnqiraj : public InstanceMapScript
{
public:
    instance_temple_of_ahnqiraj() : InstanceMapScript("instance_temple_of_ahnqiraj", 531) { }

    struct instance_temple_of_ahnqiraj_InstanceMapScript : public InstanceScript
    {
        instance_temple_of_ahnqiraj_InstanceMapScript(Map* map) : InstanceScript(map) {}

        uint64 ProphetSkeramGUID;
        uint64 LordKriGUID;
		uint64 PrincessYaujGUID;
		uint64 VemGUID;
		uint64 BattleguardSaturaGUID;
		uint64 FankrissGUID;
		uint64 ViscidusGUID;
		uint64 PrincessHuhuranGUID;
        uint64 VeklorGUID;
        uint64 VeknilashGUID;
		uint64 OuroGUID;
		uint64 CtunGUID;

		uint64 SkeramDoorGUID;
		uint64 TwinsDoorGUID;
		uint64 CtunDoorGUID;

        uint32 CthunPhase;
		uint32 CheckDoorsTimer;
		uint32 WisperTimer;

        void Initialize()
        {
            ProphetSkeramGUID     = 0;
			LordKriGUID           = 0;
			PrincessYaujGUID      = 0;
            VemGUID               = 0;
			BattleguardSaturaGUID = 0;
			FankrissGUID          = 0;
			ViscidusGUID          = 0;
			PrincessHuhuranGUID   = 0;
            VeklorGUID            = 0;
            VeknilashGUID         = 0;
			OuroGUID              = 0;
			CtunGUID              = 0;

			SkeramDoorGUID        = 0;
			TwinsDoorGUID         = 0,
			CtunDoorGUID          = 0;

            CthunPhase            = 0;
        }

		bool IsEncounterInProgress() const
        {
            return false;
        }

		void OnPlayerEnter(Player* /*player*/)
		{
			CheckDoorsTimer = 1000;
			WisperTimer = 5000;
		}

        void OnCreatureCreate(Creature* creature)
        {
            switch (creature->GetEntry())
            {
                case NPC_PROPHET_SKERAM:
					ProphetSkeramGUID = creature->GetGUID();
					break;
                case NPC_LORD_KRI:
					LordKriGUID = creature->GetGUID();
					break;
				case NPC_YAUJ:
					PrincessYaujGUID = creature->GetGUID();
					break;
				case NPC_VEM:
					VemGUID = creature->GetGUID();
					break;
				case NPC_BATTLEGUARD_SATURA:
					BattleguardSaturaGUID = creature->GetGUID();
					break;
				case NPC_FANKRISS:
					FankrissGUID = creature->GetGUID();
					break;
				case NPC_VISCIDUS:
					ViscidusGUID = creature->GetGUID();
					break;
				case NPC_PRINCESS_HUHURAN:
					PrincessHuhuranGUID = creature->GetGUID();
					break;
                case NPC_VEKLOR:
					VeklorGUID = creature->GetGUID();
					break;
                case NPC_VEKNILASH:
					VeknilashGUID = creature->GetGUID();
					break;
				case NPC_OURO:
					OuroGUID = creature->GetGUID();
					break;
				case NPC_CTUN:
					CtunGUID = creature->GetGUID();
					break;
            }
        }

		void OnGameObjectCreate(GameObject* go)
		{
			switch (go->GetEntry())
			{
			    case GO_SKERAM_DOOR:
					go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
					SkeramDoorGUID = go->GetGUID();
					break;
				case GO_TWINS_DOOR:
					go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
					TwinsDoorGUID = go->GetGUID();
					break;
				case GO_DOOR_TO_CTUN:
					go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
					CtunDoorGUID = go->GetGUID();
					break;
			}
		}

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
				case DATA_PROPHET_SKERAM:
					if (data == NOT_STARTED)
					{
						HandleGameObject(SkeramDoorGUID, false);
					}
					else if (data == IN_PROGRESS)
					{
						HandleGameObject(SkeramDoorGUID, false);
					}
					else if (data == DONE)
					{
						HandleGameObject(SkeramDoorGUID, true);
					}
					break;
				case DATA_CTHUN_PHASE:
					CthunPhase = data;
					break;
            }
        }

		uint32 GetData(uint32 type)
        {
            switch (type)
            {
                case DATA_CTHUN_PHASE:
                    return CthunPhase;
            }
            return 0;
        }

		uint64 GetData64(uint32 identifier)
        {
            switch (identifier)
            {
                case DATA_PROPHET_SKERAM:
                    return ProphetSkeramGUID;
                case DATA_LORD_KRI:
                    return LordKriGUID;
				case DATA_YAUJ:
					return PrincessYaujGUID;
				case DATA_VEM:
                    return VemGUID;
				case DATA_SATURA:
					return BattleguardSaturaGUID;
				case DATA_FANKRISS:
					return FankrissGUID;
				case DATA_VISCIDUS:
					return ViscidusGUID;
				case DATA_HUHURAN:
					return PrincessHuhuranGUID;
                case DATA_VEKLOR:
					return VeklorGUID;
				case DATA_VEKNILASH:
					return VeknilashGUID;
				case DATA_OURO:
					return OuroGUID;
				case DATA_CTUN:
					return CtunGUID;
				case DATA_SKERAM_DOOR:
					return SkeramDoorGUID;
				case DATA_CTUN_DOOR:
					return CtunDoorGUID;
				case DATA_TWINS_DOOR:
					return TwinsDoorGUID;
            }
            return 0;
        }

		void Update(uint32 diff)
		{
			if (WisperTimer <= diff)
			{
				if (Creature* Ctun = instance->GetCreature(CtunGUID))
				{
					if (!Ctun->IsAlive())
						return;

					Map::PlayerList const &PlayerList= instance->GetPlayers();

					if (!PlayerList.isEmpty())
					{
						for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
						{
							if (Player* player = itr->getSource())
								player->PlayDirectSound(RANDOM_SOUND_WHISPER, player);
						}
					}
					WisperTimer = 60000;
				}
			}
			else WisperTimer -= diff;

			// Door Timer
			if (CheckDoorsTimer <= diff)
			{
				if (Creature* Skeram = instance->GetCreature(ProphetSkeramGUID))
				{
					if (Skeram->IsAlive())
						return;

					HandleGameObject(SkeramDoorGUID, true);
				}

				if (Creature* Veknilash = instance->GetCreature(VeknilashGUID))
				{
					if (Veknilash->IsAlive())
						return;

					HandleGameObject(TwinsDoorGUID, true);
					HandleGameObject(CtunDoorGUID, true);
				}

				if (Creature* Veklor = instance->GetCreature(VeklorGUID))
				{
					if (Veklor->IsAlive())
						return;

					HandleGameObject(TwinsDoorGUID, true);
					HandleGameObject(CtunDoorGUID, true);
				}

				CheckDoorsTimer = 5000;
			}
			else CheckDoorsTimer -= diff;
		}
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_temple_of_ahnqiraj_InstanceMapScript(map);
    }
};

void AddSC_instance_temple_of_ahnqiraj()
{
    new instance_temple_of_ahnqiraj();
}