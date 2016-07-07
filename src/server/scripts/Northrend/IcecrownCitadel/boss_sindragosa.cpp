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
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"
#include "icecrown_citadel.h"

enum Texts
{
    SAY_AGGRO                           = -1659167,
    SAY_UNCHAINED_MAGIC                 = -1659168,
    EMOTE_WARN_BLISTERING_COLD          = -1659169,
    SAY_BLISTERING_COLD                 = -1659170,
    SAY_RESPITE_FOR_A_TORMENTED_SOUL    = -1659171,
    SAY_AIR_PHASE                       = -1659172,
    SAY_PHASE_2                         = -1659173,
    EMOTE_WARN_FROZEN_ORB               = -1659174,
    SAY_KILL_1                          = -1659175,
	SAY_KILL_2                          = -1659176,
    SAY_BERSERK                         = -1659177,
    SAY_DEATH                           = -1659178,
};

enum Spells
{
    // Sindragosa
    SPELL_SINDRAGOSA_S_FURY     = 70608,
    SPELL_TANK_MARKER           = 71039,
    SPELL_FROST_AURA_10N        = 70084,
	SPELL_FROST_AURA_25N        = 71050,
	SPELL_FROST_AURA_10H        = 71051,
	SPELL_FROST_AURA_25H        = 71052,
    SPELL_PERMAEATING_CHILL     = 70109,
    SPELL_CLEAVE                = 19983,
    SPELL_TAIL_SMASH            = 71077,
    SPELL_FROST_BREATH_P1       = 69649,
    SPELL_FROST_BREATH_P2       = 73061,
    SPELL_UNCHAINED_MAGIC       = 69762,
    SPELL_BACKLASH              = 69770,
    SPELL_ICY_GRIP              = 70117,
    SPELL_ICY_GRIP_JUMP         = 70122,
    SPELL_BLISTERING_COLD       = 70123,
    SPELL_FROST_BEACON          = 70126,
    SPELL_ICE_TOMB_TARGET       = 69712,
    SPELL_ICE_TOMB_DUMMY        = 69675,
    SPELL_ICE_TOMB_UNTARGETABLE = 69700,
    SPELL_ICE_TOMB_DAMAGE       = 70157,
    SPELL_ASPHYXIATION          = 71665,
    SPELL_FROST_BOMB_TRIGGER    = 69846,
    SPELL_FROST_BOMB_VISUAL     = 70022,
    SPELL_BIRTH_NO_VISUAL       = 40031,
    SPELL_FROST_BOMB            = 69845,
    SPELL_MYSTIC_BUFFET         = 70128,
    // Spinestalker
    SPELL_BELLOWING_ROAR        = 36922,
    SPELL_CLEAVE_SPINESTALKER   = 40505,
    SPELL_TAIL_SWEEP            = 71370,
    // Rimefang
    SPELL_FROST_BREATH          = 71386,
    SPELL_FROST_AURA_RIMEFANG   = 71387,
    SPELL_ICY_BLAST             = 71376,
    SPELL_ICY_BLAST_AREA        = 71380,
    // Frostwarden Handler
    SPELL_FOCUS_FIRE            = 71350,
    SPELL_ORDER_WHELP           = 71357,
    SPELL_CONCUSSIVE_SHOCK      = 71337,
	// Frost Infusion
	SPELL_FROST_INFUSION_CREDIT = 72289,
};

enum Events
{
    // Sindragosa
    EVENT_BERSERK                   = 1,
    EVENT_CLEAVE                    = 2,
    EVENT_TAIL_SMASH                = 3,
    EVENT_FROST_BREATH              = 4,
    EVENT_UNCHAINED_MAGIC           = 5,
    EVENT_ICY_GRIP                  = 6,
    EVENT_BLISTERING_COLD           = 7,
    EVENT_BLISTERING_COLD_YELL      = 8,
    EVENT_AIR_PHASE                 = 9,
    EVENT_ICE_TOMB                  = 10,
    EVENT_FROST_BOMB                = 11,
    EVENT_LAND                      = 12,
    EVENT_AIR_MOVEMENT              = 21,
    EVENT_THIRD_PHASE_CHECK         = 22,
    EVENT_AIR_MOVEMENT_FAR          = 23,
    EVENT_LAND_GROUND               = 24,
	EVENT_CHECK_PLAYERS             = 25,
    // Spinestalker
    EVENT_BELLOWING_ROAR            = 13,
    EVENT_CLEAVE_SPINESTALKER       = 14,
    EVENT_TAIL_SWEEP                = 15,
    // Rimefang
    EVENT_FROST_BREATH_RIMEFANG     = 16,
    EVENT_ICY_BLAST                 = 17,
    EVENT_ICY_BLAST_CAST            = 18,
    // Trash
    EVENT_FROSTWARDEN_ORDER_WHELP   = 19,
    EVENT_CONCUSSIVE_SHOCK          = 20,
    // event groups
    EVENT_GROUP_LAND_PHASE          = 1,
};

enum FrostwingData
{
    DATA_MYSTIC_BUFFET_STACK    = 0,
    DATA_FROSTWYRM_OWNER        = 1,
    DATA_WHELP_MARKER           = 2,
    DATA_LINKED_GAMEOBJECT      = 3,
    DATA_TRAPPED_PLAYER         = 4,
	DATA_ENABLE_ASPHYXIATION    = 5,
};

enum MovementPoints
{
    POINT_FROSTWYRM_FLY_IN  = 1,
    POINT_FROSTWYRM_LAND    = 2,
    POINT_AIR_PHASE         = 3,
    POINT_TAKEOFF           = 4,
    POINT_LAND              = 5,
    POINT_AIR_PHASE_FAR     = 6,
    POINT_LAND_GROUND       = 7,
};

enum Shadowmourne
{
    QUEST_FROST_INFUSION        = 24757,
    ITEM_SHADOW_S_EDGE          = 49888,

    SPELL_FROST_INFUSION        = 72292,
    SPELL_FROST_IMBUED_BLADE    = 72290,
};

Position const RimefangFlyPos      = {4413.309f, 2456.421f, 233.3795f, 2.890186f};
Position const RimefangLandPos     = {4413.309f, 2456.421f, 203.3848f, 2.890186f};
Position const SpinestalkerFlyPos  = {4418.895f, 2514.233f, 230.4864f, 3.396045f};
Position const SpinestalkerLandPos = {4418.895f, 2514.233f, 203.3848f, 3.396045f};
Position const SindragosaSpawnPos  = {4818.700f, 2483.710f, 287.0650f, 3.089233f};
Position const SindragosaFlyPos    = {4475.190f, 2484.570f, 234.8510f, 3.141593f};
Position const SindragosaLandPos   = {4419.190f, 2484.570f, 203.3848f, 3.141593f};
Position const SindragosaAirPos    = {4475.990f, 2484.430f, 247.9340f, 3.141593f};
Position const SindragosaAirPosFar = {4525.600f, 2485.150f, 245.0820f, 3.141593f};
Position const SindragosaFlyInPos  = {4419.190f, 2484.360f, 232.5150f, 3.141593f};

class FrostwyrmLandEvent : public BasicEvent
{
    public:
        FrostwyrmLandEvent(Creature& owner, Position const& dest) : _owner(owner), _dest(dest) { }

        bool Execute(uint64 /*eventTime*/, uint32 /*updateTime*/)
        {
            _owner.GetMotionMaster()->MoveLand(POINT_FROSTWYRM_LAND, _dest);
            return true;
        }

    private:
        Creature& _owner;
        Position const& _dest;
};

class FrostBombExplosion : public BasicEvent
{
    public:
        FrostBombExplosion(Creature* owner, uint64 sindragosaGUID) : _owner(owner), _sindragosaGUID(sindragosaGUID) { }

        bool Execute(uint64 /*eventTime*/, uint32 /*updateTime*/)
        {
            _owner->CastSpell((Unit*)0, SPELL_FROST_BOMB, false, 0, 0, _sindragosaGUID);
            _owner->RemoveAurasDueToSpell(SPELL_FROST_BOMB_VISUAL);
            return true;
        }

    private:
        Creature* _owner;
        uint64 _sindragosaGUID;
};

class boss_sindragosa : public CreatureScript
{
    public:
        boss_sindragosa() : CreatureScript("boss_sindragosa") { }

        struct boss_sindragosaAI : public BossAI
        {
            boss_sindragosaAI(Creature* creature) : BossAI(creature, DATA_SINDRAGOSA), _summoned(false) {}

			uint8 _mysticBuffetStack;
            bool _isInAirPhase;
            bool _isThirdPhase;
            bool _summoned;
			bool _airPhaseTriggered;

            void Reset()
            {
                BossAI::Reset();
                me->SetReactState(REACT_DEFENSIVE);
                DoCast(me, SPELL_TANK_MARKER, true);
				DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
                events.ScheduleEvent(EVENT_BERSERK, 600000);
                events.ScheduleEvent(EVENT_CLEAVE, 10000, EVENT_GROUP_LAND_PHASE);
                events.ScheduleEvent(EVENT_TAIL_SMASH, 20000, EVENT_GROUP_LAND_PHASE);
                events.ScheduleEvent(EVENT_FROST_BREATH, urand(8000, 12000), EVENT_GROUP_LAND_PHASE);
                events.ScheduleEvent(EVENT_UNCHAINED_MAGIC, urand(9000, 14000), EVENT_GROUP_LAND_PHASE);
                events.ScheduleEvent(EVENT_ICY_GRIP, 33500, EVENT_GROUP_LAND_PHASE);
				events.ScheduleEvent(EVENT_CHECK_PLAYERS, 5000);
                _mysticBuffetStack = 0;
                _isInAirPhase = false;
                _isThirdPhase = false;
				_airPhaseTriggered = false;

                if (!_summoned)
                {
                    me->SetCanFly(true);
                    me->SetDisableGravity(true);
                }
            }

			void EnterToBattle(Unit* who)
            {
                if (!instance->CheckRequiredBosses(DATA_SINDRAGOSA, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    instance->DoCastSpellOnPlayers(SPELL_LIGHTS_HAMMER_TELEPORT);
                    return;
                }

                BossAI::EnterToBattle(who);
                DoCast(me, RAID_MODE(SPELL_FROST_AURA_10N, SPELL_FROST_AURA_25N, SPELL_FROST_AURA_10H, SPELL_FROST_AURA_25H));
                DoCast(me, SPELL_PERMAEATING_CHILL);
                DoSendQuantumText(SAY_AGGRO, me);
            }

			bool CanAIAttack(Unit const* target) const
			{
				if (target->GetPositionZ() >= 211.0f && !me->IsWithinLOS(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ()))
					return false;

				return true;
			}

            void JustReachedHome()
            {
                BossAI::JustReachedHome();
                instance->SetBossState(DATA_SINDRAGOSA, FAIL);
                me->SetCanFly(false);
                me->SetDisableGravity(false);
            }

			void EnterEvadeMode()
			{
				BossAI::EnterEvadeMode();
			}

            void KilledUnit(Unit* who)
            {
                if (who->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_KILL_1, SAY_KILL_2), me);
            }

			void JustDied(Unit* killer)
            {
                BossAI::JustDied(killer);

                DoSendQuantumText(SAY_DEATH, me);

				if (Is25ManRaid() && me->HasAura(SPELL_SHADOWS_FATE))
					DoCastAOE(SPELL_FROST_INFUSION_CREDIT, true);

				// Quest credit
				if (Player* player = killer->ToPlayer())
				{
					player->CastSpell(player, SPELL_SOUL_FEAST, true);
					me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
				}
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_START_FROSTWYRM)
                {
                    if (_summoned)
                        return;

                    _summoned = true;
                    if (TempSummon* summon = me->ToTempSummon())
                        summon->SetTempSummonType(TEMPSUMMON_DEAD_DESPAWN);

                    if (me->IsDead())
                        return;

                    me->SetActive(true);
                    me->SetCanFly(true);
                    me->SetDisableGravity(true);
                    me->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_HOVER);
                    me->SetSpeed(MOVE_FLIGHT, 4.0f);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    float moveTime = me->GetExactDist(&SindragosaFlyPos) / (me->GetSpeed(MOVE_FLIGHT) * 0.001f);
                    me->m_Events.AddEvent(new FrostwyrmLandEvent(*me, SindragosaLandPos), me->m_Events.CalculateTime(uint64(moveTime) + 250));
                    me->GetMotionMaster()->MovePoint(POINT_FROSTWYRM_FLY_IN, SindragosaFlyPos);
                    DoCast(me, SPELL_SINDRAGOSA_S_FURY);
                }
            }

            uint32 GetData(uint32 type)
            {
                if (type == DATA_MYSTIC_BUFFET_STACK)
                    return _mysticBuffetStack;
                return 0xFFFFFFFF;
            }

            void MovementInform(uint32 type, uint32 point)
            {
                if (type != POINT_MOTION_TYPE && type != EFFECT_MOTION_TYPE)
                    return;

                switch (point)
                {
                    case POINT_FROSTWYRM_LAND:
                        me->SetActive(false);
                        me->SetCanFly(false);
                        me->SetDisableGravity(false);
                        me->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_HOVER);
                        me->SetHomePosition(SindragosaLandPos);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->SetSpeed(MOVE_FLIGHT, 2.5f);
                        // Sindragosa enters combat as soon as she lands
                        //DoZoneInCombat();
                        break;
                    case POINT_TAKEOFF:
                        events.ScheduleEvent(EVENT_AIR_MOVEMENT, 1);
                        break;
                    case POINT_AIR_PHASE:
						me->CastCustomSpell(SPELL_ICE_TOMB_TARGET, SPELLVALUE_MAX_TARGETS, RAID_MODE<int32>(2, 5, 2, 6), false);
                        //me->CastCustomSpell(SPELL_ICE_TOMB_TARGET, SPELLVALUE_MAX_TARGETS, RAID_MODE<int32>(2, 5, 2, 6), 0);
                        me->SetFacingTo(float(M_PI));
                        events.ScheduleEvent(EVENT_AIR_MOVEMENT_FAR, 1);
                        events.ScheduleEvent(EVENT_FROST_BOMB, 13000);
                        break;
                    case POINT_AIR_PHASE_FAR:
                        me->SetFacingTo(float(M_PI));
                        events.ScheduleEvent(EVENT_LAND, 30000);
                        break;
                    case POINT_LAND:
                        events.ScheduleEvent(EVENT_LAND_GROUND, 1);
                        break;
                    case POINT_LAND_GROUND:
                    {
                        me->SetCanFly(false);
                        me->SetDisableGravity(false);
                        me->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_HOVER);
                        me->SetReactState(REACT_DEFENSIVE);
                        if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == POINT_MOTION_TYPE)
                            me->GetMotionMaster()->MovementExpired();
                        _isInAirPhase = false;
                        // trigger Asphyxiation
                        EntryCheckPredicate pred(NPC_ICE_TOMB);
                        summons.DoAction(ACTION_TRIGGER_ASPHYXIATION, pred);
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
            {
                if (!_airPhaseTriggered && !HealthAbovePct(85))
				{
					_airPhaseTriggered = true;
					events.ScheduleEvent(EVENT_AIR_PHASE, 1000);
				}
				else if (!_isThirdPhase && !HealthAbovePct(35))
                {
                    events.CancelEvent(EVENT_AIR_PHASE);
                    events.ScheduleEvent(EVENT_THIRD_PHASE_CHECK, 1000);
                    _isThirdPhase = true;
                }
            }

            void JustSummoned(Creature* summon)
            {
                summons.Summon(summon);
                if (summon->GetEntry() == NPC_FROST_BOMB)
                {
                    summon->CastSpell(summon, SPELL_FROST_BOMB_VISUAL, true);
                    summon->CastSpell(summon, SPELL_BIRTH_NO_VISUAL, true);
                    summon->m_Events.AddEvent(new FrostBombExplosion(summon, me->GetGUID()), summon->m_Events.CalculateTime(5500));
                }
            }

            void SummonedCreatureDespawn(Creature* summon)
            {
                BossAI::SummonedCreatureDespawn(summon);
                if (summon->GetEntry() == NPC_ICE_TOMB)
                    summon->AI()->JustDied(summon);
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell)
            {
                if (uint32 spellId = sSpellMgr->GetSpellIdForDifficulty(70127, me))
                {
                    if (spellId == spell->Id)
                    {
                        if (Aura const* mysticBuffet = target->GetAura(spell->Id))
                            _mysticBuffetStack = std::max<uint8>(_mysticBuffetStack, mysticBuffet->GetStackAmount());

                        return;
                    }
                }

                // Frost Infusion
                if (Player* player = target->ToPlayer())
                {
                    if (uint32 spellId = sSpellMgr->GetSpellIdForDifficulty(_isThirdPhase ? SPELL_FROST_BREATH_P2 : SPELL_FROST_BREATH_P1, me))
                    {
                        if (spellId == spell->Id)
                        {
                            Item* shadowsEdge = player->GetWeaponForAttack(BASE_ATTACK, true);
                            if (player->GetQuestStatus(QUEST_FROST_INFUSION) == QUEST_STATUS_INCOMPLETE && shadowsEdge)
                            {
                                if (!player->HasAura(SPELL_FROST_IMBUED_BLADE) && shadowsEdge->GetEntry() == ITEM_SHADOW_S_EDGE)
                                {
                                    if (Aura* infusion = player->GetAura(SPELL_FROST_INFUSION))
                                    {
                                        if (infusion->GetStackAmount() == 3)
                                        {
                                            player->CastSpell(player, SPELL_FROST_IMBUED_BLADE, true);
                                            player->RemoveAura(infusion);
                                        }
                                        else
                                            player->CastSpell(player, SPELL_FROST_INFUSION, true);
                                    }
                                    else
                                        player->CastSpell(player, SPELL_FROST_INFUSION, true);
                                }
                            }

                            return;
                        }
                    }
                }
            }

			void CheckPlayerPositions()
            {
                Map* map = me->GetMap();
                if (map && map->IsDungeon())
                {
                    Map::PlayerList const &PlayerList = map->GetPlayers();

                    if (PlayerList.isEmpty())
                        return;

                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
					{
                        if (i->getSource())
						{
                            if (i->getSource()->IsAlive())
							{
                                if (i->getSource()->GetPositionZ() < 195.0f && i->getSource()->GetDistance2d(4379.1f, 2485.4f) < 100.0f)
                                    i->getSource()->TeleportTo(631, 4419.190f, 2484.570f, 205.0f, 3.141593f);
							}
						}
					}
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim() || !CheckInRoom())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BERSERK:
                            DoSendQuantumText(EMOTE_GENERIC_BERSERK_RAID, me);
                            DoSendQuantumText(SAY_BERSERK, me);
                            DoCast(me, SPELL_BERSERK);
                            break;
                        case EVENT_CLEAVE:
                            DoCastVictim(SPELL_CLEAVE);
                            events.ScheduleEvent(EVENT_CLEAVE, urand(15000, 20000), EVENT_GROUP_LAND_PHASE);
                            break;
                        case EVENT_TAIL_SMASH:
                            DoCast(me, SPELL_TAIL_SMASH);
                            events.ScheduleEvent(EVENT_TAIL_SMASH, urand(27000, 32000), EVENT_GROUP_LAND_PHASE);
                            break;
                        case EVENT_FROST_BREATH:
                            DoCastVictim(_isThirdPhase ? SPELL_FROST_BREATH_P2 : SPELL_FROST_BREATH_P1);
                            events.ScheduleEvent(EVENT_FROST_BREATH, urand(20000, 25000), EVENT_GROUP_LAND_PHASE);
                            break;
                        case EVENT_UNCHAINED_MAGIC:
                            DoSendQuantumText(SAY_UNCHAINED_MAGIC, me);
                            DoCast(me, SPELL_UNCHAINED_MAGIC);
                            events.ScheduleEvent(EVENT_UNCHAINED_MAGIC, urand(30000, 35000), EVENT_GROUP_LAND_PHASE);
                            break;
                        case EVENT_ICY_GRIP:
                            DoCast(me, SPELL_ICY_GRIP);
                            events.ScheduleEvent(EVENT_BLISTERING_COLD, 1000, EVENT_GROUP_LAND_PHASE);
                            break;
                        case EVENT_BLISTERING_COLD:
                            DoSendQuantumText(EMOTE_WARN_BLISTERING_COLD, me);
                            DoCast(me, SPELL_BLISTERING_COLD);
                            events.ScheduleEvent(EVENT_BLISTERING_COLD_YELL, 5000, EVENT_GROUP_LAND_PHASE);
                            break;
                        case EVENT_BLISTERING_COLD_YELL:
                            DoSendQuantumText(SAY_BLISTERING_COLD, me);
                            break;
                        case EVENT_AIR_PHASE:
                        {
                            _isInAirPhase = true;
                            DoSendQuantumText(SAY_AIR_PHASE, me);
                            me->SetCanFly(true);
                            me->SetDisableGravity(true);
                            me->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_HOVER);
                            me->SetReactState(REACT_PASSIVE);
                            me->AttackStop();
                            Position pos;
                            pos.Relocate(me);
                            pos.m_positionZ += 17.0f;
                            me->GetMotionMaster()->MoveTakeoff(POINT_TAKEOFF, pos);
                            events.CancelEventGroup(EVENT_GROUP_LAND_PHASE);
                            events.ScheduleEvent(EVENT_AIR_PHASE, 120000);
                            break;
                        }
                        case EVENT_AIR_MOVEMENT:
                            me->GetMotionMaster()->MovePoint(POINT_AIR_PHASE, SindragosaAirPos);
                            break;
                        case EVENT_AIR_MOVEMENT_FAR:
                            me->GetMotionMaster()->MovePoint(POINT_AIR_PHASE_FAR, SindragosaAirPosFar);
                            break;
                        case EVENT_ICE_TOMB:
							DoSendQuantumText(EMOTE_WARN_FROZEN_ORB, me);
							me->CastCustomSpell(SPELL_ICE_TOMB_TARGET, SPELLVALUE_MAX_TARGETS, RAID_MODE<int32>(1, 1, 1, 1), NULL);
							me->SetFacingTo(float(2*M_PI));
                            events.ScheduleEvent(EVENT_ICE_TOMB, urand(16000, 23000));
                            break;
                        case EVENT_FROST_BOMB:
                        {
                            float destX, destY, destZ;
                            destX = float(rand_norm()) * 75.0f + 4350.0f;
                            destY = float(rand_norm()) * 75.0f + 2450.0f;
                            destZ = 205.0f; // random number close to ground, get exact in next call
                            me->UpdateGroundPositionZ(destX, destY, destZ);
                            me->CastSpell(destX, destY, destZ, SPELL_FROST_BOMB_TRIGGER, false);
                            events.ScheduleEvent(EVENT_FROST_BOMB, urand(6000, 8000));
                            break;
                        }
                        case EVENT_LAND:
                        {
                            events.CancelEvent(EVENT_FROST_BOMB);
                            me->GetMotionMaster()->MovePoint(POINT_LAND, SindragosaFlyInPos);
                            break;
                        }
                        case EVENT_LAND_GROUND:
                            events.ScheduleEvent(EVENT_CLEAVE, urand(13000, 15000), EVENT_GROUP_LAND_PHASE);
                            events.ScheduleEvent(EVENT_TAIL_SMASH, urand(19000, 23000), EVENT_GROUP_LAND_PHASE);
                            events.ScheduleEvent(EVENT_FROST_BREATH, urand(10000, 15000), EVENT_GROUP_LAND_PHASE);
                            events.ScheduleEvent(EVENT_UNCHAINED_MAGIC, urand(12000, 17000), EVENT_GROUP_LAND_PHASE);
                            events.ScheduleEvent(EVENT_ICY_GRIP, urand(35000, 40000), EVENT_GROUP_LAND_PHASE);
                            me->GetMotionMaster()->MoveLand(POINT_LAND_GROUND, SindragosaLandPos);
                            break;
                        case EVENT_THIRD_PHASE_CHECK:
                        {
                            if (!_isInAirPhase)
                            {
                                DoSendQuantumText(SAY_PHASE_2, me);
                                events.ScheduleEvent(EVENT_ICE_TOMB, urand(7000, 10000));
                                events.RescheduleEvent(EVENT_ICY_GRIP, urand(35000, 40000));
                                DoCast(me, SPELL_MYSTIC_BUFFET, true);
                            }
                            else
                                events.ScheduleEvent(EVENT_THIRD_PHASE_CHECK, 5000);
                            break;
                        }
						case EVENT_CHECK_PLAYERS:
							CheckPlayerPositions();
							events.ScheduleEvent(EVENT_CHECK_PLAYERS, 3000);
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
            return GetIcecrownCitadelAI<boss_sindragosaAI>(creature);
        }
};

class npc_ice_tomb : public CreatureScript
{
    public:
        npc_ice_tomb() : CreatureScript("npc_ice_tomb") { }

        struct npc_ice_tombAI : public QuantumBasicAI
        {
            npc_ice_tombAI(Creature* creature) : QuantumBasicAI(creature)
            {
                _trappedPlayerGUID = 0;

				SetCombatMovement(false);
            }

			bool AsphyxiationTimerEnabled;

			uint64 _trappedPlayerGUID;

            uint32 ExistenceCheckTimer;
            uint32 AsphyxiationTimer;

            void Reset()
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK_DEST, true);
                me->SetReactState(REACT_PASSIVE);
                AsphyxiationTimer = 20000;
                AsphyxiationTimerEnabled = false;
            }

            void SetGUID(uint64 guid, int32 type/* = 0 */)
            {
                if (type == DATA_TRAPPED_PLAYER)
                {
                    _trappedPlayerGUID = guid;
                    ExistenceCheckTimer = 1*IN_MILLISECONDS;
                }
            }

            void SetData(uint32 type, uint32 /*data*/)
            {
                if (type == DATA_ENABLE_ASPHYXIATION)
                    AsphyxiationTimerEnabled = true;
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_TRIGGER_ASPHYXIATION)
				{
                    if (Player* player = ObjectAccessor::GetPlayer(*me, _trappedPlayerGUID))
                        player->CastSpell(player, SPELL_ASPHYXIATION, true);
				}
            }

            void JustDied(Unit* /*killer*/)
            {
                me->RemoveAllGameObjects();

                if (Player* player = ObjectAccessor::GetPlayer(*me, _trappedPlayerGUID))
                {
                    _trappedPlayerGUID = 0;
                    player->RemoveAurasDueToSpell(SPELL_ICE_TOMB_UNTARGETABLE);
                    player->RemoveAurasDueToSpell(SPELL_ICE_TOMB_DAMAGE);
                    player->RemoveAurasDueToSpell(SPELL_ASPHYXIATION);

                    // set back in combat with sindragosa
                    if (InstanceScript* instance = player->GetInstanceScript())
					{
                        if (Creature* sindragosa = ObjectAccessor::GetCreature(*player, instance->GetData64(DATA_SINDRAGOSA)))
                        {
                            sindragosa->SetInCombatWith(player);
                            player->SetInCombatWith(sindragosa);
                        }
					}
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!_trappedPlayerGUID)
                    return;

                if (ExistenceCheckTimer <= diff)
                {
                    Player* player = ObjectAccessor::GetPlayer(*me, _trappedPlayerGUID);
                    if (!player || player->IsDead() || !player->HasAura(SPELL_ICE_TOMB_DAMAGE))
                    {
                        // Remove object
                        JustDied(me);
                        me->DespawnAfterAction();
                        return;
                    }
                    ExistenceCheckTimer = 1*IN_MILLISECONDS;
                }
                else ExistenceCheckTimer -= diff;

                if (AsphyxiationTimerEnabled)
                {
                    if (AsphyxiationTimer <= diff)
                    {
                        AsphyxiationTimerEnabled = false;

                        if (Player* player = ObjectAccessor::GetPlayer(*me, _trappedPlayerGUID))
                            player->CastSpell(player, SPELL_ASPHYXIATION, true);
                    }
                    else AsphyxiationTimer -= diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_ice_tombAI>(creature);
        }
};

class npc_spinestalker : public CreatureScript
{
    public:
        npc_spinestalker() : CreatureScript("npc_spinestalker") { }

        struct npc_spinestalkerAI : public QuantumBasicAI
        {
            npc_spinestalkerAI(Creature* creature) : QuantumBasicAI(creature), instance(creature->GetInstanceScript()), _summoned(false){}

            void InitializeAI()
            {
                // Increase add count
                if (!me->IsDead())
                {
                    instance->SetData(DATA_SINDRAGOSA_FROSTWYRMS, me->GetDBTableGUIDLow());  // this cannot be in Reset because reset also happens on evade
                    Reset();
                }
            }

            void Reset()
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_BELLOWING_ROAR, urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_CLEAVE_SPINESTALKER, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                _events.ScheduleEvent(EVENT_TAIL_SWEEP, urand(8*IN_MILLISECONDS, 12*IN_MILLISECONDS));

                me->SetReactState(REACT_DEFENSIVE);

                if (!_summoned)
                {
                    me->SetCanFly(true);
                    me->SetDisableGravity(true);
                }
            }

            void JustRespawned()
            {
                QuantumBasicAI::JustRespawned();
                instance->SetData(DATA_SINDRAGOSA_FROSTWYRMS, me->GetDBTableGUIDLow());  // this cannot be in Reset because reset also happens on evade
            }

            void JustDied(Unit* killer)
            {
                _events.Reset();

				// Quest credit
				if (Player* player = killer->ToPlayer())
				{
					player->CastSpell(player, SPELL_SOUL_FEAST, true);
					me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
				}
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_START_FROSTWYRM)
                {
                    if (_summoned)
                        return;

                    _summoned = true;
                    if (me->IsDead())
                        return;

                    me->SetActive(true);
                    me->SetSpeed(MOVE_FLIGHT, 2.0f);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    float moveTime = me->GetExactDist(&SpinestalkerFlyPos) / (me->GetSpeed(MOVE_FLIGHT) * 0.001f);
                    me->m_Events.AddEvent(new FrostwyrmLandEvent(*me, SpinestalkerLandPos), me->m_Events.CalculateTime(uint64(moveTime) + 250));
                    me->SetDefaultMovementType(IDLE_MOTION_TYPE);
                    me->GetMotionMaster()->MoveIdle();
                    me->StopMoving();
                    me->GetMotionMaster()->MovePoint(POINT_FROSTWYRM_FLY_IN, SpinestalkerFlyPos);
                }
            }

            void MovementInform(uint32 type, uint32 point)
            {
                if (type != EFFECT_MOTION_TYPE || point != POINT_FROSTWYRM_LAND)
                    return;

                me->SetActive(false);
                me->SetCanFly(false);
                me->SetDisableGravity(false);
                me->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_HOVER);
                me->SetHomePosition(SpinestalkerLandPos);
                me->SetFacingTo(SpinestalkerLandPos.GetOrientation());
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BELLOWING_ROAR:
                            DoCast(me, SPELL_BELLOWING_ROAR);
                            _events.ScheduleEvent(EVENT_BELLOWING_ROAR, urand(25*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                            break;
                        case EVENT_CLEAVE_SPINESTALKER:
                            DoCastVictim(SPELL_CLEAVE_SPINESTALKER);
                            _events.ScheduleEvent(EVENT_CLEAVE_SPINESTALKER, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
                            break;
                        case EVENT_TAIL_SWEEP:
                            DoCast(me, SPELL_TAIL_SWEEP);
                            _events.ScheduleEvent(EVENT_TAIL_SWEEP, urand(22*IN_MILLISECONDS, 25*IN_MILLISECONDS));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
            InstanceScript* instance;
            bool _summoned;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_spinestalkerAI>(creature);
        }
};

class npc_rimefang : public CreatureScript
{
    public:
        npc_rimefang() : CreatureScript("npc_rimefang_icc") { }

        struct npc_rimefangAI : public QuantumBasicAI
        {
            npc_rimefangAI(Creature* creature) : QuantumBasicAI(creature), instance(creature->GetInstanceScript()), _summoned(false){}

            void InitializeAI()
            {
                // Increase add count
                if (!me->IsDead())
                {
                    instance->SetData(DATA_SINDRAGOSA_FROSTWYRMS, me->GetDBTableGUIDLow());  // this cannot be in Reset because reset also happens on evade
                    Reset();
                }
            }

            void Reset()
            {
                _events.Reset();
                _events.ScheduleEvent(EVENT_FROST_BREATH_RIMEFANG, urand(12000, 15000));
                _events.ScheduleEvent(EVENT_ICY_BLAST, urand(30000, 35000));
                me->SetReactState(REACT_DEFENSIVE);
                IcyBlastCounter = 0;

                if (!_summoned)
                {
                    me->SetCanFly(true);
                    me->SetDisableGravity(true);
                }
            }

            void JustRespawned()
            {
                QuantumBasicAI::JustRespawned();
                instance->SetData(DATA_SINDRAGOSA_FROSTWYRMS, me->GetDBTableGUIDLow());  // this cannot be in Reset because reset also happens on evade
            }

            void JustDied(Unit* killer)
            {
                _events.Reset();

				// Quest credit
				if (Player* player = killer->ToPlayer())
				{
					player->CastSpell(player, SPELL_SOUL_FEAST, true);
					me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
				}
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_START_FROSTWYRM)
                {
                    if (_summoned)
                        return;

                    _summoned = true;
                    if (me->IsDead())
                        return;

                    me->SetActive(true);
                    me->SetSpeed(MOVE_FLIGHT, 2.0f);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                    float moveTime = me->GetExactDist(&RimefangFlyPos) / (me->GetSpeed(MOVE_FLIGHT) * 0.001f);
                    me->m_Events.AddEvent(new FrostwyrmLandEvent(*me, RimefangLandPos), me->m_Events.CalculateTime(uint64(moveTime) + 250));
                    me->SetDefaultMovementType(IDLE_MOTION_TYPE);
                    me->GetMotionMaster()->MoveIdle();
                    me->StopMoving();
                    me->GetMotionMaster()->MovePoint(POINT_FROSTWYRM_FLY_IN, RimefangFlyPos);
                }
            }

            void MovementInform(uint32 type, uint32 point)
            {
                if (type != EFFECT_MOTION_TYPE || point != POINT_FROSTWYRM_LAND)
                    return;

                me->SetActive(false);
                me->SetCanFly(false);
                me->SetDisableGravity(false);
                me->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_HOVER);
                me->SetHomePosition(RimefangLandPos);
                me->SetFacingTo(RimefangLandPos.GetOrientation());
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            }

            void EnterToBattle(Unit* /*victim*/)
            {
                DoCast(me, SPELL_FROST_AURA_RIMEFANG, true);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FROST_BREATH_RIMEFANG:
                            DoCast(me, SPELL_FROST_BREATH);
                            _events.ScheduleEvent(EVENT_FROST_BREATH_RIMEFANG, urand(35000, 40000));
                            break;
                        case EVENT_ICY_BLAST:
                        {
                            IcyBlastCounter = RAID_MODE<uint8>(5, 7, 6, 8);
                            me->SetReactState(REACT_PASSIVE);
                            me->AttackStop();
                            me->SetCanFly(true);
                            me->GetMotionMaster()->MovePoint(POINT_FROSTWYRM_FLY_IN, RimefangFlyPos);
                            float moveTime = me->GetExactDist(&RimefangFlyPos)/(me->GetSpeed(MOVE_FLIGHT)*0.001f);
                            _events.ScheduleEvent(EVENT_ICY_BLAST, uint64(moveTime) + urand(60000, 70000));
                            _events.ScheduleEvent(EVENT_ICY_BLAST_CAST, uint64(moveTime) + 250);
                            break;
                        }
                        case EVENT_ICY_BLAST_CAST:
                            if (--IcyBlastCounter)
                            {
                                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 0.0f, true))
                                {
                                    me->SetFacingToObject(target);
                                    DoCast(target, SPELL_ICY_BLAST);
                                }
                                _events.ScheduleEvent(EVENT_ICY_BLAST_CAST, 3000);
                            }
                            else if (Unit* victim = me->SelectVictim())
                            {
                                me->SetReactState(REACT_DEFENSIVE);
                                AttackStart(victim);
                                me->SetCanFly(false);
                            }
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
            InstanceScript* instance;
            uint8 IcyBlastCounter;
            bool _summoned;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_rimefangAI>(creature);
        }
};

class npc_sindragosa_trash : public CreatureScript
{
    public:
        npc_sindragosa_trash() : CreatureScript("npc_sindragosa_trash") { }

        struct npc_sindragosa_trashAI : public QuantumBasicAI
        {
            npc_sindragosa_trashAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

            void InitializeAI()
            {
                FrostwyrmId = (me->GetHomePosition().GetPositionY() < 2484.35f) ? DATA_RIMEFANG : DATA_SPINESTALKER;
                // Increase add count
                if (!me->IsDead())
                {
                    if (me->GetEntry() == NPC_FROSTWING_WHELP)
                        instance->SetData(FrostwyrmId, me->GetDBTableGUIDLow());  // this cannot be in Reset because reset also happens on evade
                    Reset();
                }
            }

            void Reset()
            {
                // This is shared AI for handler and whelps
                if (me->GetEntry() == NPC_FROSTWARDEN_HANDLER)
                {
                    _events.ScheduleEvent(EVENT_FROSTWARDEN_ORDER_WHELP, 3000);
                    _events.ScheduleEvent(EVENT_CONCUSSIVE_SHOCK, urand(8000, 10000));
                }

                _isTaunted = false;
            }

            void JustRespawned()
            {
                QuantumBasicAI::JustRespawned();

                // Increase add count
                if (me->GetEntry() == NPC_FROSTWING_WHELP)
                    instance->SetData(FrostwyrmId, me->GetDBTableGUIDLow());  // this cannot be in Reset because reset also happens on evade
            }

            void SetData(uint32 type, uint32 data)
            {
                if (type == DATA_WHELP_MARKER)
                    _isTaunted = data != 0;
            }

            uint32 GetData(uint32 type)
            {
                if (type == DATA_FROSTWYRM_OWNER)
                    return FrostwyrmId;
                else if (type == DATA_WHELP_MARKER)
                    return uint32(_isTaunted);
                return 0;
            }

			void JustDied(Unit* killer)
			{
				if (Player* player = killer->ToPlayer())
					player->CastSpell(player, SPELL_SOUL_FEAST, true);

				me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
			}

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FROSTWARDEN_ORDER_WHELP:
                            DoCast(me, SPELL_ORDER_WHELP);
                            _events.ScheduleEvent(EVENT_FROSTWARDEN_ORDER_WHELP, 3000);
                            break;
                        case EVENT_CONCUSSIVE_SHOCK:
                            DoCast(me, SPELL_CONCUSSIVE_SHOCK);
                            _events.ScheduleEvent(EVENT_CONCUSSIVE_SHOCK, urand(10000, 13000));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
            EventMap _events;
            InstanceScript* instance;
            uint32 FrostwyrmId;
            bool _isTaunted; // Frostwing Whelp only
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_sindragosa_trashAI>(creature);
        }
};

class spell_sindragosa_s_fury : public SpellScriptLoader
{
    public:
        spell_sindragosa_s_fury() : SpellScriptLoader("spell_sindragosa_s_fury") { }

        class spell_sindragosa_s_fury_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sindragosa_s_fury_SpellScript);

            bool Load()
            {
                _targetCount = 0;

                // This script should execute only in Icecrown Citadel
                if (InstanceMap* instance = GetCaster()->GetMap()->ToInstanceMap())
                    if (instance->GetInstanceScript())
                        if (instance->GetScriptId() == sObjectMgr->GetScriptId(ICCScriptName))
                            return true;

                return false;
            }

            void SelectDest()
            {
                if (Position* dest = const_cast<WorldLocation*>(GetExplTargetDest()))
                {
                    float destX = float(rand_norm()) * 75.0f + 4350.0f;
                    float destY = float(rand_norm()) * 75.0f + 2450.0f;
                    float destZ = 205.0f; // random number close to ground, get exact in next call
                    GetCaster()->UpdateGroundPositionZ(destX, destY, destZ);
                    dest->Relocate(destX, destY, destZ);
                }
            }

            void CountTargets(std::list<Unit*>& unitList)
            {
				_targetCount = unitList.size();
            }

			void HandleDummy(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                if (!GetHitUnit()->IsAlive() || !_targetCount)
                    return;

                float resistance = float(GetHitUnit()->GetResistance(SpellSchoolMask(GetSpellInfo()->SchoolMask)));
                uint32 minResistFactor = uint32((resistance / (resistance + 510.0f)) * 10.0f) * 2;
                uint32 randomResist = urand(0, (9 - minResistFactor) * 100) / 100 + minResistFactor;

                uint32 damage = (uint32(GetEffectValue() / _targetCount) * randomResist) / 10;

                SpellNonMeleeDamage damageInfo(GetCaster(), GetHitUnit(), GetSpellInfo()->Id, GetSpellInfo()->SchoolMask);
                damageInfo.damage = damage;
                GetCaster()->SendSpellNonMeleeDamageLog(&damageInfo);
                GetCaster()->DealSpellDamage(&damageInfo, false);
            }

            void Register()
            {
				BeforeCast += SpellCastFn(spell_sindragosa_s_fury_SpellScript::SelectDest);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_sindragosa_s_fury_SpellScript::CountTargets, EFFECT_1, TARGET_UNIT_DEST_AREA_ENTRY);
				OnEffectHitTarget += SpellEffectFn(spell_sindragosa_s_fury_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
            }

            uint32 _targetCount;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sindragosa_s_fury_SpellScript();
        }
};

class UnchainedMagicTargetSelector
{
    enum CriticalSpells
    {
        // Paladin spells that identify a specc
        SPELL_PALADIN_DIVINE_STORM = 53385,
        SPELL_PALADIN_HAMMER_OF_THE_RIGHTEOUS = 53595,
        // Shaman spell that identifies an enhancer
        SPELL_SHAMAN_SHAMANISTIC_RAGE = 30823,
        // Druid spell that identifies a feral (cat or bear)
        SPELL_DRUID_BERSERK = 50334
    };

    public:
        UnchainedMagicTargetSelector() { }

        bool operator()(WorldObject* object) const
        {
            if (Unit* unit = object->ToUnit())
            {
                if (Player* player = unit->ToPlayer())
                {
                    // Remove classes that don't have mana cannot be caster or healer (atm.)
                    if (player->GetPowerType() != POWER_MANA)
                        return true;
                    else // If a class has mana, it depends on their specc if they should potentially be affected or not.
                    {
                        switch (player->GetCurrentClass())
                        {
                            case CLASS_HUNTER:
                                return true; // Remove hunters, have mana, but no caster.
                            case CLASS_PALADIN: // Only holy paladins should be affected.
                            {
                                // Divine Storm: Basic retribution paladin spell
                                // Hammer of the Righteous: Basic protection paladin spell
                                // If it's neither a ret nor a prot, it must be a holy
                                if (!player->HasActiveSpell(SPELL_PALADIN_DIVINE_STORM) && !player->HasActiveSpell(SPELL_PALADIN_HAMMER_OF_THE_RIGHTEOUS))
                                    return false;
                                else
                                    return true;
                            }
                            case CLASS_SHAMAN: // Enhancer should not be affected.
                            {
                                // Shamanistic Rage: Basic enhancer shaman spell
                                // If it's not an enhancer, it has to be an elemental or restoration shaman - caster/healer ok
                                if (!player->HasActiveSpell(SPELL_SHAMAN_SHAMANISTIC_RAGE))
                                    return false;
                                else
                                    return true;
                            }
                            case CLASS_DRUID: // Feral druids should be excluded, disregarding their current state.
                            {
                                // Berserk: Basic feral druid spell
                                // If it's not a feral, it's a moonkin or tree - caster/healer ok
                                if (!player->HasActiveSpell(SPELL_DRUID_BERSERK))
                                    return false;
                                else
                                    return true;
                            }
                            default:
                                return false;
                        }
                    }
                }
            }
            return true;
        }
};

class spell_sindragosa_unchained_magic : public SpellScriptLoader
{
    public:
        spell_sindragosa_unchained_magic() : SpellScriptLoader("spell_sindragosa_unchained_magic") { }

        class spell_sindragosa_unchained_magic_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sindragosa_unchained_magic_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if(UnchainedMagicTargetSelector());
                uint32 maxSize = uint32(GetCaster()->GetMap()->GetSpawnMode() & 1 ? 6 : 2);
                if (unitList.size() > maxSize)
                    Quantum::DataPackets::RandomResizeList(unitList, maxSize);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_sindragosa_unchained_magic_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sindragosa_unchained_magic_SpellScript();
        }
};

class spell_sindragosa_instability : public SpellScriptLoader
{
    public:
        spell_sindragosa_instability() : SpellScriptLoader("spell_sindragosa_instability") { }

        class spell_sindragosa_instability_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sindragosa_instability_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_BACKLASH))
                    return false;
                return true;
            }

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
                    GetTarget()->CastCustomSpell(SPELL_BACKLASH, SPELLVALUE_BASE_POINT0, aurEff->GetAmount(), GetTarget(), true, 0, aurEff, GetCasterGUID());
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_sindragosa_instability_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sindragosa_instability_AuraScript();
        }
};

class spell_sindragosa_frost_beacon : public SpellScriptLoader
{
    public:
        spell_sindragosa_frost_beacon() : SpellScriptLoader("spell_sindragosa_frost_beacon") { }

        class spell_sindragosa_frost_beacon_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sindragosa_frost_beacon_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ICE_TOMB_DAMAGE))
                    return false;
                return true;
            }

            void PeriodicTick(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();

                if (Unit* caster = GetCaster())
                    caster->CastSpell(GetTarget(), SPELL_ICE_TOMB_DAMAGE, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sindragosa_frost_beacon_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_sindragosa_frost_beacon_AuraScript();
        }
};

class spell_sindragosa_ice_tomb : public SpellScriptLoader
{
    public:
        spell_sindragosa_ice_tomb() : SpellScriptLoader("spell_sindragosa_ice_tomb_trap") { }

        class spell_sindragosa_ice_tomb_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sindragosa_ice_tomb_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sObjectMgr->GetCreatureTemplate(NPC_ICE_TOMB))
                    return false;
                if (!sObjectMgr->GetGameObjectTemplate(GO_ICE_BLOCK))
                    return false;
                return true;
            }

            void SummonTomb()
            {
                Position pos;
                GetHitUnit()->GetPosition(&pos);
                if (TempSummon* summon = GetCaster()->SummonCreature(NPC_ICE_TOMB, pos))
                {
                    summon->AI()->SetGUID(GetHitUnit()->GetGUID(), DATA_TRAPPED_PLAYER);
                    if (GameObject* go = summon->SummonGameObject(GO_ICE_BLOCK, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ(), pos.GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, 0))
                    {
                        go->SetSpellId(SPELL_ICE_TOMB_DAMAGE);
						go->CastSpell(GetHitUnit(), SPELL_ICE_TOMB_DAMAGE);
                        summon->AddGameObject(go);
                    }
                }
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_sindragosa_ice_tomb_SpellScript::SummonTomb);
            }
        };

        class spell_sindragosa_ice_tomb_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_sindragosa_ice_tomb_AuraScript);

            void PeriodicTick(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_sindragosa_ice_tomb_AuraScript::PeriodicTick, EFFECT_2, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sindragosa_ice_tomb_SpellScript();
        }

        AuraScript* GetAuraScript() const
        {
            return new spell_sindragosa_ice_tomb_AuraScript();
        }
};

class FrostBombTargetSelector
{
    public:
        FrostBombTargetSelector(Unit* caster, std::list<Creature*> const& collisionList) : _caster(caster), _collisionList(collisionList) { }

        bool operator()(Unit* unit)
        {
            if (unit->HasAura(SPELL_ICE_TOMB_DAMAGE) || unit->GetTypeId() != TYPE_ID_PLAYER)
                return true;

            for (std::list<Creature*>::const_iterator itr = _collisionList.begin(); itr != _collisionList.end(); ++itr)
                if ((*itr)->IsInBetween(_caster, unit, 1.0f))
                    return true;

            return false;
        }

    private:
        Unit* _caster;
        std::list<Creature*> const& _collisionList;
};

class spell_sindragosa_collision_filter : public SpellScriptLoader
{
    public:
        spell_sindragosa_collision_filter() : SpellScriptLoader("spell_sindragosa_collision_filter") { }

        class spell_sindragosa_collision_filter_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sindragosa_collision_filter_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ICE_TOMB_DAMAGE))
                    return false;
                return true;
            }

            void FilterTargets(std::list<Unit*>& unitList)
            {
                std::list<Creature*> tombs;
                GetCreatureListWithEntryInGrid(tombs, GetCaster(), NPC_ICE_TOMB, 200.0f);
                unitList.remove_if (FrostBombTargetSelector(GetCaster(), tombs));
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_sindragosa_collision_filter_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sindragosa_collision_filter_SpellScript();
        }
};

class spell_sindragosa_icy_grip : public SpellScriptLoader
{
    public:
        spell_sindragosa_icy_grip() : SpellScriptLoader("spell_sindragosa_icy_grip") { }

        class spell_sindragosa_icy_grip_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sindragosa_icy_grip_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ICY_GRIP_JUMP))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetHitUnit()->CastSpell(GetCaster(), SPELL_ICY_GRIP_JUMP, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_sindragosa_icy_grip_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sindragosa_icy_grip_SpellScript();
        }
};

class MysticBuffetTargetFilter
{
    public:
        explicit MysticBuffetTargetFilter(Unit* caster) : _caster(caster) { }

        bool operator()(Unit* unit)
        {
            return !unit->IsWithinLOSInMap(_caster);
        }

    private:
        Unit* _caster;
};

class spell_sindragosa_mystic_buffet : public SpellScriptLoader
{
    public:
        spell_sindragosa_mystic_buffet() : SpellScriptLoader("spell_sindragosa_mystic_buffet") { }

        class spell_sindragosa_mystic_buffet_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_sindragosa_mystic_buffet_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if(MysticBuffetTargetFilter(GetCaster()));
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_sindragosa_mystic_buffet_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_sindragosa_mystic_buffet_SpellScript();
        }
};

class spell_rimefang_icy_blast : public SpellScriptLoader
{
    public:
        spell_rimefang_icy_blast() : SpellScriptLoader("spell_rimefang_icy_blast") { }

        class spell_rimefang_icy_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_rimefang_icy_blast_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ICY_BLAST_AREA))
                    return false;
                return true;
            }

            void HandleTriggerMissile(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                if (Position const* pos = GetExplTargetDest())
				{
                    if (TempSummon* summon = GetCaster()->SummonCreature(NPC_ICY_BLAST, *pos, TEMPSUMMON_TIMED_DESPAWN, 40*IN_MILLISECONDS))
                        summon->CastSpell(summon, SPELL_ICY_BLAST_AREA, true);
				}
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_rimefang_icy_blast_SpellScript::HandleTriggerMissile, EFFECT_1, SPELL_EFFECT_TRIGGER_MISSILE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_rimefang_icy_blast_SpellScript();
        }
};

class OrderWhelpTargetSelector
{
    public:
        explicit OrderWhelpTargetSelector(Creature* owner) : _owner(owner) { }

        bool operator()(Creature* creature)
        {
            if (!creature->AI()->GetData(DATA_WHELP_MARKER) && creature->AI()->GetData(DATA_FROSTWYRM_OWNER) == _owner->AI()->GetData(DATA_FROSTWYRM_OWNER))
                return false;
            return true;
        }

    private:
        Creature* _owner;
};

class spell_frostwarden_handler_order_whelp : public SpellScriptLoader
{
    public:
        spell_frostwarden_handler_order_whelp() : SpellScriptLoader("spell_frostwarden_handler_order_whelp") { }

        class spell_frostwarden_handler_order_whelp_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_frostwarden_handler_order_whelp_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_FOCUS_FIRE))
                    return false;
                return true;
            }

            void FilterTargets(std::list<Unit*>& unitList)
            {
                for (std::list<Unit*>::iterator itr = unitList.begin(); itr != unitList.end();)
                {
                    if ((*itr)->GetTypeId() != TYPE_ID_PLAYER)
                        unitList.erase(itr++);
                    else
                        ++itr;
                }

                if (unitList.empty())
                    return;

                Unit* target = Quantum::DataPackets::SelectRandomContainerElement(unitList);
                unitList.clear();
                unitList.push_back(target);
            }

            void HandleForcedCast(SpellEffIndex effIndex)
            {
                // caster is Frostwarden Handler, target is player, caster of triggered is whelp
                PreventHitDefaultEffect(effIndex);
                std::list<Creature*> unitList;
                GetCreatureListWithEntryInGrid(unitList, GetCaster(), NPC_FROSTWING_WHELP, 150.0f);
                if (Creature* creature = GetCaster()->ToCreature())
                    unitList.remove_if(OrderWhelpTargetSelector(creature));

                if (unitList.empty())
                    return;

                Quantum::DataPackets::SelectRandomContainerElement(unitList)->CastSpell(GetHitUnit(), uint32(GetEffectValue()), true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_frostwarden_handler_order_whelp_SpellScript::HandleForcedCast, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_frostwarden_handler_order_whelp_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_frostwarden_handler_order_whelp_SpellScript();
        }
};

class spell_frostwarden_handler_focus_fire : public SpellScriptLoader
{
    public:
        spell_frostwarden_handler_focus_fire() : SpellScriptLoader("spell_frostwarden_handler_focus_fire") { }

        class spell_frostwarden_handler_focus_fire_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_frostwarden_handler_focus_fire_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetCaster()->AddThreat(GetHitUnit(), float(GetEffectValue()));
                GetCaster()->GetAI()->SetData(DATA_WHELP_MARKER, 1);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_frostwarden_handler_focus_fire_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        class spell_frostwarden_handler_focus_fire_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_frostwarden_handler_focus_fire_AuraScript);

            void PeriodicTick(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();
                if (Unit* caster = GetCaster())
                {
                    caster->AddThreat(GetTarget(), -float(GetSpellInfo()->Effects[EFFECT_1].CalcValue()));
                    caster->GetAI()->SetData(DATA_WHELP_MARKER, 0);
                }
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_frostwarden_handler_focus_fire_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_frostwarden_handler_focus_fire_SpellScript();
        }

        AuraScript* GetAuraScript() const
        {
            return new spell_frostwarden_handler_focus_fire_AuraScript();
        }
};

class at_sindragosa_lair : public AreaTriggerScript
{
    public:
        at_sindragosa_lair() : AreaTriggerScript("at_sindragosa_lair") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/)
        {
            if (InstanceScript* instance = player->GetInstanceScript())
            {
                if (!instance->GetData(DATA_SPINESTALKER))
                    if (Creature* spinestalker = ObjectAccessor::GetCreature(*player, instance->GetData64(DATA_SPINESTALKER)))
                        spinestalker->AI()->DoAction(ACTION_START_FROSTWYRM);

                if (!instance->GetData(DATA_RIMEFANG))
                    if (Creature* rimefang = ObjectAccessor::GetCreature(*player, instance->GetData64(DATA_RIMEFANG)))
                        rimefang->AI()->DoAction(ACTION_START_FROSTWYRM);

                if (!instance->GetData(DATA_SINDRAGOSA_FROSTWYRMS) && !instance->GetData64(DATA_SINDRAGOSA) && instance->GetBossState(DATA_SINDRAGOSA) != DONE)
                {
                    if (player->GetMap()->IsHeroic() && !instance->GetData(DATA_HEROIC_ATTEMPTS))
                        return true;

                    player->GetMap()->LoadGrid(SindragosaSpawnPos.GetPositionX(), SindragosaSpawnPos.GetPositionY());
                    if (Creature* sindragosa = player->GetMap()->SummonCreature(NPC_SINDRAGOSA, SindragosaSpawnPos))
                        sindragosa->AI()->DoAction(ACTION_START_FROSTWYRM);
                }
            }

            return true;
        }
};

class achievement_all_you_can_eat : public AchievementCriteriaScript
{
    public:
        achievement_all_you_can_eat() : AchievementCriteriaScript("achievement_all_you_can_eat") { }

        bool OnCheck(Player* /*source*/, Unit* target)
        {
            if (!target)
                return false;
            return target->GetAI()->GetData(DATA_MYSTIC_BUFFET_STACK) <= 5;
        }
};

void AddSC_boss_sindragosa()
{
    new boss_sindragosa();
    new npc_ice_tomb();
    new npc_spinestalker();
    new npc_rimefang();
    new npc_sindragosa_trash();
    new spell_sindragosa_s_fury();
    new spell_sindragosa_unchained_magic();
    new spell_sindragosa_instability();
    new spell_sindragosa_frost_beacon();
    new spell_sindragosa_ice_tomb();
	new spell_sindragosa_collision_filter();
    new spell_sindragosa_icy_grip();
    new spell_sindragosa_mystic_buffet();
    new spell_rimefang_icy_blast();
    new spell_frostwarden_handler_order_whelp();
    new spell_frostwarden_handler_focus_fire();
    new spell_trigger_spell_from_caster("spell_sindragosa_ice_tomb", SPELL_ICE_TOMB_DUMMY);
    new spell_trigger_spell_from_caster("spell_sindragosa_ice_tomb_dummy", SPELL_FROST_BEACON);
    new at_sindragosa_lair();
    new achievement_all_you_can_eat();
}