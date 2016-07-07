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

enum Yells
{
    SAY_AMBUSH_1                    = -1658050,
    SAY_AMBUSH_2                    = -1658051,
    SAY_GAUNTLET_START              = -1658052,
    SAY_TYRANNUS_INTRO_1            = -1658053,
    SAY_GORKUN_INTRO_2              = -1658054,
    SAY_TYRANNUS_INTRO_3            = -1658055,
    SAY_AGGRO                       = -1658056,
    SAY_SLAY_1                      = -1658057,
    SAY_SLAY_2                      = -1658058,
    SAY_DEATH                       = -1658059,
    SAY_MARK_RIMEFANG_1             = -1658060,
    SAY_MARK_RIMEFANG_2             = -1658061,
    SAY_DARK_MIGHT_1                = -1658062,
    SAY_DARK_MIGHT_2                = -1658063,
    SAY_GORKUN_OUTRO_1              = -1658064,
    SAY_GORKUN_OUTRO_2              = -1658065,
    SAY_JAYNA_OUTRO_3               = -1658066,
    SAY_SYLVANAS_OUTRO_3            = -1658067,
    SAY_JAYNA_OUTRO_4               = -1658068,
    SAY_SYLVANAS_OUTRO_4            = -1658069,
    SAY_JAYNA_OUTRO_5               = -1658070,
};

enum Spells
{
    SPELL_OVERLORD_BRAND            = 69172,
    SPELL_OVERLORD_BRAND_HEAL       = 69190,
    SPELL_OVERLORD_BRAND_DAMAGE     = 69189,
    SPELL_FORCEFUL_SMASH            = 69155,
    SPELL_UNHOLY_POWER              = 69167,
    SPELL_MARK_OF_RIMEFANG          = 69275,
    SPELL_HOARFROST                 = 69246,
    SPELL_ICY_BLAST                 = 69232,
    SPELL_ICY_BLAST_AURA            = 69238,
    SPELL_EJECT_ALL_PASSENGERS      = 50630,
    SPELL_FULL_HEAL                 = 43979,
};

enum Events
{
    EVENT_OVERLORD_BRAND    = 1,
    EVENT_FORCEFUL_SMASH    = 2,
    EVENT_UNHOLY_POWER      = 3,
    EVENT_MARK_OF_RIMEFANG  = 4,
    EVENT_MOVE_NEXT         = 5,
    EVENT_HOARFROST         = 6,
    EVENT_ICY_BLAST         = 7,
    EVENT_INTRO_1           = 8,
    EVENT_INTRO_2           = 9,
    EVENT_INTRO_3           = 10,
    EVENT_COMBAT_START      = 11,
	EVENT_CHECK_PLAYERS     = 12,
};

enum Phases
{
    PHASE_NONE      = 0,
    PHASE_INTRO     = 1,
    PHASE_COMBAT    = 2,
    PHASE_OUTRO     = 3,
};

enum Actions
{
    ACTION_START_INTRO      = 1,
    ACTION_START_RIMEFANG   = 2,
    ACTION_START_OUTRO      = 3,
    ACTION_END_COMBAT       = 4,
};

#define GUID_HOARFROST 1

static const Position MoveLocations1[9] =
{
    {1064.217896f, 118.629662f, 628.156311f, 0.000000f},
    {1065.733276f, 126.342400f, 628.156128f, 0.000000f},
    {1060.914185f, 130.460403f, 628.156128f, 0.000000f},
    {1059.328003f, 120.532974f, 628.156128f, 0.000000f},
    {1052.488647f, 122.232979f, 628.156128f, 0.000000f},
    {1047.673950f, 121.389717f, 628.156128f, 0.000000f},
    {1043.781250f, 113.463493f, 628.156128f, 0.000000f},
    {1044.634521f, 109.196129f, 628.518188f, 0.000000f},
    {1052.443726f, 110.813431f, 628.156250f, 0.000000f},
};

static const Position MoveLocations2[9] =
{
    {1068.739624f, 103.664474f, 630.880005f, 0.000000f},
    {1062.253784f, 101.495079f, 630.683533f, 0.000000f},
    {1057.972168f, 100.040573f, 630.238525f, 0.000000f},
    {1053.684204f, 98.358513f, 629.913330f, 0.000000f},
    {1060.612793f, 87.334480f, 631.050354f, 0.000000f},
    {1068.163208f, 90.051262f, 631.533752f, 0.000000f},
    {1046.957642f, 108.734108f, 628.526245f, 0.000000f},
    {1044.634521f, 109.196129f, 628.518188f, 0.000000f},
    {1052.443726f, 110.813431f, 628.156250f, 0.000000f},
};

static const Position LeaderPosOutro1 = {1056.97f, 125.08f, 628.156f, 2.01569f};

static const Position RimefangPos[10] =
{
    {1017.299f, 168.9740f, 642.9259f, 0.000000f},
    {1047.868f, 126.4931f, 665.0453f, 0.000000f},
    {1069.828f, 138.3837f, 665.0453f, 0.000000f},
    {1063.042f, 164.5174f, 665.0453f, 0.000000f},
    {1031.158f, 195.1441f, 665.0453f, 0.000000f},
    {1019.087f, 197.8038f, 665.0453f, 0.000000f},
    {967.6233f, 168.9670f, 665.0453f, 0.000000f},
    {969.1198f, 140.4722f, 665.0453f, 0.000000f},
    {986.7153f, 141.6424f, 665.0453f, 0.000000f},
    {1012.601f, 142.4965f, 665.0453f, 0.000000f},
};

static const Position TyrannusCombatPos = {1018.376f, 167.2495f, 628.2811f, 0.000000f};

class boss_tyrannus : public CreatureScript
{
    public:
        boss_tyrannus() : CreatureScript("boss_tyrannus") { }

        struct boss_tyrannusAI : public BossAI
        {
            boss_tyrannusAI(Creature* creature) : BossAI(creature, DATA_TYRANNUS) {}

            void InitializeAI()
            {
                if (!instance || static_cast<InstanceMap*>(me->GetMap())->GetScriptId() != sObjectMgr->GetScriptId(PoSScriptName))
                    me->IsAIEnabled = false;
                else if (instance->GetBossState(DATA_TYRANNUS) != DONE)
                    Reset();
                else
                    me->DespawnAfterAction();
            }

            void Reset()
            {
                events.Reset();
                events.SetPhase(PHASE_NONE);

				instance->SetBossState(DATA_TYRANNUS, NOT_STARTED);

				DespawnAfterWipe();

                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
            }

            Creature* GetRimefang()
            {
                return ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_RIMEFANG));
            }

            void EnterToBattle(Unit* /*who*/)
            {
                DoSendQuantumText(SAY_AGGRO, me);
            }

            void AttackStart(Unit* victim)
            {
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    return;

                if (victim && me->Attack(victim, true) && !(events.IsInPhase(PHASE_INTRO)))
                    me->GetMotionMaster()->MoveChase(victim);
            }

            void EnterEvadeMode()
            {
                instance->SetBossState(DATA_TYRANNUS, FAIL);

				DespawnAfterWipe();

                if (Creature* rimefang = GetRimefang())
                    rimefang->AI()->EnterEvadeMode();

                me->DespawnAfterAction();
            }

            void KilledUnit(Unit* victim)
            {
                if (victim->GetTypeId() == TYPE_ID_PLAYER)
                    DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

            void JustDied(Unit* killer)
            {
				if (killer->GetTypeId() == TYPE_ID_PLAYER)
				{
					DoSendQuantumText(SAY_DEATH, me);

					instance->SetBossState(DATA_TYRANNUS, DONE);

					DespawnAfterDeath();

					// Prevent corpse despawning
					if (TempSummon* summ = me->ToTempSummon())
						summ->SetTempSummonType(TEMPSUMMON_DEAD_DESPAWN);

					// Stop combat for Rimefang
					if (Creature* rimefang = GetRimefang())
						rimefang->AI()->DoAction(ACTION_END_COMBAT);
				}

				if (killer->GetTypeId() == TYPE_ID_UNIT)
				{
					me->Respawn();
					me->AI()->EnterEvadeMode();
				}
            }

			void DespawnAfterWipe()
			{
				std::list<Creature*> TrashList;
				me->GetCreatureListWithEntryInGrid(TrashList, NPC_JAINA_PART2, 500.0f);
				me->GetCreatureListWithEntryInGrid(TrashList, NPC_SYLVANAS_PART2, 500.0f);
				me->GetCreatureListWithEntryInGrid(TrashList, NPC_GORKUN_IRONSKULL_1, 500.0f);
				me->GetCreatureListWithEntryInGrid(TrashList, NPC_MARTIN_VICTUS_2, 500.0f);
				me->GetCreatureListWithEntryInGrid(TrashList, NPC_FREED_SLAVE_1_HORDE, 500.0f);
				me->GetCreatureListWithEntryInGrid(TrashList, NPC_FREED_SLAVE_2_HORDE, 500.0f);
				me->GetCreatureListWithEntryInGrid(TrashList, NPC_FREED_SLAVE_3_HORDE, 500.0f);
				me->GetCreatureListWithEntryInGrid(TrashList, NPC_FREED_SLAVE_1_ALLIANCE, 500.0f);
				me->GetCreatureListWithEntryInGrid(TrashList, NPC_FREED_SLAVE_2_ALLIANCE, 500.0f);
				me->GetCreatureListWithEntryInGrid(TrashList, NPC_FREED_SLAVE_3_ALLIANCE, 500.0f);
				me->GetCreatureListWithEntryInGrid(TrashList, NPC_WRATHBONE_REAVER, 500.0f);
				me->GetCreatureListWithEntryInGrid(TrashList, NPC_WRATHBONE_SORCERER, 500.0f);
				me->GetCreatureListWithEntryInGrid(TrashList, NPC_FALLEN_WARRIOR, 500.0f);

				if (!TrashList.empty())
				{
					for (std::list<Creature*>::const_iterator itr = TrashList.begin(); itr != TrashList.end(); ++itr)
					{
						if (Creature* trash = *itr)
							trash->DespawnAfterAction();
					}
				}
			}

			void DespawnAfterDeath()
			{
				std::list<Creature*> TrashList;
				me->GetCreatureListWithEntryInGrid(TrashList, NPC_WRATHBONE_REAVER, 500.0f);
				me->GetCreatureListWithEntryInGrid(TrashList, NPC_WRATHBONE_SORCERER, 500.0f);
				me->GetCreatureListWithEntryInGrid(TrashList, NPC_FALLEN_WARRIOR, 500.0f);

				if (!TrashList.empty())
				{
					for (std::list<Creature*>::const_iterator itr = TrashList.begin(); itr != TrashList.end(); ++itr)
					{
						if (Creature* trash = *itr)
							trash->DespawnAfterAction();
					}
				}
			}

            void DoAction(const int32 actionId)
            {
                if (actionId == ACTION_START_INTRO)
                {
					if (IsHeroic())
						instance->DoCompleteAchievement(ACHIEVEMENT_DONT_LOOK_UP);

					DoSendQuantumText(SAY_TYRANNUS_INTRO_1, me);
                    events.SetPhase(PHASE_INTRO);
                    events.ScheduleEvent(EVENT_INTRO_1, 14000, 0, PHASE_INTRO);
                    events.ScheduleEvent(EVENT_INTRO_2, 22000, 0, PHASE_INTRO);
                    events.ScheduleEvent(EVENT_INTRO_3, 34000, 0, PHASE_INTRO);
                    events.ScheduleEvent(EVENT_COMBAT_START, 36000, 0, PHASE_INTRO);
                    instance->SetBossState(DATA_TYRANNUS, IN_PROGRESS);
                    
                   if (Creature* Slave = me->SummonCreature(NPC_GORKUN_IRONSKULL_1, 1075.489868f, 20.001131f, 632.835938f, 1.659531f, TEMPSUMMON_DEAD_DESPAWN, 10000))
                   {
                      Slave->GetMotionMaster()->MovePoint(0, LeaderPosOutro1);
                      Slave->SetHomePosition(LeaderPosOutro1);

					  if (Creature* Turannus = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_TYRANNUS)))
						  me->SetFacingToObject(Turannus);
                   }

                   for (uint8 i = 0; i < 9; ++i)
				   {
                     if (Creature* Slave = me->SummonCreature(NPC_FREED_SLAVE_1_HORDE, 1086.112061f, 21.060266f, 631.892273f, 1.995682f, TEMPSUMMON_DEAD_DESPAWN, 30000))
                     {
                          Slave->GetMotionMaster()->MovePoint(0, MoveLocations1[i]);
                          Slave->SetHomePosition(MoveLocations1[i]);

						  if (Creature* Turannus = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_TYRANNUS)))
							  me->SetFacingToObject(Turannus);
                     }
					 ++i;
                     if (Creature* Slave = me->SummonCreature(NPC_FREED_SLAVE_2_HORDE, 1069.121582f, 18.495785f, 634.020203f, 1.573138f, TEMPSUMMON_DEAD_DESPAWN, 30000))
                     {
                          Slave->GetMotionMaster()->MovePoint(0, MoveLocations1[i]);
                          Slave->SetHomePosition(MoveLocations1[i]);

						  if (Creature* Turannus = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_TYRANNUS)))
							  me->SetFacingToObject(Turannus);
                     }
                     ++i;
                     if (Creature* Slave = me->SummonCreature(NPC_FREED_SLAVE_3_HORDE, 1075.489868f, 20.001131f, 632.835938f, 1.659531f, TEMPSUMMON_DEAD_DESPAWN, 30000))
                     {
						 Slave->GetMotionMaster()->MovePoint(0, MoveLocations1[i]);
						 Slave->SetHomePosition(MoveLocations1[i]);

						 if (Creature* Turannus = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_TYRANNUS)))
							 me->SetFacingToObject(Turannus);
					 }
				   }
				}
			}

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim() && !(events.IsInPhase(PHASE_INTRO)))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INTRO_1:
                            if (Creature* GorkunOrVictus = me->GetCreature(*me, instance->GetData64(DATA_VICTUS_OR_GORKUN_FREED)))
							{
								DoSendQuantumText(SAY_GORKUN_INTRO_2, GorkunOrVictus);
							}
                            break;
                        case EVENT_INTRO_2:
							DoSendQuantumText(SAY_TYRANNUS_INTRO_3, me);
                            break;
                        case EVENT_INTRO_3:
                            me->ExitVehicle();
                            me->GetMotionMaster()->MovePoint(0, TyrannusCombatPos);
                            break;
                        case EVENT_COMBAT_START:
                            if (Creature* rimefang = me->GetCreature(*me, instance->GetData64(DATA_RIMEFANG)))
                                rimefang->AI()->DoAction(ACTION_START_RIMEFANG); // set rimefang also infight

							events.SetPhase(PHASE_COMBAT);
                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            me->SetReactState(REACT_AGGRESSIVE);
                            DoCast(me, SPELL_FULL_HEAL);
                            DoZoneInCombat();
							events.ScheduleEvent(EVENT_CHECK_PLAYERS, 0.5*IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_OVERLORD_BRAND, urand(5000, 7000));
                            events.ScheduleEvent(EVENT_FORCEFUL_SMASH, urand(14000, 16000));
                            events.ScheduleEvent(EVENT_MARK_OF_RIMEFANG, urand(25000, 27000));

							for (uint8 i = 0; i < 9; ++i)
							{
								if (Creature* reaver = me->SummonCreature(NPC_WRATHBONE_REAVER, 1069.934082f, 49.015617f, 630.590210f, 1.657956f, TEMPSUMMON_DEAD_DESPAWN, 30000))
								{ 
									if (reaver->IsAlive())
									{
										if (Creature* slave = me->FindCreatureWithDistance(NPC_FREED_SLAVE_1_HORDE, 150.0f, true))
										{
											reaver->GetMotionMaster()->MovePoint(0, MoveLocations2[i]);
											reaver->SetHomePosition(MoveLocations2[i]);
											reaver->Attack(slave, true);
											reaver->GetMotionMaster()->MoveChase(slave);
										}
									}
								}
								++i;
                         if (Creature* sorcerer = me->SummonCreature(NPC_WRATHBONE_SORCERER, 1069.934082f, 49.015617f, 630.590210f, 1.657956f, TEMPSUMMON_DEAD_DESPAWN, 30000))
                         {
                            if (sorcerer->IsAlive())
                            {
                               if (Creature* slave = me->FindCreatureWithDistance(NPC_FREED_SLAVE_2_HORDE, 150.0f, true))
                               {
                                  sorcerer->GetMotionMaster()->MovePoint(0, MoveLocations2[i]);
                                  sorcerer->SetHomePosition(MoveLocations2[i]);
                                  sorcerer->Attack(slave, true);
                                  sorcerer->GetMotionMaster()->MoveChase(slave);
                               }
                            }
                         }
						 ++i;
                          if (Creature* fallen = me->SummonCreature(NPC_FALLEN_WARRIOR, 1069.934082f, 49.015617f, 630.590210f, 1.657956f, TEMPSUMMON_DEAD_DESPAWN, 30000))
                          {
                              if (fallen->IsAlive())
                              {
                                 if (Creature* slave = me->FindCreatureWithDistance(NPC_FREED_SLAVE_3_HORDE, 150.0f, true))
                                 {
                                     fallen->GetMotionMaster()->MovePoint(0, MoveLocations2[i]);
                                     fallen->SetHomePosition(MoveLocations2[i]);
                                     fallen->Attack(slave, true);
									 fallen->GetMotionMaster()->MoveChase(slave);
								 }
							  }
						  }
						}
						break;
						case EVENT_CHECK_PLAYERS:
						{
							Map::PlayerList const& Players = me->GetMap()->GetPlayers();
							for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
							{
								if (Player* player = itr->getSource())
								{
									if (!player->IsAlive())
										EnterEvadeMode();
								}
							}
							events.ScheduleEvent(EVENT_CHECK_PLAYERS, 0.5*IN_MILLISECONDS);
							break;
						}
                        case EVENT_OVERLORD_BRAND:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 0.0f, true))
							{
                                DoCast(target, SPELL_OVERLORD_BRAND);
							}
                            events.ScheduleEvent(EVENT_OVERLORD_BRAND, urand(11000, 12000));
                            break;
                        case EVENT_FORCEFUL_SMASH:
                            DoCastVictim(SPELL_FORCEFUL_SMASH);
                            events.ScheduleEvent(EVENT_UNHOLY_POWER, 1000);
                            break;
                        case EVENT_UNHOLY_POWER:
                            DoSendQuantumText(SAY_DARK_MIGHT_1, me);
                            DoSendQuantumText(SAY_DARK_MIGHT_2, me);
                            DoCast(me, SPELL_UNHOLY_POWER);
                            events.ScheduleEvent(EVENT_FORCEFUL_SMASH, urand(40000, 48000));
                            break;
                        case EVENT_MARK_OF_RIMEFANG:
                            DoSendQuantumText(SAY_MARK_RIMEFANG_1, me);
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 0.0f, true))
                            {
                                DoSendQuantumText(SAY_MARK_RIMEFANG_2, me, target);
                                DoCast(target, SPELL_MARK_OF_RIMEFANG);
                            }
                            events.ScheduleEvent(EVENT_MARK_OF_RIMEFANG, urand(24000, 26000));
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
            return new boss_tyrannusAI(creature);
        }
};

class boss_rimefang : public CreatureScript
{
    public:
        boss_rimefang() : CreatureScript("boss_rimefang") { }

        struct boss_rimefangAI : public QuantumBasicAI
        {
            boss_rimefangAI(Creature* creature) : QuantumBasicAI(creature), vehicle(creature->GetVehicleKit())
            {
                ASSERT(vehicle);
            }

			Vehicle* vehicle;
            uint64 HoarfrostTargetGUID;
            EventMap events;
            uint8 CurrentWaypoint;

            void Reset()
            {
                events.Reset();
                events.SetPhase(PHASE_NONE);
                CurrentWaypoint = 0;
                HoarfrostTargetGUID = 0;
                me->SetCanFly(true);
				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }

            void JustReachedHome()
            {
                vehicle->InstallAllAccessories(false);
            }

            void DoAction(const int32 actionId)
            {
                if (actionId == ACTION_START_RIMEFANG)
                {
                    events.SetPhase(PHASE_COMBAT);
                    DoZoneInCombat();
                    events.ScheduleEvent(EVENT_MOVE_NEXT, 500, 0, PHASE_COMBAT);
                    events.ScheduleEvent(EVENT_ICY_BLAST, 15000, 0, PHASE_COMBAT);
                }
                else if (actionId == ACTION_END_COMBAT)
                    _EnterEvadeMode();
            }

            void SetGUID(uint64 guid, int32 type)
            {
                if (type == GUID_HOARFROST)
                {
                    HoarfrostTargetGUID = guid;
                    events.ScheduleEvent(EVENT_HOARFROST, 1000);
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim() && !(events.IsInPhase(PHASE_COMBAT)))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_MOVE_NEXT:
                            if (CurrentWaypoint >= 10 || CurrentWaypoint == 0)
                                CurrentWaypoint = 1;
                            me->GetMotionMaster()->MovePoint(0, RimefangPos[CurrentWaypoint]);
                            ++CurrentWaypoint;
                            events.ScheduleEvent(EVENT_MOVE_NEXT, 2000, 0, PHASE_COMBAT);
                            break;
                        case EVENT_ICY_BLAST:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
							{
                                DoCast(target, SPELL_ICY_BLAST);
							}
                            events.ScheduleEvent(EVENT_ICY_BLAST, 15000, 0, PHASE_COMBAT);
                            break;
                        case EVENT_HOARFROST:
                            if (Unit* target = me->GetUnit(*me, HoarfrostTargetGUID))
                            {
                                DoCast(target, SPELL_HOARFROST);
                                HoarfrostTargetGUID = 0;
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_rimefangAI(creature);
        }
};

class player_overlord_brandAI : public PlayerAI
{
    public:
        player_overlord_brandAI(Player* player, uint64 casterGUID) : PlayerAI(player), _tyrannusGUID(casterGUID) { }

        void DamageDealt(Unit* /*victim*/, uint32& damage, DamageEffectType /*damageType*/)
        {
            if (Creature* tyrannus = ObjectAccessor::GetCreature(*me, _tyrannusGUID))
                if (Unit* victim = tyrannus->GetVictim())
                    me->CastCustomSpell(SPELL_OVERLORD_BRAND_DAMAGE, SPELLVALUE_BASE_POINT0, damage, victim, true, NULL, NULL, tyrannus->GetGUID());
        }

        void HealDone(Unit* /*target*/, uint32& addHealth)
        {
            if (Creature* tyrannus = ObjectAccessor::GetCreature(*me, _tyrannusGUID))
                me->CastCustomSpell(SPELL_OVERLORD_BRAND_HEAL, SPELLVALUE_BASE_POINT0, int32(addHealth * 5.5f), tyrannus, true, NULL, NULL, tyrannus->GetGUID());
        }

        void UpdateAI(uint32 const /*diff*/) { }

    private:
		uint64 _tyrannusGUID;
};

class spell_tyrannus_overlord_brand : public SpellScriptLoader
{
    public:
        spell_tyrannus_overlord_brand() : SpellScriptLoader("spell_tyrannus_overlord_brand") { }

        class spell_tyrannus_overlord_brand_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tyrannus_overlord_brand_AuraScript);

        public:
            spell_tyrannus_overlord_brand_AuraScript()
            {
                oldAI = NULL;
                oldAIState = false;
            }

        private:
            bool Load()
            {
                return GetCaster() && GetCaster()->GetEntry() == NPC_TYRANNUS;
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget()->GetTypeId() != TYPE_ID_PLAYER)
                    return;

                oldAI = GetTarget()->GetAI();
                oldAIState = GetTarget()->IsAIEnabled;
                GetTarget()->SetAI(new player_overlord_brandAI(GetTarget()->ToPlayer(), GetCasterGUID()));
                GetTarget()->IsAIEnabled = true;
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget()->GetTypeId() != TYPE_ID_PLAYER)
                    return;

                GetTarget()->IsAIEnabled = oldAIState;
                UnitAI* thisAI = GetTarget()->GetAI();
                GetTarget()->SetAI(oldAI);
                delete thisAI;
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_tyrannus_overlord_brand_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_tyrannus_overlord_brand_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }

            UnitAI* oldAI;
            bool oldAIState;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_tyrannus_overlord_brand_AuraScript();
        }
};

class spell_tyrannus_mark_of_rimefang : public SpellScriptLoader
{
    public:
        spell_tyrannus_mark_of_rimefang() : SpellScriptLoader("spell_tyrannus_mark_of_rimefang") { }

        class spell_tyrannus_mark_of_rimefang_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_tyrannus_mark_of_rimefang_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* caster = GetCaster();
                if (!caster || caster->GetTypeId() != TYPE_ID_UNIT)
                    return;

                if (InstanceScript* instance = caster->GetInstanceScript())
				{
                    if (Creature* rimefang = ObjectAccessor::GetCreature(*caster, instance->GetData64(DATA_RIMEFANG)))
                        rimefang->AI()->SetGUID(GetTarget()->GetGUID(), GUID_HOARFROST);
				}
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_tyrannus_mark_of_rimefang_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_tyrannus_mark_of_rimefang_AuraScript();
        }
};

class spell_tyrannus_rimefang_icy_blast : public SpellScriptLoader
{
    public:
        spell_tyrannus_rimefang_icy_blast() : SpellScriptLoader("spell_tyrannus_rimefang_icy_blast") { }

        class spell_tyrannus_rimefang_icy_blast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_tyrannus_rimefang_icy_blast_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ICY_BLAST_AURA))
                    return false;
                return true;
            }

            void HandleTriggerMissile(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

				if (Position const* pos = GetExplTargetDest())
				{
					if (TempSummon* summon = GetCaster()->SummonCreature(NPC_ICY_BLAST, *pos, TEMPSUMMON_TIMED_DESPAWN, 1*MINUTE*IN_MILLISECONDS))
						summon->CastSpell(summon, SPELL_ICY_BLAST_AURA, true);
				}
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_tyrannus_rimefang_icy_blast_SpellScript::HandleTriggerMissile, EFFECT_1, SPELL_EFFECT_TRIGGER_MISSILE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_tyrannus_rimefang_icy_blast_SpellScript();
        }
};

class at_tyrannus_event_starter : public AreaTriggerScript
{
public:
	at_tyrannus_event_starter() : AreaTriggerScript("at_tyrannus_event_starter") { }

	bool OnTrigger(Player* player, const AreaTriggerEntry* /*at*/)
	{
		InstanceScript* instance = player->GetInstanceScript();

		if (player->IsGameMaster() || !instance)
			return false;

		if (instance->GetBossState(DATA_TYRANNUS) != IN_PROGRESS && instance->GetBossState(DATA_TYRANNUS) != DONE)
		{
			instance->SetData(DATA_AREA_TRIGGER_ICE_CICLE, DONE);

			if (Creature* tyrannus = ObjectAccessor::GetCreature(*player, instance->GetData64(DATA_TYRANNUS)))
			{
				tyrannus->AI()->DoAction(ACTION_START_INTRO);
				return true;
			}
		}
		return false;
	}
};

void AddSC_boss_tyrannus()
{
    new boss_tyrannus();
    new boss_rimefang();
    new spell_tyrannus_overlord_brand();
    new spell_tyrannus_mark_of_rimefang();
	new spell_tyrannus_rimefang_icy_blast();
    new at_tyrannus_event_starter();
}