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
#include "GameEventMgr.h"
#include "mechanar.h"

enum Texts
{
    SAY_AGGRO                      = -1554013,
    SAY_SUMMON                     = -1554014,
    SAY_DRAGONS_BREATH_1           = -1554015,
    SAY_DRAGONS_BREATH_2           = -1554016,
    SAY_SLAY_1                     = -1554017,
    SAY_SLAY_2                     = -1554018,
    SAY_DEATH                      = -1554019,
};

enum Spells
{
    SPELL_SUMMON_RAGIN_FLAMES_5N = 35275,
	SPELL_SUMMON_RAGIN_FLAMES_5H = 39084,
    SPELL_FROST_ATTACK           = 35263,
    SPELL_ARCANE_BLAST           = 35314,
    SPELL_DRAGONS_BREATH         = 35250,
    SPELL_KNOCKBACK              = 37317,
    SPELL_SOLAR_BURN_5N          = 35267,
	SPELL_SOLAR_BURN_5H          = 38930,
    SPELL_INFERNO_5N             = 35268,
    SPELL_INFERNO_5H             = 39346,
    SPELL_RAGING_FLAMES          = 35278,
};

enum SeasonalData
{
	 GAME_EVENT_WINTER_VEIL = 2,
};

class boss_nethermancer_sepethrea : public CreatureScript
{
    public: boss_nethermancer_sepethrea() : CreatureScript("boss_nethermancer_sepethrea") { }

        struct boss_nethermancer_sepethreaAI : public QuantumBasicAI
        {
            boss_nethermancer_sepethreaAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
            {
                instance = creature->GetInstanceScript();
            }

            InstanceScript* instance;
			SummonList Summons;

            uint32 FrostAttackTimer;
            uint32 ArcaneBlastTimer;
            uint32 DragonsBreathTimer;
            uint32 KnockbackTimer;
            uint32 SolarburnTimer;

            void Reset()
            {
                FrostAttackTimer = 1*IN_MILLISECONDS;
                ArcaneBlastTimer = 3*IN_MILLISECONDS;
                DragonsBreathTimer = 5*IN_MILLISECONDS;
                KnockbackTimer = 7*IN_MILLISECONDS;
                SolarburnTimer = 9*IN_MILLISECONDS;

				Summons.DespawnAll();

				DoCast(me, SPELL_UNIT_DETECTION_ALL);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

				instance->SetData(DATA_NETHERMANCER_EVENT, NOT_STARTED);

				if (sGameEventMgr->IsActiveEvent(GAME_EVENT_WINTER_VEIL))
					me->SetDisplayId(MODEL_CHRISTMAS_SERPETHREA);
            }

            void EnterToBattle(Unit* who)
            {
				if (who->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(SAY_AGGRO, me);

                DoCast(who, DUNGEON_MODE(SPELL_SUMMON_RAGIN_FLAMES_5N, SPELL_SUMMON_RAGIN_FLAMES_5H));
                DoSendQuantumText(SAY_SUMMON, me);

				instance->SetData(DATA_NETHERMANCER_EVENT, IN_PROGRESS);
            }

            void KilledUnit(Unit* victim)
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

            void JustDied(Unit* /*killer*/)
            {
				DoSendQuantumText(SAY_DEATH, me);

				Summons.DespawnAll();

				instance->SetData(DATA_NETHERMANCER_EVENT, DONE);
            }

			void JustSummoned(Creature* summon)
			{
				if (summon->GetEntry() == NPC_RAGING_FLAMES)
				{
					summon->AI()->DoZoneInCombat();
					Summons.Summon(summon);
				}
			}

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

                if (FrostAttackTimer <= diff)
                {
                    DoCastVictim(SPELL_FROST_ATTACK);
                    FrostAttackTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
                }
                else FrostAttackTimer -= diff;

                if (ArcaneBlastTimer <= diff)
                {
                    DoCastVictim(SPELL_ARCANE_BLAST);
                    ArcaneBlastTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
                }
                else ArcaneBlastTimer -= diff;

                if (DragonsBreathTimer <= diff)
                {
                    DoCastVictim(SPELL_DRAGONS_BREATH);
					DoSendQuantumText(RAND(SAY_DRAGONS_BREATH_1, SAY_DRAGONS_BREATH_2), me);
                    DragonsBreathTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
                }
                else DragonsBreathTimer -= diff;

                if (KnockbackTimer <= diff)
                {
                    DoCastVictim(SPELL_KNOCKBACK);
                    KnockbackTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
                }
                else KnockbackTimer -= diff;

                if (SolarburnTimer <= diff)
                {
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, DUNGEON_MODE(SPELL_SOLAR_BURN_5N, SPELL_SOLAR_BURN_5H));
						SolarburnTimer = urand(11*IN_MILLISECONDS, 12*IN_MILLISECONDS);
					}
                }
                else SolarburnTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_nethermancer_sepethreaAI(creature);
        }
};

class npc_raging_flames : public CreatureScript
{
    public:
        npc_raging_flames() : CreatureScript("npc_raging_flames") { }

		struct npc_raging_flamesAI : public QuantumBasicAI
		{
			npc_raging_flamesAI(Creature* creature) : QuantumBasicAI(creature)
			{
				instance = creature->GetInstanceScript();
			}

			InstanceScript* instance;

			uint32 RagingFlameTimer;
			uint32 InfernoTimer;
			uint32 CheckTimer;

			bool onlyonce;

			void Reset()
			{
				RagingFlameTimer = 0.5*IN_MILLISECONDS;
				InfernoTimer = 3*IN_MILLISECONDS;
				CheckTimer = 2*IN_MILLISECONDS;

				onlyonce = false;

				me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_MAGIC, true);
				me->ApplySpellImmune(0, IMMUNITY_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, true);

				me->SetSpeed(MOVE_RUN, DUNGEON_MODE(0.5f, 0.7f));
			}

			void EnterToBattle(Unit* /*who*/){}

			void UpdateAI(const uint32 diff)
			{
				if (CheckTimer <= diff)
				{
					if (instance->GetData(DATA_NETHERMANCER_EVENT) != IN_PROGRESS)
					{
						me->DespawnAfterAction();
						CheckTimer = 1*IN_MILLISECONDS;
					}
				}
				else CheckTimer -= diff;

				if (!UpdateVictim())
					return;

				if (!onlyonce)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						me->GetMotionMaster()->MoveChase(target);

					onlyonce = true;
				}

				if (InfernoTimer <= diff)
				{
					DoCast(me, DUNGEON_MODE(SPELL_INFERNO_5N, SPELL_INFERNO_5H));
					me->TauntApply(me->GetVictim());
					InfernoTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
				else InfernoTimer -= diff;

				if (RagingFlameTimer <= diff)
				{
					DoCastAOE(SPELL_RAGING_FLAMES);
					RagingFlameTimer = (9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
				}
				else RagingFlameTimer -= diff;

				DoMeleeAttackIfReady();
			}
		};

		CreatureAI* GetAI(Creature* creature) const
		{
			return new npc_raging_flamesAI(creature);
		}
};

void AddSC_boss_nethermancer_sepethrea()
{
    new boss_nethermancer_sepethrea();
    new npc_raging_flames();
}