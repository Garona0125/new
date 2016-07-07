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
#include "naxxramas.h"

const DoorData doorData[] =
{
    {GO_ROOM_ANUBREKHAN,    BOSS_ANUBREKHAN,  DOOR_TYPE_ROOM,    BOUNDARY_S},
    {GO_PASSAGE_ANUBREKHAN, BOSS_ANUBREKHAN,  DOOR_TYPE_PASSAGE,          0},
    {GO_PASSAGE_FAERLINA,   BOSS_FAERLINA,    DOOR_TYPE_PASSAGE,          0},
    {GO_ROOM_MAEXXNA,       BOSS_FAERLINA,    DOOR_TYPE_PASSAGE,          0},
    {GO_ROOM_MAEXXNA,       BOSS_MAEXXNA,     DOOR_TYPE_ROOM,    BOUNDARY_SW},
    {GO_ROOM_NOTH,          BOSS_NOTH,        DOOR_TYPE_ROOM,    BOUNDARY_N},
    {GO_PASSAGE_NOTH,       BOSS_NOTH,        DOOR_TYPE_PASSAGE, BOUNDARY_E},
    {GO_ROOM_HEIGAN,        BOSS_NOTH,        DOOR_TYPE_PASSAGE,          0},
    {GO_ROOM_HEIGAN,        BOSS_HEIGAN,      DOOR_TYPE_ROOM,    BOUNDARY_N},
    {GO_PASSAGE_HEIGAN,     BOSS_HEIGAN,      DOOR_TYPE_PASSAGE, BOUNDARY_E},
    {GO_ROOM_LOATHEB,       BOSS_HEIGAN,      DOOR_TYPE_PASSAGE,          0},
    {GO_ROOM_LOATHEB,       BOSS_LOATHEB,     DOOR_TYPE_ROOM,    BOUNDARY_W},
    {GO_ROOM_GROBBULUS,     BOSS_PATCHWERK,   DOOR_TYPE_PASSAGE,          0},
    {GO_ROOM_GROBBULUS,     BOSS_GROBBULUS,   DOOR_TYPE_ROOM,             0},
    {GO_PASSAGE_GLUTH,      BOSS_GLUTH,       DOOR_TYPE_PASSAGE,  BOUNDARY_NW},
    {GO_ROOM_THADDIUS,      BOSS_GLUTH,       DOOR_TYPE_PASSAGE,          0},
    {GO_ROOM_THADDIUS,      BOSS_THADDIUS,    DOOR_TYPE_ROOM,             0},
    {GO_ROOM_GOTHIK,        BOSS_RAZUVIOUS,   DOOR_TYPE_PASSAGE,          0},
    {GO_ROOM_GOTHIK,        BOSS_GOTHIK,      DOOR_TYPE_ROOM,     BOUNDARY_N},
    {GO_PASSAGE_GOTHIK,     BOSS_GOTHIK,      DOOR_TYPE_PASSAGE,  BOUNDARY_S},
    {GO_ROOM_HORSEMEN,      BOSS_GOTHIK,      DOOR_TYPE_PASSAGE,          0},
    {GO_ROOM_HORSEMEN,      BOSS_HORSEMEN,    DOOR_TYPE_ROOM,     BOUNDARY_NE},
    {GO_PASSAGE_SAPPHIRON,  BOSS_SAPPHIRON,   DOOR_TYPE_PASSAGE,  BOUNDARY_W},
    {GO_ROOM_KELTHUZAD,     BOSS_KELTHUZAD,   DOOR_TYPE_ROOM,     BOUNDARY_S},
    {0,         0,          DOOR_TYPE_ROOM,                                0}, // EOF
};

const MinionData minionData[] =
{
    //{NPC_CRYPT_GUARD,             BOSS_ANUBREKHAN},     there is no spawn point in db, so we do not add them here
	{NPC_NAXXRAMAS_FOLLOWER,      BOSS_FAERLINA},
    {NPC_NAXXRAMAS_WORSHIPPER,    BOSS_FAERLINA},
    {NPC_DEATH_KNIGHT_UNDERSTUDY, BOSS_RAZUVIOUS},
    {NPC_SIR_ZELIEK,              BOSS_HORSEMEN},
    {NPC_THANE_KORTHAZZ,          BOSS_HORSEMEN},
    {NPC_LADY_BLAUMEUX,           BOSS_HORSEMEN},
    {NPC_BARON_RIVENDARE,         BOSS_HORSEMEN},
    {0,                                       0},
};

enum Doors
{
    DOOR_ROOM_ANUBREKHAN      = 0,
    DOOR_PASSAGE_ANUBREKHAN,
    DOOR_PASSAGE_FAERLINA,
    DOOR_ROOM_MAEXXNA,
    DOOR_ROOM_NOTH,
    DOOR_PASSAGE_NOTH,
    DOOR_ROOM_HEIGAN,
    DOOR_PASSAGE_HEIGAN,
    DOOR_ROOM_LOATHEB,
    DOOR_ROOM_GROBBULUS,
    DOOR_PASSAGE_GLUTH,
    DOOR_ROOM_THADDIUS,
    DOOR_ROOM_GOTHIK,
    DOOR_PASSAGE_GOTHIK,
    DOOR_ROOM_HORSEMEN,
    DOOR_PASSAGE_SAPPHIRON,
    DOOR_ROOM_KELTHUZAD,
    MAX_DOOR_NAXX
};

const float HeiganPos[2] = {2796, -3707};
const float HeiganEruptionSlope[3] =
{
    (-3685 - HeiganPos[1]) /(2724 - HeiganPos[0]),
    (-3647 - HeiganPos[1]) /(2749 - HeiganPos[0]),
    (-3637 - HeiganPos[1]) /(2771 - HeiganPos[0]),
};

inline uint32 GetEruptionSection(float x, float y)
{
    y -= HeiganPos[1];
    if (y < 1.0f)
        return 0;

    x -= HeiganPos[0];
    if (x > -1.0f)
        return 3;

    float slope = y/x;
    for (uint32 i = 0; i < 3; ++i)
        if (slope > HeiganEruptionSlope[i])
            return i;
    return 3;
}

class instance_naxxramas : public InstanceMapScript
{
public:
    instance_naxxramas() : InstanceMapScript("instance_naxxramas", 533) { }

    struct instance_naxxramas_InstanceMapScript : public InstanceScript
    {
        instance_naxxramas_InstanceMapScript(Map* map) : InstanceScript(map)
        {
            PlayerDeathCount = 0;
            HeiganPlayerDeathCount = 0;

            Initialize();
        }

        std::set<uint64> HeiganEruptionGUID[4];

        std::set<uint64> FaerlinaMinion;
        std::set<uint64> RazuviousMinion;
        std::set<uint64> Horsemen;

        uint32 Encounter[MAX_BOSS_NUMBER];
        uint32 PlayerDeathCount;
        uint32 HeiganPlayerDeathCount;
		uint32 AbominationCount;
        uint32 SlimeCheckTimer;
		uint32 StepTimer;
        uint32 Step;
        uint32 StepTimer2;
        uint32 Step2;

        uint64 GothikGateGUID;
        uint64 HorsemenChestGUID;
        uint64 SapphironGUID;
		uint64 Kelthuzad;
        uint64 Faerlina;
        uint64 Thane;
        uint64 Lady ;
        uint64 Baron;
        uint64 Sir;
        uint64 Thaddius;
        uint64 Heigan;
        uint64 Feugen;
        uint64 Stalagg;
		uint64 KTLichKing;
		uint64 SPLichKing;
		uint64 SPKelthuzad;
		uint64 MBKelthuzad;
		uint64 GTThane;
		uint64 GTSir;
		uint64 GTLady;
		uint64 GTBaron;
        uint64 KelthuzadTrigger;
        uint64 NaxxramasPortalGUID[4];
        uint64 NaxxramasDoorsGUID[MAX_DOOR_NAXX];

        GOState GothikDoorState;

        time_t minHorsemenDiedTime;
        time_t maxHorsemenDiedTime;

        void Initialize()
        {
            GothikGateGUID      = 0;
            HorsemenChestGUID   = 0;
            SapphironGUID       = 0;
            Faerlina            = 0;
            Thane               = 0;
            Lady                = 0;
            Baron               = 0;
            Sir                 = 0;
            Thaddius            = 0;
            Heigan              = 0;
            Feugen              = 0;
            Stalagg             = 0;
            Kelthuzad           = 0;
            KelthuzadTrigger    = 0;

            memset(NaxxramasPortalGUID, 0, sizeof(NaxxramasPortalGUID));

            memset(&Encounter, 0, sizeof(Encounter));
            GothikDoorState = GO_STATE_ACTIVE;
            SlimeCheckTimer = 1000;
        }

        bool IsEncounterInProgress() const
        {
            for (int i = 0; i < MAX_BOSS_NUMBER; ++i)
                if (Encounter[i] == IN_PROGRESS)
                    return true;

            return false;
        }

        void OnPlayerKilled(Player* player)
        {
            if (IsEncounterInProgress())
                PlayerDeathCount++;

            if (Encounter[BOSS_HEIGAN] == IN_PROGRESS)
                HeiganPlayerDeathCount++;
        }

        void OnCreatureCreate(Creature* creature)
        {
            switch(creature->GetEntry())
            {
                case NPC_SAPPHIRON:
					SapphironGUID = creature->GetGUID();
					return;
                case NPC_GRAND_WIDOW_FAERLINA:
					Faerlina = creature->GetGUID();
					return;
                case NPC_THANE_KORTHAZZ:
					Thane = creature->GetGUID();
					return;
                case NPC_LADY_BLAUMEUX:
					Lady  = creature->GetGUID();
					return;
                case NPC_BARON_RIVENDARE:
					Baron = creature->GetGUID();
					return;
                case NPC_SIR_ZELIEK:
					Sir = creature->GetGUID();
					return;
                case NPC_THADDIUS:
					Thaddius = creature->GetGUID();
					return;
                case NPC_FEUGEN:
					Feugen = creature->GetGUID();
					return;
                case NPC_STALAGG:
					Stalagg = creature->GetGUID();
					return;
                case NPC_KELTHUZAD:
					Kelthuzad = creature->GetGUID();
					return;
                case NPC_HEIGAN_THE_UNCLEAN:
					Heigan = creature->GetGUID();
					return;
				case NPC_KT_LICH_KING:
					KTLichKing = creature->GetGUID();
					return;
				case NPC_SP_LICH_KING:
					SPLichKing = creature->GetGUID();
					return;
				case NPC_SP_KEL_THUZAD:
					SPKelthuzad = creature->GetGUID();
					return;
				case NPC_MB_KEL_THUZAD:
					MBKelthuzad = creature->GetGUID();
					return;
				case NPC_GT_THANE:
					GTThane = creature->GetGUID();
					return;
				case NPC_GT_SIR:
					GTSir = creature->GetGUID();
					return;
				case NPC_GT_LADY:
					GTLady = creature->GetGUID();
					return;
				case NPC_GT_BARON:
					GTBaron = creature->GetGUID();
					return;
            }

            switch(creature->GetEntry())
            {
                case NPC_NAXXRAMAS_FOLLOWER:
				case NPC_NAXXRAMAS_WORSHIPPER:
                    FaerlinaMinion.insert(creature->GetGUID());
                    break;
                case NPC_DEATH_KNIGHT_UNDERSTUDY:
                    RazuviousMinion.insert(creature->GetGUID());
                    break;
                case NPC_SIR_ZELIEK:
                case NPC_THANE_KORTHAZZ:
                case NPC_LADY_BLAUMEUX:
                case NPC_BARON_RIVENDARE:
                    Horsemen.insert(creature->GetGUID());
                    break;
            }
            //AddMinion(creature, add);
        }

        void OnGameObjectRemove(GameObject* go)
        {
            if (go->GetGOInfo()->displayId == 6785 || go->GetGOInfo()->displayId == 1287)
            {
                uint32 section = GetEruptionSection(go->GetPositionX(), go->GetPositionY());
                HeiganEruptionGUID[section].erase(go->GetGUID());
                return;
            }

            if (go->GetEntry() == GO_SAPPHIRON_BIRTH)
            {
                if (SapphironGUID)
                {
                    if (Creature* sapphiron = instance->GetCreature(SapphironGUID))
                        sapphiron->AI()->DoAction(DATA_SAPPHIRON_BIRTH);
                    return;
                }
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            if (go->GetGOInfo()->displayId == 6785 || go->GetGOInfo()->displayId == 1287)
            {
                uint32 section = GetEruptionSection(go->GetPositionX(), go->GetPositionY());
                HeiganEruptionGUID[section].insert(go->GetGUID());
                return;
            }

            switch(go->GetEntry())
            {
                case GO_GOTHIK_GATE:
                    GothikGateGUID = go->GetGUID();
                    go->SetGoState(GothikDoorState);
                    break;
                case GO_HORSEMEN_CHEST:
					HorsemenChestGUID = go->GetGUID();
					break;
                case GO_HORSEMEN_CHEST_HERO:
					HorsemenChestGUID = go->GetGUID();
					break;
                case GO_KELTHUZAD_PORTAL01:
					NaxxramasPortalGUID[0] = go->GetGUID();
					break;
                case GO_KELTHUZAD_PORTAL02:
					NaxxramasPortalGUID[1] = go->GetGUID();
					break;
                case GO_KELTHUZAD_PORTAL03:
					NaxxramasPortalGUID[2] = go->GetGUID();
					break;
                case GO_KELTHUZAD_PORTAL04:
					NaxxramasPortalGUID[3] = go->GetGUID();
					break;
                case GO_KELTHUZAD_TRIGGER:
					KelthuzadTrigger = go->GetGUID();
					break;
                case GO_ROOM_ANUBREKHAN:
                    NaxxramasDoorsGUID[DOOR_ROOM_ANUBREKHAN] = go->GetGUID();
                    go->SetGoState(Encounter[BOSS_ANUBREKHAN] != IN_PROGRESS ? GO_STATE_ACTIVE : GO_STATE_READY);
                    break;
                case GO_PASSAGE_ANUBREKHAN:
                    NaxxramasDoorsGUID[DOOR_PASSAGE_ANUBREKHAN] = go->GetGUID();
                    go->SetGoState(Encounter[BOSS_ANUBREKHAN] == DONE ? GO_STATE_ACTIVE : GO_STATE_READY);
                    break;
                case GO_PASSAGE_FAERLINA:
                    NaxxramasDoorsGUID[DOOR_PASSAGE_FAERLINA] = go->GetGUID();
                    go->SetGoState(Encounter[BOSS_FAERLINA] == DONE ? GO_STATE_ACTIVE : GO_STATE_READY);
                    break;
                case GO_ROOM_MAEXXNA:
                    NaxxramasDoorsGUID[DOOR_ROOM_MAEXXNA] = go->GetGUID();
                    go->SetGoState((Encounter[BOSS_FAERLINA] == DONE && Encounter[BOSS_MAEXXNA] != IN_PROGRESS) ? GO_STATE_ACTIVE : GO_STATE_READY);
                    break;
                case GO_ROOM_NOTH:
                    NaxxramasDoorsGUID[DOOR_ROOM_NOTH] = go->GetGUID();
                    go->SetGoState(Encounter[BOSS_FAERLINA] != IN_PROGRESS ? GO_STATE_ACTIVE : GO_STATE_READY);
                    break;
                case GO_PASSAGE_NOTH:
                    NaxxramasDoorsGUID[DOOR_PASSAGE_NOTH] = go->GetGUID();
                    go->SetGoState(Encounter[BOSS_NOTH] == DONE ? GO_STATE_ACTIVE : GO_STATE_READY);
                    break;
                case GO_ROOM_HEIGAN:
                    NaxxramasDoorsGUID[DOOR_ROOM_HEIGAN] = go->GetGUID();
                    go->SetGoState(Encounter[BOSS_HEIGAN] != IN_PROGRESS ? GO_STATE_ACTIVE : GO_STATE_READY);
                    break;
                case GO_PASSAGE_HEIGAN:
                    NaxxramasDoorsGUID[DOOR_PASSAGE_HEIGAN] = go->GetGUID();
                    go->SetGoState(Encounter[BOSS_HEIGAN] == DONE ? GO_STATE_ACTIVE : GO_STATE_READY);
                    break;
                case GO_ROOM_LOATHEB:
                    NaxxramasDoorsGUID[DOOR_ROOM_LOATHEB] = go->GetGUID();
                    go->SetGoState((Encounter[BOSS_HEIGAN] == DONE && Encounter[BOSS_LOATHEB] != IN_PROGRESS) ? GO_STATE_ACTIVE : GO_STATE_READY);
                    break;
                case GO_ROOM_GROBBULUS:
                    NaxxramasDoorsGUID[DOOR_ROOM_GROBBULUS] = go->GetGUID();
                    go->SetGoState((Encounter[BOSS_PATCHWERK] == DONE && Encounter[BOSS_GROBBULUS] != IN_PROGRESS) ? GO_STATE_ACTIVE : GO_STATE_READY);
                    break;
                case GO_PASSAGE_GLUTH:
                    NaxxramasDoorsGUID[DOOR_PASSAGE_GLUTH] = go->GetGUID();
                    go->SetGoState(Encounter[BOSS_GLUTH] == DONE ? GO_STATE_ACTIVE : GO_STATE_READY);
                    break;
                case GO_ROOM_THADDIUS:
                    NaxxramasDoorsGUID[DOOR_ROOM_THADDIUS] = go->GetGUID();
                    go->SetGoState((Encounter[BOSS_GLUTH] == DONE && Encounter[BOSS_THADDIUS] != IN_PROGRESS) ? GO_STATE_ACTIVE : GO_STATE_READY);
                    break;
                case GO_ROOM_GOTHIK:
                    NaxxramasDoorsGUID[DOOR_ROOM_GOTHIK] = go->GetGUID();
                    go->SetGoState((Encounter[BOSS_RAZUVIOUS] == DONE && Encounter[BOSS_GOTHIK] != IN_PROGRESS) ? GO_STATE_ACTIVE : GO_STATE_READY);
                    break;
                case GO_PASSAGE_GOTHIK:
                    NaxxramasDoorsGUID[DOOR_PASSAGE_GOTHIK] = go->GetGUID();
                    go->SetGoState(Encounter[BOSS_GOTHIK] == DONE ? GO_STATE_ACTIVE : GO_STATE_READY);
                    break;
                case GO_ROOM_HORSEMEN:
                    NaxxramasDoorsGUID[DOOR_ROOM_HORSEMEN] = go->GetGUID();
                    go->SetGoState((Encounter[BOSS_GOTHIK] == DONE && Encounter[BOSS_HORSEMEN] != IN_PROGRESS) ? GO_STATE_ACTIVE : GO_STATE_READY);
                    break;
                case GO_PASSAGE_SAPPHIRON:
                    NaxxramasDoorsGUID[DOOR_PASSAGE_SAPPHIRON] = go->GetGUID();
                    go->SetGoState(Encounter[BOSS_SAPPHIRON] == DONE ? GO_STATE_ACTIVE : GO_STATE_READY);
                    break;
                case GO_ROOM_KELTHUZAD:
                    NaxxramasDoorsGUID[DOOR_ROOM_KELTHUZAD] = go->GetGUID();
                    go->SetGoState(Encounter[BOSS_KELTHUZAD] != IN_PROGRESS ? GO_STATE_ACTIVE : GO_STATE_READY);
					break;
            }
            //AddDoor(go, add);
        }

        void SetData(uint32 id, uint32 value)
        {
            switch (id)
            {
                case DATA_HEIGAN_ERUPT:
                    HeiganErupt(value);
                    break;
                case DATA_GOTHIK_GATE:
                    if (GameObject *pGothikGate = instance->GetGameObject(GothikGateGUID))
                        pGothikGate->SetGoState(GOState(value));
                    GothikDoorState = GOState(value);
                    break;
                case DATA_HORSEMEN0:
                case DATA_HORSEMEN1:
                case DATA_HORSEMEN2:
                case DATA_HORSEMEN3:
                    if (value == NOT_STARTED)
                    {
                        minHorsemenDiedTime = 0;
                        maxHorsemenDiedTime = 0;
                    }
                    else if (value == DONE)
                    {
                        time_t now = time(NULL);

                        if (minHorsemenDiedTime == 0)
                            minHorsemenDiedTime = now;

                        maxHorsemenDiedTime = now;
                    }
                    break;
                case DATA_ABOMINATION_KILLED:
                    AbominationCount = value;
                    break;
            }
        }

        uint64 GetData64(uint32 id)
        {
            switch (id)
            {
            case DATA_GRAND_WIDOW_FAERLINA:
                return Faerlina;
            case DATA_THANE_KORTHAZZ:
                return Thane;
            case DATA_LADY_BLAUMEUX:
                return Lady ;
            case DATA_BARON_RIVENDARE:
                return Baron;
            case DATA_SIR_ZELIEK:
                return Sir;
            case DATA_THADDIUS:
                return Thaddius;
            case DATA_HEIGAN_THE_UNCLEAN:
                return Heigan;
            case DATA_FEUGEN:
                return Feugen;
            case DATA_STALAGG:
                return Stalagg;
            case DATA_KELTHUZAD:
                return Kelthuzad;
            case DATA_KELTHUZAD_PORTAL_1:
                return NaxxramasPortalGUID[0];
            case DATA_KELTHUZAD_PORTAL_2:
                return NaxxramasPortalGUID[1];
            case DATA_KELTHUZAD_PORTAL_3:
                return NaxxramasPortalGUID[2];
            case DATA_KELTHUZAD_PORTAL_4:
                return NaxxramasPortalGUID[3];
            case DATA_KELTHUZAD_TRIGGER:
                return KelthuzadTrigger;
            case DATA_GO_ROOM_HEIGAN:
                return NaxxramasDoorsGUID[DOOR_ROOM_HEIGAN];
            case DATA_GO_PASSAGE_HEIGAN:
                return NaxxramasDoorsGUID[DOOR_PASSAGE_HEIGAN];
            }
            return 0;
        }

        uint32 GetData(uint32 id)
        {
            if (id == DATA_PLAYER_DEATHS)
            {
                if (InstanceFinished())
                    return PlayerDeathCount;
                else return 1;
            }

            if (id == DATA_HEIGAN_PLAYER_DEATHS)
                return HeiganPlayerDeathCount;

            switch (id)
            {
                case DATA_ABOMINATION_KILLED:
                    return AbominationCount;
                default:
                    break;
            }

            return GetNaxxBossState(id);
        }

        bool SetBossState(uint32 id, EncounterState state)
        {
            UpdateNaxxMinionState(id, state);
            UpdateNaxxDoorState(id, state);

            if (Encounter[id] != DONE)
                Encounter[id] = state;

            if (id == BOSS_HEIGAN && state == NOT_STARTED)
                HeiganPlayerDeathCount = 0;

            if (id == BOSS_HORSEMEN && state == DONE)
            {
                if (GameObject* HorsemenChest = instance->GetGameObject(HorsemenChestGUID))
                    HorsemenChest->SetRespawnTime(HorsemenChest->GetRespawnDelay());
            }

			if (id == BOSS_GOTHIK && state == DONE)
			{
				Step2 = 1;
				StepTimer2 = 100;
				GothikEvent();
			}

			if (id == BOSS_SAPPHIRON && state == DONE)
			{
				Step = 1;
				StepTimer = 100;
				SapphironEvent();
			}

            if (state == DONE)
                SaveToDB();
            return true;
        }

        EncounterState GetNaxxBossState(uint32 id)
        {
            if (id < MAX_BOSS_NUMBER)
                return ((EncounterState)Encounter[id]);
            else return NOT_STARTED;
        }

        void UpdateNaxxDoorState(uint32 id, EncounterState state)
        {
            switch(id)
            {
            case BOSS_ANUBREKHAN:
                HandleGameObject(NaxxramasDoorsGUID[DOOR_ROOM_ANUBREKHAN], state != IN_PROGRESS);
                HandleGameObject(NaxxramasDoorsGUID[DOOR_PASSAGE_ANUBREKHAN], state == DONE);
                break;
            case BOSS_FAERLINA:
                HandleGameObject(NaxxramasDoorsGUID[DOOR_PASSAGE_FAERLINA], state == DONE);
                HandleGameObject(NaxxramasDoorsGUID[DOOR_ROOM_MAEXXNA], state == DONE);
                break;
            case BOSS_MAEXXNA:
                HandleGameObject(NaxxramasDoorsGUID[DOOR_ROOM_MAEXXNA], state != IN_PROGRESS);
                break;
            case BOSS_NOTH:
                HandleGameObject(NaxxramasDoorsGUID[DOOR_ROOM_NOTH], state != IN_PROGRESS);
                HandleGameObject(NaxxramasDoorsGUID[DOOR_PASSAGE_NOTH], state == DONE);
                break;
            case BOSS_HEIGAN:
                HandleGameObject(NaxxramasDoorsGUID[DOOR_ROOM_HEIGAN], state != IN_PROGRESS);
                HandleGameObject(NaxxramasDoorsGUID[DOOR_PASSAGE_HEIGAN], state == DONE);
                HandleGameObject(NaxxramasDoorsGUID[DOOR_ROOM_LOATHEB], state == DONE);
                break;
            case BOSS_LOATHEB:
                HandleGameObject(NaxxramasDoorsGUID[DOOR_ROOM_LOATHEB], state != IN_PROGRESS);
                break;
            case BOSS_PATCHWERK:
                HandleGameObject(NaxxramasDoorsGUID[DOOR_ROOM_GROBBULUS], state == DONE);
                break;
            case BOSS_GROBBULUS:
                HandleGameObject(NaxxramasDoorsGUID[DOOR_ROOM_GROBBULUS], state != IN_PROGRESS);
                break;
            case BOSS_GLUTH:
                HandleGameObject(NaxxramasDoorsGUID[DOOR_PASSAGE_GLUTH], state == DONE);
                HandleGameObject(NaxxramasDoorsGUID[DOOR_ROOM_THADDIUS], state == DONE);
                break;
            case BOSS_THADDIUS:
                HandleGameObject(NaxxramasDoorsGUID[DOOR_ROOM_THADDIUS], state != IN_PROGRESS);
                break;
            case BOSS_RAZUVIOUS:
                HandleGameObject(NaxxramasDoorsGUID[DOOR_ROOM_GOTHIK], state == DONE);
                break;
            case BOSS_GOTHIK:
                HandleGameObject(NaxxramasDoorsGUID[DOOR_ROOM_GOTHIK], state != IN_PROGRESS);
                HandleGameObject(NaxxramasDoorsGUID[DOOR_PASSAGE_GOTHIK], state == DONE);
                HandleGameObject(NaxxramasDoorsGUID[DOOR_ROOM_HORSEMEN], state == DONE);
                break;
            case BOSS_HORSEMEN:
                HandleGameObject(NaxxramasDoorsGUID[DOOR_ROOM_HORSEMEN], state != IN_PROGRESS);
                break;
            case BOSS_SAPPHIRON:
                HandleGameObject(NaxxramasDoorsGUID[DOOR_PASSAGE_SAPPHIRON], state == DONE);
                break;
            case BOSS_KELTHUZAD:
                HandleGameObject(NaxxramasDoorsGUID[DOOR_ROOM_KELTHUZAD], state != IN_PROGRESS);
                break;
            }
        }

        bool InstanceFinished()
        {
            for(int i = 0; i < MAX_BOSS_NUMBER; ++i)
                if (Encounter[i] != DONE)
                    return false;
            return true;
        }

        void UpdateNaxxMinionState(uint32 id, EncounterState state)
        {
            switch(id)
            {
            case BOSS_FAERLINA:
                for (std::set<uint64>::const_iterator i =  FaerlinaMinion.begin(); i != FaerlinaMinion.end(); ++i)
				{
                    if (Creature* minion = instance->GetCreature((*i)))
                        UpdateMinionState(minion, state);
				}
                break;
            case BOSS_RAZUVIOUS:
                for (std::set<uint64>::const_iterator i =  RazuviousMinion.begin(); i != RazuviousMinion.end(); ++i)
				{
                    if (Creature* minion = instance->GetCreature((*i)))
                        UpdateMinionState(minion, state);
				}
                break;
            case BOSS_HORSEMEN:
                for (std::set<uint64>::const_iterator i =  Horsemen.begin(); i != Horsemen.end(); ++i)
				{
                    if (Creature* minion = instance->GetCreature((*i)))
                        UpdateMinionState(minion, state);
				}
				break;
            }
        }

        void HeiganErupt(uint32 section)
        {
            for (uint32 i = 0; i < 4; ++i)
            {
                if (i == section)
                    continue;

                for (std::set<uint64>::const_iterator itr = HeiganEruptionGUID[i].begin(); itr != HeiganEruptionGUID[i].end(); ++itr)
                {
                    if (GameObject* pHeiganEruption = instance->GetGameObject(*itr))
                    {
                        pHeiganEruption->SendCustomAnim(pHeiganEruption->GetGoAnimProgress());
                        pHeiganEruption->CastSpell(NULL, SPELL_ERUPTION);
                    }
                }
            }
        }

		void SapphironEvent()
        {
            if (!Step)
				return;

            if (GetBossState(BOSS_KELTHUZAD) == DONE)
				return;

            Creature* kel = instance->GetCreature(SPKelthuzad);
            Creature* lich = instance->GetCreature(SPLichKing);

            switch(Step)
            {
			       case 1:
                       JumpToNextStep(5000);
                       break;
                   case 2:
					   DoSendQuantumText(SAY_SAPP_DIALOG_1, kel);
                       JumpToNextStep(6000);
                       break;
                   case 3:
					   DoSendQuantumText(SAY_SAPP_DIALOG_2_LICH, lich);
                       JumpToNextStep(16000);
                       break;
                   case 4:
					   DoSendQuantumText(SAY_SAPP_DIALOG_3, kel);
                       JumpToNextStep(9000);
                       break;
                   case 5:
					   DoSendQuantumText(SAY_SAPP_DIALOG_4_LICH, lich);
                       JumpToNextStep(9000);
                       break;
                   case 6:
					   DoSendQuantumText(SAY_SAPP_DIALOG_5,kel);
                       JumpToNextStep(3000);
                       break;
            }
        }

		void GothikEvent()
        {
			if (!Step2)
				return;

            if (GetBossState(BOSS_HORSEMEN) == DONE)
               return;

            Creature* thane = instance->GetCreature(GTThane);
            Creature* sir = instance->GetCreature(GTSir);
            Creature* lady = instance->GetCreature(GTLady);
            Creature* baron = instance->GetCreature(GTBaron);

            switch(Step2)
            {
			       case 1:
                       Jump(10000);
                       break;
                   case 2:
					   DoSendQuantumText(SAY_HORSEMEN_1, sir);
                       Jump(6000);
                       break;
                   case 3:
					   DoSendQuantumText(SAY_HORSEMEN_2, lady);
                       Jump(6000);
                       break;
                   case 4:
					   DoSendQuantumText(SAY_HORSEMEN_3, baron);
                       Jump(6000);
                       break;
                   case 5:
					   DoSendQuantumText(SAY_HORSEMEN_4, lady);
                       Jump(5000);
                       break;
                   case 6:
					   DoSendQuantumText(SAY_HORSEMEN_5, sir);
                       Jump(4000);
                       break;
                   case 7:
					   DoSendQuantumText(SAY_HORSEMEN_6, thane);
                       Jump(5000);
                       break;
                   case 8:
					   DoSendQuantumText(SAY_HORSEMEN_7, baron);
                       Jump(8000);
                       break;
            }
        }

		void JumpToNextStep(uint32 Time)
        {
            StepTimer = Time;
            Step++;
        }

		void Jump(uint32 Time2)
        {
            StepTimer2 = Time2;
            Step2++;
        }

        bool CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* /*source*/, Unit const* /*target = NULL*/, uint32 /*miscvalue1 = 0*/)
        {
            switch (criteria_id)
            {
                case 7600:  // Criteria for achievement 2176: And They Would All Go Down Together 15sec of each other 10-man
                    if (Difficulty(instance->GetSpawnMode()) == RAID_DIFFICULTY_10MAN_NORMAL && (maxHorsemenDiedTime - minHorsemenDiedTime) < 15)
                        return true;
                    return false;
                case 7601:  // Criteria for achievement 2177: And They Would All Go Down Together 15sec of each other 25-man
                    if (Difficulty(instance->GetSpawnMode()) == RAID_DIFFICULTY_25MAN_NORMAL && (maxHorsemenDiedTime - minHorsemenDiedTime) < 15)
                        return true;
                    return false;
            }
            return false;
        }

        std::string GetSaveData()
        {
            std::ostringstream saveStream;
            saveStream << "N X ";
            for(int i = 0; i < MAX_BOSS_NUMBER; ++i)
                saveStream << Encounter[i] << " ";

            saveStream << PlayerDeathCount;
            return saveStream.str();
        }

        void Load(const char * data)
        {
            std::istringstream loadStream(data);
            char dataHead1, dataHead2;
            loadStream >> dataHead1 >> dataHead2;
            std::string newdata = loadStream.str();

            uint32 buff;
            if (dataHead1 == 'N' && dataHead2 == 'X')
            {
                for(int i = 0; i < MAX_BOSS_NUMBER; ++i)
                {
                    loadStream >> buff;
                    Encounter[i]= buff;
                }
                loadStream >> PlayerDeathCount;
            }
        }

		void Update(uint32 diff)
		{
			if (StepTimer < diff)
			{
                SapphironEvent();
			}
            else StepTimer -= diff;

			if (StepTimer2 < diff)
			{
                GothikEvent();
			}
            else StepTimer2 -= diff;
			return;
		}
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_naxxramas_InstanceMapScript(map);
    }
};

class AreaTrigger_at_naxxramas_frostwyrm_wing : public AreaTriggerScript
{
public:
	AreaTrigger_at_naxxramas_frostwyrm_wing() : AreaTriggerScript("at_naxxramas_frostwyrm_wing") { }

	bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
	{
		if (player->IsGameMaster())
			return false;

		InstanceScript* data = player->GetInstanceScript();
		if (data)
			for (uint32 i = BOSS_ANUBREKHAN; i < BOSS_SAPPHIRON; ++i)
				if (data->GetData(i) != DONE)
					return true;

		return false;
	}
};

void AddSC_instance_naxxramas()
{
    new instance_naxxramas();
    new AreaTrigger_at_naxxramas_frostwyrm_wing();
}