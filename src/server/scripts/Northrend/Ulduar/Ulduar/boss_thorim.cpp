/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

// Need Implement Normal amd Hard Mode Texts

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "SpellScript.h"
#include "ulduar.h"

enum Texts
{
    SAY_AGGRO_1                     = -1603270,
    SAY_AGGRO_2                     = -1603271,
    SAY_SPECIAL_1                   = -1603272,
    SAY_SPECIAL_2                   = -1603273,
    SAY_SPECIAL_3                   = -1603274,
    SAY_JUMPDOWN                    = -1603275,
    SAY_SLAY_1                      = -1603276,
    SAY_SLAY_2                      = -1603277,
    SAY_BERSERK                     = -1603278,
    SAY_WIPE                        = -1603279,
    SAY_DEATH                       = -1603280,
    SAY_END_NORMAL_1                = -1603281,
    SAY_END_NORMAL_2                = -1603282,
    SAY_END_NORMAL_3                = -1603283,
    SAY_END_HARD_1                  = -1603284,
    SAY_END_HARD_2                  = -1603285,
    SAY_END_HARD_3                  = -1603286,
	EMOTE_RUNIC_BARRIER             = -1613224,
	EMOTE_RUNIC_MIGHT               = -1613225,
	// Sif
	SAY_SIF_INTRO                   = -1613226,
    SAY_SIF_COMBAT                  = -1613227,
    SAY_SIF_KILL_THORIM             = -1613228,
};

enum Spells
{
	SPELL_RANDOM_LIGHTNING_VISUAL = 56914,
    SPELL_SHEAT_OF_LIGHTNING      = 62276,
    SPELL_STORMHAMMER             = 62042,
    SPELL_DEAFENING_THUNDER       = 62470,
    SPELL_CHARGE_ORB              = 62016,
    SPELL_SUMMON_LIGHTNING_ORB    = 62391,
    SPELL_TOUCH_OF_DOMINION       = 62565,
    SPELL_CHAIN_LIGHTNING_10      = 62131,
    SPELL_CHAIN_LIGHTNING_25      = 64390,
    SPELL_LIGHTNING_CHARGE        = 62279,
    SPELL_LIGHTNING_DESTRUCTION   = 62393,
    SPELL_LIGHTNING_RELEASE       = 62466,
    SPELL_LIGHTNING_PILLAR        = 62976,
    SPELL_UNBALANCING_STRIKE      = 62130,
    SPELL_BERSERK_PHASE_1         = 62560,
    SPELL_BERSERK_PHASE_2         = 26662,
};

// Runic Colossus (Mini Boss) Spells
enum RunicSpells
{
    SPELL_SMASH             = 62339,
    SPELL_RUNIC_BARRIER     = 62338,
    SPELL_RUNIC_CHARGE      = 62613,
    SPELL_RUNIC_SMASH       = 62465,
    SPELL_RUNIC_SMASH_LEFT  = 62057,
    SPELL_RUNIC_SMASH_RIGHT = 62058,
};

// Ancient Rune Giant (Mini Boss) Spells
enum AncientSpells
{
    SPELL_RUNIC_FORTIFICATION = 62942,
    SPELL_RUNE_DETONATION     = 62526,
    SPELL_STOMP               = 62411,
};

enum SifSpells
{
	SPELL_SIF_CHANNEL      = 64324,
    SPELL_FROSTBOLT_VOLLEY = 62580,
    SPELL_FROSTNOVA        = 62597,
    SPELL_BLIZZARD         = 62576,
    SPELL_FROSTBOLT        = 69274,
};

enum PreAddSpells
{
    SPELL_ACID_BREATH    = 62315,
    SPELL_ACID_BREATH_H  = 62415,
    SPELL_SWEEP          = 62316,
    SPELL_SWEEP_H        = 62417,
    SPELL_DEVASTATE      = 62317,
    SPELL_HEROIC_SWIPE   = 62444,
    SPELL_BARBED_SHOT    = 62318,
    SPELL_SHOOT          = 16496,
    SPELL_RENEW          = 62333,
    SPELL_RENEW_H        = 62441,
    SPELL_GREATER_HEAL   = 62334,
    SPELL_GREATER_HEAL_H = 62442,
};

enum Events
{
    EVENT_SAY_AGGRO_2        = 1,
    EVENT_STORMHAMMER        = 2,
    EVENT_CHARGE_ORB         = 3,
    EVENT_SUMMON_WARBRINGER  = 4,
    EVENT_SUMMON_EVOKER      = 5,
    EVENT_SUMMON_COMMONER    = 6,
    EVENT_BERSERK            = 7,
    EVENT_UNBALANCING_STRIKE = 8,
    EVENT_CHAIN_LIGHTNING    = 9,
    EVENT_TRANSFER_ENERGY    = 10,
    EVENT_RELEASE_ENERGY     = 11,
	EVENT_RANDOM_SAY         = 12,
};

enum Phases
{
    PHASE_0,
    PHASE_1,
    PHASE_2,
};

enum Actions
{
	ACTION_INCREASE_PREADDS_COUNT = 1,
	ACTION_RUNIC_SMASH            = 2,
	ACTION_BERSERK                = 3,
};

enum HardModeTimer
{
	MAX_HARD_MODE_TIME = 180000,
};

// Thorim Pre-Phase Adds
enum PreAdds
{
    BEHEMOTH,
    MERCENARY_CAPTAIN_A,
    MERCENARY_SOLDIER_A,
    DARK_RUNE_ACOLYTE,
    MERCENARY_CAPTAIN_H,
    MERCENARY_SOLDIER_H,
};

const uint32 PRE_PHASE_ADD[] = {32882, 32908, 32885, 32886, 32907, 32883};
#define SPELL_PRE_PRIMARY(i) RAID_MODE(SPELL_PRE_PRIMARY_N[i],SPELL_PRE_PRIMARY_H[i])
const uint32 SPELL_PRE_PRIMARY_N[] = {SPELL_ACID_BREATH,   SPELL_DEVASTATE, SPELL_BARBED_SHOT, SPELL_RENEW, SPELL_DEVASTATE, SPELL_BARBED_SHOT};
const uint32 SPELL_PRE_PRIMARY_H[] = {SPELL_ACID_BREATH_H, SPELL_DEVASTATE, SPELL_BARBED_SHOT, SPELL_RENEW_H, SPELL_DEVASTATE, SPELL_BARBED_SHOT};
#define SPELL_PRE_SECONDARY(i) RAID_MODE(SPELL_PRE_SECONDARY_N[i],SPELL_PRE_SECONDARY_H[i])
const uint32 SPELL_PRE_SECONDARY_N[] = {SPELL_SWEEP, SPELL_HEROIC_SWIPE, SPELL_SHOOT, SPELL_GREATER_HEAL, SPELL_HEROIC_SWIPE, SPELL_SHOOT};
const uint32 SPELL_PRE_SECONDARY_H[] = {SPELL_SWEEP_H, SPELL_HEROIC_SWIPE, SPELL_SHOOT, SPELL_GREATER_HEAL_H, SPELL_HEROIC_SWIPE, SPELL_SHOOT};
#define SPELL_HOLY_SMITE RAID_MODE(62335, 62443)

// Thorim Arena Phase Adds
enum ArenaAdds
{
    DARK_RUNE_CHAMPION,
    DARK_RUNE_COMMONER,
    DARK_RUNE_EVOKER,
    DARK_RUNE_WARBRINGER,
    IRON_RING_GUARD,
    IRON_HONOR_GUARD,
	DARK_RUNE_ACOLYTE_ARENA,
	DARK_RUNE_ACOLYTE_TUNNEL,
};

const uint32 ARENA_PHASE_ADD[] = {32876, 32904, 32878, 32877, 32874, 32875, 33110};

#define SPELL_ARENA_PRIMARY(i) RAID_MODE(SPELL_ARENA_PRIMARY_N[i], SPELL_ARENA_PRIMARY_H[i])

const uint32 SPELL_ARENA_PRIMARY_N[] = {35054, 62326, 62327, 62322, 64151, 42724, 62333};
const uint32 SPELL_ARENA_PRIMARY_H[] = {35054, 62326, 62445, 62322, 64151, 42724, 62441};
#define SPELL_ARENA_SECONDARY(i) RAID_MODE(SPELL_ARENA_SECONDARY_N[i],SPELL_ARENA_SECONDARY_H[i])
const uint32 SPELL_ARENA_SECONDARY_N[] = {15578, 38313, 62321, 62331, 62332, 62334};
const uint32 SPELL_ARENA_SECONDARY_H[] = {15578, 38313, 62529, 62418, 62420, 62442};
#define SPELL_AURA_OF_CELERITY 62320
#define SPELL_CHARGE 32323
#define SPELL_RUNIC_MENDING RAID_MODE(62328, 62446)

const Position Pos[7] =
{
    {2095.53f, -279.48f, 419.84f, 0.504f},
    {2092.93f, -252.96f, 419.84f, 6.024f},
    {2097.86f, -240.97f, 419.84f, 5.643f},
    {2113.14f, -225.94f, 419.84f, 5.259f},
    {2156.87f, -226.12f, 419.84f, 4.202f},
    {2172.42f, -242.70f, 419.84f, 3.583f},
    {2171.92f, -284.59f, 419.84f, 2.691f},
};

const Position PosOrbs[7] =
{
    {2104.99f, -233.484f, 433.576f, 5.49779f},
    {2092.64f, -262.594f, 433.576f, 6.26573f},
    {2104.76f, -292.719f, 433.576f, 0.78539f},
    {2164.97f, -293.375f, 433.576f, 2.35619f},
    {2164.58f, -233.333f, 433.576f, 3.90954f},
    {2145.81f, -222.196f, 433.576f, 4.45059f},
    {2123.91f, -222.443f, 433.576f, 4.97419f},
};

const Position PosCharge[7] =
{
    {2108.95f, -289.241f, 420.149f, 5.49779f},
    {2097.93f, -262.782f, 420.149f, 6.26573f},
    {2108.66f, -237.102f, 420.149f, 0.78539f},
    {2160.56f, -289.292f, 420.149f, 2.35619f},
    {2161.02f, -237.258f, 420.149f, 3.90954f},
    {2143.87f, -227.415f, 420.149f, 4.45059f},
    {2125.84f, -227.439f, 420.149f, 4.97419f},
};

const Position OrbVisualSpawnPos[3] =
{
	{2134.8f, -391.316f, 481.055f, 1.59617f},
	{2134.66f, -340.177f, 471.269f, 1.57654f},
	{2134.99f, -262.757f, 443.303f, 1.63392f},
};

#define POS_X_ARENA  2181.19f
#define POS_Y_ARENA  -299.12f

#define IN_ARENA(who) (who->GetPositionX() < POS_X_ARENA && who->GetPositionY() > POS_Y_ARENA)
#define IS_HEALER(who) (who->GetEntry() == NPC_DARK_RUNE_ACOLYTE || who->GetEntry() == NPC_RUNE_ENWOKER || who->GetEntry() ==  NPC_RUNE_ACOLYTE)

struct SummonLocation
{
    float x, y, z, o;
    uint32 entry;
};

SummonLocation preAddLocations[] =
{
    {2149.68f, -263.477f, 419.679f, 3.120f, NPC_JORMUNGAR_BEHEMOTH},
    {2131.31f, -271.640f, 419.840f, 2.188f, NPC_MERCENARY_CAPTAIN_A},
    {2127.24f, -259.182f, 419.974f, 5.917f, NPC_MERCENARY_SOLDIER_A},
    {2123.32f, -254.770f, 419.840f, 6.170f, NPC_MERCENARY_SOLDIER_A},
    {2120.10f, -258.990f, 419.840f, 6.250f, NPC_MERCENARY_SOLDIER_A},
    {2129.09f, -277.142f, 419.756f, 1.222f, NPC_DARK_RUNE_ACOLYTE},
};

SummonLocation colossusAddLocations[] =
{
    {2218.38f, -297.50f, 412.18f, 1.030f, NPC_IRON_RING_GUARD},
    {2235.07f, -297.98f, 412.18f, 1.613f, NPC_IRON_RING_GUARD},
    {2235.26f, -338.34f, 412.18f, 1.589f, NPC_IRON_RING_GUARD},
    {2217.69f, -337.39f, 412.18f, 1.241f, NPC_IRON_RING_GUARD},
    {2227.58f, -308.30f, 412.18f, 1.591f, NPC_RUNE_ACOLYTE},
    {2227.47f, -345.37f, 412.18f, 1.566f, NPC_RUNE_ACOLYTE},
};

SummonLocation giantAddLocations[] =
{
    {2198.05f, -428.77f, 419.95f, 6.056f, NPC_HONOR_GUARD},
    {2220.31f, -436.22f, 412.26f, 1.064f, NPC_HONOR_GUARD},
    {2158.88f, -441.73f, 438.25f, 0.127f, NPC_HONOR_GUARD},
    {2198.29f, -436.92f, 419.95f, 0.261f, NPC_RUNE_ACOLYTE},
    {2230.93f, -434.27f, 412.26f, 1.931f, NPC_RUNE_ACOLYTE},
};

class boss_thorim : public CreatureScript
{
public:
    boss_thorim() : CreatureScript("boss_thorim") { }

    struct boss_thorimAI : public BossAI
    {
        boss_thorimAI(Creature* creature) : BossAI(creature, DATA_THORIM)
        {
            Wipe = false;
            EncounterFinished = false;
        }

        Phases phase;

        uint8 PreAddsCount;
        uint32 EncounterTime;
        uint32 CheckTargetTimer;

        bool Wipe;
        bool HardMode;
        bool OrbSummoned;
        bool EncounterFinished;
        bool summonChampion;

        void Reset()
        {
            if (EncounterFinished)
                return;

            if (Wipe)
                DoSendQuantumText(SAY_WIPE, me);

            _Reset();

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
            me->SetReactState(REACT_PASSIVE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NON_ATTACKABLE);

            phase = PHASE_0;
            Wipe = false;
            HardMode = false;
            OrbSummoned = false;
            summonChampion = false;
            CheckTargetTimer = 7000;
            PreAddsCount = 0;

			SpawnMiniBosses();
			SpawnPrePhaseAdds();
			SummonSif();

			for (uint8 i = 0; i < 3; i++)
				me->SummonCreature(NPC_LIGHTNING_ORB_VISUAL, OrbVisualSpawnPos[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);

            if (GameObject* go = me->FindGameobjectWithDistance(GO_THORIM_LEVER, 500.0f))
                go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
        }

		void SummonSif()
		{
			me->SummonCreature(NPC_SIF, 2148.39f, -296.046f, 438.247f, 438.247f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30*IN_MILLISECONDS);
		}

		void SpawnMiniBosses()
		{
			for (uint8 i = DATA_RUNIC_COLOSSUS; i <= DATA_RUNE_GIANT; ++i)
			{
				if (Creature* MiniBoss = me->GetCreature(*me, instance->GetData64(i)))
					MiniBoss->Respawn(true);
			}
		}

		void SpawnPrePhaseAdds()
		{
			// Spawn Pre-Phase Adds
			for (uint8 i = 0; i < 6; ++i)
				me->SummonCreature(preAddLocations[i].entry, preAddLocations[i].x, preAddLocations[i].y, preAddLocations[i].z, preAddLocations[i].o, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3*IN_MILLISECONDS);
		}

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
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
							player->AddItem(ITEM_ID_ESSENCE_OF_THORIM, 1);
					}
				}
			}
		}

        void EncounterIsDone()
        {
            if (EncounterFinished)
                return;

            EncounterFinished = true;
            me->SetCurrentFaction(FACTION_FRIENDLY);
			//QuestReward();
			me->RemoveAllAuras();
			me->RemoveAllAttackers();
			me->AttackStop();
			me->CombatStop(true);
			DoCast(me, SPELL_TELEPORT);
			DoSendQuantumText(SAY_DEATH, me);
			me->DespawnAfterAction(7.5*IN_MILLISECONDS);

			if (Creature* sif = me->FindCreatureWithDistance(NPC_SIF, 500.0f))
			{
				sif->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
				sif->RemoveAllAuras();
				sif->RemoveAllAttackers();
				sif->AttackStop();
				sif->CombatStop(true);
				sif->DespawnAfterAction(7.5*IN_MILLISECONDS);
				DoSendQuantumText(SAY_SIF_KILL_THORIM, sif);
				sif->SummonCreature(NPC_UNFLUENCE_TENTACLE, 2134.77f, -253.558f, 419.794f, 1.55758f, TEMPSUMMON_TIMED_DESPAWN, 35000);
			}

            if (instance)
            {
                // Kill credit
                instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, 64985);
				instance->DoCompleteAchievement(RAID_MODE(ACHIEVEMENT_WHO_NEEDS_BLOODLUST_10, ACHIEVEMENT_WHO_NEEDS_BLOODLUST_25));
                // Lose Your Illusion
                if (HardMode)
                {
                    instance->DoCompleteAchievement(RAID_MODE(ACHIEVEMENT_LOSE_ILLUSION_10, ACHIEVEMENT_LOSE_ILLUSION_25));
					me->SummonGameObject(RAID_MODE(GO_CACHE_OF_STORMS_10H, GO_CACHE_OF_STORMS_25H), 2134.58f, -286.908f, 419.495f, 1.55988f, 0.0f, 0.0f, 1.0f, 1.0f, 604800);
                }
                else
					me->SummonGameObject(RAID_MODE(GO_CACHE_OF_STORMS_10N, GO_CACHE_OF_STORMS_25N), 2134.58f, -286.908f, 419.495f, 1.55988f, 0, 0, 1, 1, 604800);
            }

            _JustDied();
        }

        void EnterToBattle(Unit* /*who*/)
        {
			_EnterToBattle();

			DoSendQuantumText(SAY_AGGRO_1, me);

            // Spawn Thunder Orbs
            for (uint8 n = 0; n < 7; ++n)
                me->SummonCreature(NPC_THUNDER_ORB, PosOrbs[n], TEMPSUMMON_CORPSE_DESPAWN);

            Wipe = true;
            EncounterTime = 0;
            phase = PHASE_1;
            events.SetPhase(PHASE_1);
            DoCast(me, SPELL_SHEAT_OF_LIGHTNING);
            events.ScheduleEvent(EVENT_STORMHAMMER, 40000, 0, PHASE_1);
            events.ScheduleEvent(EVENT_CHARGE_ORB, 30000, 0, PHASE_1);
            events.ScheduleEvent(EVENT_SUMMON_WARBRINGER, 25000, 0, PHASE_1);
            events.ScheduleEvent(EVENT_SUMMON_EVOKER, 30000, 0, PHASE_1);
            events.ScheduleEvent(EVENT_SUMMON_COMMONER, 35000, 0, PHASE_1);
            events.ScheduleEvent(EVENT_BERSERK, 360000, 0, PHASE_1);
            events.ScheduleEvent(EVENT_SAY_AGGRO_2, 10000, 0, PHASE_1);

            if (Creature* runic = me->GetCreature(*me, instance->GetData64(DATA_RUNIC_COLOSSUS)))
            {
                runic->SetActive(true);
                runic->AI()->DoAction(ACTION_RUNIC_SMASH);
            }

            if (GameObject* go = me->FindGameobjectWithDistance(GO_THORIM_LEVER, 500.0f))
                go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
        }

		void DamageTaken(Unit* attacker, uint32 &damage)
        {
            if (damage >= me->GetHealth())
            {
                damage = 0;
                EncounterIsDone();
            }

            if (phase == PHASE_1 && attacker && instance)
            {
                Creature* colossus = me->GetCreature(*me, instance->GetData64(DATA_RUNIC_COLOSSUS));
                Creature* giant = me->GetCreature(*me, instance->GetData64(DATA_RUNE_GIANT));
                if (colossus->IsDead() && giant->IsDead() && me->IsWithinDistInMap(attacker, 10.0f) && attacker->ToPlayer())
                {
                    DoSendQuantumText(SAY_JUMPDOWN, me);
                    phase = PHASE_2;
                    events.SetPhase(PHASE_2);
                    me->RemoveAurasDueToSpell(SPELL_SHEAT_OF_LIGHTNING);
                    me->SetReactState(REACT_AGGRESSIVE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                    me->GetMotionMaster()->MoveJump(2134.79f, -263.03f, 419.84f, 10.0f, 20.0f);
                    summons.DespawnEntry(NPC_THUNDER_ORB); // despawn charged orbs
                    events.ScheduleEvent(EVENT_UNBALANCING_STRIKE, 15000, 0, PHASE_2);
                    events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 20000, 0, PHASE_2);
                    events.ScheduleEvent(EVENT_TRANSFER_ENERGY, 20000, 0, PHASE_2);
                    events.ScheduleEvent(EVENT_BERSERK, 300000, 0, PHASE_2);
					events.ScheduleEvent(EVENT_RANDOM_SAY, 30000, 0, PHASE_2);
                    // Hard Mode
                    if (EncounterTime <= MAX_HARD_MODE_TIME)
                    {
                        HardMode = true;

						// Activate sif
						if (Creature* sif = me->FindCreatureWithDistance(NPC_SIF, 250.0f))
						{
							sif->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
							sif->AI()->DoZoneInCombat();
						}

                        // Achievement Siffed
                        if (instance)
                            instance->DoCompleteAchievement(RAID_MODE(ACHIEVEMENT_SIFFED_10, ACHIEVEMENT_SIFFED_25));
                    }
                    else me->AddAura(SPELL_TOUCH_OF_DOMINION, me);
                }
            }
        }

        void EnterEvadeMode()
        {
            if (!_EnterEvadeMode())
                return;

            me->GetMotionMaster()->MoveTargetedHome();

			if (Creature* sif = me->FindCreatureWithDistance(NPC_SIF, 250.0f))
				sif->DespawnAfterAction();
        }

		void JustReachedHome()
		{
			Reset();
		}

		void DoAction(int32 const action)
        {
            switch (action)
            {
                case ACTION_BERSERK:
                    if (phase != PHASE_1)
                        return;

                    if (!OrbSummoned)
                    {
                        events.RescheduleEvent(EVENT_BERSERK, 1000);
                        OrbSummoned = true;
                    }
                    return;
                case ACTION_INCREASE_PREADDS_COUNT:
                    ++PreAddsCount;
                    break;
            }

            if (PreAddsCount >= 6 && !Wipe)
            {
                // Event starts
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                events.Reset();
                DoZoneInCombat();
            }
        }

        void JustSummoned(Creature* summon)
        {
            summons.Summon(summon);

            if (me->IsInCombatActive())
                DoZoneInCombat(summon);

            if (summon->GetEntry() == NPC_LIGHTNING_ORB)
                summon->CastSpell(summon, SPELL_LIGHTNING_DESTRUCTION, true);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            if (phase == PHASE_2 && me->GetVictim() && !IN_ARENA(me->GetVictim()))
            {
                me->GetVictim()->getHostileRefManager().deleteReference(me);
                return;
            }

            if (CheckTargetTimer <= diff)
            {
                // workaround, see mimiron script
                if (!SelectTarget(TARGET_RANDOM, 0, 200.0f, true))
                {
                    EnterEvadeMode();
                    return;
                }
                CheckTargetTimer = 7000;
            }
            else CheckTargetTimer -= diff;

            // still needed?
            if (phase == PHASE_2 && !IN_ARENA(me))
            {
                EnterEvadeMode();
                return;
            }

            events.Update(diff);
            EncounterTime += diff;

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (phase == PHASE_1)
            {
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SAY_AGGRO_2:
                            DoSendQuantumText(SAY_AGGRO_2, me);
                            break;
                        case EVENT_STORMHAMMER:
                            DoCast(SPELL_STORMHAMMER);
                            events.ScheduleEvent(EVENT_STORMHAMMER, urand(15, 20) *IN_MILLISECONDS, 0, PHASE_1);
                            break;
                        case EVENT_CHARGE_ORB:
                            DoCastAOE(SPELL_CHARGE_ORB);
                            events.ScheduleEvent(EVENT_CHARGE_ORB, urand(15, 20) *IN_MILLISECONDS, 0, PHASE_1);
                            break;
                        case EVENT_SUMMON_WARBRINGER:
                            me->SummonCreature(ARENA_PHASE_ADD[3], Pos[rand()%7], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3*IN_MILLISECONDS);
                            if (summonChampion)
                            {
                                me->SummonCreature(ARENA_PHASE_ADD[0], Pos[rand()%7], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3*IN_MILLISECONDS);
                                summonChampion = false;
                            }
                            else
                                summonChampion = true;
                            events.ScheduleEvent(EVENT_SUMMON_WARBRINGER, 20000, 0, PHASE_1);
                            break;
                        case EVENT_SUMMON_EVOKER:
                            me->SummonCreature(ARENA_PHASE_ADD[2], Pos[rand()%7], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3*IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_SUMMON_EVOKER, urand(23, 27) *IN_MILLISECONDS, 0, PHASE_1);
                            break;
                        case EVENT_SUMMON_COMMONER:
                            for (uint8 n = 0; n < urand(5, 7); ++n)
                                me->SummonCreature(ARENA_PHASE_ADD[1], Pos[rand()%7], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3*IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_SUMMON_COMMONER, 30000, 0, PHASE_1);
                            break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK_PHASE_1);
                            DoCast(me, SPELL_SUMMON_LIGHTNING_ORB, true);
                            DoSendQuantumText(SAY_BERSERK, me);
                            break;
                    }
                }
            }
            else
            {
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_UNBALANCING_STRIKE:
                            DoCastVictim(SPELL_UNBALANCING_STRIKE);
                            events.ScheduleEvent(EVENT_UNBALANCING_STRIKE, 26000, 0, PHASE_2);
                            break;
                        case EVENT_CHAIN_LIGHTNING:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
							{
                                DoCast(target, RAID_MODE(SPELL_CHAIN_LIGHTNING_10, SPELL_CHAIN_LIGHTNING_25));
							}
                            events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(7, 15) *IN_MILLISECONDS, 0, PHASE_2);
                            break;
                        case EVENT_TRANSFER_ENERGY:
                            if (Creature* source = me->SummonCreature(NPC_POWER_SOURCE, PosCharge[urand(0, 6)], TEMPSUMMON_TIMED_DESPAWN, 9000))
							{
                                source->CastSpell(source, SPELL_LIGHTNING_PILLAR, true);
							}
                            events.ScheduleEvent(EVENT_RELEASE_ENERGY, 8000, 0, PHASE_2);
                            break;
                        case EVENT_RELEASE_ENERGY:
                            if (Creature* source = me->FindCreatureWithDistance(NPC_POWER_SOURCE, 100.0f))
							{
                                DoCast(source, SPELL_LIGHTNING_RELEASE);
							}
                            DoCast(me, SPELL_LIGHTNING_CHARGE, true);
                            events.ScheduleEvent(EVENT_TRANSFER_ENERGY, 8000, 0, PHASE_2);
                            break;
						case EVENT_RANDOM_SAY:
							DoSendQuantumText(RAND(SAY_SPECIAL_1, SAY_SPECIAL_2, SAY_SPECIAL_3), me);
							events.ScheduleEvent(EVENT_RANDOM_SAY, 32000, 0, PHASE_2);
							break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK_PHASE_2);
                            DoSendQuantumText(SAY_BERSERK, me);
                            break;
                    }
                }
            }

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_thorimAI(creature);
    }
};

class npc_thorim_pre_phase : public CreatureScript
{
public:
    npc_thorim_pre_phase() : CreatureScript("npc_thorim_pre_phase") { }

    struct npc_thorim_pre_phaseAI : public QuantumBasicAI
    {
        npc_thorim_pre_phaseAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            me->SetCurrentFaction(FACTION_HOSTILE);
            for (uint8 i = 0; i < 6; ++i)
                if (me->GetEntry() == PRE_PHASE_ADD[i])
                    id = PreAdds(i);

            Healer = IS_HEALER(me);
        }

        InstanceScript* instance;
        PreAdds id;
        uint32 PrimaryTimer;
        uint32 SecondaryTimer;
        bool Healer;

        void Reset()
        {
            PrimaryTimer = urand(3000, 6000);
            SecondaryTimer = urand (12000, 15000);
        }

        void JustDied(Unit* /*killer*/)
        {
            if (Creature* thorim = me->GetCreature(*me, instance->GetData64(DATA_THORIM)))
                thorim->AI()->DoAction(ACTION_INCREASE_PREADDS_COUNT);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (PrimaryTimer <= diff)
            {
                Unit* target = NULL;
                if (Healer)
                {
                    if (!(target = SelectTargetHpFriendly(30)))
                        target = me;
                }
				else
                {
                    target = me->GetVictim();
                }

                if (target)
                {
                    DoCast(target,SPELL_PRE_PRIMARY(id));
                    PrimaryTimer = urand(15000, 20000);
                }
            }
            else PrimaryTimer -= diff;

            if (SecondaryTimer <= diff)
            {
                Unit* target = NULL;
                if (Healer)
                {
                    if (!(target = SelectTargetHpFriendly(30)))
                        target = me;
                }
				else
                {
                    target = me->GetVictim();
                }

                if (target)
                {
                    DoCast(SPELL_PRE_SECONDARY(id));
                    SecondaryTimer = urand(4000, 8000);
                }
            }
            else SecondaryTimer -= diff;

            if (id == DARK_RUNE_ACOLYTE)
                DoSpellAttackIfReady(SPELL_HOLY_SMITE);
            else
                DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_thorim_pre_phaseAI(creature);
    }
};

class npc_thorim_arena_phase : public CreatureScript
{
    public:
        npc_thorim_arena_phase() : CreatureScript("npc_thorim_arena_phase") { }

        struct npc_thorim_arena_phaseAI : public QuantumBasicAI
        {
            npc_thorim_arena_phaseAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
                me->SetCurrentFaction(FACTION_HOSTILE);
                for (uint8 i = 0; i < 7; ++i)
                    if (me->GetEntry() == ARENA_PHASE_ADD[i])
                        id = ArenaAdds(i);

                IsInArena = IN_ARENA(me);
                Healer = IS_HEALER(me);
            }

            bool isOnSameSide(const Unit* who)
            {
                return (IsInArena == IN_ARENA(who));
            }

            void DamageTaken(Unit* attacker, uint32 &damage)
            {
                if (!isOnSameSide(attacker))
                    damage = 0;
            }

            void Reset()
            {
                PrimaryTimer = urand(3000, 6000);
                SecondaryTimer = urand (7000, 9000);
                ChargeTimer = 8000;
            }

            void EnterToBattle(Unit* /*who*/)
            {
                if (id == DARK_RUNE_WARBRINGER)
                    DoCast(me, SPELL_AURA_OF_CELERITY);
            }

            void EnterEvadeMode()
            {
                if (Creature* thorim = me->GetCreature(*me, instance ? instance->GetData64(DATA_THORIM) : 0))
                    thorim->AI()->DoAction(ACTION_BERSERK);
                _EnterEvadeMode();
                me->GetMotionMaster()->MoveTargetedHome();
                Reset();
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (me->GetVictim() && !isOnSameSide(me->GetVictim()))
                {
                    me->GetVictim()->getHostileRefManager().deleteReference(me);
                    return;
                }

                if (PrimaryTimer <= diff)
                {
                    Unit* target = NULL;
                    if (Healer && id != NPC_RUNE_ENWOKER)
                    {
                        if (!(target = SelectTargetHpFriendly(30)))
                            target = me;
                    }
                    else target = me->GetVictim();

                    DoCast(SPELL_ARENA_PRIMARY(id));
                    PrimaryTimer = urand(3000, 6000);
                }
                else PrimaryTimer -= diff;

                if (SecondaryTimer <= diff)
                {
                    Unit* target = NULL;
                    if (Healer)
                    {
                        if (!(target = SelectTargetHpFriendly(30)))
                            target = me;
                    }
                    else target = me->GetVictim();

                    if (target)
                    {
                        DoCast(SPELL_ARENA_SECONDARY(id));
                        SecondaryTimer = urand(12000, 16000);
                    }
                }
                else SecondaryTimer -= diff;

                if (ChargeTimer <= diff)
                {
                    if (id == DARK_RUNE_CHAMPION)
					{
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 40, true))
						{
                            DoCast(target, SPELL_CHARGE);
							ChargeTimer = 12000;
						}
					}
                }
                else ChargeTimer -= diff;

                if (id == DARK_RUNE_ACOLYTE)
                    DoSpellAttackIfReady(SPELL_HOLY_SMITE);
                else
                    DoMeleeAttackIfReady();
            }

        private:
            InstanceScript* instance;
            ArenaAdds id;
            uint32 PrimaryTimer;
            uint32 SecondaryTimer;
            uint32 ChargeTimer;
            bool IsInArena;
            bool Healer;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_thorim_arena_phaseAI(creature);
        }
};

class npc_runic_colossus : public CreatureScript
{
    public:
        npc_runic_colossus() : CreatureScript("npc_runic_colossus") { }

        struct npc_runic_colossusAI : public QuantumBasicAI
        {
            npc_runic_colossusAI(Creature* creature) : QuantumBasicAI(creature), summons(me)
            {
                instance = creature->GetInstanceScript();
                SetImmuneToPushPullEffects(true);
            }

			InstanceScript* instance;
            SummonList summons;

            uint8 Side;
            uint8 RunicSmashPhase;
            uint32 BarrierTimer;
            uint32 SmashTimer;
            uint32 ChargeTimer;
            uint32 RunicSmashTimer;

            void Reset()
            {
                BarrierTimer = urand(12000, 15000);
                SmashTimer = urand (15000, 18000);
                ChargeTimer = urand (20000, 24000);

                RunicSmashPhase = 0;
                RunicSmashTimer = 1000;
                Side = 0;

                me->SetActive(false);
                me->GetMotionMaster()->MoveTargetedHome();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                // Runed Door closed
                if (instance)
                    instance->SetData(DATA_RUNIC_DOOR, GO_STATE_READY);

                // Spawn trashes
                summons.DespawnAll();
                for (uint8 i = 0; i < 6; i++)
                    me->SummonCreature(colossusAddLocations[i].entry, colossusAddLocations[i].x, colossusAddLocations[i].y, colossusAddLocations[i].z,
                    colossusAddLocations[i].o, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3*IN_MILLISECONDS);
            }

            void JustSummoned(Creature* summon)
            {
                summons.Summon(summon);
            }

            void JustDied(Unit* /*killer*/)
            {
                // Runed Door opened
                if (instance)
                    instance->SetData(DATA_RUNIC_DOOR, GO_STATE_ACTIVE);
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_RUNIC_SMASH:
                        RunicSmashPhase = 1;
                        break;
                }
            }

            void DoRunicSmash()
            {
                for (uint8 i = 0; i < 9; i++)
                    if (Creature* bunny = me->SummonCreature(NPC_GOLEM_BUNNY, Side ? 2236.0f : 2219.0f, i * 10 - 380.0f, 412.2f, 0, TEMPSUMMON_TIMED_DESPAWN, 5000))
                        bunny->AI()->SetData(1, (i + 1)* 200);

                for (uint8 i = 0; i < 9; i++)
                    if (Creature* bunny = me->SummonCreature(NPC_GOLEM_BUNNY, Side ? 2246.0f : 2209.0f, i * 10 - 380.0f, 412.2f, 0, TEMPSUMMON_TIMED_DESPAWN, 5000))
                        bunny->AI()->SetData(1, (i + 1)* 200);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                RunicSmashPhase = 0;
                me->InterruptNonMeleeSpells(true);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

            void UpdateAI(uint32 const diff)
            {
                if (RunicSmashPhase == 1)
                {
                    if (RunicSmashTimer <= diff)
                    {
                        Side = urand(0, 1);
                        DoCast(me, Side ? SPELL_RUNIC_SMASH_LEFT : SPELL_RUNIC_SMASH_RIGHT);
                        RunicSmashTimer = 1000;
                        RunicSmashPhase = 2;
                    }
                    else RunicSmashTimer -= diff;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (RunicSmashPhase == 2)
                {
                    RunicSmashPhase = 1;
                    DoRunicSmash();
                }

                if (!UpdateVictim())
                    return;

                if (BarrierTimer <= diff)
                {
                    DoSendQuantumText(EMOTE_RUNIC_BARRIER, me);
                    DoCast(me, SPELL_RUNIC_BARRIER);
                    BarrierTimer = urand(35000, 45000);
                }
                else BarrierTimer -= diff;

                if (SmashTimer <= diff)
                {
                    DoCast(me, SPELL_SMASH);
                    SmashTimer = urand (15000, 18000);
                }
                else SmashTimer -= diff;

                if (ChargeTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0, -8, true))
					{
                        DoCast(target, SPELL_RUNIC_CHARGE);
						ChargeTimer = 20000;
					}
                }
                else ChargeTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_runic_colossusAI(creature);
        }
};

class npc_runic_smash : public CreatureScript
{
    public:
        npc_runic_smash() : CreatureScript("npc_runic_smash") { }

        struct npc_runic_smashAI : public QuantumBasicAI
        {
            npc_runic_smashAI(Creature* creature) : QuantumBasicAI(creature)
            {
				SetCombatMovement(false);
                me->SetReactState(REACT_PASSIVE);
                me->SetDisplayId(16925);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                ExplodeTimer = 10000;
            }

            void SetData(uint32 /*type*/, uint32 data)
            {
                ExplodeTimer = data;
            }

            void UpdateAI(uint32 const diff)
            {
                if (ExplodeTimer <= diff)
                {
                    DoCastAOE(SPELL_RUNIC_SMASH, true);
                    ExplodeTimer = 10000;
                }
                else ExplodeTimer -= diff;
            }

        private:
            uint32 ExplodeTimer;
        };


        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_runic_smashAI(creature);
        }
};

class npc_ancient_rune_giant : public CreatureScript
{
public:
    npc_ancient_rune_giant() : CreatureScript("npc_ancient_rune_giant") { }

    struct npc_ancient_rune_giantAI : public QuantumBasicAI
    {
        npc_ancient_rune_giantAI(Creature* creature) : QuantumBasicAI(creature), summons(me)
        {
            instance = creature->GetInstanceScript();
            SetImmuneToPushPullEffects(true);
        }

        InstanceScript* instance;
        SummonList summons;

        uint32 StompTimer;
        uint32 DetonationTimer;

        void Reset()
        {
            StompTimer = urand(10000, 12000);
            DetonationTimer = 25000;

            me->GetMotionMaster()->MoveTargetedHome();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            // Stone Door closed
            if (instance)
                instance->SetData(DATA_STONE_DOOR, GO_STATE_READY);

            // Spawn trashes
            summons.DespawnAll();
            for (uint8 i = 0; i < 5; i++)
                me->SummonCreature(giantAddLocations[i].entry,giantAddLocations[i].x,giantAddLocations[i].y,giantAddLocations[i].z,giantAddLocations[i].o,TEMPSUMMON_CORPSE_TIMED_DESPAWN,3000);
        }

        void JustSummoned(Creature *summon)
        {
            summons.Summon(summon);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(EMOTE_RUNIC_MIGHT, me);
            DoCast(me, SPELL_RUNIC_FORTIFICATION, true);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }

        void JustDied(Unit* /*killer*/)
        {
            // Stone Door opened
            if (instance)
                instance->SetData(DATA_STONE_DOOR, GO_STATE_ACTIVE);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (StompTimer <= diff)
            {
                DoCast(me, SPELL_STOMP);
                StompTimer = urand(10000, 12000);
            }
            else StompTimer -= diff;

            if (DetonationTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 40, true))
				{
                    DoCast(target, SPELL_RUNE_DETONATION);
					DetonationTimer = urand(10000, 12000);
				}
            }
            else DetonationTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_ancient_rune_giantAI(creature);
    }
};

class npc_sif : public CreatureScript
{
public:
    npc_sif () : CreatureScript("npc_sif") { }

    struct npc_sifAI : public QuantumBasicAI
    {
        npc_sifAI(Creature* creature) : QuantumBasicAI(creature)
        {
			instance = creature->GetInstanceScript();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		InstanceScript* instance;
		bool SifIntro;

        uint32 FrostTimer;
        uint32 VolleyTimer;
        uint32 BlizzardTimer;
        uint32 NovaTimer;

        void Reset()
        {
            FrostTimer = 2000;
            VolleyTimer = 15000;
            BlizzardTimer = 30000;
            NovaTimer = urand(20000, 25000);

			SifChanneling();

			SifIntro = false;
        }

		void SifChanneling()
		{
			if (Creature* thorim = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_THORIM) : 0))
				DoCast(thorim, SPELL_SIF_CHANNEL);
		}

		void MoveInLineOfSight(Unit* who)
		{
			if (!instance || SifIntro || who->GetTypeId() != TYPE_ID_PLAYER || !who->IsWithinDistInMap(me, 60.0f))
				return;

			if (instance && SifIntro == false && who->GetTypeId() == TYPE_ID_PLAYER && who->IsWithinDistInMap(me, 60.0f))
			{
				DoSendQuantumText(SAY_SIF_INTRO, me);
				SifIntro = true;
			}
		}

		void JustReachedHome()
		{
			SifChanneling();
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoSendQuantumText(SAY_SIF_COMBAT, me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (FrostTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 60, true))
				{
                    DoCast(target, SPELL_FROSTBOLT);
					FrostTimer = 4000;
				}
            }
            else FrostTimer -= diff;

            if (VolleyTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 40, true))
                {
                    DoResetThreat();
                    me->AddThreat(target, 5000000.0f);
                    DoCast(target, SPELL_FROSTBOLT_VOLLEY, true);
					VolleyTimer = urand(15000, 20000);
				}
            }
            else VolleyTimer -= diff;

            if (BlizzardTimer <= diff)
            {
                DoCast(me, SPELL_BLIZZARD, true);
                BlizzardTimer = 45000;
            }
            else BlizzardTimer -= diff;

            if (NovaTimer <= diff)
            {
                DoCastAOE(SPELL_FROSTNOVA, true);
                NovaTimer = urand(20000, 25000);
            }
            else NovaTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sifAI(creature);
    }
};

class npc_lightning_orb_visual : public CreatureScript
{
public:
    npc_lightning_orb_visual() : CreatureScript("npc_lightning_orb_visual") {}

    struct npc_lightning_orb_visualAI : public QuantumBasicAI
    {
        npc_lightning_orb_visualAI(Creature* creature) : QuantumBasicAI(creature){}

        void Reset()
        {
			DoCast(me, SPELL_RANDOM_LIGHTNING_VISUAL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const /*diff*/){}
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_lightning_orb_visualAI(creature);
    }
};

class NotInArenaCheck
{
public:
	bool operator() (Unit* unit)
	{
		return !IN_ARENA(unit);
	}
};

class spell_stormhammer_targeting : public SpellScriptLoader
{
    public:
        spell_stormhammer_targeting() : SpellScriptLoader("spell_stormhammer_targeting") { }

        class spell_stormhammer_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stormhammer_targeting_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                target = NULL;
                unitList.remove_if (NotInArenaCheck());

                if (unitList.empty())
                    return;

                target = Quantum::DataPackets::SelectRandomContainerElement(unitList);
                SetTarget(unitList);
            }

            void SetTarget(std::list<Unit*>& unitList)
            {
                unitList.clear();

                if (target)
                    unitList.push_back(target);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_stormhammer_targeting_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_stormhammer_targeting_SpellScript::SetTarget, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_stormhammer_targeting_SpellScript::SetTarget, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
            }

			Unit* target;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_stormhammer_targeting_SpellScript();
        }
};

class spell_thorim_charge_orb_targeting : public SpellScriptLoader
{
    public:
        spell_thorim_charge_orb_targeting() : SpellScriptLoader("spell_thorim_charge_orb_targeting") {}

        class spell_thorim_charge_orb_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_thorim_charge_orb_targeting_SpellScript);

			void FilterTargets(std::list<Unit*>& unitList)
            {
                target = NULL;

                if (unitList.empty())
                    return;

                // Charge Orb should be cast always only on 1 orb
                target = Quantum::DataPackets::SelectRandomContainerElement(unitList);
                SetTarget(unitList);
            }

			void SetTarget(std::list<Unit*>& unitList)
            {
                unitList.clear();

                if (target)
                    unitList.push_back(target);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_thorim_charge_orb_targeting_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_thorim_charge_orb_targeting_SpellScript::SetTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }

			Unit* target;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_thorim_charge_orb_targeting_SpellScript();
        }
};

class ArenaAreaCheck
{
public:
	ArenaAreaCheck(bool shouldBeIn): _shouldBeIn(shouldBeIn) {}

	bool operator() (const Unit* unit)
	{
		return _shouldBeIn ? _IsInArena(unit) : !_IsInArena(unit);
	}
private:
	bool _shouldBeIn;
	bool _IsInArena(const Unit* who)
	{
		return (who->GetPositionX() < POS_X_ARENA && who->GetPositionY() > POS_Y_ARENA);
	}
};

class spell_thorim_lightning_destruction : public SpellScriptLoader
{
    public:
        spell_thorim_lightning_destruction() : SpellScriptLoader("spell_thorim_lightning_destruction") {}

        class spell_thorim_lightning_destruction_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_thorim_lightning_destruction_SpellScript);

			void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if(ArenaAreaCheck(true));
                unitList.remove_if(PlayerOrPetCheck());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_thorim_lightning_destruction_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_thorim_lightning_destruction_SpellScript();
        }
};

class spell_thorim_runic_fortification : public SpellScriptLoader
{
    public:
        spell_thorim_runic_fortification() : SpellScriptLoader("spell_thorim_runic_fortification") { }

        class spell_thorim_runic_fortification_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_thorim_runic_fortification_SpellScript);

			void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if(NoPlayerOrPetCheck());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_thorim_runic_fortification_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_thorim_runic_fortification_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_thorim_runic_fortification_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_thorim_runic_fortification_SpellScript();
        }
};

class ThorimBerserkSelector
{
public:
	bool operator() (Unit* unit)
	{
		if (unit->ToCreature())
		{
            switch (unit->ToCreature()->GetEntry())
			{
                case NPC_IRON_RING_GUARD:
				case NPC_RUNE_ACOLYTE:
				case NPC_RUNIC_COLOSSUS:
				case NPC_HONOR_GUARD:
				case NPC_RUNE_GIANT:
					return false;
				default:
					break;
			}
		}
		return true;
	}
};

class spell_thorim_berserk : public SpellScriptLoader
{
    public:
        spell_thorim_berserk() : SpellScriptLoader("spell_thorim_berserk") { }

        class spell_thorim_berserk_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_thorim_berserk_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if(ThorimBerserkSelector());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_thorim_berserk_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
				OnUnitTargetSelect += SpellUnitTargetFn(spell_thorim_berserk_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
				OnUnitTargetSelect += SpellUnitTargetFn(spell_thorim_berserk_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_thorim_berserk_SpellScript();
        }
};

void AddSC_boss_thorim()
{
    new boss_thorim();
    new npc_thorim_pre_phase();
    new npc_thorim_arena_phase();
    new npc_runic_colossus();
    new npc_runic_smash();
    new npc_ancient_rune_giant();
    new npc_sif();
	new npc_lightning_orb_visual();
    new spell_stormhammer_targeting();
	new spell_thorim_charge_orb_targeting();
    new spell_thorim_lightning_destruction();
    new spell_thorim_runic_fortification();
	new spell_thorim_berserk();
}