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
#include "Group.h"
#include "ulduar.h"

enum Texts
{
    SAY_AGGRO                       = -1603000,
    SAY_SLAY_1                      = -1603001,
    SAY_SLAY_2                      = -1603002,
    SAY_ENGAGED_FOR_FIRST_TIME      = -1603003,
    SAY_PHASE_2                     = -1603004,
    SAY_SUMMON_COLLAPSING_STAR      = -1603005,
    SAY_OUTRO_1                     = -1603006,
    SAY_OUTRO_2                     = -1603007,
    SAY_OUTRO_3                     = -1603008,
    SAY_OUTRO_4                     = -1603009,
    SAY_OUTRO_5                     = -1603010,
    SAY_BERSERK                     = -1603011,
    SAY_BIG_BANG_1                  = -1603012,
    SAY_BIG_BANG_2                  = -1603013,
    SAY_TIMER_1                     = -1603014,
    SAY_TIMER_2                     = -1603015,
    SAY_TIMER_3                     = -1603016,
    SAY_SUMMON_1                    = -1603017,
    SAY_SUMMON_2                    = -1603018,
    SAY_SUMMON_3                    = -1603019,
	EMOTE_COLLAPSING_STAR           = -1613240,
	EMOTE_COSMIC_SMASH              = -1613241,
	EMOTE_BIG_BANG                  = -1613242,
	// Brann Bronzebeard
	SAY_BRANN_CONSOLE               = -1613247,
	SAY_BRANN_ALGALON               = -1613248,
	SAY_BRANN_OUTRO                 = -1613249,
};

enum Spells
{
	SPELL_ARRIVAL                   = 64997,
    SPELL_RIDE_THE_LIGHTNING        = 64986,
	SPELL_REORIGINATION             = 64996,
	SPELL_SELF_STUN                 = 65256,
	SPELL_SUMMON_AZEROTH            = 64994,
	SPELL_KILL_CREDIT               = 65184,
	SPELL_DUAL_WIELD                = 42459,
    SPELL_ASCEND_TO_THE_HEAVENS     = 64487,
    SPELL_BERSERK                   = 47008,
    SPELL_BIG_BANG_10               = 64443,
    SPELL_BIG_BANG_25               = 64584,
	SPELL_QUANTUM_STRIKE_10         = 64395,
    SPELL_QUANTUM_STRIKE_25         = 64592,
    SPELL_COSMIC_SMASH_10           = 62301,
    SPELL_COSMIC_SMASH_25           = 64598,
    SPELL_PHASE_PUNCH               = 64412,
    SPELL_PHASE_PUNCH_PHASE         = 64417,
    SPELL_PHASE_PUNCH_ALPHA_1       = 64435,
    SPELL_PHASE_PUNCH_ALPHA_2       = 64434,
    SPELL_PHASE_PUNCH_ALPHA_3       = 64428,
    SPELL_PHASE_PUNCH_ALPHA_4       = 64421,
	SPELL_SUPERMASSIVE_FAIL         = 65311,
    SPELL_BLACK_HOLE_CREDIT         = 65312,
    SPELL_BLACK_HOLE_EXPLOSION_10   = 64122,
    SPELL_BLACK_HOLE_EXPLOSION_25   = 65108,
    SPELL_BLACK_HOLE_SPAWN_VISUAL   = 62003,
    SPELL_BLACK_HOLE_STATE          = 64135,
    SPELL_BLACK_HOLE_TRIGGER        = 62185,
    SPELL_BLACK_HOLE_PHASE          = 62168,
	SPELL_BLACK_HOLE_DAMAGE         = 62169, // NEED
	SPELL_BLACK_HOLE_TARGET_VISUAL  = 62348, // NEED
	SPELL_CONSTELLATION_PHASE       = 65508,
    SPELL_DESPAWN_BLACK_HOLE        = 64391,
    SPELL_ARCANE_BARAGE_10          = 64599,
    SPELL_ARCANE_BARAGE_25          = 64607,
    SPELL_VOID_ZONE_VISUAL          = 64469,
	SPELL_COSMIC_SMASH_VISUAL       = 62300,
	SPELL_COSMIC_SMASH_TRG_DMG_10   = 62311,
	SPELL_COSMIC_SMASH_TRG_DMG_25   = 64596,
};

enum Events
{
	EVENT_MOVE_INTO_ALGALON     = 1,
	EVENT_ALGALON_INTRO         = 2,
	EVENT_BRANN_REACHED_HOME    = 3,
	EVENT_BRANN_DESPAWN         = 4,
	EVENT_SAY_INTRO_BRANN       = 5,
    EVENT_ASCEND                = 6,
    EVENT_BERSERK               = 7,
    EVENT_BIG_BANG              = 8,
    EVENT_COSMIC_SMASH          = 9,
    EVENT_PHASE_PUNCH           = 10,
    EVENT_QUANTUM_STRIKE        = 11,
    EVENT_COLLAPSING_STAR       = 12,
    EVENT_LIVING_CONSTELLATION  = 13,
    EVENT_DARK_MATTER           = 14,
    EVENT_GEAR_CHECK            = 15,
};

enum IntroActions
{
	ACTION_START_INTRO = 0,
};

enum Data
{
    DATA_HERALD_OF_THE_TITANS = 1,
};

static Position miscLocations[] =
{
    {1632.36f, -310.09f, 385.0f, 0.0f}, // Cosmic Smash Trigger
};

static Position collapsingLocations[] =
{
    {1616.25f, -293.081f, 417.321f, 5.51543f},
    {1647.26f, -292.592f, 417.322f, 3.93168f},
    {1617.38f, -324.447f, 417.321f, 0.81718f},
    {1647.45f, -323.651f, 417.321f, 2.39740f},
};

static Position constellationLocations[] =
{
    {1649.30f, -295.34f, 458.13f, 0.0f},
    {1612.22f, -294.84f, 458.13f, 0.0f},
    {1629.95f, -327.90f, 458.13f, 0.0f},
};

static Position AlgalonEndPos = {1632.9f, -307.9f, 417.4f};
static Position BrannIntroSpawnPos = {1630.94f, -227.212f, 418.265f, 4.72514f};
static Position BrannOutroSpawnPos = {1631.73f, -278.229f, 417.322f, 4.67235f};

Position const BrannMoveToAlgalon[1] =
{
	{1632.13f, -267.7f, 417.322f, 4.80289f},
};

Position const BrannMoveToHome[1] =
{
	{1632.04f, -225.357f, 418.41f, 1.55713f},
};

enum Equip
{
	EQUIP_ID_MAIN_HAND = 45620,
	EQUIP_ID_OFF_HAND  = 45607,
};

class boss_algalon_the_observer : public CreatureScript
{
    public:
        boss_algalon_the_observer() : CreatureScript("boss_algalon_the_observer") { }

        struct boss_algalon_the_observerAI : public BossAI
        {
            boss_algalon_the_observerAI(Creature* creature) : BossAI(creature, DATA_ALGALON_THE_OBSERVER)
            {
                FirstTime = true;
            }

			uint8 ConstellationCount;
            uint8 StarCount;
            uint8 Phase;
            uint8 Step;

            uint32 StepTimer;

			bool Rewarded;
            bool HeraldOfTheTitans;
            bool FirstStars;
            bool FirstTime;
            bool WipeRaid;

            void Reset()
            {
                _Reset();

                if (FirstTime)
                {
					DoCast(me, SPELL_DUAL_WIELD, true);
					DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);

					summons.DespawnEntry(NPC_AZEROTH);
					me->SummonCreature(NPC_AZEROTH, 1633.36f, -293.938f, 417.321f, 1.62875f, TEMPSUMMON_MANUAL_DESPAWN, 7*DAY);
                    DoCastAOE(SPELL_REORIGINATION, true);
                }
                else me->SetCurrentFaction(FACTION_NEUTRAL);

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetReactState(REACT_PASSIVE);

                Phase = 0;
                StepTimer = 0;
                StarCount = 0;
                ConstellationCount = 0;
                WipeRaid = false;
                FirstStars = true;
				Rewarded = true;
                HeraldOfTheTitans = true;

                me->SetAttackTime(OFF_ATTACK, 1500);
				me->SetStatFloatValue(UNIT_FIELD_MIN_DAMAGE, float(RAID_MODE(28000, 50000)));
                me->SetStatFloatValue(UNIT_FIELD_MAX_DAMAGE, float(RAID_MODE(35000, 60000)));
                me->SetStatFloatValue(UNIT_FIELD_MIN_OFF_HAND_DAMAGE, float(RAID_MODE(15000, 28000)));
                me->SetStatFloatValue(UNIT_FIELD_MAX_OFF_HAND_DAMAGE, float(RAID_MODE(18000, 35000)));
            }

            void JustReachedHome()
            {
				Reset();
            }

            void EnterToBattle(Unit* /*who*/)
            {
                me->SetCurrentFaction(FACTION_HOSTILE);
                DoZoneInCombat();
				DoCast(me, SPELL_ARRIVAL, true);
				DoCast(me, SPELL_RIDE_THE_LIGHTNING, true);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

                if (FirstTime)
                    Step = 1;
                else
                    Step = 4;

                events.ScheduleEvent(EVENT_BERSERK, 6*MINUTE*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_BIG_BANG, 90*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_PHASE_PUNCH, 15*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_QUANTUM_STRIKE, urand(4, 14) *IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_COSMIC_SMASH, 25*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_COLLAPSING_STAR, 15*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_LIVING_CONSTELLATION, 50*IN_MILLISECONDS);

                if (!Is25ManRaid())
                    events.ScheduleEvent(EVENT_GEAR_CHECK, urand(5, 10) *IN_MILLISECONDS);
            }

			void KilledUnit(Unit* victim)
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

			void CompleteEncounter()
			{
				if (Rewarded)
				{
					instance->DoCompleteAchievement(RAID_MODE(ACHIEVEMENT_SUPERMASSIVE_10, ACHIEVEMENT_SUPERMASSIVE_25));
					instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_KILL_CREDIT);
					instance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, me->GetEntry(), 1);
					Rewarded = false;
				}
			}

            void CheckItems()
            {
                Map* map = me->GetMap();
                if (map && map->IsDungeon())
                {
                    Map::PlayerList const& Players = map->GetPlayers();

                    for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
					{
                        if (Player* player = itr->getSource())
                        {
                            if (player->IsGameMaster() || !player->IsAlive())
                                continue;

                            for (int i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
                            {
                                // don't check tabard or shirt
                                if (i == EQUIPMENT_SLOT_TABARD || i == EQUIPMENT_SLOT_BODY)
                                    continue;

                                uint32 maxLevel;
                                switch (i)
                                {
                                    case EQUIPMENT_SLOT_MAIN_HAND:
                                    case EQUIPMENT_SLOT_OFF_HAND:
                                    case EQUIPMENT_SLOT_RANGED:
                                        maxLevel = 232;
                                        break;
                                    default:
                                        maxLevel = 226;
                                        break;
                                }

                                if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
								{
                                    if (item->GetTemplate())
									{
                                        if (item->GetTemplate()->ItemLevel > maxLevel)
                                            HeraldOfTheTitans = false;
									}
								}
                            }
						}
					}
                }
            }

            void JumpToNextStep(uint32 timer)
            {
                StepTimer = timer;
                ++Step;
            }

            void DoAction(int32 const action)
            {
                if (Phase == 3)
                    return;

                _JustDied();
                me->DisappearAndDie();
            }

            uint32 GetData(uint32 type)
            {
                if (type == DATA_HERALD_OF_THE_TITANS)
                    return HeraldOfTheTitans ? 1 : 0;

                return 0;
            }

            void JustSummoned(Creature* summon)
            {
                summons.Summon(summon);

                switch (summon->GetEntry())
                {
                    case NPC_COLLAPSING_STAR:
                        ++StarCount;
                        summon->SetReactState(REACT_PASSIVE);
						summon->SetInCombatWithZone();
						summon->GetMotionMaster()->MoveRandom(15.0f);
                        break;
                    case NPC_LIVING_CONSTELLATION:
                        ++ConstellationCount;
                        summon->SetInCombatWithZone();
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                            summon->AI()->AttackStart(target);
                        break;
                    case NPC_UNLEASHED_DARK_MATTER:
                        summon->SetInCombatWithZone();
                        if (Unit* target = SelectTarget(TARGET_TOP_AGGRO, 0))
                            summon->AI()->AttackStart(target);
                        break;
                    default:
                        break;
                }
            }

            void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
            {
                switch (summon->GetEntry())
                {
                    case NPC_COLLAPSING_STAR:
                        --StarCount;
                        me->SummonCreature(NPC_BLACK_HOLE, summon->GetPositionX(), summon->GetPositionY(), summon->GetPositionZ(), me->GetOrientation());
                        break;
                    case NPC_LIVING_CONSTELLATION:
                        --ConstellationCount;
                        break;
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_PHASE_PUNCH)
                {
                    uint32 stackAmount = target->GetAuraCount(SPELL_PHASE_PUNCH);
                    switch (stackAmount)
                    {
                        case 1:
                            target->CastSpell(target, SPELL_PHASE_PUNCH_ALPHA_1, true);
                            break;
                        case 2:
                            target->CastSpell(target, SPELL_PHASE_PUNCH_ALPHA_2, true);
                            break;
                        case 3:
                            target->CastSpell(target, SPELL_PHASE_PUNCH_ALPHA_3, true);
                            break;
                        case 4:
                            target->CastSpell(target, SPELL_PHASE_PUNCH_ALPHA_4, true);
                            break;
                        case 5:
                            target->CastSpell(target, SPELL_PHASE_PUNCH_PHASE, true);
                            break;
                        default:
                            break;
                    }
                }
            }

            void EnterEvadeMode()
            {
                if (!WipeRaid)
                {
                    summons.DespawnAll();

                    // remove players from phase
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_PHASE_PUNCH_PHASE);
                    instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_BLACK_HOLE_PHASE);

                    WipeRaid = true;
                    me->InterruptNonMeleeSpells(true);
                    DoCastAOE(SPELL_ASCEND_TO_THE_HEAVENS);
                    return;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                _EnterEvadeMode();
                me->GetMotionMaster()->MoveTargetedHome();
                Reset();
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (Phase == 1 && HealthBelowPct(HEALTH_PERCENT_20))
                {
                    Phase = 2;
					DoSendQuantumText(SAY_PHASE_2, me);
                    summons.DespawnAll();
                    events.CancelEvent(EVENT_COLLAPSING_STAR);
                    events.CancelEvent(EVENT_LIVING_CONSTELLATION);
                    events.ScheduleEvent(EVENT_DARK_MATTER, 5*IN_MILLISECONDS);

                    // summon 4 unstable black holes
                    for (uint8 i = 0; i < 4; ++i)
                        me->SummonCreature(NPC_BLACK_HOLE, collapsingLocations[i]);
                }

                if (Phase == 2 && HealthBelowPct(HEALTH_PERCENT_5))
                {
                    summons.DespawnAll();
					me->InterruptNonMeleeSpells(true);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    me->SetReactState(REACT_PASSIVE);
                    me->RemoveAllAuras();
					me->AttackStop();
					me->RemoveAllAttackers();
                    Step = 1;
                    Phase = 3;
                }

                if (Phase == 0)
                {
                    if (StepTimer <= diff)
                    {
                        switch (Step)
                        {
                            case 1:
                                DoSendQuantumText(SAY_SUMMON_1, me);
                                JumpToNextStep(7500);
                                break;
                            case 2:
                                DoSendQuantumText(SAY_SUMMON_2, me);
                                JumpToNextStep(6000);
                                break;
                            case 3:
                                DoSendQuantumText(SAY_SUMMON_3, me);
                                JumpToNextStep(11000);
                                break;
                            case 4:
                                _EnterToBattle();
                                summons.DespawnEntry(NPC_AZEROTH);
                                SetEquipmentSlots(false, EQUIP_ID_MAIN_HAND, EQUIP_ID_OFF_HAND, EQUIP_NO_CHANGE);
                                DoSendQuantumText(SAY_AGGRO, me);
                                if (!FirstTime)
								{
                                    JumpToNextStep(7000);
								}
                                else
								{
                                    JumpToNextStep(15000);
								}
                                break;
                            case 5:
                                if (!FirstTime)
                                {
                                    JumpToNextStep(0);
                                    break;
                                }
								DoSendQuantumText(SAY_ENGAGED_FOR_FIRST_TIME, me);
                                JumpToNextStep(11000);
                                break;
                            case 6:
								me->RemoveAurasDueToSpell(SPELL_RIDE_THE_LIGHTNING);
                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                                me->SetReactState(REACT_AGGRESSIVE);
                                FirstTime = false;
                                Phase = 1;
                                break;
                        }
                    }
                    else StepTimer -= diff;
                    return;
                }

                if (Phase == 3)
                {
                    if (StepTimer <= diff)
                    {
                        switch (Step)
                        {
						    case 1:
								me->GetMotionMaster()->MovePoint(0, AlgalonEndPos);
								me->SummonGameObject(RAID_MODE(GO_GIFT_OF_THE_OBSERVER_10, GO_GIFT_OF_THE_OBSERVER_25), 1632.53f, -295.983f, 417.322f, 1.56774f, 0, 0, 0, 0, 604800);
								JumpToNextStep(10000);
								break;
                            case 2:
								me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_KNEEL);
								me->SetFacingTo(1.58686f);
								_JustDied();
								me->SummonCreature(NPC_BRANN_OUTRO_ALGALON, BrannOutroSpawnPos, TEMPSUMMON_MANUAL_DESPAWN, 7*DAY);
                                DoSendQuantumText(SAY_OUTRO_1, me);
                                JumpToNextStep(40000);
                                break;
                            case 3:
                                DoSendQuantumText(SAY_OUTRO_2, me);
                                JumpToNextStep(17000);
                                break;
                            case 4:
                                DoSendQuantumText(SAY_OUTRO_3, me);
                                JumpToNextStep(10000);
                                break;
                            case 5:
                                DoSendQuantumText(SAY_OUTRO_4, me);
                                JumpToNextStep(11000);
                                break;
							case 6:
								if (Creature* brann = me->FindCreatureWithDistance(NPC_BRANN_OUTRO_ALGALON, 250.0f))
								{
									DoSendQuantumText(SAY_BRANN_OUTRO, brann);
								}
								JumpToNextStep(3000);
								break;
                            case 7:
                                DoSendQuantumText(SAY_OUTRO_5, me);
                                JumpToNextStep(11000);
                                break;
                            case 8:
								me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
								me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
								JumpToNextStep(3000);
                                break;
							case 9:
								DoCast(me, SPELL_TELEPORT);
								CompleteEncounter();
								me->DespawnAfterAction(1500);
								if (Creature* brann = me->FindCreatureWithDistance(NPC_BRANN_OUTRO_ALGALON, 250.0f))
								{
									brann->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
									brann->SetSpeed(MOVE_WALK, 1, true);
									brann->GetMotionMaster()->MovePoint(0, BrannMoveToHome[0]);
									brann->DespawnAfterAction(11000);
								}
								break;
                        }
                    }
                    else StepTimer -= diff;

                    return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BIG_BANG:
                            DoSendQuantumText(RAND(SAY_BIG_BANG_1, SAY_BIG_BANG_2), me);
							DoSendQuantumText(EMOTE_BIG_BANG, me);
                            me->GetMotionMaster()->MoveDistract(10*IN_MILLISECONDS);
                            DoCastAOE(RAID_MODE(SPELL_BIG_BANG_10, SPELL_BIG_BANG_25));
                            events.RescheduleEvent(EVENT_COSMIC_SMASH, urand(10, 15) *IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_BIG_BANG, 90*IN_MILLISECONDS);
                            return;
                        case EVENT_PHASE_PUNCH:
                            DoCastVictim(SPELL_PHASE_PUNCH, true);
                            events.ScheduleEvent(EVENT_PHASE_PUNCH, 15*IN_MILLISECONDS);
                            break;
                        case EVENT_QUANTUM_STRIKE:
                            DoCastVictim(RAID_MODE(SPELL_QUANTUM_STRIKE_10, SPELL_QUANTUM_STRIKE_25));
                            events.ScheduleEvent(EVENT_QUANTUM_STRIKE, urand(4, 10) *IN_MILLISECONDS);
                            break;
                        case EVENT_COSMIC_SMASH:
							DoSendQuantumText(EMOTE_COSMIC_SMASH, me);
                            DoCastAOE(RAID_MODE(SPELL_COSMIC_SMASH_10, SPELL_COSMIC_SMASH_25));
                            events.ScheduleEvent(EVENT_COSMIC_SMASH, 25*IN_MILLISECONDS);
                            break;
                        case EVENT_COLLAPSING_STAR:
                            DoSendQuantumText(SAY_SUMMON_COLLAPSING_STAR, me);
                            if (FirstStars)
                            {
								DoSendQuantumText(EMOTE_COLLAPSING_STAR, me);
                                FirstStars = false;
                            }
                            for (uint8 i = StarCount; i < 4; ++i)
							{
                                me->SummonCreature(NPC_COLLAPSING_STAR, collapsingLocations[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3*IN_MILLISECONDS);
							}
                            events.ScheduleEvent(EVENT_COLLAPSING_STAR, 60*IN_MILLISECONDS);
                            break;
                        case EVENT_LIVING_CONSTELLATION:
                            for (uint8 i = ConstellationCount; i < 3; ++i)
							{
                                me->SummonCreature(NPC_LIVING_CONSTELLATION, constellationLocations[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1*IN_MILLISECONDS);
							}
                            events.ScheduleEvent(EVENT_LIVING_CONSTELLATION, 45*IN_MILLISECONDS);
                            break;
                        case EVENT_DARK_MATTER:
                            for (uint8 i = 0; i < 4; ++i)
							{
                                me->SummonCreature(NPC_UNLEASHED_DARK_MATTER, collapsingLocations[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3*IN_MILLISECONDS);
							}
                            events.ScheduleEvent(EVENT_DARK_MATTER, 20*IN_MILLISECONDS);
                            break;
                        case EVENT_BERSERK:
                            DoSendQuantumText(SAY_BERSERK, me);
                            DoCast(me, SPELL_BERSERK, true);
                            events.ScheduleEvent(EVENT_ASCEND, 2*IN_MILLISECONDS);
                            break;
                        case EVENT_ASCEND:
                            DoCastAOE(SPELL_ASCEND_TO_THE_HEAVENS);
                            events.ScheduleEvent(EVENT_ASCEND, 10*IN_MILLISECONDS);
                            break;
                        case EVENT_GEAR_CHECK:
                            CheckItems();
                            if (HeraldOfTheTitans)
                                events.ScheduleEvent(EVENT_GEAR_CHECK, urand(20, 30)*IN_MILLISECONDS);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_algalon_the_observerAI(creature);
        }
};

class npc_collapsing_star : public CreatureScript
{
    public:
        npc_collapsing_star() : CreatureScript("npc_collapsing_star") { }

        struct npc_collapsing_starAI : public QuantumBasicAI
        {
            npc_collapsing_starAI(Creature* creature) : QuantumBasicAI(creature){}

			bool Collapsed;

			uint32 LoseHealthTimer;

            void Reset()
            {
                Collapsed = false;
                LoseHealthTimer = 1*IN_MILLISECONDS;
            }

            void AttackStart(Unit* /*target*/) {}

            void MoveInLineOfSight(Unit* /*who*/) {}

            void DamageTaken(Unit* /*attacker*/, uint32 &damage)
            {
                if (damage >= me->GetHealth() && !Collapsed)
                {
                    Collapsed = true;
                    DoCastAOE(SPELL_BLACK_HOLE_CREDIT, true);
					DoCast(me, RAID_MODE(SPELL_BLACK_HOLE_EXPLOSION_10, SPELL_BLACK_HOLE_EXPLOSION_25), true);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (LoseHealthTimer <= diff)
                {
                    me->DealDamage(me, me->CountPctFromMaxHealth(1));
                    LoseHealthTimer = 1*IN_MILLISECONDS;
                }
                else LoseHealthTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_collapsing_starAI(creature);
        }
};

class npc_living_constellation : public CreatureScript
{
    public:
        npc_living_constellation() : CreatureScript("npc_living_constellation") { }

        struct npc_living_constellationAI : public QuantumBasicAI
        {
            npc_living_constellationAI(Creature* creature) : QuantumBasicAI(creature) { }

			uint32 ArcaneBarrageTimer;

            void Reset()
            {
				me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
				me->SetUnitMovementFlags(MOVEMENTFLAG_FLYING);
				me->SetDisableGravity(true);
                me->SetCanFly(true);
                me->SetSpeed(MOVE_FLIGHT, 0.7f);
                ArcaneBarrageTimer = 1*IN_MILLISECONDS;
            }

            void MoveInLineOfSight(Unit* who)
            {
                QuantumBasicAI::MoveInLineOfSight(who);

                if (who->GetTypeId() != TYPE_ID_UNIT)
                    return;

                if (who->GetEntry() == NPC_BLACK_HOLE && who->GetDistance(me) < 10.0f)
                {
                    who->ToCreature()->CastSpell(who, SPELL_DESPAWN_BLACK_HOLE, true);
                    who->ToCreature()->DespawnAfterAction();
                    me->DealDamage(me, me->GetHealth());
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (ArcaneBarrageTimer <= diff)
                {
                    DoCastAOE(RAID_MODE(SPELL_ARCANE_BARAGE_10, SPELL_ARCANE_BARAGE_25));
                    ArcaneBarrageTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
                }
                else ArcaneBarrageTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_living_constellationAI(creature);
        }
};

class npc_black_hole : public CreatureScript
{
    public:
        npc_black_hole() : CreatureScript("npc_black_hole") { }

        struct npc_black_holeAI : public QuantumBasicAI
        {
            npc_black_holeAI(Creature* creature) : QuantumBasicAI(creature)
            {
				SetCombatMovement(false);
				DoCast(me, SPELL_CONSTELLATION_PHASE, true);
                DoCast(me, SPELL_BLACK_HOLE_SPAWN_VISUAL, true);
                DoCast(me, SPELL_BLACK_HOLE_STATE, true);
                DoCast(me, SPELL_BLACK_HOLE_TRIGGER, true);
				DoCast(me, SPELL_VOID_ZONE_VISUAL, true);
                me->SetCurrentFaction(FACTION_HOSTILE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_PASSIVE);
                me->SetInCombatWithZone();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_black_holeAI(creature);
        }
};

class go_celestial_planetarium_access : public GameObjectScript
{
public:
	go_celestial_planetarium_access() : GameObjectScript("go_celestial_planetarium_access") { }

	bool OnGossipHello(Player* player, GameObject* go)
	{
		InstanceScript* instance = go->GetInstanceScript();
		uint32 item = uint32(go->GetMap()->GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL ? 45796 : 45798);

		if (player->HasItemCount(item, 1))
		{
			instance->SetBossState(DATA_ALGALON_THE_OBSERVER, SPECIAL);
			go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
			go->SetGoState(GO_STATE_ACTIVE);
			go->SummonCreature(NPC_BRANN_INTRO_ALGALON, BrannIntroSpawnPos, TEMPSUMMON_MANUAL_DESPAWN, 7*DAY);
		}
		return true;
	}
};

class spell_cosmic_smash : public SpellScriptLoader
{
    public:
        spell_cosmic_smash() : SpellScriptLoader("spell_cosmic_smash") { }

        class spell_cosmic_smash_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cosmic_smash_SpellScript);

            void FilterTargetsInitial(std::list<Unit*>& unitList)
            {
                sharedUnitList = unitList;
            }

            void FilterTargetsSubsequent(std::list<Unit*>& unitList)
            {
                unitList = sharedUnitList;
            }

            void HandleForceCast(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                Unit* caster = GetCaster();
                Unit* target = GetHitUnit();

                uint32 triggered_spell_id = GetSpellInfo()->Effects[effIndex].TriggerSpell;

                if (caster && target)
                    target->CastSpell(target, triggered_spell_id, true, NULL, NULL, caster->GetGUID());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_cosmic_smash_SpellScript::FilterTargetsInitial, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_cosmic_smash_SpellScript::FilterTargetsSubsequent, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_cosmic_smash_SpellScript::HandleForceCast, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
            }

            std::list<Unit*> sharedUnitList;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_cosmic_smash_SpellScript();
        }
};

class spell_cosmic_smash_summon_trigger : public SpellScriptLoader
{
    public:
        spell_cosmic_smash_summon_trigger() : SpellScriptLoader("spell_cosmic_smash_summon_trigger") { }

        class spell_cosmic_smash_summon_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cosmic_smash_summon_trigger_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (GetCaster())
                    GetCaster()->SummonCreature(NPC_COSMIC_SMASH_TRIGGER, miscLocations[0], TEMPSUMMON_TIMED_DESPAWN, 8000);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_cosmic_smash_summon_trigger_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_cosmic_smash_summon_trigger_SpellScript();
        }
};

class spell_cosmic_smash_summon_target : public SpellScriptLoader
{
    public:
        spell_cosmic_smash_summon_target() : SpellScriptLoader("spell_cosmic_smash_summon_target") { }

        class spell_cosmic_smash_summon_target_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cosmic_smash_summon_target_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (GetCaster())
                    GetCaster()->SummonCreature(NPC_COSMIC_SMASH_TARGET, GetCaster()->GetPositionX(), GetCaster()->GetPositionY(), GetCaster()->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_cosmic_smash_summon_target_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_cosmic_smash_summon_target_SpellScript();
        }
};

class spell_cosmic_smash_dmg : public SpellScriptLoader
{
    public:
        spell_cosmic_smash_dmg() : SpellScriptLoader("spell_cosmic_smash_dmg") { }

        class spell_cosmic_smash_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cosmic_smash_dmg_SpellScript);

            void CalcDamage()
            {
                if (!GetHitUnit() || !GetExplTargetDest())
                    return;

                float distance = GetHitUnit()->GetExactDist2d(GetExplTargetDest());
                if (distance < 6.0f)
                    return;

                SetHitDamage(int32(GetHitDamage() * 10.0f / pow(distance - 2.6f, 1.9f)));
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_cosmic_smash_dmg_SpellScript::CalcDamage);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_cosmic_smash_dmg_SpellScript();
        }
};

class spell_cosmic_smash_missile_target : public SpellScriptLoader
{
    public:
        spell_cosmic_smash_missile_target() : SpellScriptLoader("spell_cosmic_smash_missile_target") { }

        class spell_cosmic_smash_missile_target_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_cosmic_smash_missile_target_SpellScript);

            void FilterTarget(std::list<Unit*>& unitList)
            {
                for (std::list<Unit*>::iterator itr = unitList.begin(); itr != unitList.end(); ++itr)
                {
                    if ((*itr)->IsAlive())
                        (*itr)->Kill(*itr);
                }
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_cosmic_smash_missile_target_SpellScript::FilterTarget, EFFECT_0, TARGET_DEST_NEARBY_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_cosmic_smash_missile_target_SpellScript();
        }
};

class spell_remove_player_from_phase : public SpellScriptLoader
{
    public:
        spell_remove_player_from_phase() : SpellScriptLoader("spell_remove_player_from_phase") { }

        class spell_remove_player_from_phaseScript : public AuraScript
        {
            PrepareAuraScript(spell_remove_player_from_phaseScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BLACK_HOLE_PHASE))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_PHASE_PUNCH_PHASE))
                    return false;
                return true;
            }

			void HandlePeriodic(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();

				GetTarget()->RemoveAurasByType(SPELL_AURA_PHASE);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_remove_player_from_phaseScript::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_remove_player_from_phaseScript();
        }
};

class spell_algalon_phased : public SpellScriptLoader
{
    public:
        spell_algalon_phased() : SpellScriptLoader("spell_algalon_phased") { }

        class spell_algalon_phased_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_algalon_phased_AuraScript);

			void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* target = GetTarget())
                {
					target->RemoveAurasDueToSpell(SPELL_PHASE_PUNCH);
					target->RemoveAurasDueToSpell(SPELL_PHASE_PUNCH);
					target->RemoveAurasDueToSpell(SPELL_PHASE_PUNCH);
					target->RemoveAurasDueToSpell(SPELL_PHASE_PUNCH);
					target->RemoveAurasDueToSpell(SPELL_PHASE_PUNCH);
					target->RemoveAurasDueToSpell(SPELL_PHASE_PUNCH_ALPHA_1);
					target->RemoveAurasDueToSpell(SPELL_PHASE_PUNCH_ALPHA_2);
					target->RemoveAurasDueToSpell(SPELL_PHASE_PUNCH_ALPHA_3);
					target->RemoveAurasDueToSpell(SPELL_PHASE_PUNCH_ALPHA_4);

                    target->CombatStop();
					target->SetPhaseMask(16, true);
                    target->getHostileRefManager().deleteReferences();
                }
            }

			void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
			{
				if (Unit* target = GetTarget())
                {
					target->RemoveAurasDueToSpell(SPELL_PHASE_PUNCH);
					target->RemoveAurasDueToSpell(SPELL_PHASE_PUNCH);
					target->RemoveAurasDueToSpell(SPELL_PHASE_PUNCH);
					target->RemoveAurasDueToSpell(SPELL_PHASE_PUNCH);
					target->RemoveAurasDueToSpell(SPELL_PHASE_PUNCH);
					target->RemoveAurasDueToSpell(SPELL_PHASE_PUNCH_ALPHA_1);
					target->RemoveAurasDueToSpell(SPELL_PHASE_PUNCH_ALPHA_2);
					target->RemoveAurasDueToSpell(SPELL_PHASE_PUNCH_ALPHA_3);
					target->RemoveAurasDueToSpell(SPELL_PHASE_PUNCH_ALPHA_4);

                    target->CombatStop();
					target->SetPhaseMask(1, true);
                    target->getHostileRefManager().deleteReferences();
                }
			}

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_algalon_phased_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PHASE, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_algalon_phased_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PHASE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_algalon_phased_AuraScript();
        }
};

class achievement_herald_of_the_titans : public AchievementCriteriaScript
{
public:
	achievement_herald_of_the_titans() : AchievementCriteriaScript("achievement_herald_of_the_titans") { }

	bool OnCheck(Player* source, Unit* /*target*/)
	{
		if (!source)
			return false;

		if (InstanceScript* instance = source->GetInstanceScript())
			if (Creature* algalon = ObjectAccessor::GetCreature(*source, instance->GetData64(DATA_ALGALON_THE_OBSERVER)))
				if (algalon->AI()->GetData(DATA_HERALD_OF_THE_TITANS))
					return true;

		return false;
	}
};

class npc_brann_bronzebeard_algalon : public CreatureScript
{
public:
	npc_brann_bronzebeard_algalon() : CreatureScript("npc_brann_bronzebeard_algalon") { }
	
	struct npc_brann_bronzebeard_algalonAI : public QuantumBasicAI
	{
		npc_brann_bronzebeard_algalonAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		EventMap events;
		InstanceScript* instance;
		bool Intro;

		void Reset()
		{
			Intro = true;
		}

		void MoveInLineOfSight(Unit* who)
		{
			if (Player* player = who->ToPlayer())
			{
				if (player->IsWithinDist(me, 10.0f) && (!player->GetGroup() || !player->GetGroup()->IsLeader(player->GetGUID())) && !player->IsGameMaster())
				{
					if (Intro)
					{
						me->AI()->DoAction(ACTION_START_INTRO);
						Intro = false;
					}
				}
			}
		}

		void DoAction(const int32 action)
		{
			switch (action)
			{
				case ACTION_START_INTRO:
					events.ScheduleEvent(EVENT_MOVE_INTO_ALGALON, 1);
					break;
			}
		}

		void UpdateAI(uint32 const diff)
		{
			UpdateVictim();

			if (events.Empty())
				return;

			events.Update(diff);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				    case EVENT_MOVE_INTO_ALGALON:
						DoSendQuantumText(SAY_BRANN_CONSOLE, me);
						me->SetWalk(true);
						me->GetMotionMaster()->MovePoint(0, BrannMoveToAlgalon[0]);
						events.ScheduleEvent(EVENT_SAY_INTRO_BRANN, 17000);
						break;
					case EVENT_SAY_INTRO_BRANN:
						DoSendQuantumText(SAY_BRANN_ALGALON, me);
						events.ScheduleEvent(EVENT_ALGALON_INTRO, 6000);
						break;
					case EVENT_ALGALON_INTRO:
						events.ScheduleEvent(EVENT_BRANN_REACHED_HOME, 1);
						break;
					case EVENT_BRANN_REACHED_HOME:
						me->GetMotionMaster()->MovePoint(0, BrannMoveToHome[0]);
						events.ScheduleEvent(EVENT_BRANN_DESPAWN, 14000);
						break;
					case EVENT_BRANN_DESPAWN:
						me->DespawnAfterAction();
						break;
				}
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_brann_bronzebeard_algalonAI(creature);
	}
};

class npc_algalon_stalker_asteroid_1 : public CreatureScript
{
public:
	npc_algalon_stalker_asteroid_1() : CreatureScript("npc_algalon_stalker_asteroid_1") { }
	
	struct npc_algalon_stalker_asteroid_1AI : public QuantumBasicAI
	{
		npc_algalon_stalker_asteroid_1AI(Creature* creature) : QuantumBasicAI(creature){}

		void Reset()
		{
			DoCast(me, SPELL_COSMIC_SMASH_VISUAL, true);
			DoCast(me, RAID_MODE(SPELL_COSMIC_SMASH_TRG_DMG_10, SPELL_COSMIC_SMASH_TRG_DMG_25), true);
		}

		void UpdateAI(uint32 const /*diff*/){}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_algalon_stalker_asteroid_1AI(creature);
	}
};

void AddSC_boss_algalon_the_observer()
{
    new boss_algalon_the_observer();
    new npc_collapsing_star();
    new npc_living_constellation();
    new npc_black_hole();
    new go_celestial_planetarium_access();
    new spell_cosmic_smash();
    new spell_cosmic_smash_summon_trigger();
    new spell_cosmic_smash_summon_target();
    new spell_cosmic_smash_dmg();
    new spell_cosmic_smash_missile_target();
    new spell_remove_player_from_phase();
    new spell_algalon_phased();
    new achievement_herald_of_the_titans();
	new npc_brann_bronzebeard_algalon();
	new npc_algalon_stalker_asteroid_1();
}