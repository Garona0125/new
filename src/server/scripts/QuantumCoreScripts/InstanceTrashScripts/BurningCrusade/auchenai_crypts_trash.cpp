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

enum AuchenaiCryptsSpells
{
	SPELL_GHOST_VISUAL                = 35838,
	SPELL_FIREBALL_5N                 = 12466,
	SPELL_FIREBALL_5H                 = 17290,
	SPELL_FROSTBOLT_5N                = 15043,
	SPELL_FROSTBOLT_5H                = 15530,
	SPELL_BLAST_WAVE_5N               = 15744,
	SPELL_BLAST_WAVE_5H               = 22424,
	SPELL_SHOOT_5N                    = 15547,
	SPELL_SHOOT_5H                    = 16100,
	SPELL_SERPENT_STING_5N            = 31975,
	SPELL_SERPENT_STING_5H            = 35511,
	SPELL_VIPER_STING                 = 37551,
	SPELL_SPIRIT_VENGEANCE            = 32829,
	SPELL_SHIELD_BASH                 = 11972,
	SPELL_PROTECTION_AURA             = 32828,
	SPELL_MAJOR_HEAL                  = 33324,
	SPELL_HEAL                        = 22883,
	SPELL_RENEW_5N                    = 25058,
	SPELL_RENEW_5H                    = 38210,
	SPELL_SHADOW_SHOCK_5N             = 17439,
	SPELL_SHADOW_SHOCK_5H             = 17289,
	SPELL_SHADOWGUARD_5N              = 32861,
	SPELL_SHADOWGUARD_5H              = 38379,
	SPELL_COUNTER_KICK                = 32846,
	SPELL_CYCLONE_STRIKE              = 32849,
	SPELL_OVERPOWER                   = 37321,
	SPELL_SPIRITUAL_SIGHT             = 38168,
	SPELL_FALTER                      = 32859,
	SPELL_SHADOW_BOLT_5N              = 32860,
	SPELL_SHADOW_BOLT_5H              = 38378,
	SPELL_TOUCH_OF_THE_FORGOTTEN_5N   = 32858,
	SPELL_TOUCH_OF_THE_FORGOTTEN_5H   = 38377,
	SPELL_DRAIN_SOUL                  = 35839,
	SPELL_SEED_OF_CORRUPTION_5N       = 32863,
	SPELL_SEED_OF_CORRUPTION_5H       = 38252,
	SPELL_SHADOW_MEND                 = 33325,
	SPELL_STRIKE                      = 13584,
	SPELL_SUNDER_ARMOR                = 13444,
	SPELL_CLEAVE                      = 20666,
	SPELL_MORTAL_STRIKE               = 29572,
	SPELL_INFURIATED                  = 32886,
	SPELL_POSSES                      = 33401,

	SPELL_SUMMON_UNLIVING_SOLDIER     = 32853,
	SPELL_SUMMON_UNLIVING_SORCERER    = 32854,
	SPELL_SUMMON_UNLIVING_CLERIC      = 32855,
	SPELL_SUMMON_UNLIVING_STALKER     = 32856,
	SPELL_SUMMON_PHANTASMAL_POSSESSOR = 32857,
};

enum Texts
{
	SAY_AUCHENAI_AGGRO_1 = -1420062,
	SAY_AUCHENAI_AGGRO_2 = -1420063,
	SAY_AUCHENAI_AGGRO_3 = -1420064,
	SAY_AUCHENAI_AGGRO_4 = -1420065,
};

enum Creatures
{
	NPC_UNLIVING_SOLDIER     = 18498,
	NPC_UNLIVING_SORCERER    = 18499,
	NPC_UNLIVING_CLERIC      = 18500,
	NPC_UNLIVING_STALKER     = 18501,
	NPC_PHANTASMAL_POSSESSOR = 18503,
};

class npc_phasing_sorcerer : public CreatureScript
{
public:
    npc_phasing_sorcerer() : CreatureScript("npc_phasing_sorcerer") {}

    struct npc_phasing_sorcererAI : public QuantumBasicAI
    {
        npc_phasing_sorcererAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FireballTimer;
		uint32 FrostboltTimer;
		uint32 BlastWaveTimer;

        void Reset()
		{
			FireballTimer = 0.5*IN_MILLISECONDS;
			FrostboltTimer = 3*IN_MILLISECONDS;
			BlastWaveTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_GHOST_VISUAL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FIREBALL_5N, SPELL_FIREBALL_5H));
					FireballTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else FireballTimer -= diff;

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FROSTBOLT_5N, SPELL_FROSTBOLT_5H));
					FrostboltTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			if (BlastWaveTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_BLAST_WAVE_5N, SPELL_BLAST_WAVE_5H));
				BlastWaveTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else BlastWaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_phasing_sorcererAI(creature);
    }
};

class npc_phasing_stalker : public CreatureScript
{
public:
    npc_phasing_stalker() : CreatureScript("npc_phasing_stalker") {}

    struct npc_phasing_stalkerAI : public QuantumBasicAI
    {
        npc_phasing_stalkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShootTimer;
		uint32 SerpentStingTimer;
		uint32 ViperStingTimer;

        void Reset()
        {
			ShootTimer = 1*IN_MILLISECONDS;
			SerpentStingTimer = 2*IN_MILLISECONDS;
			ViperStingTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_GHOST_VISUAL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
					ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
				}
			}
			else ShootTimer -= diff;

			if (SerpentStingTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SERPENT_STING_5N, SPELL_SERPENT_STING_5H));
					SerpentStingTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else SerpentStingTimer -= diff;

			if (ViperStingTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_VIPER_STING);
						ViperStingTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
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
						ShootTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
					}
				}
				else ShootTimer -= diff;
			}

			if (HealthBelowPct(HEALTH_PERCENT_50))
				DoCast(me, SPELL_SPIRIT_VENGEANCE);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_phasing_stalkerAI(creature);
    }
};

class npc_phasing_soldier : public CreatureScript
{
public:
    npc_phasing_soldier() : CreatureScript("npc_phasing_soldier") {}

    struct npc_phasing_soldierAI : public QuantumBasicAI
    {
        npc_phasing_soldierAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShieldBashTimer;

        void Reset()
        {
			ShieldBashTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_GHOST_VISUAL);
			DoCast(me, SPELL_PROTECTION_AURA);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShieldBashTimer <= diff)
			{
				DoCastVictim(SPELL_SHIELD_BASH);
				ShieldBashTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else ShieldBashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_phasing_soldierAI(creature);
    }
};

class npc_phasing_cleric : public CreatureScript
{
public:
    npc_phasing_cleric() : CreatureScript("npc_phasing_cleric") {}

    struct npc_phasing_clericAI : public QuantumBasicAI
    {
        npc_phasing_clericAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 RenewTimer;
		uint32 MajorHealTimer;
		uint32 HealTimer;

        void Reset()
		{
			RenewTimer = 1*IN_MILLISECONDS;
			MajorHealTimer = 2*IN_MILLISECONDS;
			HealTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_GHOST_VISUAL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RenewTimer <= diff)
			{
				DoCast(me, DUNGEON_MODE(SPELL_RENEW_5N, SPELL_RENEW_5H));
				RenewTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else RenewTimer -= diff;

			if (MajorHealTimer <= diff)
			{
				DoCast(me, SPELL_MAJOR_HEAL);
				MajorHealTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else MajorHealTimer -= diff;

			if (HealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, SPELL_HEAL);
					HealTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
				}
			}
			else HealTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
				DoCast(me, SPELL_HEAL);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_phasing_clericAI(creature);
    }
};

class npc_auchenai_vindicator : public CreatureScript
{
public:
    npc_auchenai_vindicator() : CreatureScript("npc_auchenai_vindicator") {}

    struct npc_auchenai_vindicatorAI : public QuantumBasicAI
    {
        npc_auchenai_vindicatorAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShadowguardTimer;
		uint32 ShadowShockTimer;

		SummonList Summons;

        void Reset()
		{
			ShadowguardTimer = 0.5*IN_MILLISECONDS;
			ShadowShockTimer = 1*IN_MILLISECONDS;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			DoCast(me, SPELL_SUMMON_UNLIVING_SOLDIER, true);
			DoCast(me, SPELL_SUMMON_UNLIVING_SORCERER, true);
			DoCast(me, SPELL_SUMMON_UNLIVING_CLERIC, true);
			DoCast(me, SPELL_SUMMON_UNLIVING_STALKER, true);
			DoCast(me, SPELL_SUMMON_PHANTASMAL_POSSESSOR, true);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_AUCHENAI_AGGRO_1, SAY_AUCHENAI_AGGRO_2, SAY_AUCHENAI_AGGRO_3, SAY_AUCHENAI_AGGRO_4), me);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			switch (summon->GetEntry())
			{
                case NPC_UNLIVING_SOLDIER:
				case NPC_UNLIVING_SORCERER:
				case NPC_UNLIVING_CLERIC:
				case NPC_UNLIVING_STALKER:
				case NPC_PHANTASMAL_POSSESSOR:
					Summons.Summon(summon);
					Summons.DoZoneInCombat();
					break;
				default:
					break;
			}
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (ShadowguardTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, DUNGEON_MODE(SPELL_SHADOWGUARD_5N, SPELL_SHADOWGUARD_5H));
				ShadowguardTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else ShadowguardTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Shadowguard Buff Check in Combat
			if (!me->HasAura(DUNGEON_MODE(SPELL_SHADOWGUARD_5N, SPELL_SHADOWGUARD_5H)))
				DoCast(me, DUNGEON_MODE(SPELL_SHADOWGUARD_5N, SPELL_SHADOWGUARD_5H), true);

			if (ShadowShockTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_SHOCK_5N, SPELL_SHADOW_SHOCK_5H));
					ShadowShockTimer = 3*IN_MILLISECONDS;
				}
			}
			else ShadowShockTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_auchenai_vindicatorAI(creature);
    }
};

class npc_auchenai_monk : public CreatureScript
{
public:
    npc_auchenai_monk() : CreatureScript("npc_auchenai_monk") {}

    struct npc_auchenai_monkAI : public QuantumBasicAI
    {
        npc_auchenai_monkAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SpiritualSightTimer;
		uint32 CycloneStrikeTimer;
		uint32 CounterKickTimer;
		uint32 OverpowerTimer;

		SummonList Summons;

        void Reset()
		{
			SpiritualSightTimer = 0.5*IN_MILLISECONDS;
			CycloneStrikeTimer = 1*IN_MILLISECONDS;
			CounterKickTimer = 2*IN_MILLISECONDS;
			OverpowerTimer = 3*IN_MILLISECONDS;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_KNEEL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			DoCast(me, SPELL_SUMMON_UNLIVING_SOLDIER, true);
			DoCast(me, SPELL_SUMMON_UNLIVING_SORCERER, true);
			DoCast(me, SPELL_SUMMON_UNLIVING_CLERIC, true);
			DoCast(me, SPELL_SUMMON_UNLIVING_STALKER, true);
			DoCast(me, SPELL_SUMMON_PHANTASMAL_POSSESSOR, true);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_AUCHENAI_AGGRO_1, SAY_AUCHENAI_AGGRO_2, SAY_AUCHENAI_AGGRO_3, SAY_AUCHENAI_AGGRO_4), me);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			switch (summon->GetEntry())
			{
                case NPC_UNLIVING_SOLDIER:
				case NPC_UNLIVING_SORCERER:
				case NPC_UNLIVING_CLERIC:
				case NPC_UNLIVING_STALKER:
				case NPC_PHANTASMAL_POSSESSOR:
					Summons.Summon(summon);
					Summons.DoZoneInCombat();
					break;
				default:
					break;
			}
		}

        void UpdateAI(uint32 const diff)
        {
			// Out of Combat Timer
			if (SpiritualSightTimer <= diff && !me->IsInCombatActive())
			{
				DoCast(me, SPELL_SPIRITUAL_SIGHT);
				SpiritualSightTimer = 2*MINUTE*IN_MILLISECONDS;
			}
			else SpiritualSightTimer -= diff;

            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			// Spiritual Sight Buff Check in Combat
			if (!me->HasAura(SPELL_SPIRITUAL_SIGHT))
				DoCast(me, SPELL_SPIRITUAL_SIGHT);

			if (CycloneStrikeTimer <= diff) 
			{
				DoCastVictim(SPELL_CYCLONE_STRIKE);
				CycloneStrikeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CycloneStrikeTimer -= diff;

			if (CounterKickTimer <= diff) 
			{
				DoCastVictim(SPELL_COUNTER_KICK);
				CounterKickTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else CounterKickTimer -= diff;

			if (OverpowerTimer <= diff) 
			{
				DoCastVictim(SPELL_OVERPOWER);
				OverpowerTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else OverpowerTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_auchenai_monkAI(creature);
    }
};

class npc_auchenai_soulpriest : public CreatureScript
{
public:
    npc_auchenai_soulpriest() : CreatureScript("npc_auchenai_soulpriest") {}

    struct npc_auchenai_soulpriestAI : public QuantumBasicAI
    {
        npc_auchenai_soulpriestAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShadowBoltTimer;
		uint32 FalterTimer;
		uint32 TouchOfTheForgottenTimer;

		SummonList Summons;

        void Reset()
		{
			ShadowBoltTimer = 1*IN_MILLISECONDS;
			FalterTimer = 2*IN_MILLISECONDS;
			TouchOfTheForgottenTimer = 3*IN_MILLISECONDS;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			DoCast(me, SPELL_SUMMON_UNLIVING_SOLDIER, true);
			DoCast(me, SPELL_SUMMON_UNLIVING_SORCERER, true);
			DoCast(me, SPELL_SUMMON_UNLIVING_CLERIC, true);
			DoCast(me, SPELL_SUMMON_UNLIVING_STALKER, true);
			DoCast(me, SPELL_SUMMON_PHANTASMAL_POSSESSOR, true);

			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_AUCHENAI_AGGRO_1, SAY_AUCHENAI_AGGRO_2, SAY_AUCHENAI_AGGRO_3, SAY_AUCHENAI_AGGRO_4), me);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			switch (summon->GetEntry())
			{
                case NPC_UNLIVING_SOLDIER:
				case NPC_UNLIVING_SORCERER:
				case NPC_UNLIVING_CLERIC:
				case NPC_UNLIVING_STALKER:
				case NPC_PHANTASMAL_POSSESSOR:
					Summons.Summon(summon);
					Summons.DoZoneInCombat();
					break;
				default:
					break;
			}
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShadowBoltTimer <= diff) 
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SHADOW_BOLT_5N, SPELL_SHADOW_BOLT_5H));
					ShadowBoltTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else ShadowBoltTimer -= diff;

			if (FalterTimer <= diff) 
			{
				DoCastAOE(SPELL_FALTER);
				FalterTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else FalterTimer -= diff;

			if (TouchOfTheForgottenTimer <= diff) 
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_TOUCH_OF_THE_FORGOTTEN_5N, SPELL_TOUCH_OF_THE_FORGOTTEN_5H));
					TouchOfTheForgottenTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else TouchOfTheForgottenTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_auchenai_soulpriestAI(creature);
    }
};

class npc_auchenai_necromancer : public CreatureScript
{
public:
    npc_auchenai_necromancer() : CreatureScript("npc_auchenai_necromancer") {}

    struct npc_auchenai_necromancerAI : public QuantumBasicAI
    {
        npc_auchenai_necromancerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SeedOfCorruptionTimer;
		uint32 DrainSoulTimer;
		uint32 ShadowMendTimer;

        void Reset()
		{
			SeedOfCorruptionTimer = 0.5*IN_MILLISECONDS;
			DrainSoulTimer = 2*IN_MILLISECONDS;
			ShadowMendTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_AUCHENAI_AGGRO_1, SAY_AUCHENAI_AGGRO_2, SAY_AUCHENAI_AGGRO_3, SAY_AUCHENAI_AGGRO_4), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SeedOfCorruptionTimer <= diff) 
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SEED_OF_CORRUPTION_5N, SPELL_SEED_OF_CORRUPTION_5H));
					SeedOfCorruptionTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
				}
			}
			else SeedOfCorruptionTimer -= diff;

			if (DrainSoulTimer <= diff) 
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DRAIN_SOUL);
					DrainSoulTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else DrainSoulTimer -= diff;

			if (ShadowMendTimer <= diff) 
			{
				DoCast(me, SPELL_SHADOW_MEND, true);
				ShadowMendTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else ShadowMendTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_auchenai_necromancerAI(creature);
    }
};

class npc_unliving_soldier : public CreatureScript
{
public:
    npc_unliving_soldier() : CreatureScript("npc_unliving_soldier") {}

    struct npc_unliving_soldierAI : public QuantumBasicAI
    {
        npc_unliving_soldierAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShieldBashTimer;

        void Reset()
        {
			ShieldBashTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_GHOST_VISUAL);
			DoCast(me, SPELL_PROTECTION_AURA);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ShieldBashTimer <= diff)
			{
				DoCastVictim(SPELL_SHIELD_BASH);
				ShieldBashTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else ShieldBashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unliving_soldierAI(creature);
    }
};

class npc_unliving_sorcerer : public CreatureScript
{
public:
    npc_unliving_sorcerer() : CreatureScript("npc_unliving_sorcerer") {}

    struct npc_unliving_sorcererAI : public QuantumBasicAI
    {
        npc_unliving_sorcererAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FireballTimer;
		uint32 FrostboltTimer;
		uint32 BlastWaveTimer;

        void Reset()
		{
			FireballTimer = 1*IN_MILLISECONDS;
			FrostboltTimer = 3*IN_MILLISECONDS;
			BlastWaveTimer = 5*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_GHOST_VISUAL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FIREBALL_5N, SPELL_FIREBALL_5H));
					FireballTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else FireballTimer -= diff;

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FROSTBOLT_5N, SPELL_FROSTBOLT_5H));
					FrostboltTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else FrostboltTimer -= diff;

			if (BlastWaveTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_BLAST_WAVE_5N, SPELL_BLAST_WAVE_5H));
				BlastWaveTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else BlastWaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unliving_sorcererAI(creature);
    }
};

class npc_unliving_cleric : public CreatureScript
{
public:
    npc_unliving_cleric() : CreatureScript("npc_unliving_cleric") {}

    struct npc_unliving_clericAI : public QuantumBasicAI
    {
        npc_unliving_clericAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 RenewTimer;
		uint32 MajorHealTimer;
		uint32 HealTimer;

        void Reset()
		{
			RenewTimer = 1*IN_MILLISECONDS;
			MajorHealTimer = 2*IN_MILLISECONDS;
			HealTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_GHOST_VISUAL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RenewTimer <= diff)
			{
				DoCast(me, DUNGEON_MODE(SPELL_RENEW_5N, SPELL_RENEW_5H));
				RenewTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else RenewTimer -= diff;

			if (MajorHealTimer <= diff)
			{
				DoCast(me, SPELL_MAJOR_HEAL);
				MajorHealTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else MajorHealTimer -= diff;

			if (HealTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, SPELL_HEAL);
					HealTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
				}
			}
			else HealTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
				DoCast(me, SPELL_HEAL);

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unliving_clericAI(creature);
    }
};

class npc_unliving_stalker : public CreatureScript
{
public:
    npc_unliving_stalker() : CreatureScript("npc_unliving_stalker") {}

    struct npc_unliving_stalkerAI : public QuantumBasicAI
    {
        npc_unliving_stalkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShootTimer;
		uint32 SerpentStingTimer;
		uint32 ViperStingTimer;

        void Reset()
        {
			ShootTimer = 1*IN_MILLISECONDS;
			SerpentStingTimer = 2*IN_MILLISECONDS;
			ViperStingTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			DoCast(me, SPELL_GHOST_VISUAL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
					ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
				}
			}
			else ShootTimer -= diff;

			if (SerpentStingTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_SERPENT_STING_5N, SPELL_SERPENT_STING_5H));
					SerpentStingTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else SerpentStingTimer -= diff;

			if (ViperStingTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && target->GetPowerType() == POWER_MANA)
					{
						DoCast(target, SPELL_VIPER_STING);
						ViperStingTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
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
						ShootTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
					}
				}
				else ShootTimer -= diff;
			}

			if (HealthBelowPct(HEALTH_PERCENT_50))
				DoCast(me, SPELL_SPIRIT_VENGEANCE);
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_unliving_stalkerAI(creature);
    }
};

class npc_phantasmal_possessor : public CreatureScript
{
public:
    npc_phantasmal_possessor() : CreatureScript("npc_phantasmal_possessor") {}

    struct npc_phantasmal_possessorAI : public QuantumBasicAI
    {
        npc_phantasmal_possessorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 PossesTimer;

        void Reset()
		{
			PossesTimer = 1*IN_MILLISECONDS;

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

			if (PossesTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_POSSES);
					PossesTimer = 1*MINUTE*IN_MILLISECONDS;
				}
			}
			else PossesTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_phantasmal_possessorAI(creature);
    }
};

class npc_reanimated_bones : public CreatureScript
{
public:
    npc_reanimated_bones() : CreatureScript("npc_reanimated_bones") {}

    struct npc_reanimated_bonesAI : public QuantumBasicAI
    {
        npc_reanimated_bonesAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 StrikeTimer;
		uint32 SunderArmorTimer;

        void Reset()
		{
			StrikeTimer = 1*IN_MILLISECONDS;
			SunderArmorTimer = 2*IN_MILLISECONDS;

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

			if (StrikeTimer <= diff)
			{
				DoCastVictim(SPELL_STRIKE);
				StrikeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else StrikeTimer -= diff;

			if (SunderArmorTimer <= diff)
			{
				DoCastVictim(SPELL_SUNDER_ARMOR);
				SunderArmorTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else SunderArmorTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_reanimated_bonesAI(creature);
    }
};

class npc_raging_skeleton : public CreatureScript
{
public:
    npc_raging_skeleton() : CreatureScript("npc_raging_skeleton") {}

    struct npc_raging_skeletonAI : public QuantumBasicAI
    {
        npc_raging_skeletonAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 CleaveTimer;
		uint32 MortalStrikeTimer;

        void Reset()
		{
			CleaveTimer = 2*IN_MILLISECONDS;
			MortalStrikeTimer = 4*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
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

			if (CleaveTimer <= diff)
			{
				DoCastVictim(SPELL_CLEAVE);
				CleaveTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (MortalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_MORTAL_STRIKE);
				MortalStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else MortalStrikeTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_raging_skeletonAI(creature);
    }
};

class npc_angered_skeleton : public CreatureScript
{
public:
    npc_angered_skeleton() : CreatureScript("npc_angered_skeleton") {}

    struct npc_angered_skeletonAI : public QuantumBasicAI
    {
        npc_angered_skeletonAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 InfuriatedTimer;

        void Reset()
		{
			InfuriatedTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
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

			if (InfuriatedTimer <= diff)
			{
				DoCast(me, SPELL_INFURIATED);
				InfuriatedTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else InfuriatedTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_angered_skeletonAI(creature);
    }
};

void AddSC_auchenai_crypts_trash()
{
	new npc_phasing_sorcerer();
	new npc_phasing_stalker();
	new npc_phasing_soldier();
	new npc_phasing_cleric();
	new npc_auchenai_vindicator();
	new npc_auchenai_monk();
	new npc_auchenai_soulpriest();
	new npc_auchenai_necromancer();
	new npc_unliving_soldier();
	new npc_unliving_sorcerer();
	new npc_unliving_cleric();
	new npc_unliving_stalker();
	new npc_phantasmal_possessor();
	new npc_reanimated_bones();
	new npc_raging_skeleton();
	new npc_angered_skeleton();
}