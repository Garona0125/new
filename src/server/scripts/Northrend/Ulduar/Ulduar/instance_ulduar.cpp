/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "InstanceScript.h"
#include "ObjectMgr.h"
#include "ulduar.h"

class instance_ulduar : public InstanceMapScript
{
public:
    instance_ulduar() : InstanceMapScript("instance_ulduar", 603) { }

    struct instance_ulduar_InstanceMapScript : public InstanceScript
    {
        instance_ulduar_InstanceMapScript(InstanceMap* map) : InstanceScript(map) { }

        uint32 Encounter[MAX_ENCOUNTER];

        std::string m_strInstData;

        // Leviathan
        uint64 LeviathanGUID;
        uint64 LeviathanGateGUID;
        std::list<uint64> LeviathanDoorGUIDList;
        // Ignis
        uint64 IgnisFurnaceMasterGUID;
        // Razorscale
        uint64 RazorscaleGUID;
        uint64 RazorscaleController;
        uint64 RazorHarpoonGUIDs[4];
        uint64 ExpeditionCommanderGUID;
        // XT-002
        uint64 XT002DeconstructorGUID;
        uint64 XT002DoorGUID;
        uint64 XTToyPileGUIDs[4];
        // Assembly of Iron
        uint64 AssemblyGUIDs[3];
        uint64 IronCouncilEntranceGUID;
        uint64 ArchivumDoorGUID;
        // Kologarn
        uint64 KologarnGUID;
        uint64 KologarnChestGUID;
        uint64 KologarnBridgeGUID;
        uint64 KologarnDoorGUID;
        std::set<uint64> mRubbleSpawns;
        // Auriaya
        uint64 AuriayaGUID;
        // Hodir
        uint64 HodirGUID;
        uint64 HodirIceDoorGUID;
        uint64 HodirStoneDoorGUID;
        uint64 HodirEntranceDoorGUID;
        uint64 HodirChestGUID;
        uint64 HodirRareCacheGUID;
        // Mimiron
        uint64 MimironTramGUID;
        uint64 MimironGUID;
        uint64 LeviathanMKIIGUID;
        uint64 VX001GUID;
        uint64 AerialUnitGUID;
        uint64 MimironElevatorGUID;
        std::list<uint64> MimironDoorGUIDList;
        // Thorim
        uint64 ThorimGUID;
        uint64 ThorimDoorGUID;
        uint64 RunicColossusGUID;
        uint64 RuneGiantGUID;
        uint64 RunicDoorGUID;
        uint64 StoneDoorGUID;
        uint64 ThorimChestGUID;
        // Freya
        uint64 FreyaGUID;
        uint64 ElderBrightleafGUID;
        uint64 ElderIronbranchGUID;
        uint64 ElderStonebarkGUID;
        uint64 FreyaChestGUID;
        // Vezax
        uint64 WayToYoggGUID;
        uint64 VezaxGUID;
        uint64 VezaxDoorGUID;
        // Yogg-Saron
        uint64 YoggSaronGUID;
        uint64 SaraGUID;
        uint64 YoggSaronDoorGUID;
        uint64 YoggSaronBrainDoor1GUID;
        uint64 YoggSaronBrainDoor2GUID;
        uint64 YoggSaronBrainDoor3GUID;
        // Algalon
        uint64 AlgalonGUID;
        uint64 AlgalonBridgeGUID;
        uint64 AlgalonBridgeVisualGUID;
        uint64 AlgalonBridgeDoorGUID;
        uint64 AlgalonGlobeGUID;
        uint64 AlgalonDoor1GUID;
        uint64 AlgalonDoor2GUID;
        uint64 AlgalonAccessGUID;
		// Misc
		uint32 TeamInInstance;
        uint32 ColossusData;
        uint32 SupportKeeperFlag;
        uint32 PlayerDeathFlag;
        uint32 AlgalonKillCount;
        uint32 CountdownTimer;
        uint32 HodirRareCacheData;

        uint32 AlgalonCountdown;
        //   62 - not ready to engage
        //   61 - ready to engage, not engaged yet
        // < 61 - engaged, timer running
        //    0 - failed

        void Initialize()
        {
            SetBossNumber(MAX_ENCOUNTER);

            IgnisFurnaceMasterGUID  = 0;
            RazorscaleGUID          = 0;
            RazorscaleController    = 0;
            ExpeditionCommanderGUID = 0;
            XT002DeconstructorGUID  = 0;
            IronCouncilEntranceGUID = 0;
            ArchivumDoorGUID        = 0;
            KologarnGUID            = 0;
            AuriayaGUID             = 0;
            HodirIceDoorGUID        = 0;
            HodirStoneDoorGUID      = 0;
            HodirEntranceDoorGUID   = 0;
            MimironTramGUID         = 0;
            MimironElevatorGUID     = 0;
            MimironGUID             = 0;
            LeviathanMKIIGUID       = 0;
            HodirGUID               = 0;
            ThorimGUID              = 0;
            ThorimDoorGUID          = 0;
            RunicDoorGUID           = 0;
            StoneDoorGUID           = 0;
            FreyaGUID               = 0;
            VezaxGUID               = 0;
            YoggSaronGUID           = 0;
            AlgalonGUID             = 0;
            AlgalonBridgeGUID       = 0;
            AlgalonBridgeVisualGUID = 0;
            AlgalonBridgeDoorGUID   = 0;
            AlgalonGlobeGUID        = 0;
            AlgalonDoor1GUID        = 0;
            AlgalonDoor2GUID        = 0;
            AlgalonAccessGUID       = 0;
            SaraGUID                = 0;
            KologarnChestGUID       = 0;
            KologarnBridgeGUID      = 0;
            ThorimChestGUID         = 0;
            HodirChestGUID          = 0;
            HodirRareCacheGUID      = 0;
            FreyaChestGUID          = 0;
            LeviathanGateGUID       = 0;
            XT002DoorGUID           = 0;
            VezaxDoorGUID           = 0;
            WayToYoggGUID           = 0;
            YoggSaronDoorGUID       = 0;
            YoggSaronBrainDoor1GUID = 0;
            YoggSaronBrainDoor2GUID = 0;
            YoggSaronBrainDoor3GUID = 0;
            ColossusData            = 0;
            HodirRareCacheData      = 0;
            SupportKeeperFlag       = 0;
            PlayerDeathFlag         = 0;
            AlgalonKillCount        = 0;
			TeamInInstance          = 0;
            AlgalonCountdown        = 62;
            CountdownTimer          = 1*MINUTE*IN_MILLISECONDS;

            memset(Encounter, 0, sizeof(Encounter));
            memset(AssemblyGUIDs, 0, sizeof(AssemblyGUIDs));
            memset(XTToyPileGUIDs, 0, sizeof(XTToyPileGUIDs));
            memset(RazorHarpoonGUIDs, 0, sizeof(RazorHarpoonGUIDs));
        }

        bool IsEncounterInProgress() const
        {
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            {
                if (Encounter[i] == IN_PROGRESS)
                    return true;
            }

            return false;
        }

        uint32 TypeToDeadFlag(uint32 type)
        {
            uint32 return_value = 1;
            for (uint32 i = 0; i < type; i++)
            {
                return_value *= 2;
            }
            return return_value;
        }

        void OnPlayerKilled(Player* /*player*/)
        {
            for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            {
                if (Encounter[i] == IN_PROGRESS)
                {
                    if (i < DATA_ALGALON_THE_OBSERVER)
                        PlayerDeathFlag |= UlduarBossDeadFlags(TypeToDeadFlag(i));
                    else if (i == DATA_ALGALON_THE_OBSERVER)
                        ++AlgalonKillCount; // He feeds on your tears
                }
            }
        }

        void OnCreatureDeath(Creature* creature)
        {
            // reward leviathan kill all over the formation grounds area
            if (creature->GetEntry() == NPC_FLAME_LEVIATHAN)
            {
                Map::PlayerList const &playerList = instance->GetPlayers();

                if (playerList.isEmpty())
                    return;

                for (Map::PlayerList::const_iterator i = playerList.begin(); i != playerList.end(); ++i)
				{
                    if (Player* player = i->getSource())
                    {
                        // has been rewarded
                        if (player->IsAtGroupRewardDistance(creature))
                            continue;

                        // is somewhere else
                        if (player->GetAreaId() != AREA_FORMATION_GROUNDS)
                            continue;

                        if (player->IsAlive() || !player->GetCorpse())
                            player->KilledMonsterCredit(NPC_FLAME_LEVIATHAN, 0);
                    }
				}
            }
        }

        bool CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* /*source*/, Unit const* /*target = NULL*/, uint32 /*miscvalue1 = 0*/)
        {
            switch (criteria_id)
            {
                // Kills without Death Achievement
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_FLAMELEVIATAN_10:
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_FLAMELEVIATAN_25:
                    return !(PlayerDeathFlag & DEAD_FLAME_LEVIATHAN);
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_IGNIS_10:
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_IGNIS_25:
                    return !(PlayerDeathFlag & DEAD_IGNIS);
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_RAZORSCALE_10:
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_RAZORSCALE_25:
                    return !(PlayerDeathFlag & DEAD_RAZORSCALE);
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_XT002_10:
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_XT002_25:
                    return !(PlayerDeathFlag & DEAD_XT002);
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_ASSEMBLY_10:
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_ASSEMBLY_25:
                    return !(PlayerDeathFlag & DEAD_ASSEMBLY);
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_KOLOGARN_10:
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_KOLOGARN_25:
                    return !(PlayerDeathFlag & DEAD_KOLOGARN);
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_AURIAYA_10:
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_AURIAYA_25:
                    return !(PlayerDeathFlag & DEAD_AURIAYA);
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_HODIR_10:
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_HODIR_25:
                    return !(PlayerDeathFlag & DEAD_HODIR);
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_THORIM_10:
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_THORIM_25:
                    return !(PlayerDeathFlag & DEAD_THORIM);
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_FREYA_10:
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_FREYA_25:
                    return !(PlayerDeathFlag & DEAD_FREYA);
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_MIMIRON_10:
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_MIMIRON_25:
                    return !(PlayerDeathFlag & DEAD_MIMIRON);
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_VEZAX_10:
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_VEZAX_25:
                    return !(PlayerDeathFlag & DEAD_VEZAX);
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_YOGGSARON_10:
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_YOGGSARON_25:
                    return !(PlayerDeathFlag & DEAD_YOGGSARON);
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_ALGALON_10:
                case ACHIEVEMENT_CRITERIA_KILL_WITHOUT_DEATHS_ALGALON_25:
                    return !(AlgalonKillCount);
            }

            // Yogg-Saron
            switch (criteria_id)
            {
                case ACHIEVEMENT_CRITERIA_THE_ASSASSINATION_OF_KING_LLANE_10:
                case ACHIEVEMENT_CRITERIA_THE_ASSASSINATION_OF_KING_LLANE_25:
                {
                    if (GetBossState(DATA_YOGG_SARON) != IN_PROGRESS)
                        return false;

                    if (Creature* sara = instance->GetCreature(SaraGUID))
                        return (sara->AI()->GetData(DATA_PORTAL_PHASE) == 0);

                    return false;
                }
                case ACHIEVEMENT_CRITERIA_THE_TORTURED_CHAMPION_10:
                case ACHIEVEMENT_CRITERIA_THE_TORTURED_CHAMPION_25:
                {
                    if (GetBossState(DATA_YOGG_SARON) != IN_PROGRESS)
                        return false;

                    if (Creature* sara = instance->GetCreature(SaraGUID))
                        return (sara->AI()->GetData(DATA_PORTAL_PHASE) == 2);

                    return false;
                }
                case ACHIEVEMENT_CRITERIA_FORGING_OF_THE_DEMON_SOUL_10:
                case ACHIEVEMENT_CRITERIA_FORGING_OF_THE_DEMON_SOUL_25:
                {
                    if (GetBossState(DATA_YOGG_SARON) != IN_PROGRESS)
                        return false;

                    if (Creature* Sara = instance->GetCreature(SaraGUID))
                        return (Sara->AI()->GetData(DATA_PORTAL_PHASE) == 1);

                    return false;
                }
            }
            return false;
        }

		void OnPlayerEnter(Player* player)
		{
			if (!TeamInInstance)
				TeamInInstance = player->GetTeam();
		}
		
		void OnCreatureCreate(Creature* creature)
		{
			if (!TeamInInstance)
			{
				Map::PlayerList const &players = instance->GetPlayers();
				if (!players.isEmpty())
				{
					if (Player* player = players.begin()->getSource())
						TeamInInstance = player->GetTeam();
				}
			}

            switch (creature->GetEntry())
            {
                case NPC_FLAME_LEVIATHAN:
                    LeviathanGUID = creature->GetGUID();
                    break;
                case NPC_IGNIS_FURNACE_MASTER:
                    IgnisFurnaceMasterGUID = creature->GetGUID();
                    break;
                case NPC_RAZORSCALE:
                    RazorscaleGUID = creature->GetGUID();
                    break;
                case NPC_RAZORSCALE_CONTROLLER:
                    RazorscaleController = creature->GetGUID();
                    break;
                case NPC_EXPEDITION_COMMANDER:
                    ExpeditionCommanderGUID = creature->GetGUID();
                    return;
                case NPC_XT_002:
                    XT002DeconstructorGUID = creature->GetGUID();
                    break;
                case NPC_XT_TOY_PILE:
                    for (uint8 i = 0; i < 4; ++i)
                        if (!XTToyPileGUIDs[i])
                            XTToyPileGUIDs[i] = creature->GetGUID();
                    break;
                case NPC_STEELBREAKER:
                    AssemblyGUIDs[0] = creature->GetGUID();
                    break;
                case NPC_MOLGEIM:
                    AssemblyGUIDs[1] = creature->GetGUID();
                    break;
                case NPC_BRUNDIR:
                    AssemblyGUIDs[2] = creature->GetGUID();
                    break;
                case NPC_KOLOGARN:
                    KologarnGUID = creature->GetGUID();
                    break;
                case NPC_KOLOGARN_BRIDGE:
                    if (GetBossState(DATA_KOLOGARN) == DONE)
                    {
                        //creature->SetDeadByDefault(true);
                        creature->setDeathState(CORPSE);
                        creature->DestroyForNearbyPlayers();
                        creature->UpdateObjectVisibility(true);
                    }
                    else
                    {
                        //creature->SetDeadByDefault(false);
                        creature->setDeathState(CORPSE);
                        creature->RemoveCorpse(true);
                    }
                    break;
                case NPC_AURIAYA:
                    AuriayaGUID = creature->GetGUID();
                    break;
                case NPC_MIMIRON:
                    MimironGUID = creature->GetGUID();
                    break;
                case NPC_LEVIATHAN_MKII:
                    LeviathanMKIIGUID = creature->GetGUID();
                    break;
                case NPC_VX_001:
                    VX001GUID = creature->GetGUID();
                    break;
                case NPC_AERIAL_COMMAND_UNIT:
                    AerialUnitGUID = creature->GetGUID();
                    break;
                case NPC_HODIR:
                    HodirGUID = creature->GetGUID();
                    break;
				case NPC_SALVAGED_CHOPPER:
					if (TeamInInstance == ALLIANCE)
					{
						creature->SetDisplayId(25870);
					}
					else if (TeamInInstance == HORDE)
					{
						creature->SetDisplayId(25871);
					}
					break;
				case NPC_ELEMENTALIST_MAHFUUN:
					if (TeamInInstance == HORDE)
					{
						creature->UpdateEntry(NPC_SPIRITWALKER_TARA, HORDE);
					}
					break;
				case NPC_ELEMENTALIST_AVUUN:
					if (TeamInInstance == HORDE)
					{
						creature->UpdateEntry(NPC_SPIRITWALKER_YONA, HORDE);
					}
					break;
				case NPC_EIVI_NIGHTFEATHER:
					if (TeamInInstance == HORDE)
					{
						creature->UpdateEntry(NPC_TOR_GREYCLOUD, HORDE);
					}
					break;
				case NPC_ELLIE_NIGHTFEATHER:
					if (TeamInInstance == HORDE)
					{
						creature->UpdateEntry(NPC_KAR_GREYCLOUD, HORDE);
					}
					break;
				case NPC_MISSY_FLAMECUFFS:
					if (TeamInInstance == HORDE)
					{
						creature->UpdateEntry(NPC_AMIRA_BLAZEWEAVER, HORDE);
					}
					break;
				case NPC_SISSY_FLAMECUFFS:
					if (TeamInInstance == HORDE)
					{
						creature->UpdateEntry(NPC_VEESHA_BLAZEWEAVER, HORDE);
					}
					break;
				case NPC_FIELD_MEDIC_PENNY:
					if (TeamInInstance == HORDE)
					{
						creature->UpdateEntry(NPC_BATTLE_PRIEST_ELIZA, HORDE);
					}
					break;
				case NPC_FIELD_MEDIC_JESSI:
					if (TeamInInstance == HORDE)
					{
						creature->UpdateEntry(NPC_BATTLE_PRIEST_GINA, HORDE);
					}
					break;
                case NPC_THORIM:
                    ThorimGUID = creature->GetGUID();
                    break;
				case NPC_MERCENARY_CAPTAIN_A:
					if (TeamInInstance == HORDE)
					{
						creature->UpdateEntry(NPC_MERCENARY_CAPTAIN_H, HORDE);
					}
					break;
				case NPC_MERCENARY_SOLDIER_A:
					if (TeamInInstance == HORDE)
					{
						creature->UpdateEntry(NPC_MERCENARY_SOLDIER_H, HORDE);
					}
					break;
                case NPC_RUNIC_COLOSSUS:
                    RunicColossusGUID = creature->GetGUID();
                    break;
                case NPC_RUNE_GIANT:
                    RuneGiantGUID = creature->GetGUID();
                    break;
                case NPC_FREYA:
                    FreyaGUID = creature->GetGUID();
                    break;
                case NPC_ELDER_BRIGHTLEAF:
                    ElderBrightleafGUID = creature->GetGUID();
                    creature->SetActive(true);
                    break;
                case NPC_ELDER_IRONBRANCH:
                    ElderIronbranchGUID = creature->GetGUID();
                    creature->SetActive(true);
                    break;
                case NPC_ELDER_STONEBARK:
                    ElderStonebarkGUID = creature->GetGUID();
                    creature->SetActive(true);
                    break;
                case NPC_GENERAL_VEZAX:
                    VezaxGUID = creature->GetGUID();
                    break;
                case NPC_YOGG_SARON:
                    YoggSaronGUID = creature->GetGUID();
                    break;
                case NPC_SARA:
                    SaraGUID = creature->GetGUID();
                    break;
                case NPC_ALGALON:
                    AlgalonGUID = creature->GetGUID();
                    if (AlgalonCountdown < 62)
                    {
                        creature->SetCurrentFaction(FACTION_NEUTRAL);
                        creature->SetActive(true);
                    }
                    else creature->SetVisible(false);
					break;
                }
		}

        void OnGameObjectCreate(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case GO_IRON_COUNCIL_ENTRANCE:
                    IronCouncilEntranceGUID = go->GetGUID();
                    break;
                case GO_ARCHIVUM_DOOR:
                    ArchivumDoorGUID = go->GetGUID();
                    HandleGameObject(0, GetBossState(DATA_ASSEMBLY_OF_IRON) == DONE, go);
                    break;
                case GO_CACHE_OF_LIVING_STONE_10:
                case GO_CACHE_OF_LIVING_STONE_25:
                    KologarnChestGUID = go->GetGUID();
                    break;
                case GO_KOLOGARN_BRIDGE:
                    KologarnBridgeGUID = go->GetGUID();
                    HandleGameObject(0, GetBossState(DATA_KOLOGARN) != DONE, go);
                    break;
                case GO_KOLOGARN_DOOR:
                    KologarnDoorGUID = go->GetGUID();
					HandleGameObject(0, GetBossState(DATA_ASSEMBLY_OF_IRON) == DONE, go);
                    break;
                case GO_CACHE_OF_STORMS_10N:
                case GO_CACHE_OF_STORMS_10H:
				case GO_CACHE_OF_STORMS_25N:
				case GO_CACHE_OF_STORMS_25H:
                    ThorimChestGUID = go->GetGUID();
                    break;
                case GO_THORIM_ENCOUNTER_DOOR:
                    ThorimDoorGUID = go->GetGUID();
                    break;
                case GO_THORIM_STONE_DOOR:
                    StoneDoorGUID = go->GetGUID();
                    break;
                case GO_THORIM_RUNIC_DOOR:
                    RunicDoorGUID = go->GetGUID();
                    break;
                case GO_RARE_CACHE_OF_WINTER_10:
                case GO_RARE_CACHE_OF_WINTER_25:
                   HodirRareCacheGUID = go->GetGUID();
                   break;
                case GO_CACHE_OF_WINTER_10:
                case GO_CACHE_OF_WINTER_25:
                    HodirChestGUID = go->GetGUID();
                    break;
                case GO_HODIR_OUT_DOOR_ICE:
                    HodirIceDoorGUID = go->GetGUID();
                    if (GetBossState(DATA_HODIR) == DONE)
                        HandleGameObject(HodirIceDoorGUID, true);
                    break;
                case GO_HODIR_OUT_DOOR_STONE:
                    HodirStoneDoorGUID = go->GetGUID();
                    if (GetBossState(DATA_HODIR) == DONE)
                        HandleGameObject(HodirIceDoorGUID, true);
                    break;
                case GO_HODIR_IN_DOOR_STONE:
                    HodirEntranceDoorGUID = go->GetGUID();
                    break;
                case GO_FREYA_GIFT_10N:
                case GO_FREYA_GIFT_10H:
                    FreyaChestGUID = go->GetGUID();
                    break;
                case GO_LEVIATHAN_DOOR:
                    go->SetActive(true);
                    LeviathanDoorGUIDList.push_back(go->GetGUID());
                    break;
                case GO_LEVIATHAN_GATE:
                    LeviathanGateGUID = go->GetGUID();
                    if (GetBossState(DATA_FLAME_LEVIATHAN) == DONE)
                        go->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                    else
                        HandleGameObject(0, false, go);
                    break;
                case GO_XT002_DOOR:
                    XT002DoorGUID = go->GetGUID();
                    HandleGameObject(0, GetBossState(DATA_FLAME_LEVIATHAN) == DONE, go);
                    break;
                case GO_MIMIRON_TRAM:
                    go->SetActive(true);
                    MimironTramGUID = go->GetGUID();
                    break;
                case GO_MIMIRON_ELEVATOR:
                    go->SetActive(true);
                    MimironElevatorGUID = go->GetGUID();
                    break;
                case GO_MIMIRON_DOOR_1:
                case GO_MIMIRON_DOOR_2:
                case GO_MIMIRON_DOOR_3:
                    go->SetActive(true);
                    MimironDoorGUIDList.push_back(go->GetGUID());
                    break;
                case GO_WAY_TO_YOGG:
                    WayToYoggGUID = go->GetGUID();
                    if (GetBossState(DATA_FREYA) == DONE && GetBossState(DATA_MIMIRON) == DONE && GetBossState(DATA_HODIR) == DONE && GetBossState(DATA_THORIM) == DONE)
                        go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
                    break;
                case GO_VEZAX_DOOR:
                    VezaxDoorGUID = go->GetGUID();
					HandleGameObject(0, GetBossState(DATA_GENERAL_VEZAX) == DONE, go);
                    break;
                case GO_YOGGSARON_DOOR:
                    YoggSaronDoorGUID = go->GetGUID();
                    HandleGameObject(0, true, go);
                    break;
                case GO_YOGGBRAIN_DOOR_1:
                    YoggSaronBrainDoor1GUID = go->GetGUID();
                    break;
                case GO_YOGGBRAIN_DOOR_2:
                    YoggSaronBrainDoor2GUID = go->GetGUID();
                    HandleGameObject(0, false, go);
                case GO_RAZOR_HARPOON_1:
                    RazorHarpoonGUIDs[0] = go->GetGUID();
                    break;
                case GO_RAZOR_HARPOON_2:
                    RazorHarpoonGUIDs[1] = go->GetGUID();
                    break;
                case GO_RAZOR_HARPOON_3:
                    RazorHarpoonGUIDs[2] = go->GetGUID();
                    break;
                case GO_RAZOR_HARPOON_4:
                    RazorHarpoonGUIDs[3] = go->GetGUID();
                    break;
                case GO_YOGGBRAIN_DOOR_3:
                    YoggSaronBrainDoor3GUID = go->GetGUID();
                    HandleGameObject(0, false, go);
                    break;
				case GO_MOLE_MACHINE:
                    if (GetBossState(DATA_RAZORSCALE) == IN_PROGRESS)
                        go->SetGoState(GO_STATE_ACTIVE);
                   break;
                case GO_ALGALON_PLATFORM:
                    HandleGameObject(0, false, go);
                    break;
                case GO_ALGALON_BRIDGE:
                    AlgalonBridgeGUID = go->GetGUID();
                    HandleGameObject(0, false, go);
                    break;
                case GO_ALGALON_B_VISUAL:
                    AlgalonBridgeVisualGUID = go->GetGUID();
                    HandleGameObject(0, false, go);
                    break;
                case GO_ALGALON_B_DOOR:
                    AlgalonBridgeDoorGUID = go->GetGUID();
                    HandleGameObject(0, true, go);
                    break;
                case GO_ALGALON_GLOBE:
                    AlgalonGlobeGUID = go->GetGUID();
                    HandleGameObject(0, false, go);
                    break;
                case GO_ALGALON_DOOR_1:
                    AlgalonDoor1GUID = go->GetGUID();
                    HandleGameObject(0, AlgalonCountdown < 62 ? true : false, go);
                    break;
                case GO_ALGALON_DOOR_2:
                    AlgalonDoor2GUID = go->GetGUID();
                    HandleGameObject(0, AlgalonCountdown < 62 ? true : false, go);
                    break;
                case GO_ALGALON_ACCESS:
                    AlgalonAccessGUID = go->GetGUID();
                    if (AlgalonCountdown < 62)
                    {
                        go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        go->SetGoState(GO_STATE_ACTIVE);
                    }
                    break;
            }
        }

        void OnGameObjectRemove(GameObject* go)
        {
            switch (go->GetEntry())
            {
                case GO_LEVIATHAN_DOOR:
                    AddDoor(go, false);
                    break;
            }
        }

        void ProcessEvent(WorldObject* /*gameObject*/, uint32 eventId)
        {
            Creature* FlameLeviathan = instance->GetCreature(LeviathanGUID);

            if (FlameLeviathan && FlameLeviathan->IsAlive())
			{
                switch (eventId)
                {
                    case EVENT_TOWER_OF_STORM_DESTROYED:
                        FlameLeviathan->AI()->DoAction(1);
                        break;
                    case EVENT_TOWER_OF_FROST_DESTROYED:
                        FlameLeviathan->AI()->DoAction(2);
                        break;
                    case EVENT_TOWER_OF_FLAMES_DESTROYED:
                        FlameLeviathan->AI()->DoAction(3);
                        break;
                    case EVENT_TOWER_OF_LIFE_DESTROYED:
                        FlameLeviathan->AI()->DoAction(4);
                        break;
                }
			}
        }

        bool SetBossState(uint32 type, EncounterState state)
        {
            if (!InstanceScript::SetBossState(type, state))
                return false;

            if (Encounter[type] != DONE)
                Encounter[type] = state;

            if (state == DONE)
                SaveToDB();

            switch (type)
            {
                case DATA_FLAME_LEVIATHAN:
                    for (std::list<uint64>::iterator i = LeviathanDoorGUIDList.begin(); i != LeviathanDoorGUIDList.end(); i++)
                    {
                        if (GameObject* obj = instance->GetGameObject(*i))
                            obj->SetGoState(state == IN_PROGRESS ? GO_STATE_READY : GO_STATE_ACTIVE);
                    }

                    if (state == DONE)
						HandleGameObject(XT002DoorGUID, true);

                    break;
                case DATA_IGNIS_FURNACE_MASTER:
                case DATA_RAZORSCALE:
                case DATA_XT002_DECONSTRUCTOR:
                    HandleGameObject(XT002DoorGUID, state != IN_PROGRESS);
                    break;
                case DATA_ASSEMBLY_OF_IRON:
                    HandleGameObject(IronCouncilEntranceGUID, state != IN_PROGRESS);
                    if (state == DONE)
					{
                        HandleGameObject(ArchivumDoorGUID, true);
						HandleGameObject(KologarnDoorGUID, true);
					}
                    break;
                case DATA_AURIAYA:
                case DATA_FREYA:
                    break;
                case DATA_MIMIRON:
                    for (std::list<uint64>::iterator i = MimironDoorGUIDList.begin(); i != MimironDoorGUIDList.end(); i++)
                    {
                        if (GameObject* obj = instance->GetGameObject(*i))
                            obj->SetGoState(state == IN_PROGRESS ? GO_STATE_READY : GO_STATE_ACTIVE );
                    }
                    break;
                case DATA_GENERAL_VEZAX:
                    if (state == DONE)
                        HandleGameObject(VezaxDoorGUID, true);
                    break;
                case DATA_YOGG_SARON:
                    if (state == IN_PROGRESS)
                        HandleGameObject(YoggSaronDoorGUID, false);
                    else
                        HandleGameObject(YoggSaronDoorGUID, true);
                        break;
                case DATA_KOLOGARN:
                    if (state == DONE)
                    {
                        if (GameObject* go = instance->GetGameObject(KologarnChestGUID))
                            go->SetRespawnTime(go->GetRespawnDelay());
                        HandleGameObject(KologarnBridgeGUID, false);
                    }
                    break;
                case DATA_HODIR:
                    if (state == DONE)
                    {
                        HandleGameObject(HodirIceDoorGUID, true);
                        HandleGameObject(HodirStoneDoorGUID, true);

                        if (GameObject* HodirRareCache = instance->GetGameObject(HodirRareCacheGUID))
                            if (GetData(DATA_HODIR_RARE_CHEST) == 1)
                                HodirRareCache->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                        if (GameObject* go = instance->GetGameObject(HodirChestGUID))
                            go->SetRespawnTime(go->GetRespawnDelay());
                    }
                    HandleGameObject(HodirEntranceDoorGUID, state != IN_PROGRESS);
                    break;
                case DATA_THORIM:
                    //if (state == DONE)
                    //    if (GameObject* go = instance->GetGameObject(ThorimChestGUID))
                    //        go->SetRespawnTime(go->GetRespawnDelay());
                    if (GameObject* obj = instance->GetGameObject(ThorimDoorGUID))
                        obj->SetGoState(state == IN_PROGRESS ? GO_STATE_READY : GO_STATE_ACTIVE);
                    break;
                case DATA_ALGALON_THE_OBSERVER:
                    switch (state)
                    {
                        case SPECIAL:
                            if (Creature* algalon = instance->GetCreature(AlgalonGUID))
                            {
                                algalon->SetCurrentFaction(FACTION_NEUTRAL);
                                algalon->SetActive(true);
                                algalon->SetVisible(true);
                            }
                            HandleGameObject(AlgalonDoor1GUID, true);
                            HandleGameObject(AlgalonDoor2GUID, true);
                            AlgalonCountdown = 61;
                            SaveToDB();
                            break;
                        case NOT_STARTED:
                            HandleGameObject(AlgalonGlobeGUID, false);
                            HandleGameObject(AlgalonBridgeGUID, false);
                            HandleGameObject(AlgalonBridgeVisualGUID, false);
                            HandleGameObject(AlgalonBridgeDoorGUID, true);
                            break;
                        case IN_PROGRESS:
                            if (AlgalonCountdown > 60)
                            {
                                AlgalonCountdown = 60;
                                DoUpdateWorldState(WORLDSTATE_ALGALON_SHOW, 1);
                                DoUpdateWorldState(WORLDSTATE_ALGALON_TIME, AlgalonCountdown);
                                SaveToDB();
                            }
                            HandleGameObject(AlgalonGlobeGUID, true);
                            HandleGameObject(AlgalonBridgeGUID, true);
                            HandleGameObject(AlgalonBridgeVisualGUID, true);
                            HandleGameObject(AlgalonBridgeDoorGUID, false);
                            break;
                        case DONE:
                            AlgalonCountdown = 0;
                            DoUpdateWorldState(WORLDSTATE_ALGALON_SHOW, 0);
                            SaveToDB();
                            HandleGameObject(AlgalonGlobeGUID, false);
                            HandleGameObject(AlgalonBridgeGUID, false);
                            HandleGameObject(AlgalonBridgeVisualGUID, false);
                            HandleGameObject(AlgalonBridgeDoorGUID, true);
                            break;
                    }
                    break;
             }

             if (GetBossState(DATA_FREYA) == DONE && GetBossState(DATA_MIMIRON) == DONE && GetBossState(DATA_HODIR) == DONE && GetBossState(DATA_THORIM) == DONE)
			 {
                 if (GameObject* go = instance->GetGameObject(WayToYoggGUID))
                     go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
			 }

             return true;
        }

        EncounterState GetBossState(uint32 type)
        {
            if (type > MAX_ENCOUNTER)
                return NOT_STARTED;

            return EncounterState(Encounter[type]);
        }

        void SetData(uint32 type, uint32 data)
        {
            switch (type)
            {
                case DATA_GIANT_COLOSSUS:
                    ColossusData = data;
                    if (data == 2)
                    {
                        if (Creature* colossus = instance->GetCreature(LeviathanGUID))
                            colossus->AI()->DoAction(10);
                        if (GameObject* gate = instance->GetGameObject(LeviathanGateGUID))
                            gate->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                        SaveToDB();
                    }
                    break;
				case DATA_CALL_TRAM:
                    if (GameObject* MimironTram = instance->GetGameObject(MimironTramGUID))
                    {
						instance->LoadGrid(2307, 284.632f);

						if (data == 0)
							MimironTram->SetGoState(GO_STATE_READY);
                        if (data == 1)
							MimironTram->SetGoState(GO_STATE_ACTIVE);

                        // Send movement update to players
                        if (Map* map = MimironTram->GetMap())
                        {
							if (map->IsDungeon())
                            {
                                Map::PlayerList const &PlayerList = map->GetPlayers();

                                if (!PlayerList.isEmpty())
                                {
                                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                    {
                                        if (i->getSource())
                                        {
                                            UpdateData data;
                                            WorldPacket pkt;
                                            MimironTram->BuildValuesUpdateBlockForPlayer(&data, i->getSource());
                                            data.BuildPacket(&pkt);
                                            i->getSource()->GetSession()->SendPacket(&pkt);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    break;
                case DATA_MIMIRON_ELEVATOR:
                    if (GameObject* go = instance->GetGameObject(MimironElevatorGUID))
					{
                        go->SetGoState(GOState(data));
					}
                    break;
                case DATA_RUNIC_DOOR:
                    if (GameObject* go = instance->GetGameObject(RunicDoorGUID))
					{
                        go->SetGoState(GOState(data));
					}
                    break;
                case DATA_STONE_DOOR:
                    if (GameObject* go = instance->GetGameObject(StoneDoorGUID))
					{
                        go->SetGoState(GOState(data));
					}
                    break;
                case DATA_ADD_HELP_FLAG:
                    SupportKeeperFlag |= UlduarKeeperSupport(data);
                    break;
                case DATA_HODIR_RARE_CHEST:
                    HodirRareCacheData = data;
                    break;
                default:
                    break;
            }
        }

        uint64 GetData64(uint32 data)
        {
            switch (data)
            {
                case DATA_FLAME_LEVIATHAN:
					return LeviathanGUID;
                case DATA_IGNIS_FURNACE_MASTER:
					return IgnisFurnaceMasterGUID;
                case DATA_RAZORSCALE:
					return RazorscaleGUID;
                case DATA_RAZORSCALE_CONTROLLER:
					return RazorscaleController;
                case DATA_XT002_DECONSTRUCTOR:
					return XT002DeconstructorGUID;
                case DATA_KOLOGARN:
					return KologarnGUID;
                case DATA_AURIAYA:
					return AuriayaGUID;
                case DATA_MIMIRON:
					return MimironGUID;
                case DATA_LEVIATHAN_MK_II:
					return LeviathanMKIIGUID;
                case DATA_VX_001:
					return VX001GUID;
                case DATA_AERIAL_UNIT:
					return AerialUnitGUID;
                case DATA_TOY_PILE_0:
                case DATA_TOY_PILE_1:
                case DATA_TOY_PILE_2:
                case DATA_TOY_PILE_3:
                    return XTToyPileGUIDs[data - DATA_TOY_PILE_0];
                case DATA_HODIR:
					return HodirGUID;
                case DATA_THORIM:
					return ThorimGUID;
                case DATA_RUNIC_COLOSSUS:
					return RunicColossusGUID;
                case DATA_RUNE_GIANT:
					return RuneGiantGUID;
                case DATA_FREYA:
					return FreyaGUID;
                case DATA_ELDER_BRIGHTLEAF:
					return ElderBrightleafGUID;
                case DATA_ELDER_IRONBRANCH:
					return ElderIronbranchGUID;
                case DATA_ELDER_STONEBARK:
					return ElderStonebarkGUID;
                case DATA_GENERAL_VEZAX:
					return VezaxGUID;
                case DATA_YOGG_SARON:
					return YoggSaronGUID;
                case DATA_SARA:
					return SaraGUID;
                case DATA_ALGALON_THE_OBSERVER:
					return AlgalonGUID;
                case DATA_EXPEDITION_COMMANDER:
					return ExpeditionCommanderGUID;
                case GO_RAZOR_HARPOON_1:
					return RazorHarpoonGUIDs[0];
                case GO_RAZOR_HARPOON_2:
					return RazorHarpoonGUIDs[1];
                case GO_RAZOR_HARPOON_3:
					return RazorHarpoonGUIDs[2];
                case GO_RAZOR_HARPOON_4:
					return RazorHarpoonGUIDs[3];
                case DATA_STEELBREAKER:
					return AssemblyGUIDs[0];
                case DATA_MOLGEIM:
					return AssemblyGUIDs[1];
                case DATA_BRUNDIR:
					return AssemblyGUIDs[2];
                case TYPE_BRAIN_DOOR_1:
					return YoggSaronBrainDoor1GUID;
                case TYPE_BRAIN_DOOR_2:
					return YoggSaronBrainDoor2GUID;
                case TYPE_BRAIN_DOOR_3:
					return YoggSaronBrainDoor3GUID;
            }
            return 0;
        }

        uint32 GetData(uint32 type)
        {
            switch (type)
            {
                case DATA_GIANT_COLOSSUS:
                    return ColossusData;
                case DATA_KEEPER_SUPPORT_YOGG:
                    return SupportKeeperFlag;
                case DATA_HODIR_RARE_CHEST:
                   return HodirRareCacheData;
			   case DATA_TEAM_IN_INSTANCE:
				   return TeamInInstance;
            }
            return 0;
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << "U U " << GetBossSaveData() << GetData(DATA_GIANT_COLOSSUS) << " " << PlayerDeathFlag << " " << AlgalonCountdown;

            OUT_SAVE_INST_DATA_COMPLETE;
            return saveStream.str();
        }

        void Load(const char* strIn)
        {
            if (!strIn)
            {
                OUT_LOAD_INST_DATA_FAIL;
                return;
            }

            OUT_LOAD_INST_DATA(strIn);

            char dataHead1, dataHead2;

            std::istringstream loadStream(strIn);
            loadStream >> dataHead1 >> dataHead2;

            if (dataHead1 == 'U' && dataHead2 == 'U')
            {
                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                {
                    uint32 tmpState;
                    loadStream >> tmpState;
                    if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                        tmpState = NOT_STARTED;

                    SetBossState(i, EncounterState(tmpState));

                    // needed because of custom GetBossState(uint32 type) ?
                    Encounter[i] = tmpState;
                }
                uint32 tmpState, tmpState2, tmpState3;
                loadStream >> tmpState >> tmpState2 >> tmpState3;
                ColossusData = tmpState;
                PlayerDeathFlag = tmpState2;
                AlgalonCountdown = tmpState3;
            }

            OUT_LOAD_INST_DATA_COMPLETE;
        }

        void Update(uint32 diff)
        {
            if (AlgalonCountdown > 0 && AlgalonCountdown < 61)
            {
                if (CountdownTimer < diff)
                {
                    AlgalonCountdown--;

                    if (AlgalonCountdown)
                    {
                        DoUpdateWorldState(WORLDSTATE_ALGALON_SHOW, 1);
                        DoUpdateWorldState(WORLDSTATE_ALGALON_TIME, AlgalonCountdown);
                    }
                    else
                    {
                        if (Creature* algalon = instance->GetCreature(AlgalonGUID))
                            algalon->AI()->DoAction(1);
                    }
                    SaveToDB();
                    CountdownTimer += 1*MINUTE*IN_MILLISECONDS;
                }
                CountdownTimer -= diff;
            }
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_ulduar_InstanceMapScript(map);
    }
};

void AddSC_instance_ulduar()
{
    new instance_ulduar();
}