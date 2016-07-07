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

enum MagistersTerraceSpells
{
	SPELL_MAGIC_DUMPENING_FIELD = 44475,
	SPELL_GLAIVE_THROW_5N       = 44478,
	SPELL_GLAIVE_THROW_5H       = 46028,
	SPELL_FROSTBOLT_5N          = 44606,
	SPELL_FROSTBOLT_5H          = 46035,
	SPELL_ARCANE_NOVA_5N        = 44644,
	SPELL_ARCANE_NOVA_5H        = 46036,
	SPELL_INCREASE_SPELL_HASTE  = 44604,
	SPELL_HOLY_LIGHT_5N         = 44479,
	SPELL_HOLY_LIGHT_5H         = 46029,
	SPELL_SEAL_OF_WRATH_5N      = 44480,
	SPELL_SEAL_OF_WRATH_5H      = 46030,
	SPELL_JUDJEMENT_OF_WRATH_5N = 44482,
	SPELL_JUDJEMENT_OF_WRATH_5H = 46033,
	SPELL_FELL_ARMOR            = 44520,
	SPELL_IMMOLATE_5N           = 44518,
	SPELL_IMMOLATE_5H           = 46042,
	SPELL_INCINERATE_5N         = 44519,
	SPELL_INCINERATE_5H         = 46043,
	SPELL_SUMMON_IMP            = 44517,
	SPELL_IMP_FIREBOLT_5N       = 44577,
	SPELL_IMP_FIREBOLT_5H       = 46044,
	SPELL_INJECT_POISON_5N      = 44599,
	SPELL_INJECT_POISON_5H      = 46046,
	SPELL_PRAYER_OF_MENDING_5N  = 44583,
	SPELL_PRAYER_OF_MENDING_5H  = 46045,
	SPELL_WRETCHED_FIREBALL     = 44503,
	SPELL_WRETCHED_FROSTBOLT    = 44504,
	SPELL_WRETCHED_STAB         = 44533,
	SPELL_WRETCHED_STRIKE       = 44534,
	SPELL_DRINK_FELL_INFUSION   = 44505,
	SPELL_ENERGY_INFUSION       = 44406,
	SPELL_SHADOW_BOLT           = 15232,
	SPELL_SHADOW_BOLT_VOLLEY    = 20741,
	SPELL_BANISH                = 44765,
	SPELL_FEL_LIGHTNING         = 46480,
	SPELL_DEADLY_EMBRACE        = 44547,
	SPELL_LASH_OF_PAIN          = 44640,
	SPELL_FORKED_LIGHTNING_5N   = 20299,
	SPELL_FORKED_LIGHTNING_5H   = 46150,
	SPELL_SHOOT_5N              = 35946,
	SPELL_SHOOT_5H              = 22907,
	SPELL_FROST_ARROW           = 44639,
	SPELL_MANA_SHIELD_5N        = 17741,
	SPELL_MANA_SHIELD_5H        = 46151,
	SPELL_ARCANE_EXPLOSION      = 44538,
	SPELL_FIZZLE_FIREBOLT       = 44164,
    SPELL_SHEEP_EXPLOSION       = 44279,
};

enum Texts
{
	SAY_WRETCHED_AGGRO_1 = -1562000,
	SAY_WRETCHED_AGGRO_2 = -1562001,
	SAY_WRETCHED_AGGRO_3 = -1562002,
	SAY_WRETCHED_AGGRO_4 = -1562003,
};

enum Creatures
{
	NPC_SUNBLADE_IMP = 24815,
};

class npc_sunblade_mage_guard : public CreatureScript
{
public:
    npc_sunblade_mage_guard() : CreatureScript("npc_sunblade_mage_guard") {}

    struct npc_sunblade_mage_guardAI : public QuantumBasicAI
    {
        npc_sunblade_mage_guardAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 GlaiveThrowTimer;

        void Reset()
        {
			GlaiveThrowTimer = 1000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
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

			if (GlaiveThrowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, DUNGEON_MODE(SPELL_GLAIVE_THROW_5N, SPELL_GLAIVE_THROW_5H));
						GlaiveThrowTimer = urand(3000, 4000);
					}
				}
			}
			else GlaiveThrowTimer -= diff;

			if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
				{
                    DoStartMovement(me->GetVictim());
				}

                DoMeleeAttackIfReady();
            }
			else
			{
				if (me->GetDistance2d(me->GetVictim()) > 10)
                    DoStartNoMovement(me->GetVictim());

                if (me->GetDistance2d(me->GetVictim()) > 30)
                    DoStartMovement(me->GetVictim());

				if (GlaiveThrowTimer <= diff)
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						DoCast(target, DUNGEON_MODE(SPELL_GLAIVE_THROW_5N, SPELL_GLAIVE_THROW_5H));
						GlaiveThrowTimer = urand(9000, 10000);
					}
				}
				else GlaiveThrowTimer -= diff;

				if (HealthBelowPct(HEALTH_PERCENT_70))
				{
					if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
					{
						if (!target->HasAuraEffect(SPELL_MAGIC_DUMPENING_FIELD, 0))
						{
							me->HandleEmoteCommand(EMOTE_ONESHOT_LAUGH);
							DoCast(target, SPELL_MAGIC_DUMPENING_FIELD);
						}
					}
				}
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunblade_mage_guardAI(creature);
    }
};

class npc_sunblade_magister : public CreatureScript
{
public:
    npc_sunblade_magister() : CreatureScript("npc_sunblade_magister") {}

    struct npc_sunblade_magisterAI : public QuantumBasicAI
    {
        npc_sunblade_magisterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FrostboltTimer;
		uint32 ArcaneNovaTimer;

        void Reset()
		{
			FrostboltTimer = 500;
			ArcaneNovaTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_INCREASE_SPELL_HASTE);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_FROSTBOLT_5N, SPELL_FROSTBOLT_5H));
					FrostboltTimer = urand(3000, 4000);
				}
			}
			else FrostboltTimer -= diff;

			if (ArcaneNovaTimer <= diff)
			{
				DoCastAOE(DUNGEON_MODE(SPELL_ARCANE_NOVA_5N, SPELL_ARCANE_NOVA_5H));
				ArcaneNovaTimer = urand(5000, 6000);
			}
			else ArcaneNovaTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunblade_magisterAI(creature);
    }
};

class npc_sunblade_blood_knight : public CreatureScript
{
public:
    npc_sunblade_blood_knight() : CreatureScript("npc_sunblade_blood_knight") {}

    struct npc_sunblade_blood_knightAI : public QuantumBasicAI
    {
        npc_sunblade_blood_knightAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 SealOfWrathTimer;
		uint32 JudjementOfWrathTimer;
		uint32 HolyLightTimer;

        void Reset()
		{
			SealOfWrathTimer = 1000;
			JudjementOfWrathTimer = 3000;
			HolyLightTimer = 5000;

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

			if (SealOfWrathTimer <= diff)
			{
				DoCast(me, DUNGEON_MODE(SPELL_SEAL_OF_WRATH_5N, SPELL_SEAL_OF_WRATH_5H));
				SealOfWrathTimer = urand(2000, 3000);
			}
			else SealOfWrathTimer -= diff;

			if (JudjementOfWrathTimer <= diff)
			{
				DoCastVictim(DUNGEON_MODE(SPELL_JUDJEMENT_OF_WRATH_5N, SPELL_JUDJEMENT_OF_WRATH_5H));
				JudjementOfWrathTimer = urand(3000, 4000);
			}
			else JudjementOfWrathTimer -= diff;

			if (HolyLightTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_GREATER))
				{
					DoCast(target, DUNGEON_MODE(SPELL_HOLY_LIGHT_5N, SPELL_HOLY_LIGHT_5H));
					HolyLightTimer = urand(6000, 7000);
				}
			}
			else HolyLightTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_70))
				DoCast(me, DUNGEON_MODE(SPELL_HOLY_LIGHT_5N, SPELL_HOLY_LIGHT_5H));

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunblade_blood_knightAI(creature);
    }
};

class npc_sunblade_warlock : public CreatureScript
{
public:
    npc_sunblade_warlock() : CreatureScript("npc_sunblade_warlock") {}

    struct npc_sunblade_warlockAI : public QuantumBasicAI
    {
        npc_sunblade_warlockAI(Creature* creature) : QuantumBasicAI(creature), Summons(me)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ImmolateTimer;
		uint32 IncinerateTimer;

		SummonList Summons;

        void Reset()
		{
			ImmolateTimer = 2000;
			IncinerateTimer = 4000;

			Summons.DespawnAll();

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_FELL_ARMOR, true);
			DoCast(me, SPELL_SUMMON_IMP, true);
		}

		void JustDied(Unit* /*killer*/)
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summon)
		{
			if (summon->GetEntry() == NPC_SUNBLADE_IMP)
				Summons.Summon(summon);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ImmolateTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_IMMOLATE_5N, SPELL_IMMOLATE_5H));
					ImmolateTimer = urand(3000, 4000);
				}
			}
			else ImmolateTimer -= diff;

			if (IncinerateTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_INCINERATE_5N, SPELL_INCINERATE_5H));
					IncinerateTimer = urand(5000, 6000);
				}
			}
			else IncinerateTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunblade_warlockAI(creature);
    }
};

class npc_sunblade_imp : public CreatureScript
{
public:
    npc_sunblade_imp() : CreatureScript("npc_sunblade_imp") {}

    struct npc_sunblade_impAI : public QuantumBasicAI
    {
        npc_sunblade_impAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 FireboltTimer;

        void Reset()
        {
			DoCast(me, SPELL_UNIT_DETECTION_ALL);

			FireboltTimer = 1000;
        }

		void EnterToBattle(Unit* /*who*/){}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FireboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, DUNGEON_MODE(SPELL_IMP_FIREBOLT_5N, SPELL_IMP_FIREBOLT_5H));
					FireboltTimer = urand(2000, 3000);
				}
			}
			else FireboltTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunblade_impAI(creature);
    }
};

class npc_sunblade_physician : public CreatureScript
{
public:
    npc_sunblade_physician() : CreatureScript("npc_sunblade_physician") {}

    struct npc_sunblade_physicianAI : public QuantumBasicAI
    {
        npc_sunblade_physicianAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 InjectPoisonTimer;
		uint32 PrayerOfMendingTimer;
		bool Mending;

        void Reset()
		{
			InjectPoisonTimer = 2000;
			PrayerOfMendingTimer = 4000;

			Mending = false;

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

			if (InjectPoisonTimer <= diff)
			{
				DoCast(me, DUNGEON_MODE(SPELL_INJECT_POISON_5N, SPELL_INJECT_POISON_5H));
				InjectPoisonTimer = urand(3000, 4000);
			}
			else InjectPoisonTimer -= diff;

			if (PrayerOfMendingTimer <= diff)
			{
				if (Unit* target = SelectTargetHpFriendly(HEAL_PERCENT_DOT))
				{
					DoCast(target, DUNGEON_MODE(SPELL_PRAYER_OF_MENDING_5N, SPELL_PRAYER_OF_MENDING_5H));
					PrayerOfMendingTimer = urand(5000, 6000);
				}
			}
			else PrayerOfMendingTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_70) && !Mending)
			{
				DoCast(me, DUNGEON_MODE(SPELL_PRAYER_OF_MENDING_5N, SPELL_PRAYER_OF_MENDING_5H));
				Mending = true;
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunblade_physicianAI(creature);
    }
};

class npc_wretched_husk : public CreatureScript
{
public:
    npc_wretched_husk() : CreatureScript("npc_wretched_husk") {}

    struct npc_wretched_huskAI : public QuantumBasicAI
    {
        npc_wretched_huskAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 WretchedFireballTimer;
		uint32 WretchedFrostboltTimer;

        void Reset()
		{
			WretchedFireballTimer = 2000;
			WretchedFrostboltTimer = 4000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_WRETCHED_AGGRO_1, SAY_WRETCHED_AGGRO_2, SAY_WRETCHED_AGGRO_3, SAY_WRETCHED_AGGRO_4), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WretchedFireballTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_WRETCHED_FIREBALL, true);
					WretchedFireballTimer = urand(3000, 4000);
				}
			}
			else WretchedFireballTimer -= diff;

			if (WretchedFrostboltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_WRETCHED_FROSTBOLT, true);
					WretchedFrostboltTimer = urand(5000, 6000);
				}
			}
			else WretchedFrostboltTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_DRINK_FELL_INFUSION, 0))
					DoCast(me, SPELL_DRINK_FELL_INFUSION);
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wretched_huskAI(creature);
    }
};

class npc_wretched_skulker : public CreatureScript
{
public:
    npc_wretched_skulker() : CreatureScript("npc_wretched_skulker") {}

    struct npc_wretched_skulkerAI : public QuantumBasicAI
    {
        npc_wretched_skulkerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 WretchedStabTimer;

        void Reset()
		{
			WretchedStabTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_WRETCHED_AGGRO_1, SAY_WRETCHED_AGGRO_2, SAY_WRETCHED_AGGRO_3, SAY_WRETCHED_AGGRO_4), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WretchedStabTimer <= diff)
			{
				DoCastVictim(SPELL_WRETCHED_STAB);
				WretchedStabTimer = urand(3000, 4000);
			}
			else WretchedStabTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_DRINK_FELL_INFUSION, 0))
					DoCast(me, SPELL_DRINK_FELL_INFUSION);
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wretched_skulkerAI(creature);
    }
};

class npc_wretched_bruiser : public CreatureScript
{
public:
    npc_wretched_bruiser() : CreatureScript("npc_wretched_bruiser") {}

    struct npc_wretched_bruiserAI : public QuantumBasicAI
    {
        npc_wretched_bruiserAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 WretchedStrikeTimer;

        void Reset()
		{
			WretchedStrikeTimer = 2000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* who)
		{
			if (who->GetTypeId() == TYPE_ID_PLAYER)
				DoSendQuantumText(RAND(SAY_WRETCHED_AGGRO_1, SAY_WRETCHED_AGGRO_2, SAY_WRETCHED_AGGRO_3, SAY_WRETCHED_AGGRO_4), me);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (WretchedStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_WRETCHED_STRIKE);
				WretchedStrikeTimer = urand(3000, 4000);
			}
			else WretchedStrikeTimer -= diff;

			if (HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAuraEffect(SPELL_DRINK_FELL_INFUSION, 0))
					DoCast(me, SPELL_DRINK_FELL_INFUSION);
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wretched_bruiserAI(creature);
    }
};

class npc_brightscale_wyrm : public CreatureScript
{
public:
    npc_brightscale_wyrm() : CreatureScript("npc_brightscale_wyrm") {}

    struct npc_brightscale_wyrmAI : public QuantumBasicAI
    {
        npc_brightscale_wyrmAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

        void Reset()
		{
			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/){}

		void JustDied(Unit* killer)
		{
			DoCastAOE(SPELL_ENERGY_INFUSION, true);
		}

        void UpdateAI(const uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_brightscale_wyrmAI(creature);
    }
};

class npc_sunblade_keeper : public CreatureScript
{
public:
    npc_sunblade_keeper() : CreatureScript("npc_sunblade_keeper") {}

    struct npc_sunblade_keeperAI : public QuantumBasicAI
    {
        npc_sunblade_keeperAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShadowBoltTimer;
		uint32 ShadowBoltVolleyTimer;
		uint32 BanishTimer;

        void Reset()
		{
			ShadowBoltTimer = 1000;
			ShadowBoltVolleyTimer = 4000;
			BanishTimer = 5000;

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

			if (ShadowBoltTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_BOLT);
					ShadowBoltTimer = urand(3000, 6000);
				}
			}
			else ShadowBoltTimer -= diff;

			if (ShadowBoltVolleyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SHADOW_BOLT_VOLLEY);
					ShadowBoltVolleyTimer = urand(7000, 8000);
				}
			}
			else ShadowBoltVolleyTimer -= diff;

			if (BanishTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BANISH);
					BanishTimer = urand(9000, 10000);
				}
			}
			else BanishTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sunblade_keeperAI(creature);
    }
};

class npc_sunblade_sentinel : public CreatureScript
{
public:
    npc_sunblade_sentinel() : CreatureScript("npc_sunblade_sentinel") { }

	struct npc_sunblade_sentinelAI : public QuantumBasicAI
    {
        npc_sunblade_sentinelAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
		}

        uint32 FellLigtningTimer;

        void Reset()
		{
			FellLigtningTimer = 2000;

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

            if (FellLigtningTimer < diff)
            {
                if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FEL_LIGHTNING);
                    FellLigtningTimer = 6000;
                }
            }
			else FellLigtningTimer -= diff;

			DoMeleeAttackIfReady();
        }
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_sunblade_sentinelAI(creature);
	}
};

class npc_sister_of_torment : public CreatureScript
{
public:
    npc_sister_of_torment() : CreatureScript("npc_sister_of_torment") {}

    struct npc_sister_of_tormentAI : public QuantumBasicAI
    {
        npc_sister_of_tormentAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 DeadlyEmbraceTimer;
		uint32 LashOfPainTimer;

        void Reset()
		{
			LashOfPainTimer = 1000;
			DeadlyEmbraceTimer = 3000;

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

			if (LashOfPainTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_LASH_OF_PAIN);
					LashOfPainTimer = urand(3000, 3000);
				}
			}
			else LashOfPainTimer -= diff;

			if (DeadlyEmbraceTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DEADLY_EMBRACE);
					DeadlyEmbraceTimer = urand(5000, 6000);
				}
			}
			else DeadlyEmbraceTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_sister_of_tormentAI(creature);
    }
};

class npc_coilskar_witch : public CreatureScript
{
public:
    npc_coilskar_witch() : CreatureScript("npc_coilskar_witch") {}

    struct npc_coilskar_witchAI : public QuantumBasicAI
    {
        npc_coilskar_witchAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 ShootTimer;
		uint32 FrostArrowTimer;
		uint32 ForkedLightningTimer;

        void Reset()
        {
			ShootTimer = 2000;
			FrostArrowTimer = 4000;
			ForkedLightningTimer = 6000;

			DoCast(me, SPELL_UNIT_DETECTION_ALL);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, DUNGEON_MODE(SPELL_MANA_SHIELD_5N, SPELL_MANA_SHIELD_5H));
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
					DoCast(target, DUNGEON_MODE(SPELL_SHOOT_5N, SPELL_SHOOT_5H));
					ShootTimer = urand(3000, 4000);
				}
			}
			else ShootTimer -= diff;

			if (FrostArrowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FROST_ARROW);
					FrostArrowTimer = urand(5000, 6000);
				}
			}
			else FrostArrowTimer -= diff;

			if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
				{
                    DoStartMovement(me->GetVictim());
				}

				if (ForkedLightningTimer <= diff)
				{
					DoCastVictim(DUNGEON_MODE(SPELL_FORKED_LIGHTNING_5N, SPELL_FORKED_LIGHTNING_5H));
					ForkedLightningTimer = urand(7000, 8000);
				}
				else ForkedLightningTimer -= diff;

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
						ShootTimer = urand(9000, 10000);
					}
				}
				else ShootTimer -= diff;
			}
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_coilskar_witchAI(creature);
    }
};

class npc_ethereum_smuggler : public CreatureScript
{
public:
    npc_ethereum_smuggler() : CreatureScript("npc_ethereum_smuggler") { }

	struct npc_ethereum_smugglerAI : public QuantumBasicAI
    {
        npc_ethereum_smugglerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

        uint32 ArcaneExplosionTimer;

        void Reset()
		{
			ArcaneExplosionTimer = 2000;

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

            if (ArcaneExplosionTimer < diff)
            {
				DoCastAOE(SPELL_ARCANE_EXPLOSION);
				ArcaneExplosionTimer = 5000;
            }
			else ArcaneExplosionTimer -= diff;

			DoMeleeAttackIfReady();
        }
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_ethereum_smugglerAI(creature);
	}
};

class npc_fizzle : public CreatureScript
{
public:
    npc_fizzle() : CreatureScript("npc_fizzle") { }

	struct npc_fizzleAI : public QuantumBasicAI
    {
        npc_fizzleAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

        uint32 FireboltTimer;

        void Reset()
		{
			FireboltTimer = 1000;

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

            if (FireboltTimer < diff)
            {
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_FIZZLE_FIREBOLT);
					FireboltTimer = 3000;
				}
            }
			else FireboltTimer -= diff;

			DoMeleeAttackIfReady();
        }
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_fizzleAI(creature);
	}
};

class npc_high_explosive_sheep : public CreatureScript
{
public:
	npc_high_explosive_sheep() : CreatureScript("npc_high_explosive_sheep") {}

	struct npc_high_explosive_sheepAI : public QuantumBasicAI
	{
		npc_high_explosive_sheepAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_AURA_TYPE, SPELL_AURA_MOD_TAUNT, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
			me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_TURN, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		bool Despawn;

		Unit* SelectPlayerTargetInRange(float range)
        {
            Player *target = NULL;
            Trinity::AnyPlayerInObjectRangeCheck u_check(me, range, true);
            Trinity::PlayerSearcher<Trinity::AnyPlayerInObjectRangeCheck> searcher(me, target, u_check);
            me->VisitNearbyObject(range, searcher);
            return target;
        }

		void Reset()
		{
			Despawn = false;
		}

		void EnterToBattle(Unit* /*who*/)
		{
			if (Unit* target = SelectPlayerTargetInRange(100.0f))
				me->GetMotionMaster()->MoveFollow(target, 100.0f, 0.0f);
		}

		void SpellHit(Unit* /*caster*/, SpellInfo const* spell)
		{
			if (spell->Id == SPELL_SHEEP_EXPLOSION)
				me->DespawnAfterAction(0.5*IN_MILLISECONDS);
		}

		void JustDied(Unit* /*killer*/)
		{
			DoCastAOE(SPELL_SHEEP_EXPLOSION);
		}

		void UpdateAI(const uint32 /*diff*/)
		{
			if (!UpdateVictim())
				return;

			if (!Despawn && me->IsWithinMeleeRange(me->GetVictim()))
			{
				Despawn = true;
				DoCastAOE(SPELL_SHEEP_EXPLOSION);
			}

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_high_explosive_sheepAI(creature);
	}
};

void AddSC_magisters_terrace_trash()
{
	new npc_sunblade_mage_guard();
	new npc_sunblade_magister();
	new npc_sunblade_blood_knight();
	new npc_sunblade_warlock();
	new npc_sunblade_imp();
	new npc_sunblade_physician();
	new npc_wretched_husk();
	new npc_wretched_skulker();
	new npc_wretched_bruiser();
	new npc_brightscale_wyrm();
	new npc_sunblade_keeper();
	new npc_sunblade_sentinel();
	new npc_sister_of_torment();
	new npc_coilskar_witch();
	new npc_ethereum_smuggler();
	new npc_fizzle();
	new npc_high_explosive_sheep();
}