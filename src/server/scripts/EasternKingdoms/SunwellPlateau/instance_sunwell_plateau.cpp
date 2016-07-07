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
#include "sunwell_plateau.h"

class instance_sunwell_plateau : public InstanceMapScript
{
public:
    instance_sunwell_plateau() : InstanceMapScript("instance_sunwell_plateau", 580) { }

    struct instance_sunwell_plateau_InstanceMapScript : public InstanceScript
    {
        instance_sunwell_plateau_InstanceMapScript(Map* map) : InstanceScript(map) {}

        uint32 Encounter[MAX_ENCOUNTER];

        uint64 KalecgosDragonGUID;
        uint64 KalecgosHumanGUID;
        uint64 SathrovarrTheCorruptorGUID;
        uint64 BrutallusGUID;
        uint64 MadrigosaGUID;
        uint64 FelmystGUID;
        uint64 GrandWarlockAlythessGUID;
        uint64 LadySacrolashGUID;
        uint64 MuruGUID;
        uint64 KilJaedenGUID;
        uint64 KilJaedenControllerGUID;
        uint64 AnveenaGUID;
        uint64 KalecgosKJGUID;
        uint32 SpectralPlayersGUID;
		uint64 QuelDelarIntroGUID;

        uint64 ForceFieldGUID;
		uint64 OrbOfBlueFlight1GUID;
		uint64 OrbOfBlueFlight2GUID;
		uint64 OrbOfBlueFlight3GUID;
		uint64 OrbOfBlueFlight4GUID;
        uint64 Gate[5];
        uint64 ForceField_Collision[2];
        uint64 KalecgosWall[2];
        uint64 FireBarrierGUID;
        uint64 MurusGate[2];

        uint32 SpectralRealmTimer;
        std::vector<uint64> SpectralRealmList;
        uint32 RepairBotState;  

        void Initialize()
        {
            memset(&Encounter, 0, sizeof(Encounter));

            KalecgosDragonGUID           = 0;
            KalecgosHumanGUID            = 0;
            SathrovarrTheCorruptorGUID   = 0;
            BrutallusGUID                = 0;
            MadrigosaGUID                = 0;
            FelmystGUID                  = 0;
            GrandWarlockAlythessGUID     = 0;
            LadySacrolashGUID            = 0;
            MuruGUID                     = 0;
            KilJaedenGUID                = 0;
            KilJaedenControllerGUID      = 0;
            AnveenaGUID                  = 0;
            KalecgosKJGUID               = 0;
            SpectralPlayersGUID          = 0;
			QuelDelarIntroGUID           = 0;

            ForceFieldGUID               = 0;
			OrbOfBlueFlight1GUID         = 0;
			OrbOfBlueFlight2GUID         = 0;
			OrbOfBlueFlight3GUID         = 0;
			OrbOfBlueFlight4GUID         = 0;
            ForceField_Collision[0]      = 0;
            ForceField_Collision[1]      = 0;
            FireBarrierGUID              = 0;
            MurusGate[0]                 = 0;
            MurusGate[1]                 = 0;
            KalecgosWall[0]              = 0;
            KalecgosWall[1]              = 0;
            Gate[0]                      = 0;
            Gate[1]                      = 0;
            Gate[2]                      = 0;
            Gate[3]                      = 0;
            Gate[4]                      = 0;

            SpectralRealmTimer = 5000;
        }

        bool IsEncounterInProgress() const
        {
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                if (Encounter[i] == IN_PROGRESS)
                    return true;

            return false;
        }

        Player* GetPlayerInMap()
        {
            Map::PlayerList const& players = instance->GetPlayers();

            if (!players.isEmpty())
            {
                for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                {
                    Player* player = itr->getSource();
                    if (player && !player->HasAura(45839, 0))
						return player;
                }
            }
            return NULL;
        }

        void OnCreatureCreate(Creature* creature)
        {
            switch (creature->GetEntry())
            {
            case NPC_KALECGOS_DRAGON:
				KalecgosDragonGUID = creature->GetGUID();
				break;
            case NPC_KALECGOS_HUMAN:
				KalecgosHumanGUID = creature->GetGUID();
				break;
            case NPC_SATHROVARR_THE_CORRUPTOR:
				SathrovarrTheCorruptorGUID = creature->GetGUID();
				break;
            case NPC_BRUTALLUS:
				BrutallusGUID = creature->GetGUID();
				break;
            case NPC_MADRIGOSA:
				MadrigosaGUID = creature->GetGUID();
				break;
            case NPC_FELMYST:
				FelmystGUID = creature->GetGUID();
				break;
            case NPC_GRAND_WARLOCK_ALYTHESS:
				GrandWarlockAlythessGUID = creature->GetGUID();
				break;
            case NPC_LADY_SACROLASH:
				LadySacrolashGUID = creature->GetGUID();
				break;
            case NPC_MURU:
				MuruGUID = creature->GetGUID();
				break;
            case NPC_KILJAEDEN:
				KilJaedenGUID = creature->GetGUID();
				break;
            case NPC_KILJAEDEN_CONTROLLER:
				KilJaedenControllerGUID = creature->GetGUID();
				break;
            case NPC_ANVEENA:
				AnveenaGUID = creature->GetGUID();
				break;
            case NPC_KALECGOS_KILJAEDEN:
				KalecgosKJGUID = creature->GetGUID();
				break;
			case NPC_CHAMBERLAIN_GALIROS:
				QuelDelarIntroGUID = creature->GetGUID();
				break;
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch(go->GetEntry())
            {
                case GO_FORCE_FIELD_1:
					ForceFieldGUID = go->GetGUID();
					break;
				case GO_ORB_BLUE_FLIGHT_1:
					OrbOfBlueFlight1GUID = go->GetGUID();
					break;
				case GO_ORB_BLUE_FLIGHT_2:
					OrbOfBlueFlight2GUID = go->GetGUID();
					break;
				case GO_ORB_BLUE_FLIGHT_3:
					OrbOfBlueFlight3GUID = go->GetGUID();
					break;
				case GO_ORB_BLUE_FLIGHT_4:
					OrbOfBlueFlight4GUID = go->GetGUID();
					break;
                case GO_BOSS_COLLISION_1:
					KalecgosWall[0] = go->GetGUID();
					break;
                case GO_BOSS_COLLISION_2:
					KalecgosWall[0] = go->GetGUID();
					break;
                case GO_FIRE_BARRIER:
                    FireBarrierGUID = go->GetGUID();
                    if (Encounter[2] == DONE)
                        HandleGameObject(NULL, true, go);
                    else
                        HandleGameObject(FireBarrierGUID, false);
                    break;
                case GO_MURU_GATE:
					MurusGate[0] = go->GetGUID();
					break;
                case GO_GATE_1:
					Gate[1] = go->GetGUID();
					break;
                case GO_GATE_2:
					Gate[2] = go->GetGUID();
					break;
                case GO_GATE_3:
                    if (Encounter[4] == DONE)
                        HandleGameObject(NULL, true, go);
                    MurusGate[1]= go->GetGUID();
                    break;
            }
        }

        uint32 GetData(uint32 id)
        {
            switch (id)
            {
                case DATA_KALECGOS_EVENT:
					return Encounter[0];
                case DATA_BRUTALLUS_EVENT:
					return Encounter[1];
                case DATA_FELMYST_EVENT:
					return Encounter[2];
                case DATA_EREDAR_TWINS_EVENT:
					return Encounter[3];
                case DATA_MURU_EVENT:
					return Encounter[4];
                case DATA_KILJAEDEN_EVENT:
					return Encounter[5];
					break;
            }
            return 0;
        }

        uint64 GetData64(uint32 id)
        {
            switch (id)
            {
                case DATA_KALECGOS_DRAGON:
					return KalecgosDragonGUID;
                case DATA_KALECGOS_HUMAN:
					return KalecgosHumanGUID;
                case DATA_SATHROVARR_THE_CORRUPTOR:
					return SathrovarrTheCorruptorGUID;
                case DATA_GO_FORCEFIELD:
					return ForceFieldGUID;
				case DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_1:
					return OrbOfBlueFlight1GUID;
				case DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_2:
					return OrbOfBlueFlight2GUID;
				case DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_3:
					return OrbOfBlueFlight3GUID;
				case DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_4:
					return OrbOfBlueFlight4GUID;
                case DATA_BRUTALLUS:
					return BrutallusGUID;
                case DATA_MADRIGOSA:
					return MadrigosaGUID;
                case DATA_FELMYST:
					return FelmystGUID;
                case DATA_ALYTHESS:
					return GrandWarlockAlythessGUID;
                case DATA_SACROLASH:
					return LadySacrolashGUID;
                case DATA_MURU:
					return MuruGUID;
                case DATA_KILJAEDEN:
					return KilJaedenGUID;
                case DATA_KILJAEDEN_CONTROLLER:
					return KilJaedenControllerGUID;
                case DATA_ANVEENA:
					return AnveenaGUID;
                case DATA_KALECGOS_KJ:
					return KalecgosKJGUID;
				case DATA_QUELDELAR_INTRODUCER:
					return QuelDelarIntroGUID;
                case DATA_PLAYER_GUID:
                    Player* target = GetPlayerInMap();
                    return target->GetGUID();
            }

            switch(id)
            {
                case DATA_GO_FORECEFIELD_COLL_1:
					return ForceField_Collision[0];
					break;
                case DATA_GO_FORECEFIELD_COLL_2:
					return ForceField_Collision[1]; 
					break;
            }
            return 0;
        }

        void SetData(uint32 id, uint32 data)
        {
            switch (id)
            {
                case DATA_KALECGOS_EVENT:
					if (data == IN_PROGRESS)
						HandleGameObject(ForceFieldGUID, false);
					else 
						HandleGameObject(ForceFieldGUID, true);
					if (Encounter[0] != DONE)
						Encounter[0] = data;
					break;
                case DATA_BRUTALLUS_EVENT:
					if (Encounter[1] != DONE)
						Encounter[1] = data;
					break;
                case DATA_FELMYST_EVENT:
					if (data == DONE)
                        HandleGameObject(FireBarrierGUID, true);
                    if (Encounter[2] != DONE)
                        Encounter[2] = data;
					break;
                case DATA_EREDAR_TWINS_EVENT:
                    if (Encounter[3] != DONE)
						Encounter[3] = data;
					break;
                case DATA_MURU_EVENT:
                    switch (data)
                    {
                        case DONE:
                            HandleGameObject(MurusGate[0], true);
                            HandleGameObject(MurusGate[1], true);
                            break;
                        case IN_PROGRESS:
                            HandleGameObject(MurusGate[0], false);
                            HandleGameObject(MurusGate[1], false);
                            break;
                        case NOT_STARTED:
                            HandleGameObject(MurusGate[0], true);
                            HandleGameObject(MurusGate[1], false);
                            break;
                    }
                    Encounter[4] = data;
					break;
                case DATA_KILJAEDEN_EVENT:
                    if (Encounter[5] != DONE)
                        Encounter[5] = data;
                    break;
            }

            if (data == DONE)
                SaveToDB();
        }

        void Update(uint32 diff) {}

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;
            std::ostringstream stream;
			stream << Encounter[0] << ' '  << Encounter[1] << ' '  << Encounter[2] << ' '  << Encounter[3] << ' '
				<< Encounter[4] << ' '  << Encounter[5];
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

            stream >> Encounter[0] >> Encounter[1] >> Encounter[2] >> Encounter[3]
                >> Encounter[4] >> Encounter[5];

            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                if (Encounter[i] == IN_PROGRESS)
                    Encounter[i] = NOT_STARTED;

            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_sunwell_plateau_InstanceMapScript(map);
    }
};

void AddSC_instance_sunwell_plateau()
{
    new instance_sunwell_plateau();
}