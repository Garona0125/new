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
#include "pit_of_saron.h"

enum Spells
{
    SPELL_MIGHTY_KICK              = 69021,
    SPELL_SHADOW_BOLT_5N           = 69028,
	SPELL_SHADOW_BOLT_5H           = 70208,
    SPELL_TOXIC_WASTE_5N           = 69024,
	SPELL_TOXIC_WASTE_5H           = 70274,
    SPELL_EXPLOSIVE_BARRAGE_KRICK  = 69012,
    SPELL_EXPLOSIVE_BARRAGE_ICK    = 69263,
    SPELL_POISON_NOVA_5N           = 68989,
	SPELL_POISON_NOVA_5H           = 70434,
    SPELL_PURSUIT                  = 68987,
	SPELL_EXPLOSIVE_BARRAGE_SUMMON = 69015,
    SPELL_EXPLODING_ORB            = 69017,
    SPELL_AUTO_GROW                = 69020,
    SPELL_HASTY_GROW               = 44851,
    SPELL_EXPLOSIVE_BARRAGE_DAMAGE = 69019,
    SPELL_STRANGULATING            = 69413,
    SPELL_KRICK_KILL_CREDIT        = 71308,
    SPELL_NECROMANTIC_POWER        = 69753,
	SPELL_SUICIDE                  = 67375,
};

enum Texts
{
    // Krick
    SAY_KRICK_AGGRO           = -1658010,
    SAY_KRICK_SLAY_1          = -1658011,
    SAY_KRICK_SLAY_2          = -1658012,
    SAY_KRICK_BARRAGE_1       = -1658013,
    SAY_KRICK_BARRAGE_2       = -1658014,
    SAY_KRICK_POISON_NOVA     = -1658015,
    SAY_KRICK_CHASE_1         = -1658016,
    SAY_KRICK_CHASE_2         = -1658017,
    SAY_KRICK_CHASE_3         = -1658018,
    // Ick
    SAY_ICK_POISON_NOVA       = -1658020,
    SAY_ICK_CHASE_1           = -1658021,
    // OUTRO
    SAY_KRICK_OUTRO_1         = -1658030,
    SAY_JAYNA_OUTRO_2         = -1658031,
    SAY_SYLVANAS_OUTRO_2      = -1658032,
    SAY_KRICK_OUTRO_3         = -1658033,
    SAY_JAYNA_OUTRO_4         = -1658034,
    SAY_SYLVANAS_OUTRO_4      = -1658035,
    SAY_KRICK_OUTRO_5         = -1658036,
    SAY_TYRANNUS_OUTRO_7      = -1658037,
    SAY_KRICK_OUTRO_8         = -1658038,
    SAY_TYRANNUS_OUTRO_9      = -1658039,
    SAY_JAYNA_OUTRO_10        = -1658040,
    SAY_SYLVANAS_OUTRO_10     = -1658041,
};

enum Events
{
    EVENT_MIGHTY_KICK         = 1,
    EVENT_SHADOW_BOLT         = 2,
    EVENT_TOXIC_WASTE         = 3,
    EVENT_SPECIAL             = 4,
    EVENT_PURSUIT             = 5,
    EVENT_POISON_NOVA         = 6,
    EVENT_EXPLOSIVE_BARRAGE   = 7,
    // Krick OUTRO
    EVENT_OUTRO_1             = 8,
    EVENT_OUTRO_2             = 9,
    EVENT_OUTRO_3             = 10,
    EVENT_OUTRO_4             = 11,
    EVENT_OUTRO_5             = 12,
    EVENT_OUTRO_6             = 13,
    EVENT_OUTRO_7             = 14,
    EVENT_OUTRO_8             = 15,
    EVENT_OUTRO_9             = 16,
    EVENT_OUTRO_10            = 17,
    EVENT_OUTRO_11            = 18,
    EVENT_OUTRO_12            = 19,
    EVENT_OUTRO_13            = 20,
    EVENT_OUTRO_END           = 21,
};

enum KrickPhase
{
    PHASE_COMBAT = 1,
    PHASE_OUTRO  = 2,
};

enum Actions
{
    ACTION_OUTRO = 1,
};

enum Points
{
    POINT_KRICK_INTRO = 364770,
    POINT_KRICK_DEATH = 364771,
};

static const Position outroPos[8] =
{
    {828.9342f, 118.6247f, 509.5190f, 0.0000000f},  // Krick's Outro Position
    {841.0100f, 196.2450f, 573.9640f, 0.2046099f},  // Scourgelord Tyrannus Outro Position (Tele to...)
    {777.2274f, 119.5521f, 510.0363f, 6.0562930f},  // Sylvanas / Jaine Outro Spawn Position (NPC_SYLVANAS_PART1)
    {823.3984f, 114.4907f, 509.4899f, 0.0000000f},  // Sylvanas / Jaine Outro Move Position (1)
    {835.5887f, 139.4345f, 530.9526f, 0.0000000f},  // Tyrannus fly down Position (not sniffed)
    {828.9342f, 118.6247f, 514.5190f, 0.0000000f},  // Krick's Choke Position
    {828.9342f, 118.6247f, 509.4958f, 0.0000000f},  // Kirck's Death Position
    {914.4820f, 143.1602f, 633.3624f, 0.0000000f},  // Tyrannus fly up (not sniffed)
};

class boss_ick : public CreatureScript
{
    public:
        boss_ick() : CreatureScript("boss_ick") { }

        struct boss_ickAI : public BossAI
		{
			boss_ickAI(Creature* creature) : BossAI(creature, DATA_ICK), vehicle(creature->GetVehicleKit())
            {
				ASSERT(vehicle);
			}

			Vehicle* vehicle;
            float TempThreat;

            void InitializeAI()
            {
                if (!instance || static_cast<InstanceMap*>(me->GetMap())->GetScriptId() != sObjectMgr->GetScriptId(PoSScriptName))
                    me->IsAIEnabled = false;
                else if (!me->IsDead())
                    Reset();
            }

            void Reset()
            {
                events.Reset();
				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
                instance->SetBossState(DATA_ICK, NOT_STARTED);
            }

            Creature* GetKrick()
            {
                return ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_KRICK));
            }

            void EnterToBattle(Unit* /*who*/)
            {
                if (Creature* krick = GetKrick())
                    DoSendQuantumText(SAY_KRICK_AGGRO, krick);

                events.ScheduleEvent(EVENT_MIGHTY_KICK, 20000);
                events.ScheduleEvent(EVENT_TOXIC_WASTE, 5000);
                events.ScheduleEvent(EVENT_SHADOW_BOLT, 10000);
                events.ScheduleEvent(EVENT_SPECIAL, urand(30000, 35000));

                instance->SetBossState(DATA_ICK, IN_PROGRESS);
            }

			void EnterEvadeMode()
			{
				if (!_EnterEvadeMode())
					return;

				me->GetMotionMaster()->MoveTargetedHome();
				Reset();
			}

			void JustDied(Unit* killer)
            {
				if (Creature* krick = GetKrick())
                {
                    vehicle->RemoveAllPassengers();
                    if (krick->AI())
                        krick->AI()->DoAction(ACTION_OUTRO);
                }

                instance->SetBossState(DATA_ICK, DONE);
            }

            void SetTempThreat(float threat)
            {
                TempThreat = threat;
            }

            void ResetThreat(Unit* target)
            {
                DoModifyThreatPercent(target, -100);
                me->AddThreat(target, TempThreat);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!me->IsInCombatActive())
                    return;

                if (!me->GetVictim() && me->getThreatManager().isThreatListEmpty())
                {
                    EnterEvadeMode();
                    return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_TOXIC_WASTE:
                            if (Creature* krick = GetKrick())
							{
                                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
									krick->CastSpell(target, DUNGEON_MODE(SPELL_TOXIC_WASTE_5N, SPELL_TOXIC_WASTE_5H), TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE), true;
							}
                            events.ScheduleEvent(EVENT_TOXIC_WASTE, urand(7000, 10000));
                            break;
                        case EVENT_SHADOW_BOLT:
                            if (Creature* krick = GetKrick())
							{
                                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                                    krick->CastSpell(target, DUNGEON_MODE(SPELL_SHADOW_BOLT_5N, SPELL_SHADOW_BOLT_5H), TRIGGERED_IGNORE_CASTER_MOUNTED_OR_ON_VEHICLE), true;
							}
                            events.ScheduleEvent(EVENT_SHADOW_BOLT, 15000);
                            return;
                        case EVENT_MIGHTY_KICK:
                            DoCastVictim(SPELL_MIGHTY_KICK);
                            events.ScheduleEvent(EVENT_MIGHTY_KICK, 25000);
                            return;
                        case EVENT_SPECIAL:
                            events.ScheduleEvent(RAND(EVENT_EXPLOSIVE_BARRAGE, EVENT_POISON_NOVA, EVENT_PURSUIT), 1000);
                            events.ScheduleEvent(EVENT_SPECIAL, urand(23000, 28000));
                            break;
                        case EVENT_EXPLOSIVE_BARRAGE:
                            if (Creature* krick = GetKrick())
                            {
                                DoSendQuantumText(SAY_KRICK_BARRAGE_1, krick);
                                DoSendQuantumText(SAY_KRICK_BARRAGE_2, krick);
                                krick->CastSpell(krick, SPELL_EXPLOSIVE_BARRAGE_KRICK, true);
                                DoCast(me, SPELL_EXPLOSIVE_BARRAGE_ICK, true);
                            }
                            events.DelayEvents(20000);
                            break;
                        case EVENT_POISON_NOVA:
                            if (Creature* krick = GetKrick())
								DoSendQuantumText(SAY_KRICK_POISON_NOVA, krick);

                            DoSendQuantumText(SAY_ICK_POISON_NOVA, me);
                            DoCastAOE(DUNGEON_MODE(SPELL_POISON_NOVA_5N, SPELL_POISON_NOVA_5H));
                            break;
                        case EVENT_PURSUIT:
                            if (Creature* krick = GetKrick())
                                DoSendQuantumText(RAND(SAY_KRICK_CHASE_1, SAY_KRICK_CHASE_2, SAY_KRICK_CHASE_3), krick);

							if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
							{
								DoCast(me, SPELL_PURSUIT);
								me->AddThreat(target, 100.0f);
								me->GetMotionMaster()->MoveChase(target);
							}
                            break;
                        default:
                            break;
                    }
                }

				if (me->GetDistance2d(me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY()) > 55)
				{
					EnterEvadeMode();
					return;
				}

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_ickAI(creature);
        }
};

class boss_krick : public CreatureScript
{
    public:
        boss_krick() : CreatureScript("boss_krick") { }

        struct boss_krickAI : public QuantumBasicAI
        {
            boss_krickAI(Creature* creature) : QuantumBasicAI(creature), instanceScript(creature->GetInstanceScript()), summons(creature){}

			InstanceScript* instanceScript;
            SummonList summons;
            EventMap events;

            KrickPhase Phase;
            uint64 OutroNpcGUID;
            uint64 TyrannusGUID;

            void InitializeAI()
            {
                if (!instanceScript || static_cast<InstanceMap*>(me->GetMap())->GetScriptId() != sObjectMgr->GetScriptId(PoSScriptName))
                    me->IsAIEnabled = false;
                else if (!me->IsDead())
                    Reset();
            }

            void Reset()
            {
                events.Reset();
                Phase = PHASE_COMBAT;
                OutroNpcGUID = 0;
                TyrannusGUID = 0;

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

            Creature* GetIck()
            {
                return ObjectAccessor::GetCreature(*me, instanceScript->GetData64(DATA_ICK));
            }

            void KilledUnit(Unit* victim)
            {
				if (victim == me)
                    return;

                DoSendQuantumText(RAND(SAY_KRICK_SLAY_1, SAY_KRICK_SLAY_2), me);
            }

            void JustSummoned(Creature* summon)
            {
                summons.Summon(summon);
                if (summon->GetEntry() == NPC_EXPLODING_ORB)
                {
                    summon->CastSpell(summon, SPELL_EXPLODING_ORB, true);
                    summon->CastSpell(summon, SPELL_AUTO_GROW, true);
                }
            }

			void DoAction(const int32 actionId)
            {
                if (actionId == ACTION_OUTRO)
                {
                    Creature* tyrannusPtr = ObjectAccessor::GetCreature(*me, instanceScript->GetData64(DATA_TYRANNUS_EVENT));
                    if (tyrannusPtr)
						tyrannusPtr = me->SummonCreature(NPC_SCOURGELORD_TYRANNUS_INTRO, 830.935f, 192.413f, 557.547f, 4.903f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10);

                    me->GetMotionMaster()->MovePoint(POINT_KRICK_INTRO, outroPos[0].GetPositionX(), outroPos[0].GetPositionY(), outroPos[0].GetPositionZ());
                }
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE || id != POINT_KRICK_INTRO)
                    return;

                DoSendQuantumText(SAY_KRICK_OUTRO_1, me);
                Phase = PHASE_OUTRO;
                events.Reset();
                events.ScheduleEvent(EVENT_OUTRO_1, 1000);
            }

            void UpdateAI(const uint32 diff)
            {
                if (Phase != PHASE_OUTRO)
                    return;
                
                if (instanceScript->GetData(DATA_TYRANNUS_START) != DONE)
					instanceScript->SetData(DATA_TYRANNUS_START, DONE);
                 
                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_OUTRO_1:
                        {
                            if (Creature* temp = me->GetCreature(*me, instanceScript->GetData64(DATA_JAINA_SYLVANAS_1)))
                                temp->DespawnAfterAction();

                            Creature* jainaOrSylvanas = 0;
                            if (instanceScript->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                                jainaOrSylvanas = me->SummonCreature(NPC_JAINA_PART1, outroPos[2], TEMPSUMMON_MANUAL_DESPAWN);
                            else
                                jainaOrSylvanas = me->SummonCreature(NPC_SYLVANAS_PART1, outroPos[2], TEMPSUMMON_MANUAL_DESPAWN);

                            if (jainaOrSylvanas)
                            {
                                jainaOrSylvanas->GetMotionMaster()->MovePoint(0, outroPos[3]);
                                OutroNpcGUID = jainaOrSylvanas->GetGUID();
                            }
                            events.ScheduleEvent(EVENT_OUTRO_2, 6000);
                            break;
                        }
                        case EVENT_OUTRO_2:
                            if (Creature* jainaOrSylvanas = ObjectAccessor::GetCreature(*me, OutroNpcGUID))
                            {
                                jainaOrSylvanas->SetFacingToObject(me);
                                me->SetFacingToObject(jainaOrSylvanas);
                                if (instanceScript->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                                    DoSendQuantumText(SAY_JAYNA_OUTRO_2, jainaOrSylvanas);
                                else
                                    DoSendQuantumText(SAY_SYLVANAS_OUTRO_2, jainaOrSylvanas);
                            }
                            events.ScheduleEvent(EVENT_OUTRO_3, 5000);
                            break;
                        case EVENT_OUTRO_3:
                            DoSendQuantumText(SAY_KRICK_OUTRO_3, me);
                            events.ScheduleEvent(EVENT_OUTRO_4, 18000);
                            break;
                        case EVENT_OUTRO_4:
                            if (Creature* jainaOrSylvanas = ObjectAccessor::GetCreature(*me, OutroNpcGUID))
                            {
                                if (instanceScript->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                                    DoSendQuantumText(SAY_JAYNA_OUTRO_4, jainaOrSylvanas);
                                else
									DoSendQuantumText(SAY_SYLVANAS_OUTRO_4, jainaOrSylvanas);
                            }
							me->SummonCreature(NPC_SCOURGELORD_TYRANNUS_INTRO, 830.935f, 192.413f, 557.547f, 4.903f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10);
                            events.ScheduleEvent(EVENT_OUTRO_5, 5000);
                            break;
                        case EVENT_OUTRO_5:
                            DoSendQuantumText(SAY_KRICK_OUTRO_5, me);
                            events.ScheduleEvent(EVENT_OUTRO_6, 1000);
                            break;
                        case EVENT_OUTRO_6:
                            if (Creature* tyrannus = me->GetCreature(*me, instanceScript->GetData64(DATA_TYRANNUS_EVENT)))
                            {
                                tyrannus->SetSpeed(MOVE_FLIGHT, 3.5f, true);
                                tyrannus->GetMotionMaster()->MovePoint(1, outroPos[4]);
                                TyrannusGUID = tyrannus->GetGUID();
                            }
                            events.ScheduleEvent(EVENT_OUTRO_7, 5000);
                            break;
                        case EVENT_OUTRO_7:
                            if (Creature* tyrannus = ObjectAccessor::GetCreature(*me, TyrannusGUID))
							{
                                DoSendQuantumText(SAY_TYRANNUS_OUTRO_7, tyrannus);
							}
                            events.ScheduleEvent(EVENT_OUTRO_8, 5000);
                            break;
                        case EVENT_OUTRO_8:
                            //! HACK: Creature's can't have MOVEMENTFLAG_FLYING
                            me->AddUnitMovementFlag(MOVEMENTFLAG_FLYING);
                            me->GetMotionMaster()->MovePoint(0, outroPos[5]);
                            DoCast(me, SPELL_STRANGULATING);
                            events.ScheduleEvent(EVENT_OUTRO_9, 2000);
                            break;
                        case EVENT_OUTRO_9:
                            DoSendQuantumText(SAY_KRICK_OUTRO_8, me);
                            // TODO: Tyrannus starts killing Krick.
                            // there shall be some visual spell effect
                            if (Creature* tyrannus = ObjectAccessor::GetCreature(*me, TyrannusGUID))
                                tyrannus->CastSpell(me, SPELL_NECROMANTIC_POWER, true);  //not sure if it's the right spell :/
                            events.ScheduleEvent(EVENT_OUTRO_10, 1000);
                            break;
                        case EVENT_OUTRO_10:
                            //! HACK: Creature's can't have MOVEMENTFLAG_FLYING
                            me->RemoveUnitMovementFlag(MOVEMENTFLAG_FLYING);
                            me->AddUnitMovementFlag(MOVEMENTFLAG_FALLING_FAR);
                            me->GetMotionMaster()->MovePoint(0, outroPos[6]);
                            events.ScheduleEvent(EVENT_OUTRO_11, 2000);
                            break;
                        case EVENT_OUTRO_11:
                            DoCast(me, SPELL_KRICK_KILL_CREDIT); // don't really know if we need it
                            me->SetStandState(UNIT_STAND_STATE_DEAD);
                            me->SetHealth(0);
                            events.ScheduleEvent(EVENT_OUTRO_12, 3000);
                            break;
                        case EVENT_OUTRO_12:
                            if (Creature* tyrannus = ObjectAccessor::GetCreature(*me, TyrannusGUID))
							{
                                DoSendQuantumText(SAY_TYRANNUS_OUTRO_9, tyrannus);
							}
                            events.ScheduleEvent(EVENT_OUTRO_13, 2000);
                            break;
                        case EVENT_OUTRO_13:
                            if (Creature* jainaOrSylvanas = ObjectAccessor::GetCreature(*me, OutroNpcGUID))
                            {
                                if (instanceScript->GetData(DATA_TEAM_IN_INSTANCE) == ALLIANCE)
                                {
                                    DoSendQuantumText(SAY_JAYNA_OUTRO_10, jainaOrSylvanas);
                                    jainaOrSylvanas->SetSpeed(MOVE_WALK, 0.5f, true);
                                    jainaOrSylvanas->GetMotionMaster()->MovePoint(0, 847.737610f, -6.079165f, 509.911835f);
                                }
                                else
                                {
                                    DoSendQuantumText(SAY_SYLVANAS_OUTRO_10, jainaOrSylvanas);
                                    jainaOrSylvanas->SetSpeed(MOVE_WALK, 0.5f, true);
                                    jainaOrSylvanas->GetMotionMaster()->MovePoint(0, 847.737610f, -6.079165f, 509.911835f);
                                }
                            }
                            // End of OUTRO. for now...
                            events.ScheduleEvent(EVENT_OUTRO_END, 3000);
                            if (Creature* tyrannus = ObjectAccessor::GetCreature(*me, TyrannusGUID))
                                tyrannus->GetMotionMaster()->MovePoint(0, outroPos[7]);
                            break;
                        case EVENT_OUTRO_END:
                            if (Creature* tyrannus = ObjectAccessor::GetCreature(*me, TyrannusGUID))
                                tyrannus->DespawnAfterAction();
							me->DisappearAndDie();
                            break;
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_krickAI(creature);
        }
};

class spell_krick_explosive_barrage : public SpellScriptLoader
{
    public:
        spell_krick_explosive_barrage() : SpellScriptLoader("spell_krick_explosive_barrage") { }

        class spell_krick_explosive_barrage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_krick_explosive_barrage_AuraScript);

            void HandlePeriodicTick(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();

                if (Unit* caster = GetCaster())
				{
                    if (caster->GetTypeId() == TYPE_ID_UNIT)
                    {
                        Map::PlayerList const &players = caster->GetMap()->GetPlayers();

                        for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
						{
                            if (Player* player = itr->getSource())
							{
                                if (player->IsWithinDist(caster, 60.0f))
                                    caster->CastSpell(player, SPELL_EXPLOSIVE_BARRAGE_SUMMON, true);
							}
						}
					}
				}
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_krick_explosive_barrage_AuraScript::HandlePeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_krick_explosive_barrage_AuraScript();
        }
};

class spell_ick_explosive_barrage : public SpellScriptLoader
{
    public:
        spell_ick_explosive_barrage() : SpellScriptLoader("spell_ick_explosive_barrage") { }

        class spell_ick_explosive_barrage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ick_explosive_barrage_AuraScript);

            void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
				{
                    if (caster->GetTypeId() == TYPE_ID_UNIT)
                        caster->GetMotionMaster()->MoveIdle();
				}
            }

            void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
				{
                    if (caster->GetTypeId() == TYPE_ID_UNIT)
                    {
                        caster->GetMotionMaster()->Clear();
                        caster->GetMotionMaster()->MoveChase(caster->GetVictim());
                    }
				}
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_ick_explosive_barrage_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_ick_explosive_barrage_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_ick_explosive_barrage_AuraScript();
        }
};

class spell_exploding_orb_hasty_grow : public SpellScriptLoader
{
    public:
        spell_exploding_orb_hasty_grow() : SpellScriptLoader("spell_exploding_orb_hasty_grow") { }

        class spell_exploding_orb_hasty_grow_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_exploding_orb_hasty_grow_AuraScript);

            void OnStackChange(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetStackAmount() == 15)
                {
                    Unit* target = GetTarget(); // store target because aura gets removed
                    target->CastSpell(target, SPELL_EXPLOSIVE_BARRAGE_DAMAGE, false);
                    target->RemoveAurasDueToSpell(SPELL_HASTY_GROW);
                    target->RemoveAurasDueToSpell(SPELL_AUTO_GROW);
                    target->RemoveAurasDueToSpell(SPELL_EXPLODING_ORB);

                    if (Creature* creature = target->ToCreature())
                        creature->DespawnAfterAction();
                }
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_exploding_orb_hasty_grow_AuraScript::OnStackChange, EFFECT_0, SPELL_AURA_MOD_SCALE, AURA_EFFECT_HANDLE_REAPPLY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_exploding_orb_hasty_grow_AuraScript();
        }
};

class spell_krick_pursuit : public SpellScriptLoader
{
    public:
        spell_krick_pursuit() : SpellScriptLoader("spell_krick_pursuit") { }

        class spell_krick_pursuit_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_krick_pursuit_SpellScript);

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster()->GetTypeId() != TYPE_ID_UNIT)
                    return;

                Unit* caster = GetCaster();
                CreatureAI* ickAI = caster->ToCreature()->AI();
                if (Unit* target = ickAI->SelectTarget(TARGET_RANDOM, 0, 200.0f, true))
                {
                    DoSendQuantumText(SAY_ICK_CHASE_1, caster, target);
                    caster->AddAura(GetSpellInfo()->Id, target);
                    CAST_AI(boss_ick::boss_ickAI, ickAI)->SetTempThreat(caster->getThreatManager().getThreat(target));
                    caster->AddThreat(target, float(GetEffectValue()));
                    target->AddThreat(caster, float(GetEffectValue()));
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_krick_pursuit_SpellScript::HandleScriptEffect, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        class spell_krick_pursuit_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_krick_pursuit_AuraScript);

            void HandleExtraEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
				{
                    if (Creature* creCaster = caster->ToCreature())
                        CAST_AI(boss_ick::boss_ickAI, creCaster->AI())->ResetThreat(GetTarget());
				}
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_krick_pursuit_AuraScript::HandleExtraEffect, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_krick_pursuit_SpellScript();
        }

        AuraScript* GetAuraScript() const
        {
            return new spell_krick_pursuit_AuraScript();
        }
};

class spell_krick_pursuit_confusion : public SpellScriptLoader
{
    public:
        spell_krick_pursuit_confusion() : SpellScriptLoader("spell_krick_pursuit_confusion") { }

        class spell_krick_pursuit_confusion_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_krick_pursuit_confusion_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
				GetTarget()->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
				GetTarget()->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_THREAT, true);
				GetTarget()->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, true);
				GetTarget()->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TOTAL_THREAT, true);
				GetTarget()->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_CRITICAL_THREAT, true);
				GetTarget()->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_THREAT_ALL, true);
				GetTarget()->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_MODIFY_THREAT_PERCENT, true);
				GetTarget()->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_REDIRECT_THREAT, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
				GetTarget()->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, false);
				GetTarget()->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_THREAT, false);
				GetTarget()->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, false);
				GetTarget()->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TOTAL_THREAT, false);
				GetTarget()->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_CRITICAL_THREAT, false);
				GetTarget()->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_THREAT_ALL, false);
				GetTarget()->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_MODIFY_THREAT_PERCENT, false);
				GetTarget()->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_REDIRECT_THREAT, false);
				GetTarget()->GetMotionMaster()->Clear();
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_krick_pursuit_confusion_AuraScript::OnApply, EFFECT_2, SPELL_AURA_LINKED, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_krick_pursuit_confusion_AuraScript::OnRemove, EFFECT_2, SPELL_AURA_LINKED, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_krick_pursuit_confusion_AuraScript();
        }
};

void AddSC_boss_krick_and_ick()
{
    new boss_ick();
    new boss_krick();
    new spell_krick_explosive_barrage();
    new spell_ick_explosive_barrage();
    new spell_exploding_orb_hasty_grow();
    new spell_krick_pursuit();
    new spell_krick_pursuit_confusion();
}