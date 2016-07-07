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
#include "CombatAI.h"
#include "PassiveAI.h"
#include "ObjectMgr.h"
#include "SpellInfo.h"
#include "SpellScript.h"
#include "Vehicle.h"
#include "ulduar.h"

enum Texts
{
    SAY_AGGRO               = -1603060,
    SAY_SLAY                = -1603061,
    SAY_DEATH               = -1603062,
    SAY_TARGET_1            = -1603063,
    SAY_TARGET_2            = -1603064,
    SAY_TARGET_3            = -1603065,
    SAY_HARD_MODE           = -1603066,
    SAY_TOWER_NONE          = -1603067,
    SAY_TOWER_FROST         = -1603068,
    SAY_TOWER_FLAME         = -1603069,
    SAY_TOWER_NATURE        = -1603070,
    SAY_TOWER_STORM         = -1603071,
    SAY_PLAYER_RIDING       = -1603072,
    SAY_OVERLOAD_1          = -1603073,
    SAY_OVERLOAD_2          = -1603074,
    SAY_OVERLOAD_3          = -1603075,
	EMOTE_REPAIR            = -1613230,
	EMOTE_OVERLOAD          = -1613231,
	EMOTE_PURSUE            = -1613232,
};

enum Spells
{
    SPELL_PURSUED                  = 62374,
    SPELL_GATHERING_SPEED          = 62375,
    SPELL_BATTERING_RAM            = 62376,
    SPELL_FLAME_VENTS              = 62396,
    SPELL_MISSILE_BARRAGE          = 62400,
    SPELL_SYSTEM_SHUTDOWN          = 62475,
    SPELL_OVERLOAD_CIRCUIT         = 62399,
    SPELL_START_THE_ENGINE         = 62432, // 62472
    SPELL_SEARING_FLAME            = 62402,
    SPELL_BLAZE                    = 62292,
    SPELL_TAR_PASSIVE              = 62288,
	SPELL_TAR_PROC                 = 62287,
    SPELL_SMOKE_TRAIL              = 63575,
    SPELL_SMOKE_TRAIL_VISUAL       = 61364,
    SPELL_ELECTROSHOCK             = 62522,
    SPELL_NAPALM                   = 63666,
	SPELL_PASSENGER_LOADED         = 62340,
    // TOWER Additional SPELLS
    SPELL_THORIMS_HAMMER           = 62911, // Tower of Storms
    SPELL_MIMIRONS_INFERNO         = 62909, // Tower of Flames
    SPELL_HODIRS_FURY              = 62533, // Tower of Frost
    SPELL_FREYAS_WARD              = 62906, // Tower of Nature
    SPELL_FREYA_SUMMONS            = 62947, // Tower of Nature
    // TOWER ap & health spells
    SPELL_BUFF_TOWER_OF_STORMS     = 65076,
    SPELL_BUFF_TOWER_OF_FLAMES     = 65075,
    SPELL_BUFF_TOWER_OF_FR0ST      = 65077,
    SPELL_BUFF_TOWER_OF_LIFE       = 64482,
    // Additional Spells
    SPELL_FORCE_REACTION           = 57605, // temporary
    SPELL_LASH                     = 65062,
    SPELL_AUTO_REPAIR              = 62705,
    SPELL_BLUE_SKYBEAM             = 63769,
    SPELL_GREEN_SKYBEAM            = 63771,
    SPELL_RED_SKYBEAM              = 63772,
    SPELL_LIGHTNING_SKYBEAM        = 63773,
	SPELL_ANTI_AIR_ROCKET_DMG      = 62363,
	// Demolishers Spell
    SPELL_LIQUID_PYRITE            = 62494,
    SPELL_DUSTY_EXPLOSION          = 63360,
    SPELL_DUST_CLOUD_IMPACT        = 54740,
	SPELL_GRAB_PYRITE              = 67372,
	SPELL_GRAB_CRATE               = 67387,
	SPELL_RIDE_VEHICLE             = 65266,
};

enum Events
{
    EVENT_PURSUE            = 1,
    EVENT_MISSILE           = 2,
    EVENT_VENT              = 3,
    EVENT_SPEED             = 4,
    EVENT_SHUTDOWN          = 5,
    EVENT_REPAIR            = 6,
    EVENT_THORIMS_HAMMER    = 7,
    EVENT_MIMIRONS_INFERNO  = 8,
    EVENT_HODIRS_FURY       = 9,
    EVENT_FREYAS_WARD       = 10,
};

enum Seats
{
    SEAT_PLAYER             = 0,
    SEAT_TURRET             = 1,
    SEAT_DEVICE             = 2,
    SEAT_CANNON             = 7,
};

enum AchievementData
{
    DATA_SHUTOUT            = 1,
    DATA_UNBROKEN           = 2,
    DATA_ORBIT_ACHIEVEMENTS = 3,
};

enum Actions
{
    ACTION_TOWER_OF_STORM_DESTROYED  = 1,
    ACTION_TOWER_OF_FROST_DESTROYED  = 2,
    ACTION_TOWER_OF_FLAMES_DESTROYED = 3,
    ACTION_TOWER_OF_LIFE_DESTROYED   = 4,
    ACTION_START_ENCOUNTER           = 10,
};

Position const Misc[] =
{
	{266.689f, -33.391f, 409.99f, 0.0f},
    {379.158f, -33.318f, 409.81f, 0.0f},
    {266.611f, -133.387f, 409.81f, 0.0f},
    {158.411f, -33.311f, 409.81f, 0.0f},
    {266.699f, 66.632f, 409.81f, 0.0f},
};

Position const Center[] =
{
	{354.8771f, -12.90240f, 409.803650f, 3.05873f},
};

#define EMOTE_PYRITE "%s Has Grabbed Liquid Pyrite!"

class boss_flame_leviathan : public CreatureScript
{
    public:
        boss_flame_leviathan() : CreatureScript("boss_flame_leviathan") { }

        struct boss_flame_leviathanAI : public BossAI
        {
            boss_flame_leviathanAI(Creature* creature) : BossAI(creature, DATA_FLAME_LEVIATHAN), vehicle(creature->GetVehicleKit()){}

            void InitializeAI()
            {
                ASSERT(vehicle);

                if (!me->IsDead())
                    Reset();

                ActiveTowersCount = 4;
                Shutdown = 0;
                ActiveTowers = false;
                TowerOfStorms = false;
                TowerOfLife = false;
                TowerOfFlames = false;
                TowerOfFrost = false;
                Shutout = true;
                Unbroken = true;

                DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_STUNNED);
                me->SetReactState(REACT_PASSIVE);

                if (instance->GetData(DATA_GIANT_COLOSSUS) == 2)
                {
                    DoAction(ACTION_START_ENCOUNTER);

                    if (GameObject* gate = me->FindGameobjectWithDistance(GO_LEVIATHAN_GATE, 50.0f))
                        gate->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                }
            }

            Vehicle* vehicle;
            uint8 ActiveTowersCount;
            uint8 Shutdown;
            bool ActiveTowers;
            bool TowerOfStorms;
            bool TowerOfLife;
            bool TowerOfFlames;
            bool TowerOfFrost;
            bool Shutout;
            bool Unbroken;
            bool Pursued;

            void Reset()
            {
                _Reset();

				me->SetPowerType(POWER_ENERGY);
				me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);

                Shutdown = 0;

                Pursued = false;

                _pursueTarget = 0;

                me->SetActive(true);

				me->SetHomePosition(Center->GetPositionX(), Center->GetPositionY(), Center->GetPositionZ(), Center->GetOrientation());
            }

			void MoveInLineOfSight(Unit* who)
			{
				if (who->GetEntry() == NPC_COMMANDER || NPC_DEFENDER || NPC_ENGINEER)
					return;
			}

            void EnterToBattle(Unit* /*who*/)
            {
				_EnterToBattle();

				me->SetActive(true);

				DoZoneInCombat();

                events.ScheduleEvent(EVENT_PURSUE, 30*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_MISSILE, urand(1500, 4*IN_MILLISECONDS));
                events.ScheduleEvent(EVENT_VENT, 20*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_SPEED, 15*IN_MILLISECONDS);
				events.ScheduleEvent(EVENT_SHUTDOWN, 150*IN_MILLISECONDS);
                ActiveTower();

				HandleAccessorys(true);
            }

			void EnterEvadeMode()
			{
				me->SetHomePosition(Center->GetPositionX(), Center->GetPositionY(), Center->GetPositionZ(), Center->GetOrientation());
				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_STUNNED);
				BossAI::EnterEvadeMode();
			}

			void KilledUnit(Unit* victim)
			{
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(SAY_SLAY, me);
			}

            void JustDied(Unit* killer)
            {
				_JustDied();

				DoSendQuantumText(SAY_DEATH, me);

				SalvagedDemolisherDespawner();
				SalvagedSiegeEngineDespawner();
				SalvagedChopperDespawner();

				EndBattleEvent();

				if (GameObject* gate = me->FindGameobjectWithDistance(GO_LEVIATHAN_GATE, 1500.0f))
					gate->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
            }

			void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                switch (spell->Id)
                {
                    case SPELL_START_THE_ENGINE:
                        HandleAccessorys(true);
                        break;
                    case SPELL_ELECTROSHOCK:
                        me->InterruptSpell(CURRENT_CHANNELED_SPELL);
                        break;
					case SPELL_TAR_PASSIVE:
						DoCast(me, SPELL_TAR_PROC, true);
                        break;
                }
            }

			void SpellHitTarget(Unit* target, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_PURSUED)
                {
					if (Player* player = target->ToPlayer())
					{
						if (player->HasAuraType(SPELL_AURA_CONTROL_VEHICLE))
						{
							DoResetThreat();
							me->AddThreat(player, 9999999.0f);
							me->GetMotionMaster()->MoveChase(player);
							_pursueTarget = player->GetGUID();
							DoSendQuantumText(EMOTE_PURSUE, me, target);
						}
					}
                }
            }

            void HandleAccessorys(bool install)
            {
                if (install)
                {
                    // Seats
                    for (uint8 i = RAID_MODE<uint8>(2, 0); i < 4; ++i)
					{
                        if (Creature* seat = me->SummonCreature(NPC_SEAT, *me))
                        {
                            if (Creature* turret = me->SummonCreature(NPC_DEFENSE_TURRET, *me))
                                turret->EnterVehicle(seat, SEAT_TURRET);

							if (Creature* turret = me->SummonCreature(NPC_DEFENSE_TURRET, *me))
								turret->EnterVehicle(seat, SEAT_TURRET);

                            if (Creature* device = me->SummonCreature(NPC_OVERLOAD_DEVICE, *me))
                                device->EnterVehicle(seat, SEAT_DEVICE);

                            seat->EnterVehicle(me, i);
                        }
					}

                    // Cannon
                    if (Creature* cannon = me->SummonCreature(NPC_DEFENSE_CANNON, *me))
                        cannon->EnterVehicle(me, SEAT_CANNON);
                }
                else
                {
                    for (uint8 i = 0; i < 4; ++i)
					{
                        if (Unit* seat = vehicle->GetPassenger(i))
						{
                            if (seat->ToCreature() && seat->GetVehicleKit())
                            {
                                if (Unit* turret = seat->GetVehicleKit()->GetPassenger(SEAT_TURRET))
								{
                                    if (turret->ToCreature())
                                        turret->ToCreature()->DespawnAfterAction(1000);
								}

                                if (Unit* device = seat->GetVehicleKit()->GetPassenger(SEAT_DEVICE))
								{
                                    if (device->ToCreature())
                                        device->ToCreature()->DespawnAfterAction(1000);
								}

                                seat->ToCreature()->DespawnAfterAction(500);
                            }
						}
					}

                    // Cannon
                    if (Unit* cannon = vehicle->GetPassenger(SEAT_CANNON))
					{
                        if (cannon->ToCreature())
                            cannon->ToCreature()->DespawnAfterAction(500);
					}
                }
            }

            void SetGUID(uint64 guid, int32 /*id*/ = 0)
            {
                if (!me->IsInCombatActive())
                    return;

                if (Unit* passenger = ObjectAccessor::GetUnit(*me, guid))
				{
                    for (uint8 i = RAID_MODE<uint8>(2, 0); i < 4; ++i)
					{
                        if (Unit* seat = vehicle->GetPassenger(i))
						{
                            if (seat->GetVehicleKit()->HasEmptySeat(SEAT_PLAYER) && !seat->GetVehicleKit()->HasEmptySeat(SEAT_TURRET))
                            {
                                passenger->EnterVehicle(seat, SEAT_PLAYER);
                                return;
							}
						}
					}
				}
			}

            void ActiveTower()
            {
                if (ActiveTowers)
                {
                    if (TowerOfFrost)
                    {
                        me->AddAura(SPELL_BUFF_TOWER_OF_FR0ST, me);
                        events.ScheduleEvent(EVENT_HODIRS_FURY, 20*IN_MILLISECONDS);
                    }

                    if (TowerOfLife)
                    {
                        me->AddAura(SPELL_BUFF_TOWER_OF_LIFE, me);
                        events.ScheduleEvent(EVENT_FREYAS_WARD, 30*IN_MILLISECONDS);
                    }

                    if (TowerOfFlames)
                    {
                        me->AddAura(SPELL_BUFF_TOWER_OF_FLAMES, me);
                        events.ScheduleEvent(EVENT_MIMIRONS_INFERNO, 40*IN_MILLISECONDS);
                    }

                    if (TowerOfStorms)
                    {
                        me->AddAura(SPELL_BUFF_TOWER_OF_STORMS, me);
                        events.ScheduleEvent(EVENT_THORIMS_HAMMER, 50*IN_MILLISECONDS);
                    }

                    if (!TowerOfLife && !TowerOfFrost && !TowerOfFlames && !TowerOfStorms)
                        DoSendQuantumText(SAY_TOWER_NONE, me);
                    else
                        DoSendQuantumText(SAY_HARD_MODE, me);
                }
                else DoSendQuantumText(SAY_AGGRO, me);
            }

			void SalvagedDemolisherDespawner()
			{
				std::list<Creature*> SalvagedDemolisherList;
				me->GetCreatureListWithEntryInGrid(SalvagedDemolisherList, NPC_SALVAGED_DEMOLISHER, 550.0f);

				if (!SalvagedDemolisherList.empty())
				{
					for (std::list<Creature*>::const_iterator itr = SalvagedDemolisherList.begin(); itr != SalvagedDemolisherList.end(); ++itr)
					{
						if (Creature* demolisher = *itr)
							demolisher->Kill(demolisher);
					}
				}
			}

			void SalvagedSiegeEngineDespawner()
			{
				std::list<Creature*> SalvagedSiegeEngineList;
				me->GetCreatureListWithEntryInGrid(SalvagedSiegeEngineList, NPC_SALVAGED_SIEGE_ENGINE, 550.0f);

				if (!SalvagedSiegeEngineList.empty())
				{
					for (std::list<Creature*>::const_iterator itr = SalvagedSiegeEngineList.begin(); itr != SalvagedSiegeEngineList.end(); ++itr)
					{
						if (Creature* engine = *itr)
							engine->Kill(engine);
					}
				}
			}

			void SalvagedChopperDespawner()
			{
				std::list<Creature*> SalvagedChopperList;
				me->GetCreatureListWithEntryInGrid(SalvagedChopperList, NPC_SALVAGED_CHOPPER, 550.0f);

				if (!SalvagedChopperList.empty())
				{
					for (std::list<Creature*>::const_iterator itr = SalvagedChopperList.begin(); itr != SalvagedChopperList.end(); ++itr)
					{
						if (Creature* chopper = *itr)
							chopper->Kill(chopper);
					}
				}
			}

			void EndBattleEvent()
			{
				Map* map = me->GetMap();

				Map::PlayerList const& Players = me->GetMap()->GetPlayers();
				for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
				{
					if (!Players.isEmpty())
					{
						if (Player* player = itr->getSource())
						{
							player->ExitVehicle();
							player->KilledMonsterCredit(me->GetEntry(), 0);
							((InstanceMap*)map)->PermBindAllPlayers(Players.begin()->getSource());
						}
					}
				}
			}

            uint32 GetData(uint32 type)
            {
                switch (type)
                {
                    case DATA_SHUTOUT:
                        return Shutout ? 1 : 0;
                    case DATA_UNBROKEN:
                        return Unbroken ? 1 : 0;
                    case DATA_ORBIT_ACHIEVEMENTS:
                        if (ActiveTowers) // HardMode
                            return ActiveTowersCount;
                    default:
                        break;
                }
                return 0;
            }

            void SetData(uint32 id, uint32 data)
            {
                if (id == DATA_UNBROKEN)
                    Unbroken = data ? true : false;
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (!Pursued && me->GetVictim() && !me->GetVictim()->HasAura(SPELL_PURSUED) && !me->HasAura(SPELL_SYSTEM_SHUTDOWN))
                {
                    Pursued = true;
                    events.RescheduleEvent(EVENT_PURSUE, 1000);
                }

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_PURSUE:
                            DoSendQuantumText(RAND(SAY_TARGET_1, SAY_TARGET_2, SAY_TARGET_3), me);
                            DoCast(SPELL_PURSUED);
                            Pursued = false;
                            events.ScheduleEvent(EVENT_PURSUE, 30*IN_MILLISECONDS);
                            break;
                        case EVENT_MISSILE:
                            DoCast(me, SPELL_MISSILE_BARRAGE, true);
                            events.ScheduleEvent(EVENT_MISSILE, 1500);
                            break;
                        case EVENT_VENT:
                            DoCastAOE(SPELL_FLAME_VENTS);
                            events.ScheduleEvent(EVENT_VENT, 20*IN_MILLISECONDS);
                            break;
                        case EVENT_SPEED:
                            DoCastAOE(SPELL_GATHERING_SPEED);
                            events.ScheduleEvent(EVENT_SPEED, 15*IN_MILLISECONDS);
                            break;
                        case EVENT_SHUTDOWN:
							if (Shutout)
								Shutout = false;
                            DoSendQuantumText(RAND(SAY_OVERLOAD_1, SAY_OVERLOAD_2, SAY_OVERLOAD_3), me);
                            DoSendQuantumText(EMOTE_OVERLOAD, me);
                            me->StopMoving();
                            DoCast(me, SPELL_SYSTEM_SHUTDOWN, true);
                            events.DelayEvents(20*IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_REPAIR, 4*IN_MILLISECONDS);
                            break;
                        case EVENT_REPAIR:
                            me->MonsterTextEmote(EMOTE_REPAIR, LANG_UNIVERSAL, true);
                            HandleAccessorys(false);
							events.ScheduleEvent(EVENT_SHUTDOWN, 150*IN_MILLISECONDS);
                            break;
                        case EVENT_THORIMS_HAMMER:
                            for (uint8 i = 0; i < 15; ++i)
                            {
                                Position pos = Misc[0];
                                pos.m_positionX += float(irand(-100, 105));
                                pos.m_positionY += float(irand(-95, 95));
                                DoSummon(NPC_THORIM_RETICLE, pos, 30*IN_MILLISECONDS, TEMPSUMMON_TIMED_DESPAWN);
                            }
                            DoSendQuantumText(SAY_TOWER_STORM, me);
                            break;
                        case EVENT_MIMIRONS_INFERNO:
                            me->SummonCreature(NPC_MIMIRON_RETICLE, Misc[1]);
                            DoSendQuantumText(SAY_TOWER_FLAME, me);
                            break;
                        case EVENT_HODIRS_FURY:
                            for (uint8 i = 0; i < 2; ++i)
                                DoSummon(NPC_HODIR_RETICLE, me, 50.0f, 0);
                            DoSendQuantumText(SAY_TOWER_FROST, me);
                            break;
                        case EVENT_FREYAS_WARD:
                            DoSendQuantumText(SAY_TOWER_NATURE, me);
                            StartFreyaEvent();
                            break;
                    }
                }

                if (!me->HasAura(SPELL_SYSTEM_SHUTDOWN))
                    DoBatteringRamIfReady();

                EnterEvadeIfOutOfCombatArea(diff);
            }

            void StartFreyaEvent()
            {
                me->SummonCreature(NPC_FREYA_RETICLE, 377.02f, -119.10f, 409.81f);
                me->SummonCreature(NPC_FREYA_RETICLE, 377.02f, 54.78f, 409.81f);
                me->SummonCreature(NPC_FREYA_RETICLE, 185.62f, 54.78f, 409.81f);
                me->SummonCreature(NPC_FREYA_RETICLE, 185.62f, -119.10f, 409.81f);
            }

            void DoAction(int32 const action)
            {
                if (action == ACTION_START_ENCOUNTER)
                {
                    if (!me->IsDead())
                    {
                        me->GetMotionMaster()->MoveCharge(Center->GetPositionX(), Center->GetPositionY(), Center->GetPositionZ());
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_STUNNED);
                        return;
                    }
                }

                switch (action)
                {
                    case ACTION_ACTIVATE_HARD_MODE:
                        ActiveTowers = true;
                        TowerOfStorms = true;
                        TowerOfLife = true;
                        TowerOfFlames = true;
                        TowerOfFrost = true;
                        break;
                    case ACTION_TOWER_OF_STORM_DESTROYED:
                        if (TowerOfStorms)
                        {
                            TowerOfStorms = false;
                            --ActiveTowersCount;
                        }
                        break;
                    case ACTION_TOWER_OF_FROST_DESTROYED:
                        if (TowerOfFrost)
                        {
                            TowerOfFrost = false;
                            --ActiveTowersCount;
                        }
                        break;
                    case ACTION_TOWER_OF_FLAMES_DESTROYED:
                        if (TowerOfFlames)
                        {
                            TowerOfFlames = false;
                            --ActiveTowersCount;
                        }
                        break;
                    case ACTION_TOWER_OF_LIFE_DESTROYED:
                        if (TowerOfLife)
                        {
                            TowerOfLife = false;
                            --ActiveTowersCount;
                        }
                        break;
                    default:
                        break;
                }
            }

            private:

                void DoBatteringRamIfReady()
                {
                    if (me->IsAttackReady())
                    {
                        Unit* target = ObjectAccessor::GetUnit(*me, _pursueTarget);

                        if (me->IsWithinCombatRange(target, 30.0f))
                        {
                            DoCast(target, SPELL_BATTERING_RAM);
                            me->ResetAttackTimer();
                        }
                    }
                }

                uint64 _pursueTarget;
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_flame_leviathanAI(creature);
        }
};

class npc_flame_leviathan_defense_cannon : public CreatureScript
{
    public:
        npc_flame_leviathan_defense_cannon() : CreatureScript("npc_flame_leviathan_defense_cannon") { }

        struct npc_flame_leviathan_defense_cannonAI : public QuantumBasicAI
        {
            npc_flame_leviathan_defense_cannonAI(Creature* creature) : QuantumBasicAI(creature){}

			uint32 NapalmTimer;

            void Reset()
            {
                NapalmTimer = 15000;
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (NapalmTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        if (CanNapalmAttack(target))
						{
                            DoCast(target, SPELL_NAPALM, true);
						}
					}

                    NapalmTimer = 10000;
                }
                else
                    NapalmTimer -= diff;
            }

            bool CanNapalmAttack(Unit const* who) const
            {
                if (who->GetTypeId() == TYPE_ID_PLAYER)
                    return false;
                return true;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_flame_leviathan_defense_cannonAI(creature);
        }
};

class npc_flame_leviathan_defense_turret : public CreatureScript
{
    public:
        npc_flame_leviathan_defense_turret() : CreatureScript("npc_flame_leviathan_defense_turret") { }

        struct npc_flame_leviathan_defense_turretAI : public TurretAI
        {
            npc_flame_leviathan_defense_turretAI(Creature* creature) : TurretAI(creature)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetActive(true);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_flame_leviathan_defense_turretAI(creature);
        }
};

class npc_mechanolift : public CreatureScript
{
    public:
        npc_mechanolift() : CreatureScript("npc_mechanolift") { }

        struct npc_mechanoliftAI : public PassiveAI
        {
            npc_mechanoliftAI(Creature* creature) : PassiveAI(creature){}

            void Reset()
            {
                me->AddUnitMovementFlag(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING);
                me->SetVisible(true);
            }

            void DamageTaken(Unit* /*who*/, uint32 &damage)
            {
                if (damage >= me->GetHealth())
                {
                    if (Creature* liquid = DoSummon(NPC_LIQUID, me, 0, 190*IN_MILLISECONDS, TEMPSUMMON_TIMED_DESPAWN))
                    {
                        float x, y, z;
                        me->GetPosition(x, y, z);
                        z = me->GetMap()->GetHeight(me->GetPhaseMask(), x, y, MAX_HEIGHT);

                        liquid->SetCanFly(true);
                        liquid->GetMotionMaster()->MovePoint(0, x, y, z);
                    }

                    me->SetVisible(false);
                    me->RemoveUnitMovementFlag(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING);
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_mechanoliftAI(creature);
        }
};

class npc_liquid_pyrite : public CreatureScript
{
    public:
        npc_liquid_pyrite() : CreatureScript("npc_liquid_pyrite") { }

        struct npc_liquid_pyriteAI : public PassiveAI
        {
            npc_liquid_pyriteAI(Creature* creature) : PassiveAI(creature) { }

			uint32 DespawnTimer;

            void Reset()
            {
                DoCast(me, SPELL_LIQUID_PYRITE, true);
                me->SetDisplayId(28476);
                DespawnTimer = 5*IN_MILLISECONDS;
            }

            void MovementInform(uint32 type, uint32 /*id*/)
            {
                if (type == POINT_MOTION_TYPE)
                {
                    DoCast(me, SPELL_DUSTY_EXPLOSION, true);
                    DoCast(me, SPELL_DUST_CLOUD_IMPACT, true);
                    me->SetDisplayId(28783);
                }
            }

            void DamageTaken(Unit* /*who*/, uint32& damage)
            {
                damage = 0;
            }

            void UpdateAI(uint32 const diff)
            {
                if (DespawnTimer <= diff)
                {
                    if (me->GetVehicle())
					{
                        me->DisappearAndDie();
					}
                    DespawnTimer = 5*IN_MILLISECONDS;
                }
                else DespawnTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_liquid_pyriteAI(creature);
        }
};

class npc_pool_of_tar : public CreatureScript
{
    public:
        npc_pool_of_tar() : CreatureScript("npc_pool_of_tar") { }

        struct npc_pool_of_tarAI : public PassiveAI
        {
            npc_pool_of_tarAI(Creature* creature) : PassiveAI(creature)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
                DoCast(me, SPELL_TAR_PASSIVE, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
            }

            void DamageTaken(Unit* /*who*/, uint32& damage)
            {
                damage = 0;
            }

            void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
            {
                if ((spell->Id == 65044 || spell->Id == 65045) && !me->HasAura(SPELL_BLAZE))
                    DoCast(me, SPELL_BLAZE, true);
            }

            void UpdateAI(uint32 const /*diff*/) { }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_pool_of_tarAI(creature);
        }
};

class npc_thorims_hammer : public CreatureScript
{
    public:
        npc_thorims_hammer() : CreatureScript("npc_thorims_hammer") { }

        struct npc_thorims_hammerAI : public QuantumBasicAI
        {
            npc_thorims_hammerAI(Creature* creature) : QuantumBasicAI(creature)
            {
				SetCombatMovement(false);
                me->SetActive(true);
                me->SetDisplayId(me->GetCreatureTemplate()->Modelid2);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_PASSIVE);
            }

			uint32 Action;
            uint32 timer;

            void Reset()
            {
                timer = urand(2, 10) *IN_MILLISECONDS;
                Action = 1;
            }

            void UpdateAI(uint32 const diff)
            {
                if (timer <= diff)
                {
                    switch (Action)
                    {
                        case 1:
                            me->AddAura(SPELL_LIGHTNING_SKYBEAM, me);
                            ++Action;
                            timer = 4*IN_MILLISECONDS;
                            break;
                        case 2:
                            if (Creature* trigger = DoSummonFlyer(NPC_THORIMS_HAMMER, me, 50.0f, 0, 10*IN_MILLISECONDS, TEMPSUMMON_TIMED_DESPAWN))
                            {
                                trigger->SetDisplayId(trigger->GetCreatureTemplate()->Modelid2);
                                trigger->CastSpell(me, SPELL_THORIMS_HAMMER, true);
                            }
                            ++Action;
                            timer = 4*IN_MILLISECONDS;
                            break;
                        case 3:
                            me->RemoveAllAuras();
                            timer = 30*IN_MILLISECONDS;
                            break;
                    }
                }
                else
                    timer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_thorims_hammerAI(creature);
        }
};

class npc_mimirons_inferno : public CreatureScript
{
    public:
        npc_mimirons_inferno() : CreatureScript("npc_mimirons_inferno") { }

        struct npc_mimirons_infernoAI : public QuantumBasicAI
        {
            npc_mimirons_infernoAI(Creature* creature) : QuantumBasicAI(creature)
            {
                me->SetActive(true);
                me->SetDisplayId(me->GetCreatureTemplate()->Modelid2);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->AddAura(SPELL_RED_SKYBEAM, me);
                me->SetReactState(REACT_PASSIVE);
            }

			uint32 InfernoTimer;
            uint8 Count;
            bool PointReached;

            void Reset()
            {
				Count = 2;
                InfernoTimer = 2000;
                PointReached = true;
            }

            void MovementInform(uint32 type, uint32 id)
            {
				if (type != POINT_MOTION_TYPE || id != Count)
					return;

				if (++Count > 4)
					Count = 1;

				PointReached = true;
			}

            void UpdateAI(uint32 const diff)
            {
                if (PointReached)
                {
                    PointReached = false;
                    me->GetMotionMaster()->MovePoint(Count, Misc[Count]);
                }

                if (InfernoTimer <= diff)
                {
                    if (Creature* trigger = DoSummonFlyer(NPC_MIMIRONS_INFERNO, me, 50.0f, 0, 40*IN_MILLISECONDS, TEMPSUMMON_TIMED_DESPAWN))
                    {
                        trigger->SetDisplayId(trigger->GetCreatureTemplate()->Modelid2);
						trigger->CastSpell(me, SPELL_MIMIRONS_INFERNO, true); // Stil Bugged
                        InfernoTimer = 2*IN_MILLISECONDS;
                    }
                }
                else
                    InfernoTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_mimirons_infernoAI(creature);
        }
};

class npc_hodirs_fury : public CreatureScript
{
    public:
        npc_hodirs_fury() : CreatureScript("npc_hodirs_fury") { }

        struct npc_hodirs_furyAI : public QuantumBasicAI
        {
            npc_hodirs_furyAI(Creature* creature) : QuantumBasicAI(creature)
            {
                me->SetActive(true);
                me->SetDisplayId(me->GetCreatureTemplate()->Modelid2);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->SetReactState(REACT_PASSIVE);
                me->AddAura(SPELL_BLUE_SKYBEAM, me);
            }

			uint64 TargetGUID;
            uint32 timer;
            uint8 Action;

            void Reset()
            {
                TargetGUID = 0;
                Action = 3;
                timer = 3*IN_MILLISECONDS;
            }

            void UpdateAI(uint32 const diff)
            {
                if (timer <= diff)
                {
                    switch (Action)
                    {
                        case 1:
                        {
                            timer = 1*IN_MILLISECONDS;

                            Unit* target = ObjectAccessor::GetUnit(*me, TargetGUID);
                            if (target && me->IsInRange(target, 0.0f, 5.0f, false))
                            {
                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveIdle();
                                timer = 3*IN_MILLISECONDS;
                                TargetGUID = 0;
                                ++Action;
                            }
                            else if (!target)
                                Action = 3;
                            break;
                        }
                        case 2:
                            if (Creature* trigger = DoSummonFlyer(NPC_HODIRS_FURY, me, 50.0f, 0, 10*IN_MILLISECONDS, TEMPSUMMON_TIMED_DESPAWN))
                            {
                                trigger->SetDisplayId(trigger->GetCreatureTemplate()->Modelid2);
                                trigger->CastSpell(me, SPELL_HODIRS_FURY, true);
                                ++Action;
                            }
                            timer = 7*IN_MILLISECONDS;
                            break;
                        case 3:
                            DoAttackerAreaInCombat(me, 200.0f);
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 150.0f, true))
                            {
                                TargetGUID = target->GetGUID();
                                me->GetMotionMaster()->MoveFollow(target, 0.0f, 0.0f);
                                Action = 1;
                            }
                            timer = 5*IN_MILLISECONDS;
                    }
                }
                else
                    timer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_hodirs_furyAI(creature);
        }
};

class npc_freyas_ward : public CreatureScript
{
    public:
        npc_freyas_ward() : CreatureScript("npc_freyas_ward") { }

        struct npc_freyas_wardAI : public QuantumBasicAI
        {
            npc_freyas_wardAI(Creature* creature) : QuantumBasicAI(creature)
			{
				SetCombatMovement(false);
                me->SetActive(true);
                me->SetDisplayId(me->GetCreatureTemplate()->Modelid2);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                me->AddAura(SPELL_GREEN_SKYBEAM, me);
                me->SetReactState(REACT_PASSIVE);
            }

			uint32 SummonTimer;

            void Reset()
            {
                SummonTimer = 10*IN_MILLISECONDS;
            }

            void UpdateAI(uint32 const diff)
            {
                if (SummonTimer <= diff)
                {
                    if (Creature* trigger = DoSummonFlyer(NPC_FREYAS_WARD, me, 50.0f, 0, 10*IN_MILLISECONDS, TEMPSUMMON_TIMED_DESPAWN))
                    {
                        trigger->SetDisplayId(trigger->GetCreatureTemplate()->Modelid2);
                        trigger->CastSpell(me, SPELL_FREYAS_WARD, true);
                        SummonTimer = 30*IN_MILLISECONDS;
                    }
                }
                else SummonTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_freyas_wardAI(creature);
        }
};

class npc_freyas_ward_summon : public CreatureScript
{
    public:
        npc_freyas_ward_summon() : CreatureScript("npc_freyas_ward_summon") { }

        struct npc_freyas_ward_summonAI : public QuantumBasicAI
        {
            npc_freyas_ward_summonAI(Creature* creature) : QuantumBasicAI(creature)
            {
                me->SetActive(true);
            }

			uint32 LashTimer;

            void Reset()
            {
                LashTimer = urand(2, 8) *IN_MILLISECONDS;
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (LashTimer <= diff)
                {
                    DoCast(SPELL_LASH);
                    LashTimer = urand(8, 12) *IN_MILLISECONDS;
                }
                else
                    LashTimer -= diff;

                // no melee
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_freyas_ward_summonAI(creature);
        }
};

class npc_leviathan_player_vehicle : public CreatureScript
{
public:
	npc_leviathan_player_vehicle() : CreatureScript("npc_leviathan_player_vehicle") {}

	struct npc_leviathan_player_vehicleAI : public NullCreatureAI
	{
		npc_leviathan_player_vehicleAI(Creature* creature) : NullCreatureAI(creature)
		{
			instance = creature->GetInstanceScript();

			if (VehicleSeatEntry* vehSeat = const_cast<VehicleSeatEntry*>(sVehicleSeatStore.LookupEntry(3013)))
				vehSeat->m_flags &= ~VEHICLE_SEAT_FLAG_ALLOW_TURNING;
		}

		InstanceScript* instance;

		void PassengerBoarded(Unit* unit, int8 seat, bool apply)
		{
			if (!unit->ToPlayer() || seat != 0)
				return;

			if (apply)
				unit->CastSpell(me, SPELL_RIDE_VEHICLE, true);
			else
				me->RemoveAurasDueToSpell(SPELL_RIDE_VEHICLE);

			if (Creature* Leviathan = ObjectAccessor::GetCreature(*me, instance ? instance->GetData64(DATA_FLAME_LEVIATHAN) : 0))
			{
				if (Leviathan->IsInCombatActive())
				{
					me->SetInCombatWith(Leviathan);
					me->AddThreat(Leviathan, 1.0f);
					Leviathan->SetInCombatWith(me);
					Leviathan->AddThreat(me, 1.0f);

					if (apply)
						me->SetHealth(uint32(me->GetHealth() / 2));
				}
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_leviathan_player_vehicleAI(creature);
	}
};

class at_RX_214_repair_o_matic_station : public AreaTriggerScript
{
public:
	at_RX_214_repair_o_matic_station() : AreaTriggerScript("at_RX_214_repair_o_matic_station") { }

	bool OnTrigger(Player* player, const AreaTriggerEntry* /*at*/)
	{
		InstanceScript* instance = player->GetInstanceScript();

		if (Creature* vehicle = player->GetVehicleCreatureBase())
		{
			if (!vehicle->HasAura(SPELL_AUTO_REPAIR))
			{
				player->MonsterTextEmote(EMOTE_REPAIR, player->GetGUID(), true);
				player->CastSpell(vehicle, SPELL_AUTO_REPAIR, true);
				vehicle->SetFullHealth();

				if (Creature* leviathan = ObjectAccessor::GetCreature(*player, instance ? instance->GetData64(DATA_FLAME_LEVIATHAN) : 0))
					leviathan->AI()->SetData(DATA_UNBROKEN, 0); // set bool to false thats checked in leviathan getdata
			}
		}
		return true;
	}
};

class achievement_three_car_garage_demolisher : public AchievementCriteriaScript
{
public:
	achievement_three_car_garage_demolisher() : AchievementCriteriaScript("achievement_three_car_garage_demolisher") { }

	bool OnCheck(Player* source, Unit* /*target*/)
	{
		if (Creature* vehicle = source->GetVehicleCreatureBase())
		{
			if (vehicle->GetEntry() == NPC_SALVAGED_DEMOLISHER)
				return true;
		}
		return false;
	}
};

class achievement_three_car_garage_chopper : public AchievementCriteriaScript
{
public:
	achievement_three_car_garage_chopper() : AchievementCriteriaScript("achievement_three_car_garage_chopper") { }

	bool OnCheck(Player* source, Unit* /*target*/)
	{
		if (Creature* vehicle = source->GetVehicleCreatureBase())
		{
			if (vehicle->GetEntry() == NPC_SALVAGED_CHOPPER)
				return true;
		}
		return false;
	}
};

class achievement_three_car_garage_siege : public AchievementCriteriaScript
{
public:
	achievement_three_car_garage_siege() : AchievementCriteriaScript("achievement_three_car_garage_siege") { }

	bool OnCheck(Player* source, Unit* /*target*/)
	{
		if (Creature* vehicle = source->GetVehicleCreatureBase())
		{
			if (vehicle->GetEntry() == NPC_SALVAGED_SIEGE_ENGINE)
				return true;
		}
		return false;
	}
};

class achievement_shutout : public AchievementCriteriaScript
{
    public:
        achievement_shutout() : AchievementCriteriaScript("achievement_shutout") { }

        bool OnCheck(Player* /*source*/, Unit* target)
        {
            if (target)
                if (Creature* leviathan = target->ToCreature())
                    if (leviathan->AI()->GetData(DATA_SHUTOUT))
                        return true;

            return false;
        }
};

class achievement_unbroken : public AchievementCriteriaScript
{
    public:
        achievement_unbroken() : AchievementCriteriaScript("achievement_unbroken") { }

        bool OnCheck(Player* /*source*/, Unit* target)
        {
            if (target)
                if (Creature* leviathan = target->ToCreature())
                    if (leviathan->AI()->GetData(DATA_UNBROKEN))
                        return true;

            return false;
        }
};

class achievement_orbital_bombardment : public AchievementCriteriaScript
{
    public:
        achievement_orbital_bombardment() : AchievementCriteriaScript("achievement_orbital_bombardment") { }

        bool OnCheck(Player* /*source*/, Unit* target)
        {
            if (!target)
                return false;

            if (Creature* Leviathan = target->ToCreature())
                if (Leviathan->AI()->GetData(DATA_ORBIT_ACHIEVEMENTS) >= 1)
                    return true;

            return false;
        }
};

class achievement_orbital_devastation : public AchievementCriteriaScript
{
    public:
        achievement_orbital_devastation() : AchievementCriteriaScript("achievement_orbital_devastation") { }

        bool OnCheck(Player* /*source*/, Unit* target)
        {
            if (!target)
                return false;

            if (Creature* Leviathan = target->ToCreature())
                if (Leviathan->AI()->GetData(DATA_ORBIT_ACHIEVEMENTS) >= 2)
                    return true;

            return false;
        }
};

class achievement_nuked_from_orbit : public AchievementCriteriaScript
{
    public:
        achievement_nuked_from_orbit() : AchievementCriteriaScript("achievement_nuked_from_orbit") { }

        bool OnCheck(Player* /*source*/, Unit* target)
        {
            if (!target)
                return false;

            if (Creature* Leviathan = target->ToCreature())
                if (Leviathan->AI()->GetData(DATA_ORBIT_ACHIEVEMENTS) >= 3)
                    return true;

            return false;
        }
};

class achievement_orbit_uary : public AchievementCriteriaScript
{
    public:
        achievement_orbit_uary() : AchievementCriteriaScript("achievement_orbit_uary") { }

        bool OnCheck(Player* /*source*/, Unit* target)
        {
            if (!target)
                return false;

            if (Creature* Leviathan = target->ToCreature())
                if (Leviathan->AI()->GetData(DATA_ORBIT_ACHIEVEMENTS) == 4)
                    return true;

            return false;
        }
};

class spell_anti_air_rocket : public SpellScriptLoader
{
    public:
        spell_anti_air_rocket() : SpellScriptLoader("spell_anti_air_rocket") { }

        class spell_anti_air_rocket_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_anti_air_rocket_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_ANTI_AIR_ROCKET_DMG))
                    return false;
                return true;
            }

            void HandleTriggerMissile(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                if (const WorldLocation* pos = GetExplTargetDest())
                {
                    if (Creature* creature = GetCaster()->SummonCreature(NPC_WORLD_TRIGGER, *pos, TEMPSUMMON_TIMED_DESPAWN, 500))
                    {
                        creature->SetReactState(REACT_PASSIVE);
                        creature->SetCanFly(true);
                        creature->SetVisible(false);
                        std::list<Creature*> list;
                        GetCreatureListWithEntryInGrid(list, GetCaster(), NPC_MECHANOLIFT, 100.0f);

                        while (!list.empty())
                        {
                            std::list<Creature*>::iterator itr = list.begin();
                            std::advance(itr, urand(0, list.size()-1));

                            if ((*itr)->IsInBetween(GetCaster(), creature, 10.0f))
                            {
                                GetCaster()->CastSpell((*itr)->GetPositionX(), (*itr)->GetPositionY(), (*itr)->GetPositionZ(), SPELL_ANTI_AIR_ROCKET_DMG, true);
                                return;
                            }
                            list.erase(itr);
                        }
                    }
                }
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_anti_air_rocket_SpellScript::HandleTriggerMissile, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_anti_air_rocket_SpellScript();
        }
};

class FlameLeviathanPursuedTargetSelector
{
    public:
        bool operator() (Unit* unit)
        {
            if (unit->GetAreaId() != AREA_FORMATION_GROUNDS)
                return true;

            // ignore players loaded on leviathan seats
            if (unit->GetVehicleBase() && unit->GetVehicleBase()->GetEntry() == NPC_SEAT)
                return true;

            // if target is creature
            Creature* creatureTarget = unit->ToCreature();
            if (creatureTarget)
            {
                // needs to be one of the 3 vehicles
                if (creatureTarget->GetEntry() != NPC_SALVAGED_DEMOLISHER && creatureTarget->GetEntry() != NPC_SALVAGED_SIEGE_ENGINE && creatureTarget->GetEntry() != NPC_SALVAGED_CHOPPER)
                    return true;

                // must be a valid vehicle installation
                Vehicle* vehicle = creatureTarget->GetVehicleKit();
                if (!vehicle)
                    return true;

                // vehicle must be in use by player
                bool playerFound = false;
                for (SeatMap::const_iterator itr = vehicle->Seats.begin(); itr != vehicle->Seats.end() && !playerFound; ++itr)
                    if (IS_PLAYER_GUID(itr->second.Passenger))
                        playerFound = true;

                return !playerFound;
            }
            return false;
		}
};

class spell_pursued : public SpellScriptLoader
{
    public:
        spell_pursued() : SpellScriptLoader("spell_pursued") { }

        class spell_pursued_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pursued_SpellScript);

            bool Load()
            {
                _target = NULL;
                return GetCaster()->GetTypeId() == TYPE_ID_UNIT;
            }

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if (FlameLeviathanPursuedTargetSelector());
                if (unitList.empty())
                {
                    GetCaster()->ToCreature()->AI()->EnterEvadeMode();
                    return;
                }

                std::list<Unit*> tempList;
                for (std::list<Unit*>::iterator itr = unitList.begin(); itr != unitList.end(); ++itr)
                {
                    _target = *itr;

                    if (!_target->ToCreature() || _target->HasAura(SPELL_PURSUED))
                        continue;

                    if (_target->ToCreature()->GetEntry() == NPC_SALVAGED_SIEGE_ENGINE || _target->ToCreature()->GetEntry() == NPC_SALVAGED_DEMOLISHER)
                        tempList.push_back(_target);
                }

                if (tempList.empty())
                {
                    for (std::list<Unit*>::iterator itr = unitList.begin(); itr != unitList.end(); ++itr)
                    {
                        _target = *itr;

                        if (!_target->ToCreature() || _target->HasAura(SPELL_PURSUED))
                            continue;

                        if (_target->ToCreature()->GetEntry() == NPC_SALVAGED_CHOPPER)
                            tempList.push_back(_target);
                    }
                }

                if (!tempList.empty())
                {
                    _target = Quantum::DataPackets::SelectRandomContainerElement(tempList);
                    SetTarget(unitList);
                }
                else
                {
                    _target = Quantum::DataPackets::SelectRandomContainerElement(unitList);
                    SetTarget(unitList);
                }
            }

            void SetTarget(std::list<Unit*>& unitList)
            {
                unitList.clear();

                if (_target)
                    unitList.push_back(_target);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_pursued_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_pursued_SpellScript::SetTarget, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
            }

            Unit* _target;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pursued_SpellScript();
        }
};

class spell_flame_leviathan_flames : public SpellScriptLoader
{
    public:
        spell_flame_leviathan_flames() : SpellScriptLoader("spell_flame_leviathan_flames") { }

        class spell_flame_leviathan_flames_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_flame_leviathan_flames_SpellScript);

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (GetHitUnit()->HasAura(62297))
                    GetHitUnit()->RemoveAura(62297);
            }

            void SetDamage()
            {
                PreventHitDamage();
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_flame_leviathan_flames_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
                OnHit += SpellHitFn(spell_flame_leviathan_flames_SpellScript::SetDamage);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_flame_leviathan_flames_SpellScript();
        }
};

class spell_load_into_catapult : public SpellScriptLoader
{
    public:
        spell_load_into_catapult() : SpellScriptLoader("spell_load_into_catapult") { }

        class spell_load_into_catapult_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_load_into_catapult_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* owner = GetOwner()->ToUnit();
                if (!owner)
                    return;

                owner->CastSpell(owner, SPELL_PASSENGER_LOADED, true);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* owner = GetOwner()->ToUnit();
                if (!owner)
                    return;

                owner->RemoveAurasDueToSpell(SPELL_PASSENGER_LOADED);
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_load_into_catapult_AuraScript::OnApply, EFFECT_0, SPELL_AURA_CONTROL_VEHICLE, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_load_into_catapult_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_CONTROL_VEHICLE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_load_into_catapult_AuraScript();
        }
};

class spell_system_shutdown : public SpellScriptLoader
{
    public:
        spell_system_shutdown() : SpellScriptLoader("spell_system_shutdown") { }

        class spell_system_shutdown_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_system_shutdown_AuraScript);

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Creature* owner = GetOwner()->ToCreature();
                if (!owner)
                    return;

                //! This could probably in the SPELL_EFFECT_SEND_EVENT handler too:
                owner->AddUnitState(UNIT_STATE_STUNNED | UNIT_STATE_ROOT);
                owner->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
                owner->RemoveAurasDueToSpell(SPELL_GATHERING_SPEED);
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Creature* owner = GetOwner()->ToCreature();
                if (!owner)
                    return;

                owner->ClearUnitState(UNIT_STATE_STUNNED | UNIT_STATE_ROOT);
                owner->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_system_shutdown_AuraScript::OnApply, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
                OnEffectRemove += AuraEffectRemoveFn(spell_system_shutdown_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_system_shutdown_AuraScript();
        }
};

class spell_throw_passenger : public SpellScriptLoader
{
    public:
        spell_throw_passenger() : SpellScriptLoader("spell_throw_passenger") { }

        class spell_throw_passenger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_throw_passenger_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();

                if (caster && caster->GetVehicleKit())
				{
                    if (Unit* passenger = caster->GetVehicleKit()->GetPassenger(3))
                    {
                        passenger->ExitVehicle();

                        if (Creature* leviathan = caster->FindCreatureWithDistance(NPC_FLAME_LEVIATHAN, 120.0f))
                            leviathan->AI()->SetGUID(passenger->GetGUID());
                    }
				}
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_throw_passenger_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_throw_passenger_SpellScript();
        }
};

class spell_freyas_ward_summon : public SpellScriptLoader
{
    public:
        spell_freyas_ward_summon() : SpellScriptLoader("spell_freyas_ward_summon") { }

        class spell_freyas_ward_summon_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_freyas_ward_summon_SpellScript);

            void HandleDummy(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                if (Unit* caster = GetCaster())
				{
                    if (InstanceScript* instance = caster->GetInstanceScript())
					{
                        if (Creature* leviathan = ObjectAccessor::GetCreature(*caster, instance->GetData64(DATA_FLAME_LEVIATHAN)))
						{
                            for (uint8 i = 0; i < urand(3, 5); ++i)
                                leviathan->SummonCreature(NPC_WRITHING_LASHER, GetExplTargetDest()->GetPositionX(), GetExplTargetDest()->GetPositionY(), GetExplTargetDest()->GetPositionZ(), 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 3000);
						}
					}
				}
            }

            void HandleSummon(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);

                if (Unit* caster = GetCaster())
                    if (InstanceScript* instance = caster->GetInstanceScript())
                        if (Creature* leviathan = ObjectAccessor::GetCreature(*caster, instance->GetData64(DATA_FLAME_LEVIATHAN)))
                            leviathan->SummonCreature(NPC_WARD_OF_LIFE, GetExplTargetDest()->GetPositionX(), GetExplTargetDest()->GetPositionY(),
                            GetExplTargetDest()->GetPositionZ(), 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 3000);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_freyas_ward_summon_SpellScript::HandleDummy, EFFECT_1, SPELL_EFFECT_DUMMY);
                OnEffectHit += SpellEffectFn(spell_freyas_ward_summon_SpellScript::HandleSummon, EFFECT_2, SPELL_EFFECT_SUMMON);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_freyas_ward_summon_SpellScript();
        }
};

class FlameVentsTargetSelector
{
    public:
        bool operator() (Unit* unit)
        {
            if (unit->GetTypeId() != TYPE_ID_PLAYER)
            {
                if (unit->ToCreature()->GetEntry() == NPC_SALVAGED_SIEGE_ENGINE
					|| unit->ToCreature()->GetEntry() == NPC_SALVAGED_CHOPPER
					|| unit->ToCreature()->GetEntry() == NPC_SALVAGED_DEMOLISHER)
                    return false;

                if (!unit->ToCreature()->IsPet())
                    return true;
            }
            return unit->GetVehicle();
		}
};

class spell_flame_leviathan_flame_vents : public SpellScriptLoader
{
    public:
        spell_flame_leviathan_flame_vents() : SpellScriptLoader("spell_flame_leviathan_flame_vents") { }

        class spell_flame_leviathan_flame_vents_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_flame_leviathan_flame_vents_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if (FlameVentsTargetSelector());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_flame_leviathan_flame_vents_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_flame_leviathan_flame_vents_SpellScript();
        }
};

class spell_shield_generator : public SpellScriptLoader
{
    public:
        spell_shield_generator() : SpellScriptLoader("spell_shield_generator") { }

        class spell_shield_generator_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_shield_generator_AuraScript);

            void CalculateAmount(AuraEffect const* aurEff, int32 & amount, bool & /*canBeRecalculated*/)
            {
                if (Unit* caster = GetCaster())
				{
                    if (Unit* siege = caster->GetVehicleBase())
                        amount = int32(siege->CountPctFromMaxHealth(HEALTH_PERCENT_15));
				}
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_shield_generator_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_shield_generator_AuraScript();
        }
};

class spell_demolisher_grab_pyrite : public SpellScriptLoader
{
public:
	spell_demolisher_grab_pyrite() : SpellScriptLoader("spell_demolisher_grab_pyrite") {}

	class spell_demolisher_grab_pyrite_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_demolisher_grab_pyrite_SpellScript);

		bool Load()
		{
			return GetCaster()->GetTypeId() == TYPE_ID_PLAYER;
		}

		void HandleScript(SpellEffIndex effIndex)
		{
			PreventHitDefaultEffect(effIndex);
			Player* player = GetCaster()->ToPlayer();

			if (Creature* vehicle = player->GetVehicleCreatureBase())
			{
				if (!vehicle->HasAura(SPELL_AUTO_REPAIR))
				{
					player->MonsterTextEmote(EMOTE_PYRITE, player->GetGUID(), true);
					player->CastSpell(vehicle, SPELL_AUTO_REPAIR, true);
				}
			}
		}

		void Register()
		{
			OnEffectHitTarget += SpellEffectFn(spell_demolisher_grab_pyrite_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_demolisher_grab_pyrite_SpellScript();
	}
};

void AddSC_boss_flame_leviathan()
{
    new boss_flame_leviathan();
    new npc_flame_leviathan_defense_turret();
    new npc_flame_leviathan_defense_cannon();
    new npc_mechanolift();
    new npc_liquid_pyrite();
    new npc_pool_of_tar();
    new npc_thorims_hammer();
	//new npc_mimirons_inferno();
    new npc_hodirs_fury();
    new npc_freyas_ward();
    new npc_freyas_ward_summon();
    new npc_leviathan_player_vehicle();
    new at_RX_214_repair_o_matic_station();
	// Achievements
    new achievement_three_car_garage_demolisher();
    new achievement_three_car_garage_chopper();
    new achievement_three_car_garage_siege();
    new achievement_shutout();
    new achievement_unbroken();
    new achievement_orbital_bombardment();
    new achievement_orbital_devastation();
    new achievement_nuked_from_orbit();
    new achievement_orbit_uary();
	// Spells
    new spell_anti_air_rocket();
    new spell_pursued();
    new spell_flame_leviathan_flames();
    new spell_load_into_catapult();
    new spell_system_shutdown();
    new spell_throw_passenger();
    new spell_freyas_ward_summon();
    new spell_flame_leviathan_flame_vents();
    new spell_shield_generator();
	new spell_demolisher_grab_pyrite();
}