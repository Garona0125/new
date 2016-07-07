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
#include "SpellAuraEffects.h"
#include "naxxramas.h"

enum Spells
{
    SPELL_NECROTIC_AURA           = 55593,
    SPELL_SUMMON_SPORE            = 29234,
    SPELL_DEATHBLOOM              = 29865,
    SPELL_INEVITABLE_DOOM_10      = 29204,
    SPELL_INEVITABLE_DOOM_25      = 55052,
    SPELL_BERSERK                 = 27680,
    SPELL_FUNGAL_CREEP            = 29232,
    SPELL_WARN_NECROTIC_AURA      = 59481,
	SPELL_DEATHBLOOM_FINAL_DAMAGE = 55594,
};

enum Events
{
    EVENT_NECROTIC_AURA   = 1,
    EVENT_DEATHBLOOM      = 2,
    EVENT_INEVITABLE_DOOM = 3,
	EVENT_BERSERK         = 4,
};

enum Texts
{
   SAY_NECROTIC_AURA_APPLIED = 0,
   SAY_NECROTIC_AURA_REMOVED = 1,
   SAY_NECROTIC_AURA_FADING  = 2,
};

enum Achievement
{
   DATA_ACHIEVEMENT_SPORE_LOSER = 21822183,
};

class boss_loatheb : public CreatureScript
{
    public:
        boss_loatheb() : CreatureScript("boss_loatheb") { }

        struct boss_loathebAI : public BossAI
        {
            boss_loathebAI(Creature* creature) : BossAI(creature, BOSS_LOATHEB) {}

			bool SporeLoser;

            void Reset()
            {
                _Reset();

				SporeLoser = true;

				instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_FUNGAL_CREEP);

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

			void EnterToBattle(Unit* /*who*/)
            {
                _EnterToBattle();

                events.ScheduleEvent(EVENT_NECROTIC_AURA, 10000);
                events.ScheduleEvent(EVENT_DEATHBLOOM, 5000);
                events.ScheduleEvent(EVENT_INEVITABLE_DOOM, 120000);
                events.ScheduleEvent(EVENT_BERSERK, 12*60000);
            }

			void JustDied(Unit* /*killer*/)
			{
				_JustDied();

				if (Creature* kel = me->FindCreatureWithDistance(NPC_MB_KEL_THUZAD, 500.0f))
					DoSendQuantumText(SAY_KELTHUZAD_TAUNT_2, kel);
			}

			void SummonedCreatureDies(Creature* /*summon*/, Unit* /*killer*/)
            {
                SporeLoser = false;
            }

			uint32 GetData(uint32 id)
            {
                if (id != DATA_ACHIEVEMENT_SPORE_LOSER)
                   return 0;

                return uint32(SporeLoser);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_NECROTIC_AURA:
                            DoCastAOE(SPELL_NECROTIC_AURA);
                            events.ScheduleEvent(EVENT_NECROTIC_AURA, 20000);
                            break;
                        case EVENT_DEATHBLOOM:
                            DoCastAOE(SPELL_SUMMON_SPORE, true);
                            DoCastAOE(SPELL_DEATHBLOOM);
                            events.ScheduleEvent(EVENT_DEATHBLOOM, 30000);
                            break;
                        case EVENT_INEVITABLE_DOOM:
                            DoCastAOE(RAID_MODE<uint32>(SPELL_INEVITABLE_DOOM_10, SPELL_INEVITABLE_DOOM_25));
                            events.ScheduleEvent(EVENT_INEVITABLE_DOOM, events.GetTimer() < 5*60000 ? 30000 : 15000);
                            break;
                        case EVENT_BERSERK:
                            if (Is25ManRaid() && !me->HasAura(SPELL_BERSERK))
                                DoCast(me, SPELL_BERSERK, true);
                            events.ScheduleEvent(EVENT_BERSERK, 60000);
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_loathebAI(creature);
        }
};

class npc_loatheb_spore : public CreatureScript
{
public:
	npc_loatheb_spore() : CreatureScript("npc_loatheb_spore") { }

	struct npc_loatheb_sporeAI : public QuantumBasicAI
	{
		npc_loatheb_sporeAI(Creature* creature) : QuantumBasicAI(creature) {}

		void DamageTaken(Unit* /*attacker*/, uint32 &damage)
		{
			if (damage >= me->GetHealth())
				DoCastAOE(SPELL_FUNGAL_CREEP, true);
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_loatheb_sporeAI(creature);
	}
};

class spell_fungal_creep_targeting : public SpellScriptLoader
{
    public:
        spell_fungal_creep_targeting() : SpellScriptLoader("spell_fungal_creep_targeting") { }

        class spell_fungal_creep_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_fungal_creep_targeting_SpellScript);

			std::list<Unit*> sharedUnitList;

            void FilterTargetsInitial(std::list<Unit*>& unitList)
            {
                sharedUnitList = unitList;
            }

            void FilterTargetsSubsequent(std::list<Unit*>& unitList)
            {
                unitList = sharedUnitList;
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_fungal_creep_targeting_SpellScript::FilterTargetsInitial, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_fungal_creep_targeting_SpellScript::FilterTargetsSubsequent, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_fungal_creep_targeting_SpellScript::FilterTargetsSubsequent, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_fungal_creep_targeting_SpellScript();
        }
};


typedef boss_loatheb::boss_loathebAI LoathebAI;

class spell_loatheb_necrotic_aura_warning : public SpellScriptLoader
{
    public:
        spell_loatheb_necrotic_aura_warning() : SpellScriptLoader("spell_loatheb_necrotic_aura_warning") { }

        class spell_loatheb_necrotic_aura_warning_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_loatheb_necrotic_aura_warning_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellStore.LookupEntry(SPELL_WARN_NECROTIC_AURA))
                    return false;
                return true;
            }

            void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget()->IsAIEnabled)
                    CAST_AI(LoathebAI, GetTarget()->GetAI())->Talk(SAY_NECROTIC_AURA_APPLIED);
            }

            void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget()->IsAIEnabled)
                    CAST_AI(LoathebAI, GetTarget()->GetAI())->Talk(SAY_NECROTIC_AURA_REMOVED);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_loatheb_necrotic_aura_warning_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_loatheb_necrotic_aura_warning_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_loatheb_necrotic_aura_warning_AuraScript();
        }
};

class spell_loatheb_deathbloom : public SpellScriptLoader
{
    public:
        spell_loatheb_deathbloom() : SpellScriptLoader("spell_loatheb_deathbloom") { }

        class spell_loatheb_deathbloom_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_loatheb_deathbloom_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_DEATHBLOOM_FINAL_DAMAGE))
                    return false;
                return true;
            }

            void AfterRemove(AuraEffect const* eff, AuraEffectHandleModes /*mode*/)
            {
                if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
                    return;

                GetTarget()->CastSpell(NULL, SPELL_DEATHBLOOM_FINAL_DAMAGE, true, NULL, eff, GetCasterGUID());
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_loatheb_deathbloom_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_loatheb_deathbloom_AuraScript();
        }
};

class achievement_spore_loser : public AchievementCriteriaScript
{
public:
	achievement_spore_loser() : AchievementCriteriaScript("achievement_spore_loser") { }

	bool OnCheck(Player* /*source*/, Unit* target)
	{
		return target && target->GetAI()->GetData(DATA_ACHIEVEMENT_SPORE_LOSER);
	}
};

void AddSC_boss_loatheb()
{
    new boss_loatheb();
    new npc_loatheb_spore();
	new spell_fungal_creep_targeting();
    new spell_loatheb_necrotic_aura_warning();
	new spell_loatheb_deathbloom();
	new achievement_spore_loser();
}