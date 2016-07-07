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
#include "zulaman.h"

#define MAX_ENCOUNTER 6
#define RAND_VENDOR   2
#define CHESTS        4

enum ChestLootType
{
    NOTHING       = 0,
    ARMANI_ARMOR  = 1,
    ARMANI_WEAPON = 2,
    ARMANI_RING   = 3,
    ARMANI_MOUNT  = 4,
};

struct SHostageInfo
{
    uint32 npc, go;
    float x, y, z, o;
};

static SHostageInfo HostageInfo[] =
{
    {NPC_TANZAR, GO_TANZARS_TRUNK, -57, 1343, 40.77f, 3.2f},
    {NPC_HARKOR, GO_HARKORS_SATCHEL, 400, 1414, 74.36f, 3.3f},
    {NPC_ASHLI, GO_ASHLIS_BAG, -35, 1134, 18.71f, 1.9f},
    {NPC_KRAZ, GO_KRAZS_PACKAGE, 413, 1117,  6.32f, 3.1f},
};

class instance_zulaman : public InstanceMapScript
{
public:
    instance_zulaman() : InstanceMapScript("instance_zulaman", 568) { }

    struct instance_zulaman_InstanceMapScript : public InstanceScript
    {
        instance_zulaman_InstanceMapScript(Map* map) : InstanceScript(map) {};

		uint64 MassiveDoorGUID;
        uint64 HexLordGateGUID;
        uint64 AkilzonDoorGUID;
        uint64 ZulJinDoorGUID;
        uint64 HalazziEntranceDoorGUID;
        uint64 HalazziExitDoorGUID;

        uint32 QuestTimer;
        uint16 QuestMinute;
        uint16 ChestLooted;

        uint32 Encounter[MAX_ENCOUNTER];
        uint32 RandVendor[RAND_VENDOR];

        uint32 ChestLootTyp[CHESTS];

        uint32 SacreficeEventStarted;
        uint32 GauntletEventStatus;

        void Initialize()
        {
            memset(&Encounter, 0, sizeof(Encounter));

			MassiveDoorGUID = 0;
            HexLordGateGUID = 0;
            AkilzonDoorGUID = 0;
            HalazziEntranceDoorGUID = 0;
            HalazziExitDoorGUID = 0;
            ZulJinDoorGUID = 0;

            QuestTimer = 0;
            QuestMinute = 21;
            ChestLooted = 0;

            SacreficeEventStarted = NOT_STARTED;
            GauntletEventStatus = NOT_STARTED;

            for (uint8 i = 0; i < RAND_VENDOR; ++i)
                RandVendor[i] = NOT_STARTED;

            for (uint8 i = 0; i < 4; i++)
                ChestLootTyp[i] = NOTHING;

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
            switch(creature->GetEntry())
            {
			    case NPC_JANALAI:
				case NPC_ZULJIN:
				case NPC_HEX_LORD:
				case NPC_HALAZZI:
				case NPC_NALORAKK:
				default:
					break;
			}
		}

        void OnGameObjectCreate(GameObject* go)
        {
            switch(go->GetEntry())
            {
            case GO_HALAZZI_EXIT_DOOR:
				HalazziExitDoorGUID = go->GetGUID();
				break;
            case GO_HALAZZI_ENTER_DOOR:
				HalazziEntranceDoorGUID = go->GetGUID();
				break;
            case GO_HEX_LORD_DOOR:
				HexLordGateGUID = go->GetGUID();
				break;
            case GO_AKILZON_DOOR:
				AkilzonDoorGUID = go->GetGUID();
				break;
            case GO_ZULJIN_DOOR: 
				ZulJinDoorGUID = go->GetGUID();
				break;
            case GO_MASSIVE_DOOR:
                MassiveDoorGUID = go->GetGUID();
                if (SacreficeEventStarted == IN_PROGRESS)
                    HandleGameObject(MassiveDoorGUID, true, go);
                break;
            default:
				break;

            }
            CheckInstanceStatus();
        }

        void SummonHostage(uint8 num)
        {
            if (!QuestMinute)
                return;

            if (!SacreficeEventStarted)
                return;

            Map::PlayerList const &PlayerList = instance->GetPlayers();
            if (PlayerList.isEmpty())
                return;

            Map::PlayerList::const_iterator i = PlayerList.begin();
            if (Player* player = i->getSource())
            {
                if (Unit* Hostage = player->SummonCreature(HostageInfo[num].npc, HostageInfo[num].x, HostageInfo[num].y, HostageInfo[num].z, HostageInfo[num].o, TEMPSUMMON_DEAD_DESPAWN, 0))
                {
                    Hostage->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    Hostage->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                }
            }
        }

        void CheckInstanceStatus()
        {
            if (SacreficeEventStarted == 1)
                HandleGameObject(MassiveDoorGUID, true);

            if (PriestsDead())
                HandleGameObject(HexLordGateGUID, true);
        }

        std::string GetSaveData()
        {
			OUT_SAVE_INST_DATA;

            std::ostringstream ss;
            ss << "Z A " 
                << Encounter[0] << " " 
                << Encounter[1] << " " 
                << Encounter[2] << " " 
                << Encounter[3] << " " 
                << Encounter[4] << " " 
                << Encounter[5] << " " 
                << ChestLooted << " " 
                << QuestMinute << " " 
                << SacreficeEventStarted << " " 
                << ChestLootTyp[0] << " " 
                << ChestLootTyp[1] << " " 
                << ChestLootTyp[2] << " " 
                << ChestLootTyp[3];

			OUT_SAVE_INST_DATA_COMPLETE;
			return ss.str();
        }

        void Load(const char* load)
        {
            if (!load) return;
            std::istringstream ss(load);
            char dataHead1, dataHead2; // Z A
            uint16 datab1, datab2, datab3, datab4, datab5, datab6, data2, data3, data4, data5, data6, data7, data8;
            ss >> dataHead1 >> dataHead2 >> datab1 >> datab2 >> datab3 >> datab4 >> datab5 >> datab6 >> data2 >> data3 >> data4 >> data5 >> data6 >> data7 >> data8;
            if (dataHead1 == 'Z' && dataHead2 == 'A')
            {
                Encounter[0] = datab1;
                Encounter[1] = datab2;
                Encounter[2] = datab3;
                Encounter[3] = datab4;
                Encounter[4] = datab5;
                Encounter[5] = datab6;
                //BossKilled = data1;
                ChestLooted = data2;
                QuestMinute = data3;
                SacreficeEventStarted = data4;

                ChestLootTyp[0] = data5;
                ChestLootTyp[1] = data6;
                ChestLootTyp[2] = data7;
                ChestLootTyp[3] = data8;
            }
			else sLog->OutErrorConsole("QUANTUMCORE SCRIPTS: Zul'aman: corrupted save data.");
        }

        bool PriestsDead()
        {
            return Encounter[0] == DONE && Encounter[1] == DONE && Encounter[2] == DONE && Encounter[3] == DONE;
        }

        void SetData(uint32 type, uint32 data)
        {
            switch(type)
            {
            case DATA_NALORAKK_EVENT:
                if (Encounter[0] != DONE)
                    Encounter[0] = data;
                if (data == DONE)
                {
                    if (QuestMinute)
                    {
                        QuestMinute += 15;
                        DoUpdateWorldState(3106, QuestMinute);
                    }
                    SummonHostage(0);
                }
                break;
            case DATA_AKILZON_EVENT:
                if (Encounter[1] != DONE)
                    Encounter[1] = data;
                HandleGameObject(AkilzonDoorGUID, data != IN_PROGRESS);
                if (data == DONE)
                {
                    if (QuestMinute)
                    {
                        QuestMinute += 10;
                        DoUpdateWorldState(3106, QuestMinute);
                    }
                    SummonHostage(1);
                }
                break;
            case DATA_JANALAI_EVENT:
                if (Encounter[2] != DONE)
                    Encounter[2] = data;
                if (data == DONE)
					SummonHostage(2);
                break;
            case DATA_HALAZZI_EVENT:
                if (Encounter[3] != DONE)
                    Encounter[3] = data;
                switch(data)
                {
                case IN_PROGRESS:
                    HandleGameObject(HalazziEntranceDoorGUID, false);
                    break;
                case NOT_STARTED: 
                    HandleGameObject(HalazziEntranceDoorGUID, true);
                    HandleGameObject(HalazziExitDoorGUID, false);
                    break;
                case DONE:
                    HandleGameObject(HalazziEntranceDoorGUID, true);
                    HandleGameObject(HalazziExitDoorGUID, true);
                    SummonHostage(3);
                    break;
                }
                break;
            case DATA_HEXLORD_EVENT:
                if (Encounter[4] != DONE)
                    Encounter[4] = data;
                if (data == IN_PROGRESS)
                    HandleGameObject(HexLordGateGUID, false);
                else if (data == NOT_STARTED)
                    CheckInstanceStatus();
                break;
            case DATA_ZULJIN_EVENT:
                Encounter[5] = data;
                HandleGameObject(ZulJinDoorGUID, data != IN_PROGRESS);
                break;
            case DATA_CHEST_LOOTED:
                ++ChestLooted;
                switch(data)
                {
                    case NPC_TANZAR:
						ChestLootTyp[0] = ChestLooted;
						break;
					case NPC_HARKOR:
						ChestLootTyp[1] = ChestLooted;
						break;
					case NPC_KRAZ:
						ChestLootTyp[2] = ChestLooted;
						break;
					case NPC_ASHLI:
						ChestLootTyp[3] = ChestLooted;
						break;
                }
                SaveToDB();
                break;
            case TYPE_RAND_VENDOR_1:
                RandVendor[0] = data;
                break;
            case TYPE_RAND_VENDOR_2:
                RandVendor[1] = data;
                break;
            case DATA_TIMER_START:
                if (QuestMinute > 0)
                {
                    SacreficeEventStarted = 1;
                    SaveToDB();
                    DoUpdateWorldState(3104, 1);
                    DoUpdateWorldState(3106, QuestMinute);
                    CheckInstanceStatus();
                }
                break;
            case DATA_GAUNTLET:
                GauntletEventStatus = data;
                break;
            case DATA_TIMER_DELETE:
                QuestMinute = 0;
                SacreficeEventStarted = 1;
                DoUpdateWorldState(3104, 0);
                SaveToDB();
                CheckInstanceStatus();
                break;
            }

            if (data == DONE)
            {
                if (QuestMinute && PriestsDead())
                {
                    QuestMinute = 0;
                    DoUpdateWorldState(3104, 0);
                }
                CheckInstanceStatus();
                SaveToDB();
            }
        }

        uint32 GetData(uint32 type)
        {
            switch(type)
            {
            case DATA_NALORAKK_EVENT:
				return Encounter[0];
            case DATA_AKILZON_EVENT:
				return Encounter[1];
            case DATA_JANALAI_EVENT:
				return Encounter[2];
            case DATA_HALAZZI_EVENT:
				return Encounter[3];
            case DATA_HEXLORD_EVENT:
				return Encounter[4];
            case DATA_ZULJIN_EVENT:
				return Encounter[5];
            case DATA_CHEST_LOOTED:
				return ChestLooted;
            case TYPE_RAND_VENDOR_1:
				return RandVendor[0];
            case TYPE_RAND_VENDOR_2:
				return RandVendor[1];
            case DATA_GAUNTLET:
				return GauntletEventStatus;
            case DATA_TIMER_START:
				return SacreficeEventStarted;
            case DATA_CHEST_NAL_LOOT:
				return ChestLootTyp[0];
            case DATA_CHEST_AKI_LOOT:
				return ChestLootTyp[1];
            case DATA_CHEST_JAN_LOOT:
				return ChestLootTyp[2];
            case DATA_CHEST_HAL_LOOT:
				return ChestLootTyp[3];
            default:                  
				return 0;
            }
        }

        void Update(uint32 diff)
        {
            if (SacreficeEventStarted == 1)
            {
                if (QuestMinute)
                {
                    if (QuestTimer <= diff)
                    {
                        QuestMinute--;

                        if (QuestMinute)
                        {
                            DoUpdateWorldState(3104, 1);
                            DoUpdateWorldState(3106, QuestMinute);
                        }
						else
                        {
                            DoUpdateWorldState(3104, 0);
                        }
                        SaveToDB();
                        QuestTimer += 60000;
                    }
                    QuestTimer -= diff;
                }
            }
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_zulaman_InstanceMapScript(map);
    }
};

void AddSC_instance_zulaman()
{
    new instance_zulaman();
}