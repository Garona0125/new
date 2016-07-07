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
#include "QuantumGossip.h"
#include "QuantumSystemText.h"
#include "QuantumEscortAI.h"
#include "PassiveAI.h"
#include "Player.h"
#include "SpellInfo.h"
#include "culling_of_stratholme.h"

enum Texts
{
    //First Act - Uther and Jaina Dialog
    SAY_PHASE101                               = -1595070,  //Arthas
    SAY_PHASE102                               = -1595071,  //Uther
    SAY_PHASE103                               = -1595072,  //Arthas
    SAY_PHASE104                               = -1595073,  //Arthas
    SAY_PHASE105                               = -1595074,  //Uther
    SAY_PHASE106                               = -1595075,  //Arthas
    SAY_PHASE107                               = -1595076,  //Uther
    SAY_PHASE108                               = -1595077,  //Arthas
    SAY_PHASE109                               = -1595078,  //Arthas
    SAY_PHASE110                               = -1595079,  //Uther
    SAY_PHASE111                               = -1595080,  //Arthas
    SAY_PHASE112                               = -1595081,  //Uther
    SAY_PHASE113                               = -1595082,  //Jaina
    SAY_PHASE114                               = -1595083,  //Arthas
    SAY_PHASE115                               = -1595084,  //Uther
    SAY_PHASE116                               = -1595085,  //Arthas
    SAY_PHASE117                               = -1595086,  //Jaina
    SAY_PHASE118                               = -1595087,  //Arthas
    //Second Act - City Streets
    SAY_PHASE201                               = -1595088,  //Arthas
    SAY_PHASE202                               = -1595089,  //Cityman
    SAY_PHASE203                               = -1595090,  //Arthas
    SAY_PHASE204                               = -1595091,  //Crazyman
    SAY_PHASE205                               = -1595092,  //Arthas
    SAY_PHASE206                               = -1595009,  //Malganis
    SAY_PHASE207                               = -1595010,  //Malganis
    SAY_PHASE208                               = -1595093,  //Arthas
    SAY_PHASE209                               = -1595094,  //Arthas
    SAY_PHASE210                               = -1595095,  //Arthas
    //Third Act - Town Hall
    SAY_PHASE301                               = -1595096,  //Arthas
    SAY_PHASE302                               = -1595097,  //Drakonian
    SAY_PHASE303                               = -1595098,  //Arthas
    SAY_PHASE304                               = -1595099,  //Arthas
    SAY_PHASE305                               = -1595100,  //Drakonian
    SAY_PHASE306                               = -1595101,  //Arthas
    SAY_PHASE307                               = -1595102,  //Arthas
    SAY_PHASE308                               = -1595103,  //Arthas
    SAY_PHASE309                               = -1595104,  //Arthas
    SAY_PHASE310                               = -1595105,  //Arthas
    SAY_PHASE311                               = -1595106,  //Arthas
    SAY_PHASE312                               = -1595107,  //Arthas
    SAY_PHASE313                               = -1595108,  //Arthas
    SAY_PHASE314                               = -1595000,  //Epoch
    SAY_PHASE315                               = -1595109,  //Arthas
    //Fourth Act - Fire Corridor
    SAY_PHASE401                               = -1595110,  //Arthas
    SAY_PHASE402                               = -1595111,  //Arthas
    SAY_PHASE403                               = -1595112,  //Arthas
    SAY_PHASE404                               = -1595113,  //Arthas
    SAY_PHASE405                               = -1595114,  //Arthas
    SAY_PHASE406                               = -1595115,  //Arthas
    SAY_PHASE407                               = -1595116,  //Arthas
    //Fifth Act - Mal'Ganis Fight
    SAY_PHASE501                               = -1595117,  //Arthas
    SAY_PHASE502                               = -1595118,  //Arthas
    SAY_PHASE503                               = -1595119,  //Arthas
    SAY_PHASE504                               = -1595120,  //Arthas
};

enum Spells
{
    SPELL_FEAR              = 39176,
    SPELL_DEVOTION_AURA     = 52442,
    SPELL_EXORCISM_5N       = 52445,
    SPELL_EXORCISM_5H       = 58822,
    SPELL_HOLY_LIGHT        = 52444,
    SPELL_ARCANE_DISRUPTION = 49590,
	SPELL_CRATES_CREDIT     = 58109,
};

enum EncounterData
{
    ENCOUNTER_WAVES_NUMBER     = 8,
    ENCOUNTER_WAVES_MAX_SPAWNS = 7,
    ENCOUNTER_DRACONIAN_NUMBER = 4,
    ENCOUNTER_CHRONO_SPAWNS    = 19,
};

float WavesLocations[ENCOUNTER_WAVES_NUMBER][ENCOUNTER_WAVES_MAX_SPAWNS][5] =
{
	{
		// Wave 1
		{NPC_DEVOURING_GHOUL, 2164.698975f, 1255.392944f, 135.040878f, 0.490202f},
        {NPC_DEVOURING_GHOUL, 2183.501465f, 1263.079102f, 134.859055f, 3.169981f},
        {NPC_DEVOURING_GHOUL, 2177.512939f, 1247.313843f, 135.846695f, 1.696574f},
        {NPC_DEVOURING_GHOUL, 2171.991943f, 1246.615845f, 135.745026f, 1.696574f},
		{NPC_DARK_NECROMANCER, 2175.1f, 1253.22f, 135.148f, 1.75263f},
		{NPC_MASTER_NECROMANCER, 2170.41f, 1252.58f, 135.068f, 1.89636f},
        {0, 0, 0, 0, 0}},
    {
		// Wave 2
        {NPC_DEVOURING_GHOUL, 2254.434326f, 1163.427612f, 138.055038f, 2.077358f},
        {NPC_DEVOURING_GHOUL, 2254.703613f, 1158.867798f, 138.212234f, 2.345532f},
        {NPC_DEVOURING_GHOUL, 2257.615723f, 1162.310913f, 138.091202f, 2.077358f},
        {NPC_DARK_NECROMANCER, 2258.258057f, 1157.250732f, 138.272873f, 2.387766f},
		{NPC_ACOLYTE, 2255.7f, 1155.29f, 138.363f, 2.31104f},
		{NPC_PATCHWORK_CONSTRUCT, 2261.19f, 1160.95f, 138.137f, 2.58594f},
        {0, 0, 0, 0, 0}},
    {
		// Wave 3
        {NPC_TOMB_STALKER, 2348.120117f, 1202.302490f, 130.491104f, 4.698538f},
        {NPC_DEVOURING_GHOUL, 2352.863525f, 1207.819092f, 130.424271f, 4.949865f},
        {NPC_DEVOURING_GHOUL, 2343.593750f, 1207.915039f, 130.781311f, 4.321547f},
        {NPC_DARK_NECROMANCER, 2348.257324f, 1212.202515f, 130.670135f, 4.450352f},
		{NPC_MASTER_NECROMANCER, 2353.65f, 1202.44f, 130.459f, 4.81385f},
		{NPC_BILE_GOLEM, 2356.52f, 1203.59f, 130.479f, 4.22543f},
        {0, 0, 0, 0, 0}},
    {
		// Wave 4
        {NPC_TOMB_STALKER, 2139.825195f, 1356.277100f, 132.199615f, 5.820131f},
        {NPC_DEVOURING_GHOUL, 2137.073486f, 1362.464844f, 132.271637f, 5.820131f},
        {NPC_DEVOURING_GHOUL, 2134.075684f, 1354.148071f, 131.885864f, 5.820131f},
        {NPC_DARK_NECROMANCER, 2133.302246f, 1358.907837f, 132.037689f, 5.820131f},
		{NPC_ACOLYTE, 2133.67f, 1349.54f, 131.558f, 6.24105f},
		{NPC_CRYPT_FIEND, 2137.99f, 1350.49f, 131.906f, 6.2586f},
        {0, 0, 0, 0, 0}},
    {
		// Wave 5
        {NPC_DARK_NECROMANCER, 2264.013428f, 1174.055908f, 138.093094f, 2.860481f},
        {NPC_DEVOURING_GHOUL, 2264.207764f, 1170.892700f, 138.034973f, 2.860481f},
        {NPC_DEVOURING_GHOUL, 2266.948975f, 1176.898926f, 137.976929f, 2.860481f},
        {NPC_TOMB_STALKER, 2269.215576f, 1170.109253f, 137.742691f, 2.860481f},
        {NPC_CRYPT_FIEND, 2273.106689f, 1176.101074f, 137.880508f, 2.860481f},
		{NPC_MASTER_NECROMANCER, 2265.11f, 1180.55f, 138.185f, 3.08071f},
		{NPC_BILE_GOLEM, 2264.48f, 1164.75f, 137.918f, 2.79951f}},
    {
		// Wave 6
        {NPC_BILE_GOLEM, 2349.701660f, 1188.436646f, 130.428864f, 3.908642f},
        {NPC_DEVOURING_GHOUL, 2349.909180f, 1194.582642f, 130.417816f, 3.577001f},
        {NPC_ENRAGED_GHOUL, 2354.662598f, 1185.692017f, 130.552032f, 3.577001f},
        {NPC_ENRAGED_GHOUL, 2354.716797f, 1191.614380f, 130.539810f, 3.577001f},
		{NPC_PATCHWORK_CONSTRUCT, 2352.77f, 1181.1f, 130.565f, 3.5665f},
		{NPC_ACOLYTE, 2346.19f, 1192.64f, 130.518f, 3.83982f},
        {0, 0, 0, 0, 0}},
    {
		// Wave 7
        {NPC_PATCHWORK_CONSTRUCT, 2145.212891f, 1355.288086f, 132.288773f, 6.004838f},
        {NPC_DARK_NECROMANCER, 2137.078613f, 1357.612671f, 132.173340f, 6.004838f},
        {NPC_ENRAGED_GHOUL, 2139.402100f, 1352.541626f, 132.127518f, 5.812850f},
        {NPC_ENRAGED_GHOUL, 2142.408447f, 1360.760620f, 132.321564f, 5.812850f},
		{NPC_CRYPT_FIEND, 2146.14f, 1347.75f, 132.157f, 6.25899f},
		{NPC_BILE_GOLEM, 2139.24f, 1347.46f, 131.788f, 0.0427246f},
        {0, 0, 0, 0, 0}},
    {
		// Wave 8
        {NPC_DEVOURING_GHOUL, 2172.686279f, 1259.618164f, 134.391754f, 1.865499f},
        {NPC_CRYPT_FIEND, 2177.649170f, 1256.061157f, 135.096512f, 1.849572f},
        {NPC_PATCHWORK_CONSTRUCT, 2170.782959f, 1253.594849f, 134.973022f, 1.849572f},
        {NPC_DARK_NECROMANCER, 2175.595703f, 1249.041992f, 135.603531f, 1.849572f},
		{NPC_ACOLYTE, 2167.68f, 1258.71f, 134.262f, 1.67714f},
		{NPC_ACOLYTE, 2178.1f, 1260.93f, 134.51f, 1.7024f},
		{NPC_MASTER_NECROMANCER, 2169.23f, 1248.61f, 135.596f, 1.71967f},
    }
};

// Locations for rifts to spawn and draconians to go
float RiftAndSpawnsLocations[ENCOUNTER_CHRONO_SPAWNS][5]=
{
    {NPC_TIME_RIFT, 2431.790039f, 1190.670044f, 148.076004f, 0.187923f},
    {NPC_INFINITE_ADVERSARY, 2433.857910f, 1185.612061f, 148.075974f, 4.566168f},
    {NPC_INFINITE_ADVERSARY, 2437.577881f, 1188.241089f, 148.075974f, 0.196999f},
    {NPC_INFINITE_AGENT, 2437.165527f, 1192.294922f, 148.075974f, 0.169247f},
    {NPC_INFINITE_HUNTER, 2434.989990f, 1197.679565f, 148.075974f, 0.715971f},
    {NPC_TIME_RIFT, 2403.954834f, 1178.815430f, 148.075943f, 4.966126f},
    {NPC_INFINITE_AGENT, 2403.676758f, 1171.495850f, 148.075607f, 4.902797f},
    {NPC_INFINITE_HUNTER, 2407.691162f, 1172.162720f, 148.075607f, 4.963010f},
    {NPC_TIME_RIFT, 2414.217041f, 1133.446167f, 148.076050f, 1.706972f},
    {NPC_INFINITE_ADVERSARY, 2416.024658f, 1139.456177f, 148.076431f, 1.752129f},
    {NPC_INFINITE_HUNTER, 2410.866699f, 1139.680542f, 148.076431f, 1.752129f},
    {NPC_TIME_RIFT, 2433.081543f, 1099.869751f, 148.076157f, 1.809509f},
    {NPC_INFINITE_ADVERSARY, 2426.947998f, 1107.471680f, 148.076019f, 1.877580f},
    {NPC_INFINITE_HUNTER, 2432.944580f, 1108.896362f, 148.208160f, 2.199241f},
    {NPC_TIME_RIFT, 2444.077637f, 1114.366089f, 148.076157f, 3.049565f},
    {NPC_INFINITE_ADVERSARY, 2438.190674f, 1118.368164f, 148.076172f, 3.139232f},
    {NPC_INFINITE_AGENT, 2435.861328f, 1113.402954f, 148.169327f, 2.390271f},
    {NPC_TIME_RIFT, 2463.131592f, 1115.391724f, 152.473129f, 3.409651f},
    {NPC_EPOCH_HUNTER, 2451.809326f, 1112.901245f, 149.220459f, 3.363617f},
};

class npc_arthas : public CreatureScript
{
public:
    npc_arthas() : CreatureScript("npc_arthas") {}

	bool OnGossipHello(Player* player, Creature* creature)
    {
        npc_arthasAI* AI = CAST_AI(npc_arthasAI, creature->AI());

        if (AI && AI->bStepping == false)
        {
            switch (AI->GossipStep)
            {
                case 0:
				{
					QuestStatus status = player->GetQuestStatus(QUEST_DISPELLING_ILLUSIONS);
					if (status != QUEST_STATUS_COMPLETE && status != QUEST_STATUS_REWARDED)
						return false;
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_ARTHAS_START_STEP_1), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
                    player->SEND_GOSSIP_MENU(907, creature->GetGUID());
                    break;
				}
                case 1:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_ARTHAS_START_STEP_2), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                    player->SEND_GOSSIP_MENU(13076, creature->GetGUID());
                    break;
                case 2:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_ARTHAS_START_STEP_3), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                    player->SEND_GOSSIP_MENU(13125, creature->GetGUID());
                    break;
                case 3:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_ARTHAS_START_STEP_4), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
                    player->SEND_GOSSIP_MENU(13177, creature->GetGUID());
                    break;
                case 4:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_ARTHAS_START_STEP_5), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
                    player->SEND_GOSSIP_MENU(13179, creature->GetGUID());
                    break;
                case 5:
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sObjectMgr->GetQuantumSystemTextForDBCLocale(TEXT_ID_ARTHAS_START_STEP_6), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
                    player->SEND_GOSSIP_MENU(13287, creature->GetGUID());
                    break;
                default:
                    return false;
            }
        }
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        npc_arthasAI* ai = CAST_AI(npc_arthasAI, creature->AI());

        if (!ai)
            return false;

        player->PlayerTalkClass->ClearMenus();

        switch (action)
        {
            case GOSSIP_ACTION_INFO_DEF:
                ai->Start(true, true, player->GetGUID(), 0, false, false);
                ai->SetDespawnAtEnd(false);
                ai->bStepping = false;
                ai->Step = 1;
                break;
            case GOSSIP_ACTION_INFO_DEF+1:
                ai->bStepping = true;
                ai->Step = 24;
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                ai->SetHoldState(false);
                ai->bStepping = false;
                ai->Step = 61;
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                ai->SetHoldState(false);
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                ai->bStepping = true;
                ai->Step = 84;
                break;
            case GOSSIP_ACTION_INFO_DEF+5:
                ai->bStepping = true;
                ai->Step = 85;
                break;
        }
        player->CLOSE_GOSSIP_MENU();
        ai->SetDespawnAtFar(false);
        creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        return true;
    }

    struct npc_arthasAI : public npc_escortAI
    {
        npc_arthasAI(Creature* creature) : npc_escortAI(creature)
        {
            instance = creature->GetInstanceScript();
            Reset();
        }

        InstanceScript* instance;

        bool bStepping;
        uint32 Step;
        uint32 PhaseTimer;
        uint32 GossipStep;
        uint32 PlayerFaction;
        uint32 BossEvent;
        uint32 Wave;
        uint32 WaveWorldState;

        uint64 UtherGUID;
        uint64 JainaGUID;
        uint64 CityManGUID[2];
        uint64 WaveGUID[ENCOUNTER_WAVES_MAX_SPAWNS];
        uint64 InfiniteDraconianGUID[ENCOUNTER_DRACONIAN_NUMBER];
        uint64 StalkerGUID;

        uint64 BossGUID; //MeathookGUID || SalrammGUID
        uint64 EpochHunterGUID;
        uint64 MalganisGUID;
        uint64 InfiniteTheCorruptorGUID;

        uint32 ExorcismTimer;

        void Reset()
        {
            UtherGUID = 0;
            JainaGUID = 0;

            for (uint8 i = 0; i < 2; ++i)
                CityManGUID[i] = 0;

            for (uint8 i = 0; i < ENCOUNTER_WAVES_MAX_SPAWNS; ++i)
                WaveGUID[i] = 0;

            for (uint8 i = 0; i < ENCOUNTER_DRACONIAN_NUMBER; ++i)
                InfiniteDraconianGUID[i] = 0;

            StalkerGUID              = 0;
            BossGUID                 = 0;
            EpochHunterGUID          = 0;
            MalganisGUID             = 0;
            InfiniteTheCorruptorGUID = 0;

			DoCast(me, SPELL_UNIT_DETECTION_ALL, true);
			DoCast(me, SPELL_DEVOTION_AURA, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
			
			if (instance->GetData(DATA_ARTHAS_EVENT) != DONE)
				instance->SetData(DATA_ARTHAS_EVENT, NOT_STARTED);

			switch (instance->GetData(DATA_ARTHAS_EVENT))
			{
                case NOT_STARTED:
					bStepping = true;
					Step = 0;
					me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
					BossEvent = DATA_MEATHOOK_EVENT;
					GossipStep = 0;
					break;
			}

			PhaseTimer = 1000;
			ExorcismTimer = 7300;
			WaveWorldState = 0;
			Wave = 0;
        }

        void EnterToBattle(Unit* /*who*/){}

		void DamageTaken(Unit* /*attacker*/, uint32 &amount)
        {
            if (amount >= me->GetHealth())
            {
                amount = 0;
				me->SetFullHealth();
            }
        }

        void SpawnTimeRift(uint32 timeRiftID, uint64* guidVector)
        {
            me->SummonCreature((uint32)RiftAndSpawnsLocations[timeRiftID][0], RiftAndSpawnsLocations[timeRiftID][1], RiftAndSpawnsLocations[timeRiftID][2], RiftAndSpawnsLocations[timeRiftID][3], RiftAndSpawnsLocations[timeRiftID][4], TEMPSUMMON_TIMED_DESPAWN, 11*IN_MILLISECONDS);

            for (uint32 i = timeRiftID+1; i < ENCOUNTER_CHRONO_SPAWNS; ++i)
            {
                if ((uint32)RiftAndSpawnsLocations[i][0] == NPC_TIME_RIFT)
					break;

                if (Creature* creature = me->SummonCreature((uint32)RiftAndSpawnsLocations[i][0], RiftAndSpawnsLocations[timeRiftID][1], RiftAndSpawnsLocations[timeRiftID][2], RiftAndSpawnsLocations[timeRiftID][3], RiftAndSpawnsLocations[timeRiftID][4], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*MINUTE*IN_MILLISECONDS))
                {
                    guidVector[i-timeRiftID-1] = creature->GetGUID();
                    creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                    creature->SetReactState(REACT_PASSIVE);
                    creature->GetMotionMaster()->MovePoint(0, RiftAndSpawnsLocations[i][1], RiftAndSpawnsLocations[i][2], RiftAndSpawnsLocations[i][3]);
                    if ((uint32)RiftAndSpawnsLocations[i][0] == NPC_EPOCH_HUNTER)
                        EpochHunterGUID = creature->GetGUID();
                }
            }
        }

        void SpawnWaveGroup(uint32 waveID, uint64* guidVector)
        {
            for (uint32 i = 0; i < ENCOUNTER_WAVES_MAX_SPAWNS; ++i)
            {
                if ((uint32)WavesLocations[waveID][i][0] == 0)
					break;

                if (Creature* creature = me->SummonCreature((uint32)WavesLocations[waveID][i][0], WavesLocations[waveID][i][1], WavesLocations[waveID][i][2], WavesLocations[waveID][i][3], WavesLocations[waveID][i][4], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*MINUTE*IN_MILLISECONDS))
                    guidVector[i] = creature->GetGUID();
            }
        }

        void SetHoldState(bool bOnHold)
        {
            SetEscortPaused(bOnHold);
        }

        void JumpToNextStep(uint32 Timer)
        {
            PhaseTimer = Timer;
            ++Step;
        }

        void WaypointReached(uint32 id)
        {
            switch (id)
            {
                case 0:
                case 1:
                case 3:
                case 9:
                case 10:
                case 11:
                case 22:
                case 23:
                case 26:
                case 55:
                case 56:
                    SetHoldState(true);
                    bStepping = true;
                    break;
                case 7:
                    if (Unit* cityman0 = me->SummonCreature(NPC_CITY_MAN, 2091.977f, 1275.021f, 140.757f, 0.558f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1*MINUTE*IN_MILLISECONDS))
                        CityManGUID[0] = cityman0->GetGUID();
                    if (Unit* cityman1 = me->SummonCreature(NPC_CITY_MAN2, 2093.514f, 1275.842f, 140.408f, 3.801f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1*MINUTE*IN_MILLISECONDS))
                        CityManGUID[1] = cityman1->GetGUID();
                    break;
                case 8:
                    GossipStep = 1;
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    SetHoldState(true);
                    break;
                case 12:
                    SetRun(true);
                    DoSendQuantumText(SAY_PHASE210, me);
                    if (Unit* pDisguised0 = me->SummonCreature(NPC_CITY_MAN3, 2398.14f, 1207.81f, 134.04f, 5.155249f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3*MINUTE*IN_MILLISECONDS))
                    {
                        InfiniteDraconianGUID[0] = pDisguised0->GetGUID();
                        if (Unit* pDisguised1 = me->SummonCreature(NPC_CITY_MAN4, 2403.22f, 1205.54f, 134.04f, 3.311264f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3*MINUTE*IN_MILLISECONDS))
                        {
                            InfiniteDraconianGUID[1] = pDisguised1->GetGUID();

                            if (Unit* pDisguised2 = me->SummonCreature(NPC_CITY_MAN, 2400.82f, 1201.69f, 134.01f, 1.534082f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3*MINUTE*IN_MILLISECONDS))
                            {
                                InfiniteDraconianGUID[2] = pDisguised2->GetGUID();
                                pDisguised0->SetTarget(InfiniteDraconianGUID[1]);
                                pDisguised1->SetTarget(InfiniteDraconianGUID[0]);
                                pDisguised2->SetTarget(InfiniteDraconianGUID[1]);
                            }
                        }
                    }
                    break;
                case 20:
                    GossipStep = 2;
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    SetRun(false);
                    SetHoldState(true);
                    break;
                case 21:
                    DoSendQuantumText(SAY_PHASE301, me);
                    break;
                case 24:
                    SetRun(false);
                    SpawnTimeRift(0,&InfiniteDraconianGUID[0]);
                    DoSendQuantumText(SAY_PHASE307 ,me);
                    break;
                case 29:
                    SetRun(false);
                    SpawnTimeRift(5, &InfiniteDraconianGUID[0]);
                    SpawnTimeRift(8, &InfiniteDraconianGUID[2]);
                    DoSendQuantumText(SAY_PHASE309, me);
                    SetHoldState(true);
                    bStepping = true;
                    break;
                case 31:
                    SetRun(false);
                    SpawnTimeRift(11, &InfiniteDraconianGUID[0]);
                    SpawnTimeRift(14, &InfiniteDraconianGUID[2]);
                    DoSendQuantumText(SAY_PHASE311, me);
                    SetHoldState(true);
                    bStepping = true;
                    break;
                case 32:
                    DoSendQuantumText(SAY_PHASE401, me);
                    break;
                case 34:
                    DoSendQuantumText(SAY_PHASE402, me);
                    break;
                case 35:
                    DoSendQuantumText(SAY_PHASE403, me);
                    break;
                case 36:
					if (GameObject* gate = instance->instance->GetGameObject(instance->GetData64(DATA_SHKAF_GATE)))
						gate->SetGoState(GO_STATE_ACTIVE);
                    break;
                case 45:
                    SetRun(true);
                    //SetDespawnAtFar(false);
                    GossipStep = 4;
                    me->SetReactState(REACT_DEFENSIVE);
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    SetHoldState(true);
                    break;
                case 47:
                    SetRun(false);
                    me->SetReactState(REACT_AGGRESSIVE);
                    DoSendQuantumText(SAY_PHASE405, me);
                    break;
                case 48:
                    SetRun(true);
                    DoSendQuantumText(SAY_PHASE406, me);
					if (instance->GetData(DATA_INFINITE_EVENT) == SPECIAL && IsHeroic())
						instance->SetData(DATA_INFINITE_EVENT, IN_PROGRESS); //make visible
                    break;
                case 53:
                    DoSendQuantumText(SAY_PHASE407, me);
                    break;
                case 54:
                    //SetDespawnAtFar(false);
                    GossipStep = 5;
                    me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    SetHoldState(true);
                    break;
             }
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (bStepping)
            {
                if (PhaseTimer <= diff)
                {
                    switch (Step)
                    {
                        //After reset
                        case 0:
                            if (Unit* jaina = GetClosestCreatureWithEntry(me, NPC_JAINA, 50.0f))
                                JainaGUID = jaina->GetGUID();
                            else if (Unit* jaina = me->SummonCreature(NPC_JAINA,1895.48f,1292.66f,143.706f,0.023475f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3*MINUTE*IN_MILLISECONDS))
                                JainaGUID = jaina->GetGUID();
                            bStepping = false;
                            JumpToNextStep(0);
                            break;
                        //After waypoint 0
                        case 1:
                            me->SetWalk(false);
                            if (Unit* uther = me->SummonCreature(NPC_UTHER,1794.357f,1272.183f,140.558f,1.37f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3*MINUTE*IN_MILLISECONDS))
                            {
                                UtherGUID = uther->GetGUID();
                                uther->SetWalk(false);
                                uther->GetMotionMaster()->MovePoint(0, 1897.018f, 1287.487f, 143.481f);
                                uther->SetTarget(me->GetGUID());
                                me->SetTarget(UtherGUID);
                            }
                            JumpToNextStep(17000);
                            break;
                        case 2:
                            DoSendQuantumText(SAY_PHASE101, me);
                            JumpToNextStep(2000);
                            break;
                        case 3:
                            if (Creature* uther = Unit::GetCreature(*me, UtherGUID))
                                DoSendQuantumText(SAY_PHASE102, uther);
                            JumpToNextStep(8000);
                            break;
                        case 4:
                            SetEscortPaused(false);
                            bStepping = false;
                            SetRun(false);
                            DoSendQuantumText(SAY_PHASE103, me);
                            JumpToNextStep(0);
                            break;
                        //After waypoint 1
                        case 5:
                            if (Creature* jaina = Unit::GetCreature(*me, JainaGUID))
                                jaina->SetTarget(me->GetGUID());
                            DoSendQuantumText(SAY_PHASE104, me);
                            JumpToNextStep(10000);
                            break;
                        case 6:
                            if (Creature* uther = Unit::GetCreature(*me, UtherGUID))
                                DoSendQuantumText(SAY_PHASE105, uther);
                            JumpToNextStep(1000);
                            break;
                        case 7:
                            DoSendQuantumText(SAY_PHASE106, me);
                            JumpToNextStep(4000);
                            break;
                        case 8:
                            if (Creature* uther = Unit::GetCreature(*me, UtherGUID))
                                DoSendQuantumText(SAY_PHASE107, uther);
                            JumpToNextStep(6000);
                            break;
                        case 9:
                            DoSendQuantumText(SAY_PHASE108, me);
                            JumpToNextStep(4000);
                            break;
                        case 10:
                            if (Creature* uther = Unit::GetCreature(*me, UtherGUID))
                                DoSendQuantumText(SAY_PHASE109, uther);
                            JumpToNextStep(8000);
                            break;
                        case 11:
                            DoSendQuantumText(SAY_PHASE110, me);
                            JumpToNextStep(4000);
                            break;
                        case 12:
                            if (Creature* uther = Unit::GetCreature(*me, UtherGUID))
                                DoSendQuantumText(SAY_PHASE111, uther);
                            JumpToNextStep(4000);
                            break;
                        case 13:
                            DoSendQuantumText(SAY_PHASE112, me);
                            JumpToNextStep(11000);
                            break;
                        case 14:
                            if (Creature* jaina = Unit::GetCreature(*me, JainaGUID))
                                DoSendQuantumText(SAY_PHASE113, jaina);
                            JumpToNextStep(3000);
                            break;
                        case 15:
                            DoSendQuantumText(SAY_PHASE114, me);
                            JumpToNextStep(9000);
                            break;
                        case 16:
                            if (Creature* uther = Unit::GetCreature(*me, UtherGUID))
                                DoSendQuantumText(SAY_PHASE115, uther);
                            JumpToNextStep(4000);
                            break;
                        case 17:
                            if (Creature* uther = Unit::GetCreature(*me, UtherGUID))
                            {
                                uther->SetWalk(true);
                                uther->GetMotionMaster()->MovePoint(0, 1794.357f,1272.183f,140.558f);
                            }
                            JumpToNextStep(1000);
                            break;
                        case 18:
                            if (Creature* jaina = Unit::GetCreature(*me, JainaGUID))
                            {
                                me->SetTarget(JainaGUID);
                                jaina->SetWalk(true);
                                jaina->GetMotionMaster()->MovePoint(0, 1794.357f,1272.183f,140.558f);
                            }
                            JumpToNextStep(1000);
                            break;
                        case 19:
                            DoSendQuantumText(SAY_PHASE116, me);
                            JumpToNextStep(1000);
                            break;
                        case 20:
                            if (Creature* jaina = Unit::GetCreature(*me, JainaGUID))
                                DoSendQuantumText(SAY_PHASE117, jaina);
                            JumpToNextStep(3000);
                            break;
                        case 21:
                            SetEscortPaused(false);
                            bStepping = false;
                            me->SetTarget(0);
                            JumpToNextStep(0);
                            break;
                        //After waypoint 3
                        case 22:
                            DoSendQuantumText(SAY_PHASE118, me);
                            me->SetTarget(JainaGUID);
                            JumpToNextStep(10000);
                            break;
                        case 23:
                            SetEscortPaused(false);
                            bStepping = false;
                            SetRun(true);

                            if (Creature* jaina = Unit::GetCreature(*me, JainaGUID))
                                jaina->DisappearAndDie();

                            if (Creature* uther = Unit::GetCreature(*me, UtherGUID))
                                uther->DisappearAndDie();

                            me->SetTarget(0);
                            JumpToNextStep(0);
                            break;
                        //After Gossip 1 (waypoint 8)
                        case 24:
                            if (Unit* stalker = me->SummonCreature(NPC_INVIS_TARGET, 2026.469f,1287.088f, 143.596f, 1.37f, TEMPSUMMON_TIMED_DESPAWN, 14*IN_MILLISECONDS))
                            {
                                StalkerGUID = stalker->GetGUID();
                                me->SetTarget(StalkerGUID);
                            }
                            JumpToNextStep(1000);
                            break;
                        case 25:
                            DoSendQuantumText(SAY_PHASE201, me);
                            JumpToNextStep(12000);
                            break;
                        case 26:
                            SetEscortPaused(false);
                            bStepping = false;
                            SetRun(false);
                            me->SetTarget(0);
                            JumpToNextStep(0);
                            break;
                        //After waypoint 9
                        case 27:
							me->SetTarget(CityManGUID[0]);
							if (Creature* cityman = Unit::GetCreature(*me, CityManGUID[0]))
                            {
                                cityman->SetTarget(me->GetGUID());
                                cityman->SetWalk(true);
                                cityman->GetMotionMaster()->MovePoint(0, 2088.625f,1279.191f,140.743f);
                            }
                            JumpToNextStep(2000);
                            break;
                        case 28:
                            if (Creature* cityman = Unit::GetCreature(*me, CityManGUID[0]))
                                DoSendQuantumText(SAY_PHASE202, cityman);
                            JumpToNextStep(4000);
                            break;
                        case 29:
                            SetEscortPaused(false);
                            bStepping = false;
                            DoSendQuantumText(SAY_PHASE203, me);
                            JumpToNextStep(0);
                            break;
                        //After waypoint 10
                        case 30:
                            me->HandleEmoteCommand(37);
                            JumpToNextStep(1000);
                            break;
                        case 31:
                            SetEscortPaused(false);
                            bStepping = false;
                            if (Creature* cityman1 = Unit::GetCreature(*me, CityManGUID[1]))
                            {
                                DoSendQuantumText(SAY_PHASE204, cityman1);
                                cityman1->SetTarget(me->GetGUID());
                                if (Creature* cityman0 = Unit::GetCreature(*me, CityManGUID[0]))
                                    cityman0->Kill(cityman0);
                                me->SetTarget(CityManGUID[1]);
                            }
                            JumpToNextStep(0);
                            break;
                        //After waypoint 11
                        case 32:
                            me->HandleEmoteCommand(37);
                            JumpToNextStep(1000);
                            break;
                        case 33:
                            if (Creature* cityman1 = Unit::GetCreature(*me, CityManGUID[1]))
                                cityman1->Kill(cityman1);
                            JumpToNextStep(1000);
                            break;
                        case 34:
                            if (Unit* stalker = me->SummonCreature(NPC_INVIS_TARGET, 2081.447f, 1287.770f, 141.3241f, 1.37f, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
                            {
                                StalkerGUID = stalker->GetGUID();
                                me->SetTarget(StalkerGUID);
                            }
                            DoSendQuantumText(SAY_PHASE205, me);
                            JumpToNextStep(3000);
                            break;
                        case 35:
                            if (Unit* stalkerM = me->SummonCreature(NPC_INVIS_TARGET, 2117.349f, 1288.624f, 136.271f, 1.37f, TEMPSUMMON_TIMED_DESPAWN, 1*MINUTE*IN_MILLISECONDS))
                            {
                                StalkerGUID = stalkerM->GetGUID();
                                me->SetTarget(StalkerGUID);
                            }
                            me->SetReactState(REACT_DEFENSIVE);
                            JumpToNextStep(1000);
                            break;
                        case 36:
                            if (Creature* malganis = me->SummonCreature(NPC_MAL_GANIS, 2117.349f, 1288.624f, 136.271f, 1.37f, TEMPSUMMON_TIMED_DESPAWN, 1*MINUTE*IN_MILLISECONDS))
                            {
                                if (Creature* stalkerM = Unit::GetCreature(*me, StalkerGUID))
                                    malganis->CastSpell(stalkerM, 63793, false);

                                MalganisGUID = malganis->GetGUID();
                                DoSendQuantumText(SAY_PHASE206, malganis);
                                malganis->SetTarget(me->GetGUID());
                                malganis->SetReactState(REACT_PASSIVE);
								me->SetFacingToObject(malganis);
                            }
                            JumpToNextStep(11000);
                            break;
                        case 37:
                            if (Creature* malganis = Unit::GetCreature(*me, MalganisGUID))
                            {
                                if (Creature* cityMan = GetClosestCreatureWithEntry(malganis, NPC_CITY_MAN, 100.0f))
                                {
                                    cityMan->UpdateEntry(NPC_ZOMBIE, 0);
                                    cityMan->AIM_Initialize();
                                    cityMan->Motion_Initialize();
                                }
                                else if (Creature* cityMan2 = GetClosestCreatureWithEntry(malganis, NPC_CITY_MAN2, 100.0f))
                                {
                                    cityMan2->UpdateEntry(NPC_ZOMBIE, 0);
                                    cityMan2->AIM_Initialize();
                                    cityMan2->Motion_Initialize();
                                }
                                else // There's no one else to transform
                                    Step++;
                            }
                            else
                                Step++;
                            PhaseTimer = 250;
                            break;
                        case 38:
                            if (Creature* malganis = Unit::GetCreature(*me, MalganisGUID))
                                DoSendQuantumText(SAY_PHASE207, malganis);
                            JumpToNextStep(17000);
                            break;
                        case 39:
                            if (Creature* malganis = Unit::GetCreature(*me, MalganisGUID))
                                malganis->SetVisible(false);
                            DoSendQuantumText(SAY_PHASE208, me);
                            JumpToNextStep(7000);
                            break;
                        case 40:
                            if (Unit* stalker = me->SummonCreature(NPC_INVIS_TARGET, 2081.447f,1287.770f, 141.3241f, 1.37f, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
                            {
                                StalkerGUID = stalker->GetGUID();
                                me->SetTarget(StalkerGUID);
                            }
                            DoSendQuantumText(SAY_PHASE209, me);

                            BossEvent = DATA_MEATHOOK_EVENT;

                            if (instance->GetData(DATA_INFINITE_EVENT) != DONE && IsHeroic())   //if not killed already
                                instance->SetData(DATA_INFINITE_EVENT, SPECIAL);                //start countdown

                            //SetDespawnAtFar(false);
                            JumpToNextStep(5000);
                            break;
                        case 41: //Summon wave group
                        case 43:
                        case 45:
                        case 47:
                        case 51:
                        case 53:
                        case 55:
                        case 57:
                            if (instance->GetData(BossEvent) != DONE)
                            {
                                SpawnWaveGroup(Wave, WaveGUID);
                                Wave++;
                                WaveWorldState++;
                                instance->DoUpdateWorldState(WORLDSTATE_NUMBER_SCOURGE_WAVES_SHOW_COUNT, WaveWorldState);
                            }
                            JumpToNextStep(500);
                            break;
                        case 42: //Wait group to die
                        case 44:
                        case 46:
                        case 48:
                        case 52:
                        case 54:
                        case 56:
                        case 58:
                            if (instance->GetData(BossEvent) != DONE)
                            {
                                uint32 mobCounter = 0;
                                uint32 deadCounter = 0;
                                for (uint8 i = 0; i < ENCOUNTER_WAVES_MAX_SPAWNS; ++i)
                                {
                                    if (WaveGUID[i] == 0)
                                        break;
                                    ++mobCounter;
                                    Unit* creature = Unit::GetCreature(*me, WaveGUID[i]);
                                    if (!creature || creature->IsDead())
                                        ++deadCounter;
                                }

                                if (mobCounter <= deadCounter) //If group is dead
                                    JumpToNextStep(1000);
                                else
                                    PhaseTimer = 1000;
                            }
                            else
                                JumpToNextStep(500);
                            break;
                        case 49: //Summon Boss
                        case 59:
                            if (instance->GetData(BossEvent) != DONE)
                            {
                                uint32 BossID = 0;
                                if (BossEvent == DATA_MEATHOOK_EVENT)
                                    BossID = NPC_MEATHOOK;
                                else if (BossEvent == DATA_SALRAMM_EVENT)
                                    BossID = NPC_SALRAMM;

                                if (Unit* boss = me->SummonCreature(BossID, 2232.19f, 1331.933f, 126.662f, 3.15f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*MINUTE*IN_MILLISECONDS))
                                {
                                    BossGUID = boss->GetGUID();
                                    boss->SetWalk(true);
                                    boss->GetMotionMaster()->MovePoint(0, 2194.110f,1332.00f,130.00f);
                                }
                                WaveWorldState++;
                                instance->DoUpdateWorldState(WORLDSTATE_NUMBER_SCOURGE_WAVES_SHOW_COUNT, WaveWorldState);
                            }
                            JumpToNextStep(30000);
                            break;
                        case 50: //Wait Boss death
                        case 60:
							if (instance->GetData(BossEvent) == DONE)
							{
								JumpToNextStep(1000);

								if (BossEvent == DATA_MEATHOOK_EVENT)
									BossEvent = DATA_SALRAMM_EVENT;

								else if (BossEvent == DATA_SALRAMM_EVENT)
								{
									WaveWorldState = 0;
									SetHoldState(false);
									bStepping = false;
									BossEvent = DATA_EPOCH_EVENT;
								}
								instance->DoUpdateWorldState(WORLDSTATE_NUMBER_SCOURGE_WAVES_SHOW_COUNT, WaveWorldState);
							}
							else if (instance->GetData(BossEvent) == FAIL)
							{
								npc_escortAI::EnterEvadeMode();
								WaveWorldState = 0;
								instance->DoUpdateWorldState(WORLDSTATE_NUMBER_SCOURGE_WAVES_SHOW_COUNT, WaveWorldState);
							}
							else PhaseTimer = 10000;
							break;
                        //After Gossip 2 (waypoint 22)
                        case 61:
                            me->SetReactState(REACT_AGGRESSIVE);
                            if (Creature* pDisguised0 = Unit::GetCreature(*me, InfiniteDraconianGUID[0]))
                                pDisguised0->SetTarget(me->GetGUID());

                            if (Creature* pDisguised1 = Unit::GetCreature(*me, InfiniteDraconianGUID[1]))
                                pDisguised1->SetTarget(me->GetGUID());

                            if (Creature* pDisguised2 = Unit::GetCreature(*me, InfiniteDraconianGUID[2]))
                                pDisguised2->SetTarget(me->GetGUID());
                            JumpToNextStep(1000);
                            break;
                        case 62:
                            if (Creature* pDisguised0 = Unit::GetCreature(*me, InfiniteDraconianGUID[0]))
                                DoSendQuantumText(SAY_PHASE302, pDisguised0);
                            JumpToNextStep(7000);
                            break;
                        case 63:
                            DoSendQuantumText(SAY_PHASE303, me);
                            SetHoldState(false);
                            bStepping = false;
                            JumpToNextStep(0);
                            break;
                        //After waypoint 23
                        case 64:
                            me->HandleEmoteCommand(54);
                            JumpToNextStep(1000);
                            break;
                        case 65:
                            if (Creature* pDisguised0 = Unit::GetCreature(*me, InfiniteDraconianGUID[0]))
                                pDisguised0->HandleEmoteCommand(11);
                            JumpToNextStep(1000);
                            break;
                        case 66:
                            DoSendQuantumText(SAY_PHASE304,me);
                            JumpToNextStep(2000);
                            break;
                        case 67:
                            if (Creature* pDisguised0 = Unit::GetCreature(*me, InfiniteDraconianGUID[0]))
                                DoSendQuantumText(SAY_PHASE305,pDisguised0);
                            JumpToNextStep(1000);
                            break;
                        case 68:
                            if (Creature* pDisguised2 = Unit::GetCreature(*me, InfiniteDraconianGUID[2]))
                            {
                                pDisguised2->UpdateEntry(NPC_INFINITE_HUNTER, 0);
                                //Make them unattackable
                                pDisguised2->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                                pDisguised2->SetReactState(REACT_PASSIVE);
                            }
                            JumpToNextStep(2000);
                            break;
                        case 69:
                            if (Creature* pDisguised1 = Unit::GetCreature(*me, InfiniteDraconianGUID[1]))
                            {
                                pDisguised1->UpdateEntry(NPC_INFINITE_AGENT, 0);
                                //Make them unattackable
                                pDisguised1->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                                pDisguised1->SetReactState(REACT_PASSIVE);
                            }
                            JumpToNextStep(2000);
                            break;
                        case 70:
                            if (Creature* pDisguised0 = Unit::GetCreature(*me, InfiniteDraconianGUID[0]))
                            {
                                pDisguised0->UpdateEntry(NPC_INFINITE_ADVERSARY, 0);
                                //Make them unattackable
                                pDisguised0->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                                pDisguised0->SetReactState(REACT_PASSIVE);
                            }
                            JumpToNextStep(2000);
                            break;
                        case 71:
                        //After waypoint 26,29,31
                        case 73:
                        case 75:
                        case 77:
                            //Make creatures attackable
                            for (uint32 i = 0; i< ENCOUNTER_DRACONIAN_NUMBER; ++i)
							{
                                if (Creature* creature = Unit::GetCreature(*me, InfiniteDraconianGUID[i]))
                                {
                                    creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                                    creature->SetReactState(REACT_AGGRESSIVE);
                                }
							}
                            JumpToNextStep(5000);
                            break;
                        case 72:
                        case 74:
                        case 76:
                            if (me->IsInCombatActive())
                                PhaseTimer = 1000;
                            else
                            {
								if (Step == 72)
									DoSendQuantumText(SAY_PHASE308, me);
                                if (Step == 74)
									DoSendQuantumText(SAY_PHASE308, me);
                                if (Step == 76)
									DoSendQuantumText(SAY_PHASE310, me);
                                SetHoldState(false);
                                bStepping = false;
                                SetRun(true);
                                JumpToNextStep(2000);
                            }
                            break;
                        case 78:
                            if (me->IsInCombatActive())
                                PhaseTimer = 1000;
                            else
                            {
								DoSendQuantumText(SAY_PHASE312,me);
                                JumpToNextStep(5000);
                            }
                            break;
                        case 79:
                            DoSendQuantumText(SAY_PHASE313,me);
                            JumpToNextStep(1000);
                            break;
                        case 80:
							if (instance->GetData(DATA_EPOCH_EVENT) != DONE)
							{
								SpawnTimeRift(17, &EpochHunterGUID);
								if (Creature* pEpoch = Unit::GetCreature(*me, EpochHunterGUID))
									DoSendQuantumText(SAY_PHASE314, pEpoch);
								me->SetTarget(EpochHunterGUID);
							}
                            JumpToNextStep(18000);
                            break;
                        case 81:
							if (instance->GetData(DATA_EPOCH_EVENT) != DONE)
								DoSendQuantumText(SAY_PHASE315, me);
                            JumpToNextStep(6000);
                            break;
                        case 82:
							if (instance->GetData(DATA_EPOCH_EVENT) != DONE)
							{
								if (Creature* hunter = Unit::GetCreature(*me, EpochHunterGUID))
								{
									hunter->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
									hunter->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
									hunter->SetReactState(REACT_AGGRESSIVE);
								}
							}
                            JumpToNextStep(1000);
                            break;
                        case 83:
							if (instance->GetData(DATA_EPOCH_EVENT) == DONE)
							{
								GossipStep = 3;
								me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
								bStepping = false;
								BossEvent = DATA_MAL_GANIS_EVENT;
								JumpToNextStep(15000);
							}
							else if (instance->GetData(DATA_EPOCH_EVENT) == FAIL)
								npc_escortAI::EnterEvadeMode();
							else
								PhaseTimer = 10000;
                            break;
                        //After Gossip 4
                        case 84:
                            DoSendQuantumText(SAY_PHASE404,me);
                            //SetDespawnAtFar(false);
                            SetHoldState(false);
                            bStepping = false;
                            break;
                        //After Gossip 5
                        case 85:
                            DoSendQuantumText(SAY_PHASE501, me);
                            if (Creature* malganis = me->SummonCreature(NPC_MAL_GANIS, 2296.665f, 1502.362f, 128.362f, 4.961f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*MINUTE*IN_MILLISECONDS))
                            {
                                MalganisGUID = malganis->GetGUID();
                                malganis->SetReactState(REACT_PASSIVE);
                            }

							if (GameObject* gate = instance->instance->GetGameObject(instance->GetData64(DATA_MAL_GANIS_GATE_1)))
								gate->SetGoState(GO_STATE_ACTIVE);

                            SetHoldState(false);
                            bStepping = false;
                            JumpToNextStep(0);
                            break;
                        //After waypoint 55
                        case 86:
                            DoSendQuantumText(SAY_PHASE502, me);
                            JumpToNextStep(6000);
                            me->SetTarget(MalganisGUID);
                            break;
                        case 87:
                            if (Creature* malganis = Unit::GetCreature(*me, MalganisGUID))
                            {
                                malganis->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_UNK_6 | UNIT_FLAG_NO_AGGRO_FOR_CREATURE | UNIT_FLAG_SWIMMING);
                                malganis->SetReactState(REACT_AGGRESSIVE);
                            }
                            JumpToNextStep(1000);
                            break;
                        case 88:
							if (instance->GetData(DATA_MAL_GANIS_EVENT) == DONE)
							{
								SetHoldState(false);
								JumpToNextStep(1000);
							}
							else if (instance->GetData(DATA_MAL_GANIS_EVENT) == FAIL)
								npc_escortAI::EnterEvadeMode();
							else
								PhaseTimer = 10000;
                            break;
                        //After waypoint 56
                        case 89:
                            SetRun(true);
                            me->SetTarget(MalganisGUID);
                            DoSendQuantumText(SAY_PHASE503, me);
                            JumpToNextStep(7000);
                            break;
                        case 90:
							instance->SetData(DATA_ARTHAS_EVENT, DONE); //Rewards: Achiev & Chest ;D
							me->SetTarget(instance->GetData64(DATA_MAL_GANIS_GATE_2)); //Look behind
                            DoSendQuantumText(SAY_PHASE504, me);
                            bStepping = false;
                            break;
                    }
                }
                else PhaseTimer -= diff;
            }

            if (!me->GetVictim())
                return;

            if (ExorcismTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_EXORCISM_5N, SPELL_EXORCISM_5H), true);
					ExorcismTimer = 4000;
				}
            }
			else ExorcismTimer -= diff;

            if (me->HealthBelowPct(HEALTH_PERCENT_40))
                DoCast(me, SPELL_HOLY_LIGHT);

			DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_arthasAI (creature);
    }
};

#define GOSSIP_ITEM_DISUPTOR_BACK    "I need a Arcane Disruptor, Chromie."
#define GOSSIP_ITEM_CHROMIE_START_1  "Why have I been sent back to this panicular place and time?"
#define GOSSIP_ITEM_CHROMIE_START_2  "What was this decision?"
#define GOSSIP_ITEM_CHROMIE_START_3  "So how does the Infinite Dragonflight plan to interfere?"
#define GOSSIP_ITEM_CHROMIE_MIDDLE_1 "What do you think they're up to?"
#define GOSSIP_ITEM_CHROMIE_MIDDLE_2 "You want me to do what?"
#define GOSSIP_ITEM_CHROMIE_MIDDLE_3 "Very well, Chromie."

class npc_cos_chromie : public CreatureScript
{
public:
    npc_cos_chromie() : CreatureScript("npc_cos_chromie") {}

    bool OnGossipHello(Player* player, Creature* creature)
    {
        if (creature->IsQuestGiver())
            player->PrepareQuestMenu(creature->GetGUID());

        InstanceScript* instance = creature->GetInstanceScript();

        if (!instance)
            return true; //false?

        uint32 CratesEventData = instance->GetData(DATA_CRATES_EVENT);
        uint32 ArthasEventData = instance->GetData(DATA_ARTHAS_EVENT);

        if (CratesEventData == NOT_STARTED && creature->GetEntry() == NPC_CHROMIE_PART_1)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_CHROMIE_START_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

        if (CratesEventData == IN_PROGRESS && creature->GetEntry() == NPC_CHROMIE_PART_1 && !player->HasItemCount(ITEM_ID_ARCANE_DISRUPTOR, 1, true))
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DISUPTOR_BACK, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+7);

        if (CratesEventData == DONE && ArthasEventData == NOT_STARTED && creature->GetEntry() == NPC_CHROMIE_PART_2)
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_CHROMIE_MIDDLE_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);

        player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();

        switch(action)
        {
            case GOSSIP_ACTION_INFO_DEF+1:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_CHROMIE_START_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+2:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_CHROMIE_START_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+3:
                {
                    player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());

                    if (InstanceScript* instance = creature->GetInstanceScript())
                    {
                        uint32 CratesEventData = instance->GetData(DATA_CRATES_EVENT);

                        if (CratesEventData == NOT_STARTED)
                            instance->SetData(DATA_CRATES_EVENT, 2);
                    }
                }
                break;
            case GOSSIP_ACTION_INFO_DEF+7:
                {
                    player->CLOSE_GOSSIP_MENU();
                    ItemPosCountVec dest;
                    uint8 canStoreNewItem = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ITEM_ID_ARCANE_DISRUPTOR, 1);
                    if (canStoreNewItem == EQUIP_ERR_OK)
                    {
                        Item* newItem = NULL;
                        newItem = player->StoreNewItem(dest, ITEM_ID_ARCANE_DISRUPTOR, 1, true);
                        player->SendNewItem(newItem, 1, true, false);
                    }
                }
                break;
            case GOSSIP_ACTION_INFO_DEF+4:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_CHROMIE_MIDDLE_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+5:
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_CHROMIE_MIDDLE_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
                player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                break;
            case GOSSIP_ACTION_INFO_DEF+6:
                {
                    if (InstanceScript* instance = creature->GetInstanceScript())
                    {
                        instance->SetData(DATA_CRATES_EVENT, 0);

                        if (Creature* arthas = Creature::GetCreature(*creature, instance->GetData64(DATA_ARTHAS)))
                        {
                            npc_arthas::npc_arthasAI* ai = CAST_AI(npc_arthas::npc_arthasAI, arthas->AI());

                            if (ai)
                            {
                                if (instance->GetData(DATA_ARTHAS_EVENT) != NOT_STARTED)
                                    return false;

                                instance->SetData(DATA_ARTHAS_EVENT, IN_PROGRESS);
                                ai->Start(true,true,player->GetGUID(),0,false,false);
                                ai->SetDespawnAtFar(false);
                                ai->SetDespawnAtEnd(false);
                                ai->bStepping = false;
                                ai->Step = 1;
                            }
                        }
                    }
                    player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
                }
                break;

        }
        return true;
    }
};

class npc_crate_helper : public CreatureScript
{
public:
	npc_crate_helper() : CreatureScript("npc_create_helper_cot") { }

	struct npc_crate_helperAI : public NullCreatureAI
	{
		npc_crate_helperAI(Creature* creature) : NullCreatureAI(creature)
		{
			Marked = false;
		}

		bool Marked;

		void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
		{
			if (spell->Id == SPELL_ARCANE_DISRUPTION && !Marked)
			{
				Marked = true;

				if (InstanceScript* instance = me->GetInstanceScript())
					instance->SetData(DATA_CRATES_EVENT, 1);

				if (GameObject* crate = me->FindGameobjectWithDistance(GO_SUSPICIOUS_CRATE, 5.0f))
				{
					crate->SummonGameObject(GO_PLAGUED_CRATE, crate->GetPositionX(), crate->GetPositionY(), crate->GetPositionZ(), crate->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, DAY);
					crate->Delete();
				}
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_crate_helperAI(creature);
	}
};

void AddSC_culling_of_stratholme()
{
    new npc_arthas();
    new npc_cos_chromie();
    new npc_crate_helper();
}