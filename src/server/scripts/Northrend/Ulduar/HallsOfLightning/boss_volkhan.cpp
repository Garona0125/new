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
#include "Player.h"
#include "SpellInfo.h"
#include "halls_of_lightning.h"

enum Texts
{
    SAY_AGGRO                               = -1602032,
    SAY_SLAY_1                              = -1602033,
    SAY_SLAY_2                              = -1602034,
    SAY_SLAY_3                              = -1602035,
    SAY_DEATH                               = -1602036,
    SAY_STOMP_1                             = -1602037,
    SAY_STOMP_2                             = -1602038,
    SAY_FORGE_1                             = -1602039,
    SAY_FORGE_2                             = -1602040,
    EMOTE_TO_ANVIL                          = -1602041,
    EMOTE_SHATTER                           = -1602042,
};

enum Spells
{
    SPELL_HEAT_5N                           = 52387,
    SPELL_HEAT_5H                           = 59528,
    SPELL_SHATTERING_STOMP_5N               = 52237,
    SPELL_SHATTERING_STOMP_5H               = 59529,
    SPELL_TEMPER                            = 52238,
    SPELL_TEMPER_DUMMY                      = 52654,
    //SPELL_TEMPER_VISUAL                     = 52661,        //summons GO
    SPELL_SUMMON_MOLTEN_GOLEM               = 52405,
    SPELL_BLAST_WAVE                        = 23113,
    SPELL_IMMOLATION_STRIKE_5N              = 52433,
    SPELL_IMMOLATION_STRIKE_5H              = 59530,
    SPELL_SHATTER_5N                        = 52429,
    SPELL_SHATTER_5H                        = 59527,
};

enum Misc
{
    POINT_ID_ANVIL                          = 0,
    MAX_GOLEM                               = 2,

    DATA_SHATTER_RESISTANT                  = 2042,
};

class boss_volkhan : public CreatureScript
{
public:
    boss_volkhan() : CreatureScript("boss_volkhan") { }

    struct boss_volkhanAI : public QuantumBasicAI
    {
        boss_volkhanAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        std::list<uint64> m_lGolemGUIDList;

        bool HasTemper;
        bool IsStriking;
        bool CanShatterGolem;
        bool Move;

        uint8 GolemsShattered;
        uint32 PauseTimer;
        uint32 ShatteringStompTimer;
        uint32 ShatterTimer;
        uint32 CheckTimer;
        uint32 CheckZ;

        uint32 HealthAmountModifier;

        void Reset()
        {
            IsStriking = false;
            HasTemper = false;
            CanShatterGolem = false;
            Move = false;

            PauseTimer = 1500;
            ShatteringStompTimer = 30000;
            ShatterTimer = 3000;
            CheckTimer = 1100;
            CheckZ = 1000;
            GolemsShattered = 0;

            HealthAmountModifier = 1;

            DespawnGolem();

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
            me->SetReactState(REACT_AGGRESSIVE);

            if (!me->HasUnitMovementFlag(MOVEMENTFLAG_WALKING))
                me->SetWalk(true);

			instance->SetData(TYPE_VOLKHAN, NOT_STARTED);
        }

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			instance->SetData(TYPE_VOLKHAN, IN_PROGRESS);
        }

        void AttackStart(Unit* who)
        {
            if (me->Attack(who, true))
            {
                me->AddThreat(who, 0.0f);
                me->SetInCombatWith(who);
                who->SetInCombatWith(me);

                if (!HasTemper)
                    me->GetMotionMaster()->MoveChase(who);
            }
        }

        void JustDied(Unit* /*killer*/)
        {
			DoSendQuantumText(SAY_DEATH, me);

            DespawnGolem();

			instance->SetData(TYPE_VOLKHAN, DONE);
        }

        void KilledUnit(Unit* /*victim*/)
        {
            DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
        }

        void DespawnGolem()
        {
            if (m_lGolemGUIDList.empty())
                return;

            for (std::list<uint64>::const_iterator itr = m_lGolemGUIDList.begin(); itr != m_lGolemGUIDList.end(); ++itr)
            {
                if (Creature* temp = Unit::GetCreature(*me, *itr))
                {
                    if (temp->IsAlive())
                        temp->DespawnAfterAction();
                }
            }

            m_lGolemGUIDList.clear();
        }

        void ShatterGolem()
        {
            if (m_lGolemGUIDList.empty())
                return;

            for (std::list<uint64>::const_iterator itr = m_lGolemGUIDList.begin(); itr != m_lGolemGUIDList.end(); ++itr)
            {
                if (Creature* golem = Unit::GetCreature(*me, *itr))
                {
                    // only shatter brittle golems
                    if (golem->IsAlive() && golem->GetEntry() == NPC_BRITTLE_GOLEM)
                    {
                        golem->CastSpell(golem, DUNGEON_MODE(SPELL_SHATTER_5N, SPELL_SHATTER_5H), false);
                        GolemsShattered++;
                    }
                }
            }
        }

        void JustSummoned(Creature* summoned)
        {
            if (summoned->GetEntry() == NPC_MOLTEN_GOLEM)
            {
                m_lGolemGUIDList.push_back(summoned->GetGUID());

                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                    summoned->AI()->AttackStart(target);

                //useless at this position
                //summoned->CastSpell(summoned, DUNGEON_MODE(SPELL_HEAT_N, SPELL_HEAT_H), false, NULL, NULL, me->GetGUID());
            }
        }

		uint32 GetData(uint32 data) const
		{
			if (data == DATA_SHATTER_RESISTANT)
				return GolemsShattered;

			return 0;
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (CheckZ <= diff)
            {
                if (me->GetPositionZ() < 50.0f)
                {
                    EnterEvadeMode();
                    return;
                }
                CheckZ = 1000;
            }
            else CheckZ -= diff;

            if (IsStriking && !Move)
            {
                if (PauseTimer <= diff)
                {
                    if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    {
                        if (me->GetVictim())
                            me->GetMotionMaster()->MoveChase(me->GetVictim());
                    }

                    HasTemper = false;
                    IsStriking = false;
                    PauseTimer = 1500;
                }
                else PauseTimer -= diff;

                return;
            }

            // ShatteringStomp all the Time, 
            if (!HasTemper && !Move)
            {
                if (ShatteringStompTimer <= diff)
                {
                    DoSendQuantumText(RAND(SAY_STOMP_1, SAY_STOMP_2), me);

                    DoCast(me, DUNGEON_MODE(SPELL_SHATTERING_STOMP_5N, SPELL_SHATTERING_STOMP_5H));

                    if (Creature* temp = me->FindCreatureWithDistance(NPC_BRITTLE_GOLEM, 250.f))
                    {
                        DoSendQuantumText(EMOTE_SHATTER, me);
                        CanShatterGolem = true;
                    }

                    ShatteringStompTimer = 30000;
                }
                else ShatteringStompTimer -= diff;
            }

            // Shatter Golems 3 seconds after Shattering Stomp
            if (CanShatterGolem)
            {
                if (ShatterTimer <= diff)
                {
                    ShatterGolem();
                    ShatterTimer = 3000;
                    CanShatterGolem = false;
                }
                else ShatterTimer -= diff;
            }

            Creature* pAnvil = instance->instance->GetCreature(instance->GetData64(DATA_VOLKHAN_ANVIL));

            float fX, fY, fZ;
            me->GetContactPoint(pAnvil, fX, fY, fZ, INTERACTION_DISTANCE);

            // Health check
            if (!CanShatterGolem && me->HealthBelowPct(100 - 20 * HealthAmountModifier) && !Move)
            {
                ++HealthAmountModifier;

                if (me->IsNonMeleeSpellCasted(false))
                    me->InterruptNonMeleeSpells(false);

				DoSendQuantumText(RAND(SAY_FORGE_1, SAY_FORGE_2), me);

                if (me->GetDistance(pAnvil) > 5)
                {
                    me->GetMotionMaster()->Clear();
                    me->SetReactState(REACT_PASSIVE);
                    me->GetMotionMaster()->MovePoint(5, fX, fY, fZ);
                }

                DoSendQuantumText(EMOTE_TO_ANVIL, me);
                Move = true;
            }

            if (me->IsWithinMeleeRange(pAnvil,5) && Move)
            {
                me->GetMotionMaster()->Clear();
                me->SetReactState(REACT_AGGRESSIVE);
                HasTemper = true;
                Move = false;

                for (uint8 i = 0; i < MAX_GOLEM; ++i)
                    DoCast(SPELL_SUMMON_MOLTEN_GOLEM);

                DoCast(SPELL_TEMPER);
                IsStriking = true;
            }

            if (me->GetMotionMaster()->GetCurrentMovementGeneratorType()!=POINT_MOTION_TYPE && Move)
                //if (CheckTimer<=diff)
                {
                    me->GetMotionMaster()->MovePoint(5,fX,fY,fZ);
                    CheckTimer=1100;
                }
                //else
                    //CheckTimer-=diff;


            DoMeleeAttackIfReady();
        }
	};

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_volkhanAI(creature);
    }
};

class npc_molten_golem : public CreatureScript
{
public:
    npc_molten_golem() : CreatureScript("npc_molten_golem") { }

    struct npc_molten_golemAI : public QuantumBasicAI
    {
        npc_molten_golemAI(Creature* creature) : QuantumBasicAI(creature) {}

        bool IsFrozen;

        uint32 BlastTimer;
        uint32 DeathDelayTimer;
        uint32 ImmolationTimer;

        void Reset()
        {
            IsFrozen = false;

            BlastTimer = 20000;
            DeathDelayTimer = 0;
            ImmolationTimer = 5000;
        }

        void AttackStart(Unit* who)
        {
            if (me->Attack(who, true))
            {
                me->AddThreat(who, 0.0f);
                me->SetInCombatWith(who);
                who->SetInCombatWith(me);

                if (!IsFrozen)
                    me->GetMotionMaster()->MoveChase(who);
            }
        }

        void DamageTaken(Unit* /*pDoneBy*/, uint32 &damage)
        {
            if (damage > me->GetHealth())
            {
                me->UpdateEntry(NPC_BRITTLE_GOLEM);
                me->SetHealth(1);
                damage = 0;
                me->RemoveAllAuras();
                me->AttackStop();
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
                if (me->IsNonMeleeSpellCasted(false))
                    me->InterruptNonMeleeSpells(false);
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() == CHASE_MOTION_TYPE)
                    me->GetMotionMaster()->MovementExpired();
                IsFrozen = true;
            }
        }

        void SpellHit(Unit* /*ñaster*/, const SpellInfo* spell)
        {
            if (spell->Id == SPELL_SHATTER_5N || spell->Id == SPELL_SHATTER_5H)
            {
                if (me->GetEntry() == NPC_BRITTLE_GOLEM)
                    me->DespawnAfterAction();
            }
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim() || IsFrozen)
                return;

            if (BlastTimer <= diff)
            {
                DoCast(me, SPELL_BLAST_WAVE);
                BlastTimer = 20000;
            }
            else BlastTimer -= diff;

            if (ImmolationTimer <= diff)
            {
                DoCastVictim(DUNGEON_MODE(SPELL_IMMOLATION_STRIKE_5N, SPELL_IMMOLATION_STRIKE_5H));
                ImmolationTimer = 5000;
            }
            else ImmolationTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_molten_golemAI(creature);
    }
};

class achievement_shatter_resistant : public AchievementCriteriaScript
{
public:
	achievement_shatter_resistant() : AchievementCriteriaScript("achievement_shatter_resistant") { }

	bool OnCheck(Player* /*source*/, Unit* target)
	{
		return target && target->GetAI()->GetData(DATA_SHATTER_RESISTANT) < 5;
	}
};

void AddSC_boss_volkhan()
{
    new boss_volkhan();
    new npc_molten_golem();
	new achievement_shatter_resistant();
}