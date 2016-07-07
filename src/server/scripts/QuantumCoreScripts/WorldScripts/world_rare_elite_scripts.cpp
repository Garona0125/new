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

enum RareEliteSpells
{
	SPELL_DUAL_WIELD          = 674,
	SPELL_EYE_PECK            = 49865,
	SPELL_WING_BUFFET         = 51144,
	SPELL_SUPPRESSIVE_FIRE    = 61103,
	SPELL_MECHANO_KICK        = 61110,
	SPELL_SMASH               = 51334,
	SPELL_BLOOD_STRIKE        = 60945,
	SPELL_DEATH_COIL          = 60949,
	SPELL_FROST_STRIKE        = 60951,
	SPELL_DEATH_AND_DECAY     = 60953,
	SPELL_ROMP                = 57468,
	SPELL_FLIPPER_THWACK      = 50169,
	SPELL_BELLY_FLOP          = 61115,
	SPELL_MARK_OF_DETONATION  = 50506,
	SPELL_ARCANE_BREATH       = 60903,
	SPELL_WAR_STOMP           = 41534,
	SPELL_PUTRID_PUNT         = 61080,
	SPELL_SEETHING_EVIL       = 52342,
	SPELL_INSTANT_POISON_PROC = 28429,
	SPELL_IMPAIRING_POISON    = 36839,
	SPELL_CLEAVE              = 43273,
	SPELL_REND                = 53317,
	SPELL_STOMP               = 55196,
	SPELL_WHAMMY              = 54565,
	SPELL_BRUTAL_STRIKE       = 58460,
	SPELL_CRAZED              = 48139,
	SPELL_MACHINE_GUN         = 60906,
	SPELL_STONE_STOMP         = 49675,
	SPELL_STONE_FIST          = 49676,
	SPELL_TOUCH_OF_THE_VALKYR = 57547,
	SPELL_CHARGE              = 36140,
	SPELL_TERRIFYING_ROAR     = 36629,
	SPELL_MORTAL_WOUND        = 48137,
	SPELL_POUNCE              = 61184,
	SPELL_FROZEN_BITE         = 61185,
	SPELL_GNAW_BONE           = 50046,
	SPELL_KILLING_RAGE        = 52071,
	SPELL_BLOODTHIRST         = 35949,
	SPELL_WHIRLWIND           = 48280,
	SPELL_UNRELENT_ONSLAUGHT  = 50714,
	SPELL_FROST_ARROW         = 38952,
	SPELL_IMPROVED_WING_CLIP  = 47168,
	SPELL_SHOOT               = 50092,
	SPELL_TUSK_STRIKE         = 50410,
	SPELL_TRAMPLE             = 57066,
	SPELL_FROST_BREATH        = 47425,
	SPELL_FROST_CLEAVE        = 51857,
	SPELL_HOVER_ANIM          = 57764,
	SPELL_TIME_LAPSE          = 51020,
	SPELL_TIME_SHIFT          = 61084,
};

enum Misc
{
	WAR_MAIDEN_DRAKE_ID = 28040,
	FUMBLUB_MOUNT_ID    = 25587,
	DAION_MOUNT_ID      = 19296,
};

class npc_aotona : public CreatureScript
{
public:
    npc_aotona() : CreatureScript("npc_aotona") {}

    struct npc_aotonaAI : public QuantumBasicAI
    {
        npc_aotonaAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 EyePeckTimer;
		uint32 WingBuffetTimer;

        void Reset()
        {
			EyePeckTimer = 1*IN_MILLISECONDS;
			WingBuffetTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (EyePeckTimer <= diff)
			{
				DoCastVictim(SPELL_EYE_PECK);
				EyePeckTimer = 4*IN_MILLISECONDS;
			}
			else EyePeckTimer -= diff;

			if (WingBuffetTimer <= diff)
			{
				DoCastAOE(SPELL_WING_BUFFET);
				WingBuffetTimer = 5*IN_MILLISECONDS;
			}
			else WingBuffetTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_aotonaAI(creature);
    }
};

class npc_dirkee : public CreatureScript
{
public:
    npc_dirkee() : CreatureScript("npc_dirkee") {}

    struct npc_dirkeeAI : public QuantumBasicAI
    {
        npc_dirkeeAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 MechanoKickTimer;
		uint32 SuppressiveFireTimer;

        void Reset()
        {
			MechanoKickTimer = 0.5*IN_MILLISECONDS;
			SuppressiveFireTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MechanoKickTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MECHANO_KICK);
					MechanoKickTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
				}
			}
			else MechanoKickTimer -= diff;

			if (SuppressiveFireTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_SUPPRESSIVE_FIRE);
					SuppressiveFireTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else SuppressiveFireTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_dirkeeAI(creature);
    }
};

class npc_griegen : public CreatureScript
{
public:
    npc_griegen() : CreatureScript("npc_griegen") {}

    struct npc_griegenAI : public QuantumBasicAI
    {
        npc_griegenAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
		}

		uint32 SmashTimer;

        void Reset()
		{
			SmashTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (SmashTimer <= diff)
			{
				DoCastVictim(SPELL_SMASH);
				SmashTimer = 5*IN_MILLISECONDS;
			}
			else SmashTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_griegenAI(creature);
    }
};

class npc_high_thane_jorfus : public CreatureScript
{
public:
	npc_high_thane_jorfus() : CreatureScript("npc_high_thane_jorfus") {}

    struct npc_high_thane_jorfusAI : public QuantumBasicAI
    {
        npc_high_thane_jorfusAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
		}

		uint32 BloodStrikeTimer;
		uint32 FrostStrikeTimer;
		uint32 DeathAndDecayTimer;
		uint32 DeathCoilTimer;

        void Reset()
        {
			BloodStrikeTimer = 2*IN_MILLISECONDS;
			FrostStrikeTimer = 3*IN_MILLISECONDS;
			DeathAndDecayTimer = 4*IN_MILLISECONDS;
			DeathCoilTimer = 5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
        }

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BloodStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_BLOOD_STRIKE);
				BloodStrikeTimer = urand(3*IN_MILLISECONDS, 4*IN_MILLISECONDS);
			}
			else BloodStrikeTimer -= diff;

			if (FrostStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_FROST_STRIKE);
				FrostStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else FrostStrikeTimer -= diff;

			if (DeathAndDecayTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DEATH_AND_DECAY);
					DeathAndDecayTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
				}
			}
			else DeathAndDecayTimer -= diff;

			if (DeathCoilTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_DEATH_COIL);
					DeathCoilTimer = urand(9*IN_MILLISECONDS, 10*IN_MILLISECONDS);
				}
			}
			else DeathCoilTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_high_thane_jorfusAI(creature);
    }
};

class npc_icehorn : public CreatureScript
{
public:
    npc_icehorn() : CreatureScript("npc_icehorn") {}

    struct npc_icehornAI : public QuantumBasicAI
    {
        npc_icehornAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 RompTimer;

        void Reset()
		{
			RompTimer = 1*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (RompTimer <= diff)
			{
				DoCastVictim(SPELL_ROMP);
				RompTimer = 5*IN_MILLISECONDS;
			}
			else RompTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_icehornAI(creature);
    }
};

class npc_king_ping : public CreatureScript
{
public:
    npc_king_ping() : CreatureScript("npc_king_ping") {}

    struct npc_king_pingAI : public QuantumBasicAI
    {
        npc_king_pingAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 BellyFlopTimer;
		uint32 FlipperThwackTimer;

        void Reset()
		{
			BellyFlopTimer = 0.5*IN_MILLISECONDS;
			FlipperThwackTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BellyFlopTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_BELLY_FLOP);
					BellyFlopTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else BellyFlopTimer -= diff;

			if (FlipperThwackTimer <= diff)
			{
				DoCastVictim(SPELL_FLIPPER_THWACK);
				FlipperThwackTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else FlipperThwackTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_king_pingAI(creature);
    }
};

class npc_old_crystalbark : public CreatureScript
{
public:
    npc_old_crystalbark() : CreatureScript("npc_old_crystalbark") {}

    struct npc_old_crystalbarkAI : public QuantumBasicAI
    {
        npc_old_crystalbarkAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 MarkOfDetonationTimer;
		uint32 ArcaneBreathTimer;

        void Reset()
        {
			MarkOfDetonationTimer = 0.5*IN_MILLISECONDS;
			ArcaneBreathTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MarkOfDetonationTimer <= diff)
			{
				DoCast(me, SPELL_MARK_OF_DETONATION);
				MarkOfDetonationTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else MarkOfDetonationTimer -= diff;

			if (ArcaneBreathTimer <= diff)
			{
				DoCastAOE(SPELL_ARCANE_BREATH);
				ArcaneBreathTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else ArcaneBreathTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_old_crystalbarkAI(creature);
    }
};

class npc_putridus_the_ancient : public CreatureScript
{
public:
    npc_putridus_the_ancient() : CreatureScript("npc_putridus_the_ancient") {}

    struct npc_putridus_the_ancientAI : public QuantumBasicAI
    {
        npc_putridus_the_ancientAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
		}

		uint32 PutridPuntTimer;
		uint32 WarStompTimer;

        void Reset()
        {
			PutridPuntTimer = 2*IN_MILLISECONDS;
			WarStompTimer = 4*IN_MILLISECONDS;

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

			if (PutridPuntTimer <= diff)
			{
				DoCastVictim(SPELL_PUTRID_PUNT);
				PutridPuntTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else PutridPuntTimer -= diff;

			if (WarStompTimer <= diff)
			{
				DoCastAOE(SPELL_WAR_STOMP);
				WarStompTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else WarStompTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_putridus_the_ancientAI(creature);
    }
};

class npc_seething_hate : public CreatureScript
{
public:
    npc_seething_hate() : CreatureScript("npc_seething_hate") {}

    struct npc_seething_hateAI : public QuantumBasicAI
    {
        npc_seething_hateAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
		}

        void Reset()
        {
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			DoCast(me, SPELL_SEETHING_EVIL);
		}

        void UpdateAI(uint32 const /*diff*/)
        {
            if (!UpdateVictim())
                return;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_seething_hateAI(creature);
    }
};

class npc_terror_spinner : public CreatureScript
{
public:
    npc_terror_spinner() : CreatureScript("npc_terror_spinner") {}

    struct npc_terror_spinnerAI : public QuantumBasicAI
    {
        npc_terror_spinnerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 ImpairingPoisonTimer;
		uint32 InstantPoisonTimer;

        void Reset()
		{
			ImpairingPoisonTimer = 0.5*IN_MILLISECONDS;
			InstantPoisonTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (ImpairingPoisonTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_IMPAIRING_POISON);
					ImpairingPoisonTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else ImpairingPoisonTimer -= diff;

			if (InstantPoisonTimer <= diff)
			{
				DoCast(me, SPELL_INSTANT_POISON_PROC);
				InstantPoisonTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else InstantPoisonTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_terror_spinnerAI(creature);
    }
};

class npc_vigdis_the_war_maiden : public CreatureScript
{
public:
    npc_vigdis_the_war_maiden() : CreatureScript("npc_vigdis_the_war_maiden") {}

    struct npc_vigdis_the_war_maidenAI : public QuantumBasicAI
    {
        npc_vigdis_the_war_maidenAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 CleaveTimer;
		uint32 RendTimer;

        void Reset()
		{
			CleaveTimer = 2*IN_MILLISECONDS;
			RendTimer = 3*IN_MILLISECONDS;

			DoCast(me, SPELL_DUAL_WIELD);

			me->Mount(WAR_MAIDEN_DRAKE_ID);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			me->RemoveMount();
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
				CleaveTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else CleaveTimer -= diff;

			if (RendTimer <= diff)
			{
				DoCastVictim(SPELL_REND);
				RendTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else RendTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vigdis_the_war_maidenAI(creature);
    }
};

class npc_zuldrak_sentinel : public CreatureScript
{
public:
    npc_zuldrak_sentinel() : CreatureScript("npc_zuldrak_sentinel") {}

    struct npc_zuldrak_sentinelAI : public QuantumBasicAI
    {
        npc_zuldrak_sentinelAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 WhammyTimer;
		uint32 StompTimer;

        void Reset()
		{
			WhammyTimer = 0.5*IN_MILLISECONDS;
			StompTimer = 3*IN_MILLISECONDS;

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

			if (WhammyTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_WHAMMY);
					WhammyTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
			}
			else WhammyTimer -= diff;

			if (StompTimer <= diff)
			{
				DoCastAOE(SPELL_STOMP);
				StompTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else StompTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_zuldrak_sentinelAI(creature);
    }
};

class npc_crazed_indule_survivor : public CreatureScript
{
public:
    npc_crazed_indule_survivor() : CreatureScript("npc_crazed_indule_survivor") {}

    struct npc_crazed_indule_survivorAI : public QuantumBasicAI
    {
        npc_crazed_indule_survivorAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 BrutalStrikeTimer;

        void Reset()
		{
			BrutalStrikeTimer = 2*IN_MILLISECONDS;

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

			if (BrutalStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_BRUTAL_STRIKE);
				BrutalStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else BrutalStrikeTimer -= diff;

			if (me->HealthBelowPct(HEALTH_PERCENT_50))
			{
				if (!me->HasAura(SPELL_CRAZED))
					DoCast(me, SPELL_CRAZED);
			}

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_crazed_indule_survivorAI(creature);
    }
};

class npc_fumblub_gearwind : public CreatureScript
{
public:
    npc_fumblub_gearwind() : CreatureScript("npc_fumblub_gearwind") {}

    struct npc_fumblub_gearwindAI : public QuantumBasicAI
    {
        npc_fumblub_gearwindAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 MachineGunTimer;

        void Reset()
		{
			MachineGunTimer = 0.5*IN_MILLISECONDS;

			me->Mount(FUMBLUB_MOUNT_ID);
			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);

			me->SetPowerType(POWER_ENERGY);
			me->SetPower(POWER_ENERGY, POWER_QUANTITY_MAX);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (MachineGunTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_MACHINE_GUN);
					MachineGunTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else MachineGunTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_fumblub_gearwindAI(creature);
    }
};

class npc_grocklar : public CreatureScript
{
public:
    npc_grocklar() : CreatureScript("npc_grocklar") {}

    struct npc_grocklarAI : public QuantumBasicAI
    {
        npc_grocklarAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
		}

		uint32 StoneFistTimer;
		uint32 StoneStompTimer;

        void Reset()
        {
			StoneFistTimer = 0.5*IN_MILLISECONDS;
			StoneStompTimer = 2*IN_MILLISECONDS;

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

			if (StoneFistTimer <= diff)
			{
				DoCast(me, SPELL_STONE_FIST);
				StoneFistTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else StoneFistTimer -= diff;

			if (StoneStompTimer <= diff)
			{
				DoCastAOE(SPELL_STONE_STOMP);
				StoneStompTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else StoneStompTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_grocklarAI(creature);
    }
};

class npc_hildana_deathstealer : public CreatureScript
{
public:
    npc_hildana_deathstealer() : CreatureScript("npc_hildana_deathstealer") {}

    struct npc_hildana_deathstealerAI : public QuantumBasicAI
    {
        npc_hildana_deathstealerAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
		}

		uint32 TouchOfTheValkyrTimer;

        void Reset()
        {
			TouchOfTheValkyrTimer = 0.5*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TouchOfTheValkyrTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_TOUCH_OF_THE_VALKYR);
					TouchOfTheValkyrTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else TouchOfTheValkyrTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_hildana_deathstealerAI(creature);
    }
};

class npc_king_krush : public CreatureScript
{
public:
    npc_king_krush() : CreatureScript("npc_king_krush") {}

    struct npc_king_krushAI : public QuantumBasicAI
    {
        npc_king_krushAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 ChargeTimer;
		uint32 MortalWoundTimer;
		uint32 TerrifyingRoarTimer;

        void Reset()
        {
			ChargeTimer = 0.5*IN_MILLISECONDS;
			MortalWoundTimer = 2*IN_MILLISECONDS;
			TerrifyingRoarTimer = 3*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
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
					ChargeTimer = 5*IN_MILLISECONDS;
				}
			}
			else ChargeTimer -= diff;

			if (MortalWoundTimer <= diff)
			{
				DoCastVictim(SPELL_MORTAL_WOUND);
				MortalWoundTimer = 6*IN_MILLISECONDS;
			}
			else MortalWoundTimer -= diff;

			if (TerrifyingRoarTimer <= diff)
			{
				DoCastAOE(SPELL_TERRIFYING_ROAR);
				TerrifyingRoarTimer = 8*IN_MILLISECONDS;
			}
			else TerrifyingRoarTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_king_krushAI(creature);
    }
};

class npc_loque_nahak : public CreatureScript
{
public:
    npc_loque_nahak() : CreatureScript("npc_loque_nahak") {}

    struct npc_loque_nahakAI : public QuantumBasicAI
    {
        npc_loque_nahakAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 PounceTimer;
		uint32 FrozenBiteTimer;

        void Reset()
        {
			PounceTimer = 0.5*IN_MILLISECONDS;
			FrozenBiteTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PounceTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_POUNCE, true);
					PounceTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else PounceTimer -= diff;

			if (FrozenBiteTimer <= diff)
			{
				DoCast(me, SPELL_FROZEN_BITE);
				FrozenBiteTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else FrozenBiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_loque_nahakAI(creature);
    }
};

class npc_perobas_the_bloodthirster : public CreatureScript
{
public:
    npc_perobas_the_bloodthirster() : CreatureScript("npc_perobas_the_bloodthirster") {}

    struct npc_perobas_the_bloodthirsterAI : public QuantumBasicAI
    {
        npc_perobas_the_bloodthirsterAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 KillingRageTimer;
		uint32 GnawBoneTimer;

        void Reset()
        {
			KillingRageTimer = 0.5*IN_MILLISECONDS;
			GnawBoneTimer = 2*IN_MILLISECONDS;

			DoCast(me, SPELL_DUAL_WIELD);
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

			if (KillingRageTimer <= diff)
			{
				DoCast(me, SPELL_KILLING_RAGE);
				KillingRageTimer = 6500;
			}
			else KillingRageTimer -= diff;

			if (GnawBoneTimer <= diff)
			{
				DoCastVictim(SPELL_GNAW_BONE);
				GnawBoneTimer = 9*IN_MILLISECONDS;
			}
			else GnawBoneTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_perobas_the_bloodthirsterAI(creature);
    }
};

class npc_scarlet_highlord_daion : public CreatureScript
{
public:
    npc_scarlet_highlord_daion() : CreatureScript("npc_scarlet_highlord_daion") {}

    struct npc_scarlet_highlord_daionAI : public QuantumBasicAI
    {
        npc_scarlet_highlord_daionAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 BloodthirstTimer;
		uint32 WhirlwindTimer;

        void Reset()
		{
			BloodthirstTimer = 2*IN_MILLISECONDS;
			WhirlwindTimer = 3*IN_MILLISECONDS;

			me->Mount(DAION_MOUNT_ID);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

		void EnterToBattle(Unit* /*who*/)
		{
			me->SetPowerType(POWER_RAGE);
			me->SetPower(POWER_RAGE, POWER_QUANTITY_MAX);

			me->RemoveMount();

			DoCast(me, SPELL_UNRELENT_ONSLAUGHT);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (BloodthirstTimer <= diff)
			{
				DoCastVictim(SPELL_BLOODTHIRST);
				BloodthirstTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
			}
			else BloodthirstTimer -= diff;

			if (WhirlwindTimer <= diff)
			{
				DoCast(me, SPELL_WHIRLWIND);
				WhirlwindTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
			}
			else WhirlwindTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_scarlet_highlord_daionAI(creature);
    }
};

class npc_syreian_the_bonecarver : public CreatureScript
{
public:
    npc_syreian_the_bonecarver() : CreatureScript("npc_syreian_the_bonecarver") {}

    struct npc_syreian_the_bonecarverAI : public QuantumBasicAI
    {
        npc_syreian_the_bonecarverAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 ShootTimer;
		uint32 FrostArrowTimer;
		uint32 ImprovedWingClipTimer;

        void Reset()
        {
			ShootTimer = 0.5*IN_MILLISECONDS;
			FrostArrowTimer = 1*IN_MILLISECONDS;
			ImprovedWingClipTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_RANGED_WEAPON);
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
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_SHOOT);
						ShootTimer = urand(2*IN_MILLISECONDS, 3*IN_MILLISECONDS);
					}
				}
			}
			else ShootTimer -= diff;

			if (FrostArrowTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					if (target && me->GetDistance2d(target) > 10 && me->GetDistance2d(target) < 30)
					{
						DoCast(target, SPELL_FROST_ARROW);
						FrostArrowTimer = urand(4*IN_MILLISECONDS, 5*IN_MILLISECONDS);
					}
				}
			}
			else FrostArrowTimer -= diff;

			if (me->IsWithinMeleeRange(me->GetVictim()))
            {
                if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    DoStartMovement(me->GetVictim());

				if (ImprovedWingClipTimer <= diff)
				{
					DoCastVictim(SPELL_IMPROVED_WING_CLIP);
					ImprovedWingClipTimer = urand(6*IN_MILLISECONDS, 7*IN_MILLISECONDS);
				}
				else ImprovedWingClipTimer -= diff;

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
        return new npc_syreian_the_bonecarverAI(creature);
    }
};

class npc_tukemuth : public CreatureScript
{
public:
    npc_tukemuth() : CreatureScript("npc_tukemuth") {}

    struct npc_tukemuthAI : public QuantumBasicAI
    {
        npc_tukemuthAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 TuskStrikeTimer;
		uint32 TrampleTimer;

        void Reset()
		{
			TuskStrikeTimer = 2*IN_MILLISECONDS;
			TrampleTimer = 3*IN_MILLISECONDS;

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

			if (TuskStrikeTimer <= diff)
			{
				DoCastVictim(SPELL_TUSK_STRIKE);
				TuskStrikeTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else TuskStrikeTimer -= diff;

			if (TrampleTimer <= diff)
			{
				DoCastAOE(SPELL_TRAMPLE);
				TrampleTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else TrampleTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_tukemuthAI(creature);
    }
};

class npc_vyragosa : public CreatureScript
{
public:
    npc_vyragosa() : CreatureScript("npc_vyragosa") {}

    struct npc_vyragosaAI : public QuantumBasicAI
    {
        npc_vyragosaAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 FrostBreathTimer;
		uint32 FrostCleaveTimer;

        void Reset()
		{
			FrostBreathTimer = 2*IN_MILLISECONDS;
			FrostCleaveTimer = 4*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (FrostBreathTimer <= diff)
			{
				DoCastVictim(SPELL_FROST_BREATH);
				FrostBreathTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
			}
			else FrostBreathTimer -= diff;

			if (FrostCleaveTimer <= diff)
			{
				DoCastVictim(SPELL_FROST_CLEAVE);
				FrostCleaveTimer = urand(7*IN_MILLISECONDS, 8*IN_MILLISECONDS);
			}
			else FrostCleaveTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_vyragosaAI(creature);
    }
};

class npc_gondria : public CreatureScript
{
public:
    npc_gondria() : CreatureScript("npc_gondria") {}

    struct npc_gondriaAI : public QuantumBasicAI
    {
        npc_gondriaAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 PounceTimer;
		uint32 FrozenBiteTimer;

        void Reset()
        {
			PounceTimer = 0.5*IN_MILLISECONDS;
			FrozenBiteTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PounceTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_POUNCE, true);
					PounceTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else PounceTimer -= diff;

			if (FrozenBiteTimer <= diff)
			{
				DoCast(me, SPELL_FROZEN_BITE);
				FrozenBiteTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else FrozenBiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_gondriaAI(creature);
    }
};

class npc_arcturis : public CreatureScript
{
public:
    npc_arcturis() : CreatureScript("npc_arcturis") {}

    struct npc_arcturisAI : public QuantumBasicAI
    {
        npc_arcturisAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 PounceTimer;
		uint32 FrozenBiteTimer;

        void Reset()
        {
			PounceTimer = 0.5*IN_MILLISECONDS;
			FrozenBiteTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PounceTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_POUNCE, true);
					PounceTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else PounceTimer -= diff;

			if (FrozenBiteTimer <= diff)
			{
				DoCast(me, SPELL_FROZEN_BITE);
				FrozenBiteTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else FrozenBiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_arcturisAI(creature);
    }
};

class npc_skoll : public CreatureScript
{
public:
    npc_skoll() : CreatureScript("npc_skoll") {}

    struct npc_skollAI : public QuantumBasicAI
    {
        npc_skollAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 PounceTimer;
		uint32 FrozenBiteTimer;

        void Reset()
        {
			PounceTimer = 0.5*IN_MILLISECONDS;
			FrozenBiteTimer = 2*IN_MILLISECONDS;

			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (PounceTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_POUNCE, true);
					PounceTimer = urand(5*IN_MILLISECONDS, 6*IN_MILLISECONDS);
				}
			}
			else PounceTimer -= diff;

			if (FrozenBiteTimer <= diff)
			{
				DoCast(me, SPELL_FROZEN_BITE);
				FrozenBiteTimer = urand(8*IN_MILLISECONDS, 9*IN_MILLISECONDS);
			}
			else FrozenBiteTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_skollAI(creature);
    }
};

class npc_time_lost_proto_drake : public CreatureScript
{
public:
    npc_time_lost_proto_drake() : CreatureScript("npc_time_lost_proto_drake") {}

    struct npc_time_lost_proto_drakeAI : public QuantumBasicAI
    {
        npc_time_lost_proto_drakeAI(Creature* creature) : QuantumBasicAI(creature)
		{
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SNARE, true);
			me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
		}

		uint32 TimeLapseTimer;
		uint32 TimeShiftTimer;

        void Reset()
		{
			TimeLapseTimer = 0.5*IN_MILLISECONDS;
			TimeShiftTimer = 1*IN_MILLISECONDS;

			DoCast(me, SPELL_HOVER_ANIM);

			me->SetUInt32Value(UNIT_FIELD_BYTES_1, FIELD_BYTE_1_FLYING);
			me->SetUInt32Value(UNIT_FIELD_BYTES_2, FIELD_BYTE_2_MELEE_WEAPON);
		}

        void UpdateAI(uint32 const diff)
        {
            if (!UpdateVictim())
                return;

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;

			if (TimeLapseTimer <= diff)
			{
				if (Unit* target = SelectTarget(TARGET_RANDOM, 0))
				{
					DoCast(target, SPELL_TIME_LAPSE);
					TimeLapseTimer = 4*IN_MILLISECONDS;
				}
			}
			else TimeLapseTimer -= diff;

			if (TimeShiftTimer <= diff)
			{
				DoCastAOE(SPELL_TIME_SHIFT);
				TimeShiftTimer = 6*IN_MILLISECONDS;
			}
			else TimeShiftTimer -= diff;

			DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_time_lost_proto_drakeAI(creature);
    }
};

void AddSC_world_rare_elite_scripts()
{
	new npc_aotona();
	new npc_dirkee();
	new npc_griegen();
	new npc_high_thane_jorfus();
	new npc_icehorn();
	new npc_king_ping();
	new npc_old_crystalbark();
	new npc_putridus_the_ancient();
	new npc_seething_hate();
	new npc_terror_spinner();
	new npc_vigdis_the_war_maiden();
	new npc_zuldrak_sentinel();
	new npc_crazed_indule_survivor();
	new npc_fumblub_gearwind();
	new npc_grocklar();
	new npc_hildana_deathstealer();
	new npc_king_krush();
	new npc_loque_nahak();
	new npc_perobas_the_bloodthirster();
	new npc_scarlet_highlord_daion();
	new npc_syreian_the_bonecarver();
	new npc_tukemuth();
	new npc_vyragosa();
	new npc_gondria();
	new npc_arcturis();
	new npc_skoll();
	new npc_time_lost_proto_drake();
}