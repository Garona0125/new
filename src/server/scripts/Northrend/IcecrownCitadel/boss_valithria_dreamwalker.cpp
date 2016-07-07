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
#include "Cell.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "icecrown_citadel.h"

enum Texts
{
    SAY_LICH_KING_INTRO         = -1659158,
    SAY_VALITHRIA_ENTER_COMBAT  = -1659159,
    SAY_VALITHRIA_DREAM_PORTAL  = -1659160,
    SAY_VALITHRIA_75_PERCENT    = -1659161,
    SAY_VALITHRIA_25_PERCENT    = -1659162,
    SAY_VALITHRIA_DEATH         = -1659163,
    SAY_VALITHRIA_PLAYER_DEATH  = -1659164,
    SAY_VALITHRIA_BERSERK       = -1659165,
    SAY_VALITHRIA_SUCCESS       = -1659166,
};

enum Spells
{
    // Valithria Dreamwalker
	SPELL_WILD_GROWTH                   = 52949,
    SPELL_COPY_DAMAGE                   = 71948,
    SPELL_DREAM_PORTAL_VISUAL_PRE       = 71304,
    SPELL_NIGHTMARE_PORTAL_VISUAL_PRE   = 71986,
	SPELL_NIGHTMARE_PORTAL_VISUAL       = 71994,
    SPELL_NIGHTMARE_CLOUD               = 71970,
    SPELL_NIGHTMARE_CLOUD_VISUAL        = 71939,
    SPELL_PRE_SUMMON_DREAM_PORTAL       = 72224,
    SPELL_PRE_SUMMON_NIGHTMARE_PORTAL   = 72480,
    SPELL_SUMMON_DREAM_PORTAL           = 71305,
    SPELL_SUMMON_NIGHTMARE_PORTAL       = 71987,
    SPELL_DREAMWALKERS_RAGE             = 71189,
    SPELL_DREAM_SLIP                    = 71196,
    SPELL_ACHIEVEMENT_CHECK             = 72706,
    SPELL_CLEAR_ALL                     = 71721,
    SPELL_AWARD_REPUTATION_BOSS_KILL    = 73843,
    SPELL_CORRUPTION_VALITHRIA          = 70904,
	SPELL_VALITHRIA_IMMUNITIES          = 72724,
	SPELL_DREAM_CLOUD                   = 70876,
	SPELL_SUMMON_DREAM_PORTAL_1         = 71301,
	SPELL_SUMMON_DREAM_PORTAL_2         = 72220,
	SPELL_SUMMON_DREAM_PORTAL_3         = 72223,
	SPELL_SUMMON_DREAM_PORTAL_4         = 72225,
	SPELL_EMERALD_VIGOR                 = 70873,
    SPELL_TWISTED_NIGHTMARE             = 71941,
    // The Lich King
    SPELL_TIMER_GLUTTONOUS_ABOMINATION  = 70915,
    SPELL_TIMER_SUPPRESSER              = 70912,
    SPELL_TIMER_BLISTERING_ZOMBIE       = 70914,
    SPELL_TIMER_RISEN_ARCHMAGE          = 70916,
    SPELL_TIMER_BLAZING_SKELETON        = 70913,
    SPELL_SUMMON_SUPPRESSER             = 70936,
    SPELL_RECENTLY_SPAWNED              = 72954,
    SPELL_SPAWN_CHEST_10N               = 71207,
	SPELL_SPAWN_CHEST_25N               = 72910,
	SPELL_SPAWN_CHEST_10H               = 72911,
	SPELL_SPAWN_CHEST_25H               = 72912,
    SPELL_CORRUPTION                    = 70602,
	// Risen Archmage
    SPELL_FROSTBOLT_VOLLEY_10N          = 70759,
	SPELL_FROSTBOLT_VOLLEY_25N          = 71889,
	SPELL_FROSTBOLT_VOLLEY_10H          = 72015,
	SPELL_FROSTBOLT_VOLLEY_25H          = 72016,
    SPELL_MANA_VOID_10                  = 71179,
	SPELL_MANA_VOID_25                  = 71741,
    SPELL_COLUMN_OF_FROST_SUMMON        = 70704,
    SPELL_COLUMN_OF_FROST_DAMAGE_10N    = 70702,
	SPELL_COLUMN_OF_FROST_DAMAGE_25N    = 71746,
	SPELL_COLUMN_OF_FROST_DAMAGE_10H    = 72019,
	SPELL_COLUMN_OF_FROST_DAMAGE_25H    = 72020,
	// Blazing Skeleton
    SPELL_FIREBALL_10N                  = 70754,
	SPELL_FIREBALL_25N                  = 71748,
	SPELL_FIREBALL_10H                  = 72023,
	SPELL_FIREBALL_25H                  = 72024,
    SPELL_LAY_WASTE_10                  = 69325,
	SPELL_LAY_WASTE_25                  = 71730,
	// Supresser
    SPELL_SUPPRESSION                   = 70588,
	// Blistering Zombie
    SPELL_ACID_BURST_10N                = 70744,
	SPELL_ACID_BURST_25N                = 71733,
	SPELL_ACID_BURST_10H                = 72017,
	SPELL_ACID_BURST_25H                = 72018,
	SPELL_CORROSION_10N                 = 70751,
	SPELL_CORROSION_25N                 = 71738,
	SPELL_CORROSION_10H                 = 72021,
	SPELL_CORROSION_25H                 = 72022,
	// Gluttonous Abomination
    SPELL_GUT_SPRAY_10N                 = 70633,
	SPELL_GUT_SPRAY_25N                 = 71283,
	SPELL_GUT_SPRAY_10H                 = 72025,
	SPELL_GUT_SPRAY_25H                 = 72026,
    SPELL_ROT_WORM_SPAWNER              = 70675,
	// Rot Worm
	SPELL_ROT_WORM_SPAWN                = 70668,
	SPELL_FLESH_ROT_10N                 = 72963,
	SPELL_FLESH_ROT_25N                 = 72964,
	SPELL_FLESH_ROT_10H                 = 72965,
	SPELL_FLESH_ROT_25H                 = 72966,
};

#define SUMMON_PORTAL RAID_MODE<uint32>(SPELL_PRE_SUMMON_DREAM_PORTAL, SPELL_PRE_SUMMON_DREAM_PORTAL, \
                                        SPELL_PRE_SUMMON_NIGHTMARE_PORTAL, SPELL_PRE_SUMMON_NIGHTMARE_PORTAL)

#define EMERALD_VIGOR RAID_MODE<uint32>(SPELL_EMERALD_VIGOR, SPELL_EMERALD_VIGOR, \
                                        SPELL_TWISTED_NIGHTMARE, SPELL_TWISTED_NIGHTMARE)

enum Events
{
    EVENT_INTRO_TALK                        = 1,
    EVENT_BERSERK                           = 2,
    EVENT_DREAM_PORTAL                      = 3,
    EVENT_DREAM_SLIP                        = 4,
    EVENT_GLUTTONOUS_ABOMINATION_SUMMONER   = 5,
    EVENT_SUPPRESSER_SUMMONER               = 6,
    EVENT_BLISTERING_ZOMBIE_SUMMONER        = 7,
    EVENT_RISEN_ARCHMAGE_SUMMONER           = 8,
    EVENT_BLAZING_SKELETON_SUMMONER         = 9,
    EVENT_FROSTBOLT_VOLLEY                  = 10,
    EVENT_MANA_VOID                         = 11,
    EVENT_COLUMN_OF_FROST                   = 12,
    EVENT_FIREBALL                          = 13,
    EVENT_LAY_WASTE                         = 14,
    EVENT_SUPPRESSION                       = 15,
    EVENT_GUT_SPRAY                         = 16,
    EVENT_CHECK_PLAYER                      = 17,
    EVENT_EXPLODE                           = 18,
};

enum Actions
{
    ACTION_ENTER_COMBAT = 1,
    MISSED_PORTALS      = 2,
    ACTION_DEATH        = 3,
};

Position const ValithriaSpawnPos = {4210.813f, 2484.443f, 364.9558f, 0.01745329f};

struct ManaVoidSelector : public std::unary_function<Unit*, bool>
{
	explicit ManaVoidSelector(WorldObject const* source) : _source(source) { }

	bool operator()(Unit* unit) const
	{
		return unit->GetPowerType() == POWER_MANA && _source->GetDistance(unit) > 15.0f;
	}

	WorldObject const* _source;
};

class DelayedCastEvent : public BasicEvent
{
    public:
        DelayedCastEvent(Creature* trigger, uint32 spellId, uint64 originalCaster, uint32 despawnTime) : _trigger(trigger), _originalCaster(originalCaster), _spellId(spellId), _despawnTime(despawnTime){}

        bool Execute(uint64 /*time*/, uint32 /*diff*/)
        {
            _trigger->CastSpell(_trigger, _spellId, false, NULL, NULL, _originalCaster);
            if (_despawnTime)
                _trigger->DespawnAfterAction(_despawnTime);
            return true;
        }

    private:
        Creature* _trigger;
        uint64 _originalCaster;
        uint32 _spellId;
        uint32 _despawnTime;
};

class AuraRemoveEvent : public BasicEvent
{
    public:
        AuraRemoveEvent(Creature* trigger, uint32 spellId) : _trigger(trigger), _spellId(spellId){}

        bool Execute(uint64 /*time*/, uint32 /*diff*/)
        {
            _trigger->RemoveAurasDueToSpell(_spellId);
            return true;
        }

    private:
        Creature* _trigger;
        uint32 _spellId;
};

class ValithriaDespawner : public BasicEvent
{
    public:
        explicit ValithriaDespawner(Creature* creature) : _creature(creature){}

        bool Execute(uint64 /*currTime*/, uint32 /*diff*/)
        {
            Trinity::CreatureWorker<ValithriaDespawner> worker(_creature, *this);
            _creature->VisitNearbyGridObject(333.0f, worker);
            return true;
        }

        void operator()(Creature* creature) const
        {
            switch (creature->GetEntry())
            {
                case NPC_VALITHRIA_DREAMWALKER:
                    if (InstanceScript* instance = creature->GetInstanceScript())
					{
						instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, creature);
						
						if (instance->GetBossState(DATA_VALITHRIA_DREAMWALKER) == NOT_STARTED)
						{
							if (!creature->IsAlive())
								creature->Respawn();
						}
					}
					break;
                case NPC_BLAZING_SKELETON:
                case NPC_SUPPRESSER:
                case NPC_BLISTERING_ZOMBIE:
                case NPC_GLUTTONOUS_ABOMINATION:
                case NPC_MANA_VOID:
                case NPC_COLUMN_OF_FROST:
                case NPC_ROT_WORM:
                    creature->DespawnAfterAction();
                    return;
                case NPC_RISEN_ARCHMAGE:
                    if (!creature->GetDBTableGUIDLow())
                    {
                        creature->DespawnAfterAction();
                        return;
                    }
					creature->Relocate(creature->GetHomePosition());
                    creature->Respawn(true);
                    break;
                default:
                    return;
            }

            uint32 corpseDelay = creature->GetCorpseDelay();
            uint32 respawnDelay = creature->GetRespawnDelay();
            creature->SetCorpseDelay(1);
            creature->SetRespawnDelay(10);

            if (CreatureData const* data = creature->GetCreatureData())
                creature->SetPosition(data->posX, data->posY, data->posZ, data->orientation);

            creature->DespawnAfterAction();
            creature->SetCorpseDelay(corpseDelay);
            creature->SetRespawnDelay(respawnDelay);
        }

    private:
        Creature* _creature;
};

class boss_valithria_dreamwalker : public CreatureScript
{
    public:
        boss_valithria_dreamwalker() : CreatureScript("boss_valithria_dreamwalker") { }

        struct boss_valithria_dreamwalkerAI : public QuantumBasicAI
        {
            boss_valithria_dreamwalkerAI(Creature* creature) : QuantumBasicAI(creature), _portalCount(RAID_MODE<uint32>(3, 8, 3, 8))
			{
				instance = creature->GetInstanceScript();
			}

			EventMap _events;
            InstanceScript* instance;

            uint32 _spawnHealth;
            uint32 const _portalCount;
            uint32 MissedPortals;

            bool Under25PercentTalkDone;
            bool Over75PercentTalkDone;
            bool _justDied;
            bool _done;

            void InitializeAI()
            {
                if (CreatureData const* data = sObjectMgr->GetCreatureData(me->GetDBTableGUIDLow()))
				{
                    if (data->curhealth)
                        _spawnHealth = data->curhealth;
				}

                if (!me->IsDead())
                    Reset();
            }

            void Reset()
            {
				_events.Reset();

				DoCast(me, SPELL_WILD_GROWTH, true);
				DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
				DoCast(me, SPELL_CORRUPTION_VALITHRIA, true);
				DoCast(me, SPELL_VALITHRIA_IMMUNITIES, true);

                me->SetHealth(_spawnHealth);
                me->SetReactState(REACT_PASSIVE);
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_OBS_MOD_HEALTH, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL_PCT, true);
                me->ApplySpellImmune(0, IMMUNITY_ID, 56131, true);

				instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                MissedPortals = 0;
                Under25PercentTalkDone = false;
                Over75PercentTalkDone = false;
                _justDied = false;
                _done = false;
            }

            void AttackStart(Unit* /*target*/) {}

            void DoAction(int32 const action)
            {
                if (action != ACTION_ENTER_COMBAT)
                    return;

                DoCast(me, SPELL_COPY_DAMAGE);

				instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);

				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                _events.ScheduleEvent(EVENT_INTRO_TALK, 15000);
                _events.ScheduleEvent(EVENT_DREAM_PORTAL, urand(45000, 48000));

                if (IsHeroic())
                    _events.ScheduleEvent(EVENT_BERSERK, 420000);
            }

            void HealReceived(Unit* /*healer*/, uint32& heal)
            {
				int32 reductionPercent = me->GetTotalAuraModifier(SPELL_AURA_MOD_HEALING_PCT);

				// Enter Combat Risen Archmage after heal Valithria
				std::list<Creature*> ArchmageList;
				me->GetCreatureListWithEntryInGrid(ArchmageList, NPC_RISEN_ARCHMAGE, 250.0f);

				if (!ArchmageList.empty())
				{
					for (std::list<Creature*>::const_iterator itr = ArchmageList.begin(); itr != ArchmageList.end(); ++itr)
					{
						if (Creature* archmage = *itr)
						{
							if (archmage->IsAlive() && !archmage->IsInCombatActive())
								archmage->AI()->DoZoneInCombat();
						}
					}
				}

				// Enter Combat Lich King trigger after heal Valithria
				if (Creature* lichKing = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_VALITHRIA_LICH_KING)))
				{
					if (!lichKing->IsInCombatActive())
						lichKing->AI()->DoZoneInCombat();
				}

				// Enter Combat Valithria trigger after heal Valithria
				if (Creature* trigger = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_VALITHRIA_TRIGGER)))
				{
					if (!trigger->IsInCombatActive())
						trigger->AI()->DoZoneInCombat();
				}

				if (reductionPercent < 0)
				{
					// One 10% reduction is handled by core
					int32 originalHeal = (int32)10/9 * heal;
					int32 healValueReduced = AddPctN(originalHeal, reductionPercent);

					if (healValueReduced <= 0)
						heal = 0;
					else
						heal = healValueReduced;
				}

                // encounter complete
                if (me->HealthAbovePctHealed(100, heal) && !_done)
                {
                    _done = true;
                    DoSendQuantumText(SAY_VALITHRIA_SUCCESS, me);

					instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

                    me->RemoveAurasDueToSpell(SPELL_CORRUPTION_VALITHRIA);
                    DoCast(me, SPELL_ACHIEVEMENT_CHECK, true);
                    DoCastAOE(SPELL_DREAMWALKERS_RAGE);
                    _events.ScheduleEvent(EVENT_DREAM_SLIP, 3500);

					if (Creature* lichKing = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_VALITHRIA_LICH_KING)))
						lichKing->Kill(lichKing);
                }
                else if (!Over75PercentTalkDone && me->HealthAbovePctHealed(HEALTH_PERCENT_75, heal))
                {
                    Over75PercentTalkDone = true;
                    DoSendQuantumText(SAY_VALITHRIA_75_PERCENT, me);
                }
			}

            void DamageTaken(Unit* /*attacker*/, uint32& damage)
            {
                if (me->HealthBelowPctDamaged(HEALTH_PERCENT_25, damage))
                {
                    if (!Under25PercentTalkDone)
                    {
                        Under25PercentTalkDone = true;
                        DoSendQuantumText(SAY_VALITHRIA_25_PERCENT, me);
                    }

                    if (damage >= me->GetHealth())
                    {
                        damage = 0;
                        if (!_justDied)
                        {
                            _justDied = true;
                            DoSendQuantumText(SAY_VALITHRIA_DEATH, me);

							instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

							if (Creature* trigger = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_VALITHRIA_TRIGGER)))
								trigger->AI()->DoAction(ACTION_DEATH);
                        }
                    }
                }
            }

			void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_DREAM_SLIP)
                {
                    DoCast(me, SPELL_CLEAR_ALL, true);
                    DoCast(me, SPELL_AWARD_REPUTATION_BOSS_KILL, true);
                    // this display id was found in sniff instead of the one on aura
                    me->SetDisplayId(MODEL_ID_INVISIBLE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->DespawnAfterAction(4*IN_MILLISECONDS);

					if (Creature* trigger = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_WORLD_TRIGGER)))
					{
						trigger->SetCurrentFaction(35);
						trigger->CastSpell(trigger, RAID_MODE(SPELL_SPAWN_CHEST_10N, SPELL_SPAWN_CHEST_25N, SPELL_SPAWN_CHEST_10H, SPELL_SPAWN_CHEST_25H), true);
					}

                    if (Creature* trigger = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_VALITHRIA_TRIGGER)))
                        me->Kill(trigger);
                }
            }

            void JustSummoned(Creature* summon)
            {
                if (summon->GetEntry() == NPC_DREAM_PORTAL_PRE_EFFECT)
                {
                    summon->m_Events.AddEvent(new DelayedCastEvent(summon, SPELL_SUMMON_DREAM_PORTAL, me->GetGUID(), 6000), summon->m_Events.CalculateTime(15000));
                    summon->m_Events.AddEvent(new AuraRemoveEvent(summon, SPELL_DREAM_PORTAL_VISUAL_PRE), summon->m_Events.CalculateTime(15000));
                }
                else if (summon->GetEntry() == NPC_NIGHTMARE_PORTAL_PRE_EFFECT)
                {
                    summon->m_Events.AddEvent(new DelayedCastEvent(summon, SPELL_SUMMON_NIGHTMARE_PORTAL, me->GetGUID(), 6000), summon->m_Events.CalculateTime(15000));
                    summon->m_Events.AddEvent(new AuraRemoveEvent(summon, SPELL_NIGHTMARE_PORTAL_VISUAL_PRE), summon->m_Events.CalculateTime(15000));
                }
            }

            void SummonedCreatureDespawn(Creature* summon)
            {
                if (summon->GetEntry() == NPC_DREAM_PORTAL || summon->GetEntry() == NPC_NIGHTMARE_PORTAL)
				{
                    if (summon->AI()->GetData(MISSED_PORTALS))
                        ++MissedPortals;
				}
            }

            void UpdateAI(uint32 const diff)
            {
				if (instance->GetBossState(DATA_VALITHRIA_DREAMWALKER) != IN_PROGRESS)
					return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INTRO_TALK:
                            DoSendQuantumText(SAY_VALITHRIA_ENTER_COMBAT, me);
                            break;
                        case EVENT_BERSERK:
                            DoSendQuantumText(SAY_VALITHRIA_BERSERK, me);
                            break;
                        case EVENT_DREAM_PORTAL:
                            if (!IsHeroic())
                                DoSendQuantumText(SAY_VALITHRIA_DREAM_PORTAL, me);
                            for (uint32 i = 0; i < _portalCount; ++i)
                                DoCast(me, SUMMON_PORTAL);
                            _events.ScheduleEvent(EVENT_DREAM_PORTAL, urand(45000, 48000));
                            break;
                        case EVENT_DREAM_SLIP:
                            DoCast(me, SPELL_DREAM_SLIP);
                            break;
                        default:
                            break;
                    }
                }
            }

            uint32 GetData(uint32 type)
            {
                if (type == MISSED_PORTALS)
                    return MissedPortals;

                return 0;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<boss_valithria_dreamwalkerAI>(creature);
        }
};

class npc_green_dragon_combat_trigger : public CreatureScript
{
    public:
        npc_green_dragon_combat_trigger() : CreatureScript("npc_green_dragon_combat_trigger") { }

        struct npc_green_dragon_combat_triggerAI : public BossAI
        {
            npc_green_dragon_combat_triggerAI(Creature* creature) : BossAI(creature, DATA_VALITHRIA_DREAMWALKER) {}

			bool _evadeCheck;

            void Reset()
            {
                _Reset();
                me->SetReactState(REACT_PASSIVE);
            }

            void EnterToBattle(Unit* who)
            {
                if (!instance->CheckRequiredBosses(DATA_VALITHRIA_DREAMWALKER, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    instance->DoCastSpellOnPlayers(SPELL_LIGHTS_HAMMER_TELEPORT);
                    return;
                }

                me->SetActive(true);

                DoZoneInCombat();

                instance->SetBossState(DATA_VALITHRIA_DREAMWALKER, IN_PROGRESS);

                if (Creature* valithria = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_VALITHRIA_DREAMWALKER)))
                    valithria->AI()->DoAction(ACTION_ENTER_COMBAT);
            }

            void AttackStart(Unit* target)
            {
                if (target->GetTypeId() == TYPE_ID_PLAYER)
                    BossAI::AttackStart(target);
            }

            bool CanAIAttack(Unit const* target) const
            {
                return target->GetTypeId() == TYPE_ID_PLAYER;
            }

            void JustReachedHome()
            {
                _JustReachedHome();
                DoAction(ACTION_DEATH);
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_DEATH)
                {
					if (Creature* lichKing = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_VALITHRIA_LICH_KING)))
						lichKing->AI()->EnterEvadeMode();

					instance->SetBossState(DATA_VALITHRIA_DREAMWALKER, NOT_STARTED);
					me->m_Events.AddEvent(new ValithriaDespawner(me), me->m_Events.CalculateTime(5000));
                }
            }

            void UpdateAI(uint32 const /*diff*/)
            {
                if (!me->IsInCombatActive())
                    return;

                std::list<HostileReference*> const& threatList = me->getThreatManager().getThreatList();
                if (threatList.empty())
                {
                    EnterEvadeMode();
                    return;
                }

                // check evade every second tick
                _evadeCheck ^= true;
                if (!_evadeCheck)
                    return;

                // check if there is any player on threatlist, if not - evade
                for (std::list<HostileReference*>::const_iterator itr = threatList.begin(); itr != threatList.end(); ++itr)
				{
                    if (Unit* target = (*itr)->getTarget())
					{
                        if (target->GetTypeId() == TYPE_ID_PLAYER)
						{
                            if (target->GetInstanceId() == me->GetInstanceId())
								return; // found any player in same instance, return
						}
					}
				}

                EnterEvadeMode();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_green_dragon_combat_triggerAI>(creature);
        }
};

class npc_the_lich_king_controller : public CreatureScript
{
    public:
        npc_the_lich_king_controller() : CreatureScript("npc_the_lich_king_controller") { }

        struct npc_the_lich_king_controllerAI : public QuantumBasicAI
        {
            npc_the_lich_king_controllerAI(Creature* creature) : QuantumBasicAI(creature)
			{
				instance = creature->GetInstanceScript();
			}

			EventMap events;
            InstanceScript* instance;

            void Reset()
            {
                events.Reset();
				events.ScheduleEvent(EVENT_GLUTTONOUS_ABOMINATION_SUMMONER, urand(5000, 10000));
				events.ScheduleEvent(EVENT_SUPPRESSER_SUMMONER, urand(10000, 15000));
				events.ScheduleEvent(EVENT_BLISTERING_ZOMBIE_SUMMONER, urand(10000, 15000));
				events.ScheduleEvent(EVENT_RISEN_ARCHMAGE_SUMMONER, urand(20000, 25000));
				events.ScheduleEvent(EVENT_BLAZING_SKELETON_SUMMONER, urand(25000, 30000));
                me->SetReactState(REACT_PASSIVE);
            }

            void JustReachedHome()
            {
                me->SetActive(false);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                DoSendQuantumText(SAY_LICH_KING_INTRO, me);
                me->SetActive(true);
            }

            void JustSummoned(Creature* summon)
            {
                summon->SetPhaseMask((summon->GetPhaseMask() & ~0x10), true);
                if (summon->GetEntry() != NPC_SUPPRESSER)
				{
					if (Creature* valithria = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_VALITHRIA_DREAMWALKER)))
					{
						if (me->AI())
							me->AI()->AttackStart(valithria);
					}

					DoZoneInCombat(summon, 150.0f);
				}
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_GLUTTONOUS_ABOMINATION_SUMMONER:
                            DoCast(me, SPELL_TIMER_GLUTTONOUS_ABOMINATION);
                            break;
                        case EVENT_SUPPRESSER_SUMMONER:
                            DoCast(me, SPELL_TIMER_SUPPRESSER);
                            break;
                        case EVENT_BLISTERING_ZOMBIE_SUMMONER:
                            DoCast(me, SPELL_TIMER_BLISTERING_ZOMBIE);
                            break;
                        case EVENT_RISEN_ARCHMAGE_SUMMONER:
                            DoCast(me, SPELL_TIMER_RISEN_ARCHMAGE);
                            break;
                        case EVENT_BLAZING_SKELETON_SUMMONER:
                            DoCast(me, SPELL_TIMER_BLAZING_SKELETON);
                            break;
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_the_lich_king_controllerAI(creature);
        }
};

class npc_risen_archmage : public CreatureScript
{
    public:
        npc_risen_archmage() : CreatureScript("npc_risen_archmage") { }

        struct npc_risen_archmageAI : public QuantumBasicAI
        {
            npc_risen_archmageAI(Creature* creature) : QuantumBasicAI(creature)
			{
				instance = creature->GetInstanceScript();
			}

			EventMap events;
            InstanceScript* instance;

            bool _canCallEnterCombat;

            bool CanAIAttack(Unit const* target) const
            {
                return target->GetEntry() != NPC_VALITHRIA_DREAMWALKER;
            }

            void Reset()
            {
				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

				me->SetCorpseDelay(5);

                events.Reset();
                events.ScheduleEvent(EVENT_FROSTBOLT_VOLLEY, urand(5000, 15000));
                events.ScheduleEvent(EVENT_MANA_VOID, urand(20000, 25000));
                events.ScheduleEvent(EVENT_COLUMN_OF_FROST, urand(10000, 20000));
                _canCallEnterCombat = true;
            }

            void EnterToBattle(Unit* /*who*/)
            {
				me->CallForHelp(150.f);

                me->FinishSpell(CURRENT_CHANNELED_SPELL, false);

                if (me->GetDBTableGUIDLow() && _canCallEnterCombat)
                {
					std::list<Creature*> ArchmageList;
					me->GetCreatureListWithEntryInGrid(ArchmageList, NPC_RISEN_ARCHMAGE, 250.0f);

					if (!ArchmageList.empty())
					{
						for (std::list<Creature*>::const_iterator itr = ArchmageList.begin(); itr != ArchmageList.end(); ++itr)
						{
							if (Creature* archmage = *itr)
								archmage->AI()->DoAction(ACTION_ENTER_COMBAT);
						}
					}

                    if (Creature* lichKing = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_VALITHRIA_LICH_KING)))
                        lichKing->AI()->DoZoneInCombat();

                    if (Creature* trigger = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_VALITHRIA_TRIGGER)))
                        trigger->AI()->DoZoneInCombat();
                }
            }

            void DoAction(int32 const action)
            {
                if (action != ACTION_ENTER_COMBAT)
                    return;

                _canCallEnterCombat = false;
                DoZoneInCombat();
                _canCallEnterCombat = true;
            }

            void JustSummoned(Creature* summon)
            {
                if (summon->GetEntry() == NPC_COLUMN_OF_FROST)
                    summon->m_Events.AddEvent(new DelayedCastEvent(summon, RAID_MODE(SPELL_COLUMN_OF_FROST_DAMAGE_10N, SPELL_COLUMN_OF_FROST_DAMAGE_25N, SPELL_COLUMN_OF_FROST_DAMAGE_10H, SPELL_COLUMN_OF_FROST_DAMAGE_25H), 0, 8000), summon->m_Events.CalculateTime(2000));
                else if (summon->GetEntry() == NPC_MANA_VOID)
                    summon->DespawnAfterAction(36*IN_MILLISECONDS);
            }

			void JustDied(Unit* killer)
			{
				// Quest credit
				if (Player* player = killer->ToPlayer())
				{
					player->CastSpell(player, SPELL_SOUL_FEAST, true);
					me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
				}
			}

            void UpdateAI(uint32 const diff)
            {
                if (!me->IsInCombatActive())
				{
                    if (me->GetDBTableGUIDLow())
					{
                        if (!me->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
                            DoCast(me, SPELL_CORRUPTION);
					}
				}

                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_FROSTBOLT_VOLLEY:
                            DoCastAOE(RAID_MODE(SPELL_FROSTBOLT_VOLLEY_10N, SPELL_FROSTBOLT_VOLLEY_25N, SPELL_FROSTBOLT_VOLLEY_10H, SPELL_FROSTBOLT_VOLLEY_25H));
                            events.ScheduleEvent(EVENT_FROSTBOLT_VOLLEY, urand(3000, 4000));
                            break;
                        case EVENT_MANA_VOID:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 1, ManaVoidSelector(me)))
							{
                                DoCast(target, RAID_MODE(SPELL_MANA_VOID_10, SPELL_MANA_VOID_25, SPELL_MANA_VOID_10, SPELL_MANA_VOID_25));
							}
                            events.ScheduleEvent(EVENT_MANA_VOID, urand(20000, 25000));
                            break;
                        case EVENT_COLUMN_OF_FROST:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 1, -10.0f, true))
							{
                                DoCast(target, SPELL_COLUMN_OF_FROST_SUMMON);
							}
                            events.ScheduleEvent(EVENT_COLUMN_OF_FROST, urand(15000, 25000));
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
            return GetIcecrownCitadelAI<npc_risen_archmageAI>(creature);
        }
};

class npc_blazing_skeleton : public CreatureScript
{
    public:
        npc_blazing_skeleton() : CreatureScript("npc_blazing_skeleton") { }

        struct npc_blazing_skeletonAI : public QuantumBasicAI
        {
            npc_blazing_skeletonAI(Creature* creature) : QuantumBasicAI(creature)
			{
				instance = creature->GetInstanceScript();
			}

			EventMap _events;
            InstanceScript* instance;

            void Reset()
            {
				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

				me->SetCorpseDelay(5);

                _events.Reset();
                _events.ScheduleEvent(EVENT_FIREBALL, urand(2000, 4000));
                _events.ScheduleEvent(EVENT_LAY_WASTE, urand(15000, 20000));
            }

            void IsSummonedBy(Unit* /*summoner*/)
            {
				if (Creature* valithria = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_VALITHRIA_DREAMWALKER)))
				{
					me->AddThreat(valithria, 35.0f);
					me->AI()->AttackStart(valithria);
				}
            }

			void JustDied(Unit* killer)
			{
				// Quest credit
				if (Player* player = killer->ToPlayer())
				{
					player->CastSpell(player, SPELL_SOUL_FEAST, true);
					me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
				}
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
                        case EVENT_FIREBALL:
							if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
								DoCast(target, RAID_MODE(SPELL_FIREBALL_10N, SPELL_FIREBALL_25N, SPELL_FIREBALL_10H, SPELL_FIREBALL_25H));

                            _events.ScheduleEvent(EVENT_FIREBALL, urand(3000, 4000));
                            break;
                        case EVENT_LAY_WASTE:
                            DoCast(me, RAID_MODE(SPELL_LAY_WASTE_10, SPELL_LAY_WASTE_25, SPELL_LAY_WASTE_10, SPELL_LAY_WASTE_25));
                            _events.ScheduleEvent(EVENT_LAY_WASTE, urand(15000, 20000));
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
            return GetIcecrownCitadelAI<npc_blazing_skeletonAI>(creature);
        }
};

class npc_suppresser : public CreatureScript
{
    public:
        npc_suppresser() : CreatureScript("npc_suppresser") { }

        struct npc_suppresserAI : public QuantumBasicAI
        {
            npc_suppresserAI(Creature* creature) : QuantumBasicAI(creature)
			{
				instance = creature->GetInstanceScript();
			}

			EventMap _events;
            InstanceScript* instance;

            void Reset()
            {
				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

				me->SetCorpseDelay(5);

                _events.Reset();
                _events.ScheduleEvent(EVENT_SUPPRESSION, urand(5000, 6000));
                me->SetReactState(REACT_PASSIVE);
            }

            void IsSummonedBy(Unit* /*summoner*/)
            {
                if (Creature* valithria = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_VALITHRIA_DREAMWALKER)))
				{
					me->AddThreat(valithria, 35.0f);
					me->AI()->AttackStart(valithria);
				}
            }

			void JustDied(Unit* killer)
			{
				// Quest credit
				if (Player* player = killer->ToPlayer())
				{
					player->CastSpell(player, SPELL_SOUL_FEAST, true);
					me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
				}
			}

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                _events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                // this code will never be reached while channeling
                while (uint32 eventId = _events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SUPPRESSION:
                            DoCastAOE(SPELL_SUPPRESSION);
                            _events.ScheduleEvent(EVENT_SUPPRESSION, 5000);
                            break;
                        default:
                            break;
                    }
                }

                // this creature has REACT_PASSIVE so it does not always have victim here
                if (Unit* victim = me->GetVictim())
				{
                    if (victim->GetEntry() != NPC_VALITHRIA_DREAMWALKER)
                        DoMeleeAttackIfReady();
				}
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_suppresserAI>(creature);
        }
};

class npc_blistering_zombie : public CreatureScript
{
    public:
        npc_blistering_zombie() : CreatureScript("npc_blistering_zombie") { }

        struct npc_blistering_zombieAI : public QuantumBasicAI
        {
			npc_blistering_zombieAI(Creature* creature) : QuantumBasicAI(creature)
			{
				instance = creature->GetInstanceScript();
			}

			InstanceScript* instance;

			uint32 CorrosionTimer;

			void IsSummonedBy(Unit* /*summoner*/)
            {
                if (Creature* valithria = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_VALITHRIA_DREAMWALKER)))
				{
					me->AddThreat(valithria, 35.0f);
					me->AI()->AttackStart(valithria);
				}
            }

			void Reset()
			{
				DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);

				CorrosionTimer = 1*IN_MILLISECONDS;

				me->SetCorpseDelay(5);
			}

            void JustDied(Unit* killer)
            {
				DoCastAOE(RAID_MODE(SPELL_ACID_BURST_10N, SPELL_ACID_BURST_25N, SPELL_ACID_BURST_10H, SPELL_ACID_BURST_25H), true);

				// Quest credit
				if (Player* player = killer->ToPlayer())
				{
					player->CastSpell(player, SPELL_SOUL_FEAST, true);
					me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
				}
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

				if (CorrosionTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, RAID_MODE(SPELL_CORROSION_10N, SPELL_CORROSION_25N, SPELL_CORROSION_10H, SPELL_CORROSION_25H));
						CorrosionTimer = 4*IN_MILLISECONDS;
					}
				}
				else CorrosionTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_blistering_zombieAI>(creature);
        }
};

class npc_gluttonous_abomination : public CreatureScript
{
    public:
        npc_gluttonous_abomination() : CreatureScript("npc_gluttonous_abomination") { }

        struct npc_gluttonous_abominationAI : public QuantumBasicAI
        {
            npc_gluttonous_abominationAI(Creature* creature) : QuantumBasicAI(creature)
			{
				instance = creature->GetInstanceScript();
			}

			EventMap _events;
            InstanceScript* instance;

            void Reset()
            {
				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

				me->SetCorpseDelay(5);

                _events.Reset();
                _events.ScheduleEvent(EVENT_GUT_SPRAY, 2*IN_MILLISECONDS);
            }

            void IsSummonedBy(Unit* /*summoner*/)
            {
                if (Creature* valithria = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_VALITHRIA_DREAMWALKER)))
				{
					me->AddThreat(valithria, 35.0f);
					me->AI()->AttackStart(valithria);
				}
            }

            void JustDied(Unit* killer)
            {
                DoCast(me, SPELL_ROT_WORM_SPAWNER, true);

				// Quest credit
				if (Player* player = killer->ToPlayer())
				{
					player->CastSpell(player, SPELL_SOUL_FEAST, true);
					me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
				}
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
                        case EVENT_GUT_SPRAY:
                            DoCast(RAID_MODE(SPELL_GUT_SPRAY_10N, SPELL_GUT_SPRAY_25N, SPELL_GUT_SPRAY_10H, SPELL_GUT_SPRAY_25H));
                            _events.ScheduleEvent(EVENT_GUT_SPRAY, 5*IN_MILLISECONDS);
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
            return GetIcecrownCitadelAI<npc_gluttonous_abominationAI>(creature);
        }
};

class npc_rot_worm : public CreatureScript
{
    public:
        npc_rot_worm() : CreatureScript("npc_rot_worm") { }

        struct npc_rot_wormAI : public QuantumBasicAI
        {
            npc_rot_wormAI(Creature* creature) : QuantumBasicAI(creature)
			{
				instance = creature->GetInstanceScript();
			}

            InstanceScript* instance;

			void Reset()
			{
				DoCast(me, SPELL_ROT_WORM_SPAWN, true);
				DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);

				me->SetCorpseDelay(5);
			}

            void IsSummonedBy(Unit* /*summoner*/)
            {
				if (Creature* valithria = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_VALITHRIA_DREAMWALKER)))
				{
					me->AddThreat(valithria, 35.0f);
					me->AI()->AttackStart(valithria);
				}
			}

            void UpdateAI(uint32 const /*diff*/)
            {
                if (!UpdateVictim())
                    return;

				// We need this in order to get one aura application on the target of different worms
				if (instance->GetData64(DATA_VALITHRIA_LICH_KING))
				{
					if (Unit* victim = me->GetVictim())
					{
						if (me->IsAttackReady() && me->IsWithinMeleeRange(victim))
							me->CastSpell(victim, RAID_MODE(SPELL_FLESH_ROT_10N, SPELL_FLESH_ROT_25N, SPELL_FLESH_ROT_10H, SPELL_FLESH_ROT_25H), true, 0, 0, instance->GetData64(DATA_VALITHRIA_LICH_KING));
					}
				}

				// Despawn if encounter is done
				if (instance->GetBossState(DATA_VALITHRIA_DREAMWALKER) != IN_PROGRESS)
					me->DespawnAfterAction();
				
				DoMeleeAttackIfReady();
			}
		};

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_rot_wormAI>(creature);
        }
};

class npc_dream_portal : public CreatureScript
{
    public:
        npc_dream_portal() : CreatureScript("npc_dream_portal") { }

        struct npc_dream_portalAI : public CreatureAI
        {
            npc_dream_portalAI(Creature* creature) : CreatureAI(creature), Used(false){}

			bool Used;

			void Reset()
			{
				DoCast(me, SPELL_NIGHTMARE_PORTAL_VISUAL);
			}

            void OnSpellClick(Unit* /*clicker*/)
            {
				Used = true;
				me->DespawnAfterAction();
            }

            uint32 GetData(uint32 type)
            {
                return (type == MISSED_PORTALS && Used) ? 0 : 1;
            }

            void UpdateAI(uint32 const /*diff*/)
            {
                UpdateVictim();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_dream_portalAI>(creature);
        }
};

class npc_dream_cloud : public CreatureScript
{
    public:
        npc_dream_cloud() : CreatureScript("npc_dream_cloud") { }

        struct npc_dream_cloudAI : public QuantumBasicAI
        {
            npc_dream_cloudAI(Creature* creature) : QuantumBasicAI(creature)
			{
				instance = creature->GetInstanceScript();
			}

			EventMap events;
            InstanceScript* instance;

            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_CHECK_PLAYER, 1*IN_MILLISECONDS);

				DoCast(me, SPELL_DREAM_CLOUD);

				me->SetCorpseDelay(0);

				me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

            void UpdateAI(uint32 const diff)
            {
				if (instance->GetBossState(DATA_VALITHRIA_DREAMWALKER) != IN_PROGRESS)
					return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_CHECK_PLAYER:
                        {
                            Player* player = NULL;
                            Trinity::AnyPlayerInObjectRangeCheck check(me, 5.0f);
                            Trinity::PlayerSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(me, player, check);
                            me->VisitNearbyWorldObject(7.5f, searcher);
                            events.ScheduleEvent(player ? EVENT_EXPLODE : EVENT_CHECK_PLAYER, 1*IN_MILLISECONDS);
                            break;
                        }
                        case EVENT_EXPLODE:
                            me->GetMotionMaster()->MoveIdle();
							me->CastSpell(me, EMERALD_VIGOR, false, NULL, NULL, instance->GetData64(DATA_VALITHRIA_LICH_KING));
                            me->DespawnAfterAction(0.1*IN_MILLISECONDS);
                            break;
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_dream_cloudAI>(creature);
        }
};

class spell_dreamwalker_mana_void : public SpellScriptLoader
{
    public:
        spell_dreamwalker_mana_void() : SpellScriptLoader("spell_dreamwalker_mana_void") { }

        class spell_dreamwalker_mana_void_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dreamwalker_mana_void_AuraScript);

            void PeriodicTick(AuraEffect const* aurEff)
            {
                if (aurEff->GetTickNumber() <= 5)
				{
                    if (!(aurEff->GetTickNumber() & 1))
                        PreventDefaultAction();
				}
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dreamwalker_mana_void_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dreamwalker_mana_void_AuraScript();
        }
};

class spell_dreamwalker_decay_periodic_timer : public SpellScriptLoader
{
    public:
        spell_dreamwalker_decay_periodic_timer() : SpellScriptLoader("spell_dreamwalker_decay_periodic_timer") { }

        class spell_dreamwalker_decay_periodic_timer_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dreamwalker_decay_periodic_timer_AuraScript);

            bool Load()
            {
				switch (GetId())
				{
					case SPELL_TIMER_GLUTTONOUS_ABOMINATION:
					case SPELL_TIMER_SUPPRESSER:
					case SPELL_TIMER_BLAZING_SKELETON:
						_decayRate = 4250;
						return true;
					case SPELL_TIMER_BLISTERING_ZOMBIE:
					case SPELL_TIMER_RISEN_ARCHMAGE:
						_decayRate = 1000;
						return true;
					default:
						return false;
				}

				return false;
            }

            void DecayPeriodicTimer(AuraEffect* aurEff)
            {
				int32 completeTimer = aurEff->GetAmplitude();
				int32 tickAmount = aurEff->GetTickNumber();
				int32 newTimer = completeTimer - (tickAmount * _decayRate);

				if (newTimer <= 5000)
					newTimer = urand(5000, 15000);

				aurEff->SetPeriodicTimer(newTimer);
            }

            void Register()
            {
                OnEffectUpdatePeriodic += AuraEffectUpdatePeriodicFn(spell_dreamwalker_decay_periodic_timer_AuraScript::DecayPeriodicTimer, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }

            int32 _decayRate;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dreamwalker_decay_periodic_timer_AuraScript();
        }
};

class spell_dreamwalker_summoner : public SpellScriptLoader
{
    public:
        spell_dreamwalker_summoner() : SpellScriptLoader("spell_dreamwalker_summoner") { }

        class spell_dreamwalker_summoner_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dreamwalker_summoner_SpellScript);

            bool Load()
            {
                if (!GetCaster()->GetInstanceScript())
                    return false;
                return true;
            }

            void FilterTargets(std::list<Unit*>& targets)
            {
                targets.remove_if (Trinity::UnitAuraCheck(true, SPELL_RECENTLY_SPAWNED));
                if (targets.empty())
                    return;

                Unit* target = Quantum::DataPackets::SelectRandomContainerElement(targets);
                targets.clear();
                targets.push_back(target);
            }

            void HandleForceCast(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (!GetHitUnit())
                    return;

                GetHitUnit()->CastSpell(GetCaster(), GetSpellInfo()->Effects[effIndex].TriggerSpell, true, NULL, NULL, GetCaster()->GetInstanceScript()->GetData64(DATA_VALITHRIA_LICH_KING));
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_dreamwalker_summoner_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
                OnEffectHitTarget += SpellEffectFn(spell_dreamwalker_summoner_SpellScript::HandleForceCast, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dreamwalker_summoner_SpellScript();
        }
};

class spell_dreamwalker_summon_suppresser : public SpellScriptLoader
{
    public:
        spell_dreamwalker_summon_suppresser() : SpellScriptLoader("spell_dreamwalker_summon_suppresser") { }

        class spell_dreamwalker_summon_suppresser_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dreamwalker_summon_suppresser_AuraScript);

            void PeriodicTick(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                std::list<Creature*> summoners;
                GetCreatureListWithEntryInGrid(summoners, caster, NPC_WORLD_TRIGGER, 150.0f);
                Quantum::DataPackets::RandomResizeList(summoners, 2);
                if (summoners.empty())
                    return;

                uint8 spawnCountFront = urand(0, 4);
				uint8 spawnCountBack = 4 - spawnCountFront;

				for (uint32 i = 0; i < spawnCountFront; ++i)
                    caster->CastSpell(summoners.front(), SPELL_SUMMON_SUPPRESSER, true);
                for (uint32 i = 0; i < spawnCountBack; ++i)
                    caster->CastSpell(summoners.back(), SPELL_SUMMON_SUPPRESSER, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dreamwalker_summon_suppresser_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dreamwalker_summon_suppresser_AuraScript();
        }
};

class spell_dreamwalker_summon_suppresser_effect : public SpellScriptLoader
{
    public:
        spell_dreamwalker_summon_suppresser_effect() : SpellScriptLoader("spell_dreamwalker_summon_suppresser_effect") { }

        class spell_dreamwalker_summon_suppresser_effect_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dreamwalker_summon_suppresser_effect_SpellScript);

            bool Load()
            {
                if (!GetCaster()->GetInstanceScript())
                    return false;
                return true;
            }

            void HandleForceCast(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (!GetHitUnit())
                    return;

                GetHitUnit()->CastSpell(GetCaster(), GetSpellInfo()->Effects[effIndex].TriggerSpell, true, NULL, NULL, GetCaster()->GetInstanceScript()->GetData64(DATA_VALITHRIA_LICH_KING));
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dreamwalker_summon_suppresser_effect_SpellScript::HandleForceCast, EFFECT_0, SPELL_EFFECT_FORCE_CAST);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dreamwalker_summon_suppresser_effect_SpellScript();
        }
};

class spell_dreamwalker_summon_dream_portal : public SpellScriptLoader
{
    public:
        spell_dreamwalker_summon_dream_portal() : SpellScriptLoader("spell_dreamwalker_summon_dream_portal") { }

        class spell_dreamwalker_summon_dream_portal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dreamwalker_summon_dream_portal_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (!GetHitUnit())
                    return;

                uint32 spellId = RAND<uint32>(SPELL_SUMMON_DREAM_PORTAL_1, SPELL_SUMMON_DREAM_PORTAL_2, SPELL_SUMMON_DREAM_PORTAL_3, SPELL_SUMMON_DREAM_PORTAL_4);
                GetHitUnit()->CastSpell(GetHitUnit(), spellId, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dreamwalker_summon_dream_portal_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dreamwalker_summon_dream_portal_SpellScript();
        }
};

class spell_dreamwalker_summon_nightmare_portal : public SpellScriptLoader
{
    public:
        spell_dreamwalker_summon_nightmare_portal() : SpellScriptLoader("spell_dreamwalker_summon_nightmare_portal") { }

        class spell_dreamwalker_summon_nightmare_portal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dreamwalker_summon_nightmare_portal_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (!GetHitUnit())
                    return;

                uint32 spellId = RAND<uint32>(71977, 72481, 72482, 72483);
                GetHitUnit()->CastSpell(GetHitUnit(), spellId, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dreamwalker_summon_nightmare_portal_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dreamwalker_summon_nightmare_portal_SpellScript();
        }
};

class spell_dreamwalker_nightmare_cloud : public SpellScriptLoader
{
    public:
        spell_dreamwalker_nightmare_cloud() : SpellScriptLoader("spell_dreamwalker_nightmare_cloud") { }

        class spell_dreamwalker_nightmare_cloud_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_dreamwalker_nightmare_cloud_AuraScript);

            bool Load()
            {
                instance = GetOwner()->GetInstanceScript();
                return instance != NULL;
            }

            void PeriodicTick(AuraEffect const* /*aurEff*/)
            {
                if (instance->GetBossState(DATA_VALITHRIA_DREAMWALKER) != IN_PROGRESS)
                    PreventDefaultAction();
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_dreamwalker_nightmare_cloud_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }

            InstanceScript* instance;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_dreamwalker_nightmare_cloud_AuraScript();
        }
};

class spell_dreamwalker_twisted_nightmares : public SpellScriptLoader
{
    public:
        spell_dreamwalker_twisted_nightmares() : SpellScriptLoader("spell_dreamwalker_twisted_nightmares") { }

        class spell_dreamwalker_twisted_nightmares_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_dreamwalker_twisted_nightmares_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                // impossible with TARGET_UNIT_CASTER
                //if (!GetHitUnit())
                //    return;

                if (InstanceScript* instance = GetHitUnit()->GetInstanceScript())
					GetHitUnit()->CastSpell((Unit*)NULL, GetSpellInfo()->Effects[effIndex].TriggerSpell, true, NULL, NULL, instance->GetData64(DATA_VALITHRIA_LICH_KING));
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_dreamwalker_twisted_nightmares_SpellScript::HandleScript, EFFECT_2, SPELL_EFFECT_FORCE_CAST);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_dreamwalker_twisted_nightmares_SpellScript();
        }
};

class achievement_portal_jockey : public AchievementCriteriaScript
{
public:
	achievement_portal_jockey() : AchievementCriteriaScript("achievement_portal_jockey") { }

	bool OnCheck(Player* /*source*/, Unit* target)
	{
		return target && !target->GetAI()->GetData(MISSED_PORTALS);
	}
};

void AddSC_boss_valithria_dreamwalker()
{
    new boss_valithria_dreamwalker();
    new npc_green_dragon_combat_trigger();
    new npc_the_lich_king_controller();
    new npc_risen_archmage();
    new npc_blazing_skeleton();
    new npc_suppresser();
    new npc_blistering_zombie();
    new npc_gluttonous_abomination();
	new npc_rot_worm();
    new npc_dream_portal();
    new npc_dream_cloud();
    new spell_dreamwalker_mana_void();
    new spell_dreamwalker_decay_periodic_timer();
    new spell_dreamwalker_summoner();
    new spell_dreamwalker_summon_suppresser();
    new spell_dreamwalker_summon_suppresser_effect();
    new spell_dreamwalker_summon_dream_portal();
    new spell_dreamwalker_summon_nightmare_portal();
    new spell_dreamwalker_nightmare_cloud();
    new spell_dreamwalker_twisted_nightmares();
    new achievement_portal_jockey();
}