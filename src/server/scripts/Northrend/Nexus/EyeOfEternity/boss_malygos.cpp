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
#include "QuantumGossip.h"
#include "Vehicle.h"
#include "eye_of_eternity.h"

#define EMOTE_SURGE "%s fixes his eyes on you!"

#define FLOOR_Z  268.17f

enum Yells
{
    // Intro
    SAY_INTRO_1                     =  -1616000,
    SAY_INTRO_2                     =  -1616001,
    SAY_INTRO_3                     =  -1616002,
    SAY_INTRO_4                     =  -1616003,
    SAY_INTRO_5                     =  -1616004,
    // Phase 1
    SAY_PHASE1_AGGRO                =  -1616005,
    SAY_MAGIC_BLAST                 =  -1616006,
    SAY_SPARK_SUMMON                =  -1616035,
    SAY_SPARK_BUFFED                =  -1616007,
    SAY_PHASE1_SLAY_1               =  -1616008,
    SAY_PHASE1_SLAY_2               =  -1616009,
    SAY_PHASE1_SLAY_3               =  -1616010,
    SAY_PHASE1_END                  =  -1616012,
    // Phase 2
    SAY_PHASE2_AGGRO                =  -1616013,
    SAY_BREATH_ATTACK               =  -1616014,
	SAY_BREATH_ANNOUNCE             =  -1616015,
    SAY_ANTI_MAGIC_SHELL            =  -1616016,
    SAY_PHASE2_SLAY_1               =  -1616020,
    SAY_PHASE2_SLAY_2               =  -1616021,
    SAY_PHASE2_SLAY_3               =  -1616022,
    SAY_PHASE2_END                  =  -1616017,
    // Phase 3
    SAY_PHASE3_INTRO                =  -1616018,
    SAY_PHASE3_AGGRO                =  -1616019,
    SAY_PHASE3_SLAY_1               =  -1616023,
    SAY_PHASE3_SLAY_2               =  -1616024,
    SAY_PHASE3_SLAY_3               =  -1616025,
    SAY_SURGE_OF_POWER              =  -1616026,
    SAY_PHASE3_CAST_1               =  -1616027,
    SAY_PHASE3_CAST_2               =  -1616028,
    SAY_PHASE3_CAST_3               =  -1616029,
    SAY_DEATH                       =  -1616030,
    SAY_OUTRO_1                     =  -1616031,
    SAY_OUTRO_2                     =  -1616032,
    SAY_OUTRO_3                     =  -1616033,
    SAY_OUTRO_4                     =  -1616034,
};

enum Spells
{
	// Intro Spells
	SPELL_RANDOM_PORTAL              = 56047,
	SPELL_PORTAL_BEAM                = 56046,
	SPELL_PORTAL_OPENED              = 61236,
	SPELL_PORTAL_VISUAL_CLOSED       = 55949,
	SPELL_RIDE_VEHICLE               = 61421,
	SPELL_FLIGHT                     = 61453,
	SPELL_PERIODIC_BREATH            = 65137,
	// Phase 1
    SPELL_IRIS_VISUAL                = 61012,
    SPELL_ARCANE_BREATH_10           = 56272,
    SPELL_ARCANE_BREATH_25           = 60072,
    SPELL_ARCANE_STORM_10            = 61693,
    SPELL_ARCANE_STORM_25            = 61694,
    SPELL_VORTEX                     = 56105,
	SPELL_VORTEX_SPAWN               = 59670,
    SPELL_VORTEX_VISUAL              = 55873,
    SPELL_VORTEX_PLAYER              = 55853,
	SPELL_SUMMON_POWER_SPARK         = 56140,
    SPELL_POWER_SPARK                = 56152,
    SPELL_POWER_SPARK_VISUAL         = 55845,
    SPELL_POWER_SPARK_PLAYERS        = 55852,
	//Phase 2
	SPELL_TELEPORT_VISUAL_ONLY       = 41232,
    SPELL_ADD_RIDE_VEHICLE           = 61421,
    SPELL_ARCANE_BOMB                = 56430,
    SPELL_ARCANE_BOMB_KNOCKBACK      = 56431,
    SPELL_ARCANE_OVERLOAD            = 56432,
    SPELL_DEEP_BREATH                = 56505,
    SPELL_ARCANE_SHOCK_10            = 57058,
    SPELL_ARCANE_SHOCK_25            = 60073,
    SPELL_HASTE                      = 57060,
    SPELL_ARCANE_BARRAGE             = 56397,
    SPELL_ARCANE_BARRAGE_DMG         = 63934,
    SPELL_DESTROY_PLATFORM_CHANNEL   = 58842,
    SPELL_DESTROY_PLATFORM_BOOM      = 59084,
    SPELL_DESTROY_PLATFORM_EVENT     = 59099,
    // Phase 3
    SPELL_ARCANE_PULSE               = 57432,
    SPELL_STATIC_FIELD               = 57428,
    SPELL_STATIC_FIELD_MISSLE        = 57430,
    SPELL_SURGE_OF_POWER_10          = 57407,
    SPELL_SURGE_OF_POWER_25          = 60936,
    //SPELL_POWER_MARKER               = 60939,
    SPELL_RIDE_RED_DRAGON_BUDDY      = 56071,
    SPELL_ALEXSTRASZAS_GIFT_VISUAL   = 61023,
    SPELL_ALEXSTRASZAS_GIFT_BEAM     = 61028,
    SPELL_ENRAGE                     = 47008,
    SPELL_ROOT                       = 18373,
	SPELL_PARACHUTE                  = 66656,
};

enum Phase
{
    PHASE_NONE    = 0,
    PHASE_GROUND  = 1,
    PHASE_VORTEX  = 2,
    PHASE_ADDS    = 3,
    PHASE_DRAGONS = 4,
    PHASE_IDLE    = 5,
};

enum Achievements
{
	ACHIEVEMENT_DENYIN_THE_SCION_10 = 2148,
	ACHIEVEMENT_DENYIN_THE_SCION_25 = 2149,
};

enum Events
{
    EVENT_IDLE            = 1,
    EVENT_STORM           = 2,
    EVENT_SPARK           = 3,
    EVENT_ENRAGE          = 4,
    EVENT_VORTEX          = 5,
    EVENT_VORTEX_FLY_UP   = 6,
    EVENT_VORTEX_FLY_DOWN = 7,
    EVENT_OVERLOAD        = 8,
    EVENT_STATIC_FIELD    = 9,
    EVENT_PULSE           = 10,
    EVENT_BREATH          = 11,
    EVENT_SURGE_OF_POWER  = 12,
    EVENT_CHECK_PLAYER    = 13,
};

enum Action
{
    ACTION_START        = 1,
    ACTION_VORTEX       = 2,
    ACTION_SPARK        = 3,
    ACTION_OVERLOAD     = 4,
    ACTION_DEEP_BREATH  = 5,
    ACTION_SPAWN_ADDS   = 6,
    ACTION_SPAWN_MOUNTS = 7,
    ACTION_MOUNT_ALL    = 8,
    ACTION_CAST_SURGE   = 9,
    ACTION_CHECK_PLAYER = 10,
    ACTION_MOVE_SPARK   = 11,
    ACTION_STOP_SPARK   = 12,
    ACTION_BUFF         = 13,
};

enum MovePoints
{
    POINT_START         = 1,
    POINT_VORTEX        = 2,
    POINT_FLY_DOWN      = 3,
    POINT_PHASE_2       = 4,
    POINT_DESTROY_FLOOR = 5,
    POINT_PHASE_3       = 6,
};

enum Light
{
	LIGHT_DEFAULT      = 1773,
    LIGHT_RUNES        = 1824,
    LIGHT_SPACE_FLIGHT = 1823,
    LIGHT_CLOUDY       = 1822,
    LIGHT_CLOUDY_RUNES = 1825,
};

static Position Locations[] =
{
    {778.01f, 1276.01f, 268.2f, 2.3143f},
    {754.20f, 1301.68f, 302.5f, 2.3143f},
    {754.20f, 1301.68f, 285.5f, 2.3143f},
    {754.20f, 1301.68f, 320.5f, 2.3143f},
    {754.20f, 1301.68f, 225.0f, 2.3143f},
    {754.35f, 1300.87f, 270.2f, 0.0f},
};

const Position NexusLordSpawnLocations[] =
{
	{754.744f, 1309.78f, 266.171f, 2.24391f},
	{758.012f, 1320.95f, 266.171f, 3.70632f},
	{748.609f, 1305.71f, 266.171f, 1.41924f},
	{739.943f, 1308.23f, 266.171f, 0.219147f},
	{735.86f, 1314.44f, 266.171f, 6.07429f},
	{750.69f, 1325.11f, 266.171f, 4.55455f},
};

const Position ScionOfEnetrnitySpawnLocations[] =
{
	{738.569f, 1341.47f, 285.741f, 5.1595f},
	{791.359f, 1289.83f, 297.212f, 2.77887f},
	{778.467f, 1334.09f, 290.064f, 4.0198f},
	{717.768f, 1311.63f, 288.309f, 5.99537f},
	{722.885f, 1282.82f, 284.534f, 0.571408f},
	{757.173f, 1266.14f, 299.746f, 1.65696f},
};

static Position SparkLocations[] =
{
    {652.417f, 1200.52f, 295.972f, 0.78539f},
    {847.670f, 1408.05f, 295.972f, 3.97935f},
    {647.675f, 1403.80f, 295.972f, 5.53269f},
    {843.182f, 1215.42f, 295.972f, 2.35619f},
};

class boss_malygos : public CreatureScript
{
    public:
        boss_malygos() : CreatureScript("boss_malygos") {}

        struct boss_malygosAI : public BossAI
        {
            boss_malygosAI(Creature* creature) : BossAI(creature, BOSS_MALYGOS){}

			bool intro;
			std::set<uint64> sparkList;
            std::list<std::pair<uint64,uint64> > mounts;
            uint64 surgeTargets[3];
            uint8 targetCount;
            uint8 step;
            uint8 phase;
            uint8 addsCount;

            void Reset()
            {
                _Reset();

                step = 0;
                phase = PHASE_NONE;
                me->GetMap()->SetZoneOverrideLight(AREA_EYE_OF_ETERNITY, LIGHT_DEFAULT, 1000);
                addsCount = 0;
				intro = false;

				DoCast(me, SPELL_PERIODIC_BREATH, true);
				DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);

                me->SetCanFly(true);
                me->SetSpeed(MOVE_FLIGHT, 2.0f, true);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);

                sparkList.clear();
                mounts.clear();

				instance->DoStopTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_TIMED_START_EVENT);
            }

            void JustSummoned(Creature* summon)
            {
                switch (summon->GetEntry())
                {
                    case NPC_POWER_SPARK:
                        summon->SetActive(true);
						summon->SetCanFly(true);
						summon->SetSpeed(MOVE_FLIGHT, 1.0f);
						summon->SetReactState(REACT_PASSIVE);
						summon->SetInCombatWithZone();
                        sparkList.insert(summon->GetGUID());
                        break;
                    case NPC_STATIC_FIELD:
						summon->SetDisplayId(MODEL_ID_INVISIBLE);
                        summon->SetCanFly(true);
                        summon->SetInCombatWithZone();
                        summon->AddUnitState(UNIT_STATE_ROOT);
                        summon->SetReactState(REACT_PASSIVE);
                        summon->DespawnAfterAction(30*IN_MILLISECONDS);
                        summon->CastSpell(summon, SPELL_STATIC_FIELD, true);
                        break;
                }
                summons.Summon(summon);
            }

			void SummonedCreatureDespawn(Creature* summon)
            {
                switch (summon->GetEntry())
                {
                    case NPC_POWER_SPARK:
                        sparkList.erase(summon->GetGUID());
                        break;
                    case NPC_NEXUS_LORD:
                    case NPC_SCION_OF_ETERNITY:
                        if (Creature* disc = me->SummonCreature(NPC_HOVER_DISK_CASTER, summon->GetPositionX(), summon->GetPositionY(), FLOOR_Z, 0.0f))
                        {
                            disc->ApplySpellImmune(0, IMMUNITY_ID, SPELL_ARCANE_BOMB_KNOCKBACK, true);
                            disc->SetSpeed(MOVE_FLIGHT, 3.0f);
                        }
                        addsCount++;
                        break;
                }
                summons.Despawn(summon);
            }

            void KilledUnit(Unit* victim)
            {
                if (victim == me)
                    return;

                switch (phase)
                {
                    case PHASE_GROUND:
                    case PHASE_VORTEX:
                        DoSendQuantumText(RAND(SAY_PHASE1_SLAY_1, SAY_PHASE1_SLAY_2, SAY_PHASE1_SLAY_3), me);
                        break;
                    case PHASE_ADDS:
                        DoSendQuantumText(RAND(SAY_PHASE2_SLAY_1, SAY_PHASE2_SLAY_2, SAY_PHASE2_SLAY_3), me);
                        break;
                    case PHASE_DRAGONS:
                        DoSendQuantumText(RAND(SAY_PHASE3_SLAY_1, SAY_PHASE3_SLAY_2, SAY_PHASE3_SLAY_3), me);
                        break;
                    default:
                        break;
                }
            }

            void JustDied(Unit* /*killer*/)
            {
                DoSendQuantumText(SAY_DEATH, me);
                events.Reset();
                summons.DespawnAll();
                me->SummonCreature(NPC_ALEXSTRASZA, Locations[3], TEMPSUMMON_TIMED_DESPAWN, 9*MINUTE*IN_MILLISECONDS);
            }

            void MoveInLineOfSight(Unit* who)
            {
				if (!instance || intro || who->GetTypeId() != TYPE_ID_PLAYER || !who->IsWithinDistInMap(me, 60.0f))
					return;

				if (instance && intro == false && who->GetTypeId() == TYPE_ID_PLAYER && who->IsWithinDistInMap(me, 60.0f))
				{
					DoSendQuantumText(RAND(SAY_INTRO_1, SAY_INTRO_2, SAY_INTRO_3, SAY_INTRO_4, SAY_INTRO_5), me);
					intro = true;
				}

                if (phase != PHASE_GROUND || !who->ToCreature())
                    return;

                if (who->GetEntry() == NPC_POWER_SPARK && who->GetDistance(me) < 2.0f && !who->HasAura(SPELL_POWER_SPARK_PLAYERS))
                    who->ToCreature()->AI()->DoAction(ACTION_BUFF);
            }

            void DoAction(int32 const param)
            {
                switch (param)
                {
                    case ACTION_START:
                    {
                        _EnterToBattle();
                        me->SetInCombatWithZone();
						me->GetMotionMaster()->MoveLand(POINT_START, Locations[0]);
                        //me->GetMotionMaster()->MoveLand(POINT_START, Locations[0], 12.0f);

                        while (Creature* dragon = me->FindCreatureWithDistance(NPC_WYRMREST_SKYTALON, 250.0f))
                            dragon->DespawnAfterAction();
                        break;
                    }
                    case ACTION_VORTEX:
                    {
                        Map* map = me->GetMap();
                        if (!map->IsDungeon())
                            return;

                        Map::PlayerList const &PlayerList = map->GetPlayers();
                        for(Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
                        {
                            Player* player = itr->getSource();
                            if (player && !player->IsGameMaster() && player->IsAlive())
                            {
                                if (Creature* vortex = me->SummonCreature(NPC_VORTEX, Locations[2], TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
                                {
									vortex->CastSpell(vortex, SPELL_VORTEX_SPAWN, true);
									player->CastSpell(vortex, SPELL_VORTEX_PLAYER, true);
									vortex->SetDisplayId(MODEL_ID_INVISIBLE);
									vortex->SetCanFly(true);
                                }
                            }
                        }
                        break;
                    }
                    case ACTION_SPARK:
                    {
                        if (Creature* portal = me->SummonCreature(NPC_PORTAL_TRIGGER, SparkLocations[urand(0, 3)], TEMPSUMMON_TIMED_DESPAWN, 90*IN_MILLISECONDS))
                        {
							portal->CastSpell(portal, SPELL_SUMMON_POWER_SPARK);
                            DoSendQuantumText(SAY_SPARK_SUMMON, me);
                        }
                        break;
                    }
                    case ACTION_OVERLOAD:
                    {
                        float x, y, angle;
                        angle = float(2 * M_PI * rand_norm());
                        x = Locations[1].GetPositionX() + float(urand(10, 28)) * cos(angle);
                        y = Locations[1].GetPositionY() + float(urand(10, 28)) * sin(angle);

                        if (Creature* overload = me->SummonCreature(NPC_ARCANE_OVERLOAD, x, y, FLOOR_Z - 1.8f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 45*IN_MILLISECONDS))
                        {
                            if (!urand(0, 2))
                                DoSendQuantumText(SAY_ANTI_MAGIC_SHELL, me);
                            overload->AddUnitState(UNIT_STATE_ROOT);
                            overload->SetReactState(REACT_PASSIVE);
                            overload->SetInCombatWithZone();
                            overload->GetMotionMaster()->MoveIdle();
                            DoCast(overload, SPELL_ARCANE_BOMB, true);
                        }
                        break;
                    }
                    case ACTION_DEEP_BREATH:
                    {
                        if (Creature* surge = me->SummonCreature(NPC_SURGE_OF_POWER, Locations[3], TEMPSUMMON_TIMED_DESPAWN, 10*IN_MILLISECONDS))
                        {
							DoSendQuantumText(SAY_BREATH_ANNOUNCE, me);
                            DoSendQuantumText(SAY_BREATH_ATTACK, me);
                            surge->SetDisplayId(MODEL_ID_INVISIBLE);
                            surge->SetReactState(REACT_PASSIVE);
                            surge->SetInCombatWithZone();
                            DoCast(surge, SPELL_DEEP_BREATH);
                        }
                        break;
                    }
					case ACTION_SPAWN_ADDS:
                    {
						for (uint8 i = 0; i < 6; i++)
                        {
                            if (Creature* lord = me->SummonCreature(NPC_NEXUS_LORD, NexusLordSpawnLocations[i], TEMPSUMMON_CORPSE_DESPAWN))
                            {
								lord->SetReactState(REACT_AGGRESSIVE);
								lord->SetInCombatWithZone();
								lord->CastSpell(lord, SPELL_TELEPORT_VISUAL_ONLY, true);
                            }
                        }

						for (uint8 i = 0; i < 6; i++)
                        {
                            if (Creature* scion = me->SummonCreature(NPC_SCION_OF_ETERNITY, ScionOfEnetrnitySpawnLocations[i], TEMPSUMMON_CORPSE_DESPAWN))
                            {
								scion->SetReactState(REACT_PASSIVE);
								scion->CastSpell(scion, SPELL_FLIGHT, true);
								scion->CastSpell(scion, SPELL_TELEPORT_VISUAL_ONLY, true);

								scion->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
								scion->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                            }
                        }
                        break;
                    }
                    case ACTION_SPAWN_MOUNTS:
                    {
                        Map* map = me->GetMap();
                        if (!map->IsDungeon())
                            return;

                        Map::PlayerList const &PlayerList = map->GetPlayers();
                        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        {
                            Player* player = i->getSource();
                            if (player && !player->IsGameMaster() && player->IsAlive())
                            {
                                if (Creature* mount = player->SummonCreature(NPC_WYRMREST_SKYTALON, player->GetPositionX(), player->GetPositionY(), 220.0f, player->GetOrientation()))
                                {
									me->PlayDirectSound(14405);

                                    mount->SetCanFly(true);
                                    mount->SetSpeed(MOVE_FLIGHT, 2.0f);
                                    mounts.push_back(std::pair<uint64, uint64>(mount->GetGUID(), player->GetGUID()));
                                }
                            }
                        }
                        me->SetInCombatWithZone();
                        break;
                    }
                    case ACTION_MOUNT_ALL:
                    {
                        if (mounts.empty())
                            return;

                        for (std::list<std::pair<uint64,uint64> >::iterator iter = mounts.begin(); iter != mounts.end(); ++iter)
                        {
                            Creature* mount = Unit::GetCreature(*me, (*iter).first);
                            Player* player = Unit::GetPlayer(*me, (*iter).second);

                            if (!mount || !player)
                                continue;

                            if (!player->IsAlive())
                                continue;

                            //mount->SetCreatorGUID(player->GetGUID());
                            if (Pet* pet = player->GetPet())
                                player->RemovePet(pet, PET_SAVE_NOT_IN_SLOT, true);
                            player->UnsummonAllTotems();
                            player->CastSpell(mount, SPELL_RIDE_RED_DRAGON_BUDDY, true);

                            me->SetInCombatWith(mount);
                            me->AddThreat(mount, 10.0f);
                            mount->SetInCombatWith(me);
                            mount->AddThreat(me, 10.0f);
                        }

                        me->SetInCombatWithZone();
                        break;
                    }
                    case ACTION_CAST_SURGE:
                    {
                        if (!urand(0, 2))
                            DoSendQuantumText(SAY_SURGE_OF_POWER, me);

                        if (!Is25ManRaid())
                        {
                            if (Unit* target = SelectVehicleBaseOrPlayer())
                            {
                                if (Player* player = target->GetCharmerOrOwnerPlayerOrPlayerItself())
                                    me->MonsterTextEmote(EMOTE_SURGE, 0, true);
                                DoCast(target, SPELL_SURGE_OF_POWER_10);
                            }
                        }
                        else
                        {
                            std::list<Unit*> targetList;
                            const std::list<HostileReference*>& threatlist = me->getThreatManager().getThreatList();
                            for (std::list<HostileReference*>::const_iterator itr = threatlist.begin(); itr != threatlist.end(); ++itr)
                                if ((*itr)->getTarget()->ToCreature() && (*itr)->getTarget()->GetEntry() == NPC_WYRMREST_SKYTALON)
                                    targetList.push_back((*itr)->getTarget());

                            if (targetList.empty())
                                return;

                            // randomize and resize
                            Quantum::DataPackets::RandomResizeList<Unit*>(targetList, 3);

                            targetCount = 0;
                            // store guids and notify players
                            for (std::list<Unit*>::iterator itr = targetList.begin(); itr != targetList.end(); ++itr)
                            {
                                surgeTargets[targetCount++] = (*itr)->GetGUID();

                                if (Player* player = (*itr)->GetCharmerOrOwnerPlayerOrPlayerItself())
									me->MonsterTextEmote(EMOTE_SURGE, 0, true);
                            }
                            targetCount = 0;
                            DoCast(SPELL_SURGE_OF_POWER_25);
                        }
                        break;
                    }
                    case ACTION_CHECK_PLAYER:
                    {
                        /* workaround to prevent players from falling through map in alive state (once they got unmounted) */
                        Map* map = me->GetMap();
                        if (!map->IsDungeon())
                            return;

                        Map::PlayerList const &playerList = map->GetPlayers();
                        for (Map::PlayerList::const_iterator i = playerList.begin(); i != playerList.end(); ++i)
                        {
                            Player* player = i->getSource();
                            if (player && !player->IsGameMaster() && player->IsAlive())
                            {
                                if (!player->GetVehicle())
                                {
                                    player->SetUnitMovementFlags(0);
                                    me->DealDamage(player, player->GetHealth(), 0, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, 0, false);
                                    player->SetUnitMovementFlags(0);
                                    player->SetMovement(MOVE_ROOT);

                                    // despawn dragon
                                    for (std::list<std::pair<uint64, uint64> >::iterator iter = mounts.begin(); iter != mounts.end(); ++iter)
									{
                                        if (player->GetGUID() == (*iter).second)
										{
                                            if (Creature* mount = Unit::GetCreature(*me, (*iter).first))
                                                mount->DespawnAfterAction(1*IN_MILLISECONDS);
										}
									}
                                }
                            }
                        }
                        break;
                    }
                    case ACTION_BUFF:
                    {
                        DoCast(SPELL_POWER_SPARK);
                        DoSendQuantumText(SAY_SPARK_BUFFED, me);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SparkMovement(bool move)
            {
                if (sparkList.empty())
                    return;

                for (std::set<uint64>::const_iterator itr = sparkList.begin(); itr != sparkList.end(); ++itr)
				{
                    if (Creature* spark = me->GetCreature(*me, *itr))
                    {
                        // spark already "dead"
                        if (spark->HasAura(SPELL_POWER_SPARK_PLAYERS))
                            continue;

                        spark->AI()->DoAction(move ? ACTION_MOVE_SPARK : ACTION_STOP_SPARK);

                        // remove remaining free sparks
                        if (phase == PHASE_ADDS)
                            spark->DespawnAfterAction(1*IN_MILLISECONDS);
                    }
				}
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE && type != EFFECT_MOTION_TYPE)
                    return;

                switch (id)
                {
                    case POINT_START:
                    {
                        DoSendQuantumText(SAY_PHASE1_AGGRO, me);

						instance->DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_TIMED_START_EVENT);

                        me->SetCanFly(false);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->SetInCombatWithZone();
                        events.ScheduleEvent(EVENT_ENRAGE, 10*MINUTE*IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_STORM, 5*IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_SPARK, 20*IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_BREATH, 10*IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_VORTEX_FLY_UP, 25*IN_MILLISECONDS);
                        phase = PHASE_GROUND;
                        break;
                    }
                    case POINT_VORTEX:
                    {
                        DoCast(me, SPELL_VORTEX_VISUAL, true);
                        events.ScheduleEvent(EVENT_VORTEX, 2*IN_MILLISECONDS);
                        phase = PHASE_VORTEX;
                        break;
                    }
                    // vortex finished, reached current victim's pos.
                    case POINT_FLY_DOWN:
                    {
                        SparkMovement(true);
                        me->SetCanFly(false);

                        if (Unit* victim = SelectTarget(TARGET_TOP_AGGRO))
                            me->GetMotionMaster()->MoveChase(victim);

                        me->SetReactState(REACT_AGGRESSIVE);
                        events.ScheduleEvent(EVENT_STORM, urand(7, 10) * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_SPARK, 15*IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_BREATH, RAID_MODE<uint32>(7, 3) * IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_VORTEX_FLY_UP, 45*IN_MILLISECONDS);
                        phase = PHASE_GROUND;
                        break;
                    }
                    case POINT_PHASE_2:
                    {
                        DoSendQuantumText(SAY_PHASE2_AGGRO, me);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->RemoveAllAuras();
                        DoAction(ACTION_SPAWN_ADDS);
                        phase = PHASE_ADDS;
                        SparkMovement(false);
                        events.ScheduleEvent(EVENT_OVERLOAD, 5*IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_SURGE_OF_POWER, 60*IN_MILLISECONDS);
                        break;
                    }
                    case POINT_DESTROY_FLOOR:
                    {
                        DoSendQuantumText(SAY_PHASE3_INTRO, me);
                        DoCast(SPELL_DESTROY_PLATFORM_CHANNEL);
                        events.ScheduleEvent(EVENT_IDLE, 6*IN_MILLISECONDS);
                        step = 1;
                        phase = PHASE_IDLE;
                        break;
                    }
                    case POINT_PHASE_3:
                    {
                        DoSendQuantumText(SAY_PHASE3_AGGRO, me);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        DoCast(me, SPELL_ROOT, true);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_ARCANE_BOMB)
                {
                    target->CastSpell(target, SPELL_ARCANE_BOMB_KNOCKBACK, true);
                    target->CastSpell(target, SPELL_ARCANE_OVERLOAD, true);
                    target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                }
            }

            uint64 GetGUID(int32 /*id*/ = 0)
            {
                return surgeTargets[targetCount++];
            }

            Unit* SelectVehicleBaseOrPlayer()
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM))
                {
                    if (Unit* vehicle = target->GetVehicleBase())
                        return vehicle;

                    return target;
                }
                return NULL;
            }

            void UpdateAI(uint32 const diff)
            {
                if (phase == PHASE_NONE || (phase != PHASE_NONE && !UpdateVictim()))
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_STORM:
                            DoCast(RAID_MODE<uint32>(SPELL_ARCANE_STORM_10, SPELL_ARCANE_STORM_25));
                            if (phase == PHASE_DRAGONS)
                            {
                                if (!urand(0, 2))
                                    DoSendQuantumText(RAND(SAY_PHASE3_CAST_1, SAY_PHASE3_CAST_2, SAY_PHASE3_CAST_3), me);
                                events.ScheduleEvent(EVENT_STORM, 10*IN_MILLISECONDS);
                            }
                            else events.ScheduleEvent(EVENT_STORM, urand(10, 15) * IN_MILLISECONDS);
                            break;
                        case EVENT_SPARK:
                            DoAction(ACTION_SPARK);
                            events.ScheduleEvent(EVENT_SPARK, 20*IN_MILLISECONDS);
                            break;
                        case EVENT_BREATH:
                            DoCastVictim(RAID_MODE<uint32>(SPELL_ARCANE_BREATH_10, SPELL_ARCANE_BREATH_25));
                            events.ScheduleEvent(EVENT_BREATH, 20*IN_MILLISECONDS);
                            break;
                        case EVENT_VORTEX_FLY_UP:
                            SparkMovement(false);
                            me->SetCanFly(true);
                            me->SetReactState(REACT_PASSIVE);
                            me->GetMotionMaster()->MoveTakeoff(POINT_VORTEX, Locations[1]);
                            events.CancelEvent(EVENT_STORM);
                            events.CancelEvent(EVENT_SPARK);
                            events.CancelEvent(EVENT_BREATH);
                            phase = PHASE_IDLE;
                            break;
                        case EVENT_VORTEX:
                            DoAction(ACTION_VORTEX);
                            events.ScheduleEvent(EVENT_VORTEX_FLY_DOWN, 12*IN_MILLISECONDS);
                            break;
                        case EVENT_VORTEX_FLY_DOWN:
                            me->GetMotionMaster()->MoveLand(POINT_FLY_DOWN, Locations[5]);
                            phase = PHASE_IDLE;
                            break;
                        case EVENT_ENRAGE:
                            if (!me->HasAura(SPELL_ENRAGE))
                            {
                                if (phase == PHASE_GROUND || phase == PHASE_ADDS || phase == PHASE_DRAGONS)
                                {
                                    DoCast(me, SPELL_ENRAGE, true);

                                    if (phase == PHASE_ADDS)
                                    {
                                        me->SetCanFly(true);
                                        me->SetSpeed(MOVE_FLIGHT, 10.0f);
                                        me->SetReactState(REACT_AGGRESSIVE);
                                        me->GetMotionMaster()->Clear();
                                        me->GetMotionMaster()->MoveChase(me->GetVictim());
                                    }
                                }
                                events.ScheduleEvent(EVENT_ENRAGE, 1*IN_MILLISECONDS);
                            }
                            break;
                        case EVENT_OVERLOAD:
                            DoAction(ACTION_OVERLOAD);
                            events.ScheduleEvent(EVENT_OVERLOAD, urand(15, 20) * IN_MILLISECONDS);
                            break;
                        case EVENT_SURGE_OF_POWER:
                            if (phase == PHASE_DRAGONS)
                            {
                                DoAction(ACTION_CAST_SURGE);
                                events.ScheduleEvent(EVENT_SURGE_OF_POWER, 10*IN_MILLISECONDS);
                            }
                            else
                            {
                                DoAction(ACTION_DEEP_BREATH);
                                events.ScheduleEvent(EVENT_SURGE_OF_POWER, 60*IN_MILLISECONDS);
                            }
                            return;
                        case EVENT_STATIC_FIELD:
                            if (Unit* target = SelectVehicleBaseOrPlayer())
                            {
                                if (!urand(0, 2))
                                    DoSendQuantumText(RAND(SAY_PHASE3_CAST_1, SAY_PHASE3_CAST_2, SAY_PHASE3_CAST_3), me);
                                DoCast(target, SPELL_STATIC_FIELD_MISSLE);
                            }
                            events.ScheduleEvent(EVENT_STATIC_FIELD, 25*IN_MILLISECONDS);
                            return;
                        case EVENT_PULSE:
                            DoCast(SPELL_ARCANE_PULSE);
                            events.ScheduleEvent(EVENT_PULSE, 10*IN_MILLISECONDS);
                            break;
                        case EVENT_CHECK_PLAYER:
                            DoAction(ACTION_CHECK_PLAYER);
                            events.ScheduleEvent(EVENT_CHECK_PLAYER, 1*IN_MILLISECONDS);
                            break;
                        case EVENT_IDLE:
                            switch (step)
                            {
                                case 1:
                                {
                                    me->GetMap()->SetZoneOverrideLight(AREA_EYE_OF_ETERNITY, LIGHT_CLOUDY, 1000);
                                    DoCast(me, SPELL_DESTROY_PLATFORM_BOOM, true);
                                    DoCast(SPELL_DESTROY_PLATFORM_EVENT);
                                    events.ScheduleEvent(EVENT_IDLE, 2*IN_MILLISECONDS);
                                    ++step;
                                    break;
                                }
                                case 2:
                                {
									if (GameObject* platform = instance->instance->GetGameObject(instance->GetData64(DATA_PLATFORM)))
										platform->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);

                                    me->SetSpeed(MOVE_FLIGHT, 2.0f);
                                    me->GetMotionMaster()->MovePoint(POINT_PHASE_3, Locations[4]);
                                    DoAction(ACTION_SPAWN_MOUNTS);
                                    events.ScheduleEvent(EVENT_IDLE, 3*IN_MILLISECONDS);
                                    ++step;
                                    break;
                                }
                                case 3:
                                {
                                    DoAction(ACTION_MOUNT_ALL);
                                    events.ScheduleEvent(EVENT_SURGE_OF_POWER, 10*IN_MILLISECONDS);
                                    events.ScheduleEvent(EVENT_STORM, 15*IN_MILLISECONDS);
                                    events.ScheduleEvent(EVENT_CHECK_PLAYER, 5*IN_MILLISECONDS);
                                    events.ScheduleEvent(EVENT_STATIC_FIELD, 20*IN_MILLISECONDS);
                                    events.ScheduleEvent(EVENT_PULSE, 10*IN_MILLISECONDS);
                                    me->GetMap()->SetZoneOverrideLight(AREA_EYE_OF_ETERNITY, LIGHT_CLOUDY_RUNES, 20000);
                                    phase = PHASE_DRAGONS;
                                    ++step;
                                    break;
                                }
                            }
                            break;
                        default:
                            break;
                    }
                }

                // switch to phase 2
                if (phase == PHASE_GROUND && HealthBelowPct(50))
                {
                    if (!me->HasAura(SPELL_ENRAGE))
                    {
                        DoSendQuantumText(SAY_PHASE1_END, me);
                        me->SetCanFly(true);
                        me->SetReactState(REACT_PASSIVE);
                        me->GetMotionMaster()->MoveTakeoff(POINT_PHASE_2, Locations[3]);
                        events.CancelEvent(EVENT_STORM);
                        events.CancelEvent(EVENT_SPARK);
                        events.CancelEvent(EVENT_BREATH);
                        events.CancelEvent(EVENT_VORTEX_FLY_UP);
                        me->GetMap()->SetZoneOverrideLight(AREA_EYE_OF_ETERNITY, LIGHT_RUNES, 3000);
                        phase = PHASE_IDLE;
                    }
                }

                // switch to phase 3
                if (phase == PHASE_ADDS && addsCount == RAID_MODE<uint8>(6, 7)) // 6, 12
                {
                    DoSendQuantumText(SAY_PHASE2_END, me);
                    me->SetSpeed(MOVE_FLIGHT, 0.3f, true);
                    me->GetMotionMaster()->MovePoint(POINT_DESTROY_FLOOR, Locations[1]);
                    summons.DespawnAll(); // remove remaining anti-magic shells and discs
                    events.CancelEvent(EVENT_SURGE_OF_POWER);
                    events.CancelEvent(EVENT_OVERLOAD);
                    me->GetMap()->SetZoneOverrideLight(AREA_EYE_OF_ETERNITY, LIGHT_SPACE_FLIGHT, 1000);
                    phase = PHASE_IDLE;
                }

                if (phase == PHASE_GROUND || me->HasAura(SPELL_ENRAGE))
                    DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_malygosAI(creature);
        }
};

class npc_scion_of_eternity : public CreatureScript
{
    public:
        npc_scion_of_eternity() : CreatureScript("npc_scion_of_eternity") {}

        struct npc_scion_of_eternityAI : public QuantumBasicAI
        {
            npc_scion_of_eternityAI(Creature* creature) : QuantumBasicAI(creature)
			{
				instance = creature->GetInstanceScript();
			}

			InstanceScript* instance;
			uint32 ArcaneBarrageTimer;

            void Reset()
            {
				ArcaneBarrageTimer = urand(10, 20) *IN_MILLISECONDS;
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (ArcaneBarrageTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, false, -SPELL_ARCANE_BARRAGE_DMG))
                    {
                        me->AddAura(SPELL_ARCANE_BARRAGE_DMG, target);
                        DoCast(target, SPELL_ARCANE_BARRAGE);
                        ArcaneBarrageTimer = urand(8, 12) *IN_MILLISECONDS;
                    }
                }
                else ArcaneBarrageTimer -= diff;
            }

			void JustDied(Unit* killer)
			{
				if (killer->HasAura(SPELL_RIDE_VEHICLE))
					instance->DoCompleteAchievement(RAID_MODE(ACHIEVEMENT_DENYIN_THE_SCION_10, ACHIEVEMENT_DENYIN_THE_SCION_25));
			}
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_scion_of_eternityAI(creature);
        }
};

class npc_nexus_lord : public CreatureScript
{
    public:
        npc_nexus_lord() : CreatureScript("npc_nexus_lord") { }

        struct npc_nexus_lordAI : public QuantumBasicAI
        {
            npc_nexus_lordAI(Creature* creature) : QuantumBasicAI(creature) { }

			uint32 ArcaneShockTimer;
            uint32 HasteTimer;

            void Reset()
            {
                HasteTimer = urand(15, 20) * IN_MILLISECONDS;
                ArcaneShockTimer = urand(5, 10) * IN_MILLISECONDS;
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (ArcaneShockTimer <= diff)
                {
                    DoCastVictim(RAID_MODE(SPELL_ARCANE_SHOCK_10, SPELL_ARCANE_SHOCK_25));
                    ArcaneShockTimer = urand(5, 10) *IN_MILLISECONDS;
                }
                else
                    ArcaneShockTimer -= diff;

                if (HasteTimer <= diff)
                {
                    DoCast(me, SPELL_HASTE);
                    HasteTimer = urand(15, 25) *IN_MILLISECONDS;
                }
                else HasteTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_nexus_lordAI(creature);
        }
};

class npc_alexstrasza : public CreatureScript
{
    public:
        npc_alexstrasza() : CreatureScript("npc_alexstrasza") { }

        struct npc_alexstraszaAI : public QuantumBasicAI
        {
            npc_alexstraszaAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;
            uint32 StepTimer;
            uint8 Step;

            void Reset()
            {
                Step = 1;
                StepTimer = 5*IN_MILLISECONDS;
                me->SetCanFly(true);
                me->SetActive(true);
            }

            void UpdateAI(uint32 const diff)
            {
                if (Step > 5)
                    return;

                if (StepTimer <= diff)
                {
                    switch (Step)
                    {
                        // workaround
                        case 1:
                        {
							if (GameObject* platform = instance->instance->GetGameObject(instance->GetData64(DATA_PLATFORM)))
								platform->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);

                            Map* map = me->GetMap();
                            if (!map->IsDungeon())
                                return;

                            Map::PlayerList const &PlayerList = map->GetPlayers();
                            for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                            {
                                Player* player = i->getSource();
                                if (player->IsAlive())
                                {
                                    if (Unit* mount = player->GetVehicleBase())
                                    {
                                        player->ExitVehicle();
										player->JumpTo(me, 10.0f);
                                        player->CastSpell(player, SPELL_PARACHUTE, true);
										//player->NearTeleportTo(721.858f, 1316.81f, 267.235f, 5.8548f);
										mount->ToCreature()->DespawnAfterAction();
                                    }
                                }
                            }
                            StepTimer = 7*IN_MILLISECONDS;
                            break;
                        }
                        case 2:
                        {
                            DoSendQuantumText(SAY_OUTRO_1, me);
                            StepTimer = 9*IN_MILLISECONDS;
                            break;
                        }
                        case 3:
                        {
                            DoSendQuantumText(SAY_OUTRO_2, me);
                            if (Creature* gift = me->SummonCreature(NPC_ALEXSTRASZAS_GIFT, Locations[5], TEMPSUMMON_TIMED_DESPAWN, 90*IN_MILLISECONDS))
                            {
                                gift->SetCanFly(true);
                                gift->SetDisplayId(MODEL_ID_INVISIBLE);
                                gift->CastSpell(gift, SPELL_ALEXSTRASZAS_GIFT_VISUAL, true);
                                DoCast(SPELL_ALEXSTRASZAS_GIFT_BEAM);
                            }
                            StepTimer = 5*IN_MILLISECONDS;
                            break;
                        }
                        case 4:
                        {
                            DoSendQuantumText(SAY_OUTRO_3, me);
                            me->SummonGameObject(RAID_MODE<uint32>(GO_ALEXSTRASZAS_GIFT_10, GO_ALEXSTRASZAS_GIFT_25), Locations[5].GetPositionX(),
                                Locations[5].GetPositionY(), Locations[5].GetPositionZ(), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0);
							
							me->SummonGameObject(RAID_MODE<uint32>(GO_HEART_OF_MAGIC_10, GO_HEART_OF_MAGIC_25), Locations[5].GetPositionX() + 15.0f,
								Locations[5].GetPositionY(), Locations[5].GetPositionZ(), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0);

                            // custom
                            if (Creature* temporary = me->SummonCreature(NPC_ALEXSTRASZAS_GIFT, Locations[0]))
                            {
                                temporary->SetCurrentFaction(7);
                                temporary->SetDisplayId(856);
                                temporary->GetMotionMaster()->MoveRandom(7.0f);
                                temporary->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                                DoCast(temporary, 1130, true);

                                if (Is25ManRaid())
                                    temporary->SetLootMode(LOOT_MODE_HARD_MODE_1);
                            }

							instance->SetBossState(BOSS_MALYGOS, DONE);
							instance->SaveToDB();

                            StepTimer = 23*IN_MILLISECONDS;
                            break;
                        }
                        case 5:
                        {
                            DoSendQuantumText(SAY_OUTRO_4, me);
                            StepTimer = 10*MINUTE*IN_MILLISECONDS;
                            break;
                        }
                    }
                    ++Step;
                }
                else StepTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_alexstraszaAI(creature);
        }
};

class npc_power_spark : public CreatureScript
{
    public:
        npc_power_spark() : CreatureScript("npc_power_spark") { }

        struct npc_power_sparkAI : public QuantumBasicAI
        {
            npc_power_sparkAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

			InstanceScript* instance;
            uint32 DelayTimer;
            bool _isNear;

            void Reset()
            {
                _isNear = false;
                DelayTimer = 3*IN_MILLISECONDS;
                DoCast(me, SPELL_POWER_SPARK_VISUAL, true);
                DoAction(ACTION_MOVE_SPARK);
            }

            void DoAction(int32 const param)
            {
                switch (param)
                {
                    case ACTION_BUFF:
                        _isNear = true;
                        break;
                    case ACTION_MOVE_SPARK:
                        if (Creature* malygos = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_MALYGOS) : 0))
                            me->GetMotionMaster()->MoveFollow(malygos, 0, 0);
                        break;
                    case ACTION_STOP_SPARK:
                        me->GetMotionMaster()->Clear();
                        me->GetMotionMaster()->MoveIdle();
                        break;
                }
            }

            void DamageTaken(Unit* /*attacker*/, uint32 &damage)
            {
                if (damage >= me->GetHealth())
                {
                    _isNear = false;
                    me->RemoveAllAuras();
                    damage = me->GetHealth() - 1;
                    DoCast(me, SPELL_POWER_SPARK_PLAYERS, true);
                    me->SetCanFly(false);
                    me->SetReactState(REACT_PASSIVE);
                    me->GetMotionMaster()->Clear();
                    me->GetMotionMaster()->MoveFall();
                    me->DespawnAfterAction(60*IN_MILLISECONDS);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (_isNear)
                {
                    if (DelayTimer <= diff)
                    {
                        _isNear = false;
                        me->DespawnAfterAction(0.5*IN_MILLISECONDS);

                        if (Creature* malygos = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_MALYGOS) : 0))
                            malygos->AI()->DoAction(ACTION_BUFF);
                    }
                    else DelayTimer -= diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_power_sparkAI(creature);
        }
};

class npc_vortex_vehicle : public CreatureScript
{
    public:
        npc_vortex_vehicle() : CreatureScript("npc_vortex_vehicle") { }

        struct npc_vortex_vehicleAI : public QuantumBasicAI
        {
            npc_vortex_vehicleAI(Creature* creature) : QuantumBasicAI(creature) { }

			uint32 TurnTimer;
            float _angle;

            void Reset()
            {
                TurnTimer = 100;
                me->SetCurrentFaction(35);

                _angle = float(2 * M_PI * rand_norm());
                me->SetOrientation(_angle);
            }

            void UpdateAI(uint32 const diff)
            {
                // custom rotation
                if (TurnTimer <= diff)
                {
                    _angle += M_PI * 2 / 7;
                    if (_angle >= M_PI * 2)
                        _angle -= M_PI * 2;

                    me->SetOrientation(_angle);
                    me->SendMovementFlagUpdate();

                    TurnTimer = 100;
                }
                else TurnTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_vortex_vehicleAI(creature);
        }
};

class npc_hover_disc : public CreatureScript
{
    public:
        npc_hover_disc() : CreatureScript("npc_hover_disc") { }

        struct npc_hover_discAI : public QuantumBasicAI
        {
            npc_hover_discAI(Creature* creature) : QuantumBasicAI(creature) { }

			bool Move;
            uint32 Count;
            uint32 CheckTimer;

            void Reset()
            {
                Move = false;
                CheckTimer = 1*IN_MILLISECONDS;
                me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_ARCANE_BOMB_KNOCKBACK, true);
            }

            void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply)
            {
                if (!apply)
                    me->DespawnAfterAction(1*IN_MILLISECONDS);
            }

            void SetData(uint32 /*type*/, uint32 data)
            {
                Count = data;
            }

            void MovementInform(uint32 type, uint32 id)
            {
                if (type != POINT_MOTION_TYPE)
                    return;

                if (Unit* passenger = me->GetVehicleKit()->GetPassenger(0))
                {
                    if (!passenger->ToCreature())
                        return;

                    if (passenger->GetEntry() == NPC_NEXUS_LORD)
                    {
                        passenger->ToCreature()->SetReactState(REACT_AGGRESSIVE);
                        passenger->ToCreature()->SetInCombatWithZone();
                        me->SetReactState(REACT_AGGRESSIVE);
                        me->SetInCombatWithZone();
                    }
                    else Move = true;
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (Move)
                {
                    Move = false;
                    float x, y, angle;
                    angle = float(Count) * 2 * M_PI / 16;
                    x = Locations[1].GetPositionX() + float(urand(20, 28)) * cos(angle);
                    y = Locations[1].GetPositionY() + float(urand(20, 28)) * sin(angle);
                    me->GetMotionMaster()->MovePoint(1, x, y, FLOOR_Z + 10.0f);

                    Count--;
                    if (Count == 0)
                        Count = 16;
                }

                if (!UpdateVictim())
                    return;

                if (me->GetReactState() == REACT_AGGRESSIVE)
                {
                    if (CheckTimer <= diff)
                    {
                        Unit* passenger = me->GetVehicleKit()->GetPassenger(0);
                        if (passenger && passenger->ToCreature() && passenger->GetEntry() == NPC_NEXUS_LORD)
                        {
                            Unit* target = passenger->GetVictim();
                            if (target && target != me->GetVictim())
                            {
                                me->getThreatManager().modifyThreatPercent(me->GetVictim(), -100);
                                me->AddThreat(target, 9999999.0f);
                            }
                        }
                        CheckTimer = 1*IN_MILLISECONDS;
                    }
                    else CheckTimer -= diff;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_hover_discAI(creature);
        }
};

class go_focusing_iris : public GameObjectScript
{
    public:
        go_focusing_iris() : GameObjectScript("go_focusing_iris") { }

        bool OnGossipHello(Player* /*player*/, GameObject* go)
        {
            InstanceScript* instance = go->GetInstanceScript();

            Creature* malygos = Unit::GetCreature(*go, instance ? instance->GetData64(DATA_MALYGOS) : 0);
            if (malygos && malygos->IsAlive())
            {
                go->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
                malygos->AI()->DoAction(ACTION_START);

                if (Creature* trigger = go->SummonCreature(NPC_SURGE_OF_POWER, go->GetPositionX(), go->GetPositionY(), 277.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 6*IN_MILLISECONDS))
                {
                    trigger->SetDisplayId(MODEL_ID_INVISIBLE);
                    trigger->SetCanFly(true);
                    trigger->SetReactState(REACT_PASSIVE);
                    trigger->CastSpell(trigger, SPELL_IRIS_VISUAL, true);
                }
            }
            return true;
        }
};

class spell_surge_of_power_targeting : public SpellScriptLoader
{
    public:
        spell_surge_of_power_targeting() : SpellScriptLoader("spell_surge_of_power_targeting") { }

        class spell_surge_of_power_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_surge_of_power_targeting_SpellScript)

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.clear();
                Unit* caster = GetCaster();

                if (!caster->GetAI())
                    return;

                for (uint8 i = 0; i < 3; ++i)
                    if (Unit* target = ObjectAccessor::GetUnit(*caster, caster->GetAI()->GetGUID()))
                        unitList.push_back(target);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_surge_of_power_targeting_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_surge_of_power_targeting_SpellScript();
        }
};

void AddSC_boss_malygos()
{
    new boss_malygos();
    new npc_nexus_lord();
    new npc_scion_of_eternity();
    new npc_alexstrasza();
    new npc_power_spark();
    new npc_vortex_vehicle();
    new npc_hover_disc();
    new go_focusing_iris();
    new spell_surge_of_power_targeting();
}