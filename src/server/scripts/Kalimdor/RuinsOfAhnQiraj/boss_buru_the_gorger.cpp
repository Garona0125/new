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
#include "SpellScript.h"
#include "ruins_of_ahnqiraj.h"

enum Emotes
{
    EMOTE_TARGET                = 0,
};

enum Spells
{
    SPELL_CREEPING_PLAGUE       = 20512,
    SPELL_DISMEMBER             = 96,
    SPELL_GATHERING_SPEED       = 1834,
    SPELL_FULL_SPEED            = 1557,
    SPELL_THORNS                = 25640,
    SPELL_BURU_TRANSFORM        = 24721,
    SPELL_SUMMON_HATCHLING      = 1881,
    SPELL_EXPLODE               = 19593,
    SPELL_EXPLODE_2             = 5255,
    SPELL_BURU_EGG_TRIGGER      = 26646,
};

enum Events
{
    EVENT_DISMEMBER             = 0,
    EVENT_GATHERING_SPEED       = 1,
    EVENT_FULL_SPEED            = 2,
    EVENT_CREEPING_PLAGUE       = 3,
    EVENT_RESPAWN_EGG           = 4,
};

enum Phases
{
    PHASE_EGG                   = 0,
    PHASE_TRANSFORM             = 1,
};

enum Actions
{
    ACTION_EXPLODE              = 0,
};

class boss_buru_the_gorger : public CreatureScript
{
    public:
        boss_buru_the_gorger() : CreatureScript("boss_buru_the_gorger") { }

        struct boss_buru_the_gorgerAI : public BossAI
        {
            boss_buru_the_gorgerAI(Creature* creature) : BossAI(creature, DATA_BURU){}

			uint8 Phase;

			void Reset()
			{
				_Reset();

				DoCast(me, SPELL_UNIT_DETECTION_ALL);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			}

            void EnterEvadeMode()
            {
                BossAI::EnterEvadeMode();

				EggRespawn();
            }

            void EnterToBattle(Unit* victim)
            {
                _EnterToBattle();

                Talk(EMOTE_TARGET, victim->GetGUID());

                DoCast(me, SPELL_THORNS);

                events.ScheduleEvent(EVENT_DISMEMBER, 5*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_GATHERING_SPEED, 9*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_FULL_SPEED, 1*MINUTE*IN_MILLISECONDS);

                Phase = PHASE_EGG;
            }

			void KilledUnit(Unit* victim)
            {
                if (victim->GetTypeId() == TYPE_ID_PLAYER)
                    ChaseNewVictim();
            }

			void JustDied(Unit* /*killer*/)
			{
				_JustDied();

				EggDespawn();
			}

			void EggRespawn()
			{
				std::list<Creature*> BuruEggList;
				me->GetCreatureListWithEntryInGrid(BuruEggList, NPC_BURU_EGG, 320.0f);

				if (!BuruEggList.empty())
				{
					for (std::list<Creature*>::const_iterator itr = BuruEggList.begin(); itr != BuruEggList.end(); ++itr)
					{
						if (Creature* egg = *itr)
							egg->Respawn();
					}
				}
			}

			void EggDespawn()
			{
				std::list<Creature*> BuruEggList;
				me->GetCreatureListWithEntryInGrid(BuruEggList, NPC_BURU_EGG, 320.0f);

				if (!BuruEggList.empty())
				{
					for (std::list<Creature*>::const_iterator itr = BuruEggList.begin(); itr != BuruEggList.end(); ++itr)
					{
						if (Creature* egg = *itr)
							egg->DespawnAfterAction();
					}
				}
			}

            void DoAction(const int32 action)
            {
                if (action == ACTION_EXPLODE)
				{
                    if (Phase == PHASE_EGG)
                        me->DealDamage(me, 45000);
				}
            }

            void ChaseNewVictim()
            {
                if (Phase != PHASE_EGG)
                    return;

                me->RemoveAurasDueToSpell(SPELL_FULL_SPEED);
                me->RemoveAurasDueToSpell(SPELL_GATHERING_SPEED);

                events.ScheduleEvent(EVENT_GATHERING_SPEED, 9*IN_MILLISECONDS);
                events.ScheduleEvent(EVENT_FULL_SPEED, 1*MINUTE*IN_MILLISECONDS);

                if (Unit* victim = SelectTarget(TARGET_RANDOM, 0, 0.0f, true))
                {
                    DoResetThreat();
                    AttackStart(victim);
                    Talk(EMOTE_TARGET, victim->GetGUID());
                }
            }

            void ManageRespawn(uint64 EggGUID)
            {
                ChaseNewVictim();
                events.ScheduleEvent(EVENT_RESPAWN_EGG, 1.5*MINUTE*IN_MILLISECONDS);
            }

            void UpdateAI(const uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DISMEMBER:
                            DoCastVictim(SPELL_DISMEMBER);
                            events.ScheduleEvent(EVENT_DISMEMBER, 5*IN_MILLISECONDS);
                            break;
                        case EVENT_GATHERING_SPEED:
                            DoCast(me, SPELL_GATHERING_SPEED);
                            events.ScheduleEvent(EVENT_GATHERING_SPEED, 9*IN_MILLISECONDS);
                            break;
                        case EVENT_FULL_SPEED:
                            DoCast(me, SPELL_FULL_SPEED);
                            break;
                        case EVENT_CREEPING_PLAGUE:
                            DoCast(me, SPELL_CREEPING_PLAGUE);
                            events.ScheduleEvent(EVENT_CREEPING_PLAGUE, 6*IN_MILLISECONDS);
                            break;
                        case EVENT_RESPAWN_EGG:
							EggRespawn();
                            break;
                        default:
                            break;
                    }
                }

                if (me->GetHealthPct() < 20.0f && Phase == PHASE_EGG)
                {
                    DoCast(me, SPELL_BURU_TRANSFORM); // Enrage
                    DoCast(me, SPELL_FULL_SPEED, true);
                    me->RemoveAurasDueToSpell(SPELL_THORNS);
                    Phase = PHASE_TRANSFORM;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_buru_the_gorgerAI(creature);
        }
};

class npc_buru_egg : public CreatureScript
{
    public:
        npc_buru_egg() : CreatureScript("npc_buru_egg") { }

        struct npc_buru_eggAI : public QuantumBasicAI
        {
            npc_buru_eggAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = me->GetInstanceScript();

                SetCombatMovement(false);
            }

			InstanceScript* instance;

            void EnterToBattle(Unit* victim)
            {
                if (Creature* buru = me->GetMap()->GetCreature(instance->GetData64(DATA_BURU)))
				{
                    if (!buru->IsInCombatActive())
                        buru->AI()->AttackStart(victim);
				}
            }

			void JustDied(Unit* /*killer*/)
            {
                DoCastAOE(SPELL_EXPLODE, true);
                DoCastAOE(SPELL_EXPLODE_2, true);
                DoCast(me, SPELL_SUMMON_HATCHLING, true);

                if (Creature* buru = me->GetMap()->GetCreature(instance->GetData64(DATA_BURU)))
				{
                    if (boss_buru_the_gorger::boss_buru_the_gorgerAI* buruAI = dynamic_cast<boss_buru_the_gorger::boss_buru_the_gorgerAI*>(buru->AI()))
                        buruAI->ManageRespawn(me->GetGUID());
				}
            }

            void JustSummoned(Creature* who)
            {
                if (who->GetEntry() == NPC_HATCHLING)
				{
                    if (Creature* buru = me->GetMap()->GetCreature(instance->GetData64(DATA_BURU)))
					{
                        if (Unit* target = buru->AI()->SelectTarget(TARGET_RANDOM))
                            who->AI()->AttackStart(target);
					}
				}
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_buru_eggAI(creature);
        }
};

class spell_egg_explosion : public SpellScriptLoader
{
    public:
        spell_egg_explosion() : SpellScriptLoader("spell_egg_explosion") { }

        class spell_egg_explosion_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_egg_explosion_SpellScript);

            void HandleAfterCast()
            {
                if (Creature* buru = GetCaster()->FindCreatureWithDistance(NPC_BURU, 150.0f))
                    buru->AI()->DoAction(ACTION_EXPLODE);
            }

            void HandleDummyHitTarget(SpellEffIndex /*effIndex*/)
            {
                if (Unit* target = GetHitUnit())
                    GetCaster()->DealDamage(target, -16 * GetCaster()->GetDistance(target) + 500);
            }

            void Register()
            {
                AfterCast += SpellCastFn(spell_egg_explosion_SpellScript::HandleAfterCast);
                OnEffectHitTarget += SpellEffectFn(spell_egg_explosion_SpellScript::HandleDummyHitTarget, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_egg_explosion_SpellScript();
        }
};

void AddSC_boss_buru_the_gorger()
{
    new boss_buru_the_gorger();
    new npc_buru_egg();
    new spell_egg_explosion();
}