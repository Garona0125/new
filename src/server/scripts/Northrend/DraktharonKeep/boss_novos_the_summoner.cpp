/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "SpellScript.h"
#include "QuantumCreature.h"
#include "drak_tharon_keep.h"

enum Texts
{
    SAY_AGGRO                = -1600000,
    SAY_KILL                 = -1600001,
    SAY_DEATH                = -1600002,
    SAY_NECRO_ADD            = -1600003,
    SAY_REUBBLE_1            = -1600004,
    SAY_REUBBLE_2            = -1600005,
};

enum Spells
{
	SPELL_ARCANE_FIELD       = 47346,
	SPELL_BEAM_CHANNEL       = 52106,
	SPELL_ARCANE_BLAST_5N    = 49198,
	SPELL_ARCANE_BLAST_5H    = 59909,
	SPELL_BLIZZARD_5N        = 49034,
	SPELL_BLIZZARD_5H        = 59854,
	SPELL_FROSTBOLT_5N       = 49037,
	SPELL_FROSTBOLT_5H       = 59855,
	SPELL_WRATH_OF_MISERY_5N = 50089,
	SPELL_WRATH_OF_MISERY_5H = 59856,
	SPELL_SUMMON_MINIONS     = 59910,
};

enum CombatPhase
{
    IDLE    = 0,
    PHASE_1 = 1,
    PHASE_2 = 2,
};

enum Events
{
    EVENT_NONE        = 1,
    EVENT_CRYSTAL     = 2,
    EVENT_SUMMON      = 3,
    EVENT_BLIZZARD    = 4,
    EVENT_BLAST       = 5,
    EVENT_FROSTBOLT   = 6,
    EVENT_CURSE       = 7,
    EVENT_HERO_SUMMON = 8,
	EVENT_PHASE_TWO   = 9,
};

enum Counter
{
	MAX_SUMMONS = 5,
};

static Position AddSpawnPoint            = {-379.20f, -816.76f, 59.70f};
static Position CrystalHandlerSpawnPoint = {-326.626343f, -709.956604f, 27.813314f};
static Position AddDestinyPoint          = {-379.314545f, -772.577637f, 28.58837f};

class boss_novos_the_summoner : public CreatureScript
{
public:
    boss_novos_the_summoner() : CreatureScript("boss_novos_the_summoner") { }

    struct boss_novos_the_summonerAI : public QuantumBasicAI
    {
        boss_novos_the_summonerAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
        {
			SetCombatMovement(false);
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        bool OhNovos;
        uint8 crystalHandlerAmount;

        SummonList Summons;
        EventMap events;
        CombatPhase Phase;

        void Reset()
        {
            Phase = IDLE;
            OhNovos = true;
            me->CastStop();
            Summons.DespawnAll();
            crystalHandlerAmount = 0;
            events.Reset();

			DeactivateChanneling();

            if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER))
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);

            if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            if (me->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

			instance->SetData(DATA_NOVOS_EVENT, NOT_STARTED);
		}

        void EnterToBattle(Unit* /*who*/)
        {
            DoSendQuantumText(SAY_AGGRO, me);
            Phase = PHASE_1;
            events.ScheduleEvent(EVENT_CRYSTAL, 30*IN_MILLISECONDS);
            events.ScheduleEvent(EVENT_SUMMON, 1*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_PHASE_TWO, 2*MINUTE*IN_MILLISECONDS);

			ActivateChanneling();

			me->RemoveAllAuras();
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NOT_SELECTABLE);
			DoCastAOE(SPELL_ARCANE_FIELD);
			DoAttackerAreaInCombat(me, 100.0f);
			instance->SetData(DATA_NOVOS_EVENT, IN_PROGRESS);
		}

		void ActivateChanneling()
		{
			if (Creature* novos = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_NOVOS) : 0))
			{
				if (Creature* trigger1 = me->SummonCreature(NPC_CRYSTAL_TARGET_1, -392.455f, -724.809f, 32.2685f, 5.35816f, TEMPSUMMON_MANUAL_DESPAWN, 1000))
					trigger1->AI()->DoCast(novos, SPELL_BEAM_CHANNEL);
			}

			if (Creature* novos = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_NOVOS) : 0))
			{
				if (Creature* trigger2 = me->SummonCreature(NPC_CRYSTAL_TARGET_2, -392.123f, -750.941f, 32.3796f, 0.680678f, TEMPSUMMON_MANUAL_DESPAWN, 1000))
					trigger2->AI()->DoCast(novos, SPELL_BEAM_CHANNEL);
			}

			if (Creature* novos = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_NOVOS) : 0))
			{
				if (Creature* trigger3 = me->SummonCreature(NPC_CRYSTAL_TARGET_3, -365.368f, -751.128f, 32.4213f, 2.35619f, TEMPSUMMON_MANUAL_DESPAWN, 1000))
					trigger3->AI()->DoCast(novos, SPELL_BEAM_CHANNEL);
			}

			if (Creature* novos = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_NOVOS) : 0))
			{
				if (Creature* trigger4 = me->SummonCreature(NPC_CRYSTAL_TARGET_4, -365.477f, -724.849f, 32.3241f, 3.92699f, TEMPSUMMON_MANUAL_DESPAWN, 1000))
					trigger4->AI()->DoCast(novos, SPELL_BEAM_CHANNEL);
			}

			if (GameObject* crystal1 = me->SummonGameObject(GO_NOVOS_CRYSTAL_1, -392.416f, -724.865f, 29.4156f, 3.14159f, 0, 0, 0, 0, 604800))
				crystal1->SetGoState(GO_STATE_ACTIVE);

			if (GameObject* crystal2 = me->SummonGameObject(GO_NOVOS_CRYSTAL_2, -365.41f, -724.865f, 29.4156f, 3.14159f, 0, 0, 0, 0, 604800))
				crystal2->SetGoState(GO_STATE_ACTIVE);

			if (GameObject* crystal3 = me->SummonGameObject(GO_NOVOS_CRYSTAL_3, -392.286f, -751.087f, 29.4156f, 3.14159f, 0, 0, 0, 0, 604800))
				crystal3->SetGoState(GO_STATE_ACTIVE);

			if (GameObject* crystal4 = me->SummonGameObject(GO_NOVOS_CRYSTAL_4, -365.279f, -751.087f, 29.4156f, 3.14159f, 0, 0, 0, 0, 604800))
				crystal4->SetGoState(GO_STATE_ACTIVE);
		}

		void DeactivateChanneling()
		{
			if (GameObject* crystal1 = me->FindGameobjectWithDistance(GO_NOVOS_CRYSTAL_1, 150.0f))
				crystal1->Delete();

			if (GameObject* crystal2 = me->FindGameobjectWithDistance(GO_NOVOS_CRYSTAL_2, 150.0f))
				crystal2->Delete();

			if (GameObject* crystal3 = me->FindGameobjectWithDistance(GO_NOVOS_CRYSTAL_3, 150.0f))
				crystal3->Delete();

			if (GameObject* crystal4 = me->FindGameobjectWithDistance(GO_NOVOS_CRYSTAL_4, 150.0f))
				crystal4->Delete();
		}

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING) && Phase != PHASE_1)
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_CRYSTAL:
                        if (crystalHandlerAmount < 4)
                        {
                            DoSendQuantumText(SAY_NECRO_ADD, me);
                            me->SummonCreature(NPC_CRYSTAL_HANDLER, CrystalHandlerSpawnPoint, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20*IN_MILLISECONDS);
                            events.ScheduleEvent(EVENT_CRYSTAL, 20*IN_MILLISECONDS);
                        }
                        break;
                    case EVENT_SUMMON:
                        me->SummonCreature(RAND(NPC_FETID_TROLL_CORPSE, NPC_HULKING_CORPSE, NPC_RISEN_SHADOWCASTER), AddSpawnPoint, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 20*IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_SUMMON, 4*IN_MILLISECONDS);
                        break;
                    case EVENT_HERO_SUMMON:
                        for (int i = 0; i < MAX_SUMMONS; i++)
                            me->SummonCreature(NPC_FETID_TROLL_CORPSE, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), 0,
                            TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1*IN_MILLISECONDS);
                        events.ScheduleEvent(EVENT_HERO_SUMMON, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                        break;
                    case EVENT_BLIZZARD:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
						{
                            DoCast(target, DUNGEON_MODE(SPELL_BLIZZARD_5N, SPELL_BLIZZARD_5H));
						}
                        events.ScheduleEvent(EVENT_BLIZZARD, 15*IN_MILLISECONDS);
                        break;
                    case EVENT_FROSTBOLT:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
						{
                            DoCast(target, DUNGEON_MODE(SPELL_FROSTBOLT_5N, SPELL_FROSTBOLT_5H));
						}
                        events.ScheduleEvent(EVENT_FROSTBOLT, urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS));
                        break;
                    case EVENT_CURSE:
                        if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
						{
							DoSendQuantumText(RAND(SAY_REUBBLE_1, SAY_REUBBLE_2), me);
                            DoCast(target, DUNGEON_MODE(SPELL_WRATH_OF_MISERY_5N, SPELL_WRATH_OF_MISERY_5H));
						}
                        events.ScheduleEvent(EVENT_CURSE, 9*IN_MILLISECONDS);
                        break;
                    case EVENT_BLAST:
                        DoCastAOE(DUNGEON_MODE(SPELL_ARCANE_BLAST_5N, SPELL_ARCANE_BLAST_5H));
                        events.ScheduleEvent(EVENT_BLAST, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
                        break;
					case EVENT_PHASE_TWO:
						me->CastStop();
						me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NO_AGGRO_FOR_PLAYER | UNIT_FLAG_NOT_SELECTABLE);
						Summons.DespawnAll();
						Phase = PHASE_2;
						events.Reset();
						events.ScheduleEvent(EVENT_BLIZZARD, 0);
						events.ScheduleEvent(EVENT_BLAST, urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS));
						events.ScheduleEvent(EVENT_CURSE, 5*IN_MILLISECONDS);
						events.ScheduleEvent(EVENT_FROSTBOLT, 2*IN_MILLISECONDS);
						if (IsHeroic())
							events.ScheduleEvent(EVENT_HERO_SUMMON, 0);
						break;
                }
            }
        }

        void JustDied(Unit* /*killer*/)
        {
            DoSendQuantumText(SAY_DEATH, me);

			instance->SetData(DATA_NOVOS_EVENT, DONE);

			if (IsHeroic() && OhNovos)
				instance->DoCompleteAchievement(ACHIEVEMENT_OH_NOVOS);

            Summons.DespawnAll();
			DeactivateChanneling();
        }

        void KilledUnit(Unit* who)
        {
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(SAY_KILL, me);
        }

        void JustSummoned(Creature* summon)
        {
            switch (summon->GetEntry())
            {
                case NPC_CRYSTAL_HANDLER:
                    crystalHandlerAmount++;
                    summon->GetMotionMaster()->MovePoint(0, AddDestinyPoint);
                    break;
                case NPC_FETID_TROLL_CORPSE:
                case NPC_HULKING_CORPSE:
                case NPC_RISEN_SHADOWCASTER:
                    summon->GetMotionMaster()->MovePoint(0, AddDestinyPoint);
                    break;
				case NPC_CRYSTAL_TARGET_1:
				case NPC_CRYSTAL_TARGET_2:
				case NPC_CRYSTAL_TARGET_3:
				case NPC_CRYSTAL_TARGET_4:
					break;
                default:
                    break;
            }
            Summons.Summon(summon);
        }

		Unit* GetRandomTarget()
        {
            return SelectTarget(TARGET_RANDOM, 0, 100, true);
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_novos_the_summonerAI(creature);
    }
};

class npc_novos_servant : public CreatureScript
{
public:
    npc_novos_servant() : CreatureScript("npc_novos_servant") { }

    struct npc_novos_servantAI : public QuantumBasicAI
    {
        npc_novos_servantAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE || id != 0)
                return;

            if (Creature* novos = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_NOVOS) : 0))
            {
                CAST_AI(boss_novos_the_summoner::boss_novos_the_summonerAI, novos->AI())->OhNovos = false;
                if (Unit* target = CAST_AI(boss_novos_the_summoner::boss_novos_the_summonerAI, novos->AI())->GetRandomTarget())
                    AttackStart(target);
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_novos_servantAI(creature);
    }
};

void AddSC_boss_novos_the_summoner()
{
    new boss_novos_the_summoner();
    new npc_novos_servant();
}