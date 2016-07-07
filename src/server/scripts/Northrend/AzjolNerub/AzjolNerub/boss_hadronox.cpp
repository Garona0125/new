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
#include "azjol_nerub.h"

enum Spells
{
    SPELL_ACID_CLOUD_5N    = 53400,
	SPELL_ACID_CLOUD_5H    = 59419,
    SPELL_LEECH_POISON_5N  = 53030,
	SPELL_LEECH_POISON_5H  = 59417,
	SPELL_WEB_GRAB_5N      = 57731,
	SPELL_WEB_GRAB_5H      = 59421,
	SPELL_LEECH_POISON_PCT = 53800,
    SPELL_PIERCE_ARMOR     = 53418,
    SPELL_WEB_FRONT_DOORS  = 53177,
    SPELL_WEB_SIDE_DOORS   = 53185,
};

class boss_hadronox : public CreatureScript
{
public:
    boss_hadronox() : CreatureScript("boss_hadronox") { }

    struct boss_hadronoxAI : public QuantumBasicAI
    {
        boss_hadronoxAI(Creature* creature) : QuantumBasicAI(creature)
        {
            instance = creature->GetInstanceScript();
            FloatMaxDistance = 50.0f;
            FirstTime = true;
        }

        InstanceScript* instance;

        uint32 AcidCloudTimer;
        uint32 LeechPoisonTimer;
        uint32 PierceArmorTimer;
        uint32 GrabTimer;
        uint32 DoorsTimer;
        uint32 CheckDistanceTimer;

        bool FirstTime;

        float FloatMaxDistance;

        void Reset()
        {
            me->SetFloatValue(UNIT_FIELD_BOUNDING_RADIUS, 9.0f);
            me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 9.0f);

            AcidCloudTimer = urand(10*IN_MILLISECONDS, 14*IN_MILLISECONDS);
            LeechPoisonTimer = urand(3*IN_MILLISECONDS, 9*IN_MILLISECONDS);
            PierceArmorTimer = urand(1*IN_MILLISECONDS, 3*IN_MILLISECONDS);
            GrabTimer = urand(15*IN_MILLISECONDS, 19*IN_MILLISECONDS);
            DoorsTimer = urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS);
            CheckDistanceTimer = 2*IN_MILLISECONDS;

            if (instance->GetData(DATA_HADRONOX_EVENT) != DONE && !FirstTime)
                instance->SetData(DATA_HADRONOX_EVENT, FAIL);

            FirstTime = false;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void KilledUnit(Unit* victim)
        {
            if (!victim || !victim->HasAura(DUNGEON_MODE(SPELL_LEECH_POISON_5N, SPELL_LEECH_POISON_5H)) || !me->IsAlive())
                return;

            me->ModifyHealth(int32(me->CountPctFromMaxHealth(HEALTH_PERCENT_10)));
        }

        void JustDied(Unit* /*killer*/)
        {
			instance->SetData(DATA_HADRONOX_EVENT, DONE);
        }

        void EnterToBattle(Unit* /*who*/)
		{
			instance->SetData(DATA_HADRONOX_EVENT, IN_PROGRESS);

            me->SetInCombatWithZone();
        }

        void CheckDistance(float dist, const uint32 diff)
        {
            if (!me->IsInCombatActive())
                return;

            float x = 0.0f, y = 0.0f, z = 0.0f;
            me->GetRespawnPosition(x, y, z);

            if (CheckDistanceTimer <= diff)
                CheckDistanceTimer = 5*IN_MILLISECONDS;
            else
            {
                CheckDistanceTimer -= diff;
                return;
            }
            if (me->IsInEvadeMode() || !me->GetVictim())
                return;

            if (me->GetDistance(x, y, z) > dist)
                EnterEvadeMode();
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

            CheckDistance(FloatMaxDistance, diff);

            if (me->HasAura(SPELL_WEB_FRONT_DOORS) || me->HasAura(SPELL_WEB_SIDE_DOORS))
            {
                if (IsCombatMovementAllowed())
                    SetCombatMovement(false);
            }
            else if (!IsCombatMovementAllowed())
                SetCombatMovement(true);

            if (PierceArmorTimer <= diff)
            {
                DoCastVictim(SPELL_PIERCE_ARMOR);
                PierceArmorTimer = 8*IN_MILLISECONDS;
            }
			else PierceArmorTimer -= diff;

            if (AcidCloudTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_ACID_CLOUD_5N, SPELL_ACID_CLOUD_5H));
					AcidCloudTimer = urand(20*IN_MILLISECONDS, 30*IN_MILLISECONDS);
				}
            }
			else AcidCloudTimer -= diff;

            if (LeechPoisonTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 100, true))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_LEECH_POISON_5N, SPELL_LEECH_POISON_5H));
					LeechPoisonTimer = urand(11*IN_MILLISECONDS, 14*IN_MILLISECONDS);
				}
            }
			else LeechPoisonTimer -= diff;

            if (GrabTimer <= diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
                    DoCast(target, DUNGEON_MODE(SPELL_WEB_GRAB_5N, SPELL_WEB_GRAB_5H));
					GrabTimer = urand(15*IN_MILLISECONDS, 30*IN_MILLISECONDS);
				}
            }
			else GrabTimer -= diff;

            if (DoorsTimer <= diff)
            {
                DoorsTimer = urand(30*IN_MILLISECONDS, 60*IN_MILLISECONDS);
            }
			else DoorsTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_hadronoxAI(creature);
    }
};

class spell_hadronox_leech_poison : public SpellScriptLoader
{
public:
	spell_hadronox_leech_poison() : SpellScriptLoader("spell_hadronox_leech_poison") { }

	class spell_hadronox_leech_poison_AuraScript : public AuraScript
	{
		PrepareAuraScript(spell_hadronox_leech_poison_AuraScript);

		void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
		{
			Unit* caster = GetCaster();

			if (caster && caster->IsAlive() && GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_DEATH)
				caster->CastSpell(caster, SPELL_LEECH_POISON_PCT, true);
		}

		void Register()
		{
			AfterEffectRemove += AuraEffectRemoveFn(spell_hadronox_leech_poison_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_LEECH, AURA_EFFECT_HANDLE_REAL);
		}
	};

	AuraScript* GetAuraScript() const
	{
		return new spell_hadronox_leech_poison_AuraScript();
	}
};

void AddSC_boss_hadronox()
{
    new boss_hadronox;
	new spell_hadronox_leech_poison();
}