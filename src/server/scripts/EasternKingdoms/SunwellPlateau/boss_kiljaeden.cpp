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
#include <math.h>
#include "sunwell_plateau.h"

enum Texts
{
    SAY_KJ_OFF_COMBAT_1                         = -1580066,
    SAY_KJ_OFF_COMBAT_2                         = -1580067,
    SAY_KJ_OFF_COMBAT_3                         = -1580068,
    SAY_KJ_OFF_COMBAT_4                         = -1580069,
    SAY_KJ_OFF_COMBAT_5                         = -1580070,

    SAY_KJ_EMERGE                               = -1580071,
    SAY_KJ_SLAY1                                = -1580072,
    SAY_KJ_SLAY2                                = -1580073,
    SAY_KJ_REFLECTION_1                         = -1580074,
    SAY_KJ_REFLECTION_2                         = -1580075,
    SAY_KJ_DARKNESS_1                           = -1580076,
    SAY_KJ_DARKNESS_2                           = -1580077,
    SAY_KJ_DARKNESS_3                           = -1580078,
    SAY_KJ_PHASE_3                              = -1580079,
    SAY_KJ_PHASE_4                              = -1580080,
    SAY_KJ_PHASE_5                              = -1580081,
    SAY_KJ_DEATH                                = -1580093,
    EMOTE_KJ_DARKNESS                           = -1580094,

    SAY_KALECGOS_AWAKEN                         = -1580082,
    SAY_ANVEENA_IMPRISONED                      = -1580083,
    SAY_KALECGOS_LETGO                          = -1580084,
    SAY_ANVEENA_LOST                            = -1580085,
    SAY_KALECGOS_FOCUS                          = -1580086,
    SAY_ANVEENA_KALEC                           = -1580087,
    SAY_KALECGOS_FATE                           = -1580088,
    SAY_ANVEENA_GOODBYE                         = -1580089,
    SAY_KALECGOS_GOODBYE                        = -1580090,
    SAY_KALECGOS_ENCOURAGE                      = -1580091,
    SAY_KALECGOS_JOIN                           = -1580092,
    SAY_KALEC_ORB_READY_1                       = -1580095,
    SAY_KALEC_ORB_READY_2                       = -1580096,
    SAY_KALEC_ORB_READY_3                       = -1580097,
    SAY_KALEC_ORB_READY_4                       = -1580098,
};

enum Spells
{
    SPELL_SHADOW_BOLT_VOLLEY                    = 45770,
    SPELL_SHADOW_INFUSION                       = 45772,
    SPELL_FELFIRE_PORTAL                        = 46875,
    SPELL_SHADOW_CHANNELING                     = 46757,
    SPELL_FELFIRE_FISSION                       = 45779,
    SPELL_TRANS                                 = 23188,
    SPELL_REBIRTH                               = 44200,
    SPELL_SOUL_FLAY                             = 45442,
    SPELL_SOUL_FLAY_SLOW                        = 47106,
    SPELL_LEGION_LIGHTNING                      = 45664,
    SPELL_FIRE_BLOOM                            = 45641,
    SPELL_DESTROY_ALL_DRAKES                    = 46707,
    SPELL_SINISTER_REFLECTION                   = 45785,
	SPELL_CLONE_PLAYER                          = 57507,
    SPELL_SHADOW_SPIKE                          = 46680,
    SPELL_FLAME_DART                            = 45737,
    SPELL_DARKNESS_OF_A_THOUSAND_SOULS          = 46605,
    SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE   = 45657,
    SPELL_ARMAGEDDON_TRIGGER                    = 45909,
    SPELL_ARMAGEDDON_VISUAL                     = 45911,
    SPELL_ARMAGEDDON_VISUAL2                    = 45914,
    SPELL_ARMAGEDDON_VISUAL3                    = 24207,
    SPELL_ARMAGEDDON_SUMMON_TRIGGER             = 45921,
    SPELL_ARMAGEDDON_DAMAGE                     = 45915,
    SPELL_SHADOW_BOLT                           = 45680,
    SPELL_ANVEENA_PRISON                        = 46367,
    SPELL_ANVEENA_ENERGY_DRAIN                  = 46410,
    SPELL_SACRIFICE_OF_ANVEENA                  = 46474, 
    SPELL_SR_CURSE_OF_AGONY                     = 46190,
    SPELL_SR_SHADOW_BOLT                        = 47076,
    SPELL_SR_EARTH_SHOCK                        = 47071,
    SPELL_SR_FIREBALL                           = 47074,
    SPELL_SR_HEMORRHAGE                         = 45897,
    SPELL_SR_HOLY_SHOCK                         = 38921,
    SPELL_SR_HAMMER_OF_JUSTICE                  = 37369,
    SPELL_SR_HOLY_SMITE                         = 47077,
    SPELL_SR_RENEW                              = 47079,
    SPELL_SR_SHOOT                              = 16496,
    SPELL_SR_MULTI_SHOT                         = 48098,
    SPELL_SR_WING_CLIP                          = 40652,
    SPELL_SR_WHIRLWIND                          = 17207,
    SPELL_SR_MOONFIRE                           = 47072,
    SPELL_VENGEANCE_OF_THE_BLUE_FLIGHT          = 45839,
    SPELL_RING_OF_BLUE_FLAMES                   = 45825,
};

#define FLOOR_Z       28.050388f
#define SHIELD_ORB_Z  45.000f

enum Phase
{
    PHASE_DECEIVERS     = 1,
    PHASE_NORMAL        = 2,
    PHASE_DARKNESS      = 3,
    PHASE_ARMAGEDDON    = 4,
    PHASE_SACRIFICE     = 5,
};


enum KilJaedenTimers
{
    TIMER_SPEECH           = 0,
    TIMER_SOUL_FLAY        = 1,
    TIMER_LEGION_LIGHTNING = 2,
    TIMER_FIRE_BLOOM       = 3,
    TIMER_SUMMON_SHILEDORB = 4,
    TIMER_SHADOW_SPIKE     = 5,
    TIMER_FLAME_DART       = 6,
    TIMER_DARKNESS         = 7,
    TIMER_ORBS_EMPOWER     = 8,
    TIMER_ARMAGEDDON       = 9,
};

const Position DeceiverLocations[3] =
{
	{1684.7f, 614.42f, 28.058f, 0.698392f},
	{1682.95f, 637.75f, 27.9231f, 5.71709f},
	{1707.61f, 612.15f, 27.7946f, 1.99037f},
};

float ShieldOrbLocations[4][2]=
{
    {1698.900f, 627.870f},
    {12, 3.14f},
    {12, 3.14f/0.7f},
    {12, 3.14f*3.8f},
};

struct Speech
{
    int32 textid;
    uint32 creature, timer;
};

static Speech Speeches[] =
{
    {SAY_KJ_EMERGE,             DATA_KILJAEDEN,     0},
    {SAY_KALECGOS_JOIN,         DATA_KALECGOS_KJ,   26000},
    {SAY_KALECGOS_AWAKEN,       DATA_KALECGOS_KJ,   10000},
    {SAY_ANVEENA_IMPRISONED,    DATA_ANVEENA,       5000},
    {SAY_KJ_PHASE_3,            DATA_KILJAEDEN,     5000},
    {SAY_KALECGOS_LETGO,        DATA_KALECGOS_KJ,   10000},
    {SAY_ANVEENA_LOST,          DATA_ANVEENA,       8000},
    {SAY_KJ_PHASE_4,            DATA_KILJAEDEN,     7000},
    {SAY_KALECGOS_FOCUS,        DATA_KALECGOS_KJ,   4000},
    {SAY_ANVEENA_KALEC,         DATA_ANVEENA,       11000},
    {SAY_KALECGOS_FATE,         DATA_KALECGOS_KJ,   2000},
    {SAY_ANVEENA_GOODBYE,       DATA_ANVEENA,       6000},
    {SAY_KJ_PHASE_5,            DATA_KILJAEDEN,     5500},
    // use in End sequence?
    {SAY_KALECGOS_GOODBYE,      DATA_KALECGOS_KJ,   12000},
};

class boss_kalecgos_kj : public CreatureScript
{
public:
    boss_kalecgos_kj() : CreatureScript("boss_kalecgos_kj") { }

    struct boss_kalecgos_kjAI : public QuantumBasicAI
    {
        boss_kalecgos_kjAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
			me->SetCanFly(true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
        }

        InstanceScript* instance;
        uint8 OrbsEmpowered;
        uint8 EmpowerCount;

        void Reset()
        {
            OrbsEmpowered = 0;
            EmpowerCount = 0;

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetActive(true);

            for (uint8 i = 0; i < 4; ++i)
			{
                if (GameObject* pOrb = GetOrb(i))
                    pOrb->SetGoType(GAMEOBJECT_TYPE_BUTTON);
			}
        }

		GameObject* GetOrb(int32 index)
        {
            if (!instance)
                return NULL;

            switch (index)
            {
                case 0:
                    return instance->instance->GetGameObject(instance->GetData64(DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_1));
                case 1:
                    return instance->instance->GetGameObject(instance->GetData64(DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_2));
                case 2:
                    return instance->instance->GetGameObject(instance->GetData64(DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_3));
                case 3:
                    return instance->instance->GetGameObject(instance->GetData64(DATA_ORB_OF_THE_BLUE_DRAGONFLIGHT_4));
            }

            return NULL;
        }

        void ResetOrbs()
        {
            me->RemoveDynObject(SPELL_RING_OF_BLUE_FLAMES);
            for (uint8 i = 0; i < 4; ++i)
			{
                if (GameObject* pOrb = GetOrb(i))
                    pOrb->SetUInt32Value(GAMEOBJECT_FACTION, 0);
			}
        }

        void EmpowerOrb(bool all)
        {
            GameObject* pOrbEmpowered = GetOrb(OrbsEmpowered);
            if (!pOrbEmpowered)
                return;

            if (all)
            {
                me->RemoveDynObject(SPELL_RING_OF_BLUE_FLAMES);
                for (uint8 i = 0; i < 4; ++i)
                {
                    if (GameObject* pOrb = GetOrb(i))
                    {
                        pOrb->CastSpell(me, SPELL_RING_OF_BLUE_FLAMES);
                        pOrb->SetUInt32Value(GAMEOBJECT_FACTION, 35);
                        pOrb->SetActive(true);
                        pOrb->Refresh();
                    }
                }
                DoSendQuantumText(SAY_KALECGOS_ENCOURAGE, me);
            }
            else
            {
                if (GameObject* pOrb = GetOrb(urand(0, 3)))
                {
                    pOrb->CastSpell(me, SPELL_RING_OF_BLUE_FLAMES);
                    pOrb->SetUInt32Value(GAMEOBJECT_FACTION, 35);
                    pOrb->SetActive(true);
                    pOrb->Refresh();

                    OrbsEmpowered = (OrbsEmpowered+1)%4;

                    ++EmpowerCount;
                    switch (EmpowerCount)
                    {
                        case 1:
							DoSendQuantumText(SAY_KALEC_ORB_READY_1, me);
							break;
                        case 2:
							DoSendQuantumText(SAY_KALEC_ORB_READY_2, me);
							break;
                        case 3:
							DoSendQuantumText(SAY_KALEC_ORB_READY_3, me);
							break;
                        case 4:
							DoSendQuantumText(SAY_KALEC_ORB_READY_4, me);
							break;
                    }
                }
            }
        }

        void UpdateAI(const uint32 /*diff*/){}

        void SetRingOfBlueFlames()
        {
            me->RemoveDynObject(SPELL_RING_OF_BLUE_FLAMES);
            for (uint8 i = 0; i < 4; ++i)
            {
                if (GameObject* pOrb = GetOrb(i))
                {
                    if (pOrb->GetUInt32Value(GAMEOBJECT_FACTION) == 35)
                    {
                        pOrb->CastSpell(me, SPELL_RING_OF_BLUE_FLAMES);
                        pOrb->SetActive(true);
                        pOrb->Refresh();
                    }
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_kalecgos_kjAI(creature);
    }
};

class go_orb_of_the_blue_flight : public GameObjectScript
{
public:
    go_orb_of_the_blue_flight() : GameObjectScript("go_orb_of_the_blue_flight") { }

    bool OnGossipHello(Player* player, GameObject* go)
    {
        if (go->GetUInt32Value(GAMEOBJECT_FACTION) == 35)
        {
            InstanceScript* instance = go->GetInstanceScript();
            player->SummonCreature(NPC_POWER_OF_THE_BLUE_DRAGONFLIGHT, player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 121000);
            player->CastSpell(player, SPELL_VENGEANCE_OF_THE_BLUE_FLIGHT, false);
            go->SetUInt32Value(GAMEOBJECT_FACTION, 0);

            if (Creature* kalec = Unit::GetCreature(*player, instance->GetData64(DATA_KALECGOS_KJ)))
                CAST_AI(boss_kalecgos_kj::boss_kalecgos_kjAI, kalec->AI())->SetRingOfBlueFlames();

            go->Refresh();
        }
        return true;
    }
};

class npc_kiljaeden_controller : public CreatureScript
{
public:
    npc_kiljaeden_controller() : CreatureScript("npc_kiljaeden_controller") { }

    struct npc_kiljaeden_controllerAI : public QuantumBasicAI
    {
        npc_kiljaeden_controllerAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
			instance = creature->GetInstanceScript();
			SetCombatMovement(false);
			Reset();
        }

        InstanceScript* instance;
        SummonList Summons;

		bool SummonedDeceivers;
        bool KiljaedenDeath;

        uint32 RandomSayTimer;
        uint32 Phase;
        uint8 DeceiverDeathCount;

        void Reset()
        {
			if (Creature* KalecgosKJ = Unit::GetCreature(*me, instance->GetData64(DATA_KALECGOS_KJ)))
			{
				KalecgosKJ->Respawn();
				KalecgosKJ->GetMotionMaster()->MoveTargetedHome();
			}

			if (Creature* kj = me->FindCreatureWithDistance(NPC_KILJAEDEN, 250.0f, true))
				kj->DespawnAfterAction();

			instance->SetData(DATA_KILJAEDEN_EVENT, NOT_STARTED);

			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

            Phase = PHASE_DECEIVERS;

			if (Creature* pKalecKJ = Unit::GetCreature(*me, instance->GetData64(DATA_KALECGOS_KJ)))
				CAST_AI(boss_kalecgos_kj::boss_kalecgos_kjAI, pKalecKJ->AI())->ResetOrbs();

            DeceiverDeathCount = 0;

			SummonedDeceivers = false;
            KiljaedenDeath = false;

            RandomSayTimer = 2000;

            Summons.DespawnAll();
        }

		void JustSummoned(Creature* summon)
        {
            switch (summon->GetEntry())
            {
                case NPC_HAND_OF_THE_DECEIVER:
					summon->CastSpell(summon, 55086, true);
					summon->CastSpell(summon, SPELL_SHADOW_CHANNELING, true);
					Summons.Summon(summon);
                    break;
                case NPC_ANVEENA:
                    summon->AddUnitMovementFlag(MOVEMENTFLAG_ON_TRANSPORT | MOVEMENTFLAG_DISABLE_GRAVITY);
                    summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
					summon->CastSpell(summon, SPELL_ANVEENA_PRISON, true);
					Summons.Summon(summon);
                    break;
                case NPC_KILJAEDEN:
                    summon->CastSpell(summon, SPELL_REBIRTH, false);
					summon->AI()->DoZoneInCombat();
                    break;
				default:
					break;
            }
        }

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

        void UpdateAI(const uint32 diff)
        {
            if (RandomSayTimer <= diff && !me->IsInCombatActive())
            {
				if (Creature* muru = me->GetCreature(*me, instance->GetData64(DATA_MURU)))
				{
					if (!muru->IsAlive() && instance->GetData(DATA_KILJAEDEN_EVENT) == NOT_STARTED)
					{
						DoSendQuantumText(RAND(SAY_KJ_OFF_COMBAT_1, SAY_KJ_OFF_COMBAT_2, SAY_KJ_OFF_COMBAT_3, SAY_KJ_OFF_COMBAT_4, SAY_KJ_OFF_COMBAT_5), me);
						RandomSayTimer = 30000;
					}
				}
            }
			else RandomSayTimer -= diff;

			if (!SummonedDeceivers)
            {
                for (uint8 i = 0; i < 3; ++i)
                    me->SummonCreature(NPC_HAND_OF_THE_DECEIVER, DeceiverLocations[i], TEMPSUMMON_DEAD_DESPAWN, 35000);

                me->SummonCreature(NPC_ANVEENA, 1700.37f, 628.229f, 68.4661f, 3.97613f, TEMPSUMMON_DEAD_DESPAWN, 35000);
                DoCast(me, SPELL_ANVEENA_ENERGY_DRAIN);
                SummonedDeceivers = true;
            }

            if (DeceiverDeathCount > 2 && Phase == PHASE_DECEIVERS)
            {
                me->RemoveAurasDueToSpell(SPELL_ANVEENA_ENERGY_DRAIN);
                Phase = PHASE_NORMAL;
				me->SummonCreature(NPC_KILJAEDEN, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 7*DAY);
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_kiljaeden_controllerAI(creature);
    }
};

class boss_kiljaeden : public CreatureScript
{
public:
    boss_kiljaeden() : CreatureScript("boss_kiljaeden") { }

    struct boss_kiljaedenAI : public QuantumBasicAI
    {
        boss_kiljaedenAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();

			me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_SINISTER_REFLECTION, true);
			me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_CLONE_PLAYER, true);

			SetCombatMovement(false);
        }

        InstanceScript* instance;
        SummonList Summons;

        uint8 Phase;
        uint8 ActiveTimers;
        uint32 SpeechTimer;

        uint32 Timer[10];
        uint32 WaitTimer;
        uint8 speechCount;
        uint8 speechPhaseEnd;

        bool IsInDarkness;
        bool TimerIsDeactivated[10];
        bool IsWaiting;
        bool OrbActivated;
        bool SpeechBegins;

        void InitializeAI()
        {
            QuantumBasicAI::InitializeAI();
        }

        void Reset()
        {
            TimerIsDeactivated [TIMER_SPEECH] = false;
            Timer[TIMER_SPEECH] = 0;

            //Phase 2 Timer
            Timer[TIMER_SOUL_FLAY] = 11000;
            Timer[TIMER_LEGION_LIGHTNING] = 30000;
            Timer[TIMER_FIRE_BLOOM] = 20000;
            Timer[TIMER_SUMMON_SHILEDORB] = 35000;

            // Phase 3 Timer
            Timer[TIMER_SHADOW_SPIKE] = 4000;
            Timer[TIMER_FLAME_DART] = 3000;
            Timer[TIMER_DARKNESS] = 45000;
            Timer[TIMER_ORBS_EMPOWER] = 35000;

            // Phase 4 Timer
            Timer[TIMER_ARMAGEDDON] = 2000;

            ActiveTimers = 5;
            WaitTimer = 0;
            speechCount = 0;
            SpeechTimer = 0;

            Phase = PHASE_NORMAL;

            IsInDarkness = false;
            IsWaiting = false;
            OrbActivated = false;
            SpeechBegins = true;

			instance->SetData(DATA_KILJAEDEN_EVENT, NOT_STARTED);

			if (Creature* pKalec = Unit::GetCreature(*me, instance->GetData64(DATA_KALECGOS_KJ)))
				pKalec->RemoveDynObject(SPELL_RING_OF_BLUE_FLAMES);

            me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 12);

            ChangeTimers(false, 0);

            Summons.DespawnAll();
        }

		void EnterToBattle(Unit* /*who*/)
		{
			instance->SetData(DATA_KILJAEDEN_EVENT, IN_PROGRESS);
		}

		void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_KJ_SLAY1, SAY_KJ_SLAY2), me);
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_KJ_DEATH, me);

            Summons.DespawnAll();

			instance->SetData(DATA_KILJAEDEN_EVENT, DONE);

			// Controller Despawn
			if (Creature* control = me->FindCreatureWithDistance(NPC_KILJAEDEN_CONTROLLER, 300.0f, true))
				control->Kill(control);

			// Anveena Despawn
			if (Creature* anveena = me->FindCreatureWithDistance(NPC_ANVEENA, 300.0f, true))
				anveena->DespawnAfterAction();

			// Kalecgos Despawn
			if (Creature* kalecgos = me->FindCreatureWithDistance(NPC_KALECGOS_KILJAEDEN, 300.0f, true))
				kalecgos->DespawnAfterAction();
        }

        void EnterEvadeMode()
        {
            QuantumBasicAI::EnterEvadeMode();

            Summons.DespawnAll();

			// Reset Controller
			if (Creature* control = me->FindCreatureWithDistance(NPC_KILJAEDEN_CONTROLLER, 250.0f, true))
			{
				control->Kill(control);
				control->Respawn();
			}
        }

		void JustSummoned(Creature* summon)
        {
            if (summon->GetEntry() == NPC_ARMAGEDDON_TARGET)
                summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            else
				summon->SetLevel(me->GetCurrentLevel());

            summon->SetCurrentFaction(me->GetFaction());
            Summons.Summon(summon);
        }

		void ChangeTimers(bool status, uint32 WTimer)
        {
            for (uint8 i = 1; i < ActiveTimers; ++i)
                TimerIsDeactivated[i] = status;

            if (WTimer > 0)
            {
                IsWaiting = true;
                WaitTimer = WTimer;
            }

            if (OrbActivated)
                TimerIsDeactivated[TIMER_ORBS_EMPOWER] = true;
            if (Timer[TIMER_SHADOW_SPIKE] == 0)
                TimerIsDeactivated[TIMER_SHADOW_SPIKE] = true;
            if (Phase == PHASE_SACRIFICE)
                TimerIsDeactivated[TIMER_SUMMON_SHILEDORB] = true;
        }

        void EnterNextPhase()
        {
            SpeechBegins = true;
            OrbActivated = false;
            ChangeTimers(true, 0);
            TimerIsDeactivated[TIMER_SHADOW_SPIKE] = false;
            Timer[TIMER_SHADOW_SPIKE] = 100;
            Timer[TIMER_DARKNESS] = (Phase == PHASE_SACRIFICE) ? 15000 : urand(10000, 40000);
            Timer[TIMER_ORBS_EMPOWER] = (Phase == PHASE_SACRIFICE) ? 10000 : 5000;
        }

        void CastSinisterReflection()
        {
            DoSendQuantumText(RAND(SAY_KJ_REFLECTION_1, SAY_KJ_REFLECTION_2), me);
            for (uint8 i = 0; i < 4; ++i)
            {
                float x, y, z;
                Unit* target = NULL;
                for (uint8 i = 0; i < 6; ++i)
                {
                    target = SelectTarget(TARGET_RANDOM, 0, 100, true);
                    if (!target || !target->HasAura(SPELL_VENGEANCE_OF_THE_BLUE_FLIGHT, 0))
                        break;
                }

                if (target)
                {
                    target->GetPosition(x, y, z);
                    if (Creature* sinister = me->SummonCreature(NPC_SINISTER_REFLECTION, x, y, z, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 15*IN_MILLISECONDS))
                    {
						target->CastSpell(sinister, SPELL_CLONE_PLAYER, true);
						target->CastSpell(sinister, SPELL_SINISTER_REFLECTION, true);
                        sinister->AI()->AttackStart(target);
                    }
                }
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim() || Phase < PHASE_NORMAL)
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (IsWaiting)
            {
                if (WaitTimer <= diff)
                {
                    IsWaiting = false;
                    ChangeTimers(false, 0);
                }
				else WaitTimer -= diff;
            }

			for (uint8 t = 0; t < ActiveTimers; ++t)
            {
                if (Timer[t] <= diff && !TimerIsDeactivated[t])
                {
                    switch (t)
                    {
                        case TIMER_SPEECH:
                            if (SpeechBegins)
                            {
                                SpeechBegins=false;
                                switch (Phase)
                                {
                                    case PHASE_NORMAL:
                                        speechPhaseEnd=1;
                                        break;
                                    case PHASE_DARKNESS:
                                        speechPhaseEnd=4;
                                        break;
                                    case PHASE_ARMAGEDDON:
                                        speechPhaseEnd=7;
                                        break;
                                    case PHASE_SACRIFICE:
                                        speechPhaseEnd=12;
                                        break;
                                }
                            }
                            if (Speeches[speechCount].timer < SpeechTimer)
                            {
                                SpeechTimer = 0;

								if (Creature* pSpeechCreature = Unit::GetCreature(*me, instance->GetData64(Speeches[speechCount].creature)))
									DoSendQuantumText(Speeches[speechCount].textid, pSpeechCreature);

								if (speechCount == 12)
									if (Creature* anveena =  Unit::GetCreature(*me, instance->GetData64(DATA_ANVEENA)))
										anveena->CastSpell(me, SPELL_SACRIFICE_OF_ANVEENA, false);
								//   ChangeTimers(true, 10000); // Kil should do an emote while screaming without attacking for 10 seconds
								if (speechCount == speechPhaseEnd)
									TimerIsDeactivated[TIMER_SPEECH]=true;
								speechCount++;
                            }
                            SpeechTimer += diff;
                            break;
                        case TIMER_SOUL_FLAY:
							//DoCastVictim(SPELL_SOUL_FLAY_SLOW, true);
							if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
							{
								DoCast(target, SPELL_SOUL_FLAY, true);
								Timer[TIMER_SOUL_FLAY] = 3500;
							}
                            break;
                        case TIMER_LEGION_LIGHTNING:

							if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
							{
								me->RemoveAurasDueToSpell(SPELL_SOUL_FLAY);
								DoCast(target, SPELL_LEGION_LIGHTNING);
							}

							Timer[TIMER_LEGION_LIGHTNING] = (Phase == PHASE_SACRIFICE) ? 18000 : 30000; // 18 seconds in PHASE_SACRIFICE
							Timer[TIMER_SOUL_FLAY] = 2500;
							break;
                        case TIMER_FIRE_BLOOM:
							me->RemoveAurasDueToSpell(SPELL_SOUL_FLAY);
							DoCastAOE(SPELL_FIRE_BLOOM, true);
							Timer[TIMER_FIRE_BLOOM] = (Phase == PHASE_SACRIFICE) ? 25000 : 40000; // 25 seconds in PHASE_SACRIFICE
							Timer[TIMER_SOUL_FLAY] = 1000;
                            break;
                        case TIMER_SUMMON_SHILEDORB:
                            for (uint8 i = 1; i < Phase; ++i)
                            {
                                float sx, sy;
                                sx = ShieldOrbLocations[0][0] + sin(ShieldOrbLocations[i][0]);
                                sy = ShieldOrbLocations[0][1] + sin(ShieldOrbLocations[i][1]);
                                me->SummonCreature(NPC_SHIELD_ORB, sx, sy, SHIELD_ORB_Z, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 45000);
                            }
                            Timer[TIMER_SUMMON_SHILEDORB] = urand(30000, 60000); // 30-60seconds cooldown
                            Timer[TIMER_SOUL_FLAY] = 2000;
                            break;
                        case TIMER_SHADOW_SPIKE: // Phase 3
							CastSinisterReflection();
							DoCast(me, SPELL_SHADOW_SPIKE, true);
							ChangeTimers(true, 30000);
							Timer[TIMER_SHADOW_SPIKE] = 0;
							TimerIsDeactivated[TIMER_SPEECH] = false;
                            break;
                        case TIMER_FLAME_DART: // Phase 3
                            DoCastAOE(SPELL_FLAME_DART, false);
                            Timer[TIMER_FLAME_DART] = 3000; //TODO Timer
                            break;
                        case TIMER_DARKNESS: // Phase 3
                            if (!me->IsNonMeleeSpellCasted(false))
                            {
                                // Begins to channel for 8 seconds, then deals 50'000 damage to all raid members.
                                if (!IsInDarkness)
                                {
                                    DoSendQuantumText(EMOTE_KJ_DARKNESS, me);
                                    DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS, false);
                                    ChangeTimers(true, 9000);
                                    Timer[TIMER_DARKNESS] = 8750;
                                    TimerIsDeactivated[TIMER_DARKNESS] = false;
                                    if (Phase == PHASE_SACRIFICE)
                                        TimerIsDeactivated[TIMER_ARMAGEDDON] = false;
                                    IsInDarkness = true;
                                }
                                else
                                {
                                    Timer[TIMER_DARKNESS] = (Phase == PHASE_SACRIFICE) ? 15000 : urand(40000, 70000);
                                    IsInDarkness = false;
                                    DoCastAOE(SPELL_DARKNESS_OF_A_THOUSAND_SOULS_DAMAGE);
                                    DoSendQuantumText(RAND(SAY_KJ_DARKNESS_1, SAY_KJ_DARKNESS_2, SAY_KJ_DARKNESS_3), me);
                                }
                                Timer[TIMER_SOUL_FLAY] = 9000;
                            }
                            break;
                        case TIMER_ORBS_EMPOWER: // Phase 3
							if (Creature* pKalec = Unit::GetCreature(*me, instance->GetData64(DATA_KALECGOS_KJ)))
							{
                                switch (Phase)
								{
                                    case PHASE_SACRIFICE:
                                        CAST_AI(boss_kalecgos_kj::boss_kalecgos_kjAI, pKalec->AI())->EmpowerOrb(true);
                                        break;
                                    default:
                                        CAST_AI(boss_kalecgos_kj::boss_kalecgos_kjAI, pKalec->AI())->EmpowerOrb(false);
                                        break;
								}
							}
                            OrbActivated = true;
                            TimerIsDeactivated[TIMER_ORBS_EMPOWER] = true;
                            break;
                        case TIMER_ARMAGEDDON: // Phase 4
                            Unit* target = NULL;
                            for (uint8 z = 0; z < 6; ++z)
                            {
                                target = SelectTarget(TARGET_RANDOM, 0, 100, true);
                                if (!target || !target->HasAura(SPELL_VENGEANCE_OF_THE_BLUE_FLIGHT, 0)) break;
                            }
                            if (target)
                            {
                                float x, y, z;
                                target->GetPosition(x, y, z);
                                me->SummonCreature(NPC_ARMAGEDDON_TARGET, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN, 15000);
                            }
                            Timer[TIMER_ARMAGEDDON] = 2000; // No, I'm not kidding
                            break;
                     }
                }
            }
            DoMeleeAttackIfReady();
            // Time runs over!
            for (uint8 i = 0; i < ActiveTimers; ++i)
			{
                if (!TimerIsDeactivated[i])
                {
                    Timer[i] -= diff;
                    if (((int32)Timer[i]) < 0) Timer[i] = 0;
                }
			}

            // Phase 3
            if (Phase <= PHASE_NORMAL && !IsInDarkness)
            {
                if (Phase == PHASE_NORMAL && HealthBelowPct(85))
                {
                    Phase = PHASE_DARKNESS;
                    ActiveTimers = 9;
                    EnterNextPhase();
                }
                else
					return;
            }

            // Phase 4
            if (Phase <= PHASE_DARKNESS && !IsInDarkness)
            {
                if (Phase == PHASE_DARKNESS && HealthBelowPct(55))
                {
                    Phase = PHASE_ARMAGEDDON;
                    ActiveTimers = 10;
                    EnterNextPhase();
                }
                else
					return;
            }

            if (Phase <= PHASE_ARMAGEDDON && !IsInDarkness)
            {
                if (Phase == PHASE_ARMAGEDDON && HealthBelowPct(25))
                {
                    Phase = PHASE_SACRIFICE;
                    EnterNextPhase();
                }
                else
					return;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_kiljaedenAI(creature);
    }
};

class npc_hand_of_the_deceiver : public CreatureScript
{
public:
    npc_hand_of_the_deceiver() : CreatureScript("npc_hand_of_the_deceiver") { }

    struct npc_hand_of_the_deceiverAI : public QuantumBasicAI
    {
        npc_hand_of_the_deceiverAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 ShadowBoltVolleyTimer;
        uint32 FelfirePortalTimer;

        void Reset()
        {
            ShadowBoltVolleyTimer = 500;
            FelfirePortalTimer = 5000;

			instance->SetData(DATA_KILJAEDEN_EVENT, NOT_STARTED);
        }

        void JustSummoned(Creature* summon)
        {
            summon->SetCurrentFaction(me->GetFaction());
            summon->SetLevel(me->GetCurrentLevel());
        }

        void EnterToBattle(Unit* who)
        {
			me->RemoveAurasDueToSpell(55086);

			instance->SetData(DATA_KILJAEDEN_EVENT, IN_PROGRESS);

			if (Creature* pControl = Unit::GetCreature(*me, instance->GetData64(DATA_KILJAEDEN_CONTROLLER)))
				pControl->AddThreat(who, 5.0f);

            me->InterruptNonMeleeSpells(true);
        }

        void JustDied(Unit* /*killer*/)
        {
            if (!instance)
                return;

            if (Creature* control = Unit::GetCreature(*me, instance->GetData64(DATA_KILJAEDEN_CONTROLLER)))
                ++(CAST_AI(npc_kiljaeden_controller::npc_kiljaeden_controllerAI, control->AI())->DeceiverDeathCount);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!me->IsInCombatActive())
                DoCast(me, SPELL_SHADOW_CHANNELING);

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (HealthBelowPct(20) && !me->HasAura(SPELL_SHADOW_INFUSION, 0))
                DoCast(me, SPELL_SHADOW_INFUSION, true);

            if (ShadowBoltVolleyTimer <= diff)
            {
                DoCastAOE(SPELL_SHADOW_BOLT_VOLLEY);
                ShadowBoltVolleyTimer = urand(3000, 4000);
            }
            else ShadowBoltVolleyTimer -= diff;

            // Felfire Portal - Creatres a portal, that spawns Volatile Felfire Fiends, which do suicide bombing.
            if (FelfirePortalTimer <= diff)
            {
                if (Creature* pPortal = DoSpawnCreature(NPC_FELFIRE_PORTAL, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN, 20000))
                {
                    std::list<HostileReference*>::iterator itr;
                    for (itr = me->getThreatManager().getThreatList().begin(); itr != me->getThreatManager().getThreatList().end(); ++itr)
                    {
                        Unit* unit = Unit::GetUnit(*me, (*itr)->getUnitGuid());
                        if (unit)
                            pPortal->AddThreat(unit, 1.0f);
                    }
                }
                FelfirePortalTimer = 12000;
            }
			else FelfirePortalTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hand_of_the_deceiverAI(creature);
    }
};

class npc_felfire_portal : public CreatureScript
{
public:
    npc_felfire_portal() : CreatureScript("npc_felfire_portal") { }

    struct npc_felfire_portalAI : public QuantumBasicAI
    {
        npc_felfire_portalAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        uint32 SpawnFiendTimer;

        void Reset()
        {
            SpawnFiendTimer = 3000;

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        }

        void JustSummoned(Creature* summon)
        {
            summon->SetCurrentFaction(me->GetFaction());
            summon->SetLevel(me->GetCurrentLevel());
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (SpawnFiendTimer <= diff)
            {
                if (Creature* fiend = DoSpawnCreature(NPC_VOLATILE_FELFIRE_FIEND, 0, 0, 0, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 20000))
				{
                    fiend->AddThreat(SelectTarget(TARGET_RANDOM, 0), 100000.0f);
					SpawnFiendTimer = urand(4000, 8000);
				}
            }
			else SpawnFiendTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_felfire_portalAI(creature);
    }
};

class npc_volatile_felfire_fiend : public CreatureScript
{
public:
    npc_volatile_felfire_fiend() : CreatureScript("npc_volatile_felfire_fiend") { }

    struct npc_volatile_felfire_fiendAI : public QuantumBasicAI
    {
        npc_volatile_felfire_fiendAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint32 ExplodeTimer;

        bool LockedTarget;

        void Reset()
        {
            ExplodeTimer = 2000;
            LockedTarget = false;
        }

        void DamageTaken(Unit* /*done_by*/, uint32 &damage)
        {
            if (damage > me->GetHealth())
                DoCast(me, SPELL_FELFIRE_FISSION, true);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (!LockedTarget)
            {
                me->AddThreat(me->GetVictim(), 10000000.0f);
                LockedTarget = true;
            }

            if (ExplodeTimer)
            {
                if (ExplodeTimer <= diff)
				{
					ExplodeTimer = 0;
				}
                else ExplodeTimer -= diff;
            }

            else if (me->IsWithinDistInMap(me->GetVictim(), 3.0f)) // Explode if it's close enough to it's target
            {
                DoCastVictim(SPELL_FELFIRE_FISSION);
                me->Kill(me);
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_volatile_felfire_fiendAI (creature);
    }
};

class npc_armageddon_target : public CreatureScript
{
public:
    npc_armageddon_target() : CreatureScript("npc_armageddon_target") { }

    struct npc_armageddon_targetAI : public QuantumBasicAI
    {
        npc_armageddon_targetAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

        uint8 Spell;
        uint32 Timer;

        void Reset()
        {
            Spell = 0;
            Timer = 0;
        }

        void UpdateAI(const uint32 diff)
        {
            if (Timer <= diff)
            {
                switch (Spell)
                {
                    case 0:
                        DoCast(me, SPELL_ARMAGEDDON_VISUAL, true);
                        ++Spell;
                        break;
                    case 1:
                        DoCast(me, SPELL_ARMAGEDDON_VISUAL2, true);
                        Timer = 9000;
                        ++Spell;
                        break;
                    case 2:
                        DoCast(me, SPELL_ARMAGEDDON_TRIGGER, true);
                        ++Spell;
                        Timer = 5000;
                        break;
                    case 3:
                        me->Kill(me);
                        me->RemoveCorpse();
                        break;
                }
            }
			else Timer -=diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_armageddon_targetAI(creature);
    }
};

class npc_shield_orb : public CreatureScript
{
public:
    npc_shield_orb() : CreatureScript("npc_shield_orb") { }

    struct npc_shield_orbAI : public QuantumBasicAI
    {
        npc_shield_orbAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();

			me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_SINISTER_REFLECTION, true);
			me->ApplySpellImmune(0, IMMUNITY_ID, SPELL_CLONE_PLAYER, true);
        }

        InstanceScript* instance;

		uint32 ShadowBoltTimer;
        uint32 CheckTimer;

        bool PointReached;
        bool Clockwise;
        float x, y, r, c, mx, my;

        void Reset()
        {
			me->SetCanFly(true);
            me->SetDisableGravity(true);
            PointReached = true;
            ShadowBoltTimer = 500;
            CheckTimer = 1000;
            r = 17;
            c = 0;
            mx = ShieldOrbLocations[0][0];
            my = ShieldOrbLocations[0][1];
            Clockwise = urand(0, 1);
			DoZoneInCombat();
        }

        void UpdateAI(const uint32 diff)
        {
            if (PointReached)
            {
                if (Clockwise)
                {
                    y = my - r * sin(c);
                    x = mx - r * cos(c);
                }
                else
                {
                    y = my + r * sin(c);
                    x = mx + r * cos(c);
                }
                PointReached = false;
                CheckTimer = 1000;
                me->GetMotionMaster()->MovePoint(1, x, y, SHIELD_ORB_Z);
                c += M_PI/32;
                if (c >= 2*M_PI) c = 0;
            }
            else
            {
                if (CheckTimer <= diff)
                {
                    DoTeleportTo(x, y, SHIELD_ORB_Z);
                    PointReached = true;
                }
                else CheckTimer -= diff;
            }

			if (!UpdateVictim())
				return;

            if (ShadowBoltTimer <= diff)
            {
				DoCastAOE(SPELL_SHADOW_BOLT, true);
				ShadowBoltTimer = 2000;
            }
			else ShadowBoltTimer -= diff;
        }

        void MovementInform(uint32 type, uint32 /*id*/)
        {
            if (type != POINT_MOTION_TYPE)
                return;

            PointReached = true;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shield_orbAI(creature);
    }
};

class npc_sinster_reflection : public CreatureScript
{
public:
    npc_sinster_reflection() : CreatureScript("npc_sinster_reflection") { }

    struct npc_sinster_reflectionAI : public QuantumBasicAI
    {
        npc_sinster_reflectionAI(Creature* creature) : QuantumBasicAI(creature) {}

        uint8 victimClass;
        uint32 Timer[3];

        void Reset()
        {
            Timer[0] = 0;
            Timer[1] = 0;
            Timer[2] = 0;
            victimClass = 0;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            if ((victimClass == 0) && me->GetVictim())
            {
                victimClass = me->GetVictim()->GetCurrentClass();
                switch (victimClass)
                {
                    case CLASS_DRUID:
                        break;
                    case CLASS_HUNTER:
						me->SetCanDualWield(true);
                        break;
                    case CLASS_MAGE:
                        break;
                    case CLASS_WARLOCK:
                        break;
                    case CLASS_WARRIOR:
                        me->SetCanDualWield(true);
                        break;
                    case CLASS_PALADIN:
                        break;
                    case CLASS_PRIEST:
                        break;
                    case CLASS_SHAMAN:
                        me->SetCanDualWield(true);
                        break;
                    case CLASS_ROGUE:
                        me->SetCanDualWield(true);
                        break;
                }
            }

            switch (victimClass)
			{
                case CLASS_DRUID:
                    if (Timer[1] <= diff)
                    {
                        DoCastVictim(SPELL_SR_MOONFIRE);
                        Timer[1] = urand(2000, 4000);
                    }
                    DoMeleeAttackIfReady();
                    break;
                case CLASS_HUNTER:
                    if (Timer[1] <= diff)
                    {
                        DoCastVictim(SPELL_SR_MULTI_SHOT);
                        Timer[1] = urand(8000, 10000);
                    }
                    if (Timer[2] <= diff)
                    {
                        DoCastVictim(SPELL_SR_SHOOT);
                        Timer[2] = urand(4000, 6000);
                    }
                    if (me->IsWithinMeleeRange(me->GetVictim(), 6))
                    {
                        if (Timer[0] <= diff)
                        {
                            DoCastVictim(SPELL_SR_MULTI_SHOT);
                            Timer[0] = urand(6000, 8000);
                        }
                        DoMeleeAttackIfReady();
                    }
                    break;
                case CLASS_MAGE:
                    if (Timer[1] <= diff)
                    {
                        DoCastVictim(SPELL_SR_FIREBALL);
                        Timer[1] = urand(2000, 4000);
                    }
                    DoMeleeAttackIfReady();
                    break;
                case CLASS_WARLOCK:
                    if (Timer[1] <= diff)
                    {
                        DoCastVictim(SPELL_SR_SHADOW_BOLT);
                        Timer[1] = urand(3000, 5000);
                    }
                    if (Timer[2] <= diff)
                    {
                        DoCast(SelectTarget(TARGET_RANDOM, 0, 100, true), SPELL_SR_CURSE_OF_AGONY, true);
                        Timer[2] = urand(2000, 4000);
                    }
                    DoMeleeAttackIfReady();
                    break;
                case CLASS_WARRIOR:
                    if (Timer[1] <= diff)
                    {
                        DoCastVictim(SPELL_SR_WHIRLWIND);
                        Timer[1] = urand(9000, 11000);
                    }
                    DoMeleeAttackIfReady();
                    break;
                case CLASS_PALADIN:
                    if (Timer[1] <= diff)
                    {
                        DoCastVictim(SPELL_SR_HAMMER_OF_JUSTICE);
                        Timer[1] = urand(6000, 8000);
                    }
                    if (Timer[2] <= diff)
                    {
                        DoCastVictim(SPELL_SR_HOLY_SHOCK);
                        Timer[2] = urand(2000, 4000);
                    }
                    DoMeleeAttackIfReady();
                    break;
                case CLASS_PRIEST:
                    if (Timer[1] <= diff)
                    {
                        DoCastVictim(SPELL_SR_HOLY_SMITE);
                        Timer[1] = urand(4000, 6000);
                    }
                    if (Timer[2] <= diff)
                    {
                        DoCast(me, SPELL_SR_RENEW);
                        Timer[2] = urand(6000, 8000);
                    }
                    DoMeleeAttackIfReady();
                    break;
                case CLASS_SHAMAN:
                    if (Timer[1] <= diff)
                    {
                        DoCastVictim(SPELL_SR_EARTH_SHOCK);
                        Timer[1] = urand(4000, 6000);
                    }
                    DoMeleeAttackIfReady();
                    break;
                case CLASS_ROGUE:
                    if (Timer[1] <= diff)
                    {
                        DoCastVictim(SPELL_SR_HEMORRHAGE);
                        Timer[1] = urand(4000, 6000);
                    }
                    DoMeleeAttackIfReady();
                    break;
                }
                for (uint8 i = 0; i < 3; ++i)
					Timer[i] -= diff;
         }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sinster_reflectionAI(creature);
    }
};

void AddSC_boss_kiljaeden()
{
    new go_orb_of_the_blue_flight();
    new boss_kalecgos_kj();
    new boss_kiljaeden();
    new npc_kiljaeden_controller();
    new npc_hand_of_the_deceiver();
    new npc_felfire_portal();
    new npc_volatile_felfire_fiend();
    new npc_armageddon_target();
    new npc_shield_orb();
    new npc_sinster_reflection();
}