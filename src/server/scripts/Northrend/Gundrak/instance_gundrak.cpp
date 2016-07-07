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
#include "gundrak.h"

class instance_gundrak : public InstanceMapScript
{
public:
    instance_gundrak() : InstanceMapScript("instance_gundrak", 604) { }

    struct instance_gundrak_InstanceMapScript : public InstanceScript
    {
        instance_gundrak_InstanceMapScript(Map* map) : InstanceScript(map)
		{
			HeroicMode = map->IsHeroic();
		}

		bool HeroicMode;
        bool SpawnSupport;

        uint8 AliveDweller;
        uint32 UpdateTimer;
        uint32 Phase;
        uint64 ToActivate;

        uint64 SladRan;
        uint64 Moorabi;
        uint64 DrakkariColossus;
        uint64 GalDarah;
        uint64 EckTheFerocious;

        uint64 SladRanAltar;
        uint64 MoorabiAltar;
        uint64 DrakkariColossusAltar;
        uint64 SladRanStatue;
        uint64 MoorabiStatue;
        uint64 DrakkariColossusStatue;
        uint64 GalDarahStatue;
        uint64 EckTheFerociousDoor;
        uint64 EckTheFerociousDoorBehind;
        uint64 GalDarahDoor1;
        uint64 GalDarahDoor2;
        uint64 GalDarahDoor3;
        uint64 Bridge;
        uint64 Collision;

        uint32 Encounter[MAX_ENCOUNTER];

        GOState SladRanStatueState;
        GOState MoorabiStatueState;
        GOState DrakkariColossusStatueState;
        GOState GalDarahStatueState;
        GOState BridgeState;
        GOState CollisionState;

        std::string str_data;

        void Initialize()
        {
            SpawnSupport = false;
            AliveDweller = MAX_DWELLER;

            UpdateTimer = 0;
            Phase = 0;
            ToActivate = 0;

            SladRan = 0;
            Moorabi = 0;
            DrakkariColossus = 0;
            GalDarah = 0;
            EckTheFerocious = 0;

            SladRanAltar = 0;
            MoorabiAltar = 0;
            DrakkariColossusAltar = 0;

            SladRanStatue = 0;
            MoorabiStatue = 0;
            DrakkariColossusStatue = 0;
            GalDarahStatue = 0;

            EckTheFerociousDoor = 0;
            EckTheFerociousDoorBehind = 0;
            GalDarahDoor1 = 0;
            GalDarahDoor2 = 0;
            GalDarahDoor3 = 0;

            Bridge = 0;
            Collision = 0;

            SladRanStatueState = GO_STATE_ACTIVE;
            MoorabiStatueState = GO_STATE_ACTIVE;
            DrakkariColossusStatueState = GO_STATE_ACTIVE;
            GalDarahStatueState = GO_STATE_READY;
            BridgeState = GO_STATE_ACTIVE;
            CollisionState = GO_STATE_READY;

            memset(&Encounter, 0, sizeof(Encounter));
        }

       bool IsEncounterInProgress() const
        {
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                if (Encounter[i] == IN_PROGRESS) return true;

            return false;
        }

        void OnCreatureCreate(Creature* creature)
        {
            switch (creature->GetEntry())
            {
                case NPC_SLAD_RAN:
                    SladRan = creature->GetGUID();
                    break;
                case NPC_MOORABI:
                    Moorabi = creature->GetGUID();
                    break;
                case NPC_GALDARAH:
                    GalDarah = creature->GetGUID();
                    break;
                case NPC_DRAKKARI_COLOSSUS:
                    DrakkariColossus = creature->GetGUID();
                    break;
                case NPC_ECK:
                    EckTheFerocious = creature->GetGUID();
                    creature->SetVisible((AliveDweller > 0) ? false : true);
                    break;
                case NPC_RUIN_DWELLER:
                    if (creature->IsDead())
                        SetData(DATA_RUIN_DWELLER_DIED, 1);
                    break;
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case GO_SLADRAN_ALTAR:
                    SladRanAltar = go->GetGUID();
                    // Make sure that they start out as unusuable
                    go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    if (Encounter[0] == DONE)
                    {
                        if (SladRanStatueState == GO_STATE_ACTIVE)
                            go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        else
                        {
                            ++Phase;
                            go->SetGoState(GO_STATE_ACTIVE);
                        }
                    }
                    break;
                case GO_MORABI_ALTAR:
                    MoorabiAltar = go->GetGUID();
                    // Make sure that they start out as unusuable
                    go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    if (Encounter[0] == DONE)
                    {
                        if (MoorabiStatueState == GO_STATE_ACTIVE)
                            go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        else
                        {
                            ++Phase;
                            go->SetGoState(GO_STATE_ACTIVE);
                        }
                    }
                    break;
                case GO_COLOSSUS_ALTAR:
                    DrakkariColossusAltar = go->GetGUID();
                    // Make sure that they start out as unusuable
                    go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    if (Encounter[0] == DONE)
                    {
                        if (DrakkariColossusStatueState == GO_STATE_ACTIVE)
                            go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        else
                        {
                            ++Phase;
                            go->SetGoState(GO_STATE_ACTIVE);
                        }
                    }
                    break;
                case GO_SLADRAN_STATUE:
                    SladRanStatue = go->GetGUID();
                    go->SetGoState(SladRanStatueState);
                    break;
                case GO_MORABI_STATUE:
                    MoorabiStatue = go->GetGUID();
                    go->SetGoState(MoorabiStatueState);
                    break;
                case GO_GALDARAH_STATUE:
                    GalDarahStatue = go->GetGUID();
                    go->SetGoState(GalDarahStatueState);
                    break;
                case GO_COLOSSUS_STATUE:
                    DrakkariColossusStatue = go->GetGUID();
                    go->SetGoState(DrakkariColossusStatueState);
                    break;
                case GO_ECK_DOOR:
                    EckTheFerociousDoor = go->GetGUID();
                    if (HeroicMode && Encounter[1] == DONE)
                        HandleGameObject(0, true, go);
                    break;
                case GO_ECK_BEHIND_DOOR:
                    EckTheFerociousDoorBehind = go->GetGUID();
                    if (HeroicMode && Encounter[4] == DONE)
                        HandleGameObject(0, true, go);
                case GO_GALDARAH_DOOR_1:
                    GalDarahDoor1 = go->GetGUID();
                    if (Encounter[3] == DONE)
                        HandleGameObject(0, true, go);
                    break;
                case GO_GALDARAH_DOOR_2:
                    GalDarahDoor2 = go->GetGUID();
                    if (Encounter[3] == DONE)
                        HandleGameObject(0, true, go);
                    break;
                case GO_GALDARAH_DOOR_3:
                    GalDarahDoor3 = go->GetGUID();
                    break;
                case GO_BRIDGE_1:
                    Bridge = go->GetGUID();
                    go->SetGoState(BridgeState);
                    break;
                case GO_COLISION:
                    Collision = go->GetGUID();
                    go->SetGoState(CollisionState);

                    // Can't spawn here with SpawnGameObject because go isn't added to world yet...
                    if (CollisionState == GO_STATE_ACTIVE_ALTERNATIVE)
                        SpawnSupport = true;
                    break;
            }
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
                case DATA_SLAD_RAN_EVENT:
                    Encounter[0] = data;
                    if (data == DONE)
                    {
                        if (GameObject* go = instance->GetGameObject(SladRanAltar))
                            go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    }
                    break;
                case DATA_MOORABI_EVENT:
                    Encounter[1] = data;
                    if (data == DONE)
                    {
                        if (GameObject* go = instance->GetGameObject(MoorabiAltar))
                            go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        if (HeroicMode)
                            HandleGameObject(EckTheFerociousDoor, true);
                    }
                    break;
                case DATA_DRAKKARI_COLOSSUS_EVENT:
                    Encounter[2] = data;
                    if (data == DONE)
                    {
                        if (GameObject* go = instance->GetGameObject(DrakkariColossusAltar))
                            go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    }
                    break;
                case DATA_GAL_DARAH_EVENT:
                    Encounter[3] = data;
                    if (data == DONE)
                    {
                        HandleGameObject(GalDarahDoor1, true);
                        HandleGameObject(GalDarahDoor2, true);
                    }
                    HandleGameObject(GalDarahDoor3, data != IN_PROGRESS);
                    break;
                case DATA_ECK_THE_FEROCIOUS_EVENT:
                    Encounter[4] = data;
                    if (HeroicMode && data == DONE)
                        HandleGameObject(EckTheFerociousDoorBehind, true);
                    break;
                case DATA_RUIN_DWELLER_DIED:
                    --AliveDweller;
                    if (!AliveDweller)
                        if (Creature* eck = instance->GetCreature(EckTheFerocious))
                            eck->SetVisible(true);
                    break;
            }

            if (data == DONE)
                SaveToDB();
        }

        void SetData64(uint32 type, uint64 data)
        {
            if (type == DATA_STATUE_ACTIVATE)
            {
                ToActivate = data;
                UpdateTimer = 3500;
                ++Phase;
            }
        }

        uint32 GetData(uint32 type)
        {
            switch (type)
            {
                case DATA_SLAD_RAN_EVENT:
                    return Encounter[0];
                case DATA_MOORABI_EVENT:
                    return Encounter[1];
                case DATA_GAL_DARAH_EVENT:
                    return Encounter[2];
                case DATA_DRAKKARI_COLOSSUS_EVENT:
                    return Encounter[3];
                case DATA_ECK_THE_FEROCIOUS_EVENT:
                    return Encounter[4];
            }

            return 0;
        }

        uint64 GetData64(uint32 type)
        {
            switch (type)
            {
                case DATA_SLAD_RAN_ALTAR:
                    return SladRanAltar;
                case DATA_MOORABI_ALTAR:
                    return MoorabiAltar;
                case DATA_DRAKKARI_COLOSSUS_ALTAR:
                    return DrakkariColossusAltar;
                case DATA_SLAD_RAN_STATUE:
                    return SladRanStatue;
                case DATA_MOORABI_STATUE:
                    return MoorabiStatue;
                case DATA_DRAKKARI_COLOSSUS_STATUE:
                    return DrakkariColossusStatue;
                case DATA_DRAKKARI_COLOSSUS:
                    return DrakkariColossus;
                case DATA_STATUE_ACTIVATE:
                    return ToActivate;
            }

            return 0;
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << "G D " << Encounter[0] << ' ' << Encounter[1] << ' '
                 << Encounter[2] << ' ' << Encounter[3] << ' ' << Encounter[4] << ' '
                 << (SladRanStatue ? GetObjState(SladRanStatue) : GO_STATE_ACTIVE) << ' ' << (MoorabiStatue ? GetObjState(MoorabiStatue) : GO_STATE_ACTIVE) << ' '
                 << (DrakkariColossusStatue ? GetObjState(DrakkariColossusStatue) : GO_STATE_ACTIVE) << ' ' << (GalDarahStatue ? GetObjState(GalDarahStatue) : GO_STATE_READY) << ' '
                 << (Bridge ? GetObjState(Bridge) : GO_STATE_ACTIVE) << ' ' << (Collision ? GetObjState(Collision) : GO_STATE_READY);

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
            uint16 data0, data1, data2, data3, data4, data5, data6, data7, data8, data9, data10;

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3
                       >> data4 >> data5 >> data6 >> data7 >> data8 >> data9 >> data10;

            if (dataHead1 == 'G' && dataHead2 == 'D')
            {
                Encounter[0] = data0;
                Encounter[1] = data1;
                Encounter[2] = data2;
                Encounter[3] = data3;
                Encounter[4] = data4;
                SladRanStatueState = GOState(data5);
                MoorabiStatueState = GOState(data6);
                DrakkariColossusStatueState = GOState(data7);
                GalDarahStatueState = GOState(data8);
                BridgeState = GOState(data9);
                CollisionState = GOState(data10);

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (Encounter[i] == IN_PROGRESS)
                        Encounter[i] = NOT_STARTED;
            }
			else OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }

         void Update(uint32 diff)
         {
             // Spawn the support for the bridge if necessary
             if (SpawnSupport)
             {
                 if (GameObject* collision = instance->GetGameObject(Collision))
                     collision->SummonGameObject(GO_BRIDGE_2, collision->GetPositionX(), collision->GetPositionY(), collision->GetPositionZ(), collision->GetOrientation(), 0, 0, 0, 0, 0);
                 SpawnSupport = false;
             }

             // If there is nothing to activate, then return
             if (!ToActivate)
                 return;

             if (UpdateTimer < diff)
             {
                 UpdateTimer = 0;
                 if (ToActivate == Bridge)
                 {
                     GameObject* bridge = instance->GetGameObject(Bridge);
                     GameObject* collision = instance->GetGameObject(Collision);
                     GameObject* sladRanStatue = instance->GetGameObject(SladRanStatue);
                     GameObject* moorabiStatue = instance->GetGameObject(MoorabiStatue);
                     GameObject* drakkariColossusStatue = instance->GetGameObject(DrakkariColossusStatue);
                     GameObject* galDarahStatue = instance->GetGameObject(GalDarahStatue);

                     ToActivate = 0;

                     if (bridge && collision && sladRanStatue && moorabiStatue && drakkariColossusStatue && galDarahStatue)
                     {
                         bridge->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                         collision->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                         sladRanStatue->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                         moorabiStatue->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                         drakkariColossusStatue->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                         galDarahStatue->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);

                         // Add the GO that solidifies the bridge so you can walk on it
                         SpawnSupport = true;
                         SaveToDB();
                     }
                 }
                 else
                 {
                     uint32 spell = 0;
                     GameObject* altar = 0;
                     if (ToActivate == SladRanStatue)
                     {
                         spell = SPELL_FIRE_BEAM_SNAKE;
                         altar = instance->GetGameObject(SladRanAltar);
                     }
                     else if (ToActivate == MoorabiStatue)
                     {
                         spell = SPELL_FIRE_BEAM_MAMMOTH;
                         altar = instance->GetGameObject(MoorabiAltar);
                     }
                     else if (ToActivate == DrakkariColossusStatue)
                     {
                         spell = SPELL_FIRE_BEAM_ELEMENTAL;
                         altar = instance->GetGameObject(DrakkariColossusAltar);
                     }

                     // This is a workaround to make the beam cast properly. The caster should be ID 30298 but since the spells
                     // all are with scripted target for that same ID, it will hit itself.
                     if (altar)
					 {
                         if (Creature* trigger = altar->SummonCreature(18721, altar->GetPositionX(), altar->GetPositionY(), altar->GetPositionZ() + 3, altar->GetOrientation(), TEMPSUMMON_CORPSE_DESPAWN, 5000))
                         {
                             // Set the trigger model to invisible
                             trigger->SetDisplayId(MODEL_ID_INVISIBLE);
                             trigger->CastSpell(trigger, spell, false);
                         }
					 }

                     if (GameObject* statueGO = instance->GetGameObject(ToActivate))
                         statueGO->SetGoState(GO_STATE_READY);

                     ToActivate = 0;

                     if (Phase == 3)
                         SetData64(DATA_STATUE_ACTIVATE, Bridge);
                     else
                         SaveToDB(); // Don't save in between last statue and bridge turning in case of crash leading to stuck instance
                }
            }
            else UpdateTimer -= diff;
        }

         GOState GetObjState(uint64 guid)
         {
             if (GameObject* go = instance->GetGameObject(guid))
                 return go->GetGoState();
             return GO_STATE_ACTIVE;
         }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_gundrak_InstanceMapScript(map);
    }
};

class go_gundrak_altar : public GameObjectScript
{
    public:
        go_gundrak_altar() : GameObjectScript("go_gundrak_altar") { }

        bool OnGossipHello(Player* /*player*/, GameObject* go)
        {
            InstanceScript* instance = go->GetInstanceScript();
            uint64 statue = 0;

            go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
            go->SetGoState(GO_STATE_ACTIVE);

            if (instance)
            {
                switch (go->GetEntry())
                {
                    case GO_SLADRAN_ALTAR:
                        statue = instance->GetData64(DATA_SLAD_RAN_STATUE);
                        break;
                    case GO_MORABI_ALTAR:
                        statue = instance->GetData64(DATA_MOORABI_STATUE);
                        break;
                    case GO_COLOSSUS_ALTAR:
                        statue = instance->GetData64(DATA_DRAKKARI_COLOSSUS_STATUE);
                        break;
                }

                if (!instance->GetData64(DATA_STATUE_ACTIVATE))
                {
                    instance->SetData64(DATA_STATUE_ACTIVATE, statue);
                    go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                    go->SetGoState(GO_STATE_ACTIVE);
                }
                return true;
            }
            return false;
        }
};

void AddSC_instance_gundrak()
{
    new instance_gundrak();
    new go_gundrak_altar();
}