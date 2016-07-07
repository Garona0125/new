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
#include "gundrak.h"

enum Emotes
{
    EMOTE_MOJO           = -1604008,
	EMOTE_ACTIVATE_ALTAR = -1604009, // As the last remnants of the %s seep into the ground, the altar set into the floor nearby begins to glow faintly.
};

enum Spells
{
    SPELL_EMERGE         = 54850,
	SPELL_SPAWN_EFFECT   = 54888,
    SPELL_MIGHTY_BLOW    = 54719,
    SPELL_MORTAL_STRIKE  = 54715,
    SPELL_MERGE          = 54878,
    SPELL_SURGE          = 54801,
	SPELL_SURGE_VISUAL   = 54827,
    SPELL_FREEZE_ANIM    = 16245,
    SPELL_MOJO_WAVE_5N   = 55626,
    SPELL_MOJO_WAVE_5H   = 58993,
	SPELL_MOJO_VOLLEY_5N = 54849,
	SPELL_MOJO_VOLLEY_5H = 59453,
};

enum Action
{
    ACTION_FREEZE = 1,
    ACTION_UNFREEZE,
    DATA_EMERGED
};

static Position SpawnLoc[] =
{
    {1669.98f, 753.686f, 143.074f, 4.95674f},
    {1680.67f, 737.104f, 143.083f, 2.53073f},
    {1680.63f, 750.682f, 143.074f, 3.87463f},
    {1663.1f,  743.665f, 143.078f, 6.19592f},
    {1670.39f, 733.493f, 143.073f, 1.27409f},
};

class boss_drakkari_colossus : public CreatureScript
{
    public:
        boss_drakkari_colossus() : CreatureScript("boss_drakkari_colossus") { }

        struct boss_drakkari_colossusAI : public QuantumBasicAI
        {
            boss_drakkari_colossusAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();

                // 100% too much?
                SpellInfo* spell = (SpellInfo*)sSpellMgr->GetSpellInfo(SPELL_MORTAL_STRIKE);
                if (spell)
                    spell->ProcChance = 50;
            }

			InstanceScript* instance;

            uint8 Phase;

            uint32 MightyBlowTimer;
            uint32 MojoDespawnTimer;

            uint64 mojoGUID[5];

            void Reset()
            {
				instance->SetData(DATA_DRAKKARI_COLOSSUS_EVENT, NOT_STARTED);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                DoDespawnMojo();
                DoSpawnMojo();

                MightyBlowTimer = 10*IN_MILLISECONDS;
                Phase = 0;
                MojoDespawnTimer = 2*IN_MILLISECONDS;

                DoAction(ACTION_UNFREEZE);

				me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
            }

            void JustReachedHome()
            {
                DoCast(me, SPELL_FREEZE_ANIM);
            }

            void DoSpawnMojo()
            {
                for (uint8 i = 0; i < 5; ++i)
				{
                    if (Creature* mojo = me->SummonCreature(NPC_LIVING_MOJO, SpawnLoc[i], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1*IN_MILLISECONDS))
                    {
                        mojoGUID[i] = mojo->GetGUID();
                        mojo->SetVisible(true);
                        mojo->SetReactState(REACT_PASSIVE);
                        mojo->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    }
				}
            }

            void DoDespawnMojo()
            {
                for (uint8 i = 0; i < 5; ++i)
                {
                    if (mojoGUID[i])
                    {
                        Creature* mojo = Unit::GetCreature(*me, mojoGUID[i]);
                        if (mojo && mojo->IsAlive())
                        {
                            mojo->SetVisible(false);
                            mojo->DespawnAfterAction();
                        }
                    }
                    mojoGUID[i] = 0;
                }
            }

            void DoMoveMojo()
            {
                for (uint8 i = 0; i < 5; ++i)
				{
                    if (mojoGUID[i])
                    {
                        Creature* mojo = Unit::GetCreature(*me, mojoGUID[i]);
                        if (mojo && mojo->IsAlive())
                        {
                            mojo->GetMotionMaster()->Clear();
                            mojo->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ());
                        }
                    }
				}
            }

            void EnterEvadeMode()
            {
                me->ClearUnitState(UNIT_STATE_STUNNED | UNIT_STATE_ROOT);
                QuantumBasicAI::EnterEvadeMode();
            }

            uint32 GetData(uint32 data)
            {
                if (data == DATA_EMERGED)
                    return (Phase == 4) ? 1 : 0;

                return 0;
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_FREEZE:
                        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        me->AddUnitState(UNIT_STATE_STUNNED | UNIT_STATE_ROOT);
                        DoCast(SPELL_FREEZE_ANIM);
                        break;
                    case ACTION_UNFREEZE:
                        me->ClearUnitState(UNIT_STATE_STUNNED | UNIT_STATE_ROOT);
                        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                        me->RemoveAura(SPELL_FREEZE_ANIM);
                        DoCast(SPELL_MORTAL_STRIKE);
                        break;
                }
            }

            void EnterToBattle(Unit* /*who*/)
            {
				instance->SetData(DATA_DRAKKARI_COLOSSUS_EVENT, IN_PROGRESS);

                me->RemoveAura(SPELL_FREEZE_ANIM);
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                Phase = 1;
            }

            void DamageTaken(Unit* /*attacker*/, uint32 &damage)
            {
                if (damage >= me->GetHealth())
                    damage = me->GetHealth() - 1;
            }

            void JustDied(Unit* /*killer*/)
            {
				DoSendQuantumText(EMOTE_ACTIVATE_ALTAR, me);

				instance->SetData(DATA_DRAKKARI_COLOSSUS_EVENT, DONE);
            }

            void JustSummoned(Creature* summon)
            {
                if (HealthBelowPct(5))
                    summon->DealDamage(summon, uint32(summon->GetHealth() * 0.5), 0, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, 0, false);
                summon->AI()->AttackStart(me->GetVictim());
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

				if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (Phase == 1)
                {
					me->StopMoving();
                    DoAction(ACTION_FREEZE);
                    DoMoveMojo();
                    ++Phase;
                }

                if (Phase == 2)
                {
                    if (MojoDespawnTimer <= diff)
                    {
                        DoDespawnMojo();
                        DoAction(ACTION_UNFREEZE);
                        me->GetMotionMaster()->MoveChase(me->GetVictim());
                        ++Phase;
                    }
                    else MojoDespawnTimer -= diff;
                }

                if (Phase == 3 && HealthBelowPct(50) || Phase == 4 && HealthBelowPct(5))
                {
                    DoAction(ACTION_FREEZE);
                    DoCast(me, SPELL_EMERGE);
                    ++Phase;
                    me->RemoveAllAuras();
                }

                if (me->HasUnitState(UNIT_STATE_STUNNED))
                    return;

                if (MightyBlowTimer <= diff)
                {
                    DoCastVictim(SPELL_MIGHTY_BLOW);
                    MightyBlowTimer = 15*IN_MILLISECONDS;
                }
                else MightyBlowTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_drakkari_colossusAI(creature);
        }
};

class boss_drakkari_elemental : public CreatureScript
{
    public:
        boss_drakkari_elemental() : CreatureScript("boss_drakkari_elemental") { }

        struct boss_drakkari_elementalAI : public QuantumBasicAI
        {
            boss_drakkari_elementalAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();

				DoCast(me, SPELL_SPAWN_EFFECT);
            }

			InstanceScript* instance;

            uint32 SurgeTimer;
            uint32 DisappearTimer;
            uint32 MojoWaveTimer;

            bool Merging;

            void Reset()
            {
                SurgeTimer = 15000;
                DisappearTimer = 2500;
                MojoWaveTimer = 7000;

                Merging = false;
            }

            void EnterEvadeMode()
            {
                if (Creature* colossus = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_DRAKKARI_COLOSSUS) : 0))
                    colossus->AI()->DoAction(ACTION_UNFREEZE);

                me->SetVisible(false);
                me->DisappearAndDie();
            }

            void EnterToBattle(Unit* /*who*/)
            {
                DoCast(me, DUNGEON_MODE(SPELL_MOJO_VOLLEY_5N, SPELL_MOJO_VOLLEY_5H));
            }

            void JustDied(Unit* /*killer*/)
            {
                me->RemoveAurasDueToSpell(DUNGEON_MODE(SPELL_MOJO_VOLLEY_5N, SPELL_MOJO_VOLLEY_5H));

                if (Creature* colossus = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_DRAKKARI_COLOSSUS) : 0))
                {
                    colossus->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                    colossus->Kill(colossus);
                }
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

				if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (!Merging && HealthBelowPct(50))
				{
                    if (Creature* colossus = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_DRAKKARI_COLOSSUS) : 0))
					{
                        if (colossus->AI()->GetData(DATA_EMERGED))
                        {
                            me->InterruptNonMeleeSpells(true);
							DoSendQuantumText(EMOTE_MOJO, me);
							DoCast(me, SPELL_SURGE_VISUAL, true);
                            DoCast(colossus, SPELL_MERGE);
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                            me->RemoveAurasDueToSpell(DUNGEON_MODE(SPELL_MOJO_VOLLEY_5N, SPELL_MOJO_VOLLEY_5H));
                            Merging = true;
                        }
					}
				}

                if (Merging)
				{
                    if (DisappearTimer <= diff)
                    {
                        if (Creature* colossus = Unit::GetCreature(*me, instance ? instance->GetData64(DATA_DRAKKARI_COLOSSUS) : 0))
                            colossus->AI()->DoAction(ACTION_UNFREEZE);

                        me->DisappearAndDie();
                    }
                    else DisappearTimer -= diff;
				}

                if (SurgeTimer <= diff)
                {
					DoCast(me, SPELL_SURGE_VISUAL, true);
                    DoCastVictim(SPELL_SURGE);
                    SurgeTimer = urand(15000, 25000);
                }
                else SurgeTimer -= diff;

                if (MojoWaveTimer <= diff)
                {
                    if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
                    {
                        DoCast(target, DUNGEON_MODE(SPELL_MOJO_WAVE_5N, SPELL_MOJO_WAVE_5H));
                        MojoWaveTimer = urand(10000, 20000);
                    }
                }
                else MojoWaveTimer -= diff;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_drakkari_elementalAI(creature);
        }
};

class spell_mojo_volley_targeting : public SpellScriptLoader
{
    public:
        spell_mojo_volley_targeting() : SpellScriptLoader("spell_mojo_volley_targeting") { }

        class spell_mojo_volley_targeting_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_mojo_volley_targeting_SpellScript);

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
                OnUnitTargetSelect += SpellUnitTargetFn(spell_mojo_volley_targeting_SpellScript::FilterTargetsInitial, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_mojo_volley_targeting_SpellScript::FilterTargetsSubsequent, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
            }

            std::list<Unit*> sharedUnitList;
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_mojo_volley_targeting_SpellScript();
        }
};

class spell_mojo_volley_trigger : public SpellScriptLoader
{
    public:
        spell_mojo_volley_trigger() : SpellScriptLoader("spell_mojo_volley_trigger") { }

        class spell_mojo_volley_trigger_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_mojo_volley_trigger_AuraScript);

            void PeriodicTick(AuraEffect const* /*aurEff*/)
            {
                PreventDefaultAction();
                uint32 triggerSpellId = GetSpellInfo()->Effects[EFFECT_0].TriggerSpell;

                if (Unit* caster = GetCaster())
                    caster->CastCustomSpell(triggerSpellId, SPELLVALUE_MAX_TARGETS, irand(1, 2), caster, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_mojo_volley_trigger_AuraScript::PeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_mojo_volley_trigger_AuraScript();
        }
};

void AddSC_boss_drakkari_colossus()
{
    new boss_drakkari_colossus();
    new boss_drakkari_elemental();
    new spell_mojo_volley_targeting();
    new spell_mojo_volley_trigger();
}