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
#include "Group.h"
#include "ruby_sanctum.h"

enum Texts
{
    SAY_HALION_SPAWN                = -1666100,
    SAY_HALION_AGGRO                = -1666101,
    SAY_HALION_SLAY_1               = -1666102,
    SAY_HALION_SLAY_2               = -1666103,
    SAY_HALION_DEATH                = -1666104,
    SAY_HALION_BERSERK              = -1666105,
    SAY_HALION_METEOR_FLAME         = -1666106,
    SAY_HALION_DARK_SPECIAL         = -1666107,
    SAY_HALION_PHASE_2              = -1666108, 
    SAY_HALION_PHASE_3              = -1666109,
	EMOTE_WARNING					= -1666110,
    EMOTE_REAL_PUSH					= -1666111,
    EMOTE_REAL_PULL					= -1666112,
    EMOTE_TWIL_PUSH					= -1666113,
    EMOTE_TWIL_PULL					= -1666114,
	EMOTE_REGENERATE                = -1666115,
};

enum Spells
{
    SPELL_TWILIGHT_PRECISION        = 78243,
    SPELL_BERSERK                   = 26662,
    SPELL_START_PHASE2              = 74808,
    SPELL_TWILIGHT_ENTER            = 74807,
    SPELL_TWILIGHT_LEAVE            = 74812,
    SPELL_SUMMON_TWILIGHT_PORTAL    = 74809,
	SPELL_TWILIGHT_MENDING          = 75509,
    SPELL_FIRE_PILLAR               = 76006,
    SPELL_FIERY_EXPLOSION_VISUAL    = 76010,
    SPELL_TAIL_LASH                 = 74531,
    SPELL_TWILIGHT_DIVISION         = 75063,
    SPELL_TWILIGHT_CUTTER           = 77844,
    SPELL_TWILIGHT_CUTTER_CHANNEL   = 74768,
    SPELL_CORPOREALITY_EVEN         = 74826,
    SPELL_CORPOREALITY_20I          = 74827,
    SPELL_CORPOREALITY_40I          = 74828,
    SPELL_CORPOREALITY_60I          = 74829,
    SPELL_CORPOREALITY_80I          = 74830,
    SPELL_CORPOREALITY_100I         = 74831,
    SPELL_CORPOREALITY_20D          = 74832,
    SPELL_CORPOREALITY_40D          = 74833,
    SPELL_CORPOREALITY_60D          = 74834,
    SPELL_CORPOREALITY_80D          = 74835,
    SPELL_CORPOREALITY_100D         = 74836,
    SPELL_METEOR                    = 74637,
    SPELL_METEOR_IMPACT             = 74641,
    SPELL_METEOR_STRIKE             = 74648,
    SPELL_METEOR_FLAME              = 74718,
    SPELL_FLAME_BREATH              = 74525,
    SPELL_DARK_BREATH               = 74806,
    SPELL_DUSK_SHROUD               = 75484,
    NPC_COMBUSTION                  = 40001,
    SPELL_MARK_OF_COMBUSTION        = 74567,
    SPELL_FIERY_COMBUSTION          = 74562,
    SPELL_COMBUSTION_EXPLODE        = 74607,
    SPELL_COMBUSTION_AURA           = 74629,
    NPC_CONSUMPTION                 = 40135,
    SPELL_MARK_OF_CONSUMPTION       = 74795,
    SPELL_SOUL_CONSUMPTION          = 74792,
    SPELL_CONSUMPTION_EXPLODE       = 74799,
    SPELL_CONSUMPTION_AURA          = 74803,
    SPELL_GROW_UP                   = 36300,
};

struct Locations
{
    float x, y, z;
};

static Locations SpawnLoc[] =
{
    {3154.99f, 535.637f, 72.8887f},
};

uint32 Rdmg, Tdmg;

class boss_halion_real : public CreatureScript
{
public:
    boss_halion_real() : CreatureScript("boss_halion_real") { }

    struct boss_halion_realAI : public QuantumBasicAI
    {
        boss_halion_realAI(Creature* creature) : QuantumBasicAI(creature)
        {
			instance = creature->GetInstanceScript();
            Reset();
        }

        InstanceScript* instance;

        bool Intro;
		bool MovementStarted;

        uint8 stage;
        uint8 nextPoint;

        uint32 IntroTimer;
        uint32 IntroAppTimer;
        uint32 EnrageTimer;
        uint32 FlameBreathTimer;
        uint32 FieryCombustionTimer;
        uint32 MeteorFlameTimer;
        uint32 TailLashTimer;

        void Reset()
        {
            if (!instance)
				return;

            me->SetRespawnDelay(7*DAY);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);

			if (Creature* pClone = me->GetMap()->GetCreature(instance->GetData64(NPC_HALION_TWILIGHT)))
			{
				if (pClone->IsAlive())
					pClone->DespawnAfterAction();
			}

            if (me->IsAlive())
            {
                instance->SetData(TYPE_HALION, NOT_STARTED);
                instance->SetData(TYPE_HALION_EVENT, FAIL);
                me->SetDisplayId(MODEL_ID_INVISIBLE);
            }

            setStage(0);
            nextPoint = 0;

            Intro = false;

            IntroTimer = 1*IN_MILLISECONDS;
            IntroAppTimer = 30*IN_MILLISECONDS;
            EnrageTimer = 10*MINUTE*IN_MILLISECONDS;
            FlameBreathTimer = urand(10*IN_MILLISECONDS,18*IN_MILLISECONDS);
            FieryCombustionTimer = urand(30*IN_MILLISECONDS,40*IN_MILLISECONDS);
            MeteorFlameTimer = urand(30*IN_MILLISECONDS,35*IN_MILLISECONDS);
            TailLashTimer = urand(15*IN_MILLISECONDS,25*IN_MILLISECONDS);

            Rdmg = 0;

            SetCombatMovement(true);

            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

			if (GameObject* go = instance->instance->GetGameObject(instance->GetData64(GO_HALION_PORTAL_ENTER_TWILIGHT)))
				go->SetPhaseMask(2, true);
			if (GameObject* go = instance->instance->GetGameObject(instance->GetData64(GO_HALION_PORTAL_ENTER_TWILIGHT_1)))
				go->SetPhaseMask(2, true);
			if (GameObject* go = instance->instance->GetGameObject(instance->GetData64(GO_HALION_PORTAL_EXIT_TWILIGT)))
				go->SetPhaseMask(2, true);

			me->RemoveAurasDueToSpell(SPELL_TWILIGHT_ENTER);
        }

        void setStage(uint8 phase)
		{
			stage = phase;
		}

        uint8 getStage()
		{
			return stage;
		}

		void MoveInLineOfSight(Unit* who)
        {
            if (!instance) 
				return;

            if (!who || who->GetTypeId() != TYPE_ID_PLAYER) 
				return;

			if (instance->GetData(TYPE_ZARITHRIAN) == DONE)
			{
				if (Player* player = who->ToPlayer())
				{
					// Halion Event Start Only Rail Leader or Gm Master
					if (!Intro && player->IsWithinDist(me, 155.0f) && (!player->GetGroup() || !player->GetGroup()->IsLeader(player->GetGUID())) && !player->IsGameMaster())
					{
						DoSendQuantumText(SAY_HALION_SPAWN, me);
						Intro = true;
					}
				}

		        if (Intro && !me->IsInCombatActive() && who->IsWithinDistInMap(me, 30.0f))
			    {
					me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
					me->SetInCombatWith(who);
					SetCombatMovement(false);
					me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
					me->GetMotionMaster()->MoveIdle();
					setStage(10);
				}
			}
			else return;
        }

		void EnterToBattle(Unit* /*who*/)
        {
            if (!instance)
                return;

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            DoCast(SPELL_TWILIGHT_PRECISION);
            me->SetInCombatWithZone();

            instance->SetData(TYPE_HALION, IN_PROGRESS);

			instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me, 1);

            DoSendQuantumText(SAY_HALION_AGGRO, me);
        }

		void KilledUnit(Unit* who)
        {
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_HALION_SLAY_1, SAY_HALION_SLAY_2), me);
        }

        void JustReachedHome()
        {
            if (!instance)
                return;

            if (instance->GetData(TYPE_HALION_EVENT) != FAIL || getStage() == 0)
                return;

			instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

            QuantumBasicAI::JustReachedHome();
        }

        void EnterEvadeMode()
        {
            if (!instance)
                return;

            if (instance->GetData(TYPE_HALION_EVENT) != FAIL)
                return;

            QuantumBasicAI::EnterEvadeMode();
        }

        void JustDied(Unit* /*killer*/)
        {
            if (!instance)
                return;

			instance->SetData(TYPE_HALION, DONE);
            instance->SetData(TYPE_COUNTER, COUNTER_OFF);

			if (GameObject* go = instance->instance->GetGameObject(instance->GetData64(GO_HALION_PORTAL_ENTER_TWILIGHT)))
				go->SetPhaseMask(2, true);
			if (GameObject* go = instance->instance->GetGameObject(instance->GetData64(GO_HALION_PORTAL_ENTER_TWILIGHT_1)))
				go->SetPhaseMask(2, true);
			if (GameObject* go = instance->instance->GetGameObject(instance->GetData64(GO_HALION_PORTAL_EXIT_TWILIGT)))
				go->SetPhaseMask(2, true);

            DoSendQuantumText(SAY_HALION_DEATH, me);

			me->RemoveAllAuras();

            // Updating achievements for all players in the map
			Map::PlayerList const& Players = me->GetMap()->GetPlayers();
			for (Map::PlayerList::const_iterator itr = Players.begin(); itr != Players.end(); ++itr)
			{
				if (Player* player = itr->getSource())
					player->KilledMonsterCredit(me->GetEntry(), 0);
			}

			instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
        }

        void MovementInform(uint32 type, uint32 id)
        {
            if (!instance)
                return;

            if (type != POINT_MOTION_TYPE || !MovementStarted)
                return;

            if (id == nextPoint)
            {
                me->GetMotionMaster()->MovementExpired();
                MovementStarted = false;
            }
        }

        void StartMovement(uint32 id)
        {
            nextPoint = id;
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MovePoint(id, SpawnLoc[id].x, SpawnLoc[id].y, SpawnLoc[id].z);
            MovementStarted = true;
        }

        void DamageTaken(Unit* /*pDoneBy*/, uint32& damage)
        {
            if (!instance)
                return;

            Rdmg += damage;
            Creature* pHalionTwilight = me->GetMap()->GetCreature(instance->GetData64(NPC_HALION_TWILIGHT));

            if (!pHalionTwilight)
                return;

            if (pHalionTwilight->GetHealth()<=1 || !pHalionTwilight->IsAlive())
                return;

            if (damage < me->GetHealth())
                pHalionTwilight->SetHealth(me->GetHealth()-damage);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!instance)
                return;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            switch(getStage())
            {
                case 10: //PHASE INTRO
                    if (IntroTimer <= diff)
                    {
                        DoCast(SPELL_FIRE_PILLAR);
                        IntroAppTimer = 10*IN_MILLISECONDS;
                        IntroTimer = 30*IN_MILLISECONDS;
                    }
					else IntroTimer -= diff;

                    if (IntroAppTimer <= diff)
                    {
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_CREATURE);
                        me->RemoveFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_DISABLE_MOVE);
                        DoCast(SPELL_FIERY_EXPLOSION_VISUAL);
                        me->SetDisplayId(DISPLAY_ID_HALION_COMBAT);
                        SetCombatMovement(true);
                        me->GetMotionMaster()->MoveChase(me->GetVictim());
                        setStage(0);
                        IntroAppTimer = 30*IN_MILLISECONDS;
                    }
					else IntroAppTimer -= diff;
                     break;
                case 0: //PHASE 1 PHYSICAL REALM
                    if (FlameBreathTimer <= diff)
                    {
                        DoCast(SPELL_FLAME_BREATH);
                        FlameBreathTimer = urand(12*IN_MILLISECONDS, 20*IN_MILLISECONDS);
                    }
					else FlameBreathTimer -= diff;

                    if (TailLashTimer <= diff)
                    {
                        DoCast(SPELL_TAIL_LASH);
                        TailLashTimer = urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS);
                    }
					else TailLashTimer -= diff;

                    if (FieryCombustionTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 45.0f, true))
						{
                            DoCast(target, SPELL_FIERY_COMBUSTION);
							FieryCombustionTimer = urand(25*IN_MILLISECONDS,40*IN_MILLISECONDS);
						}
                    }
					else FieryCombustionTimer -= diff;

                    if (MeteorFlameTimer <= diff)
                    {
						DoSendQuantumText(SAY_HALION_METEOR_FLAME, me);
                        DoCast(SPELL_METEOR);
                        MeteorFlameTimer = urand(30*IN_MILLISECONDS,35*IN_MILLISECONDS);
                    }
					else MeteorFlameTimer -= diff;

                    if (HealthBelowPct(76))
                    {
                        setStage(1);
                        me->AttackStop();
                        me->InterruptNonMeleeSpells(true);
                        SetCombatMovement(false);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    }
                    break;
                case 1: // Switch to phase 2
                    {
                        DoSendQuantumText(SAY_HALION_PHASE_2, me);
                        instance->SetData(TYPE_HALION_EVENT, NOT_STARTED);
                        StartMovement(0);
                        {
                            Creature* pControl = me->GetMap()->GetCreature(instance->GetData64(NPC_HALION_CONTROLLER));
                            if (!pControl)
                                pControl = me->SummonCreature(NPC_HALION_CONTROLLER, SpawnLoc[0].x, SpawnLoc[0].y, SpawnLoc[0].z, 0, TEMPSUMMON_MANUAL_DESPAWN, 1000);
                            else if (!pControl->IsAlive())
                                pControl->Respawn();
                            pControl->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            me->SetInCombatWith(pControl);
                            pControl->SetInCombatWith(me);
                        }
                        setStage(2);
                        break;
                    }
                case 2:
                    if (MovementStarted)
                        return;
                    //DoCast(me, SPELL_SUMMON_TWILIGHT_PORTAL);
                    setStage(3);
					if (GameObject* go = instance->instance->GetGameObject(instance->GetData64(GO_HALION_PORTAL_ENTER_TWILIGHT)))
						go->SetPhaseMask(1, true);
                    if (GameObject* pGoRing = instance->instance->GetGameObject(instance->GetData64(GO_FLAME_RING)))
                          pGoRing->SetPhaseMask(65535, true);
                    break;
                case 3:
                    if (me->IsNonMeleeSpellCasted(false))
                        return;
                    DoCast(SPELL_START_PHASE2);
                    setStage(4);
                    break;
                case 4:
                    if (!me->IsNonMeleeSpellCasted(false))
                    {
                        if (Creature* pControl = me->GetMap()->GetCreature(instance->GetData64(NPC_HALION_CONTROLLER)))
                        {
                            me->SetInCombatWith(pControl);
                            pControl->SetInCombatWith(me);
                        }
                        Creature* pTwilight = me->GetMap()->GetCreature(instance->GetData64(NPC_HALION_TWILIGHT));
                        if (!pTwilight)
                            pTwilight = me->SummonCreature(NPC_HALION_TWILIGHT, SpawnLoc[0].x, SpawnLoc[0].y, SpawnLoc[0].z, 0, TEMPSUMMON_MANUAL_DESPAWN, 1000);
                        else if (!pTwilight->IsAlive())
                            pTwilight->Respawn();
                        pTwilight->SetCreatorGUID(0);
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        setStage(5);
                    }
                    break;
                case 5: // HALION awaiting end battle in TWILIGHT REALM
                    if (instance->GetData(TYPE_HALION_EVENT) == IN_PROGRESS)
					{
						me->RemoveAurasDueToSpell(SPELL_START_PHASE2);
						if (Creature* pControl = me->GetMap()->GetCreature(instance->GetData64(NPC_HALION_CONTROLLER)))
						{
							me->SetInCombatWith(pControl);
							pControl->SetInCombatWith(me);
						}
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE | UNIT_FLAG_NOT_SELECTABLE);
						me->SetHealth(me->GetMaxHealth()/2);
						me->SetInCombatWithZone();
						setStage(6);
					}
                    return;
                case 6: // Switch to phase 3
                    DoSendQuantumText(SAY_HALION_PHASE_3, me);
                    instance->SetData(TYPE_HALION_EVENT, SPECIAL);
                    setStage(7);
                    break;
                case 7:
                    if (me->IsNonMeleeSpellCasted(false))
                        return;
                    if (me->GetVictim()->GetTypeId() != TYPE_ID_PLAYER)
                        return;
                    SetCombatMovement(true);
                    me->GetMotionMaster()->MoveChase(me->GetVictim());
                    setStage(8);
                    break;
                case 8: //PHASE 3 BOTH REALMS
                    if (Creature* pTwilight = me->GetMap()->GetCreature(instance->GetData64(NPC_HALION_TWILIGHT)))
					{
                        if (pTwilight->IsAlive())
                            me->SetHealth(pTwilight->GetHealth());
					}

                    if (FlameBreathTimer <= diff)
                    {
                        DoCast(SPELL_FLAME_BREATH);
                        FlameBreathTimer = urand(12*IN_MILLISECONDS, 20*IN_MILLISECONDS);
                    }
					else FlameBreathTimer -= diff;

                    if (TailLashTimer <= diff)
                    {
                        DoCast(SPELL_TAIL_LASH);
                        TailLashTimer = urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS);
                    }
					else TailLashTimer -= diff;

                    if (FieryCombustionTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 45.0f, true))
						{
                            DoCast(target, SPELL_FIERY_COMBUSTION);
							FieryCombustionTimer = urand(25*IN_MILLISECONDS,40*IN_MILLISECONDS);
						}
                    }
					else FieryCombustionTimer -= diff;

                    if (MeteorFlameTimer <= diff)
                    {
						DoSendQuantumText(SAY_HALION_METEOR_FLAME, me);
                        DoCast(SPELL_METEOR);
                        MeteorFlameTimer = urand(30*IN_MILLISECONDS,35*IN_MILLISECONDS);
                    }
					else MeteorFlameTimer -= diff;
                    break;
                default:
                    break;
            }

            if (EnrageTimer <= diff)
            {
                DoCast(SPELL_BERSERK);
                EnrageTimer = 10*MINUTE*IN_MILLISECONDS;
                DoSendQuantumText(SAY_HALION_BERSERK, me);
            }
			else EnrageTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_halion_realAI(creature);
    }
};

class boss_halion_twilight : public CreatureScript
{
public:
    boss_halion_twilight() : CreatureScript("boss_halion_twilight") { }

    struct boss_halion_twilightAI : public QuantumBasicAI
    {
        boss_halion_twilightAI(Creature* creature) : QuantumBasicAI(creature)
        {
			instance = creature->GetInstanceScript();
            Reset();
        }

        InstanceScript* instance;

        uint8 stage;
        bool intro;
        uint32 EnrageTimer;
        uint32 DuskShroudTimer;
        uint32 DarkBreathTimer;
        uint32 SoulConsumptionTimer;
        uint32 TailLashTimer;
		uint32 RegenerateTimer;

        void Reset()
        {
            if (!instance)
				return;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);

            me->SetRespawnDelay(7*DAY);

            setStage(0);
            intro = false;
            EnrageTimer = 10*MINUTE*IN_MILLISECONDS;
            DuskShroudTimer = 2*IN_MILLISECONDS;
            Tdmg = 0;
            DarkBreathTimer = urand(12*IN_MILLISECONDS, 20*IN_MILLISECONDS);
            SoulConsumptionTimer = urand(30*IN_MILLISECONDS,40*IN_MILLISECONDS);
            TailLashTimer = urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS);
			RegenerateTimer = urand(27*IN_MILLISECONDS, 30*IN_MILLISECONDS);

            me->SetInCombatWithZone();
            if (Creature* pControl = me->GetMap()->GetCreature(instance->GetData64(NPC_HALION_CONTROLLER)))
            {
                me->SetInCombatWith(pControl);
                pControl->SetInCombatWith(me);
            }

            Creature* pFocus = me->GetMap()->GetCreature(instance->GetData64(NPC_ORB_ROTATION_FOCUS));
            if (!pFocus)
                pFocus = me->SummonCreature(NPC_ORB_ROTATION_FOCUS, SpawnLoc[0].x, SpawnLoc[0].y, SpawnLoc[0].z, 0, TEMPSUMMON_MANUAL_DESPAWN, 1000);
            else if (!pFocus->IsAlive())
                pFocus->Respawn();

            Creature* pReal = me->GetMap()->GetCreature(instance->GetData64(NPC_HALION_REAL));
            if (pReal && pReal->IsAlive())
                me->SetHealth(pReal->GetHealth());

            if (!me->HasAura(SPELL_TWILIGHT_ENTER))
                DoCast(me, SPELL_TWILIGHT_ENTER);
        }

		void MoveInLineOfSight(Unit* who)
        {
            if (!instance)
				return;

            if (!who || who->GetTypeId() != TYPE_ID_PLAYER)
                return;

            if (!intro && who->IsWithinDistInMap(me, 20.0f))
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NO_AGGRO_FOR_CREATURE);

                intro = true;
                AttackStart(who);
                setStage(1);
                DoCast(SPELL_TWILIGHT_PRECISION);
            }
        }

		void EnterToBattle(Unit* /*who*/)
        {
            if (!instance)
                return;

			instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me, 2);
        }

		void KilledUnit(Unit* who)
        {
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_HALION_SLAY_1, SAY_HALION_SLAY_2), me);
        }

		void JustDied(Unit* killer)
        {
            if (!instance)
                return;

			// Kill Real Halion after death Twilight Halion
			if (Creature* hreal = ObjectAccessor::GetCreature(*me, instance->GetData64(NPC_HALION_REAL)))
			{
				if (me->IsDamageEnoughForLootingAndReward())
					hreal->LowerPlayerDamageReq(hreal->GetMaxHealth());

				if (hreal->IsAlive())
					killer->Kill(hreal);
			}

			// Despawn before 30 seconds after death
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			me->SetDisplayId(MODEL_ID_INVISIBLE);
			me->DespawnAfterAction(30*IN_MILLISECONDS);

			instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_TWILIGHT_ENTER);
			instance->SetData(TYPE_HALION, DONE);
			instance->SetData(TYPE_COUNTER, COUNTER_OFF);
        }

		void DamageTaken(Unit* /*DoneBy*/, uint32& damage)
        {
            if (!instance)
                return;

            Tdmg += damage;
            Creature* pHalionReal = me->GetMap()->GetCreature(instance->GetData64(NPC_HALION_REAL));
            if (!pHalionReal)
                return;

            if (pHalionReal->GetHealth() <= 1 || !pHalionReal->IsAlive())
                return;

            if (damage < me->GetHealth())
                pHalionReal->SetHealth(me->GetHealth()-damage);
		}

        void setStage(uint8 phase)
		{
			stage = phase;
		}

        uint8 getStage()
		{
			return stage;
		}

        void JustReachedHome()
        {
            if (!instance)
                return;

            if (instance->GetData(TYPE_HALION_EVENT) != FAIL || getStage() == 0)
                return;

			instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);

            QuantumBasicAI::JustReachedHome();
        }

        void EnterEvadeMode()
        {
            if (!instance)
                return;

            if (instance->GetData(TYPE_HALION_EVENT) != FAIL || getStage() == 0)
                return;

            QuantumBasicAI::EnterEvadeMode();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!me->HasAura(SPELL_TWILIGHT_ENTER))
                DoCast(me, SPELL_TWILIGHT_ENTER);

            if (!instance)
            {
				me->DespawnAfterAction();
				return;
            }

            if (!instance || instance->GetData(TYPE_HALION) != IN_PROGRESS || instance->GetData(TYPE_HALION_EVENT) == FAIL)
            {
                if (Creature* pReal = me->GetMap()->GetCreature(instance->GetData64(NPC_HALION_REAL)))
				{
                    if (!pReal->IsAlive())
                        pReal->Respawn();
				}
				me->DespawnAfterAction();
            }

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            switch(getStage())
            {
                case 1: //SPAWNED - Twilight realm
                    if (DuskShroudTimer <= diff)
                    {
                        if (!me->IsNonMeleeSpellCasted(false))
						{
                            DoCast(SPELL_DUSK_SHROUD);
							DuskShroudTimer = 2*IN_MILLISECONDS;
						}
                    }
					else DuskShroudTimer -= diff;

                    if (DarkBreathTimer <= diff)
                    {
                        DoCast(SPELL_DARK_BREATH);
                        DarkBreathTimer = urand(12*IN_MILLISECONDS, 20*IN_MILLISECONDS);
                    }
					else DarkBreathTimer -= diff;

                    if (TailLashTimer <= diff)
                    {
                        DoCast(SPELL_TAIL_LASH);
                        TailLashTimer = urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS);
                    }
					else TailLashTimer -= diff;

                    if (SoulConsumptionTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 45.0f, true))
						{
                            DoCast(target, SPELL_SOUL_CONSUMPTION);
							SoulConsumptionTimer = urand(25*IN_MILLISECONDS,40*IN_MILLISECONDS);
						}
                    }
					else SoulConsumptionTimer -= diff;

					if (RegenerateTimer <= diff)
                    {
						DoSendQuantumText(EMOTE_REGENERATE, me);
						DoCastAOE(SPELL_TWILIGHT_MENDING, true);
						RegenerateTimer = urand(27*IN_MILLISECONDS, 30*IN_MILLISECONDS);
                    }
					else RegenerateTimer -= diff;

                    if (HealthBelowPct(51))
                        setStage(2);
                    break;
                case 2: //To two realms
                    instance->SetData(TYPE_HALION_EVENT, IN_PROGRESS);
                    DoSendQuantumText(SAY_HALION_PHASE_3, me);
					if (GameObject* go = instance->instance->GetGameObject(instance->GetData64(GO_HALION_PORTAL_EXIT_TWILIGT)))
						go->SetPhaseMask(32, true);
                    DoCast(SPELL_TWILIGHT_DIVISION);
                    setStage(3);
                    break;
                case 3: //PHASE 3 BOTH REALMS
                    if (DuskShroudTimer <= diff)
                    {
                        if (!me->IsNonMeleeSpellCasted(false))
						{
                            DoCast(SPELL_DUSK_SHROUD);
							DuskShroudTimer = 2*IN_MILLISECONDS;
						}
                    }
					else DuskShroudTimer -= diff;

                    if (DarkBreathTimer <= diff)
                    {
                        DoCast(SPELL_DARK_BREATH);
                        DarkBreathTimer = urand(12*IN_MILLISECONDS, 20*IN_MILLISECONDS);
                    }
					else DarkBreathTimer -= diff;

                    if (TailLashTimer <= diff)
                    {
                        DoCast(SPELL_TAIL_LASH);
                        TailLashTimer = urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS);
                    }
					else TailLashTimer -= diff;

                    if (SoulConsumptionTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 45.0f, true))
						{
                            DoCast(target, SPELL_SOUL_CONSUMPTION);
							SoulConsumptionTimer = urand(25*IN_MILLISECONDS, 40*IN_MILLISECONDS);
						}
                    }
					else SoulConsumptionTimer -= diff;

					if (RegenerateTimer <= diff)
                    {
						DoSendQuantumText(EMOTE_REGENERATE, me);
						DoCastAOE(SPELL_TWILIGHT_MENDING, true);
						RegenerateTimer = urand(27*IN_MILLISECONDS, 30*IN_MILLISECONDS);
                    }
					else RegenerateTimer -= diff;
                    break;
                default:
                    break;
            }

            if (EnrageTimer <= diff)
            {
                 DoCast(SPELL_BERSERK);
                 EnrageTimer = 10*MINUTE*IN_MILLISECONDS;
                 DoSendQuantumText(SAY_HALION_BERSERK, me);
            }
			else EnrageTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_halion_twilightAI(creature);
    }
};

struct HalionBuffLine
{
    uint32 real, twilight;  // Buff pair
    uint8 disp_corp;        // Displayed Corporeality
};

static HalionBuffLine Buff[] =
{
    {SPELL_CORPOREALITY_100D, SPELL_CORPOREALITY_100I ,0},
    {SPELL_CORPOREALITY_80D, SPELL_CORPOREALITY_80I  ,10},
    {SPELL_CORPOREALITY_60D, SPELL_CORPOREALITY_60I  ,20},
    {SPELL_CORPOREALITY_40D, SPELL_CORPOREALITY_40I  ,30},
    {SPELL_CORPOREALITY_20D, SPELL_CORPOREALITY_20I  ,40},
    {SPELL_CORPOREALITY_EVEN, SPELL_CORPOREALITY_EVEN ,50},
    {SPELL_CORPOREALITY_20I, SPELL_CORPOREALITY_20D  ,60},
    {SPELL_CORPOREALITY_40I, SPELL_CORPOREALITY_40D  ,70},
    {SPELL_CORPOREALITY_60I, SPELL_CORPOREALITY_60D  ,80},
    {SPELL_CORPOREALITY_80I, SPELL_CORPOREALITY_80D  ,90},
    {SPELL_CORPOREALITY_100I, SPELL_CORPOREALITY_100D ,100},
};

class mob_halion_control : public CreatureScript
{
public:
    mob_halion_control() : CreatureScript("mob_halion_control") { }

    struct mob_halion_controlAI : public QuantumBasicAI
    {
        mob_halion_controlAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = (InstanceScript*)creature->GetInstanceScript();
            Reset();
        }

        InstanceScript* instance;
        Creature* pHalionReal;
        Creature* pHalionTwilight;
        uint32 m_lastBuffReal, m_lastBuffTwilight, m_lastBuffNum;
        uint32 CorporealityTimer;
        bool m_detectplayers;
        uint8 m_lastCorpReal;

        void Reset()
        {
            if (!instance)
                return;

            CorporealityTimer = 5*IN_MILLISECONDS;
            m_detectplayers = true;
            me->SetDisplayId(MODEL_ID_INVISIBLE);
            me->SetPhaseMask(65535, true);
            me->SetRespawnDelay(7*DAY);
            SetCombatMovement(false);
            m_lastBuffReal = 0;
            m_lastBuffTwilight = 0;
            m_lastBuffNum = 5;
            m_lastCorpReal = 50;
            instance->SetData(TYPE_COUNTER, COUNTER_OFF);
            instance->SetData(TYPE_HALION_EVENT, NOT_STARTED);
        }

        void AttackStart(Unit* /*who*/) { return; }

        bool doSearchPlayerAtRange(float range)
        {
            Map* map = me->GetMap();
            if (map && map->IsDungeon())
            {
                Map::PlayerList const &PlayerList = map->GetPlayers();
                if (!PlayerList.isEmpty()) {
                    for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                    {
                       if (!i->getSource()->IsInMap(me))
						   continue;

                       if (i->getSource()->IsGameMaster())
						   continue;

                       if (i->getSource()->IsAlive() && i->getSource()->IsWithinDistInMap(me, range))
                           return true;
                    }
                }
            }
            return false;
        }

        void UpdateAI(const uint32 diff)
        {
            if (!instance)
            {
                me->DespawnAfterAction();
                return;
            }

            if (instance->GetData(TYPE_HALION) != IN_PROGRESS)
            {
                me->DespawnAfterAction();
                return;
            }

            if (CorporealityTimer <= diff)
            {
                if (!doSearchPlayerAtRange(100.0f))
                {
                    sLog->outDebug(LOG_FILTER_MAPS, "ruby_sanctum: cannot detect players in range! ");
                    if (!m_detectplayers)
                    {
                        instance->SetData(TYPE_HALION_EVENT, FAIL);
                        instance->SetData(TYPE_HALION, FAIL);
                        me->DespawnAfterAction();
                    }
					else m_detectplayers = false;
                }
				else m_detectplayers = true;

                if (instance->GetData(TYPE_HALION_EVENT) != SPECIAL)
                    return;

                pHalionReal = me->GetMap()->GetCreature(instance->GetData64(NPC_HALION_REAL));
                pHalionTwilight = me->GetMap()->GetCreature(instance->GetData64(NPC_HALION_TWILIGHT));

                if (pHalionTwilight && pHalionTwilight)
                    pHalionTwilight->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                float p_MaxHP = (float)pHalionReal->GetMaxHealth();
                float p_RealHP = (pHalionReal && pHalionReal->IsAlive()) ? Rdmg/p_MaxHP*100 : 0.0f;
                float p_TwilightHP = (pHalionTwilight && pHalionTwilight->IsAlive()) ? Tdmg/p_MaxHP*100 : 0.0f;
                Rdmg = 0;
                Tdmg = 0;
                float m_diff = (p_TwilightHP - p_RealHP);
                int32 buffnum;

                if (m_diff<0)
                    buffnum = m_lastBuffNum - 1;
                else {
                    if (m_diff > 0)
                        buffnum = m_lastBuffNum + 1;
                    else
                        buffnum = m_lastBuffNum;
                }

                if (buffnum < 0)
                    buffnum = 0;
                else if (buffnum > 10)
                    buffnum = 10;

                if (!m_lastBuffReal || m_lastBuffReal != Buff[buffnum].real)
                {
                    if (pHalionReal && pHalionReal->IsAlive())
                    {
                        if (m_lastBuffReal)
                            pHalionReal->RemoveAurasDueToSpell(m_lastBuffReal);

                        pHalionReal->CastSpell(pHalionReal, Buff[buffnum].real, true);
                        m_lastBuffReal = Buff[buffnum].real;
                    }
                }

                if (m_lastCorpReal - Buff[buffnum].disp_corp < 0)
				{
					DoSendQuantumText(EMOTE_REAL_PULL, pHalionReal);
                    DoSendQuantumText(EMOTE_TWIL_PUSH, pHalionTwilight);
                }
				else if (m_lastCorpReal - Buff[buffnum].disp_corp > 0)
				{
					DoSendQuantumText(EMOTE_REAL_PUSH, pHalionReal);
					DoSendQuantumText(EMOTE_TWIL_PULL, pHalionTwilight);
                }

                m_lastBuffReal = Buff[buffnum].real;
                m_lastCorpReal = (uint8)Buff[buffnum].disp_corp;
                m_lastBuffNum = buffnum;
                if (!m_lastBuffTwilight || m_lastBuffTwilight != Buff[buffnum].twilight)
                {
                    if (pHalionTwilight && pHalionTwilight->IsAlive())
                    {
                        if (m_lastBuffTwilight)
                         pHalionTwilight->RemoveAurasDueToSpell(m_lastBuffTwilight);
                            pHalionTwilight->CastSpell(pHalionTwilight, Buff[buffnum].twilight, true);
                        m_lastBuffTwilight = Buff[buffnum].twilight;
                    }
                }

                instance->SetData(TYPE_COUNTER, m_lastCorpReal);
                CorporealityTimer = 10*IN_MILLISECONDS;
            }
			else CorporealityTimer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_halion_controlAI(creature);
    }
};

class mob_orb_rotation_focus : public CreatureScript
{
public:
    mob_orb_rotation_focus() : CreatureScript("mob_orb_rotation_focus") { }

    struct mob_orb_rotation_focusAI : public QuantumBasicAI
    {
        mob_orb_rotation_focusAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = (InstanceScript*)creature->GetInstanceScript();
            Reset();
        }

        InstanceScript* instance;

        uint32 m_timer;

        float m_direction, m_nextdirection;
        bool m_warning;
        bool m_warning2;

        Creature* m_pulsar_N;
        Creature* m_pulsar_S;
        Creature* m_pulsar_E;
        Creature* m_pulsar_W;

        void Reset()
        {
            me->SetRespawnDelay(7*DAY);
            me->SetPhaseMask(65535, true);
            SetCombatMovement(false);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_direction = 0.0f;
            m_nextdirection = 0.0f;
            m_timer = 50000;
            m_warning = false;

            m_pulsar_N = me->GetMap()->GetCreature(instance->GetData64(NPC_SHADOW_PULSAR_N));
            if (!m_pulsar_N)
			{
                float x,y;
                me->GetNearPoint2D(x, y, FR_RADIUS, m_direction);
                m_pulsar_N = me->SummonCreature(NPC_SHADOW_PULSAR_N, x, y, me->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN, 5000);
			}
            
			else if (!m_pulsar_N->IsAlive())
                m_pulsar_N->Respawn();

            m_pulsar_S = me->GetMap()->GetCreature(instance->GetData64(NPC_SHADOW_PULSAR_S));
            if (!m_pulsar_S)
			{
                float x,y;
                me->GetNearPoint2D(x, y, FR_RADIUS, m_direction + M_PI);
                m_pulsar_S = me->SummonCreature(NPC_SHADOW_PULSAR_S, x, y, me->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN, 5000);
            }
			else if (!m_pulsar_S->IsAlive())
                m_pulsar_S->Respawn();
        }

        void AttackStart(Unit* /*who*/)
		{
			return;
		}

        void UpdateAI(const uint32 diff)
        {
            if (!instance)
                me->DespawnAfterAction();

            if (instance->GetData(TYPE_HALION) != IN_PROGRESS)
                me->DespawnAfterAction();

            if (instance->GetData(DATA_ORB_S) == DONE && instance->GetData(DATA_ORB_N) == DONE)
            {
                m_direction = m_nextdirection;
                m_nextdirection = (m_direction - M_PI/64.0f);
                if (m_nextdirection < 0.0f )
                    m_nextdirection = m_nextdirection + 2.0f*M_PI;
                instance->SetData(DATA_ORB_DIRECTION, (uint32)(m_nextdirection*1000));
                instance->SetData(DATA_ORB_N, SPECIAL);
                instance->SetData(DATA_ORB_S, SPECIAL);
            }

            if (m_timer%30000 - 3000 <= diff && !m_warning)
            {
                DoSendQuantumText(EMOTE_WARNING, me);
                m_warning = true;
            }

            if (m_timer%30000 - 1000 < diff && !m_warning2)
            {
                Creature* pHalionTwilight = me->GetMap()->GetCreature(instance->GetData64(NPC_HALION_TWILIGHT));
                if (pHalionTwilight)
                {
                    DoSendQuantumText(SAY_HALION_DARK_SPECIAL, pHalionTwilight);
                    m_warning2 = true;
                }
            }

            if (m_timer > 20000 && m_timer < 30000)
            {
                m_pulsar_N->CastSpell(m_pulsar_S, SPELL_TWILIGHT_CUTTER_CHANNEL, true);

                Map* map = me->GetMap();
                Map::PlayerList const &pList = map->GetPlayers();
                    if (pList.isEmpty()) return;

                for(Map::PlayerList::const_iterator i = pList.begin(); i != pList.end(); ++i)
                {
                    if (Player* player = i->getSource())
                    {
                        if (!player->HasAura(SPELL_TWILIGHT_ENTER))
                            return;

                        if (player->IsAlive())
                        {
                            float AB = m_pulsar_N->GetDistance2d(m_pulsar_S)+1;
                            float BC = m_pulsar_N->GetDistance2d(player)+1;
                            float AC = m_pulsar_S->GetDistance2d(player)+1;
                            float p = (AB + BC + AC)/2;
                            float DC = (2*sqrt(p*(p-AB)*(p-BC)*(p-AC)))/AB;
                            if (DC < 3.75f || DC > 52.0f)
                                player->CastSpell(player, SPELL_TWILIGHT_CUTTER, true, 0, 0, m_pulsar_N->GetGUID());
                        }
                    }
                }
            }

            if (m_timer <= diff)
            {
                float x, y;
                me->GetNearPoint2D(x, y, FR_RADIUS, m_nextdirection);
                me->SummonCreature(NPC_ORB_CARRIER, x, y, me->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN, 5000);
                m_timer = 50000;
                m_warning = false;
            }
			else m_timer -= diff;
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_orb_rotation_focusAI(creature);
    }
};

class mob_halion_orb : public CreatureScript
{
public:
    mob_halion_orb() : CreatureScript("mob_halion_orb") { }

    struct mob_halion_orbAI : public QuantumBasicAI
    {
        mob_halion_orbAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = (InstanceScript*)creature->GetInstanceScript();
            Reset();
        }

        InstanceScript* instance;

        float m_direction,m_delta;
        uint32 m_flag;
        uint32 m_flag1;
        bool MovementStarted;
        uint32 nextPoint;
        uint32 tc_timmer;

        void Reset()
        {
            if (!instance)
                return;

            me->SetRespawnDelay(7*DAY);
            SetCombatMovement(false);
            tc_timmer = 30000;
            me->SetPhaseMask(32, true);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            if (me->GetEntry() == NPC_SHADOW_PULSAR_N)
			{
                m_flag = DATA_ORB_N;
                m_delta = 0.0f;
            }
			else if (me->GetEntry() == NPC_SHADOW_PULSAR_S)
			{
                m_flag = DATA_ORB_S;
                m_delta = M_PI;
            }

            me->SetSpeed(MOVE_WALK, 8.0f);
            me->SetSpeed(MOVE_RUN, 8.0f);
            m_direction = 0.0f;
            nextPoint = 0;
            MovementStarted = false;
            instance->SetData(m_flag, DONE);
            sLog->outDebug(LOG_FILTER_MAPS, "EventMGR: creature %u assume m_flag %u ",me->GetEntry(),m_flag);
        }

        void AttackStart(Unit* /*who*/) { return; }

        void MovementInform(uint32 type, uint32 id)
        {
            if (!instance)
                return;

            if (type != POINT_MOTION_TYPE || !MovementStarted)
                return;

            if (id == nextPoint)
            {
                me->GetMotionMaster()->MovementExpired();
                MovementStarted = false;
                instance->SetData(m_flag, DONE);
            }
        }

        void StartMovement(uint32 id)
        {
            if (!instance)
                return;

            nextPoint = id;
            float x,y;
            instance->SetData(m_flag, IN_PROGRESS);
            MovementStarted = true;
            m_direction = ((float)instance->GetData(DATA_ORB_DIRECTION)/1000 + m_delta);
            if (m_direction > 2.0f*M_PI)
                m_direction = m_direction - 2.0f*M_PI;

            if (Creature* pFocus = me->GetMap()->GetCreature(instance->GetData64(NPC_ORB_ROTATION_FOCUS)))
                pFocus->GetNearPoint2D(x, y, FR_RADIUS, m_direction);
            else
                me->DespawnAfterAction();
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MovePoint(id, x, y,  me->GetPositionZ());
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!instance)
                me->DespawnAfterAction();

            if (instance->GetData(TYPE_HALION) != IN_PROGRESS)
                me->DespawnAfterAction();

            if (!MovementStarted && instance->GetData(m_flag) == SPECIAL)
                StartMovement(1);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_halion_orbAI(creature);
    }
};

class mob_orb_carrier : public CreatureScript
{
public:
    mob_orb_carrier() : CreatureScript("mob_orb_carrier") { }

    struct mob_orb_carrierAI : public QuantumBasicAI
    {
        mob_orb_carrierAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = (InstanceScript*)creature->GetInstanceScript();
            Reset();
        }

        InstanceScript* instance;

        bool MovementStarted;

        void Reset()
        {
            me->SetRespawnDelay(7*DAY);
            SetCombatMovement(false);
            me->SetPhaseMask(32, true);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            MovementStarted = false;
            me->RemoveUnitMovementFlag(MOVEMENTFLAG_WALKING); //or remove???
            me->SetSpeed(MOVE_RUN, 10.0f, true);
        }

        void AttackStart(Unit* /*who*/) { return; }

        void MovementInform(uint32 type, uint32 id)
        {
            if (!instance) return;

            if (type != POINT_MOTION_TYPE || !MovementStarted)
                return;

            if (id == 1)
            {
                me->GetMotionMaster()->MovementExpired();
                MovementStarted = false;
                me->DespawnAfterAction();
            }
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!instance || instance->GetData(TYPE_HALION) != IN_PROGRESS)
                  me->DespawnAfterAction();

            if (!MovementStarted)
            {
                float x, y;
                float m_direction = ((float)instance->GetData(DATA_ORB_DIRECTION)/1000.0f + M_PI - M_PI/32.0f);
                if (m_direction > 2.0f*M_PI)
                    m_direction = m_direction - 2.0f*M_PI;
                if (Creature* pFocus = me->GetMap()->GetCreature(instance->GetData64(NPC_ORB_ROTATION_FOCUS)))
                    pFocus->GetNearPoint2D(x, y, FR_RADIUS, m_direction);
                else me->DespawnAfterAction();
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MovePoint(1, x, y,  me->GetPositionZ());
                MovementStarted = true;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_orb_carrierAI(creature);
    }
};

class mob_soul_consumption : public CreatureScript
{
public:
    mob_soul_consumption() : CreatureScript("mob_soul_consumption") { }

    struct mob_soul_consumptionAI : public QuantumBasicAI
    {
        mob_soul_consumptionAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = (InstanceScript*)creature->GetInstanceScript();
            Reset();
        }

        InstanceScript* instance;

        float m_Size0;
        float m_Size;
        uint32 m_uiConsumptTimer;
        bool grow;

        void Reset()
        {
            me->SetPhaseMask(32,true);
            SetCombatMovement(false);
            m_uiConsumptTimer = 60*IN_MILLISECONDS;
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            DoCast(SPELL_CONSUMPTION_AURA);
            m_Size0 = me->GetFloatValue(OBJECT_FIELD_SCALE_X);
            m_Size = m_Size0;
            grow = false;
        }

        void AttackStart(Unit* /*who*/) { return; }

        void UpdateAI(const uint32 diff)
        {
            if (instance && instance->GetData(TYPE_HALION) != IN_PROGRESS)
                me->DespawnAfterAction();

            if (m_uiConsumptTimer <= diff)
                me->DespawnAfterAction();
            else m_uiConsumptTimer -= diff;

            Map::PlayerList const &players = instance->instance->GetPlayers();
            for(Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
            {
                if (Player* player = i->getSource())
                {
                    if (player->IsAlive() && player->HasAura(SPELL_MARK_OF_CONSUMPTION))
                    {
                        Aura *pAura = player->GetAura(SPELL_MARK_OF_CONSUMPTION);
                        uint32 stacs;
                        stacs= pAura->GetStackAmount();
                        if (!grow)
                        {
                            m_Size = (m_Size + (stacs*0.1f));
                            me->SetFloatValue(OBJECT_FIELD_SCALE_X, m_Size);
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_OF_CONSUMPTION);
                            grow = true;
                        }
                    }
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_soul_consumptionAI(creature);
    }
};

class mob_fiery_combustion : public CreatureScript
{
public:
    mob_fiery_combustion() : CreatureScript("mob_fiery_combustion") { }

    struct mob_fiery_combustionAI : public QuantumBasicAI
    {
        mob_fiery_combustionAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = (InstanceScript*)creature->GetInstanceScript();
            Reset();
        }

        InstanceScript* instance;

        float m_Size0;
        float m_Size;
        uint32 ConbustTimer;
        bool grow;

        void Reset()
        {
            me->SetPhaseMask(1,true);
            ConbustTimer = 60*IN_MILLISECONDS;
            SetCombatMovement(false);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            DoCast(SPELL_COMBUSTION_AURA);
            m_Size0 = me->GetFloatValue(OBJECT_FIELD_SCALE_X);
            m_Size = m_Size0;
            grow = false;
        }

        void AttackStart(Unit* /*who*/) { return; }

        void UpdateAI(const uint32 diff)
        {
            if (!instance)
                return;

            if (instance && instance->GetData(TYPE_HALION) != IN_PROGRESS)
                me->DespawnAfterAction();

            if (ConbustTimer <= diff)
			{
                me->DespawnAfterAction();
			}
            else ConbustTimer -= diff;

            Map::PlayerList const &players = instance->instance->GetPlayers();
            for(Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
            {
                if (Player* player = i->getSource())
                {
                    if (player->IsAlive() && player->HasAura(SPELL_MARK_OF_COMBUSTION))
                    {
                        Aura *pAura = player->GetAura(SPELL_MARK_OF_COMBUSTION);
                        uint32 stacs;
                        stacs= pAura->GetStackAmount();
                        if (!grow)
                        {
                            m_Size = (m_Size + (stacs*0.1f));
                            me->SetFloatValue(OBJECT_FIELD_SCALE_X, m_Size);
                            me->RemoveAura(SPELL_MARK_OF_COMBUSTION, player->GetGUID());
                            instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_MARK_OF_COMBUSTION);
                            grow = true;
                        }
                    }
                }
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_fiery_combustionAI(creature);
    }
};

#define TARGETS_10 5
#define TARGETS_25 7

class mob_halion_meteor : public CreatureScript
{
public:
    mob_halion_meteor() : CreatureScript("mob_halion_meteor") { }

    struct mob_halion_meteorAI : public QuantumBasicAI
    {
        mob_halion_meteorAI(Creature* creature) : QuantumBasicAI(creature)
        {
            Reset();
        }

        float x, y, radius, direction;
        uint8 stage;

        uint32 MeteorImpactTimer;
        uint32 MeteorStrikeTimer;

        void setStage(uint8 phase)
		{
			stage = phase;
		}

        uint8 getStage()
		{
			return stage;
		}

        void Reset()
        {
            me->SetDisplayId(MODEL_ID_INVISIBLE);
            me->SetRespawnDelay(7*DAY);
            SetCombatMovement(false);
            MeteorImpactTimer = 500;
            MeteorStrikeTimer = 4500;
            setStage(0);
            me->SetInCombatWithZone();
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }

        void UpdateAI(const uint32 diff)
        {
            switch(getStage())
            {
                case 0:
                    if (MeteorImpactTimer <= diff)
                    {
                        DoCast(SPELL_METEOR_IMPACT);
                        MeteorImpactTimer = 500;
                        setStage(1);
                    }
					else MeteorImpactTimer -= diff;
                    break;
                case 1:
                    if (MeteorStrikeTimer <= diff)
                    {
                        DoCast(SPELL_METEOR_STRIKE);
                        MeteorStrikeTimer = 4500;
                        setStage(2);
                    }
					else MeteorStrikeTimer -= diff;
                    break;
                case 2:
                    {
                        direction = 2.0f*M_PI*((float)urand(0,15)/16.0f);
                        radius = 0.0f;
                        for(uint8 i = 0; i < RAID_MODE(TARGETS_10,TARGETS_25,TARGETS_10,TARGETS_25); ++i)
                        {
                            radius = radius + 5.0f;
                            me->GetNearPoint2D(x, y, radius, direction);
                            me->SummonCreature(NPC_METEOR_STRIKE_1, x, y, me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 20000);
                            me->GetNearPoint2D(x, y, radius, direction+M_PI);
                            me->SummonCreature(NPC_METEOR_STRIKE_1, x, y, me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 20000);
                        }
                    };
                    {
                        direction = direction + M_PI/4;
                        radius = 0.0f;
                        for(uint8 i = 0; i < RAID_MODE(TARGETS_10,TARGETS_25,TARGETS_10,TARGETS_25); ++i)
                        {
                            radius = radius + 5.0f;
                            me->GetNearPoint2D(x, y, radius, direction);
                            me->SummonCreature(NPC_METEOR_STRIKE_1, x, y, me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 20000);
                            me->GetNearPoint2D(x, y, radius, direction+M_PI);
                            me->SummonCreature(NPC_METEOR_STRIKE_1, x, y, me->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 20000);
                        }
                     };
                     setStage(3);
                     break;
                case 3:
                    if (MeteorImpactTimer <= diff)
                    {
                        DoCast(SPELL_METEOR_IMPACT);
                        me->DespawnAfterAction();
                        MeteorImpactTimer = 500;
                    } else MeteorImpactTimer -= diff;
                    break;
                default:
                     break;
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_halion_meteorAI(creature);
    }
};

class mob_halion_flame : public CreatureScript
{
public:
    mob_halion_flame() : CreatureScript("mob_halion_flame") { }

    struct mob_halion_flameAI : public QuantumBasicAI
    {
        mob_halion_flameAI(Creature* creature) : QuantumBasicAI(creature)
        {
            Reset();
        }

        void Reset()
        {
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->SetDisplayId(MODEL_ID_INVISIBLE);
            me->SetRespawnDelay(7*DAY);
            SetCombatMovement(false);
            me->SetInCombatWithZone();
        }

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!me->HasAura(SPELL_METEOR_FLAME))
                DoCast(SPELL_METEOR_FLAME);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_halion_flameAI(creature);
    }
};

class go_halion_portal_twilight : public GameObjectScript
{
public:
    go_halion_portal_twilight() : GameObjectScript("go_halion_portal_twilight") { }

    bool OnGossipHello(Player* player, GameObject* /*pGo*/)
    {
        player->CastSpell(player, SPELL_TWILIGHT_ENTER, false);
        return true;
    }
};

class go_halion_portal_real : public GameObjectScript
{
public:
    go_halion_portal_real() : GameObjectScript("go_halion_portal_real") { }

    bool OnGossipHello(Player* player, GameObject* /*pGo*/)
    {
        player->RemoveAurasDueToSpell(SPELL_TWILIGHT_ENTER);
		return true;
    }
};

class spell_halion_fiery_combustion : public SpellScriptLoader
{
public:
    spell_halion_fiery_combustion() : SpellScriptLoader("spell_halion_fiery_combustion") { }

    class spell_halion_fiery_combustion_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_halion_fiery_combustion_AuraScript);

        bool Validate(SpellEntry const* /*spellEntry*/)
        {
            if (!sSpellStore.LookupEntry(SPELL_MARK_OF_COMBUSTION))
                return false;
            if (!sSpellStore.LookupEntry(SPELL_COMBUSTION_EXPLODE))
                return false;
            return true;
        }

        void HandlePeriodicTick(AuraEffect const* /*aurEff*/)
        {
            if (Unit* target = GetTarget())
                target->CastSpell(target, SPELL_MARK_OF_COMBUSTION, true);
        }

        void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Unit* target = GetTarget())
            {
                if (Aura *mark = target->GetAura(SPELL_MARK_OF_COMBUSTION))
                {
                    int32 bp = 2000 * mark->GetStackAmount();
                    target->CastCustomSpell(target, SPELL_COMBUSTION_EXPLODE, &bp, 0, 0, true);
                }
            }
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_halion_fiery_combustion_AuraScript::HandlePeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            OnEffectRemove += AuraEffectRemoveFn(spell_halion_fiery_combustion_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_halion_fiery_combustion_AuraScript();
    }
};

class spell_halion_soul_consumption : public SpellScriptLoader
{
public:
    spell_halion_soul_consumption() : SpellScriptLoader("spell_halion_soul_consumption") { }

    class spell_halion_soul_consumption_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_halion_soul_consumption_AuraScript);

        bool Validate(SpellEntry const* /*spellEntry*/)
        {
            if (!sSpellStore.LookupEntry(SPELL_MARK_OF_CONSUMPTION))
                return false;
            if (!sSpellStore.LookupEntry(SPELL_CONSUMPTION_EXPLODE))
                return false;
            return true;
        }

        void HandlePeriodicTick(AuraEffect const* /*aurEff*/)
        {
            if (Unit* target = GetTarget())
                target->CastSpell(target, SPELL_MARK_OF_CONSUMPTION, true);
        }

        void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (Unit* target = GetTarget())
            {
                if (Aura *mark = target->GetAura(SPELL_MARK_OF_CONSUMPTION))
                {
                    int32 bp = 2000 * mark->GetStackAmount();
                    target->CastCustomSpell(target, SPELL_CONSUMPTION_EXPLODE, &bp, 0, 0, true);
                }
            }
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_halion_soul_consumption_AuraScript::HandlePeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE);
            OnEffectRemove += AuraEffectRemoveFn(spell_halion_soul_consumption_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_halion_soul_consumption_AuraScript();
    }
};

class spell_halion_twilight_realm_handlers : public SpellScriptLoader
{
    public:
        spell_halion_twilight_realm_handlers(const char* scriptName, uint32 beforeHitSpell, bool isApplyHandler) : SpellScriptLoader(scriptName),
            _beforeHitSpell(beforeHitSpell), _isApplyHandler(isApplyHandler)
        { }

        class spell_halion_twilight_realm_handlers_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_halion_twilight_realm_handlers_AuraScript);

        public:
            spell_halion_twilight_realm_handlers_AuraScript(uint32 beforeHitSpell, bool isApplyHandler) : AuraScript(),
                _isApply(isApplyHandler), _beforeHitSpellId(beforeHitSpell)
            { }

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(_beforeHitSpellId))
                    return false;
                return true;
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*handle*/)
            {
                GetTarget()->RemoveAurasDueToSpell(SPELL_TWILIGHT_ENTER);
                if (InstanceScript* instance = GetTarget()->GetInstanceScript())
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_UNK7);
            }

            void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*handle*/)
            {
                Unit* target = GetTarget();
                if (!target)
                    return;

				target->RemoveAurasDueToSpell(_beforeHitSpellId, 0, 0, AURA_REMOVE_BY_ENEMY_SPELL);
                if (InstanceScript* instance = target->GetInstanceScript())
                    instance->SendEncounterUnit(ENCOUNTER_FRAME_UNK7);
            }

            void Register()
            {
                if (!_isApply)
                {
                    AfterEffectApply += AuraEffectApplyFn(spell_halion_twilight_realm_handlers_AuraScript::OnApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                    AfterEffectRemove += AuraEffectRemoveFn(spell_halion_twilight_realm_handlers_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                }
                else
                    AfterEffectApply += AuraEffectApplyFn(spell_halion_twilight_realm_handlers_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PHASE, AURA_EFFECT_HANDLE_REAL);
            }

            bool _isApply;
            uint32 _beforeHitSpellId;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_halion_twilight_realm_handlers_AuraScript(_beforeHitSpell, _isApplyHandler);
        }

    private:
        uint32 _beforeHitSpell;
        bool _isApplyHandler;
};

void AddSC_boss_halion()
{
    new boss_halion_real;
    new boss_halion_twilight;
    new mob_halion_meteor;
    new mob_halion_flame;
    new mob_halion_orb;
    new mob_halion_control;
    //new mob_orb_rotation_focus;
    new mob_orb_carrier;
    new mob_soul_consumption;
    new mob_fiery_combustion;
    new go_halion_portal_twilight;
    new go_halion_portal_real;
    new spell_halion_fiery_combustion;
    new spell_halion_soul_consumption;
	new spell_halion_twilight_realm_handlers("spell_halion_leave_twilight_realm", SPELL_SOUL_CONSUMPTION, false);
    new spell_halion_twilight_realm_handlers("spell_halion_enter_twilight_realm", SPELL_FIERY_COMBUSTION, true);
}