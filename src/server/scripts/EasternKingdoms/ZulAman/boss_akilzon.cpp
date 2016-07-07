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
#include "Weather.h"
#include "zulaman.h"

#define SE_LOC_X_MAX 400
#define SE_LOC_X_MIN 335
#define SE_LOC_Y_MAX 1435
#define SE_LOC_Y_MIN 1370

enum Texts
{
	SAY_AGGRO      = -1534064,
	SAY_DEATH      = -1534065,
	SAY_SLAY_1     = -1534066,
	SAY_SLAY_2     = -1534067,
	SAY_SUMMON     = -1534068,
	SAY_ENRAGE     = -1534069,
};

enum Spells
{
    SPELL_STATIC_DISRUPTION     = 43622,
    SPELL_STATIC_VISUAL         = 45265,
    SPELL_CALL_LIGHTNING        = 43661, //Missing timer
    SPELL_GUST_OF_WIND          = 43621,
    SPELL_ELECTRICAL_STORM      = 43648,
    SPELL_BERSERK               = 45078,
    SPELL_ELECTRICAL_OVERLOAD   = 43658,
    SPELL_EAGLE_SWOOP           = 44732
};

class boss_akilzon : public CreatureScript
{
    public:
		boss_akilzon(): CreatureScript("boss_akilzon") {}

        struct boss_akilzonAI : public QuantumBasicAI
        {
            boss_akilzonAI(Creature* creature) : QuantumBasicAI(creature)
            {
				instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;

            uint64 BirdGUIDs[8];
            uint64 TargetGUID;
            uint64 CycloneGUID;
            uint64 CloudGUID;

            uint32 StaticDisruptionTimer;
            uint32 GustOfWindTimer;
            uint32 CallLightingTimer;
            uint32 ElectricalStormTimer;
            uint32 SummonEaglesTimer;
            uint32 EnrageTimer;

            uint32 StormCount;
            uint32 StormSequenceTimer;

            bool isRaining;

            void Reset()
            {
                if (instance)
                    instance->SetData(DATA_AKILZON_EVENT, NOT_STARTED);

                StaticDisruptionTimer = urand(10000, 20000); //10 to 20 seconds (bosskillers)
                GustOfWindTimer = urand(20000, 30000); //20 to 30 seconds(bosskillers)
                CallLightingTimer = urand(10000, 20000); //totaly random timer. can't find any info on this
                ElectricalStormTimer = 60000; //60 seconds(bosskillers)
                EnrageTimer = 10*MINUTE*IN_MILLISECONDS; //10 minutes till enrage(bosskillers)
                SummonEaglesTimer = 99999;

                TargetGUID = 0;
                CloudGUID = 0;
                CycloneGUID = 0;
                DespawnSummons();
                for (uint8 i = 0; i < 8; ++i)
                    BirdGUIDs[i] = 0;

                StormCount = 0;
                StormSequenceTimer = 0;

                isRaining = false;

                SetWeather(WEATHER_STATE_FINE, 0.0f);
            }

            void EnterToBattle(Unit* /*who*/)
            {
				DoSendQuantumText(SAY_AGGRO, me);
				DoZoneInCombat();

                if (instance)
                    instance->SetData(DATA_AKILZON_EVENT, IN_PROGRESS);
            }

            void JustDied(Unit* /*Killer*/)
            {
				DoSendQuantumText(SAY_DEATH, me);

                if (instance)
                    instance->SetData(DATA_AKILZON_EVENT, DONE);

                DespawnSummons();
            }

            void KilledUnit(Unit* /*victim*/)
            {
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

            void DespawnSummons()
            {
                for (uint8 i = 0; i < 8; ++i)
                {
                    Unit* bird = Unit::GetUnit(*me, BirdGUIDs[i]);
                    if (bird && bird->IsAlive())
                    {
                        bird->SetVisible(false);
                        bird->setDeathState(JUST_DIED);
                    }
                }
            }

            void SetWeather(uint32 weather, float grade)
            {
                Map* map = me->GetMap();
                if (!map->IsDungeon())
                    return;

                WorldPacket data(SMSG_WEATHER, (4+4+4));
                data << uint32(weather) << float(grade) << uint8(0);

                map->SendToPlayers(&data);
            }

            void HandleStormSequence(Unit* Cloud) // 1: begin, 2-9: tick, 10: end
            {
                if (StormCount < 10 && StormCount > 1)
                {
                    // deal damage
                    int32 bp0 = 800;
                    for (uint8 i = 2; i < StormCount; ++i)
                        bp0 *= 2;

                    CellCoord p(Trinity::ComputeCellCoord(me->GetPositionX(), me->GetPositionY()));
                    Cell cell(p);
                    cell.SetNoCreate();

                    std::list<Unit*> tempUnitMap;

                    {
                        Trinity::AnyAoETargetUnitInObjectRangeCheck u_check(me, me, SIZE_OF_GRIDS);
                        Trinity::UnitListSearcher<Trinity::AnyAoETargetUnitInObjectRangeCheck> searcher(me, tempUnitMap, u_check);

                        TypeContainerVisitor<Trinity::UnitListSearcher<Trinity::AnyAoETargetUnitInObjectRangeCheck>, WorldTypeMapContainer > world_unit_searcher(searcher);
                        TypeContainerVisitor<Trinity::UnitListSearcher<Trinity::AnyAoETargetUnitInObjectRangeCheck>, GridTypeMapContainer >  grid_unit_searcher(searcher);

                        cell.Visit(p, world_unit_searcher, *me->GetMap(), *me, SIZE_OF_GRIDS);
                        cell.Visit(p, grid_unit_searcher, *me->GetMap(), *me, SIZE_OF_GRIDS);
                    }
                    //dealdamege
                    for (std::list<Unit*>::const_iterator i = tempUnitMap.begin(); i != tempUnitMap.end(); ++i)
                    {
                        if (!Cloud->IsWithinDist(*i, 6, false))
                            Cloud->CastCustomSpell(*i, 43137, &bp0, NULL, NULL, true, 0, 0, me->GetGUID());
                    }
                    // visual
                    float x, y, z;
                    z = me->GetPositionZ();
                    for (uint8 i = 0; i < 5+rand()%5; ++i)
                    {
                        x = 343.0f+rand()%60;
                        y = 1380.0f+rand()%60;
                        if (Unit* trigger = me->SummonTrigger(x, y, z, 0, 2000))
                        {
                            trigger->SetCurrentFaction(35);
                            trigger->SetMaxHealth(100000);
                            trigger->SetHealth(100000);
                            trigger->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            if (Cloud)
                                Cloud->CastCustomSpell(trigger, /*43661*/43137, &bp0, NULL, NULL, true, 0, 0, Cloud->GetGUID());
                        }
                    }
                }
                ++StormCount;
                if (StormCount > 10)
                {
                    StormCount = 0; // finish
                    SummonEaglesTimer = 5000;
                    me->InterruptNonMeleeSpells(false);
                    CloudGUID = 0;
                    if (Cloud)
                        Cloud->DealDamage(Cloud, Cloud->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    SetWeather(WEATHER_STATE_FINE, 0.0f);
                    isRaining = false;
                }
                StormSequenceTimer = 1000;
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (StormCount)
                {
                    Unit* target = Unit::GetUnit(*me, CloudGUID);
                    if (!target || !target->IsAlive())
                    {
                        EnterEvadeMode();
                        return;
                    }
                    else if (Unit* Cyclone = Unit::GetUnit(*me, CycloneGUID))
                        Cyclone->CastSpell(target, 25160, true); // keep casting or...

                    if (StormSequenceTimer <= diff)
                        HandleStormSequence(target);
                    else
                        StormSequenceTimer -= diff;

                    return;
                }

                if (EnrageTimer <= diff)
                {
					DoSendQuantumText(SAY_ENRAGE, me);
                    DoCast(me, SPELL_BERSERK, true);
                    EnrageTimer = 600000;
                }
				else EnrageTimer -= diff;

                if (StaticDisruptionTimer <= diff)
                {
                    Unit* target = SelectTarget(TARGET_RANDOM, 1);
                    if (!target) target = me->GetVictim();
                    TargetGUID = target->GetGUID();
                    DoCast(target, SPELL_STATIC_DISRUPTION, false);
                    me->SetInFront(me->GetVictim());
                    StaticDisruptionTimer = (10+rand()%8)*1000; // < 20s

                    /*if (float dist = me->IsWithinDist3d(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 5.0f) dist = 5.0f;
                    SDisruptAOEVisualTimer = 1000 + floor(dist / 30 * 1000.0f);*/
                }
				else StaticDisruptionTimer -= diff;

                if (GustOfWindTimer <= diff)
                {
                    Unit* target = SelectTarget(TARGET_RANDOM, 1);
                    if (!target) target = me->GetVictim();
                    DoCast(target, SPELL_GUST_OF_WIND);
                    GustOfWindTimer = urand(20, 30) * 1000; //20 to 30 seconds(bosskillers)
                }
				else GustOfWindTimer -= diff;

                if (CallLightingTimer <= diff)
                {
                    DoCastVictim(SPELL_CALL_LIGHTNING);
                    CallLightingTimer = urand(12, 17) * 1000; //totaly random timer. can't find any info on this
                }
				else CallLightingTimer -= diff;

                if (!isRaining && ElectricalStormTimer < uint32(8000 + rand() % 5000))
                {
                    SetWeather(WEATHER_STATE_HEAVY_RAIN, 0.9999f);
                    isRaining = true;
                }

                if (ElectricalStormTimer <= diff)
				{
                    Unit* target = SelectTarget(TARGET_RANDOM, 0, 50, true);
                    if (!target)
                    {
                        EnterEvadeMode();
                        return;
                    }
                    target->CastSpell(target, 44007, true);//cloud visual
                    DoCast(target, SPELL_ELECTRICAL_STORM, false);//storm cyclon + visual
                    float x, y, z;
                    target->GetPosition(x, y, z);
                    if (target)
                    {
                        target->SetUnitMovementFlags(MOVEMENTFLAG_DISABLE_GRAVITY);
                        target->MonsterMoveWithSpeed(x, y, me->GetPositionZ()+15, 0);
                    }
                    Unit* Cloud = me->SummonTrigger(x, y, me->GetPositionZ()+16, 0, 15000);
                    if (Cloud)
                    {
                        CloudGUID = Cloud->GetGUID();
                        Cloud->SetUnitMovementFlags(MOVEMENTFLAG_DISABLE_GRAVITY);
                        Cloud->StopMoving();
                        Cloud->SetObjectScale(1.0f);
                        Cloud->SetCurrentFaction(35);
                        Cloud->SetMaxHealth(9999999);
                        Cloud->SetHealth(9999999);
                        Cloud->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    }
                    ElectricalStormTimer = 60000; //60 seconds(bosskillers)
                    StormCount = 1;
                    StormSequenceTimer = 0;
                }
				else ElectricalStormTimer -= diff;

                if (SummonEaglesTimer <= diff)
                {
					DoSendQuantumText(SAY_SUMMON, me);

                    float x, y, z;
                    me->GetPosition(x, y, z);

                    for (uint8 i = 0; i < 8; ++i)
                    {
                        Unit* bird = Unit::GetUnit(*me, BirdGUIDs[i]);
                        if (!bird) //they despawned on die
                        {
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                            {
                                x = target->GetPositionX() + irand(-10, 10);
                                y = target->GetPositionY() + irand(-10, 10);
                                z = target->GetPositionZ() + urand(16, 20);
                                if (z > 95)
                                    z = 95.0f - urand(0, 5);
                            }

                            if (Creature* creature = me->SummonCreature(NPC_SOARING_EAGLE, x, y, z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
                            {
                                creature->AddThreat(me->GetVictim(), 1.0f);
                                creature->AI()->AttackStart(me->GetVictim());
                                BirdGUIDs[i] = creature->GetGUID();
                            }
                        }
                    }
                    SummonEaglesTimer = 999999;
                }
				else SummonEaglesTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_akilzonAI(creature);
        }
};

class mob_akilzon_eagle : public CreatureScript
{
    public:
        mob_akilzon_eagle() : CreatureScript("mob_akilzon_eagle"){}

        struct mob_akilzon_eagleAI : public QuantumBasicAI
        {
            mob_akilzon_eagleAI(Creature* creature) : QuantumBasicAI(creature) {}

            uint32 EagleSwoopTimer;
            bool arrived;
            uint64 TargetGUID;

            void Reset()
            {
                EagleSwoopTimer = urand(5000, 10000);
                arrived = true;
                TargetGUID = 0;
                me->SetUnitMovementFlags(MOVEMENTFLAG_DISABLE_GRAVITY);
            }

            void EnterToBattle(Unit* /*who*/)
			{
				DoZoneInCombat();
			}

            void MoveInLineOfSight(Unit* /*who*/) {}

            void MovementInform(uint32, uint32)
            {
                arrived = true;
                if (TargetGUID)
                {
                    if (Unit* target = Unit::GetUnit(*me, TargetGUID))
                        DoCast(target, SPELL_EAGLE_SWOOP, true);
                    TargetGUID = 0;
                    me->SetSpeed(MOVE_RUN, 1.2f);
                    EagleSwoopTimer = urand(5000, 10000);
                }
            }

            void UpdateAI(const uint32 diff)
            {
                if (EagleSwoopTimer <= diff)
                    EagleSwoopTimer = 0;
                else
                    EagleSwoopTimer -= diff;

                if (arrived)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                    {
                        float x, y, z;
                        if (EagleSwoopTimer)
                        {
                            x = target->GetPositionX() + irand(-10, 10);
                            y = target->GetPositionY() + irand(-10, 10);
                            z = target->GetPositionZ() + urand(10, 15);
                            if (z > 95)
                                z = 95.0f - urand(0, 5);
                        }
                        else
                        {
                            target->GetContactPoint(me, x, y, z);
                            z += 2;
                            me->SetSpeed(MOVE_RUN, 5.0f);
                            TargetGUID = target->GetGUID();
                        }
                        me->GetMotionMaster()->MovePoint(0, x, y, z);
                        arrived = false;
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new mob_akilzon_eagleAI(creature);
        }
};

void AddSC_boss_akilzon()
{
    new boss_akilzon();
    new mob_akilzon_eagle();
}