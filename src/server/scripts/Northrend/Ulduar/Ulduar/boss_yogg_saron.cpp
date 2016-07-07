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
#include "SpellScript.h"
#include "Vehicle.h"
#include "ulduar.h"

enum Texts
{
    SAY_SARA_PREFIGHT_1                         = -1603310,
    SAY_SARA_PREFIGHT_2                         = -1603311,
    SAY_SARA_AGGRO_1                            = -1603312,
    SAY_SARA_AGGRO_2                            = -1603313,
    SAY_SARA_AGGRO_3                            = -1603314,
    SAY_SARA_SLAY_1                             = -1603315,
    SAY_SARA_SLAY_2                             = -1603316,
    WHISP_SARA_INSANITY                         = -1603317,
    SAY_SARA_PHASE2_1                           = -1603318,
    SAY_SARA_PHASE2_2                           = -1603319,
	// Yogg
    SAY_PHASE2_1                                = -1603330,
    SAY_PHASE2_2                                = -1603331,
    SAY_PHASE2_3                                = -1603332,
    SAY_PHASE2_4                                = -1603333,
    SAY_PHASE2_5                                = -1603334,
    SAY_PHASE3                                  = -1603335,
    SAY_VISION                                  = -1603336,
    SAY_LUNCATIC_GAZE                           = -1603337,
    SAY_DEAFENING_ROAR                          = -1603338,
    WHISP_INSANITY_1                            = -1603339,
    WHISP_INSANITY_2                            = -1603340,
    SAY_DEATH                                   = -1603341,
	// King Llane
    SAY_GARONA_1_VISION_1                       = -1603342,
    SAY_GARONA_2_VISION_1                       = -1603343,
    SAY_GARONA_3_VISION_1                       = -1603344,
    SAY_YOGGSARON_4_VISION_1                    = -1603345,
    SAY_YOGGSARON_5_VISION_1                    = -1603346,
    SAY_LLANE_6_VISION_1                        = -1603347,
    SAY_GARONA_7_VISION_1                       = -1603348,
    SAY_YOGGSARON_8_VISION_1                    = -1603349,
    // Lichking
    SAY_LICHKING_1_VISION_2                     = -1603350,
    SAY_CHAMP_2_VISION_2                        = -1603351,
    SAY_CHAMP_3_VISION_2                        = -1603352,
    SAY_LICHKING_4_VISION_2                     = -1603353,
    SAY_YOGGSARON_5_VISION_2                    = -1603354,
    SAY_YOGGSARON_6_VISION_2                    = -1603355,
    // Dragon Soul
    SAY_NELTHARION_1_VISION_3                   = -1603356,
    SAY_YSERA_2_VISION_3                        = -1603357,
    SAY_NELTHARION_3_VISION_3                   = -1603358,
    SAY_MALYGOS_4_VISION_3                      = -1603359,
    SAY_YOGGSARON_5_VISION_3                    = -1603360,
};

#define EMOTE_CLOUD_PLAYER_TOUCH "%s begins to boil upon touching %s!"

enum Events
{
    ACHIEV_TIMED_START_EVENT = 21001,
};

enum MindlessSpell
{
    BRAIN_LINK     = 0,
    MALADY_OF_MIND = 1,
    DEATH_RAY      = 2,
};

enum Actions
{
    ACTION_NOVA_HIT                    = 0,
    ACTION_ACTIVATE_CLOUDS             = 1,
    ACTION_DEACTIVATE_CLOUDS           = 2,
    ACTION_PORTAL_TO_MADNESS_STORMWIND = 3,
    ACTION_PORTAL_TO_MADNESS_DRAGON    = 4,
    ACTION_PORTAL_TO_MADNESS_LICHKING  = 5,
    ACTION_BRAIN_UNDER_30_PERCENT      = 6,
    ACTION_YOGGSARON_KILLED            = 7,
    ACTION_DEATH_RAY_MOVE              = 8,
    ACTION_USED_MIND_CONTROL           = 9,
    ACTION_MODIFY_SANITY               = 10,
};

enum Spells
{
	// All Phases
    SPELL_IN_THE_MAWS_OF_THE_OLD_GOD            = 64184,
    // Keeper Freya
    SPELL_RESILIENCE_OF_NATURE                  = 62670,
    SPELL_SANITY_WELL                           = 64170,
    SPELL_SANITY_WELL_OPTIC                     = 63288,
    SPELL_SANITY_WELL_DEBUFF                    = 64169,
    // Keeper Thorim
    SPELL_FURY_OF_THE_STORM                     = 62702,
    SPELL_TITANIC_STORM                         = 64171,
    SPELL_TITANIC_STORM_DUMMY                   = 64172, // Dummy Spell ... kills weakend guardians
    // Keeper Mimiron
    SPELL_SPEED_OF_INVENTION                    = 62671,
    SPELL_DESTABILIZATION_MATRIX                = 65210, // No AoE ... Target every Tentacle or Guardian Random
    // Keeper Hodir
    SPELL_FORTITUDE_OF_FROST                    = 62650,
    SPELL_HODIRS_PROTECTIVE_GAZE                = 64174,
    SPELL_FLASH_FREEZE                          = 64175,
    SPELL_FLASH_FREEZE_COOLDOWN                 = 64176,
    // Sanity
    SPELL_SANITY                                = 63050,
    SPELL_INSANE                                = 63120,
    SPELL_INSANE_2                              = 64464,
    //Phase 1:
    SPELL_SUMMON_GUARDIAN                       = 63031,
    SPELL_OMINOUS_CLOUD                         = 60977,
    SPELL_OMINOUS_CLOUD_TRIGGER                 = 60978,
    SPELL_OMINOUS_CLOUD_DAMAGE                  = 60984,
    SPELL_OMINOUS_CLOUD_EFFECT                  = 63084,
    //  Sara
    SPELL_SARAS_FERVOR                          = 63138,
    SPELL_SARAS_BLESSING                        = 63134,
    SPELL_SARAS_ANGER                           = 63147,
    //  Guardians of Yogg-Saron
    SPELL_DOMINATE_MIND                         = 63042, // Removed by blizz, Needs Sanity Scripting
    SPELL_DARK_VOLLEY                           = 63038, // Needs Sanity Scripting
	SPELL_SHADOW_NOVA_10                        = 62714,
	SPELL_SHADOW_NOVA_25                        = 65209,
	SPELL_SHADOW_NOVA_2                         = 65719,
    //Phase 2:
    SPELL_SARA_TRANSFORMATION                   = 65157,
    SPELL_PSYCHOSIS_10                          = 63795,
	SPELL_PSYCHOSIS_25                          = 65301,
    SPELL_BRAIN_LINK                            = 63802,
    SPELL_BRAIN_LINK_DAMAGE                     = 63803, // Needs Sanity Scripting
    SPELL_BRAIN_LINK_DUMMY                      = 63804,
    SPELL_MALADY_OF_MIND                        = 63830, // Needs Sanity Scripting, Trigger 63881 on remove
    SPELL_DEATH_RAY_SUMMON                      = 63891, // Summond 1 33882 (controll deathray)
    SPELL_DEATH_RAY_DAMAGE_VISUAL               = 63886, // Damage visual Effeñt of 33881 (triggered with periodic)
    SPELL_DEATH_RAY_PERIODIC                    = 63883, // Trigger 63884
    SPELL_DEATH_RAY_ORIGIN_VISUAL               = 63893, // Visual Effect of 33882
    SPELL_DEATH_RAY_WARNING_VISUAL              = 63882,
    SPELL_SARA_SHADOWY_BARRIER                  = 64775,
    // Tentacle
    SPELL_ERUPT                                 = 64144,
    SPELL_TENTACLE_VOID_ZONE                    = 64017,
    //  Crusher Tentacle
    SPELL_DIMISH_POWER                          = 64145, // Aura around Caster
    SPELL_FOCUS_ANGER                           = 57688, // Trigger 57689
    SPELL_CRUSH                                 = 64146,
    // Corruptor Tentacle
    SPELL_DRAINING_POISON                       = 64152,
    SPELL_BLACK_PLAGUE                          = 64153,
    SPELL_APATHY                                = 64156,
    SPELL_CURSE_OF_DOOM                         = 64157,
    // Constrictor Tentacles
    SPELL_LUNGE                                 = 64123,
	SPELL_SQUEEZE_10                            = 64125,
	SPELL_SQUEEZE_25                            = 64126,
    // Influence Tentacle
    SPELL_GRIM_REPRISAL                         = 63305, // Dummy aura
    SPELL_GRIM_REPRISAL_DAMAGE                  = 64039, // Damage 1
    //  Yogg-Saron
    SPELL_EXTINGUISH_ALL_LIFE                   = 64166, // After 15 Minutes
    SPELL_SHADOWY_BARRIER                       = 63894,
    SPELL_SUMMON_CRUSHER_TENTACLE               = 64139,
    SPELL_SUMMON_CURRUPTOR_TENTACLE             = 64143,
    SPELL_SUMMON_CONSTRICTOR_TENTACLES          = 64133,
    // Brain of Yogg-Sauron
    SPELL_INDUCE_MADNESS                        = 64059,
    SPELL_SHATTERED_ILLUSIONS                   = 64173,
    SPELL_BRAIN_HURT_VISUAL                     = 64361,
    // Mind Portal
    SPELL_BRAIN_TELEPORT                        = 64027, // Not Used
    SPELL_ILLUSION_ROOM                         = 63988, // Must be removed if not in Room
    //Phase 3:
    SPELL_YOGG_SARON_TRANSFORMATION             = 63895,
    //  Yogg-Saron
    SPELL_DEAFENING_ROAR                        = 64189,
    SPELL_SUMMON_IMMORTAL_GUARDIAN              = 64158,
    SPELL_SHADOW_BEACON                         = 64465,
    SPELL_EMPOWERING_SHADOWS_SCRP_1             = 64466,
    SPELL_EMPOWERING_SHADOWS_SCRP_2             = 64467,
    SPELL_EMPOWERING_SHADOWS_HEAL_10            = 64468,
    SPELL_EMPOWERING_SHADOWS_HEAL_25            = 64486,
	SPELL_LUNATIC_GAZE_10                       = 64167,
    SPELL_LUNATIC_GAZE_25                       = 64163,
	SPELL_LUNATIC_GAZE_EFFECT_10                = 64168, // Needs Sanity Scripting
    SPELL_LUNATIC_GAZE_EFFECT_25                = 64164, // Needs Sanity Scripting
    // Immortal Guardian
    SPELL_DRAIN_LIFE_10                         = 64159,
    SPELL_DRAIN_LIFE_25                         = 64160,
    SPELL_WEAKENED                              = 64162, // Dummy on low health for Titan Storm and Shadow Beacon
    SPELL_EMPOWERED                             = 65294, // stacks 9 times ... on 100% hp it have 9 stacks .. but with <10% it havent any
};

enum BossPhase
{
    PHASE_NONE  = 0,
    PHASE_SARA  = 1,
    PHASE_BRAIN = 2,
    PHASE_YOGG  = 3,
};

enum TentacleType
{
    CRUSHER_TENTACLE        = 0,
    CORRUPTOR_TENTACLE      = 1,
    CONSTRICTOR_TENTACLE    = 2,
};

enum BrainEventPhase
{
    PORTAL_PHASE_KING_LLIANE = 0,
    PORTAL_PHASE_DRAGON_SOUL = 1,
    PORTAL_PHASE_LICH_KING   = 2,
    PORTAL_PHASE_BRAIN_NONE  = 3,
};

const Position InnerBrainLocation[3] =
{
    {1944.87f, 37.22f, 239.7f, (0.66f*M_PI)}, //King Lliane
    {2045.97f, -25.45f, 239.8f, 0           }, //Dragon Soul
    {1953.41f, -73.73f, 240.0f, (1.33f*M_PI)},  //Lich King
};

const Position BrainPortalLocation[10] =
{
    {1970.48f, -9.75f, 325.5f, 0},
    {1992.76f, -10.21f, 325.5f, 0},
    {1995.53f, -39.78f, 325.5f, 0},
    {1969.25f, -42.00f, 325.5f, 0},
    {1960.62f, -32.00f, 325.5f, 0},
    {1981.98f, -5.69f,  325.5f, 0},
    {1982.78f, -45.73f, 325.5f, 0},
    {2000.66f, -29.68f, 325.5f, 0},
    {1999.88f, -19.61f, 325.5f, 0},
    {1961.37f, -19.54f, 325.5f, 0},
};

const Position KingLlaneTentacleLocation[8] =
{
    {1949.82f, 50.77f, 239.70f, (0.8f*M_PI)},
    {1955.24f, 72.08f, 239.70f, (1.04f*M_PI)},
    {1944.51f, 90.88f, 239.70f, (1.3f*M_PI)},
    {1923.46f, 96.71f, 239.70f, (1.53f*M_PI)},
    {1904.14f, 85.99f, 239.70f, (1.78f*M_PI)},
    {1898.78f, 64.62f, 239.70f, (0.05f*M_PI)},
    {1909.74f, 45.15f, 239.70f, (0.31f*M_PI)},
    {1931.01f, 39.85f, 239.70f, (0.55f*M_PI)},
};

const Position DragonSoulTentacleLocation[8] =
{
    // NPC_RUBY_CONSORT
    {2061.44f, -11.83f, 239.80f, 0},
    {2059.87f, -37.77f, 239.80f, 0},
    // NPC_AZURE_CONSORT
    {2105.42f, -71.81f, 239.80f, (0.51f*M_PI)},
    {2131.29f, -60.90f, 239.80f, (0.68f*M_PI)},
    // NPC_OBSIDIAN_CONSORT
    {2147.62f, -42.06f, 239.80f, M_PI},
    {2147.62f, -6.91f, 239.80f, M_PI},
    //NPC_EMERALD_CONSORT
    {2125.64f, 17.08f, 239.80f, (1.35f*M_PI)},
    {2109.06f, 22.69f, 239.80f, (1.42f*M_PI)},
};

const Position LichKingTentacleLocation[9] =
{
    {1944.20f, -136.72f, 240.00f, (1.35f*M_PI)},
    {1950.06f, -139.36f, 240.00f, (1.35f*M_PI)},
    {1948.23f, -129.44f, 240.00f, (1.35f*M_PI)},

    {1920.34f, -136.38f, 240.00f, (1.35f*M_PI)},
    {1914.44f, -132.35f, 240.00f, (1.35f*M_PI)},
    {1919.69f, -129.47f, 240.00f, (1.35f*M_PI)},

    {1909.49f, -111.84f, 240.00f, (1.35f*M_PI)},
    {1902.02f, -107.69f, 240.00f, (1.35f*M_PI)},
    {1910.28f, -102.96f, 240.00f, (1.35f*M_PI)},
};

const Position BrainLocation = {1980.01f, -25.36f, 260.00f, M_PI};
const Position SaraLocation = {1980.28f, -25.58f, 325.00f, M_PI};

const Position InnerBrainTeleportLocation[3] =
{
    {2001.40f, -59.66f, 245.07f, 0},
    {1941.61f, -25.88f, 244.98f, 0},
    {2001.18f,  9.409f, 245.24f, 0},
};

const Position KeeperSpawnLocation[4] =
{
    {1939.15f, 42.47f, 338.46f, 1.7f*M_PI}, // Mimiron
    {2037.09f, 25.43f, 338.46f, 1.3f*M_PI}, // Freya
    {2036.88f, -73.66f, 338.46f, 0.7f*M_PI}, // Thorim
    {1939.94f, -90.49f, 338.46f, 0.3f*M_PI}, // Hodir
};

const Position FreyaSanityWellLocation[5] =
{
    {1901.21f, -48.69f, 332.00f, 0},
    {1901.90f, -2.78f, 332.30f, 0},
    {1993.58f, 45.56f, 332.00f, 0},
    {1985.87f, -91.10f, 330.20f, 0},
    {2040.12f, -39.16f, 329.00f, 0},
};

const Position KingLlaneSkullLocation[3] =
{
    {1929.41f, 45.27f, 239.70f, 0},
    {1902.31f, 72.53f, 239.70f, 0},
    {1925.10f, 91.52f, 239.70f, 0},
};

const Position LichkingSkullLocation[3] =
{
    {1955.12f, -111.96f,  240.00f, 0},
    {1919.55f, -131.94f,  240.00f, 0},
    {1921.03f, -93.46f,  240.00f, 0},
};

const Position DragonSoulSkullLocation[4] =
{
    {2115.89f, 4.63f, 239.80f, 0},
    {2080.65f, 37.47f, 244.3f, 0},
    {2170.20f, -33.31f, 244.3f, 0},
    {2090.49f, -57.40f, 239.8f, 0},
};

const Position EventNpcLocation[9] =
{
    { 1928.23f, 66.81f, 242.40f, 5.207f }, // King Llane
    { 1929.78f, 63.60f, 242.40f, 2.124f }, // Garona - kneeling
    { 2103.87f, -13.13f, 242.65f, 4.692f }, // Ysera
    { 2118.68f, -25.56f, 242.65f, M_PI}, // Neltharion
    { 2095.87f, -34.47f, 242.65f, 0.836f }, // Malygos
    { 2104.61f, -25.36f, 242.65f, 0.0f }, // The Dragon Soul
    { 1903.41f, -160.21f, 239.99f, 1.114f }, // Immolated Champion
    { 1909.31f, -155.88f, 239.99f, 4.222f }, // Turned Champion
    { 1907.02f, -153.92f, 239.99f, 4.187f }, // The Lich King

};

struct EventNPC
{
    uint32 entry;
    uint64 guid;
};

struct EventSpeech
{
    uint32 npc_entry;
    int32 speech_entry;
    uint32 next_speech;
    bool isSpeaking;
};

const uint32 EventNpcEntrys[9] =
{
    NPC_KING_LLANE,
    NPC_GARONA,
    NPC_YSERA,
    NPC_NELTHARION,
    NPC_MALYGOS,
    GO_THE_DRAGON_SOUL,
    NPC_IMMOLATED_CHAMPION,
    NPC_TURNED_CHAMPION,
    NPC_LICH_KING,
};

const EventSpeech EventNpcSpeaching[19] =
{
    {NPC_GARONA, SAY_GARONA_1_VISION_1, 3000, true},
    {NPC_GARONA, SAY_GARONA_2_VISION_1, 5000, true},
    {NPC_GARONA, SAY_GARONA_3_VISION_1, 5000, true},
    {NPC_YOGG_SARON, SAY_YOGGSARON_4_VISION_1, 3000, true},
    {NPC_YOGG_SARON, SAY_YOGGSARON_5_VISION_1, 3000, true},
    {NPC_KING_LLANE, SAY_LLANE_6_VISION_1, 5000, true},
    {NPC_GARONA, SAY_GARONA_7_VISION_1, 5000, true},
    {NPC_YOGG_SARON, SAY_YOGGSARON_8_VISION_1, 5000, false},

    {NPC_LICH_KING, SAY_LICHKING_1_VISION_2, 5000, true},/*8*/
    {NPC_IMMOLATED_CHAMPION, SAY_CHAMP_2_VISION_2, 5000, true},
    {NPC_IMMOLATED_CHAMPION, SAY_CHAMP_3_VISION_2, 5000, true},
    {NPC_LICH_KING, SAY_LICHKING_4_VISION_2, 5000, true},
    {NPC_YOGG_SARON, SAY_YOGGSARON_5_VISION_2, 5000, true},
    {NPC_YOGG_SARON, SAY_YOGGSARON_6_VISION_2, 5000, false},

    {NPC_NELTHARION,SAY_NELTHARION_1_VISION_3, 5000, true},/*14*/
    {NPC_YSERA, SAY_YSERA_2_VISION_3, 5000, true},
    {NPC_NELTHARION, SAY_NELTHARION_3_VISION_3, 5000, true},
    {NPC_MALYGOS, SAY_MALYGOS_4_VISION_3, 5000, true},
    {NPC_YOGG_SARON, SAY_YOGGSARON_5_VISION_3, 5000, false},
};

bool IsPlayerInBrainRoom(const Player* player)
{
    return player->GetPositionZ() < 300;
}

class boss_sara : public CreatureScript
{
public:
    boss_sara() : CreatureScript("boss_sara") { }

    struct boss_saraAI : public QuantumBasicAI
    {
        boss_saraAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();

            SetImmuneToPushPullEffects(true);

			me->SetCanFly(true);
        }

        InstanceScript* instance;
        SummonList Summons;

        BossPhase phase;
        uint32 AmountKeeperActive;

        uint64 guidYogg;
        uint64 guidYoggBrain;
        std::list<uint64> guidEventTentacles;
        std::list<uint64> guidEventSkulls;
        std::list<EventNPC> listEventNPCs;

        bool UsedMindControll;
		bool InCombat;

        // Phase 1
        uint32 SaraHelpTimer;

        uint32 GuardianSummonTimer;
        uint32 LastGuardianSummonTimer;
        uint32 RandomYellTimer;

        uint32 NovaHitCounter;
        // Phase 2
        Actions lastBrainAction;
        bool IsSpeaking;
        uint32 SpeakingPhase;
        uint32 SpeakingTimer;

        uint32 PsychosisTimer;
        uint32 MindSpellTimer;
        uint32 TentacleTimer;
        uint32 Tentacle1Timer;
        uint32 EnrageTimer;
        uint32 MadnessTimer;

        uint32 BrainEventsCount;
        BrainEventPhase currentBrainEventPhase;

        bool IsEventSpeaking;

        uint32 EventSpeakingPhase;

        // Phase 3
        uint32 DeafeningRoarTimer;
        uint32 ShadowBeaconTimer;
        uint32 LunaticGazeTimer;

        void CloudHandling(bool remove)
        {
            std::list<Creature*> CloudList;
            me->GetCreatureListWithEntryInGrid(CloudList, NPC_OMINOUS_CLOUD, 100.0f);
            if (!CloudList.empty())
            {
                for (std::list<Creature*>::iterator itr = CloudList.begin(); itr != CloudList.end(); itr++)
                {
                    if (remove)
						(*itr)->AI()->DoAction(ACTION_DEACTIVATE_CLOUDS);
                    else
						(*itr)->AI()->DoAction(ACTION_ACTIVATE_CLOUDS);
                }
            }
        }

        void Reset()
        {
			Summons.DespawnAll();
            guidEventTentacles = std::list<uint64>();
            guidEventSkulls = std::list<uint64>();
            listEventNPCs = std::list<EventNPC>();

            me->InterruptNonMeleeSpells(false);
            Position pos = me->GetHomePosition();
            me->NearTeleportTo(pos.m_positionX,pos.m_positionY,pos.m_positionZ,pos.m_orientation);
            me->RemoveAurasDueToSpell(SPELL_SHATTERED_ILLUSIONS);
            me->RemoveAurasDueToSpell(SPELL_SARA_SHADOWY_BARRIER);

            // Remove Random MoveMaster
            me->GetMotionMaster()->Clear(false);
            me->GetMotionMaster()->MoveIdle();

            // Reset Display
            me->SetCurrentFaction(FACTION_FRIENDLY);
            me->SetVisible(true);
            //me->SetDisplayId(me->GetNativeDisplayId());
            me->RemoveAurasDueToSpell(SPELL_SARA_TRANSFORMATION);
            // Reset Health
            me->SetHealth(me->GetMaxHealth());

            // Reset Phase
            phase = PHASE_NONE;
            // Respawn Clouds
            CloudHandling(false);
            SetSanityAura(true);
            // Reset HitCounter Phase 1
            NovaHitCounter = 8;
            currentBrainEventPhase = PORTAL_PHASE_BRAIN_NONE;

            // Spawn Yoggy if not spawned
            Creature* yogg = me->GetCreature(*me, guidYogg);
			if (yogg)
				yogg->DespawnAfterAction();

            if (yogg = DoSummon(NPC_YOGG_SARON,SaraLocation, 0, TEMPSUMMON_MANUAL_DESPAWN))
                yogg->SetLootMode(LOOT_MODE_DEFAULT);

            if (Creature* yoggbrain = me->GetCreature(*me, guidYoggBrain))
			{
				if (yoggbrain)
					yoggbrain->DespawnAfterAction();
			}

            DoSummon(NPC_BRAIN_OF_YOGG_SARON, BrainLocation, 0, TEMPSUMMON_MANUAL_DESPAWN);

			instance->SetBossState(DATA_YOGG_SARON, NOT_STARTED);

            RandomYellTimer = urand(10000,20000);

            UsedMindControll = false;
			InCombat = false;

            AmountKeeperActive = -1;
        }

		void MoveInLineOfSight(Unit* who)
        {
            if (phase == PHASE_NONE)
            {
				if (!instance || InCombat || who->GetTypeId() != TYPE_ID_PLAYER || !who->IsWithinDistInMap(me, 32.0f))
					return;

				if (instance && InCombat == false && who->GetTypeId() == TYPE_ID_PLAYER && who->IsWithinDistInMap(me, 32.0f))
				{
					if (instance->GetBossState(DATA_GENERAL_VEZAX) == DONE)
					{
						SetPhase(PHASE_SARA);
						EnterToBattle(who);
						InCombat = true;
					}
                }
            }
        }

        void JustDied(Unit* /*killer*/)
        {
			instance->SetBossState(DATA_YOGG_SARON, DONE);

            SetSanityAura(true);

            Summons.DespawnAll();
        }

        void DoSpawnKeeperForSupport() // Despawn on Sara Reset
        {
			uint32 SupportFlag = instance->GetData(DATA_KEEPER_SUPPORT_YOGG);

			if ((SupportFlag & MIMIRON_SUPPORT) == MIMIRON_SUPPORT)
				DoSummon(NPC_KEEPER_MIMIRON,KeeperSpawnLocation[0], 0, TEMPSUMMON_MANUAL_DESPAWN);

			if ((SupportFlag & FREYA_SUPPORT) == FREYA_SUPPORT)
				DoSummon(NPC_KEEPER_FREYA,KeeperSpawnLocation[1], 0, TEMPSUMMON_MANUAL_DESPAWN);

			if ((SupportFlag & THORIM_SUPPORT) == THORIM_SUPPORT)
				DoSummon(NPC_KEEPER_THORIM,KeeperSpawnLocation[2], 0, TEMPSUMMON_MANUAL_DESPAWN);

			if ((SupportFlag & HODIR_SUPPORT) == HODIR_SUPPORT)
				DoSummon(NPC_KEEPER_HODIR,KeeperSpawnLocation[3], 0, TEMPSUMMON_MANUAL_DESPAWN);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            SaraHelpTimer = urand(5000,6000);
            GuardianSummonTimer = 10000;
            LastGuardianSummonTimer = 35000;

            PsychosisTimer = urand(5000,5000);
            MindSpellTimer = urand(30000,30000);
            TentacleTimer = urand(3000,5000);
            Tentacle1Timer = 1000;
            MadnessTimer = 60000;

            DeafeningRoarTimer = urand(30000,60000);
            ShadowBeaconTimer = 30000;
            LunaticGazeTimer = 12000;

            EnrageTimer = 900000;

            if (Creature* yogg = me->GetCreature(*me, guidYogg))
                DoZoneInCombat(yogg);

			instance->SetBossState(DATA_YOGG_SARON, IN_PROGRESS);

            lastBrainAction = Actions(0);
        }

        void ReceiveEmote(Player* player, uint32 emote)
        {
            if (phase >= PHASE_BRAIN)
            {
                if (emote == TEXT_EMOTE_KISS)
                {
                    if (player->HasAchieved(RAID_MODE(ACHIEVMENT_KISS_AND_MAKE_UP_10,ACHIEVMENT_KISS_AND_MAKE_UP_25)))
                        return;
					
					if (me->IsWithinLOS(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ()) && me->IsWithinDistInMap(player, 100.0f))
                    {
                        if (AchievementEntry const *achievKissAndMakeUp = sAchievementStore.LookupEntry(RAID_MODE(ACHIEVMENT_KISS_AND_MAKE_UP_10, ACHIEVMENT_KISS_AND_MAKE_UP_25)))
                            player->CompletedAchievement(achievKissAndMakeUp);
                    }
                }
            }
        }

        void DamageTaken(Unit* dealer, uint32 &damage)
        {
            if (dealer->GetEntry() == NPC_GUARD_OF_YOGG_SARON)
            {
                damage = 0;
                return;
            }

            if (damage > me->GetHealth())
                damage = me->GetHealth()-1;
        }

        uint32 CountKeepersActive()
        {
            uint32 count = 0;
            if (instance)
            {
                uint32 supportFlag = instance->GetData(DATA_KEEPER_SUPPORT_YOGG);

                if (supportFlag & MIMIRON_SUPPORT)
                    count++;

                if (supportFlag & FREYA_SUPPORT)
                    count++;

                if (supportFlag & THORIM_SUPPORT)
                    count++;

                if (supportFlag & HODIR_SUPPORT)
                    count++;
            }
            return count;
        }

        void ModifySanity(Player* target, int8 amount)
        {
            if (target && target->IsAlive())
            {
                int32 newamount;
                if (Aura* aur = target->GetAura(SPELL_SANITY, GetGUID()))
                {
                    newamount = aur->GetStackAmount();
                    if (newamount > 0)
                        newamount += amount;

                    if (newamount > 100)
                        newamount = 100;

                    if (newamount <= 0)
                        target->RemoveAurasDueToSpell(SPELL_SANITY);
                    else
                        aur->SetStackAmount(newamount);
                }
            }
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            if (target && target->ToPlayer())
            {
                switch(spell->Id)
                {
                    case SPELL_PSYCHOSIS_10:
						ModifySanity(target->ToPlayer(), -9);
						break;
					case SPELL_PSYCHOSIS_25:
						ModifySanity(target->ToPlayer(), -12);
						break;
					case SPELL_BRAIN_LINK_DAMAGE:
						ModifySanity(target->ToPlayer(), -2);
						break;
					case SPELL_MALADY_OF_MIND:
						ModifySanity(target->ToPlayer(), -3);
						break;
                }
            }
        }

        void MovementInform(uint32 type, uint32 i)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (i == 1)
            {
                me->GetMotionMaster()->Clear(false);
                me->GetMotionMaster()->MoveIdle();
            }
        }

        void DoAction(const int32 action)
        {
            switch(action)
            {
            case ACTION_NOVA_HIT:
                if (phase != PHASE_SARA)
                    return;

                NovaHitCounter--;

                if (NovaHitCounter <= 0)
                    SetPhase(PHASE_BRAIN);
                else me->ModifyHealth(-25000);
                break;
            case ACTION_BRAIN_UNDER_30_PERCENT:
                SetPhase(PHASE_YOGG);
                break;
            case ACTION_YOGGSARON_KILLED:
                if (Creature* yogg = me->GetCreature(*me, guidYogg))
                    yogg->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

                if (EnrageTimer >= 480000)
                    instance->DoCompleteAchievement(RAID_MODE(ACHIEVMENT_HE_S_NOT_GETTING_ANY_OLDER_10, ACHIEVMENT_HE_S_NOT_GETTING_ANY_OLDER_25));

                if (!UsedMindControll)
                    instance->DoCompleteAchievement(RAID_MODE(ACHIEVMENT_DRIVE_ME_CRAZY_10, ACHIEVMENT_DRIVE_ME_CRAZY_25));

                if (GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL && AmountKeeperActive == 0)
                    instance->DoCompleteAchievement(ACHIEVMENT_REALM_FIRST_DEATHS_DEMISE);

                switch(AmountKeeperActive)
                {
				    case 0:
						instance->DoCompleteAchievement(RAID_MODE(ACHIEVMENT_ALONE_IN_THE_DARKNESS_10, ACHIEVMENT_ALONE_IN_THE_DARKNESS_25));
					case 1:
						instance->DoCompleteAchievement(RAID_MODE(ACHIEVMENT_ONE_LIGHTS_IN_THE_DARKNESS_10, ACHIEVMENT_ONE_LIGHTS_IN_THE_DARKNESS_25));
					case 2:
						instance->DoCompleteAchievement(RAID_MODE(ACHIEVMENT_TWO_LIGHTS_IN_THE_DARKNESS_10, ACHIEVMENT_TWO_LIGHTS_IN_THE_DARKNESS_25));
					case 3:
						instance->DoCompleteAchievement(RAID_MODE(ACHIEVMENT_THREE_LIGHTS_IN_THE_DARKNESS_10, ACHIEVMENT_THREE_LIGHTS_IN_THE_DARKNESS_25));
						break;
					default:
						break;
				}
                me->Kill(me);
                break;
            case ACTION_USED_MIND_CONTROL:
                UsedMindControll = true;
                break;
            }
        }

        uint32 GetData(uint32 data)
        {
            switch(data)
            {
                case DATA_PORTAL_PHASE:
					return currentBrainEventPhase;
			}
            return 0;
        }

        void SetPhase(BossPhase newPhase)
        {
            if (phase == newPhase)
                return;

            switch(newPhase)
            {
            case PHASE_SARA:
                DoSendQuantumText(SAY_SARA_AGGRO_1, me);
                DoSpawnKeeperForSupport();
                SetSanityAura();
                AmountKeeperActive = CountKeepersActive();
                if (Creature* yogg = me->GetCreature(*me, guidYogg))
                {
                    yogg->SetLootMode(LOOT_MODE_DEFAULT);
                    if (AmountKeeperActive <= 1)
                        yogg->AddLootMode(LOOT_MODE_HARD_MODE_1);
                    if (AmountKeeperActive == 0)
                        yogg->AddLootMode(LOOT_MODE_HARD_MODE_2);
                }
                break;
            case PHASE_BRAIN:
                me->SetHealth(me->GetMaxHealth());
                IsSpeaking = true;
                SpeakingPhase = 0;
                SpeakingTimer = 1000;
                IsEventSpeaking = false;
                EventSpeakingPhase = 0;
                //me->SetDisplayId(SARA_TRANSFORM);
                CloudHandling(true);
                RandomYellTimer = 35000;
                BrainEventsCount = 0;
                break;
            case PHASE_YOGG:
                me->SetVisible(false);
                if (Creature* yogg = me->GetCreature(*me, guidYogg))
                {
                    yogg->LowerPlayerDamageReq(uint32(yogg->GetMaxHealth()*0.3f));
                    yogg->SetHealth(yogg->CountPctFromMaxHealth(HEALTH_PERCENT_30));
                    yogg->RemoveAurasDueToSpell(SPELL_SHADOWY_BARRIER);
                    yogg->RemoveAurasDueToSpell(SPELL_SHATTERED_ILLUSIONS);
                    DoSendQuantumText(SAY_PHASE3,yogg);
                    me->AddAura(SPELL_YOGG_SARON_TRANSFORMATION,yogg);
                }

                for (std::list<uint64>::iterator itr = Summons.begin(); itr != Summons.end(); ++itr)
                {
                    if (Creature* temp = Creature::GetCreature(*me,*itr))
                    {
                        switch(temp->GetEntry())
                        {
                            case NPC_CONSTRICTOR_TENTACLE:
							case NPC_CORRUPTOR_TENTACLE:
							case NPC_CRUSHER_TENTACLE:
								temp->RemoveAurasDueToSpell(SPELL_SHATTERED_ILLUSIONS);
								break;
                        }
                    }
                }

                Summons.DespawnEntry(NPC_BRAIN_PORTAL);

                if (Creature* yoggbrain = me->GetCreature(*me, guidYoggBrain))
                    yoggbrain->InterruptNonMeleeSpells(false);

                GuardianSummonTimer = 20000;
                LastGuardianSummonTimer = 35000;
                break;
            }

            phase = newPhase;
        }

        void SaraRandomYell()
        {
            switch(phase)
            {
                case PHASE_NONE:
					DoSendQuantumText(RAND(SAY_SARA_PREFIGHT_1, SAY_SARA_PREFIGHT_2), me);
					break;
				case PHASE_SARA:
					DoSendQuantumText(RAND(SAY_SARA_AGGRO_2, SAY_SARA_AGGRO_3), me);
					break;
				case PHASE_BRAIN:
					DoSendQuantumText(RAND(SAY_SARA_PHASE2_1, SAY_SARA_PHASE2_2, SAY_SARA_AGGRO_3), me);
					break;
            }
        }

        void JustSummoned(Creature* summoned)
        {
            switch(summoned->GetEntry())
            {
            case NPC_GUARD_OF_YOGG_SARON:
                summoned->SetCurrentFaction(FACTION_HOSTILE);
                summoned->AI()->AttackStart(SelectPlayerTargetInRange(100.0f));
                break;
            case NPC_YOGG_SARON:
                guidYogg = summoned->GetGUID();
                summoned->SetReactState(REACT_PASSIVE);
                summoned->SetCurrentFaction(FACTION_HOSTILE);
                summoned->SetStandState(UNIT_STAND_STATE_SUBMERGED);
                summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                summoned->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 20.0f);
                summoned->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 20.0f);
                return;
            case NPC_BRAIN_OF_YOGG_SARON:
                guidYoggBrain = summoned->GetGUID();
                summoned->SetActive(true);
                summoned->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 25.0f);
                summoned->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 25.0f);
                summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                break;
            case NPC_RUBY_CONSORT:
            case NPC_OBSIDIAN_CONSORT:
            case NPC_AZURE_CONSORT:
            case NPC_EMERALD_CONSORT:
            case NPC_DEATHSWORN_ZEALOT:
            case NPC_SUIT_OF_ARMOR:
                summoned->SetReactState(REACT_DEFENSIVE);
                summoned->SetCurrentFaction(FACTION_CREATURE);
                break;
            case NPC_KEEPER_FREYA:
            case NPC_KEEPER_HODIR:
            case NPC_KEEPER_MIMIRON:
            case NPC_KEEPER_THORIM:
                summoned->SetActive(true);
                break;
            case NPC_LAUGHING_SKULL:
                summoned->SetCurrentFaction(FACTION_HOSTILE);
                summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                break;
            }

            Summons.Summon(summoned);
        }

        bool IsEncounterInProgress()
        {
            if (me->GetMap() && me->GetMap()->IsDungeon())
            {
                Map::PlayerList const& players = me->GetMap()->GetPlayers();
                if (!players.isEmpty())
				{
                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
					{
                        if (Player* player = itr->getSource())
						{
                            if (player->IsAlive() && !player->IsGameMaster() && !player->HasAura(SPELL_INSANE))
                                return true;
						}
					}
				}
            }
            return false;
        }

        void SetSanityAura(bool remove = false)
        {
            if (me->GetMap() && me->GetMap()->IsDungeon())
            {
                Map::PlayerList const& players = me->GetMap()->GetPlayers();
                if (!players.isEmpty())
				{
                    for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
					{
                        if (Player* player = itr->getSource())
						{
                            if (player->IsAlive())
                            {
                                if (!remove)
                                {
                                    me->AddAura(SPELL_SANITY, player);
                                    me->SetAuraStack(SPELL_SANITY, player, 100);
                                }
								else
                                    player->RemoveAurasDueToSpell(SPELL_SANITY);
                            }
						}
					}
				}
            }
        }

        Player* SelectPlayerTargetInRange(float range)
        {
            Player* target = NULL;
            Trinity::AnyPlayerInObjectRangeCheck u_check(me, range, true);
            Trinity::PlayerSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(me, target, u_check);
            me->VisitNearbyObject(range, searcher);
            return target;
        }

        Creature* GetRandomEntryTarget(uint32 entry, float range = 100.0f)
        {
            std::list<Creature*> TargetList;
            me->GetCreatureListWithEntryInGrid(TargetList,entry, range);
            if (TargetList.empty())
                return NULL;

            std::list<Creature*>::iterator itr = TargetList.begin();
            advance(itr, urand(0, TargetList.size()-1));
            return (*itr);
        }

        // For Gardian Summon
        Creature* GetRandomCloudTarget(float range = 100.0f)
        {
            return GetRandomEntryTarget(NPC_OMINOUS_CLOUD);
        }

        // For Immortal Guardian Heal
        Creature* GetRandomGuardianTarget(float range = 100.0f)
        {
            return GetRandomEntryTarget(NPC_IMMORTAL_GUARDIAN);
        }

        bool AllSpawnsDeadOrDespawned(std::list<uint64> GuidListe)
        {
            for (std::list<uint64>::iterator itr = GuidListe.begin(); itr != GuidListe.end(); ++itr)
            {
                Creature* temp = me->GetCreature(*me,*itr);
                if (temp && temp->IsAlive())
                    return false;
            }

            return true;
        }

        void DoKillAndDespawnGUIDs(std::list<uint64> GuidListe)
        {
            for (std::list<uint64>::iterator itr = GuidListe.begin(); itr != GuidListe.end(); ++itr)
            {
                Creature* temp = me->GetCreature(*me, *itr);
                if (temp && temp->IsAlive())
                {
                    temp->DealDamage(temp, temp->GetHealth());
                    temp->RemoveCorpse();
                }
            }
        }

        void SummonMadnessEventNPCs()
        {
            listEventNPCs.clear();
            uint32 npcIndex = 0;
            uint32 npcAmount = 0;

            switch(currentBrainEventPhase)
            {
            case PORTAL_PHASE_KING_LLIANE:
                npcIndex = 0;
                npcAmount = 2;
                break;
            case PORTAL_PHASE_DRAGON_SOUL:
                npcIndex = 2;
                npcAmount = 4;
                break;
            case PORTAL_PHASE_LICH_KING:
                npcIndex = 6;
                npcAmount = 3;
                break;
            }

            Creature* temp;

            for (uint8 i = 0; i < npcAmount; i++)
            {
                if (EventNpcEntrys[npcIndex+i] == GO_THE_DRAGON_SOUL)
                {
                    if (GameObject* obj = me->SummonGameObject(EventNpcEntrys[npcIndex+i],EventNpcLocation[npcIndex+i].GetPositionX(),EventNpcLocation[npcIndex+i].GetPositionY(),EventNpcLocation[npcIndex+i].GetPositionZ(),0,0,0,0,0,40000))
                    {
                        obj->SetActive(true);
                        EventNPC* info = new EventNPC();
                        info->entry = obj->GetEntry();
                        info->guid = obj->GetGUID();

                        listEventNPCs.push_back(*info);
                    }
                }
                else
                {
                    if (temp = DoSummon(EventNpcEntrys[npcIndex+i],EventNpcLocation[npcIndex+i],40000,TEMPSUMMON_TIMED_DESPAWN))
                    {
                        temp->SetActive(true);
                        EventNPC *info = new EventNPC();
                        info->entry = temp->GetEntry();
                        info->guid = temp->GetGUID();
                        switch(info->entry)
                        {
                            case NPC_GARONA:
								temp->SetStandState(UNIT_STAND_STATE_KNEEL);
								break;
						}
						listEventNPCs.push_back(*info);
                    }
                }
            }
        }

        uint64 GetEventNPCGuid(uint32 entry)
        {
            for (std::list<EventNPC>::iterator itr = listEventNPCs.begin(); itr != listEventNPCs.end(); ++itr)
            {
                if (itr->entry == entry)
                    return itr->guid;
            }
            return 0;
        }

        void DoMadness()
        {
			instance->HandleGameObject(instance->GetData64(TYPE_BRAIN_DOOR_1), false);
			instance->HandleGameObject(instance->GetData64(TYPE_BRAIN_DOOR_2), false);
			instance->HandleGameObject(instance->GetData64(TYPE_BRAIN_DOOR_3), false);

            Actions tempAction;
            switch(lastBrainAction)
            {
            case ACTION_PORTAL_TO_MADNESS_STORMWIND:
                tempAction = ACTION_PORTAL_TO_MADNESS_DRAGON;
                break;
            case ACTION_PORTAL_TO_MADNESS_DRAGON:
                tempAction = ACTION_PORTAL_TO_MADNESS_LICHKING;
                break;
            case ACTION_PORTAL_TO_MADNESS_LICHKING:
                tempAction = ACTION_PORTAL_TO_MADNESS_STORMWIND;
                break;
            default:
                tempAction = RAND(ACTION_PORTAL_TO_MADNESS_STORMWIND,ACTION_PORTAL_TO_MADNESS_DRAGON,ACTION_PORTAL_TO_MADNESS_LICHKING);
                break;
            }
            lastBrainAction = tempAction;

            // Spawn Portal
            int max = RAID_MODE(4,10);
            for (int i = 0; i < max; ++i)
            {
                if (Creature* portal = DoSummon(NPC_BRAIN_PORTAL,BrainPortalLocation[i],40000,TEMPSUMMON_TIMED_DESPAWN))
                    portal->AI()->DoAction(tempAction);
            }

            // Spawn Event Tentacle
            switch(tempAction)
            {
                case ACTION_PORTAL_TO_MADNESS_DRAGON:
					currentBrainEventPhase = PORTAL_PHASE_DRAGON_SOUL;
					guidEventTentacles.clear();
					guidEventSkulls.clear();
					uint32 entry;

					for (int i = 0; i < 8; ++i)
					{
						switch(i)
						{
						    case 0:
							case 1:
								entry = NPC_RUBY_CONSORT;
								break;
							case 2:
							case 3:
								entry = NPC_AZURE_CONSORT;
								break;
							case 4:
							case 5:
								entry = NPC_OBSIDIAN_CONSORT;
								break;
							case 6:
							case 7:
								entry = NPC_EMERALD_CONSORT;
								break;
						}

						if (Creature* summon = DoSummon(entry, DragonSoulTentacleLocation[i], 60000, TEMPSUMMON_TIMED_DESPAWN))
							guidEventTentacles.push_back(summon->GetGUID());
					}
					for (int i = 0; i < 4; ++i)
					{
						if (Creature* summon = DoSummon(NPC_LAUGHING_SKULL, DragonSoulSkullLocation[i], 60000, TEMPSUMMON_TIMED_DESPAWN))
							guidEventSkulls.push_back(summon->GetGUID());
					}
					break;
            case ACTION_PORTAL_TO_MADNESS_LICHKING:
                currentBrainEventPhase = PORTAL_PHASE_LICH_KING;
                guidEventTentacles.clear();
                guidEventSkulls.clear();
                for (int i = 0; i < 9; ++i)
                {
                    if (Creature* summon = DoSummon(NPC_DEATHSWORN_ZEALOT, LichKingTentacleLocation[i], 60000, TEMPSUMMON_TIMED_DESPAWN))
                        guidEventTentacles.push_back(summon->GetGUID());
                }
                for (int i = 0; i < 3; ++i)
                {
                    if (Creature* summon = DoSummon(NPC_LAUGHING_SKULL, LichkingSkullLocation[i], 60000, TEMPSUMMON_TIMED_DESPAWN))
                        guidEventSkulls.push_back(summon->GetGUID());
                }
                break;
            case ACTION_PORTAL_TO_MADNESS_STORMWIND:
                currentBrainEventPhase = PORTAL_PHASE_KING_LLIANE;
                guidEventTentacles.clear();
                guidEventSkulls.clear();
                for (int i = 0; i < 8; ++i)
                {
                    if (Creature* summon = DoSummon(NPC_SUIT_OF_ARMOR, KingLlaneTentacleLocation[i], 60000, TEMPSUMMON_TIMED_DESPAWN))
                        guidEventTentacles.push_back(summon->GetGUID());
                }
                for (int i = 0; i < 3; ++i)
                {
                    if (Creature* summon = DoSummon(NPC_LAUGHING_SKULL, KingLlaneSkullLocation[i], 60000, TEMPSUMMON_TIMED_DESPAWN))
                        guidEventSkulls.push_back(summon->GetGUID());
                }
                break;
            }

            if (Creature* yoggbrain = me->GetCreature(*me, guidYoggBrain))
            {
                yoggbrain->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                yoggbrain->CastSpell(yoggbrain,SPELL_INDUCE_MADNESS, false);
            }

            SummonMadnessEventNPCs();
            IsEventSpeaking = true;
            EventSpeakingPhase = 0;
            SpeakingTimer = 5000;

            BrainEventsCount++;
        }

        uint32 DoEventSpeaking(BrainEventPhase phase, uint32 part)
        {
            uint64 npcguid = 0;
            uint32 speachindex = 0;
            switch(phase)
            {
            case PORTAL_PHASE_KING_LLIANE:
                speachindex = 0;
                break;
            case PORTAL_PHASE_LICH_KING:
                speachindex = 8;
                break;
            case PORTAL_PHASE_DRAGON_SOUL:
                speachindex = 14;
            }

            if (phase+speachindex > 18)
                return 5000;

            if (EventNpcSpeaching[speachindex+part].npc_entry != NPC_YOGG_SARON)
                npcguid = GetEventNPCGuid(EventNpcSpeaching[speachindex+part].npc_entry);
            else
                npcguid = guidYogg;

            if (Creature* speaker = Creature::GetCreature(*me, npcguid))
                DoSendQuantumText(EventNpcSpeaching[speachindex+part].speech_entry, speaker);

            IsEventSpeaking = EventNpcSpeaching[speachindex+part].isSpeaking;
            return EventNpcSpeaching[speachindex+part].next_speech;
        }

        void UpdateAI(const uint32 diff)
        {
            if (instance->GetBossState(DATA_GENERAL_VEZAX) == DONE)
			{
                if (RandomYellTimer <= diff)
                {
                    SaraRandomYell();
                    RandomYellTimer = urand(40000, 60000);
                }
				else RandomYellTimer -= diff;
			}

            switch(phase)
            {
                case PHASE_SARA:
                    if (GuardianSummonTimer <= diff)
                    {
                        if (Creature* target = GetRandomCloudTarget(500.0f))
                            target->CastSpell(target, SPELL_SUMMON_GUARDIAN, true);

                        LastGuardianSummonTimer = LastGuardianSummonTimer-5000 <= 10000 ? 10000 : LastGuardianSummonTimer - 5000;
                        GuardianSummonTimer = LastGuardianSummonTimer;
                    }
					else GuardianSummonTimer -= diff;

                    if (SaraHelpTimer <= diff)
                    {
                        switch(urand(0,2))
                        {
                            case 0:
								if (Player* target = SelectPlayerTargetInRange(500.0f))
								{
									if (!IsPlayerInBrainRoom(target))
										DoCast(target, SPELL_SARAS_FERVOR, false);
								}
								break;
							case 1:
								if (Player* target = SelectPlayerTargetInRange(500.0f))
								{
									if (!IsPlayerInBrainRoom(target))
										DoCast(target, SPELL_SARAS_BLESSING, false);
								}
							case 2:
								if (Unit* target = GetRandomEntryTarget(NPC_GUARD_OF_YOGG_SARON, 500.0f))
									DoCast(target, SPELL_SARAS_ANGER, false);
								break;
							default:
								break;
                        }
                        SaraHelpTimer = urand(5000,6000);
                    }
					else SaraHelpTimer -= diff;
                    break;
                case PHASE_BRAIN:
                    if (IsSpeaking)
                    {
                        if (SpeakingTimer <= diff)
                        {
                            switch(SpeakingPhase)
                            {
                            case 0:
                                //if (Creature* yogg = me->GetCreature(*me, guidYogg))
                                //    yogg->SetVisible(true);

                                DoSendQuantumText(SAY_PHASE2_1,me);
                                SpeakingTimer = 5000;
                                break;
                            case 1:
                                DoSendQuantumText(SAY_PHASE2_2,me);
                                SpeakingTimer = 5000;
                                break;
                            case 2:
                                DoSendQuantumText(SAY_PHASE2_3,me);
                                SpeakingTimer = 4000;
                                break;
                            case 3:
                                DoSendQuantumText(SAY_PHASE2_4,me);
                                SpeakingTimer = 4000;
                                break;
                            case 4:
                                me->AddAura(SPELL_SARA_TRANSFORMATION, me);
                                DoCast(me, SPELL_SARA_SHADOWY_BARRIER, false);
                                me->GetMotionMaster()->MovePoint(1, me->GetPositionX(),me->GetPositionY(),me->GetPositionZ()+20);
                                if (Creature* yogg = me->GetCreature(*me, guidYogg))
                                {
                                    yogg->CastSpell(yogg, SPELL_SHADOWY_BARRIER, false);
                                    yogg->SetStandState(UNIT_STAND_STATE_STAND);
                                    yogg->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                                    DoSendQuantumText(SAY_PHASE2_5,yogg);

                                    yogg->CastSpell(yogg,SPELL_SUMMON_CURRUPTOR_TENTACLE,true);
                                }
                                me->SetCurrentFaction(FACTION_HOSTILE);
                                SpeakingTimer = 5000;
                                IsSpeaking = false;
                                break;
                            }
                            SpeakingPhase++;
                        }
						else SpeakingTimer -= diff;
                    }
					else
                    {
                        if (IsEventSpeaking)
                        {
                            if (SpeakingTimer <= diff)
                            {
                                SpeakingTimer = DoEventSpeaking(currentBrainEventPhase,EventSpeakingPhase);
                                EventSpeakingPhase++;
                            }
							else SpeakingTimer -= diff;
                        }

                        if (!me->HasAura(SPELL_SHATTERED_ILLUSIONS))
                        {
                            if (PsychosisTimer <= diff)
                            {
                                if (Player* target = SelectPlayerTargetInRange(500.0f))
								{
                                    if (!IsPlayerInBrainRoom(target))
                                        DoCast(target, RAID_MODE(SPELL_PSYCHOSIS_10, SPELL_PSYCHOSIS_25), false);
								}
                                PsychosisTimer = urand(5000, 6000);
                            }
							else PsychosisTimer -= diff;

                            if (MindSpellTimer <= diff)
                            {
                                if (!me->IsNonMeleeSpellCasted(false))
                                {
                                    switch(urand(0, 2))
                                    {
                                        case 0:
											DoCast(SPELL_MALADY_OF_MIND);
											break;
										case 1:
											DoCast(SPELL_BRAIN_LINK);
											break;
										case 2:
											DoCast(SPELL_DEATH_RAY_SUMMON);
											break;
                                    }
                                    MindSpellTimer = 30000;
                                }
                            }
							else MindSpellTimer -= diff;

                            if (TentacleTimer <= diff)
                            {
                                if (urand(0, 1) == 0)
                                {
                                    if (Player* target = SelectPlayerTargetInRange(500.0f))
									{
                                        if (!IsPlayerInBrainRoom(target))
                                            target->CastSpell(target, SPELL_SUMMON_CONSTRICTOR_TENTACLES, true);
									}
                                }
								else
                                {
                                    if (Creature* yogg = me->GetCreature(*me, guidYogg))
                                        yogg->CastSpell(yogg,SPELL_SUMMON_CURRUPTOR_TENTACLE,true);
                                }
                                TentacleTimer =  BrainEventsCount < 4 ? urand(5000,10000) : urand(2000,5000);
                            }
							else TentacleTimer -= diff;

                            if (Tentacle1Timer <= diff)
                            {
                                if (Creature* yogg = me->GetCreature(*me, guidYogg))
                                    yogg->CastSpell(yogg,SPELL_SUMMON_CRUSHER_TENTACLE,true);

                                Tentacle1Timer = BrainEventsCount < 4 ? urand(30000,40000) : urand(10000,15000);
                            }
							else Tentacle1Timer -= diff;
                        }
						else
                        {
                            if (Creature* yoggbrain = me->GetCreature(*me, guidYoggBrain))
							{
                                if (!yoggbrain->IsNonMeleeSpellCasted(false))
                                {
                                    if (Creature* yogg = me->GetCreature(*me, guidYogg))
                                        yogg->RemoveAurasDueToSpell(SPELL_SHATTERED_ILLUSIONS);

                                    me->RemoveAurasDueToSpell(SPELL_SHATTERED_ILLUSIONS);

                                    for (std::list<uint64>::iterator itr = Summons.begin(); itr != Summons.end(); ++itr)
                                    {
                                        if (Creature* temp = Creature::GetCreature(*me,*itr))
                                        {
                                            switch(temp->GetEntry())
                                            {
                                                case NPC_CONSTRICTOR_TENTACLE:
												case NPC_CORRUPTOR_TENTACLE:
												case NPC_CRUSHER_TENTACLE:
													temp->RemoveAurasDueToSpell(SPELL_SHATTERED_ILLUSIONS);
													break;
                                            }
                                        }
                                    }
                                }
							}
                        }

                        if (MadnessTimer <= diff)
                        {
                            if (Creature* yogg = me->GetCreature(*me, guidYogg))
                            {
                                DoSendQuantumText(SAY_VISION, yogg);
                                DoMadness();
                            }

                            //Say YoggSaron to Open Portals
                            MadnessTimer = 80000;
                        }
						else MadnessTimer -= diff;

                        if (!me->HasAura(SPELL_SHATTERED_ILLUSIONS))
                        {
                            if (currentBrainEventPhase != PORTAL_PHASE_BRAIN_NONE)
                            {
                                if (AllSpawnsDeadOrDespawned(guidEventTentacles))
                                {
                                    switch(currentBrainEventPhase)
                                    {
                                    case PORTAL_PHASE_DRAGON_SOUL:
										instance->HandleGameObject(instance->GetData64(TYPE_BRAIN_DOOR_1),true);
										break;
                                    case PORTAL_PHASE_LICH_KING:
										instance->HandleGameObject(instance->GetData64(TYPE_BRAIN_DOOR_2),true);
										break;
                                    case PORTAL_PHASE_KING_LLIANE:
										instance->HandleGameObject(instance->GetData64(TYPE_BRAIN_DOOR_3),true);
										break;
                                    }
                                    currentBrainEventPhase = PORTAL_PHASE_BRAIN_NONE;
                                    IsEventSpeaking = false;
                                    if (Creature* yogg = me->GetCreature(*me, guidYogg))
                                        me->AddAura(SPELL_SHATTERED_ILLUSIONS,yogg);
                                    me->AddAura(SPELL_SHATTERED_ILLUSIONS,me);

                                    if (Creature* yoggbrain = me->GetCreature(*me, guidYoggBrain))
                                        yoggbrain->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                                    for (std::list<uint64>::iterator itr = Summons.begin(); itr != Summons.end(); ++itr)
                                    {
                                        if (Creature* temp = Creature::GetCreature(*me, *itr))
                                        {
                                            switch(temp->GetEntry())
                                            {
                                            case NPC_CONSTRICTOR_TENTACLE:
                                            case NPC_CORRUPTOR_TENTACLE:
                                            case NPC_CRUSHER_TENTACLE:
                                                    me->AddAura(SPELL_SHATTERED_ILLUSIONS,temp);
                                                break;
                                            }
                                        }
                                    }
                                    DoKillAndDespawnGUIDs(guidEventSkulls);
                                }
                            }
                        }
                    }
                    break;
                case PHASE_YOGG:
                    if (GuardianSummonTimer <= diff)
                    {
                        if (Creature* yogg = me->GetCreature(*me, guidYogg))
                            yogg->CastSpell(yogg, SPELL_SUMMON_IMMORTAL_GUARDIAN, true);

                        LastGuardianSummonTimer = LastGuardianSummonTimer-5000 <= 10000 ? 10000 : LastGuardianSummonTimer - 5000;
                        GuardianSummonTimer = LastGuardianSummonTimer;
                    }
					else GuardianSummonTimer -= diff;

                    if (LunaticGazeTimer <= diff)
                    {
                        if (Creature* yogg = me->GetCreature(*me, guidYogg))
                        {
                            if (!yogg->IsNonMeleeSpellCasted(false))
                            {
                                DoSendQuantumText(SAY_LUNCATIC_GAZE, yogg, 0);
                                yogg->CastSpell(yogg, RAID_MODE(SPELL_LUNATIC_GAZE_10, SPELL_LUNATIC_GAZE_25), false);
                                LunaticGazeTimer = 12000;
                            }
                        }
                    }
					else LunaticGazeTimer -= diff;

                    if (GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL && AmountKeeperActive < 4)
					{
                        if (DeafeningRoarTimer <= diff)
                        {
                            if (Creature* yogg = me->GetCreature(*me, guidYogg))
                            {
                                if (!yogg->IsNonMeleeSpellCasted(false))
                                {
                                    DoSendQuantumText(SAY_DEAFENING_ROAR, yogg, 0);
                                    yogg->CastSpell(yogg, SPELL_DEAFENING_ROAR, false);
                                    DeafeningRoarTimer = 60000;
                                }
                            }
                        }
						else DeafeningRoarTimer -= diff;
					}

                    if (ShadowBeaconTimer <= diff)
                    {
                        if (Creature* yogg = me->GetCreature(*me, guidYogg))
                        {
                            if (!yogg->IsNonMeleeSpellCasted(false))
                            {
                                yogg->CastSpell(yogg, SPELL_SHADOW_BEACON, false);
                                ShadowBeaconTimer = 30000;
                            }
                        }
                    }
					else ShadowBeaconTimer -= diff;
                    break;
            }

            if (phase != PHASE_NONE)
            {
                if (EnrageTimer <= diff)
                {
                    if (Creature* yogg = me->GetCreature(*me, guidYogg))
                    {
                        yogg->InterruptNonMeleeSpells(false);
                        yogg->CastSpell(yogg, SPELL_EXTINGUISH_ALL_LIFE, false);
                    }
                    EnrageTimer = 5000;
                }
				else EnrageTimer -= diff;
            }

            if (phase != PHASE_NONE && !IsEncounterInProgress())
			{
				me->DisappearAndDie();
				me->Respawn();
			}

            // Temporary
            if (phase == PHASE_NONE && me->IsInCombatActive())
			{
				me->DisappearAndDie();
				me->Respawn();
			}
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_saraAI(creature);
    }
};

class npc_ominous_cloud : public CreatureScript
{
public:
    npc_ominous_cloud() : CreatureScript("npc_ominous_cloud") { }

    struct npc_ominous_cloudAI : public QuantumBasicAI
    {
        npc_ominous_cloudAI(Creature* creature) : QuantumBasicAI(creature)
        {
            SetImmuneToPushPullEffects(true);
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        void MoveInLineOfSight(Unit* target)
        {
            if (instance->GetBossState(DATA_YOGG_SARON) == IN_PROGRESS)
			{
                if (target && me->GetDistance2d(target) <= 5 && target->ToPlayer() && !target->ToPlayer()->IsGameMaster() && !target->HasAura(SPELL_FLASH_FREEZE))
                    TriggerGuardianSpawn();
			}
        }

        void AttackStart(Unit* /*attacker*/) {}

        void DoAction(const int32 action)
        {
            switch(action)
            {
            case ACTION_DEACTIVATE_CLOUDS:
                me->RemoveAurasDueToSpell(SPELL_SUMMON_GUARDIAN);
                me->RemoveAurasDueToSpell(SPELL_OMINOUS_CLOUD_EFFECT);
                me->SetReactState(REACT_PASSIVE);
                break;
            case ACTION_ACTIVATE_CLOUDS:
                Reset();
                me->SetReactState(REACT_AGGRESSIVE);
                break;
            }
        }

        void TriggerGuardianSpawn()
        {
            if (!me->HasAura(SPELL_SUMMON_GUARDIAN))
                DoCast(me, SPELL_SUMMON_GUARDIAN, true);
        }

        void Reset()
        {
            DoCast(me, SPELL_OMINOUS_CLOUD_EFFECT, true);
            me->RemoveAurasDueToSpell(SPELL_SUMMON_GUARDIAN);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->GetMotionMaster()->MoveRandom(25.0f);
        }

        void UpdateAI(const uint32 /*diff*/) {}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ominous_cloudAI(creature);
    }
};

class npc_guardian_of_yogg_saron : public CreatureScript
{
public:
    npc_guardian_of_yogg_saron() : CreatureScript("npc_guardian_of_yogg_saron") { }

    struct npc_guardian_of_yogg_saronAI : public QuantumBasicAI
    {
        npc_guardian_of_yogg_saronAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            me->SetCurrentFaction(FACTION_HOSTILE);
        }

        InstanceScript* instance;
        uint32 DarkVolleyTimer;

        void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            if (target && target->ToPlayer())
            {
                switch(spell->Id)
                {
                case SPELL_DARK_VOLLEY:
                    if (Creature* cSara = me->GetCreature(*me,instance->GetData64(DATA_SARA)))
                        CAST_AI(boss_sara::boss_saraAI,cSara->AI())->ModifySanity(target->ToPlayer(),-2);
                    break;
                }
            }
        }

        Unit* SelectPlayerTargetInRange(float range)
        {
            Player* target = NULL;
            Trinity::AnyPlayerInObjectRangeCheck u_check(me, range, true);
            Trinity::PlayerSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(me, target, u_check);
            me->VisitNearbyObject(range, searcher);
            return target;
        }

        void JustDied(Unit* /*killer*/)
        {
			DoCastAOE(RAID_MODE(SPELL_SHADOW_NOVA_10, SPELL_SHADOW_NOVA_25));
			DoCastAOE(SPELL_SHADOW_NOVA_2);

			if (Creature* cSara = me->GetCreature(*me,instance->GetData64(DATA_SARA)))
			{
				if (me->GetDistance2d(cSara) <= 15)
					cSara->AI()->DoAction(ACTION_NOVA_HIT);
			}

            me->DespawnAfterAction(5*IN_MILLISECONDS);
        }

        void Reset()
        {
            DarkVolleyTimer = urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS);

			if (Unit* target = SelectPlayerTargetInRange(100.0f))
				me->AI()->AttackStart(target);
        }

        void UpdateAI(const uint32 diff)
        {
            if (instance->GetBossState(DATA_YOGG_SARON) != IN_PROGRESS)
            {
                me->DealDamage(me,me->GetMaxHealth());
                me->RemoveCorpse();
            }

            if (!UpdateVictim())
                return;

            if (DarkVolleyTimer <= diff)
            {
                DoCast(SPELL_DARK_VOLLEY);
                DarkVolleyTimer = urand(20000,30000);
            }
			else DarkVolleyTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_guardian_of_yogg_saronAI(creature);
    }
};

class npc_yogg_saron_tentacle : public CreatureScript
{
public:
    npc_yogg_saron_tentacle() : CreatureScript("npc_yogg_saron_tentacle") { }

    struct npc_yogg_saron_tentacleAI : public QuantumBasicAI
    {
        npc_yogg_saron_tentacleAI(Creature* creature) : QuantumBasicAI(creature)
        {
			SetCombatMovement(false);
            instance = creature->GetInstanceScript();
            SetTentacleType(creature->GetEntry());
            once = false;
            me->SetCurrentFaction(FACTION_HOSTILE);
            me->SetCanFly(true);

			if (Creature* sara = Creature::GetCreature(*me,instance->GetData64(DATA_SARA)))
				sara->AI()->JustSummoned(me);
        }

        InstanceScript* instance;
        TentacleType t_Type;

        bool once;

        uint32 TentacleSpellTimer;

        void SetTentacleType(uint32 entry)
        {
            switch(entry)
            {
                case NPC_CRUSHER_TENTACLE:
					t_Type = CRUSHER_TENTACLE;
					break;
				case NPC_CORRUPTOR_TENTACLE:
					t_Type = CORRUPTOR_TENTACLE;
					break;
				case NPC_CONSTRICTOR_TENTACLE:
					t_Type = CONSTRICTOR_TENTACLE;
					break;
				default:
					t_Type = CORRUPTOR_TENTACLE;
					break;
            }
        }

		void Reset()
        {
            if (Unit* who = SelectPlayerTargetInRange(500.0f))
                AttackStartNoMove(who);

			me->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);

            DoCast(me, SPELL_TENTACLE_VOID_ZONE, true);
        }

        void MoveInLineOfSight(Unit* who)
        {
            if (who && me->GetDistance2d(who) <= me->GetMeleeReach() && who->ToPlayer() && !who->ToPlayer()->IsGameMaster())
                AttackStartNoMove(who);
        }

        void AttackStart(Unit* /*target*/) {}

        Unit* SelectPlayerTargetInRange(float range)
        {
            Player* target = NULL;
            Trinity::AnyPlayerInObjectRangeCheck u_check(me, range, true);
            Trinity::PlayerSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(me, target, u_check);
            me->VisitNearbyObject(range, searcher);
            return target;
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoZoneInCombat();

            TentacleSpellTimer = urand(5000, 10000);

            switch(t_Type)
            {
                case CRUSHER_TENTACLE:
					DoCast(me, SPELL_CRUSH, true);
					DoCast(SPELL_FOCUS_ANGER);
					break;
				case CONSTRICTOR_TENTACLE:
					TentacleSpellTimer = urand(20000, 30000);
					break;
            }
        }

		void PassengerBoarded(Unit* passenger, int8 /*seatId*/, bool apply)
        {
            if (apply)
                passenger->CastSpell(passenger, RAID_MODE(SPELL_SQUEEZE_10, SPELL_SQUEEZE_25), true, NULL, NULL, me->GetGUID());
            else
                passenger->RemoveAurasDueToSpell(RAID_MODE(SPELL_SQUEEZE_10, SPELL_SQUEEZE_25));
        }

		void JustDied(Unit* /*killer*/)
        {
            me->RemoveAurasDueToSpell(SPELL_TENTACLE_VOID_ZONE);
            me->DespawnAfterAction(5*IN_MILLISECONDS);
        }

        void UpdateAI(const uint32 diff)
        {
            if (instance->GetBossState(DATA_YOGG_SARON) != IN_PROGRESS)
            {
                me->DealDamage(me,me->GetMaxHealth());
                me->RemoveCorpse();
            }

            if (!once)
            {
                DoCast(SPELL_ERUPT);
                once = true;
            }

            if (!UpdateVictim())
                return;

            if (TentacleSpellTimer <= diff)
            {
                switch(t_Type)
                {
                    case CRUSHER_TENTACLE:
						if (!me->HasAura(SPELL_DIMISH_POWER))
							DoCast(SPELL_DIMISH_POWER);
						break;
					case CORRUPTOR_TENTACLE:
						if (Unit* target = SelectPlayerTargetInRange(500.0f))
							DoCast(target, RAND(SPELL_DRAINING_POISON, SPELL_BLACK_PLAGUE, SPELL_APATHY, SPELL_CURSE_OF_DOOM));
						break;
					case CONSTRICTOR_TENTACLE:
						if (Player* player = me->FindPlayerWithDistance(65.0f, true))
						{
							if (!player->HasAuraEffect(SPELL_LUNGE, 0))
								player->EnterVehicle(me);
						}
						break;
                }
                TentacleSpellTimer = urand(5000, 7000);
            }
			else TentacleSpellTimer -= diff;

            if (t_Type == CRUSHER_TENTACLE)
                DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_yogg_saron_tentacleAI(creature);
    }
};

class npc_descend_into_madness : public CreatureScript
{
public:
    npc_descend_into_madness() : CreatureScript("npc_descend_into_madness") { }

    bool OnGossipHello(Player* player,Creature* creature)
    {
        player->PlayerTalkClass->ClearMenus();

        bool AcceptTeleport = false;
        Position posTeleportPosition;
        BrainEventPhase pTemp = PORTAL_PHASE_BRAIN_NONE;
        if (creature && creature->AI())
            pTemp = CAST_AI(npc_descend_into_madnessAI, creature->AI())->bPhase;

        switch(pTemp)
        {
            case PORTAL_PHASE_KING_LLIANE:
            case PORTAL_PHASE_DRAGON_SOUL:
            case PORTAL_PHASE_LICH_KING:
				AcceptTeleport = true;
				posTeleportPosition = InnerBrainLocation[pTemp];
				break;
			default:
				AcceptTeleport = false;
				break;
        }

        if (AcceptTeleport)
        {
            if (player)
            {
                player->RemoveAurasDueToSpell(SPELL_BRAIN_LINK);
                creature->AddAura(SPELL_ILLUSION_ROOM, player);
                player->NearTeleportTo(posTeleportPosition.m_positionX, posTeleportPosition.m_positionY, posTeleportPosition.m_positionZ, posTeleportPosition.m_orientation, true);
                CAST_AI(npc_descend_into_madnessAI, creature->AI())->bPhase = PORTAL_PHASE_BRAIN_NONE;
                AcceptTeleport = false;
                player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET_2, SPELL_ILLUSION_ROOM);
                creature->DespawnAfterAction(0.5*IN_MILLISECONDS);
            }
        }

        return true;
    };

    struct npc_descend_into_madnessAI : public QuantumBasicAI
    {
        npc_descend_into_madnessAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        BrainEventPhase bPhase;

        void DoAction(const int32 action)
        {
            me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            switch(action)
            {
                case ACTION_PORTAL_TO_MADNESS_STORMWIND:
                    bPhase = PORTAL_PHASE_KING_LLIANE;
                    break;
                case ACTION_PORTAL_TO_MADNESS_DRAGON:
                    bPhase = PORTAL_PHASE_DRAGON_SOUL;
                    break;
                case ACTION_PORTAL_TO_MADNESS_LICHKING:
                    bPhase = PORTAL_PHASE_LICH_KING;
                    break;
            }
        }

        void Reset()
        {
            bPhase = PORTAL_PHASE_BRAIN_NONE;
        }

        void UpdateAI(uint32 const /*diff*/) {}
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_descend_into_madnessAI(creature);
    }
};

class npc_brain_of_yogg_saron : public CreatureScript
{
public:
    npc_brain_of_yogg_saron() : CreatureScript("npc_brain_of_yogg_saron") { }

    struct npc_brain_of_yogg_saronAI : public QuantumBasicAI
    {
        npc_brain_of_yogg_saronAI(Creature* creature) : QuantumBasicAI(creature)
        {
			SetCombatMovement(false);
            instance = creature->GetInstanceScript();
            me->SetReactState(REACT_PASSIVE);
            me->SetCurrentFaction(FACTION_HOSTILE);
            me->SetCanFly(true);
        }

        void DamageTaken(Unit* /*dealer*/, uint32 &damage)
        {
            if (damage > me->GetHealth())
                damage = me->GetHealth()-1;
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            if (!target->ToPlayer())
                return;

            if (spell->Id == SPELL_INDUCE_MADNESS)
            {
                if (IsPlayerInBrainRoom(target->ToPlayer()))
                    target->RemoveAurasDueToSpell(SPELL_SANITY);
            }
        }

        InstanceScript* instance;

        void UpdateAI(uint32 const /*diff*/)
        {
            if (HealthBelowPct(HEALTH_PERCENT_30))
            {
                if (Creature* cSara = me->GetCreature(*me,instance->GetData64(DATA_SARA)))
                        cSara->AI()->DoAction(ACTION_BRAIN_UNDER_30_PERCENT);

                if (!me->HasAura(SPELL_BRAIN_HURT_VISUAL))
                    DoCast(me, SPELL_BRAIN_HURT_VISUAL, true);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_brain_of_yogg_saronAI(creature);
    }
};

class boss_yogg_saron : public CreatureScript
{
public:
    boss_yogg_saron() : CreatureScript("boss_yogg_saron") { }

    struct boss_yogg_saronAI : public QuantumBasicAI
    {
        boss_yogg_saronAI(Creature* creature) : QuantumBasicAI(creature)
        {
			SetCombatMovement(false);
            instance = creature->GetInstanceScript();
            me->SetReactState(REACT_PASSIVE);
			me->SetUnitMovementFlags(MOVEMENTFLAG_DISABLE_GRAVITY | MOVEMENTFLAG_SWIMMING);
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
        }

        InstanceScript* instance;

        uint32 SanityCheckTimer;

        bool UsedMindControll;

        void Reset()
        {
            SanityCheckTimer = 1000;
            UsedMindControll = false;
        }

		void QuestReward()
		{
			Map::PlayerList const& Players = me->GetMap()->GetPlayers();
			for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
			{
				if (Player* player = itr->getSource())
				{
					if (Is25ManRaid())
					{
						if (player->GetQuestStatus(QUEST_CONQUEROR_OF_ULDUAR_A) == QUEST_STATUS_INCOMPLETE || player->GetQuestStatus(QUEST_CONQUEROR_OF_ULDUAR_H) == QUEST_STATUS_INCOMPLETE)
							player->AddItem(ITEM_ID_TABLETS_OF_YOGG, 1);
					}
				}
			}
		}

        void JustDied(Unit* /*killer*/)
        {
            if (Creature* Sara = me->GetCreature(*me, instance->GetData64(DATA_SARA)))
                Sara->AI()->DoAction(ACTION_YOGGSARON_KILLED);

            DoSendQuantumText(SAY_DEATH, me);

			//QuestReward();
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            if (!instance)
				return;

            if (target && target->ToPlayer())
            {
                switch(spell->Id)
                {
					case SPELL_LUNATIC_GAZE_EFFECT_10:
						if (Creature* Sara = me->GetCreature(*me, instance->GetData64(DATA_SARA)))
							CAST_AI(boss_sara::boss_saraAI, Sara->AI())->ModifySanity(target->ToPlayer(), -2);
						break;
                    case SPELL_LUNATIC_GAZE_EFFECT_25:
						if (Creature* Sara = me->GetCreature(*me, instance->GetData64(DATA_SARA)))
							CAST_AI(boss_sara::boss_saraAI, Sara->AI())->ModifySanity(target->ToPlayer(), -4);
						break;
                }
            }
        }

        void SpellHit(Unit* /*caster*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_IN_THE_MAWS_OF_THE_OLD_GOD)
                me->AddLootMode(LOOT_MODE_HARD_MODE_4);
        }

        void UpdateAI(const uint32 diff)
        {
            if (instance->GetBossState(DATA_GENERAL_VEZAX) == IN_PROGRESS)
                return;

            if (SanityCheckTimer <= diff && instance->GetBossState(DATA_GENERAL_VEZAX) == DONE)
            {
                if (me->GetMap() && me->GetMap()->IsDungeon())
                {
                    Map::PlayerList const& players = me->GetMap()->GetPlayers();
                    if (!players.isEmpty())
					{
                        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
						{
                            if (Player* player = itr->getSource())
							{
                                if (player->IsAlive() && !player->IsGameMaster())
								{
                                    if (!player->HasAura(SPELL_SANITY) && !player->HasAura(SPELL_INSANE))
                                    {
                                        if (player->GetSession()->GetSecurity() > SEC_PLAYER)
                                            continue;

                                        if (!UsedMindControll)
                                        {
                                            if (Creature* Sara = me->GetCreature(*me, instance->GetData64(DATA_SARA)))
                                                Sara->AI()->DoAction(ACTION_USED_MIND_CONTROL);

                                            UsedMindControll = true;
                                        }

                                        if (IsPlayerInBrainRoom(player))
                                        {
                                            player->RemoveAurasDueToSpell(SPELL_ILLUSION_ROOM);
                                            player->NearTeleportTo(SaraLocation.GetPositionX(), SaraLocation.GetPositionY(), SaraLocation.GetPositionZ(), M_PI, false);
                                        }

                                        me->AddAura(SPELL_INSANE, player);
                                        me->AddAura(SPELL_INSANE_2, player);
                                        DoSendQuantumText(RAND(WHISP_INSANITY_1, WHISP_INSANITY_2), me, player);
                                    }
								}
							}
						}
					}
                }

                SanityCheckTimer = 1000;
            }
			else SanityCheckTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_yogg_saronAI(creature);
    }
};

class npc_influence_tentacle : public CreatureScript
{
public:
    npc_influence_tentacle() : CreatureScript("npc_influence_tentacle") { }

    struct npc_influence_tentacleAI : public QuantumBasicAI
    {
        npc_influence_tentacleAI(Creature* creature) : QuantumBasicAI(creature)
        {
			SetCombatMovement(false);
            me->SetReactState(REACT_DEFENSIVE);
        }

        void JustDied(Unit* /*killer*/)
        {
            me->RemoveAurasDueToSpell(SPELL_TENTACLE_VOID_ZONE);
        }

        void Reset(){}

        void DamageTaken(Unit* attacker, uint32 &damage)
        {
            if (attacker->ToPlayer())
                me->CastCustomSpell(SPELL_GRIM_REPRISAL_DAMAGE, SPELLVALUE_BASE_POINT0, int32(damage *0.60), attacker,true);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            if (me->GetEntry() != NPC_INFULENCE_TENTACLE)
            {
                me->UpdateEntry(NPC_INFULENCE_TENTACLE);
                me->SetCurrentFaction(FACTION_HOSTILE);
                me->SetRegeneratingHealth(false);
                DoCast(SPELL_GRIM_REPRISAL);

                DoCast(me, SPELL_TENTACLE_VOID_ZONE, true);
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_influence_tentacleAI(creature);
    }
};

class npc_immortal_guardian : public CreatureScript
{
public:
    npc_immortal_guardian() : CreatureScript("npc_immortal_guardian") { }

    struct npc_immortal_guardianAI : public QuantumBasicAI
    {
        npc_immortal_guardianAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            me->SetCurrentFaction(FACTION_HOSTILE);
            SetImmuneToPushPullEffects(true);
        }

        InstanceScript* instance;

        uint32 DrainLifeTimer;

        Unit* SelectPlayerTargetInRange(float range)
        {
            Player *target = NULL;
            Trinity::AnyPlayerInObjectRangeCheck u_check(me, range, true);
            Trinity::PlayerSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(me, target, u_check);
            me->VisitNearbyObject(range, searcher);
            return target;
        }

        void Reset()
        {
            if (Unit* target = SelectPlayerTargetInRange(100.0f))
                me->AI()->AttackStart(target);

            DrainLifeTimer = 10000;
        }

        void JustDied(Unit* /*killer*/)
        {
            me->DespawnAfterAction(5*IN_MILLISECONDS);
        }

        void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
        {
            if (spell->Id == 64172) // Titanic Storm
            {
                if (me->HasAura(SPELL_WEAKENED))
                    me->DealDamage(me,me->GetHealth());
            }
        }

        void DamageTaken(Unit* dealer, uint32 &damage)
        {
            if (dealer->GetGUID() == me->GetGUID())
                return;

            if (me->GetHealth() < damage)
                damage = me->GetHealth()-1;
        }

        void UpdateAI(const uint32 diff)
        {
            if (instance->GetBossState(DATA_GENERAL_VEZAX) != IN_PROGRESS)
            {
                me->DealDamage(me, me->GetMaxHealth());
                me->RemoveCorpse();
            }

            {
                int8 stacks = int8(uint32(me->GetHealthPct()) / 10);
                if (stacks > 9) stacks = 9;

                if (stacks > 0)
                {
                    me->RemoveAurasDueToSpell(SPELL_WEAKENED);
                    if (!me->HasAura(SPELL_EMPOWERED))
                        me->AddAura(SPELL_EMPOWERED, me);
                    me->SetAuraStack(SPELL_EMPOWERED, me, stacks);
                }
				else
                {
                    me->RemoveAurasDueToSpell(SPELL_EMPOWERED);
                    if (!me->HasAura(SPELL_WEAKENED))
                        me->AddAura(SPELL_WEAKENED,me);
                }
            }

            if (instance->GetBossState(DATA_GENERAL_VEZAX) != IN_PROGRESS)
                return;

            if (!UpdateVictim())
                return;

            if (DrainLifeTimer < diff)
            {
                DoCastVictim(RAID_MODE(SPELL_DRAIN_LIFE_10, SPELL_DRAIN_LIFE_25));
                DrainLifeTimer = 35000;
            }
			else DrainLifeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_immortal_guardianAI(creature);
    }
};

class AllSaronitCreaturesInRange
{
    public:
        AllSaronitCreaturesInRange(const WorldObject* pObject, float fMaxRange) : m_pObject(pObject), m_fRange(fMaxRange) {}
        bool operator() (Unit* pUnit)
        {
            if (IsSaronitEntry(pUnit->GetEntry()) && m_pObject->IsWithinDist(pUnit, m_fRange,false) && pUnit->IsAlive())
                return true;

            return false;
        }

    private:
        const WorldObject* m_pObject;
        float m_fRange;

        bool IsSaronitEntry(uint32 entry)
        {
            switch(entry)
            {
            case NPC_GUARD_OF_YOGG_SARON:
            case NPC_CONSTRICTOR_TENTACLE:
            case NPC_CORRUPTOR_TENTACLE:
            case NPC_CRUSHER_TENTACLE:
            case NPC_IMMORTAL_GUARDIAN:
                return true;
            }
            return false;
        }
};

class npc_support_keeper : public CreatureScript
{
public:
    npc_support_keeper() : CreatureScript("npc_support_keeper") { }

    struct npc_support_keeperAI : public QuantumBasicAI
    {
        npc_support_keeperAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
			SetCombatMovement(false);
            instance = creature->GetInstanceScript();
        }

        SummonList Summons;
        InstanceScript* instance;

        uint32 SecondarySpellTimer;

        bool summoning;
        bool summoned;

        void AttackStart(Unit *attacker) {}

        void Reset()
        {
            uint32 auraspell = 0;
            switch(me->GetEntry())
            {
                case NPC_KEEPER_HODIR:
					auraspell = SPELL_FORTITUDE_OF_FROST;
					break;
				case NPC_KEEPER_FREYA:
					auraspell = SPELL_RESILIENCE_OF_NATURE;
					break;
				case NPC_KEEPER_THORIM:
					auraspell = SPELL_FURY_OF_THE_STORM;
					break;
				case NPC_KEEPER_MIMIRON:
					auraspell = SPELL_SPEED_OF_INVENTION;
					break;
            }
            DoCast(auraspell);

            summoning = false;
            summoned = false;

            SecondarySpellTimer = 10000;
        }

        void JustSummoned(Creature* summoned)
        {
            Summons.Summon(summoned);
        }

        void DoSummonSanityWells()
        {
            for (uint8 i = 0; i < 5 ; i++)
                DoSummon(NPC_SANITY_WELL,FreyaSanityWellLocation[i], 0, TEMPSUMMON_MANUAL_DESPAWN);
        }

        void GetAliveSaronitCreatureListInGrid(std::list<Creature*>& lList, float fMaxSearchRange)
        {
            CellCoord pair(Trinity::ComputeCellCoord(me->GetPositionX(), me->GetPositionY()));
            Cell cell(pair);
            cell.SetNoCreate();

            AllSaronitCreaturesInRange check(me, fMaxSearchRange);
            Trinity::CreatureListSearcher<AllSaronitCreaturesInRange> searcher(me, lList, check);
            TypeContainerVisitor<Trinity::CreatureListSearcher<AllSaronitCreaturesInRange>, GridTypeMapContainer> visitor(searcher);

            cell.Visit(pair, visitor, *(me->GetMap()), *me, SIZE_OF_GRIDS);
        }

        void UpdateAI(const uint32 diff)
        {
            if (SecondarySpellTimer <= diff)
            {
                switch(me->GetEntry())
                {
                case NPC_KEEPER_THORIM:
                    if (!me->HasAura(SPELL_TITANIC_STORM))
                        DoCast(SPELL_TITANIC_STORM);

                    SecondarySpellTimer = 10000;
                    return;
                case NPC_KEEPER_FREYA:
                    if (!summoned)
                    {
                        if (!summoning)
                        {
                            DoCast(SPELL_SANITY_WELL);
                            SecondarySpellTimer = 3000;
                            summoning = true;
                        }
						else
                        {
                            DoSummonSanityWells();
                            summoned = true;
                        }
                    }
                    return;
                case NPC_KEEPER_MIMIRON:
                    {
                        std::list<Creature*> creatureList;
                        GetAliveSaronitCreatureListInGrid(creatureList,5000);
                        if (!creatureList.empty())
                        {
                            std::list<Creature*>::iterator itr = creatureList.begin();
                            advance(itr, urand(0, creatureList.size()-1));
                            DoCast((*itr),SPELL_DESTABILIZATION_MATRIX,false);
                        }
                    }
                    SecondarySpellTimer = 30000;
                    return;
                case NPC_KEEPER_HODIR:
                    {
                        if (!me->HasAura(SPELL_HODIRS_PROTECTIVE_GAZE))
                            DoCast(me,SPELL_HODIRS_PROTECTIVE_GAZE,false);
                        SecondarySpellTimer = 25000;
                    }
                }
                SecondarySpellTimer = 10000;
            }
			else
            {
                if (me->GetEntry() == NPC_KEEPER_HODIR)
                {
                    if (!me->HasAura(SPELL_HODIRS_PROTECTIVE_GAZE))
                        SecondarySpellTimer -= diff;
                }
				else SecondarySpellTimer -= diff;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_support_keeperAI(creature);
    }
};

class npc_sanity_well : public CreatureScript
{
public:
    npc_sanity_well() : CreatureScript("npc_sanity_well") { }

    struct npc_sanity_wellAI : public QuantumBasicAI
    {
        npc_sanity_wellAI(Creature* creature) : QuantumBasicAI(creature)
        {
			SetCombatMovement(false);
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 SanityTickTimer;

        void Reset()
        {
            DoCast(SPELL_SANITY_WELL_OPTIC);
            SanityTickTimer = 2000;
        }

        void AttackStart(Unit* /*who*/) {}

        void MoveInLineOfSight(Unit* who)
        {
            if (who && who->ToPlayer())
			{
                if (me->IsWithinDist2d(who, 6.0f))
                {
                    if (!who->HasAura(SPELL_SANITY_WELL_DEBUFF))
                        me->AddAura(SPELL_SANITY_WELL_DEBUFF, who);
                }
				else
                {
                    if (who->HasAura(SPELL_SANITY_WELL_DEBUFF))
                        who->RemoveAurasDueToSpell(SPELL_SANITY_WELL_DEBUFF);
                }
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (instance->GetBossState(DATA_GENERAL_VEZAX) != IN_PROGRESS)
            {
                me->DealDamage(me, me->GetMaxHealth());
                me->RemoveCorpse();
            }

            if (SanityTickTimer <= diff)
            {
                std::list<Player*> plrList = me->GetNearestPlayersList(10);
                for (std::list<Player*>::const_iterator itr = plrList.begin(); itr != plrList.end(); ++itr)
                {
                    if ((*itr))
                    {
                        if ((*itr)->HasAura(SPELL_SANITY_WELL_DEBUFF))
                        {
                            if (Creature* sara = Creature::GetCreature((*me),instance->GetData64(DATA_SARA)))
                                CAST_AI(boss_sara::boss_saraAI,sara->AI())->ModifySanity((*itr), 10);
                        }
                    }
                }
                SanityTickTimer = 2000;
            }
			else SanityTickTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sanity_wellAI(creature);
    }
};

class npc_laughting_skull : public CreatureScript
{
public:
    npc_laughting_skull() : CreatureScript("npc_laughting_skull") { }

    struct npc_laughting_skullAI : public QuantumBasicAI
    {
        npc_laughting_skullAI(Creature* creature) : QuantumBasicAI(creature)
        {
			SetCombatMovement(false);
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        void Reset()
        {
            DoCast(RAID_MODE(SPELL_LUNATIC_GAZE_10, SPELL_LUNATIC_GAZE_25));
        }

        void SpellHitTarget(Unit* target, const SpellInfo* spell)
        {
            if (!instance) return;

            if (target && target->ToPlayer())
            {
                switch(spell->Id)
                {
                    case SPELL_LUNATIC_GAZE_EFFECT_10:
						if (Creature* sara = me->GetCreature(*me,instance->GetData64(DATA_SARA)))
							CAST_AI(boss_sara::boss_saraAI, sara->AI())->ModifySanity(target->ToPlayer(), -2);
						break;
					case SPELL_LUNATIC_GAZE_EFFECT_25:
						if (Creature* sara = me->GetCreature(*me,instance->GetData64(DATA_SARA)))
							CAST_AI(boss_sara::boss_saraAI, sara->AI())->ModifySanity(target->ToPlayer(), -4);
						break;
                }
            }
        }

        void AttackStart(Unit* /*who*/){}

        void UpdateAI(uint32 const /*diff*/){}
    };
	
	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_laughting_skullAI(creature);
    }
};

class npc_death_orb : public CreatureScript
{
public:
    npc_death_orb() : CreatureScript("npc_death_orb") {}

    struct npc_death_orbAI : public QuantumBasicAI
    {
        npc_death_orbAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
			SetCombatMovement(false);
            instance = creature->GetInstanceScript();
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        }

        SummonList Summons;
        InstanceScript* instance;

        bool Prepared;

        uint32 DeathRayEffectTimer;

        void Reset()
        {
            me->SetCanFly(true);

            me->SetCurrentFaction(FACTION_HOSTILE);

            Prepared = false;

            Summons.DespawnAll();

            SummonDeathRays();

            DeathRayEffectTimer = 5000;

            DoCast(me, SPELL_DEATH_RAY_ORIGIN_VISUAL, true);
        }

        void SummonDeathRays()
        {
            for (uint8 i = 0; i < 4; i++)
            {
                Position pos;
                me->GetNearPoint2D(pos.m_positionX, pos.m_positionY, float(rand_norm()*10 + 30), float(2*M_PI*rand_norm()));
                pos.m_positionZ = me->GetPositionZ();
                pos.m_positionZ = me->GetMap()->GetHeight(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), true, 500.0f);

                DoSummon(NPC_DEATH_RAY, pos, 20000, TEMPSUMMON_TIMED_DESPAWN);
            }
        }

        void JustSummoned(Creature* summoned)
        {
            switch(summoned->GetEntry())
            {
            case NPC_DEATH_RAY:
                summoned->SetCurrentFaction(FACTION_HOSTILE);
                summoned->CastSpell(summoned, SPELL_DEATH_RAY_WARNING_VISUAL, true, 0, 0, me->GetGUID());
                summoned->SetReactState(REACT_PASSIVE);
                summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                break;
            }

            Summons.Summon(summoned);
        }

        void AttackStart(Unit* /*who*/) {}

        void UpdateAI(const uint32 diff)
        {
            if (DeathRayEffectTimer <= diff)
            {
                for (std::list<uint64>::iterator itr = Summons.begin(); itr != Summons.end(); ++itr)
                {
                    if (Creature* temp = me->GetCreature(*me,*itr))
                    {
                        temp->CastSpell(temp, SPELL_DEATH_RAY_PERIODIC, true);
                        temp->CastSpell(temp, SPELL_DEATH_RAY_DAMAGE_VISUAL, true, 0, 0, me->GetGUID());

                        temp->AI()->DoAction(ACTION_DEATH_RAY_MOVE);
                    }
                }
                DeathRayEffectTimer = 30000;
            }
			else DeathRayEffectTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_death_orbAI(creature);
    }
};

class npc_death_ray : public CreatureScript
{
public:
    npc_death_ray() : CreatureScript("npc_death_ray") {}

    struct npc_death_rayAI : public QuantumBasicAI
    {
        npc_death_rayAI(Creature* creature) : QuantumBasicAI(creature){}

        bool moving;

        void Reset()
        {
            moving = true;
        }

        void DoAction(const int32 action)
        {
            if (action == ACTION_DEATH_RAY_MOVE)
                moving = false;
        }

        void DoRandomMove()
        {
            Position pos;
            me->GetNearPoint2D(pos.m_positionX, pos.m_positionY,10,float(2*M_PI*rand_norm()));
            pos.m_positionZ = me->GetPositionZ();
            pos.m_positionZ = me->GetMap()->GetHeight(pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), true, 50.0f);
            me->SetUnitMovementFlags(MOVEMENTFLAG_WALKING);
            me->GetMotionMaster()->MovePoint(1, pos);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id != 1)
                return;

            moving = false;
        }

        void AttackStart(Unit* /*who*/) {}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!moving)
            {
                DoRandomMove();
                moving = true;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_death_rayAI(creature);
    }
};

class go_flee_to_surface : public GameObjectScript
{
public:
    go_flee_to_surface() : GameObjectScript("go_flee_to_surface") {}

    bool OnGossipHello(Player* player, GameObject* /*go*/)
    {
        player->RemoveAurasDueToSpell(SPELL_ILLUSION_ROOM);
        player->NearTeleportTo(SaraLocation.GetPositionX(), SaraLocation.GetPositionY(), SaraLocation.GetPositionZ(), M_PI, false);
        player->JumpTo(40.0f,15.0f, true);
        return false;
    }
};

class spell_keeper_support_aura_targeting : public SpellScriptLoader
{
public:
	spell_keeper_support_aura_targeting() : SpellScriptLoader("spell_keeper_support_aura_targeting") { }

    class spell_keeper_support_aura_targeting_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_keeper_support_aura_targeting_AuraScript);

        void HandleEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            std::list<Unit*> targetList;
            aurEff->GetTargetList(targetList);

            for (std::list<Unit*>::iterator itr = targetList.begin(); itr != targetList.end(); ++itr)
			{
                if (!(*itr)->ToPlayer() && (*itr)->GetGUID() != GetCasterGUID())
                    (*itr)->RemoveAurasDueToSpell(GetSpellInfo()->Id);
			}
        }

        void Register()
        {
            OnEffectApply += AuraEffectApplyFn(spell_keeper_support_aura_targeting_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_keeper_support_aura_targeting_AuraScript();
    }
};

class DontLooksDirectlyInGazeCheck
{
    public:
        DontLooksDirectlyInGazeCheck(Unit* caster) : caster(caster) {}

		Unit* caster;

        bool operator() (Unit* unit)
        {
            Position pos;
            caster->GetPosition(&pos);
            return !unit->HasInArc(static_cast<float>(M_PI), &pos);
        }
};

class spell_lunatic_gaze_targeting : public SpellScriptLoader
{
    public:
        spell_lunatic_gaze_targeting() : SpellScriptLoader("spell_lunatic_gaze_targeting") { }

        class spell_lunatic_gaze_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_lunatic_gaze_targeting_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if (DontLooksDirectlyInGazeCheck(GetCaster()));
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_lunatic_gaze_targeting_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_lunatic_gaze_targeting_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_lunatic_gaze_targeting_SpellScript();
        }
};

class brainLinkTargetSelector
{
    public:
        bool operator() (Unit* unit)
        {
            return unit->GetPositionZ() < 300.0f;
        }
};

class spell_brain_link_periodic_dummy : public SpellScriptLoader
{
    public:
        spell_brain_link_periodic_dummy() : SpellScriptLoader("spell_brain_link_periodic_dummy") { }

        class spell_brain_link_periodic_dummy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_brain_link_periodic_dummy_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if (brainLinkTargetSelector());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_brain_link_periodic_dummy_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        class spell_brain_link_periodic_dummy_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_brain_link_periodic_dummy_AuraScript)

            void HandlePeriodicDummy(AuraEffect const* aurEff)
            {
                PreventDefaultAction();
                if (Unit* trigger = GetTarget())
                {
                    if (trigger->GetTypeId() == TYPE_ID_PLAYER)
                    {
                        if (!trigger->GetMap()->IsDungeon())
                            return;

                        Map::PlayerList const &players = trigger->GetMap()->GetPlayers();
                        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
						{
                            if (Player* player = itr->getSource())
							{
                                if (player->HasAura(SPELL_BRAIN_LINK) && player->GetGUID() != trigger->GetGUID() )
                                {
                                    trigger->CastSpell(player, SPELL_BRAIN_LINK_DUMMY, true);

									if (Unit* caster = GetCaster())
									{
										if (!player->IsWithinDist(trigger, float(aurEff->GetAmount())))
										{
											caster->CastSpell(trigger, SPELL_BRAIN_LINK_DAMAGE, true);

											if (InstanceScript* instance = caster->GetInstanceScript())
											{
												if (caster->ToCreature() && caster->GetGUID() == instance->GetData64(DATA_SARA))
													CAST_AI(boss_sara::boss_saraAI,caster->ToCreature()->AI())->ModifySanity(player, -2);
											}
										}
                                    }
                                }
							}
						}
                    }
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_brain_link_periodic_dummy_AuraScript::HandlePeriodicDummy, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_brain_link_periodic_dummy_SpellScript();
        }

        AuraScript* GetAuraScript() const
        {
            return new spell_brain_link_periodic_dummy_AuraScript();
        }
};

class NotIsWeakenedImmortalCheck
{
    public:
        NotIsWeakenedImmortalCheck() { }

        bool operator() (Unit* unit)
        {
            return !(unit->HasAura(SPELL_WEAKENED));
        }
};

class spell_titanic_storm_targeting : public SpellScriptLoader
{
    public:
        spell_titanic_storm_targeting() : SpellScriptLoader("spell_titanic_storm_targeting") { }

        class spell_titanic_storm_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_titanic_storm_targeting_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if (NotIsWeakenedImmortalCheck());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_titanic_storm_targeting_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_titanic_storm_targeting_SpellScript();
        }
};

class spell_insane_death_effect : public SpellScriptLoader
{
    public:
        spell_insane_death_effect() : SpellScriptLoader("spell_insane_death_effect") { }

        class spell_insane_death_effect_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_insane_death_effect_AuraScript);

            void HandleEffectRemove(AuraEffect const * /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = GetTarget())
				{
                    if (target->ToPlayer() && target->IsAlive())
                        target->DealDamage(target, target->GetHealth());
				}
            }

            void Register()
            {
                OnEffectRemove += AuraEffectRemoveFn(spell_insane_death_effect_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_AOE_CHARM, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_insane_death_effect_AuraScript();
        }
};

class spell_summon_tentacle_position : public SpellScriptLoader
{
    public:
        spell_summon_tentacle_position() : SpellScriptLoader("spell_summon_tentacle_position") { }

        class spell_summon_tentacle_position_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_summon_tentacle_position_SpellScript);

            void ChangeSummonPos(SpellEffIndex /*effIndex*/)
            {
                WorldLocation summonPos = *GetExplTargetDest();

                if (Unit* caster = GetCaster())
                    summonPos.m_positionZ = caster->GetMap()->GetHeight(summonPos.GetPositionX(), summonPos.GetPositionY(), summonPos.GetPositionZ());

                SetExplTargetDest(summonPos);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_summon_tentacle_position_SpellScript::ChangeSummonPos, EFFECT_0, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_summon_tentacle_position_SpellScript();
        }
};

class spell_empowering_shadows : public SpellScriptLoader
{
    public:
        spell_empowering_shadows() : SpellScriptLoader("spell_empowering_shadows") { }

        class spell_empowering_shadows_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_empowering_shadows_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (Unit * target = GetHitUnit())
                {
                    uint32 spell = 0;

                    switch(target->GetMap()->GetDifficulty())
                    {
                        case RAID_DIFFICULTY_10MAN_NORMAL:
							spell = SPELL_EMPOWERING_SHADOWS_HEAL_10;
							break;
						case RAID_DIFFICULTY_25MAN_NORMAL:
							spell = SPELL_EMPOWERING_SHADOWS_HEAL_25;
							break;
					}
                    if (spell)
                        GetCaster()->CastSpell(target, spell, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_empowering_shadows_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_empowering_shadows_SpellScript();
        }
};

class spell_hodir_protective_gaze : public SpellScriptLoader
{
public:
    spell_hodir_protective_gaze() : SpellScriptLoader("spell_hodir_protective_gaze") { }

    class spell_hodir_protective_gaze_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_hodir_protective_gaze_AuraScript);

        bool Validate(SpellInfo const * /*spellEntry*/)
        {
            return sSpellMgr->GetSpellInfo(SPELL_FLASH_FREEZE_COOLDOWN);
        }

        void HandleEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            std::list<Unit*> targetList;
            aurEff->GetTargetList(targetList);

            for (std::list<Unit*>::iterator itr = targetList.begin(); itr != targetList.end(); ++itr)
			{
                if (!(*itr)->ToPlayer() && (*itr)->GetGUID() != GetCasterGUID())
                    (*itr)->RemoveAurasDueToSpell(GetSpellInfo()->Id);
			}
        }

        void CalculateAmount(AuraEffect const * /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
        {
            // Set absorbtion amount to unlimited
            amount = -1;
        }

        void Absorb(AuraEffect * /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
        {
            Unit* target = GetTarget();
            if (dmgInfo.GetDamage() < target->GetHealth())
                return;

            target->CastSpell(target, SPELL_FLASH_FREEZE, true);

            absorbAmount = dmgInfo.GetDamage() - target->GetHealth() + 1;

            if (GetCaster() && GetCaster()->ToCreature())
                GetCaster()->ToCreature()->RemoveAurasDueToSpell(SPELL_HODIRS_PROTECTIVE_GAZE);
        }

        void Register()
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_hodir_protective_gaze_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            OnEffectAbsorb += AuraEffectAbsorbFn(spell_hodir_protective_gaze_AuraScript::Absorb, EFFECT_0);
            OnEffectApply += AuraEffectApplyFn(spell_hodir_protective_gaze_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_hodir_protective_gaze_AuraScript();
    }
};

class spell_sara_psychosis : public SpellScriptLoader
{
    public:
        spell_sara_psychosis() : SpellScriptLoader("spell_sara_psychosis") {}

        class spell_sara_psychosis_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sara_psychosis_SpellScript);

			void FilterTargets(std::list<Unit*>& unitList)
            {
                if (Unit* target = Quantum::DataPackets::SelectRandomContainerElement(unitList))
                {
                    unitList.clear();
                    unitList.push_back(target);
                }
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_sara_psychosis_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_sara_psychosis_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sara_psychosis_SpellScript();
        }
};

void AddSC_boss_yogg_saron()
{
    new boss_sara();
    new npc_ominous_cloud();
    new npc_guardian_of_yogg_saron();
    new npc_yogg_saron_tentacle();
    new npc_descend_into_madness();
    new npc_influence_tentacle();
    new npc_brain_of_yogg_saron();
    new boss_yogg_saron();
    new npc_immortal_guardian();
    new npc_support_keeper();
    new npc_sanity_well();
    new npc_laughting_skull();
    new npc_death_orb();
    new npc_death_ray();
    new go_flee_to_surface();
    new spell_keeper_support_aura_targeting();
    new spell_lunatic_gaze_targeting();
    new spell_brain_link_periodic_dummy();
    new spell_titanic_storm_targeting();
	new spell_insane_death_effect();
    new spell_summon_tentacle_position();
    new spell_empowering_shadows();
    new spell_hodir_protective_gaze();
	new spell_sara_psychosis();
}