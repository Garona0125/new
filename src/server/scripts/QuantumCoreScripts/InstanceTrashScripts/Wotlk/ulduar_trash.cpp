/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * Copyright (C) 2010-2015 QuantumCore <http://vk.com/quantumcore>
 *
 * Copyright (C) 2010-2012 MaNGOS project <http://getmangos.com>
 *
 */

// NEED IMPLEMENT 47677

#include "ScriptMgr.h"
#include "QuantumCreature.h"
#include "SpellScript.h"
#include <limits>
#include "../../../scripts/Northrend/Ulduar/Ulduar/ulduar.h"

struct RangeCheck : public std::unary_function<Unit*, bool>
{
    RangeCheck(Unit* base, float min, float max) : _base(base), _mindist(min), _maxdist(max) {}

	Unit* _base;
	float _mindist;
	float _maxdist;

    bool operator()(Unit* const other) const
    {
        float dist = _base->GetDistance(other->GetPositionX(), other->GetPositionY(), other->GetPositionZ());
        if (dist >= _mindist && dist <= _maxdist)
            return true;
        return false;
    }
};

struct IsEqualWO : public std::unary_function<WorldObject*, bool>
{
    IsEqualWO (const WorldObject* base) : _baseGUID(base->GetGUID()) {}

	uint64 _baseGUID;

    bool operator()(const WorldObject* target) const
    {
        if (!target)
            return false;
        return (target->GetGUID() == _baseGUID);
    }
};

struct DeathStateCheck : public std::unary_function<Unit*, bool>
{
    DeathStateCheck(bool shouldBeDead) : _shouldBeDead(shouldBeDead) {}

	bool _shouldBeDead;

    bool operator()(const Unit* target) const
    {
        if (!target)
            return false;
        return (!target->IsAlive() == _shouldBeDead);
    }
};

Position const Center[] =
{
	{354.8771f, -12.90240f, 409.803650f, 3.05873f},
};

enum SteelforgedDefender
{
	EVENT_HAMSTRING          = 1,
	EVENT_LIGHTNING_BOLT     = 2,
	EVENT_ST_SUNDER_ARMOR    = 3,

	SPELL_ACHIEVEMENT_CREDIT = 65387,
	SPELL_HAMSTRING          = 62845,
	SPELL_LIGHTNING_BOLT     = 57780,
	SPELL_ST_SUNDER_ARMOR    = 50370,
};

class npc_steelforged_defender : public CreatureScript
{
public:
	npc_steelforged_defender () : CreatureScript("npc_steelforged_defender") {}

	struct npc_steelforged_defenderAI: public QuantumBasicAI
	{
		npc_steelforged_defenderAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_HAMSTRING, 10*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_LIGHTNING_BOLT, urand(9*IN_MILLISECONDS, 12*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_ST_SUNDER_ARMOR, 2*IN_MILLISECONDS);
		}

		void JustDied(Unit* /*killer*/)
		{
			if (InstanceScript* instance = me->GetInstanceScript())
				DoCastVictim(SPELL_ACHIEVEMENT_CREDIT);
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
				    case EVENT_HAMSTRING:
						DoCastVictim(SPELL_HAMSTRING);
						events.ScheduleEvent(EVENT_HAMSTRING, 10*IN_MILLISECONDS);
						break;
					case EVENT_LIGHTNING_BOLT:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_LIGHTNING_BOLT);
						}
						events.ScheduleEvent(EVENT_LIGHTNING_BOLT, 9*IN_MILLISECONDS);
						break;
					case EVENT_ST_SUNDER_ARMOR:
						DoCastVictim(SPELL_ST_SUNDER_ARMOR);
						if (Unit* victim = me->GetVictim())
						{
							if (Aura* sunder = victim->GetAura(SPELL_ST_SUNDER_ARMOR))
							{
								if (sunder->GetStackAmount() == 5)
								{
									events.ScheduleEvent(EVENT_ST_SUNDER_ARMOR, 15*IN_MILLISECONDS);
									break;
								}
							}
						}
						events.ScheduleEvent(EVENT_ST_SUNDER_ARMOR, 2*IN_MILLISECONDS);
						break;
				}
			}
			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_steelforged_defenderAI(creature);
	}
};

enum Cannon
{
	EVENT_FLAME_IRON_CANNON = 1,
	SPELL_IRON_FLAME_CANNON = 62395,
};

class npc_ironwork_cannon : public CreatureScript
{
public:
	npc_ironwork_cannon () : CreatureScript("npc_ironwork_cannon") {}

	struct npc_ironwork_cannonAI: public QuantumBasicAI
	{
		npc_ironwork_cannonAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
			me->SetFloatValue(UNIT_FIELD_COMBAT_REACH, 200.0f);
		}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_FLAME_IRON_CANNON, 1200);
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
				    case EVENT_FLAME_IRON_CANNON:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0, RangeCheck(me, 30.0f, 200.0f)))
						{
							DoCast(target, SPELL_IRON_FLAME_CANNON);
						}
						events.ScheduleEvent(EVENT_FLAME_IRON_CANNON, 1500);
						break;
				}
			}
			// No melee attacks
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ironwork_cannonAI(creature);
	}
};

enum MoltenColossus
{
	EVENT_EARTHQUAKE   = 1,
	EVENT_MAGMA_SPLASH = 2,
	EVENT_PYROBLAST    = 3,

	SPELL_EARTHQUAKE   = 64697,
	SPELL_MAGMA_SPLASH = 64699,
	SPELL_PYROBLAST    = 64698,
};

class npc_molten_colossus : public CreatureScript
{
public:
	npc_molten_colossus () : CreatureScript("npc_molten_colossus") {}

	struct npc_molten_colossusAI: public QuantumBasicAI
	{
		npc_molten_colossusAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_EARTHQUAKE, urand(4*IN_MILLISECONDS, 8*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_MAGMA_SPLASH, urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_PYROBLAST, urand(6*IN_MILLISECONDS, 9*IN_MILLISECONDS));
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
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
                    case EVENT_EARTHQUAKE:
						DoCastAOE(SPELL_EARTHQUAKE);
						events.ScheduleEvent(EVENT_EARTHQUAKE, urand(4*IN_MILLISECONDS, 8*IN_MILLISECONDS));
						break;
					case EVENT_MAGMA_SPLASH:
						DoCastVictim(SPELL_MAGMA_SPLASH);
						events.ScheduleEvent(EVENT_MAGMA_SPLASH, urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS));
						break;
					case EVENT_PYROBLAST:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_PYROBLAST);
						}
						events.ScheduleEvent(EVENT_PYROBLAST, urand(6*IN_MILLISECONDS, 9*IN_MILLISECONDS));
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_molten_colossusAI(creature);
	}
};

enum ForgerConstruct
{
	EVENT_CONSTRUCT_CHARGE  = 1,
	EVENT_FLAME_EMISSION    = 2,

	SPELL_CHARGE            = 64719,
	SPELL_FLAME_EMISSION_10 = 64720,
	SPELL_FLAME_EMISSION_25 = 64721,
};

class npc_forge_construct : public CreatureScript
{
public:
	npc_forge_construct () : CreatureScript("npc_forge_construct") {}

	struct npc_forge_constructAI: public QuantumBasicAI
	{
		npc_forge_constructAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_CONSTRUCT_CHARGE, urand(8, 10)*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_FLAME_EMISSION, urand(4, 6)*IN_MILLISECONDS);
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
                    case EVENT_CONSTRUCT_CHARGE:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_CHARGE);
						}
						events.ScheduleEvent(EVENT_CONSTRUCT_CHARGE, urand(8*IN_MILLISECONDS, 10*IN_MILLISECONDS));
						break;
					case EVENT_FLAME_EMISSION:
						DoCast(RAID_MODE(SPELL_FLAME_EMISSION_10, SPELL_FLAME_EMISSION_25));
						events.ScheduleEvent(EVENT_FLAME_EMISSION, urand(4*IN_MILLISECONDS, 6*IN_MILLISECONDS));
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_forge_constructAI(creature);
	}
};

enum MagmaRager
{   
	EVENT_FIREBLAST             = 1,
	EVENT_SUPERHEATED_WINDS     = 2,

	SPELL_FIREBLAST             = 64773,
	SPELL_SUM_SUPERHEATED_WINDS = 64746,
};

class npc_magma_rager : public CreatureScript
{
public:
	npc_magma_rager () : CreatureScript("npc_magma_rager") {}

	struct npc_magma_ragerAI: public QuantumBasicAI
	{
		npc_magma_ragerAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_FIREBLAST, urand(4*IN_MILLISECONDS, 6*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_SUPERHEATED_WINDS, 3*IN_MILLISECONDS);
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
                    case EVENT_FIREBLAST:
						DoCastVictim(SPELL_FIREBLAST);
						events.ScheduleEvent(EVENT_FIREBLAST, urand(4*IN_MILLISECONDS,6*IN_MILLISECONDS));
						break;
					case EVENT_SUPERHEATED_WINDS:
						DoCast(SPELL_SUM_SUPERHEATED_WINDS);
						events.ScheduleEvent(EVENT_SUPERHEATED_WINDS, 12*IN_MILLISECONDS);
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_magma_ragerAI(creature);
	}
};

enum SuperheatedWinds
{
	SPELL_SUPERHEATED_WINDS = 64724,
};

class npc_superheated_winds : public CreatureScript
{
public:
	npc_superheated_winds () : CreatureScript("npc_superheated_winds") {}

	struct npc_superheated_windsAI: public QuantumBasicAI
	{
		npc_superheated_windsAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

		void Reset()
		{
			DoCastAOE(SPELL_SUPERHEATED_WINDS);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void UpdateAI(uint32 const /*diff*/){}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_superheated_windsAI(creature);
	}
};

enum MechagnomeBattletank
{
	EVENT_FLAME_CANNON = 1,
	EVENT_JUMP_ATTACK  = 2,

	SPELL_FLAME_CANNON = 64692,
	SPELL_JUMP_ATTACK  = 64953,
};

class npc_mechagnome_battletank : public CreatureScript
{
public:
	npc_mechagnome_battletank () : CreatureScript("npc_mechagnome_battletank") {}

	struct npc_mechagnome_battletankAI: public QuantumBasicAI
	{
		npc_mechagnome_battletankAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_FLAME_CANNON, urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_JUMP_ATTACK, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
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
                    case EVENT_FLAME_CANNON:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0, RangeCheck(me, 30.0f, 200.0f)))
						{
							DoCast(target, SPELL_FLAME_CANNON);
						}
						events.ScheduleEvent(EVENT_FLAME_CANNON, urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS));
						break;
					case EVENT_JUMP_ATTACK:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0, RangeCheck(me, 5.0f, 35.0f)))
						{
							DoCast(target, SPELL_JUMP_ATTACK);
						}
						events.ScheduleEvent(EVENT_JUMP_ATTACK, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_mechagnome_battletankAI(creature);
	}
};

enum MyEvents
{
	EVENT_MECHANO_KICK   = 1,
	EVENT_DEFENSE_MATRIX = 2,

	SPELL_DEFENSE_MATRIX = 65070,
	SPELL_MECHANO_KICK   = 65071,
};

class npc_parts_recovery_technician : public CreatureScript
{
public:
	npc_parts_recovery_technician () : CreatureScript("npc_parts_recovery_technician") {}

	struct npc_parts_recovery_technicianAI: public QuantumBasicAI
	{
		npc_parts_recovery_technicianAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_MECHANO_KICK, urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_DEFENSE_MATRIX, urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS));
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
                    case EVENT_MECHANO_KICK:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_MECHANO_KICK);
						}
						events.ScheduleEvent(EVENT_MECHANO_KICK, urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS));
						break;
					case EVENT_DEFENSE_MATRIX:
						if (!me->HasAura(SPELL_DEFENSE_MATRIX))
						{
							DoCast(SPELL_DEFENSE_MATRIX);
						}
						events.ScheduleEvent(EVENT_DEFENSE_MATRIX, urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS));
						break;                            
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_parts_recovery_technicianAI(creature);
	}
};

enum XR949Salvagebot
{
	EVENT_DEPLOY_SALVAGE_SAWS = 1,

	SPELL_DEPLOY_SALVAGE_SAWS = 65099,
};

class npc_xr_949_salvagebot : public CreatureScript
{
public:
	npc_xr_949_salvagebot () : CreatureScript("npc_xr_949_salvagebot") {}

	struct npc_xr_949_salvagebotAI: public QuantumBasicAI
	{
		npc_xr_949_salvagebotAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_DEPLOY_SALVAGE_SAWS, urand(5*IN_MILLISECONDS,10*IN_MILLISECONDS));
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
                    case EVENT_DEPLOY_SALVAGE_SAWS:
						DoCast(SPELL_DEPLOY_SALVAGE_SAWS);
						events.ScheduleEvent(EVENT_DEPLOY_SALVAGE_SAWS, urand(5*IN_MILLISECONDS,10*IN_MILLISECONDS));
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_xr_949_salvagebotAI(creature);
	}
};

enum SalvagebotSawblade
{
	SPELL_SAW_BLADES_10 = 65089,
	SPELL_SAW_BLADES_25 = 65102,
};

class npc_salvagebot_sawblade : public CreatureScript
{
public:
	npc_salvagebot_sawblade () : CreatureScript("npc_salvagebot_sawblade") {}

	struct npc_salvagebot_sawbladeAI: public QuantumBasicAI
	{
		npc_salvagebot_sawbladeAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			DoSpellAttackIfReady(RAID_MODE(SPELL_SAW_BLADES_10, SPELL_SAW_BLADES_25));
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_salvagebot_sawbladeAI(creature);
	}
};

enum XD175Compactobot
{
	EVENT_COMPACTED          = 1,
	EVENT_TRASH_COMPACTOR    = 2,

	SPELL_COMPACTED_10       = 65078,
	SPELL_COMPACTED_25       = 65105,
	SPELL_TRASH_COMPACTOR_10 = 65073,
	SPELL_TRASH_COMPACTOR_25 = 65106,
};

class npc_xd_175_compactobot : public CreatureScript
{
public:
	npc_xd_175_compactobot () : CreatureScript("npc_xd_175_compactobot") {}

	struct npc_xd_175_compactobotAI: public QuantumBasicAI
	{
		npc_xd_175_compactobotAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_COMPACTED, urand(5*IN_MILLISECONDS, 7*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_TRASH_COMPACTOR, urand(8*IN_MILLISECONDS, 12*IN_MILLISECONDS));
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
                    case EVENT_COMPACTED:
						DoCast(RAID_MODE(SPELL_COMPACTED_10, SPELL_COMPACTED_25));
						events.ScheduleEvent(EVENT_COMPACTED, urand(5*IN_MILLISECONDS, 7*IN_MILLISECONDS));
						break;
					case EVENT_TRASH_COMPACTOR:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0, RangeCheck(me, 10.0f, 40.0f)))
						{
							DoResetThreat();
							DoCast(target, RAID_MODE(SPELL_TRASH_COMPACTOR_10, SPELL_TRASH_COMPACTOR_25));
							me->AddThreat(target, 2000.0f);
						}
						events.ScheduleEvent(EVENT_TRASH_COMPACTOR, urand(8*IN_MILLISECONDS, 12*IN_MILLISECONDS));
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_xd_175_compactobotAI(creature);
	}
};

enum XB488Disposalbot
{
	EVENT_CUT_SCRAP_METAL    = 1,

	SPELL_CUT_SCRAP_METAL_10 = 65080,
	SPELL_CUT_SCRAP_METAL_25 = 65104,
	SPELL_SELF_DESTRCUT      = 65084,
};

class npc_xb_488_disposalbot : public CreatureScript
{
public:
	npc_xb_488_disposalbot () : CreatureScript("npc_xb_488_disposalbot") {}

	struct npc_xb_488_disposalbotAI: public QuantumBasicAI
	{
		npc_xb_488_disposalbotAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;
		bool gotDest;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			gotDest = false;
			events.Reset();
			events.ScheduleEvent(EVENT_CUT_SCRAP_METAL, urand(1*IN_MILLISECONDS, 2*IN_MILLISECONDS));
		}

		void DamageTaken(Unit* /*damager*/, uint32& damage)
		{
			if (damage >= me->GetHealth())
			{
				if (gotDest)
					return;

				gotDest = true;
				DoCast(SPELL_SELF_DESTRCUT);
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
				    case EVENT_CUT_SCRAP_METAL:
						DoCastVictim(RAID_MODE(SPELL_CUT_SCRAP_METAL_10, SPELL_CUT_SCRAP_METAL_25));
						events.ScheduleEvent(EVENT_CUT_SCRAP_METAL, urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS));
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_xb_488_disposalbotAI(creature);
	}
};

enum HardenedIronGolem
{
	EVENT_HARDEN_FISTS  = 1,
	EVENT_RUNE_PUNCH    = 2,

	SPELL_HARDEN_FIST   = 64877,
	SPELL_RUNE_PUNCH_10 = 64874,
	SPELL_RUNE_PUNCH_25 = 64967,
};

class npc_hardened_iron_golem : public CreatureScript
{
public:
	npc_hardened_iron_golem () : CreatureScript("npc_hardened_iron_golem") {}

	struct npc_hardened_iron_golemAI: public QuantumBasicAI
	{
		npc_hardened_iron_golemAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_HARDEN_FISTS, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_RUNE_PUNCH, urand(6*IN_MILLISECONDS, 12*IN_MILLISECONDS));
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
                    case EVENT_HARDEN_FISTS:
						DoCast(SPELL_HARDEN_FIST);
						if (me->GetHealthPct() > 50.0f)
						{
							events.ScheduleEvent(EVENT_HARDEN_FISTS, urand(15*IN_MILLISECONDS, 20*IN_MILLISECONDS));
						}
						else
						{
							events.ScheduleEvent(EVENT_HARDEN_FISTS, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
						}
						break;
					case EVENT_RUNE_PUNCH:
						DoCastVictim(RAID_MODE(SPELL_RUNE_PUNCH_10, SPELL_RUNE_PUNCH_25));
						events.ScheduleEvent(EVENT_RUNE_PUNCH, urand(6*IN_MILLISECONDS, 12*IN_MILLISECONDS));
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_hardened_iron_golemAI(creature);
	}
};

enum RuneEtchedForgedSentry
{
	EVENT_FLAMING_RUNE        = 1,
	EVENT_LAVABURST           = 2,
	EVENT_RUNED_FLAME_JETS    = 3,

	SPELL_FLAMING_RUNE        = 64852,
	SPELL_LAVA_BURST_10       = 64870,
	SPELL_LAVA_BURST_25       = 64991,
	SPELL_RUNED_FLAME_JETS_10 = 64847,
	SPELL_RUNED_FLAME_JETS_25 = 64988,
};

class npc_rune_etched_forged_sentry : public CreatureScript // For NPC_RUNE_ETCHED_SENTRY (outer sanctuary) and NPC_RUNE_FORGED_SENTRY (grand approach)
{
public:
	npc_rune_etched_forged_sentry () : CreatureScript("npc_rune_etched_forged_sentry") {}

	struct npc_rune_etched_forged_sentryAI: public QuantumBasicAI
	{
		npc_rune_etched_forged_sentryAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_FLAMING_RUNE, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_LAVABURST, urand(7*IN_MILLISECONDS, 10*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_RUNED_FLAME_JETS, urand(4*IN_MILLISECONDS, 6*IN_MILLISECONDS));
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
                    case EVENT_FLAMING_RUNE:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_FLAMING_RUNE);
						}
						events.ScheduleEvent(EVENT_FLAMING_RUNE, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
						break;
					case EVENT_LAVABURST:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, RAID_MODE(SPELL_LAVA_BURST_10, SPELL_LAVA_BURST_25));
						}
						events.ScheduleEvent(EVENT_LAVABURST, urand(8*IN_MILLISECONDS, 12*IN_MILLISECONDS));
						break;
					case EVENT_RUNED_FLAME_JETS:
						DoCastVictim(RAID_MODE(SPELL_RUNED_FLAME_JETS_10, SPELL_RUNED_FLAME_JETS_25));
						events.ScheduleEvent(EVENT_RUNED_FLAME_JETS, urand(8*IN_MILLISECONDS, 12*IN_MILLISECONDS));
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_rune_etched_forged_sentryAI(creature);
	}
};

enum ChamberOverseer
{
	EVENT_STAGGERING_ROAR       = 1,
	EVENT_DEVASTATING_LEAP      = 2,

	SPELL_DEVASTATING_LEAP_10   = 64820,
	SPELL_DEVASTATING_LEAP_25   = 64943,
	SPELL_STAGGERING_ROAR_10    = 64825,
	SPELL_STAGGERING_ROAR_25    = 64944,
	//SPELL_DISPLACEMENT_DEVICE = 64783, // Sense of this spell ? The summoned npc does not do anything
};

class npc_chamber_overseer : public CreatureScript
{
public:
	npc_chamber_overseer () : CreatureScript("npc_chamber_overseer") {}

	struct npc_chamber_overseerAI: public QuantumBasicAI
	{
		npc_chamber_overseerAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_STAGGERING_ROAR, urand(4*IN_MILLISECONDS, 8*IN_MILLISECONDS));
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
                    case EVENT_STAGGERING_ROAR:
						DoCastVictim(RAID_MODE(SPELL_STAGGERING_ROAR_10, SPELL_STAGGERING_ROAR_25));
						events.ScheduleEvent(EVENT_STAGGERING_ROAR, urand(4*IN_MILLISECONDS, 8*IN_MILLISECONDS));
						break;
					case EVENT_DEVASTATING_LEAP:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0, RangeCheck(me, 10.0f, 80.0f)))
						{
							DoResetThreat();
							DoCast(target, RAID_MODE(SPELL_DEVASTATING_LEAP_10, SPELL_DEVASTATING_LEAP_25));
							me->AddThreat(target, 2000.0f);
						}
						events.ScheduleEvent(EVENT_DEVASTATING_LEAP, urand(8*IN_MILLISECONDS, 12*IN_MILLISECONDS));
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_chamber_overseerAI(creature);
	}
};

enum IronMender
{
	SPELL_ELECTRO_SHOCK_10  = 64918,
	SPELL_ELECTRO_SHOCK_25  = 64971,
	SPELL_FUSE_LIGHTNING_10 = 64903,
	SPELL_FUSE_LIGHTNING_25 = 64970,
	SPELL_FUSE_METAL_10     = 64897,
	SPELL_FUSE_METAL_25     = 64968,
};

class npc_iron_mender : public CreatureScript
{
public:
	npc_iron_mender () : CreatureScript("npc_iron_mender") {}

	struct npc_iron_menderAI: public QuantumBasicAI
	{
		npc_iron_menderAI(Creature* creature) : QuantumBasicAI(creature) {}

		uint32 ElectroShockTimer;
		uint32 FuseLightningTimer;
		uint32 FuseMetalTimer;

		void Reset()
		{
			ElectroShockTimer = 1000;
			FuseLightningTimer = 3000;
			FuseMetalTimer = 5000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/){}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ElectroShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_ELECTRO_SHOCK_10, SPELL_ELECTRO_SHOCK_25));
					ElectroShockTimer = 4000;
				}
			}
			else ElectroShockTimer -= diff;

			if (FuseLightningTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, RAID_MODE(SPELL_FUSE_LIGHTNING_10, SPELL_FUSE_LIGHTNING_25));
					FuseLightningTimer = 7000;
				}
			}
			else FuseLightningTimer -= diff;

			if (FuseMetalTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, RAID_MODE(SPELL_FUSE_METAL_10, SPELL_FUSE_METAL_25));
					FuseMetalTimer = 9000;
				}
			}
			else FuseMetalTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_iron_menderAI(creature);
	}
};

enum IronDwarf
{
	SPELL_DUAL_WIELD           = 674,
	SPELL_LIGHTNING_CHARGED_10 = 64889,
	SPELL_LIGHTNING_CHARGED_25 = 64975,
};

class npc_lightning_charged_iron_dwarf : public CreatureScript
{
public:
	npc_lightning_charged_iron_dwarf () : CreatureScript("npc_lightning_charged_iron_dwarf") {}

	struct npc_lightning_charged_iron_dwarfAI: public QuantumBasicAI
	{
		npc_lightning_charged_iron_dwarfAI(Creature* creature) : QuantumBasicAI(creature) {}

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_DUAL_WIELD);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Lightning Charged Buff Check in Combat
			if (!me->HasAura(RAID_MODE(SPELL_LIGHTNING_CHARGED_10, SPELL_LIGHTNING_CHARGED_25)))
				DoCast(me, RAID_MODE(SPELL_LIGHTNING_CHARGED_10, SPELL_LIGHTNING_CHARGED_25));

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_lightning_charged_iron_dwarfAI(creature);
	}
};

enum StormTemperedKeeper
{
	EVENT_FORKED_LIGHTNING       = 1,
	EVENT_SUMMONS_CHARGED_SPHERE = 2,

	SPELL_FORKED_LIGHTNING       = 63541,
	SPELL_SEPARATION_ANXIETY     = 63539,
	SPELL_SUMMON_CHARGED_SPHERE  = 63527,
	SPELL_VENGEFUL_SURGE         = 63630,
};

class npc_storm_tempered_keeper : public CreatureScript
{
public:
	npc_storm_tempered_keeper () : CreatureScript("npc_storm_tempered_keeper") {}

	struct npc_storm_tempered_keeperAI: public QuantumBasicAI
	{
		npc_storm_tempered_keeperAI(Creature* creature) : QuantumBasicAI(creature), Summons(me){}

		EventMap events;
		SummonList Summons;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			Summons.DespawnAll();

			events.Reset();
			events.ScheduleEvent(EVENT_FORKED_LIGHTNING, 6*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_SUMMONS_CHARGED_SPHERE, 10*IN_MILLISECONDS);
		}

		void EnterToBattle(Unit* who)
		{
			QuantumBasicAI::AttackStart(who);
		}

		void JustDied(Unit* /*killer*/)
		{
			DoCastAOE(SPELL_VENGEFUL_SURGE);

			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_CHARGED_SPHERE)
				Summons.Summon(summon);
		}

		void DoSeparationCheck()
		{
			if (me->HasAura(SPELL_SEPARATION_ANXIETY))
				return;

			DoCast(me, SPELL_SEPARATION_ANXIETY);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			DoSeparationCheck();

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
                    case EVENT_FORKED_LIGHTNING:
						DoCastVictim(SPELL_FORKED_LIGHTNING);
						events.ScheduleEvent(EVENT_FORKED_LIGHTNING, 6*IN_MILLISECONDS);
						break;
					case EVENT_SUMMONS_CHARGED_SPHERE:
						DoCast(SPELL_SUMMON_CHARGED_SPHERE);
						events.ScheduleEvent(EVENT_SUMMONS_CHARGED_SPHERE, 10*IN_MILLISECONDS);
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_storm_tempered_keeperAI(creature);
	}
};

enum ChargedSphere
{
	EVENT_SUPERCHARGED = 1,

	SPELL_SUPERCHARGED = 63528,
};

class npc_charged_sphere : public CreatureScript
{
public:
	npc_charged_sphere () : CreatureScript("npc_charged_sphere") {}

	struct npc_charged_sphereAI: public QuantumBasicAI
	{
		npc_charged_sphereAI(Creature* creature) : QuantumBasicAI(creature){}

		EventMap events;

		void Reset()
		{
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();

			me->GetMotionMaster()->MoveRandom(30.0f);
			events.ScheduleEvent(EVENT_SUPERCHARGED, 2*IN_MILLISECONDS);
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
				    case EVENT_SUPERCHARGED:
						DoCastAOE(SPELL_SUPERCHARGED);
						events.ScheduleEvent(EVENT_SUPERCHARGED, 2*IN_MILLISECONDS);
						me->DespawnAfterAction(15*IN_MILLISECONDS);
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_charged_sphereAI(creature);
	}
};

enum ChampionOfHodir
{
	EVENT_FREEZING_BREATH = 1,
	EVENT_STOMP           = 2,

	SPELL_FREEZING_BREATH = 64649,
	SPELL_STOMP_10        = 64652,
	SPELL_STOMP_25        = 64639,
};

class npc_champion_of_hodir : public CreatureScript
{
public:
	npc_champion_of_hodir () : CreatureScript("npc_champion_of_hodir") {}

	struct npc_champion_of_hodirAI: public QuantumBasicAI
	{
		npc_champion_of_hodirAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_STOMP, 5*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_FREEZING_BREATH, urand(5, 8)*IN_MILLISECONDS);
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
                    case EVENT_FREEZING_BREATH:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
                        {
                            me->SetFacingToObject(target);
                            DoCastVictim(SPELL_FREEZING_BREATH);
                        }
						events.ScheduleEvent(EVENT_FREEZING_BREATH, urand(5*IN_MILLISECONDS, 8*IN_MILLISECONDS));
                        break;
					case EVENT_STOMP:
						DoCastAOE(RAID_MODE(SPELL_STOMP_10, SPELL_STOMP_25));
						events.ScheduleEvent(EVENT_STOMP, 5*IN_MILLISECONDS);
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_champion_of_hodirAI(creature);
	}
};

enum WinterRevenant
{
	EVENT_BLIZZARD                      = 1,
	EVENT_WHIRLING_STRIKE               = 2,
	EVENT_SHIELD                        = 3,

	SPELL_BLIZZARD_10                   = 64642,
	SPELL_BLIZZARD_25                   = 64653,
	SPELL_WHIRLING_STRIKE               = 64643,
	SPELL_SHIELD_OF_THE_WINTER_REVENANT = 64644,
};

class npc_winter_revenant : public CreatureScript
{
public:
	npc_winter_revenant () : CreatureScript("npc_winter_revenant") {}

	struct npc_winter_revenantAI: public QuantumBasicAI
	{
		npc_winter_revenantAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_WHIRLING_STRIKE, urand(5,8)*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_BLIZZARD, 3*IN_MILLISECONDS);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SHIELD_OF_THE_WINTER_REVENANT);
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
                    case EVENT_BLIZZARD:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, RAID_MODE(SPELL_BLIZZARD_10, SPELL_BLIZZARD_25));
						}
						events.ScheduleEvent(EVENT_BLIZZARD, 6*IN_MILLISECONDS);
						break;
					case EVENT_WHIRLING_STRIKE:
						DoCastAOE(SPELL_WHIRLING_STRIKE);
						events.ScheduleEvent(EVENT_WHIRLING_STRIKE, urand(5*IN_MILLISECONDS,8*IN_MILLISECONDS));
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_winter_revenantAI(creature);
	}
};

enum WinterRumbler
{
	EVENT_CONE_OF_COLD      = 1,
	EVENT_SNOW_BLINDNESS    = 2,

	SPELL_CONE_OF_COLD_10   = 64645,
	SPELL_CONE_OF_COLD_25   = 64655,
	SPELL_SNOW_BLINDNESS_10 = 64647,
	SPELL_SNOW_BLINDNESS_25 = 64654,
};

class npc_winter_rumbler : public CreatureScript
{
public:
	npc_winter_rumbler () : CreatureScript("npc_winter_rumbler") {}

	struct npc_winter_rumblerAI: public QuantumBasicAI
	{
		npc_winter_rumblerAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_CONE_OF_COLD, 8*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_SNOW_BLINDNESS, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
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
                    case EVENT_CONE_OF_COLD:
						DoCastVictim(RAID_MODE(SPELL_CONE_OF_COLD_10, SPELL_CONE_OF_COLD_25));
						events.ScheduleEvent(EVENT_CONE_OF_COLD, 8*IN_MILLISECONDS);
						break;
					case EVENT_SNOW_BLINDNESS:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, RAID_MODE(SPELL_SNOW_BLINDNESS_10, SPELL_SNOW_BLINDNESS_25));
						}
						events.ScheduleEvent(EVENT_SNOW_BLINDNESS, urand(10*IN_MILLISECONDS, 15*IN_MILLISECONDS));
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_winter_rumblerAI(creature);
	}
};

enum WinterJormungar
{
	EVENT_ACIDIC_BITE = 1,

	SPELL_ACIDIC_BITE = 64638,
};

class npc_winter_jormungar : public CreatureScript
{
public:
	npc_winter_jormungar () : CreatureScript("npc_winter_jormungar") {}

	struct npc_winter_jormungarAI: public QuantumBasicAI
	{
		npc_winter_jormungarAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetStandState(UNIT_STAND_STATE_SUBMERGED);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_ACIDIC_BITE, urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS));
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetStandState(UNIT_STAND_STATE_STAND);
		}

		void JustReachedHome()
		{
			Reset();
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
                    case EVENT_ACIDIC_BITE:
						if (Aura* acidic = me->GetVictim()->GetAura(SPELL_ACIDIC_BITE))
						{
							if (acidic->GetStackAmount() >= 5)
							{
								if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
								{
									DoCast(target, SPELL_ACIDIC_BITE);
									return;
								}
							}
						}
						DoCastVictim(SPELL_ACIDIC_BITE);
						events.ScheduleEvent(EVENT_ACIDIC_BITE, urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS));
						break;
				}

				DoMeleeAttackIfReady();
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_winter_jormungarAI(creature);
	}
};

enum DarkRuneThunderer
{
	EVENT_LIGHTNING_BRAND    = 1,

	SPELL_LIGHTNING_BRAND_10 = 63612,
	SPELL_LIGHTNING_BRAND_25 = 63673,
};

class npc_dark_rune_thunderer : public CreatureScript
{
public:
	npc_dark_rune_thunderer () : CreatureScript("npc_dark_rune_thunderer") {}

	struct npc_dark_rune_thundererAI: public QuantumBasicAI
	{
		npc_dark_rune_thundererAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			events.Reset();
			events.ScheduleEvent(EVENT_LIGHTNING_BRAND, 3*IN_MILLISECONDS);

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
                    case EVENT_LIGHTNING_BRAND:
						DoCastVictim(RAID_MODE(SPELL_LIGHTNING_BRAND_10, SPELL_LIGHTNING_BRAND_25));
						events.ScheduleEvent(EVENT_LIGHTNING_BRAND, urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS));
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_dark_rune_thundererAI(creature);
	}
};

enum DarkRuneRavager
{
	EVENT_RAVAGE_ARMOR = 1,
	SPELL_RAVAGE_ARMOR = 63615,
};

class npc_dark_rune_ravager : public CreatureScript
{
public:
	npc_dark_rune_ravager () : CreatureScript("npc_dark_rune_ravager") {}

	struct npc_dark_rune_ravagerAI: public QuantumBasicAI
	{
		npc_dark_rune_ravagerAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_RAVAGE_ARMOR, 2*IN_MILLISECONDS);
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
                    case EVENT_RAVAGE_ARMOR:
						DoCastVictim(SPELL_RAVAGE_ARMOR);
						events.ScheduleEvent(EVENT_RAVAGE_ARMOR, 2*IN_MILLISECONDS);
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_dark_rune_ravagerAI(creature);
	}
};

enum CorruptedServitor
{
	EVENT_VIOLENT_EARTH     = 1,
	EVENT_PETRIFY_JOINTS    = 2,

	SPELL_VIOLENT_EARTH     = 63149,
	SPELL_PETRIFY_JOINTS_10 = 63169,
	SPELL_PETRIFY_JOINTS_25 = 63549,
};

class npc_corrupted_servitor : public CreatureScript
{
public:
	npc_corrupted_servitor () : CreatureScript("npc_corrupted_servitor") {}

	struct npc_corrupted_servitorAI: public QuantumBasicAI
	{
		npc_corrupted_servitorAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_VIOLENT_EARTH, urand(6*IN_MILLISECONDS, 9*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_PETRIFY_JOINTS, 5*IN_MILLISECONDS);
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
                    case EVENT_VIOLENT_EARTH:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_VIOLENT_EARTH);
						}
						events.ScheduleEvent(EVENT_VIOLENT_EARTH, urand(6*IN_MILLISECONDS, 9*IN_MILLISECONDS));
						break;
					case EVENT_PETRIFY_JOINTS:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, RAID_MODE(SPELL_PETRIFY_JOINTS_10, SPELL_PETRIFY_JOINTS_25));
						}
						events.ScheduleEvent(EVENT_PETRIFY_JOINTS, 5*IN_MILLISECONDS);
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_corrupted_servitorAI(creature);
	}
};

enum MisguidedNymph
{
	EVENT_BIND_LIFE          = 1,
	EVENT_FROST_SPEAR        = 2,
	EVENT_WINTERS_EMBRACE    = 3,

	SPELL_BIND_LIFE_10       = 63082,
	SPELL_BIND_LIFE_25       = 63559,
	SPELL_FROST_SPEAR_10     = 63111,
	SPELL_FROST_SPEAR_25     = 63562,
	SPELL_WINTERS_EMBRACE_10 = 63136,
	SPELL_WINTERS_EMBRACE_25 = 63564,
};

class npc_misguided_nymph : public CreatureScript
{
public:
	npc_misguided_nymph () : CreatureScript("npc_misguided_nymph") {}

	struct npc_misguided_nymphAI: public QuantumBasicAI
	{
		npc_misguided_nymphAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_BIND_LIFE, 6*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_FROST_SPEAR, 8*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_WINTERS_EMBRACE, 1*IN_MILLISECONDS);
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
                    case EVENT_BIND_LIFE:
						if (me->GetHealthPct() < 80.0f)
							DoCast(me, RAID_MODE(SPELL_BIND_LIFE_10, SPELL_BIND_LIFE_25));

						events.ScheduleEvent(EVENT_BIND_LIFE, 6*IN_MILLISECONDS);
						break;
					case EVENT_FROST_SPEAR:
						DoCastVictim(RAID_MODE(SPELL_FROST_SPEAR_10, SPELL_FROST_SPEAR_25), true);
						events.ScheduleEvent(EVENT_FROST_SPEAR, 8*IN_MILLISECONDS);
						break;
					case EVENT_WINTERS_EMBRACE:
						if (!me->HasAura(RAID_MODE(SPELL_WINTERS_EMBRACE_10, SPELL_WINTERS_EMBRACE_25)))
							DoCast(me, RAID_MODE(SPELL_WINTERS_EMBRACE_10, SPELL_WINTERS_EMBRACE_25));

						events.ScheduleEvent(EVENT_WINTERS_EMBRACE, 30*IN_MILLISECONDS);
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_misguided_nymphAI(creature);
	}
};

enum GuardianLasher
{
	EVENT_GUARDIANS_LASH      = 1,

	SPELL_GUARDIANS_LASH_10   = 63047,
	SPELL_GUARDIANS_LASH_25   = 63550,
	SPELL_GUARDIAN_PHEROMONES = 63007,
};

class npc_guardian_lasher : public CreatureScript
{
public:
	npc_guardian_lasher () : CreatureScript("npc_guardian_lasher") {}

	struct npc_guardian_lasherAI: public QuantumBasicAI
	{
		npc_guardian_lasherAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(SPELL_GUARDIAN_PHEROMONES);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_GUARDIANS_LASH, 1*IN_MILLISECONDS);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Guardian Pheromones Buff Check in Combat
			if (!me->HasAura(SPELL_GUARDIAN_PHEROMONES))
				DoCast(me, SPELL_GUARDIAN_PHEROMONES);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				    case EVENT_GUARDIANS_LASH:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 5.0f))
						{
							DoCast(target, RAID_MODE(SPELL_GUARDIANS_LASH_10, SPELL_GUARDIANS_LASH_25));
						}
						events.ScheduleEvent(EVENT_GUARDIANS_LASH, 6*IN_MILLISECONDS);
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_guardian_lasherAI(creature);
	}
};

enum ForestSwarmer
{
	EVENT_POLLINATE = 1,

	SPELL_POLLINATE = 63059,
};

class npc_forest_swarmer : public CreatureScript
{
public:
	npc_forest_swarmer () : CreatureScript("npc_forest_swarmer") {}

	struct npc_forest_swarmerAI: public QuantumBasicAI
	{
		npc_forest_swarmerAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_POLLINATE, 5*IN_MILLISECONDS);
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
                    case EVENT_POLLINATE:
						DoCast(SPELL_POLLINATE);
						events.ScheduleEvent(EVENT_POLLINATE, 30*IN_MILLISECONDS);
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_forest_swarmerAI(creature);
	}
};

enum MangroveEnt
{
	EVENT_HURRICANE      = 1,
	EVENT_NOURISH        = 2,
	EVENT_TRANQUILITY    = 3,

	SPELL_HURRICANE      = 63272,
	SPELL_NOURISH_10     = 63242,
	SPELL_NOURISH_25     = 63556,
	SPELL_TRANQUILITY_10 = 63241,
	SPELL_TRANQUILITY_25 = 63554,
};

class npc_mangrove_ent : public CreatureScript
{
public:
	npc_mangrove_ent () : CreatureScript("npc_mangrove_ent") {}

	struct npc_mangrove_entAI: public QuantumBasicAI
	{
		npc_mangrove_entAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_HURRICANE, 3*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_NOURISH, 1*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_TRANQUILITY, 5*IN_MILLISECONDS);
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
                    case EVENT_HURRICANE:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_HURRICANE);
						}
						events.ScheduleEvent(EVENT_HURRICANE, 10*IN_MILLISECONDS);
						break;
					case EVENT_NOURISH:
						if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
						{
							DoCast(target, RAID_MODE(SPELL_NOURISH_10, SPELL_NOURISH_25));
						}
						events.ScheduleEvent(EVENT_NOURISH, 1500);
						break;
					case EVENT_TRANQUILITY:
						events.DelayEvents(10*IN_MILLISECONDS);
						DoCast(RAID_MODE(SPELL_TRANQUILITY_10, SPELL_TRANQUILITY_25));
						events.ScheduleEvent(EVENT_TRANQUILITY, 30*IN_MILLISECONDS);
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_mangrove_entAI(creature);
	}
};

enum IronrootLasher
{
	EVENT_IRONROOT_THORNS    = 1,

	SPELL_IRONROOT_THORNS_10 = 63240,
	SPELL_IRONROOT_THORNS_25 = 63553,
};

class npc_ironroot_lasher : public CreatureScript
{
public:
	npc_ironroot_lasher () : CreatureScript("npc_ironroot_lasher") {}

	struct npc_ironroot_lasherAI: public QuantumBasicAI
	{
		npc_ironroot_lasherAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_IRONROOT_THORNS, 2*IN_MILLISECONDS);
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
                    case EVENT_IRONROOT_THORNS:
						std::list<Unit*> allies;
						Trinity::AnyFriendlyUnitInObjectRangeCheck checker(me, me, 40.0f);
						Trinity::UnitListSearcher<Trinity::AnyFriendlyUnitInObjectRangeCheck> searcher(me, allies, checker);
						me->VisitNearbyObject(40.0f, searcher);
						allies.push_back(me->ToUnit());
						DoCast(Quantum::DataPackets::SelectRandomContainerElement(allies), RAID_MODE(SPELL_IRONROOT_THORNS_10, SPELL_IRONROOT_THORNS_25));
						events.ScheduleEvent(EVENT_IRONROOT_THORNS, 2*IN_MILLISECONDS);
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ironroot_lasherAI(creature);
	}
};

enum NaturesBlade
{
	EVENT_LIVING_TSUNAMI    = 1,

	SPELL_LIVING_TSUNAMI_10 = 63247,
	SPELL_LIVING_TSUNAMI_25 = 63568,
};

class npc_natures_blade : public CreatureScript
{
public:
	npc_natures_blade () : CreatureScript("npc_natures_blade") {}

	struct npc_natures_bladeAI: public QuantumBasicAI
	{
		npc_natures_bladeAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_LIVING_TSUNAMI, 3*IN_MILLISECONDS);
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
                    case EVENT_LIVING_TSUNAMI:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							me->SetFacingToObject(target);
							DoCast(RAID_MODE(SPELL_LIVING_TSUNAMI_10, SPELL_LIVING_TSUNAMI_25));
						}
						events.ScheduleEvent(EVENT_LIVING_TSUNAMI, 3*IN_MILLISECONDS);
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_natures_bladeAI(creature);
	}
};

enum GuardianOfLife
{
	EVENT_POISON_BREATH    = 1,

	SPELL_POISON_BREATH_10 = 63226,
	SPELL_POISON_BREATH_25 = 63551,
};

class npc_guardian_of_life : public CreatureScript
{
public:
	npc_guardian_of_life () : CreatureScript("npc_guardian_of_life") {}

	struct npc_guardian_of_lifeAI: public QuantumBasicAI
	{
		npc_guardian_of_lifeAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_POISON_BREATH, 3*IN_MILLISECONDS);
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
                    case EVENT_POISON_BREATH:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							me->SetFacingToObject(target);
							DoCast(RAID_MODE(SPELL_POISON_BREATH_10, SPELL_POISON_BREATH_25));
						}
						events.ScheduleEvent(EVENT_POISON_BREATH, 5*IN_MILLISECONDS);
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_guardian_of_lifeAI(creature);
	}
};

enum ArachnopodDestroyer
{
	EVENT_CHARGED_LEAP = 1,
	EVENT_FLAME_SPRAY  = 2,
	EVENT_MACHINE_GUN  = 3,

	SPELL_CHARGED_LEAP = 64779,
	SPELL_FLAME_SPRAY  = 64717,
	SPELL_MACHINE_GUN  = 64776,
};

class npc_arachnopod_destroyer : public CreatureScript
{
public:
	npc_arachnopod_destroyer () : CreatureScript("npc_arachnopod_destroyer") {}

	struct npc_arachnopod_destroyerAI: public QuantumBasicAI
	{
		npc_arachnopod_destroyerAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_CHARGED_LEAP, urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_FLAME_SPRAY, urand(6*IN_MILLISECONDS, 8*IN_MILLISECONDS));
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
                    case EVENT_CHARGED_LEAP:                            
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0, RangeCheck(me, 10.0f, 40.0f)))
						{
							DoResetThreat();
							DoCast(target, SPELL_CHARGED_LEAP);
						}
						events.ScheduleEvent(EVENT_CHARGED_LEAP, urand(20*IN_MILLISECONDS, 25*IN_MILLISECONDS));
						break;
					case EVENT_FLAME_SPRAY:
						DoCastVictim(SPELL_FLAME_SPRAY);
						events.ScheduleEvent(EVENT_FLAME_SPRAY, urand(6*IN_MILLISECONDS, 8*IN_MILLISECONDS));
						break;
					case EVENT_MACHINE_GUN:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_MACHINE_GUN);
						}
						events.ScheduleEvent(EVENT_MACHINE_GUN, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_arachnopod_destroyerAI(creature);
	}
};

enum ClockworkMechanic
{
	EVENT_ICE_TURRET = 1,

	SPELL_ICE_TURRET = 64966,
};

class npc_clockwork_mechanic : public CreatureScript
{
public:
	npc_clockwork_mechanic () : CreatureScript("npc_clockwork_mechanic") {}

	struct npc_clockwork_mechanicAI: public QuantumBasicAI
	{
		npc_clockwork_mechanicAI(Creature* creature) : QuantumBasicAI(creature), summons(me) {}

		EventMap events;
		SummonList summons;
		bool GotDowned;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			GotDowned = false;
			summons.DespawnAll();
			events.Reset();
			events.ScheduleEvent(EVENT_ICE_TURRET, urand(7*IN_MILLISECONDS, 10*IN_MILLISECONDS));
		}

		void DamageTaken(Unit* damager, uint32& damage)
		{
			if (damage >= me->GetHealth())
			{
				if (GotDowned)
					return;

				GotDowned = true;
				DoCastAOE(SPELL_ICE_TURRET, true);
			}
		}

		void JustSummoned(Creature* summon)
		{
			summons.Summon(summon);

			if (summons.size() >= RAID_MODE<SummonList::size_type>(4, 8))
				events.CancelEvent(EVENT_ICE_TURRET);
		}

		void SummonedCreatureDies(Creature* summon, Unit* /*killer*/)
		{
			summons.Despawn(summon);

			if (summons.size() <= RAID_MODE<SummonList::size_type>(4, 8)) 
				events.RescheduleEvent(EVENT_ICE_TURRET, urand(7*IN_MILLISECONDS, 10*IN_MILLISECONDS));
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
				    case EVENT_ICE_TURRET:
						DoCastAOE(SPELL_ICE_TURRET);
						events.ScheduleEvent(EVENT_ICE_TURRET, urand(7*IN_MILLISECONDS, 10*IN_MILLISECONDS));
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_clockwork_mechanicAI(creature);
	}
};

enum IceTurret
{
	EVENT_FROST_NOVA  = 1,

	SPELL_ICE_NOVA_10 = 66346,
	SPELL_ICE_NOVA_25 = 64919,
};

class npc_ice_turret: public CreatureScript
{
public:
	npc_ice_turret () : CreatureScript("npc_ice_turret") {}

	struct npc_ice_turretAI: public QuantumBasicAI
	{
		npc_ice_turretAI(Creature* creature) : QuantumBasicAI(creature)
		{
			SetCombatMovement(false);
		}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_FROST_NOVA, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
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
                    case EVENT_FROST_NOVA:
						DoCastAOE(RAID_MODE(SPELL_ICE_NOVA_10, SPELL_ICE_NOVA_25));
						events.ScheduleEvent(EVENT_FROST_NOVA, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
						break;
				}
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ice_turretAI(creature);
	}
};

enum ClockworkSapper
{
	EVENT_ENERGY_SAP = 1,

	SPELL_ENERGY_SAP = 64740,
};

class npc_clockwork_sapper : public CreatureScript
{
public:
	npc_clockwork_sapper () : CreatureScript("npc_clockwork_sapper") {}

	struct npc_clockwork_sapperAI: public QuantumBasicAI
	{
		npc_clockwork_sapperAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_ENERGY_SAP, urand(1*IN_MILLISECONDS, 2*IN_MILLISECONDS));
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
                    case EVENT_ENERGY_SAP:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_ENERGY_SAP);
						}
						events.ScheduleEvent(EVENT_ENERGY_SAP, urand(6*IN_MILLISECONDS, 8*IN_MILLISECONDS));
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_clockwork_sapperAI(creature);
	}
};

enum BoomerXP
{
	SPELL_BOOM_BOT          = 63767,
	SPELL_BOOM_BOT_PERIODIC = 63801,
};

class npc_boomer_xp : public CreatureScript
{
public:
	npc_boomer_xp() : CreatureScript("npc_boomer_xp") {}

	struct npc_boomer_xpAI : public QuantumBasicAI
	{
		npc_boomer_xpAI(Creature* creature) : QuantumBasicAI(creature) {}

		bool Despawn;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			Despawn = false;
		}

		void InitializeAI()
		{
			me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
			Reset();
		}

		void EnterToBattle(Unit* /*who*/)
		{
			if (Unit* target = SelectPlayerTargetInRange(100.0f))
			{
				me->AddThreat(target, std::numeric_limits<float>::max());
				me->GetMotionMaster()->MoveFollow(target, 100.0f, 0.0f);
			}
		}

		Unit* SelectPlayerTargetInRange(float range)
		{
			Player* target = 0;
			Trinity::AnyPlayerInObjectRangeCheck u_check(me, range, true);
			Trinity::PlayerSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(me, target, u_check);
			me->VisitNearbyObject(range, searcher);
			return target;
		}

		void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_BOOM_BOT_PERIODIC)
				me->DespawnAfterAction(0.5*IN_MILLISECONDS);
		}

		void JustDied(Unit* /*killer*/)
		{
			DoCast(me, SPELL_BOOM_BOT, true);
		}

		void UpdateAI(const uint32 /*diff*/)
		{
			if (!UpdateVictim())
				return;

			if (!Despawn && me->IsWithinMeleeRange(me->GetVictim()))
			{
				Despawn = true;
				me->CastSpell(me, SPELL_BOOM_BOT, true);
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_boomer_xpAI(creature);
	}
};

enum FacelessHorror
{
	EVENT_DEATH_GRIP         = 1,
	EVENT_SHADOW_CRASH       = 2,
	EVENT_DOMINATE_MIND      = 3,
	EVENT_VOID_BARRIER_BEGIN = 4,
	EVENT_VOID_BARRIER_END   = 5,

	SPELL_DEATH_GRIP_FH      = 64429,
	SPELL_SHADOW_CRASH       = 63722,
	SPELL_VOID_BARRIER       = 63710,
	SPELL_VOID_WAVE          = 63703,
	SPELL_DOMINATE_MIND      = 67229,
};

class npc_faceless_horror : public CreatureScript
{
public:
	npc_faceless_horror () : CreatureScript("npc_faceless_horror") {}

	struct npc_faceless_horrorAI: public QuantumBasicAI
	{
		npc_faceless_horrorAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_DEATH_GRIP, 0.5*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_SHADOW_CRASH, 2*IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_DOMINATE_MIND, 4*IN_MILLISECONDS);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_VOID_WAVE);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (me->GetHealthPct()<30.0f && events.GetNextEventTime(EVENT_VOID_BARRIER_BEGIN) == 0 && events.GetNextEventTime(EVENT_VOID_BARRIER_END) == 0)
				events.ScheduleEvent(EVENT_VOID_BARRIER_BEGIN, 0);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
                    case EVENT_DEATH_GRIP:
						DoResetThreat();
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							me->AddThreat(target, 10000.0f);
							DoCast(target, SPELL_DEATH_GRIP_FH, true);
						}
						events.ScheduleEvent(EVENT_DEATH_GRIP, urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS));
						break;
					case EVENT_SHADOW_CRASH:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
						{
							DoCast(target, SPELL_SHADOW_CRASH);
						}
						events.ScheduleEvent(EVENT_SHADOW_CRASH, urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS));
						break;
					case EVENT_DOMINATE_MIND:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 1))
						{
							DoCast(target, SPELL_DOMINATE_MIND, true);
						}
						events.ScheduleEvent(EVENT_DOMINATE_MIND, urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS));
						break;
					case EVENT_VOID_BARRIER_BEGIN:
						DoCast(me, SPELL_VOID_BARRIER);
						events.ScheduleEvent(EVENT_VOID_BARRIER_END, 15*IN_MILLISECONDS);
						break;
					case EVENT_VOID_BARRIER_END:
						me->RemoveAurasDueToSpell(SPELL_VOID_BARRIER);
						events.ScheduleEvent(EVENT_VOID_BARRIER_BEGIN, 10*IN_MILLISECONDS);
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_faceless_horrorAI(creature);
	}
};

enum TwilightAdherent
{
	EVENT_ARCANE_BURST   = 1,
	EVENT_BLINK          = 2,
	EVENT_GREATER_HEAL   = 3,
	EVENT_PSYCHIC_SCREAM = 4,
	EVENT_RENEW          = 5,

	SPELL_ARCANE_BURST   = 64663,
	SPELL_BLINK          = 64662,
	SPELL_GREATER_HEAL   = 63760,
	SPELL_PSYCHIC_SCREAM = 13704,
	SPELL_RENEW          = 37978,
};

class npc_twilight_adherent : public CreatureScript
{
public:
	npc_twilight_adherent () : CreatureScript("npc_twilight_adherent") {}

	struct npc_twilight_adherentAI: public QuantumBasicAI
	{
		npc_twilight_adherentAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_BLINK, urand(8*IN_MILLISECONDS, 15*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_GREATER_HEAL, urand(5*IN_MILLISECONDS, 7*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_RENEW, urand(6*IN_MILLISECONDS, 12*IN_MILLISECONDS));
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (me->GetHealthPct()<40 && events.GetNextEventTime(EVENT_PSYCHIC_SCREAM) == 0)
				events.ScheduleEvent(EVENT_PSYCHIC_SCREAM, 0.5*IN_MILLISECONDS);

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
                    case EVENT_ARCANE_BURST:
						DoCast(SPELL_ARCANE_BURST);
						break;
					case EVENT_BLINK:
						if (Unit* target = SelectTarget(TARGET_FARTHEST, 0, 150.0f))
						{
							DoCast(target, SPELL_BLINK);
						}
						events.ScheduleEvent(EVENT_BLINK, urand(8*IN_MILLISECONDS, 15*IN_MILLISECONDS));
						events.ScheduleEvent(EVENT_ARCANE_BURST, 1*IN_MILLISECONDS);
						break;
					case EVENT_GREATER_HEAL:
						if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
						{
							DoCast(target, SPELL_GREATER_HEAL);
						}
						events.ScheduleEvent(EVENT_GREATER_HEAL, urand(5*IN_MILLISECONDS, 7*IN_MILLISECONDS));
						break;
					case EVENT_PSYCHIC_SCREAM:
						DoCast(SPELL_PSYCHIC_SCREAM);
						break;
					case EVENT_RENEW:
						if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
						{
							DoCast(target, SPELL_RENEW);
						}
						events.ScheduleEvent(EVENT_RENEW, urand(6*IN_MILLISECONDS, 12*IN_MILLISECONDS));
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_twilight_adherentAI(creature);
	}
};

enum TwilightFrostMage
{
	EVENT_TF_ARCANE_BURST     = 1,
	EVENT_TF_BLINK            = 2,
	EVENT_TF_FROST_NOVA       = 3,
	EVENT_TF_FROSTBOLT        = 4,
	EVENT_TF_FROSTBOLT_VOLLEY = 5,

	SPELL_TF_ARCANE_BURST     = 64663,
	SPELL_TF_BLINK            = 64662,
	SPELL_TF_FROST_NOVA       = 63912,
	SPELL_TF_FROSTBOLT        = 63913,
	SPELL_TF_FROSTBOLT_VOLLEY = 63758,
};

class npc_twilight_frost_mage : public CreatureScript
{
public:
	npc_twilight_frost_mage () : CreatureScript("npc_twilight_frost_mage") {}

	struct npc_twilight_frost_mageAI: public QuantumBasicAI
	{
		npc_twilight_frost_mageAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_TF_FROST_NOVA, urand(8*IN_MILLISECONDS, 15*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_TF_FROSTBOLT, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_TF_FROSTBOLT_VOLLEY, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
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
				    case EVENT_TF_ARCANE_BURST:
						DoCast(SPELL_TF_ARCANE_BURST);
						break;
					case EVENT_TF_BLINK:
						if (Unit* target = SelectTarget(TARGET_FARTHEST, 0, 150.f))
						{
							DoCast(target, SPELL_TF_BLINK);
						}
						events.ScheduleEvent(EVENT_TF_ARCANE_BURST, 1*IN_MILLISECONDS);
						break;
					case EVENT_TF_FROST_NOVA:
						events.ScheduleEvent(EVENT_TF_FROST_NOVA, urand(8*IN_MILLISECONDS, 15*IN_MILLISECONDS));
						events.ScheduleEvent(EVENT_TF_BLINK, 1*IN_MILLISECONDS);
						break;
					case EVENT_TF_FROSTBOLT:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_TF_FROSTBOLT);
						}
						events.ScheduleEvent(EVENT_TF_FROSTBOLT, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
						break;
					case EVENT_TF_FROSTBOLT_VOLLEY:
						DoCast(SPELL_TF_FROSTBOLT_VOLLEY);
						events.ScheduleEvent(EVENT_TF_FROSTBOLT_VOLLEY, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_twilight_frost_mageAI(creature);
	}
};

enum TwilightPyromancer
{
	EVENT_TP_ARCANE_BURST       = 1,
	EVENT_TP_BLINK              = 2,
	EVENT_TP_FIREBALL           = 3,
	EVENT_TP_FLAME_STRIKE       = 4,
	EVENT_TP_SUMMON_FIRE_ELE    = 5,

	SPELL_TP_ARCANE_BURST       = 64663,
	SPELL_TP_BLINK              = 64662,
	SPELL_TP_FIREBALL           = 63789,
	SPELL_TP_FLAME_STRIKE       = 63775,
	SPELL_TP_SUMMON_FIRE_ELE    = 63774,

	NPC_ENSLAVED_FIRE_ELEMENTAL = 33838,
};

class npc_twilight_pyromancer : public CreatureScript
{
public:
	npc_twilight_pyromancer () : CreatureScript("npc_twilight_pyromancer") {}

	struct npc_twilight_pyromancerAI: public QuantumBasicAI
	{
		npc_twilight_pyromancerAI(Creature* creature) : QuantumBasicAI(creature), Summons(me) {}

		EventMap events;
		SummonList Summons;

		void Reset()
		{
			Summons.DespawnAll();
			events.Reset();

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(SPELL_TP_SUMMON_FIRE_ELE);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.ScheduleEvent(EVENT_TP_BLINK, urand(8*IN_MILLISECONDS, 15*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_TP_FIREBALL, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_TP_FLAME_STRIKE, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
		}

		void JustSummoned(Creature* summon)
		{
			Summons.Summon(summon);
		}

		void SummonedCreatureDies(Creature* creature, Unit* /*killer*/)
		{
			Summons.Despawn(creature);

			if (creature->GetEntry() == NPC_ENSLAVED_FIRE_ELEMENTAL)
				events.ScheduleEvent(EVENT_TP_SUMMON_FIRE_ELE, 1*IN_MILLISECONDS);
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
                    case EVENT_TP_ARCANE_BURST:
						DoCast(SPELL_TP_ARCANE_BURST);
						break;
					case EVENT_TP_BLINK:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0, 150.0f))
						{
							DoCast(target, SPELL_TP_BLINK);
						}
						events.ScheduleEvent(EVENT_TP_BLINK, urand(8*IN_MILLISECONDS, 15*IN_MILLISECONDS));
						events.ScheduleEvent(EVENT_TP_ARCANE_BURST, 1*IN_MILLISECONDS);
						break;
					case EVENT_TP_SUMMON_FIRE_ELE:
						DoCast(SPELL_TP_SUMMON_FIRE_ELE);
						break;
					case EVENT_TP_FIREBALL:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_TP_FIREBALL);
						}
						events.ScheduleEvent(EVENT_TP_FIREBALL, urand(3*IN_MILLISECONDS, 5*IN_MILLISECONDS));
						break;
					case EVENT_TP_FLAME_STRIKE:
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							DoCast(target, SPELL_TP_FLAME_STRIKE);
						}
						events.ScheduleEvent(EVENT_TP_FLAME_STRIKE, urand(5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_twilight_pyromancerAI(creature);
	}
};

enum EnslavedFireElemental
{
	EVENT_FIRE_SHIELD_CHECK = 1,
	EVENT_BLAST_WAVE        = 2,

	SPELL_FIRE_SHIELD       = 63778,
	SPELL_BLAST_WAVE        = 38064,
};

class npc_enslaved_fire_elemental : public CreatureScript
{
public:
	npc_enslaved_fire_elemental () : CreatureScript("npc_enslaved_fire_elemental") {}

	struct npc_enslaved_fire_elementalAI: public QuantumBasicAI
	{
		npc_enslaved_fire_elementalAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_FIRE_SHIELD);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_FIRE_SHIELD_CHECK, 1*IN_MILLISECONDS, 1);
			events.ScheduleEvent(EVENT_BLAST_WAVE, 6*IN_MILLISECONDS);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (!me->HasAura(SPELL_FIRE_SHIELD))
			{
				DoCast(me, SPELL_FIRE_SHIELD);
				events.DelayEvents(2*IN_MILLISECONDS, 1);
			}

			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
                    case EVENT_FIRE_SHIELD_CHECK:
					{
						std::list<Unit*> allies;
						Trinity::AnyFriendlyUnitInObjectRangeCheck checker(me, me, 20.0f);
						Trinity::UnitListSearcher<Trinity::AnyFriendlyUnitInObjectRangeCheck> searcher(me, allies, checker);
						me->VisitNearbyObject(20.0f, searcher);
						allies.remove_if(Trinity::UnitAuraCheck(true, SPELL_FIRE_SHIELD));
						if (!allies.empty())
							DoCast(Quantum::DataPackets::SelectRandomContainerElement(allies), SPELL_FIRE_SHIELD);
						events.ScheduleEvent(EVENT_FIRE_SHIELD_CHECK, 1*IN_MILLISECONDS);
					}
					break;
					case EVENT_BLAST_WAVE:
						DoCast(SPELL_BLAST_WAVE);
						events.ScheduleEvent(EVENT_BLAST_WAVE, 6*IN_MILLISECONDS);
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_enslaved_fire_elementalAI(creature);
	}
};

enum TwilightGuardian
{
	EVENT_CONCUSSION_BLOW = 1,
	EVENT_DEVASTATE       = 2,
	EVENT_SUNDER_ARMOR    = 2,
	EVENT_THUNDERCLAP     = 3,

	SPELL_CONCUSSION_BLOW = 52719,
	SPELL_DEVASTATE       = 62317,
	SPELL_SUNDER_ARMOR    = 57807,
	SPELL_THUNDERCLAP     = 63757,
};

class npc_twilight_guardian : public CreatureScript
{
public:
	npc_twilight_guardian () : CreatureScript("npc_twilight_guardian") {}

	struct npc_twilight_guardianAI: public QuantumBasicAI
	{
		npc_twilight_guardianAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_CONCUSSION_BLOW, urand(3*IN_MILLISECONDS, 6*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_DEVASTATE, urand(1500, 2500));
			events.ScheduleEvent(EVENT_THUNDERCLAP, urand (5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
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
                    case EVENT_CONCUSSION_BLOW:
						DoCastVictim(SPELL_CONCUSSION_BLOW);
						DoResetThreat();
						if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
						{
							me->AddThreat(target, 1000.0f);
							AttackStart(target);
						}
						events.ScheduleEvent(EVENT_CONCUSSION_BLOW, urand(3*IN_MILLISECONDS, 6*IN_MILLISECONDS));
						break;
					case EVENT_DEVASTATE:
						DoCastVictim(SPELL_DEVASTATE);
						events.ScheduleEvent(EVENT_DEVASTATE, urand(1500, 2500));
						break;
					case EVENT_THUNDERCLAP:
						DoCastAOE(SPELL_THUNDERCLAP);
						events.ScheduleEvent(EVENT_THUNDERCLAP, urand (5*IN_MILLISECONDS, 10*IN_MILLISECONDS));
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_twilight_guardianAI(creature);
	}
};

enum TwilightSlayer
{
	EVENT_BLADESTORM    = 1,
	EVENT_MORTAL_STRIKE = 2,

	SPELL_BLADESTORM    = 63784,
	SPELL_MORTAL_STRIKE = 35054,
};

class npc_twilight_slayer : public CreatureScript
{
public:
	npc_twilight_slayer () : CreatureScript("npc_twilight_slayer") {}

	struct npc_twilight_slayerAI: public QuantumBasicAI
	{
		npc_twilight_slayerAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			events.Reset();
			events.ScheduleEvent(EVENT_BLADESTORM, urand(4*IN_MILLISECONDS, 8*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_MORTAL_STRIKE, urand(3*IN_MILLISECONDS, 6*IN_MILLISECONDS));
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
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
                    case EVENT_BLADESTORM:
						DoCast(me, SPELL_BLADESTORM);
						events.DelayEvents(8*IN_MILLISECONDS);
						events.ScheduleEvent(EVENT_BLADESTORM, urand(12*IN_MILLISECONDS, 16*IN_MILLISECONDS));
						break;
					case EVENT_MORTAL_STRIKE:
						DoCastVictim(SPELL_MORTAL_STRIKE);
						events.ScheduleEvent(EVENT_MORTAL_STRIKE, urand(4*IN_MILLISECONDS, 6*IN_MILLISECONDS));
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_twilight_slayerAI(creature);
	}
};

enum TwilightShadowblade
{
	EVENT_DEADLY_POSION = 1,
	EVENT_FAN_OF_KNIVES = 2,

	SPELL_STEALTH       = 30831,
	SPELL_SHADOWSTEP    = 63793,
	SPELL_BACKSTAB      = 63754,
	SPELL_FAN_OF_KNIVES = 63753,
	SPELL_DEADLY_POSION = 63755,
};

class npc_twilight_shadowblade : public CreatureScript
{
public:
	npc_twilight_shadowblade () : CreatureScript("npc_twilight_shadowblade") {}

	struct npc_twilight_shadowbladeAI: public QuantumBasicAI
	{
		npc_twilight_shadowbladeAI(Creature* creature) : QuantumBasicAI(creature) {}

		EventMap events;

		bool ShadowStep;

		void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			DoCast(me, SPELL_STEALTH);

			me->SetPowerType(POWER_ENERGY);
			me->SetMaxPower(POWER_ENERGY, 100);

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			ShadowStep = false;

			events.Reset();
			events.ScheduleEvent(EVENT_DEADLY_POSION, urand(12*IN_MILLISECONDS, 15*IN_MILLISECONDS));
			events.ScheduleEvent(EVENT_FAN_OF_KNIVES, urand(6*IN_MILLISECONDS, 10*IN_MILLISECONDS));
		}

		void MoveInLineOfSight(Unit* who)
		{
			if (me->IsWithinDistInMap(who, 10.0f) && who->GetTypeId() == TYPE_ID_PLAYER)
			{
				if (ShadowStep)
				{
					me->RemoveAurasDueToSpell(SPELL_STEALTH);
					DoCast(who, SPELL_SHADOWSTEP, true);
					DoCast(who, SPELL_BACKSTAB, true);
					ShadowStep = true;
				}
			}

			events.ScheduleEvent(EVENT_DEADLY_POSION, 500);
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
				    case EVENT_DEADLY_POSION:
						DoCastVictim(SPELL_DEADLY_POSION);
						events.ScheduleEvent(EVENT_DEADLY_POSION, urand(12*IN_MILLISECONDS, 15*IN_MILLISECONDS));
						break;
					case EVENT_FAN_OF_KNIVES:
						DoCastAOE(SPELL_FAN_OF_KNIVES);
						events.ScheduleEvent(EVENT_FAN_OF_KNIVES, urand(6*IN_MILLISECONDS, 10*IN_MILLISECONDS));
						break;
				}
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_twilight_shadowbladeAI(creature);
	}
};

class npc_influence_tentacle_visual: public CreatureScript
{
public:
    npc_influence_tentacle_visual() : CreatureScript("npc_influence_tentacle_visual") {}

    struct npc_influence_tentacle_visualAI : public QuantumBasicAI
    {
        npc_influence_tentacle_visualAI(Creature* creature) : QuantumBasicAI(creature){}

		uint32 LostIllusionTimer;

        void Reset()
		{
			LostIllusionTimer = 7500;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (LostIllusionTimer <= diff && !me->IsInCombatActive())
			{
				me->PlayDirectSound(SOUND_ID_LOST_ILLUSION);
				LostIllusionTimer = 9999999;
			}
			else LostIllusionTimer -= diff;

            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_influence_tentacle_visualAI(creature);
    }
};

enum UlduarColossus
{
	SPELL_GROUND_SLAM = 62625,
};

class npc_ulduar_colossus : public CreatureScript
{
public:
	npc_ulduar_colossus() : CreatureScript("npc_ulduar_colossus") { }

	struct npc_ulduar_colossusAI : public QuantumBasicAI
	{
		npc_ulduar_colossusAI(Creature* creature) : QuantumBasicAI(creature)
		{
			instance = creature->GetInstanceScript();
		}

		uint32 GroundSlamTimer;

		InstanceScript* instance;

		void Reset()
		{
			GroundSlamTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

		void JustDied(Unit* /*who*/)
		{
			if (me->GetHomePosition().IsInDist(Center, 50.f))
				instance->SetData(DATA_GIANT_COLOSSUS, instance->GetData(DATA_GIANT_COLOSSUS) + 1);
		}

		void UpdateAI(uint32 const diff)
		{
			if (!UpdateVictim())
				return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (GroundSlamTimer <= diff)
			{
				DoCastVictim(SPELL_GROUND_SLAM);
				GroundSlamTimer = urand(20, 25)*IN_MILLISECONDS;
			}
			else GroundSlamTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ulduar_colossusAI(creature);
	}
};

class spell_pollinate : public SpellScriptLoader
{
public:
	spell_pollinate() : SpellScriptLoader("spell_pollinate") {}

	class spell_pollinate_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_pollinate_SpellScript);

		bool Validate(SpellInfo const* /*spell*/)
		{
			if (!sSpellMgr->GetSpellInfo(SPELL_POLLINATE))
				return false;
			return true;
		}

		void FilterTargets(std::list<Unit*>& unitList)
		{
			for (std::list<Unit*>::iterator itr = unitList.begin(); itr != unitList.end(); ++itr)
			{
				if (Creature* creature = (*itr)->ToCreature())
				{
					if (creature->GetEntry() != NPC_GUARDIAN_LASHER)
						itr = unitList.erase(itr);
				}
				else itr = unitList.erase(itr);
			}
		}

		void Register()
		{
			OnUnitTargetSelect += SpellUnitTargetFn(spell_pollinate_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_NEARBY_ENTRY);
			OnUnitTargetSelect += SpellUnitTargetFn(spell_pollinate_SpellScript::FilterTargets, EFFECT_1, TARGET_UNIT_NEARBY_ENTRY);
			OnUnitTargetSelect += SpellUnitTargetFn(spell_pollinate_SpellScript::FilterTargets, EFFECT_2, TARGET_UNIT_NEARBY_ENTRY);
		}
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_pollinate_SpellScript();
	}
};

void AddSC_ulduar_trash()
{
	new npc_steelforged_defender();
    new npc_ironwork_cannon();
    new npc_molten_colossus();
    new npc_forge_construct();
    new npc_magma_rager();
	new npc_superheated_winds();
    new npc_mechagnome_battletank();
    new npc_parts_recovery_technician();
    new npc_xr_949_salvagebot();
    new npc_salvagebot_sawblade();
    new npc_xd_175_compactobot();
    new npc_xb_488_disposalbot();
    new npc_hardened_iron_golem();
    new npc_rune_etched_forged_sentry();
    new npc_chamber_overseer();
    new npc_iron_mender();
    new npc_lightning_charged_iron_dwarf();
    new npc_storm_tempered_keeper();
    new npc_charged_sphere();
    new npc_champion_of_hodir();
    new npc_winter_revenant();
    new npc_winter_rumbler();
    new npc_winter_jormungar();
    new npc_dark_rune_thunderer();
    new npc_dark_rune_ravager();
    new npc_faceless_horror();
    new npc_twilight_adherent();
    new npc_twilight_frost_mage();
    new npc_twilight_pyromancer();
    new npc_enslaved_fire_elemental();
    new npc_twilight_guardian();
    new npc_twilight_slayer();
    new npc_twilight_shadowblade();
    new npc_corrupted_servitor();
    new npc_misguided_nymph();
    new npc_guardian_lasher();
    new npc_forest_swarmer();
    new npc_mangrove_ent();
    new npc_ironroot_lasher();
    new npc_natures_blade();
    new npc_guardian_of_life();
    new npc_arachnopod_destroyer();
    new npc_clockwork_mechanic();
    new npc_ice_turret();
    new npc_clockwork_sapper();
	new npc_boomer_xp();
	new npc_influence_tentacle_visual();
	new npc_ulduar_colossus();
    new spell_pollinate();
}