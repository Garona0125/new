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

enum Texts
{
    SAY_STAL_AGGRO          = -1533023,
    SAY_STAL_SLAY           = -1533024,
    SAY_STAL_DEATH          = -1533025,
    SAY_FEUG_AGGRO          = -1533026,
    SAY_FEUG_SLAY           = -1533027,
    SAY_FEUG_DEATH          = -1533028,
	SAY_GREET               = -1533029,
    SAY_AGGRO_1             = -1533030,
    SAY_AGGRO_2             = -1533031,
    SAY_AGGRO_3             = -1533032,
    SAY_SLAY                = -1533033,
    SAY_ELECT               = -1533034,
    SAY_DEATH               = -1533035,
    SAY_SCREAM1             = -1533036, //not used
    SAY_SCREAM2             = -1533037, //not used
    SAY_SCREAM3             = -1533038, //not used
    SAY_SCREAM4             = -1533039, //not used
};

enum Spells
{
	SPELL_THADIUS_SPAWN         = 28160,
	SPELL_STALAGG_TESLA         = 28097,
	SPELL_FEUGEN_TESLA          = 28109,
	SPELL_FEUGEN_CHAIN          = 28111,
	SPELL_STALAG_CHAIN          = 28096,
	SPELL_POWERSURGE_10         = 28134,
    SPELL_POWERSURGE_25         = 54529,
    SPELL_MAGNETIC_PULL         = 28338,
	SPELL_STATICFIELD_10        = 28135,
    SPELL_STATICFIELD_25        = 54528,
    SPELL_POLARITY_SHIFT        = 28089,
    SPELL_POSITIVE_CHARGE       = 28059,
    SPELL_NEGATIVE_CHARGE       = 28084,
    SPELL_BALL_LIGHTNING        = 28299,
    SPELL_CHAIN_LIGHTNING       = 28167,
    H_SPELL_CHAIN_LIGHTNING     = 54531,
    SPELL_BERSERK               = 27680,
    SPELL_POSITIVE_CHARGE_AURA  = 28062,
    SPELL_POSITIVE_CHARGE_STACK = 29659,
    SPELL_NEGATIVE_CHARGE_AURA  = 28085,
    SPELL_NEGATIVE_CHARGE_STACK = 29660,
};

enum ThaddiusActions
{
    ACTION_FEUGEN_RESET  = 1,
    ACTION_FEUGEN_DIED   = 2,
    ACTION_STALAGG_RESET = 3,
    ACTION_STALAGG_DIED  = 4,
};

enum Events
{
    EVENT_NONE            = 0,
    EVENT_SHIFT           = 1,
    EVENT_CHAIN_LIGHTNING = 2,
    EVENT_BERSERK         = 3,
};

enum Achievement
{
    DATA_SHOCKING = 76047605
};

#define EMOTE_SHIFT      "The polarity has shifted!"
#define EMOTE_TESLA_COIL "Tesla Coil overloads!"
#define EMOTE_OVERLOADS  "Thaddius overloads!"

class boss_thaddius : public CreatureScript
{
public:
    boss_thaddius() : CreatureScript("boss_thaddius") { }

    struct boss_thaddiusAI : public BossAI
    {
        boss_thaddiusAI(Creature* creature) : BossAI(creature, BOSS_THADDIUS)
        {
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_THADIUS_SPAWN);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            CheckFeugenAlive = false;

            if (Creature* Feugen = me->GetCreature(*me, instance ? instance->GetData64(DATA_FEUGEN) : 0))
                CheckFeugenAlive = Feugen->IsAlive();

            CheckStalaggAlive = false;

            if (Creature* Stalagg = me->GetCreature(*me, instance ? instance->GetData64(DATA_STALAGG) : 0))
                CheckStalaggAlive = Stalagg->IsAlive();

            if (!CheckFeugenAlive && !CheckStalaggAlive)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
				me->RemoveAurasDueToSpell(SPELL_THADIUS_SPAWN);
                me->SetReactState(REACT_AGGRESSIVE);
            }
            else
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
				DoCast(me, SPELL_THADIUS_SPAWN);
                me->SetReactState(REACT_PASSIVE);
            }
        }

        bool CheckStalaggAlive;
        bool CheckFeugenAlive;
        bool Shocking;
		bool HasTaunted;
		bool TeslaStalag;
		bool TeslaFeugen;

        uint32 AddsTimer;

        void Reset()
        {
            _Reset();

            Shocking = true;

            if (Creature* Feugen = me->GetCreature(*me, instance ? instance->GetData64(DATA_FEUGEN) : 0))
            {
                Feugen->Respawn(true);
                CheckFeugenAlive = Feugen->IsAlive();
            }

            if (Creature* Stalagg = me->GetCreature(*me, instance ? instance->GetData64(DATA_STALAGG) : 0))
            {
                Stalagg->Respawn(true);
                CheckStalaggAlive = Stalagg->IsAlive();
            }

            if (!CheckFeugenAlive && !CheckStalaggAlive)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
				me->RemoveAurasDueToSpell(SPELL_THADIUS_SPAWN);
                me->SetReactState(REACT_AGGRESSIVE);
            }
            else
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
				DoCast(me, SPELL_THADIUS_SPAWN);
                me->SetReactState(REACT_PASSIVE);
            }
        }

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(SAY_SLAY, me);
        }

        void JustDied(Unit* /*killer*/)
        {
            _JustDied();

            DoSendQuantumText(SAY_DEATH, me);

			if (Creature* kel = me->FindCreatureWithDistance(NPC_MB_KEL_THUZAD, 500.0f))
				DoSendQuantumText(SAY_KELTHUZAD_TAUNT_3, kel);

			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_POSITIVE_CHARGE);
			instance->DoRemoveAurasDueToSpellOnPlayers(SPELL_NEGATIVE_CHARGE);
        }

        void DoAction(int32 const action)
        {
            switch (action)
            {
                case ACTION_FEUGEN_RESET:
                    CheckFeugenAlive = true;
                    break;
                case ACTION_FEUGEN_DIED:
                    CheckFeugenAlive = false;
                    break;
                case ACTION_STALAGG_RESET:
                    CheckStalaggAlive = true;
                    break;
                case ACTION_STALAGG_DIED:
                    CheckStalaggAlive = false;
                    break;
            }

            if (!CheckFeugenAlive && !CheckStalaggAlive)
            {
                me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
				me->RemoveAurasDueToSpell(SPELL_THADIUS_SPAWN);
                DoZoneInCombat();
            }
            else
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NO_AGGRO_FOR_PLAYER);
				DoCast(me, SPELL_THADIUS_SPAWN);
                me->SetReactState(REACT_PASSIVE);
            }
        }

        void EnterToBattle(Unit* /*who*/)
        {
            _EnterToBattle();

			me->MonsterTextEmote(EMOTE_OVERLOADS, LANG_UNIVERSAL, true);
            DoSendQuantumText(RAND(SAY_AGGRO_1, SAY_AGGRO_2, SAY_AGGRO_3), me);

            events.ScheduleEvent(EVENT_SHIFT, 30000);
            events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(10000, 20000));
            events.ScheduleEvent(EVENT_BERSERK, 360000);
        }

		void MoveInLineOfSight(Unit* who)
		{
			if (!me->IsWithinDistInMap(who, 1000.0f) && who->GetTypeId() == TYPE_ID_PLAYER)
			{
				DoSendQuantumText(SAY_GREET, me);
				HasTaunted = true;
			}

			QuantumBasicAI::MoveInLineOfSight(who);
		}

        void DamageTaken(Unit* /*attacker*/, uint32 & /*damage*/)
        {
            me->SetReactState(REACT_AGGRESSIVE);
        }

        void SetData(uint32 id, uint32 data)
        {
            if (id == DATA_SHOCKING)
                Shocking = false;
        }

        uint32 GetData(uint32 id)
        {
            if (id != DATA_SHOCKING)
                return 0;

            return uint32(Shocking);
        }

        void UpdateAI(uint32 const diff)
        {
            if (CheckFeugenAlive && CheckStalaggAlive)
                AddsTimer = 0;

            if (CheckStalaggAlive != CheckFeugenAlive)
            {
                AddsTimer += diff;
                if (AddsTimer > 5000)
                {
                    if (!CheckStalaggAlive)
                    {
                        if (Creature* Stalagg = me->GetCreature(*me, instance ? instance->GetData64(DATA_STALAGG) : 0))
                            Stalagg->Respawn();
                    }
                    else
                    {
                        if (Creature* Feugen = me->GetCreature(*me, instance ? instance->GetData64(DATA_FEUGEN) : 0))
                            Feugen->Respawn();
                    }
                }
            }

            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_SHIFT:
						me->MonsterTextEmote(EMOTE_SHIFT, 0, true);
						DoSendQuantumText(SAY_ELECT, me);
                        DoCastAOE(SPELL_POLARITY_SHIFT);
                        events.ScheduleEvent(EVENT_SHIFT, 30000);
                        events.RescheduleEvent(EVENT_CHAIN_LIGHTNING, 6000);
                        return;
                    case EVENT_CHAIN_LIGHTNING:
                        DoCastVictim(RAID_MODE(SPELL_CHAIN_LIGHTNING, H_SPELL_CHAIN_LIGHTNING));
                        events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, urand(10000, 20000));
                        return;
                    case EVENT_BERSERK:
                        me->InterruptNonMeleeSpells(false);
                        DoCast(me, SPELL_BERSERK);
                        return;
                }
            }

            if (events.GetTimer() > 15000 && !SelectTarget(TARGET_RANDOM, 0, me->GetMeleeReach(), true))
                DoCastVictim(SPELL_BALL_LIGHTNING);
            else
                DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_thaddiusAI(creature);
    }
};

class mob_stalagg : public CreatureScript
{
public:
    mob_stalagg() : CreatureScript("mob_stalagg") { }

    struct mob_stalaggAI : public QuantumBasicAI
    {
        mob_stalaggAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 PowerSurgeTimer;
        uint32 MagneticPullTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			StalagTeslaActivate();
			StalagTeslaSpawn();

            if (Creature* Thaddius = me->GetCreature(*me, instance ? instance->GetData64(DATA_THADDIUS) : 0))
			{
                if (Thaddius->AI())
                    Thaddius->AI()->DoAction(ACTION_STALAGG_RESET);
			}
            PowerSurgeTimer = urand(20000, 25000);
            MagneticPullTimer = 20000;
        }

		void StalagTeslaActivate()
		{
			if (GameObject* StalagTesla = me->FindGameobjectWithDistance(GO_TESLA_STALAG, 100.0f))
			{
				StalagTesla->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
				StalagTesla->SetGoState(GO_STATE_ACTIVE);
			}
		}

		void StalagTeslaSpawn()
		{
			me->SummonCreature(NPC_TESLA_COIL, 3487.59f, -2911.08f, 319.419f, 3.85711f, TEMPSUMMON_MANUAL_DESPAWN);
		}

		void DespawnStalagTesla()
		{
			if (GameObject* StalagTesla = me->FindGameobjectWithDistance(GO_TESLA_STALAG, 100.0f))
			{
				StalagTesla->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
				StalagTesla->SetGoState(GO_STATE_READY);
			}

			if (Creature* TeslaCoil = me->FindCreatureWithDistance(NPC_TESLA_COIL, 100.0f))
				TeslaCoil->DespawnAfterAction();
		}

        void EnterToBattle(Unit* /*who*/)
        {
			DoSendQuantumText(SAY_STAL_AGGRO, me);
            DoCast(SPELL_STALAGG_TESLA);
        }

		void JustReachedHome()
        {
			StalagTeslaActivate();
			StalagTeslaSpawn();
			me->MonsterTextEmote(EMOTE_TESLA_COIL, LANG_UNIVERSAL, true);
        }

        void JustDied(Unit* /*killer*/)
        {
			DespawnStalagTesla();

            if (Creature* Thaddius = me->GetCreature(*me, instance ? instance->GetData64(DATA_THADDIUS) : 0))
			{
                if (Thaddius->AI())
                    Thaddius->AI()->DoAction(ACTION_STALAGG_DIED);
			}

			DoSendQuantumText(SAY_STAL_DEATH, me);
        }

		void KilledUnit(Unit* victim)
		{
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(SAY_STAL_SLAY, me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            if (MagneticPullTimer <= diff)
            {
                if (Creature* Feugen = me->GetCreature(*me, instance ? instance->GetData64(DATA_FEUGEN) : 0))
                {
                    Unit* StalaggVictim = me->GetVictim();
                    Unit* FeugenVictim = Feugen->GetVictim();

                    if (FeugenVictim && StalaggVictim)
                    {
                        Feugen->getThreatManager().addThreat(StalaggVictim, Feugen->getThreatManager().getThreat(FeugenVictim));
                        me->getThreatManager().addThreat(FeugenVictim, me->getThreatManager().getThreat(StalaggVictim));
                        Feugen->getThreatManager().modifyThreatPercent(FeugenVictim, -100);
                        me->getThreatManager().modifyThreatPercent(StalaggVictim, -100);
                        FeugenVictim->JumpTo(me, 0.3f);
                        StalaggVictim->JumpTo(Feugen, 0.3f);
                        me->GetMotionMaster()->MoveDistract(2500);
                        Feugen->GetMotionMaster()->MoveDistract(2500);
                    }
                }

                MagneticPullTimer = 20000;
            }
            else
                MagneticPullTimer -= diff;

            if (PowerSurgeTimer <= diff)
            {
                DoCast(me, RAID_MODE(SPELL_POWERSURGE_10, SPELL_POWERSURGE_25));
                PowerSurgeTimer = urand(15000, 20000);
            }
            else PowerSurgeTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_stalaggAI(creature);
    }
};

class mob_feugen : public CreatureScript
{
public:
    mob_feugen() : CreatureScript("mob_feugen") { }

    struct mob_feugenAI : public QuantumBasicAI
    {
        mob_feugenAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        uint32 StaticFieldTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			FeugenTeslaActivate();
			FeugenTeslaSpawn();

            if (Creature* Thaddius = me->GetCreature(*me, instance ? instance->GetData64(DATA_THADDIUS) : 0))
			{
                if (Thaddius->AI())
                    Thaddius->AI()->DoAction(ACTION_FEUGEN_RESET);
			}
            StaticFieldTimer = 5000;
        }

		void FeugenTeslaActivate()
		{
			if (GameObject* FeugenTesla = me->FindGameobjectWithDistance(GO_TESLA_FEUGEN, 100.0f))
			{
				FeugenTesla->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
				FeugenTesla->SetGoState(GO_STATE_ACTIVE);
			}
		}

		void FeugenTeslaSpawn()
		{
			me->SummonCreature(NPC_TESLA_COIL_1, 3527.76f, -2952.59f, 319.22f, 3.97359f, TEMPSUMMON_MANUAL_DESPAWN);
		}

		void DespawnFeugenTesla()
		{
			if (GameObject* FeugenTesla = me->FindGameobjectWithDistance(GO_TESLA_FEUGEN, 100.0f))
			{
				FeugenTesla->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND);
				FeugenTesla->SetGoState(GO_STATE_READY);
			}

			if (Creature* TeslaCoil1 = me->FindCreatureWithDistance(NPC_TESLA_COIL_1, 100.0f))
				TeslaCoil1->DespawnAfterAction();
		}

		void JustReachedHome()
        {
			FeugenTeslaActivate();
			FeugenTeslaSpawn();
			me->MonsterTextEmote(EMOTE_TESLA_COIL, LANG_UNIVERSAL, true);
        }

        void EnterToBattle(Unit* /*who*/)
        {
			DoSendQuantumText(SAY_FEUG_AGGRO, me);
            DoCast(SPELL_FEUGEN_TESLA);
        }

        void JustDied(Unit* /*killer*/)
        {
            if (Creature* Thaddius = me->GetCreature(*me, instance ? instance->GetData64(DATA_THADDIUS) : 0))
			{
                if (Thaddius->AI())
                    Thaddius->AI()->DoAction(ACTION_FEUGEN_DIED);
			}

			DoSendQuantumText(SAY_FEUG_DEATH, me);

			DespawnFeugenTesla();
        }

		void KilledUnit(Unit* victim)
		{
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(SAY_FEUG_SLAY, me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

            if (StaticFieldTimer <= diff)
            {
                DoCast(me, RAID_MODE(SPELL_STATICFIELD_10, SPELL_STATICFIELD_25));
                StaticFieldTimer = 5000;
            }
            else StaticFieldTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new mob_feugenAI(creature);
    }
};

class spell_thaddius_pos_neg_charge : public SpellScriptLoader
{
    public:
        spell_thaddius_pos_neg_charge() : SpellScriptLoader("spell_thaddius_pos_neg_charge") { }

        class spell_thaddius_pos_neg_charge_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_thaddius_pos_neg_charge_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_POSITIVE_CHARGE_AURA))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_POSITIVE_CHARGE_STACK))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_POSITIVE_CHARGE_AURA))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_NEGATIVE_CHARGE_STACK))
                    return false;
                return true;
            }

            bool Load()
            {
                return GetOriginalCaster()->GetTypeId() == TYPE_ID_UNIT;
            }

            void HandleTargets(std::list<Unit*>& targetList)
            {
                uint8 count = 0;
                for (std::list<Unit*>::iterator ihit = targetList.begin(); ihit != targetList.end(); ++ihit)
				{
                    if ((*ihit)->GetGUID() != GetCaster()->GetGUID())
					{
                        if (Player* target = (*ihit)->ToPlayer())
						{
                            if (target->HasAura(GetTriggeringSpell()->Id))
                                ++count;
						}
					}
				}

                if (count)
                {
                    uint32 spellId = 0;

                    if (GetSpellInfo()->Id == SPELL_POSITIVE_CHARGE_AURA)
                        spellId = SPELL_POSITIVE_CHARGE_STACK;
                    else // if (GetSpellInfo()->Id == SPELL_NEGATIVE_CHARGE)
                        spellId = SPELL_NEGATIVE_CHARGE_STACK;

                    GetCaster()->SetAuraStack(spellId, GetCaster(), count);
                }
            }

            void HandleDamage(SpellEffIndex /*effIndex*/)
            {
                if (!GetTriggeringSpell())
                    return;

                Unit* target = GetHitUnit();
                Unit* caster = GetOriginalCaster();

                if (target->HasAura(GetTriggeringSpell()->Id))
                    SetHitDamage(0);
                else
                {
                    if (target->GetTypeId() == TYPE_ID_PLAYER && caster->IsAIEnabled)
                        caster->ToCreature()->AI()->SetData(DATA_SHOCKING, 0);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_thaddius_pos_neg_charge_SpellScript::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_thaddius_pos_neg_charge_SpellScript::HandleTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_thaddius_pos_neg_charge_SpellScript();
        }
};

class achievement_shocking : public AchievementCriteriaScript
{
public:
	achievement_shocking() : AchievementCriteriaScript("achievement_shocking") { }

	bool OnCheck(Player* /*source*/, Unit* target)
	{
		return target && target->GetAI()->GetData(DATA_SHOCKING);
	}
};

class npc_tesla_stalagg_chain : public CreatureScript
{
public:
    npc_tesla_stalagg_chain() : CreatureScript("npc_tesla_stalagg_chain") { }

    struct npc_tesla_stalagg_chainAI : public QuantumBasicAI
    {
        npc_tesla_stalagg_chainAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        void Reset()
        {
			if (Creature* StalagTarget = me->GetCreature(*me, instance ? instance->GetData64(DATA_STALAGG) : 0))
				DoCast(StalagTarget, SPELL_STALAG_CHAIN);
        }

		void JustReachedHome()
        {
			Reset();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tesla_stalagg_chainAI(creature);
    }
};

class npc_tesla_feugen_chain : public CreatureScript
{
public:
    npc_tesla_feugen_chain() : CreatureScript("npc_tesla_feugen_chain") { }

    struct npc_tesla_feugen_chainAI : public QuantumBasicAI
    {
        npc_tesla_feugen_chainAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
        }

        InstanceScript* instance;

        void Reset()
        {
			if (Creature* FeugenTarget = me->GetCreature(*me, instance ? instance->GetData64(DATA_FEUGEN) : 0))
				DoCast(FeugenTarget, SPELL_FEUGEN_CHAIN);
        }

		void JustReachedHome()
        {
			Reset();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tesla_feugen_chainAI(creature);
    }
};

void AddSC_boss_thaddius()
{
    new boss_thaddius();
    new mob_stalagg();
    new mob_feugen();
    new spell_thaddius_pos_neg_charge();
    new achievement_shocking();
	new npc_tesla_stalagg_chain();
	new npc_tesla_feugen_chain();
}