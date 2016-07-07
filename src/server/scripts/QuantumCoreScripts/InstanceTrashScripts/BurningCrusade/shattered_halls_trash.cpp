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
#include "../../../scripts/Outland/HellfireCitadel/ShatteredHalls/shattered_halls.h"

enum ShatteredHallsSpells
{
	SPELL_DUAL_WIELD              = 674,
	SPELL_BLOODTHIRST_5N          = 30474,
	SPELL_BLOODTHIRST_5H          = 35949,
	SPELL_ENRAGE                  = 30485,
	SPELL_DEATHBLOW_5N            = 36023,
	SPELL_DEATHBLOW_5H            = 36054,
	SPELL_SLICE_AND_DICE          = 30470,
	SPELL_AURA_OF_DISCIPLINE      = 30472,
	SPELL_PUMMEL                  = 15615,
	SPELL_UPPERCUT                = 30471,
	SPELL_CLEAVE                  = 15496,
	SPELL_FEAR                    = 12542,
	SPELL_SHADOW_BOLT_5N          = 12471,
	SPELL_SHADOW_BOLT_5H          = 15232,
	SPELL_RAIN_OF_FIRE_5N         = 11990,
	SPELL_RAIN_OF_FIRE_5H         = 33508,
	SPELL_RESIST_SHADOW           = 30479,
	SPELL_MIND_BLAST_5N           = 31516,
	SPELL_MIND_BLAST_5H           = 15587,
	SPELL_POWER_WORD_SHIELD_5N    = 35944,
	SPELL_POWER_WORD_SHIELD_5H    = 36052,
	SPELL_PRAYER_OF_HEALING_5N    = 15585,
	SPELL_PRAYER_OF_HEALING_5H    = 35943,
	SPELL_SHOOT_5N                = 16100,
	SPELL_SHOOT_5H                = 22907,
	SPELL_MULTI_SHOT              = 30990,
	SPELL_SH_SHOOT_5N             = 15620,
	SPELL_SH_SHOOT_5H             = 22907,
	SPELL_INCENDIARY_SHOT_5N      = 30481,
	SPELL_INCENDIARY_SHOT_5H      = 35945,
	SPELL_SCATTER_SHOT            = 23601,
	SPELL_VIPER_STING             = 37551,
	SPELL_CURSE_OF_SHATTERED_HAND = 36020,
	SPELL_KICK                    = 36033,
	SPELL_THRASH                  = 3417,
	SPELL_IMPALING_BOLT_5N        = 30932,
	SPELL_IMPALING_BOLT_5H        = 40248,
	SPELL_VOLLEY_5N               = 34100,
	SPELL_VOLLEY_5H               = 35950,
	SPELL_CARNIVOROUS_BITE_5N     = 30639,
	SPELL_CARNIVOROUS_BITE_5H     = 32901,
	SPELL_FURIOUS_HOWL_5N         = 30636,
	SPELL_FURIOUS_HOWL_5H         = 35942,
	SPELL_SHIELD_BLOCK            = 32587,
	SPELL_CONCUSSION_BLOW         = 32588,
	SPELL_SHIELD_BASH             = 11972,
	SPELL_MORTAL_STRIKE           = 16856,
	SPELL_BATTLE_SHOUT_5N         = 30931,
	SPELL_BATTLE_SHOUT_5H         = 31403,
	SPELL_SUNDER_ARMOR_5N         = 15572,
	SPELL_SUNDER_ARMOR_5H         = 16145,
	SPELL_CHARGE_5N               = 22911,
	SPELL_CHARGE_5H               = 37511,
	SPELL_HAMSTRING               = 31553,
	SPELL_STEALTH                 = 30991,
	SPELL_SAP                     = 30980,
	SPELL_CHEAP_SHOT              = 30986,
	SPELL_BACKSTAB                = 30992,
	SPELL_CRIPPLING_POISON        = 30981,
	SPELL_WOUND_POISON            = 36974,
};

class npc_shattered_hand_heathen : public CreatureScript
{
public:
    npc_shattered_hand_heathen() : CreatureScript("npc_shattered_hand_heathen") {}

    struct npc_shattered_hand_heathenAI : public QuantumBasicAI
    {
        npc_shattered_hand_heathenAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 BloodthirstTimer;

        void Reset()
		{
			BloodthirstTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BloodthirstTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_BLOODTHIRST_5N, SPELL_BLOODTHIRST_5H));
				BloodthirstTimer = 5000;
			}
			else BloodthirstTimer -= diff;

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
        return new npc_shattered_hand_heathenAI(creature);
    }
};

class npc_shattered_hand_savage : public CreatureScript
{
public:
    npc_shattered_hand_savage() : CreatureScript("npc_shattered_hand_savage") {}

    struct npc_shattered_hand_savageAI : public QuantumBasicAI
    {
        npc_shattered_hand_savageAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 DeathblowTimer;
		uint32 SliceAndDiceTimer;

        void Reset()
		{
			SliceAndDiceTimer = 1000;
			DeathblowTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (DeathblowTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_DEATHBLOW_5N, SPELL_DEATHBLOW_5H));
				DeathblowTimer = 4000;
			}
			else DeathblowTimer -= diff;

			if (SliceAndDiceTimer <= diff)
			{
				DoCast(me, SPELL_SLICE_AND_DICE);
				SliceAndDiceTimer = 6000;
			}
			else SliceAndDiceTimer -= diff;

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
        return new npc_shattered_hand_savageAI(creature);
    }
};

class npc_shattered_hand_legionnaire : public CreatureScript
{
public:
    npc_shattered_hand_legionnaire() : CreatureScript("npc_shattered_hand_legionnaire") {}

    struct npc_shattered_hand_legionnaireAI : public QuantumBasicAI
    {
        npc_shattered_hand_legionnaireAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 PummelTimer;

        void Reset()
		{
			PummelTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_DUAL_WIELD, true);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_AURA_OF_DISCIPLINE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PummelTimer <= diff)
			{
				DoCastVictim(SPELL_PUMMEL);
				PummelTimer = 5000;
			}
			else PummelTimer -= diff;

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
        return new npc_shattered_hand_legionnaireAI(creature);
    }
};

class npc_shattered_hand_reaver : public CreatureScript
{
public:
    npc_shattered_hand_reaver() : CreatureScript("npc_shattered_hand_reaver") {}

    struct npc_shattered_hand_reaverAI : public QuantumBasicAI
    {
        npc_shattered_hand_reaverAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 UppercutTimer;
		uint32 CleaveTimer;

        void Reset()
		{
			UppercutTimer = 2000;
			CleaveTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (UppercutTimer <= diff)
			{
				DoCastVictim(SPELL_UPPERCUT);
				UppercutTimer = 3000;
			}
			else UppercutTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = 5000;
			}
			else CleaveTimer -= diff;

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
        return new npc_shattered_hand_reaverAI(creature);
    }
};

class npc_shattered_hand_archer : public CreatureScript
{
public:
    npc_shattered_hand_archer() : CreatureScript("npc_shattered_hand_archer") {}

    struct npc_shattered_hand_archerAI : public QuantumBasicAI
    {
        npc_shattered_hand_archerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShootTimer;
		uint32 MultiShotTimer;

        void Reset()
        {
			ShootTimer = 500;
			MultiShotTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

		void EnterToBattle(Unit* /*who*/){}

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
					DoCast(target, DUNGEON_MODE(SPELL_SHOOT_5N, SPELL_SHOOT_5H));
					ShootTimer = urand(2000, 3000);
				}
			}
			else ShootTimer -= diff;

			if (MultiShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MULTI_SHOT);
					MultiShotTimer = urand(4000, 5000);
				}
			}
			else MultiShotTimer -= diff;

			if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    DoStartMovement(me->GetVictim());

                DoMeleeAttackIfReady();
            }
			else
			{
				if (me->GetDistance2d(me->GetVictim()) > 10)
                    DoStartNoMovement(me->GetVictim());

                if (me->GetDistance2d(me->GetVictim()) > 30)
                    DoStartMovement(me->GetVictim());

				if (ShootTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, DUNGEON_MODE(SPELL_SHOOT_5N, SPELL_SHOOT_5H));
						ShootTimer = urand(9000, 7000);
					}
				}
				else ShootTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shattered_hand_archerAI(creature);
    }
};

class npc_shattered_hand_sharpshooter : public CreatureScript
{
public:
    npc_shattered_hand_sharpshooter() : CreatureScript("npc_shattered_hand_sharpshooter") {}

    struct npc_shattered_hand_sharpshooterAI : public QuantumBasicAI
    {
        npc_shattered_hand_sharpshooterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShootTimer;
		uint32 IncendiaryShotTimer;
		uint32 ScatterShotTimer;
		uint32 ViperStingTimer;

        void Reset()
        {
			ShootTimer = 500;
			IncendiaryShotTimer = 1000;
			ScatterShotTimer = 3000;
			ViperStingTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_RIFLE);
        }

		void EnterToBattle(Unit* /*who*/){}

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
					DoCast(target, DUNGEON_MODE(SPELL_SH_SHOOT_5N, SPELL_SH_SHOOT_5H));
					ShootTimer = urand(2000, 3000);
				}
			}
			else ShootTimer -= diff;

			if (IncendiaryShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_INCENDIARY_SHOT_5N, SPELL_INCENDIARY_SHOT_5H));
					IncendiaryShotTimer = urand(4000, 5000);
				}
			}
			else IncendiaryShotTimer -= diff;

			if (ScatterShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SCATTER_SHOT);
					ScatterShotTimer = urand(6000, 7000);
				}
			}
			else ScatterShotTimer -= diff;

			if (ViperStingTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_VIPER_STING);
						ViperStingTimer = urand(8000, 9000);
					}
				}
			}
			else ViperStingTimer -= diff;

			if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    DoStartMovement(me->GetVictim());

                DoMeleeAttackIfReady();
            }
			else
			{
				if (me->GetDistance2d(me->GetVictim()) > 10)
                    DoStartNoMovement(me->GetVictim());

                if (me->GetDistance2d(me->GetVictim()) > 30)
                    DoStartMovement(me->GetVictim());

				if (ShootTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, DUNGEON_MODE(SPELL_SHOOT_5N, SPELL_SHOOT_5H));
						ShootTimer = urand(10000, 11000);
					}
				}
				else ShootTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shattered_hand_sharpshooterAI(creature);
    }
};

class npc_shattered_hand_brawler : public CreatureScript
{
public:
    npc_shattered_hand_brawler() : CreatureScript("npc_shattered_hand_brawler") {}

    struct npc_shattered_hand_brawlerAI : public QuantumBasicAI
    {
        npc_shattered_hand_brawlerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 CurseOfShatteredHandTimer;
		uint32 KickTimer;

        void Reset()
		{
			CurseOfShatteredHandTimer = 500;
			KickTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_THRASH);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CurseOfShatteredHandTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_CURSE_OF_SHATTERED_HAND);
					CurseOfShatteredHandTimer = 4000;
				}
			}
			else CurseOfShatteredHandTimer -= diff;

			if (KickTimer <= diff)
			{
				DoCastVictim(SPELL_KICK);
				KickTimer = 5000;
			}
			else KickTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shattered_hand_brawlerAI(creature);
    }
};

class npc_shattered_hand_houndmaster : public CreatureScript
{
public:
    npc_shattered_hand_houndmaster() : CreatureScript("npc_shattered_hand_houndmaster") {}

    struct npc_shattered_hand_houndmasterAI : public QuantumBasicAI
    {
        npc_shattered_hand_houndmasterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShootTimer;
		uint32 ImpalingBoltTimer;
		uint32 VolleyTimer;

        void Reset()
        {
			ShootTimer = 500;
			ImpalingBoltTimer = 2000;
			VolleyTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_RIFLE);
        }

		void EnterToBattle(Unit* /*who*/){}

		void AttackStart(Unit* who)
        {
			AttackStartNoMove(who);
        }

		void JustDied(Unit* /*killer*/)
		{
			if (GameObject* door = me->FindGameobjectWithDistance(GO_GRAND_WARLOCK_CHAMBER_DOOR_ENTRANCE, 250.0f))
				door->UseDoorOrButton();
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
					DoCast(target, DUNGEON_MODE(SPELL_SH_SHOOT_5N, SPELL_SH_SHOOT_5H));
					ShootTimer = urand(2000, 3000);
				}
			}
			else ShootTimer -= diff;

			if (ImpalingBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_IMPALING_BOLT_5N, SPELL_IMPALING_BOLT_5H));
					ImpalingBoltTimer = urand(4000, 5000);
				}
			}
			else ImpalingBoltTimer -= diff;

			if (VolleyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_VOLLEY_5N, SPELL_VOLLEY_5H));
					VolleyTimer = urand(7000, 8000);
				}
			}
			else VolleyTimer -= diff;

			if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    DoStartMovement(me->GetVictim());

                DoMeleeAttackIfReady();
            }
			else
			{
				if (me->GetDistance2d(me->GetVictim()) > 10)
                    DoStartNoMovement(me->GetVictim());

                if (me->GetDistance2d(me->GetVictim()) > 30)
                    DoStartMovement(me->GetVictim());

				if (ShootTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, DUNGEON_MODE(SPELL_SHOOT_5N, SPELL_SHOOT_5H));
						ShootTimer = urand(6000, 7000);
					}
				}
				else ShootTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shattered_hand_houndmasterAI(creature);
    }
};

class npc_shattered_hand_champion : public CreatureScript
{
public:
    npc_shattered_hand_champion() : CreatureScript("npc_shattered_hand_champion") {}

    struct npc_shattered_hand_championAI : public QuantumBasicAI
    {
        npc_shattered_hand_championAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShieldBlockTimer;
		uint32 ConcussionBlowTimer;
		uint32 ShieldBashTimer;

        void Reset()
		{
			ShieldBlockTimer = 1000;
			ConcussionBlowTimer = 2000;
			ShieldBashTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_1H);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShieldBlockTimer <= diff)
			{
				DoCast(me, SPELL_SHIELD_BLOCK);
				ShieldBlockTimer = 4000;
			}
			else ShieldBlockTimer -= diff;

			if (ConcussionBlowTimer <= diff)
			{
				DoCastVictim(SPELL_CONCUSSION_BLOW);
				ConcussionBlowTimer = 5000;
			}
			else ConcussionBlowTimer -= diff;

			if (ShieldBashTimer <= diff)
			{
				DoCastVictim(SPELL_SHIELD_BASH);
				ShieldBashTimer = 6000;
			}
			else ShieldBashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shattered_hand_championAI(creature);
    }
};

class npc_shattered_hand_blood_guard : public CreatureScript
{
public:
    npc_shattered_hand_blood_guard() : CreatureScript("npc_shattered_hand_blood_guard") {}

    struct npc_shattered_hand_blood_guardAI : public QuantumBasicAI
    {
        npc_shattered_hand_blood_guardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 UppercutTimer;
		uint32 CleaveTimer;

        void Reset()
		{
			UppercutTimer = 2000;
			CleaveTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (UppercutTimer <= diff)
			{
				DoCastVictim(SPELL_UPPERCUT);
				UppercutTimer = 3000;
			}
			else UppercutTimer -= diff;

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = 5000;
			}
			else CleaveTimer -= diff;

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
        return new npc_shattered_hand_blood_guardAI(creature);
    }
};

class npc_shattered_hand_gladiator : public CreatureScript
{
public:
    npc_shattered_hand_gladiator() : CreatureScript("npc_shattered_hand_gladiator") {}

    struct npc_shattered_hand_gladiatorAI : public QuantumBasicAI
    {
        npc_shattered_hand_gladiatorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 MortalStrikeTimer;

        void Reset()
		{
			MortalStrikeTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_MORTAL_STRIKE);
				MortalStrikeTimer = 4000;
			}
			else MortalStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shattered_hand_gladiatorAI(creature);
    }
};

class npc_shattered_hand_centurion : public CreatureScript
{
public:
    npc_shattered_hand_centurion() : CreatureScript("npc_shattered_hand_centurion") {}

    struct npc_shattered_hand_centurionAI : public QuantumBasicAI
    {
        npc_shattered_hand_centurionAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 BallteShoutTimer;
		uint32 SunderArmorTimer;

        void Reset()
		{
			BallteShoutTimer = 1000;
			SunderArmorTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BallteShoutTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_BATTLE_SHOUT_5N, SPELL_BATTLE_SHOUT_5H));
				BallteShoutTimer = 3000;
			}
			else BallteShoutTimer -= diff;

			if (SunderArmorTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_SUNDER_ARMOR_5N, SPELL_SUNDER_ARMOR_5H));
				SunderArmorTimer = 4000;
			}
			else SunderArmorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shattered_hand_centurionAI(creature);
    }
};

class npc_shattered_hand_sentry : public CreatureScript
{
public:
    npc_shattered_hand_sentry() : CreatureScript("npc_shattered_hand_sentry") {}

    struct npc_shattered_hand_sentryAI : public QuantumBasicAI
    {
        npc_shattered_hand_sentryAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 HamstringTimer;

        void Reset()
		{
			HamstringTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCastVictim(DUNGEON_MODE(SPELL_CHARGE_5N, SPELL_CHARGE_5H));
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HamstringTimer <= diff)
			{
				DoCastVictim(SPELL_HAMSTRING);
				HamstringTimer = 4000;
			}
			else HamstringTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shattered_hand_sentryAI(creature);
    }
};

class npc_shattered_hand_zealot : public CreatureScript
{
public:
    npc_shattered_hand_zealot() : CreatureScript("npc_shattered_hand_zealot") {}

    struct npc_shattered_hand_zealotAI : public QuantumBasicAI
    {
        npc_shattered_hand_zealotAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 HamstringTimer;

        void Reset()
		{
			HamstringTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (HamstringTimer <= diff)
			{
				DoCastVictim(SPELL_HAMSTRING);
				HamstringTimer = 4000;
			}
			else HamstringTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shattered_hand_zealotAI(creature);
    }
};

class npc_shattered_hand_assassin : public CreatureScript
{
public:
    npc_shattered_hand_assassin() : CreatureScript("npc_shattered_hand_assassin") {}

    struct npc_shattered_hand_assassinAI : public QuantumBasicAI
    {
        npc_shattered_hand_assassinAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 BackstabTimer;
		uint32 PoisonTimer;

        void Reset()
		{
			BackstabTimer = 2000;
			PoisonTimer = 3000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_STEALTH);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			DoCast(who, SPELL_CHEAP_SHOT, true);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BackstabTimer <= diff)
			{
				DoCastVictim(SPELL_BACKSTAB);
				BackstabTimer = 4000;
			}
			else BackstabTimer -= diff;

			if (PoisonTimer <= diff)
			{
				DoCastVictim(SPELL_WOUND_POISON, true);
				DoCastVictim(SPELL_CRIPPLING_POISON, true);
				PoisonTimer = 5000;
			}
			else PoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shattered_hand_assassinAI(creature);
    }
};

class npc_shadowmoon_darkcaster : public CreatureScript
{
public:
    npc_shadowmoon_darkcaster() : CreatureScript("npc_shadowmoon_darkcaster") {}

    struct npc_shadowmoon_darkcasterAI : public QuantumBasicAI
    {
        npc_shadowmoon_darkcasterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FearTimer;
		uint32 ShadowBoltTimer;
		uint32 RainOfFireTimer;

        void Reset()
		{
			FearTimer = 1000;
			ShadowBoltTimer = 3000;
			RainOfFireTimer = 6000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FearTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FEAR);
					FearTimer = 8000;
				}
			}
			else FearTimer -= diff;

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BOLT_5N, SPELL_SHADOW_BOLT_5H));
					ShadowBoltTimer = urand(3000, 4000);
				}
			}
			else ShadowBoltTimer -= diff;

			if (RainOfFireTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_RAIN_OF_FIRE_5N, SPELL_RAIN_OF_FIRE_5H));
					RainOfFireTimer = urand(6000, 7000);
				}
			}
			else RainOfFireTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadowmoon_darkcasterAI(creature);
    }
};

class npc_shadowmoon_acolyte : public CreatureScript
{
public:
    npc_shadowmoon_acolyte() : CreatureScript("npc_shadowmoon_acolyte") {}

    struct npc_shadowmoon_acolyteAI : public QuantumBasicAI
    {
        npc_shadowmoon_acolyteAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 MindBlastTimer;
		uint32 PowerWordShieldTimer;
		uint32 PrayerOfHealingTimer;

        void Reset()
		{
			MindBlastTimer = 1000;
			PowerWordShieldTimer = 3000;
			PrayerOfHealingTimer = 6000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;
			
			// Resist Shadow Buff Check in Combat
			if (!me->HasAura(SPELL_RESIST_SHADOW))
				DoCast(me, SPELL_RESIST_SHADOW);

			if (MindBlastTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_MIND_BLAST_5N, SPELL_MIND_BLAST_5H));
					MindBlastTimer = urand(3000, 4000);
				}
			}
			else MindBlastTimer -= diff;

			if (PowerWordShieldTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_FULL))
				{
					DoCast(target, DUNGEON_MODE(SPELL_POWER_WORD_SHIELD_5N, SPELL_POWER_WORD_SHIELD_5H));
					PowerWordShieldTimer = urand(5000, 6000);
				}
			}
			else PowerWordShieldTimer -= diff;

			if (PrayerOfHealingTimer <= diff)
			{
				DoCast(DUNGEON_MODE(SPELL_PRAYER_OF_HEALING_5N, SPELL_PRAYER_OF_HEALING_5H));
				PrayerOfHealingTimer = urand(8000, 9000);
			}
			else PrayerOfHealingTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_shadowmoon_acolyteAI(creature);
    }
};

class npc_sharpshooter_guard : public CreatureScript
{
public:
    npc_sharpshooter_guard() : CreatureScript("npc_sharpshooter_guard") {}

    struct npc_sharpshooter_guardAI : public QuantumBasicAI
    {
        npc_sharpshooter_guardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShootTimer;
		uint32 IncendiaryShotTimer;
		uint32 ScatterShotTimer;
		uint32 ViperStingTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
			me->SetUInt32Value(UNIT_NPC_EMOTE_STATE, EMOTE_STATE_READY_RIFLE);

			ShootTimer = 500;
			IncendiaryShotTimer = 2000;
			ScatterShotTimer = 4000;
			ViperStingTimer = 5000;
        }

		void EnterToBattle(Unit* /*who*/){}

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
					DoCast(target, DUNGEON_MODE(SPELL_SH_SHOOT_5N, SPELL_SH_SHOOT_5H));
					ShootTimer = urand(2000, 3000);
				}
			}
			else ShootTimer -= diff;

			if (IncendiaryShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_INCENDIARY_SHOT_5N, SPELL_INCENDIARY_SHOT_5H));
					IncendiaryShotTimer = urand(4000, 5000);
				}
			}
			else IncendiaryShotTimer -= diff;

			if (ScatterShotTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SCATTER_SHOT);
					ScatterShotTimer = urand(6000, 7000);
				}
			}
			else ScatterShotTimer -= diff;

			if (ViperStingTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_VIPER_STING);
						ViperStingTimer = urand(8000, 9000);
					}
				}
			}
			else ViperStingTimer -= diff;

			if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    DoStartMovement(me->GetVictim());

                DoMeleeAttackIfReady();
            }
			else
			{
				if (me->GetDistance2d(me->GetVictim()) > 10)
                    DoStartNoMovement(me->GetVictim());

                if (me->GetDistance2d(me->GetVictim()) > 30)
                    DoStartMovement(me->GetVictim());

				if (ShootTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, DUNGEON_MODE(SPELL_SHOOT_5N, SPELL_SHOOT_5H));
						ShootTimer = urand(10000, 11000);
					}
				}
				else ShootTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sharpshooter_guardAI(creature);
    }
};

class npc_heathen_guard : public CreatureScript
{
public:
    npc_heathen_guard() : CreatureScript("npc_heathen_guard") {}

    struct npc_heathen_guardAI : public QuantumBasicAI
    {
        npc_heathen_guardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 BloodthirstTimer;

        void Reset()
		{
			BloodthirstTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BloodthirstTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_BLOODTHIRST_5N, SPELL_BLOODTHIRST_5H));
				BloodthirstTimer = 5000;
			}
			else BloodthirstTimer -= diff;

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
        return new npc_heathen_guardAI(creature);
    }
};

class npc_rabid_warhound : public CreatureScript
{
public:
    npc_rabid_warhound() : CreatureScript("npc_rabid_warhound") {}

    struct npc_rabid_warhoundAI : public QuantumBasicAI
    {
        npc_rabid_warhoundAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FuriousHowlTimer;
		uint32 CarnivorousBiteTimer;

        void Reset()
		{
			FuriousHowlTimer = 1000;
			CarnivorousBiteTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (CarnivorousBiteTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_CARNIVOROUS_BITE_5N, SPELL_CARNIVOROUS_BITE_5H));
				CarnivorousBiteTimer = 4000;
			}
			else CarnivorousBiteTimer -= diff;

			if (FuriousHowlTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_FURIOUS_HOWL_5N, SPELL_FURIOUS_HOWL_5H));
				FuriousHowlTimer = 8000;
			}
			else FuriousHowlTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_rabid_warhoundAI(creature);
    }
};

void AddSC_shattered_halls_trash()
{
	new npc_shattered_hand_heathen();
	new npc_shattered_hand_savage();
	new npc_shattered_hand_legionnaire();
	new npc_shattered_hand_reaver();
	new npc_shattered_hand_archer();
	new npc_shattered_hand_sharpshooter();
	new npc_shattered_hand_brawler();
	new npc_shattered_hand_houndmaster();
	new npc_shattered_hand_champion();
	new npc_shattered_hand_blood_guard();
	new npc_shattered_hand_gladiator();
	new npc_shattered_hand_centurion();
	new npc_shattered_hand_sentry();
	new npc_shattered_hand_zealot();
	new npc_shattered_hand_assassin();
	new npc_shadowmoon_darkcaster();
	new npc_shadowmoon_acolyte();
	new npc_sharpshooter_guard();
	new npc_heathen_guard();
	new npc_rabid_warhound();
}