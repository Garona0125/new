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
#include "QuantumEscortAI.h"
#include "Vehicle.h"
#include "trial_of_the_champion.h"

enum Texts
{
    SAY_AGGRO          = -1650039,
    SAY_SLAY_1         = -1650040,
	SAY_SLAY_2         = -1650041,
    SAY_SKELETON_PHASE = -1650042,
    SAY_GHOST_PHASE    = -1650043,
    SAY_DEATH          = -1650044,
};

enum Spells
{
    // Phase Knight
    SPELL_PLAGUE_STRIKE_5N    = 67884,
    SPELL_PLAGUE_STRIKE_5H    = 67724,
	SPELL_ICY_TOUCH_5N        = 67718,
    SPELL_ICY_TOUCH_5H        = 67881,
    SPELL_DEATH_RESPITE_5N    = 67745,
    SPELL_DEATH_RESPITE_5H    = 68306,
	SPELL_OBLITERATE_5N       = 67725,
    SPELL_OBLITERATE_5H       = 67883,
    SPELL_RAISE_ARELAS        = 67705,
    SPELL_RAISE_JAEREN        = 67715,
	// Phase Skeleton
    SPELL_ARMY_DEAD           = 67761,
	SPELL_DESECRATION_5N      = 67782,
	SPELL_DESECRATION_5H      = 67876,
	SPELL_DESECRATION_SUMMON  = 67778,
    SPELL_GHOUL_EXPLODE       = 67751,
    SPELL_EXPLODE_5N          = 67729,
    SPELL_EXPLODE_5H          = 67886,
	// Phase Undead Soul
	SPELL_DEATH_BITE_5N       = 67808,
    SPELL_DEATH_BITE_5H       = 67875,
	SPELL_MARKED_DEATH_5N     = 67823,
    SPELL_MARKED_DEATH_5H     = 67882,
    SPELL_BLACK_KNIGHT_RES    = 67693,
    SPELL_CLAW_5N             = 67774,
    SPELL_CLAW_5H             = 67879,
    SPELL_LEAP_5N             = 67749,
    SPELL_LEAP_5H             = 67880,
	SPELL_KILL_CREDIT         = 68663,
};

enum BlackKnightModels
{
    MODEL_SKELETON      = 29846,
    MODEL_GHOST         = 21300,
};

enum Datas
{
	DATA_I_VE_HAD_WORSE = 1,
};

enum Phases
{
    PHASE_UNDEAD   = 1,
    PHASE_SKELETON = 2,
    PHASE_GHOST    = 3,
};

class boss_black_knight : public CreatureScript
{
public:
    boss_black_knight() : CreatureScript("boss_black_knight") { }

    struct boss_black_knightAI : public QuantumBasicAI
    {
        boss_black_knightAI(Creature* creature) : QuantumBasicAI(creature), summons(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        SummonList summons;

        bool ResurrectInProgress;
        bool SummonArmy;
        bool DeathArmyDone;
		bool IveHadWorse;

        uint8 Phase;

        uint32 PlagueStrikeTimer;
        uint32 IcyTouchTimer;
        uint32 DeathRespiteTimer;
        uint32 ObliterateTimer;
        uint32 DesecrationSummonTimer;
        uint32 ResurrectTimer;
        uint32 DeathArmyCheckTimer;
        uint32 GhoulExplodeTimer;
        uint32 DeathBiteTimer;
        uint32 MarkedDeathTimer;

        void Reset()
        {
            summons.DespawnAll();
            me->SetDisplayId(me->GetNativeDisplayId());
            SetEquipmentSlots(true);
            me->ClearUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            ResurrectInProgress = false;
            SummonArmy = false;
            DeathArmyDone = false;
			IveHadWorse = true;

            Phase = PHASE_UNDEAD;

            IcyTouchTimer = urand(5000, 9000);
            PlagueStrikeTimer = urand(10000, 13000);
            DeathRespiteTimer = urand(15000, 16000);
            ObliterateTimer = urand(17000, 19000);
            DesecrationSummonTimer = urand(15000, 16000);
            DeathArmyCheckTimer = 1000;
            ResurrectTimer = 4000;
            GhoulExplodeTimer = 8000;
            DeathBiteTimer = urand (2000, 4000);
            MarkedDeathTimer = urand (5000, 7000);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoCast(instance->GetData(DATA_TEAM) == ALLIANCE ? SPELL_RAISE_ARELAS : SPELL_RAISE_JAEREN);
            DoSendQuantumText(SAY_AGGRO, me);

            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(BOSS_BLACK_KNIGHT) != IN_PROGRESS)
                    instance->SetData(BOSS_BLACK_KNIGHT, IN_PROGRESS);
			}
        }

        void JustReachedHome()
        {
            if (InstanceScript* instance = me->GetInstanceScript())
			{
                if (instance->GetData(BOSS_BLACK_KNIGHT) == IN_PROGRESS)
                    instance->SetData(BOSS_BLACK_KNIGHT, FAIL);
			}
        }

        void KilledUnit(Unit* victim)
        {
            if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

		void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);
			DoCast(me, SPELL_KILL_CREDIT);

			instance->SetData(BOSS_BLACK_KNIGHT, DONE);
        }

		void DoAction(int32 const param)
		{
			if (param == DATA_I_VE_HAD_WORSE)
				IveHadWorse = false;
		}

		uint32 GetData(uint32 type)
		{
			if (type == DATA_I_VE_HAD_WORSE)
				return IveHadWorse ? 1 : 0;
			return 0;
		}

        void JustSummoned(Creature* summon)
        {
            if (summon->GetEntry() == NPC_RISEN_JAEREN || summon->GetEntry() == NPC_RISEN_ARELAS || summon->GetEntry() == NPC_RISEN_CHAMPION)
            {
                summons.Summon(summon);
                summon->AI()->AttackStart(me->GetVictim());
            }
        }

        void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
        {
            summons.Despawn(summon);
            summon->SetCorpseDelay(5*IN_MILLISECONDS);
        }

		void DamageTaken(Unit* /*DoneBy*/, uint32& damage)
        {
            if (damage > me->GetHealth() && Phase <= PHASE_SKELETON)
            {
                damage = 0;
                me->SetHealth(0);
                me->AddUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED);
                me->GetMotionMaster()->MoveIdle();
                ResurrectInProgress = true;
                ExplodeAliveGhouls();
            }
        }

        void ExplodeAliveGhouls()
        {
            if (summons.empty())
                return;

            for (SummonList::iterator itr = summons.begin(); itr != summons.end(); ++itr)
			{
                if (Creature* ghoul = me->GetCreature(*me, *itr))
                    ghoul->CastSpell(ghoul, DUNGEON_MODE(SPELL_EXPLODE_5N, SPELL_EXPLODE_5H));
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (ResurrectInProgress)
            {
                if (ResurrectTimer <= diff)
                {
                    me->SetFullHealth();
                    switch (Phase)
                    {
                        case PHASE_UNDEAD:
                            DoSendQuantumText(SAY_SKELETON_PHASE, me);
                            me->SetDisplayId(MODEL_SKELETON);
                            break;
                        case PHASE_SKELETON:
                            DoSendQuantumText(SAY_GHOST_PHASE, me);
                            me->SetDisplayId(MODEL_GHOST);
                            SetEquipmentSlots(false, EQUIP_UNEQUIP);
                            me->GetMotionMaster()->MoveChase(me->GetVictim());
                            break;
                    }
                    DoCast(me, SPELL_BLACK_KNIGHT_RES, true);
                    Phase++;
                    ResurrectTimer = 4000;
                    ResurrectInProgress = false;
                    me->ClearUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED);
                }
				else ResurrectTimer -= diff;
                return;
            }

            switch (Phase)
            {
                case PHASE_UNDEAD:
                case PHASE_SKELETON:
                {
                    if (IcyTouchTimer <= diff)
                    {
                        DoCastVictim(DUNGEON_MODE(SPELL_ICY_TOUCH_5N, SPELL_ICY_TOUCH_5H));
                        IcyTouchTimer = urand(5000, 7000);
                    }
					else IcyTouchTimer -= diff;

                    if (PlagueStrikeTimer <= diff)
                    {
                        DoCastVictim(DUNGEON_MODE(SPELL_PLAGUE_STRIKE_5N, SPELL_PLAGUE_STRIKE_5H));
                        PlagueStrikeTimer = urand(12000, 15000);
                    }
					else PlagueStrikeTimer -= diff;

                    if (ObliterateTimer <= diff)
                    {
                        DoCastVictim(DUNGEON_MODE(SPELL_OBLITERATE_5N, SPELL_OBLITERATE_5H));
                        ObliterateTimer = urand(17000, 19000);
                    }
					else ObliterateTimer -= diff;

                    switch (Phase)
                    {
                        case PHASE_UNDEAD:
                        {
                            if (DeathRespiteTimer <= diff)
                            {
                                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
                                {
                                    if (target && target->IsAlive())
									{
                                        DoCast(target, DUNGEON_MODE(SPELL_DEATH_RESPITE_5N, SPELL_DEATH_RESPITE_5H));
										DeathRespiteTimer = urand(15000, 16000);
									}
								}
                            }
							else DeathRespiteTimer -= diff;
                            break;
                        }
                        case PHASE_SKELETON:
                        {
                            if (!SummonArmy)
                            {
                                SummonArmy = true;
                                me->AddUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED);
                                DoCast(me, SPELL_ARMY_DEAD);
                            }

                            if (!DeathArmyDone)
                            {
                                if (DeathArmyCheckTimer <= diff)
                                {
                                    me->GetMotionMaster()->MoveChase(me->GetVictim());
                                    me->ClearUnitState(UNIT_STATE_ROOT | UNIT_STATE_STUNNED);
                                    DeathArmyCheckTimer = 0;
                                    DeathArmyDone = true;
                                }
								else DeathArmyCheckTimer -= diff;
                            }

                            if (DesecrationSummonTimer <= diff)
                            {
								DoCast(SPELL_DESECRATION_SUMMON);
                                DesecrationSummonTimer = urand(15000, 16000);
							}
							else DesecrationSummonTimer -= diff;

                            if (!summons.empty() && GhoulExplodeTimer <= diff)
                            {
                                DoCast(me, SPELL_GHOUL_EXPLODE);
                                GhoulExplodeTimer = 8000;
                            }
							else GhoulExplodeTimer -= diff;
                            break;
                        }
                        break;
                    }
                    break;
                }
                case PHASE_GHOST:
                {
                    if (DeathBiteTimer <= diff)
                    {
                        DoCastAOE(DUNGEON_MODE(SPELL_DEATH_BITE_5N, SPELL_DEATH_BITE_5H));
                        DeathBiteTimer = urand (1000, 2000);
                    }
					else DeathBiteTimer -= diff;

                    if (MarkedDeathTimer <= diff)
                    {
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
                        {
                            if (target->IsAlive())
                                DoCast(target, DUNGEON_MODE(SPELL_MARKED_DEATH_5N, SPELL_MARKED_DEATH_5H));
                        }
                        MarkedDeathTimer = urand (5000, 7000);
                    }
					else MarkedDeathTimer -= diff;
                    break;
                }
            }

            if (!me->HasUnitState(UNIT_STATE_ROOT) && !me->HealthBelowPct(1))
                DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_black_knightAI (creature);
    }
};

class npc_risen_ghoul : public CreatureScript
{
public:
    npc_risen_ghoul() : CreatureScript("npc_risen_ghoul") { }

    struct npc_risen_ghoulAI : public QuantumBasicAI
    {
        npc_risen_ghoulAI(Creature* creature) : QuantumBasicAI(creature) {}

        InstanceScript* instance;
        uint32 AttackTimer;
        uint32 LeapTimer;

        void Reset()
        {
            instance = me->GetInstanceScript();
            AttackTimer = 3500;
            LeapTimer = 1000;

            if (Creature* knight = me->GetCreature(*me, instance->GetData64(DATA_BLACK_KNIGHT)))
                knight->AI()->JustSummoned(me);
        }

		void SpellHitTarget(Unit* target, SpellInfo const* spell)
		{
			if (spell->Id == DUNGEON_MODE<uint32>(SPELL_EXPLODE_5N, SPELL_EXPLODE_5H) && target->GetTypeId() == TYPE_ID_PLAYER)
				if (Creature* knight = ObjectAccessor::GetCreature(*me, instance ? instance->GetData64(DATA_BLACK_KNIGHT) : 0))
					knight->AI()->DoAction(DATA_I_VE_HAD_WORSE);
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim() || me->HasUnitState(UNIT_STATE_CASTING))
                return;

            if (LeapTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_FARTHEST, 0, 30, true))
                {
                    DoResetThreat();
                    me->AddThreat(target, 5.0f);
                    DoCast(target, DUNGEON_MODE(SPELL_LEAP_5N, SPELL_LEAP_5H));
                }
                LeapTimer = urand(7000, 10000);
            }
			else LeapTimer -= diff;

            if (AttackTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_CLAW_5N, SPELL_CLAW_5H));
                AttackTimer = urand(1000, 3500);
            }
			else AttackTimer -= diff;
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_risen_ghoulAI(creature);
    }
};

class npc_black_knight_skeletal_gryphon : public CreatureScript
{
public:
    npc_black_knight_skeletal_gryphon() : CreatureScript("npc_black_knight_skeletal_gryphon") { }

    struct npc_black_knight_skeletal_gryphonAI : public npc_escortAI
    {
        npc_black_knight_skeletal_gryphonAI(Creature* creature) : npc_escortAI(creature), vehicleKit(creature->GetVehicleKit())
        {
            instance = creature->GetInstanceScript();
            me->SetSpeed(MOVE_FLIGHT, 3.0f);
        }

        Vehicle* vehicleKit;
        InstanceScript* instance;

        void SetData(uint32 type, uint32 data)
        {
			if (type == 1)
				Start(false, true, 0, NULL);
        }

        void WaypointReached(uint32 id)
        {
            switch (id)
            {
                case 13:
                    if (Creature* announcer = me->GetCreature(*me, instance->GetData64(DATA_ANNOUNCER)))
                        me->SetFacingToObject(announcer);

                    if (Unit* blackKnight = vehicleKit->GetPassenger(0))
                        blackKnight->ExitVehicle();

                    break;
            }
        }

        void PassengerBoarded(Unit* who, int8 /*seatId*/, bool /*apply*/)
        {
            if (who->GetTypeId() == TYPE_ID_PLAYER)
                who->ExitVehicle();
        }

        void UpdateAI(const uint32 diff)
        {
            npc_escortAI::UpdateAI(diff);

            if (!UpdateVictim())
                return;
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_black_knight_skeletal_gryphonAI(creature);
    }
};

class npc_desecration_stalker : public CreatureScript
{
public:
	npc_desecration_stalker() : CreatureScript("npc_desecration_stalker") {}

    struct npc_desecration_stalkerAI : public QuantumBasicAI
    {
        npc_desecration_stalkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();
			SetCombatMovement(false);
		}

        InstanceScript* instance;

        void Reset()
        {
			DoCast(DUNGEON_MODE(SPELL_DESECRATION_5N, SPELL_DESECRATION_5H));
        }

        void UpdateAI(const uint32 /*diff*/)
        {
			if (instance->GetData(BOSS_BLACK_KNIGHT) != IN_PROGRESS)
				me->DespawnAfterAction();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_desecration_stalkerAI(creature);
    }
};

class achievement_i_ve_had_worse : public AchievementCriteriaScript
{
public:
	achievement_i_ve_had_worse() : AchievementCriteriaScript("achievement_i_ve_had_worse"){}

	bool OnCheck(Player* player, Unit* /*target*/)
	{
		if (!player)
			return false;

		if (InstanceScript* instance = player->GetInstanceScript())
		{
			if (Creature* knight = ObjectAccessor::GetCreature(*player, instance->GetData64(DATA_BLACK_KNIGHT)))
			{
				if (knight->AI()->GetData(DATA_I_VE_HAD_WORSE))
					return true;
			}
		}
		return false;
	}
};

void AddSC_boss_black_knight()
{
    new boss_black_knight();
    new npc_risen_ghoul();
    new npc_black_knight_skeletal_gryphon();
	new npc_desecration_stalker();
	new achievement_i_ve_had_worse();
}