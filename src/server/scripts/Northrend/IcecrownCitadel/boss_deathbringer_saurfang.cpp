/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "QuantumGossip.h"
#include "SpellAuras.h"
#include "Group.h"
#include "icecrown_citadel.h"

enum Texts
{
	// Muradin Bronzebeard
    SAY_INTRO_ALLIANCE_1            = -1659029,
	SAY_INTRO_ALLIANCE_2            = -1659030,
    SAY_INTRO_ALLIANCE_3            = -1659031,
    SAY_INTRO_ALLIANCE_4            = -1659032,
    SAY_INTRO_ALLIANCE_5            = -1659033,
	SAY_INTRO_ALLIANCE_6            = -1659034,
    SAY_INTRO_ALLIANCE_7            = -1659035,
    // Deathbringer Saurfang
	SAY_INTRO_HORDE_1               = -1659036,
	SAY_INTRO_HORDE_2               = -1659037,
    SAY_INTRO_HORDE_3               = -1659038,
	SAY_INTRO_HORDE_4               = -1659039,
    SAY_INTRO_HORDE_5               = -1659040,
    SAY_INTRO_HORDE_6               = -1659041,
    SAY_INTRO_HORDE_7               = -1659042,
    SAY_INTRO_HORDE_8               = -1659043,
    SAY_INTRO_HORDE_9               = -1659044,
	// Deathbringer Saurfang
    SAY_AGGRO                       = -1659045,
    SAY_MARK_OF_THE_FALLEN_CHAMPION = -1659046,
    SAY_BLOOD_BEASTS                = -1659047,
    SAY_KILL_1                      = -1659048,
	SAY_KILL_2                      = -1659049,
    SAY_FRENZY                      = -1659050,
    SAY_BERSERK                     = -1659051,
    SAY_DEATH                       = -1659052,
    EMOTE_SCENT_OF_BLOOD            = -1669053,
    // Alliance Outro
	SAY_OUTRO_ALLIANCE_1            = -1659053,
    SAY_OUTRO_ALLIANCE_2            = -1659054,
    SAY_OUTRO_ALLIANCE_3            = -1659055,
    SAY_OUTRO_ALLIANCE_4            = -1659056,
    SAY_OUTRO_ALLIANCE_5            = -1659057,
    SAY_OUTRO_ALLIANCE_6            = -1659058,
    SAY_OUTRO_ALLIANCE_7            = -1659059,
	SAY_OUTRO_ALLIANCE_8            = -1659060,
    SAY_OUTRO_ALLIANCE_9            = -1659061,
    SAY_OUTRO_ALLIANCE_10           = -1659062,
	SAY_OUTRO_ALLIANCE_11           = -1659063,
	SAY_OUTRO_ALLIANCE_12           = -1659064,
    SAY_OUTRO_ALLIANCE_13           = -1659065,
    SAY_OUTRO_ALLIANCE_14           = -1659066,
    SAY_OUTRO_ALLIANCE_15           = -1659067,
	SAY_OUTRO_ALLIANCE_16           = -1659068,
	SAY_OUTRO_ALLIANCE_17           = -1659069,
	SAY_OUTRO_ALLIANCE_18           = -1659070,
	SAY_OUTRO_ALLIANCE_19           = -1659071,
	SAY_OUTRO_ALLIANCE_20           = -1659072,
    SAY_OUTRO_ALLIANCE_21           = -1659073,
	// Horde Outro
    SAY_OUTRO_HORDE_1               = -1659074,
    SAY_OUTRO_HORDE_2               = -1659075,
    SAY_OUTRO_HORDE_3               = -1659076,
    SAY_OUTRO_HORDE_4               = -1659077,
};

enum Spells
{
    // Deathbringer Saurfang
    SPELL_ZERO_POWER                    = 72242,
    SPELL_GRIP_OF_AGONY                 = 70572, // Intro
    SPELL_BLOOD_LINK                    = 72178,
    SPELL_MARK_OF_THE_FALLEN_CHAMPION_S = 72256,
    SPELL_RUNE_OF_BLOOD_S               = 72408,
    SPELL_SUMMON_BLOOD_BEAST            = 72172,
    SPELL_SUMMON_BLOOD_BEAST_25_MAN     = 72356, // Additional cast, does not replace
    SPELL_FRENZY                        = 72737,
    SPELL_BLOOD_NOVA_TRIGGER            = 72378,
    SPELL_BLOOD_NOVA                    = 72380,
    SPELL_BLOOD_POWER                   = 72371,
    SPELL_BLOOD_LINK_POWER              = 72195,
    SPELL_BLOOD_LINK_DUMMY              = 72202,
    SPELL_MARK_OF_THE_FALLEN_CHAMPION   = 72293,
    SPELL_BOILING_BLOOD                 = 72385,
    SPELL_RUNE_OF_BLOOD                 = 72410,
    // Blood Beast
    SPELL_BLOOD_LINK_BEAST              = 72176,
    SPELL_RESISTANT_SKIN                = 72723,
    SPELL_SCENT_OF_BLOOD                = 72769, // Heroic only
	SPELL_SCENT_OF_BLOOD_TRIGGERED      = 72771, // Damage + Visual on Blood Beasts
    SPELL_RIDE_VEHICLE                  = 70640, // Outro
    SPELL_ACHIEVEMENT                   = 72928,
    SPELL_REMOVE_MARKS_OF_THE_FALLEN_CHAMPION = 72257,
    SPELL_PERMANENT_FEIGN_DEATH         = 70628,
};

// Helper to get id of the aura on different modes (HasAura(baseId) wont work)
#define BOILING_BLOOD_HELPER RAID_MODE<int32>(72385, 72441, 72442, 72443)

enum EventTypes
{
    EVENT_INTRO_ALLIANCE_1      = 1,
    EVENT_INTRO_ALLIANCE_2      = 2,
    EVENT_INTRO_ALLIANCE_3      = 3,
    EVENT_INTRO_ALLIANCE_4      = 4,
    EVENT_INTRO_ALLIANCE_5      = 5,
    EVENT_INTRO_ALLIANCE_6      = 6,
    EVENT_INTRO_ALLIANCE_7      = 7,

    EVENT_INTRO_HORDE_1         = 8,
    EVENT_INTRO_HORDE_2         = 9,
    EVENT_INTRO_HORDE_3         = 10,
    EVENT_INTRO_HORDE_4         = 11,
    EVENT_INTRO_HORDE_5         = 12,
    EVENT_INTRO_HORDE_6         = 13,
    EVENT_INTRO_HORDE_7         = 14,
    EVENT_INTRO_HORDE_8         = 15,
    EVENT_INTRO_HORDE_9         = 16,

    EVENT_INTRO_FINISH          = 17,

    EVENT_BERSERK               = 18,
    EVENT_SUMMON_BLOOD_BEAST    = 19,
    EVENT_BOILING_BLOOD         = 20,
    EVENT_BLOOD_NOVA            = 21,
    EVENT_RUNE_OF_BLOOD         = 22,
    EVENT_SCENT_OF_BLOOD        = 52,

    EVENT_OUTRO_ALLIANCE_1      = 23,
    EVENT_OUTRO_ALLIANCE_2      = 24,
    EVENT_OUTRO_ALLIANCE_3      = 25,
    EVENT_OUTRO_ALLIANCE_4      = 26,
    EVENT_OUTRO_ALLIANCE_5      = 27,
    EVENT_OUTRO_ALLIANCE_6      = 28,
    EVENT_OUTRO_ALLIANCE_7      = 29,
    EVENT_OUTRO_ALLIANCE_8      = 30,
    EVENT_OUTRO_ALLIANCE_9      = 31,
    EVENT_OUTRO_ALLIANCE_10     = 32,
    EVENT_OUTRO_ALLIANCE_11     = 33,
    EVENT_OUTRO_ALLIANCE_12     = 34,
    EVENT_OUTRO_ALLIANCE_13     = 35,
    EVENT_OUTRO_ALLIANCE_14     = 36,
    EVENT_OUTRO_ALLIANCE_15     = 37,
    EVENT_OUTRO_ALLIANCE_16     = 38,
    EVENT_OUTRO_ALLIANCE_17     = 39,
    EVENT_OUTRO_ALLIANCE_18     = 40,
    EVENT_OUTRO_ALLIANCE_19     = 41,
    EVENT_OUTRO_ALLIANCE_20     = 42,
    EVENT_OUTRO_ALLIANCE_21     = 43,

    EVENT_OUTRO_HORDE_1         = 44,
    EVENT_OUTRO_HORDE_2         = 45,
    EVENT_OUTRO_HORDE_3         = 46,
    EVENT_OUTRO_HORDE_4         = 47,
    EVENT_OUTRO_HORDE_5         = 48,
    EVENT_OUTRO_HORDE_6         = 49,
    EVENT_OUTRO_HORDE_7         = 50,
    EVENT_OUTRO_HORDE_8         = 51,
};

enum Phases
{
    PHASE_INTRO_A       = 1,
    PHASE_INTRO_H       = 2,
    PHASE_COMBAT        = 3
};

enum Actions
{
    ACTION_START_EVENT                  = -3781300,
    ACTION_CONTINUE_INTRO               = -3781301,
    ACTION_CHARGE                       = -3781302,
    ACTION_START_OUTRO                  = -3781303,
    ACTION_DESPAWN                      = -3781304,
    ACTION_INTERRUPT_INTRO              = -3781305,
    ACTION_MARK_OF_THE_FALLEN_CHAMPION  = -72293,
};

#define DATA_MADE_A_MESS 45374613 // 4537, 4613 are achievement IDs

enum MovePoints
{
    POINT_SAURFANG          = 3781300,
    POINT_FIRST_STEP        = 3781301,
    POINT_CHARGE            = 3781302,
    POINT_CHOKE             = 3781303,
    POINT_CORPSE            = 3781304,
    POINT_FINAL             = 3781305,
    POINT_EXIT              = 5,        // waypoint id
};

Position const DeathbringerPos = {-496.3542f, 2211.33f, 541.1138f, 0.0f};
Position const FirstStepPos = {-541.3177f, 2211.365f, 539.2921f, 0.0f};

Position const ChargePos[6] =
{
    {-509.6505f, 2211.377f, 539.2872f, 0.0f}, // High Overlord Saurfang/Muradin Bronzebeard
    {-508.7480f, 2211.897f, 539.2870f, 0.0f}, // front left
    {-509.2929f, 2211.411f, 539.2870f, 0.0f}, // front right
    {-506.6607f, 2211.367f, 539.2870f, 0.0f}, // back middle
    {-506.1137f, 2213.306f, 539.2870f, 0.0f}, // back left
    {-509.0040f, 2211.743f, 539.2870f, 0.0f}  // back right
};

Position const ChokePos[6] =
{
    {-514.4834f, 2211.334f, 549.2887f, 0.0f}, // High Overlord Saurfang/Muradin Bronzebeard
    {-510.1081f, 2211.592f, 546.3773f, 0.0f}, // front left
    {-513.3210f, 2211.396f, 551.2882f, 0.0f}, // front right
    {-507.3684f, 2210.353f, 545.7497f, 0.0f}, // back middle
    {-507.0486f, 2212.999f, 545.5512f, 0.0f}, // back left
    {-510.7041f, 2211.069f, 546.5298f, 0.0f}  // back right
};

Position const FinalPos = {-563.7552f, 2211.328f, 538.7848f, 0.0f};

class boss_deathbringer_saurfang : public CreatureScript
{
    public:
        boss_deathbringer_saurfang() : CreatureScript("boss_deathbringer_saurfang") { }

        struct boss_deathbringer_saurfangAI : public BossAI
        {
            boss_deathbringer_saurfangAI(Creature* creature) : BossAI(creature, DATA_DEATHBRINGER_SAURFANG)
            {
                ASSERT(creature->GetVehicleKit()); // we dont actually use it, just check if exists
                IntroDone = false;
                fallenChampionCastCount = 0;
            }

			uint32 fallenChampionCastCount;

            bool IntroDone;
            bool Frenzied;
            bool Dead;

            void Reset()
            {
                _Reset();
                me->SetReactState(REACT_DEFENSIVE);
                events.SetPhase(PHASE_COMBAT);

                Frenzied = false;
                Dead = false;

                me->SetPower(POWER_ENERGY, 0);

				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
                DoCast(me, SPELL_ZERO_POWER, true);
                DoCast(me, SPELL_BLOOD_LINK, true);
                DoCast(me, SPELL_BLOOD_POWER, true);
                DoCast(me, SPELL_MARK_OF_THE_FALLEN_CHAMPION_S, true);
                DoCast(me, SPELL_RUNE_OF_BLOOD_S, true);
				DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);

                me->RemoveAurasDueToSpell(SPELL_BERSERK);
                me->RemoveAurasDueToSpell(SPELL_FRENZY);
            }

            void EnterToBattle(Unit* who)
            {
                if (Dead)
                    return;

                if (!instance->CheckRequiredBosses(DATA_DEATHBRINGER_SAURFANG, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    instance->DoCastSpellOnPlayers(SPELL_LIGHTS_HAMMER_TELEPORT);
                    return;
                }

                // oh just screw intro, enter combat - no exploits please
                me->SetActive(true);
                DoZoneInCombat();

                events.Reset();
                events.SetPhase(PHASE_COMBAT);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                if (!IntroDone)
                {
                    DoCast(me, SPELL_GRIP_OF_AGONY);

                    if (Creature* creature = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_SAURFANG_EVENT_NPC)))
                        creature->AI()->DoAction(ACTION_INTERRUPT_INTRO);
                }

                IntroDone = true;

                DoSendQuantumText(SAY_AGGRO, me);
                events.ScheduleEvent(EVENT_SUMMON_BLOOD_BEAST, 30000, 0, PHASE_COMBAT);
                events.ScheduleEvent(EVENT_BERSERK, IsHeroic() ? 360000 : 480000, 0, PHASE_COMBAT);
                events.ScheduleEvent(EVENT_BOILING_BLOOD, 15500, 0, PHASE_COMBAT);
                events.ScheduleEvent(EVENT_BLOOD_NOVA, 17000, 0, PHASE_COMBAT);
                events.ScheduleEvent(EVENT_RUNE_OF_BLOOD, 20000, 0, PHASE_COMBAT);

                fallenChampionCastCount = 0;
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_OF_THE_FALLEN_CHAMPION);
                instance->SetBossState(DATA_DEATHBRINGER_SAURFANG, IN_PROGRESS);
            }

            void JustDied(Unit* /*killer*/)
			{
				summons.DespawnAll();
			}

            void AttackStart(Unit* victim)
            {
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER))
                    return;

                QuantumBasicAI::AttackStart(victim);
            }

            void EnterEvadeMode()
            {
                QuantumBasicAI::EnterEvadeMode();
                if (IntroDone)
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            }

            void JustReachedHome()
            {
                _JustReachedHome();
                instance->SetBossState(DATA_DEATHBRINGER_SAURFANG, FAIL);
                instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_OF_THE_FALLEN_CHAMPION);
            }

            void KilledUnit(Unit* who)
            {
                if (who->GetTypeId() == TYPE_ID_PLAYER)
                    DoSendQuantumText(RAND(SAY_KILL_1, SAY_KILL_2), me);
            }

            void DamageTaken(Unit* attacker, uint32& damage)
            {
                if (damage >= me->GetHealth())
                    damage = me->GetHealth() - 1;

                if (!Frenzied && HealthBelowPct(31)) // AT 30%, not below
                {
                    Frenzied = true;
                    DoCast(me, SPELL_FRENZY);
                    DoSendQuantumText(SAY_FRENZY, me);
                }

                if (!Dead && me->GetHealth() < FightWonValue)
                {
                    Dead = true;
                    _JustDied();
                    _EnterEvadeMode();
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NOT_SELECTABLE);
                    DoCastAOE(SPELL_REMOVE_MARKS_OF_THE_FALLEN_CHAMPION);
                    DoCast(me, SPELL_ACHIEVEMENT, true);
                    DoSendQuantumText(SAY_DEATH, me);

					instance->HandleGameObject(instance->GetData64(GO_SAURFANG_S_DOOR), true);

                    DoCast(me, SPELL_PERMANENT_FEIGN_DEATH, true);
                    if (Creature* creature = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_SAURFANG_EVENT_NPC)))
                        creature->AI()->DoAction(ACTION_START_OUTRO);

					// Quest credit
					if (Player* player = attacker->ToPlayer())
					{
						player->CastSpell(player, SPELL_SOUL_FEAST, true);
						me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
					}
				}
            }

            void JustSummoned(Creature* summon)
            {
				if (summon->GetEntry() == NPC_BLOOD_BEAST)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 0.0f, true))
						summon->AI()->AttackStart(target);

					summon->CastSpell(summon, SPELL_BLOOD_LINK_BEAST, true);
					summon->CastSpell(summon, SPELL_RESISTANT_SKIN, true);
					summons.Summon(summon);
					DoZoneInCombat(summon);
				}
            }

            void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
            {
                summons.Despawn(summon);
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE && id != POINT_SAURFANG)
                    return;

                instance->HandleGameObject(instance->GetData64(GO_SAURFANG_S_DOOR), false);
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell)
            {
                switch (spell->Id)
                {
                    case SPELL_MARK_OF_THE_FALLEN_CHAMPION:
                        DoSendQuantumText(SAY_MARK_OF_THE_FALLEN_CHAMPION, me);
                        break;
                    case 72255: // Mark of the Fallen Champion, triggered id
                    case 72444:
                    case 72445:
                    case 72446:
                        if (me->GetPower(POWER_ENERGY) != me->GetMaxPower(POWER_ENERGY))
                            target->CastCustomSpell(SPELL_BLOOD_LINK_DUMMY, SPELLVALUE_BASE_POINT0, 1, me, true);
                        break;
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const diff)
            {
				if (!UpdateVictim() && !(events.IsInPhase(PHASE_INTRO_A) || events.IsInPhase(PHASE_INTRO_H)))
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INTRO_ALLIANCE_2:
                            DoSendQuantumText(SAY_INTRO_ALLIANCE_2, me);
                            break;
                        case EVENT_INTRO_ALLIANCE_3:
                            DoSendQuantumText(SAY_INTRO_ALLIANCE_3, me);
                            break;
                        case EVENT_INTRO_ALLIANCE_6:
                            DoSendQuantumText(SAY_INTRO_ALLIANCE_6, me);
                            DoSendQuantumText(SAY_INTRO_ALLIANCE_7, me);
                            DoCast(me, SPELL_GRIP_OF_AGONY);
                            break;
                        case EVENT_INTRO_HORDE_2:
                            DoSendQuantumText(SAY_INTRO_HORDE_2, me);
                            break;
                        case EVENT_INTRO_HORDE_4:
                            DoSendQuantumText(SAY_INTRO_HORDE_4, me);
                            break;
                        case EVENT_INTRO_HORDE_9:
                            DoCast(me, SPELL_GRIP_OF_AGONY);
                            DoSendQuantumText(SAY_INTRO_HORDE_9, me);
                            break;
                        case EVENT_INTRO_FINISH:
                            events.SetPhase(PHASE_COMBAT);
                            IntroDone = true;
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                            break;
                        case EVENT_SUMMON_BLOOD_BEAST:
                            for (uint32 i10 = 0; i10 < 2; ++i10)
                                DoCast(me, SPELL_SUMMON_BLOOD_BEAST+i10);
                            if (Is25ManRaid())
                                for (uint32 i25 = 0; i25 < 3; ++i25)
                                    DoCast(me, SPELL_SUMMON_BLOOD_BEAST_25_MAN+i25);
                            DoSendQuantumText(SAY_BLOOD_BEASTS, me);
                            events.ScheduleEvent(EVENT_SUMMON_BLOOD_BEAST, 40000, 0, PHASE_COMBAT);
                            if (IsHeroic())
                                events.ScheduleEvent(EVENT_SCENT_OF_BLOOD, 10000, 0, PHASE_COMBAT);
                            break;
                        case EVENT_BLOOD_NOVA:
                            DoCastAOE(SPELL_BLOOD_NOVA_TRIGGER);
                            events.ScheduleEvent(EVENT_BLOOD_NOVA, urand(20000, 25000), 0, PHASE_COMBAT);
                            break;
                        case EVENT_RUNE_OF_BLOOD:
                            DoCastVictim(SPELL_RUNE_OF_BLOOD);
                            events.ScheduleEvent(EVENT_RUNE_OF_BLOOD, urand(20000, 25000), 0, PHASE_COMBAT);
                            break;
                        case EVENT_BOILING_BLOOD:
                            DoCast(me, SPELL_BOILING_BLOOD);
                            events.ScheduleEvent(EVENT_BOILING_BLOOD, urand(15000, 20000), 0, PHASE_COMBAT);
                            break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK);
                            DoSendQuantumText(SAY_BERSERK, me);
                            break;
                        case EVENT_SCENT_OF_BLOOD:
                            if (!summons.empty())
                            {
                                DoSendQuantumText(EMOTE_SCENT_OF_BLOOD, me);
                                DoCastAOE(SPELL_SCENT_OF_BLOOD);

								// hack: ensure dmg buff on blood beasts
								for (SummonList::iterator itr = summons.begin(); itr != summons.end(); ++itr)
								{
									if (Unit* unit = ObjectAccessor::GetUnit(*me, *itr))
									{
										if (unit->IsAlive() && unit->GetEntry() == NPC_BLOOD_BEAST)
											unit->AddAura(SPELL_SCENT_OF_BLOOD_TRIGGERED, unit);
									}
								}
                            }
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            uint32 GetData(uint32 type)
            {
                if (type == DATA_MADE_A_MESS)
                    if (fallenChampionCastCount < RAID_MODE<uint32>(3, 5, 3, 5))
                        return 1;

                return 0;
            }

            // intro setup
            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case PHASE_INTRO_A:
                    case PHASE_INTRO_H:
                    {
                        if (GameObject* teleporter = GameObject::GetGameObject(*me, instance->GetData64(GO_SCOURGE_TRANSPORTER_SAURFANG)))
                        {
                            instance->HandleGameObject(0, false, teleporter);
                            teleporter->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
                        }

                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        // controls what events will execute
                        events.SetPhase(uint32(action));

                        me->SetHomePosition(DeathbringerPos.GetPositionX(), DeathbringerPos.GetPositionY(), DeathbringerPos.GetPositionZ(), me->GetOrientation());
                        me->GetMotionMaster()->MovePoint(POINT_SAURFANG, DeathbringerPos.GetPositionX(), DeathbringerPos.GetPositionY(), DeathbringerPos.GetPositionZ());

                        events.ScheduleEvent(EVENT_INTRO_ALLIANCE_2, 2500, 0, PHASE_INTRO_A);
                        events.ScheduleEvent(EVENT_INTRO_ALLIANCE_3, 20000, 0, PHASE_INTRO_A);

						events.ScheduleEvent(EVENT_INTRO_HORDE_2, 6200, 0, PHASE_INTRO_H);
                        break;
                    }
                    case ACTION_CONTINUE_INTRO:
                    {
                        if (IntroDone)
                            return;

                        events.ScheduleEvent(EVENT_INTRO_ALLIANCE_6, 6500+500, 0, PHASE_INTRO_A);
                        events.ScheduleEvent(EVENT_INTRO_FINISH, 8000, 0, PHASE_INTRO_A);

                        events.ScheduleEvent(EVENT_INTRO_HORDE_4, 6500, 0, PHASE_INTRO_H);
                        events.ScheduleEvent(EVENT_INTRO_HORDE_9, 46700+1000+500, 0, PHASE_INTRO_H);
                        events.ScheduleEvent(EVENT_INTRO_FINISH,  46700+1000+8000, 0, PHASE_INTRO_H);
                        break;
                    }
                    case ACTION_MARK_OF_THE_FALLEN_CHAMPION:
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 0.0f, true, -SPELL_MARK_OF_THE_FALLEN_CHAMPION))
                        {
                            ++fallenChampionCastCount;
                            DoCast(target, SPELL_MARK_OF_THE_FALLEN_CHAMPION);
                            me->SetPower(POWER_ENERGY, 0);
                            if (Aura* bloodPower = me->GetAura(SPELL_BLOOD_POWER))
                                bloodPower->RecalculateAmountOfEffects();
                        }
                        break;
                    }
                    default:
                        break;
                }
            }

            static uint32 const FightWonValue;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<boss_deathbringer_saurfangAI>(creature);
        }
};

uint32 const boss_deathbringer_saurfang::boss_deathbringer_saurfangAI::FightWonValue = 100000;

class npc_high_overlord_saurfang_icc : public CreatureScript
{
    public:
        npc_high_overlord_saurfang_icc() : CreatureScript("npc_high_overlord_saurfang_icc") { }

		bool OnGossipHello(Player* player, Creature* creature)
        {
			if ((!player->GetGroup() || !player->GetGroup()->IsLeader(player->GetGUID())) && !player->IsGameMaster())
			{
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "You aren't the leader of raid...", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
				player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
				return true;
			}

            InstanceScript* instance = creature->GetInstanceScript();
            if (instance && instance->GetBossState(DATA_DEATHBRINGER_SAURFANG) != DONE)
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "We are ready to go, High Overlord. The Lich King must fall!", 631, -ACTION_START_EVENT);
                player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            }
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            player->CLOSE_GOSSIP_MENU();

			if (action == GOSSIP_ACTION_INFO_DEF+2)
				creature->MonsterSay("I will wait for the leader of yours raid", LANG_UNIVERSAL, player->GetGUID());

            if (action == -ACTION_START_EVENT)
                creature->AI()->DoAction(ACTION_START_EVENT);

            return true;
        }

        struct npc_high_overlord_saurfangAI : public QuantumBasicAI
        {
            npc_high_overlord_saurfangAI(Creature* creature) : QuantumBasicAI(creature)
            {
                ASSERT(creature->GetVehicleKit());
                instance = me->GetInstanceScript();
            }

			EventMap events;
            InstanceScript* instance;
            std::list<Creature*> _guardList;

            void Reset()
            {
                events.Reset();
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_START_EVENT:
                    {
                        // Prevent crashes
                        if (events.IsInPhase(PHASE_INTRO_A) || events.IsInPhase(PHASE_INTRO_H))
                            return;

                        GetCreatureListWithEntryInGrid(_guardList, me, NPC_SE_KOR_KRON_REAVER, 20.0f);
                        _guardList.sort(Trinity::ObjectDistanceOrderPred(me));
                        uint32 x = 1;
                        for (std::list<Creature*>::iterator itr = _guardList.begin(); itr != _guardList.end(); ++x, ++itr)
                            (*itr)->AI()->SetData(0, x);

                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        DoSendQuantumText(SAY_INTRO_HORDE_1, me);
                        events.SetPhase(PHASE_INTRO_H);
                        events.ScheduleEvent(EVENT_INTRO_HORDE_3, 18500, 0, PHASE_INTRO_H);
                        instance->HandleGameObject(instance->GetData64(GO_SAURFANG_S_DOOR), true);
                        if (Creature* deathbringer = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_DEATHBRINGER_SAURFANG)))
                            deathbringer->AI()->DoAction(PHASE_INTRO_H);
                        break;
                    }
                    case ACTION_START_OUTRO:
                    {
                        me->RemoveAurasDueToSpell(SPELL_GRIP_OF_AGONY);
                        DoSendQuantumText(SAY_OUTRO_HORDE_1, me);
                        events.ScheduleEvent(EVENT_OUTRO_HORDE_2, 10000);   // say
                        events.ScheduleEvent(EVENT_OUTRO_HORDE_3, 18000);   // say
                        events.ScheduleEvent(EVENT_OUTRO_HORDE_4, 24000);   // cast
                        events.ScheduleEvent(EVENT_OUTRO_HORDE_5, 30000);   // move
                        me->SetDisableGravity(false);
                        me->GetMotionMaster()->MoveFall();
                        for (std::list<Creature*>::iterator itr = _guardList.begin(); itr != _guardList.end(); ++itr)
                            (*itr)->AI()->DoAction(ACTION_DESPAWN);
                        break;
                    }
                    case ACTION_INTERRUPT_INTRO:
                    {
                        events.Reset();
                        for (std::list<Creature*>::iterator itr = _guardList.begin(); itr != _guardList.end(); ++itr)
                            (*itr)->AI()->DoAction(ACTION_DESPAWN);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_GRIP_OF_AGONY)
                {
                    me->SetDisableGravity(true);
                    me->GetMotionMaster()->MovePoint(POINT_CHOKE, ChokePos[0]);
                }
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type == POINT_MOTION_TYPE)
                {
                    switch (id)
                    {
                        case POINT_FIRST_STEP:
                            me->SetWalk(false);
                            DoSendQuantumText(SAY_INTRO_HORDE_3, me);
                            events.ScheduleEvent(EVENT_INTRO_HORDE_5, 15500, 0, PHASE_INTRO_H);
                            events.ScheduleEvent(EVENT_INTRO_HORDE_6, 29500, 0, PHASE_INTRO_H);
                            events.ScheduleEvent(EVENT_INTRO_HORDE_7, 43800, 0, PHASE_INTRO_H);
                            events.ScheduleEvent(EVENT_INTRO_HORDE_8, 47000, 0, PHASE_INTRO_H);
                            if (Creature* deathbringer = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_DEATHBRINGER_SAURFANG)))
                                deathbringer->AI()->DoAction(ACTION_CONTINUE_INTRO);
                            break;
                        case POINT_CORPSE:
                            if (Creature* deathbringer = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_DEATHBRINGER_SAURFANG)))
                            {
                                deathbringer->CastSpell(me, SPELL_RIDE_VEHICLE, true);  // for the packet logs.
                                deathbringer->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                deathbringer->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_DROWNED);
                            }
                            events.ScheduleEvent(EVENT_OUTRO_HORDE_5, 1000);    // move
                            events.ScheduleEvent(EVENT_OUTRO_HORDE_6, 4000);    // say
                            break;
                        case POINT_FINAL:
                            if (Creature* deathbringer = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_DEATHBRINGER_SAURFANG)))
                                deathbringer->DespawnAfterAction();
                            me->DespawnAfterAction();
                            break;
                        default:
                            break;
                    }
                }
                else if (type == WAYPOINT_MOTION_TYPE && id == POINT_EXIT)
                {
                    std::list<Creature*> guards;
                    GetCreatureListWithEntryInGrid(guards, me, NPC_KORKRON_GENERAL, 50.0f);
                    for (std::list<Creature*>::iterator itr = guards.begin(); itr != guards.end(); ++itr)
                        (*itr)->DespawnAfterAction();
                    me->DespawnAfterAction();
                }
            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INTRO_HORDE_3:
                            me->SetWalk(true);
                            me->GetMotionMaster()->MovePoint(POINT_FIRST_STEP, FirstStepPos.GetPositionX(), FirstStepPos.GetPositionY(), FirstStepPos.GetPositionZ());
                            break;
                        case EVENT_INTRO_HORDE_5:
                            DoSendQuantumText(SAY_INTRO_HORDE_5, me);
                            break;
                        case EVENT_INTRO_HORDE_6:
                            DoSendQuantumText(SAY_INTRO_HORDE_6, me);
                            break;
                        case EVENT_INTRO_HORDE_7:
                            DoSendQuantumText(SAY_INTRO_HORDE_7, me);
                            break;
                        case EVENT_INTRO_HORDE_8:
                            DoSendQuantumText(SAY_INTRO_HORDE_8, me);
                            for (std::list<Creature*>::iterator itr = _guardList.begin(); itr != _guardList.end(); ++itr)
                                (*itr)->AI()->DoAction(ACTION_CHARGE);
                            me->GetMotionMaster()->MoveCharge(ChargePos[0].GetPositionX(), ChargePos[0].GetPositionY(), ChargePos[0].GetPositionZ(), 8.5f, POINT_CHARGE);
                            break;
                        case EVENT_OUTRO_HORDE_2:
                            if (Creature* deathbringer = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_DEATHBRINGER_SAURFANG)))
                                me->SetFacingToObject(deathbringer);

                            DoSendQuantumText(SAY_OUTRO_HORDE_2, me);
                            break;
                        case EVENT_OUTRO_HORDE_3:
                            DoSendQuantumText(SAY_OUTRO_HORDE_3, me);
                            break;
                        case EVENT_OUTRO_HORDE_4:
                            if (Creature* deathbringer = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_DEATHBRINGER_SAURFANG)))
                            {
                                float x, y, z;
                                deathbringer->GetClosePoint(x, y, z, deathbringer->GetObjectSize());
                                me->SetWalk(true);
                                me->GetMotionMaster()->MovePoint(POINT_CORPSE, x, y, z);
                            }
                            break;
                        case EVENT_OUTRO_HORDE_5:
                            me->GetMotionMaster()->MovePoint(POINT_FINAL, FinalPos);
                            break;
                        case EVENT_OUTRO_HORDE_6:
                            DoSendQuantumText(SAY_OUTRO_HORDE_4, me);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_high_overlord_saurfangAI>(creature);
        }
};

class npc_muradin_bronzebeard_icc : public CreatureScript
{
    public:
        npc_muradin_bronzebeard_icc() : CreatureScript("npc_muradin_bronzebeard_icc") { }

		bool OnGossipHello(Player* player, Creature* creature)
        {
			if ((!player->GetGroup() || !player->GetGroup()->IsLeader(player->GetGUID())) && !player->IsGameMaster())
			{
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "You aren't the leader of raid...", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
				player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
				return true;
			}

            InstanceScript* instance = creature->GetInstanceScript();
            if (instance && instance->GetBossState(DATA_DEATHBRINGER_SAURFANG) != DONE)
            {
                player->ADD_GOSSIP_ITEM(0, "We are ready to fight...", 631, -ACTION_START_EVENT + 1);
                player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            }
            return true;
        }

        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            player->CLOSE_GOSSIP_MENU();

			if (action == GOSSIP_ACTION_INFO_DEF+2)
				creature->MonsterSay("I will wait for the leader of yours raid", LANG_UNIVERSAL, player->GetGUID());

            if (action == -ACTION_START_EVENT + 1)
                creature->AI()->DoAction(ACTION_START_EVENT);

            return true;
        }

        struct npc_muradin_bronzebeard_iccAI : public QuantumBasicAI
        {
            npc_muradin_bronzebeard_iccAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = me->GetInstanceScript();
            }

			EventMap events;
            InstanceScript* instance;
            std::list<Creature*> _guardList;

            void Reset()
            {
                events.Reset();
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_START_EVENT:
                    {
                        // Prevent crashes
						if (events.IsInPhase(PHASE_INTRO_A) || events.IsInPhase(PHASE_INTRO_H))
                            return;

                        events.SetPhase(PHASE_INTRO_A);
                        GetCreatureListWithEntryInGrid(_guardList, me, NPC_SE_SKYBREAKER_MARINE, 20.0f);
                        _guardList.sort(Trinity::ObjectDistanceOrderPred(me));
                        uint32 x = 1;
                        for (std::list<Creature*>::iterator itr = _guardList.begin(); itr != _guardList.end(); ++x, ++itr)
                            (*itr)->AI()->SetData(0, x);

                        me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        DoSendQuantumText(SAY_INTRO_ALLIANCE_1, me);
                        events.ScheduleEvent(EVENT_INTRO_ALLIANCE_4, 2500+17500+9500, 0, PHASE_INTRO_A);
                        instance->HandleGameObject(instance->GetData64(GO_SAURFANG_S_DOOR), true);
                        if (Creature* deathbringer = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_DEATHBRINGER_SAURFANG)))
                            deathbringer->AI()->DoAction(PHASE_INTRO_A);
                        break;
                    }
                    case ACTION_START_OUTRO:
                    {
                        me->RemoveAurasDueToSpell(SPELL_GRIP_OF_AGONY);
                        DoSendQuantumText(SAY_OUTRO_ALLIANCE_1, me);
                        me->SetDisableGravity(false);
                        me->GetMotionMaster()->MoveFall();
                        for (std::list<Creature*>::iterator itr = _guardList.begin(); itr != _guardList.end(); ++itr)
                            (*itr)->AI()->DoAction(ACTION_DESPAWN);

                        // temp until outro fully done - to put deathbringer on respawn timer (until next reset)
                        if (Creature* deathbringer = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_DEATHBRINGER_SAURFANG)))
                            deathbringer->DespawnAfterAction(5000);
                        break;
                    }
                    case ACTION_INTERRUPT_INTRO:
                        events.Reset();
                        for (std::list<Creature*>::iterator itr = _guardList.begin(); itr != _guardList.end(); ++itr)
                            (*itr)->AI()->DoAction(ACTION_DESPAWN);
                        break;
                }
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_GRIP_OF_AGONY)
                {
                    me->SetDisableGravity(true);
                    me->GetMotionMaster()->MovePoint(POINT_CHOKE, ChokePos[0]);
                }
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type == POINT_MOTION_TYPE && id == POINT_FIRST_STEP)
                {
                    me->SetWalk(false);
                    DoSendQuantumText(SAY_INTRO_ALLIANCE_4, me);
                    events.ScheduleEvent(EVENT_INTRO_ALLIANCE_5, 5000, 0, PHASE_INTRO_A);
                    if (Creature* deathbringer = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_DEATHBRINGER_SAURFANG)))
                        deathbringer->AI()->DoAction(ACTION_CONTINUE_INTRO);
                }
                else if (type == WAYPOINT_MOTION_TYPE && id == POINT_EXIT)
                {
                    std::list<Creature*> guards;
                    GetCreatureListWithEntryInGrid(guards, me, NPC_ALLIANCE_COMMANDER, 50.0f);
                    for (std::list<Creature*>::iterator itr = guards.begin(); itr != guards.end(); ++itr)
                        (*itr)->DespawnAfterAction();
                    me->DespawnAfterAction();
                }
            }

            void UpdateAI(uint32 const diff)
            {
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INTRO_ALLIANCE_4:
                            me->SetWalk(true);
                            me->GetMotionMaster()->MovePoint(POINT_FIRST_STEP, FirstStepPos.GetPositionX(), FirstStepPos.GetPositionY(), FirstStepPos.GetPositionZ());
                            break;
                        case EVENT_INTRO_ALLIANCE_5:
                            DoSendQuantumText(SAY_INTRO_ALLIANCE_5, me);
                            for (std::list<Creature*>::iterator itr = _guardList.begin(); itr != _guardList.end(); ++itr)
                                (*itr)->AI()->DoAction(ACTION_CHARGE);
                            me->GetMotionMaster()->MoveCharge(ChargePos[0].GetPositionX(), ChargePos[0].GetPositionY(), ChargePos[0].GetPositionZ(), 8.5f, POINT_CHARGE);
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_muradin_bronzebeard_iccAI>(creature);
        }
};

class npc_saurfang_event : public CreatureScript
{
    public:
        npc_saurfang_event() : CreatureScript("npc_saurfang_event") { }

        struct npc_saurfang_eventAI : public QuantumBasicAI
        {
            npc_saurfang_eventAI(Creature* creature) : QuantumBasicAI(creature)
            {
                _index = 0;
            }

			uint32 _index;

            void SetData(uint32 type, uint32 data)
            {
                ASSERT(!type && data && data < 6);
                _index = data;
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_GRIP_OF_AGONY)
                {
                    me->SetDisableGravity(true);
                    me->GetMotionMaster()->MovePoint(POINT_CHOKE, ChokePos[_index]);
                }
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_CHARGE && _index)
                    me->GetMotionMaster()->MoveCharge(ChargePos[_index].GetPositionX(), ChargePos[_index].GetPositionY(), ChargePos[_index].GetPositionZ(), 13.0f, POINT_CHARGE);
                else if (action == ACTION_DESPAWN)
                    me->DespawnAfterAction();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_saurfang_eventAI>(creature);
        }
};

class spell_deathbringer_blood_link : public SpellScriptLoader
{
    public:
        spell_deathbringer_blood_link() : SpellScriptLoader("spell_deathbringer_blood_link") { }

        class spell_deathbringer_blood_link_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_deathbringer_blood_link_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BLOOD_LINK_POWER))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_BLOOD_POWER))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                GetHitUnit()->CastCustomSpell(SPELL_BLOOD_LINK_POWER, SPELLVALUE_BASE_POINT0, GetEffectValue(), GetHitUnit(), true);
                if (Aura* bloodPower = GetHitUnit()->GetAura(SPELL_BLOOD_POWER))
                    bloodPower->RecalculateAmountOfEffects();
                PreventHitDefaultEffect(EFFECT_0);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_deathbringer_blood_link_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_deathbringer_blood_link_SpellScript();
        }
};

class spell_deathbringer_blood_link_aura : public SpellScriptLoader
{
    public:
        spell_deathbringer_blood_link_aura() : SpellScriptLoader("spell_deathbringer_blood_link_aura") { }

        class spell_deathbringer_blood_link_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_deathbringer_blood_link_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_MARK_OF_THE_FALLEN_CHAMPION))
                    return false;
                return true;
            }

            void HandlePeriodicTick(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();
                if (GetUnitOwner()->GetPowerType() == POWER_ENERGY && GetUnitOwner()->GetPower(POWER_ENERGY) == GetUnitOwner()->GetMaxPower(POWER_ENERGY))
                    if (Creature* saurfang = GetUnitOwner()->ToCreature())
                        saurfang->AI()->DoAction(ACTION_MARK_OF_THE_FALLEN_CHAMPION);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_deathbringer_blood_link_AuraScript::HandlePeriodicTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_deathbringer_blood_link_AuraScript();
        }
};

class spell_deathbringer_blood_power : public SpellScriptLoader
{
    public:
        spell_deathbringer_blood_power() : SpellScriptLoader("spell_deathbringer_blood_power") { }

        class spell_deathbringer_blood_power_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_deathbringer_blood_power_SpellScript);

            void ModAuraValue()
            {
                if (Aura* aura = GetHitAura())
                    aura->RecalculateAmountOfEffects();
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_deathbringer_blood_power_SpellScript::ModAuraValue);
            }
        };

        class spell_deathbringer_blood_power_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_deathbringer_blood_power_AuraScript);

            void RecalculateHook(AuraEffect const* /*aurEffect*/, int32& amount, bool& canBeRecalculated)
            {
                amount = int32(GetUnitOwner()->GetPower(POWER_ENERGY));
                canBeRecalculated = true;
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_deathbringer_blood_power_AuraScript::RecalculateHook, EFFECT_0, SPELL_AURA_MOD_SCALE);
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_deathbringer_blood_power_AuraScript::RecalculateHook, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            }

            bool Load()
            {
                if (GetUnitOwner()->GetPowerType() != POWER_ENERGY)
                    return false;
                return true;
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_deathbringer_blood_power_SpellScript();
        }

        AuraScript* GetAuraScript() const
        {
            return new spell_deathbringer_blood_power_AuraScript();
        }
};

class spell_deathbringer_rune_of_blood : public SpellScriptLoader
{
    public:
        spell_deathbringer_rune_of_blood() : SpellScriptLoader("spell_deathbringer_rune_of_blood") { }

        class spell_deathbringer_rune_of_blood_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_deathbringer_rune_of_blood_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BLOOD_LINK_DUMMY))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);  // make this the default handler
                if (GetCaster()->GetPower(POWER_ENERGY) != GetCaster()->GetMaxPower(POWER_ENERGY))
                    GetHitUnit()->CastCustomSpell(SPELL_BLOOD_LINK_DUMMY, SPELLVALUE_BASE_POINT0, 1, GetCaster(), true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_deathbringer_rune_of_blood_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_deathbringer_rune_of_blood_SpellScript();
        }
};

class spell_deathbringer_blood_nova : public SpellScriptLoader
{
    public:
        spell_deathbringer_blood_nova() : SpellScriptLoader("spell_deathbringer_blood_nova") { }

        class spell_deathbringer_blood_nova_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_deathbringer_blood_nova_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BLOOD_LINK_DUMMY))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);  // make this the default handler
                if (GetCaster()->GetPower(POWER_ENERGY) != GetCaster()->GetMaxPower(POWER_ENERGY))
                    GetHitUnit()->CastCustomSpell(SPELL_BLOOD_LINK_DUMMY, SPELLVALUE_BASE_POINT0, 2, GetCaster(), true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_deathbringer_blood_nova_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_deathbringer_blood_nova_SpellScript();
        }
};

class spell_deathbringer_blood_nova_targeting : public SpellScriptLoader
{
    public:
        spell_deathbringer_blood_nova_targeting() : SpellScriptLoader("spell_deathbringer_blood_nova_targeting") { }

        class spell_deathbringer_blood_nova_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_deathbringer_blood_nova_targeting_SpellScript);

			Unit* target;

            bool Load()
            {
				// initialize variable
				target = 0;
                return true;
            }

            void FilterTargetsInitial(std::list<Unit*>& unitList)
            {
				if (unitList.empty())
					return;

                // select one random target, with preference of ranged targets
                uint32 targetsAtRange = 0;
                uint32 const minTargets = uint32(GetCaster()->GetMap()->GetSpawnMode() & 1 ? 10 : 4);
                unitList.sort(Trinity::ObjectDistanceOrderPred(GetCaster(), false));

                // get target count at range
                for (std::list<Unit*>::iterator itr = unitList.begin(); itr != unitList.end(); ++itr, ++targetsAtRange)
                    if ((*itr)->GetDistance(GetCaster()) < 12.0f)
                        break;

                // set the upper cap
                if (targetsAtRange < minTargets)
                    targetsAtRange = std::min<uint32>(unitList.size() - 1, minTargets);

				if (!targetsAtRange) // test
				{
					unitList.clear();
					return;
				}

                std::list<Unit*>::const_iterator itr = unitList.begin();
				std::advance(itr, urand(0, targetsAtRange));
				target = *itr;
                unitList.clear();
                unitList.push_back(target);
            }

            // use the same target for first and second effect
            void FilterTargetsSubsequent(std::list<Unit*>& unitList)
            {
				unitList.clear();
                if (!target)
                    return;

                unitList.push_back(target);
            }

			void HandleForceCast(SpellEffIndex /*effIndex*/)
			{
				GetCaster()->CastSpell(GetHitUnit(), uint32(GetEffectValue()), TRIGGERED_FULL_MASK);
			}

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_deathbringer_blood_nova_targeting_SpellScript::FilterTargetsInitial, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_deathbringer_blood_nova_targeting_SpellScript::FilterTargetsSubsequent, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
				OnEffectHitTarget += SpellEffectFn(spell_deathbringer_blood_nova_targeting_SpellScript::HandleForceCast, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_deathbringer_blood_nova_targeting_SpellScript();
        }
};

class spell_deathbringer_boiling_blood : public SpellScriptLoader
{
    public:
        spell_deathbringer_boiling_blood() : SpellScriptLoader("spell_deathbringer_boiling_blood") { }

        class spell_deathbringer_boiling_blood_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_deathbringer_boiling_blood_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPE_ID_UNIT;
            }

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove(GetCaster()->GetVictim());
                if (unitList.empty())
                    return;

                Unit* target = Quantum::DataPackets::SelectRandomContainerElement(unitList);
                unitList.clear();
                unitList.push_back(target);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_deathbringer_boiling_blood_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_deathbringer_boiling_blood_SpellScript();
        }
};

class spell_deathbringer_remove_marks : public SpellScriptLoader
{
    public:
        spell_deathbringer_remove_marks() : SpellScriptLoader("spell_deathbringer_remove_marks") { }

        class spell_deathbringer_remove_marks_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_deathbringer_remove_marks_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetHitUnit()->RemoveAurasDueToSpell(uint32(GetEffectValue()));
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_deathbringer_remove_marks_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_deathbringer_remove_marks_SpellScript();
        }
};

class achievement_ive_gone_and_made_a_mess : public AchievementCriteriaScript
{
public:
	achievement_ive_gone_and_made_a_mess() : AchievementCriteriaScript("achievement_ive_gone_and_made_a_mess") { }

	bool OnCheck(Player* /*source*/, Unit* target)
	{
		if (target)
			if (Creature* saurfang = target->ToCreature())
				if (saurfang->AI()->GetData(DATA_MADE_A_MESS))
					return true;

		return false;
	}
};

void AddSC_boss_deathbringer_saurfang()
{
    new boss_deathbringer_saurfang();
    new npc_high_overlord_saurfang_icc();
    new npc_muradin_bronzebeard_icc();
    new npc_saurfang_event();
    new spell_deathbringer_blood_link();
    new spell_deathbringer_blood_link_aura();
    new spell_deathbringer_blood_power();
    new spell_deathbringer_rune_of_blood();
    new spell_deathbringer_blood_nova();
    new spell_deathbringer_blood_nova_targeting();
    new spell_deathbringer_boiling_blood();
    new spell_deathbringer_remove_marks();
    new achievement_ive_gone_and_made_a_mess();
}