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
#include "WorldPacket.h"
#include "the_eye.h"

enum Texts
{
    SAY_INTRO                           = -1550016,
    SAY_INTRO_CAPERNIAN                 = -1550017,
    SAY_INTRO_TELONICUS                 = -1550018,
    SAY_INTRO_THALADRED                 = -1550019,
    SAY_INTRO_SANGUINAR                 = -1550020,
    SAY_PHASE2_WEAPON                   = -1550021,
    SAY_PHASE3_ADVANCE                  = -1550022,
    SAY_PHASE4_INTRO2                   = -1550023,
    SAY_PHASE5_NUTS                     = -1550024,
    SAY_SLAY1                           = -1550025,
    SAY_SLAY2                           = -1550026,
    SAY_SLAY3                           = -1550027,
    SAY_MIND_CONTROL_1                  = -1550028,
    SAY_MIND_CONTROL_2                  = -1550029,
    SAY_GRAVITY_LAPSE_1                 = -1550030,
    SAY_GRAVITY_LAPSE_2                 = -1550031,
    SAY_SUMMON_PHOENIX1                 = -1550032,
    SAY_SUMMON_PHOENIX2                 = -1550033,
    SAY_DEATH                           = -1550034,
    SAY_THALADRED_AGGRO                 = -1550035,
    SAY_THALADRED_DEATH                 = -1550036,
    EMOTE_THALADRED_GAZE                = -1550037,
    SAY_SANGUINAR_AGGRO                 = -1550038,
    SAY_SANGUINAR_DEATH                 = -1550039,
    SAY_CAPERNIAN_AGGRO                 = -1550040,
    SAY_CAPERNIAN_DEATH                 = -1550041,
    SAY_TELONICUS_AGGRO                 = -1550042,
    SAY_TELONICUS_DEATH                 = -1550043,
};

enum Spells
{
    SPELL_SUMMON_WEAPONS                = 36976,
    SPELL_SUMMON_WEAPONA                = 36958,
    SPELL_SUMMON_WEAPONB                = 36959,
    SPELL_SUMMON_WEAPONC                = 36960,
    SPELL_SUMMON_WEAPOND                = 36961,
    SPELL_SUMMON_WEAPONE                = 36962,
    SPELL_SUMMON_WEAPONF                = 36963,
    SPELL_SUMMON_WEAPONG                = 36964,
    SPELL_RES_VISUAL                    = 24171,
    SPELL_FIREBALL                      = 22088,           //wrong but works with CastCustomSpell
    SPELL_PYROBLAST                     = 36819,
    SPELL_FLAME_STRIKE                  = 36735,
    SPELL_FLAME_STRIKE_VIS              = 36730,
    SPELL_FLAME_STRIKE_DMG              = 36731,
    SPELL_ARCANE_DISRUPTION             = 36834,
    SPELL_SHOCK_BARRIER                 = 36815,
    SPELL_PHOENIX_ANIMATION             = 36723,
    SPELL_MIND_CONTROL                  = 32830,
    SPELL_EXPLODE                       = 36092,
    SPELL_FULLPOWER                     = 36187,
    SPELL_KNOCKBACK                     = 11027,
    SPELL_GRAVITY_LAPSE                 = 34480,
    SPELL_GRAVITY_LAPSE_AURA            = 39432,
    SPELL_NETHER_BEAM                   = 35873,
    SPELL_PSYCHIC_BLOW                  = 10689,
    SPELL_SILENCE                       = 30225,
    SPELL_BELLOWING_ROAR                = 40636,
    SPELL_CAPERNIAN_FIREBALL            = 36971,
    SPELL_CONFLAGRATION                 = 37018,
    SPELL_ARCANE_EXPLOSION              = 36970,
    SPELL_BOMB                          = 37036,
    SPELL_REMOTE_TOY                    = 37027,
    SPELL_NETHER_VAPOR                  = 35858,
    SPELL_BURN                          = 36720,
    SPELL_EMBER_BLAST                   = 34341,
    SPELL_REBIRTH                       = 41587,
};

enum Modedls
{
    MODEL_ID_PHOENIX                    = 19682,
    MODEL_ID_PHOENIX_EGG                = 20245,
    MAX_ADVISORS                        = 4,
};

uint32 SpellSummonWeapon[] =
{
    SPELL_SUMMON_WEAPONA, SPELL_SUMMON_WEAPONB, SPELL_SUMMON_WEAPONC, SPELL_SUMMON_WEAPOND,
    SPELL_SUMMON_WEAPONE, SPELL_SUMMON_WEAPONF, SPELL_SUMMON_WEAPONG,
};

const float CAPERNIAN_DISTANCE = 20.0f;
const float KAEL_VISIBLE_RANGE = 50.0f;

const Position GravityPosition[] =
{
	{794.478f, -0.268021f, 77.7506f, 3.15421f},
};

enum TimePhases
{
	TIME_PHASE_2_3 = 120000,
	TIME_PHASE_3_4 = 180000,
};

struct advisorbase_ai : public QuantumBasicAI
{
    advisorbase_ai(Creature* creature) : QuantumBasicAI(creature)
    {
        instance = creature->GetInstanceScript();
        DoubledHealth = false;
    }

    InstanceScript* instance;

    bool FakeDeath;
    bool DoubledHealth;

    uint32 DelayResTimer;
    uint64 DelayResTarget;

    void Reset()
    {
        if (DoubledHealth)
        {
            me->SetMaxHealth(me->GetMaxHealth() / 2);
            DoubledHealth = false;
        }

        FakeDeath = false;
        DelayResTimer = 0;
        DelayResTarget = 0;

		DoCast(me, SPELL_UNIT_DETECTION_ALL);
		me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

        me->SetStandState(UNIT_STAND_STATE_STAND);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        if (instance->GetData(DATA_KAELTHAS_SUNSTRIDER) == 1 || instance->GetData(DATA_KAELTHAS_SUNSTRIDER) == 3)
		{
            if (Creature* Kaelthas = Unit::GetCreature(*me, instance->GetData64(DATA_KAELTHAS_SUNSTRIDER)))
                Kaelthas->AI()->EnterEvadeMode();
		}
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (!who || FakeDeath || me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        QuantumBasicAI::MoveInLineOfSight(who);
    }

    void AttackStart(Unit* who)
    {
        if (!who || FakeDeath || me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        QuantumBasicAI::AttackStart(who);
    }

    void Revive(Unit* /*target*/)
    {
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        me->SetMaxHealth(me->GetMaxHealth() * 2);
        DoubledHealth = true;
        me->SetFullHealth();
        me->SetStandState(UNIT_STAND_STATE_STAND);

        DoCast(me, SPELL_RES_VISUAL, true);
        DelayResTimer = 2000;
    }

    void DamageTaken(Unit* killer, uint32 &damage)
    {
        if (damage < me->GetHealth())
            return;

        if (FakeDeath && instance->GetData(DATA_KAELTHAS_SUNSTRIDER) != 0)
        {
            damage = 0;
            return;
        }

        if (instance->GetData(DATA_KAELTHAS_SUNSTRIDER) != 0)
        {
            damage = 0;
            FakeDeath = true;

            me->InterruptNonMeleeSpells(false);
            me->SetHealth(0);
            me->StopMoving();
            me->ClearComboPointHolders();
            me->RemoveAllAurasOnDeath();
            me->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
            me->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            me->ClearAllReactives();
            me->SetTarget(0);
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MoveIdle();
            me->SetStandState(UNIT_STAND_STATE_DEAD);
            JustDied(killer);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (DelayResTimer)
        {
            if (DelayResTimer <= diff)
            {
                DelayResTimer = 0;
                FakeDeath = false;

                Unit* Target = Unit::GetUnit(*me, DelayResTarget);
                if (!Target)
                    Target = me->GetVictim();

                DoResetThreat();
                AttackStart(Target);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MoveChase(Target);
                me->AddThreat(Target, 0.0f);
            }
			else DelayResTimer -= diff;
        }
    }
};

class boss_kaelthas : public CreatureScript
{
    public:
        boss_kaelthas() : CreatureScript("boss_kaelthas") { }

        struct boss_kaelthasAI : public QuantumBasicAI
        {
            boss_kaelthasAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
            {
                instance = creature->GetInstanceScript();
                memset(&AdvisorGuid, 0, sizeof(AdvisorGuid));
				Reset();
            }

            InstanceScript* instance;

            uint32 FireballTimer;
            uint32 ArcaneDisruptionTimer;
            uint32 PhoenixTimer;
            uint32 ShockBarrierTimer;
            uint32 GravityLapseTimer;
            uint32 GravityLapsePhase;
            uint32 NetherBeamTimer;
            uint32 NetherVaporTimer;
            uint32 FlameStrikeTimer;
            uint32 MindControlTimer;
            uint32 Phase;
            uint32 PhaseSubphase;                                   //generic
            uint32 PhaseTimer;                                     //generic timer
            uint32 PyrosCasted;

            bool InGravityLapse;
            bool IsCastingFireball;
            bool ChainPyros;

            SummonList Summons;

            uint64 AdvisorGuid[MAX_ADVISORS];

            void Reset()
            {
                FireballTimer = 5000+rand()%10000;
                ArcaneDisruptionTimer = 45000;
                MindControlTimer = 40000;
                PhoenixTimer = 50000;
                ShockBarrierTimer = 60000;
                FlameStrikeTimer = 30000;
                GravityLapseTimer = 20000;
                GravityLapsePhase = 0;
                NetherBeamTimer = 8000;
                NetherVaporTimer = 10000;
                PyrosCasted = 0;
                Phase = 0;

                InGravityLapse = false;
                IsCastingFireball = false;
                ChainPyros = false;

                if (!me->IsInCombatActive())
                    PrepareAdvisors();

                Summons.DespawnAll();

				DoCast(me, SPELL_UNIT_DETECTION_ALL);

                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

				me->SetSpeed(MOVE_WALK, 1.5f);
				me->SetSpeed(MOVE_RUN, 1.5f);
				me->RemoveUnitMovementFlag(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING);
				me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
				me->SetCanFly(false);
				me->SetDisableGravity(false);

				instance->SetData(DATA_KAELTHAS_SUNSTRIDER, NOT_STARTED);

				instance->HandleGameObject(instance->GetData64(DATA_KAETLTHAS_LEFT_DOOR), true);
				instance->HandleGameObject(instance->GetData64(DATA_KAETLTHAS_RIGHT_DOOR), true);
				instance->HandleGameObject(instance->GetData64(DATA_KAETLTHAS_ENTRANCE_DOOR_RIGHT), true);
				instance->HandleGameObject(instance->GetData64(DATA_KAETLTHAS_ENTRANCE_DOOR_LEFT), true);
				instance->HandleGameObject(instance->GetData64(DATA_KAELTHAS_PRE_ENTRANCE_DOOR_RIGHT), true);
				instance->HandleGameObject(instance->GetData64(DATA_KAELTHAS_PRE_ENTRANCE_DOOR_LEFT), true);
            }

			void MoveInLineOfSight(Unit* who)
            {
                if (!me->HasUnitState(UNIT_STATE_STUNNED) && me->CanCreatureAttack(who))
                {
                    if (!me->CanFly() && me->GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE)
                        return;

                    float attackRadius = me->GetAttackDistance(who);
                    if (me->IsWithinDistInMap(who, attackRadius) && me->IsWithinLOSInMap(who))
                    {
                        if (!me->GetVictim() && Phase >= 4)
                        {
                            who->RemoveAurasByType(SPELL_AURA_MOD_STEALTH);
                            AttackStart(who);
                        }
                        else if (me->GetMap()->IsDungeon())
                        {
                            if (!instance->GetData(DATA_KAELTHAS_SUNSTRIDER) && !Phase)
                                StartEvent();

                            who->SetInCombatWith(me);
                            me->AddThreat(who, 0.0f);
                        }
                    }
                }
            }

            void EnterToBattle(Unit* /*who*/)
            {
                if (!instance->GetData(DATA_KAELTHAS_SUNSTRIDER) && !Phase)
                    StartEvent();
            }

			void KilledUnit(Unit* /*victim*/)
            {
                DoSendQuantumText(RAND(SAY_SLAY1, SAY_SLAY2, SAY_SLAY3), me);
            }

			void JustReachedHome()
			{
				instance->SetData(DATA_KAELTHAS_SUNSTRIDER, FAIL);
				Reset();
			}

			void JustDied(Unit* /*killer*/)
            {
                DoSendQuantumText(SAY_DEATH, me);

				me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
				me->RemoveUnitMovementFlag(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING);
				me->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
				me->SetCanFly(false);
				me->SetDisableGravity(false);
				me->GetMotionMaster()->MoveFall();

                Summons.DespawnAll();

				instance->SetData(DATA_KAELTHAS_SUNSTRIDER, DONE);

                for (uint8 i = 0; i < MAX_ADVISORS; ++i)
                {
                    if (Unit* pAdvisor = Unit::GetUnit(*me, AdvisorGuid[i]))
                        pAdvisor->Kill(pAdvisor);
                }

				instance->HandleGameObject(instance->GetData64(DATA_KAETLTHAS_LEFT_DOOR), true);
				instance->HandleGameObject(instance->GetData64(DATA_KAETLTHAS_RIGHT_DOOR), true);
				instance->HandleGameObject(instance->GetData64(DATA_KAETLTHAS_ENTRANCE_DOOR_RIGHT), true);
				instance->HandleGameObject(instance->GetData64(DATA_KAETLTHAS_ENTRANCE_DOOR_LEFT), true);
				instance->HandleGameObject(instance->GetData64(DATA_KAELTHAS_PRE_ENTRANCE_DOOR_RIGHT), true);
				instance->HandleGameObject(instance->GetData64(DATA_KAELTHAS_PRE_ENTRANCE_DOOR_LEFT), true);
            }

            void PrepareAdvisors()
            {
                for (uint8 i = 0; i < MAX_ADVISORS; ++i)
                {
                    if (Creature* creature = Unit::GetCreature(*me, AdvisorGuid[i]))
                    {
                        creature->Respawn();
						creature->SetStandState(UNIT_STAND_STATE_STAND);
						creature->GetMotionMaster()->MoveTargetedHome();
                        creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        creature->SetCurrentFaction(me->GetFaction());
                        creature->AI()->EnterEvadeMode();
                    }
                }
            }

            void StartEvent()
            {
                AdvisorGuid[0] = instance->GetData64(DATA_THALADRED_THE_DARKENER);
                AdvisorGuid[1] = instance->GetData64(DATA_LORD_SANGUINAR);
                AdvisorGuid[2] = instance->GetData64(DATA_GRAND_ASTROMANCER_CAPERNIAN);
                AdvisorGuid[3] = instance->GetData64(DATA_MASTER_ENGINEER_TELONICUS);

                if (!AdvisorGuid[0] || !AdvisorGuid[1] || !AdvisorGuid[2] || !AdvisorGuid[3])
                {
                    sLog->OutErrorConsole("QUANTUMCORE SCRIPTS: Kael'Thas One or more advisors missing, Skipping Phases 1-3");

                    DoSendQuantumText(SAY_PHASE4_INTRO2, me);

                    Phase = 4;

                    instance->SetData(DATA_KAELTHAS_SUNSTRIDER, SPECIAL);

                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                        AttackStart(target);
                }
                else
                {
                    PrepareAdvisors();

                    DoSendQuantumText(SAY_INTRO, me);

                    instance->SetData(DATA_KAELTHAS_SUNSTRIDER, IN_PROGRESS);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

					instance->HandleGameObject(instance->GetData64(DATA_KAETLTHAS_LEFT_DOOR), false);
					instance->HandleGameObject(instance->GetData64(DATA_KAETLTHAS_RIGHT_DOOR), false);
					instance->HandleGameObject(instance->GetData64(DATA_KAETLTHAS_ENTRANCE_DOOR_RIGHT), false);
					instance->HandleGameObject(instance->GetData64(DATA_KAETLTHAS_ENTRANCE_DOOR_LEFT), false);
					instance->HandleGameObject(instance->GetData64(DATA_KAELTHAS_PRE_ENTRANCE_DOOR_RIGHT), false);
					instance->HandleGameObject(instance->GetData64(DATA_KAELTHAS_PRE_ENTRANCE_DOOR_LEFT), false);

                    PhaseSubphase = 0;
                    PhaseTimer = 23000;
                    Phase = 1;
                }
            }

			void SetGravityLapse()
			{
				std::list<HostileReference*>& m_threatlist = me->getThreatManager().getThreatList();
				for (std::list<HostileReference*>::const_iterator i = m_threatlist.begin(); i != m_threatlist.end(); ++i)
				{
					Unit* unit = Unit::GetUnit(*me, (*i)->getUnitGuid());
					if (unit->GetTypeId() == TYPE_ID_PLAYER)
					{
						DoCast(unit, SPELL_KNOCKBACK, true);
						unit->CastSpell(unit, SPELL_GRAVITY_LAPSE, true, 0, 0, me->GetGUID());
						unit->CastSpell(unit, SPELL_GRAVITY_LAPSE_AURA, true, 0, 0, me->GetGUID());

						WorldPacket data(SMSG_MOVE_SET_CAN_FLY, 12);
						data.append(unit->GetPackGUID());
						data << uint32(0);
						unit->SendMessageToSet(&data, true);
					}
				}
			}

			void RemoveGravityLapse()
			{
				std::list<HostileReference*>& m_threatlist = me->getThreatManager().getThreatList();
				for (std::list<HostileReference*>::const_iterator i = m_threatlist.begin(); i != m_threatlist.end(); ++i)
				{
					Unit* unit = Unit::GetUnit(*me, (*i)->getUnitGuid());
					if (unit->GetTypeId() == TYPE_ID_PLAYER)
					{
						unit->RemoveAurasDueToSpell(SPELL_GRAVITY_LAPSE);
						unit->RemoveAurasDueToSpell(SPELL_GRAVITY_LAPSE_AURA);

						WorldPacket data(SMSG_MOVE_UNSET_CAN_FLY, 12);
						data.append(unit->GetPackGUID());
						data << uint32(0);
						unit->SendMessageToSet(&data, true);
					}
				}
			}

			void JustSummoned(Creature* summon)
			{
				switch (summon->GetEntry())
				{
                    case NPC_PHOENIX:
					case NPC_NETHERSTRAND_LONGBOW:
					case NPC_DEVASTATION:
					case NPC_COSMIC_INFUSER:
					case NPC_INFINITY_BLADES:
					case NPC_WARP_SLICER:
					case NPC_PHASESHIFT_BULWARK:
					case NPC_STAFF_OF_DISINTEGRATION:
						Summons.Summon(summon);
						Summons.DoZoneInCombat();
						break;
					default:
						break;
				}
			}

            void SummonedCreatureDespawn(Creature* summon)
			{
				Summons.Despawn(summon);
			}

            void UpdateAI(const uint32 diff)
            {
                //Phase 1
                switch (Phase)
                {
                    case 1:
                    {
                        Unit* target = NULL;
                        Creature* Advisor = NULL;

                        //Subphase switch
                        switch (PhaseSubphase)
                        {
                            //Subphase 1 - Start
                            case 0:
                                if (PhaseTimer <= diff)
                                {
                                    DoSendQuantumText(SAY_INTRO_THALADRED, me);

                                    //start advisor within 7 seconds
                                    PhaseTimer = 7000;
                                    ++PhaseSubphase;
                                }
								else PhaseTimer -= diff;
                                break;

                            //Subphase 1 - Unlock advisor
                            case 1:
                                if (PhaseTimer <= diff)
                                {
                                    Advisor = (Unit::GetCreature(*me, AdvisorGuid[0]));

                                    if (Advisor)
                                    {
                                        Advisor->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                        Advisor->SetCurrentFaction(me->GetFaction());

                                        target = SelectTarget(TARGET_RANDOM, 0);
                                        if (target)
                                            Advisor->AI()->AttackStart(target);
                                    }

                                    ++PhaseSubphase;
                                }
								else PhaseTimer -= diff;
                                break;
                            //Subphase 2 - Start
                            case 2:
                                Advisor = (Unit::GetCreature(*me, AdvisorGuid[0]));

                                if (Advisor && (Advisor->getStandState() == UNIT_STAND_STATE_DEAD))
                                {
                                    DoSendQuantumText(SAY_INTRO_SANGUINAR, me);

                                    //start advisor within 12.5 seconds
                                    PhaseTimer = 12500;
                                    ++PhaseSubphase;
                                }
                                break;

                            //Subphase 2 - Unlock advisor
                            case 3:
                                if (PhaseTimer <= diff)
                                {
                                    Advisor = (Unit::GetCreature(*me, AdvisorGuid[1]));

                                    if (Advisor)
                                    {
                                        Advisor->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                        Advisor->SetCurrentFaction(me->GetFaction());

                                        target = SelectTarget(TARGET_RANDOM, 0);
                                        if (target)
                                            Advisor->AI()->AttackStart(target);
                                    }

                                    ++PhaseSubphase;
                                }
								else PhaseTimer -= diff;
                                break;
                            //Subphase 3 - Start
                            case 4:
                                Advisor = (Unit::GetCreature(*me, AdvisorGuid[1]));

                                if (Advisor && (Advisor->getStandState() == UNIT_STAND_STATE_DEAD))
                                {
                                    DoSendQuantumText(SAY_INTRO_CAPERNIAN, me);

                                    //start advisor within 7 seconds
                                    PhaseTimer = 7000;
                                    ++PhaseSubphase;
                                }
                                break;
                            //Subphase 3 - Unlock advisor
                            case 5:
                                if (PhaseTimer <= diff)
                                {
                                    Advisor = (Unit::GetCreature(*me, AdvisorGuid[2]));

                                    if (Advisor)
                                    {
                                        Advisor->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                        Advisor->SetCurrentFaction(me->GetFaction());

                                        target = SelectTarget(TARGET_RANDOM, 0);
                                        if (target)
                                            Advisor->AI()->AttackStart(target);
                                    }

                                    ++PhaseSubphase;
                                }
								else PhaseTimer -= diff;
                                break;
                            //Subphase 4 - Start
                            case 6:
                                Advisor = (Unit::GetCreature(*me, AdvisorGuid[2]));

                                if (Advisor && (Advisor->getStandState() == UNIT_STAND_STATE_DEAD))
                                {
                                    DoSendQuantumText(SAY_INTRO_TELONICUS, me);

                                    //start advisor within 8.4 seconds
                                    PhaseTimer = 8400;
                                    ++PhaseSubphase;
                                }
                                break;
                            //Subphase 4 - Unlock advisor
                            case 7:
                                if (PhaseTimer <= diff)
                                {
                                    Advisor = (Unit::GetCreature(*me, AdvisorGuid[3]));

                                    if (Advisor)
                                    {
                                        Advisor->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                        Advisor->SetCurrentFaction(me->GetFaction());

                                        target = SelectTarget(TARGET_RANDOM, 0);
                                        if (target)
                                            Advisor->AI()->AttackStart(target);
                                    }

                                    PhaseTimer = 3000;
                                    ++PhaseSubphase;
                                }
								else PhaseTimer -= diff;
                                break;
                            //End of phase 1
                            case 8:
                                Advisor = (Unit::GetCreature(*me, AdvisorGuid[3]));

                                if (Advisor && (Advisor->getStandState() == UNIT_STAND_STATE_DEAD))
                                {
                                    Phase = 2;

									instance->SetData(DATA_KAELTHAS_SUNSTRIDER, FAIL);

                                    DoSendQuantumText(SAY_PHASE2_WEAPON, me);
                                    PhaseSubphase = 0;
                                    PhaseTimer = 3500;
                                    DoCast(me, SPELL_SUMMON_WEAPONS);
                                }
                                break;
                          }
                    }
                    break;

                    case 2:
                    {
                        if (PhaseSubphase == 0)
                        {
                            if (PhaseTimer <= diff)
                            {
                                PhaseSubphase = 1;
                            }
                            else PhaseTimer -= diff;
                        }

                        //Spawn weapons
                        if (PhaseSubphase == 1)
                        {
                            DoCast(me, SPELL_SUMMON_WEAPONS, false);
                            uint8 uiMaxWeapon = sizeof(SpellSummonWeapon)/sizeof(uint32);

                            for (uint32 i = 0; i < uiMaxWeapon; ++i)
                                DoCast(me, SpellSummonWeapon[i], true);

                            PhaseSubphase = 2;
                            PhaseTimer = TIME_PHASE_2_3;
                        }

                        if (PhaseSubphase == 2)
                        {
                            if (PhaseTimer <= diff)
                            {
                                DoSendQuantumText(SAY_PHASE3_ADVANCE, me);

								instance->SetData(DATA_KAELTHAS_SUNSTRIDER, DONE);

                                Phase = 3;
                                PhaseSubphase = 0;
                            }
                            else PhaseTimer -= diff;
                        }
                    }
                    break;

                    case 3:
                    {
                        if (PhaseSubphase == 0)
                        {
                            //Respawn advisors
                            Unit* target = SelectTarget(TARGET_RANDOM, 0);

                            Creature* Advisor;
                            for (uint8 i = 0; i < MAX_ADVISORS; ++i)
                            {
                                Advisor = Unit::GetCreature(*me, AdvisorGuid[i]);

                                if (!Advisor)
                                    sLog->OutErrorConsole("Kael'Thas Advisor %u does not exist. Possibly despawned? Incorrectly Killed?", i);
                                else
                                    CAST_AI(advisorbase_ai, Advisor->AI())->Revive(target);
                            }

                            PhaseSubphase = 1;
                            PhaseTimer = TIME_PHASE_3_4;
                        }

                        if (PhaseTimer <= diff)
                        {
                            DoSendQuantumText(SAY_PHASE4_INTRO2, me);
                            Phase = 4;

							instance->SetData(DATA_KAELTHAS_SUNSTRIDER, SPECIAL);

                            // Sometimes people can collect Aggro in Phase 1-3. Reset threat before releasing Kael.
                            DoResetThreat();

                            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                                AttackStart(target);

                            PhaseTimer = 30000;
                        }
                        else
                            PhaseTimer -= diff;
                    }
                    break;
                    case 4:
                    case 5:
                    case 6:
                    {
                        //Return since we have no target
                        if (!UpdateVictim())
                            return;

                        //FireballTimer
                        if (!InGravityLapse && !ChainPyros && Phase != 5)
                        {
                            if (FireballTimer <= diff)
                            {
                                if (!IsCastingFireball)
                                {
                                    if (!me->IsNonMeleeSpellCasted(false))
                                    {
                                        //interruptable
                                        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, false);
                                        int32 dmg = 20000+rand()%5000;
                                        me->CastCustomSpell(me->GetVictim(), SPELL_FIREBALL, &dmg, 0, 0, false);
                                        IsCastingFireball = true;
                                        FireballTimer = 2500;
                                    }
                                }
                                else
                                {
                                    //apply resistance
                                    me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_INTERRUPT_CAST, true);
                                    IsCastingFireball = false;
                                    FireballTimer = 5000+rand()%10000;
                                }
                            }
                            else FireballTimer -= diff;

                            //ArcaneDisruptionTimer
                            if (ArcaneDisruptionTimer <= diff)
                            {
                                DoCastVictim(SPELL_ARCANE_DISRUPTION, true);
                                ArcaneDisruptionTimer = 60000;
                            }
                            else ArcaneDisruptionTimer -= diff;

                            if (FlameStrikeTimer <= diff)
                            {
                                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
								{
                                    DoCast(target, SPELL_FLAME_STRIKE);
									FlameStrikeTimer = 30000;
								}
                            }
                            else FlameStrikeTimer -= diff;

                            if (MindControlTimer <= diff)
                            {
								if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
								{
									DoCast(target, SPELL_MIND_CONTROL);
									MindControlTimer = 60000;
								}
                            }
                            else MindControlTimer -= diff;
                        }

                        //PhoenixTimer
                        if (PhoenixTimer <= diff)
                        {
                            DoCast(me, SPELL_PHOENIX_ANIMATION);
                            DoSendQuantumText(RAND(SAY_SUMMON_PHOENIX1, SAY_SUMMON_PHOENIX2), me);

                            PhoenixTimer = 60000;
                        }
                        else PhoenixTimer -= diff;

                        //Phase 4 specific spells
                        if (Phase == 4)
                        {
                            if (HealthBelowPct(50))
                            {
								instance->SetData(DATA_KAELTHAS_SUNSTRIDER, SPECIAL);

                                Phase = 5;
                                PhaseTimer = 10000;

                                DoSendQuantumText(SAY_PHASE5_NUTS, me);

                                me->StopMoving();
                                me->GetMotionMaster()->Clear();
                                me->GetMotionMaster()->MoveIdle();
								me->GetMotionMaster()->MovePoint(0, GravityPosition[0]);
								me->SetSpeed(MOVE_FLIGHT, 3.0f);
								me->AddUnitMovementFlag(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING);
								me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
								me->SetCanFly(true);
								me->SetDisableGravity(true);

                                me->InterruptNonMeleeSpells(false);
                                DoCast(me, SPELL_FULLPOWER);
                                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            }

                            //ShockBarrierTimer
                            if (ShockBarrierTimer <= diff)
                            {
                                DoCast(me, SPELL_SHOCK_BARRIER);
                                ChainPyros = true;
                                PyrosCasted = 0;
                                ShockBarrierTimer = 60000;
                            }
                            else ShockBarrierTimer -= diff;

                            //Chain Pyros (3 of them max)
                            if (ChainPyros && !me->IsNonMeleeSpellCasted(false))
                            {
                                if (PyrosCasted < 3)
                                {
                                    DoCastVictim(SPELL_PYROBLAST);
                                    ++PyrosCasted;
                                }
                                else
                                {
                                    ChainPyros = false;
                                    FireballTimer = 2500;
                                    ArcaneDisruptionTimer = 60000;
                                }
                            }
                        }

                        if (Phase == 5)
                        {
                            if (PhaseTimer <= diff)
                            {
                                me->InterruptNonMeleeSpells(false);
                                me->RemoveAurasDueToSpell(SPELL_FULLPOWER);

                                DoCast(me, SPELL_EXPLODE);
                                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                Phase = 6;
                                AttackStart(me->GetVictim());
                            }
                            else PhaseTimer -= diff;
                        }

                        //Phase 5
                        if (Phase == 6)
                        {

                            //GravityLapseTimer
                            if (GravityLapseTimer <= diff)
                            {
                                std::list<HostileReference*>::const_iterator i = me->getThreatManager().getThreatList().begin();
                                switch (GravityLapsePhase)
                                {
                                    case 0:
                                        me->StopMoving();
                                        me->GetMotionMaster()->Clear();
                                        me->GetMotionMaster()->MoveIdle();
										me->GetMotionMaster()->MovePoint(0, GravityPosition[0]);
										me->SetSpeed(MOVE_FLIGHT, 3.0f);
										me->AddUnitMovementFlag(MOVEMENTFLAG_CAN_FLY | MOVEMENTFLAG_FLYING);
										me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
										me->SetCanFly(true);
										me->SetDisableGravity(true);

                                        // 1) Kael'thas will portal the whole raid right into his body
                                        for (i = me->getThreatManager().getThreatList().begin(); i!= me->getThreatManager().getThreatList().end(); ++i)
                                        {
                                            Unit* unit = Unit::GetUnit(*me, (*i)->getUnitGuid());

                                            if (unit && (unit->GetTypeId() == TYPE_ID_PLAYER))
												DoTeleportPlayer(unit, 794.478f, -0.268021f, 77.7506f, 3.15421f);
                                        }

                                        GravityLapseTimer = 500;
                                        ++GravityLapsePhase;
                                        InGravityLapse = true;
                                        ShockBarrierTimer = 1000;
                                        NetherBeamTimer = 5000;
                                        break;
                                    case 1:
										// Cast Flight
                                        DoSendQuantumText(RAND(SAY_GRAVITY_LAPSE_1, SAY_GRAVITY_LAPSE_2), me);
										SetGravityLapse();
                                        GravityLapseTimer = 10000;
                                        ++GravityLapsePhase;
                                        break;
                                    case 2:
                                        //Cast nether vapor aura on self
                                        me->InterruptNonMeleeSpells(false);
                                        DoCast(me, SPELL_NETHER_VAPOR, true);
                                        GravityLapseTimer = 20000;
                                        ++GravityLapsePhase;
                                        break;
                                    case 3:
										// Remove Flight
										RemoveGravityLapse();
                                        me->RemoveAurasDueToSpell(SPELL_NETHER_VAPOR);
                                        InGravityLapse = false;
                                        GravityLapseTimer = 60000;
                                        GravityLapsePhase = 0;
                                        AttackStart(me->GetVictim());
                                        break;
                                }
                            }
                            else GravityLapseTimer -= diff;

                            if (InGravityLapse)
                            {
                                if (ShockBarrierTimer <= diff)
                                {
                                    DoCast(me, SPELL_SHOCK_BARRIER);
                                    ShockBarrierTimer = 20000;
                                }
                                else ShockBarrierTimer -= diff;

                                if (NetherBeamTimer <= diff)
                                {
                                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
									{
                                        DoCast(target, SPELL_NETHER_BEAM);
										NetherBeamTimer = 4000;
									}
                                }
                                else NetherBeamTimer -= diff;
                            }
                        }

                        if (!InGravityLapse)
                            DoMeleeAttackIfReady();
                    }
                }
            }
        };

        CreatureAI* GetAI(Creature* Creature) const
        {
            return new boss_kaelthasAI(Creature);
        }
};

class boss_thaladred_the_darkener : public CreatureScript
{
    public:
        boss_thaladred_the_darkener() : CreatureScript("boss_thaladred_the_darkener") { }

        struct boss_thaladred_the_darkenerAI : public advisorbase_ai
        {
            boss_thaladred_the_darkenerAI(Creature* creature) : advisorbase_ai(creature) { }

            uint32 GazeTimer;
            uint32 SilenceTimer;
            uint32 PsychicBlowTimer;

            void Reset()
            {
                GazeTimer = 100;
                SilenceTimer = 20000;
                PsychicBlowTimer = 10000;

                advisorbase_ai::Reset();
            }

            void EnterToBattle(Unit* who)
            {
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    return;

                if (!who || FakeDeath)
                    return;

                DoSendQuantumText(SAY_THALADRED_AGGRO, me);
                me->AddThreat(who, 5000000.0f);
            }

            void JustDied(Unit* /*killer*/)
            {
                if (instance->GetData(DATA_KAELTHAS_SUNSTRIDER) == 3)
                    DoSendQuantumText(SAY_THALADRED_DEATH, me);
            }

            void UpdateAI(const uint32 diff)
            {
                advisorbase_ai::UpdateAI(diff);

                //Faking death, don't do anything
                if (FakeDeath)
                    return;

                //Return since we have no target
                if (!UpdateVictim())
                    return;

                //GazeTimer
                if (GazeTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                    {
                        DoResetThreat();
                        me->AddThreat(target, 5000000.0f);
                        DoSendQuantumText(EMOTE_THALADRED_GAZE, me, target);
                        GazeTimer = 8500;
                    }
                }
                else
                    GazeTimer -= diff;

                //SilenceTimer
                if (SilenceTimer <= diff)
                {
                    DoCastVictim(SPELL_SILENCE);
                    SilenceTimer = 20000;
                }
                else
                    SilenceTimer -= diff;

                //PsychicBlowTimer
                if (PsychicBlowTimer <= diff)
                {
                    DoCastVictim(SPELL_PSYCHIC_BLOW);
                    PsychicBlowTimer = 20000+rand()%5000;
                }
                else
                    PsychicBlowTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* Creature) const
        {
            return new boss_thaladred_the_darkenerAI(Creature);
        }
};

//Lord Sanguinar AI
class boss_lord_sanguinar : public CreatureScript
{
    public:
        boss_lord_sanguinar() : CreatureScript("boss_lord_sanguinar") { }

        struct boss_lord_sanguinarAI : public advisorbase_ai
        {
            boss_lord_sanguinarAI(Creature* creature) : advisorbase_ai(creature) { }

            uint32 FearTimer;

            void Reset()
            {
                FearTimer = 20000;
                advisorbase_ai::Reset();
            }

            void EnterToBattle(Unit* who)
            {
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    return;

                if (!who || FakeDeath)
                    return;

                DoSendQuantumText(SAY_SANGUINAR_AGGRO, me);
            }

            void JustDied(Unit* /*Killer*/)
            {
                if (instance->GetData(DATA_KAELTHAS_SUNSTRIDER) == 3)
                    DoSendQuantumText(SAY_SANGUINAR_DEATH, me);
            }

            void UpdateAI(const uint32 diff)
            {
                advisorbase_ai::UpdateAI(diff);

                //Faking death, don't do anything
                if (FakeDeath)
                    return;

                //Return since we have no target
                if (!UpdateVictim())
                    return;

                //FearTimer
                if (FearTimer <= diff)
                {
                    DoCastVictim(SPELL_BELLOWING_ROAR);
                    FearTimer = 25000+rand()%10000;                //approximately every 30 seconds
                }
                else
                    FearTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* Creature) const
        {
            return new boss_lord_sanguinarAI(Creature);
        }
};

class boss_grand_astromancer_capernian : public CreatureScript
{
    public:
        boss_grand_astromancer_capernian() : CreatureScript("boss_grand_astromancer_capernian") { }

        struct boss_grand_astromancer_capernianAI : public advisorbase_ai
        {
            boss_grand_astromancer_capernianAI(Creature* creature) : advisorbase_ai(creature) { }

            uint32 FireballTimer;
            uint32 ConflagrationTimer;
            uint32 ArcaneExplosionTimer;
            uint32 YellTimer;

            bool Yell;

            void Reset()
            {
                FireballTimer = 2000;
                ConflagrationTimer = 20000;
                ArcaneExplosionTimer = 5000;
                YellTimer = 2000;

                Yell = false;

                advisorbase_ai::Reset();
            }

            void JustDied(Unit* /*killer*/)
            {
                if (instance->GetData(DATA_KAELTHAS_SUNSTRIDER) == 3)
                    DoSendQuantumText(SAY_CAPERNIAN_DEATH, me);
            }

            void AttackStart(Unit* who)
            {
                if (!who || FakeDeath || me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    return;

                if (me->Attack(who, true))
                {
                    me->AddThreat(who, 0.0f);
                    me->SetInCombatWith(who);
                    who->SetInCombatWith(me);

                    me->GetMotionMaster()->MoveChase(who, CAPERNIAN_DISTANCE);
                }
            }

            void EnterToBattle(Unit* who)
            {
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    return;

                if (!who || FakeDeath)
                    return;
            }

            void UpdateAI(const uint32 diff)
            {
                advisorbase_ai::UpdateAI(diff);

                //Faking Death, don't do anything
                if (FakeDeath)
                    return;

                //Return since we have no target
                if (!UpdateVictim())
                    return;

                //YellTimer
                if (!Yell)
                {
                    if (YellTimer <= diff)
                    {
                        DoSendQuantumText(SAY_CAPERNIAN_AGGRO, me);
                        Yell = true;
                    }
                    else
                        YellTimer -= diff;
                }

                //FireballTimer
                if (FireballTimer <= diff)
                {
                    DoCastVictim(SPELL_CAPERNIAN_FIREBALL);
                    FireballTimer = 4000;
                }
                else
                    FireballTimer -= diff;

                //ConflagrationTimer
                if (ConflagrationTimer <= diff)
                {
                    Unit* target = NULL;
                    target = SelectTarget(TARGET_RANDOM, 0);

                    if (target && me->IsWithinDistInMap(target, 30))
                        DoCast(target, SPELL_CONFLAGRATION);
                    else
                        DoCastVictim(SPELL_CONFLAGRATION);

                    ConflagrationTimer = 10000+rand()%5000;
                }
                else
                    ConflagrationTimer -= diff;

                //ArcaneExplosionTimer
                if (ArcaneExplosionTimer <= diff)
                {
                    bool InMeleeRange = false;

                    Unit* target = NULL;
                    std::list<HostileReference*>& m_threatlist = me->getThreatManager().getThreatList();
                    for (std::list<HostileReference*>::const_iterator i = m_threatlist.begin(); i!= m_threatlist.end(); ++i)
                    {
                        Unit* unit = Unit::GetUnit(*me, (*i)->getUnitGuid());
                                                                    //if in melee range
                        if (unit && unit->IsWithinDistInMap(me, 5))
                        {
                            InMeleeRange = true;
                            target = unit;
                            break;
                        }
                    }

                    if (InMeleeRange)
                        DoCast(target, SPELL_ARCANE_EXPLOSION);

                    ArcaneExplosionTimer = 4000+rand()%2000;
                }
                else ArcaneExplosionTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* Creature) const
        {
            return new boss_grand_astromancer_capernianAI(Creature);
        }
};

class boss_master_engineer_telonicus : public CreatureScript
{
    public:
        boss_master_engineer_telonicus() : CreatureScript("boss_master_engineer_telonicus") { }

        struct boss_master_engineer_telonicusAI : public advisorbase_ai
        {
            boss_master_engineer_telonicusAI(Creature* creature) : advisorbase_ai(creature) { }

            uint32 BombTimer;
            uint32 RemoteToyTimer;

            void Reset()
            {
                BombTimer = 10000;
                RemoteToyTimer = 5000;

                advisorbase_ai::Reset();
            }

            void JustDied(Unit* /*killer*/)
            {
                if (instance->GetData(DATA_KAELTHAS_SUNSTRIDER) == 3)
                    DoSendQuantumText(SAY_TELONICUS_DEATH, me);
            }

            void EnterToBattle(Unit* who)
            {
                if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                    return;

                if (!who || FakeDeath)
                    return;

                DoSendQuantumText(SAY_TELONICUS_AGGRO, me);
            }

            void UpdateAI(const uint32 diff)
            {
                advisorbase_ai::UpdateAI(diff);

                //Faking Death, do nothing
                if (FakeDeath)
                    return;

                if (!UpdateVictim())
                    return;

                if (BombTimer <= diff)
                {
                    DoCastVictim(SPELL_BOMB);
                    BombTimer = 25000;
                }
                else BombTimer -= diff;

                if (RemoteToyTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
                        DoCast(target, SPELL_REMOTE_TOY);
						RemoteToyTimer = 10000+rand()%5000;
					}
                }
                else RemoteToyTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* Creature) const
        {
            return new boss_master_engineer_telonicusAI(Creature);
        }
};

class npc_kael_flamestrike : public CreatureScript
{
    public:
        npc_kael_flamestrike() : CreatureScript("npc_kael_flamestrike") {}

        struct npc_kael_flamestrikeAI : public QuantumBasicAI
        {
            npc_kael_flamestrikeAI(Creature* creature) : QuantumBasicAI(creature)
			{
				SetCombatMovement(false);
			}

            uint32 Timer;

            bool Casting;
            bool KillSelf;

            void Reset()
            {
                Timer = 5000;
                Casting = false;
                KillSelf = false;

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                me->SetCurrentFaction(16);
            }

            void MoveInLineOfSight(Unit* /*who*/) {}

            void EnterToBattle(Unit* /*who*/) {}

            void UpdateAI(const uint32 diff)
            {
                if (!Casting)
                {
                    DoCast(me, SPELL_FLAME_STRIKE_VIS);
                    Casting = true;
                }

                //Timer
                if (Timer <= diff)
                {
                    if (!KillSelf)
                    {
                        me->InterruptNonMeleeSpells(false);
                        DoCast(me, SPELL_FLAME_STRIKE_DMG);
                    }
                    else me->Kill(me);

                    KillSelf = true;
                    Timer = 1000;
                }
                else Timer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* Creature) const
        {
            return new npc_kael_flamestrikeAI(Creature);
        }
};

class npc_phoenix_tk : public CreatureScript
{
    public:
        npc_phoenix_tk() : CreatureScript("npc_phoenix_tk") {}

        struct npc_phoenix_tkAI : public QuantumBasicAI
        {
            npc_phoenix_tkAI(Creature* creature) : QuantumBasicAI(creature){}

            uint32 CycleTimer;

            void Reset()
            {
                CycleTimer = 2000;
                DoCast(me, SPELL_BURN, true);

				DoCast(me, SPELL_UNIT_DETECTION_ALL);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

            void JustDied(Unit* /*killer*/)
            {
                //DoCast(me, SPELL_EMBER_BLAST, true);
                me->SummonCreature(NPC_PHOENIX_EGG, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 16000);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (CycleTimer <= diff)
                {
                    uint32 dmg = urand(4500, 5500);
                    if (me->GetHealth() > dmg)
                        me->ModifyHealth(-int32(dmg));
                    CycleTimer = 2000;
                }
                else CycleTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* Creature) const
        {
            return new npc_phoenix_tkAI(Creature);
        }
};

class npc_phoenix_egg_tk : public CreatureScript
{
    public:
        npc_phoenix_egg_tk() : CreatureScript("npc_phoenix_egg_tk") {}

        struct npc_phoenix_egg_tkAI : public QuantumBasicAI
        {
            npc_phoenix_egg_tkAI(Creature* creature) : QuantumBasicAI(creature){}

            uint32 RebirthTimer;

            void Reset()
            {
                RebirthTimer = 15000;

				DoCast(me, SPELL_UNIT_DETECTION_ALL);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

            void MoveInLineOfSight(Unit* /*who*/){}

            void AttackStart(Unit* who)
            {
                if (me->Attack(who, false))
                {
                    me->SetInCombatWith(who);
                    who->SetInCombatWith(me);
                    DoStartNoMovement(who);
                }
            }

            void JustSummoned(Creature* summoned)
            {
                summoned->AddThreat(me->GetVictim(), 0.0f);
                summoned->CastSpell(summoned, SPELL_REBIRTH, false);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!RebirthTimer)
                    return;

                if (RebirthTimer <= diff)
                {
                    me->SummonCreature(NPC_PHOENIX, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_CORPSE_DESPAWN, 5000);
                    RebirthTimer = 0;
                }
                else RebirthTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_phoenix_egg_tkAI(creature);
        }
};

void AddSC_boss_kaelthas()
{
    new boss_kaelthas();
    new boss_thaladred_the_darkener();
    new boss_lord_sanguinar();
    new boss_grand_astromancer_capernian();
    new boss_master_engineer_telonicus();
    new npc_kael_flamestrike();
    new npc_phoenix_tk();
    new npc_phoenix_egg_tk();
}