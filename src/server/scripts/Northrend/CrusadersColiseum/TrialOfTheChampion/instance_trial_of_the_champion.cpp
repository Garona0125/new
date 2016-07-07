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
#include "trial_of_the_champion.h"

class instance_trial_of_the_champion : public InstanceMapScript
{
public:
    instance_trial_of_the_champion() : InstanceMapScript("instance_trial_of_the_champion", 650) { }

    struct instance_trial_of_the_champion_InstanceMapScript : public InstanceScript
    {
        instance_trial_of_the_champion_InstanceMapScript(Map* map) : InstanceScript(map) {}

        uint32 Encounter[MAX_ENCOUNTER];
        uint32 GrandChampionEntry[3];
        uint32 MemoryEntry;
		uint32 TeamInInstance;

        uint64 AnnouncerGUID;
        uint64 TirionGUID;
        uint64 ThrallGUID;
        uint64 GarroshGUID;
        uint64 KingVarianGUID;
        uint64 JainaGUID;
        uint64 GrandChampionGUID[3];
        uint64 BlackKnightGUID;
        uint64 BlackKnightGryphonGUID;
		uint64 PlayerMountsGUID;

        uint64 MainGateGUID;
        uint64 PortcullisGUID;
        uint64 ChampionLootGUID;

        bool AchievementHadWorse;

        void Initialize()
        {
            AnnouncerGUID    = 0;
            TirionGUID       = 0;
            ThrallGUID       = 0;
            GarroshGUID      = 0;
            JainaGUID        = 0;
            MemoryEntry      = 0;
            BlackKnightGUID  = 0;
			PlayerMountsGUID = 0;

			MainGateGUID     = 0;
            PortcullisGUID   = 0;
            ChampionLootGUID = 0;

            AchievementHadWorse = true;

            TeamInInstance = 0;

            memset(&GrandChampionEntry, 0, sizeof(GrandChampionEntry));
            memset(&GrandChampionEntry, 0, sizeof(GrandChampionGUID));
            memset(&Encounter, 0, sizeof(Encounter));
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

            switch(creature->GetEntry())
            {
                case NPC_JAEREN_SUNSWORN:
                    if (TeamInInstance == ALLIANCE)
                        creature->UpdateEntry(NPC_ARELAS_BRIGHTSTAR);
                    AnnouncerGUID = creature->GetGUID();
                    break;
                case NPC_TIRION:
                    TirionGUID = creature->GetGUID();
                    break;
                case NPC_THRALL:
                    ThrallGUID = creature->GetGUID();
                    break;
                case NPC_GARROSH:
                    GarroshGUID = creature->GetGUID();
                    break;
                case NPC_VARIAN:
                    KingVarianGUID = creature->GetGUID();
                    break;
                case NPC_JAINA:
                    JainaGUID = creature->GetGUID();
                    break;
                case NPC_ARGENT_BATTLEWORG:
				case NPC_ARGENT_WARHORSE:
					if (TeamInInstance == ALLIANCE)
						creature->UpdateEntry(NPC_ARGENT_WARHORSE);
					PlayerMountsGUID = creature->GetGUID();
                    break;
                case NPC_BLACK_KNIGHT:
                    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    creature->SetReactState(REACT_PASSIVE);
                    BlackKnightGUID = creature->GetGUID();
                    break;
                case NPC_BLACK_KNIGHT_GRYPHON:
                    BlackKnightGryphonGUID = creature->GetGUID();
                    creature->SetPhaseMask(2, true);
                    break;
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch(go->GetEntry())
            {
                case GO_MAIN_GATE:
                    MainGateGUID = go->GetGUID();
                    break;
                case GO_PORTCULLIS:
                    PortcullisGUID = go->GetGUID();
                    break;
                case GO_CHAMPIONS_LOOT:
                case GO_CHAMPIONS_LOOT_H:
                case GO_EADRIC_LOOT:
                case GO_EADRIC_LOOT_H:
                case GO_PALETRESS_LOOT:
                case GO_PALETRESS_LOOT_H:
                    ChampionLootGUID = go->GetGUID();
                    break;
            }
        }

        void SetData(uint32 type, uint32 data)
        {
            switch(type)
            {
                case BOSS_GRAND_CHAMPIONS:
                    Encounter[0] = data;
                    if (data == SPECIAL)
                    {
						if (Creature* mount = instance->GetCreature(PlayerMountsGUID))
							mount->DespawnAfterAction();
                    }
					else if (data == IN_PROGRESS)
                    {
                        for (uint8 i=0; i<3; i++)
						{
                            if (Creature* boss = instance->GetCreature(GrandChampionGUID[i]))
                                AggroAllPlayers(boss);
						}
                    }
					else if (data == DONE)
                    {
                        DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_ACHIEVEMENT_CHAMPIONS);
                        if (Creature* announcer = instance->GetCreature(AnnouncerGUID))
                        {
                            announcer->GetMotionMaster()->MovePoint(0, 742.742f, 630.207f, 411.172f);
                            announcer->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                            announcer->SummonGameObject(instance->IsHeroic()? GO_CHAMPIONS_LOOT_H : GO_CHAMPIONS_LOOT, 746.59f, 618.49f, 411.09f, 1.42f, 0, 0, 0, 0, 90000000);
                        }
                    }
                    break;
                case BOSS_ARGENT_CHALLENGE_E:
                    Encounter[1] = data;
                    if (data == DONE)
                    {
                        if (Creature* announcer = instance->GetCreature(AnnouncerGUID))
                        {
                            DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_ACHIEVEMENT_EADRIC);
                            announcer->GetMotionMaster()->MovePoint(0, 742.742f, 630.207f, 411.172f);
                            announcer->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                            announcer->SummonGameObject(instance->IsHeroic()? GO_EADRIC_LOOT_H : GO_EADRIC_LOOT, 746.59f, 618.49f, 411.09f, 1.42f, 0, 0, 0, 0, 90000000);
                        }
                    }
                    break;
                case BOSS_ARGENT_CHALLENGE_P:
                    Encounter[2] = data;
                    if (data == DONE)
                    {
                        if (Creature* announcer = instance->GetCreature(AnnouncerGUID))
                        {
                            DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_ACHIEVEMENT_PALETRESS);
                            DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_ACHIEVEMENT_MEMORIES);
                            announcer->GetMotionMaster()->MovePoint(0, 742.742f, 630.207f, 411.172f);
                            announcer->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                            announcer->SummonGameObject(instance->IsHeroic()? GO_PALETRESS_LOOT_H : GO_PALETRESS_LOOT, 746.59f, 618.49f, 411.09f, 1.42f, 0, 0, 0, 0, 90000000);
                        }
                    }
                    break;
                case BOSS_BLACK_KNIGHT:
                    Encounter[3] = data;
                    if (data == DONE)
                        DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_ACHIEVEMENT_BLACK_KNIGHT);
                    break;
                case DATA_GRAND_CHAMPION_ENTRY:
                    for (uint8 i=0; i<3; i++)
                    {
                        if (GrandChampionEntry[i] == 0)
                        {
                            GrandChampionEntry[i] = data;
                            return;
                        }
                    }
                    break;
                case DATA_MEMORY_ENTRY:
                    MemoryEntry = data;
                    break;
            }

			if (data == DONE)
				SaveToDB();
		}

        uint32 GetData(uint32 data)
        {
            switch(data)
            {
                case BOSS_GRAND_CHAMPIONS:
					return Encounter[0];
                case BOSS_ARGENT_CHALLENGE_E:
					return Encounter[1];
                case BOSS_ARGENT_CHALLENGE_P:
					return Encounter[2];
                case BOSS_BLACK_KNIGHT:
					return Encounter[3];

                case DATA_TEAM:
					return TeamInInstance;
            }

            return 0;
        }

        void SetData64(uint32 type, uint64 data)
        {
            switch (type)
            {
                case DATA_GRAND_CHAMPION_GUID:
                    for (uint8 i=0; i<3; i++)
                    {
                        if (GrandChampionGUID[i] == 0)
                        {
                            GrandChampionGUID[i] = data;
                            return;
                        }
                    }
                    break;
            }
        }

        uint64 GetData64(uint32 data)
        {
            switch(data)
            {
                case DATA_ANNOUNCER:
					return AnnouncerGUID;
                case DATA_TIRION:
					return TirionGUID;
                case DATA_THRALL:
					return ThrallGUID;
                case DATA_GARROSH:
					return GarroshGUID;
                case DATA_VARIAN:
					return KingVarianGUID;
                case DATA_JAINA:
					return JainaGUID;
                case DATA_MAIN_GATE:
					return MainGateGUID;
                case DATA_PORTCULLIS:
					return PortcullisGUID;
                case DATA_CHEST:
					return ChampionLootGUID;
                case DATA_BLACK_KNIGHT:
					return BlackKnightGUID;
                case DATA_BLACK_KNIGHT_GRYPHON:
					return BlackKnightGryphonGUID;
            }

            return 0;
        }

        bool CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* /*source*/, Unit const* /*target*/, uint32 /*miscvalue1*/)
        {
            switch(criteria_id)
            {
                case CRITERIA_CHAMPION_JACOB:
                case CRITERIA_CHAMPION_LANA:
                case CRITERIA_CHAMPION_COLOSOS:
                case CRITERIA_CHAMPION_AMBROSE:
                case CRITERIA_CHAMPION_JAELYNE:
                case CRITERIA_CHAMPION_MOKRA:
                case CRITERIA_CHAMPION_VISCERI:
                case CRITERIA_CHAMPION_RUNOK:
                case CRITERIA_CHAMPION_ERESSEA:
                case CRITERIA_CHAMPION_ZULTORE:
                case CRITERIA_CHAMPION_JACOB_H:
                case CRITERIA_CHAMPION_LANA_H:
                case CRITERIA_CHAMPION_COLOSOS_H:
                case CRITERIA_CHAMPION_AMBROSE_H:
                case CRITERIA_CHAMPION_JAELYNE_H:
                case CRITERIA_CHAMPION_MOKRA_H:
                case CRITERIA_CHAMPION_VISCERI_H:
                case CRITERIA_CHAMPION_RUNOK_H:
                case CRITERIA_CHAMPION_ERESSEA_H:
                case CRITERIA_CHAMPION_ZULTORE_H:
					for(uint8 i = 0; i<3; i++)
					{
						if (GrandChampionEntry[i] == GetRelatedCreatureEntry(criteria_id))
							return true;
					}
                    return false;
                case CRITERIA_MEMORY_HOGGER:
                case CRITERIA_MEMORY_VAN_CLEEF:
                case CRITERIA_MEMORY_MUTANUS:
                case CRITERIA_MEMORY_HEROD:
                case CRITERIA_MEMORY_LUCIFIRON:
                case CRITERIA_MEMORY_THUNDERAAN:
                case CRITERIA_MEMORY_CHROMAGGUS:
                case CRITERIA_MEMORY_HAKKAR:
                case CRITERIA_MEMORY_VEKNILASH:
                case CRITERIA_MEMORY_KALITHRESH:
                case CRITERIA_MEMORY_MALCHEZAAR:
                case CRITERIA_MEMORY_GRUUL:
                case CRITERIA_MEMORY_LADY_VASHJ:
                case CRITERIA_MEMORY_ARCHIMONDE:
                case CRITERIA_MEMORY_ILLIDAN:
                case CRITERIA_MEMORY_DELRISSA:
                case CRITERIA_MEMORY_ENTROPIUS:
                case CRITERIA_MEMORY_INGVAR:
                case CRITERIA_MEMORY_CYANIGOSA:
                case CRITERIA_MEMORY_ECK:
                case CRITERIA_MEMORY_ONYXIA:
                case CRITERIA_MEMORY_HEIGAN:
                case CRITERIA_MEMORY_IGNIS:
                case CRITERIA_MEMORY_VEZAX:
                case CRITERIA_MEMORY_ALGALON:
                    return (MemoryEntry == GetRelatedCreatureEntry(criteria_id));
                case CRITERIA_I_VE_HAD_WORSE:
                    return AchievementHadWorse;
            }

            return false;
        }
        uint32 GetRelatedCreatureEntry(uint32 criteria_id)
        {
            switch(criteria_id)
            {
                case CRITERIA_CHAMPION_JACOB:
                case CRITERIA_CHAMPION_JACOB_H:
                    return NPC_JACOB;
                case CRITERIA_CHAMPION_LANA:
                case CRITERIA_CHAMPION_LANA_H:
                    return NPC_LANA;
                case CRITERIA_CHAMPION_COLOSOS:
                case CRITERIA_CHAMPION_COLOSOS_H:
                    return NPC_COLOSOS;
                case CRITERIA_CHAMPION_AMBROSE:
                case CRITERIA_CHAMPION_AMBROSE_H:
                    return NPC_AMBROSE;
                case CRITERIA_CHAMPION_JAELYNE:
                case CRITERIA_CHAMPION_JAELYNE_H:
                    return NPC_JAELYNE;
                case CRITERIA_CHAMPION_MOKRA:
                case CRITERIA_CHAMPION_MOKRA_H:
                    return NPC_MOKRA;
                case CRITERIA_CHAMPION_VISCERI:
                case CRITERIA_CHAMPION_VISCERI_H:
                    return NPC_VISCERI;
                case CRITERIA_CHAMPION_RUNOK:
                case CRITERIA_CHAMPION_RUNOK_H:
                    return NPC_RUNOK;
                case CRITERIA_CHAMPION_ERESSEA:
                case CRITERIA_CHAMPION_ERESSEA_H:
                    return NPC_ERESSEA;
                case CRITERIA_CHAMPION_ZULTORE:
                case CRITERIA_CHAMPION_ZULTORE_H:
                    return NPC_ZULTORE;
                case CRITERIA_MEMORY_HOGGER:
					return NPC_MEMORY_HOGGER;
                case CRITERIA_MEMORY_VAN_CLEEF:
					return NPC_MEMORY_VAN_CLEEF;
                case CRITERIA_MEMORY_MUTANUS:
					return NPC_MEMORY_MUTANUS;
                case CRITERIA_MEMORY_HEROD:
					return NPC_MEMORY_HEROD;
                case CRITERIA_MEMORY_LUCIFIRON:
					return NPC_MEMORY_LUCIFIRON;
                case CRITERIA_MEMORY_THUNDERAAN:
					return NPC_MEMORY_THUNDERAAN;
                case CRITERIA_MEMORY_CHROMAGGUS:
					return NPC_MEMORY_CHROMAGGUS;
                case CRITERIA_MEMORY_HAKKAR:
					return NPC_MEMORY_HAKKAR;
                case CRITERIA_MEMORY_VEKNILASH:
					return NPC_MEMORY_VEKNILASH;
                case CRITERIA_MEMORY_KALITHRESH:
					return NPC_MEMORY_KALITHRESH;
                case CRITERIA_MEMORY_MALCHEZAAR:
					return NPC_MEMORY_MALCHEZAAR;
                case CRITERIA_MEMORY_GRUUL:
					return NPC_MEMORY_GRUUL;
                case CRITERIA_MEMORY_LADY_VASHJ:
					return NPC_MEMORY_LADY_VASHJ;
                case CRITERIA_MEMORY_ARCHIMONDE:
					return NPC_MEMORY_ARCHIMONDE;
                case CRITERIA_MEMORY_ILLIDAN:
					return NPC_MEMORY_ILLIDAN;
                case CRITERIA_MEMORY_DELRISSA:
					return NPC_MEMORY_DELRISSA;
                case CRITERIA_MEMORY_ENTROPIUS:
					return NPC_MEMORY_ENTROPIUS;
                case CRITERIA_MEMORY_INGVAR:
					return NPC_MEMORY_INGVAR;
                case CRITERIA_MEMORY_CYANIGOSA:
					return NPC_MEMORY_CYANIGOSA;
                case CRITERIA_MEMORY_ECK:
					return NPC_MEMORY_ECK;
                case CRITERIA_MEMORY_ONYXIA:
					return NPC_MEMORY_ONYXIA;
                case CRITERIA_MEMORY_HEIGAN:
					return NPC_MEMORY_HEIGAN;
                case CRITERIA_MEMORY_IGNIS:
					return NPC_MEMORY_IGNIS;
                case CRITERIA_MEMORY_VEZAX:
					return NPC_MEMORY_VEZAX;
                case CRITERIA_MEMORY_ALGALON:
					return NPC_MEMORY_ALGALON;
            }
            return 0;
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;

            saveStream << "T C " << Encounter[0]
                << ' ' << Encounter[1]
                << ' ' << Encounter[2]
                << ' ' << Encounter[3];

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

            char dataHead1, dataHead2;
            uint16 data0, data1, data2, data3;

            std::istringstream loadStream(in);
            loadStream >> dataHead1 >> dataHead2 >> data0 >> data1 >> data2 >> data3;

            if (dataHead1 == 'T' && dataHead2 == 'C')
            {
                Encounter[0] = data0;
                Encounter[1] = data1;
                Encounter[2] = data2;
                Encounter[3] = data3;

                for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                    if (Encounter[i] == IN_PROGRESS)
                        Encounter[i] = NOT_STARTED;

                if (Encounter[1] != DONE && (Encounter[1] == DONE || Encounter[2] == DONE))
                {
                    // TODO: Respawn announcer OR Spawn Black Knight on the arena
                }
            }
			else OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }
    };

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_trial_of_the_champion_InstanceMapScript(map);
    }
};

void AddSC_instance_trial_of_the_champion()
{
    new instance_trial_of_the_champion();
}