/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

#include "ScriptMgr.h"
#include "ObjectMgr.h"
#include "QuantumCreature.h"
#include "SpellAuraEffects.h"
#include "ruby_sanctum.h"

enum Texts
{
	SAY_INTRO         = -1666305,
	SAY_AGGRO         = -1666300,
	SAY_SLAY_1        = -1666301,
	SAY_SLAY_2        = -1666302,
	SAY_DEATH         = -1666303,
	SAY_CLONE         = -1666304,
};

enum Spells
{
    SPELL_BLADE_TEMPEST            = 75125,
    SPELL_ENERVATING_BRAND         = 74502,
    SPELL_ENERVATING_BRAND_TRIGGER = 74505,
    SPELL_SIPHONED_MIGHT           = 74507,
    SPELL_REPELLING_WAVE           = 74509,
    SPELL_CLEAVE               = 40504,
    SPELL_CLEAR_DEBUFFS            = 34098,
    SPELL_SUMMON_CLONE             = 74511,
    SPELL_SUMMON_CLONE_VISUAL      = 64195,
    SPELL_BARRIER_CHANNEL          = 76221,
};

enum Equipment
{
    EQUIP_MAIN    = 28365,
    EQUIP_OFFHAND = EQUIP_NO_CHANGE,
    EQUIP_RANGED  = EQUIP_NO_CHANGE,
    EQUIP_DONE    = EQUIP_NO_CHANGE,
};

struct Locations
{
    float x, y, z;
};

static Locations SpawnLoc[] =
{
    {3152.329834f, 359.41757f, 85.301605f}, // Baltharus target point
    {3153.06f, 389.486f, 86.2596f}, // Baltharus initial point
};

class boss_baltharus_the_warborn : public CreatureScript
{
public:
    boss_baltharus_the_warborn() : CreatureScript("boss_baltharus_the_warborn") { }

    struct boss_baltharus_the_warbornAI : public QuantumBasicAI
    {
        boss_baltharus_the_warbornAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();

			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        }

        InstanceScript* instance;

        Creature* Clone;

        bool Intro;

        uint8 Stage;

		uint32 ChannelTimer;
		uint32 SiphonedTimer;
		uint32 BladeTempestTimer;
		uint32 CleaveTimer;
		uint32 EnevatingBrandTimer;

        void Reset()
        {
            if (!instance)
                return;

            if (me->IsAlive())
				instance->SetData(TYPE_BALTHARUS, NOT_STARTED);

			me->SetRespawnDelay(7*DAY);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

            Stage = 0;
            Clone = NULL;
			Intro = false;

			ChannelTimer = 0.5*IN_MILLISECONDS;
			SiphonedTimer = 2*IN_MILLISECONDS;
			BladeTempestTimer = 22*IN_MILLISECONDS;
			CleaveTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
            EnevatingBrandTimer = urand(25*IN_MILLISECONDS, 30*IN_MILLISECONDS);
        }

		void MoveInLineOfSight(Unit* who)
        {
            if (!instance || Intro || who->GetTypeId() != TYPE_ID_PLAYER || !who->IsWithinDistInMap(me, 60.0f))
				return;

			if (instance && Intro == false && who->GetTypeId() == TYPE_ID_PLAYER && who->IsWithinDistInMap(me, 60.0f))
			{
				instance->SetData(TYPE_EVENT,  10);
				DoSendQuantumText(SAY_INTRO, me);
				Intro = true;
			}
		}

        void JustReachedHome()
        {
            if (!instance)
				return;

            instance->SetData(TYPE_BALTHARUS, FAIL);
			Reset();
        }

		void EnterToBattle(Unit* who)
        {
            if (!instance)
				return;

            if (who->GetTypeId() != TYPE_ID_PLAYER)
				return;

            SetEquipmentSlots(false, EQUIP_MAIN, EQUIP_OFFHAND, EQUIP_RANGED);

            me->InterruptNonMeleeSpells(true);
            SetCombatMovement(true);
            instance->SetData(TYPE_BALTHARUS, IN_PROGRESS);
            DoSendQuantumText(SAY_AGGRO, me);
        }

        void JustDied(Unit* /*killer*/)
        {
            if (!instance)
				return;

            DoSendQuantumText(SAY_DEATH, me);

            instance->SetData(TYPE_BALTHARUS, DONE);
        }

        void KilledUnit(Unit* who)
        {
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
        }

        void JustSummoned(Creature* summoned)
        {
            if (!instance || !summoned)
				return;

            if (summoned->GetEntry() != NPC_BALTHARUS_TARGET)
            {
                 if (!Clone)
					 Clone = summoned;
                 else
					 if (!Clone->IsAlive())
						 Clone = summoned;
                 Clone->SetInCombatWithZone();
				 Clone->CastSpell(Clone, SPELL_SUMMON_CLONE_VISUAL, true);
				 Clone = NULL;
            }
        }

        void SummonedCreatureJustDied(Creature* summoned)
        {
             if (!instance || !summoned)
				 return;

             if (summoned == Clone)
				 Clone = NULL;
        }

        void DamageTaken(Unit* doneBy, uint32 /*&damage*/)
        {
            if (!instance)
				return;

            if (!me || !me->IsAlive())
                return;

			if (doneBy->GetGUID() == me->GetGUID()) 
				return;
        }

        void UpdateAI(const uint32 diff)
        {
			if (!instance)
				return;

			// Out Of Combat Timer
			if (ChannelTimer <= diff && !me->IsInCombatActive())
			{
				if (Creature* bunny = me->FindCreatureWithDistance(NPC_BALTHARUS_TARGET, 55.0f, true))
				{
					me->SetFacingToObject(bunny);
					DoCast(bunny, SPELL_BARRIER_CHANNEL);

					if (Creature* xerestrasza = me->GetMap()->GetCreature(instance->GetData64(NPC_XERESTRASZA)))
						me->SetUInt64Value(UNIT_FIELD_TARGET, xerestrasza->GetGUID());

					ChannelTimer = 1*MINUTE*IN_MILLISECONDS;
				}
			}
			else ChannelTimer -= diff;			

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			switch (Stage)
            {
                case 0:
                     if (HealthBelowPct(67))
						 Stage = 1;
                     break;
                case 1:
                     if (GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL || GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
					 {
						 me->InterruptNonMeleeSpells(true);
                         DoCast(SPELL_REPELLING_WAVE);
					 }
                     Stage = 2;
                     break;
                case 2:
                     if (me->IsNonMeleeSpellCasted(false))
						 return;
					 if (GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL || GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
					 {
						 DoCast(me,SPELL_CLEAR_DEBUFFS);
						 DoSendQuantumText(SAY_CLONE, me, Clone);
						 DoCast(SPELL_SUMMON_CLONE);
					 }
					 Stage = 3;
                case 3:
                     if (HealthBelowPct(51))
						 Stage = 4;
                     break;
                case 4:
                     if (GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL || GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC)
					 {
						 me->InterruptNonMeleeSpells(true);
                         DoCastAOE(SPELL_REPELLING_WAVE);
					 }
                     Stage = 5;
                     break;
                case 5:
                     if (me->IsNonMeleeSpellCasted(false))
						 return;
					 if (GetDifficulty() == RAID_DIFFICULTY_10MAN_NORMAL || GetDifficulty() == RAID_DIFFICULTY_10MAN_HEROIC)
					 {
						 DoCast(me,SPELL_CLEAR_DEBUFFS);
						 DoSendQuantumText(SAY_CLONE, me, Clone);
						 DoCast(SPELL_SUMMON_CLONE);
					 }
					 Stage = 6;
                case 6:
                     if (HealthBelowPct(34))
						 Stage = 7;
                     break;
                case 7:
                     if (GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL || GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
					 {
						 me->InterruptNonMeleeSpells(true);
                         DoCast(SPELL_REPELLING_WAVE);
					 }
                     Stage = 8;
                     break;
                case 8:
                     if (me->IsNonMeleeSpellCasted(false))
						 return;
					 if (GetDifficulty() == RAID_DIFFICULTY_25MAN_NORMAL || GetDifficulty() == RAID_DIFFICULTY_25MAN_HEROIC)
					 {
						 DoCast(me,SPELL_CLEAR_DEBUFFS);
						 DoSendQuantumText(SAY_CLONE, me, Clone);
						 DoCast(SPELL_SUMMON_CLONE);
					 }
					 Stage = 9;
				case 9:
				default:
					break;
            }

		   if (BladeTempestTimer <= diff)
		   {
			   DoCast(me,SPELL_BLADE_TEMPEST);
			   BladeTempestTimer = 22*IN_MILLISECONDS;
		   }
		   else BladeTempestTimer -= diff;

            if (EnevatingBrandTimer <= diff)
            {
				for (uint8 i = 0; i < RAID_MODE<uint8>(4, 8, 6, 10); i++)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 45.0f, true))
					{
						DoCast(target, SPELL_ENERVATING_BRAND);
						EnevatingBrandTimer = urand(25*IN_MILLISECONDS,30*IN_MILLISECONDS);
					}
				}
            }
			else EnevatingBrandTimer -= diff;
			
			if (CleaveTimer <= diff)
            {
                DoCast(SPELL_CLEAVE);
                CleaveTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
            }
			else CleaveTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_baltharus_the_warbornAI(creature);
    }
};

class npc_baltharus_the_warborn_clone : public CreatureScript
{
public:
    npc_baltharus_the_warborn_clone() : CreatureScript("npc_baltharus_the_warborn_clone") { }

    struct npc_baltharus_the_warborn_cloneAI : public QuantumBasicAI
    {
        npc_baltharus_the_warborn_cloneAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
			Reset();
        }

        InstanceScript* instance;

		uint32 SiphonedTimer;
        uint32 BladeTempestTimer;
        uint32 EnevatingBrandTimer;
        uint32 CleaveTimer;

        void Reset()
        {
            if (!instance)
				return;

			SiphonedTimer = 2*IN_MILLISECONDS;
            BladeTempestTimer = 22*IN_MILLISECONDS;
            EnevatingBrandTimer = urand(10*IN_MILLISECONDS, 20*IN_MILLISECONDS);
            CleaveTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);

            me->SetRespawnDelay(7*DAY);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);
        }

        void KilledUnit(Unit* victim)
        {
			if (victim->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
		}

        void JustDied(Unit* /*killer*/)
        {
            if (!instance)
				return;

			me->DespawnAfterAction();
        }

        void EnterToBattle(Unit* /*who*/)
        {
            if (!instance)
				return;

            SetEquipmentSlots(false, EQUIP_MAIN, EQUIP_OFFHAND, EQUIP_RANGED);

            me->SetInCombatWithZone();
			{
				Creature* baltharus = me->GetMap()->GetCreature(instance->GetData64(NPC_BALTHARUS));
				me->SetHealth(baltharus->GetHealth());
			}
        }

        void UpdateAI(const uint32 diff)
        {
            if (!instance)
				return;

            if (instance->GetData(TYPE_BALTHARUS) != IN_PROGRESS)
                me->DespawnAfterAction();

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            if (BladeTempestTimer <= diff)
            {
                DoCast(me, SPELL_BLADE_TEMPEST);
                BladeTempestTimer = 22*IN_MILLISECONDS;
            }
			else BladeTempestTimer -= diff;

            if (EnevatingBrandTimer <= diff)
            {
				for (uint8 i = 0; i < RAID_MODE<uint8>(4, 8, 8, 10); i++)
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 45.0f, true))
						DoCast(target, SPELL_ENERVATING_BRAND);
				EnevatingBrandTimer = urand(25*IN_MILLISECONDS,30*IN_MILLISECONDS);
            }
			else EnevatingBrandTimer -= diff;
			
			if (CleaveTimer <= diff)
            {
                DoCast(SPELL_CLEAVE);
                CleaveTimer = urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS);
            }
			else CleaveTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_baltharus_the_warborn_cloneAI(creature);
    }
};

static Locations SpawnLocXer[] =
{
    {3155.540039f, 342.391998f, 84.596802f},   // 0 - start point
    {3152.329834f, 359.41757f, 85.301605f},    // 1 - second say
    {3152.078369f, 383.939178f, 86.337875f},   // 2 - other says and staying
    {3154.99f, 535.637f, 72.8887f},            // 3 - Halion spawn point
};

class npc_xerestrasza : public CreatureScript
{
public:
    npc_xerestrasza() : CreatureScript("npc_xerestrasza") { }

    struct npc_xerestraszaAI : public QuantumBasicAI
    {
        npc_xerestraszaAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
            Reset();
        }

        InstanceScript* instance;
        uint32 nextEvent;
        uint32 nextPoint;
        uint32 UpdateTimer;
        bool movementstarted;
        bool onSessionEvent;

        void Reset()
        {
			if (!instance)
				return;

            nextEvent = 0;
            nextPoint = 0;
            movementstarted = false;
            UpdateTimer = 2000;
            me->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
            instance->SetData(TYPE_XERESTRASZA, NOT_STARTED);
            me->AddUnitMovementFlag(MOVEMENTFLAG_WALKING);
            me->SetSpeed(MOVE_WALK, 0.8f, true);
            me->SetRespawnDelay(7*DAY);
       }

        void MovementInform(uint32 type, uint32 id)
        {
            if (type != POINT_MOTION_TYPE || !movementstarted)
				return;

            if (id == nextPoint)
            {
                movementstarted = false;
                instance->SetData(TYPE_EVENT, nextEvent);
                me->GetMotionMaster()->MovementExpired();
            }
        }

        void StartMovement(uint32 id, uint32 _nextEvent)
        {
            nextPoint = id;
            nextEvent = _nextEvent;
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MovePoint(id, SpawnLocXer[id].x, SpawnLocXer[id].y, SpawnLocXer[id].z);
            instance->SetData(TYPE_EVENT, 0);
            movementstarted = true;
        }

        void AttackStart(Unit *who)
        {
            //ignore all attackstart commands
            return;
        }

        void MoveInLineOfSight(Unit *who)
        {
            if (!instance || !who || who->GetTypeId() != TYPE_ID_PLAYER) 
                return;

            if (instance->GetData(TYPE_BALTHARUS) != DONE
                || instance->GetData(TYPE_XERESTRASZA) != NOT_STARTED)
				return;

            if (!who->IsWithinDistInMap(me, 60.0f))
				return;

            instance->SetData(TYPE_XERESTRASZA, IN_PROGRESS);
            instance->SetData(TYPE_EVENT,  30);
            onSessionEvent = true;
        }

        void UpdateAI(const uint32 diff)
        {
			if (!instance)
				return;

            if (instance->GetData(TYPE_EVENT_NPC) == NPC_XERESTRASZA)
            {
				UpdateTimer = instance->GetData(TYPE_EVENT_TIMER);
                if (UpdateTimer <= diff)
                {
                    switch (instance->GetData(TYPE_EVENT))
                    {
						// Xerestrasza Intro
                        case 10:
							UpdateTimer = 7000;
							instance->SetData(TYPE_EVENT,  20);
							break;
                        case 20:
							DoSendQuantumText(-1666000, me);
							UpdateTimer = 6000;
							instance->SetData(TYPE_EVENT,  0);
							break;
							// Xerestrasza event
						case 30:
							instance->SetData(TYPE_XERESTRASZA, DONE);
							DoSendQuantumText(-1666001, me);
							StartMovement(1, 40);
							break;
						case 40:
							DoSendQuantumText(-1666002, me);
							StartMovement(2, 50);
							break;
                        case 50:
							DoSendQuantumText(-1666003, me);
							UpdateTimer = 10000;
							instance->SetData(TYPE_EVENT, 60);
							break;
                        case 60:
							DoSendQuantumText(-1666004, me);
							UpdateTimer = 12000;
							instance->SetData(TYPE_EVENT, 70);
							break;
                        case 70:
							DoSendQuantumText(-1666005, me);
							UpdateTimer = 10000;
							instance->SetData(TYPE_EVENT, 80);
							break;
                        case 80:
							DoSendQuantumText(-1666006, me);
							UpdateTimer = 10000;
							instance->SetData(TYPE_EVENT, 90);
							break;
                        case 90:
							DoSendQuantumText(-1666007, me);
							UpdateTimer = 10000;
							instance->SetData(TYPE_EVENT, 100);
							break;
                        case 100:
							DoSendQuantumText(-1666008, me);
							UpdateTimer = 7000;
							instance->SetData(TYPE_EVENT, 110);
							break;
                        case 110:
							UpdateTimer = 2000;
							instance->SetData(TYPE_EVENT, 0);
							me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUEST_GIVER);
							me->SetActive(false);
							break;
							// Halion spawn
						case 200:
							{
								Creature* halion = me->GetMap()->GetCreature(instance->GetData64(NPC_HALION_REAL));
								if (instance->GetData(TYPE_BALTHARUS) == DONE && instance->GetData(TYPE_RAGEFIRE) == DONE && instance->GetData(TYPE_XERESTRASZA) == DONE && instance->GetData(TYPE_ZARITHRIAN) == DONE && instance->GetData(TYPE_HALION) != DONE)
								{
									if (!halion)
										halion = me->SummonCreature(NPC_HALION_REAL, SpawnLocXer[3].x, SpawnLocXer[3].y, SpawnLocXer[3].z, 6.23f, TEMPSUMMON_MANUAL_DESPAWN, HOUR*IN_MILLISECONDS);

									if (halion && !halion->IsAlive())
										halion->Respawn();

									if (halion)
										halion->SetCreatorGUID(0);

									if (halion)
										halion->SetReactState(REACT_PASSIVE);
								}
							}
							UpdateTimer = 4000;
							instance->SetData(TYPE_EVENT,210);
							break;
						case 210:
							UpdateTimer = 2000;
							instance->SetData(TYPE_EVENT,0);
							break;
						default:
							break;
                    }
                 }
                 else UpdateTimer -= diff;

				 instance->SetData(TYPE_EVENT_TIMER, UpdateTimer);
            }
        }
    };

	CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_xerestraszaAI(creature);
    }
};

class spell_baltharus_enervating_brand : public SpellScriptLoader
{
    public:
        spell_baltharus_enervating_brand() : SpellScriptLoader("spell_baltharus_enervating_brand") { }

        class spell_baltharus_enervating_brand_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_baltharus_enervating_brand_AuraScript);

            void HandleTriggerSpell(AuraEffect const* aurEff)
            {
                PreventDefaultAction();
                Unit* target = GetTarget();
                uint32 triggerSpellId = GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell;
                target->CastSpell(target, triggerSpellId, true);

                if (Unit* caster = GetCaster())
                    if (target->GetDistance(caster) <= 12.0f)
                        target->CastSpell(caster, SPELL_SIPHONED_MIGHT, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_baltharus_enervating_brand_AuraScript::HandleTriggerSpell, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_baltharus_enervating_brand_AuraScript();
        }
};

class EnervatingBrandSelector
{
    public:
        explicit EnervatingBrandSelector(Unit* caster) : _caster(caster) {}

        bool operator()(Unit* unit)
        {
            if (_caster->GetDistance(unit) > 12.0f)
                return true;

            if (unit->GetTypeId() != TYPE_ID_PLAYER)
                return true;

            return false;
        }

    private:
        Unit* _caster;
};

class spell_baltharus_enervating_brand_trigger : public SpellScriptLoader
{
    public:
        spell_baltharus_enervating_brand_trigger() : SpellScriptLoader("spell_baltharus_enervating_brand_trigger") { }

        class spell_baltharus_enervating_brand_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_baltharus_enervating_brand_trigger_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                unitList.remove_if (EnervatingBrandSelector(GetCaster()));
                unitList.push_back(GetCaster());
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_baltharus_enervating_brand_trigger_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ALLY);
                OnUnitTargetSelect += SpellUnitTargetFn(spell_baltharus_enervating_brand_trigger_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_baltharus_enervating_brand_trigger_SpellScript();
        }
};

void AddSC_boss_baltharus_the_warborn()
{
    new boss_baltharus_the_warborn();
    new npc_baltharus_the_warborn_clone();
    new npc_xerestrasza();
	new spell_baltharus_enervating_brand();
    new spell_baltharus_enervating_brand_trigger();
}