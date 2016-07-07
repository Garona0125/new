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
#include "QuantumCreature.h"
#include "ruby_sanctum.h"

class instance_ruby_sanctum : public InstanceMapScript
{
public:
    instance_ruby_sanctum() : InstanceMapScript("instance_ruby_sanctum", 724) {}

    struct instance_ruby_sanctum_InstanceMapScript : public InstanceScript
    {
        instance_ruby_sanctum_InstanceMapScript(Map* map) : InstanceScript(map)
		{
			Initialize();
		};

        std::string strSaveData;

        //Creatures GUID
        uint32 m_auiEncounter[MAX_ENCOUNTERS+1];

        uint32 m_auiEventTimer;
        uint32 m_auiHalionEvent;

        uint32 m_auiOrbDirection;
        uint32 m_auiOrbNState;
        uint32 m_auiOrbSState;

        uint64 HalionRealGUID;
        uint64 HalionTwilightGUID;
        uint64 HalionControllerGUID;
        uint64 SavianaRagefireGUID;
        uint64 GeneralZarithianGUID;
        uint64 BaltharusGUID;
        uint64 BaltharusCloneGUID;
		uint64 BaltharusChannelTargetGUID;
        uint64 XerestraszaGUID;
        uint64 OrbNGUID;
        uint64 OrbSGUID;
        uint64 OrbFocusGUID;
        uint64 OrbCarrierGUID;
        uint64 HalionPortal1GUID;
        uint64 HalionPortal2GUID;
        uint64 HalionPortal3GUID;
        uint64 HalionFireWallSGUID;
        uint64 HalionFireWallMGUID;
        uint64 HalionFireWallLGUID;
        uint64 FireFieldGUID;
        uint64 FlameWallsGUID;
        uint64 FlameRingGUID;

        void Initialize()
        {
            for (uint8 i = 0; i < MAX_ENCOUNTERS; ++i)
                m_auiEncounter[i] = NOT_STARTED;

            m_auiEventTimer = 1000;

            HalionRealGUID = 0;
            HalionTwilightGUID = 0;
            SavianaRagefireGUID = 0;
            GeneralZarithianGUID = 0;
            BaltharusGUID = 0;
            BaltharusCloneGUID = 0;
            HalionPortal1GUID = 0;
            HalionPortal2GUID = 0;
            HalionPortal3GUID = 0;
            XerestraszaGUID = 0;
            HalionFireWallSGUID = 0;
            HalionFireWallMGUID = 0;
            HalionFireWallLGUID = 0;
            BaltharusChannelTargetGUID = 0;
            m_auiOrbDirection = 0;
            OrbNGUID = 0;
            OrbSGUID = 0;
            OrbFocusGUID = 0;
            m_auiOrbNState = NOT_STARTED;
            m_auiOrbSState = NOT_STARTED;
        }

        bool IsEncounterInProgress() const
        {
            for(uint8 i = 1; i < MAX_ENCOUNTERS ; ++i)
                if (m_auiEncounter[i] == IN_PROGRESS)
                    return true;

            return false;
        }

        void OpenDoor(uint64 guid)
        {
            if (!guid)
                return;

            GameObject* go = instance->GetGameObject(guid);
            if (go)
                go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
        }

        void CloseDoor(uint64 guid)
        {
            if (!guid)
                return;

            GameObject* go = instance->GetGameObject(guid);
			if (go)
				go->SetGoState(GO_STATE_READY);
        }

        void OpenAllDoors()
        {
            if (m_auiEncounter[TYPE_RAGEFIRE] == DONE &&
                m_auiEncounter[TYPE_BALTHARUS] == DONE &&
                m_auiEncounter[TYPE_XERESTRASZA] == DONE)
                     OpenDoor(FlameWallsGUID);
            else CloseDoor(FlameWallsGUID);
        }

        void UpdateWorldState(bool command, uint32 value)
        {
			Map::PlayerList const &players = instance->GetPlayers();

			if (command)
			{
				for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
				{
					if (Player* player = i->getSource())
					{
						if (player->IsAlive())
						{
							player->SendUpdateWorldState(UPDATE_STATE_UI_SHOW, 0);

							if (player->HasAura(74807))
								player->SendUpdateWorldState(UPDATE_STATE_UI_COUNT_T, 100 - value);
							else
								player->SendUpdateWorldState(UPDATE_STATE_UI_COUNT_R, value);

							player->SendUpdateWorldState(UPDATE_STATE_UI_SHOW, 1);
						}
					}
				}
			}
			else
			{
				for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
				{
					if (Player* player = i->getSource())
					{
						if (player->IsAlive())
							player->SendUpdateWorldState(UPDATE_STATE_UI_SHOW, 0);
					}
				}
			}
		}

        void OnCreatureCreate(Creature* creature)
        {
            switch(creature->GetEntry())
            {
                case NPC_HALION_REAL:
					HalionRealGUID = creature->GetGUID();
					break;
	            case NPC_HALION_TWILIGHT:
					HalionTwilightGUID = creature->GetGUID();
					break;
                case NPC_HALION_CONTROLLER:
					HalionControllerGUID = creature->GetGUID();
					break;
                case NPC_RAGEFIRE:
					SavianaRagefireGUID = creature->GetGUID();
					break;
                case NPC_ZARITHIAN:
					GeneralZarithianGUID = creature->GetGUID();
					break;
                case NPC_BALTHARUS:
					BaltharusGUID = creature->GetGUID();
					break;
                case NPC_BALTHARUS_TARGET:
					BaltharusChannelTargetGUID = creature->GetGUID();
					break;
                case NPC_CLONE:
					BaltharusCloneGUID = creature->GetGUID();
					break;
                case NPC_XERESTRASZA:
					XerestraszaGUID = creature->GetGUID();
					break;
                case NPC_SHADOW_PULSAR_N:
					OrbNGUID = creature->GetGUID();
					break;
                case NPC_SHADOW_PULSAR_S:
					OrbSGUID = creature->GetGUID();
					break;
                case NPC_ORB_ROTATION_FOCUS:
					OrbFocusGUID = creature->GetGUID();
					break;
                case NPC_ORB_CARRIER:
					OrbCarrierGUID = creature->GetGUID();
					break;
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch(go->GetEntry())
            {
                case GO_HALION_PORTAL_ENTER_TWILIGHT:
					HalionPortal1GUID = go->GetGUID();
					break;
                case GO_HALION_PORTAL_ENTER_TWILIGHT_1:
					HalionPortal2GUID = go->GetGUID();
					break;
                case GO_HALION_PORTAL_EXIT_TWILIGT:
					HalionPortal3GUID = go->GetGUID();
					break;
                case GO_FLAME_WALLS:
					FlameWallsGUID = go->GetGUID();
					break;
                case GO_FLAME_RING:
					FlameRingGUID = go->GetGUID();
					break;
                case GO_FIRE_FIELD:
					FireFieldGUID = go->GetGUID();
					break;
            }

            OpenAllDoors();
        }

        void SetData(uint32 type, uint32 data)
        {
            switch(type)
            {
                case TYPE_EVENT:
					m_auiEncounter[type] = data;
					data = NOT_STARTED;
					break;
                case TYPE_RAGEFIRE:
					m_auiEncounter[type] = data;
					OpenAllDoors();
					break;
                case TYPE_BALTHARUS:
					m_auiEncounter[type] = data;
					OpenAllDoors();
					break;
                case TYPE_XERESTRASZA:
					m_auiEncounter[type] = data;
					if (data == IN_PROGRESS)
						OpenDoor(FireFieldGUID);
					else if (data == NOT_STARTED)
					{
						CloseDoor(FireFieldGUID);
						OpenAllDoors();
					}
					else if (data == DONE)
					{
						OpenAllDoors();
						if (m_auiEncounter[TYPE_ZARITHRIAN] == DONE)
						{
							m_auiEncounter[TYPE_EVENT] = 200;
							m_auiEventTimer = 30000;
						};
					}
					break;
                case TYPE_ZARITHRIAN:
					m_auiEncounter[type] = data;
					if (data == DONE)
					{
						OpenDoor(FlameWallsGUID);
						m_auiEncounter[TYPE_EVENT] = 200;
						m_auiEventTimer = 30000;
					}
					else if (data == IN_PROGRESS)
						CloseDoor(FlameWallsGUID);
					else if (data == FAIL)
						OpenDoor(FlameWallsGUID);
					break;
                case TYPE_HALION:
					m_auiEncounter[type] = data;
					if (data == IN_PROGRESS)
					{
						CloseDoor(FlameRingGUID);
					}
					else
					{
						OpenDoor(FlameRingGUID);
					}
					break;
                case TYPE_HALION_EVENT:
					m_auiHalionEvent = data;
					data = NOT_STARTED;
					break;
                case TYPE_EVENT_TIMER:
					m_auiEventTimer = data;
					data = NOT_STARTED;
					break;
                case DATA_ORB_DIRECTION:
					m_auiOrbDirection = data;
					data = NOT_STARTED;
					break;
                case DATA_ORB_N:
					m_auiOrbNState = data;
					data = NOT_STARTED;
					break;
                case DATA_ORB_S:
					m_auiOrbSState = data;
					data = NOT_STARTED;
					break;
                case TYPE_COUNTER:
					if (data == COUNTER_OFF)
					{
						UpdateWorldState(false, 0);
					}
					else 
					{
						UpdateWorldState(true, data);
					}
					data = NOT_STARTED;
					break;
            }

            if (data == DONE)
            {
                OUT_SAVE_INST_DATA;

                std::ostringstream saveStream;

                for(uint8 i = 0; i < MAX_ENCOUNTERS; ++i)
                    saveStream << m_auiEncounter[i] << " ";

                strSaveData = saveStream.str();

                SaveToDB();
                OUT_SAVE_INST_DATA_COMPLETE;
            }
        }

        std::string GetSaveData()
        {
            return strSaveData;
        }

        uint32 GetData(uint32 type)
        {
            switch(type)
            {
                case TYPE_RAGEFIRE:
					return m_auiEncounter[type];
                case TYPE_BALTHARUS:
					return m_auiEncounter[type];
                case TYPE_XERESTRASZA:
					return m_auiEncounter[type];
                case TYPE_ZARITHRIAN:
					return m_auiEncounter[type];
                case TYPE_HALION:
					return m_auiEncounter[type];
                case TYPE_EVENT:
					return m_auiEncounter[type];
                case TYPE_HALION_EVENT:
					return m_auiHalionEvent;
                case TYPE_EVENT_TIMER:
					return m_auiEventTimer;
                case TYPE_EVENT_NPC:
					switch (m_auiEncounter[TYPE_EVENT])
					{
                        case 10:
						case 20:
						case 30:
						case 40:
						case 50:
						case 60:
						case 70:
						case 80:
						case 90:
						case 100:
						case 110:
						case 200:
						case 210:
							return NPC_XERESTRASZA;
							break;
						default:
							break;
					};
					return 0;

                case DATA_ORB_DIRECTION:
					return m_auiOrbDirection;
                case DATA_ORB_N:
					return m_auiOrbNState;
                case DATA_ORB_S:
					return m_auiOrbSState;
            }
            return 0;
        }

        uint64 GetData64(uint32 data)
        {
            switch(data)
            {
                case NPC_BALTHARUS:
					return BaltharusGUID;
                case NPC_CLONE:
					return BaltharusCloneGUID;
                case NPC_ZARITHIAN:
					return GeneralZarithianGUID;
                case NPC_RAGEFIRE:
					return SavianaRagefireGUID;
                case NPC_HALION_REAL:
					return HalionRealGUID;
                case NPC_HALION_TWILIGHT:
					return HalionTwilightGUID;
                case NPC_HALION_CONTROLLER:
					return HalionControllerGUID;
                case NPC_XERESTRASZA:
					return XerestraszaGUID;
                case NPC_BALTHARUS_TARGET:
					return BaltharusChannelTargetGUID;
                case GO_FLAME_WALLS:
					return FlameWallsGUID;
                case GO_FLAME_RING:
					return FlameRingGUID;
                case GO_FIRE_FIELD:
					return FireFieldGUID;
                case GO_HALION_PORTAL_ENTER_TWILIGHT:
					return HalionPortal1GUID;
                case GO_HALION_PORTAL_ENTER_TWILIGHT_1:
					return HalionPortal2GUID;
                case GO_HALION_PORTAL_EXIT_TWILIGT:
					return HalionPortal3GUID;
                case NPC_SHADOW_PULSAR_N:
					return OrbNGUID;
                case NPC_SHADOW_PULSAR_S:
					return OrbSGUID;
                case NPC_ORB_ROTATION_FOCUS:
					return OrbFocusGUID;
                case NPC_ORB_CARRIER:
					return OrbCarrierGUID;
            }
            return 0;
        }

        void Load(const char* chrIn)
        {
            if (!chrIn)
            {
                OUT_LOAD_INST_DATA_FAIL;
                return;
            }

            OUT_LOAD_INST_DATA(chrIn);

            std::istringstream loadStream(chrIn);

            for(uint8 i = 0; i < MAX_ENCOUNTERS; ++i)
            {
                loadStream >> m_auiEncounter[i];

                if (m_auiEncounter[i] == IN_PROGRESS
                    || m_auiEncounter[i] == FAIL)
                    m_auiEncounter[i] = NOT_STARTED;
            }

            m_auiEncounter[TYPE_XERESTRASZA] = NOT_STARTED;

            OUT_LOAD_INST_DATA_COMPLETE;
            OpenAllDoors();
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_ruby_sanctum_InstanceMapScript(map);
    }
};

void AddSC_instance_ruby_sanctum()
{
    new instance_ruby_sanctum;
}