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
#include "ruby_sanctum.h"

enum Texts
{
	SAY_AGGRO      = -1666400,
	SAY_SLAY_1     = -1666401,
	SAY_SLAY_2     = -1666402,
	SAY_FLIGHT     = -1666403,
	SAY_ENRAGE     = -1666404,
	SOUND_ID_DEATH = 17531,
};

enum Spells
{
    SPELL_CONFLAGRATION         = 74452,
    SPELL_FLAME_BEACON          = 74453,
    SPELL_CONFLAGRATION_2       = 74454,
    SPELL_ENRAGE                = 78722,
    SPELL_FLAME_BREATH          = 74403,
};

enum Events
{
    EVENT_ENRAGE                = 1,
    EVENT_FLIGHT                = 2,
    EVENT_FLAME_BREATH          = 3,
    EVENT_CONFLAGRATION         = 4,
    EVENT_LAND_GROUND           = 5,
    EVENT_AIR_MOVEMENT          = 6,
    EVENT_GROUP_LAND_PHASE      = 1,
};

enum MovementPoints
{
    POINT_FLIGHT                = 1,
    POINT_LAND                  = 2,
    POINT_TAKE_OFF              = 3,
    POINT_LAND_GROUND           = 4,
};

const Position SavianaRagefireFlyOutPos = {3155.51f, 683.844f, 95.0f, 4.69f};
const Position SavianaRagefireFlyInPos = {3151.07f, 636.443f, 79.540f, 4.69f};
const Position SavianaRagefireLandPos = {3151.07f, 636.443f, 78.649f, 4.69f};

class boss_saviana_ragefire : public CreatureScript
{
    public:
        boss_saviana_ragefire() : CreatureScript("boss_saviana_ragefire") { }

        struct boss_saviana_ragefireAI : public QuantumBasicAI
        {
            boss_saviana_ragefireAI(Creature* creature) : QuantumBasicAI(creature)
            {
				instance = (InstanceScript*)creature->GetInstanceScript();

				me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
				me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);

				Reset();
            }

			InstanceScript* instance;

			EventMap events;

            void Reset()
            {
				if (!instance)
					return;

				instance->SetData(TYPE_RAGEFIRE, NOT_STARTED);

				DoCast(me, SPELL_UNIT_DETECTION_WOTLK, true);

				me->SetRespawnDelay(7*DAY);
            }

            void EnterToBattle(Unit* /*who*/)
            {
				if (!instance)
					return;

				instance->SetData(TYPE_RAGEFIRE, IN_PROGRESS);
                DoSendQuantumText(SAY_AGGRO, me);
                events.Reset();
                events.ScheduleEvent(EVENT_ENRAGE, 20000, EVENT_GROUP_LAND_PHASE);
                events.ScheduleEvent(EVENT_FLAME_BREATH, 14000, EVENT_GROUP_LAND_PHASE);
                events.ScheduleEvent(EVENT_FLIGHT, 60000);
            }

			void KilledUnit(Unit* who)
			{
				if (who->GetTypeId() == TYPE_ID_PLAYER)
					DoSendQuantumText(RAND(SAY_SLAY_1, SAY_SLAY_2), me);
			}

            void JustDied(Unit* /*killer*/)
            {
				instance->SetData(TYPE_RAGEFIRE, DONE);
                me->PlayDirectSound(SOUND_ID_DEATH);
            }

            void MovementInform(uint32 type, uint32 point)
            {
                if (type != POINT_MOTION_TYPE && type != EFFECT_MOTION_TYPE)
                    return;

                switch (point)
                {
                    case POINT_FLIGHT:
                        events.ScheduleEvent(EVENT_CONFLAGRATION, 1000);
                        DoSendQuantumText(SAY_FLIGHT, me);
                        break;
                    case POINT_LAND:
                        events.ScheduleEvent(EVENT_LAND_GROUND, 1);
                        break;
                    case POINT_LAND_GROUND:
                        me->SetCanFly(false);
                        me->SetDisableGravity(false);
                        me->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_HOVER);
                        me->SetReactState(REACT_AGGRESSIVE);
                        DoStartMovement(me->GetVictim());
                        break;
                    case POINT_TAKE_OFF:
                        events.ScheduleEvent(EVENT_AIR_MOVEMENT, 1);
                        break;
                    default:
                        break;
                }
            }

            void JustReachedHome()
            {
				instance->SetData(TYPE_RAGEFIRE, FAIL);

                me->SetCanFly(false);
                me->SetDisableGravity(false);
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
                        case EVENT_FLIGHT:
                        {
							me->HandleEmoteCommand(EMOTE_ONESHOT_FLY_SIT_GROUND_UP);
                            me->SetCanFly(true);
                            me->SetDisableGravity(true);
                            me->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_HOVER);
                            me->SetReactState(REACT_PASSIVE);
                            me->AttackStop();
                            Position pos;
                            pos.Relocate(me);
                            pos.m_positionZ += 10.0f;
                            me->GetMotionMaster()->MoveTakeoff(POINT_TAKE_OFF, pos);
                            events.ScheduleEvent(EVENT_FLIGHT, 50000);
                            events.DelayEvents(12500, EVENT_GROUP_LAND_PHASE);
                            break;
                        }
                        case EVENT_CONFLAGRATION:
                            DoCast(me, SPELL_CONFLAGRATION, true);
                            break;
                        case EVENT_ENRAGE:
                            DoCast(me, SPELL_ENRAGE);
                            DoSendQuantumText(SAY_ENRAGE, me);
                            events.ScheduleEvent(EVENT_ENRAGE, urand(15000, 20000), EVENT_GROUP_LAND_PHASE);
                            break;
                        case EVENT_FLAME_BREATH:
                            DoCastVictim(SPELL_FLAME_BREATH);
                            events.ScheduleEvent(EVENT_FLAME_BREATH, urand(20000, 30000), EVENT_GROUP_LAND_PHASE);
                            break;
                        case EVENT_AIR_MOVEMENT:
                            me->GetMotionMaster()->MovePoint(POINT_FLIGHT, SavianaRagefireFlyOutPos);
                            break;
                        case EVENT_LAND_GROUND:
                            me->GetMotionMaster()->MoveLand(POINT_LAND_GROUND, SavianaRagefireLandPos);
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
			return new boss_saviana_ragefireAI(creature);
        }
};

class ConflagrationTargetSelector
{
public:
	ConflagrationTargetSelector() {}

	bool operator()(WorldObject* unit) const
	{
		return unit->GetTypeId() != TYPE_ID_PLAYER;
	}
};

class spell_saviana_conflagration_init : public SpellScriptLoader
{
    public:
        spell_saviana_conflagration_init() : SpellScriptLoader("spell_saviana_conflagration_init") { }

        class spell_saviana_conflagration_init_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_saviana_conflagration_init_SpellScript);

            void FilterTargets(std::list<Unit*>& unitList)
            {
                if (unitList.empty())
                    return;

                unitList.remove_if (ConflagrationTargetSelector());
                uint8 maxSize = uint8(GetCaster()->GetMap()->GetSpawnMode() & 1 ? 6 : 3);
                if (unitList.size() > maxSize)
                    Quantum::DataPackets::RandomResizeList(unitList, maxSize);
            }

            void HandleDummy(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetCaster()->CastSpell(GetHitUnit(), SPELL_FLAME_BEACON, true);
                GetCaster()->CastSpell(GetHitUnit(), SPELL_CONFLAGRATION_2, false);
            }

            void Register()
            {
                OnUnitTargetSelect += SpellUnitTargetFn(spell_saviana_conflagration_init_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);
                OnEffectHitTarget += SpellEffectFn(spell_saviana_conflagration_init_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_saviana_conflagration_init_SpellScript();
        }
};

class spell_saviana_conflagration_throwback : public SpellScriptLoader
{
    public:
        spell_saviana_conflagration_throwback() : SpellScriptLoader("spell_saviana_conflagration_throwback") { }

        class spell_saviana_conflagration_throwback_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_saviana_conflagration_throwback_SpellScript);

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                GetHitUnit()->CastSpell(GetCaster(), uint32(GetEffectValue()), true);
                GetHitUnit()->GetMotionMaster()->MovePoint(POINT_LAND, SavianaRagefireFlyInPos);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_saviana_conflagration_throwback_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_saviana_conflagration_throwback_SpellScript();
        }
};

void AddSC_boss_saviana_ragefire()
{
    new boss_saviana_ragefire();
    new spell_saviana_conflagration_init();
    new spell_saviana_conflagration_throwback();
}