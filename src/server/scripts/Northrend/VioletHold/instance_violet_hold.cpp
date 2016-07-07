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
#include "violet_hold.h"

class instance_violet_hold : public InstanceMapScript
{
public:
    instance_violet_hold() : InstanceMapScript("instance_violet_hold", 608) { }

    struct instance_violet_hold_InstanceMapScript : public InstanceScript
    {
        instance_violet_hold_InstanceMapScript(Map* map) : InstanceScript(map) {}

        uint64 Moragg;
        uint64 Erekem;
        uint64 ErekemGuard[2];
        uint64 Ichoron;
        uint64 Lavanthor;
        uint64 Xevozz;
        uint64 Zuramat;
        uint64 Cyanigosa;
        uint64 Sinclari;
        uint64 DefenseSystem;

        uint64 MoraggCell;
        uint64 ErekemCell;
        uint64 ErekemLeftGuardCell;
        uint64 ErekemRightGuardCell;
        uint64 IchoronCell;
        uint64 LavanthorCell;
        uint64 XevozzCell;
        uint64 ZuramatCell;
        uint64 MainDoor;
        uint64 TeleportationPortal;
        uint64 SaboteurPortal;

        uint64 ActivationCrystal[4];

        uint32 ActivationTimer;
        uint32 CyanigosaEventTimer;
        uint32 DoorSpellTimer;

        uint8 WaveCount;
        uint8 Location;
        uint8 FirstBoss;
        uint8 SecondBoss;
        uint8 RemoveNpc;

        uint8 DoorIntegrity;

        uint16 Encounter[MAX_ENCOUNTER];
        uint8 CountErekemGuards;
        uint8 CountActivationCrystals;
        uint8 CyanigosaEventPhase;
        uint8 MainEventPhase; // SPECIAL: pre event animations, IN_PROGRESS: event itself

        bool bActive;
        bool bWiped;
        bool bIsDoorSpellCasted;
        bool bCrystalActivated;

        std::list<uint8> NpcAtDoorCastingList;

        std::string str_data;

        void Initialize()
        {
            Moragg = 0;
            Erekem = 0;
            Ichoron = 0;
            Lavanthor = 0;
            Xevozz = 0;
            Zuramat = 0;
            Cyanigosa = 0;
            Sinclari = 0;
            DefenseSystem = 0;

            MoraggCell = 0;
            ErekemCell = 0;
            ErekemGuard[0] = 0;
            ErekemGuard[1] = 0;
            IchoronCell = 0;
            LavanthorCell = 0;
            XevozzCell = 0;
            ZuramatCell = 0;
            MainDoor = 0;
            TeleportationPortal = 0;
            SaboteurPortal = 0;

            RemoveNpc = 0;

            DoorIntegrity = 100;

            WaveCount = 0;
            Location = urand(0, 5);
            FirstBoss = 0;
            SecondBoss = 0;
            CountErekemGuards = 0;
            CountActivationCrystals = 0;
            CyanigosaEventPhase = 1;
            MainEventPhase = NOT_STARTED;

            ActivationTimer = 5000;
            DoorSpellTimer = 2000;
            CyanigosaEventTimer = 3*IN_MILLISECONDS;

            bActive = false;
            bIsDoorSpellCasted = false;
            bCrystalActivated = false;
            MainEventPhase = NOT_STARTED;

            memset(&Encounter, 0, sizeof(Encounter));
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
                case NPC_XEVOZZ:
                    Xevozz = creature->GetGUID();
                    break;
                case NPC_LAVANTHOR:
                    Lavanthor = creature->GetGUID();
                    break;
                case NPC_ICHORON:
                    Ichoron = creature->GetGUID();
                    break;
                case NPC_ZURAMAT:
                    Zuramat = creature->GetGUID();
                    break;
                case NPC_EREKEM:
                    Erekem = creature->GetGUID();
                    break;
                case NPC_EREKEM_GUARD:
                    if (CountErekemGuards < 2)
                    {
                        ErekemGuard[CountErekemGuards++] = creature->GetGUID();
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NON_ATTACKABLE);
                    }
                    break;
                case NPC_MORAGG:
                    Moragg = creature->GetGUID();
                    break;
                case NPC_CYANIGOSA:
                    Cyanigosa = creature->GetGUID();
                    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NON_ATTACKABLE);
                    break;
                case NPC_SINCLARI:
                    Sinclari = creature->GetGUID();
                    break;
                case NPC_DEFENSE_SYSTEM:
                    DefenseSystem = creature->GetGUID();
                    break;
            }

            if (creature->GetGUID() == FirstBoss || creature->GetGUID() == SecondBoss)
            {
                creature->AllLootRemovedFromCorpse();
                creature->RemoveLootMode(1);
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case GO_EREKEM_GUARD_1_DOOR:
                    ErekemLeftGuardCell = go->GetGUID();
                    break;
                case GO_EREKEM_GUARD_2_DOOR:
                    ErekemRightGuardCell = go->GetGUID();
                    break;
                case GO_EREKEM_DOOR:
                    ErekemCell = go->GetGUID();
                    break;
                case GO_ZURAMAT_DOOR:
                    ZuramatCell = go->GetGUID();
                    break;
                case GO_LAVANTHOR_DOOR:
                    LavanthorCell = go->GetGUID();
                    break;
                case GO_MORAGG_DOOR:
                    MoraggCell = go->GetGUID();
                    break;
                case GO_ICHORON_DOOR:
                    IchoronCell = go->GetGUID();
                    break;
                case GO_XEVOZZ_DOOR:
                    XevozzCell = go->GetGUID();
                    break;
                case GO_MAIN_DOOR:
                    MainDoor = go->GetGUID();
                    break;
                case GO_ACTIVATION_CRYSTAL:
                    if (CountActivationCrystals < 4)
                        ActivationCrystal[CountActivationCrystals++] = go->GetGUID();
                    break;
            }
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
                case DATA_1ST_BOSS_EVENT:
                    if (Encounter[0] != DONE)
                    {
                        Encounter[0] = data;
                        if (data == DONE)
                            SaveToDB();
                    }
                    break;
                case DATA_2ND_BOSS_EVENT:
                    if (Encounter[1] != DONE)
                    {
                        Encounter[1] = data;
                        if (data == DONE)
                            SaveToDB();
                    }
                    break;
                case DATA_CYANIGOSA_EVENT:
                    Encounter[2] = data;
                    if (data == DONE)
                    {
                        SaveToDB();
                        MainEventPhase = DONE;
                        if (GameObject* mainDoor = instance->GetGameObject(MainDoor))
                            mainDoor->SetGoState(GO_STATE_ACTIVE);
                    }
                    break;
                case DATA_WAVE_COUNT:
                    WaveCount = data;
                    bActive = true;
                    break;
                case DATA_REMOVE_NPC:
                    RemoveNpc = data;
                    break;
                case DATA_PORTAL_LOCATION:
                    Location = (uint8)data;
                    break;
                case DATA_DOOR_INTEGRITY:
                    DoorIntegrity = data;
                    DoUpdateWorldState(WORLD_STATE_VH_PRISON_STATE, DoorIntegrity);
                    break;
                case DATA_NPC_PRESENCE_AT_DOOR_ADD:
                    NpcAtDoorCastingList.push_back(data);
                    break;
                case DATA_NPC_PRESENCE_AT_DOOR_REMOVE:
                    if (!NpcAtDoorCastingList.empty())
                        NpcAtDoorCastingList.pop_back();
                    break;
                case DATA_MAIN_DOOR:
                    if (GameObject* mainDoor = instance->GetGameObject(MainDoor))
                    {
                        switch (data)
                        {
                            case GO_STATE_ACTIVE:
                                mainDoor->SetGoState(GO_STATE_ACTIVE);
                                break;
                            case GO_STATE_READY:
                                mainDoor->SetGoState(GO_STATE_READY);
                                break;
                            case GO_STATE_ACTIVE_ALTERNATIVE:
                                mainDoor->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                                break;
                        }
                    }
                    break;
                case DATA_START_BOSS_ENCOUNTER:
                    switch (WaveCount)
                    {
                        case 6:
                            StartBossEncounter(FirstBoss);
                            break;
                        case 12:
                            StartBossEncounter(SecondBoss);
                            break;
                    }
                    break;
                case DATA_MAIN_EVENT_PHASE:
                    MainEventPhase = data;
                    if (data == IN_PROGRESS) // Start event
                    {
                        if (GameObject* mainDoor = instance->GetGameObject(MainDoor))
                            mainDoor->SetGoState(GO_STATE_READY);
                        WaveCount = 1;
                        bActive = true;
                        RemoveNpc = 0; // might not have been reset after a wipe on a boss.
                    }
                    break;
            }
        }

        uint32 GetData(uint32 type)
        {
            switch (type)
            {
                case DATA_1ST_BOSS_EVENT:
					return Encounter[0];
                case DATA_2ND_BOSS_EVENT:
					return Encounter[1];
                case DATA_CYANIGOSA_EVENT:
					return Encounter[2];
                case DATA_WAVE_COUNT:
					return WaveCount;
                case DATA_REMOVE_NPC:
					return RemoveNpc;
                case DATA_PORTAL_LOCATION:
					return Location;
                case DATA_DOOR_INTEGRITY:
					return DoorIntegrity;
                case DATA_NPC_PRESENCE_AT_DOOR:
					return NpcAtDoorCastingList.size();
                case DATA_FIRST_BOSS:
					return FirstBoss;
                case DATA_SECOND_BOSS:
					return SecondBoss;
                case DATA_MAIN_EVENT_PHASE:
					return MainEventPhase;
                case DATA_DEFENSELESS:
					return (DoorIntegrity == 100) ? 1 : 0;
            }
            return 0;
        }

        uint64 GetData64(uint32 identifier)
        {
            switch (identifier)
            {
                case DATA_MORAGG:
					return Moragg;
                case DATA_EREKEM:
					return Erekem;
                case DATA_EREKEM_GUARD_1:
					return ErekemGuard[0];
                case DATA_EREKEM_GUARD_2:
					return ErekemGuard[1];
                case DATA_ICHORON:
					return Ichoron;
                case DATA_LAVANTHOR:
					return Lavanthor;
                case DATA_XEVOZZ:
					return Xevozz;
                case DATA_ZURAMAT:
					return Zuramat;
                case DATA_CYANIGOSA:
					return Cyanigosa;
                case DATA_MORAGG_CELL:
					return MoraggCell;
                case DATA_EREKEM_CELL:
					return ErekemCell;
                case DATA_EREKEM_RIGHT_GUARD_CELL:
					return ErekemRightGuardCell;
                case DATA_EREKEM_LEFT_GUARD_CELL:
					return ErekemLeftGuardCell;
                case DATA_ICHORON_CELL:
					return IchoronCell;
                case DATA_LAVANTHOR_CELL:
					return LavanthorCell;
                case DATA_XEVOZZ_CELL:
					return XevozzCell;
                case DATA_ZURAMAT_CELL:
					return ZuramatCell;
                case DATA_MAIN_DOOR:
					return MainDoor;
                case DATA_SINCLARI:
					return Sinclari;
                case DATA_TELEPORTATION_PORTAL:
					return TeleportationPortal;
                case DATA_SABOTEUR_PORTAL:
					return SaboteurPortal;
            }
            return 0;
        }

        void SpawnPortal()
        {
            SetData(DATA_PORTAL_LOCATION, (GetData(DATA_PORTAL_LOCATION) + urand(1, 5))%6);
            if (Creature* sinclari = instance->GetCreature(Sinclari))
			{
                if (Creature* portal = sinclari->SummonCreature(NPC_TELEPORTATION_PORTAL, PortalLocation[GetData(DATA_PORTAL_LOCATION)], TEMPSUMMON_CORPSE_DESPAWN))
                    TeleportationPortal = portal->GetGUID();
			}
        }

        void StartBossEncounter(uint8 Boss, bool bForceRespawn = true)
        {
            Creature* boss = NULL;

            switch (Boss)
            {
                case BOSS_MORAGG:
                    HandleGameObject(MoraggCell, bForceRespawn);
                    boss = instance->GetCreature(Moragg);
                    if (boss)
                        boss->GetMotionMaster()->MovePoint(0, BossStartMove1);
                    break;
                case BOSS_EREKEM:
                    HandleGameObject(ErekemCell, bForceRespawn);
                    HandleGameObject(ErekemRightGuardCell, bForceRespawn);
                    HandleGameObject(ErekemLeftGuardCell, bForceRespawn);

                    boss = instance->GetCreature(Erekem);

                    if (boss)
                        boss->GetMotionMaster()->MovePoint(0, BossStartMove2);

                    if (Creature* Guard1 = instance->GetCreature(ErekemGuard[0]))
                    {
                        if (bForceRespawn)
                            Guard1->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NON_ATTACKABLE);
                        else
                            Guard1->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NON_ATTACKABLE);
                        Guard1->GetMotionMaster()->MovePoint(0, BossStartMove21);
                    }

                    if (Creature* Guard2 = instance->GetCreature(ErekemGuard[1]))
                    {
                        if (bForceRespawn)
                            Guard2->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NON_ATTACKABLE);
                        else
                            Guard2->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NON_ATTACKABLE);
                        Guard2->GetMotionMaster()->MovePoint(0, BossStartMove22);
                    }
                    break;
                case BOSS_ICHORON:
                    HandleGameObject(IchoronCell, bForceRespawn);
                    boss = instance->GetCreature(Ichoron);
                    if (boss)
                        boss->GetMotionMaster()->MovePoint(0, BossStartMove3);
                    break;
                case BOSS_LAVANTHOR:
                    HandleGameObject(LavanthorCell, bForceRespawn);
                    boss = instance->GetCreature(Lavanthor);
                    if (boss)
                        boss->GetMotionMaster()->MovePoint(0, BossStartMove4);
                    break;
                case BOSS_XEVOZZ:
                    HandleGameObject(XevozzCell, bForceRespawn);
                    boss = instance->GetCreature(Xevozz);
                    if (boss)
                        boss->GetMotionMaster()->MovePoint(0, BossStartMove5);
                    break;
                case BOSS_ZURAMAT:
                    HandleGameObject(ZuramatCell, bForceRespawn);
                    boss = instance->GetCreature(Zuramat);
                    if (boss)
                        boss->GetMotionMaster()->MovePoint(0, BossStartMove6);
                    break;
            }

            // generic boss state changes
            if (boss)
            {
                boss->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NON_ATTACKABLE);
                boss->SetReactState(REACT_AGGRESSIVE);

                if (!bForceRespawn)
                {
                    if (boss->IsDead())
                    {
                        // respawn but avoid to be looted again
                        boss->Respawn();
                        boss->RemoveLootMode(1);
                    }
                    boss->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NON_ATTACKABLE);
                    WaveCount = 0;
                }
            }
        }

        void AddWave()
        {
            DoUpdateWorldState(WORLD_STATE_VH, 1);
            DoUpdateWorldState(WORLD_STATE_VH_WAVE_COUNT, WaveCount);

            switch (WaveCount)
            {
                case 6:
                    if (FirstBoss == 0)
                        FirstBoss = urand(1, 6);
                    if (Creature* sinclari = instance->GetCreature(Sinclari))
                    {
                        if (Creature* portal = sinclari->SummonCreature(NPC_TELEPORTATION_PORTAL, MiddleRoomPortalSaboLocation, TEMPSUMMON_CORPSE_DESPAWN))
                            SaboteurPortal = portal->GetGUID();

                        if (Creature* azureSaboteur = sinclari->SummonCreature(NPC_SABOTEOUR, MiddleRoomLocation, TEMPSUMMON_DEAD_DESPAWN))
                            azureSaboteur->CastSpell(azureSaboteur, SPELL_SABOTEUR_SHIELD_EFFECT, false);
                    }
                    break;
                case 12:
                    if (SecondBoss == 0)
                        do
                        {
                            SecondBoss = urand(1, 6);
                        }
                        while (SecondBoss == FirstBoss);
                    if (Creature* sinclari = instance->GetCreature(Sinclari))
                    {
                        if (Creature* portal = sinclari->SummonCreature(NPC_TELEPORTATION_PORTAL, MiddleRoomPortalSaboLocation, TEMPSUMMON_CORPSE_DESPAWN))
                            SaboteurPortal = portal->GetGUID();

                        if (Creature* azureSaboteur = sinclari->SummonCreature(NPC_SABOTEOUR, MiddleRoomLocation, TEMPSUMMON_DEAD_DESPAWN))
                            azureSaboteur->CastSpell(azureSaboteur, SPELL_SABOTEUR_SHIELD_EFFECT, false);
                    }
                    break;
                case 18:
                {
                    Creature* sinclari = instance->GetCreature(Sinclari);
                    if (sinclari)
                        sinclari->SummonCreature(NPC_CYANIGOSA, CyanigosasSpawnLocation, TEMPSUMMON_DEAD_DESPAWN);
                    break;
                }
                case 1:
                {
                    if (GameObject* mainDoor = instance->GetGameObject(MainDoor))
                        mainDoor->SetGoState(GO_STATE_READY);
                    DoUpdateWorldState(WORLD_STATE_VH_PRISON_STATE, 100);
                }
                default:
                    SpawnPortal();
                    break;
            }
        }

        std::string GetSaveData()
        {
			OUT_SAVE_INST_DATA;
			std::ostringstream saveStream;
            saveStream << "V H " << (uint16)Encounter[0]
			<< ' ' << (uint16)Encounter[1]
			<< ' ' << (uint16)Encounter[2]
			<< ' ' << (uint16)FirstBoss
				<< ' ' << (uint16)SecondBoss;
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
            uint16 data0, data1, data2, data3, data4;

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3 >> data4;

            if (dataHead1 == 'V' && dataHead2 == 'H')
            {
                Encounter[0] = data0;
                Encounter[1] = data1;
                Encounter[2] = data2;

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (Encounter[i] == IN_PROGRESS)
                        Encounter[i] = NOT_STARTED;

                FirstBoss = uint8(data3);
                SecondBoss = uint8(data4);
            }
			else OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }

        bool CheckWipe()
        {
            Map::PlayerList const &players = instance->GetPlayers();
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            {
                Player* player = itr->getSource();
                if (player->IsGameMaster())
                    continue;

                if (player->IsAlive() && GetData(DATA_DOOR_INTEGRITY) > 0)
                    return false;
            }
            return true;
        }

        void Update(uint32 diff)
        {
            if (!instance->HavePlayers())
                return;

            // portals should spawn if other portal is dead and doors are closed
            if (bActive && MainEventPhase == IN_PROGRESS)
            {
                if (ActivationTimer < diff)
                {
                    AddWave();
                    bActive = false;

                    switch (WaveCount)
                    {
                        case 6:
                        case 12:
                            ActivationTimer = 35000;
                            break;
                        default:
                            ActivationTimer = 5000;
                            break;
                    }
                }
                else ActivationTimer -= diff;
            }

            // if main event is in progress and players have wiped or failed then reset instance
            if ((MainEventPhase == IN_PROGRESS && CheckWipe()) || MainEventPhase == FAIL)
            {
                ActivateCrystal(true);
                DoUpdateWorldState(WORLD_STATE_VH, 0);
                SetData(DATA_REMOVE_NPC, 1);
                StartBossEncounter(FirstBoss, false);
                StartBossEncounter(SecondBoss, false);

                SetData(DATA_MAIN_DOOR, GO_STATE_ACTIVE);
                SetData(DATA_WAVE_COUNT, 0);
                MainEventPhase = NOT_STARTED;
                DoorIntegrity = 100;

                if (Creature* sinclari = instance->GetCreature(Sinclari))
                {
                    sinclari->SetVisible(true);

                    std::list<Creature*> GuardList;
                    sinclari->GetCreatureListWithEntryInGrid(GuardList, NPC_VIOLET_HOLD_GUARD, 40.0f);
                    if (!GuardList.empty())
                    {
                        for (std::list<Creature*>::const_iterator itr = GuardList.begin(); itr != GuardList.end(); ++itr)
                        {
                            if (Creature* guard = *itr)
                            {
                                guard->SetVisible(true);
                                guard->SetReactState(REACT_AGGRESSIVE);
                                guard->GetMotionMaster()->MovePoint(1, guard->GetHomePosition());
                            }
                        }
                    }
                    sinclari->GetMotionMaster()->MovePoint(1, sinclari->GetHomePosition());
                    sinclari->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
            }

            // Cyanigosa is spawned but not tranformed, prefight event
            Creature* cyanigosa = instance->GetCreature(Cyanigosa);
            if (cyanigosa && !cyanigosa->HasAura(SPELL_CYANIGOSA_SPELL_TRANSFORM))
            {
                if (CyanigosaEventTimer <= diff)
                {
                    switch (CyanigosaEventPhase)
                    {
                        case 1:
                            cyanigosa->CastSpell(cyanigosa, SPELL_CYANIGOSA_BLUE_AURA, false);
                            CyanigosaEventTimer = 7*IN_MILLISECONDS;
                            ++CyanigosaEventPhase;
                            break;
                        case 2:
                            cyanigosa->GetMotionMaster()->MoveJump(MiddleRoomLocation.GetPositionX(), MiddleRoomLocation.GetPositionY(), MiddleRoomLocation.GetPositionZ(), 10.0f, 20.0f);
                            cyanigosa->CastSpell(cyanigosa, SPELL_CYANIGOSA_BLUE_AURA, false);
                            CyanigosaEventTimer = 7*IN_MILLISECONDS;
                            ++CyanigosaEventPhase;
                            break;
                        case 3:
                            cyanigosa->RemoveAurasDueToSpell(SPELL_CYANIGOSA_BLUE_AURA);
                            cyanigosa->CastSpell(cyanigosa, SPELL_CYANIGOSA_SPELL_TRANSFORM, 0);
                            cyanigosa->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NON_ATTACKABLE);
                            cyanigosa->SetReactState(REACT_AGGRESSIVE);
                            CyanigosaEventTimer = 2*IN_MILLISECONDS;
                            ++CyanigosaEventPhase;
                            break;
                        case 4:
                            CyanigosaEventPhase = 0;
                            break;
                    }
                }
                else
                    CyanigosaEventTimer -= diff;
            }

            // if there are NPCs in front of the prison door, which are casting the door seal spell and doors are active
            if (GetData(DATA_NPC_PRESENCE_AT_DOOR) && MainEventPhase == IN_PROGRESS)
            {
                // if door integrity is > 0 then decrase it's integrity state
                if (GetData(DATA_DOOR_INTEGRITY))
                {
                    if (DoorSpellTimer < diff)
                    {
                        int32 i = GetData(DATA_DOOR_INTEGRITY) - GetData(DATA_NPC_PRESENCE_AT_DOOR);
                        SetData(DATA_DOOR_INTEGRITY, i > 0 ? uint32(i) : 0);
                        DoorSpellTimer = 2000;
                    }
                    else
                        DoorSpellTimer -= diff;
                }
                else
                    MainEventPhase = FAIL;
            }
        }

        void ActivateCrystal(bool wipe)
        {
            if (Creature* defense = instance->GetCreature(DefenseSystem))
                defense->AI()->DoAction(wipe ? 2 : 1);
        }

        void ProcessEvent(WorldObject* /*object*/, uint32 eventId)
        {
            switch (eventId)
            {
                case EVENT_ACTIVATE_CRYSTAL:
					bCrystalActivated = true; // Activation by player's will throw event signal
                    ActivateCrystal(false);
                    break;
            }
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_violet_hold_InstanceMapScript(map);
    }
};

void AddSC_instance_violet_hold()
{
    new instance_violet_hold();
}