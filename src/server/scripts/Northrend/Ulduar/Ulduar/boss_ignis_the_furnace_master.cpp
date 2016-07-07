/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

// Slag Pot Damage (Need Fix) Total Scripting (95%)

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "SpellScript.h"
#include "Vehicle.h"
#include "ulduar.h"

enum Texts
{
    SAY_AGGRO                     = -1603220,
    SAY_SLAY_1                    = -1603221,
    SAY_SLAY_2                    = -1603222,
    SAY_DEATH                     = -1603223,
    SAY_SUMMON                    = -1603224,
    SAY_SLAG_POT                  = -1603225,
    SAY_SCORCH_1                  = -1603226,
    SAY_SCORCH_2                  = -1603227,
    SAY_BERSERK                   = -1603228,
    EMOTE_JETS                    = -1603229,
};

enum Spells
{
    SPELL_FLAME_JETS_10           = 62680,
	SPELL_FLAME_JETS_25           = 63472,
    SPELL_SCORCH_10               = 62546,
	SPELL_SCORCH_25               = 63474,
    SPELL_SLAG_POT_10             = 62717,
	SPELL_SLAG_POT_25             = 63477,
    SPELL_SLAG_POT_DAMAGE_10      = 65722,
	SPELL_SLAG_POT_DAMAGE_25      = 65723,
    SPELL_SLAG_IMBUED_10          = 62836,
	SPELL_SLAG_IMBUED_25          = 63536,
    SPELL_ACTIVATE_CONSTRUCT      = 62488,
    SPELL_STRENGTH                = 64473,
    SPELL_GRAB                    = 62707,
    SPELL_BERSERK                 = 47008,
    SPELL_GRAB_ENTER_VEHICLE      = 62711,
    SPELL_HEAT                    = 65667,
    SPELL_MOLTEN                  = 62373,
    SPELL_BRITTLE_10              = 62382,
    SPELL_BRITTLE_25              = 67114,
    SPELL_SHATTER                 = 62383,
    SPELL_FREEZE_ANIM             = 63354,
    SPELL_SCORCH_GROUND_10        = 62548,
	SPELL_SCORCH_GROUND_25        = 63476,
};

enum Events
{
    EVENT_JET          = 1,
    EVENT_SCORCH       = 2,
    EVENT_SLAG_POT     = 3,
    EVENT_GRAB_POT     = 4,
    EVENT_CHANGE_POT   = 5,
	EVENT_END_POT      = 6,
    EVENT_CONSTRUCT    = 7,
    EVENT_BERSERK      = 8,
};

enum Actions
{
	ACTION_REMOVE_BUFF = 20,
};

enum AchievementData
{
	DATA_SHATTERED                = 1,
    ACHIEVEMENT_TIMED_START_EVENT = 20951,
};

Position const Pos[20] =
{
    {630.366f, 216.772f, 360.891f, 3.001970f},
    {630.594f, 231.846f, 360.891f, 3.124140f},
    {630.435f, 337.246f, 360.886f, 3.211410f},
    {630.493f, 313.349f, 360.886f, 3.054330f},
    {630.444f, 321.406f, 360.886f, 3.124140f},
    {630.366f, 247.307f, 360.888f, 3.211410f},
    {630.698f, 305.311f, 360.886f, 3.001970f},
    {630.500f, 224.559f, 360.891f, 3.054330f},
    {630.668f, 239.840f, 360.890f, 3.159050f},
    {630.384f, 329.585f, 360.886f, 3.159050f},
    {543.220f, 313.451f, 360.886f, 0.104720f},
    {543.356f, 329.408f, 360.886f, 6.248280f},
    {543.076f, 247.458f, 360.888f, 6.213370f},
    {543.117f, 232.082f, 360.891f, 0.069813f},
    {543.161f, 305.956f, 360.886f, 0.157080f},
    {543.277f, 321.482f, 360.886f, 0.052360f},
    {543.316f, 337.468f, 360.886f, 6.195920f},
    {543.280f, 239.674f, 360.890f, 6.265730f},
    {543.265f, 217.147f, 360.891f, 0.174533f},
    {543.256f, 224.831f, 360.891f, 0.122173f},
};

class boss_ignis_the_furnace_master : public CreatureScript
{
    public:
        boss_ignis_the_furnace_master() : CreatureScript("boss_ignis_the_furnace_master") { }

        struct boss_ignis_the_furnace_master_AI : public BossAI
        {
            boss_ignis_the_furnace_master_AI(Creature* creature) : BossAI(creature, DATA_IGNIS_FURNACE_MASTER), vehicle(me->GetVehicleKit())
            {
                assert(vehicle);
            }

			Vehicle* vehicle;
            std::list<Creature*> creatureList;
            bool Shattered;
            uint64 SlagPotGUID;
            uint32 ConstructTimer;

            void Reset()
            {
                _Reset();

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

                if (vehicle)
                    vehicle->RemoveAllPassengers();

                creatureList.clear();

                for (uint8 i = 0; i < 20; ++i)
				{
                    if (Creature* construct = me->SummonCreature(NPC_IRON_CONSTRUCT, Pos[i]))
                        creatureList.push_back(construct);
				}

				instance->DoStopTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEVEMENT_TIMED_START_EVENT);
            }

            void EnterToBattle(Unit* /*who*/)
            {
                _EnterToBattle();

                DoSendQuantumText(SAY_AGGRO, me);

                events.ScheduleEvent(EVENT_JET, 30000);
                events.ScheduleEvent(EVENT_SCORCH, 25000);
                events.ScheduleEvent(EVENT_SLAG_POT, 35000, 1);
                events.ScheduleEvent(EVENT_CONSTRUCT, 15000);
				events.ScheduleEvent(EVENT_END_POT, 40000);
                events.ScheduleEvent(EVENT_BERSERK, 900000);

                SlagPotGUID = 0;
                ConstructTimer = 0;

                Shattered = false;

				instance->DoStartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, ACHIEVEMENT_TIMED_START_EVENT);

				me->SetPowerType(POWER_RAGE);
				me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
            }

			void KilledUnit(Unit* victim)
            {
				if (victim->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
            }

            void JustDied(Unit* /*killer*/)
            {
                _JustDied();

                DoSendQuantumText(SAY_DEATH, me);
            }

            void UpdateAI(uint32 const diff)
            {
                if (!UpdateVictim())
                    return;

                events.Update(diff);

                ConstructTimer += diff;

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_JET:
							DoSendQuantumText(EMOTE_JETS, me);
                            DoCastAOE(RAID_MODE(SPELL_FLAME_JETS_10, SPELL_FLAME_JETS_25));
                            events.DelayEvents(5000, 1);
                            events.ScheduleEvent(EVENT_JET, urand(35000, 40000));
                            break;
                        case EVENT_SLAG_POT:
                            if (Unit* target = SelectTarget(TARGET_RANDOM, 1, 100, true))
                            {
                                DoSendQuantumText(SAY_SLAG_POT, me);
                                SlagPotGUID = target->GetGUID();
                                DoCast(target, SPELL_GRAB);
                                events.ScheduleEvent(EVENT_GRAB_POT, 500);
                            }
                            events.ScheduleEvent(EVENT_SLAG_POT, RAID_MODE(30000, 15000), 1);
                            break;
                        case EVENT_GRAB_POT:
                            if (Unit* slagPotTarget = Unit::GetUnit(*me, SlagPotGUID))
                            {
								slagPotTarget->EnterVehicle(me, 1);
                                events.ScheduleEvent(EVENT_CHANGE_POT, 1000);
                            }
                            break;
                        case EVENT_CHANGE_POT:
                            if (Unit* SlagPotTarget = Unit::GetUnit(*me, SlagPotGUID))
                            {
                                SlagPotTarget->AddAura(RAID_MODE(SPELL_SLAG_POT_10, SPELL_SLAG_POT_25), SlagPotTarget);
								SlagPotTarget->EnterVehicle(me, 1);
								events.CancelEvent(EVENT_CHANGE_POT);
								events.ScheduleEvent(EVENT_END_POT, 10000);
                            }
                            break;
						case EVENT_END_POT:
							if (Unit* SlagPotTarget = Unit::GetUnit(*me, SlagPotGUID))
							{
								SlagPotTarget->ExitVehicle();
								SlagPotTarget = NULL;
								SlagPotGUID = 0;
								events.CancelEvent(EVENT_END_POT);
							}
							break;
                        case EVENT_SCORCH:
                            DoSendQuantumText(RAND(SAY_SCORCH_1, SAY_SCORCH_2), me);
                            if (Unit* target = me->GetVictim())
							{
                                me->SummonCreature(NPC_GROUND_SCORCH, *target, TEMPSUMMON_TIMED_DESPAWN, 45000);
							}
                            DoCast(RAID_MODE(SPELL_SCORCH_10, SPELL_SCORCH_25));
                            events.ScheduleEvent(EVENT_SCORCH, 25000);
                            break;
                        case EVENT_CONSTRUCT:
                        {
                            DoSendQuantumText(SAY_SUMMON, me);
                            if (!creatureList.empty())
                            {
                                std::list<Creature*>::iterator itr = creatureList.begin();
                                std::advance(itr, urand(0, creatureList.size() - 1));
                                if (Creature* construct = (*itr))
                                {
                                    construct->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);
                                    construct->SetReactState(REACT_AGGRESSIVE);
                                    construct->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_STUNNED | UNIT_FLAG_DISABLE_MOVE);
                                    construct->AI()->AttackStart(me->GetVictim());
                                    construct->AI()->DoZoneInCombat();
                                    DoCast(me, SPELL_STRENGTH, true);
                                    creatureList.erase(itr);
                                }
                            }
                            DoCast(SPELL_ACTIVATE_CONSTRUCT);
                            events.ScheduleEvent(EVENT_CONSTRUCT, RAID_MODE(40000, 30000));
                            break;
                        }
                        case EVENT_BERSERK:
                            DoCast(me, SPELL_BERSERK, true);
                            DoSendQuantumText(SAY_BERSERK, me);
                            break;
                    }
                }

                DoMeleeAttackIfReady();

                EnterEvadeIfOutOfCombatArea(diff);
            }

            uint32 GetData(uint32 type)
            {
                if (type == DATA_SHATTERED)
                    return Shattered ? 1 : 0;

                return 0;
            }

            void DoAction(int32 const action)
            {
                switch (action)
                {
                    case ACTION_REMOVE_BUFF:
                        me->RemoveAuraFromStack(SPELL_STRENGTH);

                        if (ConstructTimer >= 5000)
                            ConstructTimer = 0;
                        else
                            Shattered = true;
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new boss_ignis_the_furnace_master_AI(creature);
        }
};

class npc_iron_construct : public CreatureScript
{
    public:
        npc_iron_construct() : CreatureScript("npc_iron_construct") { }

        struct npc_iron_constructAI : public QuantumBasicAI
        {
            npc_iron_constructAI(Creature* creature) : QuantumBasicAI(creature)
            {
                instance = creature->GetInstanceScript();
                creature->SetActive(true);
                creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_STUNNED | UNIT_FLAG_DISABLE_MOVE);
                DoCast(me, SPELL_FREEZE_ANIM, true);
            }

			InstanceScript* instance;
            bool Brittled;

            void Reset()
            {
                me->SetReactState(REACT_PASSIVE);
                Brittled = false;
            }

            void JustReachedHome()
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_STUNNED | UNIT_FLAG_DISABLE_MOVE);
                DoCast(me, SPELL_FREEZE_ANIM, true);
            }

            void DamageTaken(Unit* /*attacker*/, uint32& damage)
            {
                if (me->HasAura(RAID_MODE<uint32>(SPELL_BRITTLE_10, SPELL_BRITTLE_25)) && damage >= 5000)
                {
                    DoCast(me, SPELL_SHATTER, true);
                    if (Creature* ignis = me->GetCreature(*me, instance->GetData64(DATA_IGNIS_FURNACE_MASTER)))
					{
                        if (ignis->AI())
                            ignis->AI()->DoAction(ACTION_REMOVE_BUFF);
					}

                    me->DespawnAfterAction(1*IN_MILLISECONDS);
                }
            }

            void UpdateAI(uint32 const /*diff*/)
            {
                if (!UpdateVictim())
                    return;

                if (Aura* aur = me->GetAura(SPELL_HEAT))
                {
                    if (aur->GetStackAmount() >= 10)
                    {
                        me->RemoveAura(SPELL_HEAT);
                        DoCast(SPELL_MOLTEN);
                        Brittled = false;
                    }
                }

                // Water pools
                if (me->IsInWater() && !Brittled && me->HasAura(SPELL_MOLTEN))
                {
                    DoCast(RAID_MODE(SPELL_BRITTLE_10, SPELL_BRITTLE_25));
                    me->RemoveAura(SPELL_MOLTEN);
                    Brittled = true;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_iron_constructAI(creature);
        }
};

class npc_scorch_ground : public CreatureScript
{
public:
	npc_scorch_ground() : CreatureScript("npc_scorch_ground") { }

	struct npc_scorch_groundAI : public QuantumBasicAI
	{
		npc_scorch_groundAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
			me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
			creature->SetDisplayId(16925);
		}

		void Reset()
		{
			DoCast(me, RAID_MODE(SPELL_SCORCH_GROUND_10, SPELL_SCORCH_GROUND_25));
		}

		void UpdateAI(uint32 const /*diff*/) { }
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_scorch_groundAI(creature);
	}
};

class spell_ignis_slag_pot_10 : public SpellScriptLoader
{
    public:
        spell_ignis_slag_pot_10() : SpellScriptLoader("spell_ignis_slag_pot_10") { }

        class spell_ignis_slag_pot_10_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ignis_slag_pot_10_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SLAG_POT_DAMAGE_10) || !sSpellMgr->GetSpellInfo(SPELL_SLAG_IMBUED_10))
                    return false;
                return true;
            }

            void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (Unit* caster = GetCaster())
                {
                    Unit* target = GetTarget();
                    caster->CastSpell(target, SPELL_SLAG_POT_DAMAGE_10, true);
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget()->IsAlive())
                    GetTarget()->CastSpell(GetTarget(), SPELL_SLAG_IMBUED_10, true);
            }

            void Register()
			{
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_ignis_slag_pot_10_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_ignis_slag_pot_10_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_ignis_slag_pot_10_AuraScript();
        }
};

class spell_ignis_slag_pot_25 : public SpellScriptLoader
{
    public:
        spell_ignis_slag_pot_25() : SpellScriptLoader("spell_ignis_slag_pot_25") { }

        class spell_ignis_slag_pot_25_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ignis_slag_pot_25_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_SLAG_POT_DAMAGE_25) || !sSpellMgr->GetSpellInfo(SPELL_SLAG_IMBUED_25))
                    return false;
                return true;
            }

            void HandleEffectPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (Unit* caster = GetCaster())
                {
                    Unit* target = GetTarget();
                    caster->CastSpell(target, SPELL_SLAG_POT_DAMAGE_25, true);
                }
            }

            void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (GetTarget()->IsAlive())
                    GetTarget()->CastSpell(GetTarget(), SPELL_SLAG_IMBUED_25, true);
            }

            void Register()
			{
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_ignis_slag_pot_25_AuraScript::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
                AfterEffectRemove += AuraEffectRemoveFn(spell_ignis_slag_pot_25_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_ignis_slag_pot_25_AuraScript();
        }
};

class spell_ignis_flame_jets : public SpellScriptLoader
{
    public:
        spell_ignis_flame_jets() : SpellScriptLoader("spell_ignis_flame_jets") { }

        class spell_ignis_flame_jets_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ignis_flame_jets_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if (PlayerOrPetCheck());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_ignis_flame_jets_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_ignis_flame_jets_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ENEMY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_ignis_flame_jets_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_SRC_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_ignis_flame_jets_SpellScript();
        }
};

class spell_ignis_activate_construct : public SpellScriptLoader
{
    public:
        spell_ignis_activate_construct() : SpellScriptLoader("spell_ignis_activate_construct") {}

        class spell_ignis_activate_construct_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_ignis_activate_construct_SpellScript);

			void FilterTargets(std::list<Unit*>& unitList)
            {
                if (Unit* target = Quantum::DataPackets::SelectRandomContainerElement(unitList))
                {
                    unitList.clear();
                    unitList.push_back(target);
                }
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_ignis_activate_construct_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_ignis_activate_construct_SpellScript();
        }
};

class achievement_ignis_shattered : public AchievementCriteriaScript
{
public:
	achievement_ignis_shattered() : AchievementCriteriaScript("achievement_ignis_shattered") { }

	bool OnCheck(Player* /*source*/, Unit* target)
	{
		if (target && target->IsAIEnabled)
			return target->GetAI()->GetData(DATA_SHATTERED);

		return false;
	}
};

void AddSC_boss_ignis_the_furnace_master()
{
    new boss_ignis_the_furnace_master();
    new npc_iron_construct();
    new npc_scorch_ground();
    new spell_ignis_slag_pot_10();
	new spell_ignis_slag_pot_25();
    new spell_ignis_flame_jets();
	new spell_ignis_activate_construct();
    new achievement_ignis_shattered();
}