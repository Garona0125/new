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

enum HallsOfLightningSpells
{
	SPELL_DULL_WEAPONS_5N        = 52643,
	SPELL_DULL_WEAPONS_5H        = 59144,
	SPELL_CLEAVE                 = 42724,
	SPELL_SHIELD_SLAM_5N         = 15655,
	SPELL_SHIELD_SLAM_5H         = 59142,
	SPELL_HURL_WEAPON_5N         = 52740,
	SPELL_HURL_WEAPON_5H         = 59259,
	SPELL_WINDFURY               = 32910,
	SPELL_ENRAGE                 = 61369,
	SPELL_SHOOT_5N               = 16100,
	SPELL_SHOOT_5H               = 61515,
	SPELL_IMPACT_SHOT_5N         = 52754,
	SPELL_IMPACT_SHOT_5H         = 59148,
	SPELL_IMPACT_MULTI_SHOT_5N   = 52755,
	SPELL_IMPACT_MULTI_SHOT_5H   = 59147,
	SPELL_DISENGAGE              = 61507,
	SPELL_HAMMER_BLOW            = 52773,
	SPELL_RENEW_STEEL_5N         = 52774,
	SPELL_RENEW_STEEL_5H         = 59160,
	SPELL_WELDING_BEAM_5N        = 52778,
	SPELL_WELDING_BEAM_5H        = 59166,
	SPELL_ARC_WELD               = 59085,
	SPELL_BLAST_WAVE_5N          = 23113,
	SPELL_BLAST_WAVE_5H          = 22424,
	SPELL_MELT_ARMOR_5N          = 61509,
	SPELL_MELT_ARMOR_5H          = 61510,
	SPELL_LAVA_BURST_5N          = 53788,
	SPELL_LAVA_BURST_5H          = 59182,
	SPELL_AFTERBURN_5N           = 52624,
	SPELL_AFTERBURN_5H           = 59183,
	SPELL_STEAM_BLAST_5N         = 52531,
	SPELL_STEAM_BLAST_5H         = 59141,
	SPELL_THROW_5N               = 52904,
	SPELL_THROW_5H               = 59179,
	SPELL_DEADLY_THROW_5N        = 52885,
	SPELL_DEADLY_THROW_5H        = 59180,
	SPELL_DEFLECTION_5N          = 52879,
	SPELL_DEFLECTION_5H          = 59181,
	SPELL_FRIGHTENING_SHOUT      = 19134,
	SPELL_PIERCING_HOWL          = 23600,
	SPELL_PENETRATING_STRIKE     = 52890,
	SPELL_BLADE_TURNING_5N       = 52891,
	SPELL_BLADE_TURNING_5H       = 59173,
	SPELL_THUNDERBOLT_5N         = 52905,
	SPELL_THUNDERBOLT_5H         = 59167,
	SPELL_ELECTRO_SHOCK_5N       = 53043,
	SPELL_ELECTRO_SHOCK_5H       = 59168,
	SPELL_LIGHTNING_BOLT_5N      = 53044,
	SPELL_LIGHTNING_BOLT_5H      = 59169,
	SPELL_CYCLONE                = 60236,
	SPELL_FORKED_LIGHTNING_5N    = 53167,
	SPELL_FORKED_LIGHTNING_5H    = 59152,
	SPELL_RUNIC_FOCUS_5N         = 61596,
	SPELL_RUNIC_FOCUS_5H         = 61579,
	SPELL_RUNE_PUNCH_5N          = 53068,
	SPELL_RUNE_PUNCH_5H          = 59151,
	SPELL_STARLING_ROAR          = 53048,
	SPELL_CHARGED_FLURRY_5N      = 53049,
	SPELL_CHARGED_FLURRY_5H      = 61581,
	SPELL_STORMING_BELOW         = 53047,
	SPELL_SLEEP_5N               = 53045,
	SPELL_SLEEP_5H               = 59165,
	SPELL_RENEW_5N               = 34423,
	SPELL_RENEW_5H               = 37978,
	SPELL_CHARGE                 = 58619,
	SPELL_POISON_TIPPED_SPEAR_5N = 53059,
	SPELL_POISON_TIPPED_SPEAR_5H = 59178,
	SPELL_SOUL_STRIKE            = 32315,
	SPELL_STORMBOLT_5N           = 53072,
	SPELL_STORMBOLT_5H           = 59155,
	SPELL_THUNDERSTORM_5N        = 53071,
	SPELL_THUNDERSTORM_5H        = 59154,
};

class npc_hardened_steel_reaver : public CreatureScript
{
public:
    npc_hardened_steel_reaver() : CreatureScript("npc_hardened_steel_reaver") {}

    struct npc_hardened_steel_reaverAI : public QuantumBasicAI
    {
        npc_hardened_steel_reaverAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 CleaveTimer;
		uint32 ShieldSlamTimer;

        void Reset()
        {
			CleaveTimer = 2000;
			ShieldSlamTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, DUNGEON_MODE(SPELL_DULL_WEAPONS_5N, SPELL_DULL_WEAPONS_5H));

			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(3000, 4000);
			}
			else CleaveTimer -= diff;

			if (ShieldSlamTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_SHIELD_SLAM_5N, SPELL_SHIELD_SLAM_5H));
				ShieldSlamTimer = urand(5000, 6000);
			}
			else ShieldSlamTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hardened_steel_reaverAI(creature);
    }
};

class npc_hardened_steel_berserker : public CreatureScript
{
public:
    npc_hardened_steel_berserker() : CreatureScript("npc_hardened_steel_berserker") {}

    struct npc_hardened_steel_berserkerAI : public QuantumBasicAI
    {
        npc_hardened_steel_berserkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 HurlWeaponTimer;
		uint32 WindfuryTimer;

        void Reset()
        {
			HurlWeaponTimer = 500;
			WindfuryTimer = 1000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
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

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HurlWeaponTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_HURL_WEAPON_5N, SPELL_HURL_WEAPON_5H));
					HurlWeaponTimer = urand(3000, 4000);
				}
			}
			else HurlWeaponTimer -= diff;

			if (WindfuryTimer <= diff)
			{
				DoCast(me, SPELL_WINDFURY);
				WindfuryTimer = urand(5000, 6000);
			}
			else WindfuryTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_ENRAGE, 0))
				{
					DoCast(me, SPELL_ENRAGE);
					DoSendQuantumText(SAY_GENERIC_EMOTE_ENRAGE, me);
				}
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hardened_steel_berserkerAI(creature);
    }
};

class npc_hardened_steel_skycaller : public CreatureScript
{
public:
    npc_hardened_steel_skycaller() : CreatureScript("npc_hardened_steel_skycaller") {}

    struct npc_hardened_steel_skycallerAI : public QuantumBasicAI
    {
        npc_hardened_steel_skycallerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ShootTimer;
		uint32 ImpactShotTimer;
		uint32 ImpactMultiShotTimer;
		uint32 DisengageTimer;

        void Reset()
        {
			ShootTimer = 500;
			ImpactShotTimer = 1000;
			ImpactMultiShotTimer = 3000;
			DisengageTimer = 5000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);
		}

		void AttackStart(Unit* who)
        {
			AttackStartNoMove(who);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShootTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, DUNGEON_MODE(SPELL_SHOOT_5N, SPELL_SHOOT_5H));
						ShootTimer = urand(2000, 3000);
					}
				}
			}
			else ShootTimer -= diff;

			if (ImpactShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, DUNGEON_MODE(SPELL_IMPACT_SHOT_5N, SPELL_IMPACT_SHOT_5H));
						ImpactShotTimer = urand(4000, 5000);
					}
				}
			}
			else ImpactShotTimer -= diff;

			if (ImpactMultiShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, DUNGEON_MODE(SPELL_IMPACT_MULTI_SHOT_5N, SPELL_IMPACT_MULTI_SHOT_5H));
						ImpactMultiShotTimer = urand(6000, 7000);
					}
				}
			}
			else ImpactMultiShotTimer -= diff;

			if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    DoStartMovement(me->GetVictim());

				if (DisengageTimer <= diff && IsHeroic()) // Works Only Heroic Mode
				{
					DoCast(SPELL_DISENGAGE);
					DisengageTimer = urand(8000, 9000);
				}
				else DisengageTimer -= diff;

                DoMeleeAttackIfReady();
            }
			else
			{
				if (me->GetDistance2d(me->GetVictim()) > 10)
                    DoStartNoMovement(me->GetVictim());

                if (me->GetDistance2d(me->GetVictim()) > 30)
                    DoStartMovement(me->GetVictim());
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hardened_steel_skycallerAI(creature);
    }
};

class npc_stormforged_mender : public CreatureScript
{
public:
    npc_stormforged_mender() : CreatureScript("npc_stormforged_mender") {}

    struct npc_stormforged_menderAI : public QuantumBasicAI
    {
        npc_stormforged_menderAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 HammerBlowTimer;
		uint32 RenewSteelTimer;

        void Reset()
        {
			HammerBlowTimer = 2000;
			RenewSteelTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HammerBlowTimer <= diff)
			{
				DoCastVictim(SPELL_HAMMER_BLOW);
				HammerBlowTimer = urand(4000, 5000);
			}
			else HammerBlowTimer -= diff;

			if (RenewSteelTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, DUNGEON_MODE(SPELL_RENEW_STEEL_5N, SPELL_RENEW_STEEL_5H));
					RenewSteelTimer = urand(6000, 7000);
				}
			}
			else RenewSteelTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stormforged_menderAI(creature);
    }
};

class npc_stormforged_tactician : public CreatureScript
{
public:
    npc_stormforged_tactician() : CreatureScript("npc_stormforged_tactician") {}

    struct npc_stormforged_tacticianAI : public QuantumBasicAI
    {
        npc_stormforged_tacticianAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 WeldingBeamTimer;
		uint32 ArcWeldTimer;

        void Reset()
        {
			WeldingBeamTimer = 500;
			ArcWeldTimer = 1000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WeldingBeamTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_WELDING_BEAM_5N, SPELL_WELDING_BEAM_5H));
					WeldingBeamTimer = urand(3000, 4000);
				}
			}
			else WeldingBeamTimer -= diff;

			if (ArcWeldTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_ARC_WELD);
					ArcWeldTimer = urand(5000, 6000);
				}
			}
			else ArcWeldTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stormforged_tacticianAI(creature);
    }
};

class npc_stormforged_construct : public CreatureScript
{
public:
    npc_stormforged_construct() : CreatureScript("npc_stormforged_construct") {}

    struct npc_stormforged_constructAI : public QuantumBasicAI
    {
        npc_stormforged_constructAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ForkedLightningTimer;
		uint32 RunicFocusTimer;
		uint32 RunePunchTimer;

        void Reset()
        {
			ForkedLightningTimer = 500;
			RunicFocusTimer = 2000;
			RunePunchTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
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

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ForkedLightningTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FORKED_LIGHTNING_5N, SPELL_FORKED_LIGHTNING_5H));
					ForkedLightningTimer = urand(3000, 4000);
				}
			}
			else ForkedLightningTimer -= diff;

			if (RunicFocusTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_RUNIC_FOCUS_5N, SPELL_RUNIC_FOCUS_5H));
					RunicFocusTimer = urand(5000, 6000);
				}
			}
			else RunicFocusTimer -= diff;

			if (RunePunchTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_RUNE_PUNCH_5N, SPELL_RUNE_PUNCH_5H));
					RunePunchTimer = urand(7000, 8000);
				}
			}
			else RunePunchTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stormforged_constructAI(creature);
    }
};

class npc_stormforged_runeshaper : public CreatureScript
{
public:
    npc_stormforged_runeshaper() : CreatureScript("npc_stormforged_runeshaper") {}

    struct npc_stormforged_runeshaperAI : public QuantumBasicAI
    {
        npc_stormforged_runeshaperAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 StarlingRoarTimer;
		uint32 ChargedFlurryTimer;

        void Reset()
        {
			StarlingRoarTimer = 500;
			ChargedFlurryTimer = 1000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StarlingRoarTimer <= diff)
			{
				DoCastAOE(SPELL_STARLING_ROAR);
				StarlingRoarTimer = 4000;
			}
			else StarlingRoarTimer -= diff;

			if (ChargedFlurryTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_CHARGED_FLURRY_5N, SPELL_CHARGED_FLURRY_5H));
				ChargedFlurryTimer = 7000;
			}
			else ChargedFlurryTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stormforged_runeshaperAI(creature);
    }
};

class npc_stormforged_sentinel : public CreatureScript
{
public:
    npc_stormforged_sentinel() : CreatureScript("npc_stormforged_sentinel") {}

    struct npc_stormforged_sentinelAI : public QuantumBasicAI
    {
        npc_stormforged_sentinelAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 StormingBelowTimer;
		uint32 SleepTimer;
		uint32 RenewTimer;

        void Reset()
        {
			StormingBelowTimer = 500;
			SleepTimer = 1000;
			RenewTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (StormingBelowTimer <= diff)
			{
				DoCast(me, SPELL_STORMING_BELOW);
				StormingBelowTimer = urand(3000, 4000);
			}
			else StormingBelowTimer -= diff;

			if (SleepTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SLEEP_5N, SPELL_SLEEP_5H));
					SleepTimer = urand(5000, 6000);
				}
			}
			else SleepTimer -= diff;

			if (RenewTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, DUNGEON_MODE(SPELL_RENEW_5N, SPELL_RENEW_5H));
					RenewTimer = urand(7000, 8000);
				}
			}
			else RenewTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stormforged_sentinelAI(creature);
    }
};

class npc_stormforged_giant : public CreatureScript
{
public:
    npc_stormforged_giant() : CreatureScript("npc_stormforged_giant") {}

    struct npc_stormforged_giantAI : public QuantumBasicAI
    {
        npc_stormforged_giantAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 StormboltTimer;
		uint32 ThunderStormTimer;
		uint32 SoulStrikeTimer;

        void Reset()
        {
			StormboltTimer = 500;
			ThunderStormTimer = 1500;
			SoulStrikeTimer = 3000;

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

			if (StormboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_STORMBOLT_5N, SPELL_STORMBOLT_5H));
					StormboltTimer = urand(3000, 4000);
				}
			}
			else StormboltTimer -= diff;

			if (ThunderStormTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_THUNDERSTORM_5N, SPELL_THUNDERSTORM_5H));
				ThunderStormTimer = urand(6000, 7000);
			}
			else ThunderStormTimer -= diff;

			if (SoulStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_SOUL_STRIKE);
				SoulStrikeTimer = urand(8000, 9000);
			}
			else SoulStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stormforged_giantAI(creature);
    }
};

class npc_slag : public CreatureScript
{
public:
    npc_slag() : CreatureScript("npc_slag") {}

    struct npc_slagAI : public QuantumBasicAI
    {
        npc_slagAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 BlastWaveTimer;
		uint32 MeltArmorTimer;

        void Reset()
        {
			BlastWaveTimer = 500;
			MeltArmorTimer = 1000;

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

			if (BlastWaveTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_BLAST_WAVE_5N, SPELL_BLAST_WAVE_5H));
				BlastWaveTimer = 4000;
			}
			else BlastWaveTimer -= diff;

			if (MeltArmorTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_MELT_ARMOR_5N, SPELL_MELT_ARMOR_5H));
				MeltArmorTimer = 6000;
			}
			else MeltArmorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_slagAI(creature);
    }
};

class npc_unbound_rirestorm : public CreatureScript
{
public:
    npc_unbound_rirestorm() : CreatureScript("npc_unbound_rirestorm") {}

    struct npc_unbound_rirestormAI : public QuantumBasicAI
    {
        npc_unbound_rirestormAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 LavaBurstTimer;
		uint32 AfterburnTimer;

        void Reset()
        {
			LavaBurstTimer = 500;
			AfterburnTimer = 1000;

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

			if (LavaBurstTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_LAVA_BURST_5N, SPELL_LAVA_BURST_5H));
					LavaBurstTimer = urand(3000, 4000);
				}
			}
			else LavaBurstTimer -= diff;

			if (AfterburnTimer <= diff)
			{
				DoCast(me, DUNGEON_MODE(SPELL_AFTERBURN_5N, SPELL_AFTERBURN_5H), true);
				AfterburnTimer = urand(5000, 6000);
			}
			else AfterburnTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unbound_rirestormAI(creature);
    }
};

class npc_blistering_steamrager : public CreatureScript
{
public:
    npc_blistering_steamrager() : CreatureScript("npc_blistering_steamrager") {}

    struct npc_blistering_steamragerAI : public QuantumBasicAI
    {
        npc_blistering_steamragerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 SteamBlastTimer;

        void Reset()
        {
			SteamBlastTimer = 500;

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

			if (SteamBlastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					me->SetFacingToObject(target);
					DoCast(target, DUNGEON_MODE(SPELL_STEAM_BLAST_5N, SPELL_STEAM_BLAST_5H));
					SteamBlastTimer = 4000;
				}
			}
			else SteamBlastTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_blistering_steamragerAI(creature);
    }
};

class npc_titanium_thunderer : public CreatureScript
{
public:
    npc_titanium_thunderer() : CreatureScript("npc_titanium_thunderer") {}

    struct npc_titanium_thundererAI : public QuantumBasicAI
    {
        npc_titanium_thundererAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ThrowTimer;
		uint32 DeadlyThrowTimer;
		uint32 DeflectionTimer;

        void Reset()
        {
			ThrowTimer = 500;
			DeadlyThrowTimer = 1000;
			DeflectionTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ThrowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_THROW_5N, SPELL_THROW_5H));
					ThrowTimer = urand(3000, 4000);
				}
			}
			else ThrowTimer -= diff;

			if (DeadlyThrowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_DEADLY_THROW_5N, SPELL_DEADLY_THROW_5H));
					DeadlyThrowTimer = urand(5000, 6000);
				}
			}
			else DeadlyThrowTimer -= diff;

			if (DeflectionTimer <= diff)
			{
				DoCast(DUNGEON_MODE(SPELL_DEFLECTION_5N, SPELL_DEFLECTION_5H));
				DeflectionTimer = urand(7000, 8000);
			}
			else DeflectionTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_titanium_thundererAI(creature);
    }
};

class npc_titanium_siegebreaker : public CreatureScript
{
public:
    npc_titanium_siegebreaker() : CreatureScript("npc_titanium_siegebreaker") {}

    struct npc_titanium_siegebreakerAI : public QuantumBasicAI
    {
        npc_titanium_siegebreakerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 PiercingHowlTimer;
		uint32 FrighteningShoutTimer;
		uint32 PenetratingStrikeTimer;
		uint32 BladeTurningTimer;

        void Reset()
        {
			PiercingHowlTimer = 2000;
			FrighteningShoutTimer = 4000;
			PenetratingStrikeTimer = 5000;
			BladeTurningTimer = 6000;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
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

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PiercingHowlTimer <= diff)
			{
				DoCastAOE(SPELL_PIERCING_HOWL);
				PiercingHowlTimer = urand(4000, 5000);
			}
			else PiercingHowlTimer -= diff;

			if (FrighteningShoutTimer <= diff)
			{
				DoCastAOE(SPELL_FRIGHTENING_SHOUT);
				FrighteningShoutTimer = urand(6000, 7000);
			}
			else FrighteningShoutTimer -= diff;

			if (PenetratingStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_PENETRATING_STRIKE);
				PenetratingStrikeTimer = urand(8000, 9000);
			}
			else PenetratingStrikeTimer -= diff;

			if (BladeTurningTimer <= diff)
			{
				DoCast(me, DUNGEON_MODE(SPELL_BLADE_TURNING_5N, SPELL_BLADE_TURNING_5H));
				BladeTurningTimer = urand(10000, 11000);
			}
			else BladeTurningTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_titanium_siegebreakerAI(creature);
    }
};

class npc_titanium_vanguard : public CreatureScript
{
public:
    npc_titanium_vanguard() : CreatureScript("npc_titanium_vanguard") {}

    struct npc_titanium_vanguardAI : public QuantumBasicAI
    {
        npc_titanium_vanguardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ChargeTimer;
		uint32 PoisonTippedSpearTimer;

        void Reset()
        {
			ChargeTimer = 500;
			PoisonTippedSpearTimer = 1500;

			DoCast(me, SPELL_UNIT_DETECTION_WOTLK);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_SPELL_PRECAST);
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

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ChargeTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CHARGE);
					ChargeTimer = urand(4000, 5000);
				}
			}
			else ChargeTimer -= diff;

			if (PoisonTippedSpearTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_POISON_TIPPED_SPEAR_5N, SPELL_POISON_TIPPED_SPEAR_5H));
					PoisonTippedSpearTimer = urand(6000, 7000);
				}
			}
			else PoisonTippedSpearTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_titanium_vanguardAI(creature);
    }
};

class npc_stormfury_revenant : public CreatureScript
{
public:
    npc_stormfury_revenant() : CreatureScript("npc_stormfury_revenant") {}

    struct npc_stormfury_revenantAI : public QuantumBasicAI
    {
        npc_stormfury_revenantAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 ThunderboltTimer;
		uint32 ElectroShockTimer;

        void Reset()
        {
			ThunderboltTimer = 500;
			ElectroShockTimer = 1000;

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

			if (ThunderboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_THUNDERBOLT_5N, SPELL_THUNDERBOLT_5H), true);
					ThunderboltTimer = 4000;
				}
			}
			else ThunderboltTimer -= diff;

			if (ElectroShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_ELECTRO_SHOCK_5N, SPELL_ELECTRO_SHOCK_5H));
					ElectroShockTimer = 5000;
				}
			}
			else ElectroShockTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_stormfury_revenantAI(creature);
    }
};

class npc_storming_vortex : public CreatureScript
{
public:
    npc_storming_vortex() : CreatureScript("npc_storming_vortex") {}

    struct npc_storming_vortexAI : public QuantumBasicAI
    {
        npc_storming_vortexAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
		}

		uint32 LightningBoltTimer;
		uint32 CycloneTimer;

        void Reset()
        {
			LightningBoltTimer = 500;
			CycloneTimer = 1000;

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

			if (LightningBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_LIGHTNING_BOLT_5N, SPELL_LIGHTNING_BOLT_5H));
					LightningBoltTimer = urand(3000, 4000);
				}
			}
			else LightningBoltTimer -= diff;

			if (CycloneTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CYCLONE);
					CycloneTimer = urand(5000, 6000);
				}
			}
			else CycloneTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_storming_vortexAI(creature);
    }
};

void AddSC_halls_of_lightning_trash()
{
	new npc_hardened_steel_reaver();
	new npc_hardened_steel_berserker();
	new npc_hardened_steel_skycaller();
	new npc_stormforged_mender();
	new npc_stormforged_tactician();
	new npc_stormforged_construct();
	new npc_stormforged_runeshaper();
	new npc_stormforged_sentinel();
	new npc_stormforged_giant();
	new npc_slag();
	new npc_unbound_rirestorm();
	new npc_blistering_steamrager();
	new npc_titanium_thunderer();
	new npc_titanium_siegebreaker();
	new npc_titanium_vanguard();
	new npc_stormfury_revenant();
	new npc_storming_vortex();
}