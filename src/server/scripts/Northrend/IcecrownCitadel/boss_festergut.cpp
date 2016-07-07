/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "SpellAuras.h"
#include "icecrown_citadel.h"

enum Texts
{
    SAY_STINKY_DEAD             = -1659078,
    SAY_AGGRO                   = -1659079,
    EMOTE_GAS_SPORE             = -1659081,
    EMOTE_WARN_GAS_SPORE        = -1659082,
    SAY_PUNGENT_BLIGHT          = -1659083,
    EMOTE_WARN_PUNGENT_BLIGHT   = -1659084,
    EMOTE_PUNGENT_BLIGHT        = -1659085,
    SAY_KILL_1                  = -1659086,
	SAY_KILL_2                  = -1659087,
    SAY_BERSERK                 = -1659088,
    SAY_DEATH                   = -1659089,
};

enum Spells
{
    SPELL_INHALE_BLIGHT         = 69165,
    SPELL_PUNGENT_BLIGHT_10N    = 69195,
	SPELL_PUNGENT_BLIGHT_25N    = 71219,
	SPELL_PUNGENT_BLIGHT_10H    = 73031,
	SPELL_PUNGENT_BLIGHT_25H    = 73032,
    SPELL_GASTRIC_BLOAT_10N     = 72219,
	SPELL_GASTRIC_BLOAT_25N     = 72551,
	SPELL_GASTRIC_BLOAT_10H     = 72552,
	SPELL_GASTRIC_BLOAT_25H     = 72553,
	SPELL_VILE_GAS_10           = 69240,
	SPELL_VILE_GAS_25           = 71218,
    SPELL_INOCULATED            = 69291,
	SPELL_GASTRIC_EXPLOSION     = 72227,
    SPELL_GAS_SPORE             = 69278,
    SPELL_MORTAL_WOUND          = 71127,
    SPELL_DECIMATE              = 71123,
	SPELL_DOUBLE_ATTACK         = 19818,
	SPELL_RADIATION             = 21862,
};

#define DATA_INOCULATED_STACK 69291
#define SPELL_PLAGUE_STENCH_HELPER RAID_MODE<uint32>(71805, 71160, 71160, 71161)
#define SPELL_PUNGENT_BLIGHT_HELPER RAID_MODE<uint32>(69195, 71219, 73031, 73032)
#define SPELL_INOCULATED_HELPER RAID_MODE<uint32>(69291, 72101, 72102, 72103)

uint32 const GaseousBlight[3]       = {69157, 69162, 69164};
uint32 const GaseousBlightVisual[3] = {69126, 69152, 69154};

enum Events
{
    EVENT_BERSERK       = 1,
    EVENT_INHALE_BLIGHT = 2,
    EVENT_VILE_GAS      = 3,
    EVENT_GAS_SPORE     = 4,
    EVENT_GASTRIC_BLOAT = 5,
    EVENT_DECIMATE      = 6,
    EVENT_MORTAL_WOUND  = 7,
};

class boss_festergut : public CreatureScript
{
    public:
        boss_festergut() : CreatureScript("boss_festergut") { }

        struct boss_festergutAI : public BossAI
        {
            boss_festergutAI(Creature* creature) : BossAI(creature, DATA_FESTERGUT)
            {
                _maxInoculatedStack = 0;
                _inhaleCounter = 0;
                _gasDummyGUID = 0;
			}

			uint64 _gasDummyGUID;
            uint32 _maxInoculatedStack;
            uint32 _inhaleCounter;

            void Reset()
            {
                _Reset();
                me->SetReactState(REACT_DEFENSIVE);
                events.ScheduleEvent(EVENT_BERSERK, 300000);
                events.ScheduleEvent(EVENT_INHALE_BLIGHT, urand(25000, 30000));
                events.ScheduleEvent(EVENT_GAS_SPORE, urand(20000, 25000));
                events.ScheduleEvent(EVENT_GASTRIC_BLOAT, urand(12500, 15000));

                _maxInoculatedStack = 0;
                _inhaleCounter = 0;

                me->RemoveAurasDueToSpell(SPELL_BERSERK_2);

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

                if (Creature* gasDummy = me->FindCreatureWithDistance(NPC_GAS_DUMMY, 100.0f, true))
                {
                    _gasDummyGUID = gasDummy->GetGUID();
                    for (uint8 i = 0; i < 3; ++i)
                    {
                        me->RemoveAurasDueToSpell(GaseousBlight[i]);
                        gasDummy->RemoveAurasDueToSpell(GaseousBlightVisual[i]);
                    }
                }
            }

            void EnterToBattle(Unit* who)
            {
                if (!instance->CheckRequiredBosses(DATA_FESTERGUT, who->ToPlayer()))
                {
                    EnterEvadeMode();
                    instance->DoCastSpellOnPlayers(SPELL_LIGHTS_HAMMER_TELEPORT);
                    return;
                }

                me->SetActive(true);
                DoSendQuantumText(SAY_AGGRO, me);

                if (Creature* gasDummy = me->FindCreatureWithDistance(NPC_GAS_DUMMY, 100.0f, true))
                    _gasDummyGUID = gasDummy->GetGUID();

                if (Creature* professor = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_PROFESSOR_PUTRICIDE)))
                    professor->AI()->DoAction(ACTION_FESTERGUT_COMBAT);

                DoZoneInCombat();
            }

            void JustDied(Unit* killer)
            {
                _JustDied();

                DoSendQuantumText(SAY_DEATH, me);

                if (Creature* professor = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_PROFESSOR_PUTRICIDE)))
                    professor->AI()->DoAction(ACTION_FESTERGUT_DEATH);

                RemoveBlight();

				// Quest credit
				if (Player* player = killer->ToPlayer())
				{
					player->CastSpell(player, SPELL_SOUL_FEAST, true);
					me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
				}
            }

            void JustReachedHome()
            {
                _JustReachedHome();
                instance->SetBossState(DATA_FESTERGUT, FAIL);
            }

            void EnterEvadeMode()
            {
                QuantumBasicAI::EnterEvadeMode();

                if (Creature* professor = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_PROFESSOR_PUTRICIDE)))
                    professor->AI()->EnterEvadeMode();
            }

            void KilledUnit(Unit* who)
            {
                if (who->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_KILL_1, SAY_KILL_2), me);
            }

            void SpellHitTarget(Unit* target, SpellInfo const* spell)
            {
                if (spell->Id == SPELL_PUNGENT_BLIGHT_HELPER)
                    target->RemoveAurasDueToSpell(SPELL_INOCULATED_HELPER);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim() || !CheckInRoom())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_INHALE_BLIGHT:
                        {
                            RemoveBlight();
                            if (_inhaleCounter == 3)
                            {
								DoSendQuantumText(SAY_PUNGENT_BLIGHT, me);
                                DoSendQuantumText(EMOTE_WARN_PUNGENT_BLIGHT, me);
                                DoCast(me, RAID_MODE(SPELL_PUNGENT_BLIGHT_10N, SPELL_PUNGENT_BLIGHT_25N, SPELL_PUNGENT_BLIGHT_10H, SPELL_PUNGENT_BLIGHT_25H));
                                _inhaleCounter = 0;
                                if (Creature* professor = ObjectAccessor::GetCreature(*me, instance->GetData64(DATA_PROFESSOR_PUTRICIDE)))
                                    professor->AI()->DoAction(ACTION_FESTERGUT_GAS);
                                events.RescheduleEvent(EVENT_GAS_SPORE, urand(20000, 25000));
                            }
                            else
                            {
                                DoCast(me, SPELL_INHALE_BLIGHT);
                                // just cast and dont bother with target, conditions will handle it
                                ++_inhaleCounter;
                                if (_inhaleCounter < 3)
                                    me->CastSpell(me, GaseousBlight[_inhaleCounter], true, 0, 0, me->GetGUID());
                            }

                            events.ScheduleEvent(EVENT_INHALE_BLIGHT, urand(33500, 35000));
                            break;
                        }
						case EVENT_VILE_GAS: // testing
                        {
                            std::list<Unit*> ranged, melee;
                            uint32 minTargets = RAID_MODE<uint32>(3, 8, 3, 8);
                            SelectTargetList(ranged, 25, TARGET_RANDOM, -5.0f, true);
                            SelectTargetList(melee, 25, TARGET_RANDOM, 5.0f, true);
                            while (ranged.size() < minTargets)
                            {
                                if (melee.empty())
                                    break;

                                Unit* target = Quantum::DataPackets::SelectRandomContainerElement(melee);
                                ranged.push_back(target);
                                melee.remove(target);
                            }

                            if (!ranged.empty())
                            {
                                Quantum::DataPackets::RandomResizeList(ranged, RAID_MODE<uint32>(1, 3, 1, 3));
                                for (std::list<Unit*>::iterator itr = ranged.begin(); itr != ranged.end(); ++itr)
                                    DoCast(*itr, RAID_MODE(SPELL_VILE_GAS_10, SPELL_VILE_GAS_25, SPELL_VILE_GAS_10, SPELL_VILE_GAS_25));
                            }

                            events.ScheduleEvent(EVENT_VILE_GAS, urand(28000, 35000));
                            break;
                        }
                        case EVENT_GAS_SPORE:
							DoSendQuantumText(EMOTE_GAS_SPORE, me);
                            DoSendQuantumText(EMOTE_WARN_GAS_SPORE, me);
                            me->CastCustomSpell(SPELL_GAS_SPORE, SPELLVALUE_MAX_TARGETS, RAID_MODE<int32>(2, 3, 2, 3), me);
                            events.ScheduleEvent(EVENT_GAS_SPORE, urand(40000, 45000));
                            events.RescheduleEvent(EVENT_VILE_GAS, urand(28000, 35000));
                            break;
                        case EVENT_GASTRIC_BLOAT:
                            DoCastVictim(RAID_MODE(SPELL_GASTRIC_BLOAT_10N, SPELL_GASTRIC_BLOAT_25N, SPELL_GASTRIC_BLOAT_10H, SPELL_GASTRIC_BLOAT_25H));
                            events.ScheduleEvent(EVENT_GASTRIC_BLOAT, urand(15000, 17500));
                            break;
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK_2);
                            DoSendQuantumText(SAY_BERSERK, me);
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }

            void SetData(uint32 type, uint32 data)
            {
                if (type == DATA_INOCULATED_STACK && data > _maxInoculatedStack)
                    _maxInoculatedStack = data;
            }

            uint32 GetData(uint32 type)
            {
                if (type == DATA_INOCULATED_STACK)
                    return uint32(_maxInoculatedStack);

                return 0;
            }

            void RemoveBlight()
            {
                if (Creature* gasDummy = ObjectAccessor::GetCreature(*me, _gasDummyGUID))
				{
                    for (uint8 i = 0; i < 3; ++i)
                    {
                        me->RemoveAurasDueToSpell(GaseousBlight[i]);
                        gasDummy->RemoveAurasDueToSpell(GaseousBlightVisual[i]);
                    }
				}
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<boss_festergutAI>(creature);
        }
};

class npc_stinky_icc : public CreatureScript
{
    public:
        npc_stinky_icc() : CreatureScript("npc_stinky_icc") { }

        struct npc_stinky_iccAI : public QuantumBasicAI
        {
            npc_stinky_iccAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
            }

			EventMap events;
            InstanceScript* instance;

            void Reset()
            {
                events.Reset();
                events.ScheduleEvent(EVENT_DECIMATE, urand(20000, 25000));
                events.ScheduleEvent(EVENT_MORTAL_WOUND, urand(3000, 7000));

				DoCast(me, SPELL_RADIATION, true);
				DoCast(me, SPELL_DOUBLE_ATTACK, true);
				DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                DoCast(me, SPELL_PLAGUE_STENCH_HELPER);
            }

			void JustDied(Unit* killer)
            {
                if (Creature* festergut = me->GetCreature(*me, instance->GetData64(DATA_FESTERGUT)))
				{
                    if (festergut->IsAlive())
						DoSendQuantumText(SAY_STINKY_DEAD, festergut);
				}

				// Quest credit
				if (Player* player = killer->ToPlayer())
				{
					player->CastSpell(player, SPELL_SOUL_FEAST, true);
					me->RemoveAurasDueToSpell(SPELL_SHADOWS_FATE);
				}
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_DECIMATE:
                            DoCastVictim(SPELL_DECIMATE);
                            events.ScheduleEvent(EVENT_DECIMATE, urand(20000, 25000));
                            break;
                        case EVENT_MORTAL_WOUND:
                            DoCastVictim(SPELL_MORTAL_WOUND);
                            events.ScheduleEvent(EVENT_MORTAL_WOUND, urand(10000, 12500));
                            break;
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return GetIcecrownCitadelAI<npc_stinky_iccAI>(creature);
        }
};

class spell_festergut_pungent_blight : public SpellScriptLoader
{
    public:
        spell_festergut_pungent_blight() : SpellScriptLoader("spell_festergut_pungent_blight") { }

        class spell_festergut_pungent_blight_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_festergut_pungent_blight_SpellScript);

            bool Load()
            {
                return GetCaster()->GetTypeId() == TYPE_ID_UNIT;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                uint32 blightId = sSpellMgr->GetSpellIdForDifficulty(uint32(GetEffectValue()), GetCaster());

                GetCaster()->RemoveAurasDueToSpell(blightId);
				DoSendQuantumText(EMOTE_PUNGENT_BLIGHT, GetCaster());
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_festergut_pungent_blight_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_festergut_pungent_blight_SpellScript();
        }
};

class spell_festergut_gastric_bloat : public SpellScriptLoader
{
    public:
        spell_festergut_gastric_bloat() : SpellScriptLoader("spell_festergut_gastric_bloat") { }

        class spell_festergut_gastric_bloat_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_festergut_gastric_bloat_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_GASTRIC_EXPLOSION))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                Aura const* aura = GetHitUnit()->GetAura(GetSpellInfo()->Id);
                if (!(aura && aura->GetStackAmount() == 10))
                    return;

                GetHitUnit()->RemoveAurasDueToSpell(GetSpellInfo()->Id);
                GetHitUnit()->CastSpell(GetHitUnit(), SPELL_GASTRIC_EXPLOSION, true);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_festergut_gastric_bloat_SpellScript::HandleScript, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_festergut_gastric_bloat_SpellScript();
        }
};

class spell_festergut_blighted_spores : public SpellScriptLoader
{
    public:
        spell_festergut_blighted_spores() : SpellScriptLoader("spell_festergut_blighted_spores") { }

        class spell_festergut_blighted_spores_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_festergut_blighted_spores_AuraScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_INOCULATED))
                    return false;
                return true;
            }

            void ExtraEffect(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->CastSpell(GetTarget(), SPELL_INOCULATED, true);

                if (InstanceScript* instance = GetTarget()->GetInstanceScript())
				{
                    if (Creature* festergut = ObjectAccessor::GetCreature(*GetTarget(), instance->GetData64(DATA_FESTERGUT)))
                        festergut->AI()->SetData(DATA_INOCULATED_STACK, GetStackAmount());
				}

				HandleResidue();

			}

			void HandleResidue()
            {
                Player* player = GetUnitOwner()->ToPlayer();

                if (!player)
                    return;

                if (player->HasAura(SPELL_ORANGE_BLIGHT_RESIDUE))
                    return;

				player->CastSpell(player, SPELL_ORANGE_BLIGHT_RESIDUE, TRIGGERED_FULL_MASK);
            }

            void Register()
            {
				OnEffectRemove += AuraEffectApplyFn(spell_festergut_blighted_spores_AuraScript::ExtraEffect, EFFECT_0, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_festergut_blighted_spores_AuraScript();
        }
};

class achievement_flu_shot_shortage : public AchievementCriteriaScript
{
public:
	achievement_flu_shot_shortage() : AchievementCriteriaScript("achievement_flu_shot_shortage") { }

	bool OnCheck(Player* /*source*/, Unit* target)
	{
		if (target && target->GetTypeId() == TYPE_ID_UNIT)
			return target->ToCreature()->AI()->GetData(DATA_INOCULATED_STACK) < 3;

		return false;
	}
};

class PlagueStenchTargetSelector
{
public:
	PlagueStenchTargetSelector(Unit* caster) : _caster(caster) { }

	bool operator()(Unit* unit)
	{
		return !unit->IsWithinLOSInMap(_caster);
	}
private:
	Unit* _caster;
};

class spell_stinky_plague_stench : public SpellScriptLoader
{
    public:
        spell_stinky_plague_stench() : SpellScriptLoader("spell_stinky_plague_stench") { }

        class spell_stinky_plague_stench_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_stinky_plague_stench_SpellScript);

            bool Validate(SpellEntry const* spellEntry)
            {
                if (!sSpellStore.LookupEntry(71160))
                    return false;
                if (!sSpellStore.LookupEntry(71161))
                    return false;
                return true;
            }

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if (PlagueStenchTargetSelector(GetCaster()));
            }

            void Register()
            {
                 OnUnitTargetSelect += SpellUnitTargetFn(spell_stinky_plague_stench_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_stinky_plague_stench_SpellScript();
        }
};

void AddSC_boss_festergut()
{
    new boss_festergut();
    new npc_stinky_icc();
    new spell_festergut_pungent_blight();
    new spell_festergut_gastric_bloat();
    new spell_festergut_blighted_spores();
    new achievement_flu_shot_shortage();
	new spell_stinky_plague_stench();
}