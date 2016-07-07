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
#include "ahnkahet.h"

enum Texts
{
    SAY_AGGRO     = -1619014,
    SAY_SLAY_1    = -1619015,
    SAY_SLAY_2    = -1619016,
    SAY_SLAY_3    = -1619017,
    SAY_DEATH     = -1619018,
    SAY_EGG_SAC_1 = -1619019,
    SAY_EGG_SAC_2 = -1619020,
};

enum Spells
{
    SPELL_BROOD_PLAGUE_5N       = 56130,
    SPELL_BROOD_PLAGUE_5H       = 59467,
    SPELL_BROOD_RAGE            = 59465,
    SPELL_ENRAGE                = 26662,
    SPELL_SUMMON_SWARMERS       = 56119,
    SPELL_SUMMON_SWARM_GUARD    = 56120,
    SPELL_SPRINT                = 56354,
    SPELL_GUARDIAN_AURA         = 56151,
};

enum Datas
{
	DATA_RESPECT_YOUR_ELDERS = 1,
};

enum Events
{
    EVENT_BROOD_PLAGUE   = 1,
    EVENT_BERSERK        = 2,
    EVENT_SUMMON_SWARMER = 3,
    EVENT_CHECK_ENRAGE   = 4,
};

#define EMOTE_HATCHES "An Ahn'kahar Guardian hatches!"

class boss_elder_nadox : public CreatureScript
{
    public:
        boss_elder_nadox() : CreatureScript("boss_elder_nadox") {}

        struct boss_elder_nadoxAI : public QuantumBasicAI
        {
            boss_elder_nadoxAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
            {
                instance = creature->GetInstanceScript();
            }

			bool GuardianDied;

            uint8 HealthAmountModifier;

            InstanceScript* instance;
            SummonList Summons;
            EventMap events;

            void Reset()
            {
                Summons.DespawnAll();
                events.Reset();

                HealthAmountModifier = 1;

                GuardianDied = false;

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

				instance->SetData(DATA_ELDER_NADOX_EVENT, NOT_STARTED);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                DoSendQuantumText(SAY_AGGRO, me);

				instance->SetData(DATA_ELDER_NADOX_EVENT, IN_PROGRESS);

                events.ScheduleEvent(EVENT_BROOD_PLAGUE, 13000);
                events.ScheduleEvent(EVENT_SUMMON_SWARMER, 10000);

                if (IsHeroic())
                {
                    events.ScheduleEvent(EVENT_BERSERK, 12000);
                    events.ScheduleEvent(EVENT_CHECK_ENRAGE, 5000);
                }
            }

            void JustSummoned(Creature* summon)
            {
                Summons.Summon(summon);
                summon->AI()->DoZoneInCombat();
            }

            void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
            {
                if (summon->GetEntry() == NPC_AHNKAHAR_GUARDIAN)
                    GuardianDied = true;
            }

            uint32 GetData(uint32 type)
            {
                if (type == DATA_RESPECT_YOUR_ELDERS)
                    return !GuardianDied ? 1 : 0;

                return 0;
            }

            void KilledUnit(Unit* /*victim*/)
            {
                DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2, SAY_SLAY_3), me);
            }

            void JustDied(Unit* /*killer*/)
            {
                //Summons.DespawnAll();
                DoSendQuantumText(SAY_DEATH, me);

				instance->SetData(DATA_ELDER_NADOX_EVENT, DONE);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

				if (me->HasUnitState(UNIT_STATE_CASTING))
					return;

				events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_BROOD_PLAGUE:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
							{
                                DoCast(target, DUNGEON_MODE(SPELL_BROOD_PLAGUE_5N, SPELL_BROOD_PLAGUE_5H));
							}
                            events.ScheduleEvent(EVENT_BROOD_PLAGUE, 15000);
                            break;
                        case EVENT_BERSERK:
                            DoCast(SPELL_BROOD_RAGE);
                            events.ScheduleEvent(EVENT_BERSERK, urand(5000, 10000));
                            break;
                        case EVENT_SUMMON_SWARMER:
                            DoCast(me, SPELL_SUMMON_SWARMERS);
                            if (urand(1, 3) == 3) // 33% chance of dialog
							{
                                DoSendQuantumText(RAND(SAY_EGG_SAC_1, SAY_EGG_SAC_2), me);
							}
                            events.ScheduleEvent(EVENT_SUMMON_SWARMER, 10000);
                            break;
                        case EVENT_CHECK_ENRAGE:
                            if (me->HasAura(SPELL_ENRAGE))
                                return;
                            if (me->GetPositionZ() < 24.0f)
                                DoCast(me, SPELL_ENRAGE, true);
                            events.ScheduleEvent(EVENT_CHECK_ENRAGE, 5000);
                            break;
                        default:
                            break;
                    }
                }

                if (me->HealthBelowPct(100 - HealthAmountModifier* 25))
                {
                    me->MonsterTextEmote(EMOTE_HATCHES, me->GetGUID(), true);
                    DoCast(me, SPELL_SUMMON_SWARM_GUARD);
                    ++HealthAmountModifier;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_elder_nadoxAI(creature);
        }
};

class npc_ahnkahar_nerubian : public CreatureScript
{
    public:
        npc_ahnkahar_nerubian() : CreatureScript("npc_ahnkahar_nerubian") { }

        struct npc_ahnkahar_nerubianAI : public QuantumBasicAI
        {
            npc_ahnkahar_nerubianAI(Creature* creature) : QuantumBasicAI(creature) {}

			uint32 SprintTimer;

            void Reset()
            {
                if (me->GetEntry() == NPC_AHNKAHAR_GUARDIAN)
                    DoCast(me, SPELL_GUARDIAN_AURA, true);

                SprintTimer = 5*IN_MILLISECONDS;
            }

            void JustDied(Unit* /*killer*/)
            {
                if (me->GetEntry() == NPC_AHNKAHAR_GUARDIAN)
                    me->RemoveAurasDueToSpell(SPELL_GUARDIAN_AURA);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                if (SprintTimer <= diff)
                {
                    DoCast(me, SPELL_SPRINT);
                    SprintTimer = 20*IN_MILLISECONDS;
                }
                else SprintTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_ahnkahar_nerubianAI(creature);
        }
};

class npc_ahnkahar_swarm_egg : public CreatureScript
{
    public:
        npc_ahnkahar_swarm_egg() : CreatureScript("npc_ahnkahar_swarm_egg") { }

        struct npc_ahnkahar_swarm_eggAI : public QuantumBasicAI
        {
            npc_ahnkahar_swarm_eggAI(Creature* creature) : QuantumBasicAI(creature)
            {
				SetCombatMovement(false);
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE);
                creature->UpdateAllStats();
            }

            void Reset(){}

            void EnterToBattle(Unit* /*who*/){}

            void AttackStart(Unit* /*victim*/){}

            void MoveInLineOfSight(Unit* /*who*/){}

            void UpdateAI(uint32 const /*diff*/){}
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_ahnkahar_swarm_eggAI(creature);
        }
};

class GuardianCheck
{
    public:
        bool operator() (Unit* unit)
        {
            if (unit->GetEntry() == NPC_AHNKAHAR_GUARDIAN)
                return true;

            return false;
        }
};

class spell_elder_nadox_guardian_aura : public SpellScriptLoader
{
    public:
        spell_elder_nadox_guardian_aura() : SpellScriptLoader("spell_elder_nadox_guardian_aura") { }

        class spell_elder_nadox_guardian_aura_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_elder_nadox_guardian_aura_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
				unitList.remove_if (GuardianCheck());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_elder_nadox_guardian_aura_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_elder_nadox_guardian_aura_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_elder_nadox_guardian_aura_SpellScript();
        }
};

class achievement_respect_your_elders : public AchievementCriteriaScript
{
public:
	achievement_respect_your_elders() : AchievementCriteriaScript("achievement_respect_your_elders") { }

	bool OnCheck(Player* /*player*/, Unit* target)
	{
		if (!target)
			return false;

		if (Creature* nadox = target->ToCreature())
			if (nadox->AI()->GetData(DATA_RESPECT_YOUR_ELDERS))
				return true;

		return false;
	}
};

void AddSC_boss_elder_nadox()
{
    new boss_elder_nadox();
    new npc_ahnkahar_nerubian();
    new npc_ahnkahar_swarm_egg();
    new spell_elder_nadox_guardian_aura();
    new achievement_respect_your_elders();
}