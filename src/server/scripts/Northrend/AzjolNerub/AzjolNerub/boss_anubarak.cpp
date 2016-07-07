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
#include "azjol_nerub.h"

enum Spells
{
    SPELL_CARRION_BEETLES                         = 53520,
    SPELL_SUMMON_CARRION_BEETLES                  = 53521,
    SPELL_LEECHING_SWARM_5N                       = 53467,
	SPELL_LEECHING_SWARM_5H                       = 59430,
    SPELL_POUND_5N                                = 53472,
    SPELL_POUND_5H                                = 59433,
    SPELL_SUBMERGE                                = 53421,
    SPELL_IMPALE_DMG                              = 53454,
    SPELL_IMPALE_DMG_H                            = 59446,
    SPELL_IMPALE_SHAKEGROUND                      = 53455,
    SPELL_IMPALE_SPIKE                            = 53539,   //this is not the correct visual effect
    //SPELL_IMPALE_TARGET                           = 53458,
};

enum Yells
{
    SAY_AGGRO          = -1601000,
    SAY_SLAY_1         = -1601001,
    SAY_SLAY_2         = -1601002,
    SAY_SLAY_3         = -1601003,
	SAY_DEATH          = -1601004,
    SAY_LOCUST_1       = -1601005,
    SAY_LOCUST_2       = -1601006,
    SAY_LOCUST_3       = -1601007,
    SAY_SUBMERGE_1     = -1601008,
    SAY_SUBMERGE_2     = -1601009,
	SAY_INTRO          = -1601010,
};

enum Phases
{
    PHASE_MELEE         = 0,
    PHASE_UNDERGROUND   = 1,
    IMPALE_PHASE_TARGET = 0,
    IMPALE_PHASE_ATTACK = 1,
    IMPALE_PHASE_DMG    = 2,
};

const Position SpawnPoint[2] =
{
    {550.7f, 282.8f, 224.3f, 0.0f},
    {551.1f, 229.4f, 224.3f, 0.0f},
};

const Position SpawnPointGuardian[2] =
{
    {550.348633f, 316.006805f, 234.2947f, 0.0f},
    {550.188660f, 324.264557f, 237.7412f, 0.0f},
};

class boss_anub_arak : public CreatureScript
{
public:
    boss_anub_arak() : CreatureScript("boss_anub_arak") { }

    struct boss_anub_arakAI : public QuantumBasicAI
    {
        boss_anub_arakAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;
		SummonList Summons;

        bool Channeling;
        bool GuardianSummoned;
        bool VenomancerSummoned;
        bool DatterSummoned;
		bool AssasinSummoned;
		bool Intro;

        uint8 Phase;

        uint32 UndergroundPhase;
        uint32 CarrionBeetlesTimer;
        uint32 LeechingSwarmTimer;
        uint32 PoundTimer;
        uint32 SubmergeTimer;
        uint32 UndergroundTimer;
        uint32 VenomancerTimer;
        uint32 DatterTimer;
		uint32 AssasinTimer;
        uint32 DelayTimer;

        uint32 ImpaleTimer;
        uint32 ImpalePhase;

        uint64 ImpaleTarget;

        void Reset()
        {
            CarrionBeetlesTimer = 8*IN_MILLISECONDS;
            LeechingSwarmTimer = 20*IN_MILLISECONDS;
            ImpaleTimer = 9*IN_MILLISECONDS;
            PoundTimer = 15*IN_MILLISECONDS;

			Intro = true;
            Phase = PHASE_MELEE;
            UndergroundPhase = 0;
            Channeling = false;
            ImpalePhase = IMPALE_PHASE_TARGET;

            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            me->RemoveAura(SPELL_SUBMERGE);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            Summons.DespawnAll();

			instance->SetData(DATA_ANUBARAK_EVENT, NOT_STARTED);
			instance->DoStopTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_TIMED_START_EVENT);
        }

        Creature* DoSummonImpaleTarget(Unit* target)
        {
            Position targetPos;
            target->GetPosition(&targetPos);

            if (TempSummon* impaleTarget = me->SummonCreature(NPC_IMPALE_TARGET, targetPos, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 6*IN_MILLISECONDS))
            {
                ImpaleTarget = impaleTarget->GetGUID();
                impaleTarget->SetReactState(REACT_PASSIVE);
                impaleTarget->SetDisplayId(MODEL_ID_INVISIBLE);
                impaleTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                return impaleTarget;
            }

            return NULL;
        }

		void MoveInLineOfSight(Unit* who)
        {
			if (me->IsWithinDistInMap(who, 45.0f))
			{
				if (Intro)
				{
					DoSendQuantumText(SAY_INTRO, me);
					Intro = false;
				}
			}
		}

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

            DelayTimer = 0;

			instance->DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEV_TIMED_START_EVENT);
        }

        void DelayEventStart()
        {
			instance->SetData(DATA_ANUBARAK_EVENT, IN_PROGRESS);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (DelayTimer && DelayTimer > 5000)
			{
				DelayEventStart();
			}
            else DelayTimer += diff;

            switch (Phase)
            {
            case PHASE_UNDERGROUND:
                if (ImpaleTimer <= diff)
                {
                    switch (ImpalePhase)
                    {
                    case IMPALE_PHASE_TARGET:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
                        {
                            if (Creature* impaleTarget = DoSummonImpaleTarget(target))
							{
                                impaleTarget->CastSpell(impaleTarget, SPELL_IMPALE_SHAKEGROUND, true);
							}
                            ImpaleTimer = 3*IN_MILLISECONDS;
                            ImpalePhase = IMPALE_PHASE_ATTACK;
                        }
                        break;
                    case IMPALE_PHASE_ATTACK:
                        if (Creature* impaleTarget = Unit::GetCreature(*me, ImpaleTarget))
                        {
                            impaleTarget->CastSpell(impaleTarget, SPELL_IMPALE_SPIKE, false);
                            impaleTarget->RemoveAurasDueToSpell(SPELL_IMPALE_SHAKEGROUND);
                        }
                        ImpalePhase = IMPALE_PHASE_DMG;
                        ImpaleTimer = 1*IN_MILLISECONDS;
                        break;
                    case IMPALE_PHASE_DMG:
                        if (Creature* impaleTarget = Unit::GetCreature(*me, ImpaleTarget))
                            me->CastSpell(impaleTarget, DUNGEON_MODE(SPELL_IMPALE_DMG, SPELL_IMPALE_DMG_H), true);
                        ImpalePhase = IMPALE_PHASE_TARGET;
                        ImpaleTimer = 9*IN_MILLISECONDS;
                        break;
                    }
                }
				else ImpaleTimer -= diff;

                if (!GuardianSummoned)
                {
                    for (uint8 i = 0; i < 2; ++i)
                    {
                        if (Creature* guardian = me->SummonCreature(NPC_ANUBAR_GUARDIAN, SpawnPointGuardian[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000))
                        {
                            guardian->AddThreat(me->GetVictim(), 0.0f);
                            DoZoneInCombat(guardian);
                        }
                    }
                    GuardianSummoned = true;
                }

				if (!AssasinSummoned)
                {
                    if (AssasinTimer <= diff)
                    {
                        if (UndergroundPhase > 1)
                        {
                            for (uint8 i = 0; i < 2; ++i)
                            {
                                if (Creature* assasin = me->SummonCreature(NPC_ANUBAR_ASSASIN, SpawnPoint[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000))
                                {
                                    assasin->AddThreat(me->GetVictim(), 0.0f);
                                    DoZoneInCombat(assasin);
                                }
                            }
                            AssasinSummoned = true;
                        }
                    }
					else AssasinTimer -= diff;
                }

                if (!VenomancerSummoned)
                {
                    if (VenomancerTimer <= diff)
                    {
                        if (UndergroundPhase > 2)
                        {
                            for (uint8 i = 0; i < 2; ++i)
                            {
                                if (Creature* venomancer = me->SummonCreature(NPC_ANUBAR_VENOMANCER, SpawnPoint[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000))
                                {
                                    venomancer->AddThreat(me->GetVictim(), 0.0f);
                                    DoZoneInCombat(venomancer);
                                }
                            }
                            VenomancerSummoned = true;
                        }
                    }
					else VenomancerTimer -= diff;
                }

                if (!DatterSummoned)
                {
                    if (DatterTimer <= diff)
                    {
                        if (UndergroundPhase > 3)
                        {
                            for (uint8 i = 0; i < 2; ++i)
                            {
                                if (Creature* datter = me->SummonCreature(NPC_ANUBAR_DATTER, SpawnPoint[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000))
                                {
                                    datter->AddThreat(me->GetVictim(), 0.0f);
                                    DoZoneInCombat(datter);
                                }
                            }
                            DatterSummoned = true;
                        }
                    }
					else DatterTimer -= diff;

					if (me->HasAura(SPELL_LEECHING_SWARM_5N || SPELL_LEECHING_SWARM_5H))
						me->RemoveAurasDueToSpell(DUNGEON_MODE(SPELL_LEECHING_SWARM_5N, SPELL_LEECHING_SWARM_5H));
				}

                if (UndergroundTimer <= diff)
                {
                    me->RemoveAura(SPELL_SUBMERGE);
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    Phase = PHASE_MELEE;
                }
				else UndergroundTimer -= diff;
				break;
            case PHASE_MELEE:
                if (((UndergroundPhase == 0 && HealthBelowPct(95))
					|| UndergroundPhase == 1 && HealthBelowPct(75))
					|| (UndergroundPhase == 2 && HealthBelowPct(50))
					|| (UndergroundPhase == 3 && HealthBelowPct(25))
					&& !me->HasUnitState(UNIT_STATE_CASTING))
                {
                    GuardianSummoned = false;
                    VenomancerSummoned = false;
                    DatterSummoned = false;
					AssasinSummoned = false;

					AssasinTimer = 20*IN_MILLISECONDS;
                    VenomancerTimer = 30*IN_MILLISECONDS;
                    DatterTimer = 35*IN_MILLISECONDS;
					UndergroundTimer = 40*IN_MILLISECONDS;

                    ImpalePhase = 0;
                    ImpaleTimer = 9*IN_MILLISECONDS;

                    DoCast(me, SPELL_SUBMERGE, false);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

                    Phase = PHASE_UNDERGROUND;
                    ++UndergroundPhase;
                }

                if (Channeling == true)
                {
                    for (uint8 i = 0; i < 8; ++i)
                    DoCastVictim(SPELL_SUMMON_CARRION_BEETLES, true);
                    Channeling = false;
                }
                else if (CarrionBeetlesTimer <= diff)
                {
                    Channeling = true;
                    DoCastVictim(SPELL_CARRION_BEETLES);
                    CarrionBeetlesTimer = 25*IN_MILLISECONDS;
                }
				else CarrionBeetlesTimer -= diff;

                if (LeechingSwarmTimer <= diff)
                {
                    DoCast(me, DUNGEON_MODE(SPELL_LEECHING_SWARM_5N, SPELL_LEECHING_SWARM_5H), true);
                    LeechingSwarmTimer = 19*IN_MILLISECONDS;
                }
				else LeechingSwarmTimer -= diff;

                if (PoundTimer <= diff)
                {
                    if (Unit* target = me->GetVictim())
                    {
                        if (Creature* pImpaleTarget = DoSummonImpaleTarget(target))
                            me->CastSpell(pImpaleTarget, DUNGEON_MODE(SPELL_POUND_5N, SPELL_POUND_5H), false);
                    }
                    PoundTimer = 16500;
                }
				else PoundTimer -= diff;

                DoMeleeAttackIfReady();
                break;
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

            Summons.DespawnAll();

			instance->SetData(DATA_ANUBARAK_EVENT, DONE);
        }

        void KilledUnit(Unit* victim)
        {
            if (victim == me)
                return;

            DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
        }

        void JustSummoned(Creature* summon)
        {
            Summons.Summon(summon);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_anub_arakAI(creature);
    }
};

void AddSC_boss_anub_arak()
{
    new boss_anub_arak;
}