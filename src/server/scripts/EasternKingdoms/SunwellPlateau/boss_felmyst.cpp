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
#include "sunwell_plateau.h"

enum Texts
{
    SAY_BIRTH          = -1580036,
    SAY_SLAY_1         = -1580037,
    SAY_SLAY_2         = -1580038,
    SAY_BREATH         = -1580039,
    SAY_TAKE_OFF       = -1580040,
    SAY_BERSERK        = -1580041,
    SAY_DEATH          = -1580042,
    SAY_KALECGOS_OUTRO = -1580043,
};

enum Spells
{
    AURA_NOXIOUS_FUMES          = 47002,
    // Land phase
    SPELL_CLEAVE                = 19983,
    SPELL_CORROSION             = 45866,
    SPELL_GAS_NOVA              = 45855,
    SPELL_ENCAPSULATE_CHANNEL   = 45661,
    // Flight phase
    SPELL_VAPOR_SELECT          = 45391,   // fel to player, force cast 45392, 50000y selete target
    SPELL_VAPOR_SUMMON          = 45392,   // player summon vapor, radius around caster, 5y,
    SPELL_VAPOR_FORCE           = 45388,   // vapor to fel, force cast 45389
    SPELL_VAPOR_CHANNEL         = 45389,   // fel to vapor, green beam channel
    SPELL_VAPOR_TRIGGER         = 45411,   // linked to 45389, vapor to self, trigger 45410 and 46931
    SPELL_VAPOR_DAMAGE          = 46931,   // vapor damage, 4000
    SPELL_TRAIL_SUMMON          = 45410,   // vapor summon trail
    SPELL_TRAIL_TRIGGER         = 45399,   // trail to self, trigger 45402
    SPELL_TRAIL_DAMAGE          = 45402,   // trail damage, 2000 + 2000 dot
    SPELL_DEAD_SUMMON           = 45400,   // summon blazing dead, 5min
    SPELL_DEAD_PASSIVE          = 45415,
    SPELL_FOG_BREATH            = 45495,   // fel to self, speed burst
    SPELL_FOG_TRIGGER           = 45582,   // fog to self, trigger 45782
    SPELL_FOG_FORCE             = 45782,   // fog to player, force cast 45714
    SPELL_FOG_INFORM            = 45714,   // player let fel cast 45717, script effect
    SPELL_FOG_CHARM             = 45717,   // fel to player
    SPELL_FOG_CHARM2            = 45726,   // link to 45717
    SPELL_TRANSFORM_TRIGGER     = 44885,   // madrigosa to self, trigger 46350
    SPELL_TRANSFORM_VISUAL      = 46350,   //46411stun?
    SPELL_TRANSFORM_FELMYST     = 45068,   // become fel
    SPELL_FELMYST_SUMMON        = 45069,
    // Other
    SPELL_BERSERK               = 45078,
    SPELL_CLOUD_VISUAL          = 45212,
    SPELL_CLOUD_SUMMON          = 45884,
};

enum PhaseFelmyst
{
    PHASE_NULL   = 0,
    PHASE_GROUND = 1,
    PHASE_FLIGHT = 2,
};

enum EventFelmyst
{
    EVENT_NULL              =   0,
    EVENT_BERSERK           =   1,
    EVENT_CLEAVE            =   2,
    EVENT_CORROSION         =   3,
    EVENT_GAS_NOVA          =   4,
    EVENT_ENCAPSULATE       =   5,
    EVENT_FLIGHT            =   6,
    EVENT_FLIGHT_SEQUENCE   =   2,
    EVENT_SUMMON_DEAD       =   3,
    EVENT_SUMMON_FOG        =   4,
};

enum FelmystPosition
{
    LEFT,
    RIGHT
};

static EventFelmyst MaxTimer[]=
{
    EVENT_NULL,
    EVENT_FLIGHT,
    EVENT_SUMMON_FOG,
};

#define FLIGHT_1_X          1458
#define FLIGHT_1_Y          501
#define FLIGHT_1_Z          60

#define FLIGHT_2_X          1468
#define FLIGHT_2_Y          730
#define FLIGHT_2_Z          60

uint32 LeftFlyPos[2][3] =
{
    {1510, 670, 60},
    {1435, 650, 60}
};

uint32 RightFlyPos[2][3] = 
{
    {1515, 540, 60},
    {1435, 520, 60}
};

#define MIDDLE_Y    610

class boss_felmyst : public CreatureScript
{
public:
    boss_felmyst() : CreatureScript("boss_felmyst") {}

    struct boss_felmystAI : public QuantumBasicAI
    {
        boss_felmystAI(Creature* creature) : QuantumBasicAI(creature)
		{
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
        PhaseFelmyst Phase;
        EventFelmyst Event;
        uint32 Timer[EVENT_FLIGHT + 1];

        uint32 FlightCount;
        uint32 BreathCount;

        float BreathX, BreathY;

        void Reset()
        {
            Phase = PHASE_NULL;
            Event = EVENT_NULL;
            Timer[EVENT_BERSERK] = 600000;
            FlightCount = 0;

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetCanFly(true);
			me->SetDisableGravity(true);
            me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 10);
            me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 10);

            DespawnSummons(NPC_VAPOR_TRAIL);
            KillCharmedPlayer();
            me->SetActive(false);

			instance->SetData(DATA_FELMYST_EVENT, NOT_STARTED);

            me->SetVisible(false);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            me->SetActive(true);
            DoZoneInCombat();
            DoCast(me, AURA_NOXIOUS_FUMES, true);

			DoSendQuantumText(SAY_BIRTH, me);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
			me->SetCanFly(false);
			me->SetDisableGravity(false);
            me->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
            EnterPhase(PHASE_GROUND);

			instance->SetData(DATA_FELMYST_EVENT, IN_PROGRESS);
        }

        void AttackStart(Unit *who)
        {
            if (Phase != PHASE_FLIGHT)
                QuantumBasicAI::AttackStart(who);
        }

        void MoveInLineOfSight(Unit *who)
        {
            if (Phase != PHASE_FLIGHT)
                QuantumBasicAI::MoveInLineOfSight(who);
        }

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        void JustRespawned(){}

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

			instance->SetData(DATA_FELMYST_EVENT, DONE);

			me->SummonCreature(NPC_KALECGOS_KILJAEDEN, 1482.01f, 652.476f, 82.3469f, 4.6111f, TEMPSUMMON_TIMED_DESPAWN, 30000);

			if (Creature* kalecgos = me->FindCreatureWithDistance(NPC_KALECGOS_KILJAEDEN, 250.0f , true))
				DoSendQuantumText(SAY_KALECGOS_OUTRO , kalecgos);
        }

        FelmystPosition GetFelymstPos()
        {
            return (me->GetPositionY() > MIDDLE_Y ? LEFT : RIGHT);
        }

        void GetNextFlyPos(float &x, float &y, float &z)
        {
            GetNextFlyPos(x, y, z,GetFelymstPos());
        }

        void GetRandFlyPos(float &x, float &y, float &z)
        {
            GetNextFlyPos(x, y, z,urand(0,1) == 1 ? LEFT : RIGHT);
        }

        void GetNextFlyPos(float &x, float &y, float &z, FelmystPosition position)
        {
            bool isLeft = (position == LEFT);

            if (isLeft)
            {
                x = float(urand(0,RightFlyPos[0][0] - RightFlyPos[1][0]) + RightFlyPos[1][0]);
                y = float(urand(0,RightFlyPos[0][1] - RightFlyPos[1][1]) + RightFlyPos[1][1]);
                z = 60.0f;
            }
			else
            {
                x = float(urand(0,LeftFlyPos[0][0] - LeftFlyPos[1][0]) + LeftFlyPos[1][0]);
                y = float(urand(0,LeftFlyPos[0][1] - LeftFlyPos[1][1]) + LeftFlyPos[1][1]);
                z = 60.0f;
            }
        }

        void KillCharmedPlayer()
        {
            InstanceMap::PlayerList const &playerliste = ((InstanceMap*)me->GetMap())->GetPlayers();
            InstanceMap::PlayerList::const_iterator it;

            Map::PlayerList const &PlayerList = ((InstanceMap*)me->GetMap())->GetPlayers();
            for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            {
                Player* player = i->getSource();
                if (player && player->IsAlive() && player->HasAuraEffect(SPELL_FOG_CHARM, 0))
                    player->DealDamage(player, player->GetHealth(), 0, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, 0, false);
            }
        }

        void SpellHit(Unit* /*caster*/, const SpellInfo* /*spell*/){}

        void JustSummoned(Creature *summon)
        {
            if (summon->GetEntry() == NPC_UNYIELDING_DEAD)
            {
                summon->AI()->AttackStart(SelectTarget(TARGET_RANDOM, 0));
                DoZoneInCombat(summon);
                summon->CastSpell(summon, SPELL_DEAD_PASSIVE, true);
            }
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            if (id == 5)
                me->RemoveAurasDueToSpell(SPELL_FOG_BREATH);

            Timer[EVENT_FLIGHT_SEQUENCE] = 1;
        }

        void DamageTaken(Unit*, uint32 &damage)
        {
            if (Phase != PHASE_GROUND && damage >= me->GetHealth())
                damage = 0;
        }

        void EnterPhase(PhaseFelmyst NextPhase)
        {
            switch(NextPhase)
            {
            case PHASE_GROUND:
                Timer[EVENT_CLEAVE] = 5000 + rand()%5 * 1000;
                Timer[EVENT_CORROSION] = 10000 + rand()%10 * 1000;
                Timer[EVENT_GAS_NOVA] = 15000 + rand()%5 * 1000;
                Timer[EVENT_ENCAPSULATE] = 20000 + rand()%5 * 1000;
                Timer[EVENT_FLIGHT] = 60000;
                DoStartMovement(me);
                break;
            case PHASE_FLIGHT:
                Timer[EVENT_FLIGHT_SEQUENCE] = 1000;
                Timer[EVENT_SUMMON_DEAD] = 0;
                Timer[EVENT_SUMMON_FOG] = 0;
                FlightCount = 0;
                BreathCount = 0;
                DoStartNoMovement(me);
                break;
            default:
                break;
            }
            Phase = NextPhase;
        }

        void HandleFlightSequence()
        {
            switch(FlightCount)
            {
            case 0:
                //me->AttackStop();
                sLog->OutErrorConsole("prevent fly phase");
                me->GetMotionMaster()->Clear(false);
				me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
				me->SetCanFly(true);
				me->SetDisableGravity(true);
                me->HandleEmoteCommand(EMOTE_ONESHOT_LIFT_OFF);
                me->StopMoving();
                DoSendQuantumText(SAY_TAKE_OFF, me);
                Timer[EVENT_FLIGHT_SEQUENCE] = 2000;
                break;
            case 1:
                sLog->OutErrorConsole("Move to Fly point");
                me->GetMotionMaster()->MovePoint(0, me->GetPositionX()+1, me->GetPositionY(), me->GetPositionZ()+10);
                Timer[EVENT_FLIGHT_SEQUENCE] = 0;
                break;
            case 2:
            {
                sLog->OutErrorConsole("Summon Vapor case 2");
                Unit* target;
                target = SelectTarget(TARGET_RANDOM, 0, 150, true);
                if (!target) target = Unit::GetUnit(*me, instance->GetData64(DATA_PLAYER_GUID));
                if (target)
                {
                    Creature* Vapor = me->SummonCreature(NPC_VAPOR, target->GetPositionX()-5+rand()%10, target->GetPositionY()-5+rand()%10, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 9000);
                    if (Vapor)
                    {
                        Vapor->AI()->AttackStart(target);
                        me->InterruptNonMeleeSpells(false);
                        me->CastSpell(Vapor, SPELL_VAPOR_CHANNEL, false); // core bug
                        Vapor->CastSpell(Vapor, SPELL_VAPOR_TRIGGER, true);
                    }
                }
                else
                {
                    //EnterEvadeMode();
                    //return;
                }
                Timer[EVENT_FLIGHT_SEQUENCE] = 10000;
                break;
            }
            case 3:
            {
                DespawnSummons(NPC_VAPOR_TRAIL);
                sLog->OutErrorConsole("Summon Vapor case3");
                //me->CastSpell(me, SPELL_VAPOR_SELECT); need core support
                Unit* target;
                target = SelectTarget(TARGET_RANDOM, 0, 150, true);
                if (!target) target = Unit::GetUnit(*me, instance->GetData64(DATA_PLAYER_GUID));
                if (target)
                {
                    Creature* Vapor = me->SummonCreature(NPC_VAPOR, target->GetPositionX()-5+rand()%10, target->GetPositionY()-5+rand()%10, target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 9000);
                    if (Vapor)
                    {
                        Vapor->AI()->AttackStart(target);
                        me->InterruptNonMeleeSpells(false);
                        me->CastSpell(Vapor, SPELL_VAPOR_CHANNEL, false); // core bug
                        Vapor->CastSpell(Vapor, SPELL_VAPOR_TRIGGER, true);
                    }
                }
                else
                {
                    //EnterEvadeMode();
                    //return;
                }
                Timer[EVENT_FLIGHT_SEQUENCE] = 10000;
                break;
            }
            case 4:
                DespawnSummons(NPC_VAPOR_TRAIL);
                Timer[EVENT_FLIGHT_SEQUENCE] = 1;
                break;
            case 5:
            {
                //Unit* target;
                //target = SelectTarget(TARGET_RANDOM, 0, 150, true);
                //if (!target) target = Unit::GetUnit(*me, instance->GetData64(DATA_PLAYER_GUID));
                //if (target)
                //{
                    //BreathX = target->GetPositionX();
                    //BreathY = target->GetPositionY();
                    float x, y, z;
                    //target->GetContactPoint(me, x, y, z, 70);
                    GetRandFlyPos(x, y, z);
                    me->GetMotionMaster()->MovePoint(0, x, y, z);
                    //if (rand()%2 == 0)
                    //    me->GetMotionMaster()->MovePoint(0, FLIGHT_1_X , FLIGHT_1_Y, FLIGHT_1_Z);
                    //else
                    //    me->GetMotionMaster()->MovePoint(0, FLIGHT_2_X , FLIGHT_2_Y, FLIGHT_2_Z);
                //}else
                //{
                //    EnterEvadeMode();
                //    return;
                //}
                Timer[EVENT_FLIGHT_SEQUENCE] = 0;
                break;
            }
            case 6:
                //me->SetOrientation(me->GetAngle(BreathX, BreathY));
                me->SetOrientation(me->GetAngle(me->GetPositionX(), MIDDLE_Y));
                me->StopMoving();
                //DoTextEmote("takes a deep breath.", NULL);
                Timer[EVENT_FLIGHT_SEQUENCE] = 10000;
                break;
            case 7:
                me->CastSpell(me, SPELL_FOG_BREATH, true);
                {
                    float x, y, z;
                    GetNextFlyPos(x, y, z);
                    //me->GetPosition(x, y, z);
                    //x = 2 * BreathX - x;
                    //y = 2 * BreathY - y;
                    //if (me->GetPositionY() > 600)
                    //    me->GetMotionMaster()->MovePoint(0, FLIGHT_1_X , FLIGHT_1_Y, FLIGHT_1_Z);
                    //else
                    //    me->GetMotionMaster()->MovePoint(0, FLIGHT_2_X , FLIGHT_2_Y, FLIGHT_2_Z);
                    me->GetMotionMaster()->MovePoint(5, x, y, z);
                }
                Timer[EVENT_SUMMON_FOG] = 1;
                Timer[EVENT_FLIGHT_SEQUENCE] = 0;
                break;
            case 8:
                me->RemoveAurasDueToSpell(SPELL_FOG_BREATH);
                BreathCount++;
                Timer[EVENT_SUMMON_FOG] = 0;
                Timer[EVENT_FLIGHT_SEQUENCE] = 1;
                if (BreathCount < 3) FlightCount = 5;
                break;
            case 9:
                if (Unit* target = SelectTarget(TARGET_TOP_AGGRO, 0))
                {
                    float x, y, z;
                    target->GetContactPoint(me, x, y, z);
                    me->GetMotionMaster()->MovePoint(0, x, y, z);
                }
                else
                {
                    float x, y, z;
                    x = 1435;
                    y = MIDDLE_Y;
                    z = 0;
                    me->UpdateGroundPositionZ(x, y, z);
                    me->GetMotionMaster()->MovePoint(0, x, y, z);
                    //EnterEvadeMode();
                    //return;
                }
                Timer[EVENT_FLIGHT_SEQUENCE] = 0;
                break;
            case 10:
				me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
				me->SetCanFly(false);
				me->SetDisableGravity(false);
                me->StopMoving();
                me->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
                EnterPhase(PHASE_GROUND);
                DoStartMovement(SelectTarget(TARGET_TOP_AGGRO, 0));
                break;
            default:
                break;
            }
            FlightCount++;
        }

        void UpdateAI(const uint32 diff)
        {
			if (instance && instance->GetData(DATA_BRUTALLUS_EVENT) == DONE)
				me->SetVisible(true);

            Event = EVENT_NULL;
            for(int i = 1; i <= MaxTimer[Phase]; i++)
            {
                if (Timer[i])
				{
                    if (Timer[i] <= diff)
                    {
                        if (Event == EVENT_NULL)
                            Event = (EventFelmyst)i;
                    }
					else Timer[i] -= diff;
				}
            }

            if (Phase == PHASE_GROUND)
            {
                if (!UpdateVictim())
                    return;

				if (!me->GetVictim() && me->getThreatManager().isThreatListEmpty())
				{
					EnterEvadeMode();
					return;
				}

                switch(Event)
                {
                case EVENT_BERSERK:
                    DoSendQuantumText(SAY_BERSERK, me);
                    me->CastSpell(me, SPELL_BERSERK, true);
                    Timer[EVENT_BERSERK] = 10000;
                    break;
                case EVENT_CLEAVE:
                    me->CastSpell(me->GetVictim(), SPELL_CLEAVE, false);
                    Timer[EVENT_CLEAVE] = 5000 + rand()%5 * 1000;
                    break;
                case EVENT_CORROSION:
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        me->CastSpell(me->GetVictim(), SPELL_CORROSION, false);
                        Timer[EVENT_CORROSION] = 20000 + rand()%10 * 1000;
                    }
                    break;
                case EVENT_GAS_NOVA:
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        me->CastSpell(me, SPELL_GAS_NOVA, false);
                        Timer[EVENT_GAS_NOVA] = 20000 + rand()%5 * 1000;
                    }
                    break;
                case EVENT_ENCAPSULATE:
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 150, true))
                        {
                            me->CastSpell(target, SPELL_ENCAPSULATE_CHANNEL, false);
                            Timer[EVENT_ENCAPSULATE] = 25000 + rand()%5 * 1000;
                        }
                    }
                    break;
                case EVENT_FLIGHT:
                    EnterPhase(PHASE_FLIGHT);
                    break;
                default:
                    DoMeleeAttackIfReady();
                    break;
                }
            }

            if (Phase == PHASE_FLIGHT)
            {
                switch(Event)
                {
                case EVENT_BERSERK:
                    DoSendQuantumText(SAY_BERSERK, me);
                    me->CastSpell(me, SPELL_BERSERK, true);
                    Timer[EVENT_BERSERK] = 0;
                    break;
                case EVENT_FLIGHT_SEQUENCE:
                    HandleFlightSequence();
                    break;
                case EVENT_SUMMON_FOG:
                    {
                        float x, y, z;
                        me->GetPosition(x, y, z);
                        me->UpdateGroundPositionZ(x, y, z);
                        if (Creature* fog = me->SummonCreature(NPC_VAPOR_TRAIL, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN, 10000))
                        {
                            fog->SetCurrentFaction(16);
                            fog->RemoveAurasDueToSpell(SPELL_TRAIL_TRIGGER);
                            fog->CastSpell(fog, SPELL_FOG_TRIGGER, true);
                        }
                    }
                    Timer[EVENT_SUMMON_FOG] = 1000;
                    break;
                default:
                    break;
                }
            }
        }

        void DespawnSummons(uint32 entry)
        {
            std::list<Creature*> templist;
            GetCreatureListWithEntryInGrid(templist,me,entry,100);

            for(std::list<Creature*>::iterator i = templist.begin(); i != templist.end(); ++i)
            {
                if (entry == NPC_VAPOR_TRAIL && Phase == PHASE_FLIGHT)
                {
                    float x, y, z;
                    me->GetPosition(x, y, z);
                    me->SummonCreature(NPC_UNYIELDING_DEAD, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
                }

				//me->SetVisible(false);
                //me->setDeathState(JUST_DIED);
                //if (me->getDeathState() == CORPSE)
					//me->RemoveCorpse();
            }
        }
	};

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_felmystAI(creature);
    }
};

class mob_felmyst_vapor : public CreatureScript
{
public:
    mob_felmyst_vapor() : CreatureScript("mob_felmyst_vapor") {}

    struct mob_felmyst_vaporAI : public QuantumBasicAI
    {
        mob_felmyst_vaporAI(Creature* creature) : QuantumBasicAI(creature)
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            me->SetSpeed(MOVE_RUN, 0.8f);
        }
        void Reset() {}

        void EnterToBattle(Unit* /*who*/)
        {
            DoZoneInCombat();
            //me->CastSpell(me, SPELL_VAPOR_FORCE, true); core bug
        }
        void UpdateAI(const uint32 diff)
        {
            if (!me->GetVictim())
                AttackStart(SelectTarget(TARGET_RANDOM, 0));
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_felmyst_vaporAI (creature);
    }
};

class mob_felmyst_trail : public CreatureScript
{
public:
    mob_felmyst_trail() : CreatureScript("mob_felmyst_trail") {}

    struct mob_felmyst_trailAI : public QuantumBasicAI
    {
        mob_felmyst_trailAI(Creature* creature) : QuantumBasicAI(creature)
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->CastSpell(me, SPELL_TRAIL_TRIGGER, true);
            me->SetTarget(me->GetGUID());
            me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 0.01f);
            instance = creature->GetInstanceScript();
        }

        InstanceScript *instance;
        uint32 FogtriggerTimer;

        void SpellHitTarget(Unit *target, const SpellInfo *spell)
        {
            if (target->GetTypeId() != TYPE_ID_PLAYER)
                return;

            if (spell->Id == SPELL_FOG_FORCE)
            {
                if (Creature* Felmyst = Creature::GetCreature(*me, instance->GetData64(DATA_FELMYST)))
				{
                    if (!target->HasAura(SPELL_FOG_CHARM2, 0) && !target->HasAura(SPELL_FOG_CHARM, 0))
                    {
                        Felmyst->CastSpell(target, SPELL_FOG_CHARM2, true);
                        Felmyst->CastSpell(target, SPELL_FOG_CHARM, true);
                    }
				}
            }
        }

        void Reset()
        {
            FogtriggerTimer = 5000;
        }

        void EnterToBattle(Unit* /*who*/){}

        void AttackStart(Unit* /*who*/){}

        void MoveInLineOfSight(Unit* /*who*/){}

        void UpdateAI(const uint32 diff)
        {
            if (me->HasAuraEffect(SPELL_TRAIL_TRIGGER,0))
                return;

            //Workaround
            if (FogtriggerTimer <= diff)
            {
                Map *map = me->GetMap();

                if (!map->IsDungeon())
                    return;
                Map::PlayerList const &PlayerList = map->GetPlayers();
                for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                {
                    if (Player* player = i->getSource())
					{
                        if (player->IsAlive() && me->IsInDist2d(player, 20))
                        {
                            const SpellInfo* spellinfo = sSpellMgr->GetSpellInfo(SPELL_FOG_FORCE);
                            SpellHitTarget(player, spellinfo);
                        }
					}
                }
                FogtriggerTimer = 1000;
            }
			else FogtriggerTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_felmyst_trailAI (creature);
    }
};

void AddSC_boss_felmyst()
{
    new boss_felmyst();
    new mob_felmyst_vapor();
    new mob_felmyst_trail();
}