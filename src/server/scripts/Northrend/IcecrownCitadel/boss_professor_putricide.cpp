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
#include "Group.h"
#include "Spell.h"
#include "Vehicle.h"
#include "GridNotifiers.h"
#include "icecrown_citadel.h"

enum Texts
{
	SAY_FESTERGUT_GASEOUS_BLIGHT    = -1659080,
    SAY_FESTERGUT_DEATH             = -1659090,
    SAY_ROTFACE_OOZE_FLOOD_1        = -1659091,
	SAY_ROTFACE_OOZE_FLOOD_2        = -1659092,
    SAY_ROTFACE_DEATH               = -1659093,
    SAY_AGGRO                       = -1659106,
    EMOTE_UNSTABLE_EXPERIMENT       = -1659107,
    SAY_PHASE_TRANSITION_HEROIC     = -1659108,
    SAY_TRANSFORM_1                 = -1659109,
    SAY_TRANSFORM_2                 = -1659110,
    EMOTE_MALLEABLE_GOO             = -1659111,
    EMOTE_CHOKING_GAS_BOMB          = -1659112,
    SAY_KILL_1                      = -1659113,
	SAY_KILL_2                      = -1659114,
    SAY_BERSERK                     = -1659115,
    SAY_DEATH                       = -1659116,
	SAY_INSECTING_SWARM_EVENT       = -1659117
};

enum Spells
{
    // Festergut
    SPELL_RELEASE_GAS_VISUAL              = 69125,
    SPELL_GASEOUS_BLIGHT_LARGE            = 69157,
    SPELL_GASEOUS_BLIGHT_MEDIUM           = 69162,
    SPELL_GASEOUS_BLIGHT_SMALL            = 69164,
	SPELL_MALLEABLE_GOO_H                 = 72296,
	SPELL_MALLEABLE_GOO_SUMMON            = 72299,
    // Professor Putricide
    SPELL_SLIME_PUDDLE_TRIGGER            = 70341,
    SPELL_MALLEABLE_GOO                   = 70852,
    SPELL_UNSTABLE_EXPERIMENT             = 70351,
	SPELL_TEAR_GAS_PROC                   = 71615, // Proc Spell ID 71617
	SPELL_TEAR_GAS_PLAYER                 = 71615,    // phase transition
	SPELL_TEAR_GAS                        = 71617,
    SPELL_TEAR_GAS_CREATURE               = 71618,
    SPELL_TEAR_GAS_CANCEL                 = 71620,
    SPELL_TEAR_GAS_PERIODIC_TRIGGER       = 73170,
    SPELL_CREATE_CONCOCTION               = 71621,
    SPELL_GUZZLE_POTIONS                  = 71893,
    SPELL_OOZE_TANK_PROTECTION            = 71770, // protects the tank
    SPELL_CHOKING_GAS_BOMB                = 71255,
    SPELL_OOZE_VARIABLE                   = 74118,
    SPELL_GAS_VARIABLE                    = 74119,
    SPELL_UNBOUND_PLAGUE                  = 70911,
    SPELL_UNBOUND_PLAGUE_SEARCHER         = 70917,
    SPELL_PLAGUE_SICKNESS                 = 70953,
    SPELL_UNBOUND_PLAGUE_PROTECTION       = 70955,
    SPELL_MUTATED_PLAGUE                  = 72451,
    SPELL_MUTATED_PLAGUE_CLEAR            = 72618,
    // Slime Puddle
    SPELL_GROW_STACKER                    = 70345,
    SPELL_GROW                            = 70347,
    SPELL_SLIME_PUDDLE_AURA               = 70343,
    // Gas Cloud
    SPELL_GASEOUS_BLOAT_PROC              = 70215,
    SPELL_GASEOUS_BLOAT                   = 70672,
    SPELL_GASEOUS_BLOAT_PROTECTION        = 70812,
    SPELL_EXPUNGED_GAS                    = 70701,
    // Volatile Ooze
    SPELL_OOZE_ERUPTION                   = 70492,
    SPELL_VOLATILE_OOZE_ADHESIVE          = 70447,
    SPELL_OOZE_ERUPTION_SEARCH_PERIODIC   = 70457,
    SPELL_VOLATILE_OOZE_PROTECTION        = 70530,
    // Choking Gas Bomb
    SPELL_CHOKING_GAS_BOMB_PERIODIC       = 71259,
    SPELL_CHOKING_GAS_EXPLOSION_TRIGGER   = 71280,
    // Mutated Abomination vehicle
    SPELL_ABOMINATION_VEHICLE_POWER_DRAIN = 70385,
    SPELL_MUTATED_TRANSFORMATION          = 70311,
    SPELL_MUTATED_TRANSFORMATION_DAMAGE   = 70405,
    SPELL_MUTATED_TRANSFORMATION_NAME     = 72401,
	// Unholy Infusion
	SPELL_SHADOW_INFUSION                 = 71516,
	SPELL_UNHOLY_INFUSION_CREDIT          = 71518,

};

#define SPELL_GASEOUS_BLOAT_HELPER RAID_MODE<uint32>(70672, 72455, 72832, 72833)

enum Events
{
    // Festergut
    EVENT_FESTERGUT_DIES        = 1,
    EVENT_FESTERGUT_GOO         = 2,
    // Rotface
    EVENT_ROTFACE_DIES          = 3,
    EVENT_ROTFACE_OOZE_FLOOD    = 5,
    // Professor Putricide
    EVENT_BERSERK               = 6,    // all phases
    EVENT_SLIME_PUDDLE          = 7,    // all phases
    EVENT_UNSTABLE_EXPERIMENT   = 8,    // P1 && P2
    EVENT_TEAR_GAS              = 9,    // phase transition not heroic
    EVENT_RESUME_ATTACK         = 10,
    EVENT_MALLEABLE_GOO         = 11,
    EVENT_CHOKING_GAS_BOMB      = 12,
    EVENT_UNBOUND_PLAGUE        = 13,
    EVENT_MUTATED_PLAGUE        = 14,
    EVENT_PHASE_TRANSITION      = 15
};

enum Phases
{
    PHASE_NONE          = 0,
    PHASE_FESTERGUT     = 1,
    PHASE_ROTFACE       = 2,
    PHASE_COMBAT_1      = 4,
    PHASE_COMBAT_2      = 5,
    PHASE_COMBAT_3      = 6
};

enum Points
{
    POINT_FESTERGUT = 366260,
    POINT_ROTFACE   = 366270,
    POINT_TABLE     = 366780
};

Position const festergutWatchPos = {4324.820f, 3166.03f, 389.3831f, 3.316126f}; //emote 432 (release gas)
Position const rotfaceWatchPos   = {4390.371f, 3164.50f, 389.3890f, 5.497787f}; //emote 432 (release ooze)
Position const tablePos          = {4356.190f, 3262.90f, 389.4820f, 1.483530f};

// used in Rotface encounter
uint32 const OozeFloodSpells[4] = {69782, 69796, 69798, 69801};

enum PutricideData
{
    DATA_EXPERIMENT_STAGE   = 1,
    DATA_PHASE              = 2,
    DATA_ABOMINATION        = 3,
};

#define EXPERIMENT_STATE_OOZE false
#define EXPERIMENT_STATE_GAS  true

class AbominationDespawner
{
    public:
        explicit AbominationDespawner(Unit* owner) : _owner(owner) { }

        bool operator()(uint64 guid)
        {
            if (Unit* summon = ObjectAccessor::GetUnit(*_owner, guid))
            {
                if (summon->GetEntry() == NPC_MUTATED_ABOMINATION_10 || summon->GetEntry() == NPC_MUTATED_ABOMINATION_25)
                {
                    if (Vehicle* veh = summon->GetVehicleKit())
                        veh->RemoveAllPassengers(); // also despawns the vehicle

                    return true;
                }

                return false;
            }

            return true;
        }

    private:
        Unit* _owner;
};

struct RotfaceHeightCheck
{
	RotfaceHeightCheck(Creature* rotface) : _rotface(rotface) { }

	bool operator()(Creature* stalker) const
	{
		return stalker->GetPositionZ() > _rotface->GetPositionZ() + 5.0f;
	}
private:
	Creature* _rotface;
};

class boss_professor_putricide : public CreatureScript
{
    public:
        boss_professor_putricide() : CreatureScript("boss_professor_putricide") { }

        struct boss_professor_putricideAI : public BossAI
        {
            boss_professor_putricideAI(Creature* creature) : BossAI(creature, DATA_PROFESSOR_PUTRICIDE), _baseSpeed(creature->GetSpeedRate(MOVE_RUN)), _experimentState(EXPERIMENT_STATE_OOZE)
            {
                _phase = PHASE_NONE;
            }

			void SetPhase(Phases newPhase)
            {
                _phase = newPhase;
                events.SetPhase(newPhase);
            }

			uint64 _abominationGUID;
			uint64 _oozeFloodDummyGUIDs[4];
            Phases _phase;
            float const _baseSpeed;
            uint8 _oozeFloodStage;
            bool _experimentState;

            void Reset()
            {
				if (!(events.IsInPhase(PHASE_ROTFACE) || events.IsInPhase(PHASE_FESTERGUT)))
                    instance->SetBossState(DATA_PROFESSOR_PUTRICIDE, NOT_STARTED);

                instance->SetData(DATA_NAUSEA_ACHIEVEMENT, uint32(true));

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

				me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_USE_STANDING);

                events.Reset();
                summons.DespawnAll();
                SetPhase(PHASE_COMBAT_1);
                _experimentState = EXPERIMENT_STATE_OOZE;
                me->SetReactState(REACT_DEFENSIVE);
                me->SetWalk(false);

                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == POINT_MOTION_TYPE)
                    me->GetMotionMaster()->MovementExpired();

                if (instance->GetBossState(DATA_ROTFACE) == DONE && instance->GetBossState(DATA_FESTERGUT) == DONE)
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NOT_SELECTABLE);
            }

            void EnterToBattle(Unit* who)
            {
				if (events.IsInPhase(PHASE_ROTFACE) || events.IsInPhase(PHASE_FESTERGUT))
                    return;

                if (!instance->CheckRequiredBosses(DATA_PROFESSOR_PUTRICIDE, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    instance->DoCastSpellOnPlayers(SPELL_LIGHTS_HAMMER_TELEPORT);
                    return;
                }

                me->SetActive(true);
                events.Reset();
                events.ScheduleEvent(EVENT_BERSERK, 600000);
                events.ScheduleEvent(EVENT_SLIME_PUDDLE, 10000);
                events.ScheduleEvent(EVENT_UNSTABLE_EXPERIMENT, urand(30000, 35000));
                if (IsHeroic())
                    events.ScheduleEvent(EVENT_UNBOUND_PLAGUE, 20000);

                SetPhase(PHASE_COMBAT_1);
                DoSendQuantumText(SAY_AGGRO, me);
                DoCast(me, SPELL_OOZE_TANK_PROTECTION, true);
                DoZoneInCombat(me);

                instance->SetBossState(DATA_PROFESSOR_PUTRICIDE, IN_PROGRESS);
            }

            void JustReachedHome()
            {
                _JustReachedHome();
                me->SetWalk(false);

				if (events.IsInPhase(PHASE_COMBAT_1) || events.IsInPhase(PHASE_COMBAT_2) || events.IsInPhase(PHASE_COMBAT_3))
                    instance->SetBossState(DATA_PROFESSOR_PUTRICIDE, FAIL);
            }

            void KilledUnit(Unit* who)
            {
                if (who->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_KILL_1, SAY_KILL_2), me);
            }

            void JustDied(Unit* killer)
            {
                _JustDied();

                DoSendQuantumText(SAY_DEATH, me);

				if (Is25ManRaid() && me->HasAura(SPELL_SHADOWS_FATE))
				{
					if (Map* map = me->GetMap())
					{
						Map::PlayerList const &PlayerList = map->GetPlayers();
						for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
						{
							if (Player* player = itr->getSource())
							{
								if (player->HasAura(SPELL_SHADOW_INFUSION) && player->GetQuestStatus(QUEST_UNHOLY_INFUSION) == QUEST_STATUS_INCOMPLETE)
									player->CompleteQuest(QUEST_UNHOLY_INFUSION);
							}
						}
					}
				}

				// Quest credit
				if (Player* player = killer->ToPlayer())
				{
					player->CastSpell(player, SPELL_SOUL_FEAST, true);
					me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
				}

                DoCast(SPELL_MUTATED_PLAGUE_CLEAR);
				instance->DoRemoveAurasDueToSpellOnPlayers(RAID_MODE(70911, 72854, 72855, 72856)); // Unbound Plague
				instance->DoRemoveAurasDueToSpellOnPlayers(RAID_MODE(70953, 73117, 70953, 73117)); // Plague Sickness
				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_UNBOUND_PLAGUE_SEARCHER); // Unbound Plague Search Periodic
            }

            void JustSummoned(Creature* summon)
            {
                summons.Summon(summon);
                switch (summon->GetEntry())
                {
					case NPC_MALLEABLE_OOZE_STALKER:
						DoCast(summon, SPELL_MALLEABLE_GOO_H);
						return;
                    case NPC_GROWING_OOZE_PUDDLE:
                        summon->CastSpell(summon, SPELL_GROW_STACKER, true);
                        summon->CastSpell(summon, SPELL_SLIME_PUDDLE_AURA, true);
                        // blizzard casts this spell 7 times initially (confirmed in sniff)
                        for (uint8 i = 0; i < 7; ++i)
                            summon->CastSpell(summon, SPELL_GROW, true);
                        break;
                    case NPC_GAS_CLOUD:
                        // no possible aura seen in sniff adding the aurastate
                        summon->ModifyAuraState(AURA_STATE_UNKNOWN22, true);
                        summon->CastSpell(summon, SPELL_GASEOUS_BLOAT_PROC, true);
                        summon->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                        summon->SetReactState(REACT_PASSIVE);
                        break;
                    case NPC_VOLATILE_OOZE:
                        // no possible aura seen in sniff adding the aurastate
                        summon->ModifyAuraState(AURA_STATE_UNKNOWN19, true);
                        summon->CastSpell(summon, SPELL_OOZE_ERUPTION_SEARCH_PERIODIC, true);
                        summon->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                        summon->SetReactState(REACT_PASSIVE);
                        break;
                    case NPC_CHOKING_GAS_BOMB:
                        summon->CastSpell(summon, SPELL_CHOKING_GAS_BOMB_PERIODIC, true);
                        summon->CastSpell(summon, SPELL_CHOKING_GAS_EXPLOSION_TRIGGER, true);
                        return;
                    case NPC_MUTATED_ABOMINATION_10:
                    case NPC_MUTATED_ABOMINATION_25:
						_abominationGUID = summon->GetGUID();
                        return;
                    default:
                        break;
                }

                if (me->IsInCombatActive())
                    DoZoneInCombat(summon);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& /*damage*/)
            {
                switch (_phase)
                {
                    case PHASE_COMBAT_1:
                        if (HealthAbovePct(80))
                            return;
                        me->SetReactState(REACT_PASSIVE);
                        DoAction(ACTION_CHANGE_PHASE);
                        break;
                    case PHASE_COMBAT_2:
                        if (HealthAbovePct(35))
                            return;
                        me->SetReactState(REACT_PASSIVE);
                        DoAction(ACTION_CHANGE_PHASE);
                        break;
                    default:
                        break;
                }
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE)
                    return;
                switch (id)
                {
                    case POINT_FESTERGUT:
                        instance->SetBossState(DATA_FESTERGUT, IN_PROGRESS); // needed here for delayed gate close
                        me->SetSpeed(MOVE_RUN, _baseSpeed, true);
                        DoAction(ACTION_FESTERGUT_GAS);
                        if (Creature* festergut = Unit::GetCreature(*me, instance->GetData64(DATA_FESTERGUT)))
                            festergut->CastSpell(festergut, SPELL_GASEOUS_BLIGHT_LARGE, false, 0, 0, festergut->GetGUID());
                        break;
                    case POINT_ROTFACE:
                        instance->SetBossState(DATA_ROTFACE, IN_PROGRESS);   // needed here for delayed gate close
                        me->SetSpeed(MOVE_RUN, _baseSpeed, true);
                        DoAction(ACTION_ROTFACE_OOZE);
                        events.ScheduleEvent(EVENT_ROTFACE_OOZE_FLOOD, 25000, 0, PHASE_ROTFACE);
                        break;
                    case POINT_TABLE:
                        // stop attack
                        me->GetMotionMaster()->MoveIdle();
                        me->SetSpeed(MOVE_RUN, _baseSpeed, true);
                        if (GameObject* table = ObjectAccessor::GetGameObject(*me, instance->GetData64(DATA_PUTRICIDE_TABLE)))
                            me->SetFacingToObject(table);
                        // operating on new phase already
                        switch (_phase)
                        {
                            case PHASE_COMBAT_2:
                            {
                                SpellInfo const* spell = sSpellMgr->GetSpellInfo(SPELL_CREATE_CONCOCTION);
                                DoCast(me, SPELL_CREATE_CONCOCTION);
                                events.ScheduleEvent(EVENT_PHASE_TRANSITION, sSpellMgr->GetSpellForDifficultyFromSpell(spell, me)->CalcCastTime() + 100);
                                break;
                            }
                            case PHASE_COMBAT_3:
                            {
                                SpellInfo const* spell = sSpellMgr->GetSpellInfo(SPELL_GUZZLE_POTIONS);
                                DoCast(me, SPELL_GUZZLE_POTIONS);
                                events.ScheduleEvent(EVENT_PHASE_TRANSITION, sSpellMgr->GetSpellForDifficultyFromSpell(spell, me)->CalcCastTime() + 100);
                                break;
                            }
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                }
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_FESTERGUT_COMBAT:
                        SetPhase(PHASE_FESTERGUT);
                        me->SetSpeed(MOVE_RUN, _baseSpeed*2.0f, true);
                        me->GetMotionMaster()->MovePoint(POINT_FESTERGUT, festergutWatchPos);
                        me->SetReactState(REACT_PASSIVE);
                        DoZoneInCombat(me);
                        if (IsHeroic())
                            events.ScheduleEvent(EVENT_FESTERGUT_GOO, urand(13000, 18000), 0, PHASE_FESTERGUT);
                        break;
                    case ACTION_FESTERGUT_GAS:
                        DoSendQuantumText(SAY_FESTERGUT_GASEOUS_BLIGHT, me);
                        DoCast(me, SPELL_RELEASE_GAS_VISUAL, true);
                        break;
                    case ACTION_FESTERGUT_DEATH:
                        events.ScheduleEvent(EVENT_FESTERGUT_DIES, 4000, 0, PHASE_FESTERGUT);
                        break;
                    case ACTION_ROTFACE_COMBAT:
                    {
                        SetPhase(PHASE_ROTFACE);
                        me->SetSpeed(MOVE_RUN, _baseSpeed*2.0f, true);
                        me->GetMotionMaster()->MovePoint(POINT_ROTFACE, rotfaceWatchPos);
                        me->SetReactState(REACT_PASSIVE);
                        _oozeFloodStage = 0;
                        DoZoneInCombat(me);
                        // init random sequence of floods
						if (Creature* rotface = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_ROTFACE)))
                        {
                            std::list<Creature*> list;
                            GetCreatureListWithEntryInGrid(list, rotface, NPC_PUDDLE_STALKER, 50.0f);
                            list.remove_if(RotfaceHeightCheck(rotface));
                            if (list.size() > 4)
                            {
                                list.sort(Trinity::ObjectDistanceOrderPred(rotface));
                                do
                                {
                                    list.pop_back();
                                } while (list.size() > 4);
                            }

                            uint8 i = 0;
                            while (!list.empty())
                            {
                                std::list<Creature*>::iterator itr = list.begin();
                                std::advance(itr, urand(0, list.size()-1));
                                _oozeFloodDummyGUIDs[i++] = (*itr)->GetGUID();
                                list.erase(itr);
                            }
                        }
                        break;
                    }
                    case ACTION_ROTFACE_OOZE:
                        DoSendQuantumText(RAND(SAY_ROTFACE_OOZE_FLOOD_1, SAY_ROTFACE_OOZE_FLOOD_2), me);
                        if (Creature* dummy = Unit::GetCreature(*me, _oozeFloodDummyGUIDs[_oozeFloodStage]))
                            dummy->CastSpell(dummy, OozeFloodSpells[_oozeFloodStage], true, 0, 0, me->GetGUID()); // cast from self for LoS (with prof's GUID for logs)
                        if (++_oozeFloodStage == 4)
                            _oozeFloodStage = 0;
                        break;
                    case ACTION_ROTFACE_DEATH:
                        events.ScheduleEvent(EVENT_ROTFACE_DIES, 4500, 0, PHASE_ROTFACE);
                        break;
                    case ACTION_CHANGE_PHASE:
                        me->SetSpeed(MOVE_RUN, _baseSpeed*2.0f, true);
                        events.DelayEvents(30000);
                        me->AttackStop();
                        if (!IsHeroic())
                        {
                            DoCast(me, SPELL_TEAR_GAS);
                            events.ScheduleEvent(EVENT_TEAR_GAS, 2500);
                        }
                        else
                        {
                            DoSendQuantumText(SAY_PHASE_TRANSITION_HEROIC, me);
                            DoCast(me, SPELL_UNSTABLE_EXPERIMENT, true);
                            DoCast(me, SPELL_UNSTABLE_EXPERIMENT, true);
                            // cast variables
                            if (Is25ManRaid())
                            {
                                std::list<Unit*> targetList;
                                {
                                    const std::list<HostileReference*>& threatlist = me->getThreatManager().getThreatList();
                                    for (std::list<HostileReference*>::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
                                        if ((*itr)->getTarget()->GetTypeId() == TYPE_ID_PLAYER)
                                            targetList.push_back((*itr)->getTarget());
                                }

                                size_t half = targetList.size()/2;
                                // half gets ooze variable
                                while (half < targetList.size())
                                {
                                    std::list<Unit*>::iterator itr = targetList.begin();
                                    advance(itr, urand(0, targetList.size() - 1));
                                    (*itr)->CastSpell(*itr, SPELL_OOZE_VARIABLE, true);
                                    targetList.erase(itr);
                                }
                                // and half gets gas
                                for (std::list<Unit*>::iterator itr = targetList.begin(); itr != targetList.end(); ++itr)
                                    (*itr)->CastSpell(*itr, SPELL_GAS_VARIABLE, true);
                            }
                            me->GetMotionMaster()->MovePoint(POINT_TABLE, tablePos);
                        }
                        switch (_phase)
                        {
                            case PHASE_COMBAT_1:
                                SetPhase(PHASE_COMBAT_2);
                                events.ScheduleEvent(EVENT_MALLEABLE_GOO, urand(21000, 26000));
                                events.ScheduleEvent(EVENT_CHOKING_GAS_BOMB, urand(35000, 40000));
                                break;
                            case PHASE_COMBAT_2:
                                SetPhase(PHASE_COMBAT_3);
                                events.ScheduleEvent(EVENT_MUTATED_PLAGUE, 25000);
                                events.CancelEvent(EVENT_UNSTABLE_EXPERIMENT);
                                break;
                            default:
                                break;
                        }
                        break;
                    default:
                        break;
                }
            }

            uint32 GetData(uint32 type)
            {
                switch (type)
                {
                    case DATA_EXPERIMENT_STAGE:
                    {
                        // ALSO MODIFIES!
                        uint32 ret = uint32(_experimentState);
                        _experimentState ^= true;
                        return ret;
                    }
                    case DATA_PHASE:
                        return _phase;
                    case DATA_ABOMINATION:
                        summons.RemoveNotExisting();
                        return uint32(summons.HasEntry(NPC_MUTATED_ABOMINATION_10) || summons.HasEntry(NPC_MUTATED_ABOMINATION_25));
                    default:
                        break;
                }

                return 0;
            }

            void UpdateAI(uint32 const diff)
            {
                if ((!(events.IsInPhase(PHASE_ROTFACE) || events.IsInPhase(PHASE_FESTERGUT)) && !UpdateVictim()) || !CheckInRoom())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FESTERGUT_DIES:
                            DoSendQuantumText(SAY_FESTERGUT_DEATH, me);
                            EnterEvadeMode();
                            break;
                        case EVENT_FESTERGUT_GOO:
							me->CastCustomSpell(SPELL_MALLEABLE_GOO_SUMMON, SPELLVALUE_MAX_TARGETS, 1, NULL, true);
							events.ScheduleEvent(EVENT_FESTERGUT_GOO, (Is25ManRaid() ? 10000 : 30000) + urand(0, 5000), 0, PHASE_FESTERGUT);
                            break;
                        case EVENT_ROTFACE_DIES:
                            DoSendQuantumText(SAY_ROTFACE_DEATH, me);
                            EnterEvadeMode();
                            break;
                        case EVENT_ROTFACE_OOZE_FLOOD:
                            DoAction(ACTION_ROTFACE_OOZE);
                            events.ScheduleEvent(EVENT_ROTFACE_OOZE_FLOOD, 25000, 0, PHASE_ROTFACE);
                            break;
                        case EVENT_BERSERK:
                            DoSendQuantumText(SAY_BERSERK, me);
                            DoCast(me, SPELL_BERSERK_2);
                            break;
                        case EVENT_SLIME_PUDDLE:
                        {
                            std::list<Unit*> targets;
                            SelectTargetList(targets, 2, TARGET_RANDOM, 0.0f, true);
                            if (!targets.empty())
                                for (std::list<Unit*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
                                    DoCast(*itr, SPELL_SLIME_PUDDLE_TRIGGER);
                            events.ScheduleEvent(EVENT_SLIME_PUDDLE, 35000);
                            break;
                        }
                        case EVENT_UNSTABLE_EXPERIMENT:
                            DoSendQuantumText(EMOTE_UNSTABLE_EXPERIMENT, me);
                            DoCast(me, SPELL_UNSTABLE_EXPERIMENT);
                            events.ScheduleEvent(EVENT_UNSTABLE_EXPERIMENT, urand(35000, 40000));
                            break;
                        case EVENT_TEAR_GAS:
                            me->GetMotionMaster()->MovePoint(POINT_TABLE, tablePos);
                            DoCast(me, SPELL_TEAR_GAS_PERIODIC_TRIGGER, true);
                            break;
                        case EVENT_RESUME_ATTACK:
                            me->SetReactState(REACT_DEFENSIVE);
                            AttackStart(me->GetVictim());
                            // remove Tear Gas
                            me->RemoveAurasDueToSpell(SPELL_TEAR_GAS_PERIODIC_TRIGGER);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TEAR_GAS_PROC);
                            DoCastAOE(SPELL_TEAR_GAS_CANCEL);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_GAS_VARIABLE);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_OOZE_VARIABLE);
							if (Creature* abomination = ObjectAccessor::GetCreature(*me, _abominationGUID))
								abomination->RemoveAurasDueToSpell(SPELL_TEAR_GAS_PLAYER);
                            break;
                        case EVENT_MALLEABLE_GOO:
                            if (Is25ManRaid())
                            {
                                std::list<Unit*> targets;
                                SelectTargetList(targets, 2, TARGET_RANDOM, -7.0f, true);
                                if (!targets.empty())
                                {
                                    DoSendQuantumText(EMOTE_MALLEABLE_GOO, me);
                                    for (std::list<Unit*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
                                        DoCast(*itr, SPELL_MALLEABLE_GOO);
                                }
                            }
                            else
                            {
                                if (Unit* target = SelectTarget(TARGET_RANDOM, 1, -7.0f, true))
                                {
                                    DoSendQuantumText(EMOTE_MALLEABLE_GOO, me);
                                    DoCast(target, SPELL_MALLEABLE_GOO);
                                }
                            }
                            events.ScheduleEvent(EVENT_MALLEABLE_GOO, urand(25000, 30000));
                            break;
                        case EVENT_CHOKING_GAS_BOMB:
                            DoSendQuantumText(EMOTE_CHOKING_GAS_BOMB, me);
                            DoCast(me, SPELL_CHOKING_GAS_BOMB);
                            events.ScheduleEvent(EVENT_CHOKING_GAS_BOMB, urand(35000, 40000));
                            break;
                        case EVENT_UNBOUND_PLAGUE:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, NonTankTargetSelector(me)))
                            {
                                DoCast(target, SPELL_UNBOUND_PLAGUE);
                                DoCast(target, SPELL_UNBOUND_PLAGUE_SEARCHER);
                            }
                            events.ScheduleEvent(EVENT_UNBOUND_PLAGUE, 90000);
                            break;
                        case EVENT_MUTATED_PLAGUE:
                            DoCastVictim(SPELL_MUTATED_PLAGUE);
                            events.ScheduleEvent(EVENT_MUTATED_PLAGUE, 10000);
                            break;
                        case EVENT_PHASE_TRANSITION:
                        {
                            switch (_phase)
                            {
                                case PHASE_COMBAT_2:
                                    if (Creature* face = me->FindCreatureWithDistance(NPC_TEAR_GAS_TARGET_STALKER, 50.0f))
                                        me->SetFacingToObject(face);
                                    me->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
                                    DoSendQuantumText(SAY_TRANSFORM_1, me);
                                    events.ScheduleEvent(EVENT_RESUME_ATTACK, 5500, 0, PHASE_COMBAT_2);
                                    break;
                                case PHASE_COMBAT_3:
                                    if (Creature* face = me->FindCreatureWithDistance(NPC_TEAR_GAS_TARGET_STALKER, 50.0f))
                                        me->SetFacingToObject(face);
                                    me->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
                                    DoSendQuantumText(SAY_TRANSFORM_2, me);
									summons.remove_if(AbominationDespawner(me));
                                    events.ScheduleEvent(EVENT_RESUME_ATTACK, 8500, 0, PHASE_COMBAT_3);
                                    break;
                                default:
                                    break;
                            }
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<boss_professor_putricideAI>(creature);
        }
};

class npc_putricide_oozeAI : public QuantumBasicAI
{
    public:
        npc_putricide_oozeAI(Creature* creature, uint32 hitTargetSpellId) : QuantumBasicAI(creature), _hitTargetSpellId(hitTargetSpellId), NewTargetSelectTimer(0)
		{
			_switched = true;
		}

        void SpellHitTarget(Unit* /*target*/, SpellInfo const* spell)
        {
            if (!NewTargetSelectTimer && spell->Id == sSpellMgr->GetSpellIdForDifficulty(_hitTargetSpellId, me))
                NewTargetSelectTimer = 1000;
        }

        void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
        {
            if (spell->Id == SPELL_TEAR_GAS_CREATURE)
                NewTargetSelectTimer = 1000;
        }

        void UpdateAI(uint32 const diff)
        {
			// test
			if (!_switched)
			{
				if (InstanceScript* instance = me->GetInstanceScript())
				{
					Map::PlayerList const &PlayerList = instance->instance->GetPlayers();
					for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
					{
						if (Player* player = i->getSource())
						{
							if (player->HasAura(RAID_MODE<uint32>(70672, 72455, 72832, 72833)) && player->HasAura(RAID_MODE<uint32>(70477, 72836, 72837, 72838)) && me->GetEntry() == NPC_VOLATILE_OOZE)
							{
								CastMainSpell();
								NewTargetSelectTimer = 0;
								_switched = true;
							}
						}
					}
				}
			} // test

            if (!UpdateVictim() && !NewTargetSelectTimer)
                return;

            if (!NewTargetSelectTimer && !me->IsNonMeleeSpellCasted(false, false, true, false, true))
                NewTargetSelectTimer = 1000;

            DoMeleeAttackIfReady();

            if (!NewTargetSelectTimer)
                return;

            if (me->HasAura(SPELL_TEAR_GAS_CREATURE))
                return;

            if (NewTargetSelectTimer <= diff)
            {
                NewTargetSelectTimer = 0;
                CastMainSpell();
            }
            else
                NewTargetSelectTimer -= diff;
        }

        virtual void CastMainSpell() = 0;

    private:
        uint32 _hitTargetSpellId;
        uint32 NewTargetSelectTimer;
		bool _switched;
};

class npc_volatile_ooze : public CreatureScript
{
    public:
        npc_volatile_ooze() : CreatureScript("npc_volatile_ooze") { }

        struct npc_volatile_oozeAI : public npc_putricide_oozeAI
        {
            npc_volatile_oozeAI(Creature* creature) : npc_putricide_oozeAI(creature, SPELL_OOZE_ERUPTION){}

            void CastMainSpell()
            {
                me->CastSpell(me, SPELL_VOLATILE_OOZE_ADHESIVE, false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_volatile_oozeAI>(creature);
        }
};

class npc_gas_cloud : public CreatureScript
{
    public:
        npc_gas_cloud() : CreatureScript("npc_gas_cloud") { }

        struct npc_gas_cloudAI : public npc_putricide_oozeAI
        {
            npc_gas_cloudAI(Creature* creature) : npc_putricide_oozeAI(creature, SPELL_EXPUNGED_GAS)
            {
                NewTargetSelectTimer = 0;
            }

			uint32 NewTargetSelectTimer;

            void CastMainSpell()
            {
                me->CastCustomSpell(SPELL_GASEOUS_BLOAT, SPELLVALUE_AURA_STACK, 10, me, false);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_gas_cloudAI>(creature);
        }
};

class spell_putricide_gaseous_bloat : public SpellScriptLoader
{
    public:
        spell_putricide_gaseous_bloat() : SpellScriptLoader("spell_putricide_gaseous_bloat") { }

        class spell_putricide_gaseous_bloat_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_putricide_gaseous_bloat_AuraScript);

            void HandleExtraEffect(AuraEffect const* /*aurEff*/)
            {
                GetTarget()->RemoveAuraFromStack(GetSpellInfo()->Id, GetCasterGUID());
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_putricide_gaseous_bloat_AuraScript::HandleExtraEffect, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            }
        };

        class spell_putricide_gaseous_bloat_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_gaseous_bloat_SpellScript);

            void ModAuraStack()
            {
                if (Aura* aura = GetHitAura())
                    aura->SetStackAmount(10);
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_putricide_gaseous_bloat_SpellScript::ModAuraStack);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_putricide_gaseous_bloat_AuraScript();
        }
};

class spell_putricide_ooze_channel : public SpellScriptLoader
{
    public:
        spell_putricide_ooze_channel() : SpellScriptLoader("spell_putricide_ooze_channel") { }

        class spell_putricide_ooze_channel_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_ooze_channel_SpellScript);

            bool Validate(SpellInfo const* spell)
            {
                if (!spell->ExcludeTargetAuraSpell)
                    return false;
                if (!sSpellMgr->GetSpellInfo(spell->ExcludeTargetAuraSpell))
                    return false;
                return true;
            }

            // set up initial variables and check if caster is creature
            // this will let use safely use ToCreature() casts in entire script
            bool Load()
            {
                _target = 0;
                return GetCaster()->GetTypeId() == TYPE_ID_UNIT;
            }

            void SelectTarget(std::list<Unit*>& targetList)
            {
                if (targetList.empty())
                {
                    FinishCast(SPELL_FAILED_NO_VALID_TARGETS);
                    GetCaster()->ToCreature()->DespawnAfterAction(1);    // despawn next update
                    return;
                }

                Unit* target = Quantum::DataPackets::SelectRandomContainerElement(targetList);
                targetList.clear();
                targetList.push_back(target);
                _target = target;
            }

            void SetTarget(std::list<Unit*>& targetList)
            {
                targetList.clear();
                if (_target)
                    targetList.push_back(_target);
            }

            void StartAttack()
            {
                GetCaster()->ClearUnitState(UNIT_STATE_CASTING);
                GetCaster()->DeleteThreatList();
                GetCaster()->ToCreature()->AI()->AttackStart(GetHitUnit());
                GetCaster()->AddThreat(GetHitUnit(), 500000000.0f);    // value seen in sniff
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_putricide_ooze_channel_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_putricide_ooze_channel_SpellScript::SetTarget, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_putricide_ooze_channel_SpellScript::SetTarget, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
                AfterHit += SpellHitFn(spell_putricide_ooze_channel_SpellScript::StartAttack);
            }

            Unit* _target;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_putricide_ooze_channel_SpellScript();
        }
};

class ExactDistanceCheck
{
    public:
        ExactDistanceCheck(Unit* source, float dist) : _source(source), _dist(dist) {}

        bool operator()(Unit* unit)
        {
            return _source->GetExactDist2d(unit) > _dist;
        }

    private:
        Unit* _source;
        float _dist;
};

class spell_putricide_slime_puddle : public SpellScriptLoader
{
    public:
        spell_putricide_slime_puddle() : SpellScriptLoader("spell_putricide_slime_puddle") { }

        class spell_putricide_slime_puddle_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_slime_puddle_SpellScript);

            void ScaleRange(std::list<Unit*>& targets)
            {
                targets.remove_if(ExactDistanceCheck(GetCaster(), 2.5f * GetCaster()->GetFloatValue(OBJECT_FIELD_SCALE_X)));
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_putricide_slime_puddle_SpellScript::ScaleRange, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_putricide_slime_puddle_SpellScript::ScaleRange, EFFECT_1, TARGET_UNIT_DEST_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_putricide_slime_puddle_SpellScript();
        }
};

class spell_putricide_slime_puddle_aura : public SpellScriptLoader
{
    public:
        spell_putricide_slime_puddle_aura() : SpellScriptLoader("spell_putricide_slime_puddle_aura") { }

        class spell_putricide_slime_puddle_aura_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_slime_puddle_aura_SpellScript);

            void ReplaceAura()
            {
                if (Unit* target = GetHitUnit())
                    GetCaster()->AddAura((GetCaster()->GetMap()->GetSpawnMode() & 1) ? 72456 : 70346, target);
            }

            void Register()
            {
                OnHit += SpellHitFn(spell_putricide_slime_puddle_aura_SpellScript::ReplaceAura);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_putricide_slime_puddle_aura_SpellScript();
        }
};

class spell_putricide_unstable_experiment : public SpellScriptLoader
{
    public:
        spell_putricide_unstable_experiment() : SpellScriptLoader("spell_putricide_unstable_experiment") { }

        class spell_putricide_unstable_experiment_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_unstable_experiment_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (GetCaster()->GetTypeId() != TYPE_ID_UNIT)
                    return;

                uint32 stage = GetCaster()->ToCreature()->AI()->GetData(DATA_EXPERIMENT_STAGE);
                Creature* target = 0;
                std::list<Creature*> creList;
                GetCreatureListWithEntryInGrid(creList, GetCaster(), NPC_ABOMINATION_WING_MAD_SCIENTIST_STALKER, 200.0f);
                // 2 of them are spawned at green place - weird trick blizz
                for (std::list<Creature*>::iterator itr = creList.begin(); itr != creList.end(); ++itr)
                {
                    target = *itr;
                    std::list<Creature*> tmp;
                    GetCreatureListWithEntryInGrid(tmp, target, NPC_ABOMINATION_WING_MAD_SCIENTIST_STALKER, 10.0f);
                    if ((!stage && tmp.size() > 1) || (stage && tmp.size() == 1))
                        break;
                }

                GetCaster()->CastSpell(target, uint32(GetSpellInfo()->Effects[stage].CalcValue()), true, 0, 0, GetCaster()->GetGUID());
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_putricide_unstable_experiment_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_putricide_unstable_experiment_SpellScript();
        }
};

class spell_putricide_ooze_eruption_searcher : public SpellScriptLoader
{
    public:
        spell_putricide_ooze_eruption_searcher() : SpellScriptLoader("spell_putricide_ooze_eruption_searcher") { }

        class spell_putricide_ooze_eruption_searcher_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_ooze_eruption_searcher_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                uint32 adhesiveId = sSpellMgr->GetSpellIdForDifficulty(SPELL_VOLATILE_OOZE_ADHESIVE, GetCaster());
                if (GetHitUnit()->HasAura(adhesiveId))
                {
                    GetHitUnit()->RemoveAurasDueToSpell(adhesiveId, GetCaster()->GetGUID(), 0, AURA_REMOVE_BY_ENEMY_SPELL);
					GetCaster()->CastSpell(GetHitUnit(), SPELL_OOZE_ERUPTION, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_putricide_ooze_eruption_searcher_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_putricide_ooze_eruption_searcher_SpellScript();
        }
};

class spell_putricide_choking_gas_bomb : public SpellScriptLoader
{
    public:
        spell_putricide_choking_gas_bomb() : SpellScriptLoader("spell_putricide_choking_gas_bomb") { }

        class spell_putricide_choking_gas_bomb_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_choking_gas_bomb_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                uint32 skipIndex = urand(0, 2);
                for (uint32 i = 0; i < 3; ++i)
                {
                    if (i == skipIndex)
                        continue;

                    uint32 spellId = uint32(GetSpellInfo()->Effects[i].CalcValue());
                    GetCaster()->CastSpell(GetCaster(), spellId, true, 0, 0, GetCaster()->GetGUID());
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_putricide_choking_gas_bomb_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_putricide_choking_gas_bomb_SpellScript();
        }
};

class spell_putricide_unbound_plague : public SpellScriptLoader
{
    public:
        spell_putricide_unbound_plague() : SpellScriptLoader("spell_putricide_unbound_plague") { }

        class spell_putricide_unbound_plague_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_unbound_plague_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_UNBOUND_PLAGUE))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_UNBOUND_PLAGUE_SEARCHER))
                    return false;
                return true;
            }

			void FilterTargets(std::list<Unit*>& unitList)
            {
                if (AuraEffect const* eff = GetCaster()->GetAuraEffect(SPELL_UNBOUND_PLAGUE_SEARCHER, EFFECT_0))
                {
                    if (eff->GetTickNumber() < 2)
                    {
                        unitList.clear();
                        return;
                    }
                }

                unitList.remove_if(Trinity::UnitAuraCheck(true, sSpellMgr->GetSpellIdForDifficulty(SPELL_UNBOUND_PLAGUE, GetCaster())));
                Quantum::DataPackets::RandomResizeList(unitList, 1);
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (!GetHitUnit())
                    return;

                InstanceScript* instance = GetCaster()->GetInstanceScript();
                if (!instance)
                    return;

                uint32 plagueId = sSpellMgr->GetSpellIdForDifficulty(SPELL_UNBOUND_PLAGUE, GetCaster());

                if (!GetHitUnit()->HasAura(plagueId))
                {
                    if (Creature* professor = ObjectAccessor::GetCreature(*GetCaster(), instance->GetData64(DATA_PROFESSOR_PUTRICIDE)))
                    {
                        if (Aura* oldPlague = GetCaster()->GetAura(plagueId, professor->GetGUID()))
                        {
                            if (Aura* newPlague = professor->AddAura(plagueId, GetHitUnit()))
                            {
                                newPlague->SetMaxDuration(oldPlague->GetMaxDuration());
                                newPlague->SetDuration(oldPlague->GetDuration());
                                oldPlague->Remove();
                                GetCaster()->RemoveAurasDueToSpell(SPELL_UNBOUND_PLAGUE_SEARCHER);
                                GetCaster()->CastSpell(GetCaster(), SPELL_PLAGUE_SICKNESS, true);
                                GetCaster()->CastSpell(GetCaster(), SPELL_UNBOUND_PLAGUE_PROTECTION, true);
                                professor->CastSpell(GetHitUnit(), SPELL_UNBOUND_PLAGUE_SEARCHER, true);
                            }
                        }
                    }
                }
            }

            void Register()
            {
				OnUnitTargetSelect += SpellUnitTargetFn(spell_putricide_unbound_plague_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnEffectHitTarget += SpellEffectFn(spell_putricide_unbound_plague_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_putricide_unbound_plague_SpellScript();
        }
};

class spell_putricide_eat_ooze : public SpellScriptLoader
{
    public:
        spell_putricide_eat_ooze() : SpellScriptLoader("spell_putricide_eat_ooze") { }

        class spell_putricide_eat_ooze_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_eat_ooze_SpellScript);

            void SelectTarget(std::list<Unit*>& targets)
            {
                if (targets.empty())
                    return;

                targets.sort(Trinity::ObjectDistanceOrderPred(GetCaster()));
                Unit* target = targets.front();
                targets.clear();
                targets.push_back(target);
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Creature* target = GetHitCreature();
                if (!target)
                    return;

                if (Aura* grow = target->GetAura(uint32(GetEffectValue())))
                {
                    if (grow->GetStackAmount() < 3)
                    {
                        target->RemoveAurasDueToSpell(SPELL_GROW_STACKER);
                        target->RemoveAura(grow);
                        target->DespawnAfterAction(1);
                    }
                    else
                        grow->ModStackAmount(-3);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_putricide_eat_ooze_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_putricide_eat_ooze_SpellScript::SelectTarget, EFFECT_0, TARGET_UNIT_DEST_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_putricide_eat_ooze_SpellScript();
        }
};

class spell_putricide_mutated_plague : public SpellScriptLoader
{
    public:
        spell_putricide_mutated_plague() : SpellScriptLoader("spell_putricide_mutated_plague") { }

        class spell_putricide_mutated_plague_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_putricide_mutated_plague_AuraScript);

            void HandleTriggerSpell(AuraEffect const* aurEff)
            {
                PreventDefaultAction();
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                uint32 triggerSpell = GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell;
                SpellInfo const* spell = sSpellMgr->GetSpellInfo(triggerSpell);
                spell = sSpellMgr->GetSpellForDifficultyFromSpell(spell, caster);

                int32 damage = spell->Effects[EFFECT_0].CalcValue(caster);
                float multiplier = 2.0f;
                if (GetTarget()->GetMap()->GetSpawnMode() & 1)
                    multiplier = 3.0f;

                damage *= int32(pow(multiplier, GetStackAmount()));
                damage = int32(damage * 1.5f);

                GetTarget()->CastCustomSpell(triggerSpell, SPELLVALUE_BASE_POINT0, damage, GetTarget(), true, 0, aurEff, GetCasterGUID());
            }

            void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                uint32 healSpell = uint32(GetSpellInfo()->Effects[EFFECT_0].CalcValue());
                uint8 amount = 0;

				if (aurEff)
				{
					if (aurEff->GetBase())
					{
						amount = aurEff->GetBase()->GetStackAmount();
					}
				}

				// Putricide should be healed by 300k * stack amount
				for (uint8 i = 0; i < amount; ++i)
				{
					GetTarget()->CastSpell(GetTarget(), healSpell, true, 0, 0, GetCasterGUID());
				}
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_putricide_mutated_plague_AuraScript::HandleTriggerSpell, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_putricide_mutated_plague_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_putricide_mutated_plague_AuraScript();
        }
};

class spell_putricide_mutation_init : public SpellScriptLoader
{
    public:
        spell_putricide_mutation_init() : SpellScriptLoader("spell_putricide_mutation_init") { }

        class spell_putricide_mutation_init_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_mutation_init_SpellScript);

            SpellCastResult CheckRequirementInternal(SpellCustomErrors& extendedError)
            {
                InstanceScript* instance = GetExplTargetUnit()->GetInstanceScript();
                if (!instance)
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                Creature* professor = ObjectAccessor::GetCreature(*GetExplTargetUnit(), instance->GetData64(DATA_PROFESSOR_PUTRICIDE));
                if (!professor)
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                if (professor->AI()->GetData(DATA_PHASE) == PHASE_COMBAT_3 || !professor->IsAlive())
                {
                    extendedError = SPELL_CUSTOM_ERROR_ALL_POTIONS_USED;
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                if (professor->AI()->GetData(DATA_ABOMINATION))
                {
                    extendedError = SPELL_CUSTOM_ERROR_TOO_MANY_ABOMINATIONS;
                    return SPELL_FAILED_CUSTOM_ERROR;
                }

                return SPELL_CAST_OK;
            }

            SpellCastResult CheckRequirement()
            {
                if (!GetExplTargetUnit())
                    return SPELL_FAILED_BAD_TARGETS;

                if (GetExplTargetUnit()->GetTypeId() != TYPE_ID_PLAYER)
                    return SPELL_FAILED_TARGET_NOT_PLAYER;

                SpellCustomErrors extension = SPELL_CUSTOM_ERROR_NONE;
                SpellCastResult result = CheckRequirementInternal(extension);
                if (result != SPELL_CAST_OK)
                {
                    Spell::SendCastResult(GetExplTargetUnit()->ToPlayer(), GetSpellInfo(), 0, result, extension);
                    return result;
                }

                return SPELL_CAST_OK;
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_putricide_mutation_init_SpellScript::CheckRequirement);
            }
        };

        class spell_putricide_mutation_init_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_putricide_mutation_init_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                uint32 spellId = 70311;
                if (GetTarget()->GetMap()->GetSpawnMode() & 1)
                    spellId = 71503;

                GetTarget()->CastSpell(GetTarget(), spellId, true);
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_putricide_mutation_init_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_putricide_mutation_init_SpellScript();
        }

        AuraScript* GetAuraScript() const
        {
            return new spell_putricide_mutation_init_AuraScript();
        }
};

class spell_putricide_mutated_transformation_dismiss : public SpellScriptLoader
{
    public:
        spell_putricide_mutated_transformation_dismiss() : SpellScriptLoader("spell_putricide_mutated_transformation_dismiss") { }

        class spell_putricide_mutated_transformation_dismiss_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_putricide_mutated_transformation_dismiss_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Vehicle* veh = GetTarget()->GetVehicleKit())
                    veh->RemoveAllPassengers();
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_putricide_mutated_transformation_dismiss_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_putricide_mutated_transformation_dismiss_AuraScript();
        }
};

class spell_putricide_mutated_transformation : public SpellScriptLoader
{
    public:
        spell_putricide_mutated_transformation() : SpellScriptLoader("spell_putricide_mutated_transformation") { }

        class spell_putricide_mutated_transformation_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_mutated_transformation_SpellScript);

            void HandleSummon(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Unit* caster = GetOriginalCaster();
                if (!caster)
                    return;

                InstanceScript* instance = caster->GetInstanceScript();
                if (!instance)
                    return;

                Creature* putricide = ObjectAccessor::GetCreature(*caster, instance->GetData64(DATA_PROFESSOR_PUTRICIDE));
                if (!putricide)
                    return;

                if (putricide->AI()->GetData(DATA_ABOMINATION))
                {
                    if (Player* player = caster->ToPlayer())
                        Spell::SendCastResult(player, GetSpellInfo(), 0, SPELL_FAILED_CUSTOM_ERROR, SPELL_CUSTOM_ERROR_TOO_MANY_ABOMINATIONS);
                    return;
                }

                uint32 entry = uint32(GetSpellInfo()->Effects[effIndex].MiscValue);
                SummonPropertiesEntry const* properties = sSummonPropertiesStore.LookupEntry(uint32(GetSpellInfo()->Effects[effIndex].MiscValueB));
                uint32 duration = uint32(GetSpellInfo()->GetDuration());

                Position pos;
                caster->GetPosition(&pos);
                TempSummon* summon = caster->GetMap()->SummonCreature(entry, pos, properties, duration, caster, GetSpellInfo()->Id);
                if (!summon || !summon->IsVehicle())
                    return;

                summon->CastSpell(summon, SPELL_ABOMINATION_VEHICLE_POWER_DRAIN, true);
                summon->CastSpell(summon, SPELL_MUTATED_TRANSFORMATION_DAMAGE, true);
                caster->CastSpell(summon, SPELL_MUTATED_TRANSFORMATION_NAME, true);

                caster->EnterVehicle(summon, 0);    // VEHICLE_SPELL_RIDE_HARDCODED is used according to sniff, this is ok
                summon->SetCreatorGUID(caster->GetGUID());
                putricide->AI()->JustSummoned(summon);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_putricide_mutated_transformation_SpellScript::HandleSummon, EFFECT_0, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_putricide_mutated_transformation_SpellScript();
        }
};

class spell_putricide_mutated_transformation_dmg : public SpellScriptLoader
{
    public:
        spell_putricide_mutated_transformation_dmg() : SpellScriptLoader("spell_putricide_mutated_transformation_dmg") { }

        class spell_putricide_mutated_transformation_dmg_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_mutated_transformation_dmg_SpellScript);

            void FilterTargetsInitial(std::list<Unit*>& unitList)
            {
                if (Unit* owner = ObjectAccessor::GetUnit(*GetCaster(), GetCaster()->GetCreatorGUID()))
                    unitList.remove(owner);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_putricide_mutated_transformation_dmg_SpellScript::FilterTargetsInitial, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_putricide_mutated_transformation_dmg_SpellScript();
        }
};

class spell_putricide_regurgitated_ooze : public SpellScriptLoader
{
    public:
        spell_putricide_regurgitated_ooze() : SpellScriptLoader("spell_putricide_regurgitated_ooze") { }

        class spell_putricide_regurgitated_ooze_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_regurgitated_ooze_SpellScript);

            // the only purpose of this hook is to fail the achievement
            void ExtraEffect(SpellEffIndex /*effIndex*/)
            {
                if (InstanceScript* instance = GetCaster()->GetInstanceScript())
                    instance->SetData(DATA_NAUSEA_ACHIEVEMENT, uint32(false));
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_putricide_regurgitated_ooze_SpellScript::ExtraEffect, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_putricide_regurgitated_ooze_SpellScript();
        }
};

// Removes aura with id stored in effect value
class spell_putricide_clear_aura_effect_value : public SpellScriptLoader
{
    public:
        spell_putricide_clear_aura_effect_value() : SpellScriptLoader("spell_putricide_clear_aura_effect_value") { }

        class spell_putricide_clear_aura_effect_value_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_clear_aura_effect_value_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                uint32 auraId = sSpellMgr->GetSpellIdForDifficulty(uint32(GetEffectValue()), GetCaster());
                GetHitUnit()->RemoveAurasDueToSpell(auraId);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_putricide_clear_aura_effect_value_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_putricide_clear_aura_effect_value_SpellScript();
        }
};

// Stinky and Precious spell, it's here because its used for both (Festergut and Rotface "pets")
class spell_stinky_precious_decimate : public SpellScriptLoader
{
    public:
        spell_stinky_precious_decimate() : SpellScriptLoader("spell_stinky_precious_decimate") { }

        class spell_stinky_precious_decimate_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stinky_precious_decimate_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (GetHitUnit()->GetHealthPct() > float(GetEffectValue()))
                {
                    uint32 newHealth = GetHitUnit()->GetMaxHealth() * uint32(GetEffectValue()) / 100;
                    GetHitUnit()->SetHealth(newHealth);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_stinky_precious_decimate_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_stinky_precious_decimate_SpellScript();
        }
};

class spell_putricide_choking_gas_bomb_effect : public SpellScriptLoader
{
    public:
        spell_putricide_choking_gas_bomb_effect() : SpellScriptLoader("spell_putricide_choking_gas_bomb_effect") { }

        class spell_putricide_choking_gas_bomb_effect_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_choking_gas_bomb_effect_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if(Trinity::UnitAuraCheck(true, VEHICLE_SPELL_RIDE_HARDCODED));
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_putricide_choking_gas_bomb_effect_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_putricide_choking_gas_bomb_effect_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_putricide_choking_gas_bomb_effect_SpellScript();
        }
};

class spell_putricide_mutated_plague_effect : public SpellScriptLoader
{
    public:
        spell_putricide_mutated_plague_effect() : SpellScriptLoader("spell_putricide_mutated_plague_effect") { }

        class spell_putricide_mutated_plague_effect_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_putricide_mutated_plague_effect_SpellScript);

			void FilterTargets(std::list<Unit*>& unitList)
            {
                if (GetCaster())
                {
                    if (GetCaster()->GetMap())
                    {
                        if (GetCaster()->GetMap()->IsDungeon())
                        {
                            Map::PlayerList const& players = GetCaster()->GetMap()->GetPlayers();

                            if (!players.isEmpty())
                            {
                                unitList.clear();

                                for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                                {
                                    Player* player = itr->getSource();
                                    if (player && !player->IsGameMaster() && player->IsAlive())
                                    {
                                        unitList.push_back(player);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_putricide_mutated_plague_effect_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_putricide_mutated_plague_effect_SpellScript();
        }
};

class spell_shadow_infusion : public SpellScriptLoader
{
public:
	spell_shadow_infusion() : SpellScriptLoader("spell_shadow_infusion") { }

	class spell_shadow_infusion_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_shadow_infusion_SpellScript);               

		void HandleAfterCast()
		{
			if (Unit* vehicle = GetCaster())
			{
				if (Unit* player = vehicle->GetVehicleKit()->GetPassenger(0))
					player->AddAura(SPELL_SHADOW_INFUSION, player);
			}
		}

		void Register()
		{
			AfterCast += SpellCastFn(spell_shadow_infusion_SpellScript::HandleAfterCast);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_shadow_infusion_SpellScript();
	}
};

void AddSC_boss_professor_putricide()
{
    new boss_professor_putricide();
    new npc_volatile_ooze();
    new npc_gas_cloud();
    new spell_putricide_gaseous_bloat();
    new spell_putricide_ooze_channel();
    new spell_putricide_slime_puddle();
    new spell_putricide_slime_puddle_aura();
    new spell_putricide_unstable_experiment();
    new spell_putricide_ooze_eruption_searcher();
    new spell_putricide_choking_gas_bomb();
    new spell_putricide_unbound_plague();
    new spell_putricide_eat_ooze();
    new spell_putricide_mutated_plague();
    new spell_putricide_mutation_init();
    new spell_putricide_mutated_transformation_dismiss();
    new spell_putricide_mutated_transformation();
    new spell_putricide_mutated_transformation_dmg();
    new spell_putricide_regurgitated_ooze();
    new spell_putricide_clear_aura_effect_value();
    new spell_stinky_precious_decimate();
	new spell_putricide_choking_gas_bomb_effect();
	new spell_putricide_mutated_plague_effect();
	new spell_shadow_infusion();
}